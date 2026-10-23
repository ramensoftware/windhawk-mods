// ==WindhawkMod==
// @id              full-mac-experience
// @name            The Full MacOS Experience
// @description     macOS-style dock with magnification, Apple logo Start button, fast Launchpad and smart auto-hide
// @version         1.3
// @author          stack0ntop
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -lgdi32 -lshell32 -lshlwapi -luuid -lshcore -ladvapi32 -ldwmapi
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 🍎 The Full MacOS Experience

Bring a macOS-inspired desktop experience to Windows with a sleek dock, smooth magnification, and quick access to your apps.

Created by **stack0ntop**.

## ✨ Features

- **macOS-style Dock** — Give your Windows desktop a dock inspired by macOS.
- **Dock Magnification** — Enjoy a magnification effect as you interact with dock items.
- **Apple Logo Start Button** — A macOS-inspired alternative for accessing the Windows Start menu.
- **Fast Launchpad** — Quickly access your applications through a Launchpad-style interface.
- **Smart Auto-Hide** — Keep the dock out of the way when it isn't needed.

## 📸 Screenshots

![Dock Preview](https://raw.githubusercontent.com/stack0ntop/the-full-mac-experience/main/screenshots/dock.png)
![Launchpad Preview](https://raw.githubusercontent.com/stack0ntop/the-full-mac-experience/main/screenshots/launchpad.png)


## 📦 Requirements

- Windows
- [Windhawk](https://windhawk.net/)

## 📥 Installation

1. Install [Windhawk](https://windhawk.net/) if you haven't already.
2. Open Windhawk and locate the mod source.
3. Install or compile **The Full MacOS Experience** using the supported Windhawk workflow.
4. Enable the mod and follow any instructions shown in Windhawk.

If you're installing from source, follow the instructions provided by the repository.

## 🛠️ Troubleshooting

If the dock doesn't appear or behaves unexpectedly:

1. Check that the mod is enabled in Windhawk.
2. Restart Windows Explorer or sign out and back in.
3. Check the mod's logs in Windhawk for errors.
4. Make sure your Windows version is compatible with the mod.

Windows updates may affect compatibility. If you encounter a bug, please report it through the repository's issue tracker.

## 🔗 Links

- **Source Code:** [GitHub Repository](https://github.com/stack0ntop/the-full-mac-experience)
- **Bug Reports:** [Open an Issue](https://github.com/stack0ntop/the-full-mac-experience/issues)
- **Windhawk:** [windhawk.net](https://windhawk.net/)

Replace the placeholder GitHub links above with your actual repository URL.

## 📄 License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.

See the repository's `LICENSE` file for the full license text.

## 🙏 Credits

- [Windhawk](https://windhawk.net/) — Mod framework.
- Apple — Inspiration for the macOS-style interface.
- Microsoft — Windows platform.

This is an independent community project and is not affiliated with, endorsed by, or sponsored by Apple or Microsoft.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dock:
    - size: 32
      $name: Dock size
    - tint: 20
      $name: Dock tint
    - blur: 35
      $name: Dock blur
    - color: 0
      $name: Dock color
  $name: Dock appearance and sizeC:\ProgramData\Windhawk\EditorWorkspace\mod.wh.cpp
- magnification:
    - amount: 40
      $name: Magnification amount
    - range: 50
      $name: Magnification range
    - smoothing: 100
      $name: Magnification smoothing
    - overWindows: true
      $name: Magnify over windows
  $name: Magnification
- items:
    - recycleBin: true
      $name: Show Recycle Bin
    - appsDivider: true
      $name: Show apps divider
  $name: Dock items
- extras:
    - appleLogo: true
      $name: Apple logo on the Start button
      $description: Replaces the Windows logo. Clicking it opens Launchpad.
    - hideTray: true
      $name: Hide the clock and system tray
  $name: Taskbar extras
- launchpad:
    - availability: enabled
      $name: Launchpad
      $description: Size, grid and app names are changed from the gear button inside Launchpad. If you turned Launchpad off there, saving these settings with Enabled brings it back.
      $options:
      - enabled: Enabled
      - disabled: Disabled (the Start button opens the Windows Start menu)
    - animations: true
      $name: Launchpad fade animations
      $description: Turn off for the lowest CPU use on slow PCs.
  $name: Launchpad
- autoHide:
    - mode: intersected
      $name: Hide the dock
      $options:
      - intersected: When a window is maximized or touches the dock
      - maximized: Only when a window is maximized
      - off: Never (always shown)
    - indicator: true
      $name: Swipe-up line while the dock is hidden
      $description: A thin white line at the bottom of the screen, like on an iPhone.
    - fade: true
      $name: Fade the dock in and out
    - foregroundOnly: false
      $name: Only the active window counts
    - primaryMonitorOnly: false
      $name: Only on the primary monitor
  $name: Auto-hide
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellapi.h>
#include <shellscalingapi.h>
#include <sddl.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cwctype>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <memory>
#include <mutex>
#include <numeric>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <dwmapi.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

namespace wadt = winrt::Windows::ApplicationModel::DataTransfer;
namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuc = winrt::Windows::UI::Composition;
namespace wge = winrt::Windows::Graphics::Effects;

int g_dockSize = 32;
float g_scale = 1.4f;
int g_dockTint = 20;
int g_dockBlur = 35;
int g_magnificationRange = 50;
int g_dockTheme = 0;
int g_animationMs = 100;
bool g_showTrash = true;
bool g_appsDivider = true;
bool g_magnifyOverWindows = true;
bool g_appleLogo = true;
bool g_hideTray = true;
bool g_startHooked = false;

float g_iconSize = 39.0f;
float g_radius = 130.0f;
float g_buttonWidth = 47.0f;
float g_buttonMargin = 2.5f;
float g_dockHeight = 65.0f;
float g_dockSidePadding = 15.0f;
float g_dockRadius = 20.0f;
float g_dockBottomGap = 7.0f;
int g_taskbarHeight = 85;
float g_iconLoadSize = 63;
float g_dividerSpace = 14.0f;
float g_trashSlot = 48.0f;
float g_trashSpace = 62.0f;

constexpr float kStockIconSize = 24;
constexpr float kStockButtonWidth = 44;
constexpr int kMinTaskbarHeight = 40;
constexpr float kButtonIconPadding = 8;

constexpr float kIconSpacing = 4;
constexpr float kTrashIconScale = 1.15f;

void ComputeSizes() {
    float icon = std::round(kStockIconSize * std::exp2((g_dockSize - 20) / 50.0f));
    g_iconSize = icon;

    g_dockHeight = std::round(icon * 5 / 3);
    g_dockSidePadding = std::round(icon * 0.385f);
    g_dockRadius = std::round(g_dockHeight * 0.3f);
    g_dockBottomGap = std::round(icon / 6);
    g_radius = icon * (1 + g_magnificationRange / 20.0f);

    g_buttonWidth = std::max(kStockButtonWidth, icon + kButtonIconPadding);
    float gap = kIconSpacing * icon / 12;
    g_buttonMargin = (icon + gap - g_buttonWidth) / 2;

    g_dividerSpace = std::round(icon * 0.35f);
    g_trashSlot = std::round(icon + gap - icon * 0.1f);
    g_trashSpace = g_dividerSpace + g_trashSlot;

    float dpi = (float)GetDpiForSystem() / 96.0f;
    float wantPx = icon * g_scale * dpi;
    float loadPx = wantPx <= 48 ? 48 : wantPx <= 64 ? 64 : 256;
    g_iconLoadSize = loadPx / dpi;

    float dockTop = g_dockBottomGap + g_dockHeight;
    float iconTop = g_dockBottomGap + (g_dockHeight - icon) / 2 + g_scale * icon;
    float highest = std::max(dockTop, iconTop);

    g_taskbarHeight = std::max((int)std::ceil(highest + 2), kMinTaskbarHeight);
}

std::mutex g_shellJobsLock;
std::vector<std::function<void()>> g_shellJobs;
PTP_WORK g_shellJobWork = nullptr;

void CALLBACK RunShellJob(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK) {
    std::function<void()> job;
    {
        std::lock_guard lock(g_shellJobsLock);
        if (g_shellJobs.empty()) return;
        job = std::move(g_shellJobs.front());
        g_shellJobs.erase(g_shellJobs.begin());
    }
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    job();
    if (SUCCEEDED(co)) CoUninitialize();
}

void QueueShellJob(std::function<void()> job) {
    if (!g_shellJobWork) return;
    {
        std::lock_guard lock(g_shellJobsLock);
        g_shellJobs.push_back(std::move(job));
    }
    SubmitThreadpoolWork(g_shellJobWork);
}

constexpr WCHAR kWindowClass[] = L"Windhawk_" WH_MOD_ID;
constexpr UINT kAppBarMessage = WM_APP;
constexpr UINT kUpdateOverWindowsMessage = WM_APP + 1;
constexpr UINT kTrashChangedMessage = WM_APP + 2;
constexpr UINT kTrashCheckedMessage = WM_APP + 3;
constexpr UINT kToggleLaunchpadMessage = WM_APP + 4;

std::atomic<HWND> g_modWindow = nullptr;

HINSTANCE ModuleInstance() {
    HMODULE module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&ModuleInstance, &module);
    return module;
}

LRESULT CALLBACK ModWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ScheduleOverWindows(bool autoHide);

bool RegisterWindowClass() {
    static bool registered = [] {
        WNDCLASS wc{};
        wc.lpfnWndProc = ModWindowProc;
        wc.hInstance = ModuleInstance();
        wc.lpszClassName = kWindowClass;
        return RegisterClass(&wc) != 0;
    }();
    return registered;
}

HWND EnsureModWindow() {
    if (!g_modWindow && RegisterWindowClass()) {
        g_modWindow = CreateWindowEx(0, kWindowClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                     ModuleInstance(), nullptr);
    }
    return g_modWindow;
}

struct Button {
    winrt::weak_ref<wux::FrameworkElement> element;
    wf::IInspectable originalMargin{nullptr};
    int64_t marginToken = 0;
    wux::FrameworkElement::SizeChanged_revoker sizeChanged;
    winrt::weak_ref<wux::FrameworkElement> root;
    wf::IInspectable originalRootMinWidth{nullptr};
    double originalIconColumn = 0;
    double appliedIconColumn = 0;
    float extraLeft = 0, extraRight = 0;
};

[[clang::no_destroy]] std::optional<std::list<Button>> g_buttons{std::in_place};

Button* FindButton(wux::FrameworkElement const& el) {
    if (!g_buttons) return nullptr;
    for (auto& b : *g_buttons) {
        if (b.element.get() == el) return &b;
    }
    return nullptr;
}

void ApplySpacing(wux::FrameworkElement const& el) {
    Button* b = FindButton(el);
    wux::Thickness want{g_buttonMargin + (b ? b->extraLeft : 0), 0,
                        g_buttonMargin + (b ? b->extraRight : 0), 0};
    auto m = el.Margin();
    if (m.Left != want.Left || m.Top != want.Top || m.Right != want.Right ||
        m.Bottom != want.Bottom) {
        el.Margin(want);
    }
}

void RestoreLocalValue(wux::FrameworkElement const& el, wux::DependencyProperty const& dp,
                       wf::IInspectable const& value) {
    if (!value || value == wux::DependencyProperty::UnsetValue()) {
        el.ClearValue(dp);
    } else {
        el.SetValue(dp, value);
    }
}

float GetRowOffsetY(wux::DependencyObject const& row);
void PlaceIndicator(wux::FrameworkElement const& button, float s);

float ButtonPivotY(wux::FrameworkElement const& button) {
    return (float)button.ActualHeight() / 2 + g_iconSize / 2;
}

void SetScale(wux::FrameworkElement const& el, float s, float tx = 0) {
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
    visual.CenterPoint({(float)el.ActualWidth() / 2, ButtonPivotY(el), 0});
    visual.Scale({s, s, 1});
    float ty = GetRowOffsetY(wuxm::VisualTreeHelper::GetParent(el));
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, true);
    visual.Properties().InsertVector3(L"Translation", {tx, ty, 0});
    PlaceIndicator(el, s);
}

struct RowItem {
    winrt::weak_ref<wux::FrameworkElement> element;
    float center = 0;
    float width = 0;
    float scale = 1.0f;
    float shift = 0;
    bool trash = false;
};

std::vector<RowItem> g_row;
winrt::weak_ref<wux::UIElement> g_rowParent;

bool g_pointerOver = false;
bool g_dockExpanded = false;
float g_pointerX = 0;

bool g_anchorLeftEnd = true;
float g_maxGrowth = 0, g_lastGrowth = 0;

struct HiddenBackground {
    winrt::weak_ref<wux::FrameworkElement> element;
    double originalOpacity = 1.0;
};

std::list<HiddenBackground> g_backgrounds;

void HideBackground(wux::FrameworkElement const& el) {
    if (el.Name() != L"BackgroundControl") return;
    g_backgrounds.remove_if([](HiddenBackground const& b) { return !b.element.get(); });
    for (auto& b : g_backgrounds) {
        if (b.element.get() == el) return;
    }
    g_backgrounds.push_back({winrt::make_weak(el), el.Opacity()});
    el.Opacity(0);
    Wh_Log(L"Taskbar background hidden");
}

void RestoreHidden(HiddenBackground const& b) {
    if (auto el = b.element.get()) {
        try { el.Opacity(b.originalOpacity); } catch (...) {}
    }
}

void RestoreBackgrounds() {
    for (auto& b : g_backgrounds) RestoreHidden(b);
    g_backgrounds.clear();
}

template <typename F>
void ForEachDescendant(wux::DependencyObject const& parent, int depth, F& f) {
    int count = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(parent, i);
        bool descend = true;
        if (auto el = child.try_as<wux::FrameworkElement>()) descend = f(el);
        if (descend && depth > 1) ForEachDescendant(child, depth - 1, f);
    }
}

wux::FrameworkElement FindTaskbarFrame(wux::FrameworkElement const& el) {
    wux::DependencyObject current = el;
    for (int depth = 0; current && depth < 20; depth++) {
        if (winrt::get_class_name(current) == L"Taskbar.TaskbarFrame") {
            return current.try_as<wux::FrameworkElement>();
        }
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    return nullptr;
}

struct HiddenHighlight {
    winrt::weak_ref<wux::FrameworkElement> element;
    double originalOpacity = 1.0;
    wux::Visibility originalVisibility = wux::Visibility::Visible;
    int64_t opacityToken = 0, visibilityToken = 0;
};

std::vector<HiddenHighlight> g_highlights;

void ApplyHighlightHidden(wux::UIElement const& el) {
    if (el.Opacity() != 0) el.Opacity(0);
    if (el.Visibility() != wux::Visibility::Collapsed) el.Visibility(wux::Visibility::Collapsed);
}

void HideHighlight(wux::FrameworkElement const& el) {
    g_highlights.erase(std::remove_if(g_highlights.begin(), g_highlights.end(),
                                      [](HiddenHighlight& h) { return !h.element.get(); }),
                       g_highlights.end());
    for (auto& h : g_highlights) {
        if (h.element.get() == el) return;
    }
    auto& h = g_highlights.emplace_back();
    h.element = winrt::make_weak(el);
    h.originalOpacity = el.Opacity();
    h.originalVisibility = el.Visibility();
    auto rehide = [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
        try {
            if (auto ui = sender.try_as<wux::UIElement>()) ApplyHighlightHidden(ui);
        } catch (...) {}
    };
    h.opacityToken = el.RegisterPropertyChangedCallback(wux::UIElement::OpacityProperty(), rehide);
    h.visibilityToken =
        el.RegisterPropertyChangedCallback(wux::UIElement::VisibilityProperty(), rehide);
    ApplyHighlightHidden(el);
}

void UnhideHighlight(wux::FrameworkElement const& el) {
    for (auto it = g_highlights.begin(); it != g_highlights.end(); ++it) {
        if (it->element.get() != el) continue;
        try {
            el.UnregisterPropertyChangedCallback(wux::UIElement::OpacityProperty(), it->opacityToken);
            el.UnregisterPropertyChangedCallback(wux::UIElement::VisibilityProperty(), it->visibilityToken);
            el.Opacity(it->originalOpacity);
            el.Visibility(it->originalVisibility);
        } catch (...) {}
        g_highlights.erase(it);
        return;
    }
}

void RestoreHighlights() {
    for (auto& h : g_highlights) {
        auto el = h.element.get();
        if (!el) continue;
        try {
            el.UnregisterPropertyChangedCallback(wux::UIElement::OpacityProperty(),
                                                 h.opacityToken);
            el.UnregisterPropertyChangedCallback(wux::UIElement::VisibilityProperty(),
                                                 h.visibilityToken);
            el.Opacity(h.originalOpacity);
            el.Visibility(h.originalVisibility);
        } catch (...) {}
    }
    g_highlights.clear();
}

struct IconImage {
    winrt::weak_ref<wux::FrameworkElement> element;
    wf::IInspectable originalWidth{nullptr};
    wf::IInspectable originalHeight{nullptr};
    int64_t widthToken = 0, heightToken = 0;
};

[[clang::no_destroy]] std::optional<std::list<IconImage>> g_iconImages{std::in_place};

void ApplyIconSize(wux::FrameworkElement const& el) {
    if (el.Width() != g_iconSize) el.Width(g_iconSize);
    if (el.Height() != g_iconSize) el.Height(g_iconSize);
}

void TrackIconImage(wux::FrameworkElement const& el) {
    for (auto& i : *g_iconImages) {
        if (i.element.get() == el) return;
    }
    auto& i = g_iconImages->emplace_back();
    i.element = winrt::make_weak(el);
    i.originalWidth = el.ReadLocalValue(wux::FrameworkElement::WidthProperty());
    i.originalHeight = el.ReadLocalValue(wux::FrameworkElement::HeightProperty());
    auto reapply = [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
        try {
            if (auto fe = sender.try_as<wux::FrameworkElement>()) ApplyIconSize(fe);
        } catch (...) {}
    };
    i.widthToken =
        el.RegisterPropertyChangedCallback(wux::FrameworkElement::WidthProperty(), reapply);
    i.heightToken =
        el.RegisterPropertyChangedCallback(wux::FrameworkElement::HeightProperty(), reapply);
    ApplyIconSize(el);
}

void ReleaseIconImage(IconImage const& i) {
    auto el = i.element.get();
    if (!el) return;
    try {
        el.UnregisterPropertyChangedCallback(wux::FrameworkElement::WidthProperty(),
                                             i.widthToken);
        el.UnregisterPropertyChangedCallback(wux::FrameworkElement::HeightProperty(),
                                             i.heightToken);
        RestoreLocalValue(el, wux::FrameworkElement::WidthProperty(), i.originalWidth);
        RestoreLocalValue(el, wux::FrameworkElement::HeightProperty(), i.originalHeight);
    } catch (...) {}
}

void RestoreIconImages() {
    for (auto& i : *g_iconImages) ReleaseIconImage(i);
    g_iconImages->clear();
}

struct Icon {
    wux::FrameworkElement element{nullptr};
    float left = 0;
    float top = 0;
    float width = 0;
    float height = 0;
    float slotLeft = 0, slotWidth = 0;
};

std::vector<Icon> GetIcons(wux::UIElement const& parent) {
    std::vector<Icon> icons;
    for (auto& b : *g_buttons) {
        auto other = b.element.get();
        if (!other || other.Visibility() != wux::Visibility::Visible ||
            other.ActualWidth() <= 0) {
            continue;
        }
        if (wuxm::VisualTreeHelper::GetParent(other) != parent) continue;
        auto slot = wuxc::Primitives::LayoutInformation::GetLayoutSlot(other);
        auto margin = other.Margin();
        float x = slot.X + (float)margin.Left;
        float y = slot.Y + (float)margin.Top;
        if (y < -other.ActualHeight() || y > other.ActualHeight()) continue;
        icons.push_back({other, x, y, (float)other.ActualWidth(), (float)other.ActualHeight(),
                         slot.X, slot.Width});
    }
    return icons;
}

constexpr float kMaxBlur = 60;

enum DOCK_EFFECT_PROPERTY_MAPPING : int {
    DOCK_EFFECT_PROPERTY_MAPPING_UNKNOWN = 0,
    DOCK_EFFECT_PROPERTY_MAPPING_DIRECT = 1,
};

MIDL_INTERFACE("2FC57384-A068-44D7-A331-30982FCF7177")
IDockGraphicsEffectD2D1Interop : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, DOCK_EFFECT_PROPERTY_MAPPING* mapping) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) = 0;
};
__CRT_UUID_DECL(IDockGraphicsEffectD2D1Interop, 0x2fc57384, 0xa068, 0x44d7, 0xa3, 0x31, 0x30,
                0x98, 0x2f, 0xcf, 0x71, 0x77)

constexpr GUID kGaussianBlurId = {
    0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

class GaussianBlurEffect
    : public winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect,
                               wge::IGraphicsEffectSource, IDockGraphicsEffectD2D1Interop> {
public:
    GaussianBlurEffect(float amount, wge::IGraphicsEffectSource const& source)
        : m_amount(amount), m_source(source) {}

    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring const& name) { m_name = name; }

    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) override {
        *id = kGaussianBlurId;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, DOCK_EFFECT_PROPERTY_MAPPING* mapping) override {
        if (_wcsicmp(name, L"BlurAmount") != 0) return E_INVALIDARG;
        *index = 0;
        *mapping = DOCK_EFFECT_PROPERTY_MAPPING_DIRECT;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) override {
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) override {
        try {
            wf::IInspectable v;
            switch (index) {
                case 0: v = wf::PropertyValue::CreateSingle(m_amount); break;
                case 1: v = wf::PropertyValue::CreateUInt32(1); break;
                case 2: v = wf::PropertyValue::CreateUInt32(1); break;
                default: return E_INVALIDARG;
            }
            *value = winrt::detach_abi(v.as<wf::IPropertyValue>());
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) override {
        if (index != 0) return E_INVALIDARG;
        winrt::copy_to_abi(m_source, *source);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) override {
        *count = 1;
        return S_OK;
    }

private:
    float m_amount;
    wge::IGraphicsEffectSource m_source;
    winrt::hstring m_name;
};

wuc::CompositionRoundedRectangleGeometry SetBlurVisual(wux::UIElement const& host, float amount) {
    if (amount <= 0) {
        wuxh::ElementCompositionPreview::SetElementChildVisual(host, nullptr);
        return nullptr;
    }
    auto hostVisual = wuxh::ElementCompositionPreview::GetElementVisual(host);
    auto compositor = hostVisual.Compositor();

    auto effect = winrt::make<GaussianBlurEffect>(
        amount, wuc::CompositionEffectSourceParameter(L"Backdrop"));
    auto brush = compositor.CreateEffectFactory(effect).CreateBrush();
    brush.SetSourceParameter(L"Backdrop", compositor.CreateBackdropBrush());

    auto sprite = compositor.CreateSpriteVisual();
    sprite.Brush(brush);

    auto size = compositor.CreateExpressionAnimation(L"host.Size");
    size.SetReferenceParameter(L"host", hostVisual);
    sprite.StartAnimation(L"Size", size);
    auto shape = compositor.CreateRoundedRectangleGeometry();
    shape.CornerRadius({g_dockRadius, g_dockRadius});
    shape.StartAnimation(L"Size", size);
    sprite.Clip(compositor.CreateGeometricClip(shape));

    wuxh::ElementCompositionPreview::SetElementChildVisual(host, sprite);
    return shape;
}

struct Plate {
    winrt::weak_ref<wux::UIElement> row;
    winrt::weak_ref<wuxc::Panel> panel;
    wuxc::Grid root{nullptr};
    wuxc::Border blurHost{nullptr};
    wuxc::Border tint{nullptr};
    winrt::weak_ref<wux::FrameworkElement> frame;
    wuc::CompositionRoundedRectangleGeometry blurShape{nullptr};
    float appliedBlur = -1;
    float baseLeft = 0, baseWidth = 0;
    float top = 0;
    float left = -1, width = -1;
    float offsetY = 0;
    float dockBottom = 0;
    wux::FrameworkElement::SizeChanged_revoker rowSizeChanged;
    wux::FrameworkElement::SizeChanged_revoker panelSizeChanged;
    wux::FrameworkElement::ActualThemeChanged_revoker panelThemeChanged;
    wuxc::Grid trash{nullptr};
    wuxc::Image trashImage{nullptr};
    wuxc::Border divider{nullptr};
    wux::UIElement::Tapped_revoker trashTapped;
    wux::UIElement::RightTapped_revoker trashRightTapped;
    wux::UIElement::DragEnter_revoker trashDragEnter;
    wux::UIElement::DragOver_revoker trashDragOver;
    wux::UIElement::Drop_revoker trashDrop;
    winrt::weak_ref<wux::FrameworkElement> beforeTrash;
    float trashCenter = 0;
    int trashState = -2;
    wuxc::Border appsDivider{nullptr};
    winrt::weak_ref<wux::FrameworkElement> beforeAppsDivider;
    float appsDividerShift = 0;
};

[[clang::no_destroy]] std::optional<std::list<Plate>> g_plates{std::in_place};

Plate* GetRowPlate(wux::DependencyObject const& row) {
    if (!row) return nullptr;
    for (auto& p : *g_plates) {
        if (p.row.get() == row) return &p;
    }
    return nullptr;
}

bool IsFileExplorer(wux::FrameworkElement const& button) {
    return wux::Automation::AutomationProperties::GetAutomationId(button) ==
           L"Appid: Microsoft.Windows.Explorer";
}

wux::FrameworkElement AppsDividerButton(std::vector<wux::FrameworkElement> const& buttons) {
    size_t i = 0;
    while (i < buttons.size() && winrt::get_class_name(buttons[i]) != L"Taskbar.TaskListButton") {
        i++;
    }
    if (i < buttons.size() && IsFileExplorer(buttons[i])) i++;
    if (i == 0 || i >= buttons.size()) return nullptr;
    return buttons[i - 1];
}

void SetAppsDividerShift(Plate& p, float tx) {
    if (!p.appsDivider || std::fabs(tx - p.appsDividerShift) < 0.01f) return;
    p.appsDividerShift = tx;
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(p.appsDivider);
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(p.appsDivider, true);
    visual.Properties().InsertVector3(L"Translation", {tx, 0, 0});
}

float GetRowOffsetY(wux::DependencyObject const& row) {
    Plate* p = GetRowPlate(row);
    return p ? p->offsetY : 0;
}

int g_platesDirtyFrames = 0;

void StartFrames();

void MarkPlatesDirty() {
    g_platesDirtyFrames = 3;
    StartFrames();
}

void ApplyPlateLook(Plate& p) {
    bool light = g_dockTheme == 2;
    if (g_dockTheme == 0) {
        auto panel = p.panel.get();
        light = panel && panel.ActualTheme() == wux::ElementTheme::Light;
    }
    using winrt::Windows::UI::Color;
    uint8_t alpha = (uint8_t)(g_dockTint * 255 / 100);
    Color tint = light ? Color{alpha, 245, 245, 247} : Color{alpha, 60, 60, 65};
    Color border = light ? Color{30, 0, 0, 0} : Color{40, 255, 255, 255};
    Color line = light ? Color{70, 0, 0, 0} : Color{90, 255, 255, 255};
    for (auto& divider : {p.divider, p.appsDivider}) {
        if (divider) divider.Background(wuxm::SolidColorBrush(line));
    }
    p.tint.Background(wuxm::SolidColorBrush(tint));
    p.tint.BorderBrush(wuxm::SolidColorBrush(border));
    p.tint.BorderThickness({1, 1, 1, 1});
    p.tint.CornerRadius({g_dockRadius, g_dockRadius, g_dockRadius, g_dockRadius});

    float blur = g_dockBlur * kMaxBlur / 100;
    if (blur != p.appliedBlur) {
        p.appliedBlur = blur;
        try {
            p.blurShape = SetBlurVisual(p.blurHost, blur);
        } catch (...) {
            p.blurShape = nullptr;
            Wh_Log(L"Dock blur failed: 0x%08X %s", winrt::to_hresult(),
                   winrt::to_message().c_str());
        }
    } else if (p.blurShape) {
        p.blurShape.CornerRadius({g_dockRadius, g_dockRadius});
    }
}

void StartTrash();
void UpdateTrashIcons();
void HookUpTrash(Plate& p);

void PlaceInPanel(wuxc::Panel const& panel, wux::FrameworkElement const& el) {
    el.HorizontalAlignment(wux::HorizontalAlignment::Left);
    el.VerticalAlignment(wux::VerticalAlignment::Top);
    el.Visibility(wux::Visibility::Collapsed);
    if (panel.try_as<wuxc::Grid>()) {
        wuxc::Grid::SetRowSpan(el, 100);
        wuxc::Grid::SetColumnSpan(el, 100);
    }
}

wuxc::Border AddDivider(wuxc::Panel const& panel, wux::UIElement const& row) {
    wuxc::Border divider;
    PlaceInPanel(panel, divider);
    divider.IsHitTestVisible(false);
    divider.Width(1);
    divider.Height(g_iconSize);
    uint32_t index = 0;
    if (!panel.Children().IndexOf(row, index)) return nullptr;
    panel.Children().InsertAt(index + 1, divider);
    return divider;
}

void AddTrash(Plate& p, wuxc::Panel const& panel, wux::UIElement const& row) {
    wuxc::Grid trash;
    PlaceInPanel(panel, trash);
    trash.Width(g_iconSize * kTrashIconScale);
    trash.Height(g_iconSize * kTrashIconScale);
    trash.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
    wuxc::ToolTipService::SetToolTip(trash, winrt::box_value(L"Recycle Bin"));
    wuxc::Image image;
    image.Stretch(wuxm::Stretch::Uniform);
    trash.Children().Append(image);

    uint32_t index = 0;
    if (!panel.Children().IndexOf(row, index)) return;
    panel.Children().InsertAt(index + 1, trash);
    auto divider = AddDivider(panel, row);

    p.trash = trash;
    p.trashImage = image;
    p.divider = divider;
    HookUpTrash(p);
    StartTrash();
}

void EnsurePlate(wux::FrameworkElement const& button) {
    if (!FindTaskbarFrame(button)) return;
    auto row = wuxm::VisualTreeHelper::GetParent(button).try_as<wux::FrameworkElement>();
    if (!row) return;
    for (auto& p : *g_plates) {
        if (p.row.get() == row) return;
    }
    auto panel = wuxm::VisualTreeHelper::GetParent(row).try_as<wuxc::Panel>();
    if (!panel) {
        Wh_Log(L"The icon row's parent isn't a panel; no dock plate");
        return;
    }

    wuxc::Grid root;
    PlaceInPanel(panel, root);
    root.IsHitTestVisible(false);
    wuxc::Border blurHost;
    wuxc::Border tint;
    root.Children().Append(blurHost);
    root.Children().Append(tint);
    for (wux::FrameworkElement el : {wux::FrameworkElement(root), wux::FrameworkElement(blurHost),
                                     wux::FrameworkElement(tint)}) {
        el.UseLayoutRounding(false);
    }

    uint32_t index = 0;
    if (!panel.Children().IndexOf(row, index)) index = 0;
    panel.Children().InsertAt(index, root);

    auto& p = g_plates->emplace_back();
    p.row = winrt::make_weak(row.as<wux::UIElement>());
    p.panel = winrt::make_weak(panel);
    p.root = root;
    p.blurHost = blurHost;
    p.tint = tint;

    auto onSize = [](wf::IInspectable const&, wux::SizeChangedEventArgs const&) {
        MarkPlatesDirty();
    };
    p.rowSizeChanged = row.SizeChanged(winrt::auto_revoke, onSize);
    p.panelSizeChanged = panel.SizeChanged(winrt::auto_revoke, onSize);
    if (g_showTrash) AddTrash(p, panel, row);
    if (g_appsDivider) p.appsDivider = AddDivider(panel, row);
    ApplyPlateLook(p);
    p.panelThemeChanged = panel.ActualThemeChanged(
        winrt::auto_revoke, [](wux::FrameworkElement const&, wf::IInspectable const&) {
            for (auto& plate : *g_plates) {
                try { ApplyPlateLook(plate); } catch (...) {}
            }
        });
    Wh_Log(L"Dock plate added");
}

void ReapplyButton(wux::FrameworkElement const& button) {
    float s = 1.0f, tx = 0;
    for (auto& item : g_row) {
        if (item.element.get() == button) {
            s = item.scale;
            tx = item.shift;
            break;
        }
    }
    SetScale(button, s, tx);
}

std::pair<wux::FrameworkElement, wux::FrameworkElement> LayOutButtons(
    Plate& p, std::vector<Icon> const& icons) {
    std::vector<size_t> order(icons.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](size_t a, size_t b) {
        if (icons[a].slotLeft != icons[b].slotLeft) return icons[a].slotLeft < icons[b].slotLeft;
        return icons[a].slotWidth < icons[b].slotWidth;
    });

    constexpr size_t kNone = SIZE_MAX;
    size_t firstApp = kNone, explorer = kNone;
    for (size_t i : order) {
        if (winrt::get_class_name(icons[i].element) != L"Taskbar.TaskListButton") continue;
        if (firstApp == kNone) firstApp = i;
        if (explorer == kNone && IsFileExplorer(icons[i].element)) explorer = i;
    }
    bool moveExplorer = explorer != kNone && explorer != firstApp;

    std::vector<float> left(icons.size()), right(icons.size());
    size_t last = order.back();
    if (moveExplorer && last == explorer) last = order[order.size() - 2];
    if (p.trash) right[last] += g_trashSpace;

    wux::FrameworkElement beforeAppsDivider{nullptr};
    float dividerSpace = p.appsDivider ? g_dividerSpace : 0;
    if (moveExplorer) {
        auto& e = icons[explorer];
        float pitch = g_buttonWidth + 2 * g_buttonMargin;
        left[firstApp] += pitch + dividerSpace;
        left[explorer] = icons[firstApp].slotLeft - e.slotLeft;
        right[explorer] = -pitch - left[explorer];
        beforeAppsDivider = e.element;
    } else if (p.appsDivider) {
        std::vector<wux::FrameworkElement> buttons;
        for (size_t i : order) buttons.push_back(icons[i].element);
        beforeAppsDivider = AppsDividerButton(buttons);
        for (size_t i = 0; i < icons.size(); i++) {
            if (icons[i].element == beforeAppsDivider) right[i] += g_dividerSpace;
        }
    }

    bool changed = false;
    for (size_t i = 0; i < icons.size(); i++) {
        Button* b = FindButton(icons[i].element);
        if (!b || (std::fabs(b->extraLeft - left[i]) < 0.5f &&
                   std::fabs(b->extraRight - right[i]) < 0.5f)) {
            continue;
        }
        b->extraLeft = left[i];
        b->extraRight = right[i];
        ApplySpacing(icons[i].element);
        changed = true;
    }
    if (changed) MarkPlatesDirty();
    return {icons[last].element, beforeAppsDivider};
}

void MeasurePlate(Plate& p) {
    auto row = p.row.get();
    auto panel = p.panel.get();
    if (!row || !panel || !p.root) return;

    if (!p.frame.get()) {
        if (auto frame = FindTaskbarFrame(row.as<wux::FrameworkElement>())) {
            p.frame = winrt::make_weak(frame);
        }
    }

    auto icons = GetIcons(row);
    if (icons.empty()) {
        for (wux::UIElement el : {wux::UIElement(p.root), wux::UIElement(p.trash),
                                  wux::UIElement(p.divider), wux::UIElement(p.appsDivider)}) {
            if (el) el.Visibility(wux::Visibility::Collapsed);
        }
        return;
    }
    std::sort(icons.begin(), icons.end(),
              [](Icon const& a, Icon const& b) { return a.left < b.left; });

    float minLeft = icons[0].left, maxRight = icons[0].left + icons[0].width;
    float minTop = icons[0].top, maxBottom = icons[0].top + icons[0].height;
    for (auto& icon : icons) {
        minLeft = std::min(minLeft, icon.left);
        maxRight = std::max(maxRight, icon.left + icon.width);
        minTop = std::min(minTop, icon.top);
        maxBottom = std::max(maxBottom, icon.top + icon.height);
    }

    float avgWidth = 0;
    for (auto& icon : icons) avgWidth += icon.width;
    avgWidth /= icons.size();
    float emptyPerSide = std::max(0.0f, (avgWidth - g_iconSize) / 2);
    float padX = g_dockSidePadding - emptyPerSide;
    auto origin = row.TransformToVisual(panel).TransformPoint({0, 0});
    p.baseLeft = origin.X + minLeft - padX;
    p.baseWidth = (maxRight - minLeft) + 2 * padX;

    float height = g_dockHeight;
    p.top = (float)panel.ActualHeight() - g_dockBottomGap - height;
    float iconCenterY = origin.Y + (minTop + maxBottom) / 2;
    float offsetY = p.top + height / 2 - iconCenterY;

    double current = p.root.Height();
    if (std::isnan(current) || std::fabs(current - height) > 0.5) p.root.Height(height);
    p.left = p.width = -1;
    p.root.Visibility(wux::Visibility::Visible);

    auto [beforeTrash, beforeAppsDivider] = LayOutButtons(p, icons);
    p.beforeTrash = winrt::make_weak(beforeTrash);
    p.beforeAppsDivider = beforeAppsDivider ? winrt::make_weak(beforeAppsDivider) : nullptr;
    auto iconOf = [&](wux::FrameworkElement const& el) -> Icon const& {
        for (auto& icon : icons) {
            if (icon.element == el) return icon;
        }
        return icons.back();
    };

    float centerY = p.top + height / 2;
    auto placeDivider = [&](wuxc::Border const& divider, Icon const& icon) {
        float x = icon.left + icon.width + g_buttonMargin + g_dividerSpace / 2;
        divider.Margin({origin.X + x - 0.5f, centerY - g_iconSize / 2, 0, 0});
        divider.Visibility(wux::Visibility::Visible);
    };

    if (p.trash) {
        auto& last = iconOf(beforeTrash);
        p.trashCenter = last.left + last.width + g_buttonMargin + g_dividerSpace + g_trashSlot / 2;
        p.baseWidth += g_trashSpace;

        float trashSize = g_iconSize * kTrashIconScale;
        p.trash.Margin({origin.X + p.trashCenter - trashSize / 2, centerY - trashSize / 2, 0, 0});
        p.trash.Visibility(wux::Visibility::Visible);
        placeDivider(p.divider, last);
        UpdateTrashIcons();
    }

    if (p.appsDivider) {
        p.appsDivider.Visibility(wux::Visibility::Collapsed);
        if (beforeAppsDivider) placeDivider(p.appsDivider, iconOf(beforeAppsDivider));
    }

    p.offsetY = offsetY;
    p.dockBottom = p.top + height - origin.Y;
    for (auto& icon : icons) ReapplyButton(icon.element);
}

void ApplyPlate(Plate& p, float extraLeft, float extra) {
    if (!p.root) return;
    float left = p.baseLeft - extraLeft;
    float width = p.baseWidth + extra;
    if (std::fabs(left - p.left) > 0.25f || std::fabs(width - p.width) > 0.25f) {
        p.root.Margin({left, p.top, 0, 0});
        p.root.Width(width);
        p.left = left;
        p.width = width;
    }
}

void RemovePlates() {
    for (auto& p : *g_plates) {
        p.rowSizeChanged.revoke();
        p.panelSizeChanged.revoke();
        p.panelThemeChanged.revoke();
        if (p.blurHost) {
            try { wuxh::ElementCompositionPreview::SetElementChildVisual(p.blurHost, nullptr); }
            catch (...) {}
        }
        p.trashTapped.revoke();
        p.trashRightTapped.revoke();
        p.trashDragEnter.revoke();
        p.trashDragOver.revoke();
        p.trashDrop.revoke();
        auto panel = p.panel.get();
        if (!panel) continue;
        for (wux::UIElement el : {wux::UIElement(p.root), wux::UIElement(p.trash),
                                  wux::UIElement(p.divider), wux::UIElement(p.appsDivider)}) {
            uint32_t index;
            if (el && panel.Children().IndexOf(el, index)) panel.Children().RemoveAt(index);
        }
    }
    g_plates->clear();
}

MIDL_INTERFACE("905a0fef-bc53-11df-8c49-001e4fc686da")
IDockBufferByteAccess : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE Buffer(BYTE** value) = 0;
};
__CRT_UUID_DECL(IDockBufferByteAccess, 0x905a0fef, 0xbc53, 0x11df, 0x8c, 0x49, 0x00, 0x1e, 0x4f,
                0xc6, 0x86, 0xda)

wuxm::ImageSource LoadStockTrashIcon(bool full, int px) {
    SHSTOCKICONINFO info{sizeof(info)};
    HICON icon = nullptr;
    if (FAILED(SHGetStockIconInfo(full ? SIID_RECYCLERFULL : SIID_RECYCLER, SHGSI_ICONLOCATION,
                                  &info)) ||
        SHDefExtractIconW(info.szPath, info.iIcon, 0, &icon, nullptr, px) != S_OK || !icon) {
        Wh_Log(L"[trash] couldn't load Windows' icon");
        return nullptr;
    }

    ICONINFO ii{};
    BITMAP bm{};
    std::vector<BYTE> pixels;
    if (GetIconInfo(icon, &ii)) {
        if (ii.hbmColor && GetObject(ii.hbmColor, sizeof(bm), &bm)) {
            BITMAPINFO bi{};
            bi.bmiHeader = {sizeof(BITMAPINFOHEADER), bm.bmWidth, -bm.bmHeight, 1, 32, BI_RGB};
            pixels.resize((size_t)bm.bmWidth * bm.bmHeight * 4);
            HDC dc = GetDC(nullptr);
            if (GetDIBits(dc, ii.hbmColor, 0, bm.bmHeight, pixels.data(), &bi, DIB_RGB_COLORS) !=
                bm.bmHeight) {
                pixels.clear();
            }
            ReleaseDC(nullptr, dc);
        }
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
        if (ii.hbmMask) DeleteObject(ii.hbmMask);
    }
    DestroyIcon(icon);
    if (pixels.empty()) return nullptr;

    for (size_t i = 0; i < pixels.size(); i += 4) {
        BYTE a = pixels[i + 3];
        for (int c = 0; c < 3; c++) pixels[i + c] = (BYTE)(pixels[i + c] * a / 255);
    }
    wuxm::Imaging::WriteableBitmap bitmap(bm.bmWidth, bm.bmHeight);
    BYTE* data = nullptr;
    winrt::check_hresult(bitmap.PixelBuffer().as<IDockBufferByteAccess>()->Buffer(&data));
    memcpy(data, pixels.data(), pixels.size());
    bitmap.Invalidate();
    return bitmap;
}

[[clang::no_destroy]] wuxm::ImageSource g_trashIcons[2] = {nullptr, nullptr};

wuxm::ImageSource TrashIcon(bool full, wux::UIElement const& el) {
    auto& icon = g_trashIcons[full];
    if (icon) return icon;
    double dpiScale = 1;
    try {
        if (auto root = el.XamlRoot()) dpiScale = root.RasterizationScale();
    } catch (...) {}
    return icon = LoadStockTrashIcon(
               full, (int)std::ceil(g_iconLoadSize * kTrashIconScale * dpiScale));
}

std::atomic<int> g_trashFull = -1;
std::atomic<bool> g_trashChecking = false;
PTP_WORK g_trashWork = nullptr;
ULONG g_trashNotify = 0;

void CALLBACK CheckRecycleBin(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK) {
    HRESULT co = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    SHQUERYRBINFO info{sizeof(info)};
    if (SUCCEEDED(SHQueryRecycleBinW(nullptr, &info))) g_trashFull = info.i64NumItems > 0;
    if (SUCCEEDED(co)) CoUninitialize();
    g_trashChecking = false;
    if (HWND w = g_modWindow) PostMessage(w, kTrashCheckedMessage, 0, 0);
}

void CheckRecycleBinSoon() {
    if (g_trashWork && !g_trashChecking.exchange(true)) SubmitThreadpoolWork(g_trashWork);
}

void UpdateTrashIcons() {
    int full = g_trashFull;
    for (auto& p : *g_plates) {
        if (!p.trashImage || p.trashState == full) continue;
        p.trashState = full;
        try {
            p.trashImage.Source(TrashIcon(full == 1, p.trashImage));
        } catch (...) {
            Wh_Log(L"[trash] icon failed: 0x%08X", winrt::to_hresult());
        }
    }
}

void StartTrash() {
    if (!g_trashWork) g_trashWork = CreateThreadpoolWork(CheckRecycleBin, nullptr, nullptr);
    HWND w = EnsureModWindow();
    CheckRecycleBinSoon();
    if (g_trashNotify) return;
    PIDLIST_ABSOLUTE bin;
    if (!w || FAILED(SHGetFolderLocation(nullptr, CSIDL_BITBUCKET, nullptr, 0, &bin))) return;
    SHChangeNotifyEntry entry{bin, TRUE};
    g_trashNotify = SHChangeNotifyRegister(w, SHCNRF_ShellLevel, SHCNE_ALLEVENTS,
                                           kTrashChangedMessage, 1, &entry);
    CoTaskMemFree(bin);
    if (!g_trashNotify) Wh_Log(L"[trash] can't watch the Recycle Bin for changes");
}

void StopTrash() {
    if (g_trashNotify) {
        SHChangeNotifyDeregister(g_trashNotify);
        g_trashNotify = 0;
    }
    g_trashIcons[0] = g_trashIcons[1] = nullptr;
}

void OpenRecycleBin() {
    ShellExecuteW(nullptr, L"open", L"shell:RecycleBinFolder", nullptr, nullptr, SW_SHOWNORMAL);
}

void RecycleFiles(std::wstring const& paths) {
    SHFILEOPSTRUCTW op{};
    op.wFunc = FO_DELETE;
    op.pFrom = paths.c_str();
    op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_WANTNUKEWARNING;
    if (int error = SHFileOperationW(&op)) Wh_Log(L"[trash] recycling failed: %d", error);
    CheckRecycleBinSoon();
}

void EmptyRecycleBin() {
    SHEmptyRecycleBinW(nullptr, nullptr, 0);
    CheckRecycleBinSoon();
}

void ShowTrashMenu(wux::FrameworkElement const& target) {
    wuxc::MenuFlyoutItem open;
    open.Text(L"Open");
    open.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) { OpenRecycleBin(); });
    wuxc::MenuFlyoutItem empty;
    empty.Text(L"Empty Recycle Bin");
    empty.IsEnabled(g_trashFull != 0);
    empty.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        QueueShellJob(EmptyRecycleBin);
    });
    wuxc::MenuFlyout menu;
    menu.Items().Append(open);
    menu.Items().Append(empty);
    menu.ShowAt(target);
}

void AcceptTrashDrag(wux::DragEventArgs const& e) {
    if (!e.DataView().Contains(wadt::StandardDataFormats::StorageItems())) return;
    e.AcceptedOperation(wadt::DataPackageOperation::Move);
    if (auto ui = e.DragUIOverride()) ui.Caption(L"Move to Recycle Bin");
    e.Handled(true);
}

void OnTrashDrop(wux::DragEventArgs const& e) {
    e.Handled(true);
    e.AcceptedOperation(wadt::DataPackageOperation::None);
    if (!e.DataView().Contains(wadt::StandardDataFormats::StorageItems())) return;
    QueueShellJob([items = winrt::make_agile(e.DataView().GetStorageItemsAsync())] {
        std::wstring paths;
        try {
            for (auto const& item : items.get().get()) {
                auto path = item.Path();
                if (path.empty()) continue;
                paths.append(path.c_str(), path.size());
                paths.push_back(L'\0');
            }
        } catch (...) {
            Wh_Log(L"[trash] reading the dropped files failed: 0x%08X", winrt::to_hresult());
            return;
        }
        if (!paths.empty()) RecycleFiles(paths);
    });
}

void HookUpTrash(Plate& p) {
    p.trash.AllowDrop(true);
    p.trashTapped = p.trash.Tapped(
        winrt::auto_revoke, [](wf::IInspectable const&, wuxi::TappedRoutedEventArgs const& e) {
            e.Handled(true);
            OpenRecycleBin();
        });
    p.trashRightTapped = p.trash.RightTapped(
        winrt::auto_revoke,
        [](wf::IInspectable const& sender, wuxi::RightTappedRoutedEventArgs const& e) {
            e.Handled(true);
            try { ShowTrashMenu(sender.as<wux::FrameworkElement>()); } catch (...) {
                Wh_Log(L"[trash] menu failed: 0x%08X", winrt::to_hresult());
            }
        });
    auto accept = [](wf::IInspectable const&, wux::DragEventArgs const& e) {
        try { AcceptTrashDrag(e); } catch (...) {}
    };
    p.trashDragEnter = p.trash.DragEnter(winrt::auto_revoke, accept);
    p.trashDragOver = p.trash.DragOver(winrt::auto_revoke, accept);
    p.trashDrop = p.trash.Drop(
        winrt::auto_revoke, [](wf::IInspectable const&, wux::DragEventArgs const& e) {
            try { OnTrashDrop(e); } catch (...) {
                Wh_Log(L"[trash] drop failed: 0x%08X", winrt::to_hresult());
            }
        });
}

void SetTrashScale(Plate& p, float s, float tx, float dividerTx) {
    if (!p.trash) return;
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(p.trash);
    float size = g_iconSize * kTrashIconScale;
    visual.CenterPoint({size / 2, size / 2 + g_iconSize / 2, 0});
    visual.Scale({s, s, 1});
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(p.trash, true);
    visual.Properties().InsertVector3(L"Translation", {tx, 0, 0});
    auto divider = wuxh::ElementCompositionPreview::GetElementVisual(p.divider);
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(p.divider, true);
    divider.Properties().InsertVector3(L"Translation", {dividerTx, 0, 0});
}

struct Indicator {
    winrt::weak_ref<wux::FrameworkElement> element;
    winrt::weak_ref<wux::FrameworkElement> button;
    wux::FrameworkElement::SizeChanged_revoker sizeChanged;
};

[[clang::no_destroy]] std::optional<std::list<Indicator>> g_indicators{std::in_place};

void PlaceIndicator(wux::FrameworkElement const& button, float s) {
    wux::FrameworkElement el{nullptr};
    for (auto& i : *g_indicators) {
        if (i.button.get() == button) {
            el = i.element.get();
            break;
        }
    }
    if (!el) return;
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, true);

    auto reset = [&] {
        visual.Scale({1, 1, 1});
        visual.Properties().InsertVector3(L"Translation", {0, 0, 0});
    };
    Plate* p = GetRowPlate(wuxm::VisualTreeHelper::GetParent(button));
    if (!p || s <= 0) return reset();

    float w = (float)el.ActualWidth(), h = (float)el.ActualHeight();
    float centerY = h / 2;
    wux::DependencyObject current = el;
    while (current && current != button) {
        if (auto ui = current.try_as<wux::UIElement>()) centerY += ui.ActualOffset().y;
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    if (!current) return reset();

    auto slot = wuxc::Primitives::LayoutInformation::GetLayoutSlot(button);
    float buttonTop = slot.Y + (float)button.Margin().Top;
    float inset = std::max(2.0f, std::round(g_iconSize / 10));
    float targetY = p->dockBottom - buttonTop - inset - h / 2;

    float pivotY = ButtonPivotY(button);
    float ty = (targetY - p->offsetY - pivotY) / s + pivotY - centerY;
    visual.CenterPoint({w / 2, h / 2, 0});
    visual.Scale({1 / s, 1 / s, 1});
    visual.Properties().InsertVector3(L"Translation", {0, ty, 0});
}

void TrackIndicator(wux::FrameworkElement const& el, wux::FrameworkElement const& button) {
    for (auto& i : *g_indicators) {
        if (i.element.get() == el) return;
    }
    auto& i = g_indicators->emplace_back();
    i.element = winrt::make_weak(el);
    i.button = winrt::make_weak(button);
    i.sizeChanged = el.SizeChanged(
        winrt::auto_revoke, [weakButton = i.button](wf::IInspectable const&,
                                                    wux::SizeChangedEventArgs const&) {
            try {
                if (auto b = weakButton.get()) ReapplyButton(b);
            } catch (...) {}
        });
}

void RestoreIndicators() {
    for (auto& i : *g_indicators) {
        i.sizeChanged.revoke();
        if (auto el = i.element.get()) {
            try {
                auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
                visual.Scale({1, 1, 1});
                visual.Properties().InsertVector3(L"Translation", {0, 0, 0});
                wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, false);
            } catch (...) {}
        }
    }
    g_indicators->clear();
}

void BuildRow(wux::UIElement const& parent) {
    std::vector<RowItem> old = std::move(g_row);
    Plate* oldPlate = GetRowPlate(g_rowParent.get());
    if (g_rowParent.get() != parent) g_maxGrowth = g_lastGrowth = 0;
    g_row.clear();
    g_rowParent = nullptr;

    if (parent) {
        for (auto& icon : GetIcons(parent)) {
            float pitch = std::max(1.0f, icon.width + 2 * g_buttonMargin);
            RowItem item{winrt::make_weak(icon.element), icon.left + icon.width / 2, pitch};
            for (auto& o : old) {
                if (o.element.get() == icon.element) {
                    item.scale = o.scale;
                    item.shift = o.shift;
                    o.element = nullptr;
                    break;
                }
            }
            g_row.push_back(item);
        }
        std::sort(g_row.begin(), g_row.end(),
                  [](RowItem const& a, RowItem const& b) { return a.center < b.center; });
        g_rowParent = winrt::make_weak(parent);
        Plate* p = GetRowPlate(parent);

        if (p && p->trash && !g_row.empty() &&
            p->trash.Visibility() == wux::Visibility::Visible) {
            RowItem item{nullptr, p->trashCenter, g_trashSlot};
            item.trash = true;
            for (auto& o : old) {
                if (o.trash && oldPlate == p) {
                    item.scale = o.scale;
                    item.shift = o.shift;
                    o.trash = false;
                }
            }
            g_row.push_back(item);
        }
    }

    for (auto& o : old) {
        if (auto icon = o.element.get()) SetScale(icon, 1.0f);
        if (o.trash && oldPlate) SetTrashScale(*oldPlate, 1.0f, 0, 0);
    }
    if (oldPlate && oldPlate != GetRowPlate(parent)) SetAppsDividerShift(*oldPlate, 0);
}

winrt::event_token g_frameToken{};
bool g_frameRunning = false;
std::chrono::steady_clock::time_point g_lastFrame;

void ScanTaskbars();

void StopFrames() {
    if (!g_frameRunning) return;
    wuxm::CompositionTarget::Rendering(g_frameToken);
    g_frameRunning = false;
}

void OnFrame() {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - g_lastFrame).count();
    g_lastFrame = now;
    if (dt > 0.1f) dt = 0.1f;

    float alpha = 1.0f;
    if (g_animationMs > 0) {
        float tau = g_animationMs / 1000.0f / 3.0f;
        alpha = 1.0f - std::exp(-dt / tau);
    }

    if (g_platesDirtyFrames > 0) {
        g_platesDirtyFrames--;
        try { ScanTaskbars(); } catch (...) {
            Wh_Log(L"ScanTaskbars failed: 0x%08X", winrt::to_hresult());
        }
        for (auto& b : *g_buttons) {
            if (auto el = b.element.get()) {
                try { EnsurePlate(el); } catch (...) {
                    Wh_Log(L"EnsurePlate failed: 0x%08X", winrt::to_hresult());
                }
            }
        }
        for (auto& p : *g_plates) MeasurePlate(p);
        ScheduleOverWindows(false);
    }

    if (auto rowParent = g_rowParent.get()) BuildRow(rowParent);

    bool settled = g_platesDirtyFrames == 0;
    std::vector<float> scales(g_row.size(), 1.0f);
    for (size_t i = 0; i < g_row.size(); i++) {
        auto& item = g_row[i];

        float target = 1.0f;
        if (g_pointerOver) {
            float d = std::fabs(g_pointerX - item.center);
            if (d < g_radius) {
                float k = (std::cos(d / g_radius * 3.14159265f) + 1.0f) / 2.0f;
                target = 1.0f + (g_scale - 1.0f) * k;
            }
        }

        float s = item.scale + (target - item.scale) * alpha;
        if (std::fabs(target - s) < 0.001f) {
            s = target;
        } else {
            settled = false;
        }
        scales[i] = s;
    }

    bool expanded = g_pointerOver;
    for (float scale : scales) expanded |= scale > 1.0f;
    if (expanded != g_dockExpanded) {
        g_dockExpanded = expanded;
        ScheduleOverWindows(false);
    }

    float total = 0;
    for (size_t i = 0; i < g_row.size(); i++) {
        total += (scales[i] - 1.0f) * g_row[i].width;
    }
    if (g_pointerOver) {
        if (!g_row.empty()) {
            float middle = (g_row.front().center - g_row.front().width / 2 +
                            g_row.back().center + g_row.back().width / 2) / 2;
            g_anchorLeftEnd = g_pointerX > middle;
        }
        g_maxGrowth = std::max(g_maxGrowth, total);
    } else if (g_lastGrowth > 0) {
        g_maxGrowth *= total / g_lastGrowth;
    } else {
        g_maxGrowth = total;
    }
    g_lastGrowth = total;
    float growLeft = g_anchorLeftEnd ? g_maxGrowth / 2 : total - g_maxGrowth / 2;

    auto rowParent = g_rowParent.get();
    Plate* plate = GetRowPlate(rowParent);
    auto beforeAppsDivider = plate ? plate->beforeAppsDivider.get() : nullptr;
    float appsDividerShift = 0;
    float before = 0;
    for (size_t i = 0; i < g_row.size(); i++) {
        auto& item = g_row[i];
        float extra = (scales[i] - 1.0f) * item.width;
        float shift = before + extra / 2 - growLeft;
        float gapShift = before - growLeft;
        before += extra;
        if (beforeAppsDivider && item.element.get() == beforeAppsDivider) {
            appsDividerShift = before - growLeft;
        }

        if (item.trash) {
            if (plate && (scales[i] != item.scale || std::fabs(shift - item.shift) > 0.01f)) {
                SetTrashScale(*plate, scales[i], shift, gapShift);
                item.scale = scales[i];
                item.shift = shift;
            }
            continue;
        }

        auto icon = item.element.get();
        if (!icon) continue;
        if (scales[i] != item.scale || std::fabs(shift - item.shift) > 0.01f) {
            SetScale(icon, scales[i], shift);
            item.scale = scales[i];
            item.shift = shift;
        }
    }
    if (plate) SetAppsDividerShift(*plate, appsDividerShift);

    for (auto& p : *g_plates) {
        bool current = rowParent && p.row.get() == rowParent;
        ApplyPlate(p, current ? growLeft : 0, current ? total : 0);
    }

    if (settled) StopFrames();
}

void StartFrames() {
    if (g_frameRunning) return;
    g_lastFrame = std::chrono::steady_clock::now();
    g_frameToken = wuxm::CompositionTarget::Rendering(
        [](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                OnFrame();
            } catch (...) {
                Wh_Log(L"OnFrame error: 0x%08X %s", winrt::to_hresult(),
                       winrt::to_message().c_str());
                StopFrames();
            }
        });
    g_frameRunning = true;
}

Button& OnButtonAdded(wux::FrameworkElement const& el) {
    for (auto& b : *g_buttons) {
        if (b.element.get() == el) return b;
    }
    auto& b = g_buttons->emplace_back();
    b.element = winrt::make_weak(el);
    b.originalMargin = el.ReadLocalValue(wux::FrameworkElement::MarginProperty());
    b.marginToken = el.RegisterPropertyChangedCallback(
        wux::FrameworkElement::MarginProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            try {
                if (auto fe = sender.try_as<wux::FrameworkElement>()) ApplySpacing(fe);
            } catch (...) {}
        });
    b.sizeChanged = el.SizeChanged(
        winrt::auto_revoke,
        [](wf::IInspectable const&, wux::SizeChangedEventArgs const&) { MarkPlatesDirty(); });
    ApplySpacing(el);
    MarkPlatesDirty();
    return b;
}

void ApplyButtonWidth(Button& b, wux::FrameworkElement const& button) {
    auto root = b.root.get();
    if (!root) {
        if (wuxm::VisualTreeHelper::GetChildrenCount(button) == 0) return;
        root = wuxm::VisualTreeHelper::GetChild(button, 0).try_as<wux::FrameworkElement>();
        if (!root) return;
        b.root = winrt::make_weak(root);
        b.originalRootMinWidth = root.ReadLocalValue(wux::FrameworkElement::MinWidthProperty());
    }
    if (root.MinWidth() != g_buttonWidth) root.MinWidth(g_buttonWidth);
}

void ApplyIconColumn(Button& b) {
    auto root = b.root.get();
    auto grid = root ? root.try_as<wuxc::Grid>() : nullptr;
    if (!grid) return;
    auto columns = grid.ColumnDefinitions();
    if (columns.Size() == 0) return;
    auto column = columns.GetAt(0);
    auto width = column.Width();
    if (width.GridUnitType != wux::GridUnitType::Pixel) return;

    if (width.Value != b.appliedIconColumn) b.originalIconColumn = width.Value;
    auto padding = grid.Padding();
    double target = g_buttonWidth - padding.Left - padding.Right;
    if (width.Value != target) column.Width({target, wux::GridUnitType::Pixel});
    b.appliedIconColumn = target;
}

void ReleaseButton(Button& b) {
    b.sizeChanged.revoke();
    auto el = b.element.get();
    if (!el) return;
    if (b.marginToken) {
        el.UnregisterPropertyChangedCallback(wux::FrameworkElement::MarginProperty(),
                                             b.marginToken);
        b.marginToken = 0;
    }
    RestoreLocalValue(el, wux::FrameworkElement::MarginProperty(), b.originalMargin);
    if (auto root = b.root.get()) {
        if (b.originalIconColumn > 0) {
            if (auto grid = root.try_as<wuxc::Grid>(); grid && grid.ColumnDefinitions().Size()) {
                auto column = grid.ColumnDefinitions().GetAt(0);
                if (column.Width().Value == b.appliedIconColumn) {
                    column.Width({b.originalIconColumn, wux::GridUnitType::Pixel});
                }
            }
        }
        RestoreLocalValue(root, wux::FrameworkElement::MinWidthProperty(),
                          b.originalRootMinWidth);
    }
}

void ProcessButton(wux::FrameworkElement const& button) {
    Button& b = OnButtonAdded(button);
    ApplyButtonWidth(b, button);
    ApplyIconColumn(b);
    auto visit = [&](wux::FrameworkElement const& el) {
        auto name = el.Name();
        if (name == L"Icon") {
            TrackIconImage(el);
        } else if (name == L"RunningIndicator") {
            TrackIndicator(el, button);
        } else if ((name == L"BackgroundElement" || name == L"MultiWindowElement") &&
                   el.try_as<wuxc::Border>()) {
            HideHighlight(el);
        }
        return true;
    };
    ForEachDescendant(button, 6, visit);
}

std::vector<winrt::weak_ref<wux::FrameworkElement>> g_frames;

void NoteTaskbarFrame(wux::FrameworkElement const& frame) {
    if (!frame) return;
    for (auto& f : g_frames) {
        if (f.get() == frame) return;
    }
    g_frames.push_back(winrt::make_weak(frame));
    Wh_Log(L"Taskbar found");
    MarkPlatesDirty();
    ScheduleOverWindows(true);
}

constexpr double kStockTaskbarHeight = 48;

struct TrayRow {
    winrt::weak_ref<wux::FrameworkElement> element;
    wf::IInspectable originalHeight{nullptr};
    wf::IInspectable originalAlignment{nullptr};
};

[[clang::no_destroy]] std::optional<std::list<TrayRow>> g_trayRows{std::in_place};

void FixTrayRow(wux::FrameworkElement const& row) {
    g_trayRows->remove_if([](TrayRow const& t) { return !t.element.get(); });
    bool known = false;
    for (auto& t : *g_trayRows) known |= t.element.get() == row;
    if (!known) {
        g_trayRows->push_back(
            {winrt::make_weak(row), row.ReadLocalValue(wux::FrameworkElement::HeightProperty()),
             row.ReadLocalValue(wux::FrameworkElement::VerticalAlignmentProperty())});
        Wh_Log(L"System tray found");
    }
    if (row.Height() != kStockTaskbarHeight) row.Height(kStockTaskbarHeight);
    if (row.VerticalAlignment() != wux::VerticalAlignment::Bottom) {
        row.VerticalAlignment(wux::VerticalAlignment::Bottom);
    }
}

void RestoreTrayRows() {
    for (auto& t : *g_trayRows) {
        auto el = t.element.get();
        if (!el) continue;
        try {
            RestoreLocalValue(el, wux::FrameworkElement::HeightProperty(), t.originalHeight);
            RestoreLocalValue(el, wux::FrameworkElement::VerticalAlignmentProperty(),
                              t.originalAlignment);
        } catch (...) {}
    }
    g_trayRows->clear();
}

void ScanTray(wux::FrameworkElement const& frame) {
    auto root = frame.XamlRoot();
    auto content = root ? root.Content() : nullptr;
    if (!content) return;
    auto visit = [](wux::FrameworkElement const& el) {
        auto type = winrt::get_class_name(el);
        if (type == L"Taskbar.TaskbarFrame") return false;
        if (type == L"SystemTray.SystemTrayFrame") {
            if (g_hideTray) { HideHighlight(el); return false; }
            UnhideHighlight(el);
            auto findRow = [](wux::FrameworkElement const& child) {
                if (child.Name() == L"SystemTrayFrameGrid") FixTrayRow(child);
                return false;
            };
            ForEachDescendant(el, 1, findRow);
            return false;
        }
        return true;
    };
    ForEachDescendant(content, 10, visit);
}

struct LpApp {
    std::wstring name;
    std::wstring parsing;
    std::shared_ptr<const std::vector<uint32_t>> icon;
    int iconPx = 0;
};

constexpr wchar_t kMacDockLaunchpadClass[] = L"MacOSStyleDockLaunchpadOverlay";
constexpr GUID kMacDockBHIDEnumItems = {0x94f60519, 0x2850, 0x4924, {0xaa, 0x5a, 0xd1, 0x5e, 0x84, 0x86, 0x80, 0x39}};
constexpr UINT kLpAppsReady = WM_APP + 20;
constexpr UINT_PTR kLpTimer = 1;
constexpr int kLpAlpha = 238;
constexpr int kLpOpenMs = 200;
constexpr int kLpCloseMs = 140;

bool g_lpAnimations = true;
std::atomic<bool> g_lpModeOn = true;
std::atomic<bool> g_lpUserDisabled = false;
std::atomic<int> g_lpSize = 1;
int g_lpGrid = 0;
bool g_lpShowNames = true;
std::atomic<int> g_lpIconPx = 0;
std::atomic<bool> g_lpRebuildAgain = false;
std::mutex g_lpAppsLock;
std::vector<LpApp> g_lpAppList;
ULONGLONG g_lpListBuilt = 0;
std::atomic<bool> g_lpBuilding = false;
std::atomic<bool> g_lpStop = false;
HANDLE g_lpWorker = nullptr;
std::atomic<HWND> g_lpHwnd = nullptr;
ULONGLONG g_lpDismissedAt = 0;
bool g_lpClassRegistered = false;

bool LpAvailable() { return g_lpModeOn && !g_lpUserDisabled; }

int LpIconPx() {
    static const int base[3] = {48, 64, 84};
    int level = std::clamp(g_lpSize.load(), 0, 2);
    int px = MulDiv(base[level], (int)GetDpiForSystem(), 96);
    if (GetSystemMetrics(SM_CYSCREEN) < 800) px = px * 3 / 4;
    return std::clamp(px, 40, 128);
}

std::shared_ptr<const std::vector<uint32_t>> LpPixelsFromBitmap(HBITMAP hbm, int px) {
    BITMAP bm{};
    if (!GetObject(hbm, sizeof(bm), &bm) || bm.bmWidth <= 0 || bm.bmHeight <= 0) return nullptr;
    int w = bm.bmWidth, h = bm.bmHeight;
    std::vector<uint32_t> src((size_t)w * h);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC dc = GetDC(nullptr);
    int got = GetDIBits(dc, hbm, 0, h, src.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    if (!got) return nullptr;

    bool anyAlpha = false;
    for (uint32_t p : src) {
        if (p >> 24) { anyAlpha = true; break; }
    }
    auto out = std::make_shared<std::vector<uint32_t>>((size_t)px * px, 0u);
    int dw = w, dh = h;
    if (w > px || h > px) {
        float k = std::min((float)px / w, (float)px / h);
        dw = std::max(1, (int)(w * k));
        dh = std::max(1, (int)(h * k));
    }
    int ox = (px - dw) / 2, oy = (px - dh) / 2;
    for (int y = 0; y < dh; y++) {
        int sy = y * h / dh;
        for (int x = 0; x < dw; x++) {
            uint32_t p = src[(size_t)sy * w + (size_t)x * w / dw];
            uint32_t a = anyAlpha ? (p >> 24) : 255;
            uint32_t r = std::min<uint32_t>((p >> 16) & 255, a);
            uint32_t g = std::min<uint32_t>((p >> 8) & 255, a);
            uint32_t b = std::min<uint32_t>(p & 255, a);
            (*out)[(size_t)(oy + y) * px + ox + x] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
    return out;
}

std::shared_ptr<const std::vector<uint32_t>> LpPixelsFromIcon(HICON icon, int px) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = px;
    bi.bmiHeader.biHeight = -px;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    void* darkBits = nullptr;
    void* lightBits = nullptr;
    HBITMAP darkBmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &darkBits, nullptr, 0);
    HBITMAP lightBmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &lightBits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    std::shared_ptr<const std::vector<uint32_t>> result;
    if (dc && darkBmp && lightBmp && darkBits && lightBits) {
        size_t n = (size_t)px * px;
        HGDIOBJ old = SelectObject(dc, darkBmp);
        memset(darkBits, 0, n * 4);
        DrawIconEx(dc, 0, 0, icon, px, px, 0, nullptr, DI_NORMAL);
        SelectObject(dc, lightBmp);
        memset(lightBits, 0xFF, n * 4);
        DrawIconEx(dc, 0, 0, icon, px, px, 0, nullptr, DI_NORMAL);
        GdiFlush();
        SelectObject(dc, old);

        auto out = std::make_shared<std::vector<uint32_t>>(n, 0u);
        const uint32_t* dk = (const uint32_t*)darkBits;
        const uint32_t* lt = (const uint32_t*)lightBits;
        bool visible = false;
        for (size_t i = 0; i < n; i++) {
            int dr = (dk[i] >> 16) & 255, dg = (dk[i] >> 8) & 255, db = dk[i] & 255;
            int lr = (lt[i] >> 16) & 255, lg = (lt[i] >> 8) & 255, lb = lt[i] & 255;
            int diff = ((lr - dr) + (lg - dg) + (lb - db)) / 3;
            int a = std::clamp(255 - diff, 0, 255);
            if (!a) continue;
            visible = true;
            int r = std::min(dr, a), g = std::min(dg, a), b = std::min(db, a);
            (*out)[i] = ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
        }
        if (visible) result = out;
    }
    if (darkBmp) DeleteObject(darkBmp);
    if (lightBmp) DeleteObject(lightBmp);
    if (dc) DeleteDC(dc);
    return result;
}

bool LpIsExistingExe(std::wstring const& path) {
    if (path.size() < 5) return false;
    if (_wcsicmp(path.c_str() + path.size() - 4, L".exe") != 0) return false;
    return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

std::wstring LpResolveExePath(IShellItem* item, std::wstring const& parsing) {
    constexpr PROPERTYKEY kLinkTarget = {
        {0xB9B4B3FC, 0x2B51, 0x4A42, {0xB5, 0xD8, 0x32, 0x41, 0x46, 0xAF, 0xCF, 0x25}}, 2};
    std::wstring path;
    IShellItem2* item2 = nullptr;
    if (item && SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&item2))) && item2) {
        LPWSTR target = nullptr;
        if (SUCCEEDED(item2->GetString(kLinkTarget, &target)) && target) {
            path = target;
            CoTaskMemFree(target);
        }
        item2->Release();
    }
    if (LpIsExistingExe(path)) return path;

    path.clear();
    if (!parsing.empty() && parsing[0] == L'{') {
        size_t close = parsing.find(L'}');
        if (close != std::wstring::npos) {
            GUID id{};
            if (SUCCEEDED(CLSIDFromString(parsing.substr(0, close + 1).c_str(), &id))) {
                PWSTR base = nullptr;
                if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &base)) && base) {
                    path = std::wstring(base) + parsing.substr(close + 1);
                    CoTaskMemFree(base);
                }
            }
        }
    } else if (parsing.size() > 2 && (parsing[1] == L':' || parsing.compare(0, 2, L"\\\\") == 0)) {
        path = parsing;
    }
    if (LpIsExistingExe(path)) return path;
    return L"";
}

std::shared_ptr<const std::vector<uint32_t>> LpExeIconPixels(std::wstring const& path, int px) {
    using PrivateExtractIconsW_t =
        UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
    static PrivateExtractIconsW_t extract = [] {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        return user32 ? (PrivateExtractIconsW_t)GetProcAddress(user32, "PrivateExtractIconsW")
                      : nullptr;
    }();
    if (!extract) return nullptr;
    HICON icon = nullptr;
    UINT id = 0;
    UINT got = extract(path.c_str(), 0, px, px, &icon, &id, 1, LR_DEFAULTCOLOR);
    if (got != 1 || !icon) return nullptr;
    auto pixels = LpPixelsFromIcon(icon, px);
    DestroyIcon(icon);
    return pixels;
}

DWORD WINAPI LpBuildThread(LPVOID) {
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const int px = LpIconPx();
    g_lpIconPx = px;

    std::vector<LpApp> old;
    {
        std::lock_guard<std::mutex> lock(g_lpAppsLock);
        old = g_lpAppList;
    }

    struct Entry {
        LpApp app;
        IShellItem* item = nullptr;
    };
    std::vector<Entry> entries;

    PIDLIST_ABSOLUTE pidl = nullptr;
    IShellItem* folder = nullptr;
    if (SUCCEEDED(SHParseDisplayName(L"shell:AppsFolder", nullptr, &pidl, 0, nullptr)) && pidl) {
        if (FAILED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&folder)))) folder = nullptr;
        CoTaskMemFree(pidl);
    }
    if (folder) {
        IEnumShellItems* en = nullptr;
        if (SUCCEEDED(folder->BindToHandler(nullptr, kMacDockBHIDEnumItems, IID_PPV_ARGS(&en))) && en) {
            IShellItem* item = nullptr;
            ULONG fetched = 0;
            while (!g_lpStop && en->Next(1, &item, &fetched) == S_OK && fetched) {
                LPWSTR name = nullptr;
                LPWSTR parse = nullptr;
                item->GetDisplayName(SIGDN_NORMALDISPLAY, &name);
                item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &parse);
                if (name && parse && *name && *parse) {
                    Entry e;
                    e.app.name = name;
                    e.app.parsing = parse;
                    for (auto& o : old) {
                        if (o.icon && o.parsing == e.app.parsing) {
                            e.app.icon = o.icon;
                            e.app.iconPx = o.iconPx;
                            break;
                        }
                    }
                    e.item = item;
                    item->AddRef();
                    entries.push_back(std::move(e));
                }
                if (name) CoTaskMemFree(name);
                if (parse) CoTaskMemFree(parse);
                item->Release();
                item = nullptr;
            }
            en->Release();
        }
        folder->Release();
    }

    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return lstrcmpiW(a.app.name.c_str(), b.app.name.c_str()) < 0;
    });

    auto notify = [] {
        if (HWND w = g_lpHwnd) PostMessage(w, kLpAppsReady, 0, 0);
    };

    if (!g_lpStop && !entries.empty()) {
        std::vector<LpApp> apps;
        apps.reserve(entries.size());
        for (auto& e : entries) apps.push_back(e.app);
        std::lock_guard<std::mutex> lock(g_lpAppsLock);
        g_lpAppList = std::move(apps);
        notify();
    }

    int sinceNotify = 0;
    for (size_t i = 0; i < entries.size() && !g_lpStop && !g_lpRebuildAgain; i++) {
        auto& e = entries[i];
        if (e.app.icon && e.app.iconPx == px) continue;

        std::shared_ptr<const std::vector<uint32_t>> pixels;
        std::wstring exe = LpResolveExePath(e.item, e.app.parsing);
        if (!exe.empty()) pixels = LpExeIconPixels(exe, px);

        if (!pixels) {
            IShellItemImageFactory* factory = nullptr;
            if (SUCCEEDED(e.item->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
                HBITMAP hbm = nullptr;
                if (SUCCEEDED(factory->GetImage(SIZE{px, px}, SIIGBF_ICONONLY, &hbm)) && hbm) {
                    pixels = LpPixelsFromBitmap(hbm, px);
                    DeleteObject(hbm);
                }
                factory->Release();
            }
        }

        if (pixels) {
            std::lock_guard<std::mutex> lock(g_lpAppsLock);
            if (i < g_lpAppList.size() && g_lpAppList[i].parsing == e.app.parsing) {
                g_lpAppList[i].icon = pixels;
                g_lpAppList[i].iconPx = px;
            }
        }
        if (++sinceNotify >= 12) {
            sinceNotify = 0;
            notify();
        }
    }
    for (auto& e : entries) {
        if (e.item) e.item->Release();
    }
    {
        std::lock_guard<std::mutex> lock(g_lpAppsLock);
        g_lpListBuilt = GetTickCount64();
    }
    g_lpBuilding = false;
    notify();
    if (SUCCEEDED(co)) CoUninitialize();
    return 0;
}

void LpStartBuild(bool force = false) {
    if (g_lpBuilding.exchange(true)) {
        if (force) g_lpRebuildAgain = true;
        return;
    }
    g_lpRebuildAgain = false;
    if (g_lpWorker) {
        WaitForSingleObject(g_lpWorker, 3000);
        CloseHandle(g_lpWorker);
        g_lpWorker = nullptr;
    }
    g_lpStop = false;
    g_lpWorker = CreateThread(nullptr, 0, LpBuildThread, nullptr, 0, nullptr);
    if (!g_lpWorker) g_lpBuilding = false;
    else SetThreadPriority(g_lpWorker, THREAD_PRIORITY_BELOW_NORMAL);
}

void LpReleaseApps() {
    g_lpStop = true;
    std::lock_guard<std::mutex> lock(g_lpAppsLock);
    g_lpAppList.clear();
    g_lpListBuilt = 0;
}

void LpStopBuild() {
    g_lpStop = true;
    if (g_lpWorker) {
        WaitForSingleObject(g_lpWorker, 8000);
        CloseHandle(g_lpWorker);
        g_lpWorker = nullptr;
    }
    {
        std::lock_guard<std::mutex> lock(g_lpAppsLock);
        g_lpAppList.clear();
        g_lpListBuilt = 0;
    }
}

void LpEnsureBuilt() {
    bool empty;
    {
        std::lock_guard<std::mutex> lock(g_lpAppsLock);
        empty = g_lpAppList.empty();
    }
    if (empty) LpStartBuild();
}

struct LpState {
    HWND hwnd = nullptr;
    int W = 0, H = 0;
    float s = 1.0f;
    int px = 64;
    int cols = 7, rows = 5, perPage = 35;
    int gridL = 0, gridT = 0, cellW = 1, cellH = 1, labelH = 30;
    RECT search{};
    int dotsY = 0;
    int page = 0, hover = -1, wheel = 0;
    std::wstring query;
    std::vector<LpApp> apps;
    std::vector<int> filtered;
    HDC memDC = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    uint32_t* bits = nullptr;
    HFONT fontLabel = nullptr, fontSearch = nullptr, fontIcon = nullptr;
    int phase = 0;
    ULONGLONG t0 = 0;
    int alpha = 0, alphaFrom = 0;

    bool settingsOpen = false, hoverGear = false;
    RECT gear{}, panel{}, sizeBar{}, gridBar{}, groupBox{}, rowNames{}, rowDisable{}, footer{};
    RECT sizeSeg[3]{};
    RECT gridSeg[5]{};
    int titleY = 0, sizeLabelY = 0, gridLabelY = 0;
};
LpState g_lp;

int LpPages() { return std::max(1, ((int)g_lp.filtered.size() + g_lp.perPage - 1) / g_lp.perPage); }

void LpFilter() {
    g_lp.filtered.clear();
    std::vector<int> contains;
    std::wstring q = g_lp.query;
    for (auto& c : q) c = (wchar_t)std::towlower(c);
    for (size_t i = 0; i < g_lp.apps.size(); i++) {
        if (q.empty()) { g_lp.filtered.push_back((int)i); continue; }
        std::wstring n = g_lp.apps[i].name;
        for (auto& c : n) c = (wchar_t)std::towlower(c);
        size_t at = n.find(q);
        if (at == 0) g_lp.filtered.push_back((int)i);
        else if (at != std::wstring::npos) contains.push_back((int)i);
    }
    g_lp.filtered.insert(g_lp.filtered.end(), contains.begin(), contains.end());
    g_lp.page = std::clamp(g_lp.page, 0, LpPages() - 1);
    g_lp.hover = -1;
}

void LpLayoutPanel() {
    LpState& l = g_lp;
    float s = l.s;
    int pad = (int)(16 * s);
    int gearSize = (int)(36 * s);
    int right = l.W - (int)(28 * s);
    l.gear = RECT{right - gearSize, l.search.top, right, l.search.top + gearSize};

    int pw = std::min((int)(340 * s), l.W - (int)(32 * s));
    int left = right - pw;
    l.panel.left = left;
    l.panel.right = right;
    l.panel.top = l.gear.bottom + (int)(10 * s);

    int cy = l.panel.top + pad;
    l.titleY = cy;
    cy += (int)(26 * s);

    int segH = (int)(32 * s);
    int innerL = left + pad, innerR = right - pad;

    l.sizeLabelY = cy;
    cy += (int)(20 * s);
    l.sizeBar = RECT{innerL, cy, innerR, cy + segH};
    {
        int w = (innerR - innerL) / 3;
        for (int i = 0; i < 3; i++) {
            l.sizeSeg[i] = RECT{innerL + i * w, cy, i == 2 ? innerR : innerL + (i + 1) * w, cy + segH};
        }
    }
    cy += segH + (int)(16 * s);

    l.gridLabelY = cy;
    cy += (int)(20 * s);
    l.gridBar = RECT{innerL, cy, innerR, cy + segH};
    {
        int w = (innerR - innerL) / 5;
        for (int i = 0; i < 5; i++) {
            l.gridSeg[i] = RECT{innerL + i * w, cy, i == 4 ? innerR : innerL + (i + 1) * w, cy + segH};
        }
    }
    cy += segH + (int)(16 * s);

    int rowH = (int)(44 * s);
    l.groupBox = RECT{innerL, cy, innerR, cy + rowH * 2};
    l.rowNames = RECT{innerL, cy, innerR, cy + rowH};
    l.rowDisable = RECT{innerL, cy + rowH, innerR, cy + rowH * 2};
    cy += rowH * 2 + (int)(8 * s);

    l.footer = RECT{innerL + (int)(4 * s), cy, innerR - (int)(4 * s), cy + (int)(44 * s)};
    cy += (int)(44 * s) + pad;
    l.panel.bottom = cy;
}

void LpLayout() {
    LpState& l = g_lp;
    l.s = std::clamp((float)GetDpiForSystem() / 96.0f, 1.0f, 3.0f);
    l.px = LpIconPx();
    int margin = (int)(l.W * 0.09f);
    int sw = std::min((int)(360 * l.s), (int)(l.W * 0.5f));
    int sh = (int)(36 * l.s);
    int sy = (int)(l.H * 0.055f);
    l.search = RECT{(l.W - sw) / 2, sy, (l.W + sw) / 2, sy + sh};
    l.dotsY = l.H - (int)(l.H * 0.065f);
    l.gridL = margin;
    l.gridT = l.search.bottom + (int)(l.H * 0.05f);
    int gridW = l.W - margin * 2;
    int gridH = (l.dotsY - (int)(24 * l.s)) - l.gridT;
    l.labelH = g_lpShowNames ? (int)(34 * l.s) : 0;
    int minCellH = l.px + (int)(14 * l.s) + l.labelH;
    int minCellW = l.px + (int)(40 * l.s);

    static const int presetCols[5] = {7, 5, 6, 7, 8};
    static const int presetRows[5] = {5, 4, 4, 5, 6};
    int preset = std::clamp(g_lpGrid, 0, 4);
    l.cols = std::clamp(gridW / std::max(1, minCellW), 3, presetCols[preset]);
    l.rows = std::clamp(gridH / std::max(1, minCellH), 2, presetRows[preset]);
    l.cellW = std::max(1, gridW / l.cols);
    l.cellH = std::max(1, gridH / l.rows);
    l.perPage = l.cols * l.rows;
    LpLayoutPanel();
}

RECT LpCell(int i) {
    int col = i % g_lp.cols, row = i / g_lp.cols;
    return RECT{g_lp.gridL + col * g_lp.cellW, g_lp.gridT + row * g_lp.cellH,
                g_lp.gridL + (col + 1) * g_lp.cellW, g_lp.gridT + (row + 1) * g_lp.cellH};
}

int LpHit(int x, int y) {
    for (int i = 0; i < g_lp.perPage; i++) {
        if (g_lp.page * g_lp.perPage + i >= (int)g_lp.filtered.size()) break;
        RECT c = LpCell(i);
        if (x >= c.left && x < c.right && y >= c.top && y < c.bottom) return i;
    }
    return -1;
}

inline void LpBlendPixel(uint32_t* dst, int r, int g, int b, int a) {
    uint32_t d = *dst;
    int dr = (d >> 16) & 255, dg = (d >> 8) & 255, db = d & 255;
    dr = (r * a + dr * (255 - a)) / 255;
    dg = (g * a + dg * (255 - a)) / 255;
    db = (b * a + db * (255 - a)) / 255;
    *dst = 0xFF000000u | (dr << 16) | (dg << 8) | db;
}

void LpRoundRect(int x0, int y0, int x1, int y1, float radius, int r, int g, int b, int alpha) {
    LpState& l = g_lp;
    x0 = std::max(x0, 0); y0 = std::max(y0, 0);
    x1 = std::min(x1, l.W); y1 = std::min(y1, l.H);
    float fx0 = (float)x0, fy0 = (float)y0, fx1 = (float)x1, fy1 = (float)y1;
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            float cx = x + 0.5f, cy = y + 0.5f;
            float dx = std::max({fx0 + radius - cx, cx - (fx1 - radius), 0.0f});
            float dy = std::max({fy0 + radius - cy, cy - (fy1 - radius), 0.0f});
            float cover = 1.0f;
            if (dx > 0 || dy > 0) cover = std::clamp(radius + 0.5f - std::sqrt(dx * dx + dy * dy), 0.0f, 1.0f);
            int a = (int)(alpha * cover);
            if (a > 0) LpBlendPixel(&l.bits[(size_t)y * l.W + x], r, g, b, a);
        }
    }
}

void LpDisc(float cx, float cy, float radius, int r, int g, int b, int alpha) {
    LpState& l = g_lp;
    int x0 = std::max(0, (int)(cx - radius - 1)), x1 = std::min(l.W, (int)(cx + radius + 2));
    int y0 = std::max(0, (int)(cy - radius - 1)), y1 = std::min(l.H, (int)(cy + radius + 2));
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            float dx = x + 0.5f - cx, dy = y + 0.5f - cy;
            float cover = std::clamp(radius + 0.5f - std::sqrt(dx * dx + dy * dy), 0.0f, 1.0f);
            int a = (int)(alpha * cover);
            if (a > 0) LpBlendPixel(&l.bits[(size_t)y * l.W + x], r, g, b, a);
        }
    }
}

void LpBlitIcon(std::vector<uint32_t> const& icon, int px, int x0, int y0) {
    LpState& l = g_lp;
    for (int y = 0; y < px; y++) {
        int dy = y0 + y;
        if (dy < 0 || dy >= l.H) continue;
        const uint32_t* src = &icon[(size_t)y * px];
        uint32_t* dst = &l.bits[(size_t)dy * l.W];
        for (int x = 0; x < px; x++) {
            int dx = x0 + x;
            if (dx < 0 || dx >= l.W) continue;
            uint32_t s = src[x];
            uint32_t a = s >> 24;
            if (!a) continue;
            if (a == 255) { dst[dx] = s; continue; }
            uint32_t d = dst[dx];
            uint32_t ia = 255 - a;
            uint32_t r = ((s >> 16) & 255) + (((d >> 16) & 255) * ia) / 255;
            uint32_t g = ((s >> 8) & 255) + (((d >> 8) & 255) * ia) / 255;
            uint32_t b = (s & 255) + ((d & 255) * ia) / 255;
            dst[dx] = 0xFF000000u | (std::min(r, 255u) << 16) | (std::min(g, 255u) << 8) | std::min(b, 255u);
        }
    }
}

int LpIconTop(RECT const& c) {
    int content = g_lp.px + (g_lpShowNames ? (int)(8 * g_lp.s) + g_lp.labelH : 0);
    return c.top + std::max(0, (int)((c.bottom - c.top) - content) / 2);
}

void LpDrawText(HDC dc, const wchar_t* text, RECT r, UINT flags, COLORREF color) {
    SetTextColor(dc, color);
    DrawTextW(dc, text, -1, &r, flags);
}

void LpRenderPanel() {
    LpState& l = g_lp;
    float s = l.s;
    float rad = 16 * s;
    RECT p = l.panel;
    int sp = (int)(8 * s);

    LpRoundRect(p.left - sp, p.top + sp / 2, p.right + sp, p.bottom + sp * 2, rad + sp, 0, 0, 0, 34);
    LpRoundRect(p.left - sp / 2, p.top + sp / 4, p.right + sp / 2, p.bottom + sp, rad + sp / 2, 0, 0, 0, 44);
    LpRoundRect(p.left, p.top, p.right, p.bottom, rad, 255, 255, 255, 40);
    LpRoundRect(p.left + 1, p.top + 1, p.right - 1, p.bottom - 1, rad - 1, 44, 46, 58, 255);

    int inset = std::max(1, (int)(2 * s));
    auto segmented = [&](RECT const& bar, RECT const* segs, int selected) {
        LpRoundRect(bar.left, bar.top, bar.right, bar.bottom, 9 * s, 255, 255, 255, 24);
        RECT r = segs[selected];
        LpRoundRect(r.left + inset, r.top + inset, r.right - inset, r.bottom - inset, 7 * s, 255, 255, 255, 78);
    };
    int sizeSel = std::clamp(g_lpSize.load(), 0, 2);
    int gridSel = std::clamp(g_lpGrid, 0, 4);
    segmented(l.sizeBar, l.sizeSeg, sizeSel);
    segmented(l.gridBar, l.gridSeg, gridSel);

    LpRoundRect(l.groupBox.left, l.groupBox.top, l.groupBox.right, l.groupBox.bottom, 10 * s, 255, 255, 255, 22);
    LpRoundRect(l.groupBox.left + (int)(14 * s), l.rowNames.bottom, l.groupBox.right, l.rowNames.bottom + 1, 0, 255, 255, 255, 34);

    auto toggle = [&](RECT const& row, bool on, int tr, int tg, int tb) {
        int w = (int)(46 * s), h = (int)(28 * s);
        int x1 = row.right - (int)(14 * s), x0 = x1 - w;
        int y0 = (row.top + row.bottom - h) / 2, y1 = y0 + h;
        if (on) LpRoundRect(x0, y0, x1, y1, h / 2.0f, tr, tg, tb, 255);
        else LpRoundRect(x0, y0, x1, y1, h / 2.0f, 255, 255, 255, 40);
        float knobR = h / 2.0f - 2 * s;
        float kx = on ? x1 - h / 2.0f : x0 + h / 2.0f;
        LpDisc(kx, (y0 + y1) / 2.0f, knobR, 255, 255, 255, 255);
    };
    toggle(l.rowNames, g_lpShowNames, 52, 199, 89);
    toggle(l.rowDisable, g_lpUserDisabled, 255, 69, 58);

    HDC dc = l.memDC;
    SetBkMode(dc, TRANSPARENT);
    HGDIOBJ oldFont = SelectObject(dc, l.fontSearch);
    UINT one = DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
    int labelPad = (int)(14 * s);
    int toggleRoom = (int)(70 * s);

    LpDrawText(dc, L"Launchpad Settings", RECT{p.left, l.titleY, p.right, l.titleY + (int)(26 * s)},
               DT_CENTER | one, RGB(245, 247, 255));
    LpDrawText(dc, L"App Names", RECT{l.rowNames.left + labelPad, l.rowNames.top, l.rowNames.right - toggleRoom, l.rowNames.bottom},
               DT_LEFT | one, RGB(245, 247, 255));
    LpDrawText(dc, L"Turn Off Launchpad", RECT{l.rowDisable.left + labelPad, l.rowDisable.top, l.rowDisable.right - toggleRoom, l.rowDisable.bottom},
               DT_LEFT | one, RGB(245, 247, 255));

    SelectObject(dc, l.fontLabel);
    int capPad = (int)(4 * s);
    LpDrawText(dc, L"ICON SIZE", RECT{l.sizeBar.left + capPad, l.sizeLabelY, l.sizeBar.right, l.sizeLabelY + (int)(20 * s)},
               DT_LEFT | one, RGB(150, 156, 172));
    LpDrawText(dc, L"GRID", RECT{l.gridBar.left + capPad, l.gridLabelY, l.gridBar.right, l.gridLabelY + (int)(20 * s)},
               DT_LEFT | one, RGB(150, 156, 172));

    static const wchar_t* const sizeNames[3] = {L"Small", L"Medium", L"Large"};
    static const wchar_t* const gridNames[5] = {L"Auto", L"5\u00d74", L"6\u00d74", L"7\u00d75", L"8\u00d76"};
    for (int i = 0; i < 3; i++) {
        LpDrawText(dc, sizeNames[i], l.sizeSeg[i], DT_CENTER | one, i == sizeSel ? RGB(255, 255, 255) : RGB(200, 205, 220));
    }
    for (int i = 0; i < 5; i++) {
        LpDrawText(dc, gridNames[i], l.gridSeg[i], DT_CENTER | one, i == gridSel ? RGB(255, 255, 255) : RGB(200, 205, 220));
    }
    LpDrawText(dc, L"Turning Launchpad off brings back the regular Windows Start menu. You can enable it again in the mod's settings in Windhawk.",
               l.footer, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX, RGB(150, 156, 172));

    SelectObject(dc, oldFont);
    GdiFlush();
}

void LpRender() {
    LpState& l = g_lp;
    if (!l.bits) return;

    for (int y = 0; y < l.H; y++) {
        int t = y * 255 / std::max(1, l.H - 1);
        uint32_t r = 30 + (16 - 30) * t / 255, g = 33 + (18 - 33) * t / 255, b = 46 + (26 - 46) * t / 255;
        uint32_t px = 0xFF000000u | (r << 16) | (g << 8) | b;
        std::fill_n(&l.bits[(size_t)y * l.W], l.W, px);
    }

    float sr = (l.search.bottom - l.search.top) / 2.0f;
    LpRoundRect(l.search.left, l.search.top, l.search.right, l.search.bottom, sr, 255, 255, 255, 30);

    LpDisc((l.gear.left + l.gear.right) / 2.0f, (l.gear.top + l.gear.bottom) / 2.0f,
           (l.gear.right - l.gear.left) / 2.0f, 255, 255, 255, (l.hoverGear || l.settingsOpen) ? 62 : 30);

    int start = l.page * l.perPage;
    int count = std::min(l.perPage, (int)l.filtered.size() - start);
    int pad = (int)(6 * l.s);

    if (l.hover >= 0 && l.hover < count) {
        RECT c = LpCell(l.hover);
        LpRoundRect(c.left + pad, c.top + pad, c.right - pad, c.bottom - pad, 14 * l.s, 255, 255, 255, 26);
    }
    for (int i = 0; i < count; i++) {
        auto& app = l.apps[l.filtered[start + i]];
        RECT c = LpCell(i);
        int top = LpIconTop(c);
        if (!app.icon) {
            int side = l.px * 4 / 5;
            int x = (c.left + c.right - side) / 2, y = top + (l.px - side) / 2;
            LpRoundRect(x, y, x + side, y + side, side * 0.22f, 255, 255, 255, 30);
            continue;
        }
        LpBlitIcon(*app.icon, app.iconPx, (c.left + c.right - app.iconPx) / 2, top + (l.px - app.iconPx) / 2);
    }

    int pages = LpPages();
    if (pages > 1) {
        float gap = 18 * l.s, rad = 4 * l.s;
        float x = l.W / 2.0f - gap * (pages - 1) / 2.0f;
        for (int p = 0; p < pages; p++, x += gap) {
            LpDisc(x, (float)l.dotsY, rad, 255, 255, 255, p == l.page ? 235 : 85);
        }
    }

    HDC dc = l.memDC;
    SetBkMode(dc, TRANSPARENT);
    HGDIOBJ oldFont = SelectObject(dc, l.fontSearch);

    {
        int cy = (l.search.top + l.search.bottom) / 2;
        int gx = l.search.left + (int)(18 * l.s);
        int rr = (int)(5 * l.s);
        HPEN pen = CreatePen(PS_SOLID, std::max(1, (int)(1.6f * l.s)), RGB(165, 170, 186));
        HGDIOBJ oldPen = SelectObject(dc, pen);
        HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
        Ellipse(dc, gx - rr, cy - rr - 1, gx + rr, cy + rr - 1);
        MoveToEx(dc, gx + rr * 7 / 10, cy + rr * 7 / 10 - 1, nullptr);
        LineTo(dc, gx + rr * 7 / 10 + (int)(4 * l.s), cy + rr * 7 / 10 + (int)(4 * l.s) - 1);
        SelectObject(dc, oldBrush);
        SelectObject(dc, oldPen);
        DeleteObject(pen);

        RECT tr{gx + rr + (int)(14 * l.s), l.search.top, l.search.right - (int)(14 * l.s), l.search.bottom};
        SetTextColor(dc, l.query.empty() ? RGB(150, 156, 172) : RGB(245, 247, 255));
        std::wstring text = l.query.empty() ? L"Search" : l.query;
        DrawTextW(dc, text.c_str(), -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    }

    if (l.fontIcon) {
        SelectObject(dc, l.fontIcon);
        LpDrawText(dc, L"\uE713", l.gear, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX, RGB(235, 238, 248));
    }

    if (g_lpShowNames) {
        SelectObject(dc, l.fontLabel);
        for (int i = 0; i < count; i++) {
            RECT c = LpCell(i);
            RECT label{c.left + (int)(4 * l.s), LpIconTop(c) + l.px + (int)(8 * l.s),
                       c.right - (int)(4 * l.s), 0};
            label.bottom = label.top + l.labelH;
            const std::wstring& name = l.apps[l.filtered[start + i]].name;
            UINT flags = DT_CENTER | DT_TOP | DT_WORDBREAK | DT_END_ELLIPSIS | DT_NOPREFIX;
            RECT shadow = label;
            OffsetRect(&shadow, 1, 1);
            SetTextColor(dc, RGB(0, 0, 0));
            DrawTextW(dc, name.c_str(), -1, &shadow, flags);
            SetTextColor(dc, RGB(240, 242, 250));
            DrawTextW(dc, name.c_str(), -1, &label, flags);
        }
    }

    if (l.filtered.empty()) {
        SelectObject(dc, l.fontLabel);
        bool loading = l.apps.empty() && g_lpBuilding;
        SetTextColor(dc, RGB(170, 176, 192));
        RECT r{0, l.H / 2 - 20, l.W, l.H / 2 + 20};
        DrawTextW(dc, loading ? L"Loading apps..." : L"No results", -1, &r,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }
    SelectObject(dc, oldFont);
    GdiFlush();

    if (l.settingsOpen) LpRenderPanel();
}

void LpFreeBuffers() {
    LpState& l = g_lp;
    if (l.memDC) {
        if (l.oldBitmap) SelectObject(l.memDC, l.oldBitmap);
        DeleteDC(l.memDC);
    }
    if (l.dib) DeleteObject(l.dib);
    if (l.fontLabel) DeleteObject(l.fontLabel);
    if (l.fontSearch) DeleteObject(l.fontSearch);
    if (l.fontIcon) DeleteObject(l.fontIcon);
    l.memDC = nullptr; l.dib = nullptr; l.oldBitmap = nullptr; l.bits = nullptr;
    l.fontLabel = nullptr; l.fontSearch = nullptr; l.fontIcon = nullptr;
}

bool LpCreateBuffers() {
    LpState& l = g_lp;
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = l.W;
    bi.bmiHeader.biHeight = -l.H;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC screen = GetDC(nullptr);
    l.memDC = CreateCompatibleDC(screen);
    void* bits = nullptr;
    l.dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!l.memDC || !l.dib || !bits) { LpFreeBuffers(); return false; }
    l.bits = (uint32_t*)bits;
    l.oldBitmap = SelectObject(l.memDC, l.dib);
    l.fontLabel = CreateFontW(-(int)(13 * l.s), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                              DEFAULT_PITCH, L"Segoe UI");
    l.fontSearch = CreateFontW(-(int)(15 * l.s), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                               DEFAULT_PITCH, L"Segoe UI");
    l.fontIcon = CreateFontW(-(int)(16 * l.s), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                             DEFAULT_PITCH, L"Segoe MDL2 Assets");
    return true;
}

void LpDestroyNow() {
    if (g_lp.hwnd) DestroyWindow(g_lp.hwnd);
}

void LpBeginClose() {
    if (!g_lp.hwnd || g_lp.phase == 2) return;
    g_lpDismissedAt = GetTickCount64();
    if (!g_lpAnimations) { LpDestroyNow(); return; }
    g_lp.phase = 2;
    g_lp.t0 = GetTickCount64();
    g_lp.alphaFrom = g_lp.alpha;
    SetTimer(g_lp.hwnd, kLpTimer, 15, nullptr);
}

void LpLaunch(int filteredIndex) {
    if (filteredIndex < 0 || filteredIndex >= (int)g_lp.filtered.size()) return;
    std::wstring target = L"shell:AppsFolder\\" + g_lp.apps[g_lp.filtered[filteredIndex]].parsing;
    ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    LpBeginClose();
}

void LpChangePage(int delta) {
    int page = std::clamp(g_lp.page + delta, 0, LpPages() - 1);
    if (page == g_lp.page) return;
    g_lp.page = page;
    g_lp.hover = -1;
    LpRender();
    InvalidateRect(g_lp.hwnd, nullptr, FALSE);
}

void LpSetHover(int hover) {
    if (hover == g_lp.hover) return;
    int old = g_lp.hover;
    g_lp.hover = hover;
    LpRender();
    for (int i : {old, hover}) {
        if (i < 0) continue;
        RECT c = LpCell(i);
        InvalidateRect(g_lp.hwnd, &c, FALSE);
    }
}

void LpRefreshAll() {
    LpLayout();
    LpFilter();
    LpRender();
    InvalidateRect(g_lp.hwnd, nullptr, FALSE);
}

void LpToggleSettings() {
    g_lp.settingsOpen = !g_lp.settingsOpen;
    g_lp.hover = -1;
    LpRender();
    InvalidateRect(g_lp.hwnd, nullptr, FALSE);
}

void LpPanelClick(POINT pt) {
    LpState& l = g_lp;
    for (int i = 0; i < 3; i++) {
        if (PtInRect(&l.sizeSeg[i], pt)) {
            if (g_lpSize != i) {
                g_lpSize = i;
                Wh_SetIntValue(L"lpSize", i);
                LpRefreshAll();
                LpStartBuild(true);
            }
            return;
        }
    }
    for (int i = 0; i < 5; i++) {
        if (PtInRect(&l.gridSeg[i], pt)) {
            if (g_lpGrid != i) {
                g_lpGrid = i;
                Wh_SetIntValue(L"lpGrid", i);
                LpRefreshAll();
            }
            return;
        }
    }
    if (PtInRect(&l.rowNames, pt)) {
        g_lpShowNames = !g_lpShowNames;
        Wh_SetIntValue(L"lpNames", g_lpShowNames ? 1 : 0);
        LpRefreshAll();
        return;
    }
    if (PtInRect(&l.rowDisable, pt)) {
        g_lpUserDisabled = true;
        Wh_SetIntValue(L"lpUserDisabled", 1);
        LpReleaseApps();
        LpBeginClose();
    }
}

float LpEaseOut(float t) { float u = 1.0f - std::clamp(t, 0.0f, 1.0f); return 1.0f - u * u * u; }

LRESULT CALLBACK LpWndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (h != g_lp.hwnd) return DefWindowProcW(h, m, w, l);
    switch (m) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(h, &ps);
            if (g_lp.memDC) {
                RECT r = ps.rcPaint;
                BitBlt(dc, r.left, r.top, r.right - r.left, r.bottom - r.top, g_lp.memDC, r.left, r.top, SRCCOPY);
            }
            EndPaint(h, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_ACTIVATE:
            if (LOWORD(w) == WA_INACTIVE) LpBeginClose();
            return 0;
        case WM_TIMER:
            if (w == kLpTimer) {
                ULONGLONG now = GetTickCount64();
                if (g_lp.phase == 0) {
                    float t = (float)(now - g_lp.t0) / kLpOpenMs;
                    g_lp.alpha = (int)(kLpAlpha * LpEaseOut(t));
                    SetLayeredWindowAttributes(h, 0, (BYTE)g_lp.alpha, LWA_ALPHA);
                    if (t >= 1.0f) { g_lp.phase = 1; KillTimer(h, kLpTimer); }
                } else if (g_lp.phase == 2) {
                    float t = (float)(now - g_lp.t0) / kLpCloseMs;
                    g_lp.alpha = (int)(g_lp.alphaFrom * (1.0f - LpEaseOut(t)));
                    SetLayeredWindowAttributes(h, 0, (BYTE)std::clamp(g_lp.alpha, 0, 255), LWA_ALPHA);
                    if (t >= 1.0f) { KillTimer(h, kLpTimer); DestroyWindow(h); }
                }
            }
            return 0;
        case kLpAppsReady: {
            {
                std::lock_guard<std::mutex> lock(g_lpAppsLock);
                g_lp.apps = g_lpAppList;
            }
            LpFilter();
            LpRender();
            InvalidateRect(h, nullptr, FALSE);
            if (g_lpRebuildAgain && !g_lpBuilding && LpAvailable()) LpStartBuild();
            return 0;
        }
        case WM_CHAR:
            if (g_lp.settingsOpen) return 0;
            if (w == 8) {
                if (!g_lp.query.empty()) g_lp.query.pop_back();
            } else if (w >= 32 && w != 127) {
                g_lp.query.push_back((wchar_t)w);
            } else {
                return 0;
            }
            g_lp.page = 0;
            LpFilter();
            LpRender();
            InvalidateRect(h, nullptr, FALSE);
            return 0;
        case WM_KEYDOWN:
            if (w == VK_ESCAPE) {
                if (g_lp.settingsOpen) LpToggleSettings();
                else LpBeginClose();
            }
            else if (g_lp.settingsOpen) return 0;
            else if (w == VK_LEFT || w == VK_PRIOR) LpChangePage(-1);
            else if (w == VK_RIGHT || w == VK_NEXT) LpChangePage(1);
            else if (w == VK_RETURN) LpLaunch(g_lp.page * g_lp.perPage);
            return 0;
        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
            bool overGear = PtInRect(&g_lp.gear, POINT{x, y}) != 0;
            if (overGear != g_lp.hoverGear) {
                g_lp.hoverGear = overGear;
                LpRender();
                InvalidateRect(h, &g_lp.gear, FALSE);
            }
            if (!g_lp.settingsOpen) LpSetHover(LpHit(x, y));
            return 0;
        }
        case WM_MOUSEWHEEL:
            if (g_lp.settingsOpen) return 0;
            g_lp.wheel += GET_WHEEL_DELTA_WPARAM(w);
            if (std::abs(g_lp.wheel) >= WHEEL_DELTA) {
                LpChangePage(g_lp.wheel < 0 ? 1 : -1);
                g_lp.wheel = 0;
            }
            return 0;
        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
            POINT pt{x, y};
            if (PtInRect(&g_lp.gear, pt)) { LpToggleSettings(); return 0; }
            if (g_lp.settingsOpen) {
                if (PtInRect(&g_lp.panel, pt)) LpPanelClick(pt);
                else LpToggleSettings();
                return 0;
            }
            int cell = LpHit(x, y);
            if (cell >= 0) { LpLaunch(g_lp.page * g_lp.perPage + cell); return 0; }
            if (PtInRect(&g_lp.search, pt)) return 0;
            int pages = LpPages();
            if (pages > 1 && std::abs(y - g_lp.dotsY) < (int)(14 * g_lp.s)) {
                float gap = 18 * g_lp.s;
                float first = g_lp.W / 2.0f - gap * (pages - 1) / 2.0f;
                int p = (int)std::lround((x - first) / gap);
                if (p >= 0 && p < pages) { LpChangePage(p - g_lp.page); return 0; }
            }
            LpBeginClose();
            return 0;
        }
        case WM_DESTROY:
            KillTimer(h, kLpTimer);
            g_lpHwnd = nullptr;
            LpFreeBuffers();
            g_lp = LpState();
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

void LpOpen() {
    if (g_lp.hwnd || !LpAvailable()) return;
    if (!g_lpClassRegistered) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = LpWndProc;
        wc.hInstance = ModuleInstance();
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kMacDockLaunchpadClass;
        if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return;
        g_lpClassRegistered = true;
    }

    bool stale;
    std::vector<LpApp> apps;
    {
        std::lock_guard<std::mutex> lock(g_lpAppsLock);
        apps = g_lpAppList;
        stale = g_lpListBuilt == 0 || apps.empty() || GetTickCount64() - g_lpListBuilt > 10 * 60 * 1000;
    }
    int builtPx = g_lpIconPx;
    bool pxChanged = builtPx != 0 && builtPx != LpIconPx();
    if (stale || pxChanged) LpStartBuild(pxChanged);

    POINT pt{};
    GetCursorPos(&pt);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &mi)) return;
    RECT r = mi.rcMonitor;

    HWND h = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED, kMacDockLaunchpadClass,
                             L"Launchpad", WS_POPUP, r.left, r.top, r.right - r.left, r.bottom - r.top,
                             nullptr, nullptr, ModuleInstance(), nullptr);
    if (!h) return;

    g_lp = LpState();
    g_lp.hwnd = h;
    g_lpHwnd = h;
    g_lp.W = r.right - r.left;
    g_lp.H = r.bottom - r.top;
    g_lp.apps = std::move(apps);
    LpLayout();
    if (!LpCreateBuffers()) { DestroyWindow(h); return; }
    LpFilter();
    LpRender();

    SetLayeredWindowAttributes(h, 0, g_lpAnimations ? 0 : kLpAlpha, LWA_ALPHA);
    ShowWindow(h, SW_SHOW);
    UpdateWindow(h);
    SetForegroundWindow(h);
    SetFocus(h);
    if (g_lpAnimations) {
        g_lp.phase = 0;
        g_lp.t0 = GetTickCount64();
        SetTimer(h, kLpTimer, 15, nullptr);
    } else {
        g_lp.phase = 1;
        g_lp.alpha = kLpAlpha;
    }
}

void LaunchpadToggle() {
    if (!LpAvailable()) return;
    if (g_lp.hwnd) { LpBeginClose(); return; }
    if (GetTickCount64() - g_lpDismissedAt < 300) return;
    LpOpen();
}

void LaunchpadShutdown() {
    LpDestroyNow();
    if (g_lpClassRegistered) {
        UnregisterClassW(kMacDockLaunchpadClass, ModuleInstance());
        g_lpClassRegistered = false;
    }
}

struct LaunchpadStartHook {
    winrt::weak_ref<wux::FrameworkElement> element;
    wux::UIElement::Tapped_revoker tapped;
};
[[clang::no_destroy]] std::optional<std::list<LaunchpadStartHook>> g_launchpadStartHooks{std::in_place};
void HookLaunchpadStart(wux::FrameworkElement const& el) {
    if (g_startHooked) return;
    for (auto& hook : *g_launchpadStartHooks) if (hook.element.get() == el) return;
    LaunchpadStartHook hook;
    hook.element = winrt::make_weak(el);
    hook.tapped = el.Tapped(winrt::auto_revoke, [](wf::IInspectable const&, wuxi::TappedRoutedEventArgs const& e) {
        if (!LpAvailable()) return;
        e.Handled(true);
        LaunchpadToggle();
    });
    g_launchpadStartHooks->push_back(std::move(hook));
}

constexpr float kAppleBody[][2] = {
    {50.0f,33.0f}, {48.8f,31.5f}, {47.5f,30.2f}, {46.0f,28.9f}, {44.4f,27.7f}, {42.6f,26.7f},
    {40.7f,25.9f}, {38.8f,25.2f}, {36.7f,24.8f}, {34.5f,24.6f}, {32.3f,24.7f}, {30.0f,25.1f},
    {27.7f,25.7f}, {25.4f,26.7f}, {23.0f,28.0f}, {20.4f,29.9f}, {18.0f,32.1f}, {15.9f,34.6f},
    {14.1f,37.4f}, {12.7f,40.4f}, {11.5f,43.6f}, {10.6f,47.0f}, {10.1f,50.5f}, {9.9f,54.1f},
    {10.0f,57.7f}, {10.5f,61.3f}, {11.3f,64.9f}, {12.5f,68.5f}, {14.0f,72.0f}, {15.3f,74.8f},
    {16.7f,77.5f}, {18.3f,80.2f}, {19.8f,82.8f}, {21.5f,85.2f}, {23.2f,87.5f}, {25.0f,89.5f},
    {26.9f,91.3f}, {28.8f,92.8f}, {30.7f,94.0f}, {32.7f,94.9f}, {34.8f,95.4f}, {36.9f,95.4f},
    {39.0f,95.0f}, {40.0f,94.7f}, {41.0f,94.3f}, {41.9f,94.0f}, {42.8f,93.7f}, {43.6f,93.3f},
    {44.3f,93.0f}, {45.1f,92.7f}, {45.8f,92.4f}, {46.5f,92.2f}, {47.2f,91.9f}, {47.9f,91.8f},
    {48.6f,91.6f}, {49.3f,91.5f}, {50.0f,91.5f}, {50.7f,91.5f}, {51.4f,91.6f}, {52.1f,91.8f},
    {52.8f,91.9f}, {53.5f,92.2f}, {54.2f,92.4f}, {54.9f,92.7f}, {55.7f,93.0f}, {56.4f,93.3f},
    {57.2f,93.7f}, {58.1f,94.0f}, {59.0f,94.3f}, {60.0f,94.7f}, {61.0f,95.0f}, {63.1f,95.4f},
    {65.2f,95.4f}, {67.3f,94.9f}, {69.3f,94.0f}, {71.2f,92.8f}, {73.1f,91.3f}, {75.0f,89.5f},
    {76.8f,87.5f}, {78.5f,85.2f}, {80.2f,82.8f}, {81.7f,80.2f}, {83.3f,77.5f}, {84.7f,74.8f},
    {86.0f,72.0f}, {87.2f,69.2f}, {88.1f,66.2f}, {88.9f,63.2f}, {89.3f,60.1f}, {89.6f,56.9f},
    {90.7f,54.7f}, {89.5f,54.0f}, {88.4f,53.1f}, {87.3f,52.2f}, {86.3f,51.2f}, {85.4f,50.1f},
    {84.6f,48.9f}, {84.0f,47.6f}, {83.4f,46.3f}, {83.0f,44.9f}, {82.7f,43.6f}, {82.5f,42.1f},
    {82.5f,40.7f}, {82.6f,39.3f}, {82.8f,37.9f}, {83.2f,36.5f}, {83.6f,35.2f}, {83.5f,35.1f},
    {81.4f,32.2f}, {79.0f,29.5f}, {76.6f,27.9f}, {74.2f,26.7f}, {71.8f,25.8f}, {69.3f,25.3f},
    {66.9f,25.0f}, {64.5f,25.1f}, {62.2f,25.4f}, {60.0f,26.0f}, {58.0f,26.8f}, {56.0f,27.8f},
    {54.2f,28.9f}, {52.6f,30.2f}, {51.2f,31.5f},
};
constexpr float kAppleLeaf[][2] = {
    {53.0f,25.0f}, {52.7f,22.7f}, {52.7f,20.5f}, {52.9f,18.4f}, {53.4f,16.4f}, {54.1f,14.5f},
    {55.0f,12.7f}, {56.1f,11.0f}, {57.5f,9.5f}, {59.0f,8.0f}, {60.7f,6.7f}, {62.5f,5.6f},
    {64.5f,4.6f}, {66.7f,3.7f}, {69.0f,3.0f}, {69.3f,5.3f}, {69.3f,7.6f}, {69.0f,9.9f},
    {68.5f,12.0f}, {67.7f,14.0f}, {66.7f,16.0f}, {65.5f,17.8f}, {64.1f,19.4f}, {62.6f,20.8f},
    {60.9f,22.1f}, {59.1f,23.2f}, {57.1f,24.0f}, {55.1f,24.7f},
};

std::vector<BYTE> RenderApplePixels(int px, bool darkLogo) {
    constexpr int SS = 4;
    const int W = px * SS;
    std::vector<uint8_t> cover((size_t)px * px, 0);
    auto fill = [&](const float (*pts)[2], size_t n) {
        for (int row = 0; row < W; row++) {
            float y = (row + 0.5f) * 100.0f / W;
            float xs[32];
            int nx = 0;
            for (size_t i = 0; i < n; i++) {
                float x0 = pts[i][0], y0 = pts[i][1];
                float x1 = pts[(i + 1) % n][0], y1 = pts[(i + 1) % n][1];
                if (((y0 <= y && y1 > y) || (y1 <= y && y0 > y)) && nx < 32) {
                    xs[nx++] = x0 + (y - y0) / (y1 - y0) * (x1 - x0);
                }
            }
            std::sort(xs, xs + nx);
            for (int j = 0; j + 1 < nx; j += 2) {
                int a = std::max(0, (int)std::ceil(xs[j] * W / 100.0f - 0.5f));
                int b = std::min(W, (int)std::ceil(xs[j + 1] * W / 100.0f - 0.5f));
                for (int x = a; x < b; x++) {
                    uint8_t& c = cover[(size_t)(row / SS) * px + x / SS];
                    if (c < 255) c++;
                }
            }
        }
    };
    fill(kAppleBody, std::size(kAppleBody));
    fill(kAppleLeaf, std::size(kAppleLeaf));

    std::vector<BYTE> out((size_t)px * px * 4);
    const int color = darkLogo ? 28 : 255;
    for (size_t i = 0; i < cover.size(); i++) {
        int a = std::min(255, cover[i] * 255 / (SS * SS));
        BYTE v = (BYTE)(color * a / 255);
        out[i * 4 + 0] = v;
        out[i * 4 + 1] = v;
        out[i * 4 + 2] = v;
        out[i * 4 + 3] = (BYTE)a;
    }
    return out;
}

constexpr wchar_t kAppleLogoName[] = L"MacDockAppleLogo";

struct AppleLogo {
    winrt::weak_ref<wux::FrameworkElement> button;
    winrt::weak_ref<wux::FrameworkElement> icon;
    winrt::weak_ref<wuxc::Panel> panel;
    winrt::weak_ref<wuxc::Image> image;
    int px = 0;
    bool dark = false;
};
[[clang::no_destroy]] std::optional<std::list<AppleLogo>> g_appleLogos{std::in_place};

double AppleLogoSize() { return std::round(g_iconSize * 0.78); }

int AppleLogoPx(wux::FrameworkElement const& el) {
    double dpi = 1;
    try {
        if (auto root = el.XamlRoot()) dpi = root.RasterizationScale();
    } catch (...) {}
    int px = (int)std::ceil(AppleLogoSize() * g_scale * dpi / 16.0) * 16;
    return std::clamp(px, 64, 256);
}

void UpdateAppleImage(AppleLogo& a, wux::FrameworkElement const& button) {
    auto image = a.image.get();
    if (!image) return;
    double size = AppleLogoSize();
    if (image.Width() != size) image.Width(size);
    if (image.Height() != size) image.Height(size);
    bool dark = g_dockTheme == 2;
    int px = AppleLogoPx(button);
    if (a.px == px && a.dark == dark) return;
    auto pixels = RenderApplePixels(px, dark);
    wuxm::Imaging::WriteableBitmap bitmap(px, px);
    BYTE* data = nullptr;
    winrt::check_hresult(bitmap.PixelBuffer().as<IDockBufferByteAccess>()->Buffer(&data));
    memcpy(data, pixels.data(), pixels.size());
    bitmap.Invalidate();
    image.Source(bitmap);
    a.px = px;
    a.dark = dark;
}

void RemoveAppleLogo(AppleLogo& a) {
    try {
        if (auto icon = a.icon.get()) UnhideHighlight(icon);
        auto panel = a.panel.get();
        auto image = a.image.get();
        uint32_t index = 0;
        if (panel && image && panel.Children().IndexOf(image, index)) panel.Children().RemoveAt(index);
    } catch (...) {}
}

void RemoveAppleLogos() {
    for (auto& a : *g_appleLogos) RemoveAppleLogo(a);
    g_appleLogos->clear();
}

void ApplyAppleLogo(wux::FrameworkElement const& button) {
    for (auto it = g_appleLogos->begin(); it != g_appleLogos->end();) {
        if (it->button.get() != button) { ++it; continue; }
        auto image = it->image.get();
        bool alive = image && wuxm::VisualTreeHelper::GetParent(image);
        if (g_appleLogo && alive) {
            UpdateAppleImage(*it, button);
            return;
        }
        RemoveAppleLogo(*it);
        it = g_appleLogos->erase(it);
    }
    if (!g_appleLogo) return;

    wux::FrameworkElement icon{nullptr};
    bool named = false;
    auto visit = [&](wux::FrameworkElement const& el) {
        if (el.Name() == kAppleLogoName) return false;
        auto type = winrt::get_class_name(el);
        bool isNamed = el.Name() == L"Icon";
        bool isGlyph = type == L"Windows.UI.Xaml.Controls.Image" ||
                       type == L"Windows.UI.Xaml.Shapes.Path" ||
                       type == L"Windows.UI.Xaml.Controls.FontIcon";
        if (isNamed && !named) {
            icon = el;
            named = true;
        } else if (!icon && isGlyph) {
            icon = el;
        }
        return true;
    };
    ForEachDescendant(button, 8, visit);
    static bool loggedMissing = false;
    if (!icon) {
        if (!loggedMissing) Wh_Log(L"[apple logo] couldn't find the Start button's icon");
        loggedMissing = true;
        return;
    }

    wuxc::Panel panel{nullptr};
    wux::DependencyObject current = icon;
    for (int i = 0; current && i < 8 && !panel; i++) {
        current = wuxm::VisualTreeHelper::GetParent(current);
        if (current) panel = current.try_as<wuxc::Panel>();
    }
    if (!panel) {
        if (!loggedMissing) Wh_Log(L"[apple logo] no panel around the Start button's icon");
        loggedMissing = true;
        return;
    }

    wuxc::Image image;
    image.Name(kAppleLogoName);
    image.Stretch(wuxm::Stretch::Uniform);
    image.HorizontalAlignment(wux::HorizontalAlignment::Center);
    image.VerticalAlignment(wux::VerticalAlignment::Center);
    image.IsHitTestVisible(false);
    if (auto grid = panel.try_as<wuxc::Grid>()) {
        wuxc::Grid::SetColumnSpan(image, (int)std::max<uint32_t>(1, grid.ColumnDefinitions().Size()));
        wuxc::Grid::SetRowSpan(image, (int)std::max<uint32_t>(1, grid.RowDefinitions().Size()));
    }
    panel.Children().Append(image);
    HideHighlight(icon);

    auto& a = g_appleLogos->emplace_back();
    a.button = winrt::make_weak(button);
    a.icon = winrt::make_weak(icon);
    a.panel = winrt::make_weak(panel);
    a.image = winrt::make_weak(image);
    UpdateAppleImage(a, button);
    Wh_Log(L"[apple logo] applied");
}

void ScanTaskbars() {
    g_frames.erase(std::remove_if(g_frames.begin(), g_frames.end(),
                                  [](auto& f) { return !f.get(); }),
                   g_frames.end());
    g_buttons->remove_if([](Button const& b) { return !b.element.get(); });
    g_iconImages->remove_if([](IconImage const& i) { return !i.element.get(); });
    g_indicators->remove_if([](Indicator const& i) { return !i.element.get(); });

    auto visit = [](wux::FrameworkElement const& el) {
        auto type = winrt::get_class_name(el);
        if (type == L"Taskbar.TaskListButton" || type == L"Taskbar.ExperienceToggleButton") {
            if (type == L"Taskbar.ExperienceToggleButton" &&
                wux::Automation::AutomationProperties::GetAutomationId(el) == L"StartButton") {
                HookLaunchpadStart(el);
                try { ApplyAppleLogo(el); } catch (...) { Wh_Log(L"[apple logo] failed: 0x%08X", winrt::to_hresult()); }
            }
            ProcessButton(el);
            return false;
        }
        if (type == L"Taskbar.TaskbarBackground") {
            HideBackground(el);
            return false;
        }
        return true;
    };
    for (auto& f : g_frames) {
        auto frame = f.get();
        if (!frame) continue;
        ForEachDescendant(frame, 12, visit);
        ScanTray(frame);
    }
}

void OnTaskbarPointerMoved(wux::FrameworkElement const& frame,
                           wuxi::PointerRoutedEventArgs const& e) {
    NoteTaskbarFrame(frame);
    Plate* plate = nullptr;
    for (auto& p : *g_plates) {
        if (p.frame.get() == frame) {
            plate = &p;
            break;
        }
    }
    wux::UIElement row{nullptr};
    bool over = false;
    if (plate && plate->width > 0 && plate->root.Visibility() == wux::Visibility::Visible) {
        row = plate->row.get();
        auto panel = plate->panel.get();
        if (row && panel) {
            auto point = e.GetCurrentPoint(panel).Position();
            over = point.X >= std::min(plate->left, plate->baseLeft) &&
                   point.X <= std::max(plate->left + plate->width,
                                       plate->baseLeft + plate->baseWidth) &&
                   (point.Y >= plate->top || (g_pointerOver && g_rowParent.get() == row));
        }
    }
    if (!over) {
        if (g_pointerOver) {
            g_pointerOver = false;
            StartFrames();
        }
        return;
    }
    if (g_rowParent.get() != row) BuildRow(row);
    g_pointerX = e.GetCurrentPoint(row).Position().X;
    g_pointerOver = true;
    StartFrames();
}

void OnTaskbarPointerExited() {
    g_pointerOver = false;
    StartFrames();
}

void RemoveAppBars();
void ClipTaskbars();
void SwipeBarDestroy();

void ResetAll() {
    StopFrames();
    if (g_launchpadStartHooks) g_launchpadStartHooks->clear();
    LaunchpadShutdown();
    try { RemovePlates(); } catch (...) {}
    try { StopTrash(); } catch (...) {}
    RestoreBackgrounds();
    RemoveAppleLogos();
    RestoreHighlights();
    RestoreIconImages();
    for (auto& b : *g_buttons) {
        if (auto el = b.element.get()) {
            try {
                ReleaseButton(b);
                SetScale(el, 1.0f, 0);
                wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, false);
            } catch (...) {}
        }
    }
    SwipeBarDestroy();
    for (auto& f : g_frames) {
        if (auto frame = f.get()) {
            try {
                auto visual = wuxh::ElementCompositionPreview::GetElementVisual(frame);
                visual.StopAnimation(L"Opacity");
                visual.Opacity(1.0f);
                visual.Properties().StopAnimation(L"Translation.Y");
                visual.Properties().InsertVector3(L"Translation", {0, 0, 0});
                wuxh::ElementCompositionPreview::SetIsTranslationEnabled(frame, false);
            } catch (...) {}
        }
    }
    RestoreIndicators();
    RestoreTrayRows();
    RemoveAppBars();
    ClipTaskbars();
    if (HWND w = g_modWindow.exchange(nullptr)) DestroyWindow(w);
    g_row.clear();
    g_rowParent = nullptr;
    g_frames.clear();
    g_buttons.reset();
    g_iconImages.reset();
    g_plates.reset();
    g_indicators.reset();
    g_trayRows.reset();
    g_appleLogos.reset();
}

using RunProc_t = void (*)(void*);

bool RunFromWindowThread(HWND hWnd, RunProc_t proc, void* param) {
    static const UINT msg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param { RunProc_t proc; void* param; };

    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) { proc(param); return true; }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int code, WPARAM w, LPARAM l) -> LRESULT {
        if (code == HC_ACTION) {
            auto* cwp = (const CWPSTRUCT*)l;
            if (cwp->message == msg) {
                auto* p = (Param*)cwp->lParam;
                p->proc(p->param);
            }
        }
        return CallNextHookEx(nullptr, code, w, l);
    }, nullptr, tid);
    if (!hook) return false;

    Param p{proc, param};
    SendMessage(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

HWND FindTaskbar() {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM l) -> BOOL {
        DWORD pid; WCHAR cls[32];
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassName(hWnd, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
            *(HWND*)l = hWnd;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&result);
    return result;
}

std::atomic<bool> g_unloading = false;

wux::FrameworkElement ElementFromThis(void* pThis) {
    winrt::Windows::Foundation::IUnknown unknown;
    winrt::copy_from_abi(unknown, (void**)pThis + 3);
    return unknown.try_as<wux::FrameworkElement>();
}

void OnTaskListButtonUpdated(void* pThis) {
    if (g_unloading) return;
    try {
        if (auto button = ElementFromThis(pThis)) {
            ProcessButton(button);
            NoteTaskbarFrame(FindTaskbarFrame(button));
        }
    } catch (...) {
        Wh_Log(L"Button update failed: 0x%08X", winrt::to_hresult());
    }
}

using TaskbarFrame_OnPointer_t = int(WINAPI*)(void* pThis, void* pArgs);

wux::FrameworkElement TaskbarFrameFromThis(void* pThis) {
    wux::FrameworkElement element{nullptr};
    ((::IUnknown*)pThis)->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                         winrt::put_abi(element));
    if (!element || winrt::get_class_name(element) != L"Taskbar.TaskbarFrame") return nullptr;
    return element;
}

TaskbarFrame_OnPointer_t TaskbarFrame_OnPointerMoved_Original;
int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    int result = TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    if (g_unloading) return result;
    try {
        if (auto frame = TaskbarFrameFromThis(pThis)) {
            wuxi::PointerRoutedEventArgs args{nullptr};
            ((::IUnknown*)pArgs)->QueryInterface(
                winrt::guid_of<wuxi::PointerRoutedEventArgs>(), winrt::put_abi(args));
            if (args) OnTaskbarPointerMoved(frame, args);
        }
    } catch (...) {}
    return result;
}

TaskbarFrame_OnPointer_t TaskbarFrame_OnPointerExited_Original;
int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    int result = TaskbarFrame_OnPointerExited_Original(pThis, pArgs);
    if (g_unloading) return result;
    try {
        if (TaskbarFrameFromThis(pThis)) OnTaskbarPointerExited();
    } catch (...) {}
    return result;
}

bool g_placingJumpList = false;
bool g_hasJumpListButton = false;
wf::Rect g_jumpListButton{};

template <typename F>
void OpenJumpListFor(wux::UIElement const& sender, F original) {
    g_hasJumpListButton = false;
    if (!g_unloading) {
        try {
            auto button = sender.try_as<wux::FrameworkElement>();
            auto root = button ? button.XamlRoot() : nullptr;
            auto content = root ? root.Content() : nullptr;
            if (content && FindButton(button)) {
                g_jumpListButton = button.TransformToVisual(content).TransformBounds(
                    {0, 0, (float)button.ActualWidth(), (float)button.ActualHeight()});
                g_hasJumpListButton = true;
            }
        } catch (...) {}
    }
    original();
    g_hasJumpListButton = false;
}

using TaskbarResources_OnTaskListButtonContextRequested_t =
    void(WINAPI*)(void* pThis, wux::UIElement const& sender, void* args);
TaskbarResources_OnTaskListButtonContextRequested_t
    TaskbarResources_OnTaskListButtonContextRequested_Original;
void WINAPI TaskbarResources_OnTaskListButtonContextRequested_Hook(
    void* pThis, wux::UIElement const& sender, void* args) {
    OpenJumpListFor(sender, [&] {
        TaskbarResources_OnTaskListButtonContextRequested_Original(pThis, sender, args);
    });
}

using TaskListButtonHandlers_HandleContextRequested_t =
    void(WINAPI*)(wux::UIElement const& sender, void* args);
TaskListButtonHandlers_HandleContextRequested_t
    TaskListButtonHandlers_HandleContextRequested_Original;
void WINAPI TaskListButtonHandlers_HandleContextRequested_Hook(wux::UIElement const& sender,
                                                              void* args) {
    OpenJumpListFor(sender, [&] {
        TaskListButtonHandlers_HandleContextRequested_Original(sender, args);
    });
}

using CTaskListWnd_ComputeJumpViewPosition_t =
    HRESULT(WINAPI*)(void* pThis, void* taskBtnGroup, int param2, wf::Point* point,
                     wux::HorizontalAlignment* horizontal, wux::VerticalAlignment* vertical);
CTaskListWnd_ComputeJumpViewPosition_t CTaskListWnd_ComputeJumpViewPosition_Original;
HRESULT WINAPI CTaskListWnd_ComputeJumpViewPosition_Hook(void* pThis, void* taskBtnGroup,
                                                         int param2, wf::Point* point,
                                                         wux::HorizontalAlignment* horizontal,
                                                         wux::VerticalAlignment* vertical) {
    g_placingJumpList = true;
    HRESULT hr = CTaskListWnd_ComputeJumpViewPosition_Original(pThis, taskBtnGroup, param2,
                                                               point, horizontal, vertical);
    g_placingJumpList = false;
    return hr;
}

using ToRawPixelsInDesktopCoordinates_t = RECT(WINAPI*)(wf::Rect const& rect, UINT dpi,
                                                         HWND hWnd);
ToRawPixelsInDesktopCoordinates_t ToRawPixelsInDesktopCoordinates_Original;
RECT WINAPI ToRawPixelsInDesktopCoordinates_Hook(wf::Rect const& rect, UINT dpi, HWND hWnd) {
    if (g_placingJumpList && g_hasJumpListButton && !g_unloading) {
        wf::Rect moved = rect;
        moved.X = g_jumpListButton.X;
        moved.Width = g_jumpListButton.Width;
        return ToRawPixelsInDesktopCoordinates_Original(moved, dpi, hWnd);
    }
    return ToRawPixelsInDesktopCoordinates_Original(rect, dpi, hWnd);
}

winrt::weak_ref<wux::FrameworkElement> g_blockedDrag;

bool IsBlockedDrag(void* pThis) {
    try {
        auto blocked = g_blockedDrag.get();
        return blocked && ElementFromThis(pThis) == blocked;
    } catch (...) {
        return false;
    }
}

using TaskListButton_DragPoint_t = void(WINAPI*)(void* pThis, wf::Point point);
using TaskListButton_OnDragCompletedGesture_t = void(WINAPI*)(void* pThis);
TaskListButton_DragPoint_t TaskListButton_OnDragStartedGesture_Original;
void WINAPI TaskListButton_OnDragStartedGesture_Hook(void* pThis, wf::Point point) {
    if (!g_unloading) {
        try {
            auto button = ElementFromThis(pThis);
            if (button && IsFileExplorer(button)) {
                g_blockedDrag = winrt::make_weak(button);
                return;
            }
        } catch (...) {}
    }
    TaskListButton_OnDragStartedGesture_Original(pThis, point);
}

TaskListButton_DragPoint_t TaskListButton_UpdateDrag_Original;
void WINAPI TaskListButton_UpdateDrag_Hook(void* pThis, wf::Point point) {
    if (IsBlockedDrag(pThis)) return;
    TaskListButton_UpdateDrag_Original(pThis, point);
}

TaskListButton_OnDragCompletedGesture_t TaskListButton_OnDragCompletedGesture_Original;
void WINAPI TaskListButton_OnDragCompletedGesture_Hook(void* pThis) {
    if (IsBlockedDrag(pThis)) {
        g_blockedDrag = nullptr;
        return;
    }
    TaskListButton_OnDragCompletedGesture_Original(pThis);
    if (!g_unloading) MarkPlatesDirty();
}

std::atomic<bool> g_applyingHeight = false;

bool OverrideHeight(int enumTaskbarSize) {
    return g_taskbarHeight > 0 && (enumTaskbarSize == 1 || enumTaskbarSize == 2);
}

using VoidMethod_t = void(WINAPI*)(void* pThis);
using SetDouble_t = void(WINAPI*)(void* pThis, double value);

using TaskbarConfiguration_GetFrameSize_t = double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    if (OverrideHeight(enumTaskbarSize)) return g_taskbarHeight;
    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}

using SystemTray_GetFrameSize_t = double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTray_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis, int enumTaskbarSize) {
    if (OverrideHeight(enumTaskbarSize)) return g_taskbarHeight;
    return SystemTrayController_GetFrameSize_Original(pThis, enumTaskbarSize);
}

SystemTray_GetFrameSize_t SystemTraySecondaryController_GetFrameSize_Original;
double WINAPI SystemTraySecondaryController_GetFrameSize_Hook(void* pThis, int enumTaskbarSize) {
    if (OverrideHeight(enumTaskbarSize)) return g_taskbarHeight;
    return SystemTraySecondaryController_GetFrameSize_Original(pThis, enumTaskbarSize);
}

thread_local bool g_inTrayUpdateFrameSize = false;

VoidMethod_t SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    g_inTrayUpdateFrameSize = true;
    SystemTrayController_UpdateFrameSize_Original(pThis);
    g_inTrayUpdateFrameSize = false;
}

VoidMethod_t SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    g_inTrayUpdateFrameSize = true;
    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);
    g_inTrayUpdateFrameSize = false;
}

SetDouble_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    if (g_inTrayUpdateFrameSize && g_taskbarHeight > 0) value = g_taskbarHeight;
    SystemTrayFrame_Height_Original(pThis, value);
}

SetDouble_t TaskbarFrame_MaxHeight_Original;

SetDouble_t TaskbarFrame_Height_Original;
void WINAPI TaskbarFrame_Height_Hook(void* pThis, double value) {
    if (TaskbarFrame_MaxHeight_Original) {
        TaskbarFrame_MaxHeight_Original(pThis, std::numeric_limits<double>::infinity());
    }
    TaskbarFrame_Height_Original(pThis, value);
}

void* TaskbarController_OnGroupingModeChanged_Address;

LONG GetTaskbarFrameOffset() {
    static LONG offset = []() -> LONG {
        const BYTE* p = (const BYTE*)TaskbarController_OnGroupingModeChanged_Address;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B && (p[6] & 0xC0) == 0x80) {
            LONG o = *(LONG*)(p + 7);
            if (o > 0 && o <= 0xFFFF) return o;
        }
        Wh_Log(L"Taskbar frame offset not found");
        return 0;
    }();
    return offset;
}

VoidMethod_t TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    wux::FrameworkElement frame{nullptr};
    try {
        LONG offset = GetTaskbarFrameOffset();
        void* taskbarFrame = offset ? *(void**)((BYTE*)pThis + offset) : nullptr;
        if (taskbarFrame) {
            ((::IUnknown**)taskbarFrame)[1]->QueryInterface(
                winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(frame));
        }
        if (frame) frame.MaxHeight(std::numeric_limits<double>::infinity());
    } catch (...) {}

    TaskbarController_UpdateFrameHeight_Original(pThis);

    if (!frame) return;
    if (!g_unloading) {
        try { NoteTaskbarFrame(frame); } catch (...) {}
    }

    try {
        auto grid = wuxm::VisualTreeHelper::GetParent(frame).try_as<wux::FrameworkElement>();
        if (grid) {
            double height = frame.Height();
            double gridHeight = grid.Height();
            if (gridHeight > 0 && gridHeight != height) grid.Height(height);
        }
    } catch (...) {}
}

thread_local bool g_inTaskbarFrameGetMetrics = false;

using GetIconHeightInViewPixels_t = double(WINAPI*)(void* pThis);
GetIconHeightInViewPixels_t TaskbarConfiguration_GetIconHeightInViewPixels_Original;
double WINAPI TaskbarConfiguration_GetIconHeightInViewPixels_Hook(void* pThis) {
    double height = TaskbarConfiguration_GetIconHeightInViewPixels_Original(pThis);
    if (g_iconLoadSize <= 0 || g_inTaskbarFrameGetMetrics || height <= 16) return height;
    return g_iconLoadSize;
}

using TaskbarFrame_GetMetrics_t = void*(WINAPI*)(void* pThis, void* metrics);
TaskbarFrame_GetMetrics_t TaskbarFrame_GetMetrics_Original;
void* WINAPI TaskbarFrame_GetMetrics_Hook(void* pThis, void* metrics) {
    g_inTaskbarFrameGetMetrics = true;
    void* ret = TaskbarFrame_GetMetrics_Original(pThis, metrics);
    g_inTaskbarFrameGetMetrics = false;
    return ret;
}

void* TaskListButton_IconHeight_Address;

size_t GetIconHeightOffset() {
    static size_t offset = []() -> size_t {
        BYTE* p = (BYTE*)TaskListButton_IconHeight_Address;
        std::regex regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                         std::regex_constants::icase);
        for (int i = 0; p && i < 30; i++) {
            WH_DISASM_RESULT result;
            if (!Wh_Disasm(p, &result)) break;
            p += result.length;
            std::string_view text = result.text;
            if (text == "ret") break;
            std::match_results<std::string_view::const_iterator> match;
            if (std::regex_match(text.begin(), text.end(), match, regex)) {
                size_t o = 0;
                std::from_chars(&*match[1].first, &*match[1].first + match[1].length(), o, 16);
                Wh_Log(L"Icon height offset: 0x%zX", o);
                return o <= 0xFFFF ? o : 0;
            }
        }
        Wh_Log(L"Icon height offset not found; icon resolution unchanged");
        return 0;
    }();
    return offset;
}

template <typename F>
void WithStockIconHeight(void* pThis, F original) {
    size_t offset = g_iconLoadSize > 0 ? GetIconHeightOffset() : 0;
    if (!offset) return original();
    double* iconHeight = (double*)((BYTE*)pThis + offset);
    double saved = *iconHeight;
    *iconHeight = kStockIconSize;
    original();
    *iconHeight = saved;
}

#define STOCK_ICON_HEIGHT_HOOK(name)                                              \
    VoidMethod_t TaskListButton_##name##_Original;                                \
    void WINAPI TaskListButton_##name##_Hook(void* pThis) {                       \
        WithStockIconHeight(pThis, [=] { TaskListButton_##name##_Original(pThis); }); \
    }

STOCK_ICON_HEIGHT_HOOK(UpdateMultiWindowClip)
STOCK_ICON_HEIGHT_HOOK(CreateMultiWindowClip)
STOCK_ICON_HEIGHT_HOOK(UpdateBadge)

VoidMethod_t TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    WithStockIconHeight(pThis, [=] { TaskListButton_UpdateButtonPadding_Original(pThis); });
    OnTaskListButtonUpdated(pThis);
}

VoidMethod_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    WithStockIconHeight(pThis, [=] { TaskListButton_UpdateVisualStates_Original(pThis); });
    OnTaskListButtonUpdated(pThis);
}

VoidMethod_t TaskListButton_UpdateIconColumnDefinition_Original;
void WINAPI TaskListButton_UpdateIconColumnDefinition_Hook(void* pThis) {
    TaskListButton_UpdateIconColumnDefinition_Original(pThis);
    OnTaskListButtonUpdated(pThis);
}

using TaskListButton_OverlayIcon_t = void(WINAPI*)(void* pThis, void* stream);
TaskListButton_OverlayIcon_t TaskListButton_OverlayIcon_Original;
void WINAPI TaskListButton_OverlayIcon_Hook(void* pThis, void* stream) {
    WithStockIconHeight(pThis, [=] { TaskListButton_OverlayIcon_Original(pThis, stream); });
}

SetDouble_t ExperienceToggleButton_IconHeight_Original;
void WINAPI ExperienceToggleButton_IconHeight_Hook(void* pThis, double height) {
    if (g_iconLoadSize > 0 && height > 16) height = g_iconSize;
    ExperienceToggleButton_IconHeight_Original(pThis, height);
}

using GetDouble_t = double(WINAPI*)(void* pThis);
GetDouble_t ExperienceToggleButton_IconHeight_get;
thread_local bool g_settingToggleIconHeight = false;

void SetToggleIconHeight(void* pThis, double height) {
    g_settingToggleIconHeight = true;
    ExperienceToggleButton_IconHeight_Original(pThis, height);
    g_settingToggleIconHeight = false;
}

VoidMethod_t ExperienceToggleButton_UpdateButtonPadding_Original;
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    if (g_settingToggleIconHeight) return;
    double height = kStockIconSize;
    if (g_iconLoadSize > 0 && ExperienceToggleButton_IconHeight_get &&
        ExperienceToggleButton_IconHeight_Original) {
        height = ExperienceToggleButton_IconHeight_get(pThis);
    }
    bool swap = height > 16 && height != kStockIconSize;
    if (swap) SetToggleIconHeight(pThis, kStockIconSize);
    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    if (swap) SetToggleIconHeight(pThis, height);
}

SetDouble_t TaskbarComponentHost_IconHeight_Original;
void WINAPI TaskbarComponentHost_IconHeight_Hook(void* pThis, double height) {
    if (g_iconLoadSize > 0 && height > 16) height = g_iconSize;
    TaskbarComponentHost_IconHeight_Original(pThis, height);
}

VoidMethod_t TrayUI__StuckTrayChange_Original;

using TrayUI__HandleSettingChange_t = void(WINAPI*)(void*, void*, void*, void*, void*);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis, void* p1, void* p2, void* p3,
                                             void* p4) {
    TrayUI__HandleSettingChange_Original(pThis, p1, p2, p3, p4);
    if (g_applyingHeight) TrayUI__StuckTrayChange_Original(pThis);
}

void ApplyTaskbarHeight() {
    HWND tb = FindTaskbar();
    if (!tb) return;
    g_applyingHeight = true;
    SendMessage(tb, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    g_applyingHeight = false;

    if (HWND rebar = FindWindowEx(tb, nullptr, L"ReBarWindow32", nullptr)) {
        if (HWND taskSw = FindWindowEx(rebar, nullptr, L"MSTaskSwWClass", nullptr)) {
            SendMessage(taskSw, 0x452, 3, 0);
        }
    }
}

constexpr UINT kTraySettingMessage = WM_USER + 0x1CA;
constexpr WPARAM kTraySettingAutoHideGet = 3;
constexpr WPARAM kTraySettingAutoHideSet = 4;
constexpr UINT_PTR kTrayHideTimer = 2;

bool g_managingAutoHide = false;
bool g_autoHideBefore = false;

std::wstring UserValueName(PCWSTR name) {
    static std::wstring sid = [] {
        std::wstring result;
        HANDLE token;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return result;
        alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
        DWORD size;
        PWSTR text;
        if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size) &&
            ConvertSidToStringSidW(((TOKEN_USER*)buffer)->User.Sid, &text)) {
            result = text;
            LocalFree(text);
        }
        CloseHandle(token);
        return result;
    }();
    return std::wstring(name) + L"_" + sid;
}

enum class SmartMode { off, intersected, maximized };
SmartMode g_smartMode = SmartMode::intersected;
bool g_smartForegroundOnly = false;
bool g_smartPrimaryOnly = false;

void* TrayUI_vftable_IInspectable = nullptr;
void* TrayUI_vftable_ITrayComponentHost = nullptr;
void* CSecondaryTray_vftable_ISecondaryTray = nullptr;

using TrayUI_GetStuckMonitor_t = HMONITOR(WINAPI*)(void* pThis);
TrayUI_GetStuckMonitor_t TrayUI_GetStuckMonitor_Original;
using CSecondaryTray_GetMonitor_t = HMONITOR(WINAPI*)(void* pThis);
CSecondaryTray_GetMonitor_t CSecondaryTray_GetMonitor_Original;
using TrayUI_GetStuckRectForMonitor_t = bool(WINAPI*)(void* pThis, HMONITOR hMonitor, RECT* rect);
TrayUI_GetStuckRectForMonitor_t TrayUI_GetStuckRectForMonitor_Original;
using TrayUI_Unhide_t = void(WINAPI*)(void* pThis, int trayUnhideFlags, int unhideRequest);
TrayUI_Unhide_t TrayUI_Unhide_Original;
using CSecondaryTray__Unhide_t = void(WINAPI*)(void* pThis, int trayUnhideFlags, int unhideRequest);
CSecondaryTray__Unhide_t CSecondaryTray__Unhide_Original;

using ViewCoordinator_ShouldTaskbarBeExpanded_t = bool(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, bool expanded);
ViewCoordinator_ShouldTaskbarBeExpanded_t ViewCoordinator_ShouldTaskbarBeExpanded_Original;
using ViewCoordinator_UpdateIsExpanded_t = void(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, int reason);
ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;

using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
using CSecondaryTray_v_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;

bool SmartAutoHideOn() {
    return g_smartMode != SmartMode::off && !g_unloading && TrayUI_WndProc_Original &&
           TrayUI_vftable_IInspectable && TrayUI_vftable_ITrayComponentHost &&
           TrayUI_GetStuckMonitor_Original && TrayUI_GetStuckRectForMonitor_Original &&
           TrayUI_Unhide_Original;
}

bool AutoHideEngaged() { return (g_magnifyOverWindows || SmartAutoHideOn()) && !g_unloading; }

constexpr WCHAR kSahPendingProp[] = L"Windhawk_UpdateTaskbarStatePendingTickCount_" WH_MOD_ID;
const UINT g_sahGetRectMsg = RegisterWindowMessage(L"Windhawk_GetTaskbarRect_" WH_MOD_ID);
const UINT g_sahUpdateMsg = RegisterWindowMessage(L"Windhawk_UpdateTaskbarState_" WH_MOD_ID);

std::mutex g_sahThreadMutex;
std::atomic<HANDLE> g_sahThread = nullptr;
std::unordered_map<void*, HWND> g_sahKeptShown;
std::unordered_map<HWND, void*> g_sahViewCoordinator;
UINT_PTR g_sahTimer = 0;
std::atomic<HWND> g_sahMultitasking = nullptr;
std::atomic<HWND> g_sahAltTab = nullptr;

#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#endif
#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

using IsWindowArranged_t = BOOL(WINAPI*)(HWND hwnd);
IsWindowArranged_t pIsWindowArranged;
using GetWindowBand_t = BOOL(WINAPI*)(HWND hWnd, PDWORD pdwBand);
GetWindowBand_t pGetWindowBand;

bool SahIsWindowCloaked(HWND hwnd) {
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &isCloaked, sizeof(isCloaked))) && isCloaked;
}

enum class MultitaskingViewType { None, WinTab, AltTab };

MultitaskingViewType SahGetMultitaskingViewType(HWND hWnd) {
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) return MultitaskingViewType::None;

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className)) ||
        _wcsicmp(className, L"XamlExplorerHostIslandWindow") != 0) {
        return MultitaskingViewType::None;
    }

    constexpr DWORD ZBID_IMMERSIVE_APPCHROME = 5;
    constexpr DWORD ZBID_SYSTEM_TOOLS = 16;
    DWORD band = 0;
    if (!pGetWindowBand || !pGetWindowBand(hWnd, &band)) return MultitaskingViewType::None;

    MultitaskingViewType type;
    if (band == ZBID_IMMERSIVE_APPCHROME) type = MultitaskingViewType::WinTab;
    else if (band == ZBID_SYSTEM_TOOLS) type = MultitaskingViewType::AltTab;
    else return MultitaskingViewType::None;

    HANDLE hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, dwThreadId);
    if (!hThread) return MultitaskingViewType::None;
    bool isMultitaskingView = false;
    PWSTR description = nullptr;
    if (SUCCEEDED(GetThreadDescription(hThread, &description)) && description) {
        isMultitaskingView = wcscmp(description, L"MultitaskingView") == 0;
        LocalFree(description);
    }
    CloseHandle(hThread);
    return isMultitaskingView ? type : MultitaskingViewType::None;
}

bool SahIsTaskbarWindow(HWND hWnd) {
    WCHAR cls[32];
    if (!GetClassName(hWnd, cls, ARRAYSIZE(cls))) return false;
    return _wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

HWND SahFindTaskbarWindows(std::unordered_set<HWND>* secondary) {
    secondary->clear();
    HWND hTaskbarWnd = FindTaskbar();
    if (!hTaskbarWnd) return nullptr;
    DWORD threadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!threadId) return nullptr;
    EnumThreadWindows(
        threadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR cls[32];
            if (GetClassName(hWnd, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
                ((std::unordered_set<HWND>*)lParam)->insert(hWnd);
            }
            return TRUE;
        },
        (LPARAM)secondary);
    return hTaskbarWnd;
}

void SahGetTaskbarRect(HMONITOR monitor, RECT* rect) {
    SetRectEmpty(rect);
    if (HWND hTaskbarWnd = FindTaskbar()) {
        SendMessage(hTaskbarWnd, g_sahGetRectMsg, (WPARAM)monitor, (LPARAM)rect);
    }
}

bool SahCanHideTaskbarForWindow(HWND hWnd, HMONITOR monitor, const MONITORINFO* monitorInfo,
                                const RECT* taskbarRect) {
    if (!IsWindowVisible(hWnd) || SahIsWindowCloaked(hWnd) || IsIconic(hWnd) ||
        (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
        return false;
    }
    if (hWnd == g_sahAltTab) return false;
    if (hWnd == GetShellWindow() || GetProp(hWnd, L"DesktopWindow")) return false;
    if (GetClassWord(hWnd, GCW_ATOM) == 32768) return false;
    if (hWnd == g_lpHwnd) return false;

    WINDOWPLACEMENT wp{.length = sizeof(WINDOWPLACEMENT)};
    if (GetWindowPlacement(hWnd, &wp) && wp.showCmd == SW_SHOWMAXIMIZED) {
        return MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) == monitor;
    }

    bool isWindowArranged = pIsWindowArranged && pIsWindowArranged(hWnd);
    if (isWindowArranged && MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) != monitor) return false;

    RECT windowRect{};
    DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect, sizeof(windowRect));

    if (EqualRect(&windowRect, &monitorInfo->rcMonitor)) return true;

    if (g_smartMode == SmartMode::intersected || (g_smartMode == SmartMode::maximized && isWindowArranged)) {
        RECT intersect;
        if (IntersectRect(&intersect, &windowRect, taskbarRect)) return true;
    }
    return false;
}

bool SahShouldKeepTaskbarShown(HWND hTaskbarWnd, HMONITOR monitor, bool shown) {
    if (g_smartPrimaryOnly && monitor != MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY)) return false;
    if (g_sahMultitasking) return true;

    MONITORINFO monitorInfo{.cbSize = sizeof(MONITORINFO)};
    GetMonitorInfo(monitor, &monitorInfo);
    RECT taskbarRect{};
    SahGetTaskbarRect(monitor, &taskbarRect);

    POINT cursor{};
    GetCursorPos(&cursor);
    if (MonitorFromPoint(cursor, MONITOR_DEFAULTTONULL) == monitor) {
        bool atEdge = cursor.y >= monitorInfo.rcMonitor.bottom - 2;
        bool overDock = shown && cursor.y >= taskbarRect.top;
        if (atEdge || overDock) return true;
    }

    if (g_smartForegroundOnly) {
        HWND hForeground = GetForegroundWindow();
        return !hForeground || !SahCanHideTaskbarForWindow(hForeground, monitor, &monitorInfo, &taskbarRect);
    }

    bool canHide = false;
    DWORD taskbarThreadId = GetCurrentThreadId();
    auto proc = [&](HWND hWnd) -> BOOL {
        if (GetWindowThreadProcessId(hWnd, nullptr) == taskbarThreadId) return TRUE;
        canHide = SahCanHideTaskbarForWindow(hWnd, monitor, &monitorInfo, &taskbarRect);
        return canHide ? FALSE : TRUE;
    };
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL { return (*(decltype(proc)*)lParam)(hWnd); },
        (LPARAM)&proc);
    return !canHide;
}

void* SahQueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) ptr = (void**)ptr + 1;
    return ptr;
}

void* SahQueryViaVtableBackwards(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) ptr = (void**)ptr - 1;
    return ptr;
}

DWORD WINAPI SahEventThread(LPVOID);

void SahAdjustTaskbar(HWND hTaskbarWnd, bool clearPendingWhenDone = false) {
    if (SmartAutoHideOn() && !g_sahThread) {
        std::lock_guard<std::mutex> guard(g_sahThreadMutex);
        if (!g_sahThread) g_sahThread = CreateThread(nullptr, 0, SahEventThread, nullptr, 0, nullptr);
    }
    PostMessage(hTaskbarWnd, g_sahUpdateMsg, clearPendingWhenDone, 0);
}

void SahAdjustAllTaskbars() {
    std::unordered_set<HWND> secondary;
    if (HWND hWnd = SahFindTaskbarWindows(&secondary)) SahAdjustTaskbar(hWnd);
    for (HWND h : secondary) SahAdjustTaskbar(h);
}

bool SahAdjustAllTaskbarsIfNotPending() {
    std::unordered_set<HWND> secondary;
    HWND hWnd = SahFindTaskbarWindows(&secondary);
    DWORD currentTick = GetTickCount();
    DWORD pendingTick = 0;
    auto check = [&](HWND w) {
        DWORD t = (DWORD)(DWORD_PTR)GetProp(w, kSahPendingProp);
        if (t > pendingTick) pendingTick = t;
    };
    if (hWnd) check(hWnd);
    for (HWND h : secondary) check(h);

    if (pendingTick && currentTick - pendingTick < 1000 * 10) return false;

    auto mark = [&](HWND w) {
        SetProp(w, kSahPendingProp, (HANDLE)(DWORD_PTR)currentTick);
        SahAdjustTaskbar(w, true);
    };
    if (hWnd) mark(hWnd);
    for (HWND h : secondary) mark(h);
    return true;
}

void SahStopEventThread() {
    std::lock_guard<std::mutex> guard(g_sahThreadMutex);
    if (HANDLE thread = g_sahThread.exchange(nullptr)) {
        PostThreadMessage(GetThreadId(thread), WM_APP, 0, 0);
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(void* pThis, HWND hMMTaskbarWnd, bool expanded) {
    if (!g_unloading) {
        g_sahViewCoordinator[hMMTaskbarWnd] = pThis;
        if (SmartAutoHideOn()) {
            for (const auto& pair : g_sahKeptShown) {
                if (pair.second == hMMTaskbarWnd) return true;
            }
        }
    }
    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(pThis, hMMTaskbarWnd, expanded);
}

void SahUpdateViewCoordinatorIsExpanded(HWND hWnd) {
    constexpr int kReasonIsPointerOverTaskbarFrameChanged = 7;
    if (ViewCoordinator_UpdateIsExpanded_Original) {
        auto it = g_sahViewCoordinator.find(hWnd);
        if (it != g_sahViewCoordinator.end()) {
            ViewCoordinator_UpdateIsExpanded_Original(it->second, hWnd, kReasonIsPointerOverTaskbarFrameChanged);
        }
    }
}

bool g_indicatorOn = true;
bool g_dockFade = true;

constexpr int kDockHideShowMs = 220;
constexpr int kDockHideMs = 170;
constexpr int kDockHideDelayMs = 190;

constexpr wchar_t kSwipeBarClass[] = L"Windhawk_" WH_MOD_ID L"_SwipeBar";
constexpr int kSwipeBarMaxAlpha = 120;

struct SwipeBar {
    HWND hwnd = nullptr;
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ old = nullptr;
    int w = 0, h = 0, x = 0, y = 0;
    float value = 0, target = 0;
    ULONGLONG last = 0, delayUntil = 0;
    bool classRegistered = false;
};
SwipeBar g_bar;

LRESULT CALLBACK SwipeBarProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void SwipeBarApply() {
    if (!g_bar.hwnd || !g_bar.dc) return;
    POINT pos{g_bar.x, g_bar.y}, src{0, 0};
    SIZE size{g_bar.w, g_bar.h};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, (BYTE)std::clamp((int)std::lround(g_bar.value * kSwipeBarMaxAlpha), 0, 255), AC_SRC_ALPHA};
    UpdateLayeredWindow(g_bar.hwnd, nullptr, &pos, &size, g_bar.dc, &src, 0, &blend, ULW_ALPHA);
}

bool SwipeBarBuild(int w, int h) {
    HDC screen = GetDC(nullptr);
    g_bar.dc = CreateCompatibleDC(screen);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    g_bar.bitmap = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!g_bar.dc || !g_bar.bitmap || !bits) return false;
    g_bar.old = SelectObject(g_bar.dc, g_bar.bitmap);
    float r = h / 2.0f;
    uint32_t* px = (uint32_t*)bits;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float dx = std::max({r - (x + 0.5f), (x + 0.5f) - (w - r), 0.0f});
            float dy = (y + 0.5f) - r;
            float cover = std::clamp(r + 0.5f - std::sqrt(dx * dx + dy * dy), 0.0f, 1.0f);
            uint32_t a = (uint32_t)(cover * 255);
            px[(size_t)y * w + x] = (a << 24) | (a << 16) | (a << 8) | a;
        }
    }
    g_bar.w = w;
    g_bar.h = h;
    return true;
}

void SwipeBarDestroy() {
    if (g_bar.hwnd) DestroyWindow(g_bar.hwnd);
    if (g_bar.dc) {
        if (g_bar.old) SelectObject(g_bar.dc, g_bar.old);
        DeleteDC(g_bar.dc);
    }
    if (g_bar.bitmap) DeleteObject(g_bar.bitmap);
    bool reg = g_bar.classRegistered;
    g_bar = SwipeBar();
    if (reg) UnregisterClassW(kSwipeBarClass, ModuleInstance());
}

bool SwipeBarEnsure() {
    if (g_bar.hwnd) return true;
    HWND tb = FindTaskbar();
    if (!tb) return false;
    if (!g_bar.classRegistered) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = SwipeBarProc;
        wc.hInstance = ModuleInstance();
        wc.lpszClassName = kSwipeBarClass;
        if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;
        g_bar.classRegistered = true;
    }
    HMONITOR monitor = MonitorFromWindow(tb, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) return false;
    float s = GetDpiForWindow(tb) / 96.0f;
    int w = (int)std::lround(110 * s), h = std::max(3, (int)std::lround(4 * s));
    if (!SwipeBarBuild(w, h)) { SwipeBarDestroy(); return false; }
    g_bar.x = (mi.rcMonitor.left + mi.rcMonitor.right - w) / 2;
    g_bar.y = mi.rcMonitor.bottom - (int)std::lround(6 * s) - h;
    g_bar.hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                                 kSwipeBarClass, L"", WS_POPUP, g_bar.x, g_bar.y, w, h,
                                 nullptr, nullptr, ModuleInstance(), nullptr);
    if (!g_bar.hwnd) { SwipeBarDestroy(); return false; }
    SwipeBarApply();
    return true;
}

LRESULT CALLBACK SwipeBarProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_TIMER: {
            ULONGLONG now = GetTickCount64();
            if (now < g_bar.delayUntil) return 0;
            float dt = std::min(0.05f, (now - g_bar.last) / 1000.0f);
            g_bar.last = now;
            float speed = g_bar.target > g_bar.value ? 1 / 0.25f : 1 / 0.12f;
            if (g_bar.target > g_bar.value) g_bar.value = std::min(g_bar.target, g_bar.value + dt * speed);
            else g_bar.value = std::max(g_bar.target, g_bar.value - dt * speed);
            SwipeBarApply();
            if (g_bar.value == g_bar.target) {
                KillTimer(hwnd, 1);
                if (g_bar.value <= 0) ShowWindow(hwnd, SW_HIDE);
            }
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool SwipeBarForegroundFullscreen() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == GetShellWindow()) return false;
    RECT wr;
    if (!GetWindowRect(fg, &wr)) return false;
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfo(MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST), &mi)) return false;
    return EqualRect(&wr, &mi.rcMonitor) && !(GetWindowLong(fg, GWL_STYLE) & WS_CAPTION);
}

void SwipeBarSet(bool show) {
    if (!g_indicatorOn || g_unloading || SwipeBarForegroundFullscreen()) show = false;
    if (!show && !g_bar.hwnd) return;
    if (!SwipeBarEnsure()) return;
    g_bar.target = show ? 1.0f : 0.0f;
    g_bar.last = GetTickCount64();
    g_bar.delayUntil = show ? g_bar.last + 160 + (g_dockFade ? kDockHideDelayMs : 0) : 0;
    if (show && !IsWindowVisible(g_bar.hwnd)) {
        SwipeBarApply();
        ShowWindow(g_bar.hwnd, SW_SHOWNOACTIVATE);
    }
    SetTimer(g_bar.hwnd, 1, 15, nullptr);
}

void FadeDock(bool show) {
    if (!g_dockFade || g_unloading) return;
    float distance = g_dockBottomGap + g_dockHeight + 10.0f;
    for (auto& f : g_frames) {
        auto frame = f.get();
        if (!frame) continue;
        try {
            auto visual = wuxh::ElementCompositionPreview::GetElementVisual(frame);
            auto compositor = visual.Compositor();
            wuxh::ElementCompositionPreview::SetIsTranslationEnabled(frame, true);
            auto ease = compositor.CreateCubicBezierEasingFunction({0.2f, 0.0f}, {0.0f, 1.0f});
            auto duration = std::chrono::milliseconds(show ? kDockHideShowMs : kDockHideMs);

            auto fade = compositor.CreateScalarKeyFrameAnimation();
            fade.InsertKeyFrame(1.0f, show ? 1.0f : 0.0f, ease);
            fade.Duration(duration);
            visual.StartAnimation(L"Opacity", fade);

            auto slide = compositor.CreateScalarKeyFrameAnimation();
            slide.InsertKeyFrame(1.0f, show ? 0.0f : distance, ease);
            slide.Duration(duration);
            visual.Properties().StartAnimation(L"Translation.Y", slide);
        } catch (...) {}
    }
}

template <typename F>
void SahApplyKeepShown(HWND hWnd, void* key, bool keepShown, F unhide) {
    bool keptShown = g_sahKeptShown.contains(key);
    if (keepShown == keptShown) return;
    Wh_Log(L"[smart auto-hide] keepShown=%d", keepShown);
    if (keepShown) {
        g_sahKeptShown[key] = hWnd;
        unhide();
    } else {
        g_sahKeptShown.erase(key);
        SetTimer(hWnd, kTrayHideTimer, g_dockFade ? kDockHideDelayMs : 0, nullptr);
    }
    SahUpdateViewCoordinatorIsExpanded(hWnd);
    if (hWnd == FindTaskbar()) {
        FadeDock(keepShown);
        SwipeBarSet(!keepShown);
    }
}

LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag) {
    if (Msg == WM_NCCREATE) {
        if (SmartAutoHideOn()) SahAdjustTaskbar(hWnd);
    } else if (Msg == WM_NCDESTROY) {
        if (TrayUI_vftable_IInspectable) {
            g_sahKeptShown.erase(SahQueryViaVtableBackwards(pThis, TrayUI_vftable_IInspectable));
        }
        g_sahViewCoordinator.erase(hWnd);
    } else if (Msg == kTraySettingMessage) {
        if (wParam == kTraySettingAutoHideSet && !(BOOL)lParam && SmartAutoHideOn()) return 0;
    } else if (Msg == g_sahGetRectMsg) {
        RECT* rect = (RECT*)lParam;
        if (TrayUI_GetStuckRectForMonitor_Original &&
            TrayUI_GetStuckRectForMonitor_Original(pThis, (HMONITOR)wParam, rect)) {
        } else {
            SetRectEmpty(rect);
        }
    } else if (Msg == g_sahUpdateMsg && SmartAutoHideOn()) {
        HMONITOR monitor = TrayUI_GetStuckMonitor_Original(pThis);
        void* key = SahQueryViaVtableBackwards(pThis, TrayUI_vftable_IInspectable);
        bool keepShown = SahShouldKeepTaskbarShown(hWnd, monitor, g_sahKeptShown.contains(key));
        SahApplyKeepShown(hWnd, key, keepShown, [&] {
            void* host = SahQueryViaVtable(pThis, TrayUI_vftable_ITrayComponentHost);
            TrayUI_Unhide_Original(host, 0, 0);
        });
        if (wParam) RemoveProp(hWnd, kSahPendingProp);
    }
    return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
}

LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_NCCREATE) {
        if (SmartAutoHideOn()) SahAdjustTaskbar(hWnd);
    } else if (Msg == WM_NCDESTROY) {
        g_sahKeptShown.erase(pThis);
        g_sahViewCoordinator.erase(hWnd);
    } else if (Msg == g_sahUpdateMsg && SmartAutoHideOn() && CSecondaryTray_vftable_ISecondaryTray &&
               CSecondaryTray_GetMonitor_Original && CSecondaryTray__Unhide_Original) {
        void* iface = SahQueryViaVtable(pThis, CSecondaryTray_vftable_ISecondaryTray);
        HMONITOR monitor = CSecondaryTray_GetMonitor_Original(iface);
        bool keepShown = SahShouldKeepTaskbarShown(hWnd, monitor, g_sahKeptShown.contains(pThis));
        SahApplyKeepShown(hWnd, pThis, keepShown, [&] { CSecondaryTray__Unhide_Original(pThis, 0, 0); });
        if (wParam) RemoveProp(hWnd, kSahPendingProp);
    }
    return CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
}

void CALLBACK SahWinEventProc(HWINEVENTHOOK, DWORD event, HWND hWnd, LONG idObject, LONG, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) || SahIsTaskbarWindow(hWnd)) {
        return;
    }
    HWND hParent = GetAncestor(hWnd, GA_PARENT);
    if (hParent && hParent != GetDesktopWindow()) return;

    {
        auto type = SahGetMultitaskingViewType(hWnd);
        bool trackedWin = hWnd == g_sahMultitasking;
        bool trackedAlt = hWnd == g_sahAltTab;
        if (type != MultitaskingViewType::None || trackedWin || trackedAlt) {
            auto& target = (type == MultitaskingViewType::AltTab || trackedAlt) ? g_sahAltTab : g_sahMultitasking;
            bool entering = event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_UNCLOAKED || event == EVENT_OBJECT_CREATE;
            bool leaving = event == EVENT_OBJECT_HIDE || event == EVENT_OBJECT_CLOAKED || event == EVENT_OBJECT_DESTROY;
            if (entering) {
                HWND expected = nullptr;
                if (!target.compare_exchange_strong(expected, hWnd)) return;
            } else if (leaving) {
                HWND expected = hWnd;
                if (!target.compare_exchange_strong(expected, nullptr)) return;
            } else {
                return;
            }
        }
    }

    if (g_sahTimer) return;
    g_sahTimer = SetTimer(nullptr, 0, 200, [](HWND, UINT, UINT_PTR, DWORD) {
        if (!SahAdjustAllTaskbarsIfNotPending()) return;
        KillTimer(nullptr, g_sahTimer);
        g_sahTimer = 0;
    });
}

bool g_sahLastZone = false;

DWORD WINAPI SahEventThread(LPVOID) {
    HWINEVENTHOOK hooks[4] = {};
    hooks[0] = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE, nullptr, SahWinEventProc, 0, 0,
                               WINEVENT_OUTOFCONTEXT);
    hooks[1] = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, SahWinEventProc,
                               0, 0, WINEVENT_OUTOFCONTEXT);
    hooks[2] = SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr, SahWinEventProc, 0, 0,
                               WINEVENT_OUTOFCONTEXT);
    if (g_smartForegroundOnly) {
        hooks[3] = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, SahWinEventProc, 0, 0,
                                   WINEVENT_OUTOFCONTEXT);
    }

    UINT_PTR pollTimer = SetTimer(nullptr, 0, 120, [](HWND, UINT, UINT_PTR, DWORD) {
        POINT pt;
        GetCursorPos(&pt);
        MONITORINFO mi{sizeof(mi)};
        if (!GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi)) return;
        bool zone = pt.y >= mi.rcMonitor.bottom - 120;
        if (zone != g_sahLastZone) {
            g_sahLastZone = zone;
            SahAdjustAllTaskbarsIfNotPending();
        }
    });

    BOOL ret;
    MSG msg;
    while ((ret = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (ret == -1) break;
        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (pollTimer) KillTimer(nullptr, pollTimer);
    for (HWINEVENTHOOK hook : hooks) {
        if (hook) UnhookWinEvent(hook);
    }
    if (g_sahTimer) {
        KillTimer(nullptr, g_sahTimer);
        g_sahTimer = 0;
    }
    g_sahMultitasking = nullptr;
    g_sahAltTab = nullptr;
    return 0;
}

bool KeepTaskbarShown() { return g_magnifyOverWindows && !SmartAutoHideOn() && !g_unloading; }

void ApplyAutoHide() {
    HWND tb = FindTaskbar();
    if (!tb) return;
    if (AutoHideEngaged()) {
        if (!g_managingAutoHide) {
            g_autoHideBefore = SendMessage(tb, kTraySettingMessage, kTraySettingAutoHideGet, 0);
            g_managingAutoHide = true;
        }
        SendMessage(tb, kTraySettingMessage, kTraySettingAutoHideSet, TRUE);
    } else if (g_managingAutoHide) {
        SendMessage(tb, kTraySettingMessage, kTraySettingAutoHideSet, g_autoHideBefore);
        g_managingAutoHide = false;
    }
    Wh_Log(L"[magnify over windows] %s", AutoHideEngaged() ? L"on" : L"off");
    Wh_SetIntValue(UserValueName(L"managingAutoHide").c_str(), g_managingAutoHide);
    Wh_SetIntValue(UserValueName(L"autoHideBefore").c_str(), g_autoHideBefore);
}

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    if (SmartAutoHideOn()) {
        auto it = g_sahKeptShown.find(pThis);
        if (it != g_sahKeptShown.end()) {
            KillTimer(it->second, kTrayHideTimer);
            return;
        }
        TrayUI__Hide_Original(pThis);
        return;
    }
    if (KeepTaskbarShown()) {
        if (HWND tb = FindTaskbar()) KillTimer(tb, kTrayHideTimer);
        return;
    }
    TrayUI__Hide_Original(pThis);
}

using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
    if (SmartAutoHideOn()) {
        auto it = g_sahKeptShown.find(pThis);
        if (it != g_sahKeptShown.end()) {
            KillTimer(it->second, kTrayHideTimer);
            return;
        }
        CSecondaryTray__AutoHide_Original(pThis, param1);
        return;
    }
    if (KeepTaskbarShown()) return;
    CSecondaryTray__AutoHide_Original(pThis, param1);
}

template <typename F>
void ForEachTaskbarWindow(F f) {
    EnumWindows(
        [](HWND w, LPARAM l) -> BOOL {
            DWORD pid;
            WCHAR cls[32];
            if (GetWindowThreadProcessId(w, &pid) && pid == GetCurrentProcessId() &&
                GetClassName(w, cls, ARRAYSIZE(cls)) &&
                (!_wcsicmp(cls, L"Shell_TrayWnd") || !_wcsicmp(cls, L"Shell_SecondaryTrayWnd"))) {
                (*(F*)l)(w);
            }
            return TRUE;
        },
        (LPARAM)&f);
}

int StripHeight(UINT dpi) {
    float height = g_dockBottomGap + g_dockHeight;
    if (!g_hideTray) height = std::max(height, (float)kStockTaskbarHeight);
    return MulDiv((int)std::ceil(height), dpi, 96);
}

void ClipTaskbars() {
    ForEachTaskbarWindow([](HWND tb) {
        RECT window, have;
        if (!GetWindowRect(tb, &window)) return;
        int type = GetWindowRgnBox(tb, &have);
        if (!AutoHideEngaged() || g_dockExpanded) {
            if (type != ERROR) SetWindowRgn(tb, nullptr, TRUE);
            return;
        }
        RECT want{0, 0, window.right - window.left, window.bottom - window.top};
        want.top = std::max(0, (int)want.bottom - StripHeight(GetDpiForWindow(tb)));
        if (type == SIMPLEREGION && EqualRect(&have, &want)) return;
        SetWindowRgn(tb, CreateRectRgnIndirect(&want), TRUE);
    });
}

struct AppBar {
    HWND hwnd;
    RECT rect;
};
std::vector<AppBar> g_appBars;

void RemoveAppBars() {
    for (auto& bar : g_appBars) {
        APPBARDATA abd{sizeof(abd)};
        abd.hWnd = bar.hwnd;
        SHAppBarMessage(ABM_REMOVE, &abd);
        DestroyWindow(bar.hwnd);
    }
    g_appBars.clear();
}

void AddAppBar(RECT const& rect) {
    HWND w = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWindowClass, L"", WS_POPUP, 0, 0,
                            0, 0, nullptr, nullptr, ModuleInstance(), nullptr);
    if (!w) return;
    g_appBars.push_back({w, rect});

    APPBARDATA abd{sizeof(abd)};
    abd.hWnd = w;
    abd.uCallbackMessage = kAppBarMessage;
    SHAppBarMessage(ABM_NEW, &abd);
    abd.uEdge = ABE_BOTTOM;
    abd.rc = rect;
    SHAppBarMessage(ABM_QUERYPOS, &abd);
    abd.rc.top = abd.rc.bottom - (rect.bottom - rect.top);
    SHAppBarMessage(ABM_SETPOS, &abd);
    Wh_Log(L"[magnify over windows] keeping %d px free at the bottom of a screen",
           (int)(rect.bottom - rect.top));
}

void UpdateAppBars() {
    std::vector<RECT> want;
    if (KeepTaskbarShown()) {
        ForEachTaskbarWindow([&want](HWND tb) {
            HMONITOR monitor = MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST);
            MONITORINFO info{sizeof(info)};
            UINT dpiX, dpiY;
            if (!GetMonitorInfo(monitor, &info) ||
                FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
                return;
            }
            RECT rect = info.rcMonitor;
            rect.top = rect.bottom - StripHeight(dpiY);
            want.push_back(rect);
        });
    }
    bool same = want.size() == g_appBars.size();
    for (size_t i = 0; same && i < want.size(); i++) {
        same = EqualRect(&want[i], &g_appBars[i].rect);
    }
    if (same) return;
    RemoveAppBars();
    if (want.empty() || !RegisterWindowClass()) return;
    for (auto& rect : want) AddAppBar(rect);
}

int g_overWindowsPending = 0;

void ScheduleOverWindows(bool autoHide) {
    bool post = g_overWindowsPending == 0;
    g_overWindowsPending = std::max(g_overWindowsPending, autoHide ? 2 : 1);
    if (!post) return;
    HWND w = EnsureModWindow();
    if (!w || !PostMessage(w, kUpdateOverWindowsMessage, 0, 0)) g_overWindowsPending = 0;
}

void UpdateOverWindows() {
    int pending = std::exchange(g_overWindowsPending, 0);
    if (g_unloading || !pending) return;
    if (pending == 2) ApplyAutoHide();
    UpdateAppBars();
    ClipTaskbars();
}

void ApplyMagnifyOverWindows() {
    if (HWND tb = FindTaskbar()) {
        RunFromWindowThread(tb, [](void*) { ScheduleOverWindows(true); }, nullptr);
    }
}

LRESULT CALLBACK ModWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case kAppBarMessage:
            if (wParam == ABN_POSCHANGED) ScheduleOverWindows(false);
            return 0;
        case WM_DISPLAYCHANGE:
            ScheduleOverWindows(false);
            break;
        case kUpdateOverWindowsMessage:
            UpdateOverWindows();
            return 0;
        case kToggleLaunchpadMessage:
            if (!g_unloading) LaunchpadToggle();
            return 0;
        case kTrashChangedMessage:
            CheckRecycleBinSoon();
            return 0;
        case kTrashCheckedMessage:
            if (!g_unloading) {
                try { UpdateTrashIcons(); } catch (...) {}
            }
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

using XamlLauncher_ShowStartView_t = HRESULT(WINAPI*)(void* pThis, int showMethod, int showFlags);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original;
HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis, int showMethod, int showFlags) {
    HWND w = g_modWindow;
    if (g_unloading || !w || !LpAvailable()) {
        return XamlLauncher_ShowStartView_Original(pThis, showMethod, showFlags);
    }
    PostMessage(w, kToggleLaunchpadMessage, 0, 0);
    return S_OK;
}

bool HookShowStartView() {
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original,
            XamlLauncher_ShowStartView_Hook,
            true,
        },
    };
    return WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks)) &&
           XamlLauncher_ShowStartView_Original;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
            &TrayUI__StuckTrayChange_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &TrayUI__HandleSettingChange_Original,
            TrayUI__HandleSettingChange_Hook,
        },
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
            &CSecondaryTray__AutoHide_Original,
            CSecondaryTray__AutoHide_Hook,
            true,
        },
        {
            {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
            &CTaskListWnd_ComputeJumpViewPosition_Original,
            CTaskListWnd_ComputeJumpViewPosition_Hook,
            true,
        },
        {
            {LR"(struct tagRECT __cdecl ToRawPixelsInDesktopCoordinates(struct winrt::Windows::Foundation::Rect const &,unsigned int,struct HWND__ *))"},
            &ToRawPixelsInDesktopCoordinates_Original,
            ToRawPixelsInDesktopCoordinates_Hook,
            true,
        },
        {
            {LR"(const TrayUI::`vftable'{for `IInspectable'})"},
            &TrayUI_vftable_IInspectable,
            nullptr,
            true,
        },
        {
            {LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})"},
            &TrayUI_vftable_ITrayComponentHost,
            nullptr,
            true,
        },
        {
            {LR"(const CSecondaryTray::`vftable'{for `ISecondaryTray'})"},
            &CSecondaryTray_vftable_ISecondaryTray,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual struct HMONITOR__ * __cdecl TrayUI::GetStuckMonitor(void))"},
            &TrayUI_GetStuckMonitor_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual struct HMONITOR__ * __cdecl CSecondaryTray::GetMonitor(void))"},
            &CSecondaryTray_GetMonitor_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual bool __cdecl TrayUI::GetStuckRectForMonitor(struct HMONITOR__ *,struct tagRECT *))"},
            &TrayUI_GetStuckRectForMonitor_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &TrayUI_Unhide_Original,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &CSecondaryTray__Unhide_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
            true,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_v_WndProc_Original,
            CSecondaryTray_v_WndProc_Hook,
            true,
        },
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;

std::vector<COMBINED_SH> GetTrayHooks() {
    WindhawkUtils::SYMBOL_HOOK trayHooks[] = {
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_Original,
            SystemTrayController_UpdateFrameSize_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,
        },
    };
    return {std::make_move_iterator(std::begin(trayHooks)),
            std::make_move_iterator(std::end(trayHooks))};
}

bool HookTaskbarViewSymbols(HMODULE module, bool withTray) {
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &TaskbarConfiguration_GetFrameSize_Original,
            TaskbarConfiguration_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )"},
            &TaskbarFrame_MaxHeight_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )"},
            &TaskbarFrame_Height_Original,
            TaskbarFrame_Height_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
            &TaskbarController_OnGroupingModeChanged_Address,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
            &TaskbarController_UpdateFrameHeight_Original,
            TaskbarController_UpdateFrameHeight_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &TaskbarFrame_OnPointerExited_Original,
            TaskbarFrame_OnPointerExited_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnTaskListButtonContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
            &TaskbarResources_OnTaskListButtonContextRequested_Original,
            TaskbarResources_OnTaskListButtonContextRequested_Hook,
            true,
        },
        {
            {LR"(public: static void __cdecl winrt::Taskbar::implementation::TaskListButtonHandlers::HandleContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
            &TaskListButtonHandlers_HandleContextRequested_Original,
            TaskListButtonHandlers_HandleContextRequested_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragStartedGesture(struct winrt::Windows::Foundation::Point))"},
            &TaskListButton_OnDragStartedGesture_Original,
            TaskListButton_OnDragStartedGesture_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateDrag(struct winrt::Windows::Foundation::Point))"},
            &TaskListButton_UpdateDrag_Original,
            TaskListButton_UpdateDrag_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragCompletedGesture(void))"},
            &TaskListButton_OnDragCompletedGesture_Original,
            TaskListButton_OnDragCompletedGesture_Hook,
            true,
        },
        {
            {LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"},
            &TaskbarConfiguration_GetIconHeightInViewPixels_Original,
            TaskbarConfiguration_GetIconHeightInViewPixels_Hook,
            true,
        },
        {
            {LR"(public: struct winrt::Taskbar::implementation::TaskbarFrameMetrics __cdecl winrt::Taskbar::implementation::TaskbarFrame::GetMetrics(void)const )"},
            &TaskbarFrame_GetMetrics_Original,
            TaskbarFrame_GetMetrics_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::IconHeight(double))"},
            &TaskListButton_IconHeight_Address,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
            &TaskListButton_UpdateButtonPadding_Original,
            TaskListButton_UpdateButtonPadding_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"},
            &TaskListButton_UpdateIconColumnDefinition_Original,
            TaskListButton_UpdateIconColumnDefinition_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateMultiWindowClip(void))"},
            &TaskListButton_UpdateMultiWindowClip_Original,
            TaskListButton_UpdateMultiWindowClip_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::CreateMultiWindowClip(void))"},
            &TaskListButton_CreateMultiWindowClip_Original,
            TaskListButton_CreateMultiWindowClip_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadge(void))"},
            &TaskListButton_UpdateBadge_Original,
            TaskListButton_UpdateBadge_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
            &TaskListButton_OverlayIcon_Original,
            TaskListButton_OverlayIcon_Hook,
            true,
        },
        {
            {LR"(public: double __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(void)const )"},
            &ExperienceToggleButton_IconHeight_get,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(double))"},
            &ExperienceToggleButton_IconHeight_Original,
            ExperienceToggleButton_IconHeight_Hook,
            true,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            &ExperienceToggleButton_UpdateButtonPadding_Original,
            ExperienceToggleButton_UpdateButtonPadding_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Microsoft::Windows::Taskbar::implementation::TaskbarComponentHost::IconHeight(double))"},
            &TaskbarComponentHost_IconHeight_Original,
            TaskbarComponentHost_IconHeight_Hook,
            true,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"},
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
            &ViewCoordinator_UpdateIsExpanded_Original,
            nullptr,
            true,
        },
    };
    std::vector<COMBINED_SH> allHooks(std::make_move_iterator(std::begin(taskbarViewHooks)),
                                      std::make_move_iterator(std::end(taskbarViewHooks)));
    if (withTray) {
        for (auto& hook : GetTrayHooks()) allHooks.push_back(std::move(hook));
    }
    if (!WindhawkUtils::HookSymbols(module, allHooks.data(), allHooks.size())) return false;
    if (TaskListButton_IconHeight_Address) GetIconHeightOffset();
    return true;
}

bool HookTraySymbols(HMODULE module) {
    auto systemTrayHooks = GetTrayHooks();
    return WindhawkUtils::HookSymbols(module, systemTrayHooks.data(), systemTrayHooks.size());
}

bool TaskbarViewHasTray(HMODULE module) {
    HRSRC res = FindResource(module, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    HGLOBAL global = res ? LoadResource(module, res) : nullptr;
    void* data = global ? LockResource(global) : nullptr;
    VS_FIXEDFILEINFO* info = nullptr;
    UINT size = 0;
    if (!data || !VerQueryValue(data, L"\\", (void**)&info, &size) || !size) return false;
    return HIWORD(info->dwFileVersionMS) < 2604;
}

std::atomic<bool> g_taskbarViewHooked = false;
std::atomic<bool> g_trayHooked = false;

bool HookTaskbarModules() {
    bool hooked = false;
    if (HMODULE view = GetModuleHandle(L"Taskbar.View.dll");
        view && !g_taskbarViewHooked.exchange(true)) {
        bool withTray = !GetModuleHandle(L"SystemTray.dll") && TaskbarViewHasTray(view) &&
                        !g_trayHooked.exchange(true);
        HookTaskbarViewSymbols(view, withTray);
        hooked = true;
    }
    if (HMODULE tray = GetModuleHandle(L"SystemTray.dll");
        tray && !g_trayHooked.exchange(true)) {
        HookTraySymbols(tray);
        hooked = true;
    }
    return hooked;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module && (!g_taskbarViewHooked || !g_trayHooked) && HookTaskbarModules()) {
        Wh_ApplyHookOperations();
    }
    return module;
}

void LoadSettings() {
    g_dockSize = std::clamp(Wh_GetIntSetting(L"dock.size"), 0, 100);
    auto color = WindhawkUtils::StringSetting::make(L"dock.color");
    g_dockTheme = wcscmp(color, L"dark") == 0 ? 1 : wcscmp(color, L"light") == 0 ? 2 : 0;
    g_dockTint = std::clamp(Wh_GetIntSetting(L"dock.tint"), 0, 100);
    g_dockBlur = std::clamp(Wh_GetIntSetting(L"dock.blur"), 0, 100);
    g_scale = 1.0f + std::clamp(Wh_GetIntSetting(L"magnification.amount"), 0, 100) / 100.0f;
    g_magnificationRange = std::clamp(Wh_GetIntSetting(L"magnification.range"), 0, 100);
    g_animationMs = std::clamp(Wh_GetIntSetting(L"magnification.smoothing"), 0, 1000);
    g_magnifyOverWindows = Wh_GetIntSetting(L"magnification.overWindows");
    g_appsDivider = Wh_GetIntSetting(L"items.appsDivider");
    g_showTrash = Wh_GetIntSetting(L"items.recycleBin");

    g_appleLogo = Wh_GetIntSetting(L"extras.appleLogo");
    g_hideTray = Wh_GetIntSetting(L"extras.hideTray");
    g_lpAnimations = Wh_GetIntSetting(L"launchpad.animations");
    {
        auto availability = WindhawkUtils::StringSetting::make(L"launchpad.availability");
        g_lpModeOn = wcscmp(availability, L"disabled") != 0;
    }
    g_lpSize = std::clamp(Wh_GetIntValue(L"lpSize", 1), 0, 2);
    g_lpGrid = std::clamp(Wh_GetIntValue(L"lpGrid", 0), 0, 4);
    g_lpShowNames = Wh_GetIntValue(L"lpNames", 1) != 0;
    g_lpUserDisabled = Wh_GetIntValue(L"lpUserDisabled", 0) != 0;
    {
        auto mode = WindhawkUtils::StringSetting::make(L"autoHide.mode");
        g_smartMode = wcscmp(mode, L"off") == 0 ? SmartMode::off
                    : wcscmp(mode, L"maximized") == 0 ? SmartMode::maximized
                    : SmartMode::intersected;
    }
    g_smartForegroundOnly = Wh_GetIntSetting(L"autoHide.foregroundOnly");
    g_indicatorOn = Wh_GetIntSetting(L"autoHide.indicator");
    g_dockFade = Wh_GetIntSetting(L"autoHide.fade");
    g_smartPrimaryOnly = Wh_GetIntSetting(L"autoHide.primaryMonitorOnly");

    ComputeSizes();
    Wh_Log(L"Sizes: icon %.0f (loaded at %.0f), dock %.0f, taskbar %d, button margin %.1f",
           g_iconSize, g_iconLoadSize, g_dockHeight, g_taskbarHeight, g_buttonMargin);
}

BOOL Wh_ModInit() {
    LoadSettings();
    if (HMODULE user32 = LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        pIsWindowArranged = (IsWindowArranged_t)GetProcAddress(user32, "IsWindowArranged");
        pGetWindowBand = (GetWindowBand_t)GetProcAddress(user32, "GetWindowBand");
    }
    g_managingAutoHide = Wh_GetIntValue(UserValueName(L"managingAutoHide").c_str(), 0);
    g_autoHideBefore = Wh_GetIntValue(UserValueName(L"autoHideBefore").c_str(), 0);
    g_shellJobWork = CreateThreadpoolWork(RunShellJob, nullptr, nullptr);

    if (!HookTaskbarDllSymbols()) Wh_Log(L"taskbar.dll hooks failed; height won't change");
    HookTaskbarModules();
    g_startHooked = HookShowStartView();
    Wh_Log(L"Start hook: %s", g_startHooked ? L"installed" : L"unavailable");

    if (HMODULE kb = GetModuleHandle(L"kernelbase.dll")) {
        if (auto p = GetProcAddress(kb, "LoadLibraryExW")) {
            WindhawkUtils::SetFunctionHook((LoadLibraryExW_t)p, LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
        }
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_taskbarHeight > 0) ApplyTaskbarHeight();
    ApplyMagnifyOverWindows();
    if (LpAvailable()) LpStartBuild();
    if (SmartAutoHideOn()) SahAdjustAllTaskbars();
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    LpStopBuild();
    SahStopEventThread();

    if (HWND tb = FindTaskbar()) {
        RunFromWindowThread(tb, [](void*) { ResetAll(); }, nullptr);
    }
    ApplyAutoHide();
    UnregisterClass(kWindowClass, ModuleInstance());
    for (PTP_WORK* work : {&g_shellJobWork, &g_trashWork}) {
        if (!*work) continue;
        WaitForThreadpoolWorkCallbacks(*work, FALSE);
        CloseThreadpoolWork(*work);
        *work = nullptr;
    }

    if (g_taskbarHeight > 0) {
        g_taskbarHeight = 0;
        g_iconLoadSize = 0;
        ApplyTaskbarHeight();
    }
}

void Wh_ModSettingsChanged() {
    int oldTaskbarHeight = g_taskbarHeight;
    float oldIconLoadSize = g_iconLoadSize;
    HWND tb = FindTaskbar();
    bool ran = tb && RunFromWindowThread(tb, [](void*) {
            bool prevForeground = g_smartForegroundOnly;
            LoadSettings();
            if (g_lpModeOn && g_lpUserDisabled) {
                g_lpUserDisabled = false;
                Wh_SetIntValue(L"lpUserDisabled", 0);
            }
            if (!LpAvailable()) {
                LaunchpadShutdown();
                LpReleaseApps();
            } else {
                LpEnsureBuilt();
            }
            if (g_smartMode == SmartMode::off || prevForeground != g_smartForegroundOnly) {
                SahStopEventThread();
            }
            if (g_smartMode == SmartMode::off) {
                SwipeBarSet(false);
                FadeDock(true);
                g_sahKeptShown.clear();
                for (auto& p : g_sahViewCoordinator) SahUpdateViewCoordinatorIsExpanded(p.first);
            }
            try { ScanTaskbars(); } catch (...) {}
            if (SmartAutoHideOn()) SahAdjustAllTaskbars();
            try { RemovePlates(); } catch (...) {}
            try { StopTrash(); } catch (...) {}
            for (auto& b : *g_buttons) {
                if (auto el = b.element.get()) {
                    try {
                        ApplySpacing(el);
                        ApplyButtonWidth(b, el);
                        ApplyIconColumn(b);
                    } catch (...) {}
                }
            }
            for (auto& i : *g_iconImages) {
                if (auto el = i.element.get()) {
                    try { ApplyIconSize(el); } catch (...) {}
                }
            }
            g_row.clear();
            g_rowParent = nullptr;
            MarkPlatesDirty();
        }, nullptr);
    if (!ran) LoadSettings();
    if (g_taskbarHeight != oldTaskbarHeight || g_iconLoadSize != oldIconLoadSize) {
        ApplyTaskbarHeight();
    }
    ApplyMagnifyOverWindows();
}