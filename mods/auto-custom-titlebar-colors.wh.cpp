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
- MDI frame windows (e.g. `mmc.exe`, Event Viewer, Device Manager) handled via `DefFrameProc` hooks
- Live settings reload: colour changes apply instantly without restarting the process
- **Exclude Mozilla Browsers** toggle (default: on) -- skips Firefox, Zen Browser, Floorp, etc.; disable to re-enable titlebar colouring for those browsers

## Colour Format
- **Hex mode**: enter a 6-character hex string, e.g. `FF0000` for red (no `#` prefix)
- **RGB mode**: enter R, G, B as integers 0-255 in separate fields

Custom colours are only applied when the corresponding "Use Custom Colours" toggle is enabled.

## Notes
- UWP/WinUI windows (`ApplicationFrameWindow`, WinUI 3, XAML islands) are automatically detected and skipped to avoid conflicts
- Windows whose caption colour is set by the application itself have that colour left untouched; only the dark/light mode is kept in sync
- Visual attributes apply seamlessly on window activation and theme changes
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
#include <windhawk_utils.h>
#include <algorithm>
#include <cstring>
#include <cwchar>
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
    std::atomic<BOOL> excludeMozilla;
};
static ModSettings g_settings;

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

typedef bool (WINAPI* pShouldAppsUseDarkMode)();
static std::atomic<BOOL> g_isDarkMode = FALSE;

// App-controlled window state tracking
static std::unordered_set<HWND> g_appControlledWindows;
static std::mutex g_appControlledMutex;
static thread_local bool g_inMod = false;

struct InModGuard {
    bool prev = g_inMod;
    InModGuard() { g_inMod = true; }
    ~InModGuard() { g_inMod = prev; }
};

static std::mutex g_settingsMutex;

static std::unordered_map<HWND, BOOL> g_eligibilityCache;
static std::mutex g_eligibilityMutex;

static std::unordered_map<HWND, BOOL> g_appDarkModeWindows;
static std::mutex g_appDarkModeMutex;

static std::unordered_set<HWND> g_appliedWindows;
static std::mutex g_appliedMutex;

// -----------------------------------------------------------------------------
// Dark mode detection
// -----------------------------------------------------------------------------

static BOOL IsSystemDarkMode()
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

    static pShouldAppsUseDarkMode fn = []() -> pShouldAppsUseDarkMode {
        HMODULE hUxtheme = GetModuleHandleW(L"uxtheme.dll");
        return hUxtheme ? (pShouldAppsUseDarkMode)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(132)) : nullptr;
    }();
    if (fn)
        return fn() != 0;

    return FALSE;
}

// -----------------------------------------------------------------------------
// Window eligibility
// -----------------------------------------------------------------------------

static BOOL CALLBACK CheckXamlChildProc(HWND hwnd, LPARAM lParam) {
    WCHAR className[256];
    if (GetClassNameW(hwnd, className, 256)) {
        if (wcscmp(className, L"DesktopWindowXamlSource") == 0 ||
            wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
            wcscmp(className, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0) {
            *(BOOL*)lParam = TRUE;
            return FALSE; // Stop enumerating, we found it!
        }
    }
    return TRUE; // Continue enumerating
}

static BOOL IsWindowEligible(HWND hWnd, BOOL allowCacheUpdate = TRUE)
{
    if (!hWnd || !IsWindow(hWnd)) return FALSE;

    LONG style   = GetWindowLongW(hWnd, GWL_STYLE);
    LONG styleEx = GetWindowLongW(hWnd, GWL_EXSTYLE);

    if ((style & WS_CAPTION) != WS_CAPTION) return FALSE;
    if (styleEx & WS_EX_TOOLWINDOW)  return FALSE;
    if (style & WS_CHILD)            return FALSE;

    WCHAR className[256];
    if (GetClassNameW(hWnd, className, 256)) {
        if (wcscmp(className, L"ApplicationFrameWindow") == 0 ||
            wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
            wcscmp(className, L"DesktopWindowXamlSource") == 0 ||
            wcscmp(className, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0 ||
            wcsncmp(className, L"WinUIDesktopWin32WindowClass", 28) == 0) {
            return FALSE;
        }
        if (g_settings.excludeMozilla && wcsncmp(className, L"Mozilla", 7) == 0) {
            return FALSE;
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_eligibilityMutex);
        auto it = g_eligibilityCache.find(hWnd);
        if (it != g_eligibilityCache.end()) return it->second;
    }

    // Detect WinUI 3 / UWP modern apps and complex composite windows
    // These apps host their modern UI inside specific child bridge windows.
    BOOL hasXaml = FALSE;
    EnumChildWindows(hWnd, CheckXamlChildProc, (LPARAM)&hasXaml);
    
    BOOL isEligible = !hasXaml;
    
    if (allowCacheUpdate) {
        if (!isEligible || IsWindowVisible(hWnd)) {
            std::lock_guard<std::mutex> lock(g_eligibilityMutex);
            g_eligibilityCache[hWnd] = isEligible;
        }
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
        WindhawkUtils::StringSetting hexActive = WindhawkUtils::StringSetting::make(L"lightMode.activeColour.hex");
        if (!HexToColorref(hexActive.get(), &g_settings.activeLight)) g_settings.activeLight = RGB(255, 255, 255);

        WindhawkUtils::StringSetting hexInactive = WindhawkUtils::StringSetting::make(L"lightMode.inactiveColour.hex");
        if (!HexToColorref(hexInactive.get(), &g_settings.inactiveLight)) g_settings.inactiveLight = RGB(230, 230, 230);
    } else {
        g_settings.activeLight = RGB(
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"lightMode.activeColour.r"), 0, 255),
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"lightMode.activeColour.g"), 0, 255),
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"lightMode.activeColour.b"), 0, 255)
        );
        g_settings.inactiveLight = RGB(
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"lightMode.inactiveColour.r"), 0, 255),
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"lightMode.inactiveColour.g"), 0, 255),
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"lightMode.inactiveColour.b"), 0, 255)
        );
    }

    BOOL useHexDark = (BOOL)Wh_GetIntSetting(L"darkMode.useHex");
    if (useHexDark) {
        WindhawkUtils::StringSetting hexActive = WindhawkUtils::StringSetting::make(L"darkMode.activeColour.hex");
        if (!HexToColorref(hexActive.get(), &g_settings.activeDark)) g_settings.activeDark = RGB(32, 32, 32);

        WindhawkUtils::StringSetting hexInactive = WindhawkUtils::StringSetting::make(L"darkMode.inactiveColour.hex");
        if (!HexToColorref(hexInactive.get(), &g_settings.inactiveDark)) g_settings.inactiveDark = RGB(50, 50, 50);
    } else {
        g_settings.activeDark = RGB(
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"darkMode.activeColour.r"), 0, 255),
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"darkMode.activeColour.g"), 0, 255),
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"darkMode.activeColour.b"), 0, 255)
        );
        g_settings.inactiveDark = RGB(
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"darkMode.inactiveColour.r"), 0, 255),
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"darkMode.inactiveColour.g"), 0, 255),
            (BYTE)std::clamp((int)Wh_GetIntSetting(L"darkMode.inactiveColour.b"), 0, 255)
        );
    }
}

// -----------------------------------------------------------------------------
// Core: apply dark-mode attribute + caption colour to one window
// -----------------------------------------------------------------------------

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
static DwmSetWindowAttribute_t DwmSetWindowAttribute_orig;

static HRESULT WINAPI DwmSetWindowAttribute_hook(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
    HRESULT hr = DwmSetWindowAttribute_orig(hwnd, dwAttribute, pvAttribute, cbAttribute);
    if (!g_inMod && SUCCEEDED(hr)) {
        if (dwAttribute == DWMWA_CAPTION_COLOR && pvAttribute && cbAttribute == sizeof(COLORREF)) {
            COLORREF colour = *(COLORREF*)pvAttribute;
            std::lock_guard<std::mutex> lock(g_appControlledMutex);
            if (colour == DWMWA_COLOR_DEFAULT) {
                g_appControlledWindows.erase(hwnd); // app released control
            } else {
                g_appControlledWindows.insert(hwnd);
                Wh_Log(L"App controls DWMWA_CAPTION_COLOR for hWnd=%p", hwnd);
            }
        } else if (dwAttribute == DWMWA_USE_IMMERSIVE_DARK_MODE && pvAttribute && cbAttribute == sizeof(BOOL)) {
            std::lock_guard<std::mutex> lock(g_appDarkModeMutex);
            g_appDarkModeWindows[hwnd] = *(BOOL*)pvAttribute;
        }
    }
    return hr;
}

static VOID ApplyTitleBar(HWND hWnd, BOOL isActive, BOOL allowCacheUpdate = TRUE)
{
    if (!IsWindowEligible(hWnd, allowCacheUpdate)) {
        bool wasApplied;
        {
            std::lock_guard<std::mutex> lock(g_appliedMutex);
            wasApplied = g_appliedWindows.erase(hWnd) != 0;
        }
        if (!wasApplied) return;

        BOOL dark = FALSE;
        {
            std::lock_guard<std::mutex> lockDark(g_appDarkModeMutex);
            auto it = g_appDarkModeWindows.find(hWnd);
            if (it != g_appDarkModeWindows.end()) dark = it->second;
        }

        bool appOwnsColour;
        {
            std::lock_guard<std::mutex> lock(g_appControlledMutex);
            appOwnsColour = g_appControlledWindows.count(hWnd) != 0;
        }

        InModGuard guard;
        DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
        if (!appOwnsColour) {
            const COLORREF def = DWMWA_COLOR_DEFAULT;
            DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &def, sizeof(def));
        }
        return;
    }

    bool appOwnsColour;
    {
        std::lock_guard<std::mutex> lock(g_appControlledMutex);
        appOwnsColour = g_appControlledWindows.count(hWnd) != 0;
    }

    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        g_appliedWindows.insert(hWnd);
    }

    InModGuard guard;
    BOOL darkMode = g_isDarkMode;
    
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE,
        &darkMode, sizeof(darkMode));

    if (appOwnsColour) {
        return;
    }

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
        Wh_Log(L"ApplyTitleBar: hWnd=%p dark=%d active=%d colour=#%02X%02X%02X",
            hWnd, (BOOL)g_isDarkMode, isActive,
            GetRValue(colour), GetGValue(colour), GetBValue(colour));
    } else {
        const COLORREF def = DWMWA_COLOR_DEFAULT;
        DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &def, sizeof(def));
        Wh_Log(L"ApplyTitleBar: hWnd=%p dark=%d active=%d colour=DEFAULT",
            hWnd, (BOOL)g_isDarkMode, isActive);
    }
}

// -----------------------------------------------------------------------------
// Enumerate all eligible windows in the current process

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM)
{
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hWnd, &pid) || pid != GetCurrentProcessId())
        return TRUE;

    BOOL isActive = (GetForegroundWindow() == hWnd);
    ApplyTitleBar(hWnd, isActive);
    return TRUE;
}

static VOID ApplyToAllWindows()
{
    EnumWindows(EnumWindowsProc, 0);
}

// -----------------------------------------------------------------------------
// Shared message handler
// -----------------------------------------------------------------------------

static VOID HandleWindowMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, BOOL isAnsi = FALSE)
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
                std::lock_guard<std::mutex> lock(g_appliedMutex);
                g_appliedWindows.erase(hWnd);
            }
            break;
        }
        case WM_ACTIVATE:
        {
            BOOL isActive = (LOWORD(wParam) != WA_INACTIVE);
            ApplyTitleBar(hWnd, isActive);
            break;
        }
        case WM_NCACTIVATE:
        {
            BOOL isActive = (BOOL)wParam;
            ApplyTitleBar(hWnd, isActive);
            break;
        }
        case WM_DWMCOLORIZATIONCOLORCHANGED:
        {
            BOOL isActive = (GetForegroundWindow() == hWnd);
            ApplyTitleBar(hWnd, isActive);
            break;
        }
        case WM_SETTINGCHANGE:
        {
            // Only respond to the theme change notification — other WM_SETTINGCHANGE
            // broadcasts (work area, metrics, etc.) do not require a re-check.
            BOOL isThemeChange = lParam && (
                isAnsi
                    ? strcmp((LPCSTR)lParam, "ImmersiveColorSet") == 0
                    : wcscmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0
            );
            if (!isThemeChange) break;

            BOOL newDarkMode = IsSystemDarkMode();
            if (g_isDarkMode.exchange(newDarkMode) != newDarkMode) {
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

static LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcW_orig(hWnd, Msg, wParam, lParam);
    HandleWindowMessage(hWnd, Msg, wParam, lParam, FALSE);
    return result;
}

using DefWindowProcA_t = decltype(&DefWindowProcA);
static DefWindowProcA_t DefWindowProcA_orig;

static LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcA_orig(hWnd, Msg, wParam, lParam);
    HandleWindowMessage(hWnd, Msg, wParam, lParam, TRUE);
    return result;
}

// -----------------------------------------------------------------------------
// Hook: DefDlgProcW / DefDlgProcA
// -----------------------------------------------------------------------------

using DefDlgProcW_t = decltype(&DefDlgProcW);
static DefDlgProcW_t DefDlgProcW_orig;

static LRESULT WINAPI DefDlgProcW_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcW_orig(hWnd, Msg, wParam, lParam);
    HandleWindowMessage(hWnd, Msg, wParam, lParam, FALSE);
    return result;
}

using DefDlgProcA_t = decltype(&DefDlgProcA);
static DefDlgProcA_t DefDlgProcA_orig;

static LRESULT WINAPI DefDlgProcA_hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefDlgProcA_orig(hWnd, Msg, wParam, lParam);
    HandleWindowMessage(hWnd, Msg, wParam, lParam, TRUE);
    return result;
}

// -----------------------------------------------------------------------------
// Hook: DefFrameProcW / DefFrameProcA (MDI Frame windows, e.g. MMC)
// -----------------------------------------------------------------------------

using DefFrameProcW_t = decltype(&DefFrameProcW);
static DefFrameProcW_t DefFrameProcW_orig;

static LRESULT WINAPI DefFrameProcW_hook(HWND hWnd, HWND hWndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefFrameProcW_orig(hWnd, hWndMDIClient, Msg, wParam, lParam);
    HandleWindowMessage(hWnd, Msg, wParam, lParam, FALSE);
    return result;
}

using DefFrameProcA_t = decltype(&DefFrameProcA);
static DefFrameProcA_t DefFrameProcA_orig;

static LRESULT WINAPI DefFrameProcA_hook(HWND hWnd, HWND hWndMDIClient, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefFrameProcA_orig(hWnd, hWndMDIClient, Msg, wParam, lParam);
    HandleWindowMessage(hWnd, Msg, wParam, lParam, TRUE);
    return result;
}

// -----------------------------------------------------------------------------
// Hook: CreateWindowExW / CreateWindowExA
// -----------------------------------------------------------------------------

static void HandleCreatedWindow(HWND hWnd)
{
    if (!hWnd) return;

    WCHAR className[256];
    if (GetClassNameW(hWnd, className, 256) &&
        (wcscmp(className, L"DesktopWindowXamlSource") == 0 ||
         wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
         wcscmp(className, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0)) {
        HWND hRoot = GetAncestor(hWnd, GA_ROOT);
        if (hRoot) {
            // Mark root ineligible in the cache, then let ApplyTitleBar's revert
            // path do the full cleanup of both DWM attributes consistently.
            {
                std::lock_guard<std::mutex> lock(g_eligibilityMutex);
                g_eligibilityCache[hRoot] = FALSE;
            }
            ApplyTitleBar(hRoot, FALSE);
        }
    }
    BOOL isActive = (GetForegroundWindow() == hWnd);
    ApplyTitleBar(hWnd, isActive, FALSE);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_orig;

static HWND WINAPI CreateWindowExW_hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = CreateWindowExW_orig(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    HandleCreatedWindow(hWnd);
    return hWnd;
}

using CreateWindowExA_t = decltype(&CreateWindowExA);
static CreateWindowExA_t CreateWindowExA_orig;

static HWND WINAPI CreateWindowExA_hook(
    DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = CreateWindowExA_orig(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    HandleCreatedWindow(hWnd);
    return hWnd;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

template <typename TargetType, typename ReplacementType, typename OrigType>
static bool Hook(TargetType target, ReplacementType replacement, OrigType orig, const wchar_t* name) {
    if (!WindhawkUtils::SetFunctionHook(target, replacement, orig)) {
        Wh_Log(L"WARNING: Failed to hook %s", name);
        return false;
    }
    Wh_Log(L"Hooked %s", name);
    return true;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"=== Auto Custom Titlebar Colors - Init [PID %d] ===",
        GetCurrentProcessId());

    LoadSettings();
    g_isDarkMode = IsSystemDarkMode();
    Wh_Log(L"Initial theme: %s", (BOOL)g_isDarkMode ? L"DARK" : L"LIGHT");

    bool success = true;
    success &= Hook(DefWindowProcW,  DefWindowProcW_hook,  &DefWindowProcW_orig,  L"DefWindowProcW");
    success &= Hook(DefWindowProcA,  DefWindowProcA_hook,  &DefWindowProcA_orig,  L"DefWindowProcA");
    success &= Hook(DefDlgProcW,     DefDlgProcW_hook,     &DefDlgProcW_orig,     L"DefDlgProcW");
    success &= Hook(DefDlgProcA,     DefDlgProcA_hook,     &DefDlgProcA_orig,     L"DefDlgProcA");
    success &= Hook(DefFrameProcW,   DefFrameProcW_hook,   &DefFrameProcW_orig,   L"DefFrameProcW");
    success &= Hook(DefFrameProcA,   DefFrameProcA_hook,   &DefFrameProcA_orig,   L"DefFrameProcA");
    success &= Hook(CreateWindowExW, CreateWindowExW_hook, &CreateWindowExW_orig, L"CreateWindowExW");
    success &= Hook(CreateWindowExA, CreateWindowExA_hook, &CreateWindowExA_orig, L"CreateWindowExA");
    success &= Hook(DwmSetWindowAttribute, DwmSetWindowAttribute_hook, &DwmSetWindowAttribute_orig, L"DwmSetWindowAttribute");

    if (!success) {
        Wh_Log(L"=== Init failed: one or more hooks could not be set ===");
        return FALSE;
    }

    Wh_Log(L"=== Init complete ===");
    return TRUE;
}

VOID Wh_ModAfterInit()
{
    Wh_Log(L"[PID %d] Applying to existing windows...", GetCurrentProcessId());
    ApplyToAllWindows();
    Wh_Log(L"[PID %d] Done", GetCurrentProcessId());
}

// Called by Windhawk when the user saves new settings in the UI.
// Re-reads the current theme and repaints windows with foreground priority.
VOID Wh_ModSettingsChanged()
{
    Wh_Log(L"[PID %d] Settings changed - reapplying...", GetCurrentProcessId());
    LoadSettings();
    g_isDarkMode = IsSystemDarkMode();
    ApplyToAllWindows();
    Wh_Log(L"[PID %d] Reapply done", GetCurrentProcessId());
}

VOID Wh_ModUninit()
{
    Wh_Log(L"[PID %d] Uninit - restoring system defaults", GetCurrentProcessId());

    // Swap out the applied set atomically, then iterate over the captured snapshot.
    // This avoids holding g_appliedMutex across DWM calls (cross-process, potentially slow).
    std::unordered_set<HWND> applied;
    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        applied.swap(g_appliedWindows);
    }

    for (HWND hWnd : applied) {
        if (!IsWindow(hWnd)) continue;

        BOOL dark = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_appDarkModeMutex);
            auto it = g_appDarkModeWindows.find(hWnd);
            if (it != g_appDarkModeWindows.end()) dark = it->second;
        }

        bool appOwnsColour;
        {
            std::lock_guard<std::mutex> lock(g_appControlledMutex);
            appOwnsColour = g_appControlledWindows.count(hWnd) != 0;
        }

        InModGuard guard;
        DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
        if (!appOwnsColour) {
            const COLORREF def = DWMWA_COLOR_DEFAULT;
            DwmSetWindowAttribute(hWnd, DWMWA_CAPTION_COLOR, &def, sizeof(def));
        }
    }

    Wh_Log(L"[PID %d] Cleanup complete", GetCurrentProcessId());
}
