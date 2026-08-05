// ==WindhawkMod==
// @id              chrome-classic-menu-bar
// @name            Chrome Classic Menu Bar
// @description     Adds a native classic menu bar to Google Chrome and Microsoft Edge, with switchable dark mode
// @version         1.1.0
// @author          Gokhan
// @github          https://github.com/GokhanGerkz
// @include         chrome.exe
// @include         msedge.exe
// @compilerOptions -lcomctl32 -luser32 -lgdi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Chrome Classic Menu Bar v1.1.0

Standalone native Win32 menu bar for Google Chrome and Microsoft Edge.

Included:
  - File/Edit/View/History/Bookmarks or Favorites/Tools/Help menus.
  - Dark owner-drawn menu theme.
  - Native Chromium frame integration.
  - Browser-aware new-tab navigation for chrome:// and edge:// pages.
  - Automatic startup attachment through the Chromium window-creation hook.
  - Bounded per-window retries while Chrome or Edge finishes creating a window.
  - Edge-aware menu labels, including InPrivate and Favorites.
*/
// ==/WindhawkModReadme==

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

enum class BrowserType {
    Chrome,
    Edge,
};

enum class BrowserPage {
    Bookmarks,
    Extensions,
    Settings,
    About,
};

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

struct PendingAttachRetry {
    UINT_PTR timerId = 0;
    unsigned int attempts = 0;
};

struct BrowserPageRequest {
    HWND hwnd = nullptr;
    std::wstring url;
};

SRWLOCK g_stateLock = SRWLOCK_INIT;
std::unordered_map<HWND, WindowState> g_windows;
std::unordered_map<HWND, PendingAttachRetry> g_pendingAttachRetries;
BrowserType g_browserType = BrowserType::Chrome;
std::atomic<bool> g_stopWorker{false};
std::atomic<bool> g_darkMode{true};
HANDLE g_workerThread = nullptr;
std::atomic<bool> g_workerStarted{false};
HWND g_hotkeyWindow = nullptr;

constexpr int HOTKEY_EXTENSIONS = 20001;
constexpr int HOTKEY_SETTINGS = 20002;
constexpr int HOTKEY_ABOUT = 20003;
constexpr UINT ATTACH_RETRY_INTERVAL_MS = 200;
constexpr unsigned int ATTACH_RETRY_MAX_ATTEMPTS = 10;
constexpr UINT WM_OPEN_BROWSER_PAGE = WM_APP + 100;
constexpr wchar_t kHotkeyWindowClass[] =
    L"ClassicBrowserMenuBarHotkeyWindow";

HBRUSH g_darkBackgroundBrush = nullptr;
HBRUSH g_darkHighlightBrush = nullptr;
HFONT g_menuFont = nullptr;
bool g_menuFontCreated = false;

constexpr COLORREF kDarkBackground = RGB(32, 33, 36);
constexpr COLORREF kDarkHighlight = RGB(60, 64, 67);
constexpr COLORREF kDarkText = RGB(232, 234, 237);
constexpr COLORREF kDarkDisabled = RGB(154, 160, 166);
constexpr COLORREF kDarkSeparator = RGB(95, 99, 104);

bool IsChromiumBrowserFrame(HWND hwnd);
void AttachMenu(HWND hwnd);
void ScheduleAttachRetry(HWND hwnd);
void CancelAttachRetry(HWND hwnd);
void CALLBACK AttachRetryTimerProc(HWND hwnd, UINT, UINT_PTR timerId, DWORD);

bool InitializeBrowserType() {
    wchar_t executablePath[MAX_PATH]{};
    if (!GetModuleFileNameW(
            nullptr, executablePath, ARRAYSIZE(executablePath))) {
        Wh_Log(L"GetModuleFileNameW failed, error=%lu", GetLastError());
        return false;
    }

    const wchar_t* executableName = wcsrchr(executablePath, L'\\');
    executableName =
        executableName ? executableName + 1 : executablePath;

    if (CompareStringOrdinal(
            executableName, -1, L"chrome.exe", -1, TRUE) == CSTR_EQUAL) {
        g_browserType = BrowserType::Chrome;
        return true;
    }

    if (CompareStringOrdinal(
            executableName, -1, L"msedge.exe", -1, TRUE) == CSTR_EQUAL) {
        g_browserType = BrowserType::Edge;
        return true;
    }

    Wh_Log(L"Unsupported browser process: %s", executableName);
    return false;
}

bool IsEdgeBrowser() {
    return g_browserType == BrowserType::Edge;
}

const wchar_t* GetBrowserDisplayName() {
    return IsEdgeBrowser() ? L"Microsoft Edge" : L"Google Chrome";
}

const wchar_t* GetBrowserPageUrl(BrowserPage page) {
    if (IsEdgeBrowser()) {
        switch (page) {
            case BrowserPage::Bookmarks:
                return L"edge://favorites/";
            case BrowserPage::Extensions:
                return L"edge://extensions/";
            case BrowserPage::Settings:
                return L"edge://settings/";
            case BrowserPage::About:
                return L"edge://settings/help";
        }
    }

    switch (page) {
        case BrowserPage::Bookmarks:
            return L"chrome://bookmarks/";
        case BrowserPage::Extensions:
            return L"chrome://extensions/";
        case BrowserPage::Settings:
            return L"chrome://settings/";
        case BrowserPage::About:
            return L"chrome://settings/help";
    }

    return L"";
}

void EnableNativeBrowserFrame(HWND hwnd) {
    if (!IsChromiumBrowserFrame(hwnd)) {
        return;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    style |= WS_CAPTION | WS_SYSMENU;
    SetWindowLongPtrW(hwnd, GWL_STYLE, style);

    SetWindowPos(
        hwnd,
        nullptr,
        0,
        0,
        0,
        0,
        SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE |
            SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

bool IsChromiumBrowserFrame(HWND hwnd) {
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

bool IsMainBrowserWindow(HWND hwnd) {
    if (!IsChromiumBrowserFrame(hwnd)) {
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
    if (g_menuFont && g_menuFontCreated) {
        DeleteObject(g_menuFont);
    }
    g_menuFont = nullptr;
    g_menuFontCreated = false;

    NONCLIENTMETRICSW ncm{};
    ncm.cbSize = sizeof(ncm);
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0)) {
        ncm.lfMenuFont.lfHeight = MulDiv(ncm.lfMenuFont.lfHeight, static_cast<int>(dpi), 96);
        g_menuFont = CreateFontIndirectW(&ncm.lfMenuFont);
        if (g_menuFont) g_menuFontCreated = true;
    }

    if (!g_menuFont) {
        g_menuFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
        g_menuFontCreated = false;
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

    const wchar_t* privateWindowText = IsEdgeBrowser()
        ? L"New &InPrivate Window\tCtrl+Shift+N"
        : L"New &Incognito Window\tCtrl+Shift+N";
    const wchar_t* bookmarkThisTabText = IsEdgeBrowser()
        ? L"Add This Page to &Favorites...\tCtrl+D"
        : L"&Bookmark This Tab...\tCtrl+D";
    const wchar_t* bookmarkAllTabsText = IsEdgeBrowser()
        ? L"Add All Tabs to &Favorites...\tCtrl+Shift+D"
        : L"Bookmark &All Tabs...\tCtrl+Shift+D";
    const wchar_t* bookmarkManagerText = IsEdgeBrowser()
        ? L"Open &Favorites Page"
        : L"Bookmark &Manager";
    const wchar_t* bookmarkBarText = IsEdgeBrowser()
        ? L"Show Favorites &Bar\tCtrl+Shift+B"
        : L"Show Bookmarks &Bar\tCtrl+Shift+B";
    const wchar_t* helpText = IsEdgeBrowser()
        ? L"Microsoft Edge &Help\tF1"
        : L"Google Chrome &Help\tF1";
    const wchar_t* aboutText = IsEdgeBrowser()
        ? L"&About Microsoft Edge\tCtrl+Alt+A"
        : L"&About Google Chrome\tCtrl+Alt+A";
    const wchar_t* bookmarksMenuText =
        IsEdgeBrowser() ? L"&Favorites" : L"&Bookmarks";

    AddOwnerDrawItem(built, file, CMD_FILE_NEW_TAB, L"&New Tab\tCtrl+T");
    AddOwnerDrawItem(built, file, CMD_FILE_NEW_WINDOW, L"New &Window\tCtrl+N");
    AddOwnerDrawItem(built, file, CMD_FILE_NEW_INCOGNITO, privateWindowText);
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

    AddOwnerDrawItem(built, bookmarks, CMD_BOOKMARK_THIS_TAB, bookmarkThisTabText);
    AddOwnerDrawItem(built, bookmarks, CMD_BOOKMARK_ALL_TABS, bookmarkAllTabsText);
    AddOwnerDrawSeparator(built, bookmarks);
    AddOwnerDrawItem(built, bookmarks, CMD_BOOKMARK_MANAGER, bookmarkManagerText);
    AddOwnerDrawItem(built, bookmarks, CMD_BOOKMARK_TOGGLE_BAR, bookmarkBarText);

    AddOwnerDrawItem(built, tools, CMD_TOOLS_DOWNLOADS, L"&Downloads\tCtrl+J");
    AddOwnerDrawItem(built, tools, CMD_TOOLS_EXTENSIONS, L"&Extensions\tCtrl+Shift+E");
    AddOwnerDrawItem(built, tools, CMD_TOOLS_TASK_MANAGER, L"&Task Manager\tShift+Esc");
    AddOwnerDrawItem(built, tools, CMD_TOOLS_CLEAR_DATA, L"&Clear Browsing Data...\tCtrl+Shift+Delete");
    AddOwnerDrawSeparator(built, tools);
    AddOwnerDrawItem(built, tools, CMD_TOOLS_SETTINGS, L"&Settings\tCtrl+Alt+S");

    AddOwnerDrawItem(built, help, CMD_HELP_HELP, helpText);
    AddOwnerDrawItem(built, help, CMD_HELP_ABOUT, aboutText);

    AddOwnerDrawPopup(built, built.bar, file, L"&File");
    AddOwnerDrawPopup(built, built.bar, edit, L"&Edit");
    AddOwnerDrawPopup(built, built.bar, view, L"&View");
    AddOwnerDrawPopup(built, built.bar, history, L"&History");
    AddOwnerDrawPopup(built, built.bar, bookmarks, bookmarksMenuText);
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

void TypeUnicode(const wchar_t* text) {
    std::vector<INPUT> input;
    for (const wchar_t* p = text; *p; ++p) {
        INPUT down{};
        down.type = INPUT_KEYBOARD;
        down.ki.wScan = *p;
        down.ki.dwFlags = KEYEVENTF_UNICODE;
        input.push_back(down);

        INPUT up = down;
        up.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        input.push_back(up);
    }

    if (!input.empty()) {
        SendInput(static_cast<UINT>(input.size()), input.data(), sizeof(INPUT));
    }
}

DWORD WINAPI OpenBrowserPageWorker(LPVOID parameter) {
    auto* request = static_cast<BrowserPageRequest*>(parameter);
    if (!request) {
        return 0;
    }

    HWND hwnd = request->hwnd;
    std::wstring url = std::move(request->url);
    delete request;

    if (!IsWindow(hwnd)) {
        return 0;
    }

    Sleep(150);

    SetForegroundWindow(hwnd);
    Sleep(100);

    // Open a fresh tab first.
    SendKey(L'T', true);
    Sleep(300);

    SendKey(L'A', true);
    Sleep(100);

    // Type the URL.
    TypeUnicode(url.c_str());
    Sleep(150);

    // Navigate.
    SendKey(VK_RETURN);

    return 0;
}

void OpenBrowserUrl(HWND hwnd, const wchar_t* url) {
    if (!IsWindow(hwnd) || !url) {
        return;
    }

    auto* request = new BrowserPageRequest{hwnd, url};

    if (!PostMessageW(
            hwnd,
            WM_OPEN_BROWSER_PAGE,
            0,
            reinterpret_cast<LPARAM>(request))) {
        Wh_Log(L"OpenBrowserUrl: PostMessage failed, error=%lu",
               GetLastError());
        delete request;
    }
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
    AcquireSRWLockShared(&g_stateLock);
    HBRUSH background = g_darkMode.load()
        ? g_darkBackgroundBrush
        : GetSysColorBrush(COLOR_MENU);

    for (auto& [hwnd, state] : g_windows) {
        for (HMENU menu : state.allMenus) {
            SetMenuBackground(menu, background);
        }

        if (IsWindow(hwnd)) {
            DrawMenuBar(hwnd);
            RedrawWindow(hwnd, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
            PaintMenuBarBottomLine(hwnd);
        }
    }
    ReleaseSRWLockShared(&g_stateLock);
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
        case CMD_BOOKMARK_MANAGER:
            OpenBrowserUrl(
                hwnd, GetBrowserPageUrl(BrowserPage::Bookmarks));
            break;
        case CMD_BOOKMARK_TOGGLE_BAR: SendKey(L'B', true, true); break;

        case CMD_TOOLS_DOWNLOADS: SendKey(L'J', true); break;
        case CMD_TOOLS_EXTENSIONS:
            OpenBrowserUrl(
                hwnd, GetBrowserPageUrl(BrowserPage::Extensions));
            break;
        case CMD_TOOLS_TASK_MANAGER: SendKey(VK_ESCAPE, false, true); break;
        case CMD_TOOLS_CLEAR_DATA: SendKey(VK_DELETE, true, true); break;
        case CMD_TOOLS_SETTINGS:
            OpenBrowserUrl(
                hwnd, GetBrowserPageUrl(BrowserPage::Settings));
            break;

        case CMD_HELP_HELP: SendKey(VK_F1); break;
        case CMD_HELP_ABOUT:
            OpenBrowserUrl(
                hwnd, GetBrowserPageUrl(BrowserPage::About));
            break;
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

LRESULT CALLBACK BrowserSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam, DWORD_PTR) {
    switch (msg) {
        case WM_OPEN_BROWSER_PAGE: {
            auto* request =
                reinterpret_cast<BrowserPageRequest*>(lParam);

            HANDLE thread = CreateThread(
                nullptr,
                0,
                OpenBrowserPageWorker,
                request,
                0,
                nullptr);

            if (thread) {
                CloseHandle(thread);
            } else {
                Wh_Log(L"OpenBrowserPageWorker: CreateThread failed, error=%lu",
                       GetLastError());
                delete request;
            }

            return 0;
        }

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
            CancelAttachRetry(hwnd);

            AcquireSRWLockExclusive(&g_stateLock);
            auto it = g_windows.find(hwnd);
            if (it != g_windows.end()) {
                for (MenuItemData* data : it->second.itemData) {
                    delete data;
                }
                g_windows.erase(it);
            }
            ReleaseSRWLockExclusive(&g_stateLock);
            break;
        }
    }

    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI MenuWorkerThread(LPVOID);

void StartWorkerIfNeeded() {
    bool expected = false;
    if (!g_workerStarted.compare_exchange_strong(expected, true)) {
        return;
    }

    g_stopWorker.store(false);
    g_workerThread =
        CreateThread(nullptr, 0, MenuWorkerThread, nullptr, 0, nullptr);

    if (!g_workerThread) {
        Wh_Log(L"Failed to create menu worker thread, error=%lu",
               GetLastError());
        g_workerStarted.store(false);
    } else {
        Wh_Log(L"Hotkey worker started in browser process %lu",
               GetCurrentProcessId());
    }
}

bool IsMenuAttached(HWND hwnd) {
    AcquireSRWLockShared(&g_stateLock);
    auto it = g_windows.find(hwnd);
    bool attached =
        it != g_windows.end() &&
        it->second.menu != nullptr &&
        GetMenu(hwnd) == it->second.menu;
    ReleaseSRWLockShared(&g_stateLock);
    return attached;
}

void CancelAttachRetry(HWND hwnd) {
    UINT_PTR timerId = 0;

    AcquireSRWLockExclusive(&g_stateLock);
    auto it = g_pendingAttachRetries.find(hwnd);
    if (it != g_pendingAttachRetries.end()) {
        timerId = it->second.timerId;
        g_pendingAttachRetries.erase(it);
    }
    ReleaseSRWLockExclusive(&g_stateLock);

    if (timerId) {
        KillTimer(hwnd, timerId);
    }
}

void CALLBACK AttachRetryTimerProc(HWND hwnd, UINT, UINT_PTR timerId, DWORD) {
    if (!IsWindow(hwnd)) {
        KillTimer(hwnd, timerId);
        AcquireSRWLockExclusive(&g_stateLock);
        g_pendingAttachRetries.erase(hwnd);
        ReleaseSRWLockExclusive(&g_stateLock);
        return;
    }

    AttachMenu(hwnd);

    bool stopRetrying = IsMenuAttached(hwnd);
    unsigned int attempts = 0;

    AcquireSRWLockExclusive(&g_stateLock);
    auto it = g_pendingAttachRetries.find(hwnd);
    if (it != g_pendingAttachRetries.end()) {
        ++it->second.attempts;
        attempts = it->second.attempts;

        if (stopRetrying || attempts >= ATTACH_RETRY_MAX_ATTEMPTS) {
            g_pendingAttachRetries.erase(it);
        }
    } else {
        stopRetrying = true;
    }
    ReleaseSRWLockExclusive(&g_stateLock);

    if (stopRetrying || attempts >= ATTACH_RETRY_MAX_ATTEMPTS) {
        KillTimer(hwnd, timerId);

        if (!stopRetrying) {
            Wh_Log(L"Attach retry limit reached for %p", hwnd);
        }
    }
}

void ScheduleAttachRetry(HWND hwnd) {
    if (!IsWindow(hwnd) || IsMenuAttached(hwnd)) {
        return;
    }

    AcquireSRWLockExclusive(&g_stateLock);
    bool alreadyScheduled =
        g_pendingAttachRetries.find(hwnd) != g_pendingAttachRetries.end();
    ReleaseSRWLockExclusive(&g_stateLock);

    if (alreadyScheduled) {
        return;
    }

    UINT_PTR timerId = SetTimer(
        hwnd,
        0,
        ATTACH_RETRY_INTERVAL_MS,
        AttachRetryTimerProc);

    if (!timerId) {
        Wh_Log(L"SetTimer attach retry failed for %p, error=%lu",
               hwnd, GetLastError());
        return;
    }

    AcquireSRWLockExclusive(&g_stateLock);
    auto [it, inserted] = g_pendingAttachRetries.emplace(
        hwnd,
        PendingAttachRetry{timerId, 0});
    ReleaseSRWLockExclusive(&g_stateLock);

    if (!inserted) {
        KillTimer(hwnd, timerId);
    }
}

void AttachMenu(HWND hwnd) {
    if (!IsMainBrowserWindow(hwnd)) {
        return;
    }

    EnableNativeBrowserFrame(hwnd);

    // Hotkeys start only after a real browser frame is found.
    StartWorkerIfNeeded();

    AcquireSRWLockShared(&g_stateLock);
    auto existingIt = g_windows.find(hwnd);
    bool alreadyAttached = existingIt != g_windows.end();
    HMENU existingMenu =
        alreadyAttached ? existingIt->second.menu : nullptr;
    ReleaseSRWLockShared(&g_stateLock);

    if (alreadyAttached) {
        if (GetMenu(hwnd) != existingMenu) {
            SetMenu(hwnd, existingMenu);
            DrawMenuBar(hwnd);
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOOWNERZORDER | SWP_NOACTIVATE |
                         SWP_FRAMECHANGED);
            PaintMenuBarBottomLine(hwnd);
        }
        return;
    }

    BuiltMenu built = BuildMenuBar();
    if (!built.bar) {
        Wh_Log(L"BuildMenuBar failed for %p, error=%lu",
               hwnd, GetLastError());
        return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hwnd, BrowserSubclassProc, 0)) {
        Wh_Log(L"SetWindowSubclass failed for %p, error=%lu",
               hwnd, GetLastError());
        DestroyMenu(built.bar);
        for (MenuItemData* data : built.itemData) {
            delete data;
        }
        return;
    }

    if (!SetMenu(hwnd, built.bar)) {
        Wh_Log(L"SetMenu failed for %p, error=%lu",
               hwnd, GetLastError());
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hwnd, BrowserSubclassProc);
        DestroyMenu(built.bar);
        for (MenuItemData* data : built.itemData) {
            delete data;
        }
        return;
    }

    DrawMenuBar(hwnd);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                 SWP_NOOWNERZORDER | SWP_NOACTIVATE |
                 SWP_FRAMECHANGED);
    PaintMenuBarBottomLine(hwnd);

    AcquireSRWLockExclusive(&g_stateLock);
    g_windows.emplace(
        hwnd,
        WindowState{built.bar, std::move(built.allMenus),
                    std::move(built.itemData)});
    ReleaseSRWLockExclusive(&g_stateLock);

    CancelAttachRetry(hwnd);
    Wh_Log(L"Dark-capable classic menu attached to %p", hwnd);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM) {
    AttachMenu(hwnd);
    return TRUE;
}

HWND GetForegroundMainBrowserWindow() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return nullptr;
    }

    HWND root = GetAncestor(foreground, GA_ROOT);
    return IsMainBrowserWindow(root) ? root : nullptr;
}

LRESULT CALLBACK HotkeyWindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                                  LPARAM lParam) {
    switch (msg) {
        case WM_HOTKEY: {
            HWND browser = GetForegroundMainBrowserWindow();
            if (!browser) {
                return 0;
            }

            switch (static_cast<int>(wParam)) {
                case HOTKEY_EXTENSIONS:
                    RunCommand(browser, CMD_TOOLS_EXTENSIONS);
                    break;
                case HOTKEY_SETTINGS:
                    RunCommand(browser, CMD_TOOLS_SETTINGS);
                    break;
                case HOTKEY_ABOUT:
                    RunCommand(browser, CMD_HELP_ABOUT);
                    break;
            }
            return 0;
        }


        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

HWND WINAPI CreateWindowExW_Hook(
    DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
    DWORD style, int x, int y, int width, int height,
    HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    // Ensure the native non-client/menu surface remains visible.
    exStyle &= ~WS_EX_NOREDIRECTIONBITMAP;

    HWND hwnd = CreateWindowExW_Original(
        exStyle, className, windowName, style, x, y, width,
        height, parent, menu, instance, param);

    if (hwnd && GetAncestor(hwnd, GA_ROOT) == hwnd) {
        wchar_t actualClassName[128]{};
        bool chromiumWidgetWindow =
            GetClassNameW(hwnd, actualClassName, ARRAYSIZE(actualClassName)) &&
            wcsncmp(actualClassName, L"Chrome_WidgetWin_", 17) == 0;

        if (chromiumWidgetWindow) {
            // Chromium can pass the class as an atom and can finish configuring
            // the browser frame after CreateWindowExW returns. Inspect the
            // actual created HWND, try immediately, then retry only this
            // window for a short bounded period.
            AttachMenu(hwnd);
            if (!IsMenuAttached(hwnd)) {
                ScheduleAttachRetry(hwnd);
            }
        }
    }

    return hwnd;
}

DWORD WINAPI MenuWorkerThread(LPVOID) {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = HotkeyWindowProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kHotkeyWindowClass;

    RegisterClassExW(&wc);

    g_hotkeyWindow = CreateWindowExW(
        0,
        kHotkeyWindowClass,
        L"Classic Browser Menu Bar Hotkeys",
        0,
        0, 0, 0, 0,
        HWND_MESSAGE,
        nullptr,
        wc.hInstance,
        nullptr);

    if (!g_hotkeyWindow) {
        Wh_Log(L"Failed to create hotkey window, error=%lu",
               GetLastError());
        return 0;
    }

    if (!RegisterHotKey(
            g_hotkeyWindow,
            HOTKEY_EXTENSIONS,
            MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT,
            L'E')) {
        Wh_Log(L"Failed to register Ctrl+Shift+E, error=%lu",
               GetLastError());
    }

    if (!RegisterHotKey(
            g_hotkeyWindow,
            HOTKEY_SETTINGS,
            MOD_CONTROL | MOD_ALT | MOD_NOREPEAT,
            L'S')) {
        Wh_Log(L"Failed to register Ctrl+Alt+S, error=%lu",
               GetLastError());
    }

    if (!RegisterHotKey(
            g_hotkeyWindow,
            HOTKEY_ABOUT,
            MOD_CONTROL | MOD_ALT | MOD_NOREPEAT,
            L'A')) {
        Wh_Log(L"Failed to register Ctrl+Alt+A, error=%lu",
               GetLastError());
    }

    MSG msg{};
    while (!g_stopWorker.load() && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterHotKey(g_hotkeyWindow, HOTKEY_EXTENSIONS);
    UnregisterHotKey(g_hotkeyWindow, HOTKEY_SETTINGS);
    UnregisterHotKey(g_hotkeyWindow, HOTKEY_ABOUT);

    DestroyWindow(g_hotkeyWindow);
    g_hotkeyWindow = nullptr;
    UnregisterClassW(kHotkeyWindowClass, wc.hInstance);
    return 0;
}

using SetWindowThemeAttribute_t = decltype(&SetWindowThemeAttribute);
SetWindowThemeAttribute_t SetWindowThemeAttribute_Original = nullptr;

HRESULT WINAPI SetWindowThemeAttribute_Hook(
    HWND hwnd,
    enum WINDOWTHEMEATTRIBUTETYPE attribute,
    PVOID value,
    DWORD valueSize) {
    if (attribute == WTA_NONCLIENT) {
        // Keep native/DWM non-client controls enabled.
        return S_OK;
    }

    return SetWindowThemeAttribute_Original(
        hwnd,
        attribute,
        value,
        valueSize);
}

void DetachAllMenus() {
    AcquireSRWLockExclusive(&g_stateLock);

    for (auto& [hwnd, state] : g_windows) {
        if (IsWindow(hwnd)) {
            SetMenu(hwnd, nullptr);
            DrawMenuBar(hwnd);
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOOWNERZORDER | SWP_NOACTIVATE |
                         SWP_FRAMECHANGED);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hwnd, BrowserSubclassProc);
        }

        if (state.menu) {
            DestroyMenu(state.menu);
        }

        for (MenuItemData* data : state.itemData) {
            delete data;
        }
    }

    g_windows.clear();
    ReleaseSRWLockExclusive(&g_stateLock);
}

}  // namespace

BOOL Wh_ModInit() {
    if (wcsstr(GetCommandLineW(), L"--type=") != nullptr) {
        Wh_Log(L"Auxiliary browser process detected, skipping");
        return FALSE;
    }

    if (!InitializeBrowserType()) {
        return FALSE;
    }

    Wh_Log(L"Initializing Chrome Classic Menu Bar v1.1.0 for %s",
           GetBrowserDisplayName());

    g_darkBackgroundBrush = CreateSolidBrush(kDarkBackground);
    g_darkHighlightBrush = CreateSolidBrush(kDarkHighlight);

    if (!g_darkBackgroundBrush || !g_darkHighlightBrush) {
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
    // Covers Windhawk being enabled while Chrome or Edge is already running.
    EnumWindows(EnumWindowsProc, 0);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Chrome Classic Menu Bar v1.1.0 for %s",
           GetBrowserDisplayName());

    g_stopWorker.store(true);
    if (g_hotkeyWindow) {
        PostMessageW(g_hotkeyWindow, WM_CLOSE, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 3000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    g_workerStarted.store(false);

    std::vector<std::pair<HWND, UINT_PTR>> pendingTimers;
    AcquireSRWLockExclusive(&g_stateLock);
    pendingTimers.reserve(g_pendingAttachRetries.size());
    for (const auto& [hwnd, retry] : g_pendingAttachRetries) {
        pendingTimers.push_back({hwnd, retry.timerId});
    }
    g_pendingAttachRetries.clear();
    ReleaseSRWLockExclusive(&g_stateLock);

    for (const auto& [hwnd, timerId] : pendingTimers) {
        if (IsWindow(hwnd) && timerId) {
            KillTimer(hwnd, timerId);
        }
    }

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
}
