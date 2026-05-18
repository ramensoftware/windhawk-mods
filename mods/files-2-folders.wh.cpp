// ==WindhawkMod==
// @id              files-2-folders
// @name            Files 2 Folders
// @description     Move one or more selected files in Explorer into a subfolder (named, by extension, by name, or by date), with a workaround hotkey for other file managers
// @version         1.2
// @author          tria
// @github          https://github.com/triatomic
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lshlwapi -lshell32 -lcomctl32 -lcomdlg32 -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Files 2 Folders

Inspired by [Files 2 Folder](https://www.dcmembers.com/skwire/download/files-2-folder/)
by Jody Holmes (Skwire Empire).

When one or more items are selected in an Explorer window, adds a
**Files 2 Folder...** entry to the right-click context menu. Choosing it opens
a dialog with four options for moving the selection into a new subfolder:

1. Move the selection into a subfolder with a fixed name (e.g. `folder`).
2. Move each file into a subfolder named after the file (without extension):
   `Good.bat` -> `.\Good\Good.bat`. With a single file selected, this just
   creates one folder around that file.
3. Move each file into a subfolder named after its extension
   (e.g. all `.txt` files into `.\txt\`). With a single file selected, this
   creates one folder named after that file's extension.
4. Move the selection into a subfolder named with the current date/time,
   formatted using the date format from settings.

Folders inside the selection are skipped for modes 2 and 3 (they only make
sense for files); they are still moved for modes 1 and 4.

## Fast vs. slow mode
By default the mod uses `MoveFileExW` directly, which is essentially instant
for same-volume moves. Enable **Slow mode (shell-integrated)** in settings to
route moves through `IFileOperation` instead — that gives you the standard
Windows progress dialog, **Ctrl+Z undo**, UAC elevation prompts for protected
paths, and conflict-resolution dialogs, at the cost of significantly slower
operation on large selections.

## Workaround for other programs
A configurable global hotkey (default **Ctrl+Alt+F**) makes the mod usable
from any file manager (Total Commander, Directory Opus, Files, XYplorer,
Q-Dir, etc.):

1. Select one or more files in the other program.
2. Press **Ctrl+C** (or **Ctrl+X**) to put the file paths on the clipboard.
3. Press the hotkey. The dialog appears, the files get moved into the chosen
   subfolder under their parent directory.

Toggle **Silent** to suppress the help popup when the clipboard is empty.

## Date format
Uses the same tokens as Win32 `GetDateFormat` / `GetTimeFormat`:
`yyyy MM dd HH mm ss` etc. Example: `yyyy-MM-dd-HH-mm`.

Forbidden characters in folder names (`* : ? " < > | / \`) are replaced with
`_` automatically.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- defaultSubfolderName: "folder"
  $name: "Default subfolder name (mode 1)"
  $description: "Pre-filled name for the 'fixed name' option."
- defaultMode: 1
  $name: "Default selected mode"
  $description: "Which radio button is pre-selected (1-4)."
- theme: auto
  $name: "Dialog theme"
  $description: "Light/dark appearance of the Files 2 Folder dialog."
  $options:
  - auto: "Auto (follow Windows app theme)"
  - light: "Light"
  - dark: "Dark"
- nearCutCopy: false
  $name: "Place menu item near Cut/Copy"
  $description: "When OFF (default), the 'Files 2 Folder...' entry appears at the top of the context menu. When ON, it appears just after Cut/Copy (between Copy and Paste)."
- slowMode: false
  $name: "Slow mode (safer, with undo)"
  $description: |-
    Off (default): moves are instant — but there's no Ctrl+Z to undo, no progress bar, and no prompt if a destination needs admin rights (it just fails).
    On: moves go through the standard Windows file-operation system, so you get the familiar progress dialog, Ctrl+Z undo, the 'Replace or skip files?' prompt for conflicts, and a UAC prompt when needed. The trade-off is that it's noticeably slower, especially with hundreds of files.
- hotkeyEnabled: true
  $name: "Workaround for other programs"
  $description: "Registers a global hotkey that works in any file manager (Total Commander, Directory Opus, Files, etc.). Usage: select files in the other program, press Ctrl+C (or Ctrl+X) to copy/cut them, then press the hotkey. The mod reads the file paths from the clipboard, asks where to move them, and performs the operation. The destination is the parent folder of the first selected file. Disable this if it conflicts with another program's shortcut."
- hotkeySilent: false
  $name: "Silent"
  $description: "When ON, the hotkey does nothing if the clipboard does not currently hold 2+ file paths (no popup, no sound). Useful if you've reassigned the hotkey to a key combo you press often. When OFF (default), a help popup is shown explaining how to use the workaround."
- hotkeyChar: "F"
  $name: "Workaround hotkey: key"
  $description: "Main key (single character A-Z or 0-9)."
- hotkeyCtrl: true
  $name: "Workaround hotkey: Ctrl"
- hotkeyAlt: true
  $name: "Workaround hotkey: Alt"
- hotkeyShift: false
  $name: "Workaround hotkey: Shift"
- hotkeyWin: false
  $name: "Workaround hotkey: Win"
- dateFormat: "yyyy-MM-dd-HH-mm"
  $name: "Date format (mode 4)"
  $description: |-
    Format string for the date-named subfolder.

    Date Formats (case sensitive):
    d       Day of the month without leading zero (1-31)
    dd      Day of the month with leading zero (01-31)
    ddd     Abbreviated name for the day of the week (e.g. Mon) in the current user's language
    dddd    Full name for the day of the week (e.g. Monday) in the current user's language
    M       Month without leading zero (1-12)
    MM      Month with leading zero (01-12)
    MMM     Abbreviated month name (e.g. Jan) in the current user's language
    MMMM    Full month name (e.g. January) in the current user's language
    y       Year without century, without leading zero (0-99)
    yy      Year without century, with leading zero (00-99)
    yyyy    Year with century. For example: 2005
    gg      Period/era string for the current user's locale (blank if none)

    Time Formats (case sensitive):
    h       Hours without leading zero, 12-hour format (1-12)
    hh      Hours with leading zero, 12-hour format (01-12)
    H       Hours without leading zero, 24-hour format (0-23)
    HH      Hours with leading zero, 24-hour format (00-23)
    m       Minutes without leading zero (0-59)
    mm      Minutes with leading zero (00-59)
    s       Seconds without leading zero (0-59)
    ss      Seconds with leading zero (00-59)
    t       Single character time marker, such as A or P (depends on locale)
    tt      Multi-character time marker, such as AM or PM (depends on locale)

    You cannot use the following characters in your string:  * : ? " < > | / \
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <algorithm>

// ============================================================
//  Settings
// ============================================================
struct ModSettings {
    std::wstring defaultSubfolderName;
    std::wstring dateFormat;
    int defaultMode;
    bool slowMode;
    bool nearCutCopy;
    std::wstring theme;
    bool hotkeyEnabled;
    std::wstring hotkeyChar;
    bool hotkeyCtrl;
    bool hotkeyAlt;
    bool hotkeyShift;
    bool hotkeyWin;
    bool hotkeySilent;
} g_settings;

static void LoadSettings() {
    PCWSTR s = Wh_GetStringSetting(L"defaultSubfolderName");
    g_settings.defaultSubfolderName = (s && *s) ? s : L"folder";
    if (s) Wh_FreeStringSetting(s);

    s = Wh_GetStringSetting(L"dateFormat");
    g_settings.dateFormat = (s && *s) ? s : L"yyyy-MM-dd-HH-mm";
    if (s) Wh_FreeStringSetting(s);

    g_settings.defaultMode = (int)Wh_GetIntSetting(L"defaultMode");
    if (g_settings.defaultMode < 1 || g_settings.defaultMode > 4)
        g_settings.defaultMode = 1;

    g_settings.slowMode = Wh_GetIntSetting(L"slowMode") != 0;
    g_settings.nearCutCopy = Wh_GetIntSetting(L"nearCutCopy") != 0;

    PCWSTR th = Wh_GetStringSetting(L"theme");
    g_settings.theme = (th && *th) ? th : L"auto";
    if (th) Wh_FreeStringSetting(th);

    g_settings.hotkeyEnabled = Wh_GetIntSetting(L"hotkeyEnabled") != 0;
    PCWSTR hk = Wh_GetStringSetting(L"hotkeyChar");
    g_settings.hotkeyChar = (hk && *hk) ? hk : L"F";
    if (hk) Wh_FreeStringSetting(hk);
    g_settings.hotkeyCtrl  = Wh_GetIntSetting(L"hotkeyCtrl")  != 0;
    g_settings.hotkeyAlt   = Wh_GetIntSetting(L"hotkeyAlt")   != 0;
    g_settings.hotkeyShift = Wh_GetIntSetting(L"hotkeyShift") != 0;
    g_settings.hotkeyWin    = Wh_GetIntSetting(L"hotkeyWin")    != 0;
    g_settings.hotkeySilent = Wh_GetIntSetting(L"hotkeySilent") != 0;
}

static void RestartHotkeyThread();
void Wh_ModSettingsChanged() {
    LoadSettings();
    RestartHotkeyThread();
}

// ============================================================
//  Custom command id we inject into the shell context menu
// ============================================================
static const UINT F2F_MENU_CMD = 0xBF20;  // unlikely to clash with shell ids

// State for the currently-tracked menu. thread_local because TrackPopupMenuEx
// and the matching PostMessageW(WM_COMMAND) always run on the same UI thread,
// and the PostMessageW hook itself fires on every thread in the process —
// without thread_local, an unrelated thread's PostMessageW call could race
// with the Explorer UI thread mid-write.
static thread_local HWND  g_currentMenuHwnd        = nullptr;
static thread_local bool  g_currentMenuEligible    = false;
static thread_local std::vector<std::wstring> g_currentSelection;
static thread_local std::wstring g_currentFolder;

// ============================================================
//  Path / name helpers
// ============================================================
static std::wstring SanitizeFolderName(const std::wstring& in) {
    std::wstring out;
    out.reserve(in.size());
    for (wchar_t c : in) {
        if (c == L'*' || c == L':' || c == L'?' || c == L'"' ||
            c == L'<' || c == L'>' || c == L'|' || c == L'/' ||
            c == L'\\' || c < 32) {
            out.push_back(L'_');
        } else {
            out.push_back(c);
        }
    }
    while (!out.empty() && (out.back() == L' ' || out.back() == L'.'))
        out.pop_back();
    if (out.empty()) out = L"_";
    return out;
}

static std::wstring GetFileNameOnly(const std::wstring& full) {
    size_t slash = full.find_last_of(L"\\/");
    return (slash == std::wstring::npos) ? full : full.substr(slash + 1);
}

static std::wstring GetFileBase(const std::wstring& name) {
    size_t dot = name.find_last_of(L'.');
    if (dot == std::wstring::npos || dot == 0) return name;
    return name.substr(0, dot);
}

static std::wstring GetFileExt(const std::wstring& name) {
    size_t dot = name.find_last_of(L'.');
    if (dot == std::wstring::npos || dot == 0) return L"";
    return name.substr(dot + 1);  // without the dot
}

static bool IsDirectoryPath(const std::wstring& path) {
    DWORD a = GetFileAttributesW(path.c_str());
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY);
}

// ============================================================
//  Date format -> string. Build via GetDateFormatW + GetTimeFormatW
//  by splitting on tokens. Easiest path: feed the whole string to
//  GetDateFormatW for date tokens, then a second pass for time tokens.
//  GetDateFormatW ignores time tokens and vice versa, so we run both.
// ============================================================
static std::wstring FormatNow(const std::wstring& fmt) {
    SYSTEMTIME st;
    GetLocalTime(&st);

    // GetDateFormatW handles d, dd, ddd, dddd, M, MM, MMM, MMMM, y, yy, yyyy, gg.
    // GetTimeFormatW handles h, hh, H, HH, m, mm, s, ss, t, tt.
    // Both leave foreign tokens alone, but they also strip unknown literal
    // characters in some cases — wrapping literals in single quotes preserves
    // them. We don't try to escape; we just run both passes.
    WCHAR buf1[512] = {};
    int n = GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st,
                            fmt.c_str(), buf1, 512);
    std::wstring s = (n > 0) ? buf1 : fmt;

    WCHAR buf2[512] = {};
    n = GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, s.c_str(), buf2, 512);
    if (n > 0) s = buf2;
    return s;
}

// ============================================================
//  Get current folder path + selected items from a shell view
// ============================================================
static IShellView* GetActiveShellViewForHwnd(HWND topLevel) {
    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_IShellWindows, (void**)&pSW)) || !pSW)
        return nullptr;

    long count = 0;
    pSW->get_Count(&count);
    IShellView* pSVResult = nullptr;

    for (long i = 0; i < count && !pSVResult; ++i) {
        VARIANT vi = {};
        vi.vt = VT_I4; vi.lVal = i;
        IDispatch* pDisp = nullptr;
        if (FAILED(pSW->Item(vi, &pDisp)) || !pDisp) continue;

        IWebBrowserApp* pWBA = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowserApp, (void**)&pWBA)) && pWBA) {
            HWND hwndItem = nullptr;
            pWBA->get_HWND((SHANDLE_PTR*)&hwndItem);
            if (hwndItem == topLevel) {
                IServiceProvider* pSP = nullptr;
                if (SUCCEEDED(pWBA->QueryInterface(IID_IServiceProvider, (void**)&pSP)) && pSP) {
                    IShellBrowser* pSB = nullptr;
                    if (SUCCEEDED(pSP->QueryService(SID_STopLevelBrowser,
                                                    IID_IShellBrowser,
                                                    (void**)&pSB)) && pSB) {
                        pSB->QueryActiveShellView(&pSVResult);
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
    return pSVResult;
}

static bool GetFolderAndSelectionForHwnd(HWND hwnd,
                                         std::wstring& folderOut,
                                         std::vector<std::wstring>& itemsOut)
{
    folderOut.clear();
    itemsOut.clear();

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) root = hwnd;

    IShellView* pSV = GetActiveShellViewForHwnd(root);
    if (!pSV) return false;

    bool ok = false;
    IFolderView* pFV = nullptr;
    if (SUCCEEDED(pSV->QueryInterface(IID_IFolderView, (void**)&pFV)) && pFV) {
        IPersistFolder2* pPF = nullptr;
        if (SUCCEEDED(pFV->GetFolder(IID_IPersistFolder2, (void**)&pPF)) && pPF) {
            LPITEMIDLIST pidlFolder = nullptr;
            if (SUCCEEDED(pPF->GetCurFolder(&pidlFolder)) && pidlFolder) {
                WCHAR szFolder[MAX_PATH] = {};
                if (SHGetPathFromIDListW(pidlFolder, szFolder)) {
                    folderOut = szFolder;
                }
                CoTaskMemFree(pidlFolder);
            }
            pPF->Release();
        }

        // Selected items
        IShellFolder* pSF = nullptr;
        if (SUCCEEDED(pSV->GetItemObject(SVGIO_BACKGROUND,
                                         IID_IShellFolder,
                                         (void**)&pSF)) && pSF) {
            // Not what we want — we want selection
            pSF->Release();
        }

        IDataObject* pDO = nullptr;
        if (SUCCEEDED(pSV->GetItemObject(SVGIO_SELECTION,
                                         IID_IDataObject,
                                         (void**)&pDO)) && pDO) {
            FORMATETC fmt = { (CLIPFORMAT)RegisterClipboardFormatW(CFSTR_SHELLIDLIST),
                              nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stg = {};
            if (SUCCEEDED(pDO->GetData(&fmt, &stg))) {
                CIDA* pida = (CIDA*)GlobalLock(stg.hGlobal);
                if (pida) {
                    LPCITEMIDLIST parent = (LPCITEMIDLIST)((BYTE*)pida + pida->aoffset[0]);
                    IShellFolder* pFolder = nullptr;
                    LPITEMIDLIST pidlAbsParent = ILClone(parent);
                    if (pidlAbsParent &&
                        SUCCEEDED(SHBindToObject(nullptr, pidlAbsParent, nullptr,
                                                 IID_IShellFolder, (void**)&pFolder)) && pFolder)
                    {
                        for (UINT i = 0; i < pida->cidl; ++i) {
                            LPCITEMIDLIST child = (LPCITEMIDLIST)((BYTE*)pida + pida->aoffset[i + 1]);
                            STRRET sr = {};
                            if (SUCCEEDED(pFolder->GetDisplayNameOf(child, SHGDN_FORPARSING, &sr))) {
                                WCHAR buf[MAX_PATH] = {};
                                if (SUCCEEDED(StrRetToBufW(&sr, child, buf, MAX_PATH)) && buf[0]) {
                                    itemsOut.push_back(buf);
                                }
                            }
                        }
                        pFolder->Release();
                    }
                    if (pidlAbsParent) ILFree(pidlAbsParent);
                    GlobalUnlock(stg.hGlobal);
                }
                ReleaseStgMedium(&stg);
            }
            pDO->Release();
        }
        pFV->Release();
        ok = !folderOut.empty() && !itemsOut.empty();
    }
    pSV->Release();
    return ok;
}

// ============================================================
//  Direct Win32 move — same-volume = instant rename, no shell UI.
//  Returns number of successfully moved items.
// ============================================================
static std::wstring UniqueDest(const std::wstring& destDir,
                               const std::wstring& leaf)
{
    std::wstring base = leaf;
    std::wstring ext;
    size_t dot = leaf.find_last_of(L'.');
    if (dot != std::wstring::npos && dot != 0) {
        base = leaf.substr(0, dot);
        ext  = leaf.substr(dot);
    }
    std::wstring p = destDir + L"\\" + leaf;
    if (GetFileAttributesW(p.c_str()) == INVALID_FILE_ATTRIBUTES)
        return p;
    for (int n = 2; n < 100000; ++n) {
        p = destDir + L"\\" + base + L" (" + std::to_wstring(n) + L")" + ext;
        if (GetFileAttributesW(p.c_str()) == INVALID_FILE_ATTRIBUTES)
            return p;
    }
    return destDir + L"\\" + leaf;
}

static int MoveItemsFast(HWND owner,
                         const std::vector<std::pair<std::wstring, std::wstring>>& moves)
{
    int moved = 0;
    int failed = 0;
    std::wstring firstError;
    DWORD firstErrCode = 0;

    for (auto& m : moves) {
        const std::wstring& src = m.first;
        const std::wstring& destDir = m.second;
        std::wstring leaf = GetFileNameOnly(src);

        // Retry once if a TOCTOU race causes a collision: another process
        // could create our chosen destination filename between UniqueDest's
        // probe and our MoveFileExW call.
        bool ok = false;
        DWORD lastErr = 0;
        for (int attempt = 0; attempt < 2 && !ok; ++attempt) {
            std::wstring dst = UniqueDest(destDir, leaf);
            if (MoveFileExW(src.c_str(), dst.c_str(), MOVEFILE_COPY_ALLOWED)) {
                ok = true;
            } else {
                lastErr = GetLastError();
                if (lastErr != ERROR_ALREADY_EXISTS &&
                    lastErr != ERROR_FILE_EXISTS) break;
            }
        }
        if (ok) {
            moved++;
        } else {
            failed++;
            if (firstError.empty()) {
                firstErrCode = lastErr;
                firstError = src;
            }
        }
    }

    if (failed > 0 && owner) {
        WCHAR msg[1024];
        LPWSTR sysMsg = nullptr;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                       | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, firstErrCode,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPWSTR)&sysMsg, 0, nullptr);
        wsprintfW(msg, L"%d item(s) could not be moved.\n\nFirst error: %s\n%s",
                  failed, firstError.c_str(),
                  sysMsg ? sysMsg : L"");
        if (sysMsg) LocalFree(sysMsg);
        MessageBoxW(owner, msg, L"Files 2 Folder", MB_ICONWARNING);
    }
    return moved;
}

// ============================================================
//  Slow path — IFileOperation. Gives undo, UAC, progress UI,
//  conflict-resolution dialogs. Single PerformOperations() call.
// ============================================================
static int MoveItemsShell(HWND owner,
                          const std::vector<std::pair<std::wstring, std::wstring>>& moves)
{
    if (moves.empty()) return 0;

    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    bool comInited = SUCCEEDED(hrCo);

    IFileOperation* op = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&op))) || !op) {
        if (comInited) CoUninitialize();
        return 0;
    }
    op->SetOwnerWindow(owner);
    op->SetOperationFlags(FOF_NOCONFIRMMKDIR | FOF_NO_CONNECTED_ELEMENTS
                          | FOFX_NOCOPYHOOKS);

    int queued = 0;
    for (auto& m : moves) {
        IShellItem* pSrc = nullptr;
        if (FAILED(SHCreateItemFromParsingName(m.first.c_str(), nullptr,
                                               IID_PPV_ARGS(&pSrc))) || !pSrc)
            continue;
        IShellItem* pDst = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(m.second.c_str(), nullptr,
                                                  IID_PPV_ARGS(&pDst))) && pDst) {
            if (SUCCEEDED(op->MoveItem(pSrc, pDst, nullptr, nullptr)))
                queued++;
            pDst->Release();
        }
        pSrc->Release();
    }

    if (queued > 0) op->PerformOperations();
    op->Release();
    if (comInited) CoUninitialize();
    return queued;
}

static bool EnsureDir(const std::wstring& path) {
    if (IsDirectoryPath(path)) return true;
    return SHCreateDirectoryExW(nullptr, path.c_str(), nullptr) == ERROR_SUCCESS
        || IsDirectoryPath(path);
}

// ============================================================
//  The 4 modes
// ============================================================
enum F2FMode {
    MODE_FIXED_NAME       = 1,
    MODE_PER_FILE_NAME    = 2,
    MODE_PER_EXTENSION    = 3,
    MODE_DATE_NAMED       = 4,
};

static void DoFiles2Folder(HWND owner,
                           F2FMode mode,
                           const std::wstring& folder,
                           const std::vector<std::wstring>& items,
                           const std::wstring& fixedName,
                           const std::wstring& dateText)
{
    std::vector<std::pair<std::wstring, std::wstring>> moves;

    if (mode == MODE_FIXED_NAME || mode == MODE_DATE_NAMED) {
        std::wstring raw = (mode == MODE_FIXED_NAME) ? fixedName : dateText;
        std::wstring sub = SanitizeFolderName(raw);
        if (sub.empty() || sub == L"_") {
            MessageBoxW(owner,
                L"The subfolder name cannot be empty.",
                L"Files 2 Folder", MB_ICONWARNING);
            return;
        }
        std::wstring dest = folder + L"\\" + sub;
        if (!EnsureDir(dest)) {
            MessageBoxW(owner, L"Could not create destination folder.",
                        L"Files 2 Folder", MB_ICONERROR);
            return;
        }
        for (auto& item : items) moves.push_back({ item, dest });
    }
    else if (mode == MODE_PER_FILE_NAME) {
        for (auto& item : items) {
            if (IsDirectoryPath(item)) continue;
            std::wstring leaf = GetFileNameOnly(item);
            std::wstring base = SanitizeFolderName(GetFileBase(leaf));
            if (base.empty()) continue;
            std::wstring dest = folder + L"\\" + base;
            if (!EnsureDir(dest)) continue;
            moves.push_back({ item, dest });
        }
    }
    else if (mode == MODE_PER_EXTENSION) {
        for (auto& item : items) {
            if (IsDirectoryPath(item)) continue;
            std::wstring leaf = GetFileNameOnly(item);
            std::wstring ext  = GetFileExt(leaf);
            if (ext.empty()) ext = L"_no_ext";
            ext = SanitizeFolderName(ext);
            std::wstring dest = folder + L"\\" + ext;
            if (!EnsureDir(dest)) continue;
            moves.push_back({ item, dest });
        }
    }

    if (g_settings.slowMode) {
        MoveItemsShell(owner, moves);
    } else {
        MoveItemsFast(owner, moves);
    }

    // One notification for the source folder, plus one per unique destination.
    // (IFileOperation already notifies, but an extra UPDATEDIR is harmless.)
    SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH | SHCNF_FLUSHNOWAIT, folder.c_str(), nullptr);
    std::vector<std::wstring> uniqueDests;
    for (auto& m : moves) {
        bool seen = false;
        for (auto& d : uniqueDests) {
            if (_wcsicmp(d.c_str(), m.second.c_str()) == 0) { seen = true; break; }
        }
        if (!seen) uniqueDests.push_back(m.second);
    }
    for (auto& d : uniqueDests) {
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_PATH | SHCNF_FLUSHNOWAIT, d.c_str(), nullptr);
    }
}

// ============================================================
//  Dark-mode support
//
//  Windows exposes "AppsUseLightTheme" in the registry — 0 means dark.
//  We apply DWMWA_USE_IMMERSIVE_DARK_MODE for the title bar and a
//  custom WM_CTLCOLOR* path for the dialog body, which classic Win32
//  controls don't theme on their own.
// ============================================================
static bool IsWindowsAppDarkMode() {
    DWORD value = 1, size = sizeof(value);
    HKEY hk;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &hk) == ERROR_SUCCESS)
    {
        RegQueryValueExW(hk, L"AppsUseLightTheme", nullptr, nullptr,
                         (LPBYTE)&value, &size);
        RegCloseKey(hk);
    }
    return value == 0;
}

static bool ResolveDarkMode() {
    if (g_settings.theme == L"dark")  return true;
    if (g_settings.theme == L"light") return false;
    return IsWindowsAppDarkMode();
}

// Cached per-dialog theme state and brushes.
struct DlgTheme {
    bool dark = false;
    HBRUSH bgBrush = nullptr;
    COLORREF bgColor = 0;
    COLORREF textColor = 0;
    COLORREF editBgColor = 0;
};

static void InitDlgTheme(DlgTheme& t, bool dark) {
    t.dark = dark;
    if (dark) {
        t.bgColor    = RGB(32, 32, 32);
        t.textColor  = RGB(240, 240, 240);
        t.editBgColor = RGB(45, 45, 45);
    } else {
        t.bgColor    = GetSysColor(COLOR_BTNFACE);
        t.textColor  = GetSysColor(COLOR_WINDOWTEXT);
        t.editBgColor = GetSysColor(COLOR_WINDOW);
    }
    t.bgBrush = CreateSolidBrush(t.bgColor);
}

static void DestroyDlgTheme(DlgTheme& t) {
    if (t.bgBrush) { DeleteObject(t.bgBrush); t.bgBrush = nullptr; }
}

// DWMWA_USE_IMMERSIVE_DARK_MODE is 20 on Win11, 19 on early Win10 builds.
// Try both; the API ignores unrecognized attributes.
static void ApplyDarkTitleBar(HWND hwnd, bool dark) {
    using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
    static DwmSetWindowAttribute_t pDwmSet = []() -> DwmSetWindowAttribute_t {
        HMODULE h = LoadLibraryW(L"dwmapi.dll");
        return h ? (DwmSetWindowAttribute_t)GetProcAddress(h, "DwmSetWindowAttribute")
                 : nullptr;
    }();
    if (!pDwmSet) return;
    BOOL v = dark ? TRUE : FALSE;
    pDwmSet(hwnd, 20, &v, sizeof(v));  // DWMWA_USE_IMMERSIVE_DARK_MODE
    pDwmSet(hwnd, 19, &v, sizeof(v));  // pre-20H1 fallback
}

// Tell child controls to use dark visual style — applies to button states,
// edit borders, scrollbars. No effect on plain dialog backgrounds (we paint
// those ourselves via WM_CTLCOLOR*).
//
// To make themed BUTTON text render in light color (rather than always-black),
// we also need to enable per-window dark mode via the undocumented uxtheme
// ordinals SetPreferredAppMode (#135) + AllowDarkModeForWindow (#133). This
// is exactly what Explorer itself uses. Ordinals are stable across recent
// Win10/11 builds; if they ever change we just lose the text color in dark.
using SetWindowTheme_t = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
using AllowDarkModeForWindow_t = BOOL(WINAPI*)(HWND, BOOL);
enum PreferredAppMode { Default = 0, AllowDark = 1, ForceDark = 2, ForceLight = 3 };
using SetPreferredAppMode_t = PreferredAppMode(WINAPI*)(PreferredAppMode);
using FlushMenuThemes_t = void(WINAPI*)();

static SetWindowTheme_t GetSetWindowTheme() {
    static SetWindowTheme_t p = []() -> SetWindowTheme_t {
        HMODULE h = LoadLibraryW(L"uxtheme.dll");
        return h ? (SetWindowTheme_t)GetProcAddress(h, "SetWindowTheme") : nullptr;
    }();
    return p;
}

static AllowDarkModeForWindow_t GetAllowDarkModeForWindow() {
    static AllowDarkModeForWindow_t p = []() -> AllowDarkModeForWindow_t {
        HMODULE h = GetModuleHandleW(L"uxtheme.dll");
        if (!h) h = LoadLibraryW(L"uxtheme.dll");
        return h ? (AllowDarkModeForWindow_t)GetProcAddress(h, MAKEINTRESOURCEA(133))
                 : nullptr;
    }();
    return p;
}

static SetPreferredAppMode_t GetSetPreferredAppMode() {
    static SetPreferredAppMode_t p = []() -> SetPreferredAppMode_t {
        HMODULE h = GetModuleHandleW(L"uxtheme.dll");
        if (!h) h = LoadLibraryW(L"uxtheme.dll");
        return h ? (SetPreferredAppMode_t)GetProcAddress(h, MAKEINTRESOURCEA(135))
                 : nullptr;
    }();
    return p;
}

static void EnableProcessDarkMode() {
    static bool done = false;
    if (done) return;
    done = true;
    if (auto p = GetSetPreferredAppMode()) p(AllowDark);
}
static BOOL CALLBACK ApplyDarkChildThemeProc(HWND child, LPARAM) {
    if (auto pa = GetAllowDarkModeForWindow()) pa(child, TRUE);
    SetWindowTheme_t pp = GetSetWindowTheme();
    if (!pp) return TRUE;
    WCHAR cls[64] = {};
    GetClassNameW(child, cls, 64);
    if (!_wcsicmp(cls, L"Edit")) {
        pp(child, L"DarkMode_CFD", nullptr);
    } else {
        pp(child, L"DarkMode_Explorer", nullptr);
    }
    // Force a repaint so the new color scheme renders immediately.
    SendMessageW(child, WM_THEMECHANGED, 0, 0);
    return TRUE;
}
static void ApplyDarkChildTheme(HWND hwnd) {
    EnableProcessDarkMode();
    if (auto pa = GetAllowDarkModeForWindow()) pa(hwnd, TRUE);
    SetWindowTheme_t p = GetSetWindowTheme();
    if (!p) return;
    p(hwnd, L"DarkMode_Explorer", nullptr);
    EnumChildWindows(hwnd, ApplyDarkChildThemeProc, 0);
}

// ============================================================
//  Dialog (built via in-memory DLGTEMPLATE)
// ============================================================
struct DlgState {
    F2FMode mode;
    std::wstring fixedName;
    std::wstring dateText;
    bool ok;
    bool singleItem;
    size_t itemCount;
};

// IDs
#define IDC_RB_FIXED   1001
#define IDC_RB_PERNAME 1002
#define IDC_RB_PEREXT  1003
#define IDC_RB_DATE    1004
#define IDC_ED_FIXED   1011
#define IDC_ED_DATE    1014
#define IDC_HEADER     1020

static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {
    static DlgState* s = nullptr;
    static DlgTheme theme = {};
    switch (msg) {
    case WM_INITDIALOG: {
        s = (DlgState*)lp;
        SetWindowTextW(hDlg, L"Files 2 Folder");
        SetDlgItemTextW(hDlg, IDC_ED_FIXED, s->fixedName.c_str());
        SetDlgItemTextW(hDlg, IDC_ED_DATE, s->dateText.c_str());

        // Apply theme: title bar + child controls + cached brushes for body.
        bool dark = ResolveDarkMode();
        InitDlgTheme(theme, dark);
        ApplyDarkTitleBar(hDlg, dark);
        if (dark) ApplyDarkChildTheme(hDlg);
        if (s->singleItem) {
            SetDlgItemTextW(hDlg, IDC_HEADER,
                L"You have selected one item.  What would you like to do?");
            SetDlgItemTextW(hDlg, IDC_RB_FIXED,
                L"Move the selected item into a subfolder named:");
            SetDlgItemTextW(hDlg, IDC_RB_PERNAME,
                L"Move the file into a subfolder based on its name");
            SetDlgItemTextW(hDlg, IDC_RB_PEREXT,
                L"Move the file into a subfolder based on its file extension");
            SetDlgItemTextW(hDlg, IDC_RB_DATE,
                L"Move the selected item to a date-named subfolder:");
        } else {
            WCHAR header[128];
            wsprintfW(header,
                L"You have selected %u items.  What would you like to do?",
                (unsigned)s->itemCount);
            SetDlgItemTextW(hDlg, IDC_HEADER, header);
        }

        // Both entry points (context-menu command, WM_HOTKEY) already grant
        // this thread foreground rights, so a plain SetForegroundWindow is
        // enough — no AttachThreadInput stealing needed.
        SetForegroundWindow(hDlg);
        SetFocus(hDlg);

        int rb = IDC_RB_FIXED;
        switch (s->mode) {
            case MODE_FIXED_NAME:    rb = IDC_RB_FIXED; break;
            case MODE_PER_FILE_NAME: rb = IDC_RB_PERNAME; break;
            case MODE_PER_EXTENSION: rb = IDC_RB_PEREXT; break;
            case MODE_DATE_NAMED:    rb = IDC_RB_DATE; break;
        }
        CheckRadioButton(hDlg, IDC_RB_FIXED, IDC_RB_DATE, rb);
        return TRUE;
    }
    case WM_COMMAND: {
        WORD id = LOWORD(wp);
        if (id == IDOK) {
            if (IsDlgButtonChecked(hDlg, IDC_RB_FIXED) == BST_CHECKED)        s->mode = MODE_FIXED_NAME;
            else if (IsDlgButtonChecked(hDlg, IDC_RB_PERNAME) == BST_CHECKED) s->mode = MODE_PER_FILE_NAME;
            else if (IsDlgButtonChecked(hDlg, IDC_RB_PEREXT) == BST_CHECKED)  s->mode = MODE_PER_EXTENSION;
            else                                                               s->mode = MODE_DATE_NAMED;

            WCHAR buf[260] = {};
            GetDlgItemTextW(hDlg, IDC_ED_FIXED, buf, 260);
            s->fixedName = buf;
            GetDlgItemTextW(hDlg, IDC_ED_DATE, buf, 260);
            s->dateText = buf;
            s->ok = true;
            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        if (id == IDCANCEL) {
            s->ok = false;
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        return FALSE;
    }
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN: {
        if (!theme.dark) return FALSE;  // let system default apply
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, theme.textColor);
        SetBkColor(hdc, theme.bgColor);
        return (INT_PTR)theme.bgBrush;
    }
    case WM_CTLCOLOREDIT: {
        if (!theme.dark) return FALSE;
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, theme.textColor);
        SetBkColor(hdc, theme.editBgColor);
        // Reuse a static brush for edit background.
        static HBRUSH editBrush = nullptr;
        static COLORREF editBrushColor = 0;
        if (!editBrush || editBrushColor != theme.editBgColor) {
            if (editBrush) DeleteObject(editBrush);
            editBrush = CreateSolidBrush(theme.editBgColor);
            editBrushColor = theme.editBgColor;
        }
        return (INT_PTR)editBrush;
    }
    case WM_DESTROY:
        DestroyDlgTheme(theme);
        return FALSE;
    case WM_CLOSE:
        s->ok = false;
        EndDialog(hDlg, IDCANCEL);
        return TRUE;
    }
    return FALSE;
}

// Build a DLGTEMPLATE in memory.
// Layout (in dialog units):
//   width: 280, height: 130
//   header label at (7, 7)
//   4 radio buttons stacked, with edits next to RB1 and RB4
//   OK / Cancel at the bottom-right.
static void AppendWord(std::vector<BYTE>& v, WORD w) {
    v.push_back((BYTE)(w & 0xFF));
    v.push_back((BYTE)(w >> 8));
}
static void AppendDword(std::vector<BYTE>& v, DWORD d) {
    AppendWord(v, (WORD)(d & 0xFFFF));
    AppendWord(v, (WORD)(d >> 16));
}
static void AppendStr(std::vector<BYTE>& v, const wchar_t* s) {
    while (*s) { AppendWord(v, (WORD)*s); ++s; }
    AppendWord(v, 0);
}
static void AlignDword(std::vector<BYTE>& v) {
    while (v.size() % 4 != 0) v.push_back(0);
}

// Use classic DLGTEMPLATE (not Ex) to keep things simple.
static void AppendItemClassic(std::vector<BYTE>& v,
                              DWORD style, DWORD exStyle,
                              short x, short y, short cx, short cy,
                              WORD id, WORD ctrlClassOrdinal,
                              const wchar_t* text)
{
    AlignDword(v);
    AppendDword(v, style | WS_CHILD | WS_VISIBLE);
    AppendDword(v, exStyle);
    AppendWord(v, x); AppendWord(v, y);
    AppendWord(v, cx); AppendWord(v, cy);
    AppendWord(v, id);
    // class: ordinal form -> 0xFFFF then class ordinal
    AppendWord(v, 0xFFFF);
    AppendWord(v, ctrlClassOrdinal);
    // title
    AppendStr(v, text ? text : L"");
    // creation data
    AppendWord(v, 0);
}

static bool ShowF2FDialog(HWND owner, DlgState& state) {
    std::vector<BYTE> tpl;

    // DLGTEMPLATE
    DWORD style = DS_MODALFRAME | DS_CENTER | DS_SETFONT | WS_POPUP |
                  WS_CAPTION | WS_SYSMENU;
    AppendDword(tpl, style);
    AppendDword(tpl, 0);   // dwExtendedStyle
    // cdit (item count) — fill in after items
    size_t cditPos = tpl.size();
    AppendWord(tpl, 0);
    AppendWord(tpl, 30); AppendWord(tpl, 30);   // x, y
    AppendWord(tpl, 280); AppendWord(tpl, 110); // cx, cy
    AppendWord(tpl, 0);   // menu
    AppendWord(tpl, 0);   // class
    AppendStr(tpl, L"Files 2 Folder");          // title
    // Font (DS_SETFONT)
    AppendWord(tpl, 9);                         // pointsize
    AppendStr(tpl, L"Segoe UI");

    auto addItem = [&](DWORD st, DWORD ex, short x, short y, short cx, short cy,
                       WORD id, WORD klass, const wchar_t* txt) {
        AppendItemClassic(tpl, st, ex, x, y, cx, cy, id, klass, txt);
    };

    const WORD CLS_BUTTON = 0x0080;
    const WORD CLS_EDIT   = 0x0081;
    const WORD CLS_STATIC = 0x0082;

    WORD cdit = 0;

    // Header static
    addItem(SS_LEFT, 0, 7, 7, 266, 10, IDC_HEADER, CLS_STATIC,
            L"You have selected multiple items.  What would you like to do?");
    cdit++;

    // Row 1: RB fixed name + edit
    addItem(BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP, 0,
            7, 24, 165, 10, IDC_RB_FIXED, CLS_BUTTON,
            L"Move all selected items into a subfolder named:");
    cdit++;
    addItem(ES_AUTOHSCROLL | WS_BORDER | WS_TABSTOP, 0,
            175, 22, 98, 12, IDC_ED_FIXED, CLS_EDIT, L"");
    cdit++;

    // Row 2: RB per name
    addItem(BS_AUTORADIOBUTTON | WS_TABSTOP, 0,
            7, 40, 266, 10, IDC_RB_PERNAME, CLS_BUTTON,
            L"Move each file to an individual subfolder based on its name");
    cdit++;

    // Row 3: RB per ext
    addItem(BS_AUTORADIOBUTTON | WS_TABSTOP, 0,
            7, 54, 266, 10, IDC_RB_PEREXT, CLS_BUTTON,
            L"Move each file to a subfolder based on its file extension");
    cdit++;

    // Row 4: RB date + edit (read-only display)
    addItem(BS_AUTORADIOBUTTON | WS_TABSTOP, 0,
            7, 70, 165, 10, IDC_RB_DATE, CLS_BUTTON,
            L"Move all selected items to a date-named subfolder:");
    cdit++;
    addItem(ES_AUTOHSCROLL | ES_READONLY | WS_BORDER, 0,
            175, 68, 98, 12, IDC_ED_DATE, CLS_EDIT, L"");
    cdit++;

    // OK / Cancel
    addItem(BS_DEFPUSHBUTTON | WS_TABSTOP, 0,
            162, 92, 50, 14, IDOK, CLS_BUTTON, L"OK");
    cdit++;
    addItem(BS_PUSHBUTTON | WS_TABSTOP, 0,
            220, 92, 50, 14, IDCANCEL, CLS_BUTTON, L"Cancel");
    cdit++;

    // Patch cdit
    tpl[cditPos]     = (BYTE)(cdit & 0xFF);
    tpl[cditPos + 1] = (BYTE)(cdit >> 8);

    INT_PTR r = DialogBoxIndirectParamW(
        (HINSTANCE)GetModuleHandleW(nullptr),
        (LPCDLGTEMPLATEW)tpl.data(),
        owner, DlgProc, (LPARAM)&state);
    return (r == IDOK) && state.ok;
}

// ============================================================
//  Run the whole flow (called when our menu cmd fires)
// ============================================================
static void RunFiles2Folder(HWND owner,
                            const std::wstring& folder,
                            const std::vector<std::wstring>& items)
{
    if (folder.empty() || items.empty()) return;

    // Session memory: remember the user's last choice across dialog opens
    // (no persistence — resets on Explorer/Windhawk restart).
    static bool s_haveLastChoice = false;
    static F2FMode s_lastMode = MODE_FIXED_NAME;
    static std::wstring s_lastFixedName;

    DlgState state = {};
    state.mode = s_haveLastChoice ? s_lastMode : (F2FMode)g_settings.defaultMode;
    state.fixedName = s_haveLastChoice && !s_lastFixedName.empty()
                          ? s_lastFixedName
                          : g_settings.defaultSubfolderName;
    state.dateText = FormatNow(g_settings.dateFormat);
    state.singleItem = (items.size() == 1);
    state.itemCount = items.size();

    if (!ShowF2FDialog(owner, state)) return;

    s_haveLastChoice = true;
    s_lastMode = state.mode;
    s_lastFixedName = state.fixedName;

    DoFiles2Folder(owner, state.mode, folder, items,
                   state.fixedName, state.dateText);
}

// ============================================================
//  Hook TrackPopupMenuEx — inject menu item & detect command
// ============================================================
using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
static TrackPopupMenuEx_t TrackPopupMenuEx_Orig;

static bool IsShellViewWindow(HWND hwnd) {
    HWND w = hwnd;
    while (w) {
        WCHAR cls[64] = {};
        GetClassNameW(w, cls, 64);
        if (!_wcsicmp(cls, L"SHELLDLL_DefView")) return true;
        HWND p = GetParent(w);
        if (!p) p = GetWindow(w, GW_OWNER);
        w = p;
    }
    return false;
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hmenu, UINT fuFlags,
                                  int x, int y, HWND hwnd, LPTPMPARAMS lptpm)
{
    bool injected = false;
    g_currentMenuHwnd = hwnd;
    g_currentMenuEligible = false;
    g_currentSelection.clear();
    g_currentFolder.clear();

    if (hwnd && IsShellViewWindow(hwnd)) {
        std::wstring folder;
        std::vector<std::wstring> items;
        if (GetFolderAndSelectionForHwnd(hwnd, folder, items) && items.size() >= 1) {
            // Find the Cut/Copy block and insert just after it.
            // Explorer's shell context menu uses a small fixed range of
            // command ids for the built-in clipboard verbs:
            //   Cut = 25, Copy = 26, Paste = 27, (Paste shortcut = 28).
            // We walk all items and remember the position after the last id
            // in {Cut, Copy} we see, so we land between Copy and Paste.
            // Position depends on the nearCutCopy setting:
            //   off (default): top of menu, with a separator below.
            //   on: just after Cut/Copy. Cut/Copy ids vary by shell view —
            //       FCIDM_SHVIEW_CUT/COPY = 31001/31002 in the shell view's
            //       own menu, or 25/26 in some legacy menus.
            if (g_settings.nearCutCopy) {
                int insertPos = 0;
                int count = GetMenuItemCount(hmenu);
                for (int i = 0; i < count; ++i) {
                    UINT id = GetMenuItemID(hmenu, i);
                    if (id == 31001 || id == 31002 || id == 25 || id == 26) {
                        insertPos = i + 1;
                    }
                }
                InsertMenuW(hmenu, insertPos, MF_BYPOSITION | MF_STRING,
                            F2F_MENU_CMD, L"Files 2 &Folder...");
                InsertMenuW(hmenu, insertPos + 1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
            } else {
                InsertMenuW(hmenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
                InsertMenuW(hmenu, 0, MF_BYPOSITION | MF_STRING,
                            F2F_MENU_CMD, L"Files 2 &Folder...");
            }
            injected = true;
            g_currentMenuEligible = true;
            g_currentSelection = std::move(items);
            g_currentFolder = std::move(folder);
        }
    }

    BOOL res = TrackPopupMenuEx_Orig(hmenu, fuFlags, x, y, hwnd, lptpm);

    if (injected && (fuFlags & TPM_RETURNCMD) && (UINT)res == F2F_MENU_CMD) {
        std::wstring folder = g_currentFolder;
        std::vector<std::wstring> items = g_currentSelection;
        HWND owner = hwnd;
        g_currentMenuEligible = false;
        g_currentSelection.clear();
        g_currentFolder.clear();
        g_currentMenuHwnd = nullptr;
        RunFiles2Folder(owner, folder, items);
        return 0;
    }

    // Note: don't clear g_currentMenu* here. The user's chosen WM_COMMAND
    // arrives via the PostMessageW hook AFTER TrackPopupMenuEx returns.
    // Stale state from a previous menu is overwritten on the next
    // TrackPopupMenuEx call, so the worst case is a single pending state.

    return res;
}

// Catch the WM_COMMAND path. Shell context menus deliver commands almost
// exclusively via PostMessageW; hooking SendMessageW too is a major perf
// hit (it's the hottest call in Explorer) for marginal coverage gain.
//
// We match HWND against the menu owner we recorded so a delayed/stale
// WM_COMMAND from a different window can't trigger us.
using PostMessageW_t = BOOL(WINAPI*)(HWND, UINT, WPARAM, LPARAM);
static PostMessageW_t PostMessageW_Orig;
BOOL WINAPI PostMessageW_Hook(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp) {
    if (Msg == WM_COMMAND && LOWORD(wp) == F2F_MENU_CMD &&
        g_currentMenuEligible && hWnd == g_currentMenuHwnd)
    {
        std::wstring folder = g_currentFolder;
        std::vector<std::wstring> items = g_currentSelection;
        HWND owner = hWnd;
        g_currentMenuEligible = false;
        g_currentSelection.clear();
        g_currentFolder.clear();
        g_currentMenuHwnd = nullptr;
        RunFiles2Folder(owner, folder, items);
        return TRUE;
    }
    return PostMessageW_Orig(hWnd, Msg, wp, lp);
}

// ============================================================
//  Workaround for other programs (Total Commander, Directory
//  Opus, Files, etc.):
//  A global hotkey reads file paths from the clipboard
//  (CF_HDROP — populated when the user does Ctrl+C/Ctrl+X in
//  almost every file manager), then runs the same dialog and
//  move flow. Destination defaults to the parent of the first
//  selected file, so the moves stay in place.
//
//  The hotkey runs on a dedicated thread with its own message
//  loop and hidden window so RegisterHotKey works system-wide
//  without disturbing the host process's message pump.
// ============================================================
static HANDLE  g_hotkeyThread = nullptr;
static DWORD   g_hotkeyThreadId = 0;
static HWND    g_hotkeyWnd = nullptr;
static const UINT WM_F2F_TRIGGER  = WM_APP + 1;
static const UINT WM_F2F_SHUTDOWN = WM_APP + 2;
static const int  HOTKEY_ID = 0xF2F0;

static bool ReadClipboardFiles(std::vector<std::wstring>& out) {
    out.clear();
    // Other apps may have the clipboard open momentarily — retry a few times.
    bool opened = false;
    for (int i = 0; i < 5; ++i) {
        if (OpenClipboard(nullptr)) { opened = true; break; }
        Sleep(50);
    }
    if (!opened) return false;
    bool ok = false;
    HANDLE h = GetClipboardData(CF_HDROP);
    if (h) {
        HDROP drop = (HDROP)h;
        UINT n = DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0);
        for (UINT i = 0; i < n; ++i) {
            UINT len = DragQueryFileW(drop, i, nullptr, 0);
            if (!len) continue;
            std::wstring path(len + 1, L'\0');
            DragQueryFileW(drop, i, &path[0], len + 1);
            path.resize(len);
            if (!path.empty()) out.push_back(path);
        }
        ok = !out.empty();
    }
    CloseClipboard();
    return ok;
}

static std::wstring ParentDir(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    if (slash == std::wstring::npos) return L"";
    return path.substr(0, slash);
}

static void HandleHotkeyTriggered() {
    std::vector<std::wstring> items;
    if (!ReadClipboardFiles(items) || items.empty()) {
        if (g_settings.hotkeySilent) return;
        MessageBoxW(nullptr,
            L"Files 2 Folder workaround:\n\n"
            L"Select one or more files in your file manager and press Ctrl+C "
            L"(or Ctrl+X) first, then press the hotkey again.\n\n"
            L"The clipboard does not currently contain any file paths.",
            L"Files 2 Folder", MB_ICONINFORMATION);
        return;
    }
    std::wstring folder = ParentDir(items[0]);
    if (folder.empty()) {
        MessageBoxW(nullptr, L"Could not determine destination folder.",
                    L"Files 2 Folder", MB_ICONERROR);
        return;
    }
    RunFiles2Folder(nullptr, folder, items);
}

static UINT BuildHotkeyModifiers() {
    UINT m = 0;
    if (g_settings.hotkeyCtrl)  m |= MOD_CONTROL;
    if (g_settings.hotkeyAlt)   m |= MOD_ALT;
    if (g_settings.hotkeyShift) m |= MOD_SHIFT;
    if (g_settings.hotkeyWin)   m |= MOD_WIN;
    return m;
}

static UINT BuildHotkeyVk() {
    if (g_settings.hotkeyChar.empty()) return 'F';
    wchar_t c = towupper(g_settings.hotkeyChar[0]);
    // Only A-Z and 0-9 map cleanly to VK codes (VK_A == 'A', VK_0 == '0').
    // Anything else silently registers a wrong key — fall back to F instead.
    if (!((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9'))) {
        return 'F';
    }
    return (UINT)c;
}

static LRESULT CALLBACK HotkeyWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_HOTKEY && (int)wp == HOTKEY_ID) {
        HandleHotkeyTriggered();
        return 0;
    }
    if (msg == WM_F2F_SHUTDOWN) {
        DestroyWindow(hWnd);
        return 0;
    }
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wp, lp);
}

static DWORD WINAPI HotkeyThreadProc(LPVOID) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc   = HotkeyWndProc;
    wc.hInstance     = (HINSTANCE)GetModuleHandleW(nullptr);
    wc.lpszClassName = L"Files2FolderHotkeyWnd";
    RegisterClassW(&wc);

    g_hotkeyWnd = CreateWindowExW(0, wc.lpszClassName, L"", 0,
                                  0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                  wc.hInstance, nullptr);
    if (!g_hotkeyWnd) return 1;

    if (!RegisterHotKey(g_hotkeyWnd, HOTKEY_ID,
                        BuildHotkeyModifiers() | MOD_NOREPEAT,
                        BuildHotkeyVk()))
    {
        Wh_Log(L"Files2Folders: RegisterHotKey failed (already in use?)");
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hotkeyWnd) {
        UnregisterHotKey(g_hotkeyWnd, HOTKEY_ID);
        g_hotkeyWnd = nullptr;
    }
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

static void StartHotkeyThread() {
    if (g_hotkeyThread || !g_settings.hotkeyEnabled) return;
    g_hotkeyThread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr,
                                  0, &g_hotkeyThreadId);
}

static void StopHotkeyThread() {
    if (!g_hotkeyThread) return;
    if (g_hotkeyWnd) PostMessageW(g_hotkeyWnd, WM_F2F_SHUTDOWN, 0, 0);
    else if (g_hotkeyThreadId) PostThreadMessageW(g_hotkeyThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_hotkeyThread, 2000);
    CloseHandle(g_hotkeyThread);
    g_hotkeyThread = nullptr;
    g_hotkeyThreadId = 0;
}

// Re-register the hotkey when settings change (key/modifiers/enabled).
static void RestartHotkeyThread() {
    StopHotkeyThread();
    StartHotkeyThread();
}

// ============================================================
//  Init / Uninit
// ============================================================
BOOL Wh_ModInit() {
    Wh_Log(L"Files2Folders: Init");
    LoadSettings();

    Wh_SetFunctionHook((void*)TrackPopupMenuEx,
                       (void*)TrackPopupMenuEx_Hook,
                       (void**)&TrackPopupMenuEx_Orig);
    Wh_SetFunctionHook((void*)PostMessageW,
                       (void*)PostMessageW_Hook,
                       (void**)&PostMessageW_Orig);

    StartHotkeyThread();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Files2Folders: Uninit");
    StopHotkeyThread();
}
