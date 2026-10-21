// ==WindhawkMod==
// @id              bulk-copy-file-and-folder-names
// @name            Bulk Copy File and Folder Names
// @description     Copy the names of one or many selected files and folders in File Explorer, not the paths, with a hotkey or the right-click menu
// @version         0.3.0
// @author          Shaerif
// @github          https://github.com/Shaerif
// @include         explorer.exe
// @compilerOptions -luser32 -lole32 -loleaut32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Bulk Copy File and Folder Names
Windows has Copy as path (Ctrl+Shift+C). It gives you full paths in quotes. Often
you only want the names. This mod copies the names of the files and folders you
select in File Explorer as plain text, one per line. It works on one item or on
many at once, and a selection can mix files and folders.

## Keys
- Ctrl+Shift+N copies the names with their extensions.
- Ctrl+Alt+Shift+N copies the names without extensions. Folders keep their full
  name, so `sub.folder` stays `sub.folder`. `a.tar.gz` becomes `a.tar`. A name
  that starts with a dot, like `.gitignore`, stays whole.

The keys only work while a File Explorer window is in front. You can change both
keys in the settings.

## Right-click menu
Right-click a file, a folder, or a selection of many files and folders in a File
Explorer window. The classic menu gets two entries, Copy name and Copy name
without extension. They follow the same rules and settings as the keys. On
Windows 11 they're in the menu you get from Show more options or
Shift+right-click. You can turn them off in the settings.

## Settings
- Separator: a new line, a comma, or a space between names.
- Quote names: put each name in double quotes.
- Right-click menu: show the two menu entries.

## Limits
- Ctrl+Shift+N is also File Explorer's New folder key. While the mod is on, it
  copies names instead. Pick another key in the settings if you need New folder.
- The keys and menu entries don't work on the desktop or in file dialogs. Only
  File Explorer windows count.
- The Windows 11 compact menu can't take new entries from a mod. Use Show more
  options.
- The menu entries don't show on the column header menu or the menu you get
  from empty space in a folder.
- Items that aren't files or folders, like drives in This PC, give their
  internal name.
- If nothing is selected, nothing happens.

## Tutorial
1. Select the files and folders in a File Explorer window. A mix of both is
   fine.
   ![A File Explorer window with two folders and five files selected in the demo-names folder](https://raw.githubusercontent.com/Shaerif/windhawk-mods/main/docs/screenshots/bulk-copy-step-1-select.png)
2. Keep that window in front, press Ctrl+Shift+N and paste. You get every name
   with its extension, one per line.
   ![The selected names pasted into a text window, one per line, with their extensions](https://raw.githubusercontent.com/Shaerif/windhawk-mods/main/docs/screenshots/bulk-copy-step-2-names-with-extensions.png)
3. Press Ctrl+Alt+Shift+N with the same selection and paste again. Files lose
   their extension and folders keep their full name.
   ![The same names pasted again without extensions, while the folder Backup.2025 keeps its full name](https://raw.githubusercontent.com/Shaerif/windhawk-mods/main/docs/screenshots/bulk-copy-step-3-names-without-extensions.png)
4. Or hold Shift and right-click the selection, and pick Copy name or Copy name
   without extension.
   ![The lower part of the classic right-click menu, with Copy name and Copy name without extension in their own group above Properties](https://raw.githubusercontent.com/Shaerif/windhawk-mods/main/docs/screenshots/bulk-copy-step-4-right-click-menu.png)

### Gotchas
- Step 2: Ctrl+Shift+N doesn't make new folders while the mod is on. Move the
  copy key in the settings if you use New folder from the keyboard.
- Step 3: Only the last extension goes, so `archive.tar.gz` becomes
  `archive.tar`.
- Step 4: The compact Windows 11 menu can't show the entries. Use
  Shift+right-click or Show more options.

Read the full guide in
[docs/bulk-copy-file-and-folder-names.md](https://github.com/Shaerif/windhawk-mods/blob/main/docs/bulk-copy-file-and-folder-names.md).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- withExtModifier: ctrl_shift
  $name: Modifiers for copy names with extension
  $options:
  - ctrl_shift: Ctrl+Shift
  - ctrl_alt: Ctrl+Alt
  - ctrl_alt_shift: Ctrl+Alt+Shift
  - alt_shift: Alt+Shift
  - win_shift: Win+Shift
- withExtKey: N
  $name: Key for copy names with extension
  $description: A letter, a digit, or F1 to F24
- noExtModifier: ctrl_alt_shift
  $name: Modifiers for copy names without extension
  $options:
  - ctrl_shift: Ctrl+Shift
  - ctrl_alt: Ctrl+Alt
  - ctrl_alt_shift: Ctrl+Alt+Shift
  - alt_shift: Alt+Shift
  - win_shift: Win+Shift
- noExtKey: N
  $name: Key for copy names without extension
  $description: A letter, a digit, or F1 to F24
- separator: newline
  $name: Separator
  $description: What goes between names when you copy many files and folders at once
  $options:
  - newline: New line
  - comma: Comma
  - space: Space
- quoteNames: false
  $name: Put each name in double quotes
- contextMenu: true
  $name: Right-click menu entries
  $description: Add Copy name and Copy name without extension to the classic File Explorer menu for files and folders
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <exdisp.h>
#include <servprov.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <cwctype>
#include <string>
#include <vector>

#define WM_APP_RELOAD (WM_APP + 1)
#define WM_APP_QUIT (WM_APP + 2)

enum {
    kHotkeyWithExt = 1,
    kHotkeyNoExt = 2,
};

static const UINT_PTR kTimerId = 1;
static const UINT kTimerMs = 100;

// SID_STopLevelBrowser, defined here so the mod doesn't depend on which GUIDs
// the import libraries carry.
static const GUID kSidTopLevelBrowser = {
    0x4C96BE40, 0x915C, 0x11CF, {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};

// Shell tab windows answer this with their IShellBrowser, without a reference.
static const UINT kMsgGetShellBrowser = WM_USER + 7;

// The folder view's Refresh command. Only the empty-space menu has it.
static const UINT kRefreshCmd = 0x7103;

// Our menu IDs sit above the shell's command range (up to 0x7FFF) and below the
// system commands (0xF000). Each menu is still checked for a clash first.
static const UINT kMenuIdBase = 0xE7A0;
static const int kMenuIdTries = 16;

static const wchar_t kMenuLabelWithExt[] = L"Copy name";
static const wchar_t kMenuLabelNoExt[] = L"Copy name without extension";

struct {
    UINT withExtModifiers;
    UINT withExtKey;
    UINT noExtModifiers;
    UINT noExtKey;
    std::wstring separator;
    bool quoteNames;
} g_settings;

// The menu hook reads the settings on File Explorer's own threads, so writes
// and those reads go through this lock.
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static std::atomic<bool> g_contextMenu{true};

static HANDLE g_thread = nullptr;
static DWORD g_threadId = 0;
static HWND g_msgWnd = nullptr;
static HANDLE g_readyEvent = nullptr;

// Hotkeys are only registered while a File Explorer window is in front, so the
// same keys keep working in other apps.
static bool g_keysWanted = false;
static bool g_withExtRegistered = false;
static bool g_noExtRegistered = false;
static bool g_loggedRegisterError = false;
static std::atomic<bool> g_copying{false};

// Open menus that carry our entries, so unloading can close them first.
static std::atomic<int> g_openMenus{0};
static std::atomic<HWND> g_menuOwner{nullptr};
static thread_local bool t_inMenuHook = false;

static UINT ParseModifiers(PCWSTR s, UINT fallback) {
    if (!s) return fallback;
    if (wcscmp(s, L"ctrl_shift") == 0) return MOD_CONTROL | MOD_SHIFT;
    if (wcscmp(s, L"ctrl_alt") == 0) return MOD_CONTROL | MOD_ALT;
    if (wcscmp(s, L"ctrl_alt_shift") == 0) return MOD_CONTROL | MOD_ALT | MOD_SHIFT;
    if (wcscmp(s, L"alt_shift") == 0) return MOD_ALT | MOD_SHIFT;
    if (wcscmp(s, L"win_shift") == 0) return MOD_WIN | MOD_SHIFT;
    return fallback;
}

static UINT ParseKey(PCWSTR s, UINT fallback) {
    if (!s || !*s) return fallback;
    if (!s[1]) {
        wchar_t c = towupper(s[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) return (UINT)c;
        return fallback;
    }
    if (towupper(s[0]) == L'F') {
        int n = _wtoi(s + 1);
        if (n >= 1 && n <= 24) return VK_F1 + (n - 1);
    }
    return fallback;
}

static UINT ReadModifiers(PCWSTR name, UINT fallback) {
    PCWSTR s = Wh_GetStringSetting(name);
    UINT m = ParseModifiers(s, fallback);
    Wh_FreeStringSetting(s);
    return m | MOD_NOREPEAT;
}

static UINT ReadKey(PCWSTR name, UINT fallback) {
    PCWSTR s = Wh_GetStringSetting(name);
    UINT k = ParseKey(s, fallback);
    Wh_FreeStringSetting(s);
    return k;
}

static void LoadSettings() {
    UINT withExtModifiers = ReadModifiers(L"withExtModifier", MOD_CONTROL | MOD_SHIFT);
    UINT withExtKey = ReadKey(L"withExtKey", 'N');
    UINT noExtModifiers = ReadModifiers(L"noExtModifier", MOD_CONTROL | MOD_ALT | MOD_SHIFT);
    UINT noExtKey = ReadKey(L"noExtKey", 'N');

    PCWSTR sep = Wh_GetStringSetting(L"separator");
    std::wstring separator;
    if (sep && wcscmp(sep, L"comma") == 0) separator = L", ";
    else if (sep && wcscmp(sep, L"space") == 0) separator = L" ";
    else separator = L"\r\n";
    Wh_FreeStringSetting(sep);

    bool quoteNames = Wh_GetIntSetting(L"quoteNames") != 0;

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings.withExtModifiers = withExtModifiers;
    g_settings.withExtKey = withExtKey;
    g_settings.noExtModifiers = noExtModifiers;
    g_settings.noExtKey = noExtKey;
    g_settings.separator = std::move(separator);
    g_settings.quoteNames = quoteNames;
    ReleaseSRWLockExclusive(&g_settingsLock);

    g_contextMenu = Wh_GetIntSetting(L"contextMenu") != 0;
}

static bool IsExplorerWindow(HWND hWnd) {
    if (!hWnd) return false;
    wchar_t cls[64];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    return wcscmp(cls, L"CabinetWClass") == 0;
}

static void UnregisterKeys(HWND hWnd) {
    if (g_withExtRegistered) UnregisterHotKey(hWnd, kHotkeyWithExt);
    if (g_noExtRegistered) UnregisterHotKey(hWnd, kHotkeyNoExt);
    g_withExtRegistered = false;
    g_noExtRegistered = false;
}

static void RegisterKeys(HWND hWnd) {
    UnregisterKeys(hWnd);
    g_withExtRegistered = RegisterHotKey(hWnd, kHotkeyWithExt, g_settings.withExtModifiers,
                                         g_settings.withExtKey) != 0;
    if (!g_withExtRegistered && !g_loggedRegisterError) {
        Wh_Log(L"Could not register the copy-with-extension hotkey (error %lu)", GetLastError());
    }
    g_noExtRegistered = RegisterHotKey(hWnd, kHotkeyNoExt, g_settings.noExtModifiers,
                                       g_settings.noExtKey) != 0;
    if (!g_noExtRegistered && !g_loggedRegisterError) {
        Wh_Log(L"Could not register the copy-without-extension hotkey (error %lu)", GetLastError());
    }
    if (!g_withExtRegistered || !g_noExtRegistered) g_loggedRegisterError = true;
}

static void UpdateKeys(HWND hWnd) {
    bool wanted = IsExplorerWindow(GetForegroundWindow());
    if (wanted == g_keysWanted) return;
    g_keysWanted = wanted;
    if (wanted) RegisterKeys(hWnd);
    else UnregisterKeys(hWnd);
}

// Finds the shell browser of the active tab in the given File Explorer window.
// The caller releases the result.
static IShellBrowser* FindShellBrowser(HWND fg) {
    IShellWindows* windows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_IShellWindows,
                                (void**)&windows)) ||
        !windows) {
        return nullptr;
    }

    long count = 0;
    if (FAILED(windows->get_Count(&count))) count = 0;

    // Each tab is its own entry, and all tabs share the frame window. The first
    // tab child of the frame is the active one.
    HWND activeTab = FindWindowExW(fg, nullptr, L"ShellTabWindowClass", nullptr);

    IShellBrowser* best = nullptr;
    int bestRank = 0;
    for (long i = 0; i < count && bestRank < 3; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;
        IDispatch* disp = nullptr;
        if (windows->Item(index, &disp) != S_OK || !disp) {
            if (disp) disp->Release();
            continue;
        }

        IWebBrowserApp* app = nullptr;
        if (SUCCEEDED(disp->QueryInterface(IID_IWebBrowserApp, (void**)&app)) && app) {
            SHANDLE_PTR frame = 0;
            if (SUCCEEDED(app->get_HWND(&frame)) && (HWND)(LONG_PTR)frame == fg) {
                IServiceProvider* sp = nullptr;
                if (SUCCEEDED(app->QueryInterface(IID_IServiceProvider, (void**)&sp)) && sp) {
                    IShellBrowser* browser = nullptr;
                    if (SUCCEEDED(sp->QueryService(kSidTopLevelBrowser, IID_IShellBrowser,
                                                   (void**)&browser)) &&
                        browser) {
                        HWND tab = nullptr;
                        if (FAILED(browser->GetWindow(&tab))) tab = nullptr;
                        int rank = 1;
                        if (activeTab && tab == activeTab) rank = 3;
                        else if (tab && IsWindowVisible(tab)) rank = 2;
                        if (rank > bestRank) {
                            if (best) best->Release();
                            best = browser;
                            bestRank = rank;
                        } else {
                            browser->Release();
                        }
                    }
                    sp->Release();
                }
            }
            app->Release();
        }
        disp->Release();
    }
    windows->Release();
    return best;
}

static void StripExtension(std::wstring& name) {
    size_t dot = name.find_last_of(L'.');
    // A dot at the start is a dotfile, not an extension.
    if (dot != std::wstring::npos && dot > 0) name.resize(dot);
}

static bool ReadSelectedNames(IShellBrowser* browser, bool withExt,
                              std::vector<std::wstring>& names) {
    IShellView* view = nullptr;
    if (FAILED(browser->QueryActiveShellView(&view)) || !view) return false;

    bool ok = false;
    IFolderView2* folderView = nullptr;
    if (SUCCEEDED(view->QueryInterface(IID_IFolderView2, (void**)&folderView)) && folderView) {
        IShellItemArray* items = nullptr;
        if (SUCCEEDED(folderView->GetSelection(FALSE, &items)) && items) {
            DWORD count = 0;
            if (FAILED(items->GetCount(&count))) count = 0;
            for (DWORD i = 0; i < count; i++) {
                IShellItem* item = nullptr;
                if (FAILED(items->GetItemAt(i, &item)) || !item) continue;
                PWSTR display = nullptr;
                // The parsing name is the real file name. It keeps the extension
                // even when File Explorer hides extensions.
                if (SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &display)) &&
                    display) {
                    std::wstring name(display);
                    CoTaskMemFree(display);
                    if (!withExt) {
                        // A zip file reports itself as a folder too, but it's
                        // also a stream. Only real folders keep their full name.
                        SFGAOF attrs = 0;
                        if (FAILED(item->GetAttributes(SFGAO_FOLDER | SFGAO_STREAM, &attrs))) {
                            attrs = 0;
                        }
                        bool isFolder = (attrs & SFGAO_FOLDER) && !(attrs & SFGAO_STREAM);
                        if (!isFolder) StripExtension(name);
                    }
                    if (!name.empty()) names.push_back(std::move(name));
                }
                item->Release();
            }
            items->Release();
            ok = true;
        }
        folderView->Release();
    }
    view->Release();
    return ok;
}

static bool SetClipboardText(HWND owner, const std::wstring& text) {
    // Another program may hold the clipboard for a moment, so try a few times.
    bool opened = false;
    for (int i = 0; i < 5 && !opened; i++) {
        opened = OpenClipboard(owner) != 0;
        if (!opened) Sleep(20);
    }
    if (!opened) {
        Wh_Log(L"Could not open the clipboard (error %lu)", GetLastError());
        return false;
    }

    bool ok = false;
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (mem) {
        void* p = GlobalLock(mem);
        if (p) {
            memcpy(p, text.c_str(), bytes);
            GlobalUnlock(mem);
            if (EmptyClipboard() && SetClipboardData(CF_UNICODETEXT, mem)) {
                // The clipboard owns the memory now.
                mem = nullptr;
                ok = true;
            }
        }
        if (mem) GlobalFree(mem);
    }
    CloseClipboard();
    return ok;
}

// Copies the selected names of one shell browser. Both the hotkeys and the
// menu entries end up here.
static void CopySelection(IShellBrowser* browser, HWND clipboardOwner, bool withExt) {
    // COM calls can pump messages, so a second request could land here while
    // the first one is still running.
    if (g_copying.exchange(true)) return;

    std::vector<std::wstring> names;
    if (!ReadSelectedNames(browser, withExt, names)) {
        Wh_Log(L"Could not read the selection from this window");
    }

    if (!names.empty()) {
        AcquireSRWLockShared(&g_settingsLock);
        std::wstring separator = g_settings.separator;
        bool quoteNames = g_settings.quoteNames;
        ReleaseSRWLockShared(&g_settingsLock);

        std::wstring text;
        for (size_t i = 0; i < names.size(); i++) {
            if (i > 0) text += separator;
            if (quoteNames) text += L'"';
            text += names[i];
            if (quoteNames) text += L'"';
        }
        if (SetClipboardText(clipboardOwner, text)) {
            Wh_Log(L"Copied %zu names %s", names.size(),
                   withExt ? L"with extensions" : L"without extensions");
        }
    }

    g_copying = false;
}

static void CopyNames(HWND hWnd, bool withExt) {
    HWND fg = GetForegroundWindow();
    if (!IsExplorerWindow(fg)) return;

    IShellBrowser* browser = FindShellBrowser(fg);
    if (browser) {
        CopySelection(browser, hWnd, withExt);
        browser->Release();
    } else {
        Wh_Log(L"Could not find the shell view for this window");
    }
}

// Returns the shell browser behind a menu owner window when that window is the
// file view of a File Explorer window. Anything else, like the navigation pane,
// the desktop, or a file dialog, returns null. The caller releases the result.
static IShellBrowser* BrowserForFileView(HWND hWnd) {
    if (!hWnd || !IsExplorerWindow(GetAncestor(hWnd, GA_ROOT))) return nullptr;

    bool inFileView = false;
    for (HWND h = hWnd; h; h = GetAncestor(h, GA_PARENT)) {
        wchar_t cls[64];
        if (!GetClassNameW(h, cls, ARRAYSIZE(cls))) return nullptr;
        if (!inFileView) {
            if (wcscmp(cls, L"SHELLDLL_DefView") == 0) inFileView = true;
            continue;
        }
        if (wcscmp(cls, L"ShellTabWindowClass") == 0) {
            // The tab and the menu run on the same thread, so the pointer is
            // safe to use here. Take a reference while we hold it.
            IShellBrowser* browser =
                (IShellBrowser*)SendMessageW(h, kMsgGetShellBrowser, 0, 0);
            if (browser) browser->AddRef();
            return browser;
        }
        if (wcscmp(cls, L"CabinetWClass") == 0) break;
    }
    return nullptr;
}

static bool HasSelection(IShellBrowser* browser) {
    IShellView* view = nullptr;
    if (FAILED(browser->QueryActiveShellView(&view)) || !view) return false;
    int count = 0;
    IFolderView* folderView = nullptr;
    if (SUCCEEDED(view->QueryInterface(IID_IFolderView, (void**)&folderView)) && folderView) {
        if (FAILED(folderView->ItemCount(SVGIO_SELECTION, &count))) count = 0;
        folderView->Release();
    }
    view->Release();
    return count > 0;
}

static bool IsMenuIdFree(HMENU menu, UINT id) {
    // GetMenuState searches submenus too and returns -1 for an unknown ID.
    return GetMenuState(menu, id, MF_BYCOMMAND) == (UINT)-1;
}

// Picks three free IDs: two entries and our separator. Returns 0 if none fit.
static UINT PickMenuIdBase(HMENU menu) {
    for (int i = 0; i < kMenuIdTries; i++) {
        UINT base = kMenuIdBase + (UINT)i * 3;
        if (IsMenuIdFree(menu, base) && IsMenuIdFree(menu, base + 1) &&
            IsMenuIdFree(menu, base + 2)) {
            return base;
        }
    }
    return 0;
}

static bool HasCheckedItem(HMENU menu, int count) {
    for (int i = 0; i < count; i++) {
        UINT state = GetMenuState(menu, (UINT)i, MF_BYPOSITION);
        if (state != (UINT)-1 && (state & MF_CHECKED)) return true;
    }
    return false;
}

static bool IsOnColumnHeader(int x, int y) {
    POINT pt{x, y};
    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) return false;
    wchar_t cls[64];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    return wcscmp(cls, L"SysHeader32") == 0;
}

static bool InsertItem(HMENU menu, UINT pos, UINT id, PCWSTR label) {
    MENUITEMINFOW mii{};
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_ID | MIIM_FTYPE | (label ? MIIM_STRING : 0);
    mii.fType = label ? MFT_STRING : MFT_SEPARATOR;
    mii.wID = id;
    mii.dwTypeData = (PWSTR)label;
    return InsertMenuItemW(menu, pos, TRUE, &mii) != 0;
}

// Adds our entries above the last item, which is Properties on a file menu.
// Returns the ID base, or 0 when the menu was left alone.
static UINT AddMenuEntries(HMENU menu, HWND owner, UINT flags, int x, int y,
                           IShellBrowser** browserOut) {
    *browserOut = nullptr;
    if (!g_contextMenu || !menu || !IsMenu(menu)) return 0;
    // Without TPM_RETURNCMD the pick goes to the owner as WM_COMMAND, and the
    // shell would get an ID it doesn't know.
    if (!(flags & TPM_RETURNCMD)) return 0;

    // Cheap checks first. The empty-space menu has Refresh.
    if (!IsMenuIdFree(menu, kRefreshCmd)) return 0;
    int count = GetMenuItemCount(menu);
    if (count < 0) return 0;
    // A menu for files and folders usually has a default action, like Open,
    // and the column header menu has none. File Explorer drops Open when more
    // than 15 files are selected, so a menu without one still counts unless
    // it looks like the column header menu: check marks on the shown columns,
    // or opened over the header itself.
    if (GetMenuDefaultItem(menu, FALSE, GMDI_USEDISABLED) == (UINT)-1 &&
        (HasCheckedItem(menu, count) || IsOnColumnHeader(x, y))) {
        return 0;
    }

    IShellBrowser* browser = BrowserForFileView(owner);
    if (!browser) return 0;
    if (!HasSelection(browser)) {
        browser->Release();
        return 0;
    }

    UINT base = PickMenuIdBase(menu);
    if (!base) {
        browser->Release();
        return 0;
    }

    UINT pos = count > 0 ? (UINT)count - 1 : 0;
    bool ok = InsertItem(menu, pos, base, kMenuLabelWithExt) &&
              InsertItem(menu, pos + 1, base + 1, kMenuLabelNoExt) &&
              InsertItem(menu, pos + 2, base + 2, nullptr);
    if (!ok) {
        // Undo whatever went in.
        for (UINT id = base; id < base + 3; id++) DeleteMenu(menu, id, MF_BYCOMMAND);
        browser->Release();
        return 0;
    }

    *browserOut = browser;
    return base;
}

static void RemoveMenuEntries(HMENU menu, UINT base) {
    if (!IsMenu(menu)) return;
    for (UINT id = base; id < base + 3; id++) DeleteMenu(menu, id, MF_BYCOMMAND);
}

// Runs the menu with our entries added, handles a pick of ours, and puts the
// menu back the way the shell built it.
template <typename Track>
static BOOL TrackWithEntries(HMENU menu, UINT flags, int x, int y, HWND owner, Track track) {
    // TrackPopupMenu may call TrackPopupMenuEx, so only the outer call adds.
    if (t_inMenuHook) return track();

    IShellBrowser* browser = nullptr;
    UINT base = AddMenuEntries(menu, owner, flags, x, y, &browser);
    if (!base) return track();

    t_inMenuHook = true;
    g_menuOwner = owner;
    g_openMenus++;
    BOOL result = track();
    t_inMenuHook = false;

    RemoveMenuEntries(menu, base);

    UINT picked = (UINT)result;
    if (picked == base || picked == base + 1) {
        CopySelection(browser, owner, picked == base);
        // The shell gets no command, as if the menu was closed.
        result = 0;
    }
    browser->Release();
    // Lowered last, so unloading also waits for the copy above.
    g_openMenus--;
    return result;
}

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
static TrackPopupMenuEx_t TrackPopupMenuEx_Original;

static BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd,
                                         LPTPMPARAMS lptpm) {
    return TrackWithEntries(hMenu, uFlags, x, y, hWnd, [&] {
        return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
    });
}

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
static TrackPopupMenu_t TrackPopupMenu_Original;

static BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved,
                                       HWND hWnd, const RECT* prcRect) {
    return TrackWithEntries(hMenu, uFlags, x, y, hWnd, [&] {
        return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    });
}

static LRESULT CALLBACK MsgWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == kTimerId) UpdateKeys(hWnd);
            return 0;
        case WM_HOTKEY:
            if (wParam == kHotkeyWithExt) CopyNames(hWnd, true);
            else if (wParam == kHotkeyNoExt) CopyNames(hWnd, false);
            return 0;
        case WM_APP_RELOAD:
            // Drop the old keys. The next timer tick registers the new ones if
            // File Explorer is in front.
            UnregisterKeys(hWnd);
            g_keysWanted = false;
            g_loggedRegisterError = false;
            LoadSettings();
            UpdateKeys(hWnd);
            return 0;
        case WM_APP_QUIT:
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            KillTimer(hWnd, kTimerId);
            UnregisterKeys(hWnd);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static DWORD WINAPI ThreadProc(LPVOID) {
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool comReady = SUCCEEDED(hrCom);
    if (!comReady) Wh_Log(L"CoInitializeEx failed (0x%08lX)", (unsigned long)hrCom);

    WNDCLASSW wc{};
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WindhawkBulkCopyFileAndFolderNamesWnd";
    RegisterClassW(&wc);

    HWND wnd = nullptr;
    if (comReady) {
        wnd = CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                              wc.hInstance, nullptr);
        if (wnd) SetTimer(wnd, kTimerId, kTimerMs, nullptr);
        else Wh_Log(L"Could not create the message window (error %lu)", GetLastError());
    }
    g_msgWnd = wnd;
    SetEvent(g_readyEvent);

    if (wnd) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    g_msgWnd = nullptr;
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    if (comReady) CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    // A failed hook only costs the menu entries. The hotkeys still work.
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook,
                                        &TrackPopupMenuEx_Original)) {
        Wh_Log(L"Could not hook TrackPopupMenuEx, menu entries are off");
    }
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenu_Hook,
                                        &TrackPopupMenu_Original)) {
        Wh_Log(L"Could not hook TrackPopupMenu");
    }

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) return FALSE;
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, &g_threadId);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }
    WaitForSingleObject(g_readyEvent, 3000);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    HWND wnd = g_msgWnd;
    if (wnd) PostMessageW(wnd, WM_APP_RELOAD, 0, 0);
}

void Wh_ModBeforeUninit() {
    // A menu with our entries may still be open. Close it so the hook returns
    // before the mod's code goes away.
    g_contextMenu = false;
    for (int i = 0; i < 100 && g_openMenus > 0; i++) {
        HWND owner = g_menuOwner;
        if (owner) PostMessageW(owner, WM_CANCELMODE, 0, 0);
        Sleep(20);
    }
}

void Wh_ModUninit() {
    if (g_thread) {
        // Post to the window, not the thread. A thread message can be lost while
        // a COM call runs its own message loop.
        HWND wnd = g_msgWnd;
        if (wnd) PostMessageW(wnd, WM_APP_QUIT, 0, 0);
        else PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}
