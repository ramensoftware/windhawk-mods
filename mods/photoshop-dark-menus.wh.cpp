// ==WindhawkMod==
// @id            photoshop-dark-menus
// @name          Photoshop Dark Menus
// @description   Enables dark mode and custom separator colors for all menus in Adobe Photoshop.
// @version       1.2.0
// @author        Saber Naeemi
// @github        https://github.com/sabergraphics
// @twitter       https://x.com/SaberNaeemi
// @homepage      https://www.sabernaeemi.com
// @include       Photoshop.exe
// @compilerOptions -lUser32 -lGdi32 -lAdvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Photoshop Dark Menus

This Windhawk mod enables dark menus (top-bar dropdowns and context menus) in
Adobe Photoshop on Windows 11, along with custom color settings.

## Screenshots

![Top Bar Menu Dropdown](https://raw.githubusercontent.com/sabergraphics/photoshop-dark-menus/main/images/photoshop-dark-menu-screenshot-1.png)

![Context Menu](https://raw.githubusercontent.com/sabergraphics/photoshop-dark-menus/main/images/photoshop-dark-menu-screenshot-2.png)

## How it works

The mod has two theming engines, selectable in the settings.

### Global system colors (default)

Photoshop renders its legacy Win32 menus through the classic (unthemed) path,
which reads the shared system color table directly — so per-process dark-mode
techniques (`SetPreferredAppMode`, `GetSysColor` hooks) do not reach it. This
engine updates the active Windows session palette with `SetSysColors`, which
is the only approach found to reliably darken Photoshop's menus.

**Warning:** while this mode is active, the menu/selection/3D colors change
for *every* application on the desktop, not just Photoshop, until Photoshop
exits. Safeguards used in this mode:

- Original colors are backed up from `HKCU\Control Panel\Colors` (not from the
  possibly-already-modified live palette), so restarts and multiple instances
  cannot corrupt the backup.
- Colors are restored from an `ExitProcess` teardown hook. If another
  Photoshop instance is still running, restoration is deferred to the last
  instance to exit, so instances no longer fight over the palette.
- If Photoshop **hard-crashes** (faulty plugin, Force Quit via Task Manager),
  the teardown hook cannot run and the desktop may remain dark. Launching and
  cleanly closing Photoshop once restores the original colors.

### Process-local (experimental)

Everything stays inside the Photoshop process; no other application is
affected:

- Switches the process to the dark menu theme via `uxtheme`
  (`SetPreferredAppMode` + `FlushMenuThemes`).
- Hooks `GetSysColor` / `GetSysColorBrush` in-process, so Photoshop's legacy
  menu drawing code picks up the custom menu, text, highlight and disabled-text
  colors without touching the system palette.
- Sets a custom background brush on Photoshop's menus
  (`MENUINFO::hbrBack`, applied to submenus and to context menus via
  `TrackPopupMenu(Ex)` hooks).
- Hooks `PatBlt` / `FillRect`, strictly scoped to active menu windows (class
  `#32768` or modal `GUI_INMENUMODE`), to recolor the separator lines that
  Photoshop draws with legacy GDI calls.

In testing, this mode does **not** darken Photoshop's menu backgrounds and
text, because Photoshop's classic menu rendering bypasses the hooked user-mode
color APIs. It is kept as an opt-in for experimentation and for setups where
it may behave differently.

## Why a standalone mod?

The system-wide [Dark mode context menus](https://github.com/MGGSK/DarkMenus)
mod darkens Win32 menus globally but does not offer per-color customization,
and its approach alone does not cover Photoshop's legacy separator drawing.
This mod adds fully customizable menu colors, Photoshop-scoped GDI separator
hooks that would be unsafe in an `@include *` mod, and a global-palette
fallback for setups where process-local theming is not sufficient.

## Options

- **Theming Mode**: Global system colors (default; affects the whole desktop
  while Photoshop runs) or process-local (experimental; only affects Photoshop
  but does not darken menus on most setups).
- **Menu Background Color**: Background color for all menu popups (Default: `#282828`).
- **Menu Text Color**: Text color for active items (Default: `#DCDCDC`).
- **Highlight Background Color**: Color when hovering over an item (Default: `#505050`).
- **Highlight Text Color**: Text color when hovering over an item (Default: `#FFFFFF`).
- **Separator Line Color**: Color for separator lines. Set to match the background color to hide them completely (Default: `#383838`).
- **Disabled Text Color**: Text color for disabled menu items (Default: `#808080`).
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- ThemingMode: systemColors
  $name: "Theming Mode"
  $description: >-
    Global system colors reliably darkens Photoshop's menus but affects the
    whole desktop while Photoshop runs. Process-local only affects Photoshop
    but does not darken the menus on most setups, because Photoshop's menu
    rendering reads the system color table directly.
  $options:
  - systemColors: "Global system colors (recommended for Photoshop)"
  - processLocal: "Process-local (experimental)"
- MenuBgColor: "#282828"
  $name: "Menu Background Color"
- MenuTextColor: "#DCDCDC"
  $name: "Menu Text Color"
- HighlightBgColor: "#505050"
  $name: "Highlight Background Color"
- HighlightTextColor: "#FFFFFF"
  $name: "Highlight Text Color"
- SeparatorColor: "#383838"
  $name: "Separator Line Color"
- GrayTextColor: "#808080"
  $name: "Disabled Text Color"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <tlhelp32.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <wchar.h>
#include <wctype.h>
#include <vector>
#include <mutex>

constexpr int NUM_ELEMENTS = 10;
const INT g_sysElements[NUM_ELEMENTS] = {
    COLOR_MENU, COLOR_MENUTEXT, COLOR_HIGHLIGHT, COLOR_HIGHLIGHTTEXT,
    COLOR_BTNSHADOW, COLOR_GRAYTEXT, COLOR_BTNHIGHLIGHT,
    COLOR_3DDKSHADOW, COLOR_3DLIGHT, COLOR_MENUBAR
};

COLORREF g_origColors[NUM_ELEMENTS] = {};
bool g_hasSavedOrigColors = false;

// Mode state. Written only from Windhawk callbacks (init / settings / uninit).
bool g_processLocal = false;  // desired mode from settings
bool g_globalActive = false;  // our SetSysColors palette is currently applied
bool g_localActive = false;   // dark app mode is currently applied

// Colors read by the in-process hooks.
COLORREF g_colMenu = RGB(40, 40, 40);
COLORREF g_colText = RGB(220, 220, 220);
COLORREF g_colHighlight = RGB(80, 80, 80);
COLORREF g_colHiText = RGB(255, 255, 255);
COLORREF g_colGray = RGB(128, 128, 128);

// Brushes are published with InterlockedExchangePointer; replaced brushes are
// parked in g_retiredBrushes and only deleted at uninit, so painting threads
// can never use a freed handle.
HBRUSH g_hSeparatorBrush = nullptr;
HBRUSH g_hMenuBrush = nullptr;
HBRUSH g_hTextBrush = nullptr;
HBRUSH g_hHighlightBrush = nullptr;
HBRUSH g_hHiTextBrush = nullptr;
HBRUSH g_hGrayBrush = nullptr;
std::vector<HBRUSH> g_retiredBrushes;
std::mutex g_brushMutex;

HMODULE g_hUxtheme = nullptr;
enum class PreferredAppMode { Default, AllowDark, ForceDark, ForceLight, Max };
using SetPreferredAppMode_t = PreferredAppMode(WINAPI*)(PreferredAppMode);
using FlushMenuThemes_t = void(WINAPI*)();

COLORREF ParseHexColor(PCWSTR hexStr, COLORREF defaultColor) {
    if (!hexStr) {
        Wh_Log(L"ParseHexColor: Null string, using default.");
        return defaultColor;
    }

    const wchar_t* p = hexStr;
    if (*p == L'#') p++; // allow with or without #

    size_t len = wcslen(p);
    bool allHex = len > 0;
    for (size_t i = 0; i < len && allHex; i++) {
        if (!iswxdigit(p[i])) allHex = false;
    }

    unsigned int r, g, b;
    if (allHex && len == 6 && swscanf_s(p, L"%02x%02x%02x", &r, &g, &b) == 3) {
        return RGB(r, g, b);
    } else if (allHex && len == 3 && swscanf_s(p, L"%1x%1x%1x", &r, &g, &b) == 3) {
        return RGB(r * 17, g * 17, b * 17); // expand short hex
    }

    Wh_Log(L"ParseHexColor: Invalid format '%s', using default.", hexStr);
    return defaultColor;
}

COLORREF GetColorFromRegistry(int sysElement, COLORREF liveColorFallback) {
    const wchar_t* valueName = nullptr;
    switch (sysElement) {
        case COLOR_MENU: valueName = L"Menu"; break;
        case COLOR_MENUTEXT: valueName = L"MenuText"; break;
        case COLOR_HIGHLIGHT: valueName = L"Hilight"; break;
        case COLOR_HIGHLIGHTTEXT: valueName = L"HilightText"; break;
        case COLOR_BTNSHADOW: valueName = L"ButtonShadow"; break;
        case COLOR_GRAYTEXT: valueName = L"GrayText"; break;
        case COLOR_BTNHIGHLIGHT: valueName = L"ButtonHilight"; break;
        case COLOR_3DDKSHADOW: valueName = L"ButtonDkShadow"; break;
        case COLOR_3DLIGHT: valueName = L"ButtonLight"; break;
        case COLOR_MENUBAR: valueName = L"MenuBar"; break;
    }

    if (valueName) {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Colors", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t buffer[64] = {0};
            DWORD bufferSize = sizeof(buffer) - sizeof(WCHAR);
            DWORD type = 0;

            if (RegQueryValueExW(hKey, valueName, nullptr, &type, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
                if (type == REG_SZ) {
                    buffer[bufferSize / sizeof(WCHAR)] = L'\0';
                    int r, g, b;
                    if (swscanf_s(buffer, L"%d %d %d", &r, &g, &b) == 3 &&
                        r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
                        RegCloseKey(hKey);
                        return RGB(r, g, b);
                    }
                }
            }
            RegCloseKey(hKey);
        }
    }
    Wh_Log(L"Failed to read %s from registry. Falling back to live color.", valueName ? valueName : L"unknown");
    return liveColorFallback;
}

void PublishBrush(HBRUSH* pSlot, COLORREF color) {
    HBRUSH hNew = CreateSolidBrush(color);
    HBRUSH hOld = (HBRUSH)InterlockedExchangePointer((PVOID*)pSlot, hNew);
    if (hOld) {
        std::lock_guard<std::mutex> lock(g_brushMutex);
        g_retiredBrushes.push_back(hOld);
    }
}

void LoadSettings() {
    WindhawkUtils::StringSetting mode = WindhawkUtils::StringSetting::make(L"ThemingMode");
    g_processLocal = wcscmp(mode.get(), L"systemColors") != 0;

    g_colMenu      = ParseHexColor(WindhawkUtils::StringSetting::make(L"MenuBgColor").get(), RGB(40, 40, 40));
    g_colText      = ParseHexColor(WindhawkUtils::StringSetting::make(L"MenuTextColor").get(), RGB(220, 220, 220));
    g_colHighlight = ParseHexColor(WindhawkUtils::StringSetting::make(L"HighlightBgColor").get(), RGB(80, 80, 80));
    g_colHiText    = ParseHexColor(WindhawkUtils::StringSetting::make(L"HighlightTextColor").get(), RGB(255, 255, 255));
    g_colGray      = ParseHexColor(WindhawkUtils::StringSetting::make(L"GrayTextColor").get(), RGB(128, 128, 128));
    COLORREF colSep = ParseHexColor(WindhawkUtils::StringSetting::make(L"SeparatorColor").get(), RGB(56, 56, 56));

    PublishBrush(&g_hSeparatorBrush, colSep);
    PublishBrush(&g_hMenuBrush, g_colMenu);
    PublishBrush(&g_hTextBrush, g_colText);
    PublishBrush(&g_hHighlightBrush, g_colHighlight);
    PublishBrush(&g_hHiTextBrush, g_colHiText);
    PublishBrush(&g_hGrayBrush, g_colGray);
}

// ----- Global system colors engine (legacy fallback) -----

void SaveOriginalColors() {
    if (g_hasSavedOrigColors) return;
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        g_origColors[i] = GetColorFromRegistry(g_sysElements[i], GetSysColor(g_sysElements[i]));
    }
    g_hasSavedOrigColors = true;
}

void ApplyDarkSystemColors() {
    SaveOriginalColors();

    COLORREF darkColors[NUM_ELEMENTS] = {
        g_colMenu, g_colText, g_colHighlight, g_colHiText,
        g_colMenu, g_colGray, g_colMenu, g_colMenu, g_colMenu, g_colMenu
    };

    SetSysColors(NUM_ELEMENTS, g_sysElements, darkColors);
}

bool AnotherPhotoshopInstanceRunning() {
    wchar_t path[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) return false;
    const wchar_t* exe = wcsrchr(path, L'\\');
    exe = exe ? exe + 1 : path;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return false;

    bool found = false;
    DWORD myPid = GetCurrentProcessId();
    PROCESSENTRY32W pe = { sizeof(pe) };
    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (pe.th32ProcessID != myPid && _wcsicmp(pe.szExeFile, exe) == 0) {
                found = true;
                break;
            }
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);
    return found;
}

// force=false defers restoration to the last running Photoshop instance.
void RestoreOriginalColors(bool force) {
    if (!g_hasSavedOrigColors) return;
    if (!force && AnotherPhotoshopInstanceRunning()) {
        Wh_Log(L"Another Photoshop instance is running; deferring color restoration to it.");
        return;
    }
    SetSysColors(NUM_ELEMENTS, g_sysElements, g_origColors);
}

// ----- Process-local engine -----

void EnableDarkAppMode(bool enable) {
    if (!g_hUxtheme) {
        g_hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!g_hUxtheme) {
        Wh_Log(L"Failed to load uxtheme.dll");
        return;
    }

    auto pSetPreferredAppMode = (SetPreferredAppMode_t)(void*)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(135));
    auto pFlushMenuThemes = (FlushMenuThemes_t)(void*)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(136));
    if (!pSetPreferredAppMode || !pFlushMenuThemes) {
        Wh_Log(L"uxtheme dark mode ordinals not available on this Windows build.");
        return;
    }

    pSetPreferredAppMode(enable ? PreferredAppMode::ForceDark : PreferredAppMode::Default);
    pFlushMenuThemes();
}

void ApplyMenuBackground(HMENU hMenu, HBRUSH hBrush) {
    MENUINFO mi = { sizeof(mi) };
    mi.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
    mi.hbrBack = hBrush;
    SetMenuInfo(hMenu, &mi);
}

BOOL CALLBACK StampMenuBackgroundsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        HMENU hMenu = GetMenu(hWnd);
        if (hMenu) ApplyMenuBackground(hMenu, (HBRUSH)lParam);
    }
    return TRUE;
}

// Stamps (or clears, with nullptr) the background brush on the menu bars of
// all top-level windows of this process, including their submenus.
void StampMenuBackgrounds(HBRUSH hBrush) {
    EnumWindows(StampMenuBackgroundsProc, (LPARAM)hBrush);
}

bool IsMenuContext(HDC hdc) {
    HWND hWnd = WindowFromDC(hdc);
    if (hWnd) {
        WCHAR cls[16];
        if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"#32768") == 0) return true;
        return false;
    }
    if (GetObjectType(hdc) == OBJ_MEMDC) {
        GUITHREADINFO gti = { sizeof(GUITHREADINFO) };
        if (GetGUIThreadInfo(GetCurrentThreadId(), &gti)) {
            if (gti.flags & GUI_INMENUMODE) return true;
        }
    }
    return false;
}

// Hooks

decltype(&GetSysColor) GetSysColor_Original;
DWORD WINAPI GetSysColor_Hook(int nIndex) {
    if (g_processLocal) {
        switch (nIndex) {
            case COLOR_MENU:
            case COLOR_MENUBAR:       return g_colMenu;
            case COLOR_MENUTEXT:      return g_colText;
            case COLOR_HIGHLIGHT:     return g_colHighlight;
            case COLOR_HIGHLIGHTTEXT: return g_colHiText;
            case COLOR_GRAYTEXT:      return g_colGray;
        }
    }
    return GetSysColor_Original(nIndex);
}

decltype(&GetSysColorBrush) GetSysColorBrush_Original;
HBRUSH WINAPI GetSysColorBrush_Hook(int nIndex) {
    if (g_processLocal) {
        HBRUSH hBrush = nullptr;
        switch (nIndex) {
            case COLOR_MENU:
            case COLOR_MENUBAR:       hBrush = g_hMenuBrush; break;
            case COLOR_MENUTEXT:      hBrush = g_hTextBrush; break;
            case COLOR_HIGHLIGHT:     hBrush = g_hHighlightBrush; break;
            case COLOR_HIGHLIGHTTEXT: hBrush = g_hHiTextBrush; break;
            case COLOR_GRAYTEXT:      hBrush = g_hGrayBrush; break;
        }
        if (hBrush) return hBrush;
    }
    return GetSysColorBrush_Original(nIndex);
}

decltype(&TrackPopupMenu) TrackPopupMenu_Original;
BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT* prcRect) {
    if (g_processLocal && g_hMenuBrush) ApplyMenuBackground(hMenu, g_hMenuBrush);
    return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}

decltype(&TrackPopupMenuEx) TrackPopupMenuEx_Original;
BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) {
    if (g_processLocal && g_hMenuBrush) ApplyMenuBackground(hMenu, g_hMenuBrush);
    return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
}

decltype(&PatBlt) PatBlt_Original;
BOOL WINAPI PatBlt_Hook(HDC hdc, int x, int y, int w, int h, DWORD rop) {
    HBRUSH hBrush = g_hSeparatorBrush;
    if (rop == PATCOPY && (h == 1 || h == 2) && w > 20 && hBrush) {
        if (IsMenuContext(hdc)) {
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
            BOOL bRes = PatBlt_Original(hdc, x, y, w, h, rop);
            SelectObject(hdc, hOldBrush);
            return bRes;
        }
    }
    return PatBlt_Original(hdc, x, y, w, h, rop);
}

decltype(&FillRect) FillRect_Original;
int WINAPI FillRect_Hook(HDC hdc, const RECT *lprc, HBRUSH hbr) {
    HBRUSH hBrush = g_hSeparatorBrush;
    if (lprc && hBrush) {
        int h = lprc->bottom - lprc->top;
        int w = lprc->right - lprc->left;
        if ((h == 1 || h == 2) && w > 20) {
            if (IsMenuContext(hdc)) {
                return FillRect_Original(hdc, lprc, hBrush);
            }
        }
    }
    return FillRect_Original(hdc, lprc, hbr);
}

decltype(&ExitProcess) ExitProcess_Original;
__declspec(noreturn) void WINAPI ExitProcess_Hook(UINT uExitCode) {
    if (g_globalActive) RestoreOriginalColors(false);
    ExitProcess_Original(uExitCode);
}

// Windhawk Events

void Wh_ModSettingsChanged() {
    bool wasGlobal = g_globalActive;
    bool wasLocal = g_localActive;

    LoadSettings();

    if (g_processLocal) {
        if (wasGlobal) {
            RestoreOriginalColors(true);
            g_globalActive = false;
        }
        EnableDarkAppMode(true);
        StampMenuBackgrounds(g_hMenuBrush);
        g_localActive = true;
    } else {
        if (wasLocal) {
            StampMenuBackgrounds(nullptr);
            EnableDarkAppMode(false);
            g_localActive = false;
        }
        ApplyDarkSystemColors();
        g_globalActive = true;
    }
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (g_processLocal) {
        EnableDarkAppMode(true);
        StampMenuBackgrounds(g_hMenuBrush);
        g_localActive = true;
    } else {
        ApplyDarkSystemColors();
        g_globalActive = true;
    }

    if (!WindhawkUtils::SetFunctionHook(GetSysColor, GetSysColor_Hook, &GetSysColor_Original)) Wh_Log(L"Failed to hook GetSysColor");
    if (!WindhawkUtils::SetFunctionHook(GetSysColorBrush, GetSysColorBrush_Hook, &GetSysColorBrush_Original)) Wh_Log(L"Failed to hook GetSysColorBrush");
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenu_Hook, &TrackPopupMenu_Original)) Wh_Log(L"Failed to hook TrackPopupMenu");
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook, &TrackPopupMenuEx_Original)) Wh_Log(L"Failed to hook TrackPopupMenuEx");
    if (!WindhawkUtils::SetFunctionHook(PatBlt, PatBlt_Hook, &PatBlt_Original)) Wh_Log(L"Failed to hook PatBlt");
    if (!WindhawkUtils::SetFunctionHook(FillRect, FillRect_Hook, &FillRect_Original)) Wh_Log(L"Failed to hook FillRect");
    if (!WindhawkUtils::SetFunctionHook(ExitProcess, ExitProcess_Hook, &ExitProcess_Original)) Wh_Log(L"Failed to hook ExitProcess");

    return TRUE;
}

void Wh_ModUninit() {
    if (g_globalActive) {
        // The mod is being unloaded everywhere, so restore unconditionally;
        // concurrent instances all write the same registry-derived originals.
        RestoreOriginalColors(true);
        g_globalActive = false;
    }
    if (g_localActive) {
        StampMenuBackgrounds(nullptr);
        EnableDarkAppMode(false);
        g_localActive = false;
    }

    HBRUSH* slots[] = {
        &g_hSeparatorBrush, &g_hMenuBrush, &g_hTextBrush,
        &g_hHighlightBrush, &g_hHiTextBrush, &g_hGrayBrush
    };
    for (HBRUSH* pSlot : slots) {
        HBRUSH hBrush = (HBRUSH)InterlockedExchangePointer((PVOID*)pSlot, nullptr);
        if (hBrush) DeleteObject(hBrush);
    }

    std::lock_guard<std::mutex> lock(g_brushMutex);
    for (HBRUSH b : g_retiredBrushes) DeleteObject(b);
    g_retiredBrushes.clear();

    if (g_hUxtheme) {
        FreeLibrary(g_hUxtheme);
        g_hUxtheme = nullptr;
    }
}
