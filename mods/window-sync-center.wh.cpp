// ==WindhawkMod==
// @id              window-sync-center
// @name            Resize From Center & Window Sync
// @name:fr-FR      Redimensionner depuis le centre et synchroniser les fenêtres
// @description     Hold Alt while resizing a window to grow it from its center. Win+Alt+S gives all windows the size of the active one, Win+Alt+T stacks them behind it.
// @description:fr-FR Maintenir Alt en redimensionnant une fenêtre pour l'agrandir depuis son centre. Win+Alt+S donne à toutes les fenêtres la taille de la fenêtre active, Win+Alt+T les range derrière elle.
// @version         1.0
// @author          Jules
// @github          https://github.com/Topin5886
// @include         *
// @compilerOptions -luser32 -lcomctl32 -ldwmapi -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Resize From Center & Window Sync

Three small tools to arrange windows.

## 1. Resize from the center

While resizing a window with the mouse, **hold Alt**:

- dragging an **edge** grows (or shrinks) the window on **both opposite sides**;
- dragging a **corner** grows it on **all four sides**, its center stays in
  place.

Alt can be pressed or released in the middle of the drag: the window switches
between normal and centered resizing right away. The key can be changed in the
settings (Alt, Ctrl or Shift).

## 2. Give all windows the same size

**Win+Alt+S**: every open window takes the **size of the active window**.
Windows keep their position, unless they have to move to stay on their screen.

With several monitors, the size follows the scaling (DPI) of each monitor. A
fixed size, in percent of the screen, can be used instead in the settings.

## 3. Stack all windows behind the active one

Move a window where you want it, then press **Win+Alt+T**: all the other windows
line up **behind it, on its center** (same monitor). Each window keeps its size.
Maximized windows get back their normal size and line up too.

Tip: Win+Alt+S, then Win+Alt+T, puts all windows exactly behind the active one.

Both commands are also in the **title bar menu** of every window (right-click
the title bar, or Alt+Space).

## Windows which are left alone

Fullscreen windows (games, videos), minimized windows, windows on other virtual
desktops, dialogs, tool windows and the taskbar are never moved. Win+Alt+S also
leaves maximized windows alone.

## Notes

- The shortcuts are read directly from the keyboard, so they work even when
  another app already uses the same combination (for example, Win+Alt+T belongs
  to the Xbox Game Bar's recording timer). Both shortcuts can be changed in the
  settings, and must include at least one of Win, Ctrl, Alt or Shift.
- Windows of apps running as administrator can't be moved by the shortcuts
  (a Windows security rule).
- Resizing from the center has to run inside each app, which is why the mod is
  loaded in all processes. To exclude a program, such as a game with an
  anti-cheat, add it to the custom process exclusion list in the Advanced tab of
  this mod.

## Compatibility with AltDrag

The [AltDrag](https://windhawk.net/mods/alt-drag) mod also uses Alt by default:
holding Alt *before* clicking starts an AltDrag move. To resize from the center
while AltDrag is installed, start dragging the window border first, then press
Alt, or change the key of either mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- resizeModifier: alt
  $name: Key for resizing from the center
  $description: Hold it while dragging a window edge or corner
  $options:
  - alt: Alt
  - ctrl: Ctrl
  - shift: Shift
  - none: Disabled
- hotkey:
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
  - key: T
    $name: Key
    $description: A letter, a digit, F1 to F24, or Space, Enter, Home, End, Insert, Delete, PageUp, PageDown, Up, Down, Left, Right
  $name: Shortcut to stack all windows behind the active one
- sizeSource: active
  $name: Common size
  $options:
  - active: Size of the active window
  - fixed: Fixed size (percent of the screen)
- widthPercent: 60
  $name: Fixed width (percent of the screen)
  $description: Used with the fixed size, or when no window is active (desktop selected)
- heightPercent: 70
  $name: Fixed height (percent of the screen)
- systemMenuItem: true
  $name: Add both commands to the title bar menu
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
#include <unordered_set>
#include <vector>

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

constexpr wchar_t kSyncWndClass[] = L"WindhawkWindowSyncCenter";
constexpr wchar_t kMenuProp[] = L"WindhawkWindowSyncCenterMenu";
// System menu command IDs: < 0xF000, low 4 bits clear.
constexpr UINT kSysMenuSyncId = 0x5A70;
constexpr UINT kSysMenuGatherId = 0x5A80;
constexpr UINT kMsgSync = WM_APP + 1;  // wParam: reference window
constexpr UINT kMsgGather = WM_APP + 3;
constexpr UINT kMsgHotkey = WM_APP + 4;            // wParam: kAction*
constexpr ULONG_PTR kOwnInputMarker = 0x57534E43;  // 'WSNC' in dwExtraInfo
constexpr int kActionSync = 0;
constexpr int kActionGather = 1;

enum class ResizeModifier { None, Alt, Ctrl, Shift };

struct Hotkey {
    UINT mods = 0;
    UINT vk = 0;
    std::wstring text;  // empty when no valid key is set
};

struct Settings {
    Hotkey syncHotkey{MOD_WIN | MOD_ALT, 'S', L"Win+Alt+S"};
    Hotkey stackHotkey{MOD_WIN | MOD_ALT, 'T', L"Win+Alt+T"};
    bool sizeFromActive = true;
    int widthPercent = 60;
    int heightPercent = 70;
    bool systemMenuItem = true;
};

// Settings are reloaded on Windhawk's thread and read on window threads, so
// they are only accessed through a copy taken under the lock. The resize key
// is read on every WM_SIZING and lives in an atomic instead.
Settings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;
std::atomic<ResizeModifier> g_resizeModifier{ResizeModifier::Alt};

Settings GetSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings settings = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return settings;
}

bool g_isExplorer = false;
HANDLE g_syncThread = nullptr;
HWND g_syncWnd = nullptr;

// Only one window can be in a mouse/keyboard size loop at a time.
struct ResizeState {
    HWND hwnd = nullptr;
    RECT start{};
    POINT minTrack{};
    POINT maxTrack{};
    bool symmetricApplied = false;
    bool altUsed = false;
} g_resize;

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
        const wchar_t* display;
    } kNames[] = {
        {L"SPACE", VK_SPACE, L"Space"}, {L"ENTER", VK_RETURN, L"Enter"},
        {L"HOME", VK_HOME, L"Home"},    {L"END", VK_END, L"End"},
        {L"INSERT", VK_INSERT, L"Ins"}, {L"DELETE", VK_DELETE, L"Del"},
        {L"PAGEUP", VK_PRIOR, L"PgUp"}, {L"PAGEDOWN", VK_NEXT, L"PgDn"},
        {L"UP", VK_UP, L"Up"},          {L"DOWN", VK_DOWN, L"Down"},
        {L"LEFT", VK_LEFT, L"Left"},    {L"RIGHT", VK_RIGHT, L"Right"},
    };
    for (const auto& k : kNames) {
        if (key == k.name) {
            *display = k.display;
            return k.vk;
        }
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
    std::wstring text;
    for (const auto& m : kMods) {
        if (Wh_GetIntSetting(L"%s.%s", prefix, m.setting)) {
            hotkey.mods |= m.mod;
            text += m.text;
        }
    }
    PCWSTR key = Wh_GetStringSetting(L"%s.key", prefix);
    std::wstring keyDisplay;
    hotkey.vk = ParseKey(key, &keyDisplay);
    Wh_FreeStringSetting(key);
    // No text for a shortcut that PublishHotkeys() disables.
    if (hotkey.vk && hotkey.mods)
        hotkey.text = text + keyDisplay;
    return hotkey;
}

void LoadSettings() {
    PCWSTR mod = Wh_GetStringSetting(L"resizeModifier");
    if (wcscmp(mod, L"ctrl") == 0) {
        g_resizeModifier = ResizeModifier::Ctrl;
    } else if (wcscmp(mod, L"shift") == 0) {
        g_resizeModifier = ResizeModifier::Shift;
    } else if (wcscmp(mod, L"none") == 0) {
        g_resizeModifier = ResizeModifier::None;
    } else {
        g_resizeModifier = ResizeModifier::Alt;
    }
    Wh_FreeStringSetting(mod);

    Settings settings;
    settings.syncHotkey = LoadHotkey(L"hotkey");
    settings.stackHotkey = LoadHotkey(L"stackHotkey");

    PCWSTR source = Wh_GetStringSetting(L"sizeSource");
    settings.sizeFromActive = wcscmp(source, L"fixed") != 0;
    Wh_FreeStringSetting(source);

    auto clampPercent = [](int v) { return v < 10 ? 10 : v > 100 ? 100 : v; };
    settings.widthPercent = clampPercent(Wh_GetIntSetting(L"widthPercent"));
    settings.heightPercent = clampPercent(Wh_GetIntSetting(L"heightPercent"));
    settings.systemMenuItem = Wh_GetIntSetting(L"systemMenuItem");

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(settings);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

// ------------------------------------------------- resize from the center

bool ResizeModifierDown() {
    int vk;
    switch (g_resizeModifier.load()) {
        case ResizeModifier::Alt:
            vk = VK_MENU;
            break;
        case ResizeModifier::Ctrl:
            vk = VK_CONTROL;
            break;
        case ResizeModifier::Shift:
            vk = VK_SHIFT;
            break;
        default:
            return false;
    }
    return GetAsyncKeyState(vk) & 0x8000;
}

// Adjusts one axis of the sizing rect. lo/hi are the current edges, sLo/sHi
// the edges when the size loop started.
void AdjustAxis(LONG* lo,
                LONG* hi,
                LONG sLo,
                LONG sHi,
                bool dragLo,
                bool dragHi,
                bool symmetric,
                LONG minSize,
                LONG maxSize) {
    if (!dragLo && !dragHi)
        return;

    if (!symmetric) {
        // The opposite edge may still be where we mirrored it earlier.
        if (dragLo)
            *hi = sHi;
        else
            *lo = sLo;
        return;
    }

    LONG startSize = sHi - sLo;
    LONG grow = dragLo ? (sLo - *lo) : (*hi - sHi);
    LONG size = startSize + 2 * grow;
    if (size < minSize)
        size = minSize;
    if (maxSize > 0 && size > maxSize)
        size = maxSize;
    *lo = sLo - (size - startSize) / 2;
    *hi = *lo + size;
}

void ApplyCenteredResize(WPARAM edge, RECT* rect) {
    bool symmetric = ResizeModifierDown();
    if (!symmetric && !g_resize.symmetricApplied)
        return;
    if (symmetric) {
        g_resize.symmetricApplied = true;
        if (g_resizeModifier == ResizeModifier::Alt) {
            g_resize.altUsed = true;
        }
    }

    bool left =
        edge == WMSZ_LEFT || edge == WMSZ_TOPLEFT || edge == WMSZ_BOTTOMLEFT;
    bool right =
        edge == WMSZ_RIGHT || edge == WMSZ_TOPRIGHT || edge == WMSZ_BOTTOMRIGHT;
    bool top =
        edge == WMSZ_TOP || edge == WMSZ_TOPLEFT || edge == WMSZ_TOPRIGHT;
    bool bottom = edge == WMSZ_BOTTOM || edge == WMSZ_BOTTOMLEFT ||
                  edge == WMSZ_BOTTOMRIGHT;

    const RECT& s = g_resize.start;
    AdjustAxis(&rect->left, &rect->right, s.left, s.right, left, right,
               symmetric, g_resize.minTrack.x, g_resize.maxTrack.x);
    AdjustAxis(&rect->top, &rect->bottom, s.top, s.bottom, top, bottom,
               symmetric, g_resize.minTrack.y, g_resize.maxTrack.y);
}

void BeginSizeMove(HWND hWnd) {
    g_resize = {};
    g_resize.hwnd = hWnd;
    GetWindowRect(hWnd, &g_resize.start);

    MINMAXINFO mmi{};
    mmi.ptMaxSize = {GetSystemMetrics(SM_CXMAXIMIZED),
                     GetSystemMetrics(SM_CYMAXIMIZED)};
    mmi.ptMinTrackSize = {GetSystemMetrics(SM_CXMINTRACK),
                          GetSystemMetrics(SM_CYMINTRACK)};
    mmi.ptMaxTrackSize = {GetSystemMetrics(SM_CXMAXTRACK),
                          GetSystemMetrics(SM_CYMAXTRACK)};
    SendMessageW(hWnd, WM_GETMINMAXINFO, 0, (LPARAM)&mmi);
    g_resize.minTrack = mmi.ptMinTrackSize;
    g_resize.maxTrack = mmi.ptMaxTrackSize;
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
void EndSizeMove(HWND hWnd) {
    if (g_resize.hwnd != hWnd)
        return;

    // Releasing Alt after the drag would open the app's menu bar: slip an
    // unassigned key in between, like AutoHotkey's "menu mask key".
    if (g_resize.altUsed && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
        SendMaskKey();
    }
    g_resize = {};
}

// ------------------------------------------------------ system menu item

int FindMenuItem(HMENU menu, UINT id) {
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; i++) {
        if (GetMenuItemID(menu, i) == id)
            return i;
    }
    return -1;
}

void RemoveSysMenuItem(HWND hWnd) {
    if (!GetPropW(hWnd, kMenuProp))
        return;
    RemovePropW(hWnd, kMenuProp);

    HMENU menu = GetSystemMenu(hWnd, FALSE);
    if (!menu)
        return;
    DeleteMenu(menu, kSysMenuGatherId, MF_BYCOMMAND);
    int pos = FindMenuItem(menu, kSysMenuSyncId);
    if (pos < 0)
        return;
    DeleteMenu(menu, pos, MF_BYPOSITION);
    if (pos > 0 &&
        (GetMenuState(menu, pos - 1, MF_BYPOSITION) & MF_SEPARATOR)) {
        DeleteMenu(menu, pos - 1, MF_BYPOSITION);
    }
}

std::wstring MenuLabel(PCWSTR text, const Hotkey& hotkey) {
    std::wstring label = text;
    if (!hotkey.text.empty())
        label += L"\t" + hotkey.text;
    return label;
}

void UpdateSysMenuItem(HWND hWnd) {
    Settings settings = GetSettings();
    if (!settings.systemMenuItem) {
        RemoveSysMenuItem(hWnd);
        return;
    }

    HMENU menu = GetSystemMenu(hWnd, FALSE);
    if (!menu)
        return;

    std::wstring syncLabel =
        MenuLabel(L"Give all windows this size", settings.syncHotkey);
    std::wstring gatherLabel =
        MenuLabel(L"Stack all windows behind this one", settings.stackHotkey);

    if (FindMenuItem(menu, kSysMenuSyncId) >= 0) {
        ModifyMenuW(menu, kSysMenuSyncId, MF_BYCOMMAND | MF_STRING,
                    kSysMenuSyncId, syncLabel.c_str());
    } else {
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING, kSysMenuSyncId, syncLabel.c_str());
    }
    if (FindMenuItem(menu, kSysMenuGatherId) >= 0) {
        ModifyMenuW(menu, kSysMenuGatherId, MF_BYCOMMAND | MF_STRING,
                    kSysMenuGatherId, gatherLabel.c_str());
    } else {
        InsertMenuW(menu, FindMenuItem(menu, kSysMenuSyncId) + 1,
                    MF_BYPOSITION | MF_STRING, kSysMenuGatherId,
                    gatherLabel.c_str());
    }
    SetPropW(hWnd, kMenuProp, (HANDLE)1);
}

void PostToSyncWindow(UINT msg, HWND reference) {
    HWND target = FindWindowExW(HWND_MESSAGE, nullptr, kSyncWndClass, nullptr);
    if (target) {
        PostMessageW(target, msg, (WPARAM)reference, 0);
    } else {
        Wh_Log(L"Sync window not found (is explorer.exe running the mod?)");
    }
}

// --------------------------------------------------------------- subclass

// Every window subclassed by the mod, so that unloading can find them all,
// including the ones re-parented since (EnumWindows only sees top-level
// windows).
SRWLOCK g_subclassedLock = SRWLOCK_INIT;
std::unordered_set<HWND> g_subclassed;

void ForgetSubclassedWindow(HWND hWnd) {
    AcquireSRWLockExclusive(&g_subclassedLock);
    g_subclassed.erase(hWnd);
    ReleaseSRWLockExclusive(&g_subclassedLock);
}


LRESULT CALLBACK
SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    switch (uMsg) {
        case WM_ENTERSIZEMOVE:
            BeginSizeMove(hWnd);
            break;

        case WM_SIZING: {
            // Let the app apply its own constraints first, then mirror.
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            if (g_resize.hwnd == hWnd && lParam) {
                ApplyCenteredResize(wParam, (RECT*)lParam);
                return TRUE;
            }
            return result;
        }

        case WM_EXITSIZEMOVE:
            EndSizeMove(hWnd);
            break;

        case WM_INITMENUPOPUP:
            if (HIWORD(lParam) && (HMENU)wParam == GetSystemMenu(hWnd, FALSE)) {
                UpdateSysMenuItem(hWnd);
            }
            break;

        case WM_NCDESTROY:
            // The subclass itself is removed by Windhawk's wrapper.
            RemovePropW(hWnd, kMenuProp);
            ForgetSubclassedWindow(hWnd);
            break;

        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == kSysMenuSyncId) {
                PostToSyncWindow(kMsgSync, hWnd);
                return 0;
            }
            if ((wParam & 0xFFF0) == kSysMenuGatherId) {
                PostToSyncWindow(kMsgGather, hWnd);
                return 0;
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool IsSubclassCandidate(HWND hWnd) {
    LONG style = GetWindowLongW(hWnd, GWL_STYLE);
    if (style & WS_CHILD)
        return false;
    return style & (WS_THICKFRAME | WS_SYSMENU | WS_CAPTION);
}

void SubclassIfCandidate(HWND hWnd) {
    if (IsSubclassCandidate(hWnd) &&
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 0)) {
        AcquireSRWLockExclusive(&g_subclassedLock);
        g_subclassed.insert(hWnd);
        ReleaseSRWLockExclusive(&g_subclassedLock);
    }
}

BOOL CALLBACK CollectProcessWindow(HWND hWnd, LPARAM lParam) {
    DWORD windowPid = 0;
    GetWindowThreadProcessId(hWnd, &windowPid);
    if (windowPid == GetCurrentProcessId()) {
        ((std::vector<HWND>*)lParam)->push_back(hWnd);
    }
    return TRUE;
}

template <typename F>
void ForEachProcessWindow(F f) {
    std::vector<HWND> windows;
    EnumWindows(CollectProcessWindow, (LPARAM)&windows);
    for (HWND hWnd : windows) {
        if (!IsHungAppWindow(hWnd))
            f(hWnd);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && hWndParent != HWND_MESSAGE && !(dwStyle & WS_CHILD)) {
        SubclassIfCandidate(hWnd);
    }
    return hWnd;
}

using CreateWindowExA_t = decltype(&CreateWindowExA);
CreateWindowExA_t CreateWindowExA_Original;
HWND WINAPI CreateWindowExA_Hook(DWORD dwExStyle,
                                 LPCSTR lpClassName,
                                 LPCSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExA_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && hWndParent != HWND_MESSAGE && !(dwStyle & WS_CHILD)) {
        SubclassIfCandidate(hWnd);
    }
    return hWnd;
}

// ------------------------------------------------- sync (explorer only)

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

bool IsSyncable(HWND hWnd, bool allowMaximized = false) {
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

    wchar_t cls[64]{};
    GetClassNameW(hWnd, cls, ARRAYSIZE(cls));
    for (PCWSTR skip :
         {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
          L"Windows.UI.Core.CoreWindow"}) {
        if (wcscmp(cls, skip) == 0)
            return false;
    }

    if (IsCloaked(hWnd) || IsHungAppWindow(hWnd))
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

BOOL CALLBACK CollectSyncableWindow(HWND hWnd, LPARAM lParam) {
    if (IsSyncable(hWnd))
        ((std::vector<HWND>*)lParam)->push_back(hWnd);
    return TRUE;
}

BOOL CALLBACK CollectGatherableWindow(HWND hWnd, LPARAM lParam) {
    if (IsSyncable(hWnd, true))
        ((std::vector<HWND>*)lParam)->push_back(hWnd);
    return TRUE;
}

HWND ResolveReference(HWND reference) {
    if (!reference)
        reference = GetForegroundWindow();
    return reference ? GetAncestor(reference, GA_ROOT) : nullptr;
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

// Same size for all windows. Each window keeps its top-left corner, unless
// it has to move to stay on its screen.
void SyncAllWindows(HWND reference) {
    reference = ResolveReference(reference);
    Settings settings = GetSettings();

    // Common size in 96-DPI units, taken from the reference window.
    bool fromReference = false;
    double logicalW = 0, logicalH = 0;
    if (settings.sizeFromActive && reference && IsSyncable(reference) &&
        (GetWindowLongW(reference, GWL_STYLE) & WS_THICKFRAME)) {
        RECT fr = GetVisibleFrame(reference);
        UINT dpi = GetMonitorDpi(
            MonitorFromWindow(reference, MONITOR_DEFAULTTONEAREST));
        logicalW = (fr.right - fr.left) * 96.0 / dpi;
        logicalH = (fr.bottom - fr.top) * 96.0 / dpi;
        fromReference = true;
    }

    std::vector<HWND> windows;
    EnumWindows(CollectSyncableWindow, (LPARAM)&windows);

    int count = 0;
    for (HWND hWnd : windows) {
        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{sizeof(mi)};
        GetMonitorInfoW(monitor, &mi);
        const RECT& work = mi.rcWork;
        int workW = work.right - work.left;
        int workH = work.bottom - work.top;

        FrameInfo info = GetFrameInfo(hWnd);
        const RECT& fr = info.frame;
        const RECT& b = info.border;

        if (!(GetWindowLongW(hWnd, GWL_STYLE) & WS_THICKFRAME))
            continue;

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

        UINT flags = SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE |
                     SWP_ASYNCWINDOWPOS;
        if (SetWindowPos(hWnd, nullptr, x - b.left, y - b.top,
                         w + b.left + b.right, h + b.top + b.bottom, flags)) {
            count++;
        }
    }
    Wh_Log(L"Synced %d window(s)", count);
}

// The window others gather under: any visible app window, even maximized or
// fullscreen, but not the desktop or the taskbar.
bool IsGatherReference(HWND hWnd) {
    if (!hWnd || !IsWindowVisible(hWnd) || IsIconic(hWnd) || IsCloaked(hWnd)) {
        return false;
    }
    wchar_t cls[64]{};
    GetClassNameW(hWnd, cls, ARRAYSIZE(cls));
    for (PCWSTR skip : {L"Progman", L"WorkerW", L"Shell_TrayWnd",
                        L"Shell_SecondaryTrayWnd"}) {
        if (wcscmp(cls, skip) == 0)
            return false;
    }
    return true;
}

// Restores a maximized window to its normal size, centered on (cx, cy).
// Normal positions are in workspace coordinates: screen coordinates minus
// the offset of the primary monitor's work area (taskbar on top or left).
bool RestoreCenteredOn(HWND hWnd, int cx, int cy, const RECT& work) {
    WINDOWPLACEMENT wp{sizeof(wp)};
    if (!GetWindowPlacement(hWnd, &wp))
        return false;

    MONITORINFO primary{sizeof(primary)};
    GetMonitorInfoW(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY),
                    &primary);
    int offsetX = primary.rcWork.left - primary.rcMonitor.left;
    int offsetY = primary.rcWork.top - primary.rcMonitor.top;

    int w = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
    int h = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
    int x = cx - w / 2;
    int y = std::max<int>(cy - h / 2, work.top);
    wp.rcNormalPosition = {x - offsetX, y - offsetY, x - offsetX + w,
                           y - offsetY + h};
    wp.showCmd = SW_SHOWNOACTIVATE;
    wp.flags = 0;
    return SetWindowPlacement(hWnd, &wp);
}

// Centers every other window under the reference window (same center).
// Each window keeps its size (maximized ones get their normal size back);
// only its title bar is kept on screen.
void GatherOnWindow(HWND reference) {
    reference = ResolveReference(reference);
    if (!IsGatherReference(reference)) {
        Wh_Log(L"Gather: no usable foreground window");
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
    EnumWindows(CollectGatherableWindow, (LPARAM)&windows);

    int count = 0;
    bool restoredAny = false;
    for (HWND hWnd : windows) {
        if (hWnd == reference)
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

        // Windows are enumerated top to bottom and keep that order, so the
        // foreground window stays on top of the pile.
        if (SetWindowPos(hWnd, nullptr, x - info.border.left,
                         y - info.border.top, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                             SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS)) {
            count++;
        }
    }

    // A restored window must not end up above the foreground window.
    if (restoredAny) {
        SetWindowPos(
            reference, HWND_TOP, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    }
    Wh_Log(L"Gathered %d window(s) under the foreground window", count);
}

// ------------------------------------------------------------- shortcuts
//
// A low-level keyboard hook instead of RegisterHotKey: it sees the keys
// before the hotkeys of other apps (Win+Alt+T belongs to the Xbox Game Bar,
// for instance) and still works when a combination is already taken.

struct HookHotkey {
    std::atomic<UINT> mods{0};
    std::atomic<UINT> vk{0};
};
HookHotkey g_hookHotkeys[2];  // indexed by kAction*
HANDLE g_hookThread = nullptr;
DWORD g_hookThreadId = 0;
UINT g_swallowedVk = 0;  // hook thread only

void PublishHotkeys() {
    Settings settings = GetSettings();
    const Hotkey* hotkeys[] = {&settings.syncHotkey, &settings.stackHotkey};
    for (int i = 0; i < 2; i++) {
        // A key without modifier would swallow normal typing.
        bool valid = hotkeys[i]->vk && hotkeys[i]->mods;
        g_hookHotkeys[i].mods = hotkeys[i]->mods;
        g_hookHotkeys[i].vk = valid ? hotkeys[i]->vk : 0;
        if (!valid)
            Wh_Log(L"Shortcut %d disabled: needs a key and a modifier", i);
    }
}

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


LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const auto* key = (const KBDLLHOOKSTRUCT*)lParam;
        bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        // Injected keys count too (key remappers, macro tools), except ours.
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
                        PostMessageW(g_syncWnd, kMsgHotkey, i, 0);
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

// ------------------------------------------------------- sync window

LRESULT CALLBACK SyncWndProc(HWND hWnd,
                             UINT uMsg,
                             WPARAM wParam,
                             LPARAM lParam) {
    switch (uMsg) {
        case kMsgHotkey:
            if (wParam == kActionSync)
                SyncAllWindows(nullptr);
            if (wParam == kActionGather)
                GatherOnWindow(nullptr);
            return 0;
        case kMsgSync:
            SyncAllWindows((HWND)wParam);
            return 0;
        case kMsgGather:
            GatherOnWindow((HWND)wParam);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI SyncThread(LPVOID readyEvent) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSW wc{};
    wc.lpfnWndProc = SyncWndProc;
    wc.hInstance = (HINSTANCE)&__ImageBase;
    wc.lpszClassName = kSyncWndClass;
    RegisterClassW(&wc);

    g_syncWnd = CreateWindowExW(0, kSyncWndClass, L"", 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    SetEvent((HANDLE)readyEvent);

    MSG msg;
    while (g_syncWnd && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_syncWnd = nullptr;
    UnregisterClassW(kSyncWndClass, wc.hInstance);
    return 0;
}

HANDLE StartThread(LPTHREAD_START_ROUTINE proc, DWORD* threadId) {
    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    HANDLE thread = CreateThread(nullptr, 0, proc, ready, 0, threadId);
    if (thread)
        WaitForSingleObject(ready, 5000);
    CloseHandle(ready);
    return thread;
}

void StopThread(HANDLE* thread) {
    if (!*thread)
        return;
    // The mod is unloaded right after Wh_ModUninit: the thread must be gone.
    WaitForSingleObject(*thread, INFINITE);
    CloseHandle(*thread);
    *thread = nullptr;
}

bool IsExplorerProcess() {
    wchar_t path[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!len || len == ARRAYSIZE(path))
        return false;
    PCWSTR name = wcsrchr(path, L'\\');
    return _wcsicmp(name ? name + 1 : path, L"explorer.exe") == 0;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();
    g_isExplorer = IsExplorerProcess();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateWindowExA_Hook,
                                   &CreateWindowExA_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    ForEachProcessWindow(SubclassIfCandidate);

    if (g_isExplorer) {
        PublishHotkeys();
        g_syncThread = StartThread(SyncThread, nullptr);
        if (g_syncWnd)
            g_hookThread = StartThread(HookThread, &g_hookThreadId);
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_isExplorer)
        PublishHotkeys();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        StopThread(&g_hookThread);
    }
    if (g_syncThread) {
        if (g_syncWnd)
            PostMessageW(g_syncWnd, WM_CLOSE, 0, 0);
        StopThread(&g_syncThread);
    }

    // Every subclass must be gone before the mod is unloaded, so busy windows
    // are waited for instead of skipped.
    std::vector<HWND> windows;
    EnumWindows(CollectProcessWindow, (LPARAM)&windows);
    AcquireSRWLockShared(&g_subclassedLock);
    windows.insert(windows.end(), g_subclassed.begin(), g_subclassed.end());
    ReleaseSRWLockShared(&g_subclassedLock);
    std::sort(windows.begin(), windows.end());
    windows.erase(std::unique(windows.begin(), windows.end()), windows.end());

    for (HWND hWnd : windows) {
        RemoveSysMenuItem(hWnd);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
    }
}
