// ==WindhawkMod==
// @id              chromium-alttab-tabs
// @name            Chromium Tabs in Alt-Tab (Domain Grouped)
// @description     Injects Chromium browser tabs (Chrome, Brave, Edge, Opera, Vivaldi) into Windows Alt-Tab switcher, smartly grouped by root domain.
// @version         1.0.1
// @author          Dszae
// @github          https://github.com/Dszae
// @homepage        https://github.com/Dszae/AltTabs
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore -luuid -lshell32 -lgdi32 -ldwmapi -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chromium Tabs in Alt-Tab (Domain Grouped)

Injects tabs from Chromium-based web browsers (Google Chrome, Brave, Microsoft Edge, Opera, Vivaldi)
directly into the native Windows Alt-Tab task switcher, grouped by root domain
(e.g., stacking all GitHub tabs into a single Alt-Tab card) to prevent switcher clutter.

![Alt-Tab Simulator](https://raw.githubusercontent.com/Dszae/AltTabs/main/brag-output/brag.jpg)

### What it does:
- Discovers open browser tabs across Chromium browsers using Windows UI Automation.
- Groups multiple tabs from the same website into a single Alt-Tab card showing the domain name and tab count.
- Single tabs from distinct domains appear as individual cards.
- Off-screen proxy windows are excluded from the Windows taskbar via `ITaskbarList::DeleteTab`, remaining visible only in the Alt-Tab switcher.
- Selecting a card restores and focuses the browser window and activates the corresponding tab.

### Notes & Limitations:
- Running as a dedicated Windhawk Tool Mod ensures complete isolation from `explorer.exe`.
- Virtual cards show the tab title and browser icon; thumbnails reflect the browser window state.
- In Edge, if "Show tabs in Alt+Tab" is already enabled in Windows Settings, consider disabling that setting or disabling Edge in this mod to avoid duplicate entries.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- GroupByDomain: true
  $name: Group tabs by root domain
  $description: Stacks tabs from the same website into a single Alt-Tab thumbnail.
- MinTabsToGroup: 2
  $name: Minimum tabs for domain grouping
  $description: Single tabs will appear individually unless tab count reaches this threshold.
- ScanIntervalMs: 1500
  $name: Background Tab Refresh Interval (ms)
  $description: Interval between tab strip state polls. Lower values increase responsiveness.
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
#include <shobjidl.h>
#include <memory>

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <regex>
#include <sstream>
#include <algorithm>

#include <windhawk_api.h>

namespace ChromiumAltTabMod {

// --- CONFIGURATION ---
struct ModSettings {
    bool groupByDomain = true;
    int minTabsToGroup = 2;
    int scanIntervalMs = 1500;
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
    
    int minTabs = Wh_GetIntSetting(L"MinTabsToGroup");
    g_settings.minTabsToGroup = (minTabs >= 2) ? minTabs : 2;

    int interval = Wh_GetIntSetting(L"ScanIntervalMs");
    g_settings.scanIntervalMs = (interval >= 500) ? interval : 1500;

    g_settings.enableChrome = Wh_GetIntSetting(L"EnableChrome") != 0;
    g_settings.enableBrave = Wh_GetIntSetting(L"EnableBrave") != 0;
    g_settings.enableEdge = Wh_GetIntSetting(L"EnableEdge") != 0;
    g_settings.enableOpera = Wh_GetIntSetting(L"EnableOpera") != 0;
    g_settings.enableVivaldi = Wh_GetIntSetting(L"EnableVivaldi") != 0;
}

// Lightweight standard Win32 COM Smart Pointer (MinGW & MSVC compatible, replacing ATL)
template <typename T>
class SafeComPtr {
    T* m_ptr = nullptr;
public:
    SafeComPtr() = default;
    SafeComPtr(T* ptr) : m_ptr(ptr) { if (m_ptr) m_ptr->AddRef(); }
    ~SafeComPtr() { Release(); }

    SafeComPtr(const SafeComPtr& other) : m_ptr(other.m_ptr) {
        if (m_ptr) m_ptr->AddRef();
    }
    SafeComPtr& operator=(const SafeComPtr& other) {
        if (this != std::addressof(other)) {
            Release();
            m_ptr = other.m_ptr;
            if (m_ptr) m_ptr->AddRef();
        }
        return *this;
    }

    SafeComPtr(SafeComPtr&& other) noexcept : m_ptr(other.m_ptr) {
        other.m_ptr = nullptr;
    }
    SafeComPtr& operator=(SafeComPtr&& other) noexcept {
        if (this != std::addressof(other)) {
            Release();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    void Release() {
        if (m_ptr) {
            m_ptr->Release();
            m_ptr = nullptr;
        }
    }

    T* Get() const { return m_ptr; }
    T** GetAddressOf() { Release(); return &m_ptr; }
    T** operator&() { Release(); return &m_ptr; }
    T* operator->() const { return m_ptr; }
    operator T*() const { return m_ptr; }
    operator bool() const { return m_ptr != nullptr; }
};

// --- DATA STRUCTURES ---
struct TabItem {
    std::wstring title;
    std::wstring url;
    std::wstring domain;
    HWND browserHwnd = nullptr;
    RECT boundingRect = { 0, 0, 0, 0 };
    bool isSelected = false;
    SafeComPtr<IUIAutomationElement> uiaElement;
    HICON hFavicon = nullptr;

    TabItem() = default;
    ~TabItem() {
        if (hFavicon) {
            DestroyIcon(hFavicon);
            hFavicon = nullptr;
        }
    }

    TabItem(TabItem&& other) noexcept 
        : title(std::move(other.title)),
          url(std::move(other.url)),
          domain(std::move(other.domain)),
          browserHwnd(other.browserHwnd),
          boundingRect(other.boundingRect),
          isSelected(other.isSelected),
          uiaElement(std::move(other.uiaElement)),
          hFavicon(other.hFavicon) {
        other.hFavicon = nullptr;
    }

    TabItem& operator=(TabItem&& other) noexcept {
        if (this != std::addressof(other)) {
            if (hFavicon) DestroyIcon(hFavicon);
            title = std::move(other.title);
            url = std::move(other.url);
            domain = std::move(other.domain);
            browserHwnd = other.browserHwnd;
            boundingRect = other.boundingRect;
            isSelected = other.isSelected;
            uiaElement = std::move(other.uiaElement);
            hFavicon = other.hFavicon;
            other.hFavicon = nullptr;
        }
        return *this;
    }

    TabItem(const TabItem&) = delete;
    TabItem& operator=(const TabItem&) = delete;
};

struct DomainCluster {
    std::wstring clusterKey;
    std::wstring displayDomain;
    std::vector<std::shared_ptr<TabItem>> tabs;
    HWND proxyHwnd = nullptr;
    HTHUMBNAIL hThumbnail = nullptr;
    size_t activeTabIndex = 0;
};

// Recursive mutex to prevent self-deadlock when ReconcileProxyWindows triggers synchronous WM_ACTIVATE
static std::recursive_mutex g_proxyMapMutex;
static std::unordered_map<HWND, std::shared_ptr<DomainCluster>> g_proxyMap;

// --- DOMAIN PARSER ---
class DomainParser {
public:
    static std::wstring ExtractDomain(const std::wstring& url, const std::wstring& title, size_t fallbackIndex) {
        if (!url.empty()) {
            std::wregex urlRegex(LR"(^(?:https?:\/\/)?(?:www\.)?([a-zA-Z0-9_\-\.]+))");
            std::wsmatch match;
            if (std::regex_search(url, match, urlRegex) && match.size() > 1) {
                std::wstring host = match.str(1);
                std::transform(host.begin(), host.end(), host.begin(), ::towlower);
                std::wstring root = GetRootDomain(host);
                if (!root.empty()) return root;
            }
        }

        // Clean title by removing browser branding suffixes
        std::wstring clean = title;
        const std::wstring suffixes[] = { 
            L" - Google Chrome", L" - Brave", L" - Microsoft Edge", 
            L" - Chromium", L" - Opera", L" - Vivaldi" 
        };
        for (const auto& s : suffixes) {
            if (clean.length() >= s.length() && clean.compare(clean.length() - s.length(), s.length(), s) == 0) {
                clean = clean.substr(0, clean.length() - s.length());
                break;
            }
        }

        std::wstring lower = clean;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);

        if (lower.find(L"github") != std::wstring::npos) return L"github.com";
        if (lower.find(L"youtube") != std::wstring::npos) return L"youtube.com";
        if (lower.find(L"reddit") != std::wstring::npos) return L"reddit.com";
        if (lower.find(L"google") != std::wstring::npos) return L"google.com";
        if (lower.find(L"stack overflow") != std::wstring::npos) return L"stackoverflow.com";
        if (lower.find(L"chatgpt") != std::wstring::npos) return L"chatgpt.com";
        if (lower.find(L"claude") != std::wstring::npos) return L"claude.ai";
        if (lower.find(L"wikipedia") != std::wstring::npos) return L"wikipedia.org";
        if (lower.find(L"twitter") != std::wstring::npos || lower.find(L"x.com") != std::wstring::npos) return L"x.com";

        // Never merge unrelated unknown tabs into a shared key.
        return L"";
    }

    static std::wstring GetRootDomain(const std::wstring& host) {
        if (host == L"localhost" || host.rfind(L"127.", 0) == 0 || host.rfind(L"192.168.", 0) == 0) return host;
        
        size_t lastDot = host.find_last_of(L'.');
        if (lastDot == std::wstring::npos) return host;
        size_t secondLastDot = host.find_last_of(L'.', lastDot - 1);
        if (secondLastDot == std::wstring::npos) return host;

        return host.substr(secondLastDot + 1);
    }
};

// --- ACTIVATION ROUTER ---
class TabActivator {
public:
    static void SwitchToWindow(HWND hwnd) {
        if (!IsWindow(hwnd)) return;
        typedef void (WINAPI *pfnSwitchToThisWindow)(HWND, BOOL);
        static pfnSwitchToThisWindow s_pfn = (pfnSwitchToThisWindow)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SwitchToThisWindow");
        if (IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);
        }
        if (s_pfn) {
            s_pfn(hwnd, TRUE);
        } else {
            SetForegroundWindow(hwnd);
        }
    }

    static bool ForceForeground(HWND hwnd) {
        if (!IsWindow(hwnd)) return false;
        SwitchToWindow(hwnd);
        return true;
    }

    static bool ActivateTab(const std::shared_ptr<TabItem>& tab) {
        if (!tab) return false;
        ForceForeground(tab->browserHwnd);

        // 1. Try UIA SelectionItemPattern
        if (tab->uiaElement) {
            SafeComPtr<IUIAutomationSelectionItemPattern> pSel;
            if (SUCCEEDED(tab->uiaElement->GetCurrentPatternAs(UIA_SelectionItemPatternId, IID_IUIAutomationSelectionItemPattern, (void**)&pSel)) && pSel) {
                if (SUCCEEDED(pSel->Select())) return true;
            }

            SafeComPtr<IUIAutomationInvokePattern> pInv;
            if (SUCCEEDED(tab->uiaElement->GetCurrentPatternAs(UIA_InvokePatternId, IID_IUIAutomationInvokePattern, (void**)&pInv)) && pInv) {
                if (SUCCEEDED(pInv->Invoke())) return true;
            }
        }

        // 2. Coordinate fallback click
        if (tab->boundingRect.right > tab->boundingRect.left) {
            POINT pt = {
                tab->boundingRect.left + (tab->boundingRect.right - tab->boundingRect.left) / 2,
                tab->boundingRect.top + (tab->boundingRect.bottom - tab->boundingRect.top) / 2
            };
            if (ScreenToClient(tab->browserHwnd, &pt)) {
                LPARAM lParam = MAKELPARAM(pt.x, pt.y);
                PostMessageW(tab->browserHwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
                PostMessageW(tab->browserHwnd, WM_LBUTTONUP, 0, lParam);
                return true;
            }
        }

        return false;
    }
};

// --- CORE MANAGER ---
class ChromiumAltTabCore {
public:
    static ChromiumAltTabCore& Instance() {
        static ChromiumAltTabCore s_instance;
        return s_instance;
    }

    bool Start() {
        RegisterWindowClass();
        m_hWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        m_running.store(true);
        m_worker = std::thread(&ChromiumAltTabCore::WorkerThread, this);
        return true;
    }

    void Stop() {
        if (!m_running.load()) return;
        m_running.store(false);
        if (m_hWakeEvent) {
            SetEvent(m_hWakeEvent);
        }
        if (m_worker.joinable()) {
            m_worker.join();
        }
        if (m_hWakeEvent) {
            CloseHandle(m_hWakeEvent);
            m_hWakeEvent = nullptr;
        }
        UnregisterWindowClass();
    }

    void TriggerImmediateScan() {
        if (m_hWakeEvent) {
            SetEvent(m_hWakeEvent);
        }
    }

    static UINT GetTaskbarCreatedMsg() {
        static UINT s_uMsg = RegisterWindowMessageW(L"TaskbarCreated");
        return s_uMsg;
    }

    static void RemoveFromTaskbar(HWND hwnd) {
        // Preserved for taskbar list initialization without DeleteTab,
        // ensuring the window remains visible in Windows 11 Alt-Tab.
    }

    static LRESULT CALLBACK ProxyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == GetTaskbarCreatedMsg()) {
            return 0;
        }

        if (uMsg == WM_ACTIVATE && LOWORD(wParam) != WA_INACTIVE) {
            // Postpone activation to next message cycle so Alt-Tab overlay finishes dismissing
            PostMessageW(hwnd, WM_APP + 42, 0, 0);
            return 0;
        }

        if (uMsg == (WM_APP + 42)) {
            std::shared_ptr<DomainCluster> cluster;
            {
                std::lock_guard<std::recursive_mutex> lock(g_proxyMapMutex);
                auto it = g_proxyMap.find(hwnd);
                if (it != g_proxyMap.end()) {
                    cluster = it->second;
                }
            }

            if (cluster && !cluster->tabs.empty()) {
                size_t idx = cluster->activeTabIndex < cluster->tabs.size() ? cluster->activeTabIndex : 0;
                TabActivator::ActivateTab(cluster->tabs[idx]);
            }
            return 0;
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

private:
    ChromiumAltTabCore() = default;
    ~ChromiumAltTabCore() { Stop(); }

    void RegisterWindowClass() {
        WNDCLASSEXW wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = ProxyWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = L"ChromiumAltTab_VirtualHost";
        RegisterClassExW(&wc);
    }

    void UnregisterWindowClass() {
        UnregisterClassW(L"ChromiumAltTab_VirtualHost", GetModuleHandleW(nullptr));
    }

    void WorkerThread() {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        SafeComPtr<IUIAutomation> pUIA;
        if (FAILED(CoCreateInstance(CLSID_CUIAutomation8, nullptr, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pUIA)) || !pUIA) {
            CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pUIA);
        }

        if (!pUIA) {
            CoUninitialize();
            return;
        }

        VARIANT varProp;
        varProp.vt = VT_I4;
        varProp.lVal = UIA_TabItemControlTypeId;
        SafeComPtr<IUIAutomationCondition> pTabCond;
        pUIA->CreatePropertyCondition(UIA_ControlTypePropertyId, varProp, &pTabCond);

        // Cache tab item's own properties
        SafeComPtr<IUIAutomationCacheRequest> pCache;
        pUIA->CreateCacheRequest(&pCache);
        pCache->AddProperty(UIA_NamePropertyId);
        pCache->AddProperty(UIA_BoundingRectanglePropertyId);
        pCache->AddProperty(UIA_SelectionItemIsSelectedPropertyId);
        pCache->AddPattern(UIA_SelectionItemPatternId);
        pCache->AddPattern(UIA_InvokePatternId);
        pCache->put_TreeScope(TreeScope_Element);

        ULONGLONG nextScan = 0;

        while (m_running.load()) {
            ULONGLONG now = GetTickCount64();
            DWORD timeout = (now >= nextScan) ? 0 : (DWORD)(nextScan - now);

            DWORD res = MsgWaitForMultipleObjectsEx(1, &m_hWakeEvent, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (!m_running.load()) break;

            if (res == WAIT_OBJECT_0 + 1) {
                // Pump messages only, do not scan
                MSG msg;
                while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
                continue;
            }

            PerformTabScan(pUIA, pTabCond, pCache);

            int intervalMs = 1500;
            {
                std::lock_guard<std::mutex> lock(g_settingsMutex);
                intervalMs = g_settings.scanIntervalMs;
            }
            nextScan = GetTickCount64() + intervalMs;
        }

        DestroyProxyWindows();
        pCache.Release();
        pTabCond.Release();
        pUIA.Release();
        CoUninitialize();
    }

    static BOOL CALLBACK EnumBrowserWindowsProc(HWND hwnd, LPARAM lp) {
        if (!IsWindowVisible(hwnd)) return TRUE;
        wchar_t cls[64] = { 0 };
        GetClassNameW(hwnd, cls, 64);
        if (wcscmp(cls, L"Chrome_WidgetWin_1") != 0) return TRUE;

        RECT rc;
        if (!GetWindowRect(hwnd, &rc) || (rc.right - rc.left < 200)) return TRUE;

        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!hProc) return TRUE;

        wchar_t path[MAX_PATH] = { 0 };
        DWORD sz = MAX_PATH;
        QueryFullProcessImageNameW(hProc, 0, path, &sz);
        CloseHandle(hProc);

        std::wstring sPath(path);
        std::transform(sPath.begin(), sPath.end(), sPath.begin(), ::towlower);

        bool matches = false;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            if (g_settings.enableChrome && sPath.find(L"chrome.exe") != std::wstring::npos) matches = true;
            if (g_settings.enableBrave && sPath.find(L"brave.exe") != std::wstring::npos) matches = true;
            if (g_settings.enableEdge && sPath.find(L"msedge.exe") != std::wstring::npos) matches = true;
            if (g_settings.enableOpera && sPath.find(L"opera.exe") != std::wstring::npos) matches = true;
            if (g_settings.enableVivaldi && sPath.find(L"vivaldi.exe") != std::wstring::npos) matches = true;
        }

        if (matches) {
            auto* pList = reinterpret_cast<std::vector<HWND>*>(lp);
            if (pList) pList->push_back(hwnd);
        }
        return TRUE;
    }

    void PerformTabScan(IUIAutomation* pUIA, IUIAutomationCondition* pCond, IUIAutomationCacheRequest* pCache) {
        std::vector<HWND> browserWindows;
        EnumWindows(EnumBrowserWindowsProc, reinterpret_cast<LPARAM>(&browserWindows));

        std::vector<std::shared_ptr<TabItem>> extractedTabs;
        size_t tabCounter = 0;

        for (HWND bHwnd : browserWindows) {
            // Wake up Chromium accessibility tree if idle
            DWORD_PTR dwResult = 0;
            SendMessageTimeoutW(bHwnd, WM_GETOBJECT, 0, (LPARAM)0xFFFFFFFC /* OBJID_CLIENT */, SMTO_ABORTIFHUNG, 50, &dwResult);

            SafeComPtr<IUIAutomationElement> pWinElem;
            if (FAILED(pUIA->ElementFromHandle(bHwnd, &pWinElem)) || !pWinElem) continue;

            // Restrict search to the native tab-strip container to avoid matching in-page ARIA role='tab' elements
            SafeComPtr<IUIAutomationElement> pSearchRoot = pWinElem;

            VARIANT varTabStrip;
            varTabStrip.vt = VT_I4;
            varTabStrip.lVal = UIA_TabControlTypeId;
            SafeComPtr<IUIAutomationCondition> pStripCond;
            if (SUCCEEDED(pUIA->CreatePropertyCondition(UIA_ControlTypePropertyId, varTabStrip, &pStripCond))) {
                SafeComPtr<IUIAutomationElement> pTabStrip;
                if (SUCCEEDED(pWinElem->FindFirst(TreeScope_Descendants, pStripCond, &pTabStrip)) && pTabStrip) {
                    pSearchRoot = pTabStrip;
                }
            }

            SafeComPtr<IUIAutomationElementArray> pArray;
            if (FAILED(pSearchRoot->FindAllBuildCache(TreeScope_Descendants, pCond, pCache, &pArray)) || !pArray) continue;

            int count = 0;
            pArray->get_Length(&count);

            for (int i = 0; i < count; ++i) {
                SafeComPtr<IUIAutomationElement> pElem;
                if (FAILED(pArray->GetElement(i, &pElem)) || !pElem) continue;

                auto tab = std::make_shared<TabItem>();
                tab->browserHwnd = bHwnd;
                tab->uiaElement = pElem;

                BSTR bstrName = nullptr;
                if (SUCCEEDED(pElem->get_CachedName(&bstrName)) && bstrName) {
                    tab->title = bstrName;
                    SysFreeString(bstrName);
                }

                if (tab->title.empty() || tab->title == L"Close") continue;

                BOOL isSel = FALSE;
                SafeComPtr<IUIAutomationSelectionItemPattern> pSel;
                if (SUCCEEDED(pElem->GetCachedPatternAs(UIA_SelectionItemPatternId, IID_IUIAutomationSelectionItemPattern, (void**)&pSel)) && pSel) {
                    pSel->get_CachedIsSelected(&isSel);
                }
                tab->isSelected = (isSel == TRUE);

                RECT rc = { 0 };
                pElem->get_CachedBoundingRectangle(&rc);
                tab->boundingRect = rc;

                // Safe non-blocking icon lookup with timeout
                DWORD_PTR iconResult = 0;
                if (SendMessageTimeoutW(bHwnd, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 50, &iconResult) && iconResult) {
                    tab->hFavicon = CopyIcon((HICON)iconResult);
                }

                tab->domain = DomainParser::ExtractDomain(L"", tab->title, tabCounter++);
                extractedTabs.push_back(std::move(tab));
            }
        }

        UpdateClusters(std::move(extractedTabs));
    }

    void UpdateClusters(std::vector<std::shared_ptr<TabItem>> tabs) {
        bool groupByDomain = true;
        size_t minTabs = 2;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            groupByDomain = g_settings.groupByDomain;
            minTabs = (size_t)g_settings.minTabsToGroup;
        }

        // Group by identified domain
        std::unordered_map<std::wstring, std::vector<std::shared_ptr<TabItem>>> domainBuckets;
        std::vector<std::shared_ptr<TabItem>> unclusteredTabs;

        for (auto& tab : tabs) {
            if (!tab->domain.empty()) {
                domainBuckets[tab->domain].push_back(tab);
            } else {
                unclusteredTabs.push_back(tab);
            }
        }

        std::unordered_map<std::wstring, std::shared_ptr<DomainCluster>> newClusters;

        for (auto& [domain, tabList] : domainBuckets) {
            if (groupByDomain && tabList.size() >= minTabs) {
                auto cluster = std::make_shared<DomainCluster>();
                cluster->clusterKey = domain;
                cluster->displayDomain = domain;
                cluster->tabs = std::move(tabList);
                for (size_t i = 0; i < cluster->tabs.size(); ++i) {
                    if (cluster->tabs[i]->isSelected) cluster->activeTabIndex = i;
                }
                newClusters[domain] = cluster;
            } else {
                for (auto& t : tabList) {
                    unclusteredTabs.push_back(t);
                }
            }
        }

        // Unclustered tabs get unique individual cards
        size_t indIdx = 0;
        for (auto& tab : unclusteredTabs) {
            std::wstring key = L"ind_" + std::to_wstring(indIdx++) + L"_" + tab->title;
            auto cluster = std::make_shared<DomainCluster>();
            cluster->clusterKey = key;
            cluster->displayDomain = tab->domain.empty() ? L"" : tab->domain;
            cluster->tabs.push_back(tab);
            cluster->activeTabIndex = 0;
            newClusters[key] = cluster;
        }

        ReconcileProxyWindows(std::move(newClusters));
    }

    // Stable reconciliation: preserves existing HWNDs and deletes taskbar buttons via ITaskbarList
    void ReconcileProxyWindows(std::unordered_map<std::wstring, std::shared_ptr<DomainCluster>> updatedClusters) {
        std::lock_guard<std::recursive_mutex> lock(g_proxyMapMutex);

        // 1. Transfer existing HWNDs and thumbnails from old clusters to updated clusters
        for (auto& [key, newCluster] : updatedClusters) {
            auto it = m_clusters.find(key);
            if (it != m_clusters.end() && it->second->proxyHwnd && IsWindow(it->second->proxyHwnd)) {
                newCluster->proxyHwnd = it->second->proxyHwnd;
                newCluster->hThumbnail = it->second->hThumbnail;
                it->second->proxyHwnd = nullptr;
                it->second->hThumbnail = nullptr;
            }
        }

        // 2. Destroy windows for removed clusters
        for (auto& [key, oldCluster] : m_clusters) {
            if (oldCluster->hThumbnail) {
                DwmUnregisterThumbnail(oldCluster->hThumbnail);
                oldCluster->hThumbnail = nullptr;
            }
            if (oldCluster->proxyHwnd && IsWindow(oldCluster->proxyHwnd)) {
                g_proxyMap.erase(oldCluster->proxyHwnd);
                DestroyWindow(oldCluster->proxyHwnd);
                oldCluster->proxyHwnd = nullptr;
            }
        }

        // 3. Create or update windows for active clusters
        for (auto& [key, cluster] : updatedClusters) {
            if (cluster->tabs.empty()) continue;

            std::wstringstream ss;
            if (cluster->tabs.size() == 1) {
                ss << cluster->tabs[0]->title;
            } else {
                ss << cluster->displayDomain << L" (" << cluster->tabs.size() << L" tabs)";
            }
            std::wstring title = ss.str();

            HWND hwnd = cluster->proxyHwnd;
            if (!hwnd || !IsWindow(hwnd)) {
                hwnd = CreateWindowExW(
                    WS_EX_APPWINDOW,
                    L"ChromiumAltTab_VirtualHost",
                    title.c_str(),
                    WS_POPUP | WS_CLIPSIBLINGS,
                    -32000, -32000, 400, 300,
                    nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
                );
                if (hwnd) {
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    cluster->proxyHwnd = hwnd;
                }
            } else {
                SetWindowTextW(hwnd, title.c_str());
            }

            if (hwnd) {
                // Register live DWM thumbnail so Windows 11 Alt-Tab renders a real preview instead of a blank card
                if (!cluster->hThumbnail && !cluster->tabs.empty() && cluster->tabs[0]->browserHwnd) {
                    if (SUCCEEDED(DwmRegisterThumbnail(hwnd, cluster->tabs[0]->browserHwnd, &cluster->hThumbnail)) && cluster->hThumbnail) {
                        DWM_THUMBNAIL_PROPERTIES dti = { 0 };
                        dti.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
                        dti.rcDestination = RECT{ 0, 0, 400, 300 };
                        dti.fVisible = TRUE;
                        dti.opacity = 255;
                        DwmUpdateThumbnailProperties(cluster->hThumbnail, &dti);
                    }
                }

                g_proxyMap[hwnd] = cluster;

                if (!cluster->tabs.empty() && cluster->tabs[0]->hFavicon) {
                    SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)cluster->tabs[0]->hFavicon);
                    SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)cluster->tabs[0]->hFavicon);
                }
            }
        }

        m_clusters = std::move(updatedClusters);
    }

    void DestroyProxyWindows() {
        std::lock_guard<std::recursive_mutex> lock(g_proxyMapMutex);
        for (auto& [key, cluster] : m_clusters) {
            if (cluster->hThumbnail) {
                DwmUnregisterThumbnail(cluster->hThumbnail);
                cluster->hThumbnail = nullptr;
            }
            if (cluster->proxyHwnd && IsWindow(cluster->proxyHwnd)) {
                DestroyWindow(cluster->proxyHwnd);
                cluster->proxyHwnd = nullptr;
            }
        }
        g_proxyMap.clear();
        m_clusters.clear();
    }

    std::atomic<bool> m_running{ false };
    HANDLE m_hWakeEvent = nullptr;
    std::thread m_worker;
    std::unordered_map<std::wstring, std::shared_ptr<DomainCluster>> m_clusters;
};

// --- KEYBOARD HOOK THREAD WITH DEDICATED MESSAGE LOOP ---
static std::thread g_hookThread;
static std::atomic<DWORD> g_hookThreadId{ 0 };
static HHOOK g_hKbdHook = nullptr;

static LRESULT CALLBACK LowLevelKbdHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        auto* kbd = (KBDLLHOOKSTRUCT*)lParam;
        if (kbd->vkCode == VK_TAB && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
            ChromiumAltTabCore::Instance().TriggerImmediateScan();
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void StartKeyboardHookThread() {
    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hookThread = std::thread([ready]() {
        MSG msg;
        PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE); // Force message queue creation
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

} // namespace ChromiumAltTabMod

// --- WINDHAWK TOOL MOD ENTRY POINTS ---
BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Chromium Tabs in Alt-Tab Tool Mod v1.0.1...");

    ChromiumAltTabMod::LoadSettings();
    ChromiumAltTabMod::StartKeyboardHookThread();
    ChromiumAltTabMod::ChromiumAltTabCore::Instance().Start();

    Wh_Log(L"Chromium Tabs in Alt-Tab Mod loaded successfully.");
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Unloading Chromium Tabs in Alt-Tab Mod...");

    ChromiumAltTabMod::StopKeyboardHookThread();
    ChromiumAltTabMod::ChromiumAltTabCore::Instance().Stop();

    Wh_Log(L"Chromium Tabs in Alt-Tab Mod unloaded.");
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings updated for Chromium Tabs in Alt-Tab Mod.");
    ChromiumAltTabMod::LoadSettings();
    ChromiumAltTabMod::ChromiumAltTabCore::Instance().TriggerImmediateScan();
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
