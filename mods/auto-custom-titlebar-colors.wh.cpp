// ==WindhawkMod==
// @id              auto-custom-titlebar-colors
// @name            Auto Custom Titlebar Colors
// @description     Auto-switches titlebar dark/light mode with the Windows theme, with separate custom colours for active/inactive windows in both modes
// @version         1.2.0
// @author          Lone
// @github          https://github.com/Louis047
// @include         *
// @exclude         devenv.exe
// @compilerOptions -ldwmapi -luser32
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
- UWP/WinUI windows and Mozilla-based browsers are now auto-detected and skipped
- Window redraws are debounced (50ms minimum) to prevent interference with window managers
- Visual redraws (SetWindowPos) only occur during window activation, not deactivation, to avoid focus-grabbing issues with tiling window managers
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

- excludeMozilla: true
  $name: "Exclude Mozilla Browsers"
  $description: "Skip Firefox, Zen Browser, etc."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <atomic>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// -----------------------------------------------------------------------------
// Settings helpers
// -----------------------------------------------------------------------------

struct ModSettings {
    BOOL useCustomLight;
    BOOL useCustomDark;
    COLORREF activeLight;
    COLORREF inactiveLight;
    COLORREF activeDark;
    COLORREF inactiveDark;
    BOOL excludeMozilla;
};
static ModSettings g_settings;

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

typedef HRESULT(WINAPI* pShouldSystemUseDarkMode)();
static pShouldSystemUseDarkMode g_ShouldSystemUseDarkMode = nullptr;
static std::atomic<BOOL> g_isDarkMode = FALSE;

// App-controlled window state tracking
static std::unordered_set<HWND> g_appControlledWindows;
static std::mutex g_appControlledMutex;
thread_local bool g_inMod = false;

static std::mutex g_settingsMutex;

static std::unordered_map<HWND, BOOL> g_eligibilityCache;
static std::mutex g_eligibilityMutex;

static std::unordered_map<HWND, BOOL> g_appDarkModeWindows;
static std::mutex g_appDarkModeMutex;

// Debounce mechanism: track last SetWindowPos time per window to prevent
// rapid successive redraws that can interfere with window managers
static std::unordered_map<HWND, DWORD> g_lastSetWindowPosTime;
static std::mutex g_lastSetWindowPosMutex;
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

struct XamlSearchContext {
    BOOL hasXaml;
};

static BOOL CALLBACK CheckXamlChildProc(HWND hwnd, LPARAM lParam) {
    WCHAR className[256];
    if (GetClassNameW(hwnd, className, 256)) {
        if (wcscmp(className, L"DesktopWindowXamlSource") == 0 ||
            wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
            wcscmp(className, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0) {
            ((XamlSearchContext*)lParam)->hasXaml = TRUE;
            return FALSE; // Stop enumerating, we found it!
        }
    }
    return TRUE; // Continue enumerating
}

BOOL IsWindowEligible(HWND hWnd, BOOL allowCacheUpdate = TRUE)
{
    if (!hWnd || !IsWindow(hWnd)) return FALSE;

    LONG style   = GetWindowLongW(hWnd, GWL_STYLE);
    LONG styleEx = GetWindowLongW(hWnd, GWL_EXSTYLE);

    if (!(style & WS_CAPTION))       return FALSE;
    if (styleEx & WS_EX_TOOLWINDOW)  return FALSE;
    if (style & WS_CHILD)            return FALSE;

    // Fast top-level check for Mozilla/Gecko browsers (Firefox, Zen, Floorp)
    if (g_settings.excludeMozilla) {
        WCHAR className[256];
        if (GetClassNameW(hWnd, className, 256)) {
            if (wcsncmp(className, L"Mozilla", 7) == 0) {
                return FALSE;
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_eligibilityMutex);
        auto it = g_eligibilityCache.find(hWnd);
        if (it != g_eligibilityCache.end()) return it->second;
    }

    // Detect WinUI 3 / UWP modern apps and complex composite windows
    // These apps host their modern UI inside specific child bridge windows.
    XamlSearchContext ctx = { FALSE };
    EnumChildWindows(hWnd, CheckXamlChildProc, (LPARAM)&ctx);
    
    BOOL isEligible = !ctx.hasXaml;
    
    if (allowCacheUpdate) {
        std::lock_guard<std::mutex> lock(g_eligibilityMutex);
        g_eligibilityCache[hWnd] = isEligible;
    }

    return isEligible;
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

static void LoadSettings()
{
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    g_settings.useCustomLight = (BOOL)Wh_GetIntSetting(L"customColours.light");
    g_settings.useCustomDark = (BOOL)Wh_GetIntSetting(L"customColours.dark");
    g_settings.excludeMozilla = (BOOL)Wh_GetIntSetting(L"excludeMozilla");

    BOOL useHexLight = (BOOL)Wh_GetIntSetting(L"lightMode.useHex");
    if (useHexLight) {
        PCWSTR hexActive = Wh_GetStringSetting(L"lightMode.activeColour.hex");
        if (!HexToColorref(hexActive, &g_settings.activeLight)) g_settings.activeLight = RGB(255, 255, 255);
        Wh_FreeStringSetting(hexActive);

        PCWSTR hexInactive = Wh_GetStringSetting(L"lightMode.inactiveColour.hex");
        if (!HexToColorref(hexInactive, &g_settings.inactiveLight)) g_settings.inactiveLight = RGB(230, 230, 230);
        Wh_FreeStringSetting(hexInactive);
    } else {
        g_settings.activeLight = RGB(
            (BYTE)Wh_GetIntSetting(L"lightMode.activeColour.r"),
            (BYTE)Wh_GetIntSetting(L"lightMode.activeColour.g"),
            (BYTE)Wh_GetIntSetting(L"lightMode.activeColour.b")
        );
        g_settings.inactiveLight = RGB(
            (BYTE)Wh_GetIntSetting(L"lightMode.inactiveColour.r"),
            (BYTE)Wh_GetIntSetting(L"lightMode.inactiveColour.g"),
            (BYTE)Wh_GetIntSetting(L"lightMode.inactiveColour.b")
        );
    }

    BOOL useHexDark = (BOOL)Wh_GetIntSetting(L"darkMode.useHex");
    if (useHexDark) {
        PCWSTR hexActive = Wh_GetStringSetting(L"darkMode.activeColour.hex");
        if (!HexToColorref(hexActive, &g_settings.activeDark)) g_settings.activeDark = RGB(32, 32, 32);
        Wh_FreeStringSetting(hexActive);

        PCWSTR hexInactive = Wh_GetStringSetting(L"darkMode.inactiveColour.hex");
        if (!HexToColorref(hexInactive, &g_settings.inactiveDark)) g_settings.inactiveDark = RGB(50, 50, 50);
        Wh_FreeStringSetting(hexInactive);
    } else {
        g_settings.activeDark = RGB(
            (BYTE)Wh_GetIntSetting(L"darkMode.activeColour.r"),
            (BYTE)Wh_GetIntSetting(L"darkMode.activeColour.g"),
            (BYTE)Wh_GetIntSetting(L"darkMode.activeColour.b")
        );
        g_settings.inactiveDark = RGB(
            (BYTE)Wh_GetIntSetting(L"darkMode.inactiveColour.r"),
            (BYTE)Wh_GetIntSetting(L"darkMode.inactiveColour.g"),
            (BYTE)Wh_GetIntSetting(L"darkMode.inactiveColour.b")
        );
    }
}

// -----------------------------------------------------------------------------
// Core: apply dark-mode attribute + caption colour to one window
// -----------------------------------------------------------------------------

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
static DwmSetWindowAttribute_t DwmSetWindowAttribute_orig;

HRESULT WINAPI DwmSetWindowAttribute_hook(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
    if (!g_inMod) {
        if (dwAttribute == DWMWA_CAPTION_COLOR) {
            std::lock_guard<std::mutex> lock(g_appControlledMutex);
            g_appControlledWindows.insert(hwnd);
            Wh_Log(L"App controls DWMWA_CAPTION_COLOR for hWnd=%p", hwnd);
        } else if (dwAttribute == DWMWA_USE_IMMERSIVE_DARK_MODE && pvAttribute && cbAttribute == sizeof(BOOL)) {
            std::lock_guard<std::mutex> lock(g_appDarkModeMutex);
            g_appDarkModeWindows[hwnd] = *(BOOL*)pvAttribute;
        }
    }
    return DwmSetWindowAttribute_orig(hwnd, dwAttribute, pvAttribute, cbAttribute);
}

static VOID ApplyTitleBar(HWND hWnd, BOOL isActive, BOOL forceRedraw, BOOL allowCacheUpdate = TRUE)
{
    if (!IsWindowEligible(hWnd, allowCacheUpdate)) return;

    {
        std::lock_guard<std::mutex> lock(g_appControlledMutex);
        if (g_appControlledWindows.count(hWnd)) return;
    }

    g_inMod = true;
    BOOL darkMode = g_isDarkMode;
    
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE,
        &darkMode, sizeof(darkMode));

    BOOL useCustom;
    COLORREF colour;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        useCustom = g_isDarkMode ? g_settings.useCustomDark : g_settings.useCustomLight;
        colour = g_isDarkMode 
            ? (isActive ? g_settings.activeDark : g_settings.inactiveDark)
            : (isActive ? g_settings.activeLight : g_settings.inactiveLight);
    }

    if (useCustom) {
        DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR,
            &colour, sizeof(colour));
        Wh_Log(L"ApplyTitleBar: hWnd=%p dark=%d active=%d colour=#%06X",
            hWnd, (BOOL)g_isDarkMode, isActive, colour);
    } else {
        const COLORREF def = DWMWA_COLOR_DEFAULT;
        DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &def, sizeof(def));
        Wh_Log(L"ApplyTitleBar: hWnd=%p dark=%d active=%d colour=DEFAULT",
            hWnd, (BOOL)g_isDarkMode, isActive);
    }
    
    g_inMod = false;

    // Debounce SetWindowPos calls to prevent interference with window managers.
    // Only allow redraws if enough time has passed since the last redraw.
    if (forceRedraw && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        DWORD currentTime = GetTickCount();
        std::lock_guard<std::mutex> lock(g_lastSetWindowPosMutex);
        DWORD lastTime = g_lastSetWindowPosTime[hWnd];
        DWORD timeSinceLastRedraw = currentTime - lastTime;
        
        if (timeSinceLastRedraw >= SETWINDOWPOS_DEBOUNCE_MS || lastTime == 0) {
            SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
                SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE |
                SWP_NOZORDER | SWP_NOOWNERZORDER);
            g_lastSetWindowPosTime[hWnd] = currentTime;
        }
    }
}

// -----------------------------------------------------------------------------
// Enumerate all eligible windows in the current process

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hWnd, &pid) || pid != GetCurrentProcessId())
        return TRUE;

    BOOL isActive = (GetForegroundWindow() == hWnd);
    
    // Only refresh with SetWindowPos if it's the active window to minimize
    // interference with tiling window managers
    BOOL forceRedraw = (BOOL)lParam;
    ApplyTitleBar(hWnd, isActive, forceRedraw && isActive);
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
        case WM_NCDESTROY:
        {
            {
                std::lock_guard<std::mutex> lock(g_appControlledMutex);
                g_appControlledWindows.erase(hWnd);
            }
            {
                std::lock_guard<std::mutex> lock(g_eligibilityMutex);
                g_eligibilityCache.erase(hWnd);
            }
            {
                std::lock_guard<std::mutex> lock(g_appDarkModeMutex);
                g_appDarkModeWindows.erase(hWnd);
            }
            {
                std::lock_guard<std::mutex> lock(g_lastSetWindowPosMutex);
                g_lastSetWindowPosTime.erase(hWnd);
            }
            break;
        }
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
    HandleWindowMessage(hWnd, Msg, wParam, lParam);
    return result;
}

using DefDlgProcA_t = decltype(&DefDlgProcA);
static DefDlgProcA_t DefDlgProcA_orig;

LRESULT WINAPI DefDlgProcA_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcA_orig(hWnd, Msg, wParam, lParam);
    HandleWindowMessage(hWnd, Msg, wParam, lParam);
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

    if (hWnd) {
        if (lpClassName && !IS_INTRESOURCE(lpClassName) && (
            wcscmp(lpClassName, L"DesktopWindowXamlSource") == 0 ||
            wcscmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0 ||
            wcscmp(lpClassName, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0)) {
            HWND hRoot = GetAncestor(hWnd, GA_ROOT);
            if (hRoot) {
                {
                    std::lock_guard<std::mutex> lock(g_eligibilityMutex);
                    g_eligibilityCache[hRoot] = FALSE;
                }
                const COLORREF def = DWMWA_COLOR_DEFAULT;
                DwmSetWindowAttribute(hRoot, DWMWA_CAPTION_COLOR, &def, sizeof(def));
            }
        }
        ApplyTitleBar(hWnd, FALSE, FALSE, FALSE);
    }
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

    if (hWnd) {
        if (lpClassName && !IS_INTRESOURCE(lpClassName)) {
            WCHAR classNameW[256];
            if (MultiByteToWideChar(CP_ACP, 0, lpClassName, -1, classNameW, 256)) {
                if (wcscmp(classNameW, L"DesktopWindowXamlSource") == 0 ||
                    wcscmp(classNameW, L"Windows.UI.Core.CoreWindow") == 0 ||
                    wcscmp(classNameW, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0) {
                    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
                    if (hRoot) {
                        {
                            std::lock_guard<std::mutex> lock(g_eligibilityMutex);
                            g_eligibilityCache[hRoot] = FALSE;
                        }
                        const COLORREF def = DWMWA_COLOR_DEFAULT;
                        DwmSetWindowAttribute(hRoot, DWMWA_CAPTION_COLOR, &def, sizeof(def));
                    }
                }
            }
        }
        ApplyTitleBar(hWnd, FALSE, FALSE, FALSE);
    }
    return hWnd;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    Wh_Log(L"=== Auto Custom Titlebar Colors - Init [PID %d] ===",
        GetCurrentProcessId());

    LoadSettings();
    g_isDarkMode = IsSystemDarkMode();
    Wh_Log(L"Initial theme: %s", (BOOL)g_isDarkMode ? L"DARK" : L"LIGHT");

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
    hook((void*)DwmSetWindowAttribute, (void*)DwmSetWindowAttribute_hook, (void**)&DwmSetWindowAttribute_orig, L"DwmSetWindowAttribute");

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
    LoadSettings();
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
    if (!IsWindowEligible(hWnd, FALSE)) return TRUE;

    {
        std::lock_guard<std::mutex> lock(g_appControlledMutex);
        if (g_appControlledWindows.count(hWnd)) return TRUE;
    }

    BOOL off = FALSE;
    {
        std::lock_guard<std::mutex> lock(g_appDarkModeMutex);
        auto it = g_appDarkModeWindows.find(hWnd);
        if (it != g_appDarkModeWindows.end()) {
            off = it->second;
        }
    }
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
