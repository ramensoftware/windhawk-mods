// ==WindhawkMod==
// @id              24-bit-icons
// @name            24-bit Icons
// @description     Forces 24-bit icons like Windows 2000 and before
// @version         1.0
// @author          xalejandro
// @github          https://github.com/tetawaves
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 24-bit Icons
Forces 24-bit icons like Windows 2000 and before.  
![Before / After](https://i.imgur.com/Gl5ykd6.png)
## Notes
* Icons that are already 24-bit or lower are never touched.
* Icons stored as PNG are left alone.
* Windows caches icons, so already running programs keep their old icons until they are restarted.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <shlobj.h>

#ifdef _WIN64
#   define STDCALL  __cdecl
#   define SSTDCALL L"__cdecl"
#else
#   define STDCALL  __stdcall
#   define SSTDCALL L"__stdcall"
#endif

#define LR_CREATEREALDIB 0x0800 

typedef struct tagICONDIR
{
    BYTE  Width;
    BYTE  Height;
    BYTE  ColorCount;
    BYTE  reserved;
} ICONDIR;

typedef struct tagRESDIR
{
    ICONDIR Icon;
    WORD    Planes;
    WORD    BitCount;
    DWORD   BytesInRes;
    WORD    idIcon;
} RESDIR, *LPRESDIR;

thread_local bool g_fIcon = false;

bool IsExplorerProcess() {
    WCHAR path[MAX_PATH];
    if (!GetWindowsDirectory(path, ARRAYSIZE(path))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    wcscat_s(path, MAX_PATH, L"\\explorer.exe");

    return GetModuleHandle(path) == GetModuleHandle(nullptr);
}

HWND FindCurrentProcessTaskbarWnd() 
{
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

void InvalidateIconCache() 
{
    if (IsExplorerProcess() && FindCurrentProcessTaskbarWnd())
    {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }
}

UINT (__fastcall *GetBestImage_orig)(LPRESDIR, UINT, int, int, UINT, BOOL);
UINT __fastcall GetBestImage_hook(LPRESDIR lprd, UINT uCount, int cxDesired, int cyDesired, UINT bppDesired, BOOL fIcon)
{
    if (fIcon)
    {
        if (lprd->Icon.ColorCount == 16)
            // bppDesired equal to 8 will get the 24-bit icon
            bppDesired = 8;
    }
    return GetBestImage_orig(lprd, uCount, cxDesired, cyDesired, bppDesired, fIcon);
}

HICON (__fastcall *ConvertDIBIcon_orig)(
    LPBITMAPINFOHEADER lpbih,
    unsigned int       a2,
    HINSTANCE          hmod,
    LPCWSTR            lpName,
    BOOL               fIcon,
    DWORD              cxNew,
    DWORD              cyNew,
    UINT               LR_flags);
HICON __fastcall ConvertDIBIcon_hook(
    LPBITMAPINFOHEADER lpbih,
    unsigned int       a2,
    HINSTANCE          hmod,
    LPCWSTR            lpName,
    BOOL               fIcon,
    DWORD              cxNew,
    DWORD              cyNew,
    UINT               LR_flags)
{
    g_fIcon = fIcon;
    HICON hIcon = ConvertDIBIcon_orig(lpbih, a2, hmod, lpName, fIcon, cxNew, cyNew, LR_flags);
    g_fIcon = false;
    return hIcon;
}

HBITMAP (__fastcall *BitmapFromDIB_orig)(int, int, WORD, WORD, UINT, int, int, LPSTR, DWORD, LPBITMAPINFO, HPALETTE);
HBITMAP __fastcall BitmapFromDIB_hook(
    int          cxNew,
    int          cyNew,
    WORD         bPlanesNew,
    WORD         bBitsPixelNew,
    UINT         LR_flags,
    int          cxOld,
    int          cyOld,
    LPSTR        lpBits,
    DWORD        cbBits,
    LPBITMAPINFO lpbi,
    HPALETTE     hpal)
{
    if (g_fIcon && bBitsPixelNew == 32)
    {
        bBitsPixelNew = 24;
        LR_flags &= ~(LR_CREATEREALDIB);
    }
    return BitmapFromDIB_orig(cxNew, cyNew, bPlanesNew, bBitsPixelNew, LR_flags, cxOld, cyOld, lpBits, cbBits, lpbi, hpal);
}

#ifdef _WIN64
HBITMAP (__fastcall *BitmapFromDIB_11_orig)(int, int, WORD, WORD, UINT, int, int, LPSTR, DWORD, LPBITMAPINFO, HPALETTE);
HBITMAP __fastcall BitmapFromDIB_11_hook(
    int          cxNew,
    int          cyNew,
    WORD         bPlanesNew,
    WORD         bBitsPixelNew,
    UINT         LR_flags,
    int          cxOld,
    int          cyOld,
    LPSTR        lpBits,
    DWORD        cbBits,
    LPBITMAPINFO lpbi,
    HPALETTE     hpal)
#else
HBITMAP (__fastcall *BitmapFromDIB_11_orig)(int, int, WORD, WORD, UINT, int, int, LPSTR, DWORD, LPBITMAPINFO, LPBITMAPINFO, HPALETTE);
HBITMAP __fastcall BitmapFromDIB_11_hook(
    int          cxNew,
    int          cyNew,
    WORD         bPlanesNew,
    WORD         bBitsPixelNew,
    UINT         LR_flags,
    int          cxOld,
    int          cyOld,
    LPSTR        lpBits,
    DWORD        cbBits,
    LPBITMAPINFO lpbi,
    LPBITMAPINFO unused,
    HPALETTE     hpal)
#endif
{
    if (g_fIcon && bBitsPixelNew == 32)
    {
        bBitsPixelNew = 24;
        LR_flags &= ~(LR_CREATEREALDIB);
    }
#ifdef _WIN64
    return BitmapFromDIB_11_orig(cxNew, cyNew, bPlanesNew, bBitsPixelNew, LR_flags, cxOld, cyOld, lpBits, cbBits, lpbi, hpal);
#else
    return BitmapFromDIB_11_orig(cxNew, cyNew, bPlanesNew, bBitsPixelNew, LR_flags, cxOld, cyOld, lpBits, cbBits, lpbi, unused, hpal);
#endif
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    HMODULE hUser32 = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hUser32)
    {
        Wh_Log(L"Failed to load user32.dll");
        return FALSE;
    }

    const WindhawkUtils::SYMBOL_HOOK user32DllHooks[] =
    {
        {
            {
                L"unsigned int " SSTDCALL " GetBestImage(struct tagRESDIR *,unsigned int,int,int,unsigned int,int)",
            },
            &GetBestImage_orig,
            GetBestImage_hook,
            false
        },
        {
            {
                L"struct HICON__ * " SSTDCALL " ConvertDIBIcon(struct tagBITMAPINFOHEADER *,unsigned long,struct HINSTANCE__ *,unsigned short const *,int,unsigned long,unsigned long,unsigned int)",
            },
            &ConvertDIBIcon_orig,
            ConvertDIBIcon_hook,
            false
        }
    };

    const WindhawkUtils::SYMBOL_HOOK user32_10_DllHook
    {
        {
            L"struct HBITMAP__ * " SSTDCALL " BitmapFromDIB(int,int,unsigned short,unsigned short,unsigned int,int,int,char *,unsigned long,struct tagBITMAPINFO *,struct HPALETTE__ *)"
        },
        &BitmapFromDIB_orig,
        BitmapFromDIB_hook,
        false
    };

    const WindhawkUtils::SYMBOL_HOOK user32_11_DllHook
    {
        {
            L"struct HBITMAP__ * " SSTDCALL " BitmapFromDIB(int,int,unsigned short,unsigned short,unsigned int,int,int,char *,unsigned long,struct tagBITMAPINFO *,struct HPALETTE__ *)",
            L"struct HBITMAP__ * " SSTDCALL " BitmapFromDIB(int,int,unsigned short,unsigned short,unsigned int,int,int,char *,unsigned long,struct tagBITMAPINFO *,struct tagBITMAPINFO *,struct HPALETTE__ *)"
        },
        &BitmapFromDIB_11_orig,
        BitmapFromDIB_11_hook,
        false
    };

    if (!WindhawkUtils::HookSymbols(hUser32, user32DllHooks, ARRAYSIZE(user32DllHooks)))
    {
        Wh_Log(L"Failed to hook user32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(hUser32, &user32_10_DllHook, 1))
    {
        if (!WindhawkUtils::HookSymbols(hUser32, &user32_11_DllHook, 1))
        {
            Wh_Log(L"Failed to hook function BitmapFromDIB");
            return FALSE;
        }
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    InvalidateIconCache();
}

void Wh_ModUninit()
{
    InvalidateIconCache();
}
