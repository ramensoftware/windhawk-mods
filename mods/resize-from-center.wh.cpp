// ==WindhawkMod==
// @id              resize-from-center
// @name            Resize From Center
// @name:fr-FR      Redimensionner depuis le centre
// @description     Hold Alt while resizing a window to grow it from its center: both opposite edges move together, like Option-resize on macOS.
// @description:fr-FR Maintenir Alt en redimensionnant une fenêtre pour l'agrandir depuis son centre : les deux bords opposés bougent ensemble, comme Option sur macOS.
// @version         1.0
// @author          Jules
// @github          https://github.com/Topin5886
// @include         *
// @compilerOptions -luser32 -lcomctl32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Resize From Center

While resizing a window with the mouse, **hold Alt**:

- dragging an **edge** grows (or shrinks) the window on **both opposite sides**;
- dragging a **corner** grows it on **all four sides**, its center stays in
  place.

Alt can be pressed or released in the middle of the drag: the window switches
between normal and centered resizing right away. The key can be changed in the
settings (Alt, Ctrl or Shift). If you've used a Mac, this is the Option-resize
your hands already know.

![Hold Alt while resizing: the window grows on both sides and stays centered](https://raw.githubusercontent.com/Topin5886/windhawk-window-sync/main/docs/resize-from-center.gif)

## Works with Window Sync

When the [Window Sync](https://windhawk.net/mods/window-sync) mod is also
installed, its two commands ("Give all windows this size" and "Stack all
windows behind this one") are added to the title bar menu of every window
(right-click the title bar, or Alt+Space). This can be turned off in the
settings.

## Notes

- The resizing has to happen inside each app, which is why the mod is loaded in
  all processes. To exclude a program, such as a game with an anti-cheat, add it
  to the custom process exclusion list in the Advanced tab of this mod.
- The [AltDrag](https://windhawk.net/mods/alt-drag) mod also uses Alt by
  default: holding Alt *before* clicking starts an AltDrag move. To resize from
  the center while AltDrag is installed, start dragging the window border
  first, then press Alt, or change the key of either mod.
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
- windowSyncMenuItems: true
  $name: Add the Window Sync commands to the title bar menu
  $description: Only when the Window Sync mod is installed and running
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <unordered_set>
#include <vector>

namespace {

// Message-only window of the Window Sync mod, and its two commands.
constexpr wchar_t kWindowSyncWndClass[] = L"WindhawkWindowSync";
constexpr wchar_t kWindowSyncMsgSync[] = L"WindhawkWindowSync_Sync";
constexpr wchar_t kWindowSyncMsgStack[] = L"WindhawkWindowSync_Stack";

// System menu command IDs: < 0xF000, low 4 bits clear.
constexpr UINT kSysMenuSyncId = 0x5A70;
constexpr UINT kSysMenuStackId = 0x5A80;

// Tags our own injected keys.
constexpr ULONG_PTR kOwnInputMarker = 0x52464343;  // 'RFCC'

enum class ResizeModifier { None, Alt, Ctrl, Shift };

std::atomic<ResizeModifier> g_resizeModifier{ResizeModifier::Alt};
std::atomic<bool> g_windowSyncMenuItems{true};

// Only one window can be in a mouse/keyboard size loop at a time.
struct ResizeState {
    HWND hwnd = nullptr;
    RECT start{};
    POINT minTrack{};
    POINT maxTrack{};
    bool symmetricApplied = false;
    bool altUsed = false;
} g_resize;

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

    g_windowSyncMenuItems = Wh_GetIntSetting(L"windowSyncMenuItems");
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

void EndSizeMove(HWND hWnd) {
    if (g_resize.hwnd != hWnd)
        return;

    // Releasing Alt after the drag would open the app's menu bar: slip an
    // unassigned key in between, like AutoHotkey's "menu mask key".
    if (g_resize.altUsed && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
        INPUT in[2]{};
        in[0].type = in[1].type = INPUT_KEYBOARD;
        in[0].ki.wVk = in[1].ki.wVk = 0xE8;
        in[0].ki.dwExtraInfo = in[1].ki.dwExtraInfo = kOwnInputMarker;
        in[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, in, sizeof(INPUT));
    }
    g_resize = {};
}

// ----------------------------------------- Window Sync title bar commands

HWND FindWindowSync() {
    // FindWindowEx finds message-only windows of other processes too.
    return FindWindowExW(HWND_MESSAGE, nullptr, kWindowSyncWndClass, nullptr);
}

int FindMenuItem(HMENU menu, UINT id) {
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; i++) {
        if (GetMenuItemID(menu, i) == id)
            return i;
    }
    return -1;
}

void RemoveMenuItems(HMENU menu) {
    DeleteMenu(menu, kSysMenuStackId, MF_BYCOMMAND);
    int pos = FindMenuItem(menu, kSysMenuSyncId);
    if (pos < 0)
        return;
    DeleteMenu(menu, pos, MF_BYPOSITION);
    if (pos > 0 &&
        (GetMenuState(menu, pos - 1, MF_BYPOSITION) & MF_SEPARATOR)) {
        DeleteMenu(menu, pos - 1, MF_BYPOSITION);
    }
}

void UpdateMenuItems(HMENU menu) {
    if (!g_windowSyncMenuItems || !FindWindowSync()) {
        RemoveMenuItems(menu);
        return;
    }
    if (FindMenuItem(menu, kSysMenuSyncId) >= 0)
        return;
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kSysMenuSyncId, L"Give all windows this size");
    AppendMenuW(menu, MF_STRING, kSysMenuStackId,
                L"Stack all windows behind this one");
}

void SendWindowSyncCommand(PCWSTR messageName, HWND hWnd) {
    HWND target = FindWindowSync();
    if (target) {
        PostMessageW(target, RegisterWindowMessageW(messageName), (WPARAM)hWnd,
                     0);
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
                UpdateMenuItems((HMENU)wParam);
            }
            break;

        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == kSysMenuSyncId) {
                SendWindowSyncCommand(kWindowSyncMsgSync, hWnd);
                return 0;
            }
            if ((wParam & 0xFFF0) == kSysMenuStackId) {
                SendWindowSyncCommand(kWindowSyncMsgStack, hWnd);
                return 0;
            }
            break;

        case WM_NCDESTROY:
            // The subclass itself is removed by Windhawk's wrapper.
            ForgetSubclassedWindow(hWnd);
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

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateWindowExA_Hook,
                                   &CreateWindowExA_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    std::vector<HWND> windows;
    EnumWindows(CollectProcessWindow, (LPARAM)&windows);
    for (HWND hWnd : windows) {
        // A busy window would block the init; it gets no subclass.
        if (!IsHungAppWindow(hWnd))
            SubclassIfCandidate(hWnd);
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");

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
        if (HMENU menu = GetSystemMenu(hWnd, FALSE)) {
            if (FindMenuItem(menu, kSysMenuSyncId) >= 0)
                RemoveMenuItems(menu);
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
    }
}
