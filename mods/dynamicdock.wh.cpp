// ==WindhawkMod==
// @id              dynamicdock
// @name            DynamicDock
// @description     For Ultrawide Monitors: Force the taskbar to follow your mouse when it touches the edge and your icons will be centered on your mouse. Works with default and modded taskbars.
// @version         1.0.1
// @author          IIKiiNGsLYII
// @github          https://github.com/IIKiiNGsLYII
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DynamicDock

Best for use with ultrawide monitors, but works on any monitor. Instead of the taskbar popping up in the middle of the screen, 
the taskbar appears or slides the icons to your mouse **right where your mouse touches the screens edge**.

This is still the real Windows taskbar, so everything works like normal: jump lists (right-click), hover thumbnails, badges, drag and drop, the tray and the clock.

![DynamicDock in action](https://raw.githubusercontent.com/IIKiiNGsLYII/DynamicDock/assets/DynamicDock-default-taskbar.gif)

![DynamicDock in action](https://raw.githubusercontent.com/IIKiiNGsLYII/DynamicDock/assets/DynamicDock-with-mods.gif)

## Bottom taskbars (works with auto-hide or always visible)
1. Choose how the taskbar shows:
    - **Auto-hide:** confirm or turn on **Automatically hide the taskbar** in Windows Settings > Personalization > Taskbar > Taskbar behaviors > Select "Automatically hide the taskbar" from the menu.
    - **Always visible:** this mod also works with the default bottom taskbar that is always visible. Be sure to turn on **Keep the bottom Windows default taskbar full width** in the **Bottom taskbar** settings.
2. Keep **Taskbar alignment** on **Center**.
3. Move the mouse to the bottom edge of the screen anywhere: the bar opens under it or simply slides the icons to your mouse with the default taskbar.
- **Keep in mind** that taskbars change widths based on the amount of icons in the bar. Some additional adjustments may be required as the total taskbar pixel count changes to maintain functionality and spacing as you desire.

## Top taskbar
1. In Windows Settings > Personalization > Taskbar > Taskbar behaviors, set **Taskbar position** to **Top**. This option
   only exists in recent Windows 11 builds (for example 26200.9711). If your build doesn't have it, use the
   [Taskbar on top for Windows 11](https://windhawk.net/mods/taskbar-on-top) mod to move the taskbar to the top.
2. In DynamicDock's settings, open **Top taskbar** and turn on **Taskbar on top of screen**.
3. Touch the top edge of the screen anywhere: the app icons slide under the mouse.
- The taskbar itself stays full width across the top (Windows can't auto-hide it there), and the tray and clock
  stay at the right edge. Only the row of app icons moves, so nothing collapses or leaves empty space.
- The center zone, limits, edge distance and smooth movement work the same. The icons never slide under the tray.
- The **Tray and clock** and **Rounded corners** settings are for the compact bottom bar only and are ignored for the top taskbar option.

## Full-width bottom taskbar
Prefer the taskbar to stay full width at the bottom too? Turn on **Keep the bottom taskbar full width** (in the
**Bottom taskbar** section). The taskbar then works like the top one: it pops up full width with Widgets in the
left corner and the tray and clock at the right edge, and only the app icons slide to the mouse.

## Start menu above the dock
Want the Start menu to open above the moved bar instead of in the middle of the screen? Install the
[Shell Flyout Positions](https://windhawk.net/mods/shell-flyout-positions) mod alongside DynamicDock and, in its
**Start menu** settings, set **Horizontal alignment** to **Center-aligned to the Start button** (or left/right-aligned
to the Start button). DynamicDock moves the Start button with the bar, so Start then opens right above it.

## Compatibility
- Works with the **default Windows 11 taskbar** and with the **Windows 11 Taskbar
  Styler** mod (DynamicDock moves the bar; the styler keeps deciding how it looks).
- Settings marked **(default taskbar only)** change the look of the plain Windows
  taskbar. When Taskbar Styler or a similar mod restyles the taskbar, those
  settings are skipped so they don't fight that mod's look.
- Other taskbar mods (icon size, clock customization, auto-hide instant show,
  thumbnail reorder) can be used alongside it. *Taskbar auto-hide instant show*
  is recommended: it removes Windows' delay before the taskbar pops up.
- Windows 11 only.  

## Troubleshooting (Try this if things are not working correctly)
- **The bar doesn't move, or the taskbar looks stuck in a moved position:** turn
  DynamicDock off and on again in Windhawk. If that doesn't fix it, restart
  Explorer: open Task Manager, search for **Windows Explorer**, right-click **Windows Explorer** and choose
  **Restart**. This rebuilds the taskbar from scratch (it also closes open File
  Explorer windows). You may need to tap your windows key on the keyboard for the taskbar to reappear. Then return to windhawk and toggle your mods on/off as needed to function properly.
- After installing or updating the mod, the first touch of the screen edge can
  take a moment while DynamicDock finds the taskbar.
- To report a problem, turn on **Debug logging** on the mod's Advanced tab,
  reproduce it, and share the lines from the mod's log. You should see logs being created as you tap the screen edge in different places.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- General:
  - MinDisplayWidth: 1920
    $name: Minimum screen width
    $description: Determine the minimum screen pixel width DynamicDock is active within. This is beneficial if you have multiple monitors that are different sizes or orientations. Screens that are below the input value keep the normal taskbar centered and will not move. Make it 0 if you want it to always move to the mouse no matter the screen size.
  - AllMonitors: true
    $name: Use on all monitors
    $description: Also move the taskbar on additional monitors (each one still has to meet the minimum screen width). Turn off to use DynamicDock on the main monitor only.
  $name: General
- TopTaskbar:
  - Enabled: false
    $name: Taskbar on top of screen
    $description: Turn on if you use the top taskbar (Windows Settings > Personalization > Taskbar > Taskbar behaviors > Taskbar position, on recent Windows 11 builds; on older builds use the Taskbar on top for Windows 11 mod). DynamicDock then watches the top edge of the screen instead of the bottom. The taskbar stays full width at the top with the tray and clock at the right edge; only the app icons slide to the mouse whenever you touch the top edge (outside the center zone), even if "Follow the mouse" is off.
  $name: Top taskbar
- Position:
  - FollowMouse: true
    $name: Follow the mouse
    $description: While the bar is visible, moving the mouse along the bottom edge past either end of the bar and touching the bottom edge, centers the bar under the mouse again.
  - SmoothFollow: false
    $name: Smooth movement
    $description: When the bar moves while it is open (follow the mouse, an app opening or closing, returning to the center), it glides to its new spot instead of jumping.
  - CenterZone: 20
    $name: Center zone (% of screen width)
    $description: "If the mouse touches the bottom edge within this middle part of the screen, the taskbar opens in its normal centered position. Example: 20 means the middle 20% of the screen will always open as default centered. 0 turns this off and will always move."
  - EdgePadding: 0
    $name: Distance from screen edges (pixels)
    $description: The bar always stays at least this far from the left and right edges of the screen. 0 lets it touch the edges.
  - MaxSlide: 0
    $name: Maximum distance from center (pixels)
    $description: Limits how far the middle of the bar can move away from the middle of the screen, on both sides. 0 means no limit (the bar can go all the way to the edges).
  - SeparateSides: false
    $name: Different limits for left and right
    $description: Use the two limits below instead of "Maximum distance from center".
  - MaxSlideLeft: 0
    $name: Maximum distance to the left (pixels)
    $description: Only used with "Different limits for left and right". 0 means no limit.
  - MaxSlideRight: 0
    $name: Maximum distance to the right (pixels)
    $description: Only used with "Different limits for left and right". 0 means no limit.
  $name: Position and movement
- Tray:
  - TrayPlacement: withBar
    $name: Tray and clock position (bottom taskbar only)
    $description: Where the tray icons and clock go when the bar moves. With a top taskbar, or with "Keep the bottom taskbar full width", the tray and clock always stay at the right edge and this setting is ignored.
    $options:
    - withBar: Move with the bar (recommended for modded taskbars)
    - edge: Stay at the right edge of the screen (mimics the default taskbar location)
    - hidden: Hide them while the bar is moved
  $name: Tray and clock (bottom taskbar only)
- BottomTaskbar:
  - FullWidth: false
    $name: Keep the bottom Windows default taskbar full width (move only the icons)
    $description: Instead of gathering into a compact floating bar, the bottom taskbar stays full width with Widgets in the left corner and the tray and clock at the right edge. Only the app icons slide to the mouse. When on, the Tray and clock and Rounded corners settings are not used. This setting is not compatible with a modded taskbar styler.
  $name: Bottom taskbar
- Appearance:
  - Corners: none
    $name: Rounded corners (default bottom taskbar only)
    $description: Round the corners of the moved bar, and of the clock area when it stays at the right edge. Only used when the bar is away from its normal centered position, and only with the default Windows taskbar look (skipped when Taskbar Styler or a similar mod restyles the taskbar). Not used with a top taskbar or with "Keep the bottom taskbar full width", since those stay full width.
    $options:
    - none: Square (normal Windows look)
    - top: Round the top corners
    - all: Round the top and bottom corners
  - CornerRadius: 8
    $name: Corner radius (pixels)
    $description: How round the corners are. 8 matches Windows 11 windows; 12 to 16 looks softer, like a dock. There is not a radius limit, so it can be adjusted to fit your screen size to make it look like a fully rounded edge.
  $name: Appearance
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <uiautomation.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <climits>
#include <cmath>
#include <list>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::Foundation::Size;

enum class TrayMode { withBar, edge, hidden };
enum class Corners { none, top, all };

struct Settings {
    int minDisplayWidth;
    bool allMonitors;
    bool followMouse;
    bool smoothFollow;
    bool topTaskbar;  // taskbar at the top of the screen: watch the top edge
    bool bottomFullWidth;  // bottom taskbar stays full width; only the icons move (like the top taskbar)
    TrayMode trayMode;
    Corners corners;
    int cornerRadius;  // screen pixels
    int edgePadding;  // screen pixels
    int centerZone;   // percent of the display width around the center that keeps the stock taskbar
    int maxSlideLeft, maxSlideRight;  // screen pixels from the display center, 0 = unlimited
};
// Written by LoadSettings (engine thread), read from the poll and UI threads: always use a snapshot.
std::mutex g_settingsMutex;
Settings g_settingsData;
Settings CurrentSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settingsData;
}

// Every work item queued on the taskbar's UI thread holds one of these by value, so the unload can wait until all
// of them have run or been discarded (the lambdas live in this DLL).
std::atomic<int> g_uiTasks;
std::atomic<bool> g_unloading;
struct UiTask {
    UiTask() { ++g_uiTasks; }
    UiTask(UiTask const&) { ++g_uiTasks; }
    ~UiTask() { --g_uiTasks; }
};

// Elements are moved through layout (Margin left +dx, right -dx: a shift by dx whatever their alignment, same
// space), not a render transform, so Windows places jump lists, thumbnails and the Start menu at the moved
// buttons by itself. UI thread only.
struct Moved {
    winrt::weak_ref<FrameworkElement> element;
    Thickness original;

    double Shift(FrameworkElement e) {  // current shift, remembering the original margin on first touch
        if (element.get() != e) {
            element = winrt::make_weak(e);
            original = e.Margin();
        }
        return e.Margin().Left - original.Left;
    }
    void SetShift(FrameworkElement e, double dx) {
        Shift(e);
        e.Margin({original.Left + dx, original.Top, original.Right - dx, original.Bottom});
    }
};

// One per taskbar window (main + secondary monitors). Addresses are stable (std::list), entries are never removed.
struct Bar {
    HWND hwnd;                                 // Shell_TrayWnd or Shell_SecondaryTrayWnd of this process
    winrt::weak_ref<FrameworkElement> frame;   // the TaskbarFrame (Start + task buttons), guarded by g_barsMutex
    std::atomic<int> appliedX = INT_MIN;       // cursor x the bar is centered on, INT_MIN = stock position
    std::atomic<int> spanLeft = INT_MAX, spanRight = INT_MIN;  // moved bar span, screen pixels
    std::atomic<double> trayTarget = 0;        // where the tray's left edge belongs, DIPs
    std::atomic<bool> pending = false;         // a placement is queued on the UI thread
    std::atomic<bool> checkPending = false;    // a re-layout check is queued on the UI thread
    std::atomic<double> placedWidth = 0;       // icons width when last placed, DIPs
    std::atomic<bool> measureRow = false;      // placedWidth measures the icon row's buttons (not the whole frame)
    // Poll thread only:
    DWORD lastProbe = 0;
    DWORD hiddenAt = 0;                        // last time the taskbar was seen fully hidden
    DWORD lastCheck = 0;
    RECT monitor{};                            // cached, refreshed with the taskbar list
    // UI thread only (strong XAML references are dropped there in the final reset):
    Moved icons;
    bool frameWasStretched = false;            // we switched the frame from Stretch to Center
    winrt::weak_ref<FrameworkElement> tray;    // cached SystemTrayFrame
    winrt::weak_ref<FrameworkElement> background;
    Thickness backgroundMargin{};
    winrt::weak_ref<FrameworkElement> styledTray;  // tray we hid (hidden mode)
    winrt::weak_ref<FrameworkElement> trayBackgroundHost;  // element we gave the taskbar background (edge mode)
    Media::Brush trayOriginalBackground{nullptr};
    Media::Animation::Storyboard iconsGlide{nullptr}, trayGlide{nullptr};
    std::vector<winrt::weak_ref<FrameworkElement>> heldInPlace;  // full width: Widgets etc. kept in place
    std::vector<Media::Animation::Storyboard> heldGlides;

    explicit Bar(HWND h) : hwnd(h) {}

    void ReleaseXamlObjects() {  // UI thread
        iconsGlide = nullptr;
        trayGlide = nullptr;
        std::vector<Media::Animation::Storyboard>().swap(heldGlides);
        trayOriginalBackground = nullptr;
    }
};

// Bars hold strong XAML refs; they are released on the UI thread in the final reset, and the container is
// destroyed with g_bars.reset() in Wh_ModUninit.
std::mutex g_barsMutex;
[[clang::no_destroy]] std::optional<std::list<Bar>> g_bars{std::in_place};
std::atomic<Bar*> g_probing;  // bar whose frame the UI Automation probe is currently asking for
std::atomic<bool> g_reprobe;  // a taskbar frame we don't know yet was laid out (new monitor, Explorer rebuilt it)
std::atomic<bool> g_xamlReleased;
HANDLE g_thread;
HANDLE g_stopEvent;

Bar& GetBar(HWND hwnd) {
    std::lock_guard lock(g_barsMutex);
    for (auto& b : *g_bars)
        if (b.hwnd == hwnd) return b;
    return g_bars->emplace_back(hwnd);
}

FrameworkElement GetFrame(Bar& b) {
    std::lock_guard lock(g_barsMutex);
    return b.frame.get();
}

Bar* BarForFrame(FrameworkElement const& frame) {
    std::lock_guard lock(g_barsMutex);
    for (auto& b : *g_bars)
        if (b.frame.get() == frame) return &b;
    return nullptr;
}

std::vector<Bar*> AllBars() {
    std::lock_guard lock(g_barsMutex);
    std::vector<Bar*> bars;
    for (auto& b : *g_bars) bars.push_back(&b);
    return bars;
}

// The tray/clock isn't always a direct sibling of the icons, so search the whole (shallow) tree.
FrameworkElement FindByClass(DependencyObject node, std::wstring_view cls, int depth) {
    if (auto fe = node.try_as<FrameworkElement>(); fe && std::wstring_view(winrt::get_class_name(fe)).find(cls) != std::wstring_view::npos)
        return fe;
    if (depth == 0) return nullptr;
    int n = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++)
        if (auto found = FindByClass(Media::VisualTreeHelper::GetChild(node, i), cls, depth - 1)) return found;
    return nullptr;
}

FrameworkElement FindByName(DependencyObject node, std::wstring_view name, int depth) {
    if (auto fe = node.try_as<FrameworkElement>(); fe && fe.Name() == name) return fe;
    if (depth == 0) return nullptr;
    int n = Media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < n; i++)
        if (auto found = FindByName(Media::VisualTreeHelper::GetChild(node, i), name, depth - 1)) return found;
    return nullptr;
}

FrameworkElement FindTray(Bar& b, FrameworkElement frame) {  // UI thread; cached
    auto root = frame.XamlRoot();
    if (auto t = b.tray.get(); t && t.XamlRoot() == root) return t;
    auto t = FindByClass(root.Content(), L"SystemTrayFrame", 8);
    b.tray = t ? winrt::make_weak(t) : winrt::weak_ref<FrameworkElement>{};
    return t;
}

// Our TranslateTransforms. An element's existing RenderTransform (e.g. from a styler theme) is kept: ours is added
// next to it in a TransformGroup, and the original is put back on unload. UI thread only.
struct Wrapped {
    winrt::weak_ref<UIElement> element;
    Media::Transform original{nullptr};  // what the element had before
    Media::Transform installed{nullptr};  // what we set (our transform, or a group holding original + ours)
    Media::TranslateTransform ours{nullptr};
};
[[clang::no_destroy]] std::optional<std::vector<Wrapped>> g_wrapped{std::in_place};

Wrapped* FindWrapped(UIElement const& e) {
    for (auto& w : *g_wrapped)
        if (w.element.get() == e && e.RenderTransform() == w.installed) return &w;
    return nullptr;
}

Media::TranslateTransform Translate(FrameworkElement e) {
    if (auto w = FindWrapped(e)) return w->ours;
    std::erase_if(*g_wrapped, [&](Wrapped const& w) { auto el = w.element.get(); return !el || el == e; });
    UIElement ue = e;
    Wrapped w{winrt::make_weak(ue), e.RenderTransform(), nullptr, Media::TranslateTransform()};
    if (w.original) {
        Media::TransformGroup group;
        group.Children().Append(w.original);
        group.Children().Append(w.ours);
        w.installed = group;
    } else {
        w.installed = w.ours;
    }
    e.RenderTransform(w.installed);
    g_wrapped->push_back(w);
    return w.ours;
}

double TranslateX(FrameworkElement e) {
    auto w = FindWrapped(e);
    return w ? w->ours.X() : 0;
}

void RestoreTransforms() {  // UI thread, final reset: put every original RenderTransform back
    for (auto& w : *g_wrapped)
        if (auto e = w.element.get(); e && e.RenderTransform() == w.installed) e.RenderTransform(w.original);
    std::vector<Wrapped>().swap(*g_wrapped);
}

// Set the translation to `to`, optionally gliding there from `from` (an ease-out animation on top of the final value).
void Glide(Media::Animation::Storyboard& sb, FrameworkElement e, double from, double to, bool animate) {
    if (sb) sb.Stop();
    auto tt = Translate(e);
    tt.X(to);
    if (!animate || std::fabs(from - to) < 1) return;
    Media::Animation::DoubleAnimation anim;
    anim.From(from);
    anim.To(to);
    anim.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(200)));
    Media::Animation::CubicEase ease;
    ease.EasingMode(Media::Animation::EasingMode::EaseOut);
    anim.EasingFunction(ease);
    anim.FillBehavior(Media::Animation::FillBehavior::Stop);  // afterwards the plain value (= to) shows
    Media::Animation::Storyboard::SetTarget(anim, tt);
    Media::Animation::Storyboard::SetTargetProperty(anim, L"X");
    sb = Media::Animation::Storyboard();
    sb.Children().Append(anim);
    sb.Begin();
}

// The tray's own layout spot (without our shift or any running animation), DIPs.
double TrayHome(FrameworkElement tray) {
    auto parent = Media::VisualTreeHelper::GetParent(tray).try_as<UIElement>();
    double parentLeft = parent ? parent.TransformToVisual(nullptr).TransformPoint({0, 0}).X : 0;
    return parentLeft + tray.ActualOffset().x;
}

double LeftEdge(FrameworkElement e) {  // where it is drawn right now, DIPs
    return e.TransformToVisual(nullptr).TransformPoint({0, 0}).X;
}

// Where the app buttons actually are inside an element that may be much wider than them (the full-width icon row
// spans the whole screen with the icons centered inside), relative to the element's left edge, DIPs.
// Buttons are grouped wherever there's a big gap; the widest group is the app icons, and the rest (e.g. the
// Widgets button in the left corner) are returned in `others` so they can be kept where Windows put them.
struct Extent {
    double left = 0, right = 0;
    double othersBefore = -INFINITY, othersAfter = INFINITY;  // right edge of the last item left of the group, left edge of the first after it
    std::vector<FrameworkElement> others;
    double Width() const { return right - left; }
};
Extent ContentExtent(FrameworkElement e) {
    struct Item { double l, r; FrameworkElement el; };
    std::vector<Item> items;
    int n = Media::VisualTreeHelper::GetChildrenCount(e);
    for (int i = 0; i < n; i++) {
        auto child = Media::VisualTreeHelper::GetChild(e, i).try_as<FrameworkElement>();
        if (!child || child.Visibility() != Visibility::Visible || child.ActualWidth() <= 0) continue;
        auto at = child.TransformToVisual(e).TransformPoint({0, 0});
        if (at.X < -1 || at.X > e.ActualWidth() || at.Y < -1 || at.Y > e.ActualHeight()) continue;  // recycled buttons are parked far outside
        double x = at.X - TranslateX(child);  // ignore our own hold-in-place shift
        items.push_back({x, x + child.ActualWidth(), child});
    }
    std::sort(items.begin(), items.end(), [](Item const& a, Item const& b) { return a.l < b.l; });
    Extent best;
    size_t bestFirst = 0, bestEnd = 0;
    for (size_t first = 0, i = 0; i < items.size(); i++) {
        bool groupEnds = i + 1 == items.size() || items[i + 1].l - items[i].r > 100;
        if (!groupEnds) continue;
        if (items[i].r - items[first].l > best.Width()) {
            best.left = items[first].l;
            best.right = items[i].r;
            bestFirst = first;
            bestEnd = i + 1;
        }
        first = i + 1;
    }
    for (size_t i = 0; i < items.size(); i++) {
        if (i < bestFirst) best.othersBefore = std::max(best.othersBefore, items[i].r);
        else if (i >= bestEnd) best.othersAfter = std::min(best.othersAfter, items[i].l);
        else continue;
        best.others.push_back(items[i].el);
    }
    return best;
}

// Width of what is being moved: the whole frame (compact) or just the app buttons inside its icon row (full width).
double MovedWidth(FrameworkElement moved, bool top) {
    if (!top) return moved.ActualWidth();
    auto row = FindByName(moved, L"TaskbarFrameRepeater", 4);
    return row ? ContentExtent(row).Width() : 0;
}

bool IsInside(DependencyObject e, DependencyObject ancestor) {
    for (auto p = e; p; p = Media::VisualTreeHelper::GetParent(p))
        if (p == ancestor) return true;
    return false;
}

// Full width: elements that should stay put (Widgets, background, tray) are shifted back by the frame's shift. While
// the frame glides (from `rowFrom` to 0 on top of its new spot), they get the exact opposite glide on the same curve,
// so on screen they don't move at all.
void HoldInPlace(Bar& b, std::vector<FrameworkElement> const& elements, double shift, double rowFrom, bool animate) {
    for (auto& sb : b.heldGlides)
        if (sb) sb.Stop();
    std::vector<winrt::weak_ref<FrameworkElement>> held;
    std::vector<Media::Animation::Storyboard> glides;
    for (auto& el : elements) {
        glides.emplace_back(nullptr);
        Glide(glides.back(), el, -shift - rowFrom, -shift, animate);
        held.push_back(winrt::make_weak(el));
    }
    for (auto& weak : b.heldInPlace)  // released ones (going back to normal): same opposite glide, ending at 0
        if (auto el = weak.get(); el && std::find(elements.begin(), elements.end(), el) == elements.end()) {
            glides.emplace_back(nullptr);
            Glide(glides.back(), el, -rowFrom, 0, animate);
        }
    b.heldInPlace = std::move(held);
    b.heldGlides = std::move(glides);
}

// Stock Windows paints the taskbar background on the frame. Once the frame hugs its icons, stretch that
// background under the tray as well, so icons and tray stay one solid bar. right = 0 restores it.
void ExtendBackground(Bar& b, FrameworkElement frame, double right) {
    auto bg = FindByClass(frame, L"Taskbar.TaskbarBackground", 3);
    if (!bg) return;
    if (b.background.get() != bg) {
        b.background = winrt::make_weak(bg);
        b.backgroundMargin = bg.Margin();
    }
    auto o = b.backgroundMargin;
    bg.Margin({o.Left, o.Top, o.Right - right, o.Bottom});
}

// Round the moved bar's corners by clipping the default taskbar's background visual (a per-corner rounded clip,
// so "top only" works). radius 0 / Corners::none removes the clip.
void RoundElement(FrameworkElement e, Corners corners, double radius) {
    auto visual = Hosting::ElementCompositionPreview::GetElementVisual(e);
    if (corners == Corners::none || radius <= 0) {
        visual.Clip(nullptr);
        return;
    }
    float r = (float)radius, w = (float)e.ActualWidth(), h = (float)e.ActualHeight();
    winrt::Windows::Foundation::Numerics::float2 round{r, r}, square{0, 0}, bottom = corners == Corners::all ? round : square;
    visual.Clip(visual.Compositor().CreateRectangleClip(0, 0, w, h, round, round, bottom, bottom));
}

void RoundCorners(FrameworkElement frame, Corners corners, double radius) {
    if (auto bg = FindByClass(frame, L"Taskbar.TaskbarBackground", 3)) RoundElement(bg, corners, radius);
}

// Background get/set for whatever kind of element draws it.
Media::Brush GetBackground(FrameworkElement e) {
    if (auto p = e.try_as<Controls::Panel>()) return p.Background();
    if (auto b = e.try_as<Controls::Border>()) return b.Background();
    if (auto c = e.try_as<Controls::Control>()) return c.Background();
    return nullptr;
}
void SetBackground(FrameworkElement e, Media::Brush brush) {
    if (auto p = e.try_as<Controls::Panel>()) p.Background(brush);
    else if (auto b = e.try_as<Controls::Border>()) b.Background(brush);
    else if (auto c = e.try_as<Controls::Control>()) c.Background(brush);
}

// The element that actually draws behind the whole tray: the first Panel/Border inside it covering its full
// width (a Control's own Background is often not used by its template, so prefer what's inside).
FrameworkElement TrayBackgroundHost(FrameworkElement tray) {
    for (DependencyObject node = tray; node;) {
        int n = Media::VisualTreeHelper::GetChildrenCount(node);
        auto child = n ? Media::VisualTreeHelper::GetChild(node, 0).try_as<FrameworkElement>() : nullptr;
        if (!child || child.ActualWidth() < tray.ActualWidth() - 1) break;
        if (child.try_as<Controls::Panel>() || child.try_as<Controls::Border>()) return child;
        node = child;
    }
    return tray;
}

// Tray modes other than "with the bar": give the tray left at the edge its own copy of the taskbar
// background (stock Windows would leave it transparent), or hide it. Restores whatever it changed first.
void StyleTray(Bar& b, FrameworkElement frame, FrameworkElement tray, TrayMode mode, Corners corners = Corners::none, double radius = 0) {
    if (auto old = b.styledTray.get()) {
        old.Opacity(1);
        old.IsHitTestVisible(true);
        b.styledTray = nullptr;
    }
    if (auto host = b.trayBackgroundHost.get()) {
        SetBackground(host, b.trayOriginalBackground);
        RoundElement(host, Corners::none, 0);
        b.trayBackgroundHost = nullptr;
        b.trayOriginalBackground = nullptr;
    }
    if (!tray || mode == TrayMode::withBar) return;
    if (mode == TrayMode::hidden) {
        b.styledTray = winrt::make_weak(tray);
        tray.Opacity(0);
        tray.IsHitTestVisible(false);
        return;
    }
    if (!b.frameWasStretched) return;  // a styled taskbar keeps its own look
    auto fill = FindByName(frame, L"BackgroundFill", 6).try_as<Shapes::Rectangle>();
    auto host = TrayBackgroundHost(tray);
    Wh_Log(L"Tray background: taskbar fill %s, drawn on %s", fill ? L"found" : L"NOT found", winrt::get_class_name(host).c_str());
    if (!fill) return;
    b.trayBackgroundHost = winrt::make_weak(host);
    b.trayOriginalBackground = GetBackground(host);
    SetBackground(host, fill.Fill());
    RoundElement(host, corners, radius);  // same corners as the bar
}

// Center the bar on screen x `cursorX` (INT_MIN puts it back in its stock position), gliding if `animate`.
// Compact bottom bar: the whole bar (icons, tray, background) gathers into one compact bar under the mouse.
// Full width (top taskbar, or bottom by choice): the taskbar stays full width; only the app icons move.
// Queued on the taskbar's UI thread.
void Apply(Bar& b, int cursorX, bool animate) {
    if (g_unloading && cursorX != INT_MIN) return;
    FrameworkElement frame = GetFrame(b);
    RECT wr;
    if (!frame || !GetWindowRect(b.hwnd, &wr)) return;
    // One placement at a time: extra follow requests while one is queued are dropped (the next tick retries).
    if (b.pending.exchange(true) && cursorX != INT_MIN) return;
    auto settings = CurrentSettings();
    Bar* pb = &b;
    try {
        frame.Dispatcher().RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                                    [task = UiTask{}, pb, frame, cursorX, animate, settings, windowLeft = wr.left]() {
        Bar& b = *pb;
        try {
            auto root = frame.XamlRoot();
            auto trayFrame = FindTray(b, frame);
            double trayWas = trayFrame ? TranslateX(trayFrame) : 0;

            if (cursorX == INT_MIN) {
                b.appliedX = INT_MIN;  // first, so nothing re-places the bar while it goes back
                auto moved = b.icons.element.get();
                double movedWas = moved ? LeftEdge(moved) : 0;
                if (b.frameWasStretched) {
                    frame.HorizontalAlignment(HorizontalAlignment::Stretch);
                    b.frameWasStretched = false;
                }
                ExtendBackground(b, frame, 0);
                RoundCorners(frame, Corners::none, 0);
                StyleTray(b, frame, trayFrame, TrayMode::withBar);
                if (moved) {
                    b.icons.SetShift(moved, 0);
                    frame.UpdateLayout();
                    double rowFrom = movedWas - (LeftEdge(moved) - TranslateX(moved));
                    Glide(b.iconsGlide, moved, rowFrom, 0, animate);
                    HoldInPlace(b, {}, 0, rowFrom, animate);
                } else {
                    HoldInPlace(b, {}, 0, 0, false);
                }
                if (trayFrame) Glide(b.trayGlide, trayFrame, trayWas, 0, animate);
                b.spanLeft = INT_MAX;  // stock layout: any follow move outside the center zone places the bar
                b.spanRight = INT_MIN;
                Wh_Log(L"Reset to the normal position%s", animate ? L" (glide)" : L"");
                b.pending = false;
                return;
            }

            bool top = settings.topTaskbar || settings.bottomFullWidth;
            // The frame is always what moves (that's what Windows positions jump lists from); in full width, whatever
            // should stay put (background, Widgets, tray) is held in place, and the icon row is only measured.
            // A styled taskbar (e.g. Taskbar Styler with a frame max width) has no full-width frame: there the frame
            // already is the icon strip, so it moves as a whole and only the tray stays where it is.
            bool rowMode = top && frame.ActualWidth() >= root.Size().Width * 0.8;
            b.measureRow = rowMode;
            FrameworkElement mover = frame, row = nullptr;
            if (rowMode) {
                row = FindByName(frame, L"TaskbarFrameRepeater", 4);
                if (!row) {
                    Wh_Log(L"Full width: icon row (TaskbarFrameRepeater) not found");
                    b.pending = false;
                    return;
                }
            } else if (frame.HorizontalAlignment() == HorizontalAlignment::Stretch) {
                // Stock Windows stretches the frame across the whole taskbar and centers the icons inside it (a
                // Taskbar Styler max-width hides this). Make it hug its icons so moving the frame moves just the icons.
                frame.HorizontalAlignment(HorizontalAlignment::Center);
                b.frameWasStretched = true;
            }
            frame.UpdateLayout();  // positions must reflect the last shift before we measure from them
            double moverWas = LeftEdge(mover);

            double scale = root.RasterizationScale();
            bool trayWithBar = !top && trayFrame && settings.trayMode == TrayMode::withBar;
            // Full width: the icon row is screen-wide, so place its app buttons, not the row.
            Extent content = rowMode ? ContentExtent(row) : Extent{};
            if (rowMode) {  // make the app buttons' extent relative to the frame instead of the row
                double rowOffset = row.TransformToVisual(frame).TransformPoint({0, 0}).X;
                content.left += rowOffset;
                content.right += rowOffset;
                content.othersBefore += rowOffset;
                content.othersAfter += rowOffset;
            }
            double contentL = content.left;
            double iconsW = rowMode ? content.Width() : mover.ActualWidth();
            double trayW = trayWithBar ? trayFrame.ActualWidth() : 0;
            double total = iconsW + trayW;
            double width = root.Size().Width, mid = width / 2, pad = settings.edgePadding / scale;
            double home = LeftEdge(mover) - TranslateX(mover) - b.icons.Shift(mover);  // the mover's unshifted left edge
            // Full width keeps its tray at the right edge and Widgets in the left corner: the icons stay between them.
            double leftLimit = 0, rightLimit = width;
            bool trayInFrame = trayFrame && IsInside(trayFrame, frame);  // then it moves with the frame and is held back
            if (top) {
                if (trayFrame) rightLimit = std::min(rightLimit, TrayHome(trayFrame) - (trayInFrame ? b.icons.Shift(mover) : 0));
                if (rowMode) {
                    leftLimit = std::max(leftLimit, home + content.othersBefore);
                    rightLimit = std::min(rightLimit, home + content.othersAfter);
                }
            }
            double center = (cursorX - windowLeft) / scale;
            if (settings.maxSlideLeft) center = std::max(center, mid - settings.maxSlideLeft / scale);
            if (settings.maxSlideRight) center = std::min(center, mid + settings.maxSlideRight / scale);
            double lo = leftLimit + pad;
            double left = std::round(std::clamp(center - total / 2, lo, std::max(lo, rightLimit - total - pad)));  // never past the edges

            b.icons.SetShift(mover, left - contentL - home);
            // Moving the icons in layout can also move the tray's layout spot, so settle layout before measuring it.
            frame.UpdateLayout();
            // The layout move is instant (so pop-ups land right); the glide only animates the drawing from where it was.
            double rowFrom = moverWas - (LeftEdge(mover) - TranslateX(mover));
            Glide(b.iconsGlide, mover, rowFrom, 0, animate);

            if (top) {
                auto held = content.others;  // Widgets stays in its corner, the background and tray stay full width
                if (auto bg = rowMode ? FindByClass(frame, L"Taskbar.TaskbarBackground", 3) : nullptr) held.push_back(bg);
                if (trayInFrame) held.push_back(trayFrame);
                HoldInPlace(b, held, b.icons.Shift(mover), rowFrom, animate);
                StyleTray(b, frame, trayFrame, TrayMode::withBar);  // tray and clock stay as Windows placed them
                b.trayTarget = 0;
            } else {
                double trayHome = trayFrame ? TrayHome(trayFrame) : 0;
                if (trayFrame) Glide(b.trayGlide, trayFrame, trayWas, trayWithBar ? left + iconsW - trayHome : 0, animate);
                b.trayTarget = trayWithBar ? left + iconsW : 0;
                ExtendBackground(b, frame, b.frameWasStretched && trayWithBar ? trayW : 0);
                StyleTray(b, frame, trayFrame, settings.trayMode, settings.corners, settings.cornerRadius / scale);
                frame.UpdateLayout();  // the background's new width, for the corner clip
                RoundCorners(frame, b.frameWasStretched ? settings.corners : Corners::none, settings.cornerRadius / scale);
            }
            Wh_Log(L"Apply(%d): %s icons=%.0f tray=%.0f%s -> left=%.0f%s", cursorX, top ? L"full width" : L"compact", iconsW, trayW,
                   trayFrame ? L"" : L" (tray NOT found)", left, animate ? L" (glide)" : L"");

            b.placedWidth = iconsW;
            b.spanLeft = windowLeft + (int)(left * scale);
            b.spanRight = windowLeft + (int)((left + total) * scale);
            b.appliedX = cursorX;
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Apply failed: %08X %s", e.code().value, e.message().c_str());
        }
        b.pending = false;
        });
    } catch (winrt::hresult_error const&) {
        b.pending = false;  // the taskbar's UI thread is going away
    }
}

// Re-layout check, queued once per trigger (the frame was measured: an app opened or closed; or, while the bar is
// visible, the tray may still be settling): re-place the bar if the icons' width changed, keep the tray attached.
void QueueCheck(Bar& b) {
    if (g_unloading || b.appliedX == INT_MIN) return;
    FrameworkElement frame = GetFrame(b);
    if (!frame || b.checkPending.exchange(true)) return;
    Bar* pb = &b;
    try {
        frame.Dispatcher().RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [task = UiTask{}, pb, frame]() {
            Bar& b = *pb;
            b.checkPending = false;
            if (g_unloading || b.appliedX == INT_MIN) return;
            try {
                auto settings = CurrentSettings();
                bool iconsOnly = settings.topTaskbar || settings.bottomFullWidth;
                if (auto moved = b.icons.element.get(); moved && std::fabs(MovedWidth(moved, b.measureRow) - b.placedWidth) > 1) {
                    Apply(b, b.appliedX, settings.smoothFollow);
                    return;
                }
                if (iconsOnly || settings.trayMode != TrayMode::withBar) return;
                auto trayFrame = FindTray(b, frame);
                if (!trayFrame) return;
                double shift = TranslateX(trayFrame);
                double want = b.trayTarget - TrayHome(trayFrame);
                if (std::fabs(want - shift) > 0.5) {
                    Glide(b.trayGlide, trayFrame, LeftEdge(trayFrame) - TrayHome(trayFrame), want, true);
                    Wh_Log(L"Realigned tray by %.0f", want - shift);
                }
            } catch (winrt::hresult_error const&) {
            }
        });
    } catch (winrt::hresult_error const&) {
        b.checkPending = false;
    }
}

// When the mod loads into a running Explorer nothing re-lays out the taskbar, so MeasureOverride can't tell us
// which frame belongs to which taskbar. Asking UI Automation for a taskbar's frame bounds makes Explorer call
// TaskbarFrameAutomationPeer::GetBoundingRectangleCore, whose hook records the frame for that taskbar.
// Only used for taskbars owned by this process, so the query stays in-process.
void ProbeFrame(Bar& b) {
    HWND bridge = FindWindowEx(b.hwnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
    if (!bridge) return;
    winrt::com_ptr<IUIAutomation> uia;
    if (FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), uia.put_void()))) return;
    VARIANT cls;
    VariantInit(&cls);
    cls.vt = VT_BSTR;
    cls.bstrVal = SysAllocString(L"Taskbar.TaskbarFrameAutomationPeer");
    winrt::com_ptr<IUIAutomationCondition> cond;
    winrt::com_ptr<IUIAutomationElement> root, peer;
    RECT r;
    g_probing = &b;
    if (SUCCEEDED(uia->CreatePropertyCondition(UIA_ClassNamePropertyId, cls, cond.put())) &&
        SUCCEEDED(uia->ElementFromHandle(bridge, root.put())) && root &&
        SUCCEEDED(root->FindFirst(TreeScope_Descendants, cond.get(), peer.put())) && peer)
        peer->get_CurrentBoundingRectangle(&r);
    g_probing = nullptr;
    VariantClear(&cls);
}

bool IsOwnWindow(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid == GetCurrentProcessId();  // other explorer.exe processes (folder windows, COM servers) have no taskbar
}

// This process's taskbar windows, refreshed about once a second (poll thread only).
std::vector<HWND> Taskbars(Settings const& s) {
    static std::vector<HWND> cached;
    static DWORD refreshedAt;
    static bool allMonitors;
    if (GetTickCount() - refreshedAt < 1000 && allMonitors == s.allMonitors) return cached;
    refreshedAt = GetTickCount();
    allMonitors = s.allMonitors;
    cached.clear();
    if (HWND main = FindWindow(L"Shell_TrayWnd", nullptr); main && IsOwnWindow(main)) cached.push_back(main);
    if (s.allMonitors)
        for (HWND h = nullptr; (h = FindWindowEx(nullptr, h, L"Shell_SecondaryTrayWnd", nullptr));)
            if (IsOwnWindow(h)) cached.push_back(h);
    for (HWND h : cached) {  // monitor rects, refreshed with the list
        MONITORINFO mi = {sizeof(mi)};
        GetMonitorInfo(MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST), &mi);
        GetBar(h).monitor = mi.rcMonitor;
    }
    return cached;
}

void TickBar(Bar& b, POINT pt, Settings const& s) {
    if (!GetFrame(b)) {
        if (GetTickCount() - b.lastProbe > 2000) {  // retry, at most every 2 s
            b.lastProbe = GetTickCount();
            ProbeFrame(b);
        }
        return;
    }
    RECT m = b.monitor;
    if (m.right - m.left < s.minDisplayWidth) {
        if (b.appliedX != INT_MIN) Apply(b, INT_MIN, false);
        return;
    }
    bool top = s.topTaskbar;
    RECT tr;
    GetWindowRect(b.hwnd, &tr);
    // Fully hidden: only the thin auto-hide sliver is on screen. Once it starts sliding up, the spot is locked,
    // so small mouse moves during the slide can't make the bar jump.
    bool hidden = top ? tr.bottom <= m.top + 3 : tr.top >= m.bottom - 3;
    if (hidden) b.hiddenAt = GetTickCount();
    // While the compact bar is visible, Windows may still be settling the tray: check it now and then.
    if (!hidden && b.appliedX != INT_MIN && !s.topTaskbar && !s.bottomFullWidth && s.trayMode == TrayMode::withBar &&
        GetTickCount() - b.lastCheck > 250) {
        b.lastCheck = GetTickCount();
        QueueCheck(b);
    }

    bool atEdge = top ? pt.y <= m.top : pt.y >= m.bottom - 1 && pt.y < m.bottom;
    if (!atEdge || pt.x < m.left || pt.x >= m.right) return;  // only at this monitor's taskbar edge
    // A top taskbar can't auto-hide, so touching the edge always slides the visible bar (follow behavior).
    bool follow = s.followMouse || top;
    bool inCenterZone = abs(pt.x - (m.left + m.right) / 2) * 200 <= (m.right - m.left) * s.centerZone;
    // Glide only when moving a bar that has been fully visible for a while (following the mouse). Right after it
    // pops up (e.g. an instant auto-hide reveal), the icons should simply appear in their new spot, not slide in.
    bool glide = !hidden && s.smoothFollow && GetTickCount() - b.hiddenAt > 500;
    int applied = b.appliedX;
    if (inCenterZone) {
        if (s.topTaskbar || s.bottomFullWidth) {
            if (applied != INT_MIN && (hidden || follow)) Apply(b, INT_MIN, glide);  // full width: the stock taskbar
        } else {
            // Compact bar: the center zone shows the compact bar centered on the screen, not the full-width taskbar.
            int center = (m.left + m.right) / 2;
            if (applied != center && (hidden || follow)) Apply(b, center, glide);
        }
    } else if (hidden ? applied == INT_MIN || abs(pt.x - applied) > 2
                      : follow && (pt.x < b.spanLeft || pt.x > b.spanRight)) {
        Apply(b, pt.x, glide);
    }
}

void CALLBACK OnTick(HWND, UINT, UINT_PTR, DWORD) {
    try {
        POINT pt;
        if (!GetCursorPos(&pt)) return;
        auto s = CurrentSettings();
        auto taskbars = Taskbars(s);
        if (taskbars.empty()) return;  // no taskbar in this explorer.exe process
        static DWORD lastReprobe;
        if (g_reprobe && GetTickCount() - lastReprobe > 2000) {  // an unknown frame was laid out: re-map all taskbars
            g_reprobe = false;
            lastReprobe = GetTickCount();
            for (HWND h : taskbars) ProbeFrame(GetBar(h));
        }
        for (HWND h : taskbars) TickBar(GetBar(h), pt, s);
        // A taskbar that is no longer handled (All monitors turned off) goes back to normal.
        for (Bar* b : AllBars())
            if (b->appliedX != INT_MIN && std::find(taskbars.begin(), taskbars.end(), b->hwnd) == taskbars.end()) Apply(*b, INT_MIN, false);
    } catch (...) {
        // e.g. a taskbar's XAML island being torn down when a monitor is removed: try again on the next tick
    }
}

DWORD WINAPI PollThread(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);  // for the UI Automation probe
    UINT_PTR timer = SetTimer(nullptr, 0, 20, OnTick);
    while (MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0 + 1) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) DispatchMessage(&msg);
    }
    KillTimer(nullptr, timer);
    winrt::uninit_apartment();
    return 0;
}

using TaskbarFrameAutomationPeer_GetBoundingRectangleCore_t = int(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect* rect);
TaskbarFrameAutomationPeer_GetBoundingRectangleCore_t TaskbarFrameAutomationPeer_GetBoundingRectangleCore_Original;
int WINAPI TaskbarFrameAutomationPeer_GetBoundingRectangleCore_Hook(void* pThis, winrt::Windows::Foundation::Rect* rect) {
    if (Bar* b = g_probing) {
        try {
            Automation::Peers::FrameworkElementAutomationPeer peer = nullptr;
            ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<Automation::Peers::FrameworkElementAutomationPeer>(), winrt::put_abi(peer));
            if (auto frame = peer ? peer.Owner().try_as<FrameworkElement>() : nullptr) {
                std::lock_guard lock(g_barsMutex);
                if (b->frame.get() != frame) {
                    b->frame = winrt::make_weak(frame);
                    b->appliedX = INT_MIN;  // re-place on the next edge touch
                    Wh_Log(L"Got TaskbarFrame for %s", b->hwnd == FindWindow(L"Shell_TrayWnd", nullptr) ? L"the main taskbar" : L"a secondary taskbar");
                }
            }
        } catch (...) {
        }
    }
    return TaskbarFrameAutomationPeer_GetBoundingRectangleCore_Original(pThis, rect);
}

using TaskbarFrame_MeasureOverride_t = int(WINAPI*)(void* pThis, Size size, Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(void* pThis, Size size, Size* resultSize) {
    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
    FrameworkElement frame = nullptr;
    ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(frame));
    if (!frame) return ret;
    if (Bar* b = BarForFrame(frame)) QueueCheck(*b);  // contents may have changed (an app opened or closed)
    else g_reprobe = true;  // a frame we don't know yet: find out which taskbar it belongs to
    return ret;
}

HMODULE GetTaskbarViewModule() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    return module ? module : GetModuleHandle(L"ExplorerExtensions.dll");
}

bool HookTaskbarView(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarFrame_MeasureOverride_Original,
            TaskbarFrame_MeasureOverride_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrameAutomationPeer,struct winrt::Windows::UI::Xaml::Automation::Peers::IAutomationPeerOverrides>::GetBoundingRectangleCore(struct winrt::Windows::Foundation::Rect *))"},
            &TaskbarFrameAutomationPeer_GetBoundingRectangleCore_Original,
            TaskbarFrameAutomationPeer_GetBoundingRectangleCore_Hook,
        },
    };
    bool ok = WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
    Wh_Log(L"Hooks: %s", ok ? L"ok" : L"FAILED");
    return ok;
}

std::atomic<bool> g_taskbarViewHooked;
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module && module == GetTaskbarViewModule() && !g_taskbarViewHooked.exchange(true)) {
        Wh_Log(L"Taskbar.View loaded: %s", name);
        if (HookTaskbarView(module)) Wh_ApplyHookOperations();
    }
    return module;
}

void LoadSettings() {
    Settings s;
    s.minDisplayWidth = Wh_GetIntSetting(L"General.MinDisplayWidth");
    s.allMonitors = Wh_GetIntSetting(L"General.AllMonitors");
    s.followMouse = Wh_GetIntSetting(L"Position.FollowMouse");
    s.smoothFollow = Wh_GetIntSetting(L"Position.SmoothFollow");
    s.topTaskbar = Wh_GetIntSetting(L"TopTaskbar.Enabled");
    s.bottomFullWidth = Wh_GetIntSetting(L"BottomTaskbar.FullWidth");
    std::wstring tray = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"Tray.TrayPlacement")).get();
    s.trayMode = tray == L"edge" ? TrayMode::edge : tray == L"hidden" ? TrayMode::hidden : TrayMode::withBar;
    s.edgePadding = std::max(0, Wh_GetIntSetting(L"Position.EdgePadding"));
    std::wstring corners = WindhawkUtils::StringSetting(Wh_GetStringSetting(L"Appearance.Corners")).get();
    s.corners = corners == L"top" ? Corners::top : corners == L"all" ? Corners::all : Corners::none;
    s.cornerRadius = std::max(0, Wh_GetIntSetting(L"Appearance.CornerRadius"));
    s.centerZone = std::clamp(Wh_GetIntSetting(L"Position.CenterZone"), 0, 100);
    bool separate = Wh_GetIntSetting(L"Position.SeparateSides");
    s.maxSlideLeft = std::max(0, Wh_GetIntSetting(separate ? L"Position.MaxSlideLeft" : L"Position.MaxSlide"));
    s.maxSlideRight = std::max(0, Wh_GetIntSetting(separate ? L"Position.MaxSlideRight" : L"Position.MaxSlide"));
    std::lock_guard lock(g_settingsMutex);
    g_settingsData = s;
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"Init: Taskbar.View %s", GetTaskbarViewModule() ? L"already loaded" : L"not loaded yet, waiting for it");
    if (HMODULE module = GetTaskbarViewModule()) {
        g_taskbarViewHooked = true;
        if (!HookTaskbarView(module)) return FALSE;
    } else {
        auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(GetModuleHandle(L"kernelbase.dll"), "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    // Taskbar.View.dll may have loaded between Wh_ModInit and the LoadLibraryExW hook going live.
    if (!g_taskbarViewHooked) {
        if (HMODULE module = GetTaskbarViewModule(); module && !g_taskbarViewHooked.exchange(true) && HookTaskbarView(module))
            Wh_ApplyHookOperations();
    }
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, PollThread, nullptr, 0, nullptr);
}

void Wh_ModBeforeUninit() {
    if (g_thread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    g_unloading = true;  // queued checks return immediately; only the resets below still run
    auto bars = AllBars();
    for (Bar* b : bars) Apply(*b, INT_MIN, false);
    // Final cleanup on the UI thread (all taskbars share it): drop strong XAML refs of every bar, including bars
    // whose taskbar is gone, and put back the original RenderTransforms.
    for (Bar* b : bars) {
        FrameworkElement frame = GetFrame(*b);
        if (!frame) continue;
        try {
            frame.Dispatcher().RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [task = UiTask{}, bars]() {
                for (Bar* b : bars) b->ReleaseXamlObjects();
                RestoreTransforms();
                g_xamlReleased = true;
            });
            break;
        } catch (winrt::hresult_error const&) {
        }
    }
    while (g_uiTasks > 0) Sleep(10);  // every queued lambda lives in this DLL
    Wh_Log(L"Uninit: taskbar reset");
}

void Wh_ModUninit() {
    // Hooks are removed by now. Only release the bars if their XAML objects were dropped on the UI thread;
    // otherwise leak them rather than release XAML objects on the wrong thread.
    if (g_xamlReleased) {
        g_bars.reset();
        g_wrapped.reset();
    }
}

void Wh_ModSettingsChanged() {
    // Put every bar back first (compact and full-width modes move different parts), then use the new settings.
    for (Bar* b : AllBars()) Apply(*b, INT_MIN, false);
    LoadSettings();
    for (Bar* b : AllBars()) b->appliedX = INT_MIN;  // re-place with the new settings on the next edge touch
}
