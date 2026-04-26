// ==WindhawkMod==
// @id              paste-clipboard-content-to-explorer
// @name            Paste Clipboard Content to Explorer
// @description     Paste text and images from clipboard as files in Explorer and in file dialogs
// @version         1.2
// @author          Anixx
// @github          https://github.com/Anixx
// @include         *
// @compilerOptions -lole32 -loleaut32 -luuid -lgdiplus -lshlwapi -lshell32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Paste Clipboard Content to Explorer

Allows pasting text and images from clipboard as files in Explorer,
on the desktop and in open/save file dialogs.

## Features
- Paste text as .txt (UTF-16 LE with BOM)
- Paste images as .png
- Auto-naming with incrementing number on conflict
- Works in Explorer folders, on the desktop, in file dialogs
- Works with network folders (UNC paths)
- Paste menu item is grayed out in virtual folders (This PC, Network, etc.)
- Ctrl+V, context menu Paste and menubar Paste all work
- Shows system error dialog when pasting into a folder without write access
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <gdiplus.h>
#include <string>

using namespace Gdiplus;

static ULONG_PTR g_gdiplusToken = 0;
static CRITICAL_SECTION g_gdiplusCS;
static bool g_gdiplusCSInit = false;
static HWND g_lastContextMenuShellView = nullptr;
static bool g_menuCanPaste = false;
static bool PerformPaste(HWND sourceWindow);

// ============================================================
//  Lazy GDI+ initialization
// ============================================================
static bool EnsureGdiplusInitialized()
{
    if (g_gdiplusToken != 0) return true;
    if (!g_gdiplusCSInit)    return false;
    EnterCriticalSection(&g_gdiplusCS);
    bool result = false;
    if (g_gdiplusToken == 0)
    {
        GdiplusStartupInput gsi = {};
        gsi.GdiplusVersion = 1;
        Status st = GdiplusStartup(&g_gdiplusToken, &gsi, nullptr);
        result = (st == Ok);
        if (!result) g_gdiplusToken = 0;
    }
    else result = true;
    LeaveCriticalSection(&g_gdiplusCS);
    return result;
}

// ============================================================
//  RAII COM init
// ============================================================
struct ScopedCoInit
{
    bool m_initialized = false;
    ScopedCoInit()
    {
        HRESULT hr = CoInitializeEx(nullptr,
                                    COINIT_APARTMENTTHREADED |
                                    COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr)) m_initialized = true;
    }
    ~ScopedCoInit() { if (m_initialized) CoUninitialize(); }
};

// ============================================================
//  Default base name
// ============================================================
static std::wstring GetDefaultBaseName()
{
    std::wstring result;
    HMODULE hMod = LoadLibraryExW(L"explorerframe.dll", nullptr,
                                  LOAD_LIBRARY_AS_DATAFILE |
                                  LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (hMod)
    {
        WCHAR buf[512] = {};
        if (LoadStringW(hMod, 49922, buf, 512) > 0) result = buf;
        FreeLibrary(hMod);
    }
    if (result.empty()) result = L"Clipboard";
    return result;
}

// ============================================================
//  Unique file path
// ============================================================
static std::wstring GetUniquePath(const std::wstring& dir,
                                  const std::wstring& baseName,
                                  const std::wstring& ext)
{
    std::wstring p = dir + L'\\' + baseName + ext;
    if (!PathFileExistsW(p.c_str())) return p;
    for (int n = 1; n < 100000; ++n)
    {
        p = dir + L'\\' + baseName + L" (" + std::to_wstring(n) + L")" + ext;
        if (!PathFileExistsW(p.c_str())) return p;
    }
    return dir + L'\\' + baseName + ext;
}

// ============================================================
//  GDI+ encoder
// ============================================================
static bool GetPngClsid(CLSID* clsid)
{
    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);
    if (!size) return false;
    ImageCodecInfo* info = (ImageCodecInfo*)malloc(size);
    if (!info) return false;
    GetImageEncoders(num, size, info);
    bool found = false;
    for (UINT i = 0; i < num; ++i)
        if (!wcscmp(info[i].MimeType, L"image/png"))
            { *clsid = info[i].Clsid; found = true; break; }
    free(info);
    return found;
}

// ============================================================
//  Check write access
// ============================================================
static DWORD CheckDirectoryWriteAccess(const std::wstring& dir)
{
    HANDLE h = CreateFileW(dir.c_str(), GENERIC_WRITE,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_EXISTING,
                           FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (h == INVALID_HANDLE_VALUE) return GetLastError();
    CloseHandle(h);
    return 0;
}

// ============================================================
//  Show access denied message
// ============================================================
static void ShowAccessDeniedMessage(HWND hwnd, const std::wstring& path)
{
    LPWSTR buf = nullptr;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, ERROR_ACCESS_DENIED,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPWSTR)&buf, 0, nullptr);
    std::wstring msg;
    if (buf) { msg = buf; LocalFree(buf);
        while (!msg.empty() && (msg.back()==L'\n'||msg.back()==L'\r')) msg.pop_back(); }
    else msg = L"Access is denied.";
    msg += L"\n\n"; msg += path;
    MessageBoxW(hwnd, msg.c_str(), nullptr, MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
}

// ============================================================
//  Save image
// ============================================================
static DWORD SaveImageFromClipboard(const std::wstring& filePath,
                                    const std::wstring& targetDir)
{
    if (!EnsureGdiplusInitialized()) return ERROR_NOT_SUPPORTED;
    if (!OpenClipboard(nullptr)) return GetLastError();
    CLSID pngClsid;
    DWORD errCode = ERROR_NOT_SUPPORTED;
    if (GetPngClsid(&pngClsid))
    {
        Bitmap* bmp = nullptr;
        HANDLE hDibV5 = GetClipboardData(CF_DIBV5);
        HANDLE hDib   = GetClipboardData(CF_DIB);
        HBITMAP hBmp  = (HBITMAP)GetClipboardData(CF_BITMAP);
        if (hDibV5)
        {
            BITMAPV5HEADER* p = (BITMAPV5HEADER*)GlobalLock(hDibV5);
            if (p) {
                DWORD clrSz = 0;
                if (p->bV5BitCount <= 8)
                    clrSz = (p->bV5ClrUsed ? p->bV5ClrUsed : (1u<<p->bV5BitCount))*sizeof(RGBQUAD);
                else if (p->bV5Compression == BI_BITFIELDS) clrSz = 3*sizeof(DWORD);
                bmp = new Bitmap((BITMAPINFO*)p, (BYTE*)p+p->bV5Size+clrSz);
                if (bmp->GetLastStatus()!=Ok){delete bmp;bmp=nullptr;}
                GlobalUnlock(hDibV5);
            }
        }
        if (!bmp && hDib)
        {
            BITMAPINFO* bi = (BITMAPINFO*)GlobalLock(hDib);
            if (bi) {
                DWORD clrSz = 0;
                if (bi->bmiHeader.biBitCount <= 8)
                    clrSz = (bi->bmiHeader.biClrUsed ? bi->bmiHeader.biClrUsed : (1u<<bi->bmiHeader.biBitCount))*sizeof(RGBQUAD);
                else if (bi->bmiHeader.biCompression == BI_BITFIELDS) clrSz = 3*sizeof(DWORD);
                bmp = new Bitmap(bi, (BYTE*)bi+bi->bmiHeader.biSize+clrSz);
                if (bmp->GetLastStatus()!=Ok){delete bmp;bmp=nullptr;}
                GlobalUnlock(hDib);
            }
        }
        if (!bmp && hBmp)
        {
            bmp = Bitmap::FromHBITMAP(hBmp, nullptr);
            if (bmp && bmp->GetLastStatus()!=Ok){delete bmp;bmp=nullptr;}
        }
        if (bmp)
        {
            Status st = bmp->Save(filePath.c_str(), &pngClsid, nullptr);
            if (st == Ok) errCode = 0;
            else { DWORD e = CheckDirectoryWriteAccess(targetDir);
                   errCode = e ? e : ERROR_WRITE_FAULT; }
            delete bmp;
        }
    }
    CloseClipboard();
    return errCode;
}

// ============================================================
//  Save text
// ============================================================
static DWORD SaveTextFromClipboard(const std::wstring& filePath)
{
    if (!OpenClipboard(nullptr)) return GetLastError();
    DWORD errCode = ERROR_NO_DATA;
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    if (h)
    {
        WCHAR* text = (WCHAR*)GlobalLock(h);
        if (text)
        {
            HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_WRITE, 0,
                                       nullptr, CREATE_ALWAYS,
                                       FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                DWORD written;
                WORD bom = 0xFEFF;
                WriteFile(hFile, &bom, sizeof(bom), &written, nullptr);
                WriteFile(hFile, text, (DWORD)(wcslen(text)*sizeof(WCHAR)),
                          &written, nullptr);
                CloseHandle(hFile);
                errCode = 0;
            }
            else errCode = GetLastError();
            GlobalUnlock(h);
        }
    }
    CloseClipboard();
    return errCode;
}

// ============================================================
//  SHELLDLL_DefView detection
// ============================================================
static HWND FindShellDefViewInChain(HWND hwnd)
{
    HWND w = hwnd;
    while (w)
    {
        WCHAR cls[256] = {};
        GetClassNameW(w, cls, 256);
        if (!wcscmp(cls, L"SHELLDLL_DefView")) return w;
        HWND par = GetParent(w);
        if (!par) par = GetWindow(w, GW_OWNER);
        w = par;
    }
    return nullptr;
}

static bool IsShellViewWindow(HWND hwnd)
{
    return FindShellDefViewInChain(hwnd) != nullptr;
}

// ============================================================
//  Clipboard check
// ============================================================
static bool HasNonFileClipboardContent()
{
    if (IsClipboardFormatAvailable(CF_HDROP)) return false;
    return IsClipboardFormatAvailable(CF_UNICODETEXT) ||
           IsClipboardFormatAvailable(CF_BITMAP)      ||
           IsClipboardFormatAvailable(CF_DIB)         ||
           IsClipboardFormatAvailable(CF_DIBV5);
}

// ============================================================
//  Get current folder path
// ============================================================
static bool IsDesktopShellWindow(HWND hwnd)
{
    HWND root = GetAncestor(hwnd, GA_ROOT);
    WCHAR cls[64] = {};
    GetClassNameW(root, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW")) return true;
    HWND par = GetParent(root);
    if (par) {
        GetClassNameW(par, cls, 64);
        if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW")) return true;
    }
    return false;
}

static bool GetDesktopFolderPath(std::wstring& path)
{
    WCHAR buf[MAX_PATH] = {};
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY,
                                   nullptr, 0, buf)))
        { path = buf; return true; }
    return false;
}

static bool GetPathViaShellWindows(HWND topLevel, std::wstring& path)
{
    ScopedCoInit comInit;
    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_IShellWindows, (void**)&pSW)) || !pSW)
        return false;
    long count = 0; pSW->get_Count(&count);
    bool found = false;
    for (long i = 0; i < count && !found; ++i)
    {
        VARIANT vi = {}; vi.vt = VT_I4; vi.lVal = i;
        IDispatch* pDisp = nullptr;
        if (FAILED(pSW->Item(vi, &pDisp)) || !pDisp) continue;
        IWebBrowserApp* pWBA = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowserApp,(void**)&pWBA)) && pWBA)
        {
            HWND hwndItem = nullptr;
            pWBA->get_HWND((SHANDLE_PTR*)&hwndItem);
            if (hwndItem == topLevel)
            {
                IServiceProvider* pSP = nullptr;
                if (SUCCEEDED(pWBA->QueryInterface(IID_IServiceProvider,(void**)&pSP)) && pSP)
                {
                    IShellBrowser* pSB = nullptr;
                    if (SUCCEEDED(pSP->QueryService(SID_STopLevelBrowser,IID_IShellBrowser,(void**)&pSB)) && pSB)
                    {
                        IShellView* pSV = nullptr;
                        if (SUCCEEDED(pSB->QueryActiveShellView(&pSV)) && pSV)
                        {
                            IFolderView* pFV = nullptr;
                            if (SUCCEEDED(pSV->QueryInterface(IID_IFolderView,(void**)&pFV)) && pFV)
                            {
                                IPersistFolder2* pPF = nullptr;
                                if (SUCCEEDED(pFV->GetFolder(IID_IPersistFolder2,(void**)&pPF)) && pPF)
                                {
                                    LPITEMIDLIST pidl = nullptr;
                                    if (SUCCEEDED(pPF->GetCurFolder(&pidl)) && pidl)
                                    {
                                        WCHAR szPath[MAX_PATH] = {};
                                        SHGetPathFromIDListW(pidl, szPath);
                                        path = szPath; found = true;
                                        CoTaskMemFree(pidl);
                                    }
                                    pPF->Release();
                                }
                                pFV->Release();
                            }
                            pSV->Release();
                        }
                        pSB->Release();
                    }
                    pSP->Release();
                }
            }
            pWBA->Release();
        }
        pDisp->Release();
    }
    pSW->Release();
    return found;
}

static bool GetPathViaFileDialog(HWND topLevel, std::wstring& path)
{
    ScopedCoInit comInit;
    IRunningObjectTable* pROT = nullptr;
    if (FAILED(GetRunningObjectTable(0, &pROT)) || !pROT) return false;
    IEnumMoniker* pEnum = nullptr;
    bool found = false;
    if (SUCCEEDED(pROT->EnumRunning(&pEnum)) && pEnum)
    {
        IMoniker* pMk = nullptr;
        while (!found && pEnum->Next(1, &pMk, nullptr) == S_OK)
        {
            IUnknown* pUnk = nullptr;
            if (SUCCEEDED(pROT->GetObject(pMk, &pUnk)) && pUnk)
            {
                IFileDialog* pFD = nullptr;
                if (SUCCEEDED(pUnk->QueryInterface(IID_IFileDialog,(void**)&pFD)) && pFD)
                {
                    IOleWindow* pOW = nullptr;
                    if (SUCCEEDED(pFD->QueryInterface(IID_IOleWindow,(void**)&pOW)) && pOW)
                    {
                        HWND dlgHwnd = nullptr; pOW->GetWindow(&dlgHwnd);
                        if (dlgHwnd==topLevel ||
                            GetAncestor(dlgHwnd,GA_ROOT)==topLevel ||
                            GetAncestor(topLevel,GA_ROOT)==dlgHwnd)
                        {
                            IShellItem* pFolder = nullptr;
                            if (SUCCEEDED(pFD->GetFolder(&pFolder)) && pFolder)
                            {
                                PWSTR pszPath = nullptr;
                                if (SUCCEEDED(pFolder->GetDisplayName(
                                        SIGDN_FILESYSPATH,&pszPath)) && pszPath)
                                    { path=pszPath; CoTaskMemFree(pszPath); }
                                found = true; pFolder->Release();
                            }
                        }
                        pOW->Release();
                    }
                    pFD->Release();
                }
                pUnk->Release();
            }
            pMk->Release();
        }
        pEnum->Release();
    }
    pROT->Release();
    return found;
}

static bool GetPathViaDefViewChild(HWND topLevel, std::wstring& path)
{
    struct S { HWND defView; };
    S s = {};
    struct CB {
        static BOOL CALLBACK Enum(HWND hw, LPARAM lp) {
            WCHAR cls[64]={};GetClassNameW(hw,cls,64);
            if (!wcscmp(cls,L"SHELLDLL_DefView")){((S*)lp)->defView=hw;return FALSE;}
            return TRUE;
        }
    };
    EnumChildWindows(topLevel, CB::Enum, (LPARAM)&s);
    if (!s.defView) return false;
    HWND defViewParent = GetParent(s.defView);
    if (!defViewParent) return false;
    IShellBrowser* pSB = (IShellBrowser*)SendMessageW(defViewParent, WM_USER+7, 0, 0);
    if (!pSB) return false;
    IShellView* pSV = nullptr;
    if (FAILED(pSB->QueryActiveShellView(&pSV)) || !pSV) return false;
    bool found = false;
    IFolderView* pFV = nullptr;
    if (SUCCEEDED(pSV->QueryInterface(IID_IFolderView,(void**)&pFV)) && pFV)
    {
        IPersistFolder2* pPF = nullptr;
        if (SUCCEEDED(pFV->GetFolder(IID_IPersistFolder2,(void**)&pPF)) && pPF)
        {
            LPITEMIDLIST pidl = nullptr;
            if (SUCCEEDED(pPF->GetCurFolder(&pidl)) && pidl)
            {
                WCHAR szPath[MAX_PATH]={};
                SHGetPathFromIDListW(pidl,szPath);
                path=szPath; found=true; CoTaskMemFree(pidl);
            }
            pPF->Release();
        }
        pFV->Release();
    }
    pSV->Release();
    return found;
}

static bool GetCurrentFolderFromWindow(HWND hwnd, std::wstring& path)
{
    if (IsDesktopShellWindow(hwnd)) return GetDesktopFolderPath(path);
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) root = hwnd;
    if (GetPathViaShellWindows(root, path))  return true;
    if (GetPathViaFileDialog(root, path))    return true;
    if (GetPathViaDefViewChild(root, path))  return true;
    return false;
}

// ============================================================
//  CanPasteInWindow — called only at menu opening
// ============================================================
static bool CanPasteInWindow(HWND hwnd)
{
    if (!hwnd)                         return false;
    if (!IsShellViewWindow(hwnd))      return false;
    if (!HasNonFileClipboardContent()) return false;
    std::wstring path;
    if (!GetCurrentFolderFromWindow(hwnd, path)) return false;
    return !path.empty();
}

// ============================================================
//  ShellView window at cursor
// ============================================================
static HWND GetShellViewWindowAtCursor()
{
    DWORD pos = GetMessagePos();
    POINT pt  = { GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };
    HWND hwnd = WindowFromPoint(pt);
    while (hwnd)
    {
        if (IsShellViewWindow(hwnd)) return hwnd;
        HWND par = GetParent(hwnd);
        if (!par) break;
        hwnd = par;
    }
    return nullptr;
}

// ============================================================
//  Paste command ID check
// ============================================================
static bool IsPasteCmd(UINT id)
{
    return id == 26
        || id == 30979
        || (id >= 28690 && id <= 28720);
}

// ============================================================
//  Perform paste
// ============================================================
static bool PerformPaste(HWND sourceWindow)
{
    if (!IsShellViewWindow(sourceWindow))  return false;
    if (!HasNonFileClipboardContent())     return false;
    std::wstring targetPath;
    if (!GetCurrentFolderFromWindow(sourceWindow, targetPath))
        { Wh_Log(L"PerformPaste: window not recognized, hwnd=%p", sourceWindow); return false; }
    if (targetPath.empty())
        { Wh_Log(L"PerformPaste: virtual folder, skipping"); return false; }
    Wh_Log(L"PerformPaste: target='%s'", targetPath.c_str());
    std::wstring baseName = GetDefaultBaseName();
    std::wstring filePath;
    DWORD errCode = ERROR_NOT_SUPPORTED;
    bool hasImage = IsClipboardFormatAvailable(CF_BITMAP)||
                    IsClipboardFormatAvailable(CF_DIB)||
                    IsClipboardFormatAvailable(CF_DIBV5);
    bool hasText  = IsClipboardFormatAvailable(CF_UNICODETEXT);
    if (hasImage)
        { filePath=GetUniquePath(targetPath,baseName,L".png");
          errCode=SaveImageFromClipboard(filePath,targetPath); }
    else if (hasText)
        { filePath=GetUniquePath(targetPath,baseName,L".txt");
          errCode=SaveTextFromClipboard(filePath); }
    if (errCode == 0)
    {
        Wh_Log(L"PerformPaste: created '%s'", filePath.c_str());
        SHChangeNotify(SHCNE_CREATE,    SHCNF_PATH|SHCNF_FLUSH, filePath.c_str(),   nullptr);
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH|SHCNF_FLUSH, targetPath.c_str(), nullptr);
        return true;
    }
    Wh_Log(L"PerformPaste: failed, error=%u", errCode);
    if (errCode == ERROR_ACCESS_DENIED) ShowAccessDeniedMessage(sourceWindow, targetPath);
    return false;
}

// ============================================================
//  Hooks
// ============================================================

using EnableMenuItem_t = BOOL(WINAPI*)(HMENU, UINT, UINT);
EnableMenuItem_t EnableMenuItem_Orig;
BOOL WINAPI EnableMenuItem_Hook(HMENU hMenu, UINT uIDEnableItem, UINT uEnable)
{
    if (IsPasteCmd(uIDEnableItem) && (uEnable & (MF_GRAYED | MF_DISABLED)))
    {
        if (g_menuCanPaste)
            uEnable = (uEnable & ~(MF_GRAYED | MF_DISABLED)) | MF_ENABLED;
    }
    return EnableMenuItem_Orig(hMenu, uIDEnableItem, uEnable);
}

using GetMenuState_t = UINT(WINAPI*)(HMENU, UINT, UINT);
GetMenuState_t GetMenuState_Orig;
UINT WINAPI GetMenuState_Hook(HMENU hMenu, UINT uId, UINT uFlags)
{
    UINT result = GetMenuState_Orig(hMenu, uId, uFlags);
    if (IsPasteCmd(uId) && g_menuCanPaste)
        result = (result & ~(MF_GRAYED | MF_DISABLED)) | MF_ENABLED;
    return result;
}


using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
TrackPopupMenuEx_t TrackPopupMenuEx_Orig;
BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hmenu, UINT fuFlags,
                                   int x, int y, HWND hwnd, LPTPMPARAMS lptpm)
{
    g_menuCanPaste = CanPasteInWindow(hwnd);
    g_lastContextMenuShellView = g_menuCanPaste ? hwnd : nullptr;

    if (g_menuCanPaste)
    {
        int n = GetMenuItemCount(hmenu);
        for (int i = 0; i < n; ++i)
        {
            MENUITEMINFOW mii = {};
            mii.cbSize = sizeof(mii);
            mii.fMask  = MIIM_ID | MIIM_STATE;
            if (GetMenuItemInfoW(hmenu, (UINT)i, TRUE, &mii) && IsPasteCmd(mii.wID))
            {
                mii.fMask  = MIIM_STATE;
                mii.fState = MFS_ENABLED;
                SetMenuItemInfoW(hmenu, (UINT)i, TRUE, &mii);
            }
        }
    }
    BOOL result = TrackPopupMenuEx_Orig(hmenu, fuFlags, x, y, hwnd, lptpm);

    g_menuCanPaste = false;
    return result;
}

static void HandleInitMenuPopup(HWND hWnd, HMENU hPopup)
{
    HWND svWnd = nullptr;
    {
        WCHAR cls[64] = {};
        GetClassNameW(hWnd, cls, 64);
        if (!wcscmp(cls, L"SHELLDLL_DefView"))
            svWnd = hWnd;
    }
    if (!svWnd)
    {
        struct S { HWND found; };
        S s = {};
        struct CB {
            static BOOL CALLBACK Enum(HWND hw, LPARAM lp) {
                WCHAR cls[64]={};GetClassNameW(hw,cls,64);
                if (!wcscmp(cls,L"SHELLDLL_DefView")){((S*)lp)->found=hw;return FALSE;}
                return TRUE;
            }
        };
        EnumChildWindows(hWnd, CB::Enum, (LPARAM)&s);
        svWnd = s.found;
    }

    if (!svWnd) return;

    g_menuCanPaste = CanPasteInWindow(svWnd);
    g_lastContextMenuShellView = svWnd;

    int n = GetMenuItemCount(hPopup);
    for (int i = 0; i < n; ++i)
    {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask  = MIIM_ID | MIIM_STATE;
        if (GetMenuItemInfoW(hPopup, (UINT)i, TRUE, &mii) && IsPasteCmd(mii.wID))
        {
            mii.fMask  = MIIM_STATE;
            mii.fState = g_menuCanPaste ? MFS_ENABLED : (MFS_GRAYED | MFS_DISABLED);
            SetMenuItemInfoW(hPopup, (UINT)i, TRUE, &mii);
        }
    }
}

using CallWindowProcW_t = LRESULT(WINAPI*)(WNDPROC, HWND, UINT, WPARAM, LPARAM);
CallWindowProcW_t CallWindowProcW_Orig;
LRESULT WINAPI CallWindowProcW_Hook(WNDPROC lpPrevWndFunc, HWND hWnd,
                                     UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_INITMENUPOPUP)
        HandleInitMenuPopup(hWnd, (HMENU)wParam);
    else if (Msg == WM_UNINITMENUPOPUP)
        { g_menuCanPaste = false; g_lastContextMenuShellView = nullptr; }
    return CallWindowProcW_Orig(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

using DefWindowProcW_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcW_t DefWindowProcW_Orig;
LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg,
                                    WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_INITMENUPOPUP)
        HandleInitMenuPopup(hWnd, (HMENU)wParam);
    else if (Msg == WM_UNINITMENUPOPUP)
        { g_menuCanPaste = false; g_lastContextMenuShellView = nullptr; }
    return DefWindowProcW_Orig(hWnd, Msg, wParam, lParam);
}

using SendMessageW_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
SendMessageW_t SendMessageW_Orig;
LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT Msg,
                                  WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_COMMAND && IsPasteCmd(LOWORD(wParam)) &&
        HasNonFileClipboardContent())
    {
        HWND target = IsShellViewWindow(hWnd) ? hWnd :
                      (g_lastContextMenuShellView &&
                       IsShellViewWindow(g_lastContextMenuShellView)
                           ? g_lastContextMenuShellView : nullptr);
        if (target && PerformPaste(target)) return 0;
    }
    return SendMessageW_Orig(hWnd, Msg, wParam, lParam);
}

using PostMessageW_t = BOOL(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
PostMessageW_t PostMessageW_Orig;
BOOL WINAPI PostMessageW_Hook(HWND hWnd, UINT Msg,
                               WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_COMMAND && IsPasteCmd(LOWORD(wParam)) &&
        HasNonFileClipboardContent())
    {
        HWND target = IsShellViewWindow(hWnd) ? hWnd :
                      (g_lastContextMenuShellView &&
                       IsShellViewWindow(g_lastContextMenuShellView)
                           ? g_lastContextMenuShellView : nullptr);
        if (target && PerformPaste(target)) return TRUE;
    }
    return PostMessageW_Orig(hWnd, Msg, wParam, lParam);
}

using TranslateAcceleratorW_t = int(WINAPI*)(HWND, HACCEL, LPMSG);
TranslateAcceleratorW_t TranslateAcceleratorW_Orig;
int WINAPI TranslateAcceleratorW_Hook(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg)
{
    if (lpMsg && lpMsg->message == WM_KEYDOWN && lpMsg->wParam == 'V' &&
        (GetKeyState(VK_CONTROL) & 0x8000) &&
        !(GetKeyState(VK_SHIFT)  & 0x8000) &&
        !(GetKeyState(VK_MENU)   & 0x8000))
    {
        HWND target = nullptr;
        if (IsShellViewWindow(hWnd))             target = hWnd;
        else if (IsShellViewWindow(lpMsg->hwnd)) target = lpMsg->hwnd;
        if (target && CanPasteInWindow(target))
        {
            Wh_Log(L"TranslateAcceleratorW_Hook: Ctrl+V hwnd=%p", target);
            if (PerformPaste(target)) return 1;
        }
    }
    return TranslateAcceleratorW_Orig(hWnd, hAccTable, lpMsg);
}

// ============================================================
//  Init / Uninit
// ============================================================
BOOL Wh_ModInit()
{
    Wh_Log(L"PasteClipboardToExplorer: Init");
    InitializeCriticalSection(&g_gdiplusCS);
    g_gdiplusCSInit = true;

    Wh_SetFunctionHook((void*)EnableMenuItem,
                       (void*)EnableMenuItem_Hook,   (void**)&EnableMenuItem_Orig);
    Wh_SetFunctionHook((void*)GetMenuState,
                       (void*)GetMenuState_Hook,     (void**)&GetMenuState_Orig);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx,
                       (void*)TrackPopupMenuEx_Hook, (void**)&TrackPopupMenuEx_Orig);
    Wh_SetFunctionHook((void*)CallWindowProcW,
                       (void*)CallWindowProcW_Hook,  (void**)&CallWindowProcW_Orig);
    Wh_SetFunctionHook((void*)DefWindowProcW,
                       (void*)DefWindowProcW_Hook,   (void**)&DefWindowProcW_Orig);
    Wh_SetFunctionHook((void*)SendMessageW,
                       (void*)SendMessageW_Hook,     (void**)&SendMessageW_Orig);
    Wh_SetFunctionHook((void*)PostMessageW,
                       (void*)PostMessageW_Hook,     (void**)&PostMessageW_Orig);
    Wh_SetFunctionHook((void*)TranslateAcceleratorW,
                       (void*)TranslateAcceleratorW_Hook,
                       (void**)&TranslateAcceleratorW_Orig);
    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"PasteClipboardToExplorer: Uninit");
    if (g_gdiplusToken) { GdiplusShutdown(g_gdiplusToken); g_gdiplusToken = 0; }
    if (g_gdiplusCSInit) { DeleteCriticalSection(&g_gdiplusCS); g_gdiplusCSInit = false; }
}
