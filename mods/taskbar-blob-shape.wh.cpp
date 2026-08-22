// ==WindhawkMod==
// @id              taskbar-blob-shape
// @name            Taskbar Blob Shape
// @description     Injects a customizable blob shape behind active taskbar items.
// @version         1.1.0
// @author          Deen-0x
// @github          https://github.com/Deen-0x
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Blob Shape

Adds a blob shape behind active taskbar buttons.

![Taskbar Blob Shape](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/main/taskbar-blob-shape/demo2.gif)

Based on **Taskbar Elastic WinUI Pill** and the unreleased **Taskbar Elastic
Border** by **Lockframe**.

Every taskbar button gets its own blob shape, shown or hidden as the
button's running indicator state changes. The Start, Search, Task view and
Widgets buttons, the date and time, and the other system tray buttons
(control center, language, overflow chevron) can optionally get a blob too,
shown while their flyout is open (toggleable per group in the settings;
Search in "Search icon only" mode). The shapes are hosted in the
taskbar's RootGrid (above the task list's clipping region, so the flares
render fully) and each one is glued to its button with a composition
expression, so it follows the button through reordering, reflow, and
animations on the render thread.

The blob shape is a rounded rectangle whose top extends upward and flares
outward with concave (outside) corner radii, ending in a flat top edge:

- **Width / Height**: the main area of the blob shape ('auto' matches the
  button's background element). Horizontally the shape is centered on the
  button; vertically its flat top edge is anchored to the top of the
  taskbar, with the body hanging downward.
- **Top corner radius**: the radius of the concave top flares. The flares
  are circular quarter arcs, so this also sets how far the blob shape
  extends upward and outward. Total size is
  (Width + 2*TopRadius) x (Height + TopRadius).
- **Bottom corner radius**: the convex bottom corners of the blob shape.

**Known behavior**: when the mod is enabled mid-session, blobs appear on the
first taskbar activity (a hover or any window state change) rather than
instantly; at logon or after an Explorer restart they apply immediately.
Discovery is event-driven by design — the mod keeps no standing visual-tree
watcher.

This mod should not be enabled together with **Taskbar Elastic WinUI Pill**,
since both replace the native active indicator.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- BlobShape:
  - Dimensions: 'auto, 36'
    $name: Custom blob shape dimensions (Width, Height)
    $description: Size of the blob shape's main area. Set to 'auto' to match the button's background element, or specify pixel values (e.g., '32, 32'). The body hangs down from the top of the taskbar.
  - Margins: '0, 0, 0, 0'
    $name: Custom blob shape margin (Left, Top, Right, Bottom)
    $description: Offsets the blob shape like insets - Left/Top push it right/down, Right/Bottom push it left/up (e.g. '0, 4, 0, 0' pushes it down 4px). Vertical offsets are measured from the top of the taskbar. Accepts 1, 2 (horizontal, vertical), or 4 values. Leave empty to disable.
  - TopRadius: '8'
    $name: Top corner radius
    $description: The radius of the concave top flare corners. The blob shape extends upward and sideways by this amount. Set to 0 to disable the flare.
  - BottomRadius: '4'
    $name: Bottom corner radius
    $description: The radius of the convex bottom corners of the blob shape (e.g., 4.0).
  $name: Blob Shape Settings
- Colors:
  - BgOpacity: '1.0, 1.0'
    $name: Blob opacity (Light, Dark)
    $description: Multiplier for the blob shape's fill opacity (e.g. 0.8, 0.5). Set to 1.0 to keep the color's own alpha.
  - CustomColor: ""
    $name: Custom blob shape color
    $description: Hex color code. Supports multi-color gradients (e.g. '#FF0000,#00FF00') and Light | Dark separation (e.g. '#FFFFFF|#09131E'). Both can be combined ('light1,light2|dark1,dark2'). Leave empty to use the system accent color.
  $name: Color Settings
- SystemButtons:
  - SystemButtonsBlob: false
    $name: Start, Search and Task view blob
    $description: Show the blob behind the Start, Search and Task view buttons while they are open. Search is supported in "Search icon only" mode.
  - WidgetsBlob: false
    $name: Widgets button blob
    $description: Show the blob behind the Widgets button while the widgets board is open.
  - DateTimeBlob: false
    $name: Date and time blob
    $description: Show the blob behind the date and time while the notification center is open.
  - TrayButtonsBlob: false
    $name: System tray buttons blob
    $description: Show the blob behind the other system tray buttons (control center, language, overflow chevron) while their flyout is open.
  $name: Other Taskbar Buttons
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/base.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cwctype>
#include <string>
#include <string_view>
#include <mutex>
#include <vector>
#include <functional>
#include <optional>
#include <utility>
#include <cmath>
#include <memory>

struct Settings {
    double BottomRadius = 4.0;
    double TopRadius = 8.0;

    double CustomWidth = -1.0;
    double CustomHeight = -1.0;
    winrt::Windows::UI::Xaml::Thickness CustomMargin = {0,0,0,0};
    bool HasCustomMargin = false;

    double BgOpacityLight = 1.0;
    double BgOpacityDark = 1.0;

    bool SystemButtonsBlob = false;
    bool WidgetsBlob = false;
    bool DateTimeBlob = false;
    bool TrayButtonsBlob = false;

    std::vector<winrt::Windows::UI::Color> ParsedLightColor;
    std::vector<winrt::Windows::UI::Color> ParsedDarkColor;
} g_settings;

std::mutex g_settingsMutex;

// One entry per taskbar button that received a blob shape. The blob is
// hosted in the taskbar's RootGrid and glued to its button with a
// composition expression; the entry caches lookups, event tokens, and
// last-applied parameters.
struct BlobEntry {
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> button;
    winrt::weak_ref<winrt::Windows::UI::Xaml::Shapes::Path> blobShape;
    winrt::weak_ref<winrt::Windows::UI::Xaml::Controls::Grid> grid;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> anchor;

    // Cached per-button element lookups, re-resolved when expired or
    // detached, so hover/press events don't re-walk the subtree. bgElement
    // is also the dedicated "element whose visual we hid" — decoupled from
    // anchor (the sizing element), so a re-templated BackgroundElement can
    // never leave the old one invisible after the delayed restore.
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> iconPanel;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> bgElement;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> indicatorElement;
    winrt::event_token sizeToken{};
    winrt::event_token unloadToken{};
    winrt::event_token loadedToken{};
    winrt::event_token themeToken{};
    bool unloadAttached = false;
    bool loadedAttached = false;

    // Which flavor of taskbar button this entry tracks. Task list buttons
    // are driven by the TaskListButton::UpdateVisualStates hook; system
    // buttons (Start, Search, Task view) and the Widgets button never pass
    // through it and are driven by the toggle/state events below instead.
    // Widgets is a separate kind only so it can gate on its own setting.
    enum Kind : int8_t { KindUnknown = -1, KindTask = 0, KindSystem = 1, KindWidget = 2, KindDateTime = 3, KindTray = 4 };
    int8_t kind = KindUnknown;
    bool systemEventsAttached = false;
    winrt::event_token checkedToken{};
    winrt::event_token uncheckedToken{};

    std::vector<std::pair<winrt::weak_ref<winrt::Windows::UI::Xaml::VisualStateGroup>, winrt::event_token>> stateTokens;

    // Whether WE hid the native indicator visuals for this button, so the
    // restore paths only touch what the mod actually changed (and never
    // override a Visibility the shell might set itself). The two elements
    // are tracked separately because they restore differently: the
    // RunningIndicator comes back immediately on deactivation, while the
    // BackgroundElement comes back on a short delay (see ScheduleBgRestore)
    // so it isn't revealed mid-storyboard as a white flash.
    bool bgHidden = false;
    bool indicatorHidden = false;
    winrt::Windows::UI::Xaml::DispatcherTimer restoreTimer{nullptr};

    // Whether the blob's Translation is glued to its button's offset chain,
    // plus the X adjustment and taskbar-top Y anchor it was bound with
    // (compared to detect when a settings change requires a rebind). The
    // blob stays hidden until the binding succeeds.
    bool bound = false;
    float boundAdjX = -1e9f, boundYBase = -1e9f;

    // Last applied geometry and fill colors, so state changes (hover,
    // press) don't rebuild anything.
    double geoW = -1.0, geoH = -1.0, geoRt = -1.0, geoRb = -1.0;
    std::vector<winrt::Windows::UI::Color> lastColors;
};
std::mutex g_blobEntriesMutex;
// Set under g_blobEntriesMutex by the uninit snapshot; checked under the
// same lock wherever new work (entries, hosts, posts) could start.
std::atomic<bool> g_unloading{false};
// BlobEntry holds a DispatcherTimer — a thread-affine XAML object — so this
// container must never reach the CRT's global destructors: Wh_ModUninit does
// not run when explorer.exe itself terminates, and destroying the vector on
// the shutdown thread would release the timers off the UI thread after XAML
// teardown. [[clang::no_destroy]] suppresses the destructor; the explicit
// reset() in Wh_ModBeforeUninit remains the release path for mod unload.
[[clang::no_destroy]] std::optional<std::vector<std::shared_ptr<BlobEntry>>>
    g_blobEntries{std::in_place};
// Swept hosts (taskbar RootGrids and tray grids) and their re-sweep
// subscriptions: a host grid resizes whenever its content changes (windows
// opening/closing, buttons realizing late, tray icons coming and going), so
// SizeChanged is the ongoing discovery event for elements that appear after
// the first sweep. Weak refs + tokens only — destructor-safe as a plain
// global. Guarded by g_blobEntriesMutex.
struct SweptHost {
    winrt::weak_ref<winrt::Windows::UI::Xaml::Controls::Grid> grid;
    winrt::event_token sizeToken{};
    // Cached tray grid for this taskbar's island, resolved lazily: spares
    // the depth-10 class search over the whole XAML root on every re-sweep.
    winrt::weak_ref<winrt::Windows::UI::Xaml::Controls::Grid> trayGrid;
};
std::vector<SweptHost> g_taskbarHosts;
std::vector<SweptHost> g_trayHosts;

// Every RunAsync post whose lambda lives in this DLL — hook refreshes, the
// settings-change re-sweep and per-entry refreshes, cross-thread orphan
// cleanup — takes a ticket here under g_blobEntriesMutex, next to the same
// g_unloading check the uninit snapshot uses, and releases it when the
// lambda finishes. Wh_ModBeforeUninit waits for the count to drain, closing
// the two windows the entry/host barrier cannot see: a post queued before
// its entry exists, and a post that lands behind the barrier's own items.
// The rule: nothing from the mod image may be running or scheduled once
// Wh_ModUninit returns.
std::atomic<int> g_pendingPosts{0};
HANDLE g_postsDrained = nullptr; // manual-reset, created in Wh_ModInit

bool PostToUiThread(winrt::Windows::UI::Core::CoreDispatcher const& dispatcher,
                    std::function<void()> fn) {
    {
        std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
        if (g_unloading) return false; // no new posts after the uninit snapshot
        g_pendingPosts.fetch_add(1);
    }
    auto release = []() {
        if (g_pendingPosts.fetch_sub(1) == 1 && g_postsDrained) SetEvent(g_postsDrained);
    };
    try {
        dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                            [fn = std::move(fn), release]() {
                                try { fn(); } catch (...) {}
                                release();
                            });
        return true;
    } catch (...) {
        release(); // a failed post must not leak a count
        return false;
    }
}

std::atomic<bool> g_taskbarViewDllLoaded{false};

std::optional<winrt::Windows::UI::Color> ParseHexColor(std::wstring_view hexView) {
    if (hexView.empty()) return std::nullopt;
    std::wstring hex(hexView);
    hex.erase(0, hex.find_first_not_of(L" \t\r\n"));
    if (hex.empty()) return std::nullopt;
    hex.erase(hex.find_last_not_of(L" \t\r\n") + 1);
    if (hex[0] == L'#') hex.erase(0, 1);
    if (hex.length() == 6) hex = L"FF" + hex;
    if (hex.length() != 8) return std::nullopt;
    for (wchar_t ch : hex) {
        if (!iswxdigit(ch)) return std::nullopt;
    }
    try {
        uint32_t val = std::stoul(hex, nullptr, 16);
        return winrt::Windows::UI::Color{
            (uint8_t)((val >> 24) & 0xFF),
            (uint8_t)((val >> 16) & 0xFF),
            (uint8_t)((val >> 8) & 0xFF),
            (uint8_t)(val & 0xFF)
        };
    } catch (...) {
        return std::nullopt;
    }
}

void ParseDoublePair(PCWSTR str, double& outLight, double& outDark, double defaultVal = 1.0) {
    outLight = defaultVal; outDark = defaultVal;
    if (!str || !str[0]) return;
    std::wstring ws(str);
    size_t comma = ws.find(L',');
    if (comma != std::wstring::npos) {
        try { outLight = std::stod(ws.substr(0, comma)); } catch (...) {}
        try { outDark = std::stod(ws.substr(comma + 1)); } catch (...) {}
    } else {
        try {
            outLight = std::stod(ws);
            outDark = outLight;
        } catch (...) {}
    }
}

double ParseDouble(PCWSTR str, double defaultVal, double minVal = 0.0) {
    if (!str || !str[0]) return defaultVal;
    try {
        double val = std::stod(str);
        if (std::isnan(val)) return defaultVal;
        return std::max(val, minVal);
    } catch (...) {
        return defaultVal;
    }
}

void ParseThickness(PCWSTR str, winrt::Windows::UI::Xaml::Thickness& outThickness) {
    double outL = 0.0, outT = 0.0, outR = 0.0, outB = 0.0;
    outThickness = winrt::Windows::UI::Xaml::ThicknessHelper::FromLengths(0, 0, 0, 0);
    if (!str) return;
    std::wstring ws(str);
    if (ws.empty()) return;

    std::vector<double> vals;
    size_t pos = 0;
    while (pos < ws.length()) {
        while (pos < ws.length() && (ws[pos] == L' ' || ws[pos] == L',')) pos++;
        if (pos >= ws.length()) break;
        size_t nextComma = ws.find(L',', pos);
        if (nextComma == std::wstring::npos) nextComma = ws.length();
        try {
            double val = std::stod(ws.substr(pos, nextComma - pos));
            if (std::isnan(val)) val = 0.0;
            vals.push_back(val);
        } catch (...) {}
        pos = nextComma + 1;
    }

    if (vals.size() == 1) {
        outL = outT = outR = outB = vals[0];
    } else if (vals.size() == 2) {
        // XAML convention: "horizontal,vertical"
        outL = outR = vals[0];
        outT = outB = vals[1];
    } else if (vals.size() == 4) {
        outL = vals[0]; outT = vals[1]; outR = vals[2]; outB = vals[3];
    }
    // Any other count (e.g. 3 values) is invalid and yields zeros rather
    // than silently discarding part of the input.

    outThickness = winrt::Windows::UI::Xaml::ThicknessHelper::FromLengths(outL, outT, outR, outB);
}

void ParseGradientColorPair(PCWSTR str, std::vector<winrt::Windows::UI::Color>& light, std::vector<winrt::Windows::UI::Color>& dark) {
    light.clear(); dark.clear();
    if (!str || !str[0]) return;
    std::wstring ws(str);

    size_t pipe = ws.find(L'|');
    std::wstring lightStr = (pipe != std::wstring::npos) ? ws.substr(0, pipe) : ws;
    std::wstring darkStr = (pipe != std::wstring::npos) ? ws.substr(pipe + 1) : ws;

    auto parseColors = [](std::wstring s, std::vector<winrt::Windows::UI::Color>& outList) {
        size_t pos = 0;
        while (pos < s.length()) {
            size_t next = s.find(L',', pos);
            std::wstring part = (next == std::wstring::npos) ? s.substr(pos) : s.substr(pos, next - pos);
            size_t start = part.find_first_not_of(L" \t\r\n");
            if (start != std::wstring::npos) part.erase(0, start);
            size_t end = part.find_last_not_of(L" \t\r\n");
            if (end != std::wstring::npos) part.erase(end + 1);
            if (!part.empty()) {
                auto c = ParseHexColor(part);
                if (c.has_value()) outList.push_back(c.value());
            }
            if (next == std::wstring::npos) break;
            pos = next + 1;
        }
    };

    parseColors(lightStr, light);
    parseColors(darkStr, dark);
}

void LoadSettings() {
    std::lock_guard<std::mutex> settingsLock(g_settingsMutex);

    WindhawkUtils::StringSetting dimStr(Wh_GetStringSetting(L"BlobShape.Dimensions"));
    g_settings.CustomWidth = -1.0;
    g_settings.CustomHeight = -1.0;
    if (dimStr.get()[0]) {
        std::wstring ws(dimStr.get());
        size_t comma = ws.find(L',');
        auto parseDim = [](std::wstring s) -> double {
            size_t start = s.find_first_not_of(L" \t\r\n");
            if (start != std::wstring::npos) s.erase(0, start);
            size_t end = s.find_last_not_of(L" \t\r\n");
            if (end != std::wstring::npos) s.erase(end + 1);
            if (s == L"auto" || s.empty()) return -1.0;
            try { return std::stod(s); } catch (...) { return -1.0; }
        };
        if (comma != std::wstring::npos) {
            g_settings.CustomWidth = parseDim(ws.substr(0, comma));
            g_settings.CustomHeight = parseDim(ws.substr(comma + 1));
        } else {
            g_settings.CustomWidth = parseDim(ws);
            g_settings.CustomHeight = g_settings.CustomWidth;
        }
    }

    WindhawkUtils::StringSetting marginStr(Wh_GetStringSetting(L"BlobShape.Margins"));
    g_settings.HasCustomMargin = false;
    if (marginStr.get()[0]) {
        ParseThickness(marginStr.get(), g_settings.CustomMargin);
        std::wstring ms(marginStr.get());
        size_t start = ms.find_first_not_of(L" \t\r\n");
        if (start != std::wstring::npos) g_settings.HasCustomMargin = true;
    }

    WindhawkUtils::StringSetting radiusStr(Wh_GetStringSetting(L"BlobShape.BottomRadius"));
    g_settings.BottomRadius = ParseDouble(radiusStr.get(), 4.0);

    WindhawkUtils::StringSetting topRadiusStr(Wh_GetStringSetting(L"BlobShape.TopRadius"));
    g_settings.TopRadius = ParseDouble(topRadiusStr.get(), 8.0);

    WindhawkUtils::StringSetting customCStr(Wh_GetStringSetting(L"Colors.CustomColor"));
    ParseGradientColorPair(customCStr.get(), g_settings.ParsedLightColor, g_settings.ParsedDarkColor);

    WindhawkUtils::StringSetting bgOpStr(Wh_GetStringSetting(L"Colors.BgOpacity"));
    ParseDoublePair(bgOpStr.get(), g_settings.BgOpacityLight, g_settings.BgOpacityDark, 1.0);

    g_settings.SystemButtonsBlob = Wh_GetIntSetting(L"SystemButtons.SystemButtonsBlob") != 0;
    g_settings.WidgetsBlob = Wh_GetIntSetting(L"SystemButtons.WidgetsBlob") != 0;
    g_settings.DateTimeBlob = Wh_GetIntSetting(L"SystemButtons.DateTimeBlob") != 0;
    g_settings.TrayButtonsBlob = Wh_GetIntSetting(L"SystemButtons.TrayButtonsBlob") != 0;
}

inline winrt::Windows::UI::Color ApplyOpacity(winrt::Windows::UI::Color c, double opacity) {
    if (opacity < 0.0) opacity = 0.0;
    if (opacity > 1.0) opacity = 1.0;
    c.A = static_cast<uint8_t>(c.A * opacity);
    return c;
}

// The taskbar switches themes at runtime via the element tree's ActualTheme;
// Application::Current().RequestedTheme() is fixed at startup, so it only
// serves as the fallback when the element reports Default.
bool IsElementLightMode(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
    if (!element) return false;
    try {
        auto theme = element.ActualTheme();
        if (theme == winrt::Windows::UI::Xaml::ElementTheme::Light) return true;
        if (theme == winrt::Windows::UI::Xaml::ElementTheme::Dark) return false;
        return winrt::Windows::UI::Xaml::Application::Current().RequestedTheme() == winrt::Windows::UI::Xaml::ApplicationTheme::Light;
    } catch (...) {
        return false;
    }
}

std::vector<winrt::Windows::UI::Color> GetBlobShapeColors(const Settings& localSettings, bool isLight) {
    double opacity = isLight ? localSettings.BgOpacityLight : localSettings.BgOpacityDark;

    auto c = isLight ? localSettings.ParsedLightColor : localSettings.ParsedDarkColor;
    if (!c.empty()) {
        for (auto& col : c) col = ApplyOpacity(col, opacity);
        return c;
    }

    // No custom color configured: fall back to the system accent color.
    auto res = winrt::Windows::UI::Xaml::Application::Current().Resources();
    auto resName = isLight ? L"SystemAccentColorDark1" : L"SystemAccentColorLight2";
    if (res.HasKey(winrt::box_value(resName))) {
        return {ApplyOpacity(winrt::unbox_value<winrt::Windows::UI::Color>(res.Lookup(winrt::box_value(resName))), opacity)};
    }
    return {ApplyOpacity({255, 0, 120, 212}, opacity)};
}

winrt::Windows::UI::Xaml::Media::Brush CreateBrush(const std::vector<winrt::Windows::UI::Color>& colors) {
    if (colors.empty()) return nullptr;
    if (colors.size() == 1) {
        return winrt::Windows::UI::Xaml::Media::SolidColorBrush(colors[0]);
    }

    // Multi-color values render as a horizontal gradient.
    winrt::Windows::UI::Xaml::Media::LinearGradientBrush brush;
    brush.StartPoint({0.0, 0.5});
    brush.EndPoint({1.0, 0.5});

    auto stops = brush.GradientStops();
    for (size_t i = 0; i < colors.size(); i++) {
        winrt::Windows::UI::Xaml::Media::GradientStop stop;
        stop.Color(colors[i]);
        stop.Offset(static_cast<double>(i) / (colors.size() - 1));
        stops.Append(stop);
    }
    return brush;
}

// Builds the blob shape:
//
//   ___________________________________
//   \                                 /   <- concave flares (Rt x Rt, circular)
//    |                               |
//    |          main area            |    <- W x H
//    \_______________________________/    <- convex bottom corners (Rb)
//
// Coordinate space: (0,0) is the top-left flare tip. The extension height
// equals the top corner radius, so the total size is
// (W + 2*Rt) x (H + Rt).
winrt::Windows::UI::Xaml::Media::PathGeometry BuildBlobShapeGeometry(double W, double H, double Rt, double Rb) {
    using winrt::Windows::UI::Xaml::Media::PathFigure;
    using winrt::Windows::UI::Xaml::Media::PathGeometry;
    using winrt::Windows::UI::Xaml::Media::LineSegment;
    using winrt::Windows::UI::Xaml::Media::ArcSegment;
    using winrt::Windows::UI::Xaml::Media::SweepDirection;

    if (W < 1.0) W = 1.0;
    if (H < 1.0) H = 1.0;
    if (Rt < 0.0) Rt = 0.0;
    Rb = std::clamp(Rb, 0.0, std::min(W / 2.0, H));

    double Wt = W + 2.0 * Rt; // total width including the flare tips
    double Ht = H + Rt;       // total height including the extension

    PathFigure fig;
    fig.StartPoint({0.0f, 0.0f});
    fig.IsClosed(true);
    fig.IsFilled(true);
    auto segs = fig.Segments();

    auto addLine = [&](double x, double y) {
        LineSegment s;
        s.Point({(float)x, (float)y});
        segs.Append(s);
    };
    auto addArc = [&](double x, double y, double rx, double ry, bool clockwise) {
        if (rx <= 0.0 || ry <= 0.0) { addLine(x, y); return; }
        ArcSegment s;
        s.Point({(float)x, (float)y});
        s.Size({(float)rx, (float)ry});
        s.SweepDirection(clockwise ? SweepDirection::Clockwise : SweepDirection::Counterclockwise);
        s.IsLargeArc(false);
        segs.Append(s);
    };

    addLine(Wt, 0.0);                            // top edge, left tip -> right tip
    addArc(Wt - Rt, Rt, Rt, Rt, false);          // top-right concave flare
    addLine(Wt - Rt, Ht - Rb);                   // right side down
    addArc(Wt - Rt - Rb, Ht, Rb, Rb, true);      // bottom-right convex corner
    addLine(Rt + Rb, Ht);                        // bottom edge
    addArc(Rt, Ht - Rb, Rb, Rb, true);           // bottom-left convex corner
    addLine(Rt, Rt);                             // left side up
    addArc(0.0, 0.0, Rt, Rt, false);             // top-left concave flare, back to start

    PathGeometry geo;
    geo.Figures().Append(fig);
    return geo;
}

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Hosting;

// Resolves the XAML element from the native TaskListButton pointer the same
// way the mods this derives from do (taskbar-elastic-pill, taskbar-labels):
// the IUnknown of the WinRT object sits at a fixed offset in the
// implementation type.
FrameworkElement GetFrameworkElementFromNative(void* pThis) {
    if (!pThis) return nullptr;
    try {
        void* iUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown iUnknown;
        winrt::copy_from_abi(iUnknown, iUnknownPtr);
        return iUnknown.try_as<FrameworkElement>();
    } catch (...) {
        return nullptr;
    }
}

// Resolves a WinRT interface pointer (e.g. a hook's sender argument) to a
// FrameworkElement via plain QueryInterface — unlike the native-pointer
// resolver above, no offset probing is needed because the pointer already
// IS an interface.
FrameworkElement GetFrameworkElementFromInterface(void* pInterface) {
    if (!pInterface) return nullptr;
    try {
        if (!*(void**)pInterface) return nullptr;
        ::IUnknown* pUnk = (::IUnknown*)pInterface;
        winrt::Windows::UI::Xaml::FrameworkElement result{nullptr};
        if (SUCCEEDED(pUnk->QueryInterface(winrt::guid_of<winrt::Windows::UI::Xaml::FrameworkElement>(), winrt::put_abi(result)))) {
            return result;
        }
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

FrameworkElement FindChildByName(FrameworkElement const& parent, std::wstring_view name, int depth = 0) {
    if (!parent || depth > 5) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (child.Name() == name) return child;
            auto result = FindChildByName(child, name, depth + 1);
            if (result) return result;
        }
    }
    return nullptr;
}

FrameworkElement FindDescendantByClass(FrameworkElement const& parent, std::wstring_view className, int maxDepth, int depth = 0) {
    if (!parent || depth > maxDepth) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (winrt::get_class_name(child) == className) return child;
            auto result = FindDescendantByClass(child, className, maxDepth, depth + 1);
            if (result) return result;
        }
    }
    return nullptr;
}

VisualStateGroup GetVisualStateGroup(FrameworkElement const& root, std::wstring_view groupName) {
    auto groups = VisualStateManager::GetVisualStateGroups(root);
    for (auto const& group : groups) {
        if (group.Name() == groupName) return group;
    }
    return nullptr;
}

// A system button counts as active while its flyout/experience is open.
// Start, Task view, Widgets and the search icon are ToggleButtons, and a
// present IsChecked value is authoritative in BOTH directions. Invariant:
// toggle entries only refresh on Checked/Unchecked, so their state MUST be
// read from IsChecked — a state-name read can catch CommonStates mid-
// transition and is never re-evaluated for a toggle, freezing the blob.
// The state-name scan below serves the NON-toggle elements: Checked* as a
// prefix for the tray family (OmniButton, ChevronIconView), the exact
// Active* set for the Experience family. The null fall-through is a cheap
// safety valve, not a degradation path — a two-state ToggleButton always
// holds a value, so a build where IsChecked stops tracking would report
// false; if that ever happens, the known fix is the elastic-pill approach
// (read the Active* names off the root panel and skip IsChecked).
bool IsSystemButtonChecked(FrameworkElement const& btn) {
    try {
        if (auto toggle = btn.try_as<winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton>()) {
            auto checked = toggle.IsChecked();
            if (checked) return checked.Value();
            // IsChecked holds no value: fall through to the state-name scan.
        }
    } catch (...) {}
    try {
        auto root = VisualTreeHelper::GetChildrenCount(btn) > 0
            ? VisualTreeHelper::GetChild(btn, 0).try_as<FrameworkElement>()
            : nullptr;
        if (root) {
            for (auto const& group : VisualStateManager::GetVisualStateGroups(root)) {
                auto st = group.CurrentState();
                if (!st) continue;
                auto stateName = st.Name();
                std::wstring_view name{stateName};
                // Checked* as a prefix (Checked/CheckedNormal/...) and the
                // exact Active* names — not substring/prefix-anything: a
                // loose match would read unrelated states on some future
                // template as "open".
                if (name.starts_with(L"Checked") ||
                    name == L"ActiveNormal" ||
                    name == L"ActivePointerOver" ||
                    name == L"ActivePressed") {
                    return true;
                }
            }
        }
    } catch (...) {}
    return false;
}

// Removes an element from its parent panel, if it has one.
void RemoveFromParentPanel(winrt::Windows::UI::Xaml::UIElement const& element) {
    try {
        if (auto parent = VisualTreeHelper::GetParent(element)) {
            if (auto panel = parent.try_as<Panel>()) {
                uint32_t index;
                if (panel.Children().IndexOf(element, index)) {
                    panel.Children().RemoveAt(index);
                }
            }
        }
    } catch (...) {}
}

// Hands rendering of the native indicator visuals back to XAML. Taskbar
// buttons use an IconPanel with "BackgroundElement"; system tray elements
// have no IconPanel and name their highlight "BackgroundBorder".
void RestoreNativeVisuals(winrt::Windows::UI::Xaml::FrameworkElement const& btn) {
    try {
        auto root = FindChildByName(btn, L"IconPanel");
        if (!root) root = btn;
        if (auto bg = FindChildByName(root, L"BackgroundElement")) {
            ElementCompositionPreview::GetElementVisual(bg).IsVisible(true);
        }
        if (auto bgBorder = FindChildByName(root, L"BackgroundBorder")) {
            ElementCompositionPreview::GetElementVisual(bgBorder).IsVisible(true);
        }
        if (auto indicator = FindChildByName(root, L"RunningIndicator")) {
            ElementCompositionPreview::GetElementVisual(indicator).IsVisible(true);
        }
    } catch (...) {}
}

// Inserts the blob below the visible content: directly below the
// TaskbarFrameRepeater on the taskbar, or at index 0 — the very bottom of
// the z-order, which can never cover content — on the tray grid, which has
// no repeater by design. On a taskbar host, a missing repeater is a
// structural surprise: returns false (fail safe) so the caller leaves the
// native indicator in charge, instead of hiding it in favor of a blob that
// may render behind the taskbar background. Never appends on top.
bool InsertBlobBelowRepeater(Grid const& grid, winrt::Windows::UI::Xaml::Shapes::Path const& blobShape, bool expectRepeater) {
    auto children = grid.Children();
    uint32_t count = children.Size();
    for (uint32_t i = 0; i < count; i++) {
        auto child = children.GetAt(i).try_as<FrameworkElement>();
        if (child && child.Name() == L"TaskbarFrameRepeater") {
            children.InsertAt(i, blobShape);
            return true;
        }
    }
    if (expectRepeater) {
        Wh_Log(L"TaskbarFrameRepeater not found as a direct RootGrid child");
        return false;
    }
    children.InsertAt(0, blobShape);
    return true;
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

struct BlobEntry;
std::shared_ptr<BlobEntry> FindOrCreateEntry(winrt::Windows::UI::Xaml::FrameworkElement const& button, bool createIfMissing = true);
Grid GetHostRootGrid(winrt::Windows::UI::Xaml::FrameworkElement const& button);
void SweepExistingButtons(Grid const& grid, const Settings& localSettings);
void EnsureBlobOnButton(winrt::Windows::UI::Xaml::FrameworkElement const& button, std::shared_ptr<BlobEntry> const& entry, winrt::Windows::UI::Xaml::FrameworkElement const& iconPanel, bool isActive, const Settings& localSettings);

int8_t ClassifyButton(winrt::Windows::UI::Xaml::FrameworkElement const& button) {
    auto cls = winrt::get_class_name(button);
    std::wstring_view clsView{cls};
    if (cls == L"Taskbar.TaskListButton") return BlobEntry::KindTask;
    if (cls == L"Taskbar.AugmentedEntryPointButton") return BlobEntry::KindWidget;
    if (clsView.starts_with(L"SystemTray.")) {
        return (button.Name() == L"NotificationCenterButton")
                   ? BlobEntry::KindDateTime
                   : BlobEntry::KindTray;
    }
    return BlobEntry::KindSystem;
}

bool IsTrayKind(int8_t kind) {
    return kind == BlobEntry::KindDateTime || kind == BlobEntry::KindTray;
}

bool IsKindEnabled(int8_t kind, const Settings& localSettings) {
    switch (kind) {
        case BlobEntry::KindTask:     return true;
        case BlobEntry::KindWidget:   return localSettings.WidgetsBlob;
        case BlobEntry::KindDateTime: return localSettings.DateTimeBlob;
        case BlobEntry::KindTray:     return localSettings.TrayButtonsBlob;
        case BlobEntry::KindSystem:   return localSettings.SystemButtonsBlob;
        default:                      return false; // fail closed for future kinds
    }
}

// The single entry point for every trigger (hook, SizeChanged, Loaded, stale
// Unloaded, theme change, settings change, sibling sweep): resolves the
// entry and its cached IconPanel, reads the running indicator state, and
// applies the blob. The recursive tree walk only runs when the cached
// element has expired or been detached.
void RefreshBlob(winrt::Windows::UI::Xaml::FrameworkElement const& button, const Settings& localSettings) {
    // FindOrCreateEntry returns null for both "no entry" and "unloading";
    // without this check the disabled-kind path below would classify, walk
    // the tree and re-register hosts into vectors uninit just cleared.
    if (g_unloading) return;

    auto entry = FindOrCreateEntry(button, false);
    if (!entry) {
        // No entry yet: classify first, and don't materialize one — with its
        // hidden Path, lifecycle handlers and state subscriptions — for a
        // kind whose toggle is off. Toggle-on later re-sweeps the hosts
        // (Wh_ModSettingsChanged), which creates the entries then.
        int8_t kind = ClassifyButton(button);
        if (!IsKindEnabled(kind, localSettings)) {
            // Still register this element's island as a sweep host: on a
            // taskbar whose ONLY discovery trigger is a system button (a
            // secondary monitor with no task-button activity), returning
            // bare would kill tray and date/time discovery there — and the
            // settings-change re-sweep could never recover it, because the
            // island would not be in the host list at all. Terminates: the
            // re-entrant RefreshBlob for this button hits the
            // firstTime == false path in SweepExistingButtons.
            if (!IsTrayKind(kind)) {
                if (auto grid = GetHostRootGrid(button)) {
                    SweepExistingButtons(grid, localSettings);
                }
            }
            return;
        }
        entry = FindOrCreateEntry(button);
        if (!entry) return; // unloading
        entry->kind = kind;
    }

    auto iconPanel = entry->iconPanel.get();
    if (!iconPanel || !VisualTreeHelper::GetParent(iconPanel)) {
        iconPanel = FindChildByName(button, L"IconPanel");
        if (!iconPanel && entry->kind != BlobEntry::KindTask) {
            // System/Widgets buttons have no IconPanel; the button itself
            // serves as the lookup root and sizing anchor fallback (their
            // BackgroundElement is resolved from here and usually becomes
            // the actual anchor).
            iconPanel = button;
        }
        entry->iconPanel = iconPanel ? winrt::make_weak(iconPanel)
                                     : winrt::weak_ref<FrameworkElement>{};
    }
    if (!iconPanel) return;

    bool isActive = false;
    if (entry->kind == BlobEntry::KindTask) {
        try {
            auto grp = GetVisualStateGroup(iconPanel, L"RunningIndicatorStates");
            auto st = grp ? grp.CurrentState() : nullptr;
            isActive = st && st.Name() == L"ActiveRunningIndicator";
        } catch (...) {}
    } else {
        // Gated per kind: with the toggle off, isActive stays false and the
        // normal show=false path hides the blob and restores the natives —
        // runtime toggling needs no teardown.
        bool enabled = IsKindEnabled(entry->kind, localSettings);
        // Like the native active highlight, the blob shows on every
        // monitor's instance of the button: checked state propagates across
        // taskbars, and each instance simply follows its own checked state.
        if (enabled) isActive = IsSystemButtonChecked(button);
    }
    EnsureBlobOnButton(button, entry, iconPanel, isActive, localSettings);
}

// Restores the BackgroundElement's visual a beat AFTER deactivation instead
// of immediately: at the deactivation moment the shell's press-release and
// state-transition storyboards are still animating it (often through their
// brightest keyframes, with the pointer still over the button), so revealing
// it in the same frame the blob hides flashes the native highlight. One-shot,
// never re-arms itself, and canceled if the button re-activates first —
// rapid task switching never lets the highlight through.
void ScheduleBgRestore(std::shared_ptr<BlobEntry> const& entry) {
    if (!entry->restoreTimer) {
        auto timer = winrt::Windows::UI::Xaml::DispatcherTimer();
        timer.Interval(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(100)));
        std::weak_ptr<BlobEntry> weakEntry = entry;
        timer.Tick([weakEntry](winrt::Windows::Foundation::IInspectable const& sender, auto const&) {
            // Self-stopping via the sender: the timer must go quiet even if
            // its entry is already gone — a started DispatcherTimer is kept
            // alive by the dispatcher, and a tick after mod unload would
            // land in an unloaded DLL.
            if (auto t = sender.try_as<winrt::Windows::UI::Xaml::DispatcherTimer>()) {
                try { t.Stop(); } catch (...) {}
            }
            auto e = weakEntry.lock();
            if (!e || g_unloading || !e->bgHidden) return;
            // bgElement is the element whose visual was actually hidden.
            if (auto bgEl = e->bgElement.get()) {
                try {
                    ElementCompositionPreview::GetElementVisual(bgEl).IsVisible(true);
                } catch (...) {}
            }
            e->bgHidden = false;
        });
        entry->restoreTimer = timer;
    }
    // Start() on a running timer restarts its countdown; repeated
    // inactive-state events (hover, press) must not push the restore back,
    // so the delay stays fixed from the deactivation that armed it.
    if (!entry->restoreTimer.IsEnabled()) entry->restoreTimer.Start();
}

void SweepTray(Grid const& trayGrid, const Settings& localSettings);

// The re-runnable sweep body: enumerates the taskbar repeater's realized
// children and reaches this island's system tray. Idempotent — RefreshBlob
// finds or creates entries.
void TaskbarSweepBody(Grid const& grid, const Settings& localSettings) {
    try {
        FrameworkElement repeater = nullptr;
        auto children = grid.Children();
        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i).try_as<FrameworkElement>();
            if (child && child.Name() == L"TaskbarFrameRepeater") {
                repeater = child;
                break;
            }
        }
        if (!repeater) return;

        int count = VisualTreeHelper::GetChildrenCount(repeater);
        for (int i = 0; i < count; i++) {
            auto child = VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
            if (!child) continue;
            auto cls = winrt::get_class_name(child);
            if (cls == L"Taskbar.TaskListButton" ||
                cls == L"Taskbar.ExperienceToggleButton" ||        // Start, Task view
                cls == L"Taskbar.AugmentedEntryPointButton") {     // Widgets
                try { RefreshBlob(child, localSettings); } catch (...) {}
            } else if (cls == L"Taskbar.TaskbarExtensionElement") {
                // Search is wrapped in an extension element; the actionable
                // button is the nested SearchUx SearchIconButton, named
                // "SearchIcon" in the "Search icon only" mode (the other
                // search modes use different inner controls and are
                // currently skipped).
                if (auto inner = FindChildByName(child, L"SearchIcon")) {
                    try { RefreshBlob(inner, localSettings); } catch (...) {}
                }
            }
        }
    } catch (...) {}

    // The system tray is a sibling frame under the same XAML root on current
    // builds — reach it from here, since no hook ever fires for its
    // elements. The resolved grid is cached on the host record; the class
    // search only runs when the cache is empty or its element died.
    try {
        Grid trayGrid{nullptr};
        {
            std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
            for (auto& host : g_taskbarHosts) {
                if (host.grid.get() == grid) {
                    trayGrid = host.trayGrid.get();
                    break;
                }
            }
        }
        if (!trayGrid) {
            auto xamlRoot = grid.XamlRoot();
            auto content = xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
            auto trayFrame = content ? FindDescendantByClass(content, L"SystemTray.SystemTrayFrame", 10) : nullptr;
            if (content && !trayFrame) {
                Wh_Log(L"SystemTray.SystemTrayFrame not found under the XAML root; tray blobs inactive for this taskbar");
            }
            trayGrid = trayFrame ? FindChildByName(trayFrame, L"SystemTrayFrameGrid").try_as<Grid>() : nullptr;
            if (trayGrid) {
                std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
                for (auto& host : g_taskbarHosts) {
                    if (host.grid.get() == grid) {
                        host.trayGrid = winrt::make_weak(trayGrid);
                        break;
                    }
                }
            }
        }
        if (trayGrid) SweepTray(trayGrid, localSettings);
    } catch (...) {}
}

// First contact with a taskbar grid: sweep it, and subscribe to its
// SizeChanged as the ongoing discovery event — the grid resizes whenever
// windows open or close and whenever buttons realize late, which is exactly
// when new sweepable elements can exist. Marked BEFORE sweeping, so the
// re-entrant RefreshBlob calls can't recurse.
void SweepExistingButtons(Grid const& grid, const Settings& localSettings) {
    bool firstTime = true;
    {
        std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
        for (auto it = g_taskbarHosts.begin(); it != g_taskbarHosts.end(); ) {
            auto g = it->grid.get();
            if (!g) { it = g_taskbarHosts.erase(it); continue; }
            if (g == grid) { firstTime = false; break; }
            ++it;
        }
        if (firstTime) g_taskbarHosts.push_back({winrt::make_weak(grid), {}});
    }

    if (firstTime) {
        auto weakGrid = winrt::make_weak(grid);
        auto token = grid.SizeChanged([weakGrid](auto const&, auto const&) {
            if (g_unloading) return;
            auto g = weakGrid.get();
            if (!g) return;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try { TaskbarSweepBody(g, localSettings); } catch (...) {}
        });
        bool stored = false;
        {
            std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
            if (!g_unloading) {
                for (auto& host : g_taskbarHosts) {
                    if (host.grid.get() == grid) { host.sizeToken = token; stored = true; break; }
                }
            }
        }
        if (!stored) {
            // Untracked subscription: revoke immediately.
            try { grid.SizeChanged(token); } catch (...) {}
        }
        TaskbarSweepBody(grid, localSettings);
    }
}

bool IsUnderElementNamed(FrameworkElement const& element, std::wstring_view name, int maxUp) {
    FrameworkElement current = element;
    for (int i = 0; i < maxUp && current; i++) {
        auto parent = VisualTreeHelper::GetParent(current);
        current = parent ? parent.try_as<FrameworkElement>() : nullptr;
        if (current && current.Name() == name) return true;
    }
    return false;
}

// Collects the tray elements that get blobs. Matched buttons are not
// descended into — the IconViews nested inside an OmniButton (e.g. the
// control center's wifi/volume/battery glyphs) belong to their button, not
// to themselves.
//
// Deliberately excluded, because they have no checked state to ever show a
// blob for (verified: their state groups are hover/press feedback only):
// - NotifyIconView (notification area app icons): these are Win32
//   Shell_NotifyIcon icons — a one-way protocol that forwards clicks to the
//   owning app with no feedback channel, so the shell cannot know whether
//   anything is "open".
// - IconViews under MainStack (microphone, location, ...): tested on
//   current builds, these never entered a Checked*/Active* state when
//   interacted with (the shared IconView template does STYLE Checked*
//   states, but these instances were not observed to enter them), so a
//   blob could never show for them.
// - The Show Desktop IconView (under ShowDesktopStack): no flyout at all.
// Skipping them entirely avoids per-icon entries, hidden Paths, and
// hover-driven state subscriptions that could never produce a visible blob.
void CollectTrayButtons(FrameworkElement const& root, std::vector<FrameworkElement>& out, int depth = 0) {
    if (!root || depth > 12) return;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) continue;
        auto cls = winrt::get_class_name(child);
        if (cls == L"SystemTray.OmniButton" ||        // date/time, control center
            cls == L"SystemTray.ChevronIconView") {   // overflow chevron
            out.push_back(child);
            continue;
        }
        if (cls == L"SystemTray.IconView") {          // stack icons (language, ...)
            if (!IsUnderElementNamed(child, L"MainStack", 8) &&
                !IsUnderElementNamed(child, L"ShowDesktopStack", 8)) {
                out.push_back(child);
            }
            continue;
        }
        CollectTrayButtons(child, out, depth + 1);
    }
}

void TrayRefreshAll(Grid const& trayGrid, const Settings& localSettings) {
    std::vector<FrameworkElement> buttons;
    try { CollectTrayButtons(trayGrid, buttons); } catch (...) {}
    for (auto& b : buttons) {
        try { RefreshBlob(b, localSettings); } catch (...) {}
    }
}

// First contact with a tray grid: sweep it and subscribe to its SizeChanged
// as the ongoing discovery event — the tray resizes exactly when
// notification-area icons appear or disappear.
void SweepTray(Grid const& trayGrid, const Settings& localSettings) {
    // With both tray kinds disabled there is nothing to materialize, and
    // registering the host would keep the depth-12 collection walk running
    // on every tray resize for nothing. Safe to skip entirely: toggling
    // either kind back on re-sweeps the taskbar hosts
    // (Wh_ModSettingsChanged), which comes back through here.
    if (!localSettings.DateTimeBlob && !localSettings.TrayButtonsBlob) return;

    bool firstTime = true;
    {
        std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
        for (auto it = g_trayHosts.begin(); it != g_trayHosts.end(); ) {
            auto g = it->grid.get();
            if (!g) { it = g_trayHosts.erase(it); continue; }
            if (g == trayGrid) { firstTime = false; break; }
            ++it;
        }
        if (firstTime) g_trayHosts.push_back({winrt::make_weak(trayGrid), {}});
    }

    if (firstTime) {
        auto weakGrid = winrt::make_weak(trayGrid);
        auto token = trayGrid.SizeChanged([weakGrid](auto const&, auto const&) {
            if (g_unloading) return;
            auto g = weakGrid.get();
            if (!g) return;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try { TrayRefreshAll(g, localSettings); } catch (...) {}
        });
        bool stored = false;
        {
            std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
            if (!g_unloading) {
                for (auto& host : g_trayHosts) {
                    if (host.grid.get() == trayGrid) { host.sizeToken = token; stored = true; break; }
                }
            }
        }
        if (!stored) {
            // Untracked subscription: revoke immediately.
            try { trayGrid.SizeChanged(token); } catch (...) {}
        }
    }

    TrayRefreshAll(trayGrid, localSettings);
}

std::shared_ptr<BlobEntry> FindOrCreateEntry(winrt::Windows::UI::Xaml::FrameworkElement const& button, bool createIfMissing) {
    std::vector<std::shared_ptr<BlobEntry>> orphans;
    std::shared_ptr<BlobEntry> result;
    {
        std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
        if (g_unloading || !g_blobEntries) return nullptr; // callers bail on null

        // Prune entries whose button died without an Unloaded (rare). Their
        // blob elements live in the RootGrid, so they must be removed
        // explicitly — and an armed restore timer must be stopped, since a
        // started DispatcherTimer is kept alive by its dispatcher, not by
        // the entry. This runs on every lookup — the list is small since
        // Unloaded drops entries eagerly.
        for (auto it = g_blobEntries->begin(); it != g_blobEntries->end(); ) {
            auto btn = (*it)->button.get();
            if (!btn) {
                orphans.push_back(*it);
                it = g_blobEntries->erase(it);
                continue;
            }
            if (btn == button) result = *it;
            ++it;
        }
        if (!result && createIfMissing) {
            result = std::make_shared<BlobEntry>();
            result->button = winrt::make_weak(button);
            g_blobEntries->push_back(result);
        }
    }

    for (auto& orphan : orphans) {
        auto blob = orphan->blobShape.get();
        auto dispatcher = blob ? blob.Dispatcher() : nullptr;
        if (dispatcher && dispatcher.HasThreadAccess()) {
            // Already on the blob's UI thread (the normal case — this runs
            // from RefreshBlob). Inline is not just cheaper, it's the only
            // variant the unload barrier covers: orphans are erased from
            // g_blobEntries before this point, so a POSTED lambda would be
            // invisible to Wh_ModBeforeUninit's pending count and could run
            // after the DLL is gone. Inline removal is safe here — nothing
            // above this call is iterating grid.Children() (TaskbarSweepBody
            // has broken out of its child loop; TrayRefreshAll iterates a
            // pre-collected vector).
            if (orphan->restoreTimer) {
                try { orphan->restoreTimer.Stop(); } catch (...) {}
                orphan->restoreTimer = nullptr;
            }
            try {
                ElementCompositionPreview::GetElementVisual(blob).Properties().StopAnimation(L"Translation");
            } catch (...) {}
            RemoveFromParentPanel(blob);
        } else if (dispatcher) {
            bool posted = PostToUiThread(dispatcher, [orphan, blob]() {
                // No unload bail here: this orphan was erased from
                // g_blobEntries before the post, so the uninit snapshot
                // never sees it — this lambda is the only code that will
                // ever stop its timer, and its ticket guarantees it runs
                // before the DLL goes away.
                if (orphan->restoreTimer) {
                    try { orphan->restoreTimer.Stop(); } catch (...) {}
                    orphan->restoreTimer = nullptr;
                }
                try {
                    ElementCompositionPreview::GetElementVisual(blob).Properties().StopAnimation(L"Translation");
                } catch (...) {}
                RemoveFromParentPanel(blob);
            });
            if (!posted && orphan->restoreTimer) {
                // Post refused (uninit already started): best-effort inline
                // stop, like the no-dispatcher branch; the tick's sender
                // self-stop backstops a failed cross-thread Stop().
                try { orphan->restoreTimer.Stop(); } catch (...) {}
                orphan->restoreTimer = nullptr;
            }
        } else if (orphan->restoreTimer) {
            // No dispatcher handle (island likely gone): best-effort inline
            // stop. The tick also self-stops via its sender, so a failed
            // cross-thread Stop() still goes quiet on the next tick.
            try { orphan->restoreTimer.Stop(); } catch (...) {}
            orphan->restoreTimer = nullptr;
        }
    }
    return result;
}

// Locates the hosting grid for a button by walking up to its frame:
// Taskbar.TaskbarFrame -> RootGrid for taskbar buttons, or
// SystemTray.SystemTrayFrame -> SystemTrayFrameGrid for tray elements.
// Returns nullptr while the button isn't rooted yet — the next state change
// or SizeChanged retries.
Grid GetHostRootGrid(winrt::Windows::UI::Xaml::FrameworkElement const& button) {
    FrameworkElement current = button;
    int depth = 0;
    while (current && depth < 20) {
        auto cls = winrt::get_class_name(current);
        if (cls == L"Taskbar.TaskbarFrame") {
            auto rootGrid = FindChildByName(current, L"RootGrid");
            return rootGrid ? rootGrid.try_as<Grid>() : nullptr;
        }
        if (cls == L"SystemTray.SystemTrayFrame") {
            auto rootGrid = FindChildByName(current, L"SystemTrayFrameGrid");
            return rootGrid ? rootGrid.try_as<Grid>() : nullptr;
        }
        auto parent = VisualTreeHelper::GetParent(current);
        current = parent ? parent.try_as<FrameworkElement>() : nullptr;
        depth++;
    }
    return nullptr;
}

// Glues the blob's Translation to its button's visual offset chain with a
// composition ExpressionAnimation. Only the X axis is dynamic:
//   Translation.X = sum(chain Offset.X up to RootGrid) + adjX - self.Offset.X
//   Translation.Y = yBase - self.Offset.Y   (constant: taskbar top + margins)
// X tracking keeps the blob glued through reordering and reflow slides, while
// the flat top edge stays anchored to the top of the taskbar — Y never
// follows the button, so entrance animations ("Animation Effects") and
// mid-transition layout can't displace or freeze the shape vertically.
// Bound once per blob and never retargeted. Returns false while the chain
// can't be resolved (button not fully in the tree yet).
bool BindBlobExpression(
    winrt::Windows::UI::Xaml::Shapes::Path const& blobShape,
    Grid const& grid,
    winrt::Windows::UI::Xaml::FrameworkElement const& button,
    float adjX, float yBase)
{
    try {
        FrameworkElement gridElem = grid;
        std::vector<winrt::Windows::UI::Composition::Visual> chain;
        FrameworkElement e = button;
        int depth = 0;
        while (e && e != gridElem && depth < 15) {
            chain.push_back(ElementCompositionPreview::GetElementVisual(e));
            auto parent = VisualTreeHelper::GetParent(e);
            e = parent ? parent.try_as<FrameworkElement>() : nullptr;
            depth++;
        }
        if (!e || e != gridElem || chain.empty()) return false;

        auto vis = ElementCompositionPreview::GetElementVisual(blobShape);

        std::wstring expr = L"Vector3(";
        for (size_t i = 0; i < chain.size(); i++) {
            expr += L"p" + std::to_wstring(i) + L".Offset.X + ";
        }
        expr += L"adjX - self.Offset.X, yBase - self.Offset.Y, 0.0f)";

        auto exp = vis.Compositor().CreateExpressionAnimation(winrt::hstring(expr));
        for (size_t i = 0; i < chain.size(); i++) {
            exp.SetReferenceParameter(winrt::hstring(L"p" + std::to_wstring(i)), chain[i]);
        }
        exp.SetScalarParameter(L"adjX", adjX);
        exp.SetScalarParameter(L"yBase", yBase);
        exp.SetReferenceParameter(L"self", vis);
        vis.Properties().StartAnimation(L"Translation", exp);
        return true;
    } catch (...) {
        return false;
    }
}

// Creates (or updates) the blob shape for a single button. The shape is
// hosted in the taskbar's RootGrid — above the task list's clipping region,
// so the flare tips render fully — and glued to its button with a one-time
// composition expression. Activation is purely an opacity toggle on the
// button's own blob.
void EnsureBlobOnButton(winrt::Windows::UI::Xaml::FrameworkElement const& button, std::shared_ptr<BlobEntry> const& entry, winrt::Windows::UI::Xaml::FrameworkElement const& iconPanel, bool isActive, const Settings& localSettings) {
    auto bg = entry->bgElement.get();
    if (!bg || !VisualTreeHelper::GetParent(bg)) {
        bg = FindChildByName(iconPanel, L"BackgroundElement");
        if (!bg) bg = FindChildByName(iconPanel, L"BackgroundBorder"); // tray naming
        entry->bgElement = bg ? winrt::make_weak(bg)
                              : winrt::weak_ref<FrameworkElement>{};
    }
    FrameworkElement anchor = bg ? bg : iconPanel;

    // Suppression of the native background is decided at the END, from the
    // same condition that shows the blob, so no failure path can leave an
    // active button with neither indicator. The hand-off visual's IsVisible
    // flag is used instead of a local Opacity because the
    // RunningIndicatorStates transition storyboards hold ANIMATED values on
    // BackgroundElement, which outrank local values in XAML's precedence.
    // The RunningIndicator line is part of the button and renders above the
    // blob (which sits below the repeater in z-order), so it is suppressed
    // together with the background while the blob is shown. Tracked per
    // entry, so only state WE changed is ever touched or restored.
    auto runningIndicator = entry->indicatorElement.get();
    if (!runningIndicator || !VisualTreeHelper::GetParent(runningIndicator)) {
        runningIndicator = FindChildByName(iconPanel, L"RunningIndicator");
        entry->indicatorElement = runningIndicator ? winrt::make_weak(runningIndicator)
                                                   : winrt::weak_ref<FrameworkElement>{};
    }
    auto setNativeHidden = [&](bool hidden) {
        if (hidden) {
            // Cancel any pending delayed restore: re-activation while it's
            // in flight must keep the background suppressed.
            if (entry->restoreTimer) {
                try { entry->restoreTimer.Stop(); } catch (...) {}
            }
            if (bg && !entry->bgHidden) {
                try {
                    ElementCompositionPreview::GetElementVisual(bg).IsVisible(false);
                    entry->bgHidden = true;
                } catch (...) {}
            }
            if (runningIndicator && !entry->indicatorHidden) {
                try {
                    ElementCompositionPreview::GetElementVisual(runningIndicator).IsVisible(false);
                    entry->indicatorHidden = true;
                } catch (...) {}
            }
        } else {
            // The running dash must reflect state immediately; the
            // background follows on a delay so the deactivation storyboards
            // finish out of sight (no white highlight flash).
            if (runningIndicator && entry->indicatorHidden) {
                try {
                    ElementCompositionPreview::GetElementVisual(runningIndicator).IsVisible(true);
                    entry->indicatorHidden = false;
                } catch (...) {}
            }
            if (entry->bgHidden) ScheduleBgRestore(entry);
        }
    };

    // Button removal (window moved to another monitor, app closed, container
    // recycled) is a LIFECYCLE event, not a state change — UpdateVisualStates
    // never fires a final "inactive" for it. On a genuine Unloaded, tear the
    // blob down completely: stop its expression (which holds references to
    // the whole visual chain), unparent it, restore the native indicator,
    // and drop the entry. The create path rebuilds everything if the
    // container is used again later.
    if (!entry->unloadAttached) {
        entry->unloadAttached = true;
        std::weak_ptr<BlobEntry> weakEntry = entry;
        entry->unloadToken = button.Unloaded([weakEntry](auto const&, auto const&) {
            if (g_unloading) return;
            auto e = weakEntry.lock();
            if (!e) return;
            auto btn = e->button.get();

            // XAML delivers Unloaded asynchronously: a container that was
            // detached and immediately re-attached (recycling, cross-monitor
            // moves, pinned->running item swaps) receives a STALE Unloaded
            // while it is already live again — possibly right after the hook
            // built its blob. Tearing down on a stale event destroys a valid
            // blob with no event left to rebuild it. If the button is still
            // loaded, re-resolve and rebind against the settled tree instead
            // of tearing down.
            bool stillLoaded = false;
            if (btn) {
                try { stillLoaded = btn.IsLoaded(); } catch (...) {}
            }
            if (stillLoaded) {
                e->bound = false;
                e->grid = nullptr; // may sit under a different taskbar now
                Settings localSettings;
                { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
                try {
                    RefreshBlob(btn, localSettings);
                } catch (...) {}
                return;
            }

            if (auto blob = e->blobShape.get()) {
                try { blob.ActualThemeChanged(e->themeToken); } catch (...) {}
                try {
                    ElementCompositionPreview::GetElementVisual(blob).Properties().StopAnimation(L"Translation");
                } catch (...) {}
                RemoveFromParentPanel(blob);
            }
            if (e->restoreTimer) {
                try { e->restoreTimer.Stop(); } catch (...) {}
                e->restoreTimer = nullptr;
            }
            if (btn) {
                if (e->bgHidden || e->indicatorHidden) RestoreNativeVisuals(btn);
                if (auto anchor = e->anchor.get()) {
                    try { anchor.SizeChanged(e->sizeToken); } catch (...) {}
                }
                try { btn.Unloaded(e->unloadToken); } catch (...) {}
                if (e->loadedAttached) {
                    try { btn.Loaded(e->loadedToken); } catch (...) {}
                }
                if (e->systemEventsAttached) {
                    if (auto toggle = btn.try_as<winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton>()) {
                        try { toggle.Checked(e->checkedToken); } catch (...) {}
                        try { toggle.Unchecked(e->uncheckedToken); } catch (...) {}
                    }
                    for (auto& groupToken : e->stateTokens) {
                        if (auto group = groupToken.first.get()) {
                            try { group.CurrentStateChanged(groupToken.second); } catch (...) {}
                        }
                    }
                }
            }
            std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
            if (!g_blobEntries) return; // reset by Wh_ModBeforeUninit
            g_blobEntries->erase(
                std::remove_if(g_blobEntries->begin(), g_blobEntries->end(),
                    [&](auto& x) { return x == e; }),
                g_blobEntries->end());
        });
    }

    // Loaded fires after a (re)attached button has been measured and
    // arranged — the moment to drop the grid cache and rebind the X chain:
    // after a cross-taskbar reattach the cached weak ref can still resolve
    // to the OLD (alive) grid, which would make the chain walk fail
    // silently.
    if (!entry->loadedAttached) {
        entry->loadedAttached = true;
        std::weak_ptr<BlobEntry> weakLoadedEntry = entry;
        entry->loadedToken = button.Loaded([weakLoadedEntry](auto const&, auto const&) {
            if (g_unloading) return;
            auto e = weakLoadedEntry.lock();
            if (!e) return;
            auto btn = e->button.get();
            if (!btn) return;
            e->bound = false;
            e->grid = nullptr;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try {
                RefreshBlob(btn, localSettings);
            } catch (...) {}
        });
    }

    // System buttons never pass through the TaskListButton hook, so their
    // activation is event-driven: ToggleButton Checked/Unchecked when
    // available, otherwise CurrentStateChanged on every visual state group
    // of the template root.
    if (entry->kind != BlobEntry::KindTask && !entry->systemEventsAttached) {
        std::weak_ptr<BlobEntry> weakSysEntry = entry;
        auto refresh = [weakSysEntry]() {
            if (g_unloading) return;
            auto e = weakSysEntry.lock();
            if (!e) return;
            auto btn = e->button.get();
            if (!btn) return;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try { RefreshBlob(btn, localSettings); } catch (...) {}
        };
        bool attached = false;
        if (auto toggle = button.try_as<winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton>()) {
            try {
                entry->checkedToken = toggle.Checked([refresh](auto const&, auto const&) { refresh(); });
                entry->uncheckedToken = toggle.Unchecked([refresh](auto const&, auto const&) { refresh(); });
                attached = true;
            } catch (...) {
                // Partial subscription (Unchecked threw after Checked took):
                // revoke it, or the retry would overwrite checkedToken and
                // leave the first subscription firing forever, unrevokable —
                // including at unload.
                if (entry->checkedToken) {
                    try { toggle.Checked(entry->checkedToken); } catch (...) {}
                    entry->checkedToken = {};
                }
            }
        } else {
            try {
                auto root = VisualTreeHelper::GetChildrenCount(button) > 0
                    ? VisualTreeHelper::GetChild(button, 0).try_as<FrameworkElement>()
                    : nullptr;
                if (root) {
                    for (auto const& group : VisualStateManager::GetVisualStateGroups(root)) {
                        auto token = group.CurrentStateChanged([refresh](auto const&, auto const&) { refresh(); });
                        entry->stateTokens.push_back({winrt::make_weak(group), token});
                    }
                }
            } catch (...) {}
            attached = !entry->stateTokens.empty();
        }
        // Only latch when something was actually attached: a not-yet-templated
        // element (zero visual children during startup or an early sweep) has
        // nothing to subscribe to yet, and must retry on its next refresh
        // (Loaded, sweep, SizeChanged) instead of staying event-less forever.
        entry->systemEventsAttached = attached;
    }

    auto grid = entry->grid.get();
    if (!grid) {
        grid = GetHostRootGrid(button);
        if (!grid) {
            // Not rooted under a known frame — e.g. buttons in the overflow
            // flyout live in a separate XAML island. Hide any existing blob
            // and leave the native indicator fully in charge there.
            if (auto blob = entry->blobShape.get()) blob.Opacity(0.0);
            setNativeHidden(false);
            return;
        }
        entry->grid = winrt::make_weak(grid);
        if (!IsTrayKind(entry->kind)) {
            // First contact with this taskbar: bring its pre-existing
            // buttons up. Tray grids are swept by SweepTray instead —
            // registering them here would add a taskbar-host SizeChanged
            // subscription whose body always no-ops on a tray grid.
            SweepExistingButtons(grid, localSettings);
        }
    }

    auto blobShape = entry->blobShape.get();
    if (!blobShape) {
        blobShape = winrt::Windows::UI::Xaml::Shapes::Path();
        blobShape.Name(L"BlobShape");
        blobShape.IsHitTestVisible(false);
        blobShape.Stretch(winrt::Windows::UI::Xaml::Media::Stretch::None);
        blobShape.HorizontalAlignment(HorizontalAlignment::Left);
        blobShape.VerticalAlignment(VerticalAlignment::Top);
        // The negative right/bottom margins cancel the Path's contribution
        // to the grid's desired size, so the blob never affects taskbar
        // layout; positioning is done entirely via the Translation facade.
        blobShape.Margin(winrt::Windows::UI::Xaml::ThicknessHelper::FromLengths(0, 0, -1000, -1000));
        blobShape.Opacity(0.0);

        bool expectRepeater = !IsTrayKind(entry->kind);
        if (!InsertBlobBelowRepeater(grid, blobShape, expectRepeater)) {
            setNativeHidden(false); // no blob will be shown; retried on the next event
            return;
        }
        ElementCompositionPreview::SetIsTranslationEnabled(blobShape, true);

        {
            // Written on the UI thread, resolved on the Windhawk thread
            // (uninit, settings change): keep the cross-thread access to
            // this weak ref defined by writing it under the entries mutex.
            std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
            entry->blobShape = winrt::make_weak(blobShape);
        }
        entry->bound = false;
        entry->boundAdjX = -1e9f;
        entry->boundYBase = -1e9f;
        entry->geoW = -1.0;

        // Repaint on theme switches: without this, untouched buttons would
        // keep the previous theme's fill until their next state change.
        std::weak_ptr<BlobEntry> weakThemeEntry = entry;
        entry->themeToken = blobShape.ActualThemeChanged([weakThemeEntry](auto const&, auto const&) {
            if (g_unloading) return;
            auto e = weakThemeEntry.lock();
            if (!e) return;
            auto btn = e->button.get();
            if (!btn) return;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try {
                RefreshBlob(btn, localSettings);
            } catch (...) {}
        });
    } else {
        // If the button was re-hosted under a different taskbar's grid
        // (containers recycled across monitors), move the blob with it and
        // force a rebind against the new chain.
        auto parent = VisualTreeHelper::GetParent(blobShape);
        auto parentGrid = parent ? parent.try_as<Grid>() : nullptr;
        if (parentGrid != grid) {
            RemoveFromParentPanel(blobShape);
            entry->bound = false;
            bool expectRepeater = !IsTrayKind(entry->kind);
            if (!InsertBlobBelowRepeater(grid, blobShape, expectRepeater)) {
                setNativeHidden(false); // unparented renders nothing; retried on the next event
                return;
            }
        }
    }

    // Track the anchor's size so late layout (buttons created before their
    // first measure) and size changes re-apply geometry and binding. This is
    // a plain XAML event — no timers.
    if (entry->anchor.get() != anchor) {
        if (auto oldAnchor = entry->anchor.get()) {
            try { oldAnchor.SizeChanged(entry->sizeToken); } catch (...) {}
        }
        entry->anchor = winrt::make_weak(anchor);
        std::weak_ptr<BlobEntry> weakEntry = entry;
        entry->sizeToken = anchor.SizeChanged([weakEntry](auto const&, auto const&) {
            if (g_unloading) return;
            auto e = weakEntry.lock();
            if (!e) return;
            auto btn = e->button.get();
            if (!btn) return;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try {
                RefreshBlob(btn, localSettings);
            } catch (...) {}
        });
    }

    // Fill color — compared against the last applied color list so gradient
    // configurations don't rebuild a brush (and its stops) on every
    // hover/press state change.
    std::vector<winrt::Windows::UI::Color> newColors = GetBlobShapeColors(localSettings, IsElementLightMode(button));
    bool sameColor = blobShape.Fill() && newColors.size() == entry->lastColors.size();
    if (sameColor) {
        for (size_t i = 0; i < newColors.size(); i++) {
            auto& a = newColors[i];
            auto& b = entry->lastColors[i];
            if (a.A != b.A || a.R != b.R || a.G != b.G || a.B != b.B) {
                sameColor = false;
                break;
            }
        }
    }
    if (!sameColor) {
        blobShape.Fill(CreateBrush(newColors));
        entry->lastColors = std::move(newColors);
    }

    // Geometry and expression binding, once the anchor has a real size.
    double bW = anchor.ActualWidth();
    double bH = anchor.ActualHeight();
    if (bW > 0.001) {
        double W = localSettings.CustomWidth >= 0.0 ? localSettings.CustomWidth : bW;
        double H = localSettings.CustomHeight >= 0.0 ? localSettings.CustomHeight : bH;
        double W2 = std::max(1.0, W);
        double H2 = std::max(1.0, H);
        double Rt = std::max(0.0, localSettings.TopRadius);
        double Rb = localSettings.BottomRadius;

        if (std::abs(entry->geoW - W2) > 0.01 || std::abs(entry->geoH - H2) > 0.01 ||
            std::abs(entry->geoRt - Rt) > 0.01 || std::abs(entry->geoRb - Rb) > 0.01 ||
            !blobShape.Data()) {
            blobShape.Data(BuildBlobShapeGeometry(W2, H2, Rt, Rb));
            blobShape.Width(W2 + 2.0 * Rt);
            blobShape.Height(H2 + Rt);
            entry->geoW = W2; entry->geoH = H2;
            entry->geoRt = Rt; entry->geoRb = Rb;
        }

        // X adjustment relative to the button's left edge: the anchor's
        // horizontal offset within the button (rounded, so intra-button
        // render transforms can't jitter it), centering for a custom width,
        // and the flare tip offset.
        float adjX = (float)(-Rt);
        if (localSettings.CustomWidth >= 0.0) adjX += (float)((bW - W) / 2.0);
        if (anchor != button) {
            try {
                auto intra = anchor.TransformToVisual(button).TransformPoint({0, 0});
                adjX += std::round(intra.X);
            } catch (...) {}
        }

        // The Y coordinate is a structural constant: the flat top edge is
        // anchored to the top of the taskbar (the RootGrid origin), not to
        // the button — no element position is ever sampled for Y, so no
        // transition can displace or freeze the shape vertically. Margins
        // shift it from that anchor; the body hangs downward by Height.
        float yBase = 0.0f;
        if (localSettings.HasCustomMargin) {
            adjX += (float)(localSettings.CustomMargin.Left - localSettings.CustomMargin.Right);
            yBase += (float)(localSettings.CustomMargin.Top - localSettings.CustomMargin.Bottom);
        }
        yBase = std::round(yBase);

        if (!entry->bound ||
            std::abs(entry->boundAdjX - adjX) > 0.5f ||
            std::abs(entry->boundYBase - yBase) > 0.5f) {
            if (BindBlobExpression(blobShape, grid, button, adjX, yBase)) {
                entry->bound = true;
                entry->boundAdjX = adjX;
                entry->boundYBase = yBase;
            }
        }
    }

    // One condition drives both the blob and the native indicator: the
    // native background only disappears when the blob is actually shown in
    // its place, and the blob never renders at a stale or unbound position.
    bool show = isActive && entry->bound;
    blobShape.Opacity(show ? 1.0 : 0.0);
    setNativeHidden(show);

    // Idle blobs don't evaluate: one live expression per button would keep
    // ~20 per-frame evaluations running on the render thread for shapes at
    // Opacity(0). Stop on deactivation; the bound flag already drives the
    // cheap rebind on the next activation.
    if (!isActive && entry->bound) {
        try {
            ElementCompositionPreview::GetElementVisual(blobShape).Properties().StopAnimation(L"Translation");
        } catch (...) {}
        entry->bound = false;
    }
}

void DispatchElementRefresh(winrt::Windows::UI::Xaml::FrameworkElement const& elem) {
    auto dispatcher = elem.Dispatcher();
    if (!dispatcher) return;
    auto weakElem = winrt::make_weak(elem);

    PostToUiThread(dispatcher, [weakElem]() {
        if (g_unloading) return;
        try {
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }

            auto button = weakElem.get();
            if (!button) return;

            RefreshBlob(button, localSettings);
        } catch (...) {
            Wh_Log(L"Exception in UpdateVisualStates hook");
        }
    });
}

void DispatchButtonRefresh(void* pThis, PCWSTR source) {
    if (g_unloading) return;

    auto elem = GetFrameworkElementFromNative(pThis);
    if (!elem) {
        // Rare enough to log: the fixed-offset resolver may not fit every
        // implementation class on every build.
        Wh_Log(L"%s: element resolution failed", source);
        return;
    }
    DispatchElementRefresh(elem);
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    DispatchButtonRefresh(pThis, L"TaskListButton::UpdateVisualStates");
}

// System-button events, the way the Elastic lineage does it: the buttons'
// own UpdateVisualStates is not a hookable symbol, but TaskbarResources'
// OnExperienceToggleButtonVisualStateChanged callback fires for every
// ExperienceToggleButton state change on EVERY taskbar island, delivering
// the button as the sender argument — a plain WinRT interface, resolved
// with QueryInterface, no offset probing. This is what discovers
// secondary-monitor Start/Task view buttons, whose islands may never
// produce a task-button event: the first state change (even a hover)
// creates the entry and sweeps the island, tray included.
using OnExperienceToggleButtonVisualStateChanged_t = void(WINAPI*)(void*, void**, void**);
OnExperienceToggleButtonVisualStateChanged_t OnExperienceToggleButtonVisualStateChanged_Original;

void WINAPI OnExperienceToggleButtonVisualStateChanged_Hook(void* pThis, void** pSenderRef, void** pArgsRef) {
    OnExperienceToggleButtonVisualStateChanged_Original(pThis, pSenderRef, pArgsRef);
    if (g_unloading) return;
    if (!pSenderRef || !*pSenderRef) return;

    auto elem = GetFrameworkElementFromInterface(*pSenderRef);
    if (!elem) return;
    DispatchElementRefresh(elem);
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE m = GetModuleHandle(L"Taskbar.View.dll");
    return m ? m : GetModuleHandle(L"ExplorerExtensions.dll");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            false
        },
        {
            {
                L"public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnExperienceToggleButtonVisualStateChanged(struct winrt::Taskbar::ITaskbarButton const &,struct winrt::Taskbar::TaskbarButtonVisualStateChangedEventArgs const &)"
            },
            &OnExperienceToggleButtonVisualStateChanged_Original,
            OnExperienceToggleButtonVisualStateChanged_Hook,
            true // optional: missing symbol degrades to sweep-only discovery
        }
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to hook Taskbar.View.dll symbols");
        return false;
    }
    if (!OnExperienceToggleButtonVisualStateChanged_Original) {
        Wh_Log(L"Optional TaskbarResources::OnExperienceToggleButtonVisualStateChanged hook not resolved; system-button discovery is sweep-only");
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module && !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Taskbar View DLL loaded: %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
    }
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Blob Shape Mod");

    LoadSettings();

    HMODULE m = GetTaskbarViewModuleHandle();
    if (m) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(m)) return FALSE;
    } else {
        HMODULE kb = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLibraryExW = kb ? (decltype(&LoadLibraryExW))GetProcAddress(kb, "LoadLibraryExW") : nullptr;
        if (!pLoadLibraryExW ||
            !WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original)) {
            Wh_Log(L"Failed to hook LoadLibraryExW");
            return FALSE;
        }
    }

    // Created only once init can no longer fail: Wh_ModUninit isn't called
    // after a failed init, so an earlier creation would leak the handle on
    // every failed attempt.
    g_postsDrained = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Uninitializing Taskbar Blob Shape Mod (Before)");

    std::vector<std::shared_ptr<BlobEntry>> localEntries;
    std::vector<SweptHost> localHosts;
    {
        // The flag is set under the entries mutex, and FindOrCreateEntry
        // checks it under the same lock: a callback that raced past an
        // earlier g_unloading check can no longer create an entry AFTER this
        // snapshot — such an entry would attach handlers that cleanup()
        // never revokes, firing into an unloaded DLL.
        std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
        g_unloading = true;
        if (g_blobEntries) {
            localEntries = std::move(*g_blobEntries);
        }
        localHosts = std::move(g_trayHosts);
        g_trayHosts.clear();
        for (auto& host : g_taskbarHosts) localHosts.push_back(std::move(host));
        g_taskbarHosts.clear();
        // reset() rather than clear(): the container of thread-affine
        // timers must never survive to the CRT's global destructors.
        g_blobEntries.reset();
    }

    // From here no new refresh can be posted (PostToUiThread checks
    // g_unloading under the same mutex the snapshot just held), so the
    // pending-post count is monotonically non-increasing: reset the drain
    // event now and wait for it at the end. No early return — even with no
    // entries and no hosts, posted refreshes may still be in flight.
    if (g_postsDrained) ResetEvent(g_postsDrained);

    // One barrier covers everything posted from here: entry cleanups AND the
    // host SizeChanged detaches. The count is fixed up front (entries +
    // hosts) and every non-posted path decrements, so the wait below can't
    // complete while any posted lambda — whose code lives in this DLL — is
    // still queued.
    std::shared_ptr<void> eventLifetime(CreateEvent(nullptr, TRUE, FALSE, nullptr), [](HANDLE h) { if(h) CloseHandle(h); });
    auto pending = std::make_shared<std::atomic<int>>((int)localEntries.size() + (int)localHosts.size());

    for (auto& host : localHosts) {
        auto hostGrid = host.grid.get();
        auto dispatcher = hostGrid ? hostGrid.Dispatcher() : nullptr;
        auto token = host.sizeToken;
        if (!dispatcher) {
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
            continue;
        }
        if (dispatcher.HasThreadAccess()) {
            try { hostGrid.SizeChanged(token); } catch (...) {}
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        } else {
            bool posted = false;
            try {
                dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [hostGrid, token, pending, eventLifetime]() {
                    try { hostGrid.SizeChanged(token); } catch (...) {}
                    if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
                });
                posted = true;
            } catch (...) {}
            if (!posted) {
                if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
            }
        }
    }

    for (auto& entry : localEntries) {
        winrt::Windows::UI::Xaml::Shapes::Path blobShape{nullptr};
        winrt::Windows::UI::Xaml::FrameworkElement btn{nullptr};
        {
            // These pre-reads pick the cleanup dispatcher ONLY. The cleanup
            // itself re-resolves everything on the UI thread: a refresh
            // in flight when this snapshot was taken can still create the
            // blob afterwards, and a stale capture would leave that Path
            // parented with a live ActualThemeChanged delegate in this DLL.
            std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
            blobShape = entry->blobShape.get();
            btn = entry->button.get();
        }

        auto cleanup = [entry]() {
            try {
                // Resolved here, on the UI thread, after any in-flight
                // refresh on this dispatcher has completed — never at
                // snapshot time.
                auto blobShape = entry->blobShape.get();
                if (auto anchor = entry->anchor.get()) {
                    try { anchor.SizeChanged(entry->sizeToken); } catch (...) {}
                }
                if (blobShape) {
                    try { blobShape.ActualThemeChanged(entry->themeToken); } catch (...) {}
                    auto vis = ElementCompositionPreview::GetElementVisual(blobShape);
                    vis.Properties().StopAnimation(L"Translation");
                    RemoveFromParentPanel(blobShape);
                }
                // Stop AND release the timer here, on the UI thread: the
                // entry's last shared_ptr may be released on the Windhawk
                // thread, and it must not carry a strong XAML reference
                // across threads when that happens.
                if (entry->restoreTimer) {
                    try { entry->restoreTimer.Stop(); } catch (...) {}
                    entry->restoreTimer = nullptr;
                }
                if (auto btn = entry->button.get()) {
                    if (entry->unloadAttached) {
                        try { btn.Unloaded(entry->unloadToken); } catch (...) {}
                    }
                    if (entry->loadedAttached) {
                        try { btn.Loaded(entry->loadedToken); } catch (...) {}
                    }
                    if (entry->systemEventsAttached) {
                        if (auto toggle = btn.try_as<winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton>()) {
                            try { toggle.Checked(entry->checkedToken); } catch (...) {}
                            try { toggle.Unchecked(entry->uncheckedToken); } catch (...) {}
                        }
                        for (auto& groupToken : entry->stateTokens) {
                            if (auto group = groupToken.first.get()) {
                                try { group.CurrentStateChanged(groupToken.second); } catch (...) {}
                            }
                        }
                    }
                    if (entry->bgHidden || entry->indicatorHidden) RestoreNativeVisuals(btn);
                }
            } catch (...) { Wh_Log(L"Exception during blob shape cleanup"); }
        };

        // Entries can exist WITHOUT a blob: overflow flyout buttons have no
        // Taskbar.TaskbarFrame ancestor, so the RootGrid lookup fails for
        // them permanently — but their Loaded/Unloaded handlers are already
        // attached. Resolve the cleanup dispatcher from the button as a
        // fallback so those handlers are revoked too; left subscribed, they
        // point into this DLL and crash Explorer when they fire after unload.
        auto dispatcher = blobShape ? blobShape.Dispatcher()
                                    : (btn ? btn.Dispatcher() : nullptr);
        if (dispatcher) {
            if (dispatcher.HasThreadAccess()) {
                cleanup();
                if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
            } else {
                bool posted = false;
                try {
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [cleanup, pending, eventLifetime]() {
                        cleanup();
                        if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
                    });
                    posted = true;
                } catch (...) {}
                if (!posted) {
                    if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
                }
            }
        } else {
            // No dispatcher at all (island gone): best-effort inline stop so
            // the entry drops its timer reference before ~BlobEntry runs on
            // this thread.
            if (entry->restoreTimer) {
                try { entry->restoreTimer.Stop(); } catch (...) {}
                entry->restoreTimer = nullptr;
            }
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        }
    }

    if (pending->load() > 0 && eventLifetime.get()) {
        if (WaitForSingleObject(eventLifetime.get(), 2000) == WAIT_TIMEOUT) {
            Wh_Log(L"Timed out waiting for blob shape cleanup");
        }
    }

    // Second barrier: refreshes posted from the hooks and settings paths.
    // Their tickets were taken before g_unloading was set; wait for the
    // in-flight ones to finish executing.
    if (g_pendingPosts.load() > 0) {
        if (g_postsDrained) {
            if (WaitForSingleObject(g_postsDrained, 2000) == WAIT_TIMEOUT) {
                Wh_Log(L"Timed out waiting for posted refreshes to drain");
            }
        } else {
            // Degraded barrier (CreateEvent failed at init): bounded poll —
            // still waits rather than silently skipping.
            for (int i = 0; i < 200 && g_pendingPosts.load() > 0; i++) {
                Sleep(10);
            }
            if (g_pendingPosts.load() > 0) {
                Wh_Log(L"Timed out waiting for posted refreshes to drain");
            }
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Blob Shape Mod");

    if (g_postsDrained) {
        CloseHandle(g_postsDrained);
        g_postsDrained = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    // Kinds enabled just now may have no entries at all (entries aren't
    // created while a kind's toggle is off), so the per-entry refresh below
    // can't reach them — re-sweep the known hosts to materialize them.
    std::vector<winrt::Windows::UI::Xaml::Controls::Grid> resweepGrids;
    {
        std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
        for (auto& host : g_taskbarHosts) {
            if (auto g = host.grid.get()) resweepGrids.push_back(g);
        }
    }
    for (auto& g : resweepGrids) {
        auto dispatcher = g.Dispatcher();
        if (!dispatcher) continue;
        auto weakGrid = winrt::make_weak(g);
        PostToUiThread(dispatcher, [weakGrid]() {
            if (g_unloading) return;
            auto grid = weakGrid.get();
            if (!grid) return;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try { TaskbarSweepBody(grid, localSettings); } catch (...) {}
        });
    }

    std::vector<std::shared_ptr<BlobEntry>> localEntries;
    {
        std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
        if (g_blobEntries) localEntries = *g_blobEntries;
    }
    for (auto& entry : localEntries) {
        winrt::Windows::UI::Xaml::Shapes::Path blobShape{nullptr};
        winrt::Windows::UI::Xaml::FrameworkElement btn{nullptr};
        {
            // Same cross-thread rule as the uninit path.
            std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
            blobShape = entry->blobShape.get();
            btn = entry->button.get();
        }
        auto dispatcher = blobShape ? blobShape.Dispatcher()
                                    : (btn ? btn.Dispatcher() : nullptr);
        if (!dispatcher) continue;
        std::weak_ptr<BlobEntry> weakEntry = entry;
        // Through PostToUiThread, so the unload drain covers this post.
        PostToUiThread(dispatcher, [weakEntry]() {
            if (g_unloading) return;
            auto e = weakEntry.lock();
            if (!e) return;
            auto btn = e->button.get();
            if (!btn) return;
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
            try {
                RefreshBlob(btn, localSettings);
            } catch (...) {}
        });
    }
}
