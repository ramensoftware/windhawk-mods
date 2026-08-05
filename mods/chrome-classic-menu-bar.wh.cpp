// ==WindhawkMod==
// @id              chrome-classic-menu-bar
// @name            Chrome Classic Menu Bar
// @description     Adds a native File/Edit/View/History/Bookmarks/Tools/Help menu bar to Google Chrome, with switchable dark mode
// @version         1.0.0
// @author          Gokhan
// @github          https://github.com/GokhanGerkz
// @include         chrome.exe
// @compilerOptions -lcomctl32 -luser32 -lgdi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Chrome Classic Menu Bar v1.0.0

Standalone native Win32 menu bar for Google Chrome.

Included:
  - File/Edit/View/History/Bookmarks/Tools/Help menus.
  - Dark owner-drawn menu theme.
  - Native Chromium frame integration adapted from
    Titlebar For Everyone by Ingan121.
  - Reliable new-tab navigation for chrome:// pages.
  - Automatic startup attachment through the Chrome window-creation hook.
  - Windhawk settings for dark menus and native-frame ownership.

Compatibility:
  - If Titlebar For Everyone is enabled for Chrome, disable
    "Manage Chrome native frame" in this mod so only one mod owns the frame.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- darkMenu: true
  $name: Dark menu mode
  $description: Use a dark theme for the menu bar and pop-up menus
- manageNativeFrame: true
  $name: Manage Chrome native frame
  $description: Disable this if another mod, such as Titlebar For Everyone, already manages Chrome's native frame
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <algorithm>
#include <commctrl.h>
#include <uxtheme.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <atomic>
#include <cwchar>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

namespace {

enum CommandId : UINT {
    CMD_FILE_NEW_TAB = 10001,
    CMD_FILE_NEW_WINDOW,
    CMD_FILE_NEW_INCOGNITO,
    CMD_FILE_OPEN,
    CMD_FILE_SAVE,
    CMD_FILE_PRINT,
    CMD_FILE_CLOSE_TAB,
    CMD_FILE_EXIT,

    CMD_EDIT_UNDO = 10101,
    CMD_EDIT_REDO,
    CMD_EDIT_CUT,
    CMD_EDIT_COPY,
    CMD_EDIT_PASTE,
    CMD_EDIT_SELECT_ALL,
    CMD_EDIT_FIND,

    CMD_VIEW_RELOAD = 10201,
    CMD_VIEW_HARD_RELOAD,
    CMD_VIEW_ZOOM_IN,
    CMD_VIEW_ZOOM_OUT,
    CMD_VIEW_ZOOM_RESET,
    CMD_VIEW_FULLSCREEN,
    CMD_VIEW_DEVTOOLS,
    CMD_VIEW_SOURCE,
    CMD_VIEW_DARK_MENU,

    CMD_HISTORY_BACK = 10301,
    CMD_HISTORY_FORWARD,
    CMD_HISTORY_HOME,
    CMD_HISTORY_PAGE,
    CMD_TOOLS_DOWNLOADS,

    CMD_BOOKMARK_THIS_TAB = 10401,
    CMD_BOOKMARK_ALL_TABS,
    CMD_BOOKMARK_MANAGER,
    CMD_BOOKMARK_TOGGLE_BAR,

    CMD_TOOLS_EXTENSIONS = 10501,
    CMD_TOOLS_TASK_MANAGER,
    CMD_TOOLS_CLEAR_DATA,
    CMD_TOOLS_SETTINGS,

    CMD_HELP_HELP = 10601,
    CMD_HELP_ABOUT,
};

struct MenuItemData {
    UINT id = 0;
    std::wstring text;
    bool separator = false;
    bool topLevel = false;
};

struct WindowState {
    HMENU menu = nullptr;
    std::vector<HMENU> allMenus;
    std::vector<MenuItemData*> itemData;
};

struct BuiltMenu {
    HMENU bar = nullptr;
    std::vector<HMENU> allMenus;
    std::vector<MenuItemData*> itemData;
};


SRWLOCK g_stateLock = SRWLOCK_INIT;
std::unordered_map<HWND, WindowState> g_windows;
std::atomic<bool> g_darkMode{true};
std::atomic<bool> g_manageNativeFrame{true};

HBRUSH g_darkBackgroundBrush = nullptr;
HBRUSH g_darkHighlightBrush = nullptr;
HBRUSH g_darkSeparatorBrush = nullptr;
HFONT g_menuFont = nullptr;

constexpr COLORREF kDarkBackground = RGB(32, 33, 36);
constexpr COLORREF kDarkHighlight = RGB(60, 64, 67);
constexpr COLORREF kDarkText = RGB(232, 234, 237);
constexpr COLORREF kDarkDisabled = RGB(154, 160, 166);
constexpr COLORREF kDarkSeparator = RGB(95, 99, 104);

bool IsChromeBrowserFrame(HWND hwnd);

void EnableNativeChromeFrame(HWND hwnd) {
    if (!g_manageNativeFrame.load() || !IsChromeBrowserFrame(hwnd)) {
        return;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR requiredStyle = WS_CAPTION | WS_SYSMENU;
    if ((style & requiredStyle) == requiredStyle) {
        return;
    }

    SetWindowLongPtrW(hwnd, GWL_STYLE, style | requiredStyle);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE |
                     SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

bool IsChromeBrowserFrame(HWND hwnd) {
    if (!hwnd || GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return false;
    }

    wchar_t className[128]{};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) ||
        wcsncmp(className, L"Chrome_WidgetWin_", 17) != 0) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    return (style & WS_CAPTION) != 0;
}

bool IsMainChromeWindow(HWND hwnd) {
    if (!IsChromeBrowserFrame(hwnd)) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid == GetCurrentProcessId();
}

UINT GetDpiForWindowSafe(HWND hwnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static auto fn = reinterpret_cast<GetDpiForWindow_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    return fn ? fn(hwnd) : 96;
}

int ScaleForDpi(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi), 96);
}

void EnsureMenuFont(UINT dpi) {
    if (g_menuFont) {
        DeleteObject(g_menuFont);
        g_menuFont = nullptr;
    }

    NONCLIENTMETRICSW ncm{};
    ncm.cbSize = sizeof(ncm);
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0)) {
        ncm.lfMenuFont.lfHeight = MulDiv(ncm.lfMenuFont.lfHeight, static_cast<int>(dpi), 96);
        g_menuFont = CreateFontIndirectW(&ncm.lfMenuFont);
    }

    if (!g_menuFont) {
        g_menuFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    }
}

MenuItemData* NewItem(BuiltMenu& built, UINT id, const wchar_t* text,
                      bool separator = false, bool topLevel = false) {
    auto* data = new MenuItemData;
    data->id = id;
    data->text = text ? text : L"";
    data->separator = separator;
    data->topLevel = topLevel;
    built.itemData.push_back(data);
    return data;
}

bool AddOwnerDrawItem(BuiltMenu& built, HMENU menu, UINT id,
                      const wchar_t* text) {
    MenuItemData* data = NewItem(built, id, text);
    return AppendMenuW(menu, MF_OWNERDRAW, id,
                       reinterpret_cast<LPCWSTR>(data)) != FALSE;
}

bool AddOwnerDrawSeparator(BuiltMenu& built, HMENU menu) {
    MenuItemData* data = NewItem(built, 0, L"", true);
    return AppendMenuW(menu, MF_OWNERDRAW | MF_SEPARATOR, 0,
                       reinterpret_cast<LPCWSTR>(data)) != FALSE;
}

bool AddOwnerDrawPopup(BuiltMenu& built, HMENU bar, HMENU popup,
                       const wchar_t* text) {
    MenuItemData* data = NewItem(built, 0, text, false, true);
    return AppendMenuW(bar, MF_OWNERDRAW | MF_POPUP,
                       reinterpret_cast<UINT_PTR>(popup),
                       reinterpret_cast<LPCWSTR>(data)) != FALSE;
}

void SetMenuBackground(HMENU menu, HBRUSH brush) {
    MENUINFO mi{};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND;
    mi.hbrBack = brush;
    SetMenuInfo(menu, &mi);
}

void ApplyThemeToBuiltMenu(BuiltMenu& built) {
    HBRUSH background = g_darkMode.load()
        ? g_darkBackgroundBrush
        : GetSysColorBrush(COLOR_MENU);

    for (HMENU menu : built.allMenus) {
        SetMenuBackground(menu, background);
    }
}

BuiltMenu BuildMenuBar() {
    BuiltMenu built;
    built.bar = CreateMenu();
    if (!built.bar) {
        return built;
    }

    HMENU file = CreatePopupMenu();
    HMENU edit = CreatePopupMenu();
    HMENU view = CreatePopupMenu();
    HMENU history = CreatePopupMenu();
    HMENU bookmarks = CreatePopupMenu();
    HMENU tools = CreatePopupMenu();
    HMENU help = CreatePopupMenu();

    if (!file || !edit || !view || !history || !bookmarks || !tools || !help) {
        if (file) DestroyMenu(file);
        if (edit) DestroyMenu(edit);
        if (view) DestroyMenu(view);
        if (history) DestroyMenu(history);
        if (bookmarks) DestroyMenu(bookmarks);
        if (tools) DestroyMenu(tools);
        if (help) DestroyMenu(help);
        DestroyMenu(built.bar);
        built.bar = nullptr;
        return built;
    }

    built.allMenus = {built.bar, file, edit, view, history, bookmarks, tools, help};

    AddOwnerDrawItem(built, file, CMD_FILE_NEW_TAB, L"&New Tab\tCtrl+T");
    AddOwnerDrawItem(built, file, CMD_FILE_NEW_WINDOW, L"New &Window\tCtrl+N");
    AddOwnerDrawItem(built, file, CMD_FILE_NEW_INCOGNITO, L"New &Incognito Window\tCtrl+Shift+N");
    AddOwnerDrawSeparator(built, file);
    AddOwnerDrawItem(built, file, CMD_FILE_OPEN, L"&Open File...\tCtrl+O");
    AddOwnerDrawItem(built, file, CMD_FILE_SAVE, L"&Save Page As...\tCtrl+S");
    AddOwnerDrawItem(built, file, CMD_FILE_PRINT, L"&Print...\tCtrl+P");
    AddOwnerDrawSeparator(built, file);
    AddOwnerDrawItem(built, file, CMD_FILE_CLOSE_TAB, L"&Close Tab\tCtrl+W");
    AddOwnerDrawItem(built, file, CMD_FILE_EXIT, L"E&xit");

    AddOwnerDrawItem(built, edit, CMD_EDIT_UNDO, L"&Undo\tCtrl+Z");
    AddOwnerDrawItem(built, edit, CMD_EDIT_REDO, L"&Redo\tCtrl+Y");
    AddOwnerDrawSeparator(built, edit);
    AddOwnerDrawItem(built, edit, CMD_EDIT_CUT, L"Cu&t\tCtrl+X");
    AddOwnerDrawItem(built, edit, CMD_EDIT_COPY, L"&Copy\tCtrl+C");
    AddOwnerDrawItem(built, edit, CMD_EDIT_PASTE, L"&Paste\tCtrl+V");
    AddOwnerDrawSeparator(built, edit);
    AddOwnerDrawItem(built, edit, CMD_EDIT_SELECT_ALL, L"Select &All\tCtrl+A");
    AddOwnerDrawItem(built, edit, CMD_EDIT_FIND, L"&Find...\tCtrl+F");

    AddOwnerDrawItem(built, view, CMD_VIEW_RELOAD, L"&Reload\tCtrl+R");
    AddOwnerDrawItem(built, view, CMD_VIEW_HARD_RELOAD, L"Reload Without Cache\tCtrl+Shift+R");
    AddOwnerDrawSeparator(built, view);
    AddOwnerDrawItem(built, view, CMD_VIEW_ZOOM_IN, L"Zoom &In\tCtrl++");
    AddOwnerDrawItem(built, view, CMD_VIEW_ZOOM_OUT, L"Zoom &Out\tCtrl+-");
    AddOwnerDrawItem(built, view, CMD_VIEW_ZOOM_RESET, L"&Actual Size\tCtrl+0");
    AddOwnerDrawSeparator(built, view);
    AddOwnerDrawItem(built, view, CMD_VIEW_FULLSCREEN, L"&Full Screen\tF11");
    AddOwnerDrawItem(built, view, CMD_VIEW_DEVTOOLS, L"&Developer Tools\tCtrl+Shift+I");
    AddOwnerDrawItem(built, view, CMD_VIEW_SOURCE, L"Page &Source\tCtrl+U");
    AddOwnerDrawSeparator(built, view);
    AddOwnerDrawItem(built, view, CMD_VIEW_DARK_MENU, L"&Dark Menu Mode");

    AddOwnerDrawItem(built, history, CMD_HISTORY_BACK, L"&Back\tAlt+Left");
    AddOwnerDrawItem(built, history, CMD_HISTORY_FORWARD, L"&Forward\tAlt+Right");
    AddOwnerDrawItem(built, history, CMD_HISTORY_HOME, L"&Home\tAlt+Home");
    AddOwnerDrawSeparator(built, history);
    AddOwnerDrawItem(built, history, CMD_HISTORY_PAGE, L"Show Full &History\tCtrl+H");

    AddOwnerDrawItem(built, bookmarks, CMD_BOOKMARK_THIS_TAB, L"&Bookmark This Tab...\tCtrl+D");
    AddOwnerDrawItem(built, bookmarks, CMD_BOOKMARK_ALL_TABS, L"Bookmark &All Tabs...\tCtrl+Shift+D");
    AddOwnerDrawSeparator(built, bookmarks);
    AddOwnerDrawItem(built, bookmarks, CMD_BOOKMARK_MANAGER, L"Bookmark &Manager");
    AddOwnerDrawItem(built, bookmarks, CMD_BOOKMARK_TOGGLE_BAR, L"Show Bookmarks &Bar\tCtrl+Shift+B");

    AddOwnerDrawItem(built, tools, CMD_TOOLS_DOWNLOADS, L"&Downloads\tCtrl+J");
    AddOwnerDrawItem(built, tools, CMD_TOOLS_EXTENSIONS, L"&Extensions\tCtrl+Shift+E");
    AddOwnerDrawItem(built, tools, CMD_TOOLS_TASK_MANAGER, L"&Task Manager\tShift+Esc");
    AddOwnerDrawItem(built, tools, CMD_TOOLS_CLEAR_DATA, L"&Clear Browsing Data...\tCtrl+Shift+Delete");
    AddOwnerDrawSeparator(built, tools);
    AddOwnerDrawItem(built, tools, CMD_TOOLS_SETTINGS, L"&Settings\tCtrl+Alt+S");

    AddOwnerDrawItem(built, help, CMD_HELP_HELP, L"Google Chrome &Help\tF1");
    AddOwnerDrawItem(built, help, CMD_HELP_ABOUT, L"&About Google Chrome\tCtrl+Alt+A");

    AddOwnerDrawPopup(built, built.bar, file, L"&File");
    AddOwnerDrawPopup(built, built.bar, edit, L"&Edit");
    AddOwnerDrawPopup(built, built.bar, view, L"&View");
    AddOwnerDrawPopup(built, built.bar, history, L"&History");
    AddOwnerDrawPopup(built, built.bar, bookmarks, L"&Bookmarks");
    AddOwnerDrawPopup(built, built.bar, tools, L"&Tools");
    AddOwnerDrawPopup(built, built.bar, help, L"&Help");

    ApplyThemeToBuiltMenu(built);
    return built;
}

void SendKey(WORD vk, bool ctrl = false, bool shift = false, bool alt = false) {
    std::vector<INPUT> input;
    input.reserve(8);

    auto push = [&input](WORD key, DWORD flags) {
        INPUT i{};
        i.type = INPUT_KEYBOARD;
        i.ki.wVk = key;
        i.ki.dwFlags = flags;
        input.push_back(i);
    };

    if (ctrl) push(VK_CONTROL, 0);
    if (shift) push(VK_SHIFT, 0);
    if (alt) push(VK_MENU, 0);
    push(vk, 0);
    push(vk, KEYEVENTF_KEYUP);
    if (alt) push(VK_MENU, KEYEVENTF_KEYUP);
    if (shift) push(VK_SHIFT, KEYEVENTF_KEYUP);
    if (ctrl) push(VK_CONTROL, KEYEVENTF_KEYUP);

    SendInput(static_cast<UINT>(input.size()), input.data(), sizeof(INPUT));
}

void OpenChromeUrl(HWND hwnd, const wchar_t* url) {
    if (!IsWindow(hwnd) || !url) {
        return;
    }

    wchar_t exePath[MAX_PATH]{};
    if (!GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath))) {
        Wh_Log(L"OpenChromeUrl: GetModuleFileNameW failed, error=%lu",
               GetLastError());
        return;
    }

    std::wstring commandLine = L"\"";
    commandLine += exePath;
    commandLine += L"\" \"";
    commandLine += url;
    commandLine += L"\"";

    std::vector<wchar_t> writableCommandLine(commandLine.begin(),
                                              commandLine.end());
    writableCommandLine.push_back(L'\0');

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION processInfo{};

    if (!CreateProcessW(nullptr, writableCommandLine.data(), nullptr, nullptr,
                        FALSE, 0, nullptr, nullptr, &startupInfo,
                        &processInfo)) {
        Wh_Log(L"OpenChromeUrl: CreateProcessW failed, error=%lu",
               GetLastError());
        return;
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
}

void PaintMenuBarBottomLine(HWND hwnd) {
    if (!g_darkMode.load() || !GetMenu(hwnd)) {
        return;
    }

    MENUBARINFO mbi{};
    mbi.cbSize = sizeof(mbi);
    if (!GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi)) {
        return;
    }

    RECT rcWindow{};
    if (!GetWindowRect(hwnd, &rcWindow)) {
        return;
    }

    RECT rcLine = mbi.rcBar;
    OffsetRect(&rcLine, -rcWindow.left, -rcWindow.top);

    rcLine.top = rcLine.bottom;
    rcLine.bottom = rcLine.top + 1;

    HDC hdc = GetWindowDC(hwnd);
    if (hdc) {
        FillRect(hdc, &rcLine, g_darkBackgroundBrush);
        ReleaseDC(hwnd, hdc);
    }
}

void RedrawAllMenus() {
    struct RedrawTarget {
        HWND hwnd;
        std::vector<HMENU> menus;
    };

    std::vector<RedrawTarget> targets;
    AcquireSRWLockShared(&g_stateLock);
    targets.reserve(g_windows.size());
    for (const auto& [hwnd, state] : g_windows) {
        if (state.menu) {
            targets.push_back({hwnd, state.allMenus});
        }
    }
    ReleaseSRWLockShared(&g_stateLock);

    HBRUSH background = g_darkMode.load()
        ? g_darkBackgroundBrush
        : GetSysColorBrush(COLOR_MENU);

    for (const auto& target : targets) {
        for (HMENU menu : target.menus) {
            SetMenuBackground(menu, background);
        }

        if (IsWindow(target.hwnd)) {
            DrawMenuBar(target.hwnd);
            RedrawWindow(target.hwnd, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
            PaintMenuBarBottomLine(target.hwnd);
        }
    }
}

void RunCommand(HWND hwnd, UINT id) {
    SetForegroundWindow(hwnd);

    switch (id) {
        case CMD_FILE_NEW_TAB: SendKey(L'T', true); break;
        case CMD_FILE_NEW_WINDOW: SendKey(L'N', true); break;
        case CMD_FILE_NEW_INCOGNITO: SendKey(L'N', true, true); break;
        case CMD_FILE_OPEN: SendKey(L'O', true); break;
        case CMD_FILE_SAVE: SendKey(L'S', true); break;
        case CMD_FILE_PRINT: SendKey(L'P', true); break;
        case CMD_FILE_CLOSE_TAB: SendKey(L'W', true); break;
        case CMD_FILE_EXIT: PostMessageW(hwnd, WM_CLOSE, 0, 0); break;

        case CMD_EDIT_UNDO: SendKey(L'Z', true); break;
        case CMD_EDIT_REDO: SendKey(L'Y', true); break;
        case CMD_EDIT_CUT: SendKey(L'X', true); break;
        case CMD_EDIT_COPY: SendKey(L'C', true); break;
        case CMD_EDIT_PASTE: SendKey(L'V', true); break;
        case CMD_EDIT_SELECT_ALL: SendKey(L'A', true); break;
        case CMD_EDIT_FIND: SendKey(L'F', true); break;

        case CMD_VIEW_RELOAD: SendKey(L'R', true); break;
        case CMD_VIEW_HARD_RELOAD: SendKey(L'R', true, true); break;
        case CMD_VIEW_ZOOM_IN: SendKey(VK_OEM_PLUS, true); break;
        case CMD_VIEW_ZOOM_OUT: SendKey(VK_OEM_MINUS, true); break;
        case CMD_VIEW_ZOOM_RESET: SendKey(L'0', true); break;
        case CMD_VIEW_FULLSCREEN: SendKey(VK_F11); break;
        case CMD_VIEW_DEVTOOLS: SendKey(L'I', true, true); break;
        case CMD_VIEW_SOURCE: SendKey(L'U', true); break;
        case CMD_VIEW_DARK_MENU:
            g_darkMode.store(!g_darkMode.load());
            RedrawAllMenus();
            break;

        case CMD_HISTORY_BACK: SendKey(VK_LEFT, false, false, true); break;
        case CMD_HISTORY_FORWARD: SendKey(VK_RIGHT, false, false, true); break;
        case CMD_HISTORY_HOME: SendKey(VK_HOME, false, false, true); break;
        case CMD_HISTORY_PAGE: SendKey(L'H', true); break;

        case CMD_BOOKMARK_THIS_TAB: SendKey(L'D', true); break;
        case CMD_BOOKMARK_ALL_TABS: SendKey(L'D', true, true); break;
        case CMD_BOOKMARK_MANAGER: OpenChromeUrl(hwnd, L"chrome://bookmarks/"); break;
        case CMD_BOOKMARK_TOGGLE_BAR: SendKey(L'B', true, true); break;

        case CMD_TOOLS_DOWNLOADS: SendKey(L'J', true); break;
        case CMD_TOOLS_EXTENSIONS: OpenChromeUrl(hwnd, L"chrome://extensions/"); break;
        case CMD_TOOLS_TASK_MANAGER: SendKey(VK_ESCAPE, false, true); break;
        case CMD_TOOLS_CLEAR_DATA: SendKey(VK_DELETE, true, true); break;
        case CMD_TOOLS_SETTINGS: OpenChromeUrl(hwnd, L"chrome://settings/"); break;

        case CMD_HELP_HELP: SendKey(VK_F1); break;
        case CMD_HELP_ABOUT: OpenChromeUrl(hwnd, L"chrome://settings/help"); break;
    }
}

void DrawCheckMark(HDC hdc, const RECT& rect, COLORREF color) {
    HPEN pen = CreatePen(PS_SOLID, 2, color);
    HGDIOBJ oldPen = SelectObject(hdc, pen);

    int centerY = (rect.top + rect.bottom) / 2;
    int left = rect.left + 7;
    MoveToEx(hdc, left, centerY, nullptr);
    LineTo(hdc, left + 4, centerY + 4);
    LineTo(hdc, left + 11, centerY - 5);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void DrawOwnerItem(const DRAWITEMSTRUCT* dis) {
    if (!dis || dis->CtlType != ODT_MENU || !dis->itemData) {
        return;
    }

    auto* data = reinterpret_cast<MenuItemData*>(dis->itemData);
    HDC hdc = dis->hDC;
    RECT rect = dis->rcItem;
    bool dark = g_darkMode.load();
    bool selected = (dis->itemState & ODS_SELECTED) != 0;
    bool disabled = (dis->itemState & (ODS_DISABLED | ODS_GRAYED)) != 0;

    HBRUSH background = dark
        ? (selected ? g_darkHighlightBrush : g_darkBackgroundBrush)
        : (selected ? GetSysColorBrush(COLOR_HIGHLIGHT)
                    : GetSysColorBrush(COLOR_MENU));

    FillRect(hdc, &rect, background);

    if (data->separator) {
        int y = (rect.top + rect.bottom) / 2;
        HPEN pen = CreatePen(
            PS_SOLID,
            1,
            dark ? kDarkSeparator : GetSysColor(COLOR_MENUTEXT));
        HGDIOBJ oldPen = SelectObject(hdc, pen);
        MoveToEx(hdc, rect.left + 10, y, nullptr);
        LineTo(hdc, rect.right - 10, y);
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
        return;
    }

    COLORREF textColor;
    if (dark) {
        textColor = disabled ? kDarkDisabled : kDarkText;
    } else {
        textColor = disabled
            ? GetSysColor(COLOR_GRAYTEXT)
            : (selected ? GetSysColor(COLOR_HIGHLIGHTTEXT)
                        : GetSysColor(COLOR_MENUTEXT));
    }

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, textColor);

    HFONT oldFont = nullptr;
    if (g_menuFont) {
        oldFont = static_cast<HFONT>(SelectObject(hdc, g_menuFont));
    }

    UINT dpi = 96;
    int leftPadding = data->topLevel ? 4 : 28;
    int rightPadding = data->topLevel ? 4 : 14;
    leftPadding = ScaleForDpi(leftPadding, dpi);
    rightPadding = ScaleForDpi(rightPadding, dpi);

    RECT textRect = rect;
    textRect.left += leftPadding;
    textRect.right -= rightPadding;

    std::wstring label = data->text;
    std::wstring shortcut;
    size_t tab = label.find(L'\t');
    if (tab != std::wstring::npos) {
        shortcut = label.substr(tab + 1);
        label.resize(tab);
    }

    UINT flags = DT_SINGLELINE | DT_VCENTER | DT_HIDEPREFIX;
    if (data->topLevel) {
        flags |= DT_CENTER;
    } else {
        flags |= DT_LEFT;
    }

    DrawTextW(hdc, label.c_str(), static_cast<int>(label.size()),
              &textRect, flags);

    if (!shortcut.empty()) {
        RECT shortcutRect = textRect;
        DrawTextW(hdc, shortcut.c_str(), static_cast<int>(shortcut.size()),
                  &shortcutRect, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
    }

    if (data->id == CMD_VIEW_DARK_MENU && g_darkMode.load()) {
        DrawCheckMark(hdc, rect, textColor);
    }

    if (dis->itemState & ODS_FOCUS) {
        RECT focusRect = rect;
        InflateRect(&focusRect, -2, -2);
        DrawFocusRect(hdc, &focusRect);
    }

    if (oldFont) {
        SelectObject(hdc, oldFont);
    }
}

void MeasureOwnerItem(HWND hwnd, MEASUREITEMSTRUCT* mis) {
    if (!mis || mis->CtlType != ODT_MENU || !mis->itemData) {
        return;
    }

    auto* data = reinterpret_cast<MenuItemData*>(mis->itemData);
    UINT dpi = GetDpiForWindowSafe(hwnd);

    if (data->separator) {
        mis->itemHeight = ScaleForDpi(9, dpi);
        mis->itemWidth = ScaleForDpi(120, dpi);
        return;
    }

    HDC hdc = GetDC(hwnd);
    if (!hdc) {
        mis->itemHeight = ScaleForDpi(24, dpi);
        mis->itemWidth = ScaleForDpi(100, dpi);
        return;
    }

    EnsureMenuFont(dpi);
    HFONT oldFont = g_menuFont
        ? static_cast<HFONT>(SelectObject(hdc, g_menuFont))
        : nullptr;

    std::wstring label = data->text;
    std::wstring shortcut;
    size_t tab = label.find(L'\t');
    if (tab != std::wstring::npos) {
        shortcut = label.substr(tab + 1);
        label.resize(tab);
    }

    SIZE labelSize{};
    SIZE shortcutSize{};
    GetTextExtentPoint32W(hdc, label.c_str(), static_cast<int>(label.size()),
                          &labelSize);
    if (!shortcut.empty()) {
        GetTextExtentPoint32W(hdc, shortcut.c_str(),
                              static_cast<int>(shortcut.size()),
                              &shortcutSize);
    }

    if (oldFont) {
        SelectObject(hdc, oldFont);
    }
    ReleaseDC(hwnd, hdc);

    if (data->topLevel) {
        mis->itemWidth = labelSize.cx + ScaleForDpi(10, dpi);
        LONG h1 = labelSize.cy + ScaleForDpi(4, dpi);
        LONG h2 = static_cast<LONG>(ScaleForDpi(18, dpi));
        mis->itemHeight = (h1 > h2) ? h1 : h2;
    } else {
        int shortcutGap = shortcut.empty() ? 0 : ScaleForDpi(36, dpi);
        mis->itemWidth = labelSize.cx + shortcutSize.cx + shortcutGap +
                         ScaleForDpi(54, dpi);
        LONG h1 = labelSize.cy + ScaleForDpi(8, dpi);
        LONG h2 = static_cast<LONG>(ScaleForDpi(24, dpi));
        mis->itemHeight = (h1 > h2) ? h1 : h2;
    }
}

LRESULT CALLBACK ChromeSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam, DWORD_PTR) {
    switch (msg) {
        case WM_COMMAND:
            if (HIWORD(wParam) == 0) {
                UINT id = LOWORD(wParam);
                if (id >= CMD_FILE_NEW_TAB && id <= CMD_HELP_ABOUT) {
                    RunCommand(hwnd, id);
                    return 0;
                }
            }
            break;

        case WM_MEASUREITEM:
            MeasureOwnerItem(hwnd,
                reinterpret_cast<MEASUREITEMSTRUCT*>(lParam));
            return TRUE;

        case WM_DRAWITEM:
            DrawOwnerItem(reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
            return TRUE;

        case WM_NCCALCSIZE:
        case WM_NCHITTEST:
        case WM_NCLBUTTONDOWN:
            // Chromium normally handles these itself for its custom frame.
            // Use native Win32 handling so the restored caption/menu frame works.
            return DefWindowProcW(hwnd, msg, wParam, lParam);

        case WM_NCPAINT:
        case WM_NCACTIVATE: {
            LRESULT result = DefWindowProcW(hwnd, msg, wParam, lParam);
            PaintMenuBarBottomLine(hwnd);
            return result;
        }

        case WM_NCDESTROY: {
            WindowState state;
            bool found = false;

            AcquireSRWLockExclusive(&g_stateLock);
            auto it = g_windows.find(hwnd);
            if (it != g_windows.end()) {
                state = std::move(it->second);
                g_windows.erase(it);
                found = true;
            }
            ReleaseSRWLockExclusive(&g_stateLock);

            if (found) {
                if (state.menu) {
                    DestroyMenu(state.menu);
                }
                for (MenuItemData* data : state.itemData) {
                    delete data;
                }
            }
            break;
        }
    }

    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

void AttachMenu(HWND hwnd) {
    if (!IsMainChromeWindow(hwnd)) {
        return;
    }

    AcquireSRWLockExclusive(&g_stateLock);
    auto [it, inserted] = g_windows.emplace(hwnd, WindowState{});
    if (!inserted) {
        HMENU existingMenu = it->second.menu;
        ReleaseSRWLockExclusive(&g_stateLock);

        if (existingMenu && GetMenu(hwnd) != existingMenu) {
            SetMenu(hwnd, existingMenu);
            DrawMenuBar(hwnd);
            PaintMenuBarBottomLine(hwnd);
        }
        return;
    }
    ReleaseSRWLockExclusive(&g_stateLock);

    EnableNativeChromeFrame(hwnd);

    BuiltMenu built = BuildMenuBar();
    if (!built.bar) {
        Wh_Log(L"BuildMenuBar failed for %p, error=%lu", hwnd, GetLastError());
        AcquireSRWLockExclusive(&g_stateLock);
        g_windows.erase(hwnd);
        ReleaseSRWLockExclusive(&g_stateLock);
        return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwnd, ChromeSubclassProc, 0)) {
        Wh_Log(L"SetWindowSubclass failed for %p, error=%lu",
               hwnd, GetLastError());
        DestroyMenu(built.bar);
        for (MenuItemData* data : built.itemData) {
            delete data;
        }
        AcquireSRWLockExclusive(&g_stateLock);
        g_windows.erase(hwnd);
        ReleaseSRWLockExclusive(&g_stateLock);
        return;
    }

    if (!SetMenu(hwnd, built.bar)) {
        Wh_Log(L"SetMenu failed for %p, error=%lu", hwnd, GetLastError());
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hwnd, ChromeSubclassProc);
        DestroyMenu(built.bar);
        for (MenuItemData* data : built.itemData) {
            delete data;
        }
        AcquireSRWLockExclusive(&g_stateLock);
        g_windows.erase(hwnd);
        ReleaseSRWLockExclusive(&g_stateLock);
        return;
    }

    DrawMenuBar(hwnd);
    PaintMenuBarBottomLine(hwnd);

    bool stored = false;
    AcquireSRWLockExclusive(&g_stateLock);
    auto stateIt = g_windows.find(hwnd);
    if (stateIt != g_windows.end()) {
        stateIt->second = WindowState{built.bar, std::move(built.allMenus),
                                      std::move(built.itemData)};
        stored = true;
    }
    ReleaseSRWLockExclusive(&g_stateLock);

    if (!stored) {
        SetMenu(hwnd, nullptr);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hwnd, ChromeSubclassProc);
        DestroyMenu(built.bar);
        for (MenuItemData* data : built.itemData) {
            delete data;
        }
        return;
    }

    Wh_Log(L"Classic menu attached to %p", hwnd);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM) {
    AttachMenu(hwnd);
    return TRUE;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

HWND WINAPI CreateWindowExW_Hook(
    DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
    DWORD style, int x, int y, int width, int height,
    HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    if (g_manageNativeFrame.load()) {
        // Required for the native non-client/menu surface to remain visible.
        exStyle &= ~WS_EX_NOREDIRECTIONBITMAP;
    }

    bool chromeBrowserFrame =
        className &&
        !IS_INTRESOURCE(className) &&
        wcsncmp(className, L"Chrome_WidgetWin_", 17) == 0 &&
        (style & WS_CAPTION) != 0;

    HWND hwnd = CreateWindowExW_Original(
        exStyle, className, windowName, style, x, y, width,
        height, parent, menu, instance, param);

    if (hwnd && chromeBrowserFrame) {
        // New browser windows are attached as soon as Chrome creates them.
        AttachMenu(hwnd);
    }

    return hwnd;
}

using SetWindowThemeAttribute_t = decltype(&SetWindowThemeAttribute);
SetWindowThemeAttribute_t SetWindowThemeAttribute_Original = nullptr;

HRESULT WINAPI SetWindowThemeAttribute_Hook(
    HWND hwnd,
    enum WINDOWTHEMEATTRIBUTETYPE attribute,
    PVOID value,
    DWORD valueSize) {
    if (g_manageNativeFrame.load() && attribute == WTA_NONCLIENT) {
        // Match Titlebar For Everyone: keep native/DWM non-client controls
        // enabled by ignoring Chromium's attempt to disable them.
        return S_OK;
    }

    return SetWindowThemeAttribute_Original(
        hwnd,
        attribute,
        value,
        valueSize);
}

void DetachAllMenus() {
    std::unordered_map<HWND, WindowState> windows;
    AcquireSRWLockExclusive(&g_stateLock);
    windows.swap(g_windows);
    ReleaseSRWLockExclusive(&g_stateLock);

    for (auto& [hwnd, state] : windows) {
        if (IsWindow(hwnd)) {
            SetMenu(hwnd, nullptr);
            DrawMenuBar(hwnd);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hwnd, ChromeSubclassProc);
        }

        if (state.menu) {
            DestroyMenu(state.menu);
        }
        for (MenuItemData* data : state.itemData) {
            delete data;
        }
    }
}

void LoadSettings() {
    g_darkMode.store(Wh_GetIntSetting(L"darkMenu") != 0);
    g_manageNativeFrame.store(
        Wh_GetIntSetting(L"manageNativeFrame") != 0);
}

}  // namespace

BOOL Wh_ModInit() {
    if (wcsstr(GetCommandLineW(), L"--type=") != nullptr) {
        Wh_Log(L"Auxiliary Chrome process detected, skipping");
        return FALSE;
    }

    Wh_Log(L"Initializing Chrome Classic Menu Bar v1.0.0");
    LoadSettings();

    g_darkBackgroundBrush = CreateSolidBrush(kDarkBackground);
    g_darkHighlightBrush = CreateSolidBrush(kDarkHighlight);
    g_darkSeparatorBrush = CreateSolidBrush(kDarkSeparator);

    if (!g_darkBackgroundBrush || !g_darkHighlightBrush ||
        !g_darkSeparatorBrush) {
        Wh_Log(L"Failed to create dark-mode brushes");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(CreateWindowExW),
            reinterpret_cast<void*>(CreateWindowExW_Hook),
            reinterpret_cast<void**>(&CreateWindowExW_Original))) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(SetWindowThemeAttribute),
            reinterpret_cast<void*>(SetWindowThemeAttribute_Hook),
            reinterpret_cast<void**>(&SetWindowThemeAttribute_Original))) {
        Wh_Log(L"Failed to hook SetWindowThemeAttribute");
        return FALSE;
    }

    EnumWindows(EnumWindowsProc, 0);
    return TRUE;
}

void Wh_ModAfterInit() {
    // Covers Windhawk being enabled while Chrome is already running.
    EnumWindows(EnumWindowsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(EnumWindowsProc, 0);
    RedrawAllMenus();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Chrome Classic Menu Bar v1.0.0");

    /* no worker threads: all callbacks are owned by Chrome window threads */
    DetachAllMenus();

    if (g_menuFont &&
        g_menuFont != static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT))) {
        DeleteObject(g_menuFont);
    }
    g_menuFont = nullptr;

    if (g_darkBackgroundBrush) {
        DeleteObject(g_darkBackgroundBrush);
        g_darkBackgroundBrush = nullptr;
    }
    if (g_darkHighlightBrush) {
        DeleteObject(g_darkHighlightBrush);
        g_darkHighlightBrush = nullptr;
    }
    if (g_darkSeparatorBrush) {
        DeleteObject(g_darkSeparatorBrush);
        g_darkSeparatorBrush = nullptr;
    }
}
