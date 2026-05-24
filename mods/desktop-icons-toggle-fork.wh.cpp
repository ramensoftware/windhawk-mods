// ==WindhawkMod==
// @id              desktop-icons-toggle-fork
// @name            Toggle Desktop Icons - Fork
// @description     Instantly show/hide desktop icons with a draggable floating button, a keyboard shortcut, and adjustable opacity.
// @version         2.1
// @author          Aaron - KiivYx
// @github          https://github.com/KiivYx
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Toggle Desktop Icons

Does what the Windows option "View -> Show desktop icons" does, but
**without** the context menu. It acts directly on the desktop's `SysListView32`
window (same as the native function).

## Ways to toggle

- **Floating button** on screen: one click toggles show/hide. **Click
  and hold to drag** it wherever you want; its position is remembered.
- **Global keyboard shortcut** (default `Ctrl+Alt+D`).
- **Checkbox** "Show desktop icons" in the mod settings.

## Extra

- **Icon Opacity** from 0 to 100 (transparency), something the native
  function doesn't allow.

## Usage

1. Compile (`Ctrl+B`) and enable the mod.
2. You will see the floating button. Click to toggle, drag to relocate.
3. You can adjust the button opacity from the settings to hide it completely or make it visible to move it.

When disabling the mod, the icons become visible and opaque again, and the button disappears.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showIcons: true
  $name: Show desktop icons
  $description: Enables or disables the visibility of the desktop icons.
- opacity: 100
  $name: Icons opacity
  $description: From 0 (invisible) to 100 (fully opaque).
- showButton: true
  $name: Show floating button
  $description: On-screen button to toggle (click) and draggable (hold and move).
- buttonOpacity: 60
  $name: Floating button opacity
  $description: From 1 to 100. (Controls how transparent the base button is).
- buttonSize: 64
  $name: Floating button size
  $description: Size in pixels (e.g. 32, 46, 64).
- hotkey: "Ctrl+Alt+D"
  $name: Toggle shortcut
  $description: >-
    Global combination. Examples: Ctrl+Alt+D, Win+I, Ctrl+Shift+F8. Leave it empty
    to disable the shortcut.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <cwctype>
#include <algorithm>

// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------
static bool   g_settingShow = true;   // checkbox value
static int    g_opacity     = 100;    // 0..100
static bool   g_showButton  = true;   // show floating button
static int    g_btnOpacity  = 60;
static int    g_btnSize     = 64;
static bool   g_hidden      = false;  // runtime state
static UINT   g_mods        = 0;      // shortcut modifiers
static UINT   g_vk          = 0;      // shortcut key (0 = no shortcut)

static HANDLE g_thread      = nullptr;
static DWORD  g_threadId    = 0;
static HANDLE g_threadReady = nullptr;

// Floating button
static HWND   g_btnWnd          = nullptr;
static bool   g_classRegistered = false;
static bool   g_dragging        = false;
static bool   g_moved           = false;
static POINT  g_dragStartCur    = {0, 0};
static POINT  g_winStart        = {0, 0};
static const wchar_t* kBtnClass = L"WhDesktopIconsToggleButton";

// ---------------------------------------------------------------------------
// Locate the desktop icons window (SysListView32)
// ---------------------------------------------------------------------------
static BOOL CALLBACK FindDefViewEnumProc(HWND top, LPARAM lp) {
    WCHAR cls[64];
    if (!GetClassNameW(top, cls, ARRAYSIZE(cls)))
        return TRUE;
    if (wcscmp(cls, L"WorkerW") == 0) {
        HWND dv = FindWindowExW(top, nullptr, L"SHELLDLL_DefView", nullptr);
        if (dv) {
            *reinterpret_cast<HWND*>(lp) = dv;
            return FALSE;  // found, stop enumeration
        }
    }
    return TRUE;
}

static HWND GetDesktopListView() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND defview = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);

    // In Win10/11 with active background the DefView might hang from a WorkerW.
    if (!defview) {
        EnumWindows(FindDefViewEnumProc, reinterpret_cast<LPARAM>(&defview));
    }
    if (!defview)
        return nullptr;

    return FindWindowExW(defview, nullptr, L"SysListView32", nullptr);
}

// ---------------------------------------------------------------------------
// Apply the current state (visibility + opacity) to the icons
// ---------------------------------------------------------------------------
static void ApplyState() {
    HWND lv = GetDesktopListView();
    if (!lv) {
        Wh_Log(L"Desktop icons list not found");
        return;
    }

    if (g_hidden) {
        ShowWindow(lv, SW_HIDE);
        return;
    }

    ShowWindow(lv, SW_SHOWNA);

    LONG_PTR ex = GetWindowLongPtrW(lv, GWL_EXSTYLE);
    if (g_opacity >= 100) {
        if (ex & WS_EX_LAYERED) {
            SetWindowLongPtrW(lv, GWL_EXSTYLE, ex & ~static_cast<LONG_PTR>(WS_EX_LAYERED));
            RedrawWindow(lv, nullptr, nullptr,
                         RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
    } else {
        SetWindowLongPtrW(lv, GWL_EXSTYLE, ex | WS_EX_LAYERED);
        int a = g_opacity;
        if (a < 0) a = 0;
        BYTE alpha = static_cast<BYTE>(a * 255 / 100);
        SetLayeredWindowAttributes(lv, 0, alpha, LWA_ALPHA);
        RedrawWindow(lv, nullptr, nullptr,
                     RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
    }
}

static void ToggleNow() {
    g_hidden = !g_hidden;
    Wh_SetIntValue(L"hidden", g_hidden ? 1 : 0);
    ApplyState();
    if (g_btnWnd)
        InvalidateRect(g_btnWnd, nullptr, TRUE);
}

// ---------------------------------------------------------------------------
// Floating button: drawing and behavior (Minimalist)
// ---------------------------------------------------------------------------
static void PaintButton(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc;
    GetClientRect(hwnd, &rc);

    // Transparent magenta background
    HBRUSH keyBrush = CreateSolidBrush(RGB(255, 0, 255));
    FillRect(hdc, &rc, keyBrush);
    DeleteObject(keyBrush);

    // Simple rounded button
    COLORREF bgColor = g_hidden ? RGB(60, 60, 60) : RGB(0, 120, 215);
    HBRUSH body = CreateSolidBrush(bgColor);
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    
    HGDIOBJ oldBody = SelectObject(hdc, body);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    
    // Draw the box
    RoundRect(hdc, rc.left + 2, rc.top + 2, rc.right - 2, rc.bottom - 2, 12, 12);
    
    // Red indicator when icons are hidden
    if (g_hidden) {
        int padding = g_btnSize / 4;
        HPEN redPen = CreatePen(PS_SOLID, 3, RGB(230, 60, 60));
        SelectObject(hdc, redPen);
        MoveToEx(hdc, rc.left + padding, rc.bottom - padding, nullptr);
        LineTo(hdc, rc.right - padding, rc.top + padding);
        DeleteObject(redPen);
    }

    SelectObject(hdc, oldBody);
    SelectObject(hdc, oldPen);
    DeleteObject(body);
    DeleteObject(pen);

    EndPaint(hwnd, &ps);
}

static LRESULT CALLBACK BtnWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_LBUTTONDOWN: {
            g_dragging = true;
            g_moved = false;
            GetCursorPos(&g_dragStartCur);
            RECT rc;
            GetWindowRect(hwnd, &rc);
            g_winStart.x = rc.left;
            g_winStart.y = rc.top;
            SetCapture(hwnd);
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (g_dragging) {
                POINT p;
                GetCursorPos(&p);
                int dx = p.x - g_dragStartCur.x;
                int dy = p.y - g_dragStartCur.y;
                if (dx > 3 || dx < -3 || dy > 3 || dy < -3)
                    g_moved = true;
                    
                int newX = g_winStart.x + dx;
                int newY = g_winStart.y + dy;
                
                int sx = GetSystemMetrics(SM_CXSCREEN);
                int sy = GetSystemMetrics(SM_CYSCREEN);
                if (newX < 0) newX = 0;
                if (newY < 0) newY = 0;
                if (newX > sx - g_btnSize) newX = sx - g_btnSize;
                if (newY > sy - g_btnSize) newY = sy - g_btnSize;

                SetWindowPos(hwnd, nullptr, newX, newY,
                             0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (g_dragging) {
                g_dragging = false;
                ReleaseCapture();
                
                if (!g_moved) {
                    ToggleNow();  // it was a click: toggle
                } else {
                    RECT rc;
                    GetWindowRect(hwnd, &rc);
                    Wh_SetIntValue(L"btnX", rc.left);
                    Wh_SetIntValue(L"btnY", rc.top);
                }
            }
            return 0;
        }
        case WM_PAINT:
            PaintButton(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void CreateButtonWindow() {
    if (!g_showButton || g_btnWnd)
        return;

    HINSTANCE hInst = GetModuleHandleW(nullptr);

    if (!g_classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize        = sizeof(wc);
        wc.lpfnWndProc   = BtnWndProc;
        wc.hInstance     = hInst;
        wc.hCursor       = LoadCursorW(nullptr, IDC_SIZEALL);
        wc.lpszClassName = kBtnClass;
        RegisterClassExW(&wc);  // if it already exists, it gets ignored
        g_classRegistered = true;
    }

    int sx = GetSystemMetrics(SM_CXSCREEN);
    int defX = sx - g_btnSize - 40;
    int defY = 120;
    int x = Wh_GetIntValue(L"btnX", defX);
    int y = Wh_GetIntValue(L"btnY", defY);

    int sy = GetSystemMetrics(SM_CYSCREEN);
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x > sx - g_btnSize) x = sx - g_btnSize;
    if (y > sy - g_btnSize) y = sy - g_btnSize;

    g_btnWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE,
        kBtnClass, L"", WS_POPUP,
        x, y, g_btnSize, g_btnSize,
        nullptr, nullptr, hInst, nullptr);

    if (g_btnWnd) {
        BYTE alpha = static_cast<BYTE>(g_btnOpacity * 255 / 100);
        SetLayeredWindowAttributes(g_btnWnd, RGB(255, 0, 255), alpha,
                                   LWA_COLORKEY | LWA_ALPHA);
        ShowWindow(g_btnWnd, SW_SHOWNA);
        UpdateWindow(g_btnWnd);
    }
}

static void DestroyButtonWindow() {
    if (g_btnWnd) {
        DestroyWindow(g_btnWnd);
        g_btnWnd = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Keyboard shortcut parsing
// ---------------------------------------------------------------------------
static UINT KeyNameToVk(const std::wstring& t) {
    if (t.size() == 1) {
        wchar_t c = t[0];
        if (c >= L'a' && c <= L'z') return static_cast<UINT>(L'A' + (c - L'a'));
        if (c >= L'0' && c <= L'9') return static_cast<UINT>(c);
    }
    if (t.size() >= 2 && t[0] == L'f') {
        int n = 0;
        bool digits = true;
        for (size_t i = 1; i < t.size(); ++i) {
            wchar_t d = t[i];
            if (d < L'0' || d > L'9') { digits = false; break; }
            n = n * 10 + (d - L'0');
        }
        if (digits && n >= 1 && n <= 24) return static_cast<UINT>(VK_F1 + (n - 1));
    }
    if (t == L"space")                 return VK_SPACE;
    if (t == L"insert" || t == L"ins") return VK_INSERT;
    if (t == L"delete" || t == L"del") return VK_DELETE;
    if (t == L"home")                  return VK_HOME;
    if (t == L"end")                   return VK_END;
    if (t == L"pgup" || t == L"prior") return VK_PRIOR;
    if (t == L"pgdn" || t == L"next")  return VK_NEXT;
    return 0;
}

static bool ParseHotkey(PCWSTR s, UINT* mods, UINT* vk) {
    *mods = MOD_NOREPEAT;
    *vk = 0;
    if (!s) return false;

    std::wstring str = s;
    size_t start = 0;
    while (start <= str.size()) {
        size_t plus = str.find(L'+', start);
        std::wstring tok = (plus == std::wstring::npos)
                                ? str.substr(start)
                                : str.substr(start, plus - start);

        size_t a = tok.find_first_not_of(L" \t");
        if (a == std::wstring::npos) {
            tok.clear();
        } else {
            size_t b = tok.find_last_not_of(L" \t");
            tok = tok.substr(a, b - a + 1);
        }
        for (auto& c : tok) c = towlower(c);

        if (!tok.empty()) {
            if (tok == L"ctrl" || tok == L"control")               *mods |= MOD_CONTROL;
            else if (tok == L"alt")                                *mods |= MOD_ALT;
            else if (tok == L"shift")                              *mods |= MOD_SHIFT;
            else if (tok == L"win" || tok == L"windows" || tok == L"meta") *mods |= MOD_WIN;
            else {
                UINT k = KeyNameToVk(tok);
                if (k) *vk = k;
            }
        }

        if (plus == std::wstring::npos) break;
        start = plus + 1;
    }

    return *vk != 0;
}

// ---------------------------------------------------------------------------
// UI Thread: keyboard shortcut + button message pumping
// ---------------------------------------------------------------------------
static DWORD WINAPI UiThreadProc(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    BOOL registered = FALSE;
    if (g_vk)
        registered = RegisterHotKey(nullptr, 1, g_mods, g_vk);
    if (g_vk && !registered)
        Wh_Log(L"Could not register the shortcut (might be in use)");

    CreateButtonWindow();

    if (g_threadReady)
        SetEvent(g_threadReady);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY) {
            ToggleNow();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    DestroyButtonWindow();
    if (registered)
        UnregisterHotKey(nullptr, 1);
    return 0;
}

static void StartUiThread() {
    if (!g_vk && !g_showButton)
        return;  // nothing to do
    g_threadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_threadId);
    if (g_threadReady) {
        if (g_thread)
            WaitForSingleObject(g_threadReady, 3000);
        CloseHandle(g_threadReady);
        g_threadReady = nullptr;
    }
}

static void StopUiThread() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
        g_threadId = 0;
    }
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
static void LoadSettings() {
    g_settingShow = Wh_GetIntSetting(L"showIcons") != 0;
    g_showButton  = Wh_GetIntSetting(L"showButton") != 0;

    g_opacity = Wh_GetIntSetting(L"opacity");
    if (g_opacity < 0)   g_opacity = 0;
    if (g_opacity > 100) g_opacity = 100;

    g_btnOpacity = Wh_GetIntSetting(L"buttonOpacity");
    if (g_btnOpacity < 1)   g_btnOpacity = 1;
    if (g_btnOpacity > 100) g_btnOpacity = 100;

    g_btnSize = Wh_GetIntSetting(L"buttonSize");
    if (g_btnSize < 16)  g_btnSize = 16;
    if (g_btnSize > 256) g_btnSize = 256;

    PCWSTR hk = Wh_GetStringSetting(L"hotkey");
    if (!ParseHotkey(hk, &g_mods, &g_vk)) {
        g_mods = 0;
        g_vk = 0;
    }
    Wh_FreeStringSetting(hk);
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    int defaultHidden = g_settingShow ? 0 : 1;
    g_hidden = Wh_GetIntValue(L"hidden", defaultHidden) != 0;

    ApplyState();
    StartUiThread();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    StopUiThread();

    // Restore visible and opaque icons when disabling the mod.
    g_hidden = false;
    g_opacity = 100;
    ApplyState();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    StopUiThread();
    LoadSettings();

    // The checkbox takes priority when editing settings.
    g_hidden = !g_settingShow;
    Wh_SetIntValue(L"hidden", g_hidden ? 1 : 0);

    ApplyState();
    StartUiThread();
}