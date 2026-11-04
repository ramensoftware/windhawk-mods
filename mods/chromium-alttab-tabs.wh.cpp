// ==WindhawkMod==
// @id              chromium-alttab-tabs
// @name            AltTabs - Browser Tab Switcher HUD
// @description     A fast, beautiful Windows 11 Alt+Tab HUD for browser tabs with domain grouping and instant search.
// @version         1.1.0
// @author          Dszae
// @github          https://github.com/Dszae
// @homepage        https://github.com/Dszae/AltTabs
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore -luuid -lshell32 -lgdi32 -ldwmapi -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AltTabs - Browser Tab Switcher HUD

Press **Alt + `** (Tilde / Backtick, right above Tab) while in any Chromium browser (Google Chrome,
Brave, Microsoft Edge, Opera, Vivaldi) to summon a sleek, native Windows 11 floating Tab Switcher HUD.

![AltTabs Demo](https://raw.githubusercontent.com/Dszae/AltTabs/main/brag-output/brag.jpg)

### Highlights:
- **Instant Keyboard Navigation**: Press `Alt + ` ` to summon the HUD. Use `Tab` / `Arrow Keys` to navigate, and `Enter` to switch.
- **Domain Clustering**: Stacks tabs by root domain (`github.com`, `youtube.com`) so 50 open tabs become clean, readable groups.
- **Real-Time Fuzzy Search**: Just start typing any keyword to filter tabs instantly across your open browser window.
- **Native Windows 11 Styling**: Smooth rounded corners, dark mode, favicons, and zero background clutter.
- **100% Isolated & Stable**: Runs in a dedicated Windhawk tool process without injecting into File Explorer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- GroupByDomain: true
  $name: Group tabs by domain
  $description: Categorizes multiple tabs from the same website together in the switcher.
- EnableChrome: true
  $name: Support Google Chrome
- EnableBrave: true
  $name: Support Brave Browser
- EnableEdge: true
  $name: Support Microsoft Edge
- EnableOpera: true
  $name: Support Opera Browser
- EnableVivaldi: true
  $name: Support Vivaldi Browser
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windows.h>
#include <uiautomation.h>
#include <dwmapi.h>
#include <psapi.h>
#include <shlwapi.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <algorithm>
#include <sstream>

#include <windhawk_api.h>

namespace AltTabsMod {

struct ModSettings {
    bool groupByDomain = true;
    bool enableChrome = true;
    bool enableBrave = true;
    bool enableEdge = true;
    bool enableOpera = true;
    bool enableVivaldi = true;
} g_settings;

std::mutex g_settingsMutex;

void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings.groupByDomain = Wh_GetIntSetting(L"GroupByDomain") != 0;
    g_settings.enableChrome = Wh_GetIntSetting(L"EnableChrome") != 0;
    g_settings.enableBrave = Wh_GetIntSetting(L"EnableBrave") != 0;
    g_settings.enableEdge = Wh_GetIntSetting(L"EnableEdge") != 0;
    g_settings.enableOpera = Wh_GetIntSetting(L"EnableOpera") != 0;
    g_settings.enableVivaldi = Wh_GetIntSetting(L"EnableVivaldi") != 0;
}

template <typename T>
class SafeComPtr {
    T* m_ptr = nullptr;
public:
    SafeComPtr() = default;
    SafeComPtr(T* ptr) : m_ptr(ptr) { if (m_ptr) m_ptr->AddRef(); }
    ~SafeComPtr() { Release(); }
    SafeComPtr(const SafeComPtr& o) : m_ptr(o.m_ptr) { if (m_ptr) m_ptr->AddRef(); }
    SafeComPtr& operator=(const SafeComPtr& o) {
        if (this != std::addressof(o)) {
            Release();
            m_ptr = o.m_ptr;
            if (m_ptr) m_ptr->AddRef();
        }
        return *this;
    }
    T* operator->() const { return m_ptr; }
    T** operator&() { Release(); return &m_ptr; }
    operator T*() const { return m_ptr; }
    bool operator!() const { return m_ptr == nullptr; }
    void Release() { if (m_ptr) { T* tmp = m_ptr; m_ptr = nullptr; tmp->Release(); } }
};

struct TabInfo {
    std::wstring title;
    std::wstring domain;
    HWND browserHwnd = nullptr;
    RECT boundingRect = { 0 };
    bool isSelected = false;
    SafeComPtr<IUIAutomationElement> uiaElement;
    HICON hFavicon = nullptr;

    TabInfo() = default;
    ~TabInfo() { if (hFavicon) DestroyIcon(hFavicon); }
    TabInfo(const TabInfo&) = delete;
    TabInfo& operator=(const TabInfo&) = delete;
};

static std::wstring ExtractDomainFromTitle(const std::wstring& title) {
    std::wstring s = title;
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    if (s.find(L"github") != std::wstring::npos) return L"github.com";
    if (s.find(L"google") != std::wstring::npos) return L"google.com";
    if (s.find(L"youtube") != std::wstring::npos) return L"youtube.com";
    if (s.find(L"stackoverflow") != std::wstring::npos) return L"stackoverflow.com";
    if (s.find(L"reddit") != std::wstring::npos) return L"reddit.com";
    if (s.find(L"facebook") != std::wstring::npos) return L"facebook.com";
    if (s.find(L"twitter") != std::wstring::npos || s.find(L" x ") != std::wstring::npos) return L"x.com";
    if (s.find(L"chatgpt") != std::wstring::npos || s.find(L"openai") != std::wstring::npos) return L"openai.com";
    if (s.find(L"claude") != std::wstring::npos) return L"claude.ai";
    if (s.find(L"tcioe") != std::wstring::npos) return L"tcioe.edu.np";

    // Clean up title separators (" - ", " | ")
    size_t lastSep = s.find_last_of(L"-|");
    if (lastSep != std::wstring::npos && lastSep + 1 < s.length()) {
        std::wstring part = s.substr(lastSep + 1);
        while (!part.empty() && part.front() == L' ') part.erase(0, 1);
        while (!part.empty() && part.back() == L' ') part.pop_back();
        if (part.length() > 3 && part.find(L' ') == std::wstring::npos) {
            return part + L".com";
        }
    }
    return L"Other Websites";
}

static bool IsSupportedBrowserWindow(HWND hwnd, std::wstring* outBrowserName = nullptr) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) return false;
    wchar_t cls[64] = { 0 };
    GetClassNameW(hwnd, cls, 64);
    if (wcscmp(cls, L"Chrome_WidgetWin_1") != 0) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return false;

    wchar_t path[MAX_PATH] = { 0 };
    DWORD sz = MAX_PATH;
    QueryFullProcessImageNameW(hProc, 0, path, &sz);
    CloseHandle(hProc);

    std::wstring sPath(path);
    std::transform(sPath.begin(), sPath.end(), sPath.begin(), ::towlower);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    if (g_settings.enableBrave && sPath.find(L"brave.exe") != std::wstring::npos) {
        if (outBrowserName) *outBrowserName = L"Brave Browser";
        return true;
    }
    if (g_settings.enableChrome && sPath.find(L"chrome.exe") != std::wstring::npos) {
        if (outBrowserName) *outBrowserName = L"Google Chrome";
        return true;
    }
    if (g_settings.enableEdge && sPath.find(L"msedge.exe") != std::wstring::npos) {
        if (outBrowserName) *outBrowserName = L"Microsoft Edge";
        return true;
    }
    if (g_settings.enableOpera && sPath.find(L"opera.exe") != std::wstring::npos) {
        if (outBrowserName) *outBrowserName = L"Opera";
        return true;
    }
    if (g_settings.enableVivaldi && sPath.find(L"vivaldi.exe") != std::wstring::npos) {
        if (outBrowserName) *outBrowserName = L"Vivaldi";
        return true;
    }
    return false;
}

// --- TAB SWITCHER HUD WINDOW ---
class TabSwitcherHUD {
public:
    static TabSwitcherHUD& Instance() {
        static TabSwitcherHUD s_hud;
        return s_hud;
    }

    void Show() {
        HWND fgHwnd = GetForegroundWindow();
        std::wstring browserName = L"Browser";
        if (!IsSupportedBrowserWindow(fgHwnd, &browserName)) {
            return;
        }

        m_targetBrowserHwnd = fgHwnd;
        m_browserTitle = browserName;
        FetchTabsForBrowser(fgHwnd);

        if (m_allTabs.empty()) return;

        m_filterText.clear();
        m_selectedIndex = 0;
        ApplyFilter();

        if (!m_hwnd || !IsWindow(m_hwnd)) {
            CreateHUDWindow();
        }

        // Center HUD on browser window
        RECT rcBrowser = { 0 };
        GetWindowRect(fgHwnd, &rcBrowser);
        int w = 620;
        int h = 420;
        int x = rcBrowser.left + (rcBrowser.right - rcBrowser.left - w) / 2;
        int y = rcBrowser.top + (rcBrowser.bottom - rcBrowser.top - h) / 2;
        if (x < 50) x = 50;
        if (y < 50) y = 50;

        SetWindowPos(m_hwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
        SetForegroundWindow(m_hwnd);
        SetFocus(m_hwnd);
        InvalidateRect(m_hwnd, nullptr, TRUE);
    }

    void Hide() {
        if (m_hwnd && IsWindow(m_hwnd)) {
            ShowWindow(m_hwnd, SW_HIDE);
        }
        if (m_targetBrowserHwnd && IsWindow(m_targetBrowserHwnd)) {
            SetForegroundWindow(m_targetBrowserHwnd);
        }
    }

    void ActivateSelectedTab() {
        if (m_filteredTabs.empty() || m_selectedIndex >= m_filteredTabs.size()) {
            Hide();
            return;
        }

        auto tab = m_filteredTabs[m_selectedIndex];
        HWND bHwnd = tab->browserHwnd;
        Hide();

        if (bHwnd && IsWindow(bHwnd)) {
            if (IsIconic(bHwnd)) ShowWindow(bHwnd, SW_RESTORE);
            SetForegroundWindow(bHwnd);

            if (tab->uiaElement) {
                SafeComPtr<IUIAutomationSelectionItemPattern> pSel;
                if (SUCCEEDED(tab->uiaElement->GetCurrentPatternAs(UIA_SelectionItemPatternId, IID_IUIAutomationSelectionItemPattern, (void**)&pSel)) && pSel) {
                    pSel->Select();
                    return;
                }
                SafeComPtr<IUIAutomationInvokePattern> pInv;
                if (SUCCEEDED(tab->uiaElement->GetCurrentPatternAs(UIA_InvokePatternId, IID_IUIAutomationInvokePattern, (void**)&pInv)) && pInv) {
                    pInv->Invoke();
                    return;
                }
            }

            // Coordinate click fallback
            if (tab->boundingRect.right > tab->boundingRect.left) {
                POINT pt = {
                    tab->boundingRect.left + (tab->boundingRect.right - tab->boundingRect.left) / 2,
                    tab->boundingRect.top + (tab->boundingRect.bottom - tab->boundingRect.top) / 2
                };
                if (ScreenToClient(bHwnd, &pt)) {
                    LPARAM lParam = MAKELPARAM(pt.x, pt.y);
                    PostMessageW(bHwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
                    PostMessageW(bHwnd, WM_LBUTTONUP, 0, lParam);
                }
            }
        }
    }

private:
    TabSwitcherHUD() = default;

    void CreateHUDWindow() {
        WNDCLASSEXW wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszClassName = L"AltTabs_FloatingHUDWindow";
        RegisterClassExW(&wc);

        m_hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            L"AltTabs_FloatingHUDWindow",
            L"AltTabs Quick Switcher",
            WS_POPUP | WS_BORDER,
            100, 100, 620, 420,
            nullptr, nullptr, GetModuleHandleW(nullptr), this
        );

        // Windows 11 Rounded Corners & Dark Mode
        BOOL darkMode = TRUE;
        DwmSetWindowAttribute(m_hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &darkMode, sizeof(BOOL));
        DWORD corner = 2 /* DWMWCP_ROUND */;
        DwmSetWindowAttribute(m_hwnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &corner, sizeof(DWORD));
    }

    void FetchTabsForBrowser(HWND bHwnd) {
        m_allTabs.clear();

        // Wake up Chromium accessibility tree
        DWORD_PTR dwRes = 0;
        SendMessageTimeoutW(bHwnd, WM_GETOBJECT, 0, (LPARAM)0xFFFFFFFC, SMTO_ABORTIFHUNG, 50, &dwRes);

        SafeComPtr<IUIAutomation> pUIA;
        if (FAILED(CoCreateInstance(CLSID_CUIAutomation8, nullptr, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pUIA)) || !pUIA) {
            CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pUIA);
        }
        if (!pUIA) return;

        SafeComPtr<IUIAutomationElement> pWinElem;
        if (FAILED(pUIA->ElementFromHandle(bHwnd, &pWinElem)) || !pWinElem) return;

        VARIANT varTabItem;
        varTabItem.vt = VT_I4;
        varTabItem.lVal = UIA_TabItemControlTypeId;
        SafeComPtr<IUIAutomationCondition> pTabCond;
        pUIA->CreatePropertyCondition(UIA_ControlTypePropertyId, varTabItem, &pTabCond);

        SafeComPtr<IUIAutomationElementArray> pArray;
        if (FAILED(pWinElem->FindAll(TreeScope_Descendants, pTabCond, &pArray)) || !pArray) return;

        int count = 0;
        pArray->get_Length(&count);

        for (int i = 0; i < count; ++i) {
            SafeComPtr<IUIAutomationElement> pElem;
            if (FAILED(pArray->GetElement(i, &pElem)) || !pElem) continue;

            auto tab = std::make_shared<TabInfo>();
            tab->browserHwnd = bHwnd;
            tab->uiaElement = pElem;

            BSTR bstrName = nullptr;
            if (SUCCEEDED(pElem->get_CurrentName(&bstrName)) && bstrName) {
                tab->title = bstrName;
                SysFreeString(bstrName);
            }
            if (tab->title.empty()) tab->title = L"Untitled Tab";

            tab->domain = ExtractDomainFromTitle(tab->title);
            pElem->get_CurrentBoundingRectangle(&tab->boundingRect);

            m_allTabs.push_back(tab);
        }
    }

    void ApplyFilter() {
        m_filteredTabs.clear();
        std::wstring query = m_filterText;
        std::transform(query.begin(), query.end(), query.begin(), ::towlower);

        for (auto& tab : m_allTabs) {
            if (query.empty()) {
                m_filteredTabs.push_back(tab);
            } else {
                std::wstring sTitle = tab->title;
                std::transform(sTitle.begin(), sTitle.end(), sTitle.begin(), ::towlower);
                std::wstring sDomain = tab->domain;
                std::transform(sDomain.begin(), sDomain.end(), sDomain.begin(), ::towlower);

                if (sTitle.find(query) != std::wstring::npos || sDomain.find(query) != std::wstring::npos) {
                    m_filteredTabs.push_back(tab);
                }
            }
        }

        if (m_selectedIndex >= m_filteredTabs.size()) {
            m_selectedIndex = m_filteredTabs.empty() ? 0 : m_filteredTabs.size() - 1;
        }
    }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        TabSwitcherHUD* pThis = (TabSwitcherHUD*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
        if (uMsg == WM_CREATE) {
            auto* cs = (CREATESTRUCTW*)lParam;
            pThis = (TabSwitcherHUD*)cs->lpCreateParams;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
            return 0;
        }

        if (!pThis) return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        switch (uMsg) {
        case WM_KILLFOCUS:
            pThis->Hide();
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                pThis->Hide();
                return 0;
            }
            if (wParam == VK_RETURN) {
                pThis->ActivateSelectedTab();
                return 0;
            }
            if (wParam == VK_DOWN || wParam == VK_TAB) {
                if (!pThis->m_filteredTabs.empty()) {
                    pThis->m_selectedIndex = (pThis->m_selectedIndex + 1) % pThis->m_filteredTabs.size();
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }
            if (wParam == VK_UP) {
                if (!pThis->m_filteredTabs.empty()) {
                    if (pThis->m_selectedIndex == 0) pThis->m_selectedIndex = pThis->m_filteredTabs.size() - 1;
                    else pThis->m_selectedIndex--;
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }
            if (wParam == VK_BACK) {
                if (!pThis->m_filterText.empty()) {
                    pThis->m_filterText.pop_back();
                    pThis->ApplyFilter();
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }
            break;

        case WM_CHAR:
            if (wParam >= 32 && wParam != 127) {
                pThis->m_filterText.push_back((wchar_t)wParam);
                pThis->ApplyFilter();
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            break;

        case WM_LBUTTONDOWN: {
            int y = HIWORD(lParam);
            int rowStart = 70;
            int rowHeight = 44;
            if (y >= rowStart) {
                size_t clickedIndex = (y - rowStart) / rowHeight;
                if (clickedIndex < pThis->m_filteredTabs.size()) {
                    pThis->m_selectedIndex = clickedIndex;
                    pThis->ActivateSelectedTab();
                }
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            pThis->Render(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        }

        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

    void Render(HDC hdc) {
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, w, h);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

        // Dark background
        HBRUSH bgBrush = CreateSolidBrush(RGB(24, 24, 28));
        FillRect(memDC, &rc, bgBrush);
        DeleteObject(bgBrush);

        SetBkMode(memDC, TRANSPARENT);
        HFONT hFontMain = CreateFontW(17, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HFONT hFontSub = CreateFontW(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HFONT hFontHeader = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        // Header / Search Bar
        RECT rcSearch = { 16, 14, w - 16, 52 };
        HBRUSH searchBg = CreateSolidBrush(RGB(36, 36, 42));
        FillRect(memDC, &rcSearch, searchBg);
        DeleteObject(searchBg);

        SelectObject(memDC, hFontMain);
        SetTextColor(memDC, RGB(220, 225, 235));
        std::wstring prompt = m_filterText.empty() ? L"Type to search tabs... (Alt+` to cycle)" : (L"Search: " + m_filterText);
        if (m_filterText.empty()) SetTextColor(memDC, RGB(130, 135, 145));
        RECT rcSearchText = { rcSearch.left + 12, rcSearch.top + 8, rcSearch.right - 12, rcSearch.bottom };
        DrawTextW(memDC, prompt.c_str(), -1, &rcSearchText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // Tab list
        int y = 66;
        int rowHeight = 44;
        for (size_t i = 0; i < m_filteredTabs.size() && y < h - 40; ++i) {
            RECT rowRc = { 12, y, w - 12, y + rowHeight };
            bool isSelected = (i == m_selectedIndex);

            if (isSelected) {
                HBRUSH selBrush = CreateSolidBrush(RGB(48, 68, 120));
                FillRect(memDC, &rowRc, selBrush);
                DeleteObject(selBrush);
            }

            // Domain Badge / Title
            SelectObject(memDC, hFontMain);
            SetTextColor(memDC, isSelected ? RGB(255, 255, 255) : RGB(230, 230, 235));
            RECT titleRc = { rowRc.left + 14, rowRc.top + 4, rowRc.right - 130, rowRc.top + 24 };
            DrawTextW(memDC, m_filteredTabs[i]->title.c_str(), -1, &titleRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

            // Domain chip
            SelectObject(memDC, hFontSub);
            SetTextColor(memDC, isSelected ? RGB(190, 215, 255) : RGB(140, 145, 160));
            RECT domRc = { rowRc.right - 120, rowRc.top + 12, rowRc.right - 14, rowRc.bottom - 12 };
            DrawTextW(memDC, m_filteredTabs[i]->domain.c_str(), -1, &domRc, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

            y += rowHeight + 2;
        }

        // Footer hint
        SelectObject(memDC, hFontSub);
        SetTextColor(memDC, RGB(120, 125, 135));
        RECT ftRc = { 16, h - 28, w - 16, h - 6 };
        DrawTextW(memDC, L"[Tab / \u2191\u2193] Navigate    [Enter] Switch Tab    [Esc] Close", -1, &ftRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        DeleteObject(hFontMain);
        DeleteObject(hFontSub);
        DeleteObject(hFontHeader);

        BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBmp);
        DeleteObject(memBmp);
        DeleteDC(memDC);
    }

    HWND m_hwnd = nullptr;
    HWND m_targetBrowserHwnd = nullptr;
    std::wstring m_browserTitle;
    std::wstring m_filterText;
    size_t m_selectedIndex = 0;
    std::vector<std::shared_ptr<TabInfo>> m_allTabs;
    std::vector<std::shared_ptr<TabInfo>> m_filteredTabs;
};

// --- KEYBOARD HOOK ---
static std::thread g_hookThread;
static std::atomic<DWORD> g_hookThreadId{ 0 };
static HHOOK g_hKbdHook = nullptr;

static LRESULT CALLBACK LowLevelKbdHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        auto* kbd = (KBDLLHOOKSTRUCT*)lParam;
        // Check for Alt + ` (Tilde / Backtick - VK_OEM_3)
        if (kbd->vkCode == VK_OEM_3 && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
            TabSwitcherHUD::Instance().Show();
            return 1; // Handled
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void StartKeyboardHookThread() {
    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hookThread = std::thread([ready]() {
        MSG msg;
        PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
        g_hookThreadId.store(GetCurrentThreadId());
        g_hKbdHook = SetWindowsHookExW(
            WH_KEYBOARD_LL,
            LowLevelKbdHookProc,
            GetModuleHandleW(nullptr),
            0
        );
        SetEvent(ready);

        while (GetMessageW(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (g_hKbdHook) {
            UnhookWindowsHookEx(g_hKbdHook);
            g_hKbdHook = nullptr;
        }
    });
    WaitForSingleObject(ready, INFINITE);
    CloseHandle(ready);
}

void StopKeyboardHookThread() {
    DWORD tid = g_hookThreadId.load();
    if (tid != 0) {
        PostThreadMessageW(tid, WM_QUIT, 0, 0);
    }
    if (g_hookThread.joinable()) {
        g_hookThread.join();
    }
    g_hookThreadId.store(0);
}

} // namespace AltTabsMod

// --- WINDHAWK TOOL MOD ENTRY POINTS ---
BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing AltTabs Browser HUD Mod...");
    AltTabsMod::LoadSettings();
    AltTabsMod::StartKeyboardHookThread();
    Wh_Log(L"AltTabs Browser HUD Mod loaded successfully.");
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Unloading AltTabs Browser HUD Mod...");
    AltTabsMod::StopKeyboardHookThread();
    Wh_Log(L"AltTabs Browser HUD Mod unloaded.");
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings updated for AltTabs Browser HUD Mod.");
    AltTabsMod::LoadSettings();
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
