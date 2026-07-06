// ==WindhawkMod==
// @id              taskbar-elastic-pill
// @name            Taskbar Elastic WinUI Pill
// @description     Injects an animated sliding pill for active taskbar items.
// @version         1.0.0
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
  - Margins: "0, 0"
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
    - bounce: Bounce
    - linear: Linear
    - easein: Ease-In
    - easeout: Ease-Out
    - easeinout: Ease-In-Out
  - FadeTransition: false
    $name: Fade transition
    $description: Fade out and in during movement.
  - SquishTransition: false
    $name: Squish transition
    $description: Adds a squish effect to the pill while moving.
  $name: Animation Settings
- Colors:
  - ColorMode: accent
    $name: Pill color mode
    $options:
    - accent: System accent
    - custom: Custom hex
    - icon: App icon color
  - CustomColor: ""
    $name: Custom pill color
    $description: Hex color code, supports light/dark mode specific colors separated by comma.
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
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
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
    bool FadeTransition = false;
    bool SquishTransition = false;
    bool HideInactiveDots = false;
    int ColorMode = 0;
    bool TrackSystemButtons = false;
    std::optional<winrt::Windows::UI::Color> ParsedLightColor;
    std::optional<winrt::Windows::UI::Color> ParsedDarkColor;
    std::optional<winrt::Windows::UI::Color> ParsedSolidColor;
} g_settings;

std::mutex g_settingsMutex;
std::mutex g_pillsMutex;
struct PillContext {
    winrt::weak_ref<winrt::Windows::UI::Xaml::Shapes::Rectangle> pill;
    winrt::weak_ref<winrt::Windows::UI::Xaml::Controls::Grid> grid;
    void* gridAbi = nullptr;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> activeBtn;
    winrt::event_token layoutToken{};
    std::atomic<bool> searchAttached{false};
};
std::vector<std::shared_ptr<PillContext>> g_pillContexts;
std::atomic<bool> g_unloading{false};

std::atomic<bool> g_taskbarViewDllLoaded{false};
HMODULE g_taskbarViewModule = nullptr;

std::atomic<bool> g_searchUxDllLoaded{false};
HMODULE g_searchUxModule = nullptr;

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
    
    WindhawkUtils::StringSetting radiusStr(Wh_GetStringSetting(L"Pill.PillRadius"));
    if (radiusStr.get()[0]) {
        try {
            g_settings.PillRadius = std::stod(radiusStr.get());
        } catch (...) { g_settings.PillRadius = 1.5; }
    }

    WindhawkUtils::StringSetting animStr(Wh_GetStringSetting(L"Animation.AnimationStyle"));
    g_settings.AnimationStyle = 0;
    if (animStr.get()[0]) {
        if (wcscmp(animStr.get(), L"bounce") == 0) g_settings.AnimationStyle = 1;
        else if (wcscmp(animStr.get(), L"linear") == 0) g_settings.AnimationStyle = 2;
        else if (wcscmp(animStr.get(), L"easein") == 0) g_settings.AnimationStyle = 4;
        else if (wcscmp(animStr.get(), L"easeout") == 0) g_settings.AnimationStyle = 5;
        else if (wcscmp(animStr.get(), L"easeinout") == 0) g_settings.AnimationStyle = 6;
    }
    g_settings.FadeTransition = Wh_GetIntSetting(L"Animation.FadeTransition") != 0;
    g_settings.SquishTransition = Wh_GetIntSetting(L"Animation.SquishTransition") != 0;
    g_settings.HideInactiveDots = Wh_GetIntSetting(L"Pill.HideInactiveDots") != 0;
    g_settings.TrackSystemButtons = Wh_GetIntSetting(L"Pill.TrackSystemButtons") != 0;

    WindhawkUtils::StringSetting cmStr(Wh_GetStringSetting(L"Colors.ColorMode"));
    g_settings.ColorMode = 0;
    if (cmStr.get()[0]) {
        if (wcscmp(cmStr.get(), L"custom") == 0) g_settings.ColorMode = 1;
        else if (wcscmp(cmStr.get(), L"icon") == 0) g_settings.ColorMode = 2;
    }

    g_settings.ParsedLightColor = std::nullopt;
    g_settings.ParsedDarkColor = std::nullopt;
    g_settings.ParsedSolidColor = std::nullopt;
    WindhawkUtils::StringSetting colorStr(Wh_GetStringSetting(L"Colors.CustomColor"));
    if (colorStr.get()[0]) {
        std::wstring colorStrWs = colorStr.get();
        
        size_t commaPos = colorStrWs.find(L',');
        if (commaPos != std::wstring::npos) {
            std::wstring lightCol = colorStrWs.substr(0, commaPos);
            std::wstring darkCol = colorStrWs.substr(commaPos + 1);
            
            auto trim = [](std::wstring& s) {
                size_t start = s.find_first_not_of(L" \t");
                if (start == std::wstring::npos) { s = L""; return; }
                s.erase(0, start);
                s.erase(s.find_last_not_of(L" \t") + 1);
            };
            trim(lightCol);
            trim(darkCol);
            
            g_settings.ParsedLightColor = ParseHexColor(lightCol);
            g_settings.ParsedDarkColor = ParseHexColor(darkCol);
        } else {
            g_settings.ParsedSolidColor = ParseHexColor(colorStrWs);
        }
    }
}

struct ExtractedColors {
    winrt::Windows::UI::Color lightMode;
    winrt::Windows::UI::Color darkMode;
    uint64_t lastAccess = 0;
};

std::mutex g_iconColorMutex;
std::map<std::wstring, ExtractedColors> g_iconColorCache;
std::map<std::wstring, bool> g_iconColorExtracting;
std::atomic<uint64_t> g_colorAccessCounter{0};
std::atomic<int> g_asyncExtractCount{0};

std::vector<winrt::weak_ref<winrt::Windows::UI::Xaml::VisualStateGroup>> g_attachedGroups;
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

winrt::Windows::UI::Color GetPillColor(const Settings& localSettings, const std::wstring& buttonAbi) {
    bool isLight = (winrt::Windows::UI::Xaml::Application::Current().RequestedTheme() == winrt::Windows::UI::Xaml::ApplicationTheme::Light);

    if (localSettings.ColorMode == 2 && !buttonAbi.empty()) {
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        auto it = g_iconColorCache.find(buttonAbi);
        if (it != g_iconColorCache.end()) {
            it->second.lastAccess = ++g_colorAccessCounter;
            return isLight ? it->second.lightMode : it->second.darkMode;
        }
    }
    
    if (localSettings.ColorMode == 1 && (localSettings.ParsedLightColor.has_value() || localSettings.ParsedDarkColor.has_value() || localSettings.ParsedSolidColor.has_value())) {
        if (localSettings.ParsedLightColor.has_value() || localSettings.ParsedDarkColor.has_value()) {
            auto c = isLight ? localSettings.ParsedLightColor : localSettings.ParsedDarkColor;
            if (c.has_value()) return c.value();
        } else if (localSettings.ParsedSolidColor.has_value()) {
            return localSettings.ParsedSolidColor.value();
        }
    }
    
    auto res = winrt::Windows::UI::Xaml::Application::Current().Resources();
    auto resName = isLight ? L"SystemAccentColorDark1" : L"SystemAccentColorLight2";
    if (res.HasKey(winrt::box_value(resName))) {
        return winrt::unbox_value<winrt::Windows::UI::Color>(res.Lookup(winrt::box_value(resName)));
    }
    return {255, 0, 120, 212}; 
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
        void* iunknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown iunknown;
        winrt::copy_from_abi(iunknown, iunknownPtr);
        return iunknown.as<winrt::Windows::UI::Xaml::FrameworkElement>();
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

FrameworkElement FindChildByName(FrameworkElement const& parent, std::wstring_view name) {
    if (!parent) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    std::vector<FrameworkElement> children;
    children.reserve(count);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (child.Name() == name) return child;
            children.push_back(child);
        }
    }
    for (auto const& child : children) {
        auto result = FindChildByName(child, name);
        if (result) return result;
    }
    return nullptr;
}

FrameworkElement FindChildByClassName(FrameworkElement const& parent, std::wstring_view className) {
    if (!parent) return nullptr;
    int count = VisualTreeHelper::GetChildrenCount(parent);
    std::vector<FrameworkElement> children;
    children.reserve(count);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        if (child) {
            if (winrt::get_class_name(child) == className) return child;
            children.push_back(child);
        }
    }
    for (auto const& child : children) {
        auto result = FindChildByClassName(child, className);
        if (result) return result;
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

void ExtractIconColor(winrt::Windows::UI::Xaml::FrameworkElement button, winrt::Windows::UI::Xaml::Controls::Image icon, winrt::Windows::UI::Xaml::Shapes::Rectangle pill, std::wstring stableKey) {
    g_asyncExtractCount++;
    auto extractToken = std::shared_ptr<void>(nullptr, [](void*){ g_asyncExtractCount--; });
    auto weakButton = winrt::make_weak(button);
    auto weakPill = winrt::make_weak(pill);

    try {
        auto rtb = winrt::Windows::UI::Xaml::Media::Imaging::RenderTargetBitmap();
        auto renderOp = rtb.RenderAsync(icon);
        renderOp.Completed([weakButton, weakPill, rtb, stableKey, extractToken](auto&& op, auto&& status) {
            if (g_unloading) return;
            auto button = weakButton.get();
            auto pill = weakPill.get();
            if (!button || !pill) {
                std::lock_guard<std::mutex> lock(g_iconColorMutex);
                g_iconColorExtracting.erase(stableKey);
                return;
            }
            if (status != winrt::Windows::Foundation::AsyncStatus::Completed) {
                Wh_Log(L"ExtractIconColor: renderOp failed, status: %d", (int)status);
                std::lock_guard<std::mutex> lock(g_iconColorMutex);
                g_iconColorExtracting.erase(stableKey);
                return;
            }
            
            try {
                auto pixelOp = rtb.GetPixelsAsync();
                pixelOp.Completed([weakButton, weakPill, rtb, stableKey, extractToken](auto&& op2, auto&& status2) {
                    if (g_unloading) return;
                    auto button = weakButton.get();
                    auto pill = weakPill.get();
                    if (!button || !pill) {
                        std::lock_guard<std::mutex> lock(g_iconColorMutex);
                        g_iconColorExtracting.erase(stableKey);
                        return;
                    }
                    if (status2 != winrt::Windows::Foundation::AsyncStatus::Completed) {
                        Wh_Log(L"ExtractIconColor: pixelOp failed, status: %d", (int)status2);
                        std::lock_guard<std::mutex> lock(g_iconColorMutex);
                        g_iconColorExtracting.erase(stableKey);
                        return;
                    }
                    try {
                        auto pixels = op2.GetResults();
                        if (!pixels) {
                            Wh_Log(L"ExtractIconColor: pixels is null");
                            std::lock_guard<std::mutex> lock(g_iconColorMutex);
                            g_iconColorExtracting.erase(stableKey);
                            return;
                        }

                        IUnknown* pUnk = (IUnknown*)winrt::get_abi(pixels);
                        winrt::com_ptr<IBufferByteAccess_Local> buffer;
                        if (pUnk) {
                            pUnk->QueryInterface(IID_IBufferByteAccess_Local, buffer.put_void());
                        }
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
                                        if (g_iconColorCache.size() > 100) {
                                            std::vector<std::pair<std::wstring, uint64_t>> entries;
                                            for (auto& kv : g_iconColorCache) entries.push_back({kv.first, kv.second.lastAccess});
                                            size_t evictCount = std::min<size_t>(10, entries.size());
                                            std::nth_element(entries.begin(), entries.begin() + evictCount, entries.end(), [](auto& a, auto& b) { return a.second < b.second; });
                                            for (size_t i = 0; i < evictCount; i++) {
                                                g_iconColorCache.erase(entries[i].first);
                                                g_iconColorExtracting.erase(entries[i].first);
                                            }
                                        }
                                        variants.lastAccess = ++g_colorAccessCounter;
                                        g_iconColorCache[stableKey] = variants;
                                    }
                                    
                                    if (auto dispatcher = button.Dispatcher()) {
                                        dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [weakPill, weakButton, stableKey, extractToken]() {
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
                                                    auto brush = winrt::Windows::UI::Xaml::Media::SolidColorBrush(GetPillColor(currentSettings, stableKey));
                                                    pill.Fill(brush);
                                                }
                                            } catch(...) {}
                                        });
                                    }
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
                        g_iconColorExtracting.erase(stableKey);
                    }
                    } catch(...) {
                        Wh_Log(L"ExtractIconColor: exception in pixelOp.Completed");
                        std::lock_guard<std::mutex> lock(g_iconColorMutex);
                        g_iconColorExtracting.erase(stableKey);
                    }
                });
            } catch(...) {
                Wh_Log(L"ExtractIconColor: exception calling rtb.GetPixelsAsync");
                std::lock_guard<std::mutex> lock(g_iconColorMutex);
                g_iconColorExtracting.erase(stableKey);
            }
        });
    } catch(...) {
        Wh_Log(L"ExtractIconColor: exception in RenderAsync");
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        g_iconColorExtracting.erase(stableKey);
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
    float targetX = std::round((float)(point.X + (b.ActualWidth() / 2.0) - (pillW / 2.0)));
    
    auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(p);
    float lastTargetX = std::numeric_limits<float>::quiet_NaN();
    visual.Properties().TryGetScalar(L"LastTargetX", lastTargetX);

    if (std::isnan(lastTargetX) || std::abs(lastTargetX - targetX) > 0.1f) {
        visual.Properties().InsertScalar(L"LastTargetX", targetX);

        if (std::isnan(lastTargetX)) {
            visual.Offset(winrt::Windows::Foundation::Numerics::float3(targetX, visual.Offset().y, 0));
            visual.Properties().InsertScalar(L"LeftX", targetX);
            visual.Properties().InsertScalar(L"RightX", targetX + (float)pillW);
            visual.StopAnimation(L"Scale");
            visual.Scale(winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
        } else {
            auto compositor = visual.Compositor();
            DWORD tid = GetCurrentThreadId();
            EasingCache cache;
            {
                std::lock_guard<std::mutex> lock(g_easingMutex);
                cache = (*g_easingCaches)[tid];
            }

            if (cache.compositor != compositor) {
                cache.compositor = compositor;
                cache.stretchLeadEase = compositor.CreateCubicBezierEasingFunction({0.0f, 0.0f}, {0.0f, 1.0f});
                cache.stretchTrailEase = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {0.2f, 1.0f});
                cache.squishEase = compositor.CreateCubicBezierEasingFunction({0.25f, 0.1f}, {0.25f, 1.0f});
                cache.linearEase = compositor.CreateLinearEasingFunction();
                cache.easeIn = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {1.0f, 1.0f});
                cache.easeOut = compositor.CreateCubicBezierEasingFunction({0.0f, 0.0f}, {0.5f, 1.0f});
                cache.easeInOut = compositor.CreateCubicBezierEasingFunction({0.5f, 0.0f}, {0.5f, 1.0f});
                
                std::lock_guard<std::mutex> lock(g_easingMutex);
                (*g_easingCaches)[tid] = cache;
            }

            if (localSettings.FadeTransition) {
                auto opacityAnim = compositor.CreateScalarKeyFrameAnimation();
                opacityAnim.InsertKeyFrame(0.0f, 1.0f);
                opacityAnim.InsertKeyFrame(0.5f, 0.0f);
                opacityAnim.InsertKeyFrame(1.0f, 1.0f);
                opacityAnim.Duration(std::chrono::milliseconds(300));
                visual.Properties().StartAnimation(L"FadeOpacity", opacityAnim);
            } else {
                visual.Properties().InsertScalar(L"FadeOpacity", 1.0f);
            }

            int animStyle = localSettings.AnimationStyle;
            bool useSquish = localSettings.SquishTransition;

            // 1. Handle Horizontal Movement
            if (animStyle == 1) { // Bounce
                auto anim = compositor.CreateSpringScalarAnimation();
                anim.Target(L"Offset.X");
                anim.FinalValue(targetX);
                anim.DampingRatio(0.6f);
                anim.Period(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(50)));
                visual.StartAnimation(L"Offset.X", anim);
            } else if (animStyle == 2 || (animStyle >= 4 && animStyle <= 6)) { // KeyFrame Easing
                auto anim = compositor.CreateScalarKeyFrameAnimation();
                CompositionEasingFunction easing = nullptr;
                if (animStyle == 2) easing = cache.linearEase;
                else if (animStyle == 4) easing = cache.easeIn;
                else if (animStyle == 5) easing = cache.easeOut;
                else if (animStyle == 6) easing = cache.easeInOut;

                anim.InsertKeyFrame(1.0f, targetX, easing);
                anim.Duration(std::chrono::milliseconds(200));
                visual.StartAnimation(L"Offset.X", anim);
            } else { // Stretch (animStyle == 0)
                float targetLeft = targetX;
                float targetRight = targetX + (float)pillW;
                float currentLeft = lastTargetX;
                bool movingRight = targetLeft > currentLeft;

                auto propSet = visual.Properties();

                auto leftAnim = compositor.CreateScalarKeyFrameAnimation();
                leftAnim.InsertKeyFrame(1.0f, targetLeft, movingRight ? cache.stretchTrailEase : cache.stretchLeadEase);
                leftAnim.Duration(std::chrono::milliseconds(300));

                auto rightAnim = compositor.CreateScalarKeyFrameAnimation();
                rightAnim.InsertKeyFrame(1.0f, targetRight, movingRight ? cache.stretchLeadEase : cache.stretchTrailEase);
                rightAnim.Duration(std::chrono::milliseconds(300));

                propSet.StartAnimation(L"LeftX", leftAnim);
                propSet.StartAnimation(L"RightX", rightAnim);

                auto offsetExp = compositor.CreateExpressionAnimation(L"props.LeftX");
                offsetExp.SetReferenceParameter(L"props", propSet);
                visual.StartAnimation(L"Offset.X", offsetExp);
            }

            // 2. Handle Scaling & Squish
            if (animStyle == 0) { // Stretch uses expression scaling
                auto propSet = visual.Properties();
                if (useSquish) {
                    auto squishAnim = compositor.CreateScalarKeyFrameAnimation();
                    squishAnim.InsertKeyFrame(0.0f, 1.0f);
                    squishAnim.InsertKeyFrame(0.5f, 0.5f, cache.squishEase);
                    squishAnim.InsertKeyFrame(1.0f, 1.0f, cache.squishEase);
                    squishAnim.Duration(std::chrono::milliseconds(300));
                    propSet.InsertScalar(L"SquishY", 1.0f);
                    propSet.StartAnimation(L"SquishY", squishAnim);
                    
                    auto scaleExp = compositor.CreateExpressionAnimation(L"Vector3((props.RightX - props.LeftX) / props.LayoutW, props.SquishY, 1.0)");
                    scaleExp.SetReferenceParameter(L"props", propSet);
                    visual.CenterPoint(winrt::Windows::Foundation::Numerics::float3(0, (float)localSettings.PillHeight / 2.0f, 0));
                    visual.StartAnimation(L"Scale", scaleExp);
                } else {
                    auto scaleExp = compositor.CreateExpressionAnimation(L"Vector3((props.RightX - props.LeftX) / props.LayoutW, 1.0, 1.0)");
                    scaleExp.SetReferenceParameter(L"props", propSet);
                    visual.CenterPoint(winrt::Windows::Foundation::Numerics::float3(0, 0, 0));
                    visual.StartAnimation(L"Scale", scaleExp);
                }
            } else { // Standard rigid modes (Bounce, Linear, Ease)
                if (useSquish) {
                    auto scaleAnim = compositor.CreateVector3KeyFrameAnimation();
                    scaleAnim.InsertKeyFrame(0.0f, winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
                    scaleAnim.InsertKeyFrame(0.5f, winrt::Windows::Foundation::Numerics::float3(1.5f, 0.5f, 1.0f), cache.squishEase);
                    scaleAnim.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f), cache.squishEase);
                    scaleAnim.Duration(std::chrono::milliseconds(300));
                    visual.CenterPoint(winrt::Windows::Foundation::Numerics::float3((float)pillW / 2.0f, (float)localSettings.PillHeight / 2.0f, 0));
                    visual.StartAnimation(L"Scale", scaleAnim);
                } else {
                    visual.StopAnimation(L"Scale");
                    visual.Scale(winrt::Windows::Foundation::Numerics::float3(1.0f, 1.0f, 1.0f));
                }
            }
        }
    }

    float lastOpacityTarget = -1.0f;
    visual.Properties().TryGetScalar(L"LastShowOpacityTarget", lastOpacityTarget);
    if (lastOpacityTarget != 1.0f) {
        visual.Properties().InsertScalar(L"LastShowOpacityTarget", 1.0f);
        auto fadeAnim = visual.Compositor().CreateScalarKeyFrameAnimation();
        fadeAnim.InsertKeyFrame(1.0f, 1.0f);
        fadeAnim.Duration(std::chrono::milliseconds(150));
        visual.Properties().StartAnimation(L"ShowOpacity", fadeAnim);
    }

    return true;
}

void EnsurePillAndPosition(winrt::Windows::UI::Xaml::FrameworkElement const& button, bool isActive, const Settings& localSettings);

void AttachStateChangedHandler(winrt::Windows::UI::Xaml::VisualStateGroup const& group, winrt::Windows::UI::Xaml::FrameworkElement const& button) {
    if (!group) return;
    void* pGroup = winrt::get_abi(group);
    
    {
        std::lock_guard<std::mutex> lock(g_attachedGroupsMutex);
        bool found = false;
        for (auto it = g_attachedGroups.begin(); it != g_attachedGroups.end(); ) {
            if (auto g = it->get()) {
                if (winrt::get_abi(g) == pGroup) { found = true; break; }
                ++it;
            } else {
                it = g_attachedGroups.erase(it);
            }
        }
        if (found) return;
        g_attachedGroups.push_back(winrt::make_weak(group));
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
    
    if (!pill || !pill.Tag()) {
        if (!pill) {
            pill = winrt::Windows::UI::Xaml::Shapes::Rectangle();
            pill.Name(L"ElasticPill");
            pill.IsHitTestVisible(false);
            pill.HorizontalAlignment(HorizontalAlignment::Left);
            pill.VerticalAlignment(VerticalAlignment::Bottom);
            Canvas::SetZIndex(pill, 999);
            grid.Children().Append(pill);
            visual = ElementCompositionPreview::GetElementVisual(pill);
        }
        if (!visual) return;
        visual.Properties().InsertScalar(L"LeftX", 0.0f);
        visual.Properties().InsertScalar(L"RightX", (float)localSettings.PillWidth);
        visual.Properties().InsertScalar(L"ShowOpacity", 0.0f);
        visual.Properties().InsertScalar(L"FadeOpacity", 1.0f);

        auto opacityExp = visual.Compositor().CreateExpressionAnimation(L"props.ShowOpacity * props.FadeOpacity");
        opacityExp.SetReferenceParameter(L"props", visual.Properties());
        visual.StartAnimation(L"Opacity", opacityExp);

        pill.Tag(winrt::box_value(0.0f));
    }

    if (!visual) return;
    
    // Ensure visual properties are up-to-date with settings
    Settings settingsToUse = localSettings;
    settingsToUse.PillHeight = (std::max)(1, settingsToUse.PillHeight);
    settingsToUse.PillWidth = (std::max)(1, settingsToUse.PillWidth);
    
    float lastW = -1.0f, lastH = -1.0f, lastR = -1.0f, lastMB = -1.0f, lastMH = -1.0f;
    visual.Properties().TryGetScalar(L"LayoutW", lastW);
    visual.Properties().TryGetScalar(L"LayoutH", lastH);
    visual.Properties().TryGetScalar(L"LayoutR", lastR);
    visual.Properties().TryGetScalar(L"LayoutMB", lastMB);
    visual.Properties().TryGetScalar(L"LayoutMH", lastMH);

    if (std::abs(lastW - (float)settingsToUse.PillWidth) > 0.001f || 
        std::abs(lastH - (float)settingsToUse.PillHeight) > 0.001f || 
        std::abs(lastR - (float)settingsToUse.PillRadius) > 0.001f || 
        std::abs(lastMB - (float)settingsToUse.PillMarginBottom) > 0.001f || 
        std::abs(lastMH - (float)settingsToUse.PillMarginHorizontal) > 0.001f) {
        
        visual.Properties().InsertScalar(L"LayoutW", (float)settingsToUse.PillWidth);
        visual.Properties().InsertScalar(L"LayoutH", (float)settingsToUse.PillHeight);
        visual.Properties().InsertScalar(L"LayoutR", (float)settingsToUse.PillRadius);
        visual.Properties().InsertScalar(L"LayoutMB", (float)settingsToUse.PillMarginBottom);
        visual.Properties().InsertScalar(L"LayoutMH", (float)settingsToUse.PillMarginHorizontal);
        
        pill.Height(settingsToUse.PillHeight);
        pill.Width(settingsToUse.PillWidth);
        pill.RadiusX(settingsToUse.PillRadius);
        pill.RadiusY(settingsToUse.PillRadius);
        pill.Margin({(double)settingsToUse.PillMarginHorizontal, 0, (double)settingsToUse.PillMarginHorizontal, (double)settingsToUse.PillMarginBottom});
    }

    std::shared_ptr<PillContext> ctx = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_pillsMutex);
        if (g_pillContexts.size() > 10) {
            g_pillContexts.erase(
                std::remove_if(g_pillContexts.begin(), g_pillContexts.end(),
                    [](auto& c) { return c->grid.get() == nullptr; }),
                g_pillContexts.end());
        }

        void* targetAbi = winrt::get_abi(grid);
        auto it = std::find_if(g_pillContexts.begin(), g_pillContexts.end(), [&](const std::shared_ptr<PillContext>& c) {
            return c->gridAbi == targetAbi;
        });

        if (it != g_pillContexts.end()) {
            ctx = *it;
            ctx->pill = winrt::make_weak(pill);
        } else {
            ctx = std::make_shared<PillContext>();
            ctx->grid = winrt::make_weak(grid);
            ctx->gridAbi = targetAbi;
            ctx->pill = winrt::make_weak(pill);
            g_pillContexts.push_back(ctx);

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
        std::vector<FrameworkElement> candidateBtns;
        auto gatherBtns = [&](auto& self, FrameworkElement const& parent, int depth) -> void {
            if (!parent || depth > 3) return;
            int count = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(parent);
            for (int i = 0; i < count; i++) {
                auto child = winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
                if (child) {
                    winrt::hstring cName = winrt::get_class_name(child);
                    if (cName == L"Taskbar.TaskListButton" || 
                        (localSettings.TrackSystemButtons && (cName == L"Taskbar.ExperienceToggleButton" || cName == L"SearchUx.SearchUI.SearchButtonControl"))) {
                        candidateBtns.push_back(child);
                    } else {
                        self(self, child, depth + 1);
                    }
                }
            }
        };
        gatherBtns(gatherBtns, grid, 0);

        FrameworkElement systemActiveBtn = nullptr;

        for (auto& candidate : candidateBtns) {
            if (candidate.Visibility() != winrt::Windows::UI::Xaml::Visibility::Visible) continue;
            if (candidate.Opacity() <= 0.001f) continue;
            if (candidate.ActualWidth() <= 0.001f) continue;

            winrt::hstring cName = winrt::get_class_name(candidate);

            if (cName == L"Taskbar.TaskListButton") {
                auto sIconPanel = FindChildByName(candidate, L"IconPanel");
                auto sGroup = sIconPanel ? GetVisualStateGroup(sIconPanel, L"RunningIndicatorStates") : nullptr;
                auto sState = sGroup ? sGroup.CurrentState() : nullptr;
                if (sState && sState.Name() == L"ActiveRunningIndicator") {
                    anyActive = true;
                    targetButton = candidate;
                    break;
                }
            } else if (cName == L"Taskbar.ExperienceToggleButton") {
                auto sRootPanel = FindChildByName(candidate, L"ExperienceToggleButtonRootPanel");
                auto sGroup = sRootPanel ? GetVisualStateGroup(sRootPanel, L"CommonStates") : nullptr;
                auto sState = sGroup ? sGroup.CurrentState() : nullptr;
                if (sState && (sState.Name() == L"ActiveNormal" || sState.Name() == L"ActivePointerOver" || sState.Name() == L"ActivePressed")) {
                    if (!systemActiveBtn) systemActiveBtn = candidate;
                }
            } else if (cName == L"SearchUx.SearchUI.SearchButtonControl") {
                if (auto sIconButton = FindChildByClassName(candidate, L"SearchUx.SearchUI.SearchIconButton")) {
                    auto sRootGrid = FindChildByName(sIconButton, L"SearchBoxButtonRootPanel");
                    auto sGroup = sRootGrid ? GetVisualStateGroup(sRootGrid, L"CommonStates") : nullptr;
                    auto sState = sGroup ? sGroup.CurrentState() : nullptr;
                    if (sState && (sState.Name() == L"ActiveNormal" || sState.Name() == L"ActivePointerOver" || sState.Name() == L"ActivePressed")) {
                        if (!systemActiveBtn) systemActiveBtn = candidate;
                    }
                }
            }
        }

        if (!anyActive && systemActiveBtn) {
            anyActive = true;
            targetButton = systemActiveBtn;
        }
    }

    if (anyActive) {
        ctx->activeBtn = targetButton;
        try {
            UpdatePillPosition(pill, grid, targetButton, settingsToUse);
        } catch (...) {}
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
                    if (!g_iconColorCache.count(stableKey) && !g_iconColorExtracting[stableKey]) {
                        needsExtract = true;
                        g_iconColorExtracting[stableKey] = true;
                    }
                    isExtracting = g_iconColorExtracting[stableKey];
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
                            g_iconColorExtracting[stableKey] = false;
                        }
                    } else {
                        std::lock_guard<std::mutex> lock(g_iconColorMutex);
                        g_iconColorExtracting[stableKey] = false;
                    }
                }
            }
        }
    }

    auto brush = pill.Fill().try_as<winrt::Windows::UI::Xaml::Media::SolidColorBrush>();
    if (!brush) {
        brush = winrt::Windows::UI::Xaml::Media::SolidColorBrush(anyActive ? GetPillColor(settingsToUse, stableKey) : GetPillColor(settingsToUse, L""));
        pill.Fill(brush);
    } else {
        if (anyActive) {
            bool hasColor = true;
            if (settingsToUse.ColorMode == 2 && isExtracting) {
                std::lock_guard<std::mutex> lock(g_iconColorMutex);
                hasColor = g_iconColorCache.count(stableKey) > 0;
            }
            if (settingsToUse.ColorMode != 2 || !isExtracting || hasColor) {
                brush.Color(GetPillColor(settingsToUse, stableKey));
            }
        }
    }

    auto compositor = visual.Compositor();

    if (!anyActive) {
        float lastOpacityTarget = -1.0f;
        visual.Properties().TryGetScalar(L"LastShowOpacityTarget", lastOpacityTarget);
        if (lastOpacityTarget != 0.0f) {
            visual.Properties().InsertScalar(L"LastShowOpacityTarget", 0.0f);
            auto fadeAnim = compositor.CreateScalarKeyFrameAnimation();
            fadeAnim.InsertKeyFrame(1.0f, 0.0f);
            fadeAnim.Duration(std::chrono::milliseconds(150));
            visual.Properties().StartAnimation(L"ShowOpacity", fadeAnim);
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
    if (!pSender) return;

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
SearchIconButton_UpdateVisualStates_t SearchIconButton_UpdateVisualStates_Original;

void WINAPI SearchIconButton_UpdateVisualStates_Hook(void* pThis) {
    SearchIconButton_UpdateVisualStates_Original(pThis);
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

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE m = GetModuleHandle(L"Taskbar.View.dll");
    return m ? m : GetModuleHandle(L"ExplorerExtensions.dll");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarHooks[] = {
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
    
    if (!WindhawkUtils::HookSymbols(module, taskbarHooks, ARRAYSIZE(taskbarHooks))) {
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
    WindhawkUtils::SYMBOL_HOOK searchUxHooks[] = {
        {
            {LR"(private: void __cdecl winrt::SearchUx::SearchUI::implementation::SearchIconButton::UpdateVisualStates(void))"},
            &SearchIconButton_UpdateVisualStates_Original,
            SearchIconButton_UpdateVisualStates_Hook,
            true
        },
        {
            {L"protected: void __cdecl winrt::SearchUx::SearchUI::implementation::SearchIconButton::PlayStateChange(void)"},
            &SearchIconButton_UpdateVisualStates_Original,
            SearchIconButton_UpdateVisualStates_Hook,
            true
        }
    };

    WindhawkUtils::HookSymbols(module, searchUxHooks, ARRAYSIZE(searchUxHooks));
    if (!SearchIconButton_UpdateVisualStates_Original) {
        Wh_Log(L"Failed to hook SearchUx.UI.dll symbols entirely.");
        return false;
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module && !g_taskbarViewDllLoaded.exchange(true)) {
            g_taskbarViewModule = module;
            Wh_Log(L"Taskbar View DLL loaded: %s", lpLibFileName);
            if (HookTaskbarViewDllSymbols(module)) Wh_ApplyHookOperations();
        }
        if (!g_searchUxDllLoaded && GetSearchUxModuleHandle() == module && !g_searchUxDllLoaded.exchange(true)) {
            g_searchUxModule = module;
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
        g_taskbarViewModule = m;
        if (!HookTaskbarViewDllSymbols(m)) return FALSE;
    }
    
    HMODULE sm = GetSearchUxModuleHandle();
    if (sm) {
        g_searchUxDllLoaded = true;
        g_searchUxModule = sm;
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
        localPills = g_pillContexts;
        g_pillContexts.clear();
    }

    std::vector<AttachedEvent> localEvents;
    {
        std::lock_guard<std::mutex> lock(g_attachedEventsMutex);
        localEvents = *g_attachedEvents;
        g_attachedEvents->clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_attachedGroupsMutex);
        g_attachedGroups.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_iconColorMutex);
        g_iconColorCache.clear();
        g_iconColorExtracting.clear();
    }

    if (localPills.empty() && localEvents.empty()) return;

    std::shared_ptr<void> eventLifetime(CreateEvent(nullptr, TRUE, FALSE, nullptr), [](HANDLE h) { if(h) CloseHandle(h); });
    auto pending = std::make_shared<std::atomic<int>>((int)localPills.size() + (int)localEvents.size());

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
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [pill, grid, layoutToken, pending, eventLifetime]() {
                        try {
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

    if (pending->load() > 0 && eventLifetime.get()) {
        WaitForSingleObject(eventLifetime.get(), INFINITE);
    }
    Sleep(50); // Let layout settle
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Taskbar Elastic Pill Mod");
    {
        std::lock_guard<std::mutex> lock(g_easingMutex);
        g_easingCaches->clear();
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    std::lock_guard<std::mutex> lock(g_pillsMutex);
    std::vector<winrt::Windows::UI::Core::CoreDispatcher> dispatched;
    for (auto& ctx : g_pillContexts) {
        auto p = ctx->pill.get();
        auto a = ctx->activeBtn.get();
        auto g = ctx->grid.get();
        if (p && a && g) {
            if (auto dispatcher = p.Dispatcher()) {
                bool alreadyDispatched = false;
                for (auto& d : dispatched) {
                    if (d == dispatcher) { alreadyDispatched = true; break; }
                }
                if (!alreadyDispatched) {
                    dispatched.push_back(dispatcher);
                    dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low, [p, g, a]() {
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
    }
}
