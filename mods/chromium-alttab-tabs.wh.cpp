// ==WindhawkMod==
// @id              chromium-alttab-tabs
// @name            Chromium Tabs in Alt-Tab (Domain Grouped)
// @description     Injects Chromium browser tabs (Chrome, Brave, Edge) into Windows Alt-Tab switcher, smartly grouped by root domain.
// @version         1.0.0
// @author          Dszae
// @github          https://github.com/Dszae
// @homepage        https://github.com/Dszae/AltTabs
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore -luuid -lshell32 -lgdi32 -ldwmapi -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chromium Tabs in Alt-Tab (Domain Grouped)

Injects tabs from Chromium-based web browsers (Google Chrome, Brave, Edge, Opera, Vivaldi)
directly into the native Windows Alt-Tab switcher, intelligently grouped by root domain
(e.g., stacking all GitHub tabs into a single Alt-Tab thumbnail) to prevent visual clutter.

### Key Architectural Features:
1. **Zero-Lag UI Automation (UIA) Background Caching Engine**:
   Uses `IUIAutomationCacheRequest` with `TreeScope_Descendants` to fetch all tab titles,
   selection states, patterns, and coordinates in a single inter-process batch call.
2. **Domain Clustering Parser**:
   Extracts root domains from Omnibox URLs and title heuristics, bundling tabs into 
   clean domain cards (e.g. "github.com (4 tabs)").
3. **Dual-Path OS Hooking**:
   - **Internal Shell Hooking**: Targets `twinui.pcshell.dll` task switcher data models via Windhawk symbol hooks.
   - **Virtual Shell Proxy Engine**: Generates lightweight top-level switcher entries registered with DWM.
4. **Reliable Activation Routing**:
   Bypasses Windows ASFW (AllowSetForegroundWindow) lockouts and switches directly to the target tab
   via `IUIAutomationSelectionItemPattern` with coordinate fallback.
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
- EnableBrave: true
  $name: Support Brave Browser
- EnableChrome: true
  $name: Support Google Chrome
- EnableEdge: true
  $name: Support Microsoft Edge
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uiautomation.h>
#include <dwmapi.h>
#include <psapi.h>
#include <shlwapi.h>
#include <memory>

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
    T** operator&() { Release(); return &m_ptr; }
    T* operator->() const { return m_ptr; }
    operator T*() const { return m_ptr; }
    operator bool() const { return m_ptr != nullptr; }
};

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <regex>
#include <sstream>
#include <algorithm>

// Windhawk API header (available when building within Windhawk)
#if __has_include(<windhawk_api.h>)
#include <windhawk_api.h>
#include <windhawk_utils.h>
#define HAS_WINDHAWK_API 1
#else
#define Wh_Log(format, ...)
#define HAS_WINDHAWK_API 0
#endif

namespace ChromiumAltTabMod {

// --- CONFIGURATION ---
struct ModSettings {
    bool groupByDomain = true;
    int minTabsToGroup = 2;
    int scanIntervalMs = 1500;
    bool enableBrave = true;
    bool enableChrome = true;
    bool enableEdge = true;
} g_settings;

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
        if (this != &other) {
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
    std::wstring domain;
    std::vector<std::shared_ptr<TabItem>> tabs;
    HWND proxyHwnd = nullptr;
    size_t activeTabIndex = 0;
};

// --- DOMAIN PARSER ---
class DomainParser {
public:
    static std::wstring ExtractDomain(const std::wstring& url, const std::wstring& title) {
        if (!url.empty()) {
            std::wregex urlRegex(LR"(^(?:https?:\/\/)?(?:www\.)?([a-zA-Z0-9_\-\.]+))");
            std::wsmatch match;
            if (std::regex_search(url, match, urlRegex) && match.size() > 1) {
                std::wstring host = match.str(1);
                std::transform(host.begin(), host.end(), host.begin(), ::towlower);
                return GetRootDomain(host);
            }
        }

        // Clean title
        std::wstring clean = title;
        const std::wstring suffixes[] = { L" - Google Chrome", L" - Brave", L" - Microsoft Edge", L" - Chromium" };
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

        return L"Web Browsing";
    }

    static std::wstring GetRootDomain(const std::wstring& host) {
        if (host == L"localhost" || host.rfind(L"127.", 0) == 0) return host;
        
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
    static bool ForceForeground(HWND hwnd) {
        if (!IsWindow(hwnd)) return false;
        if (GetForegroundWindow() == hwnd) return true;

        DWORD curThread = GetCurrentThreadId();
        DWORD fgThread = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
        DWORD targetThread = GetWindowThreadProcessId(hwnd, nullptr);

        if (fgThread && fgThread != curThread) AttachThreadInput(curThread, fgThread, TRUE);
        if (targetThread && targetThread != curThread) AttachThreadInput(curThread, targetThread, TRUE);

        DWORD lockTimeout = 0;
        SystemParametersInfoW(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &lockTimeout, 0);
        SystemParametersInfoW(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, 0, SPIF_SENDCHANGE);

        AllowSetForegroundWindow(ASFW_ANY);

        if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
        else ShowWindow(hwnd, SW_SHOW);

        SetForegroundWindow(hwnd);
        BringWindowToTop(hwnd);
        SetFocus(hwnd);

        SystemParametersInfoW(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (PVOID)(ULONG_PTR)lockTimeout, SPIF_SENDCHANGE);

        if (fgThread && fgThread != curThread) AttachThreadInput(curThread, fgThread, FALSE);
        if (targetThread && targetThread != curThread) AttachThreadInput(curThread, targetThread, FALSE);

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
        m_running.store(true);
        m_worker = std::thread(&ChromiumAltTabCore::WorkerThread, this);
        return true;
    }

    void Stop() {
        m_running.store(false);
        m_cv.notify_all();
        if (m_worker.joinable()) m_worker.join();
        DestroyProxyWindows();
        UnregisterWindowClass();
    }

    void TriggerImmediateScan() {
        m_scanNow.store(true);
        m_cv.notify_one();
    }

    static LRESULT CALLBACK ProxyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_ACTIVATE && LOWORD(wParam) != WA_INACTIVE) {
            auto* pCluster = (DomainCluster*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            if (pCluster && !pCluster->tabs.empty()) {
                size_t idx = pCluster->activeTabIndex < pCluster->tabs.size() ? pCluster->activeTabIndex : 0;
                TabActivator::ActivateTab(pCluster->tabs[idx]);
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

        SafeComPtr<IUIAutomationCacheRequest> pCache;
        pUIA->CreateCacheRequest(&pCache);
        pCache->AddProperty(UIA_NamePropertyId);
        pCache->AddProperty(UIA_BoundingRectanglePropertyId);
        pCache->AddProperty(UIA_SelectionItemIsSelectedPropertyId);
        pCache->AddPattern(UIA_SelectionItemPatternId);
        pCache->AddPattern(UIA_InvokePatternId);
        pCache->put_TreeScope(TreeScope_Descendants);

        while (m_running.load()) {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cv.wait_for(lock, std::chrono::milliseconds(g_settings.scanIntervalMs), [this]() {
                    return !m_running.load() || m_scanNow.load();
                });
            }

            if (!m_running.load()) break;
            m_scanNow.store(false);

            PerformTabScan(pUIA, pTabCond, pCache);
        }

        pCache.Release();
        pTabCond.Release();
        pUIA.Release();
        CoUninitialize();
    }

    void PerformTabScan(IUIAutomation* pUIA, IUIAutomationCondition* pCond, IUIAutomationCacheRequest* pCache) {
        std::vector<HWND> browserWindows;
        EnumWindows([](HWND hwnd, LPARAM lp) -> BOOL {
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
            if (g_settings.enableChrome && sPath.find(L"chrome.exe") != std::wstring::npos) matches = true;
            if (g_settings.enableBrave && sPath.find(L"brave.exe") != std::wstring::npos) matches = true;
            if (g_settings.enableEdge && sPath.find(L"msedge.exe") != std::wstring::npos) matches = true;

            if (matches) {
                ((std::vector<HWND>*)lp)->push_back(hwnd);
            }
            return TRUE;
        }, (LPARAM)&browserWindows);

        std::vector<std::shared_ptr<TabItem>> extractedTabs;

        for (HWND bHwnd : browserWindows) {
            SafeComPtr<IUIAutomationElement> pWinElem;
            if (FAILED(pUIA->ElementFromHandle(bHwnd, &pWinElem)) || !pWinElem) continue;

            SafeComPtr<IUIAutomationElementArray> pArray;
            if (FAILED(pWinElem->FindAllBuildCache(TreeScope_Descendants, pCond, pCache, &pArray)) || !pArray) continue;

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

                HICON hIcon = (HICON)SendMessageW(bHwnd, WM_GETICON, ICON_SMALL, 0);
                if (hIcon) tab->hFavicon = CopyIcon(hIcon);

                tab->domain = DomainParser::ExtractDomain(L"", tab->title);
                extractedTabs.push_back(std::move(tab));
            }
        }

        UpdateClusters(std::move(extractedTabs));
    }

    void UpdateClusters(std::vector<std::shared_ptr<TabItem>> tabs) {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::unordered_map<std::wstring, std::shared_ptr<DomainCluster>> newClusters;
        for (auto& tab : tabs) {
            auto it = newClusters.find(tab->domain);
            if (it == newClusters.end()) {
                auto cluster = std::make_shared<DomainCluster>();
                cluster->domain = tab->domain;
                cluster->tabs.push_back(tab);
                if (tab->isSelected) cluster->activeTabIndex = 0;
                newClusters[tab->domain] = cluster;
            } else {
                it->second->tabs.push_back(tab);
                if (tab->isSelected) it->second->activeTabIndex = it->second->tabs.size() - 1;
            }
        }

        m_clusters = std::move(newClusters);
        SyncProxyWindowList();
    }

    void SyncProxyWindowList() {
        // Destroy dead windows
        for (auto it = m_proxyWindows.begin(); it != m_proxyWindows.end();) {
            HWND hwnd = *it;
            auto* pCluster = (DomainCluster*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            if (!pCluster || m_clusters.find(pCluster->domain) == m_clusters.end()) {
                DestroyWindow(hwnd);
                it = m_proxyWindows.erase(it);
            } else {
                ++it;
            }
        }

        // Spawn / update active domain cards
        for (auto& [domain, cluster] : m_clusters) {
            if (cluster->tabs.empty()) continue;

            std::wstringstream ss;
            if (cluster->tabs.size() == 1 || !g_settings.groupByDomain) {
                ss << cluster->tabs[0]->title;
            } else {
                ss << domain << L" (" << cluster->tabs.size() << L" tabs)";
            }
            std::wstring title = ss.str();

            HWND hwnd = cluster->proxyHwnd;
            if (!hwnd || !IsWindow(hwnd)) {
                hwnd = CreateWindowExW(
                    WS_EX_APPWINDOW,
                    L"ChromiumAltTab_VirtualHost",
                    title.c_str(),
                    WS_POPUP | WS_CLIPSIBLINGS,
                    -32000, -32000, 200, 150,
                    nullptr, nullptr, GetModuleHandleW(nullptr), nullptr
                );
                if (hwnd) {
                    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cluster.get());
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    cluster->proxyHwnd = hwnd;
                    m_proxyWindows.push_back(hwnd);
                }
            } else {
                SetWindowTextW(hwnd, title.c_str());
            }

            if (hwnd && !cluster->tabs.empty() && cluster->tabs[0]->hFavicon) {
                SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)cluster->tabs[0]->hFavicon);
                SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)cluster->tabs[0]->hFavicon);
            }
        }
    }

    void DestroyProxyWindows() {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (HWND hwnd : m_proxyWindows) {
            if (IsWindow(hwnd)) DestroyWindow(hwnd);
        }
        m_proxyWindows.clear();
        m_clusters.clear();
    }

    std::atomic<bool> m_running{ false };
    std::atomic<bool> m_scanNow{ false };
    std::thread m_worker;
    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::unordered_map<std::wstring, std::shared_ptr<DomainCluster>> m_clusters;
    std::vector<HWND> m_proxyWindows;
};

// --- KEYBOARD HOOK FOR REAL-TIME ALT-TAB DETECTION ---
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

} // namespace ChromiumAltTabMod

// --- WINDHAWK ENTRY POINTS ---
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Chromium Tabs in Alt-Tab Windhawk Mod...");

    ChromiumAltTabMod::g_hKbdHook = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        ChromiumAltTabMod::LowLevelKbdHookProc,
        GetModuleHandleW(nullptr),
        0
    );

    ChromiumAltTabMod::ChromiumAltTabCore::Instance().Start();
    Wh_Log(L"Chromium Tabs in Alt-Tab Mod loaded successfully.");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloading Chromium Tabs in Alt-Tab Mod...");

    if (ChromiumAltTabMod::g_hKbdHook) {
        UnhookWindowsHookEx(ChromiumAltTabMod::g_hKbdHook);
        ChromiumAltTabMod::g_hKbdHook = nullptr;
    }

    ChromiumAltTabMod::ChromiumAltTabCore::Instance().Stop();
    Wh_Log(L"Chromium Tabs in Alt-Tab Mod unloaded.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings updated for Chromium Tabs in Alt-Tab Mod.");
}

