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
`uxtheme!GetThemeFont`. It edits a private copy of `lfFaceName` and `lfHeight`
before the font is built. Explorer therefore receives a font already sized as
configured, and text measurement, row height and truncation are derived from
its metrics.

`CreateFontIndirectExW` and `GetThemeFont` extend this to DirectUI and themed
controls, including the left navigation pane.

Symbol and icon fonts are excluded, preventing glyph substitution that would
render icons as empty squares. The vertical and horizontal offsets adjust only
the drawing origin, keeping text aligned with fixed-size icon bitmaps; layout
measurement is unaffected.

## Settings

| Setting | Default | Description |
| --- | --- | --- |
| `fontFace` | `None` | An installed font face name, up to 31 characters. `None` or empty keeps the original face |
| `fontSizePt` | `0` | `0` keeps the original size; otherwise 6-72 pt. Converted to pixels using the window DPI when available |
| `verticalOffsetPx` | `-1` | Shifts drawn text vertically. Negative moves up, positive moves down; `0` disables the shift |
| `horizontalOffsetPx` | `0` | Shifts drawn text horizontally. Negative moves left, positive moves right; `0` disables the shift |
| `skipSymbolFonts` | on | Leaves Marlett, Segoe MDL2 Assets, Wingdings, etc. unchanged so icons do not turn into boxes |

## Notes and limitations

- Font changes apply to fonts created after the mod is loaded. Restart
  `explorer.exe` (or sign out and back in) after installing or changing
  settings so cached UI fonts are recreated.
- The point size is converted with the DPI of the painting window, then the
  thread DPI context, then the system DPI. Fonts created outside a paint
  cycle on a mixed-DPI multi-monitor setup may use the system DPI.
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

每个 GDI 字体都由 `LOGFONTW` 创建。本 mod 在 Windows 建字体的四个入口挂钩：
`CreateFontIndirectW`、`CreateFontW`、`CreateFontIndirectExW` 与 `uxtheme!GetThemeFont`，
并在创建前修改私有副本中的 `lfFaceName` 与 `lfHeight`。
资源管理器由此获得已按设定字号创建的字体，文字测量、行高计算与长串截断均基于该字体的度量。

`CreateFontIndirectExW` 与 `GetThemeFont` 把覆盖范围扩展到 DirectUI 与主题化控件，包括左侧导航窗格。

符号与图标字体均予排除，避免图标因字体替换而显示为方框。
垂直与水平偏移仅调整文字的绘制原点，使其与固定尺寸的图标位图保持对齐，布局测量不受影响。

## 设置项

| 设置项 | 默认值 | 说明 |
| --- | --- | --- |
| `fontFace` | `None` | 已安装的字体名，最长 31 个字符。填 `None` 或留空表示保持原字体名 |
| `fontSizePt` | `0` | `0` 保持原字号；否则按 6-72 磅设置，优先按窗口 DPI 换算为像素 |
| `verticalOffsetPx` | `-1` | 上下平移文字，负数上移、正数下移；`0` 不偏移 |
| `horizontalOffsetPx` | `0` | 左右平移文字，负数左移、正数右移；`0` 不偏移 |
| `skipSymbolFonts` | 开 | 保持 Marlett、Segoe MDL2 Assets、Wingdings 等不变，避免图标变成方框 |

## 说明与限制

- 字体改动作用于 mod 加载后创建的字体。安装或更改设置后需重启 `explorer.exe`（或注销后重新登录），让缓存的界面字体重新创建。
- 字号换算依次采用绘制窗口 DPI、线程 DPI 上下文、系统 DPI。混合缩放的多显示器环境中，绘制周期之外创建的字体可能使用系统 DPI。
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
  $name: Font size (pt)
  $name:zh-CN: 字号（磅）
  $description: 0 keeps the original size, otherwise 6-72 pt. Converted with the window DPI where available.
  $description:zh-CN: 0 表示保持原字号；否则按 6-72 磅设置，优先按窗口 DPI 换算。
- verticalOffsetPx: -1
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
#include <cstring>
#include <string_view>
#include <windhawk_utils.h>

using namespace std::string_view_literals;

namespace {

// ── 常量 ────────────────────────────────────────────────────────────────────
// 范围在代码钳制：设置块只用基础字段，兼容 Windhawk 1.7.3。
constexpr int kMinPointSize = 6;
constexpr int kMaxPointSize = 72;
constexpr int kMaxOffsetPx = 16;      // 水平、垂直共用
constexpr UINT kFallbackDpi = 96;
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
    int vertical_offset = -1;   // 负数上移
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

// ── 绘制窗口上下文 ──────────────────────────────────────────────────────────
// 字体创建回调没有 HDC；借 WM_PAINT 的窗口句柄定位 DPI。嵌套绘制以最外层为准。
class PaintContext {
public:
    void Enter(HWND hwnd) {
        if (depth_++ == 0) window_ = hwnd;
    }

    void Leave() {
        if (depth_ > 0 && --depth_ == 0) window_ = nullptr;
    }

    HWND window() const { return window_; }

private:
    HWND window_ = nullptr;
    int depth_ = 0;
};

thread_local PaintContext t_paint;

// ── DPI ─────────────────────────────────────────────────────────────────────
// 动态解析避开 SDK 版本差异；优先级：绘制窗口 > 线程上下文 > 系统 > 96。
class DpiProvider {
public:
    void Resolve(HMODULE user32) {
        get_window_ = As<GetDpiForWindow>(user32, "GetDpiForWindow");
        get_system_ = As<GetDpiForSystem>(user32, "GetDpiForSystem");
        get_context_ = As<GetThreadContext>(user32, "GetThreadDpiAwarenessContext");
        dpi_of_context_ = As<DpiOfContext>(user32, "GetDpiFromDpiAwarenessContext");
    }

    UINT Y() const {
        const UINT window_dpi = FromWindow();
        if (window_dpi > 0) return window_dpi;
        const UINT context_dpi = FromContext();
        if (context_dpi > 0) return context_dpi;
        const UINT system_dpi = FromSystem();
        if (system_dpi > 0) return system_dpi;
        return kFallbackDpi;
    }

private:
    using GetDpiForWindow = UINT(WINAPI*)(HWND);
    using GetDpiForSystem = UINT(WINAPI*)();
    using GetThreadContext = void*(WINAPI*)();
    using DpiOfContext = UINT(WINAPI*)(void*);

    template <typename Fn>
    static Fn As(HMODULE module, LPCSTR name) {
        return reinterpret_cast<Fn>(GetProcAddress(module, name));
    }

    UINT FromWindow() const {
        return (get_window_ && t_paint.window())
                   ? get_window_(t_paint.window())
                   : 0;
    }

    UINT FromContext() const {
        if (!get_context_ || !dpi_of_context_) return 0;
        void* context = get_context_();
        return context ? dpi_of_context_(context) : 0;
    }

    UINT FromSystem() const {
        return get_system_ ? get_system_() : 0;
    }

    GetDpiForWindow get_window_ = nullptr;
    GetDpiForSystem get_system_ = nullptr;
    GetThreadContext get_context_ = nullptr;
    DpiOfContext dpi_of_context_ = nullptr;
};

DpiProvider g_dpi;

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

// 负 lfHeight 表示字符高度，与系统 UI 字体惯例一致。
int PointSizeToHeight(int points, UINT dpi_y) {
    const int pixels = MulDiv(points, static_cast<int>(dpi_y), 72);
    return pixels > 0 ? -pixels : -1;
}

// 只改副本，不碰调用方的 const 结构；覆盖式赋值天然幂等，
// 同一 LOGFONTW 经过多个 hook 点结果一致。
void ApplyFontSettings(LOGFONTW& font, const Settings& s) {
    if (s.skip_symbol_fonts && IsSymbolFont(font)) return;
    if (s.face[0] != L'\0') {
        SetFace(font, s.face);
        font.lfCharSet = DEFAULT_CHARSET;  // 原 charset 可能绑定旧字体
    }
    if (s.point_size > 0) {
        font.lfHeight = PointSizeToHeight(s.point_size, g_dpi.Y());
        font.lfWidth = 0;  // 让 GDI 按高度匹配宽度，避免字形拉伸
    }
}

// ── 原函数指针 ──────────────────────────────────────────────────────────────
using CreateFontIndirectW_t = decltype(&CreateFontIndirectW);
using CreateFontW_t = decltype(&CreateFontW);
using CreateFontIndirectExW_t = decltype(&CreateFontIndirectExW);
using GetThemeFont_t = HRESULT(WINAPI*)(void*, HDC, int, int, int, LOGFONTW*);
using BeginPaint_t = decltype(&BeginPaint);
using EndPaint_t = decltype(&EndPaint);
using DrawTextW_t = decltype(&DrawTextW);
using DrawTextExW_t = decltype(&DrawTextExW);
using ExtTextOutW_t = decltype(&ExtTextOutW);
using TextOutW_t = decltype(&TextOutW);
using TabbedTextOutW_t = decltype(&TabbedTextOutW);

CreateFontIndirectW_t g_original_create_font_indirect_w = nullptr;
CreateFontW_t g_original_create_font_w = nullptr;
CreateFontIndirectExW_t g_original_create_font_indirect_ex_w = nullptr;
GetThemeFont_t g_original_get_theme_font = nullptr;
BeginPaint_t g_original_begin_paint = nullptr;
EndPaint_t g_original_end_paint = nullptr;
DrawTextW_t g_original_draw_text_w = nullptr;
DrawTextExW_t g_original_draw_text_ex_w = nullptr;
ExtTextOutW_t g_original_ext_text_out_w = nullptr;
TextOutW_t g_original_text_out_w = nullptr;
TabbedTextOutW_t g_original_tabbed_text_out_w = nullptr;

// ── 字体创建与绘制上下文 hook ───────────────────────────────────────────────
// 在创建阶段改写，让 Explorer 用新度量自行测量、排行高、截断长串。
HFONT WINAPI CreateFontIndirectW_Hook(const LOGFONTW* log_font) {
    if (!log_font) return g_original_create_font_indirect_w(log_font);
    LOGFONTW copy = *log_font;
    ApplyFontSettings(copy, g_settings.Get());
    return g_original_create_font_indirect_w(&copy);
}

// 14 个参数组装成同一份 LOGFONTW 后复用 Indirect 的原函数，不会二次改写。
HFONT WINAPI CreateFontW_Hook(INT height, INT width, INT escapement,
                              INT orientation, INT weight, DWORD italic,
                              DWORD underline, DWORD strikeout, DWORD char_set,
                              DWORD out_precision, DWORD clip_precision,
                              DWORD quality, DWORD pitch_and_family,
                              LPCWSTR face_name) {
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
    ApplyFontSettings(font, g_settings.Get());
    return g_original_create_font_indirect_w(&font);
}

// DirectUI / dui70 走这个入口；只改 elfLogFont，保留 design vector。
HFONT WINAPI CreateFontIndirectExW_Hook(const ENUMLOGFONTEXDVW* font) {
    if (!font) return g_original_create_font_indirect_ex_w(font);
    ENUMLOGFONTEXDVW copy = *font;
    ApplyFontSettings(copy.elfEnumLogfontEx.elfLogFont, g_settings.Get());
    return g_original_create_font_indirect_ex_w(&copy);
}

// 主题化控件先取 LOGFONTW 再建字体，在返回值上改写。
HRESULT WINAPI GetThemeFont_Hook(void* theme, HDC hdc, int part, int state,
                                 int prop, LOGFONTW* font) {
    const HRESULT hr =
        g_original_get_theme_font(theme, hdc, part, state, prop, font);
    if (SUCCEEDED(hr) && font) ApplyFontSettings(*font, g_settings.Get());
    return hr;
}

HDC WINAPI BeginPaint_Hook(HWND hwnd, LPPAINTSTRUCT paint) {
    HDC hdc = g_original_begin_paint(hwnd, paint);
    if (hdc && hwnd) t_paint.Enter(hwnd);
    return hdc;
}

BOOL WINAPI EndPaint_Hook(HWND hwnd, const PAINTSTRUCT* paint) {
    const BOOL result = g_original_end_paint(hwnd, paint);
    t_paint.Leave();
    return result;
}

// ── 文本偏移 hook ───────────────────────────────────────────────────────────
// 只平移绘制位置，不换字体、不改宽高；测量调用 DT_CALCRECT 原样透传。
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

// 测量、空矩形、零偏移一律透传；否则返回平移后的矩形供绘制使用。
RECT* PrepareDrawRect(RECT* src, UINT format, TextOffset offset,
                      RECT* storage) {
    if (offset.IsZero() || !src || (format & DT_CALCRECT)) return src;
    *storage = Shifted(*src, offset);
    return storage;
}

INT WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR text, INT len, LPRECT rect,
                          UINT format) {
    RECT storage;
    return g_original_draw_text_w(
        hdc, text, len,
        PrepareDrawRect(rect, format, CurrentOffset(), &storage), format);
}

INT WINAPI DrawTextExW_Hook(HDC hdc, LPWSTR text, INT len, LPRECT rect,
                            UINT format, LPDRAWTEXTPARAMS params) {
    RECT storage;
    return g_original_draw_text_ex_w(
        hdc, text, len,
        PrepareDrawRect(rect, format, CurrentOffset(), &storage), format, params);
}

BOOL WINAPI ExtTextOutW_Hook(HDC hdc, INT x, INT y, UINT options,
                             const RECT* rect, LPCWSTR text, UINT cch,
                             const INT* dx) {
    const TextOffset offset = CurrentOffset();
    RECT storage;
    const RECT* clip = rect;
    if (!offset.IsZero() && rect) {
        storage = Shifted(*rect, offset);  // 裁剪/背景同步平移，否则文字被切
        clip = &storage;
    }
    return g_original_ext_text_out_w(hdc, x + offset.x, y + offset.y, options,
                                     clip, text, cch, dx);
}

BOOL WINAPI TextOutW_Hook(HDC hdc, INT x, INT y, LPCWSTR text, INT cch) {
    const TextOffset offset = CurrentOffset();
    return g_original_text_out_w(hdc, x + offset.x, y + offset.y, text, cch);
}

LONG WINAPI TabbedTextOutW_Hook(HDC hdc, INT x, INT y, LPCWSTR text,
                                INT count, INT tab_count, const INT* tabs,
                                INT tab_origin) {
    const TextOffset offset = CurrentOffset();
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
    g_dpi.Resolve(user32);
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
    // 新增 hook 只需在此加一行。
    const Hook optional[] = {
        {gdi32, "CreateFontW", L"CreateFontW", CreateFontW_Hook,
         &g_original_create_font_w},
        {gdi32, "CreateFontIndirectExW", L"CreateFontIndirectExW",
         CreateFontIndirectExW_Hook, &g_original_create_font_indirect_ex_w},
        {uxtheme, "GetThemeFont", L"GetThemeFont", GetThemeFont_Hook,
         &g_original_get_theme_font},
        {user32, "BeginPaint", L"BeginPaint", BeginPaint_Hook,
         &g_original_begin_paint},
        {user32, "EndPaint", L"EndPaint", EndPaint_Hook,
         &g_original_end_paint},
        {user32, "DrawTextW", L"DrawTextW", DrawTextW_Hook,
         &g_original_draw_text_w},
        {user32, "DrawTextExW", L"DrawTextExW", DrawTextExW_Hook,
         &g_original_draw_text_ex_w},
        {user32, "ExtTextOutW", L"ExtTextOutW", ExtTextOutW_Hook,
         &g_original_ext_text_out_w},
        {user32, "TextOutW", L"TextOutW", TextOutW_Hook,
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
    return InstallHooks() ? TRUE : FALSE;
}

void Wh_ModSettingsChanged() {
    ReloadSettings();
}

void Wh_ModUninit() {
    // 字体对象由系统持有，设置快照随进程结束销毁，无需显式清理。
    Wh_Log(L"Unloaded");
}
