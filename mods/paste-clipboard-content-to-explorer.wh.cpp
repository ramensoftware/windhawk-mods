// ==WindhawkMod==
// @id              paste-clipboard-content-to-explorer
// @name            Paste Clipboard Content to Explorer
// @description     Paste text and images from clipboard as files in Explorer and in file dialogs
// @version         2.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         *
// @compilerOptions -lole32 -loleaut32 -luuid -lgdiplus -lshlwapi -lshell32 -lcomctl32 -loleacc
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Paste Clipboard Content to Explorer

Allows pasting text and images from clipboard as files in Explorer,
on the desktop and in open/save file dialogs.

## Features
- Paste text as .txt (UTF-8, UTF-8 with BOM, or UTF-16 LE with BOM)
- Paste images as .png
- Auto-naming with incrementing number on conflict
- Works in Explorer folders, on the desktop, in file dialogs
- Works with network folders (UNC paths)
- Paste menu item is grayed out in virtual folders (This PC, Network, etc.)
- Ctrl+V, context menu Paste and menubar Paste all work
- Shows system error dialog when pasting into a folder without write access
- Starts rename (edit label) immediately after paste, like New Text Document
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
- renameAfterPaste: true
  $name: Start rename after paste
  $description: Immediately start renaming the created file (like New Text Document)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <shtypes.h>
#include <oleacc.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <string>
#include <map>

using namespace Gdiplus;

static ULONG_PTR g_gdiplusToken = 0;
static CRITICAL_SECTION g_gdiplusCS;
static bool g_gdiplusCSInit = false;
static HWND g_lastContextMenuShellView = nullptr;
static HWND g_menubarShellView = nullptr;
static bool PerformPaste(HWND sourceWindow);

struct {
    std::wstring textEncoding = L"UTF-16";
    bool enableTextPaste = true;
    bool enableImagePaste = true;
    bool renameAfterPaste = true;
} g_settings;

void LoadSettings()
{
    PCWSTR textEncoding = Wh_GetStringSetting(L"textEncoding");
    g_settings.textEncoding = textEncoding ? textEncoding : L"UTF-16";
    Wh_FreeStringSetting(textEncoding);
    g_settings.enableTextPaste = Wh_GetIntSetting(L"enableTextPaste");
    g_settings.enableImagePaste = Wh_GetIntSetting(L"enableImagePaste");
    g_settings.renameAfterPaste = Wh_GetIntSetting(L"renameAfterPaste");
}

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

struct ScopedCoInit
{
    bool m_initialized = false;
    ScopedCoInit()
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr)) m_initialized = true;
    }
    ~ScopedCoInit() { if (m_initialized) CoUninitialize(); }
};

static std::wstring GetDefaultBaseName()
{
    std::wstring result;
    HMODULE hMod = LoadLibraryExW(L"explorerframe.dll", nullptr,
                                  LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (hMod)
    {
        WCHAR buf[512] = {};
        if (LoadStringW(hMod, 49922, buf, 512) > 0) result = buf;
        FreeLibrary(hMod);
    }
    if (result.empty()) result = L"Clipboard";
    return result;
}

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

static DWORD SaveTextFromClipboard(const std::wstring& filePath)
{
    if (!g_settings.enableTextPaste) return ERROR_NOT_SUPPORTED;
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
                if (g_settings.textEncoding == L"UTF-8-BOM")
                {
                    BYTE bom[3] = {0xEF, 0xBB, 0xBF};
                    WriteFile(hFile, bom, 3, &written, nullptr);
                    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
                    if (utf8Len > 0)
                    {
                        char* utf8Text = new char[utf8Len];
                        WideCharToMultiByte(CP_UTF8, 0, text, -1, utf8Text, utf8Len, nullptr, nullptr);
                        WriteFile(hFile, utf8Text, utf8Len - 1, &written, nullptr);
                        delete[] utf8Text;
                    }
                }
                else if (g_settings.textEncoding == L"UTF-8")
                {
                    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
                    if (utf8Len > 0)
                    {
                        char* utf8Text = new char[utf8Len];
                        WideCharToMultiByte(CP_UTF8, 0, text, -1, utf8Text, utf8Len, nullptr, nullptr);
                        WriteFile(hFile, utf8Text, utf8Len - 1, &written, nullptr);
                        delete[] utf8Text;
                    }
                }
                else
                {
                    WORD bom = 0xFEFF;
                    WriteFile(hFile, &bom, sizeof(bom), &written, nullptr);
                    WriteFile(hFile, text, (DWORD)(wcslen(text)*sizeof(WCHAR)), &written, nullptr);
                }
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

static bool HasNonFileClipboardContent()
{
    if (IsClipboardFormatAvailable(CF_HDROP)) return false;
    bool hasText = g_settings.enableTextPaste && IsClipboardFormatAvailable(CF_UNICODETEXT);
    bool hasImage = g_settings.enableImagePaste && (IsClipboardFormatAvailable(CF_BITMAP) ||
                    IsClipboardFormatAvailable(CF_DIB) ||
                    IsClipboardFormatAvailable(CF_DIBV5));
    return hasText || hasImage;
}

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
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, 0, buf)))
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
                                if (SUCCEEDED(pFolder->GetDisplayName(SIGDN_FILESYSPATH,&pszPath)) && pszPath)
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

static bool CanPasteInWindow(HWND hwnd)
{
    if (!hwnd)                         return false;
    if (!IsShellViewWindow(hwnd))      return false;
    if (!HasNonFileClipboardContent()) return false;
    std::wstring path;
    if (!GetCurrentFolderFromWindow(hwnd, path)) return false;
    return !path.empty();
}

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

static bool IsPasteCmd(UINT id) { return id == 28698; }

// ============================================================
//  Rename after paste - two phase: SELECT, then EDIT
//  0 = not found, 1 = selected, 2 = edit started
// ============================================================
static const DWORD MY_SVSIF_SELECT = 0x00000001;
static const DWORD MY_SVSIF_DESELECTOTHERS = 0x00000002;
static const DWORD MY_SVSIF_ENSUREVISIBLE = 0x00000004;
static const DWORD MY_SVSIF_FOCUSED = 0x00000008;
static const DWORD MY_SVSIF_SELECTIONMARK = 0x00000010;
static const DWORD MY_SVSIF_EDIT = 0x00000100;

static const UINT MY_SVSI_SELECT = 0x00000001;
static const UINT MY_SVSI_EDIT = 0x00000003;
static const UINT MY_SVSI_DESELECTOTHERS = 0x00000004;
static const UINT MY_SVSI_ENSUREVISIBLE = 0x00000008;
static const UINT MY_SVSI_FOCUSED = 0x00000010;
static const UINT MY_SVSI_SELECTIONMARK = 0x00000040;

static const GUID MY_SID_DefView = { 0x82D8A283, 0x1149, 0x4B30, { 0x97, 0x41, 0x1B, 0xEF, 0xCE, 0x6B, 0xD0, 0x02 } };

struct RenameRequest {
    HWND defView = nullptr;
    std::wstring fullPath;
    std::wstring fileName;
    int attempts = 0;
    int selectTick = -1;
    bool selected = false;
    UINT_PTR timerId = 0;
};

static CRITICAL_SECTION g_renameCS;
static bool g_renameCSInit = false;
static std::map<UINT_PTR, RenameRequest*> g_renameMap;
static volatile LONG g_timerSeq = 0x2000;

static HWND GetDefViewForRename(HWND sourceWindow)
{
    HWND defView = FindShellDefViewInChain(sourceWindow);
    if (defView) return defView;
    HWND root = GetAncestor(sourceWindow, GA_ROOT);
    if (!root) root = sourceWindow;
    struct S { HWND found; };
    S s = { nullptr };
    struct CB {
        static BOOL CALLBACK Enum(HWND hw, LPARAM lp) {
            WCHAR cls[64] = {};
            GetClassNameW(hw, cls, 64);
            if (!wcscmp(cls, L"SHELLDLL_DefView")) { ((S*)lp)->found = hw; return FALSE; }
            return TRUE;
        }
    };
    EnumChildWindows(root, CB::Enum, (LPARAM)&s);
    return s.found;
}

static HWND FindListViewDeep(HWND defView)
{
    HWND h = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
    if (h) return h;
    struct S { HWND found; };
    S s = { nullptr };
    struct CB {
        static BOOL CALLBACK Enum(HWND hw, LPARAM lp) {
            WCHAR cls[64] = {};
            GetClassNameW(hw, cls, 64);
            if (!wcscmp(cls, L"SysListView32")) { ((S*)lp)->found = hw; return FALSE; }
            return TRUE;
        }
    };
    EnumChildWindows(defView, CB::Enum, (LPARAM)&s);
    return s.found;
}

static int TryListViewFallback(HWND defView, const std::wstring& fileName, bool allowEdit)
{
    HWND hList = FindListViewDeep(defView);
    if (!hList) return 0;
    std::wstring noExt = fileName;
    LPCWSTR pExt = PathFindExtensionW(fileName.c_str());
    if (pExt && *pExt) noExt = fileName.substr(0, (size_t)(pExt - fileName.c_str()));
    int cnt = ListView_GetItemCount(hList);
    for (int i = 0; i < cnt; ++i)
    {
        WCHAR buf[512] = {};
        ListView_GetItemText(hList, i, 0, buf, sizeof(buf)/sizeof(buf[0]));
        if (!lstrcmpiW(buf, fileName.c_str()) || !lstrcmpiW(buf, noExt.c_str()))
        {
            SetFocus(hList);
            ListView_SetItemState(hList, (UINT)-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
            ListView_SetItemState(hList, i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            ListView_EnsureVisible(hList, i, FALSE);
            UpdateWindow(hList);
            if (!allowEdit) return 1;
            HWND hEdit = ListView_EditLabel(hList, i);
            Wh_Log(L"Rename ListView: i=%d hEdit=%p", i, hEdit);
            return hEdit ? 2 : 1;
        }
    }
    return 0;
}

static bool GetFolderViewFromDefView(HWND defView, IFolderView** ppOut)
{
    *ppOut = nullptr;
    IServiceProvider* pSP = nullptr;
    HRESULT hr = AccessibleObjectFromWindow(defView, OBJID_NATIVEOM, IID_IServiceProvider, (void**)&pSP);
    if (SUCCEEDED(hr) && pSP)
    {
        IFolderView* pFV = nullptr;
        hr = pSP->QueryService(MY_SID_DefView, IID_IFolderView, (void**)&pFV);
        if (FAILED(hr) || !pFV)
        {
            pFV = nullptr;
            hr = pSP->QueryService(IID_IFolderView, IID_IFolderView, (void**)&pFV);
        }
        pSP->Release();
        if (SUCCEEDED(hr) && pFV) { *ppOut = pFV; return true; }
    }
    HWND par = GetParent(defView);
    HWND tryH[2] = { defView, par };
    for (int k = 0; k < 2; ++k)
    {
        HWND h = tryH[k];
        if (!h) continue;
        LRESULT r = SendMessageW(h, WM_USER + 7, 0, 0);
        IShellBrowser* pSB = (IShellBrowser*)r;
        if (!pSB) continue;
        IShellView* pSV = nullptr;
        if (SUCCEEDED(pSB->QueryActiveShellView(&pSV)) && pSV)
        {
            IFolderView* pFV2 = nullptr;
            if (SUCCEEDED(pSV->QueryInterface(IID_IFolderView, (void**)&pFV2)) && pFV2)
            {
                pSV->Release();
                *ppOut = pFV2;
                return true;
            }
            pSV->Release();
        }
    }
    return false;
}

static int TrySelectAndEditOnce(HWND defView, const std::wstring& fullPath,
                                const std::wstring& fileName, bool allowEdit)
{
    IFolderView* pFV = nullptr;
    if (!GetFolderViewFromDefView(defView, &pFV))
        return TryListViewFallback(defView, fileName, allowEdit);

    IShellFolder* pFolder = nullptr;
    if (FAILED(pFV->GetFolder(IID_IShellFolder, (void**)&pFolder)) || !pFolder)
    {
        pFV->Release();
        return TryListViewFallback(defView, fileName, allowEdit);
    }

    LPITEMIDLIST pidlChild = nullptr;
    LPITEMIDLIST pidlFull = ILCreateFromPathW(fullPath.c_str());
    if (pidlFull)
    {
        IShellFolder* pParent = nullptr;
        LPCITEMIDLIST pRel = nullptr;
        if (SUCCEEDED(SHBindToParent(pidlFull, IID_IShellFolder, (void**)&pParent, &pRel)) && pRel)
        {
            pidlChild = ILClone(pRel);
            if (pParent) pParent->Release();
        }
        CoTaskMemFree(pidlFull);
    }
    if (!pidlChild)
    {
        if (FAILED(pFolder->ParseDisplayName(nullptr, nullptr, (LPWSTR)fileName.c_str(),
                                             nullptr, &pidlChild, nullptr)))
            pidlChild = nullptr;
    }
    if (!pidlChild)
    {
        Wh_Log(L"Rename: cannot get pidl '%s'", fileName.c_str());
        pFolder->Release(); pFV->Release();
        return 0;
    }

    int count = 0;
    pFV->ItemCount(SVGIO_ALLVIEW, &count);
    int found = -1;
    for (int i = 0; i < count; ++i)
    {
        LPITEMIDLIST pidlItem = nullptr;
        if (FAILED(pFV->Item(i, &pidlItem)) || !pidlItem) continue;
        HRESULT hrCmp = pFolder->CompareIDs(0, pidlItem, pidlChild);
        CoTaskMemFree(pidlItem);
        if (SUCCEEDED(hrCmp) && HRESULT_CODE(hrCmp) == 0) { found = i; break; }
    }

    IShellView* pSV = nullptr;
    pFV->QueryInterface(IID_IShellView, (void**)&pSV);

    int result = 0;
    if (found >= 0)
    {
        Wh_Log(L"Rename: found %d/%d '%s' allowEdit=%d", found, count, fileName.c_str(), allowEdit ? 1 : 0);
        SetFocus(defView);
        HRESULT hr1 = pFV->SelectItem(found, MY_SVSIF_SELECT | MY_SVSIF_DESELECTOTHERS |
                                      MY_SVSIF_ENSUREVISIBLE | MY_SVSIF_FOCUSED | MY_SVSIF_SELECTIONMARK);
        Wh_Log(L"Rename: select hr=0x%08X", hr1);
        if (FAILED(hr1) && pSV)
        {
            HRESULT hRs = pSV->SelectItem(pidlChild, MY_SVSI_SELECT | MY_SVSI_DESELECTOTHERS |
                                          MY_SVSI_ENSUREVISIBLE | MY_SVSI_FOCUSED | MY_SVSI_SELECTIONMARK);
            Wh_Log(L"Rename: SV select hr=0x%08X", hRs);
            if (SUCCEEDED(hRs)) hr1 = S_OK;
        }
        if (SUCCEEDED(hr1)) result = 1;

        if (result == 1 && allowEdit)
        {
            HRESULT hr2 = pFV->SelectItem(found, MY_SVSIF_SELECT | MY_SVSIF_FOCUSED |
                                          MY_SVSIF_ENSUREVISIBLE | MY_SVSIF_EDIT);
            Wh_Log(L"Rename: edit hr=0x%08X", hr2);
            if (SUCCEEDED(hr2)) result = 2;
            else if (pSV)
            {
                HRESULT hRe = pSV->SelectItem(pidlChild, MY_SVSI_EDIT | MY_SVSI_ENSUREVISIBLE | MY_SVSI_FOCUSED);
                Wh_Log(L"Rename: SV edit hr=0x%08X", hRe);
                if (SUCCEEDED(hRe)) result = 2;
            }
        }
    }
    else
    {
        Wh_Log(L"Rename: not in view yet count=%d '%s'", count, fileName.c_str());
        if (pSV)
        {
            HRESULT hRs = pSV->SelectItem(pidlChild, MY_SVSI_SELECT | MY_SVSI_DESELECTOTHERS |
                                          MY_SVSI_ENSUREVISIBLE | MY_SVSI_FOCUSED | MY_SVSI_SELECTIONMARK);
            if (SUCCEEDED(hRs))
            {
                result = 1;
                SetFocus(defView);
                if (allowEdit)
                {
                    HRESULT hRe = pSV->SelectItem(pidlChild, MY_SVSI_EDIT | MY_SVSI_ENSUREVISIBLE | MY_SVSI_FOCUSED);
                    if (SUCCEEDED(hRe)) result = 2;
                }
            }
        }
    }

    if (pidlChild) CoTaskMemFree(pidlChild);
    if (pSV) pSV->Release();
    pFolder->Release();
    pFV->Release();

    if (result != 2)
    {
        int lv = TryListViewFallback(defView, fileName, allowEdit);
        if (lv == 2) return 2;
        if (lv == 1 && result == 0) result = 1;
    }
    return result;
}

VOID CALLBACK RenameTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    (void)uMsg; (void)dwTime;
    RenameRequest* req = nullptr;
    if (g_renameCSInit)
    {
        EnterCriticalSection(&g_renameCS);
        auto it = g_renameMap.find(idEvent);
        if (it != g_renameMap.end()) req = it->second;
        LeaveCriticalSection(&g_renameCS);
    }
    if (!req || req->defView != hwnd) { KillTimer(hwnd, idEvent); return; }
    if (!IsWindow(hwnd))
    {
        KillTimer(hwnd, idEvent);
        if (g_renameCSInit) { EnterCriticalSection(&g_renameCS); g_renameMap.erase(idEvent); LeaveCriticalSection(&g_renameCS); }
        delete req;
        return;
    }
    req->attempts++;
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    bool needUninit = SUCCEEDED(hrCo);
    if (req->attempts == 1) SetFocus(hwnd);

    bool allowEdit = false;
    if (req->selected && req->selectTick >= 0 && (req->attempts - req->selectTick) >= 2)
        allowEdit = true;

    int res = TrySelectAndEditOnce(hwnd, req->fullPath, req->fileName, allowEdit);
    if (needUninit) CoUninitialize();

    if (res == 2)
    {
        Wh_Log(L"RenameTimer: success '%s' attempts=%d", req->fileName.c_str(), req->attempts);
        KillTimer(hwnd, idEvent);
        if (g_renameCSInit) { EnterCriticalSection(&g_renameCS); g_renameMap.erase(idEvent); LeaveCriticalSection(&g_renameCS); }
        delete req;
        return;
    }
    if (res == 1)
    {
        req->selected = true;
        if (req->selectTick < 0) req->selectTick = req->attempts;
    }
    if (req->attempts >= 40)
    {
        Wh_Log(L"RenameTimer: give up '%s' attempts=%d", req->fileName.c_str(), req->attempts);
        KillTimer(hwnd, idEvent);
        if (g_renameCSInit) { EnterCriticalSection(&g_renameCS); g_renameMap.erase(idEvent); LeaveCriticalSection(&g_renameCS); }
        delete req;
    }
}

static void PrewarmIconCache(const std::wstring& filePath)
{
    SHFILEINFOW sfi = {};
    SHGetFileInfoW(filePath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    SHGetFileInfoW(filePath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
}

static void BeginRenameAsync(const std::wstring& filePath, HWND sourceWindow)
{
    if (!g_settings.renameAfterPaste) return;
    HWND defView = GetDefViewForRename(sourceWindow);
    if (!defView || !IsWindow(defView)) { Wh_Log(L"BeginRename: no DefView"); return; }
    size_t pos = filePath.find_last_of(L"\\/");
    std::wstring fileName = (pos == std::wstring::npos) ? filePath : filePath.substr(pos + 1);
    if (fileName.empty()) return;

    RenameRequest* req = new (std::nothrow) RenameRequest();
    if (!req) return;
    req->defView = defView;
    req->fullPath = filePath;
    req->fileName = fileName;
    req->attempts = 0;
    req->selectTick = -1;
    req->selected = false;

    LONG seq = InterlockedIncrement(&g_timerSeq);
    if (seq == 0) seq = InterlockedIncrement(&g_timerSeq);
    UINT_PTR myId = (UINT_PTR)(UINT)seq;
    if (myId == 0) myId = 0x2001;
    UINT_PTR tid = SetTimer(defView, myId, 150, RenameTimerProc);
    if (!tid)
    {
        Wh_Log(L"BeginRename: SetTimer failed %u", GetLastError());
        delete req;
        return;
    }
    req->timerId = tid;
    if (g_renameCSInit)
    {
        EnterCriticalSection(&g_renameCS);
        g_renameMap[tid] = req;
        LeaveCriticalSection(&g_renameCS);
    }
    Wh_Log(L"BeginRename: timer %llu for '%s' hwnd=%p", (unsigned long long)tid, fileName.c_str(), defView);
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
        { filePath=GetUniquePath(targetPath,baseName,L".txt");
          errCode=SaveTextFromClipboard(filePath); }
    if (errCode == 0)
    {
        Wh_Log(L"PerformPaste: created '%s'", filePath.c_str());
        // Прогреть кэш иконки ДО нотификации, чтобы иконка была готова до EDIT
        PrewarmIconCache(filePath);
        // Только CREATE - UPDATEDIR вызывает двойное обновление и сброс иконок
        SHChangeNotify(SHCNE_CREATE, SHCNF_PATH|SHCNF_FLUSH, filePath.c_str(), nullptr);
        BeginRenameAsync(filePath, sourceWindow);
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
BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hmenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm)
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
        if (!wcscmp(cls, L"SHELLDLL_DefView")) svWnd = hWnd;
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
LRESULT WINAPI CallWindowProcW_Hook(WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_INITMENUPOPUP) HandleInitMenuPopup(hWnd, (HMENU)wParam);
    else if (Msg == WM_UNINITMENUPOPUP) g_menubarShellView = nullptr;
    return CallWindowProcW_Orig(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

using DefWindowProcW_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcW_t DefWindowProcW_Orig;
LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_INITMENUPOPUP) HandleInitMenuPopup(hWnd, (HMENU)wParam);
    else if (Msg == WM_UNINITMENUPOPUP) g_menubarShellView = nullptr;
    return DefWindowProcW_Orig(hWnd, Msg, wParam, lParam);
}

using SendMessageW_t = LRESULT(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
SendMessageW_t SendMessageW_Orig;
LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_COMMAND && IsPasteCmd(LOWORD(wParam)) && HasNonFileClipboardContent())
    {
        HWND target = nullptr;
        if (IsShellViewWindow(hWnd)) target = hWnd;
        else if (g_menubarShellView && IsShellViewWindow(g_menubarShellView)) target = g_menubarShellView;
        else if (g_lastContextMenuShellView && IsShellViewWindow(g_lastContextMenuShellView)) target = g_lastContextMenuShellView;
        if (target && PerformPaste(target)) return 0;
    }
    return SendMessageW_Orig(hWnd, Msg, wParam, lParam);
}

using PostMessageW_t = BOOL(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
PostMessageW_t PostMessageW_Orig;
BOOL WINAPI PostMessageW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    if (Msg == WM_COMMAND && IsPasteCmd(LOWORD(wParam)) && HasNonFileClipboardContent())
    {
        HWND target = nullptr;
        if (IsShellViewWindow(hWnd)) target = hWnd;
        else if (g_menubarShellView && IsShellViewWindow(g_menubarShellView)) target = g_menubarShellView;
        else if (g_lastContextMenuShellView && IsShellViewWindow(g_lastContextMenuShellView)) target = g_lastContextMenuShellView;
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
            if (!wcscmp(className, L"Edit")) return TranslateAcceleratorW_Orig(hWnd, hAccTable, lpMsg);
        }
        HWND target = nullptr;
        if (IsShellViewWindow(hWnd)) target = hWnd;
        else if (IsShellViewWindow(lpMsg->hwnd)) target = lpMsg->hwnd;
        if (target && CanPasteInWindow(target))
        {
            Wh_Log(L"TranslateAcceleratorW_Hook: Ctrl+V hwnd=%p", target);
            if (PerformPaste(target)) return 1;
        }
    }
    return TranslateAcceleratorW_Orig(hWnd, hAccTable, lpMsg);
}

BOOL Wh_ModInit()
{
    Wh_Log(L"PasteClipboardToExplorer: Init");
    LoadSettings();
    InitializeCriticalSection(&g_gdiplusCS);
    g_gdiplusCSInit = true;
    InitializeCriticalSection(&g_renameCS);
    g_renameCSInit = true;
    Wh_SetFunctionHook((void*)EnableMenuItem, (void*)EnableMenuItem_Hook, (void**)&EnableMenuItem_Orig);
    Wh_SetFunctionHook((void*)GetMenuState, (void*)GetMenuState_Hook, (void**)&GetMenuState_Orig);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&TrackPopupMenuEx_Orig);
    Wh_SetFunctionHook((void*)CallWindowProcW, (void*)CallWindowProcW_Hook, (void**)&CallWindowProcW_Orig);
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Orig);
    Wh_SetFunctionHook((void*)SendMessageW, (void*)SendMessageW_Hook, (void**)&SendMessageW_Orig);
    Wh_SetFunctionHook((void*)PostMessageW, (void*)PostMessageW_Hook, (void**)&PostMessageW_Orig);
    Wh_SetFunctionHook((void*)TranslateAcceleratorW, (void*)TranslateAcceleratorW_Hook, (void**)&TranslateAcceleratorW_Orig);
    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"PasteClipboardToExplorer: Uninit");
    if (g_renameCSInit)
    {
        EnterCriticalSection(&g_renameCS);
        for (auto &kv : g_renameMap) { KillTimer(kv.second->defView, kv.first); delete kv.second; }
        g_renameMap.clear();
        LeaveCriticalSection(&g_renameCS);
        DeleteCriticalSection(&g_renameCS);
        g_renameCSInit = false;
    }
    if (g_gdiplusToken) { GdiplusShutdown(g_gdiplusToken); g_gdiplusToken = 0; }
    if (g_gdiplusCSInit) { DeleteCriticalSection(&g_gdiplusCS); g_gdiplusCSInit = false; }
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"PasteClipboardToExplorer: Settings changed");
    LoadSettings();
}
