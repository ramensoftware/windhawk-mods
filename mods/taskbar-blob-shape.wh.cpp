// ==WindhawkMod==
// @id              taskbar-blob-shape
// @name            Taskbar Blob Shape
// @description     Injects a customizable blob shape behind active taskbar items.
// @version         1.0.0
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

![Taskbar Blob Shape](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/main/taskbar-blob-shape/demo.gif)

Based on **Taskbar Elastic WinUI Pill** and the unreleased **Taskbar Elastic
Border** by **Lockframe**.

Every taskbar button gets its own blob shape, shown or hidden as the
button's running indicator state changes. The shapes are hosted in the
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
    $description: Offsets the blob shape like insets - Left/Top push it right/down, Right/Bottom push it left/up (e.g. '0, 4, 0, 0' pushes it down 4px). Vertical offsets are measured from the top of the taskbar. Leave empty to disable.
  - BottomRadius: '4'
    $name: Bottom corner radius
    $description: The radius of the convex bottom corners of the blob shape (e.g., 4.0).
  - TopRadius: '8'
    $name: Top corner radius
    $description: The radius of the concave top flare corners. The blob shape extends upward and sideways by this amount. Set to 0 to disable the flare.
  $name: Blob Shape Settings
- Colors:
  - BgOpacity: '1.0, 1.0'
    $name: Blob opacity (Light, Dark)
    $description: Multiplier for the blob shape's fill opacity (e.g. 0.8, 0.5). Set to 1.0 to keep the color's own alpha.
  - CustomColor: ""
    $name: Custom blob shape color
    $description: Hex color code. Supports multi-color gradients (e.g. '#FF0000, #00FF00') and light|dark separation (e.g. 'light1, light2 | dark1, dark2'). Leave empty to use the system accent color.
  $name: Color Settings
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/base.h>
#include <algorithm>
#include <atomic>
#include <cwctype>
#include <string>
#include <string_view>
#include <mutex>
#include <vector>
#include <optional>
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
    winrt::event_token sizeToken{};
    winrt::event_token unloadToken{};
    winrt::event_token loadedToken{};
    winrt::event_token themeToken{};
    bool unloadAttached = false;
    bool loadedAttached = false;

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
std::vector<std::shared_ptr<BlobEntry>> g_blobEntries;
std::atomic<bool> g_unloading{false};

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
    } else if (vals.size() >= 4) {
        outL = vals[0]; outT = vals[1]; outR = vals[2]; outB = vals[3];
    } else if (vals.size() > 0) {
        outL = outT = outR = outB = vals[0];
    }

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

VisualStateGroup GetVisualStateGroup(FrameworkElement const& root, std::wstring_view groupName) {
    auto groups = VisualStateManager::GetVisualStateGroups(root);
    for (auto const& group : groups) {
        if (group.Name() == groupName) return group;
    }
    return nullptr;
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

// Hands rendering of the native indicator visuals back to XAML.
void RestoreNativeVisuals(winrt::Windows::UI::Xaml::FrameworkElement const& btn) {
    try {
        auto iconPanel = FindChildByName(btn, L"IconPanel");
        if (!iconPanel) return;
        if (auto bg = FindChildByName(iconPanel, L"BackgroundElement")) {
            ElementCompositionPreview::GetElementVisual(bg).IsVisible(true);
        }
        if (auto indicator = FindChildByName(iconPanel, L"RunningIndicator")) {
            ElementCompositionPreview::GetElementVisual(indicator).IsVisible(true);
        }
    } catch (...) {}
}

// Inserts the blob below the task list in z-order so it renders behind the
// buttons, like the native indicator.
void InsertBlobBelowRepeater(Grid const& grid, winrt::Windows::UI::Xaml::Shapes::Path const& blobShape) {
    auto repeater = FindChildByName(grid, L"TaskbarFrameRepeater");
    uint32_t index = 0;
    if (repeater && grid.Children().IndexOf(repeater.try_as<winrt::Windows::UI::Xaml::UIElement>(), index)) {
        grid.Children().InsertAt(index, blobShape);
    } else {
        grid.Children().Append(blobShape);
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

void EnsureBlobOnButton(winrt::Windows::UI::Xaml::FrameworkElement const& button, winrt::Windows::UI::Xaml::FrameworkElement const& iconPanel, bool isActive, const Settings& localSettings);

// The single entry point for every trigger (hook, SizeChanged, Loaded, stale
// Unloaded, theme change, settings change): resolves the button's IconPanel
// once, reads the running indicator state from it, and applies the blob —
// instead of each path doing its own recursive tree walks.
void RefreshBlob(winrt::Windows::UI::Xaml::FrameworkElement const& button, const Settings& localSettings) {
    auto iconPanel = FindChildByName(button, L"IconPanel");
    if (!iconPanel) return;
    bool isActive = false;
    try {
        auto grp = GetVisualStateGroup(iconPanel, L"RunningIndicatorStates");
        auto st = grp ? grp.CurrentState() : nullptr;
        isActive = st && st.Name() == L"ActiveRunningIndicator";
    } catch (...) {}
    EnsureBlobOnButton(button, iconPanel, isActive, localSettings);
}

std::shared_ptr<BlobEntry> FindOrCreateEntry(winrt::Windows::UI::Xaml::FrameworkElement const& button) {
    std::vector<winrt::Windows::UI::Xaml::Shapes::Path> orphans;
    std::shared_ptr<BlobEntry> result;
    {
        std::lock_guard<std::mutex> lock(g_blobEntriesMutex);

        // Prune entries whose button died without an Unloaded (rare). Their
        // blob elements live in the RootGrid, so they must be removed
        // explicitly. This runs on every lookup — the list is small since
        // Unloaded drops entries eagerly.
        for (auto it = g_blobEntries.begin(); it != g_blobEntries.end(); ) {
            auto btn = (*it)->button.get();
            if (!btn) {
                if (auto blob = (*it)->blobShape.get()) orphans.push_back(blob);
                it = g_blobEntries.erase(it);
                continue;
            }
            if (btn == button) result = *it;
            ++it;
        }
        if (!result) {
            result = std::make_shared<BlobEntry>();
            result->button = winrt::make_weak(button);
            g_blobEntries.push_back(result);
        }
    }

    for (auto& blob : orphans) {
        if (auto dispatcher = blob.Dispatcher()) {
            dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [blob]() {
                try {
                    ElementCompositionPreview::GetElementVisual(blob).Properties().StopAnimation(L"Translation");
                } catch (...) {}
                RemoveFromParentPanel(blob);
            });
        }
    }
    return result;
}

// Locates the RootGrid of the taskbar hosting this button by walking up to
// Taskbar.TaskbarFrame. Returns nullptr while the button isn't rooted yet —
// the next state change or SizeChanged retries.
Grid GetTaskbarRootGrid(winrt::Windows::UI::Xaml::FrameworkElement const& button) {
    FrameworkElement current = button;
    int depth = 0;
    while (current && depth < 20) {
        if (winrt::get_class_name(current) == L"Taskbar.TaskbarFrame") {
            auto rootGrid = FindChildByName(current, L"RootGrid");
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
void EnsureBlobOnButton(winrt::Windows::UI::Xaml::FrameworkElement const& button, winrt::Windows::UI::Xaml::FrameworkElement const& iconPanel, bool isActive, const Settings& localSettings) {
    auto bg = FindChildByName(iconPanel, L"BackgroundElement");
    FrameworkElement anchor = bg ? bg : iconPanel;

    // Suppression of the native background is decided at the END, from the
    // same condition that shows the blob, so no failure path can leave an
    // active button with neither indicator. The hand-off visual's IsVisible
    // flag is used instead of a local Opacity because the
    // RunningIndicatorStates transition storyboards hold ANIMATED values on
    // BackgroundElement, which outrank local values in XAML's precedence.
    // The RunningIndicator line is part of the button and renders above the
    // blob (which sits below the repeater in z-order), so it is suppressed
    // together with the background while the blob is shown.
    auto runningIndicator = FindChildByName(iconPanel, L"RunningIndicator");
    auto setNativeHidden = [&](bool hidden) {
        if (bg) {
            try {
                ElementCompositionPreview::GetElementVisual(bg).IsVisible(!hidden);
            } catch (...) {}
        }
        if (runningIndicator) {
            try {
                ElementCompositionPreview::GetElementVisual(runningIndicator).IsVisible(!hidden);
            } catch (...) {}
        }
    };

    auto entry = FindOrCreateEntry(button);

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
            if (btn) {
                RestoreNativeVisuals(btn);
                if (auto anchor = e->anchor.get()) {
                    try { anchor.SizeChanged(e->sizeToken); } catch (...) {}
                }
                try { btn.Unloaded(e->unloadToken); } catch (...) {}
                if (e->loadedAttached) {
                    try { btn.Loaded(e->loadedToken); } catch (...) {}
                }
            }
            std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
            g_blobEntries.erase(
                std::remove_if(g_blobEntries.begin(), g_blobEntries.end(),
                    [&](auto& x) { return x == e; }),
                g_blobEntries.end());
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

    auto grid = entry->grid.get();
    if (!grid) {
        grid = GetTaskbarRootGrid(button);
        if (!grid) {
            // Not rooted under a TaskbarFrame — e.g. buttons in the overflow
            // flyout live in a separate XAML island. Leave the native
            // indicator fully in charge there.
            setNativeHidden(false);
            return;
        }
        entry->grid = winrt::make_weak(grid);
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

        InsertBlobBelowRepeater(grid, blobShape);
        ElementCompositionPreview::SetIsTranslationEnabled(blobShape, true);

        entry->blobShape = winrt::make_weak(blobShape);
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
            InsertBlobBelowRepeater(grid, blobShape);
            entry->bound = false;
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
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_unloading) return;

    auto elem = GetFrameworkElementFromNative(pThis);
    if (!elem) return;

    auto dispatcher = elem.Dispatcher();
    if (!dispatcher) return;
    auto weakElem = winrt::make_weak(elem);

    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [weakElem]() {
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
        }
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to hook Taskbar.View.dll symbols");
        return false;
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

    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Uninitializing Taskbar Blob Shape Mod (Before)");
    g_unloading = true;

    std::vector<std::shared_ptr<BlobEntry>> localEntries;
    {
        std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
        localEntries = g_blobEntries;
        g_blobEntries.clear();
    }
    if (localEntries.empty()) return;

    std::shared_ptr<void> eventLifetime(CreateEvent(nullptr, TRUE, FALSE, nullptr), [](HANDLE h) { if(h) CloseHandle(h); });
    auto pending = std::make_shared<std::atomic<int>>((int)localEntries.size());

    for (auto& entry : localEntries) {
        auto blobShape = entry->blobShape.get();
        auto btn = entry->button.get();

        auto cleanup = [entry, blobShape]() {
            try {
                if (auto anchor = entry->anchor.get()) {
                    try { anchor.SizeChanged(entry->sizeToken); } catch (...) {}
                }
                if (blobShape) {
                    try { blobShape.ActualThemeChanged(entry->themeToken); } catch (...) {}
                    auto vis = ElementCompositionPreview::GetElementVisual(blobShape);
                    vis.Properties().StopAnimation(L"Translation");
                    RemoveFromParentPanel(blobShape);
                }
                if (auto btn = entry->button.get()) {
                    if (entry->unloadAttached) {
                        try { btn.Unloaded(entry->unloadToken); } catch (...) {}
                    }
                    if (entry->loadedAttached) {
                        try { btn.Loaded(entry->loadedToken); } catch (...) {}
                    }
                    RestoreNativeVisuals(btn);
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
                dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [cleanup, pending, eventLifetime]() {
                    cleanup();
                    if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
                });
            }
        } else {
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        }
    }

    if (pending->load() > 0 && eventLifetime.get()) {
        if (WaitForSingleObject(eventLifetime.get(), 2000) == WAIT_TIMEOUT) {
            Wh_Log(L"Timed out waiting for blob shape cleanup");
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Blob Shape Mod");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    std::vector<std::shared_ptr<BlobEntry>> localEntries;
    {
        std::lock_guard<std::mutex> lock(g_blobEntriesMutex);
        localEntries = g_blobEntries;
    }
    for (auto& entry : localEntries) {
        auto blobShape = entry->blobShape.get();
        auto btn = entry->button.get();
        auto dispatcher = blobShape ? blobShape.Dispatcher()
                                    : (btn ? btn.Dispatcher() : nullptr);
        if (!dispatcher) continue;
        std::weak_ptr<BlobEntry> weakEntry = entry;
        // High, matching the hook and the uninit cleanup: CoreDispatcher is
        // priority-ordered, so a Low item posted here could still be queued
        // when the High-priority uninit barrier completes — and would then
        // run in an unloaded DLL.
        dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [weakEntry]() {
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
