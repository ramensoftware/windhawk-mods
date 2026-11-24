// ==WindhawkMod==
// @id              chromium-alttab-tabs
// @name            AltTabs - Browser Tab Switcher HUD
// @description     Press Alt+Q in Chrome, Edge or Brave to search the open tabs of the window and switch to one with the keyboard
// @version         1.2.0
// @author          Dszae
// @github          https://github.com/Dszae
// @homepage        https://github.com/Dszae/AltTabs
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -lgdi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AltTabs - Browser Tab Switcher HUD

Press **Alt+Q** while a Chrome, Edge or Brave window is focused to open a
searchable list of the tabs in that window. Type to filter, pick a tab with
the arrow keys, and press **Enter** to switch to it.

![AltTabs](https://raw.githubusercontent.com/Dszae/AltTabs/8fdb2c819e758ff60a8b6b35ea9a46135c42e632/assets/hud.png)

## Keys

| Key | Action |
|-----|--------|
| **Alt+Q** or **Alt+\`** | Open the list. While it's open, press again to move down, or with Shift to move up. |
| Typing | Filter the tabs. Several words match tabs that contain all of them. |
| **↑** **↓**, **Tab**, **Shift+Tab** | Move the selection |
| **Page Up**, **Page Down** | Move the selection by a page |
| **Enter** or click | Switch to the selected tab |
| **Backspace**, **Ctrl+Backspace** | Delete a character, clear the filter |
| **Esc** | Close the list |

**Alt+\`** is the key above Tab on US keyboard layouts. The hotkeys are only
taken over while a window of an enabled browser is focused. In other apps
they work as usual.

## Notes

- Only the tabs of the focused browser window are listed. The current tab is
  marked with a blue bar.
- Tab titles are read with Windows UI Automation. Browsers don't expose tab
  addresses this way, so the site name next to a tab is taken from the end of
  its title, for example "YouTube" in "Some video - YouTube". Tabs whose title
  doesn't end with a site name have no label.
- Vivaldi isn't supported, its tab bar is part of a web page.
- The mod runs in its own Windhawk process. It doesn't inject code into the
  browser or into Explorer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- GroupBySite: true
  $name: Group tabs by site
  $description: List the tabs of each site together, in the order the sites first appear in the tab strip
- EnableChrome: true
  $name: Google Chrome
- EnableEdge: true
  $name: Microsoft Edge
- EnableBrave: true
  $name: Brave
- EnableOpera: true
  $name: Opera
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <dwmapi.h>
#include <initguid.h>  // Defines the UI Automation GUIDs.
#include <uiautomation.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <windhawk_api.h>

using Microsoft::WRL::ComPtr;

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE) & __ImageBase)

namespace {

constexpr UINT_PTR kDeactivateTimerId = 1;

// A focus change right after the window is shown, e.g. when Alt is released,
// doesn't close it.
constexpr ULONGLONG kActivationGraceMs = 400;

// UI Automation calls to a hung browser fail after this time.
constexpr DWORD kUiaTimeoutMs = 2000;

// Limits for the search for the tab strip in the browser's UI.
constexpr int kTabStripMaxDepth = 16;
constexpr int kTabStripMaxNodes = 400;

constexpr PCWSTR kWindowClass = L"AltTabs_TabSwitcher";

// DWMWA_USE_IMMERSIVE_DARK_MODE, DWMWA_WINDOW_CORNER_PREFERENCE and
// DWMWCP_ROUND, which older SDKs don't define.
constexpr DWORD kDwmwaUseImmersiveDarkMode = 20;
constexpr DWORD kDwmwaWindowCornerPreference = 33;
constexpr DWORD kDwmwcpRound = 2;

// Layout, in pixels at 96 DPI.
constexpr int kWindowWidth = 640;
constexpr int kWindowHeight = 440;
constexpr int kSearchTop = 14;
constexpr int kSearchBottom = 52;
constexpr int kListTop = 66;
constexpr int kRowHeight = 44;
constexpr int kRowStride = 46;
constexpr int kFooterHeight = 34;
constexpr int kSiteWidth = 140;

constexpr COLORREF kBackgroundColor = RGB(24, 24, 28);
constexpr COLORREF kSearchColor = RGB(36, 36, 42);
constexpr COLORREF kSelectionColor = RGB(48, 68, 120);
constexpr COLORREF kAccentColor = RGB(96, 165, 250);
constexpr COLORREF kTextColor = RGB(230, 230, 235);
constexpr COLORREF kSelectedTextColor = RGB(255, 255, 255);
constexpr COLORREF kSiteColor = RGB(140, 145, 160);
constexpr COLORREF kSelectedSiteColor = RGB(190, 215, 255);
constexpr COLORREF kDimTextColor = RGB(120, 125, 135);
constexpr COLORREF kScrollColor = RGB(70, 72, 82);

struct Settings {
    std::atomic<bool> groupBySite{true};
    std::atomic<bool> chrome{true};
    std::atomic<bool> edge{true};
    std::atomic<bool> brave{true};
    std::atomic<bool> opera{true};
};

Settings g_settings;

// Registered messages, since the switcher is a top-level window and can get
// WM_APP messages that other programs broadcast.
// Posted by the keyboard hook. wParam is TRUE if Shift is down.
UINT g_hotkeyMessage;
// Posted to stop the switcher thread.
UINT g_quitMessage;

HANDLE g_switcherThread;
HANDLE g_hookThread;
DWORD g_hookThreadId;
std::atomic<HWND> g_switcherWindow;
bool g_threadStartOk;

void LoadSettings() {
    g_settings.groupBySite = Wh_GetIntSetting(L"GroupBySite") != 0;
    g_settings.chrome = Wh_GetIntSetting(L"EnableChrome") != 0;
    g_settings.edge = Wh_GetIntSetting(L"EnableEdge") != 0;
    g_settings.brave = Wh_GetIntSetting(L"EnableBrave") != 0;
    g_settings.opera = Wh_GetIntSetting(L"EnableOpera") != 0;
}

// Returns whether the window is a main window of an enabled browser.
bool IsEnabledBrowserWindow(HWND hwnd) {
    if (!hwnd || !IsWindowVisible(hwnd)) {
        return false;
    }

    WCHAR className[32];
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"Chrome_WidgetWin_1") != 0) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return false;
    }

    WCHAR path[MAX_PATH];
    DWORD pathSize = ARRAYSIZE(path);
    BOOL gotPath = QueryFullProcessImageNameW(process, 0, path, &pathSize);
    CloseHandle(process);
    if (!gotPath) {
        return false;
    }

    PCWSTR fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;

    const struct {
        PCWSTR fileName;
        const std::atomic<bool>& enabled;
    } browsers[] = {
        {L"chrome.exe", g_settings.chrome},
        {L"msedge.exe", g_settings.edge},
        {L"brave.exe", g_settings.brave},
        {L"opera.exe", g_settings.opera},
    };

    for (const auto& browser : browsers) {
        if (_wcsicmp(fileName, browser.fileName) == 0) {
            return browser.enabled;
        }
    }

    return false;
}

std::wstring ToLower(std::wstring text) {
    if (!text.empty()) {
        CharLowerBuffW(text.data(), static_cast<DWORD>(text.size()));
    }
    return text;
}

std::vector<std::wstring> SplitWords(const std::wstring& text) {
    std::vector<std::wstring> words;
    size_t pos = 0;
    while (true) {
        size_t start = text.find_first_not_of(L' ', pos);
        if (start == std::wstring::npos) {
            break;
        }
        size_t end = text.find(L' ', start);
        if (end == std::wstring::npos) {
            end = text.size();
        }
        words.push_back(text.substr(start, end - start));
        pos = end;
    }
    return words;
}

// Returns the site name at the end of a page title, e.g. "YouTube" for
// "Some video - YouTube", or an empty string if the title doesn't seem to end
// with one.
std::wstring GetSiteName(const std::wstring& title) {
    static constexpr PCWSTR kSeparators[] = {
        L" - ", L" | ", L" – ", L" — ", L" · ",
    };

    size_t siteStart = std::wstring::npos;
    for (PCWSTR separator : kSeparators) {
        size_t pos = title.rfind(separator);
        if (pos == std::wstring::npos || pos == 0) {
            continue;
        }
        size_t end = pos + wcslen(separator);
        if (siteStart == std::wstring::npos || end > siteStart) {
            siteStart = end;
        }
    }

    if (siteStart == std::wstring::npos) {
        return {};
    }

    std::wstring site = title.substr(siteStart);
    size_t first = site.find_first_not_of(L' ');
    size_t last = site.find_last_not_of(L' ');
    if (first == std::wstring::npos) {
        return {};
    }
    site = site.substr(first, last - first + 1);

    // Site names are short, and don't look like paths or contain many words.
    constexpr size_t kMaxSiteLength = 30;
    constexpr std::ptrdiff_t kMaxSpaces = 2;
    if (site.size() > kMaxSiteLength ||
        site.find_first_of(L"/\\:") != std::wstring::npos ||
        std::count(site.begin(), site.end(), L' ') > kMaxSpaces) {
        return {};
    }

    return site;
}

bool IsLeafControlType(CONTROLTYPEID type) {
    switch (type) {
        case UIA_ButtonControlTypeId:
        case UIA_CheckBoxControlTypeId:
        case UIA_EditControlTypeId:
        case UIA_HyperlinkControlTypeId:
        case UIA_ImageControlTypeId:
        case UIA_MenuItemControlTypeId:
        case UIA_ProgressBarControlTypeId:
        case UIA_RadioButtonControlTypeId:
        case UIA_SeparatorControlTypeId:
        case UIA_SliderControlTypeId:
        case UIA_SplitButtonControlTypeId:
        case UIA_TabItemControlTypeId:
        case UIA_TextControlTypeId:
            return true;
    }
    return false;
}

void FillSolidRect(HDC dc, const RECT& rect, COLORREF color) {
    SetDCBrushColor(dc, color);
    FillRect(dc, &rect, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
}

struct TabEntry {
    std::wstring title;
    std::wstring site;
    std::wstring searchText;
    size_t groupOrder = 0;
    bool active = false;
    ComPtr<IUIAutomationElement> element;
};

class TabSwitcher {
   public:
    TabSwitcher() = default;
    TabSwitcher(const TabSwitcher&) = delete;
    TabSwitcher& operator=(const TabSwitcher&) = delete;

    // Both run on the switcher thread, which must have initialized COM.
    bool Init();
    void Uninit();

    HWND Window() const { return m_window; }

   private:
    static LRESULT CALLBACK WndProc(HWND hwnd,
                                    UINT message,
                                    WPARAM wParam,
                                    LPARAM lParam);
    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
    bool HandleKey(WPARAM key);

    void OnHotkey(bool backwards);
    void Show(HWND browser);
    void Hide(bool focusBrowser);
    void ActivateSelectedTab();

    void LoadTabs(HWND browser);
    std::vector<ComPtr<IUIAutomationElement>> FindTabStrips(
        IUIAutomationElement* window);
    void ApplyFilter();

    void MoveSelection(int delta, bool wrap);
    void ScrollBy(int rows);
    void EnsureSelectionVisible();
    int GetVisibleRowCount() const;
    int GetRowFromPoint(int y) const;

    int Scale(int value) const { return MulDiv(value, m_dpi, 96); }
    void UpdateDpi(UINT dpi);
    void DeleteFonts();
    void Paint(HDC hdc);

    HWND m_window = nullptr;
    HWND m_browser = nullptr;
    ULONGLONG m_shownTime = 0;
    UINT m_dpi = 96;
    HFONT m_titleFont = nullptr;
    HFONT m_smallFont = nullptr;

    ComPtr<IUIAutomation> m_uia;
    ComPtr<IUIAutomationCondition> m_trueCondition;
    ComPtr<IUIAutomationCondition> m_tabItemCondition;
    ComPtr<IUIAutomationCacheRequest> m_controlTypeCache;
    ComPtr<IUIAutomationCacheRequest> m_tabCache;

    std::vector<TabEntry> m_tabs;
    // Indices into m_tabs of the tabs that match the filter, in display order.
    std::vector<size_t> m_visible;
    std::wstring m_filter;
    size_t m_selected = 0;
    size_t m_scroll = 0;
};

bool TabSwitcher::Init() {
    HRESULT hr = CoCreateInstance(
        CLSID_CUIAutomation8, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(m_uia.ReleaseAndGetAddressOf()));
    if (FAILED(hr)) {
        hr = CoCreateInstance(CLSID_CUIAutomation, nullptr,
                              CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(m_uia.ReleaseAndGetAddressOf()));
    }
    if (FAILED(hr)) {
        Wh_Log(L"Creating CUIAutomation failed: 0x%08X", hr);
        return false;
    }

    ComPtr<IUIAutomation2> uia2;
    if (SUCCEEDED(m_uia.As(&uia2))) {
        uia2->put_ConnectionTimeout(kUiaTimeoutMs);
        uia2->put_TransactionTimeout(kUiaTimeoutMs);
    }

    VARIANT tabItemType;
    tabItemType.vt = VT_I4;
    tabItemType.lVal = UIA_TabItemControlTypeId;

    if (FAILED(m_uia->CreateTrueCondition(&m_trueCondition)) ||
        FAILED(m_uia->CreatePropertyCondition(
            UIA_ControlTypePropertyId, tabItemType, &m_tabItemCondition)) ||
        FAILED(m_uia->CreateCacheRequest(&m_controlTypeCache)) ||
        FAILED(m_controlTypeCache->AddProperty(UIA_ControlTypePropertyId)) ||
        FAILED(m_uia->CreateCacheRequest(&m_tabCache)) ||
        FAILED(m_tabCache->AddProperty(UIA_NamePropertyId)) ||
        FAILED(m_tabCache->AddProperty(
            UIA_SelectionItemIsSelectedPropertyId))) {
        Wh_Log(L"Creating UI Automation conditions failed");
        return false;
    }

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = WndProc;
    windowClass.hInstance = HINST_THISCOMPONENT;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = kWindowClass;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
        return false;
    }

    CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, kWindowClass, L"AltTabs",
                    WS_POPUP | WS_BORDER, 0, 0, 0, 0, nullptr, nullptr,
                    HINST_THISCOMPONENT, this);
    if (!m_window) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        UnregisterClassW(kWindowClass, HINST_THISCOMPONENT);
        return false;
    }

    // Dark mode and rounded corners. Older Windows versions ignore these.
    BOOL darkMode = TRUE;
    DwmSetWindowAttribute(m_window, kDwmwaUseImmersiveDarkMode, &darkMode,
                          sizeof(darkMode));
    DWORD corners = kDwmwcpRound;
    DwmSetWindowAttribute(m_window, kDwmwaWindowCornerPreference, &corners,
                          sizeof(corners));

    UpdateDpi(GetDpiForWindow(m_window));
    return true;
}

void TabSwitcher::Uninit() {
    if (m_window) {
        DestroyWindow(m_window);
        UnregisterClassW(kWindowClass, HINST_THISCOMPONENT);
    }

    DeleteFonts();

    // Release the UI Automation objects before COM is uninitialized.
    m_tabs.clear();
    m_visible.clear();
    m_tabCache.Reset();
    m_controlTypeCache.Reset();
    m_tabItemCondition.Reset();
    m_trueCondition.Reset();
    m_uia.Reset();
}

LRESULT CALLBACK TabSwitcher::WndProc(HWND hwnd,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    TabSwitcher* self;
    if (message == WM_NCCREATE) {
        auto* createStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = static_cast<TabSwitcher*>(createStruct->lpCreateParams);
        self->m_window = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<TabSwitcher*>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (!self) {
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    LRESULT result = self->HandleMessage(message, wParam, lParam);

    if (message == WM_NCDESTROY) {
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        self->m_window = nullptr;
    }

    return result;
}

LRESULT TabSwitcher::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == g_hotkeyMessage) {
        OnHotkey(wParam != 0);
        return 0;
    }

    if (message == g_quitMessage) {
        PostQuitMessage(0);
        return 0;
    }

    switch (message) {
        case WM_CLOSE:
            // Alt+F4 hides the window, it's reused next time.
            Hide(true);
            return 0;

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && m_shownTime) {
                ULONGLONG elapsed = GetTickCount64() - m_shownTime;
                if (elapsed >= kActivationGraceMs) {
                    Hide(false);
                } else {
                    SetTimer(m_window, kDeactivateTimerId,
                             static_cast<UINT>(kActivationGraceMs - elapsed),
                             nullptr);
                }
            }
            return 0;

        case WM_TIMER:
            if (wParam == kDeactivateTimerId) {
                KillTimer(m_window, kDeactivateTimerId);
                if (m_shownTime && GetForegroundWindow() != m_window) {
                    Hide(false);
                }
                return 0;
            }
            break;

        case WM_KEYDOWN:
            if (HandleKey(wParam)) {
                return 0;
            }
            break;

        case WM_SYSKEYDOWN:
            // Arrow keys and Enter while Alt is still held after Alt+Q.
            if (wParam != VK_F4 && HandleKey(wParam)) {
                return 0;
            }
            if (wParam == VK_MENU) {
                return 0;
            }
            break;

        case WM_SYSKEYUP:
            // Releasing Alt mustn't enter menu mode.
            if (wParam == VK_MENU || wParam == VK_F10) {
                return 0;
            }
            break;

        case WM_SYSCHAR:
            // Alt+letter has no meaning here, don't beep.
            return 0;

        case WM_CHAR:
            if (wParam >= L' ' && wParam != 0x7F) {
                m_filter.push_back(static_cast<WCHAR>(wParam));
                ApplyFilter();
                InvalidateRect(m_window, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONDOWN: {
            int row = GetRowFromPoint(static_cast<short>(HIWORD(lParam)));
            if (row >= 0) {
                m_selected = static_cast<size_t>(row);
                ActivateSelectedTab();
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            int notches = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            ScrollBy(-notches * 3);
            return 0;
        }

        case WM_DPICHANGED:
            // The window is sized for the browser's monitor before it's moved
            // there, so only the fonts need to change.
            UpdateDpi(HIWORD(wParam));
            InvalidateRect(m_window, nullptr, FALSE);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_window, &ps);
            Paint(hdc);
            EndPaint(m_window, &ps);
            return 0;
        }
    }

    return DefWindowProcW(m_window, message, wParam, lParam);
}

bool TabSwitcher::HandleKey(WPARAM key) {
    switch (key) {
        case VK_ESCAPE:
            Hide(true);
            return true;

        case VK_RETURN:
            ActivateSelectedTab();
            return true;

        case VK_TAB:
            MoveSelection(GetKeyState(VK_SHIFT) < 0 ? -1 : 1, true);
            return true;

        case VK_DOWN:
            MoveSelection(1, true);
            return true;

        case VK_UP:
            MoveSelection(-1, true);
            return true;

        case VK_NEXT:
            MoveSelection(GetVisibleRowCount(), false);
            return true;

        case VK_PRIOR:
            MoveSelection(-GetVisibleRowCount(), false);
            return true;

        case VK_BACK:
            if (!m_filter.empty()) {
                if (GetKeyState(VK_CONTROL) < 0) {
                    m_filter.clear();
                } else {
                    m_filter.pop_back();
                }
                ApplyFilter();
                InvalidateRect(m_window, nullptr, FALSE);
            }
            return true;
    }

    return false;
}

void TabSwitcher::OnHotkey(bool backwards) {
    if (m_shownTime) {
        if (GetForegroundWindow() != m_window) {
            // Focus was lost during the grace period, take it back.
            SetForegroundWindow(m_window);
        } else {
            MoveSelection(backwards ? -1 : 1, true);
        }
        return;
    }

    HWND foreground = GetForegroundWindow();
    HWND browser = foreground ? GetAncestor(foreground, GA_ROOTOWNER) : nullptr;
    if (!IsEnabledBrowserWindow(browser)) {
        return;
    }

    Show(browser);
}

void TabSwitcher::Show(HWND browser) {
    m_browser = browser;
    LoadTabs(browser);
    m_filter.clear();
    ApplyFilter();

    // Center on the browser window, inside the work area of its monitor.
    UINT dpi = GetDpiForWindow(browser);
    if (!dpi) {
        dpi = 96;
    }
    int width = MulDiv(kWindowWidth, dpi, 96);
    int height = MulDiv(kWindowHeight, dpi, 96);

    RECT browserRect{};
    GetWindowRect(browser, &browserRect);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(MonitorFromWindow(browser, MONITOR_DEFAULTTONEAREST),
                    &monitorInfo);
    const RECT& workArea = monitorInfo.rcWork;

    LONG x = (browserRect.left + browserRect.right - width) / 2;
    LONG y = (browserRect.top + browserRect.bottom - height) / 2;
    x = std::clamp(x, workArea.left,
                   (std::max)(workArea.left, workArea.right - width));
    y = std::clamp(y, workArea.top,
                   (std::max)(workArea.top, workArea.bottom - height));

    m_shownTime = GetTickCount64();
    SetWindowPos(m_window, HWND_TOPMOST, x, y, width, height,
                 SWP_SHOWWINDOW);
    UpdateDpi(GetDpiForWindow(m_window));
    SetForegroundWindow(m_window);
    InvalidateRect(m_window, nullptr, FALSE);
}

void TabSwitcher::Hide(bool focusBrowser) {
    if (!m_shownTime) {
        return;
    }

    // Cleared first, since hiding the window deactivates it.
    m_shownTime = 0;
    KillTimer(m_window, kDeactivateTimerId);
    ShowWindow(m_window, SW_HIDE);

    if (focusBrowser && IsWindow(m_browser)) {
        SetForegroundWindow(m_browser);
    }

    // Don't hold on to the browser's UI Automation elements while hidden.
    m_tabs.clear();
    m_visible.clear();
    m_filter.clear();
}

void TabSwitcher::ActivateSelectedTab() {
    if (m_selected >= m_visible.size()) {
        Hide(true);
        return;
    }

    ComPtr<IUIAutomationElement> element = m_tabs[m_visible[m_selected]].element;
    HWND browser = m_browser;
    Hide(false);

    if (!IsWindow(browser)) {
        return;
    }

    if (IsIconic(browser)) {
        ShowWindow(browser, SW_RESTORE);
    }
    SetForegroundWindow(browser);

    ComPtr<IUIAutomationSelectionItemPattern> selectionItem;
    if (SUCCEEDED(element->GetCurrentPatternAs(
            UIA_SelectionItemPatternId,
            IID_PPV_ARGS(selectionItem.GetAddressOf()))) &&
        selectionItem && SUCCEEDED(selectionItem->Select())) {
        return;
    }

    ComPtr<IUIAutomationLegacyIAccessiblePattern> legacy;
    if (SUCCEEDED(element->GetCurrentPatternAs(
            UIA_LegacyIAccessiblePatternId,
            IID_PPV_ARGS(legacy.GetAddressOf()))) &&
        legacy && SUCCEEDED(legacy->DoDefaultAction())) {
        return;
    }

    Wh_Log(L"Switching to the tab failed");
}

void TabSwitcher::LoadTabs(HWND browser) {
    m_tabs.clear();

    // Chromium creates its accessibility tree on the first request, so right
    // after the browser starts, the tab strip might not be there yet.
    constexpr int kAttempts = 3;
    for (int attempt = 0; attempt < kAttempts && m_tabs.empty(); attempt++) {
        if (attempt > 0) {
            Sleep(50 * attempt);
        }

        ComPtr<IUIAutomationElement> window;
        HRESULT hr = m_uia->ElementFromHandle(browser, &window);
        if (FAILED(hr) || !window) {
            Wh_Log(L"ElementFromHandle failed: 0x%08X", hr);
            continue;
        }

        for (const auto& tabStrip : FindTabStrips(window.Get())) {
            ComPtr<IUIAutomationElementArray> items;
            if (FAILED(tabStrip->FindAllBuildCache(
                    TreeScope_Descendants, m_tabItemCondition.Get(),
                    m_tabCache.Get(), &items)) ||
                !items) {
                continue;
            }

            int count = 0;
            items->get_Length(&count);
            for (int i = 0; i < count; i++) {
                ComPtr<IUIAutomationElement> item;
                if (FAILED(items->GetElement(i, &item)) || !item) {
                    continue;
                }

                TabEntry tab;

                BSTR name = nullptr;
                if (SUCCEEDED(item->get_CachedName(&name)) && name) {
                    tab.title.assign(name, SysStringLen(name));
                    SysFreeString(name);
                }
                if (tab.title.empty()) {
                    tab.title = L"(untitled)";
                }

                VARIANT selected;
                VariantInit(&selected);
                if (SUCCEEDED(item->GetCachedPropertyValue(
                        UIA_SelectionItemIsSelectedPropertyId, &selected)) &&
                    selected.vt == VT_BOOL) {
                    tab.active = selected.boolVal != VARIANT_FALSE;
                }
                VariantClear(&selected);

                tab.site = GetSiteName(tab.title);
                tab.searchText = ToLower(tab.title);
                tab.element = std::move(item);
                m_tabs.push_back(std::move(tab));
            }
        }
    }

    // Tabs of the same site share the position of the first one of them.
    std::unordered_map<std::wstring, size_t> firstTabOfSite;
    for (size_t i = 0; i < m_tabs.size(); i++) {
        TabEntry& tab = m_tabs[i];
        tab.groupOrder = i;
        if (!tab.site.empty()) {
            auto [it, inserted] = firstTabOfSite.try_emplace(ToLower(tab.site), i);
            tab.groupOrder = it->second;
        }
    }

    Wh_Log(L"Found %d tabs", static_cast<int>(m_tabs.size()));
}

// Returns the tab strips of a browser window, found with a breadth-first
// search which skips web page content.
std::vector<ComPtr<IUIAutomationElement>> TabSwitcher::FindTabStrips(
    IUIAutomationElement* window) {
    std::vector<ComPtr<IUIAutomationElement>> tabStrips;

    std::deque<std::pair<ComPtr<IUIAutomationElement>, int>> queue;
    queue.emplace_back(window, 0);
    int visitedNodes = 0;

    while (!queue.empty() && visitedNodes < kTabStripMaxNodes) {
        auto [element, depth] = std::move(queue.front());
        queue.pop_front();
        visitedNodes++;

        ComPtr<IUIAutomationElementArray> children;
        if (FAILED(element->FindAllBuildCache(
                TreeScope_Children, m_trueCondition.Get(),
                m_controlTypeCache.Get(), &children)) ||
            !children) {
            continue;
        }

        int count = 0;
        children->get_Length(&count);
        for (int i = 0; i < count; i++) {
            ComPtr<IUIAutomationElement> child;
            if (FAILED(children->GetElement(i, &child)) || !child) {
                continue;
            }

            CONTROLTYPEID type = 0;
            child->get_CachedControlType(&type);
            if (type == UIA_TabControlTypeId) {
                tabStrips.push_back(std::move(child));
            } else if (type != UIA_DocumentControlTypeId &&
                       !IsLeafControlType(type) &&
                       depth + 1 < kTabStripMaxDepth) {
                queue.emplace_back(std::move(child), depth + 1);
            }
        }
    }

    return tabStrips;
}

void TabSwitcher::ApplyFilter() {
    std::vector<std::wstring> words = SplitWords(ToLower(m_filter));

    m_visible.clear();
    for (size_t i = 0; i < m_tabs.size(); i++) {
        const std::wstring& text = m_tabs[i].searchText;
        bool matches = std::all_of(
            words.begin(), words.end(), [&text](const std::wstring& word) {
                return text.find(word) != std::wstring::npos;
            });
        if (matches) {
            m_visible.push_back(i);
        }
    }

    if (g_settings.groupBySite) {
        std::stable_sort(m_visible.begin(), m_visible.end(),
                         [this](size_t a, size_t b) {
                             return m_tabs[a].groupOrder < m_tabs[b].groupOrder;
                         });
    }

    m_selected = 0;
    m_scroll = 0;
}

void TabSwitcher::MoveSelection(int delta, bool wrap) {
    int count = static_cast<int>(m_visible.size());
    if (count == 0) {
        return;
    }

    int selected = static_cast<int>(m_selected) + delta;
    if (wrap) {
        selected = ((selected % count) + count) % count;
    } else {
        selected = std::clamp(selected, 0, count - 1);
    }

    m_selected = static_cast<size_t>(selected);
    EnsureSelectionVisible();
    InvalidateRect(m_window, nullptr, FALSE);
}

void TabSwitcher::ScrollBy(int rows) {
    int maxScroll = (std::max)(
        0, static_cast<int>(m_visible.size()) - GetVisibleRowCount());
    int scroll =
        std::clamp(static_cast<int>(m_scroll) + rows, 0, maxScroll);
    m_scroll = static_cast<size_t>(scroll);
    InvalidateRect(m_window, nullptr, FALSE);
}

void TabSwitcher::EnsureSelectionVisible() {
    size_t rows = static_cast<size_t>(GetVisibleRowCount());
    if (m_selected < m_scroll) {
        m_scroll = m_selected;
    } else if (m_selected >= m_scroll + rows) {
        m_scroll = m_selected - rows + 1;
    }
}

int TabSwitcher::GetVisibleRowCount() const {
    RECT client{};
    GetClientRect(m_window, &client);
    int listHeight = client.bottom - Scale(kFooterHeight) - Scale(kListTop);
    int rowGap = Scale(kRowStride) - Scale(kRowHeight);
    return (std::max)(1, (listHeight + rowGap) / Scale(kRowStride));
}

// Returns the index into m_visible of the row at the given client coordinate,
// or -1.
int TabSwitcher::GetRowFromPoint(int y) const {
    int offset = y - Scale(kListTop);
    if (offset < 0 || offset % Scale(kRowStride) >= Scale(kRowHeight)) {
        return -1;
    }

    int row = offset / Scale(kRowStride);
    if (row >= GetVisibleRowCount()) {
        return -1;
    }

    size_t index = m_scroll + static_cast<size_t>(row);
    if (index >= m_visible.size()) {
        return -1;
    }

    return static_cast<int>(index);
}

void TabSwitcher::UpdateDpi(UINT dpi) {
    if (!dpi) {
        dpi = 96;
    }
    if (dpi == m_dpi && m_titleFont) {
        return;
    }

    m_dpi = dpi;
    DeleteFonts();
    m_titleFont = CreateFontW(-Scale(15), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE,
                              FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH, L"Segoe UI");
    m_smallFont = CreateFontW(-Scale(12), 0, 0, 0, FW_NORMAL, FALSE, FALSE,
                              FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH, L"Segoe UI");
}

void TabSwitcher::DeleteFonts() {
    if (m_titleFont) {
        DeleteObject(m_titleFont);
        m_titleFont = nullptr;
    }
    if (m_smallFont) {
        DeleteObject(m_smallFont);
        m_smallFont = nullptr;
    }
}

void TabSwitcher::Paint(HDC hdc) {
    RECT client{};
    GetClientRect(m_window, &client);
    int width = client.right;
    int height = client.bottom;

    HDC dc = CreateCompatibleDC(hdc);
    HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
    HGDIOBJ oldBitmap = SelectObject(dc, bitmap);
    HGDIOBJ oldFont = SelectObject(dc, m_titleFont);
    SetBkMode(dc, TRANSPARENT);

    FillSolidRect(dc, client, kBackgroundColor);

    constexpr UINT kTextFlags =
        DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS;

    // Search box, with the number of matching tabs on the right.
    RECT search{Scale(16), Scale(kSearchTop), width - Scale(16),
                Scale(kSearchBottom)};
    FillSolidRect(dc, search, kSearchColor);

    RECT searchText = search;
    InflateRect(&searchText, -Scale(12), 0);

    WCHAR countText[64];
    if (m_filter.empty()) {
        swprintf_s(countText, m_tabs.size() == 1 ? L"%d tab" : L"%d tabs",
                   static_cast<int>(m_tabs.size()));
    } else {
        swprintf_s(countText, L"%d of %d", static_cast<int>(m_visible.size()),
                   static_cast<int>(m_tabs.size()));
    }
    SelectObject(dc, m_smallFont);
    SetTextColor(dc, kDimTextColor);
    DrawTextW(dc, countText, -1, &searchText, kTextFlags | DT_RIGHT);
    searchText.right -= Scale(90);

    SelectObject(dc, m_titleFont);
    if (m_filter.empty()) {
        SetTextColor(dc, kDimTextColor);
        DrawTextW(dc, L"Type to search tabs", -1, &searchText, kTextFlags);
    } else {
        SetTextColor(dc, kTextColor);
        DrawTextW(dc, m_filter.c_str(), static_cast<int>(m_filter.size()),
                  &searchText, kTextFlags);

        SIZE textSize{};
        GetTextExtentPoint32W(dc, m_filter.c_str(),
                              static_cast<int>(m_filter.size()), &textSize);
        LONG caretX =
            (std::min)(searchText.left + textSize.cx + Scale(2), searchText.right);
        RECT caret{caretX, search.top + Scale(10),
                   caretX + (std::max)(1, Scale(1)), search.bottom - Scale(10)};
        FillSolidRect(dc, caret, kAccentColor);
    }

    // Tab list.
    int listBottom = height - Scale(kFooterHeight);
    int rows = GetVisibleRowCount();
    int y = Scale(kListTop);
    for (size_t i = m_scroll;
         i < m_visible.size() && i < m_scroll + static_cast<size_t>(rows);
         i++, y += Scale(kRowStride)) {
        const TabEntry& tab = m_tabs[m_visible[i]];
        bool selected = i == m_selected;

        RECT row{Scale(12), y, width - Scale(12), y + Scale(kRowHeight)};
        if (selected) {
            FillSolidRect(dc, row, kSelectionColor);
        }

        if (tab.active) {
            RECT marker{row.left, row.top + Scale(12), row.left + Scale(3),
                        row.bottom - Scale(12)};
            FillSolidRect(dc, marker, kAccentColor);
        }

        RECT titleRect{row.left + Scale(14), row.top, row.right - Scale(14),
                       row.bottom};
        if (!tab.site.empty()) {
            RECT siteRect{row.right - Scale(kSiteWidth), row.top,
                          row.right - Scale(14), row.bottom};
            SelectObject(dc, m_smallFont);
            SetTextColor(dc, selected ? kSelectedSiteColor : kSiteColor);
            DrawTextW(dc, tab.site.c_str(), static_cast<int>(tab.site.size()),
                      &siteRect, kTextFlags | DT_RIGHT);
            titleRect.right = siteRect.left - Scale(10);
        }

        SelectObject(dc, m_titleFont);
        SetTextColor(dc, selected ? kSelectedTextColor : kTextColor);
        DrawTextW(dc, tab.title.c_str(), static_cast<int>(tab.title.size()),
                  &titleRect, kTextFlags);
    }

    if (m_visible.empty()) {
        RECT message{Scale(16), Scale(kListTop), width - Scale(16), listBottom};
        SelectObject(dc, m_titleFont);
        SetTextColor(dc, kDimTextColor);
        DrawTextW(dc,
                  m_tabs.empty() ? L"No tabs found in this window"
                                 : L"No matching tabs",
                  -1, &message, kTextFlags | DT_CENTER);
    }

    // Scroll bar.
    size_t visibleCount = m_visible.size();
    if (visibleCount > static_cast<size_t>(rows)) {
        int trackTop = Scale(kListTop);
        int trackHeight = rows * Scale(kRowStride) - (Scale(kRowStride) -
                                                       Scale(kRowHeight));
        int thumbHeight = (std::max)(
            Scale(20), MulDiv(trackHeight, rows, static_cast<int>(visibleCount)));
        int maxScroll = static_cast<int>(visibleCount) - rows;
        int thumbTop = trackTop + MulDiv(trackHeight - thumbHeight,
                                         static_cast<int>(m_scroll), maxScroll);
        RECT thumb{width - Scale(8), thumbTop, width - Scale(5),
                   thumbTop + thumbHeight};
        FillSolidRect(dc, thumb, kScrollColor);
    }

    // Footer.
    RECT footer{Scale(16), listBottom, width - Scale(16), height - Scale(6)};
    SelectObject(dc, m_smallFont);
    SetTextColor(dc, kDimTextColor);
    DrawTextW(dc,
              L"↑↓ Navigate        Enter Switch        Esc Close",
              -1, &footer, kTextFlags | DT_CENTER);

    BitBlt(hdc, 0, 0, width, height, dc, 0, 0, SRCCOPY);

    SelectObject(dc, oldFont);
    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
}

bool IsHotkey(const KBDLLHOOKSTRUCT* key) {
    // Q, and the key above Tab on US layouts.
    if (key->vkCode != 'Q' && key->vkCode != VK_OEM_3) {
        return false;
    }

    if (!(key->flags & LLKHF_ALTDOWN)) {
        return false;
    }

    // Ctrl+Alt is also AltGr, which types characters on many layouts, e.g.
    // AltGr+Q is @ on German keyboards.
    if (GetAsyncKeyState(VK_CONTROL) < 0 || GetAsyncKeyState(VK_LWIN) < 0 ||
        GetAsyncKeyState(VK_RWIN) < 0) {
        return false;
    }

    return true;
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        const auto* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        HWND switcherWindow = g_switcherWindow;
        if (switcherWindow && IsHotkey(key)) {
            HWND foreground = GetForegroundWindow();
            HWND root =
                foreground ? GetAncestor(foreground, GA_ROOTOWNER) : nullptr;
            if (foreground == switcherWindow || IsEnabledBrowserWindow(root)) {
                BOOL backwards = GetAsyncKeyState(VK_SHIFT) < 0;
                PostMessageW(switcherWindow, g_hotkeyMessage, backwards, 0);
                return 1;
            }
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Owns the switcher window and does the UI Automation work.
DWORD WINAPI SwitcherThreadProc(LPVOID param) {
    HANDLE readyEvent = param;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", hr);
        SetEvent(readyEvent);
        return 0;
    }

    {
        TabSwitcher switcher;
        // g_threadStartOk is reused for the next thread once the event is set,
        // so the result is also kept in a local variable.
        bool startOk = switcher.Init();
        if (startOk) {
            g_switcherWindow = switcher.Window();
        }
        g_threadStartOk = startOk;
        SetEvent(readyEvent);

        if (startOk) {
            MSG msg;
            while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        g_switcherWindow = nullptr;
        switcher.Uninit();
    }

    CoUninitialize();
    return 0;
}

// Only runs the keyboard hook, so that it's never blocked by the UI
// Automation calls of the switcher thread.
DWORD WINAPI HookThreadProc(LPVOID param) {
    HANDLE readyEvent = param;

    // Create the message queue before WhTool_ModUninit can post WM_QUIT.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                   HINST_THISCOMPONENT, 0);
    if (!hook) {
        Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());
    }

    g_threadStartOk = hook != nullptr;
    SetEvent(readyEvent);
    if (!hook) {
        return 0;
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWindowsHookEx(hook);
    return 0;
}

// Starts a thread and waits until it signals whether it started successfully.
HANDLE StartThread(LPTHREAD_START_ROUTINE threadProc, DWORD* threadId) {
    HANDLE readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        return nullptr;
    }

    g_threadStartOk = false;
    HANDLE thread =
        CreateThread(nullptr, 0, threadProc, readyEvent, 0, threadId);
    if (thread) {
        // The thread sets the event on every path.
        WaitForSingleObject(readyEvent, INFINITE);
        if (!g_threadStartOk) {
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
            thread = nullptr;
        }
    }

    CloseHandle(readyEvent);
    return thread;
}

void StopSwitcherThread() {
    if (!g_switcherThread) {
        return;
    }

    HWND switcherWindow = g_switcherWindow;
    if (switcherWindow) {
        PostMessageW(switcherWindow, g_quitMessage, 0, 0);
    }
    WaitForSingleObject(g_switcherThread, INFINITE);
    CloseHandle(g_switcherThread);
    g_switcherThread = nullptr;
}

}  // namespace

BOOL WhTool_ModInit() {
    LoadSettings();

    g_hotkeyMessage = RegisterWindowMessageW(L"AltTabs_Hotkey_" WH_MOD_ID);
    g_quitMessage = RegisterWindowMessageW(L"AltTabs_Quit_" WH_MOD_ID);
    if (!g_hotkeyMessage || !g_quitMessage) {
        Wh_Log(L"RegisterWindowMessageW failed: %u", GetLastError());
        return FALSE;
    }

    g_switcherThread = StartThread(SwitcherThreadProc, nullptr);
    if (!g_switcherThread) {
        Wh_Log(L"Starting the switcher thread failed");
        return FALSE;
    }

    g_hookThread = StartThread(HookThreadProc, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"Starting the keyboard hook thread failed");
        StopSwitcherThread();
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }

    StopSwitcherThread();
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
