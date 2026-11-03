// ==WindhawkMod==
// @id              windows-hello-auto-ok
// @name            Windows Hello Auto OK
// @description     Language-independent confirmation of ready Windows Hello biometric dialogs across applications.
// @version         0.1.3
// @author          hermanzamula
// @github          https://github.com/hermanzamula
// @include         CredentialUIBroker.exe
// @compilerOptions -lole32 -loleaut32 -luuid -luiautomationcore
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
only while the Hello dialog itself is the foreground window. Visible credential input fields prevent
automatic confirmation.

This removes Windows Hello's manual confirmation step: a program requesting
verification can be approved as soon as Windows recognizes you, provided its
Hello dialog is active. Background dialogs are never confirmed automatically.

The mod runs inside CredentialUIBroker.exe, with no helper process or UAC launcher.
It reacts to window events and checks readiness only while its dialog is active.
No credential values, account names, or biometric data are read.

This is an experimental release. Its readiness check describes the standard
Windows Hello UI structure, not an independent biometric-success API. Windows
still performs authentication. Custom application dialogs and future changes
to the system UI may require an update.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DelayMs: 350
  $name: Confirmation delay in milliseconds
  $description: A stable ready state is required for 150 to 3000 milliseconds.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uiautomation.h>
#include <wrl/client.h>
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

    // Each activation starts a fresh stable-readiness interval.
    bool ReadyLongEnough(ULONGLONG now, DWORD delay) {
        if (!readySince) {
            readySince = now;
            return false;
        }
        return now - *readySince >= delay;
    }

    // Resets the timer without allowing a second invoke for the same window.
    void ResetReadiness() {
        readySince.reset();
    }
};

#ifndef HELLO_AUTO_OK_TEST
constexpr DWORD kPollIntervalMs = 150;
constexpr int kMaxDialogControls = 160;

HANDLE stopEvent = nullptr;
HANDLE worker = nullptr;
std::atomic<DWORD> delayMs{350};

// Closes an owned handle and clears it so cleanup can be called more than once.
void CloseOwnedHandle(HANDLE& handle) {
    if (handle) CloseHandle(handle);
    handle = nullptr;
}

// Logs only a generic state; no dialog or account data is stored.
void ReportState(PCWSTR state) { Wh_Log(L"%s", state); }

// Restricts all UI Automation work to this broker's visible foreground window.
bool IsBrokerWindow(HWND window) {
    if (!window || GetForegroundWindow() != window || !IsWindowVisible(window)) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    return pid == GetCurrentProcessId();
}

// Finds descendants within one dialog; a failed query yields an empty COM pointer.
ComPtr<IUIAutomationElementArray> GetDialogControls(IUIAutomation* automation, HWND window) {
    ComPtr<IUIAutomationElement> root;
    ComPtr<IUIAutomationCondition> condition;
    ComPtr<IUIAutomationElementArray> controls;
    ComPtr<IUIAutomationCacheRequest> cache;
    if (FAILED(automation->CreateCacheRequest(&cache))) return {};
    for (PROPERTYID property : {UIA_IsOffscreenPropertyId, UIA_IsEnabledPropertyId,
            UIA_IsPasswordPropertyId, UIA_ControlTypePropertyId, UIA_AutomationIdPropertyId}) {
        if (FAILED(cache->AddProperty(property))) return {};
    }
    if (FAILED(automation->ElementFromHandle(window, &root)) ||
        FAILED(automation->CreateTrueCondition(&condition)) ||
        FAILED(root->FindAllBuildCache(TreeScope_Descendants, condition.Get(), cache.Get(), &controls))) return {};
    return controls;
}

// Reads structural properties, leaving offscreen controls otherwise untouched.
bool ReadControlProperties(IUIAutomationElement* element, Node& node) {
    BOOL offscreen = TRUE;
    if (FAILED(element->get_CachedIsOffscreen(&offscreen))) return false;
    node.visible = !offscreen;
    if (!node.visible) return true;

    BOOL active = FALSE, password = TRUE;
    if (FAILED(element->get_CachedIsEnabled(&active)) ||
        FAILED(element->get_CachedIsPassword(&password)) ||
        FAILED(element->get_CachedControlType(&node.type))) return false;
    node.enabled = active;
    node.password = password;
    return true;
}

// Reads only the language-neutral ID, releasing the string allocated by UIA.
bool ReadAutomationId(IUIAutomationElement* element, std::wstring& result) {
    BSTR id = nullptr;
    HRESULT status = element->get_CachedAutomationId(&id);
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

// Bounds the readiness delay to supported values.
void LoadSettings() {
    delayMs.store(static_cast<DWORD>(std::clamp(Wh_GetIntSetting(L"DelayMs"), 150, 3000)));
}

// Invokes the exact control while its broker dialog remains visible and allowed.
bool InvokeConfirmation(HWND window, IUIAutomationElement* button) {
    ComPtr<IUIAutomationInvokePattern> invoke;
    if (FAILED(button->GetCurrentPatternAs(UIA_InvokePatternId, IID_PPV_ARGS(&invoke)))) {
        ReportState(L"invoke-unavailable");
        return false;
    }
    if (!IsBrokerWindow(window)) return false;

    ReportState(L"invoking-confirmation");
    if (SUCCEEDED(invoke->Invoke())) return true;
    ReportState(L"invoke-failed");
    return false;
}

class DialogMonitor {
public:
    // Reads only the active broker dialog, resetting timing after focus changes.
    void Poll(IUIAutomation* automation) {
        HWND window = GetForegroundWindow();
        if (window != active_) ResetCandidates();
        active_ = window;
        if (IsBrokerWindow(window)) PollDialog(automation, window);
    }

    // Destroy events end a window's lifetime, allowing a reused handle to start fresh.
    void Forget(HWND window) { dialogs_.erase(window); }

    // Discards timing after a pause or exception without repeating handled confirmations.
    void ResetCandidates() {
        for (auto& [window, state] : dialogs_) state.readySince.reset();
    }

private:
    std::map<HWND, DialogState> dialogs_;
    HWND active_ = nullptr;

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
        InvokeConfirmation(window, button->element.Get());
        return true;
    }

    // Obtains the final ready snapshot only while the same dialog remains active.
    std::optional<std::vector<Node>> GetCurrentReadyDialog(IUIAutomation* automation, HWND window) {
        if (!IsBrokerWindow(window)) return std::nullopt;
        return ReadReadyDialog(automation, window);
    }
};

// Event callbacks run on the worker's message loop and never call UI Automation.
thread_local bool focusChanged = false;
thread_local std::vector<HWND> destroyedWindows;
void CALLBACK OnWindowEvent(HWINEVENTHOOK, DWORD event, HWND window,
                            LONG object, LONG child, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_FOREGROUND) focusChanged = true;
    if (event == EVENT_OBJECT_DESTROY && object == OBJID_WINDOW && child == CHILDID_SELF)
        destroyedWindows.push_back(window);
}

// Dispatches events before polling, preserving focus changes between timer ticks.
void DispatchEvents(DialogMonitor& monitor) {
    MSG message;
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    if (focusChanged) monitor.ResetCandidates();
    focusChanged = false;
    for (HWND window : destroyedWindows) monitor.Forget(window);
    destroyedWindows.clear();
}

// Sleeps indefinitely while inactive; only active broker dialogs need readiness ticks.
void RunMonitor() {
    ComPtr<IUIAutomation> automation;
    if (FAILED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&automation)))) return;
    HWINEVENTHOOK focus = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        nullptr, OnWindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK lifetime = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE,
        nullptr, OnWindowEvent, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    if (focus && lifetime) {
        DialogMonitor monitor;
        for (;;) {
            DispatchEvents(monitor);
            if (WaitForSingleObject(stopEvent, 0) == WAIT_OBJECT_0) break;
            try { monitor.Poll(automation.Get()); }
            catch (...) { monitor.ResetCandidates(); ReportState(L"snapshot-error"); }
            DWORD timeout = IsBrokerWindow(GetForegroundWindow()) ? kPollIntervalMs : INFINITE;
            DWORD result = MsgWaitForMultipleObjects(1, &stopEvent, FALSE, timeout, QS_ALLINPUT);
            if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) break;
        }
    } else {
        ReportState(L"window-event-hook-failed");
    }
    if (lifetime) UnhookWinEvent(lifetime);
    if (focus) UnhookWinEvent(focus);
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

// Rejects executables outside the two Windows broker locations, even if renamed.
bool IsSystemBroker() {
    WCHAR image[MAX_PATH], windows[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, image, ARRAYSIZE(image));
    UINT directoryLength = GetSystemWindowsDirectoryW(windows, ARRAYSIZE(windows));
    if (!length || length >= ARRAYSIZE(image) ||
        !directoryLength || directoryLength >= ARRAYSIZE(windows)) return false;
    for (PCWSTR directory : {L"\\System32\\CredentialUIBroker.exe", L"\\SysWOW64\\CredentialUIBroker.exe"}) {
        if (_wcsicmp(image, (std::wstring(windows) + directory).c_str()) == 0) return true;
    }
    return false;
}

// Starts a worker in this broker process without creating an elevated host.
bool StartRuntime() {
    if (!IsSystemBroker()) return false;
    LoadSettings();
    return StartWorker();
}

// Waits for COM and event callbacks to finish before the DLL can be unloaded.
void StopRuntime() {
    if (stopEvent) SetEvent(stopEvent);
    if (worker) WaitForSingleObject(worker, INFINITE);
    CloseOwnedHandle(worker);
    CloseOwnedHandle(stopEvent);
}
#endif
}  // namespace HelloAutoOk

#ifndef HELLO_AUTO_OK_TEST
// Windhawk loads this mod only in the credential broker.
BOOL Wh_ModInit() { return HelloAutoOk::StartRuntime(); }

// Applies the delay without restarting the worker.
void Wh_ModSettingsChanged() { HelloAutoOk::LoadSettings(); }

// Releases the worker without terminating the credential broker.
void Wh_ModUninit() { HelloAutoOk::StopRuntime(); }
#endif
