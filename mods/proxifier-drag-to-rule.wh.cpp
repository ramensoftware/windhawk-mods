// ==WindhawkMod==
// @id              proxifier-drag-to-rule
// @name            Proxifier drag and drop to rule
// @description     Open a new Proxification Rule prefilled with dropped applications.
// @version         0.1
// @author          zxhzxhz
// @github          https://github.com/zxhzxhz
// @include         Proxifier.exe
// @architecture    x86
// @compilerOptions -lcomctl32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Proxifier drag and drop to rule

Drop executable files or folders on the Proxifier main window. The mod opens
the Add Proxification Rule dialog and fills its Applications field, but never
saves a rule automatically.

The defaults were verified against Proxifier 4.11. Restart Proxifier after
enabling or updating the mod so its main window can be subclassed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable drag and drop
  $description: Accept executable files and folders dropped on the main window.
- folderMode: 0
  $name: Folder handling mode
  $description: 0 adds folder\\*.exe to the rule. 1 ignores dropped folders.
- rulesCommand: 32798
  $name: Proxification Rules command ID
  $description: Proxifier 4.11 uses 32798 (0x801E).
- rulesAddButtonId: 1001
  $name: Add button control ID
  $description: Proxifier 4.11 uses 1001.
- applicationsControlId: 1018
  $name: Applications control ID
  $description: Proxifier 4.11 uses 1018.
- nameControlId: 1017
  $name: Name control ID
  $description: Proxifier 4.11 uses 1017.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>

#include <string>
#include <vector>

namespace {

constexpr wchar_t kProxifierMainClass[] = L"Proxifier32Cls";
constexpr wchar_t kRulesDialogTitle[] = L"Proxification Rules";
constexpr wchar_t kRuleDialogTitle[] = L"Proxification Rule";
constexpr UINT_PTR kSubclassId = 1;
constexpr UINT_PTR kAutomationTimerId = 0x5052;
constexpr UINT kAutomationTimerIntervalMs = 50;

struct Settings {
    bool enabled;
    int folderMode;
    UINT rulesCommand;
    int rulesAddButtonId;
    int applicationsControlId;
    int nameControlId;
} settings;

enum class AutomationStage {
    kIdle,
    kOpeningRules,
    kOpeningRule,
};

HWND g_mainWindow = nullptr;
DWORD g_mainThreadId = 0;
AutomationStage g_automationStage = AutomationStage::kIdle;
std::wstring g_pendingApplications;
std::wstring g_pendingRuleName;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

bool EndsWithExtension(const std::wstring& path, const wchar_t* extension) {
    const size_t extensionLength = wcslen(extension);
    if (path.size() < extensionLength) {
        return false;
    }

    return _wcsicmp(path.c_str() + path.size() - extensionLength, extension) ==
           0;
}

std::wstring LeafName(const std::wstring& path) {
    const size_t separator = path.find_last_of(L"\\/");
    return separator == std::wstring::npos ? path : path.substr(separator + 1);
}

std::wstring QuoteApplicationPath(const std::wstring& path) {
    if (path.find_first_of(L" ;") == std::wstring::npos) {
        return path;
    }

    return L"\"" + path + L"\"";
}

struct DropSelection {
    std::wstring applications;
    std::wstring ruleName;
    bool containsProfile;
};

DropSelection BuildDropSelection(HDROP drop) {
    const UINT fileCount = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
    std::vector<std::wstring> applications;
    std::wstring ruleName;

    for (UINT i = 0; i < fileCount; ++i) {
        const UINT length = DragQueryFileW(drop, i, nullptr, 0);
        if (length == 0) {
            continue;
        }

        std::wstring path(length + 1, L'\0');
        DragQueryFileW(drop, i, path.data(), length + 1);
        path.resize(length);

        // A .ppx profile has built-in Proxifier drop behavior. Do not consume
        // a mixed drop either, so the original handler sees the full payload.
        if (EndsWithExtension(path, L".ppx")) {
            return {{}, {}, true};
        }

        const DWORD attributes = GetFileAttributesW(path.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"Ignoring unavailable drop target: %s", path.c_str());
            continue;
        }

        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (settings.folderMode == 0) {
                std::wstring folderName = LeafName(path);
                if (!path.empty() && path.back() != L'\\') {
                    path += L'\\';
                }
                applications.push_back(QuoteApplicationPath(path + L"*.exe"));
                if (ruleName.empty()) {
                    ruleName = std::move(folderName);
                }
            } else {
                Wh_Log(L"Ignoring dropped folder: %s", path.c_str());
            }
        } else if (EndsWithExtension(path, L".exe")) {
            applications.push_back(QuoteApplicationPath(path));
            if (ruleName.empty()) {
                ruleName = LeafName(path);
            }
        } else {
            Wh_Log(L"Ignoring non-executable drop target: %s", path.c_str());
        }
    }

    std::wstring result;
    for (const std::wstring& application : applications) {
        if (!result.empty()) {
            result += L"; ";
        }
        result += application;
    }

    return {std::move(result), std::move(ruleName), false};
}

struct FindDialogContext {
    const wchar_t* title;
    HWND result;
};

BOOL CALLBACK FindDialogForThread(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<FindDialogContext*>(lParam);
    wchar_t title[128]{};
    GetWindowTextW(hwnd, title, _countof(title));

    if (wcscmp(title, context->title) == 0) {
        context->result = hwnd;
        return FALSE;
    }

    return TRUE;
}

HWND FindDialogForMainThread(const wchar_t* title) {
    if (g_mainThreadId == 0) {
        return nullptr;
    }

    FindDialogContext context{title, nullptr};
    EnumThreadWindows(g_mainThreadId, FindDialogForThread,
                      reinterpret_cast<LPARAM>(&context));
    return context.result;
}

void StopAutomation() {
    if (g_mainWindow) {
        KillTimer(g_mainWindow, kAutomationTimerId);
    }
    g_automationStage = AutomationStage::kIdle;
    g_pendingApplications.clear();
    g_pendingRuleName.clear();
}

void AdvanceAutomation();

void StartRuleCreation(const DropSelection& selection) {
    if (g_automationStage != AutomationStage::kIdle) {
        Wh_Log(L"Ignoring drop while a rule dialog is being opened");
        return;
    }

    if (FindDialogForMainThread(kRuleDialogTitle)) {
        Wh_Log(L"Ignoring drop because a Proxification Rule dialog is already open");
        return;
    }

    g_pendingApplications = selection.applications;
    g_pendingRuleName = selection.ruleName;
    if (FindDialogForMainThread(kRulesDialogTitle)) {
        g_automationStage = AutomationStage::kOpeningRules;
        SetTimer(g_mainWindow, kAutomationTimerId, kAutomationTimerIntervalMs,
                 nullptr);
        AdvanceAutomation();
        return;
    }

    g_automationStage = AutomationStage::kOpeningRules;
    SetTimer(g_mainWindow, kAutomationTimerId, kAutomationTimerIntervalMs,
             nullptr);

    Wh_Log(L"Opening Proxification Rules for: %s", g_pendingApplications.c_str());
    PostMessageW(g_mainWindow, WM_COMMAND,
                 MAKEWPARAM(settings.rulesCommand, 0), 0);
}

void AdvanceAutomation() {
    if (g_automationStage == AutomationStage::kOpeningRules) {
        HWND rulesDialog = FindDialogForMainThread(kRulesDialogTitle);
        if (!rulesDialog) {
            return;
        }

        HWND addButton = GetDlgItem(rulesDialog, settings.rulesAddButtonId);
        if (!addButton) {
            Wh_Log(L"Rules dialog found, but Add button ID %d was not found",
                   settings.rulesAddButtonId);
            StopAutomation();
            return;
        }

        Wh_Log(L"Clicking Add in Proxification Rules");
        g_automationStage = AutomationStage::kOpeningRule;
        PostMessageW(addButton, BM_CLICK, 0, 0);
        return;
    }

    if (g_automationStage == AutomationStage::kOpeningRule) {
        HWND ruleDialog = FindDialogForMainThread(kRuleDialogTitle);
        if (!ruleDialog) {
            return;
        }

        HWND applicationsControl =
            GetDlgItem(ruleDialog, settings.applicationsControlId);
        if (!applicationsControl) {
            Wh_Log(L"Rule dialog found, but Applications control ID %d was not found",
                   settings.applicationsControlId);
            StopAutomation();
            return;
        }

        SetWindowTextW(applicationsControl, g_pendingApplications.c_str());
        SendMessageW(ruleDialog, WM_COMMAND,
                     MAKEWPARAM(settings.applicationsControlId, EN_CHANGE),
                     reinterpret_cast<LPARAM>(applicationsControl));

        HWND nameControl = GetDlgItem(ruleDialog, settings.nameControlId);
        if (nameControl) {
            SetWindowTextW(nameControl, g_pendingRuleName.c_str());
            SendMessageW(ruleDialog, WM_COMMAND,
                         MAKEWPARAM(settings.nameControlId, EN_CHANGE),
                         reinterpret_cast<LPARAM>(nameControl));
        } else {
            Wh_Log(L"Rule dialog found, but Name control ID %d was not found",
                   settings.nameControlId);
        }

        Wh_Log(L"Prefilled rule name: %s; applications: %s",
               g_pendingRuleName.c_str(), g_pendingApplications.c_str());
        StopAutomation();
    }
}

LRESULT CALLBACK MainWindowSubclass(HWND window, UINT message, WPARAM wParam,
                                    LPARAM lParam, UINT_PTR, DWORD_PTR) {
    switch (message) {
        case WM_DROPFILES: {
            HDROP drop = reinterpret_cast<HDROP>(wParam);
            DropSelection selection = BuildDropSelection(drop);

            if (selection.containsProfile) {
                Wh_Log(L"Passing .ppx drop to Proxifier's default handler");
                return DefSubclassProc(window, message, wParam, lParam);
            }

            DragFinish(drop);

            if (!selection.applications.empty() && settings.enabled) {
                StartRuleCreation(selection);
            } else if (selection.applications.empty()) {
                Wh_Log(L"Drop did not contain an executable or accepted folder");
            }
            return 0;
        }

        case WM_TIMER:
            if (wParam == kAutomationTimerId) {
                AdvanceAutomation();
                return 0;
            }
            break;

        case WM_NCDESTROY:
            StopAutomation();
            DragAcceptFiles(window, FALSE);
            g_mainWindow = nullptr;
            g_mainThreadId = 0;
            break;
    }

    return DefSubclassProc(window, message, wParam, lParam);
}

void AttachToMainWindow(HWND hwnd) {
    if (g_mainWindow || !hwnd || !IsWindow(hwnd)) {
        return;
    }

    wchar_t className[64]{};
    GetClassNameW(hwnd, className, _countof(className));
    if (wcscmp(className, kProxifierMainClass) != 0) {
        return;
    }

    if (!SetWindowSubclass(hwnd, MainWindowSubclass, kSubclassId, 0)) {
        Wh_Log(L"SetWindowSubclass failed for Proxifier main window");
        return;
    }

    g_mainWindow = hwnd;
    g_mainThreadId = GetWindowThreadProcessId(hwnd, nullptr);
    DragAcceptFiles(hwnd, TRUE);
    Wh_Log(L"Attached drag-and-drop handler to Proxifier main window");
}

BOOL CALLBACK AttachExistingMainWindow(HWND hwnd, LPARAM) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == GetCurrentProcessId()) {
        AttachToMainWindow(hwnd);
    }
    return g_mainWindow == nullptr;
}

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className,
                                 LPCWSTR windowName, DWORD style, int x, int y,
                                 int width, int height, HWND parent, HMENU menu,
                                 HINSTANCE instance, LPVOID parameter) {
    HWND hwnd = CreateWindowExW_Original(exStyle, className, windowName, style,
                                         x, y, width, height, parent, menu,
                                         instance, parameter);

    if (hwnd && !parent && !(style & WS_CHILD)) {
        AttachToMainWindow(hwnd);
    }

    return hwnd;
}

void LoadSettings() {
    settings.enabled = Wh_GetIntSetting(L"enabled") != 0;
    settings.folderMode = Wh_GetIntSetting(L"folderMode");
    settings.rulesCommand = static_cast<UINT>(Wh_GetIntSetting(L"rulesCommand"));
    settings.rulesAddButtonId = Wh_GetIntSetting(L"rulesAddButtonId");
    settings.applicationsControlId = Wh_GetIntSetting(L"applicationsControlId");
    settings.nameControlId = Wh_GetIntSetting(L"nameControlId");
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"Initializing Proxifier drag-to-rule mod");

    Wh_SetFunctionHook(reinterpret_cast<void*>(CreateWindowExW),
                       reinterpret_cast<void*>(CreateWindowExW_Hook),
                       reinterpret_cast<void**>(&CreateWindowExW_Original));
    EnumWindows(AttachExistingMainWindow, 0);
    return TRUE;
}

void Wh_ModUninit() {
    StopAutomation();
    if (g_mainWindow && IsWindow(g_mainWindow)) {
        DragAcceptFiles(g_mainWindow, FALSE);
        RemoveWindowSubclass(g_mainWindow, MainWindowSubclass, kSubclassId);
    }
    g_mainWindow = nullptr;
    g_mainThreadId = 0;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
