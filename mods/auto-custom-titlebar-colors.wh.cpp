// ==WindhawkMod==
// @id              auto-custom-titlebar-colors
// @name            Auto Custom Titlebar Colors
// @description     Auto-switches titlebar dark/light mode with the Windows theme, with separate custom colours for active/inactive windows in both modes
// @version         1.1.2
// @author          Lone
// @github          https://github.com/Louis047
// @include         *
// @exclude         devenv.exe
// @exclude         systemsettings.exe
// @exclude         applicationframehost.exe
// @compilerOptions -ldwmapi -luxtheme -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Combines automatic dark/light titlebar switching with per-mode, per-state custom colours. Inspired from the following mods:
- `Auto Dark Titlebar` by Asteski
- `Windows 11 Custom Titlebar Colors` by Th3Fanbus

## Features
- Auto-detects Windows dark/light theme via registry and applies `DWMWA_USE_IMMERSIVE_DARK_MODE`
- Custom titlebar colours for four independent states:
  - **Light Mode** - Active window
  - **Light Mode** - Inactive window
  - **Dark Mode**  - Active window
  - **Dark Mode**  - Inactive window
- Per-mode "Use Custom Colours" toggle -- leave off to keep system defaults for that mode
- Per-mode colour input format toggle -- choose between Hex (`RRGGBB`) or separate R/G/B fields
- Real-time theme change detection (responds to `WM_SETTINGCHANGE` / `WM_DWMCOLORIZATIONCOLORCHANGED`)
- New windows receive the correct style immediately via `CreateWindowEx` hooks
- Dialog windows handled via `DefDlgProc` hooks
- Live settings reload: colour changes apply instantly without restarting the process

## Colour Format
- **Hex mode**: enter a 6-character hex string, e.g. `FF0000` for red (no `#` prefix)
- **RGB mode**: enter R, G, B as integers 0-255 in separate fields

Custom colours are only applied when the corresponding "Use Custom Colours" toggle is enabled.

## Notes
- `systemsettings.exe` and `applicationframehost.exe` are excluded to avoid conflicts
- No forced repaint is issued while a mouse button is held (prevents drag-state corruption)
- Window redraws are debounced (50ms minimum) to prevent interference with window managers
- Visual redraws (SetWindowPos) only occur during window activation, not deactivation, to avoid focus-grabbing issues with tiling window managers
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- customColours:
  - light: false
    $name: "Light Mode"
  - dark: false
    $name: "Dark Mode"
  $name: "Custom Colours"

- lightMode:
  - useHex: false
    $name: "Use Hex Input"
    $description: "When on, use RRGGBB hex strings. When off, use separate R/G/B fields."
  - activeColour:
    - hex: "FFFFFF"
      $name: "Hex (RRGGBB)"
    - r: 255
      $name: "R (0-255)"
    - g: 255
      $name: "G (0-255)"
    - b: 255
      $name: "B (0-255)"
    $name: "Active Window"
  - inactiveColour:
    - hex: "E6E6E6"
      $name: "Hex (RRGGBB)"
    - r: 230
      $name: "R (0-255)"
    - g: 230
      $name: "G (0-255)"
    - b: 230
      $name: "B (0-255)"
    $name: "Inactive Window"
  $name: "Light Mode Colours"

- darkMode:
  - useHex: false
    $name: "Use Hex Input"
    $description: "When on, use RRGGBB hex strings. When off, use separate R/G/B fields."
  - activeColour:
    - hex: "202020"
      $name: "Hex (RRGGBB)"
    - r: 32
      $name: "R (0-255)"
    - g: 32
      $name: "G (0-255)"
    - b: 32
      $name: "B (0-255)"
    $name: "Active Window"
  - inactiveColour:
    - hex: "323232"
      $name: "Hex (RRGGBB)"
    - r: 50
      $name: "R (0-255)"
    - g: 50
      $name: "G (0-255)"
    - b: 50
      $name: "B (0-255)"
    $name: "Inactive Window"
  $name: "Dark Mode Colours"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

typedef HRESULT(WINAPI* pShouldSystemUseDarkMode)();
static pShouldSystemUseDarkMode g_ShouldSystemUseDarkMode = nullptr;
static BOOL g_isDarkMode = FALSE;

// Debounce mechanism: track last SetWindowPos time per window to prevent
// rapid successive redraws that can interfere with window managers
static DWORD g_lastSetWindowPosTime = 0;
const DWORD SETWINDOWPOS_DEBOUNCE_MS = 50;  // 50ms minimum between redraws

// -----------------------------------------------------------------------------
// Dark mode detection
// -----------------------------------------------------------------------------

BOOL IsSystemDarkMode()
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD value = 0, size = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr,
            (LPBYTE)&value, &size) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return value == 0;
        }
        RegCloseKey(hKey);
    }

    if (!g_ShouldSystemUseDarkMode) {
        HMODULE hUxtheme = GetModuleHandleW(L"uxtheme.dll");
        if (hUxtheme) {
            g_ShouldSystemUseDarkMode = (pShouldSystemUseDarkMode)
                GetProcAddress(hUxtheme, MAKEINTRESOURCEA(138));
        }
    }
    if (g_ShouldSystemUseDarkMode)
        return g_ShouldSystemUseDarkMode() != 0;

    return FALSE;
}

// -----------------------------------------------------------------------------
// Window eligibility
// -----------------------------------------------------------------------------

BOOL IsWindowEligible(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd)) return FALSE;

    LONG style   = GetWindowLongW(hWnd, GWL_STYLE);
    LONG styleEx = GetWindowLongW(hWnd, GWL_EXSTYLE);

    if (!(style & WS_CAPTION))       return FALSE;
    if (styleEx & WS_EX_TOOLWINDOW)  return FALSE;
    if (style & WS_CHILD)            return FALSE;

    return TRUE;
}

// -----------------------------------------------------------------------------
// Hex string (RRGGBB, no '#') -> COLORREF
// Returns FALSE on invalid input and leaves *out unchanged.
// -----------------------------------------------------------------------------

static BOOL HexToColorref(PCWSTR hex, COLORREF* out)
{
    if (!hex || !out) return FALSE;

    int len = 0;
    while (hex[len]) len++;
    if (len != 6) return FALSE;

    COLORREF result = 0;
    for (int i = 0; i < 6; i++) {
        WCHAR c = hex[i];
        BYTE nibble;
        if      (c >= L'0' && c <= L'9') nibble = (BYTE)(c - L'0');
        else if (c >= L'A' && c <= L'F') nibble = (BYTE)(c - L'A' + 10);
        else if (c >= L'a' && c <= L'f') nibble = (BYTE)(c - L'a' + 10);
        else return FALSE;
        result = (result << 4) | nibble;
    }

    // RRGGBB -> COLORREF (0x00BBGGRR)
    BYTE r = (BYTE)((result >> 16) & 0xFF);
    BYTE g = (BYTE)((result >>  8) & 0xFF);
    BYTE b = (BYTE)((result >>  0) & 0xFF);
    *out = RGB(r, g, b);
    return TRUE;
}

// -----------------------------------------------------------------------------
// Settings helpers
// -----------------------------------------------------------------------------

static BOOL UseCustomColourForMode(BOOL isDarkMode)
{
    return (BOOL)Wh_GetIntSetting(isDarkMode
        ? L"customColours.dark"
        : L"customColours.light");
}

static COLORREF GetTitleBarColour(BOOL isDarkMode, BOOL isActive)
{
    const WCHAR* modePrefix  = isDarkMode ? L"darkMode"     : L"lightMode";
    const WCHAR* statePrefix = isActive   ? L"activeColour" : L"inactiveColour";

    WCHAR useHexKey[64], hexKey[64], rKey[64], gKey[64], bKey[64];
    wsprintfW(useHexKey, L"%s.useHex",  modePrefix);
    wsprintfW(hexKey,    L"%s.%s.hex",  modePrefix, statePrefix);
    wsprintfW(rKey,      L"%s.%s.r",    modePrefix, statePrefix);
    wsprintfW(gKey,      L"%s.%s.g",    modePrefix, statePrefix);
    wsprintfW(bKey,      L"%s.%s.b",    modePrefix, statePrefix);

    BOOL useHex = (BOOL)Wh_GetIntSetting(useHexKey);

    if (useHex) {
        PCWSTR hexVal = Wh_GetStringSetting(hexKey);
        COLORREF colour = 0;
        BOOL ok = HexToColorref(hexVal, &colour);
        Wh_FreeStringSetting(hexVal);

        if (ok) return colour;

        // Invalid hex: log and fall through to RGB
        Wh_Log(L"GetTitleBarColour: invalid hex for key '%s', falling back to RGB", hexKey);
    }

    BYTE r = (BYTE)Wh_GetIntSetting(rKey);
    BYTE g = (BYTE)Wh_GetIntSetting(gKey);
    BYTE b = (BYTE)Wh_GetIntSetting(bKey);
    return RGB(r, g, b);
}

// -----------------------------------------------------------------------------
// Core: apply dark-mode attribute + caption colour to one window
// -----------------------------------------------------------------------------

static VOID ApplyTitleBar(HWND hWnd, BOOL isActive, BOOL forceRedraw)
{
    if (!IsWindowEligible(hWnd)) return;

    BOOL darkMode = g_isDarkMode;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE,
        &darkMode, sizeof(darkMode));

    if (UseCustomColourForMode(g_isDarkMode)) {
        COLORREF colour = GetTitleBarColour(g_isDarkMode, isActive);
        DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR,
            &colour, sizeof(colour));
        Wh_Log(L"ApplyTitleBar: hWnd=%p dark=%d active=%d colour=#%06X",
            hWnd, g_isDarkMode, isActive, colour);
    } else {
        const COLORREF def = DWMWA_COLOR_DEFAULT;
        DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &def, sizeof(def));
        Wh_Log(L"ApplyTitleBar: hWnd=%p dark=%d active=%d colour=DEFAULT",
            hWnd, g_isDarkMode, isActive);
    }

    // Debounce SetWindowPos calls to prevent interference with window managers.
    // Only allow redraws if enough time has passed since the last redraw.
    if (forceRedraw && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        DWORD currentTime = GetTickCount();
        DWORD timeSinceLastRedraw = currentTime - g_lastSetWindowPosTime;
        
        if (timeSinceLastRedraw >= SETWINDOWPOS_DEBOUNCE_MS) {
            SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE |
                SWP_NOZORDER | SWP_NOOWNERZORDER);
            g_lastSetWindowPosTime = currentTime;
        }
    }
}

// -----------------------------------------------------------------------------
// Enumerate all eligible windows in the current process

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    HWND parent = GetAncestor(hWnd, GA_PARENT);
    if (parent && parent != GetDesktopWindow()) return TRUE;

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hWnd, &pid) || pid != GetCurrentProcessId())
        return TRUE;

    BOOL isActive = (GetForegroundWindow() == hWnd);
    
    // Only refresh with SetWindowPos if it's the active window to minimize
    // interference with tiling window managers
    BOOL forceRedraw = (BOOL)lParam;
    if (forceRedraw && !isActive) {
        ApplyTitleBar(hWnd, isActive, FALSE);
    } else {
        ApplyTitleBar(hWnd, isActive, forceRedraw);
    }
    return TRUE;
}

static VOID ApplyToAllWindows(BOOL forceRedraw)
{
    EnumWindows(EnumWindowsProc, forceRedraw);
}

static VOID ApplyToForegroundWindow()
{
    HWND hWnd = GetForegroundWindow();
    if (hWnd) {
        DWORD pid = 0;
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId()) {
            BOOL isActive = TRUE;
            ApplyTitleBar(hWnd, isActive, TRUE);
        }
    }
}

// -----------------------------------------------------------------------------
// Shared message handler
// -----------------------------------------------------------------------------

static VOID HandleWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
        case WM_ACTIVATE:
        {
            BOOL isActive = (LOWORD(wParam) != WA_INACTIVE);
            // Only force visual redraw when a window becomes active.
            // During deactivation, apply attributes without SetWindowPos
            // to prevent interfering with window manager focus changes.
            BOOL forceRedraw = isActive;  // Only redraw on activation
            ApplyTitleBar(hWnd, isActive, forceRedraw);
            break;
        }
        case WM_NCACTIVATE:
        {
            // Only force visual redraw when a window becomes active.
            // When deactivating (wParam == FALSE), apply attributes without
            // SetWindowPos to avoid interfering with window manager focus handling.
            BOOL isActive = (BOOL)wParam;
            BOOL forceRedraw = isActive;  // Only redraw on activation
            ApplyTitleBar(hWnd, isActive, forceRedraw);
            break;
        }
        case WM_DWMCOLORIZATIONCOLORCHANGED:
        {
            BOOL isActive = (GetForegroundWindow() == hWnd);
            ApplyTitleBar(hWnd, isActive, TRUE);
            break;
        }
        case WM_SETTINGCHANGE:
        {
            BOOL isThemeChange = !lParam ||
                wcscmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0;
            if (!isThemeChange) break;

            BOOL newDarkMode = IsSystemDarkMode();
            if (newDarkMode != g_isDarkMode) {
                g_isDarkMode = newDarkMode;
                Wh_Log(L"[PID %d] Theme changed to %s",
                    GetCurrentProcessId(), newDarkMode ? L"DARK" : L"LIGHT");
                
                // Only update the foreground window to minimize interference
                // with tiling window managers. Other windows will be updated
                // when they receive activation messages.
                ApplyToForegroundWindow();
                
                // Still update all windows, but without forced redraw
                // to apply the dark mode attribute. SetWindowPos is only
                // called for the foreground window.
                ApplyToAllWindows(FALSE);
            }
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// Hook: DefWindowProcW / DefWindowProcA
// -----------------------------------------------------------------------------

using DefWindowProcW_t = decltype(&DefWindowProcW);
static DefWindowProcW_t DefWindowProcW_orig;

LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcW_orig(hWnd, Msg, wParam, lParam);
    HandleWindowMessage(hWnd, Msg, wParam, lParam);
    return result;
}

using DefWindowProcA_t = decltype(&DefWindowProcA);
static DefWindowProcA_t DefWindowProcA_orig;

LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcA_orig(hWnd, Msg, wParam, lParam);
    HandleWindowMessage(hWnd, Msg, wParam, lParam);
    return result;
}

// -----------------------------------------------------------------------------
// Hook: DefDlgProcW / DefDlgProcA
// -----------------------------------------------------------------------------

using DefDlgProcW_t = decltype(&DefDlgProcW);
static DefDlgProcW_t DefDlgProcW_orig;

LRESULT WINAPI DefDlgProcW_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcW_orig(hWnd, Msg, wParam, lParam);
    if (Msg == WM_NCACTIVATE) {
        BOOL isActive = (BOOL)wParam;
        // Only force redraw on activation, not deactivation
        ApplyTitleBar(hWnd, isActive, isActive);
    }
    return result;
}

using DefDlgProcA_t = decltype(&DefDlgProcA);
static DefDlgProcA_t DefDlgProcA_orig;

LRESULT WINAPI DefDlgProcA_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcA_orig(hWnd, Msg, wParam, lParam);
    if (Msg == WM_NCACTIVATE) {
        BOOL isActive = (BOOL)wParam;
        // Only force redraw on activation, not deactivation
        ApplyTitleBar(hWnd, isActive, isActive);
    }
    return result;
}

// -----------------------------------------------------------------------------
// Hook: CreateWindowExW / CreateWindowExA
// -----------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_orig;

HWND WINAPI CreateWindowExW_hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = CreateWindowExW_orig(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hWnd) ApplyTitleBar(hWnd, FALSE, FALSE);
    return hWnd;
}

using CreateWindowExA_t = decltype(&CreateWindowExA);
static CreateWindowExA_t CreateWindowExA_orig;

HWND WINAPI CreateWindowExA_hook(
    DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = CreateWindowExA_orig(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hWnd) ApplyTitleBar(hWnd, FALSE, FALSE);
    return hWnd;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    Wh_Log(L"=== Auto Custom Titlebar Colors - Init [PID %d] ===",
        GetCurrentProcessId());

    g_isDarkMode = IsSystemDarkMode();
    Wh_Log(L"Initial theme: %s", g_isDarkMode ? L"DARK" : L"LIGHT");

    auto hook = [](void* target, void* replacement, void** orig, const wchar_t* name) {
        if (!Wh_SetFunctionHook(target, replacement, orig))
            Wh_Log(L"WARNING: Failed to hook %s", name);
        else
            Wh_Log(L"Hooked %s", name);
    };

    hook((void*)DefWindowProcW,  (void*)DefWindowProcW_hook,  (void**)&DefWindowProcW_orig,  L"DefWindowProcW");
    hook((void*)DefWindowProcA,  (void*)DefWindowProcA_hook,  (void**)&DefWindowProcA_orig,  L"DefWindowProcA");
    hook((void*)DefDlgProcW,     (void*)DefDlgProcW_hook,     (void**)&DefDlgProcW_orig,     L"DefDlgProcW");
    hook((void*)DefDlgProcA,     (void*)DefDlgProcA_hook,     (void**)&DefDlgProcA_orig,     L"DefDlgProcA");
    hook((void*)CreateWindowExW, (void*)CreateWindowExW_hook, (void**)&CreateWindowExW_orig, L"CreateWindowExW");
    hook((void*)CreateWindowExA, (void*)CreateWindowExA_hook, (void**)&CreateWindowExA_orig, L"CreateWindowExA");

    Wh_Log(L"=== Init complete ===");
    return TRUE;
}

VOID Wh_ModAfterInit()
{
    Wh_Log(L"[PID %d] Applying to existing windows...", GetCurrentProcessId());
    ApplyToAllWindows(TRUE);
    Wh_Log(L"[PID %d] Done", GetCurrentProcessId());
}

// Called by Windhawk when the user saves new settings in the UI.
// Re-reads the current theme and repaints windows with foreground priority.
VOID Wh_ModSettingsChanged()
{
    Wh_Log(L"[PID %d] Settings changed - reapplying...", GetCurrentProcessId());
    g_isDarkMode = IsSystemDarkMode();
    ApplyToForegroundWindow();
    ApplyToAllWindows(FALSE);
    Wh_Log(L"[PID %d] Reapply done", GetCurrentProcessId());
}

static BOOL CALLBACK UninitEnumWindowsProc(HWND hWnd, LPARAM)
{
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hWnd, &pid) || pid != GetCurrentProcessId())
        return TRUE;
    if (!IsWindowEligible(hWnd)) return TRUE;

    BOOL off = FALSE;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &off, sizeof(off));

    const COLORREF def = DWMWA_COLOR_DEFAULT;
    DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &def, sizeof(def));

    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
        SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE |
        SWP_NOZORDER | SWP_NOOWNERZORDER);

    return TRUE;
}

VOID Wh_ModUninit()
{
    Wh_Log(L"[PID %d] Uninit - restoring system defaults", GetCurrentProcessId());
    EnumWindows(UninitEnumWindowsProc, 0);
    Wh_Log(L"[PID %d] Cleanup complete", GetCurrentProcessId());
}
