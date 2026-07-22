// ==WindhawkMod==
// @id              taskbar-elastic-pill
// @name            Taskbar Elastic WinUI Pill
// @description     Injects an animated sliding pill for active taskbar items.
// @version         2.0.0
// @author          Lockframe
// @github          https://github.com/Lockframe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Elastic WinUI Pill

Replaces the default active app pill in Windows 11's taskbar with an animated and customizable one.

![](https://i.imgur.com/PkPhNiH.gif)

---

## Contributions
Stretch animation by [Dan](https://github.com/crazyboyybs)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Pill:
  - Dimensions: "16, 3"
    $name: Pill Dimensions (Width, Height)
    $description: The width and height of the sliding pill in pixels.
  - Margins: "0, 5"
    $name: Pill Margins (Horizontal, Bottom)
    $description: Left margin (Right is mirrored) and Bottom margin.
  - PillRadius: '1.5'
    $name: Pill corner radius
    $description: The corner radius for the pill (e.g., 1.5).
  - HideInactiveDots: false
    $name: Hide inactive app dots
    $description: Hide the native small dot indicator for inactive apps.
  - TrackSystemButtons: false
    $name: Track System buttons
    $description: Add elastic pill to Start, Task View, and Search buttons.
  $name: Pill Settings
- Animation:
  - AnimationStyle: stretch
    $name: Animation style
    $options:
    - stretch: Stretch (WinUI accurate)
    - stretch-bounce: Elastic
    - bounce: Bounce
    - linear: Linear
    - easein: Ease-In
    - easeout: Ease-Out
    - easeinout: Ease-In-Out
    - none: None
  - UseSpringPhysics: false
    $name: Use spring physics
    $description: Switch stretch animation from simple Bezier curves to Spring physics.
  - ElasticIntensity: '1.0'
    $name: Elastic intensity
    $description: Controls the bounciness of spring animations (e.g. 2.0 makes it twice as bouncy). Only applies to spring physics modes.
  - SpeedMultiplier: '1.0'
    $name: Speed multiplier
    $description: Multiplier for animation speed (e.g., 2.0 is twice as fast).
  - FadeTransition: false
    $name: Fade transition
    $description: Fade out and in during movement.
  - SquishTransition: false
    $name: Squish transition
    $description: Adds a squish effect to the pill while moving.
  - SquishMultiplier: '1.0'
    $name: Squish multiplier (X, Y)
    $description: Controls squish intensity. Use one value for uniform scaling, or two (e.g. '2.0, 0.5') for independent X/Y control. Negative values invert the squish.
  - FadeDurationMultiplier: '1.0'
    $name: Fade duration multiplier
    $description: Controls the duration of the fade transition (e.g., 2.0 makes the fade take twice as long).
  $name: Animation Settings
- PointerInteractions:
  - HoverScale: '1.0'
    $name: Hover scale multiplier
    $description: Multiplier for how much the pill scales when hovered. Use 1 value for uniform scale (e.g. 1.1) or 2 values for X,Y scale (e.g. 1.05, 0.7). Set to 1.0 to disable.
  - PressedScale: '1.0'
    $name: Pressed scale multiplier
    $description: Multiplier for how much the pill scales when clicked. Use 1 value for uniform scale (e.g. 0.85) or 2 values for X,Y scale (e.g. 0.85, 0.5). Set to 1.0 to disable.
  - HoverBgColor: ''
    $name: Hover background color (Light, Dark)
    $description: Optional hex color overrides during hover. Supports multi-color gradients (e.g. 'hex1, hex2') and light|dark separation (e.g. 'light1, light2 | dark1, dark2').
  - PressedBgColor: ''
    $name: Pressed background color (Light, Dark)
    $description: Optional hex color overrides during click. Supports multi-color gradients and light|dark separation.
  $name: Pointer Interactions
- Colors:
  - BgOpacity: '1.0, 1.0'
    $name: Pill opacity (Light, Dark)
    $description: Multiplier for the pill fill opacity (e.g. 0.8, 0.5). Set to 1.0 to keep original alpha.
  - ColorMode: accent
    $name: Pill color mode
    $options:
    - accent: System accent
    - custom: Custom hex
    - icon: App icon color
  - CustomColor: ""
    $name: Custom pill color
    $description: Hex color code. Supports multi-color gradients (e.g. 'hex1, hex2') and light|dark separation (e.g. 'light1, light2 | dark1, dark2').
  - BgGradientDirection: horizontal
    $name: Pill gradient direction
    $options:
    - horizontal: Horizontal
    - vertical: Vertical
    - diagonal1: Diagonal (Top-Left to Bottom-Right)
    - diagonal2: Diagonal (Bottom-Left to Top-Right)
  $name: Color Settings
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/base.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <mutex>
#include <vector>
#include <optional>
#include <cmath>
#include <limits>
#include <map>
#include <memory>

struct Settings {
    int PillMarginBottom = 0;
    int PillMarginHorizontal = 0;
    int PillWidth = 16;
    int PillHeight = 3;
    double PillRadius = 1.5;
    int AnimationStyle = 0;
    bool UseSpringPhysics = false;
    double ElasticIntensity = 1.0;
    double SpeedMultiplier = 1.0;
    bool FadeTransition = false;
    bool SquishTransition = false;
    double SquishMultiplierX = 1.0;
    double SquishMultiplierY = 1.0;
    double FadeDurationMultiplier = 1.0;
    bool HideInactiveDots = false;
    int ColorMode = 0;
    bool TrackSystemButtons = false;
    
    double HoverScaleX = 1.0, HoverScaleY = 1.0;
    double PressedScaleX = 1.0, PressedScaleY = 1.0;
    std::vector<winrt::Windows::UI::Color> HoverBgColorLight;
    std::vector<winrt::Windows::UI::Color> HoverBgColorDark;
    std::vector<winrt::Windows::UI::Color> PressedBgColorLight;
    std::vector<winrt::Windows::UI::Color> PressedBgColorDark;
    
    double BgOpacityLight = 1.0;
    double BgOpacityDark = 1.0;
    int BgGradientDirection = 0;

    std::vector<winrt::Windows::UI::Color> ParsedLightColor;
    std::vector<winrt::Windows::UI::Color> ParsedDarkColor;
    std::vector<winrt::Windows::UI::Color> ParsedSolidColor;
} g_settings;

std::mutex g_settingsMutex;
std::mutex g_pillsMutex;
struct PillContext {
    winrt::weak_ref<winrt::Windows::UI::Xaml::Shapes::Rectangle> pill;
    winrt::weak_ref<winrt::Windows::UI::Xaml::Controls::Grid> grid;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> activeBtn;
    winrt::event_token layoutToken{};
    std::atomic<bool> searchAttached{false};
    winrt::Windows::UI::Xaml::Media::Animation::Storyboard colorAnimBoard{nullptr};
    std::chrono::steady_clock::time_point inactiveStartTime{};
    bool forceSnapNext{false};
    winrt::Windows::UI::Color currentTargetColor{0,0,0,0};
    bool isHover{false};
    bool isPress{false};
};
std::vector<std::shared_ptr<PillContext>>* g_pillContexts = new std::vector<std::shared_ptr<PillContext>>();
std::atomic<bool> g_unloading{false};

std::atomic<bool> g_taskbarViewDllLoaded{false};

std::atomic<bool> g_searchUxDllLoaded{false};

struct EasingCache {
    winrt::Windows::UI::Composition::Compositor compositor{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction stretchLeadEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction stretchTrailEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction squishEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction linearEase{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction easeIn{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction easeOut{nullptr};
    winrt::Windows::UI::Composition::CompositionEasingFunction easeInOut{nullptr};
};

std::map<DWORD, EasingCache>* g_easingCaches = new std::map<DWORD, EasingCache>();
std::mutex g_easingMutex;

std::optional<winrt::Windows::UI::Color> ParseHexColor(std::wstring_view hexView) {
    if (hexView.empty()) return std::nullopt;
    std::wstring hex(hexView);
    hex.erase(0, hex.find_first_not_of(L" \t\r\n"));
    if (hex.empty()) return std::nullopt;
    hex.erase(hex.find_last_not_of(L" \t\r\n") + 1);
    if (hex[0] == L'#') hex.erase(0, 1);
    if (hex.length() == 6) hex = L"FF" + hex;
    if (hex.length() != 8) return std::nullopt;
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

void ParseIntTuple(PCWSTR str, int& out1, int& out2) {
    if (!str) return;
    std::wstring ws(str);
    size_t comma = ws.find(L',');
    if (comma != std::wstring::npos) {
        try {
            out1 = std::stoi(ws.substr(0, comma));
            out2 = std::stoi(ws.substr(comma + 1));
        } catch (...) {}
    }
}

void LoadSettings() {
    std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
    
    g_settings.PillWidth = 16; g_settings.PillHeight = 3;
    WindhawkUtils::StringSetting dimStr(Wh_GetStringSetting(L"Pill.Dimensions"));
    ParseIntTuple(dimStr.get(), g_settings.PillWidth, g_settings.PillHeight);

    g_settings.PillMarginHorizontal = 0; g_settings.PillMarginBottom = 0;
    WindhawkUtils::StringSetting marStr(Wh_GetStringSetting(L"Pill.Margins"));
    ParseIntTuple(marStr.get(), g_settings.PillMarginHorizontal, g_settings.PillMarginBottom);
    
    g_settings.PillRadius = 1.5;
    WindhawkUtils::StringSetting radiusStr(Wh_GetStringSetting(L"Pill.PillRadius"));
    if (radiusStr.get()[0]) {
        try {
            g_settings.PillRadius = std::stod(radiusStr.get());
            if (g_settings.PillRadius < 0.0) g_settings.PillRadius = 0.0;
        } catch (...) { g_settings.PillRadius = 1.5; }
    }

    WindhawkUtils::StringSetting animStr(Wh_GetStringSetting(L"Animation.AnimationStyle"));
    g_settings.AnimationStyle = 0;
    if (animStr.get()[0]) {
        if (wcscmp(animStr.get(), L"bounce") == 0) g_settings.AnimationStyle = 1;
        else if (wcscmp(animStr.get(), L"stretch-bounce") == 0) g_settings.AnimationStyle = 7;
        else if (wcscmp(animStr.get(), L"linear") == 0) g_settings.AnimationStyle = 2;
        else if (wcscmp(animStr.get(), L"easein") == 0) g_settings.AnimationStyle = 4;
        else if (wcscmp(animStr.get(), L"easeout") == 0) g_settings.AnimationStyle = 5;
        else if (wcscmp(animStr.get(), L"easeinout") == 0) g_settings.AnimationStyle = 6;
        else if (wcscmp(animStr.get(), L"none") == 0) g_settings.AnimationStyle = 8;
    }
    g_settings.UseSpringPhysics = Wh_GetIntSetting(L"Animation.UseSpringPhysics") != 0;

    WindhawkUtils::StringSetting elasticMStr(Wh_GetStringSetting(L"Animation.ElasticIntensity"));
    g_settings.ElasticIntensity = 1.0;
    if (elasticMStr.get()[0]) {
        try {
            g_settings.ElasticIntensity = std::stod(elasticMStr.get());
            if (std::isnan(g_settings.ElasticIntensity) || g_settings.ElasticIntensity <= 0.0) g_settings.ElasticIntensity = 1.0;
        } catch (...) {}
    }

    WindhawkUtils::StringSetting speedStr(Wh_GetStringSetting(L"Animation.SpeedMultiplier"));
    g_settings.SpeedMultiplier = 1.0;
    if (speedStr.get()[0]) {
        try {
            g_settings.SpeedMultiplier = std::stod(speedStr.get());
            if (std::isnan(g_settings.SpeedMultiplier) || g_settings.SpeedMultiplier <= 0.0) g_settings.SpeedMultiplier = 1.0;
            g_settings.SpeedMultiplier = std::clamp(g_settings.SpeedMultiplier, 0.1, 10.0);
        } catch (...) {}
    }
    g_settings.FadeTransition = Wh_GetIntSetting(L"Animation.FadeTransition") != 0;
    g_settings.SquishTransition = Wh_GetIntSetting(L"Animation.SquishTransition") != 0;

    WindhawkUtils::StringSetting squishMStr(Wh_GetStringSetting(L"Animation.SquishMultiplier"));
    g_settings.SquishMultiplierX = 1.0;
    g_settings.SquishMultiplierY = 1.0;
    if (squishMStr.get()[0]) {
        std::wstring ws(squishMStr.get());
        size_t delim = ws.find(L',');
        if (delim == std::wstring::npos) delim = ws.find(L' ');
        if (delim != std::wstring::npos) {
            try { g_settings.SquishMultiplierX = std::stod(ws.substr(0, delim)); } catch (...) {}
            try { g_settings.SquishMultiplierY = std::stod(ws.substr(delim + 1)); } catch (...) {}
            if (std::isnan(g_settings.SquishMultiplierX)) g_settings.SquishMultiplierX = 1.0;
            if (std::isnan(g_settings.SquishMultiplierY)) g_settings.SquishMultiplierY = 1.0;
        } else {
            try {
                g_settings.SquishMultiplierX = std::stod(ws);
                if (std::isnan(g_settings.SquishMultiplierX)) g_settings.SquishMultiplierX = 1.0;
                g_settings.SquishMultiplierY = g_settings.SquishMultiplierX;
            } catch (...) {}
        }
    }

    WindhawkUtils::StringSetting fadeDurStr(Wh_GetStringSetting(L"Animation.FadeDurationMultiplier"));
    g_settings.FadeDurationMultiplier = 1.0;
    if (fadeDurStr.get()[0]) {
        try {
            g_settings.FadeDurationMultiplier = std::stod(fadeDurStr.get());
            if (std::isnan(g_settings.FadeDurationMultiplier) || g_settings.FadeDurationMultiplier <= 0.0) g_settings.FadeDurationMultiplier = 1.0;
        } catch (...) {}
    }

    WindhawkUtils::StringSetting hovScaleStr(Wh_GetStringSetting(L"PointerInteractions.HoverScale"));
    g_settings.HoverScaleX = 1.0;
    g_settings.HoverScaleY = 1.0;
    if (hovScaleStr.get()[0]) {
        std::wstring ws(hovScaleStr.get());
        size_t delim = ws.find(L',');
        if (delim == std::wstring::npos) delim = ws.find(L' ');
        if (delim != std::wstring::npos) {
            try { g_settings.HoverScaleX = std::stod(ws.substr(0, delim)); } catch (...) {}
            try { g_settings.HoverScaleY = std::stod(ws.substr(delim + 1)); } catch (...) {}
            if (std::isnan(g_settings.HoverScaleX)) g_settings.HoverScaleX = 1.0;
            if (std::isnan(g_settings.HoverScaleY)) g_settings.HoverScaleY = 1.0;
        } else {
            try {
                g_settings.HoverScaleX = std::stod(ws);
                if (std::isnan(g_settings.HoverScaleX)) g_settings.HoverScaleX = 1.0;
                g_settings.HoverScaleY = g_settings.HoverScaleX;
            } catch (...) {}
        }
    }

    WindhawkUtils::StringSetting pressScaleStr(Wh_GetStringSetting(L"PointerInteractions.PressedScale"));
    g_settings.PressedScaleX = 1.0;
    g_settings.PressedScaleY = 1.0;
    if (pressScaleStr.get()[0]) {
        std::wstring ws(pressScaleStr.get());
        size_t delim = ws.find(L',');
        if (delim == std::wstring::npos) delim = ws.find(L' ');
        if (delim != std::wstring::npos) {
            try { g_settings.PressedScaleX = std::stod(ws.substr(0, delim)); } catch (...) {}
            try { g_settings.PressedScaleY = std::stod(ws.substr(delim + 1)); } catch (...) {}
            if (std::isnan(g_settings.PressedScaleX)) g_settings.PressedScaleX = 1.0;
            if (std::isnan(g_settings.PressedScaleY)) g_settings.PressedScaleY = 1.0;
        } else {
            try {
                g_settings.PressedScaleX = std::stod(ws);
                if (std::isnan(g_settings.PressedScaleX)) g_settings.PressedScaleX = 1.0;
                g_settings.PressedScaleY = g_settings.PressedScaleX;
            } catch (...) {}
        }
    }

    WindhawkUtils::StringSetting hpS(Wh_GetStringSetting(L"PointerInteractions.HoverBgColor"));
    ParseGradientColorPair(hpS.get(), g_settings.HoverBgColorLight, g_settings.HoverBgColorDark);
    WindhawkUtils::StringSetting ppS(Wh_GetStringSetting(L"PointerInteractions.PressedBgColor"));
    ParseGradientColorPair(ppS.get(), g_settings.PressedBgColorLight, g_settings.PressedBgColorDark);

    g_settings.HideInactiveDots = Wh_GetIntSetting(L"Pill.HideInactiveDots") != 0;
    g_settings.TrackSystemButtons = Wh_GetIntSetting(L"Pill.TrackSystemButtons") != 0;

    WindhawkUtils::StringSetting cmStr(Wh_GetStringSetting(L"Colors.ColorMode"));
    g_settings.ColorMode = 0;
    if (cmStr.get()[0]) {
        if (wcscmp(cmStr.get(), L"custom") == 0) g_settings.ColorMode = 1;
        else if (wcscmp(cmStr.get(), L"icon") == 0) g_settings.ColorMode = 2;
    }

    WindhawkUtils::StringSetting bgOpStr(Wh_GetStringSetting(L"Colors.BgOpacity"));
    ParseDoublePair(bgOpStr.get(), g_settings.BgOpacityLight, g_settings.BgOpacityDark, 1.0);

    WindhawkUtils::StringSetting bgDirStr(Wh_GetStringSetting(L"Colors.BgGradientDirection"));
    g_settings.BgGradientDirection = 0;
    if (bgDirStr.get()[0]) {
        if (wcscmp(bgDirStr.get(), L"vertical") == 0) g_settings.BgGradientDirection = 1;
        else if (wcscmp(bgDirStr.get(), L"diagonal1") == 0) g_settings.BgGradientDirection = 2;
        else if (wcscmp(bgDirStr.get(), L"diagonal2") == 0) g_settings.BgGradientDirection = 3;
    }

    WindhawkUtils::StringSetting customCStr(Wh_GetStringSetting(L"Colors.CustomColor"));
    ParseGradientColorPair(customCStr.get(), g_settings.ParsedLightColor, g_settings.ParsedDarkColor);
}

struct ExtractedColors {
    winrt::Windows::UI::Color lightMode;
    winrt::Windows::UI::Color darkMode;
};

std::mutex g_iconColorMutex;
std::map<std::wstring, ExtractedColors>* g_iconColorCache = new std::map<std::wstring, ExtractedColors>();
std::map<std::wstring, bool>* g_iconColorExtracting = new std::map<std::wstring, bool>();


std::vector<winrt::weak_ref<winrt::Windows::UI::Xaml::VisualStateGroup>>* g_attachedGroups = new std::vector<winrt::weak_ref<winrt::Windows::UI::Xaml::VisualStateGroup>>();
std::mutex g_attachedGroupsMutex;

struct AttachedEvent {
    winrt::weak_ref<winrt::Windows::UI::Xaml::VisualStateGroup> group;
    winrt::event_token token;
    winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
};
std::vector<AttachedEvent>* g_attachedEvents = new std::vector<AttachedEvent>();
std::mutex g_attachedEventsMutex;

struct HSL {
    double h; // 0-360
    double s; // 0-1
    double l; // 0-1
};

HSL RgbToHsl(uint8_t r, uint8_t g, uint8_t b) {
    double rd = r / 255.0;
    double gd = g / 255.0;
    double bd = b / 255.0;
    double cmax = std::max({rd, gd, bd});
    double cmin = std::min({rd, gd, bd});
    double delta = cmax - cmin;

    HSL hsl = {0, 0, (cmax + cmin) / 2.0};

    if (delta != 0) {
        hsl.s = hsl.l < 0.5 ? delta / (cmax + cmin) : delta / (2.0 - cmax - cmin);

        if (cmax == rd) {
            hsl.h = (gd - bd) / delta + (gd < bd ? 6 : 0);
        } else if (cmax == gd) {
            hsl.h = (bd - rd) / delta + 2;
        } else {
            hsl.h = (rd - gd) / delta + 4;
        }
        hsl.h *= 60.0;
    }
    return hsl;
}

double HueToRgb(double p, double q, double t) {
    if (t < 0) t += 1.0;
    if (t > 1) t -= 1.0;
    if (t < 1.0/6.0) return p + (q - p) * 6.0 * t;
    if (t < 1.0/2.0) return q;
    if (t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
    return p;
}

winrt::Windows::UI::Color HslToRgb(HSL hsl) {
    double r, g, b;

    if (hsl.s == 0) {
        r = g = b = hsl.l;
    } else {
        double q = hsl.l < 0.5 ? hsl.l * (1.0 + hsl.s) : hsl.l + hsl.s - hsl.l * hsl.s;
        double p = 2.0 * hsl.l - q;
        double hk = hsl.h / 360.0;
        
        r = HueToRgb(p, q, hk + 1.0/3.0);
        g = HueToRgb(p, q, hk);
        b = HueToRgb(p, q, hk - 1.0/3.0);
    }

    return {255, (uint8_t)std::round(r * 255), (uint8_t)std::round(g * 255), (uint8_t)std::round(b * 255)};
}

winrt::Windows::UI::Color AdaptColorForTheme(winrt::Windows::UI::Color baseColor, bool isLightTheme) {
    HSL hsl = RgbToHsl(baseColor.R, baseColor.G, baseColor.B);
    
    if (isLightTheme) {
        if (hsl.l > 0.45) hsl.l = 0.45;
        hsl.s = std::min(1.0, hsl.s + 0.1); 
    } else {
        if (hsl.l < 0.60) hsl.l = 0.60;
        if (hsl.s > 0.8) hsl.s = std::max(0.0, hsl.s - 0.1);
    }
    
    return HslToRgb(hsl);
}

constexpr GUID IID_IBufferByteAccess_Local = {0x905a0fef, 0xbc53, 0x11df, {0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda}};
struct IBufferByteAccess_Local : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Buffer(uint8_t** value) = 0;
};

inline winrt::Windows::UI::Color ApplyOpacity(winrt::Windows::UI::Color c, double opacity) {
    if (opacity < 0.0) opacity = 0.0;
    if (opacity > 1.0) opacity = 1.0;
    c.A = static_cast<uint8_t>(c.A * opacity);
    return c;
}

std::vector<winrt::Windows::UI::Color> GetPillColors(const Settings& localSettings, const std::wstring& buttonAbi) {
    bool isLight = (winrt::Windows::UI::Xaml::Application::Current().RequestedTheme() == winrt::Windows::UI::Xaml::ApplicationTheme::Light);
    double opacity = isLight ? localSettings.BgOpacityLight : localSettings.BgOpacityDark;

    if (localSettings.ColorMode == 2 && !buttonAbi.empty()) {
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        auto it = g_iconColorCache->find(buttonAbi);
        if (it != g_iconColorCache->end()) {
            return {ApplyOpacity(isLight ? it->second.lightMode : it->second.darkMode, opacity)};
        }
    }
    
    if (localSettings.ColorMode == 1 && (!localSettings.ParsedLightColor.empty() || !localSettings.ParsedDarkColor.empty() || !localSettings.ParsedSolidColor.empty())) {
        if (!localSettings.ParsedLightColor.empty() || !localSettings.ParsedDarkColor.empty()) {
            auto c = isLight ? localSettings.ParsedLightColor : localSettings.ParsedDarkColor;
            if (!c.empty()) {
                for (auto& col : c) col = ApplyOpacity(col, opacity);
                return c;
            }
        } else if (!localSettings.ParsedSolidColor.empty()) {
            auto c = localSettings.ParsedSolidColor;
            for (auto& col : c) col = ApplyOpacity(col, opacity);
            return c;
        }
    }
    
    auto res = winrt::Windows::UI::Xaml::Application::Current().Resources();
    auto resName = isLight ? L"SystemAccentColorDark1" : L"SystemAccentColorLight2";
    if (res.HasKey(winrt::box_value(resName))) {
        return {ApplyOpacity(winrt::unbox_value<winrt::Windows::UI::Color>(res.Lookup(winrt::box_value(resName))), opacity)};
    }
    return {ApplyOpacity({255, 0, 120, 212}, opacity)}; 
}

winrt::Windows::UI::Xaml::Media::Brush CreateBrush(const std::vector<winrt::Windows::UI::Color>& colors, int direction) {
    if (colors.empty()) return nullptr;
    if (colors.size() == 1) {
        return winrt::Windows::UI::Xaml::Media::SolidColorBrush(colors[0]);
    }
    
    winrt::Windows::UI::Xaml::Media::LinearGradientBrush brush;
    if (direction == 0) { // Horizontal
        brush.StartPoint({0.0, 0.5}); brush.EndPoint({1.0, 0.5});
    } else if (direction == 1) { // Vertical
        brush.StartPoint({0.5, 0.0}); brush.EndPoint({0.5, 1.0});
    } else if (direction == 2) { // Diagonal TL-BR
        brush.StartPoint({0.0, 0.0}); brush.EndPoint({1.0, 1.0});
    } else if (direction == 3) { // Diagonal BL-TR
        brush.StartPoint({0.0, 1.0}); brush.EndPoint({1.0, 0.0});
    }
    
    auto stops = brush.GradientStops();
    for (size_t i = 0; i < colors.size(); i++) {
        winrt::Windows::UI::Xaml::Media::GradientStop stop;
        stop.Color(colors[i]);
        stop.Offset(colors.size() > 1 ? static_cast<double>(i) / (colors.size() - 1) : 0.0);
        stops.Append(stop);
    }
    return brush;
}

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

static void UpdatePillColor(
    winrt::Windows::UI::Xaml::FrameworkElement const& button,
    winrt::Windows::UI::Xaml::Shapes::Rectangle const& pill,
    std::shared_ptr<PillContext> currentCtx,
    Settings const& settingsToUse,
    bool anyActive,
    bool isPointerOver,
    bool isPressed,
    std::wstring const& stableKey,
    bool isExtracting)
{
    bool hasColor = true;
    if (anyActive && settingsToUse.ColorMode == 2) {
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        hasColor = g_iconColorCache->count(stableKey) > 0;
    }
    bool canUpdate = (!anyActive || settingsToUse.ColorMode != 2 || !isExtracting || hasColor);
    
    auto existingSolidBrush = pill.Fill().try_as<winrt::Windows::UI::Xaml::Media::SolidColorBrush>();
    
    if (!existingSolidBrush || canUpdate) {
        std::vector<winrt::Windows::UI::Color> newColors = anyActive ? GetPillColors(settingsToUse, stableKey) : GetPillColors(settingsToUse, L"");
        bool isLight = IsElementLightMode(button);
        if (isPressed) {
            auto overrideCols = isLight ? settingsToUse.PressedBgColorLight : settingsToUse.PressedBgColorDark;
            if (!overrideCols.empty()) newColors = overrideCols;
        } else if (isPointerOver) {
            auto overrideCols = isLight ? settingsToUse.HoverBgColorLight : settingsToUse.HoverBgColorDark;
            if (!overrideCols.empty()) newColors = overrideCols;
        }

        bool isSnap = false;
        if (!existingSolidBrush || newColors.size() != 1) {
            isSnap = true;
        }

        if (isSnap) {
            if (currentCtx && currentCtx->colorAnimBoard) {
                currentCtx->colorAnimBoard.Pause();
                currentCtx->colorAnimBoard = nullptr;
            }
            pill.Fill(CreateBrush(newColors, settingsToUse.BgGradientDirection));
            if (currentCtx && newColors.size() == 1) {
                currentCtx->currentTargetColor = newColors[0];
            }
        } else {
            winrt::Windows::UI::Color newColor = newColors[0];
            bool shouldAnimate = false;
            if (currentCtx) {
                if (currentCtx->currentTargetColor.A != newColor.A || currentCtx->currentTargetColor.R != newColor.R || currentCtx->currentTargetColor.G != newColor.G || currentCtx->currentTargetColor.B != newColor.B) {
                    shouldAnimate = true;
                    currentCtx->currentTargetColor = newColor;
                }
            } else {
                winrt::Windows::UI::Color oldColor = existingSolidBrush.Color();
                if (oldColor.A != newColor.A || oldColor.R != newColor.R || oldColor.G != newColor.G || oldColor.B != newColor.B) {
                    shouldAnimate = true;
                }
            }

            if (shouldAnimate) {
                winrt::Windows::UI::Xaml::Media::Animation::ColorAnimation colorAnim;
                colorAnim.To(newColor);
                winrt::Windows::UI::Xaml::Duration dur;
                dur.TimeSpan = winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(static_cast<long long>((150.0 * settingsToUse.FadeDurationMultiplier) / settingsToUse.SpeedMultiplier)));
                dur.Type = winrt::Windows::UI::Xaml::DurationType::TimeSpan;
                colorAnim.Duration(dur);

                if (currentCtx && currentCtx->colorAnimBoard) {
                    currentCtx->colorAnimBoard.Pause();
                    currentCtx->colorAnimBoard = nullptr;
                }

                auto storyboard = winrt::Windows::UI::Xaml::Media::Animation::Storyboard();
                storyboard.Children().Append(colorAnim);
                winrt::Windows::UI::Xaml::Media::Animation::Storyboard::SetTarget(colorAnim, existingSolidBrush);
                winrt::Windows::UI::Xaml::Media::Animation::Storyboard::SetTargetProperty(colorAnim, L"Color");
                
                if (currentCtx) {
                    currentCtx->colorAnimBoard = storyboard;
                    auto weakCtx = std::weak_ptr<PillContext>(currentCtx);
                    storyboard.Completed([weakCtx](auto const&, auto const&) {
                        if (auto context = weakCtx.lock()) {
                            context->colorAnimBoard = nullptr;
                        }
                    });
                }
                
                storyboard.Begin();
            }
        }
    }
}

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Shapes;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Hosting;
using namespace winrt::Windows::UI::Composition;

FrameworkElement GetFrameworkElementFromNative(void* pThis) {
    if (!pThis) return nullptr;
    try {
        void* iUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown iUnknown;
        winrt::copy_from_abi(iUnknown, iUnknownPtr);
        return iUnknown.try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
    } catch (...) {
        return nullptr;
    }
}

FrameworkElement GetFrameworkElementFromInterface(void* pInterface) {
    if (!pInterface) return nullptr;
    try {
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

FrameworkElement FindChildByClassName(FrameworkElement const& parent, std::wstring_view className, int depth = 0) {
    if (!parent || depth > 5) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (winrt::get_class_name(child) == className) return child;
            auto result = FindChildByClassName(child, className, depth + 1);
            if (result) return result;
        }
    }
    return nullptr;
}

void RestoreNativeIndicators(FrameworkElement const& parent) {
    if (!parent) return;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (winrt::get_class_name(child) == L"Taskbar.TaskListButton") {
                auto indicator = FindChildByName(child, L"RunningIndicator");
                if (indicator) {
                    indicator.Opacity(1.0);
                }
            }
            RestoreNativeIndicators(child);
        }
    }
}

VisualStateGroup GetVisualStateGroup(FrameworkElement const& root, std::wstring_view groupName) {
    auto groups = VisualStateManager::GetVisualStateGroups(root);
    for (auto const& group : groups) {
        if (group.Name() == groupName) return group;
    }
    return nullptr;
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

void EnsurePillAndPosition(winrt::Windows::UI::Xaml::FrameworkElement const& button, bool isActive, const Settings& localSettings);

bool UpdatePillPosition(
    winrt::Windows::UI::Xaml::Shapes::Rectangle const& p,
    winrt::Windows::UI::Xaml::Controls::Grid const& g,
    winrt::Windows::UI::Xaml::FrameworkElement const& b,
    Settings const& localSettings);

struct ButtonEventTokens {
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> btn;
    winrt::event_token enter;
    winrt::event_token exit;
    winrt::Windows::Foundation::IInspectable press;
    winrt::Windows::Foundation::IInspectable release;
};
std::vector<ButtonEventTokens>* g_attachedPointerButtons = new std::vector<ButtonEventTokens>();
std::mutex g_attachedPointerButtonsMutex;

static void HandlePointerUpdate(std::weak_ptr<PillContext> weakCtx, winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> weakBtn, bool hover, bool press) {
    if (g_unloading) return;
    if (auto c = weakCtx.lock()) {
        if (auto b = weakBtn.get()) {
            if (c->activeBtn.get() == b) {
                auto p = c->pill.get();
                auto g = c->grid.get();
                if (!p || !g) return;

                c->isHover = hover;
                c->isPress = press;
                Settings localSettings;
                { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
                
                try {
                    UpdatePillPosition(p, g, b, localSettings);
                    
                    std::wstring appName = winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(b).c_str();
                    std::wstring stableKey = appName;
                    if (stableKey.empty()) {
                        stableKey = std::to_wstring((uintptr_t)winrt::get_abi(b));
                    }
                    bool isExtracting = false;
                    {
                        std::lock_guard<std::mutex> lock(g_iconColorMutex);
                        isExtracting = g_iconColorExtracting->count(stableKey) > 0;
                    }
                    UpdatePillColor(b, p, c, localSettings, true, c->isHover, c->isPress, stableKey, isExtracting);
                } catch (...) {}
            }
        }
    }
}

struct ModPointerEnterHandler {
    std::weak_ptr<PillContext> weakCtx;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> weakBtn;
    void operator()(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) const {
        HandlePointerUpdate(weakCtx, weakBtn, true, false);
    }
};

struct ModPointerExitHandler {
    std::weak_ptr<PillContext> weakCtx;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> weakBtn;
    void operator()(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) const {
        HandlePointerUpdate(weakCtx, weakBtn, false, false);
    }
};

struct ModPointerPressHandler {
    std::weak_ptr<PillContext> weakCtx;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> weakBtn;
    void operator()(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) const {
        HandlePointerUpdate(weakCtx, weakBtn, true, true);
    }
};

struct ModPointerReleaseHandler {
    std::weak_ptr<PillContext> weakCtx;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> weakBtn;
    void operator()(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) const {
        HandlePointerUpdate(weakCtx, weakBtn, true, false);
    }
};

void AttachPointerEvents(winrt::Windows::UI::Xaml::FrameworkElement const& button, std::shared_ptr<PillContext> ctx) {
    if (!button || !ctx) return;
    Settings localSettings;
    { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
    
    void* pBtn = winrt::get_abi(button);
    {
        std::lock_guard<std::mutex> lock(g_attachedPointerButtonsMutex);
        g_attachedPointerButtons->erase(
            std::remove_if(g_attachedPointerButtons->begin(), g_attachedPointerButtons->end(),
                [](const ButtonEventTokens& item) { return item.btn.get() == nullptr; }),
            g_attachedPointerButtons->end());

        for (auto& item : *g_attachedPointerButtons) {
            if (auto b = item.btn.get()) {
                if (winrt::get_abi(b) == pBtn) return;
            }
        }
    }

    auto weakCtx = std::weak_ptr<PillContext>(ctx);
    auto weakBtn = winrt::make_weak(button);

    winrt::Windows::UI::Xaml::Input::PointerEventHandler enterHandler = ModPointerEnterHandler{weakCtx, weakBtn};
    winrt::Windows::UI::Xaml::Input::PointerEventHandler exitHandler = ModPointerExitHandler{weakCtx, weakBtn};
    winrt::Windows::UI::Xaml::Input::PointerEventHandler pressHandler = ModPointerPressHandler{weakCtx, weakBtn};
    winrt::Windows::UI::Xaml::Input::PointerEventHandler releaseHandler = ModPointerReleaseHandler{weakCtx, weakBtn};

    ButtonEventTokens tokens;
    tokens.btn = weakBtn;
    tokens.enter = button.PointerEntered(enterHandler);
    tokens.exit = button.PointerExited(exitHandler);
    tokens.press = winrt::box_value(pressHandler);
    tokens.release = winrt::box_value(releaseHandler);
    
    button.AddHandler(winrt::Windows::UI::Xaml::UIElement::PointerPressedEvent(), tokens.press, true);
    button.AddHandler(winrt::Windows::UI::Xaml::UIElement::PointerReleasedEvent(), tokens.release, true);
    button.AddHandler(winrt::Windows::UI::Xaml::UIElement::PointerCanceledEvent(), tokens.release, true);
    button.AddHandler(winrt::Windows::UI::Xaml::UIElement::PointerCaptureLostEvent(), tokens.release, true);

    {
        std::lock_guard<std::mutex> lock(g_attachedPointerButtonsMutex);
        g_attachedPointerButtons->push_back(tokens);
    }
}

void ExtractIconColor(winrt::Windows::UI::Xaml::FrameworkElement button, winrt::Windows::UI::Xaml::Controls::Image icon, winrt::Windows::UI::Xaml::Shapes::Rectangle pill, std::wstring stableKey) {
    auto weakButton = winrt::make_weak(button);
    auto weakPill = winrt::make_weak(pill);
    auto dispatcher = button.Dispatcher();
    if (!dispatcher) {
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        g_iconColorExtracting->erase(stableKey);
        return;
    }

    try {
        auto rtb = winrt::Windows::UI::Xaml::Media::Imaging::RenderTargetBitmap();
        auto renderOp = rtb.RenderAsync(icon);
        renderOp.Completed([weakButton, weakPill, rtb, stableKey, dispatcher](auto&& op, auto&& status) {
            if (g_unloading) return;
            if (status != winrt::Windows::Foundation::AsyncStatus::Completed) {
                Wh_Log(L"ExtractIconColor: renderOp failed, status: %d", (int)status);
                std::lock_guard<std::mutex> lock(g_iconColorMutex);
                g_iconColorExtracting->erase(stableKey);
                return;
            }
            
            // Hop to UI thread to call GetPixelsAsync
            dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [weakButton, weakPill, rtb, stableKey, dispatcher]() {
                if (g_unloading) return;
                try {
                    auto pixelOp = rtb.GetPixelsAsync();
                    pixelOp.Completed([weakButton, weakPill, stableKey, dispatcher](auto&& op2, auto&& status2) {
                        if (g_unloading) return;
                        if (status2 != winrt::Windows::Foundation::AsyncStatus::Completed) {
                            Wh_Log(L"ExtractIconColor: pixelOp failed, status: %d", (int)status2);
                            std::lock_guard<std::mutex> lock(g_iconColorMutex);
                            g_iconColorExtracting->erase(stableKey);
                            return;
                        }
                        try {
                            auto pixels = op2.GetResults();
                            if (!pixels) {
                                Wh_Log(L"ExtractIconColor: pixels is null");
                                std::lock_guard<std::mutex> lock(g_iconColorMutex);
                                g_iconColorExtracting->erase(stableKey);
                                return;
                            }

                            winrt::com_ptr<IBufferByteAccess_Local> buffer;
                            winrt::get_unknown(pixels)->QueryInterface(IID_IBufferByteAccess_Local, buffer.put_void());
                            if (buffer) {
                                uint8_t* data = nullptr;
                                HRESULT hrBuf = buffer->Buffer(&data);
                                if (SUCCEEDED(hrBuf) && data) {
                                    uint32_t length = pixels.Length();
                                    struct ColorBucket {
                                        uint64_t r = 0, g = 0, b = 0;
                                        uint32_t count = 0;
                                        uint64_t score = 0;
                                    };
                                    ColorBucket buckets[64] = {};
                                    
                                    Wh_Log(L"ExtractIconColor: processing buffer, length %u", length);
                                    
                                    // Ensure we don't go out of bounds
                                    if (length >= 4) {
                                        for (uint32_t idx = 0; idx <= length - 4; idx += 4) {
                                            uint8_t a = data[idx + 3];
                                            if (a > 50) {
                                                uint8_t b_val = data[idx];
                                                uint8_t g_val = data[idx + 1];
                                                uint8_t r_val = data[idx + 2];
                                                
                                                uint32_t rIdx = r_val >> 6;
                                                uint32_t gIdx = g_val >> 6;
                                                uint32_t bIdx = b_val >> 6;
                                                uint32_t bucketIdx = (rIdx << 4) | (gIdx << 2) | bIdx;
                                                
                                                uint8_t cmax = r_val;
                                                if (g_val > cmax) cmax = g_val;
                                                if (b_val > cmax) cmax = b_val;
                                                uint8_t cmin = r_val;
                                                if (g_val < cmin) cmin = g_val;
                                                if (b_val < cmin) cmin = b_val;
                                                uint32_t saturation = cmax - cmin;
                                                
                                                buckets[bucketIdx].r += r_val;
                                                buckets[bucketIdx].g += g_val;
                                                buckets[bucketIdx].b += b_val;
                                                buckets[bucketIdx].count++;
                                                buckets[bucketIdx].score += (saturation * 2) + cmax + 10;
                                            }
                                        }
                                    }
                                    
                                    uint32_t bestBucket = 0;
                                    uint64_t bestScore = 0;
                                    for (uint32_t i = 0; i < 64; i++) {
                                        if (buckets[i].score > bestScore) {
                                            bestScore = buckets[i].score;
                                            bestBucket = i;
                                        }
                                    }
                                    
                                    if (buckets[bestBucket].count > 0) {
                                        uint8_t finalR = buckets[bestBucket].r / buckets[bestBucket].count;
                                        uint8_t finalG = buckets[bestBucket].g / buckets[bestBucket].count;
                                        uint8_t finalB = buckets[bestBucket].b / buckets[bestBucket].count;
                                        winrt::Windows::UI::Color baseColor = {255, finalR, finalG, finalB};
                                        
                                        ExtractedColors variants;
                                        variants.lightMode = AdaptColorForTheme(baseColor, true);
                                        variants.darkMode = AdaptColorForTheme(baseColor, false);
                                        
                                        Wh_Log(L"ExtractIconColor: success base R:%d G:%d B:%d", baseColor.R, baseColor.G, baseColor.B);
                                        {
                                            std::lock_guard<std::mutex> lock(g_iconColorMutex);
                                            if (g_iconColorCache->size() > 100) {
                                                g_iconColorCache->clear();
                                                g_iconColorExtracting->clear();
                                            }
                                            (*g_iconColorCache)[stableKey] = variants;
                                        }
                                        
                                        dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [weakPill, weakButton, stableKey]() {
                                            if (g_unloading) return;
                                            auto button = weakButton.get();
                                            auto pill = weakPill.get();
                                            if (!button || !pill) return;
                                            try {
                                                auto iconPanel = FindChildByName(button, L"IconPanel");
                                                auto group = iconPanel ? GetVisualStateGroup(iconPanel, L"RunningIndicatorStates") : nullptr;
                                                auto state = group ? group.CurrentState() : nullptr;
                                                if (state && state.Name() == L"ActiveRunningIndicator") {
                                                    Settings currentSettings;
                                                    { std::lock_guard<std::mutex> sLock(g_settingsMutex); currentSettings = g_settings; }
                                                    EnsurePillAndPosition(button, true, currentSettings);
                                                }
                                            } catch(...) {}
                                        });
                                    } else {
                                        Wh_Log(L"ExtractIconColor: count is 0 (transparent image or empty)");
                                    }
                                } else {
                                    Wh_Log(L"ExtractIconColor: buffer->Buffer failed or data null, HR 0x%08X", hrBuf);
                                }
                            } else {
                                Wh_Log(L"ExtractIconColor: QueryInterface failed");
                            }
                        
                            {
                                std::lock_guard<std::mutex> lock(g_iconColorMutex);
                                g_iconColorExtracting->erase(stableKey);
                            }
                        } catch(...) {
                            Wh_Log(L"ExtractIconColor: exception in pixelOp.Completed");
                            std::lock_guard<std::mutex> lock(g_iconColorMutex);
                            g_iconColorExtracting->erase(stableKey);
                        }
                    });
                } catch(...) {
                    Wh_Log(L"ExtractIconColor: exception calling rtb.GetPixelsAsync");
                    std::lock_guard<std::mutex> lock(g_iconColorMutex);
                    g_iconColorExtracting->erase(stableKey);
                }
            });
        });
    } catch(...) {
        Wh_Log(L"ExtractIconColor: exception in RenderAsync");
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        g_iconColorExtracting->erase(stableKey);
    }
}

bool UpdatePillPosition(
    winrt::Windows::UI::Xaml::Shapes::Rectangle const& p,
    winrt::Windows::UI::Xaml::Controls::Grid const& g,
    winrt::Windows::UI::Xaml::FrameworkElement const& b,
    Settings const& localSettings)
{
    if (b.ActualWidth() <= 0.001f) return false;

    auto transform = b.TransformToVisual(g);
    auto point = transform.TransformPoint({0, 0});
    
    double actW = p.ActualWidth();
    double pillW = actW > 0 ? actW : p.Width();
    if (pillW < 1.0) pillW = 1.0;
    
    double actH = p.ActualHeight();
    double pillH = actH > 0 ? actH : p.Height();
    if (pillH < 1.0) pillH = 1.0;

    float layoutX = static_cast<float>(g.Padding().Left + g.BorderThickness().Left + p.Margin().Left);
    float targetX = std::round((float)(point.X + (b.ActualWidth() / 2.0) - (pillW / 2.0))) - layoutX;
    float pillYOffset = (float)(pillH - localSettings.PillHeight) / 2.0f;
    
    std::shared_ptr<PillContext> ctx = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_pillsMutex);
        for (auto& c : *g_pillContexts) {
            if (c->grid.get() == g) {
                ctx = c;
                break;
            }
        }
    }

    auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(p);
    float lastTargetX = std::numeric_limits<float>::quiet_NaN();
    if (visual.Properties().TryGetScalar(L"LastTargetX", lastTargetX) != winrt::Windows::UI::Composition::CompositionGetValueStatus::Succeeded) {
        lastTargetX = std::numeric_limits<float>::quiet_NaN();
    }

    bool shouldSnap = std::isnan(lastTargetX);
    if (ctx && ctx->forceSnapNext) {
        shouldSnap = true;
        ctx->forceSnapNext = false;
    }

    visual.CenterPoint(winrt::Windows::Foundation::Numerics::float3((float)pillW / 2.0f, (float)pillH / 2.0f, 0.0f));

    if (shouldSnap || std::abs(lastTargetX - targetX) > 0.1f) {
        visual.Properties().InsertScalar(L"LastTargetX", targetX);
        visual.Properties().InsertScalar(L"LayoutW", (float)pillW);

        if (shouldSnap) {
            visual.Properties().InsertVector3(L"Translation", winrt::Windows::Foundation::Numerics::float3(targetX, pillYOffset, 0.0f));
            visual.Properties().InsertScalar(L"LeftX", targetX);
            visual.Properties().InsertScalar(L"RightX", targetX + (float)pillW);
            visual.Properties().StopAnimation(L"BaseScale");
            visual.Properties().InsertVector3(L"BaseScale", winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
        } else {
            auto compositor = visual.Compositor();
            DWORD tid = GetCurrentThreadId();
            EasingCache* pCache = nullptr;
            {
                std::lock_guard<std::mutex> lock(g_easingMutex);
                pCache = &(*g_easingCaches)[tid];
            }
            EasingCache& cache = *pCache;

            if (cache.compositor != compositor) {
                cache.compositor = compositor;
                cache.stretchLeadEase = compositor.CreateCubicBezierEasingFunction({0.0f, 0.0f}, {0.0f, 1.0f});
                cache.stretchTrailEase = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {0.2f, 1.0f});
                cache.squishEase = compositor.CreateCubicBezierEasingFunction({0.25f, 0.1f}, {0.25f, 1.0f});
                cache.linearEase = compositor.CreateLinearEasingFunction();
                cache.easeIn = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {1.0f, 1.0f});
                cache.easeOut = compositor.CreateCubicBezierEasingFunction({0.0f, 0.0f}, {0.5f, 1.0f});
                cache.easeInOut = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {0.5f, 1.0f});
            }

            if (localSettings.FadeTransition) {
                auto opacityAnim = compositor.CreateScalarKeyFrameAnimation();
                opacityAnim.InsertKeyFrame(0.0f, 1.0f);
                opacityAnim.InsertKeyFrame(0.5f, 0.0f);
                opacityAnim.InsertKeyFrame(1.0f, 1.0f);
                opacityAnim.Duration(std::chrono::milliseconds(static_cast<long long>((300.0 * localSettings.FadeDurationMultiplier) / localSettings.SpeedMultiplier)));
                visual.Properties().StartAnimation(L"FadeOpacity", opacityAnim);
            } else {
                visual.Properties().InsertScalar(L"FadeOpacity", 1.0f);
            }

            int animStyle = localSettings.AnimationStyle;
            bool useSquish = localSettings.SquishTransition;

            // 1. Handle Horizontal Movement
            if (animStyle == 1) { // Bounce
                auto anim = compositor.CreateSpringScalarAnimation();
                anim.Target(L"Translation.X");
                anim.FinalValue(targetX);
                anim.DampingRatio(std::clamp(0.6f / (float)localSettings.ElasticIntensity, 0.1f, 1.0f));
                anim.Period(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(static_cast<long long>(50 / localSettings.SpeedMultiplier))));
                visual.Properties().StartAnimation(L"Translation.X", anim);
            } else if (animStyle == 2 || (animStyle >= 4 && animStyle <= 6)) { // KeyFrame Easing
                auto anim = compositor.CreateScalarKeyFrameAnimation();
                CompositionEasingFunction easing = nullptr;
                if (animStyle == 2) easing = cache.linearEase;
                else if (animStyle == 4) easing = cache.easeIn;
                else if (animStyle == 5) easing = cache.easeOut;
                else if (animStyle == 6) easing = cache.easeInOut;

                anim.InsertKeyFrame(1.0f, targetX, easing);
                anim.Duration(std::chrono::milliseconds(static_cast<long long>(200 / localSettings.SpeedMultiplier)));
                visual.Properties().StartAnimation(L"Translation.X", anim);
            } else if (animStyle == 8) { // None
                visual.Properties().InsertVector3(L"Translation", winrt::Windows::Foundation::Numerics::float3(targetX, pillYOffset, 0.0f));
                visual.Properties().InsertScalar(L"LeftX", targetX);
                visual.Properties().InsertScalar(L"RightX", targetX + (float)pillW);
            } else { // Stretch (animStyle == 0 or 7)
                float targetLeft = targetX;
                float targetRight = targetX + (float)pillW;
                float currentLeft = lastTargetX;
                bool movingRight = targetLeft > currentLeft;

                auto propSet = visual.Properties();

                if (localSettings.UseSpringPhysics || animStyle == 7) {
                    float baseDamping = animStyle == 7 ? 0.70f : 1.0f;
                    float damping = std::clamp(baseDamping / (float)localSettings.ElasticIntensity, 0.1f, 1.0f);
                    long long periodL = static_cast<long long>((movingRight ? 60.0 : 45.0) / localSettings.SpeedMultiplier);
                    long long periodR = static_cast<long long>((movingRight ? 45.0 : 60.0) / localSettings.SpeedMultiplier);
                    
                    auto leftAnim = compositor.CreateSpringScalarAnimation();
                    leftAnim.FinalValue(targetLeft);
                    leftAnim.DampingRatio(damping);
                    leftAnim.Period(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(periodL)));

                    auto rightAnim = compositor.CreateSpringScalarAnimation();
                    rightAnim.FinalValue(targetRight);
                    rightAnim.DampingRatio(damping);
                    rightAnim.Period(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(periodR)));

                    propSet.StartAnimation(L"LeftX", leftAnim);
                    propSet.StartAnimation(L"RightX", rightAnim);
                } else {
                    auto leftAnim = compositor.CreateScalarKeyFrameAnimation();
                    leftAnim.InsertKeyFrame(1.0f, targetLeft, movingRight ? cache.stretchTrailEase : cache.stretchLeadEase);
                    leftAnim.Duration(std::chrono::milliseconds(static_cast<long long>(300 / localSettings.SpeedMultiplier)));

                    auto rightAnim = compositor.CreateScalarKeyFrameAnimation();
                    rightAnim.InsertKeyFrame(1.0f, targetRight, movingRight ? cache.stretchLeadEase : cache.stretchTrailEase);
                    rightAnim.Duration(std::chrono::milliseconds(static_cast<long long>(300 / localSettings.SpeedMultiplier)));

                    propSet.StartAnimation(L"LeftX", leftAnim);
                    propSet.StartAnimation(L"RightX", rightAnim);
                }

                auto offsetExp = compositor.CreateExpressionAnimation(L"(props.LeftX + props.RightX - props.LayoutW) / 2.0");
                offsetExp.SetReferenceParameter(L"props", propSet);
                visual.Properties().StartAnimation(L"Translation.X", offsetExp);
            }

            // 2. Handle Scaling & Squish
            if (animStyle == 0 || animStyle == 7) { // Stretch uses expression scaling
                auto propSet = visual.Properties();
                if (useSquish) {
                    float squishY = std::fmax(0.1f, 1.0f - (0.5f * localSettings.SquishMultiplierY));
                    auto squishYAnim = compositor.CreateScalarKeyFrameAnimation();
                    squishYAnim.InsertKeyFrame(0.0f, 1.0f);
                    squishYAnim.InsertKeyFrame(0.5f, squishY, cache.squishEase);
                    squishYAnim.InsertKeyFrame(1.0f, 1.0f, cache.squishEase);
                    squishYAnim.Duration(std::chrono::milliseconds(static_cast<long long>(300 / localSettings.SpeedMultiplier)));
                    propSet.InsertScalar(L"SquishY", 1.0f);
                    propSet.StartAnimation(L"SquishY", squishYAnim);
                    
                    float squishX = std::fmax(0.1f, 1.0f + (0.5f * localSettings.SquishMultiplierX));
                    auto squishXAnim = compositor.CreateScalarKeyFrameAnimation();
                    squishXAnim.InsertKeyFrame(0.0f, 1.0f);
                    squishXAnim.InsertKeyFrame(0.5f, squishX, cache.squishEase);
                    squishXAnim.InsertKeyFrame(1.0f, 1.0f, cache.squishEase);
                    squishXAnim.Duration(std::chrono::milliseconds(static_cast<long long>(300 / localSettings.SpeedMultiplier)));
                    propSet.InsertScalar(L"SquishX", 1.0f);
                    propSet.StartAnimation(L"SquishX", squishXAnim);
                    
                    auto scaleExp = compositor.CreateExpressionAnimation(L"Vector3(((props.RightX - props.LeftX) / props.LayoutW) * props.SquishX, props.SquishY, 1.0)");
                    scaleExp.SetReferenceParameter(L"props", propSet);
                    visual.Properties().StartAnimation(L"BaseScale", scaleExp);
                } else {
                    auto scaleExp = compositor.CreateExpressionAnimation(L"Vector3((props.RightX - props.LeftX) / props.LayoutW, 1.0, 1.0)");
                    scaleExp.SetReferenceParameter(L"props", propSet);
                    visual.Properties().StartAnimation(L"BaseScale", scaleExp);
                }
            } else { // Standard rigid modes (Bounce, Linear, Ease)
                if (useSquish) {
                    auto scaleAnim = compositor.CreateVector3KeyFrameAnimation();
                    scaleAnim.InsertKeyFrame(0.0f, winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
                    float squishY = std::fmax(0.1f, 1.0f - (0.5f * localSettings.SquishMultiplierY));
                    float squishX = std::fmax(0.1f, 1.0f + ((animStyle == 8 ? 0.2f : 0.5f) * localSettings.SquishMultiplierX));
                    scaleAnim.InsertKeyFrame(0.5f, winrt::Windows::Foundation::Numerics::float3(squishX, squishY, 1.0f), cache.squishEase);
                    scaleAnim.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f), cache.squishEase);
                    scaleAnim.Duration(std::chrono::milliseconds(static_cast<long long>(300 / localSettings.SpeedMultiplier)));
                    visual.Properties().StartAnimation(L"BaseScale", scaleAnim);
                } else {
                    visual.Properties().StopAnimation(L"BaseScale");
                    visual.Properties().InsertVector3(L"BaseScale", winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
                }
            }
        }
    }

    float pScaleX = 1.0f;
    float pScaleY = 1.0f;
    if (ctx && ctx->isPress) {
        pScaleX = (float)localSettings.PressedScaleX;
        pScaleY = (float)localSettings.PressedScaleY;
    } else if (ctx && ctx->isHover) {
        pScaleX = (float)localSettings.HoverScaleX;
        pScaleY = (float)localSettings.HoverScaleY;
    }
    
    winrt::Windows::Foundation::Numerics::float3 targetPScale = {pScaleX, pScaleY, 1.0f};
    winrt::Windows::Foundation::Numerics::float3 currentPScale = {1.0f, 1.0f, 1.0f};
    visual.Properties().TryGetVector3(L"PointerScaleTarget", currentPScale);
    if (std::abs(currentPScale.x - targetPScale.x) > 0.001f || std::abs(currentPScale.y - targetPScale.y) > 0.001f) {
        visual.Properties().InsertVector3(L"PointerScaleTarget", targetPScale);
        auto pointerScaleAnim = visual.Compositor().CreateVector3KeyFrameAnimation();
        pointerScaleAnim.InsertKeyFrame(1.0f, targetPScale);
        pointerScaleAnim.Duration(std::chrono::milliseconds(static_cast<long long>(150 / localSettings.SpeedMultiplier)));
        visual.Properties().StartAnimation(L"PointerScaleVec", pointerScaleAnim);
    }

    float lastOpacityTarget = -1.0f;
    visual.Properties().TryGetScalar(L"LastShowOpacityTarget", lastOpacityTarget);
    if (lastOpacityTarget != 1.0f) {
        visual.Properties().InsertScalar(L"LastShowOpacityTarget", 1.0f);
        auto fadeAnim = visual.Compositor().CreateScalarKeyFrameAnimation();
        fadeAnim.InsertKeyFrame(1.0f, 1.0f);
        fadeAnim.Duration(std::chrono::milliseconds(static_cast<long long>((150.0 * localSettings.FadeDurationMultiplier) / localSettings.SpeedMultiplier)));
        visual.Properties().StartAnimation(L"ShowOpacity", fadeAnim);
    }

    return true;
}

void AttachStateChangedHandler(winrt::Windows::UI::Xaml::VisualStateGroup const& group, winrt::Windows::UI::Xaml::FrameworkElement const& button) {
    if (!group) return;
    void* pGroup = winrt::get_abi(group);
    
    {
        std::lock_guard<std::mutex> lock(g_attachedGroupsMutex);
        bool found = false;
        for (auto it = g_attachedGroups->begin(); it != g_attachedGroups->end(); ) {
            if (auto g = it->get()) {
                if (winrt::get_abi(g) == pGroup) { found = true; break; }
                ++it;
            } else {
                it = g_attachedGroups->erase(it);
            }
        }
        if (found) return;
        g_attachedGroups->push_back(winrt::make_weak(group));
    }

    auto weakElem = winrt::make_weak(button);
    auto token = group.CurrentStateChanged([weakElem](auto const& sender, auto const& args) {
        auto b = weakElem.get();
        if (!b) return;
        
        auto dispatcher = b.Dispatcher();
        dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [weakElem, args]() {
            if (g_unloading) return;
            try {
                auto b = weakElem.get();
                if (!b) return;

                Settings localSettings;
                { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
                if (!localSettings.TrackSystemButtons) return;

                bool isActive = false;
                if (args.NewState()) {
                    std::wstring stateName = args.NewState().Name().c_str();
                    if (stateName == L"ActiveNormal" || stateName == L"ActivePointerOver" || stateName == L"ActivePressed") {
                        isActive = true;
                    }
                }

                EnsurePillAndPosition(b, isActive, localSettings);
            } catch (...) {}
        });
    });

    auto dispatcher = button.Dispatcher();
    {
        std::lock_guard<std::mutex> lock(g_attachedEventsMutex);
        g_attachedEvents->push_back({winrt::make_weak(group), token, dispatcher});
    }
}

void AttachSearchButton(winrt::Windows::UI::Xaml::Controls::Grid const& grid) {
    if (!grid) return;
    auto searchBtn = FindChildByClassName(grid, L"SearchUx.SearchUI.SearchButtonControl");
    if (searchBtn) {
        Wh_Log(L"Found SearchButtonControl in RootGrid");
        if (auto sIconButton = FindChildByClassName(searchBtn, L"SearchUx.SearchUI.SearchIconButton")) {
            auto sRootGrid = FindChildByName(sIconButton, L"SearchBoxButtonRootPanel");
            if (sRootGrid) {
                if (auto sGroup = GetVisualStateGroup(sRootGrid, L"CommonStates")) {
                    AttachStateChangedHandler(sGroup, searchBtn);
                    Wh_Log(L"Successfully attached to Search button!");
                }
            }
        }
    }
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_unloading) return;

    auto elem = GetFrameworkElementFromNative(pThis);
    if (!elem) return;

    auto dispatcher = elem.Dispatcher();
    auto weakElem = winrt::make_weak(elem);

    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [weakElem]() {
        if (g_unloading) return;
        try {
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }

            auto button = weakElem.get();
            if (!button) return;

            auto iconPanel = FindChildByName(button, L"IconPanel");
            auto group = iconPanel ? GetVisualStateGroup(iconPanel, L"RunningIndicatorStates") : nullptr;
            auto currentState = group ? group.CurrentState() : nullptr;
            bool isActive = (currentState && currentState.Name() == L"ActiveRunningIndicator");

            // Hide native RunningIndicator
            auto runningIndicator = FindChildByName(button, L"RunningIndicator");
            if (runningIndicator) {
                if (localSettings.HideInactiveDots || isActive) {
                    runningIndicator.Opacity(0.0);
                } else {
                    runningIndicator.Opacity(1.0);
                }
            }

            EnsurePillAndPosition(button, isActive, localSettings);
        } catch (...) {
            Wh_Log(L"Exception in UpdateVisualStates hook");
        }
    });
}

void EnsurePillAndPosition(winrt::Windows::UI::Xaml::FrameworkElement const& button, bool isActive, const Settings& localSettings) {
    // Find TaskbarFrame -> RootGrid
    FrameworkElement current = button;
    FrameworkElement rootGrid = nullptr;
    int searchDepth = 0;
    while (current && searchDepth < 20) {
        searchDepth++;
        if (winrt::get_class_name(current) == L"Taskbar.TaskbarFrame") {
            rootGrid = FindChildByName(current, L"RootGrid");
            break;
        }
        auto parent = VisualTreeHelper::GetParent(current);
        current = parent ? parent.try_as<FrameworkElement>() : nullptr;
    }

    if (!rootGrid) return;
    auto grid = rootGrid.try_as<Grid>();
    if (!grid) return;

    // Ensure ElasticPill exists in RootGrid
    auto pill = FindChildByName(grid, L"ElasticPill").try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>();
    auto visual = pill ? ElementCompositionPreview::GetElementVisual(pill) : nullptr;
    
    float currentTag = 0.0f;
    if (pill && pill.Tag()) {
        try { currentTag = winrt::unbox_value<float>(pill.Tag()); } catch(...) {}
    }

    if (!pill || currentTag != 4.0f) {
        if (!pill) {
            pill = winrt::Windows::UI::Xaml::Shapes::Rectangle();
            pill.Name(L"ElasticPill");
            pill.IsHitTestVisible(false);
            pill.HorizontalAlignment(HorizontalAlignment::Left);
            pill.VerticalAlignment(VerticalAlignment::Bottom);
            Canvas::SetZIndex(pill, 999);
            grid.Children().Append(pill);
            ElementCompositionPreview::SetIsTranslationEnabled(pill, true);
            visual = ElementCompositionPreview::GetElementVisual(pill);
        }
        if (!visual) return;
        visual.Properties().InsertScalar(L"LeftX", 0.0f);
        visual.Properties().InsertScalar(L"RightX", (float)localSettings.PillWidth * 4.0f);
        visual.Properties().InsertScalar(L"ShowOpacity", 0.0f);
        visual.Properties().InsertScalar(L"LastShowOpacityTarget", -1.0f);
        visual.Properties().InsertScalar(L"FadeOpacity", 1.0f);
        visual.Properties().InsertVector3(L"PointerScaleVec", {1.0f, 1.0f, 1.0f});
        visual.Properties().InsertVector3(L"BaseScale", {1.0f, 1.0f, 1.0f});

        auto opacityExp = visual.Compositor().CreateExpressionAnimation(L"props.ShowOpacity * props.FadeOpacity");
        opacityExp.SetReferenceParameter(L"props", visual.Properties());
        visual.StartAnimation(L"Opacity", opacityExp);
        
        auto finalScaleExp = visual.Compositor().CreateExpressionAnimation(L"(props.BaseScale * props.PointerScaleVec) * 0.25f");
        finalScaleExp.SetReferenceParameter(L"props", visual.Properties());
        visual.StartAnimation(L"Scale", finalScaleExp);

        pill.Tag(winrt::box_value(4.0f));
    }

    if (!visual) return;
    
    // Ensure visual properties are up-to-date with settings
    Settings settingsToUse = localSettings;
    settingsToUse.PillHeight = (std::max)(1, settingsToUse.PillHeight);
    settingsToUse.PillWidth = (std::max)(1, settingsToUse.PillWidth);
    
    float lastW = -1.0f, lastH = -1.0f, lastR = -1.0f, lastMB = -1.0f, lastMH = -1.0f;
    visual.Properties().TryGetScalar(L"SettingsW", lastW);
    visual.Properties().TryGetScalar(L"SettingsH", lastH);
    visual.Properties().TryGetScalar(L"SettingsR", lastR);
    visual.Properties().TryGetScalar(L"SettingsMB", lastMB);
    visual.Properties().TryGetScalar(L"SettingsMH", lastMH);

    if (std::abs(lastW - (float)settingsToUse.PillWidth) > 0.001f || 
        std::abs(lastH - (float)settingsToUse.PillHeight) > 0.001f || 
        std::abs(lastR - (float)settingsToUse.PillRadius) > 0.001f || 
        std::abs(lastMB - (float)settingsToUse.PillMarginBottom) > 0.001f || 
        std::abs(lastMH - (float)settingsToUse.PillMarginHorizontal) > 0.001f) {
        
        visual.Properties().InsertScalar(L"SettingsW", (float)settingsToUse.PillWidth);
        visual.Properties().InsertScalar(L"SettingsH", (float)settingsToUse.PillHeight);
        visual.Properties().InsertScalar(L"SettingsR", (float)settingsToUse.PillRadius);
        visual.Properties().InsertScalar(L"SettingsMB", (float)settingsToUse.PillMarginBottom);
        visual.Properties().InsertScalar(L"SettingsMH", (float)settingsToUse.PillMarginHorizontal);
        
        visual.Properties().InsertScalar(L"LayoutW", (float)settingsToUse.PillWidth * 4.0f);
        
        visual.Properties().InsertScalar(L"LastTargetX", std::numeric_limits<float>::quiet_NaN());
        
        pill.Height(settingsToUse.PillHeight * 4.0);
        pill.Width(settingsToUse.PillWidth * 4.0);
        pill.RadiusX(settingsToUse.PillRadius * 4.0);
        pill.RadiusY(settingsToUse.PillRadius * 4.0);
        pill.Margin({(double)settingsToUse.PillMarginHorizontal, 0, (double)settingsToUse.PillMarginHorizontal, (double)settingsToUse.PillMarginBottom});
    }

    std::shared_ptr<PillContext> ctx = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_pillsMutex);
        if (g_pillContexts->size() > 10) {
            g_pillContexts->erase(
                std::remove_if(g_pillContexts->begin(), g_pillContexts->end(),
                    [](auto& c) { return c->grid.get() == nullptr; }),
                g_pillContexts->end());
        }

        auto it = std::find_if(g_pillContexts->begin(), g_pillContexts->end(), [&](const std::shared_ptr<PillContext>& c) {
            return c->grid.get() == grid;
        });

        if (it != g_pillContexts->end()) {
            ctx = *it;
            ctx->pill = winrt::make_weak(pill);
        } else {
            ctx = std::make_shared<PillContext>();
            ctx->grid = winrt::make_weak(grid);
            ctx->pill = winrt::make_weak(pill);
            g_pillContexts->push_back(ctx);

            auto weakCtx = std::weak_ptr<PillContext>(ctx);
            auto weakGrid = winrt::make_weak(grid);
            ctx->layoutToken = grid.LayoutUpdated([weakCtx, weakGrid](auto const&, auto const&) {
                if (g_unloading) return;
                auto context = weakCtx.lock();
                auto g = weakGrid.get();
                if (!context || !g) return;

                auto active = context->activeBtn.get();
                auto p = context->pill.get();
                if (active && p) {
                    Settings localSettings;
                    { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
                    try {
                        UpdatePillPosition(p, g, active, localSettings);
                    } catch (...) { Wh_Log(L"Exception in LayoutUpdated handler"); }
                }
            });
        }
    }

    if (localSettings.TrackSystemButtons && !ctx->searchAttached) {
        try { 
            AttachSearchButton(grid); 
            ctx->searchAttached = true;
        } catch (...) {}
    }

    if (!localSettings.TrackSystemButtons) {
        if (winrt::get_class_name(button) != L"Taskbar.TaskListButton") {
            return; // Only allow TaskListButton if TrackSystemButtons is false
        }
    }

    bool anyActive = isActive;
    FrameworkElement targetButton = button;
    if (!isActive && grid) {
        if (ctx && ctx->activeBtn.get() && ctx->activeBtn.get() != button) {
            anyActive = true;
            targetButton = ctx->activeBtn.get();
        }
    }

    if (anyActive) {
        ctx->activeBtn = targetButton;
        try {
            UpdatePillPosition(pill, grid, targetButton, settingsToUse);
        } catch (...) {}
        AttachPointerEvents(targetButton, ctx);
    } else {
        ctx->activeBtn = nullptr;
    }

    std::wstring stableKey = L"";
    bool isExtracting = false;

    if (anyActive) {
        std::wstring appName = winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(targetButton).c_str();
        stableKey = appName;
        if (stableKey.empty()) {
            stableKey = std::to_wstring((uintptr_t)winrt::get_abi(targetButton));
        }
        
        if (settingsToUse.ColorMode == 2) {
            winrt::hstring cName = winrt::get_class_name(targetButton);
            if (cName == L"Taskbar.ExperienceToggleButton" || cName == L"SearchUx.SearchUI.SearchButtonControl") {
                // System buttons don't have extractable app icons, just use system accent color
            } else {
                bool needsExtract = false;
                {
                    std::lock_guard<std::mutex> lock(g_iconColorMutex);
                    auto it = g_iconColorExtracting->find(stableKey);
                    if (!g_iconColorCache->count(stableKey) && (it == g_iconColorExtracting->end() || !it->second)) {
                        needsExtract = true;
                        (*g_iconColorExtracting)[stableKey] = true;
                        isExtracting = true;
                    } else {
                        isExtracting = (it != g_iconColorExtracting->end() && it->second);
                    }
                }
                if (needsExtract) {
                    if (auto iconPanel = FindChildByName(targetButton, L"IconPanel")) {
                        if (auto icon = FindChildByName(iconPanel, L"Icon").try_as<winrt::Windows::UI::Xaml::Controls::Image>()) {
                            if (auto dispatcher = targetButton.Dispatcher()) {
                                dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [targetButton, icon, pill, stableKey]() {
                                    ExtractIconColor(targetButton, icon, pill, stableKey);
                                });
                            }
                        } else {
                            std::lock_guard<std::mutex> lock(g_iconColorMutex);
                            (*g_iconColorExtracting)[stableKey] = false;
                            isExtracting = false;
                        }
                    } else {
                        std::lock_guard<std::mutex> lock(g_iconColorMutex);
                        (*g_iconColorExtracting)[stableKey] = false;
                        isExtracting = false;
                    }
                }
            }
        }
    }

    std::shared_ptr<PillContext> currentCtx = ctx;

    bool isPointerOver = currentCtx ? currentCtx->isHover : false;
    bool isPressed = currentCtx ? currentCtx->isPress : false;
    
    UpdatePillColor(button, pill, currentCtx, settingsToUse, anyActive, isPointerOver, isPressed, stableKey, isExtracting);

    auto compositor = visual.Compositor();

    if (!anyActive) {
        if (currentCtx && currentCtx->inactiveStartTime.time_since_epoch().count() == 0) {
            currentCtx->inactiveStartTime = std::chrono::steady_clock::now();
        }
        float lastOpacityTarget = -1.0f;
        visual.Properties().TryGetScalar(L"LastShowOpacityTarget", lastOpacityTarget);
        if (lastOpacityTarget != 0.0f) {
            visual.Properties().InsertScalar(L"LastShowOpacityTarget", 0.0f);
            auto fadeAnim = compositor.CreateScalarKeyFrameAnimation();
            fadeAnim.InsertKeyFrame(1.0f, 0.0f);
            fadeAnim.Duration(std::chrono::milliseconds(static_cast<long long>((150.0 * settingsToUse.FadeDurationMultiplier) / settingsToUse.SpeedMultiplier)));
            visual.Properties().StartAnimation(L"ShowOpacity", fadeAnim);
        }
    } else {
        if (currentCtx && currentCtx->inactiveStartTime.time_since_epoch().count() != 0) {
            auto now = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - currentCtx->inactiveStartTime).count();
            if (diff > static_cast<long long>(150 / settingsToUse.SpeedMultiplier)) {
                currentCtx->forceSnapNext = true;
            }
            currentCtx->inactiveStartTime = {};
        }
    }
}

using OnExperienceToggleButtonVisualStateChanged_t = void(WINAPI*)(void*, void**, void**);
OnExperienceToggleButtonVisualStateChanged_t OnExperienceToggleButtonVisualStateChanged_Original;

void WINAPI OnExperienceToggleButtonVisualStateChanged_Hook(void* pThis, void** pSenderRef, void** pArgsRef) {
    OnExperienceToggleButtonVisualStateChanged_Original(pThis, pSenderRef, pArgsRef);
    if (g_unloading) return;

    if (!pSenderRef || !*pSenderRef) return;
    void* pSender = *pSenderRef;

    auto elem = GetFrameworkElementFromInterface(pSender);
    if (!elem) return;

    auto dispatcher = elem.Dispatcher();
    auto weakElem = winrt::make_weak(elem);

    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [weakElem]() {
        if (g_unloading) return;
        try {
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }

            if (!localSettings.TrackSystemButtons) return;

            auto button = weakElem.get();
            if (!button) return;

            auto rootPanel = FindChildByName(button, L"ExperienceToggleButtonRootPanel");
            auto group = rootPanel ? GetVisualStateGroup(rootPanel, L"CommonStates") : nullptr;
            auto currentState = group ? group.CurrentState() : nullptr;
            bool isActive = false;
            if (currentState) {
                std::wstring stateName = currentState.Name().c_str();
                if (stateName == L"ActiveNormal" || stateName == L"ActivePointerOver" || stateName == L"ActivePressed") {
                    isActive = true;
                }
            }

            EnsurePillAndPosition(button, isActive, localSettings);
        } catch (...) {
            Wh_Log(L"Exception in OnExperienceToggleButtonVisualStateChanged hook");
        }
    });
}

using SearchIconButton_UpdateVisualStates_t = void(WINAPI*)(void*);
SearchIconButton_UpdateVisualStates_t SearchIconButton_UpdateVisualStates_Original1;
SearchIconButton_UpdateVisualStates_t SearchIconButton_UpdateVisualStates_Original2;

void SearchIconButton_UpdateVisualStates_Common(void* pThis) {
    if (g_unloading) return;
    auto elem = GetFrameworkElementFromNative(pThis);
    if (!elem) return;

    auto dispatcher = elem.Dispatcher();
    auto weakElem = winrt::make_weak(elem);

    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [weakElem]() {
        if (g_unloading) return;
        try {
            Settings localSettings;
            { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }

            if (!localSettings.TrackSystemButtons) return;

            auto button = weakElem.get();
            if (!button) return;

            auto rootGrid = FindChildByName(button, L"SearchBoxButtonRootPanel");
            auto group = rootGrid ? GetVisualStateGroup(rootGrid, L"CommonStates") : nullptr;
            auto currentState = group ? group.CurrentState() : nullptr;
            bool isActive = false;
            if (currentState) {
                std::wstring stateName = currentState.Name().c_str();
                if (stateName == L"ActiveNormal" || stateName == L"ActivePointerOver" || stateName == L"ActivePressed") {
                    isActive = true;
                }
            }

            EnsurePillAndPosition(button, isActive, localSettings);
        } catch (...) {
            Wh_Log(L"Exception in SearchIconButton_UpdateVisualStates hook");
        }
    });
}

void WINAPI SearchIconButton_UpdateVisualStates_Hook1(void* pThis) {
    SearchIconButton_UpdateVisualStates_Original1(pThis);
    SearchIconButton_UpdateVisualStates_Common(pThis);
}

void WINAPI SearchIconButton_UpdateVisualStates_Hook2(void* pThis) {
    SearchIconButton_UpdateVisualStates_Original2(pThis);
    SearchIconButton_UpdateVisualStates_Common(pThis);
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
            true
        }
    };
    
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to hook Taskbar.View.dll symbols");
        return false;
    }
    return true;
}

HMODULE GetSearchUxModuleHandle() {
    return GetModuleHandle(L"SearchUx.UI.dll");
}

bool HookSearchUxDllSymbols(HMODULE module) {
    // SearchUx.UI.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(private: void __cdecl winrt::SearchUx::SearchUI::implementation::SearchIconButton::UpdateVisualStates(void))"},
            &SearchIconButton_UpdateVisualStates_Original1,
            SearchIconButton_UpdateVisualStates_Hook1,
            true
        },
        {
            {L"protected: void __cdecl winrt::SearchUx::SearchUI::implementation::SearchIconButton::PlayStateChange(void)"},
            &SearchIconButton_UpdateVisualStates_Original2,
            SearchIconButton_UpdateVisualStates_Hook2,
            true
        }
    };
    
    WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));

    if (SearchIconButton_UpdateVisualStates_Original1 || SearchIconButton_UpdateVisualStates_Original2) {
        return true;
    }

    Wh_Log(L"Failed to hook SearchUx.UI.dll symbols entirely.");
    return false;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module && !g_taskbarViewDllLoaded.exchange(true)) {
            Wh_Log(L"Taskbar View DLL loaded: %s", lpLibFileName);
            if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
        }
        if (!g_searchUxDllLoaded && GetSearchUxModuleHandle() == module && !g_searchUxDllLoaded.exchange(true)) {
            Wh_Log(L"SearchUx UI DLL loaded: %s", lpLibFileName);
            if (HookSearchUxDllSymbols(module)) Wh_ApplyHookOperations();
        }
    }
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Taskbar Elastic Pill Mod");
    LoadSettings();

    HMODULE m = GetTaskbarViewModuleHandle();
    if (m) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(m)) return FALSE;
    }
    
    HMODULE sm = GetSearchUxModuleHandle();
    if (sm) {
        g_searchUxDllLoaded = true;
        HookSearchUxDllSymbols(sm);
    }
    
    if (!m || !sm) {
        HMODULE kb = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(kb, "LoadLibraryExW");
        if (!WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original)) {
            Wh_Log(L"Failed to hook LoadLibraryExW");
            return FALSE;
        }
    }
    
    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Uninitializing Taskbar Elastic Pill Mod (Before)");
    g_unloading = true;
    
    std::vector<std::shared_ptr<PillContext>> localPills;
    {
        std::lock_guard<std::mutex> lock(g_pillsMutex);
        localPills = *g_pillContexts;
        g_pillContexts->clear();
    }

    std::vector<AttachedEvent> localEvents;
    {
        std::lock_guard<std::mutex> lock(g_attachedEventsMutex);
        localEvents = *g_attachedEvents;
        g_attachedEvents->clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_attachedGroupsMutex);
        g_attachedGroups->clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        g_iconColorCache->clear();
        g_iconColorExtracting->clear();
    }

    std::vector<ButtonEventTokens> localPointerTokens;
    {
        std::lock_guard<std::mutex> lock(g_attachedPointerButtonsMutex);
        localPointerTokens = *g_attachedPointerButtons;
        g_attachedPointerButtons->clear();
    }

    if (localPills.empty() && localEvents.empty() && localPointerTokens.empty()) return;

    std::shared_ptr<void> eventLifetime(CreateEvent(nullptr, TRUE, FALSE, nullptr), [](HANDLE h) { if(h) CloseHandle(h); });
    auto pending = std::make_shared<std::atomic<int>>((int)localPills.size() + (int)localEvents.size() + (int)localPointerTokens.size());

    for (auto& ev : localEvents) {
        if (ev.dispatcher) {
            if (ev.dispatcher.HasThreadAccess()) {
                try { if (auto group = ev.group.get()) group.CurrentStateChanged(ev.token); } catch (...) {}
                if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
            } else {
                ev.dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [ev, pending, eventLifetime]() {
                    try {
                        if (auto group = ev.group.get()) {
                            group.CurrentStateChanged(ev.token);
                        }
                    } catch (...) {}
                    if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
                });
            }
        } else {
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        }
    }

    for (auto& ctx : localPills) {
        auto grid = ctx->grid.get();
        auto pill = ctx->pill.get();
        auto layoutToken = ctx->layoutToken;
        
        if (pill) {
            auto dispatcher = pill.Dispatcher();
            if (dispatcher) {
                if (dispatcher.HasThreadAccess()) {
                    try {
                        ctx->colorAnimBoard = nullptr;
                        if (grid) { grid.LayoutUpdated(layoutToken); }
                        if (auto parent = VisualTreeHelper::GetParent(pill)) {
                            if (auto pGrid = parent.try_as<Grid>()) {
                                uint32_t index;
                                if (pGrid.Children().IndexOf(pill, index)) {
                                    pGrid.Children().RemoveAt(index);
                                }
                                RestoreNativeIndicators(pGrid);
                            }
                        }
                    } catch (...) { Wh_Log(L"Exception during pill cleanup"); }
                    
                    if (pending->fetch_sub(1) == 1 && eventLifetime.get()) {
                        SetEvent(eventLifetime.get());
                    }
                } else {
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [ctx, pill, grid, layoutToken, pending, eventLifetime]() {
                        try {
                            ctx->colorAnimBoard = nullptr;
                            if (grid) { grid.LayoutUpdated(layoutToken); }
                            if (auto parent = VisualTreeHelper::GetParent(pill)) {
                                if (auto pGrid = parent.try_as<Grid>()) {
                                    uint32_t index;
                                    if (pGrid.Children().IndexOf(pill, index)) {
                                        pGrid.Children().RemoveAt(index);
                                    }
                                    RestoreNativeIndicators(pGrid);
                                }
                            }
                        } catch (...) { Wh_Log(L"Exception during pill cleanup"); }
                        
                        if (pending->fetch_sub(1) == 1 && eventLifetime.get()) {
                            SetEvent(eventLifetime.get());
                        }
                    });
                }
            } else {
                if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
            }
        } else {
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        }
    }

    for (auto& item : localPointerTokens) {
        auto button = item.btn.get();
        if (button) {
            auto dispatcher = button.Dispatcher();
            if (dispatcher) {
                if (dispatcher.HasThreadAccess()) {
                    try {
                        button.PointerEntered(item.enter);
                        button.PointerExited(item.exit);
                        button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerPressedEvent(), item.press);
                        button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerReleasedEvent(), item.release);
                        button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerCanceledEvent(), item.release);
                        button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerCaptureLostEvent(), item.release);
                    } catch (...) { Wh_Log(L"Exception revoking pointer events"); }
                    if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
                } else {
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [button, item, pending, eventLifetime]() {
                        try {
                            button.PointerEntered(item.enter);
                            button.PointerExited(item.exit);
                            button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerPressedEvent(), item.press);
                            button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerReleasedEvent(), item.release);
                            button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerCanceledEvent(), item.release);
                            button.RemoveHandler(winrt::Windows::UI::Xaml::UIElement::PointerCaptureLostEvent(), item.release);
                        } catch (...) { Wh_Log(L"Exception revoking pointer events"); }
                        if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
                    });
                }
            } else {
                if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
            }
        } else {
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        }
    }
    std::vector<winrt::Windows::UI::Core::CoreDispatcher> allDispatchers;
    auto addDispatcher = [&](winrt::Windows::UI::Core::CoreDispatcher d) {
        if (!d) return;
        for (auto& existing : allDispatchers) {
            if (existing == d) return;
        }
        allDispatchers.push_back(d);
    };
    for (auto& ev : localEvents) addDispatcher(ev.dispatcher);
    for (auto& ctx : localPills) { if (auto p = ctx->pill.get()) addDispatcher(p.Dispatcher()); }
    for (auto& item : localPointerTokens) { if (auto b = item.btn.get()) addDispatcher(b.Dispatcher()); }

    for (auto& d : allDispatchers) {
        pending->fetch_add(1);
        d.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [pending, eventLifetime]() {
            DWORD tid = GetCurrentThreadId();
            {
                std::lock_guard<std::mutex> lock(g_easingMutex);
                g_easingCaches->erase(tid);
            }
            if (pending->fetch_sub(1) == 1 && eventLifetime.get()) SetEvent(eventLifetime.get());
        });
    }

    if (pending->load() > 0 && eventLifetime.get()) {
        if (WaitForSingleObject(eventLifetime.get(), 2000) == WAIT_TIMEOUT) {
            Wh_Log(L"Cleanup timed out. Mod may crash if pending callbacks execute.");
        }
    }
    Sleep(50); // Let layout settle
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Elastic Pill Mod");
    delete g_pillContexts;
    delete g_easingCaches;
    delete g_iconColorCache;
    delete g_attachedGroups;
    delete g_attachedEvents;
    delete g_iconColorExtracting;
    delete g_attachedPointerButtons;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    struct PillUpdateData {
        winrt::Windows::UI::Xaml::Shapes::Rectangle p{nullptr};
        winrt::Windows::UI::Xaml::FrameworkElement a{nullptr};
        winrt::Windows::UI::Xaml::Controls::Grid g{nullptr};
    };
    std::vector<PillUpdateData> updates;
    {
        std::lock_guard<std::mutex> lock(g_pillsMutex);
        for (auto& ctx : *g_pillContexts) {
            auto p = ctx->pill.get();
            auto a = ctx->activeBtn.get();
            auto g = ctx->grid.get();
            if (p && a && g) {
                updates.push_back({p, a, g});
            }
        }
    }
    for (auto& update : updates) {
        if (auto dispatcher = update.p.Dispatcher()) {
            auto p = update.p; auto g = update.g; auto a = update.a;
            dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [p, g, a]() {
                if (g_unloading) return;
                Settings localSettings;
                { std::lock_guard<std::mutex> lock(g_settingsMutex); localSettings = g_settings; }
                try { 
                    UpdatePillPosition(p, g, a, localSettings); 
                    EnsurePillAndPosition(a, true, localSettings); 
                } catch (...) {}
            });
        }
    }
}
