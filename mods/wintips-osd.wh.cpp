// ==WindhawkMod==
// @id              wintips-osd
// @name            WinTips OSD
// @name:zh-CN      WinTips 键盘状态提示
// @description     Shows a Win11-style floating OSD for Caps/Num/Scroll Lock, Insert and copy/paste/cut
// @description:zh-CN 按下大写/数字/滚动锁定、Insert 或复制/粘贴/剪切时弹出 Win11 质感的浮窗提示
// @version         1.6.0
// @author          DoYouNatureQ
// @github          https://github.com/DoYouNatureQ
// @homepage        https://github.com/DoYouNatureQ/WinTips
// @license         GPL-3.0-or-later
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lgdi32 -ladvapi32 -lshell32 -lshcore -ldwmapi -limm32 -ldwrite
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# WinTips OSD

A Windows 11 style floating card for the lock keys, Insert and clipboard actions.
It runs as a Windhawk tool mod in its own process: nothing is injected into other
programs and no system functions are hooked.

![Caps Lock tip sliding in over the desktop](https://raw.githubusercontent.com/DoYouNatureQ/WinTips/main/Windhawk/preview/osd-demo-dark-en.gif)

**Caps Lock**, **Num Lock**, **Scroll Lock** and **Insert** pop a small card out
of the nearest screen edge — a padlock that rotates and bounces, the state in
words, an accent bar. **Ctrl+C / V / X**, plus the classic **Ctrl+Insert**,
**Shift+Insert** and **Shift+Delete**, show a check mark or scissors with a `×N`
badge counting consecutive actions. **Win+Space**, **Ctrl+Shift**, **Alt+Shift**
and **Shift** inside an IME show a globe with the input language or the
Chinese/English mode.

![Light theme, frosted card over a wallpaper](https://raw.githubusercontent.com/DoYouNatureQ/WinTips/main/Windhawk/preview/osd-acrylic-light-en.png)

## Features

- **Blurred backdrop, live.** The card blurs what is behind it, the way the
  Windows 11 OSD does, and keeps it refreshing (Windows 10 2004 and later) so
  video and scrolling pages move through as a soft wash instead of a frozen
  frame. *Card material → Solid* for an opaque card.
- **Windows 11 look, Windows 10 included.** The mod paints the card itself, so
  it needs neither system acrylic nor system rounded corners. Per-monitor DPI,
  light/dark theme and the system accent color are followed. The labels are
  drawn by DirectWrite — the same engine the native OSD uses — so they stay
  smooth at every DPI scale, with a GDI+ fallback where dwrite.dll is not
  available.
- **Stays in front.** A fullscreen game, recorder or launcher overlay that covers
  the card is noticed within 100 ms and the card is re-raised; when nothing
  covers it, no other window is touched.
- **Hugs the taskbar.** Anchored to the taskbar's live edge, so an auto-hiding
  bar never covers the card, and the card slides in from the nearest edge.
- **Clipboard aware.** Copy and cut tips wait for the clipboard to actually
  change, so a `Ctrl+C` that interrupts a console app and a permanent
  `Shift+Delete` stay quiet. Context-menu copies can be reported too.
- **Works under elevated windows.** Windows stops calling the keyboard hook while
  an administrator window has focus; the lock keys and Insert then come from Raw
  Input, which is delivered per device and ignores both focus and integrity
  level. A key-state poll stays behind it for anything Raw Input cannot see, such
  as a remote or a virtual keyboard. See below for what does not work there.
- **Light on resources.** Nothing runs while the card is hidden. A frame takes
  about 0.5 ms at 100% size; the live backdrop and the topmost guard only tick
  while a card is up.

![Paste tip with an ×3 badge](https://raw.githubusercontent.com/DoYouNatureQ/WinTips/main/Windhawk/preview/osd-paste-en.png)

## Settings

- **Tips** — Caps Lock, Num Lock, Scroll Lock, Insert, Copy, Paste and Cut, each
  on or off. *Also notify non-keyboard copies* adds context-menu and in-app
  copies; *Show the ×N count badge* hides the badge on the copy/paste/cut cards
  when turned off.
- **Input language & Chinese/English tip** — off by default; shows the globe card
  after a language switch. Set it back to off if it fires more often than it
  helps.
- **Behaviour** — *Display duration* 500–10000 ms; *Animation speed* 100 is
  normal (300 ms), 0 disables it; *Skip while a fullscreen app is in front*.
- **Placement** — *Vertical* above the taskbar or below the top edge,
  *Horizontal* left, center or right, *Show on* the active window's display
  (default), the mouse's display or the primary one, plus pixel offsets. The
  taskbar edge is tracked live.
- **Look** — *Theme* follow Windows, dark or light; *Card material* acrylic or
  solid; *Tint opacity* 20–90% (55% by default, acrylic only); *Blur radius*
  0–40 px; *Accent color* `#RRGGBB`, empty for the system accent; *Card size*
  60–200%, gap to the screen edge unchanged.
- **Language** — language of the text on the card: Follow Windows (eight UI
  languages built in, English for anything else) or pin one.

## Things to know

- Runs as a **tool mod**: it keeps working when Explorer restarts and never loads
  into other processes. The standalone **WinTips.exe** does the same job and the
  mod stays quiet while it runs, so both can be installed — but keep only one of
  the two startup entries.
- Insert is event-based: Windows keeps no global Insert state, and only a bare
  Insert counts (with Ctrl or Shift held it is a copy or a paste).
- An **exclusive-fullscreen** game covers the card, since that mode owns the
  display pipeline. Borderless and windowed games are fine.
- Behind an **elevated window** the backdrop grab comes back black, so that tip
  falls back to a solid card. The language is still reported there, the IME mode
  is not. Copy, paste and cut are not either: they need the key the hook is
  denied, and Raw Input only answers for the lock keys and Insert.
- The language card needs a keystroke to hang on: switching with the mouse from
  the language bar or an IME status window shows nothing.
- The card is **left out of screen capture** while the live backdrop runs
  (`WDA_EXCLUDEFROMCAPTURE`): screenshots, recordings, streams and screen sharing
  don't show it. That is what keeps the live blur from feeding on itself. Pick
  *Card material → Solid* if you want the card in a recording — it uses the
  one-shot backdrop, which needs no exclusion.
- Without the live-refresh capability the backdrop is a snapshot taken when the
  card appeared; sliding in and following the taskbar still work, because the
  grab includes a margin around the card.

## Compared to Lock Keys Notifier

[Lock Keys Notifier](https://windhawk.net/mods/lock-keys-notifier) reports the
same lock keys but aims elsewhere: it draws a configurable toast on a solid
background, while this mod reproduces the native Windows 11 OSD and adds
clipboard tips. Want to tune every pixel of the card — layouts, fonts, shadows,
sounds? Lock Keys Notifier fits better. Want the native look or clipboard tips?
This one.

## Links

- Standalone version, source and issue tracker:
  [WinTips](https://github.com/DoYouNatureQ/WinTips)

## License

GPL-3.0-or-later, as the standalone WinTips app. The look and motion take
reference from the LockWindow design in FluentFlyout (also GPL-3.0); the code
here is an independent implementation, and redistributed versions must ship the
complete corresponding source under the GPL.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language
  $name:zh-CN: 语言
  $description: >-
    Language of the text on the card. Follow Windows reads the Windows display
    language and picks one of the eight built-in UI languages; anything else
    reads English. Any other value pins the card to that language.
  $description:zh-CN: >-
    浮窗上文字使用的语言。默认跟随系统：按 Windows 显示语言自动匹配内置的八种之一，其他语言显示英文。也可以固定为某一种。
  $options:
  - auto: Follow Windows
  - zh: 简体中文
  - zh-Hant: 繁體中文
  - en: English
  - de: Deutsch
  - ja: 日本語
  - ru: Русский
  - es: Español
  - pt: Português
  $options:zh-CN:
  - auto: 跟随系统
  - zh: 简体中文
  - zh-Hant: 繁體中文
  - en: English
  - de: Deutsch
  - ja: 日本語
  - ru: Русский
  - es: Español
  - pt: Português
- notifyCapsLock: true
  $name: Caps Lock
  $name:zh-CN: 大写锁定
  $description: Show a tip when Caps Lock is toggled
  $description:zh-CN: 切换大写锁定时弹出提示
- notifyNumLock: true
  $name: Num Lock
  $name:zh-CN: 数字锁定
  $description: Show a tip when Num Lock is toggled
  $description:zh-CN: 切换数字锁定时弹出提示
- notifyScrollLock: true
  $name: Scroll Lock
  $name:zh-CN: 滚动锁定
  $description: Show a tip when Scroll Lock is toggled
  $description:zh-CN: 切换滚动锁定时弹出提示
- notifyInsert: true
  $name: Insert
  $name:zh-CN: Insert 键
  $description: Show a tip when Insert is pressed
  $description:zh-CN: 按下 Insert 时弹出提示
- notifyCopy: true
  $name: Copy
  $name:zh-CN: 复制
  $description: Show a tip when copying (Ctrl+C or Ctrl+Insert)
  $description:zh-CN: 复制（Ctrl+C 或 Ctrl+Insert）时弹出提示
- notifyPaste: true
  $name: Paste
  $name:zh-CN: 粘贴
  $description: Show a tip when pasting (Ctrl+V or Shift+Insert)
  $description:zh-CN: 粘贴（Ctrl+V 或 Shift+Insert）时弹出提示
- notifyCut: true
  $name: Cut
  $name:zh-CN: 剪切
  $description: Show a tip when cutting (Ctrl+X or Shift+Delete)
  $description:zh-CN: 剪切（Ctrl+X 或 Shift+Delete）时弹出提示
- notifyAnyCopy: true
  $name: Also notify non-keyboard copies
  $name:zh-CN: 任意方式复制都提示
  $description: >-
    Also show the copy tip for context menu copies, in-app "Copy" buttons and
    any other change to the clipboard that wasn't made with Ctrl+C.

    Requires the Copy option above.
  $description:zh-CN: >-
    右键菜单、应用内“复制”按钮等非键盘复制（剪贴板更新即提示）也会弹出提示。

    需要同时开启上面的“复制”选项。
- countBadge: true
  $name: Show the ×N count badge
  $name:zh-CN: 显示 ×N 次数徽章
  $description: >-
    Copy, paste and cut cards show a small ×N badge counting consecutive
    actions. Turn this off to keep just the icon and the label.
  $description:zh-CN: >-
    复制、粘贴、剪切卡片右上角的 ×N 次数徽章记录连续操作次数。不需要时可关闭，卡片只保留图标和文字。
- notifyLanguage: false
  $name: Input language & Chinese/English tip
  $name:zh-CN: 输入法语言与中英文提示
  $description: >-
    Shows the input language after Win+Space, Ctrl+Shift or Alt+Shift, and the
    Chinese/English mode after Shift inside an IME. Off by default: unless you
    switch often and on purpose, it shows up more often than it helps.
  $description:zh-CN: >-
    Win+Space、Ctrl+Shift、Alt+Shift 换输入语言，或输入法内按 Shift 切中英文时，浮窗显示当前状态。默认关闭：除非确实经常刻意切换，否则它出现得比帮到你的次数多。
- displayMs: 2000
  $name: Display duration (milliseconds)
  $name:zh-CN: 显示时长（毫秒）
  $description: How long the card stays on screen, 500 to 10000
  $description:zh-CN: 浮窗停留时间，500 至 10000
- animationPercent: 100
  $name: Animation speed (percent)
  $name:zh-CN: 动画速度（百分比）
  $description: >-
    100 is normal. The animation lasts 300 ms times this percentage, so 50 is
    twice as fast and 200 is twice as slow. Set to 0 to disable animation.
  $description:zh-CN: >-
    100 为正常速度，动画时长 = 300 毫秒 × 百分比，50 快一倍、200 慢一倍。设为 0 关闭动画。
- tipScale: 100
  $name: Card size (percent)
  $name:zh-CN: 浮窗大小（百分比）
  $description: >-
    Scales the whole card: text, icon, accent bar and the ×N badge grow
    together. 60 to 200, 100 is the Windows 11 size. Applied the next time a
    tip appears; the margin to the screen edge stays the same.
  $description:zh-CN: >-
    整个浮窗等比缩放：文字、图标、指示条与 ×N 徽章一起跟着变大变小。60 至 200，100 为 Win11 原生大小。下次弹出时生效，与屏幕边缘的间距保持不变。
- vertical: bottom
  $name: Vertical position
  $name:zh-CN: 垂直位置
  $options:
  - bottom: Above the taskbar
  - top: Below the top edge
  $options:zh-CN:
  - bottom: 任务栏上方
  - top: 屏幕顶部下方
- horizontal: center
  $name: Horizontal position
  $name:zh-CN: 水平位置
  $options:
  - left: Left
  - center: Center
  - right: Right
  $options:zh-CN:
  - left: 左
  - center: 居中
  - right: 右
- monitor: focus
  $name: Show on
  $name:zh-CN: 显示在
  $description: >-
    Focus puts the card on the display of the window you are working in, which
    is where you are looking in a multi-monitor setup; it falls back to the
    pointer's display when nothing is in front. Cursor follows the mouse
    pointer, Primary always uses the main display.
  $description:zh-CN: >-
    跟随焦点：卡片出现在当前活动窗口所在的那块显示器，多屏时就落在你正在操作的那一块（没有前台窗口时退回鼠标所在屏）。跟随鼠标：按指针位置。主显示器：始终显示在主屏。
  $options:
  - focus: The display with the active window
  - cursor: The monitor with the mouse cursor
  - primary: The primary monitor
  $options:zh-CN:
  - focus: 活动窗口所在的显示器
  - cursor: 鼠标所在的显示器
  - primary: 主显示器
- offsetX: 0
  $name: Horizontal offset (pixels)
  $name:zh-CN: 水平偏移（像素）
  $description: Positive moves the card right
  $description:zh-CN: 正值向右移动
- offsetY: 0
  $name: Vertical offset (pixels)
  $name:zh-CN: 垂直偏移（像素）
  $description: Positive moves the card down
  $description:zh-CN: 正值向下移动
- theme: system
  $name: Theme
  $name:zh-CN: 主题
  $options:
  - system: Follow Windows
  - dark: Dark
  - light: Light
  $options:zh-CN:
  - system: 跟随系统
  - dark: 深色
  - light: 浅色
- material: acrylic
  $name: Card material
  $name:zh-CN: 卡片材质
  $description: >-
    Acrylic blurs whatever is behind the card, like the real Win11 OSD, and
    keeps the blur in step with the desktop while the card is up. Solid paints
    an opaque card instead.
  $description:zh-CN: >-
    亚克力会模糊卡片背后的画面，并在卡片显示期间持续刷新，观感最接近原生 Win11 提示。纯色则绘制不透明卡片。
  $options:
  - acrylic: Acrylic (blurred background)
  - solid: Solid color
  $options:zh-CN:
  - acrylic: 亚克力（模糊背景）
  - solid: 纯色
- tintOpacity: 55
  $name: Tint opacity (percent)
  $name:zh-CN: 底色不透明度（百分比）
  $description: >-
    Only used by the Acrylic material: how much of the card is the tint
    painted over the blurred backdrop. Lower shows more of the blurred
    background, higher looks more solid. 20 to 90, 55 is the balance point
    where text stays readable over a light wallpaper.
  $description:zh-CN: >-
    仅亚克力材质有效：模糊背景之上那层底色盖住多少。数值越低越透、背景越清晰，越高越接近实心。20 至 90，55 是"看得见背后"与"文字不失读"的平衡点。
- blurRadius: 16
  $name: Blur radius (pixels)
  $name:zh-CN: 模糊半径（像素）
  $description: >-
    Only used by the Acrylic material. Higher values look softer; 0 keeps the
    background sharp behind the tint. 16 matches the WinTips app's default.
  $description:zh-CN: >-
    仅亚克力材质有效。数值越大越模糊，0 表示背景不模糊、只叠一层色调。16 与 WinTips 应用默认值一致。
- accentColor: ""
  $name: Accent color
  $name:zh-CN: 强调色
  $description: >-
    Hex color such as #0067C0 for the status bar and the ×N badge. Leave empty
    to follow the Windows accent color.
  $description:zh-CN: >-
    状态条与 ×N 徽章的十六进制颜色，例如 #0067C0。留空则跟随系统强调色。
- suppressFullscreen: false
  $name: Skip while a fullscreen app is in front
  $name:zh-CN: 全屏应用前台时不提示
  $description: Suppresses tips during games, fullscreen video and presentation mode
  $description:zh-CN: 游戏、全屏视频与演示模式下不弹出提示
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#include <imm.h>
#include <dwrite.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>

#ifdef __MINGW32__
#include <algorithm>
using std::max;
using std::min;
#endif

#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;

// =============================================================================
// Constants and types
// =============================================================================

namespace {

// Design units are DIPs (96 DPI); everything is scaled to physical pixels by
// the DPI of the monitor the card is about to appear on.
constexpr float kBaseWidth = 160.0f;
constexpr float kBaseHeight = 50.0f;
constexpr float kMinTipScale = 0.6f;  // card size setting, as a multiplier
constexpr float kMaxTipScale = 2.0f;
constexpr float kGap = 16.0f;         // distance to the taskbar / work area
constexpr float kSlideDistance = 20.0f;
constexpr float kZoneRatio = 0.15f;   // edge zone that picks the slide direction
constexpr int kBaseAnimationMs = 300;
constexpr int kClipIntentMs = 800;    // wait this long for the clipboard to change
constexpr int kClipMergeMs = 250;     // coalesce clipboard manager write-backs
constexpr int kPollMs = 50;           // elevated-window fallback

// How often the fallback question - "has the foreground gone above us?" - is
// asked when the window event says nothing. Slower than the event, its only job
// is to notice sooner or later, and while nothing is elevated it costs a cached
// comparison. It runs unconditionally: the event comes from SetWinEventHook,
// which Windows is just as entitled to filter as the keyboard hook, so losing
// it is precisely the case where the fallback matters.
constexpr int kPollProbeMs = 500;

// One press can be reported by every capture path at once (hook, Raw Input,
// poll). Only the first report may show a card: a second card for the same
// physical press would show the opposite of the real lock. Measured between the
// presses themselves, not between the moments each path noticed them.
constexpr int kSameKindWindowMs = 200;

// Raw Input repeats a key-down every few dozen ms while it is held; every
// repeat refreshes the stamp, so this only expires when nothing at all arrived
// for this long - the matching key-up was lost (device unplugged, a secure
// desktop that swallowed it). Treating the next press as new is then the
// recovery, and a desktop switch re-seeds the state on top of it.
constexpr int kRawRepeatGuardMs = 1500;
constexpr int kFollowIntervalMs = 60; // the taskbar slide takes ~200 ms
constexpr int kFollowMs = 140;        // shorter than the entrance, so it keeps up

// The live backdrop: the desktop behind the card is grabbed again this often
// and the blur is re-aligned to wherever the card is now. A grab costs about
// 7 ms of a core (it waits for the compositor), so this is not a frame rate to
// chase - the smoothness comes from the blend below rather than from more
// frames. 22 ms matches the WinTips app; the blend is what hides the beat.
constexpr int kLiveRefreshMs = 22;

// Box-blur passes. The one-shot snapshot runs three and approaches a Gaussian;
// the live path runs two and widens the radius instead (see kLiveRadiusScale) -
// same softness per frame for a third less convolution. Same split the
// WinTips app makes.
constexpr int kStaticBlurPasses = 3;
constexpr int kLiveBlurPasses = 2;

// The live path blurs with two passes, whose sigma is only radius * 0.82; the
// radius grows by a quarter (16 -> 20) to match the three-pass snapshot.
constexpr float kLiveRadiusScale = 1.25f;

// How far the card may wander inside one grab, in DIPs. The snapshot is taken
// around the card instead of exactly behind it, and the drawing code samples
// the part that is behind the card right now: sliding in and following the
// taskbar are covered without a new grab.
constexpr int kLivePaddingDip = 40;
constexpr int kStaticPaddingDip = 72; // one grab per tip: cover the whole slide

// New frame's weight in the blend (256 = new frame only). Around 150 the image
// catches up in about three frames - fast enough to read as "live", slow enough
// that a ~45 Hz capture of a busy desktop doesn't flicker. Same value the
// WinTips app uses.
constexpr int kLiveBlendWeight = 150;

// The topmost guard: check this often whether something else landed on top of
// the card, and speed up when another app keeps raising itself over it.
constexpr int kTopmostGuardMs = 100;
constexpr int kTopmostFastMs = 30;
constexpr int kTopmostRaisesBeforeFast = 3;
constexpr int kTopmostMaxWalk = 64; // Z-order walk limit, not a card limit

enum TipKind {
    kCapsLock = 0,
    kNumLock,
    kScrollLock,
    kInsert,
    kCopy,
    kPaste,
    kCut,
    kLanguage, // input language / IME mode, the only kind with no on/off state
    kKindCount,
};

// The keys the fallback polls: the three lock keys and Insert, which are the
// first four kinds in a row.
constexpr int kPollKeyCount = kInsert + 1;

constexpr UINT WM_APP_TIP = WM_APP + 1;
constexpr UINT WM_APP_QUIT = WM_APP + 2;

// Posted by the hook when a key that may switch the input language went down,
// so the sampling it starts reads another process's IME window without holding
// up the hook. Window message, handled on the UI thread.
constexpr UINT WM_APP_LANGUAGE_WATCH = WM_APP + 7;

constexpr UINT_PTR kTimerAnim = 1;
constexpr UINT_PTR kTimerHold = 2;
constexpr UINT_PTR kTimerFollow = 3;
constexpr UINT_PTR kTimerLiveBackdrop = 6;
constexpr UINT_PTR kTimerTopmost = 7;
constexpr UINT_PTR kTimerInput = 8;  // the language watch, on the UI thread

// Thread timers on the hook thread (window timers would need the card window).
constexpr UINT_PTR kThreadTimerPoll = 1;
constexpr UINT_PTR kThreadTimerProbe = 2;

// The language watch: poll this often for this long after a switch key.
constexpr int kInputPollMs = 50;
constexpr int kInputWindowMs = 2000;

// =============================================================================
// Settings
// =============================================================================

enum Material {
    kMaterialAcrylic = 0,
    kMaterialSolid = 1,
};

enum MonitorMode {
    kMonitorFocus = 0,   // the display with the active window
    kMonitorCursor = 1,  // the display the pointer is on
    kMonitorPrimary = 2, // always the main display
};

enum LanguageMode {
    kLangAuto = 0, // follow the Windows display language
    kLangZhHans = 1,
    kLangZhHant = 2,
    kLangEnglish = 3,
    kLangGerman = 4,
    kLangJapanese = 5,
    kLangRussian = 6,
    kLangSpanish = 7,
    kLangPortuguese = 8,
    kLangCount = 9,
};

struct Settings {
    int language = kLangAuto;
    bool notify[kKindCount] = {true, true, true, true,
                               true, true, true, false}; // language tip off
    bool notifyAnyCopy = true;
    bool countBadge = true; // ×N badge on copy/paste/cut cards (issue #7304)
    int displayMs = 2000;
    int animationPercent = 100;
    int tipScalePercent = 100;
    int vertical = 1;   // 0 = top, 1 = bottom
    int horizontal = 1; // 0 = left, 1 = center, 2 = right
    int offsetX = 0;
    int offsetY = 0;
    int monitor = kMonitorFocus;
    int theme = 0;      // 0 = system, 1 = dark, 2 = light
    int material = kMaterialAcrylic;
    // How much of the card the tint covers on the acrylic path, in percent.
    // 55 is the same balance point the WinTips app settled on: below ~40 the
    // text starts fighting a busy wallpaper, above ~75 the blur is invisible.
    int tintOpacity = 55;
    // Same default the WinTips app uses: soft enough to read as frosted glass,
    // sharp enough that the desktop behind stays recognizable.
    int blurRadius = 16;
    std::wstring accentColor;
    bool suppressFullscreen = false;

    int AnimationMs() const {
        if (animationPercent <= 0) {
            return 0;
        }
        return kBaseAnimationMs * animationPercent / 100;
    }

    // The card size setting as a multiplier. Clamped here rather than at load
    // time so a hand-edited registry value can never produce a 0x0 card.
    float TipScale() const {
        int pct = tipScalePercent;
        if (pct < (int)(kMinTipScale * 100)) {
            pct = (int)(kMinTipScale * 100);
        } else if (pct > (int)(kMaxTipScale * 100)) {
            pct = (int)(kMaxTipScale * 100);
        }
        return (float)pct / 100.0f;
    }
};

Settings g_settings;

bool EqualsNoCase(const std::wstring& a, const wchar_t* b) {
    return _wcsicmp(a.c_str(), b) == 0;
}

int Clamp(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// Setting value -> Language. Accepts the codes the WinTips app writes
// (zh / zh-Hant / en / de / ja / ru / es / pt) plus the usual Windows-style
// aliases, so a hand-edited registry value still lands somewhere sensible.
// Anything else - including an empty value - means Follow Windows.
int ParseLanguageCode(const wchar_t* code) {
    if (!code || !code[0]) {
        return kLangAuto;
    }
    std::wstring v(code);
    if (EqualsNoCase(v, L"zh") || EqualsNoCase(v, L"zh-CN") ||
        EqualsNoCase(v, L"zh-Hans") || EqualsNoCase(v, L"chs")) {
        return kLangZhHans;
    }
    if (EqualsNoCase(v, L"zh-Hant") || EqualsNoCase(v, L"zh-TW") ||
        EqualsNoCase(v, L"zh-HK") || EqualsNoCase(v, L"zh-MO") ||
        EqualsNoCase(v, L"cht")) {
        return kLangZhHant;
    }
    if (EqualsNoCase(v, L"en") || EqualsNoCase(v, L"en-US") ||
        EqualsNoCase(v, L"en-GB")) {
        return kLangEnglish;
    }
    if (EqualsNoCase(v, L"de") || EqualsNoCase(v, L"de-DE")) {
        return kLangGerman;
    }
    if (EqualsNoCase(v, L"ja") || EqualsNoCase(v, L"jp") ||
        EqualsNoCase(v, L"ja-JP")) {
        return kLangJapanese;
    }
    if (EqualsNoCase(v, L"ru") || EqualsNoCase(v, L"ru-RU")) {
        return kLangRussian;
    }
    if (EqualsNoCase(v, L"es") || EqualsNoCase(v, L"es-ES")) {
        return kLangSpanish;
    }
    if (EqualsNoCase(v, L"pt") || EqualsNoCase(v, L"pt-BR") ||
        EqualsNoCase(v, L"pt-PT")) {
        return kLangPortuguese;
    }
    return kLangAuto;
}

void LoadSettings() {
    Settings s;

    // Follow Windows is the default, and what an empty or unknown value falls
    // back to: the card then reads whatever the Windows display language is.
    const wchar_t* str = Wh_GetStringSetting(L"language");
    s.language = ParseLanguageCode(str);
    Wh_FreeStringSetting(str);

    s.notify[kCapsLock] = Wh_GetIntSetting(L"notifyCapsLock") != 0;
    s.notify[kNumLock] = Wh_GetIntSetting(L"notifyNumLock") != 0;
    s.notify[kScrollLock] = Wh_GetIntSetting(L"notifyScrollLock") != 0;
    s.notify[kInsert] = Wh_GetIntSetting(L"notifyInsert") != 0;
    s.notify[kCopy] = Wh_GetIntSetting(L"notifyCopy") != 0;
    s.notify[kPaste] = Wh_GetIntSetting(L"notifyPaste") != 0;
    s.notify[kCut] = Wh_GetIntSetting(L"notifyCut") != 0;
    s.notifyAnyCopy = Wh_GetIntSetting(L"notifyAnyCopy") != 0;
    s.countBadge = Wh_GetIntSetting(L"countBadge") != 0;
    s.notify[kLanguage] = Wh_GetIntSetting(L"notifyLanguage") != 0;

    s.displayMs = Clamp(Wh_GetIntSetting(L"displayMs"), 500, 10000);
    s.animationPercent = Clamp(Wh_GetIntSetting(L"animationPercent"), 0, 300);
    s.tipScalePercent = Clamp(Wh_GetIntSetting(L"tipScale"), 60, 200);
    s.offsetX = Clamp(Wh_GetIntSetting(L"offsetX"), -4000, 4000);
    s.offsetY = Clamp(Wh_GetIntSetting(L"offsetY"), -4000, 4000);
    s.suppressFullscreen = Wh_GetIntSetting(L"suppressFullscreen") != 0;

    str = Wh_GetStringSetting(L"vertical");
    s.vertical = (str && EqualsNoCase(str, L"top")) ? 0 : 1;
    Wh_FreeStringSetting(str);

    str = Wh_GetStringSetting(L"horizontal");
    if (str && EqualsNoCase(str, L"left")) {
        s.horizontal = 0;
    } else if (str && EqualsNoCase(str, L"right")) {
        s.horizontal = 2;
    } else {
        s.horizontal = 1;
    }
    Wh_FreeStringSetting(str);

    str = Wh_GetStringSetting(L"theme");
    if (str && EqualsNoCase(str, L"dark")) {
        s.theme = 1;
    } else if (str && EqualsNoCase(str, L"light")) {
        s.theme = 2;
    } else {
        s.theme = 0;
    }
    Wh_FreeStringSetting(str);

    str = Wh_GetStringSetting(L"monitor");
    if (str && EqualsNoCase(str, L"primary")) {
        s.monitor = kMonitorPrimary;
    } else if (str && EqualsNoCase(str, L"cursor")) {
        s.monitor = kMonitorCursor;
    } else {
        s.monitor = kMonitorFocus;
    }
    Wh_FreeStringSetting(str);

    str = Wh_GetStringSetting(L"accentColor");
    s.accentColor = str ? str : L"";
    Wh_FreeStringSetting(str);

    str = Wh_GetStringSetting(L"material");
    s.material = (str && EqualsNoCase(str, L"solid")) ? kMaterialSolid
                                                      : kMaterialAcrylic;
    Wh_FreeStringSetting(str);

    s.tintOpacity = Clamp(Wh_GetIntSetting(L"tintOpacity"), 20, 90);

    int blur = Wh_GetIntSetting(L"blurRadius");
    s.blurRadius = Clamp(blur, 0, 40);

    g_settings = s;
}

// =============================================================================
// Localization
// =============================================================================

// The card is only ever drawn after DetectLanguage() has run; English is the
// fallback so a missing call shows up as English instead of pinning every user
// to one language.
LanguageMode g_lang = kLangEnglish;

// One row per language: the eight tip labels, then the two lock states, the
// Insert event and the two IME modes. A table rather than a switch keeps a
// missing translation a compile-time error (the initializer needs kKindCount
// labels) instead of a blank label at runtime.
struct LanguageStrings {
    const wchar_t* key[kKindCount];
    const wchar_t* on;
    const wchar_t* off;
    const wchar_t* insertPressed;
    const wchar_t* imeChinese;
    const wchar_t* imeEnglish;
};

const LanguageStrings kLanguageStrings[kLangCount] = {
    // Follow Windows: never used as-is, kept so the table indices line up with
    // the LanguageMode enum. English is what resolving "auto" falls back to.
    {{L"Caps Lock", L"Num Lock", L"Scroll Lock", L"Insert", L"Copied",
      L"Pasted", L"Cut", L"Input language"},
     L"On", L"Off", L"Insert pressed", L"Chinese input", L"English input"},
    // Simplified Chinese
    {{L"大写锁定", L"数字锁定", L"滚动锁定", L"Insert", L"已复制", L"已粘贴",
      L"已剪切", L"输入法语言"},
     L"开启", L"关闭", L"Insert 键已按下", L"中文输入", L"英文输入"},
    // Traditional Chinese
    {{L"大寫鎖定", L"數字鎖定", L"捲動鎖定", L"Insert", L"已複製", L"已貼上",
      L"已剪下", L"輸入法語言"},
     L"開啟", L"關閉", L"Insert 鍵已按下", L"中文輸入", L"英文輸入"},
    // English
    {{L"Caps Lock", L"Num Lock", L"Scroll Lock", L"Insert", L"Copied",
      L"Pasted", L"Cut", L"Input language"},
     L"On", L"Off", L"Insert pressed", L"Chinese input", L"English input"},
    // German
    {{L"Feststelltaste", L"Num-Taste", L"Rollen", L"Einfg", L"Kopiert",
      L"Eingefügt", L"Ausgeschnitten", L"Eingabesprache"},
     L"An", L"Aus", L"Einfg gedrückt", L"Chinesische Eingabe",
     L"Englische Eingabe"},
    // Japanese
    {{L"Caps Lock", L"Num Lock", L"Scroll Lock", L"Insert", L"コピーしました",
      L"貼り付けました", L"切り取りました", L"入力言語"},
     L"オン", L"オフ", L"Insert キーを押しました", L"中国語入力", L"英語入力"},
    // Russian
    {{L"Caps Lock", L"Num Lock", L"Scroll Lock", L"Insert", L"Скопировано",
      L"Вставлено", L"Вырезано", L"Язык ввода"},
     L"Вкл", L"Выкл", L"Insert нажата", L"Ввод по-китайски",
     L"Ввод по-английски"},
    // Spanish
    {{L"Bloq Mayús", L"Bloq Num", L"Bloq Despl", L"Insert", L"Copiado",
      L"Pegado", L"Cortado", L"Idioma de entrada"},
     L"Activado", L"Desactivado", L"Insert pulsado", L"Entrada en chino",
     L"Entrada en inglés"},
    // Portuguese
    {{L"Caps Lock", L"Num Lock", L"Scroll Lock", L"Insert", L"Copiado",
      L"Colado", L"Recortado", L"Idioma de entrada"},
     L"Ativado", L"Desativado", L"Insert pressionado", L"Entrada em chinês",
     L"Entrada em inglês"},
};

// The language actually drawn: the setting when it names one, otherwise the
// Windows display language.
LanguageMode ResolveLanguage() {
    if (g_settings.language > kLangAuto && g_settings.language < kLangCount) {
        return (LanguageMode)g_settings.language;
    }
    LANGID lang = GetUserDefaultUILanguage();
    if (!lang) {
        return kLangEnglish;
    }
    switch (PRIMARYLANGID(lang)) {
        case LANG_CHINESE: {
            // Traditional scripts are Taiwan, Hong Kong and Macao; the
            // mainland and Singapore builds are simplified.
            WORD sub = SUBLANGID(lang);
            return (sub == SUBLANG_CHINESE_TRADITIONAL ||
                    sub == SUBLANG_CHINESE_HONGKONG ||
                    sub == SUBLANG_CHINESE_MACAU)
                       ? kLangZhHant
                       : kLangZhHans;
        }
        case LANG_GERMAN:
            return kLangGerman;
        case LANG_JAPANESE:
            return kLangJapanese;
        case LANG_RUSSIAN:
            return kLangRussian;
        case LANG_SPANISH:
            return kLangSpanish;
        case LANG_PORTUGUESE:
            return kLangPortuguese;
        default:
            return kLangEnglish;
    }
}

void DetectLanguage() {
    g_lang = ResolveLanguage();
}

const LanguageStrings& Strings() {
    return kLanguageStrings[g_lang];
}

const wchar_t* KeyName(TipKind kind) {
    return Strings().key[kind];
}

// The language card carries no fixed label: the text is whatever the switch
// resolved to, either an input language name or an IME mode. Written when the
// tip is raised and read while it is drawn, both on the UI thread.
std::wstring g_languageText;

void FormatTip(TipKind kind, bool on, std::wstring& out) {
    switch (kind) {
        case kCapsLock:
        case kNumLock:
        case kScrollLock:
            // "{label} {state}" holds in every language the card ships with.
            out = KeyName(kind);
            out += L' ';
            out += on ? Strings().on : Strings().off;
            break;
        case kInsert:
            out = Strings().insertPressed;
            break;
        case kLanguage:
            // Fall back to a plain label so the card is never blank.
            out = g_languageText.empty() ? KeyName(kLanguage) : g_languageText;
            break;
        default:
            out = KeyName(kind);
            break;
    }
}

// =============================================================================
// Theme
// =============================================================================

struct Theme {
    bool dark;
    bool highContrast;
    Color surface;
    Color tint;  // overlay painted on top of the blurred backdrop
    Color text;
    Color accent;
    Color onAccent;
    Color border;
};

bool IsDarkTheme() {
    HKEY key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                      L"Personalize",
                      0, KEY_READ, &key) == ERROR_SUCCESS) {
        DWORD value = 1;
        DWORD size = sizeof(value);
        DWORD type = 0;
        LONG r = RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, &type,
                                  (LPBYTE)&value, &size);
        RegCloseKey(key);
        if (r == ERROR_SUCCESS && type == REG_DWORD) {
            return value == 0;
        }
    }
    return false;
}

Color SystemAccent() {
    HKEY key;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", 0,
                      KEY_READ, &key) == ERROR_SUCCESS) {
        DWORD value = 0;
        DWORD size = sizeof(value);
        DWORD type = 0;
        LONG r = RegQueryValueExW(key, L"AccentColor", nullptr, &type,
                                  (LPBYTE)&value, &size);
        RegCloseKey(key);
        if (r == ERROR_SUCCESS && type == REG_DWORD) {
            return Color(255, GetRValue(value), GetGValue(value),
                         GetBValue(value));
        }
    }
    return Color(255, 0x00, 0x67, 0xC0);
}

bool ParseHexColor(const std::wstring& text, Color& out) {
    if (text.size() != 7 || text[0] != L'#') {
        return false;
    }
    for (int i = 1; i < 7; i++) {
        if (!iswxdigit(text[i])) {
            return false;
        }
    }
    unsigned int r = wcstoul(text.substr(1, 2).c_str(), nullptr, 16);
    unsigned int g = wcstoul(text.substr(3, 2).c_str(), nullptr, 16);
    unsigned int b = wcstoul(text.substr(5, 2).c_str(), nullptr, 16);
    out = Color(255, (BYTE)r, (BYTE)g, (BYTE)b);
    return true;
}

Theme CurrentTheme() {
    Theme t{};
    HIGHCONTRAST hc{sizeof(HIGHCONTRAST)};
    t.highContrast =
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) &&
        (hc.dwFlags & HCF_HIGHCONTRASTON);
    t.dark = t.highContrast ? false
                            : (g_settings.theme == 1 ? true
                                                     : (g_settings.theme == 2
                                                            ? false
                                                            : IsDarkTheme()));

    if (t.highContrast) {
        t.surface = Color(255, GetRValue(GetSysColor(COLOR_WINDOW)),
                          GetGValue(GetSysColor(COLOR_WINDOW)),
                          GetBValue(GetSysColor(COLOR_WINDOW)));
        t.text = Color(255, GetRValue(GetSysColor(COLOR_WINDOWTEXT)),
                       GetGValue(GetSysColor(COLOR_WINDOWTEXT)),
                       GetBValue(GetSysColor(COLOR_WINDOWTEXT)));
        t.accent = Color(255, GetRValue(GetSysColor(COLOR_HIGHLIGHT)),
                         GetGValue(GetSysColor(COLOR_HIGHLIGHT)),
                         GetBValue(GetSysColor(COLOR_HIGHLIGHT)));
        t.onAccent = Color(255, GetRValue(GetSysColor(COLOR_HIGHLIGHTTEXT)),
                           GetGValue(GetSysColor(COLOR_HIGHLIGHTTEXT)),
                           GetBValue(GetSysColor(COLOR_HIGHLIGHTTEXT)));
        t.border = t.text;
        t.tint = t.surface;
        return t;
    }

    // Slightly translucent so the card reads as a floating layer even though it
    // is drawn by us rather than by the DWM backdrop material.
    const BYTE alpha = 238;
    t.surface = t.dark ? Color(alpha, 0x2B, 0x2B, 0x2B)
                       : Color(alpha, 0xFB, 0xFB, 0xFB);
    // Acrylic is a tint over the blurred backdrop. The alpha comes from the
    // tint opacity setting (20-90%, default 55), which is the same knob the
    // WinTips app exposes: low values keep the wallpaper visible through the
    // blur, high ones drift back towards the solid card.
    const BYTE tintAlpha =
        (BYTE)std::lround((float)Clamp(g_settings.tintOpacity, 20, 90) * 255.0f /
                          100.0f);
    t.tint = t.dark ? Color(tintAlpha, 0x20, 0x20, 0x20)
                    : Color(tintAlpha, 0xF3, 0xF3, 0xF3);
    t.text = t.dark ? Color(255, 0xFF, 0xFF, 0xFF) : Color(255, 0x19, 0x19, 0x19);
    t.accent = ParseHexColor(g_settings.accentColor, t.accent) ? t.accent
                                                               : SystemAccent();
    t.onAccent = Color(255, 255, 255, 255);
    t.border = t.dark ? Color(0x14, 0xFF, 0xFF, 0xFF)
                      : Color(0x1A, 0x00, 0x00, 0x00);
    return t;
}

// =============================================================================
// Small math helpers
// =============================================================================

float Clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

float EaseOutCubic(float t) {
    t = Clamp01(t);
    float inv = 1.0f - t;
    return 1.0f - inv * inv * inv;
}

float EaseInCubic(float t) {
    t = Clamp01(t);
    return t * t * t;
}

float EaseOutQuad(float t) {
    t = Clamp01(t);
    return 1.0f - (1.0f - t) * (1.0f - t);
}

float EaseInOutCubic(float t) {
    t = Clamp01(t);
    if (t < 0.5f) {
        return 4.0f * t * t * t;
    }
    float f = (2.0f * t) - 2.0f;
    return 0.5f * f * f * f + 1.0f;
}

float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// =============================================================================
// Monitors and DPI
// =============================================================================

using SetThreadDpiAwarenessContext_t = DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);

float ScaleForMonitor(HMONITOR monitor) {
    if (!monitor) {
        return 1.0f;
    }
    // shcore is in the import table (-lshcore), so this is an ordinary call:
    // a tip re-reads the DPI of the display it is about to land on, and that
    // should not go through a proc address on every frame.
    UINT dpiX = 0, dpiY = 0;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
        dpiX > 0) {
        return (float)dpiX / 96.0f;
    }
    HDC dc = GetDC(nullptr);
    int dpi = dc ? GetDeviceCaps(dc, LOGPIXELSX) : 96;
    if (dc) {
        ReleaseDC(nullptr, dc);
    }
    return dpi > 0 ? (float)dpi / 96.0f : 1.0f;
}

// =============================================================================
// Taskbar
// =============================================================================
// The work area only says whether the taskbar reserved space: with auto-hide on
// it stays at the full display while the taskbar slides in and out, and it lags
// behind a taskbar that was just dragged to another edge. The taskbar's own
// window rect follows both, so the card is anchored to that instead - the same
// source the standalone build uses (Native/TaskbarInfo.cs). Collapsed, the tray
// window parks 2 px inside the screen, which is what keeps the card hugging the
// edge; slid out, it hands the card the 48 px it needs as clearance.

constexpr wchar_t kTrayClass[] = L"Shell_TrayWnd";
constexpr wchar_t kSecondaryTrayClass[] = L"Shell_SecondaryTrayWnd";

struct TaskbarBand {
    RECT rect{};
    int edge = ABE_BOTTOM;
    bool valid = false;
};

int g_bandEdge = -1; // cached docked edge of the primary taskbar, -1 unknown

// The primary display is the one whose origin is (0,0). Cached because the
// follow loop asks for it up to 17 times a second while a card is up.
HMONITOR g_primaryMonitor;
bool g_primaryMonitorKnown;

HMONITOR PrimaryMonitor() {
    if (!g_primaryMonitorKnown) {
        g_primaryMonitor =
            MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
        g_primaryMonitorKnown = true;
    }
    return g_primaryMonitor;
}

bool ClipToRect(const RECT& src, const RECT& bounds, RECT& out) {
    out.left = std::max(src.left, bounds.left);
    out.top = std::max(src.top, bounds.top);
    out.right = std::min(src.right, bounds.right);
    out.bottom = std::min(src.bottom, bounds.bottom);
    return out.right > out.left && out.bottom > out.top;
}

// Whether the band really hugs that edge and spans the whole display. A cached
// edge that no longer fits means the taskbar was dragged somewhere else.
bool BandHugsEdge(const RECT& band, const RECT& bounds, int edge) {
    long width = bounds.right - bounds.left;
    long height = bounds.bottom - bounds.top;
    switch (edge) {
        case ABE_TOP:
            return band.top <= bounds.top && band.right - band.left >= width;
        case ABE_LEFT:
            return band.left <= bounds.left && band.bottom - band.top >= height;
        case ABE_RIGHT:
            return band.right >= bounds.right && band.bottom - band.top >= height;
        default:
            return band.bottom >= bounds.bottom &&
                   band.right - band.left >= width;
    }
}

int NearestEdge(const RECT& band, const RECT& bounds) {
    if (band.right - band.left >= band.bottom - band.top) {
        return std::abs(band.top - bounds.top) <=
                       std::abs(bounds.bottom - band.bottom)
                   ? ABE_TOP
                   : ABE_BOTTOM;
    }
    return std::abs(band.left - bounds.left) <=
                   std::abs(bounds.right - band.right)
               ? ABE_LEFT
               : ABE_RIGHT;
}

// Windows 11 gives every extra display its own taskbar window. The one that
// belongs to a display is looked up once and then kept: the follow loop runs
// every 60 ms, and walking every top-level window on each tick (what an
// EnumWindows filter would do) is exactly the kind of wake-up cost that shows
// up in Task Manager. Cached, a tick costs one GetWindowRect.
HWND g_secondaryTray;
RECT g_secondaryTrayFor{};
bool g_secondaryTrayResolved;

// The taskbar window sitting on `bounds`, or null when that display has none.
// A display without a taskbar is remembered too, so the follow loop does not
// re-search for it 17 times a second.
HWND SecondaryTrayWindow(const RECT& bounds) {
    if (g_secondaryTrayResolved && EqualRect(&g_secondaryTrayFor, &bounds) &&
        (!g_secondaryTray || IsWindow(g_secondaryTray))) {
        return g_secondaryTray;
    }

    HWND best = nullptr;
    long bestArea = 0;
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExW(nullptr, hwnd, kSecondaryTrayClass, nullptr)) !=
           nullptr) {
        RECT live{};
        RECT clipped{};
        if (!GetWindowRect(hwnd, &live) || !ClipToRect(live, bounds, clipped)) {
            continue;
        }
        // The largest overlap is this display's bar; a neighbouring display's
        // bar can graze the edge but never covers the middle of the screen.
        long area =
            (clipped.right - clipped.left) * (clipped.bottom - clipped.top);
        if (area > bestArea) {
            bestArea = area;
            best = hwnd;
        }
    }

    g_secondaryTray = best;
    g_secondaryTrayFor = bounds;
    g_secondaryTrayResolved = true;
    return best;
}

// Drops everything cached about the display topology.
void InvalidateTaskbarCache() {
    g_bandEdge = -1;
    g_secondaryTray = nullptr;
    g_secondaryTrayFor = RECT{};
    g_secondaryTrayResolved = false;
    g_primaryMonitorKnown = false;
}

// The band this display's taskbar occupies right now, in screen pixels.
bool QueryTaskbarBand(HMONITOR monitor, const RECT& bounds, TaskbarBand& out) {
    bool primary = !monitor || monitor == PrimaryMonitor();
    RECT live{};

    bool found = false;
    if (primary) {
        HWND tray = FindWindowW(kTrayClass, nullptr);
        found =
            tray && GetWindowRect(tray, &live) && ClipToRect(live, bounds, out.rect);
    }
    if (!found) {
        HWND tray = SecondaryTrayWindow(bounds);
        found =
            tray && GetWindowRect(tray, &live) && ClipToRect(live, bounds, out.rect);
    }

    if (!found && !primary && g_secondaryTrayResolved && g_secondaryTray) {
        // The cached bar is no longer on this display (it was dragged to
        // another screen): look once more, then keep whatever comes back -
        // including "this display has no taskbar" - so the follow loop stays
        // at one GetWindowRect per tick either way.
        g_secondaryTrayResolved = false;
        HWND tray = SecondaryTrayWindow(bounds);
        found =
            tray && GetWindowRect(tray, &live) && ClipToRect(live, bounds, out.rect);
    }

    if (!found) {
        // No taskbar window on this display. Only the primary taskbar has an
        // ABM target, so a display without its own bar keeps the work area the
        // system reported instead of borrowing the primary's rect.
        if (!primary) {
            return false;
        }
        APPBARDATA pos{};
        pos.cbSize = sizeof(pos);
        if (!SHAppBarMessage(ABM_GETTASKBARPOS, &pos) ||
            !ClipToRect(pos.rc, bounds, out.rect)) {
            return false;
        }
        g_bandEdge = (int)pos.uEdge;
        out.edge = (int)pos.uEdge;
        out.valid = true;
        return true;
    }

    // A secondary taskbar has no ABM target of its own, but it always hugs one
    // edge of the display it sits on, so the geometry alone is enough there.
    int edge = primary ? g_bandEdge : -1;
    if (edge < 0 || !BandHugsEdge(out.rect, bounds, edge)) {
        if (primary) {
            APPBARDATA pos{};
            pos.cbSize = sizeof(pos);
            edge = SHAppBarMessage(ABM_GETTASKBARPOS, &pos) ? (int)pos.uEdge : -1;
            if (edge < 0 || !BandHugsEdge(out.rect, bounds, edge)) {
                edge = NearestEdge(out.rect, bounds);
            }
            g_bandEdge = edge;
        } else {
            edge = NearestEdge(out.rect, bounds);
        }
    }
    out.edge = edge;
    out.valid = true;
    return true;
}

// The part of the display the card may use: the monitor minus the band the
// taskbar occupies right now. Collapsed that band is the 2 px reveal strip, so
// the card ends up right at the screen edge.
RECT UsableRect(const RECT& bounds, const TaskbarBand& band) {
    if (!band.valid) {
        return bounds;
    }
    RECT r = bounds;
    switch (band.edge) {
        case ABE_TOP:
            r.top = std::max(r.top, band.rect.bottom);
            break;
        case ABE_LEFT:
            r.left = std::max(r.left, band.rect.right);
            break;
        case ABE_RIGHT:
            r.right = std::min(r.right, band.rect.left);
            break;
        default:
            r.bottom = std::min(r.bottom, band.rect.top);
            break;
    }
    // A band that swallowed the display would leave nowhere to put the card.
    if (r.right - r.left < 2 || r.bottom - r.top < 2) {
        return bounds;
    }
    return r;
}

// Only an auto-hidden taskbar moves while the card is up; pinned, it keeps
// still and there is nothing to follow.
bool TaskbarAutoHide() {
    APPBARDATA state{};
    state.cbSize = sizeof(state);
    return (SHAppBarMessage(ABM_GETSTATE, &state) & ABS_AUTOHIDE) != 0;
}

// The display the card is drawn on. Resolved once per tip so the card lands on
// the monitor the user is actually looking at, with that monitor's own scale.
struct MonitorTarget {
    HMONITOR handle = nullptr;
    RECT bounds{0, 0, 0, 0}; // whole monitor, in virtual-screen coordinates
    RECT work{0, 0, 0, 0};   // same, minus the taskbar as it is right now
    TaskbarBand band;        // the taskbar itself, for the follow loop
    float scale = 1.0f;
    bool valid = false;

    int Width() const {
        return (int)std::max<LONG>(1, bounds.right - bounds.left);
    }
    int Height() const {
        return (int)std::max<LONG>(1, bounds.bottom - bounds.top);
    }
};

MonitorTarget g_monitor;

bool LoadMonitor(HMONITOR handle, MonitorTarget& out) {
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!handle || !GetMonitorInfoW(handle, &info)) {
        return false;
    }
    out.handle = handle;
    out.bounds = info.rcMonitor;
    out.work = info.rcWork;
    out.band = TaskbarBand{};
    if (QueryTaskbarBand(handle, out.bounds, out.band)) {
        out.work = UsableRect(out.bounds, out.band);
    }
    out.scale = ScaleForMonitor(handle);
    out.valid = true;
    return true;
}

// Where the card goes. Focus (the default) is the display of the active window
// - the one the user is working in - with the pointer's display and then the
// primary one as fallbacks; the same chain as the standalone build's
// MonitorArea.Focused(), so both editions land in the same place.
HMONITOR PickMonitor(int mode) {
    if (mode == kMonitorFocus) {
        HWND foreground = GetForegroundWindow();
        if (foreground) {
            HMONITOR monitor =
                MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
            if (monitor) {
                return monitor;
            }
        }
    }
    if (mode != kMonitorPrimary) {
        POINT cursor{};
        if (GetCursorPos(&cursor)) {
            HMONITOR monitor =
                MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
            if (monitor) {
                return monitor;
            }
        }
    }
    return PrimaryMonitor();
}

bool ResolveTargetMonitor() {
    MonitorTarget target;
    if (!LoadMonitor(PickMonitor(g_settings.monitor), target)) {
        return false;
    }
    g_monitor = target;
    return true;
}

// =============================================================================
// Placement (port of UI/OsdPlacement.cs, made monitor-aware)
// =============================================================================

struct Placement {
    int x;
    int y;
    float dx;  // slide-in offset, in physical pixels
    float dy;
};

enum PopSide { kPopNone, kPopLeft, kPopRight, kPopTop, kPopBottom };

PopSide ActiveSide(const MonitorTarget& mon, float left, float top, float w,
                   float h) {
    float screenW = (float)mon.Width();
    float screenH = (float)mon.Height();
    float centerX = left + w / 2.0f - (float)mon.bounds.left;
    float centerY = top + h / 2.0f - (float)mon.bounds.top;

    float zoneW = screenW * kZoneRatio;
    if (centerX < zoneW) {
        return kPopLeft;
    }
    if (centerX > screenW - zoneW) {
        return kPopRight;
    }
    float zoneH = screenH * kZoneRatio;
    if (centerY < zoneH) {
        return kPopTop;
    }
    if (centerY > screenH - zoneH) {
        return kPopBottom;
    }
    return kPopNone;
}

// Whether the card hangs off the top edge of its monitor. The accent bar then
// mirrors above the label, so it always sits on the edge the card is anchored
// to instead of being wedged between the text and the screen content. The
// split is the monitor's vertical middle — the same threshold the slide
// direction falls back to when the card sits away from every edge.
bool CardHangsTop(const MonitorTarget& mon, int x, int y, float w, float h) {
    // An unresolved monitor leaves bounds at zero, so fall back to the primary
    // screen height the settings were computed against.
    float height = mon.valid ? (float)mon.Height() : 0.0f;
    float originY = mon.valid ? (float)mon.bounds.top : 0.0f;
    if (height <= 1.0f) {
        height = (float)GetSystemMetrics(SM_CYSCREEN);
        originY = 0.0f;
    }
    float centerY = (float)y + h / 2.0f - originY;
    return centerY < height / 2.0f;
}

// Computes the card position for one monitor plus the slide offset that matches
// the nearest edge of that monitor. Kept free of globals so the standalone
// build can exercise it against made-up monitor layouts.
Placement ComputePlacement(const MonitorTarget& mon, int vertical, int horizontal,
                           int offsetX, int offsetY, float w, float h,
                           float scale) {
    float gap = kGap * scale;
    const RECT& bounds = mon.bounds;
    const RECT& work = mon.work;

    float left;
    if (horizontal == 0) {
        left = (float)work.left + gap;
    } else if (horizontal == 2) {
        left = (float)work.right - gap - w;
    } else {
        left = (float)bounds.left + ((float)mon.Width() - w) / 2.0f;
    }

    // `work` is the monitor minus the band the taskbar occupies right now, so
    // this sits above a slid-out taskbar and right at the edge when it is
    // collapsed - see QueryTaskbarBand/UsableRect.
    float top = vertical == 0 ? (float)work.top + gap
                              : (float)work.bottom - gap - h;

    left += (float)offsetX;
    top += (float)offsetY;

    float maxLeft = std::max((float)bounds.left, (float)bounds.right - w);
    float maxTop = std::max((float)bounds.top, (float)bounds.bottom - h);
    left = std::min(std::max(left, (float)bounds.left), maxLeft);
    top = std::min(std::max(top, (float)bounds.top), maxTop);

    Placement p{};
    p.x = (int)std::lround(left);
    p.y = (int)std::lround(top);

    float distance = kSlideDistance * scale;
    switch (ActiveSide(mon, left, top, w, h)) {
        case kPopLeft:
            p.dx = -distance;
            break;
        case kPopRight:
            p.dx = distance;
            break;
        case kPopTop:
            p.dy = -distance;
            break;
        case kPopBottom:
            p.dy = distance;
            break;
        default:
            if (top + h / 2.0f < (float)(bounds.top + bounds.bottom) / 2.0f) {
                p.dy = -distance;
            } else {
                p.dy = distance;
            }
            break;
    }
    return p;
}

Placement ComputePlacement(float w, float h, float scale) {
    return ComputePlacement(g_monitor, g_settings.vertical,
                            g_settings.horizontal, g_settings.offsetX,
                            g_settings.offsetY, w, h, scale);
}

// =============================================================================
// Drawing primitives
// =============================================================================

void AddRoundRect(GraphicsPath& path, RectF rect, float radius) {
    if (radius <= 0.0f) {
        path.AddRectangle(rect);
        return;
    }
    float d = radius * 2.0f;
    if (d > rect.Width) {
        d = rect.Width;
    }
    if (d > rect.Height) {
        d = rect.Height;
    }
    path.AddArc(rect.X, rect.Y, d, d, 180.0f, 90.0f);
    path.AddArc(rect.X + rect.Width - d, rect.Y, d, d, 270.0f, 90.0f);
    path.AddArc(rect.X + rect.Width - d, rect.Y + rect.Height - d, d, d, 0.0f,
                90.0f);
    path.AddArc(rect.X, rect.Y + rect.Height - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

// Lock icon, ported from the XAML path data on a 24x24 canvas and scaled to
// `size`. `angle` opens the shackle (25 degrees when off), `bounce` is the
// small downward kick applied while the shackle settles.
void DrawLock(Graphics& g, RectF box, const Theme& theme, float angle,
              float bounce) {
    float s = box.Width / 24.0f;
    GraphicsState state = g.Save();
    g.TranslateTransform(box.X, box.Y);
    g.ScaleTransform(s, s);

    SolidBrush brush(theme.text);

    // Body: outer rounded rect minus an inner one, 1.5 units thick.
    GraphicsPath body(FillModeAlternate);
    AddRoundRect(body, RectF(4.0f, 7.0f, 16.0f, 14.0f), 3.25f);
    AddRoundRect(body, RectF(5.5f, 8.5f, 13.0f, 11.0f), 1.75f);
    g.FillPath(&brush, &body);

    // Keyhole.
    g.FillEllipse(&brush, RectF(10.5f, 12.5f, 3.0f, 3.0f));

    // Shackle: a 1.5-unit arch. The XAML version is a filled ring built from two
    // arcs; drawing it as a centred stroke is pixel-identical and avoids the
    // way GDI+ starts a new figure on every AddArc call.
    GraphicsState shackleState = g.Save();
    g.TranslateTransform(16.0f, 7.0f + bounce);
    g.RotateTransform(angle);
    g.TranslateTransform(-16.0f, -7.0f);

    GraphicsPath shackle;
    shackle.StartFigure();
    shackle.AddLine(PointF(8.75f, 7.0f), PointF(8.75f, 5.0f));
    // Semicircle of radius 3.25 approximated by two cubic Beziers; AddBezier
    // never splits the figure, unlike AddArc.
    const float k = 0.5523f * 3.25f;
    shackle.AddBezier(PointF(8.75f, 5.0f), PointF(8.75f, 5.0f - k),
                      PointF(12.0f - k, 1.75f), PointF(12.0f, 1.75f));
    shackle.AddBezier(PointF(12.0f, 1.75f), PointF(12.0f + k, 1.75f),
                      PointF(15.25f, 5.0f - k), PointF(15.25f, 5.0f));
    shackle.AddLine(PointF(15.25f, 5.0f), PointF(15.25f, 7.0f));

    Pen shacklePen(theme.text, 1.5f);
    shacklePen.SetStartCap(LineCapFlat);
    shacklePen.SetEndCap(LineCapFlat);
    g.DrawPath(&shacklePen, &shackle);

    g.Restore(shackleState);
    g.Restore(state);
}

// Globe drawn as strokes: an outline, a vertical ellipse for the meridians and
// a horizon. Ported from "M3,11 A8,8 … M11,3 A4.5,8 … M3,11 H19" at 1.5.
void DrawGlobe(Graphics& g, RectF box, const Theme& theme) {
    float s = box.Width / 24.0f;
    GraphicsState state = g.Save();
    g.TranslateTransform(box.X, box.Y);
    g.ScaleTransform(s, s);

    Pen pen(theme.text, 1.5f);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    pen.SetLineJoin(LineJoinRound);

    // Circle r=8 at (11,11); meridian ellipse rx=4.5, ry=8; horizon y=11.
    g.DrawEllipse(&pen, RectF(3.0f, 3.0f, 16.0f, 16.0f));
    g.DrawEllipse(&pen, RectF(6.5f, 3.0f, 9.0f, 16.0f));
    g.DrawLine(&pen, PointF(3.0f, 11.0f), PointF(19.0f, 11.0f));

    g.Restore(state);
}

// Check mark drawn on progressively: `progress` 0 draws nothing, 1 draws the
// whole stroke. Ported from the "M5,12.5 L10,17.5 L19,7" polyline.
void DrawCheck(Graphics& g, RectF box, const Theme& theme, float progress) {
    float s = box.Width / 24.0f;
    GraphicsState state = g.Save();
    g.TranslateTransform(box.X, box.Y);
    g.ScaleTransform(s, s);

    const PointF a(5.0f, 12.5f);
    const PointF b(10.0f, 17.5f);
    const PointF c(19.0f, 7.0f);
    float len1 = std::sqrt((b.X - a.X) * (b.X - a.X) + (b.Y - a.Y) * (b.Y - a.Y));
    float len2 = std::sqrt((c.X - b.X) * (c.X - b.X) + (c.Y - b.Y) * (c.Y - b.Y));
    float total = len1 + len2;
    float drawn = total * Clamp01(progress);

    if (drawn > 0.01f) {
        GraphicsPath path;
        path.StartFigure();
        path.AddLine(a, drawn <= len1
                            ? PointF(Lerp(a.X, b.X, drawn / len1),
                                     Lerp(a.Y, b.Y, drawn / len1))
                            : b);
        if (drawn > len1) {
            float t = (drawn - len1) / len2;
            path.AddLine(b, PointF(Lerp(b.X, c.X, t), Lerp(b.Y, c.Y, t)));
        }

        Pen pen(theme.text, 2.4f);
        pen.SetStartCap(LineCapRound);
        pen.SetEndCap(LineCapRound);
        pen.SetLineJoin(LineJoinRound);
        g.DrawPath(&pen, &path);
    }

    g.Restore(state);
}

// Scissors glyph from Segoe Fluent Icons (E8C6).
void DrawScissors(Graphics& g, RectF box, const Theme& theme, float alpha) {
    float s = box.Width / 24.0f;
    GraphicsState state = g.Save();
    g.TranslateTransform(box.X, box.Y);
    g.ScaleTransform(s, s);

    BYTE a = (BYTE)(255 * Clamp01(alpha));
    SolidBrush brush(Color(a, GetRValue(theme.text.ToCOLORREF()),
                           GetGValue(theme.text.ToCOLORREF()),
                           GetBValue(theme.text.ToCOLORREF())));

    FontFamily family(L"Segoe Fluent Icons");
    if (family.GetLastStatus() != Ok) {
        FontFamily fallback(L"Segoe MDL2 Assets");
        if (fallback.GetLastStatus() != Ok) {
            g.Restore(state);
            return;
        }
        Font font(&fallback, 16.0f, FontStyleRegular, UnitPixel);
        StringFormat format;
        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentCenter);
        g.DrawString(L"\xE8C6", -1, &font, RectF(0.0f, 3.0f, 24.0f, 18.0f),
                     &format, &brush);
        g.Restore(state);
        return;
    }

    Font font(&family, 16.0f, FontStyleRegular, UnitPixel);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);
    g.DrawString(L"\xE8C6", -1, &font, RectF(0.0f, 3.0f, 24.0f, 18.0f), &format,
                 &brush);
    g.Restore(state);
}

// HarmonyOS Sans is the preferred UI face (matches the WinTips app), but GDI has
// no graceful fallback of its own: an unknown lfFaceName silently lands on
// whatever the mapper picks (usually Arial), which looks nothing like the WPF
// original. So each candidate is probed once and Segoe UI is the last resort.
//
// SC comes first on purpose. The Windows builds of plain "HarmonyOS Sans" carry
// Latin glyphs only, so Chinese there falls through to GDI font linking and the
// two scripts end up on different files with mismatched metrics. "HarmonyOS Sans
// SC" holds Latin and CJK in one face, which is what the WPF side uses too.
// EnumFontFamiliesExW wants a FONTENUMPROCW, and that is __stdcall on the 32-bit
// build. A capture-less lambda is __cdecl, and clang refuses the conversion
// outright - MSVC lets it slide, which is why the standalone harness built fine
// while CI failed on [compile:x86]. A real CALLBACK function is the portable way.
int CALLBACK FontProbeProc(const LOGFONTW*, const TEXTMETRICW*, DWORD,
                           LPARAM param) {
    *reinterpret_cast<bool*>(param) = true;
    return 0;  // one hit is enough
}

const wchar_t* UiFaceName() {
    static const wchar_t* face = []() -> const wchar_t* {
        HDC dc = GetDC(nullptr);
        if (!dc) {
            return L"Segoe UI";
        }
        auto installed = [dc](const wchar_t* name) {
            bool found = false;
            LOGFONTW probe{};
            probe.lfCharSet = DEFAULT_CHARSET;
            wcscpy_s(probe.lfFaceName, LF_FACESIZE, name);
            EnumFontFamiliesExW(dc, &probe, FontProbeProc,
                                reinterpret_cast<LPARAM>(&found), 0);
            return found;
        };
        const wchar_t* result = L"Segoe UI";
        if (installed(L"HarmonyOS Sans SC")) {
            result = L"HarmonyOS Sans SC";
        } else if (installed(L"HarmonyOS Sans")) {
            result = L"HarmonyOS Sans";  // Latin-only build: CJK falls back
        }
        ReleaseDC(nullptr, dc);
        return result;
    }();
    return face;
}

// Segoe UI Variable is a variable font, which GDI+ can't select a weight for;
// a LOGFONT with lfWeight gives an exact 500/600 weight on Segoe UI instead.
// HarmonyOS Sans ships as discrete faces (Thin/Light/Regular/Medium/Bold/Black),
// so lfWeight 500/600 maps onto Medium/Bold the same way.
// The same LOGFONT feeds both the HFONT used for measuring and the GDI+ Font
// used for drawing, so the two never disagree about metrics.
LOGFONTW MakeUiLogFont(float pixelSize, int weight) {
    LOGFONTW lf{};
    lf.lfHeight = -(LONG)std::lround(pixelSize);
    lf.lfWeight = weight;
    lf.lfQuality = ANTIALIASED_QUALITY;
    lf.lfCharSet = DEFAULT_CHARSET;
    wcscpy_s(lf.lfFaceName, LF_FACESIZE, UiFaceName());
    return lf;
}

// GDI+ MeasureString pads its result by roughly a sixth of an em, which would
// inflate every card. GetTextExtentPoint32 reports the real advance width of
// the same font, so the card width matches the WPF original.
float MeasureTextWidth(HDC dc, const LOGFONTW& logFont, const std::wstring& text) {
    if (text.empty() || !dc) {
        return 0.0f;
    }
    HFONT font = CreateFontIndirectW(&logFont);
    if (!font) {
        return 0.0f;
    }
    HGDIOBJ previous = SelectObject(dc, font);
    SIZE size{};
    GetTextExtentPoint32W(dc, text.c_str(), (INT)text.size(), &size);
    SelectObject(dc, previous);
    DeleteObject(font);
    return (float)size.cx;
}

// DirectWrite text, defined with the surface code below: it rasterises the
// glyphs and blends them straight into the card's DIB, which is only known
// there - the same forward-declaration split PaintCardBackground uses above.
// DWrite is what the native Windows 11 OSD draws its labels with; GDI+'s
// grid-fit aliasing is what issue #6977 reported as pixelated text.
bool DWriteAvailable();
float MeasureTextWidthDWrite(const std::wstring& text, float size, int weight);
float DrawTextDWrite(const std::wstring& text, float size, int weight,
                     const Color& color, float alphaScale, float x,
                     float centerY, bool centerAlign);

// =============================================================================
// Frame rendering
// =============================================================================

struct FrameState {
    TipKind kind = kCapsLock;
    bool on = false;
    bool prevOn = false;   // pose the animation starts from
    float statusT = 1.0f;  // 0 = start pose, 1 = target pose
    float clipT = 1.0f;    // check mark draw progress
    float badgeScale = 1.0f;
    float badgeAlpha = 1.0f;
    int count = 0;
};

struct LayoutMetrics {
    float width;
    float height;
    float iconSize;
    float textX;
    float textY;
    float textWidth;
    float badgeX;
    float badgeWidth;
    float indicatorY;
};

bool IsToggleKind(TipKind kind) {
    return kind == kCapsLock || kind == kNumLock || kind == kScrollLock;
}

bool IsClipKind(TipKind kind) {
    return kind == kCopy || kind == kPaste || kind == kCut;
}

// The language card is the one kind with neither a lock pose nor a count: a
// globe, one line of text, no accent bar.
bool IsLanguageKind(TipKind kind) {
    return kind == kLanguage;
}

std::wstring CountText(int count) {
    wchar_t buf[16];
    swprintf(buf, 16, L"\u00D7%d", count);
    return buf;
}

// Fills the card shape: either the captured backdrop with a tint on top
// (acrylic) or a flat surface color. Defined after the Surface type below.
void PaintCardBackground(Graphics& g, const GraphicsPath& card, float width,
                         float height, const Theme& theme);

// Lays out and draws the card into a Graphics that is already offset so that
// (0,0) is the top-left corner of the window. `barOnTop` mirrors the accent bar
// above the label for a card that hangs off the top edge of its monitor (see
// CardHangsTop).
void RenderCard(Graphics& g, float scale, float width, float height,
                const FrameState& f, const Theme& theme, bool barOnTop) {
    std::wstring label;
    FormatTip(f.kind, f.on, label);

    // --- fonts -------------------------------------------------------------
    // DirectWrite draws the label when it can be reached; the GDI+ path below
    // is the fallback, and it keeps the font objects only then.
    bool useDWrite = DWriteAvailable();
    HDC screenDc = GetDC(nullptr);
    LOGFONTW textLogFont = MakeUiLogFont(14.0f * scale, 500);
    LOGFONTW badgeLogFont = MakeUiLogFont(12.5f * scale, 600);

    // `clip` marks the card family (icon, vertical rhythm); `badge` only says
    // whether the ×N pill is drawn and reserves width (issue #7304).
    bool clip = IsClipKind(f.kind);
    bool badge = clip && g_settings.countBadge;
    bool lang = IsLanguageKind(f.kind);
    float textWidth = MeasureTextWidth(screenDc, textLogFont, label);
    if (useDWrite) {
        float width = MeasureTextWidthDWrite(label, 14.0f * scale, 500);
        if (width > 0.0f) {
            textWidth = width;
        }
    }
    float badgeWidth = 0.0f;
    if (badge) {
        badgeWidth =
            MeasureTextWidth(screenDc, badgeLogFont, CountText(f.count)) +
            14.0f * scale;
        if (useDWrite) {
            float width = MeasureTextWidthDWrite(CountText(f.count),
                                                 12.5f * scale, 600);
            if (width > 0.0f) {
                badgeWidth = width + 14.0f * scale;
            }
        }
    }

    Font* textFont = nullptr;
    Font* badgeFont = nullptr;
    if (!useDWrite) {
        textFont = new Font(screenDc, &textLogFont);
        badgeFont = new Font(screenDc, &badgeLogFont);
    }
    ReleaseDC(nullptr, screenDc);

    // --- card --------------------------------------------------------------
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHalf);
    g.SetTextRenderingHint(TextRenderingHintAntiAlias);

    GraphicsPath card;
    AddRoundRect(card, RectF(0.0f, 0.0f, width, height), 8.0f * scale);
    PaintCardBackground(g, card, width, height, theme);

    // --- icon --------------------------------------------------------------
    // Lock-mode content grid is asymmetric: 6 DIP above the label and 10 DIP
    // below, because the accent bar owns the bottom 10 DIP. Mirroring the bar
    // swaps that to 10/6, which drops the region's centre 4 DIP — the icon and
    // label follow it. Clipboard cards have no bar and stay symmetric.
    float gridShift = (!clip && !lang && barOnTop) ? 4.0f * scale : 0.0f;

    float iconSize = 22.0f * scale;
    RectF iconBox(16.0f * scale,
                  (height - iconSize) / 2.0f - 1.0f * scale + gridShift,
                  iconSize, iconSize);

    float poseT = EaseOutQuad(f.statusT);
    float pose = Lerp(f.prevOn ? 1.0f : 0.0f, f.on ? 1.0f : 0.0f, poseT);
    if (lang) {
        // No pose to animate: the card only says which input is active now.
        DrawGlobe(g, iconBox, theme);
    } else if (clip) {
        if (f.kind == kCut) {
            DrawScissors(g, iconBox, theme, EaseOutQuad(f.clipT));
        } else {
            DrawCheck(g, iconBox, theme, EaseOutQuad(f.clipT));
        }
    } else {
        float angle = Lerp(25.0f, 0.0f, pose);
        float t = Clamp01(f.statusT);
        float bounce = t < 0.1f ? EaseOutCubic(t / 0.1f)
                                : 1.0f - EaseInOutCubic((t - 0.1f) / 0.9f);
        DrawLock(g, iconBox, theme, angle, bounce);
    }

    // --- text --------------------------------------------------------------
    // The hint matters only for the GDI+ fallback now, and AntiAlias (without
    // the grid fit) is the best it can do - see the DirectWrite section below.
    g.SetTextRenderingHint(TextRenderingHintAntiAlias);
    SolidBrush textBrush(theme.text);
    StringFormat textFormat;
    textFormat.SetFormatFlags(StringFormatFlagsNoWrap | StringFormatFlagsNoClip);
    textFormat.SetAlignment(StringAlignmentNear);
    textFormat.SetLineAlignment(StringAlignmentCenter);

    float textX;
    if (badge) {
        const float gridLeft = 16.0f * scale;
        const float iconEnd = 22.0f * scale;
        float badgeStart = width - gridLeft - 8.0f * scale - badgeWidth;
        float mid = (iconEnd + badgeStart) / 2.0f;
        textX = std::max(iconEnd + 8.0f * scale, mid - textWidth / 2.0f);
        textX += gridLeft;
    } else if (clip) {
        // Badge hidden (issue #7304): center the label on the card itself, the
        // way the badge-on layout lands, instead of the icon-gutter offset the
        // lock cards use. The max keeps a gap when a label runs long.
        textX = std::max((16.0f + 22.0f + 8.0f) * scale,
                         width / 2.0f - textWidth / 2.0f);
    } else {
        // Centered in the content grid, offset by the icon gutter.
        float contentLeft = 16.0f * scale;
        float contentRight = width - 12.0f * scale;
        textX = (contentLeft + contentRight) / 2.0f - textWidth / 2.0f +
                18.0f * scale / 2.0f;
    }

    // Vertical: the exe's XAML maths puts the label centre at ~22, but that
    // reads as high on the rendered card (ink sits 3 px above centre), so the
    // mod keeps the visually centred positions users are used to.
    float textY = (clip ? height / 2.0f - 1.0f * scale
                        : height / 2.0f - 2.0f * scale + gridShift);
    // NoWrap keeps a slightly narrower GDI+ measure from breaking the label
    // into two lines, so the rect only needs to be tall enough for one line.
    RectF textRect(textX, textY - 14.0f * scale, width - textX + 8.0f * scale,
                   28.0f * scale);
    if (useDWrite) {
        // DirectWrite writes straight into the surface pixels, so everything
        // GDI+ has queued must land in the DIB first.
        g.Flush();
        DrawTextDWrite(label, 14.0f * scale, 500, theme.text, 1.0f,
                       textX + textWidth / 2.0f, textY, true);
    } else {
        g.DrawString(label.c_str(), (INT)label.size(), textFont, textRect,
                     &textFormat, &textBrush);
    }

    // --- clipboard badge ---------------------------------------------------
    if (badge) {
        // Geometry copied from the exe's OsdWindow.xaml: the pill is right
        // aligned inside the content grid (right inset 12, no extra margin),
        // vertically centred in the grid band [6, 40] -> centre 23 DIP, and
        // sized like the Border there: 12.5 px text line + 2 px padding on
        // each side (~20.5 DIP tall, corner radius 9).
        float badgeHeight = 20.5f * scale;
        float badgeX = width - 12.0f * scale - badgeWidth;
        float badgeY = (23.0f - badgeHeight / 2.0f) * scale;

        GraphicsState badgeState = g.Save();
        g.TranslateTransform(badgeX + badgeWidth / 2.0f, badgeY + badgeHeight / 2.0f);
        g.ScaleTransform(f.badgeScale, f.badgeScale);
        g.TranslateTransform(-(badgeX + badgeWidth / 2.0f),
                             -(badgeY + badgeHeight / 2.0f));

        GraphicsPath badge;
        AddRoundRect(badge, RectF(badgeX, badgeY, badgeWidth, badgeHeight),
                     9.0f * scale);
        Color accent = theme.accent;
        accent = Color((BYTE)(accent.GetA() * Clamp01(f.badgeAlpha)),
                       accent.GetR(), accent.GetG(), accent.GetB());
        SolidBrush badgeBrush(accent);
        g.FillPath(&badgeBrush, &badge);

        Color onAccent = theme.onAccent;
        onAccent = Color((BYTE)(onAccent.GetA() * Clamp01(f.badgeAlpha)),
                         onAccent.GetR(), onAccent.GetG(), onAccent.GetB());
        SolidBrush badgeText(onAccent);
        StringFormat badgeFormat;
        badgeFormat.SetFormatFlags(StringFormatFlagsNoClip);
        badgeFormat.SetAlignment(StringAlignmentCenter);
        badgeFormat.SetLineAlignment(StringAlignmentCenter);
        std::wstring countText = CountText(f.count);
        if (useDWrite) {
            g.Flush();
            // The scale transform around the badge centre is reproduced by
            // growing the font itself: scaling about the badge's centre keeps
            // the midpoint, and the alpha follows the same animation value.
            DrawTextDWrite(countText, 12.5f * scale * Clamp01(f.badgeScale),
                           600, theme.onAccent, Clamp01(f.badgeAlpha),
                           badgeX + badgeWidth / 2.0f,
                           badgeY + badgeHeight / 2.0f, true);
        } else {
            g.DrawString(countText.c_str(), (INT)countText.size(), badgeFont,
                         RectF(badgeX, badgeY, badgeWidth, badgeHeight),
                         &badgeFormat, &badgeText);
        }

        g.Restore(badgeState);
    }

    // --- accent bar ---------------------------------------------------------
    // No bar on the language card: it has no two states for the bar to show.
    if (!clip && !lang) {
        float indicatorHeight = 3.0f * scale;
        float indicatorWidth = Lerp(36.0f, 60.0f, pose) * scale;
        // Default pose: 7 DIP above the bottom edge. Mirrored: the same 7 DIP
        // below the top edge, so the bar sits above the label.
        float indicatorY = barOnTop
                               ? 7.0f * scale
                               : height - 7.0f * scale - indicatorHeight;
        float indicatorX = (width - indicatorWidth) / 2.0f;
        BYTE alpha = (BYTE)std::lround(Lerp(0.2f, 1.0f, pose) * 255.0f);
        Color accent = theme.accent;
        SolidBrush indicator(
            Color(alpha, accent.GetR(), accent.GetG(), accent.GetB()));
        GraphicsPath bar;
        AddRoundRect(bar,
                     RectF(indicatorX, indicatorY, indicatorWidth,
                           indicatorHeight),
                     indicatorHeight / 2.0f);
        g.FillPath(&indicator, &bar);
    }

    delete textFont;
    delete badgeFont;
}

// Measures the card size for the current tip, in physical pixels.
void MeasureCard(float scale, const FrameState& f, int& outWidth,
                 int& outHeight) {
    std::wstring label;
    FormatTip(f.kind, f.on, label);

    HDC screenDc = GetDC(nullptr);
    LOGFONTW textLogFont = MakeUiLogFont(14.0f * scale, 500);
    LOGFONTW badgeLogFont = MakeUiLogFont(12.5f * scale, 600);

    // Width reserves room for the pill only when the badge setting shows it.
    bool clip = IsClipKind(f.kind);
    bool badge = clip && g_settings.countBadge;
    float textWidth = MeasureTextWidth(screenDc, textLogFont, label);
    if (DWriteAvailable()) {
        float width = MeasureTextWidthDWrite(label, 14.0f * scale, 500);
        if (width > 0.0f) {
            textWidth = width;
        }
    }
    float badgeWidth = 0.0f;
    if (badge) {
        badgeWidth =
            MeasureTextWidth(screenDc, badgeLogFont, CountText(f.count)) +
            14.0f * scale;
        if (DWriteAvailable()) {
            float width =
                MeasureTextWidthDWrite(CountText(f.count), 12.5f * scale, 600);
            if (width > 0.0f) {
                badgeWidth = width + 14.0f * scale;
            }
        }
    }

    float widthDip =
        16.0f + 22.0f + 18.0f + textWidth / scale + 12.0f +
        // The exe reserves MeasureText(count) + 22 for the pill zone; the
        // mod's badgeWidth already includes the pill's 14 px padding, so the
        // equivalent here is badgeWidth + 8.
        (badge ? badgeWidth / scale + 8.0f : 0.0f);
    outWidth = (int)std::lround(std::max(kBaseWidth * scale,
                                         std::ceil(widthDip) * scale));
    outHeight = (int)std::lround(kBaseHeight * scale);

    ReleaseDC(nullptr, screenDc);
}

}  // namespace

namespace {

constexpr UINT_PTR kTimerClip = 5;
constexpr UINT WM_APP_POLL = WM_APP + 4;
constexpr UINT WM_APP_SETTINGS = WM_APP + 5;

// =============================================================================
// Layered ARGB surface
// =============================================================================

struct Surface {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    void* bits = nullptr;
    int width = 0;
    int height = 0;
    int stride = 0;

    ~Surface() {
        Release();
    }

    void Release() {
        if (dc) {
            if (oldBitmap) {
                SelectObject(dc, oldBitmap);
            }
            if (bitmap) {
                DeleteObject(bitmap);
            }
            DeleteDC(dc);
        }
        dc = nullptr;
        bitmap = nullptr;
        oldBitmap = nullptr;
        bits = nullptr;
        width = 0;
        height = 0;
        stride = 0;
    }

    bool Ensure(int w, int h) {
        if (dc && w == width && h == height) {
            return true;
        }
        Release();
        if (w <= 0 || h <= 0) {
            return false;
        }

        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;  // top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        HDC screen = GetDC(nullptr);
        dc = CreateCompatibleDC(screen);
        bitmap = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        ReleaseDC(nullptr, screen);
        if (!dc || !bitmap || !bits) {
            Release();
            return false;
        }

        oldBitmap = SelectObject(dc, bitmap);
        width = w;
        height = h;
        stride = w * 4;
        return true;
    }

    // GDI+ hands back a DIB it has written to as premultiplied BGRA already: a
    // 32-bit DIB opened through an HDC is treated as PARGB, so every blend it
    // performs is composited against the (zeroed) destination and stored
    // pre-multiplied. UpdateLayeredWindow wants exactly that, so no extra pass
    // is needed - running one multiplies by alpha a second time and turns every
    // antialiased edge pixel into a dark fringe.
    void Clear() {
        if (bits) {
            memset(bits, 0, (size_t)stride * height);
        }
    }
};

// =============================================================================
// Blurred backdrop ("acrylic")
//
// A DWM acrylic backdrop needs a non-layered window, and a non-layered window
// can neither fade nor blend per pixel. Instead the area behind the card is
// copied and blurred, and the result is composited into the same ARGB surface,
// so the card keeps real blur *and* full animation.
//
// The copy is taken with a margin around the card, and the drawing code picks
// the part that is behind the card at that moment: the card can slide in and
// follow an auto-hiding taskbar without a new grab. Where Windows allows it
// (10 2004 and later, DWM composing, no remote session) the window is excluded
// from screen capture and the grab repeats while the card is up, so the blur
// moves with the desktop instead of being a frozen photograph.
// =============================================================================

// Where the card is being drawn this frame, in screen pixels. The backdrop is
// sampled at this offset, so it has to be known before the drawing code, which
// sits above the rest of the runtime state.
int g_presentX;
int g_presentY;

struct Backdrop {
    Surface image; // the captured area, screen (x,y) to (x+w, y+h)
    int x = 0;
    int y = 0;
    bool valid = false;

    void Invalidate() {
        valid = false;
    }

    // True while a card of this size at this place is still inside the grab.
    bool Covers(int px, int py, int w, int h) const {
        return valid && w > 0 && h > 0 && px >= x && py >= y &&
               px + w <= x + image.width && py + h <= y + image.height;
    }
};

Backdrop g_backdrop;

// Separable box blur, `passes` rounds of horizontal + vertical. Three passes
// approach a Gaussian closely enough for the one-shot snapshot; the live path
// runs two and widens the radius to pay for it - see kLiveRadiusScale. Stays
// well under a millisecond for a card-sized region either way.
void BlurAxis(BYTE* bits, int width, int height, int radius, bool horizontal) {
    if (!bits || radius <= 0) {
        return;
    }

    const int lineCount = horizontal ? height : width;
    const int lineLength = horizontal ? width : height;
    const int lineStride = horizontal ? width * 4 : 4;
    const int elemStride = horizontal ? 4 : width * 4;
    const int window = radius * 2 + 1;
    const double invWindow = 1.0 / (double)window;
    if (lineLength <= 1 || lineCount <= 0) {
        return;
    }

    std::vector<BYTE> line((size_t)lineLength * 4);
    for (int l = 0; l < lineCount; l++) {
        BYTE* base = bits + (size_t)l * lineStride;
        for (int i = 0; i < lineLength; i++) {
            memcpy(&line[(size_t)i * 4], base + (size_t)i * elemStride, 4);
        }

        int sum[3] = {0, 0, 0};
        for (int i = -radius; i <= radius; i++) {
            int idx = Clamp(i, 0, lineLength - 1);
            const BYTE* px = &line[(size_t)idx * 4];
            sum[0] += px[0];
            sum[1] += px[1];
            sum[2] += px[2];
        }

        for (int i = 0; i < lineLength; i++) {
            BYTE* dst = base + (size_t)i * elemStride;
            // Rounded rather than truncated: a box blur averaging a flat wall
            // loses half a level per pass to truncation, and two passes of that
            // show up as a banded, slightly dirty wash.
            dst[0] = (BYTE)(sum[0] * invWindow + 0.5);
            dst[1] = (BYTE)(sum[1] * invWindow + 0.5);
            dst[2] = (BYTE)(sum[2] * invWindow + 0.5);

            int add = Clamp(i + radius + 1, 0, lineLength - 1);
            int sub = Clamp(i - radius, 0, lineLength - 1);
            const BYTE* addPx = &line[(size_t)add * 4];
            const BYTE* subPx = &line[(size_t)sub * 4];
            sum[0] += addPx[0] - subPx[0];
            sum[1] += addPx[1] - subPx[1];
            sum[2] += addPx[2] - subPx[2];
        }
    }
}

void BlurBackdrop(Surface& image, int radius, int passes) {
    if (!image.bits || radius <= 0) {
        return;
    }
    for (int pass = 0; pass < passes; pass++) {
        BlurAxis((BYTE*)image.bits, image.width, image.height, radius, true);
        BlurAxis((BYTE*)image.bits, image.width, image.height, radius, false);
    }
}

// A screen copy of an elevated window comes back black without an error code,
// so look at the pixels instead of the return value.
bool SurfaceIsBlank(const Surface& image) {
    if (!image.bits) {
        return true;
    }
    const BYTE* base = (const BYTE*)image.bits;
    for (int y = 0; y < image.height; y += 4) {
        const BYTE* row = base + (size_t)y * image.stride;
        for (int x = 0; x < image.width; x += 4) {
            const BYTE* px = row + x * 4;
            if (px[0] | px[1] | px[2]) {
                return false;
            }
        }
    }
    return true;
}

// One grab: the card's rectangle grown by `pad` on every side, blurred in
// place. The margin is what lets the card move without a new grab - the
// drawing code samples the part of it that is behind the card at the time.
bool CaptureBackdrop(HDC source, int x, int y, int w, int h, int radius,
                     int pad) {
    g_backdrop.Invalidate();
    if (!source || w <= 0 || h <= 0) {
        return false;
    }
    if (pad < 0) {
        pad = 0;
    }
    const int grabW = w + pad * 2;
    const int grabH = h + pad * 2;
    const int grabX = x - pad;
    const int grabY = y - pad;
    if (!g_backdrop.image.Ensure(grabW, grabH)) {
        return false;
    }
    if (!BitBlt(g_backdrop.image.dc, 0, 0, grabW, grabH, source, grabX, grabY,
                SRCCOPY | CAPTUREBLT)) {
        return false;
    }
    if (SurfaceIsBlank(g_backdrop.image)) {
        return false;
    }

    BlurBackdrop(g_backdrop.image, radius, kStaticBlurPasses);
    g_backdrop.x = grabX;
    g_backdrop.y = grabY;
    g_backdrop.valid = true;
    return true;
}

// target = target * weight + previous * (256 - weight), per pixel, BGR only.
//
// A grab waits for a composed frame, so the live path tops out well below the
// refresh rate; against a video or a scrolling page every frame is a visible
// jump. Blending towards the previous frame turns that into a smooth ramp,
// which is also what frosted glass is supposed to look like: whatever moves
// behind it comes through as a soft wash.
void BlendPixels(const BYTE* previous, BYTE* target, int bytes, int weight) {
    if (!previous || !target || weight >= 256) {
        return;
    }
    if (weight < 0) {
        weight = 0;
    }
    const int back = 256 - weight;
    for (int i = 0; i + 3 < bytes; i += 4) {
        target[i] = (BYTE)((target[i] * weight + previous[i] * back) >> 8);
        target[i + 1] =
            (BYTE)((target[i + 1] * weight + previous[i + 1] * back) >> 8);
        target[i + 2] =
            (BYTE)((target[i + 2] * weight + previous[i + 2] * back) >> 8);
    }
}

void PaintCardBackground(Graphics& g, const GraphicsPath& card, float width,
                         float height, const Theme& theme) {
    const Surface& image = g_backdrop.image;
    const int cardW = (int)std::lround(width);
    const int cardH = (int)std::lround(height);
    bool acrylic = g_settings.material != kMaterialSolid && g_backdrop.valid &&
                   image.bits && cardW > 0 && cardH > 0 &&
                   cardW <= image.width && cardH <= image.height;
    if (acrylic) {
        Bitmap bitmap(image.width, image.height, image.stride,
                      PixelFormat32bppRGB, (BYTE*)image.bits);
        // The card is somewhere inside the grab; clamp so a card that has slid
        // past the margin shows the nearest edge rather than an empty block.
        const int offsetX = Clamp(g_presentX - g_backdrop.x, 0,
                                  image.width - cardW);
        const int offsetY = Clamp(g_presentY - g_backdrop.y, 0,
                                  image.height - cardH);
        GraphicsState state = g.Save();
        g.SetClip(&card);
        g.DrawImage(&bitmap, RectF(0.0f, 0.0f, width, height), (REAL)offsetX,
                    (REAL)offsetY, (REAL)cardW, (REAL)cardH, UnitPixel);
        g.Restore(state);

        SolidBrush tint(theme.tint);
        g.FillPath(&tint, &card);
    } else {
        SolidBrush surface(theme.surface);
        g.FillPath(&surface, &card);
    }

    // The glass edge is lit from above and fades out towards the bottom: the
    // eye decides "this is a pane of glass" mostly from the boundary, and a
    // border at one flat alpha reads like a sticker instead. Solid keeps the
    // original flat border - there is no glass to catch the light.
    if (acrylic && !theme.highContrast) {
        LinearGradientBrush lit(
            PointF(0.0f, 0.0f), PointF(0.0f, height),
            theme.dark ? Color(0x3D, 0xFF, 0xFF, 0xFF)
                       : Color(0x4A, 0xFF, 0xFF, 0xFF),
            theme.dark ? Color(0x0F, 0xFF, 0xFF, 0xFF)
                       : Color(0x14, 0x00, 0x00, 0x00));
        Pen border(&lit, 1.0f);
        g.DrawPath(&border, &card);
    } else {
        Pen border(theme.border, 1.0f);
        g.DrawPath(&border, &card);
    }
}

// =============================================================================
// Input state
// =============================================================================

// What tells the language card whether anything changed. Language and IME mode
// belong to the *thread* that owns the input, not to the top-level window, and
// the thread that owns it isn't always the one behind GetForegroundWindow: a
// fullscreen game hands input to a child window, a UWP game to a CoreWindow in
// its host process, and a launcher overlay (Steam, Discord, a recorder) may sit
// in front of both. Reading the language off the top-level window then returns
// the same value forever and the card never appears. GetGUIThreadInfo answers
// for the foreground thread, which is the one that matters.
struct InputState {
    DWORD threadId = 0;
    DWORD processId = 0;
    HKL hkl = nullptr;
    HWND imeWnd = nullptr;
    int conversionMode = -1; // -1 = no IME window, or it refused to answer

    bool Usable() const {
        return hkl != nullptr;
    }
    bool HasIme() const {
        return imeWnd != nullptr && conversionMode >= 0;
    }
    bool IsNative() const {
        return (conversionMode & IME_CMODE_NATIVE) != 0;
    }
};

HWND FocusWindow() {
    GUITHREADINFO info{};
    info.cbSize = sizeof(info);
    // idThread 0 means "the foreground thread".
    if (GetGUIThreadInfo(0, &info)) {
        if (info.hwndFocus) {
            return info.hwndFocus;
        }
        if (info.hwndActive) {
            return info.hwndActive;
        }
    }
    return GetForegroundWindow();
}

// WM_IME_CONTROL's "get conversion mode" sub-command: IMEs all implement it,
// but the SDK headers only ship the neighbouring values, so spell it out.
constexpr WPARAM kImeGetConversionMode = 0x0001;

// Synchronous message with a short timeout: the target thread can be busy
// loading a level, and a language tip is never worth stalling the hook for.
int ReadConversionMode(HWND imeWnd) {
    DWORD_PTR result = 0;
    if (!SendMessageTimeoutW(imeWnd, WM_IME_CONTROL, kImeGetConversionMode, 0,
                             SMTO_ABORTIFHUNG, 40, &result)) {
        return -1;
    }
    return (int)result;
}

// The IME mode labels ("Chinese input" / "English input") are Chinese-only, and
// every GUI thread has a default IME window whatever its layout, so a Japanese
// or Korean IME's mode toggle would be described with the wrong words. Ask for
// the mode only where the label makes sense.
bool ChineseLayout(HKL hkl) {
    return PRIMARYLANGID(LOWORD((ULONG_PTR)hkl)) == LANG_CHINESE;
}

InputState CurrentInputState() {
    InputState state;
    HWND hwnd = FocusWindow();
    if (!hwnd) {
        return state;
    }
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (!tid) {
        return state;
    }
    HKL hkl = GetKeyboardLayout(tid);
    if (!hkl) {
        return state;
    }
    state.threadId = tid;
    state.processId = pid;
    state.hkl = hkl;
    if (ChineseLayout(hkl)) {
        state.imeWnd = ImmGetDefaultIMEWnd(hwnd);
        if (state.imeWnd) {
            state.conversionMode = ReadConversionMode(state.imeWnd);
        }
    }
    return state;
}

// =============================================================================
// Runtime state
// =============================================================================

HWND g_hwnd;
HHOOK g_hook;
DWORD g_hookThreadId;
HANDLE g_uiThread;
HANDLE g_hookThread;
HANDLE g_readyEvent;
ULONG_PTR g_gdiplusToken;

Surface g_surface;
float g_scale = 1.0f;     // the target display's DPI scale
float g_cardScale = 1.0f; // g_scale times the card size setting
Theme g_theme;
FrameState g_frame;
Placement g_placement; // where the card belongs with the taskbar as it is now
int g_posX;            // where it is actually drawn, eased toward g_placement
int g_posY;
bool g_following;      // an ease toward g_placement is in flight
float g_followFromX;
float g_followFromY;
ULONGLONG g_followStart;
int g_followMs;
bool g_backdropPending; // moved since the last screen copy
int g_cardWidth;
int g_cardHeight;

enum Phase { kPhaseHidden, kPhaseIn, kPhaseHold, kPhaseOut };
Phase g_phase = kPhaseHidden;
ULONGLONG g_phaseStart;
int g_animMs;
int g_statusMs;
ULONGLONG g_statusStart;
int g_clipMs;
ULONGLONG g_clipStart;
ULONGLONG g_badgeStart;
bool g_badgePop;
bool g_badgeFade;
int g_count;
TipKind g_currentKind = kCapsLock;
ULONGLONG g_lastShowTick;
bool g_clipIntentActive;
TipKind g_clipIntentKind = kCopy;

// Hook thread only, and only as an auto-repeat filter: a key-down for a key
// that is already in here is the keyboard repeating, not a new press. It is
// deliberately not used to answer "is Ctrl down" - a key-up the hook never sees
// (the secure desktop, an elevated window) would stay in here forever.
bool g_vkDown[256];

// The lock state itself, and the one place it is allowed to change.
//
// It is not read from the system at press time. GetKeyState answers out of the
// calling thread's own copy of the key state: it starts as the live state when
// the thread is created and only moves forward as that thread processes
// keyboard input. The hook thread never processes any, so its copy is frozen at
// load time and four taps in a row would read back the same value.
// GetAsyncKeyState only says "held down", which is not a lock state either. So
// the state is one flip per captured press, which makes "every press changes
// it" true by construction, and ResyncKeyState() re-seeds it from the live state
// on a desktop switch - the one thing that can swallow presses.
//
// Both the hook thread and the UI thread (Raw Input) write this, so mutation is
// serialised by AllowTip: it settles, per key, which one capture path owns a
// given press before the read-modify-write below happens.
bool g_toggleState[3];

// The elevated-window fallback, also hook thread only. A low-level keyboard
// hook isn't called for input the process isn't allowed to see, so a window
// running at a higher integrity level makes the mod go blind. Reading the key
// state instead ignores focus and integrity level alike, at the price of a
// timer - which is why it only runs while such a window is in front.
HWINEVENTHOOK g_foregroundHook;
HWINEVENTHOOK g_desktopHook;
DWORD g_selfRid;                   // own integrity level, read once at startup
bool g_pollRunning;
bool g_pollDown[kPollKeyCount];    // per key: held down at the last poll
// Mirrors g_pollRunning for the UI thread, which decides there whether a Raw
// Input key is worth reporting at all. See HandleRawInput.
volatile LONG g_hookBlind;
HWND g_levelHwnd;                  // window the verdict below was measured on
bool g_levelRaised;
bool g_levelKnown;
// The short-lived thread that re-reads the lock state after a desktop switch.
// Kept so a second switch during its 150 ms sleep doesn't start another one, and
// so the teardown can wait for its last write. Hook thread only.
HANDLE g_resyncThread;

// When each key last produced a tip, shared by every capture path so a press is
// reported once. The press time sits in the upper 62 bits and the path that
// claimed it in the low two, so one atomic word carries both and a 32-bit
// process can neither tear the pair nor read one without the other. See
// AllowTip.
ULONGLONG g_lastFired[kPollKeyCount];

// Raw Input key state, UI thread only. Key-down is what counts; the flags exist
// to drop auto-repeat and to expire a press whose release was never delivered.
bool g_rawDown[kPollKeyCount];
ULONGLONG g_rawDownAt[kPollKeyCount];

// Whether the Raw Input registration succeeded. The polling fallback has no way
// to tell numpad 0 from Insert - GetAsyncKeyState only knows virtual keys, and
// with Num Lock off numpad 0 reports VK_INSERT - so while this is true the poll
// leaves Insert to Raw Input entirely. Raw Input is not blocked by UIPI, so it
// covers exactly the elevated-foreground cases the poll exists for.
bool g_rawInputOk;

// The language watch. A language or IME-mode change has no notification of its
// own: it is a property of the thread that owns the input, and the only reliable
// trigger is "a key that switches it was just pressed". So a switch key starts a
// short watch that compares the input state of the focused thread a few times a
// second until it settles or times out.
//
// It lives on the UI thread. Finding the state means asking the foreground
// app's IME window a question, and that answer can take tens of milliseconds to
// arrive - which the hook thread must never wait for, because every keystroke in
// the session waits behind it. The hook only posts a message; the sampling runs
// on the thread that draws the card, where a slow answer costs a frame.
volatile LONG g_languageEnabled;  // mirrors the setting, read by the hook
bool g_inputWatching;             // UI thread only, here and below
int g_inputTicks;
bool g_inputHasLast;
InputState g_inputLast;

void StartInputWatch();
void InputTick();

// =============================================================================
// DirectWrite text
//
// GDI+'s AntiAliasGridFit snaps every glyph to the pixel grid, which is what
// makes the label look pixelated at 100% scaling - windhawk-mods issue #6977.
// The native Windows 11 OSD draws its labels with DirectWrite, and so does the
// card now: each glyph run is rasterised into a grayscale coverage texture and
// blended over the card's premultiplied pixels by hand. Doing the blend
// directly is what keeps the per-pixel alpha the layered window needs - a
// Direct2D render target bound to the DC would flatten it.
//
// NATURAL_SYMMETRIC is the WPF-style symmetric grayscale smoothing, which is
// what the standalone WinTips app (WPF) shows as well, so both editions draw
// the label the same way. Subpixel positioning stays on (pixel snapping off),
// which is what keeps the glyphs smooth while the card slides in. If dwrite.dll
// cannot be reached the GDI+ path remains, now without the grid fit.
// =============================================================================

IDWriteFactory* g_dwriteFactory = nullptr;
bool g_dwriteTried = false;

bool DWriteAvailable() {
    if (!g_dwriteTried) {
        g_dwriteTried = true;
        HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                         __uuidof(IDWriteFactory),
                                         (IUnknown**)&g_dwriteFactory);
        if (FAILED(hr)) {
            g_dwriteFactory = nullptr;
            Wh_Log(L"DWriteCreateFactory failed: 0x%08lX", hr);
        }
    }
    return g_dwriteFactory != nullptr;
}

// Text formats are cached: the size only changes when the card moves to a
// display with another DPI scale or the card size setting changes, and a font
// match on every frame would spend exactly the milliseconds the animation
// needs. Four slots hold the two sizes of the card plus a little churn.
struct CachedTextFormat {
    float size;
    int weight;
    IDWriteTextFormat* format;
};

CachedTextFormat g_textFormatCache[4] = {};

void DWriteShutdown() {
    for (CachedTextFormat& entry : g_textFormatCache) {
        if (entry.format) {
            entry.format->Release();
        }
        entry.size = 0.0f;
        entry.weight = 0;
        entry.format = nullptr;
    }
    if (g_dwriteFactory) {
        g_dwriteFactory->Release();
        g_dwriteFactory = nullptr;
    }
    g_dwriteTried = false;
}

IDWriteTextFormat* GetTextFormat(float size, int weight) {
    if (!DWriteAvailable()) {
        return nullptr;
    }
    for (CachedTextFormat& entry : g_textFormatCache) {
        if (entry.format && entry.size == size && entry.weight == weight) {
            return entry.format;
        }
    }

    IDWriteTextFormat* format = nullptr;
    HRESULT hr = g_dwriteFactory->CreateTextFormat(
        UiFaceName(), nullptr, (DWRITE_FONT_WEIGHT)weight,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"",
        &format);
    if (FAILED(hr)) {
        Wh_Log(L"CreateTextFormat(%s, %g) failed: 0x%08lX", UiFaceName(), size,
               hr);
        return nullptr;
    }
    format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    CachedTextFormat* slot = nullptr;
    for (CachedTextFormat& entry : g_textFormatCache) {
        if (!entry.format) {
            slot = &entry;
            break;
        }
    }
    if (!slot) {
        slot = &g_textFormatCache[0]; // rare overflow: evict the oldest
        if (slot->format) {
            slot->format->Release();
        }
    }
    slot->size = size;
    slot->weight = weight;
    slot->format = format;
    return format;
}

float MeasureTextWidthDWrite(const std::wstring& text, float size,
                             int weight) {
    if (text.empty()) {
        return 0.0f;
    }
    IDWriteTextFormat* format = GetTextFormat(size, weight);
    if (!format) {
        return 0.0f;
    }
    IDWriteTextLayout* layout = nullptr;
    if (FAILED(g_dwriteFactory->CreateTextLayout(text.c_str(),
                                                 (UINT32)text.size(), format,
                                                 10000.0f, 1000.0f, &layout))) {
        return 0.0f;
    }
    DWRITE_TEXT_METRICS metrics{};
    layout->GetMetrics(&metrics);
    layout->Release();
    return metrics.widthIncludingTrailingWhitespace;
}

// Blends DirectWrite's coverage texture straight into the card's premultiplied
// pixels. A process-lifetime object: the refcount is pinned at one and the
// draw parameters travel through the members rather than the client context.
class CardTextRenderer : public IDWriteTextRenderer {
public:
    BYTE* bits = nullptr;
    int width = 0;
    int height = 0;
    int stride = 0;
    int coverageScale = 256; // extra multiplier on the coverage (badge fade)
    BYTE premul[256][3];     // premultiplied text color per coverage level

    void Begin(BYTE* surfaceBits, int surfaceWidth, int surfaceHeight,
               int surfaceStride, const Color& color, float alphaScale) {
        bits = surfaceBits;
        width = surfaceWidth;
        height = surfaceHeight;
        stride = surfaceStride;
        coverageScale =
            (int)std::lround(Clamp01(alphaScale) * 256.0f);
        for (int i = 0; i < 256; i++) {
            premul[i][0] = (BYTE)((color.GetB() * i + 127) / 255);
            premul[i][1] = (BYTE)((color.GetG() * i + 127) / 255);
            premul[i][2] = (BYTE)((color.GetR() * i + 127) / 255);
        }
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) {
        if (!object) {
            return E_POINTER;
        }
        *object = nullptr;
        if (riid == __uuidof(IUnknown) ||
            riid == __uuidof(IDWritePixelSnapping) ||
            riid == __uuidof(IDWriteTextRenderer)) {
            *object = (IDWriteTextRenderer*)this;
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() {
        return 1;
    }

    ULONG STDMETHODCALLTYPE Release() {
        return 1;
    }

    // Subpixel positioning on: positions are not rounded to the pixel grid,
    // which is what keeps the text smooth while the card slides.
    HRESULT STDMETHODCALLTYPE IsPixelSnappingDisabled(void*, BOOL* isDisabled) {
        if (isDisabled) {
            *isDisabled = TRUE;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetCurrentTransform(void*, DWRITE_MATRIX* matrix) {
        if (matrix) {
            *matrix = DWRITE_MATRIX{1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
        }
        return S_OK;
    }

    // The em sizes handed to CreateTextFormat are pixels already, so one DIP
    // is one pixel here.
    HRESULT STDMETHODCALLTYPE GetPixelsPerDip(void*, FLOAT* pixelsPerDip) {
        if (pixelsPerDip) {
            *pixelsPerDip = 1.0f;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE
    DrawGlyphRun(void*, FLOAT originX, FLOAT originY,
                 DWRITE_MEASURING_MODE measuringMode,
                 const DWRITE_GLYPH_RUN* glyphRun,
                 const DWRITE_GLYPH_RUN_DESCRIPTION*, IUnknown*) {
        return BlendGlyphRun(originX, originY, measuringMode, glyphRun);
    }

    HRESULT STDMETHODCALLTYPE DrawUnderline(void*, FLOAT, FLOAT,
                                            const DWRITE_UNDERLINE*, IUnknown*) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DrawStrikethrough(void*, FLOAT, FLOAT,
                                                const DWRITE_STRIKETHROUGH*,
                                                IUnknown*) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DrawInlineObject(void*, FLOAT, FLOAT,
                                               IDWriteInlineObject*, BOOL, BOOL,
                                               IUnknown*) {
        return S_OK;
    }

private:
    HRESULT BlendGlyphRun(FLOAT originX, FLOAT originY,
                          DWRITE_MEASURING_MODE measuringMode,
                          const DWRITE_GLYPH_RUN* glyphRun) {
        if (!bits || !g_dwriteFactory || !glyphRun ||
            glyphRun->glyphCount == 0) {
            return S_OK;
        }

        IDWriteGlyphRunAnalysis* analysis = nullptr;
        if (FAILED(g_dwriteFactory->CreateGlyphRunAnalysis(
                glyphRun, 1.0f, nullptr, DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC,
                measuringMode, originX, originY, &analysis))) {
            // A missing label for one frame beats a failed card for the tip.
            return S_OK;
        }

        RECT bounds{};
        if (SUCCEEDED(analysis->GetAlphaTextureBounds(
                DWRITE_TEXTURE_CLEARTYPE_3x1, &bounds)) &&
            bounds.right > bounds.left && bounds.bottom > bounds.top) {
            const int left = std::max((int)bounds.left, 0);
            const int top = std::max((int)bounds.top, 0);
            const int right = std::min((int)bounds.right, width);
            const int bottom = std::min((int)bounds.bottom, height);
            if (right > left && bottom > top) {
                // The CLEARTYPE_3x1 texture is three bytes per pixel - one
                // coverage value per subpixel column - whatever the rendering
                // mode. Averaging the three columns turns it into the
                // grayscale coverage the card's surface needs.
                const int textureWidth = bounds.right - bounds.left;
                const int textureHeight = bounds.bottom - bounds.top;
                std::vector<BYTE> texture((size_t)textureWidth * 3 *
                                          textureHeight);
                HRESULT texHr = analysis->CreateAlphaTexture(
                        DWRITE_TEXTURE_CLEARTYPE_3x1, &bounds, texture.data(),
                        (UINT32)texture.size());
                if (SUCCEEDED(texHr)) {
                    for (int y = top; y < bottom; y++) {
                        const BYTE* source =
                            texture.data() +
                            (size_t)(y - bounds.top) * textureWidth * 3;
                        BYTE* row = bits + (size_t)y * stride;
                        for (int x = left; x < right; x++) {
                            const BYTE* tex =
                                source + (size_t)(x - bounds.left) * 3;
                            const unsigned gray =
                                ((unsigned)tex[0] + tex[1] + tex[2] + 1) / 3;
                            const unsigned coverage =
                                (gray * coverageScale + 128) >> 8;
                            if (!coverage) {
                                continue;
                            }
                            const unsigned inverse = 255 - coverage;
                            BYTE* pixel = row + (size_t)x * 4;
                            // Premultiplied "over": out = src + dst*(1-srcA).
                            pixel[0] = (BYTE)(premul[coverage][0] +
                                              (pixel[0] * inverse + 127) / 255);
                            pixel[1] = (BYTE)(premul[coverage][1] +
                                              (pixel[1] * inverse + 127) / 255);
                            pixel[2] = (BYTE)(premul[coverage][2] +
                                              (pixel[2] * inverse + 127) / 255);
                            pixel[3] = (BYTE)(coverage +
                                              (pixel[3] * inverse + 127) / 255);
                        }
                    }
                }
            }
        }

        analysis->Release();
        return S_OK;
    }
};

CardTextRenderer g_textRenderer;

// Draws one line of text into the card surface. `x`/`centerY` name the point
// the line is centred on - the same point the GDI+ path centred its rect
// around - unless `centerAlign` is false, when `x` is the left edge instead.
// Returns the measured line width, or 0 when DirectWrite is unavailable and
// the caller must draw with GDI+.
float DrawTextDWrite(const std::wstring& text, float size, int weight,
                     const Color& color, float alphaScale, float x,
                     float centerY, bool centerAlign) {
    if (text.empty() || !DWriteAvailable() || !g_surface.bits) {
        return 0.0f;
    }
    IDWriteTextFormat* format = GetTextFormat(size, weight);
    if (!format) {
        return 0.0f;
    }
    IDWriteTextLayout* layout = nullptr;
    if (FAILED(g_dwriteFactory->CreateTextLayout(text.c_str(),
                                                 (UINT32)text.size(), format,
                                                 10000.0f, 1000.0f, &layout))) {
        return 0.0f;
    }

    // One line, always: the labels are single strings and wrapping is off.
    DWRITE_TEXT_METRICS metrics{};
    layout->GetMetrics(&metrics);
    UINT32 lineCount = 0;
    layout->GetLineMetrics(nullptr, 0, &lineCount);
    float lineHeight = size * 1.3f;
    if (lineCount > 0) {
        DWRITE_LINE_METRICS line{};
        UINT32 fetched = 1;
        if (SUCCEEDED(layout->GetLineMetrics(&line, 1, &fetched))) {
            lineHeight = (float)line.height;
        }
    }

    const float lineWidth = metrics.widthIncludingTrailingWhitespace;
    const float originX = centerAlign ? x - lineWidth / 2.0f : x;
    const float originY = centerY - lineHeight / 2.0f;

    g_textRenderer.Begin((BYTE*)g_surface.bits, g_surface.width,
                         g_surface.height, g_surface.stride, color, alphaScale);
    layout->Draw(nullptr, &g_textRenderer, originX, originY);
    layout->Release();
    return lineWidth;
}


// =============================================================================
// Live backdrop
//
// The snapshot above is taken once and then only re-aligned, so it stays a
// photograph of the moment the card appeared. Grabbing again while the card is
// up needs one thing from the system: a way to keep the card out of its own
// copy. SetWindowDisplayAffinity(WDA_EXCLUDEFROMCAPTURE) does exactly that, but
// only Windows 10 2004 and later honour it, and it leans on DWM composition -
// no composition, or a remote desktop session, and the exclusion can't be
// trusted. When it is available a worker thread grabs, blurs and blends the
// desktop behind the card a few dozen times a second; when it isn't, the
// one-shot path stays as it was.
// =============================================================================

constexpr DWORD kWdaNone = 0x00000000;
constexpr DWORD kWdaExcludeFromCapture = 0x00000011;

// Resolved at runtime rather than linked: the mod is built for Windows 10 as a
// whole, and neither the header declaration nor the import library is
// guaranteed to be there in every toolchain.
typedef BOOL(WINAPI* SetWindowDisplayAffinity_t)(HWND, DWORD);
typedef UINT(WINAPI* TimePeriod_t)(UINT);

SetWindowDisplayAffinity_t DisplayAffinityFn() {
    static SetWindowDisplayAffinity_t fn = [] {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        return user32 ? (SetWindowDisplayAffinity_t)GetProcAddress(
                            user32, "SetWindowDisplayAffinity")
                      : (SetWindowDisplayAffinity_t) nullptr;
    }();
    return fn;
}

TimePeriod_t TimePeriodFn(const char* name) {
    static HMODULE winmm;
    static bool tried;
    if (!tried) {
        tried = true;
        // SYSTEM32 only: winmm.dll is not a KnownDLL, so a bare name would let
        // the default search order pick up a copy from next to windhawk.exe.
        winmm = LoadLibraryExW(L"winmm.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    return winmm ? (TimePeriod_t)GetProcAddress(winmm, name) : nullptr;
}

// RtlGetVersion answers honestly where GetVersionEx lies to a process without a
// compatibility manifest, which is exactly the situation a mod runs in.
struct OsVersionInfo {
    ULONG size;
    ULONG major;
    ULONG minor;
    ULONG build;
    ULONG platform;
    WCHAR csd[128];
};

DWORD WindowsBuild() {
    static DWORD build = 0;
    if (!build) {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        if (ntdll) {
            auto rtlGetVersion =
                (LONG(WINAPI*)(OsVersionInfo*))GetProcAddress(ntdll,
                                                              "RtlGetVersion");
            if (rtlGetVersion) {
                OsVersionInfo info{};
                info.size = sizeof(info);
                if (rtlGetVersion(&info) == 0) {
                    build = info.build;
                }
            }
        }
        if (!build) {
            build = 1;
        }
    }
    return build;
}

bool LiveBackdropSupported() {
    static int cached = -1;
    if (cached < 0) {
        BOOL composing = FALSE;
        bool ok = WindowsBuild() >= 19041 && DisplayAffinityFn() != nullptr &&
                  GetSystemMetrics(SM_REMOTESESSION) == 0 &&
                  SUCCEEDED(DwmIsCompositionEnabled(&composing)) && composing;
        if (!ok) {
            Wh_Log(L"live backdrop unavailable (build %lu)", WindowsBuild());
        }
        cached = ok ? 1 : 0;
    }
    return cached == 1;
}

bool g_liveRunning;      // UI thread only: the worker below is up
bool g_livePreciseTimer; // timeBeginPeriod(1) is held by us
HANDLE g_liveThread;
HANDLE g_liveWake;
volatile LONG g_liveGeneration; // bumped to tell the worker to stop
int g_livePad;
int g_liveRadius;

// The hand-off, one frame at a time. The worker owns the pixels it captures;
// the UI thread takes a copy under the lock and draws from its own surface, so
// neither side ever reads a buffer the other is writing.
CRITICAL_SECTION g_liveLock;
bool g_liveLockReady;
volatile LONG g_livePending; // 1 = a frame is waiting for the UI thread
std::vector<BYTE> g_liveFrame;
int g_liveFrameX;
int g_liveFrameY;
int g_liveFrameW;
int g_liveFrameH;

void StepAnimation(); // defined below, with the rest of the animation code

// Worker side: hand the frame over if the UI thread has taken the last one.
// Queueing instead would only ever deliver an older picture, and a burst of
// stale frames looks worse than a dropped one.
void PublishLiveFrame(const Surface& image, int x, int y) {
    if (!image.bits || image.width <= 0 || image.height <= 0) {
        return;
    }
    EnterCriticalSection(&g_liveLock);
    if (InterlockedCompareExchange(&g_livePending, 1, 0) == 0) {
        const size_t bytes = (size_t)image.width * image.height * 4;
        if (g_liveFrame.size() < bytes) {
            g_liveFrame.resize(bytes);
        }
        memcpy(g_liveFrame.data(), image.bits, bytes);
        g_liveFrameX = x;
        g_liveFrameY = y;
        g_liveFrameW = image.width;
        g_liveFrameH = image.height;
    }
    LeaveCriticalSection(&g_liveLock);
}

// UI side: move the waiting frame into the backdrop. Returns false when there
// is nothing new, so a tick with no frame costs an atomic exchange and nothing
// else.
bool TakeLiveFrame() {
    if (!g_liveLockReady) {
        return false;
    }
    bool taken = false;
    EnterCriticalSection(&g_liveLock);
    if (InterlockedExchange(&g_livePending, 0) == 1) {
        const size_t bytes = (size_t)g_liveFrameW * g_liveFrameH * 4;
        if (g_liveFrameW > 0 && g_liveFrameH > 0 &&
            g_liveFrame.size() >= bytes &&
            g_backdrop.image.Ensure(g_liveFrameW, g_liveFrameH)) {
            memcpy(g_backdrop.image.bits, g_liveFrame.data(), bytes);
            g_backdrop.x = g_liveFrameX;
            g_backdrop.y = g_liveFrameY;
            g_backdrop.valid = true;
            taken = true;
        }
    }
    LeaveCriticalSection(&g_liveLock);
    return taken;
}

DWORD WINAPI LiveCaptureThread(LPVOID param) {
    const LONG generation = (LONG)(LONG_PTR)param;
    const int pad = g_livePad;
    const int radius = g_liveRadius;

    Surface work;
    std::vector<BYTE> previous; // the frame last handed over, for the blend
    int previousX = 0;
    int previousY = 0;
    int previousW = 0;
    int previousH = 0;
    bool hasPrevious = false;
    int misses = 0;
    ULONGLONG next = GetTickCount64();

    while (generation == g_liveGeneration) {
        HWND hwnd = g_hwnd;
        RECT box{};
        int cardW = 0;
        int cardH = 0;
        if (hwnd && GetWindowRect(hwnd, &box)) {
            cardW = box.right - box.left;
            cardH = box.bottom - box.top;
        }

        if (cardW <= 0 || cardH <= 0) {
            if (++misses >= 10) {
                break;
            }
        } else {
            const int grabW = cardW + pad * 2;
            const int grabH = cardH + pad * 2;
            const int grabX = box.left - pad;
            const int grabY = box.top - pad;

            bool grabbed = false;
            if (work.Ensure(grabW, grabH)) {
                HDC screen = GetDC(nullptr);
                if (screen) {
                    grabbed = BitBlt(work.dc, 0, 0, grabW, grabH, screen, grabX,
                                     grabY, SRCCOPY | CAPTUREBLT) != FALSE;
                    ReleaseDC(nullptr, screen);
                }
            }
            // A black copy means an elevated window is in front, or the desktop
            // is in the middle of a switch. Keep what the card already shows
            // instead of painting a black rectangle into it.
            if (!grabbed || SurfaceIsBlank(work)) {
                if (++misses >= 10) {
                    break;
                }
            } else {
                misses = 0;
                BlurBackdrop(work, radius, kLiveBlurPasses);

                const int bytes = grabW * grabH * 4;
                const int movedX = grabX - previousX;
                const int movedY = grabY - previousY;
                const bool moved = movedX > 2 || movedX < -2 || movedY > 2 ||
                                   movedY < -2;
                // While the card itself is moving the background is a pan, and
                // blending two positions smears the edges into a ghost. Only
                // blend once the card has settled where it belongs.
                if (hasPrevious && previousW == grabW && previousH == grabH &&
                    !moved && previous.size() >= (size_t)bytes) {
                    BlendPixels(previous.data(), (BYTE*)work.bits, bytes,
                                kLiveBlendWeight);
                }

                PublishLiveFrame(work, grabX, grabY);

                previous.resize((size_t)bytes);
                memcpy(previous.data(), work.bits, (size_t)bytes);
                previousX = grabX;
                previousY = grabY;
                previousW = grabW;
                previousH = grabH;
                hasPrevious = true;
            }
        }

        // Pace off the previous beat rather than "now plus the interval": a
        // single slow frame then doesn't push everything else back.
        next += kLiveRefreshMs;
        ULONGLONG now = GetTickCount64();
        if (now <= next) {
            WaitForSingleObject(g_liveWake, (DWORD)(next - now));
        } else {
            next = now;
        }
    }
    return 0;
}

void StopLiveCapture();

void StartLiveCapture() {
    if (g_liveRunning || !g_hwnd) {
        return;
    }
    if (g_settings.material == kMaterialSolid ||
        !LiveBackdropSupported()) {
        return;
    }
    SetWindowDisplayAffinity_t setAffinity = DisplayAffinityFn();
    if (!setAffinity ||
        !setAffinity(g_hwnd, kWdaExcludeFromCapture)) {
        return;
    }

    if (!g_liveLockReady) {
        InitializeCriticalSection(&g_liveLock);
        g_liveLockReady = true;
    }
    InterlockedExchange(&g_livePending, 0);

    g_livePad = (int)std::lround((float)kLivePaddingDip *
                                 std::max(1.0f, g_scale));
    // Two blur passes need a wider radius to match the three-pass snapshot;
    // 16 * 1.25 = 20, the same split the WinTips app makes.
    g_liveRadius = (int)std::lround((float)g_settings.blurRadius *
                                    kLiveRadiusScale * std::max(1.0f, g_scale));

    g_liveWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_liveWake) {
        setAffinity(g_hwnd, kWdaNone);
        return;
    }

    g_liveRunning = true;
    const LONG generation = InterlockedIncrement(&g_liveGeneration);
    g_liveThread = CreateThread(nullptr, 0, LiveCaptureThread,
                                (LPVOID)(LONG_PTR)generation, 0, nullptr);
    if (!g_liveThread) {
        g_liveRunning = false;
        CloseHandle(g_liveWake);
        g_liveWake = nullptr;
        setAffinity(g_hwnd, kWdaNone);
        return;
    }
    SetThreadPriority(g_liveThread, THREAD_PRIORITY_BELOW_NORMAL);

    // The loop waits 22 ms at a time, and the default 15.6 ms timer granularity
    // would round that up to 38 ms - the capture rate would drop to about 26 Hz
    // and the blur would visibly beat against the animation. Raised only while
    // the card is up; a tray app must not hold the system out of idle.
    TimePeriod_t beginPeriod = TimePeriodFn("timeBeginPeriod");
    if (beginPeriod && beginPeriod(1) == 0) {
        g_livePreciseTimer = true;
    }

    SetTimer(g_hwnd, kTimerLiveBackdrop, kLiveRefreshMs, nullptr);
}

void StopLiveCapture() {
    if (!g_liveRunning) {
        return;
    }
    g_liveRunning = false;
    InterlockedIncrement(&g_liveGeneration);

    if (g_liveWake) {
        SetEvent(g_liveWake);
    }
    if (g_liveThread) {
        // Unbounded: the worker only ever waits on its own event or on a grab,
        // never on this thread, so it always gets back here to notice the
        // generation bump. A timeout that just closes the handle would leave a
        // second worker running beside the next one.
        WaitForSingleObject(g_liveThread, INFINITE);
        CloseHandle(g_liveThread);
        g_liveThread = nullptr;
    }
    if (g_liveWake) {
        CloseHandle(g_liveWake);
        g_liveWake = nullptr;
    }

    if (g_livePreciseTimer) {
        g_livePreciseTimer = false;
        TimePeriod_t endPeriod = TimePeriodFn("timeEndPeriod");
        if (endPeriod) {
            endPeriod(1);
        }
    }

    if (g_hwnd) {
        KillTimer(g_hwnd, kTimerLiveBackdrop);
        SetWindowDisplayAffinity_t setAffinity = DisplayAffinityFn();
        if (setAffinity) {
            setAffinity(g_hwnd, kWdaNone);
        }
    }

    EnterCriticalSection(&g_liveLock);
    InterlockedExchange(&g_livePending, 0);
    LeaveCriticalSection(&g_liveLock);
}

// New frame on screen: re-align the backdrop to where the card is now and
// repaint. Reusing the animation step keeps the fade and the slide consistent
// instead of presenting the card at full opacity behind its own animation.
void OnLiveBackdropTick() {
    if (g_phase == kPhaseHidden) {
        StopLiveCapture();
        return;
    }
    if (g_settings.material == kMaterialSolid) {
        StopLiveCapture();
        g_backdrop.Invalidate();
        return;
    }
    // The worker gives up after ten grabs in a row that produced nothing (the
    // desktop is locked, the display is mid-switch). Tear the timer down with
    // it instead of ticking over a thread that will never deliver again: the
    // card keeps the last frame, and the next tip starts a fresh worker.
    if (g_liveThread && WaitForSingleObject(g_liveThread, 0) == WAIT_OBJECT_0) {
        StopLiveCapture();
        return;
    }
    if (!TakeLiveFrame()) {
        return;
    }
    StepAnimation();
}

// =============================================================================
// Keyboard helpers
// =============================================================================

int KindToVk(TipKind kind) {
    switch (kind) {
        case kCapsLock:
            return VK_CAPITAL;
        case kNumLock:
            return VK_NUMLOCK;
        case kScrollLock:
            return VK_SCROLL;
        case kInsert:
            return VK_INSERT;
        case kCopy:
            return 'C';
        case kPaste:
            return 'V';
        default:
            return 'X';
    }
}

int KindFromVk(DWORD vk) {
    switch (vk) {
        case VK_CAPITAL:
            return kCapsLock;
        case VK_NUMLOCK:
            return kNumLock;
        case VK_SCROLL:
            return kScrollLock;
        case VK_INSERT:
            return kInsert;
        default:
            return -1;
    }
}

int ClipKindFromVk(DWORD vk) {
    switch (vk) {
        case 'C':
            return kCopy;
        case 'V':
            return kPaste;
        case 'X':
            return kCut;
        default:
            return -1;
    }
}

// Windows has always shipped a second set of clipboard shortcuts: Ctrl+Insert
// copies, Shift+Insert pastes and Shift+Delete cuts. Terminal windows lean on
// them, and two of the three land on keys this mod reports by itself, so the
// modifiers are what tells a plain Insert apart from a copy or a paste.
int LegacyClipKindFromVk(DWORD vk, bool ctrl, bool shift) {
    switch (vk) {
        case VK_INSERT:
            if (ctrl) {
                return kCopy;
            }
            return shift ? kPaste : -1;
        case VK_DELETE:
            // Ctrl+Shift+Delete is "clear browsing data" in browsers and "delete
            // line" in editors, never a cut.
            return shift && !ctrl ? kCut : -1;
        default:
            return -1;
    }
}

// Modifier state comes from GetAsyncKeyState, not from the hook's own table.
// The table only learns about a key-up the hook is called for, and the hook is
// not called for anything that happens while the secure desktop is up: after
// Ctrl+Alt+Del, Win+L or a UAC prompt, Ctrl and Win would stay "held" until they
// were pressed and released again in an ordinary window - which turns every
// later V into a paste. GetAsyncKeyState has already accounted for every event
// that came before the one being handled, and reads "up" once the key was
// released on another desktop, so it answers this question and the table can't.
//
// Left and right are asked separately: the generic codes are resolved by the
// keyboard layout, the specific ones are not.
bool KeyDownNow(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

bool CtrlDown() {
    return KeyDownNow(VK_LCONTROL) || KeyDownNow(VK_RCONTROL);
}

// Ctrl as a *clipboard* modifier, which is a narrower question than CtrlDown().
//
// AltGr arrives as Left Ctrl + Right Alt, so CtrlDown() is true for as long as
// AltGr is held. On Czech, Slovak, Hungarian, Polish and Croatian layouts
// AltGr+V types "@": claiming a paste on every e-mail address typed is worse
// than missing a deliberate Ctrl+RightAlt+V, and AltGr+C / AltGr+X would arm
// the copy and cut intents the same way.
//
// Asked of Right Alt specifically, not of "some Alt is down": Ctrl+LeftAlt+C is
// not AltGr on any layout, and ignoring it would lose a real copy.
bool ClipCtrlDown() {
    return CtrlDown() && !KeyDownNow(VK_RMENU);
}

bool ShiftDown() {
    return KeyDownNow(VK_LSHIFT) || KeyDownNow(VK_RSHIFT);
}

bool AltDown() {
    return KeyDownNow(VK_LMENU) || KeyDownNow(VK_RMENU);
}

bool WinDown() {
    return KeyDownNow(VK_LWIN) || KeyDownNow(VK_RWIN);
}

// Keys that may have just changed the input language or the IME mode: their own
// modifiers (Shift alone toggles Chinese/English inside an IME, Win+Space walks
// the language list, Ctrl/Alt+Shift is the older shortcut for it) and Space.
// Modifiers count on their own because they are pressed before the letter they
// modify, which is exactly when the watch has to be running.
bool SwitchCandidate(DWORD vk) {
    if (vk == VK_SPACE) {
        return CtrlDown() || AltDown() || WinDown();
    }
    switch (vk) {
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
        case VK_LWIN:
        case VK_RWIN:
            return true;
        default:
            return false;
    }
}

// A key press resolved to the tip it should raise, or -1 for "not ours".
// `extended` is the hook's LLKHF_EXTENDED bit. With Num Lock off, numpad 0
// reports VK_INSERT too, and the E0 prefix is the only thing marking the real
// Insert key: without this check, every 0 on the numpad would raise an Insert
// card - or a copied/pasted one under Ctrl/Shift.
int TipKindForVk(DWORD vk, bool extended, bool ctrl, bool shift) {
    if (vk == VK_INSERT && !extended) {
        return -1;
    }
    int kind = LegacyClipKindFromVk(vk, ctrl, shift);
    if (kind >= 0) {
        return kind;
    }
    kind = KindFromVk(vk);
    if (kind >= 0) {
        return kind;
    }
    return ctrl ? ClipKindFromVk(vk) : -1;
}

// Which capture path is reporting. Two paths can see the same press; one path
// seeing two presses in a row is a different thing and must not be dropped.
enum TipSource {
    kSourceHook = 0,
    kSourcePoll = 1,
    kSourceRaw = 2,
};

// Claims a press for the calling capture path. False means some other path has
// already reported this very press, in which case the caller must neither flip
// the tracked state nor raise a card.
//
// `at` is when the key went down, not when this path got round to it. Raw Input
// is handled on the UI thread, which can be busy capturing a backdrop for a
// whole frame: its report then arrives well after the hook's, and comparing
// processing times would make one keystroke look like two. Comparing press
// times is what makes them the same press.
//
// `source` is what the time window alone cannot tell apart: the same press seen
// by a second path (drop it - flipping twice would leave every later card
// showing the opposite of the key) and a second, genuine press from the path
// that already reported one (keep it - the system toggled twice, and dropping it
// would leave the state one toggle behind for the rest of the session). Nobody
// holds a lock key down expecting a card every 200 ms; two real taps that fast
// are rare but not impossible, and getting them wrong is permanent.
bool AllowTip(TipKind kind, ULONGLONG at, int source) {
    if ((unsigned)kind >= (unsigned)kPollKeyCount) {
        return true;  // nothing shared to protect: copy/paste/cut/language
    }

    const ULONGLONG stamp = (at << 2) | (ULONGLONG)source;
    for (;;) {
        // Time and path in one word, read in one go: this process is 32-bit, so
        // a plain 64-bit read could tear and report a wild press time.
        const ULONGLONG current = (ULONGLONG)InterlockedCompareExchange64(
            (volatile LONG64*)&g_lastFired[kind], 0, 0);
        // Signed, and both sides of zero: the two paths measure the same press a
        // few ms apart, and either one can be the earlier of the two.
        const LONGLONG delta = (LONGLONG)(at - (current >> 2));
        if (delta > -(LONGLONG)kSameKindWindowMs &&
            delta < (LONGLONG)kSameKindWindowMs) {
            return (int)(current & 3) == source;
        }
        // The exchange decides the race: exactly one caller sees the stamp it
        // read and goes on; anyone else re-reads the winner's and backs off.
        if (InterlockedCompareExchange64((volatile LONG64*)&g_lastFired[kind],
                                         (LONGLONG)stamp,
                                         (LONGLONG)current) ==
            (LONGLONG)current) {
            return true;
        }
    }
}

// When the message being handled was posted, in the 64-bit tick domain.
// GetMessageTime is the same millisecond counter as GetTickCount, only 32 bits
// wide; putting it back together with the current tick recovers the elapsed
// time even across the 49-day wrap.
ULONGLONG MessagePostTime() {
    DWORD posted = (DWORD)GetMessageTime();
    ULONGLONG now = GetTickCount64();
    ULONGLONG at = (now & ~0xFFFFFFFFull) | posted;
    if (at > now) {
        at -= 0x100000000ull;  // posted just before the counter wrapped
    }
    return at;
}

// =============================================================================
// Environment checks
// =============================================================================

// The standalone WinTips.exe holds this mutex while it runs. When both are
// installed we stay quiet instead of drawing a second overlay on top of it.
bool OtherInstanceRunning() {
    HANDLE mutex = OpenMutexW(SYNCHRONIZE, FALSE, L"Local\\WinTips.SingleInstance");
    if (!mutex) {
        return false;
    }
    CloseHandle(mutex);
    return true;
}

bool FullscreenActive() {
    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
        if (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN ||
            state == QUNS_PRESENTATION_MODE) {
            return true;
        }
    }

    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return false;
    }
    RECT rect;
    if (!GetWindowRect(foreground, &rect)) {
        return false;
    }
    HMONITOR monitor = MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return false;
    }
    if (!EqualRect(&rect, &info.rcMonitor)) {
        return false;
    }
    wchar_t className[64]{};
    GetClassNameW(foreground, className, 64);
    return wcscmp(className, L"Progman") != 0 &&
           wcscmp(className, L"WorkerW") != 0;
}

bool ClipboardHasContent() {
    static const UINT formats[] = {CF_UNICODETEXT, CF_HDROP,     CF_BITMAP,
                                   CF_DIB,        CF_ENHMETAFILE, CF_METAFILEPICT};
    for (UINT format : formats) {
        if (IsClipboardFormatAvailable(format)) {
            return true;
        }
    }
    return false;
}

// =============================================================================
// Presentation
// =============================================================================

// Two scales: the display's DPI decides how many physical pixels a DIP is, the
// card size setting scales the card itself. Everything inside the card uses
// their product; margins, the slide distance and the blur radius stay on the
// DPI scale, so a bigger card still keeps the same 16 DIP gap to the edge.
void UpdateScales() {
    g_scale = g_monitor.valid ? g_monitor.scale : 1.0f;
    g_cardScale = g_scale * g_settings.TipScale();
}

void UpdateGeometry() {
    // The active window may be on another display since the last tip, so the
    // target monitor (and with it the DPI scale) is re-resolved every time.
    if (!ResolveTargetMonitor()) {
        LoadMonitor(PrimaryMonitor(), g_monitor);
    }
    UpdateScales();
    MeasureCard(g_cardScale, g_frame, g_cardWidth, g_cardHeight);
    g_placement =
        ComputePlacement((float)g_cardWidth, (float)g_cardHeight, g_scale);
    // A fresh layout starts at the target; the follow ease only takes over when
    // the taskbar moves under a card that is already on screen.
    g_posX = g_placement.x;
    g_posY = g_placement.y;
    g_following = false;
    g_backdropPending = false;
    g_surface.Ensure(g_cardWidth, g_cardHeight);
}

// The card has to land on a display. Its place is derived from the foreground
// window, that window's monitor, the work area, the taskbar's live edge and the
// DPI - any of which can come back nonsensical during a fullscreen switch, with
// the taskbar covered, or across a monitor hot-plug. A card that lands nowhere
// is indistinguishable from "no tip at all", which is the hardest failure to
// diagnose, so the resting position is checked once per show and pulled back to
// the centre of the work area if it failed. Once, not followed: this must not
// turn into a source of jitter.
void EnsureOnScreen() {
    if (g_cardWidth <= 0 || g_cardHeight <= 0) {
        return;
    }

    POINT centre{g_posX + g_cardWidth / 2, g_posY + g_cardHeight / 2};
    if (MonitorFromPoint(centre, MONITOR_DEFAULTTONULL)) {
        return;
    }

    // The target itself may be the monitor that went away; the primary one is
    // then the only safe place left.
    MonitorTarget target = g_monitor;
    if (!target.valid) {
        LoadMonitor(PrimaryMonitor(), target);
    }
    if (!target.valid) {
        return;
    }

    int usableW = (int)std::max<LONG>(1, target.work.right - target.work.left);
    int usableH = (int)std::max<LONG>(1, target.work.bottom - target.work.top);
    g_posX = target.work.left + (usableW - g_cardWidth) / 2;
    g_posY = target.work.top + (usableH - g_cardHeight) / 2;
    // The slide and the follow loop ease from the placement, so a jump has to
    // move it too or the next frame would drag the card straight back out.
    g_placement.x = g_posX;
    g_placement.y = g_posY;

    Wh_Log(L"card landed off-screen, recentred to %d,%d", g_posX, g_posY);
}

// Grabs the pixels behind the card and blurs them. The window has to be hidden
// while doing it, otherwise the copy would contain the previous card.
void RefreshBackdrop() {
    if (!g_hwnd || g_settings.material == kMaterialSolid) {
        g_backdrop.Invalidate();
        return;
    }
    // Still inside the margin of the last grab: the drawing code re-samples it
    // at the card's current place, so there is nothing to do.
    if (g_backdrop.Covers(g_posX, g_posY, g_cardWidth, g_cardHeight)) {
        return;
    }
    // The live path re-aligns on its own within one refresh interval, and the
    // hide/capture/show dance below would flicker a card that is already up.
    if (g_liveRunning) {
        return;
    }

    bool wasVisible = IsWindowVisible(g_hwnd) != FALSE;
    if (wasVisible) {
        ShowWindow(g_hwnd, SW_HIDE);
        // Hiding only affects the frame the compositor builds next, while the
        // copy below reads the frame it has already built. Without a wait the
        // previous card gets baked into its own snapshot, and that card-sized
        // flat rectangle is exactly what the new card then draws on. One
        // composed frame is enough; a failed flush (composition off) just
        // means there was nothing to wait for.
        DwmFlush();
    }

    int radius = (int)std::lround((float)g_settings.blurRadius * g_scale);
    int pad = (int)std::lround((float)kStaticPaddingDip * std::max(1.0f, g_scale));
    HDC screen = GetDC(nullptr);
    CaptureBackdrop(screen, g_posX, g_posY, g_cardWidth, g_cardHeight, radius,
                    pad);
    ReleaseDC(nullptr, screen);

    if (wasVisible) {
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    }
}

void Present(float dx, float dy, BYTE alpha) {
    if (!g_surface.dc || !g_hwnd) {
        return;
    }

    // Where the card lands this frame: the backdrop is sampled at this offset,
    // so a card sliding in still shows the desktop it is sliding over rather
    // than dragging its snapshot along with it.
    g_presentX = g_posX + (int)std::lround(dx);
    g_presentY = g_posY + (int)std::lround(dy);

    g_surface.Clear();
    {
        Graphics g(g_surface.dc);
        RenderCard(g, g_cardScale, (float)g_cardWidth, (float)g_cardHeight,
                   g_frame, g_theme,
                   CardHangsTop(g_monitor, g_posX, g_posY, (float)g_cardWidth,
                                (float)g_cardHeight));
    }

    HDC screen = GetDC(nullptr);
    POINT src{0, 0};
    SIZE size{g_cardWidth, g_cardHeight};
    POINT dst{g_posX + (int)std::lround(dx), g_posY + (int)std::lround(dy)};

    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = alpha;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(g_hwnd, screen, &dst, &size, g_surface.dc, &src, 0,
                        &blend, ULW_ALPHA);
    ReleaseDC(nullptr, screen);
}

void HideNow() {
    // Both of these watch the card rather than paint it, so they go first: the
    // capture worker must be joined before the window goes away, and the guard
    // has nothing left to defend once the card is hidden.
    StopLiveCapture();
    if (g_hwnd) {
        ShowWindow(g_hwnd, SW_HIDE);
        KillTimer(g_hwnd, kTimerAnim);
        KillTimer(g_hwnd, kTimerFollow);
        KillTimer(g_hwnd, kTimerTopmost);
    }
    g_phase = kPhaseHidden;
    g_following = false;
    g_backdropPending = false;
    g_count = 0;
    g_badgePop = false;
    // The desktop may have changed while the card was up; capture again next
    // time instead of replaying a stale snapshot.
    g_backdrop.Invalidate();
}

void StepAnimation() {
    if (!g_hwnd) {
        return;
    }

    ULONGLONG now = GetTickCount64();
    float alpha = 1.0f;
    float dx = 0.0f;
    float dy = 0.0f;

    // The taskbar moved under the card: slide to the place it left for it.
    if (g_following) {
        float t = g_followMs > 0 ? (float)(now - g_followStart) / g_followMs
                                 : 1.0f;
        float e = EaseOutCubic(t);
        g_posX = (int)std::lround(Lerp(g_followFromX, (float)g_placement.x, e));
        g_posY = (int)std::lround(Lerp(g_followFromY, (float)g_placement.y, e));
        if (t >= 1.0f) {
            g_following = false;
            g_posX = g_placement.x;
            g_posY = g_placement.y;
        }
    }

    if (g_phase == kPhaseIn) {
        float t = g_animMs > 0 ? (float)(now - g_phaseStart) / g_animMs : 1.0f;
        float e = EaseOutCubic(t);
        alpha = e;
        dx = g_placement.dx * (1.0f - e);
        dy = g_placement.dy * (1.0f - e);
        if (t >= 1.0f) {
            g_phase = kPhaseHold;
        }
    } else if (g_phase == kPhaseOut) {
        float t = g_animMs > 0 ? (float)(now - g_phaseStart) / g_animMs : 1.0f;
        float e = EaseInCubic(t);
        alpha = 1.0f - e;
        dx = g_placement.dx * e;
        dy = g_placement.dy * e;
        if (t >= 1.0f) {
            HideNow();
            return;
        }
    }

    bool busy = g_phase == kPhaseIn || g_phase == kPhaseOut;

    if (g_statusMs > 0) {
        float t = (float)(now - g_statusStart) / g_statusMs;
        g_frame.statusT = Clamp01(t);
        busy = busy || t < 1.0f;
    } else {
        g_frame.statusT = 1.0f;
    }

    if (IsClipKind(g_frame.kind)) {
        if (g_clipMs > 0) {
            float t = (float)(now - g_clipStart) / g_clipMs;
            g_frame.clipT = Clamp01(t);
            busy = busy || t < 1.0f;
        } else {
            g_frame.clipT = 1.0f;
        }

        if (g_badgePop && g_animMs > 0) {
            float t = (float)(now - g_badgeStart) / g_animMs;
            if (t < 1.0f) {
                busy = true;
                g_frame.badgeScale =
                    t < 0.3f ? Lerp(1.0f, 1.3f, EaseOutCubic(t / 0.3f))
                             : Lerp(1.3f, 1.0f, EaseInOutCubic((t - 0.3f) / 0.7f));
            } else {
                g_frame.badgeScale = 1.0f;
                g_badgePop = false;
            }
        }

        if (g_badgeFade && g_statusMs > 0) {
            float t = (float)(now - g_badgeStart) / g_statusMs;
            g_frame.badgeAlpha = Clamp01(t);
            busy = busy || t < 1.0f;
        } else if (!g_badgeFade) {
            g_frame.badgeAlpha = 1.0f;
        }
    }

    Present(dx, dy, (BYTE)std::lround(255.0f * Clamp01(alpha)));

    if (g_phase == kPhaseHold && !busy && !g_following) {
        KillTimer(g_hwnd, kTimerAnim);
    }
}

// =============================================================================
// Topmost guard
//
// WS_EX_TOPMOST says "I am in the topmost band", not "I am at the front of it":
// inside the band the order is whatever the last SetWindowPos did. A fullscreen
// game, a recorder or a launcher overlay raises itself when it takes focus, and
// some games do it on every frame - the card then ends up behind the game a
// couple of hundred milliseconds after it appears, which reads as "the tip is
// still covered" or even as "there was no tip at all".
//
// So: only act when actually covered. Every tick, walk the Z-order upwards and
// look for a visible window of another process sitting over the card's centre.
// If there is one, raise once - after that the card is in front again and the
// next tick usually has nothing to do. No tug of war with other topmost apps,
// and no flicker from re-raising on a fixed schedule.
// =============================================================================

int g_topmostRaises;
bool g_topmostUnfixable;  // this show has raised and stayed covered anyway

void RaiseTopmost() {
    if (!g_hwnd) {
        return;
    }
    SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

// DWM-cloaked windows keep WS_VISIBLE while showing nothing: a closed shell
// flyout stays in the Z-order, and so does a UWP app that was suspended. Those
// aren't on screen, so they can't be covering the card either.
bool WindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked != 0;
}

// The window covering the card's centre, or null when nothing is. Only the
// centre point counts: the taskbar, a toast or a tooltip clipping a corner is
// supposed to sit next to the card, while a fullscreen game or a maximised
// window always covers the middle.
HWND FindCoveringWindow(HWND self) {
    RECT box{};
    if (!GetWindowRect(self, &box)) {
        return nullptr;
    }
    DWORD selfPid = 0;
    if (!GetWindowThreadProcessId(self, &selfPid)) {
        return nullptr;
    }
    const int centreX = box.left + (box.right - box.left) / 2;
    const int centreY = box.top + (box.bottom - box.top) / 2;

    HWND walk = GetWindow(self, GW_HWNDPREV);
    for (int i = 0; i < kTopmostMaxWalk && walk; i++) {
        DWORD pid = 0;
        RECT other{};
        if (IsWindowVisible(walk) && !IsIconic(walk) &&
            GetWindowThreadProcessId(walk, &pid) && pid && pid != selfPid &&
            GetWindowRect(walk, &other) && centreX >= other.left &&
            centreX < other.right && centreY >= other.top &&
            centreY < other.bottom && !WindowCloaked(walk)) {
            return walk;
        }
        walk = GetWindow(walk, GW_HWNDPREV);
    }
    return nullptr;
}

void StopTopmostGuard() {
    if (g_hwnd) {
        KillTimer(g_hwnd, kTimerTopmost);
    }
}

void StartTopmostGuard() {
    if (!g_hwnd) {
        return;
    }
    g_topmostRaises = 0;
    g_topmostUnfixable = false;
    RaiseTopmost();
    SetTimer(g_hwnd, kTimerTopmost, kTopmostGuardMs, nullptr);
}

void OnTopmostTick() {
    if (!g_hwnd) {
        return;
    }
    if (g_phase == kPhaseHidden) {
        StopTopmostGuard();
        return;
    }
    // The card fades out before it hides; nothing to fight for then.
    if (g_phase == kPhaseOut || !IsWindowVisible(g_hwnd)) {
        return;
    }
    // A window in a Z-order band no SetWindowPos can beat (a UWP overlay, an
    // exclusive-fullscreen game) stays in front no matter how often the card is
    // raised. Once that is known, stop: the raise is a wasted call and the 30 ms
    // timer behind it would only burn cycles for the rest of the tip.
    if (g_topmostUnfixable) {
        StopTopmostGuard();
        return;
    }
    if (!FindCoveringWindow(g_hwnd)) {
        return;
    }

    RaiseTopmost();
    g_topmostRaises++;

    // Look again straight away. Still covered means the raise did not take -
    // exclusive fullscreen, or the system simply refused - which is a different
    // fault from "had to raise once", and the only way to tell them apart
    // afterwards. Once per show: a game that re-raises every frame would
    // otherwise fill the log with it.
    if (!g_topmostUnfixable && FindCoveringWindow(g_hwnd)) {
        g_topmostUnfixable = true;
        Wh_Log(L"card still covered after raise #%d", g_topmostRaises);
        // Nothing more can be done for this tip, and the timer would otherwise
        // keep firing - at 30 ms once the raise count passed the fast limit -
        // for as long as the card stays up.
        StopTopmostGuard();
        return;
    }

    // Something is raising itself over the card over and over. Speed up rather
    // than settle into a "visible for 100 ms, covered for 100 ms" flicker.
    if (g_topmostRaises > kTopmostRaisesBeforeFast) {
        SetTimer(g_hwnd, kTimerTopmost, kTopmostFastMs, nullptr);
    }
}

// Keeps the card off a taskbar that slides in and out. Same shape as the
// standalone build's FollowTaskbar: re-read the band every 60 ms (a few
// microseconds - FindWindow plus GetWindowRect, deliberately no ABM query) and
// only touch the window once the place the card belongs has actually moved.
void FollowTaskbar() {
    if (!g_hwnd || g_phase == kPhaseHidden || g_phase == kPhaseOut) {
        return;
    }
    if (!g_monitor.valid) {
        return;
    }

    // Deliberately just the band: the tray window is already cached, so a tick
    // costs one GetWindowRect - while a full monitor + DPI query every 60 ms
    // would show up in the profile. Bounds and scale only change on
    // WM_DISPLAYCHANGE / WM_DPICHANGED, which OnDisplayChanged already handles.
    TaskbarBand band;
    if (!QueryTaskbarBand(g_monitor.handle, g_monitor.bounds, band) ||
        !band.valid) {
        return;
    }

    if (g_monitor.band.valid && EqualRect(&band.rect, &g_monitor.band.rect)) {
        // The taskbar has not moved. If the card just finished easing, take a
        // fresh screen copy at its new place - captured while it was still up,
        // so the snapshot would otherwise stay frozen where it used to be. Once
        // per move: a capture that comes back unusable must not turn into a
        // hide/show loop.
        if (g_backdropPending && g_phase == kPhaseHold && !g_following) {
            g_backdropPending = false;
            RefreshBackdrop();
        }
        return;
    }
    g_monitor.band = band;
    g_monitor.work = UsableRect(g_monitor.bounds, band);

    Placement target =
        ComputePlacement((float)g_cardWidth, (float)g_cardHeight, g_scale);
    if (target.x == g_posX && target.y == g_posY) {
        g_placement = target;
        return;
    }

    g_placement = target;
    g_followFromX = (float)g_posX;
    g_followFromY = (float)g_posY;
    g_followStart = GetTickCount64();
    g_followMs = g_settings.AnimationMs() > 0 ? kFollowMs : 0;
    g_following = g_followMs > 0;
    g_backdropPending = true;
    if (!g_following) {
        g_posX = target.x;
        g_posY = target.y;
    }
    // The taskbar raised itself when it slid out; put the card back on top of
    // it, otherwise a partial overlap still hides the lower half.
    RaiseTopmost();
    if (g_following) {
        SetTimer(g_hwnd, kTimerAnim, 16, nullptr);
    }
    StepAnimation();
}

// Only an auto-hidden taskbar moves while the card is up. Pinned, the band is
// static, so the poll would never find work to do and is not started at all.
void StartFollow() {
    if (!g_hwnd) {
        return;
    }
    KillTimer(g_hwnd, kTimerFollow);
    if (!g_monitor.valid || !TaskbarAutoHide()) {
        return;
    }
    SetTimer(g_hwnd, kTimerFollow, kFollowIntervalMs, nullptr);
}

void StopFollow() {
    if (g_hwnd) {
        KillTimer(g_hwnd, kTimerFollow);
    }
    g_following = false;
}

void StartStatusTween(bool fromOn) {
    ULONGLONG now = GetTickCount64();
    int animMs = g_settings.AnimationMs();
    g_statusMs = animMs > 0 ? std::max(1, (int)(animMs / 1.5f)) : 0;
    g_statusStart = now;
    g_clipMs = g_statusMs;
    g_clipStart = now;
    g_badgeStart = now;

    g_frame.prevOn = fromOn;
    g_frame.statusT = g_statusMs > 0 ? 0.0f : 1.0f;
    g_frame.clipT = g_clipMs > 0 ? 0.0f : 1.0f;
    g_frame.badgeAlpha = g_badgeFade ? 0.0f : 1.0f;
    g_frame.badgeScale = 1.0f;
}

void ShowTip(TipKind kind, bool on) {
    bool wasVisible = g_phase != kPhaseHidden;
    bool clip = IsClipKind(kind);

    bool fromOn = wasVisible ? g_frame.on : !on;

    if (clip) {
        bool same = wasVisible && g_currentKind == kind;
        g_count = same ? g_count + 1 : 1;
        g_badgePop = same;
        g_badgeFade = !same;
    } else {
        g_count = 0;
        g_badgePop = false;
        g_badgeFade = false;
    }

    g_currentKind = kind;
    g_lastShowTick = GetTickCount64();

    g_frame.kind = kind;
    g_frame.on = on;
    g_frame.count = g_count;

    g_theme = CurrentTheme();
    UpdateGeometry();
    EnsureOnScreen();
    RefreshBackdrop();

    int animMs = g_settings.AnimationMs();
    g_animMs = animMs;
    StartStatusTween(fromOn);

    if (!wasVisible) {
        g_phase = animMs > 0 ? kPhaseIn : kPhaseHold;
        g_phaseStart = GetTickCount64();
        if (animMs <= 0) {
            g_frame.statusT = 1.0f;
            g_frame.clipT = 1.0f;
            g_frame.badgeAlpha = 1.0f;
        }
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    } else if (g_phase == kPhaseOut) {
        // A tip arrived while the card was fading out: keep it on screen
        // instead of letting the pending hide finish.
        g_phase = kPhaseHold;
        g_phaseStart = GetTickCount64();
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    }

    if (g_hwnd) {
        // Another topmost window (the taskbar itself, most likely) may have
        // been raised above the card since the last tip - and it may keep doing
        // it while this tip is up, which is what the guard below watches for.
        StartTopmostGuard();
        SetTimer(g_hwnd, kTimerAnim, 16, nullptr);
        KillTimer(g_hwnd, kTimerHold);
        SetTimer(g_hwnd, kTimerHold,
                 (UINT)Clamp(g_settings.displayMs, 500, 10000), nullptr);
    }

    // The one-shot snapshot above was taken while the window was still hidden;
    // from here on the live path keeps it current, where Windows allows it.
    StartLiveCapture();

    StartFollow();
    StepAnimation();
}

void BeginHide() {
    if (g_phase == kPhaseHidden) {
        return;
    }
    // Stop chasing the taskbar: the exit slide owns the position from here.
    StopFollow();
    // The card is on its way out; it has nothing left to defend.
    StopTopmostGuard();
    if (g_hwnd) {
        KillTimer(g_hwnd, kTimerHold);
    }
    int animMs = g_settings.AnimationMs();
    if (animMs <= 0) {
        HideNow();
        return;
    }
    g_animMs = animMs;
    g_phase = kPhaseOut;
    g_phaseStart = GetTickCount64();
    if (g_hwnd) {
        SetTimer(g_hwnd, kTimerAnim, 16, nullptr);
    }
}

// =============================================================================
// Tip handling
// =============================================================================

bool ShouldSkip(bool honorFullscreen) {
    if (OtherInstanceRunning()) {
        return true;
    }
    if (honorFullscreen && g_settings.suppressFullscreen && FullscreenActive()) {
        return true;
    }
    return false;
}

void HandleTip(TipKind kind, bool predictedOn) {
    if (!g_settings.notify[kind]) {
        return;
    }
    if (ShouldSkip(true)) {
        return;
    }

    if (IsToggleKind(kind)) {
        // The hook thread already predicted the post-toggle state from its own
        // (authoritative) key state; a key-up message corrects it if wrong.
        ShowTip(kind, predictedOn);
        return;
    }

    if (kind == kInsert || kind == kPaste) {
        ShowTip(kind, false);
        return;
    }

    // Copy and cut are only reported once the clipboard really changed; this is
    // what keeps Ctrl+C in a console window from showing a false "Copied".
    g_clipIntentActive = true;
    g_clipIntentKind = kind;
    if (g_hwnd) {
        KillTimer(g_hwnd, kTimerClip);
        SetTimer(g_hwnd, kTimerClip, kClipIntentMs, nullptr);
    }
}

// Display name of an input language, for the card: the localized name of its
// locale, the native name if that is missing, and the raw handle as a last
// resort so the card can never come up blank.
std::wstring LanguageDisplayName(HKL hkl) {
    wchar_t locale[LOCALE_NAME_MAX_LENGTH] = {};
    LANGID langId = (LANGID)((ULONG_PTR)hkl & 0xFFFF);
    if (LCIDToLocaleName(langId, locale, LOCALE_NAME_MAX_LENGTH, 0) > 1) {
        for (DWORD kind : {LOCALE_SLOCALIZEDDISPLAYNAME,
                           LOCALE_SNATIVEDISPLAYNAME}) {
            wchar_t name[128] = {};
            if (GetLocaleInfoEx(locale, kind, name, 128) > 1) {
                return name;
            }
        }
    }
    wchar_t fallback[16];
    swprintf(fallback, 16, L"0x%08X", (unsigned)(ULONG_PTR)hkl);
    return fallback;
}

// A language or IME-mode change reported by the hook thread.
void HandleLanguageTip(int mode, HKL hkl) {
    if (!g_settings.notify[kLanguage]) {
        return;
    }
    if (ShouldSkip(true)) {
        return;
    }
    switch (mode) {
        case 1:
            g_languageText = Strings().imeChinese;
            break;
        case 2:
            g_languageText = Strings().imeEnglish;
            break;
        default:
            g_languageText = LanguageDisplayName(hkl);
            break;
    }
    if (g_languageText.empty()) {
        return;
    }
    ShowTip(kLanguage, true);
}

// A toggle seen only by the polling fallback (elevated window in front).
void HandlePoll(TipKind kind, bool state) {
    if (g_phase != kPhaseHidden && kind == g_currentKind &&
        g_frame.on == state) {
        return;
    }
    if (!g_settings.notify[kind]) {
        return;
    }
    if (ShouldSkip(true)) {
        return;
    }
    ShowTip(kind, state);
}

void HandleClipboardUpdate() {
    if (g_clipIntentActive) {
        g_clipIntentActive = false;
        if (g_hwnd) {
            KillTimer(g_hwnd, kTimerClip);
        }
        ShowTip(g_clipIntentKind, false);
        return;
    }

    if (!g_settings.notify[kCopy] || !g_settings.notifyAnyCopy) {
        return;
    }
    if (!ClipboardHasContent()) {
        return;
    }
    // Clipboard managers read and write the same data right after a copy; those
    // back-to-back updates must not show a second tip.
    if (GetTickCount64() - g_lastShowTick < kClipMergeMs) {
        return;
    }
    if (ShouldSkip(true)) {
        return;
    }
    ShowTip(kCopy, false);
}

// =============================================================================
// Raw Input
//
// The third way in, and the only one that both survives an elevated window and
// never samples: the hook is cut by UIPI, and polling can miss a quick tap that
// starts and ends between two 50 ms reads. Raw Input comes off the device stack
// straight to a window that asked for it with RIDEV_INPUTSINK, so neither focus
// nor integrity level matters. It is what makes the fallback below an actual
// fallback rather than the only line of defence.
//
// The card window itself is the sink: 1x1, hidden most of the time, and
// INPUTSINK does not care about either. Keys therefore arrive on the UI thread
// and go straight to HandleTip - no cross-thread post, no extra window.
//
// It is listened to only while the keyboard hook is blind (an elevated window in
// front). Raw Input reports the physical key even when a low-level-hook remapper
// - PowerToys Keyboard Manager, AutoHotkey - swallows it, so a Caps Lock remapped
// to Esc or Ctrl would raise a Caps Lock card on every press and flip the tracked
// state for a toggle the system never made. Where the hook does see the press,
// the hook is the better witness and Raw Input has nothing to add.
// =============================================================================

bool RegisterRawInputKeys(HWND hwnd, bool enable) {
    RAWINPUTDEVICE device{};
    device.usUsagePage = 0x01;  // HID_USAGE_PAGE_GENERIC
    device.usUsage = 0x06;      // HID_USAGE_GENERIC_KEYBOARD
    device.dwFlags = enable ? RIDEV_INPUTSINK : RIDEV_REMOVE;
    device.hwndTarget = enable ? hwnd : nullptr;
    return RegisterRawInputDevices(&device, 1, sizeof(device)) != FALSE;
}

int RawKeyIndex(const RAWKEYBOARD& key) {
    // Real Insert and numpad 0 share the scan code 0x52; only the E0 prefix
    // tells them apart. With Num Lock off, numpad 0 reports VK_INSERT as its
    // virtual key, so even the VKey match below must check that flag - a
    // numpad 0 press must never come back as kInsert.
    const bool e0 = (key.Flags & RI_KEY_E0) != 0;

    for (int i = 0; i < kPollKeyCount; i++) {
        if (KindToVk((TipKind)i) == key.VKey) {
            if ((TipKind)i == kInsert && !e0) {
                return -1;  // numpad 0 with Num Lock off
            }
            return i;
        }
    }
    // Nothing matched, so the virtual key either names some other key or is
    // missing altogether - 0xFF, which happens for a few keys that only ever
    // arrive by scan code. Only that second case may fall through to the scan
    // code, because a scan code alone does not identify a key: numpad 0 with
    // Num Lock on is 0x52 like Insert, and Ctrl+Break is 0x46 like Scroll Lock.
    // The E0 prefix is what separates each pair, and the numpad copies are the
    // ones without it.
    if (key.VKey != 0xFF) {
        return -1;
    }
    switch (key.MakeCode) {
        case 0x3A:
            return kCapsLock;
        case 0x45:
            return kNumLock;
        case 0x46:
            return e0 ? -1 : kScrollLock;
        case 0x52:
            return e0 ? kInsert : -1;
        default:
            return -1;
    }
}

void HandleRawKey(const RAWKEYBOARD& key, ULONGLONG at) {
    int index = RawKeyIndex(key);
    if (index < 0) {
        return;
    }

    if (key.Flags & RI_KEY_BREAK) {
        g_rawDown[index] = false;
        return;
    }

    ULONGLONG now = GetTickCount64();
    if (g_rawDown[index]) {
        if (now - g_rawDownAt[index] < (ULONGLONG)kRawRepeatGuardMs) {
            // Auto-repeat of a key that is still held. Refresh the stamp: a
            // hold longer than the guard must not turn into a new press every
            // guard period, which would flip the tracked state over and over.
            g_rawDownAt[index] = now;
            return;
        }
        // Nothing arrived for this key in a long time, so the key-up was lost;
        // this is a new press after all.
    }
    g_rawDown[index] = true;
    g_rawDownAt[index] = now;

    TipKind kind = (TipKind)index;

    if (kind == kInsert) {
        // Ctrl+Insert is a copy and Shift+Insert a paste, exactly as in the
        // hook path, so a modified Insert is not a bare press.
        if (CtrlDown() || ShiftDown()) {
            return;
        }
        if (!AllowTip(kind, at, kSourceRaw)) {
            return;
        }
        HandleTip(kind, false);
        return;
    }

    if (!IsToggleKind(kind)) {
        return;
    }
    if (!AllowTip(kind, at, kSourceRaw)) {
        return;  // another path claimed this press and already flipped
    }
    bool state = !g_toggleState[index];
    g_toggleState[index] = state;
    HandleTip(kind, state);
}

void HandleRawInput(LPARAM lParam) {
    // Nothing to add while the hook is working - and a press it would report
    // here but the hook never saw is a press the hook (or a remapper ahead of
    // it) ate, not a lock toggle. See the note above.
    if (InterlockedCompareExchange(&g_hookBlind, 0, 0) == 0) {
        return;
    }

    const UINT header = sizeof(RAWINPUTHEADER);

    UINT size = 0;
    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, header) !=
            0 ||
        size == 0 || size > sizeof(RAWINPUT)) {
        return;
    }

    RAWINPUT raw{};
    UINT read = size;
    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &read, header) ==
        (UINT)-1) {
        return;
    }
    if (raw.header.dwType != RIM_TYPEKEYBOARD) {
        return;
    }
    HandleRawKey(raw.data.keyboard, MessagePostTime());
}

void ApplySettings() {
    LoadSettings();
    DetectLanguage();
    InterlockedExchange(&g_languageEnabled,
                        g_settings.notify[kLanguage] ? 1 : 0);
    g_theme = CurrentTheme();
    g_backdrop.Invalidate();
    if (g_phase != kPhaseHidden) {
        UpdateGeometry();
        RefreshBackdrop();
        // Switching to Solid must stop the live path, and switching back has to
        // bring it up again - the card is on screen, so nobody else will.
        if (g_settings.material == kMaterialSolid) {
            StopLiveCapture();
        } else {
            StartLiveCapture();
        }
        StartFollow(); // the new settings may not want a follow loop at all
        StepAnimation();
    }
}

void OnTimer(UINT_PTR id) {
    switch (id) {
        case kTimerAnim:
            StepAnimation();
            break;
        case kTimerHold:
            BeginHide();
            break;
        case kTimerFollow:
            FollowTaskbar();
            break;
        case kTimerLiveBackdrop:
            OnLiveBackdropTick();
            break;
        case kTimerTopmost:
            OnTopmostTick();
            break;
        case kTimerInput:
            InputTick();
            break;
        case kTimerClip:
            if (g_hwnd) {
                KillTimer(g_hwnd, kTimerClip);
            }
            g_clipIntentActive = false;
            break;
        default:
            break;
    }
}

void OnDisplayChanged() {
    InvalidateTaskbarCache(); // monitors, DPI and taskbar placement may all differ
    g_theme = CurrentTheme();
    if (g_phase != kPhaseHidden) {
        // The margin and the blur radius are scaled per display, so the capture
        // worker has to be restarted on the new one rather than carried over.
        StopLiveCapture();
        UpdateGeometry();
        RefreshBackdrop();
        StartLiveCapture();
        StartFollow(); // auto-hide may have been switched on or off
        StepAnimation();
    } else if (ResolveTargetMonitor()) {
        UpdateScales();
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_TIP:
            HandleTip((TipKind)wParam, lParam != 0);
            return 0;
        case WM_APP_POLL:
            HandlePoll((TipKind)wParam, lParam != 0);
            return 0;
        case WM_APP_LANGUAGE_WATCH:
            StartInputWatch();
            return 0;
        case WM_APP_SETTINGS:
            ApplySettings();
            return 0;
        case WM_APP_QUIT:
            PostQuitMessage(0);
            return 0;
        case WM_CLIPBOARDUPDATE:
            HandleClipboardUpdate();
            return 0;
        case WM_TIMER:
            OnTimer((UINT_PTR)wParam);
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
        case WM_SETTINGCHANGE:
            OnDisplayChanged();
            return 0;
        case WM_INPUT:
            HandleRawInput(lParam);
            break;  // fall through to DefWindowProc, which releases the buffer
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// =============================================================================
// Threads
// =============================================================================

constexpr wchar_t kWindowClass[] = L"WindhawkWinTipsOsd";

DWORD WINAPI UiThreadProc(LPVOID) {
    // Only this thread gets per-monitor DPI awareness; the host process
    // (windhawk.exe) must keep whatever mode it already has. In Windhawk this
    // is a no-op - windhawk.exe declares PerMonitorV2 in its manifest - but the
    // standalone harness that includes this file has no manifest, so the call
    // is what makes the overlay render at the right size there.
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        auto setContext = (SetThreadDpiAwarenessContext_t)GetProcAddress(
            user32, "SetThreadDpiAwarenessContext");
        if (setContext) {
            setContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
    }

    GdiplusStartupInput input;
    if (GdiplusStartup(&g_gdiplusToken, &input, nullptr) != Ok) {
        Wh_Log(L"GdiplusStartup failed");
        return 0;
    }

    if (ResolveTargetMonitor()) {
        UpdateScales();
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kWindowClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassExW(&wc);

    g_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE |
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kWindowClass, L"WinTipsOSD", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
        GetModuleHandleW(nullptr), nullptr);

    if (!g_hwnd) {
        Wh_Log(L"CreateWindowEx failed: %lu", GetLastError());
        GdiplusShutdown(g_gdiplusToken);
        return 0;
    }

    AddClipboardFormatListener(g_hwnd);

    // Session-wide keyboard capture that does not go through the hook. It has
    // to be undone on the way out as well: Windhawk unloads a mod inside the
    // live process, so a leftover registration would keep posting WM_INPUT to a
    // window that no longer exists.
    g_rawInputOk = RegisterRawInputKeys(g_hwnd, true);
    if (!g_rawInputOk) {
        Wh_Log(L"RegisterRawInputDevices failed: %lu", GetLastError());
    } else {
        Wh_Log(L"raw input keyboard sink registered");
    }

    if (g_readyEvent) {
        SetEvent(g_readyEvent);
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    RegisterRawInputKeys(nullptr, false);
    RemoveClipboardFormatListener(g_hwnd);
    // Join the capture worker before the window it captures around disappears;
    // it also drops the timer resolution and the capture exclusion on the way
    // out, which matters when the mod is unloaded rather than just idling.
    StopLiveCapture();
    g_surface.Release();
    g_backdrop.image.Release();
    DestroyWindow(g_hwnd);
    g_hwnd = nullptr;
    DWriteShutdown();
    GdiplusShutdown(g_gdiplusToken);
    return 0;
}

// =============================================================================
// Elevated-window fallback
// =============================================================================

// Reading the foreground process' integrity level tells us whether the hook is
// being bypassed: Windows stops calling low-level keyboard hooks for input sent
// to a window of a higher integrity level, so an administrator window silently
// swallows every key. There is no way around UIPI from the hook side, but
// GetAsyncKeyState/GetKeyState report the session-wide state regardless - they
// are the fallback, and this check decides when to switch to them.

constexpr DWORD kRidMedium = 0x2000; // asInvoker under UAC, when the read fails

DWORD IntegrityRid(HANDLE process) {
    HANDLE token = nullptr;
    if (!OpenProcessToken(process, TOKEN_QUERY, &token)) {
        return 0;
    }
    DWORD rid = 0;
    DWORD size = 0;
    GetTokenInformation(token, TokenIntegrityLevel, nullptr, 0, &size);
    if (size > 0) {
        auto* label =
            (TOKEN_MANDATORY_LABEL*)HeapAlloc(GetProcessHeap(), 0, size);
        if (label) {
            if (GetTokenInformation(token, TokenIntegrityLevel, label, size,
                                    &size)) {
                SID* sid = (SID*)label->Label.Sid;
                BYTE* count = sid ? GetSidSubAuthorityCount(sid) : nullptr;
                if (count && *count > 0) {
                    // The level is the last sub-authority: S-1-16-<RID>.
                    DWORD* sub = GetSidSubAuthority(sid, (DWORD)(*count - 1));
                    if (sub) {
                        rid = *sub;
                    }
                }
            }
            HeapFree(GetProcessHeap(), 0, label);
        }
    }
    CloseHandle(token);
    return rid;
}

// True while the input desktop is the one this process runs on. A locked
// workstation or a UAC prompt puts the secure desktop in front: it delivers no
// window events to us, the keyboard hook isn't called for it, and
// GetAsyncKeyState can't see its keys either. Nothing can be observed there, so
// nothing needs watching - and the poll would otherwise tick for hours behind a
// lock screen.
bool InputDesktopIsUser() {
    HDESK desktop = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
    if (!desktop) {
        return false;
    }
    // The interactive desktop is named "Default" in every language; Winlogon's
    // secure desktop is "Winlogon", UAC's is "Secure Desktop".
    wchar_t name[64] = {};
    DWORD needed = 0;
    bool user = false;
    if (GetUserObjectInformationW(desktop, UOI_NAME, name,
                                  sizeof(name) - sizeof(wchar_t), &needed)) {
        user = _wcsicmp(name, L"Default") == 0;
    }
    CloseDesktop(desktop);
    return user;
}

// True when the foreground window outranks this process. The verdict is cached
// by window handle: the foreground doesn't change between two polls, so this
// opens one process token per window rather than one per tick.
//
// A window whose process can't be opened at all (a protected one) counts as
// higher: polling a little too much costs nothing, missing it costs the tip.
bool ForegroundOutranksSelf() {
    if (!InputDesktopIsUser()) {
        return false;
    }
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        // No foreground window does not mean the hook is working. Fullscreen
        // transitions, a display-mode rebuild and the moment the secure desktop
        // goes away all return null for a while, and the window in front is
        // usually still the elevated one. Treating that as "hook fine" silences
        // every tip for the whole stretch; one extra poll costs a handful of
        // GetAsyncKeyState calls, so assume the fallback is needed.
        return true;
    }
    if (g_levelKnown && foreground == g_levelHwnd) {
        return g_levelRaised;
    }

    DWORD pid = 0;
    bool raised = false;
    if (GetWindowThreadProcessId(foreground, &pid) && pid) {
        HANDLE process =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (process) {
            DWORD rid = IntegrityRid(process);
            CloseHandle(process);
            raised = rid == 0 || rid > g_selfRid;
        } else {
            raised = true;
        }
    }

    g_levelHwnd = foreground;
    g_levelRaised = raised;
    g_levelKnown = true;
    return raised;
}

void StartFallbackPoll() {
    if (g_pollRunning) {
        return;
    }
    // The user may be holding a key as the elevated window takes over: take the
    // current state as the baseline so it isn't reported as a fresh press.
    for (int i = 0; i < kPollKeyCount; i++) {
        g_pollDown[i] = (GetAsyncKeyState(KindToVk((TipKind)i)) & 0x8000) != 0;
    }
    g_pollRunning = true;
    InterlockedExchange(&g_hookBlind, 1);
    SetTimer(nullptr, kThreadTimerPoll, kPollMs, nullptr);
    Wh_Log(L"fallback poll started");
}

void StopFallbackPoll() {
    if (!g_pollRunning) {
        return;
    }
    g_pollRunning = false;
    InterlockedExchange(&g_hookBlind, 0);
    KillTimer(nullptr, kThreadTimerPoll);
    ZeroMemory(g_pollDown, sizeof(g_pollDown));
    Wh_Log(L"fallback poll stopped");
}

void RefreshFallbackPoll() {
    if (ForegroundOutranksSelf()) {
        StartFallbackPoll();
    } else {
        StopFallbackPoll();
    }
}

void CALLBACK ForegroundChangedProc(HWINEVENTHOOK, DWORD event, HWND, LONG, LONG,
                                    DWORD, DWORD) {
    if (event != EVENT_SYSTEM_FOREGROUND) {
        return;
    }
    RefreshFallbackPoll();
}

// Reads the real lock state and adopts it.
//
// On a thread of its own, and that is the point: GetKeyState answers out of the
// calling thread's own copy of the key state, which starts as the live state
// when the thread is created and only moves forward as that thread processes
// keyboard input. A thread that has not processed any yet therefore reports the
// live state - the same reason the startup seed can trust its read, and exactly
// what the hook thread, whose copy froze at load time, cannot offer.
//
// It reads and nothing else: g_vkDown and g_pollDown belong to the hook thread,
// which resets them itself before starting this thread.
DWORD WINAPI ResyncKeyStateThread(LPVOID) {
    // The event arrives as the switch happens; give the desktop a moment to
    // finish coming back, then make sure it really is ours.
    Sleep(150);
    if (!InputDesktopIsUser()) {
        return 0;
    }

    for (int kind = 0; kind <= kScrollLock; kind++) {
        g_toggleState[kind] = (GetKeyState(KindToVk((TipKind)kind)) & 1) != 0;
    }
    Wh_Log(L"key state resynced: caps %d num %d scroll %d",
           (int)g_toggleState[kCapsLock], (int)g_toggleState[kNumLock],
           (int)g_toggleState[kScrollLock]);
    return 0;
}

void ResyncKeyState() {
    // A resync already sleeping is as good as a new one: it reads the state
    // after the switch either way.
    if (g_resyncThread) {
        if (WaitForSingleObject(g_resyncThread, 0) == WAIT_TIMEOUT) {
            return;
        }
        CloseHandle(g_resyncThread);
        g_resyncThread = nullptr;
    }
    g_resyncThread =
        CreateThread(nullptr, 0, ResyncKeyStateThread, nullptr, 0, nullptr);
}

void StopResync() {
    if (!g_resyncThread) {
        return;
    }
    WaitForSingleObject(g_resyncThread, 1000);
    CloseHandle(g_resyncThread);
    g_resyncThread = nullptr;
}

// The input desktop changed: to the lock screen, to a UAC prompt, or back.
//
// This is the one place the tracked state is corrected against the system.
// Presses made on the secure desktop (typing a password is the classic moment to
// notice Caps Lock is on) go past the hook, past Raw Input and past the poll,
// which would leave every later card inverted until the mod restarted.
void CALLBACK DesktopChangedProc(HWINEVENTHOOK, DWORD event, HWND, LONG, LONG,
                                 DWORD, DWORD) {
    if (event != EVENT_SYSTEM_DESKTOPSWITCH) {
        return;
    }
    if (InputDesktopIsUser()) {
        // Anything held when the desktop went away was released somewhere
        // nothing could see it, so the press state from before the switch is
        // wrong. Cleared here rather than in the resync thread: this is the
        // thread that owns both tables.
        ZeroMemory(g_vkDown, sizeof(g_vkDown));
        for (int i = 0; i < kPollKeyCount; i++) {
            g_pollDown[i] =
                (GetAsyncKeyState(KindToVk((TipKind)i)) & 0x8000) != 0;
        }
        ResyncKeyState();
        RefreshFallbackPoll();
        return;
    }
    // Gone to the secure desktop: nothing to observe, nothing to poll.
    StopFallbackPoll();
}

// =============================================================================
// Language watch
// =============================================================================

// Windows has no notification for "the input language changed": it is a
// per-thread property that simply becomes something else, and the earliest and
// only dependable hint is the key press that asked for the change. So a switch
// key starts a short watch that samples the focused thread's state until it
// differs from what was there before the press, or until the window closes.

// Everything below runs on the UI thread: the hook posts WM_APP_LANGUAGE_WATCH
// and returns, so the questions asked here - which are cross-process and can
// each take tens of milliseconds - never sit in the way of the keyboard.
//
// Starting a few milliseconds later is harmless: the baseline only has to be
// older than the switch, and no switch happens on the key-down that starts the
// watch. Shift alone toggles the IME mode on its release, Ctrl+Shift and
// Alt+Shift on the release of the combination, and Win+Space goes through the
// shell - in every case a later event, tens of milliseconds later.

void StopInputWatch() {
    if (!g_inputWatching) {
        return;
    }
    g_inputWatching = false;
    if (g_hwnd) {
        KillTimer(g_hwnd, kTimerInput);
    }
    g_inputHasLast = false;
}

void StartInputWatch() {
    if (!g_hwnd) {
        return;
    }
    // A watch that is already running holds a baseline from before the first
    // key of the combination, which is a better baseline than one taken now:
    // Win+Space switches on the Space press, and this runs a message later, so
    // a busy UI thread (a backdrop grab, a frame) could read the *new* layout
    // and then wait for a change that already happened. Keep what we have.
    if (!g_inputWatching || !g_inputHasLast) {
        InputState baseline = CurrentInputState();
        if (baseline.Usable()) {
            g_inputLast = baseline;
            g_inputHasLast = true;
        } else {
            // Unreadable in the middle of a switch (fullscreen transition,
            // desktop recomposition): a baseline from some other app would
            // report the wrong language, so drop it and take the first good
            // sample instead.
            g_inputHasLast = false;
        }
    }
    g_inputTicks = 0;
    // SetTimer on an id that already runs restarts it, which is what a second
    // switch key during a watch should do.
    g_inputWatching = true;
    SetTimer(g_hwnd, kTimerInput, kInputPollMs, nullptr);
}

void InputTick() {
    if (++g_inputTicks * kInputPollMs > kInputWindowMs) {
        StopInputWatch();
        return;
    }

    InputState now = CurrentInputState();
    // Skip this sample rather than stop watching: a fullscreen switch or a
    // resolution change leaves the foreground empty for a moment, and quitting
    // here would miss the change that follows it.
    if (!now.Usable()) {
        return;
    }
    if (!g_inputHasLast) {
        g_inputLast = now;
        g_inputHasLast = true;
        return;
    }

    if (now.threadId != g_inputLast.threadId) {
        // Same app, new thread: a game rebuilding its window after a fullscreen
        // switch, or an IME window created on demand. The language really did
        // change, so report it. A different app (Alt+Tab) only refreshes the
        // baseline - otherwise every window switch would raise a card.
        bool sameApp = now.processId == g_inputLast.processId;
        bool hklChanged = now.hkl != g_inputLast.hkl;
        g_inputLast = now;
        if (sameApp && hklChanged) {
            HandleLanguageTip(0, now.hkl);
            StopInputWatch();
        }
        return;
    }

    if (now.hkl != g_inputLast.hkl) {
        g_inputLast = now;
        HandleLanguageTip(0, now.hkl);
        StopInputWatch();
        return;
    }

    if (now.HasIme() && g_inputLast.HasIme()) {
        if (now.IsNative() == g_inputLast.IsNative()) {
            g_inputLast = now;
            return;
        }
        bool native = now.IsNative();
        g_inputLast = now;
        HandleLanguageTip(native ? 1 : 2, nullptr);
        StopInputWatch();
        return;
    }

    if (g_inputLast.HasIme()) {
        // The baseline had an IME mode and this sample has none: the target
        // thread was probably busy and the 40 ms message timed out. Keep the
        // baseline - overwriting it with "no IME" would silence Shift for good.
        return;
    }

    g_inputLast = now;
}

void PollTick() {
    // The foreground event can be missed; re-checking here also ends the poll
    // the moment an ordinary window comes back to the front.
    if (!ForegroundOutranksSelf()) {
        StopFallbackPoll();
        return;
    }

    // Ctrl+Insert is a copy and Shift+Insert a paste, exactly as in the hook
    // path, so a modified Insert must not be reported as a bare press.
    const bool insertModified = CtrlDown() || ShiftDown();

    // Nothing below corrects the tracked state against GetKeyState: that read is
    // this thread's message-queue mirror, and the hook thread never receives
    // keyboard messages, so the mirror freezes at load time and a "correction"
    // would rewrite a correct state into a stale one. ResyncKeyState() is where
    // the real state is read, on a thread whose mirror is still fresh.
    for (int i = 0; i < kPollKeyCount; i++) {
        int vk = KindToVk((TipKind)i);
        bool down = (GetAsyncKeyState(vk) & 0x8000) != 0;
        bool pressed = down && !g_pollDown[i];
        g_pollDown[i] = down;

        if (i == kInsert) {
            // GetAsyncKeyState sees only the virtual key: with Num Lock off,
            // numpad 0 presses read as VK_INSERT here, and there is no scan
            // code to disprove it. Raw Input, which can see the E0 prefix,
            // reports Insert on its own when registered - so the poll stays
            // out of its way and only covers the registration-failed case,
            // where the rare numpad-0 misreport beats losing Insert entirely.
            if (pressed && !insertModified && !g_rawInputOk && g_hwnd &&
                AllowTip((TipKind)kInsert, GetTickCount64(), kSourcePoll)) {
                // Same path as a hooked Insert: notify switch, fullscreen
                // check and the tip itself all live in HandleTip.
                PostMessageW(g_hwnd, WM_APP_TIP, (WPARAM)kInsert, 0);
            }
            continue;
        }

        if (pressed) {
            // A lock key toggles exactly once per press, so flipping the state
            // we track is exact - and unlike reading it here, it can't catch the
            // old value before the system has applied the toggle.
            if (!AllowTip((TipKind)i, GetTickCount64(), kSourcePoll)) {
                continue;  // the hook or Raw Input already reported this press
            }
            bool state = !g_toggleState[i];
            g_toggleState[i] = state;
            if (g_hwnd) {
                PostMessageW(g_hwnd, WM_APP_POLL, (WPARAM)i, state ? 1 : 0);
            }
        }
    }
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* info = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vk = info->vkCode & 0xFF;
        bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        bool up = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

        if (down) {
            // Ignore auto-repeat so holding a key fires exactly once.
            if (!g_vkDown[vk]) {
                g_vkDown[vk] = true;

                // Start the language watch before anything else: a switch key
                // is the only hint that the language may be about to change.
                // Posted rather than called: finding the input state means
                // asking the foreground app's IME window a question and waiting
                // for the answer, and every keystroke in the session waits for
                // this hook to return. The UI thread asks instead.
                if (InterlockedCompareExchange(&g_languageEnabled, 1, 1) != 0 &&
                    SwitchCandidate(vk) && g_hwnd) {
                    PostMessageW(g_hwnd, WM_APP_LANGUAGE_WATCH, 0, 0);
                }

                // ClipCtrlDown, not CtrlDown: with Right Alt held this is AltGr
                // and the letter is a character, not a clipboard shortcut.
                // The extended flag is what marks the real Insert key: with
                // Num Lock off, numpad 0 also arrives as VK_INSERT, and only
                // the E0-prefixed (extended) copy is Insert.
                const bool extended = (info->flags & LLKHF_EXTENDED) != 0;
                int kind = TipKindForVk(vk, extended, ClipCtrlDown(), ShiftDown());

                if (kind >= 0 &&
                    AllowTip((TipKind)kind, GetTickCount64(), kSourceHook)) {
                    bool predicted = false;
                    if (IsToggleKind((TipKind)kind)) {
                        // Flip our own copy of the state instead of reading the
                        // key here: a lock key toggles exactly once per press,
                        // so tracking it is exact even for rapid taps, while a
                        // read this early sometimes still returns the old value
                        // and would make the card flip back a moment later.
                        predicted = !g_toggleState[kind];
                        g_toggleState[kind] = predicted;
                    }
                    if (g_hwnd) {
                        PostMessageW(g_hwnd, WM_APP_TIP, (WPARAM)kind,
                                     predicted ? 1 : 0);
                    }
                }
            }
        } else if (up) {
            g_vkDown[vk] = false;
        }
    }

    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID) {
    g_hookThreadId = GetCurrentThreadId();

    // The fallback compares the foreground window against our own integrity
    // level, and that cannot change for a running process: read it once here.
    g_selfRid = IntegrityRid(GetCurrentProcess());
    if (g_selfRid == 0) {
        g_selfRid = kRidMedium;
    }

    // Seed the known state so the polling fallback doesn't report a bogus tip
    // for keys that are already on when the mod loads. Read on this thread
    // before it pumps a message, which is the one moment its key-state mirror is
    // the live state rather than a snapshot of it.
    for (int kind = 0; kind <= kScrollLock; kind++) {
        g_toggleState[kind] = (GetKeyState(KindToVk((TipKind)kind)) & 1) != 0;
    }

    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&LowLevelKeyboardProc, &module);

    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, module, 0);
    if (!g_hook) {
        Wh_Log(L"SetWindowsHookExW failed: %lu", GetLastError());
    }

    // Out-of-context window events are queued to the thread that installed the
    // hook, which is this one: it starts and stops the fallback as the front
    // window changes, cheaper and quicker than probing for it.
    g_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        ForegroundChangedProc, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_foregroundHook) {
        Wh_Log(L"SetWinEventHook failed: %lu", GetLastError());
    }

    // The same for the input desktop, which is where presses the mod can't see
    // happen: the lock screen and UAC prompts. Coming back from one of those is
    // the moment to re-read the real lock state, and going to one is the moment
    // to stop polling.
    g_desktopHook = SetWinEventHook(
        EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH, nullptr,
        DesktopChangedProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_desktopHook) {
        Wh_Log(L"SetWinEventHook(desktop) failed: %lu", GetLastError());
    }

    // The probe runs either way. Without the window event it is the only way to
    // notice an elevated window at all; with it, it is the insurance against
    // the event being filtered - which is the same UIPI rule that swallows the
    // hook, so the two failures go together. Its verdict is cached by window
    // handle, so while nothing is elevated it costs one comparison.
    SetTimer(nullptr, kThreadTimerProbe, kPollProbeMs, nullptr);

    RefreshFallbackPoll();

    // The low-level hook and the window events only arrive while this thread
    // pumps messages.
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_TIMER) {
            if (msg.wParam == kThreadTimerPoll) {
                PollTick();
            } else if (msg.wParam == kThreadTimerProbe) {
                RefreshFallbackPoll();
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }
    if (g_desktopHook) {
        UnhookWinEvent(g_desktopHook);
        g_desktopHook = nullptr;
    }
    KillTimer(nullptr, kThreadTimerProbe);
    StopFallbackPoll();
    // The resync thread writes the state this thread reads; it must be gone
    // before the thread that owns it is.
    StopResync();

    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
    }
    return 0;
}

}  // namespace

// =============================================================================
// Windhawk tool mod entry points
// =============================================================================

BOOL WhTool_ModInit() {
    // Settings first: DetectLanguage() reads the language setting.
    LoadSettings();
    DetectLanguage();
    InterlockedExchange(&g_languageEnabled,
                        g_settings.notify[kLanguage] ? 1 : 0);

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (!g_uiThread) {
        Wh_Log(L"Failed to start the UI thread");
        return FALSE;
    }

    if (g_readyEvent) {
        WaitForSingleObject(g_readyEvent, 5000);
    }

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, nullptr);
    if (!g_hookThread) {
        Wh_Log(L"Failed to start the hook thread");
    }

    Wh_Log(L"WinTips OSD started");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_QUIT, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 3000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }
    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, 3000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
    Wh_Log(L"WinTips OSD stopped");
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
