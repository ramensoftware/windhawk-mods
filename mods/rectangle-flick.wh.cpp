// ==WindhawkMod==
// @id              rectangle-flick
// @name            Rectangle Flick
// @description     Rectangle Pro-inspired window management: hold Ctrl+Alt, preview with a mouse gesture, and release to snap.
// @version         1.0.0
// @author          kr3mil
// @github          https://github.com/kr3mil
// @homepage        https://github.com/kr3mil/rectangle-flick
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Rectangle Flick

Bring [Rectangle Pro](https://rectangleapp.com/pro/)-style mouse gestures to
Windows 11. Inspired by its **Window Throw** interaction, Rectangle Flick lets
you preview and arrange the focused window without clicking or dragging it.
This is an independent Windhawk mod, not affiliated with Rectangle Pro or its
developer. It recreates the core interaction, not Rectangle Pro's full feature set.

[Source code, releases and issue reports](https://github.com/kr3mil/rectangle-flick)

## How to use

1. Focus the window you want to arrange. Your cursor can be anywhere on screen.
2. Hold **Ctrl + Left Alt**, in either order. Either Ctrl key works.
3. A translucent red circle marks the starting point and stays fixed there.
4. Move the mouse away from the circle to preview a position. There is no time limit.
5. Release either modifier to apply the preview.

Change direction while holding the keys to choose another position. Move back
inside the starting safe area to clear the preview. Press **Esc**, click, scroll,
or switch windows to cancel. Release and press the chord again to start over.
Right Alt (AltGr) is excluded so international text entry does not activate it.

| Direction | Position |
| --- | --- |
| Left / right | Left / right half |
| Up | Maximise |
| Down | Centred at 70% of the monitor work area |
| Up-left / up-right | Top-left / top-right quarter |
| Down-left / down-right | Bottom-left / bottom-right quarter |

## Settings

- **Minimum distance:** the safe area around the starting point, in physical pixels.
- **Diagonal balance:** how balanced horizontal and vertical movement must be
  to select a corner. Higher values make diagonal sectors narrower.
- **Return cursor:** optionally return the cursor to the circle after applying.
- **Cooldown:** minimum time before a new gesture can start.
- **Excluded applications:** executable names separated by semicolons.

The defaults are 50 pixels, 58% diagonal balance (approximately 30–60 degrees
within each quadrant), a 150 ms cooldown, and cursor return off.

## Compatibility

Designed for Windows 11 and ordinary resizable desktop windows. Uses the focused
window's monitor and respects its taskbar work area. The indicator and preview
are click-through and do not take focus. All keyboard and mouse input passes
through, so application shortcuts using Ctrl+Alt can still run.

Fullscreen windows, system surfaces, non-resizable windows, and excluded apps
are skipped. Add your windowed games to the exclusions. Elevated applications
can reject requests from Explorer, and application minimum sizes or custom
frames can cause the actual result to differ from the preview. Down centres
and resizes; it does not restore an earlier window position.

Changes to settings reload the mod. Disabling it removes the overlays and hooks
and leaves windows in their current positions. If Windows stops delivering
gestures, disable and re-enable the mod. Thirds, monitor transfer, position history,
and custom direction mappings are not included in this release.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- distance: 50
  $name: Minimum flick distance (physical pixels, 10–500)
- diagonalRatio: 58
  $name: Diagonal balance (percent, 20–95; higher means narrower diagonals)
- restoreCursor: false
  $name: Return cursor to gesture start
- cooldown: 150
  $name: Minimum time between gestures (milliseconds, 0–2000)
- excludedApps: "windhawk.exe;VSCodium.exe;StartMenuExperienceHost.exe;SearchHost.exe;ShellExperienceHost.exe;ShellHost.exe;LockApp.exe;GameBar.exe;Terraria.exe;RuneLite.exe"
  $name: Excluded executable names (semicolon separated; add windowed games here)
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <dwmapi.h>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <string>

namespace {
struct Settings {
    int distance = 50, diagonalRatio = 58, cooldown = 150;
    bool restoreCursor = false;
    std::wstring excluded;
} settings; // Immutable while threads run; changes request a Windhawk reload.

enum class Direction { Left, Right, Up, Down, UpLeft, UpRight, DownLeft, DownRight };
Direction Classify(long dx, long dy, int ratio) {
    auto x = std::abs(static_cast<int64_t>(dx));
    auto y = std::abs(static_cast<int64_t>(dy));
    if (std::min(x, y) * 100 >= std::max(x, y) * ratio) {
        if (dy < 0) return dx < 0 ? Direction::UpLeft : Direction::UpRight;
        return dx < 0 ? Direction::DownLeft : Direction::DownRight;
    }
    if (x > y) return dx < 0 ? Direction::Left : Direction::Right;
    return dy < 0 ? Direction::Up : Direction::Down;
}

RECT Region(RECT work, Direction direction) {
    LONG midX = work.left + (work.right - work.left) / 2;
    LONG midY = work.top + (work.bottom - work.top) / 2;
    switch (direction) {
    case Direction::Left: work.right = midX; break;
    case Direction::Right: work.left = midX; break;
    case Direction::UpLeft: work.right = midX; work.bottom = midY; break;
    case Direction::UpRight: work.left = midX; work.bottom = midY; break;
    case Direction::DownLeft: work.right = midX; work.top = midY; break;
    case Direction::DownRight: work.left = midX; work.top = midY; break;
    case Direction::Down: {
        LONG w = (work.right - work.left) * 7 / 10;
        LONG h = (work.bottom - work.top) * 7 / 10;
        work.left += (work.right - work.left - w) / 2;
        work.top += (work.bottom - work.top - h) / 2;
        work.right = work.left + w;
        work.bottom = work.top + h;
        break;
    }
    case Direction::Up: break;
    }
    return work;
}

struct Request { HWND window; DWORD pid, tid; POINT origin; Direction direction; };
Request request{}; // Single slot, published through actionEvent; busy guards reuse.
std::atomic<bool> busy{false};
HANDLE stopEvent, actionEvent, visualEvent, visualReadyEvent, readyEvent, inputThread, actionThread, singleton;
bool hooksReady, visualsReady;
HMODULE modModule;
HHOOK keyboardHook, mouseHook;
struct Gesture {
    bool leftCtrl = false, rightCtrl = false, leftAlt = false, rightAlt = false;
    bool armed = false, selected = false;
    Direction direction = Direction::Left;
    POINT origin{};
    HWND window = nullptr;
    DWORD pid = 0, tid = 0;
    ULONGLONG started = 0, lastAction = 0;
} gesture; // Owned exclusively by input thread.

struct VisualState { Request target{}; bool active = false, selected = false; };
VisualState visualState;
SRWLOCK visualLock = SRWLOCK_INIT;

bool ChordHeld(const Gesture& g) {
    return (g.leftCtrl || g.rightCtrl) && g.leftAlt && !g.rightAlt;
}
void PublishVisual() {
    // The lock protects only a small copy; never hold it across a Win32 call.
    VisualState state{{gesture.window, gesture.pid, gesture.tid, gesture.origin, gesture.direction},
                      gesture.armed, gesture.selected};
    AcquireSRWLockExclusive(&visualLock);
    visualState = state;
    ReleaseSRWLockExclusive(&visualLock);
    SetEvent(visualEvent);
}
void CancelGesture() {
    gesture.armed = gesture.selected = false;
    PublishVisual();
}
void CALLBACK ForegroundChanged(HWINEVENTHOOK, DWORD, HWND window, LONG, LONG, DWORD, DWORD) {
    if (gesture.armed && window != gesture.window) CancelGesture();
}

bool ButtonsDown() {
    for (int key : {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2})
        if (GetAsyncKeyState(key) & 0x8000) return true;
    return false;
}

bool IsEligibleWindow(HWND window) {
    if (!window || !IsWindow(window) || !IsWindowVisible(window) || IsIconic(window) ||
        window == GetDesktopWindow() || window == GetShellWindow() ||
        GetAncestor(window, GA_ROOT) != window) return false;
    auto style = GetWindowLongPtrW(window, GWL_STYLE);
    auto ex = GetWindowLongPtrW(window, GWL_EXSTYLE);
    if ((style & (WS_CHILD | WS_DISABLED)) || !(style & WS_THICKFRAME) ||
        (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) || GetWindow(window, GW_OWNER)) return false;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked)
        return false;
    WCHAR cls[128]{};
    GetClassNameW(window, cls, ARRAYSIZE(cls));
    for (auto name : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", L"Progman", L"WorkerW",
                      L"tooltips_class32", L"#32768", L"XamlExplorerHostIslandWindow"})
        if (!_wcsicmp(cls, name)) return false;
    DWORD pid;
    GetWindowThreadProcessId(window, &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    WCHAR path[32768];
    DWORD size = ARRAYSIZE(path);
    BOOL found = QueryFullProcessImageNameW(process, 0, path, &size);
    CloseHandle(process);
    if (!found) return false;
    const WCHAR* base = wcsrchr(path, L'\\');
    base = base ? base + 1 : path;
    size_t start = 0;
    while (start < settings.excluded.size()) {
        size_t end = settings.excluded.find(L';', start);
        if (end == std::wstring::npos) end = settings.excluded.size();
        auto name = settings.excluded.substr(start, end - start);
        auto first = name.find_first_not_of(L" \t");
        if (first != std::wstring::npos) {
            name = name.substr(first, name.find_last_not_of(L" \t") - first + 1);
            if (!_wcsicmp(base, name.c_str())) return false;
        }
        start = end + 1;
    }
    // Conservatively skip fullscreen windows, including borderless games.
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    RECT bounds{};
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &info) ||
        !GetWindowRect(window, &bounds)) return false;
    if (!IsZoomed(window) && bounds.left <= info.rcMonitor.left && bounds.top <= info.rcMonitor.top &&
        bounds.right >= info.rcMonitor.right && bounds.bottom >= info.rcMonitor.bottom) return false;
    return true;
}

bool SameTarget(const Request& r) {
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(r.window, &pid);
    return tid == r.tid && pid == r.pid && GetForegroundWindow() == r.window;
}

void Apply(const Request& r) {
    if (!SameTarget(r) || !IsEligibleWindow(r.window)) return;
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(MonitorFromWindow(r.window, MONITOR_DEFAULTTONEAREST), &info)) return;
    if (r.direction == Direction::Up) {
        if ((GetWindowLongPtrW(r.window, GWL_STYLE) & WS_MAXIMIZEBOX) &&
            ShowWindowAsync(r.window, SW_MAXIMIZE) && settings.restoreCursor)
            SetCursorPos(r.origin.x, r.origin.y);
        return;
    }
    if (IsZoomed(r.window)) {
        ShowWindowAsync(r.window, SW_RESTORE);
        // Bounded wait only during an action. Never block the hook thread or idle-poll.
        for (int i = 0; i < 25 && IsZoomed(r.window); ++i)
            if (WaitForSingleObject(stopEvent, 10) != WAIT_TIMEOUT) return;
        if (IsZoomed(r.window)) { Wh_Log(L"Restore timed out for %p", r.window); return; }
    }
    if (!SameTarget(r) || WaitForSingleObject(stopEvent, 0) != WAIT_TIMEOUT) return;
    RECT outer{}, frame{};
    if (!GetWindowRect(r.window, &outer)) return;
    RECT target = Region(info.rcWork, r.direction);
    // Both threads use per-monitor V2 awareness: GetWindowRect and DWM bounds
    // are in physical screen coordinates, including negative monitor origins.
    if (SUCCEEDED(DwmGetWindowAttribute(r.window, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(frame)))) {
        LONG l = frame.left - outer.left, t = frame.top - outer.top;
        LONG rr = outer.right - frame.right, b = outer.bottom - frame.bottom;
        // Reject stale/implausible DWM bounds during animation.
        if (l >= 0 && t >= 0 && rr >= 0 && b >= 0 && std::max({l,t,rr,b}) <= 64) {
            target.left -= l; target.top -= t; target.right += rr; target.bottom += b;
        }
    }
    if (!SetWindowPos(r.window, nullptr, target.left, target.top,
                      target.right - target.left, target.bottom - target.top,
                      SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS | SWP_NOOWNERZORDER))
        Wh_Log(L"SetWindowPos failed for %p: %lu", r.window, GetLastError());
    else if (settings.restoreCursor && SameTarget(r))
        SetCursorPos(r.origin.x, r.origin.y);
}

LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    if (msg == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    if (msg == WM_ERASEBKGND) return 1;
    if (msg == WM_PAINT) {
        PAINTSTRUCT paint;
        HDC dc = BeginPaint(hwnd, &paint);
        RECT rc;
        GetClientRect(hwnd, &rc);
        bool circle = GetWindowLongPtrW(hwnd, GWLP_USERDATA) != 0;
        HBRUSH fill = CreateSolidBrush(circle ? RGB(240, 45, 60) : RGB(65, 145, 245));
        FillRect(dc, &rc, fill);
        DeleteObject(fill);
        if (!circle) FrameRect(dc, &rc, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
        EndPaint(hwnd, &paint);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// All overlay creation, painting and positioning belongs to the action thread.
// WS_EX_TRANSPARENT on layered windows passes input through across processes.
struct Overlays {
    HWND circle = nullptr, preview = nullptr;
    ATOM atom = 0;
    static constexpr PCWSTR className = L"Windhawk.RectangleFlick.Overlay.0.2";
    ~Overlays() {
        if (circle) DestroyWindow(circle);
        if (preview) DestroyWindow(preview);
        if (atom) UnregisterClassW(className, modModule);
    }
    bool Init() {
        WNDCLASSW cls{};
        cls.lpfnWndProc = OverlayProc;
        cls.hInstance = modModule;
        cls.lpszClassName = className;
        atom = RegisterClassW(&cls);
        if (!atom) return false;
        DWORD ex = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
        preview = CreateWindowExW(ex, className, L"Rectangle Flick preview", WS_POPUP,
                                  0, 0, 1, 1, nullptr, nullptr, modModule, nullptr);
        circle = CreateWindowExW(ex, className, L"Rectangle Flick active", WS_POPUP,
                                 0, 0, 24, 24, nullptr, nullptr, modModule, nullptr);
        if (!preview || !circle) return false;
        SetWindowLongPtrW(circle, GWLP_USERDATA, 1);
        HRGN region = CreateEllipticRgn(0, 0, 24, 24);
        if (!region) return false;
        if (!SetWindowRgn(circle, region, FALSE)) { DeleteObject(region); return false; }
        return SetLayeredWindowAttributes(circle, 0, 125, LWA_ALPHA) &&
               SetLayeredWindowAttributes(preview, 0, 65, LWA_ALPHA);
    }
    void Hide() { ShowWindow(circle, SW_HIDE); ShowWindow(preview, SW_HIDE); }
    void Update(const VisualState& state, bool eligible) {
        if (!state.active || !eligible || !SameTarget(state.target)) { Hide(); return; }
        if (state.selected) {
            MONITORINFO info{};
            info.cbSize = sizeof(info);
            if (GetMonitorInfoW(MonitorFromWindow(state.target.window, MONITOR_DEFAULTTONEAREST), &info)) {
                RECT r = Region(info.rcWork, state.target.direction);
                SetWindowPos(preview, HWND_TOPMOST, r.left, r.top, r.right-r.left, r.bottom-r.top,
                             SWP_NOACTIVATE | SWP_SHOWWINDOW);
            } else ShowWindow(preview, SW_HIDE);
        } else ShowWindow(preview, SW_HIDE);
        // Window Throw reticle: fixed at activation, matching the origin used
        // for direction classification and the safe-area distance threshold.
        SetWindowPos(circle, HWND_TOPMOST, state.target.origin.x-12, state.target.origin.y-12, 24, 24,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
};

DWORD WINAPI ActionMain(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    Overlays overlays;
    visualsReady = overlays.Init();
    if (!visualsReady) Wh_Log(L"Overlay creation failed: %lu", GetLastError());
    SetEvent(visualReadyEvent);
    if (!visualsReady) return 0;
    HANDLE events[]{stopEvent, actionEvent, visualEvent};
    HWND checkedWindow = nullptr;
    bool eligible = false;
    for (;;) {
        DWORD result = MsgWaitForMultipleObjectsEx(3, events, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) break;
        if (result == WAIT_OBJECT_0 + 1) {
            overlays.Hide();
            Apply(request);
            busy.store(false, std::memory_order_release);
        } else if (result == WAIT_OBJECT_0 + 2) {
            AcquireSRWLockShared(&visualLock);
            VisualState state = visualState;
            ReleaseSRWLockShared(&visualLock);
            if (!state.active) checkedWindow = nullptr;
            else if (checkedWindow != state.target.window) {
                checkedWindow = state.target.window;
                eligible = IsEligibleWindow(checkedWindow);
            }
            overlays.Update(state, eligible);
        }
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    return 0;
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM message, LPARAM data) {
    if (code == HC_ACTION) {
        const auto& key = *reinterpret_cast<KBDLLHOOKSTRUCT*>(data);
        if (!(key.flags & LLKHF_INJECTED)) {
            bool down = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
            if (key.vkCode == VK_LCONTROL || key.vkCode == VK_RCONTROL ||
                key.vkCode == VK_LMENU || key.vkCode == VK_RMENU) {
                bool wasHeld = ChordHeld(gesture);
                switch (key.vkCode) {
                case VK_LCONTROL: gesture.leftCtrl = down; break;
                case VK_RCONTROL: gesture.rightCtrl = down; break;
                case VK_LMENU: gesture.leftAlt = down; break;
                case VK_RMENU: gesture.rightAlt = down; break;
                }
                bool held = ChordHeld(gesture);
                if (wasHeld && !held) {
                    if (!down && gesture.armed && gesture.selected &&
                        GetForegroundWindow() == gesture.window &&
                        !busy.exchange(true, std::memory_order_acq_rel)) {
                        request = {gesture.window, gesture.pid, gesture.tid, gesture.origin, gesture.direction};
                        if (SetEvent(actionEvent)) gesture.lastAction = GetTickCount64();
                        else busy.store(false, std::memory_order_release);
                    }
                    CancelGesture();
                }
                if (held && !wasHeld) {
                    gesture.started = GetTickCount64();
                    gesture.window = GetForegroundWindow();
                    gesture.tid = GetWindowThreadProcessId(gesture.window, &gesture.pid);
                    gesture.selected = false;
                    gesture.armed = !busy.load(std::memory_order_acquire) && GetCursorPos(&gesture.origin) && !ButtonsDown() &&
                        !(GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
                        !(GetAsyncKeyState(VK_LWIN) & 0x8000) &&
                        !(GetAsyncKeyState(VK_RWIN) & 0x8000) &&
                        gesture.started - gesture.lastAction >= static_cast<ULONGLONG>(settings.cooldown);
                    PublishVisual();
                }
            } else if (down && gesture.armed) CancelGesture();
        }
    }
    return CallNextHookEx(nullptr, code, message, data);
}

LRESULT CALLBACK MouseProc(int code, WPARAM message, LPARAM data) {
    if (code == HC_ACTION && gesture.armed) {
        const auto& mouse = *reinterpret_cast<MSLLHOOKSTRUCT*>(data);
        if (!(mouse.flags & LLMHF_INJECTED)) {
            if (message != WM_MOUSEMOVE || GetForegroundWindow() != gesture.window ||
                !(GetAsyncKeyState(VK_CONTROL) & 0x8000) || !(GetAsyncKeyState(VK_LMENU) & 0x8000)) {
                CancelGesture();
            } else {
                LONG dx = mouse.pt.x - gesture.origin.x, dy = mouse.pt.y - gesture.origin.y;
                gesture.selected = int64_t(dx)*dx + int64_t(dy)*dy >= int64_t(settings.distance)*settings.distance;
                if (gesture.selected) gesture.direction = Classify(dx, dy, settings.diagonalRatio);
                PublishVisual();
            }
        }
    }
    return CallNextHookEx(nullptr, code, message, data);
}

DWORD WINAPI InputMain(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    // Starting/reloading with Ctrl held requires release before arming.
    gesture.leftCtrl = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0;
    gesture.rightCtrl = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    gesture.leftAlt = (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0;
    gesture.rightAlt = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&InputMain), &module);
    keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, module, 0);
    if (!keyboardHook) Wh_Log(L"Keyboard hook failed: %lu", GetLastError());
    mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, module, 0);
    if (!mouseHook) Wh_Log(L"Mouse hook failed: %lu", GetLastError());
    HWINEVENTHOOK foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                                   nullptr, ForegroundChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!foregroundHook) Wh_Log(L"Foreground event hook failed: %lu", GetLastError());
    hooksReady = keyboardHook && mouseHook && foregroundHook;
    SetEvent(readyEvent);
    if (hooksReady) {
        for (;;) {
            DWORD result = MsgWaitForMultipleObjectsEx(1, &stopEvent, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (result != WAIT_OBJECT_0 + 1) break;
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }
    if (mouseHook) UnhookWindowsHookEx(mouseHook);
    if (keyboardHook) UnhookWindowsHookEx(keyboardHook);
    if (foregroundHook) UnhookWinEvent(foregroundHook);
    return 0;
}

void Stop() {
    if (stopEvent) SetEvent(stopEvent);
    for (HANDLE thread : {inputThread, actionThread})
        if (thread) WaitForSingleObject(thread, INFINITE);
    for (HANDLE* handle : {&inputThread, &actionThread, &readyEvent, &visualReadyEvent, &visualEvent, &actionEvent, &stopEvent, &singleton}) {
        if (*handle) CloseHandle(*handle);
        *handle = nullptr;
    }
}
} // namespace

BOOL Wh_ModInit() {
    // One instance per logon session even when Explorer uses separate processes.
    singleton = CreateMutexW(nullptr, FALSE, L"Local\\Windhawk.RectangleFlick.0.1");
    if (!singleton) return FALSE;
    if (GetLastError() == ERROR_ALREADY_EXISTS) { Stop(); return FALSE; }
    settings.distance = std::clamp(Wh_GetIntSetting(L"distance"), 10, 500);
    settings.diagonalRatio = std::clamp(Wh_GetIntSetting(L"diagonalRatio"), 20, 95);
    settings.cooldown = std::clamp(Wh_GetIntSetting(L"cooldown"), 0, 2000);
    settings.restoreCursor = Wh_GetIntSetting(L"restoreCursor") != 0;
    PCWSTR excluded = Wh_GetStringSetting(L"excludedApps");
    if (excluded) { settings.excluded = excluded; Wh_FreeStringSetting(excluded); }
    stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    actionEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    visualEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    visualReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!stopEvent || !actionEvent || !readyEvent || !visualEvent || !visualReadyEvent) { Stop(); return FALSE; }
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&ActionMain), &modModule)) { Stop(); return FALSE; }
    actionThread = CreateThread(nullptr, 0, ActionMain, nullptr, 0, nullptr);
    if (actionThread && WaitForSingleObject(visualReadyEvent, 5000) == WAIT_OBJECT_0 && visualsReady)
        inputThread = CreateThread(nullptr, 0, InputMain, nullptr, 0, nullptr);
    if (!inputThread || WaitForSingleObject(readyEvent, 5000) != WAIT_OBJECT_0 || !hooksReady) {
        Wh_Log(L"Gesture worker startup failed: %lu", GetLastError());
        Stop();
        return FALSE;
    }
    Wh_Log(L"Rectangle Flick ready: Ctrl+Left Alt, preview then release, %d px, diagonal ratio %d%%",
           settings.distance, settings.diagonalRatio);
    return TRUE;
}

void Wh_ModUninit() { Stop(); }
BOOL Wh_ModSettingsChanged(BOOL* reload) { *reload = TRUE; return TRUE; }
