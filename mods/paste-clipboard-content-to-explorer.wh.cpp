// ==WindhawkMod==
// @id              paste-clipboard-content-to-explorer
// @name            Paste Clipboard Content to Explorer
// @description     Paste text and images from clipboard as files in Explorer and in file dialogs
// @version         2.0
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
- Paste text as a file (UTF-8, UTF-8 with BOM, or UTF-16 LE with BOM)
- Optionally detect the text type (e.g. JSON, HTML, Python, SQL, Markdown, YAML)
  and use it as the file extension instead of .txt (works with UTF-8 encoding only)
- Paste images as .png
- After pasting, the new file is selected and put into rename mode,
  like New → Text Document
- Auto-naming with incrementing number on conflict
- Works in Explorer folders, on the desktop, in file dialogs
- Works with network folders (UNC paths)
- Paste menu item is grayed out in virtual folders (This PC, Network, etc.)
- Ctrl+V, context menu Paste and menubar Paste all work
- Shows system error dialog when pasting into a folder without write access

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- textEncoding: UTF-16
  $name: Text encoding
  $description: Encoding for pasted text files
  $options:
  - UTF-16: UTF-16 LE with BOM
  - UTF-8-BOM: UTF-8 with BOM
  - UTF-8: UTF-8 without BOM
- enableTextPaste: true
  $name: Enable text paste
  $description: Allow pasting text from clipboard
- enableImagePaste: true
  $name: Enable image paste
  $description: Allow pasting images from clipboard
- detectTextType: true
  $name: Detect text type and set file extension
  $description: Guesses the format or programming language of pasted text (e.g. .json, .py, .html, .sql) and uses it as the file extension. File types that would break with the selected encoding (for example scripts with a BOM, or non-ASCII .bat/.ps1 without a BOM) are saved as .txt instead.
- renameAfterPaste: true
  $name: Select and rename the new file after pasting
  $description: Selects the newly created file and enters rename mode, like New → Text Document does.
*/
// ==/WindhawkModSettings==

#include <mutex>
#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <gdiplus.h>
#include <string>
#include <windhawk_utils.h>
#include <atomic>
#include <map>
#include <mutex>
#include <vector>
#include <utility>
#include <initializer_list>

using namespace Gdiplus;

static ULONG_PTR g_gdiplusToken = 0;
static CRITICAL_SECTION g_gdiplusCS;
static bool g_gdiplusCSInit = false;
static HWND g_lastContextMenuShellView = nullptr;
static HWND g_menubarShellView = nullptr;
static bool PerformPaste(HWND sourceWindow);
enum TextEnc { kEncUtf16 = 0, kEncUtf8 = 1, kEncUtf8Bom = 2 };

// Settings
struct {
    std::atomic<int>  textEncoding{kEncUtf16};
    std::atomic<bool> enableTextPaste{true};
    std::atomic<bool> enableImagePaste{true};
    std::atomic<bool> detectTextType{true};
    std::atomic<bool> renameAfterPaste{true};
} g_settings;

// ============================================================
//  Settings
// ============================================================
void LoadSettings()
{
    WindhawkUtils::StringSetting enc =
        WindhawkUtils::StringSetting::make(L"textEncoding");
    int e = kEncUtf16;
    if (!wcscmp(enc.get(), L"UTF-8"))          e = kEncUtf8;
    else if (!wcscmp(enc.get(), L"UTF-8-BOM")) e = kEncUtf8Bom;
    g_settings.textEncoding = e;

    g_settings.enableTextPaste  = Wh_GetIntSetting(L"enableTextPaste")  != 0;
    g_settings.enableImagePaste = Wh_GetIntSetting(L"enableImagePaste") != 0;
    g_settings.detectTextType   = Wh_GetIntSetting(L"detectTextType")   != 0;
    g_settings.renameAfterPaste = Wh_GetIntSetting(L"renameAfterPaste") != 0;
}

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
                                  LOAD_LIBRARY_AS_IMAGE_RESOURCE |
                                  LOAD_LIBRARY_SEARCH_SYSTEM32);
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
    if (!g_settings.enableImagePaste) return ERROR_NOT_SUPPORTED;
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
    
    bool hasText = g_settings.enableTextPaste && IsClipboardFormatAvailable(CF_UNICODETEXT);
    bool hasImage = g_settings.enableImagePaste && (IsClipboardFormatAvailable(CF_BITMAP) ||
                    IsClipboardFormatAvailable(CF_DIB) ||
                    IsClipboardFormatAvailable(CF_DIBV5));
    
    return hasText || hasImage;
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

static bool GetFolderPathFromShellView(IShellView* pSV, std::wstring& path)
{
    bool found = false;
    IFolderView* pFV = nullptr;
    if (SUCCEEDED(pSV->QueryInterface(IID_IFolderView, (void**)&pFV)) && pFV)
    {
        IPersistFolder2* pPF = nullptr;
        if (SUCCEEDED(pFV->GetFolder(IID_IPersistFolder2, (void**)&pPF)) && pPF)
        {
            LPITEMIDLIST pidl = nullptr;
            if (SUCCEEDED(pPF->GetCurFolder(&pidl)) && pidl)
            {
                WCHAR szPath[MAX_PATH] = {};
                SHGetPathFromIDListW(pidl, szPath);
                path = szPath; found = true;   // empty path = virtual folder
                CoTaskMemFree(pidl);
            }
            pPF->Release();
        }
        pFV->Release();
    }
    return found;
}

// Shell view of the tab/dialog that actually contains hwnd (AddRef'd)
static IShellView* GetOwnShellView(HWND hwnd)
{
    HWND defView = FindShellDefViewInChain(hwnd);
    HWND parent  = defView ? GetParent(defView) : nullptr;
    if (!parent) return nullptr;
    IShellBrowser* pSB = (IShellBrowser*)SendMessageW(parent, WM_USER + 7, 0, 0);
    if (!pSB) return nullptr;
    IShellView* pSV = nullptr;
    if (FAILED(pSB->QueryActiveShellView(&pSV))) return nullptr;
    return pSV;
}

static bool GetCurrentFolderFromWindow(HWND hwnd, std::wstring& path)
{
    if (IsDesktopShellWindow(hwnd)) return GetDesktopFolderPath(path);

    // The tab's own browser first: all tabs share one top-level HWND
    {
        ScopedCoInit comInit;
        IShellView* pSV = GetOwnShellView(hwnd);
        if (pSV)
        {
            bool ok = GetFolderPathFromShellView(pSV, path);
            pSV->Release();
            if (ok) return true;
        }
    }

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) root = hwnd;
    if (GetPathViaShellWindows(root, path))  return true;
    if (GetPathViaFileDialog(root, path))    return true;
    if (GetPathViaDefViewChild(root, path))  return true;
    return false;
}

// ============================================================
//  CanPasteInWindow
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
    return id == 28698;
}
// ============================================================
//  Select the new item and enter rename mode
// ============================================================
static IShellView* GetShellViewForWindow(HWND hwnd)
{
    IShellView* pSV = nullptr;
    bool desktop = IsDesktopShellWindow(hwnd);

    // 1) Per-tab / per-dialog shell browser (WM_GETISHELLBROWSER, not AddRef'd)
    if (!desktop)
    {
        pSV = GetOwnShellView(hwnd);
        if (pSV) return pSV;
    }

    // 2) ShellWindows (desktop, or fallback)
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) root = hwnd;

    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_IShellWindows, (void**)&pSW)) || !pSW)
        return nullptr;

    IDispatch* pDisp = nullptr;
    if (desktop)
    {
        VARIANT vLoc = {}; vLoc.vt = VT_I4; vLoc.lVal = CSIDL_DESKTOP;
        VARIANT vEmpty = {};
        long lhwnd = 0;
        pSW->FindWindowSW(&vLoc, &vEmpty, SWC_DESKTOP, &lhwnd,
                          SWFO_NEEDDISPATCH, &pDisp);
    }
    else
    {
        long count = 0; pSW->get_Count(&count);
        for (long i = 0; i < count && !pDisp; ++i)
        {
            VARIANT vi = {}; vi.vt = VT_I4; vi.lVal = i;
            IDispatch* pItem = nullptr;
            if (FAILED(pSW->Item(vi, &pItem)) || !pItem) continue;
            IWebBrowserApp* pWBA = nullptr;
            if (SUCCEEDED(pItem->QueryInterface(IID_IWebBrowserApp, (void**)&pWBA)) && pWBA)
            {
                SHANDLE_PTR h = 0;
                pWBA->get_HWND(&h);
                if ((HWND)(LONG_PTR)h == root) { pDisp = pItem; pDisp->AddRef(); }
                pWBA->Release();
            }
            pItem->Release();
        }
    }
    if (pDisp)
    {
        IServiceProvider* pSP = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_IServiceProvider, (void**)&pSP)) && pSP)
        {
            IShellBrowser* pSB = nullptr;
            if (SUCCEEDED(pSP->QueryService(SID_STopLevelBrowser, IID_IShellBrowser,
                                            (void**)&pSB)) && pSB)
            {
                pSB->QueryActiveShellView(&pSV);
                pSB->Release();
            }
            pSP->Release();
        }
        pDisp->Release();
    }
    pSW->Release();
    return pSV;
}

static bool IsItemFocused(IShellView* pSV, PCUITEMID_CHILD pidl)
{
    bool same = false;
    IFolderView* pFV = nullptr;
    if (SUCCEEDED(pSV->QueryInterface(IID_IFolderView, (void**)&pFV)) && pFV)
    {
        int idx = -1;
        if (SUCCEEDED(pFV->GetFocusedItem(&idx)) && idx >= 0)
        {
            PITEMID_CHILD pidlItem = nullptr;
            if (SUCCEEDED(pFV->Item(idx, &pidlItem)) && pidlItem)
            {
                IShellFolder* psf = nullptr;
                if (SUCCEEDED(pFV->GetFolder(IID_IShellFolder, (void**)&psf)) && psf)
                {
                    HRESULT hr = psf->CompareIDs(0, pidl, pidlItem);
                    same = SUCCEEDED(hr) && (short)HRESULT_CODE(hr) == 0;
                    psf->Release();
                }
                CoTaskMemFree(pidlItem);
            }
        }
        pFV->Release();
    }
    return same;
}

// ============================================================
//  Run a callback on the thread that owns a window
//  (thread timers must be stopped from the thread that created them)
// ============================================================
using RunFromWindowThreadProc_t = void (WINAPI*)(PVOID);

struct RunFromWindowThreadParam
{
    RunFromWindowThreadProc_t proc;
    PVOID                     procParam;
};

static UINT g_runFromWindowThreadMsg = 0;

static LRESULT CALLBACK RunFromWindowThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (g_runFromWindowThreadMsg && cwp->message == g_runFromWindowThreadMsg)
        {
            auto* p = (RunFromWindowThreadParam*)cwp->lParam;
            p->proc(p->procParam);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam)
{
    if (!g_runFromWindowThreadMsg) return false;

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) return false;

    if (threadId == GetCurrentThreadId())
    {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunFromWindowThreadHookProc,
                                   nullptr, threadId);
    if (!hook) return false;

    RunFromWindowThreadParam param = { proc, procParam };
    SendMessageW(hWnd, g_runFromWindowThreadMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}

// ============================================================
//  Pending "select + rename" state
// ============================================================
// returns true when finished (rename mode entered)
static bool TrySelectAndRename(IShellView* pSV, const std::wstring& filePath)
{
    bool done = false;
    PIDLIST_ABSOLUTE pidlAbs = nullptr;
    if (SUCCEEDED(SHParseDisplayName(filePath.c_str(), nullptr, &pidlAbs, 0, nullptr)) && pidlAbs)
    {
        PCUITEMID_CHILD pidlChild = (PCUITEMID_CHILD)ILFindLastID(pidlAbs);

        // Step 1: select + focus. Only succeeds once the view knows the item.
        HRESULT hr = pSV->SelectItem(pidlChild,
            SVSI_SELECT | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE | SVSI_FOCUSED);

        // Step 2: verify, then enter rename mode exactly once.
        if (SUCCEEDED(hr) && IsItemFocused(pSV, pidlChild))
        {
            pSV->SelectItem(pidlChild,
                SVSI_SELECT | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE |
                SVSI_FOCUSED | SVSI_EDIT);
            done = true;
        }
        CoTaskMemFree(pidlAbs);
    }
    return done;
}

struct PendingSelect
{
    HWND         hwnd     = nullptr;
    std::wstring path;
    IShellView*  view     = nullptr;   // AddRef'd, belongs to the owning thread
    int          attempts = 0;
    UINT_PTR     timerId  = 0;
};
static std::map<DWORD, PendingSelect> g_pending;   // key: thread that called SetTimer
static std::mutex                     g_pendingMutex;
static std::atomic<bool>              g_unloading{false};

static const UINT kSelectRetryMs     = 60;
static const int  kSelectMaxAttempts = 40;   // ~2.4 s

// MUST run on the thread that created the timer. Only touches that thread's entry.
static void StopPendingSelect()
{
    PendingSelect entry;
    {
        std::lock_guard<std::mutex> lock(g_pendingMutex);
        auto it = g_pending.find(GetCurrentThreadId());
        if (it == g_pending.end()) return;
        entry = it->second;
        g_pending.erase(it);
    }
    if (entry.timerId) KillTimer(nullptr, entry.timerId);
    if (entry.view)    entry.view->Release();
}

static void WINAPI StopPendingSelectThunk(PVOID)
{
    StopPendingSelect();
}

static BOOL CALLBACK FirstThreadWindowProc(HWND hwnd, LPARAM lParam)
{
    *(HWND*)lParam = hwnd;
    return FALSE;
}

// Called from Wh_ModUninit (engine thread): stop every timer on its own thread.
static void CancelAllPendingSelects()
{
    g_unloading = true;

    std::vector<std::pair<DWORD, HWND>> targets;
    {
        std::lock_guard<std::mutex> lock(g_pendingMutex);
        for (auto& kv : g_pending) targets.emplace_back(kv.first, kv.second.hwnd);
    }

    for (auto& t : targets)
    {
        DWORD tid  = t.first;
        HWND  hwnd = t.second;

        // The timer belongs to the thread, not to the window: if the window
        // is gone, reach the thread through any other window it owns.
        if (!IsWindow(hwnd) || GetWindowThreadProcessId(hwnd, nullptr) != tid)
        {
            hwnd = nullptr;
            EnumThreadWindows(tid, FirstThreadWindowProc, (LPARAM)&hwnd);
        }

        // No lock is held while switching threads.
        bool stopped = hwnd && RunFromWindowThread(hwnd, StopPendingSelectThunk, nullptr);
        if (!stopped)
        {
            // Thread has no windows left: only reset the state (view leaked).
            std::lock_guard<std::mutex> lock(g_pendingMutex);
            g_pending.erase(tid);
        }
    }
}

static VOID CALLBACK PendingSelectTimerProc(HWND, UINT, UINT_PTR id, DWORD)
{
    HWND         hwnd     = nullptr;
    std::wstring path;
    IShellView*  view     = nullptr;
    int          attempts = 0;
    bool         stale    = false;
    {
        std::lock_guard<std::mutex> lock(g_pendingMutex);
        auto it = g_pending.find(GetCurrentThreadId());
        if (it == g_pending.end() || it->second.timerId != id || !it->second.view)
            stale = true;
        else
        {
            attempts = ++it->second.attempts;
            hwnd     = it->second.hwnd;
            path     = it->second.path;
            view     = it->second.view;
            view->AddRef();   // stays valid even if a nested paste stops us
        }
    }
    if (stale) { KillTimer(nullptr, id); return; }

    bool finished = !IsWindow(hwnd) ||
                    TrySelectAndRename(view, path) ||
                    attempts >= kSelectMaxAttempts;
    view->Release();

    if (finished) StopPendingSelect();
}

static void ScheduleSelectAndRename(HWND hwnd, const std::wstring& path)
{
    if (g_unloading) return;

    // The SendMessage/PostMessage hooks run on the caller's thread. The timer
    // and the window must belong to the same thread, otherwise skip.
    if (GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId()) return;

    StopPendingSelect();   // this thread's previous entry only

    IShellView* pSV = nullptr;
    {
        ScopedCoInit comInit;
        pSV = GetShellViewForWindow(hwnd);
    }
    if (!pSV) return;

    UINT_PTR timerId = SetTimer(nullptr, 0, kSelectRetryMs, PendingSelectTimerProc);
    if (!timerId) { pSV->Release(); return; }

    PendingSelect e;
    e.hwnd    = hwnd;
    e.path    = path;
    e.view    = pSV;
    e.timerId = timerId;

    std::lock_guard<std::mutex> lock(g_pendingMutex);
    g_pending[GetCurrentThreadId()] = e;
}

// ============================================================
//  Text type detection (heuristic, self-contained)
// ============================================================
struct Sig { const WCHAR* s; int w; };
struct LangDef { const WCHAR* ext; const Sig* sigs; int count; bool ci; };
#define SIGS(a) a, (int)(sizeof(a) / sizeof((a)[0]))

static const int kDetectMaxChars = 4096;
static const int kMinScore       = 5;
static const int kStrongWeight   = 6;   // signature weight that is enough on its own

// ---- case-insensitive languages (matched against lowercase copy) ----
static const Sig kSigHtml[] = {
    {L"<!doctype html",10},{L"<html",8},{L"</html>",5},{L"<head",3},{L"<body",3},{L"</body>",3},
    {L"</div>",3},{L"<div",2},{L"<script",2},{L"</script>",2},{L"<a href",2},{L"<p>",2},{L"</p>",2},
    {L"<span",1},{L"<br",1},{L"<ul",2},{L"<li",2},{L"<table",2},{L"<h1",2},{L"<h2",2},{L"<img ",2},
    {L"<input",2},{L"<button",2},{L"<meta ",3},{L"<link ",2},{L"<style",3},{L"<form",2},{L"<title",2},
    {L"class=\"",2}
};
static const Sig kSigCss[] = {
    {L"margin:",2},{L"padding:",2},{L"display:",3},{L"color:",2},{L"font-size:",3},{L"font-family:",3},
    {L"background:",2},{L"background-color:",3},{L"border:",2},{L"px;",3},{L"rem;",2},{L"@media",4},
    {L":hover",3},{L"!important",3},{L"::before",3},{L"::after",3},{L"var(--",4},{L"position:",2},
    {L"width:",2},{L"height:",2},{L"@import url",3},{L"@keyframes",5},{L"text-align:",3},
    {L"z-index:",3},{L"@font-face",5},{L":root",3},{L"rgba(",2}
};
static const Sig kSigSql[] = {
    {L"\nselect ",3},{L"select ",1},{L"select * from",6},{L" from ",1},{L" where ",1},
    {L"insert into ",5},{L"create table ",6},{L"alter table ",6},{L"drop table ",5},{L"delete from ",5},
    {L"update ",1},{L" set ",1},{L"join ",2},{L"inner join",4},{L"left join",4},{L"group by ",4},
    {L"order by ",3},{L"primary key",4},{L"varchar(",4},{L"not null",3},{L"create index",4},
    {L"create view",5},{L"values (",3},{L"values(",3},{L"count(*)",4}
};
static const Sig kSigBat[] = {
    {L"@echo off",9},{L"setlocal",4},{L"%~dp0",6},{L"goto ",2},{L"\nrem ",3},{L"\nif exist ",4},
    {L"if not exist",4},{L"errorlevel",4},{L"\npause",3},{L"echo.",3},{L"\ncd /d ",4},
    {L"start \"\"",3},{L"\nset ",1},{L"\ncall ",2},{L"\n::",3},{L" >nul",4}
};
static const Sig kSigPs1[] = {
    {L"param(",3},{L"write-host",5},{L"write-output",5},{L"get-childitem",5},{L"get-content",5},
    {L"get-process",5},{L"set-location",5},{L"set-content",5},{L"foreach-object",5},{L"where-object",5},
    {L"select-object",5},{L"new-object",5},{L"import-module",5},{L"$env:",5},{L"[cmdletbinding",6},
    {L"-erroraction",5},{L"$psscriptroot",6},{L" -eq ",3},{L" -ne ",3},{L" -match ",3},{L" -like ",3},
    {L" -and ",3},{L"$true",3},{L"$false",3},{L"$null",3},{L"$_.",3},{L"invoke-",3},{L"[string]",3},
    {L"[int]",3},{L"| out-",3}
};

// ---- case-sensitive languages ----
static const Sig kSigMd[] = {
    {L"```",8},{L"\n## ",3},{L"\n### ",3},{L"\n# ",2},{L"](http",4},{L"](",2},{L"\n- [ ] ",4},
    {L"\n- [x] ",4},{L"**",2},{L"\n> ",2},{L"![",3},{L"\n* ",1},{L"\n- ",1},{L"\n1. ",1},{L"\n---\n",1}
};
static const Sig kSigTs[] = {
    {L": string",4},{L": number",4},{L": boolean",4},{L": any",3},{L": void",3},{L"interface ",3},
    {L" as const",3},{L"<string>",2},{L"readonly ",2},{L"\ntype ",2},{L"\nexport interface ",4},
    {L"\nexport type ",4},{L"import type ",4},
    {L"function ",2},{L"const ",2},{L"=> ",2},{L"import ",1},{L"===",2},{L"console.log",3}
};
static const Sig kSigJs[] = {
    {L"console.log",4},{L"function ",2},{L"=> ",2},{L"const ",2},{L"let ",1},{L"var ",1},
    {L"require(",3},{L"module.exports",4},{L"document.",2},{L"===",3},{L"!==",3},{L"export default",4},
    {L"export const",3},{L"from '",1},{L"from \"",1},{L"async ",1},{L"await ",1},{L"window.",1},
    {L"addEventListener",4},{L"JSON.stringify",3},{L"Promise",1}
};
static const Sig kSigPy[] = {
    {L"\ndef ",3},{L"\nelif ",3},{L"__name__",4},{L"__init__",3},{L"self.",2},{L"):\n",2},
    {L"\nimport ",1},{L"\nfrom ",1},{L" import ",1},{L"print(",1},{L"lambda ",1},{L" is not None",3},
    {L"\nasync def ",3},{L"\nclass ",1},{L"    pass\n",2},{L"except ",2},{L"\ntry:\n",3}
};
static const Sig kSigCpp[] = {
    {L"std::",4},{L"#include <iostream>",7},{L"cout <<",5},{L"cin >>",4},{L"using namespace",4},
    {L"namespace ",2},{L"template<",3},{L"template <",3},{L"nullptr",2},{L"#include <vector>",4},
    {L"#include <string>",4},{L"#include <memory>",3},{L"#include <",2},{L"public:",2},{L"private:",2},
    {L"protected:",2},{L"::",1},{L"class ",1},{L"int main(",3},{L"auto ",1},{L"constexpr",3},
    {L"unique_ptr",4},{L"#define ",1}
};
static const Sig kSigC[] = {
    {L"#include <stdio.h>",6},{L"#include <stdlib.h>",5},{L"#include <string.h>",4},{L"#include <",2},
    {L"#include \"",2},{L"printf(",3},{L"int main(",4},{L"malloc(",3},{L"sizeof(",2},{L"size_t",2},
    {L"#define ",2},{L"->",1},{L"typedef ",2},{L"#ifdef",2},{L"#ifndef",2},{L"#pragma",2},
    {L"void ",1},{L"struct ",1}
};
static const Sig kSigCs[] = {
    {L"using System",6},{L"Console.WriteLine",6},{L"namespace ",1},{L"static void Main(",5},
    {L"string[] args",2},{L"public class ",2},{L"{ get; set; }",6},{L"async Task",4},{L"foreach (",2},
    {L"[HttpGet",4},{L"public static ",1},{L"private readonly ",3},{L"var ",1},{L"new List<",3},
    {L"IEnumerable<",4},{L"string.Format",3},{L"public async ",3},{L"internal ",2},{L"sealed ",2}
};
static const Sig kSigJava[] = {
    {L"public static void main",7},{L"System.out.print",6},{L"import java.",6},{L"import javax.",5},
    {L"public class ",2},{L"@Override",3},{L"implements ",3},{L"extends ",1},{L"new ArrayList<",4},
    {L"\npackage ",1},{L"private final ",3},{L"throws ",2},{L"String[] args",2},{L"public interface ",3},
    {L"@Autowired",5},{L"public void ",2},{L"List<String>",3},{L"HashMap<",3},{L"import org.",4}
};
static const Sig kSigGo[] = {
    {L"package main",6},{L"\nfunc ",3},{L"fmt.",5},{L":= ",3},{L"import (",3},{L"go func",3},
    {L"defer ",3},{L"err != nil",6},{L"chan ",2},{L"\npackage ",1},{L"func (",2},{L"make(",2},
    {L"interface{}",3},{L" struct {",2}
};
static const Sig kSigRs[] = {
    {L"fn main()",6},{L"fn ",2},{L"let mut ",5},{L"println!(",6},{L"use std::",5},{L"impl ",2},
    {L"pub fn ",4},{L"&str",3},{L"String::from",4},{L"#[derive(",5},{L"vec![",4},{L"Option<",2},
    {L"Result<",2},{L".unwrap()",3},{L"&mut ",3},{L"pub struct ",4},{L"crate::",4},{L"'static",3}
};
static const Sig kSigPhp[] = {
    {L"<?php",10},{L"<?=",4},{L"$this->",4},{L"$_GET",4},{L"$_POST",4},{L"$_SERVER",4},
    {L"public function ",3},{L"echo ",1},{L" as $",3},{L"array(",2},{L"isset(",3},{L"::class",2},
    {L"namespace App",3},{L"use App\\",3},{L"$",1}
};
static const Sig kSigRb[] = {
    {L"\nend\n",3},{L"puts ",3},{L"require '",2},{L"require \"",2},{L"attr_accessor",5},{L"attr_reader",5},
    {L" do |",4},{L"elsif ",5},{L"unless ",4},{L".each do",4},{L"def initialize",6},{L"\ndef ",2},
    {L"\nmodule ",2},{L"respond_to",3},{L"#{",3}
};
static const Sig kSigSh[] = {
    {L"\nfi\n",4},{L"; then",5},{L"then\n",3},{L"\ndone\n",3},{L"esac",3},{L"if [ ",4},{L"if [[ ",5},
    {L"$(",2},{L"${",2},{L"echo ",1},{L"sudo ",3},{L"apt-get ",3},{L"apt install",3},{L"chmod ",3},
    {L"export ",2},{L"grep ",2},{L" | ",1},{L" && ",1},{L"mkdir -p",3},{L"/dev/null",4},{L"$1",2},
    {L"curl ",2},{L"\nset -e",5},{L"; do",4},{L"\"$@\"",4},{L"2>&1",4},{L"pip install",2},
    {L"npm install",2},{L"git clone",2}
};
static const Sig kSigLua[] = {
    {L"\nlocal ",3},{L"local function ",5},{L"function ",1},{L"elseif ",4},{L" ~= ",4},{L"ipairs(",4},
    {L"pairs(",3},{L"print(",1},{L" then\n",2},{L"table.insert",5},{L"string.format",3},{L"-- ",1}
};
static const Sig kSigKt[] = {
    {L"\nfun ",4},{L" fun ",3},{L"val ",3},{L"var ",1},{L"println(",2},{L"data class ",6},
    {L"override fun ",6},{L"companion object",6},{L"when (",3},{L"when {",3},{L"import kotlin",5},
    {L"suspend fun",6},{L"?:",2},{L"lateinit ",5},{L"fun main(",5},{L"listOf(",4},{L"mutableListOf",5}
};
static const Sig kSigSwift[] = {
    {L"import SwiftUI",8},{L"import UIKit",8},{L"import Foundation",6},{L"\nfunc ",2},{L"let ",1},
    {L"var ",1},{L"guard let ",6},{L"@State",5},{L"if let ",4},{L"override func",5},{L"@objc",5},
    {L"@IBOutlet",6},{L"extension ",3},{L"some View",6},{L"private(set)",5},{L"@Published",6}
};
static const Sig kSigPl[] = {
    {L"use strict",7},{L"use warnings",6},{L"my $",4},{L"my @",5},{L"my %",5},{L"\nsub ",3},
    {L"=~",4},{L"@_",3},{L"->{",2},{L"foreach my",5},{L"__END__",4},{L"$self->",3},{L"bless ",5}
};

// Order matters on ties (earlier wins).
static const LangDef kLangs[] = {
    { L".html",  SIGS(kSigHtml),  true  },
    { L".css",   SIGS(kSigCss),   true  },
    { L".sql",   SIGS(kSigSql),   true  },
    { L".bat",   SIGS(kSigBat),   true  },
    { L".ps1",   SIGS(kSigPs1),   true  },
    { L".md",    SIGS(kSigMd),    false },
    { L".ts",    SIGS(kSigTs),    false },
    { L".js",    SIGS(kSigJs),    false },
    { L".py",    SIGS(kSigPy),    false },
    { L".cpp",   SIGS(kSigCpp),   false },
    { L".c",     SIGS(kSigC),     false },
    { L".cs",    SIGS(kSigCs),    false },
    { L".java",  SIGS(kSigJava),  false },
    { L".go",    SIGS(kSigGo),    false },
    { L".rs",    SIGS(kSigRs),    false },
    { L".php",   SIGS(kSigPhp),   false },
    { L".rb",    SIGS(kSigRb),    false },
    { L".sh",    SIGS(kSigSh),    false },
    { L".lua",   SIGS(kSigLua),   false },
    { L".kt",    SIGS(kSigKt),    false },
    { L".swift", SIGS(kSigSwift), false },
    { L".pl",    SIGS(kSigPl),    false },
};

static inline bool IsSp(WCHAR c) { return c==L' '||c==L'\t'||c==L'\r'||c==L'\n'; }
static inline bool IsKeyChar(WCHAR c)
{
    return (c>=L'a'&&c<=L'z')||(c>=L'A'&&c<=L'Z')||(c>=L'0'&&c<=L'9')||c==L'_'||c==L'-'||c==L'.';
}
static inline bool IsDigitAt(const std::wstring& s, size_t k, size_t en)
{
    return k < en && s[k] >= L'0' && s[k] <= L'9';
}

// s[0] is a '\n' sentinel, real text starts at s[1]
static bool StartsAt1(const std::wstring& h, const WCHAR* p)
{
    size_t n = wcslen(p);
    return h.size() >= 1 + n && h.compare(1, n, p) == 0;
}

static inline bool IsWordChar(WCHAR c)
{
    return c == L'_' || IsCharAlphaNumericW(c);
}

// Substring search that respects word boundaries on the edges of the signature
static bool FindSig(const std::wstring& hay, const WCHAR* sig)
{
    size_t n = wcslen(sig);
    if (n == 0) return false;
    bool needBefore = IsWordChar(sig[0]);
    bool needAfter  = IsWordChar(sig[n - 1]);
    size_t pos = 0;
    while ((pos = hay.find(sig, pos)) != std::wstring::npos)
    {
        bool ok = true;
        if (needBefore && pos > 0 && IsWordChar(hay[pos - 1])) ok = false;
        if (ok && needAfter && pos + n < hay.size() && IsWordChar(hay[pos + n])) ok = false;
        if (ok) return true;
        ++pos;
    }
    return false;
}

static int ScoreSigs(const std::wstring& hay, const Sig* sigs, int n, int* maxWeight)
{
    int sc = 0;
    *maxWeight = 0;
    for (int i = 0; i < n; ++i)
        if (FindSig(hay, sigs[i].s))
        {
            sc += sigs[i].w;
            if (sigs[i].w > *maxWeight) *maxWeight = sigs[i].w;
        }
    return sc;
}

// At least one line that ends the way code lines usually do
static bool HasCodeStructure(const std::wstring& s)
{
    size_t i = 0, len = s.size();
    while (i < len)
    {
        while (i < len && s[i] == L'\n') ++i;
        size_t st = i;
        while (i < len && s[i] != L'\n') ++i;
        size_t en = i;
        while (en > st && (s[en - 1] == L' ' || s[en - 1] == L'\t')) --en;
        if (en <= st) continue;
        WCHAR c = s[en - 1];
        if (c == L';' || c == L'{' || c == L'}' || c == L'>') return true;
        if (c == L':' && en - st >= 2 && s[en - 2] == L')') return true;
    }
    return false;
}

static const WCHAR* DetectShebang(const std::wstring& s)
{
    size_t le = s.find(L'\n', 1);
    std::wstring line = s.substr(1, (le == std::wstring::npos ? s.size() : le) - 1);
    if (!line.empty()) CharLowerBuffW(&line[0], (DWORD)line.size());
    if (line.find(L"python") != std::wstring::npos) return L".py";
    if (line.find(L"node")   != std::wstring::npos ||
        line.find(L"deno")   != std::wstring::npos ||
        line.find(L" bun")   != std::wstring::npos) return L".js";
    if (line.find(L"pwsh")   != std::wstring::npos ||
        line.find(L"powershell") != std::wstring::npos) return L".ps1";
    if (line.find(L"perl")   != std::wstring::npos) return L".pl";
    if (line.find(L"ruby")   != std::wstring::npos) return L".rb";
    if (line.find(L"php")    != std::wstring::npos) return L".php";
    return L".sh";
}

static bool LooksLikeJson(const std::wstring& s, WCHAR c0, WCHAR cN)
{
    size_t i = 2;
    while (i < s.size() && IsSp(s[i])) ++i;
    if (i >= s.size()) return false;
    WCHAR n = s[i];
    if (c0 == L'{')
    {
        if (cN != L'}') return false;
        if (n == L'}') return true;
        if (n != L'"') return false;
        size_t q = s.find(L'"', i + 1);
        if (q == std::wstring::npos) return false;
        ++q;
        while (q < s.size() && IsSp(s[q])) ++q;
        return q < s.size() && s[q] == L':';
    }
    if (c0 == L'[')
    {
        if (cN != L']') return false;
        return n==L'"' || n==L'{' || n==L'[' || n==L']' || n==L'-' ||
               (n>=L'0' && n<=L'9') ||
               s.compare(i, 4, L"true") == 0 || s.compare(i, 5, L"false") == 0 ||
               s.compare(i, 4, L"null") == 0;
    }
    return false;
}

// log / ini / toml / yaml / tsv / csv by line structure. Returns nullptr if unknown.
static const WCHAR* DetectDataFormat(const std::wstring& s)
{
    int n = 0, yamlKeys = 0, yamlItems = 0, logLines = 0, sect = 0, kv = 0;
    int commaRef = -1, tabRef = -1;
    bool commaOk = true, tabOk = true, yamlDoc = false;
    int yamlNested = 0;
    bool prevKeyOpen = false;
    size_t prevIndent = 0;
    size_t i = 0, len = s.size();

    while (i < len && n < 40)
    {
        while (i < len && s[i] == L'\n') ++i;
        if (i >= len) break;
        size_t st = i;
        while (i < len && s[i] != L'\n') ++i;
        size_t en = i;

        size_t a = st;
        while (a < en && (s[a] == L' ' || s[a] == L'\t')) ++a;
        if (a >= en) continue;

        WCHAR c = s[a];
        if (c == L'#' || c == L';') continue;                       // comment
        if (en - a >= 3 && c == L'-' && s[a+1] == L'-' && s[a+2] == L'-') { yamlDoc = true; continue; }
        ++n;

        // delimiter consistency (csv / tsv)
        int commas = 0, tabs = 0;
        for (size_t k = st; k < en; ++k)
        {
            if (s[k] == L',') ++commas;
            else if (s[k] == L'\t') ++tabs;
        }
        if (commaRef < 0) commaRef = commas; else if (commas != commaRef) commaOk = false;
        if (tabRef   < 0) tabRef   = tabs;   else if (tabs   != tabRef)   tabOk   = false;

        // log: leading timestamp (yyyy-mm-dd... or hh:mm:ss)
        size_t k = a;
        if (c == L'[') ++k;
        if (k + 6 < en && IsDigitAt(s,k,en) && IsDigitAt(s,k+1,en) && IsDigitAt(s,k+2,en) &&
            IsDigitAt(s,k+3,en) && (s[k+4] == L'-' || s[k+4] == L'/') &&
            IsDigitAt(s,k+5,en) && IsDigitAt(s,k+6,en))
            ++logLines;
        else if (k + 5 < en && IsDigitAt(s,k,en) && IsDigitAt(s,k+1,en) && s[k+2] == L':' &&
                 IsDigitAt(s,k+3,en) && IsDigitAt(s,k+4,en) && s[k+5] == L':')
            ++logLines;

        // ini / toml
        if (c == L'[' && s[en-1] == L']') ++sect;
        else { for (size_t q = a + 1; q < en; ++q) if (s[q] == L'=') { ++kv; break; } }

        // yaml
        size_t indent = a - st;
        if (prevKeyOpen && indent > prevIndent) ++yamlNested;
        prevKeyOpen = false;
        if (c == L'-' && a + 1 < en && s[a+1] == L' ') ++yamlItems;
        else
        {
            size_t q = a;
            while (q < en && IsKeyChar(s[q])) ++q;
            if (q > a && q < en && s[q] == L':' && (q + 1 == en || s[q+1] == L' '))
            {
                ++yamlKeys;
                if (q + 1 == en) { prevKeyOpen = true; prevIndent = indent; }
            }
        }
        // YAML needs a YAML-specific marker, plain "Key: value" lines are not enough
        bool yamlMarker = yamlDoc || yamlNested >= 1 || (yamlItems >= 1 && yamlKeys >= 1);
        if (yamlKeys >= 1 && yamlMarker &&
            (yamlKeys + yamlItems) >= 2 &&
            (yamlKeys + yamlItems) * 5 >= n * 4)
            return L".yaml";

        if (tabOk && tabRef >= 1) return L".tsv";
        if (commaOk && ((commaRef >= 2 && n >= 3) || (commaRef >= 1 && n >= 6)))
            return L".csv";
        }

    if (n < 2) return nullptr;

    if (logLines >= 2 && logLines * 5 >= n * 3) return L".log";

    if (sect >= 1 && kv >= 1 && (sect + kv) * 5 >= n * 4)
    {
        static const WCHAR* kToml[] = { L"\n[package]", L"\n[dependencies]", L"\n[tool.",
                                        L"\n[project]", L"\n[workspace]", L"\n[[" };
        for (const WCHAR* t : kToml)
            if (s.find(t) != std::wstring::npos) return L".toml";
        return L".ini";
    }

    if ((yamlKeys >= 2 || (yamlDoc && yamlKeys >= 1)) &&
        (yamlKeys + yamlItems) * 5 >= n * 4 && (n >= 3 || yamlDoc))
        return L".yaml";

    if (tabOk && tabRef >= 1) return L".tsv";
    if (commaOk && commaRef >= 1 && (n >= 3 || commaRef >= 2)) return L".csv";

    return nullptr;
}

static const WCHAR* DetectTextExtension(const WCHAR* text, size_t len)
{
    size_t b = 0, e = len;
    while (b < len && IsSp(text[b])) ++b;
    while (e > b && IsSp(text[e - 1])) --e;
    if (b >= e) return L".txt";
    WCHAR c0 = text[b], cN = text[e - 1];

    // Normalized head: '\n' sentinel + first N chars without '\r'
    std::wstring s;
    s.reserve(kDetectMaxChars + 2);
    s.push_back(L'\n');
    size_t lim = (b + kDetectMaxChars < len) ? b + kDetectMaxChars : len;
    for (size_t i = b; i < lim; ++i)
        if (text[i] != L'\r') s.push_back(text[i]);
    if (lim < len)   // truncated: drop the partial last line
    {
        size_t p = s.find_last_of(L'\n');
        if (p != std::wstring::npos && p > 0) s.resize(p);
    }
    s.push_back(L'\n');

    // 1) Shebang
    if (c0 == L'#' && b + 1 < len && text[b + 1] == L'!') return DetectShebang(s);

    std::wstring low = s;
    CharLowerBuffW(&low[0], (DWORD)low.size());

    // 2) Strong markup signatures
    if (c0 == L'<')
    {
        if (StartsAt1(low, L"<?php")) return L".php";
        if (StartsAt1(low, L"<?xml"))
            return low.find(L"<svg") != std::wstring::npos ? L".svg" : L".xml";
        if (StartsAt1(low, L"<svg")) return L".svg";
        if (StartsAt1(low, L"<!doctype html") || StartsAt1(low, L"<html")) return L".html";
    }

    // 3) JSON
    if ((c0 == L'{' || c0 == L'[') && LooksLikeJson(s, c0, cN)) return L".json";

    // 4) Signature scoring
    const WCHAR* bestExt = nullptr;
    int bestScore = 0;
    bool structured = HasCodeStructure(s);
    for (const LangDef& L : kLangs)
    {
        int maxW = 0;
        int sc = ScoreSigs(L.ci ? low : s, L.sigs, L.count, &maxW);
        if (!structured && maxW < kStrongWeight) continue;   // looks like plain prose
        if (sc > bestScore) { bestScore = sc; bestExt = L.ext; }
    }

    // 5) Generic <tag>...</tag> that doesn't look like HTML -> XML
    if (c0 == L'<' && cN == L'>')
    {
        if (bestExt && !wcscmp(bestExt, L".html") && bestScore >= kMinScore) return L".html";
        if (s.find(L"</") != std::wstring::npos || s.find(L"/>") != std::wstring::npos)
            return L".xml";
    }

    if (bestExt && bestScore >= kMinScore) return bestExt;

    // 6) Data formats
    const WCHAR* data = DetectDataFormat(s);
    if (data) return data;

    return L".txt";
}

static DWORD ReadClipboardText(std::wstring& out)
{
    if (!g_settings.enableTextPaste) return ERROR_NOT_SUPPORTED;
    if (!OpenClipboard(nullptr)) return GetLastError();
    DWORD errCode = ERROR_NO_DATA;
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    if (h)
    {
        const WCHAR* text = (const WCHAR*)GlobalLock(h);
        if (text)
        {
            out = text;
            GlobalUnlock(h);
            errCode = 0;
        }
    }
    CloseClipboard();
    return errCode;
}

static DWORD SaveTextToFile(const std::wstring& filePath,
                            const std::wstring& text, int enc)
{
    HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_WRITE, 0,
                               nullptr, CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return GetLastError();

    DWORD written;
    if (enc == kEncUtf8 || enc == kEncUtf8Bom)
    {
        if (enc == kEncUtf8Bom)
        {
            BYTE bom[3] = {0xEF, 0xBB, 0xBF};
            WriteFile(hFile, bom, 3, &written, nullptr);
        }
        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(),
                                          nullptr, 0, nullptr, nullptr);
        if (utf8Len > 0)
        {
            std::string buf(utf8Len, '\0');
            WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(),
                                &buf[0], utf8Len, nullptr, nullptr);
            WriteFile(hFile, buf.data(), (DWORD)buf.size(), &written, nullptr);
        }
    }
    else // UTF-16 LE with BOM
    {
        WORD bom = 0xFEFF;
        WriteFile(hFile, &bom, sizeof(bom), &written, nullptr);
        WriteFile(hFile, text.c_str(), (DWORD)(text.size() * sizeof(WCHAR)),
                  &written, nullptr);
    }
    CloseHandle(hFile);
    return 0;
}

static bool ExtIn(const WCHAR* ext, std::initializer_list<const WCHAR*> list)
{
    for (const WCHAR* e : list)
        if (!wcscmp(ext, e)) return true;
    return false;
}

static bool IsAsciiOnly(const std::wstring& t)
{
    for (WCHAR c : t) if (c > 0x7F) return false;
    return true;
}

// Keeps the detected extension only if the configured encoding doesn't break it.
static const WCHAR* ApplyEncodingRules(const WCHAR* ext, int enc, const std::wstring& text)
{
    if (!wcscmp(ext, L".txt")) return ext;
    bool ok = true;
    switch (enc)
    {
    case kEncUtf16:   // only formats that read UTF-16 with a BOM
        ok = ExtIn(ext, { L".ps1", L".html", L".tsv", L".log" });
        break;
    case kEncUtf8Bom: // a BOM breaks these
        ok = !ExtIn(ext, { L".sh", L".py", L".pl", L".rb", L".php",
                           L".json", L".java", L".bat", L".toml" });
        break;
    default:          // UTF-8 without BOM: ANSI/OEM readers break on non-ASCII
        if (ExtIn(ext, { L".ps1", L".bat" })) ok = IsAsciiOnly(text);
        break;
    }
    return ok ? ext : L".txt";
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
    bool hasImage = g_settings.enableImagePaste && (IsClipboardFormatAvailable(CF_BITMAP)||
                    IsClipboardFormatAvailable(CF_DIB)||
                    IsClipboardFormatAvailable(CF_DIBV5));
    bool hasText  = g_settings.enableTextPaste && IsClipboardFormatAvailable(CF_UNICODETEXT);
    if (hasImage)
        { filePath=GetUniquePath(targetPath,baseName,L".png");
          errCode=SaveImageFromClipboard(filePath,targetPath); }
    else if (hasText)
    {
        std::wstring text;
        errCode = ReadClipboardText(text);
        if (errCode == 0)
        {
            int enc = g_settings.textEncoding;
            const WCHAR* ext = L".txt";
            if (g_settings.detectTextType)
                ext = ApplyEncodingRules(DetectTextExtension(text.c_str(), text.size()),
                                         enc, text);
            Wh_Log(L"PerformPaste: text extension '%s'", ext);
            filePath = GetUniquePath(targetPath, baseName, ext);
            errCode  = SaveTextToFile(filePath, text, enc);
        }
    }
    if (errCode == 0)
    {
        Wh_Log(L"PerformPaste: created '%s'", filePath.c_str());
        SHChangeNotify(SHCNE_CREATE,    SHCNF_PATH|SHCNF_FLUSH, filePath.c_str(),   nullptr);
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH|SHCNF_FLUSH, targetPath.c_str(), nullptr);
        if (g_settings.renameAfterPaste)
            ScheduleSelectAndRename(sourceWindow, filePath);
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
        HWND hwnd = GetShellViewWindowAtCursor();
        if (!hwnd) hwnd = g_lastContextMenuShellView;
        if (!hwnd) hwnd = g_menubarShellView;
        if (hwnd && CanPasteInWindow(hwnd))
            uEnable = (uEnable & ~(MF_GRAYED | MF_DISABLED)) | MF_ENABLED;
    }
    return EnableMenuItem_Orig(hMenu, uIDEnableItem, uEnable);
}

using GetMenuState_t = UINT(WINAPI*)(HMENU, UINT, UINT);
GetMenuState_t GetMenuState_Orig;
UINT WINAPI GetMenuState_Hook(HMENU hMenu, UINT uId, UINT uFlags)
{
    UINT result = GetMenuState_Orig(hMenu, uId, uFlags);
    if (IsPasteCmd(uId))
    {
        HWND hwnd = GetShellViewWindowAtCursor();
        if (!hwnd) hwnd = g_lastContextMenuShellView;
        if (!hwnd) hwnd = g_menubarShellView;
        if (hwnd && CanPasteInWindow(hwnd))
            result = (result & ~(MF_GRAYED | MF_DISABLED)) | MF_ENABLED;
    }
    return result;
}

using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
TrackPopupMenuEx_t TrackPopupMenuEx_Orig;
BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hmenu, UINT fuFlags,
                                   int x, int y, HWND hwnd, LPTPMPARAMS lptpm)
{
    bool canPaste = CanPasteInWindow(hwnd);
    g_lastContextMenuShellView = canPaste ? hwnd : nullptr;

    if (canPaste)
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
    return TrackPopupMenuEx_Orig(hmenu, fuFlags, x, y, hwnd, lptpm);
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

    g_menubarShellView = svWnd;
    bool canPaste = CanPasteInWindow(svWnd);

    int n = GetMenuItemCount(hPopup);
    for (int i = 0; i < n; ++i)
    {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask  = MIIM_ID | MIIM_STATE;
        if (GetMenuItemInfoW(hPopup, (UINT)i, TRUE, &mii) && IsPasteCmd(mii.wID))
        {
            mii.fMask  = MIIM_STATE;
            mii.fState = canPaste ? MFS_ENABLED : (MFS_GRAYED | MFS_DISABLED);
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
        g_menubarShellView = nullptr;
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
        g_menubarShellView = nullptr;
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
        HWND target = nullptr;
        if (IsShellViewWindow(hWnd))
            target = hWnd;
        else if (g_menubarShellView && IsShellViewWindow(g_menubarShellView))
            target = g_menubarShellView;
        else if (g_lastContextMenuShellView &&
                 IsShellViewWindow(g_lastContextMenuShellView))
            target = g_lastContextMenuShellView;
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
        HWND target = nullptr;
        if (IsShellViewWindow(hWnd))
            target = hWnd;
        else if (g_menubarShellView && IsShellViewWindow(g_menubarShellView))
            target = g_menubarShellView;
        else if (g_lastContextMenuShellView &&
                 IsShellViewWindow(g_lastContextMenuShellView))
            target = g_lastContextMenuShellView;
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
        HWND focusWnd = GetFocus();
        if (focusWnd)
        {
            WCHAR className[256] = {};
            GetClassNameW(focusWnd, className, 256);
            
            if (!wcscmp(className, L"Edit"))
            {
                return TranslateAcceleratorW_Orig(hWnd, hAccTable, lpMsg);
            }
        }
        
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
    g_unloading = false;
    LoadSettings();

    InitializeCriticalSection(&g_gdiplusCS);
    g_gdiplusCSInit = true;

    g_runFromWindowThreadMsg = RegisterWindowMessageW(
        L"Windhawk_RunFromWindowThread_paste-clipboard-content-to-explorer");

    WindhawkUtils::SetFunctionHook(EnableMenuItem,        EnableMenuItem_Hook,        &EnableMenuItem_Orig);
    WindhawkUtils::SetFunctionHook(GetMenuState,          GetMenuState_Hook,          &GetMenuState_Orig);
    WindhawkUtils::SetFunctionHook(TrackPopupMenuEx,      TrackPopupMenuEx_Hook,      &TrackPopupMenuEx_Orig);
    WindhawkUtils::SetFunctionHook(CallWindowProcW,       CallWindowProcW_Hook,       &CallWindowProcW_Orig);
    WindhawkUtils::SetFunctionHook(DefWindowProcW,        DefWindowProcW_Hook,        &DefWindowProcW_Orig);
    WindhawkUtils::SetFunctionHook(SendMessageW,          SendMessageW_Hook,          &SendMessageW_Orig);
    WindhawkUtils::SetFunctionHook(PostMessageW,          PostMessageW_Hook,          &PostMessageW_Orig);
    WindhawkUtils::SetFunctionHook(TranslateAcceleratorW, TranslateAcceleratorW_Hook, &TranslateAcceleratorW_Orig);
    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"PasteClipboardToExplorer: Uninit");
    CancelAllPendingSelects();   // each timer is stopped on its own thread
    if (g_gdiplusToken) { GdiplusShutdown(g_gdiplusToken); g_gdiplusToken = 0; }
    if (g_gdiplusCSInit) { DeleteCriticalSection(&g_gdiplusCS); g_gdiplusCSInit = false; }
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"PasteClipboardToExplorer: Settings changed");
    LoadSettings();
}
