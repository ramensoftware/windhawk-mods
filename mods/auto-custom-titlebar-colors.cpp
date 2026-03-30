// ==WindhawkMod==
// @id              auto-custom-titlebar-colours
// @name            Auto Custom Titlebar Colors
// @description     Auto-switches titlebar dark/light mode with the Windows theme, with separate custom colours for active/inactive windows in both modes
// @version         1.0.0
// @author          Lone
// @github			https://github.com/Louis047
// @include         *
// @exclude         devenv.exe
// @compilerOptions -ldwmapi -luxtheme -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Auto Dark Titlebar with Custom Colours

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

## Colour Format
- **Hex mode**: enter a 6-character hex string, e.g. `FF0000` for red (no `#` prefix)
- **RGB mode**: enter R, G, B as integers 0-255 in separate fields

Custom colours are only applied when the corresponding "Use Custom Colours" toggle is enabled.

## Notes
- `systemsettings.exe` and `applicationframehost.exe` are excluded to avoid conflicts
- No forced repaint is issued while a mouse button is held (prevents drag-state corruption)
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
// Process exclusion (cached after first call)
// -----------------------------------------------------------------------------

BOOL IsProcessExcluded()
{
    static int cached = -1;
    if (cached != -1) return cached == 1;

    WCHAR exePath[MAX_PATH];
    if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) == 0) {
        cached = 0;
        return FALSE;
    }

    WCHAR* fileName = wcsrchr(exePath, L'\\');
    fileName = fileName ? fileName + 1 : exePath;
    for (WCHAR* p = fileName; *p; p++) *p = towlower(*p);

    static const WCHAR* excluded[] = {
        L"systemsettings.exe",
        L"applicationframehost.exe",
        nullptr
    };

    for (int i = 0; excluded[i]; i++) {
        if (wcscmp(fileName, excluded[i]) == 0) {
            Wh_Log(L"Process excluded: %s", fileName);
            cached = 1;
            return TRUE;
        }
    }

    cached = 0;
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

    // Must be exactly 6 characters, all valid hex digits
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

    // result is 0xRRGGBB, COLORREF is 0x00BBGGRR
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
    const WCHAR* modePrefix  = isDarkMode ? L"darkMode"      : L"lightMode";
    const WCHAR* statePrefix = isActive   ? L"activeColour"  : L"inactiveColour";

    WCHAR useHexKey[64], hexKey[64], rKey[64], gKey[64], bKey[64];
    wsprintfW(useHexKey, L"%s.useHex",    modePrefix);
    wsprintfW(hexKey,    L"%s.%s.hex",    modePrefix, statePrefix);
    wsprintfW(rKey,      L"%s.%s.r",      modePrefix, statePrefix);
    wsprintfW(gKey,      L"%s.%s.g",      modePrefix, statePrefix);
    wsprintfW(bKey,      L"%s.%s.b",      modePrefix, statePrefix);

    BOOL useHex = (BOOL)Wh_GetIntSetting(useHexKey);

    if (useHex) {
        // Correct API: returns PCWSTR, must be freed with Wh_FreeStringSetting
        PCWSTR hexVal = Wh_GetStringSetting(hexKey);
        COLORREF colour = 0;
        BOOL ok = HexToColorref(hexVal, &colour);
        Wh_FreeStringSetting(hexVal);

        if (ok) return colour;

        // Bad hex input: log and fall through to RGB
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

    if (forceRedraw && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
            SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE |
            SWP_NOZORDER | SWP_NOOWNERZORDER);
    }
}

// -----------------------------------------------------------------------------
// Enumerate all eligible windows in the current process
// -----------------------------------------------------------------------------

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM)
{
    HWND parent = GetAncestor(hWnd, GA_PARENT);
    if (parent && parent != GetDesktopWindow()) return TRUE;

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hWnd, &pid) || pid != GetCurrentProcessId())
        return TRUE;

    BOOL isActive = (GetForegroundWindow() == hWnd);
    ApplyTitleBar(hWnd, isActive, TRUE);
    return TRUE;
}

static VOID ApplyToAllWindows()
{
    EnumWindows(EnumWindowsProc, 0);
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
            ApplyTitleBar(hWnd, isActive, TRUE);
            break;
        }
        case WM_NCACTIVATE:
        {
            ApplyTitleBar(hWnd, (BOOL)wParam, TRUE);
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
                ApplyToAllWindows();
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
    if (!IsProcessExcluded())
        HandleWindowMessage(hWnd, Msg, wParam, lParam);
    return result;
}

using DefWindowProcA_t = decltype(&DefWindowProcA);
static DefWindowProcA_t DefWindowProcA_orig;

LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcA_orig(hWnd, Msg, wParam, lParam);
    if (!IsProcessExcluded())
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
    if (!IsProcessExcluded() && Msg == WM_NCACTIVATE)
        ApplyTitleBar(hWnd, (BOOL)wParam, TRUE);
    return result;
}

using DefDlgProcA_t = decltype(&DefDlgProcA);
static DefDlgProcA_t DefDlgProcA_orig;

LRESULT WINAPI DefDlgProcA_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcA_orig(hWnd, Msg, wParam, lParam);
    if (!IsProcessExcluded() && Msg == WM_NCACTIVATE)
        ApplyTitleBar(hWnd, (BOOL)wParam, TRUE);
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

    if (hWnd && !IsProcessExcluded())
        ApplyTitleBar(hWnd, FALSE, FALSE);

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

    if (hWnd && !IsProcessExcluded())
        ApplyTitleBar(hWnd, FALSE, FALSE);

    return hWnd;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    Wh_Log(L"=== Auto Dark Titlebar with Custom Colours - Init [PID %d] ===",
        GetCurrentProcessId());

    if (IsProcessExcluded()) {
        Wh_Log(L"Process excluded - skipping hooks");
        return TRUE;
    }

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
    if (IsProcessExcluded()) return;

    Wh_Log(L"[PID %d] Applying to existing windows...", GetCurrentProcessId());
    ApplyToAllWindows();
    Wh_Log(L"[PID %d] Done", GetCurrentProcessId());
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
    if (IsProcessExcluded()) return;

    Wh_Log(L"[PID %d] Uninit - restoring system defaults", GetCurrentProcessId());
    EnumWindows(UninitEnumWindowsProc, 0);
    Wh_Log(L"[PID %d] Cleanup complete", GetCurrentProcessId());
}
