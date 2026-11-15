// ==WindhawkMod==
// @id              translucenttb-alternative-windows11
// @name            仿制TranslucentTB功能
// @description     仿制TranslucentTB功能，通过八种独立的状态配置、六种背景模式（原生、普通、不透明、透明、模糊、亚克力）以及可随主题自动调整的前景颜色，来自定义 Windows 11 任务栏。这个是自用版本，目前仅支持 Wndows 11(专业版 26H2)
// @version         1.3.0
// @author          greglee0
// @github          https://github.com/greglee0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lcomctl32 -luuid -lwtsapi32 -ladvapi32 -lshlwapi -lshcore
// @license         GPL-3.0
// ==/WindhawkMod==

// SPDX-License-Identifier: GPL-3.0-only
// Background identification, Taskbar.View hooks and window filtering adapted
// from Dynamic Taskbar Transparency 0.3.9, author 11581 / r1file, GPL-3.0:
// https://github.com/ramensoftware/windhawk-mods/blob/0deea25526cac2db241eccee154f4f599a062eb6/mods/dynamic-taskbar-transparency.wh.cpp
// Taskbar Styler, author m417z, GPLv3, was consulted for control paths and
// BindingExpression restoration limitations. Launcher notifications follow the
// IAppVisibility mechanism used by TranslucentTB (GPLv3).
// Changes (2026-10-31): animation-layer property overrides, foreground discovery,
// UI-thread-owned state, message-based teardown and event-driven state machine.
// Version 1.1 adds per-element ActualTheme policy, live theme notifications,
// separate system/app observations, configurable foreground and shell view events.
// Version 1.1.1 removes the invalid private-this COM offset, hooks genuine
// OnApplyTemplate COM ABI entry points, and defers property/visibility repairs.
// Version 1.2.1 adds eight independent background profiles and uniquely named
// acrylic noise effects; native resources remain underneath animation overrides.
// ShellViewCoordinator ABI below follows TranslucentTB's WindowsUdk IDL (GPLv3):
// https://github.com/TranslucentTB/TranslucentTB/blob/release/WindowsUdk/ShellViewCoordinator.idl
// This program is free software under GNU GPL version 3, without any warranty.
// License: https://www.gnu.org/licenses/gpl-3.0.html (also in accompanying LICENSE).

// ==WindhawkModReadme==
/*
# 动态透明任务栏与白色前景

## 1.3.0：按状态独立的六种背景模式

八个状态组分别提供同一套独立设置：启用、模式、背景颜色、透明度和模糊程度。
模式选项固定为原生、正常、不透明、透明、模糊、亚克力；每个状态可以单独选择，
不会共享桌面或默认回退的参数。设置修改后通过 UI 消息立即重新应用，不需要重启 Explorer。

### 1.2.2：加载故障诊断与延迟依赖

修复修饰后的 OnApplyTemplate 符号误按反修饰名称查找的问题。
Wh_ModInit 不再因 Taskbar.View 未加载、符号下载失败或 XAML 尚未就绪返回 FALSE。
AfterInit 后独立线程解析符号，1/2/5/10/30/60 秒退避，成功后停止安装重试；
后台窗口检测不会等待符号下载。不钩 LoadLibrary，不强制加载任务栏 DLL。
BeforeUninit 先停止安装线程，再由 Windhawk 移除 Hook；XAML 仍仅在原线程操作。
保留八状态背景、前景切换、最大化检测与动态主题逻辑。

日志：LOAD_INIT_ENTER=已经进入初始化；LOAD_INIT_FAIL=基础设施失败；
LOAD_WAIT_MODULE=DLL 未就绪；LOAD_SYMBOL_FAILED=符号/PDB 未取得或 ABI 不匹配；
LOAD_HOOK_REGISTER_FAILED / LOAD_HOOK_APPLY_FAILED=安装/应用失败；
LOAD_XAML_WAIT_TEMPLATE / LOAD_XAML_WAIT_ROOT=等待 XAML；
LOAD_XAML_SCAN=支持属性、活动覆盖与失败数；LOAD_STYLE_NO_TARGETS=控件不匹配。
日志缺失不能单凭此判定“未注入”，还需 Windhawk 引擎日志与 DLL 映射证据。
启用时已有模板不保证触发 OnApplyTemplate；另使用实际 COM ABI OnPointerEntered
可选入口，鼠标移入任务栏按钮时发现既有树。缺少可选符号则明确报告等待模板。
不自动重启 Explorer；真实桌面交互验收仍需在保留安全模式恢复能力的条件下进行。

## 1.2.1：每种状态独立设置

桌面、打开普通窗口、窗口最大化、开始菜单、搜索、任务视图、节电模式和默认回退，
分别提供 Enabled、Mode、BackgroundColor、Transparency、BlurAmount。
六种模式：原生、正常、不透明、透明、模糊、亚克力。
原生完全释放覆盖（忽略颜色/透明度/模糊）；正常保留原生材质并叠加所选着色；
不透明使用纯色板；透明使用可调 alpha 色板（100 为 Clear）；
模糊使用可调高斯背景，亚克力另含平铺低透明度噪点。
模糊/亚克力的透明度仅控制着色层；原生不支持独立模糊半径，因此不伪装支持。
前景 Auto/Native/White/Black/Custom 仅用于透明模式，其余模式保留原生前景。

优先级：锁屏/高对比度安全原生 > 开始 > 搜索 > 任务视图 > 检测失败回退 >
节电 > 最大化 > 普通窗口 > 桌面。关闭某状态的 Enabled 使用默认回退；
回退也关闭则恢复原生。修改的始终是任务栏，不是开始/搜索/任务视图菜单本身。
设置修改通过 UI 消息使用最新状态，不重启 Explorer。

亚克力为自定义 Composition 材质，不承诺与系统 AcrylicBrush 像素级一致。
真实隔离 XAML 测试通过不等于已在 Explorer 安装验收；请先保持旧崩溃版本禁用。

1.1.0 存在已确认的 Explorer 访问违规，应保持禁用。1.1.1 已移除导致崩溃的
`this+3` 私有布局转换，改用实际 OnApplyTemplate ABI 的 QueryInterface；属性变化
回调只投递延迟修复消息，避免在 XAML 通知内部重新启动动画。真实 Explorer 的
Start/Search/Task View 修复后交互尚未验收，不能据隔离测试宣称完全稳定。

仅注入 x64 explorer.exe，不需要其他模组。优先级：开始菜单/搜索/任务视图 > 当前虚拟桌面上
可见且未最小化的最大化窗口 > 普通窗口/桌面。有最大化窗口时，所有显示器的
任务栏一起恢复原生。全屏窗口也视为最大化。开始菜单进程、全局主题、注册表、
RequestedTheme 和应用图标均不修改。

背景沿用 Dynamic Taskbar Transparency 的实际类/名称判定：
Taskbar.TaskbarBackground 内的 BackgroundFill / BackgroundStroke。
只在实际任务栏 XamlRoot 内处理 SystemTray 与任务栏文字/单色字体图标。
TextBlock 用 Foreground，IconElement 用 Foreground，Path/Shape 用 Fill/Stroke，
PathIcon 的 Geometry 由 IconElement.Foreground 着色。
不把任意 Image 或 Composition Visual 当成有 Foreground 的控件。
位图、多色应用图标、纯 Composition 图标不重绘；诊断日志会列出这些限制。

支持系统/应用各自浅色、深色的四种组合。Auto 优先依据目标任务栏元素 ActualTheme，
系统与应用注册表值只读用于分别记录和兜底；不拿应用模式代替任务栏实际主题。
ActualThemeChanged、UISettings.ColorValuesChanged、WM_THEMECHANGED/WM_SETTINGCHANGE
和注册表变更通知共同驱动刷新，低频扫描兜底，不需要重启 Explorer。

透明前景可选 Auto / Native / White / Black / Custom；Auto 的 LightThemeForeground
与 DarkThemeForeground 默认均为白色，可自定义 #AARRGGBB 或 #RRGGBB。
ForceLight / ForceDark 只选择本模组颜色分支，从不修改 RequestedTheme 或全局主题。
FollowSystemThemeChanges=false 仅冻结模组自动配色分支；原生绑定仍跟随 Windows。
RestoreNativeThemeResources=false 仅关闭主题通知时主动重建覆盖；切回原生/卸载时
始终停止覆盖，绝不保留旧主题画刷或用固定黑白色伪装原生。

前景覆盖在动画优先级临时显示所选颜色，避免替换 local value 和主题资源；停止动画
后恢复系统当前绑定/主题画刷。背景只覆盖两个矩形的 Opacity 与其父 Panel.Background，
不写 BackgroundFill.Fill，避开系统 OnApplyTemplate 的初始化约束。
没有持续播放的动画和渐变轮询。

事件合并 30ms，开始菜单通知不等待合并；回到透明状态稳定等待 120ms。
窗口/XAML 兜底扫描每 5 秒。跨线程只传 HWND/原子状态，所有 XAML 操作在所属线程。
UI 线程无响应时卸载会保留 DLL 映射并排队恢复，防止回调执行已卸载代码；日志会说明。

安装：Windhawk → 创建新模组 → 全选替换为本文件 → 编译 → 启用。
首次下载微软 Taskbar.View 符号需要网络。若缺少必需符号则保持原生并退避重试，不拒绝初始化。
已经存在的任务栏可在鼠标移入按钮后发现；如果可选 ABI 缺失则等待模板重建。
本版不钩取私有 UpdateVisualStates 或使用任何私有 this 偏移。
可启用 diagnostics 记录实际类型、
名称、属性以及未覆盖的 Image/Composition 元素。日志不读取窗口标题或用户文本。

编译成功不等同于所有 26H2 控件或全部交互已经验收；实测范围见交付的测试报告。
对未知纯 Composition/位图托盘图标，不承诺全部变白；没有以修改全局主题来规避。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- diagnostics: false
  $name: 记录实际 XAML 控件和恢复验证
  $description: 开启 Windhawk 日志后可看到树中类型、名称、命中属性与不支持的图标类型。
- Theme:
  - ThemeDetection: Auto
    $name: ThemeDetection / 主题策略检测
    $options:
    - Auto: Auto / 使用任务栏 ActualTheme
    - ForceLight: ForceLight / 仅选择模组浅色配色
    - ForceDark: ForceDark / 仅选择模组深色配色
  - TransparentForegroundMode: Auto
    $name: TransparentForegroundMode / 透明状态前景
    $options:
    - Auto: Auto / 按任务栏主题配色
    - Native: Native / 保留原生前景
    - White: White / 白色
    - Black: Black / 黑色
    - Custom: Custom / 自定义颜色
  - LightThemeForeground: '#FFFFFFFF'
    $name: LightThemeForeground / 浅色任务栏配色
  - DarkThemeForeground: '#FFFFFFFF'
    $name: DarkThemeForeground / 深色任务栏配色
  - CustomForegroundColor: '#FFFFFFFF'
    $name: CustomForegroundColor / 自定义前景
    $description: 支持 #AARRGGBB 或 #RRGGBB；仅透明状态生效。
  - FollowSystemThemeChanges: true
    $name: FollowSystemThemeChanges / 跟随运行时主题变化
  - RestoreNativeThemeResources: true
    $name: RestoreNativeThemeResources / 主题变化时主动重建覆盖
    $description: 关闭只跳过主动重建。原生状态和卸载始终释放覆盖、恢复当前原生资源。
  $name: Theme / 主题
- Desktop:
  - Enabled: true
    $name: 启用此状态（关闭后使用默认回退）
  - Mode: Transparent
    $name: 背景模式
    $options:
    - Native: 原生
    - Normal: 正常
    - Opaque: 不透明
    - Transparent: 透明
    - Blur: 模糊
    - Acrylic: 亚克力
  - BackgroundColor: Auto
    $name: 背景颜色
    $description: Auto 跟随任务栏实际主题，或 #RRGGBB / #AARRGGBB。
  - Transparency: 100
    $name: 透明度（0–100）
    $description: 0 为不透明，100 为完全透明。模糊/亚克力中控制着色层；原生忽略，不透明固定为 0。
  - BlurAmount: 30
    $name: 模糊程度（0–100）
    $description: 模糊/亚克力的半径；其他模式不使用。
  $name: 在桌面时
- NormalWindow:
  - Enabled: true
    $name: 启用此状态（关闭后使用默认回退）
  - Mode: Transparent
    $name: 背景模式
    $options:
    - Native: 原生
    - Normal: 正常
    - Opaque: 不透明
    - Transparent: 透明
    - Blur: 模糊
    - Acrylic: 亚克力
  - BackgroundColor: Auto
    $name: 背景颜色
    $description: Auto 跟随任务栏实际主题，或 #RRGGBB / #AARRGGBB。
  - Transparency: 100
    $name: 透明度（0–100）
    $description: 0 为不透明，100 为完全透明。模糊/亚克力中控制着色层；原生忽略，不透明固定为 0。
  - BlurAmount: 30
    $name: 模糊程度（0–100）
    $description: 模糊/亚克力的半径；其他模式不使用。
  $name: 打开任何普通窗口时
- Maximized:
  - Enabled: true
    $name: 启用此状态（关闭后使用默认回退）
  - Mode: Native
    $name: 背景模式
    $options:
    - Native: 原生
    - Normal: 正常
    - Opaque: 不透明
    - Transparent: 透明
    - Blur: 模糊
    - Acrylic: 亚克力
  - BackgroundColor: Auto
    $name: 背景颜色
    $description: Auto 跟随任务栏实际主题，或 #RRGGBB / #AARRGGBB。
  - Transparency: 0
    $name: 透明度（0–100）
    $description: 0 为不透明，100 为完全透明。模糊/亚克力中控制着色层；原生忽略，不透明固定为 0。
  - BlurAmount: 30
    $name: 模糊程度（0–100）
    $description: 模糊/亚克力的半径；其他模式不使用。
  $name: 窗口最大化时
- StartMenu:
  - Enabled: true
    $name: 启用此状态（关闭后使用默认回退）
  - Mode: Native
    $name: 背景模式
    $options:
    - Native: 原生
    - Normal: 正常
    - Opaque: 不透明
    - Transparent: 透明
    - Blur: 模糊
    - Acrylic: 亚克力
  - BackgroundColor: Auto
    $name: 背景颜色
    $description: Auto 跟随任务栏实际主题，或 #RRGGBB / #AARRGGBB。
  - Transparency: 0
    $name: 透明度（0–100）
    $description: 0 为不透明，100 为完全透明。模糊/亚克力中控制着色层；原生忽略，不透明固定为 0。
  - BlurAmount: 30
    $name: 模糊程度（0–100）
    $description: 模糊/亚克力的半径；其他模式不使用。
  $name: 打开开始菜单时
- Search:
  - Enabled: true
    $name: 启用此状态（关闭后使用默认回退）
  - Mode: Native
    $name: 背景模式
    $options:
    - Native: 原生
    - Normal: 正常
    - Opaque: 不透明
    - Transparent: 透明
    - Blur: 模糊
    - Acrylic: 亚克力
  - BackgroundColor: Auto
    $name: 背景颜色
    $description: Auto 跟随任务栏实际主题，或 #RRGGBB / #AARRGGBB。
  - Transparency: 0
    $name: 透明度（0–100）
    $description: 0 为不透明，100 为完全透明。模糊/亚克力中控制着色层；原生忽略，不透明固定为 0。
  - BlurAmount: 30
    $name: 模糊程度（0–100）
    $description: 模糊/亚克力的半径；其他模式不使用。
  $name: 打开搜索菜单时
- TaskView:
  - Enabled: true
    $name: 启用此状态（关闭后使用默认回退）
  - Mode: Native
    $name: 背景模式
    $options:
    - Native: 原生
    - Normal: 正常
    - Opaque: 不透明
    - Transparent: 透明
    - Blur: 模糊
    - Acrylic: 亚克力
  - BackgroundColor: Auto
    $name: 背景颜色
    $description: Auto 跟随任务栏实际主题，或 #RRGGBB / #AARRGGBB。
  - Transparency: 0
    $name: 透明度（0–100）
    $description: 0 为不透明，100 为完全透明。模糊/亚克力中控制着色层；原生忽略，不透明固定为 0。
  - BlurAmount: 30
    $name: 模糊程度（0–100）
    $description: 模糊/亚克力的半径；其他模式不使用。
  $name: 打开任务视图时
- PowerSaving:
  - Enabled: true
    $name: 启用此状态（关闭后使用默认回退）
  - Mode: Native
    $name: 背景模式
    $options:
    - Native: 原生
    - Normal: 正常
    - Opaque: 不透明
    - Transparent: 透明
    - Blur: 模糊
    - Acrylic: 亚克力
  - BackgroundColor: Auto
    $name: 背景颜色
    $description: Auto 跟随任务栏实际主题，或 #RRGGBB / #AARRGGBB。
  - Transparency: 0
    $name: 透明度（0–100）
    $description: 0 为不透明，100 为完全透明。模糊/亚克力中控制着色层；原生忽略，不透明固定为 0。
  - BlurAmount: 30
    $name: 模糊程度（0–100）
    $description: 模糊/亚克力的半径；其他模式不使用。
  $name: 开启节电模式时
- Fallback:
  - Enabled: true
    $name: 启用此状态（关闭后使用默认回退）
  - Mode: Native
    $name: 背景模式
    $options:
    - Native: 原生
    - Normal: 正常
    - Opaque: 不透明
    - Transparent: 透明
    - Blur: 模糊
    - Acrylic: 亚克力
  - BackgroundColor: Auto
    $name: 背景颜色
    $description: Auto 跟随任务栏实际主题，或 #RRGGBB / #AARRGGBB。
  - Transparency: 0
    $name: 透明度（0–100）
    $description: 0 为不透明，100 为完全透明。模糊/亚克力中控制着色层；原生忽略，不透明固定为 0。
  - BlurAmount: 30
    $name: 模糊程度（0–100）
    $description: 模糊/亚克力的半径；其他模式不使用。
  $name: 默认回退状态
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#undef GetCurrentTime
#include <windows.h>
#include <dwmapi.h>
#include <shobjidl.h>
#include <wtsapi32.h>
#include <roapi.h>
#include <powersetting.h>
#include <initguid.h>
#include <d2d1_1.h>
#include <d2d1effects_2.h>
#include <windows.graphics.effects.h>
#include <shlwapi.h>
#include <shcore.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <atomic>
#include <cstdio>
#include <memory>
#include <mutex>
#include <optional>
#include <cstdint>
#include <cwctype>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using namespace winrt::Windows::UI::Xaml;
namespace C = winrt::Windows::UI::Xaml::Controls;
namespace M = winrt::Windows::UI::Xaml::Media;
namespace A = winrt::Windows::UI::Xaml::Media::Animation;
namespace S = winrt::Windows::UI::Xaml::Shapes;
namespace F = winrt::Windows::Foundation;
using Inspectable = F::IInspectable;

constexpr wchar_t kUiClass[] = L"DTTWhite.UI.1." WH_MOD_ID;
constexpr wchar_t kWorkerClass[] = L"DTTWhite.Worker.1." WH_MOD_ID;
constexpr UINT kApply = WM_APP + 71, kCleanup = WM_APP + 72;
constexpr UINT kDetect = WM_APP + 73, kStop = WM_APP + 74;
constexpr UINT kDiscover = WM_APP + 75;
constexpr UINT kThemeRefresh = WM_APP + 76, kObserveTheme = WM_APP + 77;
constexpr UINT kPropertyRefresh = WM_APP + 78;
constexpr UINT_PTR kFallbackTimer = 1, kEventTimer = 2, kClearTimer = 3;
constexpr UINT_PTR kDiscoveryTimer = 4;
constexpr UINT_PTR kPropertyTimer = 5;
// Explorer's Peek notifications, as used by taskbar-background-helper.
constexpr DWORD EVENT_SYSTEM_PEEKSTART = 0x0021;
constexpr DWORD EVENT_SYSTEM_PEEKEND = 0x0022;
std::atomic<bool> g_unloading{false}, g_native{true}, g_diagnostics{false};
std::atomic<bool> g_eventPosted{false}, g_peeking{false};
std::atomic<bool> g_launcherOpen{false};
std::atomic<bool> g_shellNativeRequested{false};
enum class TaskbarState { Desktop, NormalWindow, Maximized, TaskView, Search, Start,
                          PowerSaving, Locked, HighContrast, Fallback, Clear = Desktop };
std::atomic<TaskbarState> g_taskbarState{TaskbarState::Fallback};
std::atomic<HWND> g_workerWindow{nullptr};
std::thread g_worker;
HANDLE g_ready = nullptr;
HMODULE g_module = nullptr;
std::mutex g_windowsMutex;
std::vector<HWND> g_uiWindows;
thread_local HWND g_uiWindow = nullptr; // No WinRT TLS destructor at DLL unload.
std::atomic<bool> g_canLog{true};
#define DTT_LOG(...) do { if (g_canLog.load()) Wh_Log(__VA_ARGS__); } while (0)
std::atomic<bool> g_symbolsHooked{false};
// Only the dependency thread resolves/registers/applies hooks. It is joined in
// BeforeUninit, BEFORE Windhawk disallows hook operations. No loader hook/XAML
// access is needed to observe a delayed Taskbar.View load.
std::thread g_dependencyThread;
HANDLE g_dependencyWake = nullptr;
std::atomic<bool> g_afterInitStarted{false};
std::atomic<unsigned> g_rootsDiscovered{0}, g_stylePasses{0};
unsigned DependencyRetryDelay(unsigned attempt) {
    constexpr unsigned delays[] = {1000, 2000, 5000, 10000, 30000, 60000};
    return delays[std::min<size_t>(attempt, ARRAYSIZE(delays) - 1)];
}
std::atomic<unsigned> g_discoveryCalls{0};
std::atomic<unsigned> g_externalCallbacks{0};

enum class ThemeDetection { Auto, ForceLight, ForceDark };
enum class ForegroundMode { Auto, Native, White, Black, Custom };
enum class BackgroundMode { Native, Normal, Opaque, Transparent, Blur, Acrylic };
struct BackgroundAppearance {
    BackgroundMode mode = BackgroundMode::Native;
    uint32_t color = 0xFF202020;
    bool themeColor = true;
    int transparency = 0; // 0 = opaque, 100 = clear (tint opacity for blur/acrylic).
    int blur = 30;        // Gaussian standard deviation in device independent pixels.
    bool enabled = true;
    bool operator==(BackgroundAppearance const&) const = default;
};
struct ThemeSettings {
    ThemeDetection detection = ThemeDetection::Auto;
    ForegroundMode foreground = ForegroundMode::Auto;
    uint32_t lightColor = 0xFFFFFFFF, darkColor = 0xFFFFFFFF, customColor = 0xFFFFFFFF;
    bool follow = true, refreshResources = true;
    uint64_t revision = 1;
    BackgroundAppearance desktop{BackgroundMode::Transparent, 0xFF202020, true, 100, 30};
    BackgroundAppearance normalWindow{BackgroundMode::Transparent, 0xFF202020, true, 100, 30};
    BackgroundAppearance maximized, startMenu, search, taskView, powerSaving, fallback;
};
struct ThemeObservation {
    bool systemLight = true, appsLight = true;
    bool systemKnown = false, appsKnown = false;
    bool uiAppLight = true, uiColorKnown = false;
    uint64_t revision = 0;
};
std::mutex g_themeMutex;
ThemeSettings g_themeSettings;
ThemeObservation g_themeObservation;

ThemeSettings GetThemeSettings() {
    std::lock_guard lock(g_themeMutex);
    return g_themeSettings;
}
ThemeObservation GetThemeObservation() {
    std::lock_guard lock(g_themeMutex);
    return g_themeObservation;
}
void StoreThemeSettings(ThemeSettings settings) {
    std::lock_guard lock(g_themeMutex);
    settings.revision = g_themeSettings.revision + 1;
    g_themeSettings = settings;
}
BackgroundMode ParseBackgroundMode(std::wstring_view text, BackgroundMode fallback) {
    // Windhawk stores option keys, but accepting the visible Chinese labels keeps
    // existing hand-edited profiles readable without changing the schema.
    if (text == L"Native" || text == L"原生") return BackgroundMode::Native;
    if (text == L"Normal" || text == L"正常") return BackgroundMode::Normal;
    if (text == L"Opaque" || text == L"不透明") return BackgroundMode::Opaque;
    if (text == L"Transparent" || text == L"透明") return BackgroundMode::Transparent;
    if (text == L"Blur" || text == L"模糊") return BackgroundMode::Blur;
    if (text == L"Acrylic" || text == L"亚克力") return BackgroundMode::Acrylic;
    return fallback;
}
bool ParseArgb(std::wstring_view text, uint32_t& result) {
    if (text.empty() || text.front() != L'#' || (text.size() != 7 && text.size() != 9))
        return false;
    uint32_t color = 0;
    for (wchar_t ch : text.substr(1)) {
        unsigned digit;
        if (ch >= L'0' && ch <= L'9') digit = ch - L'0';
        else if (ch >= L'a' && ch <= L'f') digit = ch - L'a' + 10;
        else if (ch >= L'A' && ch <= L'F') digit = ch - L'A' + 10;
        else return false;
        color = (color << 4) | digit;
    }
    result = text.size() == 7 ? color | 0xFF000000 : color;
    return true;
}
winrt::Windows::UI::Color ArgbColor(uint32_t value) {
    return {static_cast<BYTE>(value >> 24), static_cast<BYTE>(value >> 16),
            static_cast<BYTE>(value >> 8), static_cast<BYTE>(value)};
}
std::wstring ReadStringSetting(PCWSTR name) {
    auto value = WindhawkUtils::StringSetting::make(name);
    return value.get() ? value.get() : L"";
}
void LoadThemeSettings() {
    ThemeSettings settings;
    auto detection = ReadStringSetting(L"Theme.ThemeDetection");
    if (detection == L"ForceLight") settings.detection = ThemeDetection::ForceLight;
    else if (detection == L"ForceDark") settings.detection = ThemeDetection::ForceDark;
    auto mode = ReadStringSetting(L"Theme.TransparentForegroundMode");
    if (mode == L"Native") settings.foreground = ForegroundMode::Native;
    else if (mode == L"White") settings.foreground = ForegroundMode::White;
    else if (mode == L"Black") settings.foreground = ForegroundMode::Black;
    else if (mode == L"Custom") settings.foreground = ForegroundMode::Custom;
    auto readColor = [](PCWSTR key, uint32_t& color) {
        auto text = ReadStringSetting(key);
        if (!ParseArgb(text, color))
            DTT_LOG(L"Invalid/missing color %s='%s'; default #FFFFFFFF", key, text.c_str());
    };
    readColor(L"Theme.LightThemeForeground", settings.lightColor);
    readColor(L"Theme.DarkThemeForeground", settings.darkColor);
    readColor(L"Theme.CustomForegroundColor", settings.customColor);
    auto readBackground = [&](PCWSTR prefix, BackgroundAppearance defaults) {
        BackgroundAppearance result = defaults;
        wchar_t key[128];
        swprintf_s(key, L"%s.Mode", prefix);
        auto modeText = ReadStringSetting(key);
        // Preserve all defaults for a new group not yet present in an old install.
        if (modeText.empty()) return result;
        auto parsed = ParseBackgroundMode(modeText, result.mode);
        if (parsed == result.mode && modeText != L"Native" && modeText != L"原生" &&
            modeText != L"Normal" && modeText != L"正常" && modeText != L"Opaque" &&
            modeText != L"不透明" && modeText != L"Transparent" && modeText != L"透明" &&
            modeText != L"Blur" && modeText != L"模糊" && modeText != L"Acrylic" &&
            modeText != L"亚克力")
            DTT_LOG(L"Unknown background mode %s; retaining default", modeText.c_str());
        result.mode = parsed;
        swprintf_s(key, L"%s.BackgroundColor", prefix);
        auto text = ReadStringSetting(key);
        result.themeColor = text == L"Auto" || text == L"auto" || text.empty();
        if (!result.themeColor && !ParseArgb(text, result.color)) {
            result.themeColor = true;
            DTT_LOG(L"Invalid %s='%s'; using ActualTheme color", key, text.c_str());
        }
        swprintf_s(key, L"%s.Transparency", prefix);
        result.transparency = std::clamp(Wh_GetIntSetting(key), 0, 100);
        swprintf_s(key, L"%s.BlurAmount", prefix);
        result.blur = std::clamp(Wh_GetIntSetting(key), 0, 100);
        swprintf_s(key, L"%s.Enabled", prefix);
        result.enabled = Wh_GetIntSetting(key) != 0;
        return result;
    };
    settings.desktop = readBackground(L"Desktop", settings.desktop);
    settings.normalWindow = readBackground(L"NormalWindow", settings.normalWindow);
    settings.maximized = readBackground(L"Maximized", settings.maximized);
    settings.startMenu = readBackground(L"StartMenu", settings.startMenu);
    settings.search = readBackground(L"Search", settings.search);
    settings.taskView = readBackground(L"TaskView", settings.taskView);
    settings.powerSaving = readBackground(L"PowerSaving", settings.powerSaving);
    settings.fallback = readBackground(L"Fallback", settings.fallback);
    // Old installed copies may not yet have the new group persisted. Preserve
    // advertised defaults until Windhawk supplies a nonempty mode/detection.
    bool newGroupMissing = mode.empty() && detection.empty();
    settings.follow = newGroupMissing || Wh_GetIntSetting(L"Theme.FollowSystemThemeChanges") != 0;
    settings.refreshResources = newGroupMissing || Wh_GetIntSetting(L"Theme.RestoreNativeThemeResources") != 0;
    StoreThemeSettings(settings);
}
void BroadcastUi(UINT message) {
    std::lock_guard lock(g_windowsMutex);
    for (HWND window : g_uiWindows) PostMessageW(window, message, 0, 0);
}

struct ExternalCallbackGuard {
    ExternalCallbackGuard() { ++g_externalCallbacks; }
    ~ExternalCallbackGuard() { --g_externalCallbacks; }
};
void RequestThemeObservation() noexcept {
    if (!g_unloading)
        if (auto worker = g_workerWindow.load()) PostMessageW(worker, kObserveTheme, 0, 0);
}

void LogException(PCWSTR operation) noexcept {
    DTT_LOG(L"%s failed hr=%08X", operation, winrt::to_hresult());
}

void ObserveTheme(winrt::Windows::UI::ViewManagement::UISettings const& uiSettings,
                  bool notify = true) {
    ThemeObservation observed;
    constexpr PCWSTR key = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    auto read = [&](PCWSTR name, bool& value) {
        DWORD number = 0, bytes = sizeof(number);
        LSTATUS status = RegGetValueW(HKEY_CURRENT_USER, key, name, RRF_RT_REG_DWORD,
                                     nullptr, &number, &bytes);
        if (status == ERROR_SUCCESS) value = number != 0;
        return status == ERROR_SUCCESS;
    };
    observed.systemKnown = read(L"SystemUsesLightTheme", observed.systemLight);
    observed.appsKnown = read(L"AppsUseLightTheme", observed.appsLight);
    try {
        if (uiSettings) {
            auto background = uiSettings.GetColorValue(
                winrt::Windows::UI::ViewManagement::UIColorType::Background);
            observed.uiAppLight = 299 * background.R + 587 * background.G + 114 * background.B >= 128000;
            observed.uiColorKnown = true;
            if (!observed.appsKnown) observed.appsLight = observed.uiAppLight;
        }
    } catch (...) { LogException(L"UISettings.GetColorValue"); }
    bool changed;
    {
        std::lock_guard lock(g_themeMutex);
        auto& previous = g_themeObservation;
        changed = previous.systemLight != observed.systemLight ||
            previous.appsLight != observed.appsLight || previous.systemKnown != observed.systemKnown ||
            previous.appsKnown != observed.appsKnown || previous.uiColorKnown != observed.uiColorKnown ||
            previous.uiAppLight != observed.uiAppLight;
        observed.revision = previous.revision + 1;
        previous = observed;
    }
    if (changed || g_diagnostics)
        DTT_LOG(L"THEME_OBSERVED system=%s apps=%s mixed=%d UISettingsApps=%s; taskbar uses ActualTheme",
            observed.systemKnown ? (observed.systemLight ? L"Light" : L"Dark") : L"Unknown",
            observed.appsKnown ? (observed.appsLight ? L"Light" : L"Dark") : L"Unknown",
            observed.systemKnown && observed.appsKnown && observed.systemLight != observed.appsLight,
            observed.uiColorKnown ? (observed.uiAppLight ? L"Light" : L"Dark") : L"Unknown");
    // Even identical mode values may accompany changed accent/theme resources.
    if (notify || changed) BroadcastUi(kThemeRefresh);
}

// Effect interop and Composite/Flood/Gaussian adapters adapted from DTT 0.3.9,
// GPL-3.0, 11581/r1file. No private taskbar object-layout code is reused.
namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<winrt::impl::abi_t<F::IPropertyValue>>{
        winrt::impl::guid_v<F::IPropertyValue>};

typedef enum MY_D2D1_GAUSSIANBLUR_OPTIMIZATION {
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED = 0,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED = 1,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY = 2,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_FORCE_DWORD = 0xffffffff
} MY_D2D1_GAUSSIANBLUR_OPTIMIZATION;

#ifndef BUILD_WINDOWS
namespace ABI {
#endif
namespace Windows {
namespace Graphics {
namespace Effects {

typedef interface IGraphicsEffectSource IGraphicsEffectSource;
typedef interface IGraphicsEffectD2D1Interop IGraphicsEffectD2D1Interop;

typedef enum GRAPHICS_EFFECT_PROPERTY_MAPPING {
    GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORX,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORY,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORZ,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORW,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4
} GRAPHICS_EFFECT_PROPERTY_MAPPING;

#undef INTERFACE
#define INTERFACE IGraphicsEffectD2D1Interop
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop,
                       IUnknown,
                       "2FC57384-A068-44D7-A331-30982FCF7177") {
    STDMETHOD(GetEffectId)(_Out_ GUID* id) PURE;
    STDMETHOD(GetNamedPropertyMapping)
    (LPCWSTR name,
     _Out_ UINT* index,
     _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) PURE;
    STDMETHOD(GetPropertyCount)(_Out_ UINT* count) PURE;
    STDMETHOD(GetProperty)
    (UINT index, _Outptr_ winrt::impl::abi_t<F::IPropertyValue>** value) PURE;
    STDMETHOD(GetSource)(UINT index,
                         _Outptr_ IGraphicsEffectSource** source) PURE;
    STDMETHOD(GetSourceCount)(_Out_ UINT* count) PURE;
};

}  // namespace Effects
}  // namespace Graphics
}  // namespace Windows
#ifndef BUILD_WINDOWS
}  // namespace ABI
#endif

namespace awge = ABI::Windows::Graphics::Effects;

template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<awge::IGraphicsEffectD2D1Interop>{
        0x2FC57384,
        0xA068,
        0x44D7,
        {0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77}};

struct CompositeEffect
    : winrt::implements<CompositeEffect,
                        wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) {
            return E_INVALIDARG;
        }
        *id = CLSID_D2D1Composite;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) {
            return E_INVALIDARG;
        }
        if (std::wstring_view(name) == L"Mode") {
            *index = D2D1_COMPOSITE_PROP_MODE;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 1;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<F::IPropertyValue>** value) noexcept override try {
        if (!value) {
            return E_INVALIDARG;
        }
        if (index != D2D1_COMPOSITE_PROP_MODE) {
            return E_BOUNDS;
        }
        *value = F::PropertyValue::CreateUInt32(static_cast<UINT32>(Mode))
                     .as<winrt::impl::abi_t<F::IPropertyValue>>()
                     .detach();
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSource(UINT index,
                                        awge::IGraphicsEffectSource** source)
        noexcept override try {
        if (!source) {
            return E_INVALIDARG;
        }
        winrt::copy_to_abi(Sources.at(index), *reinterpret_cast<void**>(source));
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = static_cast<UINT>(Sources.size());
        return S_OK;
    }

    winrt::hstring Name() {
        return name;
    }

    void Name(winrt::hstring value) {
        name = value;
    }

    std::vector<wge::IGraphicsEffectSource> Sources;
    D2D1_COMPOSITE_MODE Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;

private:
    winrt::hstring name = L"CompositeEffect";
};

struct FloodEffect
    : winrt::implements<FloodEffect,
                        wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) {
            return E_INVALIDARG;
        }
        *id = CLSID_D2D1Flood;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) {
            return E_INVALIDARG;
        }
        if (std::wstring_view(name) == L"Color") {
            *index = D2D1_FLOOD_PROP_COLOR;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 1;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<F::IPropertyValue>** value) noexcept override try {
        if (!value) {
            return E_INVALIDARG;
        }
        if (index != D2D1_FLOOD_PROP_COLOR) {
            return E_BOUNDS;
        }
        *value = F::PropertyValue::CreateSingleArray({
                     Color.R / 255.0f,
                     Color.G / 255.0f,
                     Color.B / 255.0f,
                     Color.A / 255.0f,
                 })
                     .as<winrt::impl::abi_t<F::IPropertyValue>>()
                     .detach();
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSource(UINT,
                                        awge::IGraphicsEffectSource** source)
        noexcept override {
        if (!source) {
            return E_INVALIDARG;
        }
        return E_BOUNDS;
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 0;
        return S_OK;
    }

    winrt::hstring Name() {
        return name;
    }

    void Name(winrt::hstring value) {
        name = value;
    }

    winrt::Windows::UI::Color Color{};

private:
    winrt::hstring name = L"FloodEffect";
};

struct GaussianBlurEffect
    : winrt::implements<GaussianBlurEffect,
                        wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) {
            return E_INVALIDARG;
        }
        *id = CLSID_D2D1GaussianBlur;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) {
            return E_INVALIDARG;
        }

        const std::wstring_view nameView(name);
        if (nameView == L"BlurAmount") {
            *index = D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        if (nameView == L"Optimization") {
            *index = D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        if (nameView == L"BorderMode") {
            *index = D2D1_GAUSSIANBLUR_PROP_BORDER_MODE;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }

        return E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 3;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<F::IPropertyValue>** value) noexcept override try {
        if (!value) {
            return E_INVALIDARG;
        }

        switch (index) {
            case D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION:
                *value = F::PropertyValue::CreateSingle(BlurAmount)
                             .as<winrt::impl::abi_t<F::IPropertyValue>>()
                             .detach();
                break;
            case D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION:
                *value =
                    F::PropertyValue::CreateUInt32(
                        static_cast<UINT32>(Optimization))
                        .as<winrt::impl::abi_t<F::IPropertyValue>>()
                        .detach();
                break;
            case D2D1_GAUSSIANBLUR_PROP_BORDER_MODE:
                *value = F::PropertyValue::CreateUInt32(
                             static_cast<UINT32>(BorderMode))
                             .as<winrt::impl::abi_t<F::IPropertyValue>>()
                             .detach();
                break;
            default:
                return E_BOUNDS;
        }

        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSource(UINT index,
                                        awge::IGraphicsEffectSource** source)
        noexcept override {
        if (!source) {
            return E_INVALIDARG;
        }
        if (index != 0) {
            return E_BOUNDS;
        }
        winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 1;
        return S_OK;
    }

    winrt::hstring Name() {
        return name;
    }

    void Name(winrt::hstring value) {
        name = value;
    }

    wge::IGraphicsEffectSource Source{nullptr};
    float BlurAmount = 3.0f;
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION Optimization =
        MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED;
    D2D1_BORDER_MODE BorderMode = D2D1_BORDER_MODE_SOFT;

private:
    winrt::hstring name = L"GaussianBlurEffect";
};


// Generic one-source D2D adapter for acrylic noise wrapping/opacity. The main
// blur/tint graph uses the actual DTT source adapters immediately above.
struct MaterialUnaryEffect : winrt::implements<MaterialUnaryEffect,
    wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop> {
    GUID effectId{};
    std::vector<F::IInspectable> values;
    wge::IGraphicsEffectSource source{nullptr};
    winrt::hstring name = L"MaterialUnary";
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_POINTER; *id=effectId; return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR, UINT*,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING*) noexcept override { return E_INVALIDARG; }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_POINTER; *count=static_cast<UINT>(values.size()); return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<F::IPropertyValue>** value) noexcept override try {
        if (!value) return E_POINTER; *value=nullptr;
        if (index>=values.size()) return E_BOUNDS;
        *value=values[index].as<winrt::impl::abi_t<F::IPropertyValue>>().detach(); return S_OK;
    } catch (...) { return winrt::to_hresult(); }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** value) noexcept override {
        if (!value) return E_POINTER; *value=nullptr;
        if (index) return E_BOUNDS;
        winrt::copy_to_abi(source,*reinterpret_cast<void**>(value)); return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_POINTER; *count=1; return S_OK;
    }
    winrt::hstring Name() { return name; }
    void Name(winrt::hstring const& value) { name=value; }
};

// Custom acrylic material: adjustable Gaussian backdrop, tint and tiled grain.
// Native AcrylicBrush has no public blur-radius control. This graph deliberately
// owns its sources, never modifies a shared native brush or taskbar Fill, and
// uses an embedded procedural BMP (no external file/download dependency).
struct TaskbarMaterialBrush : M::XamlCompositionBrushBaseT<TaskbarMaterialBrush> {
    wuc::Compositor compositor{nullptr};
    float radius;
    winrt::Windows::UI::Color tint;
    bool acrylic;
    bool connected=false, effectFailed=false;
    HRESULT effectError=S_OK;
    int buildStage=0;
    bool HasEffect() const { return bool(CompositionBrush()); }
    wuc::CompositionBackdropBrush backdrop{nullptr};
    wuc::CompositionSurfaceBrush noiseBrush{nullptr};
    M::LoadedImageSurface noiseImage{nullptr};
    winrt::Windows::Storage::Streams::IRandomAccessStream noiseStream{nullptr};
    TaskbarMaterialBrush(UIElement const& target, float blur,
        winrt::Windows::UI::Color color, bool grain)
        : compositor(wuxh::ElementCompositionPreview::GetElementVisual(target).Compositor()),
          radius(std::clamp(blur,0.0f,100.0f)),tint(color),acrylic(grain) {
        auto fallback=color; fallback.A=255; FallbackColor(fallback);
    }
    void MakeNoise() {
        buildStage=1;
        constexpr unsigned width=128, height=128, stride=width*3;
        std::vector<BYTE> bmp(sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+stride*height);
        BITMAPFILEHEADER file{}; file.bfType=0x4D42; file.bfSize=static_cast<DWORD>(bmp.size());
        file.bfOffBits=sizeof(file)+sizeof(BITMAPINFOHEADER);
        BITMAPINFOHEADER info{}; info.biSize=sizeof(info); info.biWidth=width; info.biHeight=height;
        info.biPlanes=1; info.biBitCount=24; info.biCompression=BI_RGB; info.biSizeImage=stride*height;
        std::memcpy(bmp.data(),&file,sizeof(file));
        std::memcpy(bmp.data()+sizeof(file),&info,sizeof(info));
        uint32_t random=0xC0DEC0DE;
        for(unsigned i=file.bfOffBits;i<bmp.size();i+=3) {
            random^=random<<13; random^=random>>17; random^=random<<5;
            BYTE gray=static_cast<BYTE>(random); bmp[i]=bmp[i+1]=bmp[i+2]=gray;
        }
        winrt::com_ptr<IStream> stream;
        stream.attach(SHCreateMemStream(bmp.data(),static_cast<UINT>(bmp.size())));
        if(!stream) winrt::throw_hresult(E_OUTOFMEMORY);
        winrt::check_hresult(CreateRandomAccessStreamOverStream(stream.get(),BSOS_DEFAULT,
            winrt::guid_of<decltype(noiseStream)>(),winrt::put_abi(noiseStream)));
        buildStage=2;
        noiseImage=M::LoadedImageSurface::StartLoadFromStream(noiseStream);
        noiseBrush=compositor.CreateSurfaceBrush(noiseImage);
        noiseBrush.Stretch(wuc::CompositionStretch::None);
        noiseBrush.HorizontalAlignmentRatio(0); noiseBrush.VerticalAlignmentRatio(0);
    }
    wuc::CompositionBrush CreateMaterial() {
        backdrop=compositor.CreateBackdropBrush();
        auto blur=winrt::make_self<GaussianBlurEffect>();
        blur->Source=wuc::CompositionEffectSourceParameter(L"backdrop");
        blur->BlurAmount=radius; blur->BorderMode=D2D1_BORDER_MODE_HARD;
        auto flood=winrt::make_self<FloodEffect>(); flood->Color=tint;
        auto composite=winrt::make_self<CompositeEffect>();
        composite->Mode=D2D1_COMPOSITE_MODE_SOURCE_OVER;
        composite->Sources.push_back(*blur); composite->Sources.push_back(*flood);
        if(acrylic) {
            MakeNoise();
            // CreateEffectFactory requires distinct node names in this graph.
            // Both adapters previously inherited "MaterialUnary", causing
            // E_INVALIDARG. Keep the wrap effect; only distinguish its identity.
            auto tile=winrt::make_self<MaterialUnaryEffect>(); tile->effectId=CLSID_D2D1Border;
            tile->Name(L"AcrylicNoiseWrap");
            tile->values={F::PropertyValue::CreateUInt32(D2D1_BORDER_EDGE_MODE_WRAP),
                          F::PropertyValue::CreateUInt32(D2D1_BORDER_EDGE_MODE_WRAP)};
            tile->source=wuc::CompositionEffectSourceParameter(L"noise");
            auto opacity=winrt::make_self<MaterialUnaryEffect>(); opacity->effectId=CLSID_D2D1Opacity;
            opacity->Name(L"AcrylicNoiseOpacity");
            opacity->values={F::PropertyValue::CreateSingle(0.035f)}; opacity->source=*tile;
            composite->Sources.push_back(*opacity);
        }
        buildStage=3;
        auto factory=compositor.CreateEffectFactory(*composite);
        buildStage=4;
        auto result=factory.CreateBrush(); result.SetSourceParameter(L"backdrop",backdrop);
        if(acrylic) result.SetSourceParameter(L"noise",noiseBrush);
        return result;
    }
    void OnConnected() noexcept {
        connected=true;
        try { if(!CompositionBrush()) CompositionBrush(CreateMaterial()); }
        catch (...) { effectFailed=true;effectError=winrt::to_hresult();DTT_LOG(L"Material effect stage=%d hr=%08X; fallback color",buildStage,effectError); }
    }
    void OnDisconnected() noexcept {
        connected=false;
        try {
            if(auto brush=CompositionBrush()) { CompositionBrush(nullptr); brush.Close(); }
            if(noiseBrush) noiseBrush.Close(); noiseBrush=nullptr;
            if(noiseImage) noiseImage.Close(); noiseImage=nullptr;
            if(noiseStream) noiseStream.Close(); noiseStream=nullptr;
            if(backdrop) backdrop.Close(); backdrop=nullptr;
        } catch (...) { LogException(L"Material disconnect"); }
    }
};

bool ClassIs(Inspectable const& object, std::wstring_view name) {
    return object && winrt::get_class_name(object) == name;
}

// Animation precedence preserves the target's local value and live native theme.
// The UI thread owns each Storyboard, stops it before dropping its weak target,
// and removes the Storyboard children so no animation clock pins a recycled view.
A::Storyboard MakeOverride(DependencyObject const& target, PCWSTR path,
                           Inspectable const& value) {
    A::ObjectAnimationUsingKeyFrames animation;
    animation.EnableDependentAnimation(true);
    animation.Duration(DurationHelper::FromTimeSpan(F::TimeSpan{0}));
    A::DiscreteObjectKeyFrame key;
    key.KeyTime(A::KeyTimeHelper::FromTimeSpan(F::TimeSpan{0}));
    key.Value(value);
    animation.KeyFrames().Append(key);
    A::Storyboard::SetTarget(animation, target);
    A::Storyboard::SetTargetProperty(animation, path);
    A::Storyboard storyboard;
    storyboard.FillBehavior(A::FillBehavior::HoldEnd);
    storyboard.Children().Append(animation);
    return storyboard;
}

struct Override {
    winrt::weak_ref<DependencyObject> target;
    DependencyProperty property{nullptr};
    std::wstring path;
    Inspectable value{nullptr};
    A::Storyboard animation{nullptr};
    int64_t changeToken = 0;
    bool registered = false;
    bool active = false;
    bool changing = false;
    bool failed = false;
    bool foreground = false;
    winrt::weak_ref<FrameworkElement> themeOwner;
    bool frozenLight = true, hasFrozenTheme = false;
    uint64_t settingsRevision = 0;
    std::optional<uint32_t> appliedColor;
    bool dirty = false;
    ULONGLONG repairWindow = 0;
    unsigned repairs = 0;
    bool background = false;
    BackgroundAppearance appliedBackground{};
    bool hasAppliedBackground = false;
    winrt::com_ptr<TaskbarMaterialBrush> material;
};



struct Root {
    winrt::weak_ref<FrameworkElement> content;
    winrt::weak_ref<FrameworkElement> themeOwner;
    winrt::event_token themeToken{};
    bool themeSubscribed = false;
};

struct UiContext {
    HWND window = nullptr;
    bool windowOwnsContext = false;
    std::vector<Root> roots;
    std::vector<winrt::weak_ref<FrameworkElement>> seeds;
    std::vector<std::shared_ptr<Override>> overrides;
    // Objects are logged only once while alive; weak refs do not pin templates.
    std::vector<winrt::weak_ref<FrameworkElement>> diagnosed;
    bool scanning = false;
    bool native = true;
    bool discoveryQueued = false;
    ULONGLONG lastScan = 0;
    winrt::weak_ref<FrameworkElement> walkThemeOwner;
    ULONGLONG lastPropertyRefresh = 0;
    ULONGLONG lastDiagnostic = 0;
};

void RevokeRoot(Root& root) {
    if (root.themeSubscribed)
        if (auto owner = root.themeOwner.get()) owner.ActualThemeChanged(root.themeToken);
    root.themeSubscribed = false;
}
void TrackRootTheme(UiContext& context, FrameworkElement const& content,
                    FrameworkElement const& owner) {
    Root root;
    root.content = winrt::make_weak(content);
    root.themeOwner = winrt::make_weak(owner);
    context.roots.push_back(std::move(root));
    auto& stored = context.roots.back();
    try {
        stored.themeToken = owner.ActualThemeChanged([window = context.window](auto&&, auto&&) {
            // Callback runs on this XAML thread; never selects taskbar state.
            if (!g_unloading && window) PostMessageW(window, kThemeRefresh, 0, 0);
        });
        stored.themeSubscribed = true;
    } catch (...) { LogException(L"ActualThemeChanged subscribe (scan fallback active)"); }
}
bool SelectPolicyLight(ThemeSettings const& settings, ElementTheme actual,
                       ThemeObservation const& observation) {
    if (settings.detection == ThemeDetection::ForceLight) return true;
    if (settings.detection == ThemeDetection::ForceDark) return false;
    if (actual == ElementTheme::Light) return true;
    if (actual == ElementTheme::Dark) return false;
    // Never choose taskbar theme from AppsUseLightTheme or the app UISettings color.
    return observation.systemKnown ? observation.systemLight : true;
}
std::optional<uint32_t> SelectedForeground(Override& item, ThemeSettings const& settings,
                                         ThemeObservation const& observation) {
    ElementTheme actual = ElementTheme::Default;
    try { if (auto owner = item.themeOwner.get()) actual = owner.ActualTheme(); }
    catch (...) { LogException(L"ActualTheme read; SystemUsesLightTheme fallback"); }
    bool light = SelectPolicyLight(settings, actual, observation);
    if (settings.follow || !item.hasFrozenTheme || item.settingsRevision != settings.revision) {
        item.frozenLight = light;
        item.hasFrozenTheme = true;
    }
    item.settingsRevision = settings.revision;
    switch (settings.foreground) {
        case ForegroundMode::Native: return std::nullopt;
        case ForegroundMode::White: return 0xFFFFFFFF;
        case ForegroundMode::Black: return 0xFF000000;
        case ForegroundMode::Custom: return settings.customColor;
        case ForegroundMode::Auto: return item.frozenLight ? settings.lightColor : settings.darkColor;
    }
    return std::nullopt;
}

// Scope checks are performed on the XAML thread and never match a Start host.
bool HasAncestor(DependencyObject current, std::wstring_view type) {
    for (int depth = 0; current && depth < 80; ++depth) {
        if (ClassIs(current, type)) return true;
        current = M::VisualTreeHelper::GetParent(current);
    }
    return false;
}

bool IsForegroundScope(FrameworkElement const& element) {
    bool scope = false;
    DependencyObject current = element;
    for (int depth = 0; current && depth < 80; ++depth) {
        auto type = winrt::get_class_name(current);
        if (type == L"Windows.UI.Xaml.Controls.Primitives.Popup" ||
            type == L"Taskbar.TaskbarHoverFlyout" ||
            type == L"SystemTray.NotifyIconView" ||
            type == L"SystemTray.ImageIconContent") return false;
        if (auto fe = current.try_as<FrameworkElement>()) {
            auto name = fe.Name();
            if (name == L"HoverFlyoutBackgroundControl" ||
                name == L"OverflowFlyoutBackgroundBorder") return false;
        }
        scope |= type == L"SystemTray.SystemTrayFrame" ||
                 type == L"Taskbar.TaskbarFrame";
        current = M::VisualTreeHelper::GetParent(current);
    }
    return scope;
}

// A tree is admitted only after finding an actual taskbar frame.
bool ContainsFrame(DependencyObject const& node, int depth = 48) {
    if (!node || depth <= 0) return false;
    if (ClassIs(node, L"Taskbar.TaskbarFrame")) return true;
    int count = M::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i)
        if (ContainsFrame(M::VisualTreeHelper::GetChild(node, i), depth - 1))
            return true;
    return false;
}

FrameworkElement FindFrame(DependencyObject const& node, int depth = 48) {
    if (!node || depth <= 0) return nullptr;
    if (ClassIs(node, L"Taskbar.TaskbarFrame")) return node.try_as<FrameworkElement>();
    int count = M::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i)
        if (auto frame = FindFrame(M::VisualTreeHelper::GetChild(node, i), depth - 1))
            return frame;
    return nullptr;
}

void AddOverride(UiContext& context, DependencyObject const& target,
                 DependencyProperty const& property, PCWSTR path,
                 Inspectable const& value) {
    for (auto const& item : context.overrides)
        if (item->property == property && item->target.get() == target) return;
    auto item = std::make_shared<Override>();
    item->target = winrt::make_weak(target);
    item->property = property;
    item->path = path;
    item->value = value;
    item->foreground = property != UIElement::OpacityProperty() &&
                       property != C::Panel::BackgroundProperty();
    if (auto owner = context.walkThemeOwner.get()) item->themeOwner = winrt::make_weak(owner);
    else if (auto fe = target.try_as<FrameworkElement>()) item->themeOwner = winrt::make_weak(fe);
    // Insert before registering so an allocation failure cannot leave a callback
    // in Explorer pointing to code which Windhawk may later unload.
    context.overrides.push_back(item);
    std::weak_ptr<Override> weakItem = item;
    try {
    item->changeToken = target.RegisterPropertyChangedCallback(property,
        [weakItem, window = context.window](DependencyObject const&, DependencyProperty const&) noexcept {
            auto item = weakItem.lock();
            if (g_unloading || !item || !item->active || item->changing || item->failed || item->dirty)
                return;
            // Never Stop/Begin a storyboard while XAML is notifying a property
            // change: its resource/state/animation collections can still be mid-write.
            item->dirty = true;
            if (window) PostMessageW(window, kPropertyRefresh, 0, 0);
        });
    item->registered = true;
    } catch (...) {
        context.overrides.pop_back();
        throw;
    }
    if (g_diagnostics) {
        auto fe = target.try_as<FrameworkElement>();
        DTT_LOG(L"PROPERTY %s#%s.%s", winrt::get_class_name(target).c_str(),
               fe ? fe.Name().c_str() : L"", path);
    }
}

void DiscoverElement(UiContext& context, FrameworkElement const& element) {
    bool firstDiagnostic = false;
    if (g_diagnostics) {
        bool known = std::any_of(context.diagnosed.begin(), context.diagnosed.end(),
            [&](auto const& weak) { return weak.get() == element; });
        if (!known) {
            firstDiagnostic = true;
            context.diagnosed.push_back(winrt::make_weak(element));
            DTT_LOG(L"TREE %s#%s", winrt::get_class_name(element).c_str(),
                   element.Name().c_str());
        }
    }
    // Exact background discovery taken from Dynamic Taskbar Transparency.
    auto name = element.Name();
    if (element.try_as<S::Rectangle>() &&
        (name == L"BackgroundFill" || name == L"BackgroundStroke") &&
        HasAncestor(element, L"Taskbar.TaskbarBackground") &&
        HasAncestor(element, L"Taskbar.TaskbarFrame") &&
        IsForegroundScope(element)) {
        AddOverride(context, element, UIElement::OpacityProperty(), L"Opacity",
                    winrt::box_value(0.0));
        auto parent = M::VisualTreeHelper::GetParent(element).try_as<C::Panel>();
        if (parent)
            AddOverride(context, parent, C::Panel::BackgroundProperty(), L"Background",
                M::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
        return;
    }
    if (!IsForegroundScope(element)) return;
    M::SolidColorBrush white(winrt::Windows::UI::Color{255, 255, 255, 255});
    if (element.try_as<C::TextBlock>()) {
        AddOverride(context, element, C::TextBlock::ForegroundProperty(),
                    L"Foreground", white);
    } else if (element.try_as<C::IconElement>()) {
        // FontIcon, SymbolIcon, PathIcon (also BitmapIcon monochrome mask).
        AddOverride(context, element, C::IconElement::ForegroundProperty(),
                    L"Foreground", white);
    } else if (auto shape = element.try_as<S::Shape>()) {
        // Only foreground paths, not arbitrary button/running-indicator rectangles.
        // Geometry is data; its owning Shape's Fill/Stroke are the brushes.
        if (shape.try_as<S::Path>() &&
            !HasAncestor(element, L"Taskbar.TaskbarBackground")) {
            if (shape.Fill()) AddOverride(context, shape, S::Shape::FillProperty(),
                                          L"Fill", white);
            if (shape.Stroke()) AddOverride(context, shape, S::Shape::StrokeProperty(),
                                            L"Stroke", white);
        }
    } else if (auto control = element.try_as<C::Control>()) {
        auto type = winrt::get_class_name(element);
        if (type == L"SystemTray.AdaptiveTextBlock" ||
            type == L"SystemTray.LanguageTextIconContent" ||
            type == L"SystemTray.TextIconContent" ||
            type == L"SystemTray.BatteryIconContent")
            AddOverride(context, control, C::Control::ForegroundProperty(),
                        L"Foreground", white);
    }
    // Raster/composition-backed images have no generic recolor API. Do not change
    // application images, mutate shared brush objects, or guess private layouts.
    if (element.try_as<C::Image>() && firstDiagnostic) {
        bool known = std::any_of(context.overrides.begin(), context.overrides.end(),
            [&](auto const& item) { return item->target.get() == element; });
        if (!known) DTT_LOG(L"UNSUPPORTED Image#%s: bitmap/composition color preserved",
                          name.c_str());
    }
}

void Walk(UiContext& context, DependencyObject const& node, int depth = 48) {
    if (!node || depth <= 0) return;
    if (auto fe = node.try_as<FrameworkElement>()) {
        try { DiscoverElement(context, fe); }
        catch (...) { LogException(L"DiscoverElement"); }
    }
    int count = M::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i)
        Walk(context, M::VisualTreeHelper::GetChild(node, i), depth - 1);
}

void StopOverride(Override& item) {
    item.active = false;
    item.dirty = false;
    if (!item.animation) return;
    item.animation.Stop();
    item.animation.Children().Clear();
    item.animation = nullptr;
}

BackgroundAppearance BackgroundForState(ThemeSettings const& settings, TaskbarState state) {
    BackgroundAppearance result;
    switch(state) {
        case TaskbarState::Desktop: result=settings.desktop; break;
        case TaskbarState::NormalWindow: result=settings.normalWindow; break;
        case TaskbarState::Maximized: result=settings.maximized; break;
        case TaskbarState::Start: result=settings.startMenu; break;
        case TaskbarState::Search: result=settings.search; break;
        case TaskbarState::TaskView: result=settings.taskView; break;
        case TaskbarState::PowerSaving: result=settings.powerSaving; break;
        case TaskbarState::Fallback: result=settings.fallback; break;
        case TaskbarState::Locked: case TaskbarState::HighContrast: return {};
    }
    if(!result.enabled) result=settings.fallback;
    if(!result.enabled) return {};
    result.transparency=std::clamp(result.transparency,0,100);
    result.blur=std::clamp(result.blur,0,100);
    return result;
}
uint32_t BackgroundTint(BackgroundAppearance const& background, Override const& item,
                        ThemeObservation const& observed) {
    uint32_t color=background.color;
    if(background.themeColor) {
        ElementTheme actual=ElementTheme::Default;
        try { if(auto owner=item.themeOwner.get()) actual=owner.ActualTheme(); } catch(...) {}
        bool light=actual==ElementTheme::Light ||
            (actual!=ElementTheme::Dark && (!observed.systemKnown || observed.systemLight));
        color=light ? 0xFFF3F3F3 : 0xFF202020;
    }
    if(background.mode==BackgroundMode::Opaque) return color|0xFF000000;
    unsigned alpha=((color>>24)*static_cast<unsigned>(100-background.transparency)+50)/100;
    return (color&0xFFFFFF)|(alpha<<24);
}
void SetOverrideValue(Override& item, Inspectable const& value) {
    StopOverride(item); item.value=value;
}
void ApplyState(UiContext& context, TaskbarState state, bool forceNative=false) {
    auto settings=GetThemeSettings();
    auto observation=GetThemeObservation();
    auto appearance=BackgroundForState(settings,state);
    bool native=forceNative || appearance.mode==BackgroundMode::Native || g_unloading;
    native |= std::any_of(context.overrides.begin(),context.overrides.end(),
        [](auto const& item){return item->failed;});
    for(auto const& pointer:context.overrides) {
        auto& item=*pointer;
        try {
            auto target=item.target.get();
            if(!target){StopOverride(item);continue;}
            bool stop=native;
            if(item.foreground) {
                // Foreground policy belongs to the transparent state. Native,
                // normal and material/solid panels retain current theme colors.
                auto color=appearance.mode==BackgroundMode::Transparent ?
                    SelectedForeground(item,settings,observation) : std::optional<uint32_t>{};
                stop|=!color;
                if(!stop && item.appliedColor!=color) {
                    SetOverrideValue(item,M::SolidColorBrush(ArgbColor(*color)));
                    item.appliedColor=color;
                }
            } else if(item.path==L"Opacity") {
                double opacity=0.0;
                if(appearance.mode==BackgroundMode::Normal) {
                    auto base=target.GetAnimationBaseValue(UIElement::OpacityProperty());
                    opacity=winrt::unbox_value_or<double>(base,1.0)*(100-appearance.transparency)/100.0;
                }
                if(!stop && (!item.value || winrt::unbox_value<double>(item.value)!=opacity))
                    SetOverrideValue(item,winrt::box_value(opacity));
            } else if(item.path==L"Background") {
                uint32_t tint=BackgroundTint(appearance,item,observation);
                auto key=appearance;key.color=tint;key.themeColor=false;
                if(!stop && (!item.hasAppliedBackground || item.appliedBackground!=key || !item.value)) {
                    StopOverride(item);
                    item.material=nullptr;
                    if(appearance.mode==BackgroundMode::Blur || appearance.mode==BackgroundMode::Acrylic) {
                        auto ui=target.as<UIElement>();
                        item.material=winrt::make_self<TaskbarMaterialBrush>(ui,static_cast<float>(appearance.blur),
                            ArgbColor(tint),appearance.mode==BackgroundMode::Acrylic);
                        item.value=item.material.as<M::Brush>();
                    } else item.value=M::SolidColorBrush(ArgbColor(tint));
                    item.appliedBackground=key;item.hasAppliedBackground=true;
                }
            }
            if(stop) {
                StopOverride(item);item.appliedColor.reset();
                if(item.path==L"Background") {item.value=nullptr;item.material=nullptr;item.hasAppliedBackground=false;}
                continue;
            }
            if(item.animation && item.dirty) {
                item.dirty=false;
                auto effective=target.GetValue(item.property);
                bool matches=item.path==L"Opacity" ?
                    winrt::unbox_value<double>(effective)==winrt::unbox_value<double>(item.value) :
                    effective==item.value;
                if(!matches) {
                    ULONGLONG now=GetTickCount64();
                    if(now-item.repairWindow>=1000){item.repairWindow=now;item.repairs=0;}
                    if(++item.repairs>4){item.failed=true;StopOverride(item);continue;}
                    StopOverride(item);
                }
            }
            if(item.animation || item.failed)continue;
            item.changing=true;item.active=true;
            item.animation=MakeOverride(target,item.path.c_str(),item.value);
            item.animation.Begin();item.animation.SkipToFill();item.changing=false;
        } catch(...) {
            LogException(L"Apply state appearance");item.changing=false;item.failed=true;
            try{StopOverride(item);}catch(...){LogException(L"Appearance Stop recovery");}
        }
    }
    if(!native && std::any_of(context.overrides.begin(),context.overrides.end(),
        [](auto const& item){return item->failed;})) {
        for(auto const& item:context.overrides)
            try{StopOverride(*item);}catch(...){LogException(L"Native safety fallback");}
        native=true;DTT_LOG(L"NATIVE_SAFETY property/effect conflict; UI thread retained native");
    }
    if(context.native!=native)DTT_LOG(L"UI thread=%lu native=%d state=%d mode=%d properties=%zu",
        GetCurrentThreadId(),native,static_cast<int>(state),static_cast<int>(appearance.mode),context.overrides.size());
    context.native=native;
}
// Compatibility helper for isolated property tests; production uses full state.
void Apply(UiContext& context,bool native){ApplyState(context,TaskbarState::Desktop,native);}

void RefreshTheme(UiContext& context) {
    auto settings = GetThemeSettings();
    if (settings.refreshResources) {
        // Expose live native resources briefly within the same UI message, then
        // apply the newest state + settings. No local values/resources are saved.
        for (auto const& item : context.overrides) {
            try { StopOverride(*item); } catch (...) { LogException(L"Theme Stop"); }
        }
    }
    if (g_diagnostics) {
        auto observed = GetThemeObservation();
        DTT_LOG(L"THEME_REFRESH systemLight=%d appsLight=%d mixed=%d native=%d follow=%d",
            observed.systemLight, observed.appsLight,
            observed.systemKnown && observed.appsKnown && observed.systemLight != observed.appsLight,
            g_native.load(), settings.follow);
        for (auto const& root : context.roots)
            if (auto owner = root.themeOwner.get())
                DTT_LOG(L"THEME_ACTUAL %s=%d", winrt::get_class_name(owner).c_str(),
                    static_cast<int>(owner.ActualTheme()));
    }
    ApplyState(context, g_taskbarState.load());
}

void ReleaseOverride(Override& item) {
    StopOverride(item);
    if (auto target = item.target.get(); target && item.registered)
        target.UnregisterPropertyChangedCallback(item.property, item.changeToken);
    item.registered = false;
}

void Scan(UiContext& context) {
    if (context.scanning || g_unloading) return;
    context.scanning = true;
    context.lastScan = GetTickCount64();
    context.discoveryQueued = false;
    try {
        for (auto const& seed : context.seeds) {
            auto element = seed.get();
            if (!element) continue;
            auto root = element.XamlRoot();
            auto content = root ? root.Content().try_as<FrameworkElement>() : nullptr;
            if (!content) continue;
            auto frame = FindFrame(content);
            if (!frame) continue;
            bool known = std::any_of(context.roots.begin(), context.roots.end(),
                [&](auto const& item) { return item.content.get() == content; });
            if (!known) {
                TrackRootTheme(context, content, frame);
                ++g_rootsDiscovered;
                DTT_LOG(L"ROOT admitted thread=%lu %s", GetCurrentThreadId(),
                       winrt::get_class_name(content).c_str());
            }
        }
        std::erase_if(context.seeds, [](auto const& seed) { return !seed.get(); });
        std::erase_if(context.roots, [](auto& root) {
            if (root.content.get() && root.themeOwner.get()) return false;
            RevokeRoot(root);
            return true;
        });
        std::erase_if(context.diagnosed, [](auto const& weak) { return !weak.get(); });
        // Stop animations on detached/recycled elements before they can pin old trees.
        for (auto& pointer : context.overrides) {
            auto& item = *pointer;
            auto target = item.target.get();
            auto fe = target ? target.try_as<FrameworkElement>() : nullptr;
            if (!fe || !fe.IsLoaded() || !IsForegroundScope(fe)) {
                ReleaseOverride(item);
                item.target = nullptr;
            }
        }
        std::erase_if(context.overrides, [](auto const& item) { return !item->target.get(); });
        for (auto const& root : context.roots)
            if (auto content = root.content.get()) {
                context.walkThemeOwner = root.themeOwner;
                Walk(context, content);
            }
        context.walkThemeOwner = nullptr;
        ApplyState(context, g_taskbarState.load());
        auto now = GetTickCount64();
        if (!context.lastDiagnostic || (g_diagnostics && now-context.lastDiagnostic >= 30000)) {
            size_t foreground=0, background=0, active=0, failed=0;
            for (auto const& item : context.overrides) {
                foreground += item->foreground; background += item->background;
                active += item->active; failed += item->failed;
            }
            DTT_LOG(L"LOAD_XAML_SCAN thread=%lu roots=%zu seeds=%zu properties=%zu foreground=%zu background=%zu active=%zu failed=%zu native=%d",
                GetCurrentThreadId(),context.roots.size(),context.seeds.size(),context.overrides.size(),
                foreground,background,active,failed,context.native);
            if (context.roots.empty()) DTT_LOG(L"LOAD_XAML_WAIT_ROOT: template seen but taskbar XamlRoot not ready; UI timer retries at 5s");
            else if (context.overrides.empty()) DTT_LOG(L"LOAD_STYLE_NO_TARGETS: taskbar root found, no supported properties; enable diagnostics for actual tree");
            if (active) ++g_stylePasses;
            context.lastDiagnostic=now;
        }
    } catch (...) { LogException(L"Scan"); }
    context.scanning = false;
}

LRESULT CALLBACK UiWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto context = reinterpret_cast<UiContext*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        context = static_cast<UiContext*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(context));
    }
    if (context) try {
        if (message == kCleanup) {
            KillTimer(hwnd, kFallbackTimer);
            KillTimer(hwnd, kDiscoveryTimer);
            KillTimer(hwnd, kPropertyTimer);
            Apply(*context, true);
            for (auto const& item : context->overrides) ReleaseOverride(*item);
            context->overrides.clear();
            for (auto& root : context->roots) RevokeRoot(root);
            context->roots.clear();
            context->seeds.clear();
            context->diagnosed.clear();
            DestroyWindow(hwnd);
            return 0;
        }
        if (message == WM_NCDESTROY) {
            {
                std::lock_guard lock(g_windowsMutex);
                std::erase(g_uiWindows, hwnd);
            }
            g_uiWindow = nullptr;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            if (context->windowOwnsContext) delete context;
        } else if (message == kPropertyRefresh) {
            MSG queued;
            while (PeekMessageW(&queued, hwnd, kPropertyRefresh, kPropertyRefresh, PM_REMOVE)) {}
            auto elapsed = GetTickCount64() - context->lastPropertyRefresh;
            if (!g_unloading && elapsed >= 50) {
                context->lastPropertyRefresh = GetTickCount64();
                ApplyState(*context, g_taskbarState.load());
            } else if (!g_unloading) {
                SetTimer(hwnd, kPropertyTimer, static_cast<UINT>(50 - elapsed), nullptr);
            }
            return 0;
        } else if (message == WM_TIMER && wParam == kPropertyTimer) {
            KillTimer(hwnd, kPropertyTimer);
            context->lastPropertyRefresh = GetTickCount64();
            if (!g_unloading) ApplyState(*context, g_taskbarState.load());
            return 0;
        } else if (message == kThemeRefresh) {
            // Multiple notifications share a message; newest state/settings win.
            MSG queued;
            while (PeekMessageW(&queued, hwnd, kThemeRefresh, kThemeRefresh, PM_REMOVE)) {}
            if (!g_unloading) RefreshTheme(*context);
            return 0;
        } else if (message == kApply) {
            // No state captured in messages: obsolete queued messages read the
            // newest state, so native/clear cannot race to overwrite each other.
            if (!g_unloading) ApplyState(*context, g_taskbarState.load());
            return 0;
        } else if (message == kDiscover) {
            auto elapsed = GetTickCount64() - context->lastScan;
            if (elapsed >= 250) Scan(*context);
            else SetTimer(hwnd, kDiscoveryTimer, static_cast<UINT>(250 - elapsed), nullptr);
            return 0;
        } else if (message == WM_TIMER &&
                   (wParam == kFallbackTimer || wParam == kDiscoveryTimer)) {
            if (wParam == kDiscoveryTimer) KillTimer(hwnd, kDiscoveryTimer);
            Scan(*context);
            return 0;
        }
    } catch (...) { LogException(L"UiWndProc"); }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// Only called with the `this` parameter of a real COM ABI method. An arbitrary
// private C++ implementation object is never accepted or offset into.
FrameworkElement FrameworkElementFromAbi(::IUnknown* object) {
    if (!object) return nullptr;
    FrameworkElement element{nullptr};
    HRESULT hr = object->QueryInterface(winrt::guid_of<FrameworkElement>(),
        reinterpret_cast<void**>(winrt::put_abi(element)));
    return SUCCEEDED(hr) ? element : nullptr;
}
void QueueFromAbi(::IUnknown* object) noexcept {
    struct CallGuard {
        CallGuard() { ++g_discoveryCalls; }
        ~CallGuard() { --g_discoveryCalls; }
    } guard;
    if (g_unloading) return;
    try {
        auto element = FrameworkElementFromAbi(object);
        if (!element) {
            if (g_diagnostics) DTT_LOG(L"LOAD_XAML_NO_INTERFACE: ABI object has no FrameworkElement");
            return;
        }
        if (!element.Dispatcher().HasThreadAccess()) {
            DTT_LOG(L"LOAD_XAML_WRONG_THREAD thread=%lu; skipped without touching tree",GetCurrentThreadId());
            return;
        }
        if (!g_uiWindow) {
            auto context = std::make_unique<UiContext>();
            HWND window = CreateWindowExW(0, kUiClass, L"", 0, 0, 0, 0, 0,
                HWND_MESSAGE, nullptr, g_module, context.get());
            if (!window) { DTT_LOG(L"UI window failed error=%lu", GetLastError()); return; }
            context->window = window;
            context->windowOwnsContext = true;
            context.release();
            g_uiWindow = window;
            {
                std::lock_guard lock(g_windowsMutex);
                g_uiWindows.push_back(window);
            }
            SetTimer(window, kFallbackTimer, 5000, nullptr);
        }
        auto context = reinterpret_cast<UiContext*>(GetWindowLongPtrW(g_uiWindow, GWLP_USERDATA));
        bool known = std::any_of(context->seeds.begin(), context->seeds.end(),
            [&](auto const& weak) { return weak.get() == element; });
        if (!known) context->seeds.push_back(winrt::make_weak(element));
        // Per-thread coalescing via a private window, no XAML dispatcher lambda
        // remains queued when Windhawk unloads the DLL.
        if (!context->discoveryQueued) {
            context->discoveryQueued = true;
            PostMessageW(g_uiWindow, kDiscover, 0, 0);
        }
    } catch (...) { LogException(L"QueueFromAbi"); }
}

// IFrameworkElementOverrides::OnApplyTemplate ABI returns HRESULT, and its this
// pointer is IInspectable-compatible. These are the produce<> ABI symbols, NOT
// implementation::OnApplyTemplate/UpdateVisualStates (whose this layout varies).
using TemplateAbi = HRESULT(STDMETHODCALLTYPE*)(::IUnknown*);
TemplateAbi g_frameTemplate = nullptr, g_buttonTemplate = nullptr, g_toggleTemplate = nullptr;
// Genuine IControlOverrides COM ABI, verified against the local public PDB.
// Optional hover entry points discover an already-built taskbar without forcing
// ApplyTemplate, calling a private implementation method, or restarting Explorer.
using PointerAbi = HRESULT(STDMETHODCALLTYPE*)(::IUnknown*,void*);
PointerAbi g_framePointer=nullptr,g_buttonPointer=nullptr,g_togglePointer=nullptr;
HRESULT STDMETHODCALLTYPE FramePointerHook(::IUnknown* self,void* args) { HRESULT result=g_framePointer(self,args); if(SUCCEEDED(result))QueueFromAbi(self); return result; }
HRESULT STDMETHODCALLTYPE ButtonPointerHook(::IUnknown* self,void* args) { HRESULT result=g_buttonPointer(self,args); if(SUCCEEDED(result))QueueFromAbi(self); return result; }
HRESULT STDMETHODCALLTYPE TogglePointerHook(::IUnknown* self,void* args) { HRESULT result=g_togglePointer(self,args); if(SUCCEEDED(result))QueueFromAbi(self); return result; }
HRESULT STDMETHODCALLTYPE FrameTemplateHook(::IUnknown* self) {
    HRESULT result = g_frameTemplate(self);
    if (SUCCEEDED(result)) QueueFromAbi(self);
    return result;
}
HRESULT STDMETHODCALLTYPE ButtonTemplateHook(::IUnknown* self) {
    HRESULT result = g_buttonTemplate(self);
    if (SUCCEEDED(result)) QueueFromAbi(self);
    return result;
}
HRESULT STDMETHODCALLTYPE ToggleTemplateHook(::IUnknown* self) {
    HRESULT result = g_toggleTemplate(self);
    if (SUCCEEDED(result)) QueueFromAbi(self);
    return result;
}

HMODULE TaskbarModule() {
    auto module = GetModuleHandleW(L"Taskbar.View.dll");
    return module ? module : GetModuleHandleW(L"ExplorerExtensions.dll");
}

// Resolve first, then register each hook at most once. Failed symbol lookup must
// never leave the "hooked" bit set, or a partially queued hook installed twice.
TemplateAbi g_frameAddress=nullptr, g_buttonAddress=nullptr, g_toggleAddress=nullptr;
PointerAbi g_framePointerAddress=nullptr,g_buttonPointerAddress=nullptr,g_togglePointerAddress=nullptr;
bool g_frameRegistered=false, g_buttonRegistered=false, g_toggleRegistered=false;
bool g_framePointerRegistered=false,g_buttonPointerRegistered=false,g_togglePointerRegistered=false;
bool g_hookOperationsPending=false;
HMODULE g_taskbarModuleReference=nullptr;

bool HookTaskbar(HMODULE module) {
    if (!g_frameAddress) {
        WindhawkUtils::SYMBOL_HOOK symbols[] = {
            {{LR"(?OnApplyTemplate@?$produce@UTaskbarFrame@implementation@Taskbar@winrt@@UIFrameworkElementOverrides@Xaml@UI@Windows@4@@impl@winrt@@UEAAHXZ)"},
                &g_frameAddress, nullptr},
            {{LR"(?OnApplyTemplate@?$produce@UTaskListButton@implementation@Taskbar@winrt@@UIFrameworkElementOverrides@Xaml@UI@Windows@4@@impl@winrt@@UEAAHXZ)"},
                &g_buttonAddress, nullptr, true},
            {{LR"(?OnApplyTemplate@?$produce@UExperienceToggleButton@implementation@Taskbar@winrt@@UIFrameworkElementOverrides@Xaml@UI@Windows@4@@impl@winrt@@UEAAHXZ)"},
                &g_toggleAddress, nullptr, true},
            {{LR"(?OnPointerEntered@?$produce@UTaskbarFrame@implementation@Taskbar@winrt@@UIControlOverrides@Controls@Xaml@UI@Windows@4@@impl@winrt@@UEAAHPEAX@Z)"},
                &g_framePointerAddress, nullptr, true},
            {{LR"(?OnPointerEntered@?$produce@UTaskListButton@implementation@Taskbar@winrt@@UIControlOverrides@Controls@Xaml@UI@Windows@4@@impl@winrt@@UEAAHPEAX@Z)"},
                &g_buttonPointerAddress, nullptr, true},
            {{LR"(?OnPointerEntered@?$produce@UExperienceToggleButton@implementation@Taskbar@winrt@@UIControlOverrides@Controls@Xaml@UI@Windows@4@@impl@winrt@@UEAAHPEAX@Z)"},
                &g_togglePointerAddress, nullptr, true},
        };
        WH_HOOK_SYMBOLS_OPTIONS options{};
        options.optionsSize=sizeof(options);
        options.noUndecoratedSymbols=TRUE; // Inputs above are MSVC decorated names.
        options.onlineCacheUrl=nullptr;   // Preserve Windhawk default cache.
        DTT_LOG(L"LOAD_SYMBOL_BEGIN module=%p decorated=1; asynchronous symbol download may take time",module);
        bool resolved=WindhawkUtils::HookSymbols(module,symbols,ARRAYSIZE(symbols),&options);
        if (!resolved || !g_frameAddress) {
            // Resolution-only call has queued NO hooks, so retry is safe.
            g_frameAddress=nullptr; g_buttonAddress=nullptr; g_toggleAddress=nullptr;
            g_framePointerAddress=nullptr;g_buttonPointerAddress=nullptr;g_togglePointerAddress=nullptr;
            DTT_LOG(L"LOAD_SYMBOL_FAILED: unavailable PDB/cache or incompatible ABI symbols; loaded but native; backoff retry");
            return false;
        }
        DTT_LOG(L"LOAD_SYMBOL_OK frame=%p button=%p toggle=%p",g_frameAddress,g_buttonAddress,g_toggleAddress);
        DTT_LOG(L"LOAD_SYMBOL_OPTIONAL_HOVER frame=%p button=%p toggle=%p",g_framePointerAddress,g_buttonPointerAddress,g_togglePointerAddress);
    }
    if (g_unloading) return false;
    auto install=[](PCWSTR name,auto address,auto hook,auto* original,bool& registered) {
        if (!address || registered) return true;
        if (!WindhawkUtils::SetFunctionHook(address,hook,original)) {
            DTT_LOG(L"LOAD_HOOK_REGISTER_FAILED name=%s error=%lu; will retry",name,GetLastError());
            return false;
        }
        registered=true; g_hookOperationsPending=true;
        DTT_LOG(L"LOAD_HOOK_REGISTER_OK name=%s",name);
        return true;
    };
    bool frame=install(L"TaskbarFrame",g_frameAddress,FrameTemplateHook,&g_frameTemplate,g_frameRegistered);
    bool button=install(L"TaskListButton",g_buttonAddress,ButtonTemplateHook,&g_buttonTemplate,g_buttonRegistered);
    bool toggle=install(L"ExperienceToggleButton",g_toggleAddress,ToggleTemplateHook,&g_toggleTemplate,g_toggleRegistered);
    bool framePointer=install(L"TaskbarFrame.Hover",g_framePointerAddress,FramePointerHook,&g_framePointer,g_framePointerRegistered);
    bool buttonPointer=install(L"TaskListButton.Hover",g_buttonPointerAddress,ButtonPointerHook,&g_buttonPointer,g_buttonPointerRegistered);
    bool togglePointer=install(L"ExperienceToggleButton.Hover",g_togglePointerAddress,TogglePointerHook,&g_togglePointer,g_togglePointerRegistered);
    if (g_unloading) return false;
    if (g_hookOperationsPending) {
        if (!Wh_ApplyHookOperations()) {
            DTT_LOG(L"LOAD_HOOK_APPLY_FAILED error=%lu; registrations retained; only apply will be retried",GetLastError());
            return false;
        }
        g_hookOperationsPending=false;
        DTT_LOG(L"LOAD_HOOK_APPLY_OK");
    }
    if (!frame || !button || !toggle || !framePointer || !buttonPointer || !togglePointer) return false;
    g_symbolsHooked=true;
    DTT_LOG(L"LOAD_HOOK_READY; taskbar UI discovery via genuine OnApplyTemplate/optional OnPointerEntered ABI; move pointer across taskbar");
    return true;
}

void DependencyMain() noexcept {
    unsigned attempt=0;
    while (!g_unloading) {
        unsigned delay=30000;
        try {
            if (!g_symbolsHooked) {
                HMODULE module=TaskbarModule(), retained=nullptr;
                if (!module || !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                        reinterpret_cast<PCWSTR>(module),&retained)) {
                    DTT_LOG(L"LOAD_WAIT_MODULE attempt=%u Taskbar.View.dll/ExplorerExtensions.dll not loaded; no forced LoadLibrary",attempt+1);
                } else {
                    // Hold a normal module reference for resolution and hooked code.
                    // A successful registration retains it until Uninit has removed hooks.
                    if (!g_taskbarModuleReference) g_taskbarModuleReference=retained;
                    else FreeLibrary(retained);
                    wchar_t path[MAX_PATH]{}; GetModuleFileNameW(g_taskbarModuleReference,path,ARRAYSIZE(path));
                    DTT_LOG(L"LOAD_MODULE_READY attempt=%u path=%s",attempt+1,path);
                    HookTaskbar(g_taskbarModuleReference);
                }
                delay=DependencyRetryDelay(attempt++);
            } else if (!g_rootsDiscovered) {
                DTT_LOG(L"LOAD_XAML_WAIT_TEMPLATE: hooks active but no taskbar root; move pointer across taskbar for optional ABI discovery; no forced template recreation");
            } else {
                DTT_LOG(L"LOAD_STATUS hooks=1 rootsEver=%u stylePasses=%u (discovery is not visual acceptance)",g_rootsDiscovered.load(),g_stylePasses.load());
            }
        } catch (...) { LogException(L"LOAD_DEPENDENCY_EXCEPTION (native fallback, retry)"); delay=DependencyRetryDelay(attempt++); }
        if (g_unloading) break;
        DWORD result=WaitForSingleObject(g_dependencyWake,delay);
        if (result==WAIT_FAILED) { DTT_LOG(L"LOAD_RETRY_WAIT_FAILED error=%lu",GetLastError()); break; }
    }
    DTT_LOG(L"LOAD_DEPENDENCY_STOPPED");
}

std::wstring WindowClass(HWND hwnd) {
    wchar_t name[128]{};
    GetClassNameW(hwnd, name, ARRAYSIZE(name));
    return name;
}
bool IsTaskbar(std::wstring_view type) {
    return type == L"Shell_TrayWnd" || type == L"Shell_SecondaryTrayWnd";
}
bool Cloaked(HWND hwnd) {
    DWORD value = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &value, sizeof(value))) && value;
}
std::wstring ProcessName(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return {};
    wchar_t path[2048]{};
    DWORD length = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &length);
    CloseHandle(process);
    if (!ok) return {};
    std::wstring name(path, length);
    auto slash = name.find_last_of(L"\\/");
    if (slash != std::wstring::npos) name.erase(0, slash + 1);
    std::transform(name.begin(), name.end(), name.begin(), towlower);
    return name;
}
bool UserCandidate(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || Cloaked(hwnd) ||
        GetAncestor(hwnd, GA_ROOT) != hwnd) return false;
    auto type = WindowClass(hwnd);
    if (IsTaskbar(type) || type == L"Progman" || type == L"WorkerW" ||
        type == L"#32768" || type == L"SysShadow" ||
        type == L"NotifyIconOverflowWindow" || type == L"TaskListThumbnailWnd" ||
        type == L"MultitaskingViewFrame" || type == L"XamlExplorerHostIslandWindow" ||
        type == L"Windows.UI.Composition.DesktopWindowContentBridge" ||
        type == L"Microsoft.UI.Content.PopupWindowSiteBridge") return false;
    auto exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
    auto process = ProcessName(hwnd);
    if (process == L"startmenuexperiencehost.exe" || process == L"searchhost.exe" ||
        process == L"shellexperiencehost.exe" || process == L"shellhost.exe" ||
        process == L"textinputhost.exe" || process == L"lockapp.exe") return false;
    if (process == L"explorer.exe" && type != L"CabinetWClass" && type != L"ExploreWClass")
        return false;
    RECT rect{};
    return GetWindowRect(hwnd, &rect) && rect.right - rect.left >= 120 &&
           rect.bottom - rect.top >= 80;
}

bool IsTaskViewWindow(HWND hwnd) {
    auto type = WindowClass(hwnd);
    if (type == L"MultitaskingViewFrame") return true;
    // Confirmed upstream taskbar-background-helper fallback: Explorer thread
    // description + immersive app-chrome window band. Never match other apps.
    DWORD pid = 0;
    DWORD tid = GetWindowThreadProcessId(hwnd, &pid);
    if (!tid || pid != GetCurrentProcessId()) return false;
    using GetWindowBand_t = BOOL(WINAPI*)(HWND, DWORD*);
    static auto getBand = reinterpret_cast<GetWindowBand_t>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetWindowBand"));
    if (getBand) {
        DWORD band = 0;
        if (!getBand(hwnd, &band) || band != 5) return false;
    }
    HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
    if (!thread) return false;
    PWSTR description = nullptr;
    bool matches = SUCCEEDED(GetThreadDescription(thread, &description)) && description &&
                   wcscmp(description, L"MultitaskingView") == 0;
    if (description) LocalFree(description);
    CloseHandle(thread);
    return matches;
}
struct Detection {
    bool maximized = false, startWindow = false, searchWindow = false, taskViewWindow = false;
    unsigned candidates = 0;
};
bool MaximizedForPolicy(HWND hwnd) {
    if (!UserCandidate(hwnd)) return false;
    if (IsZoomed(hwnd)) return true;
    RECT rect{};
    MONITORINFO monitor{sizeof(monitor)};
    auto display = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
    if (!display || !GetMonitorInfoW(display, &monitor) || !GetWindowRect(hwnd, &rect))
        return false;
    auto m = monitor.rcMonitor;
    return rect.left <= m.left + 2 && rect.top <= m.top + 2 &&
           rect.right >= m.right - 2 && rect.bottom >= m.bottom - 2;
}
struct DetectionContext {
    Detection result;
    IVirtualDesktopManager* desktops;
};
Detection DetectWindows(IVirtualDesktopManager* desktops = nullptr) {
    DetectionContext context{{}, desktops};
    EnumWindows([](HWND hwnd, LPARAM parameter) -> BOOL {
        auto& context = *reinterpret_cast<DetectionContext*>(parameter);
        auto& detection = context.result;
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || Cloaked(hwnd)) return TRUE;
        auto name = ProcessName(hwnd);
        RECT rect{};
        if (GetWindowRect(hwnd, &rect) && rect.right - rect.left >= 60 && rect.bottom - rect.top >= 60) {
            detection.startWindow |= name == L"startmenuexperiencehost.exe";
            detection.searchWindow |= name == L"searchhost.exe";
            detection.taskViewWindow |= IsTaskViewWindow(hwnd);
        }
        if (detection.maximized || !UserCandidate(hwnd)) return TRUE;
        if (context.desktops) {
            BOOL current = TRUE;
            if (SUCCEEDED(context.desktops->IsWindowOnCurrentVirtualDesktop(hwnd, &current)) && !current)
                return TRUE;
        }
        ++detection.candidates;
        detection.maximized |= MaximizedForPolicy(hwnd);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&context));
    return context.result;
}

PCWSTR StateName(TaskbarState state) {
    switch(state) {
        case TaskbarState::Desktop:return L"DESKTOP";
        case TaskbarState::NormalWindow:return L"NORMAL_WINDOW";
        case TaskbarState::Maximized:return L"MAXIMIZED";
        case TaskbarState::Start:return L"START";
        case TaskbarState::Search:return L"SEARCH";
        case TaskbarState::TaskView:return L"TASK_VIEW";
        case TaskbarState::PowerSaving:return L"POWER_SAVING";
        case TaskbarState::Locked:return L"LOCKED";
        case TaskbarState::HighContrast:return L"HIGH_CONTRAST";
        case TaskbarState::Fallback:return L"FALLBACK";
    }
    return L"UNKNOWN";
}
void PublishState(TaskbarState state, PCWSTR reason) {
    bool changed=g_taskbarState.exchange(state)!=state;
    auto appearance=BackgroundForState(GetThemeSettings(),state);
    g_native=appearance.mode==BackgroundMode::Native;
    if(!changed)return;
    DTT_LOG(L"STATE %s reason=%s mode=%d",StateName(state),reason,static_cast<int>(appearance.mode));
    BroadcastUi(kApply);
}
void Publish(bool native, PCWSTR reason) {
    // Emergency native requests cannot accidentally select a customized fallback.
    PublishState(native ? TaskbarState::Locked : TaskbarState::Desktop,reason);
}
void SignalDetection(bool immediate = false) noexcept {
    auto window = g_workerWindow.load();
    if (window && !g_unloading && (immediate || !g_eventPosted.exchange(true)))
        PostMessageW(window, kDetect, immediate, 0);
}

struct LauncherSink : winrt::implements<LauncherSink, IAppVisibilityEvents> {
    HRESULT STDMETHODCALLTYPE AppVisibilityOnMonitorChanged(
        HMONITOR, MONITOR_APP_VISIBILITY, MONITOR_APP_VISIBILITY) noexcept override {
        ExternalCallbackGuard guard;
        SignalDetection(true);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE LauncherVisibilityChange(BOOL visible) noexcept override {
        ExternalCallbackGuard guard;
        if (g_unloading) return S_OK;
        g_launcherOpen = visible != FALSE;
        // Do not log, acquire a UI registry mutex or query Shell/XAML from a
        // COM notification. The worker resolves the newest state on its queue.
        if (visible) g_shellNativeRequested = true;
        SignalDetection(true);
        return S_OK;
    }
};

// Minimal, slot-accurate ABI from TranslucentTB's checked-in ShellViewCoordinator.idl.
// No WindowsUdk headers, winmd generation, extra DLL or companion mod is required.
MIDL_INTERFACE("BC6FD3A4-3561-5629-ABDD-170AB289EF2F") DttShellView : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE AddShow(IUnknown*, winrt::event_token*) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveShow(winrt::event_token) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddDismiss(IUnknown*, winrt::event_token*) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveDismiss(winrt::event_token) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReportVisibility(int) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryShow(::IInspectable*, ::IInspectable**) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryDismiss(::IInspectable*, ::IInspectable**) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddVisibility(IUnknown*, winrt::event_token*) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveVisibility(winrt::event_token) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVisibility(int*) = 0;
};
__CRT_UUID_DECL(DttShellView, 0xbc6fd3a4, 0x3561, 0x5629, 0xab, 0xdd, 0x17, 0x0a, 0xb2, 0x89, 0xef, 0x2f)
MIDL_INTERFACE("BB8446E1-05D0-5510-80DD-96E9EBD9E006") DttShellViewFactory : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE CreateInstance(int, ::IInspectable**) = 0;
};
__CRT_UUID_DECL(DttShellViewFactory, 0xbb8446e1, 0x05d0, 0x5510, 0x80, 0xdd, 0x96, 0xe9, 0xeb, 0xd9, 0xe0, 0x06)
// UUIDv5 of the WinRT TypedEventHandler<ShellViewCoordinator,IInspectable> signature.
MIDL_INTERFACE("FBB1C339-33C5-5B33-A6F2-D3564BEAEFE0") DttVisibilityHandler : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Invoke(::IInspectable*, ::IInspectable*) = 0;
};
__CRT_UUID_DECL(DttVisibilityHandler, 0xfbb1c339, 0x33c5, 0x5b33, 0xa6, 0xf2, 0xd3, 0x56, 0x4b, 0xea, 0xef, 0xe0)
struct ShellVisibilityHandler : winrt::implements<ShellVisibilityHandler, DttVisibilityHandler> {
    HRESULT STDMETHODCALLTYPE Invoke(::IInspectable*, ::IInspectable*) noexcept override {
        ExternalCallbackGuard guard;
        if (g_unloading) return S_OK;
        // Even the Visibility getter can reenter the Shell during its own event.
        // Re-query only from WorkerWndProc after this notification has returned.
        g_shellNativeRequested = true;
        SignalDetection(true);
        return S_OK;
    }
};
struct ShellViewWatch {
    winrt::com_ptr<DttShellView> view;
    winrt::com_ptr<ShellVisibilityHandler> handler;
    winrt::event_token token{};
    bool subscribed = false;
    HRESULT Open(int kind) noexcept {
        try {
            winrt::com_ptr<DttShellViewFactory> factory;
            winrt::hstring name(L"WindowsUdk.UI.Shell.ShellViewCoordinator");
            HRESULT hr = RoGetActivationFactory(reinterpret_cast<HSTRING>(winrt::get_abi(name)),
                __uuidof(DttShellViewFactory), factory.put_void());
            if (FAILED(hr)) return hr;
            winrt::com_ptr<::IInspectable> instance;
            hr = factory->CreateInstance(kind, instance.put());
            if (FAILED(hr)) return hr;
            hr = instance->QueryInterface(__uuidof(DttShellView), view.put_void());
            if (FAILED(hr)) return hr;
            handler = winrt::make_self<ShellVisibilityHandler>();
            hr = view->AddVisibility(handler.get(), &token);
            subscribed = SUCCEEDED(hr);
            return hr;
        } catch (...) { return winrt::to_hresult(); }
    }
    bool Visible(bool fallback) const {
        int value = 1;
        return view && SUCCEEDED(view->GetVisibility(&value)) ? value == 0 : fallback;
    }
    void Close() noexcept {
        if (view && subscribed) view->RemoveVisibility(token);
        subscribed = false;
        view = nullptr;
        handler = nullptr;
    }
};

struct WorkerContext {
    winrt::com_ptr<IAppVisibility> visibility;
    winrt::com_ptr<LauncherSink> sink;
    winrt::com_ptr<IVirtualDesktopManager> desktops;
    DWORD cookie = 0;
    bool locked = false;
    bool clearPending = false;
    bool visibilityWarning = false;
    bool eventTimerPending = false;
    winrt::Windows::UI::ViewManagement::UISettings uiSettings{nullptr};
    winrt::event_token colorsToken{};
    bool colorsSubscribed = false;
    HKEY themeKey = nullptr;
    HANDLE themeEvent = nullptr;
    ShellViewWatch searchView, taskView;
    HWND window = nullptr;
    std::vector<HWINEVENTHOOK> hooks;
    bool sessionRegistered = false;
    winrt::event_token powerToken{};
    bool powerSubscribed=false;
    ~WorkerContext() {
        if(powerSubscribed)winrt::Windows::System::Power::PowerManager::EnergySaverStatusChanged(powerToken);
        for (auto hook : hooks) UnhookWinEvent(hook);
        searchView.Close(); taskView.Close();
        if (visibility && cookie) visibility->Unadvise(cookie);
        if (colorsSubscribed && uiSettings) uiSettings.ColorValuesChanged(colorsToken);
        if (themeKey) RegCloseKey(themeKey);
        if (themeEvent) CloseHandle(themeEvent);
        if (window) {
            if (sessionRegistered) WTSUnRegisterSessionNotification(window);
            if (IsWindow(window)) DestroyWindow(window);
            HWND expected = window;
            g_workerWindow.compare_exchange_strong(expected, nullptr);
        }
    }
};

// Shared by live detector and isolated state/theme matrix tests.
TaskbarState ResolveState(bool start, bool search, bool taskView, bool maximized,
                          bool peek, bool locked, bool highContrast,
                          bool hasWindow=false, bool powerSaving=false, bool valid=true) {
    if (locked) return TaskbarState::Locked;
    if (highContrast) return TaskbarState::HighContrast;
    if (start) return TaskbarState::Start;
    if (search) return TaskbarState::Search;
    if (taskView) return TaskbarState::TaskView;
    if (!valid) return TaskbarState::Fallback;
    if (powerSaving) return TaskbarState::PowerSaving;
    if (maximized && !peek) return TaskbarState::Maximized;
    if (hasWindow && !peek) return TaskbarState::NormalWindow;
    return TaskbarState::Desktop;
}

void DetectAndPublish(HWND window, WorkerContext& context, bool commitClear = false) {
    auto windows = DetectWindows(context.desktops.get());
    BOOL launcher = FALSE;
    bool start = windows.startWindow || (context.cookie && g_launcherOpen);
    if (context.visibility) {
        HRESULT hr = context.visibility->IsLauncherVisible(&launcher);
        if (SUCCEEDED(hr)) start |= launcher != FALSE;
        else if (!context.visibilityWarning) {
            context.visibilityWarning = true;
            DTT_LOG(L"IsLauncherVisible failed %08X; visible Start-host fallback", hr);
        }
    }
    HIGHCONTRASTW contrast{sizeof(contrast)};
    bool highContrast = SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
                        (contrast.dwFlags & HCF_HIGHCONTRASTON);
    SYSTEM_POWER_STATUS power{};
    bool powerSaving = GetSystemPowerStatus(&power) && power.SystemStatusFlag == 1;
    try { powerSaving=winrt::Windows::System::Power::PowerManager::EnergySaverStatus()==
        winrt::Windows::System::Power::EnergySaverStatus::On; } catch(...) {}
    bool search = context.searchView.Visible(windows.searchWindow);
    bool taskView = context.taskView.Visible(windows.taskViewWindow);
    auto state = ResolveState(start, search, taskView, windows.maximized,
                              g_peeking, context.locked, highContrast,
                              windows.candidates!=0,powerSaving);
    bool important=state!=TaskbarState::Desktop && state!=TaskbarState::NormalWindow;
    if (important) {
        KillTimer(window, kClearTimer);
        context.clearPending = false;
        PublishState(state,L"WINDOW_SHELL_POWER_EVENT");
    } else if (commitClear || (g_taskbarState==TaskbarState::Desktop ||
                              g_taskbarState==TaskbarState::NormalWindow)) {
        context.clearPending = false;
        PublishState(state,g_peeking ? L"PEEK" : L"WINDOW_ENUMERATION");
    } else if (!context.clearPending) {
        context.clearPending = true;
        SetTimer(window, kClearTimer, 120, nullptr);
    }
}

LRESULT CALLBACK WorkerWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    auto context = reinterpret_cast<WorkerContext*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        context = static_cast<WorkerContext*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(context));
    }
    if (context) try {
        if (message == kStop) { PostQuitMessage(0); return 0; }
        if (message == kObserveTheme) {
            MSG queued;
            while (PeekMessageW(&queued, hwnd, kObserveTheme, kObserveTheme, PM_REMOVE)) {}
            ObserveTheme(context->uiSettings);
            return 0;
        }
        if (message == kDetect) {
            g_eventPosted = false;
            if (g_shellNativeRequested.exchange(false)) Publish(true, L"SHELL_NOTIFICATION");
            if (wParam) DetectAndPublish(hwnd, *context);
            else if (!context->eventTimerPending) {
                context->eventTimerPending = true;
                SetTimer(hwnd, kEventTimer, 30, nullptr);
            }
            return 0;
        }
        if (message == WM_WTSSESSION_CHANGE) {
            if (wParam == WTS_SESSION_LOCK || wParam == WTS_SESSION_LOGOFF)
                context->locked = true;
            if (wParam == WTS_SESSION_UNLOCK || wParam == WTS_SESSION_LOGON)
                context->locked = false;
            DetectAndPublish(hwnd, *context);
            return 0;
        }
        if (message == WM_DISPLAYCHANGE || message == WM_SETTINGCHANGE ||
            message == WM_THEMECHANGED || message == WM_SYSCOLORCHANGE ||
            message == WM_DWMCOLORIZATIONCOLORCHANGED || message == WM_POWERBROADCAST) {
            RequestThemeObservation();
            SignalDetection(true);
        }
        if (message == WM_TIMER) {
            if (wParam != kFallbackTimer) KillTimer(hwnd, wParam);
            if (wParam == kEventTimer) context->eventTimerPending = false;
            if (wParam == kFallbackTimer) ObserveTheme(context->uiSettings, false);
            DetectAndPublish(hwnd, *context, wParam == kClearTimer);
            return 0;
        }
    } catch (...) { LogException(L"WorkerWndProc"); PublishState(TaskbarState::Fallback, L"DETECTION_ERROR"); }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void CALLBACK EventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG object,
                        LONG, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_PEEKSTART || event == EVENT_SYSTEM_PEEKEND) {
        g_peeking = event == EVENT_SYSTEM_PEEKSTART;
        SignalDetection(true);
    } else if (event == EVENT_SYSTEM_FOREGROUND ||
               event == EVENT_SYSTEM_MINIMIZESTART || event == EVENT_SYSTEM_MINIMIZEEND ||
               (hwnd && object == OBJID_WINDOW)) {
        SignalDetection();
    }
}

void WorkerMain() noexcept {
    HRESULT com = RoInitialize(RO_INIT_SINGLETHREADED);
    DTT_LOG(L"LOAD_WORKER_COM thread=%lu hr=%08X (not a taskbar XAML thread)",GetCurrentThreadId(),com);
    if (FAILED(com)) {
        DTT_LOG(L"LOAD_WORKER_COM_FAILED; loaded degraded; no XAML initialization attempted");
        if (g_ready) SetEvent(g_ready);
        return;
    }
    bool readyAnnounced = false;
    try {
        WorkerContext context;
        HRESULT hr = CoCreateInstance(CLSID_AppVisibility, nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IAppVisibility), context.visibility.put_void());
        if (SUCCEEDED(hr)) {
            BOOL initial = FALSE;
            if (SUCCEEDED(context.visibility->IsLauncherVisible(&initial)))
                g_launcherOpen = initial != FALSE;
            context.sink = winrt::make_self<LauncherSink>();
            hr = context.visibility->Advise(context.sink.get(), &context.cookie);
        }
        DTT_LOG(L"IAppVisibility/Advise hr=%08X (fallback: visible Start host)", hr);
        hr = CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IVirtualDesktopManager), context.desktops.put_void());
        DTT_LOG(L"IVirtualDesktopManager hr=%08X (fallback: DWM cloaking)", hr);
        HWND window = CreateWindowExW(0, kWorkerClass, L"", WS_POPUP,
            0, 0, 0, 0, nullptr, nullptr, g_module, &context);
        g_workerWindow = window;
        context.window = window;
        if (!window) { DTT_LOG(L"Worker window failed %lu", GetLastError()); SetEvent(g_ready); }
        else {
            try {
                context.powerToken=winrt::Windows::System::Power::PowerManager::EnergySaverStatusChanged(
                    [](auto&&,auto&&){ExternalCallbackGuard guard;SignalDetection(true);});
                context.powerSubscribed=true;
            }catch(...){LogException(L"EnergySaverStatusChanged (Win32/scan fallback active)");}
            try {
                context.uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
                context.colorsToken = context.uiSettings.ColorValuesChanged([](auto&&, auto&&) {
                    ExternalCallbackGuard guard;
                    RequestThemeObservation();
                });
                context.colorsSubscribed = true;
            } catch (...) { LogException(L"UISettings.ColorValuesChanged subscribe"); }
            ObserveTheme(context.uiSettings);
            // RegNotify is supplementary; ActualTheme and UISettings remain the
            // authoritative live XAML/app notifications. Never write this key.
            if (RegOpenKeyExW(HKEY_CURRENT_USER,
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                    0, KEY_QUERY_VALUE | KEY_NOTIFY, &context.themeKey) == ERROR_SUCCESS) {
                context.themeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
                if (context.themeEvent && RegNotifyChangeKeyValue(context.themeKey, FALSE,
                    REG_NOTIFY_CHANGE_LAST_SET, context.themeEvent, TRUE) != ERROR_SUCCESS) {
                    CloseHandle(context.themeEvent); context.themeEvent = nullptr;
                }
            }
            HRESULT searchHr = context.searchView.Open(1); // IDL ShellView::Search
            HRESULT taskHr = context.taskView.Open(3);     // IDL ShellView::TaskView
            DTT_LOG(L"ShellViewCoordinator Search=%08X TaskView=%08X; window fallback enabled",
                    searchHr, taskHr);
            context.sessionRegistered = WTSRegisterSessionNotification(window, NOTIFY_FOR_THIS_SESSION);
            const DWORD ranges[][2] = {
                {EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE},
                {EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE},
                {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
                {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
                {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
                {EVENT_SYSTEM_PEEKSTART, EVENT_SYSTEM_PEEKEND},
            };
            for (auto const& range : ranges) {
                auto hook = SetWinEventHook(range[0], range[1], nullptr, EventProc,
                    0, 0, WINEVENT_OUTOFCONTEXT);
                if (hook) context.hooks.push_back(hook);
                else DTT_LOG(L"WinEvent hook %lu failed %lu", range[0], GetLastError());
            }
            SetTimer(window, kFallbackTimer, 5000, nullptr);
            DetectAndPublish(window, context);
            SetEvent(g_ready);
            readyAnnounced = true;
            DTT_LOG(L"LOAD_WORKER_READY thread=%lu; window/theme/shell detection active",GetCurrentThreadId());
            bool running = !g_unloading.load();
            while (running) {
                DWORD count = context.themeEvent ? 1 : 0;
                DWORD result = MsgWaitForMultipleObjectsEx(count,
                    count ? &context.themeEvent : nullptr, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
                if (count && result == WAIT_OBJECT_0) {
                    ObserveTheme(context.uiSettings);
                    if (RegNotifyChangeKeyValue(context.themeKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                            context.themeEvent, TRUE) != ERROR_SUCCESS) {
                        CloseHandle(context.themeEvent); context.themeEvent = nullptr;
                    }
                }
                MSG message;
                while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                    if (message.message == WM_QUIT) { running = false; break; }
                    TranslateMessage(&message);
                    DispatchMessageW(&message);
                }
                if (result == WAIT_FAILED) { DTT_LOG(L"Worker message wait failed %lu", GetLastError()); break; }
            }
            for (auto hook : context.hooks) UnhookWinEvent(hook);
            context.hooks.clear();
            context.searchView.Close(); context.taskView.Close();
            if(context.powerSubscribed)winrt::Windows::System::Power::PowerManager::EnergySaverStatusChanged(context.powerToken);
            context.powerSubscribed=false;
            if (context.colorsSubscribed) context.uiSettings.ColorValuesChanged(context.colorsToken);
            context.colorsSubscribed = false;
            context.uiSettings = nullptr;
            if (context.themeKey) { RegCloseKey(context.themeKey); context.themeKey = nullptr; }
            if (context.themeEvent) { CloseHandle(context.themeEvent); context.themeEvent = nullptr; }
            WTSUnRegisterSessionNotification(window);
            context.sessionRegistered = false;
            DestroyWindow(window);
            context.window = nullptr;
        }
        g_workerWindow = nullptr;
        if (context.visibility && context.cookie) {
            context.visibility->Unadvise(context.cookie);
            context.cookie = 0;
        }
        context.sink = nullptr;
        context.visibility = nullptr;
    } catch (...) {
        LogException(L"WorkerMain");
        PublishState(TaskbarState::Fallback, L"WORKER_ERROR");
        if (!readyAnnounced && g_ready) SetEvent(g_ready);
    }
    if (SUCCEEDED(com)) RoUninitialize();
}

BOOL Wh_ModInit() {
    g_canLog=true; g_unloading=false; g_native=true;
    // This is the first mod log. Its absence CANNOT prove non-injection: logging
    // may be disabled or the OS loader can fail before reaching this function.
    DTT_LOG(L"LOAD_INIT_ENTER version=1.2.2 pid=%lu tid=%lu pointerBits=%u target=explorer.exe",GetCurrentProcessId(),GetCurrentThreadId(),unsigned(sizeof(void*)*8));
    bool uiRegistered=false,workerRegistered=false;
    auto cleanup=[&] {
        if(g_ready){CloseHandle(g_ready);g_ready=nullptr;}
        if(g_dependencyWake){CloseHandle(g_dependencyWake);g_dependencyWake=nullptr;}
        if(uiRegistered)UnregisterClassW(kUiClass,g_module);
        if(workerRegistered)UnregisterClassW(kWorkerClass,g_module);
    };
    try {
        g_diagnostics=Wh_GetIntSetting(L"diagnostics")!=0;
        LoadThemeSettings(); g_taskbarState=TaskbarState::Fallback;
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<PCWSTR>(&Wh_ModInit),&g_module)) {
            DTT_LOG(L"LOAD_INIT_FAIL stage=self-module error=%lu",GetLastError()); return FALSE;
        }
        WNDCLASSW ui{}; ui.hInstance=g_module;ui.lpszClassName=kUiClass;ui.lpfnWndProc=UiWndProc;
        WNDCLASSW worker{}; worker.hInstance=g_module;worker.lpszClassName=kWorkerClass;worker.lpfnWndProc=WorkerWndProc;
        if(!RegisterClassW(&ui)) {
            DTT_LOG(L"LOAD_INIT_FAIL stage=ui-class error=%lu",GetLastError());cleanup();return FALSE;
        }
        uiRegistered=true;
        if(!RegisterClassW(&worker)) {
            DTT_LOG(L"LOAD_INIT_FAIL stage=worker-class error=%lu",GetLastError());cleanup();return FALSE;
        }
        workerRegistered=true;
        g_ready=CreateEventW(nullptr,TRUE,FALSE,nullptr);
        g_dependencyWake=CreateEventW(nullptr,FALSE,FALSE,nullptr);
        if(!g_ready || !g_dependencyWake) {
            DTT_LOG(L"LOAD_INIT_FAIL stage=events error=%lu",GetLastError());cleanup();return FALSE;
        }
        DTT_LOG(L"LOAD_INIT_OK return=TRUE taskbarModule=%p; dependencies deferred until AfterInit; no XAML access/no symbol lookup",TaskbarModule());
        return TRUE;
    } catch(...) { LogException(L"LOAD_INIT_EXCEPTION");cleanup();return FALSE; }
}

void Wh_ModAfterInit() {
    if(g_afterInitStarted.exchange(true) || g_unloading) return;
    DTT_LOG(L"LOAD_AFTER_INIT: Windhawk initialization complete; starting independent detector and dependency worker");
    try { g_worker=std::thread(WorkerMain); }
    catch(...) { LogException(L"LOAD_DETECTOR_START_FAILED (loaded degraded)"); }
    try { g_dependencyThread=std::thread(DependencyMain); }
    catch(...) { LogException(L"LOAD_DEPENDENCY_START_FAILED (loaded native; re-enable to retry)"); }
}

void Wh_ModBeforeUninit() {
    // Windhawk forbids new hook operations after this callback returns.
    // Stop/join the only installing thread while hook APIs are still legal.
    g_unloading=true; g_native=true;
    if(g_dependencyWake) SetEvent(g_dependencyWake);
    if(g_dependencyThread.joinable()) g_dependencyThread.join();
    if(auto worker=g_workerWindow.load()) PostMessageW(worker,kStop,0,0);
    if(g_worker.joinable()) g_worker.join();
    DTT_LOG(L"LOAD_BEFORE_UNINIT workers stopped; Windhawk can remove hooks");
}

void Wh_ModSettingsChanged() {
    g_diagnostics = Wh_GetIntSetting(L"diagnostics") != 0;
    LoadThemeSettings();
    BroadcastUi(kThemeRefresh);
    RequestThemeObservation();
    SignalDetection(true);
    if(g_dependencyWake) SetEvent(g_dependencyWake);
}

void Wh_ModUninit() {
    Wh_ModBeforeUninit(); // Idempotent for hosts that omit BeforeUninit.
    g_unloading = true;
    g_native = true;
    if (auto worker = g_workerWindow.load()) PostMessageW(worker, kStop, 0, 0);
    if (g_worker.joinable()) g_worker.join();
    while (g_externalCallbacks.load()) Sleep(1);
    // A hook already in progress before unloading must publish its UI window
    // before we snapshot the owning threads. No XAML work is done here.
    while (g_discoveryCalls.load()) Sleep(1);
    std::vector<HWND> windows;
    { std::lock_guard lock(g_windowsMutex); windows = g_uiWindows; }
    bool pending = false;
    for (auto window : windows) {
        DWORD_PTR result = 0;
        SendMessageTimeoutW(window, kCleanup, 0, 0,
                SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &result);
        if (IsWindow(window)) {
            // Never unload a WndProc/Storyboard cleanup still owned by a hung
            // UI thread. Pin only on this exceptional path; no forced termination.
            HMODULE pinned;
            GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_PIN, reinterpret_cast<PCWSTR>(&UiWndProc), &pinned);
            PostMessageW(window, kCleanup, 0, 0);
            pending = true;
            DTT_LOG(L"CLEANUP_PENDING UI thread unresponsive; DLL pinned, native restore queued");
        }
    }
    if (!pending) UnregisterClassW(kUiClass, g_module);
    UnregisterClassW(kWorkerClass, g_module);
    if(g_ready){CloseHandle(g_ready);g_ready=nullptr;}
    if(g_dependencyWake){CloseHandle(g_dependencyWake);g_dependencyWake=nullptr;}
    if(g_taskbarModuleReference && !pending){FreeLibrary(g_taskbarModuleReference);g_taskbarModuleReference=nullptr;}
    DTT_LOG(L"Uninitialized native restore pending=%d", pending);
    g_canLog = false;
}

