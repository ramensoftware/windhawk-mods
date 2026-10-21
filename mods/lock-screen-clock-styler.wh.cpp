// ==WindhawkMod==
// @id              lock-screen-clock-styler
// @name            Lock Screen Clock Styler
// @name:tr-TR      Kilit Ekranı Saat Özelleştirici
// @description     Customize the Windows 11 lock screen clock with modern styles, date display, and widget controls.
// @description:tr-TR Windows 11 kilit ekranı saatini modern stiller, tarih göstergesi ve widget denetimleriyle özelleştirin.
// @version         0.4.0
// @author          ali0zdas
// @github          https://github.com/ali0zdas
// @license         MIT
// @include         LockApp.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Lock Screen Clock Styler

Customize and enhance the Windows 11 lock screen clock with modern designs, frosted glass panels, dynamic date capsules, and clean layout controls.

## Features:
- **Minimalist Sonoma (Clean Typography):** Apple-inspired lock screen typography with date positioned above the time, slender display numerals, dual-layer atmospheric shadow, and bottom-left or center placement.
- **Modern Flip Clock:** Split glass cards for hour and minute with an elegant horizontal flip seam.
- **Classic Flip Clock:** Classic mechanical Solari-inspired flip clock with matte charcoal flap cards, aged ivory cream typography, mechanical split seam, and hinge axle clips.
- **Cyberpunk Sci-Fi HUD Terminal:** Bespoke tactical Netrunner HUD terminal with dark obsidian glass chassis, tactical corner brackets in acid yellow and hot magenta, top telemetry diagnostic bar, electric cyan digits with rhythmic tactical pulse, and bottom telemetry date logger.
- **Retro Pixel / LED Dot Matrix Clock:** Authentic dot matrix display inspired by classic digital boards with active and ambient inactive LED dots.
- **Minimalist Aura / Floating Clock:** Frameless floating typography with ambient soft drop-shadow depth and an elegant frosted date pill.
- **Battery & Charging Capsule:** Live battery percentage and power indicator (⚡ %85) for laptop users, automatically hidden on desktop PCs.
- **12-Hour / 24-Hour Format:** Switch between 24-hour time and 12-hour time with dedicated AM/PM indicators across themes.
- **Show Seconds Toggle:** Optional live seconds display. Supported by every style except the two flip clocks.
- **Frosted Glass / Acrylic Styling:** Translucent glass panels with subtle border highlights and rounded corners.
- **Dynamic Date Display:** Elegant date and day capsule positioned beneath or above the clock, adapting to the user's system locale.
- **Hide Default Widgets:** Clean up the lock screen by hiding MSN weather, sports, and news widgets.
- **Hide / Replace Default Clock:** Seamlessly replace the stock Windows 11 clock with custom styles or adjust vertical offsets.
- **Extensive Customization:** Card opacity, font size, vertical offsets, and alignments are fully customizable via settings.
- **No XAML Diagnostics:** The mod walks the lock screen visual tree from its own UI thread instead of registering a XAML Diagnostics TAP. It can therefore run alongside other XAML-based Windhawk mods such as Windows 11 Start Menu Styler.

## Screenshots:
| Style | Preview |
| :---: | :---: |
| Minimalist Sonoma | ![Minimalist Sonoma](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/sonoma.png) |
| Retro Pixel / LED Matrix | ![Retro Pixel](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/Retro%20Pixel.png) |
| Modern Flip Clock | ![Modern Flip Clock](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/modern%20flip%20clock.png) |
| Classic Flip Clock | ![Classic Flip Clock](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/classical%20flip%20clock.png) |
| Cyberpunk Sci-Fi HUD | ![Cyberpunk Sci-Fi HUD](https://raw.githubusercontent.com/ali0zdas/windhawk-mod-lock-screen-clock-styler/main/screenshots/cyberpunk%20style.png) |

## Security Note:
This mod exclusively targets `LockApp.exe`. It does not inject into or interfere with `LogonUI.exe` or `winlogon.exe` in the Secure Desktop, preventing any system lockup or logon security risks.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- clockStyle: pixel
  $name: "Clock Style"
  $name:tr-TR: "Saat Tasarım Stili"
  $description: "The clock style to display on the lock screen."
  $description:tr-TR: "Kilit ekranında görüntülenecek saat stili."
  $options:
    - pixel: "Retro Pixel / LED Dot Matrix"
    - flip: "Modern Flip Clock"
    - vintage: "Classic Flip Clock"
    - macos: "Minimalist Sonoma (Clean Typography)"
    - cyberpunk: "Cyberpunk Sci-Fi HUD Terminal"
    - aura: "Minimalist Aura / Floating Clock"
    - defaultOnly: "Default Clock Only (No Custom Clock)"
- timeFormat: 24h
  $name: "Time Format"
  $name:tr-TR: "Saat Formatı"
  $description: "Select 24-hour format or 12-hour format with AM/PM indicator."
  $description:tr-TR: "24 saat formatı veya AM/PM göstergeli 12 saat formatı seçimi."
  $options:
    - 24h: "24-Hour (14:30)"
    - 12h: "12-Hour AM/PM (02:30 PM)"
- showSeconds: false
  $name: "Show Seconds"
  $name:tr-TR: "Saniyeyi Göster"
  $description: "Adds live seconds to the clock. Supported by every style except the Modern and Classic flip clocks."
  $description:tr-TR: "Saate canlı saniye ekler. Modern ve Klasik flip saatler dışındaki tüm stillerde desteklenir."
- glassOpacity: 45
  $name: "Glass Opacity (Flip Cards)"
  $name:tr-TR: "Cam Opaklığı (Flip Kartları)"
  $description: "Translucency of the Modern and Classic flip clock cards (0 to 255; 0 is completely transparent, 255 is fully opaque). Other styles are not affected."
  $description:tr-TR: "Modern ve Klasik flip saat kartlarının yarı saydamlığı (0 ile 255 arası; 0 tamamen saydam, 255 tam opak). Diğer stiller etkilenmez."
- clockFontSize: 92
  $name: "Digit Font Size"
  $name:tr-TR: "Rakam Font Boyutu"
  $description: "Base font size for the clock digits (20 to 300). Each style scales this value for its own layout."
  $description:tr-TR: "Saat rakamları için temel punto büyüklüğü (20 ile 300 arası). Her stil bu değeri kendi yerleşimine göre ölçekler."
- verticalOffset: 0
  $name: "Vertical Offset (px)"
  $name:tr-TR: "Dikey Konum Ofseti (px)"
  $description: "Shifts the clock vertically (-200 to +400). Out-of-range values are clamped."
  $description:tr-TR: "Saati dikey olarak kaydırır (-200 ile +400 arası). Aralık dışı değerler sınırlandırılır."
- clockAlignment: auto
  $name: "Clock Horizontal Alignment"
  $name:tr-TR: "Saat Yatay Hizalaması"
  $description: "Horizontal alignment and layout of the clock on screen. Automatic uses the theme default (bottom-left for Minimalist Sonoma, center for other styles)."
  $description:tr-TR: "Saatin ekrandaki yerleşim ve hizalaması. Otomatik seçeneği tema varsayılanını kullanır (Minimalist Sonoma için sol alt, diğer stiller için merkez)."
  $options:
    - auto: "Automatic (Theme Default)"
    - center: "Center of Screen"
    - bottomLeft: "Bottom-Left (Sonoma Style)"
    - left: "Left Side"
    - right: "Right Side"
- hideDefaultWidgets: false
  $name: "Hide Default Widgets"
  $name:tr-TR: "Varsayılan Widget'ları Gizle"
  $description: "Hides MSN weather, sports, and news widget areas on the lock screen. Disabled by default so the mod only restyles the clock unless asked otherwise."
  $description:tr-TR: "Kilit ekranındaki MSN hava durumu, spor ve haber widget alanını gizler. Mod varsayılan olarak yalnızca saati yeniden stillendirir; widget'ları gizlemek için bu seçeneği açın."
- hideDefaultClock: true
  $name: "Hide Default Lock Screen Clock"
  $name:tr-TR: "Varsayılan Kilit Ekranı Saatini Gizle"
  $description: "Hides the default Windows 11 clock and date text to make way for the new clock."
  $description:tr-TR: "Windows 11'in standart saat ve tarih metnini gizleyerek yeni saatin görünmesini sağlar."
- showDateCapsule: true
  $name: "Show Date Display"
  $name:tr-TR: "Tarih Göstergesini Göster"
  $description: "Displays a date and weekday display (above or below clock cards)."
  $description:tr-TR: "Tarih ve gün göstergesini görüntüler."
- showBatteryStatus: false
  $name: "Show Battery & Charging Capsule"
  $name:tr-TR: "Pil ve Şarj Kapsülünü Göster"
  $description: "Displays battery percentage and charging status on laptops (automatically hidden on desktop PCs)."
  $description:tr-TR: "Dizüstü bilgisayarlarda pil yüzdesi ve şarj durumu kapsülünü görüntüler (Masaüstü bilgisayarlarda otomatik gizlenir)."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#undef GetCurrentTime
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxd = winrt::Windows::UI::Xaml::Documents;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;
namespace wu = winrt::Windows::UI;

// Mod Configuration Settings
struct ModSettings {
    std::wstring clockStyle;
    std::wstring timeFormat;
    bool showSeconds;
    int glassOpacity;
    int clockFontSize;
    int verticalOffset;
    std::wstring clockAlignment;
    bool hideDefaultWidgets;
    bool hideDefaultClock;
    bool showDateCapsule;
    bool showBatteryStatus;
} g_settings;

// Loads every setting. Before the UI hook is installed this may run on any
// thread; afterwards it must run on the lock screen UI thread.
void LoadSettings() {
    auto style = WindhawkUtils::StringSetting::make(L"clockStyle");
    g_settings.clockStyle = (style.get() && *style.get()) ? style.get() : L"pixel";
    if (g_settings.clockStyle != L"pixel" && g_settings.clockStyle != L"flip" &&
        g_settings.clockStyle != L"vintage" && g_settings.clockStyle != L"macos" &&
        g_settings.clockStyle != L"cyberpunk" && g_settings.clockStyle != L"aura" &&
        g_settings.clockStyle != L"defaultOnly") {
        g_settings.clockStyle = L"pixel";
    }

    auto fmt = WindhawkUtils::StringSetting::make(L"timeFormat");
    g_settings.timeFormat = (fmt.get() && *fmt.get()) ? fmt.get() : L"24h";
    if (g_settings.timeFormat != L"24h" && g_settings.timeFormat != L"12h") {
        g_settings.timeFormat = L"24h";
    }

    // The settings UI is the single source of truth for seconds. The flip
    // clocks have no seconds display, so the toggle is ignored for them.
    bool supportsSeconds =
        (g_settings.clockStyle != L"flip" && g_settings.clockStyle != L"vintage");
    g_settings.showSeconds = supportsSeconds && Wh_GetIntSetting(L"showSeconds") != 0;

    g_settings.glassOpacity = Wh_GetIntSetting(L"glassOpacity");
    if (g_settings.glassOpacity < 0) g_settings.glassOpacity = 0;
    if (g_settings.glassOpacity > 255) g_settings.glassOpacity = 255;

    g_settings.clockFontSize = Wh_GetIntSetting(L"clockFontSize");
    if (g_settings.clockFontSize < 20) g_settings.clockFontSize = 20;
    if (g_settings.clockFontSize > 300) g_settings.clockFontSize = 300;

    g_settings.verticalOffset = Wh_GetIntSetting(L"verticalOffset");
    if (g_settings.verticalOffset < -200) g_settings.verticalOffset = -200;
    if (g_settings.verticalOffset > 400) g_settings.verticalOffset = 400;

    auto align = WindhawkUtils::StringSetting::make(L"clockAlignment");
    std::wstring rawAlign = (align.get() && *align.get()) ? align.get() : L"auto";
    if (rawAlign != L"auto" && rawAlign != L"center" && rawAlign != L"bottomLeft" &&
        rawAlign != L"left" && rawAlign != L"right") {
        rawAlign = L"auto";
    }

    // "auto" maps to the theme default; an explicit choice is always honored.
    g_settings.clockAlignment = (rawAlign == L"auto")
        ? (g_settings.clockStyle == L"macos" ? L"bottomLeft" : L"center")
        : rawAlign;

    g_settings.hideDefaultWidgets = Wh_GetIntSetting(L"hideDefaultWidgets") != 0;
    g_settings.hideDefaultClock = Wh_GetIntSetting(L"hideDefaultClock") != 0;
    g_settings.showDateCapsule = Wh_GetIntSetting(L"showDateCapsule") != 0;
    g_settings.showBatteryStatus = Wh_GetIntSetting(L"showBatteryStatus") != 0;

    Wh_Log(L"Settings loaded: style=%s, fmt=%s, showSec=%d, opacity=%d, font=%d, vertOffset=%d, align=%s, hideWidgets=%d, hideClock=%d, showBattery=%d",
           g_settings.clockStyle.c_str(), g_settings.timeFormat.c_str(), g_settings.showSeconds, g_settings.glassOpacity,
           g_settings.clockFontSize, g_settings.verticalOffset, g_settings.clockAlignment.c_str(), g_settings.hideDefaultWidgets, g_settings.hideDefaultClock, g_settings.showBatteryStatus);
}

// Strong XAML references. These belong to the lock screen UI thread and must
// never be released by CRT static destructors at process exit. They are marked
// no_destroy and released manually from the UI thread in TeardownOnUiThread().
[[clang::no_destroy]] wuxc::Grid g_lockRootGrid{ nullptr };
[[clang::no_destroy]] wux::UIElement g_activeClockElement{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_hourTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_hourBotTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_minTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_minBotTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_secTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_ampmTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_auraTimeTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_auraShadowTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_dateTextBlock{ nullptr };

// Minimalist Sonoma (Clean Typography) Theme State
[[clang::no_destroy]] wuxc::TextBlock g_macosTimeTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_macosShadowTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_macosDateTextBlock{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_macosDateShadowBlock{ nullptr };

// Battery Capsule State
[[clang::no_destroy]] wuxc::Border g_batteryCapsule{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_batteryIconText{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_batteryStatusText{ nullptr };

// Cyberpunk HUD Pulsing State
[[clang::no_destroy]] wuxc::TextBlock g_cyberSlash1{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_cyberSlash2{ nullptr };
[[clang::no_destroy]] wuxc::TextBlock g_cyberLiveLabel{ nullptr };

// Elements whose Visibility this mod changed. The original local value is kept
// so it can be restored without overwriting Windows' own bindings/state.
struct TrackedElement {
    wux::FrameworkElement element{ nullptr };
    winrt::Windows::Foundation::IInspectable originalValue{ nullptr };
    bool hadLocalValue = false;
};

// no_destroy: releasing these vectors in a CRT destructor at process exit would
// release XAML objects off the UI thread. They are cleared on the UI thread.
[[clang::no_destroy]] std::vector<TrackedElement> g_hiddenClockElements;
[[clang::no_destroy]] std::vector<TrackedElement> g_hiddenWidgetElements;

// Dirty-checking time cache for high-efficiency rendering (eliminates redundant DirectWrite invalidations)
struct TimeCache {
    int lastDisplayHour = -1;
    int lastMinute = -1;
    int lastSecond = -1;
    int lastDay = -1;
    int lastMonth = -1;
    int lastYear = -1;
    std::wstring lastAmPm;
    std::wstring lastAuraTime;
    std::wstring lastDateStr;
    std::wstring lastMacosTime;
    std::wstring lastMacosDate;
    int lastBatteryPct = -1;
    bool lastIsCharging = false;

    void Invalidate() {
        lastDisplayHour = -1;
        lastMinute = -1;
        lastSecond = -1;
        lastDay = -1;
        lastMonth = -1;
        lastYear = -1;
        lastAmPm.clear();
        lastAuraTime.clear();
        lastDateStr.clear();
        lastMacosTime.clear();
        lastMacosDate.clear();
        lastBatteryPct = -1;
        lastIsCharging = false;
    }
} g_timeCache;

// 7x11 Dot Matrix Font Table for Digits 0 to 9 with 2-Dot Stroke Thickness
// Each digit consists of 11 rows; each row uses the lower 7 bits (bit 6 = col 0, bit 0 = col 6).
static const uint8_t FONT_7x11[10][11] = {
    // 0
    { 0b0111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111110 },
    // 1
    { 0b0011100,
      0b0111100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0001100,
      0b0111110,
      0b0111110 },
    // 2
    { 0b0111110,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b0111111,
      0b1111110,
      0b1100000,
      0b1100000,
      0b1111111,
      0b1111111 },
    // 3
    { 0b0111110,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b0011110,
      0b0000011,
      0b0000011,
      0b0000011,
      0b1111111,
      0b0111110 },
    // 4
    { 0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b0000011 },
    // 5
    { 0b1111111,
      0b1111111,
      0b1100000,
      0b1100000,
      0b1111110,
      0b0111111,
      0b0000011,
      0b0000011,
      0b0000011,
      0b1111111,
      0b0111110 },
    // 6
    { 0b0111110,
      0b1111111,
      0b1100000,
      0b1100000,
      0b1111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111110 },
    // 7
    { 0b1111111,
      0b1111111,
      0b0000011,
      0b0000011,
      0b0000110,
      0b0001100,
      0b0011000,
      0b0011000,
      0b0110000,
      0b0110000,
      0b0110000 },
    // 8
    { 0b0111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b0111110,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111110 },
    // 9
    { 0b0111110,
      0b1111111,
      0b1100011,
      0b1100011,
      0b1100011,
      0b1111111,
      0b0111111,
      0b0000011,
      0b0000011,
      0b1111111,
      0b0111110 }
};

// Pixel Clock State
struct PixelDigitGrid {
    std::vector<wuxs::Ellipse> dots; // 77 dots per digit (7 cols x 11 rows)
    int lastVal = -1; // Cached last rendered digit to avoid redundant updates
};
// no_destroy: the six dot vectors hold XAML Ellipse references and must only be
// cleared from the UI thread (see ResetClockElementReferences()).
[[clang::no_destroy]] PixelDigitGrid g_pixelDigits[6]; // H1, H2, M1, M2, S1, S2
[[clang::no_destroy]] wuxm::SolidColorBrush g_brushActiveDot{ nullptr };
[[clang::no_destroy]] wuxm::SolidColorBrush g_brushInactiveDot{ nullptr };

// Win32 Battery Status Helper
struct BatteryInfo {
    bool hasBattery = false;
    bool isCharging = false;
    int percentage = -1;
};

BatteryInfo GetCurrentBatteryInfo() {
    BatteryInfo info;
    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps)) {
        // BatteryFlag 128 means "no system battery"; the unknown value (255)
        // also has that bit set. BatteryFlag 8 means "charging".
        if ((sps.BatteryFlag & 128) == 0 && sps.BatteryLifePercent <= 100) {
            info.hasBattery = true;
            info.percentage = (int)sps.BatteryLifePercent;
            info.isCharging = (sps.BatteryFlag & 8) != 0;
        }
    }
    return info;
}

// Build Universal Frosted Battery & Power Status Capsule
wux::UIElement CreateBatteryCapsule(bool alignLeft, bool isDarkTheme = false) {
    auto batteryInfo = GetCurrentBatteryInfo();
    if (!batteryInfo.hasBattery) {
        return nullptr; // Desktop PC without battery: do not render
    }

    wuxc::Border capsule;
    capsule.Name(L"WindhawkBatteryCapsule");
    if (isDarkTheme) {
        capsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(210, 14, 14, 14)));
        capsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(40, 255, 255, 255)));
    } else {
        capsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(32, 255, 255, 255)));
        capsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(55, 255, 255, 255)));
    }
    capsule.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
    capsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(10.0));
    capsule.Padding(wux::Thickness{ 12.0, 4.0, 14.0, 4.0 });
    capsule.Margin(wux::Thickness{ alignLeft ? 6.0 : 0.0, 12.0, 0.0, 0.0 });
    capsule.HorizontalAlignment(alignLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);

    wuxc::StackPanel panel;
    panel.Orientation(wuxc::Orientation::Horizontal);
    panel.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock iconText;
    iconText.Name(L"WindhawkBatteryIconText");
    iconText.Text(batteryInfo.isCharging ? L"⚡ " : L"🔋 ");
    iconText.FontSize(12.5);
    iconText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    iconText.Foreground(batteryInfo.isCharging 
        ? wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 252, 238, 10)) // Gold / Yellow
        : wuxm::SolidColorBrush(wu::Colors::White()));
    iconText.VerticalAlignment(wux::VerticalAlignment::Center);
    panel.Children().Append(iconText);

    wuxc::TextBlock statusText;
    statusText.Name(L"WindhawkBatteryStatusText");
    WCHAR buf[32];
    swprintf_s(buf, L"%d%%", batteryInfo.percentage);
    statusText.Text(buf);
    statusText.FontSize(12.5);
    statusText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
    statusText.FontWeight(winrt::Windows::UI::Text::FontWeights::Medium());
    statusText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(235, 255, 255, 255)));
    statusText.VerticalAlignment(wux::VerticalAlignment::Center);
    panel.Children().Append(statusText);

    capsule.Child(panel);
    g_batteryCapsule = capsule;
    g_batteryIconText = iconText;
    g_batteryStatusText = statusText;
    return capsule;
}

// Helper to apply root horizontal and vertical alignments (supports bottomLeft Sonoma style)
void ApplyAlignmentAndMargin(wuxc::Grid& rootContainer) {
    if (g_settings.clockAlignment == L"left") {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Left);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Center);
        rootContainer.Margin(wux::Thickness{ 80.0, (double)g_settings.verticalOffset, 0.0, 0.0 });
    } else if (g_settings.clockAlignment == L"right") {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Right);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Center);
        rootContainer.Margin(wux::Thickness{ 0.0, (double)g_settings.verticalOffset, 80.0, 0.0 });
    } else if (g_settings.clockAlignment == L"bottomLeft") {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Left);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Bottom);
        rootContainer.Margin(wux::Thickness{ 80.0, 0.0, 0.0, 80.0 + (double)g_settings.verticalOffset });
    } else {
        rootContainer.HorizontalAlignment(wux::HorizontalAlignment::Center);
        rootContainer.VerticalAlignment(wux::VerticalAlignment::Center);
        rootContainer.Margin(wux::Thickness{ 0.0, (double)g_settings.verticalOffset, 0.0, 0.0 });
    }
}

// Update single pixel digit dots (7x11 matrix) with dirty-checking
void SetPixelDigitValue(PixelDigitGrid& digitRef, int val) {
    if (val < 0 || val > 9 || digitRef.dots.size() != 77 || !g_brushActiveDot || !g_brushInactiveDot) {
        return;
    }
    if (digitRef.lastVal == val) {
        return; // Value unchanged; skip re-applying brushes to 77 Ellipses
    }
    digitRef.lastVal = val;

    const uint8_t* rows = FONT_7x11[val];
    for (int r = 0; r < 11; r++) {
        uint8_t rowBits = rows[r];
        for (int c = 0; c < 7; c++) {
            bool active = (rowBits & (1 << (6 - c))) != 0;
            int idx = r * 7 + c;
            digitRef.dots[idx].Fill(active ? g_brushActiveDot : g_brushInactiveDot);
        }
    }
}

// Update Time and Date with dirty-checking cache (eliminates 95%+ redundant DirectWrite passes)
void UpdateTimeAndDate() {
    SYSTEMTIME st;
    GetLocalTime(&st);

    bool is12h = (g_settings.timeFormat == L"12h");
    int rawHour = st.wHour;
    int displayHour = rawHour;
    bool isPM = (rawHour >= 12);
    const wchar_t* ampmStr = isPM ? L"PM" : L"AM";
    if (is12h) {
        displayHour = rawHour % 12;
        if (displayHour == 0) displayHour = 12;
    }

    bool hourChanged = (displayHour != g_timeCache.lastDisplayHour);
    bool minuteChanged = (st.wMinute != g_timeCache.lastMinute);
    bool secondChanged = (st.wSecond != g_timeCache.lastSecond);
    bool ampmChanged = (g_timeCache.lastAmPm != ampmStr);
    bool dateChanged = (st.wDay != g_timeCache.lastDay || st.wMonth != g_timeCache.lastMonth || st.wYear != g_timeCache.lastYear);

    if (g_ampmTextBlock && ampmChanged) {
        g_ampmTextBlock.Text(ampmStr);
    }
    // Update the cache even when the current style renders AM/PM inside its
    // main text. Otherwise "ampmChanged" stays true and defeats dirty checking.
    g_timeCache.lastAmPm = ampmStr;

    if (g_settings.clockStyle == L"pixel") {
        int h1 = displayHour / 10;
        int h2 = displayHour % 10;
        int m1 = st.wMinute / 10;
        int m2 = st.wMinute % 10;

        SetPixelDigitValue(g_pixelDigits[0], h1);
        SetPixelDigitValue(g_pixelDigits[1], h2);
        SetPixelDigitValue(g_pixelDigits[2], m1);
        SetPixelDigitValue(g_pixelDigits[3], m2);

        if (g_settings.showSeconds) {
            int s1 = st.wSecond / 10;
            int s2 = st.wSecond % 10;
            SetPixelDigitValue(g_pixelDigits[4], s1);
            SetPixelDigitValue(g_pixelDigits[5], s2);
        }
    } else if (g_settings.clockStyle == L"macos") {
        bool shouldUpdateTime = g_settings.showSeconds ? secondChanged : (minuteChanged || hourChanged || ampmChanged);
        if (g_macosTimeTextBlock && (shouldUpdateTime || g_timeCache.lastMacosTime.empty())) {
            WCHAR timeBuf[32];
            if (is12h) {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%d:%02d:%02d %s", displayHour, st.wMinute, st.wSecond, ampmStr);
                } else {
                    swprintf_s(timeBuf, L"%d:%02d %s", displayHour, st.wMinute, ampmStr);
                }
            } else {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%02d:%02d:%02d", displayHour, st.wMinute, st.wSecond);
                } else {
                    swprintf_s(timeBuf, L"%02d:%02d", displayHour, st.wMinute);
                }
            }
            g_macosTimeTextBlock.Text(timeBuf);
            if (g_macosShadowTextBlock) {
                g_macosShadowTextBlock.Text(timeBuf);
            }
            g_timeCache.lastMacosTime = timeBuf;
        }

        if (g_macosDateTextBlock && (dateChanged || g_timeCache.lastMacosDate.empty())) {
            WCHAR dateBuf[128];
            if (GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st, L"dddd, d MMMM", dateBuf, ARRAYSIZE(dateBuf)) == 0) {
                GetDateFormatW(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, nullptr, dateBuf, ARRAYSIZE(dateBuf));
            }
            g_macosDateTextBlock.Text(dateBuf);
            if (g_macosDateShadowBlock) {
                g_macosDateShadowBlock.Text(dateBuf);
            }
            g_timeCache.lastMacosDate = dateBuf;
        }
    } else if (g_settings.clockStyle == L"aura") {
        bool shouldUpdateAura = g_settings.showSeconds ? secondChanged : (minuteChanged || hourChanged || ampmChanged);
        if (g_auraTimeTextBlock && (shouldUpdateAura || g_timeCache.lastAuraTime.empty())) {
            WCHAR timeBuf[32];
            if (is12h) {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%d:%02d:%02d %s", displayHour, st.wMinute, st.wSecond, ampmStr);
                } else {
                    swprintf_s(timeBuf, L"%d:%02d %s", displayHour, st.wMinute, ampmStr);
                }
            } else {
                if (g_settings.showSeconds) {
                    swprintf_s(timeBuf, L"%02d:%02d:%02d", displayHour, st.wMinute, st.wSecond);
                } else {
                    swprintf_s(timeBuf, L"%02d:%02d", displayHour, st.wMinute);
                }
            }
            g_auraTimeTextBlock.Text(timeBuf);
            if (g_auraShadowTextBlock) {
                g_auraShadowTextBlock.Text(timeBuf);
            }
            g_timeCache.lastAuraTime = timeBuf;
        }
    } else {
        // Flip Clock, Retro Vintage, Cyberpunk HUD
        if (g_hourTextBlock && hourChanged) {
            WCHAR hourBuf[8];
            // Text-based 12-hour clocks omit the leading zero consistently.
            if (is12h) {
                swprintf_s(hourBuf, L"%d", displayHour);
            } else {
                swprintf_s(hourBuf, L"%02d", displayHour);
            }
            g_hourTextBlock.Text(hourBuf);
            if (g_hourBotTextBlock) {
                g_hourBotTextBlock.Text(hourBuf);
            }
        }

        if (g_minTextBlock && minuteChanged) {
            WCHAR minBuf[8];
            swprintf_s(minBuf, L"%02d", st.wMinute);
            g_minTextBlock.Text(minBuf);
            if (g_minBotTextBlock) {
                g_minBotTextBlock.Text(minBuf);
            }
        }

        if (g_secTextBlock && g_settings.showSeconds && secondChanged) {
            WCHAR secBuf[8];
            swprintf_s(secBuf, L"%02d", st.wSecond);
            g_secTextBlock.Text(secBuf);
        }

        // Cyberpunk HUD Tactical Separator Pulse
        if (g_settings.clockStyle == L"cyberpunk" && secondChanged) {
            bool pulseState = (st.wSecond % 2 == 0);
            if (g_cyberSlash1) {
                g_cyberSlash1.Opacity(pulseState ? 1.0 : 0.35);
            }
            if (g_cyberSlash2) {
                g_cyberSlash2.Opacity(pulseState ? 1.0 : 0.35);
            }
            if (g_cyberLiveLabel) {
                g_cyberLiveLabel.Opacity(pulseState ? 1.0 : 0.55);
            }
        }
    }

    // Update the date cache even when the current style has no date capsule.
    // Otherwise "dateChanged" stays true forever for the Sonoma style, which
    // would format and assign the date on every tick.
    if (dateChanged) {
        g_timeCache.lastDay = st.wDay;
        g_timeCache.lastMonth = st.wMonth;
        g_timeCache.lastYear = st.wYear;
    }

    if (g_dateTextBlock && g_settings.showDateCapsule && (dateChanged || g_timeCache.lastDateStr.empty())) {
        WCHAR dateBuf[128];
        if (GetDateFormatW(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, nullptr, dateBuf, ARRAYSIZE(dateBuf)) > 0) {
            g_dateTextBlock.Text(dateBuf);
            g_timeCache.lastDateStr = dateBuf;
        }
    }

    // Battery percentage and power status update
    if (g_batteryStatusText && g_settings.showBatteryStatus && (minuteChanged || g_timeCache.lastBatteryPct == -1)) {
        auto bInfo = GetCurrentBatteryInfo();
        if (bInfo.hasBattery) {
            if (bInfo.percentage != g_timeCache.lastBatteryPct || bInfo.isCharging != g_timeCache.lastIsCharging) {
                WCHAR bBuf[32];
                if (g_settings.clockStyle == L"cyberpunk") {
                    swprintf_s(bBuf, bInfo.isCharging ? L"PWR: %d%% [CHG]" : L"BAT: %d%%", bInfo.percentage);
                } else {
                    swprintf_s(bBuf, L"%d%%", bInfo.percentage);
                }
                g_batteryStatusText.Text(bBuf);
                if (g_batteryIconText) {
                    g_batteryIconText.Text(bInfo.isCharging ? L"⚡ " : L"🔋 ");
                    g_batteryIconText.Foreground(bInfo.isCharging 
                        ? wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 252, 238, 10))
                        : wuxm::SolidColorBrush(wu::Colors::White()));
                }
                g_timeCache.lastBatteryPct = bInfo.percentage;
                g_timeCache.lastIsCharging = bInfo.isCharging;
            }
        }
    }

    g_timeCache.lastDisplayHour = displayHour;
    g_timeCache.lastMinute = st.wMinute;
    g_timeCache.lastSecond = st.wSecond;
}

// Build Single Flip Card Element (Two physically split upper/lower flaps with clean scissors-cut gap)
wuxc::Grid CreateFlipCard(double width, double height, double cornerRadius, double fontSize,
                          wuxc::TextBlock& outTopText, wuxc::TextBlock& outBotText,
                          const wchar_t* textName, bool isHourCard = false) {
    wuxc::Grid card;
    card.Width(width);
    card.Height(height);

    double seamGap = 4.0;
    double halfHeight = (height - seamGap) / 2.0;

    // Optical centering offset for Segoe UI Variable ExtraBold: aligns the
    // digit to the split line so both halves are visually 50/50.
    double opticalOffset = fontSize * 0.045;

    // --- 1. TOP FLAP (upper half card & digits) ---
    wuxc::Grid topContainer;
    topContainer.Width(width);
    topContainer.Height(height);

    wuxm::RectangleGeometry topClip;
    topClip.Rect(winrt::Windows::Foundation::Rect{ 0.0f, 0.0f, (float)width, (float)halfHeight });
    topContainer.Clip(topClip);

    // Top flap acrylic body & border: rounded top corners, 2 px inner cut,
    // open bottom edge.
    wuxc::Border topFlap;
    topFlap.Width(width);
    topFlap.Height(halfHeight);
    topFlap.VerticalAlignment(wux::VerticalAlignment::Top);
    topFlap.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.glassOpacity, 255, 255, 255)));
    topFlap.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 255, 255, 255)));
    topFlap.BorderThickness(wux::Thickness{ 1.5, 1.5, 1.5, 0.0 });
    topFlap.CornerRadius(wux::CornerRadiusHelper::FromRadii(cornerRadius, cornerRadius, 2.0, 2.0));
    topContainer.Children().Append(topFlap);

    // Top digit: centered in the card; the lower half is clipped at the split.
    wuxc::TextBlock topText;
    topText.Name(textName);
    topText.FontSize(fontSize);
    topText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    topText.FontWeight(winrt::Windows::UI::Text::FontWeights::ExtraBold());
    topText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
    topText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    topText.VerticalAlignment(wux::VerticalAlignment::Center);
    topText.Margin(wux::Thickness{ 0.0, opticalOffset, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(topText, wux::FontNumeralAlignment::Tabular);
    topContainer.Children().Append(topText);

    // AM/PM badge on the clock card (12-hour format, top-left of the top flap).
    if (isHourCard && g_settings.timeFormat == L"12h") {
        wuxc::Border ampmBadge;
        ampmBadge.HorizontalAlignment(wux::HorizontalAlignment::Left);
        ampmBadge.VerticalAlignment(wux::VerticalAlignment::Top);
        ampmBadge.Margin(wux::Thickness{ 14.0, 12.0, 0.0, 0.0 });
        ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(45, 255, 255, 255)));
        ampmBadge.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(75, 255, 255, 255)));
        ampmBadge.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(6.0));
        ampmBadge.Padding(wux::Thickness{ 7.0, 2.0, 7.0, 2.0 });

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkFlipAmPmText");
        ampmText.FontSize(12.0);
        ampmText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
        ampmBadge.Child(ampmText);

        topContainer.Children().Append(ampmBadge);
        g_ampmTextBlock = ampmText;
    }

    // --- 2. BOTTOM FLAP (lower half card & digits) ---
    wuxc::Grid botContainer;
    botContainer.Width(width);
    botContainer.Height(height);

    wuxm::RectangleGeometry botClip;
    botClip.Rect(winrt::Windows::Foundation::Rect{ 0.0f, (float)(halfHeight + seamGap), (float)width, (float)halfHeight });
    botContainer.Clip(botClip);

    // Bottom flap acrylic body & border: 2 px inner cut, rounded bottom
    // corners, open top edge.
    wuxc::Border bottomFlap;
    bottomFlap.Width(width);
    bottomFlap.Height(halfHeight);
    bottomFlap.VerticalAlignment(wux::VerticalAlignment::Bottom);
    bottomFlap.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.glassOpacity, 255, 255, 255)));
    bottomFlap.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 255, 255, 255)));
    bottomFlap.BorderThickness(wux::Thickness{ 1.5, 0.0, 1.5, 1.5 });
    bottomFlap.CornerRadius(wux::CornerRadiusHelper::FromRadii(2.0, 2.0, cornerRadius, cornerRadius));
    botContainer.Children().Append(bottomFlap);

    // Bottom digit: same center coordinates as the top digit; the upper half
    // is clipped at the split.
    std::wstring botName = std::wstring(textName) + L"_Bot";
    wuxc::TextBlock botText;
    botText.Name(botName);
    botText.FontSize(fontSize);
    botText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    botText.FontWeight(winrt::Windows::UI::Text::FontWeights::ExtraBold());
    botText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
    botText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    botText.VerticalAlignment(wux::VerticalAlignment::Center);
    botText.Margin(wux::Thickness{ 0.0, opticalOffset, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(botText, wux::FontNumeralAlignment::Tabular);
    botContainer.Children().Append(botText);

    card.Children().Append(topContainer);
    card.Children().Append(botContainer);

    outTopText = topText;
    outBotText = botText;
    return card;
}

// Build Flip Clock UI Element
wux::UIElement BuildFlipClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkFlipClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel cardsLayout;
    cardsLayout.Orientation(wuxc::Orientation::Horizontal);
    cardsLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);
    cardsLayout.Spacing(22.0);

    // Near-square large chunky card proportions (aspect ratio ~0.99)
    double cardWidth = (double)g_settings.clockFontSize * 2.02;
    double cardHeight = (double)g_settings.clockFontSize * 2.05;
    double flipFontSize = (double)g_settings.clockFontSize * 1.72;
    double cornerRad = 22.0;

    // 1. Hour Card
    auto hourCard = CreateFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_hourTextBlock, g_hourBotTextBlock, L"WindhawkFlipHourText", true);
    cardsLayout.Children().Append(hourCard);

    // 2. Minute Card
    auto minCard = CreateFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_minTextBlock, g_minBotTextBlock, L"WindhawkFlipMinText", false);
    cardsLayout.Children().Append(minCard);

    verticalLayout.Children().Append(cardsLayout);

    // Date Capsule
    if (g_settings.showDateCapsule) {
        wuxc::Border dateCapsule;
        dateCapsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(35, 255, 255, 255)));
        dateCapsule.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(60, 255, 255, 255)));
        dateCapsule.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        dateCapsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(12.0));
        dateCapsule.Padding(wux::Thickness{ 20.0, 8.0, 20.0, 8.0 });
        dateCapsule.Margin(wux::Thickness{ 0.0, 18.0, 0.0, 0.0 });
        dateCapsule.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkFlipDateText");
        dateText.FontSize(18.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 255, 255, 255)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);

        dateCapsule.Child(dateText);
        verticalLayout.Children().Append(dateCapsule);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Build Single Vintage Flip Card Element (Retro Mechanical Airport/Station Solari Style)
wuxc::Grid CreateVintageFlipCard(double width, double height, double cornerRadius, double fontSize, wuxc::TextBlock& outText, const wchar_t* textName, bool isHourCard = false) {
    wuxc::Grid card;
    card.Width(width);
    card.Height(height);

    // Fliqlo matte charcoal black flap body (#0e0e0e) - frameless matte finish.
    wuxc::Border flapBackplate;
    flapBackplate.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(242, 14, 14, 14)));
    flapBackplate.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(cornerRadius));

    // Fliqlo Original Matte Light Silver Digit Typography (#b9b9b9 / rgb(185, 185, 185))
    wuxc::TextBlock cardText;
    cardText.Name(textName);
    cardText.FontSize(fontSize);
    cardText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    cardText.FontWeight(winrt::Windows::UI::Text::FontWeights::ExtraBold());
    cardText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 185, 185, 185)));
    cardText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    cardText.VerticalAlignment(wux::VerticalAlignment::Center);
    double opticalOffset = fontSize * 0.045;
    cardText.Margin(wux::Thickness{ 0.0, opticalOffset, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(cardText, wux::FontNumeralAlignment::Tabular);

    // Center Mechanical Split: Upper shadow + Lower light catch bevel + Side axle hinge clips
    wuxc::Grid splitSeam;
    splitSeam.Height(10.0);
    splitSeam.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    splitSeam.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::StackPanel seamLines;
    seamLines.Orientation(wuxc::Orientation::Vertical);
    seamLines.VerticalAlignment(wux::VerticalAlignment::Center);

    // Deep shadow groove between top and bottom flaps
    wuxs::Rectangle shadowGroove;
    shadowGroove.Height(2.0);
    shadowGroove.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    shadowGroove.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 0, 0)));
    shadowGroove.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });

    // Subtle edge highlight on bottom flap catching ambient overhead light
    wuxs::Rectangle bevelHighlight;
    bevelHighlight.Height(1.0);
    bevelHighlight.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    bevelHighlight.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(40, 255, 255, 255)));
    bevelHighlight.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });

    seamLines.Children().Append(shadowGroove);
    seamLines.Children().Append(bevelHighlight);
    splitSeam.Children().Append(seamLines);

    // Left and Right Axle Clips (Mechanical hinge pins)
    wuxs::Rectangle leftClip;
    leftClip.Width(4.0);
    leftClip.Height(9.0);
    leftClip.RadiusX(1.0);
    leftClip.RadiusY(1.0);
    leftClip.HorizontalAlignment(wux::HorizontalAlignment::Left);
    leftClip.VerticalAlignment(wux::VerticalAlignment::Center);
    leftClip.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 8, 8, 8)));

    wuxs::Rectangle rightClip;
    rightClip.Width(4.0);
    rightClip.Height(9.0);
    rightClip.RadiusX(1.0);
    rightClip.RadiusY(1.0);
    rightClip.HorizontalAlignment(wux::HorizontalAlignment::Right);
    rightClip.VerticalAlignment(wux::VerticalAlignment::Center);
    rightClip.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 8, 8, 8)));

    splitSeam.Children().Append(leftClip);
    splitSeam.Children().Append(rightClip);

    card.Children().Append(flapBackplate);
    card.Children().Append(cardText);
    card.Children().Append(splitSeam);
    outText = cardText;

    // Vintage Mechanical AM/PM Tag on Hour Card (12h format)
    if (isHourCard && g_settings.timeFormat == L"12h") {
        wuxc::Border ampmBadge;
        ampmBadge.HorizontalAlignment(wux::HorizontalAlignment::Left);
        ampmBadge.VerticalAlignment(wux::VerticalAlignment::Top);
        ampmBadge.Margin(wux::Thickness{ 12.0, 10.0, 0.0, 0.0 });
        ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(245, 14, 14, 14)));
        ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(4.0));
        ampmBadge.Padding(wux::Thickness{ 6.0, 1.5, 6.0, 1.5 });

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkVintageAmPmText");
        ampmText.FontSize(11.0);
        ampmText.FontFamily(wuxm::FontFamily(L"Consolas"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
        ampmBadge.Child(ampmText);

        card.Children().Append(ampmBadge);
        g_ampmTextBlock = ampmText;
    }

    return card;
}

// Build Retro Vintage Flip Clock UI Element
wux::UIElement BuildVintageClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkVintageClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel cardsLayout;
    cardsLayout.Orientation(wuxc::Orientation::Horizontal);
    cardsLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);
    cardsLayout.Spacing(20.0);

    // Near-square large chunky card proportions (aspect ratio ~0.99)
    double cardWidth = (double)g_settings.clockFontSize * 2.02;
    double cardHeight = (double)g_settings.clockFontSize * 2.05;
    double flipFontSize = (double)g_settings.clockFontSize * 1.72;
    double cornerRad = 18.0;

    // 1. Hour Card
    auto hourCard = CreateVintageFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_hourTextBlock, L"WindhawkVintageHourText", true);
    cardsLayout.Children().Append(hourCard);

    // 2. Minute Card
    auto minCard = CreateVintageFlipCard(cardWidth, cardHeight, cornerRad, flipFontSize, g_minTextBlock, L"WindhawkVintageMinText", false);
    cardsLayout.Children().Append(minCard);

    verticalLayout.Children().Append(cardsLayout);

    // Vintage Date Plate (Fliqlo Matte Dark Style)
    if (g_settings.showDateCapsule) {
        wuxc::Border dateCapsule;
        dateCapsule.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(220, 14, 14, 14)));
        dateCapsule.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(8.0));
        dateCapsule.Padding(wux::Thickness{ 22.0, 7.0, 22.0, 7.0 });
        dateCapsule.Margin(wux::Thickness{ 0.0, 18.0, 0.0, 0.0 });
        dateCapsule.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkVintageDateText");
        dateText.FontSize(18.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 185, 185, 185)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);

        dateCapsule.Child(dateText);
        verticalLayout.Children().Append(dateCapsule);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false, true);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Build Cyberpunk Sci-Fi HUD Terminal Clock Element (Authentic Netrunner / Tactical Cyberpunk HUD)
wux::UIElement BuildCyberpunkClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkCyberpunkHUDContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    // Outer Tactical HUD Bracket Frame
    wuxc::Grid hudFrame;
    hudFrame.HorizontalAlignment(wux::HorizontalAlignment::Center);
    hudFrame.VerticalAlignment(wux::VerticalAlignment::Center);

    // 1. Dark Obsidian Glass Chassis with Neon Cyan Edge
    wuxc::Border chassis;
    chassis.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(215, 8, 10, 18)));
    chassis.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(130, 0, 245, 255))); // Electric Neon Cyan
    chassis.BorderThickness(wux::Thickness{ 1.5, 1.5, 1.5, 1.5 });
    chassis.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(6.0));
    chassis.Padding(wux::Thickness{ 32.0, 18.0, 32.0, 20.0 });

    wuxc::StackPanel hudContent;
    hudContent.Orientation(wuxc::Orientation::Vertical);
    hudContent.HorizontalAlignment(wux::HorizontalAlignment::Center);

    // --- TOP CYBER HEADER BAR ---
    wuxc::Grid topBar;
    topBar.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    topBar.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 14.0 });

    // Left Tag: [ SYS.LOC // TIME_SYNC ]
    wuxc::TextBlock topTag;
    topTag.Text(L"SYS.LOC // TIME_SYNC");
    topTag.FontSize(11.0);
    topTag.FontFamily(wuxm::FontFamily(L"Consolas"));
    topTag.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    topTag.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(220, 0, 245, 255)));
    topTag.HorizontalAlignment(wux::HorizontalAlignment::Left);
    topTag.VerticalAlignment(wux::VerticalAlignment::Center);

    // Right Telemetry Blocks: 3 Cyan + 1 Acid Yellow
    wuxc::StackPanel telemetryPills;
    telemetryPills.Orientation(wuxc::Orientation::Horizontal);
    telemetryPills.HorizontalAlignment(wux::HorizontalAlignment::Right);
    telemetryPills.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock liveLabel;
    liveLabel.Text(L"ONLINE ");
    liveLabel.FontSize(10.0);
    liveLabel.FontFamily(wuxm::FontFamily(L"Consolas"));
    liveLabel.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    liveLabel.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(220, 252, 238, 10))); // Acid Yellow
    liveLabel.VerticalAlignment(wux::VerticalAlignment::Center);
    telemetryPills.Children().Append(liveLabel);
    g_cyberLiveLabel = liveLabel;

    for (int i = 0; i < 3; i++) {
        wuxs::Rectangle pill;
        pill.Width(6.0);
        pill.Height(9.0);
        pill.RadiusX(1.0);
        pill.RadiusY(1.0);
        pill.Margin(wux::Thickness{ 2.0, 0.0, 0.0, 0.0 });
        pill.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(230, 0, 245, 255)));
        telemetryPills.Children().Append(pill);
    }
    wuxs::Rectangle yellowPill;
    yellowPill.Width(6.0);
    yellowPill.Height(9.0);
    yellowPill.RadiusX(1.0);
    yellowPill.RadiusY(1.0);
    yellowPill.Margin(wux::Thickness{ 2.0, 0.0, 0.0, 0.0 });
    yellowPill.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(250, 252, 238, 10))); // Acid Yellow
    telemetryPills.Children().Append(yellowPill);

    // Tactical laptop battery telemetry (if battery present)
    auto bInfo = GetCurrentBatteryInfo();
    if (g_settings.showBatteryStatus && bInfo.hasBattery) {
        wuxc::Border cyberBat;
        cyberBat.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(40, 0, 245, 255)));
        cyberBat.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(90, 0, 245, 255)));
        cyberBat.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        cyberBat.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(3.0));
        cyberBat.Padding(wux::Thickness{ 6.0, 1.0, 6.0, 1.0 });
        cyberBat.Margin(wux::Thickness{ 6.0, 0.0, 0.0, 0.0 });
        cyberBat.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock batText;
        batText.Name(L"WindhawkCyberBatteryText");
        WCHAR bBuf[32];
        swprintf_s(bBuf, bInfo.isCharging ? L"PWR: %d%% [CHG]" : L"BAT: %d%%", bInfo.percentage);
        batText.Text(bBuf);
        batText.FontSize(9.5);
        batText.FontFamily(wuxm::FontFamily(L"Consolas"));
        batText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        batText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 0, 245, 255)));
        cyberBat.Child(batText);
        telemetryPills.Children().Append(cyberBat);
        g_batteryStatusText = batText;
    }

    topBar.Children().Append(topTag);
    topBar.Children().Append(telemetryPills);
    hudContent.Children().Append(topBar);

    // Top Header Divider (Neon Cyan hair line)
    wuxs::Rectangle headerDivider;
    headerDivider.Height(1.0);
    headerDivider.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    headerDivider.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(70, 0, 245, 255)));
    headerDivider.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 16.0 });
    hudContent.Children().Append(headerDivider);

    // --- CENTER DIGITS DISPLAY ---
    wuxc::StackPanel digitsPanel;
    digitsPanel.Orientation(wuxc::Orientation::Horizontal);
    digitsPanel.HorizontalAlignment(wux::HorizontalAlignment::Center);
    digitsPanel.VerticalAlignment(wux::VerticalAlignment::Center);

    double numFontSize = (double)g_settings.clockFontSize * 1.05;
    double digitBlockWidth = numFontSize * 1.28;

    // Hour
    wuxc::TextBlock hourText;
    hourText.Name(L"WindhawkCyberHourText");
    hourText.FontSize(numFontSize);
    hourText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    hourText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    hourText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 235, 252, 255))); // Electric White-Cyan
    hourText.Width(digitBlockWidth);
    hourText.TextAlignment(wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(hourText, wux::FontNumeralAlignment::Tabular);
    digitsPanel.Children().Append(hourText);
    g_hourTextBlock = hourText;

    // Slashes 1 (//) in Acid Yellow
    wuxc::TextBlock slash1;
    slash1.Text(L" // ");
    slash1.FontSize(numFontSize * 0.7);
    slash1.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    slash1.FontWeight(winrt::Windows::UI::Text::FontWeights::Light());
    slash1.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(230, 252, 238, 10))); // Acid Yellow
    slash1.VerticalAlignment(wux::VerticalAlignment::Center);
    slash1.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });
    digitsPanel.Children().Append(slash1);
    g_cyberSlash1 = slash1;

    // Minute
    wuxc::TextBlock minText;
    minText.Name(L"WindhawkCyberMinText");
    minText.FontSize(numFontSize);
    minText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    minText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    minText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 235, 252, 255)));
    minText.Width(digitBlockWidth);
    minText.TextAlignment(wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(minText, wux::FontNumeralAlignment::Tabular);
    digitsPanel.Children().Append(minText);
    g_minTextBlock = minText;

    // Seconds (if enabled)
    if (g_settings.showSeconds) {
        wuxc::TextBlock slash2;
        slash2.Text(L" // ");
        slash2.FontSize(numFontSize * 0.7);
        slash2.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
        slash2.FontWeight(winrt::Windows::UI::Text::FontWeights::Light());
        slash2.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 255, 0, 128))); // Hot Magenta
        slash2.VerticalAlignment(wux::VerticalAlignment::Center);
        slash2.Margin(wux::Thickness{ 4.0, 0.0, 4.0, 0.0 });
        digitsPanel.Children().Append(slash2);
        g_cyberSlash2 = slash2;

        wuxc::TextBlock secText;
        secText.Name(L"WindhawkCyberSecText");
        secText.FontSize(numFontSize);
        secText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
        secText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        secText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 235, 252, 255)));
        secText.Width(digitBlockWidth);
        secText.TextAlignment(wux::TextAlignment::Center);
        wuxd::Typography::SetNumeralAlignment(secText, wux::FontNumeralAlignment::Tabular);
        digitsPanel.Children().Append(secText);
        g_secTextBlock = secText;
    }

    // 12-Hour AM/PM Tactical Indicator
    if (g_settings.timeFormat == L"12h") {
        wuxc::Border ampmBadge;
        ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(50, 0, 245, 255)));
        ampmBadge.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(180, 0, 245, 255)));
        ampmBadge.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(4.0));
        ampmBadge.Padding(wux::Thickness{ 8.0, 4.0, 8.0, 4.0 });
        ampmBadge.Margin(wux::Thickness{ 12.0, 0.0, 0.0, 0.0 });
        ampmBadge.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkCyberAmPmText");
        ampmText.FontSize(numFontSize * 0.32);
        ampmText.FontFamily(wuxm::FontFamily(L"Consolas"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 245, 255)));
        ampmBadge.Child(ampmText);
        digitsPanel.Children().Append(ampmBadge);
        g_ampmTextBlock = ampmText;
    }

    hudContent.Children().Append(digitsPanel);

    // Bottom Scanline Bar (Dotted / Segmented Laser Track)
    wuxs::Rectangle scanline;
    scanline.Height(2.0);
    scanline.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    scanline.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(90, 0, 245, 255)));
    scanline.Margin(wux::Thickness{ 0.0, 14.0, 0.0, 14.0 });
    hudContent.Children().Append(scanline);

    // --- BOTTOM TELEMETRY DATE BAR ---
    if (g_settings.showDateCapsule) {
        wuxc::StackPanel bottomBar;
        bottomBar.Orientation(wuxc::Orientation::Horizontal);
        bottomBar.HorizontalAlignment(wux::HorizontalAlignment::Center);

        // Acid Yellow Telemetry Tag Badge
        wuxc::Border tagBadge;
        tagBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(235, 252, 238, 10))); // Cyber Yellow
        tagBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(3.0));
        tagBadge.Padding(wux::Thickness{ 7.0, 2.0, 7.0, 2.0 });
        tagBadge.Margin(wux::Thickness{ 0.0, 0.0, 10.0, 0.0 });

        wuxc::TextBlock tagText;
        tagText.Text(L"DATE.LOG");
        tagText.FontSize(11.0);
        tagText.FontFamily(wuxm::FontFamily(L"Consolas"));
        tagText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        tagText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 10, 12, 18))); // Deep Black
        tagBadge.Child(tagText);
        bottomBar.Children().Append(tagBadge);

        // Date String (Cyan glow)
        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkCyberDateText");
        dateText.FontSize(15.0);
        dateText.FontFamily(wuxm::FontFamily(L"Consolas"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 0, 245, 255))); // Electric Cyan
        dateText.VerticalAlignment(wux::VerticalAlignment::Center);
        bottomBar.Children().Append(dateText);
        g_dateTextBlock = dateText;

        hudContent.Children().Append(bottomBar);
    }

    chassis.Child(hudContent);
    hudFrame.Children().Append(chassis);

    // --- TACTICAL HUD CORNER ACCENTS (4 Tactical Chamfer Brackets) ---
    // 1. Top-Left L-bracket (Acid Yellow)
    wuxc::Grid cornerTL;
    cornerTL.HorizontalAlignment(wux::HorizontalAlignment::Left);
    cornerTL.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTL.Margin(wux::Thickness{ -2.0, -2.0, 0.0, 0.0 });
    wuxs::Rectangle tlH;
    tlH.Width(16.0); tlH.Height(3.0);
    tlH.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 252, 238, 10)));
    tlH.HorizontalAlignment(wux::HorizontalAlignment::Left);
    tlH.VerticalAlignment(wux::VerticalAlignment::Top);
    wuxs::Rectangle tlV;
    tlV.Width(3.0); tlV.Height(16.0);
    tlV.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 252, 238, 10)));
    tlV.HorizontalAlignment(wux::HorizontalAlignment::Left);
    tlV.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTL.Children().Append(tlH);
    cornerTL.Children().Append(tlV);
    hudFrame.Children().Append(cornerTL);

    // 2. Top-Right Accent (Hot Magenta)
    wuxc::Grid cornerTR;
    cornerTR.HorizontalAlignment(wux::HorizontalAlignment::Right);
    cornerTR.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTR.Margin(wux::Thickness{ 0.0, -2.0, -2.0, 0.0 });
    wuxs::Rectangle trH;
    trH.Width(16.0); trH.Height(3.0);
    trH.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 255, 0, 128)));
    trH.HorizontalAlignment(wux::HorizontalAlignment::Right);
    trH.VerticalAlignment(wux::VerticalAlignment::Top);
    wuxs::Rectangle trV;
    trV.Width(3.0); trV.Height(16.0);
    trV.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 255, 0, 128)));
    trV.HorizontalAlignment(wux::HorizontalAlignment::Right);
    trV.VerticalAlignment(wux::VerticalAlignment::Top);
    cornerTR.Children().Append(trH);
    cornerTR.Children().Append(trV);
    hudFrame.Children().Append(cornerTR);

    // 3. Bottom-Left Accent (Electric Cyan)
    wuxc::Grid cornerBL;
    cornerBL.HorizontalAlignment(wux::HorizontalAlignment::Left);
    cornerBL.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBL.Margin(wux::Thickness{ -2.0, 0.0, 0.0, -2.0 });
    wuxs::Rectangle blH;
    blH.Width(16.0); blH.Height(3.0);
    blH.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 245, 255)));
    blH.HorizontalAlignment(wux::HorizontalAlignment::Left);
    blH.VerticalAlignment(wux::VerticalAlignment::Bottom);
    wuxs::Rectangle blV;
    blV.Width(3.0); blV.Height(16.0);
    blV.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 245, 255)));
    blV.HorizontalAlignment(wux::HorizontalAlignment::Left);
    blV.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBL.Children().Append(blH);
    cornerBL.Children().Append(blV);
    hudFrame.Children().Append(cornerBL);

    // 4. Bottom-Right Accent (Electric Cyan)
    wuxc::Grid cornerBR;
    cornerBR.HorizontalAlignment(wux::HorizontalAlignment::Right);
    cornerBR.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBR.Margin(wux::Thickness{ 0.0, 0.0, -2.0, -2.0 });
    wuxs::Rectangle brH;
    brH.Width(16.0); brH.Height(3.0);
    brH.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 245, 255)));
    brH.HorizontalAlignment(wux::HorizontalAlignment::Right);
    brH.VerticalAlignment(wux::VerticalAlignment::Bottom);
    wuxs::Rectangle brV;
    brV.Width(3.0); brV.Height(16.0);
    brV.Fill(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(255, 0, 245, 255)));
    brV.HorizontalAlignment(wux::HorizontalAlignment::Right);
    brV.VerticalAlignment(wux::VerticalAlignment::Bottom);
    cornerBR.Children().Append(brH);
    cornerBR.Children().Append(brV);
    hudFrame.Children().Append(cornerBR);

    verticalLayout.Children().Append(hudFrame);
    rootContainer.Children().Append(verticalLayout);

    UpdateTimeAndDate();

    return rootContainer;
}

// Build Pixel Digit Grid Helper (7 cols x 11 rows, 77 dots)
wuxc::Grid CreatePixelDigitGrid(PixelDigitGrid& digitRef, double dotSize, double spacing) {
    wuxc::Grid grid;
    for (int c = 0; c < 7; c++) {
        wuxc::ColumnDefinition colDef;
        colDef.Width(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.ColumnDefinitions().Append(colDef);
    }
    for (int r = 0; r < 11; r++) {
        wuxc::RowDefinition rowDef;
        rowDef.Height(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.RowDefinitions().Append(rowDef);
    }

    digitRef.dots.clear();
    digitRef.dots.reserve(77);

    for (int r = 0; r < 11; r++) {
        for (int c = 0; c < 7; c++) {
            wuxs::Ellipse dot;
            dot.Width(dotSize);
            dot.Height(dotSize);
            dot.Fill(g_brushInactiveDot);
            dot.HorizontalAlignment(wux::HorizontalAlignment::Center);
            dot.VerticalAlignment(wux::VerticalAlignment::Center);
            wuxc::Grid::SetRow(dot, r);
            wuxc::Grid::SetColumn(dot, c);
            grid.Children().Append(dot);
            digitRef.dots.push_back(dot);
        }
    }
    return grid;
}

// Build Pixel Colon Helper (2 cols x 11 rows, 4 rows x 2 dots active per colon point)
wuxc::Grid CreatePixelColon(double dotSize, double spacing) {
    wuxc::Grid grid;
    for (int c = 0; c < 2; c++) {
        wuxc::ColumnDefinition colDef;
        colDef.Width(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.ColumnDefinitions().Append(colDef);
    }
    for (int r = 0; r < 11; r++) {
        wuxc::RowDefinition rowDef;
        rowDef.Height(wux::GridLengthHelper::FromPixels(dotSize + spacing));
        grid.RowDefinitions().Append(rowDef);
    }

    for (int r = 0; r < 11; r++) {
        for (int c = 0; c < 2; c++) {
            wuxs::Ellipse dot;
            dot.Width(dotSize);
            dot.Height(dotSize);
            bool isColonDot = (r == 2 || r == 3 || r == 7 || r == 8);
            dot.Fill(isColonDot ? g_brushActiveDot : g_brushInactiveDot);
            dot.HorizontalAlignment(wux::HorizontalAlignment::Center);
            dot.VerticalAlignment(wux::VerticalAlignment::Center);
            wuxc::Grid::SetRow(dot, r);
            wuxc::Grid::SetColumn(dot, c);
            grid.Children().Append(dot);
        }
    }
    return grid;
}

// Build Retro Pixel / LED Dot Matrix Clock Element
wux::UIElement BuildPixelClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkPixelClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    // High-contrast brushes: 100% active white, subtle 6% ambient inactive dots
    g_brushActiveDot = wuxm::SolidColorBrush(wu::Colors::White());
    g_brushInactiveDot = wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(15, 255, 255, 255));

    // Dark frosted acrylic backplate for perfect contrast on any wallpaper
    wuxc::Border darkBackplate;
    darkBackplate.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(32, 10, 14, 22)));
    darkBackplate.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(15, 255, 255, 255)));
    darkBackplate.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
    darkBackplate.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(20.0));
    darkBackplate.Padding(wux::Thickness{ 26.0, 24.0, 26.0, 24.0 });
    darkBackplate.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    wuxc::StackPanel matrixLayout;
    matrixLayout.Orientation(wuxc::Orientation::Horizontal);
    matrixLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);
    matrixLayout.VerticalAlignment(wux::VerticalAlignment::Center);

    // Scale dot size appropriately for 7x11 double-pixel thickness
    double dotSize = (double)g_settings.clockFontSize * 0.095;
    double spacing = dotSize * 0.35;
    double digitGap = dotSize * 1.3;
    double colonGap = dotSize * 1.0;

    // H1 & H2
    auto h1 = CreatePixelDigitGrid(g_pixelDigits[0], dotSize, spacing);
    auto h2 = CreatePixelDigitGrid(g_pixelDigits[1], dotSize, spacing);
    h1.Margin(wux::Thickness{ 0.0, 0.0, digitGap, 0.0 });
    h2.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });
    matrixLayout.Children().Append(h1);
    matrixLayout.Children().Append(h2);

    // Colon 1 (between hours and minutes)
    auto colon1 = CreatePixelColon(dotSize, spacing);
    colon1.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });
    matrixLayout.Children().Append(colon1);

    // M1 & M2
    auto m1 = CreatePixelDigitGrid(g_pixelDigits[2], dotSize, spacing);
    auto m2 = CreatePixelDigitGrid(g_pixelDigits[3], dotSize, spacing);
    m1.Margin(wux::Thickness{ 0.0, 0.0, digitGap, 0.0 });
    matrixLayout.Children().Append(m1);
    matrixLayout.Children().Append(m2);

    // Optional Seconds (HH:MM:SS format at the same scale and alignment)
    if (g_settings.showSeconds) {
        m2.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });

        // Colon 2 (between minutes and seconds)
        auto colon2 = CreatePixelColon(dotSize, spacing);
        colon2.Margin(wux::Thickness{ 0.0, 0.0, colonGap, 0.0 });
        matrixLayout.Children().Append(colon2);

        // S1 & S2
        auto s1 = CreatePixelDigitGrid(g_pixelDigits[4], dotSize, spacing);
        auto s2 = CreatePixelDigitGrid(g_pixelDigits[5], dotSize, spacing);
        s1.Margin(wux::Thickness{ 0.0, 0.0, digitGap, 0.0 });
        matrixLayout.Children().Append(s1);
        matrixLayout.Children().Append(s2);
    }

    // Optional 12-Hour AM/PM Indicator
    if (g_settings.timeFormat == L"12h") {
        wuxc::Border ampmBadge;
        ampmBadge.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(35, 255, 255, 255)));
        ampmBadge.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(60, 255, 255, 255)));
        ampmBadge.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        ampmBadge.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(6.0));
        ampmBadge.Padding(wux::Thickness{ 8.0, 3.0, 8.0, 3.0 });
        ampmBadge.Margin(wux::Thickness{ digitGap, 0.0, 0.0, 0.0 });
        ampmBadge.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock ampmText;
        ampmText.Name(L"WindhawkPixelAmPmText");
        ampmText.FontSize(dotSize * 2.8);
        ampmText.FontFamily(wuxm::FontFamily(L"Consolas"));
        ampmText.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        ampmText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
        ampmBadge.Child(ampmText);
        matrixLayout.Children().Append(ampmBadge);
        g_ampmTextBlock = ampmText;
    }

    verticalLayout.Children().Append(matrixLayout);

    // Date display
    if (g_settings.showDateCapsule) {
        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkPixelDateText");
        dateText.FontSize(19.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(235, 255, 255, 255)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);
        dateText.Margin(wux::Thickness{ 0.0, 22.0, 0.0, 0.0 });

        verticalLayout.Children().Append(dateText);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    darkBackplate.Child(verticalLayout);
    rootContainer.Children().Append(darkBackplate);

    UpdateTimeAndDate();

    return rootContainer;
}

// Build Minimalist Aura / Floating Clock Element
wux::UIElement BuildAuraClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkAuraClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(wux::HorizontalAlignment::Center);

    // Layered floating time container with soft ambient drop-shadow
    wuxc::Grid timeLayerGrid;
    timeLayerGrid.HorizontalAlignment(wux::HorizontalAlignment::Center);

    double fontSize = (double)(g_settings.clockFontSize * 1.35);

    // 1. Ambient Drop-Shadow Aura Layer (Behind foreground text for contrast on any wallpaper)
    wuxc::TextBlock shadowText;
    shadowText.FontSize(fontSize);
    shadowText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    shadowText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    shadowText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(140, 0, 0, 0)));
    shadowText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    shadowText.TextAlignment(wux::TextAlignment::Center);
    shadowText.Margin(wux::Thickness{ 0.0, 4.0, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(shadowText, wux::FontNumeralAlignment::Tabular);
    timeLayerGrid.Children().Append(shadowText);
    g_auraShadowTextBlock = shadowText;

    // 2. Foreground Crisp Pure White Floating Text Layer
    wuxc::TextBlock timeText;
    timeText.FontSize(fontSize);
    timeText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    timeText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    timeText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
    timeText.HorizontalAlignment(wux::HorizontalAlignment::Center);
    timeText.TextAlignment(wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(timeText, wux::FontNumeralAlignment::Tabular);
    timeLayerGrid.Children().Append(timeText);
    g_auraTimeTextBlock = timeText;

    verticalLayout.Children().Append(timeLayerGrid);

    // Floating Aura Date Pill
    if (g_settings.showDateCapsule) {
        wuxc::Border auraDatePill;
        auraDatePill.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(38, 255, 255, 255)));
        auraDatePill.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(65, 255, 255, 255)));
        auraDatePill.BorderThickness(wux::Thickness{ 1.0, 1.0, 1.0, 1.0 });
        auraDatePill.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(18.0));
        auraDatePill.Padding(wux::Thickness{ 22.0, 7.0, 22.0, 7.0 });
        auraDatePill.Margin(wux::Thickness{ 0.0, 14.0, 0.0, 0.0 });
        auraDatePill.HorizontalAlignment(wux::HorizontalAlignment::Center);

        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkAuraDateText");
        dateText.FontSize(18.0);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Text"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Medium());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(245, 255, 255, 255)));
        dateText.HorizontalAlignment(wux::HorizontalAlignment::Center);

        auraDatePill.Child(dateText);
        verticalLayout.Children().Append(auraDatePill);
        g_dateTextBlock = dateText;
    }

    // Battery Capsule (if enabled and laptop battery present)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(false);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Build Minimalist Sonoma (Clean Typography) Clock Element
wux::UIElement BuildMacOSClockElement() {
    wuxc::Grid rootContainer;
    rootContainer.Name(L"WindhawkMacOSClockContainer");
    ApplyAlignmentAndMargin(rootContainer);

    bool isBottomLeft = (g_settings.clockAlignment == L"bottomLeft");

    wuxc::StackPanel verticalLayout;
    verticalLayout.Orientation(wuxc::Orientation::Vertical);
    verticalLayout.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);

    // 1. macOS Date Header (Positioned elegantly ABOVE the time)
    if (g_settings.showDateCapsule) {
        wuxc::Grid dateLayerGrid;
        dateLayerGrid.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
        dateLayerGrid.Margin(wux::Thickness{ 0.0, 0.0, 0.0, 6.0 });

        double dateFontSize = (double)g_settings.clockFontSize * 0.23;
        if (dateFontSize < 18.0) dateFontSize = 18.0;

        // Atmospheric Shadow Layer for Date
        wuxc::TextBlock dateShadow;
        dateShadow.FontSize(dateFontSize);
        dateShadow.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
        dateShadow.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
        dateShadow.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(70, 0, 0, 0)));
        dateShadow.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
        dateShadow.TextAlignment(isBottomLeft ? wux::TextAlignment::Left : wux::TextAlignment::Center);
        dateShadow.Margin(wux::Thickness{ 0.0, 1.5, 0.0, 0.0 });
        dateLayerGrid.Children().Append(dateShadow);
        g_macosDateShadowBlock = dateShadow;

        // Foreground Date Text
        wuxc::TextBlock dateText;
        dateText.Name(L"WindhawkMacOSDateText");
        dateText.FontSize(dateFontSize);
        dateText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
        dateText.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
        dateText.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(240, 255, 255, 255)));
        dateText.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
        dateText.TextAlignment(isBottomLeft ? wux::TextAlignment::Left : wux::TextAlignment::Center);
        dateLayerGrid.Children().Append(dateText);
        g_macosDateTextBlock = dateText;

        verticalLayout.Children().Append(dateLayerGrid);
    }

    // 2. Giant macOS Typography Time Display (Thin/Light, San Francisco aesthetic via Segoe UI Variable Display Light)
    wuxc::Grid timeLayerGrid;
    timeLayerGrid.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);

    double timeFontSize = (double)g_settings.clockFontSize * 1.55;

    // Atmospheric Soft Shadow Layer for Time
    wuxc::TextBlock timeShadow;
    timeShadow.FontSize(timeFontSize);
    timeShadow.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    timeShadow.FontWeight(winrt::Windows::UI::Text::FontWeights::Light());
    timeShadow.Foreground(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 0, 0, 0)));
    timeShadow.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
    timeShadow.TextAlignment(isBottomLeft ? wux::TextAlignment::Left : wux::TextAlignment::Center);
    timeShadow.Margin(wux::Thickness{ 0.0, 3.5, 0.0, 0.0 });
    wuxd::Typography::SetNumeralAlignment(timeShadow, wux::FontNumeralAlignment::Tabular);
    timeLayerGrid.Children().Append(timeShadow);
    g_macosShadowTextBlock = timeShadow;

    // Foreground Pure White Clean Time Text
    wuxc::TextBlock timeText;
    timeText.Name(L"WindhawkMacOSTimeText");
    timeText.FontSize(timeFontSize);
    timeText.FontFamily(wuxm::FontFamily(L"Segoe UI Variable Display"));
    timeText.FontWeight(winrt::Windows::UI::Text::FontWeights::Light());
    timeText.Foreground(wuxm::SolidColorBrush(wu::Colors::White()));
    timeText.HorizontalAlignment(isBottomLeft ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Center);
    timeText.TextAlignment(isBottomLeft ? wux::TextAlignment::Left : wux::TextAlignment::Center);
    wuxd::Typography::SetNumeralAlignment(timeText, wux::FontNumeralAlignment::Tabular);
    timeLayerGrid.Children().Append(timeText);
    g_macosTimeTextBlock = timeText;

    verticalLayout.Children().Append(timeLayerGrid);

    // 3. macOS Sleek Battery Capsule (Underneath time if enabled)
    if (g_settings.showBatteryStatus) {
        auto batteryCapsule = CreateBatteryCapsule(isBottomLeft);
        if (batteryCapsule) {
            verticalLayout.Children().Append(batteryCapsule);
        }
    }

    rootContainer.Children().Append(verticalLayout);
    UpdateTimeAndDate();

    return rootContainer;
}

// Factory function for selected clock style
wux::UIElement BuildClockElement() {
    if (g_settings.clockStyle == L"macos") {
        return BuildMacOSClockElement();
    } else if (g_settings.clockStyle == L"pixel") {
        return BuildPixelClockElement();
    } else if (g_settings.clockStyle == L"flip") {
        return BuildFlipClockElement();
    } else if (g_settings.clockStyle == L"vintage") {
        return BuildVintageClockElement();
    } else if (g_settings.clockStyle == L"cyberpunk") {
        return BuildCyberpunkClockElement();
    } else if (g_settings.clockStyle == L"aura") {
        return BuildAuraClockElement();
    }
    return BuildPixelClockElement();
}

// ---------------------------------------------------------------------------
// UI thread discovery and synchronous dispatch
// ---------------------------------------------------------------------------

std::atomic<HWND> g_coreWindow{ nullptr };
std::atomic<HHOOK> g_getMessageHook{ nullptr };
std::atomic<bool> g_settingsDirty{ false };
HANDLE g_stopEvent = nullptr;
HANDLE g_workerThread = nullptr;

UINT g_runOnUiThreadMessage = 0;

using RunOnUiThreadProc = void(WINAPI*)(PVOID);

struct RunOnUiThreadParam {
    RunOnUiThreadProc proc;
    PVOID param;
    bool ran;
};

// WH_CALLWNDPROC hook used by RunOnUiThread(). It runs on the target UI thread
// before the window procedure sees the message.
LRESULT CALLBACK RunOnUiThreadHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp && cwp->message == g_runOnUiThreadMessage) {
            auto* p = reinterpret_cast<RunOnUiThreadParam*>(cwp->lParam);
            if (p && p->proc) {
                try {
                    p->proc(p->param);
                    p->ran = true;
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"UI task failed: %s", e.message().c_str());
                } catch (...) {
                    Wh_Log(L"UI task failed with an unknown error.");
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Runs proc on the thread that owns hWnd and returns only after it has
// finished. Mixing this with queued dispatcher callbacks is not allowed: the
// synchronous path is the only way to guarantee that no mod code is left
// behind when the DLL is unloaded.
bool RunOnUiThread(HWND hWnd, RunOnUiThreadProc proc, PVOID param) {
    if (!hWnd || !proc) {
        return false;
    }

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    static std::mutex s_runMutex;
    std::lock_guard<std::mutex> lock(s_runMutex);

    if (!g_runOnUiThreadMessage) {
        return false;
    }

    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, RunOnUiThreadHookProc, nullptr, threadId);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx(WH_CALLWNDPROC) failed: %u", GetLastError());
        return false;
    }

    RunOnUiThreadParam p{ proc, param, false };
    SendMessageW(hWnd, g_runOnUiThreadMessage, 0, reinterpret_cast<LPARAM>(&p));
    UnhookWindowsHookEx(hook);
    return p.ran;
}

// ---------------------------------------------------------------------------
// Visual tree helpers
// ---------------------------------------------------------------------------

bool IsClockAttached() {
    if (!g_activeClockElement) {
        return false;
    }
    try {
        return wuxm::VisualTreeHelper::GetParent(g_activeClockElement) != nullptr;
    } catch (...) {
        return false;
    }
}

// Restores the local Visibility value that existed before this mod hid an
// element. ClearValue() is used when the element had no local value, which
// keeps Windows' own bindings and visual states intact.
void RestoreTrackedElements(std::vector<TrackedElement>& tracked) {
    auto prop = wux::UIElement::VisibilityProperty();
    for (auto& trackedElement : tracked) {
        if (!trackedElement.element) {
            continue;
        }
        try {
            if (trackedElement.hadLocalValue) {
                trackedElement.element.SetValue(prop, trackedElement.originalValue);
            } else {
                trackedElement.element.ClearValue(prop);
            }
        } catch (...) {
            // If restoring the saved value fails, at least remove our local
            // value so Windows can re-evaluate its own bindings and states.
            try {
                trackedElement.element.ClearValue(prop);
            } catch (...) {}
        }
    }
    tracked.clear();
}

// Keeps the tracking list in sync with the current tree and hides only the
// elements that are actually supposed to be hidden.
void SyncTrackedElements(std::vector<TrackedElement>& tracked,
                         std::vector<wux::FrameworkElement> const& found,
                         bool shouldHide) {
    tracked.erase(
        std::remove_if(tracked.begin(), tracked.end(),
                       [&found](TrackedElement const& trackedElement) {
                           if (!trackedElement.element) {
                               return true;
                           }
                           return std::none_of(
                               found.begin(), found.end(),
                               [&trackedElement](wux::FrameworkElement const& element) {
                                   return element == trackedElement.element;
                               });
                       }),
        tracked.end());

    if (!shouldHide) {
        RestoreTrackedElements(tracked);
        return;
    }

    auto prop = wux::UIElement::VisibilityProperty();
    for (auto const& element : found) {
        if (!element) {
            continue;
        }

        bool alreadyTracked = std::any_of(
            tracked.begin(), tracked.end(),
            [&element](TrackedElement const& trackedElement) {
                return trackedElement.element == element;
            });
        if (alreadyTracked) {
            continue;
        }

        TrackedElement trackedElement;
        trackedElement.element = element;
        try {
            auto localValue = element.ReadLocalValue(prop);
            trackedElement.hadLocalValue =
                !(localValue == wux::DependencyProperty::UnsetValue());
            if (trackedElement.hadLocalValue) {
                trackedElement.originalValue = localValue;
            }
        } catch (...) {
            trackedElement.hadLocalValue = false;
        }

        tracked.push_back(trackedElement);
        try {
            element.Visibility(wux::Visibility::Collapsed);
        } catch (...) {}
    }

    // Re-assert the value on every scan. Local values normally win over visual
    // states, but re-applying is cheap and keeps the element hidden if Windows
    // replaces the tree or resets the property.
    for (auto& trackedElement : tracked) {
        try {
            trackedElement.element.Visibility(wux::Visibility::Collapsed);
        } catch (...) {}
    }
}

// ---------------------------------------------------------------------------
// Clock element management
// ---------------------------------------------------------------------------

void RemoveActiveClockElement() {
    if (!g_activeClockElement) {
        return;
    }
    if (g_lockRootGrid) {
        uint32_t index = 0;
        try {
            if (g_lockRootGrid.Children().IndexOf(g_activeClockElement, index)) {
                g_lockRootGrid.Children().RemoveAt(index);
            }
        } catch (...) {}
    }
    g_activeClockElement = nullptr;
}

// Releases every strong XAML reference. Must run on the lock screen UI thread.
void ResetClockElementReferences() {
    g_hourTextBlock = nullptr;
    g_hourBotTextBlock = nullptr;
    g_minTextBlock = nullptr;
    g_minBotTextBlock = nullptr;
    g_secTextBlock = nullptr;
    g_ampmTextBlock = nullptr;
    g_auraTimeTextBlock = nullptr;
    g_auraShadowTextBlock = nullptr;
    g_macosTimeTextBlock = nullptr;
    g_macosShadowTextBlock = nullptr;
    g_macosDateTextBlock = nullptr;
    g_macosDateShadowBlock = nullptr;
    g_batteryCapsule = nullptr;
    g_batteryIconText = nullptr;
    g_batteryStatusText = nullptr;
    g_cyberSlash1 = nullptr;
    g_cyberSlash2 = nullptr;
    g_cyberLiveLabel = nullptr;
    g_dateTextBlock = nullptr;

    for (int i = 0; i < 6; i++) {
        g_pixelDigits[i].dots.clear();
        g_pixelDigits[i].lastVal = -1;
    }
    g_brushActiveDot = nullptr;
    g_brushInactiveDot = nullptr;
}

void RestoreDefaultElements() {
    RestoreTrackedElements(g_hiddenClockElements);
    RestoreTrackedElements(g_hiddenWidgetElements);
}

struct ScanResult {
    wuxc::Grid root{ nullptr };
    bool hasLockRoot = false;
    bool hasMainPage = false;
    std::vector<wux::FrameworkElement> clockElements;
    std::vector<wux::FrameworkElement> widgetElements;
};

void ScanNode(wux::DependencyObject const& node, ScanResult& result, int depth) {
    if (!node || depth > 96) {
        return;
    }

    // Our own clock contains hundreds of Ellipses in the pixel style. There is
    // nothing to find inside it, so skip the whole subtree.
    if (g_activeClockElement && node == g_activeClockElement) {
        return;
    }

    if (auto element = node.try_as<wux::FrameworkElement>()) {
        auto name = element.Name();

        if (name == L"LockRootGrid") {
            if (!result.hasLockRoot) {
                if (auto grid = element.try_as<wuxc::Grid>()) {
                    result.root = grid;
                    result.hasLockRoot = true;
                }
            }
        } else if (name == L"MainPageContainer") {
            if (!result.hasMainPage) {
                if (auto grid = element.try_as<wuxc::Grid>()) {
                    if (!result.hasLockRoot) {
                        result.root = grid;
                    }
                    result.hasMainPage = true;
                }
            }
        } else if (name == L"TimeAndDatePanel" || name == L"Time" || name == L"Date") {
            result.clockElements.push_back(element);
        } else if (name == L"AdaptiveCardDetailContainer" ||
                   name == L"DetailTextPanel" || name == L"LockWidgetCanvas") {
            result.widgetElements.push_back(element);
        }
    }

    int childCount = 0;
    try {
        childCount = wuxm::VisualTreeHelper::GetChildrenCount(node);
    } catch (...) {
        return;
    }
    for (int i = 0; i < childCount; i++) {
        try {
            ScanNode(wuxm::VisualTreeHelper::GetChild(node, i), result, depth + 1);
        } catch (...) {}
    }
}

// Finds the lock screen root and the stock clock/widget elements, then injects
// or refreshes the selected clock. Must run on the lock screen UI thread.
void AttachToTree() {
    auto content = wux::Window::Current().Content();
    if (!content) {
        return;
    }

    ScanResult scan;
    ScanNode(content, scan, 0);
    if (!scan.root) {
        // The tree is still being built; the next tick will retry.
        return;
    }

    bool sameRoot = (g_lockRootGrid == scan.root);
    if (!sameRoot) {
        RemoveActiveClockElement();
        ResetClockElementReferences();
        g_lockRootGrid = scan.root;
        g_timeCache.Invalidate();
        Wh_Log(L"Attached to the lock screen visual tree.");
    }

    bool hideClock =
        g_settings.hideDefaultClock && (g_settings.clockStyle != L"defaultOnly");
    SyncTrackedElements(g_hiddenClockElements, scan.clockElements, hideClock);
    SyncTrackedElements(g_hiddenWidgetElements, scan.widgetElements,
                        g_settings.hideDefaultWidgets);

    if (g_settings.clockStyle == L"defaultOnly") {
        RemoveActiveClockElement();
        g_timeCache.Invalidate();
        return;
    }

    if (!g_activeClockElement || !IsClockAttached()) {
        RemoveActiveClockElement();
        ResetClockElementReferences();
        g_timeCache.Invalidate();

        try {
            g_activeClockElement = BuildClockElement();
            if (g_activeClockElement) {
                g_lockRootGrid.Children().Append(g_activeClockElement);
                Wh_Log(L"Clock injected: style=%s, seconds=%d",
                       g_settings.clockStyle.c_str(), g_settings.showSeconds);
            }
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Failed to inject clock: %s", e.message().c_str());
            g_activeClockElement = nullptr;
        } catch (...) {
            Wh_Log(L"Failed to inject clock: unknown error.");
            g_activeClockElement = nullptr;
        }
    }

    UpdateTimeAndDate();
}

// Reloads settings and rebuilds the clock on the UI thread.
void WINAPI RebuildOnUiThread(PVOID) {
    g_settingsDirty = false;
    LoadSettings();
    RestoreDefaultElements();
    RemoveActiveClockElement();
    ResetClockElementReferences();
    g_timeCache.Invalidate();
    g_lockRootGrid = nullptr;
    AttachToTree();
    Wh_Log(L"Settings applied on the lock screen UI thread.");
}

// ---------------------------------------------------------------------------
// Periodic UI work
// ---------------------------------------------------------------------------

ULONGLONG g_lastUiTick = 0;
ULONGLONG g_lastFullScan = 0;

void OnUiThreadTick() {
    ULONGLONG now = GetTickCount64();
    int intervalMs = g_settings.showSeconds ? 500 : 1000;
    if ((now - g_lastUiTick) < static_cast<ULONGLONG>(intervalMs)) {
        return;
    }
    g_lastUiTick = now;

    if (g_settingsDirty.exchange(false)) {
        try {
            RebuildOnUiThread(nullptr);
        } catch (...) {
            // Keep the request alive so the next tick retries the rebuild.
            g_settingsDirty = true;
            throw;
        }
        return;
    }

    bool hasClockStyle = (g_settings.clockStyle != L"defaultOnly");
    bool needScan = (!g_lockRootGrid || (hasClockStyle && !g_activeClockElement) ||
                     (now - g_lastFullScan) >= 3000);
    if (needScan) {
        g_lastFullScan = now;
        AttachToTree();
    } else if (hasClockStyle) {
        UpdateTimeAndDate();
    }
}

LRESULT CALLBACK GetMessageHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        try {
            OnUiThreadTick();
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"UI tick failed: %s", e.message().c_str());
        } catch (...) {
            Wh_Log(L"UI tick failed with an unknown error.");
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Teardown and worker thread
// ---------------------------------------------------------------------------

void WINAPI TeardownOnUiThread(PVOID) {
    // Unhook from the UI thread: no WH_GETMESSAGE callback can be executing at
    // the same time on this thread, so it is safe for the DLL to unload next.
    HHOOK hook = g_getMessageHook.exchange(nullptr);
    if (hook) {
        UnhookWindowsHookEx(hook);
    }

    RestoreDefaultElements();
    RemoveActiveClockElement();
    ResetClockElementReferences();
    g_lockRootGrid = nullptr;
    g_timeCache.Invalidate();
    Wh_Log(L"UI teardown complete.");
}

BOOL CALLBACK FindCoreWindowProc(HWND hWnd, LPARAM lParam) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId != GetCurrentProcessId()) {
        return TRUE;
    }

    WCHAR className[128];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) > 0 &&
        _wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindCoreWindow() {
    HWND found = nullptr;
    EnumWindows(FindCoreWindowProc, reinterpret_cast<LPARAM>(&found));
    return found;
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    // Wait for LockApp's CoreWindow, then install the message hook on its
    // thread. Retry until the window exists or the mod is unloaded.
    while (WaitForSingleObject(g_stopEvent, 250) == WAIT_TIMEOUT) {
        if (g_getMessageHook.load()) {
            break;
        }

        HWND window = FindCoreWindow();
        if (!window) {
            continue;
        }

        DWORD threadId = GetWindowThreadProcessId(window, nullptr);
        HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, GetMessageHookProc, nullptr, threadId);
        if (hook) {
            g_coreWindow = window;
            g_getMessageHook = hook;
            Wh_Log(L"Installed WH_GETMESSAGE hook on UI thread %u (hwnd=%p).",
                   threadId, window);
            break;
        }
        Wh_Log(L"SetWindowsHookEx(WH_GETMESSAGE) failed: %u", GetLastError());
    }

    // Wake the UI thread periodically. The hook itself throttles updates to
    // 500 ms when seconds are shown and to 1000 ms otherwise.
    while (WaitForSingleObject(g_stopEvent, 500) == WAIT_TIMEOUT) {
        HWND window = g_coreWindow.load();
        if (!window) {
            continue;
        }
        if (!IsWindow(window)) {
            Wh_Log(L"The lock screen window disappeared; stopping the worker.");
            break;
        }
        PostMessageW(window, WM_NULL, 0, 0);
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Lock Screen Clock Styler initializing (Wh_ModInit)...");

    LoadSettings();

    g_runOnUiThreadMessage =
        RegisterWindowMessageW(L"Windhawk_LockScreenClockStyler_RunOnUiThread_v1");
    if (!g_runOnUiThreadMessage) {
        Wh_Log(L"RegisterWindowMessageW failed: %u", GetLastError());
        return FALSE;
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());
        return FALSE;
    }

    g_coreWindow = nullptr;
    g_getMessageHook = nullptr;
    g_settingsDirty = false;

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Lock Screen Clock Styler uninitializing (Wh_ModUninit)...");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }

    HWND window = g_coreWindow.load();
    HHOOK hook = g_getMessageHook.load();
    if (window && hook && IsWindow(window)) {
        // Synchronous. When this returns, TeardownOnUiThread has finished and
        // the WH_GETMESSAGE hook has been removed from the UI thread.
        if (!RunOnUiThread(window, TeardownOnUiThread, nullptr)) {
            Wh_Log(L"Failed to run teardown on the UI thread.");
            HHOOK remainingHook = g_getMessageHook.exchange(nullptr);
            if (remainingHook) {
                UnhookWindowsHookEx(remainingHook);
            }
            // XAML objects are intentionally left alone here. They belong to
            // the UI thread and releasing them from this thread would be unsafe.
        }
    } else {
        HHOOK remainingHook = g_getMessageHook.exchange(nullptr);
        if (remainingHook) {
            UnhookWindowsHookEx(remainingHook);
        }
    }

    g_coreWindow = nullptr;
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    Wh_Log(L"Lock Screen Clock Styler uninitialized.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Mod settings changed (Wh_ModSettingsChanged)...");

    HWND window = g_coreWindow.load();
    HHOOK hook = g_getMessageHook.load();
    if (hook) {
        // The UI hook is active, so g_settings must only be modified on the
        // lock screen UI thread.
        if (window && IsWindow(window) &&
            RunOnUiThread(window, RebuildOnUiThread, nullptr)) {
            return;
        }
        // The UI thread did not process the synchronous task. Let the next
        // tick retry instead of touching XAML state or settings from here.
        Wh_Log(L"Deferring settings change to the UI thread.");
        g_settingsDirty = true;
    } else {
        // No visual tree yet, so nothing else can be reading these settings.
        LoadSettings();
    }
}
