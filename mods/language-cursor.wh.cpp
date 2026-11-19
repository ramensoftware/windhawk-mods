// ==WindhawkMod==
// @id              language-cursor
// @name            Language Cursor
// @name:uk         Мова біля курсора
// @description     Shows the current keyboard layout as a small badge next to the mouse pointer, with a color, image or custom pointer for each language.
// @description:uk  Показує поточну розкладку клавіатури маленькою позначкою біля курсора миші, з кольором, зображенням або власним курсором для кожної мови.
// @version         1.0.0
// @author          Sanekx Arcs
// @github          https://github.com/SanekxArcs
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -lgdiplus -lgdi32 -lshcore -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Language Cursor

Shows the current keyboard layout as a small badge next to the mouse pointer,
so you know which language you are about to type in before you press a key. It
follows the layout of the app you are working in, not of Explorer.

## Indicator modes

* **Label next to pointer**: a floating badge that follows the mouse pointer.
* **Label inside text cursor**: the label is drawn into the I-beam pointer that
  Windows shows over text fields. This replaces the system I-beam pointer while
  the mod is running.
* **Off**: no badge. Custom pointers per language still apply.

## When the badge is shown

* **Always**.
* **Only over text fields**: while the pointer is an I-beam.
* **Briefly after switching language**: for a duration you choose.

It can also hide when the pointer is hidden and in full-screen apps. You can
set its position around the pointer, distance, font size, opacity, corner
roundness and drop shadow.

## Per-language styles

Add one entry per keyboard layout under **Languages**. Each entry can set:

* a label of up to 6 characters (by default the language code, such as `EN`);
* a background color as hex RGB, such as `#FFB900`;
* an image shown in place of the label (PNG, JPG, BMP or GIF, first frame);
* a custom mouse pointer (CUR or ANI) used while that layout is active.

The layout is identified by its locale, such as `uk-UA`, `en-US`, `en-GB` or
`pl-PL`, or by its HKL value as 8 or 16 hex digits. Layouts without an entry
get an automatic label and color. Paths may contain environment variables such
as `%USERPROFILE%`, and the files must stay available.

## Custom pointers and safety

Custom pointers and the text-cursor mode change the Windows system pointers.
The mod restores the Windows pointer scheme when it is disabled or unloaded,
and on its next start if Explorer crashed while pointers were changed. To
restore them by hand, open Windows Mouse settings > Pointers and click Apply.

## Performance

The mod runs one background thread in Explorer and installs no hooks. It checks
the pointer 60 times a second only while the badge is visible and the pointer
is moving, and slows down to about 8 checks a second when nothing is happening.

## Limitations

* The badge isn't shown over the secure desktop (UAC prompts, lock screen), and
  may not appear over some full-screen games.
* Animated GIF images show only their first frame.
* The **Switching enabled** setting pauses and resumes everything without
  disabling the mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Switching enabled (pause/resume)
- indicatorMode: "FollowPointer"
  $name: Indicator mode
  $options:
  - FollowPointer: Label next to pointer
  - TextCursor: Label inside text cursor
  - 'Off': Off (custom pointers still apply)
- position: "TopLeft"
  $name: Badge position
  $options:
  - TopLeft: Top left
  - Top: Top
  - TopRight: Top right
  - Left: Left
  - Right: Right
  - BottomLeft: Bottom left
  - Bottom: Bottom
  - BottomRight: Bottom right
- offsetX: "-14"
  $name: Horizontal distance (DIP, -40 to 120)
- offsetY: "12"
  $name: Vertical distance (DIP, -40 to 120)
- size: "9"
  $name: Label font size (DIP, 8 to 40)
- opacity: 60
  $name: Badge opacity (20 to 100 percent)
- roundness: 100
  $name: Corner roundness (0 square to 100 pill)
- shadow: false
  $name: Drop shadow
- visibility: "OverText"
  $name: Show badge
  $options:
  - Always: Always
  - OverText: Only over text fields (I-beam pointer)
  - AfterSwitch: Briefly after switching language
- afterSwitchMs: 1500
  $name: After-switch duration (500 to 5000 ms)
- hideWhenPointerHidden: true
  $name: Hide when the pointer is hidden
- hideInFullScreen: true
  $name: Hide in fullscreen apps
- typingScale: 200
  $name: Text-cursor label scale (100 to 300 percent)
- languages:
  - - layout: "FFFFFFFFF0A80422"
      $name: HKL or locale
    - culture: "uk-UA"
      $name: Fallback locale for changed HKL
    - label: ""
      $name: Label (empty = automatic, up to 6 characters)
    - color: "#FFB900"
      $name: Background color (empty = automatic, or hex RGB)
    - imagePath: ""
      $name: Badge image path (PNG/JPG/BMP/GIF)
    - cursorPath: ""
      $name: Custom pointer path (CUR/ANI)
  - - layout: "0000000004090409"
      $name: HKL or locale
    - culture: "en-US"
      $name: Fallback locale for changed HKL
    - label: ""
      $name: Label (empty = automatic, up to 6 characters)
    - color: "#1B1B1F"
      $name: Background color (empty = automatic, or hex RGB)
    - imagePath: ""
      $name: Badge image path (PNG/JPG/BMP/GIF)
    - cursorPath: ""
      $name: Custom pointer path (CUR/ANI)
  - - layout: "0000000008090809"
      $name: HKL or locale
    - culture: "en-GB"
      $name: Fallback locale for changed HKL
    - label: ""
      $name: Label (empty = automatic, up to 6 characters)
    - color: "#E3008C"
      $name: Background color (empty = automatic, or hex RGB)
    - imagePath: ""
      $name: Badge image path (PNG/JPG/BMP/GIF)
    - cursorPath: ""
      $name: Custom pointer path (CUR/ANI)
  $name: Languages (add one entry per layout)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <gdiplus.h>
#include <shellscalingapi.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

enum class Mode { FollowPointer, TextCursor, Off };
enum class Position { TopLeft, Top, TopRight, Left, Right, BottomLeft, Bottom, BottomRight };
enum class Visibility { Always, OverText, AfterSwitch };

struct Mapping {
    std::wstring layout, culture, label, color, imagePath, cursorPath;
};

struct Settings {
    bool enabled = true;
    Mode mode = Mode::FollowPointer;
    Position position = Position::BottomRight;
    double offsetX = 14, offsetY = 18, size = 12, opacity = .95, roundness = .45;
    bool shadow = true;
    Visibility visibility = Visibility::Always;
    DWORD afterSwitchMs = 1500;
    bool hideWhenPointerHidden = true, hideInFullScreen = true;
    double typingScale = 1;
    std::vector<Mapping> mappings;
};

struct Language {
    HKL layout = nullptr;
    std::wstring id, culture, label;
};

struct Style {
    std::wstring text, image;
    Gdiplus::Color background{255, 0, 103, 192};
    double size = 12, roundness = .45;
    bool shadow = true;
};

struct Badge {
    int width = 0, height = 0, margin = 0;
    std::vector<DWORD> pixels;
};

std::mutex g_settingsMutex;
Settings g_settings;
uint64_t g_revision = 0;
HANDLE g_stop = nullptr, g_wake = nullptr, g_ready = nullptr, g_thread = nullptr;
std::atomic<bool> g_started = false;

std::wstring ReadString(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result;
    try { if (value) { result = value; } }
    catch (...) { if (value) { Wh_FreeStringSetting(value); } throw; }
    if (value) { Wh_FreeStringSetting(value); }
    return result;
}

double ParseNumber(const std::wstring& text, double fallback, double minimum, double maximum) {
    if (text.empty()) { return fallback; }
    wchar_t* end = nullptr;
    double value = wcstod(text.c_str(), &end);
    if (!end || end == text.c_str() || *end || !std::isfinite(value)) { return fallback; }
    return std::clamp(value, minimum, maximum);
}

std::wstring ExpandPath(const std::wstring& path) {
    if (path.empty()) { return {}; }
    DWORD count = ExpandEnvironmentStringsW(path.c_str(), nullptr, 0);
    if (!count || count > 32768) { return {}; }
    std::wstring result(count, 0);
    if (ExpandEnvironmentStringsW(path.c_str(), result.data(), count) != count) { return {}; }
    result.resize(count - 1);
    return result;
}

std::wstring LimitLabel(const std::wstring& text) {
    size_t end = 0;
    for (int count = 0; count < 6 && end < text.size(); ++count) {
        wchar_t current = text[end++];
        if (current >= 0xd800 && current <= 0xdbff && end < text.size() &&
            text[end] >= 0xdc00 && text[end] <= 0xdfff) { ++end; }
    }
    return text.substr(0, end);
}

Settings LoadSettings() {
    Settings settings;
    settings.enabled = Wh_GetIntSetting(L"enabled") != 0;
    auto mode = ReadString(L"indicatorMode");
    settings.mode = mode == L"TextCursor" ? Mode::TextCursor : mode == L"Off" ? Mode::Off : Mode::FollowPointer;
    auto position = ReadString(L"position");
    const PCWSTR positions[] = {L"TopLeft", L"Top", L"TopRight", L"Left", L"Right", L"BottomLeft", L"Bottom", L"BottomRight"};
    for (size_t index = 0; index < ARRAYSIZE(positions); ++index) {
        if (position == positions[index]) { settings.position = static_cast<Position>(index); }
    }
    settings.offsetX = ParseNumber(ReadString(L"offsetX"), 14, -40, 120);
    settings.offsetY = ParseNumber(ReadString(L"offsetY"), 18, -40, 120);
    settings.size = ParseNumber(ReadString(L"size"), 12, 8, 40);
    settings.opacity = std::clamp(Wh_GetIntSetting(L"opacity"), 20, 100) / 100.;
    settings.roundness = std::clamp(Wh_GetIntSetting(L"roundness"), 0, 100) / 100.;
    settings.shadow = Wh_GetIntSetting(L"shadow") != 0;
    auto visibility = ReadString(L"visibility");
    settings.visibility = visibility == L"OverText" ? Visibility::OverText :
        visibility == L"AfterSwitch" ? Visibility::AfterSwitch : Visibility::Always;
    settings.afterSwitchMs = std::clamp(Wh_GetIntSetting(L"afterSwitchMs"), 500, 5000);
    settings.hideWhenPointerHidden = Wh_GetIntSetting(L"hideWhenPointerHidden") != 0;
    settings.hideInFullScreen = Wh_GetIntSetting(L"hideInFullScreen") != 0;
    settings.typingScale = std::clamp(Wh_GetIntSetting(L"typingScale"), 100, 300) / 100.;
    for (int index = 0; index < 256; ++index) {
        std::wstring prefix = L"languages[" + std::to_wstring(index) + L"].";
        Mapping mapping;
        mapping.layout = ReadString((prefix + L"layout").c_str());
        if (mapping.layout.empty()) { break; }
        mapping.culture = ReadString((prefix + L"culture").c_str());
        mapping.label = LimitLabel(ReadString((prefix + L"label").c_str()));
        mapping.color = ReadString((prefix + L"color").c_str());
        mapping.imagePath = ExpandPath(ReadString((prefix + L"imagePath").c_str()));
        mapping.cursorPath = ExpandPath(ReadString((prefix + L"cursorPath").c_str()));
        settings.mappings.push_back(std::move(mapping));
    }
    return settings;
}

bool EqualText(const std::wstring& left, const std::wstring& right) {
    return _wcsicmp(left.c_str(), right.c_str()) == 0;
}

bool ParseLayout(const std::wstring& text, DWORD& layout) {
    if (text.size() != 8 && text.size() != 16) { return false; }
    if (!std::all_of(text.begin(), text.end(), [](wchar_t value) { return iswxdigit(value); })) { return false; }
    layout = static_cast<DWORD>(wcstoull(text.c_str(), nullptr, 16));
    return true;
}

Language DescribeLanguage(HKL layout) {
    Language language;
    language.layout = layout;
    wchar_t identifier[17]{};
    swprintf_s(identifier, L"%016llX", static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(layout)));
    language.id = identifier;
    wchar_t culture[LOCALE_NAME_MAX_LENGTH]{};
    if (LCIDToLocaleName(MAKELCID(LOWORD(reinterpret_cast<uintptr_t>(layout)), SORT_DEFAULT),
                         culture, ARRAYSIZE(culture), 0)) { language.culture = culture; }
    wchar_t code[16]{};
    if (!language.culture.empty()) {
        GetLocaleInfoEx(language.culture.c_str(), LOCALE_SISO639LANGNAME, code, ARRAYSIZE(code));
    }
    language.label = code[0] ? code : L"??";
    if (EqualText(language.label, L"uk")) { language.label = L"UA"; }
    CharUpperBuffW(language.label.data(), static_cast<DWORD>(language.label.size()));
    return language;
}

const Mapping* SelectMapping(const Settings& settings, const Language& language,
                             const std::vector<HKL>& installed) {
    DWORD actual = static_cast<DWORD>(reinterpret_cast<uintptr_t>(language.layout));
    for (const auto& mapping : settings.mappings) {
        DWORD candidate = 0;
        if (ParseLayout(mapping.layout, candidate) && candidate == actual) { return &mapping; }
    }
    for (const auto& mapping : settings.mappings) {
        DWORD candidate = 0;
        if (!ParseLayout(mapping.layout, candidate)) {
            if (EqualText(mapping.layout, language.culture)) { return &mapping; }
        } else if (!mapping.culture.empty() && EqualText(mapping.culture, language.culture)) {
            bool stillInstalled = std::any_of(installed.begin(), installed.end(), [&](HKL value) {
                return static_cast<DWORD>(reinterpret_cast<uintptr_t>(value)) == candidate;
            });
            if (!stillInstalled) { return &mapping; }
        }
    }
    return nullptr;
}

std::vector<HKL> InstalledLayouts() {
    int count = GetKeyboardLayoutList(0, nullptr);
    if (count <= 0 || count > 256) { return {}; }
    std::vector<HKL> result(count);
    int returned = GetKeyboardLayoutList(count, result.data());
    result.resize(std::clamp(returned, 0, count));
    return result;
}

HKL ForegroundLayout() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) { return nullptr; }
    wchar_t className[128]{};
    GetClassNameW(foreground, className, ARRAYSIZE(className));
    if (wcscmp(className, L"ApplicationFrameWindow") == 0) {
        HWND core = FindWindowExW(foreground, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
        if (core) { foreground = core; }
    }
    DWORD thread = GetWindowThreadProcessId(foreground, nullptr);
    GUITHREADINFO information{sizeof(information)};
    if (thread && GetGUIThreadInfo(thread, &information) && information.hwndFocus) {
        DWORD focused = GetWindowThreadProcessId(information.hwndFocus, nullptr);
        if (focused) { thread = focused; }
    }
    return thread ? GetKeyboardLayout(thread) : nullptr;
}

Gdiplus::Color ParseColor(const std::wstring& text, Gdiplus::Color fallback) {
    std::wstring value = text;
    if (!value.empty() && value.front() == L'#') { value.erase(0, 1); }
    if (value.size() != 6 || !std::all_of(value.begin(), value.end(), [](wchar_t part) { return iswxdigit(part); })) {
        return fallback;
    }
    return Gdiplus::Color(0xff000000 | static_cast<UINT>(wcstoul(value.c_str(), nullptr, 16)));
}

double Luminance(Gdiplus::Color color) {
    auto channel = [](BYTE value) { double fraction = value / 255.;
        return fraction <= .03928 ? fraction / 12.92 : std::pow((fraction + .055) / 1.055, 2.4); };
    return .2126 * channel(color.GetR()) + .7152 * channel(color.GetG()) + .0722 * channel(color.GetB());
}

Gdiplus::Color ForegroundColor(Gdiplus::Color background) {
    Gdiplus::Color dark(255, 27, 27, 31);
    double luminance = Luminance(background);
    return 1.05 / (luminance + .05) >= (luminance + .05) / (Luminance(dark) + .05)
        ? Gdiplus::Color(255, 255, 255, 255) : dark;
}

Style BuildStyle(const Settings& settings, const Language& language, const Mapping* mapping) {
    constexpr DWORD palette[] = {0xff0067c0, 0xffc42b1c, 0xff0f7b0f, 0xff8764b8,
        0xffca5010, 0xff038387, 0xffe3008c, 0xff4a5459};
    Style style;
    style.text = mapping && !mapping->label.empty() ? mapping->label : language.label;
    style.background = Gdiplus::Color(palette[LOWORD(reinterpret_cast<uintptr_t>(language.layout)) % ARRAYSIZE(palette)]);
    if (mapping) {
        style.background = ParseColor(mapping->color, style.background);
        style.image = mapping->imagePath;
    }
    style.size = settings.size;
    style.roundness = settings.roundness;
    style.shadow = settings.shadow;
    return style;
}

POINT PlaceBadge(Position position, POINT cursor, int width, int height,
                 double offsetX, double offsetY, RECT bounds, int margin) {
    bool left = position == Position::TopLeft || position == Position::Left || position == Position::BottomLeft;
    bool right = position == Position::TopRight || position == Position::Right || position == Position::BottomRight;
    bool top = position == Position::TopLeft || position == Position::Top || position == Position::TopRight;
    bool bottom = position == Position::BottomLeft || position == Position::Bottom || position == Position::BottomRight;
    double horizontal = left ? cursor.x - offsetX - width : right ? cursor.x + offsetX : cursor.x - width / 2.;
    double vertical = top ? cursor.y - offsetY - height : bottom ? cursor.y + offsetY : cursor.y - height / 2.;
    if (horizontal + width > bounds.right && right) { horizontal = cursor.x - offsetX - width; }
    else if (horizontal < bounds.left && left) { horizontal = cursor.x + offsetX; }
    if (vertical + height > bounds.bottom && bottom) { vertical = cursor.y - offsetY - height; }
    else if (vertical < bounds.top && top) { vertical = cursor.y + offsetY; }
    horizontal = std::clamp(horizontal, double(bounds.left), double(std::max(bounds.left, bounds.right - width)));
    vertical = std::clamp(vertical, double(bounds.top), double(std::max(bounds.top, bounds.bottom - height)));
    return {static_cast<LONG>(std::lround(horizontal)) - margin, static_cast<LONG>(std::lround(vertical)) - margin};
}

bool ShouldShow(const Settings& settings, bool pointerVisible, bool overText,
                bool fullscreen, ULONGLONG now, ULONGLONG switched) {
    if (!settings.enabled || settings.mode != Mode::FollowPointer ||
        (settings.hideWhenPointerHidden && !pointerVisible) ||
        (settings.hideInFullScreen && fullscreen)) { return false; }
    return settings.visibility == Visibility::Always ||
        (settings.visibility == Visibility::OverText && overText) ||
        (settings.visibility == Visibility::AfterSwitch && now >= switched && now - switched < settings.afterSwitchMs);
}

bool IsFullscreen() {
    HWND foreground = GetForegroundWindow();
    if (!foreground || foreground == GetShellWindow() || IsIconic(foreground)) { return false; }
    wchar_t name[128]{};
    GetClassNameW(foreground, name, ARRAYSIZE(name));
    if (!wcscmp(name, L"Progman") || !wcscmp(name, L"WorkerW")) { return false; }
    RECT window{};
    MONITORINFO monitor{sizeof(monitor)};
    if (!GetWindowRect(foreground, &window) ||
        !GetMonitorInfoW(MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST), &monitor)) { return false; }
    return window.left <= monitor.rcMonitor.left && window.top <= monitor.rcMonitor.top &&
        window.right >= monitor.rcMonitor.right && window.bottom >= monitor.rcMonitor.bottom;
}

}

namespace {

void RoundedPath(Gdiplus::GraphicsPath& path, Gdiplus::RectF rect, float radius) {
    float diameter = std::min(radius * 2, std::min(rect.Width, rect.Height));
    if (diameter < 1) { path.AddRectangle(rect); return; }
    path.AddArc(rect.X, rect.Y, diameter, diameter, 180, 90);
    path.AddArc(rect.GetRight() - diameter, rect.Y, diameter, diameter, 270, 90);
    path.AddArc(rect.GetRight() - diameter, rect.GetBottom() - diameter, diameter, diameter, 0, 90);
    path.AddArc(rect.X, rect.GetBottom() - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();
}

bool ReadableImage(const std::wstring& path) {
    if (path.empty()) { return false; }
    WIN32_FILE_ATTRIBUTE_DATA data{};
    return GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &data) &&
        !(data.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_OFFLINE |
                                   FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS)) &&
        data.nFileSizeHigh == 0 && data.nFileSizeLow <= 16 * 1024 * 1024;
}

Badge PixelsFromBitmap(Gdiplus::Bitmap& bitmap, int margin) {
    Badge result{static_cast<int>(bitmap.GetWidth()), static_cast<int>(bitmap.GetHeight()), margin, {}};
    result.pixels.resize(static_cast<size_t>(result.width) * result.height);
    Gdiplus::Rect rectangle(0, 0, result.width, result.height);
    Gdiplus::BitmapData data{};
    if (bitmap.LockBits(&rectangle, Gdiplus::ImageLockModeRead, PixelFormat32bppPARGB, &data) != Gdiplus::Ok) {
        return {};
    }
    for (int row = 0; row < result.height; ++row) {
        memcpy(result.pixels.data() + static_cast<size_t>(row) * result.width,
               static_cast<BYTE*>(data.Scan0) + static_cast<ptrdiff_t>(row) * data.Stride,
               static_cast<size_t>(result.width) * sizeof(DWORD));
    }
    bitmap.UnlockBits(&data);
    return result;
}

Badge RenderBadge(const Style& style, double scale) {
    scale = std::clamp(scale, .5, 4.);
    float fontSize = static_cast<float>(std::clamp(style.size, 4., 320.) * scale);
    Gdiplus::Font font(L"Segoe UI", fontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Bitmap measuring(1, 1, PixelFormat32bppPARGB);
    Gdiplus::Graphics measure(&measuring);
    Gdiplus::RectF measured;
    measure.MeasureString(style.text.c_str(), -1, &font, Gdiplus::PointF(0, 0),
                           Gdiplus::StringFormat::GenericTypographic(), &measured);
    int contentHeight = std::max(1, static_cast<int>(std::lround(fontSize * 1.6)));
    std::unique_ptr<Gdiplus::Bitmap> image;
    if (ReadableImage(style.image)) {
        image.reset(Gdiplus::Bitmap::FromFile(style.image.c_str(), FALSE));
        if (!image || image->GetLastStatus() != Gdiplus::Ok || !image->GetWidth() || !image->GetHeight() ||
            image->GetWidth() > 8192 || image->GetHeight() > 8192) { image.reset(); }
    }
    double aspect = image ? std::clamp(double(image->GetWidth()) / image->GetHeight(), .5, 3.) : 1;
    int contentWidth = image ? static_cast<int>(std::lround(contentHeight * aspect)) :
        std::max(contentHeight, static_cast<int>(std::ceil(measured.Width + fontSize * 1.1)));
    int margin = std::max(1, static_cast<int>(std::ceil((style.shadow ? 5 : 1) * scale)));
    int width = contentWidth + margin * 2, height = contentHeight + margin * 2;
    Gdiplus::Bitmap bitmap(width, height, PixelFormat32bppPARGB);
    if (bitmap.GetLastStatus() != Gdiplus::Ok) { return {}; }
    {
        Gdiplus::Graphics graphics(&bitmap);
        graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        Gdiplus::RectF rectangle(static_cast<float>(margin), static_cast<float>(margin),
                                static_cast<float>(contentWidth), static_cast<float>(contentHeight));
        float radius = static_cast<float>(std::clamp(style.roundness, 0., 1.) * contentHeight / 2);
        if (style.shadow) {
            for (int layer = 4; layer >= 1; --layer) {
                float spread = static_cast<float>(layer * .85 * scale);
                Gdiplus::RectF shadow = rectangle;
                shadow.Inflate(spread, spread);
                shadow.Y += static_cast<float>(1.2 * scale);
                Gdiplus::GraphicsPath shadowPath;
                RoundedPath(shadowPath, shadow, radius + spread);
                Gdiplus::SolidBrush brush(Gdiplus::Color(static_cast<BYTE>(22 - layer * 3), 0, 0, 0));
                graphics.FillPath(&brush, &shadowPath);
            }
        }
        Gdiplus::GraphicsPath shape;
        RoundedPath(shape, rectangle, radius);
        if (image) {
            auto saved = graphics.Save();
            graphics.SetClip(&shape);
            graphics.DrawImage(image.get(), rectangle);
            graphics.Restore(saved);
        } else {
            Gdiplus::SolidBrush background(style.background);
            graphics.FillPath(&background, &shape);
            Gdiplus::SolidBrush foreground(ForegroundColor(style.background));
            Gdiplus::StringFormat format;
            format.SetAlignment(Gdiplus::StringAlignmentCenter);
            format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);
            graphics.DrawString(style.text.c_str(), -1, &font, rectangle, &format, &foreground);
        }
        Gdiplus::Pen outline(Luminance(style.background) > .5 ? Gdiplus::Color(48, 0, 0, 0) :
                             Gdiplus::Color(56, 255, 255, 255), static_cast<float>(scale));
        graphics.DrawPath(&outline, &shape);
    }
    return PixelsFromBitmap(bitmap, margin);
}

class Surface {
public:
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    Surface() = default;
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
    ~Surface() { Reset(); }
    void Reset() {
        if (previous && dc) { SelectObject(dc, previous); }
        if (bitmap) { DeleteObject(bitmap); }
        if (dc) { DeleteDC(dc); }
        dc = nullptr; bitmap = nullptr; previous = nullptr;
    }
    bool Set(const Badge& badge) {
        Reset();
        if (badge.pixels.empty()) { return false; }
        BITMAPINFO information{};
        information.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        information.bmiHeader.biWidth = badge.width;
        information.bmiHeader.biHeight = -badge.height;
        information.bmiHeader.biPlanes = 1;
        information.bmiHeader.biBitCount = 32;
        information.bmiHeader.biCompression = BI_RGB;
        void* pixels = nullptr;
        dc = CreateCompatibleDC(nullptr);
        if (!dc) { return false; }
        bitmap = CreateDIBSection(dc, &information, DIB_RGB_COLORS, &pixels, nullptr, 0);
        if (!bitmap || !pixels) { Reset(); return false; }
        previous = SelectObject(dc, bitmap);
        if (!previous || previous == HGDI_ERROR) { previous = nullptr; Reset(); return false; }
        memcpy(pixels, badge.pixels.data(), badge.pixels.size() * sizeof(DWORD));
        return true;
    }
};

HCURSOR MakeTextCursor(Style style, double scale, int factor) {
    factor = std::clamp(factor, 1, 8);
    style.size = 8 * std::clamp(scale, 1., 3.) * factor;
    style.shadow = false;
    Badge label = RenderBadge(style, 1);
    if (label.pixels.empty()) { return nullptr; }
    int beamHeight = 32 * factor, labelLeft = 14 * factor;
    Badge cursor;
    cursor.width = std::min(256, labelLeft + label.width - label.margin);
    cursor.height = std::min(256, std::max(beamHeight, label.height));
    cursor.pixels.resize(static_cast<size_t>(cursor.width) * cursor.height);
    int beamTop = (cursor.height - beamHeight) / 2;
    auto fill = [&](int left, int top, int width, int height, DWORD color) {
        for (int row = top * factor; row < (top + height) * factor; ++row) {
            for (int column = left * factor; column < (left + width) * factor; ++column) {
                int vertical = beamTop + row;
                if (vertical >= 0 && vertical < cursor.height && column >= 0 && column < cursor.width) {
                    cursor.pixels[static_cast<size_t>(vertical) * cursor.width + column] = color;
                }
            }
        }
    };
    fill(2, 3, 10, 2, 0xff000000); fill(2, 27, 10, 2, 0xff000000);
    fill(5, 3, 3, 26, 0xff000000); fill(6, 5, 1, 22, 0xffffffff);
    fill(3, 4, 8, 1, 0xffffffff); fill(3, 27, 8, 1, 0xffffffff);
    int labelTop = (cursor.height - label.height) / 2;
    for (int row = 0; row < label.height; ++row) {
        for (int column = 0; column < label.width; ++column) {
            int horizontal = labelLeft - label.margin + column, vertical = labelTop + row;
            if (horizontal >= 0 && horizontal < cursor.width && vertical >= 0 && vertical < cursor.height) {
                cursor.pixels[static_cast<size_t>(vertical) * cursor.width + horizontal] =
                    label.pixels[static_cast<size_t>(row) * label.width + column];
            }
        }
    }
    Surface surface;
    if (!surface.Set(cursor)) { return nullptr; }
    int maskStride = ((cursor.width + 15) / 16) * 2;
    std::vector<BYTE> maskPixels(static_cast<size_t>(maskStride) * cursor.height, 0);
    for (int row = 0; row < cursor.height; ++row) {
        for (int column = 0; column < cursor.width; ++column) {
            if (!(cursor.pixels[static_cast<size_t>(row) * cursor.width + column] >> 24)) {
                maskPixels[static_cast<size_t>(row) * maskStride + column / 8] |= 0x80 >> (column % 8);
            }
        }
    }
    HBITMAP mask = CreateBitmap(cursor.width, cursor.height, 1, 1, maskPixels.data());
    if (!mask) { return nullptr; }
    SelectObject(surface.dc, surface.previous);
    surface.previous = nullptr;
    ICONINFO information{};
    information.fIcon = FALSE;
    information.xHotspot = static_cast<DWORD>(6.5 * factor - .5);
    information.yHotspot = beamTop + 16 * factor;
    information.hbmColor = surface.bitmap;
    information.hbmMask = mask;
    HCURSOR result = static_cast<HCURSOR>(CreateIconIndirect(&information));
    DeleteObject(mask);
    return result;
}

using CursorWriter = BOOL(WINAPI*)(HCURSOR, DWORD);
CursorWriter g_writeCursor = SetSystemCursor;
bool ResetSystemCursors() { return SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0) != FALSE; }
bool (*g_resetCursors)() = ResetSystemCursors;

bool IsCursorPath(const std::wstring& path) {
    size_t dot = path.find_last_of(L'.');
    return dot != std::wstring::npos &&
        (EqualText(path.substr(dot), L".cur") || EqualText(path.substr(dot), L".ani"));
}

class CursorManager {
    std::wstring arrow_, text_;
    bool marked_ = false;
    bool Mark() {
        if (!marked_) { marked_ = Wh_SetIntValue(L"cursorsApplied", 1) != FALSE; }
        return marked_;
    }
    bool Transfer(HCURSOR cursor, DWORD role) {
        if (!cursor) { return false; }
        if (!Mark() || !g_writeCursor(cursor, role)) {
            DestroyCursor(cursor);
            return false;
        }
        return true;
    }
    bool Failed() { Restore(); return false; }
public:
    ~CursorManager() { Restore(); }
    bool Restore() {
        if (!marked_ && arrow_.empty() && text_.empty()) { return true; }
        if (!g_resetCursors()) { Wh_Log(L"Could not restore Windows cursor scheme"); return false; }
        arrow_.clear(); text_.clear(); marked_ = false;
        Wh_SetIntValue(L"cursorsApplied", 0);
        return true;
    }
    bool Apply(const std::wstring& arrow, const Style* textStyle, const std::wstring& key,
               double scale, int cursorFactor) {
        std::wstring desiredText = textStyle ? key : L"";
        if ((arrow.empty() && !arrow_.empty()) || (desiredText.empty() && !text_.empty())) {
            if (!Restore()) { return false; }
        }
        if (!arrow.empty() && !EqualText(arrow, arrow_)) {
            if (!IsCursorPath(arrow)) { return Failed(); }
            HCURSOR loaded = LoadCursorFromFileW(arrow.c_str());
            if (!loaded) { return Failed(); }
            HCURSOR copy = static_cast<HCURSOR>(CopyIcon(loaded));
            DestroyCursor(loaded);
            if (!Transfer(copy, 32512)) { return Failed(); }
            arrow_ = arrow;
        }
        if (textStyle && desiredText != text_) {
            if (!Transfer(MakeTextCursor(*textStyle, scale, cursorFactor), 32513)) { return Failed(); }
            text_ = desiredText;
        }
        return true;
    }
};

}

namespace {

constexpr PCWSTR kWindowClass = L"Windhawk.LanguageCursor.Overlay.0.1";

LRESULT CALLBACK OverlayProcedure(HWND window, UINT message, WPARAM parameter, LPARAM data) {
    if (message == WM_NCHITTEST) { return HTTRANSPARENT; }
    if (message == WM_MOUSEACTIVATE) { return MA_NOACTIVATE; }
    if (message == WM_ERASEBKGND) { return 1; }
    return DefWindowProcW(window, message, parameter, data);
}

class Runtime {
    HWND window_ = nullptr;
    HMODULE module_ = nullptr;
    ATOM windowClass_ = 0;
    Settings settings_;
    uint64_t revision_ = UINT64_MAX;
    Language language_;
    std::vector<HKL> installed_;
    Mapping mapping_;
    bool hasMapping_ = false;
    Style style_;
    Badge badge_;
    Surface surface_;
    CursorManager cursors_;
    HMONITOR monitor_ = nullptr;
    RECT bounds_{};
    double dpiScale_ = 1;
    ULONGLONG lastLanguage_ = 0, switched_ = 0, lastScreen_ = 0, retryAt_ = 0;
    bool fullscreen_ = false, cursorPending_ = true, shown_ = false;
    BYTE alpha_ = 0;
    POINT location_{LONG_MIN, LONG_MIN};
    POINT lastCursor_{LONG_MIN, LONG_MIN};
    ULONGLONG movedAt_ = 0;
    bool fading_ = false;

public:
    ~Runtime() {
        if (window_) { DestroyWindow(window_); }
        if (windowClass_) { UnregisterClassW(kWindowClass, module_); }
    }
    bool Initialize() {
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<PCWSTR>(OverlayProcedure), &module_);
        WNDCLASSEXW definition{sizeof(definition)};
        definition.hInstance = module_;
        definition.lpfnWndProc = OverlayProcedure;
        definition.lpszClassName = kWindowClass;
        windowClass_ = RegisterClassExW(&definition);
        if (!windowClass_) { return false; }
        window_ = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE |
                                      WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                                  kWindowClass, L"Language Cursor", WS_POPUP, 0, 0, 1, 1,
                                  nullptr, nullptr, module_, nullptr);
        return window_ != nullptr;
    }
    HWND Window() const { return window_; }
    // How long to sleep before the next Tick. The fast rate is only needed
    // while the badge is on screen and has to follow the pointer or fade.
    DWORD PollInterval() const {
        if (!settings_.enabled) { return 500; }
        if (settings_.mode != Mode::FollowPointer) { return 120; }
        if (fading_) { return 16; }
        bool moving = GetTickCount64() - movedAt_ < 1000;
        if (alpha_) { return moving ? 16 : 50; }
        return moving ? 50 : 120;
    }
    void Tick() {
        ULONGLONG now = GetTickCount64();
        bool dirty = false;
        {
            std::lock_guard lock(g_settingsMutex);
            if (revision_ != g_revision) {
                settings_ = g_settings;
                revision_ = g_revision;
                dirty = true;
            }
        }
        if (dirty || now - lastLanguage_ >= 120) {
            lastLanguage_ = now;
            HKL current = ForegroundLayout();
            if (current && current != language_.layout) {
                language_ = DescribeLanguage(current);
                switched_ = now;
                dirty = true;
            }
            if (dirty) {
                installed_ = InstalledLayouts();
                const Mapping* selected = SelectMapping(settings_, language_, installed_);
                hasMapping_ = selected != nullptr;
                mapping_ = selected ? *selected : Mapping{};
            }
        }
        CURSORINFO cursor{sizeof(cursor)};
        bool gotCursor = GetCursorInfo(&cursor) != FALSE;
        if (gotCursor && (cursor.ptScreenPos.x != lastCursor_.x ||
                          cursor.ptScreenPos.y != lastCursor_.y)) {
            lastCursor_ = cursor.ptScreenPos;
            movedAt_ = now;
        }
        bool screenRefresh = dirty || now - lastScreen_ >= 500;
        if (screenRefresh) {
            lastScreen_ = now;
            fullscreen_ = IsFullscreen();
        }
        if (gotCursor) {
            HMONITOR monitor = MonitorFromPoint(cursor.ptScreenPos, MONITOR_DEFAULTTONEAREST);
            if (monitor != monitor_ || screenRefresh) {
                MONITORINFO information{sizeof(information)};
                if (GetMonitorInfoW(monitor, &information)) { bounds_ = information.rcMonitor; }
                UINT horizontal = 96, vertical = 96;
                if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &horizontal, &vertical))) { horizontal = 96; }
                double nextScale = std::clamp(horizontal / 96., .5, 4.);
                if (nextScale != dpiScale_) { dirty = true; }
                dpiScale_ = nextScale;
                monitor_ = monitor;
            }
        }
        if (dirty) {
            style_ = BuildStyle(settings_, language_, hasMapping_ ? &mapping_ : nullptr);
            badge_ = settings_.mode == Mode::FollowPointer ? RenderBadge(style_, dpiScale_) : Badge{};
            if (!badge_.pixels.empty() && !surface_.Set(badge_)) { badge_ = {}; }
            cursorPending_ = true;
            retryAt_ = 0;
        }
        if (cursorPending_ && now >= retryAt_) {
            int factor = std::clamp(static_cast<int>(std::lround(std::max(
                dpiScale_, GetSystemMetrics(SM_CYCURSOR) / 32.))), 1, 8);
            const Style* text = settings_.enabled && language_.layout && settings_.mode == Mode::TextCursor ? &style_ : nullptr;
            std::wstring arrow = settings_.enabled && hasMapping_ ? mapping_.cursorPath : L"";
            std::wstring key = language_.id + L"/" + std::to_wstring(revision_) + L"/" + std::to_wstring(factor);
            bool applied = settings_.enabled ? cursors_.Apply(arrow, text, key, settings_.typingScale, factor) : cursors_.Restore();
            cursorPending_ = !applied;
            if (!applied) {
                Wh_Log(L"Cursor update failed; check configured CUR/ANI or image paths");
                retryAt_ = now + 1000;
            }
        }
        bool overText = gotCursor && cursor.hCursor && cursor.hCursor == LoadCursorW(nullptr, IDC_IBEAM);
        bool visible = gotCursor && language_.layout && !badge_.pixels.empty() &&
            ShouldShow(settings_, (cursor.flags & CURSOR_SHOWING) != 0, overText, fullscreen_, now, switched_);
        BYTE target = visible ? static_cast<BYTE>(std::lround(settings_.opacity * 255)) : 0;
        BYTE nextAlpha = alpha_ < target ? static_cast<BYTE>(std::min(int(target), alpha_ + 48)) :
            static_cast<BYTE>(std::max(int(target), alpha_ - 32));
        bool alphaChanged = alpha_ != nextAlpha;
        alpha_ = nextAlpha;
        fading_ = alpha_ != target;
        if (!alpha_) {
            if (shown_) { ShowWindow(window_, SW_HIDE); shown_ = false; }
            return;
        }
        if (!gotCursor || badge_.pixels.empty() || !surface_.dc) {
            if (shown_) { ShowWindow(window_, SW_HIDE); shown_ = false; }
            alpha_ = 0;
            fading_ = false;
            return;
        }
        POINT destination = PlaceBadge(settings_.position, cursor.ptScreenPos,
            badge_.width - 2 * badge_.margin, badge_.height - 2 * badge_.margin,
            settings_.offsetX * dpiScale_, settings_.offsetY * dpiScale_, bounds_, badge_.margin);
        if (dirty || alphaChanged || !shown_) {
            SIZE size{badge_.width, badge_.height};
            POINT source{};
            BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha_, AC_SRC_ALPHA};
            if (!UpdateLayeredWindow(window_, nullptr, &destination, &size, surface_.dc,
                                      &source, 0, &blend, ULW_ALPHA)) { return; }
        } else if (destination.x != location_.x || destination.y != location_.y) {
            SetWindowPos(window_, HWND_TOPMOST, destination.x, destination.y, 0, 0,
                         SWP_NOSIZE | SWP_NOACTIVATE);
        }
        location_ = destination;
        if (!shown_) { ShowWindow(window_, SW_SHOWNOACTIVATE); shown_ = true; }
        if (screenRefresh) {
            SetWindowPos(window_, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
};

DWORD WINAPI Worker(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HANDLE singleton = CreateMutexW(nullptr, FALSE, L"Local\\Windhawk.LanguageCursor.Singleton");
    DWORD ownership = singleton ? WaitForSingleObject(singleton, 0) : WAIT_FAILED;
    if (ownership != WAIT_OBJECT_0 && ownership != WAIT_ABANDONED) {
        Wh_Log(L"Another Language Cursor mod instance is already running");
        if (singleton) { CloseHandle(singleton); }
        SetEvent(g_ready);
        return 1;
    }
    Gdiplus::GdiplusStartupInput startup;
    ULONG_PTR token = 0;
    Gdiplus::Status startupStatus = Gdiplus::GdiplusStartup(&token, &startup, nullptr);
    try {
        if (startupStatus == Gdiplus::Ok) {
            if (Wh_GetIntValue(L"cursorsApplied", 0) != 0) {
                if (!g_resetCursors()) { throw std::runtime_error("Crash recovery failed"); }
                Wh_SetIntValue(L"cursorsApplied", 0);
            }
            Runtime runtime;
            if (runtime.Initialize()) {
                g_started = true;
                SetEvent(g_ready);
                HANDLE events[] = {g_stop, g_wake};
                bool quit = false;
                while (!quit) {
                    DWORD wake = MsgWaitForMultipleObjectsEx(2, events, runtime.PollInterval(), QS_ALLINPUT, MWMO_INPUTAVAILABLE);
                    if (wake == WAIT_OBJECT_0 || wake == WAIT_FAILED) { break; }
                    MSG message;
                    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                        if (message.message == WM_QUIT) { quit = true; break; }
                        TranslateMessage(&message);
                        DispatchMessageW(&message);
                    }
                    if (!quit) { runtime.Tick(); }
                }
            }
        }
    } catch (...) { Wh_Log(L"Language Cursor worker stopped after an error"); }
    g_started = false;
    SetEvent(g_ready);
    if (startupStatus == Gdiplus::Ok) { Gdiplus::GdiplusShutdown(token); }
    ReleaseMutex(singleton);
    CloseHandle(singleton);
    return 0;
}

void Stop() {
    if (g_stop) { SetEvent(g_stop); }
    if (g_thread) { WaitForSingleObject(g_thread, INFINITE); }
    for (HANDLE* handle : {&g_thread, &g_stop, &g_wake, &g_ready}) {
        if (*handle) { CloseHandle(*handle); *handle = nullptr; }
    }
}

void Reload() {
    Settings settings = LoadSettings();
    {
        std::lock_guard lock(g_settingsMutex);
        g_settings = std::move(settings);
        ++g_revision;
    }
    if (g_wake) { SetEvent(g_wake); }
}

}

BOOL Wh_ModInit() {
    try { Reload(); }
    catch (...) { Wh_Log(L"Could not load Language Cursor settings"); return FALSE; }
    g_started = false;
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_stop && g_wake && g_ready) { g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr); }
    if (!g_thread || WaitForSingleObject(g_ready, 5000) != WAIT_OBJECT_0 || !g_started) {
        Stop();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    try { Reload(); }
    catch (...) { Wh_Log(L"Invalid settings update; keeping previous settings"); }
}

void Wh_ModBeforeUninit() { Stop(); }
void Wh_ModUninit() { Stop(); }
