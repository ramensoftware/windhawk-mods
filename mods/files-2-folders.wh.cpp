// ==WindhawkMod==
// @id              files-2-folders
// @name            Files 2 Folders
// @description     Move one or more selected files in Explorer into a subfolder (named, by extension, by name, or by date), with a workaround hotkey for other file managers
// @version         1.6.1
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
**Move to a folder...** entry to the right-click context menu (using your
Windows language's own "Move to a folder" string). Choosing it opens a dialog
with four options for moving the selection into a new subfolder:

1. Move the selection into a subfolder with a fixed name (defaults to the
   localized Windows "New folder" name; e.g. `New folder`). If a folder of
   that name already exists, the new one is numbered `New folder (2)`,
   `New folder (3)`, etc. — the same way Explorer numbers duplicates.
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

This works in both Explorer folder windows and on the **desktop**. The entry
also appears in the classic menu bar's **Edit** dropdown (just after Cut/Copy)
when the menu bar is shown — there it's labelled **Move to a folder... (F2F)**,
with the `(F2F)` tag added to set it apart from Explorer's own built-in
"Move to a folder..." command in the same menu.

The menu entry and the default subfolder name use Windows' own localized
strings, so they appear in your OS language automatically.

## Silent mode
Enable **Silent mode (right-click menu)** in settings to skip the dialog
entirely: choosing **Move to a folder...** from the right-click menu then
moves the selection immediately, using the **Default selected mode** (and default
subfolder name / date format) from settings. The hotkey workaround is
unaffected and always shows the dialog.

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

Ctrl/Alt-based combos use a normal global hotkey. Enabling the **Win** modifier
switches the workaround to a low-level keyboard hook (the technique PowerToys
uses) and suppresses the OS default for that combo.

**Important:** bare `Win`+`<letter>` shortcuts (Win+F, Win+E, Win+R, Win+L,
Win+G, …) are handled by Windows itself, *below* the hook, and **cannot be
intercepted** — they won't work even with the hook (PowerToys can't remap them
either). To use the Win key, **pair it with another modifier**: enable Win
together with Ctrl, Alt, or Shift (e.g. **Win+Shift+F** or **Win+Alt+F**). If
you don't need a Win-based shortcut, leave Win off and use a Ctrl/Alt combo —
the hook watches all keystrokes while active and is slightly heavier.

## Date format
Uses the same tokens as Win32 `GetDateFormat` / `GetTimeFormat`:
`yyyy MM dd HH mm ss` etc. Example: `yyyy-MM-dd-HH-mm`.

Forbidden characters in folder names (`* : ? " < > | / \`) are replaced with
`_` automatically.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- defaultSubfolderName: ""
  $name: "Default subfolder name (mode 1)"
  $description: "Pre-filled name for the \"Fixed name\" option. Leave empty to use Windows' own localized \"New folder\" name (matches your OS language)."
- defaultMode: fixed
  $name: "Default selected mode"
  $description: "Which radio button is pre-selected when the dialog opens."
  $options:
  - fixed: "Fixed name"
  - perName: "By file name"
  - perExt: "By extension"
  - date: "By date"
- theme: auto
  $name: "Dialog theme"
  $description: "Light/dark appearance of the \"Files 2 Folder\" dialog."
  $options:
  - auto: "Auto (follow Windows app theme)"
  - light: "Light"
  - dark: "Dark"
- silentMode: false
  $name: "Silent mode (right-click menu)"
  $description: |-
    On: choosing "Move to a folder..." from the right-click menu skips the dialog and immediately moves the selection, using the "Default selected mode" (with the default subfolder name / date format) from these settings.
    Off (default): the dialog is shown first.
    The hotkey workaround is unaffected — it always shows the dialog.
- nearCutCopy: false
  $name: "Place menu item near Cut/Copy"
  $description: |-
    On: the "Move to a folder..." entry appears just after Cut/Copy (between Copy and Paste).
    Off (default): it appears at the top of the menu.
- slowMode: false
  $name: "Slow mode (safer, with undo)"
  $description: |-
    On: moves go through the standard Windows file-operation system, so you get the familiar progress dialog, Ctrl+Z undo, the "Replace or skip files?" prompt for conflicts, and a UAC prompt when needed. The trade-off is that it's noticeably slower, especially with hundreds of files.
    Off (default): moves are instant — but there's no Ctrl+Z to undo, no progress bar, and no prompt if a destination needs admin rights (it just fails).
- hotkeyEnabled: true
  $name: "Workaround for other programs"
  $description: |-
    On (default): registers a global hotkey that works in any file manager (Total Commander, Directory Opus, Files, etc.). Usage: select files in the other program, press Ctrl+C (or Ctrl+X) to copy/cut them, then press the hotkey — the mod reads the file paths from the clipboard, asks where to move them, and moves them into the parent folder of the first selected file.
    Off: disables the hotkey. Turn it off if it conflicts with another program's shortcut.
- hotkeySilent: false
  $name: "Silent"
  $description: |-
    On: the hotkey does nothing if the clipboard doesn't hold 2+ file paths (no popup, no sound). Useful if you've reassigned the hotkey to a key combo you press often.
    Off (default): a help popup explains how to use the workaround.
- hotkeyChar: "F"
  $name: "Workaround hotkey: key"
  $description: "Main key (single character A–Z or 0–9)."
- hotkeyCtrl: true
  $name: "Workaround hotkey: Ctrl"
- hotkeyAlt: true
  $name: "Workaround hotkey: Alt"
- hotkeyShift: false
  $name: "Workaround hotkey: Shift"
- hotkeyWin: false
  $name: "Workaround hotkey: Win"
  $description: "IMPORTANT: bare Win+<letter> shortcuts (Win+F, Win+E, Win+R, Win+L, Win+G, etc.) are handled by Windows itself and CANNOT be intercepted — they will not work even with the keyboard hook. To use the Win key, you MUST combine it with another modifier: enable Win together with Ctrl, Alt, or Shift (e.g. Win+Shift+F or Win+Alt+F). Enabling any Win combo switches the workaround to a low-level keyboard hook (the technique PowerToys uses), which watches all keystrokes while active and is slightly heavier than a plain hotkey — leave Win off and use Ctrl/Alt if you don't need it."
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
#include <windhawk_utils.h>
#include <string>
#include <vector>
#include <algorithm>

// ============================================================
//  Localized strings from shell32.dll
//
//  shell32 ships string resources that the shell uses for its own UI, so
//  pulling them out gives us text that's already translated into the user's
//  Windows language. We use:
//    16859 -> "New folder"        (default subfolder name)
//    30305 -> "Move to a folder"  (context-menu entry; ships with a '&'
//                                   accelerator and a trailing "..." in some
//                                   builds, which is exactly what we want. In
//                                   the menu-bar Edit dropdown we append
//                                   " (F2F)" so our item is distinct from
//                                   Explorer's identically-named built-in
//                                   command, which lives there too)
//  If a resource is missing (very old/odd builds) we fall back to an English
//  literal supplied by the caller.
// ============================================================
static std::wstring LoadShell32String(UINT id, const wchar_t* fallback) {
    static HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr,
                                             LOAD_LIBRARY_AS_DATAFILE);
    if (hShell32) {
        // LoadStringW with cchBufferMax == 0 returns a read-only pointer to the
        // resource and its length, avoiding a guess at the buffer size.
        LPCWSTR p = nullptr;
        int len = LoadStringW(hShell32, id, (LPWSTR)&p, 0);
        if (len > 0 && p)
            return std::wstring(p, len);
    }
    return fallback ? fallback : L"";
}

// ============================================================
//  The 4 modes. The string keys here MUST match the defaultMode $options
//  keys in the settings block above — this table is the single mapping from
//  the dropdown value to the mode used throughout the code.
// ============================================================
enum F2FMode {
    MODE_FIXED_NAME       = 1,
    MODE_PER_FILE_NAME    = 2,
    MODE_PER_EXTENSION    = 3,
    MODE_DATE_NAMED       = 4,
};

static const struct { const wchar_t* key; F2FMode mode; } kModeKeys[] = {
    { L"fixed",   MODE_FIXED_NAME },
    { L"perName", MODE_PER_FILE_NAME },
    { L"perExt",  MODE_PER_EXTENSION },
    { L"date",    MODE_DATE_NAMED },
};

static F2FMode ModeFromKey(const std::wstring& key) {
    for (auto& e : kModeKeys)
        if (key == e.key) return e.mode;
    Wh_Log(L"Files2Folders: unknown defaultMode key '%s', using fixed", key.c_str());
    return MODE_FIXED_NAME;
}

// ============================================================
//  Settings
// ============================================================
struct ModSettings {
    std::wstring defaultSubfolderName;
    std::wstring dateFormat;
    int defaultMode;
    bool slowMode;
    bool silentMode;
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
    // Empty default in the schema means "use the shell's own localized
    // 'New folder' string" — so the pre-filled name matches the language of
    // the OS without the user configuring anything.
    PCWSTR s = Wh_GetStringSetting(L"defaultSubfolderName");
    g_settings.defaultSubfolderName =
        (s && *s) ? s : LoadShell32String(16859, L"New folder");
    if (s) Wh_FreeStringSetting(s);

    s = Wh_GetStringSetting(L"dateFormat");
    g_settings.dateFormat = (s && *s) ? s : L"yyyy-MM-dd-HH-mm";
    if (s) Wh_FreeStringSetting(s);

    // defaultMode is a string dropdown; ModeFromKey maps it to F2FMode via the
    // single kModeKeys table (unknown keys fall back to fixed, with a log line).
    PCWSTR dm = Wh_GetStringSetting(L"defaultMode");
    std::wstring dmStr = dm ? dm : L"";
    if (dm) Wh_FreeStringSetting(dm);
    g_settings.defaultMode = ModeFromKey(dmStr);

    g_settings.slowMode = Wh_GetIntSetting(L"slowMode") != 0;
    g_settings.silentMode = Wh_GetIntSetting(L"silentMode") != 0;
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

// The Cut/Copy command ids vary by menu surface: FCIDM_SHVIEW_CUT/COPY =
// 31001/31002 in the shell view's own menu, or 25/26 in some legacy menus.
// One source of truth so the right-click and menu-bar paths agree.
static bool IsCutCopyId(UINT id) {
    return id == 31001 || id == 31002 || id == 25 || id == 26;
}

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

// Common tail for both the browser-window and desktop lookups: an
// IShellWindows item (IDispatch) -> top-level IShellBrowser -> active view.
// Caller releases the returned view.
static IShellView* ShellViewFromDispatch(IDispatch* pDisp) {
    IShellView* pSV = nullptr;
    IServiceProvider* pSP = nullptr;
    if (SUCCEEDED(pDisp->QueryInterface(IID_IServiceProvider, (void**)&pSP)) && pSP) {
        IShellBrowser* pSB = nullptr;
        if (SUCCEEDED(pSP->QueryService(SID_STopLevelBrowser,
                                        IID_IShellBrowser, (void**)&pSB)) && pSB) {
            pSB->QueryActiveShellView(&pSV);
            pSB->Release();
        }
        pSP->Release();
    }
    return pSV;
}

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
                pSVResult = ShellViewFromDispatch(pDisp);
            }
            pWBA->Release();
        }
        pDisp->Release();
    }
    pSW->Release();
    return pSVResult;
}

// Active shell view of the desktop, via IShellWindows::FindWindowSW with
// SWC_DESKTOP. Caller releases. The desktop's SHELLDLL_DefView lives inside
// Progman / WorkerW and is NOT in the IShellWindows browser collection, so
// GetActiveShellViewForHwnd can't find it — this is the dedicated path.
static IShellView* GetDesktopShellView() {
    IShellWindows* pSW = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_IShellWindows, (void**)&pSW)) || !pSW)
        return nullptr;

    IShellView* pSVResult = nullptr;
    VARIANT vEmpty = {};
    long lhwnd = 0;
    IDispatch* pDisp = nullptr;
    if (SUCCEEDED(pSW->FindWindowSW(&vEmpty, &vEmpty, SWC_DESKTOP, &lhwnd,
                                    SWFO_NEEDDISPATCH, &pDisp)) && pDisp) {
        pSVResult = ShellViewFromDispatch(pDisp);
        pDisp->Release();
    }
    pSW->Release();
    return pSVResult;
}

// Pull the current folder path + selected item paths out of a shell view.
// Shared by the browser-window and desktop code paths.
static bool ExtractFolderAndSelection(IShellView* pSV,
                                      std::wstring& folderOut,
                                      std::vector<std::wstring>& itemsOut)
{
    folderOut.clear();
    itemsOut.clear();
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

        // The container's own pidl may not resolve to a filesystem path
        // (libraries, search results, namespace extensions), but the selected
        // items are absolute filesystem paths. In that case derive the folder
        // from the first item's parent so the operation can still proceed.
        if (folderOut.empty() && !itemsOut.empty()) {
            const std::wstring& first = itemsOut.front();
            size_t slash = first.find_last_of(L"\\/");
            if (slash != std::wstring::npos)
                folderOut = first.substr(0, slash);
        }

        ok = !folderOut.empty() && !itemsOut.empty();
    }
    return ok;
}

// Extract folder + selection for the menu's window. Try the browser-window
// lookup first; if that finds no view (the desktop is not in the IShellWindows
// browser collection), fall back to the desktop view. Trying both — rather than
// guessing desktop-vs-browser from window classes up front — avoids the two
// heuristics disagreeing on unusual shell topologies (the desktop right-click
// host is also a SHELLDLL_DefView, so a class-based guess is unreliable).
static bool GetFolderAndSelectionForHwnd(HWND hwnd,
                                         std::wstring& folderOut,
                                         std::vector<std::wstring>& itemsOut)
{
    folderOut.clear();
    itemsOut.clear();

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) root = hwnd;

    IShellView* pSV = GetActiveShellViewForHwnd(root);
    if (!pSV) pSV = GetDesktopShellView();
    if (!pSV) return false;

    bool ok = ExtractFolderAndSelection(pSV, folderOut, itemsOut);
    pSV->Release();
    return ok;
}

// ============================================================
//  Direct Win32 move — same-volume = instant rename, no shell UI.
//  Returns number of successfully moved items.
// ============================================================
// Pick a "destDir\leaf" path that doesn't collide with anything already there,
// numbering duplicates the way Explorer does: "leaf", "leaf (2)", "leaf (3)"...
// When splitExtension is true (files), the number goes before the extension
// ("name (2).txt"); when false (folders, which have no extension) it's appended
// to the whole name ("New folder (2)").
static std::wstring UniqueDest(const std::wstring& destDir,
                               const std::wstring& leaf,
                               bool splitExtension)
{
    std::wstring base = leaf, ext;
    if (splitExtension) {
        size_t dot = leaf.find_last_of(L'.');
        if (dot != std::wstring::npos && dot != 0) {
            base = leaf.substr(0, dot);
            ext  = leaf.substr(dot);
        }
    }
    std::wstring p = destDir + L"\\" + base + ext;
    if (GetFileAttributesW(p.c_str()) == INVALID_FILE_ATTRIBUTES)
        return p;
    for (int n = 2; n < 100000; ++n) {
        p = destDir + L"\\" + base + L" (" + std::to_wstring(n) + L")" + ext;
        if (GetFileAttributesW(p.c_str()) == INVALID_FILE_ATTRIBUTES)
            return p;
    }
    return destDir + L"\\" + base + ext;
}

static int MoveItemsFast(HWND owner,
                         const std::vector<std::pair<std::wstring, std::wstring>>& moves,
                         bool silent)
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
            std::wstring dst = UniqueDest(destDir, leaf, /*splitExtension=*/true);
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

    if (failed > 0 && owner && !silent) {
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

static void DoFiles2Folder(HWND owner,
                           F2FMode mode,
                           const std::wstring& folder,
                           const std::vector<std::wstring>& items,
                           const std::wstring& fixedName,
                           const std::wstring& dateText,
                           bool silent)
{
    std::vector<std::pair<std::wstring, std::wstring>> moves;

    if (mode == MODE_FIXED_NAME || mode == MODE_DATE_NAMED) {
        std::wstring raw = (mode == MODE_FIXED_NAME) ? fixedName : dateText;
        std::wstring sub = SanitizeFolderName(raw);
        if (sub.empty() || sub == L"_") {
            if (!silent)
                MessageBoxW(owner,
                    L"The subfolder name cannot be empty.",
                    L"Files 2 Folder", MB_ICONWARNING);
            return;
        }
        // Don't reuse an existing folder of this name — number it like
        // Explorer does ("New folder", "New folder (2)", ...). Folders have no
        // extension, so don't split one off.
        std::wstring dest = UniqueDest(folder, sub, /*splitExtension=*/false);
        if (!EnsureDir(dest)) {
            if (!silent)
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
        MoveItemsFast(owner, moves, silent);
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
    // The auto-select-companion-radio behavior (EN_SETFOCUS below) must only
    // react to user focus changes, not the initial focus the dialog manager
    // assigns during/after WM_INITDIALOG — otherwise the default mode chosen
    // here is immediately overwritten by whichever edit box gets first focus.
    static bool ready = false;
    switch (msg) {
    case WM_INITDIALOG: {
        ready = false;
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

        int rb = IDC_RB_FIXED;
        switch (s->mode) {
            case MODE_FIXED_NAME:    rb = IDC_RB_FIXED; break;
            case MODE_PER_FILE_NAME: rb = IDC_RB_PERNAME; break;
            case MODE_PER_EXTENSION: rb = IDC_RB_PEREXT; break;
            case MODE_DATE_NAMED:    rb = IDC_RB_DATE; break;
        }
        CheckRadioButton(hDlg, IDC_RB_FIXED, IDC_RB_DATE, rb);

        // Focus the chosen radio so the BS_AUTORADIOBUTTON group settles on it
        // and the dialog manager doesn't move the checkmark to the first radio.
        // We set focus ourselves and return FALSE so the manager leaves it be.
        ready = true;  // from now on, EN_SETFOCUS reflects real user focus
        SetFocus(GetDlgItem(hDlg, rb));
        return FALSE;  // we set focus; don't let the manager reset it
    }
    case WM_COMMAND: {
        WORD id   = LOWORD(wp);
        WORD code = HIWORD(wp);

        // Clicking or tabbing into an edit box selects its companion radio,
        // so the mode the user is editing is the mode that runs on OK.
        if (code == EN_SETFOCUS && ready) {
            if (id == IDC_ED_FIXED)
                CheckRadioButton(hDlg, IDC_RB_FIXED, IDC_RB_DATE, IDC_RB_FIXED);
            else if (id == IDC_ED_DATE)
                CheckRadioButton(hDlg, IDC_RB_FIXED, IDC_RB_DATE, IDC_RB_DATE);
            return TRUE;
        }
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
                            const std::vector<std::wstring>& items,
                            bool silent)
{
    if (folder.empty() || items.empty()) return;

    // Silent mode (right-click menu only): skip the dialog and move using the
    // configured defaults.
    if (silent) {
        F2FMode mode = (F2FMode)g_settings.defaultMode;
        DoFiles2Folder(owner, mode, folder, items,
                       g_settings.defaultSubfolderName,
                       FormatNow(g_settings.dateFormat), /*silent=*/true);
        return;
    }

    // The dialog always opens on the configured "Default selected mode" and
    // "Default subfolder name" — no session memory of previous choices.
    DlgState state = {};
    state.mode = (F2FMode)g_settings.defaultMode;
    state.fixedName = g_settings.defaultSubfolderName;
    state.dateText = FormatNow(g_settings.dateFormat);
    state.singleItem = (items.size() == 1);
    state.itemCount = items.size();

    if (!ShowF2FDialog(owner, state)) return;

    DoFiles2Folder(owner, state.mode, folder, items,
                   state.fixedName, state.dateText, /*silent=*/false);
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

// Is our command already present in this menu? Guards against re-inserting
// when WM_INITMENUPOPUP fires repeatedly for the same popup.
static bool MenuHasF2FItem(HMENU hmenu) {
    int count = GetMenuItemCount(hmenu);
    for (int i = 0; i < count; ++i)
        if (GetMenuItemID(hmenu, i) == F2F_MENU_CMD) return true;
    return false;
}

// Index just past the last Cut/Copy item, or -1 if the menu has neither.
// Used both to decide whether a menu is "the Edit/clipboard menu" and to find
// the insert position for the nearCutCopy layout.
static int MenuPosAfterCutCopy(HMENU hmenu) {
    int pos = -1;
    int count = GetMenuItemCount(hmenu);
    for (int i = 0; i < count; ++i)
        if (IsCutCopyId(GetMenuItemID(hmenu, i))) pos = i + 1;
    return pos;
}

// Insert the "Files 2 Folder" command into a menu, positioned per the
// nearCutCopy setting. Shared by the right-click (TrackPopupMenuEx) path and
// the classic menu-bar Edit dropdown (WM_INITMENUPOPUP) path.
//   nearCutCopy off: top of menu + separator below.
//   nearCutCopy on : just after the Cut/Copy block.
//
// addSuffix appends " (F2F)" to the label. Pass true ONLY for the menu-bar Edit
// dropdown, where Explorer has its own built-in "Move to a folder..." command
// sharing this exact string (resource 30305) — without the suffix the two are
// indistinguishable and users click the native one, which opens a folder-picker
// instead of our dialog. The right-click context menu has no such built-in
// twin, so it passes false and keeps the clean localized label.
static void InsertF2FMenuItem(HMENU hmenu, bool nearCutCopy, bool addSuffix) {
    if (MenuHasF2FItem(hmenu)) return;

    // Localized label from shell32 ("Move to a folder..."), English fallback.
    // Locale-fixed for the process, so resolve each variant once.
    static const std::wstring baseLabel =
        LoadShell32String(30305, L"Files 2 &Folder...");
    static const std::wstring suffixLabel = baseLabel + L" (F2F)";
    const std::wstring& label = addSuffix ? suffixLabel : baseLabel;
    if (nearCutCopy) {
        int insertPos = MenuPosAfterCutCopy(hmenu);
        if (insertPos < 0) insertPos = 0;  // no Cut/Copy found — top of menu
        InsertMenuW(hmenu, insertPos, MF_BYPOSITION | MF_STRING,
                    F2F_MENU_CMD, label.c_str());
        // Only add a trailing separator if the item we're pushing down isn't
        // already one — Explorer normally has a separator after the
        // Cut/Copy/Paste block, and inserting ours next to it would produce a
        // doubled separator.
        bool nextIsSeparator = false;
        if (insertPos + 1 < GetMenuItemCount(hmenu)) {
            MENUITEMINFOW mii = { sizeof(mii) };
            mii.fMask = MIIM_FTYPE;
            if (GetMenuItemInfoW(hmenu, insertPos + 1, TRUE, &mii))
                nextIsSeparator = (mii.fType & MFT_SEPARATOR) != 0;
        }
        if (!nextIsSeparator)
            InsertMenuW(hmenu, insertPos + 1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
    } else {
        InsertMenuW(hmenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
        InsertMenuW(hmenu, 0, MF_BYPOSITION | MF_STRING,
                    F2F_MENU_CMD, label.c_str());
    }
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
            InsertF2FMenuItem(hmenu, g_settings.nearCutCopy, /*addSuffix=*/false);
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
        RunFiles2Folder(owner, folder, items, g_settings.silentMode);
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
        RunFiles2Folder(owner, folder, items, g_settings.silentMode);
        return TRUE;
    }
    return PostMessageW_Orig(hWnd, Msg, wp, lp);
}

// ============================================================
//  Classic menu-bar "Edit" dropdown (File / Edit / View / Tools)
//
//  The classic Explorer menu bar is the frame window's own Win32 menu; its
//  dropdowns are tracked by the system, not via TrackPopupMenuEx, so the hook
//  above never sees them. To add our entry there we subclass the Explorer
//  frame window (CabinetWClass / ExploreWClass) and:
//    - WM_INITMENUPOPUP: when the popup about to open is the Edit menu (we
//      detect it by the presence of the Cut/Copy command ids) and the view
//      has a selection, insert our item right after Cut/Copy.
//    - WM_COMMAND for our id: resolve the current selection and run the flow.
//  We always place it after Cut/Copy in this menu (the "top of menu" layout
//  used by the context menu doesn't make sense for the Edit dropdown).
// ============================================================
static bool IsExplorerFrameClass(HWND hwnd) {
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    return !_wcsicmp(cls, L"CabinetWClass") || !_wcsicmp(cls, L"ExploreWClass");
}

// Does this frame currently host a shell view? A direct child-window walk for
// SHELLDLL_DefView — far cheaper than the COM IShellWindows enumeration, so we
// use it as a fast gate in WM_INITMENUPOPUP before paying for selection lookup.
static BOOL CALLBACK FindShellViewChildProc(HWND child, LPARAM lp) {
    WCHAR cls[64] = {};
    GetClassNameW(child, cls, 64);
    if (!_wcsicmp(cls, L"SHELLDLL_DefView")) {
        *(HWND*)lp = child;
        return FALSE;  // stop
    }
    return TRUE;
}
static bool FrameHasShellView(HWND frame) {
    HWND sv = nullptr;
    EnumChildWindows(frame, FindShellViewChildProc, (LPARAM)&sv);
    return sv != nullptr;
}

// One-shot latch for the menu-id diagnostic below (logs a single sample).
static bool g_loggedMenuIds = false;

static LRESULT CALLBACK FrameSubclassProc(HWND hwnd, UINT uMsg,
                                          WPARAM wParam, LPARAM lParam,
                                          DWORD_PTR /*dwRefData*/)
{
    switch (uMsg) {
    case WM_INITMENUPOPUP: {
        HMENU popup = (HMENU)wParam;
        // HIWORD(lParam) is TRUE for the window (system) menu — skip those.
        // Cheap gates first (menu has Cut/Copy; frame hosts a shell view), so
        // the COM selection lookup only runs for the actual Edit dropdown.
        if (popup && !HIWORD(lParam)) {
            bool hasCutCopy = MenuPosAfterCutCopy(popup) >= 0;
            if (hasCutCopy && FrameHasShellView(hwnd)) {
                std::wstring folder;
                std::vector<std::wstring> items;
                if (GetFolderAndSelectionForHwnd(hwnd, folder, items) &&
                    !items.empty())
                {
                    InsertF2FMenuItem(popup, /*nearCutCopy=*/true, /*addSuffix=*/true);
                }
            } else if (!hasCutCopy && !g_loggedMenuIds) {
                // Diagnostic (once per process): the Cut/Copy command ids in the
                // classic menu-bar Edit dropdown are build/shell-dependent. If
                // our entry never shows up there, enable mod logging and look
                // for this line — it lists a popup's command ids so the right
                // ids can be added to IsCutCopyId().
                g_loggedMenuIds = true;
                int count = GetMenuItemCount(popup);
                std::wstring ids;
                for (int i = 0; i < count; ++i)
                    ids += std::to_wstring((int)GetMenuItemID(popup, i)) + L" ";
                Wh_Log(L"Files2Folders: sample menu popup ids: %s", ids.c_str());
            }
        }
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == F2F_MENU_CMD && HIWORD(wParam) == 0) {
            std::wstring folder;
            std::vector<std::wstring> items;
            if (GetFolderAndSelectionForHwnd(hwnd, folder, items) &&
                !items.empty())
            {
                RunFiles2Folder(hwnd, folder, items, g_settings.silentMode);
            }
            return 0;  // handled
        }
        break;
    case WM_NCDESTROY:
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, FrameSubclassProc);
        break;
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

// Subclass any Explorer frame window in this process that we haven't already.
// We mark handled frames with a window property so repeated attach passes
// (and the polling thread below) don't double-subclass.
static const wchar_t* kF2FSubclassProp = L"Files2FolderFrameSubclassed";

static BOOL CALLBACK AttachFrameProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;  // only our own windows
    if (!IsExplorerFrameClass(hwnd)) return TRUE;
    if (GetPropW(hwnd, kF2FSubclassProp)) return TRUE;  // already done
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, FrameSubclassProc, 0))
        SetPropW(hwnd, kF2FSubclassProp, (HANDLE)1);
    return TRUE;
}

static void AttachAllFrames() {
    EnumWindows(AttachFrameProc, 0);
}

static BOOL CALLBACK DetachFrameProc(HWND hwnd, LPARAM) {
    if (GetPropW(hwnd, kF2FSubclassProp)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, FrameSubclassProc);
        RemovePropW(hwnd, kF2FSubclassProp);
    }
    return TRUE;
}

static void DetachAllFrames() {
    EnumWindows(DetachFrameProc, 0);
}

// New Explorer windows open after the mod loads, so a one-shot attach isn't
// enough. A lightweight thread re-runs the attach pass every ~1s; the property
// guard makes re-attaching a no-op for frames already handled. Shutdown is
// signalled via an event so the thread wakes immediately instead of polling a
// flag.
static HANDLE g_frameWatchThread = nullptr;
static HANDLE g_frameWatchStopEvent = nullptr;

static DWORD WINAPI FrameWatchThreadProc(LPVOID) {
    do {
        AttachAllFrames();
    } while (WaitForSingleObject(g_frameWatchStopEvent, 1000) == WAIT_TIMEOUT);
    return 0;
}

static void StartFrameWatch() {
    if (g_frameWatchThread) return;
    g_frameWatchStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_frameWatchThread = CreateThread(nullptr, 0, FrameWatchThreadProc,
                                      nullptr, 0, nullptr);
}

static void StopFrameWatch() {
    if (g_frameWatchThread) {
        SetEvent(g_frameWatchStopEvent);
        WaitForSingleObject(g_frameWatchThread, 2000);
        CloseHandle(g_frameWatchThread);
        g_frameWatchThread = nullptr;
    }
    if (g_frameWatchStopEvent) {
        CloseHandle(g_frameWatchStopEvent);
        g_frameWatchStopEvent = nullptr;
    }
    DetachAllFrames();
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
static HHOOK   g_llKeyHook = nullptr;   // only used for Win-based combos
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
    RunFiles2Folder(nullptr, folder, items, /*silent=*/false);
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

// Human-readable combo (e.g. "Ctrl+Alt+F") for failure messages.
static std::wstring DescribeHotkey() {
    std::wstring s;
    if (g_settings.hotkeyCtrl)  s += L"Ctrl+";
    if (g_settings.hotkeyAlt)   s += L"Alt+";
    if (g_settings.hotkeyShift) s += L"Shift+";
    if (g_settings.hotkeyWin)   s += L"Win+";
    s += (wchar_t)BuildHotkeyVk();
    return s;
}

static LRESULT CALLBACK HotkeyWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    // WM_HOTKEY (RegisterHotKey path) and WM_F2F_TRIGGER (low-level-hook path)
    // both mean "the combo fired" — the real work runs here, off the hook.
    if ((msg == WM_HOTKEY && (int)wp == HOTKEY_ID) || msg == WM_F2F_TRIGGER) {
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

// Low-level keyboard hook — used ONLY for Win-based combos, which Windows
// reserves so RegisterHotKey can't claim them (Win+F, Win+E, …). See
// keyboardhook.md for the full risk write-up. The callback must do the
// absolute minimum and return fast (Windows silently uninstalls a hook whose
// callback exceeds LowLevelHooksTimeout, ~300 ms): we detect the combo, post a
// message to the worker window, and return — all real work happens there.
static bool ModifiersMatchForLLHook() {
    auto down = [](int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; };
    bool ctrl  = down(VK_CONTROL);
    bool alt   = down(VK_MENU);
    bool shift = down(VK_SHIFT);
    bool win   = down(VK_LWIN) || down(VK_RWIN);
    // Require exactly the configured modifier set so e.g. Win+Shift+F doesn't
    // fire a Win+F binding.
    return ctrl  == g_settings.hotkeyCtrl
        && alt   == g_settings.hotkeyAlt
        && shift == g_settings.hotkeyShift
        && win   == g_settings.hotkeyWin;
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)lParam;
        UINT vk = BuildHotkeyVk();
        if (k && k->vkCode == vk &&
            (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) &&
            ModifiersMatchForLLHook())
        {
            // Fire once per press (ignore auto-repeat is unnecessary: a fresh
            // KEYDOWN with matching modifiers is the user re-pressing). Post and
            // swallow so the OS default (e.g. Win+F -> Feedback Hub) doesn't run.
            if (g_hotkeyWnd) PostMessageW(g_hotkeyWnd, WM_F2F_TRIGGER, 0, 0);
            return 1;  // consume the keystroke
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
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

    // Win-based combos can't be claimed by RegisterHotKey, so use a low-level
    // keyboard hook for those (what PowerToys uses). Ctrl/Alt-only combos keep
    // the clean, zero-overhead RegisterHotKey path.
    //
    // Caveat: bare Win+<letter> shortcuts (Win+F, Win+L, Win+E, …) are handled
    // by Windows below the hook and CANNOT be intercepted — the user must pair
    // Win with another modifier (Win+Shift+F, Win+Alt+F, …). Warn if they
    // selected Win alone.
    if (g_settings.hotkeyWin) {
        bool hasOtherMod = g_settings.hotkeyCtrl || g_settings.hotkeyAlt ||
                           g_settings.hotkeyShift;
        if (!hasOtherMod && !g_settings.hotkeySilent) {
            std::wstring combo = DescribeHotkey();
            Wh_Log(L"Files2Folders: bare Win combo (%s) can't be intercepted",
                   combo.c_str());
            MessageBoxW(nullptr,
                (L"Files 2 Folder: the hotkey \"" + combo + L"\" will not work.\n\n"
                 L"Bare Win+<key> shortcuts are handled by Windows itself and "
                 L"cannot be intercepted, even with a keyboard hook.\n\n"
                 L"Combine the Win key with another modifier — enable Ctrl, Alt, "
                 L"or Shift as well (e.g. Win+Shift+F).").c_str(),
                L"Files 2 Folder", MB_ICONWARNING | MB_OK);
        }
        g_llKeyHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                        (HINSTANCE)GetModuleHandleW(nullptr), 0);
        if (!g_llKeyHook) {
            Wh_Log(L"Files2Folders: SetWindowsHookEx(WH_KEYBOARD_LL) failed");
            if (!g_settings.hotkeySilent)
                MessageBoxW(nullptr,
                    L"Files 2 Folder could not install the keyboard hook needed "
                    L"for a Win-based hotkey.\n\nTry a Ctrl/Alt combination "
                    L"instead.",
                    L"Files 2 Folder", MB_ICONWARNING | MB_OK);
        }
    } else if (!RegisterHotKey(g_hotkeyWnd, HOTKEY_ID,
                               BuildHotkeyModifiers() | MOD_NOREPEAT,
                               BuildHotkeyVk()))
    {
        std::wstring combo = DescribeHotkey();
        Wh_Log(L"Files2Folders: RegisterHotKey(%s) failed (reserved or in use?)",
               combo.c_str());
        // Make the failure visible — otherwise the workaround just silently
        // "doesn't work". Suppressed in Silent mode.
        if (!g_settings.hotkeySilent) {
            std::wstring msg =
                L"Files 2 Folder could not register the workaround hotkey:\n\n    "
                + combo +
                L"\n\nThe combination is already in use by another program.\n\n"
                L"Pick a different key or modifier in the mod's settings.";
            MessageBoxW(nullptr, msg.c_str(), L"Files 2 Folder",
                        MB_ICONWARNING | MB_OK);
        }
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_llKeyHook) {
        UnhookWindowsHookEx(g_llKeyHook);
        g_llKeyHook = nullptr;
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
    StartFrameWatch();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Files2Folders: Uninit");
    StopHotkeyThread();
    StopFrameWatch();
}
