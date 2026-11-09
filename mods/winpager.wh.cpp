// ==WindhawkMod==
// @id              winpager
// @name            WinPager
// @description     A KDE Plasma-style virtual desktop pager in the Windows 11 taskbar: live window outlines, click or scroll to switch, drag windows between desktops
// @version         0.1.1
// @author          seanmamasde
// @github          https://github.com/seanmamasde
// @homepage        https://github.com/seanmamasde/WinPager
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -lwindowscodecs -ldwmapi -lshcore -lruntimeobject -lole32 -luuid -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# WinPager

A virtual desktop pager like KDE Plasma's, living in the Windows 11 taskbar.

![WinPager in the taskbar, showing a desktop's windows](https://raw.githubusercontent.com/seanmamasde/WinPager/main/assets/screenshot1.png)

![The on-screen display when switching desktops](https://raw.githubusercontent.com/seanmamasde/WinPager/main/assets/screenshot2.png)

- Shows every virtual desktop with live window outlines, optionally with application icons
- Click a desktop to switch to it; click the current one to open Task View (configurable)
- Scroll over it to flip through desktops
- Drag a window outline onto another desktop to move that window there
- Hover a desktop for a list of its windows; right-click to add or remove desktops
- Ctrl+Shift+Win+Left/Right moves the active window to the neighbouring desktop and follows it
- KWin-style on-screen display when switching, and an option to hide Windows' own "Desktop N" popup
- A pager on each monitor's taskbar, or one on the main taskbar showing all monitors
- Can show only a few desktops at a time, scrolling along with the current one
- Follows light/dark mode and the accent color

The settings mirror those of Plasma's Pager widget, plus position, theme and the switching popups.

The mod runs in its own process; it isn't injected into explorer.exe.

## Notes

- Windows 11 24H2/25H2 (build 26100 and later) only. Like other virtual desktop tools, it relies on undocumented
  virtual desktop interfaces, so a Windows update can break it. On a build it doesn't know, it shows an error.
- The taskbar doesn't reserve space for it. Next to a centered row of buttons it shows only as many desktops as fit
  (at least the current one); on a left-aligned taskbar, a row of buttons long enough to reach the tray runs under it.
- WinPager is also a standalone app, [WinPager.exe](https://github.com/seanmamasde/WinPager), built from the same
  source. Only one of the two runs at a time: if you switch to the mod, quit WinPager.exe and turn off its
  "Start with Windows".
- Bugs and ideas: [github.com/seanmamasde/WinPager/issues](https://github.com/seanmamasde/WinPager/issues)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showWindowOutlines: true
  $name: Show window outlines
- showWindowIcons: false
  $name: Show application icons on window outlines
- showOnlyCurrentScreen: true
  $name: Show only current screen
  $description: Puts a pager on each monitor's taskbar. Off shows one pager on the main taskbar with every monitor.
- wrapPage: false
  $name: Navigation wraps around
- displayedText: none
  $name: Text display
  $options:
  - none: No text
  - number: Desktop number
  - name: Desktop name
- currentDesktopSelected: taskView
  $name: Selecting current virtual desktop
  $options:
  - nothing: Does nothing
  - showDesktop: Shows the desktop
  - taskView: Shows Task View
- position: left
  $name: Position
  $description: The left edge comes after the Widgets button if it's shown; on a left-aligned taskbar the pager goes beside the tray instead.
  $options:
  - left: Left edge of the taskbar
  - tray: Next to the system tray
- maxDesktops: 0
  $name: Desktops shown
  $description: Show at most this many desktops (0 = all); the shown ones scroll along to keep the current desktop in view.
- theme: windows
  $name: Theme
  $options:
  - windows: Follow Windows
  - light: Light
  - dark: Dark
- accentHighlight: true
  $name: Highlight with the Windows accent color
- switchOsd: true
  $name: Show on-screen display when switching
- hideSystemOsd: true
  $name: Hide the Windows "Desktop N" popup
- moveLeft: ctrl+shift+win+37
  $name: Move window one desktop to the left
  $description: Moves the active window to the neighbouring desktop and follows it there. Modifiers, then the key's virtual-key code (37 is the Left arrow); empty turns it off.
  #! $format: hotkey
- moveRight: ctrl+shift+win+39
  $name: Move window one desktop to the right
  $description: As above; 39 is the Right arrow.
  #! $format: hotkey
*/
// ==/WindhawkModSettings==

// Source: https://github.com/seanmamasde/WinPager. This mod is windhawk/winpager.wh.cpp there, with the files it
// #includes inlined by tools/make-windhawk-mod.ps1; WinPager.exe is built from the same files.

#include <windhawk_utils.h>

#include <cwchar>

// ---- src/vdapi.cpp ----
// ---- src/vdapi.h ----
#include <windows.h>
#include <wrl/client.h>
#include <string>
#include <vector>

struct IServiceProvider;
struct IVirtualDesktopManager;
struct IVirtualDesktopManagerInternal;
struct IApplicationViewCollection;
struct IVirtualDesktopNotificationService;
struct IVirtualDesktop;

namespace vd {

struct Desktop {
    GUID id{};
    std::wstring name;  // empty = default name ("Desktop N")
};

// wParam of the notify message
enum Change : WPARAM { List = 0, Current = 1, View = 2 };

// Undocumented Windows 11 24H2/25H2 (build 26100+) virtual desktop API hosted by explorer.
class Api {
public:
    Api();
    ~Api();
    HRESULT init(HWND notifyWnd, UINT notifyMsg);
    void reset();
    bool ready() const { return mgr_ != nullptr; }

    HRESULT desktops(std::vector<Desktop>& out, int& current);
    HRESULT switchTo(const GUID& id, bool animate = true);
    HRESULT create();
    HRESULT remove(const GUID& id, const GUID& fallback);
    HRESULT moveWindow(HWND hwnd, const GUID& to);
    HRESULT moveForegroundAndSwitch(const GUID& to);
    GUID windowDesktop(HWND hwnd) const;

private:
    Microsoft::WRL::ComPtr<IVirtualDesktop> find(const GUID& id);

    Microsoft::WRL::ComPtr<IVirtualDesktopManagerInternal> mgr_;
    Microsoft::WRL::ComPtr<IApplicationViewCollection> views_;
    Microsoft::WRL::ComPtr<IVirtualDesktopManager> pub_;
    Microsoft::WRL::ComPtr<IVirtualDesktopNotificationService> svc_;
    DWORD cookie_ = 0;
};

// explorer restarted or the API object died
inline bool isDisconnected(HRESULT hr) {
    return hr == RPC_E_DISCONNECTED || hr == HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE) ||
           hr == HRESULT_FROM_WIN32(RPC_S_CALL_FAILED) || hr == CO_E_OBJNOTCONNECTED;
}

}  // namespace vd

// ---- src/vdapi.cpp, continued ----
#include <inspectable.h>
#include <objectarray.h>
#include <servprov.h>
#include <shobjidl.h>
#include <winstring.h>

#include <atomic>

using Microsoft::WRL::ComPtr;

// Opaque: only ever passed back to explorer.
struct IApplicationView : IInspectable {};

// Interface layouts for build 26100/26200, cross-checked against MScholtes/VirtualDesktop
// (VirtualDesktop11-24H2.cs) and Ciantic/VirtualDesktopAccessor (interfaces.rs).
MIDL_INTERFACE("3F07F4BE-B107-441A-AF0F-39D82529072C")
IVirtualDesktop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IApplicationView*, BOOL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetId(GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetName(HSTRING*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWallpaperPath(HSTRING*) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsRemote(BOOL*) = 0;
};

MIDL_INTERFACE("1841C6D7-4F9D-42C0-AF41-8747538F10E5")
IApplicationViewCollection : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetViews(IObjectArray**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetViewsByZOrder(IObjectArray**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetViewsByAppUserModelId(PCWSTR, IObjectArray**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetViewForHwnd(HWND, IApplicationView**) = 0;
};

MIDL_INTERFACE("53F5CA0B-158F-4124-900C-057158060B27")
IVirtualDesktopManagerInternal : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveViewToDesktop(IApplicationView*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE CanViewMoveDesktops(IApplicationView*, BOOL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentDesktop(IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesktops(IObjectArray**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAdjacentDesktop(IVirtualDesktop*, UINT, IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktop(IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktopAndMoveForegroundView(IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDesktop(IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveDesktop(IVirtualDesktop*, UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveDesktop(IVirtualDesktop*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindDesktop(const GUID*, IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDesktopSwitchIncludeExcludeViews(IVirtualDesktop*, IObjectArray**, IObjectArray**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDesktopName(IVirtualDesktop*, HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDesktopWallpaper(IVirtualDesktop*, HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateWallpaperPathForAllDesktops(HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyDesktopState(IApplicationView*, IApplicationView*) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateRemoteDesktop(HSTRING, IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchRemoteDesktop(IVirtualDesktop*, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SwitchDesktopWithAnimation(IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLastActiveDesktop(IVirtualDesktop**) = 0;
    virtual HRESULT STDMETHODCALLTYPE WaitForAnimationToComplete() = 0;
};

MIDL_INTERFACE("B9E5E94D-233E-49AB-AF5C-2B4541C3AADE")
IVirtualDesktopNotification : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopCreated(IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyBegin(IVirtualDesktop*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyFailed(IVirtualDesktop*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopDestroyed(IVirtualDesktop*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopMoved(IVirtualDesktop*, INT64, INT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopNameChanged(IVirtualDesktop*, HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE ViewVirtualDesktopChanged(IApplicationView*) = 0;
    virtual HRESULT STDMETHODCALLTYPE CurrentVirtualDesktopChanged(IVirtualDesktop*, IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopWallpaperChanged(IVirtualDesktop*, HSTRING) = 0;
    virtual HRESULT STDMETHODCALLTYPE VirtualDesktopSwitched(IVirtualDesktop*) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoteVirtualDesktopConnected(IVirtualDesktop*) = 0;
};

MIDL_INTERFACE("0CD45E71-D927-4F15-8B0A-8FEF525337BF")
IVirtualDesktopNotificationService : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Register(IVirtualDesktopNotification*, DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD) = 0;
};

// mingw (the Windhawk mod's compiler) ignores MIDL_INTERFACE's ID in __uuidof; keep these in step with the IDs above
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IVirtualDesktop, 0x3F07F4BE, 0xB107, 0x441A, 0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C)
__CRT_UUID_DECL(IApplicationViewCollection, 0x1841C6D7, 0x4F9D, 0x42C0, 0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5)
__CRT_UUID_DECL(IVirtualDesktopManagerInternal, 0x53F5CA0B, 0x158F, 0x4124, 0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27)
__CRT_UUID_DECL(IVirtualDesktopNotification, 0xB9E5E94D, 0x233E, 0x49AB, 0xAF, 0x5C, 0x2B, 0x45, 0x41, 0xC3, 0xAA, 0xDE)
__CRT_UUID_DECL(IVirtualDesktopNotificationService, 0x0CD45E71, 0xD927, 0x4F15, 0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF)
#endif

namespace {

constexpr CLSID CLSID_ImmersiveShell = {0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};
constexpr GUID SID_VirtualDesktopManagerInternal = {0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};
constexpr GUID SID_VirtualDesktopNotificationService = {0xA501FDEC, 0x4A09, 0x464C, {0xAE, 0x4E, 0x1B, 0x9C, 0x21, 0xB8, 0x49, 0x18}};
constexpr CLSID CLSID_VirtualDesktopManager_ = {0xAA509086, 0x5CA9, 0x4C25, {0x8F, 0x95, 0x58, 0x9D, 0x3C, 0x07, 0xB4, 0x8A}};

// Callbacks arrive on our STA; only post, never call back into explorer from here.
class Sink final : public IVirtualDesktopNotification {
    std::atomic<ULONG> ref_{1};
    HWND wnd_;
    UINT msg_;
    HRESULT post(vd::Change c) {
        PostMessageW(wnd_, msg_, c, 0);
        return S_OK;
    }

public:
    Sink(HWND w, UINT m) : wnd_(w), msg_(m) {}
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IVirtualDesktopNotification)) {
            *ppv = static_cast<IVirtualDesktopNotification*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++ref_; }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = --ref_;
        if (!r) delete this;
        return r;
    }
    STDMETHODIMP VirtualDesktopCreated(IVirtualDesktop*) override { return post(vd::List); }
    STDMETHODIMP VirtualDesktopDestroyBegin(IVirtualDesktop*, IVirtualDesktop*) override { return S_OK; }
    STDMETHODIMP VirtualDesktopDestroyFailed(IVirtualDesktop*, IVirtualDesktop*) override { return post(vd::List); }
    STDMETHODIMP VirtualDesktopDestroyed(IVirtualDesktop*, IVirtualDesktop*) override { return post(vd::List); }
    STDMETHODIMP VirtualDesktopMoved(IVirtualDesktop*, INT64, INT64) override { return post(vd::List); }
    STDMETHODIMP VirtualDesktopNameChanged(IVirtualDesktop*, HSTRING) override { return post(vd::List); }
    STDMETHODIMP ViewVirtualDesktopChanged(IApplicationView*) override { return post(vd::View); }
    STDMETHODIMP CurrentVirtualDesktopChanged(IVirtualDesktop*, IVirtualDesktop*) override { return post(vd::Current); }
    STDMETHODIMP VirtualDesktopWallpaperChanged(IVirtualDesktop*, HSTRING) override { return S_OK; }
    STDMETHODIMP VirtualDesktopSwitched(IVirtualDesktop*) override { return post(vd::Current); }
    STDMETHODIMP RemoteVirtualDesktopConnected(IVirtualDesktop*) override { return post(vd::List); }
};

}  // namespace

namespace vd {

Api::Api() = default;
Api::~Api() { reset(); }

HRESULT Api::init(HWND notifyWnd, UINT notifyMsg) {
    reset();
    ComPtr<IServiceProvider> shell;
    HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&shell));
    if (SUCCEEDED(hr)) hr = shell->QueryService(SID_VirtualDesktopManagerInternal, IID_PPV_ARGS(&mgr_));
    if (SUCCEEDED(hr)) hr = shell->QueryService(__uuidof(IApplicationViewCollection), IID_PPV_ARGS(&views_));
    if (SUCCEEDED(hr)) hr = shell->QueryService(SID_VirtualDesktopNotificationService, IID_PPV_ARGS(&svc_));
    if (SUCCEEDED(hr)) hr = CoCreateInstance(CLSID_VirtualDesktopManager_, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pub_));
    if (SUCCEEDED(hr)) {
        ComPtr<Sink> sink;
        sink.Attach(new Sink(notifyWnd, notifyMsg));
        hr = svc_->Register(sink.Get(), &cookie_);
    }
    if (FAILED(hr)) reset();
    return hr;
}

void Api::reset() {
    if (svc_ && cookie_) svc_->Unregister(cookie_);
    cookie_ = 0;
    svc_.Reset();
    pub_.Reset();
    views_.Reset();
    mgr_.Reset();
}

ComPtr<IVirtualDesktop> Api::find(const GUID& id) {
    ComPtr<IVirtualDesktop> d;
    if (mgr_) mgr_->FindDesktop(&id, &d);
    return d;
}

HRESULT Api::desktops(std::vector<Desktop>& out, int& current) {
    if (!mgr_) return E_UNEXPECTED;
    ComPtr<IObjectArray> arr;
    ComPtr<IVirtualDesktop> cur;
    HRESULT hr = mgr_->GetDesktops(&arr);
    if (SUCCEEDED(hr)) hr = mgr_->GetCurrentDesktop(&cur);
    if (FAILED(hr)) return hr;
    GUID curId{};
    cur->GetId(&curId);
    UINT n = 0;
    arr->GetCount(&n);
    out.clear();
    current = 0;
    for (UINT i = 0; i < n; ++i) {
        ComPtr<IVirtualDesktop> d;
        if (FAILED(arr->GetAt(i, IID_PPV_ARGS(&d)))) continue;
        Desktop x;
        d->GetId(&x.id);
        HSTRING s = nullptr;
        if (SUCCEEDED(d->GetName(&s)) && s) {
            UINT32 len = 0;
            PCWSTR p = WindowsGetStringRawBuffer(s, &len);
            x.name.assign(p, len);
            WindowsDeleteString(s);
        }
        if (x.id == curId) current = static_cast<int>(out.size());
        out.push_back(std::move(x));
    }
    return S_OK;
}

HRESULT Api::switchTo(const GUID& id, bool animate) {
    auto d = find(id);
    if (!d) return E_INVALIDARG;
    if (animate) {
        const HRESULT hr = mgr_->SwitchDesktopWithAnimation(d.Get());
        if (SUCCEEDED(hr) || isDisconnected(hr)) return hr;
    }
    return mgr_->SwitchDesktop(d.Get());
}

HRESULT Api::create() {
    if (!mgr_) return E_UNEXPECTED;
    ComPtr<IVirtualDesktop> d;
    return mgr_->CreateDesktop(&d);
}

HRESULT Api::remove(const GUID& id, const GUID& fallback) {
    auto d = find(id), f = find(fallback);
    if (!d || !f) return E_INVALIDARG;
    return mgr_->RemoveDesktop(d.Get(), f.Get());
}

HRESULT Api::moveWindow(HWND hwnd, const GUID& to) {
    auto d = find(to);
    if (!d || !views_) return E_INVALIDARG;
    ComPtr<IApplicationView> view;
    HRESULT hr = views_->GetViewForHwnd(hwnd, &view);
    return SUCCEEDED(hr) ? mgr_->MoveViewToDesktop(view.Get(), d.Get()) : hr;
}

HRESULT Api::moveForegroundAndSwitch(const GUID& to) {
    auto d = find(to);
    return d ? mgr_->SwitchDesktopAndMoveForegroundView(d.Get()) : E_INVALIDARG;
}

GUID Api::windowDesktop(HWND hwnd) const {
    GUID g{};
    if (pub_) pub_->GetWindowDesktopId(hwnd, &g);
    return g;
}

}  // namespace vd
// ---- src/model.cpp ----
// ---- src/model.h ----

struct Win {
    HWND hwnd;
    RECT rect;  // visible frame bounds (physical px); restored rect when minimized
    int desktop;  // index into Model::desktops, -1 = shown on all desktops
    bool minimized;
    HMONITOR monitor;
};

struct Model {
    std::vector<vd::Desktop> desktops;
    int current = 0;
    std::vector<Win> windows;  // z-order, topmost first
    HWND active = nullptr;

    // Each returns true when something the pager draws changed.
    bool refreshDesktops(vd::Api& api, HRESULT& hr);
    bool refreshWindows(const vd::Api& api);
    bool refreshGeometry();

    int indexOf(const GUID& id) const;
    std::wstring desktopName(int i) const;
    static std::wstring title(HWND hwnd);
};

// ---- src/model.cpp, continued ----
#include <dwmapi.h>

namespace {

bool same(const RECT& a, const RECT& b) {
    return a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
}

bool same(const std::vector<Win>& a, const std::vector<Win>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i].hwnd != b[i].hwnd || !same(a[i].rect, b[i].rect) || a[i].desktop != b[i].desktop ||
            a[i].minimized != b[i].minimized)
            return false;
    return true;
}

RECT frameRect(HWND h, bool minimized) {
    RECT r{};
    if (minimized) {
        WINDOWPLACEMENT wp{sizeof wp};
        GetWindowPlacement(h, &wp);
        // shortcut: workspace coords, off by the taskbar size only when it sits at the top; good enough for screen matching.
        return wp.rcNormalPosition;
    }
    if (FAILED(DwmGetWindowAttribute(h, DWMWA_EXTENDED_FRAME_BOUNDS, &r, sizeof r))) GetWindowRect(h, &r);
    return r;
}

// Alt+Tab / taskbar membership rules.
bool isCandidate(HWND h, DWORD selfPid, HWND shell) {
    if (h == shell || !IsWindowVisible(h)) return false;
    const LONG ex = GetWindowLongW(h, GWL_EXSTYLE);
    if (!(ex & WS_EX_APPWINDOW)) {
        if (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
        if (GetWindow(h, GW_OWNER)) return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == selfPid) return false;
    wchar_t cls[32];
    return !(GetClassNameW(h, cls, 32) && wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0);
}

// a named CALLBACK, not a lambda: only MSVC converts lambdas to the 32-bit stdcall convention (the mod builds for x86)
BOOL CALLBACK collectWindow(HWND h, LPARAM p) {
    reinterpret_cast<std::vector<HWND>*>(p)->push_back(h);
    return TRUE;
}

}  // namespace

int Model::indexOf(const GUID& id) const {
    for (size_t i = 0; i < desktops.size(); ++i)
        if (desktops[i].id == id) return static_cast<int>(i);
    return -1;
}

std::wstring Model::desktopName(int i) const {
    if (i < 0 || i >= static_cast<int>(desktops.size())) return {};
    if (!desktops[i].name.empty()) return desktops[i].name;
    return L"Desktop " + std::to_wstring(i + 1);
}

std::wstring Model::title(HWND hwnd) {
    std::wstring s(static_cast<size_t>(GetWindowTextLengthW(hwnd)) + 1, L'\0');
    s.resize(static_cast<size_t>(GetWindowTextW(hwnd, s.data(), static_cast<int>(s.size()))));
    return s;
}

bool Model::refreshDesktops(vd::Api& api, HRESULT& hr) {
    std::vector<vd::Desktop> next;
    int cur = 0;
    hr = api.desktops(next, cur);
    if (FAILED(hr)) return false;
    bool changed = cur != current || next.size() != desktops.size();
    for (size_t i = 0; !changed && i < next.size(); ++i)
        changed = next[i].id != desktops[i].id || next[i].name != desktops[i].name;
    desktops.swap(next);
    current = cur;
    return changed;
}

bool Model::refreshWindows(const vd::Api& api) {
    std::vector<HWND> all;
    all.reserve(512);
    EnumWindows(collectWindow, reinterpret_cast<LPARAM>(&all));

    const DWORD self = GetCurrentProcessId();
    const HWND shell = GetShellWindow();
    std::vector<Win> next;
    next.reserve(64);
    for (HWND h : all) {
        if (!isCandidate(h, self, shell)) continue;
        const bool minimized = IsIconic(h);
        const RECT r = frameRect(h, minimized);
        if (r.right <= r.left || r.bottom <= r.top) continue;
        DWORD cloaked = 0;
        DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked, sizeof cloaked);
        const int d = indexOf(api.windowDesktop(h));
        // Windows on other desktops are shell-cloaked; anything else cloaked is not really shown.
        if (cloaked && (d < 0 || d == current || !(cloaked & DWM_CLOAKED_SHELL))) continue;
        next.push_back({h, r, d, minimized, MonitorFromRect(&r, MONITOR_DEFAULTTONEAREST)});
    }

    const HWND fg = GetForegroundWindow();
    const HWND act = fg ? GetAncestor(fg, GA_ROOTOWNER) : nullptr;
    const bool changed = act != active || !same(next, windows);
    windows.swap(next);
    active = act;
    return changed;
}

bool Model::refreshGeometry() {
    bool changed = false;
    for (auto& w : windows) {
        const bool minimized = IsIconic(w.hwnd);
        const RECT r = frameRect(w.hwnd, minimized);
        if (minimized != w.minimized || !same(r, w.rect)) {
            changed = true;
            w.rect = r;
            w.minimized = minimized;
            w.monitor = MonitorFromRect(&r, MONITOR_DEFAULTTONEAREST);
        }
    }
    return changed;
}
// ---- src/theme.cpp ----
// ---- src/settings.h ----
#include <windows.h>
#include <d2d1.h>

struct Hotkey {
    UINT mods;  // MOD_* flags
    UINT vk;    // 0 = none
    bool operator==(const Hotkey&) const = default;
};

// Option names and enums follow KDE's pager main.xml; the last block is Windows-specific.
struct Settings {
    bool showWindowOutlines = true;
    bool showWindowIcons = false;
    bool showOnlyCurrentScreen = true;  // also: one pager per monitor taskbar instead of one on the main taskbar
    bool wrapPage = false;
    int displayedText = 2;           // 0 number, 1 name, 2 none
    int currentDesktopSelected = 2;  // 0 nothing, 1 show desktop, 2 Task View (Win+Tab)

    int position = 0;     // 0 left edge, 1 next to the system tray
    int maxDesktops = 0;  // desktops shown at most, 0 = all; the shown ones follow the current desktop
    int theme = 0;     // 0 follow Windows, 1 light, 2 dark
    bool accentHighlight = true;
    bool switchOsd = true;      // KWin's desktop change OSD in the middle of the screen
    bool hideSystemOsd = true;  // Windows' own "Desktop N" popup above the taskbar
    Hotkey moveLeft{MOD_CONTROL | MOD_SHIFT | MOD_WIN, VK_LEFT};
    Hotkey moveRight{MOD_CONTROL | MOD_SHIFT | MOD_WIN, VK_RIGHT};

    void load();
    bool operator==(const Settings&) const = default;
};

struct Palette {
    bool dark;
    D2D1_COLOR_F text;       // KDE Theme.textColor
    D2D1_COLOR_F highlight;  // KDE ButtonFocus (used by the hover/active desktop frames)
};

DWORD readDword(const wchar_t* key, const wchar_t* name, DWORD def);  // HKCU value, def when missing
bool systemDark();  // taskbar ("Windows mode")
COLORREF accentColor(int shade);  // 0 = Light3 ... 3 = accent ... 6 = Dark3
Palette pagerPalette(const Settings& s);


// ---- src/theme.cpp, continued ----
namespace {

D2D1_COLOR_F toColor(COLORREF c) {
    return D2D1::ColorF(GetRValue(c) / 255.f, GetGValue(c) / 255.f, GetBValue(c) / 255.f);
}

}  // namespace

DWORD readDword(const wchar_t* key, const wchar_t* name, DWORD def) {
    DWORD v = 0, cb = sizeof v;
    return RegGetValueW(HKEY_CURRENT_USER, key, name, RRF_RT_REG_DWORD, nullptr, &v, &cb) == ERROR_SUCCESS ? v : def;
}

bool systemDark() {
    return readDword(L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", 1) == 0;
}

COLORREF accentColor(int shade) {
    BYTE p[32];
    DWORD cb = sizeof p;
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", L"AccentPalette",
                     RRF_RT_REG_BINARY, nullptr, p, &cb) == ERROR_SUCCESS && cb >= 28)
        return RGB(p[shade * 4], p[shade * 4 + 1], p[shade * 4 + 2]);
    return readDword(L"Software\\Microsoft\\Windows\\DWM", L"AccentColor", 0xFFD47800) & 0xFFFFFF;  // ABGR
}

Palette pagerPalette(const Settings& s) {
    Palette p;
    p.dark = s.theme == 0 ? systemDark() : s.theme == 2;
    p.text = p.dark ? D2D1::ColorF(1, 1, 1) : D2D1::ColorF(0, 0, 0);
    // WinUI AccentFillColorDefault: AccentDark1 on light, AccentLight2 on dark
    p.highlight = s.accentHighlight ? toColor(accentColor(p.dark ? 1 : 4)) : p.text;
    return p;
}
// ---- src/pager.cpp ----
// ---- src/pager.h ----
// ---- src/logic.h ----
// Pure helpers (no Win32 calls) shared by the app and tests/selftest.cpp.
#include <windows.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cwchar>
#include <string>
#include <utility>
#include <vector>

// Fluent "Fast Out, Slow In" easing, cubic-bezier(0, 0, 0, 1). With both x control points at 0, x(u) = u^3, so the
// curve has a closed form: y(t) = 3 t^(2/3) - 2 t.
inline float fluentDecelerate(float t) {
    t = std::clamp(t, 0.f, 1.f);
    return 3 * std::cbrt(t * t) - 2 * t;
}

// KDE wheel / shortcut navigation: returns current when there is nowhere to go.
inline int stepDesktop(int current, int count, int delta, bool wrap) {
    if (count <= 0) return current;
    const int t = current + delta;
    if (wrap) return ((t % count) + count) % count;
    return t < 0 || t >= count ? current : t;
}

// First desktop of a `shown`-wide window onto `count` desktops: scrolls from `first` only as far as needed to keep
// `current` in view.
inline int visibleFirst(int first, int current, int shown, int count) {
    first = std::clamp(first, 0, std::max(0, count - shown));
    if (shown < 1 || current < 0 || current >= count) return first;
    return std::clamp(first, current - shown + 1, current);
}

// Desktop rects `item` px wide, `gap` px apart, that fit in `room` px; at least one (the current desktop).
inline int fittingDesktops(int room, int item, int gap) {
    return std::max(1, (room + gap) / std::max(1, item + gap));
}

// Windhawk's hotkey setting, "[ctrl+][alt+][shift+][win+]<virtual-key code>" ("ctrl+shift+win+37"), as {MOD_* flags, vk};
// vk 0 = none.
inline std::pair<UINT, UINT> parseHotkey(const wchar_t* s) {
    UINT mods = 0;
    if (wcsstr(s, L"ctrl+")) mods |= MOD_CONTROL;
    if (wcsstr(s, L"alt+")) mods |= MOD_ALT;
    if (wcsstr(s, L"shift+")) mods |= MOD_SHIFT;
    if (wcsstr(s, L"win+")) mods |= MOD_WIN;
    const wchar_t* key = wcsrchr(s, L'+');
    return {mods, static_cast<UINT>(_wtoi(key ? key + 1 : s))};
}

struct FRect {
    float l, t, r, b;
};

// KDE windowmodel/main.qml: geometry relative to the shown area, scaled, rounded, clamped to the item.
inline FRect mapWindowRect(const RECT& win, const RECT& area, const FRect& item) {
    const float sx = (item.r - item.l) / float(area.right - area.left);
    const float sy = (item.b - item.t) / float(area.bottom - area.top);
    const LONG l = std::max(win.left, area.left), t = std::max(win.top, area.top);
    const LONG r = std::min(win.right, area.right), b = std::min(win.bottom, area.bottom);
    const float x = item.l + std::round((l - area.left) * sx);
    const float y = item.t + std::round((t - area.top) * sy);
    return {x, y, x + std::round((r - l) * sx), y + std::round((b - t) * sy)};
}

struct TipLine {
    std::wstring text;
    bool bullet = false;
};

// KDE pager tooltip sub text: a single title, or "N Windows:" + up to 4 bullets
// (5 when exactly 5, so "…and 1 other" never replaces a line), then minimized windows.
inline std::vector<TipLine> tooltipLines(const std::vector<std::wstring>& visible, const std::vector<std::wstring>& minimized) {
    std::vector<TipLine> out;
    auto list = [&](const std::vector<std::wstring>& w) {
        const size_t max = w.size() == 5 ? 5 : 4;
        for (size_t i = 0; i < w.size() && i < max; ++i) out.push_back({w[i], true});
        if (w.size() > max) {
            const size_t rest = w.size() - max;
            out.push_back({L"\u2026and " + std::to_wstring(rest) + (rest == 1 ? L" other window" : L" other windows")});
        }
    };
    if (visible.size() == 1) {
        out.push_back({visible[0]});
    } else if (visible.size() > 1) {
        out.push_back({std::to_wstring(visible.size()) + L" Windows:"});
        list(visible);
    }
    if (!visible.empty() && !minimized.empty()) out.push_back({});
    if (!minimized.empty()) {
        out.push_back({std::to_wstring(minimized.size()) + (minimized.size() == 1 ? L" Minimized Window:" : L" Minimized Windows:")});
        list(minimized);
    }
    return out;
}

// ---- src/pager.h, continued ----
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>

class App;
struct Win;

// 32bpp premultiplied DIB selected into a memory DC.
struct Surface {
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    int w = 0, h = 0;
    Surface() = default;
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
    ~Surface() { release(); }
    bool ensure(int cw, int ch);
    void release();
};

class Tooltip {
public:
    explicit Tooltip(App& app) : app_(app) {}
    ~Tooltip();
    void show(const RECT& anchor, const RECT& bar, int desktop, HMONITOR filter, float scale);
    void hide();
    bool visible() const { return hwnd_ && IsWindowVisible(hwnd_); }
    int desktop() const { return desktop_; }
    void refresh() { if (visible()) render(); }
    void restyle();

private:
    static LRESULT CALLBACK proc(HWND, UINT, WPARAM, LPARAM);
    void render();

    App& app_;
    HWND hwnd_ = nullptr;
    int desktop_ = -1;
    HMONITOR filter_ = nullptr;
    RECT anchor_{}, bar_{};
    float scale_ = 1;
    Surface surf_;
};

// One pager, embedded as a child of a taskbar window (Shell_TrayWnd / Shell_SecondaryTrayWnd).
class Pager {
public:
    Pager(App& app, HWND taskbar);
    ~Pager();
    HWND taskbar() const { return taskbar_; }
    bool alive() const { return hwnd_ && IsWindow(hwnd_) && IsWindow(taskbar_); }
    void update(bool relayout, bool redraw);
    void restyle();
    void raise() {
        if (alive() && GetWindow(taskbar_, GW_CHILD) != hwnd_)
            SetWindowPos(hwnd_, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

private:
    static LRESULT CALLBACK proc(HWND, UINT, WPARAM, LPARAM);
    LRESULT handle(UINT msg, WPARAM wp, LPARAM lp);
    void layout();
    void render();
    void drawWindow(const Win& w, const FRect& r, bool currentDesktop);
    FRect itemRect(int i) const;
    bool shows(const Win& w, int desktop) const;
    int desktopAt(POINT p) const;
    const Win* windowAt(int desktop, POINT p) const;
    void setHover(int i);
    void showTip(int i);
    void endDrag(bool drop);
    void dragHover(const POINTL* screen);  // OLE drag over the pager (nullptr = left)
    friend class DropTarget;

    App& app_;
    HWND hwnd_ = nullptr;
    HWND taskbar_;
    bool primary_ = false;  // Shell_TrayWnd (hosts the Widgets button) vs Shell_SecondaryTrayWnd
    HMONITOR monitor_ = nullptr;
    RECT area_{};  // screen area each desktop rect represents
    float scale_ = 1;
    int x_ = 0, y_ = 0, w_ = 0, h_ = 0, itemW_ = 0, itemH_ = 0, gap_ = 1, count_ = 0;
    int first_ = 0, shown_ = 1;  // desktops drawn: shown_ of the count_, from first_ on
    int hover_ = -1;
    int wheel_ = 0;
    bool tracking_ = false;
    bool tipSuppressed_ = false;
    struct Drag {
        bool pressed = false, active = false;
        int desktop = -1, target = -1;
        HWND hwnd = nullptr;
        POINT start{};
        FRect origin{}, rect{};
    } drag_;
    int dropDesktop_ = -1;
    Surface surf_;
    Tooltip tip_;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> label_;
    Microsoft::WRL::ComPtr<IDropTarget> drop_;
};

// KWin's desktop change OSD (kwin src/plugins/desktopchangeosd/osd.qml) without the direction arrows: the new desktop's
// name over a row of desktops whose active frame cross-fades from the previous one; hides 1 s after the last switch.
// The surface is a Windows 11 one: Mica, DWM corners, and Fluent entrance/exit motion.
class Osd {
public:
    explicit Osd(App& app) : app_(app) {}
    ~Osd();
    void show(int from, int to);
    void restyle();

private:
    static LRESULT CALLBACK proc(HWND, UINT, WPARAM, LPARAM);
    void layout();
    void render();
    void tick();
    void place();  // window opacity and position for the current animation state

    App& app_;
    HWND hwnd_ = nullptr;
    HMONITOR monitor_ = nullptr;
    std::vector<float> level_;  // active frame opacity per desktop
    int current_ = -1;
    enum class Phase { Hidden, In, Hold, Out } phase_ = Phase::Hidden;
    ULONGLONG last_ = 0, hideAt_ = 0, phaseStart_ = 0;
    float alpha_ = 0, rise_ = 0, alphaFrom_ = 0, riseFrom_ = 0;  // rise: px below the resting position
    BYTE placedAlpha_ = 0;
    RECT placed_{};
    float scale_ = 0;
    int x_ = 0, y_ = 0, w_ = 0, h_ = 0, cellW_ = 0, cellH_ = 0, gap_ = 0, pad_ = 0, head_ = 0;
    Surface surf_;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> heading_;
};

// ---- src/app.h ----

#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>

#include <memory>
#include <unordered_map>

class Pager;
class Osd;

enum : UINT {
    WM_APP_VD = WM_APP + 1,  // vd::Change in wParam
    WM_APP_SETTINGS,         // sent by a second instance
    WM_APP_TASKBARS,         // a pager's taskbar went away
    WM_APP_ICON,             // a window's icon, looked up off this thread (app.cpp fetchIcon)
};

inline constexpr wchar_t kControllerClass[] = L"WinPager.Controller";

class App {
public:
    App();
    ~App();
    int run(HINSTANCE inst);

    HINSTANCE inst = nullptr;
    Model model;
    Settings settings;
    Palette palette{};
    vd::Api api;
    Microsoft::WRL::ComPtr<ID2D1Factory> d2d;
    Microsoft::WRL::ComPtr<IDWriteFactory> dwrite;
    Microsoft::WRL::ComPtr<IWICImagingFactory> wic;
    Microsoft::WRL::ComPtr<ID2D1DCRenderTarget> rt;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
    Microsoft::WRL::ComPtr<ID2D1Layer> layer;
    bool hotkeyOk[2] = {true, true};

    int currentDesktop() const { return pending_ >= 0 ? pending_ : model.current; }
    void pagerDied() { PostMessageW(hwnd_, WM_APP_TASKBARS, 0, 0); }
    void switchTo(int desktop, bool activate = true);
    void activateCurrent();
    void moveWindow(HWND hwnd, int desktop);
    void contextMenu(POINT screen);
    ID2D1Bitmap* icon(HWND hwnd);

private:
    static LRESULT CALLBACK proc(HWND, UINT, WPARAM, LPARAM);
    static void CALLBACK onWinEvent(HWINEVENTHOOK, DWORD ev, HWND h, LONG obj, LONG child, DWORD, DWORD);
    LRESULT handle(UINT msg, WPARAM wp, LPARAM lp);
    void schedule(unsigned dirty);
    void refresh();
    void initApi();
    bool createTarget();
    void applyTheme();
    void rebuildPagers();
    void registerHotkeys();
    void moveForeground(int dir);
    void openSettings();
    void hideSystemOsd();
    void showOsd(int from, int to);
    void flushOsd();

    HWND hwnd_ = nullptr;
    UINT msgTaskbarCreated_ = 0;
    unsigned dirty_ = 0;
    bool timerPending_ = false;
    int pending_ = -1;     // desktop we asked explorer to switch to
    int activateOn_ = -1;  // focus that desktop's top window once the switch lands
    std::vector<HWINEVENTHOOK> hooks_;
    HWINEVENTHOOK reorderHook_ = nullptr;
    DWORD reorderPid_ = 0;
    std::vector<std::unique_ptr<Pager>> pagers_;
    std::unique_ptr<Osd> osd_;
    int osdFrom_ = -1, osdTo_ = -1;  // OSD held back while explorer's slide animation covers the screen
    HWND slide_ = nullptr;           // that animation surface (VirtualDesktopHotkeySwitcher), while it plays
    HWND systemOsd_ = nullptr;       // explorer's shared OSD host (volume, brightness, "Desktop N")
    ULONGLONG switchTick_ = 0;
    std::unordered_map<HWND, Microsoft::WRL::ComPtr<ID2D1Bitmap>> icons_;
};

// ---- src/pager.cpp, continued ----
#include <dwmapi.h>
#include <ole2.h>
#include <shellscalingapi.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>

using Microsoft::WRL::ComPtr;

namespace {

constexpr wchar_t kPagerClass[] = L"WinPager";
constexpr wchar_t kTipClass[] = L"WinPager.Tooltip";
constexpr wchar_t kOsdClass[] = L"WinPager.OSD";
constexpr float kRadius = 4;  // taskbar item corner radius (Taskbar.View AEP/button style), logical px
constexpr float kGap = 2;     // space between desktop rects, logical px
enum : UINT_PTR { kTimerTip = 1, kTimerDrop, kTimerOsd };
constexpr ULONGLONG kOsdHold = 1000;  // KWin PopupHideDelay default
constexpr float kOsdCrossfade = 500;  // osd.qml: PopupHideDelay / 2
// Windows motion tokens ("Motion in Windows" on learn.microsoft.com): Direct Entrance / Direct Exit move along
// cubic-bezier(0,0,0,1) in 333 / 167 ms, always paired with the 83 ms linear fade.
constexpr float kOsdEnter = 333, kOsdExit = 167, kOsdFade = 83, kOsdRise = 24;

D2D1_COLOR_F alpha(D2D1_COLOR_F c, float a) {
    c.a = a;
    return c;
}

D2D1_ROUNDED_RECT rounded(const FRect& r, float inset, float radius) {
    const float rad = std::max(0.f, radius - inset);
    return {{r.l + inset, r.t + inset, r.r - inset, r.b - inset}, rad, rad};
}

enum class Frame { Normal, Hover, Current };

// Desktop frame states of Breeze's widgets/pager.svg. Normal stays KDE's text 10 %; hover and current keep KDE's
// "light fill, stronger edge" shape, but current is a tint with a crisp accent edge instead of a 60 % accent block,
// which outweighed everything else on the Windows taskbar.
void drawFrame(ID2D1RenderTarget* rt, ID2D1SolidColorBrush* br, const Palette& p, const FRect& r, float rad, Frame f, float opacity = 1) {
    if (f == Frame::Normal) {
        br->SetColor(alpha(p.text, .1f * opacity));
        rt->FillRoundedRectangle(rounded(r, 0, rad), br);
        return;
    }
    const bool hover = f == Frame::Hover;
    br->SetColor(alpha(p.highlight, (hover ? .1f : .22f) * opacity));
    rt->FillRoundedRectangle(rounded(r, 1, rad), br);
    br->SetColor(alpha(p.highlight, (hover ? .5f : 1.f) * opacity));
    rt->DrawRoundedRectangle(rounded(r, .5f, rad), br, 1.f);
}

void registerClass(HINSTANCE inst, const wchar_t* name, WNDPROC proc) {
    WNDCLASSEXW wc{sizeof wc};
    wc.lpfnWndProc = proc;
    wc.hInstance = inst;
    wc.lpszClassName = name;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassExW(&wc);  // fails harmlessly once registered
}

ComPtr<IDWriteTextFormat> textFormat(IDWriteFactory* f, const wchar_t* family, DWRITE_FONT_WEIGHT weight, float px, bool center) {
    ComPtr<IDWriteTextFormat> tf;
    if (FAILED(f->CreateTextFormat(family, nullptr, weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, px, L"", &tf)))
        return nullptr;
    tf->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    ComPtr<IDWriteInlineObject> ellipsis;
    f->CreateEllipsisTrimmingSign(tf.Get(), &ellipsis);
    const DWRITE_TRIMMING trim{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    tf->SetTrimming(&trim, ellipsis.Get());
    if (center) {
        tf->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        tf->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    return tf;
}

template <class T>
T* self(HWND h, UINT msg, LPARAM lp) {
    if (msg == WM_NCCREATE)
        SetWindowLongPtrW(h, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams));
    return reinterpret_cast<T*>(GetWindowLongPtrW(h, GWLP_USERDATA));
}

}  // namespace

bool Surface::ensure(int cw, int ch) {
    if (dc && cw == w && ch == h) return true;
    release();
    BITMAPINFO bi{};
    bi.bmiHeader = {sizeof(BITMAPINFOHEADER), cw, -ch, 1, 32, BI_RGB};
    void* bits = nullptr;
    dc = CreateCompatibleDC(nullptr);
    bmp = dc ? CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
    if (!bmp) {
        release();
        return false;
    }
    old = SelectObject(dc, bmp);
    w = cw;
    h = ch;
    return true;
}

void Surface::release() {
    if (dc) {
        if (old) SelectObject(dc, old);
        DeleteDC(dc);
    }
    if (bmp) DeleteObject(bmp);
    dc = nullptr;
    bmp = nullptr;
    old = nullptr;
    w = h = 0;
}

// ---------------------------------------------------------------- Tooltip

Tooltip::~Tooltip() {
    if (hwnd_) DestroyWindow(hwnd_);
}

LRESULT CALLBACK Tooltip::proc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    auto* t = self<Tooltip>(h, msg, lp);
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        if (t && t->surf_.dc) BitBlt(dc, 0, 0, t->surf_.w, t->surf_.h, t->surf_.dc, 0, 0, SRCCOPY);
        EndPaint(h, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_NCACTIVATE:  // system backdrops go flat on inactive windows, and this one never activates
        return DefWindowProcW(h, msg, TRUE, lp);
    case WM_NCHITTEST:
        return HTTRANSPARENT;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

void Tooltip::restyle() {
    if (!hwnd_) return;
    const BOOL dark = app_.palette.dark;
    DwmSetWindowAttribute(hwnd_, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof dark);
}

void Tooltip::hide() {
    if (hwnd_) ShowWindow(hwnd_, SW_HIDE);
    desktop_ = -1;
}

void Tooltip::show(const RECT& anchor, const RECT& bar, int desktop, HMONITOR filter, float scale) {
    if (!hwnd_) {
        registerClass(app_.inst, kTipClass, proc);
        hwnd_ = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT, kTipClass, L"",
                                WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, app_.inst, this);
        if (!hwnd_) return;
        // Same recipe as the shell's own flyouts: DWM-rounded, acrylic system backdrop, content drawn with alpha.
        const MARGINS glass{-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(hwnd_, &glass);
        const DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd_, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof corner);
        const DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_TRANSIENTWINDOW;
        DwmSetWindowAttribute(hwnd_, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof backdrop);
        DefWindowProcW(hwnd_, WM_NCACTIVATE, TRUE, 0);
        restyle();
    }
    anchor_ = anchor;
    bar_ = bar;
    desktop_ = desktop;
    filter_ = filter;
    scale_ = scale;
    render();
}

void Tooltip::render() {
    const Model& m = app_.model;
    if (desktop_ < 0 || desktop_ >= static_cast<int>(m.desktops.size())) return hide();

    std::vector<std::wstring> shown, minimized;
    for (const Win& w : m.windows)
        if ((w.desktop == desktop_ || w.desktop < 0) && (!filter_ || w.monitor == filter_))
            (w.minimized ? minimized : shown).push_back(Model::title(w.hwnd));
    const auto lines = tooltipLines(shown, minimized);

    const float s = scale_;
    IDWriteFactory* f = app_.dwrite.Get();
    const auto titleFmt = textFormat(f, L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_SEMI_BOLD, 14 * s, false);
    const auto bodyFmt = textFormat(f, L"Segoe UI Variable Small", DWRITE_FONT_WEIGHT_NORMAL, 12 * s, false);
    if (!titleFmt || !bodyFmt) return;
    const float padX = std::round(11 * s), padT = std::round(7 * s), padB = std::round(9 * s);
    const float maxW = std::round(400 * s), indent = std::round(6 * s);

    struct Line {
        ComPtr<IDWriteTextLayout> layout;
        float x, y;
        bool body;
    };
    std::vector<Line> laid;
    float y = padT, width = 0;
    auto add = [&](const std::wstring& text, IDWriteTextFormat* fmt, float x, bool body) {
        ComPtr<IDWriteTextLayout> l;
        const std::wstring t = text.empty() ? L" " : text;
        if (FAILED(f->CreateTextLayout(t.c_str(), static_cast<UINT32>(t.size()), fmt, maxW - (x - padX), 1e4f, &l))) return;
        DWRITE_TEXT_METRICS tm{};
        l->GetMetrics(&tm);
        laid.push_back({l, x, y, body});
        width = std::max(width, x - padX + tm.widthIncludingTrailingWhitespace);
        y += tm.height;
    };
    add(m.desktopName(desktop_), titleFmt.Get(), padX, false);
    if (!lines.empty()) y += std::round(3 * s);
    for (const auto& ln : lines) add(ln.bullet ? L"•  " + ln.text : ln.text, bodyFmt.Get(), ln.bullet ? padX + indent : padX, true);

    const int W = static_cast<int>(std::ceil(std::min(width, maxW) + 2 * padX));
    const int H = static_cast<int>(std::ceil(y + padB));
    if (!surf_.ensure(W, H)) return;

    ID2D1DCRenderTarget* rt = app_.rt.Get();
    ID2D1SolidColorBrush* br = app_.brush.Get();
    const RECT rc{0, 0, W, H};
    if (FAILED(rt->BindDC(surf_.dc, &rc))) return;
    const bool dark = app_.palette.dark;
    // WinUI TextFillColorPrimary / TextFillColorSecondary
    const auto primary = dark ? D2D1::ColorF(1, 1, 1, 1) : D2D1::ColorF(0, 0, 0, .894f);
    const auto secondary = dark ? D2D1::ColorF(1, 1, 1, .786f) : D2D1::ColorF(0, 0, 0, .62f);
    rt->BeginDraw();
    rt->Clear(D2D1::ColorF(0, 0, 0, 0));
    for (const auto& l : laid) {
        br->SetColor(l.body ? secondary : primary);
        rt->DrawTextLayout({l.x, l.y}, l.layout.Get(), br);
    }
    rt->EndDraw();

    MONITORINFO mi{sizeof mi};
    GetMonitorInfoW(MonitorFromRect(&anchor_, MONITOR_DEFAULTTONEAREST), &mi);
    const int gap = static_cast<int>(std::round(8 * s));
    const int x = std::clamp(static_cast<int>((anchor_.left + anchor_.right) / 2 - W / 2), static_cast<int>(mi.rcMonitor.left) + gap,
                             std::max(static_cast<int>(mi.rcMonitor.left) + gap, static_cast<int>(mi.rcMonitor.right) - W - gap));
    const bool barAtBottom = bar_.top > (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
    const int top = barAtBottom ? bar_.top - gap - H : bar_.bottom + gap;
    SetWindowPos(hwnd_, HWND_TOPMOST, x, top, W, H, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(hwnd_, nullptr, FALSE);
}

// ---------------------------------------------------------------- Osd

Osd::~Osd() {
    if (hwnd_) DestroyWindow(hwnd_);
}

LRESULT CALLBACK Osd::proc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    auto* o = self<Osd>(h, msg, lp);
    switch (msg) {
    case WM_TIMER:
        if (o) o->tick();
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        if (o && o->surf_.dc) BitBlt(dc, 0, 0, o->surf_.w, o->surf_.h, o->surf_.dc, 0, 0, SRCCOPY);
        EndPaint(h, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_NCACTIVATE:  // system backdrops go flat on inactive windows, and this one never activates
        return DefWindowProcW(h, msg, TRUE, lp);
    case WM_NCHITTEST:
        return HTTRANSPARENT;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

void Osd::show(int from, int to) {
    const int n = static_cast<int>(app_.model.desktops.size());
    if (to < 0 || to >= n) return;
    if (!hwnd_) {  // output only, like KWin's: click-through and never activated
        registerClass(app_.inst, kOsdClass, proc);
        // Layered only for the whole-window fade: DWM still draws the system backdrop and fades it along.
        hwnd_ = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kOsdClass,
                                L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, app_.inst, this);
        if (!hwnd_) return;
        SetLayeredWindowAttributes(hwnd_, 0, 0, LWA_ALPHA);
        const MARGINS glass{-1, -1, -1, -1};  // content is drawn with alpha over the backdrop
        DwmExtendFrameIntoClientArea(hwnd_, &glass);
        const DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
        DwmSetWindowAttribute(hwnd_, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof corner);
        const DWM_SYSTEMBACKDROP_TYPE mica = DWMSBT_MAINWINDOW;
        DwmSetWindowAttribute(hwnd_, DWMWA_SYSTEMBACKDROP_TYPE, &mica, sizeof mica);
        DefWindowProcW(hwnd_, WM_NCACTIVATE, TRUE, 0);
        restyle();
    }
    const bool visible = phase_ != Phase::Hidden;
    const ULONGLONG now = GetTickCount64();
    level_.resize(n, 0.f);
    if (!visible) {  // the previous desktop's frame fades out while the new one fades in
        std::ranges::fill(level_, 0.f);
        if (from >= 0 && from < n) level_[from] = 1;
        last_ = now;
        POINT pt;
        GetCursorPos(&pt);
        monitor_ = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);  // the active screen; kept while visible, as in KWin
    }
    current_ = to;
    hideAt_ = now + kOsdHold;
    layout();
    render();
    if (phase_ == Phase::Hidden || phase_ == Phase::Out) {  // (re)enter from wherever the exit got to
        alphaFrom_ = alpha_;
        riseFrom_ = visible ? rise_ : kOsdRise * scale_;
        rise_ = riseFrom_;
        phase_ = Phase::In;
        phaseStart_ = now;
    }
    place();
    if (!visible) {
        SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        UpdateWindow(hwnd_);
        SetTimer(hwnd_, kTimerOsd, USER_TIMER_MINIMUM, nullptr);
    }
}

void Osd::restyle() {
    if (!hwnd_) return;
    const BOOL dark = app_.palette.dark;  // dark or light Mica, like the taskbar
    DwmSetWindowAttribute(hwnd_, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof dark);
    if (phase_ != Phase::Hidden) render();
}

void Osd::place() {
    const BYTE a = static_cast<BYTE>(std::lround(alpha_ * 255));
    const int y = y_ + static_cast<int>(std::lround(rise_));
    const RECT r{x_, y, x_ + w_, y + h_};
    if (a != placedAlpha_) SetLayeredWindowAttributes(hwnd_, 0, a, LWA_ALPHA);
    if (!EqualRect(&r, &placed_)) SetWindowPos(hwnd_, nullptr, x_, y, w_, h_, SWP_NOZORDER | SWP_NOACTIVATE);
    placedAlpha_ = a;
    placed_ = r;
}

void Osd::layout() {
    MONITORINFO mi{sizeof mi};
    GetMonitorInfoW(monitor_, &mi);
    UINT dpi = 96, dpiY = 96;
    GetDpiForMonitor(monitor_, MDT_EFFECTIVE_DPI, &dpi, &dpiY);
    if (dpi / 96.f != scale_ || !heading_) {
        scale_ = dpi / 96.f;
        heading_ = textFormat(app_.dwrite.Get(), L"Segoe UI Variable Display", DWRITE_FONT_WEIGHT_NORMAL, 20 * scale_, true);
    }
    const RECT& m = mi.rcMonitor;
    const int sw = m.right - m.left, sh = m.bottom - m.top, n = std::max(1, static_cast<int>(level_.size()));
    // osd.qml: itemWidth = screenWidth * min(0.8 / columns, 0.1), itemHeight keeps the screen's aspect ratio
    cellW_ = static_cast<int>(sw * std::min(0.8 / n, 0.1));
    cellH_ = static_cast<int>(std::min(cellW_ * double(sh) / sw, sh * 0.1));
    const auto px = [this](float v) { return static_cast<int>(std::lround(v * scale_)); };
    gap_ = px(4);
    pad_ = px(12);
    head_ = px(36);  // 28 px heading line + 8 px space
    w_ = n * cellW_ + (n - 1) * gap_ + 2 * pad_;
    h_ = head_ + cellH_ + 2 * pad_;
    x_ = (m.left + m.right - w_) / 2;
    y_ = (m.top + m.bottom - h_) / 2;
}

void Osd::render() {
    if (!surf_.ensure(w_, h_)) return;
    const Palette& p = app_.palette;
    ID2D1DCRenderTarget* rt = app_.rt.Get();
    ID2D1SolidColorBrush* br = app_.brush.Get();
    const RECT rc{0, 0, w_, h_};
    if (FAILED(rt->BindDC(surf_.dc, &rc))) return;
    const FRect panel{0, 0, static_cast<float>(w_), static_cast<float>(h_)};
    rt->BeginDraw();
    rt->Clear(D2D1::ColorF(0, 0, 0, 0));  // DWM draws the Mica panel, its corners and edge behind this
    const float pad = static_cast<float>(pad_);
    if (heading_) {
        const std::wstring name = app_.model.desktopName(current_);
        br->SetColor(p.dark ? D2D1::ColorF(1, 1, 1) : D2D1::ColorF(0, 0, 0, .894f));
        rt->DrawText(name.c_str(), static_cast<UINT32>(name.size()), heading_.Get(),
                     {panel.l + pad, panel.t + pad, panel.r - pad, panel.t + pad + std::round(28 * scale_)}, br, D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
    const float cellRad = std::round(kRadius * scale_);
    for (size_t i = 0; i < level_.size(); ++i) {
        const float x = panel.l + pad + static_cast<float>(i * (cellW_ + gap_)), y = panel.t + pad + head_;
        const FRect cell{x, y, x + cellW_, y + cellH_};
        drawFrame(rt, br, p, cell, cellRad, Frame::Normal);
        if (level_[i] > 0) drawFrame(rt, br, p, cell, cellRad, Frame::Current, level_[i]);
    }
    rt->EndDraw();
    InvalidateRect(hwnd_, nullptr, FALSE);
}

void Osd::tick() {
    const ULONGLONG now = GetTickCount64();
    const float dt = static_cast<float>(now - last_);
    last_ = now;
    bool redraw = false;
    for (size_t i = 0; i < level_.size(); ++i) {  // osd.qml: Behavior on opacity, linear
        const float target = static_cast<int>(i) == current_ ? 1.f : 0.f, step = dt / kOsdCrossfade;
        const float v = level_[i] < target ? std::min(target, level_[i] + step) : std::max(target, level_[i] - step);
        redraw |= v != level_[i];
        level_[i] = v;
    }
    if ((phase_ == Phase::In || phase_ == Phase::Hold) && now >= hideAt_) {  // exit from wherever the entrance got to
        alphaFrom_ = alpha_;
        riseFrom_ = rise_;
        phase_ = Phase::Out;
        phaseStart_ = now;
    }
    const float t = static_cast<float>(now - phaseStart_);
    if (phase_ == Phase::In) {  // rise into place while fading in
        alpha_ = alphaFrom_ + (1 - alphaFrom_) * std::min(1.f, t / kOsdFade);
        rise_ = riseFrom_ * (1 - fluentDecelerate(t / kOsdEnter));
        if (t >= kOsdEnter) phase_ = Phase::Hold;
    } else if (phase_ == Phase::Out) {  // drop back down while fading out
        alpha_ = alphaFrom_ * (1 - std::min(1.f, t / kOsdFade));
        rise_ = riseFrom_ + (kOsdRise * scale_ - riseFrom_) * fluentDecelerate(t / kOsdExit);
        if (t >= kOsdExit) {  // idle again: no timer, no window
            KillTimer(hwnd_, kTimerOsd);
            ShowWindow(hwnd_, SW_HIDE);
            phase_ = Phase::Hidden;
            alpha_ = 0;
            return;
        }
    }
    if (redraw) render();
    place();
}

// ---------------------------------------------------------------- Pager

// KDE: holding a drag (file, text, ...) over a desktop for a second switches to it. Never accepts the drop.
class DropTarget final : public IDropTarget {
    std::atomic<ULONG> ref_{1};
    Pager& pager_;

public:
    explicit DropTarget(Pager& p) : pager_(p) {}
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDropTarget)) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++ref_; }
    STDMETHODIMP_(ULONG) Release() override {
        const ULONG r = --ref_;
        if (!r) delete this;
        return r;
    }
    STDMETHODIMP DragEnter(IDataObject*, DWORD, POINTL pt, DWORD* effect) override { return over(pt, effect); }
    STDMETHODIMP DragOver(DWORD, POINTL pt, DWORD* effect) override { return over(pt, effect); }
    STDMETHODIMP DragLeave() override {
        pager_.dragHover(nullptr);
        return S_OK;
    }
    STDMETHODIMP Drop(IDataObject*, DWORD, POINTL, DWORD* effect) override {
        *effect = DROPEFFECT_NONE;
        pager_.dragHover(nullptr);
        return S_OK;
    }

private:
    HRESULT over(POINTL pt, DWORD* effect) {
        *effect = DROPEFFECT_NONE;
        pager_.dragHover(&pt);
        return S_OK;
    }
};

Pager::Pager(App& app, HWND taskbar) : app_(app), taskbar_(taskbar), tip_(app) {
    wchar_t cls[32];
    primary_ = GetClassNameW(taskbar, cls, 32) && !wcscmp(cls, L"Shell_TrayWnd");
    registerClass(app.inst, kPagerClass, proc);
    // NOPARENTNOTIFY: no WM_PARENTNOTIFY sent into explorer, which could hang us, on create, destroy and clicks
    hwnd_ = CreateWindowExW(WS_EX_LAYERED | WS_EX_NOPARENTNOTIFY, kPagerClass, L"WinPager", WS_CHILD | WS_CLIPSIBLINGS, 0, 0,
                            1, 1, taskbar_, nullptr, app.inst, this);
    if (!hwnd_) return;  // taskbar in a shell band right now; App retries
    drop_.Attach(new DropTarget(*this));
    RegisterDragDrop(hwnd_, drop_.Get());
    update(true, true);
    ShowWindow(hwnd_, SW_SHOWNA);
}

Pager::~Pager() {
    if (hwnd_ && IsWindow(hwnd_)) DestroyWindow(hwnd_);
}

void Pager::dragHover(const POINTL* screen) {
    int d = -1;
    if (screen) {
        POINT p{screen->x, screen->y};
        ScreenToClient(hwnd_, &p);
        d = desktopAt(p);
    }
    if (d == dropDesktop_) return;
    dropDesktop_ = d;
    KillTimer(hwnd_, kTimerDrop);
    if (d >= 0) SetTimer(hwnd_, kTimerDrop, 1000, nullptr);
    tip_.hide();
    render();
}

LRESULT CALLBACK Pager::proc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    auto* p = self<Pager>(h, msg, lp);
    if (!p) return DefWindowProcW(h, msg, wp, lp);
    if (msg == WM_NCCREATE) p->hwnd_ = h;
    return p->handle(msg, wp, lp);
}

void Pager::layout() {
    const Settings& s = app_.settings;
    RECT tb{};
    GetClientRect(taskbar_, &tb);
    const float scale = GetDpiForWindow(taskbar_) / 96.f;
    if (scale != scale_ || !label_) {
        scale_ = scale;
        label_ = textFormat(app_.dwrite.Get(), L"Segoe UI Variable Small", DWRITE_FONT_WEIGHT_NORMAL, 12 * scale_, true);
    }
    monitor_ = MonitorFromWindow(taskbar_, MONITOR_DEFAULTTONEAREST);
    if (s.showOnlyCurrentScreen) {
        MONITORINFO mi{sizeof mi};
        GetMonitorInfoW(monitor_, &mi);
        area_ = mi.rcMonitor;
    } else {
        const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN), vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
        area_ = {vx, vy, vx + GetSystemMetrics(SM_CXVIRTUALSCREEN), vy + GetSystemMetrics(SM_CYVIRTUALSCREEN)};
    }
    count_ = std::max(1, static_cast<int>(app_.model.desktops.size()));

    const int H = tb.bottom - tb.top;
    const int margin = static_cast<int>(std::lround(4 * scale_));
    itemH_ = std::max(8, H - 2 * margin);
    const double ratio = double(area_.right - area_.left) / std::max(1L, area_.bottom - area_.top);
    itemW_ = std::max(8, static_cast<int>(itemH_ * ratio));
    gap_ = std::max(1, static_cast<int>(std::lround(kGap * scale_)));
    h_ = itemH_;
    y_ = (H - h_) / 2;
    auto advanced = [](const wchar_t* name, DWORD def) {
        DWORD v = def, cb = sizeof v;
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", name, RRF_RT_REG_DWORD,
                     nullptr, &v, &cb);
        return v;
    };
    // Make room for the Widgets entry point (an AEP button in explorer's taskbar XAML, primary taskbar only): it takes
    // the leading edge of a centered taskbar and sits beside the tray on a left-aligned one; margins 6,0,8,0, max 152 px.
    const bool centered = advanced(L"TaskbarAl", 1) != 0;  // missing = centered (Windows 11 default)
    const bool widgets = primary_ && advanced(L"TaskbarDa", 1) != 0;
    const int aep = widgets ? static_cast<int>(std::lround((6 + 152 + 8) * scale_)) : 0;
    const int pad = static_cast<int>(std::lround(6 * scale_));
    const bool leading = s.position == 0 && centered;
    LONG right = tb.right;  // where the tray starts
    if (HWND tray = FindWindowExW(taskbar_, nullptr, L"TrayNotifyWnd", nullptr)) {
        RECT r;
        GetWindowRect(tray, &r);
        MapWindowPoints(nullptr, taskbar_, reinterpret_cast<POINT*>(&r), 2);
        right = r.left;
    }
    shown_ = s.maxDesktops > 0 ? std::min(count_, s.maxDesktops) : count_;
    // The taskbar only makes room for its own items (the Widgets button's fixed slot), so beside a centered row of
    // buttons show just the desktops that fit. Explorer keeps its hidden legacy Start window over the XAML Start button,
    // and moving it fires the LOCATIONCHANGE that relayouts us.
    RECT start;
    if (HWND st = centered ? FindWindowExW(taskbar_, nullptr, L"Start", nullptr) : nullptr;
        st && GetWindowRect(st, &start) && start.right > start.left) {
        MapWindowPoints(nullptr, taskbar_, reinterpret_cast<POINT*>(&start), 2);
        // shortcut: the row's right end is mirrored from its left end, which is off once a crowded row gets pushed right
        // by the Widgets button; read the buttons through UI Automation if the tray-side position needs that case.
        const int room = leading ? start.left - 2 * pad - aep : right - (tb.right - start.left) - 2 * pad;
        shown_ = std::min(shown_, fittingDesktops(room, itemW_, gap_));
    }
    w_ = shown_ * itemW_ + (shown_ - 1) * gap_;
    x_ = leading ? pad + aep : right - pad - w_ - (centered ? 0 : aep);
    SetWindowPos(hwnd_, HWND_TOP, x_, y_, w_, h_, SWP_NOACTIVATE);
}

void Pager::restyle() {
    tip_.restyle();
    label_.Reset();
    update(true, true);
}

void Pager::update(bool relayout, bool redraw) {
    if (!alive()) return;
    if (relayout || count_ != std::max(1, static_cast<int>(app_.model.desktops.size()))) {
        layout();
        redraw = true;
    }
    if (const int first = visibleFirst(first_, app_.currentDesktop(), shown_, count_); first != first_) {
        first_ = first;  // scrolled to keep the current desktop in view
        tip_.hide();
        redraw = true;
    }
    if (!redraw) return;
    if (hover_ < first_ || hover_ >= first_ + shown_) hover_ = -1;
    if (drag_.pressed && drag_.hwnd &&
        std::ranges::none_of(app_.model.windows, [&](const Win& w) { return w.hwnd == drag_.hwnd; }))
        endDrag(false);
    render();
    tip_.refresh();
}

FRect Pager::itemRect(int i) const {
    const float x = static_cast<float>((i - first_) * (itemW_ + gap_));
    return {x, 0, x + itemW_, static_cast<float>(itemH_)};
}

bool Pager::shows(const Win& w, int desktop) const {
    return (w.desktop == desktop || w.desktop < 0) && (!app_.settings.showOnlyCurrentScreen || w.monitor == monitor_);
}

int Pager::desktopAt(POINT p) const {
    if (p.x < 0 || p.y < 0 || p.y >= h_) return -1;
    const int i = p.x / (itemW_ + gap_);
    return i < shown_ && p.x - i * (itemW_ + gap_) < itemW_ ? first_ + i : -1;
}

const Win* Pager::windowAt(int desktop, POINT p) const {
    const FRect item = itemRect(desktop);
    for (const Win& w : app_.model.windows) {  // topmost first
        if (w.minimized || !shows(w, desktop)) continue;
        const FRect r = mapWindowRect(w.rect, area_, item);
        if (p.x >= r.l && p.x < r.r && p.y >= r.t && p.y < r.b) return &w;
    }
    return nullptr;
}

void Pager::drawWindow(const Win& w, const FRect& r, bool currentDesktop) {
    if (r.r - r.l < 1 || r.b - r.t < 1) return;
    ID2D1DCRenderTarget* rt = app_.rt.Get();
    ID2D1SolidColorBrush* br = app_.brush.Get();
    const D2D1_COLOR_F text = app_.palette.text;
    const bool active = w.hwnd == app_.model.active;
    // KDE's fills; on a light taskbar black at 60 % turns a maximized window into the heaviest thing on the bar,
    // so fills are lightened there (outline borders stay KDE's).
    const float k = app_.palette.dark ? 1.f : .55f;
    br->SetColor(alpha(text, k * (currentDesktop ? (active ? .6f : .35f) : (active ? .5f : .17f))));
    rt->FillRectangle({r.l, r.t, r.r, r.b}, br);
    br->SetColor(alpha(text, active ? 1.f : .5f));
    rt->DrawRectangle({r.l + .5f, r.t + .5f, r.r - .5f, r.b - .5f}, br, 1.f);
    if (!app_.settings.showWindowIcons) return;
    const float avail = std::min(r.r - r.l, r.b - r.t), sp = 4 * scale_;
    const float size = std::floor(std::min({32 * scale_, avail, std::max(avail - 2 * sp, 2 * sp)}));
    if (size < 6) return;
    if (ID2D1Bitmap* icon = app_.icon(w.hwnd)) {
        const float x = std::round((r.l + r.r - size) / 2), y = std::round((r.t + r.b - size) / 2);
        rt->DrawBitmap(icon, {x, y, x + size, y + size}, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }
}

void Pager::render() {
    if (!alive() || w_ <= 0 || h_ <= 0 || !surf_.ensure(w_, h_)) return;
    const Model& m = app_.model;
    const Settings& s = app_.settings;
    const Palette& p = app_.palette;
    ID2D1DCRenderTarget* rt = app_.rt.Get();
    ID2D1SolidColorBrush* br = app_.brush.Get();
    const RECT rc{0, 0, w_, h_};
    if (FAILED(rt->BindDC(surf_.dc, &rc))) return;

    const float rad = std::round(kRadius * scale_);
    const int current = app_.currentDesktop();
    const Win* dragged = nullptr;
    rt->BeginDraw();
    // alpha 1/255 keeps gaps hit-testable so the tooltip doesn't flicker between rects
    rt->Clear(D2D1::ColorF(0, 0, 0, 1 / 255.f));
    for (int i = first_; i < first_ + shown_; ++i) {
        const FRect r = itemRect(i);
        const bool isCurrent = i == current;
        if (s.showWindowOutlines) {
            ComPtr<ID2D1RoundedRectangleGeometry> clip;
            app_.d2d->CreateRoundedRectangleGeometry(rounded(r, 1, rad), &clip);
            rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get()), app_.layer.Get());
            for (auto it = m.windows.rbegin(); it != m.windows.rend(); ++it) {  // bottom of the stack first
                if (it->minimized || !shows(*it, i)) continue;
                if (drag_.active && it->hwnd == drag_.hwnd) {
                    dragged = &*it;
                    continue;
                }
                drawWindow(*it, mapWindowRect(it->rect, area_, r), isCurrent);
            }
            rt->PopLayer();
        }
        // KDE state priority: hover > active > normal.
        const bool hover = drag_.active ? drag_.target == i : dropDesktop_ >= 0 ? dropDesktop_ == i : hover_ == i;
        drawFrame(rt, br, p, r, rad, hover ? Frame::Hover : isCurrent ? Frame::Current : Frame::Normal);
        if (s.displayedText != 2 && label_) {
            const std::wstring t = s.displayedText == 0 ? std::to_wstring(i + 1) : m.desktopName(i);
            br->SetColor(p.text);
            rt->DrawText(t.c_str(), static_cast<UINT32>(t.size()), label_.Get(), {r.l + 2, r.t + 1, r.r - 2, r.b - 1}, br,
                         D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
    }
    if (dragged) drawWindow(*dragged, drag_.rect, drag_.desktop == current);
    if (rt->EndDraw() == D2DERR_RECREATE_TARGET) return;

    POINT src{0, 0};
    SIZE size{w_, h_};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd_, nullptr, nullptr, &size, surf_.dc, &src, 0, &blend, ULW_ALPHA);
}

void Pager::setHover(int i) {
    if (i == hover_) return;
    hover_ = i;
    render();
    KillTimer(hwnd_, kTimerTip);
    if (i < 0 || tipSuppressed_) return tip_.hide();
    if (tip_.visible()) return showTip(i);
    UINT ms = 400;
    SystemParametersInfoW(SPI_GETMOUSEHOVERTIME, 0, &ms, 0);
    SetTimer(hwnd_, kTimerTip, ms, nullptr);
}

void Pager::showTip(int i) {
    const FRect r = itemRect(i);
    RECT a{static_cast<LONG>(r.l), static_cast<LONG>(r.t), static_cast<LONG>(r.r), static_cast<LONG>(r.b)};
    MapWindowPoints(hwnd_, nullptr, reinterpret_cast<POINT*>(&a), 2);
    RECT bar;
    GetWindowRect(taskbar_, &bar);
    tip_.show(a, bar, i, app_.settings.showOnlyCurrentScreen ? monitor_ : nullptr, scale_);
}

void Pager::endDrag(bool drop) {
    const Drag d = drag_;
    drag_ = {};
    if (GetCapture() == hwnd_) ReleaseCapture();
    if (drop && d.active && d.target >= 0 && d.target != d.desktop) app_.moveWindow(d.hwnd, d.target);
    render();
}

LRESULT Pager::handle(UINT msg, WPARAM wp, LPARAM lp) {
    const POINT pt{GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
    switch (msg) {
    case WM_MOUSEMOVE: {
        if (!tracking_) {
            TRACKMOUSEEVENT t{sizeof t, TME_LEAVE, hwnd_, 0};
            tracking_ = TrackMouseEvent(&t);
        }
        if (!drag_.pressed) {
            setHover(desktopAt(pt));
            return 0;
        }
        const LONG dx = pt.x - drag_.start.x, dy = pt.y - drag_.start.y;
        if (!drag_.active && drag_.hwnd && (std::abs(dx) > 1 || std::abs(dy) > 1)) drag_.active = true;  // KDE: threshold 1
        if (drag_.active) {
            const FRect& o = drag_.origin;
            const float w = o.r - o.l, h = o.b - o.t;
            const float x = std::clamp(o.l + dx, 0.f, std::max(0.f, w_ - w));
            const float y = std::clamp(o.t + dy, 0.f, std::max(0.f, h_ - h));
            drag_.rect = {x, y, x + w, y + h};
            drag_.target = desktopAt({static_cast<LONG>(x + w / 2), static_cast<LONG>(y + h / 2)});
            render();
        }
        return 0;
    }
    case WM_MOUSELEAVE:
        tracking_ = false;
        tipSuppressed_ = false;
        if (!drag_.pressed) setHover(-1);
        return 0;
    case WM_LBUTTONDOWN: {
        KillTimer(hwnd_, kTimerTip);
        tip_.hide();
        tipSuppressed_ = true;
        const int d = desktopAt(pt);
        if (d < 0) return 0;
        SetCapture(hwnd_);
        drag_ = {};
        drag_.pressed = true;
        drag_.desktop = d;
        drag_.start = pt;
        if (const Win* w = app_.settings.showWindowOutlines ? windowAt(d, pt) : nullptr) {
            drag_.hwnd = w->hwnd;
            drag_.origin = drag_.rect = mapWindowRect(w->rect, area_, itemRect(d));
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        if (!drag_.pressed) return 0;
        const bool click = !drag_.active && desktopAt(pt) == drag_.desktop;
        const int d = drag_.desktop;
        endDrag(true);
        if (click) app_.switchTo(d);
        return 0;
    }
    case WM_CAPTURECHANGED:
        if (drag_.pressed && reinterpret_cast<HWND>(lp) != hwnd_) endDrag(false);
        return 0;
    case WM_RBUTTONUP: {
        POINT s = pt;
        ClientToScreen(hwnd_, &s);
        tip_.hide();
        app_.contextMenu(s);
        return 0;
    }
    case WM_MOUSEWHEEL:
    case WM_MOUSEHWHEEL: {
        const int delta = GET_WHEEL_DELTA_WPARAM(wp);
        wheel_ += msg == WM_MOUSEHWHEEL ? -delta : delta;
        int steps = 0;
        for (; wheel_ >= WHEEL_DELTA; wheel_ -= WHEEL_DELTA) --steps;  // wheel up = previous desktop
        for (; wheel_ <= -WHEEL_DELTA; wheel_ += WHEEL_DELTA) ++steps;
        if (steps) {
            wheel_ = 0;
            const int n = static_cast<int>(app_.model.desktops.size()), cur = app_.currentDesktop();
            int t = cur;
            for (int i = 0; i < std::abs(steps); ++i) t = stepDesktop(t, n, steps > 0 ? 1 : -1, app_.settings.wrapPage);
            if (t != cur) app_.switchTo(t);
        }
        return msg == WM_MOUSEHWHEEL;
    }
    case WM_TIMER:
        KillTimer(hwnd_, wp);
        if (wp == kTimerTip && hover_ >= 0 && !tipSuppressed_) showTip(hover_);
        if (wp == kTimerDrop && dropDesktop_ >= 0 && dropDesktop_ != app_.currentDesktop())
            app_.switchTo(dropDesktop_, false);  // don't steal focus from the drag source
        return 0;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    case WM_DPICHANGED_AFTERPARENT:
        update(true, true);
        return 0;
    case WM_DESTROY:
        RevokeDragDrop(hwnd_);
        break;
    case WM_NCDESTROY: {
        const LRESULT r = DefWindowProcW(hwnd_, msg, wp, lp);
        hwnd_ = nullptr;
        app_.pagerDied();  // e.g. the taskbar went away with its monitor
        return r;
    }
    }
    return DefWindowProcW(hwnd_, msg, wp, lp);
}
// ---- src/app.cpp ----


#include <shellapi.h>
#include <shlobj.h>
// clang-format off
#include <initguid.h>
#include <propkey.h>
// clang-format on

#include <algorithm>
#include <utility>

using Microsoft::WRL::ComPtr;

namespace {

App* g_app = nullptr;

enum : UINT_PTR { kTimerRefresh = 1, kTimerReinit, kTimerTaskbars, kTimerActivate, kTimerSlide };
enum : unsigned { kDesktops = 1, kWindows = 2, kGeometry = 4, kLayout = 8, kTaskbars = 16, kTitles = 32 };
enum : int { kHotkeyLeft = 1, kHotkeyRight = 2 };
enum : UINT { IDM_ADD = 1, IDM_REMOVE, IDM_CONFIGURE_VD, IDM_SETTINGS, IDM_EXIT };

// uxtheme ordinals 135/136 (SetPreferredAppMode / FlushMenuThemes)
void setMenuTheme(bool dark) {
    enum class Mode { Default, AllowDark, ForceDark, ForceLight };
    using SetMode = Mode(WINAPI*)(Mode);
    using Flush = void(WINAPI*)();
    static const HMODULE ux = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    static const auto set = reinterpret_cast<SetMode>(GetProcAddress(ux, MAKEINTRESOURCEA(135)));
    static const auto flush = reinterpret_cast<Flush>(GetProcAddress(ux, MAKEINTRESOURCEA(136)));
    if (set) set(dark ? Mode::ForceDark : Mode::ForceLight);
    if (flush) flush();
}

// ask the shell for the app's tile icon via its AUMID (UWP apps)
HBITMAP packagedAppIcon(HWND h) {
    ComPtr<IPropertyStore> store;
    if (FAILED(SHGetPropertyStoreForWindow(h, IID_PPV_ARGS(&store)))) return nullptr;
    PROPVARIANT v;
    PropVariantInit(&v);
    HBITMAP bmp = nullptr;
    ComPtr<IShellItemImageFactory> f;
    if (SUCCEEDED(store->GetValue(PKEY_AppUserModel_ID, &v)) && v.vt == VT_LPWSTR &&
        SUCCEEDED(SHCreateItemInKnownFolder(FOLDERID_AppsFolder, KF_FLAG_DONT_VERIFY, v.pwszVal, IID_PPV_ARGS(&f))))
        f->GetImage({32, 32}, SIIGBF_ICONONLY, &bmp);
    PropVariantClear(&v);
    return bmp;
}

// A window's icon, looked up on the thread pool and posted back as WM_APP_ICON: WM_GETICON waits on the window's app
// and the packaged-app lookup on the shell, and the pagers share the taskbar's input queue, so neither may block here.
struct IconRequest {
    HWND controller, window;
    HICON icon = nullptr;
    HBITMAP bitmap = nullptr;
};

void CALLBACK fetchIcon(PTP_CALLBACK_INSTANCE, void* p) {
    auto* r = static_cast<IconRequest*>(p);
    HICON ic = nullptr;
    for (WPARAM type : {WPARAM(ICON_BIG), WPARAM(ICON_SMALL2)}) {
        DWORD_PTR res = 0;
        if (!ic && SendMessageTimeoutW(r->window, WM_GETICON, type, 0, SMTO_ABORTIFHUNG, 50, &res) && res) ic = reinterpret_cast<HICON>(res);
    }
    if (!ic) ic = reinterpret_cast<HICON>(GetClassLongPtrW(r->window, GCLP_HICON));
    if (ic) {
        r->icon = CopyIcon(ic);  // the app may destroy its own before the controller converts it
    } else if (SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {
        r->bitmap = packagedAppIcon(r->window);
        CoUninitialize();
    }
    if (!PostMessageW(r->controller, WM_APP_ICON, 0, reinterpret_cast<LPARAM>(r))) {  // the controller is gone
        if (r->icon) DestroyIcon(r->icon);
        if (r->bitmap) DeleteObject(r->bitmap);
        delete r;
    }
}

// The mod's pager starts at every sign-in and settings change: report a Windows build it doesn't know once, not each time.
bool unsupportedNoticeDue() {
    wchar_t build[16] = L"", seen[16] = L"";
    DWORD cb = sizeof build;
    RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"CurrentBuild", RRF_RT_REG_SZ, nullptr, build, &cb);
    Wh_GetStringValue(L"unsupportedBuild", seen, ARRAYSIZE(seen));
    if (!wcscmp(build, seen)) return false;
    Wh_SetStringValue(L"unsupportedBuild", build);
    return true;
}

// shared OSD host from explorer (from modernflyout repo)
// ZBID_ABOVELOCK_UX
bool isSystemOsd(HWND h) {
    using GetBand = BOOL(WINAPI*)(HWND, DWORD*);
    static const auto getBand = reinterpret_cast<GetBand>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetWindowBand"));
    constexpr DWORD ZBID_ABOVELOCK_UX = 18;
    DWORD pid = 0, shell = 0, band = 0;
    GetWindowThreadProcessId(h, &pid);
    GetWindowThreadProcessId(GetShellWindow(), &shell);
    wchar_t cls[32];
    return pid == shell && GetClassNameW(h, cls, 32) && !wcscmp(cls, L"XamlExplorerHostIslandWindow") && getBand &&
           getBand(h, &band) && band == ZBID_ABOVELOCK_UX;
}

bool shellOverlayActive() {
    wchar_t cls[32];
    const HWND f = GetForegroundWindow();
    return f && GetClassNameW(f, cls, 32) && !wcscmp(cls, L"XamlExplorerHostIslandWindow");
}

void pressWinTab() {
    const WORD keys[4] = {VK_LWIN, VK_TAB, VK_TAB, VK_LWIN};
    INPUT in[4]{};
    for (int i = 0; i < 4; ++i) {
        in[i].type = INPUT_KEYBOARD;
        in[i].ki.wVk = keys[i];
        in[i].ki.wScan = static_cast<WORD>(MapVirtualKeyW(keys[i], MAPVK_VK_TO_VSC));  // RDP sessions drop scan code 0
        in[i].ki.dwFlags = (i >= 2 ? KEYEVENTF_KEYUP : 0) | (keys[i] == VK_LWIN ? KEYEVENTF_EXTENDEDKEY : 0);
    }
    SendInput(4, in, sizeof(INPUT));
}

}  // namespace

App::App() = default;
App::~App() = default;

int App::run(HINSTANCE hinst) {
    g_app = this;
    inst = hinst;
    settings.load();
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2d.GetAddressOf())) ||
        FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                   reinterpret_cast<IUnknown**>(dwrite.GetAddressOf()))) ||
        FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic))) || !createTarget())
        return 1;

    WNDCLASSEXW wc{sizeof wc};
    wc.lpfnWndProc = proc;
    wc.hInstance = inst;
    wc.lpszClassName = kControllerClass;
    RegisterClassExW(&wc);
    if (!CreateWindowExW(WS_EX_TOOLWINDOW, kControllerClass, L"WinPager", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, this))
        return 1;
    msgTaskbarCreated_ = RegisterWindowMessageW(L"TaskbarCreated");

    applyTheme();
    initApi();
    static constexpr std::pair<DWORD, DWORD> kEvents[] = {
        {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
        {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE},
        {EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_NAMECHANGE},
        {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
    };
    for (auto [lo, hi] : kEvents)
        if (HWINEVENTHOOK h = SetWinEventHook(lo, hi, nullptr, onWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS))
            hooks_.push_back(h);
    registerHotkeys();
    schedule(kTaskbars | kDesktops | kWindows | kLayout);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    for (HWINEVENTHOOK h : hooks_) UnhookWinEvent(h);
    if (reorderHook_) UnhookWinEvent(reorderHook_);
    pagers_.clear();
    api.reset();
    g_app = nullptr;
    return static_cast<int>(msg.wParam);
}

bool App::createTarget() {
    icons_.clear();
    layer.Reset();
    brush.Reset();
    rt.Reset();
    // keep D3D/driver DLLs out of the process
    const auto props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE,
                                                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
    if (FAILED(d2d->CreateDCRenderTarget(&props, &rt))) return false;
    rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    return SUCCEEDED(rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &brush)) && SUCCEEDED(rt->CreateLayer(&layer));
}

LRESULT CALLBACK App::proc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCCREATE) {
        auto* a = static_cast<App*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);
        a->hwnd_ = h;
        SetWindowLongPtrW(h, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(a));
    }
    auto* a = reinterpret_cast<App*>(GetWindowLongPtrW(h, GWLP_USERDATA));
    return a ? a->handle(msg, wp, lp) : DefWindowProcW(h, msg, wp, lp);
}

LRESULT App::handle(UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == msgTaskbarCreated_ && msg) {  // explorer (re)started
        pagers_.clear();
        initApi();
        schedule(kTaskbars | kDesktops | kWindows | kLayout);
        return 0;
    }
    switch (msg) {
    case WM_APP_VD:
        if (wp == vd::Current) {
            switchTick_ = GetTickCount64();
            hideSystemOsd();
        }
        schedule(wp == vd::View ? kWindows : kDesktops | kWindows);
        return 0;
    case WM_APP_TASKBARS:
        schedule(kTaskbars);
        return 0;
    case WM_APP_SETTINGS:
        openSettings();
        return 0;
    case WM_APP_ICON: {  // fetchIcon's answer; its handles are ours to free
        const std::unique_ptr<IconRequest> r(reinterpret_cast<IconRequest*>(lp));
        ComPtr<IWICBitmap> wb;
        if (r->icon) wic->CreateBitmapFromHICON(r->icon, &wb);
        else if (r->bitmap) wic->CreateBitmapFromHBITMAP(r->bitmap, nullptr, WICBitmapUsePremultipliedAlpha, &wb);
        if (r->icon) DestroyIcon(r->icon);
        if (r->bitmap) DeleteObject(r->bitmap);
        const auto it = icons_.find(r->window);  // gone if the window closed meanwhile
        ComPtr<IWICFormatConverter> cv;
        if (it != icons_.end() && wb && SUCCEEDED(wic->CreateFormatConverter(&cv)) &&
            SUCCEEDED(cv->Initialize(wb.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom)) &&
            SUCCEEDED(rt->CreateBitmapFromWicBitmap(cv.Get(), &it->second)))
            for (auto& p : pagers_) p->update(false, true);
        return 0;
    }
    case WM_TIMER:
        if (wp == kTimerRefresh) {
            refresh();
        } else if (wp == kTimerReinit) {
            initApi();
        } else if (wp == kTimerTaskbars) {
            KillTimer(hwnd_, kTimerTaskbars);
            schedule(kTaskbars | kLayout);
        } else if (wp == kTimerActivate) {
            KillTimer(hwnd_, kTimerActivate);
            if (activateOn_ >= 0 && activateOn_ == model.current) SetForegroundWindow(GetShellWindow());
            activateOn_ = -1;
        } else if (wp == kTimerSlide) {  // missed the end of the slide animation
            slide_ = nullptr;
            flushOsd();
        }
        return 0;
    case WM_HOTKEY:
        moveForeground(wp == kHotkeyLeft ? -1 : 1);
        return 0;
    case WM_SETTINGCHANGE:
        if (lp && !wcscmp(reinterpret_cast<const wchar_t*>(lp), L"ImmersiveColorSet")) applyTheme();
        schedule(kLayout);
        return 0;
    case WM_DWMCOLORIZATIONCOLORCHANGED:
        applyTheme();
        return 0;
    case WM_DISPLAYCHANGE:
        schedule(kTaskbars | kLayout);
        SetTimer(hwnd_, kTimerTaskbars, 2000, nullptr);  // explorer recreates secondary taskbars a bit later
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd_);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd_, msg, wp, lp);
}

void CALLBACK App::onWinEvent(HWINEVENTHOOK, DWORD ev, HWND h, LONG obj, LONG child, DWORD, DWORD) {
    App* a = g_app;
    if (!a || !h) return;
    if (ev == EVENT_OBJECT_REORDER || ev == EVENT_SYSTEM_FOREGROUND) {
        // Keep above explorer's XAML bridge, and retry embedding
        // while Start/Search/Task View are up the taskbar sits in band ZBID_IMMERSIVE_MOGO
        // and refuses cross-process child creation
        for (auto& p : a->pagers_) p->raise();
        if (std::ranges::any_of(a->pagers_, [](const auto& p) { return !p->alive(); })) a->schedule(kTaskbars);
        if (ev == EVENT_OBJECT_REORDER) return;
    }
    if (obj != OBJID_WINDOW || child != CHILDID_SELF) return;
    if (h == a->slide_ && (ev == EVENT_OBJECT_HIDE || ev == EVENT_OBJECT_DESTROY)) {
        a->slide_ = nullptr;
        a->flushOsd();
        return;
    }
    const bool tracked = std::ranges::any_of(a->model.windows, [h](const Win& w) { return w.hwnd == h; });
    switch (ev) {
    case EVENT_OBJECT_LOCATIONCHANGE:
        if (tracked) {
            a->schedule(kGeometry);
        } else if (std::ranges::any_of(a->pagers_, [h](const auto& p) { return p->taskbar() == h || GetParent(h) == p->taskbar(); })) {
            a->schedule(kLayout);  // taskbar resized or the tray grew
        }
        return;
    case EVENT_OBJECT_NAMECHANGE:
        if (tracked) a->schedule(kTitles);
        return;
    case EVENT_OBJECT_DESTROY:
        if (tracked) a->schedule(kWindows);
        return;
    }
    if (!tracked && GetAncestor(h, GA_PARENT) != GetDesktopWindow()) return;
    if (ev == EVENT_OBJECT_SHOW && !tracked) {
        wchar_t cls[40] = L"";
        GetClassNameW(h, cls, 40);
        if (!wcscmp(cls, L"Shell_SecondaryTrayWnd")) a->schedule(kTaskbars);
        if (!wcscmp(cls, L"VirtualDesktopHotkeySwitcher")) {  // explorer's full-screen slide
            a->slide_ = h;
            return;
        }
        if (isSystemOsd(h)) {
            a->systemOsd_ = h;
            a->hideSystemOsd();
            return;
        }
    }
    a->schedule(kWindows);
}

void App::schedule(unsigned d) {
    dirty_ |= d;
    if (!timerPending_) timerPending_ = SetTimer(hwnd_, kTimerRefresh, 16, nullptr) != 0;
}

void App::refresh() {
    KillTimer(hwnd_, kTimerRefresh);
    timerPending_ = false;
    unsigned d = std::exchange(dirty_, 0u);
    if (d & kTaskbars) {
        rebuildPagers();
        if (std::ranges::any_of(pagers_, [](const auto& p) { return !p->alive(); })) SetTimer(hwnd_, kTimerTaskbars, 1000, nullptr);
        d |= kLayout;
    }
    bool changed = d & (kLayout | kTitles);
    if ((d & kDesktops) && api.ready()) {
        HRESULT hr = S_OK;
        const size_t before = model.desktops.size();
        const GUID was = before ? model.desktops[model.current].id : GUID{};
        changed |= model.refreshDesktops(api, hr);
        pending_ = -1;
        if (vd::isDisconnected(hr)) {
            api.reset();
            SetTimer(hwnd_, kTimerReinit, 1000, nullptr);
        }
        if (model.desktops.size() != before) d |= kLayout;
        d |= kWindows;
        if (SUCCEEDED(hr) && before && !model.desktops.empty() && model.desktops[model.current].id != was)
            showOsd(model.indexOf(was), model.current);
    }
    if (d & kWindows) {
        changed |= model.refreshWindows(api);
        if (activateOn_ >= 0 && activateOn_ == model.current) {
            // API switches leave focus on the now-cloaked window
            // do what win+ctrl+arrow does
            const auto top = std::ranges::find_if(model.windows, [&](const Win& w) { return w.desktop == activateOn_ && !w.minimized; });
            if (top != model.windows.end()) {
                SetForegroundWindow(top->hwnd);
                activateOn_ = -1;
                KillTimer(hwnd_, kTimerActivate);
            }
        }
        std::erase_if(icons_, [this](const auto& kv) {
            return std::ranges::none_of(model.windows, [&](const Win& w) { return w.hwnd == kv.first; });
        });
    } else if (d & kGeometry) {
        changed |= model.refreshGeometry();
    }
    for (auto& p : pagers_) p->update(d & kLayout, changed);
}

void App::initApi() {
    KillTimer(hwnd_, kTimerReinit);
    const HRESULT hr = api.init(hwnd_, WM_APP_VD);
    if (SUCCEEDED(hr)) return schedule(kDesktops | kWindows | kLayout);
    if (hr == E_NOINTERFACE) {
        if (unsupportedNoticeDue())
            MessageBoxW(nullptr,
                        L"This Windows build exposes a virtual desktop interface WinPager doesn't know.\n"
                        L"WinPager targets Windows 11 24H2/25H2 (build 26100 and later).",
                        L"WinPager", MB_ICONERROR);
        DestroyWindow(hwnd_);
        return;
    }
    SetTimer(hwnd_, kTimerReinit, 2000, nullptr);  // explorer not (yet) running
}

void App::applyTheme() {
    palette = pagerPalette(settings);
    setMenuTheme(palette.dark);
    for (auto& p : pagers_) p->restyle();
    if (osd_) osd_->restyle();
}

void App::showOsd(int from, int to) {
    if (!settings.switchOsd || shellOverlayActive()) return;
    if (slide_ && IsWindowVisible(slide_)) {  // the OSD would play its cross-fade underneath
        if (osdTo_ < 0) osdFrom_ = from;
        osdTo_ = to;
        SetTimer(hwnd_, kTimerSlide, 1000, nullptr);
        return;
    }
    if (!osd_) osd_ = std::make_unique<Osd>(*this);
    osd_->show(from, to);
}

void App::flushOsd() {
    KillTimer(hwnd_, kTimerSlide);
    const int from = std::exchange(osdFrom_, -1), to = std::exchange(osdTo_, -1);
    if (to >= 0 && to == model.current) showOsd(from, to);
}

void App::hideSystemOsd() {
    if (!settings.hideSystemOsd || GetTickCount64() - switchTick_ >= 600) return;
    if (!systemOsd_ || !IsWindow(systemOsd_))  // not seen shown yet this session
        for (HWND h = nullptr; (h = FindWindowExW(nullptr, h, L"XamlExplorerHostIslandWindow", nullptr));)
            if (isSystemOsd(h)) systemOsd_ = h;
    if (systemOsd_ && IsWindowVisible(systemOsd_)) ShowWindowAsync(systemOsd_, SW_HIDE);
}


void App::rebuildPagers() {
    std::vector<HWND> bars;
    HWND main = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (main) bars.push_back(main);
    DWORD shellPid = 0;
    if (main) GetWindowThreadProcessId(main, &shellPid);
    if (shellPid != reorderPid_) {  // z-order changes only matter inside explorer, keep this hook off busy apps
        if (reorderHook_) UnhookWinEvent(reorderHook_);
        reorderHook_ = shellPid ? SetWinEventHook(EVENT_OBJECT_REORDER, EVENT_OBJECT_REORDER, nullptr, onWinEvent, shellPid, 0,
                                                  WINEVENT_OUTOFCONTEXT)
                                : nullptr;
        reorderPid_ = shellPid;
    }
    if (settings.showOnlyCurrentScreen)
        for (HWND h = nullptr; (h = FindWindowExW(nullptr, h, L"Shell_SecondaryTrayWnd", nullptr));) bars.push_back(h);
    std::erase_if(pagers_, [&](const auto& p) { return !p->alive() || std::ranges::find(bars, p->taskbar()) == bars.end(); });
    for (HWND b : bars)
        if (std::ranges::none_of(pagers_, [b](const auto& p) { return p->taskbar() == b; }))
            pagers_.push_back(std::make_unique<Pager>(*this, b));
}

void App::registerHotkeys() {
    const Hotkey* keys[2] = {&settings.moveLeft, &settings.moveRight};
    for (int i = 0; i < 2; ++i) {
        UnregisterHotKey(hwnd_, kHotkeyLeft + i);
        hotkeyOk[i] = !keys[i]->vk || RegisterHotKey(hwnd_, kHotkeyLeft + i, keys[i]->mods | MOD_NOREPEAT, keys[i]->vk);
        if (!hotkeyOk[i]) Wh_Log(L"%s: another app already uses this shortcut", i ? L"moveRight" : L"moveLeft");
    }
}

void App::switchTo(int desktop, bool activate) {
    if (desktop < 0 || desktop >= static_cast<int>(model.desktops.size())) return;
    if (desktop == currentDesktop()) return activateCurrent();
    if (FAILED(api.switchTo(model.desktops[desktop].id))) return;
    pending_ = desktop;
    if (!activate) return;
    activateOn_ = desktop;
    SetTimer(hwnd_, kTimerActivate, 700, nullptr);  // empty desktop
}

void App::activateCurrent() {
    if (settings.currentDesktopSelected == 1) {
        if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) PostMessageW(tray, WM_COMMAND, 407, 0);  // Show desktop toggle
    } else if (settings.currentDesktopSelected == 2) {
        pressWinTab();
    }
}

void App::moveWindow(HWND hwnd, int desktop) {
    if (desktop < 0 || desktop >= static_cast<int>(model.desktops.size())) return;
    api.moveWindow(hwnd, model.desktops[desktop].id);
    schedule(kWindows);
}

void App::moveForeground(int dir) {
    if (timerPending_) refresh();
    const int n = static_cast<int>(model.desktops.size()), cur = currentDesktop();
    const int t = stepDesktop(cur, n, dir, settings.wrapPage);
    if (t == cur) return;
    const GUID id = model.desktops[t].id;
    const HWND fg = GetForegroundWindow();
    const HWND root = fg ? GetAncestor(fg, GA_ROOTOWNER) : nullptr;
    const bool movable = root && std::ranges::any_of(model.windows, [root](const Win& w) { return w.hwnd == root; });
    HRESULT hr = movable ? api.moveForegroundAndSwitch(id) : api.switchTo(id);
    if (FAILED(hr) && movable && SUCCEEDED(hr = api.moveWindow(root, id)) && SUCCEEDED(hr = api.switchTo(id)))
        SetForegroundWindow(root);
    if (SUCCEEDED(hr)) pending_ = t;
}

void App::contextMenu(POINT pt) {
    const int n = static_cast<int>(model.desktops.size());
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, IDM_ADD, L"Add Virtual Desktop");
    AppendMenuW(m, MF_STRING | (n > 1 ? 0 : MF_GRAYED), IDM_REMOVE, L"Remove Virtual Desktop");
    AppendMenuW(m, MF_STRING, IDM_CONFIGURE_VD, L"Configure Virtual Desktops…");
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, IDM_SETTINGS, L"Configure WinPager…");
    SetForegroundWindow(hwnd_);
    const UINT cmd = TrackPopupMenuEx(m, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, hwnd_, nullptr);
    PostMessageW(hwnd_, WM_NULL, 0, 0);
    DestroyMenu(m);
    switch (cmd) {
    case IDM_ADD:
        api.create();
        break;
    case IDM_REMOVE:
        if (n > 1) api.remove(model.desktops[n - 1].id, model.desktops[n - 2].id);
        break;
    case IDM_CONFIGURE_VD:
        ShellExecuteW(nullptr, L"open", L"ms-settings:multitasking", nullptr, nullptr, SW_SHOWNORMAL);
        break;
    case IDM_SETTINGS:
        openSettings();
        break;
    }
}

void App::openSettings() {
    std::wstring exe(MAX_PATH, L'\0');
    exe.resize(GetModuleFileNameW(nullptr, exe.data(), MAX_PATH));
    exe = exe.substr(0, exe.rfind(L'\\') + 1) + L"windhawk.exe";
    ShellExecuteW(nullptr, nullptr, exe.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

ID2D1Bitmap* App::icon(HWND h) {
    auto [it, fresh] = icons_.try_emplace(h);
    if (fresh) {  // drawn without one until WM_APP_ICON
        std::unique_ptr<IconRequest> r(new IconRequest{hwnd_, h});
        if (TrySubmitThreadpoolCallback(fetchIcon, r.get(), nullptr)) r.release();
    }
    return it->second.Get();
}

// ---- windhawk/winpager.wh.cpp, continued ----
// ---------------------------------------------------------------- Windhawk
// What src/settings.cpp and src/main.cpp do for WinPager.exe: the settings are the mod's, and the pager runs on a
// thread of the tool mod process.

void Settings::load() {
    using WindhawkUtils::StringSetting;
    auto flag = [](const wchar_t* name, bool& v) { v = Wh_GetIntSetting(name) != 0; };
    auto choice = [](const wchar_t* name, int& v, std::initializer_list<const wchar_t*> options) {
        const StringSetting s = StringSetting::make(name);
        for (int i = 0; const wchar_t* o : options) {
            if (!wcscmp(s, o)) v = i;
            ++i;
        }
    };
    auto hotkey = [](const wchar_t* name, Hotkey& h) {
        const StringSetting s = StringSetting::make(name);
        const auto [mods, vk] = parseHotkey(s);
        if (!vk && *s.get()) Wh_Log(L"%s: can't read \"%s\", expected a virtual-key code like ctrl+shift+win+37", name, s.get());
        h = {mods, vk};
    };
    flag(L"showWindowOutlines", showWindowOutlines);
    flag(L"showWindowIcons", showWindowIcons);
    flag(L"showOnlyCurrentScreen", showOnlyCurrentScreen);
    flag(L"wrapPage", wrapPage);
    choice(L"displayedText", displayedText, {L"number", L"name", L"none"});
    choice(L"currentDesktopSelected", currentDesktopSelected, {L"nothing", L"showDesktop", L"taskView"});
    choice(L"position", position, {L"left", L"tray"});
    maxDesktops = std::clamp(Wh_GetIntSetting(L"maxDesktops"), 0, 99);
    choice(L"theme", theme, {L"windows", L"light", L"dark"});
    flag(L"accentHighlight", accentHighlight);
    flag(L"switchOsd", switchOsd);
    flag(L"hideSystemOsd", hideSystemOsd);
    hotkey(L"moveLeft", moveLeft);
    hotkey(L"moveRight", moveRight);
}

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

HANDLE g_pagerThread;
DWORD g_pagerThreadId;

BOOL CALLBACK closeController(HWND h, LPARAM) {
    wchar_t cls[64];
    if (GetClassNameW(h, cls, 64) && !wcscmp(cls, kControllerClass)) DestroyWindow(h);
    return TRUE;
}

DWORD WINAPI pagerThread(void*) {
    // WinPager.exe's mutex: only one of the two draws
    const HANDLE single = CreateMutexW(nullptr, FALSE, L"Local\\WinPager.instance");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        Wh_Log(L"WinPager.exe is running");
        CloseHandle(single);
        return 0;
    }
    if (SUCCEEDED(OleInitialize(nullptr))) {  // STA + OLE drag and drop
        {
            App app;
            app.run(reinterpret_cast<HINSTANCE>(&__ImageBase));
            // stopPager's WM_QUIT leaves the controller window up: close it while App is still alive
            EnumThreadWindows(GetCurrentThreadId(), closeController, 0);
        }
        OleUninitialize();
    }
    if (single) CloseHandle(single);
    return 0;
}

void startPager() {
    g_pagerThread = CreateThread(nullptr, 0, pagerThread, nullptr, 0, &g_pagerThreadId);
}

void stopPager() {
    if (!g_pagerThread) return;
    // ends App::run's message loop; PostThreadMessage fails until the thread has a message queue
    while (!PostThreadMessageW(g_pagerThreadId, WM_QUIT, 0, 0) && WaitForSingleObject(g_pagerThread, 10) == WAIT_TIMEOUT) {
    }
    WaitForSingleObject(g_pagerThread, INFINITE);
    CloseHandle(g_pagerThread);
    g_pagerThread = nullptr;
}

}  // namespace

BOOL WhTool_ModInit() {
    startPager();
    return g_pagerThread != nullptr;
}

void WhTool_ModSettingsChanged() {  // start over with the new settings
    stopPager();
    startPager();
}

void WhTool_ModUninit() {
    stopPager();
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
