// ==WindhawkMod==
// @id              aleph-canvas
// @name            Aleph - Infinite Window Canvas
// @name:zh-CN      Aleph - 无限窗口画布
// @description     Arrange open windows across a pannable canvas with live DWM previews.
// @description:zh-CN 在可平移画布上管理已打开的窗口，并显示 DWM 实时预览。
// @version         1.8.0
// @author          CYoJkoY
// @github          https://github.com/CYoJkoY
// @license         MIT
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @compilerOptions -ladvapi32 -ld2d1 -ldwmapi -ldwrite -lgdi32 -lole32 -lshell32 -lshcore -luuid -lwindowscodecs
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Aleph - Infinite Window Canvas

Aleph presents eligible application windows on a navigable canvas. Live
windows use DWM thumbnails; minimized and other-desktop windows use icon cards.
Drag cards to arrange them without moving the real desktop windows.

## Controls

- `Ctrl+Alt+Shift+F12`: toggle the canvas. Leave the setting empty to disable
  the hotkey. If the hotkey does
  nothing, another program may already use it; pick a different combination.
- Mouse wheel: zoom around the pointer.
- Alt+wheel over a stacked card: cycle the stack; plain wheel still zooms.
- The card header carries up to three buttons: pin, group (windows of the same
  application only), and close. Hovering a button shows its localized label.
- Group joins the windows of one application into a stack; pressing it again
  splits the stack into a row. The grouped state is saved with the layout.
- Hold Alt and drag a card onto another one to make a stack of your own, even
  across different applications; a drop only counts when the cards overlap
  solidly. Hold Alt and drag a member out to release just that window. A plain
  drop never groups anything. Custom stacks are saved with the layout.
- Close sends `WM_CLOSE` to the real window, so the application closes; the
  card leaves the canvas once the next scan reports the window gone.
- Middle mouse drag: pan the canvas.
- Hold Space and drag empty canvas: pan.
- Drag a card: arrange it; nearby edges snap into alignment.
- Shift+click a stacked card: bring the next window of the same app to the front.
- Double-click a card or press Enter: focus the window; for an other-desktop window, first try to move it here.
- Tab / Shift+Tab: select a card; arrow keys pan; Home fits all cards; Escape hides the canvas.
- Click or drag the minimap: navigate the canvas.
- The canvas hides when another window takes input focus.

## Notes

Card positions are saved in the Windhawk mod storage directory as
`layout-<user-SID>.json`.
Canvas coordinates use physical pixels relative to the virtual desktop origin.
Card badges and the empty-canvas hint follow the interface language setting.
`Auto` uses the system UI language and falls back to English when that language
is not one of the thirteen supported ones.
Icon cards ask the shell for the largest available icon (256 px when the jumbo
image list exists) and fall back to smaller lists, then to the window icon.
Arranging cards never moves real application windows. Activating an
other-desktop card asks the documented `IVirtualDesktopManager` interface to
move that window onto the desktop displaying the canvas. Windows can reject
requests for windows owned by another process. A failed move is logged and
the mod still attempts normal foreground activation, which can also fail.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Behavior:
    - language: auto
      $name: Interface language
      $name:zh-CN: 界面语言
      $description: Language used by canvas interface text. Auto follows the system UI language and falls back to English when unsupported.
      $description:zh-CN: 画布界面文本使用的语言。自动模式跟随系统界面语言，不支持时回退为英文。
      $options:
        - auto: Auto
        - en: English
        - fr: Français
        - zh: 简体中文
        - zh_TW: 繁體中文
        - ja: 日本語
        - ko: 한국어
        - ru: Русский
        - pl: Polski
        - es: Español
        - pt: Português
        - de: Deutsch
        - tr: Türkçe
        - it: Italiano
      $options:zh-CN:
        - auto: 自动
        - en: English
        - fr: Français
        - zh: 简体中文
        - zh_TW: 繁體中文
        - ja: 日本語
        - ko: 한국어
        - ru: Русский
        - pl: Polski
        - es: Español
        - pt: Português
        - de: Deutsch
        - tr: Türkçe
        - it: Italiano
    - toggle_hotkey: Ctrl+Alt+Shift+F12
      $name: Toggle hotkey
      $name:zh-CN: 显示/隐藏快捷键
      $description: Global hotkey used to show or hide the canvas. Leave blank to disable. Prefer modifiers plus a function key; Ctrl+Alt with a letter can collide with AltGr.
      $description:zh-CN: 用于显示或隐藏画布的全局快捷键，留空禁用。建议使用修饰键加功能键；Ctrl+Alt 加字母可能与 AltGr 冲突。
    - open_on_start: false
      $name: Open on startup
      $name:zh-CN: 启动时打开
      $description: Open the canvas when its Windhawk tool process starts.
      $description:zh-CN: Windhawk 工具进程启动时打开画布。
    - animation_ms: 140
      $name: Dive-in animation duration
      $name:zh-CN: 潜入动画时长
      $description: Duration of the camera transition when activating a window, in milliseconds. Range 60-600.
      $description:zh-CN: 激活窗口时的摄像机过渡时长，单位毫秒。范围 60-600。
    - rescan_interval_ms: 400
      $name: Window scan interval
      $name:zh-CN: 窗口扫描间隔
      $description: How often the visible canvas checks for opened or closed windows, in milliseconds. Range 100-5000.
      $description:zh-CN: 画布显示时检查窗口打开或关闭的频率，单位毫秒。范围 100-5000。
  $name: Behavior
  $name:zh-CN: 行为

- Canvas:
    - show_grid: true
      $name: Show grid
      $name:zh-CN: 显示网格
      $description: Draw a coordinate grid behind window cards.
      $description:zh-CN: 在窗口卡片后方绘制坐标网格。
    - grid_spacing: 96
      $name: Grid spacing
      $name:zh-CN: 网格间距
      $description: World-space spacing between grid lines. Range 16-512.
      $description:zh-CN: 网格线之间的世界坐标间距。范围 16-512。
    - gap: 32
      $name: Card gap
      $name:zh-CN: 卡片间距
      $description: Minimum world-space gap used when packing new cards. Range 0-256.
      $description:zh-CN: 自动排列新卡片时使用的最小世界坐标间距。范围 0-256。
    - snap_distance: 8
      $name: Snap distance
      $name:zh-CN: 磁吸距离
      $description: Screen-pixel distance for aligning card edges and centers. Range 0-48.
      $description:zh-CN: 卡片边缘和中心轴触发对齐的屏幕像素距离。范围 0-48。
    - include_other_desktops: true
      $name: Include other virtual desktops
      $name:zh-CN: 包含其他虚拟桌面
      $description: Show eligible windows cloaked on another virtual desktop as icon cards.
      $description:zh-CN: 将其他虚拟桌面上符合条件的隐藏窗口显示为图标卡片。
    - save_layout: true
      $name: Save card positions
      $name:zh-CN: 保存卡片位置
      $description: Persist card positions between runs.
      $description:zh-CN: 在多次运行之间保存卡片位置。
    - initial_layout: packed
      $name: Initial layout
      $name:zh-CN: 初始布局
      $description: Place new cards in free space or at their real desktop positions.
      $description:zh-CN: 将新卡片放入空闲区域，或放在其实际桌面位置。
      $options:
        - packed: Packed
        - desktop: Desktop positions
      $options:zh-CN:
        - packed: 自动排列
        - desktop: 桌面位置
    - card_stacks_enabled: true
      $name: Group card stacks
      $name:zh-CN: 启用卡片堆叠
      $description: Group windows from the same application and fan older members behind the active card.
      $description:zh-CN: 将同一应用程序的窗口分为一组，并把较早的成员展开到当前卡片后方。
    - card_stack_step_degrees: 65
      $name: Stack angle step (tenths of a degree)
      $name:zh-CN: 堆叠角度步进（十分之一度）
      $description: Rotation added by each stack position, in tenths of a degree. Range 0-900.
      $description:zh-CN: 每个堆叠位置增加的旋转角度，单位为十分之一度。范围 0-900。
    - card_stack_max_degrees: 240
      $name: Stack maximum angle (tenths of a degree)
      $name:zh-CN: 堆叠最大角度（十分之一度）
      $description: Maximum rotation applied to a stack member, in tenths of a degree. Range 0-900.
      $description:zh-CN: 堆叠成员可用的最大旋转角度，单位为十分之一度。范围 0-900。
    - card_stack_scale_percent: 975
      $name: Stack scale factor (thousandths)
      $name:zh-CN: 堆叠缩放系数（千分之一）
      $description: Per-level scale multiplier, in thousandths. Range 800-1000.
      $description:zh-CN: 每一级堆叠使用的缩放系数，单位为千分之一。范围 800-1000。
    - card_stack_max_depth: 5
      $name: Stack depth
      $name:zh-CN: 堆叠深度
      $description: Maximum number of visible layers behind the active card. Range 0-8.
      $description:zh-CN: 当前卡片后方可见的最大层数。范围 0-8。
    - card_stack_rotation_direction: counter_clockwise
      $name: Stack rotation direction
      $name:zh-CN: 堆叠旋转方向
      $description: Direction the fan turns away from the front card.
      $description:zh-CN: 卡片堆从当前卡片向外展开时的旋转方向。
      $options:
        - counter_clockwise: Counter-clockwise
        - clockwise: Clockwise
        - none: No rotation
      $options:zh-CN:
        - counter_clockwise: 逆时针
        - clockwise: 顺时针
        - none: 不旋转
  $name: Canvas
  $name:zh-CN: 画布

- Appearance:
    - min_zoom_percent: 15
      $name: Minimum zoom
      $name:zh-CN: 最小缩放
      $description: Minimum canvas zoom percentage. Range 15-245.
      $description:zh-CN: 画布最小缩放百分比。范围 15-245。
    - max_zoom_percent: 250
      $name: Maximum zoom
      $name:zh-CN: 最大缩放
      $description: Maximum canvas zoom percentage. Range 20-250 and at least five points above the minimum.
      $description:zh-CN: 画布最大缩放百分比。范围 20-250，且至少比最小值高 5。
    - minimap_enabled: true
      $name: Show minimap
      $name:zh-CN: 显示小地图
      $description: Show the interactive overview map in the lower-right corner.
      $description:zh-CN: 在右下角显示可交互的概览地图。
    - minimap_size: 180
      $name: Minimap size
      $name:zh-CN: 小地图尺寸
      $description: Minimap width and height in display-scaled pixels. Range 96-420.
      $description:zh-CN: 小地图宽高，按显示缩放比例调整。范围 96-420。
    - background_color: 0B0E14
      $name: Background color
      $name:zh-CN: 背景颜色
      $description: Six-digit RGB hex color without the leading #.
      $description:zh-CN: 六位 RGB 十六进制颜色，不含开头的 #。
    - grid_color: 1B2436
      $name: Grid color
      $name:zh-CN: 网格颜色
      $description: Six-digit RGB hex color without the leading #.
      $description:zh-CN: 六位 RGB 十六进制颜色，不含开头的 #。
    - accent_color: 38BDF8
      $name: Accent color
      $name:zh-CN: 强调色
      $description: Six-digit RGB hex color without the leading #.
      $description:zh-CN: 六位 RGB 十六进制颜色，不含开头的 #。
    - text_color: E2E8F0
      $name: Text color
      $name:zh-CN: 文字颜色
      $description: Six-digit RGB hex color without the leading #.
      $description:zh-CN: 六位 RGB 十六进制颜色，不含开头的 #。
    - muted_text_color: 64748B
      $name: Muted text color
      $name:zh-CN: 次要文字颜色
      $description: Six-digit RGB hex color without the leading #.
      $description:zh-CN: 六位 RGB 十六进制颜色，不含开头的 #。
    - card_fill_color: "111827"
      $name: Card fill color
      $name:zh-CN: 卡片填充颜色
      $description: Six-digit RGB hex color without the leading #.
      $description:zh-CN: 六位 RGB 十六进制颜色，不含开头的 #。
  $name: Appearance
  $name:zh-CN: 外观

- Application:
    - exclude_list: [""]
      $name: Excluded executables
      $name:zh-CN: 排除的可执行文件
      $description: One executable name per entry, such as `program.exe`. Names are matched case-insensitively; lines beginning with # are comments.
      $description:zh-CN: 每个条目填写一个可执行文件名，例如 `program.exe`。名称不区分大小写，以 # 开头的条目为注释。
  $name: Application
  $name:zh-CN: 应用程序
*/
// ==/WindhawkModSettings==


// System includes plus the shared Windhawk and Win32 helper headers.

#include <windows.h>
#include <windowsx.h>
#include <sddl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shobjidl.h>
#include <wincodec.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// The DWM cloak values are older than the SDK headers some builds ship.
#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif
#ifndef DWM_CLOAKED_APP
#define DWM_CLOAKED_APP 0x00000001
#endif
#ifndef DWM_CLOAKED_SHELL
#define DWM_CLOAKED_SHELL 0x00000002
#endif

// Repository-wide helpers. The paths stay relative to this header so the
// amalgamator can inline them in the same way as local modules.

// Header-only helpers around the Windhawk API.
//
// This file is inlined by tools/bundle.py, so it must stay self-contained and
// must never include another repository header. Every function is inline so
// the amalgamated translation unit keeps a single definition.

#include <windows.h>

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <string>

namespace yoko {

// Reads an integer setting and clamps it to the documented range.
//
// Windhawk returns 0 for a missing value and does not reject out-of-range
// input, so the clamp has to happen here. An inverted range is a programming
// error in the settings block; it is logged and collapsed to the lower bound
// instead of producing undefined behavior.
inline int GetClampedIntSetting(const wchar_t* path, int minimum, int maximum) {
    if (path == nullptr) {
        Wh_Log(L"GetClampedIntSetting called without a setting path");
        return minimum;
    }
    if (maximum < minimum) {
        Wh_Log(L"GetClampedIntSetting range is inverted for %s", path);
        return minimum;
    }
    const int value = Wh_GetIntSetting(path);
    if (value < minimum) {
        return minimum;
    }
    if (value > maximum) {
        return maximum;
    }
    return value;
}

// Reads a string setting and falls back when the value is absent.
//
// WindhawkUtils::StringSetting owns the buffer, so the result is copied before
// the temporary goes out of scope.
inline std::wstring GetStringSetting(const wchar_t* path,
                                     const wchar_t* fallback) {
    const wchar_t* safeFallback = fallback != nullptr ? fallback : L"";
    if (path == nullptr) {
        Wh_Log(L"GetStringSetting called without a setting path");
        return safeFallback;
    }
    WindhawkUtils::StringSetting value =
        WindhawkUtils::StringSetting::make(path);
    const wchar_t* text = value.get();
    if (text == nullptr) {
        return safeFallback;
    }
    return text;
}

// Logs the current thread's last error next to a short context string.
//
// The error is captured first so the logging call itself cannot replace it.
inline void LogLastError(const wchar_t* context) {
    const DWORD error = GetLastError();
    Wh_Log(L"%s failed: %lu", context != nullptr ? context : L"Win32 call",
           error);
}

}  // namespace yoko

// Header-only Win32 ownership and validation helpers.
//
// This file is inlined by tools/bundle.py, so it must stay self-contained and
// must never include another repository header.

#include <windows.h>

namespace yoko::win32 {

inline constexpr float kMinimumDpiScale = 0.5f;
inline constexpr float kMaximumDpiScale = 4.0f;
inline constexpr UINT kDefaultDpi = 96;

// Owns a kernel handle and closes it exactly once.
class UniqueHandle {
public:
    UniqueHandle() = default;

    explicit UniqueHandle(HANDLE handle) : handle_(handle) {}

    ~UniqueHandle() { reset(); }

    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;

    UniqueHandle(UniqueHandle&& other) noexcept : handle_(other.release()) {}

    UniqueHandle& operator=(UniqueHandle&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    HANDLE get() const { return handle_; }

    bool valid() const {
        return handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE;
    }

    HANDLE release() {
        const HANDLE handle = handle_;
        handle_ = nullptr;
        return handle;
    }

    void reset(HANDLE handle = nullptr) {
        if (handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE) {
            CloseHandle(handle_);
        }
        handle_ = handle;
    }

private:
    HANDLE handle_ = nullptr;
};

// Owns a GDI object and deletes it exactly once.
class UniqueGdiObject {
public:
    UniqueGdiObject() = default;

    explicit UniqueGdiObject(HGDIOBJ object) : object_(object) {}

    ~UniqueGdiObject() { reset(); }

    UniqueGdiObject(const UniqueGdiObject&) = delete;
    UniqueGdiObject& operator=(const UniqueGdiObject&) = delete;

    UniqueGdiObject(UniqueGdiObject&& other) noexcept
        : object_(other.release()) {}

    UniqueGdiObject& operator=(UniqueGdiObject&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    HGDIOBJ get() const { return object_; }

    bool valid() const { return object_ != nullptr; }

    HGDIOBJ release() {
        const HGDIOBJ object = object_;
        object_ = nullptr;
        return object;
    }

    void reset(HGDIOBJ object = nullptr) {
        if (object_ != nullptr) {
            DeleteObject(object_);
        }
        object_ = object;
    }

private:
    HGDIOBJ object_ = nullptr;
};

// A CURSORINFO that is zero-initialized with the required cbSize.
struct UniqueCursorInfo {
    CURSORINFO value{};

    UniqueCursorInfo() { value.cbSize = sizeof(value); }

    bool query() { return GetCursorInfo(&value) != FALSE; }

    bool visible() const {
        return (value.flags & CURSOR_SHOWING) != 0 && value.hCursor != nullptr;
    }
};

// A window handle is only usable while the window still exists.
inline bool IsValidWindow(HWND window) {
    return window != nullptr && IsWindow(window) != FALSE;
}

// Converts a monitor DPI to a clamped scale factor. 96 DPI maps to 1.0.
inline float DpiScaleForDpi(UINT dpi) {
    const UINT effectiveDpi = dpi == 0 ? kDefaultDpi : dpi;
    const float scale = static_cast<float>(effectiveDpi) /
                        static_cast<float>(kDefaultDpi);
    if (scale < kMinimumDpiScale) {
        return kMinimumDpiScale;
    }
    if (scale > kMaximumDpiScale) {
        return kMaximumDpiScale;
    }
    return scale;
}

}  // namespace yoko::win32

// Compile-time geometry, timing and policy values.
//
// Everything a user may want to change is a Windhawk setting; this header only
// holds canvas structure that must stay internally consistent (frame pacing,
// hit-test tolerances, cap sizes).


constexpr UINT kToggleHotkeyId = 0xA1E1;
constexpr int kMaxCards = 64;
constexpr int kMaxLayoutEntries = 256;
constexpr int kMaxExcludeEntries = 64;
constexpr int kMaxStackDepth = 8;

constexpr int kFrameIntervalMs = 16;
constexpr int kMinRescanIntervalMs = 100;
constexpr int kMaxRescanIntervalMs = 5000;

constexpr int kMinZoomPercent = 15;
constexpr int kMaxZoomPercent = 250;
constexpr int kMinAnimationMs = 60;
constexpr int kMaxAnimationMs = 600;

constexpr float kZoomStep = 1.15f;
constexpr float kCameraSmoothingPerSecond = 20.0f;
constexpr float kCameraEpsilonPx = 0.2f;
constexpr float kZoomEpsilon = 0.001f;
constexpr float kCardOpacityEpsilon = 0.001f;

constexpr float kCardHeaderPx = 44.0f;
constexpr float kCardHeaderMinPx = 14.0f;
constexpr float kCardHeaderMaxPx = 54.0f;
constexpr float kCardHeaderButtonPx = 30.0f;
constexpr float kCardHeaderButtonInsetPx = 6.0f;
constexpr float kCardHeaderButtonGapPx = 8.0f;
constexpr float kCardCornerRadiusPx = 8.0f;
constexpr float kCardTitlePaddingPx = 10.0f;
constexpr float kCardIconMaxPx = 128.0f;

constexpr float kMinimapMarginPx = 16.0f;
constexpr int kMinimapMinSize = 96;
constexpr int kMinimapMaxSize = 420;
constexpr float kMinimapPaddingPx = 8.0f;

constexpr int kMaxGridLines = 400;
constexpr float kMinGridSpacingPx = 6.0f;

constexpr DWORD kLayoutSaveDelayMs = 800;
constexpr DWORD kIconRetryDelayMs = 4000;
constexpr DWORD kThumbnailRetryDelayMs = 2000;

// A hotkey registration can fail transiently, for example right after a
// settings reload while another program still holds the combination. The
// worker retries with a capped exponential backoff so a reloaded hotkey
// becomes active without the user touching the settings again. The cap is a
// full minute: a combination held for good by another program costs the idle
// worker one wake-up per minute, not one every few seconds.
constexpr DWORD kHotkeyRetryInitialMs = 1000;
constexpr DWORD kHotkeyRetryMaxMs = 60000;

[[nodiscard]] inline DWORD NextHotkeyRetryDelay(DWORD previous) {
    if (previous == 0) {
        return kHotkeyRetryInitialMs;
    }
    const DWORD doubled =
        previous > kHotkeyRetryMaxMs / 2 ? kHotkeyRetryMaxMs : previous * 2;
    return std::min(doubled, kHotkeyRetryMaxMs);
}

// Value types shared by the canvas modules.


// Compile-time canvas interface strings and language resolution.
//
// The string table order is fixed: it mirrors LanguageId, the Windhawk setting
// options, and the native localization regressions.


#include <array>
#include <cstdint>
#include <string_view>

enum class LanguageId : uint8_t {
    English,
    French,
    SimplifiedChinese,
    Japanese,
    Korean,
    TraditionalChinese,
    Russian,
    Polish,
    Spanish,
    Portuguese,
    German,
    Turkish,
    Italian,
    Count,
};

enum class CanvasTextId : uint8_t {
    MinimizedBadge,
    OtherDesktopBadge,
    PreviewUnavailableBadge,
    EmptyCanvas,
    PinTooltip,
    PinActiveTooltip,
    CloseTooltip,
    GroupTooltip,
    UngroupTooltip,
    Count,
};

struct CanvasLocaleEntry {
    std::wstring_view texts[static_cast<size_t>(LanguageId::Count)];
};

inline constexpr std::array<CanvasLocaleEntry,
                            static_cast<size_t>(CanvasTextId::Count)>
    kCanvasStrings = {{
        {{L"MINIMIZED", L"RÉDUIT", L"已最小化", L"最小化", L"최소화",
          L"已最小化", L"СВЁРНУТО", L"ZMINIMALIZOWANE", L"MINIMIZADO",
          L"MINIMIZADO", L"MINIMIERT", L"KÜÇÜLTÜLDÜ", L"RIDOTTO"}},
        {{L"OTHER DESKTOP", L"AUTRE BUREAU", L"其他桌面", L"別のデスクトップ",
          L"다른 데스크톱", L"其他桌面", L"ДРУГОЙ РАБОЧИЙ СТОЛ",
          L"INNY PULPIT", L"OTRO ESCRITORIO", L"OUTRA ÁREA DE TRABALHO",
          L"ANDERER DESKTOP", L"BAŞKA MASAÜSTÜ", L"ALTRO DESKTOP"}},
        {{L"PREVIEW UNAVAILABLE", L"APERÇU INDISPONIBLE", L"预览不可用",
          L"プレビュー利用不可", L"미리 보기 사용 불가", L"預覽無法使用",
          L"ПРЕДПРОСМОТР НЕДОСТУПЕН", L"PODGLĄD NIEDOSTĘPNY",
          L"VISTA PREVIA NO DISPONIBLE", L"PRÉ-VISUALIZAÇÃO INDISPONÍVEL",
          L"VORSCHAU NICHT VERFÜGBAR", L"ÖNİZLEME KULLANILAMIYOR",
          L"ANTEPRIMA NON DISPONIBILE"}},
        {{L"No eligible windows", L"Aucune fenêtre éligible", L"没有可用窗口",
          L"対象のウィンドウがありません", L"대상 창이 없습니다",
          L"沒有可用視窗", L"Нет подходящих окон", L"Brak kwalifikujących okien",
          L"No hay ventanas aptas", L"Não há janelas elegíveis",
          L"Keine geeigneten Fenster", L"Uygun pencere yok",
          L"Nessuna finestra idonea"}},
        {{L"Pin to top", L"Épingler", L"置顶", L"最前面に固定",
          L"맨 위에 고정", L"置頂", L"Закрепить", L"Przypnij", L"Fijar arriba",
          L"Fixar no topo", L"Anheften", L"Üste sabitle", L"Fissa in alto"}},
        {{L"Unpin", L"Détacher", L"取消置顶", L"固定を解除", L"고정 해제",
          L"取消置頂", L"Открепить", L"Odepnij", L"Desfijar", L"Desafixar",
          L"Lösen", L"Sabitlemeyi kaldır", L"Rimuovi fissaggio"}},
        {{L"Close window", L"Fermer la fenêtre", L"关闭窗口",
          L"ウィンドウを閉じる", L"창 닫기", L"關閉視窗", L"Закрыть окно",
          L"Zamknij okno", L"Cerrar ventana", L"Fechar janela",
          L"Fenster schließen", L"Pencereyi kapat", L"Chiudi finestra"}},
        {{L"Group windows", L"Regrouper", L"分组", L"グループ化", L"그룹화",
          L"分組", L"Сгруппировать", L"Grupuj", L"Agrupar", L"Agrupar",
          L"Gruppieren", L"Grupla", L"Raggruppa"}},
        {{L"Ungroup", L"Dissocier", L"取消分组", L"グループ解除", L"그룹 해제",
          L"取消分組", L"Разгруппировать", L"Rozgrupuj", L"Desagrupar",
          L"Desagrupar", L"Gruppierung aufheben", L"Gruplamayı kaldır",
          L"Separa"}},
    }};

// Setting key -> language. Keys are stable identifiers; display names live in
// the Windhawk metadata so translators only touch one place.
struct LanguageOption {
    const wchar_t* code;
    LanguageId language;
};

inline constexpr LanguageOption kLanguageOptions[] = {
    {L"en", LanguageId::English},
    {L"fr", LanguageId::French},
    {L"zh", LanguageId::SimplifiedChinese},
    {L"zh_TW", LanguageId::TraditionalChinese},
    {L"ja", LanguageId::Japanese},
    {L"ko", LanguageId::Korean},
    {L"ru", LanguageId::Russian},
    {L"pl", LanguageId::Polish},
    {L"es", LanguageId::Spanish},
    {L"pt", LanguageId::Portuguese},
    {L"de", LanguageId::German},
    {L"tr", LanguageId::Turkish},
    {L"it", LanguageId::Italian},
};

inline constexpr wchar_t kAutoLanguageCode[] = L"auto";

// System UI language -> canvas language, in addition to the Chinese sublanguage
// rule that needs the sublanguage identifier.
struct SystemLanguageRule {
    WORD primary;
    LanguageId language;
};

inline constexpr SystemLanguageRule kSystemLanguageRules[] = {
    {LANG_FRENCH, LanguageId::French},
    {LANG_JAPANESE, LanguageId::Japanese},
    {LANG_KOREAN, LanguageId::Korean},
    {LANG_RUSSIAN, LanguageId::Russian},
    {LANG_POLISH, LanguageId::Polish},
    {LANG_SPANISH, LanguageId::Spanish},
    {LANG_PORTUGUESE, LanguageId::Portuguese},
    {LANG_GERMAN, LanguageId::German},
    {LANG_TURKISH, LanguageId::Turkish},
    {LANG_ITALIAN, LanguageId::Italian},
};

[[nodiscard]] inline const wchar_t* CanvasText(CanvasTextId textId,
                                               LanguageId language) {
    const auto entryIndex = static_cast<size_t>(textId);
    const auto languageIndex = static_cast<size_t>(language);
    if (entryIndex >= static_cast<size_t>(CanvasTextId::Count) ||
        languageIndex >= static_cast<size_t>(LanguageId::Count)) {
        return L"";
    }
    return kCanvasStrings[entryIndex].texts[languageIndex].data();
}

[[nodiscard]] inline size_t CanvasTextLength(CanvasTextId textId,
                                             LanguageId language) {
    const auto entryIndex = static_cast<size_t>(textId);
    const auto languageIndex = static_cast<size_t>(language);
    if (entryIndex >= static_cast<size_t>(CanvasTextId::Count) ||
        languageIndex >= static_cast<size_t>(LanguageId::Count)) {
        return 0;
    }
    return kCanvasStrings[entryIndex].texts[languageIndex].size();
}

// Every Chinese sublanguage listed here uses Traditional characters: Taiwan,
// Hong Kong and Macau. Simplified Chinese is the fallback for the rest.
[[nodiscard]] inline bool IsTraditionalChineseSubLanguage(WORD subLanguage) {
    return subLanguage == SUBLANG_CHINESE_TRADITIONAL ||
           subLanguage == SUBLANG_CHINESE_HONGKONG ||
           subLanguage == SUBLANG_CHINESE_MACAU;
}

[[nodiscard]] inline LanguageId DetectSystemLanguage() {
    const LANGID identifier = GetUserDefaultUILanguage();
    const WORD primary = PRIMARYLANGID(identifier);
    if (primary == LANG_CHINESE) {
        return IsTraditionalChineseSubLanguage(SUBLANGID(identifier))
                   ? LanguageId::TraditionalChinese
                   : LanguageId::SimplifiedChinese;
    }
    for (const SystemLanguageRule& rule : kSystemLanguageRules) {
        if (rule.primary == primary) {
            return rule.language;
        }
    }
    return LanguageId::English;
}

// Matches an explicit setting key. "auto" is not a language and is handled by
// ResolveLanguageSetting instead.
[[nodiscard]] inline bool TryParseLanguageCode(const wchar_t* code,
                                               LanguageId& language) {
    if (code == nullptr) {
        return false;
    }
    for (const LanguageOption& option : kLanguageOptions) {
        if (_wcsicmp(option.code, code) == 0) {
            language = option.language;
            return true;
        }
    }
    return false;
}

// Resolves the stored setting value. "auto" and a missing value follow the
// system UI language; an unrecognized key falls back to English so a corrupted
// setting never leaves the canvas without text.
[[nodiscard]] inline LanguageId ResolveLanguageSetting(const wchar_t* code) {
    if (code == nullptr || _wcsicmp(code, kAutoLanguageCode) == 0) {
        return DetectSystemLanguage();
    }
    LanguageId language = LanguageId::English;
    if (!TryParseLanguageCode(code, language)) {
        return LanguageId::English;
    }
    return language;
}

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct RectF {
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;

    float width() const { return right - left; }
    float height() const { return bottom - top; }
    float centerX() const { return (left + right) * 0.5f; }
    float centerY() const { return (top + bottom) * 0.5f; }
    bool empty() const { return right <= left || bottom <= top; }
};

inline RectF MakeRectF(float left, float top, float width, float height) {
    return RectF{left, top, left + width, top + height};
}

inline RectF RectFromRect(const RECT& rect) {
    return RectF{static_cast<float>(rect.left), static_cast<float>(rect.top),
                 static_cast<float>(rect.right),
                 static_cast<float>(rect.bottom)};
}

inline RectF UnionRect(const RectF& a, const RectF& b) {
    if (a.empty()) {
        return b;
    }
    if (b.empty()) {
        return a;
    }
    return RectF{std::min(a.left, b.left), std::min(a.top, b.top),
                 std::max(a.right, b.right), std::max(a.bottom, b.bottom)};
}

inline RectF GrowRect(const RectF& rect, float amount) {
    return RectF{rect.left - amount, rect.top - amount, rect.right + amount,
                 rect.bottom + amount};
}

inline bool RectsOverlap(const RectF& a, const RectF& b) {
    return a.left < b.right && b.left < a.right && a.top < b.bottom &&
           b.top < a.bottom;
}

inline bool ContainsPoint(const RectF& rect, float x, float y) {
    return x >= rect.left && x < rect.right && y >= rect.top && y < rect.bottom;
}

struct Camera {
    float centerX = 0.0f;
    float centerY = 0.0f;
    float zoom = 0.6f;
    float targetCenterX = 0.0f;
    float targetCenterY = 0.0f;
    float targetZoom = 0.6f;
    bool zoomAnchored = false;
    Vec2 zoomAnchorWorld{};
    Vec2 zoomAnchorScreen{};

    bool settled() const {
        return std::fabs(targetCenterX - centerX) < kCameraEpsilonPx &&
               std::fabs(targetCenterY - centerY) < kCameraEpsilonPx &&
               std::fabs(targetZoom - zoom) < kZoomEpsilon;
    }

    void snapToTarget() {
        centerX = targetCenterX;
        centerY = targetCenterY;
        zoom = targetZoom;
    }
};

enum class WindowVisualState {
    Live,
    Minimized,
    OtherDesktop,
};

enum class InitialLayout {
    Packed,
    Desktop,
};

enum class CardHeaderAction : uint8_t {
    None,
    Pin,
    Group,
    Close,
};

// One DWM thumbnail handle. Registration is the only ownership, so moving a
// card inside the vector never leaks or double-unregisters.
class Thumbnail {
public:
    Thumbnail() = default;
    ~Thumbnail() { reset(); }

    Thumbnail(const Thumbnail&) = delete;
    Thumbnail& operator=(const Thumbnail&) = delete;

    Thumbnail(Thumbnail&& other) noexcept : handle_(other.release()) {}

    Thumbnail& operator=(Thumbnail&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }
        return *this;
    }

    HTHUMBNAIL get() const { return handle_; }
    bool valid() const { return handle_ != nullptr; }

    HTHUMBNAIL release() {
        const HTHUMBNAIL handle = handle_;
        handle_ = nullptr;
        return handle;
    }

    void reset(HTHUMBNAIL handle = nullptr) {
        if (handle_ != nullptr) {
            DwmUnregisterThumbnail(handle_);
        }
        handle_ = handle;
    }

private:
    HTHUMBNAIL handle_ = nullptr;
};

struct CardStackPlacement {
    std::wstring groupKey;
    size_t slot = 0;
    bool grouped = true;
    bool visible = true;
    float rotationRadians = 0.0f;
    float scale = 1.0f;
    float opacity = 1.0f;
    bool drawAboveStack = false;
    // The one rectangle the whole stack occupies. When a stack of two or more
    // forms, the frame takes the front card's world rectangle and every member
    // is drawn inside it, so the fan always shares a single size and centre
    // instead of every window keeping its own proportions. Empty while the
    // card stands alone, so a lone window never gets a stale frame.
    RectF frameWorld{};
};

struct StackCardTransition {
    std::wstring fingerprint;
    float fromRotation = 0.0f;
    float fromScale = 1.0f;
    float fromOpacity = 1.0f;
    float toRotation = 0.0f;
    float toScale = 1.0f;
    bool outgoing = false;
    bool incoming = false;
};

struct StackCycleTransition {
    std::wstring groupKey;
    DWORD startTick = 0;
    DWORD durationMs = 0;
    std::vector<StackCardTransition> cards;
};

struct Card {
    HWND hwnd = nullptr;
    DWORD processId = 0;
    DWORD threadId = 0;
    bool destroyed = false;
    std::wstring windowClass;
    WindowVisualState state = WindowVisualState::Live;
    RECT framePx = {};
    RectF world = {};
    std::wstring title;
    std::wstring fingerprint;
    std::wstring exePath;
    std::wstring applicationKey;
    // Hand-made stack membership, written by Alt-dropping one card onto
    // another and saved with the layout. A custom stack may span different
    // applications, so it wins over the automatic per-executable key. Empty
    // means the card follows the automatic grouping.
    std::wstring stackId;
    bool hasStackPeers = false;
    // Scan index from EnumWindows, topmost window first (0 = topmost).
    // Copied from ScannedWindow on every merge so stack order follows
    // the desktop Z-order instead of card insertion order.
    size_t zOrder = 0;
    CardStackPlacement stack;
    Thumbnail thumbnail;
    bool thumbnailFailed = false;
    DWORD thumbnailRetryTick = 0;
    RECT thumbnailDestination = {};
    bool thumbnailVisible = false;
    bool pinned = false;
};

// World rectangle a card occupies on the canvas. Every member of a formed
// stack shares the stack frame, so the fan reads as one card no matter how
// differently the member windows are sized; the front card owns the live
// preview inside that frame. Ungrouped cards and stacks without a frame use
// their own world rectangle.
inline RectF StackWorldRect(const Card& card) {
    if (card.stack.grouped && !card.stack.groupKey.empty() &&
        !card.stack.frameWorld.empty()) {
        return card.stack.frameWorld;
    }
    return card.world;
}

inline bool IsStackBackground(const Card& card) {
    return card.stack.grouped && !card.stack.groupKey.empty() &&
           card.stack.slot != 0;
}

inline bool SameActiveStack(const Card& left, const Card& right) {
    return left.stack.grouped && right.stack.grouped &&
           !left.stack.groupKey.empty() && left.stack.groupKey == right.stack.groupKey;
}

// The key that decides which stack a card belongs to. A hand-made stack wins
// over the automatic per-executable key so a custom group never leaks into the
// application-wide one.
inline std::wstring StackGroupKeyOf(const Card& card) {
    return card.stackId.empty() ? card.applicationKey : card.stackId;
}

struct ScannedWindow {
    HWND hwnd = nullptr;
    DWORD processId = 0;
    DWORD threadId = 0;
    std::wstring windowClass;
    WindowVisualState state = WindowVisualState::Live;
    RECT framePx = {};
    std::wstring title;
    std::wstring fingerprint;
    std::wstring exePath;
    std::wstring applicationKey;
    // Scan index from EnumWindows, topmost window first (0 = topmost).
    // Stacks order members by this rank so the focused/topmost window
    // becomes the straight front card instead of an arbitrary member.
    size_t zOrder = 0;
};

inline bool MatchesWindow(const Card& card, const ScannedWindow& window) {
    return !card.destroyed && card.hwnd == window.hwnd &&
           card.processId == window.processId &&
           card.threadId == window.threadId &&
           card.windowClass == window.windowClass &&
           card.exePath == window.exePath;
}

struct CanvasSettings {
    std::wstring toggleHotkey = L"Ctrl+Alt+Shift+F12";
    LanguageId language = LanguageId::English;
    bool openOnStart = false;
    int animationMs = 140;
    int rescanIntervalMs = 400;
    bool showGrid = true;
    int gridSpacing = 96;
    int gap = 32;
    int snapDistance = 8;
    int minZoomPercent = 15;
    int maxZoomPercent = 250;
    bool includeOtherDesktops = true;
    bool saveLayout = true;
    bool minimapEnabled = true;
    int minimapSize = 180;
    InitialLayout initialLayout = InitialLayout::Packed;
    uint32_t backgroundColor = 0x0B0E14;
    uint32_t gridColor = 0x1B2436;
    uint32_t accentColor = 0x38BDF8;
    uint32_t textColor = 0xE2E8F0;
    uint32_t mutedTextColor = 0x64748B;
    uint32_t cardFillColor = 0x111827;
    std::vector<std::wstring> excludeList;
    bool stacksEnabled = true;
    float stackStepDegrees = 6.5f;
    float stackMaxDegrees = 24.0f;
    float stackScaleFactor = 0.975f;
    int stackMaxDepth = 5;
    int stackRotationDirection = -1;
};

struct LayoutEntry {
    std::wstring fingerprint;
    RectF world = {};
    bool pinned = false;
    bool grouped = true;
    // Manual stack the card belongs to, saved so a hand-made group survives a
    // restart. Empty for cards that follow the automatic application group.
    std::wstring stackId;
};

struct LayoutSnapshot {
    std::vector<LayoutEntry> entries;
};

// The single mutable canvas state owned by the worker thread.


struct CanvasContext {
    // Worker thread plumbing.
    HANDLE workerThread = nullptr;
    HANDLE stopEvent = nullptr;
    HANDLE settingsEvent = nullptr;
    HANDLE readyEvent = nullptr;
    bool startupSucceeded = false;
    DWORD workerThreadId = 0;
    std::atomic_bool settingsDirty{false};
    HWINEVENTHOOK destroyHook = nullptr;
    UINT hotkeyModifiers = 0;
    UINT hotkeyKey = 0;
    bool hotkeyRegistered = false;
    UINT hotkeyId = kToggleHotkeyId;
    // Pending retry state for a hotkey that failed to register. Zero delay
    // means no retry is scheduled.
    DWORD hotkeyRetryDelayMs = 0;
    DWORD hotkeyRetryTick = 0;

    // Windows. The host carries the DWM thumbnails and the grid; the HUD is a
    // click-through layered window drawn above it.
    HWND hostWindow = nullptr;
    HWND hudWindow = nullptr;
    // Non-owning. The worker thread creates the COM manager before the canvas
    // loop starts and releases it after the loop returns.
    IVirtualDesktopManager* desktopManager = nullptr;
    bool visible = false;
    RECT virtualScreen = {};
    int chromeDpi = 96;
    HMONITOR chromeMonitor = nullptr;

    // Cards and camera.
    std::vector<Card> cards;
    std::vector<StackCycleTransition> stackTransitions;
    Camera camera{};
    bool cameraMotion = false;
    DWORD lastRescanTick = 0;
    DWORD lastFrameTick = 0;
    bool layoutLoaded = false;
    bool cameraInitialized = false;
    bool layoutDirty = false;
    DWORD layoutDirtyTick = 0;
    bool scanRequested = false;
    bool sceneDirty = true;

    // Interaction state.
    bool panning = false;
    POINT panAnchorCursor = {};
    Vec2 panAnchorWorld = {};
    int draggedCard = -1;
    POINT dragGrabOffset = {};
    POINT dragCursor = {};
    // True once an Alt-drag has pulled the grabbed card out of its stack. The
    // same gesture may still hand that card to another stack, but never back
    // to the one it just left.
    bool dragDetached = false;
    // The stack key the dragged card was pulled out of, so a drop ignores the
    // fan the card came from instead of sliding it back in.
    std::wstring detachFromKey;
    // The card an Alt+drag would currently stack onto, painted as a drop cue.
    int dropTargetCard = -1;
    int hoverCard = -1;
    int selectedCard = -1;
    CardHeaderAction hoverHeaderAction = CardHeaderAction::None;
    bool draggingMinimap = false;
    bool minimapHover = false;
    RectF minimapRect = {};
    RectF guideVertical = {};
    RectF guideHorizontal = {};
    bool hasGuideVertical = false;
    bool hasGuideHorizontal = false;

    // Dive-in transition.
    bool diving = false;
    DWORD diveStartTick = 0;
    int diveCard = -1;
    Camera diveStart{};
    Camera diveTarget{};

    // Runtime resources.
    ID2D1Factory* d2dFactory = nullptr;
    ID2D1DCRenderTarget* hudTarget = nullptr;
    ID2D1SolidColorBrush* hudBrush = nullptr;
    IDWriteFactory* writeFactory = nullptr;
    IDWriteTextFormat* titleFormat = nullptr;
    IDWriteTextFormat* badgeFormat = nullptr;
    IDWriteTextFormat* tooltipFormat = nullptr;
    IWICImagingFactory* wicFactory = nullptr;
    HDC hudDc = nullptr;
    HBITMAP hudBitmap = nullptr;
    HGDIOBJ hudOriginalBitmap = nullptr;
    int hudWidth = 0;
    int hudHeight = 0;
    std::unordered_map<std::wstring, ID2D1Bitmap*> iconCache;
    std::unordered_map<std::wstring, DWORD> iconRetryTick;

    // Settings and persisted layout.
    CanvasSettings settings{};
    LayoutSnapshot layout{};
};

extern CanvasContext context;

// Camera projections and small numeric helpers.


inline float ClampFloat(float value, float minimum, float maximum) {
    if (value < minimum) {
        return minimum;
    }
    if (value > maximum) {
        return maximum;
    }
    return value;
}

inline float ClampZoom(float zoom) {
    const float minimum =
        static_cast<float>(context.settings.minZoomPercent) / 100.0f;
    const float maximum =
        static_cast<float>(context.settings.maxZoomPercent) / 100.0f;
    return ClampFloat(zoom, minimum, std::max(minimum, maximum));
}

// Window chrome uses physical pixels but should stay proportional to the
// display that hosts the canvas, so every fixed metric is scaled by the DPI of
// the monitor under the cursor.
inline float ChromeScale() {
    return yoko::win32::DpiScaleForDpi(static_cast<UINT>(context.chromeDpi));
}

inline float CardHeaderHeight() {
    return std::clamp(kCardHeaderPx * ChromeScale() * context.camera.zoom,
                      kCardHeaderMinPx * ChromeScale(),
                      kCardHeaderMaxPx * ChromeScale());
}

inline float ScreenCenterX() {
    return static_cast<float>(context.virtualScreen.right -
                              context.virtualScreen.left) *
           0.5f;
}

inline float ScreenCenterY() {
    return static_cast<float>(context.virtualScreen.bottom -
                              context.virtualScreen.top) *
           0.5f;
}

// World units are virtual-screen pixels, so a card's stored rectangle matches
// the window's own frame rectangle until the user rearranges the canvas.
inline RectF WorldToScreen(const RectF& world, const Camera& camera) {
    return RectF{
        (world.left - camera.centerX) * camera.zoom + ScreenCenterX(),
        (world.top - camera.centerY) * camera.zoom + ScreenCenterY(),
        (world.right - camera.centerX) * camera.zoom + ScreenCenterX(),
        (world.bottom - camera.centerY) * camera.zoom + ScreenCenterY()};
}

inline RectF CardScreenBounds(const Card& card) {
    RectF rect = WorldToScreen(StackWorldRect(card), context.camera);
    rect.top -= CardHeaderHeight();
    return rect;
}

// Largest rectangle with the given content aspect that fits inside bounds and
// stays centred. A live preview is letterboxed with this instead of being
// stretched onto a differently shaped stack frame, so a window never appears
// distorted when it shares a frame with a front card of another size.
inline RectF AspectFitRect(const RectF& bounds, float contentWidth,
                           float contentHeight) {
    if (bounds.empty() || contentWidth <= 0.0f || contentHeight <= 0.0f) {
        return bounds;
    }
    const float scale = std::min(bounds.width() / contentWidth,
                                 bounds.height() / contentHeight);
    const float width = contentWidth * scale;
    const float height = contentHeight * scale;
    return MakeRectF(bounds.centerX() - width * 0.5f,
                     bounds.centerY() - height * 0.5f, width, height);
}

inline Vec2 ScreenToWorld(Vec2 point, const Camera& camera) {
    return Vec2{(point.x - ScreenCenterX()) / camera.zoom + camera.centerX,
                (point.y - ScreenCenterY()) / camera.zoom + camera.centerY};
}

// Keeps the world point under the cursor fixed while the zoom changes.
inline void ZoomAtPoint(Camera& camera, float newZoom, Vec2 cursor) {
    const float clamped = ClampZoom(newZoom);
    const Vec2 anchor = ScreenToWorld(cursor, camera);
    camera.targetZoom = clamped;
    camera.targetCenterX = anchor.x - (cursor.x - ScreenCenterX()) / clamped;
    camera.targetCenterY = anchor.y - (cursor.y - ScreenCenterY()) / clamped;
    camera.zoomAnchorWorld = anchor;
    camera.zoomAnchorScreen = cursor;
    camera.zoomAnchored = true;
}

inline void CenterOnPoint(Camera& camera, Vec2 worldPoint) {
    camera.zoomAnchored = false;
    camera.targetCenterX = worldPoint.x;
    camera.targetCenterY = worldPoint.y;
}

inline float SmoothTowards(float current, float target, float factor) {
    const float blended = current + (target - current) * factor;
    return std::fabs(target - blended) < kCameraEpsilonPx ? target : blended;
}

inline void AdvanceCamera(Camera& camera, float seconds) {
    const float factor =
        1.0f - std::exp(-kCameraSmoothingPerSecond * std::max(seconds, 0.0f));
    const float zoomDelta = camera.targetZoom - camera.zoom;
    camera.zoom = std::fabs(zoomDelta) < kZoomEpsilon
                      ? camera.targetZoom
                      : camera.zoom + zoomDelta * factor;
    if (camera.zoomAnchored) {
        camera.centerX = camera.zoomAnchorWorld.x -
            (camera.zoomAnchorScreen.x - ScreenCenterX()) / camera.zoom;
        camera.centerY = camera.zoomAnchorWorld.y -
            (camera.zoomAnchorScreen.y - ScreenCenterY()) / camera.zoom;
        return;
    }
    camera.centerX = SmoothTowards(camera.centerX, camera.targetCenterX, factor);
    camera.centerY = SmoothTowards(camera.centerY, camera.targetCenterY, factor);
}

inline float EaseOutCubic(float t) {
    const float clamped = ClampFloat(t, 0.0f, 1.0f);
    const float inverse = 1.0f - clamped;
    return 1.0f - inverse * inverse * inverse;
}

inline RectF ViewportWorld(const Camera& camera) {
    const Vec2 topLeft = ScreenToWorld(Vec2{0.0f, 0.0f}, camera);
    const Vec2 bottomRight =
        ScreenToWorld(Vec2{static_cast<float>(context.virtualScreen.right -
                                              context.virtualScreen.left),
                           static_cast<float>(context.virtualScreen.bottom -
                                              context.virtualScreen.top)},
                      camera);
    return RectF{topLeft.x, topLeft.y, bottomRight.x, bottomRight.y};
}

inline bool ParseHexColor(const wchar_t* text, uint32_t& out) {
    if (text == nullptr) {
        return false;
    }
    const wchar_t* cursor = text;
    while (*cursor == L' ' || *cursor == L'#') {
        ++cursor;
    }
    if (wcslen(cursor) != 6) {
        return false;
    }
    uint32_t value = 0;
    for (int index = 0; index < 6; ++index) {
        const wchar_t digit = cursor[index];
        uint32_t nibble = 0;
        if (digit >= L'0' && digit <= L'9') {
            nibble = static_cast<uint32_t>(digit - L'0');
        } else if (digit >= L'a' && digit <= L'f') {
            nibble = static_cast<uint32_t>(digit - L'a' + 10);
        } else if (digit >= L'A' && digit <= L'F') {
            nibble = static_cast<uint32_t>(digit - L'A' + 10);
        } else {
            return false;
        }
        value = (value << 4) | nibble;
    }
    out = value;
    return true;
}

// Small string helpers shared by settings, scanning and rendering.


static std::wstring TrimText(const std::wstring& value) {
    const size_t first = value.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) {
        return std::wstring();
    }
    const size_t last = value.find_last_not_of(L" \t\r\n");
    return value.substr(first, last - first + 1);
}

static std::wstring ToLowerText(const std::wstring& value) {
    std::wstring result = value;
    for (wchar_t& item : result) {
        item = static_cast<wchar_t>(towlower(item));
    }
    return result;
}

static std::wstring ExecutableName(const std::wstring& path) {
    const size_t slash = path.find_last_of(L"\\/");
    return ToLowerText(slash == std::wstring::npos ? path
                                                   : path.substr(slash + 1));
}

static std::string ToUtf8(const std::wstring& value) {
    if (value.empty()) {
        return std::string();
    }
    const int required = WideCharToMultiByte(
        CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), nullptr, 0,
        nullptr, nullptr);
    if (required <= 0) {
        return std::string();
    }
    std::string result(static_cast<size_t>(required), '\0');
    if (WideCharToMultiByte(CP_UTF8, 0, value.c_str(),
                            static_cast<int>(value.size()), result.data(),
                            required, nullptr, nullptr) != required) {
        return std::string();
    }
    return result;
}

static std::wstring FromUtf8(const std::string& value) {
    if (value.empty()) {
        return std::wstring();
    }
    const int required = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS, value.c_str(),
        static_cast<int>(value.size()), nullptr, 0);
    if (required <= 0) {
        return std::wstring();
    }
    std::wstring result(static_cast<size_t>(required), L'\0');
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.c_str(),
                            static_cast<int>(value.size()), result.data(),
                            required) != required) {
        return std::wstring();
    }
    return result;
}

static std::wstring FormatInteger(int value) {
    wchar_t buffer[32] = {};
    swprintf_s(buffer, L"%d", value);
    return std::wstring(buffer);
}

// Maps the named rotation direction option to the multiplier used by the stack
// geometry: 1 clockwise, 0 no rotation, -1 counter-clockwise. The legacy
// numeric spellings stay accepted so a stored layout or an old settings file
// keeps its direction; anything else falls back to counter-clockwise.
static int RotationDirectionFromText(const std::wstring& value) {
    const std::wstring text = ToLowerText(TrimText(value));
    if (text == L"clockwise" || text == L"1") {
        return 1;
    }
    if (text == L"none" || text == L"0") {
        return 0;
    }
    return -1;
}

// Settings schema, hotkey parsing and persistence paths.


static uint32_t ReadColorSetting(const wchar_t* path, uint32_t fallback) {
    uint32_t parsed = 0;
    WindhawkUtils::StringSetting value = WindhawkUtils::StringSetting::make(path);
    return ParseHexColor(value.get(), parsed) ? parsed : fallback;
}

struct NamedHotkeyValue {
    const wchar_t* name;
    UINT value;
};

static constexpr NamedHotkeyValue kNamedHotkeyKeys[] = {
    {L"space", VK_SPACE},       {L"tab", VK_TAB},
    {L"enter", VK_RETURN},      {L"return", VK_RETURN},
    {L"escape", VK_ESCAPE},     {L"esc", VK_ESCAPE},
    {L"backspace", VK_BACK},    {L"insert", VK_INSERT},
    {L"delete", VK_DELETE},     {L"home", VK_HOME},
    {L"end", VK_END},           {L"pageup", VK_PRIOR},
    {L"pagedown", VK_NEXT},     {L"left", VK_LEFT},
    {L"right", VK_RIGHT},       {L"up", VK_UP},
    {L"down", VK_DOWN},
};

static constexpr NamedHotkeyValue kNamedHotkeyModifiers[] = {
    {L"ctrl", MOD_CONTROL}, {L"control", MOD_CONTROL},
    {L"alt", MOD_ALT},      {L"shift", MOD_SHIFT},
    {L"win", MOD_WIN},      {L"windows", MOD_WIN},
};

static bool ParseCharacterKey(wchar_t character, UINT& key) {
    const bool digit = character >= L'0' && character <= L'9';
    const bool letter = character >= L'a' && character <= L'z';
    if (!digit && !letter) {
        return false;
    }
    key = static_cast<UINT>(towupper(character));
    return true;
}

static bool ParseFunctionKey(const std::wstring& token, UINT& key) {
    if (token.size() < 2 || token[0] != L'f') {
        return false;
    }
    UINT number = 0;
    for (size_t index = 1; index < token.size(); ++index) {
        const wchar_t character = token[index];
        if (character < L'0' || character > L'9') {
            return false;
        }
        number = number * 10 + static_cast<UINT>(character - L'0');
        if (number > 24) {
            return false;
        }
    }
    if (number == 0) {
        return false;
    }
    key = VK_F1 + number - 1;
    return true;
}

static bool ParseKeyName(const std::wstring& token, UINT& key) {
    const std::wstring lower = ToLowerText(token);
    if (lower.size() == 1) {
        return ParseCharacterKey(lower[0], key);
    }
    for (const NamedHotkeyValue& entry : kNamedHotkeyKeys) {
        if (lower == entry.name) {
            key = entry.value;
            return true;
        }
    }
    return ParseFunctionKey(lower, key);
}

struct ParsedHotkey {
    UINT modifiers = 0;
    UINT key = 0;
};

static bool ParseHotkeyToken(const std::wstring& token, ParsedHotkey& parsed) {
    const std::wstring lower = ToLowerText(token);
    for (const NamedHotkeyValue& entry : kNamedHotkeyModifiers) {
        if (lower == entry.name) {
            parsed.modifiers |= entry.value;
            return true;
        }
    }
    if (parsed.key != 0) {
        return false;
    }
    return ParseKeyName(lower, parsed.key);
}

static bool ParseHotkeyText(const std::wstring& text, UINT& modifiers, UINT& key) {
    modifiers = 0;
    key = 0;
    const std::wstring trimmed = TrimText(text);
    if (trimmed.empty()) {
        return false;
    }

    size_t start = 0;
    ParsedHotkey parsed;
    while (start <= trimmed.size()) {
        const size_t plus = trimmed.find(L'+', start);
        const std::wstring token = TrimText(
            trimmed.substr(start, plus == std::wstring::npos ? std::wstring::npos
                                                            : plus - start));
        if (token.empty() || !ParseHotkeyToken(token, parsed)) {
            return false;
        }
        if (plus == std::wstring::npos) {
            break;
        }
        start = plus + 1;
    }
    if (parsed.key == 0) {
        return false;
    }
    modifiers = parsed.modifiers;
    key = parsed.key;
    return true;
}

static void LoadExcludeList() {
    context.settings.excludeList.clear();
    int consecutiveEmpty = 0;
    for (int index = 0; index < kMaxExcludeEntries; ++index) {
        WindhawkUtils::StringSetting setting = WindhawkUtils::StringSetting::make(
            L"Application.exclude_list[%d]", index);
        const wchar_t* value = setting.get();
        const bool empty = value == nullptr || *value == L'\0';
        consecutiveEmpty = empty ? consecutiveEmpty + 1 : 0;
        if (consecutiveEmpty >= 2) {
            break;
        }
        if (empty) {
            continue;
        }
        const std::wstring lower = ToLowerText(TrimText(value));
        if (!lower.empty() && lower[0] != L'#') {
            context.settings.excludeList.push_back(lower);
        }
    }
}

static void LoadSettings() {
    CanvasSettings& settings = context.settings;

    WindhawkUtils::StringSetting language =
        WindhawkUtils::StringSetting::make(L"Behavior.language");
    settings.language = ResolveLanguageSetting(language.get());

    WindhawkUtils::StringSetting hotkey =
        WindhawkUtils::StringSetting::make(L"Behavior.toggle_hotkey");
    const wchar_t* hotkeyText = hotkey.get();
    if (hotkeyText != nullptr) {
        settings.toggleHotkey = TrimText(hotkeyText);
    }

    settings.openOnStart =
        yoko::GetClampedIntSetting(L"Behavior.open_on_start", 0, 1) != 0;
    settings.animationMs =
        yoko::GetClampedIntSetting(L"Behavior.animation_ms", kMinAnimationMs,
                                   kMaxAnimationMs);
    settings.rescanIntervalMs = yoko::GetClampedIntSetting(
        L"Behavior.rescan_interval_ms", kMinRescanIntervalMs,
        kMaxRescanIntervalMs);

    settings.showGrid =
        yoko::GetClampedIntSetting(L"Canvas.show_grid", 0, 1) != 0;
    settings.gridSpacing =
        yoko::GetClampedIntSetting(L"Canvas.grid_spacing", 16, 512);
    settings.gap = yoko::GetClampedIntSetting(L"Canvas.gap", 0, 256);
    settings.snapDistance =
        yoko::GetClampedIntSetting(L"Canvas.snap_distance", 0, 48);
    settings.includeOtherDesktops =
        yoko::GetClampedIntSetting(L"Canvas.include_other_desktops", 0, 1) != 0;
    settings.saveLayout =
        yoko::GetClampedIntSetting(L"Canvas.save_layout", 0, 1) != 0;

    settings.minZoomPercent = yoko::GetClampedIntSetting(
        L"Appearance.min_zoom_percent", kMinZoomPercent, kMaxZoomPercent - 5);
    settings.maxZoomPercent = yoko::GetClampedIntSetting(
        L"Appearance.max_zoom_percent", settings.minZoomPercent + 5,
        kMaxZoomPercent);

    settings.minimapEnabled =
        yoko::GetClampedIntSetting(L"Appearance.minimap_enabled", 0, 1) != 0;
    settings.minimapSize = yoko::GetClampedIntSetting(
        L"Appearance.minimap_size", kMinimapMinSize, kMinimapMaxSize);

    settings.backgroundColor =
        ReadColorSetting(L"Appearance.background_color", 0x0B0E14);
    settings.gridColor = ReadColorSetting(L"Appearance.grid_color", 0x1B2436);
    settings.accentColor = ReadColorSetting(L"Appearance.accent_color", 0x38BDF8);
    settings.textColor = ReadColorSetting(L"Appearance.text_color", 0xE2E8F0);
    settings.mutedTextColor =
        ReadColorSetting(L"Appearance.muted_text_color", 0x64748B);
    settings.cardFillColor =
        ReadColorSetting(L"Appearance.card_fill_color", 0x111827);

    settings.stacksEnabled =
        yoko::GetClampedIntSetting(L"Canvas.card_stacks_enabled", 0, 1) != 0;
    settings.stackStepDegrees =
        static_cast<float>(yoko::GetClampedIntSetting(
            L"Canvas.card_stack_step_degrees", 0, 900)) / 10.0f;
    settings.stackMaxDegrees =
        static_cast<float>(yoko::GetClampedIntSetting(
            L"Canvas.card_stack_max_degrees", 0, 900)) / 10.0f;
    settings.stackScaleFactor =
        static_cast<float>(yoko::GetClampedIntSetting(
            L"Canvas.card_stack_scale_percent", 800, 1000)) / 1000.0f;
    settings.stackMaxDepth =
        yoko::GetClampedIntSetting(L"Canvas.card_stack_max_depth", 0, kMaxStackDepth);
    settings.stackRotationDirection = RotationDirectionFromText(
        yoko::GetStringSetting(L"Canvas.card_stack_rotation_direction",
                               L"counter_clockwise"));

    WindhawkUtils::StringSetting initialLayout =
        WindhawkUtils::StringSetting::make(L"Canvas.initial_layout");
    const wchar_t* layoutText = initialLayout.get();
    settings.initialLayout =
        layoutText != nullptr && _wcsicmp(layoutText, L"desktop") == 0
            ? InitialLayout::Desktop
            : InitialLayout::Packed;

    LoadExcludeList();

    context.camera.targetZoom = ClampZoom(context.camera.targetZoom);
    context.camera.zoom = ClampZoom(context.camera.zoom);
}

static std::wstring LayoutDirectory() {
    WCHAR path[MAX_PATH] = {};
    const size_t length = Wh_GetModStoragePath(path, ARRAYSIZE(path));
    if (length == 0) {
        return std::wstring();
    }
    return std::wstring(path, length);
}

static bool CurrentUserSid(std::wstring& sid) {
    HANDLE tokenHandle = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle)) {
        return false;
    }
    yoko::win32::UniqueHandle token(tokenHandle);
    DWORD length = 0;
    const bool queryFailed = !GetTokenInformation(token.get(), TokenUser,
                                                  nullptr, 0, &length);
    if (queryFailed && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        return false;
    }
    std::vector<BYTE> tokenData(length);
    if (!GetTokenInformation(token.get(), TokenUser, tokenData.data(), length,
                             &length)) {
        return false;
    }
    const auto* user = reinterpret_cast<const TOKEN_USER*>(tokenData.data());
    LPWSTR sidText = nullptr;
    if (!ConvertSidToStringSidW(user->User.Sid, &sidText)) {
        return false;
    }
    sid = sidText;
    LocalFree(sidText);
    return true;
}

static std::wstring LayoutFileName() {
    std::wstring sid;
    if (!CurrentUserSid(sid)) {
        Wh_Log(L"Aleph: cannot resolve the current user SID");
        return L"layout.json";
    }
    return L"layout-" + sid + L".json";
}

static std::wstring LayoutFilePath() {
    const std::wstring directory = LayoutDirectory();
    if (directory.empty()) {
        return std::wstring();
    }
    return directory + L"\\" + LayoutFileName();
}

// Crash-safe JSON persistence for card positions.

#include <cerrno>
#include <limits>
#include <locale.h>
#include <shlobj.h>

static std::wstring EscapeJsonText(const std::wstring& value) {
    std::wstring result;
    for (const wchar_t character : value) {
        if (character == L'"' || character == L'\\') {
            result.push_back(L'\\');
            result.push_back(character);
            continue;
        }
        if (character == L'\n') {
            result += L"\\n";
            continue;
        }
        if (character < 0x20) {
            static constexpr wchar_t hexDigits[] = L"0123456789abcdef";
            result += L"\\u00";
            result.push_back(hexDigits[(character >> 4) & 0xF]);
            result.push_back(hexDigits[character & 0xF]);
            continue;
        }
        result.push_back(character);
    }
    return result;
}

static bool WriteAll(HANDLE file, const std::string& data) {
    size_t offset = 0;
    while (offset < data.size()) {
        const DWORD chunk = static_cast<DWORD>(
            std::min<size_t>(data.size() - offset, 1u << 20));
        DWORD written = 0;
        if (!WriteFile(file, data.data() + offset, chunk, &written, nullptr) ||
            written == 0) {
            return false;
        }
        offset += written;
    }
    return true;
}

static bool ReadWholeFile(const std::wstring& path, std::string& out) {
    yoko::win32::UniqueHandle file(CreateFileW(
        path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file.valid()) {
        return false;
    }
    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(file.get(), &size) || size.QuadPart <= 0 ||
        size.QuadPart > (16 << 20)) {
        return false;
    }
    out.assign(static_cast<size_t>(size.QuadPart), '\0');
    size_t offset = 0;
    while (offset < out.size()) {
        const DWORD chunk = static_cast<DWORD>(
            std::min<size_t>(out.size() - offset, 1u << 20));
        DWORD read = 0;
        if (!ReadFile(file.get(), out.data() + offset, chunk, &read, nullptr) ||
            read == 0) {
            return false;
        }
        offset += read;
    }
    return true;
}

static bool IsLayoutWorldValid(const RectF& world) {
    if (!std::isfinite(world.left) || !std::isfinite(world.top) ||
        !std::isfinite(world.right) || !std::isfinite(world.bottom)) {
        return false;
    }
    return std::isfinite(world.width()) && std::isfinite(world.height()) &&
           world.width() > 0.0f && world.height() > 0.0f;
}

static bool FormatLayoutNumbers(const RectF& world, std::string& output) {
    if (!IsLayoutWorldValid(world)) {
        return false;
    }
    _locale_t locale = _create_locale(LC_NUMERIC, "C");
    if (locale == nullptr) {
        return false;
    }
    char numbers[192] = {};
    const int written = _snprintf_l(
        numbers, sizeof(numbers), "\"x\":%.9g,\"y\":%.9g,\"w\":%.9g,\"h\":%.9g",
        locale, static_cast<double>(world.left),
        static_cast<double>(world.top), static_cast<double>(world.width()),
        static_cast<double>(world.height()));
    _free_locale(locale);
    if (written < 0 || static_cast<size_t>(written) >= sizeof(numbers)) {
        return false;
    }
    output.assign(numbers, static_cast<size_t>(written));
    return true;
}

static std::string SerializeLayout(const LayoutSnapshot& layout) {
    std::string output = "{\"version\":1,\"entries\":[";
    bool first = true;
    for (const LayoutEntry& entry : layout.entries) {
        if (entry.fingerprint.empty()) {
            continue;
        }
        std::string numbers;
        if (!FormatLayoutNumbers(entry.world, numbers)) {
            return std::string();
        }
        if (!first) {
            output += ",";
        }
        first = false;
        output += "{\"key\":\"";
        output += ToUtf8(EscapeJsonText(entry.fingerprint));
        output += "\",";
        output += numbers;
        if (entry.pinned) {
            output += ",\"pin\":1";
        }
        if (!entry.grouped) {
            output += ",\"is_grouped\":0";
        }
        if (!entry.stackId.empty()) {
            output += ",\"stack\":\"";
            output += ToUtf8(EscapeJsonText(entry.stackId));
            output += "\"";
        }
        output += "}";
    }
    output += "]}\n";
    return output;
}

static bool IsJsonDigit(char character) {
    return character >= '0' && character <= '9';
}

static constexpr int kMaxJsonDepth = 64;

static void SkipJsonSpace(const std::string& text, size_t& index) {
    while (index < text.size() && (text[index] == ' ' || text[index] == '\t' ||
                                   text[index] == '\n' || text[index] == '\r')) {
        ++index;
    }
}

static bool ReadJsonHexQuad(const std::string& text, size_t& index,
                            uint32_t& value) {
    if (index + 4 > text.size()) {
        return false;
    }
    value = 0;
    for (int digitIndex = 0; digitIndex < 4; ++digitIndex) {
        const char character = text[index];
        uint32_t digit = 0;
        if (character >= '0' && character <= '9') {
            digit = static_cast<uint32_t>(character - '0');
        } else if (character >= 'a' && character <= 'f') {
            digit = static_cast<uint32_t>(character - 'a' + 10);
        } else if (character >= 'A' && character <= 'F') {
            digit = static_cast<uint32_t>(character - 'A' + 10);
        } else {
            return false;
        }
        value = value * 16 + digit;
        ++index;
    }
    return true;
}

static bool AppendJsonCodePoint(std::string& output, uint32_t codePoint) {
    if (codePoint <= 0x7F) {
        output.push_back(static_cast<char>(codePoint));
        return true;
    }
    if (codePoint <= 0x7FF) {
        output.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
        output.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        return true;
    }
    if (codePoint >= 0xD800 && codePoint <= 0xDFFF) {
        return false;
    }
    if (codePoint <= 0xFFFF) {
        output.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
        output.push_back(
            static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
        output.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        return true;
    }
    if (codePoint > 0x10FFFF) {
        return false;
    }
    output.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
    output.push_back(
        static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
    output.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
    output.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    return true;
}

static bool ReadJsonUnicodeEscape(const std::string& text, size_t& index,
                                  std::string& output) {
    uint32_t codePoint = 0;
    if (!ReadJsonHexQuad(text, index, codePoint)) {
        return false;
    }
    if (codePoint >= 0xD800 && codePoint <= 0xDBFF) {
        if (index + 2 > text.size() || text[index] != '\\' ||
            text[index + 1] != 'u') {
            return false;
        }
        index += 2;
        uint32_t lowSurrogate = 0;
        if (!ReadJsonHexQuad(text, index, lowSurrogate)) {
            return false;
        }
        if (lowSurrogate < 0xDC00 || lowSurrogate > 0xDFFF) {
            return false;
        }
        codePoint = 0x10000 + ((codePoint - 0xD800) << 10) +
                    (lowSurrogate - 0xDC00);
    } else if (codePoint >= 0xDC00 && codePoint <= 0xDFFF) {
        return false;
    }
    return AppendJsonCodePoint(output, codePoint);
}

static bool ReadJsonEscape(const std::string& text, size_t& index,
                           std::string& output) {
    if (index >= text.size()) {
        return false;
    }
    const char escape = text[index];
    ++index;
    switch (escape) {
        case '"':
            output.push_back('"');
            return true;
        case '\\':
            output.push_back('\\');
            return true;
        case '/':
            output.push_back('/');
            return true;
        case 'b':
            output.push_back('\b');
            return true;
        case 'f':
            output.push_back('\f');
            return true;
        case 'n':
            output.push_back('\n');
            return true;
        case 'r':
            output.push_back('\r');
            return true;
        case 't':
            output.push_back('\t');
            return true;
        case 'u':
            return ReadJsonUnicodeEscape(text, index, output);
        default:
            return false;
    }
}

static bool ReadJsonString(const std::string& text, size_t& index,
                           std::string& output) {
    if (index >= text.size() || text[index] != '"') {
        return false;
    }
    ++index;
    output.clear();
    while (index < text.size() && text[index] != '"') {
        const char character = text[index];
        const bool escaped = character == '\\';
        if (escaped) {
            ++index;
        }
        if (escaped && !ReadJsonEscape(text, index, output)) {
            return false;
        }
        if (escaped) {
            continue;
        }
        ++index;
        if (static_cast<unsigned char>(character) < 0x20) {
            return false;
        }
        output.push_back(character);
    }
    if (index >= text.size()) {
        return false;
    }
    ++index;
    return true;
}

static bool ReadJsonDigits(const std::string& text, size_t& index) {
    const size_t start = index;
    while (index < text.size() && IsJsonDigit(text[index])) {
        ++index;
    }
    return index > start;
}

static bool ReadJsonInteger(const std::string& text, size_t& index) {
    if (index < text.size() && text[index] == '-') {
        ++index;
    }
    if (index >= text.size()) {
        return false;
    }
    if (text[index] != '0') {
        return ReadJsonDigits(text, index);
    }
    ++index;
    return true;
}

static bool ReadJsonFraction(const std::string& text, size_t& index) {
    if (index < text.size() && text[index] == '.') {
        ++index;
        return ReadJsonDigits(text, index);
    }
    return true;
}

static bool ReadJsonExponent(const std::string& text, size_t& index) {
    if (index >= text.size() ||
        (text[index] != 'e' && text[index] != 'E')) {
        return true;
    }
    ++index;
    if (index < text.size() &&
        (text[index] == '+' || text[index] == '-')) {
        ++index;
    }
    return ReadJsonDigits(text, index);
}

static bool ReadJsonNumber(const std::string& text, size_t& index,
                           double& value) {
    const size_t start = index;
    if (!ReadJsonInteger(text, index) || !ReadJsonFraction(text, index) ||
        !ReadJsonExponent(text, index)) {
        return false;
    }
    _locale_t locale = _create_locale(LC_NUMERIC, "C");
    if (locale == nullptr) {
        return false;
    }
    errno = 0;
    char* end = nullptr;
    const double parsed = _strtod_l(text.c_str() + start, &end, locale);
    const bool valid = end == text.c_str() + index && errno != ERANGE &&
                       std::isfinite(parsed);
    _free_locale(locale);
    if (!valid) {
        return false;
    }
    value = parsed;
    return true;
}

static bool ConsumeJsonLiteral(const std::string& text, size_t& index,
                               const char* literal) {
    for (size_t offset = 0; literal[offset] != '\0'; ++offset) {
        if (index >= text.size() || text[index] != literal[offset]) {
            return false;
        }
        ++index;
    }
    return true;
}

static bool SkipJsonValue(const std::string& text, size_t& index, int depth);

template <typename FieldReader>
static bool ReadJsonObject(const std::string& text, size_t& index,
                           FieldReader readField) {
    SkipJsonSpace(text, index);
    if (index >= text.size() || text[index] != '{') {
        return false;
    }
    ++index;
    SkipJsonSpace(text, index);
    if (index < text.size() && text[index] == '}') {
        ++index;
        return true;
    }
    while (true) {
        std::string key;
        if (!ReadJsonString(text, index, key)) {
            return false;
        }
        SkipJsonSpace(text, index);
        if (index >= text.size() || text[index] != ':') {
            return false;
        }
        ++index;
        SkipJsonSpace(text, index);
        if (!readField(key)) {
            return false;
        }
        SkipJsonSpace(text, index);
        if (index >= text.size()) {
            return false;
        }
        const char delimiter = text[index];
        ++index;
        if (delimiter == '}') {
            return true;
        }
        if (delimiter != ',') {
            return false;
        }
        SkipJsonSpace(text, index);
    }
}

static bool SkipJsonObject(const std::string& text, size_t& index, int depth) {
    if (depth > kMaxJsonDepth) {
        return false;
    }
    return ReadJsonObject(text, index, [&](const std::string&) {
        return SkipJsonValue(text, index, depth + 1);
    });
}

static bool SkipJsonArray(const std::string& text, size_t& index, int depth) {
    if (depth > kMaxJsonDepth || index >= text.size() || text[index] != '[') {
        return false;
    }
    ++index;
    SkipJsonSpace(text, index);
    if (index < text.size() && text[index] == ']') {
        ++index;
        return true;
    }
    while (true) {
        if (!SkipJsonValue(text, index, depth + 1)) {
            return false;
        }
        SkipJsonSpace(text, index);
        if (index >= text.size()) {
            return false;
        }
        const char delimiter = text[index];
        ++index;
        if (delimiter == ']') {
            return true;
        }
        if (delimiter != ',') {
            return false;
        }
        SkipJsonSpace(text, index);
    }
}

static bool SkipJsonValue(const std::string& text, size_t& index, int depth) {
    if (depth > kMaxJsonDepth) {
        return false;
    }
    SkipJsonSpace(text, index);
    if (index >= text.size()) {
        return false;
    }
    const char character = text[index];
    if (character == '"') {
        std::string ignored;
        return ReadJsonString(text, index, ignored);
    }
    if (character == '{') {
        return SkipJsonObject(text, index, depth);
    }
    if (character == '[') {
        return SkipJsonArray(text, index, depth);
    }
    if (character == 't') {
        return ConsumeJsonLiteral(text, index, "true");
    }
    if (character == 'f') {
        return ConsumeJsonLiteral(text, index, "false");
    }
    if (character == 'n') {
        return ConsumeJsonLiteral(text, index, "null");
    }
    double ignored = 0.0;
    return ReadJsonNumber(text, index, ignored);
}

struct JsonEntryFields {
    std::wstring fingerprint;
    std::wstring stackId;
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;
    double pin = 0.0;
    double grouped = 1.0;
    bool hasFingerprint = false;
    bool hasStack = false;
    bool hasX = false;
    bool hasY = false;
    bool hasWidth = false;
    bool hasHeight = false;
    bool hasPin = false;
    bool hasGrouped = false;
};

struct JsonEntryCursor {
    const std::string& text;
    size_t& index;
    JsonEntryFields& fields;
};

static bool ReadJsonEntryNumber(JsonEntryCursor& cursor, bool& assigned,
                                double& value) {
    if (assigned || !ReadJsonNumber(cursor.text, cursor.index, value)) {
        return false;
    }
    assigned = true;
    return true;
}

static bool ReadJsonEntryField(JsonEntryCursor& cursor,
                               const std::string& key) {
    if (key == "key") {
        if (cursor.fields.hasFingerprint) {
            return false;
        }
        std::string encoded;
        if (!ReadJsonString(cursor.text, cursor.index, encoded)) {
            return false;
        }
        cursor.fields.fingerprint = FromUtf8(encoded);
        if (cursor.fields.fingerprint.empty()) {
            return false;
        }
        cursor.fields.hasFingerprint = true;
        return true;
    }
    if (key == "x") {
        return ReadJsonEntryNumber(cursor, cursor.fields.hasX,
                                   cursor.fields.x);
    }
    if (key == "y") {
        return ReadJsonEntryNumber(cursor, cursor.fields.hasY,
                                   cursor.fields.y);
    }
    if (key == "w") {
        return ReadJsonEntryNumber(cursor, cursor.fields.hasWidth,
                                   cursor.fields.width);
    }
    if (key == "h") {
        return ReadJsonEntryNumber(cursor, cursor.fields.hasHeight,
                                   cursor.fields.height);
    }
    if (key == "pin") {
        return ReadJsonEntryNumber(cursor, cursor.fields.hasPin,
                                   cursor.fields.pin);
    }
    if (key == "is_grouped") {
        return ReadJsonEntryNumber(cursor, cursor.fields.hasGrouped,
                                   cursor.fields.grouped);
    }
    if (key == "stack") {
        if (cursor.fields.hasStack) {
            return false;
        }
        std::string encoded;
        if (!ReadJsonString(cursor.text, cursor.index, encoded)) {
            return false;
        }
        cursor.fields.stackId = FromUtf8(encoded);
        if (cursor.fields.stackId.empty()) {
            return false;
        }
        cursor.fields.hasStack = true;
        return true;
    }
    return SkipJsonValue(cursor.text, cursor.index, 2);
}

static bool IsLayoutNumberInRange(double value) {
    constexpr double maximum = std::numeric_limits<float>::max();
    return std::isfinite(value) && value >= -maximum && value <= maximum;
}

static bool BuildLayoutEntry(const JsonEntryFields& fields, LayoutEntry& entry) {
    if (!fields.hasFingerprint || !fields.hasX || !fields.hasY ||
        !fields.hasWidth || !fields.hasHeight) {
        return false;
    }
    if (!IsLayoutNumberInRange(fields.x) ||
        !IsLayoutNumberInRange(fields.y) ||
        !IsLayoutNumberInRange(fields.width) ||
        !IsLayoutNumberInRange(fields.height)) {
        return false;
    }
    if (fields.hasPin && fields.pin != 0.0 && fields.pin != 1.0) {
        return false;
    }
    if (fields.hasGrouped && fields.grouped != 0.0 && fields.grouped != 1.0) {
        return false;
    }
    const float left = static_cast<float>(fields.x);
    const float top = static_cast<float>(fields.y);
    const float width = static_cast<float>(fields.width);
    const float height = static_cast<float>(fields.height);
    const float right = left + width;
    const float bottom = top + height;
    const RectF world{left, top, right, bottom};
    if (!IsLayoutWorldValid(world)) {
        return false;
    }
    entry.fingerprint = fields.fingerprint;
    entry.world = world;
    entry.pinned = fields.hasPin && fields.pin == 1.0;
    entry.grouped = !fields.hasGrouped || fields.grouped == 1.0;
    entry.stackId = fields.hasStack ? fields.stackId : std::wstring();
    return true;
}

static bool ReadJsonEntryObject(const std::string& text, size_t& index,
                                LayoutEntry& entry) {
    JsonEntryFields fields;
    JsonEntryCursor cursor{text, index, fields};
    const bool parsed = ReadJsonObject(text, index, [&](const std::string& key) {
        return ReadJsonEntryField(cursor, key);
    });
    if (!parsed) {
        return false;
    }
    return BuildLayoutEntry(fields, entry);
}

static bool ReadJsonEntries(const std::string& text, size_t& index,
                            LayoutSnapshot& layout) {
    SkipJsonSpace(text, index);
    if (index >= text.size() || text[index] != '[') {
        return false;
    }
    ++index;
    SkipJsonSpace(text, index);
    if (index < text.size() && text[index] == ']') {
        ++index;
        return true;
    }
    while (true) {
        LayoutEntry entry;
        if (!ReadJsonEntryObject(text, index, entry)) {
            return false;
        }
        if (layout.entries.size() <
            static_cast<size_t>(kMaxLayoutEntries)) {
            layout.entries.push_back(std::move(entry));
        }
        SkipJsonSpace(text, index);
        if (index >= text.size()) {
            return false;
        }
        const char delimiter = text[index];
        ++index;
        if (delimiter == ']') {
            return true;
        }
        if (delimiter != ',') {
            return false;
        }
        SkipJsonSpace(text, index);
    }
}

struct JsonRootFields {
    LayoutSnapshot& layout;
    bool hasVersion = false;
    bool hasEntries = false;
    double version = 0.0;
};

struct JsonRootCursor {
    const std::string& text;
    size_t& index;
    JsonRootFields& fields;
};

static bool ReadJsonRootField(JsonRootCursor& cursor,
                              const std::string& key) {
    if (key == "version") {
        if (cursor.fields.hasVersion ||
            !ReadJsonNumber(cursor.text, cursor.index,
                            cursor.fields.version)) {
            return false;
        }
        cursor.fields.hasVersion = true;
        return true;
    }
    if (key == "entries") {
        if (cursor.fields.hasEntries ||
            !ReadJsonEntries(cursor.text, cursor.index,
                             cursor.fields.layout)) {
            return false;
        }
        cursor.fields.hasEntries = true;
        return true;
    }
    return SkipJsonValue(cursor.text, cursor.index, 1);
}

static bool ParseLayoutDocument(const std::string& text,
                                LayoutSnapshot& layout) {
    if (FromUtf8(text).empty()) {
        return false;
    }
    size_t index = 0;
    JsonRootFields fields{layout};
    JsonRootCursor cursor{text, index, fields};
    const bool parsed = ReadJsonObject(text, index, [&](const std::string& key) {
        return ReadJsonRootField(cursor, key);
    });
    if (!parsed) {
        return false;
    }
    SkipJsonSpace(text, index);
    return index == text.size() && fields.hasVersion &&
           fields.hasEntries && fields.version == 1.0;
}

static LayoutSnapshot ParseLayout(const std::string& text) {
    LayoutSnapshot layout;
    if (!ParseLayoutDocument(text, layout)) {
        return LayoutSnapshot{};
    }
    return layout;
}

static void LoadLayout() {
    context.layout.entries.clear();
    if (!context.settings.saveLayout) {
        return;
    }
    const std::wstring path = LayoutFilePath();
    if (path.empty()) {
        return;
    }
    std::string text;
    if (!ReadWholeFile(path, text)) {
        return;
    }
    context.layout = ParseLayout(text);
}

static bool EnsureLayoutDirectory(const std::wstring& directory) {
    if (directory.empty()) {
        return false;
    }
    const int result =
        SHCreateDirectoryExW(nullptr, directory.c_str(), nullptr);
    if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS) {
        SetLastError(static_cast<DWORD>(result));
        return false;
    }
    const DWORD attributes = GetFileAttributesW(directory.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        SetLastError(ERROR_DIRECTORY);
        return false;
    }
    return true;
}

static bool SaveLayoutNow() {
    if (!context.settings.saveLayout) {
        return false;
    }
    const std::wstring path = LayoutFilePath();
    if (path.empty()) {
        return false;
    }
    const std::wstring directory = LayoutDirectory();
    if (directory.empty()) {
        return false;
    }
    if (!EnsureLayoutDirectory(directory)) {
        Wh_Log(L"Aleph: cannot create %s: %lu", directory.c_str(),
               GetLastError());
        return false;
    }

    const std::string data = SerializeLayout(context.layout);
    if (data.empty()) {
        return false;
    }
    const std::wstring temporaryPath = path + L".tmp";
    {
        yoko::win32::UniqueHandle file(CreateFileW(
            temporaryPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr));
        if (!file.valid()) {
            Wh_Log(L"Aleph: cannot create %s", temporaryPath.c_str());
            return false;
        }
        if (!WriteAll(file.get(), data) || !FlushFileBuffers(file.get())) {
            Wh_Log(L"Aleph: cannot write %s", temporaryPath.c_str());
            return false;
        }
    }
    if (!MoveFileExW(temporaryPath.c_str(), path.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"Aleph: cannot replace %s: %lu", path.c_str(), GetLastError());
        return false;
    }
    return true;
}

static void MarkLayoutDirty(DWORD now) {
    if (!context.settings.saveLayout) {
        return;
    }
    context.layoutDirty = true;
    context.layoutDirtyTick = now;
}

static void FlushLayoutIfDue(DWORD now) {
    if (!context.layoutDirty) {
        return;
    }
    if (!context.settings.saveLayout) {
        context.layoutDirty = false;
        return;
    }
    if (now - context.layoutDirtyTick < kLayoutSaveDelayMs) {
        return;
    }
    if (SaveLayoutNow()) {
        context.layoutDirty = false;
        return;
    }
    context.layoutDirtyTick = now;
}

// Cross-version top-level window enumeration and filtering.



// Shared geometry helpers for scan candidate selection and card packing.


inline float CardArea(const ScannedWindow& window) {
    return static_cast<float>(window.framePx.right - window.framePx.left) *
           static_cast<float>(window.framePx.bottom - window.framePx.top);
}

#include <propkey.h>

static std::wstring ReadApplicationKey(HWND window, const std::wstring& exeName) {
    IPropertyStore* store = nullptr;
    if (FAILED(SHGetPropertyStoreForWindow(window, IID_PPV_ARGS(&store)))) {
        return exeName;
    }
    PROPVARIANT value{};
    const HRESULT result = store->GetValue(PKEY_AppUserModel_ID, &value);
    store->Release();
    std::wstring key;
    if (SUCCEEDED(result) && value.vt == VT_LPWSTR &&
        value.pwszVal != nullptr && value.pwszVal[0] != L'\0') {
        key = L"aumid:" + ToLowerText(value.pwszVal);
    }
    const HRESULT clearResult = PropVariantClear(&value);
    if (FAILED(clearResult)) {
        Wh_Log(L"Aleph: PropVariantClear failed: 0x%08X",
               static_cast<unsigned>(clearResult));
    }
    return key.empty() ? exeName : key;
}

static bool IsOwnProcessWindow(HWND window) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    return pid != 0 && pid == GetCurrentProcessId();
}

static std::wstring ReadWindowClass(HWND window) {
    WCHAR buffer[256] = {};
    const int length =
        GetClassNameW(window, buffer, static_cast<int>(ARRAYSIZE(buffer)));
    if (length <= 0) {
        return std::wstring();
    }
    return ToLowerText(std::wstring(buffer, static_cast<size_t>(length)));
}

static std::wstring ReadWindowText(HWND window) {
    const int length = GetWindowTextLengthW(window);
    if (length <= 0 || length > 1024) {
        return std::wstring();
    }
    std::wstring text(static_cast<size_t>(length) + 1, L'\0');
    const int copied =
        GetWindowTextW(window, text.data(), static_cast<int>(text.size()));
    if (copied <= 0) {
        return std::wstring();
    }
    text.resize(static_cast<size_t>(copied));
    return text;
}

static std::wstring ReadProcessImagePath(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (process == nullptr) {
        return std::wstring();
    }
    WCHAR buffer[MAX_PATH * 2] = {};
    DWORD size = static_cast<DWORD>(ARRAYSIZE(buffer));
    const BOOL success = QueryFullProcessImageNameW(process, 0, buffer, &size);
    CloseHandle(process);
    if (!success || size == 0) {
        return std::wstring();
    }
    return std::wstring(buffer, size);
}

static bool IsShellWindowClass(const std::wstring& windowClass) {
    static const wchar_t* const kShellClasses[] = {
        L"progman",
        L"workerw",
        L"shell_traywnd",
        L"shell_secondarytraywnd",
        L"traynotifywnd",
        L"windows.ui.core.corewindow",
        L"startmenuwindow",
        L"searchflyout",
        L"xamlexplorerhostislandwindow",
        L"windows.ui.composition.desktopwindowcontentbridge",
    };
    for (const wchar_t* name : kShellClasses) {
        if (windowClass == name) {
            return true;
        }
    }
    return false;
}

static bool ReadCloaked(HWND window, DWORD& cloaked) {
    cloaked = 0;
    const HRESULT result = DwmGetWindowAttribute(
        window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    return SUCCEEDED(result);
}

// DWMWA_EXTENDED_FRAME_BOUNDS excludes the invisible resize border, so cards
// line up with what the user sees instead of leaving a transparent gap.
static bool ReadFrameBounds(HWND window, RECT& bounds) {
    RECT extended = {};
    if (SUCCEEDED(DwmGetWindowAttribute(
            window, DWMWA_EXTENDED_FRAME_BOUNDS, &extended, sizeof(extended))) &&
        extended.right > extended.left && extended.bottom > extended.top) {
        bounds = extended;
        return true;
    }
    RECT fallback = {};
    if (!GetWindowRect(window, &fallback)) {
        return false;
    }
    bounds = fallback;
    return bounds.right > bounds.left && bounds.bottom > bounds.top;
}

static bool ReadRestoredBounds(HWND window, RECT& bounds) {
    WINDOWPLACEMENT placement = {};
    placement.length = sizeof(placement);
    if (!GetWindowPlacement(window, &placement)) {
        return false;
    }
    bounds = placement.rcNormalPosition;
    if (bounds.right <= bounds.left || bounds.bottom <= bounds.top) {
        return false;
    }
    if ((GetWindowLongPtrW(window, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) != 0) {
        return true;
    }
    MONITORINFO monitor = {};
    monitor.cbSize = sizeof(monitor);
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST),
                          &monitor)) {
        return false;
    }
    OffsetRect(&bounds, monitor.rcWork.left - monitor.rcMonitor.left,
               monitor.rcWork.top - monitor.rcMonitor.top);
    return true;
}

static bool IsOtherDesktop(HWND window, IVirtualDesktopManager* manager) {
    BOOL current = TRUE;
    return manager != nullptr &&
           SUCCEEDED(manager->IsWindowOnCurrentVirtualDesktop(window, &current)) &&
           current == FALSE;
}

static bool IsExcludedExecutable(const std::wstring& exeName) {
    for (const std::wstring& entry : context.settings.excludeList) {
        if (entry == exeName) {
            return true;
        }
    }
    return false;
}

// Returns false when the window must not appear on the canvas. A true result
// with CrossDesktop reports a window on another virtual desktop, which the
// caller may drop based on settings.
static bool ClassifyWindow(HWND window, IVirtualDesktopManager* desktopManager,
                           ScannedWindow& out) {
    if (window == nullptr || IsOwnProcessWindow(window)) {
        return false;
    }
    if (GetShellWindow() == window) {
        return false;
    }
    if (!IsWindowVisible(window)) {
        return false;
    }

    const LONG_PTR extendedStyle = GetWindowLongPtrW(window, GWL_EXSTYLE);
    const bool appWindow = (extendedStyle & WS_EX_APPWINDOW) != 0;
    if ((extendedStyle & WS_EX_TOOLWINDOW) != 0 && !appWindow) {
        return false;
    }
    if (GetWindow(window, GW_OWNER) != nullptr && !appWindow) {
        return false;
    }

    const std::wstring windowClass = ReadWindowClass(window);
    if (windowClass.empty() || IsShellWindowClass(windowClass)) {
        return false;
    }
    const std::wstring title = ReadWindowText(window);
    if (title.empty()) {
        return false;
    }

    DWORD cloaked = 0;
    if (!ReadCloaked(window, cloaked) ||
        (cloaked & ~static_cast<DWORD>(DWM_CLOAKED_SHELL)) != 0) {
        return false;
    }
    const bool crossDesktop = (cloaked & DWM_CLOAKED_SHELL) != 0;
    if (crossDesktop && !IsOtherDesktop(window, desktopManager)) {
        return false;
    }

    DWORD pid = 0;
    const DWORD threadId = GetWindowThreadProcessId(window, &pid);
    if (pid == 0 || pid == GetCurrentProcessId()) {
        return false;
    }
    const std::wstring exePath = ReadProcessImagePath(pid);
    const std::wstring exeName = ExecutableName(exePath);
    if (exeName.empty() || IsExcludedExecutable(exeName)) {
        return false;
    }

    const bool minimized = IsIconic(window) != FALSE;
    RECT frame = {};
    if (minimized) {
        if (!ReadRestoredBounds(window, frame)) {
            return false;
        }
    } else if (!ReadFrameBounds(window, frame)) {
        return false;
    }

    out.hwnd = window;
    out.processId = pid;
    out.threadId = threadId;
    out.windowClass = windowClass;
    out.state = crossDesktop ? WindowVisualState::OtherDesktop
                            : (minimized ? WindowVisualState::Minimized
                                         : WindowVisualState::Live);
    out.framePx = frame;
    out.title = title;
    out.exePath = exePath;
    out.fingerprint.clear();
    return true;
}

// Enumerates every eligible window. The virtual desktop manager is passed as
// a parameter so the callback never needs global COM state.
struct CollectPass {
    std::vector<ScannedWindow> windows;
    IVirtualDesktopManager* desktopManager = nullptr;

    static BOOL CALLBACK Thunk(HWND window, LPARAM parameter) {
        CollectPass* pass = reinterpret_cast<CollectPass*>(parameter);
        if (pass == nullptr || pass->windows.size() >=
                                  static_cast<size_t>(kMaxCards) * 4) {
            return TRUE;
        }
        ScannedWindow candidate;
        if (!ClassifyWindow(window, pass->desktopManager, candidate)) {
            return TRUE;
        }
        if (candidate.state == WindowVisualState::OtherDesktop &&
            !context.settings.includeOtherDesktops) {
            return TRUE;
        }
        pass->windows.push_back(std::move(candidate));
        return TRUE;
    }
};

static std::wstring ExistingFingerprint(const ScannedWindow& window) {
    for (const Card& card : context.cards) {
        if (MatchesWindow(card, window)) {
            return card.fingerprint;
        }
    }
    return {};
}

static bool FingerprintClaimed(const std::vector<ScannedWindow>& windows,
                               const std::wstring& fingerprint) {
    return std::any_of(windows.begin(), windows.end(),
                       [&fingerprint](const ScannedWindow& window) {
                           return window.fingerprint == fingerprint;
                       });
}

static bool MatchesExistingCard(const ScannedWindow& window) {
    return std::any_of(context.cards.begin(), context.cards.end(),
                       [&window](const Card& card) {
                           return MatchesWindow(card, window);
                       });
}

// Keep every window already represented on the canvas, then keep the largest
// new windows before the hard card limit. Existing order preserves Z-order;
// new candidates fall back to area and identity for deterministic selection.
inline void SelectScanCandidates(std::vector<ScannedWindow>& windows) {
    const size_t maximum = static_cast<size_t>(kMaxCards);
    if (windows.size() <= maximum) {
        return;
    }
    const auto existingEnd = std::stable_partition(
        windows.begin(), windows.end(), MatchesExistingCard);
    std::stable_sort(existingEnd, windows.end(),
                     [](const ScannedWindow& left, const ScannedWindow& right) {
                         const float leftArea = CardArea(left);
                         const float rightArea = CardArea(right);
                         if (leftArea != rightArea) {
                             return leftArea > rightArea;
                         }
                         if (left.exePath != right.exePath) {
                             return left.exePath < right.exePath;
                         }
                         if (left.windowClass != right.windowClass) {
                             return left.windowClass < right.windowClass;
                         }
                         if (left.processId != right.processId) {
                             return left.processId < right.processId;
                         }
                         return left.threadId < right.threadId;
                     });
    windows.resize(maximum);
}

static bool HasEarlierFingerprintKey(const ScannedWindow& left,
                                     const ScannedWindow& right) {
    if (left.exePath != right.exePath) {
        return left.exePath < right.exePath;
    }
    if (left.windowClass != right.windowClass) {
        return left.windowClass < right.windowClass;
    }
    if (left.title != right.title) {
        return left.title < right.title;
    }
    if (left.processId != right.processId) {
        return left.processId < right.processId;
    }
    if (left.threadId != right.threadId) {
        return left.threadId < right.threadId;
    }
    return left.hwnd < right.hwnd;
}

static void AssignFingerprints(std::vector<ScannedWindow>& windows) {
    for (ScannedWindow& window : windows) {
        window.fingerprint = ExistingFingerprint(window);
    }
    std::vector<ScannedWindow*> newWindows;
    newWindows.reserve(windows.size());
    for (ScannedWindow& window : windows) {
        if (window.fingerprint.empty()) {
            newWindows.push_back(&window);
        }
    }
    std::sort(newWindows.begin(), newWindows.end(),
              [](const ScannedWindow* left, const ScannedWindow* right) {
                  return HasEarlierFingerprintKey(*left, *right);
              });
    for (ScannedWindow* window : newWindows) {
        const std::wstring base =
            ExecutableName(window->exePath) + L"|" + window->windowClass + L"#";
        unsigned int ordinal = 0;
        while (FingerprintClaimed(windows, base + std::to_wstring(ordinal))) {
            ++ordinal;
        }
        window->fingerprint = base + std::to_wstring(ordinal);
    }
}

inline void PrepareScanCandidates(std::vector<ScannedWindow>& windows) {
    AssignFingerprints(windows);
    SelectScanCandidates(windows);
}

static std::vector<ScannedWindow> ScanWindows(IVirtualDesktopManager* manager) {
    CollectPass pass;
    pass.desktopManager = manager;
    EnumWindows(&CollectPass::Thunk, reinterpret_cast<LPARAM>(&pass));
    PrepareScanCandidates(pass.windows);
    // EnumWindows yields windows topmost first, so the vector index is
    // the desktop Z-order rank. Record it before MergeScan reorders
    // additions by area, or stacks would lose which window is on top.
    for (size_t index = 0; index < pass.windows.size(); ++index) {
        ScannedWindow& window = pass.windows[index];
        window.zOrder = index;
        if (context.settings.stacksEnabled) {
            window.applicationKey = ReadApplicationKey(window.hwnd,
                                                       ExecutableName(window.exePath));
        }
    }
    return std::move(pass.windows);
}

static const LayoutEntry* FindLayoutEntry(const std::wstring& fingerprint) {
    for (const LayoutEntry& entry : context.layout.entries) {
        if (entry.fingerprint == fingerprint) {
            return &entry;
        }
    }
    return nullptr;
}

static void RememberLayoutPin(const std::wstring& fingerprint,
                              const RectF& world, bool pinned, bool grouped);

static void RememberLayoutEntry(const std::wstring& fingerprint,
                                const RectF& world) {
    const LayoutEntry* existing = FindLayoutEntry(fingerprint);
    const bool pinned = existing != nullptr && existing->pinned;
    const bool grouped = existing == nullptr || existing->grouped;
    RememberLayoutPin(fingerprint, world, pinned, grouped);
}

static void MoveLayoutEntryToBack(size_t index) {
    LayoutEntry moved = std::move(context.layout.entries[index]);
    context.layout.entries.erase(
        context.layout.entries.begin() + static_cast<std::ptrdiff_t>(index));
    context.layout.entries.push_back(std::move(moved));
}

// The vector order is the recency order: index zero is the least recently
// touched entry and the back is the most recent one.
static bool EvictFirstUnpinnedLayoutEntry() {
    for (auto entry = context.layout.entries.begin();
         entry != context.layout.entries.end(); ++entry) {
        if (entry->pinned) {
            continue;
        }
        context.layout.entries.erase(entry);
        return true;
    }
    return false;
}

static void RememberLayoutPin(const std::wstring& fingerprint,
                              const RectF& world, bool pinned, bool grouped) {
    auto& entries = context.layout.entries;
    const auto existing = std::find_if(
        entries.begin(), entries.end(),
        [&fingerprint](const LayoutEntry& entry) {
            return entry.fingerprint == fingerprint;
        });
    if (existing != entries.end()) {
        existing->world = world;
        existing->pinned = pinned;
        existing->grouped = grouped;
        MoveLayoutEntryToBack(
            static_cast<size_t>(existing - entries.begin()));
        return;
    }
    if (entries.size() >= static_cast<size_t>(kMaxLayoutEntries) &&
        !EvictFirstUnpinnedLayoutEntry()) {
        return;
    }
    entries.push_back(LayoutEntry{fingerprint, world, pinned, grouped});
}

// The manual stack a card belongs to is stored in the same entry, so a
// hand-made group comes back after a restart instead of dissolving. The entry
// must exist already, which RememberLayoutCard guarantees by writing the
// geometry first.
static void RememberLayoutStackId(const std::wstring& fingerprint,
                                  const std::wstring& stackId) {
    const auto existing = std::find_if(
        context.layout.entries.begin(), context.layout.entries.end(),
        [&fingerprint](const LayoutEntry& entry) {
            return entry.fingerprint == fingerprint;
        });
    if (existing == context.layout.entries.end() ||
        existing->stackId == stackId) {
        return;
    }
    existing->stackId = stackId;
    MarkLayoutDirty(GetTickCount());
}

// Saves everything a card contributes to the layout: where it sits, whether it
// is pinned, whether it wants to stack, and which hand-made stack it joined.
static void RememberLayoutCard(const Card& card) {
    RememberLayoutPin(card.fingerprint, StackWorldRect(card), card.pinned,
                      card.stack.grouped);
    RememberLayoutStackId(card.fingerprint, card.stackId);
    MarkLayoutDirty(GetTickCount());
}

// Card creation, packing and snapping.
//
// Arranging cards never moves a real window. A card keeps its own position in
// world space, and dive-in solves for the camera state that projects the card
// back onto the window's real desktop rectangle. That keeps the user's desktop
// arrangement untouched and works for elevated windows the mod cannot move.
// The one deliberate move lives in interaction.h: activating an other-desktop
// card pulls that window onto the desktop that displays the canvas.



#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <string>
#include <utility>
#include <vector>

struct StackMember {
    size_t index;
    std::wstring groupKey;
};

inline Vec2 RotatePoint(Vec2 point, Vec2 center, float radians) {
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);
    const float x = point.x - center.x;
    const float y = point.y - center.y;
    return Vec2{center.x + x * cosine - y * sine,
                center.y + x * sine + y * cosine};
}

inline Vec2 CardLocalPoint(Vec2 screenPoint, const RectF& bounds,
                           const CardStackPlacement& placement) {
    const Vec2 center{bounds.centerX(), bounds.centerY()};
    const Vec2 unrotated = RotatePoint(screenPoint, center,
                                       -placement.rotationRadians);
    const float scale = placement.scale > 0.0f ? placement.scale : 1.0f;
    return Vec2{center.x + (unrotated.x - center.x) / scale,
                center.y + (unrotated.y - center.y) / scale};
}

inline std::array<Vec2, 4> CardScreenQuad(const RectF& bounds,
                                         const CardStackPlacement& placement) {
    const Vec2 center{bounds.centerX(), bounds.centerY()};
    const float scale = placement.scale > 0.0f ? placement.scale : 1.0f;
    const std::array<Vec2, 4> corners = {
        Vec2{bounds.left, bounds.top}, Vec2{bounds.right, bounds.top},
        Vec2{bounds.right, bounds.bottom}, Vec2{bounds.left, bounds.bottom}};
    std::array<Vec2, 4> quad{};
    for (size_t index = 0; index < corners.size(); ++index) {
        const Vec2 scaled{center.x + (corners[index].x - center.x) * scale,
                          center.y + (corners[index].y - center.y) * scale};
        quad[index] = RotatePoint(scaled, center, placement.rotationRadians);
    }
    return quad;
}

inline RectF CardQuadBounds(const std::array<Vec2, 4>& quad) {
    RectF bounds{quad[0].x, quad[0].y, quad[0].x, quad[0].y};
    for (const Vec2& point : quad) {
        bounds.left = std::min(bounds.left, point.x);
        bounds.top = std::min(bounds.top, point.y);
        bounds.right = std::max(bounds.right, point.x);
        bounds.bottom = std::max(bounds.bottom, point.y);
    }
    return bounds;
}

inline void SubtractRect(const RectF& source, const RectF& hole,
                         std::vector<RectF>& result) {
    if (!RectsOverlap(source, hole)) {
        result.push_back(source);
        return;
    }
    const RectF overlap{std::max(source.left, hole.left),
                        std::max(source.top, hole.top),
                        std::min(source.right, hole.right),
                        std::min(source.bottom, hole.bottom)};
    const RectF pieces[] = {
        {source.left, source.top, source.right, overlap.top},
        {source.left, overlap.bottom, source.right, source.bottom},
        {source.left, overlap.top, overlap.left, overlap.bottom},
        {overlap.right, overlap.top, source.right, overlap.bottom}};
    for (const RectF& piece : pieces) {
        if (!piece.empty()) {
            result.push_back(piece);
        }
    }
}

struct CardDrawClips {
    bool clipped = false;
    std::vector<RectF> regions;
};

inline std::vector<RectF> CardOcclusionHoles(
    const RectF& viewport, const RectF& cardBounds,
    const std::vector<RectF>& holes) {
    std::vector<RectF> relevantHoles;
    for (const RectF& hole : holes) {
        if (RectsOverlap(cardBounds, hole) && RectsOverlap(viewport, hole)) {
            const RectF clippedHole{std::max(viewport.left, hole.left),
                                    std::max(viewport.top, hole.top),
                                    std::min(viewport.right, hole.right),
                                    std::min(viewport.bottom, hole.bottom)};
            relevantHoles.push_back(clippedHole);
        }
    }
    return relevantHoles;
}

inline std::vector<RectF> SubtractCardOcclusionHoles(
    const RectF& viewport, const std::vector<RectF>& holes) {
    std::vector<RectF> regions{viewport};
    for (const RectF& hole : holes) {
        std::vector<RectF> remaining;
        for (const RectF& region : regions) {
            SubtractRect(region, hole, remaining);
        }
        regions = std::move(remaining);
        if (regions.empty()) {
            break;
        }
    }
    return regions;
}

inline RectF UnoccludedThumbnailRect(const RectF& destination,
                                     const std::vector<RectF>& higherThumbnails) {
    const auto occlusion = CardOcclusionHoles(destination, destination, higherThumbnails);
    if (occlusion.empty()) {
        return destination;
    }
    const auto visible = SubtractCardOcclusionHoles(destination, occlusion);
    RectF bounds{};
    double area = 0.0;
    for (const RectF& region : visible) {
        bounds = UnionRect(bounds, region);
        area += static_cast<double>(region.width()) * static_cast<double>(region.height());
    }
    return area == static_cast<double>(bounds.width()) * static_cast<double>(bounds.height())
               ? bounds : RectF{};
}

inline CardDrawClips CardDrawRegions(
    const RectF& viewport, const std::array<Vec2, 4>& cardQuad,
    const std::vector<RectF>& holes) {
    if (viewport.empty()) {
        return CardDrawClips{true, {}};
    }
    const std::vector<RectF> relevantHoles =
        CardOcclusionHoles(viewport, CardQuadBounds(cardQuad), holes);
    if (relevantHoles.empty()) {
        return CardDrawClips{};
    }
    return CardDrawClips{true, SubtractCardOcclusionHoles(viewport, relevantHoles)};
}

inline CardStackPlacement StackPlacementForSlot(size_t slot,
                                               const std::wstring& groupKey,
                                               const CanvasSettings& settings) {
    const auto fanDepth = static_cast<size_t>(
        std::clamp(settings.stackMaxDepth, 0, kMaxStackDepth));
    const float depth = static_cast<float>(slot);
    const float degrees = std::min(depth * settings.stackStepDegrees,
                                   settings.stackMaxDegrees);
    const float radians = degrees * (std::numbers::pi_v<float> / 180.0f) *
                          static_cast<float>(settings.stackRotationDirection);
    return CardStackPlacement{groupKey, slot, true, slot <= fanDepth, radians,
                              std::pow(settings.stackScaleFactor, depth)};
}

inline bool IsStackFront(const Card& card, const std::wstring& groupKey) {
    return card.stack.grouped && card.stack.slot == 0 &&
           card.stack.groupKey == groupKey;
}

inline void ResetStackPlacement(Card& card) {
    card.stack.groupKey.clear();
    card.stack.slot = 0;
    card.stack.rotationRadians = 0.0f;
    card.stack.scale = 1.0f;
    card.stack.opacity = 1.0f;
    card.stack.drawAboveStack = false;
    card.stack.visible = true;
    card.stack.frameWorld = RectF{};
}

inline void ClearAllStackPlacements(std::vector<Card>& cards) {
    for (Card& card : cards) {
        ResetStackPlacement(card);
    }
}

inline void ClearIneligibleStackPlacements(std::vector<Card>& cards) {
    for (Card& card : cards) {
        if (!card.stack.grouped || StackGroupKeyOf(card).empty()) {
            ResetStackPlacement(card);
        }
    }
}

inline std::vector<StackMember> CollectStackMembers(const std::vector<Card>& cards) {
    std::vector<StackMember> members;
    members.reserve(cards.size());
    for (size_t index = 0; index < cards.size(); ++index) {
        const Card& card = cards[index];
        const std::wstring stackKey = StackGroupKeyOf(card);
        if (card.stack.grouped && !stackKey.empty()) {
            members.push_back({index, stackKey});
        }
    }
    return members;
}

inline size_t StackFrontPosition(const std::vector<Card>& cards,
                                 const std::vector<StackMember>& members,
                                 const std::wstring& groupKey) {
    const auto previous = std::find_if(
        members.begin(), members.end(), [&cards, &groupKey](const StackMember& member) {
            return IsStackFront(cards[member.index], groupKey);
        });
    if (previous == members.end()) {
        return 0;
    }
    return static_cast<size_t>(previous - members.begin());
}

// A card that leaves a stack keeps its own window size and only takes the
// place the stack stood, so releasing a member never leaves it wearing the
// frame's proportions.
inline void RestoreOwnSizeCentredInFrame(Card& card) {
    if (!card.stack.grouped || card.stack.groupKey.empty() ||
        card.stack.frameWorld.empty() || card.world.empty()) {
        return;
    }
    const RectF& frame = card.stack.frameWorld;
    card.world = MakeRectF(frame.centerX() - card.world.width() * 0.5f,
                           frame.centerY() - card.world.height() * 0.5f,
                           card.world.width(), card.world.height());
}

// The shared rectangle the whole stack occupies. A formed stack keeps the
// frame it was given, so cycling the fan never resizes the card and every
// member shares one size and one centre; a new stack takes the front window's
// world rectangle. Keeping the frame instead of re-deriving it from the front
// window is what makes the fan read as one object: the front window can be
// resized, maximized or replaced without the card jumping in size.
inline RectF StackFrameForOrder(const std::vector<Card>& cards,
                                const std::vector<size_t>& order) {
    for (const size_t index : order) {
        if (index < cards.size() && !cards[index].stack.frameWorld.empty()) {
            return cards[index].stack.frameWorld;
        }
    }
    if (order.empty() || order.front() >= cards.size()) {
        return RectF{};
    }
    return cards[order.front()].world;
}

inline void ApplyStackOrder(std::vector<Card>& cards,
                            const std::vector<size_t>& order,
                            const CanvasSettings& settings) {
    const RectF frame = StackFrameForOrder(cards, order);
    for (size_t slot = 0; slot < order.size(); ++slot) {
        const size_t index = order[slot];
        if (index >= cards.size()) {
            continue;
        }
        const std::wstring groupKey = cards[index].stack.groupKey;
        cards[index].stack = StackPlacementForSlot(slot, groupKey, settings);
        cards[index].stack.frameWorld = frame;
    }
}

inline void AssignStack(std::vector<Card>& cards,
                        const std::vector<StackMember>& members,
                        const CanvasSettings& settings) {
    if (members.size() == 1) {
        Card& card = cards[members.front().index];
        RestoreOwnSizeCentredInFrame(card);
        ResetStackPlacement(card);
        return;
    }
    const std::wstring groupKey = members.front().groupKey;
    const size_t start = StackFrontPosition(cards, members, groupKey);
    std::vector<size_t> order;
    order.reserve(members.size());
    for (size_t slot = 0; slot < members.size(); ++slot) {
        const StackMember& member = members[(start + slot) % members.size()];
        cards[member.index].stack.groupKey = member.groupKey;
        cards[member.index].stack.grouped = true;
        order.push_back(member.index);
    }
    ApplyStackOrder(cards, order, settings);
}

inline void AssignStackGroups(std::vector<Card>& cards,
                              const std::vector<StackMember>& members,
                              const CanvasSettings& settings) {
    std::vector<StackMember> sorted = members;
    std::stable_sort(sorted.begin(), sorted.end(), [&cards](const StackMember& left,
                                                            const StackMember& right) {
        if (left.groupKey != right.groupKey) {
            return left.groupKey < right.groupKey;
        }
        // Within one app, members fan in desktop Z-order: the topmost /
        // focused window is first, so a newly formed stack hands slot 0
        // (0 degrees, scale 1, drawn on top) to that window. Fingerprint
        // only breaks ties, e.g. cards built without a scan rank.
        if (cards[left.index].zOrder != cards[right.index].zOrder) {
            return cards[left.index].zOrder < cards[right.index].zOrder;
        }
        return cards[left.index].fingerprint < cards[right.index].fingerprint;
    });
    for (auto group = sorted.begin(); group != sorted.end();) {
        const auto end = std::find_if(group + 1, sorted.end(), [&group](const StackMember& member) {
            return member.groupKey != group->groupKey;
        });
        AssignStack(cards, std::vector<StackMember>(group, end), settings);
        group = end;
    }
}

inline void ReconcileCardStacks(std::vector<Card>& cards, const CanvasSettings& settings) {
    if (!settings.stacksEnabled) {
        ClearAllStackPlacements(cards);
        return;
    }
    // Peer counts follow the same key that forms the stacks, so a hand-made
    // group also offers the header button that splits it again.
    std::unordered_map<std::wstring, size_t> stackPeerCounts;
    for (const Card& card : cards) {
        const std::wstring stackKey = StackGroupKeyOf(card);
        if (!stackKey.empty()) {
            ++stackPeerCounts[stackKey];
        }
    }
    for (Card& card : cards) {
        const auto count = stackPeerCounts.find(StackGroupKeyOf(card));
        card.hasStackPeers = count != stackPeerCounts.end() && count->second > 1;
    }
    ClearIneligibleStackPlacements(cards);
    const auto members = CollectStackMembers(cards);
    AssignStackGroups(cards, members, settings);
}

inline void CollectStackSlots(const std::vector<Card>& cards,
                              const std::wstring& groupKey,
                              std::vector<std::pair<size_t, size_t>>& slots) {
    slots.reserve(cards.size());
    for (size_t index = 0; index < cards.size(); ++index) {
        const CardStackPlacement& placement = cards[index].stack;
        if (placement.grouped && placement.groupKey == groupKey) {
            slots.emplace_back(placement.slot, index);
        }
    }
}

inline bool HasDuplicateSlot(const std::vector<std::pair<size_t, size_t>>& slots) {
    for (size_t index = 1; index < slots.size(); ++index) {
        if (slots[index].first == slots[index - 1].first) {
            return true;
        }
    }
    return false;
}

inline bool StackMemberOrder(const std::vector<Card>& cards,
                             const std::wstring& groupKey,
                             std::vector<size_t>& members) {
    members.clear();
    std::vector<std::pair<size_t, size_t>> slots;
    CollectStackSlots(cards, groupKey, slots);
    std::stable_sort(slots.begin(), slots.end(), [](const auto& left, const auto& right) {
        return left.first < right.first;
    });
    if (HasDuplicateSlot(slots)) {
        return false;
    }
    members.reserve(slots.size());
    for (const auto& slot : slots) {
        members.push_back(slot.second);
    }
    return !members.empty();
}

inline std::vector<size_t> RotateStackOrder(const std::vector<size_t>& members, int shift) {
    std::vector<size_t> order;
    order.reserve(members.size());
    const int count = static_cast<int>(members.size());
    for (int slot = 0; slot < count; ++slot) {
        order.push_back(members[static_cast<size_t>((shift + slot) % count)]);
    }
    return order;
}

inline bool CycleCardStack(std::vector<Card>& cards, const CanvasSettings& settings,
                           int hitIndex, int steps) {
    if (!settings.stacksEnabled || hitIndex < 0 || steps == 0 ||
        static_cast<size_t>(hitIndex) >= cards.size()) {
        return false;
    }
    const CardStackPlacement& hitPlacement = cards[static_cast<size_t>(hitIndex)].stack;
    if (!hitPlacement.grouped || hitPlacement.groupKey.empty()) {
        return false;
    }
    std::vector<size_t> members;
    if (!StackMemberOrder(cards, hitPlacement.groupKey, members) || members.size() <= 1) {
        return false;
    }
    if (hitPlacement.slot >= members.size() ||
        members[hitPlacement.slot] != static_cast<size_t>(hitIndex)) {
        return false;
    }
    const int count = static_cast<int>(members.size());
    const int shift = (steps % count + count) % count;
    if (shift == 0) {
        return false;
    }
    ApplyStackOrder(cards, RotateStackOrder(members, shift), settings);
    return true;
}

// Alt+wheel cycles a stack instead of zooming. Wheel delta is in
// WHEEL_DELTA units (120 per detent, positive = wheel up): wheel down
// brings the next card to the front, wheel up brings the previous one
// back. A sub-detent delta maps to zero steps and cycles nothing.
inline int StackWheelSteps(int wheelDelta) {
    return -wheelDelta / WHEEL_DELTA;
}

inline bool CycleStackForWheel(std::vector<Card>& cards,
                               const CanvasSettings& settings, int hitIndex,
                               int wheelDelta) {
    if (wheelDelta == 0) {
        return false;
    }
    return CycleCardStack(cards, settings, hitIndex, StackWheelSteps(wheelDelta));
}

// Alt+wheel cycle phases, as fractions of the transition duration. The
// outgoing front card lifts above the stack and fades out first, then the
// incoming front fades in while every member eases into its new slot, so the
// stack reads as one set of cards rotating under the new front.
constexpr float kStackCycleLiftFraction = 0.35f;
constexpr float kStackCycleIncomingFraction = 0.65f;
constexpr float kStackCycleFadeSpan =
    kStackCycleIncomingFraction - kStackCycleLiftFraction;
constexpr float kStackCycleSettleSpan = 1.0f - kStackCycleIncomingFraction;

struct StackCycleRequest {
    const CanvasSettings& settings;
    int hitIndex;
    int steps;
    DWORD now;
};

inline Card* FindStackCard(std::vector<Card>& cards, const std::wstring& groupKey,
                          const std::wstring& fingerprint) {
    const auto card = std::find_if(
        cards.begin(), cards.end(), [&fingerprint, &groupKey](const Card& candidate) {
            return candidate.fingerprint == fingerprint &&
                   candidate.stack.groupKey == groupKey;
        });
    return card == cards.end() ? nullptr : &*card;
}

inline float StackCycleProgress(const StackCycleTransition& transition, DWORD now) {
    if (transition.durationMs == 0) {
        return 1.0f;
    }
    const float elapsed = static_cast<float>(now - transition.startTick);
    return std::clamp(elapsed / static_cast<float>(transition.durationMs), 0.0f, 1.0f);
}

inline float StackCycleOpacity(const StackCardTransition& pose, float progress) {
    if (pose.outgoing) {
        if (progress < kStackCycleLiftFraction) {
            return pose.fromOpacity * (1.0f - progress / kStackCycleLiftFraction);
        }
        if (progress < kStackCycleIncomingFraction) {
            return 0.0f;
        }
        return std::clamp((progress - kStackCycleIncomingFraction) /
                              kStackCycleSettleSpan,
                          0.0f, 1.0f);
    }
    if (pose.incoming) {
        if (progress < kStackCycleLiftFraction) {
            return 0.0f;
        }
        if (progress < kStackCycleIncomingFraction) {
            return std::clamp((progress - kStackCycleLiftFraction) /
                                  kStackCycleFadeSpan,
                              0.0f, 1.0f);
        }
    }
    return pose.fromOpacity +
        (1.0f - pose.fromOpacity) * progress;
}

inline void ApplyStackCyclePose(Card& card, const StackCardTransition& pose,
                                float eased, float progress) {
    card.stack.rotationRadians =
        pose.fromRotation + (pose.toRotation - pose.fromRotation) * eased;
    card.stack.scale = pose.fromScale + (pose.toScale - pose.fromScale) * eased;
    card.stack.drawAboveStack = pose.outgoing && progress < kStackCycleLiftFraction;
    card.stack.opacity = StackCycleOpacity(pose, progress);
}

inline void SettleStackCyclePose(Card& card, const StackCardTransition& pose) {
    card.stack.rotationRadians = pose.toRotation;
    card.stack.scale = pose.toScale;
    card.stack.opacity = 1.0f;
    card.stack.drawAboveStack = false;
}

inline void ApplyStackCycleFrame(std::vector<Card>& cards,
                                const StackCycleTransition& transition,
                                float progress) {
    const float eased = progress * progress * (3.0f - 2.0f * progress);
    for (const StackCardTransition& pose : transition.cards) {
        Card* card = FindStackCard(cards, transition.groupKey, pose.fingerprint);
        if (card == nullptr) {
            continue;
        }
        if (progress >= 1.0f) {
            SettleStackCyclePose(*card, pose);
            continue;
        }
        ApplyStackCyclePose(*card, pose, eased, progress);
    }
}

inline bool AdvanceStackCycleTransitions(
    std::vector<Card>& cards, std::vector<StackCycleTransition>& transitions,
    DWORD now) {
    bool active = false;
    for (size_t transitionIndex = transitions.size(); transitionIndex-- > 0;) {
        const StackCycleTransition& transition = transitions[transitionIndex];
        const float progress = StackCycleProgress(transition, now);
        ApplyStackCycleFrame(cards, transition, progress);
        if (progress >= 1.0f) {
            transitions.erase(transitions.begin() +
                              static_cast<std::ptrdiff_t>(transitionIndex));
            continue;
        }
        active = true;
    }
    return active;
}

inline bool CycleStackWithAnimation(std::vector<Card>& cards,
                                    const StackCycleRequest& request,
                                    std::vector<StackCycleTransition>& transitions) {
    if (request.steps == 0 || request.hitIndex < 0 ||
        static_cast<size_t>(request.hitIndex) >= cards.size()) {
        return false;
    }
    const std::wstring groupKey =
        cards[static_cast<size_t>(request.hitIndex)].stack.groupKey;
    if (groupKey.empty()) {
        return false;
    }
    AdvanceStackCycleTransitions(cards, transitions, request.now);
    std::vector<size_t> members;
    if (!StackMemberOrder(cards, groupKey, members) || members.size() <= 1) {
        return false;
    }
    const size_t frontIndex = members.front();
    std::vector<StackCardTransition> poses;
    poses.reserve(members.size());
    for (const size_t index : members) {
        const Card& card = cards[index];
        poses.push_back(StackCardTransition{card.fingerprint,
            card.stack.rotationRadians, card.stack.scale, card.stack.opacity,
            0.0f, 1.0f, index == frontIndex, false});
    }
    if (!CycleCardStack(cards, request.settings, request.hitIndex, request.steps)) {
        return false;
    }
    for (StackCardTransition& pose : poses) {
        Card* card = FindStackCard(cards, groupKey, pose.fingerprint);
        if (card != nullptr) {
            pose.toRotation = card->stack.rotationRadians;
            pose.toScale = card->stack.scale;
            pose.incoming = card->stack.slot == 0 && !pose.outgoing;
            card->stack.opacity = pose.fromOpacity;
            card->stack.drawAboveStack = pose.outgoing;
        }
    }
    transitions.erase(
        std::remove_if(transitions.begin(), transitions.end(), [&groupKey](const auto& item) {
            return item.groupKey == groupKey;
        }), transitions.end());
    if (request.settings.animationMs <= 0) {
        for (Card& card : cards) {
            if (card.stack.groupKey == groupKey) {
                card.stack.opacity = 1.0f;
                card.stack.drawAboveStack = false;
            }
        }
        return true;
    }
    transitions.push_back(StackCycleTransition{groupKey, request.now,
        static_cast<DWORD>(request.settings.animationMs), std::move(poses)});
    AdvanceStackCycleTransitions(cards, transitions, request.now);
    return true;
}



inline int VirtualScreenWidth() {
    return context.virtualScreen.right - context.virtualScreen.left;
}

inline int VirtualScreenHeight() {
    return context.virtualScreen.bottom - context.virtualScreen.top;
}

// Describes a desktop rectangle in canvas world units. World units are
// virtual-screen physical pixels measured from the top-left corner of the
// virtual screen, matching the projection in math_utils.h.
inline RectF WorldFromFrame(const RECT& frame) {
    return RectF{
        static_cast<float>(frame.left - context.virtualScreen.left),
        static_cast<float>(frame.top - context.virtualScreen.top),
        static_cast<float>(frame.right - context.virtualScreen.left),
        static_cast<float>(frame.bottom - context.virtualScreen.top)};
}

inline bool UpdateChromeDpi() {
    POINT cursor = {};
    if (!GetCursorPos(&cursor)) {
        return false;
    }
    const HMONITOR monitor =
        MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    context.chromeMonitor = monitor;
    UINT dpiX = 96;
    UINT dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
        dpiX != 0) {
        const bool changed = context.chromeDpi != static_cast<int>(dpiX);
        context.chromeDpi = static_cast<int>(dpiX);
        return changed;
    }
    return false;
}

inline bool CardOverlapsAny(const RectF& candidate, float gap,
                            int ignoreIndex) {
    const RectF padded = GrowRect(candidate, gap);
    for (size_t index = 0; index < context.cards.size(); ++index) {
        if (static_cast<int>(index) == ignoreIndex) {
            continue;
        }
        if (RectsOverlap(padded, context.cards[index].world)) {
            return true;
        }
    }
    return false;
}

// Spiral shelf search around the world origin: the first ring tries the
// right/bottom ring, then grows outward. Step size follows the candidate so
// large windows never need a long linear scan.
inline POINT PackingOffset(int ring, int step) {
    if (ring == 0) {
        return POINT{0, 0};
    }
    const int edgeLength = ring * 2;
    const int side = step / edgeLength;
    const int offset = step % edgeLength;
    switch (side) {
        case 0: return POINT{-ring + offset, -ring};
        case 1: return POINT{ring, -ring + offset};
        case 2: return POINT{ring - offset, ring};
        default: return POINT{-ring, ring - offset};
    }
}

inline RectF TryPackingRing(int ring, Vec2 size) {
    const float gap = static_cast<float>(context.settings.gap);
    const float stepX = std::max(size.x + gap, 64.0f);
    const float stepY = std::max(size.y + gap, 64.0f);
    const int count = std::max(1, ring * 8);
    for (int step = 0; step < count; ++step) {
        const POINT offset = PackingOffset(ring, step);
        const RectF candidate = MakeRectF(static_cast<float>(offset.x) * stepX,
                                         static_cast<float>(offset.y) * stepY,
                                         size.x, size.y);
        if (!CardOverlapsAny(candidate, gap, -1)) {
            return candidate;
        }
    }
    return RectF{};
}

inline RectF FindFreeSpot(Vec2 size) {
    for (int ring = 0; ring < 48; ++ring) {
        const RectF candidate = TryPackingRing(ring, size);
        if (!candidate.empty()) {
            return candidate;
        }
    }
    float right = 0.0f;
    for (const Card& card : context.cards) {
        right = std::max(right, card.world.right);
    }
    return MakeRectF(right + static_cast<float>(context.settings.gap), 0.0f,
                     size.x, size.y);
}

inline void InsertCard(ScannedWindow&& window, RectF world) {
    Card card;
    card.hwnd = window.hwnd;
    card.processId = window.processId;
    card.threadId = window.threadId;
    card.windowClass = std::move(window.windowClass);
    card.state = window.state;
    card.framePx = window.framePx;
    card.world = world;
    card.title = std::move(window.title);
    card.fingerprint = std::move(window.fingerprint);
    card.exePath = std::move(window.exePath);
    card.applicationKey = std::move(window.applicationKey);
    card.zOrder = window.zOrder;
    context.cards.push_back(std::move(card));
}

inline int FindCardIndex(const ScannedWindow& window) {
    for (size_t index = 0; index < context.cards.size(); ++index) {
        if (MatchesWindow(context.cards[index], window)) {
            return static_cast<int>(index);
        }
    }
    return -1;
}

inline RectF InitialWorldFor(const ScannedWindow& window) {
    if (context.settings.initialLayout == InitialLayout::Desktop) {
        return WorldFromFrame(window.framePx);
    }
    return FindFreeSpot(Vec2{static_cast<float>(window.framePx.right -
                                               window.framePx.left),
                             static_cast<float>(window.framePx.bottom -
                                                window.framePx.top)});
}

inline void RemoveCardAt(size_t index) {
    if (index >= context.cards.size()) {
        return;
    }
    context.cards[index].thumbnail.reset();
    context.cards.erase(context.cards.begin() +
                        static_cast<long long>(index));
    if (context.selectedCard == static_cast<int>(index)) {
        context.selectedCard = -1;
    } else if (context.selectedCard > static_cast<int>(index)) {
        --context.selectedCard;
    }
    if (context.hoverCard == static_cast<int>(index)) {
        context.hoverCard = -1;
    } else if (context.hoverCard > static_cast<int>(index)) {
        --context.hoverCard;
    }
    if (context.draggedCard == static_cast<int>(index)) {
        context.draggedCard = -1;
    } else if (context.draggedCard > static_cast<int>(index)) {
        --context.draggedCard;
    }
    if (context.dropTargetCard == static_cast<int>(index)) {
        context.dropTargetCard = -1;
    } else if (context.dropTargetCard > static_cast<int>(index)) {
        --context.dropTargetCard;
    }
}

inline void ClearCards() {
    for (Card& card : context.cards) {
        card.thumbnail.reset();
    }
    context.cards.clear();
    context.hoverCard = -1;
    context.selectedCard = -1;
    context.draggedCard = -1;
}

// Applies one scan result to an existing card. The canvas position is owned by
// the canvas, so a window that moved on the desktop never drags its card
// along; only the descriptive fields follow the window.
inline bool UpdateCardFromScan(size_t index, const ScannedWindow& window) {
    Card& card = context.cards[index];
    const bool applicationChanged = card.applicationKey != window.applicationKey;
    const bool changed = card.state != window.state || card.title != window.title ||
                         !EqualRect(&card.framePx, &window.framePx) ||
                         card.zOrder != window.zOrder || applicationChanged;
    if (applicationChanged) {
        card.world = StackWorldRect(card);
        ResetStackPlacement(card);
        card.applicationKey = window.applicationKey;
    }
    if (card.state != window.state) {
        card.thumbnail.reset();
    }
    const float width = static_cast<float>(window.framePx.right - window.framePx.left);
    const float height = static_cast<float>(window.framePx.bottom - window.framePx.top);
    card.world.right = card.world.left + width;
    card.world.bottom = card.world.top + height;
    card.state = window.state;
    card.framePx = window.framePx;
    card.title = window.title;
    card.exePath = window.exePath;
    card.zOrder = window.zOrder;
    if (const LayoutEntry* saved = FindLayoutEntry(window.fingerprint);
        saved != nullptr) {
        card.pinned = saved->pinned;
        card.stack.grouped = saved->grouped;
        card.stackId = saved->stackId;
    }
    return changed;
}

// Merges one scan into the card vector. Existing cards keep their canvas
// position and thumbnail. New cards take a saved position when the fingerprint
// matches, otherwise they are packed largest-first so wide windows claim the
// centre ring before small dialogs fill the gaps.
inline bool MergeScan(const std::vector<ScannedWindow>& scanned) {
    std::vector<bool> seen(context.cards.size(), false);
    std::vector<ScannedWindow> additions;
    bool changed = false;
    for (const ScannedWindow& window : scanned) {
        const int existing = FindCardIndex(window);
        if (existing >= 0) {
            seen[static_cast<size_t>(existing)] = true;
            changed = UpdateCardFromScan(static_cast<size_t>(existing), window) || changed;
            continue;
        }
        additions.push_back(window);
    }

    for (size_t index = context.cards.size(); index-- > 0;) {
        if (index < seen.size() && seen[static_cast<size_t>(index)]) {
            continue;
        }
        RemoveCardAt(index);
        changed = true;
    }

    if (!additions.empty()) {
        std::stable_sort(additions.begin(), additions.end(),
                         [](const ScannedWindow& left, const ScannedWindow& right) {
                             return CardArea(left) > CardArea(right);
                         });
        const DWORD now = GetTickCount();
        for (const ScannedWindow& window : additions) {
            const LayoutEntry* saved = FindLayoutEntry(window.fingerprint);
            RectF world = saved != nullptr ? saved->world : InitialWorldFor(window);
            world.right = world.left + static_cast<float>(window.framePx.right - window.framePx.left);
            world.bottom = world.top + static_cast<float>(window.framePx.bottom - window.framePx.top);
            InsertCard(ScannedWindow{window}, world);
            if (saved != nullptr) {
                context.cards.back().pinned = saved->pinned;
                context.cards.back().stack.grouped = saved->grouped;
                context.cards.back().stackId = saved->stackId;
            }
            RememberLayoutEntry(window.fingerprint, world);
            changed = true;
        }
        if (changed) {
            MarkLayoutDirty(now);
        }
    }
    if (changed) {
        context.stackTransitions.clear();
        ReconcileCardStacks(context.cards, context.settings);
    }
    return changed;
}

struct SnapResult {
    float left = 0.0f;
    float top = 0.0f;
    bool snapVertical = false;
    bool snapHorizontal = false;
    float guideX = 0.0f;
    float guideY = 0.0f;
};

struct SnapAxis {
    float distance;
    float shift;
    float guide = 0.0f;
    bool snapped = false;
};

inline void ConsiderSnap(float value, float target, SnapAxis& axis) {
    const float delta = target - value;
    if (std::fabs(delta) > axis.distance ||
        std::fabs(delta) >= std::fabs(axis.shift)) {
        return;
    }
    axis.shift = delta;
    axis.guide = target;
    axis.snapped = true;
}

inline void SnapEdges(const float (&moving)[3], const float (&other)[3],
                       SnapAxis& axis) {
    for (const float value : moving) {
        for (const float target : other) {
            ConsiderSnap(value, target, axis);
        }
    }
}

inline void SnapAgainstCard(const RectF& moving, const RectF& other,
                            std::pair<SnapAxis, SnapAxis>& axes) {
    const float movingX[3] = {moving.left, moving.centerX(), moving.right};
    const float movingY[3] = {moving.top, moving.centerY(), moving.bottom};
    const float otherX[3] = {other.left, other.centerX(), other.right};
    const float otherY[3] = {other.top, other.centerY(), other.bottom};
    SnapEdges(movingX, otherX, axes.first);
    SnapEdges(movingY, otherY, axes.second);
}

// Magnetic alignment uses the moving card's left, center and right edges
// against the same three edges of every other card.
inline SnapResult ComputeSnap(const RectF& moving, int ignoreIndex) {
    SnapResult result{moving.left, moving.top};
    const float distance =
        static_cast<float>(context.settings.snapDistance) / context.camera.zoom;
    if (distance <= 0.0f) {
        return result;
    }
    std::pair<SnapAxis, SnapAxis> axes{SnapAxis{distance, distance + 1.0f},
                                      SnapAxis{distance, distance + 1.0f}};
    const bool hasMovingCard = ignoreIndex >= 0 &&
        static_cast<size_t>(ignoreIndex) < context.cards.size();
    for (size_t index = 0; index < context.cards.size(); ++index) {
        if (static_cast<int>(index) == ignoreIndex ||
            (hasMovingCard && SameActiveStack(
                context.cards[static_cast<size_t>(ignoreIndex)], context.cards[index]))) {
            continue;
        }
        SnapAgainstCard(moving, StackWorldRect(context.cards[index]), axes);
    }
    result.snapVertical = axes.first.snapped;
    result.snapHorizontal = axes.second.snapped;
    result.guideX = axes.first.guide;
    result.guideY = axes.second.guide;
    result.left += result.snapVertical ? axes.first.shift : 0.0f;
    result.top += result.snapHorizontal ? axes.second.shift : 0.0f;
    return result;
}

// Direct2D, DirectWrite, WIC and shared GDI surfaces.


// Icon source selection for icon cards.
//
// An icon card draws its bitmap at up to kCardIconMaxPx display pixels, so the
// lookup starts at the largest source the shell offers. Asking for a 48 px
// shell icon or the 32 px window icon enlarges the bitmap two to four times and
// the interpolation shows as a soft, blurry icon on a card that has no live
// preview to distract from it.


#include <cstddef>

// Largest first. The shell answers with the requested size or fails, so a
// missing jumbo list on an older image list simply falls through.
inline constexpr int kShellIconListSizes[] = {SHIL_JUMBO, SHIL_EXTRALARGE,
                                              SHIL_LARGE};

// Returns the first icon the callback produces for the candidate sizes, or
// nullptr when every candidate fails. Ownership of a returned icon passes to
// the caller, which must destroy it.
template <size_t Count, typename QueryIcon>
inline HICON QueryFirstAvailableIcon(const int (&sizes)[Count],
                                     QueryIcon queryIcon) {
    for (size_t index = 0; index < Count; ++index) {
        HICON icon = queryIcon(sizes[index]);
        if (icon != nullptr) {
            return icon;
        }
    }
    return nullptr;
}

#include <commoncontrols.h>

inline D2D1_COLOR_F ToColorF(uint32_t rgb, float alpha) {
    return D2D1::ColorF(static_cast<float>((rgb >> 16) & 0xFF) / 255.0f,
                        static_cast<float>((rgb >> 8) & 0xFF) / 255.0f,
                        static_cast<float>(rgb & 0xFF) / 255.0f, alpha);
}

inline HBITMAP CreateHudDib(int width, int height) {
    if (width <= 0 || height <= 0) {
        return nullptr;
    }
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    return CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
}

inline void ReleaseIconCache() {
    for (auto& pair : context.iconCache) {
        if (pair.second != nullptr) {
            pair.second->Release();
        }
    }
    context.iconCache.clear();
    context.iconRetryTick.clear();
}

inline bool ConsumeDueIconRetries(DWORD now) {
    bool due = false;
    auto retry = context.iconRetryTick.begin();
    while (retry != context.iconRetryTick.end()) {
        if (now - retry->second < kIconRetryDelayMs) {
            ++retry;
            continue;
        }
        retry = context.iconRetryTick.erase(retry);
        due = true;
    }
    return due;
}

inline void ReleaseHudSurface() {
    ReleaseIconCache();
    if (context.hudBrush != nullptr) {
        context.hudBrush->Release();
        context.hudBrush = nullptr;
    }
    if (context.hudTarget != nullptr) {
        context.hudTarget->Release();
        context.hudTarget = nullptr;
    }
    if (context.hudDc != nullptr) {
        if (context.hudOriginalBitmap != nullptr) {
            SelectObject(context.hudDc, context.hudOriginalBitmap);
            context.hudOriginalBitmap = nullptr;
        }
        DeleteDC(context.hudDc);
        context.hudDc = nullptr;
    }
    if (context.hudBitmap != nullptr) {
        DeleteObject(context.hudBitmap);
        context.hudBitmap = nullptr;
    }
    context.hudWidth = 0;
    context.hudHeight = 0;
}

inline void ReleaseTextFormats() {
    if (context.titleFormat != nullptr) {
        context.titleFormat->Release();
        context.titleFormat = nullptr;
    }
    if (context.badgeFormat != nullptr) {
        context.badgeFormat->Release();
        context.badgeFormat = nullptr;
    }
    if (context.tooltipFormat != nullptr) {
        context.tooltipFormat->Release();
        context.tooltipFormat = nullptr;
    }
}

inline void ReleaseTextResources() {
    ReleaseTextFormats();
    if (context.writeFactory != nullptr) {
        context.writeFactory->Release();
        context.writeFactory = nullptr;
    }
}

inline void ReleaseGraphics() {
    ReleaseHudSurface();
    ReleaseTextResources();
    if (context.wicFactory != nullptr) {
        context.wicFactory->Release();
        context.wicFactory = nullptr;
    }
    if (context.d2dFactory != nullptr) {
        context.d2dFactory->Release();
        context.d2dFactory = nullptr;
    }
}

inline bool CreateTitleFormat() {
    const float size = 13.0f * ChromeScale();
    const HRESULT result = context.writeFactory->CreateTextFormat(
        L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"",
        &context.titleFormat);
    if (FAILED(result)) {
        return false;
    }
    return SUCCEEDED(context.titleFormat->SetWordWrapping(
               DWRITE_WORD_WRAPPING_NO_WRAP)) &&
           SUCCEEDED(context.titleFormat->SetTextAlignment(
               DWRITE_TEXT_ALIGNMENT_LEADING)) &&
           SUCCEEDED(context.titleFormat->SetParagraphAlignment(
               DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}

inline bool CreateBadgeFormat() {
    const float badgeSize = 11.0f * ChromeScale();
    const HRESULT result = context.writeFactory->CreateTextFormat(
        L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, badgeSize, L"", &context.badgeFormat);
    if (FAILED(result)) {
        return false;
    }
    return SUCCEEDED(context.badgeFormat->SetWordWrapping(
               DWRITE_WORD_WRAPPING_NO_WRAP)) &&
           SUCCEEDED(context.badgeFormat->SetTextAlignment(
               DWRITE_TEXT_ALIGNMENT_TRAILING)) &&
           SUCCEEDED(context.badgeFormat->SetParagraphAlignment(
               DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}

inline bool CreateTooltipFormat() {
    const float tooltipSize = 11.0f * ChromeScale();
    const HRESULT result = context.writeFactory->CreateTextFormat(
        L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, tooltipSize, L"", &context.tooltipFormat);
    if (FAILED(result)) {
        return false;
    }
    return SUCCEEDED(context.tooltipFormat->SetWordWrapping(
               DWRITE_WORD_WRAPPING_NO_WRAP)) &&
           SUCCEEDED(context.tooltipFormat->SetTextAlignment(
               DWRITE_TEXT_ALIGNMENT_CENTER)) &&
           SUCCEEDED(context.tooltipFormat->SetParagraphAlignment(
               DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}

inline bool CreateTextFormats() {
    ReleaseTextFormats();
    if (context.writeFactory == nullptr) {
        return false;
    }
    if (!CreateTitleFormat() || !CreateBadgeFormat() ||
        !CreateTooltipFormat()) {
        ReleaseTextFormats();
        return false;
    }
    return true;
}

inline bool CreateHudBackingStore(int width, int height) {
    context.hudDc = CreateCompatibleDC(nullptr);
    if (context.hudDc == nullptr) {
        return false;
    }
    context.hudBitmap = CreateHudDib(width, height);
    if (context.hudBitmap == nullptr) {
        return false;
    }
    const HGDIOBJ original = SelectObject(context.hudDc, context.hudBitmap);
    if (original == nullptr || original == HGDI_ERROR) {
        return false;
    }
    context.hudOriginalBitmap = original;
    return true;
}

inline bool CreateHudRenderTarget(int width, int height) {
    if (context.d2dFactory == nullptr) {
        return false;
    }
    const D2D1_RENDER_TARGET_PROPERTIES properties =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED));
    const HRESULT result = context.d2dFactory->CreateDCRenderTarget(
        &properties, &context.hudTarget);
    if (FAILED(result) || context.hudTarget == nullptr) {
        return false;
    }
    const RECT bounds = {0, 0, width, height};
    if (FAILED(context.hudTarget->BindDC(context.hudDc, &bounds))) {
        return false;
    }
    return SUCCEEDED(context.hudTarget->CreateSolidColorBrush(
               D2D1::ColorF(D2D1::ColorF::White), &context.hudBrush)) &&
           context.hudBrush != nullptr;
}

inline bool CreateHudSurface(int width, int height) {
    if (width <= 0 || height <= 0) {
        return false;
    }
    if (context.hudTarget != nullptr && context.hudDc != nullptr &&
        context.hudBrush != nullptr && context.hudBitmap != nullptr &&
        context.hudWidth >= width && context.hudHeight >= height) {
        return true;
    }
    ReleaseHudSurface();
    if (!CreateHudBackingStore(width, height) ||
        !CreateHudRenderTarget(width, height)) {
        ReleaseHudSurface();
        return false;
    }
    context.hudWidth = width;
    context.hudHeight = height;
    return true;
}

inline bool EnsureGraphics() {
    if (context.d2dFactory == nullptr) {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                     &context.d2dFactory))) {
            Wh_Log(L"Aleph: D2D1CreateFactory failed");
            return false;
        }
    }
    if (context.writeFactory == nullptr) {
        if (FAILED(DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown**>(&context.writeFactory)))) {
            Wh_Log(L"Aleph: DWriteCreateFactory failed");
            return false;
        }
    }
    if (context.wicFactory == nullptr) {
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                    CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(&context.wicFactory)))) {
            Wh_Log(L"Aleph: WIC factory creation failed");
            return false;
        }
    }
    if ((context.titleFormat == nullptr || context.badgeFormat == nullptr ||
         context.tooltipFormat == nullptr) &&
        !CreateTextFormats()) {
        Wh_Log(L"Aleph: text format creation failed");
        return false;
    }
    return true;
}

inline HICON QueryWindowIcon(HWND window) {
    DWORD_PTR iconResult = 0;
    const LRESULT sent = SendMessageTimeoutW(
        window, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 50,
        &iconResult);
    HICON icon = sent != 0 ? reinterpret_cast<HICON>(iconResult) : nullptr;
    if (icon == nullptr) {
        icon = reinterpret_cast<HICON>(
            GetClassLongPtrW(window, GCLP_HICON));
    }
    if (icon == nullptr) {
        icon = reinterpret_cast<HICON>(
            GetClassLongPtrW(window, GCLP_HICONSM));
    }
    return icon;
}

inline HICON QueryShellIconAtSize(int imageIndex, int listSize) {
    IImageList* images = nullptr;
    const HRESULT queried =
        SHGetImageList(listSize, IID_PPV_ARGS(&images));
    if (FAILED(queried) || images == nullptr) {
        if (images != nullptr) {
            images->Release();
        }
        return nullptr;
    }
    HICON icon = nullptr;
    const HRESULT result = images->GetIcon(imageIndex, ILD_TRANSPARENT, &icon);
    images->Release();
    if (FAILED(result)) {
        if (icon != nullptr) {
            DestroyIcon(icon);
        }
        return nullptr;
    }
    return icon;
}

inline HICON QueryShellIcon(const std::wstring& exePath) {
    if (exePath.empty()) {
        return nullptr;
    }
    SHFILEINFOW info = {};
    if (SHGetFileInfoW(exePath.c_str(), 0, &info, sizeof(info),
                       SHGFI_SYSICONINDEX) == 0) {
        return nullptr;
    }
    return QueryFirstAvailableIcon(
        kShellIconListSizes,
        [&](int listSize) {
            return QueryShellIconAtSize(info.iIcon, listSize);
        });
}

inline ID2D1Bitmap* CreateHudIconBitmap(IWICBitmapSource* bitmap) {
    IWICBitmapSource* converted = nullptr;
    const HRESULT conversion = WICConvertBitmapSource(
        GUID_WICPixelFormat32bppPBGRA, bitmap, &converted);
    if (FAILED(conversion) || converted == nullptr) {
        if (converted != nullptr) {
            converted->Release();
        }
        return nullptr;
    }
    ID2D1Bitmap* result = nullptr;
    const HRESULT created = context.hudTarget->CreateBitmapFromWicBitmap(
        converted, nullptr, &result);
    converted->Release();
    if (FAILED(created)) {
        if (result != nullptr) {
            result->Release();
        }
        return nullptr;
    }
    return result;
}

inline ID2D1Bitmap* CreateIconBitmap(HICON icon) {
    if (icon == nullptr || context.wicFactory == nullptr ||
        context.hudTarget == nullptr) {
        return nullptr;
    }
    IWICBitmap* bitmap = nullptr;
    const HRESULT converted =
        context.wicFactory->CreateBitmapFromHICON(icon, &bitmap);
    if (FAILED(converted) || bitmap == nullptr) {
        if (bitmap != nullptr) {
            bitmap->Release();
        }
        return nullptr;
    }
    ID2D1Bitmap* result = CreateHudIconBitmap(bitmap);
    bitmap->Release();
    return result;
}

inline ID2D1Bitmap* LoadShellIconBitmap(const std::wstring& exePath) {
    HICON icon = QueryShellIcon(exePath);
    if (icon == nullptr) {
        return nullptr;
    }
    ID2D1Bitmap* bitmap = CreateIconBitmap(icon);
    DestroyIcon(icon);
    return bitmap;
}

inline ID2D1Bitmap* LoadIconBitmap(const std::wstring& exePath,
                                   const std::wstring& windowClass,
                                   HWND window) {
    if (context.hudTarget == nullptr || context.wicFactory == nullptr) {
        return nullptr;
    }
    const std::wstring key = exePath + L"|" + windowClass;
    const auto cached = context.iconCache.find(key);
    if (cached != context.iconCache.end() && cached->second != nullptr) {
        return cached->second;
    }
    const auto retry = context.iconRetryTick.find(key);
    if (retry != context.iconRetryTick.end() &&
        GetTickCount() - retry->second < kIconRetryDelayMs) {
        return nullptr;
    }

    // The shell icon is the largest source, so it wins whenever it exists; the
    // window icon covers executables the shell cannot resolve.
    ID2D1Bitmap* result = LoadShellIconBitmap(exePath);
    if (result == nullptr) {
        result = CreateIconBitmap(QueryWindowIcon(window));
    }
    if (result == nullptr) {
        context.iconRetryTick[key] = GetTickCount();
        return nullptr;
    }
    context.iconCache[key] = result;
    context.iconRetryTick.erase(key);
    return result;
}


inline RectF MinimapInnerRect(const RectF& minimap) {
    const float pad = kMinimapPaddingPx * ChromeScale();
    return RectF{minimap.left + pad, minimap.top + pad, minimap.right - pad,
                 minimap.bottom - pad};
}

inline RectF CanvasWorldBounds() {
    RectF bounds{};
    for (const Card& card : context.cards) {
        bounds = UnionRect(bounds, StackWorldRect(card));
    }
    if (bounds.empty()) {
        return RectF{-500.0f, -500.0f, 500.0f, 500.0f};
    }
    const float margin = 100.0f;
    return GrowRect(bounds, margin);
}

inline RectF CalculateMinimapRect() {
    const float scale = ChromeScale();
    const float size = static_cast<float>(context.settings.minimapSize) * scale;
    const float margin = kMinimapMarginPx * scale;
    const float width = std::min(size, static_cast<float>(VirtualScreenWidth()) - margin * 2.0f);
    const float height = std::min(size, static_cast<float>(VirtualScreenHeight()) - margin * 2.0f);
    return MakeRectF(static_cast<float>(VirtualScreenWidth()) - width - margin,
                     static_cast<float>(VirtualScreenHeight()) - height - margin,
                     width, height);
}

inline RectF MinimapWorldToScreen(const RectF& world, const RectF& minimap,
                                 const RectF& bounds) {
    const RectF inner = MinimapInnerRect(minimap);
    const float scale = std::min(inner.width() / bounds.width(),
                                 inner.height() / bounds.height());
    const float offsetX = inner.left + (inner.width() - bounds.width() * scale) * 0.5f;
    const float offsetY = inner.top + (inner.height() - bounds.height() * scale) * 0.5f;
    return RectF{offsetX + (world.left - bounds.left) * scale,
                 offsetY + (world.top - bounds.top) * scale,
                 offsetX + (world.right - bounds.left) * scale,
                 offsetY + (world.bottom - bounds.top) * scale};
}

// Clicking or dragging inside the minimap centres the camera on the world
// point under the pointer.
inline void PanCameraFromMinimap(Vec2 point) {
    const RectF bounds = CanvasWorldBounds();
    const RectF inner = MinimapInnerRect(context.minimapRect);
    if (inner.empty()) {
        return;
    }
    const float scale = std::min(inner.width() / bounds.width(),
                                 inner.height() / bounds.height());
    if (scale <= 0.0f) {
        return;
    }
    const float offsetX = inner.left + (inner.width() - bounds.width() * scale) * 0.5f;
    const float offsetY = inner.top + (inner.height() - bounds.height() * scale) * 0.5f;
    CenterOnPoint(context.camera,
                  Vec2{bounds.left + (point.x - offsetX) / scale,
                       bounds.top + (point.y - offsetY) / scale});
}


inline void FitAllCards() {
    if (context.cards.empty()) {
        CenterOnPoint(context.camera, Vec2{ScreenCenterX(), ScreenCenterY()});
        return;
    }
    RectF bounds{};
    for (const Card& card : context.cards) {
        bounds = UnionRect(bounds, StackWorldRect(card));
    }
    const float width = static_cast<float>(std::max(VirtualScreenWidth(), 1));
    const float height = static_cast<float>(std::max(VirtualScreenHeight(), 1));
    const float zoom = std::min((width * 0.8f) / std::max(bounds.width(), 1.0f),
                                (height * 0.8f) / std::max(bounds.height(), 1.0f));
    context.camera.targetZoom = ClampZoom(zoom);
    CenterOnPoint(context.camera, Vec2{bounds.centerX(), bounds.centerY()});
}

inline void CenterOnCard(int index) {
    if (index < 0 || static_cast<size_t>(index) >= context.cards.size()) {
        return;
    }
    const RectF world = StackWorldRect(context.cards[static_cast<size_t>(index)]);
    CenterOnPoint(context.camera, Vec2{world.centerX(), world.centerY()});
}

inline Camera DiveCameraForCard(const Card& card) {
    Camera target = context.camera;
    target.targetZoom = 1.0f;
    const RectF world = StackWorldRect(card);
    const float frameCenterX =
        static_cast<float>(card.framePx.left - context.virtualScreen.left) +
        static_cast<float>(card.framePx.right - card.framePx.left) * 0.5f;
    const float frameCenterY =
        static_cast<float>(card.framePx.top - context.virtualScreen.top) +
        static_cast<float>(card.framePx.bottom - card.framePx.top) * 0.5f;
    target.targetCenterX = world.centerX() - frameCenterX + ScreenCenterX();
    target.targetCenterY = world.centerY() - frameCenterY + ScreenCenterY();
    return target;
}


// Card header chrome.
//
// Every live card reserves a fixed DIP strip above its body. The strip carries
// the title and its buttons: group joins or splits the windows of one stack
// key - an application group or a hand-made stack - pin keeps a card above
// every unpinned card, close closes the card's real window.
//
// This header stays free of Direct2D and Win32 drawing calls so the native
// regressions can exercise the geometry on its own.


#include <algorithm>
#include <unordered_map>

inline RectF CardHeaderRect(const Card& card) {
    const RectF bounds = CardScreenBounds(card);
    return RectF{bounds.left, bounds.top, bounds.right,
                 bounds.top + CardHeaderHeight()};
}

// The inset shrinks with the header so a zoomed-out card still keeps its
// buttons inside the strip instead of clipping them.
inline float CardHeaderButtonInset(float headerHeight, float scale) {
    return std::min(kCardHeaderButtonInsetPx * scale, headerHeight * 0.2f);
}

inline float CardHeaderButtonSize(float headerHeight, float scale) {
    if (headerHeight <= 0.0f) {
        return 0.0f;
    }
    const float inset = CardHeaderButtonInset(headerHeight, scale);
    return std::min(kCardHeaderButtonPx * scale, headerHeight - inset * 2.0f);
}

// A window group only exists once another window of the same stack key can
// join it, so the group button appears only for those cards and only while
// stacking is enabled. Tying the button count to the cards keeps every card's
// row width predictable for both drawing and hit testing.
inline int CardHeaderButtonCount(const std::vector<Card>& cards, size_t index,
                                 bool stacksEnabled) {
    if (!stacksEnabled || index >= cards.size()) {
        return 2;
    }
    return cards[index].hasStackPeers ? 3 : 2;
}

inline float CardHeaderButtonRowWidth(const RectF& header, float scale,
                                      int buttons) {
    if (buttons <= 0 || header.empty()) {
        return 0.0f;
    }
    const float size = CardHeaderButtonSize(header.height(), scale);
    const float inset = CardHeaderButtonInset(header.height(), scale);
    const float gap = kCardHeaderButtonGapPx * scale;
    const float count = static_cast<float>(buttons);
    return size * count + gap * (count - 1.0f) + inset * 2.0f;
}

inline bool CardHeaderButtonsFit(const RectF& header, float scale,
                                 int buttons) {
    if (header.empty()) {
        return false;
    }
    const float size = CardHeaderButtonSize(header.height(), scale);
    if (size <= 0.0f) {
        return false;
    }
    const float needed = CardHeaderButtonRowWidth(header, scale, buttons) +
                         kCardTitlePaddingPx * scale * 2.0f;
    return header.width() >= needed;
}

// A narrow card gives up the optional group button first, so a small window of
// a repeated application still keeps its pin and close buttons instead of
// losing the whole row.
inline int CardHeaderButtonCountForWidth(const RectF& header, float scale,
                                         int buttons) {
    if (buttons > 2 && !CardHeaderButtonsFit(header, scale, buttons) &&
        CardHeaderButtonsFit(header, scale, 2)) {
        return 2;
    }
    return buttons;
}

inline RectF CardCloseButtonRect(const RectF& header, float scale, int buttons) {
    if (!CardHeaderButtonsFit(header, scale, buttons)) {
        return {};
    }
    const float size = CardHeaderButtonSize(header.height(), scale);
    const float inset = CardHeaderButtonInset(header.height(), scale);
    const float top = header.centerY() - size * 0.5f;
    return MakeRectF(header.right - inset - size, top, size, size);
}

inline RectF CardPinButtonRect(const RectF& header, float scale, int buttons) {
    const RectF close = CardCloseButtonRect(header, scale, buttons);
    if (close.empty()) {
        return {};
    }
    const float gap = kCardHeaderButtonGapPx * scale;
    return MakeRectF(close.left - gap - close.width(), close.top, close.width(),
                     close.height());
}

// The group button sits left of the pin; an empty rect means the card has no
// window group and therefore no group button.
inline RectF CardGroupButtonRect(const RectF& header, float scale, int buttons) {
    if (buttons < 3) {
        return {};
    }
    const RectF pin = CardPinButtonRect(header, scale, buttons);
    if (pin.empty()) {
        return {};
    }
    const float gap = kCardHeaderButtonGapPx * scale;
    return MakeRectF(pin.left - gap - pin.width(), pin.top, pin.width(),
                     pin.height());
}

inline CardHeaderAction HitTestCardHeader(const RectF& header, Vec2 point,
                                          float scale, int buttons) {
    if (ContainsPoint(CardCloseButtonRect(header, scale, buttons), point.x,
                      point.y)) {
        return CardHeaderAction::Close;
    }
    if (ContainsPoint(CardPinButtonRect(header, scale, buttons), point.x,
                      point.y)) {
        return CardHeaderAction::Pin;
    }
    if (ContainsPoint(CardGroupButtonRect(header, scale, buttons), point.x,
                      point.y)) {
        return CardHeaderAction::Group;
    }
    return CardHeaderAction::None;
}

inline bool IsGroupedStackCard(const Card& card) {
    return card.stack.grouped && !card.stack.groupKey.empty();
}

// Stack members share one draw group so they stay contiguous in paint order;
// an ungrouped card is its own group and keeps its scan position.
inline std::wstring CardDrawGroupKey(const Card& card, size_t index) {
    if (IsGroupedStackCard(card)) {
        return L"S:" + card.stack.groupKey;
    }
    return L"C:" + card.fingerprint + L":" + std::to_wstring(index);
}

inline std::unordered_map<std::wstring, bool> CollectStackPins(
    const std::vector<Card>& cards) {
    std::unordered_map<std::wstring, bool> stackPinned;
    for (const Card& card : cards) {
        if (!IsGroupedStackCard(card)) {
            continue;
        }
        bool& pinned = stackPinned[card.stack.groupKey];
        pinned = pinned || card.pinned;
    }
    return stackPinned;
}

inline std::vector<size_t> AssignDrawGroups(const std::vector<Card>& cards) {
    std::unordered_map<std::wstring, size_t> groupIds;
    std::vector<size_t> drawGroups(cards.size());
    for (size_t index = 0; index < cards.size(); ++index) {
        const auto entry = groupIds.try_emplace(
            CardDrawGroupKey(cards[index], index), groupIds.size()).first;
        drawGroups[index] = entry->second;
    }
    return drawGroups;
}

inline bool CardDrawPinned(
    const Card& card, const std::unordered_map<std::wstring, bool>& stackPinned) {
    if (!IsGroupedStackCard(card)) {
        return card.pinned;
    }
    const auto entry = stackPinned.find(card.stack.groupKey);
    return entry == stackPinned.end() ? card.pinned : entry->second;
}

inline bool IsStackFrontCard(const Card& card) {
    return IsGroupedStackCard(card) &&
           (card.stack.slot == 0 || card.stack.drawAboveStack);
}

// The card lifted out of the stack paints above its own members, then the
// highest slot (deepest background card) paints first and the front slot
// (slot 0) paints last, so a stack always reads as one card with its front on
// top.
inline bool StackDrawLess(const Card& left, const Card& right) {
    if (left.stack.drawAboveStack != right.stack.drawAboveStack) {
        return !left.stack.drawAboveStack;
    }
    return left.stack.slot > right.stack.slot;
}

// Stack fronts are the highest layer, followed by pinned cards, so a pin lifts
// other cards without covering the upright card at the front of a stack.
// Hit testing walks this order backwards, reaching the visible top card first.
inline std::vector<size_t> CardDrawOrder(const std::vector<Card>& cards) {
    const std::unordered_map<std::wstring, bool> stackPinned =
        CollectStackPins(cards);
    const std::vector<size_t> drawGroups = AssignDrawGroups(cards);
    std::vector<size_t> order;
    order.reserve(cards.size());
    for (size_t index = 0; index < cards.size(); ++index) {
        order.push_back(index);
    }
    std::stable_sort(
        order.begin(), order.end(),
        [&cards, &drawGroups, &stackPinned](size_t left, size_t right) {
            const bool leftFront = IsStackFrontCard(cards[left]);
            const bool rightFront = IsStackFrontCard(cards[right]);
            if (leftFront != rightFront) {
                return !leftFront;
            }
            const bool leftPinned = CardDrawPinned(cards[left], stackPinned);
            const bool rightPinned = CardDrawPinned(cards[right], stackPinned);
            if (leftPinned != rightPinned) {
                return !leftPinned;
            }
            if (drawGroups[left] != drawGroups[right]) {
                return drawGroups[left] < drawGroups[right];
            }
            if (!IsGroupedStackCard(cards[left]) ||
                !IsGroupedStackCard(cards[right])) {
                return false;
            }
            return StackDrawLess(cards[left], cards[right]);
        });
    return order;
}

inline int HitTestCardInOrder(const std::vector<Card>& cards, Vec2 point) {
    const std::vector<size_t> order = CardDrawOrder(cards);
    for (size_t slot = order.size(); slot-- > 0;) {
        const size_t index = order[slot];
        if (ContainsPoint(CardScreenBounds(cards[index]), point.x, point.y)) {
            return static_cast<int>(index);
        }
    }
    return -1;
}

inline void ReleaseThumbnail(Card& card) {
    card.thumbnail.reset();
    card.thumbnailVisible = false;
    card.thumbnailDestination = {};
}

inline void FailThumbnail(Card& card, DWORD now) {
    ReleaseThumbnail(card);
    card.thumbnailFailed = true;
    card.thumbnailRetryTick = now;
}

inline bool HasDueThumbnailRetry(DWORD now) {
    for (const Card& card : context.cards) {
        if (card.state == WindowVisualState::Live && !card.destroyed &&
            !IsStackBackground(card) &&
            card.thumbnailFailed &&
            now - card.thumbnailRetryTick >= kThumbnailRetryDelayMs) {
            return true;
        }
    }
    return false;
}

inline bool EnsureThumbnail(Card& card, DWORD now) {
    if (card.destroyed || card.state != WindowVisualState::Live ||
        IsWindow(card.hwnd) == FALSE) {
        ReleaseThumbnail(card);
        return false;
    }
    if (card.thumbnail.valid()) {
        return true;
    }
    if (card.thumbnailFailed && now - card.thumbnailRetryTick < kThumbnailRetryDelayMs) {
        return false;
    }
    HTHUMBNAIL handle = nullptr;
    const HRESULT result =
        DwmRegisterThumbnail(context.hostWindow, card.hwnd, &handle);
    card.thumbnail.reset(handle);
    if (FAILED(result) || !card.thumbnail.valid()) {
        FailThumbnail(card, now);
        return false;
    }
    card.thumbnailFailed = false;
    return true;
}

struct ThumbnailGeometry {
    RectF source{};
    RectF destination{};
};

inline RECT ThumbnailRect(const RectF& rect) {
    return RECT{static_cast<LONG>(std::lround(rect.left)),
                static_cast<LONG>(std::lround(rect.top)),
                static_cast<LONG>(std::lround(rect.right)),
                static_cast<LONG>(std::lround(rect.bottom))};
}

inline RectF ThumbnailViewportClip(const RectF& destination) {
    return RectF{std::max(0.0f, destination.left),
                 std::max(0.0f, destination.top),
                 std::min(static_cast<float>(VirtualScreenWidth()),
                           destination.right),
                 std::min(static_cast<float>(VirtualScreenHeight()),
                           destination.bottom)};
}

inline bool ReadThumbnailSource(const Card& card, RectF& source) {
    RECT window = {};
    RECT frame = {};
    RECT cropped = {};
    SIZE size = {};
    if (GetWindowRect(card.hwnd, &window) == FALSE ||
        !ReadFrameBounds(card.hwnd, frame) ||
        IntersectRect(&cropped, &window, &frame) == FALSE ||
        FAILED(DwmQueryThumbnailSourceSize(card.thumbnail.get(), &size))) {
        return false;
    }
    const RectF outer = RectFromRect(window);
    if (outer.empty() || size.cx <= 0 || size.cy <= 0) {
        return false;
    }
    const float scaleX = static_cast<float>(size.cx) / outer.width();
    const float scaleY = static_cast<float>(size.cy) / outer.height();
    source = RectF{static_cast<float>(cropped.left - window.left) * scaleX,
                   static_cast<float>(cropped.top - window.top) * scaleY,
                   static_cast<float>(cropped.right - window.left) * scaleX,
                   static_cast<float>(cropped.bottom - window.top) * scaleY};
    return !source.empty();
}

// The live preview covers the whole card body. A stack member shares the
// stack frame, which may not have the same aspect as its own window, so the
// preview is letterboxed inside the frame instead of being stretched onto it.
// A standalone card keeps the plain body rectangle.
inline RectF ThumbnailDestinationFor(const Card& card) {
    const RectF frame = WorldToScreen(StackWorldRect(card), context.camera);
    if (frame.empty() || !card.stack.grouped || card.stack.groupKey.empty()) {
        return frame;
    }
    const float width =
        static_cast<float>(card.framePx.right - card.framePx.left);
    const float height =
        static_cast<float>(card.framePx.bottom - card.framePx.top);
    if (width <= 0.0f || height <= 0.0f) {
        return frame;
    }
    return AspectFitRect(frame, width, height);
}

inline bool ClipThumbnailGeometry(ThumbnailGeometry& geometry, const RectF& clip) {
    const RectF clipped{std::max(clip.left, geometry.destination.left),
                        std::max(clip.top, geometry.destination.top),
                        std::min(clip.right, geometry.destination.right),
                        std::min(clip.bottom, geometry.destination.bottom)};
    if (clipped.empty() || geometry.destination.empty() || geometry.source.empty()) {
        return false;
    }
    const float scaleX = geometry.source.width() / geometry.destination.width();
    const float scaleY = geometry.source.height() / geometry.destination.height();
    geometry.source = RectF{
        geometry.source.left + (clipped.left - geometry.destination.left) * scaleX,
        geometry.source.top + (clipped.top - geometry.destination.top) * scaleY,
        geometry.source.right - (geometry.destination.right - clipped.right) * scaleX,
        geometry.source.bottom - (geometry.destination.bottom - clipped.bottom) * scaleY};
    geometry.destination = clipped;
    const RECT source = ThumbnailRect(geometry.source);
    const RECT destination = ThumbnailRect(geometry.destination);
    return source.right > source.left && source.bottom > source.top &&
           destination.right > destination.left && destination.bottom > destination.top;
}

inline void HideThumbnail(Card& card, DWORD now) {
    DWM_THUMBNAIL_PROPERTIES properties = {};
    properties.dwFlags = DWM_TNP_VISIBLE;
    properties.fVisible = FALSE;
    if (FAILED(DwmUpdateThumbnailProperties(card.thumbnail.get(), &properties))) {
        FailThumbnail(card, now);
        return;
    }
    card.thumbnailVisible = false;
    card.thumbnailDestination = {};
}

inline void SubmitThumbnail(Card& card, const ThumbnailGeometry& geometry,
                            DWORD now) {
    DWM_THUMBNAIL_PROPERTIES properties = {};
    properties.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE |
                         DWM_TNP_VISIBLE | DWM_TNP_OPACITY |
                         DWM_TNP_SOURCECLIENTAREAONLY;
    properties.rcDestination = ThumbnailRect(geometry.destination);
    properties.rcSource = ThumbnailRect(geometry.source);
    properties.fVisible = TRUE;
    properties.opacity = 255;
    properties.fSourceClientAreaOnly = FALSE;
    if (FAILED(DwmUpdateThumbnailProperties(card.thumbnail.get(), &properties))) {
        FailThumbnail(card, now);
        return;
    }
    card.thumbnailDestination = properties.rcDestination;
    card.thumbnailVisible = true;
}

inline bool CanPrepareThumbnail(Card& card, DWORD now) {
    const bool stackAnimating = std::any_of(
        context.stackTransitions.begin(), context.stackTransitions.end(),
        [&card](const StackCycleTransition& transition) {
            return transition.groupKey == card.stack.groupKey;
        });
    if (stackAnimating) {
        if (card.thumbnail.valid() && card.thumbnailVisible) {
            HideThumbnail(card, now);
        }
        return false;
    }
    // DWM thumbnails cannot be rotated or scaled, and a stack shows only
    // its front window live. Background members render as icon cards, so
    // never register or show a thumbnail for them.
    if (IsStackBackground(card)) {
        if (card.thumbnail.valid()) {
            HideThumbnail(card, now);
        }
        return false;
    }
    return EnsureThumbnail(card, now);
}

inline bool PrepareThumbnail(Card& card, DWORD now,
                             ThumbnailGeometry& geometry) {
    if (!CanPrepareThumbnail(card, now)) {
        return false;
    }
    geometry.destination = ThumbnailDestinationFor(card);
    if (ThumbnailViewportClip(geometry.destination).empty()) {
        HideThumbnail(card, now);
        return false;
    }
    if (!ReadThumbnailSource(card, geometry.source)) {
        FailThumbnail(card, now);
        return false;
    }
    if (!ClipThumbnailGeometry(geometry, ThumbnailViewportClip(geometry.destination))) {
        HideThumbnail(card, now);
        return false;
    }
    return true;
}

inline void UpdateCardThumbnail(Card& card, DWORD now,
                                const std::vector<RectF>& higherThumbnails) {
    ThumbnailGeometry geometry;
    if (!PrepareThumbnail(card, now, geometry)) {
        return;
    }
    const RECT destination = ThumbnailRect(geometry.destination);
    const RectF visible = UnoccludedThumbnailRect(RectFromRect(destination), higherThumbnails);
    const RECT cropped = ThumbnailRect(visible);
    if (EqualRect(&destination, &cropped) == FALSE &&
        !ClipThumbnailGeometry(geometry, visible)) {
        if (card.thumbnailVisible) {
            HideThumbnail(card, now);
        }
        return;
    }
    SubmitThumbnail(card, geometry, now);
}

inline void UpdateThumbnails() {
    const DWORD now = GetTickCount();
    const auto drawOrder = CardDrawOrder(context.cards);
    std::vector<RectF> higherThumbnails;
    higherThumbnails.reserve(drawOrder.size());
    for (auto index = drawOrder.rbegin(); index != drawOrder.rend(); ++index) {
        Card& card = context.cards[*index];
        UpdateCardThumbnail(card, now, higherThumbnails);
        if (card.thumbnailVisible) {
            higherThumbnails.push_back(RectFromRect(card.thumbnailDestination));
        }
    }
}

inline void ReleaseAllThumbnails() {
    for (Card& card : context.cards) {
        ReleaseThumbnail(card);
    }
}


inline COLORREF ColorRef(uint32_t color) {
    return RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

inline void DrawHostScene(HDC dc) {
    RECT client = {0, 0, context.virtualScreen.right - context.virtualScreen.left,
                   context.virtualScreen.bottom - context.virtualScreen.top};
    HBRUSH background = CreateSolidBrush(ColorRef(context.settings.backgroundColor));
    FillRect(dc, &client, background);
    DeleteObject(background);
    if (!context.settings.showGrid) {
        return;
    }
    const float spacing = static_cast<float>(context.settings.gridSpacing) *
                          context.camera.zoom;
    if (spacing < kMinGridSpacingPx) {
        return;
    }
    HPEN pen = CreatePen(PS_SOLID, 1, ColorRef(context.settings.gridColor));
    HGDIOBJ oldPen = SelectObject(dc, pen);
    const RectF view = ViewportWorld(context.camera);
    const float startX = std::floor(view.left / context.settings.gridSpacing) *
                         context.settings.gridSpacing;
    const float startY = std::floor(view.top / context.settings.gridSpacing) *
                         context.settings.gridSpacing;
    const int lineCountX = std::min(kMaxGridLines, static_cast<int>(
        std::ceil(static_cast<float>(context.virtualScreen.right - context.virtualScreen.left) / spacing) + 2.0f));
    const int lineCountY = std::min(kMaxGridLines, static_cast<int>(
        std::ceil(static_cast<float>(context.virtualScreen.bottom - context.virtualScreen.top) / spacing) + 2.0f));
    for (int index = 0; index < lineCountX; ++index) {
        const float worldX = startX + static_cast<float>(index) * context.settings.gridSpacing;
        const float screenX = (worldX - context.camera.centerX) * context.camera.zoom +
                              static_cast<float>(client.right) * 0.5f;
        MoveToEx(dc, static_cast<int>(screenX), 0, nullptr);
        LineTo(dc, static_cast<int>(screenX), client.bottom);
    }
    for (int index = 0; index < lineCountY; ++index) {
        const float worldY = startY + static_cast<float>(index) * context.settings.gridSpacing;
        const float screenY = (worldY - context.camera.centerY) * context.camera.zoom +
                              static_cast<float>(client.bottom) * 0.5f;
        MoveToEx(dc, 0, static_cast<int>(screenY), nullptr);
        LineTo(dc, client.right, static_cast<int>(screenY));
    }
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}


inline void SetHudColor(uint32_t rgb, float alpha = 1.0f) {
    context.hudBrush->SetColor(ToColorF(rgb, alpha));
}

inline D2D1_ROUNDED_RECT RoundedRect(const RectF& rect, float radius) {
    const float fitted = std::max(
        0.0f, std::min(radius, std::min(rect.width(), rect.height()) * 0.5f));
    return D2D1::RoundedRect(D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom),
                             fitted, fitted);
}

inline void DrawCardTitle(const Card& card, const RectF& rect, float scale) {
    const float padding = kCardTitlePaddingPx * scale;
    if (context.titleFormat == nullptr || rect.width() <= padding * 2.0f ||
        rect.empty()) {
        return;
    }
    const D2D1_RECT_F titleRect = D2D1::RectF(
        rect.left + padding, rect.top, rect.right - padding, rect.bottom);
    SetHudColor(context.settings.textColor);
    context.hudTarget->DrawTextW(card.title.c_str(),
                                 static_cast<UINT32>(card.title.size()),
                                 context.titleFormat, titleRect,
                                 context.hudBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

inline void DrawHeaderButtonBackdrop(const RectF& rect, bool highlighted,
                                     float scale) {
    if (rect.empty() || !highlighted) {
        return;
    }
    SetHudColor(context.settings.accentColor, 0.22f);
    context.hudTarget->FillRoundedRectangle(RoundedRect(rect, 4.0f * scale),
                                            context.hudBrush);
}

inline void DrawCloseGlyph(const RectF& rect, float scale) {
    if (rect.empty()) {
        return;
    }
    const float inset = rect.width() * 0.3f;
    SetHudColor(context.settings.textColor, 0.9f);
    context.hudTarget->DrawLine(
        D2D1::Point2F(rect.left + inset, rect.top + inset),
        D2D1::Point2F(rect.right - inset, rect.bottom - inset),
        context.hudBrush, 1.2f * scale);
    context.hudTarget->DrawLine(
        D2D1::Point2F(rect.right - inset, rect.top + inset),
        D2D1::Point2F(rect.left + inset, rect.bottom - inset),
        context.hudBrush, 1.2f * scale);
}

inline void DrawPinGlyph(const RectF& rect, bool pinned, float scale) {
    if (rect.empty()) {
        return;
    }
    const float centerX = rect.centerX();
    const float head = rect.top + rect.height() * 0.24f;
    const float shoulder = rect.top + rect.height() * 0.58f;
    const float needle = rect.top + rect.height() * 0.78f;
    const float halfWidth = rect.width() * 0.22f;
    const float stroke = (pinned ? 1.6f : 1.1f) * scale;
    SetHudColor(pinned ? context.settings.accentColor : context.settings.textColor,
                pinned ? 1.0f : 0.85f);
    context.hudTarget->DrawLine(D2D1::Point2F(centerX, head),
                                D2D1::Point2F(centerX - halfWidth, shoulder),
                                context.hudBrush, stroke);
    context.hudTarget->DrawLine(D2D1::Point2F(centerX, head),
                                D2D1::Point2F(centerX + halfWidth, shoulder),
                                context.hudBrush, stroke);
    context.hudTarget->DrawLine(
        D2D1::Point2F(centerX - halfWidth, shoulder),
        D2D1::Point2F(centerX + halfWidth, shoulder), context.hudBrush, stroke);
    context.hudTarget->DrawLine(D2D1::Point2F(centerX, shoulder),
                                D2D1::Point2F(centerX, needle),
                                context.hudBrush, stroke);
}

// Two overlapping squares read as "these windows are grouped".
inline void DrawGroupedGlyph(const RectF& rect, float stroke) {
    const float width = rect.width();
    const float height = rect.height();
    context.hudTarget->DrawRectangle(
        D2D1::RectF(rect.left + width * 0.18f, rect.top + height * 0.18f,
                    rect.left + width * 0.62f, rect.top + height * 0.62f),
        context.hudBrush, stroke);
    context.hudTarget->DrawRectangle(
        D2D1::RectF(rect.left + width * 0.38f, rect.top + height * 0.38f,
                    rect.left + width * 0.82f, rect.top + height * 0.82f),
        context.hudBrush, stroke);
}

// Two separated squares read as "these windows can still be grouped".
inline void DrawSplitGlyph(const RectF& rect, float stroke) {
    const float width = rect.width();
    const float height = rect.height();
    context.hudTarget->DrawRectangle(
        D2D1::RectF(rect.left + width * 0.12f, rect.top + height * 0.28f,
                    rect.left + width * 0.46f, rect.top + height * 0.72f),
        context.hudBrush, stroke);
    context.hudTarget->DrawRectangle(
        D2D1::RectF(rect.left + width * 0.54f, rect.top + height * 0.28f,
                    rect.left + width * 0.88f, rect.top + height * 0.72f),
        context.hudBrush, stroke);
}

inline void DrawGroupGlyph(const RectF& rect, bool grouped, float scale) {
    if (rect.empty()) {
        return;
    }
    SetHudColor(grouped ? context.settings.accentColor
                        : context.settings.textColor,
                grouped ? 1.0f : 0.85f);
    const float stroke = 1.2f * scale;
    if (grouped) {
        DrawGroupedGlyph(rect, stroke);
        return;
    }
    DrawSplitGlyph(rect, stroke);
}
// The header geometry is resolved once and then shared by the title, the
// backdrops and the glyphs, so the three can never disagree about where a
// button begins.
struct CardHeaderRow {
    RectF group;
    RectF pin;
    RectF close;
    float scale = 1.0f;
};

[[nodiscard]] inline CardHeaderRow CardHeaderRowFor(const RectF& header,
                                                    float scale, int buttons) {
    return CardHeaderRow{CardGroupButtonRect(header, scale, buttons),
                         CardPinButtonRect(header, scale, buttons),
                         CardCloseButtonRect(header, scale, buttons), scale};
}

// The title stops before the leftmost button, so it never slides under one.
[[nodiscard]] inline RectF CardHeaderTitleRect(const RectF& header,
                                               const CardHeaderRow& row) {
    const RectF titleLimit = row.group.empty() ? row.pin : row.group;
    if (titleLimit.empty()) {
        return header;
    }
    RectF title = header;
    title.right = titleLimit.left;
    return title;
}

// The row is drawn from the right edge inwards: close, then pin, then the
// group button when the card has one. Each backdrop lights up under the
// cursor, so the tooltip drawn above always names the button it points at.
inline void DrawCardHeaderButtons(const Card& card, const CardHeaderRow& row,
                                  CardHeaderAction hovered) {
    DrawHeaderButtonBackdrop(row.group, hovered == CardHeaderAction::Group,
                             row.scale);
    DrawHeaderButtonBackdrop(row.pin, hovered == CardHeaderAction::Pin,
                             row.scale);
    DrawHeaderButtonBackdrop(row.close, hovered == CardHeaderAction::Close,
                             row.scale);
    DrawGroupGlyph(row.group, IsGroupedStackCard(card), row.scale);
    DrawPinGlyph(row.pin, card.pinned, row.scale);
    DrawCloseGlyph(row.close, row.scale);
}

inline void DrawCardHeader(const Card& card, int index, const RectF& header,
                           float scale) {
    if (header.empty()) {
        return;
    }
    SetHudColor(context.settings.backgroundColor, 0.9f);
    context.hudTarget->FillRectangle(
        D2D1::RectF(header.left, header.top, header.right, header.bottom),
        context.hudBrush);
    const int buttons = CardHeaderButtonCountForWidth(
        header, scale,
        CardHeaderButtonCount(context.cards, static_cast<size_t>(index),
                              context.settings.stacksEnabled));
    const CardHeaderRow row = CardHeaderRowFor(header, scale, buttons);
    DrawCardTitle(card, CardHeaderTitleRect(header, row), scale);
    const CardHeaderAction hovered =
        index == context.hoverCard ? context.hoverHeaderAction
                                   : CardHeaderAction::None;
    DrawCardHeaderButtons(card, row, hovered);
}

// Tooltips are drawn by the HUD itself: the HUD is a layered click-through
// window, so a native Win32 tooltip on it would never receive hover input.
struct TooltipMetrics {
    float width = 0.0f;
    float height = 0.0f;
};

inline bool MeasureTooltipText(const wchar_t* text, size_t length,
                               TooltipMetrics& metrics) {
    metrics = TooltipMetrics{};
    if (text == nullptr || context.writeFactory == nullptr ||
        context.tooltipFormat == nullptr || length == 0) {
        return false;
    }
    IDWriteTextLayout* layout = nullptr;
    const HRESULT created = context.writeFactory->CreateTextLayout(
        text, static_cast<UINT32>(length), context.tooltipFormat,
        static_cast<float>(VirtualScreenWidth()),
        static_cast<float>(VirtualScreenHeight()), &layout);
    if (FAILED(created) || layout == nullptr) {
        return false;
    }
    DWRITE_TEXT_METRICS measured = {};
    const HRESULT result = layout->GetMetrics(&measured);
    // CreateTextLayout hands over the only reference, so it is released on
    // both the success and the failure path.
    layout->Release();
    if (FAILED(result)) {
        return false;
    }
    metrics.width = measured.width;
    metrics.height = measured.height;
    return true;
}

[[nodiscard]] inline CanvasTextId HeaderTooltipText(const Card& card) {
    switch (context.hoverHeaderAction) {
        case CardHeaderAction::Close:
            return CanvasTextId::CloseTooltip;
        case CardHeaderAction::Pin:
            return card.pinned ? CanvasTextId::PinActiveTooltip
                               : CanvasTextId::PinTooltip;
        case CardHeaderAction::Group:
            return IsGroupedStackCard(card) ? CanvasTextId::UngroupTooltip
                                            : CanvasTextId::GroupTooltip;
        case CardHeaderAction::None:
            break;
    }
    return CanvasTextId::PinTooltip;
}

[[nodiscard]] inline RectF HeaderActionRect(const RectF& header, float scale,
                                            CardHeaderAction action,
                                            int buttons) {
    switch (action) {
        case CardHeaderAction::Close:
            return CardCloseButtonRect(header, scale, buttons);
        case CardHeaderAction::Pin:
            return CardPinButtonRect(header, scale, buttons);
        case CardHeaderAction::Group:
            return CardGroupButtonRect(header, scale, buttons);
        case CardHeaderAction::None:
            break;
    }
    return {};
}

// Defaults above the button; flips below and clamps horizontally so a button
// near a screen edge keeps its label fully visible.
[[nodiscard]] inline RectF PlaceTooltipBox(const RectF& anchor,
                                           const TooltipMetrics& metrics,
                                           float scale) {
    const float margin = 4.0f * scale;
    const float gap = 6.0f * scale;
    const float padding = 6.0f * scale;
    const float width = metrics.width + padding * 2.0f;
    // Padding on all four sides; a single-sided value would leave the label
    // flush against the top edge of its own box.
    const float height = metrics.height + padding * 2.0f;
    const float screenWidth = static_cast<float>(VirtualScreenWidth());
    const float maxLeft = std::max(margin, screenWidth - margin - width);
    const float left =
        std::max(margin, std::min(anchor.centerX() - width * 0.5f, maxLeft));
    float top = anchor.top - gap - height;
    if (top < margin) {
        top = anchor.bottom + gap;
    }
    return MakeRectF(left, top, width, height);
}

// What the header tooltip needs to draw itself: the button it points at and
// the already localized label. An empty anchor means nothing is hovered.
struct HeaderTooltip {
    RectF anchor;
    const wchar_t* text = nullptr;
    size_t length = 0;
};

[[nodiscard]] inline HeaderTooltip ResolveHeaderTooltip(float scale) {
    HeaderTooltip tooltip;
    if (context.hoverHeaderAction == CardHeaderAction::None ||
        context.hoverCard < 0 ||
        static_cast<size_t>(context.hoverCard) >= context.cards.size()) {
        return tooltip;
    }
    const size_t cardIndex = static_cast<size_t>(context.hoverCard);
    const Card& card = context.cards[cardIndex];
    const RectF header = CardHeaderRect(card);
    const int buttons = CardHeaderButtonCountForWidth(
        header, scale,
        CardHeaderButtonCount(context.cards, cardIndex,
                              context.settings.stacksEnabled));
    tooltip.anchor =
        HeaderActionRect(header, scale, context.hoverHeaderAction, buttons);
    const CanvasTextId textId = HeaderTooltipText(card);
    tooltip.text = CanvasText(textId, context.settings.language);
    tooltip.length = CanvasTextLength(textId, context.settings.language);
    return tooltip;
}

inline void DrawTooltipBox(const RectF& box, const HeaderTooltip& tooltip,
                           float scale) {
    SetHudColor(context.settings.backgroundColor, 0.95f);
    context.hudTarget->FillRoundedRectangle(RoundedRect(box, 4.0f * scale),
                                            context.hudBrush);
    SetHudColor(context.settings.gridColor, 0.9f);
    context.hudTarget->DrawRoundedRectangle(RoundedRect(box, 4.0f * scale),
                                            context.hudBrush, 1.0f * scale);
    const float padding = 6.0f * scale;
    SetHudColor(context.settings.textColor);
    context.hudTarget->DrawTextW(
        tooltip.text, static_cast<UINT32>(tooltip.length), context.tooltipFormat,
        D2D1::RectF(box.left + padding, box.top + padding, box.right - padding,
                    box.bottom - padding),
        context.hudBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

inline void DrawHeaderTooltip(float scale) {
    const HeaderTooltip tooltip = ResolveHeaderTooltip(scale);
    if (tooltip.anchor.empty() || tooltip.text == nullptr) {
        return;
    }
    TooltipMetrics metrics;
    if (!MeasureTooltipText(tooltip.text, tooltip.length, metrics)) {
        return;
    }
    DrawTooltipBox(PlaceTooltipBox(tooltip.anchor, metrics, scale), tooltip,
                   scale);
}

inline RectF CardBadgeBounds(const RectF& body, float scale) {
    if (context.badgeFormat == nullptr) {
        return {};
    }
    const float padding = kCardTitlePaddingPx * scale;
    const float height = context.badgeFormat->GetFontSize() * 2.0f;
    if (body.width() <= padding * 2.0f ||
        body.height() <= height + padding * 2.0f) {
        return {};
    }
    return RectF{body.left + padding, body.bottom - padding - height,
                 body.right - padding, body.bottom - padding};
}

inline void DrawCardIcon(const Card& card, const RectF& area) {
    const float size = std::min(
        kCardIconMaxPx * ChromeScale(), std::min(area.width(), area.height()));
    if (area.empty() || size <= 0.0f) {
        return;
    }
    ID2D1Bitmap* icon =
        LoadIconBitmap(card.exePath, card.windowClass, card.hwnd);
    if (icon == nullptr) {
        return;
    }
    const float half = size * 0.5f;
    context.hudTarget->DrawBitmap(
        icon, D2D1::RectF(area.centerX() - half, area.centerY() - half,
                          area.centerX() + half, area.centerY() + half),
        0.9f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
}

[[nodiscard]] inline CanvasTextId CardBadgeTextId(WindowVisualState state) {
    switch (state) {
        case WindowVisualState::Minimized:
            return CanvasTextId::MinimizedBadge;
        case WindowVisualState::OtherDesktop:
            return CanvasTextId::OtherDesktopBadge;
        case WindowVisualState::Live:
            break;
    }
    return CanvasTextId::PreviewUnavailableBadge;
}

inline void DrawCardBadge(const Card& card, const RectF& rect) {
    if (rect.empty() || context.badgeFormat == nullptr) {
        return;
    }
    const CanvasTextId badgeId = CardBadgeTextId(card.state);
    SetHudColor(context.settings.mutedTextColor);
    context.hudTarget->DrawTextW(
        CanvasText(badgeId, context.settings.language),
        static_cast<UINT32>(
            CanvasTextLength(badgeId, context.settings.language)),
        context.badgeFormat,
        D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom),
        context.hudBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

inline bool CardShowsBadge(const Card& card) {
    return card.state != WindowVisualState::Live ||
           (card.thumbnailFailed && !IsStackBackground(card));
}

inline void DrawCardBody(const Card& card, const RectF& body, float scale) {
    if (body.empty()) {
        return;
    }
    const float radius = kCardCornerRadiusPx * scale;
    SetHudColor(context.settings.cardFillColor);
    // A live preview covers the body, so only the letterbox bands around a
    // window whose aspect differs from the stack frame keep the card fill.
    // The bands are clipped to the rounded body so the fill never leaks past
    // the card's corners.
    if (card.state == WindowVisualState::Live && card.thumbnail.valid() &&
        card.thumbnailVisible) {
        const RectF destination = RectFromRect(card.thumbnailDestination);
        if (!destination.empty()) {
            const std::vector<RectF> bands = SubtractCardOcclusionHoles(
                body, CardOcclusionHoles(body, body, {destination}));
            for (const RectF& band : bands) {
                context.hudTarget->PushAxisAlignedClip(
                    D2D1::RectF(band.left, band.top, band.right, band.bottom),
                    D2D1_ANTIALIAS_MODE_ALIASED);
                context.hudTarget->FillRoundedRectangle(RoundedRect(body, radius),
                                                       context.hudBrush);
                context.hudTarget->PopAxisAlignedClip();
            }
            return;
        }
    }
    context.hudTarget->FillRoundedRectangle(
        RoundedRect(body, radius), context.hudBrush);
    const RectF badge = CardShowsBadge(card) ? CardBadgeBounds(body, scale) : RectF{};
    const float padding = kCardTitlePaddingPx * scale;
    RectF iconArea = GrowRect(body, -padding);
    if (!badge.empty()) {
        iconArea.bottom = badge.top - padding;
    }
    DrawCardIcon(card, iconArea);
    DrawCardBadge(card, badge);
}

inline void DrawCard(const Card& card, int index, float scale) {
    if (card.stack.opacity <= kCardOpacityEpsilon) {
        return;
    }
    const RectF rect = CardScreenBounds(card);
    if (rect.empty()) {
        return;
    }
    // Cull on the rotated/scaled quad, not the unrotated bounds: a fanned
    // stack member's corners extend past its own rectangle.
    const std::array<Vec2, 4> quad = CardScreenQuad(rect, card.stack);
    const RectF quadBounds = CardQuadBounds(quad);
    if (quadBounds.right < 0.0f || quadBounds.bottom < 0.0f ||
        quadBounds.left > static_cast<float>(VirtualScreenWidth()) ||
        quadBounds.top > static_cast<float>(VirtualScreenHeight())) {
        return;
    }
    const float radius = kCardCornerRadiusPx * scale;
    // A card that would take an Alt+drop is highlighted like the hovered card,
    // so the merge target is visible before the button comes up.
    const bool emphasized = index == context.hoverCard ||
                            index == context.selectedCard ||
                            index == context.dropTargetCard;
    const RectF body = WorldToScreen(StackWorldRect(card), context.camera);
    const RectF header = CardHeaderRect(card);
    const bool transformed = card.stack.rotationRadians != 0.0f ||
                             card.stack.scale != 1.0f;
    D2D1_MATRIX_3X2_F savedTransform = D2D1::Matrix3x2F::Identity();
    if (transformed) {
        const Vec2 center{rect.centerX(), rect.centerY()};
        const float degrees = card.stack.rotationRadians *
                              (180.0f / 3.141592653589793f);
        context.hudTarget->GetTransform(&savedTransform);
        context.hudTarget->SetTransform(
            D2D1::Matrix3x2F::Scale(card.stack.scale, card.stack.scale,
                                     D2D1::Point2F(center.x, center.y)) *
            D2D1::Matrix3x2F::Rotation(degrees, D2D1::Point2F(center.x, center.y)));
    }
    const bool translucent = card.stack.opacity < 1.0f;
    if (translucent) {
        const RectF bounds = GrowRect(rect, scale + 1.0f);
        const auto layer = D2D1::LayerParameters(
            D2D1::RectF(bounds.left, bounds.top, bounds.right, bounds.bottom),
            nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(),
            card.stack.opacity);
        context.hudTarget->PushLayer(&layer, nullptr);
    }
    DrawCardBody(card, body, scale);
    DrawCardHeader(card, index, header, scale);
    SetHudColor(emphasized ? context.settings.accentColor : context.settings.gridColor);
    context.hudTarget->DrawRoundedRectangle(
        RoundedRect(rect, radius), context.hudBrush,
        emphasized ? 2.0f * scale : 1.0f * scale);
    if (translucent) {
        context.hudTarget->PopLayer();
    }
    if (transformed) {
        context.hudTarget->SetTransform(savedTransform);
    }
}

// A live preview is composited inside the host window, which sits below the HUD
// layer, so HUD artwork is always above every preview. A card may therefore
// only yield the preview rectangles of cards painted after it: reserving a
// lower card's preview would punch that card through this one, and the upright
// card at the front of a stack would show the cards stacked beneath it.
inline std::vector<RectF> HigherThumbnailRects(
    const std::vector<Card>& cards, const std::vector<size_t>& order,
    size_t position) {
    std::vector<RectF> thumbnails;
    for (size_t slot = position + 1; slot < order.size(); ++slot) {
        const size_t index = order[slot];
        if (index >= cards.size()) {
            continue;
        }
        const Card& card = cards[index];
        if (card.state != WindowVisualState::Live || !card.thumbnail.valid() ||
            !card.thumbnailVisible) {
            continue;
        }
        const RectF destination = RectFromRect(card.thumbnailDestination);
        if (!destination.empty()) {
            thumbnails.push_back(destination);
        }
    }
    return thumbnails;
}

// One card's paint request: the card, its index for hover/selection emphasis,
// and the chrome scale shared by every card in the frame.
struct HudCardPaint {
    const Card& card;
    int index;
    float scale;
};

inline void DrawCardWithThumbnailClips(const HudCardPaint& paint,
                                       const CardDrawClips& clips) {
    if (!clips.clipped) {
        DrawCard(paint.card, paint.index, paint.scale);
        return;
    }
    for (const RectF& region : clips.regions) {
        context.hudTarget->PushAxisAlignedClip(
            D2D1::RectF(region.left, region.top, region.right, region.bottom),
            D2D1_ANTIALIAS_MODE_ALIASED);
        DrawCard(paint.card, paint.index, paint.scale);
        context.hudTarget->PopAxisAlignedClip();
    }
}

inline CardDrawClips CardThumbnailDrawClips(
    const std::vector<Card>& cards, const std::vector<size_t>& order,
    size_t position) {
    const size_t index = order[position];
    if (index >= cards.size()) {
        return CardDrawClips{};
    }
    const RectF bounds = CardScreenBounds(cards[index]);
    const std::array<Vec2, 4> quad = CardScreenQuad(bounds, cards[index].stack);
    const RectF viewport{0.0f, 0.0f, static_cast<float>(VirtualScreenWidth()),
                         static_cast<float>(VirtualScreenHeight())};
    return CardDrawRegions(viewport, quad,
                           HigherThumbnailRects(cards, order, position));
}

inline void DrawMinimapContents(const RectF& map, float scale) {
    const RectF bounds = CanvasWorldBounds();
    const RectF inner = MinimapInnerRect(map);
    if (bounds.empty() || inner.empty()) {
        return;
    }
    context.hudTarget->PushAxisAlignedClip(
        D2D1::RectF(inner.left, inner.top, inner.right, inner.bottom),
        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    for (const Card& card : context.cards) {
        const RectF projected = MinimapWorldToScreen(StackWorldRect(card), map, bounds);
        SetHudColor(card.state == WindowVisualState::Live
                        ? context.settings.accentColor
                        : context.settings.mutedTextColor);
        context.hudTarget->FillRectangle(
            D2D1::RectF(projected.left, projected.top, projected.right, projected.bottom),
            context.hudBrush);
    }
    const RectF view = MinimapWorldToScreen(ViewportWorld(context.camera), map, bounds);
    SetHudColor(context.settings.textColor, 0.85f);
    context.hudTarget->DrawRectangle(
        D2D1::RectF(view.left, view.top, view.right, view.bottom), context.hudBrush,
        1.0f * scale);
    context.hudTarget->PopAxisAlignedClip();
}

inline void DrawMinimap(float scale) {
    context.minimapRect = {};
    if (!context.settings.minimapEnabled) {
        return;
    }
    const RectF map = CalculateMinimapRect();
    if (map.empty()) {
        return;
    }
    context.minimapRect = map;
    SetHudColor(context.settings.backgroundColor, 0.9f);
    context.hudTarget->FillRoundedRectangle(RoundedRect(map, 6.0f * scale),
                                            context.hudBrush);
    SetHudColor(context.settings.gridColor);
    context.hudTarget->DrawRoundedRectangle(RoundedRect(map, 6.0f * scale),
                                            context.hudBrush, 1.0f * scale);
    DrawMinimapContents(map, scale);
}

inline void DrawEmptyCanvas(float scale) {
    if (!context.cards.empty()) {
        return;
    }
    const D2D1_RECT_F emptyRect = D2D1::RectF(
        24.0f * scale, 24.0f * scale,
        static_cast<float>(VirtualScreenWidth()) - 24.0f * scale,
        static_cast<float>(VirtualScreenHeight()) - 24.0f * scale);
    SetHudColor(context.settings.mutedTextColor);
    context.hudTarget->DrawTextW(
        CanvasText(CanvasTextId::EmptyCanvas, context.settings.language),
        CanvasTextLength(CanvasTextId::EmptyCanvas, context.settings.language),
        context.titleFormat,
        emptyRect, context.hudBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

inline void DrawAlignmentGuides(float scale) {
    if (context.hasGuideVertical) {
        SetHudColor(context.settings.accentColor, 0.8f);
        context.hudTarget->DrawLine(D2D1::Point2F(context.guideVertical.left, 0.0f),
                                    D2D1::Point2F(context.guideVertical.left,
                                                  static_cast<float>(VirtualScreenHeight())),
                                    context.hudBrush, 1.0f * scale);
    }
    if (context.hasGuideHorizontal) {
        SetHudColor(context.settings.accentColor, 0.8f);
        context.hudTarget->DrawLine(D2D1::Point2F(0.0f, context.guideHorizontal.top),
                                    D2D1::Point2F(static_cast<float>(VirtualScreenWidth()),
                                                  context.guideHorizontal.top),
                                    context.hudBrush, 1.0f * scale);
    }
}

inline bool PresentHudFrame() {
    POINT destination = {context.virtualScreen.left, context.virtualScreen.top};
    POINT origin = {0, 0};
    SIZE size = {VirtualScreenWidth(), VirtualScreenHeight()};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    return UpdateLayeredWindow(context.hudWindow, nullptr, &destination, &size,
                               context.hudDc, &origin, 0, &blend, ULW_ALPHA) != FALSE;
}

inline bool SubmitHudFrame() {
    const int width = VirtualScreenWidth();
    const int height = VirtualScreenHeight();
    if (!EnsureGraphics() || !CreateHudSurface(width, height)) {
        return false;
    }
    const RECT bounds = {0, 0, width, height};
    if (FAILED(context.hudTarget->BindDC(context.hudDc, &bounds))) {
        ReleaseHudSurface();
        return false;
    }
    context.hudTarget->BeginDraw();
    context.hudTarget->Clear(D2D1::ColorF(0, 0.0f));
    const float scale = ChromeScale();
    const std::vector<size_t> order = CardDrawOrder(context.cards);
    for (size_t position = 0; position < order.size(); ++position) {
        const size_t index = order[position];
        const CardDrawClips clips =
            CardThumbnailDrawClips(context.cards, order, position);
        DrawCardWithThumbnailClips(
            HudCardPaint{context.cards[index], static_cast<int>(index), scale},
            clips);
    }
    DrawEmptyCanvas(scale);
    DrawMinimap(scale);
    DrawAlignmentGuides(scale);
    DrawHeaderTooltip(scale);
    if (FAILED(context.hudTarget->EndDraw())) {
        ReleaseHudSurface();
        return false;
    }
    return PresentHudFrame();
}



inline RECT ReadVirtualScreen() {
    const int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return RECT{left, top, left + width, top + height};
}

inline void PlaceCanvasWindows() {
    context.virtualScreen = ReadVirtualScreen();
    const HWND windows[2] = {context.hostWindow, context.hudWindow};
    for (const HWND window : windows) {
        if (window == nullptr) {
            continue;
        }
        SetWindowPos(window, HWND_TOPMOST, context.virtualScreen.left,
                     context.virtualScreen.top, VirtualScreenWidth(),
                     VirtualScreenHeight(), SWP_NOACTIVATE);
    }
    ReleaseHudSurface();
    context.sceneDirty = true;
    context.scanRequested = true;
}

inline void CALLBACK OnWindowDestroyed(HWINEVENTHOOK, DWORD, HWND window,
                                       LONG objectId, LONG childId, DWORD,
                                       DWORD) {
    if (objectId != OBJID_WINDOW || childId != CHILDID_SELF) {
        return;
    }
    for (Card& card : context.cards) {
        if (card.hwnd == window) {
            card.destroyed = true;
            card.thumbnail.reset();
            context.scanRequested = true;
            context.sceneDirty = true;
        }
    }
}

inline void InstallDestroyHook() {
    if (context.destroyHook != nullptr) {
        return;
    }
    context.destroyHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr, OnWindowDestroyed,
        0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (context.destroyHook == nullptr) {
        Wh_Log(L"Aleph: SetWinEventHook failed: %lu", GetLastError());
    }
}

inline void UninstallDestroyHook() {
    if (context.destroyHook == nullptr) {
        return;
    }
    UnhookWinEvent(context.destroyHook);
    context.destroyHook = nullptr;
}

inline void HideCanvasWindows() {
    context.visible = false;
    UninstallDestroyHook();
    if (GetCapture() == context.hostWindow) {
        ReleaseCapture();
    }
    if (context.hudWindow != nullptr) {
        ShowWindow(context.hudWindow, SW_HIDE);
    }
    if (context.hostWindow != nullptr) {
        ShowWindow(context.hostWindow, SW_HIDE);
    }
    ReleaseAllThumbnails();
    context.panning = false;
    context.draggingMinimap = false;
    context.draggedCard = -1;
    context.diving = false;
    context.hasGuideVertical = false;
    context.hasGuideHorizontal = false;
    context.hoverCard = -1;
    context.minimapHover = false;
    context.camera.snapToTarget();
    context.sceneDirty = true;
}

inline void ShowCanvasWindows() {
    if (context.visible) {
        return;
    }
    if (UpdateChromeDpi()) {
        ReleaseTextFormats();
    }
    PlaceCanvasWindows();
    if (!context.layoutLoaded) {
        LoadLayout();
        context.layoutLoaded = true;
    }
    context.visible = true;
    context.scanRequested = true;
    context.lastFrameTick = GetTickCount();
    SetForegroundWindow(context.hostWindow);
    ShowWindow(context.hostWindow, SW_SHOWNOACTIVATE);
    ShowWindow(context.hudWindow, SW_SHOWNOACTIVATE);
    SetForegroundWindow(context.hostWindow);
    SetFocus(context.hostWindow);
    context.sceneDirty = true;
    InstallDestroyHook();
}

inline void ToggleCanvas() {
    if (context.visible) {
        HideCanvasWindows();
        return;
    }
    ShowCanvasWindows();
}

// Moving an other-desktop window onto the desktop that shows the canvas.
//
// `IVirtualDesktopManager::MoveWindowToDesktop` needs the GUID of the target
// desktop, and the documented way to obtain one is `GetWindowDesktopId`. The
// canvas host window is the only window the mod owns that the user is looking
// at when activating a card, so its desktop identifier is the pull target.


enum class DesktopPullResult {
    // No usable manager, handle or reference; the window stays where it is.
    Unavailable,
    // The window already lives on the target desktop.
    AlreadyHere,
    // The move was accepted.
    Moved,
    // The move was requested and refused.
    Rejected,
};

struct DesktopPullOutcome {
    DesktopPullResult result = DesktopPullResult::Unavailable;
    HRESULT error = S_OK;
};

inline DesktopPullOutcome PullWindowToDesktop(IVirtualDesktopManager* manager,
                                              HWND window, HWND reference) {
    DesktopPullOutcome outcome;
    if (manager == nullptr || window == nullptr || reference == nullptr) {
        return outcome;
    }
    GUID target = {};
    if (FAILED(manager->GetWindowDesktopId(reference, &target))) {
        return outcome;
    }
    GUID current = {};
    if (SUCCEEDED(manager->GetWindowDesktopId(window, &current)) &&
        IsEqualGUID(current, target) != FALSE) {
        outcome.result = DesktopPullResult::AlreadyHere;
        return outcome;
    }
    // An unreadable source desktop is not proof that no move is needed, so only
    // a confirmed match skips the request.
    outcome.error = manager->MoveWindowToDesktop(window, target);
    outcome.result = SUCCEEDED(outcome.error) ? DesktopPullResult::Moved
                                              : DesktopPullResult::Rejected;
    return outcome;
}

#include <optional>

inline POINT CursorInCanvas() {
    POINT point = {};
    GetCursorPos(&point);
    point.x -= context.virtualScreen.left;
    point.y -= context.virtualScreen.top;
    return point;
}

inline int HitTestCard(Vec2 point) {
    return HitTestCardInOrder(context.cards, point);
}

// Alt is the modifier that turns a plain drag into a stack gesture: it both
// pulls a card out of its fan and joins it to the card it is dropped on. Every
// handler reads it through here so the two halves always agree.
inline bool AltHeld() {
    return (GetKeyState(VK_MENU) & 0x8000) != 0;
}

inline CardHeaderAction HeaderActionAt(int index, Vec2 point) {
    if (index < 0 || static_cast<size_t>(index) >= context.cards.size()) {
        return CardHeaderAction::None;
    }
    const size_t cardIndex = static_cast<size_t>(index);
    const RectF header = CardHeaderRect(context.cards[cardIndex]);
    const float scale = ChromeScale();
    const int buttons = CardHeaderButtonCountForWidth(
        header, scale,
        CardHeaderButtonCount(context.cards, cardIndex,
                              context.settings.stacksEnabled));
    return HitTestCardHeader(header, point, scale, buttons);
}

inline void ToggleCardPin(int index) {
    if (index < 0 || static_cast<size_t>(index) >= context.cards.size()) {
        return;
    }
    Card& card = context.cards[static_cast<size_t>(index)];
    card.pinned = !card.pinned;
    RememberLayoutPin(card.fingerprint, StackWorldRect(card), card.pinned, card.stack.grouped);
    MarkLayoutDirty(GetTickCount());
    context.sceneDirty = true;
}

// A close request goes to the real window. The card only leaves the canvas
// once the next scan reports the window gone, so a document that refuses to
// close - a save prompt, for example - keeps its card instead of silently
// disappearing and reappearing the next time the canvas is shown.
inline void CloseCardWindow(int index) {
    if (index < 0 || static_cast<size_t>(index) >= context.cards.size()) {
        return;
    }
    const Card& card = context.cards[static_cast<size_t>(index)];
    const HWND window = card.hwnd;
    DWORD processId = 0;
    const DWORD threadId = card.destroyed || window == nullptr
                               ? 0 : GetWindowThreadProcessId(window, &processId);
    if (threadId == 0 || threadId != card.threadId || processId != card.processId) {
        RemoveCardAt(static_cast<size_t>(index));
        context.stackTransitions.clear();
        ReconcileCardStacks(context.cards, context.settings);
    } else if (PostMessageW(window, WM_CLOSE, 0, 0) == FALSE) {
        Wh_Log(L"Aleph: WM_CLOSE could not be posted: %lu", GetLastError());
        return;
    }
    context.hoverHeaderAction = CardHeaderAction::None;
    context.scanRequested = true;
    context.sceneDirty = true;
}

inline std::vector<size_t> CollectGroupMemberIndices(
    const std::wstring& groupKey) {
    std::vector<size_t> members;
    if (groupKey.empty()) {
        return members;
    }
    for (size_t index = 0; index < context.cards.size(); ++index) {
        const Card& card = context.cards[index];
        if (StackGroupKeyOf(card) == groupKey) {
            members.push_back(index);
        }
    }
    std::stable_sort(members.begin(), members.end(),
                     [](const size_t& left, const size_t& right) {
                         return context.cards[left].stack.slot <
                                context.cards[right].stack.slot;
                     });
    return members;
}

// The members that actually fan out of one stack, front first. A window of the
// same application that the user kept apart shares the key but is not part of
// the fan, so joining or splitting a stack never drags it in.
inline std::vector<size_t> CollectFannedMemberIndices(
    const std::wstring& groupKey) {
    std::vector<size_t> members;
    if (groupKey.empty()) {
        return members;
    }
    for (size_t index = 0; index < context.cards.size(); ++index) {
        const Card& card = context.cards[index];
        if (IsGroupedStackCard(card) && card.stack.groupKey == groupKey) {
            members.push_back(index);
        }
    }
    std::stable_sort(members.begin(), members.end(),
                     [](const size_t& left, const size_t& right) {
                         return context.cards[left].stack.slot <
                                context.cards[right].stack.slot;
                     });
    return members;
}

// The stack's own frame is the place the group occupies; members without one
// fall back to the first member's world rectangle.
inline RectF StackFrameForMembers(const std::vector<size_t>& members) {
    for (const size_t index : members) {
        if (index < context.cards.size() &&
            !context.cards[index].stack.frameWorld.empty()) {
            return context.cards[index].stack.frameWorld;
        }
    }
    if (members.empty() || members.front() >= context.cards.size()) {
        return RectF{};
    }
    return context.cards[members.front()].world;
}

// Grouping reuses the stack the canvas already knows: every member takes the
// key and the shared frame, then the regular reconcile fans them into slots.
inline void GroupCards(const std::vector<size_t>& members,
                       const std::wstring& groupKey) {
    if (members.size() < 2 || groupKey.empty()) {
        return;
    }
    const RectF frame = StackFrameForMembers(members);
    for (const size_t index : members) {
        if (index >= context.cards.size()) {
            continue;
        }
        Card& card = context.cards[index];
        ResetStackPlacement(card);
        card.stack.grouped = true;
        card.stack.groupKey = groupKey;
        card.stack.frameWorld = frame;
        // The grouped flag is saved too: a later rescan restores the stack
        // from the layout instead of splitting the windows apart again.
        RememberLayoutPin(card.fingerprint, StackWorldRect(card), card.pinned, true);
    }
    MarkLayoutDirty(GetTickCount());
}

// Ungrouping fans the members out in a row from where the stack stood, so each
// window becomes individually visible and draggable instead of jumping back to
// its own desktop position. Every released member takes back its own window
// size, so a card never keeps the proportions of the frame it shared. The
// ungrouped flag is saved with the layout so a rescan does not restack the
// windows.
inline void UngroupCards(const std::vector<size_t>& members) {
    std::optional<float> nextLeft;
    for (const size_t index : members) {
        if (index >= context.cards.size()) {
            continue;
        }
        Card& card = context.cards[index];
        if (IsGroupedStackCard(card)) {
            RestoreOwnSizeCentredInFrame(card);
            const float width = card.world.width();
            card.world.left = nextLeft.value_or(card.world.left);
            card.world.right = card.world.left + width;
            nextLeft = card.world.right + static_cast<float>(context.settings.gap);
        }
        ResetStackPlacement(card);
        card.stack.grouped = false;
        card.stackId.clear();
        // One write keeps the released geometry, the ungrouped flag and the
        // cleared hand-made id in step instead of saving them twice.
        RememberLayoutCard(card);
    }
    MarkLayoutDirty(GetTickCount());
}

inline void ToggleCardGroup(int index) {
    if (!context.settings.stacksEnabled || index < 0 ||
        static_cast<size_t>(index) >= context.cards.size()) {
        return;
    }
    const Card& card = context.cards[static_cast<size_t>(index)];
    const bool grouped = IsGroupedStackCard(card);
    const std::wstring groupKey = StackGroupKeyOf(card);
    if (grouped) {
        // Only the windows that really fan out are released: a same-app window
        // the user kept apart already stands on its own.
        const std::vector<size_t> fanned =
            CollectFannedMemberIndices(groupKey);
        if (fanned.size() < 2) {
            return;
        }
        UngroupCards(fanned);
    } else {
        std::vector<size_t> candidates = CollectGroupMemberIndices(groupKey);
        if (candidates.size() < 2) {
            return;
        }
        // The clicked window owns the shared frame: it is the card the user
        // acted on and the one at the front, so regrouping must not shrink the
        // stack to whichever member happens to sort first.
        const auto clicked = std::find(candidates.begin(), candidates.end(),
                                       static_cast<size_t>(index));
        if (clicked != candidates.end()) {
            std::rotate(candidates.begin(), clicked, clicked + 1);
        }
        GroupCards(candidates, groupKey);
    }
    // A split group must not keep the animation of a stack it no longer has.
    context.stackTransitions.clear();
    ReconcileCardStacks(context.cards, context.settings);
    context.hoverHeaderAction = CardHeaderAction::None;
    context.sceneDirty = true;
}

// A hand-made stack is keyed by the fingerprints of its members: they survive
// in the layout file, so the id of a group never changes between restarts and
// the same windows come back together. The fingerprints are sorted so stacking
// B onto A yields the same id as stacking A onto B: the id names the set of
// windows, not the order they were dropped in.
inline std::wstring NewStackIdFor(const std::vector<size_t>& members) {
    std::vector<std::wstring> fingerprints;
    fingerprints.reserve(members.size());
    for (const size_t index : members) {
        if (index < context.cards.size()) {
            fingerprints.push_back(context.cards[index].fingerprint);
        }
    }
    if (fingerprints.empty()) {
        return std::wstring();
    }
    std::sort(fingerprints.begin(), fingerprints.end());
    std::wstring stackId = L"stack:";
    for (size_t slot = 0; slot < fingerprints.size(); ++slot) {
        if (slot > 0) {
            stackId.push_back(L'|');
        }
        stackId += fingerprints[slot];
    }
    return stackId;
}

// Alt+dropping one card onto another joins them into a stack of their own,
// which may span different applications. Dropping onto a member of an existing
// fan brings that fan's windows along instead of starting a second one, and the
// dropped card becomes the new front so it reads as the card just placed.
inline void StackCardOntoCard(size_t draggedIndex, size_t targetIndex) {
    if (draggedIndex >= context.cards.size() ||
        targetIndex >= context.cards.size() || draggedIndex == targetIndex) {
        return;
    }
    if (SameActiveStack(context.cards[draggedIndex],
                        context.cards[targetIndex])) {
        return;
    }
    // A window the user kept apart shares the key but not the fan, so only the
    // fanned members are carried along.
    std::vector<size_t> members = CollectFannedMemberIndices(
        context.cards[targetIndex].stack.groupKey);
    if (members.size() < 2) {
        members.assign(1, targetIndex);
    }
    if (std::find(members.begin(), members.end(), draggedIndex) ==
        members.end()) {
        members.push_back(draggedIndex);
    }
    // Reuse the id of a stack that already is hand-made, so joining it keeps
    // the group (and its saved layout) intact.
    const std::wstring targetStackId = context.cards[targetIndex].stackId;
    const std::wstring stackId =
        targetStackId.empty() ? NewStackIdFor(members) : targetStackId;
    if (stackId.empty()) {
        return;
    }
    // Read the frame before the reset below: dropping onto a member of a formed
    // fan must keep that fan's shared frame, or the whole stack would snap to
    // the one member's own (now stale) window rectangle. A lone target lends its
    // own rectangle so the stack grows out of the card that was aimed at.
    const RectF frame = StackWorldRect(context.cards[targetIndex]);
    for (const size_t index : members) {
        Card& card = context.cards[index];
        ResetStackPlacement(card);
        card.stackId = stackId;
        card.stack.grouped = true;
        card.stack.groupKey = stackId;
    }
    std::vector<size_t> order;
    order.reserve(members.size());
    order.push_back(draggedIndex);
    for (const size_t index : members) {
        if (index != draggedIndex) {
            order.push_back(index);
        }
    }
    for (Card& card : context.cards) {
        if (card.stack.groupKey == stackId) {
            card.stack.frameWorld = frame;
        }
    }
    context.stackTransitions.clear();
    ReconcileCardStacks(context.cards, context.settings);
    ApplyStackOrder(context.cards, order, context.settings);
    for (const size_t index : members) {
        RememberLayoutCard(context.cards[index]);
    }
    context.selectedCard = static_cast<int>(draggedIndex);
    context.dropTargetCard = -1;
    context.sceneDirty = true;
}

// Alt+dragging a member out of a stack releases just that window: it takes
// back its own size, keeps the place the stack stood, and is marked ungrouped
// so the automatic per-application stack does not pull it straight back in.
inline void DetachCardFromStack(size_t index) {
    if (index >= context.cards.size() ||
        !IsGroupedStackCard(context.cards[index])) {
        return;
    }
    const std::wstring leftKey = context.cards[index].stack.groupKey;
    RestoreOwnSizeCentredInFrame(context.cards[index]);
    Card& card = context.cards[index];
    card.stackId.clear();
    ResetStackPlacement(card);
    card.stack.grouped = false;
    context.stackTransitions.clear();
    ReconcileCardStacks(context.cards, context.settings);
    RememberLayoutCard(card);
    // The rest of the fan kept its place, so their saved geometry has to follow
    // the slots the release handed out.
    for (Card& member : context.cards) {
        if (member.stack.grouped && !member.stack.groupKey.empty() &&
            member.stack.groupKey == leftKey) {
            RememberLayoutCard(member);
        }
    }
    context.detachFromKey = leftKey;
    context.dragDetached = true;
    context.sceneDirty = true;
}

// The card a drop lands on. Only a solid overlap counts, so brushing a card
// never merges two windows by accident; the largest overlap wins when the
// dragged card covers several.
inline int DropTargetCard(size_t draggedIndex) {
    constexpr float kMergeOverlapFraction = 0.35f;
    if (draggedIndex >= context.cards.size()) {
        return -1;
    }
    const RectF source = StackWorldRect(context.cards[draggedIndex]);
    int best = -1;
    float bestArea = 0.0f;
    float bestSmallerArea = 0.0f;
    for (size_t index = 0; index < context.cards.size(); ++index) {
        const Card& card = context.cards[index];
        if (index == draggedIndex || card.state != WindowVisualState::Live ||
            SameActiveStack(context.cards[draggedIndex], card) ||
            (!context.detachFromKey.empty() &&
             card.stack.groupKey == context.detachFromKey)) {
            continue;
        }
        const RectF bounds = StackWorldRect(card);
        const float left = std::max(source.left, bounds.left);
        const float top = std::max(source.top, bounds.top);
        const float right = std::min(source.right, bounds.right);
        const float bottom = std::min(source.bottom, bounds.bottom);
        if (right <= left || bottom <= top) {
            continue;
        }
        const float area = (right - left) * (bottom - top);
        if (area <= bestArea) {
            continue;
        }
        bestArea = area;
        bestSmallerArea = std::min(source.width() * source.height(),
                                   bounds.width() * bounds.height());
        best = static_cast<int>(index);
    }
    if (best < 0 || bestArea < bestSmallerArea * kMergeOverlapFraction) {
        return -1;
    }
    return best;
}

inline bool PointInMinimap(Vec2 point) {
    return context.settings.minimapEnabled &&
           ContainsPoint(context.minimapRect, point.x, point.y);
}

// An other-desktop card only becomes usable once its window joins the desktop
// that displays the canvas. A refused move keeps the old behaviour: the dive
// still ends in foreground activation.
inline void PullOtherDesktopWindow(const Card& card) {
    if (card.destroyed) {
        return;
    }
    const DesktopPullOutcome outcome =
        PullWindowToDesktop(context.desktopManager, card.hwnd, context.hostWindow);
    if (outcome.result != DesktopPullResult::Rejected) {
        return;
    }
    Wh_Log(L"Aleph: MoveWindowToDesktop failed: 0x%08X",
           static_cast<unsigned int>(outcome.error));
}

inline void BeginDive(int index) {
    if (index < 0 || static_cast<size_t>(index) >= context.cards.size()) {
        return;
    }
    context.panning = false;
    context.draggingMinimap = false;
    context.draggedCard = -1;
    context.camera.zoomAnchored = false;
    context.diveCard = index;
    context.diveStart = context.camera;
    Card& card = context.cards[static_cast<size_t>(index)];
    context.diveTarget = DiveCameraForCard(card);
    context.diveStartTick = GetTickCount();
    if (card.state == WindowVisualState::OtherDesktop) {
        PullOtherDesktopWindow(card);
    }
    // Pulling a minimized cross-desktop window leaves it iconic, so the restore
    // follows the pull instead of the scan state.
    if (IsIconic(card.hwnd) != FALSE) {
        ShowWindowAsync(card.hwnd, SW_RESTORE);
    }
    context.diving = true;
}

inline void FinishDive() {
    if (context.diveCard < 0 ||
        static_cast<size_t>(context.diveCard) >= context.cards.size()) {
        context.diving = false;
        return;
    }
    Card& card = context.cards[static_cast<size_t>(context.diveCard)];
    if (!card.destroyed && IsWindow(card.hwnd)) {
        SetForegroundWindow(card.hwnd);
    }
    context.camera.targetCenterX = context.camera.centerX;
    context.camera.targetCenterY = context.camera.centerY;
    context.camera.targetZoom = context.camera.zoom;
    HideCanvasWindows();
}

inline void AdvanceDive(DWORD now) {
    if (!context.diving) {
        return;
    }
    const float duration = static_cast<float>(context.settings.animationMs);
    const float progress = duration <= 0.0f ? 1.0f
        : static_cast<float>(now - context.diveStartTick) / duration;
    const float eased = EaseOutCubic(progress);
    context.camera.centerX = context.diveStart.centerX +
        (context.diveTarget.targetCenterX - context.diveStart.centerX) * eased;
    context.camera.centerY = context.diveStart.centerY +
        (context.diveTarget.targetCenterY - context.diveStart.centerY) * eased;
    context.camera.zoom = context.diveStart.zoom +
        (context.diveTarget.targetZoom - context.diveStart.zoom) * eased;
}

inline void StartPan(POINT cursor) {
    context.camera.snapToTarget();
    context.camera.zoomAnchored = false;
    context.panning = true;
    context.panAnchorCursor = cursor;
    context.panAnchorWorld = ScreenToWorld(
        Vec2{static_cast<float>(cursor.x), static_cast<float>(cursor.y)}, context.camera);
}

inline void UpdatePan(POINT cursor) {
    const Vec2 anchor = context.panAnchorWorld;
    const float zoom = std::max(context.camera.zoom, 0.01f);
    context.camera.targetCenterX = anchor.x -
        (static_cast<float>(cursor.x) - ScreenCenterX()) / zoom;
    context.camera.targetCenterY = anchor.y -
        (static_cast<float>(cursor.y) - ScreenCenterY()) / zoom;
}

// The grab offset keeps the card under the point the user pressed, so a card
// swapped for one of another size mid-drag never jumps.
inline void SetDragGrabOffset(int index, POINT cursor) {
    if (index < 0 || static_cast<size_t>(index) >= context.cards.size()) {
        return;
    }
    const RectF screen = WorldToScreen(
        StackWorldRect(context.cards[static_cast<size_t>(index)]),
        context.camera);
    context.dragGrabOffset.x =
        static_cast<LONG>(static_cast<float>(cursor.x) - screen.left);
    context.dragGrabOffset.y =
        static_cast<LONG>(static_cast<float>(cursor.y) - screen.top);
}

inline void BeginCardDrag(int index, POINT cursor) {
    context.hoverHeaderAction = CardHeaderAction::None;
    context.camera.snapToTarget();
    context.camera.zoomAnchored = false;
    context.draggedCard = index;
    context.dragDetached = false;
    context.detachFromKey.clear();
    context.dropTargetCard = -1;
    context.dragCursor = cursor;
    SetDragGrabOffset(index, cursor);
    SetCapture(context.hostWindow);
}

inline void UpdateCardDrag(POINT cursor, bool combine) {
    if (context.draggedCard < 0 ||
        static_cast<size_t>(context.draggedCard) >= context.cards.size()) {
        return;
    }
    context.dragCursor = cursor;
    // Alt+dragging a member pulls it out of the fan first, so the rest of the
    // gesture moves one window at its own size instead of the whole stack.
    if (!context.dragDetached &&
        combine &&
        IsGroupedStackCard(context.cards[static_cast<size_t>(context.draggedCard)])) {
        DetachCardFromStack(static_cast<size_t>(context.draggedCard));
        SetDragGrabOffset(context.draggedCard, cursor);
    }
    const Vec2 world = ScreenToWorld(
        Vec2{static_cast<float>(cursor.x - context.dragGrabOffset.x),
             static_cast<float>(cursor.y - context.dragGrabOffset.y)}, context.camera);
    Card& card = context.cards[static_cast<size_t>(context.draggedCard)];
    // The frame is what the user grabs, so the drag follows its size instead
    // of a single member's window size.
    const RectF current = StackWorldRect(card);
    RectF moved = MakeRectF(world.x, world.y, current.width(), current.height());
    const SnapResult snap = ComputeSnap(moved, context.draggedCard);
    moved.left = snap.left;
    moved.top = snap.top;
    moved.right = moved.left + current.width();
    moved.bottom = moved.top + current.height();
    if (IsGroupedStackCard(card)) {
        // Dragging a stack moves its one frame, so every member keeps the
        // shared size and centre while the whole stack follows the cursor.
        const RectF frame = moved;
        for (Card& member : context.cards) {
            if (SameActiveStack(card, member)) {
                member.stack.frameWorld = frame;
            }
        }
    } else {
        card.world = moved;
        RememberLayoutEntry(card.fingerprint, card.world);
        MarkLayoutDirty(GetTickCount());
    }
    // Alt+dragging also shows which card the window would join, so the merge
    // is a deliberate aim instead of a surprise on release.
    context.dropTargetCard =
        combine ? DropTargetCard(static_cast<size_t>(context.draggedCard)) : -1;
    context.hasGuideVertical = snap.snapVertical;
    context.hasGuideHorizontal = snap.snapHorizontal;
    context.guideVertical = RectF{(snap.guideX - context.camera.centerX) * context.camera.zoom +
                                      ScreenCenterX(), 0.0f, 0.0f, 0.0f};
    context.guideHorizontal = RectF{0.0f,
        (snap.guideY - context.camera.centerY) * context.camera.zoom + ScreenCenterY(),
        0.0f, 0.0f};
}

inline void UpdateCardDrag(POINT cursor) {
    UpdateCardDrag(cursor, AltHeld());
}

inline void RememberDraggedStack() {
    if (context.draggedCard < 0 ||
        static_cast<size_t>(context.draggedCard) >= context.cards.size()) {
        return;
    }
    const Card& dragged = context.cards[static_cast<size_t>(context.draggedCard)];
    if (!IsGroupedStackCard(dragged)) {
        return;
    }
    for (const Card& member : context.cards) {
        if (SameActiveStack(dragged, member)) {
            RememberLayoutPin(member.fingerprint, StackWorldRect(member),
                              member.pinned, member.stack.grouped);
        }
    }
    MarkLayoutDirty(GetTickCount());
}

inline void EndPointerDrag(bool combine) {
    // Alt+drop joins the dragged window to the card it lands on. A plain drop
    // leaves both cards alone: only the Z-order decides what covers what.
    if (context.draggedCard >= 0 && combine) {
        const int target =
            DropTargetCard(static_cast<size_t>(context.draggedCard));
        if (target >= 0) {
            StackCardOntoCard(static_cast<size_t>(context.draggedCard),
                              static_cast<size_t>(target));
        }
    }
    RememberDraggedStack();
    context.panning = false;
    context.draggingMinimap = false;
    context.draggedCard = -1;
    context.dragDetached = false;
    context.detachFromKey.clear();
    context.dropTargetCard = -1;
    context.hasGuideVertical = false;
    context.hasGuideHorizontal = false;
    context.hoverHeaderAction = CardHeaderAction::None;
    context.sceneDirty = true;
    if (GetCapture() == context.hostWindow) {
        ReleaseCapture();
    }
}

inline void EndPointerDrag() {
    EndPointerDrag(AltHeld());
}

inline void HandleMouseMove(POINT cursor) {
    if (context.diving) {
        return;
    }
    if (UpdateChromeDpi()) {
        ReleaseTextFormats();
        context.sceneDirty = true;
    }
    context.dragCursor = cursor;
    if (context.draggingMinimap) {
        PanCameraFromMinimap(Vec2{static_cast<float>(cursor.x),
                                  static_cast<float>(cursor.y)});
        return;
    }
    if (context.draggedCard >= 0) {
        UpdateCardDrag(cursor);
        context.sceneDirty = true;
        return;
    }
    if (context.panning) {
        UpdatePan(cursor);
        context.sceneDirty = true;
        return;
    }
    const Vec2 point{static_cast<float>(cursor.x), static_cast<float>(cursor.y)};
    const bool minimapHover = PointInMinimap(point);
    const int hoverCard = minimapHover ? -1 : HitTestCard(point);
    const CardHeaderAction hoverAction =
        hoverCard >= 0 ? HeaderActionAt(hoverCard, point) : CardHeaderAction::None;
    if (minimapHover != context.minimapHover || hoverCard != context.hoverCard ||
        hoverAction != context.hoverHeaderAction) {
        context.minimapHover = minimapHover;
        context.hoverCard = hoverCard;
        context.hoverHeaderAction = hoverAction;
        context.sceneDirty = true;
    }
}

inline void HandleMouseLeave() {
    context.hoverCard = -1;
    context.hoverHeaderAction = CardHeaderAction::None;
    context.minimapHover = false;
    context.sceneDirty = true;
}

inline void HandleLeftButtonDown(POINT cursor) {
    if (context.diving) {
        return;
    }
    context.sceneDirty = true;
    const Vec2 point{static_cast<float>(cursor.x), static_cast<float>(cursor.y)};
    if (PointInMinimap(point)) {
        context.draggingMinimap = true;
        SetCapture(context.hostWindow);
        PanCameraFromMinimap(point);
        return;
    }
    const int card = HitTestCard(point);
    if (card >= 0) {
        const CardHeaderAction action = HeaderActionAt(card, point);
        if (action == CardHeaderAction::Close) {
            CloseCardWindow(card);
            return;
        }
        if (action == CardHeaderAction::Pin) {
            ToggleCardPin(card);
            return;
        }
        if (action == CardHeaderAction::Group) {
            ToggleCardGroup(card);
            return;
        }
    }
    // Shift+click on a stacked card brings the next window in the stack
    // to the front. Without it the background windows would be visible
    // in the fan but unreachable by mouse; drag and dive stay unchanged.
    if (card >= 0 && (GetKeyState(VK_SHIFT) & 0x8000) != 0) {
        const std::wstring groupKey = context.cards[static_cast<size_t>(card)].stack.groupKey;
        if (CycleCardStack(context.cards, context.settings, card, 1)) {
            context.selectedCard = card;
            for (size_t index = 0; index < context.cards.size(); ++index) {
                if (context.cards[index].stack.groupKey == groupKey &&
                    context.cards[index].stack.slot == 0) {
                    context.selectedCard = static_cast<int>(index);
                    break;
                }
            }
            return;
        }
    }
    context.selectedCard = card;
    if (card >= 0) {
        BeginCardDrag(card, cursor);
        return;
    }
    if ((GetKeyState(VK_SPACE) & 0x8000) != 0) {
        StartPan(cursor);
        SetCapture(context.hostWindow);
    }
}

inline void HandleMouseWheel(WPARAM wParam, LPARAM lParam) {
    if (context.diving || GET_WHEEL_DELTA_WPARAM(wParam) == 0) {
        return;
    }
    POINT cursor = {GET_X_LPARAM(lParam) - context.virtualScreen.left,
                    GET_Y_LPARAM(lParam) - context.virtualScreen.top};
    // Alt+wheel over a stack cycles it instead of zooming: the wheel is
    // otherwise bound to canvas zoom, so a plain wheel can never switch
    // cards. Alt+wheel that misses a cyclable stack falls through to
    // zoom, and a plain wheel is unchanged.
    if (AltHeld()) {
        const int hit = HitTestCard(Vec2{static_cast<float>(cursor.x),
                                         static_cast<float>(cursor.y)});
        if (hit >= 0 && CycleStackWithAnimation(
                context.cards,
                StackCycleRequest{context.settings, hit,
                    StackWheelSteps(GET_WHEEL_DELTA_WPARAM(wParam)), GetTickCount()},
                context.stackTransitions)) {
            const std::wstring groupKey =
                context.cards[static_cast<size_t>(hit)].stack.groupKey;
            context.selectedCard = hit;
            for (size_t index = 0; index < context.cards.size(); ++index) {
                if (context.cards[index].stack.groupKey == groupKey &&
                    context.cards[index].stack.slot == 0) {
                    context.selectedCard = static_cast<int>(index);
                    break;
                }
            }
            context.sceneDirty = true;
            return;
        }
    }
    const float steps = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) /
                        static_cast<float>(WHEEL_DELTA);
    const float factor = std::pow(kZoomStep, steps);
    ZoomAtPoint(context.camera, context.camera.targetZoom * factor,
                Vec2{static_cast<float>(cursor.x), static_cast<float>(cursor.y)});
}

inline void HandleKeyDown(WPARAM key) {
    if (key == VK_ESCAPE) {
        HideCanvasWindows();
        return;
    }
    if (context.diving) {
        return;
    }
    if (key == VK_RETURN) {
        BeginDive(context.selectedCard >= 0 ? context.selectedCard : context.hoverCard);
        return;
    }
    if (key == VK_HOME) {
        FitAllCards();
        return;
    }
    if (key == VK_TAB && !context.cards.empty()) {
        const int direction = (GetKeyState(VK_SHIFT) & 0x8000) != 0 ? -1 : 1;
        const int count = static_cast<int>(context.cards.size());
        const int start = context.selectedCard < 0
                              ? (direction > 0 ? -1 : 0)
                              : context.selectedCard;
        context.selectedCard = (start + direction + count) % count;
        CenterOnCard(context.selectedCard);
        return;
    }
    if (key == VK_LEFT || key == VK_RIGHT || key == VK_UP || key == VK_DOWN) {
        context.camera.zoomAnchored = false;
        const float amount = 120.0f / std::max(context.camera.zoom, 0.01f);
        context.camera.targetCenterX += key == VK_LEFT ? -amount : key == VK_RIGHT ? amount : 0.0f;
        context.camera.targetCenterY += key == VK_UP ? -amount : key == VK_DOWN ? amount : 0.0f;
    }
}


static constexpr wchar_t kHostClassName[] = L"AlephCanvasHostWindow";
static constexpr wchar_t kHudClassName[] = L"AlephCanvasHudWindow";

inline void SyncHudZOrder() {
    if (!context.visible || context.hudWindow == nullptr) {
        return;
    }
    SetWindowPos(context.hudWindow, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

inline void RefreshCanvasGeometry() {
    if (UpdateChromeDpi()) {
        ReleaseTextFormats();
    }
    PlaceCanvasWindows();
}

inline LRESULT CALLBACK CanvasWindowProc(HWND window, UINT message,
                                          WPARAM wParam, LPARAM lParam) {
    if (window == context.hudWindow) {
        if (message == WM_NCHITTEST) {
            return HTTRANSPARENT;
        }
        return DefWindowProcW(window, message, wParam, lParam);
    }
    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT paint = {};
            HDC dc = BeginPaint(window, &paint);
            DrawHostScene(dc);
            EndPaint(window, &paint);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEMOVE: {
            TRACKMOUSEEVENT tracking = {sizeof(tracking), TME_LEAVE, window, 0};
            TrackMouseEvent(&tracking);
            const POINT cursor = CursorInCanvas();
            HandleMouseMove(cursor);
            return 0;
        }
        case WM_MOUSELEAVE:
            HandleMouseLeave();
            return 0;
        case WM_LBUTTONDOWN:
            HandleLeftButtonDown(CursorInCanvas());
            return 0;
        case WM_LBUTTONDBLCLK: {
            const Vec2 point{static_cast<float>(CursorInCanvas().x),
                             static_cast<float>(CursorInCanvas().y)};
            const int index = HitTestCard(point);
            if (HeaderActionAt(index, point) != CardHeaderAction::None) {
                return 0;
            }
            BeginDive(index);
            return 0;
        }
        case WM_LBUTTONUP:
            EndPointerDrag();
            return 0;
        case WM_MBUTTONDOWN:
            if (context.diving) return 0;
            StartPan(CursorInCanvas());
            SetCapture(window);
            return 0;
        case WM_MBUTTONUP:
        case WM_CAPTURECHANGED:
            EndPointerDrag();
            return 0;
        case WM_MOUSEWHEEL:
            HandleMouseWheel(wParam, lParam);
            return 0;
        case WM_KEYDOWN:
            HandleKeyDown(wParam);
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && context.visible &&
                !context.diving) {
                HideCanvasWindows();
            }
            return DefWindowProcW(window, message, wParam, lParam);
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            RefreshCanvasGeometry();
            return 0;
        case WM_WINDOWPOSCHANGED:
            SyncHudZOrder();
            return DefWindowProcW(window, message, wParam, lParam);
        case WM_CLOSE:
            HideCanvasWindows();
            return 0;
        default:
            return DefWindowProcW(window, message, wParam, lParam);
    }
}

inline bool RegisterCanvasWindows(HINSTANCE instance) {
    WNDCLASSEXW hostClass = {};
    hostClass.cbSize = sizeof(hostClass);
    hostClass.lpfnWndProc = CanvasWindowProc;
    hostClass.hInstance = instance;
    hostClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    hostClass.lpszClassName = kHostClassName;
    hostClass.style = CS_DBLCLKS;
    if (RegisterClassExW(&hostClass) == 0) {
        return false;
    }
    WNDCLASSEXW hudClass = hostClass;
    hudClass.lpszClassName = kHudClassName;
    if (RegisterClassExW(&hudClass) == 0) {
        UnregisterClassW(kHostClassName, instance);
        return false;
    }
    context.virtualScreen = ReadVirtualScreen();
    context.hostWindow = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW, kHostClassName, L"Aleph Canvas",
        WS_POPUP, context.virtualScreen.left, context.virtualScreen.top,
        VirtualScreenWidth(), VirtualScreenHeight(), nullptr, nullptr, instance,
        nullptr);
    context.hudWindow = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE,
        kHudClassName, L"Aleph Canvas HUD", WS_POPUP,
        context.virtualScreen.left, context.virtualScreen.top,
        VirtualScreenWidth(), VirtualScreenHeight(), nullptr, nullptr, instance,
        nullptr);
    if (context.hostWindow == nullptr || context.hudWindow == nullptr) {
        if (context.hudWindow != nullptr) DestroyWindow(context.hudWindow);
        if (context.hostWindow != nullptr) DestroyWindow(context.hostWindow);
        context.hudWindow = nullptr;
        context.hostWindow = nullptr;
        UnregisterClassW(kHudClassName, instance);
        UnregisterClassW(kHostClassName, instance);
        return false;
    }
    return true;
}

// `logFailure` is false for the automatic backoff retries: a combination that
// stays taken would otherwise log one failure every few seconds forever.
inline bool RegisterCanvasHotkey(bool logFailure) {
    if (context.settings.toggleHotkey.empty()) {
        context.hotkeyRetryDelayMs = 0;
        context.hotkeyRetryTick = 0;
        if (context.hotkeyRegistered &&
            UnregisterHotKey(nullptr, context.hotkeyId) == FALSE) {
            Wh_Log(L"Aleph: UnregisterHotKey failed: %lu", GetLastError());
            return false;
        }
        context.hotkeyRegistered = false;
        return true;
    }
    UINT modifiers = 0;
    UINT key = 0;
    if (!ParseHotkeyText(context.settings.toggleHotkey, modifiers, key)) {
        Wh_Log(L"Aleph: invalid toggle hotkey");
        // An unparseable setting cannot become valid by retrying, so drop any
        // pending retry instead of polling forever.
        context.hotkeyRetryDelayMs = 0;
        context.hotkeyRetryTick = 0;
        return false;
    }
    if (context.hotkeyRegistered && context.hotkeyModifiers == modifiers &&
        context.hotkeyKey == key) {
        context.hotkeyRetryDelayMs = 0;
        context.hotkeyRetryTick = 0;
        return true;
    }
    const UINT nextId = context.hotkeyId == kToggleHotkeyId
                            ? kToggleHotkeyId + 1 : kToggleHotkeyId;
    if (!RegisterHotKey(nullptr, nextId, modifiers | MOD_NOREPEAT, key)) {
        if (logFailure) {
            Wh_Log(L"Aleph: RegisterHotKey failed: %lu", GetLastError());
        }
        // The combination may still be held by another program that is closing,
        // so schedule a capped exponential backoff retry. Without live cards
        // the canvas stays hidden, but the loop still wakes to retry.
        context.hotkeyRetryDelayMs =
            NextHotkeyRetryDelay(context.hotkeyRetryDelayMs);
        context.hotkeyRetryTick = GetTickCount();
        return false;
    }
    if (context.hotkeyRegistered) {
        UnregisterHotKey(nullptr, context.hotkeyId);
    }
    context.hotkeyId = nextId;
    context.hotkeyModifiers = modifiers;
    context.hotkeyKey = key;
    context.hotkeyRegistered = true;
    context.hotkeyRetryDelayMs = 0;
    context.hotkeyRetryTick = 0;
    return true;
}

inline bool HotkeyRetryPending() {
    return context.hotkeyRetryDelayMs != 0;
}

// Milliseconds until the next hotkey retry is due; zero means it is due now.
inline DWORD HotkeyRetryRemaining(DWORD now) {
    if (!HotkeyRetryPending()) {
        return INFINITE;
    }
    const DWORD elapsed = now - context.hotkeyRetryTick;
    return elapsed >= context.hotkeyRetryDelayMs
               ? 0 : context.hotkeyRetryDelayMs - elapsed;
}

inline void RetryCanvasHotkeyIfDue(DWORD now) {
    if (HotkeyRetryRemaining(now) != 0) {
        return;
    }
    if (RegisterCanvasHotkey(false)) {
        Wh_Log(L"Aleph: toggle hotkey registered on retry");
    }
}

inline void ScanAndMerge(IVirtualDesktopManager* desktopManager, DWORD now) {
    if (!context.visible || context.diving ||
        (!context.scanRequested && now - context.lastRescanTick <
             static_cast<DWORD>(context.settings.rescanIntervalMs))) {
        return;
    }
    const std::vector<ScannedWindow> scanned = ScanWindows(desktopManager);
    context.sceneDirty = MergeScan(scanned) || context.sceneDirty;
    if (HasDueThumbnailRetry(now) || ConsumeDueIconRetries(now)) {
        context.sceneDirty = true;
    }
    if (!context.cameraInitialized) {
        FitAllCards();
        context.camera.snapToTarget();
        context.cameraInitialized = true;
    }
    context.lastRescanTick = now;
    context.scanRequested = false;
}

inline void RenderCanvasFrame() {
    if (!context.visible) {
        return;
    }
    UpdateThumbnails();
    InvalidateRect(context.hostWindow, nullptr, FALSE);
    UpdateWindow(context.hostWindow);
    SubmitHudFrame();
    context.sceneDirty = false;
}

inline void UpdateCameraAndDive(DWORD now) {
    const DWORD elapsed = now - context.lastFrameTick;
    context.lastFrameTick = now;
    const float seconds = static_cast<float>(elapsed) / 1000.0f;
    AdvanceDive(now);
    if (!context.diving) {
        AdvanceCamera(context.camera, seconds);
    }
    context.cameraMotion = !context.camera.settled();
}

inline void ProcessCanvasMessage(const MSG& message) {
    if (message.message == WM_HOTKEY && context.hotkeyRegistered &&
        !context.settings.toggleHotkey.empty() && message.wParam == context.hotkeyId) {
        ToggleCanvas();
        return;
    }
    TranslateMessage(&message);
    DispatchMessageW(&message);
}

inline void CleanupCanvas(HINSTANCE instance) {
    UnregisterHotKey(nullptr, context.hotkeyId);
    context.hotkeyRegistered = false;
    HideCanvasWindows();
    ReleaseAllThumbnails();
    ClearCards();
    if (context.hudWindow != nullptr) DestroyWindow(context.hudWindow);
    if (context.hostWindow != nullptr) DestroyWindow(context.hostWindow);
    context.hudWindow = nullptr;
    context.hostWindow = nullptr;
    ReleaseGraphics();
    UnregisterClassW(kHudClassName, instance);
    UnregisterClassW(kHostClassName, instance);
}

inline bool DrainCanvasMessages() {
    MSG message = {};
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            return false;
        }
        ProcessCanvasMessage(message);
    }
    return true;
}

inline void ApplyPendingSettings() {
    if (!context.settingsDirty.exchange(false, std::memory_order_acq_rel)) {
        return;
    }
    LoadSettings();
    context.stackTransitions.clear();
    ReconcileCardStacks(context.cards, context.settings);
    RegisterCanvasHotkey(true);
    context.camera.targetZoom = ClampZoom(context.camera.targetZoom);
    context.camera.zoomAnchored = false;
    context.scanRequested = true;
    context.sceneDirty = true;
}

inline DWORD CanvasWaitTimeout(DWORD now) {
    DWORD timeout = INFINITE;
    if (context.visible) {
        const DWORD interval = static_cast<DWORD>(context.settings.rescanIntervalMs);
        const DWORD elapsed = now - context.lastRescanTick;
        timeout = elapsed >= interval ? 0 : interval - elapsed;
    }
    if (context.layoutDirty) {
        const DWORD elapsed = now - context.layoutDirtyTick;
        const DWORD remaining = elapsed >= kLayoutSaveDelayMs
                                    ? 0 : kLayoutSaveDelayMs - elapsed;
        timeout = std::min(timeout, remaining);
    }
    // A pending hotkey retry must wake the loop even while the canvas is
    // hidden, so a failed registration self-heals without user input.
    timeout = std::min(timeout, HotkeyRetryRemaining(now));
    return timeout;
}

inline void AdvanceCanvasFrame(IVirtualDesktopManager* manager, DWORD now) {
    ApplyPendingSettings();
    RetryCanvasHotkeyIfDue(now);
    ScanAndMerge(manager, now);
    UpdateCameraAndDive(now);
    const bool stackAnimationWasActive = !context.stackTransitions.empty();
    const bool stackAnimationActive = AdvanceStackCycleTransitions(
        context.cards, context.stackTransitions, now);
    if (stackAnimationWasActive || stackAnimationActive) {
        context.sceneDirty = true;
    }
    if (context.visible && (context.sceneDirty || context.cameraMotion ||
                            context.diving)) {
        RenderCanvasFrame();
    }
    if (context.diving && now - context.diveStartTick >=
                              static_cast<DWORD>(context.settings.animationMs)) {
        FinishDive();
    }
    FlushLayoutIfDue(now);
}

inline DWORD PacedCanvasTimeout(DWORD now) {
    if (!context.visible || (!context.cameraMotion && !context.diving &&
                             context.stackTransitions.empty())) {
        return CanvasWaitTimeout(now);
    }
    const DWORD started = GetTickCount();
    const HRESULT result = DwmFlush();
    return FAILED(result) || GetTickCount() == started ? kFrameIntervalMs : 0;
}

inline void RunCanvasLoop(IVirtualDesktopManager* manager) {
    const HANDLE events[2] = {context.stopEvent, context.settingsEvent};
    while (WaitForSingleObject(context.stopEvent, 0) != WAIT_OBJECT_0) {
        if (!DrainCanvasMessages()) {
            return;
        }
        const DWORD now = GetTickCount();
        AdvanceCanvasFrame(manager, now);
        const DWORD wait = MsgWaitForMultipleObjectsEx(
            2, events, PacedCanvasTimeout(now), QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) {
            return;
        }
    }
}

inline DWORD WINAPI CanvasWorkerThread(LPVOID) {
    const DPI_AWARENESS_CONTEXT previousDpi =
        SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(comResult)) {
        if (previousDpi != nullptr) SetThreadDpiAwarenessContext(previousDpi);
        return 1;
    }
    HINSTANCE instance = GetModuleHandleW(nullptr);
    if (!RegisterCanvasWindows(instance)) {
        CoUninitialize();
        if (previousDpi != nullptr) SetThreadDpiAwarenessContext(previousDpi);
        return 1;
    }
    if (!RegisterCanvasHotkey(true)) {
        Wh_Log(L"Aleph: the toggle hotkey is unavailable");
    }
    IVirtualDesktopManager* desktopManager = nullptr;
    CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                    IID_PPV_ARGS(&desktopManager));
    context.desktopManager = desktopManager;
    if (context.settings.openOnStart) {
        ToggleCanvas();
    }
    context.startupSucceeded = true;
    SetEvent(context.readyEvent);
    RunCanvasLoop(desktopManager);
    context.desktopManager = nullptr;
    if (desktopManager != nullptr) {
        desktopManager->Release();
    }
    if (context.layoutDirty) {
        SaveLayoutNow();
    }
    CleanupCanvas(instance);
    CoUninitialize();
    if (previousDpi != nullptr) SetThreadDpiAwarenessContext(previousDpi);
    return 0;
}

CanvasContext context;

void CloseWorkerEvents() {
    HANDLE* events[] = {&context.stopEvent, &context.settingsEvent,
                       &context.readyEvent};
    for (HANDLE* event : events) {
        if (*event != nullptr) {
            CloseHandle(*event);
            *event = nullptr;
        }
    }
}

BOOL WhTool_ModInit() {
    LoadSettings();
    context.stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    context.settingsEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    context.readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (context.stopEvent == nullptr || context.settingsEvent == nullptr ||
        context.readyEvent == nullptr) {
        CloseWorkerEvents();
        return FALSE;
    }
    context.workerThread = CreateThread(nullptr, 0, CanvasWorkerThread, nullptr,
                                        0, &context.workerThreadId);
    if (context.workerThread == nullptr) {
        CloseWorkerEvents();
        return FALSE;
    }
    const HANDLE startup[] = {context.readyEvent, context.workerThread};
    const DWORD result = WaitForMultipleObjects(2, startup, FALSE, 15000);
    if (result == WAIT_OBJECT_0 && context.startupSucceeded) {
        return TRUE;
    }
    SetEvent(context.stopEvent);
    WaitForSingleObject(context.workerThread, INFINITE);
    CloseHandle(context.workerThread);
    context.workerThread = nullptr;
    CloseWorkerEvents();
    return FALSE;
}

void WhTool_ModUninit() {
    if (context.stopEvent != nullptr) {
        SetEvent(context.stopEvent);
    }
    if (context.workerThread != nullptr) {
        WaitForSingleObject(context.workerThread, INFINITE);
        CloseHandle(context.workerThread);
        context.workerThread = nullptr;
    }
    CloseWorkerEvents();
}

void WhTool_ModSettingsChanged() {
    context.settingsDirty.store(true, std::memory_order_release);
    if (context.settingsEvent != nullptr) {
        SetEvent(context.settingsEvent);
    }
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
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
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

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
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
