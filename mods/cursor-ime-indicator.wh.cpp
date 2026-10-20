// ==WindhawkMod==
// @id              cursor-ime-indicator
// @name            Cursor IME Indicator
// @name:zh-CN      光标输入法指示器
// @description     Shows the active input language and Caps Lock state next to the mouse pointer.
// @description:zh-CN 在鼠标指针旁显示当前输入语言与大写锁定状态。
// @version         1.0
// @author          CYoJkoY
// @github          https://github.com/CYoJkoY
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lgdi32 -limm32 -lshcore -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Cursor IME Indicator

Cursor IME Indicator draws a small pill next to the mouse pointer with the
input language that the foreground window is using, and a separate "CL" pill
while Caps Lock is on. The badge follows the pointer, stays out of the way of
clicks, and disappears when it is not needed.

## What the badge shows

- **Zh** - a Simplified Chinese input method is in its native (Chinese) mode.
- **Zh-TW**, **Zh-HK**, **Zh-MO**, **Zh-SG** - the corresponding Chinese
  sub-language is active.
- **En** - an English keyboard layout is active, or an IME is in its Latin
  (alphanumeric) mode.
- **Ja**, **Ko**, **Fr**, **De**, **Es**, **Pt**, **Ru**, **Pl**, **It**, **Tr**
  and every other layout - the ISO 639 language code with a capital first
  letter and an upper-case region.
- **CL** - Caps Lock is on, drawn as a separate pill next to the language pill.

The Chinese, English and Caps Lock labels are configurable, so the badge can
show "中", "英" and "大写" instead. Any other language can be relabelled with
the override list, for example "ja=日" or "zh-TW=繁".

## How the language is detected

The mod runs in its own process and watches the foreground window. The
keyboard layout of the foreground thread is read with `GetKeyboardLayout`, which
reports the layout that the focused window is actually using, including windows
that switch layout per application.

Microsoft's Chinese, Japanese and Korean input methods keep their native and
their Latin mode on the same layout, so the layout alone cannot tell those modes
apart. For these three languages the mod asks the IME window for its open
status with `WM_IME_CONTROL` / `IMC_GETOPENSTATUS`. A closed IME types Latin
input and shows the English label even though Microsoft's Chinese IME keeps
`IME_CMODE_NATIVE` in its remembered conversion mode. An open IME is then
classified with `IMC_GETCONVERSIONMODE`: `IME_CMODE_NATIVE` means the native
language, and its absence means Latin mode. `ImmGetConversionStatus` is the
fallback when the IME window does not answer. A query that fails completely
reports the language label, so a hung application can never make the badge
claim that Latin mode is active. Other input frameworks are reported from
their layout only, without an IME query.

IME mode is a per-window property, so the mod resolves the focused window of
the foreground thread with `GetGUIThreadInfo` and asks that window first,
falling back to the foreground window itself. The language comes from
`GetKeyboardLayout` of the same thread, which covers applications that switch
layouts per window. Chinese codes come from the sub-language of the layout,
because all Chinese layouts share the ISO 639 code "zh"; other languages use
`GetLocaleInfoW`. Only Chinese codes therefore carry a region ("zh-TW",
"zh-HK"); every other language uses the bare two-letter code that the override
list expects. A layout without a resolvable code shows the unknown label.

Caps Lock is read from the documented toggle bit of `GetKeyState(VK_CAPITAL)`
and polled together with the language state, so the badge follows the keyboard
indicator light.

## Behavior

By default the badge appears while there is recent keyboard or mouse input and
is hidden again after the configured linger time. A change of the foreground
language, the IME mode, or the Caps Lock state keeps the badge visible for the
same linger time as well. Setting the visibility mode to "always" keeps it
visible whenever a foreground layout is available.

The badge is a layered, click-through, topmost tool window. It is excluded from
the taskbar and Alt+Tab, and it never takes focus.

## Performance

The overlay thread sleeps while the badge is hidden and wakes only to poll the
language and Caps Lock state, so an idle system costs a small number of timer
wakes per second. While visible, cursor tracking runs at roughly 60 frames per
second, but the GDI+ surface is only redrawn when the label, the colors, or the
monitor DPI change; movement alone uses `SetWindowPos`, which reuses the
existing layered surface.

Input method queries are bounded by a short `SendMessageTimeout`, so a busy or
hung application cannot stall the overlay thread.

## Privacy

The mod reads only the foreground keyboard layout, the IME conversion mode and
the Caps Lock state. It does not read keystrokes, input text, window titles, or
application content, and it never accesses the network.

## Related mods

Cursor Tail draws trails and interaction feedback around the pointer. This mod
is independent of it: it adds an input-language badge and does not share code,
settings, or overlay windows with Cursor Tail, so both mods can be enabled at
the same time.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Behavior:
    - visibility_mode: 0
      $name: Visibility mode
      $name:zh-CN: 显示方式
      $description: 0 shows the badge while there is recent input and hides it after the linger time. 1 keeps it visible whenever a foreground input layout is available.
      $description:zh-CN: 0 表示有近期输入时显示、超过停留时间后隐藏；1 表示只要存在前台输入布局就始终显示。
    - linger_ms: 1400
      $name: Linger time (ms)
      $name:zh-CN: 停留时间（毫秒）
      $description: In visibility mode 0, how long the badge stays visible after the last input, from 200 to 10000 milliseconds.
      $description:zh-CN: 显示方式为 0 时，最后一次输入后徽标继续显示的时间，范围 200 到 10000 毫秒。
    - show_caps_lock: true
      $name: Caps Lock pill
      $name:zh-CN: 大写锁定徽标
      $description: Show a separate pill while Caps Lock is on.
      $description:zh-CN: 大写锁定时显示独立的徽标。
    - offset_x: 18
      $name: Horizontal offset
      $name:zh-CN: 水平偏移
      $description: Horizontal offset from the pointer hotspot to the left edge of the badge, in 96-DPI pixels. Negative values place the badge to the left of the pointer.
      $description:zh-CN: 徽标左边缘相对指针热点的水平偏移，单位为 96 DPI 像素。负值会把徽标放到指针左侧。
    - offset_y: 20
      $name: Vertical offset
      $name:zh-CN: 垂直偏移
      $description: Vertical offset from the pointer hotspot to the top edge of the badge, in 96-DPI pixels. Negative values place the badge above the pointer.
      $description:zh-CN: 徽标上边缘相对指针热点的垂直偏移，单位为 96 DPI 像素。负值会把徽标放到指针上方。
    - state_poll_interval_ms: 120
      $name: Language poll interval (ms)
      $name:zh-CN: 语言轮询间隔（毫秒）
      $description: How often the foreground input language is checked, from 50 to 1000 milliseconds. Lower values react faster and cost slightly more CPU.
      $description:zh-CN: 检查前台输入语言的间隔，范围 50 到 1000 毫秒。数值越低反应越快，CPU 占用略高。
  $name: Behavior
  $name:zh-CN: 行为

- Appearance:
    - background_color: "#1B1D23"
      $name: Background color
      $name:zh-CN: 背景颜色
      $description: Hexadecimal RGB color of the badge background.
      $description:zh-CN: 徽标背景使用的十六进制 RGB 颜色。
    - background_opacity: 88
      $name: Background opacity
      $name:zh-CN: 背景不透明度
      $description: Background opacity from 0 to 100 percent.
      $description:zh-CN: 背景不透明度，范围 0 到 100%。
    - border_color: "#FFFFFF"
      $name: Border color
      $name:zh-CN: 边框颜色
      $description: Hexadecimal RGB color of the badge border.
      $description:zh-CN: 徽标边框使用的十六进制 RGB 颜色。
    - border_opacity: 20
      $name: Border opacity
      $name:zh-CN: 边框不透明度
      $description: Border opacity from 0 to 100 percent. Set to 0 to hide the border.
      $description:zh-CN: 边框不透明度，范围 0 到 100%。设为 0 可隐藏边框。
    - text_color: "#FFFFFF"
      $name: Language text color
      $name:zh-CN: 语言文字颜色
      $description: Hexadecimal RGB color of the language label.
      $description:zh-CN: 语言标签使用的十六进制 RGB 颜色。
    - caps_text_color: "#FFC14D"
      $name: Caps Lock color
      $name:zh-CN: 大写锁定颜色
      $description: Hexadecimal RGB color of the Caps Lock pill text and border.
      $description:zh-CN: 大写锁定徽标的文字与边框颜色。
    - caps_background_opacity: 22
      $name: Caps Lock background opacity
      $name:zh-CN: 大写锁定背景不透明度
      $description: Background tint of the Caps Lock pill, from 0 to 100 percent of the Caps Lock color.
      $description:zh-CN: 大写锁定徽标背景的不透明度，范围 0 到 100%，使用大写锁定颜色着色。
    - font_family: "Segoe UI"
      $name: Font family
      $name:zh-CN: 字体
      $description: Font family used for both labels.
      $description:zh-CN: 两个标签使用的字体。
    - font_size: 13
      $name: Font size
      $name:zh-CN: 字号
      $description: Font size in 96-DPI pixels, from 8 to 32.
      $description:zh-CN: 字号，单位为 96 DPI 像素，范围 8 到 32。
    - padding_x: 7
      $name: Horizontal padding
      $name:zh-CN: 水平内边距
      $description: Horizontal padding between the text and the pill edge, in 96-DPI pixels.
      $description:zh-CN: 文字与徽标边缘之间的水平内边距，单位为 96 DPI 像素。
    - padding_y: 3
      $name: Vertical padding
      $name:zh-CN: 垂直内边距
      $description: Vertical padding between the text and the pill edge, in 96-DPI pixels.
      $description:zh-CN: 文字与徽标边缘之间的垂直内边距，单位为 96 DPI 像素。
    - corner_radius: 8
      $name: Corner radius
      $name:zh-CN: 圆角半径
      $description: Corner radius of the pills in 96-DPI pixels. Large values produce fully rounded ends.
      $description:zh-CN: 徽标圆角半径，单位为 96 DPI 像素。较大的数值会得到完全圆形的端点。
    - gap: 4
      $name: Pill gap
      $name:zh-CN: 徽标间距
      $description: Space between the language pill and the Caps Lock pill, in 96-DPI pixels.
      $description:zh-CN: 语言徽标与大写锁定徽标之间的间距，单位为 96 DPI 像素。
  $name: Appearance
  $name:zh-CN: 外观

- Labels:
    - chinese: "Zh"
      $name: Chinese label
      $name:zh-CN: 中文标签
      $description: Text shown while a Chinese input method is in Chinese mode.
      $description:zh-CN: 中文输入法处于中文模式时显示的文字。
    - english: "En"
      $name: English label
      $name:zh-CN: 英文标签
      $description: Text shown for English mode or an English keyboard layout.
      $description:zh-CN: 英文模式或英文键盘布局时显示的文字。
    - caps_lock: "CL"
      $name: Caps Lock label
      $name:zh-CN: 大写锁定标签
      $description: Text shown while Caps Lock is on.
      $description:zh-CN: 大写锁定开启时显示的文字。
    - unknown: "??"
      $name: Unknown label
      $name:zh-CN: 未知标签
      $description: Text shown when the code of the active input language cannot be resolved.
      $description:zh-CN: 无法解析当前输入语言代码时显示的文字。
    - overrides: [""]
      $name: Language label overrides
      $name:zh-CN: 语言标签覆盖
      $description: One entry per line in the form code=label, for example "ja=日" or "zh-TW=繁". The code is the language code the badge already uses, matched case-insensitively; an exact code such as zh-TW wins over its primary language zh.
      $description:zh-CN: 每行一条，格式为 代码=标签，例如 "ja=日" 或 "zh-TW=繁"。代码即徽标本身使用的语言代码，不区分大小写；精确代码（如 zh-TW）优先于其主语言（如 zh）。
  $name: Labels
  $name:zh-CN: 标签
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <imm.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <windhawk_utils.h>
#include <gdiplus.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

constexpr DWORD kVisibleFrameIntervalMs = 16;
constexpr DWORD kHiddenPollIntervalMs = 120;
constexpr DWORD kImeQueryTimeoutMs = 40;
constexpr DWORD kBackbufferIdleReleaseMs = 4000;
constexpr UINT_PTR kImeMessageTimeoutFlags = SMTO_ABORTIFHUNG;

////////////////////////////////////////////////////////////////////////////////
// Settings
////////////////////////////////////////////////////////////////////////////////

struct IndicatorSettings {
    int visibilityMode;
    int lingerMs;
    int showCapsLock;
    int offsetX;
    int offsetY;
    int statePollIntervalMs;
    uint32_t backgroundRgb;
    float backgroundOpacity;
    uint32_t borderRgb;
    float borderOpacity;
    uint32_t textRgb;
    uint32_t capsTextRgb;
    float capsBackgroundOpacity;
    std::wstring fontFamily;
    float fontSize;
    float paddingX;
    float paddingY;
    float cornerRadius;
    float gap;
    std::wstring chineseLabel;
    std::wstring englishLabel;
    std::wstring capsLabel;
    std::wstring unknownLabel;
};

IndicatorSettings g_settings = {
    0, 1400, 1, 18, 20, 120,
    0x001B1D23u, 0.88f, 0x00FFFFFFu, 0.20f, 0x00FFFFFFu, 0x00FFC14Du, 0.22f,
    L"Segoe UI", 13.0f, 7.0f, 3.0f, 8.0f, 4.0f,
    L"Zh", L"En", L"CL", L"??",
};

////////////////////////////////////////////////////////////////////////////////
// Global state
////////////////////////////////////////////////////////////////////////////////

std::atomic<HWND> g_overlayHwnd{nullptr};
HANDLE g_threadHandle = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_wakeEvent = nullptr;
std::atomic_bool g_settingsDirty{false};

// One "code=label" entry from the Labels.overrides list.
struct LanguageOverride {
    std::wstring code;
    std::wstring label;
};

std::vector<LanguageOverride> g_languageOverrides;

bool g_gdiplusReady = false;
ULONG_PTR g_gdiplusToken = 0;

HBITMAP g_surfaceBitmap = nullptr;
HDC g_surfaceDc = nullptr;
HGDIOBJ g_surfaceOriginalBitmap = nullptr;
void* g_surfaceBits = nullptr;
int g_surfaceWidth = 0;
int g_surfaceHeight = 0;
ULONGLONG g_surfaceIdleSince = 0;

enum class ImeMode : uint8_t {
    Unknown,
    Native,
    Alphanumeric,
};

struct InputState {
    LANGID langId;
    ImeMode imeMode;
    bool hasForeground;
};

InputState g_inputState = {0, ImeMode::Unknown, false};
std::wstring g_renderedLanguage;
std::wstring g_renderedCaps;
float g_renderedScale = 0.0f;
int g_badgeX = 0;
int g_badgeY = 0;
bool g_badgeVisible = false;
bool g_hasPresentedBadge = false;
// Tick of the last detected language or IME mode change, so the badge stays
// visible for the linger time after a change even without further input.
ULONGLONG g_lastStateChangeTick = 0;

////////////////////////////////////////////////////////////////////////////////
// Small helpers
////////////////////////////////////////////////////////////////////////////////

static inline uint8_t ColorRed(uint32_t rgb) {
    return static_cast<uint8_t>((rgb >> 16) & 0xFFu);
}

static inline uint8_t ColorGreen(uint32_t rgb) {
    return static_cast<uint8_t>((rgb >> 8) & 0xFFu);
}

static inline uint8_t ColorBlue(uint32_t rgb) {
    return static_cast<uint8_t>(rgb & 0xFFu);
}

static Gdiplus::Color ToArgb(uint32_t rgb, float alpha) {
    const float clamped = std::clamp(alpha, 0.0f, 1.0f);
    return Gdiplus::Color(
        static_cast<BYTE>(clamped * 255.0f + 0.5f),
        ColorRed(rgb),
        ColorGreen(rgb),
        ColorBlue(rgb));
}

static bool ParseHexColor(const wchar_t* text, uint32_t* outRgb) {
    if (!text || !outRgb) return false;
    while (*text == L' ' || *text == L'\t') ++text;
    if (*text == L'#') ++text;

    uint32_t value = 0;
    int digits = 0;
    for (; digits < 6; ++digits) {
        const wchar_t character = text[digits];
        int nibble;
        if (character >= L'0' && character <= L'9') {
            nibble = character - L'0';
        } else if (character >= L'a' && character <= L'f') {
            nibble = character - L'a' + 10;
        } else if (character >= L'A' && character <= L'F') {
            nibble = character - L'A' + 10;
        } else {
            break;
        }
        value = (value << 4) | static_cast<uint32_t>(nibble);
    }

    if (digits != 6) return false;
    *outRgb = value;
    return true;
}

static std::wstring ReadSettingString(const wchar_t* path, const wchar_t* fallback) {
    WindhawkUtils::StringSetting value = WindhawkUtils::StringSetting::make(path);
    const wchar_t* text = value.get();
    if (!text || !*text) return std::wstring(fallback);
    return std::wstring(text);
}

static uint32_t ReadSettingColor(const wchar_t* path, uint32_t fallback) {
    WindhawkUtils::StringSetting value = WindhawkUtils::StringSetting::make(path);
    uint32_t parsed = 0;
    return ParseHexColor(value.get(), &parsed) ? parsed : fallback;
}

////////////////////////////////////////////////////////////////////////////////
// Language labels
////////////////////////////////////////////////////////////////////////////////

static constexpr int kMaxLanguageOverrideEntries = 64;

static std::wstring TrimWhitespace(const std::wstring& text) {
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && iswspace(text[begin]) != 0) ++begin;
    while (end > begin && iswspace(text[end - 1]) != 0) --end;
    return text.substr(begin, end - begin);
}

static std::wstring ToLowerCode(const std::wstring& text) {
    std::wstring lowered = text;
    for (wchar_t& character : lowered) {
        character = static_cast<wchar_t>(towlower(character));
    }
    return lowered;
}

// The Chinese sub-languages share the ISO 639-1 code "zh", so the region has
// to come from the sub-language of the layout instead of the locale name.
static std::wstring ChineseLanguageCode(LANGID langId) {
    switch (SUBLANGID(langId)) {
        case SUBLANG_CHINESE_TRADITIONAL:
            return std::wstring(L"zh-TW");
        case SUBLANG_CHINESE_HONGKONG:
            return std::wstring(L"zh-HK");
        case SUBLANG_CHINESE_MACAU:
            return std::wstring(L"zh-MO");
        case SUBLANG_CHINESE_SINGAPORE:
            return std::wstring(L"zh-SG");
        default:
            return std::wstring(L"zh");
    }
}

// Builds the lower-case language code used to look up an override, for
// example "zh-TW" or "ja". An empty result means the layout has no known code.
static std::wstring ResolveLanguageCode(LANGID langId) {
    if (PRIMARYLANGID(langId) == LANG_CHINESE) {
        return ChineseLanguageCode(langId);
    }

    WCHAR code[LOCALE_NAME_MAX_LENGTH] = {};
    const LCID locale = MAKELCID(langId, SORT_DEFAULT);
    int length = GetLocaleInfoW(
        locale, LOCALE_SISO639LANGNAME, code, ARRAYSIZE(code));
    if (length <= 1) {
        length = GetLocaleInfoW(
            locale, LOCALE_SABBREVLANGNAME, code, ARRAYSIZE(code));
    }
    if (length <= 1) return std::wstring();
    return ToLowerCode(TrimWhitespace(std::wstring(code)));
}

// "zh-TW" becomes "Zh-TW": the language part keeps a capital first letter and
// the region part is fully upper case.
static std::wstring BuiltInLabelForCode(const std::wstring& code) {
    if (code.empty()) return std::wstring();

    std::wstring label = code;
    bool inRegion = false;
    bool atSegmentStart = true;
    for (wchar_t& character : label) {
        if (character == L'-' || character == L'_') {
            character = L'-';
            inRegion = true;
            atSegmentStart = true;
            continue;
        }
        if (inRegion) {
            character = static_cast<wchar_t>(towupper(character));
        } else {
            character = static_cast<wchar_t>(
                atSegmentStart ? towupper(character) : towlower(character));
            atSegmentStart = false;
        }
    }
    return label;
}

// One "code=label" list entry, for example "fr=Fr" or "zh=中".
static bool ParseLanguageOverrideEntry(
    const std::wstring& text, LanguageOverride* parsed) {
    if (!parsed) return false;

    const size_t separator = text.find(L'=');
    if (separator == std::wstring::npos) return false;

    std::wstring code = ToLowerCode(TrimWhitespace(text.substr(0, separator)));
    std::wstring label = TrimWhitespace(text.substr(separator + 1));
    if (code.empty() || label.empty()) return false;

    parsed->code = std::move(code);
    parsed->label = std::move(label);
    return true;
}

static void ParseLanguageOverrides(std::vector<LanguageOverride>* parsedOverrides) {
    if (!parsedOverrides) return;
    parsedOverrides->clear();

    int consecutiveEmptyEntries = 0;
    for (int index = 0; index < kMaxLanguageOverrideEntries; ++index) {
        WindhawkUtils::StringSetting setting =
            WindhawkUtils::StringSetting::make(L"Labels.overrides[%d]", index);
        const wchar_t* text = setting.get();
        if (!text || !*text) {
            if (++consecutiveEmptyEntries >= 2) break;
            continue;
        }

        consecutiveEmptyEntries = 0;
        LanguageOverride entry;
        if (ParseLanguageOverrideEntry(std::wstring(text), &entry)) {
            parsedOverrides->push_back(std::move(entry));
        }
    }
}

// Copies the matching label. An exact code wins over the primary language, so
// "zh-TW=繁" refines a broader "zh=中". Override codes are stored lower case
// and compared without case, because language codes arrive with an upper-case
// region such as "zh-TW".
static bool TryFindLanguageOverride(const std::wstring& code, std::wstring* label) {
    if (code.empty() || !label) return false;

    for (const LanguageOverride& entry : g_languageOverrides) {
        if (_wcsicmp(entry.code.c_str(), code.c_str()) == 0) {
            *label = entry.label;
            return true;
        }
    }

    const size_t separator = code.find(L'-');
    if (separator == std::wstring::npos) return false;

    const std::wstring primary = code.substr(0, separator);
    for (const LanguageOverride& entry : g_languageOverrides) {
        if (_wcsicmp(entry.code.c_str(), primary.c_str()) == 0) {
            *label = entry.label;
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Settings loading
////////////////////////////////////////////////////////////////////////////////

static void LoadSettings() {
    IndicatorSettings next = g_settings;

    next.visibilityMode = std::clamp(Wh_GetIntSetting(L"Behavior.visibility_mode"), 0, 1);
    next.lingerMs = std::clamp(Wh_GetIntSetting(L"Behavior.linger_ms"), 200, 10000);
    next.showCapsLock = std::clamp(Wh_GetIntSetting(L"Behavior.show_caps_lock"), 0, 1);
    next.offsetX = std::clamp(Wh_GetIntSetting(L"Behavior.offset_x"), -200, 200);
    next.offsetY = std::clamp(Wh_GetIntSetting(L"Behavior.offset_y"), -200, 200);
    next.statePollIntervalMs =
        std::clamp(Wh_GetIntSetting(L"Behavior.state_poll_interval_ms"), 50, 1000);

    next.backgroundRgb = ReadSettingColor(L"Appearance.background_color", 0x001B1D23u);
    next.backgroundOpacity =
        std::clamp(Wh_GetIntSetting(L"Appearance.background_opacity"), 0, 100) / 100.0f;
    next.borderRgb = ReadSettingColor(L"Appearance.border_color", 0x00FFFFFFu);
    next.borderOpacity =
        std::clamp(Wh_GetIntSetting(L"Appearance.border_opacity"), 0, 100) / 100.0f;
    next.textRgb = ReadSettingColor(L"Appearance.text_color", 0x00FFFFFFu);
    next.capsTextRgb = ReadSettingColor(L"Appearance.caps_text_color", 0x00FFC14Du);
    next.capsBackgroundOpacity =
        std::clamp(Wh_GetIntSetting(L"Appearance.caps_background_opacity"), 0, 100) / 100.0f;
    next.fontFamily = ReadSettingString(L"Appearance.font_family", L"Segoe UI");
    next.fontSize = static_cast<float>(
        std::clamp(Wh_GetIntSetting(L"Appearance.font_size"), 8, 32));
    next.paddingX = static_cast<float>(
        std::clamp(Wh_GetIntSetting(L"Appearance.padding_x"), 0, 40));
    next.paddingY = static_cast<float>(
        std::clamp(Wh_GetIntSetting(L"Appearance.padding_y"), 0, 40));
    next.cornerRadius = static_cast<float>(
        std::clamp(Wh_GetIntSetting(L"Appearance.corner_radius"), 0, 32));
    next.gap = static_cast<float>(std::clamp(Wh_GetIntSetting(L"Appearance.gap"), 0, 40));

    next.chineseLabel = ReadSettingString(L"Labels.chinese", L"Zh");
    next.englishLabel = ReadSettingString(L"Labels.english", L"En");
    next.capsLabel = ReadSettingString(L"Labels.caps_lock", L"CL");
    next.unknownLabel = ReadSettingString(L"Labels.unknown", L"??");

    std::vector<LanguageOverride> nextOverrides;
    ParseLanguageOverrides(&nextOverrides);

    g_languageOverrides = std::move(nextOverrides);
    g_settings = next;
    g_renderedScale = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////
// Input state detection
////////////////////////////////////////////////////////////////////////////////

// The MinGW imm.h that Windhawk ships does not declare the WM_IME_CONTROL
// sub-commands, so mirror the values from the Windows SDK. The guards keep the
// SDK values if a future header defines them.
#ifndef IMC_GETCONVERSIONMODE
#define IMC_GETCONVERSIONMODE 0x0001
#endif
#ifndef IMC_GETOPENSTATUS
#define IMC_GETOPENSTATUS 0x0005
#endif

// Microsoft's Chinese, Japanese and Korean input methods keep the native and
// the Latin mode on one and the same keyboard layout, so only they need the
// extra IME conversion-mode query. Every other layout reports its language.
static bool IsImeToggleLanguage(LANGID langId) {
    const LANGID primary = PRIMARYLANGID(langId);
    return primary == LANG_CHINESE || primary == LANG_JAPANESE
        || primary == LANG_KOREAN;
}

// The focused child window owns the IME context in most applications, and it
// can belong to a thread other than the one that owns the foreground window,
// so ask the GUI thread for its focus window before falling back.
static HWND ResolveQueryWindow(HWND foreground) {
    if (!foreground) return nullptr;

    const DWORD threadId = GetWindowThreadProcessId(foreground, nullptr);
    if (threadId) {
        GUITHREADINFO info = {};
        info.cbSize = sizeof(info);
        if (GetGUIThreadInfo(threadId, &info) && info.hwndFocus) {
            return info.hwndFocus;
        }
    }
    return foreground;
}

// The IME open status decides whether the layout types its native language at
// all. Microsoft's Chinese IME keeps IME_CMODE_NATIVE in its conversion mode
// after it is closed, so reading the conversion mode first would report "Zh"
// while the user is typing English. A closed IME is Latin input regardless of
// the conversion mode that it remembers.
static ImeMode ImeModeFromStatus(bool isOpen, DWORD conversionMode) {
    if (!isOpen) return ImeMode::Alphanumeric;
    return (conversionMode & IME_CMODE_NATIVE) != 0 ? ImeMode::Native
                                                    : ImeMode::Alphanumeric;
}

// Returns true when the IME window answered. SendMessageTimeoutW returns a
// nonzero value when the message was handled even if the IME answered zero,
// so the result value and the answer stay separate.
static bool QueryImeWindow(HWND window, ImeMode* mode) {
    if (!window || !mode) return false;

    HWND imeWindow = ImmGetDefaultIMEWnd(window);
    if (!imeWindow) return false;

    DWORD_PTR openStatus = 0;
    const LRESULT openResult = SendMessageTimeoutW(
        imeWindow,
        WM_IME_CONTROL,
        IMC_GETOPENSTATUS,
        0,
        kImeMessageTimeoutFlags,
        kImeQueryTimeoutMs,
        &openStatus);
    if (openResult == 0) return false;

    // A closed IME does not need the conversion mode, and the mode it keeps
    // from the last native session must not override the closed state.
    if (openStatus == 0) {
        *mode = ImeMode::Alphanumeric;
        return true;
    }

    DWORD_PTR conversionMode = 0;
    const LRESULT conversionResult = SendMessageTimeoutW(
        imeWindow,
        WM_IME_CONTROL,
        IMC_GETCONVERSIONMODE,
        0,
        kImeMessageTimeoutFlags,
        kImeQueryTimeoutMs,
        &conversionMode);
    if (conversionResult == 0) return false;

    *mode = ImeModeFromStatus(true, static_cast<DWORD>(conversionMode));
    return true;
}

// Fallback for windows whose IME context is reachable from this process.
static bool QueryImeContext(HWND window, ImeMode* mode) {
    if (!window || !mode) return false;

    HIMC context = ImmGetContext(window);
    if (!context) return false;

    const BOOL isOpen = ImmGetOpenStatus(context);
    DWORD conversionMode = 0;
    DWORD sentenceMode = 0;
    const BOOL statusOk = isOpen
        ? ImmGetConversionStatus(context, &conversionMode, &sentenceMode)
        : TRUE;
    ImmReleaseContext(window, context);
    if (!statusOk) return false;

    *mode = ImeModeFromStatus(isOpen != FALSE, conversionMode);
    return true;
}

static ImeMode QueryImeMode(HWND queryWindow, HWND foreground) {
    ImeMode mode = ImeMode::Unknown;
    if (QueryImeWindow(queryWindow, &mode)) return mode;
    if (queryWindow != foreground && QueryImeWindow(foreground, &mode)) return mode;
    if (QueryImeContext(queryWindow, &mode)) return mode;
    if (queryWindow != foreground && QueryImeContext(foreground, &mode)) return mode;
    return ImeMode::Unknown;
}

static InputState QueryInputState() {
    InputState state = {0, ImeMode::Unknown, false};

    HWND foreground = GetForegroundWindow();
    if (!foreground) return state;
    if (foreground == g_overlayHwnd.load(std::memory_order_acquire)) return state;

    const HWND queryWindow = ResolveQueryWindow(foreground);
    const DWORD threadId = GetWindowThreadProcessId(queryWindow, nullptr);
    if (!threadId) return state;

    const HKL layout = GetKeyboardLayout(threadId);
    if (!layout) return state;

    state.langId =
        static_cast<LANGID>(reinterpret_cast<UINT_PTR>(layout) & 0xFFFFu);
    if (state.langId == 0) return state;

    state.hasForeground = true;
    if (IsImeToggleLanguage(state.langId) && ImmIsIME(layout)) {
        state.imeMode = QueryImeMode(queryWindow, foreground);
    }

    return state;
}

static bool SameInputState(const InputState& left, const InputState& right) {
    return left.langId == right.langId
        && left.imeMode == right.imeMode
        && left.hasForeground == right.hasForeground;
}

static std::wstring BuildLanguageLabel(const InputState& state) {
    if (!state.hasForeground) return std::wstring();

    // The native/Latin switch of an IME decides the label before the language
    // does: a Chinese IME in Latin mode types English and shows "En".
    if (IsImeToggleLanguage(state.langId)
        && state.imeMode == ImeMode::Alphanumeric) {
        return g_settings.englishLabel;
    }

    const std::wstring code = ResolveLanguageCode(state.langId);
    std::wstring overrideLabel;
    if (!code.empty() && TryFindLanguageOverride(code, &overrideLabel)) {
        return overrideLabel;
    }
    if (code.empty()) return g_settings.unknownLabel;

    if (code == L"zh") return g_settings.chineseLabel;
    if (PRIMARYLANGID(state.langId) == LANG_ENGLISH) return g_settings.englishLabel;
    return BuiltInLabelForCode(code);
}

static bool QueryCapsLock() {
    // The low-order bit of GetKeyState is the documented toggle state, and
    // Windows keeps it synchronized for the lock keys across processes. The
    // low-order bit of GetAsyncKeyState means "pressed since the previous
    // query" instead, and is documented as unreliable, so it cannot report the
    // Caps Lock state.
    return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}

static bool HasRecentInput(int lingerMs) {
    LASTINPUTINFO info = {};
    info.cbSize = sizeof(info);
    if (!GetLastInputInfo(&info)) return false;
    return (GetTickCount() - info.dwTime) <= static_cast<DWORD>(lingerMs);
}

////////////////////////////////////////////////////////////////////////////////
// DPI helpers
////////////////////////////////////////////////////////////////////////////////

static void EnablePerMonitorDpiAwareness() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) return;

    using SetThreadDpiAwarenessContext_t = HANDLE(WINAPI*)(HANDLE);
    auto setContext = reinterpret_cast<SetThreadDpiAwarenessContext_t>(
        GetProcAddress(user32, "SetThreadDpiAwarenessContext"));
    if (!setContext) return;

    // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 is (HANDLE)-4.
    constexpr INT_PTR kPerMonitorAwareV2 = -4;
    setContext(reinterpret_cast<HANDLE>(kPerMonitorAwareV2));
}

static float GetScaleForPoint(const POINT& point) {
    HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96;
    UINT dpiY = 96;
    if (monitor && SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))
        && dpiX > 0) {
        return static_cast<float>(dpiX) / 96.0f;
    }
    return 1.0f;
}

static void ClampBadgePosition(const POINT& cursor, float scale, int* x, int* y) {
    int left = cursor.x + static_cast<int>(std::lround(g_settings.offsetX * scale));
    int top = cursor.y + static_cast<int>(std::lround(g_settings.offsetY * scale));
    const int width = g_surfaceWidth;
    const int height = g_surfaceHeight;

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (GetMonitorInfoW(
            MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST),
            &monitorInfo)) {
        const RECT& work = monitorInfo.rcWork;
        if (left + width > work.right) {
            left = cursor.x - static_cast<int>(std::lround(g_settings.offsetX * scale)) - width;
        }
        if (top + height > work.bottom) {
            top = cursor.y - static_cast<int>(std::lround(g_settings.offsetY * scale)) - height;
        }
        const int workLeft = static_cast<int>(work.left);
        const int workTop = static_cast<int>(work.top);
        const int maxLeft =
            std::max(workLeft, static_cast<int>(work.right) - width);
        const int maxTop =
            std::max(workTop, static_cast<int>(work.bottom) - height);
        left = std::clamp(left, workLeft, maxLeft);
        top = std::clamp(top, workTop, maxTop);
    }

    *x = left;
    *y = top;
}

////////////////////////////////////////////////////////////////////////////////
// Surface management
////////////////////////////////////////////////////////////////////////////////

static void ReleaseSurface() {
    if (g_surfaceDc) {
        if (g_surfaceOriginalBitmap) {
            SelectObject(g_surfaceDc, g_surfaceOriginalBitmap);
            g_surfaceOriginalBitmap = nullptr;
        }
        DeleteDC(g_surfaceDc);
        g_surfaceDc = nullptr;
    }
    if (g_surfaceBitmap) {
        DeleteObject(g_surfaceBitmap);
        g_surfaceBitmap = nullptr;
    }
    g_surfaceBits = nullptr;
    g_surfaceWidth = 0;
    g_surfaceHeight = 0;
    g_surfaceIdleSince = 0;
}

static bool EnsureSurface(int width, int height) {
    if (width <= 0 || height <= 0) return false;
    if (g_surfaceDc && g_surfaceBitmap && g_surfaceWidth == width
        && g_surfaceHeight == height) {
        g_surfaceIdleSince = 0;
        return true;
    }

    ReleaseSurface();

    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(
        nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) DeleteObject(bitmap);
        return false;
    }

    HDC memoryDc = CreateCompatibleDC(nullptr);
    if (!memoryDc) {
        DeleteObject(bitmap);
        return false;
    }

    g_surfaceOriginalBitmap = SelectObject(memoryDc, bitmap);
    g_surfaceBitmap = bitmap;
    g_surfaceDc = memoryDc;
    g_surfaceBits = bits;
    g_surfaceWidth = width;
    g_surfaceHeight = height;
    g_surfaceIdleSince = 0;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Badge rendering
////////////////////////////////////////////////////////////////////////////////

static void AddRoundedRect(
    Gdiplus::GraphicsPath& path,
    const Gdiplus::RectF& rect,
    float radius) {
    const float diameter =
        std::min(radius * 2.0f, std::min(rect.Width, rect.Height));
    if (diameter <= 0.01f) {
        path.AddRectangle(rect);
        return;
    }

    path.AddArc(rect.X, rect.Y, diameter, diameter, 180.0f, 90.0f);
    path.AddArc(
        rect.X + rect.Width - diameter, rect.Y, diameter, diameter, 270.0f, 90.0f);
    path.AddArc(
        rect.X + rect.Width - diameter,
        rect.Y + rect.Height - diameter,
        diameter,
        diameter,
        0.0f,
        90.0f);
    path.AddArc(
        rect.X, rect.Y + rect.Height - diameter, diameter, diameter, 90.0f, 90.0f);
    path.CloseFigure();
}

static bool RenderBadge(
    const std::wstring& languageLabel,
    const std::wstring& capsLabel,
    float scale) {
    if (languageLabel.empty()) return false;
    if (!g_gdiplusReady) return false;

    const bool showCaps = !capsLabel.empty();
    const float scaledFontSize = g_settings.fontSize * scale;
    const float paddingX = g_settings.paddingX * scale;
    const float paddingY = g_settings.paddingY * scale;
    const float gap = g_settings.gap * scale;
    const float radius = g_settings.cornerRadius * scale;

    Gdiplus::Bitmap measureBitmap(1, 1, PixelFormat32bppPARGB);
    Gdiplus::Graphics measure(&measureBitmap);
    measure.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

    Gdiplus::Font font(
        g_settings.fontFamily.c_str(),
        scaledFontSize,
        Gdiplus::FontStyleRegular,
        Gdiplus::UnitPixel);
    if (font.GetLastStatus() != Gdiplus::Ok) return false;

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    format.SetFormatFlags(
        Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsNoClip);

    const Gdiplus::RectF measureRect(0.0f, 0.0f, 4096.0f, 4096.0f);
    Gdiplus::RectF languageBounds;
    if (measure.MeasureString(
            languageLabel.c_str(),
            static_cast<INT>(languageLabel.size()),
            &font,
            measureRect,
            &format,
            &languageBounds) != Gdiplus::Ok) {
        return false;
    }

    // MeasureString reserves roughly one sixth of an em of side bearing on
    // each side. Removing it keeps the pill padding visually consistent.
    const float sideBearing = scaledFontSize / 6.0f;
    const float languageWidth =
        std::max(scaledFontSize, languageBounds.Width - sideBearing) + paddingX * 2.0f;
    const float lineHeight = font.GetHeight(&measure);
    const float pillHeight = lineHeight + paddingY * 2.0f;

    float capsWidth = 0.0f;
    Gdiplus::RectF capsBounds;
    if (showCaps) {
        if (measure.MeasureString(
                capsLabel.c_str(),
                static_cast<INT>(capsLabel.size()),
                &font,
                measureRect,
                &format,
                &capsBounds) != Gdiplus::Ok) {
            return false;
        }
        capsWidth =
            std::max(scaledFontSize, capsBounds.Width - sideBearing) + paddingX * 2.0f;
    }

    const float totalWidth = languageWidth + (showCaps ? gap + capsWidth : 0.0f);
    const int width = static_cast<int>(std::ceil(totalWidth));
    const int height = static_cast<int>(std::ceil(pillHeight));
    if (!EnsureSurface(width, height)) return false;

    Gdiplus::Bitmap target(
        width,
        height,
        width * 4,
        PixelFormat32bppPARGB,
        static_cast<BYTE*>(g_surfaceBits));
    if (target.GetLastStatus() != Gdiplus::Ok) return false;

    Gdiplus::Graphics graphics(&target);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
    graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

    const Gdiplus::RectF languageRect(0.0f, 0.0f, languageWidth, pillHeight);
    Gdiplus::GraphicsPath languagePath;
    AddRoundedRect(languagePath, languageRect, radius);
    Gdiplus::SolidBrush backgroundBrush(
        ToArgb(g_settings.backgroundRgb, g_settings.backgroundOpacity));
    graphics.FillPath(&backgroundBrush, &languagePath);
    if (g_settings.borderOpacity > 0.0f) {
        Gdiplus::Pen borderPen(
            ToArgb(g_settings.borderRgb, g_settings.borderOpacity),
            std::max(1.0f, scale));
        graphics.DrawPath(&borderPen, &languagePath);
    }
    Gdiplus::SolidBrush textBrush(ToArgb(g_settings.textRgb, 1.0f));
    graphics.DrawString(
        languageLabel.c_str(),
        static_cast<INT>(languageLabel.size()),
        &font,
        languageRect,
        &format,
        &textBrush);

    if (showCaps) {
        const Gdiplus::RectF capsRect(
            languageWidth + gap, 0.0f, capsWidth, pillHeight);
        Gdiplus::GraphicsPath capsPath;
        AddRoundedRect(capsPath, capsRect, radius);
        Gdiplus::SolidBrush capsBackgroundBrush(
            ToArgb(g_settings.capsTextRgb, g_settings.capsBackgroundOpacity));
        graphics.FillPath(&capsBackgroundBrush, &capsPath);
        Gdiplus::Pen capsBorderPen(
            ToArgb(g_settings.capsTextRgb, 0.55f), std::max(1.0f, scale));
        graphics.DrawPath(&capsBorderPen, &capsPath);
        Gdiplus::SolidBrush capsTextBrush(ToArgb(g_settings.capsTextRgb, 1.0f));
        graphics.DrawString(
            capsLabel.c_str(),
            static_cast<INT>(capsLabel.size()),
            &font,
            capsRect,
            &format,
            &capsTextBrush);
    }

    graphics.Flush(Gdiplus::FlushIntentionSync);
    g_renderedLanguage = languageLabel;
    g_renderedCaps = capsLabel;
    g_renderedScale = scale;
    return true;
}

static bool PresentBadge(int x, int y) {
    HWND hwnd = g_overlayHwnd.load(std::memory_order_acquire);
    if (!hwnd || !g_surfaceDc) return false;

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) return false;

    POINT destination = {x, y};
    SIZE size = {g_surfaceWidth, g_surfaceHeight};
    POINT source = {0, 0};
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    const BOOL updated = UpdateLayeredWindow(
        hwnd,
        screenDc,
        &destination,
        &size,
        g_surfaceDc,
        &source,
        0,
        &blend,
        ULW_ALPHA);
    ReleaseDC(nullptr, screenDc);

    if (!updated) return false;
    g_badgeX = x;
    g_badgeY = y;
    g_hasPresentedBadge = true;
    return true;
}

static void MoveBadge(int x, int y) {
    HWND hwnd = g_overlayHwnd.load(std::memory_order_acquire);
    if (!hwnd || !g_hasPresentedBadge) return;
    if (x == g_badgeX && y == g_badgeY) return;
    if (SetWindowPos(
            hwnd,
            nullptr,
            x,
            y,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER)) {
        g_badgeX = x;
        g_badgeY = y;
    }
}

static void SetBadgeVisible(bool visible) {
    HWND hwnd = g_overlayHwnd.load(std::memory_order_acquire);
    if (!hwnd) return;
    if (g_badgeVisible == visible) return;
    ShowWindow(hwnd, visible ? SW_SHOWNA : SW_HIDE);
    g_badgeVisible = visible;
    if (!visible) {
        g_surfaceIdleSince = GetTickCount64();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Overlay window
////////////////////////////////////////////////////////////////////////////////

static LRESULT CALLBACK OverlayWndProc(
    HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static void ResetBadgeState() {
    g_renderedLanguage.clear();
    g_renderedCaps.clear();
    g_renderedScale = 0.0f;
    g_hasPresentedBadge = false;
    g_badgeX = 0;
    g_badgeY = 0;
    g_inputState = {0, ImeMode::Unknown, false};
}

static void UpdateBadge() {
    if (!g_inputState.hasForeground) {
        SetBadgeVisible(false);
        return;
    }

    const DWORD lingerMs = static_cast<DWORD>(g_settings.lingerMs);
    const bool recentInput = HasRecentInput(g_settings.lingerMs);
    const bool recentStateChange = g_lastStateChangeTick != 0
        && (GetTickCount64() - g_lastStateChangeTick) <= lingerMs;
    const bool visible = g_settings.visibilityMode == 1
        || recentInput
        || recentStateChange;
    if (!visible) {
        SetBadgeVisible(false);
        return;
    }

    POINT cursor = {};
    if (!GetCursorPos(&cursor)) {
        SetBadgeVisible(false);
        return;
    }

    const float scale = GetScaleForPoint(cursor);
    const std::wstring languageLabel = BuildLanguageLabel(g_inputState);
    const std::wstring capsLabel =
        (g_settings.showCapsLock && QueryCapsLock()) ? g_settings.capsLabel : std::wstring();

    const bool contentChanged =
        languageLabel != g_renderedLanguage
        || capsLabel != g_renderedCaps
        || std::fabs(scale - g_renderedScale) > 0.01f
        || !g_hasPresentedBadge;

    if (contentChanged) {
        if (!RenderBadge(languageLabel, capsLabel, scale)) {
            SetBadgeVisible(false);
            return;
        }
    }

    int badgeX = 0;
    int badgeY = 0;
    ClampBadgePosition(cursor, scale, &badgeX, &badgeY);

    if (contentChanged || !g_hasPresentedBadge) {
        if (!PresentBadge(badgeX, badgeY)) {
            SetBadgeVisible(false);
            return;
        }
    } else {
        MoveBadge(badgeX, badgeY);
    }

    SetBadgeVisible(true);
}

static void MaybeReleaseIdleSurface() {
    if (g_badgeVisible || !g_surfaceDc) {
        g_surfaceIdleSince = 0;
        return;
    }
    const ULONGLONG now = GetTickCount64();
    if (g_surfaceIdleSince == 0) {
        g_surfaceIdleSince = now;
        return;
    }
    if (now - g_surfaceIdleSince >= kBackbufferIdleReleaseMs) {
        ReleaseSurface();
        ResetBadgeState();
    }
}

DWORD WINAPI OverlayThreadProc(LPVOID) {
    Gdiplus::GdiplusStartupInput gdiplusInput;
    if (Gdiplus::GdiplusStartup(
            &g_gdiplusToken, &gdiplusInput, nullptr) != Gdiplus::Ok) {
        Wh_Log(L"GdiplusStartup failed");
        g_gdiplusToken = 0;
        return 0;
    }
    g_gdiplusReady = true;

    EnablePerMonitorDpiAwareness();

    HINSTANCE instance = GetModuleHandleW(nullptr);
    const wchar_t className[] = L"CursorImeIndicatorOverlay";
    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = OverlayWndProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    const ATOM atom = RegisterClassW(&windowClass);
    if (!atom) {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        g_gdiplusReady = false;
        return 0;
    }

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW
            | WS_EX_NOACTIVATE,
        className,
        L"CursorImeIndicatorOverlay",
        WS_POPUP,
        0,
        0,
        1,
        1,
        nullptr,
        nullptr,
        instance,
        nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        UnregisterClassW(className, instance);
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        g_gdiplusReady = false;
        return 0;
    }

    g_overlayHwnd.store(hwnd, std::memory_order_release);
    if (g_settingsDirty.exchange(false, std::memory_order_acq_rel)) {
        LoadSettings();
    }

    ULONGLONG nextStatePoll = 0;
    bool forceStatePoll = true;
    HWND previousForeground = nullptr;

    bool running = true;
    while (running) {
        const DWORD waitMs =
            g_badgeVisible ? kVisibleFrameIntervalMs : kHiddenPollIntervalMs;
        HANDLE handles[2] = {g_stopEvent, g_wakeEvent};
        const DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE, waitMs);
        if (waitResult == WAIT_OBJECT_0) {
            running = false;
            break;
        }
        if (waitResult == WAIT_FAILED) {
            Wh_Log(L"WaitForMultipleObjects failed: %lu", GetLastError());
            running = false;
            break;
        }

        if (g_settingsDirty.exchange(false, std::memory_order_acq_rel)) {
            LoadSettings();
            forceStatePoll = true;
        }

        const ULONGLONG now = GetTickCount64();
        const HWND foreground = GetForegroundWindow();
        if (foreground != previousForeground) {
            previousForeground = foreground;
            forceStatePoll = true;
        }

        const DWORD pollInterval =
            static_cast<DWORD>(std::max(g_settings.statePollIntervalMs, 50));
        if (forceStatePoll || now >= nextStatePoll) {
            forceStatePoll = false;
            const InputState previous = g_inputState;
            g_inputState = QueryInputState();
            nextStatePoll = now + pollInterval;

            if (!SameInputState(previous, g_inputState)) {
                g_lastStateChangeTick = GetTickCount64();
            }
        }

        if (!g_inputState.hasForeground) {
            SetBadgeVisible(false);
        } else {
            UpdateBadge();
        }

        MaybeReleaseIdleSurface();
    }

    SetBadgeVisible(false);
    ReleaseSurface();
    ResetBadgeState();
    g_overlayHwnd.store(nullptr, std::memory_order_release);
    DestroyWindow(hwnd);
    UnregisterClassW(className, instance);
    g_gdiplusReady = false;
    Gdiplus::GdiplusShutdown(g_gdiplusToken);
    g_gdiplusToken = 0;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod entry points
////////////////////////////////////////////////////////////////////////////////

BOOL WhTool_ModInit() {
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return FALSE;
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_wakeEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    g_threadHandle =
        CreateThread(nullptr, 0, OverlayThreadProc, nullptr, 0, nullptr);
    if (!g_threadHandle) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_threadHandle) {
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = nullptr;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    g_settingsDirty.store(true, std::memory_order_release);
    if (g_wakeEvent) SetEvent(g_wakeEvent);
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
    switch (GetModuleFileName(
        nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
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

    STARTUPINFO si = {};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
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
