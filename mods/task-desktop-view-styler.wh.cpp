// ==WindhawkMod==
// @id              task-desktop-view-styler
// @name            Task View / Desktop View Styler
// @description     Restyles the native Windows 11 Task View (Desktop View) — rounded corners, cleaner backdrop, transparent desktops bar — by editing its live XAML tree, no overlay, non-invasive
// @version         4.5.0
// @author          mahtab-ali
// @github          https://github.com/mahtab-ali
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -loleacc
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task View / Desktop View Styler

![Styled Task View preview](https://raw.githubusercontent.com/mahtab-ali/taskview-clean-styler/main/screenshot.png)

Restyle the Windows 11 **Task View** (the **Desktop View** / virtual-desktops
switcher you get with `Win+Tab`) to give the window cards and desktop
thumbnails rounded corners, a cleaner backdrop, and an optional transparent
desktops bar.

Instead of drawing its own window, this mod **restyles the real Windows Task
View in place**. It attaches to Explorer's live XAML visual tree through
`InitializeXamlDiagnosticsEx` (the same, non-invasive mechanism the
"Windows 11 Taskbar Styler" mod uses) and, as Task View's elements are created,
sets `CornerRadius` / `Background` on the ones you target — giving the thumbnails
and panels rounded corners and a cleaner look without replacing anything.

Works out of the box: rounds the thumbnail cards (and their hover/selection
backplates, so corners never conflict), optionally flattens borders, and can
clear the bottom desktop bar's background.

## Tuning for your Windows build

The XAML element class names inside Task View change between builds, so the
target lists are configurable:

1. Turn on **Log element types (diagnostics)**.
2. Open Task View (Win+Tab) and watch the log (Windhawk log viewer or DebugView).
   You'll see `XAML + <Type>  name=<...>` for each element.
3. Add a distinctive substring of the element you want to style to **Round-corner
   targets** or **Clear-background targets** (matching is case-insensitive and
   checks both the type and the x:Name).
4. Turn diagnostics **off** for daily use — it's verbose and slows Task View.

This only *sets properties* on existing elements — no windows, no input hooks,
nothing written to disk. Disabling the mod reverts on the next Task View open.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cornerRadius: 8
  $name: Corner radius
  $description: Rounded-corner radius (px) applied to matched elements AND their hover/selection backplates, so the corners never conflict.
- removeBorders: true
  $name: Remove borders
  $description: Also zero the border thickness on rounded elements for a flatter, cleaner look.
- background: ""
  $name: Background tint (optional)
  $description: Hex color like #202024 or #CC202024 (AARRGGBB) painted on matched Panels/Borders/Controls. Empty = leave as-is.
- targets:
  - "SwitchItemListViewItem"
  - "VirtualDesktopThumbnailButton"
  - "BackgroundBorder"
  - "ListViewItemPresenter"
  - "ThumbnailBorderHighlight"
  $name: Round-corner targets
  $description: Round the corners of elements whose XAML type name or x_Name contains any of these substrings (case-insensitive). The card AND its hover/selection backplate are both listed so their radii match.
- clearBackground:
  - "VirtualDesktopBarBackground"
  $name: Clear-background targets
  $description: Make the background/backplate transparent on elements matching these substrings. VirtualDesktopBarBackground is the desktops-bar backplate, so only the clean desktops show.
- hideElements:
  - ""
  $name: Hide-element targets
  $description: Collapse (hide) elements matching these substrings. Use it to remove an extra overlay/backplate that draws on top of the default one. Find its name with diagnostics logging.
- moveDesktopsToTop:
  - ""
  $name: Move-to-top targets (EXPERIMENTAL)
  $description: Pin matching elements to the TOP via VerticalAlignment=Top + Grid row 0. WARNING — this fights Task View's layout animation, so the desktops slide up from the bottom and dragging gets jagged. Leave empty for the clean native fade. Only fill in if you accept the janky animation.
- logElementTypes: false
  $name: Log element types (diagnostics)
  $description: Write every created XAML element's type/name to the Windhawk log so you can discover what to target. Turn OFF for normal use — it is verbose and slows Task View.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <ocidl.h>   // IObjectWithSite
#include <xamlom.h>

// The Win32 GetCurrentTime() macro (-> GetTickCount) collides with a WinRT XAML
// method of the same name; undefine it before pulling in the projections.
#undef GetCurrentTime

#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <new>
#include <cstdarg>

namespace xaml = winrt::Windows::UI::Xaml;
namespace xc   = winrt::Windows::UI::Xaml::Controls;
namespace xm   = winrt::Windows::UI::Xaml::Media;

// ===================== Diagnostics → plain text file ===================
// Everything is mirrored to %TEMP%\taskview-styler.log so you can just open
// that file in Notepad instead of hunting through Windhawk's log viewer.
static std::wstring g_logPath;
static bool         g_fileLogEnabled = false;  // only write the temp file when diagnostics are on

static void FileLog(const wchar_t* line) {
    if (!g_fileLogEnabled || g_logPath.empty()) return;
    HANDLE h = CreateFileW(g_logPath.c_str(), FILE_APPEND_DATA,
                           FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                           OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    std::wstring l = std::wstring(line) + L"\r\n";
    int n = WideCharToMultiByte(CP_UTF8, 0, l.c_str(), (int)l.size(), nullptr, 0, nullptr, nullptr);
    if (n > 0) {
        std::string s((size_t)n, '\0');
        WideCharToMultiByte(CP_UTF8, 0, l.c_str(), (int)l.size(), s.data(), n, nullptr, nullptr);
        DWORD written = 0;
        WriteFile(h, s.data(), (DWORD)s.size(), &written, nullptr);
    }
    CloseHandle(h);
}

static void LogF(const wchar_t* fmt, ...) {
    wchar_t buf[1200];
    va_list ap;
    va_start(ap, fmt);
    vswprintf(buf, 1200, fmt, ap);
    va_end(ap);
    Wh_Log(L"%s", buf);
    FileLog(buf);
}

// CLSID for our TAP (Tools-Assisted-Platform) object. Arbitrary but unique.
// {7C9D4B21-3E5A-4F18-9C2B-1A6E0F3D5B72}
static const CLSID CLSID_WindhawkTAP =
    { 0x7c9d4b21, 0x3e5a, 0x4f18, { 0x9c, 0x2b, 0x1a, 0x6e, 0x0f, 0x3d, 0x5b, 0x72 } };

// ============================ Settings =================================

struct {
    double            cornerRadius = 8.0;
    bool              removeBorders = true;
    bool              haveBg = false;
    winrt::Windows::UI::Color bgColor{};
    bool              logTypes = false;
    std::vector<std::wstring> targets;       // round corners
    std::vector<std::wstring> clearTargets;  // strip background
    std::vector<std::wstring> hideTargets;   // collapse (hide)
    std::vector<std::wstring> topTargets;    // pin to top
} g_settings;

// Read a Windhawk array setting (key[0], key[1], ...) into `out`.
static void LoadStringList(PCWSTR key, std::vector<std::wstring>& out) {
    out.clear();
    for (int i = 0; ; i++) {
        PCWSTR t = Wh_GetStringSetting(L"%s[%d]", key, i);
        if (!t || !*t) { if (t) Wh_FreeStringSetting(t); break; }
        out.emplace_back(t);
        Wh_FreeStringSetting(t);
    }
}

static bool ParseHexColor(const std::wstring& s, winrt::Windows::UI::Color& out) {
    std::wstring h = s;
    if (!h.empty() && h[0] == L'#') h.erase(0, 1);
    if (h.size() != 6 && h.size() != 8) return false;
    auto hx = [&](int i) -> int {
        wchar_t c = h[i];
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    };
    int v[8];
    for (size_t i = 0; i < h.size(); i++) { v[i] = hx((int)i); if (v[i] < 0) return false; }
    int idx = 0;
    BYTE a = 255;
    if (h.size() == 8) { a = (BYTE)(v[0] * 16 + v[1]); idx = 2; }
    BYTE r = (BYTE)(v[idx] * 16 + v[idx + 1]);
    BYTE g = (BYTE)(v[idx + 2] * 16 + v[idx + 3]);
    BYTE b = (BYTE)(v[idx + 4] * 16 + v[idx + 5]);
    out = winrt::Windows::UI::Color{ a, r, g, b };
    return true;
}

static void LoadSettings() {
    g_settings.cornerRadius  = (double)Wh_GetIntSetting(L"cornerRadius");
    g_settings.removeBorders = Wh_GetIntSetting(L"removeBorders") != 0;
    g_settings.logTypes      = Wh_GetIntSetting(L"logElementTypes") != 0;
    g_fileLogEnabled         = g_settings.logTypes;

    g_settings.haveBg = false;
    if (PCWSTR bg = Wh_GetStringSetting(L"background")) {
        std::wstring s = bg;
        Wh_FreeStringSetting(bg);
        if (!s.empty()) g_settings.haveBg = ParseHexColor(s, g_settings.bgColor);
    }

    LoadStringList(L"targets", g_settings.targets);
    LoadStringList(L"clearBackground", g_settings.clearTargets);
    LoadStringList(L"hideElements", g_settings.hideTargets);
    LoadStringList(L"moveDesktopsToTop", g_settings.topTargets);
}

static bool MatchesList(const std::wstring& typeName, const std::vector<std::wstring>& list) {
    for (auto& t : list) {
        if (t.empty()) continue;
        // case-insensitive substring
        auto it = std::search(
            typeName.begin(), typeName.end(), t.begin(), t.end(),
            [](wchar_t a, wchar_t b) { return towlower(a) == towlower(b); });
        if (it != typeName.end()) return true;
    }
    return false;
}

// ===================== Apply style to one element ======================

static void ApplyStyle(const winrt::Windows::Foundation::IInspectable& obj) {
    try {
        double r = g_settings.cornerRadius;
        xaml::CornerRadius cr{ r, r, r, r };
        xaml::Thickness zero{ 0, 0, 0, 0 };
        bool bg = g_settings.haveBg;

        if (auto border = obj.try_as<xc::Border>()) {
            border.CornerRadius(cr);
            if (g_settings.removeBorders) border.BorderThickness(zero);
            if (bg) border.Background(xm::SolidColorBrush(g_settings.bgColor));
        } else if (auto ctrl = obj.try_as<xc::Control>()) {
            // GridViewItem / ListViewItem / buttons, etc.
            ctrl.CornerRadius(cr);
            if (g_settings.removeBorders) ctrl.BorderThickness(zero);
            if (bg) ctrl.Background(xm::SolidColorBrush(g_settings.bgColor));
        } else if (auto cp = obj.try_as<xc::ContentPresenter>()) {
            // ListViewItemPresenter / GridViewItemPresenter derive from
            // ContentPresenter and draw the resting + hover + selection
            // backplates from ONE CornerRadius — matching it here is what stops
            // the hover highlight showing a second, differently-rounded layer.
            cp.CornerRadius(cr);
            if (g_settings.removeBorders) cp.BorderThickness(zero);
            if (bg) cp.Background(xm::SolidColorBrush(g_settings.bgColor));
        } else if (auto panel = obj.try_as<xc::Panel>()) {
            // Panels have no CornerRadius; only a tint applies.
            if (bg) panel.Background(xm::SolidColorBrush(g_settings.bgColor));
        }
    } catch (...) {
        // try_as / property set can throw during teardown; ignore.
    }
}

// Strip the background/backplate off an element so only its children show —
// used on the bottom virtual-desktops bar to leave just the clean desktops.
static void ClearBackground(const winrt::Windows::Foundation::IInspectable& obj) {
    try {
        xm::Brush nullBrush{ nullptr };  // null brush = transparent / no fill
        xaml::Thickness zero{ 0, 0, 0, 0 };
        if (auto border = obj.try_as<xc::Border>()) {
            border.Background(nullBrush);
            border.BorderThickness(zero);
        } else if (auto ctrl = obj.try_as<xc::Control>()) {
            ctrl.Background(nullBrush);
            ctrl.BorderThickness(zero);
        } else if (auto cp = obj.try_as<xc::ContentPresenter>()) {
            cp.Background(nullBrush);
            cp.BorderThickness(zero);
        } else if (auto panel = obj.try_as<xc::Panel>()) {
            panel.Background(nullBrush);
        }
    } catch (...) {
    }
}

// Collapse an element — removes a stray overlay/backplate that draws on top of
// the default one.
static void HideElement(const winrt::Windows::Foundation::IInspectable& obj) {
    try {
        if (auto ui = obj.try_as<xaml::UIElement>())
            ui.Visibility(xaml::Visibility::Collapsed);
    } catch (...) {
    }
}

// Pin an element to the top of its container (e.g. move the virtual-desktops
// bar from the bottom to the top). Sets VerticalAlignment=Top and, if it lives
// in a Grid, moves it to row 0.
static void MoveToTop(const winrt::Windows::Foundation::IInspectable& obj) {
    try {
        if (auto fe = obj.try_as<xaml::FrameworkElement>()) {
            fe.VerticalAlignment(xaml::VerticalAlignment::Top);
            xc::Grid::SetRow(fe, 0);
        }
    } catch (...) {
    }
}

// ===================== Visual-tree watcher (COM) =======================
// Implements the TAP contract: IObjectWithSite (so XAML diagnostics can hand us
// the IXamlDiagnostics site) + IVisualTreeServiceCallback2 (element add/remove).

class VisualTreeWatcher : public IObjectWithSite, public IVisualTreeServiceCallback2 {
public:
    VisualTreeWatcher() : m_ref(1) {}

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IObjectWithSite)) {
            *ppv = static_cast<IObjectWithSite*>(this);
        } else if (riid == __uuidof(IVisualTreeServiceCallback) ||
                   riid == __uuidof(IVisualTreeServiceCallback2)) {
            *ppv = static_cast<IVisualTreeServiceCallback2*>(this);
        } else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_ref); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG r = InterlockedDecrement(&m_ref);
        if (!r) delete this;
        return r;
    }

    // IObjectWithSite
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) override {
        m_diag = nullptr;
        m_vts = nullptr;
        m_site = nullptr;
        if (site) {
            m_site.copy_from(site);  // AddRef's; com_ptr has no raw-pointer assign
            HRESULT h1 = site->QueryInterface(__uuidof(IXamlDiagnostics), (void**)m_diag.put());
            HRESULT h2 = site->QueryInterface(__uuidof(IVisualTreeService3), (void**)m_vts.put());
            LogF(L">>> SetSite called. IXamlDiagnostics=0x%08X IVisualTreeService3=0x%08X",
                 (unsigned)h1, (unsigned)h2);
            HRESULT h3 = E_FAIL;
            if (m_vts)
                h3 = m_vts->AdviseVisualTreeChange(static_cast<IVisualTreeServiceCallback2*>(this));
            LogF(L">>> AdviseVisualTreeChange hr=0x%08X — diagnostics connected.", (unsigned)h3);
        } else {
            LogF(L">>> SetSite(nullptr) — diagnostics disconnected.");
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppv) override {
        if (m_site) return m_site->QueryInterface(riid, ppv);
        *ppv = nullptr;
        return E_FAIL;
    }

    // IVisualTreeServiceCallback / 2
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation,
                                                 VisualElement element,
                                                 VisualMutationType mutationType) override {
        if (mutationType != Add || !element.Type) return S_OK;

        auto& s = g_settings;
        // Fast path: nothing configured and not logging → ignore immediately.
        if (!s.logTypes && s.targets.empty() && s.clearTargets.empty() &&
            s.hideTargets.empty() && s.topTargets.empty())
            return S_OK;

        std::wstring type = element.Type;
        std::wstring name = element.Name ? element.Name : L"";
        if (s.logTypes)
            LogF(L"XAML + %s  name=%s", type.c_str(), name.c_str());

        // Match against both the type name and the element's x:Name.
        auto hit = [&](const std::vector<std::wstring>& list) {
            return MatchesList(type, list) || (!name.empty() && MatchesList(name, list));
        };
        bool doRound = hit(s.targets);
        bool doClear = hit(s.clearTargets);
        bool doHide  = hit(s.hideTargets);
        bool doTop   = hit(s.topTargets);

        if (m_diag && (doRound || doClear || doHide || doTop)) {
            IInspectable* raw = nullptr;
            if (SUCCEEDED(m_diag->GetIInspectableFromHandle(element.Handle, &raw)) && raw) {
                winrt::Windows::Foundation::IInspectable obj{ nullptr };
                winrt::attach_abi(obj, raw);  // take ownership of the +1 ref
                if (doRound) ApplyStyle(obj);
                if (doClear) ClearBackground(obj);
                if (doHide)  HideElement(obj);
                if (doTop)   MoveToTop(obj);
            }
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle, VisualElementState,
                                                    LPCWSTR) override {
        return S_OK;
    }

private:
    LONG m_ref;
    winrt::com_ptr<IXamlDiagnostics>     m_diag;
    winrt::com_ptr<IVisualTreeService3>  m_vts;
    winrt::com_ptr<IUnknown>             m_site;
};

// ===================== Class factory + DLL exports =====================
// InitializeXamlDiagnosticsEx loads this DLL into the target and CoCreates
// CLSID_WindhawkTAP, so we must expose it via DllGetClassObject.

class TAPFactory : public IClassFactory {
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IClassFactory)) {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return 2; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }

    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID riid, void** ppv) override {
        if (outer) return CLASS_E_NOAGGREGATION;
        auto* w = new (std::nothrow) VisualTreeWatcher();
        if (!w) return E_OUTOFMEMORY;
        HRESULT hr = w->QueryInterface(riid, ppv);
        w->Release();
        return hr;
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) override { return S_OK; }
};

static TAPFactory g_factory;
static DWORD      g_clsCookie = 0;

// MUST be exported: InitializeXamlDiagnosticsEx LoadLibrary's this DLL and calls
// GetProcAddress("DllGetClassObject") to build the TAP. Without the export the
// styling object is never created and SetSite never fires.
extern "C" __declspec(dllexport) HRESULT __stdcall
DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    if (rclsid == CLSID_WindhawkTAP) return g_factory.QueryInterface(riid, ppv);
    *ppv = nullptr;
    return CLASS_E_CLASSNOTAVAILABLE;
}
extern "C" __declspec(dllexport) HRESULT __stdcall DllCanUnloadNow() { return S_FALSE; }

// ===================== Inject the TAP into this process ================

using InitXamlDiagFn = HRESULT(WINAPI*)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, CLSID, LPCWSTR);

static bool InjectTAP() {
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&InjectTAP), &self);
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(self, path, MAX_PATH);

    LogF(L"TAP dll path = %s", path);

    // Register our class factory at runtime so the diagnostics host can create
    // the TAP via CoCreateInstance — relying on the mod DLL's DllGetClassObject
    // export alone is unreliable under Windhawk.
    if (!g_clsCookie) {
        HRESULT rr = CoRegisterClassObject(
            CLSID_WindhawkTAP, static_cast<IClassFactory*>(&g_factory),
            CLSCTX_INPROC_SERVER, REGCLS_MULTIPLEUSE, &g_clsCookie);
        LogF(L"CoRegisterClassObject hr=0x%08X cookie=%u", (unsigned)rr, (unsigned)g_clsCookie);
    }

    HMODULE xaml = LoadLibraryW(L"Windows.UI.Xaml.dll");
    if (!xaml) { LogF(L"Windows.UI.Xaml.dll not loaded"); return false; }
    auto pInit = (InitXamlDiagFn)GetProcAddress(xaml, "InitializeXamlDiagnosticsEx");
    if (!pInit) { LogF(L"InitializeXamlDiagnosticsEx not found"); return false; }

    // endpoint name, target pid, (xaml-diag dll = default), our TAP dll, our CLSID, init data
    HRESULT hr = pInit(L"VisualDiagConnection1", GetCurrentProcessId(),
                       nullptr, path, CLSID_WindhawkTAP, nullptr);
    LogF(L"InitializeXamlDiagnosticsEx hr=0x%08X", (unsigned)hr);
    return SUCCEEDED(hr);
}

// ============================ Mod lifecycle ============================

BOOL Wh_ModInit() {
    // Diagnostics log path: %TEMP%\taskview-styler.log (reset each load).
    wchar_t tmp[MAX_PATH] = {};
    GetTempPathW(MAX_PATH, tmp);
    g_logPath = std::wstring(tmp) + L"taskview-styler.log";
    DeleteFileW(g_logPath.c_str());

    LogF(L"=== Task View Clean Styler init (pid %u) ===", (unsigned)GetCurrentProcessId());

    // CoRegisterClassObject needs COM on this thread. Ignore failure (it may
    // already be initialized in another mode).
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    LoadSettings();
    LogF(L"settings: cornerRadius=%d removeBorders=%d logTypes=%d round=%zu clear=%zu",
         (int)g_settings.cornerRadius, (int)g_settings.removeBorders, (int)g_settings.logTypes,
         g_settings.targets.size(), g_settings.clearTargets.size());

    // XAML diagnostics callbacks arrive on Explorer's XAML UI thread, which is
    // already COM/WinRT-initialized, so we don't init an apartment here.
    if (!InjectTAP())
        LogF(L"TAP injection FAILED — try reloading the mod after opening Task View once.");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    // New rules apply to elements created after this point (re-open Task View).
}

void Wh_ModUninit() {
    Wh_Log(L"Task View Clean Styler uninit");
    // The TAP object is owned by the XAML diagnostics connection; clearing the
    // target list makes any late callbacks no-ops. A full disconnect happens
    // when Explorer tears down the diagnostics connection.
    if (g_clsCookie) { CoRevokeClassObject(g_clsCookie); g_clsCookie = 0; }
    g_settings.targets.clear();
    g_settings.clearTargets.clear();
    g_settings.hideTargets.clear();
    g_settings.topTargets.clear();
    g_settings.haveBg = false;
}
