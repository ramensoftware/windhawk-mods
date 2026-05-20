// ==WindhawkMod==
// @id              native-titlebars-uwp-lite
// @name            Remove UWP titlebars Lite
// @description     Enables native titlebars in UWP apps
// @version         1.1.0
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

![Screenshot](https://i.imgur.com/gqw78kB.png)

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

// Определяем все GUID локально
static const GUID BHID_SFUIObject_local =
    {0x3981e225, 0xf559, 0x11d3, {0x8e, 0x3a, 0x00, 0xc0, 0x4f, 0x68, 0x37, 0xd5}};

static const GUID FOLDERID_AppsFolder_local =
    {0x1e87508d, 0x89c2, 0x42f0, {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};

static const GUID IID_IExtractIconW_local =
    {0x000214fa, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

// PKEY_AppUserModel_ID = {9F4C2855-9F79-4B39-A8D0-E1D42DE1D5F3}, 5
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

HICON BitmapToHIcon(Gdiplus::Bitmap* bitmap)
{
    HICON hIcon = NULL;
    if (bitmap && bitmap->GetLastStatus() == Gdiplus::Ok)
    {
        bitmap->GetHICON(&hIcon);
    }
    return hIcon;
}

HICON LoadPngAsIcon(const WCHAR* szFile)
{
    using namespace Gdiplus;
    
    static ULONG_PTR gdiplusToken = 0;
    if (gdiplusToken == 0)
    {
        GdiplusStartupInput gdiplusStartupInput;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    }

    // szFile = "C:\...\StoreAppList.png-100"
    // Нам нужно найти "StoreAppList.scale-100.png" или похожее
    
    // Получаем директорию и базовое имя файла
    WCHAR dir[MAX_PATH];
    WCHAR baseName[MAX_PATH];
    wcscpy_s(dir, MAX_PATH, szFile);
    
    // Обрезаем до директории
    WCHAR* lastSlash = wcsrchr(dir, L'\\');
    if (!lastSlash) return NULL;
    
    wcscpy_s(baseName, MAX_PATH, lastSlash + 1);
    *(lastSlash + 1) = L'\0';
    
    // Убираем суффикс "-100" из имени файла
    // baseName = "StoreAppList.png-100" -> "StoreAppList"
    WCHAR stem[MAX_PATH];
    wcscpy_s(stem, MAX_PATH, baseName);
    WCHAR* pngPos = wcsstr(stem, L".png");
    if (pngPos) *pngPos = L'\0';
    
    Wh_Log(L"Dir: %s, Stem: %s", dir, stem);
    
    // Ищем файлы по маске stem*.png
    WCHAR searchMask[MAX_PATH];
    swprintf_s(searchMask, MAX_PATH, L"%s%s*.png", dir, stem);
    Wh_Log(L"Search mask: %s", searchMask);
    
    WCHAR bestFile[MAX_PATH] = {0};
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(searchMask, &fd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            Wh_Log(L"Found file: %s", fd.cFileName);
            // Берём первый найденный файл
            if (bestFile[0] == L'\0')
            {
                swprintf_s(bestFile, MAX_PATH, L"%s%s", dir, fd.cFileName);
            }
            // Предпочитаем scale-100 или просто .png
            if (wcsstr(fd.cFileName, L"scale-100") ||
                wcscmp(fd.cFileName + wcslen(fd.cFileName) - 4, L".png") == 0)
            {
                swprintf_s(bestFile, MAX_PATH, L"%s%s", dir, fd.cFileName);
                break;
            }
        }
        while (FindNextFileW(hFind, &fd));
        FindClose(hFind);
    }
    else
    {
        Wh_Log(L"FindFirstFile failed, error=%d", GetLastError());
    }

    if (bestFile[0] == L'\0')
    {
        Wh_Log(L"No PNG file found");
        return NULL;
    }

    Wh_Log(L"Loading: %s", bestFile);

    IStream* pStream = NULL;
    HRESULT hr = SHCreateStreamOnFileEx(
        bestFile,
        STGM_READ | STGM_SHARE_DENY_NONE,
        0, FALSE, NULL, &pStream);

    if (SUCCEEDED(hr) && pStream)
    {
        Bitmap* bitmap = new Bitmap(pStream);
        pStream->Release();
        
        if (bitmap && bitmap->GetLastStatus() == Ok)
        {
            Wh_Log(L"Bitmap loaded, width=%d height=%d", bitmap->GetWidth(), bitmap->GetHeight());
            HICON hIcon = BitmapToHIcon(bitmap);
            delete bitmap;
            Wh_Log(L"HICON: %p", hIcon);
            return hIcon;
        }
        else if (bitmap)
        {
            Wh_Log(L"Bitmap status=%d", bitmap->GetLastStatus());
            delete bitmap;
        }
    }
    else
    {
        Wh_Log(L"Stream failed hr=0x%08X", hr);
    }

    // Прямая загрузка
    Bitmap* bitmap = new Bitmap(bestFile);
    if (bitmap && bitmap->GetLastStatus() == Ok)
    {
        HICON hIcon = BitmapToHIcon(bitmap);
        delete bitmap;
        return hIcon;
    }
    if (bitmap) delete bitmap;

    Wh_Log(L"Failed to load PNG");
    return NULL;
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
                            hIcon = LoadPngAsIcon(szFile);
                        }
                        else
                        {
                            HICON hLarge = NULL, hSmall = NULL;
                            ExtractIconExW(szFile, iIndex, &hLarge, &hSmall, 1);

                            if (hLarge)
                                hIcon = hLarge;
                            else if (hSmall)
                                hIcon = hSmall;
                        }
                    }
                    pei->Release();
                }

                if (!hIcon)
                {
                    SHFILEINFOW sfi = {0};
                    DWORD_PTR ret = SHGetFileInfoW(pv.pwszVal, 0, &sfi, sizeof(sfi),
                        SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);

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
        RemoveWindowSubclass(hWnd, SubclassProc, uIdSubclass);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_NCPAINT || uMsg == WM_NCCALCSIZE)
    {
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_NCACTIVATE || uMsg == WM_ACTIVATE)
    {
        HWND hAppFrame = FindApplicationFrameWindow();

        if (hAppFrame)
        {
            HICON hIcon = GetIconForWindow(hAppFrame);
            if (hIcon)
            {
                SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
                SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
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
    dwExStyle &= ~0x00200000L; // WS_EX_NOREDIRECTIONBITMAP
    dwStyle = WS_OVERLAPPEDWINDOW | WS_DLGFRAME;

    HWND res = CreateWindowInBandEx_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);

    if (res != NULL)
    {
        SetWindowSubclass(res, SubclassProc, 1, 0);

        HWND hAppFrame = FindApplicationFrameWindow();

        if (hAppFrame != NULL)
        {
            HICON hIcon = GetIconForWindow(hAppFrame);
            if (hIcon)
            {
                SendMessageW(res, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
                SendMessageW(res, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
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
        return 0x80263001; // DWM_E_COMPOSITIONDISABLED
    }

    return DwmExtendFrameIntoClientArea_orig(hWnd, pMarInset);
}

BOOL Wh_ModInit()
{
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
