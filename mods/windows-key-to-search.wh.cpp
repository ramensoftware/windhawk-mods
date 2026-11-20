// ==WindhawkMod==
// @id              windows-key-to-search
// @name            Windows Key to Search
// @description     Open Windows Search when either Windows key is tapped.
// @version         1.2
// @author          Tyler
// @github          https://github.com/snowtyler
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @include         windhawk-mod-uiaccess.exe
// @compilerOptions -luser32 -lshell32 -ladvapi32 -lole32 -loleaut32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Key to Search

Tap either Windows key to open Search instead of Start, and tap again to
close it. This works even when the taskbar Search button is hidden.

Windows-key shortcuts (Win+E, Win+R, Win+Shift+S, etc.) work normally, and so
do other tools that use the Windows key, such as PowerToys Keyboard Manager
remaps and Win+drag in AltSnap.

On the Windows 11 taskbar, the taskbar appears alongside Search, even over
fullscreen apps. This relies on an internal Windows message and may break in
future Windows versions; it can be turned off in the settings. It isn't used
on Windows 10 or with a restored Windows 10 taskbar (e.g. ExplorerPatcher).

Thanks to [Asteski](https://github.com/Asteski) for the method of opening
Search used since version 1.2.

## Administrator windows

- **Windhawk 2.0 alpha 4+:** works in administrator windows automatically.
- **Windhawk 1.7.3:** administrator windows keep normal Windows-key behavior
  unless you enable **Support administrator windows on Windhawk 1.7.3**. That
  option shows a UAC prompt at sign-in and after enabling or updating the mod.

## Limitations

- Doesn't work on secure desktops such as the UAC prompt.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showTaskbarWithSearch: true
  $name: Show the taskbar with Search
  $description: >-
    Show the taskbar alongside Search, including over fullscreen apps. Windows 11
    taskbar only.
- supportAdministratorWindows: false
  $name: Support administrator windows on Windhawk 1.7.3
  $description: >-
    Run as administrator so the mod works in administrator windows. Shows a UAC
    prompt at sign-in. Re-enable the mod after changing. Not needed on
    Windhawk 2.0.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <wrl/client.h>
#include <atomic>
#include <initializer_list>
#include <cwchar>

namespace {
constexpr ULONG_PTR kInputTag = 0x57485352;
constexpr UINT kOpenSearchMessage = WM_APP + 1;
// Posted when a Win key goes down or up, so the hook thread installs the
// mouse hook only while a Win key is held.
constexpr UINT kSyncMouseHookMessage = WM_APP + 2;
// Pressed before a Win release to keep Start from opening. A modifier, so
// browsers don't treat it as keyboard use and show focus rings (an unassigned
// key such as 0xE8 does). Shift, because a Ctrl tap triggers "Show location of
// pointer when I press the CTRL key" and an Alt tap activates menu bars.
constexpr WORD kMenuMaskKey = VK_RSHIFT;
HANDLE g_thread;
HANDLE g_ready;
DWORD g_threadId;
bool g_hooksInstalled;
bool g_hasElevatedInputAccess;
bool g_canRemapForeground;
HWND g_cachedForeground;
HANDLE g_elevationThread;
HANDLE g_elevationThreadDone;
bool g_keys[256];
bool g_tap[2];
bool g_shortcutUsed[2];
HANDLE g_searchThread;
HANDLE g_searchStop;
HANDLE g_searchRequests;
HANDLE g_searchReady;
bool g_searchInitialized;
// TrayUI::WndProc forwards this to CTaskBand::_HandleSearchVisibilityChange,
// updates the Search monitor, unhides the tray, and resets its z-order.
// Verified in Windows 11 Taskbar.dll; lParam is an HMONITOR, not a pointer.
constexpr UINT kTaskbarSearchVisibility = WM_USER + 0x1D1;
UINT_PTR g_taskbarTimer;
HWND g_taskbarWindow;
HWND g_taskbarSearchWindow;
std::atomic<bool> g_showTaskbar{true};
ULONGLONG g_taskbarDeadline;
bool g_taskbarNotified;
HWND g_lastSearchCandidate;

void EndTaskbarReveal() {
    if (g_taskbarTimer) {
        KillTimer(nullptr, g_taskbarTimer);
        g_taskbarTimer = 0;
    }
    if (g_taskbarNotified) {
        if (!PostMessageW(g_taskbarWindow, kTaskbarSearchVisibility, FALSE, 0)) {
            Wh_Log(L"Could not clear taskbar Search visibility (error %lu)", GetLastError());
        }
    }
    g_taskbarWindow = nullptr;
    g_taskbarSearchWindow = nullptr;
    g_taskbarNotified = false;
    g_lastSearchCandidate = nullptr;
}

bool IsSearchWindow(HWND window) {
    DWORD pid = 0;
    WCHAR className[64] = {};
    GetClassNameW(window, className, ARRAYSIZE(className));
    const bool logCandidate = window != g_lastSearchCandidate;
    g_lastSearchCandidate = window;
    if (!GetWindowThreadProcessId(window, &pid)) {
        if (logCandidate) {
            Wh_Log(L"Foreground window: no process (error %lu)", GetLastError());
        }
        return false;
    }
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        if (logCandidate) {
            Wh_Log(L"Foreground window: HWND=%p, class=%s, PID=%lu; process query denied (error %lu)",
                   window, className, pid, GetLastError());
        }
        return false;
    }
    WCHAR path[MAX_PATH];
    DWORD length = ARRAYSIZE(path);
    const BOOL read = QueryFullProcessImageNameW(process, 0, path, &length);
    const DWORD error = read ? ERROR_SUCCESS : GetLastError();
    CloseHandle(process);
    if (!read) {
        if (logCandidate) {
            Wh_Log(L"Foreground window: HWND=%p, class=%s, PID=%lu; path query failed (error %lu)",
                   window, className, pid, error);
        }
        return false;
    }
    const WCHAR* name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    if (logCandidate) {
        Wh_Log(L"Foreground window: HWND=%p, class=%s, PID=%lu, process=%s",
               window, className, pid, name);
    }
    // Newer Windows 11 builds host Search in ShellHost.exe. It also hosts
    // other shell flyouts, so require its frame class. This runs on every tap
    // (to choose between Escape and opening Search) and while tracking the
    // taskbar after our own Search request.
    if (_wcsicmp(name, L"ShellHost.exe") == 0) {
        return wcscmp(className, L"ShellUXFrameWindowClass") == 0;
    }
    return _wcsicmp(name, L"SearchHost.exe") == 0 ||
           _wcsicmp(name, L"SearchApp.exe") == 0 ||
           _wcsicmp(name, L"SearchUI.exe") == 0;
}

void CheckTaskbarReveal() {
    if (!g_taskbarTimer) {
        return;
    }
    HWND foreground = GetForegroundWindow();
    if (g_taskbarSearchWindow && foreground == g_taskbarSearchWindow) {
        return;
    }
    if (IsSearchWindow(foreground)) {
        g_taskbarSearchWindow = foreground;
        // Search can open on a monitor other than the former fullscreen app.
        HMONITOR monitor = MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
        if (!monitor ||
            !PostMessageW(g_taskbarWindow, kTaskbarSearchVisibility, TRUE,
                          reinterpret_cast<LPARAM>(monitor))) {
            Wh_Log(L"Could not notify taskbar of Search visibility (error %lu)", GetLastError());
            EndTaskbarReveal();
            return;
        }
        g_taskbarNotified = true;
        Wh_Log(L"Notified taskbar: Search visible on monitor %p", monitor);
    } else if (g_taskbarNotified || GetTickCount64() >= g_taskbarDeadline) {
        if (!g_taskbarNotified) {
            Wh_Log(L"Search visibility request expired without finding the Search foreground window");
        }
        EndTaskbarReveal();
    }
}

void PrepareTaskbarReveal() {
    EndTaskbarReveal();
    if (!g_showTaskbar) {
        return;
    }
    // A tap while Search is open closes it; tracking would re-show the taskbar
    // until the closing Search window loses the foreground.
    if (IsSearchWindow(GetForegroundWindow())) {
        return;
    }
    // The primary tray forwards this notification to secondary taskbars too.
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    // The message is only verified on the Windows 11 XAML taskbar, not on
    // Windows 10 or a restored Windows 10 taskbar (e.g. ExplorerPatcher).
    if (!taskbar ||
        !FindWindowExW(taskbar, nullptr,
                       L"Windows.UI.Composition.DesktopWindowContentBridge",
                       nullptr)) {
        Wh_Log(L"Taskbar Search visibility unavailable: no Windows 11 taskbar");
        return;
    }
    g_taskbarWindow = taskbar;
    g_taskbarDeadline = GetTickCount64() + 1500;
    g_taskbarTimer = SetTimer(nullptr, 0, 50, nullptr);
    if (!g_taskbarTimer) {
        Wh_Log(L"Could not start Search visibility tracking (error %lu)", GetLastError());
        EndTaskbarReveal();
    } else {
        Wh_Log(L"Tracking Search visibility for taskbar HWND=%p", taskbar);
    }
}

bool IsWindowsKey(DWORD vk) {
    return vk == VK_LWIN || vk == VK_RWIN;
}

bool IsModifierKey(DWORD vk) {
    return vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU ||
           vk == VK_LSHIFT || vk == VK_RSHIFT ||
           vk == VK_LCONTROL || vk == VK_RCONTROL ||
           vk == VK_LMENU || vk == VK_RMENU;
}

INPUT KeyboardInput(WORD vk, DWORD flags = 0, WORD scan = 0) {
    if (vk == VK_RCONTROL || vk == VK_RMENU || IsWindowsKey(vk)) {
        flags |= KEYEVENTF_EXTENDEDKEY;
    }
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.wScan = scan;
    input.ki.dwFlags = flags;
    input.ki.dwExtraInfo = kInputTag;
    return input;
}

// Builds the swallowed Win release. The mask key goes down before it so Start
// doesn't open. If the mask key is already physically held, only its down is
// sent (it reads as an autorepeat) and no up, so the held key isn't released.
UINT MaskedWinRelease(INPUT* out, WORD vk) {
    UINT count = 0;
    out[count++] = KeyboardInput(kMenuMaskKey);
    if (!g_keys[kMenuMaskKey]) {
        out[count++] = KeyboardInput(kMenuMaskKey, KEYEVENTF_KEYUP);
    }
    out[count++] = KeyboardInput(vk, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP);
    return count;
}

bool Inject(INPUT* inputs, UINT count) {
    const UINT sent = SendInput(count, inputs, sizeof(INPUT));
    if (sent == count) {
        return true;
    }
    // Release any key-downs that made it into a partially accepted batch.
    INPUT releases[32] = {};
    UINT releaseCount = 0;
    for (UINT i = 0; i < sent; ++i) {
        if (!(inputs[i].ki.dwFlags & KEYEVENTF_KEYUP)) {
            if (IsWindowsKey(inputs[i].ki.wVk)) {
                releases[releaseCount++] = KeyboardInput(kMenuMaskKey);
                releases[releaseCount++] = KeyboardInput(kMenuMaskKey, KEYEVENTF_KEYUP);
            }
            releases[releaseCount] = inputs[i];
            releases[releaseCount++].ki.dwFlags |= KEYEVENTF_KEYUP;
        }
    }
    if (releaseCount) {
        SendInput(releaseCount, releases, sizeof(INPUT));
    }
    return false;
}

// All COM objects are created, used, and released exclusively on this MTA.
class SearchClient {
    template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    ComPtr<IUIAutomation> automation;
    ComPtr<IUIAutomationElement> button;
    ComPtr<IUIAutomationTogglePattern> toggle;
    HWND taskbar = nullptr;
    DWORD taskbarProcess = 0;

    void Invalidate() {
        toggle.Reset();
        button.Reset();
    }

    HRESULT Resolve() {
        HWND window = FindWindowW(L"Shell_TrayWnd", nullptr);
        DWORD process = 0;
        if (window) GetWindowThreadProcessId(window, &process);
        if (window != taskbar || process != taskbarProcess) {
            Invalidate();
            taskbar = window;
            taskbarProcess = process;
        }
        if (!window) return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        if (toggle) return S_OK;
        HRESULT hr = S_OK;
        if (!automation) {
            hr = CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&automation));
            if (FAILED(hr)) return hr;
        }
        ComPtr<IUIAutomationElement> root;
        hr = automation->ElementFromHandle(window, &root);
        if (FAILED(hr)) return hr;
        VARIANT id;
        VariantInit(&id);
        id.vt = VT_BSTR;
        id.bstrVal = SysAllocString(L"SearchButton");
        if (!id.bstrVal) return E_OUTOFMEMORY;
        ComPtr<IUIAutomationCondition> condition;
        hr = automation->CreatePropertyCondition(UIA_AutomationIdPropertyId, id, &condition);
        VariantClear(&id);
        if (FAILED(hr)) return hr;
        hr = root->FindFirst(TreeScope_Descendants, condition.Get(), button.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
        if (!button) return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        hr = button->GetCurrentPatternAs(UIA_TogglePatternId, IID_PPV_ARGS(&toggle));
        // A button without Toggle must fail so the SearchCommand fallback runs.
        if (SUCCEEDED(hr) && !toggle) hr = E_NOINTERFACE;
        return hr;
    }

public:
    static HRESULT ShellSearch() {
        CLSID clsid;
        HRESULT hr = CLSIDFromProgID(L"Shell.Application", &clsid);
        if (FAILED(hr)) return hr;
        ComPtr<IDispatch> shell;
        hr = CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
                              IID_PPV_ARGS(&shell));
        if (FAILED(hr)) return hr;
        LPOLESTR name = const_cast<LPOLESTR>(L"SearchCommand");
        DISPID dispid;
        hr = shell->GetIDsOfNames(IID_NULL, &name, 1, LOCALE_USER_DEFAULT, &dispid);
        if (FAILED(hr)) return hr;
        DISPPARAMS args = {};
        return shell->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD,
                             &args, nullptr, nullptr, nullptr);
    }

    HRESULT Open() {
        HRESULT hr = E_FAIL;
        for (int attempt = 0; attempt < 2; ++attempt) {
            hr = Resolve();
            if (SUCCEEDED(hr)) hr = toggle->Toggle();
            if (SUCCEEDED(hr)) {
                Wh_Log(L"Search: taskbar Toggle succeeded");
                return hr;
            }
            Invalidate();
            if (hr != static_cast<HRESULT>(UIA_E_ELEMENTNOTAVAILABLE) &&
                hr != static_cast<HRESULT>(RPC_E_DISCONNECTED)) break;
            Wh_Log(L"Search: stale UIA object (0x%08X), attempt %d", hr, attempt + 1);
        }
        Wh_Log(L"Search: UIA failed (0x%08X); using SearchCommand", hr);
        hr = ShellSearch();
        Wh_Log(L"Search: SearchCommand result 0x%08X", hr);
        return hr;
    }
};

DWORD WINAPI SearchThread(void*) {
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    g_searchInitialized = SUCCEEDED(hr);
    SetEvent(g_searchReady);
    if (FAILED(hr)) {
        Wh_Log(L"Search: COM initialization failed (0x%08X)", hr);
        return 0;
    }
    {
        SearchClient client;
        HANDLE waits[] = {g_searchStop, g_searchRequests};
        while (WaitForMultipleObjects(ARRAYSIZE(waits), waits, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) {
            client.Open();
        }
    }
    CoUninitialize();
    return 0;
}

void OpenSearch() {
    // SearchCommand only opens Search. Handle dismissal on the input thread,
    // before queuing COM work, so the hidden-button fallback also toggles.
    // Do not delay Escape behind UIA calls that might outlive Search's focus.
    if (IsSearchWindow(GetForegroundWindow())) {
        INPUT escape[] = {
            KeyboardInput(VK_ESCAPE),
            KeyboardInput(VK_ESCAPE, KEYEVENTF_KEYUP),
        };
        if (Inject(escape, ARRAYSIZE(escape))) {
            EndTaskbarReveal();
            Wh_Log(L"Search: dismissed focused Search with Escape");
        } else {
            Wh_Log(L"Search: could not send Escape");
        }
        return;
    }
    PrepareTaskbarReveal();
    // Auto-reset event: taps made while a UIA call is slow coalesce into one.
    if (!SetEvent(g_searchRequests)) {
        Wh_Log(L"Could not queue Search (error %lu)", GetLastError());
        EndTaskbarReveal();
    }
}

void ResetKeyTracking() {
    const bool winHeld = g_keys[VK_LWIN] || g_keys[VK_RWIN];
    ZeroMemory(g_keys, sizeof(g_keys));
    ZeroMemory(g_tap, sizeof(g_tap));
    ZeroMemory(g_shortcutUsed, sizeof(g_shortcutUsed));
    if (winHeld) {
        PostThreadMessageW(g_threadId, kSyncMouseHookMessage, 0, 0);
    }
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM message, LPARAM param) {
    if (code != HC_ACTION) {
        return CallNextHookEx(nullptr, code, message, param);
    }
    const auto& key = *reinterpret_cast<const KBDLLHOOKSTRUCT*>(param);
    if (key.dwExtraInfo == kInputTag || key.vkCode >= 256) {
        return CallNextHookEx(nullptr, code, message, param);
    }
    const bool down = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
    const bool up = message == WM_KEYUP || message == WM_SYSKEYUP;
    if (!down && !up) {
        return CallNextHookEx(nullptr, code, message, param);
    }
    if (!g_hasElevatedInputAccess &&
        (!g_canRemapForeground || GetForegroundWindow() != g_cachedForeground)) {
        // Inaccessible/elevated foreground windows retain their original input.
        ResetKeyTracking();
        if (IsWindowsKey(key.vkCode) && down) {
            // Record the held Win so autorepeats after focus moves to an
            // accessible window aren't mistaken for a fresh press (and a tap).
            g_keys[key.vkCode] = true;
        }
        return CallNextHookEx(nullptr, code, message, param);
    }
    const bool win = IsWindowsKey(key.vkCode);
    const int index = key.vkCode == VK_LWIN ? 0 : 1;
    if (win && up && !g_keys[key.vkCode]) {
        // A down outside our desktop/access scope has no tracked tap.
        return CallNextHookEx(nullptr, code, message, param);
    }
    if (!win && (key.flags & LLKHF_INJECTED)) {
        // Other apps' injected keys (AltSnap's kill key, PowerToys remaps)
        // never autorepeat and may lack a matching up, so they aren't
        // recorded in g_keys: each down is a fresh press and can't leave a
        // stale record that makes a later physical press look like a repeat.
        if (down) {
            g_tap[0] = g_tap[1] = false;
            if (!IsModifierKey(key.vkCode)) {
                for (int i = 0; i < 2; ++i) {
                    if (g_keys[i == 0 ? VK_LWIN : VK_RWIN]) {
                        g_shortcutUsed[i] = true;
                    }
                }
            }
        }
        return CallNextHookEx(nullptr, code, message, param);
    }
    if (down) {
        // A down while already recorded as held is an autorepeat. Never ask
        // GetAsyncKeyState: it isn't reliable for repeats in the hook. Stale
        // records from lost releases (Win+L) are cleared by desktop-switch
        // events; one that slips through only costs that press its Search.
        const bool repeat = g_keys[key.vkCode];
        g_keys[key.vkCode] = true;
        if (win) {
            if (repeat) {
                return CallNextHookEx(nullptr, code, message, param);
            }
            // Any other key (including the other Win) cancels a tap.
            g_tap[0] = g_tap[1] = false;
            g_tap[index] = true;
            g_shortcutUsed[index] = false;
            PostThreadMessageW(g_threadId, kSyncMouseHookMessage, 0, 0);
        } else if (!repeat || !IsModifierKey(key.vkCode)) {
            // A fresh key down (a modifier too, which is how Win+Shift gets a
            // masked release and no Search) or a non-modifier repeat while Win
            // is held cancels the tap, even a key held before Win: a missed
            // Search is safer than Search opening on top of a shortcut.
            g_tap[0] = g_tap[1] = false;
            if (!repeat && !IsModifierKey(key.vkCode)) {
                for (int i = 0; i < 2; ++i) {
                    if (g_keys[i == 0 ? VK_LWIN : VK_RWIN]) {
                        g_shortcutUsed[i] = true;
                    }
                }
            }
        }
    } else {
        g_keys[key.vkCode] = false;
        if (win) {
            const bool tap = g_tap[index];
            const bool shortcutUsed = g_shortcutUsed[index];
            g_tap[index] = false;
            g_shortcutUsed[index] = false;
            PostThreadMessageW(g_threadId, kSyncMouseHookMessage, 0, 0);
            if (tap) {
                // Post to our own thread so input is sent after this hook
                // returns, rather than during the physical Win-up callback.
                if (!PostThreadMessageW(g_threadId, kOpenSearchMessage, 0, 0)) {
                    Wh_Log(L"Could not queue Search (error %lu)", GetLastError());
                }
            }
            if (shortcutUsed) {
                // Win+V and other flyouts must receive the real release, with
                // no mask keystroke to dismiss their UI.
                return CallNextHookEx(nullptr, code, message, param);
            }
            // Bare taps and modifier-only combos (e.g. Win+Shift) would open
            // Start; swallow the release and send it with a mask key instead.
            INPUT release[3];
            const UINT releaseCount =
                MaskedWinRelease(release, static_cast<WORD>(key.vkCode));
            if (Inject(release, releaseCount)) {
                return 1;
            }
        }
    }
    return CallNextHookEx(nullptr, code, message, param);
}

LRESULT CALLBACK MouseProc(int code, WPARAM message, LPARAM param) {
    if (code == HC_ACTION && (g_keys[VK_LWIN] || g_keys[VK_RWIN])) {
        switch (message) {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_XBUTTONDOWN:
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
                // Win+click/drag/scroll isn't a bare tap. The release stays
                // masked (not passed through): Start suppression after a
                // mouse gesture isn't guaranteed, and no flyout needs the
                // real release. Never swallow mouse input.
                g_tap[0] = g_tap[1] = false;
                break;
        }
    }
    return CallNextHookEx(nullptr, code, message, param);
}

// Installs the mouse hook while a Win key is held and removes it otherwise,
// so no global mouse hook runs (and delays input) the rest of the time.
void SyncMouseHook(HHOOK& mouse, HMODULE module) {
    const bool winHeld = g_keys[VK_LWIN] || g_keys[VK_RWIN];
    if (winHeld && !mouse) {
        // Optional: without it only Win+mouse gestures lose tap cancellation.
        mouse = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, module, 0);
        if (!mouse) {
            Wh_Log(L"Mouse hook failed (error %lu); Win+mouse may open Search",
                   GetLastError());
        }
    } else if (!winHeld && mouse) {
        UnhookWindowsHookEx(mouse);
        mouse = nullptr;
    }
}

void RefreshForegroundAccess() {
    g_cachedForeground = GetForegroundWindow();
    g_canRemapForeground = g_hasElevatedInputAccess;
    if (g_hasElevatedInputAccess) {
        return;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(g_cachedForeground, &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    HANDLE token = nullptr;
    if (process && OpenProcessToken(process, TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation = {};
        DWORD uiAccess = 0;
        DWORD length = 0;
        const BOOL readElevation = GetTokenInformation(
            token, TokenElevation, &elevation, sizeof(elevation), &length);
        const BOOL readUiAccess = GetTokenInformation(
            token, TokenUIAccess, &uiAccess, sizeof(uiAccess), &length);
        g_canRemapForeground = readElevation && readUiAccess &&
                              !elevation.TokenIsElevated && !uiAccess;
        CloseHandle(token);
    }
    if (process) {
        CloseHandle(process);
    }
    if (!g_canRemapForeground) {
        ResetKeyTracking();
    }
}

void CALLBACK InputContextChanged(HWINEVENTHOOK, DWORD event, HWND, LONG,
                                  LONG, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_DESKTOPSWITCH) {
        // Releases on the lock screen/UAC desktop are invisible to our hook.
        ResetKeyTracking();
        EndTaskbarReveal();
    }
    RefreshForegroundAccess();
    CheckTaskbarReveal();
}

DWORD WINAPI HookThread(void*) {
    // Create the message queue before publishing readiness to ModInit.
    MSG message;
    PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
    for (int vk = VK_BACK; vk < 256; ++vk) {
        // Hooks report left/right modifiers, never these aggregate values.
        // Seeding aggregates would leave stale "held" keys after release.
        if (vk != VK_SHIFT && vk != VK_CONTROL && vk != VK_MENU) {
            g_keys[vk] = (GetAsyncKeyState(vk) & 0x8000) != 0;
        }
    }
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<LPCWSTR>(&KeyboardProc), &module);
    HWINEVENTHOOK foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        InputContextChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK desktopHook = SetWinEventHook(
        EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH, nullptr,
        InputContextChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
    RefreshForegroundAccess();
    HHOOK keyboard = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, module, 0);
    g_hooksInstalled = keyboard && foregroundHook && desktopHook;
    // Installed only while a Win key is held (see SyncMouseHook).
    HHOOK mouse = nullptr;
    SetEvent(g_ready);

    if (g_hooksInstalled) {
        SyncMouseHook(mouse, module);  // A Win key may already be held.
        while (GetMessageW(&message, nullptr, 0, 0) > 0) {
            if (message.message == WM_TIMER && g_taskbarTimer &&
                message.wParam == g_taskbarTimer) {
                CheckTaskbarReveal();
                continue;
            }
            if (message.message == kSyncMouseHookMessage) {
                SyncMouseHook(mouse, module);
                continue;
            }
            if (message.message == kOpenSearchMessage) {
                RefreshForegroundAccess();
                if (!g_canRemapForeground) {
                    continue;
                }
                OpenSearch();
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    EndTaskbarReveal();
    if (keyboard) {
        UnhookWindowsHookEx(keyboard);
    }
    if (mouse) {
        UnhookWindowsHookEx(mouse);
    }
    if (foregroundHook) {
        UnhookWinEvent(foregroundHook);
    }
    if (desktopHook) {
        UnhookWinEvent(desktopHook);
    }
    // A forwarded Windows key may still be held when the mod is disabled.
    INPUT releases[6] = {};
    UINT count = 0;
    for (int i = 0; i < 2; ++i) {
        const WORD vk = i == 0 ? VK_LWIN : VK_RWIN;
        if (g_keys[vk]) {
            count += MaskedWinRelease(releases + count, vk);
        }
    }
    if (count) {
        Inject(releases, count);
    }
    return 0;
}

void Cleanup() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_searchStop) SetEvent(g_searchStop);
    if (g_searchThread) {
        // Do not unload the DLL while a COM call is executing.
        WaitForSingleObject(g_searchThread, INFINITE);
        CloseHandle(g_searchThread);
        g_searchThread = nullptr;
    }
    for (HANDLE* handle : {&g_searchReady, &g_searchRequests, &g_searchStop}) {
        if (*handle) CloseHandle(*handle);
        *handle = nullptr;
    }
    g_searchInitialized = false;
    if (g_ready) {
        CloseHandle(g_ready);
        g_ready = nullptr;
    }
}
}  // namespace

bool HasElevatedInputAccess() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return false;
    }
    TOKEN_ELEVATION elevation = {};
    DWORD uiAccess = 0;
    DWORD length = 0;
    const BOOL readElevation = GetTokenInformation(
        token, TokenElevation, &elevation, sizeof(elevation), &length);
    const BOOL readUiAccess = GetTokenInformation(
        token, TokenUIAccess, &uiAccess, sizeof(uiAccess), &length);
    CloseHandle(token);
    Wh_Log(L"Keyboard helper PID=%lu: elevated=%lu, UIAccess=%lu",
           GetCurrentProcessId(), elevation.TokenIsElevated, uiAccess);
    return (readElevation && elevation.TokenIsElevated) ||
           (readUiAccess && uiAccess);
}

BOOL WhTool_ModInit() {
    g_showTaskbar = Wh_GetIntSetting(L"showTaskbarWithSearch") != 0;
    g_hasElevatedInputAccess = HasElevatedInputAccess();
    g_searchStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_searchReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_searchRequests = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_searchStop || !g_searchReady || !g_searchRequests) {
        Cleanup();
        return FALSE;
    }
    g_searchThread = CreateThread(nullptr, 0, SearchThread, nullptr, 0, nullptr);
    if (!g_searchThread) {
        Cleanup();
        return FALSE;
    }
    WaitForSingleObject(g_searchReady, INFINITE);
    if (!g_searchInitialized) {
        Cleanup();
        return FALSE;
    }
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_ready) {
        Cleanup();
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Cleanup();
        return FALSE;
    }
    WaitForSingleObject(g_ready, INFINITE);
    if (!g_hooksInstalled) {
        Cleanup();
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    Cleanup();
}

void WhTool_ModSettingsChanged() {
    // Read on the hook thread; this callback runs on another thread.
    g_showTaskbar = Wh_GetIntSetting(L"showTaskbarWithSearch") != 0;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool-mod launcher. The default CreateProcessInternalW path follows
// the wiki snippet. The optional elevation path runs on a background thread.
// For mods which don't need to inject to other
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

void LaunchStandardHelper() {
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
    WCHAR commandLine[MAX_PATH + 2 +
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
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
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

bool IsToolHelperRunning() {
    HANDLE helper = OpenMutexW(SYNCHRONIZE, FALSE, L"windhawk-tool-mod_" WH_MOD_ID);
    if (helper) {
        CloseHandle(helper);
        return true;
    }
    // A higher-integrity helper may deny access; it still exists.
    return GetLastError() == ERROR_ACCESS_DENIED;
}

DWORD WINAPI OptionalElevationThread(void*) {
    // Hold a separate reservation through the prompt and helper startup. This
    // prevents two launcher instances from showing concurrent UAC prompts.
    HANDLE reservation = CreateMutexW(
        nullptr, FALSE, L"Local\\Windhawk.Search.Launch_" WH_MOD_ID);
    if (!reservation) {
        Wh_Log(L"Could not reserve elevated helper launch");
        return 0;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS || IsToolHelperRunning() ||
        WaitForSingleObject(g_elevationThreadDone, 0) == WAIT_OBJECT_0) {
        CloseHandle(reservation);
        return 0;
    }
    WCHAR path[MAX_PATH];
    DWORD pathLength = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!pathLength || pathLength == ARRAYSIZE(path)) {
        CloseHandle(reservation);
        return 0;
    }
    WCHAR parameters[sizeof(L"-tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)];
    swprintf_s(parameters, L"-tool-mod \"%s\"", WH_MOD_ID);
    SHELLEXECUTEINFOW launch = {};
    launch.cbSize = sizeof(launch);
    launch.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    launch.lpVerb = L"runas";
    launch.lpFile = path;
    launch.lpParameters = parameters;
    launch.nShow = SW_HIDE;
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const BOOL launched = ShellExecuteExW(&launch);
    const DWORD error = launched ? ERROR_SUCCESS : GetLastError();
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    if (!launched && WaitForSingleObject(g_elevationThreadDone, 0) != WAIT_OBJECT_0) {
        Wh_Log(L"Elevation cancelled or unavailable (error %lu); using normal helper", error);
        LaunchStandardHelper();
    }
    // Process creation can finish before Windhawk loads the mod into it. Keep
    // the reservation until the helper owns its mutex (or startup times out).
    const ULONGLONG deadline = GetTickCount64() + 10000;
    while (GetTickCount64() < deadline && !IsToolHelperRunning() &&
           WaitForSingleObject(g_elevationThreadDone, 50) == WAIT_TIMEOUT) {
        if (launched && launch.hProcess &&
            WaitForSingleObject(launch.hProcess, 0) == WAIT_OBJECT_0) {
            break;
        }
    }
    if (launch.hProcess) {
        CloseHandle(launch.hProcess);
    }
    CloseHandle(reservation);
    return 0;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }
    if (!Wh_GetIntSetting(L"supportAdministratorWindows")) {
        LaunchStandardHelper();
        return;
    }
    if (IsToolHelperRunning()) {
        return;
    }
    g_elevationThreadDone = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_elevationThreadDone) {
        g_elevationThread = CreateThread(nullptr, 0, OptionalElevationThread,
                                         nullptr, 0, nullptr);
    }
    if (!g_elevationThread) {
        Wh_Log(L"Could not start elevation worker; using normal helper");
        if (g_elevationThreadDone) {
            CloseHandle(g_elevationThreadDone);
            g_elevationThreadDone = nullptr;
        }
        LaunchStandardHelper();
    }
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        if (g_elevationThreadDone) {
            SetEvent(g_elevationThreadDone);
        }
        if (g_elevationThread) {
            // The DLL must remain mapped until its worker has returned.
            WaitForSingleObject(g_elevationThread, INFINITE);
            CloseHandle(g_elevationThread);
            g_elevationThread = nullptr;
        }
        if (g_elevationThreadDone) {
            CloseHandle(g_elevationThreadDone);
            g_elevationThreadDone = nullptr;
        }
        return;
    }
    WhTool_ModUninit();
    ExitProcess(0);
}
