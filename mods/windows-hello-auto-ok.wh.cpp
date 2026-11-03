// ==WindhawkMod==
// @id              windows-hello-auto-ok
// @name            Windows Hello Auto OK
// @description     Language-independent confirmation of ready Windows Hello biometric dialogs across applications.
// @version         0.1.2
// @author          hermanzamula
// @github          https://github.com/hermanzamula
// @include         windhawk.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luuid -ladvapi32 -lshell32 -luiautomationcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Hello Auto OK

Confirms the ready OK button in standard Windows Hello biometric dialogs,
regardless of the requesting application or interface language.

![Animated preview](https://raw.githubusercontent.com/hermanzamula/windhawk-windows-hello-auto-ok/main/docs/preview.gif)

*Illustrated demo using an English interface and the display name User.*

Detection uses AutomationId, control types, visibility and enabled state.
It never matches captions, translated messages, account names or websites.
Visible dialogs owned by the Windows CredentialUIBroker are monitored directly,
even when another application has focus. Visible credential input fields prevent
automatic confirmation.

The mod runs in an isolated Windhawk tool process. Windows requires elevated
access to credential UI, so enabling the mod may display a UAC consent prompt.
Declining that prompt leaves automatic confirmation inactive; toggle the mod
off and on to retry. No scheduled tasks or services are created by this mod.

This is an experimental release. Its readiness check describes the standard
Windows Hello UI structure, not an independent biometric-success API. Windows
still performs authentication. Custom application dialogs and future changes
to the system UI may require an update.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Enabled: true
  $name: Enable automatic confirmation
- DelayMs: 350
  $name: Confirmation delay in milliseconds
  $description: A stable ready state is required for 150 to 3000 milliseconds.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uiautomation.h>
#include <wrl/client.h>
#include <shellapi.h>
#include <algorithm>
#include <atomic>
#include <map>
#include <optional>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace HelloAutoOk {
struct Node {
    std::wstring automationId;
    CONTROLTYPEID type = 0;
    bool enabled = false;
    bool visible = true;
    bool password = false;
#ifndef HELLO_AUTO_OK_TEST
    ComPtr<IUIAutomationElement> element;
#endif
};

// Identifies the enabled system confirmation button without reading its caption.
bool IsConfirmation(const Node& node) {
    return node.visible && node.enabled && node.type == UIA_ButtonControlTypeId &&
           node.automationId == L"OkButton";
}

// Counts visible controls by automation ID and, when supplied, control type.
int CountControls(const std::vector<Node>& nodes, PCWSTR id, CONTROLTYPEID type = 0) {
    return static_cast<int>(std::count_if(nodes.begin(), nodes.end(), [&](const Node& node) {
        return node.visible && node.automationId == id && (!type || node.type == type);
    }));
}

// Rejects dialogs that are still accepting a PIN, password, or other text input.
bool HasCredentialInput(const std::vector<Node>& nodes) {
    return std::any_of(nodes.begin(), nodes.end(), [](const Node& node) {
        return node.visible && node.enabled &&
               (node.password || node.type == UIA_EditControlTypeId);
    });
}

// Accepts either the expanded credential list or its collapsed sign-in-options link.
bool HasCredentialSelector(const std::vector<Node>& nodes) {
    return CountControls(nodes, L"CredentialsListV2", UIA_ListControlTypeId) +
           CountControls(nodes, L"ChooseAnotherOption", UIA_HyperlinkControlTypeId) == 1;
}

// Requires the standard biometric layout and one unambiguous, enabled OK button.
bool IsReady(const std::vector<Node>& nodes) {
    return !HasCredentialInput(nodes) &&
           CountControls(nodes, L"OkButton") == 1 &&
           std::any_of(nodes.begin(), nodes.end(), IsConfirmation) &&
           CountControls(nodes, L"BioFeedbackText", UIA_TextControlTypeId) == 1 &&
           HasCredentialSelector(nodes);
}

struct DialogState {
    std::optional<ULONGLONG> readySince;
    bool handled = false;

    // Each dialog has its own timer, independent of focus and other open dialogs.
    bool ReadyLongEnough(ULONGLONG now, DWORD delay) {
        if (!readySince) {
            readySince = now;
            return false;
        }
        return now - *readySince >= delay;
    }

    // A complete non-ready state starts a new confirmation cycle for this window.
    void ResetReadiness() {
        readySince.reset();
        handled = false;
    }
};

#ifndef HELLO_AUTO_OK_TEST
constexpr DWORD kPollIntervalMs = 150;
constexpr DWORD kResultWaitMs = 900;
constexpr DWORD kShutdownWaitMs = 5000;
constexpr int kMaxDialogControls = 160;

HANDLE stopEvent = nullptr;
HANDLE worker = nullptr;
std::atomic<bool> enabled{true};
std::atomic<DWORD> delayMs{350};
std::wstring nativeBrokerPath;
std::wstring wowBrokerPath;

// Closes an owned handle and clears it so cleanup can be called more than once.
void CloseOwnedHandle(HANDLE& handle) {
    if (handle) CloseHandle(handle);
    handle = nullptr;
}

// Checks the current process token; elevation applies only to the tool host.
bool IsElevated() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return false;

    TOKEN_ELEVATION elevation{};
    DWORD size = 0;
    bool queried = GetTokenInformation(token, TokenElevation, &elevation,
                                      sizeof(elevation), &size);
    CloseOwnedHandle(token);
    return queried && elevation.TokenIsElevated;
}

// Stores a generic diagnostic state without recording dialog or account data.
void ReportState(PCWSTR state) {
    Wh_SetStringValue(L"runtime-state", state);
    Wh_Log(L"%s", state);
}

// Builds native and WOW64 paths without 32-bit system-directory redirection.
bool InitializeBrokerPaths() {
    WCHAR windows[MAX_PATH];
    UINT length = GetSystemWindowsDirectoryW(windows, ARRAYSIZE(windows));
    if (!length || length >= ARRAYSIZE(windows)) return false;

    nativeBrokerPath = std::wstring(windows) + L"\\System32\\CredentialUIBroker.exe";
    wowBrokerPath = std::wstring(windows) + L"\\SysWOW64\\CredentialUIBroker.exe";
    return true;
}

// Reads an executable path using query-only process access.
std::wstring GetProcessPath(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return {};

    WCHAR path[32768];
    DWORD length = ARRAYSIZE(path);
    bool queried = QueryFullProcessImageNameW(process, 0, path, &length);
    CloseOwnedHandle(process);
    return queried ? std::wstring(path, length) : std::wstring{};
}

// Accepts only visible windows owned by a Windows credential-broker executable.
bool IsBrokerWindow(HWND window) {
    if (!window || !IsWindowVisible(window)) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    std::wstring path = GetProcessPath(pid);
    return _wcsicmp(path.c_str(), nativeBrokerPath.c_str()) == 0 ||
           _wcsicmp(path.c_str(), wowBrokerPath.c_str()) == 0;
}

// Collects only visible credential-broker windows on the current desktop.
BOOL CALLBACK CollectBrokerWindow(HWND window, LPARAM context) {
    try {
        if (IsBrokerWindow(window))
            reinterpret_cast<std::vector<HWND>*>(context)->push_back(window);
        return TRUE;
    } catch (...) {
        return FALSE;
    }
}

// Enumerates Hello dialogs without consulting the requesting app or foreground window.
std::vector<HWND> GetBrokerWindows() {
    std::vector<HWND> windows;
    if (!EnumWindows(CollectBrokerWindow, reinterpret_cast<LPARAM>(&windows))) return {};
    return windows;
}

// Finds descendants within one dialog; a failed query yields an empty COM pointer.
ComPtr<IUIAutomationElementArray> GetDialogControls(IUIAutomation* automation, HWND window) {
    ComPtr<IUIAutomationElement> root;
    ComPtr<IUIAutomationCondition> condition;
    ComPtr<IUIAutomationElementArray> controls;
    if (FAILED(automation->ElementFromHandle(window, &root)) ||
        FAILED(automation->CreateTrueCondition(&condition)) ||
        FAILED(root->FindAll(TreeScope_Descendants, condition.Get(), &controls))) return {};
    return controls;
}

// Reads structural properties, leaving offscreen controls otherwise untouched.
bool ReadControlProperties(IUIAutomationElement* element, Node& node) {
    BOOL offscreen = TRUE;
    if (FAILED(element->get_CurrentIsOffscreen(&offscreen))) return false;
    node.visible = !offscreen;
    if (!node.visible) return true;

    BOOL active = FALSE, password = TRUE;
    if (FAILED(element->get_CurrentIsEnabled(&active)) ||
        FAILED(element->get_CurrentIsPassword(&password)) ||
        FAILED(element->get_CurrentControlType(&node.type))) return false;
    node.enabled = active;
    node.password = password;
    return true;
}

// Reads only the language-neutral ID, releasing the string allocated by UIA.
bool ReadAutomationId(IUIAutomationElement* element, std::wstring& result) {
    BSTR id = nullptr;
    HRESULT status = element->get_CurrentAutomationId(&id);
    if (SUCCEEDED(status) && id) result.assign(id, SysStringLen(id));
    SysFreeString(id);
    return SUCCEEDED(status);
}

// Captures a visible control without querying credential values or localized text.
bool ReadControl(IUIAutomationElement* element, Node& node) {
    if (!ReadControlProperties(element, node)) return false;
    if (!node.visible) return true;

    node.element = element;
    if (node.password || node.type == UIA_EditControlTypeId) return true;
    return ReadAutomationId(element, node.automationId);
}

// Returns a complete dialog snapshot, or no result if any control changes mid-query.
std::optional<std::vector<Node>> ReadDialog(IUIAutomation* automation, HWND window) {
    if (!IsBrokerWindow(window)) return std::nullopt;
    auto controls = GetDialogControls(automation, window);
    if (!controls) return std::nullopt;

    int count = 0;
    if (FAILED(controls->get_Length(&count)) || count > kMaxDialogControls) return std::nullopt;

    std::vector<Node> nodes;
    for (int i = 0; i < count; ++i) {
        ComPtr<IUIAutomationElement> element;
        Node node;
        if (FAILED(controls->GetElement(i, &element)) || !ReadControl(element.Get(), node))
            return std::nullopt;
        if (node.visible) nodes.push_back(std::move(node));
    }
    return nodes;
}

// Combines snapshot acquisition and the structural readiness check.
std::optional<std::vector<Node>> ReadReadyDialog(IUIAutomation* automation, HWND window) {
    auto nodes = ReadDialog(automation, window);
    if (!nodes || !IsReady(*nodes)) return std::nullopt;
    return nodes;
}

// Reloads the pause switch and bounds the readiness delay to supported values.
void LoadSettings() {
    enabled.store(Wh_GetIntSetting(L"Enabled") != 0);
    delayMs.store(static_cast<DWORD>(std::clamp(Wh_GetIntSetting(L"DelayMs"), 150, 3000)));
}

// Invokes the exact control while its broker dialog remains visible and allowed.
bool InvokeConfirmation(HWND window, IUIAutomationElement* button) {
    ComPtr<IUIAutomationInvokePattern> invoke;
    if (FAILED(button->GetCurrentPatternAs(UIA_InvokePatternId, IID_PPV_ARGS(&invoke)))) {
        ReportState(L"invoke-unavailable");
        return false;
    }
    if (!IsBrokerWindow(window) || !enabled.load()) return false;

    ReportState(L"invoking-confirmation");
    if (SUCCEEDED(invoke->Invoke())) return true;
    ReportState(L"invoke-failed");
    return false;
}

// Counts a confirmation only after its window closes; false requests worker shutdown.
bool RecordConfirmationResult(HWND window) {
    if (WaitForSingleObject(stopEvent, kResultWaitMs) != WAIT_TIMEOUT) return false;
    if (!IsWindow(window) || !IsWindowVisible(window)) {
        Wh_SetIntValue(L"confirmed-count", Wh_GetIntValue(L"confirmed-count", 0) + 1);
        ReportState(L"confirmed");
    } else {
        ReportState(L"confirmation-result-unknown");
    }
    return true;
}

class DialogMonitor {
public:
    // Checks every visible Hello dialog. False means shutdown was requested.
    bool Poll(IUIAutomation* automation) {
        auto windows = GetBrokerWindows();
        DiscardClosedDialogs(windows);
        if (!enabled.load()) {
            ResetCandidates();
            return true;
        }
        for (HWND window : windows) {
            if (!PollDialog(automation, window)) return false;
        }
        return true;
    }

    // Discards timing after a pause or exception without repeating handled confirmations.
    void ResetCandidates() {
        for (auto& [window, state] : dialogs_) state.readySince.reset();
    }

private:
    std::map<HWND, DialogState> dialogs_;

    // Forget hidden or destroyed dialogs so reused window handles get fresh state.
    void DiscardClosedDialogs(const std::vector<HWND>& windows) {
        std::erase_if(dialogs_, [&](const auto& entry) {
            return std::find(windows.begin(), windows.end(), entry.first) == windows.end();
        });
    }

    // Advances one dialog's timer, preserving its handled state through transient UIA errors.
    bool PollDialog(IUIAutomation* automation, HWND window) {
        auto& state = dialogs_[window];
        auto nodes = ReadDialog(automation, window);
        if (!nodes) {
            state.readySince.reset();
            return true;
        }
        if (!IsReady(*nodes)) {
            state.ResetReadiness();
            return true;
        }
        if (state.handled || !state.ReadyLongEnough(GetTickCount64(), delayMs.load())) return true;
        return ConfirmIfStillReady(automation, window, state);
    }

    // Rechecks the dialog itself before marking it handled and invoking its OK control.
    bool ConfirmIfStillReady(IUIAutomation* automation, HWND window, DialogState& state) {
        auto nodes = GetCurrentReadyDialog(automation, window);
        if (!nodes) {
            state.readySince.reset();
            return true;
        }

        auto button = std::find_if(nodes->begin(), nodes->end(), IsConfirmation);
        state.handled = true;
        state.readySince.reset();
        if (!InvokeConfirmation(window, button->element.Get())) return true;
        return RecordConfirmationResult(window);
    }

    // Obtains the final ready snapshot unless automatic confirmation has been paused.
    std::optional<std::vector<Node>> GetCurrentReadyDialog(IUIAutomation* automation, HWND window) {
        if (!enabled.load()) return std::nullopt;
        return ReadReadyDialog(automation, window);
    }
};

// Owns UIA for the polling loop and discards candidates after transient failures.
void RunMonitor() {
    ComPtr<IUIAutomation> automation;
    if (FAILED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&automation)))) {
        ReportState(L"uia-initialization-failed");
        return;
    }

    DialogMonitor monitor;
    ReportState(L"watching");
    while (WaitForSingleObject(stopEvent, kPollIntervalMs) == WAIT_TIMEOUT) {
        try {
            if (!monitor.Poll(automation.Get())) break;
        } catch (...) {
            monitor.ResetCandidates();
            ReportState(L"snapshot-error");
        }
    }
}

// Keeps COM initialization and teardown on the worker thread, outside Windhawk callbacks.
DWORD WINAPI Watch(void*) {
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        ReportState(L"com-initialization-failed");
        return 1;
    }

    RunMonitor();
    CoUninitialize();
    ReportState(L"stopped");
    return 0;
}

// Creates the worker's stop signal and releases it if thread creation fails.
bool StartWorker() {
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent) return false;

    worker = CreateThread(nullptr, 0, Watch, nullptr, 0, nullptr);
    if (worker) return true;
    CloseOwnedHandle(stopEvent);
    return false;
}

// Starts monitoring only inside an elevated tool host with valid broker paths.
bool StartRuntime() {
    if (!IsElevated()) {
        ReportState(L"elevation-required");
        return false;
    }
    if (!InitializeBrokerPaths()) return false;
    LoadSettings();
    return StartWorker();
}

// Stops the worker before unloading its code; a stuck provider ends only this tool host.
void StopRuntime() {
    if (stopEvent) SetEvent(stopEvent);
    if (worker && WaitForSingleObject(worker, kShutdownWaitMs) != WAIT_OBJECT_0)
        ExitProcess(0);
    CloseOwnedHandle(worker);
    CloseOwnedHandle(stopEvent);
}
#endif
}  // namespace HelloAutoOk

#ifndef HELLO_AUTO_OK_TEST
// Starts the mod's runtime when Windhawk loads its dedicated worker process.
BOOL WhTool_ModInit() { return HelloAutoOk::StartRuntime(); }

// Applies settings changes without restarting the worker.
void WhTool_ModSettingsChanged() { HelloAutoOk::LoadSettings(); }

// Stops monitoring before the dedicated worker process exits.
void WhTool_ModUninit() { HelloAutoOk::StopRuntime(); }

// Adapted from the official Windhawk 1.7.3 tool-mod process model:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
namespace ToolHost {
constexpr PCWSTR kMutexName = L"windhawk-tool-mod_" WH_MOD_ID;
bool isLauncher = false;
HANDLE instanceMutex = nullptr;

enum class ProcessRole { Ignore, Launcher, Worker };

// Excludes Windhawk services, which run outside the interactive user session.
bool IsServiceSession() {
    DWORD sessionId = 0;
    return ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0;
}

// Excludes short-lived Windhawk control commands from launching another worker.
bool IsControlCommand(PCWSTR argument) {
    for (const auto* flag : {L"-service", L"-service-start", L"-service-stop", L"-exit",
                             L"-restart", L"-safe-mode", L"-check-for-updates",
                             L"-app-settings-changed", L"-run-ui"}) {
        if (wcscmp(argument, flag) == 0) return true;
    }
    return false;
}

// Separates the launcher, this mod's worker, and unrelated Windhawk invocations.
ProcessRole GetProcessRole() {
    if (IsServiceSession()) return ProcessRole::Ignore;
    int count = 0;
    LPWSTR* arguments = CommandLineToArgvW(GetCommandLineW(), &count);
    if (!arguments) return ProcessRole::Ignore;

    bool excluded = std::any_of(arguments + 1, arguments + count, IsControlCommand);
    ProcessRole role = ProcessRole::Launcher;
    for (int i = 1; i < count - 1; ++i) {
        if (wcscmp(arguments[i], L"-tool-mod") == 0) {
            role = wcscmp(arguments[i + 1], WH_MOD_ID) == 0
                ? ProcessRole::Worker : ProcessRole::Ignore;
            break;
        }
    }
    LocalFree(arguments);
    return excluded ? ProcessRole::Ignore : role;
}

// Claims the per-session mutex so only one worker for this mod can run.
bool ClaimWorkerInstance() {
    instanceMutex = CreateMutexW(nullptr, TRUE, kMutexName);
    return instanceMutex && GetLastError() != ERROR_ALREADY_EXISTS;
}

// Ends Windhawk's original main thread while the mod's worker keeps the host alive.
void WINAPI StopDefaultMainThread() { ExitThread(0); }

// Replaces only this dedicated executable's entry point, as required by the tool pattern.
void HookHostEntryPoint() {
    auto* base = reinterpret_cast<BYTE*>(GetModuleHandleW(nullptr));
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    void* entryPoint = base + nt->OptionalHeader.AddressOfEntryPoint;
    Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(StopDefaultMainThread), nullptr);
}

// Starts one isolated runtime before preventing the host's normal application startup.
BOOL InitializeWorkerHost() {
    if (!ClaimWorkerInstance() || !WhTool_ModInit()) ExitProcess(1);
    HookHostEntryPoint();
    return TRUE;
}

// Detects an existing worker; denied mutex access also means another instance owns it.
bool IsWorkerRunning() {
    HANDLE existing = OpenMutexW(SYNCHRONIZE, FALSE, kMutexName);
    if (!existing) return GetLastError() == ERROR_ACCESS_DENIED;
    CloseHandle(existing);
    return true;
}

// Uses the current Windhawk executable for the child rather than a configured path.
std::wstring GetExecutablePath() {
    WCHAR path[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    return length && length < ARRAYSIZE(path) ? std::wstring(path, length) : std::wstring{};
}

// Asks Windows to elevate the isolated host when needed and leaves consent to the user.
void LaunchWorker(const std::wstring& path) {
    HRESULT apartment = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    SHELLEXECUTEINFOW launch{sizeof(launch)};
    launch.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
    launch.lpVerb = HelloAutoOk::IsElevated() ? nullptr : L"runas";
    launch.lpFile = path.c_str();
    launch.lpParameters = L"-tool-mod \"" WH_MOD_ID L"\"";
    launch.nShow = SW_HIDE;

    if (!ShellExecuteExW(&launch)) {
        Wh_Log(L"Tool host not started; error %lu", GetLastError());
    } else if (launch.hProcess) {
        CloseHandle(launch.hProcess);
    }
    if (SUCCEEDED(apartment)) CoUninitialize();
}
}  // namespace ToolHost

// Routes Windhawk's initialization callback according to this process's role.
BOOL Wh_ModInit() {
    switch (ToolHost::GetProcessRole()) {
        case ToolHost::ProcessRole::Worker:
            return ToolHost::InitializeWorkerHost();
        case ToolHost::ProcessRole::Ignore:
            return FALSE;
        case ToolHost::ProcessRole::Launcher:
            ToolHost::isLauncher = true;
            return TRUE;
    }
    return FALSE;
}

// Launches a worker after Windhawk finishes loading this mod into its launcher.
void Wh_ModAfterInit() {
    if (!ToolHost::isLauncher || ToolHost::IsWorkerRunning()) return;
    auto path = ToolHost::GetExecutablePath();
    if (!path.empty()) ToolHost::LaunchWorker(path);
}

// Settings belong to the worker; launcher instances have no monitor to update.
void Wh_ModSettingsChanged() {
    if (!ToolHost::isLauncher) WhTool_ModSettingsChanged();
}

// Unloads the runtime only in the worker and leaves the normal Windhawk process alive.
void Wh_ModUninit() {
    if (ToolHost::isLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}
#endif
