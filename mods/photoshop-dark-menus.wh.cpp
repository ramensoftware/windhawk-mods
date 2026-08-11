// ==WindhawkMod==
// @id            photoshop-dark-menus
// @name          Photoshop Dark Menus
// @description   Enables dark mode and custom separator colors for all menus in Adobe Photoshop.
// @version       1.1.0
// @author        Saber Naeemi
// @github        https://github.com/sabergraphics
// @twitter       https://x.com/SaberNaeemi
// @homepage      https://www.sabernaeemi.com
// @include       Photoshop.exe
// @compilerOptions -lUser32 -lGdi32 -lAdvapi32
// @license       MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Photoshop Dark Menus

Forces dark mode for all context and top menu bar dropdowns in Adobe Photoshop without permanently altering registry keys on disk.

### Features
- Dark backgrounds and customizable text colors across all menus.
- Independent separator line color control (set to match menu background to hide separators).
- Legible disabled item text styling.
- Automatic restoration of default Windows system colors upon exiting Photoshop.

### Screenshots
![Photoshop Dark Menu Dropdown](https://raw.githubusercontent.com/sabergraphics/photoshop-dark-menus/main/images/photoshop-dark-menu-screenshot-1.png)

![Photoshop Dark Context Menu](https://raw.githubusercontent.com/sabergraphics/photoshop-dark-menus/main/images/photoshop-dark-menu-screenshot-2.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
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
  $description: "Set this to the menu background color to hide separators."
- GrayTextColor: "#808080"
  $name: "Disabled Text Color"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <wchar.h>

constexpr int NUM_ELEMENTS = 10;

const INT g_sysElements[NUM_ELEMENTS] = {
    COLOR_MENU,
    COLOR_MENUTEXT,
    COLOR_HIGHLIGHT,
    COLOR_HIGHLIGHTTEXT,
    COLOR_BTNSHADOW,
    COLOR_GRAYTEXT,
    COLOR_BTNHIGHLIGHT,
    COLOR_3DDKSHADOW,
    COLOR_3DLIGHT,
    COLOR_MENUBAR
};

COLORREF g_origColors[NUM_ELEMENTS] = {};
bool g_hasSavedOrigColors = false;

HBRUSH g_hSeparatorBrush = nullptr;

COLORREF ParseHexColor(const PCWSTR hexStr, COLORREF defaultColor) {
    if (!hexStr || wcslen(hexStr) < 7 || hexStr[0] != L'#')
        return defaultColor;

    unsigned int r = 0, g = 0, b = 0;
    if (swscanf_s(hexStr + 1, L"%02x%02x%02x", &r, &g, &b) == 3) {
        return RGB(r, g, b);
    }
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
            DWORD bufferSize = sizeof(buffer);
            if (RegQueryValueExW(hKey, valueName, nullptr, nullptr, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
                int r, g, b;
                if (swscanf_s(buffer, L"%d %d %d", &r, &g, &b) == 3) {
                    RegCloseKey(hKey);
                    return RGB(r, g, b);
                }
            }
            RegCloseKey(hKey);
        }
    }
    return liveColorFallback;
}

void SaveOriginalColors() {
    if (g_hasSavedOrigColors) return;
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        g_origColors[i] = GetColorFromRegistry(g_sysElements[i], GetSysColor(g_sysElements[i]));
    }
    g_hasSavedOrigColors = true;
}

void ApplyDarkSystemColors() {
    SaveOriginalColors();

    COLORREF colMenu      = ParseHexColor(WindhawkUtils::StringSetting::make(L"MenuBgColor").get(), RGB(40, 40, 40));
    COLORREF colText      = ParseHexColor(WindhawkUtils::StringSetting::make(L"MenuTextColor").get(), RGB(220, 220, 220));
    COLORREF colHighlight = ParseHexColor(WindhawkUtils::StringSetting::make(L"HighlightBgColor").get(), RGB(80, 80, 80));
    COLORREF colHiText    = ParseHexColor(WindhawkUtils::StringSetting::make(L"HighlightTextColor").get(), RGB(255, 255, 255));
    COLORREF colSep       = ParseHexColor(WindhawkUtils::StringSetting::make(L"SeparatorColor").get(), RGB(56, 56, 56));
    COLORREF colGray      = ParseHexColor(WindhawkUtils::StringSetting::make(L"GrayTextColor").get(), RGB(128, 128, 128));

    HBRUSH hNewSepBrush = CreateSolidBrush(colSep);
    HBRUSH hOldSepBrush = (HBRUSH)InterlockedExchangePointer((PVOID*)&g_hSeparatorBrush, hNewSepBrush);
    if (hOldSepBrush) DeleteObject(hOldSepBrush);

    COLORREF darkColors[NUM_ELEMENTS] = {
        colMenu,      // COLOR_MENU
        colText,      // COLOR_MENUTEXT
        colHighlight, // COLOR_HIGHLIGHT
        colHiText,    // COLOR_HIGHLIGHTTEXT
        colMenu,      // COLOR_BTNSHADOW
        colGray,      // COLOR_GRAYTEXT
        colMenu,      // COLOR_BTNHIGHLIGHT
        colMenu,      // COLOR_3DDKSHADOW
        colMenu,      // COLOR_3DLIGHT
        colMenu       // COLOR_MENUBAR
    };

    SetSysColors(NUM_ELEMENTS, g_sysElements, darkColors);
}

void RestoreOriginalColors() {
    if (g_hasSavedOrigColors) {
        SetSysColors(NUM_ELEMENTS, g_sysElements, g_origColors);
    }
    HBRUSH hOldBrush = (HBRUSH)InterlockedExchangePointer((PVOID*)&g_hSeparatorBrush, nullptr);
    if (hOldBrush) DeleteObject(hOldBrush);
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------

// Safely determines if the drawing is happening for a menu, 
// even if a memory DC (double-buffering) is being used.
bool IsMenuContext(HDC hdc) {
    HWND hWnd = WindowFromDC(hdc);
    if (hWnd) {
        // Direct drawing to a real window
        WCHAR cls[16];
        if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"#32768") == 0) {
            return true;
        }
        return false;
    } 
    
    // If hWnd is NULL, it is likely a Memory DC used for double-buffering.
    // Check if the current thread is actively displaying a menu.
    if (GetObjectType(hdc) == OBJ_MEMDC) {
        GUITHREADINFO gti = { sizeof(GUITHREADINFO) };
        if (GetGUIThreadInfo(GetCurrentThreadId(), &gti)) {
            if (gti.flags & GUI_INMENUMODE) {
                return true;
            }
        }
    }
    
    return false;
}

using PatBlt_t = BOOL (WINAPI*)(HDC hdc, int x, int y, int w, int h, DWORD rop);
PatBlt_t PatBlt_Original = nullptr;

BOOL WINAPI PatBlt_Hook(HDC hdc, int x, int y, int w, int h, DWORD rop) {
    HBRUSH hBrush = g_hSeparatorBrush; 
    if (rop == PATCOPY && (h == 1 || h == 2) && w > 20 && hBrush && IsMenuContext(hdc)) {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        BOOL bRes = PatBlt_Original(hdc, x, y, w, h, rop);
        SelectObject(hdc, hOldBrush);
        return bRes;
    }
    return PatBlt_Original(hdc, x, y, w, h, rop);
}

using FillRect_t = int (WINAPI*)(HDC hdc, const RECT *lprc, HBRUSH hbr);
FillRect_t FillRect_Original = nullptr;

int WINAPI FillRect_Hook(HDC hdc, const RECT *lprc, HBRUSH hbr) {
    HBRUSH hBrush = g_hSeparatorBrush; 
    if (lprc && hBrush && IsMenuContext(hdc)) {
        int h = lprc->bottom - lprc->top;
        int w = lprc->right - lprc->left;
        if ((h == 1 || h == 2) && w > 20) {
            return FillRect_Original(hdc, lprc, hBrush);
        }
    }
    return FillRect_Original(hdc, lprc, hbr);
}

using ExitProcess_t = void (WINAPI*)(UINT uExitCode);
ExitProcess_t ExitProcess_Original = nullptr;

void WINAPI ExitProcess_Hook(UINT uExitCode) {
    RestoreOriginalColors();
    ExitProcess_Original(uExitCode);
}

// -------------------------------------------------------------------------
// Windhawk Events
// -------------------------------------------------------------------------

void Wh_ModSettingsChanged() {
    ApplyDarkSystemColors();
}

BOOL Wh_ModInit() {
    ApplyDarkSystemColors();

    HMODULE hGdi32 = GetModuleHandleW(L"gdi32.dll");
    if (hGdi32) {
        void* pPatBlt = (void*)GetProcAddress(hGdi32, "PatBlt");
        if (pPatBlt) Wh_SetFunctionHook(pPatBlt, (void*)PatBlt_Hook, (void**)&PatBlt_Original);
    }

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        void* pFillRect = (void*)GetProcAddress(hUser32, "FillRect");
        if (pFillRect) Wh_SetFunctionHook(pFillRect, (void*)FillRect_Hook, (void**)&FillRect_Original);
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (hKernel32) {
        void* pExitProcess = (void*)GetProcAddress(hKernel32, "ExitProcess");
        if (pExitProcess) Wh_SetFunctionHook(pExitProcess, (void*)ExitProcess_Hook, (void**)&ExitProcess_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    RestoreOriginalColors();
}
