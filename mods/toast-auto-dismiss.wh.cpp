// ==WindhawkMod==
// @id              toast-auto-dismiss
// @name            Toast auto-dismiss
// @description     Closes toast notification banners after a configurable number of seconds, unless the mouse pointer is over them
// @version         1.0
// @author          nikitor32
// @github          https://github.com/nikitor32
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32
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
finds it by size and position in the bottom right corner of the screen
and closes it through UI Automation - the same interface screen readers
use to press buttons. No hooks are installed, and no system files or
registry keys are modified.
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
#include <uiautomation.h>

#include <string.h>

#include <atomic>
#include <utility>
#include <vector>

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

// The name lookup below is only touched by the polling thread.
std::vector<std::pair<DWORD, bool>> g_pidCache;

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

// Toast banners are hosted by ShellExperienceHost.exe. The process name
// is checked once per window process and then cached.
bool PidIsShellExperienceHost(DWORD pid) {
    for (const auto& entry : g_pidCache) {
        if (entry.first == pid) {
            return entry.second;
        }
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
    g_pidCache.push_back({pid, result});
    return result;
}

// Decides whether a window is a toast banner currently shown on screen.
//
// ShellExperienceHost keeps one persistent "Windows.UI.Core.CoreWindow"
// window for banners and resizes it: at rest only a ~22px sliver is
// left along the bottom of the screen, while a real banner is a few
// hundred pixels tall and sits in the bottom right corner. The same
// class is also used by other always-present shell windows - for
// example a full-height strip along the right edge of the screen - so
// the window is additionally required to be reasonably short (no more
// than half the screen height) and narrow.
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

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || !PidIsShellExperienceHost(pid)) {
        return false;
    }

    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) {
        return false;
    }

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    int maxHeight = GetSystemMetrics(SM_CYSCREEN) / 2;
    if (maxHeight > 700) {
        maxHeight = 700;
    }
    if (height < 80 || height > maxHeight) {
        return false;
    }
    if (width < 150 || width > 600) {
        return false;
    }

    // The banner touches the right edge of the primary monitor (banners
    // always appear on the primary monitor). The virtual screen edge is
    // checked as well for setups where the primary monitor is not the
    // rightmost one.
    int primaryRight = GetSystemMetrics(SM_CXSCREEN);
    int virtualRight =
        GetSystemMetrics(SM_XVIRTUALSCREEN) + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    bool nearRight = (rc.right >= primaryRight - 8 && rc.right <= primaryRight + 8) ||
                     (rc.right >= virtualRight - 8 && rc.right <= virtualRight + 8);
    if (!nearRight) {
        return false;
    }

    // ...and its bottom is close to the bottom of that monitor.
    int primaryBottom = GetSystemMetrics(SM_CYSCREEN);
    int virtualBottom =
        GetSystemMetrics(SM_YVIRTUALSCREEN) + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    bool nearBottom = (rc.bottom >= primaryBottom - 500 && rc.bottom <= primaryBottom + 8) ||
                      (rc.bottom >= virtualBottom - 500 && rc.bottom <= virtualBottom + 8);
    if (!nearBottom) {
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

// Dismisses the banner through UI Automation by invoking its close
// button - the same button the user presses with the pointer. This is
// deliberately not WM_CLOSE or SC_CLOSE: the shell ignores those for
// this window.
bool TryDismissUia(HWND hwnd) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool comInitialized = SUCCEEDED(hrCo);
    if (FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE) {
        return false;
    }

    bool invoked = false;
    IUIAutomation* uia = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(CUIAutomation), nullptr,
                                  CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation),
                                  reinterpret_cast<void**>(&uia));
    if (SUCCEEDED(hr) && uia) {
        IUIAutomationElement* element = nullptr;
        hr = uia->ElementFromHandle(hwnd, &element);
        if (SUCCEEDED(hr) && element) {
            IUIAutomationCondition* condition = nullptr;
            hr = uia->CreateTrueCondition(&condition);
            if (SUCCEEDED(hr) && condition) {
                IUIAutomationElementArray* children = nullptr;
                hr = element->FindAll(TreeScope_Descendants, condition, &children);
                if (SUCCEEDED(hr) && children) {
                    int count = 0;
                    children->get_Length(&count);
                    for (int i = 0; i < count; i++) {
                        IUIAutomationElement* child = nullptr;
                        if (FAILED(children->GetElement(i, &child)) || !child) {
                            continue;
                        }
                        BSTR automationId = nullptr;
                        if (SUCCEEDED(child->get_CurrentAutomationId(&automationId)) &&
                            automationId) {
                            if (wcscmp(automationId, L"DismissButton") == 0) {
                                IUnknown* pattern = nullptr;
                                if (SUCCEEDED(child->GetCurrentPattern(UIA_InvokePatternId,
                                                                      &pattern)) &&
                                    pattern) {
                                    IUIAutomationInvokePattern* invoke = nullptr;
                                    if (SUCCEEDED(pattern->QueryInterface(
                                            __uuidof(IUIAutomationInvokePattern),
                                            reinterpret_cast<void**>(&invoke))) &&
                                        invoke) {
                                        if (SUCCEEDED(invoke->Invoke())) {
                                            invoked = true;
                                        }
                                        invoke->Release();
                                    }
                                    pattern->Release();
                                }
                            }
                            SysFreeString(automationId);
                        }
                        child->Release();
                        if (invoked) {
                            break;
                        }
                    }
                    children->Release();
                }
                condition->Release();
            }
            element->Release();
        }
        uia->Release();
    }

    if (comInitialized) {
        CoUninitialize();
    }
    return invoked;
}

void PollOnce(ToastState* state) {
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
    if (TryDismissUia(hwnd)) {
        Wh_Log(L"Banner dismissed (%08X, attempt %d)", (DWORD)(ULONG_PTR)hwnd,
               state->attempts);
    } else if (state->attempts <= kMaxLoggedFailures) {
        Wh_Log(L"Dismiss failed (%08X, attempt %d)", (DWORD)(ULONG_PTR)hwnd,
               state->attempts);
    }
}

DWORD WINAPI PollThread(LPVOID /*param*/) {
    // The state is local: only this thread touches it.
    ToastState state;

    // Waiting on the stop event doubles as the polling interval, so the
    // thread exits promptly once the mod is unloaded.
    while (WaitForSingleObject(g_stopEvent, kPollIntervalMs) == WAIT_TIMEOUT) {
        PollOnce(&state);
    }
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
