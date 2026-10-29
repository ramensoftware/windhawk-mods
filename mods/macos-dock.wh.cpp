// ==WindhawkMod==
// @id              macos-dock
// @name            macOS Dock
// @description     Turns the Windows 11 taskbar into a floating macOS-style dock, with glass behind the system tray, a Liquid Glass design and a minimal Start menu (fork of "MacOS Style Dock")
// @version         1.0.0
// @author          GEOCYT
// @github          https://github.com/urikurik2022-stack
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @architecture    amd64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -lgdi32 -lshell32 -lshcore -ladvapi32 -ldwmapi
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS Dock

Turns the Windows 11 taskbar into a dock with the look and feel of macOS,
while it keeps working the way Windows does, and gives the system tray and
the Start menu the same glass. It restyles the real taskbar and Start menu
rather than replacing them, so pinned apps, jump lists, thumbnails,
dragging files onto apps and rearranging your apps all work as usual.

A fork of "MacOS Style Dock" by Zach Vaughn (GPL-3.0), which made the dock,
its magnification, indicators, dividers and Recycle Bin. This fork adds the
dock's shadow and margins, auto-hide, glass behind the system tray, the
Liquid Glass design and the Start menu.

## The look

- **A floating glass dock.** Your icons sit on a rounded plate of glass,
  floating above the bottom of the screen with a soft shadow under it, in
  place of the taskbar's flat background. It shrinks to fit your icons and
  stays centered. It's dark, light, or follows your Windows theme.
- **Two designs.** Frosted glass: blurred and tinted, with a thin edge. Or
  Liquid Glass: clear, with livelier colors behind it; its edges bend and
  blur what's behind them like thick glass, with a bright rim and light
  along the top.
- **Glass behind the system tray.** The clock and the icons beside it get
  their own panel with the dock's look, at the same height as the dock, on
  every monitor.
- **A minimal Start menu.** Only your pinned apps' icons, without their
  names, in even columns, at the top, middle or bottom of the menu, on the
  same glass. The search box (unless you turn it on), the Pinned header,
  Recommended, All apps and the phone panel are hidden; your picture and
  the power button stay. Start typing after opening Start to search, as
  usual.
- **Bigger icons, no boxes.** Dock icons are shown larger, without the
  square highlight Windows draws behind them on hover or for the active
  app, and loaded at the size they reach when fully magnified, so they stay
  crisp as they grow.
- **Running indicators.** A small dot under each running app.
- **Dividers.** Thin lines separate Start, Task View and File Explorer from
  your other apps, and your apps from the Recycle Bin.

## How it works

- **Magnification.** Icons grow as the pointer passes over them, smoothly
  falling off with distance, and push their neighbors aside. As on a Mac,
  the dock grows from one end, and shrinks back as soon as the pointer
  leaves it.
- **Over your windows.** Maximized windows reach down to the top of the
  dock, and magnified icons rise over them.
- **Auto-hide.** If you like, the dock slides off the bottom of the screen
  when the pointer leaves it, and comes back when the pointer reaches the
  bottom edge, so windows can use the whole screen.
- **A fixed order.** Start comes first, then Task View, then File Explorer,
  a divider, and the rest of your apps.
- **Jump lists in the right place.** Right-click menus open above the icon
  you clicked.
- **A Recycle Bin** at the right end of the dock, with Windows' own icon,
  which shows whether it's empty or full. Click it to open it, drag files
  onto it to delete them, or right-click it to open or empty it.

## Settings

- **Design:** frosted glass or Liquid Glass, for the dock, the tray's glass
  and the Start menu.
- **Liquid Glass:** how clear it is, how much it blurs, how strongly its
  edges bend what's behind them, and how bright its rim and highlight are.
- **Dock Panel:** the size of the icons and dock, the color (System, Dark or
  Light), frosted glass's opacity and blur, the dock's margins from the
  bottom and side edges of the screen, and how round its corners are.
- **Shadow:** how dark, soft and low the shadow under the dock is.
- **Magnification:** how much icons grow, how far from the pointer, how
  long the animation takes, and whether icons magnify over maximized
  windows.
- **Dock Items:** the divider after File Explorer, the Recycle Bin, and the
  glass behind the system tray.
- **Start menu:** on or off, the icons' size, how many columns, where the
  icons sit (top, center or bottom, with fine offsets), and the search box.
- **Auto-hide:** hide the dock automatically.

## Compatibility

- Windows 11 only, with the taskbar at the bottom of the screen and its
  icons centered. Made on Windows 11 25H2 (build 26200), with the
  redesigned Start menu; the classic one's names are covered too.
- x64 PCs only for now; ARM64 isn't supported yet.
- The "Small taskbar buttons" setting isn't supported.
- Leave Windows' "Automatically hide the taskbar" setting off, and use the
  mod's Auto-hide setting instead. While "Magnify over windows" or Auto-hide
  is on, the mod turns it on behind the scenes, and puts it back as it was
  when the settings or the mod are turned off. If the mod can't put it back,
  for example when Windhawk is uninstalled while Explorer isn't running,
  turn it off yourself in Windows' taskbar settings.
- Not designed to be used with other taskbar or Start menu mods. In
  particular, don't combine it with "Windows 11 Taskbar Styler", "Windows 11
  Start Menu Styler", "Taskbar height and icon size", "Taskbar Dock
  Animation", "Taskbar Dock Animation Plus", "Mac Island & Dock", "Taskbar
  Multirow", "Start button always on the left" or "Compact Start Menu".
- If the Start menu doesn't change right away, close it from Task Manager
  (it starts again by itself) or restart Explorer.
- If Windows' Smart App Control is on, it blocks this mod (as it does all
  Windhawk mods), because the mod is compiled on your PC and isn't signed.

## Credits

The dock itself is Zach Vaughn's "MacOS Style Dock". Its taskbar height
code is based on m417z's "Taskbar height and icon size" mod; the way it
handles dragging icons and places right-click menus follows his "Taskbar
Multirow" mod, and the way it keeps the taskbar shown follows his "Taskbar
auto-hide when maximized" mod (all GPL-3.0). The tray glass is placed as in
"Taskbar Blob Shape"; the Start menu is reached as in "Compact Start Menu",
with element names from the themes of "Windows 11 Start Menu Styler"; the
Liquid Glass look follows the LiquidGlass themes and the color matrix of
"Windows 11 Taskbar Styler", and its edge masks follow "Acrylic Color
Glows".
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- design: frosted
  $name: Design
  $description: The look of the dock, the system tray's glass and the Start menu. Liquid Glass is clear, with livelier colors behind it, edges that bend what's behind them, a bright rim and light along the top (see Liquid Glass below).
  $options:
  - frosted: Frosted glass
  - liquid: Liquid Glass
- liquid:
  - transparency: 80
    $name: Transparency (0-100)
    $description: How clear the glass is. 100 = no tint at all. (Default = 80)
  - blur: 4
    $name: Blur (0-30)
    $description: How much what's behind the glass is blurred. (Default = 4)
  - refraction: 60
    $name: Edge refraction (0-100)
    $description: How strongly the glass's edges blur and brighten what's behind them, like light bending through the edge of thick glass. 0 turns it off. (Default = 60)
  - highlight: 60
    $name: Highlight (0-100)
    $description: How bright the glass's rim and the light along its top are. (Default = 60)
  $name: Liquid Glass
- dock:
  - size: 55
    $name: Size (0-100)
    $description: Size of the icons and the dock. (Default = 55)
  - color: auto
    $name: Color
    $description: The color of the dock panel.
    $options:
    - auto: System
    - dark: Dark
    - light: Light
  - tint: 20
    $name: Opacity (0-100)
    $description: How opaque frosted glass's tint is. 0 = clear glass, 100 = solid. (Default = 20)
  - blur: 35
    $name: Blur (0-100)
    $description: How blurred frosted glass is. (Default = 35)
  - bottomMargin: 7
    $name: Bottom margin (px)
    $description: Gap between the dock and the bottom edge of the screen. (Default = 7)
  - sideMargin: 8
    $name: Side margin (px)
    $description: The dock never comes closer than this to the left and right edges of the screen. (Default = 8)
  - roundness: 30
    $name: Corner roundness (0-50)
    $description: Corner radius as a percentage of the dock's height. 50 makes its ends fully round. (Default = 30)
  $name: Dock Panel
- shadow:
  - opacity: 30
    $name: Opacity (0-100)
    $description: How dark the shadow under the dock is. 0 turns it off. (Default = 30)
  - blur: 16
    $name: Softness (px)
    $description: Blur radius of the shadow. (Default = 16)
  - offset: 3
    $name: Offset (px)
    $description: How far down the shadow is moved. (Default = 3)
  $name: Shadow
- magnification:
  - amount: 60
    $name: Amount (0-100)
    $description: How much the icons grow when hovered. (Default = 60)
  - range: 55
    $name: Range (0-100)
    $description: How far from the pointer icons grow. (Default = 55)
  - smoothing: 100
    $name: Smoothing (ms)
    $description: Duration of animation transitions. (Default = 100)
  - overWindows: true
    $name: Magnify over windows
    $description: Maximized windows reach down to the dock, and magnified icons rise over them.
  $name: Magnification
- items:
  - appsDivider: true
    $name: Divider after File Explorer
    $description: A divider between Start, Task View and File Explorer, and the rest of your apps.
  - recycleBin: true
    $name: Recycle Bin
    $description: Show the Recycle Bin at the right end of the dock, after a divider.
  - trayGlass: true
    $name: Glass behind the system tray
    $description: The clock and the icons beside it get their own panel, with the dock's look (color, opacity, blur, corners, shadow and margins).
  $name: Dock Items
- startMenu:
  - enabled: true
    $name: Customize the Start menu
    $description: Only the pinned apps' icons, without their names, on glass like the dock's (see Design). The search box, the Pinned header, Recommended, All apps and the phone panel are hidden; your picture and the power button stay.
  - iconSize: 48
    $name: Icon size (px)
    $description: Size of the pinned apps' icons in the Start menu. (Default = 48)
  - columns: 6
    $name: Columns
    $description: How many icons in a row. The columns are spread evenly across the menu. (Default = 6)
  - position: center
    $name: Icons position
    $description: Where the grid of icons sits in the Start menu, top to bottom.
    $options:
    - top: Top
    - center: Center
    - bottom: Bottom
  - offsetX: 0
    $name: Horizontal offset (px)
    $description: Moves the icons to the right; negative values move them to the left. (Default = 0)
  - offsetY: 0
    $name: Vertical offset (px)
    $description: Moves the icons down; negative values move them up. They're kept within the menu. (Default = 0)
  - showSearch: false
    $name: Search box
    $description: Show the search box at the top of the Start menu. Typing right after opening Start searches either way.
  $name: Start menu
- autoHide:
  - enabled: false
    $name: Hide the dock automatically
    $description: The dock slides off the bottom of the screen and comes back when the pointer reaches the bottom edge (Windows' own auto-hide). While it's on, "Magnify over windows" has no effect.
  $name: Auto-hide
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellapi.h>
#include <shellscalingapi.h>
#include <sddl.h>
#include <shlobj.h>
#include <d2d1_1.h>
#include <dwmapi.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cmath>
#include <functional>
#include <iterator>
#include <limits>
#include <list>
#include <mutex>
#include <numeric>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <unordered_set>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

namespace wadt = winrt::Windows::ApplicationModel::DataTransfer;
namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;
namespace wuc = winrt::Windows::UI::Composition;
namespace wge = winrt::Windows::Graphics::Effects;

// ---- Settings, as the user set them ----
int g_dockSize = 55;           // 0-100
float g_scale = 1.6f;          // from Magnification Amount: 0-100 -> 1.0-2.0
int g_dockTint = 20;           // 0-100
int g_dockBlur = 35;           // 0-100
int g_magnificationRange = 55; // 0-100
int g_dockTheme = 0;           // 0 = System (follows Windows), 1 = dark, 2 = light
int g_animationMs = 100;
bool g_showTrash = true;
bool g_appsDivider = true;
bool g_magnifyOverWindows = true;
bool g_autoHide = false;       // the dock hides itself (Auto-hide)
bool g_trayGlass = true;       // the system tray gets its own glass
bool g_liquidGlass = false;    // Design: Liquid Glass rather than frosted glass
int g_liquidTransparency = 80; // 0-100
int g_liquidBlur = 4;          // 0-30, std. deviation
int g_liquidRefraction = 60;   // 0-100; 0 = no edge lens
int g_liquidHighlight = 60;    // 0-100
bool g_startMenu = true;       // the Start menu is customized
int g_startIconSize = 48;      // px, the Start menu's pinned icons
int g_startColumns = 6;        // icons in a row in the Start menu
int g_startPosition = 1;       // the icons' place: 0 = top, 1 = center, 2 = bottom
int g_startOffsetX = 0;        // px, then moved right by this
int g_startOffsetY = 0;        // px, and down by this
bool g_startShowSearch = false;  // the Start menu keeps its search box
int g_dockBottomMargin = 7;    // px, between the dock and the screen's bottom
int g_dockSideMargin = 8;      // px, the closest the dock gets to the screen's sides
int g_dockRoundness = 30;      // 0-50, corner radius as % of the dock's height
int g_shadowOpacity = 30;      // 0-100; 0 = no shadow
int g_shadowBlur = 16;         // px
int g_shadowOffset = 3;        // px, downward

// ---- Sizes derived from the settings (see ComputeSizes) ----
float g_iconSize = 39.0f;
float g_radius = 130.0f;       // effect radius
float g_buttonWidth = 47.0f;   // widened to fit the icon (see ApplyButtonWidth)
float g_buttonMargin = 2.5f;   // per side; negative pulls buttons together
float g_dockHeight = 65.0f;
float g_dockSidePadding = 15.0f;
float g_dockRadius = 20.0f;
float g_dockBottomGap = 7.0f;
int g_taskbarHeight = 85;      // 0 = leave the taskbar at its normal height
float g_iconLoadSize = 63;     // resolution icons are loaded at; 0 = Windows' own
float g_dividerSpace = 14.0f;  // room for a divider
float g_trashSlot = 48.0f;     // room for the Recycle Bin itself
float g_trashSpace = 62.0f;    // room reserved after the last icon: divider + Recycle Bin

constexpr float kStockIconSize = 24;      // Windows 11 taskbar icon size
constexpr float kStockButtonWidth = 44;   // Windows 11 taskbar button width
constexpr int kMinTaskbarHeight = 40;     // keeps the clock from being squashed
constexpr float kButtonIconPadding = 8;   // a button is at least its icon + this wide

// Fixed tuning, found by testing. Given for the stock 24px icon and scales
// with the dock.
constexpr float kIconSpacing = 4;         // on the old 0-10 scale: gap = icon / 3
// Windows' Recycle Bin icon has empty space around it (app icons fill their
// square), so it's drawn this much bigger, in the same slot.
constexpr float kTrashIconScale = 1.15f;

void ComputeSizes() {
    // Exponential, so every step of the slider feels the same:
    // 0 -> 18px, 20 -> 24px (Windows' own size), 55 -> 39px, 100 -> 73px.
    float icon = std::round(kStockIconSize * std::exp2((g_dockSize - 20) / 50.0f));
    g_iconSize = icon;

    // Ratios that give the original look (40px dock, 12px corners, 4px gap)
    // at the stock 24px icon.
    g_dockHeight = std::round(icon * 5 / 3);
    // Space between the outermost icons and the dock's ends: 15px at the
    // default 39px icon (Size 55).
    g_dockSidePadding = std::round(icon * 0.385f);
    g_dockRadius = std::round(g_dockHeight * g_dockRoundness / 100.0f);
    g_dockBottomGap = (float)g_dockBottomMargin;
    // Range 0-100: icons grow up to 1-6 icons from the pointer.
    g_radius = icon * (1 + g_magnificationRange / 20.0f);

    // Buttons stay at the stock width unless the icon needs more room.
    // Visible gap between icons, scaled with the icons. The margins make
    // every button take up exactly icon + gap.
    g_buttonWidth = std::max(kStockButtonWidth, icon + kButtonIconPadding);
    float gap = kIconSpacing * icon / 12;
    g_buttonMargin = (icon + gap - g_buttonWidth) / 2;

    // After the divider's space, the Recycle Bin takes up a little less than
    // a button (icon + gap), since it's drawn bigger than an icon.
    g_dividerSpace = std::round(icon * 0.35f);
    g_trashSlot = std::round(icon + gap - icon * 0.1f);
    g_trashSpace = g_dividerSpace + g_trashSlot;

    // Icons are loaded at the largest size they're ever shown at: fully
    // magnified. Shown smaller, they're scaled down, which stays sharp.
    g_iconLoadSize = std::ceil(icon * g_scale);

    // The highest point the dock reaches: the top of the dock itself, or of a
    // fully magnified icon. The icon sits centered in the dock and grows
    // upward from its bottom edge, so its top ends up
    // bottomGap + (dock - icon) / 2 + scale * icon above the screen's bottom.
    // With little magnification, that's lower than the top of the dock.
    // The shadow reaches above the dock by its blur, less its downward offset.
    float shadowAbove = g_shadowOpacity > 0 ? std::max(0, g_shadowBlur - g_shadowOffset) : 0;
    float dockTop = g_dockBottomGap + g_dockHeight + shadowAbove;
    float iconTop = g_dockBottomGap + (g_dockHeight - icon) / 2 + g_scale * icon;
    float highest = std::max(dockTop, iconTop);
    // The tray's glass is at least as tall as the tray (see TrayGlassHeight).
    if (g_trayGlass) {
        highest = std::max(highest, g_dockBottomGap + std::max(g_dockHeight, 48.0f) + shadowAbove);
    }

    // The taskbar is just tall enough for that (+2px so a magnified icon never
    // touches the edge).
    g_taskbarHeight = std::max((int)std::ceil(highest + 2), kMinTaskbarHeight);
}

// ---------------------------------------------------------------------------
// Shell jobs
// ---------------------------------------------------------------------------
// Things done through the Windows shell (recycling files, emptying the
// Recycle Bin) run on a thread pool thread, one at a time: they can take a
// while and show Windows' own progress and confirmation windows, which
// mustn't hold up the taskbar.

std::mutex g_shellJobsLock;
std::vector<std::function<void()>> g_shellJobs;
PTP_WORK g_shellJobWork = nullptr;  // created in Wh_ModInit

void CALLBACK RunShellJob(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK) {
    std::function<void()> job;
    {
        std::lock_guard lock(g_shellJobsLock);
        if (g_shellJobs.empty()) return;
        job = std::move(g_shellJobs.front());
        g_shellJobs.erase(g_shellJobs.begin());
    }
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    job();
    if (SUCCEEDED(co)) CoUninitialize();
}

void QueueShellJob(std::function<void()> job) {
    if (!g_shellJobWork) return;
    {
        std::lock_guard lock(g_shellJobsLock);
        g_shellJobs.push_back(std::move(job));
    }
    SubmitThreadpoolWork(g_shellJobWork);
}

// ---------------------------------------------------------------------------
// The mod's window
// ---------------------------------------------------------------------------
// A hidden window on the taskbar's thread, where Windows reports Recycle Bin
// changes, and where the mod's own updates are posted to run outside the
// taskbar's code. The app bars (see "Magnify over windows") are windows of
// the same class, and get screen and app bar changes.

constexpr WCHAR kWindowClass[] = L"Windhawk_" WH_MOD_ID;
constexpr UINT kAppBarMessage = WM_APP;              // ABN_* from the shell
constexpr UINT kUpdateOverWindowsMessage = WM_APP + 1;
constexpr UINT kTrashChangedMessage = WM_APP + 2;    // from SHChangeNotifyRegister
constexpr UINT kTrashCheckedMessage = WM_APP + 3;    // from CheckRecycleBin

std::atomic<HWND> g_modWindow = nullptr;

HINSTANCE ModuleInstance() {
    HMODULE module = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&ModuleInstance, &module);
    return module;
}

LRESULT CALLBACK ModWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ScheduleOverWindows(bool autoHide);  // see "Magnify over windows"

bool RegisterWindowClass() {
    static bool registered = [] {
        WNDCLASS wc{};
        wc.lpfnWndProc = ModWindowProc;
        wc.hInstance = ModuleInstance();
        wc.lpszClassName = kWindowClass;
        return RegisterClass(&wc) != 0;
    }();
    return registered;
}

// Run on the taskbar's thread.
HWND EnsureModWindow() {
    if (!g_modWindow && RegisterWindowClass()) {
        g_modWindow = CreateWindowEx(0, kWindowClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                     ModuleInstance(), nullptr);
    }
    return g_modWindow;
}

// ---------------------------------------------------------------------------
// The actual feature
// ---------------------------------------------------------------------------

struct Button {
    winrt::weak_ref<wux::FrameworkElement> element;
    wf::IInspectable originalMargin{nullptr};  // local value, may be UnsetValue
    int64_t marginToken = 0;
    wux::FrameworkElement::SizeChanged_revoker sizeChanged;
    // The button's template root, widened to fit the icon (ApplyButtonWidth).
    winrt::weak_ref<wux::FrameworkElement> root;
    wf::IInspectable originalRootMinWidth{nullptr};
    // Its icon column, for app buttons (ApplyIconColumn): the taskbar's own
    // width, and the one we last set.
    double originalIconColumn = 0;
    double appliedIconColumn = 0;
    // Room on either side, beyond the spacing: for a divider or the Recycle
    // Bin, and to move File Explorer (see LayOutButtons).
    float extraLeft = 0, extraRight = 0;
};

// Containers holding event revokers or XAML objects are marked no_destroy:
// when Explorer exits, XAML shuts down before globals are destroyed, and
// destroying them then could crash it. They're wrapped in std::optional so
// that ResetAll can destroy them when the mod is unloaded (.reset()).
// Globals holding only weak references and plain values don't need this.
[[clang::no_destroy]] std::optional<std::list<Button>> g_buttons{std::in_place};

Button* FindButton(wux::FrameworkElement const& el) {
    if (!g_buttons) return nullptr;
    for (auto& b : *g_buttons) {
        if (b.element.get() == el) return &b;
    }
    return nullptr;
}

// Side margins set the space between buttons: negative pulls neighbors
// closer together, positive pushes them apart. Some buttons get extra room on
// top (see LayOutButtons), which the taskbar lays the dock out with. Only
// writes when different, since it also runs from the Margin change callback
// (see OnButtonAdded).
void ApplySpacing(wux::FrameworkElement const& el) {
    Button* b = FindButton(el);
    wux::Thickness want{g_buttonMargin + (b ? b->extraLeft : 0), 0,
                        g_buttonMargin + (b ? b->extraRight : 0), 0};
    auto m = el.Margin();
    if (m.Left != want.Left || m.Top != want.Top || m.Right != want.Right ||
        m.Bottom != want.Bottom) {
        el.Margin(want);
    }
}

// Puts back a property's value as read with ReadLocalValue: the value set
// on the element itself, or none (UnsetValue).
void RestoreLocalValue(wux::FrameworkElement const& el, wux::DependencyProperty const& dp,
                       wf::IInspectable const& value) {
    if (!value || value == wux::DependencyProperty::UnsetValue()) {
        el.ClearValue(dp);
    } else {
        el.SetValue(dp, value);
    }
}

float GetRowOffsetY(wux::DependencyObject const& row);
void PlaceIndicator(wux::FrameworkElement const& button, float s);

// For the log: the thread an element is used on, and the size of its taskbar
// (the screen's width, in view pixels), which tells the monitors apart.
std::wstring DescribeTaskbarOf(wux::FrameworkElement const& el) {
    WCHAR text[96];
    float width = -1, height = -1;
    try {
        if (auto root = el ? el.XamlRoot() : nullptr) {
            width = root.Size().Width;
            height = root.Size().Height;
        }
    } catch (...) {}
    swprintf_s(text, L"thread %u, taskbar %.0fx%.0f", GetCurrentThreadId(), width, height);
    return text;
}

// The point a button is magnified around: the bottom edge of its icon. The
// icon is centered in its button, so that's half an icon below the center.
float ButtonPivotY(wux::FrameworkElement const& button) {
    return (float)button.ActualHeight() / 2 + g_iconSize / 2;
}

// Scale the icon (growing upward from the icon's bottom edge), shift it
// sideways by `tx` pixels, and move it down onto the dock by its row's
// vertical offset (see MeasurePlate). None of this affects the taskbar's
// layout.
void SetScale(wux::FrameworkElement const& el, float s, float tx = 0) {
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
    visual.CenterPoint({(float)el.ActualWidth() / 2, ButtonPivotY(el), 0});
    visual.Scale({s, s, 1});
    float ty = GetRowOffsetY(wuxm::VisualTreeHelper::GetParent(el));
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, true);
    visual.Properties().InsertVector3(L"Translation", {tx, ty, 0});
    // Everything inside the button moves with it; the running indicator gets
    // pulled back down to the bottom of the dock.
    PlaceIndicator(el, s);
}

// The row of icons the pointer is currently over. Its positions are re-read
// every frame (see OnFrame), so it follows apps opening, closing and being
// dragged to a new spot.
struct RowItem {
    winrt::weak_ref<wux::FrameworkElement> element;
    float center = 0;  // horizontal center, in the row's coordinates
    float width = 0;
    float scale = 1.0f;
    float shift = 0;
    bool trash = false;  // the Recycle Bin, which isn't a taskbar button
};

std::vector<RowItem> g_row;
winrt::weak_ref<wux::UIElement> g_rowParent;

// Latest pointer state. Pointer events only write these; the per-frame
// update below reads them.
bool g_pointerOver = false;
bool g_dockExpanded = false;  // icons magnified or about to be (see ClipTaskbars)
float g_pointerX = 0;

// Which end of the dock stays put as it grows and shrinks, and the most it
// has grown during this hover (see OnFrame).
bool g_anchorLeftEnd = true;
float g_maxGrowth = 0, g_lastGrowth = 0;

// ---- Hidden taskbar background ----

struct HiddenBackground {
    winrt::weak_ref<wux::FrameworkElement> element;
    double originalOpacity = 1.0;
};

std::list<HiddenBackground> g_backgrounds;

// Makes the taskbar's background invisible (opacity 0), remembering how to
// put it back. Opacity rather than Collapsed, so it still catches clicks on
// the taskbar's empty space.
void HideBackground(wux::FrameworkElement const& el) {
    if (el.Name() != L"BackgroundControl") return;
    g_backgrounds.remove_if([](HiddenBackground const& b) { return !b.element.get(); });
    for (auto& b : g_backgrounds) {
        if (b.element.get() == el) return;
    }
    g_backgrounds.push_back({winrt::make_weak(el), el.Opacity()});
    el.Opacity(0);
    Wh_Log(L"Taskbar background hidden");
}

void RestoreHidden(HiddenBackground const& b) {
    if (auto el = b.element.get()) {
        try { el.Opacity(b.originalOpacity); } catch (...) {}
    }
}

void RestoreBackgrounds() {
    for (auto& b : g_backgrounds) RestoreHidden(b);
    g_backgrounds.clear();
}

// ---- Finding the taskbar's elements ----

// Calls `f` for every element under `parent`, at most `depth` levels down.
// `f` returns false to skip that element's own children.
template <typename F>
void ForEachDescendant(wux::DependencyObject const& parent, int depth, F& f) {
    int count = wuxm::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(parent, i);
        bool descend = true;
        if (auto el = child.try_as<wux::FrameworkElement>()) descend = f(el);
        if (descend && depth > 1) ForEachDescendant(child, depth - 1, f);
    }
}

// The taskbar (Taskbar.TaskbarFrame) that `el` is in, or null.
wux::FrameworkElement FindTaskbarFrame(wux::FrameworkElement const& el) {
    wux::DependencyObject current = el;
    for (int depth = 0; current && depth < 20; depth++) {
        if (winrt::get_class_name(current) == L"Taskbar.TaskbarFrame") {
            return current.try_as<wux::FrameworkElement>();
        }
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    return nullptr;
}

// ---- Button highlight ----
// The rounded box behind each icon when it's hovered or its window is active
// ("BackgroundElement"), and its stacked copy for apps with several windows
// ("MultiWindowElement"). Each is hidden two ways at once: Opacity 0, and
// Collapsed, which isn't drawn at all whatever its opacity. The taskbar fades
// a new app's highlight in with an animation on Opacity, and an animation
// always beats a value we set, so opacity alone would leave it visible. If
// the taskbar later sets either property itself, it's hidden again.

struct HiddenHighlight {
    winrt::weak_ref<wux::FrameworkElement> element;
    double originalOpacity = 1.0;
    wux::Visibility originalVisibility = wux::Visibility::Visible;
    int64_t opacityToken = 0, visibilityToken = 0;
};

std::vector<HiddenHighlight> g_highlights;

void ApplyHighlightHidden(wux::UIElement const& el) {
    if (el.Opacity() != 0) el.Opacity(0);
    if (el.Visibility() != wux::Visibility::Collapsed) el.Visibility(wux::Visibility::Collapsed);
}

void HideHighlight(wux::FrameworkElement const& el) {
    // Forget the highlights of buttons that are gone.
    g_highlights.erase(std::remove_if(g_highlights.begin(), g_highlights.end(),
                                      [](HiddenHighlight& h) { return !h.element.get(); }),
                       g_highlights.end());
    for (auto& h : g_highlights) {
        if (h.element.get() == el) return;
    }
    auto& h = g_highlights.emplace_back();
    h.element = winrt::make_weak(el);
    h.originalOpacity = el.Opacity();
    h.originalVisibility = el.Visibility();
    auto rehide = [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
        try {
            if (auto ui = sender.try_as<wux::UIElement>()) ApplyHighlightHidden(ui);
        } catch (...) {}
    };
    h.opacityToken = el.RegisterPropertyChangedCallback(wux::UIElement::OpacityProperty(), rehide);
    h.visibilityToken =
        el.RegisterPropertyChangedCallback(wux::UIElement::VisibilityProperty(), rehide);
    ApplyHighlightHidden(el);
}

void RestoreHighlights() {
    for (auto& h : g_highlights) {
        auto el = h.element.get();
        if (!el) continue;
        try {
            el.UnregisterPropertyChangedCallback(wux::UIElement::OpacityProperty(),
                                                 h.opacityToken);
            el.UnregisterPropertyChangedCallback(wux::UIElement::VisibilityProperty(),
                                                 h.visibilityToken);
            el.Opacity(h.originalOpacity);
            el.Visibility(h.originalVisibility);
        } catch (...) {}
    }
    g_highlights.clear();
}

// ---- Icon size ----
// The image named "Icon" inside each button. Windows sizes it at 24px; we
// give it the dock's icon size, and set it again whenever the taskbar puts
// its own size back. Resizing the image (rather than scaling the button)
// lets Windows draw the icon from its full-resolution source, so it stays
// sharp as long as that source is big enough.

struct IconImage {
    winrt::weak_ref<wux::FrameworkElement> element;
    wf::IInspectable originalWidth{nullptr};   // local values, may be UnsetValue
    wf::IInspectable originalHeight{nullptr};
    int64_t widthToken = 0, heightToken = 0;
};

[[clang::no_destroy]] std::optional<std::list<IconImage>> g_iconImages{std::in_place};

void ApplyIconSize(wux::FrameworkElement const& el) {
    // Width() is NaN when unset, and NaN != anything, so this also covers it.
    if (el.Width() != g_iconSize) el.Width(g_iconSize);
    if (el.Height() != g_iconSize) el.Height(g_iconSize);
}

void TrackIconImage(wux::FrameworkElement const& el) {
    for (auto& i : *g_iconImages) {
        if (i.element.get() == el) return;
    }
    auto& i = g_iconImages->emplace_back();
    i.element = winrt::make_weak(el);
    i.originalWidth = el.ReadLocalValue(wux::FrameworkElement::WidthProperty());
    i.originalHeight = el.ReadLocalValue(wux::FrameworkElement::HeightProperty());
    auto reapply = [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
        try {
            if (auto fe = sender.try_as<wux::FrameworkElement>()) ApplyIconSize(fe);
        } catch (...) {}
    };
    i.widthToken =
        el.RegisterPropertyChangedCallback(wux::FrameworkElement::WidthProperty(), reapply);
    i.heightToken =
        el.RegisterPropertyChangedCallback(wux::FrameworkElement::HeightProperty(), reapply);
    ApplyIconSize(el);
}

// Undoes TrackIconImage for one image.
void ReleaseIconImage(IconImage const& i) {
    auto el = i.element.get();
    if (!el) return;
    try {
        el.UnregisterPropertyChangedCallback(wux::FrameworkElement::WidthProperty(),
                                             i.widthToken);
        el.UnregisterPropertyChangedCallback(wux::FrameworkElement::HeightProperty(),
                                             i.heightToken);
        RestoreLocalValue(el, wux::FrameworkElement::WidthProperty(), i.originalWidth);
        RestoreLocalValue(el, wux::FrameworkElement::HeightProperty(), i.originalHeight);
    } catch (...) {}
}

void RestoreIconImages() {
    for (auto& i : *g_iconImages) ReleaseIconImage(i);
    g_iconImages->clear();
}

// A visible taskbar button and where it sits inside its row.
struct Icon {
    wux::FrameworkElement element{nullptr};
    float left = 0;  // in the row's coordinates
    float top = 0;
    float width = 0;
    float height = 0;
    // The space the taskbar laid it out in, margins included. These tile the
    // row in the taskbar's own order, whatever the margins move the buttons to.
    float slotLeft = 0, slotWidth = 0;
};

// All visible tracked buttons whose parent is `parent`.
std::vector<Icon> GetIcons(wux::UIElement const& parent) {
    std::vector<Icon> icons;
    for (auto& b : *g_buttons) {
        auto other = b.element.get();
        if (!other || other.Visibility() != wux::Visibility::Visible ||
            other.ActualWidth() <= 0) {
            continue;
        }
        if (wuxm::VisualTreeHelper::GetParent(other) != parent) continue;
        // Where the taskbar laid the button out. Unlike TransformToVisual,
        // this ignores the scaling/shifting we apply, so measurements taken
        // while icons are magnified are still correct.
        auto slot = wuxc::Primitives::LayoutInformation::GetLayoutSlot(other);
        auto margin = other.Margin();
        float x = slot.X + (float)margin.Left;
        float y = slot.Y + (float)margin.Top;
        // Skip recycled buttons parked out of view by the taskbar.
        if (y < -other.ActualHeight() || y > other.ActualHeight()) continue;
        icons.push_back({other, x, y, (float)other.ActualWidth(), (float)other.ActualHeight(),
                         slot.X, slot.Width});
    }
    return icons;
}

// ---- Dock plate: a rounded box drawn behind each taskbar's icons ----

// Blur 100 = this much Gaussian blur. 50 gives 30, the same as Windows' own
// acrylic material.
constexpr float kMaxBlur = 60;

// A Gaussian blur for Windows.UI.Composition. Composition takes effects as
// descriptions it hands to Direct2D, normally built with the Win2D library;
// Explorer doesn't ship Win2D, so this describes the one effect we need
// ourselves, the same way Win2D does.
enum DOCK_EFFECT_PROPERTY_MAPPING : int {
    DOCK_EFFECT_PROPERTY_MAPPING_UNKNOWN = 0,
    DOCK_EFFECT_PROPERTY_MAPPING_DIRECT = 1,
};

MIDL_INTERFACE("2FC57384-A068-44D7-A331-30982FCF7177")
IDockGraphicsEffectD2D1Interop : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, DOCK_EFFECT_PROPERTY_MAPPING* mapping) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) = 0;
};
__CRT_UUID_DECL(IDockGraphicsEffectD2D1Interop, 0x2fc57384, 0xa068, 0x44d7, 0xa3, 0x31, 0x30,
                0x98, 0x2f, 0xcf, 0x71, 0x77)

// CLSID_D2D1GaussianBlur
constexpr GUID kGaussianBlurId = {
    0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

class GaussianBlurEffect
    : public winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect,
                               wge::IGraphicsEffectSource, IDockGraphicsEffectD2D1Interop> {
public:
    GaussianBlurEffect(float amount, wge::IGraphicsEffectSource const& source)
        : m_amount(amount), m_source(source) {}

    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring const& name) { m_name = name; }

    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) override {
        *id = kGaussianBlurId;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, DOCK_EFFECT_PROPERTY_MAPPING* mapping) override {
        if (_wcsicmp(name, L"BlurAmount") != 0) return E_INVALIDARG;
        *index = 0;
        *mapping = DOCK_EFFECT_PROPERTY_MAPPING_DIRECT;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) override {
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) override {
        try {
            wf::IInspectable v;
            switch (index) {
                case 0: v = wf::PropertyValue::CreateSingle(m_amount); break;  // std. deviation
                case 1: v = wf::PropertyValue::CreateUInt32(1); break;  // optimization: balanced
                case 2: v = wf::PropertyValue::CreateUInt32(1); break;  // border mode: hard
                default: return E_INVALIDARG;
            }
            *value = winrt::detach_abi(v.as<wf::IPropertyValue>());
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) override {
        if (index != 0) return E_INVALIDARG;
        winrt::copy_to_abi(m_source, *source);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) override {
        *count = 1;
        return S_OK;
    }

private:
    float m_amount;
    wge::IGraphicsEffectSource m_source;
    winrt::hstring m_name;
};

// A D2D color matrix effect on `source`; Liquid Glass uses it to make the
// colors behind the glass livelier (as the blur brush of "Windows 11 Taskbar
// Styler" does for TintSaturation).
// CLSID_D2D1ColorMatrix, from d2d1effects.h (defined here, like
// kGaussianBlurId, so nothing extra needs linking)
constexpr GUID kColorMatrixId = {
    0x921f03d6, 0x641c, 0x47df, {0x85, 0x2d, 0xb4, 0xbb, 0x61, 0x53, 0xae, 0x11}};

class ColorMatrixEffect
    : public winrt::implements<ColorMatrixEffect, wge::IGraphicsEffect,
                               wge::IGraphicsEffectSource, IDockGraphicsEffectD2D1Interop> {
public:
    ColorMatrixEffect(std::array<float, 20> const& matrix, wge::IGraphicsEffectSource const& source)
        : m_matrix(matrix), m_source(source) {}

    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring const& name) { m_name = name; }

    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) override {
        *id = kColorMatrixId;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, DOCK_EFFECT_PROPERTY_MAPPING* mapping) override {
        if (_wcsicmp(name, L"ColorMatrix") == 0) {
            *index = D2D1_COLORMATRIX_PROP_COLOR_MATRIX;
        } else if (_wcsicmp(name, L"AlphaMode") == 0) {
            *index = D2D1_COLORMATRIX_PROP_ALPHA_MODE;
        } else if (_wcsicmp(name, L"ClampOutput") == 0) {
            *index = D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT;
        } else {
            return E_INVALIDARG;
        }
        *mapping = DOCK_EFFECT_PROPERTY_MAPPING_DIRECT;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) override {
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) override {
        try {
            wf::IInspectable v;
            switch (index) {
                case D2D1_COLORMATRIX_PROP_COLOR_MATRIX:
                    v = wf::PropertyValue::CreateSingleArray(
                        winrt::array_view<const float>(m_matrix.data(), m_matrix.data() + 20));
                    break;
                case D2D1_COLORMATRIX_PROP_ALPHA_MODE:
                    v = wf::PropertyValue::CreateUInt32(D2D1_COLORMATRIX_ALPHA_MODE_PREMULTIPLIED);
                    break;
                case D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT:
                    v = wf::PropertyValue::CreateBoolean(false);
                    break;
                default:
                    return E_INVALIDARG;
            }
            *value = winrt::detach_abi(v.as<wf::IPropertyValue>());
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) override {
        if (index != 0) return E_INVALIDARG;
        winrt::copy_to_abi(m_source, *source);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) override {
        *count = 1;
        return S_OK;
    }

private:
    std::array<float, 20> m_matrix;
    wge::IGraphicsEffectSource m_source;
    winrt::hstring m_name;
};

// A color matrix that changes saturation by `s` (1 = unchanged): a blend
// between each color's luminance (Rec. 709) and the color itself.
std::array<float, 20> SaturationMatrix(float s) {
    constexpr float r = 0.2126f, g = 0.7152f, b = 0.0722f;
    float i = 1 - s;
    return {
        i * r + s, i * r,     i * r,     0,
        i * g,     i * g + s, i * g,     0,
        i * b,     i * b,     i * b + s, 0,
        0,         0,         0,         1,
        0,         0,         0,         0,
    };
}

// Puts a blurred copy of what's behind `host` inside it, with rounded
// corners, its colors' saturation changed by `saturation` (1 = unchanged).
// Returns the corner shape so its radius can be changed later, or nothing
// when the blur is off.
wuc::CompositionRoundedRectangleGeometry SetBlurVisual(wux::UIElement const& host, float amount,
                                                       float saturation) {
    if (amount <= 0) {
        wuxh::ElementCompositionPreview::SetElementChildVisual(host, nullptr);
        return nullptr;
    }
    auto hostVisual = wuxh::ElementCompositionPreview::GetElementVisual(host);
    auto compositor = hostVisual.Compositor();

    wge::IGraphicsEffect effect = winrt::make<GaussianBlurEffect>(
        amount, wuc::CompositionEffectSourceParameter(L"Backdrop"));
    if (saturation != 1) {
        effect = winrt::make<ColorMatrixEffect>(SaturationMatrix(saturation),
                                                effect.as<wge::IGraphicsEffectSource>());
    }
    auto brush = compositor.CreateEffectFactory(effect).CreateBrush();
    brush.SetSourceParameter(L"Backdrop", compositor.CreateBackdropBrush());

    auto sprite = compositor.CreateSpriteVisual();
    sprite.Brush(brush);

    // Follow the host element's size, and clip to the dock's rounded shape.
    auto size = compositor.CreateExpressionAnimation(L"host.Size");
    size.SetReferenceParameter(L"host", hostVisual);
    sprite.StartAnimation(L"Size", size);
    auto shape = compositor.CreateRoundedRectangleGeometry();
    shape.CornerRadius({g_dockRadius, g_dockRadius});  // ApplyGlassLook sets the glass's own
    shape.StartAnimation(L"Size", size);
    sprite.Clip(compositor.CreateGeometricClip(shape));

    wuxh::ElementCompositionPreview::SetElementChildVisual(host, sprite);
    return shape;
}

// Liquid Glass's saturation, and a color matrix that also brightens by `gain`.
constexpr float kLiquidSaturation = 1.4f;

std::array<float, 20> SaturationGainMatrix(float saturation, float gain) {
    auto m = SaturationMatrix(saturation);
    for (int row = 0; row < 3; row++) {
        for (int column = 0; column < 3; column++) m[row * 4 + column] *= gain;
    }
    return m;
}

// Liquid Glass's edges: what's behind them, blurred more and a little
// brighter, fading out toward the middle, like light bending through the
// edge of thick glass. Four bands, one along each edge, each a blurred copy
// of what's behind it masked with a gradient (as in "Acrylic Color Glows"),
// together clipped to the glass's rounded corners, inside `host`. A band is
// `width` wide, at most half the glass. Returns the corner shape.
wuc::CompositionRoundedRectangleGeometry SetLensVisual(wux::UIElement const& host, float width,
                                                       float blur, float gain, float radius) {
    if (width <= 0) {
        wuxh::ElementCompositionPreview::SetElementChildVisual(host, nullptr);
        return nullptr;
    }
    using winrt::Windows::UI::Color;
    auto hostVisual = wuxh::ElementCompositionPreview::GetElementVisual(host);
    auto compositor = hostVisual.Compositor();

    wge::IGraphicsEffect effect = winrt::make<GaussianBlurEffect>(
        blur, wuc::CompositionEffectSourceParameter(L"Backdrop"));
    effect = winrt::make<ColorMatrixEffect>(SaturationGainMatrix(kLiquidSaturation, gain),
                                            effect.as<wge::IGraphicsEffectSource>());
    auto factory = compositor.CreateEffectFactory(effect);

    auto container = compositor.CreateContainerVisual();
    auto size = compositor.CreateExpressionAnimation(L"host.Size");
    size.SetReferenceParameter(L"host", hostVisual);
    container.StartAnimation(L"Size", size);

    // Each band's size and place, and its gradient from the edge (opaque)
    // inward (clear). `w` is the band's width, at most half the glass.
    constexpr PCWSTR kWidth = L"Min(w, Min(host.Size.X, host.Size.Y) * 0.5)";
    struct Band {
        std::wstring size, offset;
        winrt::Windows::Foundation::Numerics::float2 from, to;
    };
    std::wstring w = kWidth;
    Band bands[] = {
        {L"Vector2(host.Size.X, " + w + L")", L"Vector3(0, 0, 0)", {0, 0}, {0, 1}},
        {L"Vector2(host.Size.X, " + w + L")", L"Vector3(0, host.Size.Y - " + w + L", 0)",
         {0, 1}, {0, 0}},
        {L"Vector2(" + w + L", host.Size.Y)", L"Vector3(0, 0, 0)", {0, 0}, {1, 0}},
        {L"Vector2(" + w + L", host.Size.Y)", L"Vector3(host.Size.X - " + w + L", 0, 0)",
         {1, 0}, {0, 0}},
    };
    for (auto& band : bands) {
        auto brush = factory.CreateBrush();
        brush.SetSourceParameter(L"Backdrop", compositor.CreateBackdropBrush());
        // Falls off faster than linearly, so the middle stays clear.
        auto fade = compositor.CreateLinearGradientBrush();
        fade.StartPoint(band.from);
        fade.EndPoint(band.to);
        fade.ColorStops().Append(compositor.CreateColorGradientStop(0.0f, Color{255, 255, 255, 255}));
        fade.ColorStops().Append(compositor.CreateColorGradientStop(0.4f, Color{110, 255, 255, 255}));
        fade.ColorStops().Append(compositor.CreateColorGradientStop(1.0f, Color{0, 255, 255, 255}));
        auto mask = compositor.CreateMaskBrush();
        mask.Source(brush);
        mask.Mask(fade);

        auto sprite = compositor.CreateSpriteVisual();
        sprite.Brush(mask);
        for (auto [property, expression] :
             {std::pair{L"Size", band.size.c_str()}, std::pair{L"Offset", band.offset.c_str()}}) {
            auto animation = compositor.CreateExpressionAnimation(expression);
            animation.SetReferenceParameter(L"host", hostVisual);
            animation.SetScalarParameter(L"w", width);
            sprite.StartAnimation(property, animation);
        }
        container.Children().InsertAtTop(sprite);
    }

    auto shape = compositor.CreateRoundedRectangleGeometry();
    shape.CornerRadius({radius, radius});
    shape.StartAnimation(L"Size", size);
    container.Clip(compositor.CreateGeometricClip(shape));
    wuxh::ElementCompositionPreview::SetElementChildVisual(host, container);
    return shape;
}

// A soft shadow under the dock: a Composition drop shadow in the shape of
// `mask` (a rounded rectangle the size of the dock), drawn inside `host`.
// Returns the shadow so its look can be changed, or nothing when it's off.
wuc::DropShadow SetShadowVisual(wux::UIElement const& host, wuxs::Rectangle const& mask) {
    if (g_shadowOpacity <= 0) {
        wuxh::ElementCompositionPreview::SetElementChildVisual(host, nullptr);
        return nullptr;
    }
    auto hostVisual = wuxh::ElementCompositionPreview::GetElementVisual(host);
    auto compositor = hostVisual.Compositor();

    // Without a mask, the shadow would be the sprite's plain rectangle. A
    // shape's alpha mask gives it the dock's rounded corners (a Border has no
    // GetAlphaMask, as "Island Media Controls" notes).
    auto shadow = compositor.CreateDropShadow();
    shadow.Mask(mask.GetAlphaMask());

    // The sprite has no brush of its own: only its shadow is drawn.
    auto sprite = compositor.CreateSpriteVisual();
    sprite.Shadow(shadow);
    auto size = compositor.CreateExpressionAnimation(L"host.Size");
    size.SetReferenceParameter(L"host", hostVisual);
    sprite.StartAnimation(L"Size", size);

    wuxh::ElementCompositionPreview::SetElementChildVisual(host, sprite);
    return shadow;
}

// Frosted glass, as layers in a grid: the shadow at the bottom, then the
// blur, then the tint and outline on top. The dock's plate and the system
// tray's glass are both made of it, so they look the same.
struct Glass {
    wuxc::Grid root{nullptr};                // gets positioned and sized
    wuxs::Rectangle shadowMask{nullptr};     // gives the shadow its shape
    wuxc::Border shadowHost{nullptr};        // the shadow is drawn in here
    wuxc::Border blurHost{nullptr};
    wuxc::Border lensHost{nullptr};          // Liquid Glass's edges (SetLensVisual)
    wuxc::Border tint{nullptr};
    wuxc::Border sheen{nullptr};             // Liquid Glass's highlight, on top
    wuc::DropShadow shadow{nullptr};
    wuc::CompositionRoundedRectangleGeometry blurShape{nullptr};
    wuc::CompositionRoundedRectangleGeometry lensShape{nullptr};
    float appliedBlur = -1;
    float appliedSaturation = 1;
    float appliedLensWidth = 0, appliedLensBlur = 0, appliedLensGain = 0;
};

Glass MakeGlass() {
    Glass g;
    g.root = wuxc::Grid();
    g.root.IsHitTestVisible(false);
    // The shadow's shape. It must be drawn for its alpha mask to have
    // anything in it, so it's black at 1% opacity: invisible under the glass.
    g.shadowMask = wuxs::Rectangle();
    g.shadowMask.Fill(wuxm::SolidColorBrush(winrt::Windows::UI::Color{255, 0, 0, 0}));
    g.shadowMask.Opacity(0.01);
    g.shadowHost = wuxc::Border();
    g.blurHost = wuxc::Border();
    g.lensHost = wuxc::Border();
    g.tint = wuxc::Border();
    g.sheen = wuxc::Border();
    g.root.Children().Append(g.shadowMask);
    g.root.Children().Append(g.shadowHost);
    g.root.Children().Append(g.blurHost);
    g.root.Children().Append(g.lensHost);
    g.root.Children().Append(g.tint);  // later children draw on top
    g.root.Children().Append(g.sheen);
    // Placed exactly, not rounded to whole pixels. The plate's left edge and
    // width are set separately, and each rounded on its own made the right
    // edge wobble by a pixel while the left one moved (see ApplyPlate).
    for (wux::FrameworkElement el : {wux::FrameworkElement(g.root), wux::FrameworkElement(g.shadowMask),
                                     wux::FrameworkElement(g.shadowHost),
                                     wux::FrameworkElement(g.blurHost), wux::FrameworkElement(g.lensHost),
                                     wux::FrameworkElement(g.tint), wux::FrameworkElement(g.sheen)}) {
        el.UseLayoutRounding(false);
    }
    return g;
}

// Whether glass on `el` is drawn in light colors (Color setting: System
// follows the element's theme).
bool UseLightGlass(wux::FrameworkElement const& el) {
    if (g_dockTheme != 0) return g_dockTheme == 2;
    return el && el.ActualTheme() == wux::ElementTheme::Light;
}

// A brush that goes from `from` to `to` (relative to what it fills) through
// `stops` (offset, color).
wuxm::LinearGradientBrush Gradient(
    wf::Point from, wf::Point to,
    std::initializer_list<std::pair<float, winrt::Windows::UI::Color>> stops) {
    wuxm::LinearGradientBrush brush;
    brush.StartPoint(from);
    brush.EndPoint(to);
    for (auto& [offset, color] : stops) {
        wuxm::GradientStop stop;
        stop.Offset(offset);
        stop.Color(color);
        brush.GradientStops().Append(stop);
    }
    return brush;
}

// White (or black) at `alpha` scaled by `amount` (0-1).
winrt::Windows::UI::Color White(float alpha, float amount) {
    return {(uint8_t)std::clamp(alpha * amount, 0.0f, 255.0f), 255, 255, 255};
}
winrt::Windows::UI::Color Black(float alpha, float amount) {
    return {(uint8_t)std::clamp(alpha * amount, 0.0f, 255.0f), 0, 0, 0};
}

// The desktop behind the glass, blurred (Blur), under a translucent color
// (Opacity), dark or light (Color), with rounded corners and a shadow.
// `what` names it in the log.
// `radius` is its corners' radius, and `withShadow` whether it casts the
// shadow (the Start menu has a shadow of its own).
void ApplyGlassLook(Glass& g, bool light, PCWSTR what, float radius, bool withShadow) {
    using winrt::Windows::UI::Color;
    float blur = g_dockBlur * kMaxBlur / 100;
    float saturation = 1;
    wux::CornerRadius corners{radius, radius, radius, radius};
    g.tint.BorderThickness({1, 1, 1, 1});
    g.tint.CornerRadius(corners);
    float lensWidth = 0, lensBlur = 0, lensGain = 1;
    if (g_liquidGlass) {
        // Liquid Glass, modeled on the LiquidGlass themes of "Windows 11
        // Taskbar Styler": a light blur and tint over livelier colors, edges
        // that blur and brighten what's behind them (SetLensVisual), a rim
        // lit from the top left, and light along the top that fades out
        // toward the middle.
        blur = (float)g_liquidBlur;
        saturation = kLiquidSaturation;
        if (g_liquidRefraction > 0) {
            lensWidth = 3 + g_liquidRefraction * 0.12f;
            lensBlur = blur + 4 + g_liquidRefraction * 0.16f;
            lensGain = 1 + g_liquidRefraction * 0.0015f;
        }
        uint8_t alpha = (uint8_t)((100 - g_liquidTransparency) * 255 / 100);
        g.tint.Background(wuxm::SolidColorBrush(light ? Color{alpha, 255, 255, 255}
                                                      : Color{alpha, 16, 16, 20}));
        float h = g_liquidHighlight / 100.0f;
        g.tint.BorderBrush(light ? Gradient({0, 0}, {1, 1}, {{0.0f, White(255, h)},
                                                             {0.3f, White(110, h)},
                                                             {0.6f, Black(20, h)},
                                                             {1.0f, Black(50, h)}})
                                 : Gradient({0, 0}, {1, 1}, {{0.0f, White(230, h)},
                                                             {0.2f, White(90, h)},
                                                             {0.5f, White(25, h)},
                                                             {0.8f, White(60, h)},
                                                             {1.0f, White(150, h)}}));
        float sheen = light ? 120 : 70;
        g.sheen.Background(Gradient({0, 0}, {0, 1}, {{0.0f, White(sheen, h)},
                                                     {0.5f, White(0, h)},
                                                     {0.85f, White(0, h)},
                                                     {1.0f, White(sheen * 0.3f, h)}}));
        g.sheen.CornerRadius(corners);
        g.sheen.Visibility(wux::Visibility::Visible);
    } else {
        uint8_t alpha = (uint8_t)(g_dockTint * 255 / 100);
        Color tint = light ? Color{alpha, 245, 245, 247} : Color{alpha, 60, 60, 65};
        Color border = light ? Color{30, 0, 0, 0} : Color{40, 255, 255, 255};
        g.tint.Background(wuxm::SolidColorBrush(tint));
        g.tint.BorderBrush(wuxm::SolidColorBrush(border));
        g.sheen.Visibility(wux::Visibility::Collapsed);
    }

    if (blur != g.appliedBlur || saturation != g.appliedSaturation) {
        g.appliedBlur = blur;
        g.appliedSaturation = saturation;
        try {
            g.blurShape = SetBlurVisual(g.blurHost, blur, saturation);
            if (g.blurShape) g.blurShape.CornerRadius({radius, radius});
        } catch (...) {
            g.blurShape = nullptr;
            Wh_Log(L"%s blur failed: 0x%08X %s", what, winrt::to_hresult(),
                   winrt::to_message().c_str());
        }
    } else if (g.blurShape) {
        g.blurShape.CornerRadius({radius, radius});
    }

    if (lensWidth != g.appliedLensWidth || lensBlur != g.appliedLensBlur ||
        lensGain != g.appliedLensGain) {
        g.appliedLensWidth = lensWidth;
        g.appliedLensBlur = lensBlur;
        g.appliedLensGain = lensGain;
        try {
            g.lensShape = SetLensVisual(g.lensHost, lensWidth, lensBlur, lensGain, radius);
        } catch (...) {
            g.lensShape = nullptr;
            Wh_Log(L"%s edge refraction failed: 0x%08X %s", what, winrt::to_hresult(),
                   winrt::to_message().c_str());
        }
    } else if (g.lensShape) {
        g.lensShape.CornerRadius({radius, radius});
    }

    g.shadowMask.RadiusX(radius);
    g.shadowMask.RadiusY(radius);
    if (!g.shadow && withShadow && g_shadowOpacity > 0) {
        try {
            g.shadow = SetShadowVisual(g.shadowHost, g.shadowMask);
            Wh_Log(L"%s shadow added", what);
        } catch (...) {
            g.shadow = nullptr;
            Wh_Log(L"%s shadow failed: 0x%08X %s", what, winrt::to_hresult(),
                   winrt::to_message().c_str());
        }
    }
    if (g.shadow) {
        g.shadow.Color(Color{255, 0, 0, 0});
        g.shadow.Opacity(g_shadowOpacity / 100.0f);
        g.shadow.BlurRadius((float)g_shadowBlur);
        g.shadow.Offset({0, (float)g_shadowOffset, 0});
    }
}

// Removes the glass's Composition visuals (the element tree is its owner's to
// remove).
void ReleaseGlass(Glass& g) {
    for (wux::UIElement host : {wux::UIElement(g.blurHost), wux::UIElement(g.lensHost),
                                wux::UIElement(g.shadowHost)}) {
        if (!host) continue;
        try { wuxh::ElementCompositionPreview::SetElementChildVisual(host, nullptr); }
        catch (...) {}
    }
}

// The dock's plate: glass around one row of icons.
struct Plate {
    winrt::weak_ref<wux::UIElement> row;     // the icons' parent
    winrt::weak_ref<wuxc::Panel> panel;      // the row's parent; the plate lives here
    Glass glass;
    winrt::weak_ref<wux::FrameworkElement> frame;  // the taskbar it's in
    float baseLeft = 0, baseWidth = 0;       // size when nothing is magnified
    float top = 0;
    float left = -1, width = -1;             // last applied
    float offsetY = 0;                       // how far the row's icons are moved down
    float dockBottom = 0;                    // bottom edge of the dock, in the row's coordinates
    std::wstring loggedGeometry;             // last geometry logged
    // The screen's sides less the side margin, in the panel's coordinates;
    // the plate is kept between them (see ApplyPlate). Unknown: no limit.
    float boundLeft = -std::numeric_limits<float>::infinity();
    float boundRight = std::numeric_limits<float>::infinity();
    bool loggedBounds = false;
    bool loggedClamp = false;
    wux::FrameworkElement::SizeChanged_revoker rowSizeChanged;
    wux::FrameworkElement::SizeChanged_revoker panelSizeChanged;
    wux::FrameworkElement::ActualThemeChanged_revoker panelThemeChanged;
    // The Recycle Bin at the dock's right end, after a divider. They aren't
    // in the row (the taskbar owns it); they sit in the panel over the space
    // reserved after the last button (see LayOutButtons).
    wuxc::Grid trash{nullptr};
    wuxc::Image trashImage{nullptr};
    wuxc::Border divider{nullptr};
    wux::UIElement::Tapped_revoker trashTapped;
    wux::UIElement::RightTapped_revoker trashRightTapped;
    wux::UIElement::DragEnter_revoker trashDragEnter;
    wux::UIElement::DragOver_revoker trashDragOver;
    wux::UIElement::Drop_revoker trashDrop;
    winrt::weak_ref<wux::FrameworkElement> beforeTrash;  // the last button
    float trashCenter = 0;  // in the row's coordinates
    int trashState = -2;    // icon shown: -1 unknown, 0 empty, 1 full
    // The divider between Start, Task View, File Explorer and the other apps,
    // in the space reserved after the button before it (see AppsDividerButton).
    wuxc::Border appsDivider{nullptr};
    winrt::weak_ref<wux::FrameworkElement> beforeAppsDivider;
    float appsDividerShift = 0;  // last applied, for magnification
};

[[clang::no_destroy]] std::optional<std::list<Plate>> g_plates{std::in_place};

Plate* GetRowPlate(wux::DependencyObject const& row) {
    if (!row) return nullptr;
    for (auto& p : *g_plates) {
        if (p.row.get() == row) return &p;
    }
    return nullptr;
}

bool IsFileExplorer(wux::FrameworkElement const& button) {
    return wux::Automation::AutomationProperties::GetAutomationId(button) ==
           L"Appid: Microsoft.Windows.Explorer";
}

// The button the apps divider goes after, given the dock's buttons from left
// to right: File Explorer when it's the first app, otherwise the last of the
// system buttons before the apps (Start, Task View). None when there's nothing
// on one side of it.
wux::FrameworkElement AppsDividerButton(std::vector<wux::FrameworkElement> const& buttons) {
    size_t i = 0;
    while (i < buttons.size() && winrt::get_class_name(buttons[i]) != L"Taskbar.TaskListButton") {
        i++;
    }
    if (i < buttons.size() && IsFileExplorer(buttons[i])) i++;
    // The divider goes just before buttons[i].
    if (i == 0 || i >= buttons.size()) return nullptr;
    return buttons[i - 1];
}

// Moves the apps divider with the magnification's spreading.
void SetAppsDividerShift(Plate& p, float tx) {
    if (!p.appsDivider || std::fabs(tx - p.appsDividerShift) < 0.01f) return;
    p.appsDividerShift = tx;
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(p.appsDivider);
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(p.appsDivider, true);
    visual.Properties().InsertVector3(L"Translation", {tx, 0, 0});
}

// How far the icons in `row` are moved down to sit on its dock plate.
float GetRowOffsetY(wux::DependencyObject const& row) {
    Plate* p = GetRowPlate(row);
    return p ? p->offsetY : 0;
}

// Frames left in which to re-measure the plates. Set when buttons come or go,
// and counted down over a few frames so the taskbar has laid them out first.
int g_platesDirtyFrames = 0;

void StartFrames();

void MarkPlatesDirty() {
    g_platesDirtyFrames = 3;
    StartFrames();
}

// The dock's glass (see ApplyGlassLook). The dividers are drawn in the
// opposite color.
void ApplyPlateLook(Plate& p) {
    bool light = UseLightGlass(p.panel.get());
    using winrt::Windows::UI::Color;
    Color line = light ? Color{70, 0, 0, 0} : Color{90, 255, 255, 255};
    for (auto& divider : {p.divider, p.appsDivider}) {
        if (divider) divider.Background(wuxm::SolidColorBrush(line));
    }
    ApplyGlassLook(p.glass, light, L"Dock", g_dockRadius, true);
}

void StartTrash();
void UpdateTrashIcons();
void HookUpTrash(Plate& p);

// Lays `el` out from the panel's top-left corner, positioned by its margin
// (spanning the whole grid, if it's one, so that's where it's measured from).
void PlaceInPanel(wuxc::Panel const& panel, wux::FrameworkElement const& el) {
    el.HorizontalAlignment(wux::HorizontalAlignment::Left);
    el.VerticalAlignment(wux::VerticalAlignment::Top);
    el.Visibility(wux::Visibility::Collapsed);
    if (panel.try_as<wuxc::Grid>()) {
        wuxc::Grid::SetRowSpan(el, 100);
        wuxc::Grid::SetColumnSpan(el, 100);
    }
}

// A thin vertical line, an icon tall. Put right after the icon row (like the
// Recycle Bin) so it's drawn above it.
wuxc::Border AddDivider(wuxc::Panel const& panel, wux::UIElement const& row) {
    wuxc::Border divider;
    PlaceInPanel(panel, divider);
    divider.IsHitTestVisible(false);
    divider.Width(1);
    divider.Height(g_iconSize);
    uint32_t index = 0;
    if (!panel.Children().IndexOf(row, index)) return nullptr;
    panel.Children().InsertAt(index + 1, divider);
    return divider;
}

// The divider and the Recycle Bin.
void AddTrash(Plate& p, wuxc::Panel const& panel, wux::UIElement const& row) {
    wuxc::Grid trash;
    PlaceInPanel(panel, trash);
    trash.Width(g_iconSize * kTrashIconScale);
    trash.Height(g_iconSize * kTrashIconScale);
    // A transparent background makes the whole square clickable.
    trash.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
    wuxc::ToolTipService::SetToolTip(trash, winrt::box_value(L"Recycle Bin"));
    wuxc::Image image;
    image.Stretch(wuxm::Stretch::Uniform);
    trash.Children().Append(image);

    uint32_t index = 0;
    if (!panel.Children().IndexOf(row, index)) return;
    panel.Children().InsertAt(index + 1, trash);
    auto divider = AddDivider(panel, row);

    p.trash = trash;
    p.trashImage = image;
    p.divider = divider;
    HookUpTrash(p);
    StartTrash();
}

void EnsurePlate(wux::FrameworkElement const& button) {
    // Only rows in a taskbar: not, say, the overflow flyout's.
    if (!FindTaskbarFrame(button)) return;
    auto row = wuxm::VisualTreeHelper::GetParent(button).try_as<wux::FrameworkElement>();
    if (!row) return;
    for (auto& p : *g_plates) {
        if (p.row.get() == row) return;
    }
    auto panel = wuxm::VisualTreeHelper::GetParent(row).try_as<wuxc::Panel>();
    if (!panel) {
        Wh_Log(L"The icon row's parent isn't a panel; no dock plate");
        return;
    }

    Glass glass = MakeGlass();
    auto root = glass.root;
    PlaceInPanel(panel, root);

    // Insert it right before the icon row: drawn above whatever comes earlier
    // (the taskbar background), below the icons.
    uint32_t index = 0;
    if (!panel.Children().IndexOf(row, index)) index = 0;
    panel.Children().InsertAt(index, root);

    auto& p = g_plates->emplace_back();
    p.row = winrt::make_weak(row.as<wux::UIElement>());
    p.panel = winrt::make_weak(panel);
    p.glass = glass;

    auto onSize = [](wf::IInspectable const&, wux::SizeChangedEventArgs const&) {
        MarkPlatesDirty();
    };
    p.rowSizeChanged = row.SizeChanged(winrt::auto_revoke, onSize);
    p.panelSizeChanged = panel.SizeChanged(winrt::auto_revoke, onSize);
    if (g_showTrash) AddTrash(p, panel, row);
    if (g_appsDivider) p.appsDivider = AddDivider(panel, row);
    ApplyPlateLook(p);
    // Windows switched between light and dark mode (for System).
    p.panelThemeChanged = panel.ActualThemeChanged(
        winrt::auto_revoke, [](wux::FrameworkElement const&, wf::IInspectable const&) {
            for (auto& plate : *g_plates) {
                try { ApplyPlateLook(plate); } catch (...) {}
            }
        });
    Wh_Log(L"Dock plate added (%s)", DescribeTaskbarOf(row).c_str());
}

// Re-applies a button's transform with its current magnification.
void ReapplyButton(wux::FrameworkElement const& button) {
    float s = 1.0f, tx = 0;
    for (auto& item : g_row) {
        if (item.element.get() == button) {
            s = item.scale;
            tx = item.shift;
            break;
        }
    }
    SetScale(button, s, tx);
}

// Works out the room each of the row's buttons gets beyond the spacing, and
// applies it where it changed (see ApplySpacing). The taskbar then lays them
// out again, and they're measured again. `icons` are the row's buttons, in
// any order. Returns the buttons the Recycle Bin and the apps divider go
// after.
//
// - The Recycle Bin and its divider: room after the last button.
// - The apps divider: room after its button (see AppsDividerButton).
// - File Explorer, when it isn't the first app in the taskbar's own order
//   (opened without being pinned, or another app was dragged before it): the
//   first app gets room before it for File Explorer and the divider, and
//   File Explorer is moved back into that room by its left margin, with a
//   right margin that makes its own space zero wide, so the buttons around
//   it stay where they are. Clicks still land on the right button, since
//   this is the taskbar's own layout.
std::pair<wux::FrameworkElement, wux::FrameworkElement> LayOutButtons(
    Plate& p, std::vector<Icon> const& icons) {
    // The taskbar's own order. A moved File Explorer's space is zero wide,
    // at the same place as the next one's, which it comes before.
    std::vector<size_t> order(icons.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](size_t a, size_t b) {
        if (icons[a].slotLeft != icons[b].slotLeft) return icons[a].slotLeft < icons[b].slotLeft;
        return icons[a].slotWidth < icons[b].slotWidth;
    });

    constexpr size_t kNone = SIZE_MAX;
    size_t firstApp = kNone, explorer = kNone;
    for (size_t i : order) {
        if (winrt::get_class_name(icons[i].element) != L"Taskbar.TaskListButton") continue;
        if (firstApp == kNone) firstApp = i;
        if (explorer == kNone && IsFileExplorer(icons[i].element)) explorer = i;
    }
    bool moveExplorer = explorer != kNone && explorer != firstApp;

    std::vector<float> left(icons.size()), right(icons.size());
    size_t last = order.back();  // as shown
    if (moveExplorer && last == explorer) last = order[order.size() - 2];
    if (p.trash) right[last] += g_trashSpace;

    wux::FrameworkElement beforeAppsDivider{nullptr};
    float dividerSpace = p.appsDivider ? g_dividerSpace : 0;
    if (moveExplorer) {
        // Its button stretches to fill the room it's given, so the room is
        // worked out from the button width, not from its measured one.
        auto& e = icons[explorer];
        float pitch = g_buttonWidth + 2 * g_buttonMargin;
        left[firstApp] += pitch + dividerSpace;
        left[explorer] = icons[firstApp].slotLeft - e.slotLeft;
        right[explorer] = -pitch - left[explorer];
        beforeAppsDivider = e.element;
    } else if (p.appsDivider) {
        std::vector<wux::FrameworkElement> buttons;
        for (size_t i : order) buttons.push_back(icons[i].element);
        beforeAppsDivider = AppsDividerButton(buttons);
        for (size_t i = 0; i < icons.size(); i++) {
            if (icons[i].element == beforeAppsDivider) right[i] += g_dividerSpace;
        }
    }

    bool changed = false;
    for (size_t i = 0; i < icons.size(); i++) {
        Button* b = FindButton(icons[i].element);
        if (!b || (std::fabs(b->extraLeft - left[i]) < 0.5f &&
                   std::fabs(b->extraRight - right[i]) < 0.5f)) {
            continue;
        }
        b->extraLeft = left[i];
        b->extraRight = right[i];
        ApplySpacing(icons[i].element);
        changed = true;
    }
    if (changed) MarkPlatesDirty();
    return {icons[last].element, beforeAppsDivider};
}

// Where the screen's sides are, less the side margin, in the panel's
// coordinates. The taskbar's XAML content spans its whole monitor, so its
// root's width is the screen's.
void UpdatePlateBounds(Plate& p, wuxc::Panel const& panel) {
    try {
        auto panelInRoot = panel.TransformToVisual(nullptr).TransformPoint({0, 0});
        float rootWidth = (float)panel.XamlRoot().Size().Width;
        p.boundLeft = g_dockSideMargin - panelInRoot.X;
        p.boundRight = rootWidth - g_dockSideMargin - panelInRoot.X;
        if (!p.loggedBounds) {
            p.loggedBounds = true;
            Wh_Log(L"Dock bounds: panel at x=%.1f, root width %.1f, side margin %d",
                   panelInRoot.X, rootWidth, g_dockSideMargin);
        }
    } catch (...) {
        p.boundLeft = -std::numeric_limits<float>::infinity();
        p.boundRight = std::numeric_limits<float>::infinity();
        Wh_Log(L"Dock bounds unknown: 0x%08X", winrt::to_hresult());
    }
}

// Re-measure the plate around the icons, as if nothing were magnified.
void MeasurePlate(Plate& p) {
    auto row = p.row.get();
    auto panel = p.panel.get();
    if (!row || !panel || !p.glass.root) return;

    // The taskbar it's in, for the pointer hooks. Looked up here, once it's
    // surely in the tree.
    if (!p.frame.get()) {
        if (auto frame = FindTaskbarFrame(row.as<wux::FrameworkElement>())) {
            p.frame = winrt::make_weak(frame);
        }
    }

    auto icons = GetIcons(row);
    if (icons.empty()) {
        for (wux::UIElement el : {wux::UIElement(p.glass.root), wux::UIElement(p.trash),
                                  wux::UIElement(p.divider), wux::UIElement(p.appsDivider)}) {
            if (el) el.Visibility(wux::Visibility::Collapsed);
        }
        return;
    }
    std::sort(icons.begin(), icons.end(),
              [](Icon const& a, Icon const& b) { return a.left < b.left; });

    float minLeft = icons[0].left, maxRight = icons[0].left + icons[0].width;
    float minTop = icons[0].top, maxBottom = icons[0].top + icons[0].height;
    for (auto& icon : icons) {
        minLeft = std::min(minLeft, icon.left);
        maxRight = std::max(maxRight, icon.left + icon.width);
        minTop = std::min(minTop, icon.top);
        maxBottom = std::max(maxBottom, icon.top + icon.height);
    }

    // Horizontally: the buttons are wider than their icons (empty space on
    // each side), so pull the edges in by that much, then add the padding
    // measured from the icons themselves.
    float avgWidth = 0;
    for (auto& icon : icons) avgWidth += icon.width;
    avgWidth /= icons.size();
    float emptyPerSide = std::max(0.0f, (avgWidth - g_iconSize) / 2);
    float padX = g_dockSidePadding - emptyPerSide;
    auto origin = row.TransformToVisual(panel).TransformPoint({0, 0});
    p.baseLeft = origin.X + minLeft - padX;
    p.baseWidth = (maxRight - minLeft) + 2 * padX;
    UpdatePlateBounds(p, panel);

    // Vertically: the dock sits at the bottom of the taskbar, and the icons
    // are moved down onto its center. Layout can't do this: the taskbar
    // stretches its buttons to its full height and centers each icon in its
    // button, so the icons always end up in the middle of the taskbar.
    float height = g_dockHeight;
    p.top = (float)panel.ActualHeight() - g_dockBottomGap - height;
    float iconCenterY = origin.Y + (minTop + maxBottom) / 2;
    float offsetY = p.top + height / 2 - iconCenterY;

    // A new element's Height is NaN, and any comparison with NaN is false, so
    // check for it explicitly.
    double current = p.glass.root.Height();
    if (std::isnan(current) || std::fabs(current - height) > 0.5) p.glass.root.Height(height);
    p.left = p.width = -1;  // force ApplyPlate to write
    p.glass.root.Visibility(wux::Visibility::Visible);

    // Room between the buttons for the dividers and the Recycle Bin, and File
    // Explorer moved into place.
    auto [beforeTrash, beforeAppsDivider] = LayOutButtons(p, icons);
    p.beforeTrash = winrt::make_weak(beforeTrash);
    p.beforeAppsDivider = beforeAppsDivider ? winrt::make_weak(beforeAppsDivider) : nullptr;
    auto iconOf = [&](wux::FrameworkElement const& el) -> Icon const& {
        for (auto& icon : icons) {
            if (icon.element == el) return icon;
        }
        return icons.back();
    };

    // Where a divider goes: centered in the space after `icon`, which starts
    // at the end of the icon's own space (half the gap past it).
    float centerY = p.top + height / 2;
    auto placeDivider = [&](wuxc::Border const& divider, Icon const& icon) {
        float x = icon.left + icon.width + g_buttonMargin + g_dividerSpace / 2;
        divider.Margin({origin.X + x - 0.5f, centerY - g_iconSize / 2, 0, 0});
        divider.Visibility(wux::Visibility::Visible);
    };

    // The Recycle Bin goes past the last button, after its divider.
    if (p.trash) {
        auto& last = iconOf(beforeTrash);
        p.trashCenter = last.left + last.width + g_buttonMargin + g_dividerSpace + g_trashSlot / 2;
        p.baseWidth += g_trashSpace;

        float trashSize = g_iconSize * kTrashIconScale;
        p.trash.Margin({origin.X + p.trashCenter - trashSize / 2, centerY - trashSize / 2, 0, 0});
        p.trash.Visibility(wux::Visibility::Visible);
        placeDivider(p.divider, last);
        UpdateTrashIcons();
    }

    // The divider after File Explorer. Its space is between buttons, so the
    // plate's width above already includes it.
    if (p.appsDivider) {
        p.appsDivider.Visibility(wux::Visibility::Collapsed);
        if (beforeAppsDivider) placeDivider(p.appsDivider, iconOf(beforeAppsDivider));
    }

    // Re-apply every icon's transform so the new offset and dock bottom take
    // effect, keeping any magnification currently in progress.
    p.offsetY = offsetY;
    p.dockBottom = p.top + height - origin.Y;

    // For the log, whenever it changes: where the plate is, in its panel and
    // on its taskbar.
    try {
        auto panelInRoot = panel.TransformToVisual(nullptr).TransformPoint({0, 0});
        auto rootSize = panel.XamlRoot().Size();
        WCHAR text[320];
        swprintf_s(text,
                   L"Dock plate (taskbar %.0fx%.0f): %zu icons, plate %.1f x %.1f at %.1f,%.1f "
                   L"on the taskbar, panel %s at %.1f,%.1f %.1f x %.1f, icons moved %.1f down",
                   rootSize.Width, rootSize.Height, icons.size(), p.baseWidth, height,
                   panelInRoot.X + p.baseLeft, panelInRoot.Y + p.top,
                   winrt::get_class_name(panel).c_str(), panelInRoot.X, panelInRoot.Y,
                   panel.ActualWidth(), panel.ActualHeight(), offsetY);
        if (p.loggedGeometry != text) {
            p.loggedGeometry = text;
            Wh_Log(L"%s", text);
        }
    } catch (...) {}
    for (auto& icon : icons) ReapplyButton(icon.element);
}

// Position the plate, widened by `extra` px in all, `extraLeft` of it to the
// left.
void ApplyPlate(Plate& p, float extraLeft, float extra) {
    if (!p.glass.root) return;
    float left = p.baseLeft - extraLeft;
    float width = p.baseWidth + extra;
    // Keep it off the screen's sides (see UpdatePlateBounds).
    if (p.boundRight > p.boundLeft) {
        float right = std::min(left + width, p.boundRight);
        left = std::max(left, p.boundLeft);
        if (right - left < width - 0.5f && !p.loggedClamp) {
            p.loggedClamp = true;
            Wh_Log(L"Dock clamped to the side margins: %.1f px wide instead of %.1f",
                   right - left, width);
        }
        width = std::max(0.0f, right - left);
    }
    if (std::fabs(left - p.left) > 0.25f || std::fabs(width - p.width) > 0.25f) {
        p.glass.root.Margin({left, p.top, 0, 0});
        p.glass.root.Width(width);
        p.left = left;
        p.width = width;
    }
}

void RemovePlates() {
    for (auto& p : *g_plates) {
        p.rowSizeChanged.revoke();
        p.panelSizeChanged.revoke();
        p.panelThemeChanged.revoke();
        ReleaseGlass(p.glass);
        p.trashTapped.revoke();
        p.trashRightTapped.revoke();
        p.trashDragEnter.revoke();
        p.trashDragOver.revoke();
        p.trashDrop.revoke();
        auto panel = p.panel.get();
        if (!panel) continue;
        for (wux::UIElement el : {wux::UIElement(p.glass.root), wux::UIElement(p.trash),
                                  wux::UIElement(p.divider), wux::UIElement(p.appsDivider)}) {
            uint32_t index;
            if (el && panel.Children().IndexOf(el, index)) panel.Children().RemoveAt(index);
        }
    }
    g_plates->clear();
}

// ---- Recycle Bin ----

// Raw access to a WriteableBitmap's pixels (IBufferByteAccess).
MIDL_INTERFACE("905a0fef-bc53-11df-8c49-001e4fc686da")
IDockBufferByteAccess : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE Buffer(BYTE** value) = 0;
};
__CRT_UUID_DECL(IDockBufferByteAccess, 0x905a0fef, 0xbc53, 0x11df, 0x8c, 0x49, 0x00, 0x1e, 0x4f,
                0xc6, 0x86, 0xda)

// Windows' own Recycle Bin icon, `px` pixels square.
wuxm::ImageSource LoadStockTrashIcon(bool full, int px) {
    SHSTOCKICONINFO info{sizeof(info)};
    HICON icon = nullptr;
    if (FAILED(SHGetStockIconInfo(full ? SIID_RECYCLERFULL : SIID_RECYCLER, SHGSI_ICONLOCATION,
                                  &info)) ||
        SHDefExtractIconW(info.szPath, info.iIcon, 0, &icon, nullptr, px) != S_OK || !icon) {
        Wh_Log(L"[trash] couldn't load Windows' icon");
        return nullptr;
    }

    // Its pixels, top row first, in the BGRA order XAML uses.
    ICONINFO ii{};
    BITMAP bm{};
    std::vector<BYTE> pixels;
    if (GetIconInfo(icon, &ii)) {
        if (ii.hbmColor && GetObject(ii.hbmColor, sizeof(bm), &bm)) {
            BITMAPINFO bi{};
            bi.bmiHeader = {sizeof(BITMAPINFOHEADER), bm.bmWidth, -bm.bmHeight, 1, 32, BI_RGB};
            pixels.resize((size_t)bm.bmWidth * bm.bmHeight * 4);
            HDC dc = GetDC(nullptr);
            if (GetDIBits(dc, ii.hbmColor, 0, bm.bmHeight, pixels.data(), &bi, DIB_RGB_COLORS) !=
                bm.bmHeight) {
                pixels.clear();
            }
            ReleaseDC(nullptr, dc);
        }
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
        if (ii.hbmMask) DeleteObject(ii.hbmMask);
    }
    DestroyIcon(icon);
    if (pixels.empty()) return nullptr;

    // XAML wants the colors premultiplied by their transparency.
    for (size_t i = 0; i < pixels.size(); i += 4) {
        BYTE a = pixels[i + 3];
        for (int c = 0; c < 3; c++) pixels[i + c] = (BYTE)(pixels[i + c] * a / 255);
    }
    wuxm::Imaging::WriteableBitmap bitmap(bm.bmWidth, bm.bmHeight);
    BYTE* data = nullptr;
    winrt::check_hresult(bitmap.PixelBuffer().as<IDockBufferByteAccess>()->Buffer(&data));
    memcpy(data, pixels.data(), pixels.size());
    bitmap.Invalidate();
    return bitmap;
}

// The icons, loaded once and shared by every taskbar's Recycle Bin.
[[clang::no_destroy]] wuxm::ImageSource g_trashIcons[2] = {nullptr, nullptr};

// Windows' icon for the Recycle Bin's state, loaded at the size it's shown at
// fully magnified.
wuxm::ImageSource TrashIcon(bool full, wux::UIElement const& el) {
    auto& icon = g_trashIcons[full];
    if (icon) return icon;
    double dpiScale = 1;
    try {
        if (auto root = el.XamlRoot()) dpiScale = root.RasterizationScale();
    } catch (...) {}
    return icon = LoadStockTrashIcon(
               full, (int)std::ceil(g_iconLoadSize * kTrashIconScale * dpiScale));
}

// Whether the Recycle Bin has anything in it: -1 = not known yet, 0 = empty,
// 1 = full. Checked whenever Windows reports a change to it, on a thread pool
// thread since it can take a moment with many drives.
std::atomic<int> g_trashFull = -1;
std::atomic<bool> g_trashChecking = false;
PTP_WORK g_trashWork = nullptr;
ULONG g_trashNotify = 0;  // the SHChangeNotifyRegister registration

void CALLBACK CheckRecycleBin(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK) {
    HRESULT co = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    SHQUERYRBINFO info{sizeof(info)};
    if (SUCCEEDED(SHQueryRecycleBinW(nullptr, &info))) g_trashFull = info.i64NumItems > 0;
    if (SUCCEEDED(co)) CoUninitialize();
    g_trashChecking = false;
    if (HWND w = g_modWindow) PostMessage(w, kTrashCheckedMessage, 0, 0);  // show it
}

void CheckRecycleBinSoon() {
    if (g_trashWork && !g_trashChecking.exchange(true)) SubmitThreadpoolWork(g_trashWork);
}

// Shows each Recycle Bin's icon for the latest known state.
void UpdateTrashIcons() {
    int full = g_trashFull;
    for (auto& p : *g_plates) {
        if (!p.trashImage || p.trashState == full) continue;
        p.trashState = full;
        try {
            p.trashImage.Source(TrashIcon(full == 1, p.trashImage));
        } catch (...) {
            Wh_Log(L"[trash] icon failed: 0x%08X", winrt::to_hresult());
        }
    }
}

// Asks Windows to report changes to the Recycle Bin to the mod's window,
// which then checks it again (see ModWindowProc).
void StartTrash() {
    if (!g_trashWork) g_trashWork = CreateThreadpoolWork(CheckRecycleBin, nullptr, nullptr);
    HWND w = EnsureModWindow();  // first, as it's sent the result
    CheckRecycleBinSoon();
    if (g_trashNotify) return;
    PIDLIST_ABSOLUTE bin;
    if (!w || FAILED(SHGetFolderLocation(nullptr, CSIDL_BITBUCKET, nullptr, 0, &bin))) return;
    SHChangeNotifyEntry entry{bin, TRUE};
    g_trashNotify = SHChangeNotifyRegister(w, SHCNRF_ShellLevel, SHCNE_ALLEVENTS,
                                           kTrashChangedMessage, 1, &entry);
    CoTaskMemFree(bin);
    if (!g_trashNotify) Wh_Log(L"[trash] can't watch the Recycle Bin for changes");
}

// Stops watching it and lets go of the icons. The check that may still be
// running is waited for in Wh_ModBeforeUninit, off the taskbar's thread.
void StopTrash() {
    if (g_trashNotify) {
        SHChangeNotifyDeregister(g_trashNotify);
        g_trashNotify = 0;
    }
    g_trashIcons[0] = g_trashIcons[1] = nullptr;
}

void OpenRecycleBin() {
    ShellExecuteW(nullptr, L"open", L"shell:RecycleBinFolder", nullptr, nullptr, SW_SHOWNORMAL);
}

// Moves files to the Recycle Bin, so they can be restored; Windows warns
// first if one is too big for it and would be deleted for good. `paths` is a
// list of paths, each ending in a null character. Run as a shell job.
void RecycleFiles(std::wstring const& paths) {
    SHFILEOPSTRUCTW op{};
    op.wFunc = FO_DELETE;
    op.pFrom = paths.c_str();  // its own final null ends the list
    op.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_WANTNUKEWARNING;
    if (int error = SHFileOperationW(&op)) Wh_Log(L"[trash] recycling failed: %d", error);
    CheckRecycleBinSoon();
}

// Asks first, like emptying it anywhere else in Windows. Run as a shell job.
void EmptyRecycleBin() {
    SHEmptyRecycleBinW(nullptr, nullptr, 0);
    CheckRecycleBinSoon();
}

// Right-click: Open, and Empty Recycle Bin (unavailable when it's empty).
void ShowTrashMenu(wux::FrameworkElement const& target) {
    wuxc::MenuFlyoutItem open;
    open.Text(L"Open");
    open.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) { OpenRecycleBin(); });
    wuxc::MenuFlyoutItem empty;
    empty.Text(L"Empty Recycle Bin");
    empty.IsEnabled(g_trashFull != 0);
    empty.Click([](wf::IInspectable const&, wux::RoutedEventArgs const&) {
        QueueShellJob(EmptyRecycleBin);
    });
    wuxc::MenuFlyout menu;
    menu.Items().Append(open);
    menu.Items().Append(empty);
    menu.ShowAt(target);
}

// Files dragged over the Recycle Bin: offer to move them there.
void AcceptTrashDrag(wux::DragEventArgs const& e) {
    if (!e.DataView().Contains(wadt::StandardDataFormats::StorageItems())) return;
    e.AcceptedOperation(wadt::DataPackageOperation::Move);
    if (auto ui = e.DragUIOverride()) ui.Caption(L"Move to Recycle Bin");
    e.Handled(true);
}

// Files dropped on the Recycle Bin: their paths are read (which Windows does
// asynchronously), then they're recycled, both in a shell job, so the mod
// waits for them before it unloads.
//
// The drop finishes right away, without waiting for that. While it hasn't
// finished, the window the files were dragged from is stuck waiting for it,
// and that window is also what the paths are read from: waiting would freeze
// both for good. It also reports that nothing happened to the files, so the
// window they came from doesn't treat them as moved and delete them itself.
void OnTrashDrop(wux::DragEventArgs const& e) {
    e.Handled(true);
    e.AcceptedOperation(wadt::DataPackageOperation::None);
    if (!e.DataView().Contains(wadt::StandardDataFormats::StorageItems())) return;
    QueueShellJob([items = winrt::make_agile(e.DataView().GetStorageItemsAsync())] {
        std::wstring paths;
        try {
            for (auto const& item : items.get().get()) {
                auto path = item.Path();
                if (path.empty()) continue;  // not a file on disk
                paths.append(path.c_str(), path.size());
                paths.push_back(L'\0');
            }
        } catch (...) {
            Wh_Log(L"[trash] reading the dropped files failed: 0x%08X", winrt::to_hresult());
            return;
        }
        if (!paths.empty()) RecycleFiles(paths);
    });
}

void HookUpTrash(Plate& p) {
    p.trash.AllowDrop(true);
    p.trashTapped = p.trash.Tapped(
        winrt::auto_revoke, [](wf::IInspectable const&, wuxi::TappedRoutedEventArgs const& e) {
            e.Handled(true);
            OpenRecycleBin();
        });
    p.trashRightTapped = p.trash.RightTapped(
        winrt::auto_revoke,
        [](wf::IInspectable const& sender, wuxi::RightTappedRoutedEventArgs const& e) {
            e.Handled(true);
            try { ShowTrashMenu(sender.as<wux::FrameworkElement>()); } catch (...) {
                Wh_Log(L"[trash] menu failed: 0x%08X", winrt::to_hresult());
            }
        });
    auto accept = [](wf::IInspectable const&, wux::DragEventArgs const& e) {
        try { AcceptTrashDrag(e); } catch (...) {}
    };
    p.trashDragEnter = p.trash.DragEnter(winrt::auto_revoke, accept);
    p.trashDragOver = p.trash.DragOver(winrt::auto_revoke, accept);
    p.trashDrop = p.trash.Drop(
        winrt::auto_revoke, [](wf::IInspectable const&, wux::DragEventArgs const& e) {
            try { OnTrashDrop(e); } catch (...) {
                Wh_Log(L"[trash] drop failed: 0x%08X", winrt::to_hresult());
            }
        });
}

// Magnifies the Recycle Bin like a button (growing upward from its bottom
// edge) and shifts it and the divider sideways.
void SetTrashScale(Plate& p, float s, float tx, float dividerTx) {
    if (!p.trash) return;
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(p.trash);
    // Its pivot is on the same line as the buttons': the bottom of an icon.
    float size = g_iconSize * kTrashIconScale;
    visual.CenterPoint({size / 2, size / 2 + g_iconSize / 2, 0});
    visual.Scale({s, s, 1});
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(p.trash, true);
    visual.Properties().InsertVector3(L"Translation", {tx, 0, 0});
    auto divider = wuxh::ElementCompositionPreview::GetElementVisual(p.divider);
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(p.divider, true);
    divider.Properties().InsertVector3(L"Translation", {dividerTx, 0, 0});
}

// ---- Running indicators ----
// The little pill under each running app. It's part of its button, so it
// moves and grows with it; PlaceIndicator undoes that, keeping it at its
// normal size, centered under the icon, just above the bottom of the dock.

struct Indicator {
    winrt::weak_ref<wux::FrameworkElement> element;
    winrt::weak_ref<wux::FrameworkElement> button;
    wux::FrameworkElement::SizeChanged_revoker sizeChanged;
};

[[clang::no_destroy]] std::optional<std::list<Indicator>> g_indicators{std::in_place};

void PlaceIndicator(wux::FrameworkElement const& button, float s) {
    wux::FrameworkElement el{nullptr};
    for (auto& i : *g_indicators) {
        if (i.button.get() == button) {
            el = i.element.get();
            break;
        }
    }
    if (!el) return;
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, true);

    auto reset = [&] {
        visual.Scale({1, 1, 1});
        visual.Properties().InsertVector3(L"Translation", {0, 0, 0});
    };
    Plate* p = GetRowPlate(wuxm::VisualTreeHelper::GetParent(button));
    if (!p || s <= 0) return reset();

    // The indicator's vertical center within the button, as laid out.
    float w = (float)el.ActualWidth(), h = (float)el.ActualHeight();
    float centerY = h / 2;
    wux::DependencyObject current = el;
    while (current && current != button) {
        if (auto ui = current.try_as<wux::UIElement>()) centerY += ui.ActualOffset().y;
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    if (!current) return reset();  // not inside this button after all

    // Where it should be: just above the dock's bottom edge.
    auto slot = wuxc::Primitives::LayoutInformation::GetLayoutSlot(button);
    float buttonTop = slot.Y + (float)button.Margin().Top;
    float inset = std::max(2.0f, std::round(g_iconSize / 10));
    float targetY = p->dockBottom - buttonTop - inset - h / 2;

    // The button scales by s around its pivot, then moves down by offsetY.
    // Scale the indicator by 1/s around its own center to cancel the growth,
    // and move it so that after the button's transform it lands on targetY.
    float pivotY = ButtonPivotY(button);
    float ty = (targetY - p->offsetY - pivotY) / s + pivotY - centerY;
    visual.CenterPoint({w / 2, h / 2, 0});
    visual.Scale({1 / s, 1 / s, 1});
    visual.Properties().InsertVector3(L"Translation", {0, ty, 0});
}

void TrackIndicator(wux::FrameworkElement const& el, wux::FrameworkElement const& button) {
    for (auto& i : *g_indicators) {
        if (i.element.get() == el) return;
    }
    auto& i = g_indicators->emplace_back();
    i.element = winrt::make_weak(el);
    i.button = winrt::make_weak(button);
    // Laid out for the first time, or wider for the active app: re-place it.
    i.sizeChanged = el.SizeChanged(
        winrt::auto_revoke, [weakButton = i.button](wf::IInspectable const&,
                                                    wux::SizeChangedEventArgs const&) {
            try {
                if (auto b = weakButton.get()) ReapplyButton(b);
            } catch (...) {}
        });
}

void RestoreIndicators() {
    for (auto& i : *g_indicators) {
        i.sizeChanged.revoke();
        if (auto el = i.element.get()) {
            try {
                auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
                visual.Scale({1, 1, 1});
                visual.Properties().InsertVector3(L"Translation", {0, 0, 0});
                wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, false);
            } catch (...) {}
        }
    }
    g_indicators->clear();
}

// (Re)builds the list of icons in `parent`. Icons that were already in the
// list keep their current size, so rebuilding mid-hover doesn't jump; icons
// that dropped out (e.g. the pointer moved to another monitor's taskbar) go
// back to normal size right away.
void BuildRow(wux::UIElement const& parent) {
    std::vector<RowItem> old = std::move(g_row);
    Plate* oldPlate = GetRowPlate(g_rowParent.get());
    if (g_rowParent.get() != parent) g_maxGrowth = g_lastGrowth = 0;  // another taskbar
    g_row.clear();
    g_rowParent = nullptr;

    if (parent) {
        for (auto& icon : GetIcons(parent)) {
            // The margins change how far apart buttons sit, so spread by the
            // space each one actually takes up rather than its own width.
            float pitch = std::max(1.0f, icon.width + 2 * g_buttonMargin);
            RowItem item{winrt::make_weak(icon.element), icon.left + icon.width / 2, pitch};
            for (auto& o : old) {
                if (o.element.get() == icon.element) {
                    item.scale = o.scale;
                    item.shift = o.shift;
                    o.element = nullptr;  // still in the row; don't reset it below
                    break;
                }
            }
            g_row.push_back(item);
        }
        // Left to right, so the spreading below shifts icons in the right order.
        std::sort(g_row.begin(), g_row.end(),
                  [](RowItem const& a, RowItem const& b) { return a.center < b.center; });
        g_rowParent = winrt::make_weak(parent);
        Plate* p = GetRowPlate(parent);

        // The Recycle Bin comes last.
        if (p && p->trash && !g_row.empty() &&
            p->trash.Visibility() == wux::Visibility::Visible) {
            RowItem item{nullptr, p->trashCenter, g_trashSlot};
            item.trash = true;
            for (auto& o : old) {
                if (o.trash && oldPlate == p) {
                    item.scale = o.scale;
                    item.shift = o.shift;
                    o.trash = false;  // still in the row; don't reset it below
                }
            }
            g_row.push_back(item);
        }
    }

    for (auto& o : old) {
        if (auto icon = o.element.get()) SetScale(icon, 1.0f);
        if (o.trash && oldPlate) SetTrashScale(*oldPlate, 1.0f, 0, 0);
    }
    if (oldPlate && oldPlate != GetRowPlate(parent)) SetAppsDividerShift(*oldPlate, 0);
}

// ---- Once-per-frame update ----
// CompositionTarget::Rendering fires once per screen refresh. We only
// subscribe while icons are moving toward their target size.

winrt::event_token g_frameToken{};
bool g_frameRunning = false;
std::chrono::steady_clock::time_point g_lastFrame;

void ScanTaskbars();

void StopFrames() {
    if (!g_frameRunning) return;
    wuxm::CompositionTarget::Rendering(g_frameToken);
    g_frameRunning = false;
}

void OnFrame() {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - g_lastFrame).count();
    g_lastFrame = now;
    if (dt > 0.1f) dt = 0.1f;

    // How far to move toward the target this frame. animationMs is roughly
    // the time to get (almost) all the way there.
    float alpha = 1.0f;
    if (g_animationMs > 0) {
        float tau = g_animationMs / 1000.0f / 3.0f;
        alpha = 1.0f - std::exp(-dt / tau);
    }

    // 0. If buttons recently came, went or were laid out: find any the hooks
    //    haven't reported, create missing plates and re-measure them. Done
    //    here rather than in the hooks, because the taskbar doesn't like its
    //    tree being changed while it's updating a button.
    if (g_platesDirtyFrames > 0) {
        g_platesDirtyFrames--;
        try { ScanTaskbars(); } catch (...) {
            Wh_Log(L"ScanTaskbars failed: 0x%08X", winrt::to_hresult());
        }
        for (auto& b : *g_buttons) {
            if (auto el = b.element.get()) {
                try { EnsurePlate(el); } catch (...) {
                    Wh_Log(L"EnsurePlate failed: 0x%08X", winrt::to_hresult());
                }
            }
        }
        for (auto& p : *g_plates) MeasurePlate(p);
        ScheduleOverWindows(false);  // the taskbar may have changed size
    }

    // 0b. Re-read where the row's icons are, in their current order, so the
    //     magnification follows apps that opened, closed or were dragged to
    //     a new spot. Sizes in progress are kept.
    if (auto rowParent = g_rowParent.get()) BuildRow(rowParent);

    // 1. Move each icon's scale toward its target.
    bool settled = g_platesDirtyFrames == 0;
    std::vector<float> scales(g_row.size(), 1.0f);
    for (size_t i = 0; i < g_row.size(); i++) {
        auto& item = g_row[i];

        float target = 1.0f;
        if (g_pointerOver) {
            float d = std::fabs(g_pointerX - item.center);
            if (d < g_radius) {
                // Cosine falloff: full size under the pointer, normal at the radius.
                float k = (std::cos(d / g_radius * 3.14159265f) + 1.0f) / 2.0f;
                target = 1.0f + (g_scale - 1.0f) * k;
            }
        }

        float s = item.scale + (target - item.scale) * alpha;
        if (std::fabs(target - s) < 0.001f) {
            s = target;
        } else {
            settled = false;
        }
        scales[i] = s;
    }

    // While the pointer is on the dock or icons are still magnified, the
    // taskbar isn't clipped (see ClipTaskbars).
    bool expanded = g_pointerOver;
    for (float scale : scales) expanded |= scale > 1.0f;
    if (expanded != g_dockExpanded) {
        g_dockExpanded = expanded;
        ScheduleOverWindows(false);
    }

    // 2. Spread: each icon is pushed right by the extra width of the icons
    //    before it, then the whole row is pulled back left by part of the
    //    total (growLeft). As on a Mac, the ends of the dock are where they'd
    //    be if it had grown evenly on both sides to its widest so far
    //    (maxGrowth), and one of them stays there: the left end with the
    //    pointer right of the dock's middle, so the dock grows and shrinks
    //    on the right, and the other way around. Crossing the middle doesn't
    //    move anything, since that's where the dock is widest.
    float total = 0;
    for (size_t i = 0; i < g_row.size(); i++) {
        total += (scales[i] - 1.0f) * g_row[i].width;
    }
    if (g_pointerOver) {
        if (!g_row.empty()) {
            float middle = (g_row.front().center - g_row.front().width / 2 +
                            g_row.back().center + g_row.back().width / 2) / 2;
            g_anchorLeftEnd = g_pointerX > middle;
        }
        g_maxGrowth = std::max(g_maxGrowth, total);
    } else if (g_lastGrowth > 0) {
        // Left the dock: shrink back keeping the same shape, so neither end
        // jumps.
        g_maxGrowth *= total / g_lastGrowth;
    } else {
        g_maxGrowth = total;
    }
    g_lastGrowth = total;
    float growLeft = g_anchorLeftEnd ? g_maxGrowth / 2 : total - g_maxGrowth / 2;

    auto rowParent = g_rowParent.get();
    Plate* plate = GetRowPlate(rowParent);
    auto beforeAppsDivider = plate ? plate->beforeAppsDivider.get() : nullptr;
    float appsDividerShift = 0;
    float before = 0;
    for (size_t i = 0; i < g_row.size(); i++) {
        auto& item = g_row[i];
        float extra = (scales[i] - 1.0f) * item.width;
        float shift = before + extra / 2 - growLeft;
        float gapShift = before - growLeft;  // of the space just before this item
        before += extra;
        // The apps divider sits in the space just after its button.
        if (beforeAppsDivider && item.element.get() == beforeAppsDivider) {
            appsDividerShift = before - growLeft;
        }

        if (item.trash) {
            // Its divider sits in the space just before it.
            if (plate && (scales[i] != item.scale || std::fabs(shift - item.shift) > 0.01f)) {
                SetTrashScale(*plate, scales[i], shift, gapShift);
                item.scale = scales[i];
                item.shift = shift;
            }
            continue;
        }

        auto icon = item.element.get();
        if (!icon) continue;
        if (scales[i] != item.scale || std::fabs(shift - item.shift) > 0.01f) {
            SetScale(icon, scales[i], shift);
            item.scale = scales[i];
            item.shift = shift;
        }
    }
    if (plate) SetAppsDividerShift(*plate, appsDividerShift);

    // 3. Widen the plate under the current row the same way.
    for (auto& p : *g_plates) {
        bool current = rowParent && p.row.get() == rowParent;
        ApplyPlate(p, current ? growLeft : 0, current ? total : 0);
    }

    if (settled) StopFrames();
}

void StartFrames() {
    if (g_frameRunning) return;
    static DWORD loggedThread = 0;
    if (loggedThread != GetCurrentThreadId()) {
        loggedThread = GetCurrentThreadId();
        Wh_Log(L"Frame loop on thread %u", loggedThread);
    }
    g_lastFrame = std::chrono::steady_clock::now();
    g_frameToken = wuxm::CompositionTarget::Rendering(
        [](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                OnFrame();
            } catch (...) {
                Wh_Log(L"OnFrame error: 0x%08X %s", winrt::to_hresult(),
                       winrt::to_message().c_str());
                StopFrames();
            }
        });
    g_frameRunning = true;
}

// ---- Taskbar buttons ----

Button& OnButtonAdded(wux::FrameworkElement const& el) {
    for (auto& b : *g_buttons) {
        if (b.element.get() == el) return b;
    }
    auto& b = g_buttons->emplace_back();
    b.element = winrt::make_weak(el);
    b.originalMargin = el.ReadLocalValue(wux::FrameworkElement::MarginProperty());
    // A button for a newly opened app is still being set up when we see it;
    // if the taskbar sets its own margin afterwards, put the dock's back.
    b.marginToken = el.RegisterPropertyChangedCallback(
        wux::FrameworkElement::MarginProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            try {
                if (auto fe = sender.try_as<wux::FrameworkElement>()) ApplySpacing(fe);
            } catch (...) {}
        });
    // It's also laid out (or resized) later than the plates' few-frame
    // re-measure; re-measure again when it is, so it gets moved onto the dock.
    b.sizeChanged = el.SizeChanged(
        winrt::auto_revoke,
        [](wf::IInspectable const&, wux::SizeChangedEventArgs const&) { MarkPlatesDirty(); });
    ApplySpacing(el);
    MarkPlatesDirty();  // the plate is created on the next frame
    return b;
}

// Makes the button wide enough for the dock's icon. The taskbar keeps its
// buttons at the stock width, since its layout code sees the stock 24px icon,
// and that cuts off bigger icons. The button's template root (for app
// buttons, "IconPanel") is given a minimum width instead, the way "Taskbar
// Labels" widens buttons; the button sizes itself to fit.
void ApplyButtonWidth(Button& b, wux::FrameworkElement const& button) {
    auto root = b.root.get();
    if (!root) {
        if (wuxm::VisualTreeHelper::GetChildrenCount(button) == 0) return;  // no template yet
        root = wuxm::VisualTreeHelper::GetChild(button, 0).try_as<wux::FrameworkElement>();
        if (!root) return;
        b.root = winrt::make_weak(root);
        b.originalRootMinWidth = root.ReadLocalValue(wux::FrameworkElement::MinWidthProperty());
    }
    if (root.MinWidth() != g_buttonWidth) root.MinWidth(g_buttonWidth);
}

// App buttons' template root ("IconPanel") is a grid whose first column holds
// the icon. The taskbar sizes that column for the stock icon (in
// TaskListButton::UpdateIconColumnDefinition, hooked below), which cuts off
// bigger icons whatever the button's width, so it's widened to fill the
// button. Not needed when the column sizes itself, or isn't there.
void ApplyIconColumn(Button& b) {
    auto root = b.root.get();
    auto grid = root ? root.try_as<wuxc::Grid>() : nullptr;
    if (!grid) return;
    auto columns = grid.ColumnDefinitions();
    if (columns.Size() == 0) return;
    auto column = columns.GetAt(0);
    auto width = column.Width();
    if (width.GridUnitType != wux::GridUnitType::Pixel) return;

    // A value we didn't set is the taskbar's own (it sets it again on updates),
    // kept to put back on unload.
    if (width.Value != b.appliedIconColumn) b.originalIconColumn = width.Value;
    auto padding = grid.Padding();
    double target = g_buttonWidth - padding.Left - padding.Right;
    if (width.Value != target) column.Width({target, wux::GridUnitType::Pixel});
    b.appliedIconColumn = target;
}

// Undoes OnButtonAdded for one button.
void ReleaseButton(Button& b) {
    b.sizeChanged.revoke();
    auto el = b.element.get();
    if (!el) return;
    if (b.marginToken) {
        el.UnregisterPropertyChangedCallback(wux::FrameworkElement::MarginProperty(),
                                             b.marginToken);
        b.marginToken = 0;
    }
    RestoreLocalValue(el, wux::FrameworkElement::MarginProperty(), b.originalMargin);
    if (auto root = b.root.get()) {
        if (b.originalIconColumn > 0) {
            if (auto grid = root.try_as<wuxc::Grid>(); grid && grid.ColumnDefinitions().Size()) {
                auto column = grid.ColumnDefinitions().GetAt(0);
                if (column.Width().Value == b.appliedIconColumn) {
                    column.Width({b.originalIconColumn, wux::GridUnitType::Pixel});
                }
            }
        }
        RestoreLocalValue(root, wux::FrameworkElement::MinWidthProperty(),
                          b.originalRootMinWidth);
    }
}

// Everything the mod changes in one taskbar button (an app, Start, Task
// View...): its spacing, icon, running indicator and highlights. Safe to call
// again; whatever is already handled is skipped.
void ProcessButton(wux::FrameworkElement const& button) {
    Button& b = OnButtonAdded(button);
    ApplyButtonWidth(b, button);
    ApplyIconColumn(b);
    auto visit = [&](wux::FrameworkElement const& el) {
        auto name = el.Name();
        if (name == L"Icon") {
            TrackIconImage(el);
        } else if (name == L"RunningIndicator") {
            TrackIndicator(el, button);
        } else if ((name == L"BackgroundElement" || name == L"MultiWindowElement") &&
                   el.try_as<wuxc::Border>()) {
            HideHighlight(el);
        }
        return true;
    };
    ForEachDescendant(button, 6, visit);
}

// ---- Taskbars ----
// Each taskbar (one per monitor) is a Taskbar.TaskbarFrame. The hooks report
// them, and on the next frames their trees are walked for every button and
// the background (see ScanTaskbars). That also catches buttons that existed
// before the mod loaded, which the button hooks may not have reported yet.

std::vector<winrt::weak_ref<wux::FrameworkElement>> g_frames;

void NoteTaskbarFrame(wux::FrameworkElement const& frame) {
    if (!frame) return;
    for (auto& f : g_frames) {
        if (f.get() == frame) return;
    }
    g_frames.push_back(winrt::make_weak(frame));
    Wh_Log(L"Taskbar found (%s), %zu known", DescribeTaskbarOf(frame).c_str(), g_frames.size());
    MarkPlatesDirty();
    ScheduleOverWindows(true);
}

// ---- System tray ----
// The clock and the icons beside it are kept at Windows' usual size, at the
// bottom of the screen where they normally are, rather than stretched to the
// full height of the taller taskbar (which made their hover highlights tall
// and left the clock floating). Their row, "SystemTrayFrameGrid", is given
// the normal taskbar's height and aligned to the bottom.
//
// With "Glass behind the system tray", the row also gets glass like the
// dock's, as tall as the dock and as far from the screen's bottom, with the
// row centered in it and kept the side margin from the screen's right edge.
// The glass is the row's first child, so it's drawn behind the tray's icons.
// The row is a Grid on older Windows 11 builds and a StackPanel on newer
// ones; negative margins make the glass take no room in either (as in
// "Taskbar Blob Shape"), so the tray's own layout doesn't change.

constexpr double kStockTaskbarHeight = 48;

struct TrayRow {
    winrt::weak_ref<wux::FrameworkElement> element;
    wf::IInspectable originalHeight{nullptr};  // local values, may be UnsetValue
    wf::IInspectable originalAlignment{nullptr};
    wf::IInspectable originalMargin{nullptr};
    // The glass, when there is some (see EnsureTrayGlass).
    winrt::weak_ref<wuxc::Panel> panel;        // the row, as the glass's parent
    Glass glass;
    bool noGlass = false;                      // the row can't hold any; logged
    std::wstring loggedLayout;                 // last geometry logged
    // The row's parent (SystemTray.SystemTrayFrame), when its height was set
    // to fit the glass (see FitTrayFrame).
    winrt::weak_ref<wux::FrameworkElement> frame;
    wf::IInspectable originalFrameHeight{nullptr};
    bool frameHeightSet = false;
    wux::FrameworkElement::SizeChanged_revoker sizeChanged;
    wux::FrameworkElement::ActualThemeChanged_revoker themeChanged;
};

[[clang::no_destroy]] std::optional<std::list<TrayRow>> g_trayRows{std::in_place};

// The tray glass's height: the dock's, or the tray's own if that's taller.
float TrayGlassHeight() { return std::max(g_dockHeight, (float)kStockTaskbarHeight); }

// Room between the tray's outermost items and the ends of its glass.
float TrayGlassPadding() { return std::round(TrayGlassHeight() * 0.12f); }

void ApplyTrayGlassLook(TrayRow& t) {
    auto row = t.element.get();
    if (row && t.glass.root) ApplyGlassLook(t.glass, UseLightGlass(row), L"Tray", g_dockRadius, true);
}

// Gives the row its glass, once.
void EnsureTrayGlass(TrayRow& t, wux::FrameworkElement const& row) {
    if (t.glass.root || t.noGlass) return;
    auto panel = row.try_as<wuxc::Panel>();
    if (!panel) {
        t.noGlass = true;
        Wh_Log(L"System tray row is a %s, not a panel; no tray glass",
               winrt::get_class_name(row).c_str());
        return;
    }
    t.glass = MakeGlass();
    auto root = t.glass.root;
    root.HorizontalAlignment(wux::HorizontalAlignment::Left);
    root.VerticalAlignment(wux::VerticalAlignment::Top);
    if (panel.try_as<wuxc::Grid>()) {
        wuxc::Grid::SetRowSpan(root, 100);
        wuxc::Grid::SetColumnSpan(root, 100);
    }
    panel.Children().InsertAt(0, root);
    t.panel = winrt::make_weak(panel);

    t.sizeChanged = row.SizeChanged(
        winrt::auto_revoke,
        [](wf::IInspectable const&, wux::SizeChangedEventArgs const&) { MarkPlatesDirty(); });
    // Windows switched between light and dark mode (for System).
    t.themeChanged = row.ActualThemeChanged(
        winrt::auto_revoke, [](wux::FrameworkElement const&, wf::IInspectable const&) {
            for (auto& tray : *g_trayRows) {
                try { ApplyTrayGlassLook(tray); } catch (...) {}
            }
        });
    ApplyTrayGlassLook(t);
    Wh_Log(L"Tray glass added (row is a %s)", winrt::get_class_name(row).c_str());
}

// The row is moved up by the bottom margin, so its frame must be as tall as
// the taskbar: in a frame only as tall as the row (one of the trays was 48 px
// in a 70 px taskbar), the taskbar clips the row's top off, and the glass's
// rounded corners and shadow with it. The tray height hooks normally make it
// that tall when Windows resizes the tray; this covers a tray that hasn't
// been resized since the mod loaded.
void FitTrayFrame(TrayRow& t, wux::FrameworkElement const& row) {
    if (g_taskbarHeight <= 0) return;
    auto frame = wuxm::VisualTreeHelper::GetParent(row).try_as<wux::FrameworkElement>();
    if (!frame || winrt::get_class_name(frame) != L"SystemTray.SystemTrayFrame") return;
    double want = g_taskbarHeight;
    if (frame.ActualHeight() >= want - 0.5) return;
    double height = frame.Height();
    if (!std::isnan(height) && height >= want - 0.5) return;  // set, not laid out yet
    if (!t.frameHeightSet) {
        t.frame = winrt::make_weak(frame);
        t.originalFrameHeight = frame.ReadLocalValue(wux::FrameworkElement::HeightProperty());
        t.frameHeightSet = true;
    }
    Wh_Log(L"Tray frame is %.1f tall, set to %.0f so the tray glass fits",
           frame.ActualHeight(), want);
    frame.Height(want);
}

// Sizes the glass around the row, and moves the row up and in from the
// screen's right edge so it's centered in the glass.
void LayOutTrayGlass(TrayRow& t, wux::FrameworkElement const& row) {
    auto root = t.glass.root;
    if (!root) return;
    float height = TrayGlassHeight();
    float pad = TrayGlassPadding();
    float extraY = (height - (float)kStockTaskbarHeight) / 2;

    wux::Thickness want{0, 0, g_dockSideMargin + pad, g_dockBottomGap + extraY};
    auto m = row.Margin();
    if (m.Left != want.Left || m.Top != want.Top || m.Right != want.Right ||
        m.Bottom != want.Bottom) {
        row.Margin(want);
    }

    float rowWidth = (float)row.ActualWidth();
    if (rowWidth <= 0) {
        root.Visibility(wux::Visibility::Collapsed);
        return;
    }
    float width = rowWidth + 2 * pad;
    // From the row's top-left corner: `pad` to the left and `extraY` above
    // it. The far margins cancel the size, so it takes no room in the row.
    wux::Thickness margin{-pad, -extraY, -(width - pad), -(height - extraY)};
    if (std::fabs(root.Width() - width) > 0.25 || std::isnan(root.Width())) root.Width(width);
    if (std::fabs(root.Height() - height) > 0.25 || std::isnan(root.Height())) root.Height(height);
    auto have = root.Margin();
    if (have.Left != margin.Left || have.Top != margin.Top || have.Right != margin.Right ||
        have.Bottom != margin.Bottom) {
        root.Margin(margin);
    }
    root.Visibility(wux::Visibility::Visible);

    // Logged whenever it changes: the values are only right once the
    // taskbar has laid the row out, a frame or more after they're set.
    try {
        auto inRoot = row.TransformToVisual(nullptr).TransformPoint({0, 0});
        auto rootSize = row.XamlRoot().Size();
        auto parent = wuxm::VisualTreeHelper::GetParent(row).try_as<wux::FrameworkElement>();
        WCHAR text[320];
        swprintf_s(text,
                   L"Tray glass (taskbar %.0fx%.0f): row %.1f x %.1f at %.1f,%.1f "
                   L"(Height %.1f, valign %d, parent %s %.1f tall), glass %.1f x %.1f, "
                   L"%.1f px from the right edge, %.1f px from the bottom",
                   rootSize.Width, rootSize.Height, rowWidth, row.ActualHeight(), inRoot.X,
                   inRoot.Y, row.Height(), (int)row.VerticalAlignment(),
                   parent ? winrt::get_class_name(parent).c_str() : L"none",
                   parent ? parent.ActualHeight() : -1.0, width, height,
                   rootSize.Width - (inRoot.X + rowWidth + pad),
                   rootSize.Height - (inRoot.Y + row.ActualHeight() + extraY));
        if (t.loggedLayout != text) {
            t.loggedLayout = text;
            Wh_Log(L"%s", text);
        }
    } catch (...) {}
}

void FixTrayRow(wux::FrameworkElement const& row) {
    g_trayRows->remove_if([](TrayRow const& t) { return !t.element.get(); });
    TrayRow* tray = nullptr;
    for (auto& t : *g_trayRows) {
        if (t.element.get() == row) tray = &t;
    }
    if (!tray) {
        tray = &g_trayRows->emplace_back();
        tray->element = winrt::make_weak(row);
        tray->originalHeight = row.ReadLocalValue(wux::FrameworkElement::HeightProperty());
        tray->originalAlignment =
            row.ReadLocalValue(wux::FrameworkElement::VerticalAlignmentProperty());
        tray->originalMargin = row.ReadLocalValue(wux::FrameworkElement::MarginProperty());
        Wh_Log(L"System tray found (%s)", DescribeTaskbarOf(row).c_str());
    }
    if (row.Height() != kStockTaskbarHeight) row.Height(kStockTaskbarHeight);
    if (row.VerticalAlignment() != wux::VerticalAlignment::Bottom) {
        row.VerticalAlignment(wux::VerticalAlignment::Bottom);
    }
    if (g_trayGlass) {
        try {
            EnsureTrayGlass(*tray, row);
            FitTrayFrame(*tray, row);
            LayOutTrayGlass(*tray, row);
        } catch (...) {
            Wh_Log(L"Tray glass failed: 0x%08X", winrt::to_hresult());
        }
    }
}

void RestoreTrayRows() {
    for (auto& t : *g_trayRows) {
        t.sizeChanged.revoke();
        t.themeChanged.revoke();
        ReleaseGlass(t.glass);
        try {
            auto panel = t.panel.get();
            uint32_t index;
            if (panel && t.glass.root && panel.Children().IndexOf(t.glass.root, index)) {
                panel.Children().RemoveAt(index);
            }
        } catch (...) {}
        auto el = t.element.get();
        if (!el) continue;
        try {
            RestoreLocalValue(el, wux::FrameworkElement::HeightProperty(), t.originalHeight);
            RestoreLocalValue(el, wux::FrameworkElement::VerticalAlignmentProperty(),
                              t.originalAlignment);
            RestoreLocalValue(el, wux::FrameworkElement::MarginProperty(), t.originalMargin);
        } catch (...) {}
        if (auto frame = t.frame.get(); frame && t.frameHeightSet) {
            try {
                RestoreLocalValue(frame, wux::FrameworkElement::HeightProperty(),
                                  t.originalFrameHeight);
            } catch (...) {}
        }
    }
    g_trayRows->clear();
}

// Finds the system tray next to `frame` (it's elsewhere in the same tree).
void ScanTray(wux::FrameworkElement const& frame) {
    auto root = frame.XamlRoot();
    auto content = root ? root.Content() : nullptr;
    if (!content) return;
    auto visit = [](wux::FrameworkElement const& el) {
        auto type = winrt::get_class_name(el);
        if (type == L"Taskbar.TaskbarFrame") return false;  // the dock; scanned elsewhere
        if (type == L"SystemTray.SystemTrayFrame") {
            auto findRow = [](wux::FrameworkElement const& child) {
                if (child.Name() == L"SystemTrayFrameGrid") FixTrayRow(child);
                return false;
            };
            ForEachDescendant(el, 1, findRow);
            return false;
        }
        return true;
    };
    ForEachDescendant(content, 10, visit);
}

void ScanTaskbars() {
    // Forget whatever is gone.
    g_frames.erase(std::remove_if(g_frames.begin(), g_frames.end(),
                                  [](auto& f) { return !f.get(); }),
                   g_frames.end());
    g_buttons->remove_if([](Button const& b) { return !b.element.get(); });
    g_iconImages->remove_if([](IconImage const& i) { return !i.element.get(); });
    g_indicators->remove_if([](Indicator const& i) { return !i.element.get(); });

    auto visit = [](wux::FrameworkElement const& el) {
        auto type = winrt::get_class_name(el);
        if (type == L"Taskbar.TaskListButton" || type == L"Taskbar.ExperienceToggleButton") {
            ProcessButton(el);
            return false;
        }
        if (type == L"Taskbar.TaskbarBackground") {
            HideBackground(el);
            return false;
        }
        return true;
    };
    for (auto& f : g_frames) {
        auto frame = f.get();
        if (!frame) continue;
        ForEachDescendant(frame, 12, visit);
        ScanTray(frame);
    }
}

// ---- Pointer (reported by the TaskbarFrame hooks) ----

// The pointer moved over a taskbar. It counts as over the dock when it's
// within the plate's width, gaps between icons included.
void OnTaskbarPointerMoved(wux::FrameworkElement const& frame,
                           wuxi::PointerRoutedEventArgs const& e) {
    NoteTaskbarFrame(frame);
    Plate* plate = nullptr;
    for (auto& p : *g_plates) {
        if (p.frame.get() == frame) {
            plate = &p;
            break;
        }
    }
    wux::UIElement row{nullptr};
    bool over = false;
    if (plate && plate->width > 0 && plate->glass.root.Visibility() == wux::Visibility::Visible) {
        row = plate->row.get();
        auto panel = plate->panel.get();
        if (row && panel) {
            // Measured against the dock as it's shown, and never less than its
            // unmagnified size: an edge that shrinks back past a still pointer
            // would otherwise grow again under it, making the icons jitter.
            // Above the dock only counts once it's magnifying, where the
            // magnified icons are.
            auto point = e.GetCurrentPoint(panel).Position();
            over = point.X >= std::min(plate->left, plate->baseLeft) &&
                   point.X <= std::max(plate->left + plate->width,
                                       plate->baseLeft + plate->baseWidth) &&
                   (point.Y >= plate->top || (g_pointerOver && g_rowParent.get() == row));
        }
    }
    if (!over) {
        if (g_pointerOver) {
            g_pointerOver = false;
            StartFrames();
        }
        return;
    }
    if (g_rowParent.get() != row) BuildRow(row);
    g_pointerX = e.GetCurrentPoint(row).Position().X;
    g_pointerOver = true;
    StartFrames();
}

void OnTaskbarPointerExited() {
    g_pointerOver = false;
    StartFrames();
}

void RemoveAppBars();
void ClipTaskbars();

void ResetAll() {
    StopFrames();
    try { RemovePlates(); } catch (...) {}
    try { StopTrash(); } catch (...) {}
    RestoreBackgrounds();
    RestoreHighlights();
    RestoreIconImages();
    for (auto& b : *g_buttons) {
        if (auto el = b.element.get()) {
            try {
                ReleaseButton(b);
                SetScale(el, 1.0f, 0);
                wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, false);
            } catch (...) {}
        }
    }
    RestoreIndicators();
    RestoreTrayRows();
    RemoveAppBars();
    ClipTaskbars();  // unclipped, as g_unloading is set
    if (HWND w = g_modWindow.exchange(nullptr)) DestroyWindow(w);
    g_row.clear();
    g_rowParent = nullptr;
    g_frames.clear();
    // Last, since everything above still reads them (see the no_destroy note
    // at g_buttons). Nothing touches them after this: the hooks check
    // g_unloading, and every callback and event handler is removed above.
    g_buttons.reset();
    g_iconImages.reset();
    g_plates.reset();
    g_indicators.reset();
    g_trayRows.reset();
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

using RunProc_t = void (*)(void*);

bool RunFromWindowThread(HWND hWnd, RunProc_t proc, void* param) {
    static const UINT msg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param { RunProc_t proc; void* param; };

    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) { proc(param); return true; }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int code, WPARAM w, LPARAM l) -> LRESULT {
        if (code == HC_ACTION) {
            auto* cwp = (const CWPSTRUCT*)l;
            if (cwp->message == msg) {
                auto* p = (Param*)cwp->lParam;
                p->proc(p->param);
            }
        }
        return CallNextHookEx(nullptr, code, w, l);
    }, nullptr, tid);
    if (!hook) return false;

    Param p{proc, param};
    SendMessage(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

HWND FindTaskbar() {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM l) -> BOOL {
        DWORD pid; WCHAR cls[32];
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassName(hWnd, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
            *(HWND*)l = hWnd;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&result);
    return result;
}

// ---------------------------------------------------------------------------
// Reaching the taskbar's elements from its own code
// ---------------------------------------------------------------------------
// The taskbar's button and frame classes are XAML elements. In a hook on one
// of their methods, the element is the interface at pThis + 3 pointers (the
// same as in "Taskbar Labels").

// Set while the mod unloads, so the hooks stop touching the taskbar.
std::atomic<bool> g_unloading = false;

wux::FrameworkElement ElementFromThis(void* pThis) {
    winrt::Windows::Foundation::IUnknown unknown;
    winrt::copy_from_abi(unknown, (void**)pThis + 3);
    return unknown.try_as<wux::FrameworkElement>();
}

// A taskbar app button (TaskListButton) updated itself: apply the dock's
// changes to it right away, before it's drawn, and note its taskbar so the
// rest is found on the next frames.
void OnTaskListButtonUpdated(void* pThis) {
    if (g_unloading) return;
    try {
        if (auto button = ElementFromThis(pThis)) {
            ProcessButton(button);
            NoteTaskbarFrame(FindTaskbarFrame(button));
        }
    } catch (...) {
        Wh_Log(L"Button update failed: 0x%08X", winrt::to_hresult());
    }
}

// ---- The pointer over the taskbar (as in "Taskbar Dock Animation") ----

using TaskbarFrame_OnPointer_t = int(WINAPI*)(void* pThis, void* pArgs);

// The TaskbarFrame these hooks were called for, or null for any other class.
wux::FrameworkElement TaskbarFrameFromThis(void* pThis) {
    wux::FrameworkElement element{nullptr};
    ((::IUnknown*)pThis)->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                         winrt::put_abi(element));
    if (!element || winrt::get_class_name(element) != L"Taskbar.TaskbarFrame") return nullptr;
    return element;
}

TaskbarFrame_OnPointer_t TaskbarFrame_OnPointerMoved_Original;
int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    int result = TaskbarFrame_OnPointerMoved_Original(pThis, pArgs);
    if (g_unloading) return result;
    try {
        if (auto frame = TaskbarFrameFromThis(pThis)) {
            wuxi::PointerRoutedEventArgs args{nullptr};
            ((::IUnknown*)pArgs)->QueryInterface(
                winrt::guid_of<wuxi::PointerRoutedEventArgs>(), winrt::put_abi(args));
            if (args) OnTaskbarPointerMoved(frame, args);
        }
    } catch (...) {}
    return result;
}

TaskbarFrame_OnPointer_t TaskbarFrame_OnPointerExited_Original;
int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* pArgs) {
    int result = TaskbarFrame_OnPointerExited_Original(pThis, pArgs);
    if (g_unloading) return result;
    try {
        if (TaskbarFrameFromThis(pThis)) OnTaskbarPointerExited();
    } catch (...) {}
    return result;
}

// ---- Where a button's right-click menu opens (as in "Taskbar Multirow") ----
// Windows centers a taskbar button's right-click menu (its jump list) on
// where the taskbar's layout put the button. For File Explorer, moved into its
// spot by its margins (see LayOutButtons), that's somewhere else entirely. So
// while the menu is being placed, it's centered on where the button is.

bool g_placingJumpList = false;   // in CTaskListWnd::_ComputeJumpViewPosition
bool g_hasJumpListButton = false;  // a button's menu is being opened
wf::Rect g_jumpListButton{};       // its bounds, in its taskbar's XAML content

// Notes where the button whose menu is being opened is, and opens it.
template <typename F>
void OpenJumpListFor(wux::UIElement const& sender, F original) {
    g_hasJumpListButton = false;
    if (!g_unloading) {
        try {
            auto button = sender.try_as<wux::FrameworkElement>();
            auto root = button ? button.XamlRoot() : nullptr;
            auto content = root ? root.Content() : nullptr;
            if (content && FindButton(button)) {
                g_jumpListButton = button.TransformToVisual(content).TransformBounds(
                    {0, 0, (float)button.ActualWidth(), (float)button.ActualHeight()});
                g_hasJumpListButton = true;
            }
        } catch (...) {}
    }
    original();
    g_hasJumpListButton = false;
}

using TaskbarResources_OnTaskListButtonContextRequested_t =
    void(WINAPI*)(void* pThis, wux::UIElement const& sender, void* args);
TaskbarResources_OnTaskListButtonContextRequested_t
    TaskbarResources_OnTaskListButtonContextRequested_Original;
void WINAPI TaskbarResources_OnTaskListButtonContextRequested_Hook(
    void* pThis, wux::UIElement const& sender, void* args) {
    OpenJumpListFor(sender, [&] {
        TaskbarResources_OnTaskListButtonContextRequested_Original(pThis, sender, args);
    });
}

// The same, on builds that skip the one above.
using TaskListButtonHandlers_HandleContextRequested_t =
    void(WINAPI*)(wux::UIElement const& sender, void* args);
TaskListButtonHandlers_HandleContextRequested_t
    TaskListButtonHandlers_HandleContextRequested_Original;
void WINAPI TaskListButtonHandlers_HandleContextRequested_Hook(wux::UIElement const& sender,
                                                              void* args) {
    OpenJumpListFor(sender, [&] {
        TaskListButtonHandlers_HandleContextRequested_Original(sender, args);
    });
}

using CTaskListWnd_ComputeJumpViewPosition_t =
    HRESULT(WINAPI*)(void* pThis, void* taskBtnGroup, int param2, wf::Point* point,
                     wux::HorizontalAlignment* horizontal, wux::VerticalAlignment* vertical);
CTaskListWnd_ComputeJumpViewPosition_t CTaskListWnd_ComputeJumpViewPosition_Original;
HRESULT WINAPI CTaskListWnd_ComputeJumpViewPosition_Hook(void* pThis, void* taskBtnGroup,
                                                         int param2, wf::Point* point,
                                                         wux::HorizontalAlignment* horizontal,
                                                         wux::VerticalAlignment* vertical) {
    g_placingJumpList = true;
    HRESULT hr = CTaskListWnd_ComputeJumpViewPosition_Original(pThis, taskBtnGroup, param2,
                                                               point, horizontal, vertical);
    g_placingJumpList = false;
    return hr;
}

// Converts the button's bounds to screen pixels for placing the menu.
using ToRawPixelsInDesktopCoordinates_t = RECT(WINAPI*)(wf::Rect const& rect, UINT dpi,
                                                         HWND hWnd);
ToRawPixelsInDesktopCoordinates_t ToRawPixelsInDesktopCoordinates_Original;
RECT WINAPI ToRawPixelsInDesktopCoordinates_Hook(wf::Rect const& rect, UINT dpi, HWND hWnd) {
    if (g_placingJumpList && g_hasJumpListButton && !g_unloading) {
        wf::Rect moved = rect;
        moved.X = g_jumpListButton.X;
        moved.Width = g_jumpListButton.Width;
        return ToRawPixelsInDesktopCoordinates_Original(moved, dpi, hWnd);
    }
    return ToRawPixelsInDesktopCoordinates_Original(rect, dpi, hWnd);
}

// ---- Dragging apps to a new spot (as in "Taskbar Multirow") ----
// File Explorer has its own spot (see LayOutButtons), so dragging it doesn't
// start: the start, every move and the end of that drag are skipped. Any
// other app is dragged as usual, and the dock is laid out again once it's
// dropped.

winrt::weak_ref<wux::FrameworkElement> g_blockedDrag;  // File Explorer, while held

bool IsBlockedDrag(void* pThis) {
    try {
        auto blocked = g_blockedDrag.get();
        return blocked && ElementFromThis(pThis) == blocked;
    } catch (...) {
        return false;
    }
}

using TaskListButton_DragPoint_t = void(WINAPI*)(void* pThis, wf::Point point);
using TaskListButton_OnDragCompletedGesture_t = void(WINAPI*)(void* pThis);
TaskListButton_DragPoint_t TaskListButton_OnDragStartedGesture_Original;
void WINAPI TaskListButton_OnDragStartedGesture_Hook(void* pThis, wf::Point point) {
    if (!g_unloading) {
        try {
            auto button = ElementFromThis(pThis);
            if (button && IsFileExplorer(button)) {
                g_blockedDrag = winrt::make_weak(button);
                return;
            }
        } catch (...) {}
    }
    TaskListButton_OnDragStartedGesture_Original(pThis, point);
}

TaskListButton_DragPoint_t TaskListButton_UpdateDrag_Original;
void WINAPI TaskListButton_UpdateDrag_Hook(void* pThis, wf::Point point) {
    if (IsBlockedDrag(pThis)) return;
    TaskListButton_UpdateDrag_Original(pThis, point);
}

TaskListButton_OnDragCompletedGesture_t TaskListButton_OnDragCompletedGesture_Original;
void WINAPI TaskListButton_OnDragCompletedGesture_Hook(void* pThis) {
    if (IsBlockedDrag(pThis)) {
        g_blockedDrag = nullptr;
        return;
    }
    TaskListButton_OnDragCompletedGesture_Original(pThis);
    if (!g_unloading) MarkPlatesDirty();
}

// ---------------------------------------------------------------------------
// Taskbar height
// ---------------------------------------------------------------------------
// The taskbar window's height is decided by Explorer's own code, not by its
// XAML, so these hook the functions that report and apply it. Same strategy
// as m417z's "Taskbar height and icon size" mod, trimmed to the height only.
// Every hook passes straight through when g_taskbarHeight is 0.

std::atomic<bool> g_applyingHeight = false;

// enumTaskbarSize: 0 = small, 1 = regular, 2 = large.
bool OverrideHeight(int enumTaskbarSize) {
    return g_taskbarHeight > 0 && (enumTaskbarSize == 1 || enumTaskbarSize == 2);
}

using VoidMethod_t = void(WINAPI*)(void* pThis);
using SetDouble_t = void(WINAPI*)(void* pThis, double value);

// ---- Height of the taskbar window ----

using TaskbarConfiguration_GetFrameSize_t = double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    if (OverrideHeight(enumTaskbarSize)) return g_taskbarHeight;
    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}

// ---- Height of the system tray (clock, notification area) ----

using SystemTray_GetFrameSize_t = double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTray_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis, int enumTaskbarSize) {
    if (OverrideHeight(enumTaskbarSize)) return g_taskbarHeight;
    return SystemTrayController_GetFrameSize_Original(pThis, enumTaskbarSize);
}

// Same, for the taskbars on other monitors.
SystemTray_GetFrameSize_t SystemTraySecondaryController_GetFrameSize_Original;
double WINAPI SystemTraySecondaryController_GetFrameSize_Hook(void* pThis, int enumTaskbarSize) {
    if (OverrideHeight(enumTaskbarSize)) return g_taskbarHeight;
    return SystemTraySecondaryController_GetFrameSize_Original(pThis, enumTaskbarSize);
}

// While the tray resizes itself, make its frame exactly the taskbar's height.
thread_local bool g_inTrayUpdateFrameSize = false;

VoidMethod_t SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    g_inTrayUpdateFrameSize = true;
    SystemTrayController_UpdateFrameSize_Original(pThis);
    g_inTrayUpdateFrameSize = false;
}

VoidMethod_t SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    g_inTrayUpdateFrameSize = true;
    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);
    g_inTrayUpdateFrameSize = false;
}

SetDouble_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    if (g_inTrayUpdateFrameSize && g_taskbarHeight > 0) value = g_taskbarHeight;
    SystemTrayFrame_Height_Original(pThis, value);
}

// ---- The XAML frame inside the taskbar window ----

// Without lifting MaxHeight, the frame stays at the stock height.
SetDouble_t TaskbarFrame_MaxHeight_Original;

// Windows 11 22H2/23H2 set the frame's height through this.
SetDouble_t TaskbarFrame_Height_Original;
void WINAPI TaskbarFrame_Height_Hook(void* pThis, double value) {
    if (TaskbarFrame_MaxHeight_Original) {
        TaskbarFrame_MaxHeight_Original(pThis, std::numeric_limits<double>::infinity());
    }
    TaskbarFrame_Height_Original(pThis, value);
}

// Windows 11 24H2 and newer set it in TaskbarController::UpdateFrameHeight.
// The controller keeps its TaskbarFrame at an offset that's read from the
// start of OnGroupingModeChanged:  sub rsp, 28h / mov rax|r8, [rcx+offset]
void* TaskbarController_OnGroupingModeChanged_Address;

LONG GetTaskbarFrameOffset() {
    static LONG offset = []() -> LONG {
        const BYTE* p = (const BYTE*)TaskbarController_OnGroupingModeChanged_Address;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B && (p[6] & 0xC0) == 0x80) {
            LONG o = *(LONG*)(p + 7);
            if (o > 0 && o <= 0xFFFF) return o;
        }
        Wh_Log(L"Taskbar frame offset not found");
        return 0;
    }();
    return offset;
}

VoidMethod_t TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    wux::FrameworkElement frame{nullptr};
    try {
        LONG offset = GetTaskbarFrameOffset();
        void* taskbarFrame = offset ? *(void**)((BYTE*)pThis + offset) : nullptr;
        if (taskbarFrame) {
            ((::IUnknown**)taskbarFrame)[1]->QueryInterface(
                winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(frame));
        }
        if (frame) frame.MaxHeight(std::numeric_limits<double>::infinity());
    } catch (...) {}

    TaskbarController_UpdateFrameHeight_Original(pThis);

    if (!frame) return;
    // A taskbar, found as soon as its height is set (at startup too).
    if (!g_unloading) {
        try { NoteTaskbarFrame(frame); } catch (...) {}
    }

    // The grid holding the frame has a height of its own; keep it matching.
    try {
        auto grid = wuxm::VisualTreeHelper::GetParent(frame).try_as<wux::FrameworkElement>();
        if (grid) {
            double height = frame.Height();
            double gridHeight = grid.Height();
            if (gridHeight > 0 && gridHeight != height) grid.Height(height);
        }
    } catch (...) {}
}

// ---------------------------------------------------------------------------
// Icon resolution
// ---------------------------------------------------------------------------
// The taskbar loads each app's icon at the size it thinks icons are shown,
// normally 24px (times the display scaling), and anything bigger is that
// small image stretched. TaskbarConfiguration::GetIconHeightInViewPixels is
// where that size comes from, so it now reports g_iconLoadSize, and the icons
// load at full magnification. The icon images are still sized by
// ApplyIconSize, so they show at the dock's icon size.
//
// The taskbar also uses the icon height to lay out its buttons (padding,
// badges, the multi-window look), and some of that code tells the taskbar
// size settings apart by it, so all of that still gets the stock 24px, the
// same as before. Start, Task View and other system buttons get the dock's
// icon size. Adapted from m417z's "Taskbar height and icon size".

// TaskbarFrame::GetMetrics picks the button width from the icon height.
thread_local bool g_inTaskbarFrameGetMetrics = false;

using GetIconHeightInViewPixels_t = double(WINAPI*)(void* pThis);
GetIconHeightInViewPixels_t TaskbarConfiguration_GetIconHeightInViewPixels_Original;
double WINAPI TaskbarConfiguration_GetIconHeightInViewPixels_Hook(void* pThis) {
    double height = TaskbarConfiguration_GetIconHeightInViewPixels_Original(pThis);
    // 16 = the "small taskbar buttons" setting, which this mod doesn't support.
    if (g_iconLoadSize <= 0 || g_inTaskbarFrameGetMetrics || height <= 16) return height;
    return g_iconLoadSize;
}

using TaskbarFrame_GetMetrics_t = void*(WINAPI*)(void* pThis, void* metrics);
TaskbarFrame_GetMetrics_t TaskbarFrame_GetMetrics_Original;
void* WINAPI TaskbarFrame_GetMetrics_Hook(void* pThis, void* metrics) {
    g_inTaskbarFrameGetMetrics = true;
    void* ret = TaskbarFrame_GetMetrics_Original(pThis, metrics);
    g_inTaskbarFrameGetMetrics = false;
    return ret;
}

// ---- App buttons: layout keeps seeing 24px ----

// Where a TaskListButton keeps its icon height, read from its setter, which
// starts by loading the current value:  movsd xmmN, qword ptr [rcx+offset]
void* TaskListButton_IconHeight_Address;

size_t GetIconHeightOffset() {
    static size_t offset = []() -> size_t {
        BYTE* p = (BYTE*)TaskListButton_IconHeight_Address;
        std::regex regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                         std::regex_constants::icase);
        for (int i = 0; p && i < 30; i++) {
            WH_DISASM_RESULT result;
            if (!Wh_Disasm(p, &result)) break;
            p += result.length;
            std::string_view text = result.text;
            if (text == "ret") break;
            std::match_results<std::string_view::const_iterator> match;
            if (std::regex_match(text.begin(), text.end(), match, regex)) {
                size_t o = 0;
                std::from_chars(&*match[1].first, &*match[1].first + match[1].length(), o, 16);
                Wh_Log(L"Icon height offset: 0x%zX", o);
                return o <= 0xFFFF ? o : 0;
            }
        }
        Wh_Log(L"Icon height offset not found; icon resolution unchanged");
        return 0;
    }();
    return offset;
}

// Runs `original` with the button's icon height set to the stock 24px, then
// puts the real value back.
template <typename F>
void WithStockIconHeight(void* pThis, F original) {
    size_t offset = g_iconLoadSize > 0 ? GetIconHeightOffset() : 0;
    if (!offset) return original();
    double* iconHeight = (double*)((BYTE*)pThis + offset);
    double saved = *iconHeight;
    *iconHeight = kStockIconSize;
    original();
    *iconHeight = saved;
}

#define STOCK_ICON_HEIGHT_HOOK(name)                                              \
    VoidMethod_t TaskListButton_##name##_Original;                                \
    void WINAPI TaskListButton_##name##_Hook(void* pThis) {                       \
        WithStockIconHeight(pThis, [=] { TaskListButton_##name##_Original(pThis); }); \
    }

STOCK_ICON_HEIGHT_HOOK(UpdateMultiWindowClip)
STOCK_ICON_HEIGHT_HOOK(CreateMultiWindowClip)
STOCK_ICON_HEIGHT_HOOK(UpdateBadge)

// These two also run whenever a button is set up or changes state (hover,
// active window, a second window...), which is when the dock applies its
// changes to it (see OnTaskListButtonUpdated).
VoidMethod_t TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    WithStockIconHeight(pThis, [=] { TaskListButton_UpdateButtonPadding_Original(pThis); });
    OnTaskListButtonUpdated(pThis);
}

VoidMethod_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    WithStockIconHeight(pThis, [=] { TaskListButton_UpdateVisualStates_Original(pThis); });
    OnTaskListButtonUpdated(pThis);
}

// Sets the width of the button's icon column back to the stock one; widen it
// again right after (see ApplyIconColumn).
VoidMethod_t TaskListButton_UpdateIconColumnDefinition_Original;
void WINAPI TaskListButton_UpdateIconColumnDefinition_Hook(void* pThis) {
    TaskListButton_UpdateIconColumnDefinition_Original(pThis);
    OnTaskListButtonUpdated(pThis);
}

using TaskListButton_OverlayIcon_t = void(WINAPI*)(void* pThis, void* stream);
TaskListButton_OverlayIcon_t TaskListButton_OverlayIcon_Original;
void WINAPI TaskListButton_OverlayIcon_Hook(void* pThis, void* stream) {
    WithStockIconHeight(pThis, [=] { TaskListButton_OverlayIcon_Original(pThis, stream); });
}

// ---- Start, Task View and other system buttons ----

// Their icon is shown at the dock's icon size, not the loading size.
SetDouble_t ExperienceToggleButton_IconHeight_Original;
void WINAPI ExperienceToggleButton_IconHeight_Hook(void* pThis, double height) {
    if (g_iconLoadSize > 0 && height > 16) height = g_iconSize;
    ExperienceToggleButton_IconHeight_Original(pThis, height);
}

// Their padding is worked out for the stock 24px, as before. Setting the
// height calls UpdateButtonPadding again, which the flag skips.
using GetDouble_t = double(WINAPI*)(void* pThis);
GetDouble_t ExperienceToggleButton_IconHeight_get;
thread_local bool g_settingToggleIconHeight = false;

void SetToggleIconHeight(void* pThis, double height) {
    g_settingToggleIconHeight = true;
    ExperienceToggleButton_IconHeight_Original(pThis, height);
    g_settingToggleIconHeight = false;
}

VoidMethod_t ExperienceToggleButton_UpdateButtonPadding_Original;
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    if (g_settingToggleIconHeight) return;
    double height = kStockIconSize;
    if (g_iconLoadSize > 0 && ExperienceToggleButton_IconHeight_get &&
        ExperienceToggleButton_IconHeight_Original) {
        height = ExperienceToggleButton_IconHeight_get(pThis);
    }
    bool swap = height > 16 && height != kStockIconSize;
    if (swap) SetToggleIconHeight(pThis, kStockIconSize);
    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    if (swap) SetToggleIconHeight(pThis, height);
}

// Taskbar extensions such as widgets: shown at the dock's icon size too.
SetDouble_t TaskbarComponentHost_IconHeight_Original;
void WINAPI TaskbarComponentHost_IconHeight_Hook(void* pThis, double height) {
    if (g_iconLoadSize > 0 && height > 16) height = g_iconSize;
    TaskbarComponentHost_IconHeight_Original(pThis, height);
}

// ---- Making Explorer apply a new height ----

// Explorer re-reads the height when it handles a settings change; this makes
// it also resize the taskbar window right away.
VoidMethod_t TrayUI__StuckTrayChange_Original;

using TrayUI__HandleSettingChange_t = void(WINAPI*)(void*, void*, void*, void*, void*);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis, void* p1, void* p2, void* p3,
                                             void* p4) {
    TrayUI__HandleSettingChange_Original(pThis, p1, p2, p3, p4);
    if (g_applyingHeight) TrayUI__StuckTrayChange_Original(pThis);
}

void ApplyTaskbarHeight() {
    HWND tb = FindTaskbar();
    if (!tb) return;
    g_applyingHeight = true;
    SendMessage(tb, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    g_applyingHeight = false;

    // Make the taskbar reload its icons, at the new resolution.
    if (HWND rebar = FindWindowEx(tb, nullptr, L"ReBarWindow32", nullptr)) {
        if (HWND taskSw = FindWindowEx(rebar, nullptr, L"MSTaskSwWClass", nullptr)) {
            SendMessage(taskSw, 0x452, 3, 0);
        }
    }
}

// ---- Magnify over windows ----
// Maximized windows reach down to the top of the dock, and magnified icons
// rise over them, as on a Mac. Things Windows already does make that happen,
// managed by Explorer itself:
// - "Automatically hide the taskbar" stops Windows from keeping any space
//   free for the taskbar. It's turned on (and back off afterwards, if it was
//   off), and the taskbar is kept from actually hiding (as in "Taskbar
//   auto-hide when maximized").
// - An app bar, the way docked toolbars keep a strip of the screen free,
//   with just the dock's height, on each screen with a taskbar.
// - A window region: the taskbar window, which is taller than the dock to
//   leave room for magnified icons, is clipped to that strip while no icon is
//   magnified, so it doesn't cover the bottom of maximized windows.

// TrayUI::_HandleTrayPrivateSettingMessage: gets or sets auto-hide.
constexpr UINT kTraySettingMessage = WM_USER + 0x1CA;
constexpr WPARAM kTraySettingAutoHideGet = 3;
constexpr WPARAM kTraySettingAutoHideSet = 4;
constexpr UINT_PTR kTrayHideTimer = 2;

// Whether the mod has turned auto-hide on, and whether it was on before,
// to put it back as it was. Saved, so that's still done after Explorer
// restarts unexpectedly.
bool g_managingAutoHide = false;
bool g_autoHideBefore = false;

// Auto-hide is a per-user setting, and the mod's storage may be shared by
// every user of the PC, so what's saved is per user.
std::wstring UserValueName(PCWSTR name) {
    static std::wstring sid = [] {
        std::wstring result;
        HANDLE token;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return result;
        alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
        DWORD size;
        PWSTR text;
        if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size) &&
            ConvertSidToStringSidW(((TOKEN_USER*)buffer)->User.Sid, &text)) {
            result = text;
            LocalFree(text);
        }
        CloseHandle(token);
        return result;
    }();
    return std::wstring(name) + L"_" + sid;
}

// "Magnify over windows": auto-hide is on, but the taskbar never hides.
// Auto-hide, when set, wins: the taskbar is let hide, as Windows does it.
bool KeepTaskbarShown() { return g_magnifyOverWindows && !g_autoHide && !g_unloading; }

// Windows' auto-hide is wanted for both: for Auto-hide, so the dock hides,
// and for "Magnify over windows", so Windows keeps no space for it.
bool WantWindowsAutoHide() { return (g_magnifyOverWindows || g_autoHide) && !g_unloading; }

void ApplyAutoHide() {
    HWND tb = FindTaskbar();
    if (!tb) return;
    if (WantWindowsAutoHide()) {
        if (!g_managingAutoHide) {
            g_autoHideBefore = SendMessage(tb, kTraySettingMessage, kTraySettingAutoHideGet, 0);
            g_managingAutoHide = true;
        }
        SendMessage(tb, kTraySettingMessage, kTraySettingAutoHideSet, TRUE);
    } else if (g_managingAutoHide) {
        SendMessage(tb, kTraySettingMessage, kTraySettingAutoHideSet, g_autoHideBefore);
        g_managingAutoHide = false;
    }
    Wh_Log(L"[auto-hide] mode: %s; Windows auto-hide managed by the mod: %d (was %d before)",
           g_unloading            ? L"off (unloading)"
           : g_autoHide           ? L"dock hides automatically"
           : g_magnifyOverWindows ? L"magnify over windows (taskbar kept shown)"
                                  : L"off",
           g_managingAutoHide, g_autoHideBefore);
    Wh_SetIntValue(UserValueName(L"managingAutoHide").c_str(), g_managingAutoHide);
    Wh_SetIntValue(UserValueName(L"autoHideBefore").c_str(), g_autoHideBefore);
}

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    if (KeepTaskbarShown()) {
        if (HWND tb = FindTaskbar()) KillTimer(tb, kTrayHideTimer);
        return;
    }
    TrayUI__Hide_Original(pThis);
}

// The same, for the taskbars on other screens.
using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
    if (KeepTaskbarShown()) return;
    CSecondaryTray__AutoHide_Original(pThis, param1);
}

// Calls `f` with each taskbar window (one per screen).
template <typename F>
void ForEachTaskbarWindow(F f) {
    EnumWindows(
        [](HWND w, LPARAM l) -> BOOL {
            DWORD pid;
            WCHAR cls[32];
            if (GetWindowThreadProcessId(w, &pid) && pid == GetCurrentProcessId() &&
                GetClassName(w, cls, ARRAYSIZE(cls)) &&
                (!_wcsicmp(cls, L"Shell_TrayWnd") || !_wcsicmp(cls, L"Shell_SecondaryTrayWnd"))) {
                (*(F*)l)(w);
            }
            return TRUE;
        },
        (LPARAM)&f);
}

// For the log: every taskbar window, with its thread and place on screen.
void LogTaskbarWindows() {
    ForEachTaskbarWindow([](HWND tb) {
        WCHAR cls[32] = L"";
        GetClassName(tb, cls, ARRAYSIZE(cls));
        RECT r{};
        GetWindowRect(tb, &r);
        Wh_Log(L"Taskbar window %p %s: thread %u, rect %d,%d %dx%d", tb, cls,
               GetWindowThreadProcessId(tb, nullptr), (int)r.left, (int)r.top,
               (int)(r.right - r.left), (int)(r.bottom - r.top));
    });
}

// The strip kept free at the bottom of the screen, in pixels: the dock and
// the gap under it, or the system tray (with its glass) if that's taller.
int StripHeight(UINT dpi) {
    float tray = g_trayGlass ? g_dockBottomGap + TrayGlassHeight() : (float)kStockTaskbarHeight;
    float height = std::max(g_dockBottomGap + g_dockHeight, tray);
    return MulDiv((int)std::ceil(height), dpi, 96);
}

void ClipTaskbars() {
    ForEachTaskbarWindow([](HWND tb) {
        RECT window, have;
        if (!GetWindowRect(tb, &window)) return;
        int type = GetWindowRgnBox(tb, &have);
        if (!KeepTaskbarShown() || g_dockExpanded) {
            if (type != ERROR) SetWindowRgn(tb, nullptr, TRUE);
            return;
        }
        RECT want{0, 0, window.right - window.left, window.bottom - window.top};
        want.top = std::max(0, (int)want.bottom - StripHeight(GetDpiForWindow(tb)));
        if (type == SIMPLEREGION && EqualRect(&have, &want)) return;
        SetWindowRgn(tb, CreateRectRgnIndirect(&want), TRUE);
    });
}

struct AppBar {
    HWND hwnd;
    RECT rect;  // the strip it keeps free
};
std::vector<AppBar> g_appBars;  // one per screen with a taskbar

void RemoveAppBars() {
    for (auto& bar : g_appBars) {
        APPBARDATA abd{sizeof(abd)};
        abd.hWnd = bar.hwnd;
        SHAppBarMessage(ABM_REMOVE, &abd);
        DestroyWindow(bar.hwnd);
    }
    g_appBars.clear();
}

// Keeps `rect` free, at the bottom of a screen.
void AddAppBar(RECT const& rect) {
    HWND w = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWindowClass, L"", WS_POPUP, 0, 0,
                            0, 0, nullptr, nullptr, ModuleInstance(), nullptr);
    if (!w) return;
    g_appBars.push_back({w, rect});

    APPBARDATA abd{sizeof(abd)};
    abd.hWnd = w;
    abd.uCallbackMessage = kAppBarMessage;
    SHAppBarMessage(ABM_NEW, &abd);
    abd.uEdge = ABE_BOTTOM;
    abd.rc = rect;
    SHAppBarMessage(ABM_QUERYPOS, &abd);
    abd.rc.top = abd.rc.bottom - (rect.bottom - rect.top);
    SHAppBarMessage(ABM_SETPOS, &abd);
    Wh_Log(L"[magnify over windows] keeping %d px free at the bottom of a screen",
           (int)(rect.bottom - rect.top));
}

// Makes the app bars match the screens with a taskbar. Only changes them
// when they don't match: changing them is announced to every app bar, these
// included, which would otherwise make them again, and again.
void UpdateAppBars() {
    std::vector<RECT> want;
    if (KeepTaskbarShown()) {
        ForEachTaskbarWindow([&want](HWND tb) {
            HMONITOR monitor = MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST);
            MONITORINFO info{sizeof(info)};
            UINT dpiX, dpiY;
            if (!GetMonitorInfo(monitor, &info) ||
                FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
                return;
            }
            RECT rect = info.rcMonitor;
            rect.top = rect.bottom - StripHeight(dpiY);
            want.push_back(rect);
        });
    }
    bool same = want.size() == g_appBars.size();
    for (size_t i = 0; same && i < want.size(); i++) {
        same = EqualRect(&want[i], &g_appBars[i].rect);
    }
    if (same) return;
    RemoveAppBars();
    if (want.empty() || !RegisterWindowClass()) return;
    for (auto& rect : want) AddAppBar(rect);
}

// Updates the app bars and the clipping, and auto-hide too if `autoHide`, on
// the taskbar's thread, from its message loop: outside the taskbar's own
// code, and once for any number of requests until then.
int g_overWindowsPending = 0;  // 1 = update, 2 = and auto-hide

void ScheduleOverWindows(bool autoHide) {
    bool post = g_overWindowsPending == 0;
    g_overWindowsPending = std::max(g_overWindowsPending, autoHide ? 2 : 1);
    if (!post) return;
    HWND w = EnsureModWindow();
    if (!w || !PostMessage(w, kUpdateOverWindowsMessage, 0, 0)) g_overWindowsPending = 0;
}

void UpdateOverWindows() {
    int pending = std::exchange(g_overWindowsPending, 0);
    if (g_unloading || !pending) return;
    if (pending == 2) ApplyAutoHide();
    UpdateAppBars();
    ClipTaskbars();
}

// Applies "Magnify over windows" as it's now set. Also done whenever a
// taskbar appears (see NoteTaskbarFrame), since the mod is often loaded
// before there is one.
void ApplyMagnifyOverWindows() {
    if (HWND tb = FindTaskbar()) {
        RunFromWindowThread(tb, [](void*) { ScheduleOverWindows(true); }, nullptr);
    }
}

LRESULT CALLBACK ModWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case kAppBarMessage:
            if (wParam == ABN_POSCHANGED) ScheduleOverWindows(false);
            return 0;
        case WM_DISPLAYCHANGE:
            ScheduleOverWindows(false);
            break;
        case kUpdateOverWindowsMessage:
            UpdateOverWindows();
            return 0;
        case kTrashChangedMessage:
            CheckRecycleBinSoon();
            return 0;
        case kTrashCheckedMessage:
            if (!g_unloading) {
                try { UpdateTrashIcons(); } catch (...) {}
            }
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// ---- Installing the hooks ----

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
            &TrayUI__StuckTrayChange_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &TrayUI__HandleSettingChange_Original,
            TrayUI__HandleSettingChange_Hook,
        },
        // Magnify over windows: the taskbar doesn't auto-hide
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
            &CSecondaryTray__AutoHide_Original,
            CSecondaryTray__AutoHide_Hook,
            true,
        },
        // Where a button's right-click menu opens
        {
            {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
            &CTaskListWnd_ComputeJumpViewPosition_Original,
            CTaskListWnd_ComputeJumpViewPosition_Hook,
            true,
        },
        {
            {LR"(struct tagRECT __cdecl ToRawPixelsInDesktopCoordinates(struct winrt::Windows::Foundation::Rect const &,unsigned int,struct HWND__ *))"},
            &ToRawPixelsInDesktopCoordinates_Original,
            ToRawPixelsInDesktopCoordinates_Hook,
            true,
        },
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// The name the symbol extraction script expects for a list of hooks that
// isn't one of the hook arrays itself.
using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;

// The system tray's hooks. Newer builds keep the tray in SystemTray.dll,
// older ones in Taskbar.View.dll (see TaskbarViewHasTray).
std::vector<COMBINED_SH> GetTrayHooks() {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK trayHooks[] = {
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_Original,
            SystemTrayController_UpdateFrameSize_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,
        },
    };
    return {std::make_move_iterator(std::begin(trayHooks)),
            std::make_move_iterator(std::end(trayHooks))};
}

bool HookTaskbarViewSymbols(HMODULE module, bool withTray) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &TaskbarConfiguration_GetFrameSize_Original,
            TaskbarConfiguration_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )"},
            &TaskbarFrame_MaxHeight_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )"},
            &TaskbarFrame_Height_Original,
            TaskbarFrame_Height_Hook,
            true,  // Gone in 24H2.
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
            &TaskbarController_OnGroupingModeChanged_Address,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
            &TaskbarController_UpdateFrameHeight_Original,
            TaskbarController_UpdateFrameHeight_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &TaskbarFrame_OnPointerExited_Original,
            TaskbarFrame_OnPointerExited_Hook,
            true,
        },

        // Where a button's right-click menu opens
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarResources::OnTaskListButtonContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
            &TaskbarResources_OnTaskListButtonContextRequested_Original,
            TaskbarResources_OnTaskListButtonContextRequested_Hook,
            true,
        },
        {
            {LR"(public: static void __cdecl winrt::Taskbar::implementation::TaskListButtonHandlers::HandleContextRequested(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs const &))"},
            &TaskListButtonHandlers_HandleContextRequested_Original,
            TaskListButtonHandlers_HandleContextRequested_Hook,
            true,
        },

        // Dragging apps to a new spot
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragStartedGesture(struct winrt::Windows::Foundation::Point))"},
            &TaskListButton_OnDragStartedGesture_Original,
            TaskListButton_OnDragStartedGesture_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateDrag(struct winrt::Windows::Foundation::Point))"},
            &TaskListButton_UpdateDrag_Original,
            TaskListButton_UpdateDrag_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::OnDragCompletedGesture(void))"},
            &TaskListButton_OnDragCompletedGesture_Original,
            TaskListButton_OnDragCompletedGesture_Hook,
            true,
        },

        // Icon resolution
        {
            {LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"},
            &TaskbarConfiguration_GetIconHeightInViewPixels_Original,
            TaskbarConfiguration_GetIconHeightInViewPixels_Hook,
            true,
        },
        {
            {LR"(public: struct winrt::Taskbar::implementation::TaskbarFrameMetrics __cdecl winrt::Taskbar::implementation::TaskbarFrame::GetMetrics(void)const )"},
            &TaskbarFrame_GetMetrics_Original,
            TaskbarFrame_GetMetrics_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::IconHeight(double))"},
            &TaskListButton_IconHeight_Address,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
            &TaskListButton_UpdateButtonPadding_Original,
            TaskListButton_UpdateButtonPadding_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"},
            &TaskListButton_UpdateIconColumnDefinition_Original,
            TaskListButton_UpdateIconColumnDefinition_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateMultiWindowClip(void))"},
            &TaskListButton_UpdateMultiWindowClip_Original,
            TaskListButton_UpdateMultiWindowClip_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::CreateMultiWindowClip(void))"},
            &TaskListButton_CreateMultiWindowClip_Original,
            TaskListButton_CreateMultiWindowClip_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadge(void))"},
            &TaskListButton_UpdateBadge_Original,
            TaskListButton_UpdateBadge_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
            &TaskListButton_OverlayIcon_Original,
            TaskListButton_OverlayIcon_Hook,
            true,
        },
        {
            {LR"(public: double __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(void)const )"},
            &ExperienceToggleButton_IconHeight_get,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(double))"},
            &ExperienceToggleButton_IconHeight_Original,
            ExperienceToggleButton_IconHeight_Hook,
            true,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            &ExperienceToggleButton_UpdateButtonPadding_Original,
            ExperienceToggleButton_UpdateButtonPadding_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Microsoft::Windows::Taskbar::implementation::TaskbarComponentHost::IconHeight(double))"},
            &TaskbarComponentHost_IconHeight_Original,
            TaskbarComponentHost_IconHeight_Hook,
            true,
        },
    };
    // On builds where the tray is in Taskbar.View.dll too, its hooks go in the
    // same call, so the module's symbols are only looked up once.
    std::vector<COMBINED_SH> allHooks(std::make_move_iterator(std::begin(taskbarViewHooks)),
                                      std::make_move_iterator(std::end(taskbarViewHooks)));
    if (withTray) {
        for (auto& hook : GetTrayHooks()) allHooks.push_back(std::move(hook));
    }
    if (!WindhawkUtils::HookSymbols(module, allHooks.data(), allHooks.size())) return false;
    if (TaskListButton_IconHeight_Address) GetIconHeightOffset();  // find it up front
    return true;
}

bool HookTraySymbols(HMODULE module) {
    auto systemTrayHooks = GetTrayHooks();
    return WindhawkUtils::HookSymbols(module, systemTrayHooks.data(), systemTrayHooks.size());
}

// Taskbar.View.dll before version 2604 contains the system tray too; newer
// builds moved it into SystemTray.dll.
bool TaskbarViewHasTray(HMODULE module) {
    HRSRC res = FindResource(module, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    HGLOBAL global = res ? LoadResource(module, res) : nullptr;
    void* data = global ? LockResource(global) : nullptr;
    VS_FIXEDFILEINFO* info = nullptr;
    UINT size = 0;
    if (!data || !VerQueryValue(data, L"\\", (void**)&info, &size) || !size) return false;
    return HIWORD(info->dwFileVersionMS) < 2604;
}

std::atomic<bool> g_taskbarViewHooked = false;
std::atomic<bool> g_trayHooked = false;

// Hooks whichever of the taskbar's DLLs are loaded and not hooked yet.
// Returns true if anything new was hooked.
bool HookTaskbarModules() {
    bool hooked = false;
    if (HMODULE view = GetModuleHandle(L"Taskbar.View.dll");
        view && !g_taskbarViewHooked.exchange(true)) {
        bool withTray = !GetModuleHandle(L"SystemTray.dll") && TaskbarViewHasTray(view) &&
                        !g_trayHooked.exchange(true);
        HookTaskbarViewSymbols(view, withTray);
        hooked = true;
    }
    if (HMODULE tray = GetModuleHandle(L"SystemTray.dll");
        tray && !g_trayHooked.exchange(true)) {
        HookTraySymbols(tray);
        hooked = true;
    }
    return hooked;
}

// The taskbar's DLLs load late when Explorer starts after the mod.
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module && (!g_taskbarViewHooked || !g_trayHooked) && HookTaskbarModules()) {
        Wh_ApplyHookOperations();
    }
    return module;
}

// ---------------------------------------------------------------------------
// Start menu (StartMenuExperienceHost.exe)
// ---------------------------------------------------------------------------
// The mod also loads into the Start menu's process, where it does only this:
// the Start menu keeps just the pinned apps' icons, on glass like the dock's.
// As in "Compact Start Menu", the Start menu's XAML tree is walked on its
// window's thread every 250 ms (it builds and recycles elements as it opens
// and scrolls), and elements are hidden by their names. The names come from
// that mod and from the themes of "Windows 11 Start Menu Styler", for both the
// redesigned and the classic Start menu; which ones are found is logged once.

bool IsStartMenuProcess() {
    WCHAR path[MAX_PATH];
    DWORD length = GetModuleFileName(nullptr, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) return false;
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"StartMenuExperienceHost.exe") == 0;
}

bool g_isStartMenuProcess = false;

// Hidden wherever they are, by name: the search box, the Pinned header with
// its "Show all" button and page dots, Recommended, the All apps heading and
// page (classic), and the phone panel and its button. AcrylicOverlay is a
// layer over the background, which the glass replaces (as in LiquidGlass2).
constexpr PCWSTR kStartHiddenNames[] = {
    L"SearchBoxToggleButton",  L"StartMenuSearchBox",
    L"PinnedListHeaderGrid",   L"PinnedListHeaderText",
    L"ShowMorePinnedGrid",     L"ShowMorePinnedButton",
    L"PinnedListPipsPager",    L"TopLevelSuggestionsRoot",
    L"TopLevelSuggestionsListHeader", L"TopLevelSuggestionsContainer",
    L"ShowMoreSuggestions",    L"NoTopLevelSuggestionsText",
    L"AllListHeading",         L"AllAppsPaneHeader",
    L"AllAppsRoot",            L"ShowAllAppsButton",
    L"ShowHideCompanion",      L"RightCompanionContainerGrid",
    L"AcrylicOverlay",
};

// The search box's names among those (kept with the Search box setting).
bool IsStartSearchBox(winrt::hstring const& name) {
    return name == L"SearchBoxToggleButton" || name == L"StartMenuSearchBox";
}

struct StartHiddenElement {
    winrt::weak_ref<wux::FrameworkElement> element;
    wf::IInspectable originalVisibility{nullptr};  // local value, may be UnsetValue
    int64_t token = 0;
};

// A property the mod set on one of the Start menu's elements, with the
// element's own (local) value to put back.
struct StartChangedValue {
    winrt::weak_ref<wux::FrameworkElement> element;
    wux::DependencyProperty property{nullptr};
    wf::IInspectable original{nullptr};  // may be UnsetValue
};

// The glass behind the Start menu, in its background border.
struct StartBackground {
    winrt::weak_ref<wuxc::Border> border;
    wf::IInspectable originalBackground{nullptr};
    wf::IInspectable originalBorderThickness{nullptr};
    Glass glass;
    float radius = 8;
    wux::FrameworkElement::ActualThemeChanged_revoker themeChanged;
};

// All on the Start menu's thread.
[[clang::no_destroy]] std::vector<StartHiddenElement> g_startHidden;
// The hidden elements' COM identities, to tell quickly whether one is known.
// Gone elements are pruned now and then (see StartProcessTree).
[[clang::no_destroy]] std::unordered_set<void*> g_startHiddenIds;
[[clang::no_destroy]] std::vector<StartChangedValue> g_startChanged;
[[clang::no_destroy]] std::optional<StartBackground> g_startBackground;
[[clang::no_destroy]] std::vector<std::wstring> g_startLogged;
[[clang::no_destroy]] wux::DispatcherTimer g_startTimer{nullptr};

// Whether `key` is new, for logging things once.
bool StartFirstTime(std::wstring const& key) {
    if (std::find(g_startLogged.begin(), g_startLogged.end(), key) != g_startLogged.end()) {
        return false;
    }
    g_startLogged.push_back(key);
    return true;
}

// Whether an ancestor of `el`, at most `depth` levels up, matches `match`.
template <typename F>
bool HasAncestor(wux::DependencyObject const& el, int depth, F match) {
    auto current = wuxm::VisualTreeHelper::GetParent(el);
    for (int i = 0; current && i < depth; i++) {
        if (auto fe = current.try_as<wux::FrameworkElement>(); fe && match(fe)) return true;
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    return false;
}

// Collapses `el`, keeping it collapsed, and remembers it to show it again.
// `what` names it in the log.
void* ComIdentity(wf::IInspectable const& object) {
    return winrt::get_abi(object.as<wf::IUnknown>());
}

void StartHide(wux::FrameworkElement const& el, PCWSTR what) {
    void* id = ComIdentity(el);
    if (g_startHiddenIds.count(id)) {
        if (el.Visibility() != wux::Visibility::Collapsed) {
            el.Visibility(wux::Visibility::Collapsed);
        }
        return;
    }
    g_startHiddenIds.insert(id);
    StartHiddenElement h;
    h.element = winrt::make_weak(el);
    h.originalVisibility = el.ReadLocalValue(wux::UIElement::VisibilityProperty());
    // The Start menu shows some of these again itself (visual states); put
    // them back whenever it does.
    h.token = el.RegisterPropertyChangedCallback(
        wux::UIElement::VisibilityProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            auto e = sender.try_as<wux::UIElement>();
            if (e && e.Visibility() != wux::Visibility::Collapsed) {
                e.Visibility(wux::Visibility::Collapsed);
            }
        });
    el.Visibility(wux::Visibility::Collapsed);
    g_startHidden.push_back(std::move(h));
    if (StartFirstTime(what)) {
        Wh_Log(L"Start menu: hiding %s (%s)", what, winrt::get_class_name(el).c_str());
    }
}

// Forgets hidden elements that are gone (the Start menu replaced them).
void StartPruneHidden() {
    std::erase_if(g_startHidden, [](StartHiddenElement const& h) { return !h.element.get(); });
    g_startHiddenIds.clear();
    for (auto& h : g_startHidden) {
        if (auto el = h.element.get()) g_startHiddenIds.insert(ComIdentity(el));
    }
    std::erase_if(g_startChanged, [](StartChangedValue const& c) { return !c.element.get(); });
}

// A pinned app's name, under its icon.
bool IsPinnedAppName(wux::FrameworkElement const& el, winrt::hstring const& name) {
    return name == L"DisplayNameAndDownloadIconContainer" &&
           HasAncestor(el, 3, [](wux::FrameworkElement const& a) {
               return winrt::get_class_name(a) == L"StartMenu.PinnedListTile";
           });
}

// The panel that lays out the All apps list. In the redesigned Start menu,
// the pinned apps and Recommended are in the header of the same scrolling
// list (AllAppsGrid), so only its items panel is hidden, not the list. The
// nearest named list above it tells which list it belongs to.
constexpr std::wstring_view kItemsWrapGrid = L"Windows.UI.Xaml.Controls.ItemsWrapGrid";
constexpr std::wstring_view kItemsStackPanel = L"Windows.UI.Xaml.Controls.ItemsStackPanel";

bool IsAllAppsItemsPanel(wux::FrameworkElement const& el, std::wstring_view type) {
    if (type != kItemsWrapGrid && type != kItemsStackPanel) return false;
    bool allApps = false;
    HasAncestor(el, 12, [&allApps](wux::FrameworkElement const& a) {
        auto name = a.Name();
        if (name == L"PinnedList" || name == L"StartMenuPinnedList" ||
            name == L"SideBySidePinnedWrapper") {
            return true;  // a pinned list: not All apps
        }
        if (name == L"AllAppsGrid" || name == L"AppsList" || name == L"ZoomedOutListView") {
            allApps = true;
            return true;
        }
        return false;
    });
    return allApps;
}

// Sets `el`'s `property` to `value`, unless it already has it, remembering
// its own value the first time. `same` compares the current value with the
// wanted one.
template <typename T, typename Same>
void StartSet(wux::FrameworkElement const& el, wux::DependencyProperty const& property,
              T const& value, Same same) {
    if (same(winrt::unbox_value<T>(el.GetValue(property)), value)) return;
    bool known = false;
    for (auto& c : g_startChanged) {
        if (c.property == property && c.element.get() == el) {
            known = true;
            break;
        }
    }
    if (!known) {
        g_startChanged.push_back(
            {winrt::make_weak(el), property, el.ReadLocalValue(property)});
    }
    el.SetValue(property, winrt::box_value(value));
}

void StartSetDouble(wux::FrameworkElement const& el, wux::DependencyProperty const& property,
                    double value) {
    // NaN (Auto) never equals itself; setting it again each time would make
    // the Start menu lay itself out again on every pass.
    StartSet(el, property, value, [](double a, double b) {
        return (std::isnan(a) && std::isnan(b)) || std::fabs(a - b) < 0.01;
    });
}

void StartSetThickness(wux::FrameworkElement const& el, wux::DependencyProperty const& property,
                       wux::Thickness value) {
    StartSet(el, property, value, [](wux::Thickness const& a, wux::Thickness const& b) {
        return a.Left == b.Left && a.Top == b.Top && a.Right == b.Right && a.Bottom == b.Bottom;
    });
}

template <typename E>
void StartSetEnum(wux::FrameworkElement const& el, wux::DependencyProperty const& property,
                  E value) {
    StartSet(el, property, value, [](E a, E b) { return a == b; });
}

// The pinned apps' cells, as in Windows' own grid (96 x 84): the columns
// share the list's width, and a row is 7/8 as tall as a column is wide. Never
// less than the icon with some room around it.
constexpr double kStartCellAspect = 84.0 / 96.0;

double StartCellWidth(double listWidth) {
    double minimum = g_startIconSize + 8;
    if (listWidth <= 0) return std::round(g_startIconSize * 2);
    return std::max(minimum, std::floor(listWidth / g_startColumns));
}

double StartCellHeight(double cellWidth) {
    return std::max(g_startIconSize + 8.0, std::round(cellWidth * kStartCellAspect));
}

// The first ancestor of `el`, at most `depth` levels up, that matches `match`.
template <typename F>
wux::FrameworkElement FindAncestor(wux::DependencyObject const& el, int depth, F match) {
    auto current = wuxm::VisualTreeHelper::GetParent(el);
    for (int i = 0; current && i < depth; i++) {
        if (auto fe = current.try_as<wux::FrameworkElement>(); fe && match(fe)) return fe;
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    return nullptr;
}

// The pinned apps' grid, found by StartLayOutPinned, and the shift that puts
// it in the middle of the Start menu (see StartCenterPinned).
[[clang::no_destroy]] winrt::weak_ref<wux::FrameworkElement> g_startPinnedGrid;
[[clang::no_destroy]] wuxm::TranslateTransform g_startPinnedShift{nullptr};
[[clang::no_destroy]] std::wstring g_startLayoutLogged;
bool g_startChainLogged = false;

// For the log, once: the pinned grid's containers up to the root, with their
// sizes, to see how the Start menu is put together on this Windows build.
void LogPinnedChain(wux::FrameworkElement const& grid) {
    if (g_startChainLogged) return;
    g_startChainLogged = true;
    std::wstring chain;
    wux::DependencyObject current = grid;
    for (int i = 0; current && i < 30; i++) {
        if (auto fe = current.try_as<wux::FrameworkElement>()) {
            WCHAR part[200];
            swprintf_s(part, L"%s%s#%s %.0fx%.0f", i ? L" < " : L"",
                       winrt::get_class_name(fe).c_str(), fe.Name().c_str(), fe.ActualWidth(),
                       fe.ActualHeight());
            chain += part;
        }
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    Wh_Log(L"Start menu: pinned grid in %s", chain.c_str());
}

// Whether `el` is in the pinned apps' list.
bool IsInPinnedList(wux::FrameworkElement const& el, int depth) {
    return HasAncestor(el, depth, [](wux::FrameworkElement const& a) {
        auto name = a.Name();
        return name == L"PinnedList" || name == L"StartMenuPinnedList";
    });
}

// Makes every container between the pinned grid and the pinned list (`list`)
// tall enough for the whole grid. The Start menu sizes them for its own rows
// (84 px), so with taller rows the last ones were cut off. The grid's own
// height is all its rows, since a scrolling view measures it without a limit.
void StartFitPinned(wux::FrameworkElement const& list, wux::FrameworkElement const& grid) {
    double gridHeight = grid.ActualHeight();
    wux::DependencyObject current = wuxm::VisualTreeHelper::GetParent(grid);
    for (int i = 0; current && i < 20; i++) {
        if (auto el = current.try_as<wux::FrameworkElement>()) {
            // Where the grid's bottom is in this container. The shift is a
            // render transform on `list`, which moves `list` and the grid
            // together, so it doesn't change this.
            double bottom = grid.TransformToVisual(el).TransformPoint({0, 0}).Y + gridHeight;
            double want = std::ceil(bottom);
            if (el.ActualHeight() < want - 0.5) {
                std::wstring what = std::wstring(winrt::get_class_name(el)) + L"#" +
                                    std::wstring(el.Name());
                if (StartFirstTime(L"fit " + what)) {
                    Wh_Log(L"Start menu: %s was %.0f tall, made %.0f so the pinned apps fit",
                           what.c_str(), el.ActualHeight(), want);
                }
                StartSetDouble(el, wux::FrameworkElement::HeightProperty(), want);
                StartSetDouble(el, wux::FrameworkElement::MaxHeightProperty(),
                               std::numeric_limits<double>::infinity());
            }
            if (el == list) break;
        }
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
}

// Moves the pinned list (`list`) so that its grid of apps is at the top, in
// the middle or at the bottom (Icons position) of what the Start menu shows
// of it, the nearest scrolling view around it, then by the offsets. It's
// moved with a render transform, which doesn't change the Start menu's
// layout, so this works however the containers around it are laid out.
void StartCenterPinned(wux::FrameworkElement const& list) {
    auto grid = g_startPinnedGrid.get();
    if (!grid || grid.ActualHeight() <= 0) return;
    StartFitPinned(list, grid);
    auto viewer = FindAncestor(list, 20, [](wux::FrameworkElement const& a) {
        return a.try_as<wuxc::ScrollViewer>() != nullptr;
    });
    if (!viewer || viewer.ActualHeight() <= 0) return;

    if (!g_startPinnedShift || list.RenderTransform() != g_startPinnedShift) {
        // Remembered the first time, like the other changed values.
        bool known = false;
        for (auto& c : g_startChanged) {
            if (c.property == wux::UIElement::RenderTransformProperty() &&
                c.element.get() == list) {
                known = true;
            }
        }
        if (!known) {
            g_startChanged.push_back({winrt::make_weak(list),
                                      wux::UIElement::RenderTransformProperty(),
                                      list.ReadLocalValue(wux::UIElement::RenderTransformProperty())});
        }
        g_startPinnedShift = wuxm::TranslateTransform();
        list.RenderTransform(g_startPinnedShift);
    }

    // Where the grid is now (this includes the current shift), and where its
    // top should be. It's kept within the view, so no row is cut off; a grid
    // taller than the view stays at its top.
    double viewHeight = viewer.ActualHeight();
    double gridHeight = grid.ActualHeight();
    double room = std::max(0.0, viewHeight - gridHeight);
    double top = grid.TransformToVisual(viewer).TransformPoint({0, 0}).Y;
    double wantTop = g_startPosition == 0 ? 0 : g_startPosition == 2 ? room : room / 2;
    wantTop = std::clamp(wantTop + g_startOffsetY, 0.0, room);
    double shift = g_startPinnedShift.Y() + wantTop - top;
    if (std::fabs(shift - g_startPinnedShift.Y()) > 0.5) g_startPinnedShift.Y(shift);
    if (g_startPinnedShift.X() != g_startOffsetX) g_startPinnedShift.X(g_startOffsetX);

    static constexpr PCWSTR kPositions[] = {L"top", L"center", L"bottom"};
    WCHAR text[240];
    swprintf_s(text,
               L"Start menu: pinned grid %.0fx%.0f in a %.0fx%.0f view (%s#%s), at the %s, "
               L"moved %.0f down and %d right",
               grid.ActualWidth(), gridHeight, viewer.ActualWidth(), viewHeight,
               winrt::get_class_name(viewer).c_str(), viewer.Name().c_str(),
               kPositions[g_startPosition], shift, g_startOffsetX);
    if (g_startLayoutLogged != text) {
        g_startLayoutLogged = text;
        Wh_Log(L"%s", text);
    }
}

// Centers the pinned apps, and makes their icons the Icon size setting, each
// centered in a square cell. Their names are hidden (see IsPinnedAppName), so
// a tile is just its icon, and it's centered in its cell both ways. Returns
// whether `el` was one of them, for the log.
bool StartLayOutPinned(wux::FrameworkElement const& el, std::wstring_view type,
                       winrt::hstring const& name) {
    double icon = g_startIconSize;

    // The grid of pinned apps (an ItemsWrapGrid, as in the themes of
    // "Windows 11 Start Menu Styler"): Columns columns, sharing the pinned
    // list's width, centered as a whole.
    if (type == kItemsWrapGrid) {
        auto list = FindAncestor(el, 8, [](wux::FrameworkElement const& a) {
            return a.Name() == L"PinnedList";
        });
        if (!list) return false;
        double width = StartCellWidth(list.ActualWidth());
        double height = StartCellHeight(width);
        StartSetDouble(el, wuxc::ItemsWrapGrid::ItemWidthProperty(), width);
        StartSetDouble(el, wuxc::ItemsWrapGrid::ItemHeightProperty(), height);
        StartSet(el, wuxc::ItemsWrapGrid::MaximumRowsOrColumnsProperty(), (int32_t)g_startColumns,
                 [](int32_t a, int32_t b) { return a == b; });
        StartSetEnum(el, wux::FrameworkElement::HorizontalAlignmentProperty(),
                     wux::HorizontalAlignment::Center);
        g_startPinnedGrid = winrt::make_weak(el);
        LogPinnedChain(el);
        return true;
    }
    // The pinned list as a whole: moved to the middle of the Start menu.
    if (name == L"StartMenuPinnedList") {
        StartCenterPinned(el);
        return true;
    }
    // Each app fills its cell (the cell's size comes from the grid above);
    // its own fixed size and margin would crop it.
    constexpr double kAuto = std::numeric_limits<double>::quiet_NaN();
    if (type == L"Windows.UI.Xaml.Controls.GridViewItem") {
        if (!IsInPinnedList(el, 8)) return false;
        StartSetDouble(el, wux::FrameworkElement::WidthProperty(), kAuto);
        StartSetDouble(el, wux::FrameworkElement::HeightProperty(), kAuto);
        StartSetThickness(el, wux::FrameworkElement::MarginProperty(), {0, 0, 0, 0});
        StartSetEnum(el, wux::FrameworkElement::HorizontalAlignmentProperty(),
                     wux::HorizontalAlignment::Stretch);
        StartSetEnum(el, wux::FrameworkElement::VerticalAlignmentProperty(),
                     wux::VerticalAlignment::Stretch);
        return true;
    }
    // The tile itself: as big as its icon, in the middle of the cell.
    if (type == L"StartMenu.PinnedListTile") {
        StartSetDouble(el, wux::FrameworkElement::WidthProperty(), kAuto);
        StartSetDouble(el, wux::FrameworkElement::HeightProperty(), kAuto);
        StartSetEnum(el, wux::FrameworkElement::HorizontalAlignmentProperty(),
                     wux::HorizontalAlignment::Center);
        StartSetEnum(el, wux::FrameworkElement::VerticalAlignmentProperty(),
                     wux::VerticalAlignment::Center);
        return true;
    }
    auto inTile = [&el](int depth) {
        return HasAncestor(el, depth, [](wux::FrameworkElement const& a) {
            return winrt::get_class_name(a) == L"StartMenu.PinnedListTile";
        });
    };
    // The tile's content: as small as the icon, in the middle of the tile.
    if (name == L"Root" && inTile(1)) {
        if (type == L"Windows.UI.Xaml.Controls.Grid") {
            StartSetThickness(el, wuxc::Grid::PaddingProperty(), {0, 0, 0, 0});
        }
        StartSetEnum(el, wux::FrameworkElement::HorizontalAlignmentProperty(),
                     wux::HorizontalAlignment::Center);
        StartSetEnum(el, wux::FrameworkElement::VerticalAlignmentProperty(),
                     wux::VerticalAlignment::Center);
        return true;
    }
    if ((name == L"LogoContainer" && inTile(2)) || (name == L"Logo" && inTile(3))) {
        StartSetDouble(el, wux::FrameworkElement::WidthProperty(), icon);
        StartSetDouble(el, wux::FrameworkElement::HeightProperty(), icon);
        StartSetDouble(el, wux::FrameworkElement::MinWidthProperty(), 0);
        StartSetDouble(el, wux::FrameworkElement::MinHeightProperty(), 0);
        StartSetDouble(el, wux::FrameworkElement::MaxWidthProperty(), icon);
        StartSetDouble(el, wux::FrameworkElement::MaxHeightProperty(), icon);
        StartSetThickness(el, wux::FrameworkElement::MarginProperty(), {0, 0, 0, 0});
        StartSetEnum(el, wux::FrameworkElement::HorizontalAlignmentProperty(),
                     wux::HorizontalAlignment::Center);
        StartSetEnum(el, wux::FrameworkElement::VerticalAlignmentProperty(),
                     wux::VerticalAlignment::Center);
        return true;
    }
    return false;
}

void StartApplyGlassLook() {
    if (!g_startBackground) return;
    auto border = g_startBackground->border.get();
    if (!border) return;
    ApplyGlassLook(g_startBackground->glass, UseLightGlass(border), L"Start menu",
                   g_startBackground->radius, false);
}

void StartReleaseBackground() {
    if (!g_startBackground) return;
    auto& bg = *g_startBackground;
    bg.themeChanged.revoke();
    ReleaseGlass(bg.glass);
    if (auto border = bg.border.get()) {
        try {
            if (border.Child() == bg.glass.root) border.Child(nullptr);
            RestoreLocalValue(border, wuxc::Border::BackgroundProperty(), bg.originalBackground);
            RestoreLocalValue(border, wuxc::Border::BorderThicknessProperty(),
                              bg.originalBorderThickness);
        } catch (...) {}
    }
    g_startBackground.reset();
}

// Puts the glass in the Start menu's background border (Border#AcrylicBorder,
// where LiquidGlass2 sets its blur), in place of its own background.
void StartApplyBackground(wuxc::Border const& border) {
    if (g_startBackground) {
        auto current = g_startBackground->border.get();
        if (current) return;
        StartReleaseBackground();  // that one is gone; the Start menu made a new one
    }
    // The phone panel has one of its own; it's hidden.
    if (HasAncestor(border, 8, [](wux::FrameworkElement const& a) {
            return winrt::get_class_name(a) == L"StartMenu.StartMenuCompanion";
        })) {
        return;
    }
    if (auto child = border.Child()) {
        if (StartFirstTime(L"AcrylicBorder child")) {
            Wh_Log(L"Start menu: its background border holds a %s; no glass",
                   winrt::get_class_name(child).c_str());
        }
        return;
    }
    auto& bg = g_startBackground.emplace();
    bg.border = winrt::make_weak(border);
    bg.originalBackground = border.ReadLocalValue(wuxc::Border::BackgroundProperty());
    bg.originalBorderThickness = border.ReadLocalValue(wuxc::Border::BorderThicknessProperty());
    float radius = (float)border.CornerRadius().TopLeft;
    bg.radius = radius > 0 ? radius : 8;
    bg.glass = MakeGlass();
    border.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
    border.BorderThickness({0, 0, 0, 0});
    border.Child(bg.glass.root);
    StartApplyGlassLook();
    // Windows switched between light and dark mode (for System).
    bg.themeChanged = border.ActualThemeChanged(
        winrt::auto_revoke, [](wux::FrameworkElement const&, wf::IInspectable const&) {
            try { StartApplyGlassLook(); } catch (...) {}
        });
    Wh_Log(L"Start menu: glass added (%s, corner radius %.0f)",
           g_liquidGlass ? L"Liquid Glass" : L"frosted glass", bg.radius);
}

// Returns whether to look inside `el` (not when it's hidden).
bool StartProcessElement(wux::FrameworkElement const& el) {
    auto name = el.Name();
    auto typeName = winrt::get_class_name(el);
    std::wstring_view type = typeName;
    if (!name.empty()) {
        for (PCWSTR hidden : kStartHiddenNames) {
            if (name == hidden) {
                if (g_startShowSearch && IsStartSearchBox(name)) break;
                StartHide(el, hidden);
                return false;
            }
        }
        if (name == L"AcrylicBorder") {
            if (auto border = el.try_as<wuxc::Border>()) StartApplyBackground(border);
            return false;
        }
        if (IsPinnedAppName(el, name)) {
            StartHide(el, L"pinned app names");
            return false;
        }
    }
    if (IsAllAppsItemsPanel(el, type)) {
        StartHide(el, L"All apps list");
        return false;
    }
    if (StartLayOutPinned(el, type, name)) {
        std::wstring what = L"pinned " + std::wstring(type) + L"#" + std::wstring(name);
        if (StartFirstTime(what)) {
            Wh_Log(L"Start menu: laying out %s (icons %d px, %d columns)", what.c_str(),
                   g_startIconSize, g_startColumns);
        }
    }
    return true;
}

// The Start menu's window, for StartIsShown.
HWND g_startCoreWindow = nullptr;
int g_startTicks = 0;

// Whether the Start menu is open: its window is shown and not cloaked (Start
// stays loaded while closed, hidden from view by DWM).
bool StartIsShown() {
    if (!g_startCoreWindow) return true;
    if (!IsWindowVisible(g_startCoreWindow)) return false;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(g_startCoreWindow, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return false;
    }
    return true;
}

void StartProcessTree();

// Every 250 ms while the Start menu is open, and every 2 s while it's closed
// (its tree stays as it was left, and is kept up to date for when it opens).
void StartOnTimer() {
    g_startTicks++;
    if (!StartIsShown() && g_startTicks % 8 != 0) return;
    if (g_startTicks % 40 == 0) StartPruneHidden();
    StartProcessTree();
}

void StartProcessTree() {
    try {
        auto window = wux::Window::Current();
        auto root = window ? window.Content() : nullptr;
        if (!root) return;
        if (StartFirstTime(L"root")) {
            Wh_Log(L"Start menu: XAML root is a %s", winrt::get_class_name(root).c_str());
        }
        std::vector<wux::DependencyObject> stack{root};
        while (!stack.empty()) {
            auto current = stack.back();
            stack.pop_back();
            bool inside = true;
            if (auto el = current.try_as<wux::FrameworkElement>()) {
                try {
                    inside = StartProcessElement(el);
                } catch (...) {
                    if (StartFirstTime(L"element error")) {
                        Wh_Log(L"Start menu: element error 0x%08X", winrt::to_hresult());
                    }
                }
            }
            if (!inside) continue;
            int count = wuxm::VisualTreeHelper::GetChildrenCount(current);
            for (int i = count - 1; i >= 0; i--) {
                stack.push_back(wuxm::VisualTreeHelper::GetChild(current, i));
            }
        }
    } catch (...) {
        if (StartFirstTime(L"walk error")) {
            Wh_Log(L"Start menu: walking its tree failed: 0x%08X", winrt::to_hresult());
        }
    }
}

// On the Start menu's thread.
HWND FindStartMenuCoreWindow();

void StartInstall() {
    if (g_startTimer || !g_startMenu) return;
    g_startCoreWindow = FindStartMenuCoreWindow();
    g_startTicks = 0;
    try {
        g_startTimer = wux::DispatcherTimer();
        g_startTimer.Interval(std::chrono::milliseconds(250));
        g_startTimer.Tick([](wf::IInspectable const&, wf::IInspectable const&) {
            StartOnTimer();
        });
        g_startTimer.Start();
        Wh_Log(L"Start menu: watching its XAML tree (thread %u)", GetCurrentThreadId());
        StartProcessTree();
    } catch (...) {
        g_startTimer = nullptr;
        Wh_Log(L"Start menu: can't watch its tree: 0x%08X", winrt::to_hresult());
    }
}

// On the Start menu's thread: shows everything again, as it was.
void StartUninstall() {
    if (g_startTimer) {
        try { g_startTimer.Stop(); } catch (...) {}
        g_startTimer = nullptr;
    }
    for (auto& h : g_startHidden) {
        auto el = h.element.get();
        if (!el) continue;
        try {
            el.UnregisterPropertyChangedCallback(wux::UIElement::VisibilityProperty(), h.token);
            RestoreLocalValue(el, wux::UIElement::VisibilityProperty(), h.originalVisibility);
        } catch (...) {}
    }
    g_startHidden.clear();
    g_startHiddenIds.clear();
    for (auto& c : g_startChanged) {
        auto el = c.element.get();
        if (!el) continue;
        try { RestoreLocalValue(el, c.property, c.original); } catch (...) {}
    }
    g_startChanged.clear();
    g_startPinnedGrid = nullptr;
    g_startPinnedShift = nullptr;
    g_startLayoutLogged.clear();
    g_startChainLogged = false;
    StartReleaseBackground();
    g_startLogged.clear();
}

// The Start menu's window (a CoreWindow), if it's there yet.
HWND FindStartMenuCoreWindow() {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM l) -> BOOL {
        DWORD pid;
        WCHAR cls[64];
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassName(hWnd, cls, ARRAYSIZE(cls)) &&
            _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
            *(HWND*)l = hWnd;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&result);
    return result;
}

// The Start menu may make its window after the mod loads: catch it as it's
// made (as in "Compact Start Menu").
void OnStartMenuWindowCreated(HWND hWnd, LPCWSTR className) {
    bool named = ((ULONG_PTR)className & ~(ULONG_PTR)0xffff) != 0;
    if (!named || _wcsicmp(className, L"Windows.UI.Core.CoreWindow") != 0) return;
    RunFromWindowThread(hWnd, [](void*) { StartInstall(); }, nullptr);
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle, LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y,
                                           int nWidth, int nHeight, HWND hWndParent,
                                           HMENU hMenu, HINSTANCE hInstance, PVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                    PVOID lpParam, DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X,
                                            Y, nWidth, nHeight, hWndParent, hMenu, hInstance,
                                            lpParam, dwBand);
    if (hWnd) OnStartMenuWindowCreated(hWnd, lpClassName);
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle, LPCWSTR lpClassName,
                                             LPCWSTR lpWindowName, DWORD dwStyle, int X,
                                             int Y, int nWidth, int nHeight, HWND hWndParent,
                                             HMENU hMenu, HINSTANCE hInstance, PVOID lpParam,
                                             DWORD dwBand, DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                      LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y,
                                      int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                      HINSTANCE hInstance, PVOID lpParam, DWORD dwBand,
                                      DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(dwExStyle, lpClassName, lpWindowName, dwStyle,
                                              X, Y, nWidth, nHeight, hWndParent, hMenu,
                                              hInstance, lpParam, dwBand, dwTypeFlags);
    if (hWnd) OnStartMenuWindowCreated(hWnd, lpClassName);
    return hWnd;
}

// The entry points, for the Start menu's process.
void LoadSettings();

BOOL StartMenuModInit() {
    LoadSettings();
    if (HMODULE user32 = LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        if (void* p = (void*)GetProcAddress(user32, "CreateWindowInBand")) {
            Wh_SetFunctionHook(p, (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }
        if (void* p = (void*)GetProcAddress(user32, "CreateWindowInBandEx")) {
            Wh_SetFunctionHook(p, (void*)CreateWindowInBandEx_Hook,
                               (void**)&CreateWindowInBandEx_Original);
        }
    }
    return TRUE;
}

void StartMenuModAfterInit() {
    HWND w = FindStartMenuCoreWindow();
    Wh_Log(L"Start menu: window %s", w ? L"found" : L"not made yet");
    if (w) RunFromWindowThread(w, [](void*) { StartInstall(); }, nullptr);
}

void StartMenuModBeforeUninit() {
    if (HWND w = FindStartMenuCoreWindow()) {
        RunFromWindowThread(w, [](void*) { StartUninstall(); }, nullptr);
    }
}

void StartMenuModSettingsChanged() {
    HWND w = FindStartMenuCoreWindow();
    bool ran = w && RunFromWindowThread(w, [](void*) {
        StartUninstall();
        LoadSettings();
        StartInstall();
    }, nullptr);
    if (!ran) LoadSettings();
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

void LoadSettings() {
    g_dockSize = std::clamp(Wh_GetIntSetting(L"dock.size"), 0, 100);
    g_liquidGlass = wcscmp(WindhawkUtils::StringSetting::make(L"design"), L"liquid") == 0;
    g_liquidTransparency = std::clamp(Wh_GetIntSetting(L"liquid.transparency"), 0, 100);
    g_liquidBlur = std::clamp(Wh_GetIntSetting(L"liquid.blur"), 0, 30);
    g_liquidRefraction = std::clamp(Wh_GetIntSetting(L"liquid.refraction"), 0, 100);
    g_liquidHighlight = std::clamp(Wh_GetIntSetting(L"liquid.highlight"), 0, 100);
    auto color = WindhawkUtils::StringSetting::make(L"dock.color");
    g_dockTheme = wcscmp(color, L"dark") == 0 ? 1 : wcscmp(color, L"light") == 0 ? 2 : 0;
    g_dockTint = std::clamp(Wh_GetIntSetting(L"dock.tint"), 0, 100);
    g_dockBlur = std::clamp(Wh_GetIntSetting(L"dock.blur"), 0, 100);
    g_dockBottomMargin = std::clamp(Wh_GetIntSetting(L"dock.bottomMargin"), 0, 100);
    g_dockSideMargin = std::clamp(Wh_GetIntSetting(L"dock.sideMargin"), 0, 1000);
    g_dockRoundness = std::clamp(Wh_GetIntSetting(L"dock.roundness"), 0, 50);
    g_shadowOpacity = std::clamp(Wh_GetIntSetting(L"shadow.opacity"), 0, 100);
    g_shadowBlur = std::clamp(Wh_GetIntSetting(L"shadow.blur"), 0, 100);
    g_shadowOffset = std::clamp(Wh_GetIntSetting(L"shadow.offset"), 0, 50);
    // Amount 0-100 -> the icon grows to 1.0x-2.0x.
    g_scale = 1.0f + std::clamp(Wh_GetIntSetting(L"magnification.amount"), 0, 100) / 100.0f;
    g_magnificationRange = std::clamp(Wh_GetIntSetting(L"magnification.range"), 0, 100);
    g_animationMs = std::clamp(Wh_GetIntSetting(L"magnification.smoothing"), 0, 1000);
    g_magnifyOverWindows = Wh_GetIntSetting(L"magnification.overWindows");
    g_autoHide = Wh_GetIntSetting(L"autoHide.enabled");
    g_appsDivider = Wh_GetIntSetting(L"items.appsDivider");
    g_showTrash = Wh_GetIntSetting(L"items.recycleBin");
    g_trayGlass = Wh_GetIntSetting(L"items.trayGlass");
    g_startMenu = Wh_GetIntSetting(L"startMenu.enabled");
    g_startIconSize = std::clamp(Wh_GetIntSetting(L"startMenu.iconSize"), 16, 128);
    g_startColumns = std::clamp(Wh_GetIntSetting(L"startMenu.columns"), 1, 12);
    auto position = WindhawkUtils::StringSetting::make(L"startMenu.position");
    g_startPosition = wcscmp(position, L"top") == 0 ? 0 : wcscmp(position, L"bottom") == 0 ? 2 : 1;
    g_startOffsetX = std::clamp(Wh_GetIntSetting(L"startMenu.offsetX"), -2000, 2000);
    g_startOffsetY = std::clamp(Wh_GetIntSetting(L"startMenu.offsetY"), -2000, 2000);
    g_startShowSearch = Wh_GetIntSetting(L"startMenu.showSearch");

    ComputeSizes();
    Wh_Log(L"Sizes: icon %.0f (loaded at %.0f), dock %.0f, taskbar %d, button margin %.1f",
           g_iconSize, g_iconLoadSize, g_dockHeight, g_taskbarHeight, g_buttonMargin);
    Wh_Log(L"Dock: bottom %.0f, side %d, radius %.0f, shadow %d%% blur %d offset %d",
           g_dockBottomGap, g_dockSideMargin, g_dockRadius, g_shadowOpacity, g_shadowBlur,
           g_shadowOffset);
    Wh_Log(L"Design: %s (Liquid Glass: transparency %d, blur %d, refraction %d, highlight %d)",
           g_liquidGlass ? L"Liquid Glass" : L"frosted glass", g_liquidTransparency, g_liquidBlur,
           g_liquidRefraction, g_liquidHighlight);
}

BOOL Wh_ModInit() {
    g_isStartMenuProcess = IsStartMenuProcess();
    if (g_isStartMenuProcess) return StartMenuModInit();
    LoadSettings();
    g_managingAutoHide = Wh_GetIntValue(UserValueName(L"managingAutoHide").c_str(), 0);
    g_autoHideBefore = Wh_GetIntValue(UserValueName(L"autoHideBefore").c_str(), 0);
    g_shellJobWork = CreateThreadpoolWork(RunShellJob, nullptr, nullptr);

    if (!HookTaskbarDllSymbols()) Wh_Log(L"taskbar.dll hooks failed; height won't change");
    HookTaskbarModules();

    if (HMODULE kb = GetModuleHandle(L"kernelbase.dll")) {
        if (auto p = GetProcAddress(kb, "LoadLibraryExW")) {
            WindhawkUtils::SetFunctionHook((LoadLibraryExW_t)p, LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
        }
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isStartMenuProcess) {
        StartMenuModAfterInit();
        return;
    }
    LogTaskbarWindows();
    // Besides the height, this makes the taskbar update all its buttons, which
    // runs the hooks that find them (and the taskbars they're in).
    if (g_taskbarHeight > 0) ApplyTaskbarHeight();
    ApplyMagnifyOverWindows();
}

// Runs before the hooks are removed, which the taskbar needs to go back to its
// normal height and icon size.
void Wh_ModBeforeUninit() {
    if (g_isStartMenuProcess) {
        StartMenuModBeforeUninit();
        return;
    }
    // The hooks stay in place until after this; stop them from applying the
    // dock's changes again.
    g_unloading = true;

    // Undo the changes to the taskbar's elements first, so that when it goes
    // back to its normal size below, it sizes its icons itself, with nothing
    // of ours left to override that.
    if (HWND tb = FindTaskbar()) {
        RunFromWindowThread(tb, [](void*) { ResetAll(); }, nullptr);
    }
    ApplyAutoHide();  // back off, if the mod turned it on
    UnregisterClass(kWindowClass, ModuleInstance());
    // Shell jobs first: they end by checking the Recycle Bin.
    for (PTP_WORK* work : {&g_shellJobWork, &g_trashWork}) {
        if (!*work) continue;
        WaitForThreadpoolWorkCallbacks(*work, FALSE);
        CloseThreadpoolWork(*work);
        *work = nullptr;
    }

    if (g_taskbarHeight > 0) {
        g_taskbarHeight = 0;
        g_iconLoadSize = 0;
        ApplyTaskbarHeight();
    }
}

void Wh_ModSettingsChanged() {
    if (g_isStartMenuProcess) {
        StartMenuModSettingsChanged();
        return;
    }
    LogTaskbarWindows();
    int oldTaskbarHeight = g_taskbarHeight;
    float oldIconLoadSize = g_iconLoadSize;
    // The frame loop and the hooks read the settings on the taskbar thread, so
    // they're loaded there too. Scale settings are read every frame; the
    // plates (with the Recycle Bin) are made again on the next frame, and the
    // spacing and icon sizes need a refresh.
    HWND tb = FindTaskbar();
    bool ran = tb && RunFromWindowThread(tb, [](void*) {
            LoadSettings();
            try { RemovePlates(); } catch (...) {}
            try { StopTrash(); } catch (...) {}
            try { RestoreTrayRows(); } catch (...) {}  // found again on the next frames
            for (auto& b : *g_buttons) {
                if (auto el = b.element.get()) {
                    try {
                        ApplySpacing(el);
                        ApplyButtonWidth(b, el);
                        ApplyIconColumn(b);
                    } catch (...) {}
                }
            }
            for (auto& i : *g_iconImages) {
                if (auto el = i.element.get()) {
                    try { ApplyIconSize(el); } catch (...) {}
                }
            }
            g_row.clear();  // rebuilt with the new spacing on next hover
            g_rowParent = nullptr;
            MarkPlatesDirty();
        }, nullptr);
    if (!ran) LoadSettings();
    if (g_taskbarHeight != oldTaskbarHeight || g_iconLoadSize != oldIconLoadSize) {
        ApplyTaskbarHeight();
    }
    ApplyMagnifyOverWindows();  // the dock's height may have changed too
}
