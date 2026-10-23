// ==WindhawkMod==
// @id              window-sync
// @name            Window Sync
// @name:fr-FR      Synchroniser les fenêtres
// @description     One shortcut gives all windows the size of the active one, another stacks them all behind it, lined up on its center.
// @description:fr-FR Un raccourci donne à toutes les fenêtres la taille de la fenêtre active, un autre les range toutes derrière elle, alignées sur son centre.
// @version         1.0
// @author          Jules
// @github          https://github.com/Topin5886
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32 -ldwmapi -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Sync

Two shortcuts to tidy up your windows.

## Give all windows the same size

**Win+Alt+S**: every open window takes the **size of the active window**.
Windows keep their position, unless they have to move to stay on their screen.

The size is measured on the visible frame of the windows, not on the invisible
resize borders Windows adds around most apps, so different apps really end up
the same size. With several monitors, the size follows the scaling (DPI) of
each monitor. A fixed size, in percent of the screen, can be used instead in
the settings.

![All the windows have the same size](https://raw.githubusercontent.com/Topin5886/windhawk-window-sync/main/docs/same-size.gif)

## Stack all windows behind the active one

Move a window where you want it, then press **Win+Alt+C**: all the other
windows line up **behind it, on its center** (same monitor). Each window keeps
its size. Maximized windows get back their normal size and line up too.

Tip: Win+Alt+S, then Win+Alt+C, puts all windows exactly behind the active one.

![Move a window, press the shortcut, and every other window lines up behind it](https://raw.githubusercontent.com/Topin5886/windhawk-window-sync/main/docs/stack-behind.gif)

## Windows which are left alone

Fullscreen windows (games, videos), minimized windows, windows on other virtual
desktops, dialogs, tool windows and the taskbar are never moved. Win+Alt+S also
leaves maximized windows alone, and Win+Alt+C leaves always-on-top windows
alone. Windows of apps running as administrator can't be moved (a Windows
security rule).

When Win+Alt+C brings a window from a monitor with a different scaling (DPI),
the app resizes itself for the new monitor after the move, so it may end up
slightly off-center.

## Shortcuts

Both shortcuts can be changed in the settings, and must include at least one of
Win, Ctrl, Alt or Shift. A shortcut that is already used by another program
isn't taken over: the mod log says so, and another one can be picked. To take
over such a shortcut anyway (for example Win+Alt+T, which belongs to the Xbox
Game Bar's recording timer), turn on **Take over shortcuts used by other
programs**.

## Title bar menu

With the [Resize From Center](https://windhawk.net/mods/resize-from-center)
mod, both commands are also in the title bar menu of every window (right-click
the title bar, or Alt+Space).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- syncHotkey:
  - win: true
    $name: Win
  - ctrl: false
    $name: Ctrl
  - alt: true
    $name: Alt
  - shift: false
    $name: Shift
  - key: S
    $name: Key
    $description: A letter, a digit, F1 to F24, or Space, Enter, Home, End, Insert, Delete, PageUp, PageDown, Up, Down, Left, Right
  $name: Shortcut to give all windows the same size
- stackHotkey:
  - win: true
    $name: Win
  - ctrl: false
    $name: Ctrl
  - alt: true
    $name: Alt
  - shift: false
    $name: Shift
  - key: C
    $name: Key
    $description: A letter, a digit, F1 to F24, or Space, Enter, Home, End, Insert, Delete, PageUp, PageDown, Up, Down, Left, Right
  $name: Shortcut to stack all windows behind the active one
- overrideTakenShortcuts: false
  $name: Take over shortcuts used by other programs
  $description: Reads the shortcuts directly from the keyboard, before other programs. The original action of a taken shortcut stops working while the mod runs
- sizeSource: active
  $name: Common size
  $options:
  - active: Size of the active window
  - fixed: Fixed size (percent of the screen)
- widthPercent: 60
  $name: Fixed width (percent of the screen)
  $description: Used with the fixed size, or when the active window can't be used (desktop selected, a maximized window, or a window that can't be resized)
- heightPercent: 70
  $name: Fixed height (percent of the screen)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <string>
#include <vector>

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

// Found by the Resize From Center mod (resize-from-center.wh.cpp), which
// forwards its title bar commands. Shared with that mod: keep both sides in
// sync.
constexpr wchar_t kWndClass[] = L"WindhawkWindowSync";
constexpr wchar_t kMsgSyncName[] = L"WindhawkWindowSync_Sync";
constexpr wchar_t kMsgStackName[] = L"WindhawkWindowSync_Stack";

constexpr UINT kMsgReload = WM_APP + 1;
constexpr UINT kMsgHookHotkey = WM_APP + 2;  // wParam: hotkey id
constexpr int kSyncHotkeyId = 1;
constexpr int kStackHotkeyId = 2;
constexpr ULONG_PTR kOwnInputMarker = 0x57534E43;  // 'WSNC' in dwExtraInfo

struct Hotkey {
    UINT mods = 0;
    UINT vk = 0;
    std::wstring text;  // for the log
};

struct Settings {
    Hotkey syncHotkey;
    Hotkey stackHotkey;
    bool overrideTakenShortcuts = false;
    bool sizeFromActive = true;
    int widthPercent = 60;
    int heightPercent = 70;
};

// Written on Windhawk's thread, read on the window thread: copied under lock.
Settings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;

Settings GetSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings settings = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return settings;
}

HANDLE g_windowThread = nullptr;
std::atomic<HWND> g_wnd{nullptr};
UINT g_msgSync = 0;
UINT g_msgStack = 0;

// ---------------------------------------------------------------- settings

UINT ParseKey(std::wstring key, std::wstring* display) {
    while (!key.empty() && iswspace(key.back()))
        key.pop_back();
    while (!key.empty() && iswspace(key.front()))
        key.erase(key.begin());
    for (auto& c : key)
        c = towupper(c);
    *display = key;

    if (key.size() == 1 && ((key[0] >= L'A' && key[0] <= L'Z') ||
                            (key[0] >= L'0' && key[0] <= L'9'))) {
        return key[0];
    }
    if (key.size() >= 2 && key[0] == L'F') {
        int n = _wtoi(key.c_str() + 1);
        if (n >= 1 && n <= 24)
            return VK_F1 + n - 1;
    }

    static const struct {
        const wchar_t* name;
        UINT vk;
    } kNames[] = {
        {L"SPACE", VK_SPACE}, {L"ENTER", VK_RETURN},  {L"HOME", VK_HOME},
        {L"END", VK_END},     {L"INSERT", VK_INSERT}, {L"DELETE", VK_DELETE},
        {L"PAGEUP", VK_PRIOR}, {L"PAGEDOWN", VK_NEXT}, {L"UP", VK_UP},
        {L"DOWN", VK_DOWN},   {L"LEFT", VK_LEFT},     {L"RIGHT", VK_RIGHT},
    };
    for (const auto& k : kNames) {
        if (key == k.name)
            return k.vk;
    }
    return 0;
}

Hotkey LoadHotkey(PCWSTR prefix) {
    static const struct {
        PCWSTR setting;
        UINT mod;
        PCWSTR text;
    } kMods[] = {
        {L"win", MOD_WIN, L"Win+"},
        {L"ctrl", MOD_CONTROL, L"Ctrl+"},
        {L"alt", MOD_ALT, L"Alt+"},
        {L"shift", MOD_SHIFT, L"Shift+"},
    };

    Hotkey hotkey;
    for (const auto& m : kMods) {
        if (Wh_GetIntSetting(L"%s.%s", prefix, m.setting)) {
            hotkey.mods |= m.mod;
            hotkey.text += m.text;
        }
    }
    auto key = WindhawkUtils::StringSetting::make(L"%s.key", prefix);
    std::wstring keyDisplay;
    hotkey.vk = ParseKey(key.get(), &keyDisplay);
    hotkey.text += keyDisplay;
    if (!hotkey.vk || !hotkey.mods) {
        // A key without modifier would swallow normal typing.
        Wh_Log(L"Shortcut %s disabled: it needs a valid key and a modifier",
               hotkey.text.c_str());
        hotkey.vk = 0;
    }
    return hotkey;
}

void LoadSettings() {
    Settings settings;
    settings.syncHotkey = LoadHotkey(L"syncHotkey");
    settings.stackHotkey = LoadHotkey(L"stackHotkey");
    settings.overrideTakenShortcuts = Wh_GetIntSetting(L"overrideTakenShortcuts");

    auto source = WindhawkUtils::StringSetting::make(L"sizeSource");
    settings.sizeFromActive = wcscmp(source, L"fixed") != 0;

    auto clampPercent = [](int v) { return std::clamp(v, 10, 100); };
    settings.widthPercent = clampPercent(Wh_GetIntSetting(L"widthPercent"));
    settings.heightPercent = clampPercent(Wh_GetIntSetting(L"heightPercent"));

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(settings);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

// ---------------------------------------------------------- window helpers

bool IsCloaked(HWND hWnd) {
    int cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked;
}

RECT GetVisibleFrame(HWND hWnd) {
    RECT rc{};
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc,
                                     sizeof(rc)))) {
        GetWindowRect(hWnd, &rc);
    }
    return rc;
}

UINT GetMonitorDpi(HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = 96;
    }
    return dpiX;
}

bool IsShellWindow(HWND hWnd) {
    wchar_t cls[64]{};
    GetClassNameW(hWnd, cls, ARRAYSIZE(cls));
    for (PCWSTR skip :
         {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
          L"Windows.UI.Core.CoreWindow"}) {
        if (wcscmp(cls, skip) == 0)
            return true;
    }
    return false;
}

bool IsArrangeable(HWND hWnd, bool allowMaximized) {
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd))
        return false;
    bool maximized = IsZoomed(hWnd);
    if (maximized && !allowMaximized)
        return false;

    LONG style = GetWindowLongW(hWnd, GWL_STYLE);
    LONG exStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);
    if (style & WS_CHILD)
        return false;
    if (exStyle & WS_EX_NOACTIVATE)
        return false;
    if (!(exStyle & WS_EX_APPWINDOW) &&
        ((exStyle & WS_EX_TOOLWINDOW) || GetWindow(hWnd, GW_OWNER))) {
        return false;  // tool windows, dialogs and other owned popups
    }
    if ((style & WS_CAPTION) != WS_CAPTION && !(style & WS_THICKFRAME)) {
        return false;
    }
    if (IsShellWindow(hWnd) || IsCloaked(hWnd) || IsHungAppWindow(hWnd))
        return false;

    RECT wr;
    if (!GetWindowRect(hWnd, &wr) || IsRectEmpty(&wr))
        return false;

    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &mi);
    if (!maximized && wr.left <= mi.rcMonitor.left &&
        wr.top <= mi.rcMonitor.top && wr.right >= mi.rcMonitor.right &&
        wr.bottom >= mi.rcMonitor.bottom) {
        return false;  // fullscreen
    }
    return true;
}

BOOL CALLBACK CollectSyncable(HWND hWnd, LPARAM lParam) {
    if (IsArrangeable(hWnd, false))
        ((std::vector<HWND>*)lParam)->push_back(hWnd);
    return TRUE;
}

BOOL CALLBACK CollectStackable(HWND hWnd, LPARAM lParam) {
    if (IsArrangeable(hWnd, true))
        ((std::vector<HWND>*)lParam)->push_back(hWnd);
    return TRUE;
}

// The window a command applies to: the clicked or active window, or the
// main window of the active dialog.
HWND ResolveReference(HWND reference) {
    if (!reference)
        reference = GetForegroundWindow();
    return reference ? GetAncestor(reference, GA_ROOTOWNER) : nullptr;
}

struct FrameInfo {
    RECT frame;   // visible frame
    RECT border;  // invisible resize borders around it (left/top/right/bottom)
};

FrameInfo GetFrameInfo(HWND hWnd) {
    FrameInfo info{};
    RECT wr;
    GetWindowRect(hWnd, &wr);
    info.frame = GetVisibleFrame(hWnd);
    info.border.left = std::max(0L, info.frame.left - wr.left);
    info.border.top = std::max(0L, info.frame.top - wr.top);
    info.border.right = std::max(0L, wr.right - info.frame.right);
    info.border.bottom = std::max(0L, wr.bottom - info.frame.bottom);
    return info;
}

// ------------------------------------------------------------ arrangement

// Same size for all windows. Each window keeps its top-left corner, unless
// it has to move to stay on its screen.
void SyncAllWindows(HWND reference) {
    reference = ResolveReference(reference);
    Settings settings = GetSettings();

    // Common size in 96-DPI units, taken from the reference window.
    bool fromReference = false;
    double logicalW = 0, logicalH = 0;
    if (settings.sizeFromActive && reference &&
        IsArrangeable(reference, false) &&
        (GetWindowLongW(reference, GWL_STYLE) & WS_THICKFRAME)) {
        RECT fr = GetVisibleFrame(reference);
        UINT dpi = GetMonitorDpi(
            MonitorFromWindow(reference, MONITOR_DEFAULTTONEAREST));
        logicalW = (fr.right - fr.left) * 96.0 / dpi;
        logicalH = (fr.bottom - fr.top) * 96.0 / dpi;
        fromReference = true;
    }

    std::vector<HWND> windows;
    EnumWindows(CollectSyncable, (LPARAM)&windows);

    int count = 0;
    for (HWND hWnd : windows) {
        if (!(GetWindowLongW(hWnd, GWL_STYLE) & WS_THICKFRAME))
            continue;

        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{sizeof(mi)};
        GetMonitorInfoW(monitor, &mi);
        const RECT& work = mi.rcWork;
        int workW = work.right - work.left;
        int workH = work.bottom - work.top;

        FrameInfo info = GetFrameInfo(hWnd);
        const RECT& fr = info.frame;
        const RECT& b = info.border;

        int w, h;
        if (fromReference) {
            UINT dpi = GetMonitorDpi(monitor);
            w = (int)std::lround(logicalW * dpi / 96.0);
            h = (int)std::lround(logicalH * dpi / 96.0);
        } else {
            w = workW * settings.widthPercent / 100;
            h = workH * settings.heightPercent / 100;
        }
        w = std::min(w, workW);
        h = std::min(h, workH);

        int x = std::clamp<int>(fr.left, work.left, work.right - w);
        int y = std::clamp<int>(fr.top, work.top, work.bottom - h);

        if (SetWindowPos(hWnd, nullptr, x - b.left, y - b.top,
                         w + b.left + b.right, h + b.top + b.bottom,
                         SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE |
                             SWP_ASYNCWINDOWPOS)) {
            count++;
        }
    }
    Wh_Log(L"Synced %d window(s)", count);
}

// Restores a maximized window to its normal size, centered on (cx, cy),
// without waiting for the window's thread: both requests are posted to it
// and processed in order.
bool RestoreCenteredOn(HWND hWnd, int cx, int cy, const RECT& work) {
    WINDOWPLACEMENT wp{sizeof(wp)};
    if (!GetWindowPlacement(hWnd, &wp))
        return false;

    int w = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
    int h = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
    int x = cx - w / 2;
    int y = std::max<int>(cy - h / 2, work.top);

    ShowWindowAsync(hWnd, SW_SHOWNOACTIVATE);
    return SetWindowPos(hWnd, nullptr, x, y, w, h,
                        SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE |
                            SWP_ASYNCWINDOWPOS);
}
// The window others stack behind: any visible app window, even maximized or
// fullscreen, but not the desktop or the taskbar.
bool IsStackReference(HWND hWnd) {
    return hWnd && IsWindowVisible(hWnd) && !IsIconic(hWnd) &&
           !IsCloaked(hWnd) && !IsShellWindow(hWnd);
}

// Centers every other window behind the reference window (same center).
// Each window keeps its size (maximized ones get their normal size back);
// only its title bar is kept on screen.
void StackBehindWindow(HWND reference) {
    reference = ResolveReference(reference);
    if (!IsStackReference(reference)) {
        Wh_Log(L"Stack: no usable active window");
        return;
    }

    RECT ref = GetVisibleFrame(reference);
    int cx = (ref.left + ref.right) / 2;
    int cy = (ref.top + ref.bottom) / 2;

    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromWindow(reference, MONITOR_DEFAULTTONEAREST),
                    &mi);
    const RECT& work = mi.rcWork;

    std::vector<HWND> windows;
    EnumWindows(CollectStackable, (LPARAM)&windows);

    int count = 0;
    bool restoredAny = false;
    for (HWND hWnd : windows) {
        if (hWnd == reference)
            continue;
        // Always-on-top windows would end up in front of it, not behind.
        if (GetWindowLongW(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)
            continue;

        if (IsZoomed(hWnd)) {
            if (RestoreCenteredOn(hWnd, cx, cy, work)) {
                count++;
                restoredAny = true;
            }
            continue;
        }

        FrameInfo info = GetFrameInfo(hWnd);
        const RECT& fr = info.frame;
        int w = fr.right - fr.left;
        int h = fr.bottom - fr.top;

        int x = cx - w / 2;
        int y = std::max<int>(cy - h / 2, work.top);

        // The stacking order is kept, so the active window stays on top.
        if (SetWindowPos(hWnd, nullptr, x - info.border.left,
                         y - info.border.top, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                             SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS)) {
            count++;
        }
    }

    // A restored window must not end up above the active window.
    if (restoredAny) {
        SetWindowPos(reference, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                         SWP_ASYNCWINDOWPOS);
    }
    Wh_Log(L"Stacked %d window(s) behind the active window", count);
}

// ---------------------------------------------- taking over shortcuts
//
// Only with "Take over shortcuts used by other programs": a low-level
// keyboard hook on its own thread sees the keys before the hotkeys of other
// programs. The hook only posts a message, the work is done on the window
// thread.

struct HookHotkey {
    std::atomic<UINT> mods{0};
    std::atomic<UINT> vk{0};
};
HookHotkey g_hookHotkeys[2];  // sync, stack
HANDLE g_hookThread = nullptr;
DWORD g_hookThreadId = 0;
UINT g_swallowedVk = 0;  // hook thread only

UINT CurrentModifiers() {
    auto down = [](int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; };
    UINT mods = 0;
    if (down(VK_LWIN) || down(VK_RWIN))
        mods |= MOD_WIN;
    if (down(VK_CONTROL))
        mods |= MOD_CONTROL;
    if (down(VK_MENU))
        mods |= MOD_ALT;
    if (down(VK_SHIFT))
        mods |= MOD_SHIFT;
    return mods;
}

void SendMaskKey() {
    // An unassigned key between the modifiers' press and release stops
    // Start (Win) or the menu bar (Alt) from opening.
    INPUT in[2]{};
    in[0].type = in[1].type = INPUT_KEYBOARD;
    in[0].ki.wVk = in[1].ki.wVk = 0xE8;
    in[0].ki.dwExtraInfo = in[1].ki.dwExtraInfo = kOwnInputMarker;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const auto* key = (const KBDLLHOOKSTRUCT*)lParam;
        bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        if (key->dwExtraInfo != kOwnInputMarker && key->vkCode) {
            if (key->vkCode == g_swallowedVk) {
                // Auto-repeat and release of a key we already handled.
                if (!down)
                    g_swallowedVk = 0;
                return 1;
            }
            if (down) {
                UINT mods = CurrentModifiers();
                for (int i = 0; i < 2; i++) {
                    if (g_hookHotkeys[i].vk == key->vkCode &&
                        g_hookHotkeys[i].mods == mods) {
                        g_swallowedVk = key->vkCode;
                        SendMaskKey();
                        PostMessageW(g_wnd, kMsgHookHotkey,
                                     i == 0 ? kSyncHotkeyId : kStackHotkeyId,
                                     0);
                        return 1;
                    }
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI HookThread(LPVOID readyEvent) {
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);  // create the queue
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                   (HINSTANCE)&__ImageBase, 0);
    if (!hook)
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
    SetEvent((HANDLE)readyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        DispatchMessageW(&msg);
    }

    if (hook)
        UnhookWindowsHookEx(hook);
    return 0;
}

void StopHookThread() {
    if (!g_hookThread)
        return;
    PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_hookThread, INFINITE);
    CloseHandle(g_hookThread);
    g_hookThread = nullptr;
}

void StartHookThread() {
    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hookThread =
        CreateThread(nullptr, 0, HookThread, ready, 0, &g_hookThreadId);
    if (g_hookThread)
        WaitForSingleObject(ready, INFINITE);
    CloseHandle(ready);
}

// Runs on the window thread.
void ApplyShortcuts(HWND hWnd) {
    Settings settings = GetSettings();
    UnregisterHotKey(hWnd, kSyncHotkeyId);
    UnregisterHotKey(hWnd, kStackHotkeyId);
    StopHookThread();

    const Hotkey* hotkeys[] = {&settings.syncHotkey, &settings.stackHotkey};
    const int ids[] = {kSyncHotkeyId, kStackHotkeyId};

    if (settings.overrideTakenShortcuts) {
        for (int i = 0; i < 2; i++) {
            g_hookHotkeys[i].mods = hotkeys[i]->mods;
            g_hookHotkeys[i].vk = hotkeys[i]->vk;
        }
        StartHookThread();
        return;
    }

    for (int i = 0; i < 2; i++) {
        if (!hotkeys[i]->vk)
            continue;
        if (!RegisterHotKey(hWnd, ids[i], hotkeys[i]->mods | MOD_NOREPEAT,
                            hotkeys[i]->vk)) {
            Wh_Log(L"Shortcut %s is already used by another program (error "
                   L"%u). Pick another one, or turn on \"Take over shortcuts "
                   L"used by other programs\"",
                   hotkeys[i]->text.c_str(), GetLastError());
        }
    }
}

// ------------------------------------------------------------ window thread

void RunCommand(int hotkeyId, HWND reference) {
    if (hotkeyId == kSyncHotkeyId)
        SyncAllWindows(reference);
    else if (hotkeyId == kStackHotkeyId)
        StackBehindWindow(reference);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_HOTKEY:
        case kMsgHookHotkey:
            RunCommand((int)wParam, nullptr);
            return 0;
        case kMsgReload:
            ApplyShortcuts(hWnd);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            UnregisterHotKey(hWnd, kSyncHotkeyId);
            UnregisterHotKey(hWnd, kStackHotkeyId);
            StopHookThread();
            PostQuitMessage(0);
            return 0;
    }
    if (uMsg == g_msgSync && g_msgSync) {
        RunCommand(kSyncHotkeyId, (HWND)wParam);
        return 0;
    }
    if (uMsg == g_msgStack && g_msgStack) {
        RunCommand(kStackHotkeyId, (HWND)wParam);
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI WindowThread(LPVOID readyEvent) {
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = (HINSTANCE)&__ImageBase;
    wc.lpszClassName = kWndClass;
    RegisterClassW(&wc);

    HWND wnd = CreateWindowExW(0, kWndClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                               nullptr, wc.hInstance, nullptr);
    g_wnd = wnd;
    if (wnd) {
        // If Windhawk runs elevated (portable, started as administrator), let
        // the title bar commands of Resize From Center reach this window.
        ChangeWindowMessageFilterEx(wnd, g_msgSync, MSGFLT_ALLOW, nullptr);
        ChangeWindowMessageFilterEx(wnd, g_msgStack, MSGFLT_ALLOW, nullptr);
        ApplyShortcuts(wnd);
    }
    SetEvent((HANDLE)readyEvent);

    MSG msg;
    while (wnd && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_wnd = nullptr;
    UnregisterClassW(kWndClass, wc.hInstance);
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    LoadSettings();
    g_msgSync = RegisterWindowMessageW(kMsgSyncName);
    g_msgStack = RegisterWindowMessageW(kMsgStackName);

    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_windowThread = CreateThread(nullptr, 0, WindowThread, ready, 0, nullptr);
    if (g_windowThread)
        WaitForSingleObject(ready, INFINITE);
    CloseHandle(ready);
    return g_wnd != nullptr;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_wnd)
        PostMessageW(g_wnd, kMsgReload, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    if (g_windowThread) {
        if (g_wnd)
            PostMessageW(g_wnd, WM_CLOSE, 0, 0);
        WaitForSingleObject(g_windowThread, INFINITE);
        CloseHandle(g_windowThread);
        g_windowThread = nullptr;
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
