// ==WindhawkMod==
// @id              lock-screen-clock-styler
// @name            Lock Screen Clock Styler
// @name:tr-TR      Kilit Ekranı Saat Özelleştirici
// @description     Customize the Windows 11 lock screen clock with modern styles, date display, and widget controls.
// @description:tr-TR Windows 11 kilit ekranı saatini modern stiller, tarih göstergesi ve widget denetimleriyle özelleştirin.
// @version         0.3.1
// @author          ali0zdas
// @github          https://github.com/ali0zdas
// @include         LockApp.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
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
- **Show Seconds Toggle:** Optional live seconds display (direct support on Cyberpunk & Retro Pixel; user-toggleable on Minimalist styles).
- **Frosted Glass / Acrylic Styling:** Translucent glass panels with subtle border highlights and rounded corners.
- **Dynamic Date Display:** Elegant date and day capsule positioned beneath or above the clock, adapting to the user's system locale.
- **Hide Default Widgets:** Clean up the lock screen by hiding MSN weather, sports, and news widgets.
- **Hide / Replace Default Clock:** Seamlessly replace the stock Windows 11 clock with custom styles or adjust vertical offsets.
- **Extensive Customization:** Card opacity, font size, vertical offsets, and alignments are fully customizable via settings.

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
    - defaultOnly: "Default Clock Only (Widgets Hidden)"
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
  $description: "Displays live seconds (direct support on Cyberpunk and Retro Pixel; optional on Minimalist styles)."
  $description:tr-TR: "Canlı saniye göstergesini etkinleştirir (Cyberpunk ve Retro Piksel stillerinde doğrudan etkindir; Minimalist stillerde isteğe bağlıdır)."
- glassOpacity: 45
  $name: "Glass Opacity (Liquid Glass Alpha)"
  $name:tr-TR: "Cam Opaklığı (Alfa Değeri)"
  $description: "Translucency level of the flip cards (0 to 255; 0 is completely transparent, 255 is fully opaque)."
  $description:tr-TR: "Flip kartlarının yarı saydamlık derecesi (0 ile 255 arası; 0 tamamen saydam, 255 tam opak)."
- clockFontSize: 92
  $name: "Digit Font Size"
  $name:tr-TR: "Rakam Font Boyutu"
  $description: "Font size for hour and minute digits."
  $description:tr-TR: "Saat ve dakika rakamlarının punto büyüklüğü."
- verticalOffset: 0
  $name: "Vertical Offset (px)"
  $name:tr-TR: "Dikey Konum Ofseti (px)"
  $description: "Offset to shift the clock vertically up or down (-200 to +400)."
  $description:tr-TR: "Saatin ekrandaki dikey konumunu yukarı veya aşağı kaydırma ofseti (-200 ile +400 arası)."
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
- hideDefaultWidgets: true
  $name: "Hide Default Widgets"
  $name:tr-TR: "Varsayılan Widget'ları Gizle"
  $description: "Hides MSN weather, sports, and news widget areas on the lock screen."
  $description:tr-TR: "Kilit ekranındaki MSN hava durumu, spor ve haber widget alanını gizler."
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
#include <xamlom.h>
#include <ocidl.h>
#include <Unknwn.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <algorithm>
#include <atomic>
#include <string>
#include <vector>
#include <chrono>

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

// Settings Loader
void LoadSettings() {
    static std::wstring s_lastKnownStyle = L"";
    static std::wstring s_lastKnownRawAlign = L"";
    static bool s_lastKnownRawSeconds = false;
    static bool s_settingsInitialized = false;

    PCWSTR styleStr = Wh_GetStringSetting(L"clockStyle");
    g_settings.clockStyle = (styleStr && *styleStr) ? styleStr : L"pixel";
    Wh_FreeStringSetting(styleStr);

    PCWSTR fmtStr = Wh_GetStringSetting(L"timeFormat");
    g_settings.timeFormat = (fmtStr && *fmtStr) ? fmtStr : L"24h";
    Wh_FreeStringSetting(fmtStr);

    bool themeChanged = (s_settingsInitialized && g_settings.clockStyle != s_lastKnownStyle);

    bool rawShowSeconds = Wh_GetIntSetting(L"showSeconds") != 0;
    bool rawSecondsToggled = (s_settingsInitialized && rawShowSeconds != s_lastKnownRawSeconds);
    bool isPixelOrCyber = (g_settings.clockStyle == L"pixel" || g_settings.clockStyle == L"cyberpunk");
    // Flip stilleri saniye göstergesini desteklemez; bu ayar onlarda etkisizdir.
    bool supportsSeconds = isPixelOrCyber || g_settings.clockStyle == L"macos" || g_settings.clockStyle == L"aura";

    if (!supportsSeconds) {
        g_settings.showSeconds = false;
    } else if (isPixelOrCyber) {
        g_settings.showSeconds = rawShowSeconds;
    } else {
        std::wstring storageKey = L"userSec_" + g_settings.clockStyle;

        if (!s_settingsInitialized || themeChanged) {
            // Tema değişti veya mod yeni yüklendi: yalnızca kullanıcının bu tema için
            // daha önce açıkça tercih ettiği değeri kullan.
            g_settings.showSeconds = rawShowSeconds && (Wh_GetIntValue(storageKey.c_str(), 0) != 0);
            if (rawSecondsToggled) {
                // Tema ve saniye ayarı birlikte değiştirildiyse tercihi yeni temaya yaz.
                Wh_SetIntValue(storageKey.c_str(), rawShowSeconds ? 1 : 0);
                g_settings.showSeconds = rawShowSeconds;
            }
        } else if (rawSecondsToggled) {
            // Kullanıcı saniye anahtarını değiştirdi: bu temanın tercihini kaydet.
            Wh_SetIntValue(storageKey.c_str(), rawShowSeconds ? 1 : 0);
            g_settings.showSeconds = rawShowSeconds;
        }
        // Aksi halde (alakasız bir ayar değişti) mevcut etkin değeri koru.
        // Böylece başka bir temada açık kalan saniye tercihi bu temaya sızmaz.
    }
    s_lastKnownRawSeconds = rawShowSeconds;

    g_settings.glassOpacity = Wh_GetIntSetting(L"glassOpacity");
    if (g_settings.glassOpacity < 0 || g_settings.glassOpacity > 255) {
        g_settings.glassOpacity = 45;
    }

    g_settings.clockFontSize = Wh_GetIntSetting(L"clockFontSize");
    if (g_settings.clockFontSize <= 0) {
        g_settings.clockFontSize = 92;
    }

    g_settings.verticalOffset = Wh_GetIntSetting(L"verticalOffset");

    // Hizalama: "auto" tema varsayılanını kullanır (Sonoma -> sol alt, diğerleri -> merkez).
    // Kullanıcının açıkça seçtiği bir değer ise her zaman doğrudan uygulanır.
    PCWSTR alignStr = Wh_GetStringSetting(L"clockAlignment");
    std::wstring rawAlign = (alignStr && *alignStr) ? alignStr : L"auto";
    Wh_FreeStringSetting(alignStr);

    if (rawAlign != L"auto" && rawAlign != L"center" && rawAlign != L"bottomLeft" &&
        rawAlign != L"left" && rawAlign != L"right") {
        rawAlign = L"auto";
    }

    if (rawAlign != s_lastKnownRawAlign && s_settingsInitialized) {
        Wh_Log(L"Clock alignment explicitly changed to: %s", rawAlign.c_str());
    }

    std::wstring defaultAlignForTheme = (g_settings.clockStyle == L"macos") ? L"bottomLeft" : L"center";
    g_settings.clockAlignment = (rawAlign == L"auto") ? defaultAlignForTheme : rawAlign;

    s_lastKnownRawAlign = rawAlign;
    s_lastKnownStyle = g_settings.clockStyle;
    s_settingsInitialized = true;

    g_settings.hideDefaultWidgets = Wh_GetIntSetting(L"hideDefaultWidgets") != 0;
    g_settings.hideDefaultClock = Wh_GetIntSetting(L"hideDefaultClock") != 0;
    g_settings.showDateCapsule = Wh_GetIntSetting(L"showDateCapsule") != 0;
    g_settings.showBatteryStatus = Wh_GetIntSetting(L"showBatteryStatus") != 0;

    Wh_Log(L"Settings loaded: style=%s, fmt=%s, showSec=%d, opacity=%d, font=%d, vertOffset=%d, align=%s, hideWidgets=%d, hideClock=%d, showBattery=%d",
           g_settings.clockStyle.c_str(), g_settings.timeFormat.c_str(), g_settings.showSeconds, g_settings.glassOpacity,
           g_settings.clockFontSize, g_settings.verticalOffset, g_settings.clockAlignment.c_str(), g_settings.hideDefaultWidgets, g_settings.hideDefaultClock, g_settings.showBatteryStatus);
}

// UI State References
wuxc::Grid g_lockRootGrid{ nullptr };
wux::UIElement g_activeClockElement{ nullptr };
wuxc::TextBlock g_hourTextBlock{ nullptr };
wuxc::TextBlock g_hourBotTextBlock{ nullptr };
wuxc::TextBlock g_minTextBlock{ nullptr };
wuxc::TextBlock g_minBotTextBlock{ nullptr };
wuxc::TextBlock g_secTextBlock{ nullptr };
wuxc::TextBlock g_ampmTextBlock{ nullptr };
wuxc::TextBlock g_auraTimeTextBlock{ nullptr };
wuxc::TextBlock g_auraShadowTextBlock{ nullptr };
wuxc::TextBlock g_dateTextBlock{ nullptr };
wux::DispatcherTimer g_clockTimer{ nullptr };
bool g_clockInjected = false;

// Minimalist Sonoma (Clean Typography) Theme State
wuxc::TextBlock g_macosTimeTextBlock{ nullptr };
wuxc::TextBlock g_macosShadowTextBlock{ nullptr };
wuxc::TextBlock g_macosDateTextBlock{ nullptr };
wuxc::TextBlock g_macosDateShadowBlock{ nullptr };

// Battery Capsule State
wuxc::Border g_batteryCapsule{ nullptr };
wuxc::TextBlock g_batteryIconText{ nullptr };
wuxc::TextBlock g_batteryStatusText{ nullptr };

// Cyberpunk HUD Pulsing State
wuxc::TextBlock g_cyberSlash1{ nullptr };
wuxc::TextBlock g_cyberSlash2{ nullptr };
wuxc::TextBlock g_cyberLiveLabel{ nullptr };

// Tracked default elements for restoring on uninit or settings change
std::vector<wux::FrameworkElement> g_hiddenClockElements;
std::vector<wux::FrameworkElement> g_hiddenWidgetElements;

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
PixelDigitGrid g_pixelDigits[6]; // H1, H2, M1, M2, S1, S2
wuxm::SolidColorBrush g_brushActiveDot{ nullptr };
wuxm::SolidColorBrush g_brushInactiveDot{ nullptr };

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
        // BatteryFlag: 128 = No system battery, 255 = Unknown status
        if ((sps.BatteryFlag & 128) == 0 && sps.BatteryFlag != 255 && sps.BatteryLifePercent <= 100) {
            info.hasBattery = true;
            info.percentage = (int)sps.BatteryLifePercent;
            info.isCharging = (sps.ACLineStatus == 1) || ((sps.BatteryFlag & 8) != 0);
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
        g_timeCache.lastAmPm = ampmStr;
    }

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
                    swprintf_s(timeBuf, L"%02d:%02d:%02d %s", displayHour, st.wMinute, st.wSecond, ampmStr);
                } else {
                    swprintf_s(timeBuf, L"%02d:%02d %s", displayHour, st.wMinute, ampmStr);
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
            if (is12h && (g_settings.clockStyle == L"flip" || g_settings.clockStyle == L"vintage")) {
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

    if (g_dateTextBlock && g_settings.showDateCapsule && (dateChanged || g_timeCache.lastDateStr.empty())) {
        WCHAR dateBuf[128];
        if (GetDateFormatW(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, nullptr, dateBuf, ARRAYSIZE(dateBuf)) > 0) {
            g_dateTextBlock.Text(dateBuf);
            g_timeCache.lastDateStr = dateBuf;
        }
        g_timeCache.lastDay = st.wDay;
        g_timeCache.lastMonth = st.wMonth;
        g_timeCache.lastYear = st.wYear;
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

// Start Live Clock Timer with adaptive throttling (500ms when seconds active, 1000ms otherwise)
void StartClockTimer() {
    if (g_clockTimer) {
        g_clockTimer.Stop();
        g_clockTimer = nullptr;
    }
    g_clockTimer = wux::DispatcherTimer();
    int intervalMs = g_settings.showSeconds ? 500 : 1000;
    g_clockTimer.Interval(std::chrono::milliseconds(intervalMs));
    g_clockTimer.Tick([](auto&&, auto&&) {
        UpdateTimeAndDate();
    });
    g_clockTimer.Start();
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

    // Segoe UI Variable ExtraBold için optik merkezleme payı (rakamı tam kesik çizgisine oturtarak 50/50 böler)
    double opticalOffset = fontSize * 0.045;

    // --- 1. ÜST KANAT (Üst yarı kart & rakamlar) ---
    wuxc::Grid topContainer;
    topContainer.Width(width);
    topContainer.Height(height);

    wuxm::RectangleGeometry topClip;
    topClip.Rect(winrt::Windows::Foundation::Rect{ 0.0f, 0.0f, (float)width, (float)halfHeight });
    topContainer.Clip(topClip);

    // Üst Kanat Akrilik Gövde & Kenarlık (üst köşeler yuvarlatılmış, iç kesim 2px, alt kenar açık)
    wuxc::Border topFlap;
    topFlap.Width(width);
    topFlap.Height(halfHeight);
    topFlap.VerticalAlignment(wux::VerticalAlignment::Top);
    topFlap.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.glassOpacity, 255, 255, 255)));
    topFlap.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 255, 255, 255)));
    topFlap.BorderThickness(wux::Thickness{ 1.5, 1.5, 1.5, 0.0 });
    topFlap.CornerRadius(wux::CornerRadiusHelper::FromRadii(cornerRadius, cornerRadius, 2.0, 2.0));
    topContainer.Children().Append(topFlap);

    // Üst Rakam: Kartın merkezine hizalı, alt yarısı yarıktan itibaren kırpılır
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

    // Saat Kartındaki AM/PM Göstergesi (12 saat formatı, üst kanat sol üstünde)
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

    // --- 2. ALT KANAT (Alt yarı kart & rakamlar) ---
    wuxc::Grid botContainer;
    botContainer.Width(width);
    botContainer.Height(height);

    wuxm::RectangleGeometry botClip;
    botClip.Rect(winrt::Windows::Foundation::Rect{ 0.0f, (float)(halfHeight + seamGap), (float)width, (float)halfHeight });
    botContainer.Clip(botClip);

    // Alt Kanat Akrilik Gövde & Kenarlık (iç kesim 2px, alt köşeler yuvarlatılmış, üst kenar açık)
    wuxc::Border bottomFlap;
    bottomFlap.Width(width);
    bottomFlap.Height(halfHeight);
    bottomFlap.VerticalAlignment(wux::VerticalAlignment::Bottom);
    bottomFlap.Background(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb((uint8_t)g_settings.glassOpacity, 255, 255, 255)));
    bottomFlap.BorderBrush(wuxm::SolidColorBrush(wu::ColorHelper::FromArgb(80, 255, 255, 255)));
    bottomFlap.BorderThickness(wux::Thickness{ 1.5, 0.0, 1.5, 1.5 });
    bottomFlap.CornerRadius(wux::CornerRadiusHelper::FromRadii(2.0, 2.0, cornerRadius, cornerRadius));
    botContainer.Children().Append(bottomFlap);

    // Alt Rakam: Üst rakamla birebir aynı merkez koordinatlarında, üst yarısı yarıktan itibaren kırpılır
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
    StartClockTimer();

    return rootContainer;
}

// Build Single Vintage Flip Card Element (Retro Mechanical Airport/Station Solari Style)
wuxc::Grid CreateVintageFlipCard(double width, double height, double cornerRadius, double fontSize, wuxc::TextBlock& outText, const wchar_t* textName, bool isHourCard = false) {
    wuxc::Grid card;
    card.Width(width);
    card.Height(height);

    // Fliqlo Matte Charcoal Black Flap Body (#0e0e0e) - Çerçevesiz mat gövde
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
    StartClockTimer();

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
    StartClockTimer();

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
    StartClockTimer();

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
    StartClockTimer();

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
    StartClockTimer();

    return rootContainer;
}

// Factory function for selected clock style
wux::UIElement BuildClockElement() {
    if (g_settings.clockStyle == L"macos") {
        return BuildMacOSClockElement();
    } else if (g_settings.clockStyle == L"pixel") {
        return BuildPixelClockElement();
    } else if (g_settings.clockStyle == L"vintage") {
        return BuildVintageClockElement();
    } else if (g_settings.clockStyle == L"cyberpunk") {
        return BuildCyberpunkClockElement();
    } else if (g_settings.clockStyle == L"aura") {
        return BuildAuraClockElement();
    }
    return BuildPixelClockElement();
}

// Update visibility of default Windows clock and widget elements dynamically
void UpdateDefaultElementsVisibility() {
    bool hideClock = g_settings.hideDefaultClock && (g_settings.clockStyle != L"defaultOnly");
    auto clockVis = hideClock ? wux::Visibility::Collapsed : wux::Visibility::Visible;
    for (auto& elem : g_hiddenClockElements) {
        if (elem) {
            try { elem.Visibility(clockVis); } catch (...) {}
        }
    }

    auto widgetVis = g_settings.hideDefaultWidgets ? wux::Visibility::Collapsed : wux::Visibility::Visible;
    for (auto& elem : g_hiddenWidgetElements) {
        if (elem) {
            try { elem.Visibility(widgetVis); } catch (...) {}
        }
    }
}

// Enjekte edilen saat öğesini barındırıcı grid'den ayırır (yalnızca XAML UI thread'inde).
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

// Saat zamanlayıcısını güvenle durdurur.
void StopClockTimer() {
    if (g_clockTimer) {
        try { g_clockTimer.Stop(); } catch (...) {}
        g_clockTimer = nullptr;
    }
}

// Tüm XAML öğe referanslarını sıfırlar (yalnızca XAML UI thread'inde).
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
}

// Ayar değişikliğinde UI'ı yeniden kurar. Yalnızca XAML UI thread'inde çağrılmalıdır.
void RebuildClockUIOnUIThread() {
    if (!g_lockRootGrid) return;

    UpdateDefaultElementsVisibility();
    RemoveActiveClockElement();
    ResetClockElementReferences();
    g_timeCache.Invalidate();

    if (g_settings.clockStyle != L"defaultOnly") {
        try {
            g_activeClockElement = BuildClockElement();
            g_lockRootGrid.Children().Append(g_activeClockElement);
            g_clockInjected = true;
            Wh_Log(L"Clock UI successfully rebuilt for style: %s (showSec=%d)",
                   g_settings.clockStyle.c_str(), g_settings.showSeconds);
        } catch (...) {
            Wh_Log(L"Failed to rebuild clock UI for style: %s", g_settings.clockStyle.c_str());
            g_activeClockElement = nullptr;
            g_clockInjected = false;
            StopClockTimer();
        }
    } else {
        // Varsayılan saat modunda özel saat yoktur; boşuna çalışan timer'ı durdur.
        StopClockTimer();
        g_clockInjected = false;
    }
}

// Visual Tree Watcher (IVisualTreeServiceCallback2)
class VisualTreeWatcher : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile>
{
public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site) :
        m_XamlDiagnostics(site.as<IXamlDiagnostics>())
    {
        Wh_Log(L"Initializing VisualTreeWatcher...");

        HANDLE thread = CreateThread(
            nullptr, 0,
            [](LPVOID lpParam) WINAPI -> DWORD {
                auto watcher = reinterpret_cast<VisualTreeWatcher*>(lpParam);
                HRESULT hr = watcher->m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(watcher);
                watcher->Release();
                if (FAILED(hr)) {
                    Wh_Log(L"AdviseVisualTreeChange failed: %08X", hr);
                } else {
                    Wh_Log(L"AdviseVisualTreeChange succeeded.");
                }
                return 0;
            },
            this, 0, nullptr);
        if (thread) {
            AddRef();
            CloseHandle(thread);
        }
    }

    ~VisualTreeWatcher() {
        Wh_Log(L"VisualTreeWatcher destroyed.");
    }

    void Unadvise() {
        if (m_XamlDiagnostics) {
            m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
        }
    }

private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation, VisualElement element, VisualMutationType mutationType) override try
    {
        if (mutationType == Add)
        {
            winrt::Windows::Foundation::IInspectable obj;
            HRESULT hr = m_XamlDiagnostics->GetIInspectableFromHandle(element.Handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj)));
            if (SUCCEEDED(hr) && obj)
            {
                auto frameworkElement = obj.try_as<wux::FrameworkElement>();
                if (frameworkElement)
                {
                    std::wstring name = frameworkElement.Name().c_str();

                    // 1. Lock Screen Root Grid: LockRootGrid / MainPageContainer
                    if (name == L"LockRootGrid" || name == L"MainPageContainer")
                    {
                        auto grid = frameworkElement.try_as<wuxc::Grid>();
                        if (grid)
                        {
                            bool sameHost = (g_lockRootGrid == grid);
                            bool isClockAttached = false;
                            if (sameHost && g_activeClockElement)
                            {
                                uint32_t idx = 0;
                                try {
                                    isClockAttached = grid.Children().IndexOf(g_activeClockElement, idx);
                                } catch (...) {}
                            }

                            if (!sameHost || !isClockAttached)
                            {
                                // Yeni bir kök ağaç ya da saat kopmuş: aynı anda yalnızca tek
                                // saat örneği kalması için önce eski barındırıcıdan ayır.
                                Wh_Log(L"Found lock screen root grid: %s (isAttached=%d, injected=%d). Injecting Clock...",
                                       name.c_str(), isClockAttached, g_clockInjected);
                                RemoveActiveClockElement();
                                g_lockRootGrid = grid;
                                ResetClockElementReferences();
                                g_timeCache.Invalidate();

                                if (g_settings.clockStyle != L"defaultOnly")
                                {
                                    try {
                                        g_activeClockElement = BuildClockElement();
                                        grid.Children().Append(g_activeClockElement);
                                        g_clockInjected = true;
                                        Wh_Log(L"Clock successfully injected! Style: %s", g_settings.clockStyle.c_str());
                                    } catch (...) {
                                        Wh_Log(L"Failed to inject clock for style: %s", g_settings.clockStyle.c_str());
                                        g_activeClockElement = nullptr;
                                        g_clockInjected = false;
                                        StopClockTimer();
                                    }
                                }
                                else
                                {
                                    StopClockTimer();
                                    g_clockInjected = false;
                                }
                            }
                        }
                    }

                    // 2. Default Time & Date Panel: TimeAndDatePanel
                    if (name == L"TimeAndDatePanel" || name == L"Time" || name == L"Date")
                    {
                        bool alreadyTracked = false;
                        for (auto& tracked : g_hiddenClockElements) {
                            if (tracked == frameworkElement) {
                                alreadyTracked = true;
                                break;
                            }
                        }
                        if (!alreadyTracked) {
                            g_hiddenClockElements.push_back(frameworkElement);
                        }

                        if (g_settings.hideDefaultClock && g_settings.clockStyle != L"defaultOnly")
                        {
                            Wh_Log(L"Hiding default clock/date element: %s", name.c_str());
                            frameworkElement.Visibility(wux::Visibility::Collapsed);
                        }
                    }

                    // 3. Default Widget / Weather Panels
                    if (name == L"AdaptiveCardDetailContainer" || name == L"DetailTextPanel" || name == L"LockWidgetCanvas")
                    {
                        bool alreadyTracked = false;
                        for (auto& tracked : g_hiddenWidgetElements) {
                            if (tracked == frameworkElement) {
                                alreadyTracked = true;
                                break;
                            }
                        }
                        if (!alreadyTracked) {
                            g_hiddenWidgetElements.push_back(frameworkElement);
                        }

                        if (g_settings.hideDefaultWidgets)
                        {
                            Wh_Log(L"Hiding lock screen widget/weather area: %s", name.c_str());
                            frameworkElement.Visibility(wux::Visibility::Collapsed);
                        }
                    }
                }
            }
        }
        else if (mutationType == Remove)
        {
            winrt::Windows::Foundation::IInspectable obj;
            HRESULT hr = m_XamlDiagnostics->GetIInspectableFromHandle(element.Handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj)));
            if (SUCCEEDED(hr) && obj)
            {
                auto frameworkElement = obj.try_as<wux::FrameworkElement>();
                if (frameworkElement)
                {
                    std::wstring name = frameworkElement.Name().c_str();

                    if (frameworkElement == g_activeClockElement)
                    {
                        Wh_Log(L"Injected clock element removed from visual tree.");
                        g_activeClockElement = nullptr;
                        g_clockInjected = false;
                        StopClockTimer();
                    }

                    // Ölü ağaç referanslarını takip listelerinden çıkar (bellek sızıntısını önler).
                    g_hiddenClockElements.erase(
                        std::remove(g_hiddenClockElements.begin(), g_hiddenClockElements.end(), frameworkElement),
                        g_hiddenClockElements.end());
                    g_hiddenWidgetElements.erase(
                        std::remove(g_hiddenWidgetElements.begin(), g_hiddenWidgetElements.end(), frameworkElement),
                        g_hiddenWidgetElements.end());

                    if (name == L"LockRootGrid" || name == L"MainPageContainer")
                    {
                        Wh_Log(L"Lock screen root removed: %s", name.c_str());
                        if (g_lockRootGrid == frameworkElement) {
                            g_lockRootGrid = nullptr;
                        }
                        g_activeClockElement = nullptr;
                        g_clockInjected = false;
                        StopClockTimer();
                        g_hiddenClockElements.clear();
                        g_hiddenWidgetElements.clear();
                    }
                }
            }
        }
        return S_OK;
    }
    catch (...)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept override
    {
        return S_OK;
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics{ nullptr };
};

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher{ nullptr };

// {C85D8CC7-5463-40E8-A432-F5916B6427E5}
static constexpr CLSID CLSID_WindhawkTAP = { 0xc85d8cc7, 0x5463, 0x40e8, { 0xa4, 0x32, 0xf5, 0x91, 0x6b, 0x64, 0x27, 0xe5 } };

class WindhawkTAP : public winrt::implements<WindhawkTAP, IObjectWithSite, winrt::non_agile>
{
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown *pUnkSite) override try
    {
        if (g_visualTreeWatcher) {
            g_visualTreeWatcher->Unadvise();
            g_visualTreeWatcher = nullptr;
        }

        site.copy_from(pUnkSite);

        if (site) {
            HMODULE hSelf = nullptr;
            if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)&LoadSettings, &hSelf)) {
                // Balance GetModuleHandleExW and InitializeXamlDiagnosticsEx references
                FreeLibrary(hSelf);
                FreeLibrary(hSelf);
            }
            g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
        }
        return S_OK;
    }
    catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void **ppvSite) noexcept override {
        return site.as(riid, ppvSite);
    }

private:
    winrt::com_ptr<IUnknown> site;
};

template<class T>
struct SimpleFactory : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile>
{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override try
    {
        if (!pUnkOuter) {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        }
        return CLASS_E_NOAGGREGATION;
    }
    catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try
{
    if (rclsid == CLSID_WindhawkTAP) {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}
catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllCanUnloadNow(void) {
    if (winrt::get_module_lock()) {
        return S_FALSE;
    }
    return S_OK;
}

#pragma clang diagnostic pop

HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&GetCurrentModuleHandle, &module);
    return module;
}

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX = decltype(&InitializeXamlDiagnosticsEx);

std::atomic<bool> g_tapInjected = false;

HRESULT InjectWindhawkTAP() noexcept
{
    if (g_tapInjected.exchange(true)) {
        return S_OK; // Already injected
    }

    HMODULE module = GetCurrentModuleHandle();
    if (!module) {
        Wh_Log(L"GetCurrentModuleHandle failed: %u", GetLastError());
        g_tapInjected = false;
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    if (GetModuleFileNameW(module, location, ARRAYSIZE(location)) == 0) {
        Wh_Log(L"GetModuleFileNameW failed: %u", GetLastError());
        g_tapInjected = false;
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux = LoadLibraryExW(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!wux) {
        Wh_Log(L"Failed to load Windows.UI.Xaml.dll: %u", GetLastError());
        g_tapInjected = false;
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) {
        Wh_Log(L"InitializeXamlDiagnosticsEx export not found!");
        g_tapInjected = false;
        return HRESULT_FROM_WIN32(GetLastError());
    }

    Wh_Log(L"Starting InitializeXamlDiagnosticsEx connection loop (TAP DLL: %s)...", location);
    
    HRESULT hr = E_FAIL;
    for (int i = 0; i < 100; i++)
    {
        WCHAR connectionName[256];
        wsprintfW(connectionName, L"VisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), L"", location, CLSID_WindhawkTAP, nullptr);
        if (SUCCEEDED(hr))
        {
            Wh_Log(L"InitializeXamlDiagnosticsEx successfully connected! (%s, hr=%08X)", connectionName, hr);
            return hr;
        }
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            // Beklenmeyen hata: tüm isimleri denemek yerine erkenden çık,
            // ancak bayrağı sıfırla ki sonraki fırsatta yeniden denenebilsin.
            Wh_Log(L"InitializeXamlDiagnosticsEx failed unexpectedly (%s, hr=%08X)", connectionName, hr);
            break;
        }
    }

    Wh_Log(L"InitializeXamlDiagnosticsEx failed across all connection names (hr=%08X)", hr);
    g_tapInjected = false;
    return hr;
}

// Window Creation Hooks (Catching CoreWindow)
void CheckAndInjectForWindow(HWND hWnd, LPCWSTR lpClassName, PCSTR sourceFunc) {
    if (!hWnd || !lpClassName) return;

    BOOL bTextual = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextual && _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        Wh_Log(L"CoreWindow detected: %08X (Source: %S). Injecting TAP...", (DWORD)(ULONG_PTR)hWnd, sourceFunc);
        InjectWindhawkTAP();
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd) {
        CheckAndInjectForWindow(hWnd, lpClassName, __FUNCTION__);
    }
    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID, DWORD);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                    HINSTANCE hInstance, PVOID lpParam, DWORD dwBand)
{
    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (hWnd) {
        CheckAndInjectForWindow(hWnd, lpClassName, __FUNCTION__);
    }
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, PVOID, DWORD, DWORD);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                      int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                      HINSTANCE hInstance, PVOID lpParam, DWORD dwBand, DWORD dwTypeFlags)
{
    HWND hWnd = CreateWindowInBandEx_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (hWnd) {
        CheckAndInjectForWindow(hWnd, lpClassName, __FUNCTION__);
    }
    return hWnd;
}

// Library Load Hook (Catching Windows.UI.Xaml.dll readiness)
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && lpLibFileName) {
        if (wcsstr(lpLibFileName, L"Windows.UI.Xaml.dll") || wcsstr(lpLibFileName, L"windows.ui.xaml.dll")) {
            Wh_Log(L"Windows.UI.Xaml.dll loaded, initiating TAP injection...");
            InjectWindhawkTAP();
        }
    }
    return module;
}

// Enumerate existing CoreWindow instances
void SearchExistingCoreWindows() {
    EnumWindows([](HWND hWnd, LPARAM) CALLBACK -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid == GetCurrentProcessId()) {
            WCHAR clsName[64];
            if (GetClassNameW(hWnd, clsName, ARRAYSIZE(clsName)) > 0) {
                if (_wcsicmp(clsName, L"Windows.UI.Core.CoreWindow") == 0) {
                    Wh_Log(L"Existing CoreWindow found: %08X", (DWORD)(ULONG_PTR)hWnd);
                    InjectWindhawkTAP();
                    return FALSE;
                }
            }
        }
        return TRUE;
    }, 0);
}

// Windhawk Lifecycle Functions
BOOL Wh_ModInit() {
    Wh_Log(L"Lock Screen Clock Styler initializing (Wh_ModInit)...");

    LoadSettings();

    // Hook window creation APIs
    Wh_SetFunctionHook((void*)CreateWindowExW,
                       (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    HMODULE user32Module = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        void* pCreateWindowInBand = (void*)GetProcAddress(user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            Wh_SetFunctionHook(pCreateWindowInBand,
                               (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }

        void* pCreateWindowInBandEx = (void*)GetProcAddress(user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx) {
            Wh_SetFunctionHook(pCreateWindowInBandEx,
                               (void*)CreateWindowInBandEx_Hook,
                               (void**)&CreateWindowInBandEx_Original);
        }
    }

    // Windows.UI.Xaml.dll loading hook
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (kernelBase) {
        auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(kernelBase, "LoadLibraryExW");
        if (pLoadLibraryExW) {
            Wh_SetFunctionHook((void*)pLoadLibraryExW,
                               (void*)LoadLibraryExW_Hook,
                               (void**)&LoadLibraryExW_Original);
        }
    }

    // If Windows.UI.Xaml.dll is already loaded, check directly
    if (GetModuleHandleW(L"Windows.UI.Xaml.dll")) {
        Wh_Log(L"Windows.UI.Xaml.dll already loaded.");
        SearchExistingCoreWindows();
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"Wh_ModAfterInit: Scanning for existing windows...");
    SearchExistingCoreWindows();
}

void Wh_ModUninit() {
    Wh_Log(L"Lock Screen Clock Styler uninitializing (Wh_ModUninit)...");
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->Unadvise();
        g_visualTreeWatcher = nullptr;
    }

    // Enjekte edilen UI'ı temizle ve gizlenen varsayılan saat/widget öğelerini geri getir.
    // Tüm XAML işlemleri (timer durdurma dahil) UI thread'inde yürütülmelidir.
    // Referanslar kopyalanır: dispatch gecikirse globaller temizlendikten sonra da
    // doğru öğeler üzerinde çalışılır.
    wuxc::Grid rootGrid = g_lockRootGrid;
    wux::UIElement activeClock = g_activeClockElement;
    std::vector<wux::FrameworkElement> hiddenClock = g_hiddenClockElements;
    std::vector<wux::FrameworkElement> hiddenWidget = g_hiddenWidgetElements;

    auto cleanup = [rootGrid, activeClock, hiddenClock, hiddenWidget]() {
        StopClockTimer();

        if (activeClock) {
            try {
                uint32_t index = 0;
                if (rootGrid && rootGrid.Children().IndexOf(activeClock, index)) {
                    rootGrid.Children().RemoveAt(index);
                }
            } catch (...) {}
        }

        for (const auto& elem : hiddenClock) {
            if (elem) {
                try { elem.Visibility(wux::Visibility::Visible); } catch (...) {}
            }
        }
        for (const auto& elem : hiddenWidget) {
            if (elem) {
                try { elem.Visibility(wux::Visibility::Visible); } catch (...) {}
            }
        }
    };

    auto dispatcher = rootGrid ? rootGrid.Dispatcher() : nullptr;
    if (dispatcher && !dispatcher.HasThreadAccess()) {
        HANDLE hEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        bool dispatched = false;
        try {
            dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, [cleanup, hEvent]() {
                cleanup();
                if (hEvent) SetEvent(hEvent);
            });
            dispatched = true;
        } catch (...) {
            Wh_Log(L"Failed to dispatch cleanup to the UI thread.");
        }

        if (dispatched && hEvent) {
            // Zaman aşımında handle'ı kapatmıyoruz: kapanmış bir handle'a sonradan
            // SetEvent çağrılmasındansa süreç kapanırken tek handle sızması yeğdir.
            if (WaitForSingleObject(hEvent, 500) == WAIT_OBJECT_0) {
                CloseHandle(hEvent);
            }
        } else {
            if (hEvent) CloseHandle(hEvent);
            cleanup();
        }
    } else {
        // UI ağacı yoksa (veya zaten UI thread'indeysek) doğrudan temizle.
        cleanup();
    }

    g_tapInjected = false;
    g_clockInjected = false;
    g_activeClockElement = nullptr;
    g_lockRootGrid = nullptr;
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
    g_timeCache.Invalidate();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Mod settings changed (Wh_ModSettingsChanged)...");

    // g_settings yalnızca XAML UI thread'inde yazılır; timer ve UI kurulumu aynı
    // thread'de okuduğu için std::wstring üyelerinde veri yarışı oluşmaz.
    auto dispatcher = g_lockRootGrid ? g_lockRootGrid.Dispatcher() : nullptr;
    if (!dispatcher) {
        // Kilit ekranı ağacı henüz yok: okuyan bir UI thread'i olmadığı için
        // ayarları doğrudan yüklemek güvenlidir.
        LoadSettings();
        g_timeCache.Invalidate();
        for (int i = 0; i < 6; i++) {
            g_pixelDigits[i].lastVal = -1;
        }
        return;
    }

    try {
        dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal, []() {
            LoadSettings();
            g_timeCache.Invalidate();
            for (int i = 0; i < 6; i++) {
                g_pixelDigits[i].lastVal = -1;
            }
            RebuildClockUIOnUIThread();
        });
    } catch (...) {
        // Dispatcher kapanıyorsa ayarları yine de yükle; bir sonraki oturumda geçerli olur.
        Wh_Log(L"Failed to dispatch settings change to the UI thread.");
        LoadSettings();
        g_timeCache.Invalidate();
        for (int i = 0; i < 6; i++) {
            g_pixelDigits[i].lastVal = -1;
        }
    }
}
