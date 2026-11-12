// ==WindhawkMod==
// @id              taskbar-margin
// @name            Taskbar Margin
// @name:zh-CN      任务栏边距
// @name:ja-JP      タスクバーの余白
// @description     Adds a configurable margin to the taskbar content
// @description:zh-CN 为任务栏内容添加可调节的边距
// @description:ja-JP タスクバーのコンテンツに、幅を設定できる余白を追加します
// @version         1.1
// @author          loliri
// @github          https://github.com/loliri
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The taskbar XAML access is based on the Start button always on the left mod
// by m417z, which is also licensed under the GNU General Public License v3.0:
// https://github.com/m417z/my-windhawk-mods
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/loliri/windhawk-taskbar-margin/issues

// ==WindhawkModReadme==
/*
# Taskbar Margin

Shifts the taskbar **content** by a configurable number of pixels, leaving an empty margin on the left or on the right of the taskbar. The taskbar background stays full width, and with a left margin, the taskbar context menu follows the content.

Only the taskbar itself is affected.

![Taskbar without a margin](https://raw.githubusercontent.com/loliri/windhawk-taskbar-margin/main/images/before.png) \
_Before_

![Taskbar with a left margin](https://raw.githubusercontent.com/loliri/windhawk-taskbar-margin/main/images/after.png) \
_After_

![Taskbar with a right margin](https://raw.githubusercontent.com/loliri/windhawk-taskbar-margin/main/images/right-placement.png) \
_Right placement, with the FluentFlyout widget in the reserved space_

![Taskbar context menu](https://raw.githubusercontent.com/loliri/windhawk-taskbar-margin/main/images/jumplist.png) \
_The context menu follows the margin_

## Settings

| Setting               | Default      | Description                                                                                                                                                                                                                                                   |
| --------------------- | ------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Placement             | Left         | Which side of the taskbar content the margin is placed on. The right side leaves the space between the app list and the system tray.                                                                                                                          |
| Left margin (pixels)  | 220          | How much empty space to leave on the left of the taskbar content.                                                                                                                                                                                             |
| Right margin (pixels) | 350          | How much empty space to leave between the app list and the system tray.                                                                                                                                                                                       |
| Follow display DPI    | on           | Scale the margin with the display DPI, so it keeps the same visual size on high-DPI displays. Turn off to keep it at a constant physical pixel size.                                                                                                          |
| Displays              | All          | Which displays the margin applies to: all, only the primary one, only the secondary ones, or only a specific one. Each display has its own taskbar and its own DPI, so the margin is calculated per display.                                                  |
| Specific display      | `\\.\DISPLAY1` | The device name of the display the margin applies to when Displays is set to "Specific display only". The comparison is case-insensitive, and an empty value matches no display. The device name of the display each taskbar is on is written to the mod log. |

## Notes

Requires Windows 11.

The taskbar must be:

- **left-aligned** (Settings → Personalization → Taskbar → Taskbar icon alignment)
- **Bottom** or **Top** (New in Windows 11 26H2, Settings → Personalization → Taskbar → Taskbar position)

The taskbar is not mirrored correctly on right-to-left display languages: the margin is applied to the chosen physical side regardless of the taskbar's flow direction, while the context menu is moved to the right (with the left placement).

## Compatibility

- **TranslucentTB** is confirmed compatible and can be used alongside this mod.
- **Windows 11 Taskbar Styler** is not an alternative to this mod for the left placement. It changes the taskbar itself, while this mod changes the position of the elements inside it. A taskbar restyled that way loses the taskbar's effects on the margin area, while this mod keeps them across the whole taskbar, margin included. It also cannot move the context menu (the jump list), which this mod does. For the right placement, a single Styler rule can grow the app list margin in the same way; what this mod adds there is the per-display scope and the option not to follow the display DPI.
- **Taskbar Styler compatibility**: with themes that customize the taskbar button metrics (e.g. Lucent, or the MacDock themes in ramensoftware/windhawk-mods#6988), Explorer can restart-loop with the right placement on Windows 11 26H2 build 26300 - on secondary taskbars, and on the primary taskbar as well if the Small icon size is set above its default in the Taskbar height and icon size mod. Setting MediumTaskbarButtonExtent=55 does not prevent this. The left placement is not affected.
- **Taskbar jump list on cursor pos** changes the same anchor point. Both mods offset it, so depending on the hook order the menu can open offset from the cursor. Use one or the other.
- Tested on Windows 11 26H2.

## Suggested use

The mod is meant for widgets that are drawn on top of the taskbar, such as the taskbar widget of [FluentFlyout](https://github.com/unchihugo/FluentFlyout) or the taskbar lyrics of [Lyricify](https://github.com/WXRIW/Lyricify-App) (Lyricify Fusion). Without reserved space, they overlap the taskbar buttons. Reserve the space on the side where the widget is shown, and set the margin to the widget's width:

- **Widget at the left end:** use the left placement. The whole taskbar content shifts to the right, so the taskbar elements tile linearly instead of overlapping each other. With FluentFlyout, set the widget's position to the bottom left corner and turn on the fixed widget width.
- **Widget at the right end, next to the system tray:** use the right placement. The space is left between the app list and the system tray, and the tray itself does not move. For example, this suits FluentFlyout in its usual place at the bottom right corner.

## Implementation notes

The taskbar context menu (the jump list) is not laid out by XAML. Its anchor point is computed in `explorer.exe` by `CTaskListWnd::_ComputeJumpViewPosition` in `taskbar.dll`, and handed to the process that draws the menu. The point is in physical screen pixels.

Shifting the taskbar's XAML content therefore does not move the menu on its own, because the menu is not placed relative to it. The mod adjusts the anchor point instead, which is why it hooks that function.

## License

GPL-3.0. The taskbar XAML access is based on the [Start button always on the left](https://github.com/m417z/my-windhawk-mods) mod by m417z, which is also licensed under GPL-3.0.

## Feedback

Bug reports and feature requests are welcome in [Issues](https://github.com/loliri/windhawk-taskbar-margin/issues).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- placement: left
  $name: Placement
  $name:zh-CN: 位置
  $name:ja-JP: 配置
  $description: >-
    Which side of the taskbar content the margin is placed on. The right side
    leaves the space between the app list and the system tray.
  $description:zh-CN: >-
    边距位于任务栏内容的哪一侧。右侧时，留空位于应用列表与系统托盘之间。
  $description:ja-JP: >-
    タスクバーのコンテンツのどちら側に余白を置くか。右側では、アプリ一覧とシステムトレイの間に余白が空きます。
  $options:
    - left: Left
    - right: Right
  $options:zh-CN:
    - left: 左侧
    - right: 右侧
  $options:ja-JP:
    - left: 左側
    - right: 右側
- leftMargin: 220
  $name: Left margin (pixels)
  $name:zh-CN: 左边距（像素）
  $name:ja-JP: 左側の余白（ピクセル）
  $description: >-
    How many pixels of empty space to leave on the left of the taskbar content.
    A larger value shifts the content further right. Only used when the
    placement is set to the left.
  $description:zh-CN: >-
    任务栏内容左侧留空的像素数，数值越大整体越靠右。仅在位置设置为左侧时生效。
  $description:ja-JP: >-
    タスクバーのコンテンツの左側に空ける余白のピクセル数。大きいほどコンテンツが右へずれます。配置が左側のときだけ使われます。
- rightMargin: 350
  $name: Right margin (pixels)
  $name:zh-CN: 右边距（像素）
  $name:ja-JP: 右側の余白（ピクセル）
  $description: >-
    How many pixels of empty space to leave between the app list and the
    system tray. A larger value ends the app list further left. Only used
    when the placement is set to the right.
  $description:zh-CN: >-
    应用列表与系统托盘之间留空的像素数，数值越大应用列表结束得越靠左。仅在位置设置为右侧时生效。
  $description:ja-JP: >-
    アプリ一覧とシステムトレイの間に空ける余白のピクセル数。大きいほどアプリ一覧が左側で終わります。配置が右側のときだけ使われます。
- followDpi: true
  $name: Follow display DPI
  $name:zh-CN: 跟随显示器 DPI
  $name:ja-JP: ディスプレイの DPI に合わせる
  $description: >-
    Scale the margin together with the display DPI, so it keeps the same visual
    size on high-DPI displays. Turn off to keep it at a constant physical pixel
    size.
  $description:zh-CN: >-
    让边距随显示器 DPI 一同缩放，在高 DPI 显示器上保持相同的视觉大小。关闭后边距将固定为恒定的物理像素数。
  $description:ja-JP: >-
    余白をディスプレイの DPI に合わせて拡大縮小し、高 DPI ディスプレイでも見た目の大きさが同じになるようにします。オフにすると物理ピクセルで一定の大きさになります。
- displays: all
  $name: Displays
  $name:zh-CN: 生效显示器
  $name:ja-JP: 対象ディスプレイ
  $description: >-
    Which displays the margin is applied to. Each display keeps its own taskbar
    and its own DPI, so the margin is calculated per display.
  $description:zh-CN: >-
    边距应用在哪些显示器上。每个显示器有自己的任务栏和 DPI，边距按显示器分别计算。
  $description:ja-JP: >-
    余白を適用するディスプレイ。各ディスプレイには独自のタスクバーと DPI があり、余白はディスプレイごとに計算されます。
  $options:
    - all: All displays
    - primary: Primary display only
    - secondary: Secondary displays only
    - specific: Specific display only
  $options:zh-CN:
    - all: 所有显示器
    - primary: 仅主显示器
    - secondary: 仅副显示器
    - specific: 仅指定显示器
  $options:ja-JP:
    - all: すべてのディスプレイ
    - primary: プライマリーディスプレイのみ
    - secondary: セカンダリーディスプレイのみ
    - specific: 指定したディスプレイのみ
- specificDisplay: '\\.\DISPLAY1'
  $name: Specific display
  $name:zh-CN: 指定显示器
  $name:ja-JP: 指定するディスプレイ
  $description: >-
    The device name of the display the margin is applied to when Displays is
    set to "Specific display only", e.g. \\.\DISPLAY1. The comparison is
    case-insensitive, and an empty value matches no display. The device name
    of the display each taskbar is on is written to the mod log. Display
    numbers can change when cables or connections
    change.
  $description:zh-CN: >-
    当“生效显示器”设置为“仅指定显示器”时，边距所应用显示器的设备名，例如 \\.\DISPLAY1。比较不区分大小写，空值不匹配任何显示器。每个任务栏所在显示器的设备名都会写入 Mod 日志。显示器编号可能会随线缆或连接的变化而变化。
  $description:ja-JP: >-
    「対象ディスプレイ」が「指定したディスプレイのみ」のとき、余白を適用するディスプレイのデバイス名（例: \\.\DISPLAY1）。大文字小文字は区別しません。空欄の場合はどのディスプレイにも一致しません。各タスクバーのディスプレイのデバイス名は Mod のログに出力されます。DISPLAY の番号はケーブル接続先の変更などで変わることがあります。
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <shellscalingapi.h>

#include <windhawk_utils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

enum class Displays
{
    All,
    Primary,
    Secondary,
    Specific,
};

enum class Placement
{
    Left,
    Right,
};

struct
{
    Placement placement;
    int leftMargin;
    int rightMargin;
    bool followDpi;
    Displays displays;
    std::wstring specificDisplay;
} g_settings;

// Guards g_settings.specificDisplay: LoadSettings reassigns the string on
// the Windhawk engine thread while taskbar threads read it in
// ShouldApplyToMonitor.
std::mutex g_settingsMutex;

std::atomic<bool> g_unloading;

// Which property the mod set on an element. The property itself is a WinRT
// object, and keeping it in a global container would leave a strong reference
// to release at process shutdown, where the XAML objects may already be gone.
// The tag is resolved back to the static property when it is cleared.
enum class AppliedProperty
{
    GridPadding,
    FrameworkElementMargin,
};

// Elements whose value the mod set, so it can be cleared again. The app
// list element used by the right placement is the exception: the value the
// mod sets is computed from the element's original margin rather than
// being absolute, so its record also keeps the value the mod set and
// whether the replaced value was a local value. The remove pass restores
// the replaced value only if the element still holds the mod's value,
// and clears the property instead of writing the saved value back when
// there was no local value, so that a value which came from the template
// or a style is not pinned as a local value.
struct AppliedElement
{
    winrt::weak_ref<FrameworkElement> element;
    AppliedProperty property;
    bool hasOriginalMargin = false;
    Thickness originalMargin{};
    Thickness appliedMargin{};
    bool hadLocalMargin = false;
};

// The elements the mod set on one taskbar, together with the thread that
// applied them and the display that taskbar is on. The thread id keeps the
// apply/remove pass scoped to its own taskbar, and the display lets the jump
// list hook look up whether this taskbar actually has the margin.
struct AppliedTaskbar
{
    DWORD threadId;
    HMONITOR monitor;
    std::vector<AppliedElement> elements;
};

// The taskbars the mod has changed. Guarded in case the jump list hook runs on
// a different thread than the apply pass.
std::mutex g_appliedTaskbarsMutex;
std::vector<AppliedTaskbar> g_appliedTaskbars;

void *CTaskBand_ITaskListWndSite_vftable;

void *CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis, void **result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void *TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void *(WINAPI *)(void *pThis,
                                                             void **result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI *)(void *pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

// The taskbar's XAML tree is reached through its host object rather than
// through XAML diagnostics, so that the mod can run alongside other mods which
// need to be Explorer's XAML diagnostics consumer.
XamlRoot XamlRootFromTaskbarHostSharedPtr(void *taskbarHostSharedPtr[2])
{
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1])
    {
        return nullptr;
    }

    // The element is only reachable through the host object, so a host that
    // exists without one yet has to be treated as not ready.
    if (!taskbarHostSharedPtr[0])
    {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE *b = (const BYTE *)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F)
        {
            taskbarElementIUnknownOffset = b[7];
        }
        else
        {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD *p = (const DWORD *)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00)
        {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        }
        else
        {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto *taskbarElementIUnknown =
        *(IUnknown **)((BYTE *)taskbarHostSharedPtr[0] +
                       taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd)
{
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd)
    {
        return nullptr;
    }

    void *taskBand = (void *)GetWindowLongPtr(hTaskSwWnd, 0);
    void *taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void **)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++)
    {
        if (i == 20)
        {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void **)taskBandForTaskListWndSite + 1;
    }

    void *taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd)
{
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd)
    {
        return nullptr;
    }

    void *taskBand = (void *)GetWindowLongPtr(hTaskSwWnd, 0);
    void *taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void **)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++)
    {
        if (i == 20)
        {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void **)taskBandForTaskListWndSite + 1;
    }

    void *taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback)
{
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++)
    {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child)
        {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child))
        {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name)
{
    return EnumChildElements(element, [name](FrameworkElement child)
                             { return child.Name() == name; });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className)
{
    return EnumChildElements(element, [className](FrameworkElement child)
                             { return winrt::get_class_name(child) == className; });
}

// Whether the margin should be applied to the display the taskbar is on. The
// primary display is the one whose top-left corner is the origin of the
// virtual screen.
bool ShouldApplyToMonitor(HMONITOR monitor)
{
    switch (g_settings.displays)
    {
    case Displays::All:
        return true;

    case Displays::Primary:
    {
        MONITORINFO monitorInfo{.cbSize = sizeof(MONITORINFO)};
        if (!GetMonitorInfo(monitor, &monitorInfo))
        {
            return false;
        }

        return (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
    }

    case Displays::Secondary:
    {
        MONITORINFO monitorInfo{.cbSize = sizeof(MONITORINFO)};
        if (!GetMonitorInfo(monitor, &monitorInfo))
        {
            return false;
        }

        return (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) == 0;
    }

    case Displays::Specific:
    {
        // Copied under the lock: the string is reassigned by LoadSettings
        // on the engine thread while this runs on a taskbar thread.
        std::wstring specificDisplay;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            specificDisplay = g_settings.specificDisplay;
        }

        // An empty setting matches no display, so that the margin is never
        // applied to an unintended display before a name is set.
        if (specificDisplay.empty())
        {
            return false;
        }

        MONITORINFOEX monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (!GetMonitorInfo(monitor, &monitorInfo))
        {
            return false;
        }

        return _wcsicmp(monitorInfo.szDevice, specificDisplay.c_str()) == 0;
    }
    }

    return true;
}

// A margin in DIPs, which is what XAML expects. With DPI following on the
// setting is taken as DIPs; with it off the setting is taken as physical pixels
// and converted, so the margin keeps a constant physical size.
double GetMarginInDips(FrameworkElement const &element, int marginPixels)
{
    if (g_settings.followDpi)
    {
        return static_cast<double>(marginPixels);
    }

    try
    {
        auto xamlRoot = element.XamlRoot();
        if (xamlRoot)
        {
            double scale = xamlRoot.RasterizationScale();
            if (scale > 0)
            {
                return static_cast<double>(marginPixels) / scale;
            }
        }
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }

    return static_cast<double>(marginPixels);
}

// Returns whether the margin was applied to this taskbar.
bool ApplyMarginToTaskbar(HWND hTaskbarWnd, XamlRoot xamlRoot)
{
    HMONITOR monitor =
        MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST);

    // Logged before the scope check, so that the device name for the
    // "Specific display only" scope can be read off the log even when no
    // display matches the configured name.
    MONITORINFOEX monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (GetMonitorInfo(monitor, &monitorInfo))
    {
        Wh_Log(L"Taskbar display: %s", monitorInfo.szDevice);
    }

    if (!ShouldApplyToMonitor(monitor))
    {
        return true;
    }

    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    if (!content)
    {
        Wh_Log(L"Failed to get the taskbar content element");
        return false;
    }

    auto taskbarFrame = FindChildByClassName(content, L"Taskbar.TaskbarFrame");
    auto rootGrid = taskbarFrame ? FindChildByName(taskbarFrame, L"RootGrid")
                                 : nullptr;
    if (!rootGrid)
    {
        Wh_Log(L"Failed to find the taskbar RootGrid");
        return false;
    }

    auto grid = rootGrid.try_as<Controls::Grid>();
    if (!grid)
    {
        Wh_Log(L"RootGrid is not a Grid, skipping");
        return false;
    }

    bool placeLeft = g_settings.placement == Placement::Left;

    double margin = GetMarginInDips(
        taskbarFrame, placeLeft ? g_settings.leftMargin
                                : g_settings.rightMargin);

    AppliedTaskbar appliedTaskbar;
    appliedTaskbar.threadId = GetCurrentThreadId();
    appliedTaskbar.monitor = monitor;

    if (placeLeft)
    {
        // The padding shifts the taskbar content, and the background is
        // pulled back by the same amount so that it keeps spanning the
        // full width.
        grid.Padding(Thickness{margin, 0, 0, 0});
        appliedTaskbar.elements.push_back(
            {winrt::make_weak(rootGrid), AppliedProperty::GridPadding});

        auto taskbarBackground =
            FindChildByClassName(rootGrid, L"Taskbar.TaskbarBackground");
        if (taskbarBackground)
        {
            taskbarBackground.Margin(Thickness{-margin, 0, 0, 0});
            appliedTaskbar.elements.push_back(
                {winrt::make_weak(taskbarBackground),
                 AppliedProperty::FrameworkElementMargin});
        }
        else
        {
            Wh_Log(L"Failed to find TaskbarBackground");
        }
    }
    else
    {
        // The right placement reserves the space inside the app list
        // instead of shifting the whole content: the app list's own right
        // margin grows, so the list ends earlier and the space opens
        // between the list and the system tray, which itself does not
        // move. Padding the RootGrid on the right is deliberately not
        // used for this: it changes the layout constraints of the whole
        // taskbar, which was observed to send Explorer into a restart
        // loop on a restyled taskbar (Taskbar Styler with a theme
        // applied), while the app list margin only affects the list
        // itself.
        //
        // The target is computed from the element's current margin. The
        // remove pass restores the right component before every apply,
        // so the reservation does not accumulate. The one exception is a
        // right component that another mod changed in the meantime: the
        // remove pass has to leave it alone, and the width it already
        // holds is then counted as part of the original here.
        // The app list is a direct child of the RootGrid.
        auto appList = FindChildByName(rootGrid, L"TaskbarFrameRepeater");
        if (!appList)
        {
            Wh_Log(L"Failed to find the app list repeater");
            return false;
        }

        Thickness originalMargin = appList.Margin();
        Thickness targetMargin = originalMargin;
        targetMargin.Right += margin;

        AppliedElement applied;
        applied.element = winrt::make_weak(appList);
        applied.property = AppliedProperty::FrameworkElementMargin;
        applied.hasOriginalMargin = true;
        applied.originalMargin = originalMargin;
        applied.appliedMargin = targetMargin;
        applied.hadLocalMargin =
            appList.ReadLocalValue(FrameworkElement::MarginProperty()) !=
            DependencyProperty::UnsetValue();

        if (targetMargin.Right != originalMargin.Right)
        {
            appList.Margin(targetMargin);
        }

        appliedTaskbar.elements.push_back(std::move(applied));
    }

    std::lock_guard<std::mutex> lock(g_appliedTaskbarsMutex);
    g_appliedTaskbars.push_back(std::move(appliedTaskbar));

    return true;
}

// Clears the values the mod set on the taskbars of the calling thread.
void RemoveAppliedMargins()
{
    DWORD dwThreadId = GetCurrentThreadId();

    std::vector<AppliedTaskbar> appliedTaskbars;

    {
        std::lock_guard<std::mutex> lock(g_appliedTaskbarsMutex);

        for (auto it = g_appliedTaskbars.begin();
             it != g_appliedTaskbars.end();)
        {
            if (it->threadId == dwThreadId)
            {
                appliedTaskbars.push_back(std::move(*it));
                it = g_appliedTaskbars.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    for (const auto &appliedTaskbar : appliedTaskbars)
    {
        for (const auto &applied : appliedTaskbar.elements)
        {
            if (auto element = applied.element.get())
            {
                try
                {
                    if (applied.hasOriginalMargin)
                    {
                        // Undo only the right component, the only one
                        // this mod changed: another mod may have changed
                        // the other components of the same margin in the
                        // meantime (taskbar-system-info, for example,
                        // sets the left component of this element's
                        // margin), and those are not this mod's to
                        // restore. The component is restored only if it
                        // still holds the value this mod set.
                        auto currentMargin = element.Margin();
                        if (currentMargin.Right ==
                            applied.appliedMargin.Right)
                        {
                            if (!applied.hadLocalMargin &&
                                currentMargin == applied.appliedMargin)
                            {
                                // The replaced value was not a local
                                // value and nothing else changed: clear
                                // the property instead of writing the
                                // saved value back, so that a value from
                                // the template or a style is not pinned
                                // as a local value.
                                element.ClearValue(
                                    FrameworkElement::MarginProperty());
                            }
                            else
                            {
                                currentMargin.Right =
                                    applied.originalMargin.Right;
                                element.Margin(currentMargin);
                            }
                        }
                    }
                    else
                    {
                        element.ClearValue(
                            applied.property == AppliedProperty::GridPadding
                                ? Controls::Grid::PaddingProperty()
                                : FrameworkElement::MarginProperty());
                    }
                }
                catch (winrt::hresult_error const &ex)
                {
                    Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
                }
            }
        }
    }
}

// Whether the margin is currently applied to the taskbar on this display. The
// jump list hook uses this instead of a single global flag, so that a taskbar
// which does not have the margin never gets its anchor shifted.
bool IsMarginAppliedToMonitor(HMONITOR monitor)
{
    std::lock_guard<std::mutex> lock(g_appliedTaskbarsMutex);
    for (const auto &appliedTaskbar : g_appliedTaskbars)
    {
        if (appliedTaskbar.monitor == monitor)
        {
            return true;
        }
    }

    return false;
}

using ComputeJumpViewPosition_t = HRESULT(WINAPI *)(
    void *pThis,
    void *pTaskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point *point,
    winrt::Windows::UI::Xaml::HorizontalAlignment *hAlign,
    winrt::Windows::UI::Xaml::VerticalAlignment *vAlign);

ComputeJumpViewPosition_t ComputeJumpViewPosition_Original;

HRESULT WINAPI ComputeJumpViewPosition_Hook(
    void *pThis,
    void *pTaskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point *point,
    winrt::Windows::UI::Xaml::HorizontalAlignment *hAlign,
    winrt::Windows::UI::Xaml::VerticalAlignment *vAlign)
{
    HRESULT hr = ComputeJumpViewPosition_Original(pThis, pTaskBtnGroup, param2,
                                                  point, hAlign, vAlign);

    // Only the left placement moves the task buttons: with the margin on the
    // right, the buttons of a left-aligned taskbar keep their positions, so
    // the anchor needs no adjustment.
    if (FAILED(hr) || !point || g_unloading ||
        g_settings.placement != Placement::Left || !g_settings.leftMargin)
    {
        return hr;
    }

    // The anchor is in physical screen pixels. The DPI is taken from the
    // display the anchor is on, so that displays at different scales each get
    // the right offset.
    HMONITOR monitor =
        MonitorFromPoint(POINT{(LONG)point->X, (LONG)point->Y},
                         MONITOR_DEFAULTTONEAREST);
    if (!IsMarginAppliedToMonitor(monitor))
    {
        return hr;
    }

    int offset = g_settings.leftMargin;
    if (g_settings.followDpi)
    {
        UINT dpiX = 0;
        UINT dpiY = 0;
        if (SUCCEEDED(
                GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
            dpiX > 0)
        {
            offset = MulDiv(g_settings.leftMargin, dpiX,
                            USER_DEFAULT_SCREEN_DPI);
        }
    }

    Wh_Log(L"Jump list anchor x: %d -> %d", (int)point->X,
           (int)(point->X + offset));
    point->X += static_cast<float>(offset);

    return hr;
}

using RunFromWindowThreadProc_t = void(WINAPI *)(void *parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void *procParam)
{
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM
    {
        RunFromWindowThreadProc_t proc;
        void *procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0)
    {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId())
    {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT
        {
            if (nCode == HC_ACTION)
            {
                const CWPSTRUCT *cwp = (const CWPSTRUCT *)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg)
                {
                    RUN_FROM_WINDOW_THREAD_PARAM *param =
                        (RUN_FROM_WINDOW_THREAD_PARAM *)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook)
    {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

bool ApplySettingsFromTaskbarThread();

// The taskbar's XAML tree is not ready when the taskbar window is created, so
// the XamlRoot lookup fails at that point. Applying once and giving up is what
// breaks a late start, where the taskbar is built after the mod loads, so the
// apply is retried on a timer until every taskbar on the thread has it. The
// timer is per thread, so that each taskbar thread keeps its own retry state.
thread_local winrt::Windows::System::DispatcherQueueTimer g_retryTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    g_retryTimerRevoker;
thread_local int g_retryAttempts;

constexpr int kMaxApplyAttempts = 20;
constexpr int kApplyRetryIntervalMs = 500;

void StopRetryTimer()
{
    if (g_retryTimer)
    {
        try
        {
            g_retryTimer.Stop();
        }
        catch (winrt::hresult_error const &ex)
        {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    g_retryTimerRevoker.revoke();
    g_retryTimer = nullptr;
    g_retryAttempts = 0;
}

void RetryTimerTick(winrt::Windows::System::DispatcherQueueTimer const &,
                    winrt::Windows::Foundation::IInspectable const &)
{
    if (g_unloading)
    {
        StopRetryTimer();
        return;
    }

    if (ApplySettingsFromTaskbarThread())
    {
        Wh_Log(L"Applied on attempt %d", g_retryAttempts + 1);
        StopRetryTimer();
        return;
    }

    if (++g_retryAttempts >= kMaxApplyAttempts)
    {
        Wh_Log(L"Gave up applying the margin");
        StopRetryTimer();
    }
}

void StartRetryTimer()
{
    if (g_retryTimer)
    {
        return;
    }

    try
    {
        auto dispatcherQueue =
            winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (!dispatcherQueue)
        {
            Wh_Log(L"No dispatcher queue, cannot retry");
            return;
        }

        g_retryTimer = dispatcherQueue.CreateTimer();
        g_retryTimer.IsRepeating(true);
        g_retryTimer.Interval(
            std::chrono::milliseconds{kApplyRetryIntervalMs});
        g_retryTimerRevoker = g_retryTimer.Tick(winrt::auto_revoke,
                                                RetryTimerTick);
        g_retryTimer.Start();
    }
    catch (winrt::hresult_error const &ex)
    {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

// Applies the margin to every taskbar on the calling thread. Returns whether
// all of them got it, so that a taskbar whose XAML is not built yet keeps the
// retry going instead of being skipped because another one succeeded.
bool ApplySettingsFromTaskbarThread()
{
    Wh_Log(L">");

    RemoveAppliedMargins();

    if (g_unloading)
    {
        return true;
    }

    bool allApplied = true;

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
            {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0)
            {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            }
            else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0)
            {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            }
            else
            {
                return TRUE;
            }

            if (!xamlRoot)
            {
                Wh_Log(L"Getting XamlRoot failed");
                *reinterpret_cast<bool *>(lParam) = false;
                return TRUE;
            }

            if (!ApplyMarginToTaskbar(hWnd, xamlRoot))
            {
                *reinterpret_cast<bool *>(lParam) = false;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&allApplied));

    return allApplied;
}

void ApplySettingsOnTaskbarThread()
{
    if (!ApplySettingsFromTaskbarThread() && !g_unloading)
    {
        Wh_Log(L"Taskbar XAML not ready, will retry");
        StartRetryTimer();
    }
}

void ApplySettings(HWND hTaskbarWnd)
{
    RunFromWindowThread(
        hTaskbarWnd, [](void *)
        { ApplySettingsOnTaskbarThread(); }, nullptr);
}

void RemoveSettingsFromTaskbarThread()
{
    StopRetryTimer();
    RemoveAppliedMargins();
}

void RemoveSettings(HWND hTaskbarWnd)
{
    RunFromWindowThread(
        hTaskbarWnd, [](void *)
        { RemoveSettingsFromTaskbarThread(); },
        nullptr);
}

// Every taskbar window lives on the taskbar thread, and the apply pass already
// covers all the taskbars on the thread it runs on, so each thread is visited
// only once even when it owns several taskbar windows.
void ForEachTaskbarWindow(void (*proc)(HWND))
{
    struct ENUM_PARAM
    {
        void (*proc)(HWND);
        std::vector<DWORD> visitedThreadIds;
    };

    ENUM_PARAM param{proc};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL
        {
            auto *param = reinterpret_cast<ENUM_PARAM *>(lParam);

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
            {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
            {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Shell_TrayWnd") != 0 &&
                _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") != 0)
            {
                return TRUE;
            }

            DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
            if (std::find(param->visitedThreadIds.begin(),
                          param->visitedThreadIds.end(),
                          dwThreadId) != param->visitedThreadIds.end())
            {
                return TRUE;
            }

            param->visitedThreadIds.push_back(dwThreadId);
            param->proc(hWnd);

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName)
{
    if (!lpClassName)
    {
        return;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (!bTextualClassName)
    {
        return;
    }

    if (_wcsicmp(lpClassName, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0)
    {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        ApplySettings(hWnd);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 PVOID lpParam)
{
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd)
    {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI *)(DWORD dwExStyle,
                                            LPCWSTR lpClassName,
                                            LPCWSTR lpWindowName,
                                            DWORD dwStyle,
                                            int X,
                                            int Y,
                                            int nWidth,
                                            int nHeight,
                                            HWND hWndParent,
                                            HMENU hMenu,
                                            HINSTANCE hInstance,
                                            PVOID lpParam,
                                            DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    PVOID lpParam,
                                    DWORD dwBand)
{
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd)
    {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI *)(DWORD dwExStyle,
                                              LPCWSTR lpClassName,
                                              LPCWSTR lpWindowName,
                                              DWORD dwStyle,
                                              int X,
                                              int Y,
                                              int nWidth,
                                              int nHeight,
                                              HWND hWndParent,
                                              HMENU hMenu,
                                              HINSTANCE hInstance,
                                              PVOID lpParam,
                                              DWORD dwBand,
                                              DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
                                      LPCWSTR lpClassName,
                                      LPCWSTR lpWindowName,
                                      DWORD dwStyle,
                                      int X,
                                      int Y,
                                      int nWidth,
                                      int nHeight,
                                      HWND hWndParent,
                                      HMENU hMenu,
                                      HINSTANCE hInstance,
                                      PVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlags)
{
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd)
    {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
}

bool HookTaskbarDllSymbols()
{
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module)
    {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
            &ComputeJumpViewPosition_Original,
            ComputeJumpViewPosition_Hook,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks)))
    {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void LoadSettings()
{
    g_settings.placement = Placement::Left;
    WindhawkUtils::StringSetting placement =
        WindhawkUtils::StringSetting::make(L"placement");
    if (wcscmp(placement, L"right") == 0)
    {
        g_settings.placement = Placement::Right;
    }

    g_settings.leftMargin = Wh_GetIntSetting(L"leftMargin");
    if (g_settings.leftMargin < 0)
    {
        g_settings.leftMargin = 0;
    }

    g_settings.rightMargin = Wh_GetIntSetting(L"rightMargin");
    if (g_settings.rightMargin < 0)
    {
        g_settings.rightMargin = 0;
    }

    g_settings.followDpi = Wh_GetIntSetting(L"followDpi") != 0;

    g_settings.displays = Displays::All;
    WindhawkUtils::StringSetting displays =
        WindhawkUtils::StringSetting::make(L"displays");
    if (wcscmp(displays, L"primary") == 0)
    {
        g_settings.displays = Displays::Primary;
    }
    else if (wcscmp(displays, L"secondary") == 0)
    {
        g_settings.displays = Displays::Secondary;
    }
    else if (wcscmp(displays, L"specific") == 0)
    {
        g_settings.displays = Displays::Specific;
    }

    WindhawkUtils::StringSetting specificDisplay =
        WindhawkUtils::StringSetting::make(L"specificDisplay");
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings.specificDisplay = specificDisplay;
    }
}

BOOL Wh_ModInit()
{
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols())
    {
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module)
    {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand)
        {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }

        auto pCreateWindowInBandEx = (CreateWindowInBandEx_t)GetProcAddress(
            user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx)
        {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBandEx,
                                           CreateWindowInBandEx_Hook,
                                           &CreateWindowInBandEx_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit()
{
    Wh_Log(L">");

    ForEachTaskbarWindow(ApplySettings);
}

void Wh_ModBeforeUninit()
{
    Wh_Log(L">");

    g_unloading = true;

    ForEachTaskbarWindow(RemoveSettings);
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L">");

    LoadSettings();

    ForEachTaskbarWindow(ApplySettings);
}
