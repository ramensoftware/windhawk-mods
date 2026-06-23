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
// @compilerOptions -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 File Open/Save Toggle Bar Icon

Restores the bitmap arrow glyph on the "Toggle Folders" button in open/save dialogs.

This glyph was changed to an icon in later versions of Windows. The icon scaling differs, so it wasn't possible to just
make the icon look like the Windows 7 glyph.

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

UINT (WINAPI *pfnGetDpiForWindow)(HWND hwnd) = nullptr;

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

thread_local void *g_pCurrentToggleBar = nullptr;
thread_local HWND g_hwndCurrentToggleBar = nullptr;

// Worth keeping in mind:
// "Capture relies on the toggle bar being created via CreateWindowExW. If a
// future build (or a differently-behaving app, once the scope broadens) creates
// the toolbar through a path your hook doesn't see, the handle stays null and
// ScaleAndSet… quietly logs 'hwndToggleBar is null' and does nothing — graceful,
// but the feature silently won't apply. The analogous mod hooks CreateToolbarEx
// for the same purpose; worth keeping in mind if you broaden @include."
// https://github.com/ramensoftware/windhawk-mods/pull/4447#issuecomment-4765563181
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_orig;
HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle,
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
                     LPVOID lpParam)
{
    HWND hwnd = CreateWindowExW_orig(dwExStyle, lpClassName, lpWindowName, 
        dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    if (hwnd && g_pCurrentToggleBar && !IS_INTRESOURCE(lpClassName) &&
        0 == _wcsicmp(lpClassName, L"ToolbarWindow32"))
        g_hwndCurrentToggleBar = hwnd;

    return hwnd;
}

void (__thiscall *CFileOpenSave__ScaleAndSetToggleBarImageListIfNeeded_orig)(class CFileOpenSave *pThis);
void (__thiscall *CFileOpenSave___SetToggleBar_orig)(class CFileOpenSave *pThis);

class CFileOpenSave
{
public:
    // Entirely replaces original implementation.
    void ScaleAndSetToggleBarImageListIfNeeded()
    {
        HWND hwndToggleBar = g_hwndCurrentToggleBar;

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

void __thiscall CFileOpenSave___SetToggleBar_hook(class CFileOpenSave *pThis)
{
    g_pCurrentToggleBar = pThis;
    CFileOpenSave___SetToggleBar_orig(pThis);
    g_pCurrentToggleBar = nullptr;
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
    {
        {
#ifdef _WIN64
            L"protected: void __cdecl CFileOpenSave::_SetToggleBar(void)",
#else
            L"protected: void __thiscall CFileOpenSave::_SetToggleBar(void)",
#endif
        },
        &CFileOpenSave___SetToggleBar_orig,
        CFileOpenSave___SetToggleBar_hook,
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

    pfnGetDpiForWindow = (decltype(pfnGetDpiForWindow))GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");

    if (!Wh_SetFunctionHook(
        (void *)CreateWindowExW,
        (void *)CreateWindowExW_hook,
        (void **)&CreateWindowExW_orig
    ))
    {
        Wh_Log(L"Failed to hook CreateWindowExW.");
        return FALSE;
    }

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
        return FALSE;
    }

    g_hbmArrowDown = LoadBitmapW(g_hmodComdlg32_7, (LPCWSTR)0x241);
    Wh_Log(L"hbmArrow down: %p", g_hbmArrowDown);
    g_hbmArrowUp = LoadBitmapW(g_hmodComdlg32_7, (LPCWSTR)0x242);
    Wh_Log(L"hbmArrow up: %p", g_hbmArrowUp);

    if (!g_hbmArrowDown || !g_hbmArrowUp)
    {
        Wh_Log(
            L"Failed to load one or more bitmaps from the module \"%s\". "
            L"Please ensure that bitmap resources 577 (0x241) and 578 (0x242) "
            L"exist in the target binary. An unmodified binary from Windows 7 "
            L"will feature these resources.",
            g_spszComdlg32Path.c_str()
        );
        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    if (g_hbmArrowDown)
        DeleteObject(g_hbmArrowDown);

    if (g_hbmArrowUp)
        DeleteObject(g_hbmArrowUp);

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
