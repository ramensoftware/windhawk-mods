// ==WindhawkMod==
// @id              win7-file-open-save-toggle-bar-icon
// @name            Windows 7 File Open/Save Toggle Bar Icon
// @description     Restores the bitmap arrow glyph on the "Toggle Folders" button in open/save dialogs.
// @version         1.0
// @author          Leymonaide
// @github          https://github.com/Leymonaide
// @twitter         https://twitter.com/Leym0naide
// @homepage        https://leymonaide.github.io/
// @include         *
// @compilerOptions -lcomdlg32 -lgdi32 -lntdll
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 File Open/Save Toggle Bar Icon

Restores the bitmap arrow glyph on the "Toggle Folders" button in open/save dialogs.

This glyph was changed to an icon in later versions of Windows. The icon scaling differs, so it wasn't possible to just
make the icon look like the Windows 7 glyph.

This mod is currently tested and known to work on Windows 10 builds 19041 through 19045. It is not tested on other
versions. Since the mod injects into all processes to function, the mod will silently refuse to load on unsupported
versions of Windows.

![Preview image](https://raw.githubusercontent.com/Leymonaide/images/refs/heads/main/win7-file-open-save-toggle-bar-icon.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- win7_comdlg32_path: C:\path\to\win7\comdlg32.dll
  $name: Path to Windows 7 comdlg32.dll
  $description: The bitmap will be loaded from this DLL.

- use_per_window_dpi: true
  $name: Use per-window DPI
  $description: >
    Windows 7 only supported per-process DPI. If you want perfect accuracy to
    Windows 7, then disable this option.
*/
// ==/WindhawkModSettings==

#include <string>
#include <windhawk_utils.h>

EXTERN_C NTSYSAPI NTSTATUS NTAPI RtlGetVersion(PRTL_OSVERSIONINFOW lpVersionInformation);

HMODULE g_hmodComdlg32_7 = nullptr;
std::wstring g_spszComdlg32Path;
bool g_usePerWindowDpi = false;

bool g_fHighDPIAware = false;
bool g_fHighDPI = false;
int g_iLPX = -1;
int g_iLPY = -1;

HBITMAP g_hbmArrowDown = nullptr;
HBITMAP g_hbmArrowUp = nullptr;

UINT (*pfnGetDpiForWindow)(HWND hwnd) = nullptr;

void InitDPI()
{
    bool isProcessDpiAware = IsProcessDPIAware();
    if (g_iLPX == -1 || g_fHighDPIAware != isProcessDpiAware)
    {
        g_fHighDPIAware = isProcessDpiAware;

        // Get the pixel density of the display:
        HDC hdc = GetDC(NULL);
        if (hdc)
        {
            g_iLPX = GetDeviceCaps(hdc, LOGPIXELSX);
            g_iLPY = GetDeviceCaps(hdc, LOGPIXELSY);
            g_fHighDPI = g_iLPX != 96;
            ReleaseDC(NULL, hdc);
        }
    }
}

void SHLogicalToPhysicalDPI(int *px, int *py)
{
    InitDPI();
    if (px)
        *px = MulDiv(*px, g_iLPX, 96);
    if (py)
        *py = MulDiv(*py, g_iLPY, 96);
}

void (__thiscall *CFileOpenSave__ScaleAndSetToggleBarImageListIfNeeded_orig)(class CFileOpenSave *pThis);

class CFileOpenSave
{
public:
    // The current offsets are applicable for 19041.1806, and reportedly works
    // on other 19041 variants. Support for other builds will be added later.
    HWND get_hwndToggleBar()
    {
#ifdef _WIN64
        return *(HWND *)((size_t)this + (70 * 8));
#else
        return *(HWND *)((size_t)this + (79 * 4));
#endif
    }

    // Entirely replaces original implementation.
    void ScaleAndSetToggleBarImageListIfNeeded()
    {
        HWND hwndToggleBar = get_hwndToggleBar();

        if (!hwndToggleBar)
        {
            Wh_Log(L"hwndToggleBar is null");
            return;
        }

        int buttonHeight = 21;
        if (g_usePerWindowDpi && pfnGetDpiForWindow)
        {
            UINT uDpi = pfnGetDpiForWindow(hwndToggleBar);
            buttonHeight = MulDiv(buttonHeight, uDpi, 96);
        }
        else
        {
            SHLogicalToPhysicalDPI(&buttonHeight, nullptr);
        }

        // Windows 7 does not have variants of the bitmap for larger DPI scales,
        // so it will appear at a low scale on such devices.
        SendMessageW(hwndToggleBar, TB_SETBUTTONSIZE, 0, MAKELPARAM(150, buttonHeight));
        SendMessageW(hwndToggleBar, TB_SETPADDING, 0, MAKELPARAM(0, buttonHeight - 21));
        SendMessageW(hwndToggleBar, TB_SETBITMAPSIZE, 0, MAKELPARAM(18, 21));
        {   
            TBADDBITMAP tbab {
                .hInst = nullptr,
                .nID = (UINT_PTR)g_hbmArrowDown,
            };
            SendMessageW(hwndToggleBar, TB_ADDBITMAP, 1, (LPARAM)&tbab);
        }
        {
            TBADDBITMAP tbab {
                .hInst = nullptr,
                .nID = (UINT_PTR)g_hbmArrowUp,
            };
            SendMessageW(hwndToggleBar, TB_ADDBITMAP, 1, (LPARAM)&tbab);
        }
    }
};

void __thiscall CFileOpenSave__ScaleAndSetToggleBarImageListIfNeeded_hook(class CFileOpenSave *pThis)
{
    return pThis->ScaleAndSetToggleBarImageListIfNeeded();
}

// comdlg32.dll
const WindhawkUtils::SYMBOL_HOOK c_rghkComdlg32[] = {
    {
        {
#ifdef _WIN64
            L"protected: void __cdecl CFileOpenSave::ScaleAndSetToggleBarImageListIfNeeded(void)",
#else
            L"protected: void __thiscall CFileOpenSave::ScaleAndSetToggleBarImageListIfNeeded(void)",
#endif
        },
        &CFileOpenSave__ScaleAndSetToggleBarImageListIfNeeded_orig,
        CFileOpenSave__ScaleAndSetToggleBarImageListIfNeeded_hook,
    },
};

void LoadSettings()
{
    g_spszComdlg32Path = WindhawkUtils::StringSetting::make(L"win7_comdlg32_path");
    g_usePerWindowDpi = Wh_GetIntSetting(L"use_per_window_dpi");
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    RTL_OSVERSIONINFOW osvi;
    RtlGetVersion(&osvi);


    if (osvi.dwBuildNumber < 19041 || osvi.dwBuildNumber > 19045)
    {
        return FALSE;
    }

    HMODULE hUser32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    pfnGetDpiForWindow = (decltype(pfnGetDpiForWindow))GetProcAddress(hUser32, "GetDpiForWindow");
    FreeLibrary(hUser32);

    g_hmodComdlg32_7 = LoadLibraryExW(g_spszComdlg32Path.c_str(), nullptr,
        LOAD_LIBRARY_AS_DATAFILE);

    if (!g_hmodComdlg32_7)
    {
        Wh_Log(L"Path to 7 comdlg32 not specified or the module is invalid.");
        return FALSE;
    }

    HMODULE hmodComdlg32_10 = LoadLibraryExW(L"comdlg32.dll", nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!WindhawkUtils::HookSymbols(hmodComdlg32_10, c_rghkComdlg32, ARRAYSIZE(c_rghkComdlg32)))
    {
        Wh_Log(L"Failed to hook symbols in comdlg32.dll.");
    }

    g_hbmArrowDown = LoadBitmapW(g_hmodComdlg32_7, (LPCWSTR)0x241);
    Wh_Log(L"hbmArrow down: %p", g_hbmArrowDown);
    g_hbmArrowUp = LoadBitmapW(g_hmodComdlg32_7, (LPCWSTR)0x242);
    Wh_Log(L"hbmArrow up: %p", g_hbmArrowUp);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    if (g_hmodComdlg32_7)
        FreeLibrary(g_hmodComdlg32_7);
}

// The mod setting were changed, reload them.
BOOL Wh_ModSettingsChanged(BOOL *pbReload)
{
    Wh_Log(L"SettingsChanged");
    *pbReload = TRUE;
    return TRUE;
}
