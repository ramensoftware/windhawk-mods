// ==WindhawkMod==
// @id              bulk-copy-file-and-folder-names
// @name            Bulk Copy File and Folder Names
// @description     Copy the names of one or many selected files and folders in File Explorer, not the paths, from the right-click menu or with optional hotkeys
// @version         0.3.0
// @author          Shaerif
// @github          https://github.com/Shaerif
// @include         explorer.exe
// @compilerOptions -luser32 -lole32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Bulk Copy File and Folder Names
Windows has Copy as path (Ctrl+Shift+C), but it gives you full paths in quotes,
and often you only want the names. This mod copies the names of the files and
folders you select in File Explorer as plain text, one per line. It works on one
item or on many at once. A selection can mix files and folders.

## Right-click menu
Right-click a file, a folder, or a selection of many files and folders in a File
Explorer window. The classic menu gets two entries, Copy name and Copy name
without extension. On Windows 11 they're in the menu you get from Show more
options or Shift+right-click. The entries are on by default, and you can turn
them off in the settings.

Copy name without extension keeps folder names whole, so `sub.folder` stays
`sub.folder`, and `a.tar.gz` becomes `a.tar`. A name that starts with a dot, like
`.gitignore`, stays whole.

## Hotkeys
The hotkeys are off until you turn them on, so a fresh install takes no key away
from File Explorer. To turn them on, open the mod's Settings tab in Windhawk,
tick Hotkeys, and save. The defaults are Ctrl+Shift+Y for names with extensions
and Ctrl+Shift+U for names without. You can change both keys there too.

A key is handled inside the File Explorer window that has the focus, so it never
reaches other programs. It does nothing while you type in a rename box, the
address bar or the search box, and it does nothing when no file or folder is
selected. In both cases the key goes to File Explorer as usual.

If you pick a key that File Explorer already uses, like Ctrl+Shift+N for New
folder, the mod wins while the hotkeys are on and something is selected.

## Settings
- Hotkeys: turn the two key combinations on. Off by default.
- Modifiers and key for each copy action. Ctrl+Alt and Ctrl+Alt+Shift also
  match AltGr, so skip them if your keyboard layout types letters with AltGr.
- Separator: a new line, a comma, or a space between names.
- Quote names: put each name in double quotes.
- Right-click menu: show the two menu entries. On by default.

## Limits
- The keys and menu entries don't work on the desktop or in file dialogs. Only
  File Explorer windows count.
- The Windows 11 compact menu can't take new entries from a mod. Use Show more
  options.
- The menu entries don't show on the column header menu, the menu you get from
  empty space in a folder, or the menu you get after a drag with the right
  mouse button.
- Items that aren't files or folders, like drives in This PC, give their
  internal name.
- If nothing is selected, nothing happens.

## Tutorial
1. Select the files and folders in a File Explorer window. A mix of both is
   fine.
   ![A File Explorer window with two folders and five files selected in the demo-names folder](https://raw.githubusercontent.com/Shaerif/windhawk-mods/main/docs/screenshots/bulk-copy-step-1-select.png)
2. Hold Shift and right-click the selection. Pick Copy name or Copy name
   without extension.
   ![The lower part of the classic right-click menu, with Copy name and Copy name without extension in their own group above Properties](https://raw.githubusercontent.com/Shaerif/windhawk-mods/main/docs/screenshots/bulk-copy-step-4-right-click-menu.png)
3. Paste. Copy name gives you every name with its extension, one per line.
   ![The selected names pasted into a text window, one per line, with their extensions](https://raw.githubusercontent.com/Shaerif/windhawk-mods/main/docs/screenshots/bulk-copy-step-2-names-with-extensions.png)
4. Copy name without extension drops the extension from files and keeps the
   full name of folders.
   ![The same names pasted again without extensions, while the folder Backup.2025 keeps its full name](https://raw.githubusercontent.com/Shaerif/windhawk-mods/main/docs/screenshots/bulk-copy-step-3-names-without-extensions.png)
5. If you'd rather use the keyboard, tick Hotkeys in the settings. Ctrl+Shift+Y
   then does what Copy name does, and Ctrl+Shift+U does what Copy name without
   extension does.

### Gotchas
- In step 2, the compact Windows 11 menu can't show the entries. Use
  Shift+right-click or Show more options.
- In step 4, only the last extension goes, so `archive.tar.gz` becomes
  `archive.tar`.
- In step 5, a key does nothing while you type in a rename box, the address bar
  or the search box, and nothing while no item is selected. File Explorer gets
  the key instead.

## Credits
The way the hotkeys are caught, a hook on TranslateAcceleratorW that steps
aside while a text box has the focus, follows the approach of the Explorer
Custom Shortcuts mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkeys: false
  $name: Hotkeys
  $description: Turn on the two key combinations below. They're off until you do, so the mod takes no key away from File Explorer. A key only acts in a File Explorer window, and never while you type in a rename box, the address bar or the search box.
- withExtModifier: ctrl_shift
  $name: Modifiers for copy names with extension
  $description: Ctrl+Alt and Ctrl+Alt+Shift also match AltGr. On a keyboard layout that types letters or symbols with AltGr, those two choices block that typing while File Explorer has the focus.
  $options:
  - ctrl_shift: Ctrl+Shift
  - ctrl_alt: Ctrl+Alt
  - ctrl_alt_shift: Ctrl+Alt+Shift
  - alt_shift: Alt+Shift
  - win_shift: Win+Shift
- withExtKey: Y
  $name: Key for copy names with extension
  $description: A letter, a digit, or F1 to F24
- noExtModifier: ctrl_shift
  $name: Modifiers for copy names without extension
  $description: Ctrl+Alt and Ctrl+Alt+Shift also match AltGr. On a keyboard layout that types letters or symbols with AltGr, those two choices block that typing while File Explorer has the focus.
  $options:
  - ctrl_shift: Ctrl+Shift
  - ctrl_alt: Ctrl+Alt
  - ctrl_alt_shift: Ctrl+Alt+Shift
  - alt_shift: Alt+Shift
  - win_shift: Win+Shift
- noExtKey: U
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

#include <shlobj.h>
#include <shobjidl.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <cwctype>
#include <string>
#include <vector>

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

// Modifier sets for the hotkeys, as MOD_CONTROL, MOD_SHIFT, MOD_ALT and
// MOD_WIN bits. Keys are virtual key codes.
struct {
    UINT withExtModifiers;
    UINT withExtKey;
    UINT noExtModifiers;
    UINT noExtKey;
    std::wstring separator;
    bool quoteNames;
} g_settings;

// The hooks read the settings on File Explorer's own threads, so writes and
// those reads go through this lock.
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static std::atomic<bool> g_hotkeys{false};
static std::atomic<bool> g_contextMenu{true};
static std::atomic<bool> g_unloading{false};

// Open menus that carry our entries, plus hotkey copies still running, so
// unloading can close the menus and wait for all of them first.
static std::atomic<int> g_openMenus{0};
static std::atomic<HWND> g_menuOwner{nullptr};
static thread_local bool t_inMenuHook = false;
static thread_local bool t_copying = false;

static UINT ParseModifiers(PCWSTR s, UINT fallback) {
    if (wcscmp(s, L"ctrl_shift") == 0) return MOD_CONTROL | MOD_SHIFT;
    if (wcscmp(s, L"ctrl_alt") == 0) return MOD_CONTROL | MOD_ALT;
    if (wcscmp(s, L"ctrl_alt_shift") == 0) return MOD_CONTROL | MOD_ALT | MOD_SHIFT;
    if (wcscmp(s, L"alt_shift") == 0) return MOD_ALT | MOD_SHIFT;
    if (wcscmp(s, L"win_shift") == 0) return MOD_WIN | MOD_SHIFT;
    return fallback;
}

static UINT ParseKey(PCWSTR s, UINT fallback) {
    if (!*s) return fallback;
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

static void LoadSettings() {
    using WindhawkUtils::StringSetting;

    UINT withExtModifiers =
        ParseModifiers(StringSetting::make(L"withExtModifier"), MOD_CONTROL | MOD_SHIFT);
    UINT withExtKey = ParseKey(StringSetting::make(L"withExtKey"), 'Y');
    UINT noExtModifiers =
        ParseModifiers(StringSetting::make(L"noExtModifier"), MOD_CONTROL | MOD_SHIFT);
    UINT noExtKey = ParseKey(StringSetting::make(L"noExtKey"), 'U');

    StringSetting sep = StringSetting::make(L"separator");
    std::wstring separator = L"\r\n";
    if (wcscmp(sep, L"comma") == 0) separator = L", ";
    else if (wcscmp(sep, L"space") == 0) separator = L" ";

    bool quoteNames = Wh_GetIntSetting(L"quoteNames") != 0;

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings.withExtModifiers = withExtModifiers;
    g_settings.withExtKey = withExtKey;
    g_settings.noExtModifiers = noExtModifiers;
    g_settings.noExtKey = noExtKey;
    g_settings.separator = std::move(separator);
    g_settings.quoteNames = quoteNames;
    ReleaseSRWLockExclusive(&g_settingsLock);

    g_hotkeys = Wh_GetIntSetting(L"hotkeys") != 0;
    g_contextMenu = Wh_GetIntSetting(L"contextMenu") != 0;
}

static bool HasClass(HWND hWnd, PCWSTR name) {
    wchar_t cls[64];
    return hWnd && GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, name) == 0;
}

static bool IsExplorerWindow(HWND hWnd) {
    return HasClass(hWnd, L"CabinetWClass");
}

// Returns the shell browser of a tab window. The tab must live on the calling
// thread, so the pointer it hands out stays in this apartment. The caller
// releases the result.
static IShellBrowser* BrowserForTab(HWND tab) {
    if (!tab || GetWindowThreadProcessId(tab, nullptr) != GetCurrentThreadId()) return nullptr;
    IShellBrowser* browser = (IShellBrowser*)SendMessageW(tab, kMsgGetShellBrowser, 0, 0);
    if (browser) browser->AddRef();
    return browser;
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
        Wh_Log(L"Couldn't open the clipboard (error %lu)", GetLastError());
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
// menu entries end up here. Returns true when names went to the clipboard.
static bool CopySelection(IShellBrowser* browser, HWND clipboardOwner, bool withExt) {
    // A shell call may pump messages, so a second request could land here
    // while the first one is still running on this thread.
    if (t_copying) return false;
    t_copying = true;

    bool copied = false;
    std::vector<std::wstring> names;
    if (!ReadSelectedNames(browser, withExt, names)) {
        Wh_Log(L"Couldn't read the selection from this window");
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
        copied = SetClipboardText(clipboardOwner, text);
        if (copied) {
            Wh_Log(L"Copied %zu names %s", names.size(),
                   withExt ? L"with extensions" : L"without extensions");
        }
    }

    t_copying = false;
    return copied;
}

// Hotkeys. A key press reaches us through TranslateAcceleratorW, which File
// Explorer calls on the window's own thread for every key message. The match
// happens there, and the copy runs in the same apartment as the view.

static bool ModifiersHeld(UINT wanted) {
    UINT held = 0;
    if (GetKeyState(VK_CONTROL) & 0x8000) held |= MOD_CONTROL;
    if (GetKeyState(VK_SHIFT) & 0x8000) held |= MOD_SHIFT;
    if (GetKeyState(VK_MENU) & 0x8000) held |= MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) held |= MOD_WIN;
    return held == wanted;
}

// 1 for the names-with-extension key, 2 for the names-without-extension key,
// 0 for anything else.
static int MatchHotkey(UINT vk) {
    AcquireSRWLockShared(&g_settingsLock);
    UINT withExtModifiers = g_settings.withExtModifiers;
    UINT withExtKey = g_settings.withExtKey;
    UINT noExtModifiers = g_settings.noExtModifiers;
    UINT noExtKey = g_settings.noExtKey;
    ReleaseSRWLockShared(&g_settingsLock);

    if (vk == withExtKey && ModifiersHeld(withExtModifiers)) return 1;
    if (vk == noExtKey && ModifiersHeld(noExtModifiers)) return 2;
    return 0;
}

// True while the user is typing somewhere in this window: the rename box, the
// address bar, the search box, or any other text field. The key belongs to
// that field then.
static bool IsTextFieldFocused(HWND root, HWND focus) {
    if (!focus) return false;

    // Text fields in File Explorer are an Edit control, a rich edit, or sit
    // inside a wrapper with Edit in its class name, like the search box.
    for (HWND h = focus; h && h != root; h = GetAncestor(h, GA_PARENT)) {
        wchar_t cls[64];
        if (!GetClassNameW(h, cls, ARRAYSIZE(cls))) break;
        if (wcsstr(cls, L"Edit") || wcsstr(cls, L"Address Band")) return true;
    }

    // Whatever its class, a window with a caret takes typed text.
    GUITHREADINFO info{};
    info.cbSize = sizeof(info);
    if (GetGUIThreadInfo(GetCurrentThreadId(), &info) && info.hwndCaret) return true;
    return false;
}

// The tab that holds the given window. Falls back to the first tab of the
// frame when the window sits outside the tabs, like the toolbar area.
static HWND TabForWindow(HWND hWnd, HWND root) {
    for (HWND h = hWnd; h && h != root; h = GetAncestor(h, GA_PARENT)) {
        if (HasClass(h, L"ShellTabWindowClass")) return h;
    }
    return FindWindowExW(root, nullptr, L"ShellTabWindowClass", nullptr);
}

// Returns true when the key was one of ours and names were copied. In every
// other case the key is left to File Explorer.
static bool HandleHotkey(const MSG* msg) {
    int which = MatchHotkey((UINT)msg->wParam);
    if (!which) return false;

    HWND root = GetAncestor(msg->hwnd, GA_ROOT);
    if (!IsExplorerWindow(root)) return false;
    if (IsTextFieldFocused(root, msg->hwnd)) return false;

    IShellBrowser* browser = BrowserForTab(TabForWindow(msg->hwnd, root));
    if (!browser) return false;
    bool copied = CopySelection(browser, root, which == 1);
    browser->Release();
    if (!copied) return false;

    // With Alt held, File Explorer waits for the rest of a menu shortcut. A
    // focus round trip drops it out of that state.
    if (msg->message == WM_SYSKEYDOWN && (GetKeyState(VK_MENU) & 0x8000)) {
        HWND focus = GetFocus();
        SetFocus(root);
        if (focus && IsWindow(focus)) SetFocus(focus);
    }
    return true;
}

using TranslateAcceleratorW_t = decltype(&TranslateAcceleratorW);
static TranslateAcceleratorW_t TranslateAcceleratorW_Original;

// Keeps g_openMenus raised for one scope, so unloading waits for a hotkey copy
// the same way it waits for a menu.
struct BusyGuard {
    BusyGuard() { g_openMenus++; }
    ~BusyGuard() { g_openMenus--; }
};

static int WINAPI TranslateAcceleratorW_Hook(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg) {
    if (lpMsg && g_hotkeys && !g_unloading &&
        (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN) &&
        // Bit 30 marks an auto repeat. One press copies once.
        !(lpMsg->lParam & (1 << 30))) {
        BusyGuard busy;
        // Checked again with the count raised, so an unload that starts in
        // between either sees the count or is seen here.
        if (!g_unloading && HandleHotkey(lpMsg)) return 1;
    }
    return TranslateAcceleratorW_Original(hWnd, hAccTable, lpMsg);
}

// Right-click menu.

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
        if (wcscmp(cls, L"ShellTabWindowClass") == 0) return BrowserForTab(h);
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

// The menu after a drag with the right mouse button (Copy here, Move here,
// Create shortcuts here, Cancel) ends with a Cancel item whose ID is 0, so a
// pick of it looks like a closed menu. The shell's own menu resources all end
// that way. A file menu ends with Properties, which has a real ID.
static bool EndsWithCancel(HMENU menu, int count) {
    if (count < 1) return false;
    UINT state = GetMenuState(menu, (UINT)count - 1, MF_BYPOSITION);
    if (state == (UINT)-1 || (state & MF_SEPARATOR)) return false;
    return GetMenuItemID(menu, count - 1) == 0;
}

static bool IsOnColumnHeader(int x, int y) {
    POINT pt{x, y};
    return HasClass(WindowFromPoint(pt), L"SysHeader32");
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
    // Most menus in explorer.exe belong to the taskbar, the tray or the
    // desktop. Only a File Explorer window gets a closer look.
    if (!owner || !IsExplorerWindow(GetAncestor(owner, GA_ROOT))) return 0;

    // Cheap checks first. The empty-space menu has Refresh.
    if (!IsMenuIdFree(menu, kRefreshCmd)) return 0;
    int count = GetMenuItemCount(menu);
    if (count < 0) return 0;
    if (EndsWithCancel(menu, count)) return 0;
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

BOOL Wh_ModInit() {
    LoadSettings();

    // Each hook only costs its own feature when it fails.
    if (!WindhawkUtils::SetFunctionHook(TranslateAcceleratorW, TranslateAcceleratorW_Hook,
                                        &TranslateAcceleratorW_Original)) {
        Wh_Log(L"Couldn't hook TranslateAcceleratorW, so the hotkeys are off");
    }
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook,
                                        &TrackPopupMenuEx_Original)) {
        Wh_Log(L"Couldn't hook TrackPopupMenuEx, so the menu entries are off");
    }
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenu_Hook,
                                        &TrackPopupMenu_Original)) {
        Wh_Log(L"Couldn't hook TrackPopupMenu");
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    // No new work from here on. The hooks pass everything through.
    g_unloading = true;
    g_hotkeys = false;
    g_contextMenu = false;

    // A menu with our entries may still be open, or a hotkey copy may still be
    // running. Close the menu and wait, so every hook returns before the mod's
    // code goes away.
    for (int i = 0; i < 100 && g_openMenus > 0; i++) {
        HWND owner = g_menuOwner;
        if (owner) PostMessageW(owner, WM_CANCELMODE, 0, 0);
        Sleep(20);
    }
}
