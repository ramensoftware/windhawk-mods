// ==WindhawkMod==
// @id              explorer-font-size-changer
// @name            Explorer Font & Size Changer
// @name:zh-CN      资源管理器字体与字号修改
// @description     Change the GDI font face and size in Windows 11 File Explorer.
// @description:zh-CN 修改 Windows 11 资源管理器中 GDI 文字的字体与字号，自动处理放大后的间距、对齐与列宽内的省略显示。
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

修改 Windows 11 资源管理器中由 GDI 绘制的文字：更换字体、调整字号，并自动处理字号放大后带来的一系列排版问题。

字号变大后，mod 会自动完成三件事，通常无需额外调整：

- 在文字与前方图标之间补一点很小的留白，避免挤在一起；
- 上提 1 像素并保持文字的垂直居中点，让文字与固定尺寸的图标对齐；
- 列头等短标签会按放大幅度与文字构成自动让出显示空间，长中文列名也能完整显示。

## 设置项

| 设置 | 默认值 | 说明 |
| --- | --- | --- |
| 字体名称 `fontFace` | `None` | 填已安装字体的名称，最长 31 个字符。`None` 或留空表示保持系统原字体 |
| 字号（磅）`fontSizePt` | `0` | `0` 保持原字号；否则按 6–72 pt 设置，并按当前显示器的 DPI 换算，高分屏下不会偏小 |
| 额外垂直偏移 `verticalOffsetPx` | `0` | 在自动对齐基础上的额外微调，负数上提。一般保持 `0` 即可 |
| 自动图标文字间距 `autoIconGap` | 开 | 按字号放大程度自动加 0–6 像素留白，字号回到原值时自动归零 |
| 额外图标文字间距 `iconGapPx` | `0` | 在自动值上微调，范围 −8 到 16，负值收回、正值加宽 |
| 忽略符号图标字体 `skipSymbolFonts` | 开 | 保持 Marlett、Segoe MDL2 Assets、Wingdings 等图标字体不变，避免图标变成方框 |
| 忽略对话框文本字体 `skipDialogText` | 开 | 忽略文件夹选项、菜单等窗口的文字字体改动，避免固定宽度的控件把文字截断。关闭则对进程内所有 GDI 文字生效 |

设置保存后立即作用于新绘制的文字；若部分界面没有变化，重启 `explorer.exe`。

## 作用范围

本 mod 只作用于 `explorer.exe` 中走 GDI 的文字绘制（`DrawTextW`、`DrawTextExW`、`ExtTextOutW`、`TextOutW`、`TabbedTextOutW`）。Windows 11 里由 XAML / DirectWrite 渲染的新界面不受影响；图标由 ImageList / XAML 按固定像素尺寸绘制，也不会随字号变大。若需要图标尺寸、列宽与行高跟着一起调整，配合 *Windows 11 File Explorer Styler* 使用。

文件夹选项、文件属性等系统对话框的控件尺寸由对话框模板按原字体算好后固定，绘制层无法改动，放大文字会被控件边界裁掉。mod 默认只放大资源管理器主窗口（`CabinetWClass` / `ExploreWClass`）内的文字，其余窗口一律保持原样；判定跟随绘制窗口上下文，双缓冲绘制到内存 DC 时同样有效。若希望连其他窗口一起放大，可关闭「忽略对话框文本字体」，但需接受文字截断。

适用于 x86-64 架构的 Windhawk（1.7.3 及以上）。设置面板只使用基础字段，数值范围由代码钳制，不依赖较新版本的设置注解。

## 致谢

字体替换思路来自 [Gabriela Cristei](https://github.com/cristeigabriela) 的 *Explorer Font Changer*。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fontFace: "None"
  $name: Font face
  $name:zh-CN: 字体名称
  $description: None or empty keeps the original font face. Max 31 characters.
  $description:zh-CN: 填 None 或留空表示不替换字体名，最长 31 个字符。
- fontSizePt: 0
  $name: Font size (pt)
  $name:zh-CN: 字号（磅）
  $description: 0 keeps the original size, otherwise 6-72 pt. Converted with the HDC DPI.
  $description:zh-CN: 0 表示保持原字号，否则按 6-72 磅设置，会按当前 HDC 的 DPI 换算。
- verticalOffsetPx: 0
  $name: Extra vertical offset (px)
  $name:zh-CN: 额外垂直偏移（像素）
  $description: Extra fine tune on top of the built-in 1 px lift. Default 0 is recommended.
  $description:zh-CN: 在内置 1 像素上提基础上的额外微调，建议保持默认 0。
- autoIconGap: true
  $name: Auto icon-text gap
  $name:zh-CN: 自动图标文字间距
  $description: Add a tiny leading gap that grows with the font size. Keeps icons and text from crowding.
  $description:zh-CN: 按字号放大程度自动在文字前加很小的间隙，避免图标与文字挤在一起。
- iconGapPx: 0
  $name: Extra icon-text gap (px)
  $name:zh-CN: 额外图标文字间距（像素）
  $description: Fine tune the auto gap. Negative pulls text back, positive adds more space. Range is -8 to 16.
  $description:zh-CN: 在自动值基础上微调，负值收回、正值加宽，范围 -8 到 16。
- skipSymbolFonts: true
  $name: Ignore symbol and icon fonts
  $name:zh-CN: 忽略符号图标字体
  $description: Recommended. Keeps Marlett, Segoe MDL2 Assets, Wingdings, etc. unchanged.
  $description:zh-CN: 建议开启，避免 Marlett、Segoe MDL2 Assets、Wingdings 等图标字体被误改。
- skipDialogText: true
  $name: Ignore dialog text font
  $name:zh-CN: 忽略对话框文本字体
  $description: Apply the font only to File Explorer main windows. Other windows keep their original text to avoid clipping; turn off to enlarge all GDI text.
  $description:zh-CN: 忽略对话框等窗口的文字字体改动，避免固定宽度的控件把文字截断；关闭则对进程内所有 GDI 文字生效。
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <algorithm>
#include <cstring>
#include <string_view>
#include <windhawk_utils.h>

using namespace std::string_view_literals;

namespace {

// ── 常量 ────────────────────────────────────────────────────────────────────
// 范围一律在代码里钳制：设置块只用基础字段，Windhawk 1.7.3 也能通过校验。
// 这样即使设置面板没有范围注解，误填的数值也不会把文字画飞。
constexpr int kMinPointSize = 6;
constexpr int kMaxPointSize = 72;
constexpr int kFallbackDpiY = 96;  // GetDeviceCaps 取不到 DPI 时的兜底
constexpr int kMaxVerticalOffset = 64;
constexpr int kMinGapAdjust = -8;
constexpr int kMaxGapAdjust = 16;
constexpr int kMaxGapPx = 16;      // 前导留白总上限（自动 + 手动）
constexpr int kMaxAutoGapPx = 6;   // 自动留白刻意保持"很小"
constexpr int kTextWidthPaddingPx = 2;
constexpr int kLogFontBytes = static_cast<int>(sizeof(LOGFONTW));

// 字号放大时固定上提 1px，对齐观感最好；已并入中心补偿，无需用户设置。
constexpr int kBuiltInLiftPx = -1;

// 省略抑制：只给短标签（列头等）让出空间，长文本严格钳制，避免压到右邻列。
// 上限由两部分取较大值：按字号高度增量的通用值，以及按文本构成的补偿值。
constexpr int kMaxShortTextChars = 16;  // 超过此长度视为文件名等长文本，cap=0
constexpr int kExpandPerHeightPx = 4;   // 每 1px 字符高度增量允许 4px 宽度放宽
constexpr int kBaseExpandPx = 16;       // 短标签的基础放宽量
constexpr int kMaxExpandPx = 160;       // 放宽总上限，防止极窄列撑爆

// 长中文列名（如"参与创作的艺术家"）需要单独兜底：全角字符宽度约等于
// 字号高度，9 个字放大后比原列宽多出上百像素，仅按高度增量算的上限兜不住。
// 按字符数线性放宽，全角给足、拉丁收敛，避免中等长度英文文件名越界。
constexpr int kPerWideCharPx = 14;   // 含 CJK / 全角字符时每字符允许让出
constexpr int kPerNarrowCharPx = 6;  // 纯拉丁短标签时每字符允许让出

// 诊断开关：默认关闭。打开后在 Windhawk 日志里打印每次抑制判定的
// 文本、所需宽度、可用宽度与放宽量，用于定位"控件自己已截断字符串"的情况。
constexpr bool kLogEllipsisDecisions = false;

// 判定改用"主窗口白名单"而非"对话框黑名单"：文件夹选项、属性表、菜单、
// 提示框等窗口类名不固定，逐个枚举必然漏；而资源管理器主窗口类名是稳定的。
// 沿 parent 链命中任一层即视为主窗口绘制上下文。
static constexpr std::wstring_view kExplorerFrameClasses[] = {
    L"CabinetWClass"sv,  // Win11 文件资源管理器
    L"ExploreWClass"sv,  // 经典资源管理器 / 桌面窗口
};

// ── 基础工具 ────────────────────────────────────────────────────────────────
// 只比较 ASCII 字母大小写，够用且不需要区域设置；用于识别 None 与图标字体名。
constexpr wchar_t AsciiLower(wchar_t c) {
    return (c >= L'A' && c <= L'Z') ? static_cast<wchar_t>(c + (L'a' - L'A')) : c;
}

bool AsciiIEquals(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (AsciiLower(a[i]) != AsciiLower(b[i])) {
            return false;
        }
    }
    return true;
}

std::wstring_view LogFontFace(const LOGFONTW& lf) {
    size_t len = 0;
    while (len < LF_FACESIZE && lf.lfFaceName[len] != L'\0') {
        ++len;
    }
    return {lf.lfFaceName, len};
}

bool IsKeepFace(std::wstring_view face) {
    return face.empty() || AsciiIEquals(face, L"None"sv);
}

void CopyFace(std::wstring_view face, WCHAR (&dest)[LF_FACESIZE]) {
    std::memset(dest, 0, sizeof(dest));
    const size_t count =
        std::min(face.size(), static_cast<size_t>(LF_FACESIZE - 1));
    std::memcpy(dest, face.data(), count * sizeof(WCHAR));
}

constexpr int AbsHeight(LONG height) {
    return height < 0 ? static_cast<int>(-height) : static_cast<int>(height);
}

// ── 设置 ────────────────────────────────────────────────────────────────────
// face 用定长数组而非 std::wstring：绘制线程每次只做一次加锁拷贝，
// 快照后续全程无堆分配、无字符串生命周期问题。
struct Settings {
    WCHAR face[LF_FACESIZE] = {};
    int sizePt = 0;
    int verticalOffsetPx = 0;
    int iconGapPx = 0;
    bool changeFace = false;
    bool changeSize = false;
    bool autoIconGap = true;
    bool skipSymbolFonts = true;
    bool skipDialogText = true;
};

Settings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;  // 设置热更新在 UI 线程，绘制可发生在任意线程

Settings SnapshotSettings() {
    Settings snapshot = {};
    AcquireSRWLockShared(&g_settingsLock);
    snapshot = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return snapshot;
}

// 用最基础的字符串设置 API 读取并立即释放：不依赖 RAII 包装的实现差异，
// 版本兼容性最好。字符串值写进定长数组前必须校验长度，LOGFONTW 只留 32 个 WCHAR。
void ReloadSettings() {
    PCWSTR raw_face = Wh_GetStringSetting(L"fontFace");
    const std::wstring_view face = raw_face ? std::wstring_view(raw_face)
                                            : std::wstring_view();

    Settings next = {};
    next.changeFace = !IsKeepFace(face);

    if (next.changeFace && face.size() > static_cast<size_t>(LF_FACESIZE - 1)) {
        Wh_Log(L"[ExplorerFont] 字体名过长，已保持原字体名（最多 %d 个字符）",
               LF_FACESIZE - 1);
        next.changeFace = false;
    }
    if (next.changeFace) {
        CopyFace(face, next.face);
    }
    if (raw_face) {
        Wh_FreeStringSetting(raw_face);
    }

    int point_size = Wh_GetIntSetting(L"fontSizePt");
    if (point_size < 0) {
        point_size = 0;  // 0 = 保持原字号
    }
    if (point_size > 0) {
        point_size = std::clamp(point_size, kMinPointSize, kMaxPointSize);
    }
    next.sizePt = point_size;
    next.changeSize = point_size > 0;

    next.verticalOffsetPx = std::clamp(Wh_GetIntSetting(L"verticalOffsetPx"),
                                       -kMaxVerticalOffset, kMaxVerticalOffset);
    next.iconGapPx = std::clamp(Wh_GetIntSetting(L"iconGapPx"),
                                kMinGapAdjust, kMaxGapAdjust);
    next.autoIconGap = Wh_GetIntSetting(L"autoIconGap") != 0;
    next.skipSymbolFonts = Wh_GetIntSetting(L"skipSymbolFonts") != 0;
    next.skipDialogText = Wh_GetIntSetting(L"skipDialogText") != 0;

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = next;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

HMODULE LoadSystemLibrary(LPCWSTR library) {
    return LoadLibraryExW(library, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
}

// ── 字体派生 ────────────────────────────────────────────────────────────────
// 从 HDC 当前 HFONT 读出 LOGFONT，只改名字与高度，保留字重、斜体等属性。

// 图标不是字体，替换后只会变成方框。按字符集或字体名两类识别。
bool ShouldSkipFont(const LOGFONTW& lf, const Settings& settings) {
    if (!settings.skipSymbolFonts) {
        return false;
    }
    if (lf.lfCharSet == SYMBOL_CHARSET) {
        return true;
    }

    static constexpr std::wstring_view kSymbolFonts[] = {
        L"Marlett"sv,
        L"Symbol"sv,
        L"Wingdings"sv,
        L"Wingdings 2"sv,
        L"Wingdings 3"sv,
        L"Webdings"sv,
        L"Segoe MDL2 Assets"sv,
        L"Segoe Fluent Icons"sv,
        L"Segoe UI Symbol"sv,
    };
    const auto face = LogFontFace(lf);
    for (const auto& name : kSymbolFonts) {
        if (AsciiIEquals(face, name)) {
            return true;
        }
    }
    return false;
}

// 返回 false 表示本次不该换字体（无变更或符号字体），调用方继续用原 HFONT。
bool ApplyFontSettings(LOGFONTW& lf, int dpi_y, const Settings& settings) {
    if ((!settings.changeFace && !settings.changeSize) ||
        ShouldSkipFont(lf, settings)) {
        return false;
    }

    if (settings.changeFace) {
        const std::wstring_view configured_face(settings.face);
        std::memset(lf.lfFaceName, 0, sizeof(lf.lfFaceName));
        std::memcpy(lf.lfFaceName, configured_face.data(),
                    configured_face.size() * sizeof(WCHAR));
        // 重置这两项，让字体映射器按新名字重新匹配，别沿用旧字体的字符集/族。
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfPitchAndFamily = static_cast<BYTE>(DEFAULT_PITCH | FF_DONTCARE);
    }

    if (settings.changeSize) {
        // 磅 → 逻辑像素用 MulDiv(point, dpi, 72)；负值表示字符高度而非单元格高度，
        // 这样 'p' 之类带下伸部的字符不会被裁。lfWidth=0 让 GDI 按高度自动匹配宽度。
        int height = -MulDiv(settings.sizePt, dpi_y, 72);
        if (height >= 0) {
            height = -1;
        }
        lf.lfHeight = height;
        lf.lfWidth = 0;
    }
    return true;
}

// 每放大约 5 像素补 1 像素留白，最多 6 像素；字号回到原值时返回 0。
int AutoGap(int old_height, int new_height) {
    const int delta = new_height - old_height;
    return delta <= 0 ? 0 : std::clamp((delta + 3) / 5, 0, kMaxAutoGapPx);
}

// ── 绘制范围 ────────────────────────────────────────────────────────────────
// 系统对话框的控件尺寸由对话框模板按原字体换算后写死，绘制层改不了窗口矩形，
// 放大文字必然被控件边界裁掉，因此这类窗口整次绘制保持原样。
//
// 关键教训：仅靠 WindowFromDC 判定会失效——Win11 控件普遍用 BufferedPaint /
// 双缓冲，DrawText 收到的是内存 DC，WindowFromDC 返回 NULL；上一版把"查不到
// 窗口"一律当主窗口放行，正好把对话框放过。修法是 hook BeginPaint/EndPaint
// 把本次 WM_PAINT 的窗口记进 TLS，让内存 DC 上的绘制继承窗口归属。
thread_local HWND t_paint_window = nullptr;
thread_local int t_paint_depth = 0;

// 只走 parent 链（GA_PARENT 不含 owner）：模态对话框常把主窗口设为 owner，
// 若沿 owner 链向上找，对话框会误命中主窗口类名。
bool IsExplorerFrame(HWND hwnd) {
    wchar_t class_name[64] = {};
    for (int depth = 0; hwnd && depth < 16; ++depth) {
        const int length =
            GetClassNameW(hwnd, class_name, ARRAYSIZE(class_name));
        if (length > 0) {
            const std::wstring_view name(class_name,
                                         static_cast<size_t>(length));
            for (const auto& frame : kExplorerFrameClasses) {
                if (AsciiIEquals(name, frame)) {
                    return true;
                }
            }
        }
        hwnd = GetAncestor(hwnd, GA_PARENT);
    }
    return false;
}

// 两态判定：只有明确属于资源管理器主窗口才放大；其余一律保持原样。
// 查不到窗口时归为 kOther 而非放行——对话框里那些无窗口关联的缓存绘制也一并兜住。
enum class DrawScope { kExplorer, kOther };

DrawScope CurrentDrawScope(HDC hdc) {
    const HWND hwnd = t_paint_window ? t_paint_window : WindowFromDC(hdc);
    return (hwnd && IsExplorerFrame(hwnd)) ? DrawScope::kExplorer
                                           : DrawScope::kOther;
}

// 统一解析 -1 形式的长度，供测宽与"短文本"分档共用。
INT EffectiveTextLength(LPCWSTR text, INT text_len) {
    if (!text) {
        return 0;
    }
    return text_len < 0 ? lstrlenW(text) : text_len;
}

// 直接调系统函数测宽：测量类 hook 已整体移除（测量恒按原字体），不存在递归。
// 含换行/制表符的文本返回 -1，单行宽度不能用来估算折行文本。
int MeasureTextWidth(HDC hdc, LPCWSTR text, INT text_len) {
    if (!hdc || !text) {
        return -1;
    }
    const INT len = EffectiveTextLength(text, text_len);
    if (len <= 0) {
        return 0;
    }
    for (INT i = 0; i < len; ++i) {
        if (text[i] == L'\n' || text[i] == L'\r' || text[i] == L'\t') {
            return -1;
        }
    }
    SIZE size = {};
    if (!GetTextExtentPoint32W(hdc, text, len, &size)) {
        return -1;
    }
    return static_cast<int>(size.cx);
}

bool IsSingleLineWidthMeasurable(UINT format) {
    return (format & DT_SINGLELINE) || !(format & DT_WORDBREAK);
}

// 短标签的放宽上限 = max(按字号高度增量算出的通用值, 按文本构成算出的值)。
// 后者专门覆盖长中文列名：全角字符宽度约等于字号高度，字号一放大，
// 累积宽度增量远大于拉丁短标签，固定或高度比例上限都会让它仍被省略。
// 取 max 保证"只增不减"——已修好的短标签不会因为这次调整退化。
// 长文本返回 0，保持严格钳制，绝不压到右邻列。
int ShortTextExpandCap(LPCWSTR text, INT text_len, int base_cap) {
    const INT len = EffectiveTextLength(text, text_len);
    if (len <= 0 || len > kMaxShortTextChars) {
        return 0;
    }

    bool has_wide = false;
    for (INT i = 0; i < len; ++i) {
        if (text[i] >= 0x2E80) {  // CJK 及全角标点区间
            has_wide = true;
            break;
        }
    }
    const int per_char = has_wide ? kPerWideCharPx : kPerNarrowCharPx;
    const int by_text = std::clamp(
        kBaseExpandPx + static_cast<int>(len) * per_char, 0, kMaxExpandPx);
    return std::max(base_cap, by_text);
}

// 每次绘制期间临时把派生字体选入 HDC，并算出本次绘制的四个几何量：
//   dy            垂直偏移 = 用户微调 + 内置 -1
//   center_expand 上下等量扩展量，保持 DT_VCENTER 的居中点
//   leading_gap   左对齐文字前的留白
//   max_expand    短标签允许向右让出的上限（随字号增量自适应）
// 析构必须先恢复原 HFONT 再删除临时 HFONT；在字体仍被选入时 DeleteObject 会失败。
// active() 为 false 表示本次不介入（无变更或不属于主窗口绘制范围），调用方直接转发原函数。
class FontScope {
public:
    explicit FontScope(HDC hdc) : hdc_(hdc) {
        if (!hdc_) {
            return;
        }
        const Settings settings = SnapshotSettings();

        // 非主窗口绘制（对话框、菜单、提示框等）默认整次走原路径。
        // active_ 保持 false，连内置 -1 与图标留白都不叠加，
        // 否则会出现"字没变大却挪了位置"的半吊子效果。
        if (settings.skipDialogText &&
            CurrentDrawScope(hdc_) != DrawScope::kExplorer) {
            return;
        }

        dy_ = settings.verticalOffsetPx;
        leading_gap_ = std::clamp(settings.iconGapPx, 0, kMaxGapPx);

        // 既无字体变更也无位移时完全走原路径，不产生任何 HDC 操作。
        if (!settings.changeFace && !settings.changeSize && dy_ == 0 &&
            leading_gap_ == 0) {
            return;
        }
        active_ = true;
        if (!settings.changeFace && !settings.changeSize) {
            return;  // 仅位移，无需碰 HFONT
        }
        InstallFont(settings);
    }

    ~FontScope() {
        if (!hdc_ || !new_font_) {
            return;
        }
        if (old_font_ && old_font_ != HGDI_ERROR) {
            SelectObject(hdc_, old_font_);
        }
        DeleteObject(new_font_);
    }

    FontScope(const FontScope&) = delete;
    FontScope& operator=(const FontScope&) = delete;

    bool active() const { return active_; }
    int dy() const { return dy_; }
    int center_expand() const { return center_expand_; }
    int leading_gap() const { return leading_gap_; }
    int max_expand_px() const { return max_expand_; }

private:
    void InstallFont(const Settings& settings) {
        int dpi_y = GetDeviceCaps(hdc_, LOGPIXELSY);
        if (dpi_y <= 0) {
            dpi_y = kFallbackDpiY;
        }

        HGDIOBJ current = GetCurrentObject(hdc_, OBJ_FONT);
        if (!current || current == HGDI_ERROR) {
            return;
        }
        LOGFONTW lf = {};
        if (GetObjectW(static_cast<HANDLE>(current), kLogFontBytes, &lf) !=
            kLogFontBytes) {
            return;
        }

        const int old_height = AbsHeight(lf.lfHeight);
        if (!ApplyFontSettings(lf, dpi_y, settings)) {
            return;  // 符号字体等被忽略：保留手动微调，不加自动留白
        }
        const int new_height = AbsHeight(lf.lfHeight);

        // 字号放大越多，文字宽度增量越大，允许让出的空间也相应增加；
        // 只换字体名（高度不变）时给基础值。长文本不走这条放宽路径。
        const int height_delta = std::max(0, new_height - old_height);
        max_expand_ = std::clamp(
            kBaseExpandPx + height_delta * kExpandPerHeightPx, 0, kMaxExpandPx);

        if (settings.autoIconGap && settings.changeSize) {
            leading_gap_ = std::clamp(leading_gap_ + AutoGap(old_height, new_height),
                                      0, kMaxGapPx);
        }
        if (settings.changeSize) {
            // 中心补偿常开：字号撑开时上下各让一半，视觉中心不相对图标下坠。
            center_expand_ = std::max(0, (new_height - old_height) / 2);
        }
        if (center_expand_ > 0) {
            dy_ += kBuiltInLiftPx;  // 只在字号确实放大时叠加，改字体名不受影响
        }

        new_font_ = reinterpret_cast<HGDIOBJ>(CreateFontIndirectW(&lf));
        if (!new_font_) {
            return;
        }
        HGDIOBJ previous = SelectObject(hdc_, new_font_);
        if (!previous || previous == HGDI_ERROR) {
            DeleteObject(new_font_);
            new_font_ = nullptr;
            return;
        }
        old_font_ = previous;
    }

    HDC hdc_ = nullptr;
    HGDIOBJ old_font_ = nullptr;
    HGDIOBJ new_font_ = nullptr;
    int dy_ = 0;
    int center_expand_ = 0;
    int leading_gap_ = 0;
    int max_expand_ = 0;
    bool active_ = false;
};

// 前导留白只让出矩形内部（只动 left），不占右边界。
// 放不下时把留白整段收回、把像素还给文字；居中/右对齐跳过，以免推离对齐基准。
// needed / margins 由 AdjustDrawRect 统一测好传入，避免一次绘制重复测宽。
int ResolveLeadingGap(const RECT& bounds,
                      const FontScope& scope,
                      UINT format,
                      int needed,
                      int margins) {
    const int gap = scope.leading_gap();
    if (gap <= 0 || (format & (DT_CENTER | DT_RIGHT)) ||
        !IsSingleLineWidthMeasurable(format)) {
        return 0;
    }
    if (needed < 0) {
        return gap;  // 测不出宽度时不猜，保留留白
    }
    const int wanted = needed + margins + kTextWidthPaddingPx;
    // RECT 成员是 LONG，显式收成 int 参与比较，避免隐式转换与模板推导歧义。
    const int available =
        static_cast<int>(bounds.right - bounds.left) - gap;
    return wanted > available ? 0 : gap;
}

// 有 rect 的绘制统一走这里：垂直补偿 → 水平留白 → 省略抑制 → 右边界兜底。
// bounds 是调用方按原字体算出的文本区；字号放大后短文字常只差几像素就被截断。
RECT AdjustDrawRect(HDC hdc,
                    const RECT& bounds,
                    const FontScope& scope,
                    UINT format,
                    LPCWSTR text,
                    INT text_len,
                    const DRAWTEXTPARAMS* params) {
    RECT rect = bounds;

    // ① 上下等量扩展保持居中点，再整体平移 dy（已含内置 -1）。
    const int expand = scope.center_expand();
    if (expand > 0) {
        rect.top -= expand;
        rect.bottom += expand;
    }
    const int dy = scope.dy();
    if (dy != 0) {
        rect.top += dy;
        rect.bottom += dy;
    }

    // 单次测宽，供 ② 留白决策与 ③ 省略抑制共用。
    const bool measurable = IsSingleLineWidthMeasurable(format);
    const int needed = measurable ? MeasureTextWidth(hdc, text, text_len) : -1;
    const int margins = params ? params->iLeftMargin + params->iRightMargin : 0;
    const INT char_count = EffectiveTextLength(text, text_len);
    const bool left_aligned = !(format & (DT_CENTER | DT_RIGHT));

    // ② 留白只加在 left 上，放不下就整段收回。
    rect.left += ResolveLeadingGap(bounds, scope, format, needed, margins);

    // ③ 抑制提前省略：短标签按实测宽度把右边界放宽到刚好容纳。
    // 不要求 DT_END_ELLIPSIS —— DrawText 默认就裁剪到 rect，文字超宽即显示不全。
    // 关键用 min(extra, cap)：早期版本写成"extra <= cap 才放宽"，超出量一过
    // 上限就整段失效。cap 现在按文本构成自适应（见 ShortTextExpandCap），
    // 长中文列名与拉丁短标签各取所需；长文本 cap=0 保持严格钳制。
    const int cap = left_aligned
                        ? ShortTextExpandCap(text, text_len,
                                             scope.max_expand_px())
                        : 0;
    if (cap > 0 && needed >= 0) {
        const int wanted = needed + margins + kTextWidthPaddingPx;
        const int available = static_cast<int>(rect.right - rect.left);
        const int extra = wanted - available;
        const int granted = std::clamp(extra, 0, cap);
        rect.right += granted;

        if (kLogEllipsisDecisions) {
            // 指定 text_len 时原串未必以 NUL 结尾，拷到定长缓冲再打印，
            // 才能看出控件是否早已把字符串截断成带省略号的形式。
            wchar_t preview[25] = {};
            const int preview_len = std::min<INT>(char_count, 24);
            std::memcpy(preview, text, preview_len * sizeof(WCHAR));
            Wh_Log(L"[ExplorerFont] chars=%d need=%d avail=%d extra=%d cap=%d granted=%d text=%s",
                   char_count, wanted, available, extra, cap, granted, preview);
        }
    }

    // ④ 兜底：越界只允许到第 ③ 步算出的上限。
    const int limit = static_cast<int>(bounds.right) + cap;
    if (rect.right > limit) {
        rect.right = limit;
    }
    return rect;
}

// ── 原始函数指针 ────────────────────────────────────────────────────────────
// 只 hook 绘制函数与 BeginPaint/EndPaint：测量恒走原路径，调用方按原字体
// 分配的行高与图标位置才不会被带偏。
using DrawTextW_t = decltype(&DrawTextW);
using DrawTextExW_t = decltype(&DrawTextExW);
using ExtTextOutW_t = decltype(&ExtTextOutW);
using TextOutW_t = decltype(&TextOutW);
using TabbedTextOutW_t = decltype(&TabbedTextOutW);
using BeginPaint_t = decltype(&BeginPaint);
using EndPaint_t = decltype(&EndPaint);

DrawTextW_t g_original_draw_text_w = nullptr;
DrawTextExW_t g_original_draw_text_ex_w = nullptr;
ExtTextOutW_t g_original_ext_text_out_w = nullptr;
TextOutW_t g_original_text_out_w = nullptr;
TabbedTextOutW_t g_original_tabbed_text_out_w = nullptr;
BeginPaint_t g_original_begin_paint = nullptr;
EndPaint_t g_original_end_paint = nullptr;

// ── Hook 实现 ───────────────────────────────────────────────────────────────
// 记录本次 WM_PAINT 的窗口归属。DrawText 等绘制很可能发生在由此派生的内存 DC
// 上（BufferedPaint / CreateCompatibleDC），它们反查不到窗口，全靠这段上下文。
// depth 计数保证嵌套 BeginPaint 时由最外层决定归属，内层不会提前清空。
HDC WINAPI BeginPaint_Hook(HWND hwnd, LPPAINTSTRUCT paint) {
    HDC hdc = g_original_begin_paint(hwnd, paint);
    if (hdc && hwnd) {
        if (t_paint_depth == 0) {
            t_paint_window = hwnd;
        }
        ++t_paint_depth;
    }
    return hdc;
}

BOOL WINAPI EndPaint_Hook(HWND hwnd, const PAINTSTRUCT* paint) {
    const BOOL result = g_original_end_paint(hwnd, paint);
    if (t_paint_depth > 0 && --t_paint_depth == 0) {
        t_paint_window = nullptr;
    }
    return result;
}

// DT_CALCRECT 是测量：必须完全走原路径、原字体，且要在 FontScope 构造之前返回。
// 之前 FontScope 已把大字体选入 HDC 再"透传"，控件按大字体测出"放不下"
// 便自行把文本截断成带省略号的字符串，之后无论怎么放宽 rect 都救不回来。
// 提到最前面也真正落实了"测量恒按原字体"的设计，避免布局尺寸被带偏。
INT WINAPI DrawTextW_Hook(HDC hdc,
                          LPCWSTR text,
                          INT text_len,
                          LPRECT rect,
                          UINT format) {
    if ((format & DT_CALCRECT) || !rect) {
        return g_original_draw_text_w(hdc, text, text_len, rect, format);
    }
    FontScope scope(hdc);
    if (!scope.active()) {
        return g_original_draw_text_w(hdc, text, text_len, rect, format);
    }
    RECT adjusted = AdjustDrawRect(hdc, *rect, scope, format, text, text_len,
                                   nullptr);
    return g_original_draw_text_w(hdc, text, text_len, &adjusted, format);
}

INT WINAPI DrawTextExW_Hook(HDC hdc,
                            LPWSTR text,
                            INT text_len,
                            LPRECT rect,
                            UINT format,
                            LPDRAWTEXTPARAMS params) {
    if ((format & DT_CALCRECT) || !rect) {
        return g_original_draw_text_ex_w(hdc, text, text_len, rect, format,
                                         params);
    }
    FontScope scope(hdc);
    if (!scope.active()) {
        return g_original_draw_text_ex_w(hdc, text, text_len, rect, format,
                                         params);
    }
    // DrawTextEx 的左右 margin 要计入所需宽度，否则会误判"放不下"。
    RECT adjusted = AdjustDrawRect(hdc, *rect, scope, format, text, text_len,
                                   params);
    return g_original_draw_text_ex_w(hdc, text, text_len, &adjusted, format,
                                     params);
}

// ExtTextOut 没有省略标志，文字超出 ETO_CLIPPED 裁剪区就被直接裁掉，
// 视觉上等同于"提前省略"。comctl32 的列头等控件常走这条路径。
// 短标签按实测宽度放宽裁剪区（与 AdjustDrawRect 同一上限）；
// 长文本收回留白、保持原裁剪，绝不压到右邻列。
BOOL WINAPI ExtTextOutW_Hook(HDC hdc,
                             INT x,
                             INT y,
                             UINT options,
                             const RECT* rect,
                             LPCWSTR text,
                             UINT cch,
                             const INT* dx) {
    FontScope scope(hdc);
    const int dy = scope.active() ? scope.dy() : 0;
    int gap = scope.active() ? scope.leading_gap() : 0;

    RECT storage = {};
    const RECT* clip = rect;
    if (rect && scope.active()) {
        storage = *rect;
        if (dy != 0) {
            storage.top += dy;
            storage.bottom += dy;
        }
        if ((options & ETO_CLIPPED) && text) {
            // 与 AdjustDrawRect 用同一个 cap：列头无论走 DrawText 还是
            // ExtTextOut，长中文列名都能拿到一致的放宽额度。
            const int cap = ShortTextExpandCap(text, static_cast<INT>(cch),
                                               scope.max_expand_px());
            const int needed =
                MeasureTextWidth(hdc, text, static_cast<INT>(cch));
            if (needed >= 0) {
                const int wanted_right =
                    x + gap + needed + kTextWidthPaddingPx;
                if (wanted_right > storage.right) {
                    if (cap > 0) {
                        // RECT 成员是 LONG；先统一成 int 再比较，
                        // 否则 std::min 会因 int / long 混合而模板推导失败。
                        const int cap_right =
                            static_cast<int>(storage.right) + cap;
                        storage.right = std::min(wanted_right, cap_right);
                    } else if (gap > 0) {
                        gap = 0;  // 长文本：收回留白，交给 GDI 裁剪
                    }
                }
            }
        }
        clip = &storage;
    }
    return g_original_ext_text_out_w(hdc, x + gap, y + dy, options, clip, text,
                                     cch, dx);
}

BOOL WINAPI TextOutW_Hook(HDC hdc,
                          INT x,
                          INT y,
                          LPCWSTR text,
                          INT cch) {
    FontScope scope(hdc);
    const int dy = scope.active() ? scope.dy() : 0;
    const int gap = scope.active() ? scope.leading_gap() : 0;
    return g_original_text_out_w(hdc, x + gap, y + dy, text, cch);
}

LONG WINAPI TabbedTextOutW_Hook(HDC hdc,
                                INT x,
                                INT y,
                                LPCWSTR text,
                                INT text_len,
                                INT tab_count,
                                const INT* tab_stops,
                                INT tab_origin) {
    FontScope scope(hdc);
    const int dy = scope.active() ? scope.dy() : 0;
    const int gap = scope.active() ? scope.leading_gap() : 0;
    // 起点右移时 tab 计算的原点必须同步，否则制表位整体左偏。
    return g_original_tabbed_text_out_w(hdc, x + gap, y + dy, text, text_len,
                                        tab_count, tab_stops, tab_origin + gap);
}

// ── Hook 注册 ───────────────────────────────────────────────────────────────
// 表驱动：新增一个函数只需加一行。label 单独存宽字符串，日志不必做 ANSI 转换。
struct HookSpec {
    HMODULE module;
    const char* export_name;
    const wchar_t* label;
    void* detour;
    void** original;
};

bool ApplyHooks(HMODULE user32, HMODULE gdi32) {
    const HookSpec specs[] = {
        {user32, "DrawTextW", L"DrawTextW",
         reinterpret_cast<void*>(DrawTextW_Hook),
         reinterpret_cast<void**>(&g_original_draw_text_w)},
        {user32, "DrawTextExW", L"DrawTextExW",
         reinterpret_cast<void*>(DrawTextExW_Hook),
         reinterpret_cast<void**>(&g_original_draw_text_ex_w)},
        {user32, "TabbedTextOutW", L"TabbedTextOutW",
         reinterpret_cast<void*>(TabbedTextOutW_Hook),
         reinterpret_cast<void**>(&g_original_tabbed_text_out_w)},
        {gdi32, "ExtTextOutW", L"ExtTextOutW",
         reinterpret_cast<void*>(ExtTextOutW_Hook),
         reinterpret_cast<void**>(&g_original_ext_text_out_w)},
        {gdi32, "TextOutW", L"TextOutW",
         reinterpret_cast<void*>(TextOutW_Hook),
         reinterpret_cast<void**>(&g_original_text_out_w)},
        {user32, "BeginPaint", L"BeginPaint",
         reinterpret_cast<void*>(BeginPaint_Hook),
         reinterpret_cast<void**>(&g_original_begin_paint)},
        {user32, "EndPaint", L"EndPaint",
         reinterpret_cast<void*>(EndPaint_Hook),
         reinterpret_cast<void**>(&g_original_end_paint)},
    };

    bool ok = true;
    for (const auto& spec : specs) {
        void* target =
            reinterpret_cast<void*>(GetProcAddress(spec.module, spec.export_name));
        if (!target) {
            Wh_Log(L"[ExplorerFont] 未找到函数 %s", spec.label);
            ok = false;
            continue;
        }
        // 只以 original 是否被填充判断成败，不依赖 Wh_SetFunctionHook 的返回类型。
        Wh_SetFunctionHook(target, spec.detour, spec.original);
        if (!*spec.original) {
            Wh_Log(L"[ExplorerFont] Hook 安装失败 %s", spec.label);
            ok = false;
        }
    }
    return ok;
}

}  // namespace

// ── Windhawk 生命周期 ───────────────────────────────────────────────────────
BOOL Wh_ModInit() {
    InitializeSRWLock(&g_settingsLock);
    ReloadSettings();

    HMODULE user32 = LoadSystemLibrary(L"user32.dll");
    HMODULE gdi32 = LoadSystemLibrary(L"gdi32.dll");
    if (!user32 || !gdi32) {
        Wh_Log(L"[ExplorerFont] 未能加载 user32.dll / gdi32.dll");
        return FALSE;
    }
    return ApplyHooks(user32, gdi32) ? TRUE : FALSE;
}

// 绘制每次都从当前快照派生字体，保存设置即生效；XAML 界面仍需重启 explorer.exe。
void Wh_ModSettingsChanged() {
    ReloadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"[ExplorerFont] unloaded");
}
