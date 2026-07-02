// ==WindhawkMod==
// @id              native-titlebars-uwp-lite
// @name            Remove UWP titlebars Lite
// @description     Enables native titlebars in UWP apps
// @version         1.2.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         ApplicationFrameHost.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lcomctl32 -ldwmapi -lshell32 -lole32 -lpropsys -lgdiplus -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

Replaces the UWP titlebars with native Win32 titlebars.

This mod is focused on the Classic theme, so may produce sub-optimal results in other cases.

![Screenshot](https://i.imgur.com/oeCYWJY.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <CommCtrl.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <propsys.h>
#include <propkey.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <gdiplus.h>
#include <vector>

static const GUID BHID_SFUIObject_local =
    {0x3981e225, 0xf559, 0x11d3, {0x8e, 0x3a, 0x00, 0xc0, 0x4f, 0x68, 0x37, 0xd5}};

static const GUID FOLDERID_AppsFolder_local =
    {0x1e87508d, 0x89c2, 0x42f0, {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};

static const GUID IID_IExtractIconW_local =
    {0x000214fa, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

static const PROPERTYKEY PKEY_AppUserModel_ID_local = 
    {{0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}}, 5};

typedef INT64 (*CTitleBar__CreateTitleBarWindow_t)(void *);
CTitleBar__CreateTitleBarWindow_t CTitleBar__CreateTitleBarWindow_orig;
INT64 __fastcall CTitleBar__CreateTitleBarWindow_hook(void *pThis)
{
    return 0;
}

typedef INT64 (*CTitleBar__PaintButton_t)(INT64, INT64, INT64, UINT, int *, DWORD);
CTitleBar__PaintButton_t CTitleBar__PaintButton_orig;
INT64 __fastcall CTitleBar__PaintButton_hook(
    INT64 a1,
    INT64 a2,
    INT64 a3,
    UINT a4,
    int *a5,
    DWORD a6)
{
    return 0;
}

struct FindAppFrameData
{
    DWORD processId;
    HWND hAppFrame;
};

BOOL CALLBACK FindAppFrameProc(HWND hWnd, LPARAM lParam)
{
    FindAppFrameData *pData = (FindAppFrameData *)lParam;
    
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != pData->processId)
        return TRUE;

    WCHAR wszClassName[64] = {0};
    GetClassNameW(hWnd, wszClassName, ARRAYSIZE(wszClassName));
    if (wcscmp(wszClassName, L"ApplicationFrameWindow") == 0)
    {
        pData->hAppFrame = hWnd;
        return FALSE;
    }

    return TRUE;
}

HWND FindApplicationFrameWindow()
{
    FindAppFrameData data = {0};
    data.processId = GetCurrentProcessId();
    EnumWindows(FindAppFrameProc, (LPARAM)&data);
    return data.hAppFrame;
}

// Tries to find the ApplicationFrameWindow that actually owns the given
// titlebar window, by walking the owner/parent chain, instead of blindly
// grabbing the first ApplicationFrameWindow found in the process (which
// breaks when the host process hosts more than one app window).
HWND FindOwningApplicationFrameWindow(HWND hWnd)
{
    HWND hCandidate = hWnd;

    for (int i = 0; i < 16 && hCandidate; i++)
    {
        WCHAR wszClassName[64] = {0};
        GetClassNameW(hCandidate, wszClassName, ARRAYSIZE(wszClassName));
        if (wcscmp(wszClassName, L"ApplicationFrameWindow") == 0)
        {
            return hCandidate;
        }

        HWND hNext = GetWindow(hCandidate, GW_OWNER);
        if (!hNext)
        {
            hNext = GetParent(hCandidate);
        }
        if (hNext == hCandidate)
        {
            break;
        }
        hCandidate = hNext;
    }

    // Fallback: old behavior (best-effort, may be inaccurate if the
    // process hosts multiple ApplicationFrameWindow instances).
    return FindApplicationFrameWindow();
}

static ULONG_PTR g_gdiplusToken = 0;

// Tracks every window we've subclassed so Wh_ModUninit can clean them
// all up before the mod DLL is unloaded (otherwise a leftover subclass
// would keep pointing at unloaded code and crash on the next message).
static CRITICAL_SECTION g_csSubclassedWindows;
static std::vector<HWND> g_subclassedWindows;

void RememberSubclassedWindow(HWND hWnd)
{
    EnterCriticalSection(&g_csSubclassedWindows);
    g_subclassedWindows.push_back(hWnd);
    LeaveCriticalSection(&g_csSubclassedWindows);
}

void ForgetSubclassedWindow(HWND hWnd)
{
    EnterCriticalSection(&g_csSubclassedWindows);
    for (auto it = g_subclassedWindows.begin(); it != g_subclassedWindows.end(); ++it)
    {
        if (*it == hWnd)
        {
            g_subclassedWindows.erase(it);
            break;
        }
    }
    LeaveCriticalSection(&g_csSubclassedWindows);
}

void EnsureGdiplus()
{
    if (g_gdiplusToken == 0)
    {
        Gdiplus::GdiplusStartupInput inp;
        Gdiplus::GdiplusStartup(&g_gdiplusToken, &inp, NULL);
    }
}

HICON LoadPngAsIconSized(const WCHAR* szFile, int targetSize)
{
    EnsureGdiplus();

    WCHAR dir[MAX_PATH];
    WCHAR stem[MAX_PATH];
    wcscpy_s(dir, MAX_PATH, szFile);

    WCHAR* lastSlash = wcsrchr(dir, L'\\');
    if (!lastSlash) return NULL;

    wcscpy_s(stem, MAX_PATH, lastSlash + 1);
    *(lastSlash + 1) = L'\0';

    WCHAR* pngPos = wcsstr(stem, L".png");
    if (pngPos) *pngPos = L'\0';

    const WCHAR* priorities[] = {
        L"targetsize-16",
        L"targetsize-24",
        L"scale-100",
        NULL
    };

    WCHAR bestFile[MAX_PATH] = {0};

    for (int p = 0; priorities[p] != NULL && bestFile[0] == L'\0'; p++)
    {
        WCHAR searchMask[MAX_PATH];
        swprintf_s(searchMask, MAX_PATH, L"%s%s*%s*.png", dir, stem, priorities[p]);

        WIN32_FIND_DATAW fd;
        HANDLE hFind = FindFirstFileW(searchMask, &fd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            swprintf_s(bestFile, MAX_PATH, L"%s%s", dir, fd.cFileName);
            FindClose(hFind);
        }
    }

    if (bestFile[0] == L'\0')
    {
        WCHAR searchMask[MAX_PATH];
        swprintf_s(searchMask, MAX_PATH, L"%s%s*.png", dir, stem);

        WIN32_FIND_DATAW fd;
        HANDLE hFind = FindFirstFileW(searchMask, &fd);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            swprintf_s(bestFile, MAX_PATH, L"%s%s", dir, fd.cFileName);
            FindClose(hFind);
        }
    }

    if (bestFile[0] == L'\0')
    {
        Wh_Log(L"No PNG found for: %s", szFile);
        return NULL;
    }

    Wh_Log(L"Loading PNG: %s at size %d", bestFile, targetSize);

    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(bestFile);
    if (!bmp || bmp->GetLastStatus() != Gdiplus::Ok)
    {
        if (bmp) delete bmp;
        return NULL;
    }

    Gdiplus::Bitmap* scaled = new Gdiplus::Bitmap(targetSize, targetSize, PixelFormat32bppARGB);
    if (!scaled || scaled->GetLastStatus() != Gdiplus::Ok)
    {
        delete bmp;
        if (scaled) delete scaled;
        return NULL;
    }

    Gdiplus::Graphics g(scaled);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.DrawImage(bmp, 0, 0, targetSize, targetSize);
    delete bmp;

    HICON hIcon = NULL;
    scaled->GetHICON(&hIcon);
    delete scaled;

    return hIcon;
}

HICON GetIconForWindow(HWND hWnd)
{
    HICON hIcon = NULL;

    IPropertyStore *pps = NULL;
    HRESULT hr = SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&pps));

    if (SUCCEEDED(hr))
    {
        PROPVARIANT pv;
        PropVariantInit(&pv);
        hr = pps->GetValue(PKEY_AppUserModel_ID_local, &pv);

        if (SUCCEEDED(hr) && pv.vt == VT_LPWSTR && pv.pwszVal)
        {
            IShellItem *psi = NULL;
            hr = SHCreateItemInKnownFolder(
                FOLDERID_AppsFolder_local,
                0,
                pv.pwszVal,
                IID_PPV_ARGS(&psi));

            if (SUCCEEDED(hr))
            {
                IExtractIconW *pei = NULL;
                hr = psi->BindToHandler(NULL, BHID_SFUIObject_local, IID_IExtractIconW_local, (void**)&pei);

                if (SUCCEEDED(hr))
                {
                    WCHAR szFile[MAX_PATH] = {0};
                    int iIndex = 0;
                    UINT uFlags = 0;
                    hr = pei->GetIconLocation(0, szFile, MAX_PATH, &iIndex, &uFlags);

                    if (SUCCEEDED(hr))
                    {
                        if (wcsstr(szFile, L".png"))
                        {
                            hIcon = LoadPngAsIconSized(szFile, 16);
                        }
                        else
                        {
                            hIcon = (HICON)LoadImageW(
                                NULL, szFile, IMAGE_ICON,
                                16, 16, LR_LOADFROMFILE);

                            if (!hIcon)
                            {
                                HICON hSmall = NULL, hLarge = NULL;
                                ExtractIconExW(szFile, iIndex, &hLarge, &hSmall, 1);
                                if (hSmall)
                                {
                                    hIcon = hSmall;
                                    if (hLarge) DestroyIcon(hLarge);
                                }
                                else if (hLarge)
                                {
                                    hIcon = hLarge;
                                }
                            }
                        }
                    }
                    pei->Release();
                }

                if (!hIcon)
                {
                    SHFILEINFOW sfi = {0};
                    DWORD_PTR ret = SHGetFileInfoW(pv.pwszVal, 0, &sfi, sizeof(sfi),
                        SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);

                    if (ret && sfi.hIcon)
                        hIcon = sfi.hIcon;
                }

                psi->Release();
            }
        }
        PropVariantClear(&pv);
        pps->Release();
    }

    return hIcon;
}

LRESULT CALLBACK SubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR uIdSubclass,
    DWORD_PTR dwRefData)
{
    if (uMsg == WM_NCDESTROY)
    {
        RemovePropW(hWnd, L"NativeTitlebarIconSet");
        RemoveWindowSubclass(hWnd, SubclassProc, uIdSubclass);
        ForgetSubclassedWindow(hWnd);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_NCPAINT || uMsg == WM_NCCALCSIZE)
    {
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_NCACTIVATE || uMsg == WM_ACTIVATE)
    {
        // Only try until we succeed once; the AppUserModelID / icon
        // may not be available yet right when the window is created,
        // but becomes available shortly after (around activation).
        if (!GetPropW(hWnd, L"NativeTitlebarIconSet"))
        {
            HWND hAppFrame = FindOwningApplicationFrameWindow(hWnd);

            if (hAppFrame)
            {
                HICON hIcon = GetIconForWindow(hAppFrame);
                if (hIcon)
                {
                    SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
                    SetPropW(hWnd, L"NativeTitlebarIconSet", (HANDLE)1);
                }
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef HWND (*CreateWindowInBandEx_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID, DWORD, DWORD);
CreateWindowInBandEx_t CreateWindowInBandEx_orig;
HWND WINAPI CreateWindowInBandEx_hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int x,
    int y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam,
    DWORD dwBand,
    DWORD dwTypeFlags)
{
    dwExStyle &= ~WS_EX_DLGMODALFRAME;
    dwExStyle &= ~0x00200000L;
    dwStyle = WS_OVERLAPPEDWINDOW | WS_DLGFRAME;

    HWND res = CreateWindowInBandEx_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);

    if (res != NULL)
    {
        SetWindowSubclass(res, SubclassProc, 1, 0);
        RememberSubclassedWindow(res);

        HWND hAppFrame = FindOwningApplicationFrameWindow(res);

        if (hAppFrame != NULL)
        {
            HICON hIcon = GetIconForWindow(hAppFrame);
            if (hIcon)
            {
                SendMessageW(res, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
                SetPropW(res, L"NativeTitlebarIconSet", (HANDLE)1);
            }
        }
    }

    return res;
}

typedef HRESULT (WINAPI *DwmExtendFrameIntoClientArea_t)(HWND, const MARGINS *);
DwmExtendFrameIntoClientArea_t DwmExtendFrameIntoClientArea_orig;
HRESULT WINAPI DwmExtendFrameIntoClientArea_hook(
    HWND hWnd,
    const MARGINS *pMarInset)
{
    WCHAR wszClassName[32] = {0};
    GetClassNameW(hWnd, wszClassName, ARRAYSIZE(wszClassName));

    if (wcscmp(wszClassName, L"ApplicationFrameWindow") == 0)
    {
        return 0x80263001;
    }

    return DwmExtendFrameIntoClientArea_orig(hWnd, pMarInset);
}

BOOL Wh_ModInit()
{
    InitializeCriticalSection(&g_csSubclassedWindows);

    Wh_SetFunctionHook((void *)GetProcAddress(LoadLibraryW(L"user32.dll"), "CreateWindowInBandEx"), (void *)CreateWindowInBandEx_hook, (void **)&CreateWindowInBandEx_orig);

    Wh_SetFunctionHook((void *)DwmExtendFrameIntoClientArea, (void *)DwmExtendFrameIntoClientArea_hook, (void **)&DwmExtendFrameIntoClientArea_orig);

    WindhawkUtils::SYMBOL_HOOK ApplicationFrame_dll_hooks[] = {
        {
            {L"private: long __cdecl CTitleBar::_CreateTitleBarWindow(void)"},
            (void **)&CTitleBar__CreateTitleBarWindow_orig,
            (void *)CTitleBar__CreateTitleBarWindow_hook,
            false
        },
        {
            {L"private: long __cdecl CTitleBar::_PaintButton(struct IDCompositionSurface *,enum CTitleBar::TitleBarControl,enum TITLE_BAR_BITMAP_TYPE,struct tagRECT const &,struct tagRECT const &)"},
            (void **)&CTitleBar__PaintButton_orig,
            (void *)CTitleBar__PaintButton_hook,
            false
        },
    };

    return WindhawkUtils::HookSymbols(LoadLibraryW(L"ApplicationFrame.dll"), ApplicationFrame_dll_hooks, ARRAYSIZE(ApplicationFrame_dll_hooks));
}

void Wh_ModUninit()
{
    // Remove our subclass from every window we're still attached to,
    // otherwise these windows would keep calling into SubclassProc
    // after this DLL is unloaded, causing a crash.
    EnterCriticalSection(&g_csSubclassedWindows);
    std::vector<HWND> windowsCopy = g_subclassedWindows;
    g_subclassedWindows.clear();
    LeaveCriticalSection(&g_csSubclassedWindows);

    for (HWND hWnd : windowsCopy)
    {
        if (IsWindow(hWnd))
        {
            RemovePropW(hWnd, L"NativeTitlebarIconSet");
            RemoveWindowSubclass(hWnd, SubclassProc, 1);
        }
    }

    DeleteCriticalSection(&g_csSubclassedWindows);

    if (g_gdiplusToken != 0)
    {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}
