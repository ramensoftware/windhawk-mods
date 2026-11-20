// ==WindhawkMod==
// @id              native-titlebars-uwp-lite
// @name            Remove UWP titlebars
// @description     Enables native titlebars in UWP apps
// @version         2.1.1
// @author          Anixx
// @github          https://github.com/Anixx
// @include         ApplicationFrameHost.exe
// @include         dwm.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lcomctl32 -ldwmapi -lshell32 -lole32 -lgdiplus -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

Replaces the UWP titlebars with native Win32 titlebars.

Also runs in dwm.exe: DWM normally gives "immersive" windows (UWP frames) only
a close button. The DWM part adds minimize/maximize back for frames that this
mod gave a native titlebar, so DWM (and OpenGlass) draw all three buttons.

![Screenshot](https://i.imgur.com/w5OqyJJ.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <CommCtrl.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <propsys.h>
#include <shlobj.h>
#include <uxtheme.h>
#include <gdiplus.h>
#include <vector>

#ifndef WM_POPUPSYSTEMMENU
#define WM_POPUPSYSTEMMENU 0x0313
#endif

static const GUID BHID_SFUIObject_local =
    {0x3981e225, 0xf559, 0x11d3, {0x8e, 0x3a, 0x00, 0xc0, 0x4f, 0x68, 0x37, 0xd5}};

static const GUID FOLDERID_AppsFolder_local =
    {0x1e87508d, 0x89c2, 0x42f0, {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};

static const GUID IID_IExtractIconW_local =
    {0x000214fa, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

static const PROPERTYKEY PKEY_AppUserModel_ID_local =
    {{0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}}, 5};

INT64 __fastcall CTitleBar_Generic_hook(void *, void *, void *, void *) {return 0;}

static void *g_dummyOrig;

static ULONG_PTR g_gdiplusToken = 0;

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

static CRITICAL_SECTION g_csGdiplus;

void EnsureGdiplus()
{
    EnterCriticalSection(&g_csGdiplus);
    if (g_gdiplusToken == 0)
    {
        Gdiplus::GdiplusStartupInput inp;
        Gdiplus::GdiplusStartup(&g_gdiplusToken, &inp, NULL);
    }
    LeaveCriticalSection(&g_csGdiplus);
}

HICON LoadPngAsIconSized(const WCHAR *szFile, int targetSize)
{
    EnsureGdiplus();

    WCHAR dir[MAX_PATH];
    WCHAR stem[MAX_PATH];
    wcscpy_s(dir, MAX_PATH, szFile);

    WCHAR *lastSlash = wcsrchr(dir, L'\\');
    if (!lastSlash) return NULL;

    wcscpy_s(stem, MAX_PATH, lastSlash + 1);
    *(lastSlash + 1) = L'\0';

    WCHAR *pngPos = wcsstr(stem, L".png");
    if (pngPos) *pngPos = L'\0';

    const WCHAR *priorities_large[] = {
        L"targetsize-256",
        L"targetsize-96",
        L"targetsize-48",
        L"scale-400",
        L"scale-200",
        L"scale-100",
        NULL
    };

    const WCHAR *priorities_small[] = {
        L"targetsize-16",
        L"targetsize-24",
        L"scale-100",
        NULL
    };

    const WCHAR **priorities = (targetSize >= 48) ? priorities_large : priorities_small;

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

    Gdiplus::Bitmap *bmp = new Gdiplus::Bitmap(bestFile);
    if (bmp->GetLastStatus() != Gdiplus::Ok)
    {
        delete bmp;
        return NULL;
    }

    Gdiplus::Bitmap *scaled = new Gdiplus::Bitmap(targetSize, targetSize, PixelFormat32bppARGB);
    if (scaled->GetLastStatus() != Gdiplus::Ok)
    {
        delete bmp;
        delete scaled;
        return NULL;
    }

    {
        Gdiplus::Graphics g(scaled);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.DrawImage(bmp, 0, 0, targetSize, targetSize);
    }
    delete bmp;

    HICON hIcon = NULL;
    scaled->GetHICON(&hIcon);
    delete scaled;

    return hIcon;
}

HICON GetIconForWindow(HWND hWnd, int size)
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
                hr = psi->BindToHandler(NULL, BHID_SFUIObject_local,
                                        IID_IExtractIconW_local, (void **)&pei);

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
                            hIcon = LoadPngAsIconSized(szFile, size);
                        }
                        else
                        {
                            hIcon = (HICON)LoadImageW(
                                NULL, szFile, IMAGE_ICON,
                                size, size, LR_LOADFROMFILE);

                            if (!hIcon)
                            {
                                HICON hSmall = NULL, hLarge = NULL;
                                ExtractIconExW(szFile, iIndex, &hLarge, &hSmall, 1);

                                HICON hWanted = (size >= 32) ? hLarge : hSmall;
                                HICON hOther  = (size >= 32) ? hSmall : hLarge;

                                if (hWanted)
                                {
                                    hIcon = hWanted;
                                    if (hOther) DestroyIcon(hOther);
                                }
                                else if (hOther)
                                {
                                    hIcon = hOther;
                                }
                            }
                        }
                    }
                    pei->Release();
                }

                if (!hIcon)
                {
                    SHFILEINFOW sfi = {0};
                    UINT sizeFlag = (size >= 32) ? SHGFI_LARGEICON : SHGFI_SMALLICON;
                    DWORD_PTR ret = SHGetFileInfoW(pv.pwszVal, 0, &sfi, sizeof(sfi),
                                                   SHGFI_ICON | sizeFlag |
                                                   SHGFI_USEFILEATTRIBUTES);
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

void SetTitlebarIcon(HWND hWnd)
{
    HICON hSmall = GetIconForWindow(hWnd, 16);
    HICON hBig   = GetIconForWindow(hWnd, 256);

    if (hSmall)
    {
        SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmall);
        SetPropW(hWnd, L"NativeTitlebarIconSmall", (HANDLE)hSmall);
    }

    if (hBig)
    {
        SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hBig);
        SetPropW(hWnd, L"NativeTitlebarIconBig", (HANDLE)hBig);
    }

    if (hSmall || hBig)
    {
        SetPropW(hWnd, L"NativeTitlebarIconSet", (HANDLE)1);
    }
}

void DestroyTitlebarIcons(HWND hWnd)
{
    if (HICON hSmall = (HICON)GetPropW(hWnd, L"NativeTitlebarIconSmall"))
    {
        DestroyIcon(hSmall);
        RemovePropW(hWnd, L"NativeTitlebarIconSmall");
    }

    if (HICON hBig = (HICON)GetPropW(hWnd, L"NativeTitlebarIconBig"))
    {
        DestroyIcon(hBig);
        RemovePropW(hWnd, L"NativeTitlebarIconBig");
    }

    RemovePropW(hWnd, L"NativeTitlebarIconSet");
}

LRESULT CALLBACK SubclassProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    DWORD_PTR dwRefData)
{
    if (uMsg == WM_NCDESTROY)
    {
        DestroyTitlebarIcons(hWnd);
        ForgetSubclassedWindow(hWnd);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }
    if (uMsg == WM_NCRBUTTONDOWN ||
        uMsg == WM_NCRBUTTONUP ||
        uMsg == WM_NCLBUTTONDBLCLK ||
        uMsg == WM_POPUPSYSTEMMENU ||
        uMsg == WM_SYSCOMMAND ||
        uMsg == WM_NCPAINT || 
        uMsg == WM_NCCALCSIZE)
    {
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    // Caption buttons are drawn by DWM (and OpenGlass), so their geometry is
    // DWM's, not the UWP title bar's. Let DefWindowProc hit-test them and
    // drive hover/press/click; everything else keeps ApplicationFrameHost's
    // own handling.
    // AFH's window procedure keeps WM_SETICON to itself, so the native frame
    // (drawn by DefWindowProc) never learns the icon. Store it there too.
    if (uMsg == WM_SETICON)
    {
        DefWindowProcW(hWnd, uMsg, wParam, lParam);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_NCHITTEST)
    {
        LRESULT ht = DefWindowProcW(hWnd, uMsg, wParam, lParam);
        if (ht == HTMINBUTTON || ht == HTMAXBUTTON || ht == HTCLOSE)
            return ht;
        LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        // Don't let AFH claim button codes where DWM has no button.
        if (res == HTMINBUTTON || res == HTMAXBUTTON || res == HTCLOSE)
            return HTCAPTION;
        return res;
    }

    if ((uMsg == WM_NCLBUTTONDOWN || uMsg == WM_NCLBUTTONUP ||
         uMsg == WM_NCLBUTTONDBLCLK || uMsg == WM_NCMOUSEMOVE ||
         uMsg == WM_NCMOUSEHOVER) &&
        (wParam == HTMINBUTTON || wParam == HTMAXBUTTON || wParam == HTCLOSE))
    {
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_NCMOUSELEAVE)
    {
        DefWindowProcW(hWnd, uMsg, wParam, lParam);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_NCACTIVATE || uMsg == WM_ACTIVATE)
    {
        if (!GetPropW(hWnd, L"NativeTitlebarIconSet"))
        {
            SetTitlebarIcon(hWnd);
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
    if (IS_INTRESOURCE(lpClassName) ||
        _wcsicmp(lpClassName, L"ApplicationFrameWindow") != 0)
    {
        return CreateWindowInBandEx_orig(
            dwExStyle, lpClassName, lpWindowName, dwStyle,
            x, y, nWidth, nHeight,
            hWndParent, hMenu, hInstance, lpParam,
            dwBand, dwTypeFlags);
    }

    dwExStyle &= ~WS_EX_DLGMODALFRAME;
    dwExStyle &= ~WS_EX_NOREDIRECTIONBITMAP;
    dwStyle = WS_OVERLAPPEDWINDOW;

    // Note: AFH passes dwTypeFlags=0x3. Bit 0x1 makes DWM show only the
    // close button, but both bits are required for the app's CoreWindow to
    // be hosted inside the frame, so they must be left unchanged.

    HWND res = CreateWindowInBandEx_orig(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        x, y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam,
        dwBand, dwTypeFlags);

    if (res != NULL)
    {
        WindhawkUtils::SetWindowSubclassFromAnyThread(res, SubclassProc, 0);
        RememberSubclassedWindow(res);
        SetTitlebarIcon(res);
    }

    return res;
}

typedef HRESULT(WINAPI *DwmExtendFrameIntoClientArea_t)(HWND, const MARGINS *);
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

// ApplicationFrameHost calls SetWindowThemeAttribute(WTA_NONCLIENT) on the
// frame with flags 0x10007 / mask 0x1000F. Bit 0x10000 is undocumented and
// makes DWM drop the min/max/close caption buttons (the UWP title bar draws
// its own). With the UWP title bar stubbed out, nothing draws them at all,
// so strip that bit for frame windows. The documented bits 0x7
// (NODRAWCAPTION | NODRAWICON | NOSYSMENU) are stripped too, otherwise stock
// DWM draws the native titlebar without title text and icon.
#define WTNCA_NOCAPTIONBUTTONS_UNDOC 0x10000
#define WTNCA_STRIP_FLAGS (WTNCA_NOCAPTIONBUTTONS_UNDOC | WTNCA_NODRAWCAPTION | \
                           WTNCA_NODRAWICON | WTNCA_NOSYSMENU)

// Frames converted by this mod have a redirection bitmap; stock UWP frames
// (e.g. created before the mod loaded) don't, and are left alone.
static bool IsConvertedFrame(HWND hWnd)
{
    return !(GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_NOREDIRECTIONBITMAP);
}

static bool IsApplicationFrameWindow(HWND hWnd)
{
    WCHAR wszClassName[32] = {0};
    return hWnd && GetClassNameW(hWnd, wszClassName, ARRAYSIZE(wszClassName)) &&
           wcscmp(wszClassName, L"ApplicationFrameWindow") == 0;
}

typedef HRESULT(WINAPI *SetWindowThemeAttribute_t)(HWND, enum WINDOWTHEMEATTRIBUTETYPE, PVOID, DWORD);
SetWindowThemeAttribute_t SetWindowThemeAttribute_orig;
HRESULT WINAPI SetWindowThemeAttribute_hook(
    HWND hWnd,
    enum WINDOWTHEMEATTRIBUTETYPE eAttribute,
    PVOID pvAttribute,
    DWORD cbAttribute)
{
    if (eAttribute == WTA_NONCLIENT &&
        pvAttribute && cbAttribute >= sizeof(WTA_OPTIONS) &&
        IsApplicationFrameWindow(hWnd) && IsConvertedFrame(hWnd))
    {
        WTA_OPTIONS opts = *(const WTA_OPTIONS *)pvAttribute;
        opts.dwFlags &= ~WTNCA_STRIP_FLAGS;
        return SetWindowThemeAttribute_orig(hWnd, eAttribute, &opts, sizeof(opts));
    }

    return SetWindowThemeAttribute_orig(hWnd, eAttribute, pvAttribute, cbAttribute);
}

// Restore caption buttons on frames that already exist when the mod (re)loads,
// e.g. after editing/recompiling. Only touches frames that already carry the
// native titlebar (no WS_EX_NOREDIRECTIONBITMAP), i.e. ones this mod created.
static BOOL CALLBACK RestoreCaptionButtonsEnumProc(HWND hWnd, LPARAM)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId() &&
        IsApplicationFrameWindow(hWnd) &&
        IsConvertedFrame(hWnd))
    {
        WTA_OPTIONS opts = {0, WTNCA_STRIP_FLAGS};
        SetWindowThemeAttribute(hWnd, WTA_NONCLIENT, &opts, sizeof(opts));
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
    return TRUE;
}

// ---------------------------------------------------------------------------
// dwm.exe part
// ---------------------------------------------------------------------------
// uDWM decides which caption buttons a window gets in
// CTopLevelWindow::GetCurrentStyle. For immersive windows (UWP frames are
// created with CreateWindowInBandEx type flag 0x1) it adds close but skips
// minimize/maximize. That flag can't be changed in ApplicationFrameHost without
// breaking app hosting, so add the bits here. Only windows that already got a
// close button qualify; stock UWP frames don't (AFH hides it with theme flag
// 0x10000, which the AFH part strips only for frames it converted).

constexpr UINT NCF_MIN_VISIBLE   = 0x0100;
constexpr UINT NCF_MAX_VISIBLE   = 0x0200;
constexpr UINT NCF_CLOSE_VISIBLE = 0x0400;
constexpr UINT NCF_MIN_ENABLED   = 0x1000;
constexpr UINT NCF_MAX_ENABLED   = 0x2000;
// Also skipped for immersive windows: caption icon and system-menu area
// (set by the non-immersive path unless WTNCA_NODRAWICON / WTNCA_NOSYSMENU,
// which the AFH part strips for converted frames).
constexpr UINT NCF_ICON          = 0x10000;
constexpr UINT NCF_SYSMENU       = 0x0080;

// CWindowData style copy as read by GetCurrentStyle on uDWM 10.0.26100.x.
// Sanity-checked below, so a wrong offset on another build is a no-op.
constexpr size_t kWindowDataStyleOffset = 0x74;

using IsImmersiveWindow_t = bool(*)(const void *pWindowData);
IsImmersiveWindow_t CWindowData_IsImmersiveWindow;

using GetCurrentStyle_t = UINT(*)(const void *pWindowData, bool);
GetCurrentStyle_t GetCurrentStyle_orig;
UINT GetCurrentStyle_hook(const void *pWindowData, bool b)
{
    UINT flags = GetCurrentStyle_orig(pWindowData, b);

    if (!pWindowData || !(flags & NCF_CLOSE_VISIBLE) ||
        (flags & (NCF_MIN_VISIBLE | NCF_MAX_VISIBLE)))
        return flags;

    DWORD style = *(const DWORD *)((const BYTE *)pWindowData + kWindowDataStyleOffset);
    if ((style & (WS_CAPTION | WS_SYSMENU)) != (WS_CAPTION | WS_SYSMENU) ||
        !(style & (WS_MINIMIZEBOX | WS_MAXIMIZEBOX)) ||
        !CWindowData_IsImmersiveWindow(pWindowData))
        return flags;

    // Same bits the non-immersive path sets.
    flags |= NCF_ICON | NCF_SYSMENU;
    if (style & WS_MINIMIZEBOX) flags |= NCF_MIN_VISIBLE | NCF_MAX_VISIBLE | NCF_MIN_ENABLED;
    if (style & WS_MAXIMIZEBOX) flags |= NCF_MIN_VISIBLE | NCF_MAX_VISIBLE | NCF_MAX_ENABLED;
    return flags;
}

static bool g_isDwm = false;

static BOOL DwmModInit()
{
    HMODULE uDWM = GetModuleHandleW(L"uDWM.dll");
    if (!uDWM)
    {
        Wh_Log(L"uDWM.dll not loaded");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                L"public: static enum NCAREA_FLAGS __cdecl CTopLevelWindow::GetCurrentStyle(class CWindowData const *,bool)",
                L"?GetCurrentStyle@CTopLevelWindow@@SA?AW4NCAREA_FLAGS@@PEBVCWindowData@@_N@Z",
            },
            (void **)&GetCurrentStyle_orig,
            (void *)GetCurrentStyle_hook,
        },
        {
            {
                L"public: bool __cdecl CWindowData::IsImmersiveWindow(void)const ",
                L"public: bool __cdecl CWindowData::IsImmersiveWindow(void)const",
                L"?IsImmersiveWindow@CWindowData@@QEBA_NXZ",
            },
            (void **)&CWindowData_IsImmersiveWindow,
            nullptr,
        },
    };

    if (!WindhawkUtils::HookSymbols(uDWM, hooks, ARRAYSIZE(hooks)))
    {
        Wh_Log(L"uDWM symbol hooks failed");
        return FALSE;
    }
    return TRUE;
}

// ---------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    WCHAR exe[MAX_PATH];
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    const WCHAR *name = wcsrchr(exe, L'\\');
    if (name && _wcsicmp(name + 1, L"dwm.exe") == 0)
    {
        g_isDwm = true;
        return DwmModInit();
    }

    InitializeCriticalSection(&g_csSubclassedWindows);
    InitializeCriticalSection(&g_csGdiplus);

    if (HMODULE hUxTheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32))
    {
        if (void *p = (void *)GetProcAddress(hUxTheme, "SetWindowThemeAttribute"))
        {
            Wh_SetFunctionHook(p, (void *)SetWindowThemeAttribute_hook,
                               (void **)&SetWindowThemeAttribute_orig);
        }
    }

    Wh_SetFunctionHook(
        (void *)GetProcAddress(GetModuleHandle(L"user32.dll"), "CreateWindowInBandEx"),
        (void *)CreateWindowInBandEx_hook,
        (void **)&CreateWindowInBandEx_orig);

    Wh_SetFunctionHook(
        (void *)DwmExtendFrameIntoClientArea,
        (void *)DwmExtendFrameIntoClientArea_hook,
        (void **)&DwmExtendFrameIntoClientArea_orig);

    WindhawkUtils::SYMBOL_HOOK ApplicationFrame_dll_hooks[] = {
        { {L"private: long __cdecl CTitleBar::_NotifyViewOfPersistence(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_OnTitleBarHitTestVisualUpdated(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_GetApplicationViewTitleBar(struct _GUID const &,void * *)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_NotifyVisibilityChanged(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateBackgroundVisualTree(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateConstraints(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateHoverPressState(enum CTitleBar::TitleBarControl)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateColorsFromApplicationViewAsync(bool)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateHitTestWindow(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_Cloak(int,bool,bool)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_ValidateDCompDevice(long)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_PaintTitle(struct IDCompositionSurface *,struct tagRECT const &)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_ComputeTitleBarRects(struct tagRECT const *)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_PlaceVisuals(struct tagRECT const *,bool)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateColorsFromApplicationView(bool)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateMica(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateBackgroundSurfaces(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_ManuallyPositionElements(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_SetIsVisible(int)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_NotifyReservedMetricsChanged(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_GetFrame(struct IApplicationFrameInternal * *)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_ShowTitleBar(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_GetCoreApplicationViewTitleBar(struct _GUID const &,void * *)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_ShowActionsMenu(struct tagPOINT *,enum TitleBarTelemetry::ACTIONS_MENU_INVOKE)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_InitializePersistence(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_PersistColors(struct Windows::UI::ViewManagement::IApplicationViewTitleBar *)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateDrawnByAppSetting(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_CommitDCompDevice(void *)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_InitializeDCompVariables(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_AnimateTitleBar(bool)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateButtonState(enum CTitleBar::TitleBarControl,bool)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_HideActionsMenu(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_GetDCompProvider(struct IDCompProvider * *)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateBackgroundTransforms(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_VisibleButtonsUpdated(void)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateButtonStateAndCommit(enum CTitleBar::TitleBarControl,bool)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_ShowSystemMenu(struct tagPOINT *,enum TitleBarTelemetry::SYSTEM_MENU_INVOKE)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateVisiblityForHideOptions(enum TITLE_BAR_HIDE_OPTIONS)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
        { {L"private: long __cdecl CTitleBar::_UpdateAllButtonStates(enum CTitleBar::DrawButtonFilter,bool)"}, (void **)&g_dummyOrig, (void *)CTitleBar_Generic_hook, true },
    };

    return WindhawkUtils::HookSymbols(
        LoadLibraryExW(L"ApplicationFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32),
        ApplicationFrame_dll_hooks,
        ARRAYSIZE(ApplicationFrame_dll_hooks));
}

void Wh_ModAfterInit()
{
    if (g_isDwm) return;
    EnumWindows(RestoreCaptionButtonsEnumProc, 0);
}

void Wh_ModUninit()
{
    if (g_isDwm) return;
    EnterCriticalSection(&g_csSubclassedWindows);
    std::vector<HWND> windowsCopy = g_subclassedWindows;
    g_subclassedWindows.clear();
    LeaveCriticalSection(&g_csSubclassedWindows);

    for (HWND hWnd : windowsCopy)
    {
        if (IsWindow(hWnd))
        {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
            SendMessageW(hWnd, WM_SETICON, ICON_SMALL, 0);
            SendMessageW(hWnd, WM_SETICON, ICON_BIG, 0);
            DestroyTitlebarIcons(hWnd);
        }
    }

    DeleteCriticalSection(&g_csSubclassedWindows);
    DeleteCriticalSection(&g_csGdiplus);

    if (g_gdiplusToken != 0)
    {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}
