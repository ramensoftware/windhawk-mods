// ==WindhawkMod==
// @id              toast-auto-dismiss
// @name            Toast auto-dismiss
// @description     Closes toast notification banners after a configurable number of seconds, unless the mouse pointer is over them
// @version         1.0
// @author          nikitor32
// @github          https://github.com/nikitor32
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -ldwmapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Toast auto-dismiss

Windows 11 toast banners usually disappear on their own in about five
seconds, but reminders and notifications from some apps can stay on the
screen until you deal with them.

The mod watches for toast banners and closes them after a configurable
timeout (5 seconds by default). Closing works exactly like pressing the
banner's own close (X) button: the notification is not lost, it stays in
the notification center, and no app notification settings are changed.

While the mouse pointer is over a banner, the countdown is paused, so
the banner's own buttons (reply, dismiss, and similar) remain clickable
for as long as needed.

Windows has its own "Dismiss notifications after this amount of
time" option (Settings > Accessibility > Visual effects, 5 seconds by
default), but many toasts ignore it and stay on the screen until
dismissed. The mod closes banners that ignore the system setting, and
its timeout can also be shorter than the system's five seconds.

The mod runs in a dedicated Windhawk process (as a tool mod) and does
not inject into other applications.

## Settings

* **Timeout (seconds)** - the maximum time a banner stays on screen.
  Default: 5, set to 0 to disable the mod. Use a smaller value to make
  banners disappear faster, or a larger value as a safety net for
  notifications that get stuck on the screen. The value is limited to
  3600 seconds.
* **Pause while hovered** - keep the banner while the pointer is over
  it. Default: on.

## How it works

The banner is a regular window owned by the Windows shell. The mod
finds it by size and position in the bottom right corner of a monitor
and closes it through UI Automation - the same interface screen readers
use to press buttons. No hooks are installed, and no system files or
registry keys are modified.

## Limitations

* Banners are detected by their position in the bottom right corner
  of a monitor, so banners shown elsewhere are not closed. This
  includes banners moved to another corner or monitor (for example by
  the Notifications placement mod) and right-to-left languages, where
  banners appear in the bottom left corner.
* Toasts that are meant to stay on the screen until handled, such as
  incoming call or alarm notifications, are closed by the timeout as
  well. Point at a banner to pause the countdown while it matters.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- timeoutSeconds: 5
  $name: Timeout (seconds)
  $description: The maximum time a toast banner stays on screen. Set to 0 to disable the mod. Values above 3600 are reduced to 3600.
- pauseWhileHovered: true
  $name: Pause while hovered
  $description: The countdown is paused while the mouse pointer is over the banner, so its buttons stay clickable.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <uiautomation.h>

#include <string.h>

#include <atomic>
#include <vector>

// Only available in Windows 10, version 1607 and later.
WINUSERAPI UINT WINAPI GetDpiForWindow(HWND);

// The settings can be updated from another thread while the polling
// thread reads them, so atomic types are used to avoid data races.
std::atomic<int> g_timeoutSeconds{5};
std::atomic<bool> g_pauseWhileHovered{true};

// How often we look for a toast banner, in milliseconds. The banner
// lives for seconds, so a quarter of a second is plenty.
constexpr DWORD kPollIntervalMs = 250;

// A banner which survives the timeout is dismissed again every second
// for as long as it stays on screen, in case UI Automation was busy at
// that moment. Only the first failures are logged so the Windhawk log
// doesn't fill up while a banner is stuck.
constexpr ULONGLONG kRetryIntervalMs = 1000;
constexpr int kMaxLoggedFailures = 3;

HANDLE g_stopEvent = nullptr;
HANDLE g_threadHandle = nullptr;

// The polling state is only touched by the polling thread.
struct ToastState {
    HWND hwnd = nullptr;
    RECT rect{};
    ULONGLONG appearedAt = 0;
    ULONGLONG lastAttemptAt = 0;
    int attempts = 0;
};

// The name lookup below is only touched by the polling thread. The
// cache is keyed by window: a window never changes process, and unlike
// a process id, a window handle value of a live window cannot be
// reused by an unrelated process.
struct PidCacheEntry {
    HWND hwnd;
    DWORD pid;
    bool isShellExperienceHost;
};
std::vector<PidCacheEntry> g_pidCache;

// Toast banners are hosted by ShellExperienceHost.exe. The process name
// is checked once per window and then cached.
bool PidIsShellExperienceHost(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return false;
    }

    for (auto it = g_pidCache.begin(); it != g_pidCache.end(); ++it) {
        if (it->hwnd != hwnd) {
            continue;
        }
        if (it->pid == pid) {
            return it->isShellExperienceHost;
        }
        // The handle value was reused by a window of another process:
        // drop the stale entry and check the new window.
        g_pidCache.erase(it);
        break;
    }

    bool result = false;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (h) {
        WCHAR path[MAX_PATH] = {};
        DWORD size = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(h, 0, path, &size)) {
            const WCHAR* base = path;
            for (const WCHAR* p = path; *p; p++) {
                if (*p == L'\\') {
                    base = p + 1;
                }
            }
            result = _wcsicmp(base, L"ShellExperienceHost.exe") == 0;
        }
        CloseHandle(h);
    }

    if (g_pidCache.size() > 128) {
        g_pidCache.clear();
    }
    g_pidCache.push_back({hwnd, pid, result});
    return result;
}

void LoadSettings() {
    int timeoutSeconds = Wh_GetIntSetting(L"timeoutSeconds");
    if (timeoutSeconds < 0) {
        timeoutSeconds = 0;
    }
    if (timeoutSeconds > 3600) {
        timeoutSeconds = 3600;
    }
    g_timeoutSeconds.store(timeoutSeconds);
    g_pauseWhileHovered.store(Wh_GetIntSetting(L"pauseWhileHovered") != 0);
}

// Decides whether a window is a toast banner currently shown on screen.
//
// ShellExperienceHost keeps one persistent "Windows.UI.Core.CoreWindow"
// window for banners and resizes it: at rest only a ~22px sliver is
// left along the bottom of the screen, while a real banner is a few
// hundred pixels tall and sits in the bottom right corner. The same
// class is also used by other shell windows - for example a
// full-height strip along the right edge of the screen - so the window
// is additionally required to be reasonably short (no more than half
// of its monitor's height) and narrow.
//
// Note: on this Windows build the banner window is not returned by
// EnumWindows or EnumThreadWindows, but FindWindowEx over the desktop's
// children does return it, so that is the enumeration used here.
bool GetToastRectIfBanner(HWND hwnd, RECT* out) {
    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    WCHAR className[128] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) ||
        _wcsicmp(className, L"Windows.UI.Core.CoreWindow") != 0) {
        return false;
    }

    // A cloaked window is kept by the shell but is not shown, for
    // example on another virtual desktop.
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    if (!PidIsShellExperienceHost(hwnd)) {
        return false;
    }

    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) {
        return false;
    }

    // The limits below are written in effective (100%-scaled) pixels
    // and are converted to physical pixels using the DPI of the
    // monitor the window is on, so the detection keeps working at any
    // display scaling: window rectangles and cursor position are in
    // physical pixels because the tool process is per-monitor DPI
    // aware, while a toast's size in effective pixels stays the same.
    UINT dpi = GetDpiForWindow(hwnd);
    if (!dpi) {
        dpi = USER_DEFAULT_SCREEN_DPI;
    }
    auto px = [dpi](int value) {
        return MulDiv(value, dpi, USER_DEFAULT_SCREEN_DPI);
    };

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        return false;
    }

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    // The banner is short (at most half of its monitor's height, and
    // no taller than a large notification) and about as wide as a
    // notification.
    int maxHeight = px(700);
    int halfMonitorHeight =
        (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) / 2;
    if (maxHeight > halfMonitorHeight) {
        maxHeight = halfMonitorHeight;
    }
    if (height < px(80) || height > maxHeight) {
        return false;
    }
    if (width < px(150) || width > px(600)) {
        return false;
    }

    // The banner touches the right edge of its monitor...
    int overhang = rc.right - monitorInfo.rcMonitor.right;
    if (overhang < -px(8) || overhang > px(8)) {
        return false;
    }

    // ...and sits at the bottom, above the taskbar.
    if (rc.bottom < monitorInfo.rcWork.bottom - px(500) ||
        rc.bottom > monitorInfo.rcMonitor.bottom + px(8)) {
        return false;
    }

    *out = rc;
    return true;
}

HWND FindToastBanner(RECT* out) {
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExW(HWND_DESKTOP, hwnd, nullptr, nullptr)) != nullptr) {
        if (GetToastRectIfBanner(hwnd, out)) {
            return hwnd;
        }
    }
    return nullptr;
}

bool IsPointerOver(const RECT& rc) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }
    return PtInRect(&rc, pt) != 0;
}

// A minimal RAII holder for COM interfaces: releases the interface in
// its destructor.
template <typename T>
class ComPtr {
   public:
    ComPtr() = default;
    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ~ComPtr() {
        if (ptr_) {
            ptr_->Release();
        }
    }

    T** Put() {
        return &ptr_;
    }
    T* Get() const {
        return ptr_;
    }
    T* operator->() const {
        return ptr_;
    }
    explicit operator bool() const {
        return ptr_ != nullptr;
    }

   private:
    T* ptr_ = nullptr;
};

// Dismisses the toasts through UI Automation by invoking their close
// buttons - the same buttons the user presses with the pointer. This is
// deliberately not WM_CLOSE or SC_CLOSE: the shell ignores those for
// this window. Stacked toasts each have their own close button, and
// every one of them is invoked so the whole stack clears at once.
bool TryDismissUia(IUIAutomation* uia, HWND hwnd) {
    if (!uia) {
        return false;
    }

    ComPtr<IUIAutomationElement> element;
    if (FAILED(uia->ElementFromHandle(hwnd, element.Put())) || !element) {
        return false;
    }

    // The close buttons are looked up by their automation id in a
    // single cross-process query instead of walking the whole element
    // tree.
    VARIANT id{};
    id.vt = VT_BSTR;
    id.bstrVal = SysAllocString(L"DismissButton");
    if (!id.bstrVal) {
        return false;
    }
    ComPtr<IUIAutomationCondition> condition;
    HRESULT hr = uia->CreatePropertyCondition(UIA_AutomationIdPropertyId, id,
                                              condition.Put());
    VariantClear(&id);
    if (FAILED(hr) || !condition) {
        return false;
    }

    ComPtr<IUIAutomationElementArray> buttons;
    if (FAILED(element->FindAll(TreeScope_Descendants, condition.Get(),
                                buttons.Put())) ||
        !buttons) {
        return false;
    }

    int count = 0;
    buttons->get_Length(&count);

    bool invoked = false;
    for (int i = 0; i < count; i++) {
        ComPtr<IUIAutomationElement> button;
        if (FAILED(buttons->GetElement(i, button.Put())) || !button) {
            continue;
        }
        ComPtr<IUIAutomationInvokePattern> invoke;
        if (FAILED(button->GetCurrentPatternAs(
                UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern),
                reinterpret_cast<void**>(invoke.Put()))) ||
            !invoke) {
            continue;
        }
        if (SUCCEEDED(invoke->Invoke())) {
            invoked = true;
        }
    }
    return invoked;
}

void PollOnce(IUIAutomation* uia, ToastState* state) {
    // The mod is disabled.
    if (g_timeoutSeconds.load() <= 0) {
        return;
    }

    RECT rc;
    HWND hwnd = FindToastBanner(&rc);
    ULONGLONG now = GetTickCount64();

    if (!hwnd) {
        if (state->hwnd) {
            Wh_Log(L"Banner gone (%08X)", (DWORD)(ULONG_PTR)state->hwnd);
            *state = ToastState{};
        }
        return;
    }

    if (state->hwnd != hwnd) {
        Wh_Log(L"Banner appeared (%08X, %dx%d)", (DWORD)(ULONG_PTR)hwnd,
               rc.right - rc.left, rc.bottom - rc.top);
        *state = ToastState{};
        state->hwnd = hwnd;
        state->rect = rc;
        state->appearedAt = now;
        return;
    }

    // The banner grew: another toast was stacked on top of the visible
    // one. The new toast gets a full countdown of its own instead of
    // sharing the time of the toast below it.
    if (rc.bottom - rc.top > state->rect.bottom - state->rect.top) {
        state->appearedAt = now;
        state->attempts = 0;
    }
    state->rect = rc;

    // The pointer is over the banner: the user wants to interact with
    // it. The countdown restarts once the pointer leaves.
    if (g_pauseWhileHovered.load() && IsPointerOver(rc)) {
        state->appearedAt = now;
        return;
    }

    ULONGLONG timeoutMs = (ULONGLONG)g_timeoutSeconds.load() * 1000;
    if (now - state->appearedAt < timeoutMs) {
        return;
    }
    if (state->attempts > 0 && now - state->lastAttemptAt < kRetryIntervalMs) {
        return;
    }

    state->lastAttemptAt = now;
    state->attempts++;
    if (TryDismissUia(uia, hwnd)) {
        Wh_Log(L"Banner dismissed (%08X, attempt %d)", (DWORD)(ULONG_PTR)hwnd,
               state->attempts);
        // A toast that arrived while the stack was being dismissed
        // keeps the window up and gets a countdown of its own.
        state->appearedAt = now;
        state->attempts = 0;
    } else if (state->attempts <= kMaxLoggedFailures) {
        Wh_Log(L"Dismiss failed (%08X, attempt %d)", (DWORD)(ULONG_PTR)hwnd,
               state->attempts);
    }
}

DWORD WINAPI PollThread(LPVOID /*param*/) {
    // COM is initialized once for the whole thread: the polling loop is
    // this thread's only user of COM and UI Automation.
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        Wh_Log(L"CoInitializeEx failed");
        return 0;
    }

    {
        ComPtr<IUIAutomation> uia;
        HRESULT hr = CoCreateInstance(__uuidof(CUIAutomation), nullptr,
                                      CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation),
                                      reinterpret_cast<void**>(uia.Put()));
        if (FAILED(hr) || !uia) {
            Wh_Log(L"Creating the UI Automation object failed");
        } else {
            // The state is local: only this thread touches it.
            ToastState state;

            // Waiting on the stop event doubles as the polling
            // interval, so the thread exits promptly once the mod is
            // unloaded.
            while (WaitForSingleObject(g_stopEvent, kPollIntervalMs) ==
                   WAIT_TIMEOUT) {
                PollOnce(uia.Get(), &state);
            }
        }
    }  // The UI Automation object is released before CoUninitialize.

    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_threadHandle = CreateThread(nullptr, 0, PollThread, nullptr, 0, nullptr);
    if (!g_threadHandle) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    // Wait for the polling thread without a timeout: it can still be
    // inside a UI Automation call when the stop event is set, and
    // unloading the mod while the thread is still running would crash
    // the process.
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_threadHandle) {
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
