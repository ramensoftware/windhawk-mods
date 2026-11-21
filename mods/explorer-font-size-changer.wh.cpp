// ==WindhawkMod==
// @id              explorer-font-size-changer
// @name            Explorer Font & Size Changer
// @name:zh-CN      资源管理器字体与字号修改
// @description     Change the GDI font face and point size in Windows 11 File Explorer.
// @description:zh-CN 修改 Windows 11 资源管理器中 GDI 文字的字体与字号。
// @version         1.0
// @author          wodenwhel
// @github          https://github.com/wodenwhel
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Font & Size Changer

Change the font face and point size of GDI text in Windows 11 File Explorer.

## How it works

Every GDI font starts as a `LOGFONTW`. The mod hooks the four places Windows
creates one: `CreateFontIndirectW`, `CreateFontW`, `CreateFontIndirectExW` and
`uxtheme!GetThemeFont`. It replaces `lfFaceName` and scales `lfHeight` and
`lfWidth` in a private copy before the font is built. Each font is scaled
exactly once, relative to Windows' 9 pt UI baseline, so Explorer's own DPI
scaling and the size difference between headings and body text are preserved.
Text measurement, row height and truncation all follow the resulting metrics.

`CreateFontIndirectExW` and `GetThemeFont` extend this to DirectUI and themed
controls, including the left navigation pane.

Symbol and icon fonts are excluded, preventing glyph substitution that would
render icons as empty squares. The vertical and horizontal offsets adjust only
the drawing origin, keeping text aligned with fixed-size icon bitmaps; layout
measurement is unaffected.

## Screenshot

| Before | After |
| --- | --- |
| ![Before: default Explorer font](https://raw.githubusercontent.com/wodenwhel/images/e15776dd9ec89c68fe3865b7c2f41b1b35cf5238/windhawk/explorer-font-size-changer-Before.png) | ![After: custom font face and size](https://raw.githubusercontent.com/wodenwhel/images/e15776dd9ec89c68fe3865b7c2f41b1b35cf5238/windhawk/explorer-font-size-changer-After.png) |

## Settings

| Setting | Default | Description |
| --- | --- | --- |
| `fontFace` | `None` | An installed font face name, up to 31 characters. `None` or empty keeps the original face |
| `fontSizePt` | `0` | `0` keeps the original size; otherwise 6-72 scales all text proportionally from Windows' 9 pt UI baseline, preserving relative sizes |
| `verticalOffsetPx` | `0` | Shifts drawn text vertically. Negative moves up, positive moves down; `0` disables the shift |
| `horizontalOffsetPx` | `0` | Shifts drawn text horizontally. Negative moves left, positive moves right; `0` disables the shift |
| `skipSymbolFonts` | on | Leaves Marlett, Segoe MDL2 Assets, Wingdings, etc. unchanged so icons do not turn into boxes |

## Notes and limitations

- Font changes apply to fonts created after the mod is loaded. Restart
  `explorer.exe` (or sign out and back in) after installing, changing settings
  or disabling the mod so cached UI fonts are recreated.
- The size setting scales proportionally from Windows' 9 pt UI baseline, so
  headings, captions and body text keep their relative sizes on mixed-DPI
  monitors.
- Some Windows 11 surfaces use XAML / DirectWrite rather than GDI. They keep
  their original fonts.
- The two offset settings shift GDI-drawn text only. They do not change icon
  bitmaps, row heights or column widths, so large values can push text outside
  its row or cell.
- Requires x86-64 Windhawk 1.7.3 or newer.

## Credits

Font substitution approach inspired by [Gabriela Cristei](https://github.com/cristeigabriela) and *Explorer Font Changer*.

---

# 资源管理器字体与字号修改

修改 Windows 11 资源管理器中 GDI 文字的字体与字号。

## 工作原理

每个 GDI 字体都由 `LOGFONTW` 创建。本 mod 在 Windows 建字体的四个入口挂钩：`CreateFontIndirectW`、`CreateFontW`、`CreateFontIndirectExW` 与 `uxtheme!GetThemeFont`，并在创建前替换私有副本中的 `lfFaceName`、按比例缩放 `lfHeight` 与 `lfWidth`。每个字体只缩放一次，缩放以 Windows 标准 9 磅界面文字为基准，资源管理器自身的 DPI 换算以及标题与正文的大小层级均予保留。文字测量、行高计算与长串截断均基于所得字体的度量。

`CreateFontIndirectExW` 与 `GetThemeFont` 把覆盖范围扩展到 DirectUI 与主题化控件，包括左侧导航窗格。

符号与图标字体均予排除，避免图标因字体替换而显示为方框。垂直与水平偏移仅调整文字的绘制原点，使其与固定尺寸的图标位图保持对齐，布局测量不受影响。

## 效果截图

| 修改前 | 修改后 |
| --- | --- |
| ![修改前：资源管理器默认字体](https://raw.githubusercontent.com/wodenwhel/images/e15776dd9ec89c68fe3865b7c2f41b1b35cf5238/windhawk/explorer-font-size-changer-Before.png) | ![修改后：自定义字体与字号](https://raw.githubusercontent.com/wodenwhel/images/e15776dd9ec89c68fe3865b7c2f41b1b35cf5238/windhawk/explorer-font-size-changer-After.png) |

## 设置项

| 设置项 | 默认值 | 说明 |
| --- | --- | --- |
| `fontFace` | `None` | 已安装的字体名，最长 31 个字符。填 `None` 或留空表示保持原字体名 |
| `fontSizePt` | `0` | `0` 保持原字号；否则按 6-72 设置，以 Windows 标准 9 磅界面文字为基准等比缩放，保留文字间的大小层级 |
| `verticalOffsetPx` | `0` | 上下平移文字，负数上移、正数下移；`0` 不偏移 |
| `horizontalOffsetPx` | `0` | 左右平移文字，负数左移、正数右移；`0` 不偏移 |
| `skipSymbolFonts` | 开 | 保持 Marlett、Segoe MDL2 Assets、Wingdings 等不变，避免图标变成方框 |

## 说明与限制

- 字体改动作用于 mod 加载后创建的字体。安装、更改设置或禁用后需重启 `explorer.exe`（或注销后重新登录），让缓存的界面字体重新创建。
- 字号设置以 Windows 标准 9 磅界面文字为基准等比缩放，标题、小字与正文保持原有层级，混合 DPI 的多显示器环境同样适用。
- Windows 11 部分界面使用 XAML / DirectWrite 而非 GDI，其字体保持原样。
- 两个偏移只平移 GDI 绘制的文字，不改变图标位图、行高或列宽；取值过大会把文字推出行或单元格。
- 需要 x86-64 Windhawk 1.7.3 或更新版本。

## 致谢

字体替换思路受 [Gabriela Cristei](https://github.com/cristeigabriela) 与 *Explorer Font Changer* 启发。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fontFace: "None"
  $name: Font face
  $name:zh-CN: 字体名称
  $description: None or empty keeps the original font face. Max 31 characters.
  $description:zh-CN: 填 None 或留空表示保持原字体名，最长 31 个字符。
- fontSizePt: 0
  $name: Font size (9 pt base)
  $name:zh-CN: 字号（以 9 磅为基准）
  $description: 0 keeps the original size, otherwise 6-72 scales all text proportionally from the Windows 9 pt UI baseline, preserving relative sizes.
  $description:zh-CN: 0 表示保持原字号；否则按 6-72 设置，以 Windows 标准 9 磅界面文字为基准等比缩放，保留文字间的大小层级。
- verticalOffsetPx: 0
  $name: Text vertical offset (px)
  $name:zh-CN: 文本垂直偏移（像素）
  $description: Shifts drawn text vertically. Negative moves up, positive moves down, 0 disables it. Range -16 to 16.
  $description:zh-CN: 上下平移文字，负数上移，正数下移，0 表示不偏移，范围 -16 到 16。
- horizontalOffsetPx: 0
  $name: Text horizontal offset (px)
  $name:zh-CN: 文本水平偏移（像素）
  $description: Shifts drawn text horizontally. Negative moves left, positive moves right, 0 disables it. Range -16 to 16.
  $description:zh-CN: 左右平移文字，负数左移，正数右移，0 表示不偏移，范围 -16 到 16。
- skipSymbolFonts: true
  $name: Ignore symbol and icon fonts
  $name:zh-CN: 忽略符号图标字体
  $description: Recommended. Keeps Marlett, Segoe MDL2 Assets, Wingdings, etc. unchanged.
  $description:zh-CN: 建议开启，避免 Marlett、Segoe MDL2 Assets、Wingdings 等图标字体被误改。
*/
// ==/WindhawkModSettings==

// ENUMLOGFONTEXDVW 受 _WIN32_WINNT 条件编译保护，先兜底再包含。
#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0500
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <windhawk_utils.h>

using namespace std::string_view_literals;

namespace {

// ── 常量 ────────────────────────────────────────────────────────────────────
// 范围在代码钳制：设置块只用基础字段，兼容 Windhawk 1.7.3。
constexpr int kMinPointSize = 6;
constexpr int kMaxPointSize = 72;
constexpr int kDefaultUiPointSize = 9;  // Windows 标准 UI 文字，缩放基准
constexpr int kMaxOffsetPx = 16;        // 水平、垂直共用
constexpr size_t kMaxFaceChars = LF_FACESIZE - 1;  // 31

// ── 文本工具 ────────────────────────────────────────────────────────────────
constexpr wchar_t ToLower(wchar_t c) {
    return (c >= L'A' && c <= L'Z') ? static_cast<wchar_t>(c + 32) : c;
}

bool EqualsIgnoreCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (ToLower(a[i]) != ToLower(b[i])) return false;
    }
    return true;
}

bool StartsWithIgnoreCase(std::wstring_view text, std::wstring_view prefix) {
    return text.size() >= prefix.size() &&
           EqualsIgnoreCase(text.substr(0, prefix.size()), prefix);
}

// ── 设置 ────────────────────────────────────────────────────────────────────
// 纯 POD 快照按值拷贝；读写由 SRWLOCK 保护。
struct Settings {
    WCHAR face[LF_FACESIZE] = {};
    int point_size = 0;         // 0 = 保持原字号
    int vertical_offset = 0;    // 负数上移
    int horizontal_offset = 0;  // 正数右移
    bool skip_symbol_fonts = true;
};

class SettingsStore {
public:
    Settings Get() const {
        AcquireSRWLockShared(&lock_);
        Settings copy = value_;
        ReleaseSRWLockShared(&lock_);
        return copy;
    }

    void Set(const Settings& value) {
        AcquireSRWLockExclusive(&lock_);
        value_ = value;
        ReleaseSRWLockExclusive(&lock_);
    }

private:
    mutable SRWLOCK lock_ = {};
    Settings value_;
};

SettingsStore g_settings;

// "None" 与留空都表示不换字体名（沿用原 mod 约定）。
bool IsNoneKeyword(std::wstring_view face) {
    return EqualsIgnoreCase(face, L"None"sv);
}

Settings LoadSettings() {
    Settings s;

    PCWSTR raw_face = Wh_GetStringSetting(L"fontFace");
    const std::wstring_view face(raw_face);
    if (face.size() > kMaxFaceChars) {
        Wh_Log(L"Font face is too long (max 31 characters)");
    } else if (!face.empty() && !IsNoneKeyword(face)) {
        std::memcpy(s.face, face.data(), face.size() * sizeof(WCHAR));
    }
    Wh_FreeStringSetting(raw_face);

    // < 6pt 视为保持原字号，避免误填小数字把字缩到不可见。
    const int pt = Wh_GetIntSetting(L"fontSizePt");
    s.point_size = pt >= kMinPointSize ? std::min(pt, kMaxPointSize) : 0;
    s.vertical_offset = std::clamp(Wh_GetIntSetting(L"verticalOffsetPx"),
                                   -kMaxOffsetPx, kMaxOffsetPx);
    s.horizontal_offset = std::clamp(Wh_GetIntSetting(L"horizontalOffsetPx"),
                                     -kMaxOffsetPx, kMaxOffsetPx);
    s.skip_symbol_fonts = Wh_GetIntSetting(L"skipSymbolFonts") != 0;
    return s;
}

void ReloadSettings() {
    g_settings.Set(LoadSettings());
}

// ── 字体改写 ────────────────────────────────────────────────────────────────
// lfFaceName 可能占满 32 个 WCHAR 且无 NUL，按长度取视图而非 wcslen。
std::wstring_view LogFontFace(const LOGFONTW& font) {
    size_t length = 0;
    while (length < LF_FACESIZE && font.lfFaceName[length] != L'\0') ++length;
    return {font.lfFaceName, length};
}

void SetFace(LOGFONTW& font, std::wstring_view face) {
    std::memset(font.lfFaceName, 0, sizeof(font.lfFaceName));
    const size_t count = std::min<size_t>(face.size(), kMaxFaceChars);
    std::memcpy(font.lfFaceName, face.data(), count * sizeof(WCHAR));
}

constexpr std::wstring_view kSymbolFonts[] = {
    L"Marlett"sv,
    L"Segoe MDL2 Assets"sv,
    L"Segoe Fluent Icons"sv,
    L"Wingdings"sv,
    L"Webdings"sv,
    L"Symbol"sv,
};

// 符号字体整类跳过：换掉会把图标画成方框。
bool IsSymbolFont(const LOGFONTW& font) {
    if (font.lfCharSet == SYMBOL_CHARSET) return true;
    const std::wstring_view face = LogFontFace(font);
    for (std::wstring_view prefix : kSymbolFonts) {
        if (StartsWithIgnoreCase(face, prefix)) return true;
    }
    return false;
}

// 调用方已按窗口 DPI 与文字角色算好 lfHeight / lfWidth；等比缩放即可，
// 无需自行换算 DPI，标题与小字的大小层级也不会被抹平。
// 偏置随符号取反，负数按绝对值四舍五入，保证 9 pt 时严格恒等。
LONG Scaled(LONG value, int points) {
    const LONGLONG product =
        static_cast<LONGLONG>(value) * static_cast<LONGLONG>(points);
    const LONGLONG base = kDefaultUiPointSize;
    const LONGLONG bias = product >= 0 ? base / 2 : -base / 2;
    return static_cast<LONG>((product + bias) / base);
}

// 调用方可能 GetObjectW 回读本 mod 已缩放的字体再重建（就地重命名框等），
// 连乘会累积放大。记录改写结果的指纹，命中即视为已缩放，跳过第二次。
constexpr size_t kScaledFontSlots = 64;
SRWLOCK g_scaled_font_lock = SRWLOCK_INIT;
uint64_t g_scaled_font_fingerprints[kScaledFontSlots] = {};
size_t g_scaled_font_cursor = 0;

uint64_t FontFingerprint(const LOGFONTW& font) {
    uint64_t hash = 0xcbf29ce484222325ULL;  // FNV-1a
    auto mix = [&hash](uint64_t value) {
        hash ^= value;
        hash *= 0x100000001b3ULL;
    };
    mix(static_cast<uint64_t>(static_cast<uint32_t>(font.lfHeight)));
    mix(static_cast<uint64_t>(static_cast<uint32_t>(font.lfWidth)));
    mix(static_cast<uint64_t>(static_cast<uint32_t>(font.lfWeight)));
    mix(font.lfCharSet);
    mix(font.lfItalic);
    mix(font.lfQuality);
    mix(font.lfPitchAndFamily);
    for (int i = 0; i < LF_FACESIZE && font.lfFaceName[i] != L'\0'; ++i) {
        mix(static_cast<uint64_t>(font.lfFaceName[i]));
    }
    return hash;
}

bool AlreadyScaled(const LOGFONTW& font) {
    const uint64_t fingerprint = FontFingerprint(font);
    AcquireSRWLockShared(&g_scaled_font_lock);
    bool found = false;
    for (uint64_t entry : g_scaled_font_fingerprints) {
        if (entry == fingerprint) {
            found = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_scaled_font_lock);
    return found;
}

void MarkScaled(const LOGFONTW& font) {
    AcquireSRWLockExclusive(&g_scaled_font_lock);
    g_scaled_font_fingerprints[g_scaled_font_cursor] = FontFingerprint(font);
    g_scaled_font_cursor = (g_scaled_font_cursor + 1) % kScaledFontSlots;
    ReleaseSRWLockExclusive(&g_scaled_font_lock);
}

// 字号设置改变后，旧标记对应的是旧倍率，必须清空重新基准。
void ResetScaledFontMarks() {
    AcquireSRWLockExclusive(&g_scaled_font_lock);
    std::memset(g_scaled_font_fingerprints, 0,
                sizeof(g_scaled_font_fingerprints));
    g_scaled_font_cursor = 0;
    ReleaseSRWLockExclusive(&g_scaled_font_lock);
}

// 只改副本。apply_size=false 供 GetThemeFont 使用：调用方随后仍会走
// CreateFontIndirectW，尺寸在那里缩放，避免重复。
void ApplyFontSettings(LOGFONTW& font, const Settings& s, bool apply_size) {
    if (s.skip_symbol_fonts && IsSymbolFont(font)) return;
    if (s.face[0] != L'\0') {
        SetFace(font, s.face);
        font.lfCharSet = DEFAULT_CHARSET;  // 原 charset 可能绑定旧字体
    }
    // 9 pt 是恒等基准，不介入也不留标记，否则会把原始字体误判为已缩放。
    if (apply_size && s.point_size > 0 &&
        s.point_size != kDefaultUiPointSize && !AlreadyScaled(font)) {
        font.lfHeight = Scaled(font.lfHeight, s.point_size);
        font.lfWidth = Scaled(font.lfWidth, s.point_size);
        MarkScaled(font);  // 记录改写后的结果，供回读派生时比对
    }
}

// 缩放不幂等：gdi32 内部可能由 CreateFontIndirectW 转入 CreateFontIndirectExW。
// 线程标记包住原函数调用，保证每个字体只缩放一次。
thread_local bool t_size_applied = false;

struct SizeOnceGuard {
    const bool previous = t_size_applied;
    SizeOnceGuard() { t_size_applied = true; }
    ~SizeOnceGuard() { t_size_applied = previous; }
};

// ── 原函数指针 ──────────────────────────────────────────────────────────────
using CreateFontIndirectW_t = decltype(&CreateFontIndirectW);
using CreateFontW_t = decltype(&CreateFontW);
using CreateFontIndirectExW_t = decltype(&CreateFontIndirectExW);
using GetThemeFont_t = HRESULT(WINAPI*)(void*, HDC, int, int, int, LOGFONTW*);
using DrawTextW_t = decltype(&DrawTextW);
using DrawTextExW_t = decltype(&DrawTextExW);
using ExtTextOutW_t = decltype(&ExtTextOutW);
using TextOutW_t = decltype(&TextOutW);
using TabbedTextOutW_t = decltype(&TabbedTextOutW);

CreateFontIndirectW_t g_original_create_font_indirect_w = nullptr;
CreateFontW_t g_original_create_font_w = nullptr;
CreateFontIndirectExW_t g_original_create_font_indirect_ex_w = nullptr;
GetThemeFont_t g_original_get_theme_font = nullptr;
DrawTextW_t g_original_draw_text_w = nullptr;
DrawTextExW_t g_original_draw_text_ex_w = nullptr;
ExtTextOutW_t g_original_ext_text_out_w = nullptr;
TextOutW_t g_original_text_out_w = nullptr;
TabbedTextOutW_t g_original_tabbed_text_out_w = nullptr;

// ── 字体创建 hook ───────────────────────────────────────────────────────────
// 创建阶段改写，让 Explorer 用新度量自行测量、排行高、截断长串。
HFONT WINAPI CreateFontIndirectW_Hook(const LOGFONTW* log_font) {
    if (!log_font || t_size_applied) {
        return g_original_create_font_indirect_w(log_font);
    }
    LOGFONTW copy = *log_font;
    ApplyFontSettings(copy, g_settings.Get(), true);
    SizeOnceGuard guard;
    return g_original_create_font_indirect_w(&copy);
}

// 14 个参数组装成同一份 LOGFONTW 后复用 Indirect 的原函数，不会二次改写。
HFONT WINAPI CreateFontW_Hook(INT height, INT width, INT escapement,
                              INT orientation, INT weight, DWORD italic,
                              DWORD underline, DWORD strikeout, DWORD char_set,
                              DWORD out_precision, DWORD clip_precision,
                              DWORD quality, DWORD pitch_and_family,
                              LPCWSTR face_name) {
    if (t_size_applied) {
        return g_original_create_font_w(
            height, width, escapement, orientation, weight, italic, underline,
            strikeout, char_set, out_precision, clip_precision, quality,
            pitch_and_family, face_name);
    }
    LOGFONTW font = {};
    font.lfHeight = height;
    font.lfWidth = width;
    font.lfEscapement = escapement;
    font.lfOrientation = orientation;
    font.lfWeight = weight;
    font.lfItalic = static_cast<BYTE>(italic);
    font.lfUnderline = static_cast<BYTE>(underline);
    font.lfStrikeOut = static_cast<BYTE>(strikeout);
    font.lfCharSet = static_cast<BYTE>(char_set);
    font.lfOutPrecision = static_cast<BYTE>(out_precision);
    font.lfClipPrecision = static_cast<BYTE>(clip_precision);
    font.lfQuality = static_cast<BYTE>(quality);
    font.lfPitchAndFamily = static_cast<BYTE>(pitch_and_family);
    if (face_name) SetFace(font, std::wstring_view(face_name));
    ApplyFontSettings(font, g_settings.Get(), true);
    SizeOnceGuard guard;
    return g_original_create_font_indirect_w(&font);
}

// DirectUI / dui70 走这个入口。GDI 只读 dvNumAxes 个值，调用方缓冲区可能
// 刚好结束在那里，故按实际长度复制，避免读越界。
HFONT WINAPI CreateFontIndirectExW_Hook(const ENUMLOGFONTEXDVW* font) {
    if (!font || t_size_applied) {
        return g_original_create_font_indirect_ex_w(font);
    }
    const DWORD axes = font->elfDesignVector.dvNumAxes;
    if (axes > MM_MAX_NUMAXES) {
        return g_original_create_font_indirect_ex_w(font);
    }
    const size_t used = offsetof(ENUMLOGFONTEXDVW, elfDesignVector) +
                        offsetof(DESIGNVECTOR, dvValues) +
                        static_cast<size_t>(axes) * sizeof(LONG);
    ENUMLOGFONTEXDVW copy = {};
    std::memcpy(&copy, font, used);
    ApplyFontSettings(copy.elfEnumLogfontEx.elfLogFont, g_settings.Get(), true);
    SizeOnceGuard guard;
    return g_original_create_font_indirect_ex_w(&copy);
}

// 主题化控件先取 LOGFONTW 再建字体；只替换字体名，
// 尺寸留给随后的 CreateFontIndirectW，避免缩放两次。
HRESULT WINAPI GetThemeFont_Hook(void* theme, HDC hdc, int part, int state,
                                 int prop, LOGFONTW* font) {
    const HRESULT hr =
        g_original_get_theme_font(theme, hdc, part, state, prop, font);
    if (SUCCEEDED(hr) && font) {
        ApplyFontSettings(*font, g_settings.Get(), false);
    }
    return hr;
}

// ── 文本偏移 hook ───────────────────────────────────────────────────────────
// 只平移绘制位置，不换字体、不改宽高；测量调用 DT_CALCRECT 原样透传。
// DrawText / TabbedTextOut 内部会再调 ExtTextOutW；外层已应用偏移时内层
// 必须透传，否则偏移叠加两次。
thread_local int t_offset_depth = 0;

struct OffsetScope {
    OffsetScope() { ++t_offset_depth; }
    ~OffsetScope() { --t_offset_depth; }
};

struct TextOffset {
    int x = 0;
    int y = 0;
    bool IsZero() const { return x == 0 && y == 0; }
};

TextOffset CurrentOffset() {
    const Settings s = g_settings.Get();
    return {s.horizontal_offset, s.vertical_offset};
}

// 四边同步平移，宽高不变。
RECT Shifted(RECT rect, TextOffset offset) {
    rect.left += offset.x;
    rect.right += offset.x;
    rect.top += offset.y;
    rect.bottom += offset.y;
    return rect;
}

INT WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR text, INT len, LPRECT rect,
                          UINT format) {
    const TextOffset offset = CurrentOffset();
    if (offset.IsZero() || t_offset_depth > 0 || !rect ||
        (format & DT_CALCRECT)) {
        return g_original_draw_text_w(hdc, text, len, rect, format);
    }
    OffsetScope scope;
    RECT adjusted = Shifted(*rect, offset);
    return g_original_draw_text_w(hdc, text, len, &adjusted, format);
}

INT WINAPI DrawTextExW_Hook(HDC hdc, LPWSTR text, INT len, LPRECT rect,
                            UINT format, LPDRAWTEXTPARAMS params) {
    const TextOffset offset = CurrentOffset();
    if (offset.IsZero() || t_offset_depth > 0 || !rect ||
        (format & DT_CALCRECT)) {
        return g_original_draw_text_ex_w(hdc, text, len, rect, format, params);
    }
    OffsetScope scope;
    RECT adjusted = Shifted(*rect, offset);
    return g_original_draw_text_ex_w(hdc, text, len, &adjusted, format, params);
}

BOOL WINAPI ExtTextOutW_Hook(HDC hdc, INT x, INT y, UINT options,
                             const RECT* rect, LPCWSTR text, UINT cch,
                             const INT* dx) {
    const TextOffset offset = CurrentOffset();
    // 零偏移、无文字（纯擦背景）、外层已偏移时，原样透传。
    if (offset.IsZero() || cch == 0 || t_offset_depth > 0) {
        return g_original_ext_text_out_w(hdc, x, y, options, rect, text, cch,
                                         dx);
    }
    OffsetScope scope;

    // ETO_OPAQUE 时 rect 同时是背景填充区：先按原矩形填充，再单独绘制
    // 平移后的文字，避免背景被一并挪走。
    if ((options & ETO_OPAQUE) && rect) {
        g_original_ext_text_out_w(hdc, x, y, ETO_OPAQUE, rect, L"", 0,
                                  nullptr);
        RECT clip = Shifted(*rect, offset);
        return g_original_ext_text_out_w(
            hdc, x + offset.x, y + offset.y, options & ~ETO_OPAQUE, &clip,
            text, cch, dx);
    }

    RECT storage = {};
    const RECT* clip = rect;
    if (rect) {
        storage = Shifted(*rect, offset);  // 裁剪矩形同步平移，否则文字被切
        clip = &storage;
    }
    return g_original_ext_text_out_w(hdc, x + offset.x, y + offset.y, options,
                                     clip, text, cch, dx);
}

BOOL WINAPI TextOutW_Hook(HDC hdc, INT x, INT y, LPCWSTR text, INT cch) {
    const TextOffset offset = CurrentOffset();
    if (offset.IsZero() || t_offset_depth > 0) {
        return g_original_text_out_w(hdc, x, y, text, cch);
    }
    OffsetScope scope;
    return g_original_text_out_w(hdc, x + offset.x, y + offset.y, text, cch);
}

LONG WINAPI TabbedTextOutW_Hook(HDC hdc, INT x, INT y, LPCWSTR text,
                                INT count, INT tab_count, const INT* tabs,
                                INT tab_origin) {
    const TextOffset offset = CurrentOffset();
    if (offset.IsZero() || t_offset_depth > 0) {
        return g_original_tabbed_text_out_w(hdc, x, y, text, count, tab_count,
                                            tabs, tab_origin);
    }
    OffsetScope scope;
    // 制表位以 tab_origin 为基准，文字右移时原点须同步，否则制表位错位。
    return g_original_tabbed_text_out_w(
        hdc, x + offset.x, y + offset.y, text, count, tab_count, tabs,
        tab_origin + offset.x);
}

// ── Hook 注册 ───────────────────────────────────────────────────────────────
// 取导出名而非直接引用符号：兼容 IAT 转发与不同编译器 ABI。
// 仅核心 hook 失败会拒绝加载 —— 返回 FALSE 会让 Windhawk 回滚全部 hook。
struct Hook {
    HMODULE module;
    LPCSTR export_name;
    PCWSTR label;
    void* detour;
    void** original;

    template <typename Fn>
    Hook(HMODULE mod, LPCSTR name, PCWSTR lbl, Fn fn, Fn* orig)
        : module(mod),
          export_name(name),
          label(lbl),
          detour(reinterpret_cast<void*>(fn)),
          original(reinterpret_cast<void**>(orig)) {}

    bool Install() const {
        void* target = module
                           ? reinterpret_cast<void*>(
                                 GetProcAddress(module, export_name))
                           : nullptr;
        if (!target) {
            Wh_Log(L"Hook unavailable (export not found): %s", label);
            return false;
        }
        Wh_SetFunctionHook(target, detour, original);
        if (!*original) {
            Wh_Log(L"Hook unavailable (not installed): %s", label);
            return false;
        }
        return true;
    }
};

bool InstallHooks() {
    // GetModuleHandle 不加引用计数；这两个 DLL 在 explorer.exe 中必定已加载。
    const HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    const HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!gdi32 || !user32) {
        Wh_Log(L"Core system modules are not loaded");
        return false;
    }
    Wh_Log(L"Initializing font hooks");

    // 唯一必需项：字体改写主入口。其余失败只记日志，不拖垮整个 mod。
    const Hook core{gdi32, "CreateFontIndirectW", L"CreateFontIndirectW",
                    CreateFontIndirectW_Hook,
                    &g_original_create_font_indirect_w};
    if (!core.Install()) {
        Wh_Log(L"Core hook failed; mod will not load");
        return false;
    }

    // uxtheme 注入早期可能未映射：按需加载一次，失败只跳过主题字体 hook。
    HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
    if (!uxtheme) {
        uxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr,
                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!uxtheme) {
            Wh_Log(L"uxtheme.dll unavailable; theme font hook skipped");
        }
    }

    // CreateFontW 复用核心的 original，须排在其后；其余表项互不依赖。
    // ExtTextOutW / TextOutW 是 gdi32 导出，不是 user32。
    // 新增 hook 只需在此加一行。
    const Hook optional[] = {
        {gdi32, "CreateFontW", L"CreateFontW", CreateFontW_Hook,
         &g_original_create_font_w},
        {gdi32, "CreateFontIndirectExW", L"CreateFontIndirectExW",
         CreateFontIndirectExW_Hook, &g_original_create_font_indirect_ex_w},
        {uxtheme, "GetThemeFont", L"GetThemeFont", GetThemeFont_Hook,
         &g_original_get_theme_font},
        {user32, "DrawTextW", L"DrawTextW", DrawTextW_Hook,
         &g_original_draw_text_w},
        {user32, "DrawTextExW", L"DrawTextExW", DrawTextExW_Hook,
         &g_original_draw_text_ex_w},
        {gdi32, "ExtTextOutW", L"ExtTextOutW", ExtTextOutW_Hook,
         &g_original_ext_text_out_w},
        {gdi32, "TextOutW", L"TextOutW", TextOutW_Hook,
         &g_original_text_out_w},
        {user32, "TabbedTextOutW", L"TabbedTextOutW", TabbedTextOutW_Hook,
         &g_original_tabbed_text_out_w},
    };

    int missing = 0;
    for (const Hook& hook : optional) {
        if (!hook.Install()) ++missing;
    }
    Wh_Log(L"Font hooks ready (%d optional hooks unavailable)", missing);
    return true;
}

}  // namespace

BOOL Wh_ModInit() {
    ReloadSettings();
    ResetScaledFontMarks();
    return InstallHooks() ? TRUE : FALSE;
}

void Wh_ModSettingsChanged() {
    ReloadSettings();
    ResetScaledFontMarks();
}

void Wh_ModUninit() {
    // 字体对象由系统持有，设置快照随进程结束销毁，无需显式清理。
    Wh_Log(L"Unloaded");
}
