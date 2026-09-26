// ==WindhawkMod==
// @id              win2000-hung-app-dialog
// @name            Windows 2000 End Program Dialog
// @description     Replaces the Windows 11 "not responding" dialog with the End Program dialog of Windows 2000, in English or Russian
// @name:ru         Окно «Завершение программы» из Windows 2000
// @description:ru  Заменяет окно Windows 11 о зависшей программе окном «Завершение программы» из Windows 2000 - на русском или английском
// @version         1.3.0
// @author          appEW
// @github          https://github.com/appEW
// @include         explorer.exe
// @include         WerFault.exe
// @include         WerFaultSecure.exe
// @compilerOptions -ldwmapi -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 2000 End Program Dialog

When a program stops responding and you try to close it, Windows 11 shows its
Windows Error Reporting dialog ("... is not responding" with *Close the
program* and *Wait for the program to respond*). This mod shows the End Program
dialog of Windows 2000 in its place, before the Windows 11 one ever appears:

> **End Program - Notepad**
>
> This program is not responding.
>
> To return to Windows and check the status of the program, click Cancel.
>
> If you choose to end the program immediately, you will lose any unsaved
> data. To end the program now, click End Now.
>
> [ End Now ] [ Cancel ]

The dialog is not a copy: it is the original `DIALOG #10` of `winsrv.dll.mui`,
which Windows 11 24H2 still ships, together with the messages of the Windows
2000 dialog, so its wording, layout and font are exactly those of Windows 2000.
Like the original, it shows the icon of the hung program with the Windows 2000
warning sign over it, and has no icon in its title bar.

The dialog is shown in the Windows display language when Windows has it in
that language, and in English otherwise; the *Language of the dialog* setting
can force English or Russian. The Windows 11 dialog is recognised in English
and in Russian.

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

## How it works

The Explorer instance quietly pre-warms the on-demand Windows Error Reporting
service and asks Windhawk to scan its service host, so Windhawk can inject the
`WerFault.exe` child before its entry point. An early hook then cloaks the WER
TaskDialog during `TDN_DIALOG_CONSTRUCTED`, before Windows displays it. A
synchronous in-process accessibility hook covers the short interval after
construction as well. A dedicated broker thread validates the WER window, its
DWM ghost owner and its visual tree, and then displays `DIALOG #10`.
Enumeration of already created windows remains as a fallback for late
injection.

The original WER TaskDialog stays alive. The Windows 2000 buttons forward the
original WER button IDs with `TDM_CLICK_BUTTON`, so the operating system's own
end/wait logic runs exactly as before. The stock dialog is brought back if the
mod is unloaded, forwarding fails, or WER does not react in time.

The watcher is limited to the current `WerFault.exe` process. Running beside
the WER window also avoids the cross-integrity UIPI restrictions on
`TDM_CLICK_BUTTON`.

---

## По-русски

Когда программа перестаёт отвечать и вы пытаетесь её закрыть, Windows 11
показывает окно отчётов об ошибках («... не отвечает» с кнопками «Закрыть
программу» и «Ожидание отклика программы»). Мод показывает вместо него окно
«Завершение программы» из Windows 2000 - ещё до того, как окно Windows 11
появится на экране:

> **Завершение программы - Блокнот**
>
> Эта программа не отвечает.
>
> Чтобы вернуться в Windows и проверить состояние приложения, нажмите кнопку
> "Отмена".
>
> Если закрыть программу прямо сейчас, то можно потерять все несохраненные
> данные. Чтобы завершить программу сейчас, нажмите кнопку "Завершить сейчас".
>
> [ Завершить сейчас ] [ Отмена ]

Это не копия, а оригинальный `DIALOG #10` из `winsrv.dll.mui`, который до сих
пор есть в Windows 11 24H2, вместе с сообщениями окна Windows 2000, поэтому
текст, расположение и шрифт в точности как в Windows 2000. Как и в оригинале,
в окне показан значок зависшей программы со знаком предупреждения Windows 2000,
а в заголовке значка нет.

Окно показывается на языке интерфейса Windows, если в Windows есть его версия
на этом языке, иначе - на английском; параметр «Язык окна» позволяет выбрать
английский или русский принудительно. Окно Windows 11 распознаётся на
английском и на русском.

Кнопки окна Windows 2000 нажимают соответствующие кнопки исходного окна
Windows 11, поэтому завершение или ожидание программы выполняет сама Windows.
Если что-то пойдёт не так, возвращается обычное окно Windows 11.

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language of the dialog
  $name:ru: Язык окна
  $description: >-
    The dialog is taken from the winsrv.dll.mui of this language. Automatic
    uses the Windows display language, and English when that one has no
    such file.
  $description:ru: >-
    Окно берётся из winsrv.dll.mui этого языка. «Автоматически» - язык
    интерфейса Windows, а если для него такого файла нет - английский.
  $options:
  - auto: Automatic
  - en-US: English
  - ru-RU: Russian
  $options:ru:
  - auto: Автоматически
  - en-US: Английский
  - ru-RU: Русский
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <dwmapi.h>
#include <evntprov.h>
#include <windows.h>

#include <atomic>
#include <cwctype>
#include <map>
#include <memory>
#include <new>
#include <string>
#include <vector>

namespace {

constexpr UINT kDialogResourceId = 10;
constexpr int kStatusControlId = 257;
constexpr int kEndNowButtonId = 259;
constexpr int kIconControlId = 0x7FFE;
constexpr UINT_PTR kActionTimerId = 1;
constexpr UINT_PTR kDiscoveryTimerId = 2;
constexpr UINT_PTR kEarlyCloakTimerId = 0x5748324B;
constexpr UINT kActionPollMs = 250;
constexpr UINT kDiscoveryPollMs = 100;
constexpr UINT kEarlyCloakFallbackMs = 1500;
constexpr ULONGLONG kDiscoveryLifetimeMs = 10000;
constexpr ULONGLONG kActionTimeoutMs = 15000;
constexpr DWORD kTextTimeoutMs = 100;
constexpr DWORD kDwmwaCloak = 13;
constexpr int kWerCloseButtonId = 10;
constexpr int kWerWaitButtonId = 8;

constexpr UINT kBrokerCandidateMessage = WM_APP + 1;
constexpr UINT kBrokerStockDestroyedMessage = WM_APP + 2;
constexpr UINT kBrokerFinalizeMessage = WM_APP + 3;
constexpr UINT kBrokerEarlyCloakFailOpenMessage = WM_APP + 4;

constexpr wchar_t kBrokerWindowClass[] =
    L"Win2000HungAppDialogBrokerWindow";
constexpr wchar_t kEarlyCloakProperty[] =
    L"Win2000HungAppDialogRu.EarlyCloak";
constexpr wchar_t kClassicDialogProperty[] =
    L"Win2000HungAppDialogRu.ClassicDialog";
constexpr wchar_t kWindhawkScanEvent[] =
    L"Global\\WindhawkScanForProcesses";
constexpr GUID kWerSvcTriggerProvider = {
    0xE46EEAD8,
    0x0C54,
    0x4489,
    {0x98, 0x98, 0x8F, 0xA7, 0x9D, 0x05, 0x9E, 0x0E},
};
// winsrv.dll.mui keeps the Windows 2000 texts of this dialog in its message
// table. They are read from the same MUI file as the dialog, so the status
// line always matches the language of the dialog itself; the English texts
// below are only used if a message cannot be read.
constexpr DWORD kNotRespondingMessageId = 19;
constexpr DWORD kEndingMessageId = 20;
constexpr wchar_t kDefaultNotResponding[] = L"This program is not responding.";
constexpr wchar_t kDefaultEnding[] = L"Ending Program...Please wait";
constexpr wchar_t kDefaultAppName[] = L"Program";
constexpr wchar_t kRussianAppName[] = L"Программа";

// The original 16x16 four-bit Windows 2000 warning overlay. The bytes are
// the RT_ICON payload, so the mod stays a single self-contained source file.
constexpr BYTE kWin2000WarningIcon16[] = {
    0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00,
    0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0x00,
    0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00,
    0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00,
    0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x88, 0x88, 0x88,
    0x88, 0x88, 0x88, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88,
    0x3B, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0x08, 0x3B, 0xBB, 0xBB, 0x80,
    0x8B, 0xBB, 0xBB, 0x08, 0x3B, 0xBB, 0xBB, 0x80, 0x8B, 0xBB, 0xBB, 0x00,
    0x03, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xB0, 0x80, 0x03, 0xBB, 0xBB, 0xB0,
    0xBB, 0xBB, 0xB0, 0x00, 0x00, 0x3B, 0xBB, 0x30, 0x3B, 0xBB, 0x08, 0x00,
    0x00, 0x3B, 0xBB, 0x00, 0x0B, 0xBB, 0x00, 0x00, 0x00, 0x03, 0xBB, 0x00,
    0x0B, 0xB0, 0x80, 0x00, 0x00, 0x03, 0xBB, 0x00, 0x0B, 0xB0, 0x00, 0x00,
    0x00, 0x00, 0x3B, 0xB0, 0xBB, 0x08, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xBB,
    0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xBB, 0xB0, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x03, 0xBB, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,
    0x30, 0x00, 0x00, 0x00, 0xC0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x80, 0x01, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00,
    0xC0, 0x07, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0xE0, 0x0F, 0x00, 0x00,
    0xF0, 0x0F, 0x00, 0x00, 0xF0, 0x1F, 0x00, 0x00, 0xF8, 0x1F, 0x00, 0x00,
    0xF8, 0x3F, 0x00, 0x00, 0xFC, 0x7F, 0x00, 0x00,
};

struct HangButtons {
    int closeId = kWerCloseButtonId;
    int waitId = kWerWaitButtonId;
};

using WerUIpTaskDialogIndirect_t = HRESULT(WINAPI*)(
    const TASKDIALOGCONFIG* config);

struct EarlyTaskDialogCallbackState {
    PFTASKDIALOGCALLBACK originalCallback = nullptr;
    LONG_PTR originalCallbackData = 0;
};

enum class HostMode {
    None,
    ExplorerPrewarm,
    WerFault,
};

struct Session {
    HWND stockDialog = nullptr;
    HWND classicDialog = nullptr;
    HWND ghostOwner = nullptr;
    HWND hungWindow = nullptr;
    DWORD stockProcessId = 0;
    DWORD stockThreadId = 0;
    HangButtons buttons;
    std::wstring applicationName;
    std::wstring caption;
    HICON applicationIcon = nullptr;
    HICON warningIcon = nullptr;
    bool stockWasVisible = false;
    bool stockCloakedByUs = false;
    bool stockHiddenByUs = false;
    bool selectionSent = false;
    bool closing = false;
    ULONGLONG actionDeadline = 0;
};

HMODULE g_resourceModule = nullptr;
std::wstring g_notResponding = kDefaultNotResponding;
std::wstring g_ending = kDefaultEnding;
std::wstring g_fallbackAppName = kDefaultAppName;
HMODULE g_selfModule = nullptr;
WerUIpTaskDialogIndirect_t g_werUIpTaskDialogIndirectOriginal = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_readyEvent = nullptr;
HANDLE g_brokerThread = nullptr;
std::atomic<HWND> g_brokerWindow{nullptr};
std::atomic<bool> g_brokerInitialized{false};
std::atomic<bool> g_active{false};
HostMode g_hostMode = HostMode::None;

// The following state is accessed only on the broker thread.
HWINEVENTHOOK g_winEventHook = nullptr;
std::map<HWND, std::unique_ptr<Session>> g_sessions;
HINSTANCE g_windowClassInstance = nullptr;
bool g_windowClassRegistered = false;
ULONGLONG g_discoveryDeadline = 0;

std::wstring Lowercase(std::wstring value) {
    if (!value.empty()) {
        CharLowerBuffW(value.data(), static_cast<DWORD>(value.size()));
    }
    return value;
}

bool ContainsInsensitive(const std::wstring& value, const wchar_t* needle) {
    return Lowercase(value).find(Lowercase(needle)) != std::wstring::npos;
}

void Trim(std::wstring& value) {
    constexpr wchar_t kWhitespace[] = L" \t\r\n";
    const size_t first = value.find_first_not_of(kWhitespace);
    if (first == std::wstring::npos) {
        value.clear();
        return;
    }
    const size_t last = value.find_last_not_of(kWhitespace);
    value = value.substr(first, last - first + 1);
}

std::wstring ResolveTaskDialogText(HINSTANCE instance, PCWSTR text) {
    if (!text) {
        return {};
    }
    if (!IS_INTRESOURCE(text)) {
        return text;
    }

    wchar_t buffer[2048] = {};
    const int length =
        LoadStringW(instance, LOWORD(reinterpret_cast<ULONG_PTR>(text)),
                    buffer, ARRAYSIZE(buffer));
    return length > 0 ? std::wstring(buffer, length) : std::wstring{};
}

bool IsWerHangPromptConfig(const TASKDIALOGCONFIG* config) {
    if (!config || config->cbSize != sizeof(TASKDIALOGCONFIG) ||
        config->cButtons < 2 || config->cButtons > 3 ||
        !config->pButtons) {
        return false;
    }

    const std::wstring instruction =
        ResolveTaskDialogText(config->hInstance, config->pszMainInstruction);
    if (!ContainsInsensitive(instruction, L"не отвечает") &&
        !ContainsInsensitive(instruction, L"not responding")) {
        return false;
    }

    bool hasClose = false;
    bool hasWait = false;
    for (UINT index = 0; index < config->cButtons; ++index) {
        const TASKDIALOG_BUTTON& button = config->pButtons[index];
        const std::wstring text =
            ResolveTaskDialogText(config->hInstance, button.pszButtonText);
        if (button.nButtonID == kWerWaitButtonId &&
            (ContainsInsensitive(text, L"ожидание отклика программы") ||
             ContainsInsensitive(text, L"wait for the program"))) {
            hasWait = true;
        } else if (button.nButtonID == kWerCloseButtonId &&
                   (ContainsInsensitive(text, L"закрыть программу") ||
                    ContainsInsensitive(text, L"close the program") ||
                    ContainsInsensitive(text, L"end task"))) {
            hasClose = true;
        } else if (button.nButtonID != 4) {
            return false;
        }
    }
    return hasClose && hasWait;
}

std::wstring WindowTextWithTimeout(HWND window) {
    if (!window || !IsWindow(window)) {
        return {};
    }

    wchar_t text[2048] = {};
    DWORD_PTR ignored = 0;
    if (!SendMessageTimeoutW(window, WM_GETTEXT, ARRAYSIZE(text),
                             reinterpret_cast<LPARAM>(text),
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, kTextTimeoutMs,
                             &ignored)) {
        return {};
    }
    return text;
}

std::wstring ProcessPath(DWORD processId) {
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return {};
    }

    wchar_t path[32768] = {};
    DWORD length = ARRAYSIZE(path);
    const BOOL ok = QueryFullProcessImageNameW(process, 0, path, &length);
    CloseHandle(process);
    return ok ? std::wstring(path, length) : std::wstring{};
}

std::wstring CurrentProcessBaseName() {
    wchar_t path[32768] = {};
    const DWORD length =
        GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) {
        return {};
    }

    const wchar_t* baseName = path;
    for (const wchar_t* cursor = path; *cursor; ++cursor) {
        if (*cursor == L'\\' || *cursor == L'/') {
            baseName = cursor + 1;
        }
    }
    return Lowercase(baseName);
}

std::wstring JoinPath(const wchar_t* directory, const wchar_t* fileName) {
    std::wstring path = directory;
    if (!path.empty() && path.back() != L'\\') {
        path += L'\\';
    }
    path += fileName;
    return path;
}

bool EqualPath(const std::wstring& left, const std::wstring& right) {
    return !left.empty() && !right.empty() &&
           _wcsicmp(left.c_str(), right.c_str()) == 0;
}

bool IsSystemWerProcess(DWORD processId) {
    const std::wstring processPath = ProcessPath(processId);
    if (processPath.empty()) {
        return false;
    }

    wchar_t systemDirectory[MAX_PATH] = {};
    if (GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory))) {
        if (EqualPath(processPath,
                      JoinPath(systemDirectory, L"WerFault.exe")) ||
            EqualPath(processPath,
                      JoinPath(systemDirectory, L"WerFaultSecure.exe"))) {
            return true;
        }
    }

    wchar_t wow64Directory[MAX_PATH] = {};
    if (GetSystemWow64DirectoryW(wow64Directory,
                                 ARRAYSIZE(wow64Directory))) {
        if (EqualPath(processPath,
                      JoinPath(wow64Directory, L"WerFault.exe")) ||
            EqualPath(processPath,
                      JoinPath(wow64Directory, L"WerFaultSecure.exe"))) {
            return true;
        }
    }

    return false;
}

struct WerTreeSearchContext {
    int visibleButtonCount = 0;
    bool hasDirectUiWindow = false;
};

BOOL CALLBACK InspectWerTreeCallback(HWND window, LPARAM parameter) {
    auto* context = reinterpret_cast<WerTreeSearchContext*>(parameter);
    wchar_t className[64] = {};
    if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (_wcsicmp(className, L"DirectUIHWND") == 0) {
        context->hasDirectUiWindow = true;
    } else if (_wcsicmp(className, L"Button") == 0 &&
               IsWindowVisible(window)) {
        ++context->visibleButtonCount;
    }
    return TRUE;
}

bool HasInitialWerConsentTree(HWND taskDialog) {
    WerTreeSearchContext context;
    EnumChildWindows(taskDialog, InspectWerTreeCallback,
                     reinterpret_cast<LPARAM>(&context));
    return context.hasDirectUiWindow && context.visibleButtonCount == 2;
}

using HungWindowFromGhostWindow_t = HWND(WINAPI*)(HWND);

HWND ResolveHungWindow(HWND ghostWindow) {
    static const auto function = reinterpret_cast<HungWindowFromGhostWindow_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"),
                       "HungWindowFromGhostWindow"));
    return function ? function(ghostWindow) : nullptr;
}

bool IsPotentialWerHangDialog(HWND window) {
    if (!window || !IsWindow(window) ||
        GetAncestor(window, GA_ROOT) != window ||
        GetPropW(window, kClassicDialogProperty)) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId != GetCurrentProcessId()) {
        return false;
    }

    wchar_t className[64] = {};
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"#32770") != 0) {
        return false;
    }

    const HWND owner = GetWindow(window, GW_OWNER);
    wchar_t ownerClass[64] = {};
    return owner && GetClassNameW(owner, ownerClass, ARRAYSIZE(ownerClass)) &&
           wcscmp(ownerClass, L"Ghost") == 0 &&
           ResolveHungWindow(owner) != nullptr;
}

void CALLBACK EarlyCloakFallbackTimerProc(HWND window, UINT, UINT_PTR timerId,
                                          DWORD) {
    KillTimer(window, timerId);
    const HWND broker = g_brokerWindow.load();
    DWORD_PTR ignored = 0;
    if (g_active.load() && broker &&
        SendMessageTimeoutW(
            broker, kBrokerEarlyCloakFailOpenMessage,
            reinterpret_cast<WPARAM>(window), 0,
            SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT, 250,
            &ignored)) {
        return;
    }

    // With no broker there cannot be an active replacement session. Fail open
    // immediately instead of leaving a stock WER window permanently cloaked.
    if (reinterpret_cast<ULONG_PTR>(
            GetPropW(window, kEarlyCloakProperty)) == 1) {
        BOOL cloak = FALSE;
        DwmSetWindowAttribute(
            window, static_cast<DWMWINDOWATTRIBUTE>(kDwmwaCloak), &cloak,
            sizeof(cloak));
        RemovePropW(window, kEarlyCloakProperty);
    }
}

bool EarlyCloakStockDialog(HWND window) {
    if (!IsPotentialWerHangDialog(window)) {
        return false;
    }

    const ULONG_PTR existing =
        reinterpret_cast<ULONG_PTR>(GetPropW(window, kEarlyCloakProperty));
    if (existing == 2) {
        return true;
    }

    BOOL cloak = TRUE;
    if (FAILED(DwmSetWindowAttribute(
            window, static_cast<DWMWINDOWATTRIBUTE>(kDwmwaCloak), &cloak,
            sizeof(cloak)))) {
        return false;
    }

    if (!SetPropW(window, kEarlyCloakProperty,
                  reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(1))) ||
        !SetTimer(window, kEarlyCloakTimerId, kEarlyCloakFallbackMs,
                  EarlyCloakFallbackTimerProc)) {
        KillTimer(window, kEarlyCloakTimerId);
        RemovePropW(window, kEarlyCloakProperty);
        cloak = FALSE;
        DwmSetWindowAttribute(
            window, static_cast<DWMWINDOWATTRIBUTE>(kDwmwaCloak), &cloak,
            sizeof(cloak));
        return false;
    }
    return true;
}

void QueueEarlyWerCandidate(HWND taskDialog) {
    if (!g_active.load()) {
        return;
    }
    if (!EarlyCloakStockDialog(taskDialog)) {
        return;
    }

    const HWND broker = g_brokerWindow.load();
    if (broker) {
        PostMessageW(broker, kBrokerCandidateMessage,
                     reinterpret_cast<WPARAM>(taskDialog), 0);
    }
}

HRESULT CALLBACK EarlyTaskDialogCallback(HWND taskDialog, UINT notification,
                                         WPARAM wParam, LPARAM lParam,
                                         LONG_PTR callbackData) {
    auto* state = reinterpret_cast<EarlyTaskDialogCallbackState*>(callbackData);
    if (notification == TDN_DIALOG_CONSTRUCTED ||
        notification == TDN_CREATED || notification == TDN_NAVIGATED) {
        QueueEarlyWerCandidate(taskDialog);
    }

    if (state && state->originalCallback) {
        return state->originalCallback(taskDialog, notification, wParam,
                                       lParam,
                                       state->originalCallbackData);
    }
    return S_OK;
}

HRESULT WINAPI WerUIpTaskDialogIndirectHook(
    const TASKDIALOGCONFIG* config) {
    bool isHangPrompt = false;
    try {
        isHangPrompt = IsWerHangPromptConfig(config);
    } catch (...) {
        Wh_Log(L"Early WER hook failed; using stock TaskDialog path");
        return g_werUIpTaskDialogIndirectOriginal(config);
    }

    if (!isHangPrompt) {
        return g_werUIpTaskDialogIndirectOriginal(config);
    }

    TASKDIALOGCONFIG proxyConfig = *config;
    EarlyTaskDialogCallbackState state = {
        config->pfCallback,
        config->lpCallbackData,
    };
    proxyConfig.pfCallback = EarlyTaskDialogCallback;
    proxyConfig.lpCallbackData = reinterpret_cast<LONG_PTR>(&state);
    Wh_Log(L"Using pre-display WER TaskDialog cloak");
    return g_werUIpTaskDialogIndirectOriginal(&proxyConfig);
}

struct InstructionSearchContext {
    std::wstring applicationName;
};

void StripNotRespondingSuffix(std::wstring& value) {
    constexpr const wchar_t* suffixes[] = {
        L" не отвечает",
        L" is not responding",
        L" not responding",
    };
    const std::wstring lowerValue = Lowercase(value);
    for (const wchar_t* suffix : suffixes) {
        const std::wstring lowerSuffix = Lowercase(suffix);
        if (lowerValue.size() >= lowerSuffix.size() &&
            lowerValue.compare(lowerValue.size() - lowerSuffix.size(),
                               lowerSuffix.size(), lowerSuffix) == 0) {
            value.resize(value.size() - lowerSuffix.size());
            break;
        }
    }
    Trim(value);
}

BOOL CALLBACK FindInstructionCallback(HWND window, LPARAM parameter) {
    auto* context = reinterpret_cast<InstructionSearchContext*>(parameter);
    const std::wstring text = WindowTextWithTimeout(window);
    if (!ContainsInsensitive(text, L" не отвечает") &&
        !ContainsInsensitive(text, L" is not responding")) {
        return TRUE;
    }

    context->applicationName = text;
    const size_t lineBreak = context->applicationName.find_first_of(L"\r\n");
    if (lineBreak != std::wstring::npos) {
        context->applicationName.resize(lineBreak);
    }
    StripNotRespondingSuffix(context->applicationName);
    return context->applicationName.empty();
}

std::wstring GetApplicationName(HWND taskDialog) {
    std::wstring name = WindowTextWithTimeout(taskDialog);
    StripNotRespondingSuffix(name);

    if (name.empty() || ContainsInsensitive(name, L"microsoft windows") ||
        ContainsInsensitive(name, L"отчеты об ошибках windows") ||
        ContainsInsensitive(name, L"windows error reporting")) {
        InstructionSearchContext context;
        EnumChildWindows(taskDialog, FindInstructionCallback,
                         reinterpret_cast<LPARAM>(&context));
        if (!context.applicationName.empty()) {
            name = std::move(context.applicationName);
        }
    }

    return name.empty() ? g_fallbackAppName : name;
}

HICON CopyWindowIcon(HWND window) {
    if (!window || !IsWindow(window)) {
        return nullptr;
    }

    DWORD_PTR result = 0;
    constexpr WPARAM kIconTypes[] = {ICON_BIG, ICON_SMALL2, ICON_SMALL};
    for (WPARAM iconType : kIconTypes) {
        if (SendMessageTimeoutW(window, WM_GETICON, iconType, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK,
                                kTextTimeoutMs, &result) &&
            result) {
            return CopyIcon(reinterpret_cast<HICON>(result));
        }
    }

    HICON icon =
        reinterpret_cast<HICON>(GetClassLongPtrW(window, GCLP_HICON));
    if (!icon) {
        icon = reinterpret_cast<HICON>(
            GetClassLongPtrW(window, GCLP_HICONSM));
    }
    return icon ? CopyIcon(icon) : nullptr;
}

HICON GetApplicationIcon(HWND hungWindow, HWND taskDialog) {
    if (HICON icon = CopyWindowIcon(hungWindow)) {
        return icon;
    }
    if (HICON icon = CopyWindowIcon(taskDialog)) {
        return icon;
    }
    HICON shared = LoadIconW(nullptr, IDI_APPLICATION);
    return shared ? CopyIcon(shared) : nullptr;
}

HICON LoadWindows2000WarningIcon() {
    return CreateIconFromResourceEx(
        const_cast<PBYTE>(kWin2000WarningIcon16),
        sizeof(kWin2000WarningIcon16), TRUE, 0x00030000, 16, 16,
        LR_DEFAULTCOLOR);
}

void CenterDialog(HWND dialog, HWND referenceWindow) {
    RECT dialogRect = {};
    if (!GetWindowRect(dialog, &dialogRect)) {
        return;
    }

    MONITORINFO monitorInfo = {sizeof(monitorInfo)};
    HMONITOR monitor = MonitorFromWindow(
        referenceWindow ? referenceWindow : dialog, MONITOR_DEFAULTTONEAREST);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return;
    }

    const int width = dialogRect.right - dialogRect.left;
    const int height = dialogRect.bottom - dialogRect.top;
    const RECT& work = monitorInfo.rcWork;
    const int x = work.left + ((work.right - work.left) - width) / 2;
    const int y = work.top + ((work.bottom - work.top) - height) / 2;
    SetWindowPos(dialog, HWND_TOP, x, y, 0, 0,
                 SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

void AddApplicationIcon(HWND dialog, Session* session) {
    if (!session->applicationIcon) {
        return;
    }

    // DIALOG #10 deliberately has no icon in its caption. Windows 2000 drew
    // the hung application's 32x32 icon in the body and overlaid a 16x16
    // warning badge in the lower-left corner.
    RECT iconOrigin = {8, 8, 8, 8};
    if (!MapDialogRect(dialog, &iconOrigin)) {
        return;
    }

    UINT dpi = GetDpiForWindow(dialog);
    if (!dpi) {
        dpi = USER_DEFAULT_SCREEN_DPI;
    }
    int iconWidth = GetSystemMetricsForDpi(SM_CXICON, dpi);
    int iconHeight = GetSystemMetricsForDpi(SM_CYICON, dpi);
    if (iconWidth <= 0) {
        iconWidth = MulDiv(32, dpi, USER_DEFAULT_SCREEN_DPI);
    }
    if (iconHeight <= 0) {
        iconHeight = MulDiv(32, dpi, USER_DEFAULT_SCREEN_DPI);
    }

    CreateWindowExW(
        0, L"STATIC", nullptr, WS_CHILD | WS_VISIBLE | SS_OWNERDRAW,
        iconOrigin.left, iconOrigin.top, iconWidth, iconHeight, dialog,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(kIconControlId)),
        nullptr, nullptr);
}

bool IsSameStockWindow(const Session* session) {
    if (!session || !session->stockDialog ||
        !IsWindow(session->stockDialog)) {
        return false;
    }

    DWORD processId = 0;
    const DWORD threadId =
        GetWindowThreadProcessId(session->stockDialog, &processId);
    if (processId != session->stockProcessId ||
        threadId != session->stockThreadId) {
        return false;
    }

    wchar_t className[64] = {};
    if (!GetClassNameW(session->stockDialog, className,
                       ARRAYSIZE(className)) ||
        wcscmp(className, L"#32770") != 0 ||
        GetWindow(session->stockDialog, GW_OWNER) != session->ghostOwner) {
        return false;
    }

    return IsWindow(session->ghostOwner) &&
           ResolveHungWindow(session->ghostOwner) == session->hungWindow;
}

void HideStockDialog(Session* session) {
    if (!session || !IsSameStockWindow(session)) {
        return;
    }

    session->stockWasVisible = IsWindowVisible(session->stockDialog);
    BOOL cloak = TRUE;
    if (SUCCEEDED(DwmSetWindowAttribute(
            session->stockDialog,
            static_cast<DWMWINDOWATTRIBUTE>(kDwmwaCloak), &cloak,
            sizeof(cloak)))) {
        session->stockCloakedByUs = true;
        return;
    }

    if (session->stockWasVisible &&
        ShowWindowAsync(session->stockDialog, SW_HIDE)) {
        session->stockHiddenByUs = true;
    }
}

void RestoreStockDialog(Session* session) {
    if (!session || !IsSameStockWindow(session)) {
        return;
    }

    if (session->stockCloakedByUs) {
        BOOL cloak = FALSE;
        DwmSetWindowAttribute(
            session->stockDialog,
            static_cast<DWMWINDOWATTRIBUTE>(kDwmwaCloak), &cloak,
            sizeof(cloak));
        session->stockCloakedByUs = false;
    }
    if (session->stockHiddenByUs && session->stockWasVisible) {
        ShowWindowAsync(session->stockDialog, SW_SHOW);
        session->stockHiddenByUs = false;
    }
    SetForegroundWindow(session->stockDialog);
}

void RequestFinalize(Session* session, bool restoreStock) {
    const HWND broker = g_brokerWindow.load();
    if (session && broker) {
        PostMessageW(broker, kBrokerFinalizeMessage,
                     reinterpret_cast<WPARAM>(session->stockDialog),
                     restoreStock);
    }
}

void SendSelection(Session* session, int buttonId, bool hideClassicDialog) {
    if (!session || session->selectionSent ||
        !IsSameStockWindow(session)) {
        return;
    }

    session->selectionSent = true;
    EnableWindow(GetDlgItem(session->classicDialog, kEndNowButtonId), FALSE);
    EnableWindow(GetDlgItem(session->classicDialog, IDCANCEL), FALSE);

    if (hideClassicDialog) {
        ShowWindow(session->classicDialog, SW_HIDE);
    } else {
        SetDlgItemTextW(session->classicDialog, kStatusControlId,
                        g_ending.c_str());
        UpdateWindow(session->classicDialog);
    }

    session->actionDeadline = GetTickCount64() + kActionTimeoutMs;
    if (!SetTimer(session->classicDialog, kActionTimerId, kActionPollMs,
                  nullptr)) {
        Wh_Log(L"Failed to create the WER action timer, GLE=%u",
               GetLastError());
        session->selectionSent = false;
        EnableWindow(GetDlgItem(session->classicDialog, kEndNowButtonId), TRUE);
        EnableWindow(GetDlgItem(session->classicDialog, IDCANCEL), TRUE);
        SetDlgItemTextW(session->classicDialog, kStatusControlId,
                        g_notResponding.c_str());
        ShowWindow(session->classicDialog, SW_SHOW);
        SetForegroundWindow(session->classicDialog);
        return;
    }

    if (!PostMessageW(session->stockDialog, TDM_CLICK_BUTTON, buttonId, 0)) {
        Wh_Log(L"TDM_CLICK_BUTTON failed for hwnd=%p, GLE=%u",
               session->stockDialog, GetLastError());
        RequestFinalize(session, true);
    }
}

INT_PTR CALLBACK ClassicDialogProc(HWND dialog, UINT message, WPARAM wParam,
                                   LPARAM lParam) {
    auto* session = reinterpret_cast<Session*>(
        GetWindowLongPtrW(dialog, DWLP_USER));

    switch (message) {
        case WM_INITDIALOG:
            session = reinterpret_cast<Session*>(lParam);
            SetWindowLongPtrW(dialog, DWLP_USER,
                              reinterpret_cast<LONG_PTR>(session));
            if (!session) {
                return FALSE;
            }
            SetPropW(dialog, kClassicDialogProperty,
                     reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(1)));
            {
                BOOL cloak = FALSE;
                DwmSetWindowAttribute(
                    dialog, static_cast<DWMWINDOWATTRIBUTE>(kDwmwaCloak),
                    &cloak, sizeof(cloak));
            }
            session->classicDialog = dialog;
            {
                // The caption of DIALOG #10 is the "End Program - " prefix
                // in the language of the MUI file it was loaded from.
                wchar_t prefix[256] = {};
                GetWindowTextW(dialog, prefix, ARRAYSIZE(prefix));
                session->caption = prefix;
                session->caption += session->applicationName;
            }
            SetWindowTextW(dialog, session->caption.c_str());
            SetDlgItemTextW(dialog, kStatusControlId,
                            g_notResponding.c_str());
            AddApplicationIcon(dialog, session);
            CenterDialog(dialog, session->ghostOwner);
            SendMessageW(dialog, DM_SETDEFID, IDCANCEL, 0);
            SetFocus(GetDlgItem(dialog, IDCANCEL));
            return FALSE;

        case WM_COMMAND:
            if (!session) {
                break;
            }
            if (LOWORD(wParam) == kEndNowButtonId) {
                SendSelection(session, session->buttons.closeId, false);
                return TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL) {
                SendSelection(session, session->buttons.waitId, true);
                return TRUE;
            }
            break;

        case WM_DRAWITEM:
            if (session && wParam == kIconControlId && lParam) {
                auto* draw = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
                if (draw->CtlType != ODT_STATIC) {
                    break;
                }
                FillRect(draw->hDC, &draw->rcItem,
                         GetSysColorBrush(COLOR_3DFACE));
                const int iconWidth =
                    draw->rcItem.right - draw->rcItem.left;
                const int iconHeight =
                    draw->rcItem.bottom - draw->rcItem.top;
                if (session->applicationIcon) {
                    DrawIconEx(draw->hDC, draw->rcItem.left,
                               draw->rcItem.top, session->applicationIcon,
                               iconWidth, iconHeight, 0, nullptr, DI_NORMAL);
                }
                if (session->warningIcon) {
                    UINT dpi = GetDpiForWindow(dialog);
                    if (!dpi) {
                        dpi = USER_DEFAULT_SCREEN_DPI;
                    }
                    int badgeWidth = GetSystemMetricsForDpi(SM_CXSMICON, dpi);
                    int badgeHeight = GetSystemMetricsForDpi(SM_CYSMICON, dpi);
                    if (badgeWidth <= 0) {
                        badgeWidth =
                            MulDiv(16, dpi, USER_DEFAULT_SCREEN_DPI);
                    }
                    if (badgeHeight <= 0) {
                        badgeHeight =
                            MulDiv(16, dpi, USER_DEFAULT_SCREEN_DPI);
                    }
                    badgeWidth = badgeWidth < iconWidth ? badgeWidth : iconWidth;
                    badgeHeight =
                        badgeHeight < iconHeight ? badgeHeight : iconHeight;
                    DrawIconEx(draw->hDC, draw->rcItem.left,
                               draw->rcItem.bottom - badgeHeight,
                               session->warningIcon, badgeWidth, badgeHeight,
                               0, nullptr, DI_NORMAL);
                }
                return TRUE;
            }
            break;

        case WM_CLOSE:
            if (session) {
                SendSelection(session, session->buttons.waitId, true);
            }
            return TRUE;

        case WM_TIMER:
            if (wParam == kActionTimerId && session &&
                session->selectionSent) {
                if (!IsSameStockWindow(session)) {
                    KillTimer(dialog, kActionTimerId);
                    RequestFinalize(session, false);
                    return TRUE;
                }

                if (GetTickCount64() >= session->actionDeadline) {
                    KillTimer(dialog, kActionTimerId);
                    Wh_Log(L"WER action timed out for hwnd=%p; restoring stock UI",
                           session->stockDialog);
                    RequestFinalize(session, true);
                    return TRUE;
                }
            }
            break;

        case WM_NCDESTROY:
            if (session) {
                RemovePropW(dialog, kClassicDialogProperty);
                session->classicDialog = nullptr;
                if (!session->closing) {
                    RequestFinalize(session, !session->selectionSent);
                }
            }
            break;
    }

    return FALSE;
}

void CloseSession(HWND stockDialog, bool restoreStock) {
    const auto iterator = g_sessions.find(stockDialog);
    if (iterator == g_sessions.end()) {
        return;
    }

    std::unique_ptr<Session> session = std::move(iterator->second);
    g_sessions.erase(iterator);
    session->closing = true;

    if (session->stockDialog && IsWindow(session->stockDialog)) {
        KillTimer(session->stockDialog, kEarlyCloakTimerId);
        RemovePropW(session->stockDialog, kEarlyCloakProperty);
    }

    if (restoreStock) {
        RestoreStockDialog(session.get());
    }
    if (session->classicDialog && IsWindow(session->classicDialog)) {
        KillTimer(session->classicDialog, kActionTimerId);
        DestroyWindow(session->classicDialog);
        session->classicDialog = nullptr;
    }
    if (session->warningIcon) {
        DestroyIcon(session->warningIcon);
        session->warningIcon = nullptr;
    }
    if (session->applicationIcon) {
        DestroyIcon(session->applicationIcon);
        session->applicationIcon = nullptr;
    }
}

bool IsWerHangTaskDialog(HWND window, DWORD* processId, HWND* ghostOwner,
                         HWND* hungWindow) {
    if (!window || !IsWindow(window) ||
        GetAncestor(window, GA_ROOT) != window ||
        !IsWindowVisible(window)) {
        return false;
    }

    wchar_t className[64] = {};
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"#32770") != 0) {
        return false;
    }

    DWORD candidateProcessId = 0;
    GetWindowThreadProcessId(window, &candidateProcessId);
    if (candidateProcessId != GetCurrentProcessId() ||
        !IsSystemWerProcess(candidateProcessId)) {
        return false;
    }

    const HWND candidateGhostOwner = GetWindow(window, GW_OWNER);
    wchar_t ownerClassName[64] = {};
    if (!candidateGhostOwner ||
        !GetClassNameW(candidateGhostOwner, ownerClassName,
                       ARRAYSIZE(ownerClassName)) ||
        _wcsicmp(ownerClassName, L"Ghost") != 0) {
        return false;
    }

    const HWND candidateHungWindow = ResolveHungWindow(candidateGhostOwner);
    if (!candidateHungWindow || !IsWindow(candidateHungWindow)) {
        return false;
    }

    if (!HasInitialWerConsentTree(window)) {
        return false;
    }

    *processId = candidateProcessId;
    *ghostOwner = candidateGhostOwner;
    *hungWindow = candidateHungWindow;
    return true;
}

void StartSession(HWND stockDialog) {
    const auto existingSession = g_sessions.find(stockDialog);
    if (existingSession != g_sessions.end()) {
        // WER can show the same TaskDialog again while changing state. Keep
        // it cloaked for as long as the Windows 2000 replacement is active.
        HideStockDialog(existingSession->second.get());
        return;
    }

    DWORD processId = 0;
    HWND ghostOwner = nullptr;
    HWND hungWindow = nullptr;
    if (!IsWerHangTaskDialog(stockDialog, &processId, &ghostOwner,
                             &hungWindow)) {
        return;
    }

    auto session = std::unique_ptr<Session>(new (std::nothrow) Session);
    if (!session) {
        return;
    }

    session->stockDialog = stockDialog;
    session->stockProcessId = processId;
    session->stockThreadId =
        GetWindowThreadProcessId(stockDialog, nullptr);
    session->ghostOwner = ghostOwner;
    session->hungWindow = hungWindow;
    session->applicationName = GetApplicationName(stockDialog);
    session->applicationIcon =
        GetApplicationIcon(session->hungWindow, stockDialog);
    session->warningIcon = LoadWindows2000WarningIcon();

    Session* sessionPointer = session.get();
    HWND classicDialog = CreateDialogParamW(
        g_resourceModule, MAKEINTRESOURCEW(kDialogResourceId),
        session->ghostOwner, ClassicDialogProc,
        reinterpret_cast<LPARAM>(sessionPointer));
    if (!classicDialog) {
        Wh_Log(L"CreateDialogParamW(DIALOG #10) failed, GLE=%u",
               GetLastError());
        if (session->warningIcon) {
            DestroyIcon(session->warningIcon);
        }
        if (session->applicationIcon) {
            DestroyIcon(session->applicationIcon);
        }
        return;
    }

    session->classicDialog = classicDialog;
    SetPropW(stockDialog, kEarlyCloakProperty,
             reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(2)));
    KillTimer(stockDialog, kEarlyCloakTimerId);
    g_sessions.emplace(stockDialog, std::move(session));

    HideStockDialog(sessionPointer);
    ShowWindow(classicDialog, SW_SHOW);
    SetForegroundWindow(classicDialog);
    SetActiveWindow(classicDialog);

    Wh_Log(L"Late-attached to WER hwnd=%p pid=%u app=%s close=%d wait=%d",
           stockDialog, processId, sessionPointer->applicationName.c_str(),
           sessionPointer->buttons.closeId,
           sessionPointer->buttons.waitId);
}

BOOL CALLBACK EnumerateExistingWindowsCallback(HWND window, LPARAM) {
    StartSession(window);
    return TRUE;
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND window,
                           LONG objectId, LONG childId, DWORD, DWORD) {
    if (!g_active.load() || !window || objectId != OBJID_WINDOW ||
        childId != CHILDID_SELF) {
        return;
    }

    const HWND broker = g_brokerWindow.load();
    if (!broker) {
        return;
    }

    if (event == EVENT_OBJECT_SHOW) {
        // WINEVENT_INCONTEXT runs on the UI thread. Cloak the strongly
        // fingerprinted stock dialog before queueing the slower DirectUI
        // validation to the broker, so it never reaches a compositor frame.
        const HWND rootWindow = GetAncestor(window, GA_ROOT);
        EarlyCloakStockDialog(rootWindow ? rootWindow : window);
        PostMessageW(broker, kBrokerCandidateMessage,
                     reinterpret_cast<WPARAM>(rootWindow ? rootWindow : window),
                     0);
    } else if (event == EVENT_OBJECT_DESTROY) {
        PostMessageW(broker, kBrokerStockDestroyedMessage,
                     reinterpret_cast<WPARAM>(window), 0);
    }
}

LRESULT CALLBACK BrokerWindowProc(HWND window, UINT message, WPARAM wParam,
                                  LPARAM lParam) {
    switch (message) {
        case WM_TIMER:
            if (wParam == kDiscoveryTimerId) {
                EnumWindows(EnumerateExistingWindowsCallback, 0);
                if (!g_sessions.empty() ||
                    GetTickCount64() >= g_discoveryDeadline) {
                    KillTimer(window, kDiscoveryTimerId);
                }
                return 0;
            }
            break;

        case kBrokerCandidateMessage:
            StartSession(reinterpret_cast<HWND>(wParam));
            return 0;

        case kBrokerStockDestroyedMessage:
            CloseSession(reinterpret_cast<HWND>(wParam), false);
            return 0;

        case kBrokerFinalizeMessage:
            CloseSession(reinterpret_cast<HWND>(wParam), lParam != 0);
            return 0;

        case kBrokerEarlyCloakFailOpenMessage: {
            const HWND stockDialog = reinterpret_cast<HWND>(wParam);
            if (g_sessions.find(stockDialog) == g_sessions.end() &&
                reinterpret_cast<ULONG_PTR>(
                    GetPropW(stockDialog, kEarlyCloakProperty)) == 1) {
                BOOL cloak = FALSE;
                DwmSetWindowAttribute(
                    stockDialog,
                    static_cast<DWMWINDOWATTRIBUTE>(kDwmwaCloak), &cloak,
                    sizeof(cloak));
                RemovePropW(stockDialog, kEarlyCloakProperty);
            }
            return 0;
        }
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

bool RouteDialogMessage(MSG* message) {
    for (const auto& [stockDialog, session] : g_sessions) {
        (void)stockDialog;
        const HWND dialog = session->classicDialog;
        if (dialog && IsWindow(dialog) &&
            (message->hwnd == dialog || IsChild(dialog, message->hwnd)) &&
            IsDialogMessageW(dialog, message)) {
            return true;
        }
    }
    return false;
}

void CloseAllSessions(bool restoreStock) {
    while (!g_sessions.empty()) {
        CloseSession(g_sessions.begin()->first, restoreStock);
    }
}

bool TriggerWerService() {
    REGHANDLE provider = 0;
    if (EventRegister(&kWerSvcTriggerProvider, nullptr, nullptr, &provider) !=
        ERROR_SUCCESS) {
        return false;
    }

    EVENT_DESCRIPTOR event = {};
    event.Id = 0;
    event.Version = 0;
    event.Channel = 16;
    event.Level = 4;  // TRACE_LEVEL_INFORMATION
    event.Opcode = 0;
    event.Task = 1;
    event.Keyword = 0x8000000000000001ULL;
    const ULONG result = EventWrite(provider, &event, 0, nullptr);
    EventUnregister(provider);
    return result == ERROR_SUCCESS;
}

void SignalWindhawkProcessScan() {
    HANDLE event =
        OpenEventW(EVENT_MODIFY_STATE, FALSE, kWindhawkScanEvent);
    if (event) {
        SetEvent(event);
        CloseHandle(event);
    }
}

bool HasWindhawkCustomizationSession(DWORD processId) {
    wchar_t name[128] = {};
    _snwprintf_s(name, ARRAYSIZE(name), _TRUNCATE,
                 L"Global\\WindhawkCustomizationSessionSemaphore-pid=%lu",
                 processId);
    HANDLE semaphore = OpenSemaphoreW(SYNCHRONIZE, FALSE, name);
    if (semaphore) {
        CloseHandle(semaphore);
        return true;
    }
    return GetLastError() == ERROR_ACCESS_DENIED;
}

bool QueryWerService(SC_HANDLE service, SERVICE_STATUS_PROCESS* status) {
    DWORD bytesNeeded = 0;
    return QueryServiceStatusEx(
               service, SC_STATUS_PROCESS_INFO,
               reinterpret_cast<LPBYTE>(status), sizeof(*status),
               &bytesNeeded) != FALSE;
}

DWORD WINAPI WerServicePrewarmThreadProc(void*) {
    bool isShellProcess = false;
    for (int attempt = 0; attempt < 100; ++attempt) {
        const HWND shellWindow = GetShellWindow();
        if (shellWindow) {
            DWORD shellProcessId = 0;
            GetWindowThreadProcessId(shellWindow, &shellProcessId);
            if (shellProcessId != GetCurrentProcessId()) {
                return 0;
            }
            isShellProcess = true;
            break;
        }
        if (WaitForSingleObject(g_stopEvent, 100) != WAIT_TIMEOUT) {
            return 0;
        }
    }
    if (!isShellProcess) {
        return 3;
    }

    SC_HANDLE manager =
        OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!manager) {
        return 1;
    }
    SC_HANDLE service =
        OpenServiceW(manager, L"WerSvc", SERVICE_QUERY_STATUS);
    if (!service) {
        CloseServiceHandle(manager);
        return 2;
    }

    DWORD injectedProcessId = 0;
    ULONGLONG nextTriggerAttempt = 0;
    ULONGLONG nextHeartbeat = 0;
    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        SERVICE_STATUS_PROCESS status = {};
        if (!QueryWerService(service, &status)) {
            break;
        }

        if (status.dwCurrentState != SERVICE_RUNNING ||
            !status.dwProcessId) {
            injectedProcessId = 0;
            nextHeartbeat = 0;
            const ULONGLONG now = GetTickCount64();
            if (now >= nextTriggerAttempt) {
                TriggerWerService();
                nextTriggerAttempt = now + 5000;
            }
        } else {
            const ULONGLONG now = GetTickCount64();
            if (now >= nextHeartbeat) {
                TriggerWerService();
                nextHeartbeat = now + 60000;
            }

            if (status.dwProcessId != injectedProcessId ||
                !HasWindhawkCustomizationSession(status.dwProcessId)) {
                bool injected = false;
                SignalWindhawkProcessScan();
                for (int attempt = 0; attempt < 60; ++attempt) {
                    if (HasWindhawkCustomizationSession(status.dwProcessId)) {
                        injected = true;
                        break;
                    }
                    if (WaitForSingleObject(g_stopEvent, 50) != WAIT_TIMEOUT) {
                        break;
                    }
                }
                if (injected) {
                    // The semaphore is published immediately before the
                    // injected session finishes installing CreateProcess
                    // hooks. Give that short initialization tail time to
                    // complete before treating the service host as ready.
                    if (WaitForSingleObject(g_stopEvent, 100) != WAIT_TIMEOUT) {
                        break;
                    }
                    injectedProcessId = status.dwProcessId;
                    Wh_Log(L"Pre-warmed WerSvc host pid=%u",
                           injectedProcessId);
                }
            }
        }

        // Poll quickly only while the service host is being started and
        // injected. Once ready, a five-second health check is sufficient;
        // the 60-second heartbeat is well inside WerSvc's idle timeout.
        const DWORD pollIntervalMs = injectedProcessId ? 5000 : 250;
        if (WaitForSingleObject(g_stopEvent, pollIntervalMs) != WAIT_TIMEOUT) {
            break;
        }
    }

    CloseServiceHandle(service);
    CloseServiceHandle(manager);
    return 0;
}

DWORD WINAPI BrokerThreadProc(void*) {
    g_windowClassInstance = GetModuleHandleW(nullptr);
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = BrokerWindowProc;
    windowClass.hInstance = g_windowClassInstance;
    windowClass.lpszClassName = kBrokerWindowClass;
    if (!RegisterClassExW(&windowClass)) {
        SetEvent(g_readyEvent);
        return 1;
    }
    g_windowClassRegistered = true;

    HWND broker = CreateWindowExW(
        0, kBrokerWindowClass, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE,
        nullptr, g_windowClassInstance, nullptr);
    if (!broker) {
        UnregisterClassW(kBrokerWindowClass, g_windowClassInstance);
        g_windowClassRegistered = false;
        SetEvent(g_readyEvent);
        return 2;
    }
    g_brokerWindow.store(broker);

    g_winEventHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY, EVENT_OBJECT_SHOW, g_selfModule, WinEventProc,
        GetCurrentProcessId(), 0, WINEVENT_INCONTEXT);
    if (!g_winEventHook) {
        g_brokerWindow.store(nullptr);
        DestroyWindow(broker);
        UnregisterClassW(kBrokerWindowClass, g_windowClassInstance);
        g_windowClassRegistered = false;
        SetEvent(g_readyEvent);
        return 3;
    }

    g_brokerInitialized.store(true);
    SetEvent(g_readyEvent);

    // Installing the hook before enumeration closes the show/enumerate race.
    EnumWindows(EnumerateExistingWindowsCallback, 0);
    g_discoveryDeadline = GetTickCount64() + kDiscoveryLifetimeMs;
    SetTimer(broker, kDiscoveryTimerId, kDiscoveryPollMs, nullptr);

    bool stop = false;
    while (!stop) {
        const DWORD waitResult = MsgWaitForMultipleObjects(
            1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }
        if (waitResult != WAIT_OBJECT_0 + 1) {
            break;
        }

        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                stop = true;
                break;
            }
            if (RouteDialogMessage(&message)) {
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    UnhookWinEvent(g_winEventHook);
    g_winEventHook = nullptr;
    KillTimer(broker, kDiscoveryTimerId);
    CloseAllSessions(true);

    g_brokerWindow.store(nullptr);
    DestroyWindow(broker);
    if (g_windowClassRegistered) {
        UnregisterClassW(kBrokerWindowClass, g_windowClassInstance);
        g_windowClassRegistered = false;
    }
    return 0;
}

bool PinCurrentModuleUntilProcessExit() {
    HMODULE self = nullptr;
    const auto address = reinterpret_cast<LPCWSTR>(
        reinterpret_cast<ULONG_PTR>(&PinCurrentModuleUntilProcessExit));
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_PIN,
                           address, &self)) {
        Wh_Log(L"GetModuleHandleExW(PIN) failed, GLE=%u", GetLastError());
        return false;
    }
    g_selfModule = self;
    return true;
}

bool InstallEarlyWerHook() {
    HMODULE werui = GetModuleHandleW(L"werui.dll");
    if (!werui) {
        werui = LoadLibraryExW(L"werui.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!werui) {
        Wh_Log(L"werui.dll is unavailable, GLE=%u", GetLastError());
        return false;
    }

    FARPROC function = GetProcAddress(werui, "WerUIpTaskDialogIndirect");
    if (!function) {
        Wh_Log(L"WerUIpTaskDialogIndirect is unavailable, GLE=%u",
               GetLastError());
        return false;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(function),
            reinterpret_cast<void*>(WerUIpTaskDialogIndirectHook),
            reinterpret_cast<void**>(
                &g_werUIpTaskDialogIndirectOriginal))) {
        Wh_Log(L"Failed to hook WerUIpTaskDialogIndirect");
        return false;
    }

    Wh_Log(L"Pre-display WER hook queued");
    return true;
}

std::wstring LoadResourceMessage(DWORD messageId) {
    wchar_t buffer[512] = {};
    const DWORD length = FormatMessageW(
        FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS |
            FORMAT_MESSAGE_MAX_WIDTH_MASK,
        g_resourceModule, messageId, 0, buffer, ARRAYSIZE(buffer), nullptr);
    std::wstring message(buffer, length);
    Trim(message);
    return message;
}

// The languages to take the dialog from, in order: the one chosen in the
// settings, else the Windows display language, then English.
std::vector<std::wstring> DialogLanguages() {
    std::vector<std::wstring> languages;

    PCWSTR setting = Wh_GetStringSetting(L"language");
    if (setting && *setting && wcscmp(setting, L"auto") != 0) {
        languages.emplace_back(setting);
    }
    Wh_FreeStringSetting(setting);

    wchar_t userLanguage[LOCALE_NAME_MAX_LENGTH] = {};
    if (LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT),
                         userLanguage, ARRAYSIZE(userLanguage), 0)) {
        languages.emplace_back(userLanguage);
    }

    languages.emplace_back(L"en-US");
    return languages;
}

bool LoadWindows2000DialogResource() {
    wchar_t systemDirectory[MAX_PATH] = {};
    const UINT length =
        GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
    if (!length || length >= ARRAYSIZE(systemDirectory)) {
        return false;
    }

    for (const std::wstring& language : DialogLanguages()) {
        const std::wstring path = JoinPath(
            systemDirectory, (language + L"\\winsrv.dll.mui").c_str());
        HMODULE module = LoadLibraryExW(
            path.c_str(), nullptr,
            LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
        if (!module) {
            Wh_Log(L"LoadLibraryExW(%s) failed, GLE=%u", path.c_str(),
                   GetLastError());
            continue;
        }
        if (!FindResourceW(module, MAKEINTRESOURCEW(kDialogResourceId),
                           RT_DIALOG)) {
            Wh_Log(L"DIALOG #10 is missing in %s", path.c_str());
            FreeLibrary(module);
            continue;
        }

        g_resourceModule = module;
        std::wstring message = LoadResourceMessage(kNotRespondingMessageId);
        g_notResponding = message.empty() ? kDefaultNotResponding : message;
        message = LoadResourceMessage(kEndingMessageId);
        g_ending = message.empty() ? kDefaultEnding : message;
        g_fallbackAppName =
            _wcsnicmp(language.c_str(), L"ru", 2) == 0 ? kRussianAppName
                                                       : kDefaultAppName;
        Wh_Log(L"Using DIALOG #10 from %s", path.c_str());
        return true;
    }
    return false;
}

void CleanupInitializationObjects() {
    if (g_brokerThread) {
        CloseHandle(g_brokerThread);
        g_brokerThread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_resourceModule) {
        FreeLibrary(g_resourceModule);
        g_resourceModule = nullptr;
    }
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    const std::wstring processName = CurrentProcessBaseName();
    if (processName == L"explorer.exe") {
        g_hostMode = HostMode::ExplorerPrewarm;
        g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_stopEvent) {
            CleanupInitializationObjects();
            g_hostMode = HostMode::None;
            return FALSE;
        }

        g_brokerThread = CreateThread(
            nullptr, 0, WerServicePrewarmThreadProc, nullptr, 0, nullptr);
        if (!g_brokerThread) {
            CleanupInitializationObjects();
            g_hostMode = HostMode::None;
            return FALSE;
        }

        Wh_Log(L"WerSvc pre-warm worker started");
        return TRUE;
    }

    if (processName != L"werfault.exe" &&
        processName != L"werfaultsecure.exe") {
        return FALSE;
    }
    g_hostMode = HostMode::WerFault;
    g_active.store(true);

    if (!PinCurrentModuleUntilProcessExit()) {
        g_active.store(false);
        g_hostMode = HostMode::None;
        return FALSE;
    }

    // Queue this first and return promptly below. Windhawk commits hook
    // operations automatically immediately after Wh_ModInit returns.
    InstallEarlyWerHook();

    if (!LoadWindows2000DialogResource()) {
        g_active.store(false);
        g_hostMode = HostMode::None;
        return FALSE;
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_readyEvent) {
        CleanupInitializationObjects();
        g_active.store(false);
        g_hostMode = HostMode::None;
        return FALSE;
    }

    g_brokerThread =
        CreateThread(nullptr, 0, BrokerThreadProc, nullptr, 0, nullptr);
    if (!g_brokerThread) {
        CleanupInitializationObjects();
        g_active.store(false);
        g_hostMode = HostMode::None;
        return FALSE;
    }

    // Don't wait here: returning promptly lets Windhawk commit the early hook
    // before WerFault reaches its TaskDialog call. The broker signals its own
    // readiness and the early-cloak timer fails open if initialization fails.
    Wh_Log(L"WER broker started asynchronously");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_active.store(false);

    if (g_brokerThread) {
        SetEvent(g_stopEvent);
        const HWND broker = g_brokerWindow.load();
        if (broker) {
            PostMessageW(broker, WM_NULL, 0, 0);
        }

        // The module must not unload while the broker's WinEvent callback or
        // dialog procedures can still execute. The broker never waits on the
        // calling thread, so this join has no lock inversion.
        WaitForSingleObject(g_brokerThread, INFINITE);
    }

    CleanupInitializationObjects();
    g_brokerInitialized.store(false);
    g_hostMode = HostMode::None;
}
