// ==WindhawkMod==
// @id              mobile-open-animation
// @name            Mobile Open Animation
// @name:zh-CN      移动端风格的窗口打开动画
// @description     App windows zoom open from the icon (cursor position) with a fade, phone-style, using a splash panel with the app icon.
// @description:zh-CN 让应用窗口像手机桌面那样从你点击的图标处放大铺开并淡入：用一块带应用图标的占位面板做展开动画，等真窗口画好再交接。
// @version         1.0.0
// @author          Nico6719
// @github          https://github.com/Nico6719
// @include         *
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         LockApp.exe
// @exclude         SystemSettings.exe
// @exclude         TextInputHost.exe
// @exclude         Widgets.exe
// @exclude         PhoneExperienceHost.exe
// @exclude         GameBarPresenceWriter.exe
// @exclude         RuntimeBroker.exe
// @exclude         dllhost.exe
// @exclude         sihost.exe
// @exclude         taskhostw.exe
// @exclude         ctfmon.exe
// @exclude         CredentialUIBroker.exe
// @exclude         backgroundTaskHost.exe
// @exclude         SecurityHealthSystray.exe
// @exclude         OneDrive.exe
// @exclude         msedgewebview2.exe
// @compilerOptions -luser32 -lgdi32 -ldwmapi -lshell32 -lwinmm -ladvapi32 -lole32 -loleaut32 -luuid
// @license         MIT
// ==/WindhawkMod==

// The process exclusions above are window hosts that are known to be bad animation
// targets. They are metadata rather than a runtime check so the DLL is not loaded into
// them at all: a runtime check runs after injection, with all of this mod's imports
// (shell32, ole32, ...) already resolved in those processes.
// svchost.exe is deliberately absent - Windhawk already ignores pattern targets for it.

// ==WindhawkModReadme==
/*
# Mobile Open Animation

![Demo](https://raw.githubusercontent.com/Nico6719/windhawk-mods/assets/mobile-open-animation.gif)

Windows apps open the way they do on a phone: the window zooms open from the icon you
clicked, and fades in, instead of appearing all at once. The app icon keeps its size
the whole time; only the frame grows.

Works from desktop icons, the taskbar, the Start menu and search, and from
double-clicking a file that starts a program.

## Settings

Open the mod's "Settings" tab in Windhawk. The ones you are most likely to touch:

- **Zoom a splash panel with the app icon** - on by default, and what the animation
  above shows: a panel carrying the app icon appears the instant you click and zooms
  open, and hands over to the real window once it has painted. Turn it off to animate
  the real window instead: cheaper, but a slow-starting app shows no visible animation.
- **Zoom origin** - where the window grows out from. The default is the icon you
  clicked, which is the point of the mod; the other choices are the window centre and
  the bottom of the screen.
- **Start size** - the size of the square the animation starts from. 96 (the default)
  is roughly a desktop icon at 96 DPI.
- **Duration / Panel fade-out** - how fast the animation runs. Phones use 200-300ms.
- **Frame interval** - 0 follows the monitor's refresh rate, which is what you want on a
  high-refresh display.
- **Excluded window classes** - the mod ships with a list of window classes it never
  touches (UWP hosts, tray and tooltip windows, the desktop, and so on). Add your own
  here if a program misbehaves. To keep the mod out of a whole process instead, use
  Windhawk's own process exclusion list in the mod's Advanced tab - that also avoids
  loading the mod there at all.

Everything else has a description in the settings panel.

## What has no animation

- **UWP / WinUI apps** - Windows 11 Notepad, Settings, Terminal, Calculator, Photos and
  so on. They animate themselves, so the mod skips them on purpose. Note that
  `C:\Windows\System32\notepad.exe` may not even exist on Windows 11 (Notepad is a
  Store app now), so **do not use Notepad to test**.
- **Online games with anti-cheat, and security suites** - they refuse to be injected, so
  the mod cannot get in.
- **Dialogs** - off by default. There is a setting to enable them.
- **Windows of another process** - a launch only ever animates the windows of the
  process it started.
- **System UI hosts** - the shell experience hosts, the Start menu, search, the lock
  screen and a few more are excluded from the mod entirely, so it is not even loaded
  into them.
- Windows with absurdly small sizes.

## A program has no animation, now what

1. Open Windhawk, go to this mod's page and turn logging on for it (that switch is in
   Windhawk's own interface, not a mod setting).
2. Start that program once.
3. Read the log in Windhawk.

The decision for every window and the reason it was skipped are written there, with the
timings. Attach it to an issue and it can be diagnosed.

## Together with other animation mods

**Windows Animations** has an "Animate app launches" option of its own. Both mods hook
the same window-shown path and cloak the same windows, so **enable only one of the two
launch animations**. What is unique here is growing the window out of the icon you
clicked, and the icon splash panel; Windows Animations covers minimize, restore and
close, and an app-launch animation without the icon.

## Note

This mod applies to almost every process, matching Windhawk's own injection scope.
**Quit Windhawk before playing online games with anti-cheat**, or add the game to
Windhawk's process exclusion list.

---

## 中文

![效果](https://raw.githubusercontent.com/Nico6719/windhawk-mods/assets/mobile-open-animation.gif)

让 Windows 应用像手机那样「打开」：窗口从你点击的图标处放大铺开并淡入，而不是整块弹出来。
图标在整段动画里保持原大小，只有外框在长大。

桌面图标、任务栏、开始菜单和搜索、双击文件启动程序都支持。

### 设置

在 Windhawk 里打开本 mod 的「设置」标签页。常用的几个：

- **用图标占位面板做展开动画** —— 默认开，就是上面动图的效果：点下去立刻出现一块带应用图标的
  占位面板并放大铺开，等真窗口画好再交接。关掉后改成直接动画窗口本身，更省资源，但对启动慢的
  应用会看不出动画。
- **缩放锚点** —— 窗口从哪儿长出来。默认「你点击的图标」（这就是本 mod 的卖点），另外还有
  窗口中心和屏幕底部。
- **起始边长** —— 动画从多大的方块开始。96（默认）大约就是桌面图标的大小。
- **时长 / 面板淡出时长** —— 动画快慢。手机一般是 200-300ms。
- **帧间隔** —— 0 跟随显示器刷新率（高刷屏就该用这个）。
- **排除的窗口类名** —— mod 自带一份永不触碰的窗口类清单（UWP 宿主、托盘与提示窗口、桌面等）。
  某个程序表现异常时把它的类名加进去。要整个进程都不参与，请用本 mod「高级」标签页里 Windhawk
  自带的进程排除列表 —— 那样还能连带避免把 mod 加载进去。

其余设置项都带内置说明，鼠标移上去就能看到。

### 哪些窗口不会有动画

- **UWP / WinUI 应用** —— Win11 的记事本、设置、终端、计算器、照片等。它们有自己的打开动画，
  本 mod 会主动跳过。另外 Win11 上 `C:\Windows\System32\notepad.exe` 可能根本不存在
  （记事本已是商店版），**不要用记事本测试**。
- **带反作弊的在线游戏、杀毒软件** —— 这些程序拒绝被注入，mod 无从下手。
- **对话框** —— 默认不做动画，可以在设置里打开。
- **别的进程的窗口** —— 一次启动只给这次启动出来的那个进程的窗口做动画。
- **系统界面宿主** —— 壳体验宿主、开始菜单、搜索、锁屏等进程被整体排除，mod 根本不会被加载进去。
- 尺寸异常小的窗口。

### 某个程序没有动画，怎么办

1. 打开 Windhawk，在本 mod 的页面里为它打开日志开关（那是 Windhawk 界面里的开关，不是 mod 设置）。
2. 启动那个程序一次。
3. 在 Windhawk 里看日志。

每个窗口的判定结果、跳过原因和各阶段耗时都在里面。贴到 issue 里就能定位。

### 和别的动画 mod 一起用

**Windows Animations** 也有自己的「Animate app launches」选项。两个 mod 都挂窗口显示路径、
都给同一批窗口做 cloak，所以这两个「打开动画」**只能开一个**。本 mod 独有的东西是「从你点的
图标处放大」和那块图标占位面板；Windows Animations 管的是最小化、还原、关闭，以及没有图标的
打开动画。

### 注意

本 mod 作用于几乎所有进程，和 Windhawk 本身的注入范围一致。
**玩带反作弊的在线游戏之前请先退出 Windhawk**，或者把游戏加进 Windhawk 的进程排除列表。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- splash: true
  $name: Zoom a splash panel with the app icon
  $name:zh-CN: 用图标占位面板做展开动画
  $description: >-
    Show a panel with the app icon right after the click, zoom it open, and hand
    over once the real window has painted. This is the only way to react the
    instant you click, and it is what a phone does. Turn it off to fall back to
    animating the real window directly (cheaper, but slow-starting apps will
    show no visible animation).
  $description:zh-CN: >-
    开启时点击后立刻出现一块带应用图标的占位面板并放大铺开，等真窗口画好再交接。
    这是唯一能做到「点下去马上有反应」的方式，也是手机的观感。
    关闭后退回到直接动画真窗口（更省资源，但对启动慢的应用会看不出动画）。
- startSize: 96
  $name: Start size (px)
  $name:zh-CN: 起始边长 (px)
  $description: >-
    The animation starts from a square of this size - the icon itself, not some
    percentage of the window. 96 is roughly the size of a desktop icon at 96 DPI.
    Smaller looks more like "growing out of the icon", larger is gentler.
  $description:zh-CN: >-
    动画从这个边长的正方形开始 —— 也就是「图标本身」那一小块，不是窗口的某个百分比。
    96（96dpi 下）大约就是桌面图标的大小。调小会更像「从图标里长出来」，调大则更温和。
- durationMs: 240
  $name: Duration (ms)
  $name:zh-CN: 时长 (ms)
  $description: Length of the zoom animation. Phones typically use 200-300.
  $description:zh-CN: 展开动画时长。手机一般 200-300。
- fadeInPercent: 0
  $name: Initial fade-in (%)
  $name:zh-CN: 起始淡入 (%)
  $description: >-
    0 means the panel is fully opaque the moment it appears, which is what a
    phone does. Raise it to 10-20 for a fade-in, but keep it clearly below the
    point where the easing has finished its movement.
  $description:zh-CN: >-
    0 = 面板一出现就完全不透明（手机上就是这样，图标是瞬间出现的，不需要淡入）。
    想让它淡进来可以调到 10-20，但必须明显小于缓动走完位移的时间。
- handoffMs: 110
  $name: Panel fade-out (ms)
  $name:zh-CN: 面板淡出时长 (ms)
  $description: >-
    How long the panel takes to fade out once the real window is ready underneath it.
    The panel is already fully transparent by then, so this only bridges the moment the
    real window is revealed.
  $description:zh-CN: >-
    真窗口在面板底下画好之后，面板淡出所用的时间。这时面板已经全透明，这一步只是把
    「露出真窗口」这一下过渡得自然些。
- readyMs: 8000
  $name: Wait for content timeout (ms)
  $name:zh-CN: 等内容的超时 (ms)
  $description: >-
    How long the panel may stay on top at most. It stays until the app has
    really painted its UI, which is what a phone splash screen does. Chromium
    apps (Edge/Chrome/VS Code) often need 2-3 seconds when cold; too small a
    value removes the panel before they are ready and exposes an empty window.
    Normally the panel is removed earlier - this is only the upper bound.
  $description:zh-CN: >-
    面板最多顶多久。面板会一直顶到应用真的画好界面为止 —— 这才是手机的启动画面行为。
    Chromium 系（Edge/Chrome/VS Code）冷启动经常要 2-3 秒，设太小会在它还没画好时就把
    面板撤掉，露出空窗口。正常情况下面板会提前撤掉，这个值只是兜底上限。
- origin: cursor
  $name: Zoom origin
  $name:zh-CN: 缩放锚点
  $description: The point the window zooms out from.
  $description:zh-CN: 从哪个点向外放大。
  $options:
  - cursor: The clicked icon (never follows the mouse)
  - windowcenter: Window center
  - screenbottom: Screen bottom (as if rising from a taskbar icon)
  $options:zh-CN:
  - cursor: 点图标的位置（壳进程记录的真实点击位置；拿不到时退到窗口中心，绝不跟鼠标走）
  - windowcenter: 窗口中心
  - screenbottom: 屏幕底部（≈ 从任务栏图标冒出来）
- shellUiAnchor: true
  $name: Also anchor on the Start menu / taskbar
  $name:zh-CN: 也从开始菜单 / 任务栏取锚点
  $description: >-
    When a program is started from the Start menu or from the taskbar, grow the
    window out of that place instead of out of the click position. Only applies
    to origin = cursor. Turn it off to always grow from where you clicked.
  $description:zh-CN: >-
    从开始菜单或任务栏启动程序时，从那个位置长出来，而不是从点击位置。
    只对「缩放锚点 = cursor」生效。关掉它就永远从你点的位置长出来。
- curve: fastOutSlowIn
  $name: Easing curve (cubic bezier)
  $name:zh-CN: 缓动曲线（三次贝塞尔）
  $description: >-
    fastOutSlowIn is the curve Android's home screen uses for its open-app
    container transform: a short slow start, then fast, then a long settle.
    decelerate starts at its fastest with no slow start and feels harsher. None
    of these are linear or simple power functions.
  $description:zh-CN: >-
    fastOutSlowIn 就是 Android 桌面「点图标打开应用」那个容器变换用的曲线：
    开头一小段慢 → 很快 → 长长地收住。decelerate 起步就是最快、没有开头那一段，
    观感比 Android 脆。都不是匀速，也不是简单幂函数。
  $options:
  - fastOutSlowIn: fastOutSlowIn (the Android home screen curve, recommended)
  - decelerate: decelerate (fastest start, long settle)
  - emphasizedDecelerate: emphasized decelerate (even longer, more pronounced settle)
  - standard: standard (Material standard curve)
  - easeOutCubic: easeOutCubic (plain ease-out)
  - linear: linear (not recommended)
  $options:zh-CN:
  - fastOutSlowIn: fastOutSlowIn（安卓桌面那条曲线，推荐）
  - decelerate: decelerate（起步最快，收尾长）
  - emphasizedDecelerate: emphasized decelerate（收尾更长更夸张）
  - standard: standard（Material 标准曲线）
  - easeOutCubic: easeOutCubic（普通缓出）
  - linear: 匀速（不建议）
- frameMs: 0
  $name: Frame interval (ms)
  $name:zh-CN: 帧间隔 (ms)
  $description: >-
    0 follows the monitor's refresh rate (recommended): about 5.6ms/frame on a
    180Hz screen, 6.9ms on 144Hz. Hardcoding 60 would waste your refresh rate. A
    value greater than 0 uses that fixed interval and will not be faster than
    the monitor. Note that the real-window fallback path is limited to 60fps
    because it resizes the real window on every frame and forces the app to
    re-layout.
  $description:zh-CN: >-
    0 = 跟随显示器刷新率（推荐）：180Hz 屏约 5.6ms/帧、144Hz 约 6.9ms/帧，
    写死 60 只会白白浪费你的刷新率。填大于 0 的值则固定用该间隔，不会比显示器更快。
    注意真窗口回落路径最多 60fps —— 那条路每帧都要改真窗口几何、逼应用重排。
- splashBg: auto
  $name: Panel background
  $name:zh-CN: 占位面板底色
  $description: >-
    auto tries to infer the app's own background from the window class, so that the
    handover does not flash a different colour. Most modern apps paint their background
    themselves instead of setting a class brush, in which case the system light/dark app
    mode is used.
  $description:zh-CN: >-
    auto 会尝试从窗口类的背景画刷推断出应用自己的底色，让交接时不会闪色。多数现代应用
    是自己画背景、不设类画刷，这种情况下跟随系统的浅色/深色应用模式。
  $options:
  - auto: Automatic (follow the window background)
  - dark: Dark
  - light: Light
  $options:zh-CN:
  - auto: 自动（跟随窗口背景）
  - dark: 深色
  - light: 浅色
- animateDialogs: false
  $name: Animate dialogs too
  $name:zh-CN: 对话框也做动画
  $description: Standard dialogs such as Open, Save As and the Run box. Off by default; the animation is meant for app main windows.
  $description:zh-CN: 打开文件、另存为、"运行"对话框等 32770 类标准对话框。默认关，动画只给应用主窗口。
- excludeClasses:
    - ""
  $name: Excluded window classes
  $name:zh-CN: 排除的窗口类名
  $description: >-
    Window class names that skip the animation, e.g. Chrome_WidgetWin_1. One per
    line. To keep the mod out of a whole process, use Windhawk's own process
    exclusion list in the mod's Advanced tab - that also avoids loading the mod
    there in the first place.
  $description:zh-CN: >-
    不参与动画的窗口类名，例如 Chrome_WidgetWin_1。一行一个。
    要整个进程都不参与，请用本 mod「高级」标签页里 Windhawk 自带的进程排除列表 ——
    那样还能连带避免把 mod 加载进去。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <dwmapi.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <shlobj.h>
#include <commctrl.h>
#include <exdisp.h>
#include <oleauto.h>
#include <tlhelp32.h>
#include <atomic>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cwchar>
#include <string>
#include <vector>
#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif
#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif
#ifndef DWMWA_CLOAK
#define DWMWA_CLOAK 13
#endif
#ifndef COLOR_ENDCOLORS
#define COLOR_ENDCOLORS 30
#endif

// Settings

// Scalars only: passed by value and copied into animation slots
struct AnimParams {
    bool splash;
    int startSizePx;
    int durationMs;
    int fadeInPercent;
    int handoffMs;
    int readyTimeoutMs;
    int originMode;      // 0 cursor, 1 window centre, 2 screen bottom
    bool shellUiAnchor;  // also anchor on the Start menu / taskbar (origin=cursor only)
    int easing;          // index into kEasingKeys
    int frameIntervalMs;
    int splashBgMode;    // 0 auto, 1 dark, 2 light
    bool animateDialogs;
};

static SRWLOCK g_lock = SRWLOCK_INIT;
static AnimParams g_params = {};
static std::vector<std::wstring> g_excludeClasses;

static std::wstring g_thisExeName;
static std::wstring g_thisExePath;
static bool g_isShell = false;  // whether this process is explorer.exe (the shell)

// Cross-process sharing: "where the user just clicked".
//
// This is what makes "zoom out of the icon" possible. When you click an
// already-running app (Edge, VS Code, Explorer - anything single-instance), the
// new window appears in the OLD process, which has no idea where you clicked.
// Guessing from its own cursor position gives you "the animation follows the
// mouse".
//
// Clicking a desktop icon, a taskbar button or a Start menu entry is all
// dispatched by the shell process (explorer.exe), so the shell writes the
// "cursor position + timestamp" into a named shared memory block at the moment
// it calls ShellExecuteExW, and the target process reads it when it shows its
// window. That way, no matter which process the window appears in, the position
// is the one the user actually clicked.

struct LaunchClick {
    volatile LONG tick;      // low 32 bits of GetTickCount64
    volatile LONG x;
    volatile LONG y;
    volatile LONG ownerPid;
    volatile LONG targetLen;      // 0 = the writer could not determine the target
    volatile LONG targetReady;    // set to 1 once targetExe is fully written
    wchar_t targetExe[64];        // lowercase exe name
};

static void ToLowerInPlace(std::wstring& s);  // used by ExtractExeName, defined later
static void WideToUtf8(PCWSTR w, char* out, int outChars);  // the exe name is printed in the log
static bool IsThreadPumping(HWND hwnd);  // used by RestoreWindowStyle, defined later

// With the "target exe must match" gate in place, 30 seconds cannot leak into
// another program
static const DWORD kClickValidMs = 30000;
static const DWORD kClickUnknownTargetMs = 1500;  // when the target is unknown
static const DWORD kClickNameMismatchMs = 3000;  // tolerated name mismatch

// How long after a click the process it launched may still be created. The bound
// has to be tight, and 10000ms was not: measured, opening PowerPoint twice from the
// Start menu both times picked up the click left on the desktop's "This PC" icon,
// at 2984ms and 6125ms before the process existed, and both were accepted as "the
// click that launched us" because they were inside the old 10 second window.
//
// The Start menu and search hosts are excluded from this mod, so a launch from them
// records NOTHING - whatever click is left in the slot belongs to something else
// entirely. What keeps a real launch working is that the shell dispatches it right
// after the click: measured over the reads whose click preceded the process, p50
// 141ms, p75 1437ms, 82% within 2000ms. Everything past a couple of seconds is a
// stub launcher at best (VSCodium 6.3s) and an unrelated click the rest of the time,
// and losing the stub-launcher case to the launch anchor is the cheaper mistake.
static const DWORD kLaunchGraceMs = 2000;

static HANDLE g_clickMapping = nullptr;
static LaunchClick* g_clickShared = nullptr;

static ULONGLONG g_initTick = 0;

static void ClickShareInit() {
    // Give the shared memory a DACL everyone can open. Otherwise, if a process at
    // a different integrity level (an elevated one, say) creates the block first,
    // Explorer cannot write into it and the whole mechanism fails silently.
    SECURITY_ATTRIBUTES sa = {};
    SECURITY_DESCRIPTOR sd;
    if (InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION) &&
        SetSecurityDescriptorDacl(&sd, TRUE, nullptr, FALSE)) {
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = &sd;
    }

    g_clickMapping = CreateFileMappingW(
        INVALID_HANDLE_VALUE, sa.nLength ? &sa : nullptr, PAGE_READWRITE, 0,
        sizeof(LaunchClick), L"Local\\WhMobileOpenAnimationClick");
    if (!g_clickMapping) return;
    g_clickShared = (LaunchClick*)MapViewOfFile(g_clickMapping, FILE_MAP_ALL_ACCESS, 0,
                                               0, sizeof(LaunchClick));
}

// SHELLEXECUTEINFOW::lpFile is a bare path - the arguments live in lpParameters - so a
// space is part of the path and must not end it. Cutting at the first space turned every
// target under "C:\Program Files\..." into "C:\Program", which matched nothing and
// silently fell back to the 1500ms unknown-target window. Only the quotes Windows may
// wrap a path in are stripped.
static std::wstring ExtractExeName(const wchar_t* path) {
    if (!path) return std::wstring();
    std::wstring s(path);
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"') {
        s = s.substr(1, s.size() - 2);
    }
    const size_t slash = s.find_last_of(L"\\/");
    if (slash != std::wstring::npos) s = s.substr(slash + 1);
    if (s.size() < 5) return std::wstring();
    std::wstring ext = s.substr(s.size() - 4);
    ToLowerInPlace(ext);
    if (ext != L".exe") return std::wstring();
    ToLowerInPlace(s);
    if (s.size() >= 64) s.resize(63);
    return s;
}

static void ClickShareRecord(const char* why, const wchar_t* targetExe,
                            const POINT* pt = nullptr) {
    if (!g_clickShared) {
        if (why) Wh_Log(L"  record click failed (shared memory unavailable) why=%S", why);
        return;
    }
    // pt is the position the message was queued with, which under load is not the same
    // as the cursor position by the time the message is dispatched.
    POINT p;
    if (pt) {
        p = *pt;
    } else if (!GetCursorPos(&p)) {
        return;
    }

    // targetReady is set last, so the reader can tell whether targetExe is a
// complete frame
    InterlockedExchange(&g_clickShared->targetReady, 0);
    InterlockedExchange(&g_clickShared->targetLen, 0);

    const std::wstring exe = targetExe ? std::wstring(targetExe) : std::wstring();
    if (!exe.empty()) {
        const int n = (int)exe.size();
        for (int i = 0; i < n; i++) {
            g_clickShared->targetExe[i] = exe[i];
        }
        g_clickShared->targetExe[n] = 0;
        InterlockedExchange(&g_clickShared->targetLen, n);
    }

    InterlockedExchange(&g_clickShared->x, p.x);
    InterlockedExchange(&g_clickShared->y, p.y);
    InterlockedExchange(&g_clickShared->ownerPid, (LONG)GetCurrentProcessId());
    InterlockedExchange(&g_clickShared->targetReady, 1);
    InterlockedExchange(&g_clickShared->tick, (LONG)GetTickCount64());
    // mouse-down is high frequency, so it is not logged line by line
    if (why) {
        char targetUtf8[64] = "(unknown)";
        if (!exe.empty()) WideToUtf8(exe.c_str(), targetUtf8, sizeof(targetUtf8));
        Wh_Log(L"  record click (%ld,%ld) why=%S target=%s", p.x, p.y, why,
               exe.empty() ? L"(unknown)" : exe.c_str());
    }
}

// Read without consuming: one launch may open several windows, and consuming the
// record on the main window would lose it for the rest.
//
// The target exe must be checked. The record is global, so a coordinate left by
// an earlier click on a DIFFERENT program stays visible to every process for the
// whole validity window. Without the check you get "zoomed out of another app".
//
// A record older than this process is the exception: it is almost certainly the
// click that launched us, and it has to be trusted past the short windows below.
// But it HAS to be bounded by how long the launch itself took. Measured: p50
// 141ms, p75 1437ms, p90 11672ms of click-to-process latency. An earlier version
// only asked "was the record older than me?", which any process started within
// 30 seconds of any click satisfies - so a process that had started on its own
// still grabbed a 29 second old click and every window zoomed out of the same
// spot. That is what kLaunchGraceMs bounds.
static bool ClickShareRead(POINT* out, bool* ours) {
    *ours = false;
    if (!g_clickShared) {
        static bool logged = false;
        if (!logged) {
            logged = true;
            Wh_Log(L"  click record: shared memory unavailable (skipping, later attempts follow)");
        }
        return false;
    }
    const LONG tick = InterlockedCompareExchange(&g_clickShared->tick, 0, 0);
    if (!tick) return false;
    if (!InterlockedCompareExchange(&g_clickShared->targetReady, 0, 0)) return false;
    const DWORD age = (DWORD)GetTickCount64() - (DWORD)tick;
    if (age > kClickValidMs) {
        Wh_Log(L"  click record: too old (%lu ms > %lu ms)", (unsigned long)age,
             (unsigned long)kClickValidMs);
        return false;
    }

    // clickToProcess <= kLaunchGraceMs means the record predates this process by
    // no more than a plausible launch latency, i.e. it is the click that started
    // us. The subtraction is unsigned on purpose: a record NEWER than this process
    // (belongs to some other program's click, or to a window this long-running
    // process is only now showing) wraps to a huge value and is rejected.
    const DWORD clickToProcess = (DWORD)g_initTick - (DWORD)tick;
    const bool spawnedByThisClick = clickToProcess <= kLaunchGraceMs;

    const LONG len = InterlockedCompareExchange(&g_clickShared->targetLen, 0, 0);
    if (len <= 0) {
        // When the target is unknown only a short window applies, because 30 seconds
        // would pick up a click made somewhere else entirely.
        //
        // spawnedByThisClick is the only thing that widens it. "The record was
        // written by this very process" used to count too, on the reasoning that an
        // in-process click is always for us - but explorer.exe is both the recorder
        // and a target, and measured, that let a 27 second old click through, after
        // which every File Explorer window grew out of that stale spot.
        const DWORD limit = spawnedByThisClick ? kClickValidMs : kClickUnknownTargetMs;
        if (age > limit) {
            Wh_Log(L"  click record: unknown target and already %lu ms old (limit %lu ms)",
                 (unsigned long)age, (unsigned long)limit);
            return false;
        }
    } else {
        wchar_t target[64] = {};
        for (LONG i = 0; i < len && i < 63; i++) {
            target[i] = g_clickShared->targetExe[i];
        }
        // The record is for another program. Past 3 seconds it is almost certainly
        // unrelated to this launch; within 3 seconds it is accepted, because when
        // activation goes through a shortcut, a launcher or UWP the name in lpFile
        // is not necessarily the final exe. Desktop icons are .lnk, so lpFile gives
        // "outlook.lnk" where the process calls itself "outlook.exe" - that
        // mismatch is the normal case, not an error, and spawnedByThisClick is what
        // lets it through.
        if (!spawnedByThisClick && _wcsicmp(target, g_thisExeName.c_str()) != 0 &&
            age > kClickNameMismatchMs) {
            Wh_Log(L"  click record: target is %s, this is %s (recorded %lu ms ago)", target,
                 g_thisExeName.c_str(), (unsigned long)age);
            return false;
        }
    }

    out->x = InterlockedCompareExchange(&g_clickShared->x, 0, 0);
    out->y = InterlockedCompareExchange(&g_clickShared->y, 0, 0);
    *ours = spawnedByThisClick;
    Wh_Log(L"  click record ok (%ld,%ld) recorded %lu ms ago target=%S launch delay=%lu ms%S",
         out->x, out->y, (unsigned long)age, len > 0 ? "matched" : "unknown (1500ms window)",
         (unsigned long)clickToProcess,
         spawnedByThisClick ? " (launch of this process)" : " (left by another process)");
    return true;
}

// Recorded at the moment the shell dispatches a launch. Browser helper processes
// and updates spawned through CreateProcess never reach here, so they cannot
// pollute the record. lpFile is what the user actually clicked, and a process
// uses it to confirm "this record is about me".
using ShellExecuteExW_t = decltype(&ShellExecuteExW);
static ShellExecuteExW_t pOrigShellExecuteExW = nullptr;

BOOL WINAPI HookedShellExecuteExW(LPSHELLEXECUTEINFOW info) {
    if (info) {
        ClickShareRecord("ShellExecuteExW", ExtractExeName(info->lpFile).c_str());
    } else {
        ClickShareRecord("ShellExecuteExW", nullptr);
    }
    return pOrigShellExecuteExW(info);
}


// Mouse-down is recorded from DispatchMessageW, and ONLY in the shell process.
// Two constraints, both measured the hard way:
// it must not be a blocking function - Windhawk's unload waits for no thread's stack to
// sit inside the mod DLL before FreeLibrary (ThreadsCallStackWaitForRegions), and a GUI
// thread parks inside GetMessageW for its whole idle time, so every reload burned the full
// 80 second bound and then returned threads into unmapped memory (25 explorer crashes);
// and it must not be a global low-level hook - with @include * that is invoked in 100+
// processes, serially, per input event, and the cursor visibly stutters.
// DispatchMessageW returns as soon as the window procedure does, and explorer's pump is
// PeekMessage-driven, so this is where its clicks are visible. Gating on g_isShell is the
// second half of the answer: one process instead of a hundred.
using DispatchMessageW_t = decltype(&DispatchMessageW);
static DispatchMessageW_t pOrigDispatchMessageW = nullptr;

LRESULT WINAPI HookedDispatchMessageW(const MSG* msg) {
    if (msg && (msg->message == WM_LBUTTONDOWN || msg->message == WM_LBUTTONDBLCLK)) {
        // Only a coordinate is known here, not which program was clicked, hence the
        // unknown-target window on the reading side. The position is taken from the
        // message, not from the cursor: the click was queued when the mouse was there.
        ClickShareRecord(nullptr, nullptr, &msg->pt);
    }
    return pOrigDispatchMessageW(msg);
}

// A process injected at creation reaches Wh_ModInit within tens of milliseconds of
// starting; anything older than this was already running and is a mod reload.
static const ULONGLONG kFreshLaunchMaxAgeMs = 2000;

// Whether THIS process was just created, as opposed to a mod reload landing in one
// that has been running for hours.
//
// This is the difference between a real launch anchor and a bogus one. A reload
// re-runs Wh_ModInit, so sampling the cursor there and calling it "the position at
// launch" is wrong: for the whole kLaunchAnchorMs window every window the process
// opens afterwards then grows out of wherever the mouse sat at reload time.
// Measured on explorer.exe (running since boot): anchor=(752,106) 来源=启动瞬间的鼠标位置
// three windows in a row, right after a reload.
static bool WeAreAFreshLaunch() {
    FILETIME created = {}, exited = {}, kernel = {}, user = {};
    if (!GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user)) {
        return false;
    }
    FILETIME now = {};
    GetSystemTimeAsFileTime(&now);
    ULARGE_INTEGER a = {};
    ULARGE_INTEGER b = {};
    a.LowPart = created.dwLowDateTime;
    a.HighPart = created.dwHighDateTime;
    b.LowPart = now.dwLowDateTime;
    b.HighPart = now.dwHighDateTime;
    if (b.QuadPart <= a.QuadPart) return false;
    return (b.QuadPart - a.QuadPart) / 10000ULL < kFreshLaunchMaxAgeMs;
}

// Launch anchor: the cursor position recorded when the process was just created,
// which is right after the user clicked the icon.
//
// The cursor position at ShowWindow time must never be used. The gap between
// double-clicking the icon and the window appearing is often hundreds of
// milliseconds or even seconds (process start, initialisation), and the hand has
// long left the icon by then, so the anchor lands on a random spot.
static POINT g_launchAnchor = {0, 0};
static bool g_launchAnchorValid = false;
// Every window shown within this interval after start-up counts as part of this
// launch and uses the launch anchor. It cannot be limited to "the first window":
// VS Code and Edge put other top-level windows up first, and if those take the
// anchor the main window falls back to the current cursor position.
static const ULONGLONG kLaunchAnchorMs = 15000;

// Diagnostics go to Windhawk's own log through Wh_Log: no file of our own, no ring
// buffer, no state, and the on/off switch is Windhawk's own log toggle for this mod.
//
// Wh_Log is called directly at every site rather than through a helper. The macro
// prefixes each line with the call site's line and function, and it only evaluates its
// arguments when logging is enabled for the mod - a helper takes both away, and it made
// every line report the helper's own line number.
//
// The format strings are wide, so %s takes a wchar_t* and %S a narrow (ASCII) one.
static void WideToUtf8(PCWSTR w, char* out, int outChars) {
    out[0] = '\0';
    if (!w || !w[0]) return;
    WideCharToMultiByte(CP_UTF8, 0, w, -1, out, outChars, nullptr, nullptr);
}

// "Skipped" logging that includes the window class, so it is obvious which rule
// rejected which class.
//
// The same (class + reason) pair is logged only once: control classes are shown
// over and over and would flood Windhawk's log. The reason must be part of the
// deduplication key, otherwise different reasons for the same class get swallowed.
static void DiagSkip(const char* where, HWND hwnd, const char* reason) {
    // The class name is always taken from the HWND itself, never from the pointer the
    // caller passed in.
    //
    // CreateWindowEx's className may be an ATOM (a small integer) returned by
    // RegisterClass, and reading it as a string dereferences a tiny address such as
    // 0xC0xx. That is what crashed Baidu Netdisk (wcsnlen+79 / wcsncpy+2D, caller
    // HookedCreateWindowExW), and why explorer and the Control Panel crashed in the
    // same msvcrt region.
    wchar_t clsBuf[256] = L"";
    if (hwnd) GetClassNameW(hwnd, clsBuf, 256);

    static SRWLOCK seenLock = SRWLOCK_INIT;
    static wchar_t seen[48][64];
    static char seenReason[48][96];
    static int seenCount = 0;

    char reasonBuf[96] = "";
    if (reason) {
        strncpy(reasonBuf, reason, sizeof(reasonBuf) - 1);
        reasonBuf[sizeof(reasonBuf) - 1] = '\0';
    }

    // try-lock: a hook path must never block on a lock (a suspended thread would
    // freeze everything behind it)
    if (!TryAcquireSRWLockExclusive(&seenLock)) {
        Wh_Log(L"  skip %S: class=? (dedup table busy) reason=%S", where,
             reason ? reason : "unknown");
        return;
    }
    bool dup = false;
    const wchar_t* key = clsBuf[0] ? clsBuf : L"(no class)";
    for (int i = 0; i < seenCount; i++) {
        if (wcscmp(seen[i], key) == 0 && strcmp(seenReason[i], reasonBuf) == 0) {
            dup = true;
            break;
        }
    }
    if (!dup && seenCount < 48) {
        wcsncpy(seen[seenCount], clsBuf, 63);
        seen[seenCount][63] = L'\0';
        strncpy(seenReason[seenCount], reasonBuf, sizeof(seenReason[0]) - 1);
        seenReason[seenCount][sizeof(seenReason[0]) - 1] = '\0';
        seenCount++;
    }
    ReleaseSRWLockExclusive(&seenLock);
    if (dup) return;

    // The window rect is not printed any more: it adds nothing to a skip line, and this
    // path runs for every rejected window show whether or not logging is enabled.
    Wh_Log(L"  skip %S: class=%s hwnd=%p reason=%S", where, key, (void*)hwnd,
           reason ? reason : "unknown");
}

// Settings access

static void ToLowerInPlace(std::wstring& s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
}

static int ClampInt(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static const PCWSTR kOriginKeys[] = {L"cursor", L"windowcenter", L"screenbottom"};
static const PCWSTR kEasingKeys[] = {L"fastOutSlowIn", L"decelerate",
                                     L"emphasizedDecelerate", L"standard",
                                     L"easeOutCubic", L"linear"};
static const PCWSTR kSplashBgKeys[] = {L"auto", L"dark", L"light"};

static int ReadEnumSetting(PCWSTR name, const PCWSTR* keys, int count, int def) {
    auto v = WindhawkUtils::StringSetting::make(name);
    for (int i = 0; i < count; i++) {
        if (wcscmp(v.get(), keys[i]) == 0) return i;
    }
    return def;
}

static void LoadSettings() {
    AnimParams p = {};
    p.splash = Wh_GetIntSetting(L"splash") != 0;
    p.startSizePx = ClampInt((int)Wh_GetIntSetting(L"startSize"), 16, 800);
    p.durationMs = ClampInt((int)Wh_GetIntSetting(L"durationMs"), 30, 2000);
    p.fadeInPercent = ClampInt((int)Wh_GetIntSetting(L"fadeInPercent"), 0, 100);
    p.handoffMs = ClampInt((int)Wh_GetIntSetting(L"handoffMs"), 0, 1000);
        // Not set yet returns 0, in which case the default of 8000 applies: the panel
    // stays on top until the app has really painted, which is what a phone splash
    // screen does.
    {
        const int v = (int)Wh_GetIntSetting(L"readyMs");
        p.readyTimeoutMs = v > 0 ? ClampInt(v, 500, 20000) : 8000;
    }
    // 0 means "follow the monitor refresh rate".
    p.frameIntervalMs = ClampInt((int)Wh_GetIntSetting(L"frameMs"), 0, 200);
    p.originMode = ReadEnumSetting(L"origin", kOriginKeys, 3, 0);
    p.shellUiAnchor = Wh_GetIntSetting(L"shellUiAnchor") != 0;
    p.easing = ReadEnumSetting(L"curve", kEasingKeys,
                               (int)(sizeof(kEasingKeys) / sizeof(kEasingKeys[0])), 0);
    p.splashBgMode = ReadEnumSetting(L"splashBg", kSplashBgKeys, 3, 0);
    p.animateDialogs = Wh_GetIntSetting(L"animateDialogs") != 0;

    // Wh_GetStringSetting never returns NULL - an unset value comes back as L"", so the
    // empty string is what ends the array. StringSetting frees the value for us.
    std::vector<std::wstring> classes;
    for (int i = 0;; i++) {
        auto v = WindhawkUtils::StringSetting::make(L"excludeClasses[%d]", i);
        if (!*v.get()) break;
        classes.emplace_back(v.get());
    }

    AcquireSRWLockExclusive(&g_lock);
    g_params = p;
    g_excludeClasses = std::move(classes);
    ReleaseSRWLockExclusive(&g_lock);
}

static AnimParams GetParams() {
    AcquireSRWLockShared(&g_lock);
    AnimParams p = g_params;
    ReleaseSRWLockShared(&g_lock);
    return p;
}

// Built-in exclusion list: the window classes below. The process list that used to
// live here is the @exclude metadata at the top of the file now, so those processes
// are not injected at all.

static const wchar_t* const kExcludedClasses[] = {
    L"ApplicationFrameWindow",              // UWP host frame
    L"Windows.UI.Core.CoreWindow",          // UWP
    L"Microsoft.UI.Content.PopupWindowSiteBridge",
    L"XamlExplorerHostIslandWindow",
    L"Windows.UI.Input.InputSite.WindowClass",
    L"Windows.UI.Composition.DesktopWindowContentBridge",
    L"MsoSplash",                           // Office splash screen
    L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
    L"NotifyIconOverflowWindow",
    L"Progman", L"WorkerW",
    L"TaskListThumbnailWnd", L"SysShadow",
    L"tooltips_class32", L"DropdownWindow",
    // Overlay / OSD. NVIDIA's fullscreen overlay is this class: it covers the screen
    // (1919x1080) without being topmost, so the "fullscreen and topmost" rule does not
    // catch it. The overlay forwards and handles input, and animating it means
    // cloaking it before it is shown, which stops its input routing from coming up -
    // the whole desktop becomes unclickable as soon as the overlay appears.
    L"CEF-OSC-WIDGET",
};

// Window decisions (reason explains every rejection, for troubleshooting)

static bool IsCloaked(HWND hwnd) {
    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked)))) {
        return cloaked != FALSE;
    }
    return false;
}

// Hide a window with DWM cloaking (DWMWA_CLOAK=13 is writable, DWMWA_CLOAKED=14 is
// read-only).
//
// This is the correct way to hide a window: DWM keeps compositing it and the app
// keeps drawing normally, but the user cannot see it. It touches no window styles,
// so there is no surface recreation and no flicker, and it works on
// DirectComposition windows (Chromium, Electron, Office) which carry
// WS_EX_NOREDIRECTIONBITMAP and turn black if WS_EX_LAYERED is added.
static bool SetCloak(HWND hwnd, bool cloak) {
    BOOL v = cloak ? TRUE : FALSE;
    return SUCCEEDED(DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &v, sizeof(v)));
}

static const char* IsExcludedClass(const wchar_t* cls) {
    for (const auto* c : kExcludedClasses) {
        if (wcscmp(cls, c) == 0) return "window class is on the built-in list";
    }
    AcquireSRWLockShared(&g_lock);
    bool found = false;
    for (const auto& c : g_excludeClasses) {
        if (c == cls) {
            found = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_lock);
    return found ? "window class is in excludeClasses" : nullptr;
}

// Whether the window nearly covers its monitor.
//
// The test for "borderless fullscreen": a fullscreen video player, a browser in
// F11 and a game all lack WS_CAPTION, yet they are obviously main windows, so a
// missing title bar must not get them skipped as a class.
static bool CoversMonitor(const RECT& rc) {
    MONITORINFO mi = {sizeof(MONITORINFO)};
    HMONITOR mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
    if (!GetMonitorInfo(mon, &mi)) return false;
    const long long monW = (long long)mi.rcMonitor.right - mi.rcMonitor.left;
    const long long monH = (long long)mi.rcMonitor.bottom - mi.rcMonitor.top;
    if (monW <= 0 || monH <= 0) return false;
    const long long w = (long long)rc.right - rc.left;
    const long long h = (long long)rc.bottom - rc.top;
    // Leave a little slack: the taskbar and display scaling edges can differ by a few
    // pixels
    return w * 100 >= monW * 92 && h * 100 >= monH * 92;
}

static bool ShouldAnimate(HWND hwnd, const AnimParams& p, const wchar_t* cls,
                          const char** reason) {
    if (!IsWindow(hwnd)) {
        *reason = "not a window";
        return false;
    }

    // A visible owner means a typical dialog or popup, so skip it. Some programs
    // (Chromium and friends) hang their main window off an invisible helper window,
    // where the owner is not visible and the window still counts as a main window.
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (owner && IsWindowVisible(owner)) {
        *reason = "has a visible owner (dialog/popup)";
        return false;
    }

    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    if (style & WS_CHILD) {
        *reason = "WS_CHILD child window";
        return false;
    }

    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) {
        *reason = "GetWindowRect failed";
        return false;
    }
    const bool fullscreen = CoversMonitor(rc);

    if (!(style & WS_CAPTION) && !fullscreen) {
        *reason = "no WS_CAPTION";
        return false;
    }
    // Maximized windows are only skipped on the fallback path that animates the real
    // window, because that path animates by changing the real window's rectangle and
    // changing a maximized window's rectangle drops it out of the maximized state.
    // The splash path never touches the real geometry (it only cloaks it), so
    // maximized windows animate as usual.
    if ((style & WS_MAXIMIZE) && !p.splash) {
        *reason = "WS_MAXIMIZE (the real-window path does not support it)";
        return false;
    }

    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    // Same for WS_EX_TOOLWINDOW: borderless fullscreen windows often carry it (to stay
    // out of the taskbar and Alt+Tab) and they are still main windows.
    if ((exStyle & WS_EX_TOOLWINDOW) && !fullscreen) {
        *reason = "WS_EX_TOOLWINDOW";
        return false;
    }
    // "Covers the screen and is always topmost", on the other hand, means an overlay
    // (a recording tool's OSD, NVIDIA's overlay), not an application main window.
    // Those must not be animated: opening an unrelated program would suddenly put an
    // animation on top of the screen.
    if (fullscreen && (exStyle & WS_EX_TOPMOST)) {
        *reason = "fullscreen topmost overlay (OSD)";
        return false;
    }
    if (exStyle & WS_EX_NOACTIVATE) {
        *reason = "WS_EX_NOACTIVATE";
        return false;
    }

    if (!p.splash) {
        // Both restrictions apply only to the fallback path that animates the real window,
        // because it hides and fades the real window using WS_EX_LAYERED plus alpha. A
        // composited window (DirectComposition, with NOREDIRECTIONBITMAP) goes black when
        // layered, and an app that drives its own opacity should not have the wheel taken
        // from it. The splash path hides the window with DWM cloaking and touches no
        // styles, so neither restriction applies there.
        if (exStyle & WS_EX_NOREDIRECTIONBITMAP) {
            *reason = "WS_EX_NOREDIRECTIONBITMAP (the real-window path cannot handle composition windows)";
            return false;
        }
        if (exStyle & WS_EX_LAYERED) {
            COLORREF key = 0;
            BYTE alpha = 255;
            DWORD flags = 0;
            if (!GetLayeredWindowAttributes(hwnd, &key, &alpha, &flags)) {
                *reason = "already layered, attributes unavailable";
                return false;
            }
            if (!(flags & LWA_ALPHA)) {
                *reason = "layered with a colour key";
                return false;
            }
            if (alpha < 255) {
                *reason = "the app animates its own transparency";
                return false;
            }
        }
    }

    // #32770 is the standard dialog class (the Run box, file pickers, message boxes).
    // Controlled by a setting, off by default.
    if (wcscmp(cls, L"#32770") == 0 && !p.animateDialogs) {
        *reason = "dialog class #32770 (enable it in the settings)";
        return false;
    }
    if (const char* r = IsExcludedClass(cls)) {
        *reason = r;
        return false;
    }

    // DWM cloaking is deliberately NOT checked here. Chromium programs (Edge, Chrome,
    // VS Code) hand their window over in a state where it is already WS_VISIBLE but
    // still cloaked by DWM, and treating that as a rejection is what once left the
    // whole family without animations. A cloaked window should not be refused; it
    // should be waited on until the app uncloaks it, which IsWindowReady handles.

    if ((rc.right - rc.left) < 160) {
        *reason = "too narrow";
        return false;
    }
    if ((rc.bottom - rc.top) < 100) {
        *reason = "too short";
        return false;
    }

    return true;
}

// Geometry / easing

// Easing: cubic bezier
//
// Power functions (x^3, x^5) make a poor ease-out: they look close to linear.
// Android and iOS entry animations use cubic bezier control points, so the same
// curve definitions are used here.

struct Curve {
    double x1, y1, x2, y2;
};

// Both ends of the control polygon are fixed at (0,0) and (1,1), so a curve needs
// only its two middle parameters
static const Curve kCurves[] = {
    // The container transform Android's home screen uses to open an app is
    // fast-out-slow-in: a short slow start, then fast, then a long settle.
    {0.40, 0.00, 0.20, 1.00},  // fastOutSlowIn: the default, Android's home screen curve
    {0.00, 0.00, 0.20, 1.00},  // decelerate: fastest from the start, no slow lead-in
    {0.05, 0.70, 0.10, 1.00},  // emphasized decelerate: an even longer settle
    {0.20, 0.00, 0.00, 1.00},  // standard: the Material standard curve
    {0.33, 1.00, 0.68, 1.00},  // easeOutCubic
    {0.00, 0.00, 1.00, 1.00},  // linear
};
static const int kCurveCount = (int)(sizeof(kCurves) / sizeof(kCurves[0]));

// The order of the options in the settings must match the curves above one to one, otherwise choosing A yields B
static_assert(sizeof(kEasingKeys) / sizeof(kEasingKeys[0]) ==
                  sizeof(kCurves) / sizeof(kCurves[0]),
              "kEasingKeys and kCurves must have the same order/count");

static double BezierAxis(double t, double p1, double p2) {
    const double mt = 1.0 - t;
    return 3.0 * mt * mt * t * p1 + 3.0 * mt * t * t * p2 + t * t * t;
}

static double BezierSlope(double t, double p1, double p2) {
    const double mt = 1.0 - t;
    return 3.0 * mt * mt * p1 + 6.0 * mt * t * (p2 - p1) + 3.0 * t * t * (1.0 - p2);
}

// Given x, find y: solve for the parameter t with Newton iteration
static double CubicBezier(double x, const Curve& c) {
    if (x <= 0.0) return 0.0;
    if (x >= 1.0) return 1.0;
    double t = x;
    for (int i = 0; i < 8; i++) {
        const double err = BezierAxis(t, c.x1, c.x2) - x;
        if (fabs(err) < 1e-6) break;
        const double d = BezierSlope(t, c.x1, c.x2);
        if (fabs(d) < 1e-9) break;
        t -= err / d;
        if (t < 0.0) t = 0.0;
        if (t > 1.0) t = 1.0;
    }
    return BezierAxis(t, c.y1, c.y2);
}

static float ApplyEasing(int easing, float t) {
    if (t <= 0.f) return 0.f;
    if (t >= 1.f) return 1.f;
    if (easing < 0 || easing >= kCurveCount) easing = 0;
    return (float)CubicBezier((double)t, kCurves[easing]);
}

static float SmoothStep(float t) {
    if (t <= 0.f) return 0.f;
    if (t >= 1.f) return 1.f;
    return t * t * (3.f - 2.f * t);
}

// Uniform scaling about origin: scale=1 gives back target itself
static RECT ScaleAbout(const RECT& target, POINT origin, float scale) {
    const double x = (double)target.left;
    const double y = (double)target.top;
    const double w = (double)(target.right - target.left);
    const double h = (double)(target.bottom - target.top);

    RECT r;
    r.left = (LONG)lround((double)origin.x + (x - (double)origin.x) * scale);
    r.top = (LONG)lround((double)origin.y + (y - (double)origin.y) * scale);
    r.right = r.left + (LONG)lround(w * scale);
    r.bottom = r.top + (LONG)lround(h * scale);
    return r;
}

// The start rectangle: a square of edge size centred on the anchor, which is the
// icon itself, not a percentage of the window.
static RECT StartRect(POINT anchor, int size) {
    RECT r;
    const int half = size / 2;
    r.left = anchor.x - half;
    r.top = anchor.y - half;
    r.right = r.left + size;
    r.bottom = r.top + size;
    return r;
}

// Linear interpolation from the start rectangle to the window rectangle: position
// and size change together, which is the container transform of "the icon grows
// into the window in place".
static RECT LerpRect(const RECT& a, const RECT& b, float e) {
    RECT r;
    r.left = (LONG)lround(a.left + (b.left - a.left) * e);
    r.top = (LONG)lround(a.top + (b.top - a.top) * e);
    r.right = (LONG)lround(a.right + (b.right - a.right) * e);
    r.bottom = (LONG)lround(a.bottom + (b.bottom - a.bottom) * e);
    return r;
}

static int LerpInt(int a, int b, float e) {
    return (int)lround(a + (b - a) * e);
}

// High-resolution timing
//
// Animation progress must never come from GetTickCount64: its granularity is the
// system tick (15.6ms by default), so even at 60fps the position only changes
// every 15.6ms and every other frame is a duplicate. A 240ms animation then has
// about 15 distinct positions, which reads as dropped frames.
// QueryPerformanceCounter is microsecond resolution and unaffected by the tick.

static ULONGLONG QpcNow() {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (ULONGLONG)li.QuadPart;
}

static ULONGLONG QpcFreq() {
    static ULONGLONG freq = 0;
    if (!freq) {
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        freq = li.QuadPart ? (ULONGLONG)li.QuadPart : 1;
    }
    return freq;
}

static float QpcMs(ULONGLONG from, ULONGLONG to) {
    return (float)((double)(to - from) * 1000.0 / (double)QpcFreq());
}

static float QpcSinceMs(ULONGLONG from) {
    return QpcMs(from, QpcNow());
}

// Frame interval: follow the monitor refresh rate
//
// Hardcoding 60fps is wrong: on a 120/144Hz screen it wastes the refresh rate and
// makes the animation look coarser than the UI around it. DWM's composition
// cadence is asked first, since it is what actually decides when a frame reaches
// the screen, then the display mode's refresh rate, then 60Hz.

// Uses the refresh rate straight from the display mode, in Hz, with 60Hz as the fallback.
static float GetDisplayPeriodMs(HWND hwnd) {
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (GetMonitorInfoW(mon, (MONITORINFO*)&mi)) {
        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) &&
            dm.dmDisplayFrequency >= 24 && dm.dmDisplayFrequency <= 500) {
            return 1000.0f / (float)dm.dmDisplayFrequency;
        }
    }
    return 1000.0f / 60.0f;
}

// --- Shell UI geometry -----------------------------------------------------
//
// Read the shell's own windows instead of guessing from the cursor. Verified by
// enumerating this machine (1920x1080, taskbar along the bottom, y 1032..1080):
//   Start button : Shell_TrayWnd's child of class "Start", title "开始",
//                  rect (849,1032)-(894,1080). The only real geometry the taskbar
//                  exposes.
//   search box   : NO window of its own. It is a XAML element inside the taskbar
//                  island (TrayDummySearchControl has a 0x0 rect), so it can only
//                  be approximated by the slot Windows reserves for it: the Start
//                  button's width again, to its right.
//   flyouts      : owned by StartMenuExperienceHost.exe / SearchHost.exe, and they
//                  have no window at all until the flyout is opened.

static const int kFlyoutSearchStripMinPx = 40;  // floor, so a short flyout still lands in the box

// The search box occupies the top strip of the Start menu and of the search
// flyout: about 7% of the flyout's height down (~42px on the default 600px Start
// menu). A few pixels off is invisible in a zoom, so this does not have to be
// exact.
static int FlyoutSearchStripY(const RECT& r) {
    const int h = r.bottom - r.top;
    int y = r.top + h * 7 / 100;
    if (y < r.top + kFlyoutSearchStripMinPx) y = r.top + kFlyoutSearchStripMinPx;
    return y;
}

static bool FindStartButtonRect(RECT* out) {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray) return false;
    HWND start = FindWindowExW(tray, nullptr, L"Start", nullptr);
    return start && GetWindowRect(start, out);
}

static bool GetTaskbarSearchPoint(POINT* out) {
    RECT sb;
    if (!FindStartButtonRect(&sb)) return false;
    const int h = sb.bottom - sb.top;
    out->x = sb.right + h / 2;
    out->y = (sb.top + sb.bottom) / 2;
    return true;
}

// The EnumWindows callback has to be a real CALLBACK (__stdcall) function, not a
// capture-less lambda. WNDENUMPROC is stdcall, and converting a lambda to it is not
// portable: it compiles under some flag sets and is rejected under others with
// "no matching function for call to 'EnumWindows'". A plain CALLBACK function is
// accepted by every flag set the engine and the editor use.
struct FlyoutSearch {
    const DWORD* pids;
    const bool* search;
    int count;
    RECT rect;
    bool isSearch;
    bool found;
};

static BOOL CALLBACK FindFlyoutWindowProc(HWND hwnd, LPARAM lp) {
    FlyoutSearch* p = (FlyoutSearch*)lp;
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    for (int i = 0; i < p->count; i++) {
        if (p->pids[i] != pid) continue;
        RECT r;
        if (!GetWindowRect(hwnd, &r)) continue;
        // The hosts also own small helper windows; the flyout is large
        if (r.right - r.left < 200 || r.bottom - r.top < 200) continue;
        p->rect = r;
        p->isSearch = p->search[i];
        p->found = true;
        return FALSE;
    }
    return TRUE;
}

// The open Start menu or search flyout, if there is one. Found by owning process
// rather than by window class: the class name is an implementation detail, while
// "a visible top-level window owned by StartMenuExperienceHost" is exactly what an
// open Start menu is. `isSearch` separates the two hosts, because their search
// boxes sit in different places.
static bool FindFlyoutRect(RECT* out, bool* isSearch) {
    static const wchar_t* const kHosts[] = {L"startmenuexperiencehost.exe",
                                            L"searchhost.exe", L"searchapp.exe"};
    DWORD pids[8] = {};
    bool search[8] = {};
    int count = 0;

    // One process snapshot to map host name -> pid, far cheaper than OpenProcess
    // per enumerated window.
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe = {};
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(snap, &pe)) {
            do {
                std::wstring exe = pe.szExeFile;
                ToLowerInPlace(exe);
                for (int h = 0; h < (int)(sizeof(kHosts) / sizeof(kHosts[0])); h++) {
                    if (exe == kHosts[h] &&
                        count < (int)(sizeof(pids) / sizeof(pids[0]))) {
                        pids[count] = pe.th32ProcessID;
                        search[count] = (h != 0);
                        count++;
                    }
                }
            } while (Process32NextW(snap, &pe));
        }
        CloseHandle(snap);
    }
    if (!count) return false;

    FlyoutSearch f = {};
    f.pids = pids;
    f.search = search;
    f.count = count;
    EnumWindows(FindFlyoutWindowProc, (LPARAM)&f);
    if (!f.found) return false;
    *out = f.rect;
    *isSearch = f.isSearch;
    return true;
}

// The flyout, sampled at injection time. It CANNOT be sampled when the window is
// shown: the Start menu and the search flyout close the moment you click, so by
// then their window is gone and the rect is unreadable. Measured: every attempt to
// read it at ShowWindow time failed, which is why the Start menu branch logged
// nothing at all until this moved here.
static bool g_launchFlyoutValid = false;
static RECT g_launchFlyout = {};
static bool g_launchFlyoutIsSearch = false;

// Cheap gate first: both flyouts are XAML islands, so while one is open the
// foreground window is a CoreWindow. Two calls normally, instead of the process
// snapshot plus the window walk below - which as an unconditional cost would run
// in every one of the hundred-plus injected processes at start-up.
static void ProbeLaunchFlyout() {
    HWND fg = GetForegroundWindow();
    if (!fg) return;
    wchar_t cls[64] = L"";
    GetClassNameW(fg, cls, 64);
    if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") != 0) return;
    if (FindFlyoutRect(&g_launchFlyout, &g_launchFlyoutIsSearch)) {
        g_launchFlyoutValid = true;
    }
}

// Anchor for a launch that came from the shell's own UI. False means no shell UI is
// involved and the caller should fall back to the click record / launch anchor.
//
// The decision is made from the CLICK position, never from the live cursor. Using
// the current cursor here is what made this mod hijack launches onto the taskbar:
// measured 来源=搜索框（开始按钮右侧，近似） anchor=(896,1056) - the taskbar - for
// windows the user had opened from the desktop, because the hand happened to be
// resting on the taskbar when the window finally appeared.
static bool ResolveShellUiAnchor(POINT* out, const char** source) {
    const POINT click = g_launchAnchor;

    if (g_launchFlyoutValid) {
        if (PtInRect(&g_launchFlyout, click)) {
            *out = click;  // the entry that was clicked
            *source = "start menu/search: the item that was clicked";
            return true;
        }
        if (g_launchFlyoutIsSearch && GetTaskbarSearchPoint(out)) {
            // Keyboard launch out of search: the box is the one on the taskbar, and
            // the flyout hangs directly below it.
            *source = "search box (right of the Start button, approximate)";
            return true;
        }
        out->x = (g_launchFlyout.left + g_launchFlyout.right) / 2;
        out->y = FlyoutSearchStripY(g_launchFlyout);
        *source = "start menu: search box";
        return true;
    }

    // No flyout at injection time. If the CLICK was on the taskbar, the launch came
    // from there, and the Start button is the only window on it to read.
    RECT tray, sb;
    HWND trayWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (trayWnd && GetWindowRect(trayWnd, &tray) && FindStartButtonRect(&sb) &&
        PtInRect(&tray, click)) {
        if (PtInRect(&sb, click)) {
            out->x = (sb.left + sb.right) / 2;
            out->y = (sb.top + sb.bottom) / 2;
            *source = "taskbar Start button (real position)";
            return true;
        }
        if (GetTaskbarSearchPoint(out)) {
            *source = "taskbar search box (right of the Start button, approximate)";
            return true;
        }
    }
    return false;
}

static POINT ResolveOrigin(const RECT& target, int mode, bool useLaunchAnchor,
                           bool shellUiAnchor, const char** source) {
    POINT o;
    *source = "windowcenter";

    switch (mode) {
        case 1:
            o.x = (target.left + target.right) / 2;
            o.y = (target.top + target.bottom) / 2;
            *source = "window centre";
            break;
        case 2: {
            MONITORINFO mi = {sizeof(MONITORINFO)};
            HMONITOR mon = MonitorFromRect(&target, MONITOR_DEFAULTTONEAREST);
            o.x = (target.left + target.right) / 2;
            o.y = GetMonitorInfo(mon, &mi) ? mi.rcWork.bottom : target.bottom;
            *source = "screen bottom";
            break;
        }
        default: {
            POINT p;
            bool ours = false;
            const bool haveClick = ClickShareRead(&p, &ours);

            // A fresh process means a launch, and a launch can have come from the
            // shell's own UI. Checked first because its geometry is real, and
            // because it is what a recorded click CANNOT cover: the Start menu and
            // search hosts are excluded from this mod and their input goes through
            // the XAML CoreWindow stack rather than a Win32 message loop, so no hook
            // here would ever see those clicks.
            if (useLaunchAnchor && g_launchAnchorValid && shellUiAnchor) {
                POINT shell;
                const char* shellSource = nullptr;
                if (ResolveShellUiAnchor(&shell, &shellSource)) {
                    o = shell;
                    *source = shellSource;
                    break;
                }
            }

            if (haveClick && ours) {
                o = p;
                *source = "click position (recorded by the shell process)";
                break;
            }
            if (useLaunchAnchor && g_launchAnchorValid) {
                o = g_launchAnchor;
                *source = "cursor position at launch";
                break;
            }
            if (haveClick) {
                // A long-running process showing another window: its own launch is
                // ancient, so the anchor is meaningless and the record is all there is
                o = p;
                *source = "click position (left by another process)";
                break;
            }
            // Fallback: the window centre. The current cursor position is deliberately
            //    never used, because that is exactly what makes the animation follow the
            //    mouse: by the time the window appears the hand has long left the icon.
            o.x = (target.left + target.right) / 2;
            o.y = (target.top + target.bottom) / 2;
            *source = "window centre (fallback when there is no click position)";
            break;
        }
    }
    return o;
}

// Alpha is linear and finishes before the easing does, so movement is still going on while the panel is visible.
struct FrameVals {
    float ease;  // eased progress
    BYTE alpha;
};

static FrameVals EvalCurves(int easing, float t, float fadeEnd) {
    FrameVals v;
    v.ease = ApplyEasing(easing, t);

    float fadeT = fadeEnd > 0.01f ? t / fadeEnd : 1.f;
    if (fadeT > 1.f) fadeT = 1.f;
    if (t >= 1.f) fadeT = 1.f;
    v.alpha = (BYTE)lround(255.0 * fadeT);
    return v;
}

// Animation slots

static const int kMaxAnimations = 8;
// The render surface is the final size at 1:1, so very large windows are expensive to fill every frame; past this limit the real-window path is used
static const long long kMaxPanelPixels = 12000000LL;
// The readiness ceiling when EndPaint is never seen (DXGI/DComp). CAD puts up an
// empty window first and only paints a second later, so handing over at 700ms
// exposes a blank panel.
static const ULONGLONG kReadyNoPaintMs = 2000;
// The same HWND is not animated twice within this window. CAD's "close the empty
// window, then ShowWindow the real one" is otherwise judged twice, and the second
// pass has no anchor left.
static const ULONGLONG kNoRepeatAnimMs = 3000;
// Wall-clock ceiling for the handover fade. A finishing step must have a hard
// bound: if the animation thread does not exit, the splash window keeps the DLL's
// window procedure alive, the engine refuses to unload and the whole mod hangs in
// Uninitializing. The measured offender is Outlook, which puts up its own #32770
// dialog during the handover.
static const ULONGLONG kHandoffWallClockMs = 2000;

struct AnimSlot {
    std::atomic<HWND> hwnd{nullptr};
    RECT target{};
    POINT origin{};
    ULONGLONG showTick = 0;  // when the window was shown; used only for the content timeout
    AnimParams params = {};

    // Real window
    bool cloakedByUs = false;  // the cloaking is ours (splash path)
    bool wasLayered = false;   // the app is layered by itself (real-window fallback path)
    bool addedLayered = false; // WS_EX_LAYERED is ours - only then may it be removed
    BYTE originalAlpha = 255;
    int lastRealAlpha = -1;
    std::atomic<bool> painted{false};  // EndPaint was observed after the window was shown

    // Splash panel
    HWND splash = nullptr;
    HDC splashDC = nullptr;
    HBITMAP splashBitmap = nullptr;
    HGDIOBJ splashOldBitmap = nullptr;
    uint32_t* splashBits = nullptr;
    int spriteW = 0;  // render surface size = final window size
    int spriteH = 0;
    HICON icon = nullptr;
    bool ownIcon = false;
    int iconSizePx = 64;   // icon edge length, computed for the final size
    int radiusPx = 8;      // corner radius, computed for the final size
    uint32_t bg = 0;
    unsigned ulwFailures = 0;
    // The frame interval actually used (derived from the refresh rate when set to 0)
    DWORD frameIntervalMs = 16;
    float displayPeriodMs = 1000.0f / 60.0f;
    // Parameters of the previous frame's content: the bitmap is not repainted while
    // size and radius stay the same (the fade keeps them constant, so nothing needs
    // repainting there, only alpha changes)
    int lastRenderW = -1;
    int lastRenderH = -1;
    int lastRenderRadius = -1;
    uint32_t lastRenderBg = 0;  // the background colour is part of the frame content: a change forces a repaint
    // Incremental repainting must remember where the previous frame actually drew (size, radius, icon rect)
    int drawnW = 0;
    int drawnH = 0;
    int drawnRadius = 0;
    RECT drawnIcon = {};
    int drawnIconSize = 0;
    uint32_t drawnBg = 0;   // background of the previous frame: a change forces a full repaint
    unsigned long long fillPixels = 0;  // pixels actually filled (after incremental updates)
    unsigned long long fullPixels = 0;  // what those same frames would cost with full repaints, for comparison
    // Whether this frame fights the app for z-order: yes while zooming, no during the handover (see ApplySplashFrame)
    bool raiseEachFrame = false;
};

static AnimSlot g_slots[kMaxAnimations];
static std::atomic<int> g_activeAnimations{0};
static std::atomic<bool> g_unloading{false};

// Every thread that runs mod code, so that the unload can wait for all of them.
//
// Windhawk unmaps this DLL immediately after Wh_ModUninit returns, so nothing may still
// be executing in it. Handles are registered when the thread is created, finished ones
// are reaped on the next registration, and Wh_ModUninit joins whatever is left.
// They are deliberately never closed from inside the threads themselves: that would
// turn the join into a wait on an invalid handle, which is no wait at all.
static SRWLOCK g_threadsLock = SRWLOCK_INIT;
// A vector rather than a fixed array: an animation thread releases its slot before it
// exits, so the slot can be reused while that thread is still in its last few
// instructions, and a burst can put more threads in flight than there are slots. A fixed
// array would silently drop a handle there, and a thread that is not joined is exactly
// the crash this is here to prevent.
static std::vector<HANDLE> g_modThreads;

static void RegisterModThread(HANDLE h) {
    if (!h) return;
    AcquireSRWLockExclusive(&g_threadsLock);
    for (auto it = g_modThreads.begin(); it != g_modThreads.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_modThreads.erase(it);
        } else {
            ++it;
        }
    }
    g_modThreads.push_back(h);
    ReleaseSRWLockExclusive(&g_threadsLock);
}

static void JoinModThreads() {
    // Two passes: a deferred thread that passed its unloading check just before
    // g_unloading was set can still start an animation thread, and that one is
    // registered while the first pass is running. Nothing else can appear - the hooks
    // are already removed by the time Wh_ModUninit is called.
    for (int pass = 0; pass < 2; pass++) {
        std::vector<HANDLE> take;
        AcquireSRWLockExclusive(&g_threadsLock);
        take.swap(g_modThreads);
        ReleaseSRWLockExclusive(&g_threadsLock);
        if (take.empty()) break;
        for (HANDLE h : take) {
            WaitForSingleObject(h, INFINITE);
            CloseHandle(h);
        }
    }
}

// Per-process failure streak: after a few handovers in a row that end with the
// window still invisible, this process stops animating entirely.
//
// The criterion is "the window is still not visible when the animation ends",
// which is the user seeing "the program will not open, there is just a blank
// panel". That is far worse than no animation, and for apps like this (self-drawn
// or D3D apps that paint very late) betting on the next attempt is not worth it.
static std::atomic<int> g_handoffFailStreak{0};
static std::atomic<bool> g_compatDisabled{false};
static const int kHandoffFailLimit = 3;

static thread_local bool g_inHook = false;
static thread_local bool g_internalMove = false;

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t pOrigShowWindow = nullptr;
using ShowWindowAsync_t = decltype(&ShowWindowAsync);
static ShowWindowAsync_t pOrigShowWindowAsync = nullptr;
using SetWindowPlacement_t = decltype(&SetWindowPlacement);
static SetWindowPlacement_t pOrigSetWindowPlacement = nullptr;
using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t pOrigSetWindowPos = nullptr;
using EndPaint_t = decltype(&EndPaint);
static EndPaint_t pOrigEndPaint = nullptr;
using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t pOrigCreateWindowExW = nullptr;
using CreateWindowExA_t = decltype(&CreateWindowExA);
static CreateWindowExA_t pOrigCreateWindowExA = nullptr;

static const wchar_t* const kSplashClass = L"WhMobileOpenAnimationSplash";
// The version the log prints is the engine's macro, built from @version, so it cannot
// disagree with the published metadata the way a hand-kept copy can - it said "v3.12" for a
// while after @version had moved to 1.0.0.

static int FindSlot(HWND hwnd) {
    if (g_activeAnimations.load(std::memory_order_relaxed) == 0) return -1;
    int found = -1;
    AcquireSRWLockShared(&g_lock);
    for (int i = 0; i < kMaxAnimations; i++) {
        if (g_slots[i].hwnd.load(std::memory_order_relaxed) == hwnd) {
            found = i;
            break;
        }
    }
    ReleaseSRWLockShared(&g_lock);
    return found;
}

// Recently animated windows: CAD calls ShowWindow once for an empty window and
// again for the real one, and by the second call the launch anchor is used up (the
// cursor left long ago), so the real window would fall back to the window centre.
// A short window of memory suppresses the duplicate judgement.
static SRWLOCK g_recentLock = SRWLOCK_INIT;
static HWND g_recentHwnd[kMaxAnimations] = {};
static ULONGLONG g_recentTick[kMaxAnimations] = {};

static bool TooSoon(HWND hwnd) {
    const ULONGLONG now = GetTickCount64();
    bool hit = false;
    AcquireSRWLockShared(&g_recentLock);
    for (int i = 0; i < kMaxAnimations; i++) {
        if (g_recentHwnd[i] == hwnd && now - g_recentTick[i] < kNoRepeatAnimMs) {
            hit = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_recentLock);
    return hit;
}

static void MarkAnimated(HWND hwnd) {
    const ULONGLONG now = GetTickCount64();
    AcquireSRWLockExclusive(&g_recentLock);
    int slot = 0;
    ULONGLONG oldest = ~0ULL;
    for (int i = 0; i < kMaxAnimations; i++) {
        if (g_recentHwnd[i] == nullptr || now - g_recentTick[i] > kNoRepeatAnimMs) break;
        if (g_recentTick[i] < oldest) {
            oldest = g_recentTick[i];
            slot = i;
        }
    }
    g_recentHwnd[slot] = hwnd;
    g_recentTick[slot] = now;
    ReleaseSRWLockExclusive(&g_recentLock);
}

static int AllocSlot(HWND hwnd) {
    AnimSlot* free_slot = nullptr;
    AcquireSRWLockExclusive(&g_lock);
    for (int i = 0; i < kMaxAnimations; i++) {
        if (g_slots[i].hwnd.load(std::memory_order_relaxed) == nullptr) {
            free_slot = &g_slots[i];
            free_slot->hwnd.store(hwnd);
            break;
        }
    }
    if (free_slot) g_activeAnimations.fetch_add(1, std::memory_order_relaxed);
    ReleaseSRWLockExclusive(&g_lock);
    if (!free_slot) return -1;
    return (int)(free_slot - g_slots);
}

static void ReleaseSlot(AnimSlot* slot) {
    // Clearing hwnd must be the last step: only then may the slot be reused
    slot->hwnd.store(nullptr);
    g_activeAnimations.fetch_sub(1, std::memory_order_relaxed);
}

static BOOL CallRealSetWindowPos(HWND hwnd, HWND after, int x, int y, int cx,
                                 int cy, UINT flags) {
    if (pOrigSetWindowPos) return pOrigSetWindowPos(hwnd, after, x, y, cx, cy, flags);
    return SetWindowPos(hwnd, after, x, y, cx, cy, flags);
}

// target and origin are rewritten by the SetWindowPos hook, so reads always go through these two snapshot helpers
static RECT SnapshotTarget(AnimSlot* slot) {
    AcquireSRWLockShared(&g_lock);
    RECT r = slot->target;
    ReleaseSRWLockShared(&g_lock);
    return r;
}

static POINT SnapshotOrigin(AnimSlot* slot) {
    AcquireSRWLockShared(&g_lock);
    POINT o = slot->origin;
    ReleaseSRWLockShared(&g_lock);
    return o;
}

// Splash panel: pixel rendering

static inline uint32_t MakePixel(int a, int r, int g, int b) {
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

// Linear interpolation between two opaque background colours (alpha stays 255)
static inline uint32_t LerpPixel(uint32_t from, uint32_t to, double e) {
    const int r0 = (int)((from >> 16) & 0xFF), g0 = (int)((from >> 8) & 0xFF),
              b0 = (int)(from & 0xFF);
    const int r1 = (int)((to >> 16) & 0xFF), g1 = (int)((to >> 8) & 0xFF),
              b1 = (int)(to & 0xFF);
    return MakePixel(255, r0 + (int)lround((r1 - r0) * e),
                     g0 + (int)lround((g1 - g0) * e),
                     b0 + (int)lround((b1 - b0) * e));
}

// The render surface's stride is spriteW, the final size, while each frame fills
// only the top-left fw by fh. The three functions below therefore must take the
// stride explicitly and never index by logical width.
static void FillPixels(uint32_t* bits, int stride, int w, int h, uint32_t px) {
    for (int y = 0; y < h; y++) {
        uint32_t* row = bits + (size_t)y * stride;
        for (int x = 0; x < w; x++) row[x] = px;
    }
}

// Offset rectangle fill for incremental repaints: touches only [x0,x1) x [y0,y1), clipping out-of-range parts
static void FillArea(uint32_t* bits, int stride, int spriteH, int x0, int y0, int x1,
                     int y1, uint32_t px) {
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > stride) x1 = stride;
    if (y1 > spriteH) y1 = spriteH;
    if (x0 >= x1 || y0 >= y1) return;
    for (int y = y0; y < y1; y++) {
        uint32_t* row = bits + (size_t)y * stride;
        for (int x = x0; x < x1; x++) row[x] = px;
    }
}

// Corners get 1px anti-aliasing. Only straight alpha is written here; premultiplying is done once, by PremultiplyRect.
static void DrawRoundedCorners(uint32_t* bits, int stride, int w, int h, int r, int a,
                               int rr, int gg, int bb) {
    if (r <= 0) return;
    const double cx[4] = {(double)r, (double)(w - r), (double)r, (double)(w - r)};
    const double cy[4] = {(double)r, (double)r, (double)(h - r), (double)(h - r)};
    const int sx[4] = {0, w - r, 0, w - r};
    const int sy[4] = {0, 0, h - r, h - r};

    for (int q = 0; q < 4; q++) {
        for (int y = 0; y < r; y++) {
            for (int x = 0; x < r; x++) {
                const double dx = (sx[q] + x + 0.5) - cx[q];
                const double dy = (sy[q] + y + 0.5) - cy[q];
                const double d = sqrt(dx * dx + dy * dy);
                const int py = sy[q] + y;
                const int px = sx[q] + x;
                if (py < 0 || py >= h || px < 0 || px >= w) continue;
                if (d <= r - 0.5) continue;  // inside the corner radius the background is already correct

                int cov = (d >= r + 0.5) ? 0 : (int)lround(255.0 * (r + 0.5 - d));
                if (cov < 0) cov = 0;
                if (cov > 255) cov = 255;
                bits[(size_t)py * stride + px] = MakePixel(a * cov / 255, rr, gg, bb);
            }
        }
    }
}

// UpdateLayeredWindow requires premultiplied alpha, while DrawIconEx and the
// background fill write straight values. Only the drawn area is processed: the
// icon rectangle and the four corners.
static void PremultiplyRect(uint32_t* bits, int stride, int w, int h, RECT r) {
    int x0 = r.left < 0 ? 0 : r.left;
    int y0 = r.top < 0 ? 0 : r.top;
    int x1 = r.right > w ? w : r.right;
    int y1 = r.bottom > h ? h : r.bottom;
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            const uint32_t p = bits[(size_t)y * stride + x];
            const uint32_t a = p >> 24;
            if (a == 255) continue;  // a=0 must be multiplied too (to zero the RGB): a premultiplied surface requires
    // RGB <= A
            const uint32_t b = ((p & 0xFF) * a) / 255;
            const uint32_t g = (((p >> 8) & 0xFF) * a) / 255;
            const uint32_t rr = (((p >> 16) & 0xFF) * a) / 255;
            bits[(size_t)y * stride + x] = (a << 24) | (rr << 16) | (g << 8) | b;
        }
    }
}

// Background: prefer the window class's own background brush so the handover does not flash a different colour
static bool SystemAppsUseLightTheme();  // used below, declared up front

static uint32_t ResolvePanelColor(HWND hwnd, int mode) {
    const uint32_t kDark = MakePixel(255, 0x1F, 0x1F, 0x1F);
    const uint32_t kLight = MakePixel(255, 0xFF, 0xFF, 0xFF);
    if (mode == 1) return kDark;
    if (mode == 2) return kLight;

    COLORREF c = 0xFFFFFFFF;
    bool ok = false;
    const ULONG_PTR br = (ULONG_PTR)GetClassLongPtr(hwnd, GCLP_HBRBACKGROUND);
    if (br > 0 && br <= (ULONG_PTR)(COLOR_ENDCOLORS + 1)) {
        c = GetSysColor((int)br - 1);  // system colour index (COLOR_x + 1)
        ok = true;
    } else if (br > 0) {
        LOGBRUSH lb = {};
        if (GetObjectW((HBRUSH)br, sizeof(lb), &lb) && lb.lbStyle == BS_SOLID) {
            c = lb.lbColor;
            ok = true;
        }
    }
    if (!ok) {
        // Modern apps rarely set a class background brush (they paint it themselves), so
        // auto inference usually fails here. Falling back to the system light/dark app
        // mode gets closer to the real colour than always using dark, which would flash
        // from a dark panel to a light window for light-theme apps.
        return SystemAppsUseLightTheme() ? kLight : kDark;
    }
    return MakePixel(255, GetRValue(c), GetGValue(c), GetBValue(c));
}

// whether the system's app mode is light or dark
static bool SystemAppsUseLightTheme() {
    DWORD v = 1;
    DWORD size = sizeof(v);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &size);
    return v != 0;
}

// Sample the colour the app actually paints, so the panel background can be
// carried over smoothly at the handover.
//
// PW_CLIENTONLY is required (without it the toolbar colour is sampled: measured
// on BitComet as the dark brown 51,34,12 while the real background is F0F0F0),
// and the luminance mode is used rather than the mean, which a few icon pixels
// would skew. It still works on cloaked windows, since it goes through
// WM_PRINTCLIENT and ignores visibility.
static COLORREF SampleWindowBackground(HWND hwnd) {
    if (!IsWindow(hwnd)) return 0xFFFFFFFF;

    // PrintWindow sends WM_PRINT/WM_PRINTCLIENT synchronously and has no timeout, so on
    // a window whose thread is busy it blocks this thread for as long as that thread
    // takes - and it runs before the first frame, so the delay is visible. A hung-window
    // probe bounds it; the sampling is optional and the panel colour is the fallback.
    DWORD_PTR probe = 0;
    if (!SendMessageTimeoutW(hwnd, WM_NULL, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 50,
                             &probe)) {
        return 0xFFFFFFFF;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = 48;
    bmi.bmiHeader.biHeight = -48;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC memDC = CreateCompatibleDC(nullptr);
    HBITMAP bmp =
        memDC ? CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0) : nullptr;
    if (!memDC || !bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        if (memDC) DeleteDC(memDC);
        return 0xFFFFFFFF;
    }
    HGDIOBJ old = SelectObject(memDC, bmp);
    const BOOL ok = PrintWindow(hwnd, memDC, PW_CLIENTONLY | PW_RENDERFULLCONTENT);

    // Take the dominant colour of the client area's top-left 48x48: bucket by
    // luminance into 32 bins, pick the fullest bin and average within it. A plain
    // mean is skewed by a few icon or text pixels, and sampling the centre 16x16
    // picks up the window's own content (toolbars, title) - neither equals the
    // background colour.
    unsigned long long r = 0, g = 0, b = 0;
    int n = 0;
    COLORREF best = 0xFFFFFFFF;
    if (ok && bits) {
        const uint32_t* px = (const uint32_t*)bits;
        struct Bucket {
            unsigned long long sumR, sumG, sumB;
            int count;
        } buckets[32] = {};
        for (int y = 0; y < 48; y++) {
            for (int x = 0; x < 48; x++) {
                const uint32_t p = px[(size_t)y * 48 + x] & 0x00FFFFFF;
                const int lum = (int)(((p >> 16) & 0xFF) * 30 + ((p >> 8) & 0xFF) * 59 +
                                     (p & 0xFF) * 11) / 100;
                const int li = lum * 32 / 256;
                buckets[li].sumR += (p >> 16) & 0xFF;
                buckets[li].sumG += (p >> 8) & 0xFF;
                buckets[li].sumB += p & 0xFF;
                buckets[li].count++;
            }
        }
        int bestCount = 0;
        int bestIdx = -1;
        for (int i = 0; i < 32; i++) {
            if (buckets[i].count > bestCount) {
                bestCount = buckets[i].count;
                bestIdx = i;
            }
        }
        // A fullest bin below 1/8 means the whole client area is a gradient, so the sampled colour is unreliable and discarded.
        // Pure black counts as a failure: PrintWindow returns all zeros for DComp/WinUI windows.
        if (bestIdx >= 0 && bestCount >= 48 * 48 / 8) {
            n = buckets[bestIdx].count;
            r = buckets[bestIdx].sumR;
            g = buckets[bestIdx].sumG;
            b = buckets[bestIdx].sumB;
            best = RGB((int)(r / n), (int)(g / n), (int)(b / n));
            if (best == RGB(0, 0, 0)) n = 0;  // all zeros means PrintWindow failed, not that the window really is black
        }
    }
    SelectObject(memDC, old);
    DeleteObject(bmp);
    DeleteDC(memDC);
    if (!ok || n == 0) return 0xFFFFFFFF;
    return best;
}

// Frame sizes to fall back through when even the probe fails.
static const int kIconRequestSizes[] = {256, 128, 64, 48, 32};

// The pixel width of an HICON, or 0 if it cannot be measured.
static int IconPixelSize(HICON icon) {
    ICONINFO ii = {};
    if (!GetIconInfo(icon, &ii)) return 0;
    int px = 0;
    if (ii.hbmColor) {
        BITMAP bm = {};
        if (GetObject(ii.hbmColor, sizeof(bm), &bm)) px = bm.bmWidth;
        DeleteObject(ii.hbmColor);
    }
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    return px;
}

// The 256px icon of the folder an Explorer window shows.
//
// A folder window's WM_GETICON/class icon is only the small frame (32/48px), so the
// panel scales it up and the Recycle Bin, the user profile folder and network places
// come out visibly blurry. The shell keeps a 256px copy of every folder icon in its
// system image list (SHIL_JUMBO) - the one the desktop and Explorer themselves draw
// from. There is no API from a window to that image-list index, but a folder window
// can be asked which folder it is showing, through its shell browser:
//
//   IShellWindows::Item(i) + IWebBrowser2::get_HWND() == hwnd
//     (FindWindowSW(SWC_EXPLORER) does not match folder windows - measured, it
//      returns S_FALSE even for a window the enumeration reports)
//   -> IServiceProvider -> SID_STopLevelBrowser -> IShellBrowser
//   -> QueryActiveShellView -> IFolderView -> IPersistFolder2::GetCurFolder
//   -> SHGetFileInfo(PIDL, SHGFI_SYSICONINDEX) -> SHGetImageList(SHIL_JUMBO)
//      -> GetIcon -> 256x256 HICON
//
// get_LocationURL would be one step shorter, but it is percent-encoded ("%20", UTF-8
// escapes), so every folder with a space or a non-ASCII name would have to be decoded
// first - and it is empty for virtual folders anyway. The view hands over the PIDL of
// every folder, file system or not, which is why it is the only path here.
//
// Measured: Recycle Bin iIcon=32, Network iIcon=17, a file system folder iIcon=106, all
// three 256x256, and for file system folders both ways report the same index.
//
// mingw has neither IImageList (commctrl.h only forward-declares it) nor
// IID_IImageList, so both are spelled out. The other GUIDs are spelled out for the
// same reason most people avoid -luuid, but as plain constants - the values are
// copied from the mingw headers.
struct IImageListIface : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Add(HBITMAP, HBITMAP, int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReplaceIcon(int, HICON, int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOverlayImage(int, int) = 0;
    virtual HRESULT STDMETHODCALLTYPE Replace(int, HBITMAP, HBITMAP) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddMasked(HBITMAP, COLORREF, int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Draw(IMAGELISTDRAWPARAMS*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove(int) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIcon(int, UINT, HICON*) = 0;
};

static const GUID kIID_IImageList = {0x46EB5926, 0x582E, 0x4017,
                                     {0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x09, 0x50}};
static const GUID kIID_IServiceProvider = {0x6d5140c1, 0x7436, 0x11ce,
                                           {0x80, 0x34, 0x00, 0xaa, 0x00, 0x60, 0x09, 0xfa}};
static const GUID kSID_STopLevelBrowser = {0x4c96be40, 0x915c, 0x11cf,
                                           {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};
static const GUID kIID_IShellBrowser = {0x000214e2, 0x0000, 0x0000,
                                        {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID kIID_IFolderView = {0xcde725b0, 0xccc9, 0x4519,
                                      {0x91, 0x7e, 0x32, 0x5d, 0x72, 0xfa, 0xb4, 0xce}};
static const GUID kIID_IPersistFolder2 = {0x1ac3d9f0, 0x175c, 0x11d1,
                                          {0x95, 0xbe, 0x00, 0x60, 0x97, 0x97, 0xea, 0x4f}};

// Returns nullptr for anything that is not a folder window, and for every failure
// along the chain above; the caller then draws the window's own icon, which is the
// old behaviour, so this can only make an icon better, never worse.
static HICON ShellFolderJumboIcon(HWND hwnd) {
    // Folder windows live in explorer.exe and nowhere else. Everything else
    // explorer owns (taskbar, tray, Alt+Tab, the desktop itself) is not a folder.
    if (g_thisExeName != L"explorer.exe") return nullptr;
    wchar_t cls[32] = {};
    if (!GetClassNameW(hwnd, cls, 32) || _wcsicmp(cls, L"CabinetWClass") != 0) return nullptr;

    // One animation thread per animation, so this is always the thread's first
    // apartment and S_OK; FAILED covers S_FALSE/RPC_E_CHANGED_MODE in theory only.
    if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) return nullptr;

    HICON icon = nullptr;
    IShellWindows* sw = nullptr;
    if (SUCCEEDED(
            CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                             IID_IShellWindows, (void**)&sw)) &&
        sw) {
        IWebBrowser2* wb = nullptr;
        // One pass: the folder has been navigated to before the window is shown, and
        // showing it is what starts the animation. A window that somehow is not in the
        // list yet falls back to its own icon below rather than holding up the frame.
        long count = 0;
        if (SUCCEEDED(sw->get_Count(&count))) {
            for (long i = 0; i < count && !wb; i++) {
                VARIANT vi;
                VariantInit(&vi);
                vi.vt = VT_I4;
                vi.lVal = i;
                IDispatch* d = nullptr;
                if (SUCCEEDED(sw->Item(vi, &d)) && d) {
                    IWebBrowser2* cand = nullptr;
                    if (SUCCEEDED(d->QueryInterface(IID_IWebBrowser2, (void**)&cand)) &&
                        cand) {
                        LONG_PTR wh = 0;
                        if (SUCCEEDED(cand->get_HWND(&wh)) && (HWND)wh == hwnd) {
                            wb = cand;  // keep the reference
                        } else {
                            cand->Release();
                        }
                    }
                    d->Release();
                }
                VariantClear(&vi);
            }
        }

        PIDLIST_ABSOLUTE pidl = nullptr;
        if (wb) {
            // Which folder the window is showing, from its own shell view.
            IServiceProvider* sp = nullptr;
            IShellBrowser* sb = nullptr;
            IShellView* sv = nullptr;
            IFolderView* fv = nullptr;
            IPersistFolder2* pf = nullptr;
            if (SUCCEEDED(wb->QueryInterface(kIID_IServiceProvider, (void**)&sp)) && sp)
                sp->QueryService(kSID_STopLevelBrowser, kIID_IShellBrowser, (void**)&sb);
            if (sp) sp->Release();
            if (sb) sb->QueryActiveShellView(&sv);
            if (sb) sb->Release();
            if (sv) sv->QueryInterface(kIID_IFolderView, (void**)&fv);
            if (sv) sv->Release();
            if (fv) fv->GetFolder(kIID_IPersistFolder2, (void**)&pf);
            if (fv) fv->Release();
            if (pf) pf->GetCurFolder(&pidl);
            if (pf) pf->Release();
            wb->Release();
        }
        sw->Release();

        if (pidl) {
            SHFILEINFOW sfi = {};
            if (SHGetFileInfoW((PCWSTR)pidl, 0, &sfi, sizeof(sfi),
                               SHGFI_PIDL | SHGFI_SYSICONINDEX)) {
                IImageListIface* il = nullptr;
                if (SUCCEEDED(SHGetImageList(SHIL_JUMBO, kIID_IImageList, (void**)&il)) && il) {
                    il->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &icon);
                    il->Release();
                    if (icon) Wh_Log(L"  folder icon iIcon=%d", sfi.iIcon);
                }
            }
            ILFree(pidl);
        }
    }

    if (!icon) Wh_Log(L"  folder icon unavailable, falling back to the window icon");
    CoUninitialize();
    return icon;
}


// The file a Windhawk window's icon should come from, when it is not the process's
// own.
//
// Windhawk's editor is NOT windhawk.exe: it is a copy of VSCodium shipped at
// <install>\UI\VSCodium.exe, and that copy's icon is still VSCodium's own - a blue
// coral on a white disc, which is what the animation used to show for a Windhawk
// window. Windhawk's own icon, the eagle, lives one level up in
// <install>\windhawk.exe. Verified by extracting both:
// UI\VSCodium.exe icon group 0 is the blue coral, windhawk.exe icon group 0 is the
// eagle, and src/windhawk/app/rsrc/app.ico in the Windhawk source is that eagle.
static bool WindhawkUiIconFile(std::wstring* out) {
    if (g_thisExeName != L"vscodium.exe") return false;
    const size_t exeSlash = g_thisExePath.find_last_of(L"\\/");
    if (exeSlash == std::wstring::npos) return false;
    const size_t dirSlash = g_thisExePath.find_last_of(L"\\/", exeSlash - 1);
    if (dirSlash == std::wstring::npos) return false;

    std::wstring dir = g_thisExePath.substr(dirSlash + 1, exeSlash - dirSlash - 1);
    ToLowerInPlace(dir);
    if (dir != L"ui") return false;

    const size_t parentSlash = g_thisExePath.find_last_of(L"\\/", dirSlash - 1);
    std::wstring parent =
        g_thisExePath.substr(parentSlash == std::wstring::npos ? 0 : parentSlash + 1,
                             dirSlash - (parentSlash == std::wstring::npos ? 0 : parentSlash + 1));
    ToLowerInPlace(parent);
    if (parent != L"windhawk") return false;

    *out = g_thisExePath.substr(0, dirSlash) + L"\\windhawk.exe";
    return true;
}

// App icon for the panel.
//
// Order: for an Explorer folder window the shell's 256px folder icon, then the
// window's own icon, then its class icon, then the exe's. The window's is what the
// application chose to show and what Windows shows for that window in the title bar,
// the taskbar and Alt+Tab; a folder window's is just too small to scale up. The only
// other exception is Windhawk's own windows, whose icon comes from
// <install>\windhawk.exe instead - see WindhawkUiIconFile.
static HICON AcquireAppIcon(HWND hwnd, int wantPx, bool* needDestroy) {
    *needDestroy = false;
    const int want = ClampInt(wantPx, 8, 512);

    if (HICON folder = ShellFolderJumboIcon(hwnd)) {
        *needDestroy = true;
        return folder;
    }

    std::wstring iconFile;
    const bool preferFile = WindhawkUiIconFile(&iconFile);
    if (preferFile && !iconFile.empty()) {
        // Ask for the largest frame the file has, not for the size about to be drawn.
        // PrivateExtractIcons returns an icon of exactly the requested size, picking
        // the closest resource and scaling it, so asking for 64 out of a file whose
        // frames start at 256 throws the good frame away. cxIcon = cyIcon = 0 reports
        // the size it would pick: measured 256 for windhawk.exe, whose frames are
        // 256/64/48/32/24/16.
        int nativePx = 0;
        HICON probe = nullptr;
        if (PrivateExtractIconsW(iconFile.c_str(), 0, 0, 0, &probe, nullptr, 1, 0) == 1 &&
            probe) {
            nativePx = IconPixelSize(probe);
            DestroyIcon(probe);
        }
        const int req = nativePx > want ? nativePx : want;
        HICON fromFile = nullptr;
        if (PrivateExtractIconsW(iconFile.c_str(), 0, req, req, &fromFile, nullptr, 1, 0) ==
                1 &&
            fromFile) {
            *needDestroy = true;
            return fromFile;
        }
        for (int size : kIconRequestSizes) {
            if (size >= req) continue;  // already tried, or bigger than what worked
            HICON fallback = nullptr;
            if (PrivateExtractIconsW(iconFile.c_str(), 0, size, size, &fallback, nullptr,
                                     1, 0) == 1 &&
                fallback) {
                *needDestroy = true;
                return fallback;
            }
        }
    }

    // Short timeout plus SMTO_ABORTIFHUNG: a hung window must not stall the first
    // frame of the animation.
    DWORD_PTR res = 0;
    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG, 30, &res) &&
        res) {
        return (HICON)res;
    }
    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG, 30, &res) &&
        res) {
        return (HICON)res;
    }
    HICON icon = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
    if (icon) return icon;
    icon = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);
    if (icon) return icon;
    if (!g_thisExePath.empty()) {
        HICON large = nullptr;
        if (ExtractIconExW(g_thisExePath.c_str(), 0, &large, nullptr, 1) > 0 && large) {
            *needDestroy = true;
            return large;
        }
    }
    return nullptr;
}

// Repaint the panel at the current frame's actual size.
//
// It has to be repainted each frame: UpdateLayeredWindow does not scale, it clips
// when the source size differs from the window size, so the surface handed in must
// match the window exactly.
//
// The panel only ever grows, so what actually changes per frame is the new ring,
// last frame's icon sprite and the corners that fell inside the panel; every other
// pixel is already correct. Total fill over the animation therefore drops from
// "frames times average area" to "exactly the final area", saving the most
// expensive frames (near full size, about 2M pixels per frame).
static void RenderPanelFrame(AnimSlot* slot, int fw, int fh, int radius) {
    if (!slot->splashBits || fw <= 0 || fh <= 0) return;

    uint32_t* bits = slot->splashBits;
    const int stride = slot->spriteW;  // the stride is the final size, not the current frame size
    const int spriteH = slot->spriteH;
    const uint32_t bg = slot->bg;
    const int bgR = (int)((bg >> 16) & 0xFF);
    const int bgG = (int)((bg >> 8) & 0xFF);
    const int bgB = (int)(bg & 0xFF);

    radius = ClampInt(radius, 0, (fh < fw ? fh : fw) / 2);

    const int oldW = slot->drawnW;
    const int oldH = slot->drawnH;
    const int oldRadius = slot->drawnRadius;

    if (oldW <= 0 || fw < oldW || fh < oldH || bg != slot->drawnBg) {
        // First frame, size regression, or a background change. Missing the background
        // case repaints only the corners in the new colour while the interior keeps the
        // old one, which shows up as "the four corners turn white" (seen on CPU-Z).
        FillPixels(bits, stride, fw, fh, bg);
        slot->fillPixels += (unsigned long long)fw * fh;
        slot->drawnIconSize = 0;
    } else {
        // 1. Erase the previous frame's icon: it is centred, so a size change moves it
        if (slot->drawnIconSize > 0) {
            const RECT oi = slot->drawnIcon;
            FillArea(bits, stride, spriteH, oi.left, oi.top, oi.right, oi.bottom, bg);
            slot->fillPixels += (unsigned long long)(oi.right - oi.left) *
                                (unsigned long long)(oi.bottom - oi.top);
        }
        // 2. Erase the previous frame's corners: they now fall inside the panel and must be filled solid
        if (oldRadius > 0) {
            const int r = oldRadius + 1;
            FillArea(bits, stride, spriteH, 0, 0, r, r, bg);
            FillArea(bits, stride, spriteH, oldW - r, 0, oldW, r, bg);
            FillArea(bits, stride, spriteH, 0, oldH - r, r, oldH, bg);
            FillArea(bits, stride, spriteH, oldW - r, oldH - r, oldW, oldH, bg);
            slot->fillPixels += (unsigned long long)r * r * 4;
        }
        // 3. Fill only the new ring: the right strip up to the old height plus the bottom strip across the new width
        if (fw > oldW) {
            FillArea(bits, stride, spriteH, oldW, 0, fw, oldH, bg);
            slot->fillPixels += (unsigned long long)(fw - oldW) * oldH;
        }
        if (fh > oldH) {
            FillArea(bits, stride, spriteH, 0, oldH, fw, fh, bg);
            slot->fillPixels += (unsigned long long)fw * (fh - oldH);
        }
    }

    DrawRoundedCorners(bits, stride, fw, fh, radius, 255, bgR, bgG, bgB);
    if (radius > 0) {
        PremultiplyRect(bits, stride, fw, fh, (RECT){0, 0, radius + 1, radius + 1});
        PremultiplyRect(bits, stride, fw, fh,
                        (RECT){fw - radius - 1, 0, fw, radius + 1});
        PremultiplyRect(bits, stride, fw, fh,
                        (RECT){0, fh - radius - 1, radius + 1, fh});
        PremultiplyRect(bits, stride, fw, fh,
                        (RECT){fw - radius - 1, fh - radius - 1, fw, fh});
    }

    slot->drawnIconSize = 0;
    if (slot->icon) {
        // The icon keeps its size throughout, as it does on a phone, shrinking only when the frame is too small for it.
        const int fit = (fh < fw ? fh : fw) * 2 / 3;
        int iconSize = slot->iconSizePx;
        if (iconSize > fit) iconSize = fit;
        iconSize = ClampInt(iconSize, 8, 4096);
        if (iconSize > 8) {
            const int ix = (fw - iconSize) / 2;
            const int iy = (fh - iconSize) / 2;
            // GDI's default stretch mode is COLORONCOLOR - nearest neighbour, which
            // is what turned a 32px icon scaled to 64px into hard blocks. HALFTONE
            // interpolates, and the SetBrushOrgEx after it is what MSDN requires for
            // HALFTONE to be applied correctly.
            const int oldStretch = SetStretchBltMode(slot->splashDC, HALFTONE);
            SetBrushOrgEx(slot->splashDC, 0, 0, nullptr);
            DrawIconEx(slot->splashDC, ix, iy, slot->icon, iconSize, iconSize, 0,
                       nullptr, DI_NORMAL);
            SetStretchBltMode(slot->splashDC, oldStretch);
            const RECT ir = {ix - 1, iy - 1, ix + iconSize + 1, iy + iconSize + 1};
            PremultiplyRect(bits, stride, fw, fh, ir);
            slot->drawnIcon = ir;           // this is the rect the next frame must erase the sprite from
            slot->drawnIconSize = iconSize;
        }
    }

    
    slot->drawnW = fw;
    slot->drawnH = fh;
    slot->drawnRadius = radius;
    slot->drawnBg = bg;
}

// Splash panel: window

static LRESULT CALLBACK SplashWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                      LPARAM lParam) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            // All content comes from UpdateLayeredWindow; this only validates the region
            ValidateRect(hwnd, nullptr);
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// The window class belongs to the mod's own module instance, not to the host exe.
//
// Registering it under the host's hInstance is a crash waiting for the second load: a
// window class is not unregistered when a DLL is unloaded, so a class whose window
// procedure points into this DLL outlives it, and the next CreateWindowEx with that
// name jumps into unmapped memory. There is also no reusing an existing class of that
// name - if it exists, it is exactly that stale one.
static HINSTANCE g_modInstance = nullptr;
static bool g_splashClassRegistered = false;

static bool RegisterSplashClass() {
    if (g_splashClassRegistered) return true;
    if (!g_modInstance) {
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)&SplashWndProc, (HMODULE*)&g_modInstance);
    }
    if (!g_modInstance) return false;

    WNDCLASSEXW wc = {sizeof(WNDCLASSEXW)};
    wc.lpfnWndProc = SplashWndProc;
    wc.hInstance = g_modInstance;
    wc.lpszClassName = kSplashClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    g_splashClassRegistered = RegisterClassExW(&wc) != 0;
    return g_splashClassRegistered;
}

static void DestroySplash(AnimSlot* slot) {
    if (slot->splash) {
        DestroyWindow(slot->splash);
        slot->splash = nullptr;
    }
    if (slot->splashDC) {
        if (slot->splashOldBitmap) SelectObject(slot->splashDC, slot->splashOldBitmap);
        DeleteDC(slot->splashDC);
        slot->splashDC = nullptr;
        slot->splashOldBitmap = nullptr;
    }
    if (slot->splashBitmap) {
        DeleteObject(slot->splashBitmap);
        slot->splashBitmap = nullptr;
    }
    if (slot->icon && slot->ownIcon) DestroyIcon(slot->icon);
    slot->icon = nullptr;
    slot->ownIcon = false;
    slot->splashBits = nullptr;
    slot->spriteW = slot->spriteH = 0;
    slot->lastRenderW = slot->lastRenderH = slot->lastRenderRadius = -1;
    slot->drawnW = slot->drawnH = slot->drawnRadius = 0;
    slot->drawnIconSize = 0;
    slot->drawnBg = 0;
    slot->fillPixels = 0;
    slot->fullPixels = 0;
}

static bool CreateSplash(AnimSlot* slot, HWND owner) {
    const RECT rc = SnapshotTarget(slot);
    const int w = rc.right - rc.left;
    const int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return false;
    if ((long long)w * h > kMaxPanelPixels) {
        Wh_Log(L"  splash: panel too big %dx%d, using real-window path", w, h);
        return false;
    }
    // Registration happens in Wh_ModInit. Reading the flag here rather than calling
    // RegisterSplashClass() again keeps animation threads from racing on the registration.
    if (!g_splashClassRegistered) {
        Wh_Log(L"  splash: window class is not registered, using the real-window path");
        return false;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC screenDC = GetDC(nullptr);
    if (!screenDC) return false;
    void* bits = nullptr;
    slot->splashBitmap =
        CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    slot->splashDC = CreateCompatibleDC(screenDC);
    ReleaseDC(nullptr, screenDC);
    if (!slot->splashBitmap || !slot->splashDC || !bits) {
        Wh_Log(L"  splash: CreateDIBSection/DC failed (%lu)", GetLastError());
        DestroySplash(slot);
        return false;
    }

    slot->splashBits = (uint32_t*)bits;
    slot->splashOldBitmap = SelectObject(slot->splashDC, slot->splashBitmap);
    slot->spriteW = w;
    slot->spriteH = h;
    slot->bg = ResolvePanelColor(owner, slot->params.splashBgMode);

    UINT dpi = GetDpiForWindow(owner);
    if (dpi < 48 || dpi > 480) dpi = 96;
    slot->iconSizePx = MulDiv(64, (int)dpi, 96);
    slot->radiusPx = MulDiv(8, (int)dpi, 96);
    slot->icon = AcquireAppIcon(owner, slot->iconSizePx, &slot->ownIcon);

    const RECT start = StartRect(SnapshotOrigin(slot), slot->params.startSizePx);

    g_inHook = true;
    slot->splash = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        kSplashClass, L"", WS_POPUP, start.left, start.top,
        start.right - start.left, start.bottom - start.top,
        // nullptr here: the panel must NOT be owned by the target window. An owned window
        // inherits its owner's DWM cloaked state (MSDN DWM_CLOAKED_INHERITED, "the cloaked
        // value is inherited from its owner window"), and cloaking is exactly how the
        // target window is hidden, so the panel would hide along with it and the user
        // would see nothing.
        nullptr, nullptr, g_modInstance, nullptr);
    g_inHook = false;

    if (slot->splash) {
        // Topmost is required: many apps put up their own topmost splash at start-up
        // (AutoCAD's AdSplashWindowClass), which would bury a non-topmost panel. The
        // animation would run underneath and only appear once that splash closes, looking
        // like "the animation shows up after the window refreshes".
        // The panel carries WS_EX_TOOLWINDOW, so it stays out of the taskbar and Alt+Tab and never takes focus.
        g_internalMove = true;
        SetWindowPos(slot->splash, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        g_internalMove = false;
    }

    if (!slot->splash) {
        Wh_Log(L"  splash: CreateWindowEx failed (%lu)", GetLastError());
        DestroySplash(slot);
        return false;
    }

    
    Wh_Log(L"  splash: created start %dx%d at (%d,%d) -> target %dx%d at (%d,%d), bg=%08X, icon=%S",
         start.right - start.left, start.bottom - start.top, start.left, start.top, w, h,
         (int)slot->target.left, (int)slot->target.top, slot->bg,
         slot->icon ? "yes" : "NO");
    return true;
}


static void ApplySplashFrame(AnimSlot* slot, const RECT& r, BYTE alpha, int radius) {
    if (!slot->splash || !slot->splashDC) return;

    int fw = (int)(r.right - r.left);
    int fh = (int)(r.bottom - r.top);
    if (fw < 1 || fh < 1) return;
    if (fw > slot->spriteW) fw = slot->spriteW;
    if (fh > slot->spriteH) fh = slot->spriteH;

    // Content is repainted only when size, radius or background change; alpha travels
    // in the blend function and needs no bitmap work. During the fade the size is
    // constant, so nothing is repainted at all and only alpha changes.
    if (fw != slot->lastRenderW || fh != slot->lastRenderH ||
        radius != slot->lastRenderRadius || slot->bg != slot->lastRenderBg) {
        slot->fullPixels += (unsigned long long)fw * fh;  // for comparison: the cost of a full repaint
        RenderPanelFrame(slot, fw, fh, radius);
        slot->lastRenderW = fw;
        slot->lastRenderH = fh;
        slot->lastRenderRadius = radius;
        slot->lastRenderBg = slot->bg;
    }

    // Raising must NOT happen every frame: SetWindowPos is synchronous, so a busy app
    // (Outlook putting up its own dialog during the handover) blocks it, the animation
    // thread never exits, the splash window keeps the DLL's window procedure alive,
    // the engine cannot unload and the mod hangs in Uninitializing.
    //
    // While zooming the z-order does have to be fought for every frame; during the
    // handover the size is constant and UpdateLayeredWindow raises the window itself.
    // The caller therefore states explicitly whether a frame needs raising.
    if (slot->raiseEachFrame) {
        g_internalMove = true;
        SetWindowPos(slot->splash, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        g_internalMove = false;
    }

    HDC screenDC = GetDC(nullptr);
    if (!screenDC) return;
    POINT dst = {r.left, r.top};
    POINT src = {0, 0};
    SIZE size = {fw, fh};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    if (!UpdateLayeredWindow(slot->splash, screenDC, &dst, &size, slot->splashDC,
                             &src, 0, &blend, ULW_ALPHA)) {
        if (++slot->ulwFailures <= 3) {
            Wh_Log(L"  splash: UpdateLayeredWindow failed (%lu) size=%dx%d", GetLastError(),
                 fw, fh);
        }
    }
    ReleaseDC(nullptr, screenDC);
}

// Real window

static void SetRealAlpha(HWND hwnd, AnimSlot* slot, BYTE alpha) {
    if (slot->lastRealAlpha == (int)alpha) return;
    SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
    slot->lastRealAlpha = (int)alpha;
}

static void ApplyRealFrame(HWND hwnd, AnimSlot* slot, const RECT& r, BYTE alpha) {
    g_internalMove = true;
    SetRealAlpha(hwnd, slot, alpha);
    CallRealSetWindowPos(hwnd, nullptr, r.left, r.top, r.right - r.left,
                         r.bottom - r.top,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING |
                             SWP_ASYNCWINDOWPOS);
    g_internalMove = false;
}

// Hide the window before it is shown. Two mechanisms:
//   useCloak=true  (splash path): DWM cloaking. No style changes, works on
//                  composited windows, and no flicker when it is lifted.
//   useCloak=false (real-window fallback): WS_EX_LAYERED + alpha=0, because that
//                  path shows the real window itself growing, which needs alpha
//                  rather than a full hide.
static void HideForAnimation(HWND hwnd, AnimSlot* slot, bool useCloak) {
    slot->cloakedByUs = false;
    slot->wasLayered = false;
    slot->addedLayered = false;

    if (useCloak) {
        // A window the app has already cloaked itself (Chromium waits for its first frame
        // this way) is left alone: the app decides when to lift it. We only cloak windows
        // that were not cloaked to begin with.
        if (!IsCloaked(hwnd) && SetCloak(hwnd, true)) slot->cloakedByUs = true;
        return;
    }

    const LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    slot->wasLayered = (exStyle & WS_EX_LAYERED) != 0;
    if (slot->wasLayered) {
        COLORREF key = 0;
        DWORD flags = 0;
        if (!GetLayeredWindowAttributes(hwnd, &key, &slot->originalAlpha, &flags))
            slot->originalAlpha = 255;
        SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
        return;
    }
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    slot->addedLayered = true;
    SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
}

// Remove the layered style we added. Removing WS_EX_LAYERED from a visible window
// makes DWM recreate the window surface and flash for one frame, so this must run
// while the panel still covers the window completely. The SetWindowPos here is
// synchronous, so the recreation finishes before the call returns rather than
// flashing after the panel has faded out.
static void RestoreWindowStyle(AnimSlot* slot, HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    // Lift our DWM cloaking first. This is instantaneous, recreates no surface, and
    // the panel still covers the window, so nothing is visible to the user.
    if (slot->cloakedByUs) {
        SetCloak(hwnd, false);
        slot->cloakedByUs = false;
    }

    g_internalMove = true;
    if (slot->wasLayered) {
        SetLayeredWindowAttributes(hwnd, 0, slot->originalAlpha, LWA_ALPHA);
    } else if (slot->addedLayered) {
        // The alpha is restored first and unconditionally. Every exit from the real-window
        // path can leave the window at alpha 0 - the app hides it during the readiness wait,
        // StartAnimation fails after the alpha was set, an unload arrives mid-animation -
        // and it would stay invisible the next time the app shows it.
        // SetLayeredWindowAttributes sends nothing to the window's thread, so this cannot
        // block and does not need the pumping check.
        // The check is for this being the second call (the tail of the animation and the
        // unload cleanup both land here): a window that no longer has the style would just
        // fail the call, and one the app layered again in between must keep its own alpha.
        // No style also means no alpha, so a window that is not layered is visible anyway.
        if (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED) {
            SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        }
        // Only the style removal is gated: SetWindowLong on another thread's window is a
        // synchronous send, and Wh_ModUninit waits for this thread with INFINITE, so a hung
        // app would hold up the unload. Keeping WS_EX_LAYERED is harmless there, since the
        // alpha above already made the window visible again.
        // It is also only the style we added: "remove WS_EX_LAYERED if present" would remove
        // the layering WinUI, WPF and Office set themselves, and removing layering always
        // makes DWM recreate the surface, which is one frame of flicker.
        if (IsThreadPumping(hwnd) && (GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED)) {
            const LONG cur = GetWindowLong(hwnd, GWL_EXSTYLE);
            SetWindowLong(hwnd, GWL_EXSTYLE, cur & ~WS_EX_LAYERED);
            CallRealSetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                                     SWP_NOACTIVATE | SWP_FRAMECHANGED);
        }
    }
    g_internalMove = false;
    slot->lastRealAlpha = -1;
}

// Deciding that the content is ready

static bool IsThreadPumping(HWND hwnd) {
    DWORD_PTR result = 0;
    return SendMessageTimeout(hwnd, WM_NULL, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 30,
                              &result) != 0;
}

// The primary signal is EndPaint, whose meaning is "this window has finished
// painting" (it clears the update region). An empty GetUpdateRect must not be used
// as the signal instead, because a just-shown window can have an empty update
// region anyway.
//
// GetUpdateRect reports the PENDING region, which empties once painting is done,
// so for DXGI/DComp windows an empty rectangle means the area has been painted,
// not that painting has not started. The DWM cloaked state and the window's own
// visibility are considered alongside it.
static bool HasPendingPaint(HWND hwnd) {
    if (!IsWindow(hwnd)) return false;
    RECT rc = {};
    // A non-empty update region means painting is still pending
    if (GetUpdateRect(hwnd, &rc, FALSE) && !IsRectEmpty(&rc)) return true;
    return false;
}

// Readiness test. CAD puts up an empty window and paints a second later, so EndPaint alone is not enough.
static bool IsWindowReady(AnimSlot* slot, HWND hwnd, ULONGLONG showTick) {
    if (!IsWindow(hwnd)) return true;  // the window is gone, there is nothing to reveal

    const ULONGLONG elapsed = GetTickCount64() - showTick;
    const bool timedOut = elapsed >= (ULONGLONG)slot->params.readyTimeoutMs;

    // The app hid the window half-way through (MFC apps hide and re-show during
    // start-up). That must NOT count as ready: the panel would be pulled off, the
    // desktop would show through, and the app reappearing would look like a flash.
    // The panel stays put until the window comes back, up to readyMs.
    if (!IsWindowVisible(hwnd)) return timedOut;

    // The app is still DWM-cloaking it (Chromium waits for its first frame this way),
    // so keep waiting with the panel up until the app uncloaks it. If the cloaking is
    // OURS it must never be waited on: the app does not know about it and will never
    // lift it, so the wait would run to the timeout and look like a hang.
    if (!slot->cloakedByUs && IsCloaked(hwnd)) return timedOut;

    if (slot->painted.load(std::memory_order_relaxed)) return true;
    if (timedOut) return true;
    // Apps that never call EndPaint (DComp and D3D ones; CAD renders through DXGI) can
    // only be waited out. 700ms is the measured lower bound, but CAD's "empty window
    // first, real painting a second later" is misjudged as ready at that point, the
    // panel is pulled off over a blank surface and CAD's repaint only arrives seconds
    // later.
    //
    // So the wait is staged: at 700ms a window with no pending update region and
    // nothing painted yet keeps waiting, up to 2s. The animation itself has finished
    // by then, so waiting only keeps the panel up a little longer.
    if (elapsed >= 700) {
        if (slot->painted.load(std::memory_order_relaxed)) return true;
        if (elapsed >= kReadyNoPaintMs) return true;
        if (IsThreadPumping(hwnd) && !HasPendingPaint(hwnd)) return true;
        return false;
    }
    return false;
}

// Animation thread

static void PumpMessages() {
    MSG msg;
    int guard = 0;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE) && guard++ < 64) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

// The panel's zoom animation. The time base t0 has to be taken here: thread
// creation, class registration, bitmap allocation and icon extraction all sit
// between ShowWindow and the first drawable frame, so using showTick as the base
// would skip the whole animation.
static void RunSplashZoom(AnimSlot* slot, HWND hwnd) {
    const ULONGLONG t0 = QpcNow();  // must be high resolution: GetTickCount64 only has 15.6ms granularity
    const DWORD duration = (DWORD)slot->params.durationMs;
    const float fadeEnd = (float)slot->params.fadeInPercent / 100.f;
    const POINT anchor = SnapshotOrigin(slot);
    const RECT startRect = StartRect(anchor, slot->params.startSizePx);
    const int startRadius = slot->params.startSizePx * 22 / 100;
    int frames = 0;
    slot->raiseEachFrame = true;  // during the zoom, fight the app's own splash screen for z-order

    for (;;) {
        const ULONGLONG frameStart = QpcNow();
        const float elapsedMs = QpcMs(t0, frameStart);
        float t = duration ? elapsedMs / (float)duration : 1.f;
        const bool done = (t >= 1.f);
        if (t > 1.f) t = 1.f;

        const FrameVals v = EvalCurves(slot->params.easing, t, fadeEnd);
        const RECT frame = LerpRect(startRect, SnapshotTarget(slot), v.ease);
        const int radius = LerpInt(startRadius, slot->radiusPx, v.ease);

        ApplySplashFrame(slot, frame, v.alpha, radius);
        frames++;
        PumpMessages();

        if (!IsWindow(slot->splash) || !IsWindow(hwnd)) break;
        if (done || g_unloading.load(std::memory_order_relaxed)) break;

        const float spentMs = QpcSinceMs(frameStart);
        const float interval = (float)slot->frameIntervalMs;
        Sleep(spentMs + 0.5f < interval ? (DWORD)lround(interval - spentMs) : 1);
    }

    const float totalMs = QpcSinceMs(t0);
    Wh_Log(L"  zoom done: %d frames in %.1f ms (%.1f fps, target %.1f ms/frame); "
         L"filled %.2fM pixels (full repaints would cost %.2fM, saving %.0f%%)",
         frames, totalMs, totalMs > 0.1f ? frames * 1000.0f / totalMs : 0.0f,
         (double)slot->frameIntervalMs, (double)slot->fillPixels / 1e6,
         (double)slot->fullPixels / 1e6,
         slot->fullPixels ? 100.0 - 100.0 * (double)slot->fillPixels / (double)slot->fullPixels
                          : 0.0);
}

// The panel rests at the final rectangle while the real window paints
static void RunWaitReady(AnimSlot* slot, HWND hwnd, ULONGLONG showTick) {
    RECT applied = SnapshotTarget(slot);
    const ULONGLONG t0 = GetTickCount64();

    // The app has just been activated and the z-order moved. The panel has no owner,
    // so it has to be raised again or it can end up below the target window, which is
    // invisible but still occupies its place in the z-order.
    if (slot->splash && IsWindow(slot->splash)) {
        SetWindowPos(slot->splash, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    slot->raiseEachFrame = false;  // the rect is settled, all that is left is waiting; stop fighting for z-order

    while (!IsWindowReady(slot, hwnd, showTick) &&
           !g_unloading.load(std::memory_order_relaxed)) {
        if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) break;
        const RECT target = SnapshotTarget(slot);
        if (!EqualRect(&target, &applied)) {
            ApplySplashFrame(slot, target, 255, slot->radiusPx);
            applied = target;
        }
        PumpMessages();
        Sleep(15);
    }
    Wh_Log(L"  ready after %llu ms (painted=%d cloaked=%d)",
         (unsigned long long)(GetTickCount64() - t0), slot->painted.load() ? 1 : 0,
         IsCloaked(hwnd) ? 1 : 0);
}

// Fade the panel out to reveal the real window. bgTo is the app's real background
// colour, 0 meaning the sample failed and the colour stays put.
//
// The background must be interpolated together with alpha. Swapping to the sampled
// colour in a frame before starting the fade left that frame at alpha 255, so the
// whole panel changed colour at once, which is the flash users saw.
static void RunFadeOutPanel(AnimSlot* slot, uint32_t bgTo) {
    const RECT target = SnapshotTarget(slot);
    const DWORD duration = (DWORD)slot->params.handoffMs;
    const ULONGLONG t0 = QpcNow();
    const uint32_t bgFrom = slot->bg;
    const bool lerpBg = (bgTo != 0) && (bgTo != bgFrom);
    // The handover is a finishing step and needs a wall-clock ceiling. A busy app
    // (Outlook putting up its own #32770 dialog during the handover, measured) blows
    // every ApplySplashFrame frame, and following the timeline alone would never exit:
    // the thread does not return, the splash window stays alive, the engine cannot
    // unload the DLL and the mod hangs in Uninitializing. On timeout it breaks
    // straight out, and the tail still runs DestroySplash and ReleaseSlot.
    const ULONGLONG wall = GetTickCount64() + kHandoffWallClockMs;
    bool outOfTime = false;

    for (;;) {
        const ULONGLONG frameStart = QpcNow();
        float t = duration ? QpcMs(t0, frameStart) / (float)duration : 1.f;
        const bool done = (t >= 1.f);
        if (t > 1.f) t = 1.f;

        const float e = SmoothStep(t);
        if (lerpBg) slot->bg = LerpPixel(bgFrom, bgTo, (double)e);
        ApplySplashFrame(slot, target, (BYTE)lround(255.0 * (1.f - e)),
                         slot->radiusPx);
        PumpMessages();
        if (done || g_unloading.load(std::memory_order_relaxed)) break;
        if (GetTickCount64() >= wall) {
            outOfTime = true;
            break;
        }

        const float spentMs = QpcSinceMs(frameStart);
        const float interval = (float)slot->frameIntervalMs;
        Sleep(spentMs + 0.5f < interval ? (DWORD)lround(interval - spentMs) : 1);
    }
    if (outOfTime) {
        Wh_Log(L"  !! handoff fade still running after %llu ms, forcing the teardown",
             (unsigned long long)kHandoffWallClockMs);
    }
}

// Fallback path with splash disabled: animate the real window itself
static void RunRealZoom(AnimSlot* slot, HWND hwnd, ULONGLONG showTick) {
    while (!IsWindowReady(slot, hwnd, showTick) &&
           !g_unloading.load(std::memory_order_relaxed)) {
        if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
            // StartAnimation has already put the window at the first animation frame, and
            // this path only puts it back when the zoom runs to completion. Returning here
            // would leave it shrunk - and apps save their window rect on exit, so the wrong
            // size can stick. ApplyRealFrame is asynchronous, so this cannot block.
            ApplyRealFrame(hwnd, slot, SnapshotTarget(slot), 255);
            return;
        }
        Sleep(15);
    }

    
    const ULONGLONG t0 = QpcNow();
    const DWORD duration = (DWORD)slot->params.durationMs;
    // The fallback path must not really shrink the window to icon size, which would
    // force the app to re-layout at a few dozen pixels, so it uses a gentle 70% scale.
    const float scaleFrom = 0.70f;
    const float fadeEnd = (float)slot->params.fadeInPercent / 100.f;
    // This path changes the real window's geometry every frame, one WM_SIZE plus an
    // app re-layout, which is far heavier than the panel, so it is capped at 60fps
    // even on a 144Hz display.
    const float intervalMs = (float)(slot->frameIntervalMs > 16 ? slot->frameIntervalMs : 16);

    for (;;) {
        const ULONGLONG frameStart = QpcNow();
        float t = duration ? QpcMs(t0, frameStart) / (float)duration : 1.f;
        const bool done = (t >= 1.f);
        if (t > 1.f) t = 1.f;

        const FrameVals v = EvalCurves(slot->params.easing, t, fadeEnd);
        const float scale = scaleFrom + (1.f - scaleFrom) * v.ease;
        const RECT frame =
            ScaleAbout(SnapshotTarget(slot), SnapshotOrigin(slot), scale);
        ApplyRealFrame(hwnd, slot, frame, done ? 255 : v.alpha);

        if (!IsWindow(hwnd)) return;
        if (done || g_unloading.load(std::memory_order_relaxed)) break;

        const float spentMs = QpcSinceMs(frameStart);
        Sleep(spentMs + 0.5f < intervalMs ? (DWORD)lround(intervalMs - spentMs) : 1);
    }

    // An unload breaks out of the loop before the last frame, which would leave the window
    // at a partial frame. Setting the full rect again is a no-op when the zoom did finish.
    if (IsWindow(hwnd)) ApplyRealFrame(hwnd, slot, SnapshotTarget(slot), 255);
}

// The body of the animation. The finishing steps (destroy the panel, restore the
// styles, release the slot) are placed at the end of this function rather than
// wrapped in SEH: clang rejects __try without -fms-extensions, and
// @compilerOptions cannot add compile flags. The guarantee is structural instead:
// there is no early return in RunAnimationBody, every exit funnels into the three
// closing calls, and RestoreWindowStyle is idempotent. Missing them leaves the
// window permanently cloaked or at alpha 0, as if the application window had
// vanished.
static void RunAnimationBody(AnimSlot* slot, HWND hwnd) {
    const ULONGLONG showTick = slot->showTick;
    const ULONGLONG createStart = GetTickCount64();
    bool splashOk = slot->params.splash && CreateSplash(slot, hwnd);
    const ULONGLONG createCost = GetTickCount64() - createStart;
    if (createCost > 5) Wh_Log(L"  splash create cost %llu ms", (unsigned long long)createCost);

    if (splashOk) {
        const RECT start = StartRect(SnapshotOrigin(slot), slot->params.startSizePx);
        ApplySplashFrame(slot, start, 0, slot->params.startSizePx * 22 / 100);
        g_inHook = true;
        ShowWindow(slot->splash, SW_SHOWNOACTIVATE);
        g_inHook = false;

        RunSplashZoom(slot, hwnd);

        bool handedOff = false;
        if (IsWindow(hwnd) && IsWindowVisible(hwnd) &&
            !g_unloading.load(std::memory_order_relaxed)) {
            handedOff = true;
            RunWaitReady(slot, hwnd, showTick);
            // The sampled colour is only the end point of the fade, never applied before it starts, while alpha is still 255
            uint32_t bgTo = 0;
            const COLORREF sampled = SampleWindowBackground(hwnd);
            if (sampled != 0xFFFFFFFF) {
                bgTo = MakePixel(255, GetRValue(sampled), GetGValue(sampled),
                                 GetBValue(sampled));
                Wh_Log(L"  handoff colour %08X -> %08X (fade %llu ms)", slot->bg, bgTo,
                     (unsigned long long)slot->params.handoffMs);
            } else {
                Wh_Log(L"  handoff colour sampling failed, keeping the panel colour %08X", slot->bg);
            }
            RestoreWindowStyle(slot, hwnd);
            RunFadeOutPanel(slot, bgTo);
        } else {
            // The app hid or destroyed the window: the styles still have to be restored
            RestoreWindowStyle(slot, hwnd);
        }

        // Failure criterion: the window is still not visible after the panel is gone. By
        // then the user has already seen "the program opened and there is nothing", and a
        // third attempt would look the same, so this process is cut off.
        if (handedOff && !g_unloading.load(std::memory_order_relaxed)) {
            if (IsWindow(hwnd) && !IsWindowVisible(hwnd)) {
                const int n = g_handoffFailStreak.fetch_add(1, std::memory_order_relaxed) + 1;
                Wh_Log(L"  !! window still not visible after handoff, %d in a row", n);
                if (n >= kHandoffFailLimit) {
                    g_compatDisabled.store(true, std::memory_order_relaxed);
                    Wh_Log(L"  !!! cutting this process off, no more animations here");
                }
            } else {
                g_handoffFailStreak.store(0, std::memory_order_relaxed);
            }
        }
    } else if (slot->params.splash) {
        // The panel could not be created: the class registration failed, or the window is
        // above kMaxPanelPixels. The window is cloaked from the show, and the fallback zoom
        // is invisible through a cloak - the alpha calls do nothing on a window that is not
        // layered - so reveal it right away instead of animating it. The tail of this
        // function restores the style and lifts the cloak.
        Wh_Log(L"  no panel and the window is cloaked, showing it without animation");
    } else {
        RunRealZoom(slot, hwnd, showTick);
    }

    DestroySplash(slot);
    RestoreWindowStyle(slot, hwnd);
    ReleaseSlot(slot);
    Wh_Log(L"  done [%s] total %llu ms", g_thisExeName.c_str(),
         (unsigned long long)(GetTickCount64() - showTick));
}

static DWORD WINAPI AnimationThread(LPVOID param) {
    const int index = (int)(INT_PTR)param;
    AnimSlot* slot = &g_slots[index];
    const HWND hwnd = slot->hwnd.load();

    // Our thread must use the same DPI coordinate semantics as the real window. On a
    // scaled display (125%/150%) a thread whose DPI awareness differs from the
    // window's computes a panel position and size that do not line up with it, which
    // shows up as misalignment.
    DPI_AWARENESS_CONTEXT prevDpi = nullptr;
    if (IsWindow(hwnd)) {
        prevDpi = SetThreadDpiAwarenessContext(GetWindowDpiAwarenessContext(hwnd));
    }

    // Windows' default timer resolution is 15.6ms, so without raising it Sleep(8) really
    // sleeps 15.6ms and frames jitter between 16 and 31ms. Requested per animation
    // thread and not for the process lifetime: the mod is loaded into a hundred
    // processes, and the system timer runs at the highest resolution anyone asks for.
    timeBeginPeriod(1);
    RunAnimationBody(slot, hwnd);
    timeEndPeriod(1);

    if (prevDpi) SetThreadDpiAwarenessContext(prevDpi);
    return 0;
}

// Start-up

static bool StartAnimation(int index, HWND hwnd) {
    AnimSlot* slot = &g_slots[index];
    const AnimParams& p = slot->params;

    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) return false;

    // The maximized check only matters on the fallback path (see ShouldAnimate)
    if (!p.splash) {
        WINDOWPLACEMENT wp = {sizeof(WINDOWPLACEMENT)};
        if (GetWindowPlacement(hwnd, &wp) && wp.showCmd == SW_SHOWMAXIMIZED)
            return false;
        if (GetWindowLong(hwnd, GWL_STYLE) & WS_MAXIMIZE) return false;
    }

    RECT rc = {};
    if (!GetWindowRect(hwnd, &rc)) return false;
    if ((rc.right - rc.left) < 160 || (rc.bottom - rc.top) < 100) return false;

    slot->target = rc;
    // Windows shown shortly after start-up all count as part of this launch and use
    // the mouse position recorded at start-up, when the hand still was on the icon.
    const bool useLaunchAnchor =
        g_launchAnchorValid && (GetTickCount64() - g_initTick) < kLaunchAnchorMs;
    const char* originSource = "?";
    slot->origin = ResolveOrigin(rc, p.originMode, useLaunchAnchor, p.shellUiAnchor,
                                 &originSource);
    slot->lastRealAlpha = -1;
    slot->painted.store(false, std::memory_order_relaxed);
    slot->ulwFailures = 0;
    MarkAnimated(hwnd);

    // Frame interval: 0 follows the refresh rate (about 8.3ms per frame at 120Hz, 6.9 at 144Hz)
    slot->displayPeriodMs = GetDisplayPeriodMs(hwnd);
    if (p.frameIntervalMs > 0) {
        slot->frameIntervalMs = (DWORD)p.frameIntervalMs;
    } else {
        slot->frameIntervalMs =
            (DWORD)ClampInt((int)lround(slot->displayPeriodMs), 2, 50);
    }

    // Splash mode never touches the real window's geometry; it only has to stay fully transparent
    if (!p.splash) {
        ApplyRealFrame(hwnd, slot, ScaleAbout(rc, slot->origin, 0.70f), 0);
    }

    HANDLE thread =
        CreateThread(nullptr, 0, AnimationThread, (LPVOID)(INT_PTR)index, 0, nullptr);
    if (!thread) return false;
    RegisterModThread(thread);

    wchar_t clsName[128] = L"";
    GetClassNameW(hwnd, clsName, 128);
    Wh_Log(L"  accepted class=%s %dx%d at (%d,%d) anchor=(%d,%d) source=%S frame=%.1fms(%.0ffps) "
         L"display=%.1fms",
         clsName, rc.right - rc.left, rc.bottom - rc.top, rc.left, rc.top,
         slot->origin.x, slot->origin.y, originSource, (double)slot->frameIntervalMs,
         1000.0 / (double)slot->frameIntervalMs, (double)slot->displayPeriodMs);
    return true;
}

// Shared by the three show paths (ShowWindow, ShowWindowAsync, SetWindowPos) plus
// "created already visible": take over before the window is shown, and decide
// afterwards whether it can be animated.
static void BeginAnimatedShow(HWND hwnd, const AnimParams& p, int index) {
    AnimSlot* slot = &g_slots[index];
    slot->params = p;
    slot->showTick = GetTickCount64();
    HideForAnimation(hwnd, slot, p.splash);  // hide it before showing, so the full-size window never flashes
}

static void FinishAnimatedShow(HWND hwnd, int index) {
    if (!StartAnimation(index, hwnd)) {
        AnimSlot* slot = &g_slots[index];
        DiagSkip("after show", hwnd, "size or maximize condition not met");
        RestoreWindowStyle(slot, hwnd);
        ReleaseSlot(slot);
    }
}

static BOOL ShowAnimated(HWND hwnd, const AnimParams& p, int nCmdShow) {
    if (TooSoon(hwnd)) {
        DiagSkip("repeat show", hwnd, "animated a moment ago");
        g_inHook = true;
        BOOL r0 = pOrigShowWindow(hwnd, nCmdShow);
        g_inHook = false;
        return r0;
    }
    const int index = AllocSlot(hwnd);
    if (index < 0) {
        g_inHook = true;
        BOOL r = pOrigShowWindow(hwnd, nCmdShow);
        g_inHook = false;
        return r;
    }

    BeginAnimatedShow(hwnd, p, index);

    g_inHook = true;
    BOOL result = pOrigShowWindow(hwnd, nCmdShow);
    g_inHook = false;

    FinishAnimatedShow(hwnd, index);
    return result;
}

// Hook: ShowWindow / ShowWindowAsync

static bool IsShowCommand(int nCmdShow) {
    // The test is not "does this nCmdShow count as showing" but "was it hidden before
    // and visible after".
    //
    // A whitelist of five or six commands (SW_SHOW and friends) silently let every
    // other show path through: a maximized window is sometimes shown with SW_RESTORE,
    // which makes it visible immediately, and the later ShowWindow then finds it
    // "already visible" and skips the animation entirely. Only minimise-style commands
    // are excluded here; everything else goes through ShouldAnimateShow.
    switch (nCmdShow) {
        case SW_HIDE:
        case SW_MINIMIZE:
        case SW_SHOWMINIMIZED:
        case SW_SHOWMINNOACTIVE:
        case SW_FORCEMINIMIZE:
            return false;
        default:
            return true;
    }
}

static bool ShouldAnimateShow(HWND hwnd, const AnimParams& p, wchar_t* clsOut,
                              size_t clsCount, const char** reason,
                              bool expectHidden = true) {
    clsOut[0] = L'\0';
    *reason = nullptr;
    // The show hooks fire in the caller's process and the window they are handed can
    // belong to another one: a single-instance app's second launch finds the first
    // instance's hidden main window, calls ShowWindow/ShowWindowAsync on it and exits.
    // Taking that window over hides it in a process that is about to disappear, which
    // leaves it invisible for good. Only windows of this process are ours to animate.
    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hwnd, &ownerPid);
    if (ownerPid != GetCurrentProcessId()) {
        *reason = "window of another process";
        return false;
    }
    // Nothing is taken over while unloading: the panel window class is about to be
    // unregistered and the animation threads are finishing, so starting a new
    // animation would leave an unmanaged window behind.
    if (g_unloading.load(std::memory_order_relaxed)) {
        *reason = "mod is unloading";
        return false;
    }
    if (g_compatDisabled.load(std::memory_order_relaxed)) {
        *reason = "this process is cut off (repeated handoff failures)";
        return false;
    }
    // The ShowWindow path requires the window to still be hidden; the created-already-visible path requires the opposite.
    if (expectHidden) {
        if (IsWindowVisible(hwnd)) {
            *reason = "already visible (this is not a show)";
            return false;
        }
    } else {
        if (!IsWindowVisible(hwnd)) {
            *reason = "created hidden (this is not the show path)";
            return false;
        }
    }
    if (FindSlot(hwnd) >= 0) {
        *reason = "already animating";
        return false;
    }
    GetClassNameW(hwnd, clsOut, (int)clsCount);
    return ShouldAnimate(hwnd, p, clsOut, reason);
}

// Hook: CreateWindowExW, for windows created already visible

// Many modern programs (Chromium/Electron apps such as VS Code, Edge and Chrome,
// plus Office) create their main window with WS_VISIBLE already set, showing it in
// one step and never calling ShowWindow, so hooking ShowWindow alone would never
// see them.
//
// The key point: the window has just been created and painted nothing yet (WM_PAINT
// only arrives once the app returns to its message loop), so setting alpha to 0
// right after CreateWindowExW returns hides it without the user seeing a frame.
static void StartAnimationForCreatedWindow(HWND hwnd, const AnimParams& p) {
    if (TooSoon(hwnd)) return;
    const int index = AllocSlot(hwnd);
    if (index < 0) return;
    BeginAnimatedShow(hwnd, p, index);
    FinishAnimatedShow(hwnd, index);
}

// Judgement and takeover for already-visible windows, shared by the W and A versions
static void HandleWindowCreatedVisible(HWND hwnd, const char* where) {
    const AnimParams p = GetParams();
    wchar_t cls[256];
    const char* reason = nullptr;
    if (ShouldAnimateShow(hwnd, p, cls, 256, &reason, /*expectHidden=*/false)) {
        StartAnimationForCreatedWindow(hwnd, p);
    } else {
        DiagSkip(where, hwnd, reason);
    }
}

HWND WINAPI HookedCreateWindowExW(DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
                                  DWORD style, int x, int y, int w, int h, HWND parent,
                                  HMENU menu, HINSTANCE instance, LPVOID param) {
    if (g_inHook) {
        return pOrigCreateWindowExW(exStyle, className, windowName, style, x, y, w, h,
                                    parent, menu, instance, param);
    }

    // Only top-level windows created already visible are of interest, and !parent must
    // NOT be required: for a top-level window CreateWindowEx's hWndParent is the
    // owner, and many programs (Edge among them) hang their main window off an
    // invisible helper window and create it with WS_VISIBLE in one step. Requiring
    // !parent would silently skip them and the window would appear at full size with
    // no animation. Whether the owner is visible is decided by ShouldAnimate, which
    // rejects only visible owners.
    const bool candidate = (style & WS_VISIBLE) && !(style & WS_CHILD);

    HWND hwnd = pOrigCreateWindowExW(exStyle, className, windowName, style, x, y, w, h,
                                     parent, menu, instance, param);
    if (!hwnd) return hwnd;

    if (candidate) {
        // Reaching this point means "created already visible". The creation path, class
        // name and styles are all logged, because when this case used to be missed the log
        // said only "already visible" and nothing could be diagnosed.
        //
        // className may be an ATOM and must never be used as a string; the class name
        // always comes from the hwnd. Reading the ATOM as a string here is what gave
        // explorer, the Control Panel, Baidu Netdisk and IDA their 0xc0000005 crashes. The
        // window is created by now, and a later misjudgement must never lose it.
        wchar_t clsNow[256] = L"";
        GetClassNameW(hwnd, clsNow, 256);
        Wh_Log(L"  visible on creation: CreateWindowExW class=%s style=%08lX exStyle=%08lX parent=%p owner=%p",
             clsNow[0] ? clsNow : L"(empty)", (unsigned long)style, (unsigned long)exStyle,
             (void*)parent, (void*)GetWindow(hwnd, GW_OWNER));
        HandleWindowCreatedVisible(hwnd, "CreateWindowExW(WS_VISIBLE)");
    } else if (style & WS_VISIBLE) {
        // Subwindows are of no interest and are not logged, there are far too many; a WS_VISIBLE window that is not a candidate gets one line
        DiagSkip("CreateWindowExW(visible, not a candidate)", hwnd, "child window");
    }
    return hwnd;
}

// The ANSI version has to be hooked too: with only the W version hooked, programs
// calling CreateWindowExA (some older tools and wrappers) never reach this path
// and likewise show no animation.
HWND WINAPI HookedCreateWindowExA(DWORD exStyle, LPCSTR className, LPCSTR windowName,
                                  DWORD style, int x, int y, int w, int h, HWND parent,
                                  HMENU menu, HINSTANCE instance, LPVOID param) {
    if (g_inHook) {
        return pOrigCreateWindowExA(exStyle, className, windowName, style, x, y, w, h,
                                    parent, menu, instance, param);
    }
    const bool candidate = (style & WS_VISIBLE) && !(style & WS_CHILD);
    HWND hwnd = pOrigCreateWindowExA(exStyle, className, windowName, style, x, y, w, h,
                                     parent, menu, instance, param);
    if (hwnd && candidate) {
        // Same discipline as the W version: the class name comes only from the hwnd. The
        // caller's className pointer may already be freed or may be an ATOM, and touching
        // it is where those 0xc0000005 crashes came from.
        wchar_t clsNow[256] = L"";
        GetClassNameW(hwnd, clsNow, 256);
        Wh_Log(L"  visible on creation: CreateWindowExA class=%s style=%08lX exStyle=%08lX parent=%p owner=%p",
             clsNow[0] ? clsNow : L"(empty)", (unsigned long)style,
             (unsigned long)exStyle, (void*)parent, (void*)GetWindow(hwnd, GW_OWNER));
        HandleWindowCreatedVisible(hwnd, "CreateWindowExA(WS_VISIBLE)");
    }
    return hwnd;
}

BOOL WINAPI HookedShowWindow(HWND hwnd, int nCmdShow) {
    if (g_inHook) return pOrigShowWindow(hwnd, nCmdShow);
    if (!IsShowCommand(nCmdShow)) return pOrigShowWindow(hwnd, nCmdShow);

    // This hook must never swallow the call: if the judgement gives up, the window
    // still has to be shown. An app that never sees ShowWindow return is left with a
    // window that never appears, which is far worse than no animation. Every early
    // return inside ShowAnimated goes through the same original call, so no path
    // decides without showing.
    const AnimParams p = GetParams();
    wchar_t cls[256];
    const char* reason = nullptr;
    if (!ShouldAnimateShow(hwnd, p, cls, 256, &reason)) {
        DiagSkip("ShowWindow", hwnd, reason);
    } else {
        return ShowAnimated(hwnd, p, nCmdShow);
    }

    g_inHook = true;
    const BOOL passthrough = pOrigShowWindow(hwnd, nCmdShow);
    g_inHook = false;
    return passthrough;
}

// ShowWindowAsync posts the request to the window thread's queue without going
// through ShowWindow, so the window has to be made transparent before it is shown
// and then polled until it really is visible.
static DWORD WINAPI DeferredAnimationThread(LPVOID param) {
    const int index = (int)(INT_PTR)param;

    AnimSlot* slot = &g_slots[index];
    const HWND hwnd = slot->hwnd.load();

    for (int i = 0; i < 400; i++) {
        if (!IsWindow(hwnd)) break;
        if (IsWindowVisible(hwnd)) break;
        if (g_unloading.load(std::memory_order_relaxed)) break;
        Sleep(10);
    }

    if (g_unloading.load(std::memory_order_relaxed) || !StartAnimation(index, hwnd)) {
        RestoreWindowStyle(slot, hwnd);
        ReleaseSlot(slot);
    }
    return 0;
}

BOOL WINAPI HookedShowWindowAsync(HWND hwnd, int nCmdShow) {
    if (g_inHook) return pOrigShowWindowAsync(hwnd, nCmdShow);
    if (!IsShowCommand(nCmdShow)) return pOrigShowWindowAsync(hwnd, nCmdShow);

    const AnimParams p = GetParams();
    wchar_t cls[256];
    const char* reason = nullptr;
    if (!ShouldAnimateShow(hwnd, p, cls, 256, &reason)) {
        DiagSkip("ShowWindowAsync", hwnd, reason);
        return pOrigShowWindowAsync(hwnd, nCmdShow);
    }

    // ShowWindowAsync exists so that the caller does not wait for the window's thread:
    // apps call it from a worker thread precisely because the UI thread is blocked, often
    // on that same worker. The real-window path changes GWL_EXSTYLE, which for a window
    // owned by another thread is a synchronous send (WM_STYLECHANGING/WM_STYLECHANGED),
    // so taking the window over here would turn this call back into a blocking one and can
    // hang both threads. The splash path only sets a DWM attribute and sends no messages.
    if (!p.splash && GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId()) {
        DiagSkip("ShowWindowAsync (another thread's window)", hwnd,
                 "the real-window path would block the caller");
        return pOrigShowWindowAsync(hwnd, nCmdShow);
    }

    if (TooSoon(hwnd)) return pOrigShowWindowAsync(hwnd, nCmdShow);

    const int index = AllocSlot(hwnd);
    if (index < 0) return pOrigShowWindowAsync(hwnd, nCmdShow);

    AnimSlot* slot = &g_slots[index];
    slot->params = p;
    slot->showTick = GetTickCount64();
    slot->painted.store(false, std::memory_order_relaxed);
    HideForAnimation(hwnd, slot, p.splash);

    g_inHook = true;
    BOOL result = pOrigShowWindowAsync(hwnd, nCmdShow);
    g_inHook = false;

    HANDLE thread = CreateThread(nullptr, 0, DeferredAnimationThread,
                                 (LPVOID)(INT_PTR)index, 0, nullptr);
    if (thread) {
        RegisterModThread(thread);
    } else {
        RestoreWindowStyle(slot, hwnd);
        ReleaseSlot(slot);
    }
    return result;
}

// Hook: SetWindowPlacement
//
// A window that has to come up maximized is often not shown with ShowWindow at all: an app
// that also has to set the maximized placement calls SetWindowPlacement, whose showCmd
// shows the window as a side effect. Chromium does exactly that, which is why a maximized
// Edge window had no animation while a normal one did - the hook never saw a show.
//
// The structure mirrors the ShowWindow hook, and so does the rule that matters most: the
// call is never swallowed. Every path that does not animate ends in the original call.
BOOL WINAPI HookedSetWindowPlacement(HWND hwnd, const WINDOWPLACEMENT* wp) {
    if (g_inHook || !wp || wp->length < sizeof(WINDOWPLACEMENT)) {
        return pOrigSetWindowPlacement(hwnd, wp);
    }
    if (!IsShowCommand(wp->showCmd)) return pOrigSetWindowPlacement(hwnd, wp);

    const AnimParams p = GetParams();
    wchar_t cls[256];
    const char* reason = nullptr;
    if (!ShouldAnimateShow(hwnd, p, cls, 256, &reason)) {
        DiagSkip("SetWindowPlacement", hwnd, reason);
        return pOrigSetWindowPlacement(hwnd, wp);
    }
    if (TooSoon(hwnd)) {
        DiagSkip("repeat show", hwnd, "animated a moment ago");
        return pOrigSetWindowPlacement(hwnd, wp);
    }
    const int index = AllocSlot(hwnd);
    if (index < 0) return pOrigSetWindowPlacement(hwnd, wp);

    BeginAnimatedShow(hwnd, p, index);

    g_inHook = true;
    const BOOL result = pOrigSetWindowPlacement(hwnd, wp);
    g_inHook = false;

    FinishAnimatedShow(hwnd, index);
    return result;
}

// Hook: EndPaint (the content-ready signal) and SetWindowPos (following later moves)

// @include * means this code runs in EVERY process, critical system ones such as
// svchost and dllhost included, and SEH is not available: clang only accepts __try
// with -fms-extensions, and @compilerOptions can add libraries but not compile
// flags. Protection is therefore structural. Each hook's judgement touches only
// what it stored itself and what it reads back from the hwnd, never a raw pointer
// from the caller, and every early return happens after the original was called.

BOOL WINAPI HookedEndPaint(HWND hwnd, const PAINTSTRUCT* ps) {
    // With no animation running, this hook costs one atomic read
    if (g_activeAnimations.load(std::memory_order_relaxed) == 0 || !ps) {
        return pOrigEndPaint(hwnd, ps);
    }
    const BOOL r = pOrigEndPaint(hwnd, ps);
    // ps is filled in by win32k, but rcPaint has been seen badly written by drivers and
    // apps. The coordinates are compared here rather than going through IsRectEmpty,
    // which treats a negative size as an empty rectangle and would misread it as
    // "finished painting". Only strictly valid rectangles are accepted.
    const RECT& rc = ps->rcPaint;
    if (rc.right > rc.left && rc.bottom > rc.top && rc.right < 0x40000000 &&
        rc.bottom < 0x40000000) {
        const int index = FindSlot(hwnd);
        if (index >= 0) {
            g_slots[index].painted.store(true, std::memory_order_relaxed);
        }
    }
    return r;
}

BOOL WINAPI HookedSetWindowPos(HWND hwnd, HWND after, int x, int y, int cx, int cy,
                               UINT flags) {
    if (g_inHook || g_internalMove) {
        return pOrigSetWindowPos(hwnd, after, x, y, cx, cy, flags);
    }

    // The third show path: the app shows the window with
    // SetWindowPos(SWP_SHOWWINDOW). Such programs neither call ShowWindow nor create
    // the window with WS_VISIBLE, so this is the only place they can be caught.
    if ((flags & SWP_SHOWWINDOW) && !IsWindowVisible(hwnd)) {
        const AnimParams p = GetParams();
        wchar_t cls[256];
        const char* reason = nullptr;
        if (ShouldAnimateShow(hwnd, p, cls, 256, &reason) && !TooSoon(hwnd)) {
            const int index = AllocSlot(hwnd);
            if (index >= 0) {
                BeginAnimatedShow(hwnd, p, index);
                g_inHook = true;
                BOOL r = pOrigSetWindowPos(hwnd, after, x, y, cx, cy, flags);
                g_inHook = false;
                FinishAnimatedShow(hwnd, index);
                return r;
            }
        } else {
            DiagSkip("SetWindowPos(SWP_SHOWWINDOW)", hwnd, reason);
        }
    }

    if (g_activeAnimations.load(std::memory_order_relaxed) == 0) {
        return pOrigSetWindowPos(hwnd, after, x, y, cx, cy, flags);
    }

    BOOL result = pOrigSetWindowPos(hwnd, after, x, y, cx, cy, flags);

    // The app moved the window after showing it, for example restoring the position of
    // the previous session. Only when BOTH flags are set is the geometry unchanged:
    // testing for "neither is set" missed a move that came together with SWP_NOSIZE.
    if ((flags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOMOVE | SWP_NOSIZE)) {
        const int index = FindSlot(hwnd);
        if (index >= 0) {
            RECT rc;
            if (GetWindowRect(hwnd, &rc) && rc.right > rc.left && rc.bottom > rc.top) {
                AcquireSRWLockExclusive(&g_lock);
                RECT& t = g_slots[index].target;
                // Only what this call actually changed is taken from the current rect. On
                // the real-window path the window can be sitting at an animation frame, so a
                // move-only call (SWP_NOSIZE) would otherwise adopt that frame's size as the
                // target and the window would end up shrunk for good.
                if (flags & SWP_NOSIZE) {
                    rc.right = rc.left + (t.right - t.left);
                    rc.bottom = rc.top + (t.bottom - t.top);
                }
                if (flags & SWP_NOMOVE) OffsetRect(&rc, t.left - rc.left, t.top - rc.top);
                t = rc;
                ReleaseSRWLockExclusive(&g_lock);
            }
        }
    }
    return result;
}

// Lifecycle

static void CacheSelfPaths() {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    g_thisExePath = path;
    std::wstring full(path);
    const size_t slash = full.find_last_of(L"\\/");
    g_thisExeName = full.substr(slash == std::wstring::npos ? 0 : slash + 1);
    ToLowerInPlace(g_thisExeName);
}

BOOL Wh_ModInit() {
    CacheSelfPaths();

    g_isShell = (g_thisExeName == L"explorer.exe");
    ClickShareInit();

    // Windhawk injects into a process at creation time, so the cursor position right
    // now is essentially the icon the user just clicked. That is the premise of the
    // whole "zoom out of the icon" behaviour, so it is captured as early as possible.
    //
    // The fresh-launch test is not optional: a mod reload lands in processes that
    // have been running for hours, and taking their cursor position as a launch
    // anchor makes every window they open for the next kLaunchAnchorMs grow out of
    // wherever the mouse was during the reload.
    if (WeAreAFreshLaunch() && GetCursorPos(&g_launchAnchor)) g_launchAnchorValid = true;
    g_initTick = GetTickCount64();

    // Sampled here on purpose: this runs ~100ms after the click, while the Start
    // menu may still be fading out. By the time the window is shown it is gone.
    if (g_launchAnchorValid) ProbeLaunchFlyout();

    LoadSettings();
    Wh_Log(L"INIT [%s] v%s splash=%d", g_thisExeName.c_str(), WH_MOD_VERSION,
           g_params.splash ? 1 : 0);

    if (!WindhawkUtils::SetFunctionHook(ShowWindow, HookedShowWindow, &pOrigShowWindow)) {
        Wh_Log(L"FAILED to hook ShowWindow");
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(ShowWindowAsync, HookedShowWindowAsync,
                                        &pOrigShowWindowAsync)) {
        Wh_Log(L"failed to hook ShowWindowAsync (non-fatal)");
    }
    // Windows that come up maximized go through SetWindowPlacement instead of ShowWindow
    // (Chromium is the common example), so without this hook they never animate at all.
    if (!WindhawkUtils::SetFunctionHook(SetWindowPlacement, HookedSetWindowPlacement,
                                        &pOrigSetWindowPlacement)) {
        Wh_Log(L"failed to hook SetWindowPlacement (maximized windows will not animate)");
    }
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, HookedCreateWindowExW,
                                        &pOrigCreateWindowExW)) {
        Wh_Log(L"failed to hook CreateWindowExW (windows created with WS_VISIBLE "
             L"won't animate)");
    }
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExA, HookedCreateWindowExA,
                                        &pOrigCreateWindowExA)) {
        Wh_Log(L"failed to hook CreateWindowExA (ANSI programs will not animate)");
    }

    // ShellExecuteExW is hooked only in the shell process, where its meaning is
    // unambiguous: it really dispatches a launch. Hooking it elsewhere is pure
    // overhead. The Start menu belongs to StartMenuExperienceHost and tray icons to
    // their own host processes, none of them explorer, but they are all places where
    // clicking starts a program.
    if (g_isShell) {
        if (WindhawkUtils::SetFunctionHook(ShellExecuteExW, HookedShellExecuteExW,
                                           &pOrigShellExecuteExW)) {
            Wh_Log(L"hooked ShellExecuteExW (shell process: records the click position)");
        } else {
            Wh_Log(L"failed to hook ShellExecuteExW");
        }
        // The mouse-down recorder is installed here too, and only here. Explorer
        // owns the desktop and the taskbar, so this catches the icon click itself;
        // doing it in every process is what made WH_MOUSE_LL stutter (see
        // HookedDispatchMessageW), and GetMessageW is not usable at all.
        if (WindhawkUtils::SetFunctionHook(DispatchMessageW, HookedDispatchMessageW,
                                           &pOrigDispatchMessageW)) {
            Wh_Log(L"hooked DispatchMessageW (shell process: records mouse downs)");
        } else {
            Wh_Log(L"failed to hook DispatchMessageW (click position relies on ShellExecuteExW alone)");
        }
    }

    if (!WindhawkUtils::SetFunctionHook(EndPaint, HookedEndPaint, &pOrigEndPaint)) {
        Wh_Log(L"failed to hook EndPaint (readiness falls back to timeout)");
    }
    if (!WindhawkUtils::SetFunctionHook(SetWindowPos, HookedSetWindowPos,
                                        &pOrigSetWindowPos)) {
        Wh_Log(L"FAILED to hook SetWindowPos");
        return FALSE;
    }

    // Registered last, and only once: every path below this point that returns FALSE
    // leaves the mod uninited, and Wh_ModUninit is not called for it, so a class
    // registered earlier would stay registered with a procedure in an unloaded image.
    // Failing here is not fatal - every window takes the real-window path instead.
    if (!RegisterSplashClass()) {
        Wh_Log(L"splash window class registration failed, using the real-window path");
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"settings reloaded: splash=%d", g_params.splash ? 1 : 0);
}

void Wh_ModBeforeUninit() {
    // Nothing is torn down here on purpose. The engine thread used to destroy the panel
    // of a thread that had not exited yet, which frees a DC, a DIB and an icon that
    // thread is still drawing with. The threads wind down on their own: every loop in
    // them checks g_unloading, and Wh_ModUninit waits for them.
    g_unloading.store(true, std::memory_order_relaxed);
}

void Wh_ModUninit() {
    // Windhawk unmaps this DLL as soon as this returns, so no thread of ours may still be
    // running. It also means the hooks are gone and no new one can start.
    JoinModThreads();

    // Every panel has to be destroyed first: its window procedure lives in this DLL,
    // and once the DLL is unloaded that procedure dangles, so the next click crashes
    // the host process. The target windows' cloaking is lifted at the same time. The
    // threads are all joined above, so these resources have no other owner left.
    for (int i = 0; i < kMaxAnimations; i++) {
        AnimSlot* slot = &g_slots[i];
        if (!slot->splash && !slot->hwnd.load()) continue;
        DestroySplash(slot);
        if (HWND hwnd = slot->hwnd.load()) RestoreWindowStyle(slot, hwnd);
    }

    if (g_clickShared) {
        UnmapViewOfFile(g_clickShared);
        g_clickShared = nullptr;
    }
    if (g_clickMapping) {
        CloseHandle(g_clickMapping);
        g_clickMapping = nullptr;
    }
    if (g_splashClassRegistered) {
        UnregisterClassW(kSplashClass, g_modInstance);
        g_splashClassRegistered = false;
    }
}
