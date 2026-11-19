// ==WindhawkMod==
// @id              ryjs-launchpad
// @name            RYJS Launchpad
// @name:zh-CN      RYJS Launchpad
// @description     A fullscreen Launchpad-style Start replacement with expanded groups, free group layout, manual and web entries, themes, bilingual UI and instant search
// @description:zh-CN 全屏 Launchpad 风格开始菜单替代：展开分组、自由分组布局、手动与网址入口、主题、中英双语与即时搜索
// @version         1.0.0
// @author          Fangyu Guo (ryjs)
// @github          https://github.com/ryjs140
// @homepage        https://github.com/ryjs140/ryjs-launchpad
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lshell32 -lshcore -ldwmapi -ld2d1 -ldwrite -ld3d11 -ldxgi -ldcomp -lole32 -luuid -lcomdlg32 -luxtheme -lpropsys -lpowrprof -lwindowscodecs -limm32 -lcomctl32 -lwinhttp -lshlwapi -lruntimeobject -loleaut32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# RYJS Launchpad

Developed and maintained by **Fangyu Guo (ryjs)**. This is a derivative work,
not an independent rewrite:

| Project | Author | Role |
| --- | --- | --- |
| [Fullscreen Start (Launchpad style)](https://windhawk.net/mods/fullscreen-start) | [Rafael Mendes](https://github.com/RafaelMendes1407) | Original launcher, its rendering and the Start hook |
| [Seamless Launchpad](https://github.com/DavidHiFi/davids-windhawk-mods/tree/main/mods/local/seamless-launchpad) | [DavidHiFi](https://github.com/DavidHiFi) | Direct base: grid, search, folders, power dock, settings |
| RYJS Launchpad | Fangyu Guo (ryjs) | The changes listed under "RYJS edition" below |

All three are MIT licensed. The original copyright notices and the MIT text
are kept verbatim in the License section at the end of this readme.

**Settings precedence.** The built-in settings window (the `...` button) and
Windhawk's settings page edit the same settings. A value saved in the built-in
window overrides the Windhawk page; changing that same setting on the Windhawk
page again takes precedence. The Windhawk page does not refresh to show values
saved in the built-in window. Use either page, not both for the same setting.

![RYJS Launchpad](https://raw.githubusercontent.com/ryjs140/ryjs-launchpad/main/media/preview.png)

## RYJS edition
- Expanded groups show their apps directly. Choose Layout > Expanded groups;
  the original scrolling and paged folder modes remain available.
- Right-click empty space to create a group. Right-click a group title to
  rename it or enter Edit group layout. In edit mode drag a title to move
  the group, or its lower-right handle to resize by whole icon cells.
  Enter/Finish saves; Escape cancels the gesture, then the edit session.
  Drag apps between groups,
  or use Move to group (also available in search).
- Add entry accepts programs, shortcuts, Remote Desktop files, Store apps
  and web addresses. Edit entry saves aliases, arguments and working folders.
  Rename/Restore original name change only the Launchpad display name.
  Website icons are fetched on explicit add/update and cached locally.
- Labels have no backplates or shadows. Per-label pure black/white follows
  the text-free rendered blur plus tint; DWM materials use a pre-open estimate.
  New discoveries never insert themselves into an existing arrangement.
- Launchpad menus use a rounded acrylic surface and theme-aware text, while
  retaining native submenu, keyboard and accessibility behavior.
- Interface language follows Windows, with manual Chinese/English selection.
  The Interface font picker lists local fonts without changing icon glyphs.
- Icon/name sizes, spacing, group width/flow, blur, opacity, taskbar coverage,
  paging, durations, easing, reduced motion and frame caps are configurable.
- The built-in settings window saves to private per-mod storage without
  administrator access or windhawk-cli. Later changes to the same setting
  in Windhawk override its private value.
- Layout updates keep a .bak file. Import layout file and Restore layout
  backup are available in the ... menu. Group files are separate from the
  original folder layout; unknown newer schemas are never overwritten.
- performance.csv distinguishes first-visible latency, render cost, frame
  intervals, submission cadence and transition duration. Submission FPS is
  not a measurement of physical monitor presentation.

### 中文操作
在“应用布局”中选择“展开分组”，应用会直接显示在分组内；原文件夹模式仍可选。
右键空白处或点击搜索栏的“…”可新建分组，右键标题可重命名、前移、后移或删除分组。
进入“编辑分组布局”后拖动标题可自由移动分组，拖动右下角可调整尺寸；重叠时显示红色预览并拒绝提交。
“添加入口”支持程序、快捷方式、RDP、商店应用和网址；右键“编辑入口”可改名、路径、参数和图标。
缩小分组后在组内滚动查看应用，按住 Shift 滚动整体画布；未进入编辑模式时保留已有排列。
删除分组会将应用移到“未分组”，不会卸载应用。字体、名称字号、图标间距及分组布局可在设置中调整。
Launchpad 自有菜单采用圆角亚克力玻璃效果，并跟随当前主题调整文字与悬停对比度；原生子菜单和键盘操作保持不变。
设置保存在模块私有目录中；修改 Windhawk 设置页的同名选项后，以该次修改为准。
布局自动保留上一次备份；“…”菜单提供导入及恢复。导入前会保留当前布局。

---

Replaces the Start menu with a fullscreen grid of all your apps, in the style
of the macOS Launchpad, with instant search and a blurred background.

## Features
- **Fullscreen.** Fills the monitor the mouse is on, or one monitor you pick.
  It leaves the taskbar, side bars and the Standalone Dock visible, or covers
  the whole screen if you turn that on.
- **Your choice of apps.** The "All apps" list of Start, your desktop icons,
  both together, or only the apps you pin. With only pinned apps, search still
  finds every app, and right-click pins or unpins one.
- **Scroll or pages.** One list that scrolls, or pages that flip left and
  right with dots below, like Launchpad.
- **Instant search.** Just type. Case and accents don't matter, and names that
  start with your text come first. The search box is a real text field, with a
  blinking caret and click-and-drag selection, and Ctrl+A, copy, cut and paste
  work as usual.
- **GPU rendered.** The grid, the blur and the folder glass all rasterize on
  the GPU (Direct2D over Direct3D), so it stays snappy at any resolution.
- **Wiggle edit mode.** Grab and hold any icon and every icon starts to
  wiggle, like iOS, while you arrange them.
- **Adjustable blur.** Pick the blur type, strength, opacity and color of the
  background. The search bar, folder glass, dots and power bar use the
  Catppuccin Mocha palette.
- **Opens like Start.** The Windows key, Ctrl+Esc and the Start button of the
  taskbar or the Standalone Dock all open it.
- **Your own Windows key.** The Windows key, Shift + Windows key and the Start
  button can each open this grid, the original Start, or a program of your
  choice, such as a launcher. The program starts even when it isn't running,
  so this replaces the same feature of Open-Shell.
- **Your own order.** Drag icons to move them, across pages and in and out
  of folders. Drop an app on another to make a folder.
- **Right-click menu.** Open, run as administrator, open the file location
  or the shortcut location, change the icon, pin or unpin, or remove an app
  from the grid.
- **Power buttons.** A small glass bar at the bottom with Lock, Sign out,
  Sleep, Hibernate, Restart and Shut down. Pick which ones show, or hide the
  bar. The first click asks ("Click again to restart") and a second click on
  the same button does it. Turn on **No confirmation** to act on the first
  click.
- **Works over administrator windows.** Windows keeps keys pressed in an app
  running as administrator away from Explorer, so Shift + Windows key can't
  open the grid there. The optional helper in the `helper` folder of this
  mod's repository runs as administrator and fixes that (see its README).

## Keyboard and mouse
| Key | Action |
| --- | --- |
| Type | Search |
| Backspace, Ctrl+Backspace, Delete | Delete the selection, a letter, or the last word |
| Ctrl+A, Ctrl+C, Ctrl+X, Ctrl+V | Select all, copy, cut, paste |
| Left, Right, Home, End | Move the caret in the search; hold Shift to select. With an empty search, Left and Right pick an app |
| Up, Down, Enter | Pick an app and open it. While searching, the first result is already picked |
| Esc | Clear the search, or close when the search is empty |
| Mouse wheel, Page Up, Page Down | Scroll, or flip pages |
| Sideways scroll, page dots | Flip pages |
| The ... button, top right | Open this mod's own settings window (private settings), manage groups, restore/import layouts, bring back removed apps, or sort A to Z |
| Grab and hold an icon | All icons wiggle; drag to move it. Hold it at the screen's edge to flip the page |
| Drop an app on another | Make a folder. Drop more apps on it to add them |
| Hold an app over a folder | The folder opens, so you can place it inside |
| Drag an app out of an open folder | Take it out of the folder |
| Type in an open folder | Rename it |
| Right-click an icon | Its menu. Right-click empty space for the ... menu |
| Power buttons, bottom | Lock, sign out, sleep, hibernate, restart or shut down. Click twice, or once with No confirmation on |

Your order, folders, removed apps and icons are kept in Windhawk's storage
folder for this mod. New apps go at the end.

Clicking outside the icons or switching to another window closes it.

## Blur types
- **Adjustable** (default): a blurred snapshot of the screen, taken when Start
  opens. It stays still while Start is open.
- **Windows acrylic** and **Windows blur**: drawn by Windows, so they follow
  what changes behind them. Windows fixes their strength, so the strength
  setting doesn't apply.
- **No blur**: only the color.

## Notes
- Disable Seamless Launchpad, Fullscreen Start Plus and Fullscreen Start
  (Launchpad style) before you enable this mod. All of them replace Start, so
  only one should run. RYJS refuses to load while another Launchpad window is
  present and never disables or unloads that mod for you.
- On its first run, RYJS copies your order, folders, pins and icons from the
  storage folder of Seamless Launchpad Custom, Seamless Launchpad or Fullscreen
  Start Plus, if one of them is installed. Only those layout files are read,
  and nothing in the old folder is modified or deleted; the old settings file
  is copied into the new folder as `migration-original-ui-settings.ini` for
  reference rather than applied, so no settings are inherited silently. Set
  the appearance you want once in the settings window.
- The power buttons never force apps to close. If an app has unsaved work,
  Windows asks about it as usual. Hibernate shows only when hibernation is on
  in Windows.
- Set **Windows key**, **Shift + Windows key** or **Start button** to "Open
  the original Start" to get the normal Start menu for that action. Shift +
  Windows key opens it by default.
- If Open-Shell or another Start replacement also takes the Windows key, turn
  that feature off there, or it gets the key first.
- The first time the mod loads, Windhawk downloads the symbols of
  `twinui.pcshell.dll`, which can take a minute.
- Setting changes apply the next time Start opens.

## Compatibility
- Windhawk 1.7.3 and later, including 2.0. The settings block uses only the
  `$name`, `$description` and `$options` keys, so it parses on both.
- Windows 11. The Start replacement hooks `ShowStartView` in
  `twinui.pcshell.dll` and, when present, the `ExperienceToggleButton` of the
  Windows 11 taskbar. Windows 10 was not tested by the maintainer.
- Explorer only (`@include explorer.exe`). Nothing is injected into other
  processes.

## Data and network
- No telemetry and no analytics. RYJS does not send anything anywhere except
  the requests described next.
- The only network requests are made when you add a web entry or ask to
  refresh its icon: the page is read to find its declared icon, with
  `/favicon.ico` as the fallback. Declared icons can be hosted on a different
  server than the page you entered (a CDN or a third party). Requests have time
  limits and a size cap, are sent without cookies, and the result is cached in
  the mod's storage. Nothing is uploaded. Set a custom icon for the entry and
  no request is made for it.
- Layouts, groups, entries, icons and settings stay in Windhawk's per-mod
  storage folder on this PC. Scratch files (the exported Start layout and the
  backup worker) live in a `tmp` folder next to it and are removed when the
  work is done. The optional full backup writes a `.ryjs.zip`
  file only where you choose, and import restores data, never code. Running a
  Store app as administrator uses a temporary shortcut in that same `tmp`
  folder; it is deleted after the launch.

## Credits
- [Fullscreen Start (Launchpad style)](https://windhawk.net/mods/fullscreen-start)
  by [Rafael Mendes](https://github.com/RafaelMendes1407): the origin of the
  launcher, its rendering and its Start hook.
- [Seamless Launchpad](https://github.com/DavidHiFi/davids-windhawk-mods/tree/main/mods/local/seamless-launchpad)
  by [DavidHiFi](https://github.com/DavidHiFi): the direct base of this mod.
  Most of the grid, search, folder, power-dock and settings code comes from it.
- RYJS Launchpad changes by Fangyu Guo (ryjs).
- The Catppuccin Mocha palette used by the original search bar, folder glass
  and power bar.

All three projects are MIT licensed.

## Copyright notices

The notices below are reproduced as the upstream projects state them. No year
is invented for a project that does not declare one.

- **Seamless Launchpad** — `Copyright (c) 2026 DavidHiFi`, from the `LICENSE`
  file of https://github.com/DavidHiFi/davids-windhawk-mods.
- **Fullscreen Start (Launchpad style)** — `Copyright (c) Rafael Mendes`. The
  upstream mod declares `@license MIT` in its metadata but ships no separate
  `LICENSE` file and no copyright line, so no year is stated here.
- **RYJS Launchpad changes** — `Copyright (c) 2026 Fangyu Guo (ryjs)`.

## License

```text
MIT License

Copyright (c) 2026 DavidHiFi
Copyright (c) Rafael Mendes
Copyright (c) 2026 Fangyu Guo (ryjs)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- interfaceLanguage: auto
  $name: Interface language
  $description: Follow Windows display language. Save to apply; app names stay unchanged.
  $options:
  - auto: Automatic (Windows)
  - zh-CN: Simplified Chinese
  - en: English
  $name:zh-CN: 界面语言
  $description:zh-CN: 跟随 Windows 显示语言，保存后生效。应用名称保持原样。
  $options:zh-CN:
  - auto: 自动（跟随 Windows）
  - zh-CN: 简体中文
  - en: English（英语）
- uiFont: auto
  $name: Interface font
  $description: Choose a locally installed font. Applies to text only; symbol icons use their own font.
  $name:zh-CN: 界面字体
  $description:zh-CN: 选择本机已安装字体，用于所有界面文字。功能图标使用独立的符号字体。
- winKeyAction: grid
  $name: Windows key
  $description: Also Ctrl+Esc.
  $options:
  - grid: Open this grid
  - start: Open the original Start
  - program: Run the program below
  $name:zh-CN: Windows 键
  $description:zh-CN: 同时适用于 Ctrl+Esc。
  $options:zh-CN:
  - grid: 打开启动器
  - start: 打开系统开始菜单
  - program: 运行下方程序
- shiftWinKeyAction: start
  $name: Shift + Windows key
  $options:
  - grid: Open this grid
  - start: Open the original Start
  - program: Run the program below
  $name:zh-CN: Shift + Windows 键
  $options:zh-CN:
  - grid: 打开启动器
  - start: 打开系统开始菜单
  - program: 运行下方程序
- startButtonAction: grid
  $name: Start button
  $description: The taskbar Start button, the Standalone Dock's, and every other way Start opens that is not the
    keyboard.
  $options:
  - grid: Open this grid
  - start: Open the original Start
  - program: Run the program below
  $name:zh-CN: 开始按钮
  $description:zh-CN: 适用于任务栏、独立 Dock 的开始按钮及其他非键盘打开方式。
  $options:zh-CN:
  - grid: 打开启动器
  - start: 打开系统开始菜单
  - program: 运行下方程序
- program: ''
  $name: Program
  $description: What "Run the program below" starts. A command line such as "C:\Program Files\App\app.exe" --flag,
    or a shortcut, file, folder or web address
  $name:zh-CN: 程序
  $description:zh-CN: 选择“运行下方程序”时执行的命令。可填写带参数的程序路径、快捷方式、文件、文件夹或网址。
- monitor: mouse
  $name: Monitor
  $description: Where the grid opens. Monitors 2 to 4 are the other monitors, counted left to right. If the picked
    monitor is not connected, it opens where the mouse is.
  $options:
  - mouse: The monitor with the mouse
  - '1': Main monitor
  - '2': Monitor 2
  - '3': Monitor 3
  - '4': Monitor 4
  $name:zh-CN: 显示器
  $description:zh-CN: 选择启动器显示的位置。显示器 2 至 4 按从左到右排列；所选显示器未连接时，使用鼠标所在的显示器。
  $options:zh-CN:
  - mouse: 鼠标所在显示器
  - '1': 主显示器
  - '2': 显示器 2
  - '3': 显示器 3
  - '4': 显示器 4
- appSource: allApps
  $name: Apps to show
  $description: With pinned apps, search still finds every app. Right-click an app to pin or unpin it.
  $options:
  - allApps: Start menu apps (the All apps list)
  - desktop: Desktop icons
  - both: Start menu apps and desktop icons
  - pinned: Only the apps you pin
  $name:zh-CN: 显示的应用
  $description:zh-CN: 仅显示固定应用时仍可搜索全部应用。右键可固定或取消固定。
  $options:zh-CN:
  - allApps: 开始菜单中的所有应用
  - desktop: 桌面图标
  - both: 开始菜单应用与桌面图标
  - pinned: 仅显示固定的应用
- gridLayout: groups
  $name: Layout
  $description: How the grid is arranged.
  $options:
  - scroll: One list that scrolls up and down
  - pages: Pages that flip left and right, like Launchpad
  - groups: Expanded groups
  $name:zh-CN: 布局方式
  $description:zh-CN: 选择应用排列和浏览方式。
  $options:zh-CN:
  - scroll: 上下滚动列表
  - pages: 左右翻页
  - groups: 展开分组
- gridColumns: 0
  $name: Columns
  $description: '0 to 30. 0 picks a number that fits the screen (Suggested: 0).'
  $name:zh-CN: 每行图标数量
  $description:zh-CN: 范围 0–30。0 表示根据屏幕自动调整（建议：0）。
- globalColumns: 0
  $name: Columns per row
  $description: Global grid columns, 1–30. 0 keeps the saved density or estimates it for a new layout. Preview changes
    in the layout editor.
  $name:zh-CN: 每行列数（全局）
  $description:zh-CN: 全局列数为 1–30。0 沿用已保存的密度，新布局则自动估算。进入布局编辑可预览列数变化。
- coverScreen: false
  $name: Cover the whole screen
  $description: On, the grid also covers the taskbar, side bars and the Standalone Dock. Off, they stay visible.
  $name:zh-CN: 覆盖整个屏幕
  $description:zh-CN: 开启后覆盖任务栏、侧边栏和独立 Dock；关闭后保留它们的可见区域。
- gridIconSize: 56
  $name: Icon size
  $description: '24 to 192 (Suggested: 56). Pixels at 100% scaling.'
  $name:zh-CN: 图标大小
  $description:zh-CN: 范围 24–192，单位为 100% 缩放下的像素（建议：56）。
- showLabels: true
  $name: Icon labels
  $description: On, the app name shows under each icon. Off, icons only.
  $name:zh-CN: 显示应用名称
  $description:zh-CN: 开启后在图标下显示应用名称；关闭后仅显示图标。
- normalizeIcons: true
  $name: Fill small icons
  $description: Scale small or padded icons to full size.
  $name:zh-CN: 放大小图标
  $description:zh-CN: 将尺寸较小或留白较多的图标放大至统一大小。
- blurMode: custom
  $name: Background blur
  $description: The kind of blur behind the grid.
  $options:
  - custom: Adjustable (uses the strength below)
  - acrylic: Windows acrylic (DWM)
  - blur: Windows blur (DWM)
  - none: No blur
  $name:zh-CN: 背景模糊
  $description:zh-CN: 选择启动器的背景模糊方式。
  $options:zh-CN:
  - custom: 可调节（使用下方强度）
  - acrylic: Windows 亚克力（DWM）
  - blur: Windows 模糊（DWM）
  - none: 不模糊
- blurAmount: 60
  $name: Blur strength
  $description: '0 to 100 (Suggested: 60). Only for the adjustable blur. The Windows ones have a fixed strength.'
  $name:zh-CN: 模糊强度
  $description:zh-CN: 范围 0–100（建议：60）。仅对可调节模糊生效；Windows 模糊强度固定。
- backgroundOpacity: 30
  $name: Background opacity (%)
  $description: '0 to 100 (Suggested: 30). 0 leaves only the blur, 100 covers everything with the color.'
  $name:zh-CN: 背景不透明度（%）
  $description:zh-CN: 范围 0–100（建议：30）。0 仅保留模糊效果，100 使用纯色完全覆盖。
- tintColor: '#000000'
  $name: Background color
  $description: 'In hexadecimal, like #1E1E2E.'
  $name:zh-CN: 背景颜色
  $description:zh-CN: '使用十六进制颜色，例如 #1E1E2E。'
- glowStrength: 60
  $name: Glow strength
  $description: '0 to 100 (Suggested: 60). How strongly hovered buttons glow.'
  $name:zh-CN: 光晕强度
  $description:zh-CN: 范围 0–100（建议：60）。控制鼠标悬停时按钮的光晕强度。
- chromeOpacity: 90
  $name: Search bar and dock opacity (%)
  $description: '0 to 100 (Suggested: 90). How solid the search bar and the power dock are.'
  $name:zh-CN: 搜索栏与电源栏不透明度（%）
  $description:zh-CN: 范围 0–100（建议：90）。控制搜索栏与电源栏的不透明度。
- powerBar: true
  $name: Power dock
  $description: The pill of power buttons at the bottom of the grid.
  $name:zh-CN: 显示电源栏
  $description:zh-CN: 在启动器底部显示电源操作按钮。
- dockAlignment: center
  $name: Dock position
  $description: Where the power dock sits along the bottom edge.
  $options:
  - left: Left
  - center: Center
  - right: Right
  $name:zh-CN: 电源栏位置
  $description:zh-CN: 选择电源栏在屏幕底部的对齐位置。
  $options:zh-CN:
  - left: 靠左
  - center: 居中
  - right: 靠右
- powerButtons:
  - lock: true
    $name: Lock
    $name:zh-CN: 锁定
  - signOut: true
    $name: Sign out
    $name:zh-CN: 注销
  - sleep: true
    $name: Sleep
    $name:zh-CN: 睡眠
  - hibernate: true
    $name: Hibernate
    $description: Shows only when hibernation is turned on in Windows.
    $name:zh-CN: 休眠
    $description:zh-CN: 仅在 Windows 已启用休眠时显示。
  - restart: true
    $name: Restart
    $name:zh-CN: 重启
  - shutDown: true
    $name: Shut down
    $name:zh-CN: 关机
  $name: Power dock buttons
  $name:zh-CN: 电源栏按钮
- powerNoConfirm: false
  $name: Skip confirmation
  $description: On, a power button acts on the first click. Off, the first click arms it and a second click on the
    same button confirms.
  $name:zh-CN: 跳过二次确认
  $description:zh-CN: 开启后点击一次即执行。关闭后需再次点击同一按钮确认。
- wiggle: true
  $name: Edit mode wiggle
  $description: While you drag an icon, all icons wiggle like on iOS.
  $name:zh-CN: 编辑时图标轻晃
  $description:zh-CN: 拖动图标时，其他图标轻微晃动以提示编辑状态。
- holdTime: 420
  $name: Drag hold time
  $description: '150 to 1500 (Suggested: 420). Milliseconds to hold an icon before dragging starts.'
  $name:zh-CN: 长按拖动等待时间
  $description:zh-CN: 范围 150–1500 毫秒（建议：420）。长按图标达到此时间后开始拖动。
- animations: true
  $name: Animations
  $description: Off, everything snaps instantly. Opening, folders, hover fades, tile moves and wiggle all turn off.
  $name:zh-CN: 启用动画
  $description:zh-CN: 关闭后立即完成切换，同时停用打开、文件夹、悬停、图标移动和轻晃动画。
- settingsGlass: true
  $name: Blurred settings window
  $description: On, the settings window is see-through glass with Windows acrylic behind it. Off, it is solid. Applies
    when you save.
  $name:zh-CN: 设置窗口毛玻璃
  $description:zh-CN: 开启后设置窗口使用半透明亚克力背景；关闭后使用不透明背景。保存后生效。
- nativeSettingsTitleBar: false
  $name: Native Windows title bar
  $description: Use the Windows title bar and window buttons for this settings window. Applies when you save.
  $name:zh-CN: 使用系统标题栏
  $description:zh-CN: 为设置窗口使用 Windows 原生标题栏和窗口按钮。保存后生效。
- settingsTrayIcon: true
  $name: Tray icon
  $description: A Launchpad icon in the system tray. Click it to open these settings, right-click it for a menu.
  $name:zh-CN: 显示托盘图标
  $description:zh-CN: 在系统托盘显示启动器图标。点击打开设置，右键打开菜单。
- gpuPresenter: true
  $name: GPU composition
  $description: Use GPU composition for smoother animation. Turn off for the compatibility renderer.
  $name:zh-CN: GPU 合成呈现
  $description:zh-CN: 使用 GPU 合成呈现以改善动画流畅度；关闭后使用兼容渲染路径。
- performanceLogging: false
  $name: Record performance
  $description: Record opening latency, frame intervals and rendering cost in the mod storage folder.
  $name:zh-CN: 记录性能
  $description:zh-CN: 在模块存储目录记录打开延迟、帧间隔和渲染耗时。
- frameRateLimit: 0
  $name: Frame rate limit
  $description: 0 follows the selected monitor. Other values cap animation updates without changing the monitor.
  $name:zh-CN: 动画帧率上限
  $description:zh-CN: 0 跟随所选显示器刷新率。其他值限制动画更新频率，不改变显示器设置。
- openDuration: 280
  $name: Open duration (ms)
  $description: Duration of the opening transition.
  $name:zh-CN: 打开动画时长（毫秒）
  $description:zh-CN: 控制启动器打开过渡的持续时间。
- closeDuration: 180
  $name: Close duration (ms)
  $description: Duration of the closing transition.
  $name:zh-CN: 关闭动画时长（毫秒）
  $description:zh-CN: 控制启动器关闭过渡的持续时间。
- animationEasing: settle
  $name: Animation easing
  $description: Choose how movement slows down. These curves do not overshoot.
  $options:
  - settle: Gentle settle
  - cubic: Cubic ease-out
  - quint: Quintic ease-out
  $name:zh-CN: 动画缓动
  $description:zh-CN: 选择动画减速收尾的方式，所有曲线均无过度回弹。
  $options:zh-CN:
  - settle: 柔和收尾
  - cubic: 三次方减速
  - quint: 五次方减速
- reducedMotion: false
  $name: Reduce motion
  $description: Use immediate transitions. Windows animation accessibility settings are also respected.
  $name:zh-CN: 减少动画
  $description:zh-CN: 立即完成过渡，同时遵循 Windows 的动画辅助功能设置。
- labelFontSize: 13
  $name: Application name size
  $description: Text size in DIP. Display scaling is applied automatically.
  $name:zh-CN: 应用名称字号
  $description:zh-CN: 使用设备无关像素 DIP，自动适应显示缩放。
- horizontalSpacing: 56
  $name: Horizontal icon spacing
  $description: Preferred horizontal space between icons, in DIP.
  $name:zh-CN: 图标水平间距
  $description:zh-CN: 图标之间的首选水平间距，单位为 DIP。
- verticalSpacing: 16
  $name: Vertical icon spacing
  $description: Additional space between icon rows, in DIP.
  $name:zh-CN: 图标垂直间距
  $description:zh-CN: 图标行之间的额外间距，单位为 DIP。
- groupWidth: 0
  $name: Group width
  $description: Preferred group width in DIP. 0 adapts to the screen and number of groups.
  $name:zh-CN: 分组宽度
  $description:zh-CN: 首选分组宽度，单位为 DIP。0 根据屏幕和分组数量自动调整。
- groupSpacing: 32
  $name: Group spacing
  $description: Minimum horizontal and vertical space between groups, in DIP.
  $name:zh-CN: 分组间距
  $description:zh-CN: 分组之间水平和垂直方向的最小间距，单位为 DIP。
- groupFlow: columns
  $name: Group arrangement
  $description: Arrange groups in columns or use one full-width group per row.
  $options:
  - columns: Responsive columns
  - rows: Full-width rows
  $name:zh-CN: 分组排列方式
  $description:zh-CN: 将分组排列成多列，或每行使用一个全宽分组。
  $options:zh-CN:
  - columns: 自适应多列
  - rows: 每行一个全宽分组
- groupNavigation: scroll
  $name: Group navigation
  $description: Large groups continue on the next page when paging is selected.
  $options:
  - scroll: Vertical scrolling
  - pages: Horizontal pages
  $name:zh-CN: 分组浏览方式
  $description:zh-CN: 选择分页时，大分组的其余应用会在后续页面继续显示。
  $options:zh-CN:
  - scroll: 上下滚动
  - pages: 左右分页
- folderDuration: 180
  $name: Folder duration (ms)
  $description: Duration of folder expansion and collapse.
  $name:zh-CN: 文件夹动画时长（毫秒）
  $description:zh-CN: 文件夹展开与收起的动画时长。
- themeMode: custom
  $name: Theme mode
  $description: Choose a preset, follow Windows, or keep your own colors.
  $options:
  - custom: Custom
  - light: Light
  - dark: Dark
  - system: Follow Windows
  $name:zh-CN: 主题模式
  $description:zh-CN: 选择浅色、深色、跟随 Windows，或保留自定义颜色。
  $options:zh-CN:
  - custom: 自定义
  - light: 浅色
  - dark: 深色
  - system: 跟随 Windows
- backgroundDim: 0
  $name: Background dimming
  $description: Additional darkening of the blurred background, 0–80 percent.
  $name:zh-CN: 背景压暗
  $description:zh-CN: 进一步压暗毛玻璃背景，范围为 0～80%。
- launchpadAnimation: iconFlyIn
  $name: Launchpad animation style
  $description: Choose how the launchpad opens and closes.
  $options:
  - iconFlyIn: iOS icon fly-in
  - classic: Classic zoom and fade
  $name:zh-CN: 启动台动画风格
  $description:zh-CN: 选择启动台展开和收起的动画风格。
  $options:zh-CN:
  - iconFlyIn: iOS 图标飞入
  - classic: 经典缩放与淡入淡出
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <wincodec.h>
#include <winhttp.h>
#include <shlwapi.h>
#include <regex>
// Original code and credits: DavidHiFi and Rafael Mendes. See the MIT readme above.
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <knownfolders.h>
#include <shellscalingapi.h>
#include <commdlg.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <dwmapi.h>
#include <propsys.h>
#include <powrprof.h>
#include <imm.h>
// Otherwise the Win32 macro renames ID2D1RenderTarget::DrawText.
#undef DrawText
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <d2d1effects.h>
#include <d3d11.h>
#include <dwrite.h>
#include <wrl/client.h>
#ifdef SEAMLESS_PREVIEW
// preview_api.h is used by dev/preview only, never by the mod.
#else
#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Core.h>
#endif
#include <windows.h>
#include <algorithm>
#include <atomic>
#include <iterator>
#include <string>
#include <string_view>

namespace launchpad_i18n {
// English phrases are stable resource keys; stored app/folder names are never translated.
inline std::atomic<bool> chinese{false};
struct Resource { std::wstring_view english; PCWSTR zhCN; };
inline constexpr Resource resources[] = {
// Generated by dev/generate_resources.py. Edit dev/strings.zh-CN.json.
    {L"  ·  MIT license  ·  A Windhawk mod", L"  ·  MIT 许可证  ·  Windhawk 模块"},
    {L" changes not saved", L" 项更改未保存"},
    {L" unavailable", L" 个暂不可用"},
    {L"0 follows the selected monitor. Other values cap animation updates without changing the monitor.", L"0 跟随所选显示器刷新率。其他值限制动画更新频率，不改变显示器设置。"},
    {L"0 to 100 (Suggested: 30). 0 leaves only the blur, 100 covers everything with the color.", L"范围 0–100（建议：30）。0 仅保留模糊效果，100 使用纯色完全覆盖。"},
    {L"0 to 100 (Suggested: 60). How strongly hovered buttons glow.", L"范围 0–100（建议：60）。控制鼠标悬停时按钮的光晕强度。"},
    {L"0 to 100 (Suggested: 60). Only for the adjustable blur. The Windows ones have a fixed strength.", L"范围 0–100（建议：60）。仅对可调节模糊生效；Windows 模糊强度固定。"},
    {L"0 to 100 (Suggested: 90). How solid the search bar and the power dock are.", L"范围 0–100（建议：90）。控制搜索栏与电源栏的不透明度。"},
    {L"0 to 30. 0 picks a number that fits the screen (Suggested: 0).", L"范围 0–30。0 表示根据屏幕自动调整（建议：0）。"},
    {L"0–100 percent, combined with the name backplate.", L"0～100%，可与名称底衬共同使用。"},
    {L"0–100 percent. Zero preserves transparent expanded groups.", L"0～100%。设为 0 时展开分组保持透明。"},
    {L"1 change not saved", L"有 1 项更改未保存"},
    {L"150 to 1500 (Suggested: 420). Milliseconds to hold an icon before dragging starts.", L"范围 150–1500 毫秒（建议：420）。长按图标达到此时间后开始拖动。"},
    {L"24 to 192 (Suggested: 56). Pixels at 100% scaling.", L"范围 24–192，单位为 100% 缩放下的像素（建议：56）。"},
    {L"24 to 192 (Suggested: 72). Pixels at 100% scaling.", L"范围 24–192，单位为 100% 缩放下的像素（建议：72）。"},
    {L"4 to 40 (Suggested: 14). How far a glow spreads, in DIP at 100% scaling.", L"范围 4–40（建议：14），单位为设备无关像素 DIP。"},
    {L"A Launchpad icon in the system tray. Click it to open these settings, right-click it for a menu.", L"在系统托盘显示启动器图标。点击打开设置，右键打开菜单。"},
    {L"A backup or import is already running.", L"正在备份或导入，请等待完成。"},
    {L"About", L"关于"},
    {L"Accent color", L"强调色"},
    {L"Add entry…", L"添加入口…"},
    {L"Add or edit entry", L"添加或编辑入口"},
    {L"Additional darkening of the blurred background, 0–80 percent.", L"进一步压暗毛玻璃背景，范围为 0～80%。"},
    {L"Additional space between icon rows, in DIP.", L"图标行之间的额外间距，单位为 DIP。"},
    {L"Adjustable (uses the strength below)", L"可调节（使用下方强度）"},
    {L"All files", L"所有文件"},
    {L"Also Ctrl+Esc.", L"同时适用于 Ctrl+Esc。"},
    {L"Animation easing", L"动画缓动"},
    {L"Animations", L"启用动画"},
    {L"Appearance", L"外观"},
    {L"Application and group names in the custom theme.", L"自定义主题中的应用和分组名称颜色。"},
    {L"Application name", L"应用显示名称"},
    {L"Application name size", L"应用名称字号"},
    {L"Apps to show", L"显示的应用"},
    {L"Apps, folders and search in one fullscreen grid, in place of Start.", L"在全屏启动器中浏览、搜索应用并整理文件夹。"},
    {L"Apps, icon sizes, pages and motion.", L"调整应用来源、图标大小、翻页和动画。"},
    {L"Arrange groups in columns or use one full-width group per row.", L"将分组排列成多列，或每行使用一个全宽分组。"},
    {L"Author", L"作者"},
    {L"Automatic (Windows)", L"自动（跟随 Windows）"},
    {L"Automatic (language default)", L"自动（按界面语言选择）"},
    {L"Background blur", L"背景模糊"},
    {L"Background color", L"背景颜色"},
    {L"Background color of the application under the pointer.", L"鼠标悬停应用的背景色。"},
    {L"Background color of the keyboard-selected application.", L"键盘选中应用的背景色。"},
    {L"Background dimming", L"背景压暗"},
    {L"Background opacity (%)", L"背景不透明度（%）"},
    {L"Backup and restore", L"备份与恢复"},
    {L"Backup verified:", L"备份已通过校验："},
    {L"Based on Fullscreen Start by Rafael Mendes", L"基于 Rafael Mendes 的 Fullscreen Start"},
    {L"Blur strength", L"模糊强度"},
    {L"Blur, colors, glow, search bar and this window.", L"调整模糊、颜色、光晕、搜索栏和设置窗口。"},
    {L"Blurred settings window", L"设置窗口毛玻璃"},
    {L"Bring back a removed app", L"恢复已移除的应用"},
    {L"Bring them all back", L"恢复所有已移除应用"},
    {L"Browse…", L"浏览…"},
    {L"Cancel", L"取消"},
    {L"Cancel group editing", L"取消分组编辑"},
    {L"Cannot equalize spacing in this arrangement.", L"当前排列无法统一间距，请先调整分组位置或尺寸。"},
    {L"Center", L"居中"},
    {L"Change icon…", L"更换图标…"},
    {L"Choose Store app…", L"选择商店应用…"},
    {L"Choose a locally installed font. Applies to text only; symbol icons use their own font.", L"选择本机已安装字体，用于所有界面文字。功能图标使用独立的符号字体。"},
    {L"Choose a picture for the icon", L"选择图标图片"},
    {L"Choose a preset, follow Windows, or keep your own colors.", L"选择浅色、深色、跟随 Windows，或保留自定义颜色。"},
    {L"Choose how movement slows down. These curves do not overshoot.", L"选择动画减速收尾的方式，所有曲线均无过度回弹。"},
    {L"Choose how the launchpad opens and closes.", L"选择启动台展开和收起的动画风格。"},
    {L"Choose how you open Launchpad and other programs.", L"选择启动器和其他程序的打开方式。"},
    {L"Choose which power actions appear in the dock.", L"选择电源栏中显示的操作。"},
    {L"Classic zoom and fade", L"经典缩放与淡入淡出"},
    {L"Click again to hibernate", L"再次点击以休眠"},
    {L"Click again to lock", L"再次点击以锁定"},
    {L"Click again to restart", L"再次点击以重启"},
    {L"Click again to shut down", L"再次点击以关机"},
    {L"Click again to sign out", L"再次点击以注销"},
    {L"Click again to sleep", L"再次点击以睡眠"},
    {L"Close duration (ms)", L"关闭动画时长（毫秒）"},
    {L"Close the settings window and try importing again.", L"请关闭设置窗口后重新导入。"},
    {L"Columns", L"每行图标数量"},
    {L"Columns per row", L"每行列数（全局）"},
    {L"Complete Launchpad backups", L"Launchpad 完整备份"},
    {L"Complete backup imported:", L"完整备份已导入："},
    {L"Complete backup saved and verified:", L"完整备份已保存并通过校验："},
    {L"Complete backup/import failed:", L"备份或导入失败："},
    {L"Complete backup/import in progress…", L"正在备份或导入，请稍候…"},
    {L"Continue editing", L"继续编辑"},
    {L"Corner radius in DIP, independent of icon size.", L"底衬圆角大小，不影响图标尺寸。"},
    {L"Could not back up the current layout. Import was cancelled.", L"无法备份当前布局，已取消导入。"},
    {L"Could not export the layout. Use a full backup to preserve unreadable data.", L"当前布局无法导出。请使用完整备份工具保留不可读数据。"},
    {L"Could not open this entry. Use Edit entry to repair its target.", L"无法打开此入口，请通过“编辑入口”修复目标路径。"},
    {L"Could not open {0} selected apps.", L"有 {0} 个选中的应用无法打开。"},
    {L"Could not restore group geometry.", L"无法恢复分组位置和尺寸。"},
    {L"Could not save entry.", L"入口保存失败。"},
    {L"Could not save group geometry.", L"分组位置和尺寸保存失败，已保留原状态。"},
    {L"Could not save private settings. The previous file was kept.", L"无法保存模块设置，已保留原设置文件。"},
    {L"Could not save the current layout before backup.", L"备份前无法保存当前布局。"},
    {L"Could not save the layout. The previous layout was kept.", L"布局保存失败，原有布局已保留。"},
    {L"Could not start the backup helper.", L"无法启动备份助手。"},
    {L"Couldn't find windhawk-cli.exe, so nothing was saved", L"未找到 windhawk-cli.exe，设置未保存。"},
    {L"Couldn't start windhawk-cli (error %lu)", L"无法启动 windhawk-cli（错误 %lu）"},
    {L"Couldn't write settings-save.txt", L"无法写入 settings-save.txt"},
    {L"Cover the whole screen", L"覆盖整个屏幕"},
    {L"Create complete backup", L"完整备份"},
    {L"Create complete backup…", L"完整备份…"},
    {L"Credits", L"致谢"},
    {L"Cubic ease-out", L"三次方减速"},
    {L"Custom", L"自定义"},
    {L"Custom icon (optional)", L"自定义图标（可选）"},
    {L"Dark", L"深色"},
    {L"Data is read-only. Resolve the load error before editing.", L"数据当前只读，请先解决读取问题再编辑。"},
    {L"DavidHiFi on GitHub", L"原作者 DavidHiFi 的 GitHub"},
    {L"Delete group", L"删除分组（应用移到未分组）"},
    {L"Delete manual entry", L"删除手动入口"},
    {L"Desktop icons", L"桌面图标"},
    {L"Destination group", L"目标分组"},
    {L"Discard changes", L"放弃修改"},
    {L"Display name", L"显示名称"},
    {L"Dock position", L"电源栏位置"},
    {L"Drag hold time", L"长按拖动等待时间"},
    {L"Drop apps here", L"将应用拖到这里"},
    {L"Duration of folder expansion and collapse.", L"文件夹展开与收起的动画时长。"},
    {L"Duration of the closing transition.", L"控制启动器关闭过渡的持续时间。"},
    {L"Duration of the opening transition.", L"控制启动器打开过渡的持续时间。"},
    {L"Edit entry…", L"编辑入口…"},
    {L"Edit group layout", L"编辑分组布局"},
    {L"Edit mode wiggle", L"编辑时图标轻晃"},
    {L"End of current grid", L"当前网格末尾"},
    {L"English", L"English（英语）"},
    {L"Enter a name and a valid target.", L"请填写显示名称和有效的文件路径或网址。"},
    {L"Equalize group spacing", L"统一分组间距"},
    {L"Exit preview", L"退出预览"},
    {L"Expanded groups", L"展开分组"},
    {L"Export layout file", L"导出当前布局文件"},
    {L"File or web address", L"文件路径或网址"},
    {L"Fill small icons", L"放大小图标"},
    {L"Finish and save", L"完成并保存"},
    {L"Finish group editing", L"保存并完成编辑"},
    {L"Folder", L"文件夹"},
    {L"Folder duration (ms)", L"文件夹动画时长（毫秒）"},
    {L"Follow Windows", L"跟随 Windows"},
    {L"Follow Windows display language. Save to apply; app names stay unchanged.", L"跟随 Windows 显示语言，保存后生效。应用名称保持原样。"},
    {L"Frame rate limit", L"动画帧率上限"},
    {L"Free and open source. A tip on Ko-fi keeps it moving.", L"免费开源，可通过 Ko-fi 支持原作者继续维护。"},
    {L"Full-width rows", L"每行一个全宽分组"},
    {L"GPU composition", L"GPU 合成呈现"},
    {L"Gentle settle", L"柔和收尾"},
    {L"Global grid columns, 1–30. 0 keeps the saved density or estimates it for a new layout. Preview changes in the layout editor.", L"全局列数为 1–30。0 沿用已保存的密度，新布局则自动估算。进入布局编辑可预览列数变化。"},
    {L"Glow size", L"光晕范围"},
    {L"Glow strength", L"光晕强度"},
    {L"Grid", L"应用布局"},
    {L"Group and folder background in the custom theme.", L"自定义主题中的分组与文件夹背景色。"},
    {L"Group arrangement", L"分组排列方式"},
    {L"Group background color", L"分组背景颜色"},
    {L"Group background opacity", L"分组背景透明度"},
    {L"Group name", L"分组名称"},
    {L"Group navigation", L"分组浏览方式"},
    {L"Group spacing", L"分组间距"},
    {L"Group width", L"分组宽度"},
    {L"Hibernate", L"休眠"},
    {L"Horizontal icon spacing", L"图标水平间距"},
    {L"Horizontal pages", L"左右分页"},
    {L"Hover color", L"悬停颜色"},
    {L"How the grid is arranged.", L"选择应用排列和浏览方式。"},
    {L"Icon labels", L"显示应用名称"},
    {L"Icon size", L"图标大小"},
    {L"Import complete backup", L"导入完整备份"},
    {L"Import complete backup…", L"导入完整备份…"},
    {L"Import layout file", L"导入布局文件"},
    {L"In hexadecimal, like #1E1E2E.", L"使用十六进制颜色，例如 #1E1E2E。"},
    {L"In hexadecimal, like #89B4FA. Tints the glows and highlights.", L"使用十六进制颜色，例如 #89B4FA。用于光晕和高亮。"},
    {L"Independent preview. Keyboard hooks and system integration are inactive.", L"独立预览：按键接管和系统集成均未启用，此页按键设置仅保存到预览数据。"},
    {L"Interface font", L"界面字体"},
    {L"Interface language", L"界面语言"},
    {L"Invalid placement: release to keep previous position", L"无法放置：松开后保留原位置"},
    {L"Keys", L"启动方式"},
    {L"Language", L"语言"},
    {L"Large groups continue on the next page when paging is selected.", L"选择分页时，大分组的其余应用会在后续页面继续显示。"},
    {L"Launch arguments", L"启动参数"},
    {L"Launch failed", L"启动失败"},
    {L"Launchpad animation style", L"启动台动画风格"},
    {L"Launchpad layout files", L"启动器布局文件"},
    {L"Layout", L"布局方式"},
    {L"Layout data is unreadable or unsupported: changes are read-only. Restore a verified backup.", L"布局数据不可读或格式不受支持，当前禁止保存。请恢复已校验的备份。"},
    {L"Left", L"靠左"},
    {L"Light", L"浅色"},
    {L"Like it? Help the mod keep going.", L"支持这个开源项目"},
    {L"Loading apps…", L"正在加载应用…"},
    {L"Lock", L"锁定"},
    {L"Look", L"外观"},
    {L"Made by DavidHiFi, based on Fullscreen Start by Rafael Mendes.", L"DavidHiFi 开发，基于 Rafael Mendes 的 Fullscreen Start。"},
    {L"Main monitor", L"主显示器"},
    {L"Maintained by ryjs. Based on DavidHiFi's Seamless Launchpad and Rafael Mendes's Fullscreen Start.", L"由 ryjs 维护，基于 DavidHiFi 的 Seamless Launchpad 和 Rafael Mendes 的 Fullscreen Start。"},
    {L"Minimum horizontal and vertical space between groups, in DIP.", L"分组之间水平和垂直方向的最小间距，单位为 DIP。"},
    {L"Monitor", L"显示器"},
    {L"Monitor 2", L"显示器 2"},
    {L"Monitor 3", L"显示器 3"},
    {L"Monitor 4", L"显示器 4"},
    {L"Motion", L"动画"},
    {L"Move group earlier", L"分组前移"},
    {L"Move group later", L"分组后移"},
    {L"Move to group", L"移动到分组"},
    {L"Name backplate corner radius", L"名称底衬圆角"},
    {L"Name backplate opacity", L"名称底衬强度"},
    {L"Name shadow strength", L"名称阴影强度"},
    {L"Native Windows title bar", L"使用系统标题栏"},
    {L"New group", L"新建分组"},
    {L"Newer layout format: changes are read-only. Restore a backup or import a supported layout.", L"布局版本较新，当前只读。请恢复备份或导入兼容布局。"},
    {L"No apps", L"暂无应用"},
    {L"No apps found", L"未找到应用"},
    {L"No blur", L"不模糊"},
    {L"No room for the original group size.", L"当前布局空间不足，无法恢复分组的原始尺寸。"},
    {L"Not saved. The admin prompt was turned down.", L"管理员授权已取消，设置未保存。"},
    {L"Not set", L"未设置"},
    {L"Nothing changed", L"设置未发生变化"},
    {L"Nothing pinned yet. Type to find an app, then right-click it to pin it", L"尚未固定应用。输入名称搜索，然后右键固定应用。"},
    {L"Off, everything snaps instantly. Opening, folders, hover fades, tile moves and wiggle all turn off.", L"关闭后立即完成切换，同时停用打开、文件夹、悬停、图标移动和轻晃动画。"},
    {L"On, a power button acts on the first click. Off, the first click arms it and a second click on the same button confirms.", L"开启后点击一次即执行。关闭后需再次点击同一按钮确认。"},
    {L"On, the app name shows under each icon. Off, icons only.", L"开启后在图标下显示应用名称；关闭后仅显示图标。"},
    {L"On, the grid also covers the taskbar, side bars and the Standalone Dock. Off, they stay visible.", L"开启后覆盖任务栏、侧边栏和独立 Dock；关闭后保留它们的可见区域。"},
    {L"On, the settings window is see-through glass with Windows acrylic behind it. Off, it is solid. Applies when you save.", L"开启后设置窗口使用半透明亚克力背景；关闭后使用不透明背景。保存后生效。"},
    {L"One list that scrolls up and down", L"上下滚动列表"},
    {L"Only the apps you pin", L"仅显示固定的应用"},
    {L"Open", L"打开"},
    {L"Open Ko-fi", L"打开 Ko-fi"},
    {L"Open Launchpad", L"打开启动器"},
    {L"Open duration (ms)", L"打开动画时长（毫秒）"},
    {L"Open file location", L"打开文件所在位置"},
    {L"Open settings", L"打开设置"},
    {L"Open shortcut location", L"打开快捷方式所在位置"},
    {L"Open the original Start", L"打开系统开始菜单"},
    {L"Open this grid", L"打开启动器"},
    {L"Open {0}", L"打开 {0} 个"},
    {L"Opens a new issue on GitHub, with the mod's name filled in", L"在 GitHub 创建问题，并自动填写模块名称。"},
    {L"Original size", L"原始尺寸"},
    {L"Overlap: release to keep previous position", L"发生重叠：松开后保留原位置"},
    {L"Pages that flip left and right, like Launchpad", L"左右翻页"},
    {L"Pictures and icons", L"图片和图标"},
    {L"Pin to this grid", L"固定到启动器"},
    {L"Power", L"电源"},
    {L"Power buttons", L"电源按钮"},
    {L"Power dock", L"显示电源栏"},
    {L"Power dock buttons", L"电源栏按钮"},
    {L"Preferred group width in DIP. 0 adapts to the screen and number of groups.", L"首选分组宽度，单位为 DIP。0 根据屏幕和分组数量自动调整。"},
    {L"Preferred horizontal space between icons, in DIP.", L"图标之间的首选水平间距，单位为 DIP。"},
    {L"Press keys...", L"请按快捷键…"},
    {L"Press the keys", L"请按快捷键"},
    {L"Press the keys you want. Esc cancels, Backspace clears.", L"请按所需快捷键。Esc 取消，Backspace 清空。"},
    {L"Preview settings could not be saved.", L"无法保存预览设置。"},
    {L"Preview: power actions are disabled.", L"独立预览：电源操作已禁用。"},
    {L"Previous data:", L"替换前的数据："},
    {L"Program", L"程序"},
    {L"Project", L"项目"},
    {L"Project page on GitHub", L"GitHub 项目主页"},
    {L"Properties", L"属性"},
    {L"Protect names from bright window glare. Zero turns the backplate off.", L"避免白色窗口光晕淹没名称，设为 0 可关闭底衬。"},
    {L"Put every app back in A to Z order and out of its folder? Removed apps stay removed.", L"将所有应用移出文件夹并按名称重新排序？已移除的应用不会恢复。"},
    {L"Quintic ease-out", L"五次方减速"},
    {L"RYJS Launchpad settings", L"RYJS Launchpad 设置"},
    {L"Readability", L"文字可读性"},
    {L"Record opening latency, frame intervals and rendering cost in the mod storage folder.", L"在模块存储目录记录打开延迟、帧间隔和渲染耗时。"},
    {L"Record performance", L"记录性能"},
    {L"Reduce motion", L"减少动画"},
    {L"Refresh website icon", L"重新获取网站图标"},
    {L"Remove from group", L"移出此分组"},
    {L"Remove from this grid", L"从启动器中移除"},
    {L"Rename", L"重命名"},
    {L"Rename group", L"重命名分组"},
    {L"Rendering", L"渲染"},
    {L"Replace the current layout, icons and settings? The previous data will be kept for rollback.", L"要替换当前布局、图标和设置吗？原数据将保留，便于回退。"},
    {L"Report a problem", L"报告问题"},
    {L"Reset to suggested", L"恢复建议值"},
    {L"Responsive columns", L"自适应多列"},
    {L"Restart", L"重启"},
    {L"Restore layout backup", L"恢复上一次布局备份"},
    {L"Restore original group size", L"恢复分组原始尺寸"},
    {L"Restore original icon", L"恢复原始图标"},
    {L"Restore original name", L"恢复原始名称"},
    {L"Restore previous group geometry", L"恢复上次分组位置和尺寸"},
    {L"Restore the previous saved group geometry?", L"是否恢复上次保存的分组位置和尺寸？"},
    {L"Right", L"靠右"},
    {L"Run as administrator", L"以管理员身份运行"},
    {L"Run the program below", L"运行下方程序"},
    {L"Save", L"保存"},
    {L"Save changes to the group layout?", L"是否保存分组布局修改？"},
    {L"Save layout", L"保存"},
    {L"Save layout? Yes saves, No discards, Cancel continues editing.", L"是否保存布局？选择是以保存，选择否以放弃修改，选择取消以继续编辑。"},
    {L"Saved", L"已保存"},
    {L"Saving...", L"正在保存…"},
    {L"Scale small or padded icons to full size.", L"将尺寸较小或留白较多的图标放大至统一大小。"},
    {L"Seamless Launchpad settings", L"Seamless Launchpad 设置"},
    {L"Search", L"搜索应用"},
    {L"Search bar and dock opacity (%)", L"搜索栏与电源栏不透明度（%）"},
    {L"Search for an app, then use Move to group", L"搜索应用后，右键选择“移动到分组”"},
    {L"Selected {0}", L"已选 {0} 个"},
    {L"Selection color", L"选中颜色"},
    {L"Settings", L"设置"},
    {L"Settings window", L"设置窗口"},
    {L"Shift + Windows key", L"Shift + Windows 键"},
    {L"Show as expanded group", L"显示为常驻分组"},
    {L"Show as traditional folder", L"显示为传统文件夹"},
    {L"Shows only when hibernation is turned on in Windows.", L"仅在 Windows 已启用休眠时显示。"},
    {L"Shut down", L"关机"},
    {L"Sign out", L"注销"},
    {L"Simplified Chinese", L"简体中文"},
    {L"Skip confirmation", L"跳过二次确认"},
    {L"Sleep", L"睡眠"},
    {L"Sort A to Z", L"按名称排序"},
    {L"Sort apps within groups", L"对各组内应用按名称排序"},
    {L"Start button", L"开始按钮"},
    {L"Start menu apps (the All apps list)", L"开始菜单中的所有应用"},
    {L"Start menu apps and desktop icons", L"开始菜单应用与桌面图标"},
    {L"Suggested values are filled in. Press Save to keep them.", L"已填入建议值，点击“保存”后生效。"},
    {L"Take out of folder", L"移出文件夹"},
    {L"Take the apps out", L"解散文件夹"},
    {L"Text color", L"文字颜色"},
    {L"Text size in DIP. Display scaling is applied automatically.", L"使用设备无关像素 DIP，自动适应显示缩放。"},
    {L"The backup helper did not return a result.", L"备份助手未返回结果。"},
    {L"The kind of blur behind the grid.", L"选择启动器的背景模糊方式。"},
    {L"The layout file is empty, unreadable, or uses a newer version.", L"布局文件为空、无法读取，或使用了较新的版本。"},
    {L"The monitor with the mouse", L"鼠标所在显示器"},
    {L"The original Windhawk mod", L"原始 Windhawk 模块"},
    {L"The pill of power buttons at the bottom of the grid.", L"在启动器底部显示电源操作按钮。"},
    {L"The target is missing. Keep it so you can repair it later?", L"目标路径不存在，是否保留入口以便稍后修复？"},
    {L"The taskbar Start button, the Standalone Dock's, and every other way Start opens that is not the keyboard.", L"适用于任务栏、独立 Dock 的开始按钮及其他非键盘打开方式。"},
    {L"Theme", L"主题"},
    {L"Theme mode", L"主题模式"},
    {L"Tray icon", L"显示托盘图标"},
    {L"Typography", L"字体"},
    {L"Ungrouped", L"未分组"},
    {L"Unpin from this grid", L"取消固定"},
    {L"Upstream source on GitHub", L"上游项目源码（GitHub）"},
    {L"Use GPU composition for smoother animation. Turn off for the compatibility renderer.", L"使用 GPU 合成呈现以改善动画流畅度；关闭后使用兼容渲染路径。"},
    {L"Use a color like #89B4FA.", L"请输入类似 #89B4FA 的颜色。"},
    {L"Use immediate transitions. Windows animation accessibility settings are also respected.", L"立即完成过渡，同时遵循 Windows 的动画辅助功能设置。"},
    {L"Use the Windows title bar and window buttons for this settings window. Applies when you save.", L"为设置窗口使用 Windows 原生标题栏和窗口按钮。保存后生效。"},
    {L"Version ", L"版本 "},
    {L"Version, credits, support and links.", L"版本、致谢、支持和项目链接。"},
    {L"Vertical icon spacing", L"图标垂直间距"},
    {L"Vertical scrolling", L"上下滚动"},
    {L"What \"Run the program below\" starts. A command line such as \"C:\\Program Files\\App\\app.exe\" --flag, or a shortcut, file, folder or web address", L"选择“运行下方程序”时执行的命令。可填写带参数的程序路径、快捷方式、文件、文件夹或网址。"},
    {L"Where the grid opens. Monitors 2 to 4 are the other monitors, counted left to right. If the picked monitor is not connected, it opens where the mouse is.", L"选择启动器显示的位置。显示器 2 至 4 按从左到右排列；所选显示器未连接时，使用鼠标所在的显示器。"},
    {L"Where the grid opens. The list shows the monitors connected right now, main first, then the others left to right. If the picked monitor is not connected, it opens where the mouse is.", L"选择启动器显示的位置。列表实时显示当前连接的显示器：主显示器在前，其余按从左到右排列；所选显示器未连接时，使用鼠标所在的显示器。"},
    {L"Where the power dock sits along the bottom edge.", L"选择电源栏在屏幕底部的对齐位置。"},
    {L"While you drag an icon, all icons wiggle like on iOS.", L"拖动图标时，其他图标轻微晃动以提示编辑状态。"},
    {L"Windhawk didn't answer in 15 seconds. Settings may not be saved.", L"Windhawk 在 15 秒内未响应，设置可能未保存。"},
    {L"Windhawk didn't save the settings (code %lu)", L"Windhawk 未保存设置（代码 %lu）"},
    {L"Windows acrylic (DWM)", L"Windows 亚克力（DWM）"},
    {L"Windows blur (DWM)", L"Windows 模糊（DWM）"},
    {L"Windows key", L"Windows 键"},
    {L"With pinned apps, search still finds every app. Right-click an app to pin or unpin it.", L"仅显示固定应用时仍可搜索全部应用。右键可固定或取消固定。"},
    {L"Working directory", L"工作目录"},
    {L"iOS icon fly-in", L"iOS 图标飞入"},

};
inline void Select(std::wstring_view language) {
    chinese.store(language == L"zh-CN" ||
        (language != L"en" && PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_CHINESE),
        std::memory_order_relaxed);
}
inline PCWSTR Tr(PCWSTR english) {
    if (!english || !chinese.load(std::memory_order_relaxed)) return english;
    std::wstring_view key(english);
    auto p = std::lower_bound(std::begin(resources), std::end(resources), key,
        [](const Resource& r, std::wstring_view k) { return r.english < k; });
    return p != std::end(resources) && p->english == key ? p->zhCN : english;
}
inline std::wstring Text(const std::wstring& english) { return Tr(english.c_str()); }
inline PCWSTR Font() { return chinese.load(std::memory_order_relaxed) ? L"Microsoft YaHei UI" : L"Segoe UI Variable Text"; }
inline PCWSTR Locale() { return chinese.load(std::memory_order_relaxed) ? L"zh-CN" : L"en-US"; }
inline std::wstring IconFileFilter() {
    std::wstring result;
    for (PCWSTR part : {Tr(L"Pictures and icons"), L"*.ico;*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.exe", Tr(L"All files"), L"*.*"}) {
        result += part;
        result += L'\0';
    }
    result += L'\0';
    return result;
}
} // namespace launchpad_i18n

#include <windows.h>
#include <d3d11.h>
#include <d2d1_1.h>
#include <dxgi1_3.h>
#include <dcomp.h>
#include <wrl/client.h>

// Flip-model GPU composition. No GPU->CPU readback or layered-window bitmap
// upload is used in this path. The original presenter remains a fallback.
class LaunchpadPresenter {
    template<class T> using Ptr = Microsoft::WRL::ComPtr<T>;
    Ptr<IDXGISwapChain1> chain;
    Ptr<IDCompositionDevice> device;
    Ptr<IDCompositionTarget> target;
    Ptr<IDCompositionVisual> visual;
    Ptr<IDCompositionEffectGroup> opacityEffect;
    UINT width = 0, height = 0;
    RECT bounds{};
public:
    bool failed = false;
    bool Active() const { return chain != nullptr; }
    bool Statistics(DXGI_FRAME_STATISTICS& statistics) const {
        return chain && SUCCEEDED(chain->GetFrameStatistics(&statistics));
    }
    void Reset() {
        opacityEffect.Reset(); visual.Reset(); target.Reset(); device.Reset(); chain.Reset();
        width = height = 0;
        bounds = {};
        failed = false;
    }
    static void LegacyStyle(HWND hwnd) {
        auto style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (!(style & WS_EX_LAYERED) || (style & WS_EX_NOREDIRECTIONBITMAP))
            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, (style | WS_EX_LAYERED) & ~WS_EX_NOREDIRECTIONBITMAP);
    }
    bool Ensure(HWND hwnd, ID3D11Device* d3d, UINT w, UINT h) {
        if (failed) return false;
        if (chain && width == w && height == h) return true;
        Reset();
        auto fail = [&] { Reset(); failed = true; LegacyStyle(hwnd); return false; };
        Ptr<IDXGIDevice> dxgi;
        Ptr<IDXGIAdapter> adapter;
        Ptr<IDXGIFactory2> factory;
        if (FAILED(d3d->QueryInterface(IID_PPV_ARGS(&dxgi))) || FAILED(dxgi->GetAdapter(&adapter)) ||
            FAILED(adapter->GetParent(IID_PPV_ARGS(&factory)))) return fail();
        DXGI_SWAP_CHAIN_DESC1 desc{};
        desc.Width = w; desc.Height = h;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        if (FAILED(factory->CreateSwapChainForComposition(d3d, &desc, nullptr, &chain)) ||
            FAILED(DCompositionCreateDevice(dxgi.Get(), __uuidof(IDCompositionDevice), (void**)device.GetAddressOf())) ||
            FAILED(device->CreateTargetForHwnd(hwnd, TRUE, &target)) ||
            FAILED(device->CreateVisual(&visual)) || FAILED(visual->SetContent(chain.Get())) ||
            FAILED(device->CreateEffectGroup(&opacityEffect)) || FAILED(visual->SetEffect(opacityEffect.Get())) ||
            FAILED(target->SetRoot(visual.Get())) || FAILED(device->Commit())) return fail();
        Ptr<IDXGIDevice1> queued;
        if (SUCCEEDED(dxgi.As(&queued))) queued->SetMaximumFrameLatency(1);
        auto style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, (style | WS_EX_NOREDIRECTIONBITMAP) & ~WS_EX_LAYERED);
        width = w; height = h;
        return true;
    }
    bool Present(ID3D11Device* d3d, ID2D1Bitmap1* image, float opacity) {
        if (!chain) return false;
        Ptr<IDXGISurface> surface;
        Ptr<ID3D11Texture2D> source, destination;
        Ptr<ID3D11DeviceContext> context;
        if (FAILED(image->GetSurface(&surface)) || FAILED(surface.As(&source)) ||
            FAILED(chain->GetBuffer(0, IID_PPV_ARGS(&destination)))) return false;
        d3d->GetImmediateContext(&context);
        context->CopyResource(destination.Get(), source.Get());
        if (FAILED(opacityEffect->SetOpacity(opacity)) || FAILED(device->Commit())) return false;
        // DWM composes at the display cadence; the high-resolution scheduler
        // submits one fresh image per monitor period, without blocking input
        // inside a vsync wait on the UI thread.
        return SUCCEEDED(chain->Present(0, 0));
    }
    void Position(HWND hwnd, const RECT& area) {
        if (!EqualRect(&area, &bounds)) {
            SetWindowPos(hwnd, nullptr, area.left, area.top, area.right-area.left, area.bottom-area.top, SWP_NOZORDER | SWP_NOACTIVATE);
            bounds = area;
        }
    }
};

#include <windows.h>
#include <dxgi.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <numeric>
#include <string>
#include <vector>

struct LaunchpadMetrics {
    bool active = false;
    std::string phase;
    double start = 0, last = 0, visibleMs = -1, hz = 60;
    int width = 0, height = 0;
    float scale = 1;
    bool havePresentation = false, presentationInvalid = false, presentationReliable = true;
    DXGI_FRAME_STATISTICS firstPresentation{}, lastPresentation{};
    std::vector<double> intervals, renders;
    void Begin(const char* name, double now, int w, int h, float dpiScale, double refresh) {
        phase = name; start = now; last = 0; visibleMs = -1;
        width = w; height = h; scale = dpiScale; hz = refresh;
        intervals.clear(); renders.clear();
        havePresentation=false; presentationInvalid=false;
        presentationReliable=GetSystemMetrics(SM_CMONITORS)==1;
        intervals.reserve(512); renders.reserve(512);
        active = true;
    }
    void Frame(double now) {
        if (!active) return;
        if (last && intervals.size() < 8192) intervals.push_back((now - last) * 1000);
        last = now;
    }
    void Rendered(double ms) { if (active && renders.size() < 8192) renders.push_back(ms); }
    void Presented(const DXGI_FRAME_STATISTICS& statistics) {
        if (!active || !presentationReliable || statistics.SyncQPCTime.QuadPart<=0) return;
        if (!havePresentation) { firstPresentation=statistics; havePresentation=true; }
        else if (statistics.PresentCount<lastPresentation.PresentCount || statistics.SyncQPCTime.QuadPart<lastPresentation.SyncQPCTime.QuadPart) presentationInvalid=true;
        lastPresentation=statistics;
    }
    std::wstring Finish(const std::wstring& path, double now, bool gpu) {
        if (!active) return {};
        active = false;
        auto mean = [](const auto& v) { return v.empty() ? 0.0 : std::accumulate(v.begin(), v.end(), 0.0) / v.size(); };
        auto p95 = [](auto v) {
            if (v.empty()) return 0.0;
            std::sort(v.begin(), v.end());
            return v[(size_t)std::ceil(v.size() * 0.95) - 1];
        };
        double intervalMean = mean(intervals), renderMean = mean(renders), variance = 0;
        size_t late = 0;
        for (double ms : intervals) { variance += (ms - intervalMean) * (ms - intervalMean); late += ms > 1500 / hz; }
        double jitter = intervals.empty() ? 0 : std::sqrt(variance / intervals.size());
        std::string presented="unavailable";
        if (gpu && havePresentation && !presentationInvalid && lastPresentation.PresentCount>firstPresentation.PresentCount &&
            lastPresentation.SyncQPCTime.QuadPart>firstPresentation.SyncQPCTime.QuadPart) {
            LARGE_INTEGER frequency; QueryPerformanceFrequency(&frequency);
            double fps=(lastPresentation.PresentCount-firstPresentation.PresentCount)*(double)frequency.QuadPart/
                (lastPresentation.SyncQPCTime.QuadPart-firstPresentation.SyncQPCTime.QuadPart);
            char number[64]; std::snprintf(number,sizeof(number),"%.3f",fps); presented=number;
        }
        double duration=(now-start)*1000;
        double animationMs=phase=="open"&&visibleMs>=0?std::max(0.0,duration-visibleMs):duration;
        char line[1024];
        std::snprintf(line, sizeof(line), "%s,%d,%d,%.3f,%.3f,%s,%.3f,%.3f,%zu,%.3f,%.3f,%.3f,%.3f,%.3f,%zu,%s,%.3f\r\n",
            phase.c_str(), width, height, scale, hz, gpu ? "gpu-composition" : "layered-readback", visibleMs,
            (now - start) * 1000, renders.size(), renderMean, p95(renders), intervalMean, jitter,
            intervalMean > 0 ? 1000 / intervalMean : 0, late, presented.c_str(), animationMs);
        // Bound log size. Rotation retains the previous capture for comparison.
        WIN32_FILE_ATTRIBUTE_DATA attributes{};
        if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attributes) &&
            (attributes.nFileSizeHigh || attributes.nFileSizeLow > 4 * 1024 * 1024))
            MoveFileExW(path.c_str(), (path + L".previous.csv").c_str(), MOVEFILE_REPLACE_EXISTING);
        HANDLE file = CreateFileW(path.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file != INVALID_HANDLE_VALUE) {
            DWORD written;
            if (GetFileSize(file, nullptr) == 0) {
                constexpr char header[] = "phase,width,height,scale,target_hz,presenter,first_visible_ms,duration_ms,render_count,render_mean_ms,render_p95_ms,interval_mean_ms,interval_stddev_ms,submitted_fps,late_intervals,dxgi_presented_fps,animation_ms\r\n";
                WriteFile(file, header, sizeof(header) - 1, &written, nullptr);
            }
            WriteFile(file, line, (DWORD)strlen(line), &written, nullptr);
            CloseHandle(file);
        }
        std::string message = "performance: "; message += line;
        return std::wstring(message.begin(), message.end());
    }
};

#include <windows.h>
#include <mutex>
#include <string>
#include <vector>

namespace launchpad_settings_store {
// Private per-mod settings, usable on Windhawk 1.x without a CLI or elevation.
// Each override carries the host value it replaced. A subsequent edit of that
// same setting in Windhawk takes precedence and invalidates the old override.
inline std::recursive_mutex mutex;
struct Change { std::wstring key, value, host; };
inline std::wstring Encode(const std::wstring& value) {
    constexpr WCHAR hex[] = L"0123456789ABCDEF";
    std::wstring out; out.reserve(value.size() * 4);
    for (WCHAR c : value) for (int shift : {12,8,4,0}) out += hex[((unsigned)c >> shift) & 15];
    return out;
}
inline bool Decode(const std::wstring& value, std::wstring& out) {
    if (value.size() % 4) return false;
    out.clear();
    unsigned code = 0;
    for (size_t i=0; i<value.size(); i++) {
        WCHAR c=value[i];
        unsigned digit=c>=L'0'&&c<=L'9' ? c-L'0' : c>=L'A'&&c<=L'F' ? c-L'A'+10 : 99;
        if (digit>15) return false;
        code=(code<<4)|digit;
        if (i%4==3) { out+=(WCHAR)code; code=0; }
    }
    return true;
}
inline std::wstring Read(const std::wstring& path, PCWSTR section, PCWSTR key) {
    std::wstring result(1024,L'\0');
    while (result.size() <= 1024*1024) {
        DWORD n=GetPrivateProfileStringW(section,key,L"",result.data(),(DWORD)result.size(),path.c_str());
        if (n+1<result.size()) { result.resize(n); return result; }
        result.resize(result.size()*2);
    }
    return {};
}
inline std::wstring Resolve(const std::wstring& path, PCWSTR key, const std::wstring& host) {
    std::lock_guard lock(mutex);
    if (GetPrivateProfileIntW(L"meta",L"version",1,path.c_str())>1 || Read(path,L"present",key)!=L"1") return host;
    std::wstring baseline,value;
    if (!Decode(Read(path,L"host",key),baseline) || !Decode(Read(path,L"values",key),value)) return host;
    if (baseline!=host) {
        WritePrivateProfileStringW(L"present",key,nullptr,path.c_str());
        return host;
    }
    return value;
}
// Used only when inheriting another mod's settings. Never invalidate or write
// to its file, even when a host value has changed since its private override.
inline std::wstring ResolveReadOnly(const std::wstring& path, PCWSTR key, const std::wstring& host) {
    std::lock_guard lock(mutex);
    if (GetPrivateProfileIntW(L"meta",L"version",1,path.c_str()) != 1 || Read(path,L"present",key)!=L"1") return host;
    std::wstring baseline, value;
    return Decode(Read(path,L"host",key),baseline) && Decode(Read(path,L"values",key),value) && baseline==host ? value : host;
}
inline bool Save(const std::wstring& path, const std::vector<Change>& changes) {
    std::lock_guard lock(mutex);
    if (GetPrivateProfileIntW(L"meta",L"version",1,path.c_str())>1) return false;
    std::wstring temporary=path+L".tmp";
    bool existing=GetFileAttributesW(path.c_str())!=INVALID_FILE_ATTRIBUTES;
    if (existing) { if (!CopyFileW(path.c_str(),temporary.c_str(),FALSE)) return false; }
    else {
        HANDLE file=CreateFileW(temporary.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
        if (file==INVALID_HANDLE_VALUE) return false;
        WORD bom=0xFEFF; DWORD written; bool ok=WriteFile(file,&bom,2,&written,nullptr)&&written==2;
        CloseHandle(file); if (!ok) return false;
    }
    bool ok=WritePrivateProfileStringW(L"meta",L"version",L"1",temporary.c_str())!=FALSE;
    for (const auto& change : changes) {
        ok=WritePrivateProfileStringW(L"host",change.key.c_str(),Encode(change.host).c_str(),temporary.c_str())&&ok;
        ok=WritePrivateProfileStringW(L"values",change.key.c_str(),Encode(change.value).c_str(),temporary.c_str())&&ok;
        ok=WritePrivateProfileStringW(L"present",change.key.c_str(),L"1",temporary.c_str())&&ok;
    }
    WritePrivateProfileStringW(nullptr,nullptr,nullptr,temporary.c_str());
    if (existing && !CopyFileW(path.c_str(),(path+L".bak").c_str(),FALSE)) ok=false;
    if (ok) ok=MoveFileExW(temporary.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=FALSE;
    if (!ok) DeleteFileW(temporary.c_str());
    // Discard the profile cache after an atomic file replacement.
    WritePrivateProfileStringW(nullptr,nullptr,nullptr,path.c_str());
    return ok;
}
} // namespace launchpad_settings_store


#include <algorithm>
#include <cmath>

namespace launchpad_fly_in {

inline float SmoothStep(float value) {
    float t = std::clamp(value, 0.f, 1.f);
    return t * t * (3.f - 2.f * t);
}

// A critically damped unit-mass step, with zero initial velocity.
// Blend out the small residual in the tail so the finite timeline settles
// at exactly one with zero velocity and can be played back in either direction.
inline float Spring(float time) {
    float t = std::clamp(time, 0.f, 1.f);
    float x = 7.5f * t;  // Natural angular frequency times local duration.
    float residual = (1.f + x) * std::exp(-x);
    return 1.f - residual * (1.f - SmoothStep((t - .72f) / .28f));
}

struct Pose {
    float dx, dy, scale, opacity;
};

inline Pose Sample(float phase, float x, float y, float width, float height,
                   float margin, float delayFraction) {
    if (phase >= 1.f) return {0.f, 0.f, 1.f, 1.f};

    // Intersect the outward ray with a rectangle padded by the tile extent.
    // Normalize each axis separately to preserve the same effect on tall and
    // wide displays. The convergence point sits near the viewport centre.
    float originX = width * .5f, originY = height * .48f;
    float rayX = x - originX, rayY = y - originY;
    float radius = std::max(std::abs(rayX) / originX,
                            std::abs(rayY) / (rayY < 0 ? originY : height - originY));
    if (radius < .001f) rayY = -1.f;
    float edgeX = rayX < 0 ? originX + margin : width - originX + margin;
    float edgeY = rayY < 0 ? originY + margin : height - originY + margin;
    float reach = 1.f / std::max(std::abs(rayX) / edgeX, std::abs(rayY) / edgeY);
    float delay = delayFraction * std::clamp(radius, 0.f, 1.f);
    float settled = Spring((phase - delay) / (1.f - delay));
    float remaining = 1.f - settled;
    return {
        (originX + rayX * reach - x) * remaining,
        (originY + rayY * reach - y) * remaining,
        1.f + std::clamp(reach - 1.f, .50f, 1.55f) * remaining,
        SmoothStep(settled / .52f),
    };
}

// Analytically integrate the playback speed. When retargeted mid-flight, keep
// the previous speed and blend toward the new one with a 12 ms time constant.
// Position, scale and opacity continue to be sampled from the same timeline.
inline float Advance(float phase, double& velocity, float target, double elapsed,
                     int durationMs) {
    if (phase == target) {
        velocity = 0;
        return target;
    }
    constexpr double response = .012;
    double speed = (target == 1.f ? 1000.0 : -1000.0) / durationMs;
    if (velocity == 0 && (phase == 0.f || phase == 1.f)) velocity = speed;
    double time = std::max(0.0, elapsed);
    double blend = -std::expm1(-time / response);
    double next = phase + speed * time + (velocity - speed) * response * blend;
    velocity += (speed - velocity) * blend;
    constexpr double endpointTolerance = 1e-7;
    if (next <= endpointTolerance || next >= 1.0 - endpointTolerance) {
        velocity = 0;
        return next < .5 ? 0.f : 1.f;
    }
    return static_cast<float>(next);
}

}  // namespace launchpad_fly_in


#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include <filesystem>
#include <functional>
#include <fstream>
#include <stdexcept>

namespace ryjs_data {
namespace fs = std::filesystem;
inline constexpr int kStorageVersion = 1;

inline bool ReadUtf8(const std::wstring& path, std::wstring& text) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER length;
    bool ok = GetFileSizeEx(file, &length) && length.QuadPart <= (8 << 20) && length.QuadPart >= 0;
    std::string bytes(ok ? (size_t)length.QuadPart : 0, '\0');
    DWORD read = 0;
    ok = ok && ReadFile(file, bytes.data(), (DWORD)bytes.size(), &read, nullptr) && read == bytes.size();
    CloseHandle(file);
    if (!ok) return false;
    int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(), (int)bytes.size(), nullptr, 0);
    if (!bytes.empty() && !count) return false;
    text.resize(count);
    if (count) MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, bytes.data(), (int)bytes.size(), text.data(), count);
    if (!text.empty() && text.front() == 0xFEFF) text.erase(0, 1);
    return text.find(L'\0') == std::wstring::npos;
}

inline bool Validate(const std::wstring& name, const std::wstring& text) {
    bool layout = name.starts_with(L"layout-") || name.starts_with(L"groups-v1-");
    bool groups = name.starts_with(L"groups-v1-");
    if (!layout && name != L"pins.txt" && name != L"icons.txt") return true;
    // An empty legacy layout is a valid empty list: unpinning the last app
    // writes one, and refusing it used to make the whole mod load disabled.
    // Group layouts always carry their schema header, so an empty one is bad.
    if (layout && text.empty()) return !groups;
    size_t pos = 0, rows = 0;
    bool folder = false;
    while (pos < text.size()) {
        size_t end = text.find(L'\n', pos);
        if (end == std::wstring::npos) end = text.size();
        auto line = text.substr(pos, end - pos); pos = end + 1;
        if (!line.empty() && line.back() == L'\r') line.pop_back();
        if (line.empty()) continue;
        std::vector<std::wstring> f;
        size_t begin = 0;
        for (;;) {
            auto tab = line.find(L'\t', begin);
            f.push_back(line.substr(begin, tab == std::wstring::npos ? tab : tab - begin));
            if (tab == std::wstring::npos) break;
            begin = tab + 1;
        }
        if (f.size() < 2 || f[1].empty()) return false;
        if (f[0] == L"schema") {
            if (!groups || rows || f.size() != 3 || f[1] != L"1" || f[2] != L"groups") return false;
        } else if (groups && !rows) return false;
        else if (layout && f[0] == L"folder") {
            if (f.size() != 2 && !(f.size() == 3 && f[2] == L"inbox")) return false;
            folder = true;
        } else if (layout && f[0] == L"in") {
            if (!folder || f.size() != 2) return false;
        } else if (layout && (f[0] == L"app" || f[0] == L"hidden")) {
            if (f.size() != 2) return false;
            if (f[0] == L"app") folder = false;
        } else if (name == L"pins.txt" && f[0] == L"pin" && f.size() == 2) {
        } else if (name == L"icons.txt" && f[0] == L"icon" && f.size() == 3 && !f[2].empty()) {
        } else return false;
        ++rows;
    }
    return !layout || !groups || rows > 0;
}

inline bool ValidateDirectory(const std::wstring& directory, const std::wstring& skip = {}) {
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            auto name = entry.path().filename().wstring();
            if (name == skip) continue;
            if (name.starts_with(L"groups-") && !name.starts_with(L"groups-v1-")) return false;
            if (entry.is_regular_file() && entry.path().extension() == L".txt") {
                std::wstring text;
                if (!ReadUtf8(entry.path().wstring(), text) || !Validate(name, text)) return false;
            }
        }
        return true;
    } catch (...) { return false; }
}

// data-v1 is committed by a single directory rename. A partial copy or a
// failed settings conversion remains in .pending and can never look like a
// complete migration on the next start. Source files are only read/copied.
inline bool Prepare(const std::wstring& source, const std::wstring& target,
                    const std::function<bool(const std::wstring&)>& settings) {
    try {
        if (fs::exists(target)) {
            auto marker = target + L"\\ryjs-storage.ini";
            return GetPrivateProfileIntW(L"storage", L"schema", 0, marker.c_str()) == kStorageVersion &&
                   ValidateDirectory(target);
        }
        auto stage = target + L".pending";
        if (fs::exists(stage)) {
            // Explorer was killed, or a copy failed, during an earlier first
            // run. The user's real data is still in the old folder, so drop
            // the partial copy and start over instead of refusing to load.
            std::error_code error;
            fs::remove_all(stage, error);
            if (error || fs::exists(stage)) return false;
        }
        if (!source.empty() && !ValidateDirectory(source)) return false;
        fs::create_directories(stage);
        if (!source.empty()) {
            for (const auto& entry : fs::recursive_directory_iterator(source)) {
                // Never follow junctions/symlinks or copy ephemeral writer files.
                if (entry.is_symlink() || (GetFileAttributesW(entry.path().c_str()) & FILE_ATTRIBUTE_REPARSE_POINT)) return false;
                auto relative = fs::relative(entry.path(), source);
                auto destination = fs::path(stage) / relative;
                if (entry.is_directory()) { fs::create_directories(destination); continue; }
                if (!entry.is_regular_file() || entry.path().extension() == L".tmp" ||
                    entry.path().filename().wstring().starts_with(L"performance")) continue;
                if (!CopyFileW(entry.path().c_str(), destination.c_str(), TRUE)) return false;
                // Compare source/destination bytes after copying; any racing
                // change aborts before committing the new data directory.
                auto readBytes = [](const fs::path& path) {
                    std::ifstream file(path, std::ios::binary);
                    if (!file) throw std::runtime_error("unreadable migration file");
                    return std::string(std::istreambuf_iterator<char>(file), {});
                };
                if (readBytes(entry.path()) != readBytes(destination)) return false;
            }
        }
        if (!settings(stage) || !ValidateDirectory(stage)) return false;
        auto marker = stage + L"\\ryjs-storage.ini";
        if (!WritePrivateProfileStringW(L"storage", L"schema", L"1", marker.c_str()) ||
            !WritePrivateProfileStringW(L"storage", L"source", source.c_str(), marker.c_str())) return false;
        WritePrivateProfileStringW(nullptr, nullptr, nullptr, marker.c_str());
        return MoveFileExW(stage.c_str(), target.c_str(), MOVEFILE_WRITE_THROUGH) != FALSE;
    } catch (...) { return false; }
}
} // namespace ryjs_data


using Microsoft::WRL::ComPtr;

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

using launchpad_i18n::Tr;

#ifdef SEAMLESS_PREVIEW
constexpr WCHAR kWindowClass[] = L"RYJSLaunchpadPreview";
constexpr WCHAR kBackdropClass[] = L"RYJSLaunchpadPreviewBackdrop";
#else
constexpr WCHAR kWindowClass[] = L"WindhawkRYJSLaunchpad";
constexpr WCHAR kBackdropClass[] = L"WindhawkRYJSLaunchpadBackdrop";
#endif
constexpr WCHAR kAppsFolder[] = L"shell:AppsFolder\\";

// From the Standalone Dock mod: the edge strip it takes.
constexpr WCHAR kDockClass[] = L"WindhawkMacDock";
constexpr WCHAR kDockPlateClass[] = L"WindhawkMacDockPlate";
constexpr WCHAR kDockEdgeProp[] = L"WindhawkMacDock.Edge";
constexpr WCHAR kDockReservedProp[] = L"WindhawkMacDock.Reserved";

constexpr UINT WM_APP_TOGGLE = WM_APP + 1;
constexpr UINT WM_APP_APPS = WM_APP + 2;  // lParam: new AppList*.
constexpr UINT WM_APP_RELOAD = WM_APP + 3;
constexpr UINT WM_APP_QUIT = WM_APP + 4;
constexpr UINT WM_APP_RUN = WM_APP + 5;
constexpr UINT WM_APP_SHIFT_WIN = WM_APP + 6;
// A lone Windows key tap, which the grid answers by closing (see KeyboardHook:
// once the mod answered Start's first show, Windows never asks again, so the
// second tap has to be caught at the key itself).
constexpr UINT WM_APP_WIN_TAP = WM_APP + 7;

constexpr UINT_PTR kTimerFrame = 1;
constexpr UINT_PTR kTimerFirstLoad = 4;
constexpr UINT_PTR kTimerShiftWin = 5;
constexpr UINT_PTR kTimerCaret = 6;  // The search caret's blink.
constexpr UINT_PTR kTimerPowerArm = 7;  // An unconfirmed power button resets.

constexpr GUID kBhidEnumItems = {
    0x94f60519,
    0x2850,
    0x4924,
    {0xaa, 0x5a, 0xd1, 0x5e, 0x84, 0x86, 0x80, 0x39}};

double Now() {
    static LARGE_INTEGER frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}

std::wstring ToLower(std::wstring text) {
    if (!text.empty()) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, text.data(),
                      (int)text.size(), text.data(), (int)text.size(),
                      nullptr, nullptr, 0);
    }
    return text;
}

bool EndsWith(const std::wstring& text, const std::wstring& suffix) {
    return text.size() >= suffix.size() &&
           text.compare(text.size() - suffix.size(), suffix.size(), suffix) ==
               0;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class BlurMode { Custom, Acrylic, Blur, None };
enum class AppSource { AllApps, Desktop, Both, Pinned };
enum class KeyAction { Grid, Start, Program };
enum class DockAlign { Left, Center, Right };

struct Settings {
    std::wstring font;
    std::wstring theme=L"custom";
    float backgroundDim=0;
    bool gpuPresenter = true, performanceLogging = false, reducedMotion = false;
    bool iconFlyIn = true;
    int openMs = 280, closeMs = 180, folderMs = 180, frameRateLimit = 0;
    std::wstring easing = L"settle";
    bool groups = false, groupRows = false;
    int groupWidth = 0, groupGap = 32, horizontalGap = 56, verticalGap = 16;
    float labelSize = 13;
    BlurMode blurMode = BlurMode::Custom;
    int blurAmount = 60;
    float opacity = 0.3f;
    D2D1_COLOR_F tint{0, 0, 0, 1};
    int iconSize = 56;
    int columns = 0, globalColumns = 0;
    bool pages = false;
    bool coverScreen = false;
    AppSource source = AppSource::AllApps;
    int monitor = 0;  // 0: the one with the mouse; 1: main; 2+: the others, left to right.
    std::wstring program;
    bool powerBar = true;
    bool powerShown[6] = {true, true, true, true, true, true};  // kPower order.
    bool powerNoConfirm = false;
    // 3.0.0 look and feel. Defaults here; schema and parsing in LoadSettings.
    float glowStrength = 0.6f;    // 0..1, hover/focus glow alpha.
    float chromeOpacity = 0.9f;   // 0..1, search bar and dock fill (Mocha base).
    DockAlign dockAlign = DockAlign::Center;
    bool wiggle = true;           // Edit-mode wiggle while dragging.
    bool animations = true;       // Off: every transition snaps.
    bool trayIcon = true;         // A tray icon that opens the settings.
    int holdMs = 420;             // Press time before an icon lifts for dragging.
    bool showLabels = true;
    bool normalizeIcons = true;   // Scale small/padded icons up to the grid size, no frame.
};

enum class PowerAction { Lock, SignOut, Sleep, Hibernate, Restart, ShutDown };

struct PowerButton {
    PCWSTR key;     // Under powerButtons. in the settings.
    PCWSTR label;
    PCWSTR verb;    // "Click again to <verb>".
    WCHAR glyph;    // Segoe Fluent Icons.
    bool danger;    // Requires confirmation before activation.
};

constexpr PowerButton kPower[] = {
    {L"lock", L"Lock", L"lock", 0xE72E, false},
    {L"signOut", L"Sign out", L"sign out", 0xF3B1, true},
    {L"sleep", L"Sleep", L"sleep", 0xE708, false},
    {L"hibernate", L"Hibernate", L"hibernate", 0xE823, false},
    {L"restart", L"Restart", L"restart", 0xE72C, true},
    {L"shutDown", L"Shut down", L"shut down", 0xE7E8, true},
};

Settings g_settings;
std::atomic<bool> g_systemAnimations{true};
std::atomic<bool> g_requestedAnimations{true};

void RefreshSystemAnimations() {
    BOOL enabled = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0);
    g_systemAnimations = enabled != FALSE;
}

// The mod's own switch plus the Windows "Animation effects" setting.
bool Animate() {
    return g_requestedAnimations.load() && g_systemAnimations.load();
}
// Also read in the ShowStartView hook, on Explorer's threads.
std::atomic<KeyAction> g_winKeyAction{KeyAction::Grid};
std::atomic<KeyAction> g_shiftWinKeyAction{KeyAction::Start};
std::atomic<KeyAction> g_startButtonAction{KeyAction::Grid};

std::wstring DataRoot();
std::wstring ModTempPath();

std::wstring GetStringSetting(PCWSTR name) {
#ifdef SEAMLESS_PREVIEW
    return Preview::Setting(name);
#else
    std::wstring host = WindhawkUtils::StringSetting::make(name).get();
    auto root = DataRoot();
    if (root.empty()) return host;
    return launchpad_settings_store::Resolve(root+L"\\ui-settings.ini",name,host);
#endif
}

// Windhawk stores a number or boolean setting as REG_DWORD, and
// Wh_GetStringSetting converts a DWORD to its decimal string (see
// PortableSettings::RawItemToString in the Windhawk engine). Reading it as a
// string therefore works for both storage shapes and still distinguishes a
// missing key (empty string, use the schema default) from a real zero.
int IntSetting(PCWSTR name, int def) {
    std::wstring value = GetStringSetting(name);
    return value.empty() ? def : _wtoi(value.c_str());
}

D2D1_COLOR_F ParseColor(std::wstring text, D2D1_COLOR_F fallback) {
    text.erase(std::remove_if(text.begin(), text.end(), iswspace), text.end());
    if (!text.empty() && text[0] == L'#') {
        text.erase(0, 1);
    }
    if (text.size() != 6 ||
        !std::all_of(text.begin(), text.end(), iswxdigit)) {
        return fallback;
    }
    UINT rgb = wcstoul(text.c_str(), nullptr, 16);
    return D2D1::ColorF(((rgb >> 16) & 0xFF) / 255.0f,
                        ((rgb >> 8) & 0xFF) / 255.0f, (rgb & 0xFF) / 255.0f);
}

void LoadSettings() {
    launchpad_i18n::Select(GetStringSetting(L"interfaceLanguage"));
    Settings s;
    RefreshSystemAnimations();
    s.gpuPresenter = IntSetting(L"gpuPresenter", 1) != 0;
    s.performanceLogging = IntSetting(L"performanceLogging", 0) != 0;
    s.reducedMotion = IntSetting(L"reducedMotion", 0) != 0;
    s.iconFlyIn = GetStringSetting(L"launchpadAnimation") != L"classic";
    s.openMs = std::clamp(IntSetting(L"openDuration", 280), 60, 1500);
    s.closeMs = std::clamp(IntSetting(L"closeDuration", 180), 60, 1500);
    s.folderMs = std::clamp(IntSetting(L"folderDuration", 180), 60, 1500);
    s.frameRateLimit = std::clamp(IntSetting(L"frameRateLimit", 0), 0, 1000);
    s.easing = GetStringSetting(L"animationEasing");
    if (s.easing.empty()) s.easing = L"settle";
    s.font = GetStringSetting(L"uiFont");
    if (s.font.empty() || s.font == L"auto") s.font = launchpad_i18n::Font();
    std::wstring mode = GetStringSetting(L"blurMode");
    s.blurMode = mode == L"acrylic" ? BlurMode::Acrylic
                 : mode == L"blur"  ? BlurMode::Blur
                 : mode == L"none"  ? BlurMode::None
                                    : BlurMode::Custom;
    s.blurAmount = std::clamp(IntSetting(L"blurAmount", 60), 0, 100);
    s.opacity =
        std::clamp(IntSetting(L"backgroundOpacity", 30), 0, 100) / 100.0f;
    s.tint = ParseColor(GetStringSetting(L"tintColor"), s.tint);
    s.iconSize = std::clamp(IntSetting(L"gridIconSize", 56), 24, 192);
    s.columns = std::clamp(IntSetting(L"gridColumns", 0), 0, 30);
    s.globalColumns=std::clamp(IntSetting(L"globalColumns",0),0,30);
    s.groups = GetStringSetting(L"gridLayout").empty() || GetStringSetting(L"gridLayout") == L"groups";
    s.pages = GetStringSetting(L"gridLayout") == L"pages" || (s.groups && GetStringSetting(L"groupNavigation") == L"pages");
    s.groupRows = GetStringSetting(L"groupFlow") == L"rows";
    s.groupWidth = std::clamp(IntSetting(L"groupWidth", 0), 0, 1600);
    s.groupGap = std::clamp(IntSetting(L"groupSpacing", 32), 8, 160);
    s.horizontalGap = std::clamp(IntSetting(L"horizontalSpacing", 56), 8, 240);
    s.verticalGap = std::clamp(IntSetting(L"verticalSpacing", 16), 0, 180);
    s.labelSize = std::clamp(IntSetting(L"labelFontSize", 13), 9, 32);
    s.coverScreen = IntSetting(L"coverScreen", 0) != 0;
    std::wstring source = GetStringSetting(L"appSource");
    s.source = source == L"desktop"  ? AppSource::Desktop
               : source == L"both"   ? AppSource::Both
               : source == L"pinned" ? AppSource::Pinned
                                     : AppSource::AllApps;
    s.monitor = std::max(0, _wtoi(GetStringSetting(L"monitor").c_str()));
    s.program = GetStringSetting(L"program");
    s.powerBar = IntSetting(L"powerBar", 1) != 0;
    for (int i = 0; i < 6; i++) {
        s.powerShown[i] =
            IntSetting((std::wstring(L"powerButtons.") + kPower[i].key).c_str(), 1) != 0;
    }
    s.powerNoConfirm = IntSetting(L"powerNoConfirm", 0) != 0;
    s.glowStrength =
        std::clamp(IntSetting(L"glowStrength", 60), 0, 100) / 100.0f;
    s.chromeOpacity =
        std::clamp(IntSetting(L"chromeOpacity", 90), 0, 100) / 100.0f;
    std::wstring dock = ToLower(GetStringSetting(L"dockAlignment"));
    s.dockAlign = dock == L"left"    ? DockAlign::Left
                  : dock == L"right" ? DockAlign::Right
                                     : DockAlign::Center;
    s.wiggle = IntSetting(L"wiggle", 1) != 0;
    s.animations = IntSetting(L"animations", 1) != 0;
    s.holdMs = std::clamp(IntSetting(L"holdTime", 420), 150, 1500);
    s.showLabels = IntSetting(L"showLabels", 1) != 0;
    s.normalizeIcons = IntSetting(L"normalizeIcons", 1) != 0;
    s.trayIcon = IntSetting(L"settingsTrayIcon", 1) != 0;
    s.theme=GetStringSetting(L"themeMode");
    s.backgroundDim=std::clamp(IntSetting(L"backgroundDim",0),0,80)/100.0f;
    bool light=s.theme==L"light";
    if(s.theme==L"system") {
        DWORD value=0,size=sizeof(value);
        RegGetValueW(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",L"AppsUseLightTheme",RRF_RT_REG_DWORD,nullptr,&value,&size);
        light=value!=0;
    }
    if(s.theme==L"light"||s.theme==L"dark"||s.theme==L"system") {
        s.tint=D2D1::ColorF(light?0xEEEEEE:0x141414);
        s.opacity=std::max(s.opacity,light?0.55f:0.40f);
    }
    g_settings = s;
    g_requestedAnimations = s.animations && !s.reducedMotion;
    auto action = [](PCWSTR name) {
        std::wstring value = GetStringSetting(name);
        return value == L"start"     ? KeyAction::Start
               : value == L"program" ? KeyAction::Program
                                     : KeyAction::Grid;
    };
    g_winKeyAction = action(L"winKeyAction");
    g_shiftWinKeyAction = action(L"shiftWinKeyAction");
    g_startButtonAction = action(L"startButtonAction");
}

// Runs the "program" setting: a command line, or else anything ShellExecute
// opens (a shortcut, file, folder or web address).
void RunProgram() {
    std::wstring command = g_settings.program;
    command.erase(0, command.find_first_not_of(L" \t"));
    command.erase(command.find_last_not_of(L" \t") + 1);
    if (command.empty()) {
        return;
    }
    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi{};
    std::wstring line = command;
    if (CreateProcessW(nullptr, line.data(), nullptr, nullptr, FALSE, 0,
                       nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return;
    }
    if (command.size() > 1 && command.front() == L'"' &&
        command.back() == L'"') {
        command = command.substr(1, command.size() - 2);
    }
    ShellExecuteW(nullptr, nullptr, command.c_str(), nullptr, nullptr,
                  SW_SHOWNORMAL);
}

////////////////////////////////////////////////////////////////////////////////
// App list
//
// Comes from the virtual folder "shell:AppsFolder" (the "All apps" list of
// Start), the desktop, or Start's pins, depending on the setting. It's read on
// a separate thread, with the icons, and handed ready to the window.

struct App {
    std::wstring name;
    std::wstring originalName, stableKey, layoutTarget, arguments, workingDirectory;
    bool manual = false, unavailable = false;
    std::wstring target;  // shell:AppsFolder\... or a path, opened with ShellExecute.
    bool desktop = false;  // Came from the desktop.
    std::vector<UINT32> pixels;  // Square icon, premultiplied BGRA.
    int iconSize = 0;
    ComPtr<ID2D1Bitmap> bitmap;
    UINT generation = 0;
};
using AppList = std::vector<std::shared_ptr<App>>;

// The globals that hold COM objects (D2D bitmaps, factories, text formats) are
// released by the Start thread before it ends (no_destroy): when Explorer
// exits, Wh_ModUninit isn't called, the thread is already gone, and static
// destructors would release them on the exiting thread under the loader lock.
[[clang::no_destroy]] AppList g_apps;
bool g_appsLoaded;
double g_appsTime;
int g_appsKey;  // AppsKey() of the loaded list.
bool g_appsNormalized = true;  // "Fill small icons" when the list was read.

HANDLE g_worker;
std::atomic<bool> g_workerStop;
std::atomic<bool> g_fullImportBusy=false,g_fullBackupBusy=false;
std::atomic<HWND> g_wnd;

// Bounds of the pixels whose alpha is over a threshold, optionally inside a
// clip box.
struct IconBox {
    int left = 0, top = 0, right = -1, bottom = -1;
    bool empty() const { return right < left || bottom < top; }
    int width() const { return right - left + 1; }
    int height() const { return bottom - top + 1; }
};

static IconBox ContentBounds(const std::vector<UINT32>& pixels, int width,
                             int height, UINT32 threshold,
                             const IconBox& clip) {
    IconBox b;
    b.left = clip.right + 1;
    b.top = clip.bottom + 1;
    for (int y = std::max(0, clip.top); y <= std::min(height - 1, clip.bottom);
         y++) {
        for (int x = std::max(0, clip.left);
             x <= std::min(width - 1, clip.right); x++) {
            if ((pixels[(size_t)y * width + x] >> 24) > threshold) {
                b.left = std::min(b.left, x);
                b.right = std::max(b.right, x);
                b.top = std::min(b.top, y);
                b.bottom = std::max(b.bottom, y);
            }
        }
    }
    return b;
}

// Some icons arrive as a small image centred on a large canvas, often inside
// a thin frame the shell drew: a near-opaque ring of one colour along the
// edge over a faint fill. Strips such a ring, but only when what is left
// inside is really smaller, so full-bleed icons keep their edges.
static void StripIconFrame(const std::vector<UINT32>& pixels, int width,
                           int height, IconBox& box) {
    // A side is a frame line when nearly all of it is opaque in one colour.
    auto frameLine = [&](bool vertical, int pos, int from, int to) {
        int dense = 0, total = 0;
        UINT minR = 255, maxR = 0, minG = 255, maxG = 0, minB = 255, maxB = 0;
        for (int i = from; i <= to; i++) {
            UINT32 p = vertical ? pixels[(size_t)i * width + pos]
                                : pixels[(size_t)pos * width + i];
            total++;
            if ((p >> 24) <= 200) {
                continue;
            }
            dense++;
            UINT r = (p >> 16) & 0xFF, g = (p >> 8) & 0xFF, b = p & 0xFF;
            minR = std::min(minR, r);
            maxR = std::max(maxR, r);
            minG = std::min(minG, g);
            maxG = std::max(maxG, g);
            minB = std::min(minB, b);
            maxB = std::max(maxB, b);
        }
        return dense && dense * 10 >= total * 9 && maxR - minR < 40 &&
               maxG - minG < 40 && maxB - minB < 40;
    };
    int strip[4] = {};  // Left, top, right, bottom.
    // Keep at least one row and one column inside the box: a small, evenly
    // opaque icon otherwise walks a side past the opposite one and reads
    // pixels outside the image.
    auto room = [&] {
        return box.top + strip[1] < box.bottom - strip[3] &&
               box.left + strip[0] < box.right - strip[2];
    };
    for (int step = 0; step < 12 && room(); step++) {
        if (frameLine(false, box.top + strip[1], box.left + strip[0],
                      box.right - strip[2])) {
            strip[1]++;
        }
        if (frameLine(false, box.bottom - strip[3], box.left + strip[0],
                      box.right - strip[2])) {
            strip[3]++;
        }
        if (frameLine(true, box.left + strip[0], box.top + strip[1],
                      box.bottom - strip[3])) {
            strip[0]++;
        }
        if (frameLine(true, box.right - strip[2], box.top + strip[1],
                      box.bottom - strip[3])) {
            strip[2]++;
        }
    }
    if (!strip[0] && !strip[1] && !strip[2] && !strip[3]) {
        return;
    }
    IconBox rest;
    rest.left = box.left + strip[0];
    rest.top = box.top + strip[1];
    rest.right = box.right - strip[2];
    rest.bottom = box.bottom - strip[3];
    if (rest.empty()) {
        return;
    }
    IconBox dense = ContentBounds(pixels, width, height, 200, rest);
    if (dense.empty()) {
        dense = ContentBounds(pixels, width, height, 8, rest);
    }
    if (dense.empty() ||
        std::max(dense.width(), dense.height()) * 4 >=
            std::max(box.width(), box.height()) * 3) {
        return;  // The icon fills the box: the edges were its own.
    }
    box = dense;
}

// Box-average resample of a region, in premultiplied pixels.
static std::vector<UINT32> Resample(const std::vector<UINT32>& pixels,
                                    int width, int height, const IconBox& box,
                                    int outW, int outH) {
    std::vector<UINT32> out((size_t)outW * outH, 0);
    for (int oy = 0; oy < outH; oy++) {
        int sy0 = box.top + (int)((double)oy * box.height() / outH);
        int sy1 = std::max(
            sy0 + 1, box.top + (int)((double)(oy + 1) * box.height() / outH));
        for (int ox = 0; ox < outW; ox++) {
            int sx0 = box.left + (int)((double)ox * box.width() / outW);
            int sx1 = std::max(sx0 + 1, box.left +
                                            (int)((double)(ox + 1) *
                                                  box.width() / outW));
            UINT a = 0, r = 0, g = 0, b = 0, n = 0;
            for (int sy = sy0; sy < std::min(sy1, height); sy++) {
                for (int sx = sx0; sx < std::min(sx1, width); sx++) {
                    UINT p = pixels[(size_t)sy * width + sx];
                    a += p >> 24;
                    r += (p >> 16) & 0xFF;
                    g += (p >> 8) & 0xFF;
                    b += p & 0xFF;
                    n++;
                }
            }
            if (!n) {
                continue;
            }
            out[(size_t)oy * outW + ox] =
                ((a / n) << 24) | ((r / n) << 16) | ((g / n) << 8) | (b / n);
        }
    }
    return out;
}

// Centres a side*side square crop of the source into the app's pixels.
static void CenterCopy(const std::vector<UINT32>& pixels, int width,
                       int height, int cx, int cy, int side, App& app) {
    app.iconSize = side;
    app.pixels.assign((size_t)side * side, 0);
    int x0 = cx - side / 2, y0 = cy - side / 2;
    for (int y = 0; y < side; y++) {
        int sy = y0 + y;
        if (sy < 0 || sy >= height) {
            continue;
        }
        for (int x = 0; x < side; x++) {
            int sx = x0 + x;
            if (sx >= 0 && sx < width) {
                app.pixels[(size_t)y * side + x] = pixels[sy * width + sx];
            }
        }
    }
}

// Normalizes the alpha, then crops the icon to its content. The shell often
// returns a small image centred in a large canvas, sometimes inside a thin
// frame. With "Normalize icons" the content is also scaled up to the icon
// size, aspect kept, so every icon reads at the same size; without it only
// the old small-icon crop applies.
void FinishIcon(std::vector<UINT32> pixels, int width, int height, App& app) {
    if (std::none_of(pixels.begin(), pixels.end(),
                     [](UINT32 p) { return (p >> 24) != 0; })) {
        for (auto& p : pixels) {
            p |= 0xFF000000;
        }
    }
    bool premultiplied =
        std::none_of(pixels.begin(), pixels.end(), [](UINT32 p) {
            UINT a = p >> 24;
            return ((p >> 16) & 0xFF) > a || ((p >> 8) & 0xFF) > a ||
                   (p & 0xFF) > a;
        });
    if (!premultiplied) {
        for (auto& p : pixels) {
            UINT a = p >> 24;
            UINT r = ((p >> 16) & 0xFF) * a / 255;
            UINT g = ((p >> 8) & 0xFF) * a / 255;
            UINT b = (p & 0xFF) * a / 255;
            p = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }

    IconBox box = ContentBounds(pixels, width, height, 8,
                                {0, 0, width - 1, height - 1});
    if (box.empty()) {
        return;
    }
    int full = std::max(width, height);
    if (!g_settings.normalizeIcons) {
        // The old crop: a small icon centred in padding, no resampling.
        int content = std::max(box.width(), box.height());
        if (content < full * 0.6f) {
            CenterCopy(pixels, width, height, (box.left + box.right + 1) / 2,
                       (box.top + box.bottom + 1) / 2,
                       std::min(full, (int)std::ceil(content * 1.08f)), app);
        } else {
            CenterCopy(pixels, width, height, width / 2, height / 2, full,
                       app);
        }
        return;
    }

    // Drop a shell-drawn frame, then find the real icon in what is left: the
    // dense pixels, ignoring the faint fill a frame canvas comes with.
    StripIconFrame(pixels, width, height, box);
    IconBox dense = ContentBounds(pixels, width, height, 200, box);
    if (dense.empty()) {
        dense = box;
    }
    int content = std::max(dense.width(), dense.height());
    if (content * 20 >= full * 17) {
        // Already near full size: centre it, no resampling, no upscaling.
        CenterCopy(pixels, width, height, (dense.left + dense.right + 1) / 2,
                   (dense.top + dense.bottom + 1) / 2, full, app);
        return;
    }
    // Resample the content up to the icon size, aspect kept, centred.
    float k = (float)full / content;
    int outW = std::max(1, (int)std::lround(dense.width() * k));
    int outH = std::max(1, (int)std::lround(dense.height() * k));
    std::vector<UINT32> scaled =
        Resample(pixels, width, height, dense, outW, outH);
    app.iconSize = full;
    app.pixels.assign((size_t)full * full, 0);
    int x0 = (full - outW) / 2, y0 = (full - outH) / 2;
    for (int y = 0; y < outH; y++) {
        for (int x = 0; x < outW; x++) {
            app.pixels[(size_t)(y0 + y) * full + x0 + x] =
                scaled[(size_t)y * outW + x];
        }
    }
}

// The default flags give the item's icon; without SIIGBF_ICONONLY a picture
// gives its own image.
void LoadAppIcon(IShellItem* item,
                 int size,
                 App& app,
                 SIIGBF flags = SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK) {
    ComPtr<IShellItemImageFactory> factory;
    HBITMAP bitmap = nullptr;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&factory))) ||
        FAILED(factory->GetImage(SIZE{size, size}, flags, &bitmap))) {
        return;
    }
    BITMAP bm{};
    if (GetObjectW(bitmap, sizeof(bm), &bm) && bm.bmWidth > 0) {
        int width = bm.bmWidth, height = std::abs(bm.bmHeight);
        std::vector<UINT32> pixels((size_t)width * height);
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        HDC dc = GetDC(nullptr);
        int lines = GetDIBits(dc, bitmap, 0, height, pixels.data(), &bi,
                              DIB_RGB_COLORS);
        ReleaseDC(nullptr, dc);
        if (lines) {
            FinishIcon(std::move(pixels), width, height, app);
        }
    }
    DeleteObject(bitmap);
}

// Uninstallers, documents and website links that some programs put in the
// Start menu.
bool ShouldSkip(const std::wstring& name, const std::wstring& parsing) {
    std::wstring lowerName = ToLower(name);
    std::wstring lowerTarget = ToLower(parsing);
    if (lowerName.empty() ||
        lowerTarget.find(L"\\unins0") != std::wstring::npos ||
        lowerTarget.find(L"\\uninst.exe") != std::wstring::npos ||
        lowerTarget.find(L"\\uninstall.exe") != std::wstring::npos ||
        lowerTarget.rfind(L"http://", 0) == 0 ||
        lowerTarget.rfind(L"https://", 0) == 0) {
        return true;
    }
    for (PCWSTR ext : {L".chm", L".txt", L".pdf", L".rtf", L".htm", L".html"}) {
        if (EndsWith(lowerTarget, ext)) {
            return true;
        }
    }
    return false;
}

bool NameLess(const std::wstring& a, const std::wstring& b) {
    return CompareStringEx(LOCALE_NAME_USER_DEFAULT,
                           LINGUISTIC_IGNORECASE | SORT_DIGITSASNUMBERS,
                           a.c_str(), (int)a.size(), b.c_str(), (int)b.size(),
                           nullptr, nullptr, 0) == CSTR_LESS_THAN;
}

void AddApp(IShellItem* item,
            std::wstring target,
            int iconPixels,
            AppList& list,
            bool desktop = false) {
    PWSTR name = nullptr;
    if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && *name) {
        auto app = std::make_shared<App>();
        app->name = name;
        // With file extensions shown, shortcuts are named "Brave.lnk".
        std::wstring lower = ToLower(app->name);
        if (lower.size() > 4 &&
            (EndsWith(lower, L".lnk") || EndsWith(lower, L".url"))) {
            app->name.resize(app->name.size() - 4);
        }
        app->target = std::move(target);
        app->desktop = desktop;
        LoadAppIcon(item, iconPixels, *app);
        list.push_back(std::move(app));
    }
    CoTaskMemFree(name);
}

void AddStartApps(int iconPixels, AppList& list) {
    ComPtr<IShellItem> folder;
    ComPtr<IEnumShellItems> items;
    if (FAILED(SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr,
                                           IID_PPV_ARGS(&folder))) ||
        FAILED(folder->BindToHandler(nullptr, kBhidEnumItems,
                                     IID_PPV_ARGS(&items)))) {
        return;
    }
    ComPtr<IShellItem> item;
    while (!g_workerStop &&
           items->Next(1, item.ReleaseAndGetAddressOf(), nullptr) == S_OK) {
        PWSTR name = nullptr, parsing = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) &&
            SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING,
                                           &parsing)) &&
            !ShouldSkip(name, parsing)) {
            AddApp(item.Get(), std::wstring(kAppsFolder) + parsing, iconPixels,
                   list);
        }
        CoTaskMemFree(name);
        CoTaskMemFree(parsing);
    }
}

// Everything on the desktop: the user's own and the one shared by all users.
void AddDesktopItems(int iconPixels, AppList& list) {
    for (const KNOWNFOLDERID* id :
         {&FOLDERID_Desktop, &FOLDERID_PublicDesktop}) {
        ComPtr<IShellItem> folder;
        ComPtr<IEnumShellItems> items;
        if (FAILED(SHGetKnownFolderItem(*id, KF_FLAG_DEFAULT, nullptr,
                                        IID_PPV_ARGS(&folder))) ||
            FAILED(folder->BindToHandler(nullptr, kBhidEnumItems,
                                         IID_PPV_ARGS(&items)))) {
            continue;
        }
        ComPtr<IShellItem> item;
        while (!g_workerStop &&
               items->Next(1, item.ReleaseAndGetAddressOf(), nullptr) == S_OK) {
            SFGAOF attributes = 0;
            PWSTR path = nullptr;
            if (SUCCEEDED(item->GetAttributes(SFGAO_HIDDEN, &attributes)) &&
                !(attributes & SFGAO_HIDDEN) &&
                SUCCEEDED(item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING,
                                               &path))) {
                AddApp(item.Get(), path, iconPixels, list, true);
            }
            CoTaskMemFree(path);
        }
    }
}

// Reads the JSON string whose opening quote is json[i]; i ends past the
// closing quote.
std::wstring ReadJsonString(const std::wstring& json, size_t& i) {
    std::wstring out;
    for (i++; i < json.size() && json[i] != L'"'; i++) {
        if (json[i] != L'\\' || i + 1 >= json.size()) {
            out += json[i];
            continue;
        }
        WCHAR c = json[++i];
        switch (c) {
            case L'n':
                out += L'\n';
                break;
            case L't':
                out += L'\t';
                break;
            case L'r':
                out += L'\r';
                break;
            case L'b':
                out += L'\b';
                break;
            case L'f':
                out += L'\f';
                break;
            case L'u':
                if (i + 4 < json.size()) {
                    out += (WCHAR)wcstoul(json.substr(i + 1, 4).c_str(),
                                          nullptr, 16);
                    i += 4;
                }
                break;
            default:  // \" \\ \/
                out += c;
                break;
        }
    }
    i++;
    return out;
}

// Start's pins in their order, as Windows' own Export-StartLayout writes them:
// {"pinnedList":[{"desktopAppLink":"%APPDATA%\\...\\X.lnk"},
// {"packagedAppId":"Microsoft.WindowsCalculator_8wekyb3d8bbwe!App"}]}.
// The file Start keeps them in (start2.bin) is encrypted, so the export is the
// supported way to read them.
std::wstring ExportStartLayout() {
    WCHAR system[MAX_PATH];
    auto directory = ModTempPath();
    if (directory.empty() || !GetSystemDirectoryW(system, MAX_PATH) ||
        (FAILED(SHCreateDirectoryExW(nullptr, directory.c_str(), nullptr)) &&
         GetFileAttributesW(directory.c_str()) == INVALID_FILE_ATTRIBUTES)) {
        return {};
    }
    std::wstring path = directory + L"\\pins-" +
                        std::to_wstring(GetCurrentProcessId()) + L"-" +
                        std::to_wstring(GetCurrentThreadId()) + L".json";
    DeleteFileW(path.c_str());
    std::wstring quoted;  // Inside '...' a quote is written twice.
    for (WCHAR c : path) {
        quoted += c;
        if (c == L'\'') {
            quoted += c;
        }
    }
    std::wstring command = L"\"" + std::wstring(system) +
                           L"\\WindowsPowerShell\\v1.0\\powershell.exe\" "
                           L"-NoProfile -NonInteractive -ExecutionPolicy Bypass "
                           L"-Command \"Export-StartLayout -Path '" +
                           quoted + L"'\"";
    STARTUPINFOW si{sizeof(si)};
    PROCESS_INFORMATION pi{};
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        return {};
    }
    CloseHandle(pi.hThread);
    for (int waited = 0;
         WaitForSingleObject(pi.hProcess, 100) == WAIT_TIMEOUT; waited++) {
        if (g_workerStop || waited > 300) {
            TerminateProcess(pi.hProcess, 1);
            break;
        }
    }
    CloseHandle(pi.hProcess);

    std::string bytes;
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              nullptr, OPEN_EXISTING, 0, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        DWORD size = GetFileSize(file, nullptr), read = 0;
        if (size != INVALID_FILE_SIZE && size < (16 << 20)) {
            bytes.resize(size);
            if (!ReadFile(file, bytes.data(), size, &read, nullptr)) {
                read = 0;
            }
            bytes.resize(read);
        }
        CloseHandle(file);
        DeleteFileW(path.c_str());
    }
    int length = MultiByteToWideChar(CP_UTF8, 0, bytes.data(),
                                     (int)bytes.size(), nullptr, 0);
    std::wstring json(length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                        json.data(), length);
    return json;
}

// False when the pins couldn't be read at all.
bool AddStartPins(int iconPixels, AppList& list) {
    std::wstring json = ExportStartLayout();
    if (json.find(L"\"pinnedList\"") == std::wstring::npos) {
        return false;
    }
    for (size_t i = json.find(L'"'); i != std::wstring::npos && !g_workerStop;
         i = json.find(L'"', i)) {
        std::wstring key = ReadJsonString(json, i);
        size_t value = json.find_first_not_of(L" \t\r\n", i);
        if (value == std::wstring::npos || json[value] != L':') {
            continue;
        }
        value = json.find_first_not_of(L" \t\r\n", value + 1);
        if (value != std::wstring::npos && json[value] == L'{' &&
            key == L"secondaryTile") {
            // A pinned website. Its packagedAppId is the browser, which would
            // show up as a plain browser tile, so the whole object is skipped.
            int depth = 0;
            for (i = value; i < json.size();) {
                if (json[i] == L'"') {
                    ReadJsonString(json, i);
                    continue;
                }
                if (json[i] == L'{') {
                    depth++;
                } else if (json[i] == L'}' && --depth == 0) {
                    i++;
                    break;
                }
                i++;
            }
            continue;
        }
        if (value == std::wstring::npos || json[value] != L'"') {
            continue;
        }
        i = value;
        std::wstring pin = ReadJsonString(json, i);
        std::wstring target;
        if (key == L"desktopAppLink") {
            WCHAR expanded[MAX_PATH * 2];
            DWORD n = ExpandEnvironmentStringsW(pin.c_str(), expanded,
                                                ARRAYSIZE(expanded));
            target = n && n <= ARRAYSIZE(expanded) ? expanded : pin;
        } else if (key == L"desktopAppId" || key == L"packagedAppId") {
            target = std::wstring(kAppsFolder) + pin;
        } else {
            continue;
        }
        ComPtr<IShellItem> item;
        if (SUCCEEDED(SHCreateItemFromParsingName(target.c_str(), nullptr,
                                                  IID_PPV_ARGS(&item)))) {
            AddApp(item.Get(), target, iconPixels, list);
        }
    }
    return true;
}

std::wstring StoragePath(const std::wstring& file);
bool WriteLines(const std::wstring& path,
                const std::vector<std::wstring>& lines,bool backup=true);

// The first time "only the apps you pin" is used, the pins start as Start's
// own, matched by name to the apps already read. Written even when Start's
// pins can't be read, so the slow export runs once.
void SeedPins(int iconPixels, AppList& list) {
    std::wstring path = StoragePath(L"pins.txt");
    if (path.empty() ||
        GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return;
    }
    AppList pins;
    AddStartPins(iconPixels, pins);
    std::vector<std::wstring> lines;
    for (auto& pin : pins) {
        auto same = std::find_if(list.begin(), list.end(), [&](const auto& a) {
            return ToLower(a->name) == ToLower(pin->name);
        });
        lines.push_back(L"pin\t" +
                        (same != list.end() ? (*same)->target : pin->target));
        if (same == list.end()) {
            list.push_back(std::move(pin));
        }
    }
    if (!g_workerStop) {
        WriteLines(path, lines);
    }
}

AppList* EnumerateApps(int iconPixels, AppSource source) {
    auto list = std::make_unique<AppList>();
    // Pinned: every app is read, so the search finds the ones not pinned.
    if (source != AppSource::Desktop) {
        AddStartApps(iconPixels, *list);
    }
    if (source != AppSource::AllApps) {
        AddDesktopItems(iconPixels, *list);
    }
    if (source == AppSource::Pinned) {
        SeedPins(iconPixels, *list);
    }

    std::stable_sort(list->begin(), list->end(),
                     [](const auto& a, const auto& b) {
                         return NameLess(a->name, b->name);
                     });
    // The same app may show up twice (user and all-users shortcut). A desktop
    // shortcut named like a Start app is dropped too: the stable sort keeps
    // the Start app, added first.
    list->erase(std::unique(list->begin(), list->end(),
                            [](const auto& a, const auto& b) {
                                return a->name == b->name &&
                                       (a->desktop != b->desktop ||
                                        ToLower(a->target) ==
                                            ToLower(b->target));
                            }),
                list->end());
    return list.release();
}

// What a loaded list depends on: the icon size and the source.
// Bumped when the icon pipeline changes, so a stale list with old-style
// framed icons can't be reused within a session.
constexpr int kIconPipelineVersion = 3;

int AppsKey(int iconPixels, AppSource source) {
    return iconPixels | ((int)source << 16) | (kIconPipelineVersion << 24);
}

DWORD WINAPI LoadAppsThread(void* param) {
    int key = (int)(INT_PTR)param;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    AppList* list =
        EnumerateApps(key & 0xFFFF, (AppSource)((key >> 16) & 0xFF));
    CoUninitialize();
    HWND wnd = g_wnd;
    if (g_workerStop || !wnd ||
        !PostMessageW(wnd, WM_APP_APPS, key, (LPARAM)list)) {
        delete list;
    }
    return 0;
}

// Icons are read at the size of the monitor with the largest scaling.
int IconPixels() {
    UINT maxDpi = 96;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM param) -> BOOL {
            UINT dpiX = 96, dpiY = 96;
            if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX,
                                           &dpiY))) {
                *(UINT*)param = std::max(*(UINT*)param, dpiX);
            }
            return TRUE;
        },
        (LPARAM)&maxDpi);
    return std::min(256, (int)std::ceil(g_settings.iconSize * maxDpi / 96.0));
}

int AppsKey() {
    return AppsKey(IconPixels(), g_settings.source);
}

void StartLoadingApps() {
    if (g_fullImportBusy) return;
    if (g_worker) {
        if (WaitForSingleObject(g_worker, 0) == WAIT_TIMEOUT) {
            return;  // Already reading.
        }
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    g_appsTime = Now();
    g_worker = CreateThread(nullptr, 0, LoadAppsThread,
                            (void*)(INT_PTR)AppsKey(), 0, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Arrangement
//
// The order of the grid, its folders and the apps removed from it, saved as
// text in the mod's storage folder: one file per "Apps to show" choice, and
// one for the replaced icons. Apps are known by their target, and apps that
// aren't in the file yet go at the end.

struct Folder;

// One icon of the grid: an app or a folder.
struct Tile {
    std::shared_ptr<App> app;
    std::shared_ptr<Folder> folder;
};
using Tiles = std::vector<Tile>;

struct Folder {
    std::wstring name;
    Tiles apps;
    bool inbox = false;
    bool collapsed = false; // Explicit traditional-folder presentation in a free layout.
    std::vector<std::pair<size_t,std::wstring>> unavailable;
};

[[clang::no_destroy]] Tiles g_tiles;
bool g_layoutReadOnly = false;
bool g_dataReadOnly = false;
extern bool g_entriesReadOnly;
bool SaveGroupPositions(bool finishing=false);
void BeforeArrangeGeometry();
void AfterArrangeGeometry();

std::shared_ptr<Folder> InboxGroup() {
    for (const auto& tile : g_tiles) if (tile.folder && tile.folder->inbox) return tile.folder;
    auto group = std::make_shared<Folder>();
    group->name = Tr(L"Ungrouped"); group->inbox = true;
    g_tiles.push_back({nullptr, group});
    return group;
}
void NormalizeGroups() {
    if (!g_settings.groups) return;
    Tiles ungrouped;
    for (const auto& tile : g_tiles) if (tile.app) ungrouped.push_back(tile);
    std::erase_if(g_tiles, [](const Tile& tile) { return tile.app != nullptr; });
    if (!ungrouped.empty() || g_tiles.empty()) {
        auto group = InboxGroup();
        group->apps.insert(group->apps.end(), ungrouped.begin(), ungrouped.end());
    }
}
[[clang::no_destroy]] std::set<std::wstring> g_hidden;  // Removed apps' keys.
[[clang::no_destroy]] std::map<std::wstring, std::wstring> g_customIcons;  // Key: picture.
// Keys of the pinned apps, kept in pins.txt. Only with "only the apps you pin".
[[clang::no_destroy]] std::set<std::wstring> g_pins;

bool PinnedOnly() {
    return ((g_appsKey >> 16) & 0xFF) == (int)AppSource::Pinned;
}

// Not on the grid: removed, or not pinned when only pinned apps show.
bool OffGrid(const std::wstring& key) {
    return g_hidden.count(key) || (PinnedOnly() && !g_pins.count(key));
}

std::wstring AppKey(const App& app) {
    return ToLower(app.stableKey.empty() ? app.target : app.stableKey);
}
const std::wstring& LayoutAppKey(const App& app) {
    return app.layoutTarget.empty()?app.target:app.layoutTarget;
}

// The Windhawk namespace is independent; versioned data is committed only
// after a complete, validated copy of the selected previous mod.
std::wstring DataRoot() {
    WCHAR path[32768];
    if (!Wh_GetModStoragePath(path, ARRAYSIZE(path))) return {};
#ifdef SEAMLESS_PREVIEW
    return path;
#else
    return std::wstring(path) + L"\\data-v1";
#endif
}

// Scratch files live next to data-v1, never inside it: a backup worker whose
// working directory sat inside the data folder would lock it against import,
// and  clearing the mod's storage should take them with it.
std::wstring ModTempPath() {
    auto root = DataRoot();
    if (root.empty()) return {};
    return (std::filesystem::path(root).parent_path() / L"tmp").wstring();
}

#ifndef SEAMLESS_PREVIEW
bool PrepareStorage() {
    auto target = DataRoot();
    if (target.empty()) return false;
    if (std::filesystem::exists(target))
        return ryjs_data::Prepare(L"", target, [](const auto&) { return true; });
    // First run: inherit the layout, groups, pins and icons of a predecessor.
    // Those files are the data a user would miss. Its settings are not copied:
    // reading them means reading Windhawk's private per-mod registry layout,
    // which is not API, does not exist on portable Windhawk, and is a layout
    // the user can review and retype in the settings window anyway.
    auto parent = std::filesystem::path(target).parent_path().parent_path();
    std::wstring source;
    for (PCWSTR id : {L"local@seamless-launchpad-custom", L"seamless-launchpad-custom",
                      L"local@seamless-launchpad", L"seamless-launchpad",
                      L"local@fullscreen-start-plus", L"fullscreen-start-plus"}) {
        auto candidate = parent / id;
        if (!std::filesystem::is_directory(candidate)) continue;
        for (const auto& file : std::filesystem::directory_iterator(candidate)) {
            auto name = file.path().filename().wstring();
            if (name.starts_with(L"layout-") || name.starts_with(L"groups-")) {
                source = candidate.wstring(); break;
            }
        }
        if (!source.empty()) break;
    }
    bool copied = ryjs_data::Prepare(source, target, [](const std::wstring& stage) {
        // Keep the predecessor's settings file with the copied data instead of
        // applying it: nothing is lost, and no settings are inherited silently.
        auto original = stage + L"\\ui-settings.ini";
        if (GetFileAttributesW(original.c_str()) == INVALID_FILE_ATTRIBUTES) return true;
        return MoveFileW(original.c_str(),
                         (stage + L"\\migration-original-ui-settings.ini").c_str()) != FALSE;
    });
    if (copied) { Wh_Log(L"RYJS data ready, original storage untouched: %s", source.c_str()); }
    else { Wh_Log(L"RYJS data preparation failed; originals kept: %s", source.c_str()); }
    return copied;
}
#endif

std::wstring StoragePath(const std::wstring& file) {
    auto directory = DataRoot();
    if (directory.empty()) return {};
    SHCreateDirectoryExW(nullptr, directory.c_str(), nullptr);
    return directory + L"\\" + file;
}

std::wstring LegacyLayoutFile() {
    static constexpr PCWSTR names[] = {L"allApps", L"desktop", L"both",
                                       L"pinned"};
    return std::wstring(L"layout-") + names[std::clamp((g_appsKey >> 16) & 0xFF, 0, 3)] +
           L".txt";
}

std::wstring LayoutFile() {
    auto file = LegacyLayoutFile();
    if (g_settings.groups) file.replace(0, 7, L"groups-v1-");
    return file;
}

// Tab-separated fields of each line of a UTF-8 file.
std::vector<std::vector<std::wstring>> ReadLines(const std::wstring& path, bool* success = nullptr) {
    std::vector<std::vector<std::wstring>> lines;
    std::wstring text;
    bool ok = ryjs_data::ReadUtf8(path, text);
    if (success) *success = ok;
    if (!ok) return lines;
    std::vector<std::wstring> fields(1);
    for (WCHAR c : text + L"\n") {
        if (c == L'\n') {
            if (fields.size() > 1 || !fields[0].empty()) {
                lines.push_back(std::move(fields));
            }
            fields.assign(1, {});
        } else if (c == L'\t') {
            fields.emplace_back();
        } else if (c != L'\r') {
            fields.back() += c;
        }
    }
    return lines;
}

// Written whole to a temporary file first, so a crash can't leave half a file.
bool WriteLines(const std::wstring& path,
                const std::vector<std::wstring>& lines,bool backup) {
    if (path.empty() || g_dataReadOnly) {
        return false;
    }
    std::wstring text;
    for (const auto& line : lines) {
        text += line + L"\r\n";
    }
    int length = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(),
                                     nullptr, 0, nullptr, nullptr);
    std::string bytes(length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(),
                        bytes.data(), length, nullptr, nullptr);
    std::wstring temp = path + L".tmp";
    HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Can't write %s: %u", temp.c_str(), GetLastError());
        return false;
    }
    DWORD written = 0;
    bool ok = WriteFile(file, bytes.data(), (DWORD)bytes.size(), &written,
                        nullptr) &&
              written == bytes.size();
    CloseHandle(file);
    if (ok && backup && GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES && !CopyFileW(path.c_str(), (path + L".bak").c_str(), FALSE)) {
        Wh_Log(L"Cannot back up %s: %u; keeping previous data", path.c_str(), GetLastError());
        DeleteFileW(temp.c_str()); return false;
    }
    if (!ok || !MoveFileExW(temp.c_str(), path.c_str(),
                            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        Wh_Log(L"Can't save %s: %u", path.c_str(), GetLastError());
        DeleteFileW(temp.c_str());
        return false;
    }
    return true;
}

// The arrangement as the lines of its file:
// hidden<TAB>target, app<TAB>target, folder<TAB>name, in<TAB>target (an app
// of the folder above).
std::vector<std::wstring> ArrangementLines() {
    std::vector<std::wstring> lines;
    if (g_settings.groups) lines.push_back(L"schema\t1\tgroups");
    for (const auto& key : g_hidden) {
        lines.push_back(L"hidden\t" + key);
    }
    for (const auto& tile : g_tiles) {
        if (tile.app) {
            lines.push_back(L"app\t" + LayoutAppKey(*tile.app));
            continue;
        }
        lines.push_back(L"folder\t" + (tile.folder->name.empty()
                                            ? std::wstring(Tr(L"Folder"))
                                            : tile.folder->name) + (tile.folder->inbox ? L"\tinbox" : L""));
        size_t appIndex=0, missingIndex=0, position=0;
        const auto& apps=tile.folder->apps;
        const auto& missing=tile.folder->unavailable;
        while (appIndex<apps.size() || missingIndex<missing.size()) {
            if (missingIndex<missing.size() && (appIndex>=apps.size() || missing[missingIndex].first<=position))
                lines.push_back(L"in\t"+missing[missingIndex++].second);
            else lines.push_back(L"in\t"+LayoutAppKey(*apps[appIndex++].app));
            ++position;
        }
    }
    return lines;
}

// Empty folders go, and a folder with one app becomes that app.
void TidyTiles() {
    if (g_settings.groups) return; // Empty and single-app groups are intentional.
    Tiles tidy;
    for (auto& tile : g_tiles) {
        if (tile.folder && tile.folder->apps.size() < 2) {
            if (!tile.folder->apps.empty()) {
                tidy.push_back(tile.folder->apps[0]);
            }
        } else {
            tidy.push_back(std::move(tile));
        }
    }
    g_tiles.swap(tidy);
}

void ArrangeTiles(const std::vector<std::wstring>& lines,bool appendNew=false) {
    BeforeArrangeGeometry();
    std::vector<std::vector<std::wstring>> parsed;
    for (const auto& line : lines) {
        std::vector<std::wstring> fields(1);
        for (WCHAR c : line) {
            if (c == L'\t') {
                fields.emplace_back();
            } else {
                fields.back() += c;
            }
        }
        parsed.push_back(std::move(fields));
    }
    g_hidden.clear();
    for (const auto& f : parsed) {
        if (f.size() >= 2 && f[0] == L"hidden") {
            g_hidden.insert(ToLower(f[1]));
        }
    }
    std::map<std::wstring, std::shared_ptr<App>> byKey;
    for (const auto& app : g_apps) {
        byKey.emplace(AppKey(*app), app);
    }
    std::set<std::wstring> placed;
    auto take = [&](const std::wstring& target) -> std::shared_ptr<App> {
        std::wstring key = ToLower(target);
        auto it = byKey.find(key);
        if (it == byKey.end() || OffGrid(key) || !placed.insert(key).second) {
            return nullptr;
        }
        it->second->layoutTarget=target;
        return it->second;
    };
    g_tiles.clear();
    std::shared_ptr<Folder> folder;
    size_t folderPosition=0;
    for (const auto& f : parsed) {
        if (f.size() < 2) {
            continue;
        }
        if (f[0] == L"folder") {
            folder = std::make_shared<Folder>();
            folderPosition=0;
            folder->name = f[1];
            folder->inbox = f.size() >= 3 && f[2] == L"inbox";
            g_tiles.push_back({nullptr, folder});
        } else if (f[0] == L"in" && folder) {
            if (auto app = take(f[1])) {
                folder->apps.push_back({std::move(app), nullptr});
            } else if (g_settings.groups && !byKey.count(ToLower(f[1])) && !OffGrid(ToLower(f[1])) && placed.insert(ToLower(f[1])).second) {
                folder->unavailable.emplace_back(folderPosition,f[1]);
            }
            ++folderPosition;
        } else if (f[0] == L"app") {
            folder = nullptr;
            if (auto app = take(f[1])) {
                g_tiles.push_back({std::move(app), nullptr});
            }
        }
    }
    for (const auto& app : g_apps) {
        if(app->manual)continue; // Only explicit addition/pinning chooses a manual entry's place.
        if(!appendNew&&!lines.empty())continue; // Discovery never inserts into an existing arrangement.
        if (auto taken = take(AppKey(*app))) {
            g_tiles.push_back({std::move(taken), nullptr});
        }
    }
    TidyTiles();
    NormalizeGroups();
    AfterArrangeGeometry();
}

bool SaveTiles(bool explicitImport = false) {
    if (g_fullImportBusy) return false;
    if (g_entriesReadOnly) return false;
    if (g_layoutReadOnly || g_dataReadOnly) { Wh_Log(L"Layout is read-only; keeping original file unchanged"); return false; }
    auto path = StoragePath(LayoutFile());
    if (!explicitImport && GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES) {
        std::wstring existing;
        if (!ryjs_data::ReadUtf8(path, existing) || !ryjs_data::Validate(LayoutFile(), existing)) {
            g_layoutReadOnly = g_dataReadOnly = true;
            Wh_Log(L"Layout changed to an unreadable/unsupported file; refusing to overwrite it.");
            return false;
        }
    }
    return WriteLines(path, ArrangementLines()) && SaveGroupPositions();
}

// Rasterize supported SVG icons once on load; no readback in the frame loop.
// https://learn.microsoft.com/windows/win32/direct2d/svg-support
bool LoadSvgIcon(const std::wstring& path,int size,App& app) {
    std::wstring text;
    if(!ryjs_data::ReadUtf8(path,text)||text.find(L"<svg")==std::wstring::npos)return false;
    ComPtr<IStream> stream;
    if(FAILED(SHCreateStreamOnFileEx(path.c_str(),STGM_READ|STGM_SHARE_DENY_WRITE,0,FALSE,nullptr,&stream)))return false;
    ComPtr<ID3D11Device> d3d;
    if(FAILED(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,D3D11_CREATE_DEVICE_BGRA_SUPPORT,nullptr,0,D3D11_SDK_VERSION,&d3d,nullptr,nullptr))&&
       FAILED(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_WARP,nullptr,D3D11_CREATE_DEVICE_BGRA_SUPPORT,nullptr,0,D3D11_SDK_VERSION,&d3d,nullptr,nullptr)))return false;
    ComPtr<IDXGIDevice> dxgi;ComPtr<ID2D1Factory1> factory;ComPtr<ID2D1Device> device;
    ComPtr<ID2D1DeviceContext> context;ComPtr<ID2D1DeviceContext5> svgContext;
    if(FAILED(d3d.As(&dxgi))||FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,IID_PPV_ARGS(&factory)))||
        FAILED(factory->CreateDevice(dxgi.Get(),&device))||FAILED(device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,&context))||FAILED(context.As(&svgContext)))return false;
    UINT side=(UINT)std::clamp(size,16,512);
    ComPtr<ID2D1SvgDocument> svg;ComPtr<ID2D1Bitmap1> target,readback;
    auto properties=D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED));
    if(FAILED(svgContext->CreateSvgDocument(stream.Get(),D2D1::SizeF((float)side,(float)side),&svg))||
        FAILED(context->CreateBitmap(D2D1::SizeU(side,side),nullptr,0,properties,&target)))return false;
    context->SetTarget(target.Get());context->BeginDraw();context->Clear(D2D1::ColorF(0,0));svgContext->DrawSvgDocument(svg.Get());
    if(FAILED(context->EndDraw()))return false;
    properties.bitmapOptions=D2D1_BITMAP_OPTIONS_CPU_READ|D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    if(FAILED(context->CreateBitmap(D2D1::SizeU(side,side),nullptr,0,properties,&readback))||FAILED(readback->CopyFromBitmap(nullptr,target.Get(),nullptr)))return false;
    D2D1_MAPPED_RECT mapped{};if(FAILED(readback->Map(D2D1_MAP_OPTIONS_READ,&mapped)))return false;
    app.pixels.resize((size_t)side*side);app.iconSize=side;
    for(UINT y=0;y<side;y++)std::copy_n((UINT32*)(mapped.bits+y*mapped.pitch),side,app.pixels.data()+y*side);
    readback->Unmap();app.bitmap.Reset();app.generation=0;
    return std::any_of(app.pixels.begin(),app.pixels.end(),[](UINT32 p){return (p>>24)!=0;});
}


// A picture (or the icon of a file) in place of an app's icon.
bool LoadIconFile(const std::wstring& path, int size, App& app) {
    ComPtr<IWICImagingFactory> factory;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICBitmapScaler> scaler;
    ComPtr<IWICFormatConverter> converter;
    if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&factory))) &&
        SUCCEEDED(factory->CreateDecoderFromFilename(path.c_str(),nullptr,GENERIC_READ,WICDecodeMetadataCacheOnLoad,&decoder)) &&
        SUCCEEDED(decoder->GetFrame(0,&frame))) {
        UINT frames=0;decoder->GetFrameCount(&frames);unsigned best=UINT_MAX;
        for(UINT i=0;i<std::min(frames,64u);i++) {
            ComPtr<IWICBitmapFrameDecode> candidate;UINT cw=0,ch=0;
            if(FAILED(decoder->GetFrame(i,&candidate))||FAILED(candidate->GetSize(&cw,&ch)))continue;
            unsigned edge=std::min(cw,ch),score=(unsigned)std::abs((int)edge-size)+(edge<(UINT)size?10000:0);
            if(score<best){best=score;frame=candidate;}
        }
        UINT w=0,h=0;frame->GetSize(&w,&h);
        UINT side=(UINT)std::clamp(size,16,512);
        UINT dw=w>=h?side:std::max(1u,(UINT)((unsigned long long)side*w/std::max(1u,h)));
        UINT dh=h>=w?side:std::max(1u,(UINT)((unsigned long long)side*h/std::max(1u,w)));
        if (w && h && w<=16384 && h<=16384 && SUCCEEDED(factory->CreateBitmapScaler(&scaler)) &&
            SUCCEEDED(scaler->Initialize(frame.Get(),dw,dh,WICBitmapInterpolationModeFant)) &&
            SUCCEEDED(factory->CreateFormatConverter(&converter)) &&
            SUCCEEDED(converter->Initialize(scaler.Get(),GUID_WICPixelFormat32bppPBGRA,WICBitmapDitherTypeNone,nullptr,0,WICBitmapPaletteTypeCustom))) {
            std::vector<UINT32> pixels((size_t)dw*dh);
            if (SUCCEEDED(converter->CopyPixels(nullptr,dw*4,(UINT)pixels.size()*4,(BYTE*)pixels.data()))) {
                app.pixels.assign((size_t)side*side,0);app.iconSize=side;
                for(UINT y=0;y<dh;y++)std::copy_n(pixels.data()+y*dw,dw,app.pixels.data()+(y+(side-dh)/2)*side+(side-dw)/2);
                app.bitmap.Reset();app.generation=0;return true;
            }
        }
    }
    if(LoadSvgIcon(path,size,app))return true;
    ComPtr<IShellItem> item;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                           IID_PPV_ARGS(&item)))) {
        return false;
    }
    App loaded;
    LoadAppIcon(item.Get(), size, loaded, SIIGBF_BIGGERSIZEOK);
    if (loaded.pixels.empty()) {
        return false;
    }
    app.pixels = std::move(loaded.pixels);
    app.iconSize = loaded.iconSize;
    app.bitmap.Reset();
    app.generation = 0;  // Remade on the next draw.
    return true;
}

// The app's own icon again.
void ReloadAppIcon(int size, App& app) {
    ComPtr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(app.target.c_str(), nullptr,
                                              IID_PPV_ARGS(&item)))) {
        App loaded;
        LoadAppIcon(item.Get(), size, loaded);
        app.pixels = std::move(loaded.pixels);
        app.iconSize = loaded.iconSize;
        app.bitmap.Reset();
        app.generation = 0;
    }
}

void SaveCustomIcons() {
    std::vector<std::wstring> lines;
    for (const auto& [key, path] : g_customIcons) {
        lines.push_back(L"icon\t" + key + L"\t" + path);
    }
    WriteLines(StoragePath(L"icons.txt"), lines);
}

void ApplyCustomIcons(int size) {
    g_customIcons.clear();
    for (const auto& f : ReadLines(StoragePath(L"icons.txt"))) {
        if (f.size() >= 3 && f[0] == L"icon") {
            g_customIcons[ToLower(f[1])] = f[2];
        }
    }
    for (const auto& app : g_apps) {
        auto it = g_customIcons.find(AppKey(*app));
        if (it != g_customIcons.end()) {
            LoadIconFile(it->second, size, *app);
        }
    }
}

// Stable identities keep edits independent of automatic discovery and layout order.
struct EntryRecord {
    bool manual=false;
    std::wstring target,name,arguments,directory;
};
[[clang::no_destroy]] std::map<std::wstring,EntryRecord> g_entries;
bool g_entriesReadOnly=false;

std::wstring EntryEncode(const std::wstring& value) {
    std::wstring result;
    for (auto c:value) {
        if (c==L'%') result+=L"%25";
        else if (c==L'\t') result+=L"%09";
        else if (c==L'\n') result+=L"%0A";
        else if (c==L'\r') result+=L"%0D";
        else result+=c;
    }
    return result;
}
bool EntryDecode(const std::wstring& value,std::wstring& result) {
    result.clear();
    for (size_t i=0;i<value.size();i++) {
        if (value[i]!=L'%') { result+=value[i]; continue; }
        auto escape=value.substr(i,3);
        if (escape==L"%25") result+=L'%';
        else if (escape==L"%09") result+=L'\t';
        else if (escape==L"%0A") result+=L'\n';
        else if (escape==L"%0D") result+=L'\r';
        else return false;
        i+=2;
    }
    return true;
}
bool SaveEntries() {
    if (g_entriesReadOnly) return false;
    std::vector<std::wstring> lines{L"schema\t1\tentries"};
    for (const auto& [key,e]:g_entries) lines.push_back(L"entry\t"+EntryEncode(key)+L"\t"+(e.manual?L"1":L"0")+
        L"\t"+EntryEncode(e.target)+L"\t"+EntryEncode(e.name)+L"\t"+EntryEncode(e.arguments)+L"\t"+EntryEncode(e.directory));
    return WriteLines(StoragePath(L"entries-v1.txt"),lines);
}
bool LoadEntries() {
    auto path=StoragePath(L"entries-v1.txt");
    bool read=false;
    auto rows=ReadLines(path,&read);
    bool missing=GetFileAttributesW(path.c_str())==INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND;
    if (missing) { g_entries.clear(); g_entriesReadOnly=false; return true; }
    std::map<std::wstring,EntryRecord> parsed;
    bool valid=read && !rows.empty() && rows[0]==std::vector<std::wstring>{L"schema",L"1",L"entries"};
    for (size_t i=1;valid && i<rows.size();i++) {
        auto& r=rows[i]; std::wstring key; EntryRecord e;
        valid=r.size()==7 && r[0]==L"entry" && (r[2]==L"0"||r[2]==L"1");
        if (!valid) break;
        e.manual=r[2]==L"1";
        valid=EntryDecode(r[1],key) && EntryDecode(r[3],e.target) && EntryDecode(r[4],e.name) &&
            EntryDecode(r[5],e.arguments) && EntryDecode(r[6],e.directory) && !key.empty() && !e.target.empty() && !e.name.empty();
        if (valid) valid=parsed.emplace(key,std::move(e)).second;
    }
    g_entriesReadOnly=!valid;
    if (valid) g_entries=std::move(parsed);
    else Wh_Log(L"Entry data is unreadable or unsupported; keeping it read-only.");
    return valid;
}
bool IsWebEntry(const std::wstring& target) {
    auto lower=ToLower(target);
    return lower.starts_with(L"https://") || lower.starts_with(L"http://");
}
bool IsEntryMissing(const App& app) {
    if (IsWebEntry(app.target) || ToLower(app.target).starts_with(L"shell:")) return false;
    return GetFileAttributesW(app.target.c_str())==INVALID_FILE_ATTRIBUTES;
}
std::wstring WebIconPath(const std::wstring& key) {
    unsigned long long hash=14695981039346656037ull;
    for (WCHAR c:key) { hash^=c;hash*=1099511628211ull; }
    WCHAR name[40]{};swprintf_s(name,L"web-icons\\%016llx.ico",hash);
    return StoragePath(name);
}
void ApplyEntryRecord(App& app,const std::wstring& key,const EntryRecord& e,int size) {
    if (app.originalName.empty()) app.originalName=app.name;
    if(app.layoutTarget.empty())app.layoutTarget=app.target.empty()?key:app.target;
    app.stableKey=key; app.manual=e.manual; app.name=e.name;
    app.target=e.target; app.arguments=e.arguments; app.workingDirectory=e.directory;
    app.unavailable=IsEntryMissing(app);
    ReloadAppIcon(size,app);
    if (IsWebEntry(app.target)) {
        auto icon=WebIconPath(key+L"|"+app.target);
        LoadIconFile(icon,size,app);
    }
}
void ApplyEntries(int size) {
    if (!LoadEntries()) return;
    for (auto& app:g_apps) app->originalName=app->name;
    for (const auto& [key,e]:g_entries) {
        auto found=std::find_if(g_apps.begin(),g_apps.end(),[&](const auto& a) {return AppKey(*a)==key;});
        if (found!=g_apps.end()) ApplyEntryRecord(**found,key,e,size);
        else {
            // Keep a modified automatic app too, so a missing target can be repaired.
            auto app=std::make_shared<App>(); app->name=e.name;
            ApplyEntryRecord(*app,key,e,size); g_apps.push_back(app);
        }
    }
}

// Launchpad-only names, keyed by AppKey, never by the visible label. No shell,
// shortcut or registry write is part of this feature.
struct DisplayNameRecord {std::wstring original,custom;};
[[clang::no_destroy]] std::map<std::wstring,DisplayNameRecord> g_displayNames;
bool g_displayNamesReadOnly=false;
bool LoadDisplayNames() {
    auto path=StoragePath(L"display-names-v1.txt");bool read=false;auto rows=ReadLines(path,&read);
    if(GetFileAttributesW(path.c_str())==INVALID_FILE_ATTRIBUTES&&GetLastError()==ERROR_FILE_NOT_FOUND) {
        g_displayNames.clear();g_displayNamesReadOnly=false;return true;
    }
    bool valid=read&&!rows.empty()&&rows[0]==std::vector<std::wstring>{L"schema",L"1",L"display-names"};
    std::map<std::wstring,DisplayNameRecord> parsed;
    for(size_t i=1;valid&&i<rows.size();i++) {
        const auto& r=rows[i];std::wstring key;DisplayNameRecord record;
        valid=r.size()==4&&r[0]==L"name"&&EntryDecode(r[1],key)&&EntryDecode(r[2],record.original)&&
            EntryDecode(r[3],record.custom)&&!key.empty()&&!record.original.empty();
        if(valid)valid=parsed.emplace(key,record).second;
    }
    g_displayNamesReadOnly=!valid;if(valid)g_displayNames=std::move(parsed);
    return valid;
}
bool SaveDisplayNames() {
    if(g_displayNamesReadOnly||g_dataReadOnly)return false;
    std::vector<std::wstring> lines{L"schema\t1\tdisplay-names"};
    for(const auto& [key,r]:g_displayNames)lines.push_back(L"name\t"+EntryEncode(key)+L"\t"+EntryEncode(r.original)+L"\t"+EntryEncode(r.custom));
    return WriteLines(StoragePath(L"display-names-v1.txt"),lines);
}
void ApplyDisplayName(App& app) {
    auto found=g_displayNames.find(AppKey(app));if(found==g_displayNames.end())return;
    // Preserve the source name separately, including for missing/manual entries.
    if(app.manual||app.originalName.empty()||(app.unavailable&&app.originalName==app.name))app.originalName=found->second.original;
    app.name=found->second.custom.empty()?app.originalName:found->second.custom;
}
void ApplyDisplayNames() {
    if(!LoadDisplayNames())return;
    for(auto& app:g_apps)ApplyDisplayName(*app);
}
bool SetDisplayName(const std::shared_ptr<App>& app,const std::wstring& name) {
    if(!app||g_displayNamesReadOnly||g_dataReadOnly)return false;
    if(!name.empty()&&(name.size()>160||name.find_first_not_of(L" ")==std::wstring::npos||
        std::any_of(name.begin(),name.end(),[](WCHAR c){return c<32;})))return false;
    auto key=AppKey(*app);auto before=g_displayNames;
    std::wstring original=app->originalName.empty()?app->name:app->originalName;
    auto found=g_displayNames.find(key);
    if(found!=g_displayNames.end()&&(app->manual||app->originalName.empty()))original=found->second.original;
    g_displayNames[key]={original,name};
    if(!SaveDisplayNames()){g_displayNames=std::move(before);return false;}
    app->originalName=original;app->name=name.empty()?original:name;
    return true;
}

// Optional geometry only. Legacy membership/order files retain their format.
struct GroupPosition {
    float x=0,y=0,w=400,h=240; // Legacy DIP rectangle, also a compatibility copy for grid records.
    int columns=0,rows=0;   // Legacy whole-cell counts.
    int gridX=0,gridY=0,gridW=0,gridH=0;
    int style=0; // 0: legacy, 1: exact DIPs, 2: grid-sized icon body, 3: automatic columns in exact DIPs, 4: previous whole-cell bounds, 5: square-grid bounds.
    bool snapped=false;
    bool SquareCells() const { return style == 5; }
    bool WholeCells() const { return style == 4; }
    bool OnGrid() const { return gridW > 0; }
};
[[clang::no_destroy]] std::map<std::shared_ptr<Folder>,GroupPosition> g_groupPositions;
[[clang::no_destroy]] std::map<std::shared_ptr<Folder>,int> g_groupRows;
int g_squareGridColumns=0,g_groupEditPreviousColumns=0;
bool g_wholeCellLayout=false,g_groupEditWasWholeCell=false;
bool g_freeGroupLayout=false,g_groupEditMode=false,g_groupPositionsReadOnly=false;
[[clang::no_destroy]] std::map<std::shared_ptr<Folder>,GroupPosition> g_groupEditSnapshot;
[[clang::no_destroy]] std::map<std::shared_ptr<Folder>,int> g_groupEditRows;
bool g_groupEditWasFree=false,g_groupEditDirty=false;
struct GroupEditNode {
    std::shared_ptr<Folder> group;
    GroupPosition position;
    bool fixed=false; // An untouched automatic group split across pages.
};
struct GroupVisualTransition {
    GroupPosition from,to;
    double started=0;
};
using GroupTransitions = std::map<std::shared_ptr<Folder>,GroupVisualTransition>;
[[clang::no_destroy]] GroupTransitions g_groupTransitions;
struct GroupGeometryGesture {
    std::shared_ptr<Folder> group;
    GroupPosition original,draft,continuous;
    std::vector<GroupEditNode> baseline,solution;
    D2D1_POINT_2F grab{},handle{};
    bool solved=false;
    POINT start{},at{};
    double edgeSince=0;
    int edge=0,activeStart=0;
    float lastScroll=0;
    bool resize=false,valid=false,moved=false;
};
[[clang::no_destroy]] GroupGeometryGesture g_groupGesture;
[[clang::no_destroy]] std::shared_ptr<Folder> g_groupResizeHover;
float g_groupResizeGlow=0;

struct ArrangementGeometry {
    size_t ordinal;std::wstring name;GroupPosition position;
    int row;bool collapsed;
};
[[clang::no_destroy]] std::vector<ArrangementGeometry> g_arrangementGeometry;
void BeforeArrangeGeometry() {
    g_arrangementGeometry.clear();
    if(!g_freeGroupLayout&&!g_groupEditMode)return;
    for(size_t i=0;i<g_tiles.size();i++)if(auto group=g_tiles[i].folder) {
        auto found=g_groupPositions.find(group);
        if(found!=g_groupPositions.end())g_arrangementGeometry.push_back({i,group->name,found->second,g_groupRows[group],group->collapsed});
    }
}
void AfterArrangeGeometry() {
    if(!g_freeGroupLayout&&!g_groupEditMode)return;
    g_groupPositions.clear();g_groupRows.clear();
    for(const auto& saved:g_arrangementGeometry)if(saved.ordinal<g_tiles.size()) {
        auto group=g_tiles[saved.ordinal].folder;
        if(group&&group->name==saved.name) {
            g_groupPositions[group]=saved.position;
            g_groupRows[group]=saved.row;
            group->collapsed=saved.collapsed;
        }
    }
    g_arrangementGeometry.clear();
}
std::wstring GroupPositionFile(){return StoragePath(L"group-geometry-"+LegacyLayoutFile());}
void ClearGroupPositions() {
    g_groupPositions.clear();
    g_groupRows.clear();
    g_groupGesture={};g_groupTransitions.clear();g_groupResizeHover.reset();g_groupResizeGlow=0;
    g_freeGroupLayout=false;g_wholeCellLayout=false;g_squareGridColumns=0;
    g_groupEditMode=false;
    g_groupEditSnapshot.clear();g_groupEditRows.clear();g_groupEditDirty=false;
    g_groupPositionsReadOnly=false;
}
bool PositionNumber(const std::wstring& text,float& value) {
    WCHAR* end=nullptr;value=wcstof(text.c_str(),&end);
    return !text.empty()&&end&&!*end&&std::isfinite(value)&&value>=0&&value<=1000000;
}
bool LoadGroupPositions(const std::wstring& backup={}) {
    auto path=backup.empty()?GroupPositionFile():backup;
    bool read=false;auto rows=ReadLines(path,&read);
    if(backup.empty()&&GetFileAttributesW(path.c_str())==INVALID_FILE_ATTRIBUTES&&GetLastError()==ERROR_FILE_NOT_FOUND)return true;
    float columns=0;
    bool v6=read&&!rows.empty()&&rows[0].size()==4&&rows[0][0]==L"schema"&&rows[0][1]==L"6"&&
        rows[0][2]==L"group-geometry"&&PositionNumber(rows[0][3],columns)&&
        columns==std::floor(columns)&&columns>=1&&columns<=30;
    bool v5=v6||(read&&!rows.empty()&&rows[0]==std::vector<std::wstring>{L"schema",L"5",L"group-geometry"});
    bool v4=v5||(read&&!rows.empty()&&rows[0]==std::vector<std::wstring>{L"schema",L"4",L"group-geometry"});
    bool v3=read&&!rows.empty()&&rows[0]==std::vector<std::wstring>{L"schema",L"3",L"group-geometry"};
    bool v2=read&&!rows.empty()&&rows[0]==std::vector<std::wstring>{L"schema",L"2",L"group-geometry"};
    bool valid=v4||v3||v2||(read&&!rows.empty()&&rows[0]==std::vector<std::wstring>{L"schema",L"1",L"group-geometry"});
    std::map<std::shared_ptr<Folder>,GroupPosition> parsed;
    std::map<std::shared_ptr<Folder>,bool> collapsed;
    for(size_t i=1;valid&&i<rows.size();i++) {
        auto& r=rows[i];GroupPosition p;std::wstring name;float ordinal=0;
        valid=r.size()==(v4?18:v3?14:v2?10:8)&&r[0]==L"group"&&(r[7]==L"group"||r[7]==L"folder")&&PositionNumber(r[1],ordinal)&&ordinal==std::floor(ordinal)&&ordinal<g_tiles.size()&&
            EntryDecode(r[2],name)&&PositionNumber(r[3],p.x)&&PositionNumber(r[4],p.y)&&PositionNumber(r[5],p.w)&&PositionNumber(r[6],p.h)&&(v6?(p.w>0&&p.h>0):(p.w>=48&&p.h>=60));
        if(valid&&(v2||v3||v4)) {
            float columns=0,lines=0;
            valid=PositionNumber(r[8],columns)&&PositionNumber(r[9],lines)&&columns==std::floor(columns)&&lines==std::floor(lines)&&
                columns<=10000&&lines<=10000&&((columns==0&&lines==0)||(columns>=1&&lines>=1));
            p.columns=(int)columns;p.rows=(int)lines;
        }
        if(valid&&(v3||v4)) {
            int* fields[]{&p.gridX,&p.gridY,&p.gridW,&p.gridH};
            for(int j=0;j<4&&valid;j++) {
                float value=0;
                valid=PositionNumber(r[10+j],value)&&value==std::floor(value);
                if(valid)*fields[j]=(int)value;
            }
            if(v4&&valid) {
                float oldWidth=0,oldHeight=0;
                valid=(r[14]==L"0"||r[14]==L"1"||r[14]==L"2"||r[14]==L"3"||(v5&&r[14]==L"4")||(v6&&r[14]==L"5"))&&(r[15]==L"0"||r[15]==L"1")&&
                    PositionNumber(r[16],oldWidth)&&PositionNumber(r[17],oldHeight)&&
                    ((!oldWidth&&!oldHeight)||(oldWidth>=48&&oldHeight>=60));
                if(valid){p.style=r[14][0]-L'0';p.snapped=r[15]==L"1";}
            }
            valid=valid&&((p.gridW>0)==(p.gridH>0));
            if(p.SquareCells())valid=valid&&p.snapped&&p.gridW>=2&&p.gridW%2==0&&p.gridH>=6&&p.gridH%4==2&&
                p.gridX%(2*(int)columns)+p.gridW<=2*(int)columns;
            else if(p.WholeCells())valid=valid&&p.snapped&&p.gridW>=1&&p.gridH>=2;
            else if(p.style==0)valid=valid&&((p.OnGrid()&&!p.columns)||
                (!p.gridW&&(p.snapped||(!p.gridX&&!p.gridY))));
            else valid=valid&&(p.OnGrid()==(p.style==2))&&(p.snapped||(!p.gridX&&!p.gridY));
        }
        if(!valid)break;
        auto group=g_tiles[(size_t)ordinal].folder;
        valid=group&&group->name==name&&parsed.emplace(group,p).second;
        if(valid)collapsed[group]=r[7]==L"folder";
    }
    if(valid&&!v3&&!v4)for(const auto& tile:g_tiles)if(tile.folder&&!parsed.count(tile.folder))valid=false;
    if(!backup.empty()) {
        if(!valid)return false;
        std::vector<std::wstring> lines;
        for(const auto& fields:rows) {
            std::wstring line;
            for(size_t i=0;i<fields.size();++i){if(i)line+=L'\t';line+=fields[i];}
            lines.push_back(std::move(line));
        }
        if(!WriteLines(GroupPositionFile(),lines,false))return false;
    }
    g_groupPositionsReadOnly=!valid;
    if(valid){
        g_groupPositions=std::move(parsed);
        for(const auto& [group,value]:collapsed)group->collapsed=value;
        g_freeGroupLayout=true;
        g_wholeCellLayout=v5;
        g_squareGridColumns=v6?(int)columns:0;
    }
    else {g_layoutReadOnly=true;Wh_Log(L"Group geometry cannot be matched safely; layout writes disabled.");}
    return valid;
}
bool SaveGroupPositions(bool finishing) {
    if(g_groupPositionsReadOnly||g_layoutReadOnly||g_dataReadOnly)return false;
    if(!g_freeGroupLayout&&!finishing)return true;
    if(g_groupEditMode&&!finishing)return true; // The edit session commits only on Finish.
    auto path=GroupPositionFile();
    bool read=false;
    auto existingRows=ReadLines(path,&read);
    auto attributes=GetFileAttributesW(path.c_str());
    if(!read&&(attributes!=INVALID_FILE_ATTRIBUTES||GetLastError()!=ERROR_FILE_NOT_FOUND))return false;
    if(read) {
        if(existingRows.empty())return false;
        const auto& header=existingRows[0];
        bool oldShape=header.size()==3&&header[1].size()==1&&header[1][0]>=L'1'&&header[1][0]<=L'5';
        if(!(oldShape||(header.size()==4&&header[1]==L"6"))||
            header[0]!=L"schema"||header[2]!=L"group-geometry")return false;
    }
    bool wholeCells=g_wholeCellLayout||(read&&existingRows[0][1]==L"5");
    std::vector<std::wstring> lines{g_squareGridColumns?
        L"schema\t6\tgroup-geometry\t"+std::to_wstring(g_squareGridColumns):
        wholeCells?L"schema\t5\tgroup-geometry":L"schema\t4\tgroup-geometry"};
    for(size_t i=0;i<g_tiles.size();i++)if(auto group=g_tiles[i].folder) {
        auto found=g_groupPositions.find(group);
        if(found==g_groupPositions.end())continue;
        const auto& p=found->second;
        lines.push_back(L"group\t"+std::to_wstring(i)+L"\t"+EntryEncode(group->name)+L"\t"+std::to_wstring(p.x)+L"\t"+
            std::to_wstring(p.y)+L"\t"+std::to_wstring(p.w)+L"\t"+std::to_wstring(p.h)+L"\t"+(group->collapsed?L"folder":L"group")+
            L"\t"+std::to_wstring(p.columns)+L"\t"+std::to_wstring(p.rows)+
            L"\t"+std::to_wstring(p.gridX)+L"\t"+std::to_wstring(p.gridY)+
            L"\t"+std::to_wstring(p.gridW)+L"\t"+std::to_wstring(p.gridH)+
            L"\t"+std::to_wstring(p.style)+L"\t"+(p.snapped?L"1":L"0")+
            L"\t0\t0");
    }
    std::vector<std::wstring> existing;
    for(const auto& fields:existingRows) {
        std::wstring line;for(size_t i=0;i<fields.size();i++){if(i)line+=L'\t';line+=fields[i];}existing.push_back(line);
    }
    if(existing==lines)return true;
    return WriteLines(GroupPositionFile(),lines,false);
}


void SavePins() {
    std::vector<std::wstring> lines;
    for (const auto& key : g_pins) {
        lines.push_back(L"pin\t" + key);
    }
    WriteLines(StoragePath(L"pins.txt"), lines);
}

void LoadTiles() {
    ClearGroupPositions();
    // Do not replace the in-memory layout or write any data if any active
    // layout/pins/icons file becomes unreadable or unsupported after startup.
    if (!ryjs_data::ValidateDirectory(DataRoot())) {
        g_layoutReadOnly = g_dataReadOnly = true;
        Wh_Log(L"Invalid/unreadable layout data. Saving blocked; restore an external backup.");
        return;
    }
    g_pins.clear();
    for (const auto& f : ReadLines(StoragePath(L"pins.txt"))) {
        if (f.size() >= 2 && f[0] == L"pin") {
            g_pins.insert(ToLower(f[1]));
        }
    }
    auto path = StoragePath(LayoutFile());
    g_layoutReadOnly = false;
    auto records = ReadLines(path);
    bool importing = g_settings.groups && GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES;
    if (importing) {
        auto legacy = StoragePath(LegacyLayoutFile());
        records = ReadLines(legacy);
        CopyFileW(legacy.c_str(), (path + L".imported-layout.bak").c_str(), TRUE);
    }
    if (!records.empty() && records[0].size() >= 2 && records[0][0] == L"schema" && _wtoi(records[0][1].c_str()) > 1) {
        g_layoutReadOnly = true;
        records = ReadLines(path + L".bak");
        if (!records.empty() && records[0].size() >= 2 && records[0][0] == L"schema" && _wtoi(records[0][1].c_str()) > 1) records.clear();
        Wh_Log(L"Layout schema is newer than supported. Fallback is read-only.");
    }
    std::vector<std::wstring> lines;
    for (const auto& f : records) {
        std::wstring line;
        for (size_t i = 0; i < f.size(); i++) {
            line += (i ? L"\t" : L"") + f[i];
        }
        lines.push_back(std::move(line));
    }
    ArrangeTiles(lines);
    if(g_settings.groups&&LoadGroupPositions()&&records.empty()&&!g_layoutReadOnly) {
        // Fresh layouts start on the whole-cell grid. Existing arrangements
        // without a geometry file retain their legacy layout until editing.
        g_wholeCellLayout=true;
        g_freeGroupLayout=true;
    }
    if (importing) SaveTiles();
}

////////////////////////////////////////////////////////////////////////////////
// Search

std::wstring g_query;
bool g_searchActivated = false;
// 0 = the resting pill, 1 = the open field. Eased toward SearchExpanded() a
// frame at a time, so tapping the box grows it instead of snapping open.
float g_searchExpand = 0;
bool SearchExpanded() { return g_searchActivated || !g_query.empty(); }
// The search field is a small text editor: a caret and a selection anchor as
// UTF-16 indices into g_query (the selection is the span between them), a
// tick that times the caret's blink, and the state of a drag-selection.
size_t g_caret = 0;
size_t g_anchor = 0;
UINT64 g_caretTick = 0;
bool g_searchDrag = false;
UINT64 g_lastSearchClick = 0;
int g_lastSearchX = -1000;
// The arrangement, or the apps the search finds.
[[clang::no_destroy]] Tiles g_visible;

// 0: the name starts with the text; 1: a word of the name starts with it;
// 2: it's in the middle; -1: it isn't there. Ignoring case and accents.
int MatchRank(const std::wstring& name, const std::wstring& word) {
    int pos = FindNLSStringEx(
        LOCALE_NAME_USER_DEFAULT,
        FIND_FROMSTART | LINGUISTIC_IGNORECASE | LINGUISTIC_IGNOREDIACRITIC,
        name.c_str(), (int)name.size(), word.c_str(), (int)word.size(),
        nullptr, nullptr, nullptr, 0);
    if (pos < 0) {
        return -1;
    }
    if (pos == 0) {
        return 0;
    }
    return iswalnum(name[pos - 1]) ? 2 : 1;
}

bool GroupVisible(const std::shared_ptr<Folder>& group) {
    return group && (!group->inbox || !group->apps.empty() || g_groupEditMode);
}

void UpdateFilter() {
    if (g_query.empty()) {
        g_visible.clear();
        for (const auto& tile : g_tiles) {
            if (g_settings.groups && tile.folder && !GroupVisible(tile.folder)) continue;
            if (g_settings.groups && tile.folder && !tile.folder->collapsed) g_visible.insert(g_visible.end(), tile.folder->apps.begin(), tile.folder->apps.end());
            else g_visible.push_back(tile);
        }
        return;
    }
    std::vector<std::wstring> words;
    size_t start = 0;
    while (start < g_query.size()) {
        size_t end = g_query.find(L' ', start);
        if (end == std::wstring::npos) {
            end = g_query.size();
        }
        if (end > start) {
            words.push_back(g_query.substr(start, end - start));
        }
        start = end + 1;
    }

    std::vector<std::pair<int, int>> ranked;  // (position, index)
    for (int i = 0; i < (int)g_apps.size(); i++) {
        if (g_hidden.count(AppKey(*g_apps[i]))) {
            continue;
        }
        int rank = 0;
        for (size_t w = 0; w < words.size() && rank >= 0; w++) {
            int r = MatchRank(g_apps[i]->name, words[w]);
            if (r < 0 && !g_apps[i]->originalName.empty()) r = MatchRank(g_apps[i]->originalName, words[w]);
            rank = r < 0 ? -1 : (w == 0 ? r : rank);
        }
        if (rank >= 0) {
            // With only pinned apps, the pinned ones come first.
            bool off = PinnedOnly() && !g_pins.count(AppKey(*g_apps[i]));
            ranked.emplace_back(rank + (off ? 3 : 0), i);
        }
    }
    std::stable_sort(ranked.begin(), ranked.end(),
                     [](const auto& a, const auto& b) {
                         return a.first < b.first;
                     });
    g_visible.clear();
    for (const auto& r : ranked) {
        g_visible.push_back({g_apps[r.second], nullptr});
    }
}

////////////////////////////////////////////////////////////////////////////////
// Available area
//
// The monitor's work area already excludes the visible taskbar and the side
// bars that reserve space. The Standalone Dock usually doesn't reserve it (it
// hides by itself), so its strip is removed separately.

RECT AvailableArea(HMONITOR monitor) {
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    if (g_settings.coverScreen) {
        return mi.rcMonitor;
    }
    struct Search {
        HMONITOR monitor;
        RECT monitorRect;
        RECT area;
    } search{monitor, mi.rcMonitor, mi.rcWork};

    EnumWindows(
        [](HWND hwnd, LPARAM param) -> BOOL {
            auto* s = (Search*)param;
            WCHAR className[64];
            if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) ||
                wcscmp(className, kDockClass) ||
                MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) != s->monitor) {
                return TRUE;
            }
            int edge = (int)(INT_PTR)GetPropW(hwnd, kDockEdgeProp);
            int reserved = (int)(INT_PTR)GetPropW(hwnd, kDockReservedProp);
            const RECT& m = s->monitorRect;
            RECT& a = s->area;
            switch (edge) {
                case 1:
                    a.bottom = std::min<LONG>(a.bottom, m.bottom - reserved);
                    break;
                case 2:
                    a.left = std::max<LONG>(a.left, m.left + reserved);
                    break;
                case 3:
                    a.right = std::min<LONG>(a.right, m.right - reserved);
                    break;
            }
            return TRUE;
        },
        (LPARAM)&search);

    // A giant Dock can't reduce the area to zero.
    RECT& a = search.area;
    if (a.right - a.left < 200 || a.bottom - a.top < 200) {
        a = mi.rcWork;
    }
    return a;
}

// Monitor 1 is the main one, the rest follow from left to right (then top to
// bottom). A number with no monitor behind it falls back to the mouse.
HMONITOR ChosenMonitor() {
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR mouse = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    if (g_settings.monitor <= 0) {
        return mouse;
    }
    struct Entry {
        bool primary;
        RECT rc;
        HMONITOR monitor;
    };
    std::vector<Entry> monitors;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM param) -> BOOL {
            MONITORINFO mi{sizeof(mi)};
            GetMonitorInfoW(monitor, &mi);
            ((std::vector<Entry>*)param)
                ->push_back({(mi.dwFlags & MONITORINFOF_PRIMARY) != 0,
                             mi.rcMonitor, monitor});
            return TRUE;
        },
        (LPARAM)&monitors);
    std::stable_sort(monitors.begin(), monitors.end(),
                     [](const Entry& a, const Entry& b) {
                         if (a.primary != b.primary) {
                             return a.primary;
                         }
                         if (a.rc.left != b.rc.left) {
                             return a.rc.left < b.rc.left;
                         }
                         return a.rc.top < b.rc.top;
                     });
    if (g_settings.monitor > (int)monitors.size()) {
        return mouse;
    }
    return monitors[g_settings.monitor - 1].monitor;
}

// Puts the Dock windows above Start, which leaves the Dock's strip free.
// The background ones first: the Dock window is owned by one of them and stays above.
void RaiseDockWindows() {
    // Collects before moving: raising a window changes the order FindWindowEx
    // walks, and with two Docks it kept alternating between them endlessly.
    std::vector<HWND> windows;
    for (PCWSTR className : {kDockPlateClass, kDockClass}) {
        HWND hwnd = nullptr;
        while ((hwnd = FindWindowExW(nullptr, hwnd, className, nullptr))) {
            if (IsWindowVisible(hwnd)) {
                windows.push_back(hwnd);
            }
        }
    }
    // Without waiting for the Dock thread: the request goes to its queue.
    for (HWND hwnd : windows) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Drawing

[[clang::no_destroy]] ComPtr<ID2D1Factory> g_d2d;
[[clang::no_destroy]] ComPtr<IDWriteFactory> g_dwrite;
[[clang::no_destroy]] ComPtr<ID3D11Device> g_d3d;
[[clang::no_destroy]] ComPtr<ID2D1Device> g_d2dDevice;
// Private context for the label shadow renderer. Reset with the rest of the
// device objects so a device loss cannot leave it pointing at the old device.
[[clang::no_destroy]] ComPtr<ID2D1DeviceContext> g_shadowContext;
[[clang::no_destroy]] ComPtr<ID2D1DeviceContext> g_target;
[[clang::no_destroy]] ComPtr<ID2D1SolidColorBrush> g_brush;
[[clang::no_destroy]] ComPtr<ID2D1Bitmap1> g_frame;  // The GPU frame.
[[clang::no_destroy]] ComPtr<ID3D11Texture2D> g_staging;
[[clang::no_destroy]] LaunchpadPresenter g_presenter;
bool UseGpuPresenter() {
#ifdef SEAMLESS_PREVIEW
    if (Preview::smoke && !Preview::benchmarkGpu) return false;
#endif
    return g_settings.gpuPresenter && !g_presenter.failed;
}
int g_frameWidth = 0, g_frameHeight = 0;
UINT g_targetGeneration = 1;
struct TileGlassResources {
    ComPtr<ID2D1RadialGradientBrush> surface, rim;
};
[[clang::no_destroy]] TileGlassResources g_tileGlass;

struct Surface {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ old = nullptr;
    int width = 0, height = 0;
};
Surface g_surface;

void FreeSurface(Surface& s) {
    if (s.dc) {
        SelectObject(s.dc, s.old);
        DeleteObject(s.bitmap);
        DeleteDC(s.dc);
    }
    s = {};
}

bool EnsureSurface(Surface& s, int width, int height) {
    if (s.dc && s.width == width && s.height == height) {
        return true;
    }
    FreeSurface(s);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    s.dc = CreateCompatibleDC(nullptr);
    s.bitmap = CreateDIBSection(s.dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!s.bitmap) {
        DeleteDC(s.dc);
        s = {};
        return false;
    }
    s.old = SelectObject(s.dc, s.bitmap);
    s.width = width;
    s.height = height;
    return true;
}

void ResetTarget();


bool EnsureTarget() {
    if (g_target) {
        return true;
    }
    // A real GPU context: the grid rasterizes on the GPU and each frame is
    // copied back into the layered-window DIB. WARP is the fallback.
    for (D3D_DRIVER_TYPE type :
         {D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP}) {
        if (SUCCEEDED(D3D11CreateDevice(
                nullptr, type, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                nullptr, 0, D3D11_SDK_VERSION, &g_d3d, nullptr, nullptr))) {
            break;
        }
        g_d3d.Reset();
    }
    if (!g_d3d) {
        return false;
    }
    ComPtr<IDXGIDevice> dxgi;
    ComPtr<ID2D1Factory1> factory1;
    if (FAILED(g_d3d.As(&dxgi)) || FAILED(g_d2d.As(&factory1)) ||
        FAILED(factory1->CreateDevice(dxgi.Get(), &g_d2dDevice)) ||
        FAILED(g_d2dDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_target))) {
        ResetTarget();
        return false;
    }
    g_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    g_target->CreateSolidColorBrush(D2D1::ColorF(0, 0), &g_brush);
    g_targetGeneration++;
    return true;
}

void ResetTarget() {
    g_tileGlass={};
    g_presenter.Reset();
    g_frame.Reset();
    g_staging.Reset();
    g_frameWidth = g_frameHeight = 0;
    g_brush.Reset();
    g_shadowContext.Reset();  // Private context of the label shadow renderer.
    g_target.Reset();
    g_d2dDevice.Reset();
    g_d3d.Reset();
}

// The GPU frame target and the staging texture it is copied back through.
// Recreated whenever the window size changes.
bool EnsureFrame(int width, int height) {
    if (g_frame && g_frameWidth == width && g_frameHeight == height) {
        return true;
    }
    g_frame.Reset();
    g_staging.Reset();
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    if (FAILED(g_target->CreateBitmap(D2D1::SizeU(width, height), nullptr, 0,
                                      props, &g_frame))) {
        return false;
    }
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    ComPtr<ID3D11DeviceContext> ctx;
    g_d3d->GetImmediateContext(&ctx);
    if (!UseGpuPresenter() && FAILED(g_d3d->CreateTexture2D(&desc, nullptr, &g_staging))) {
        g_frame.Reset();
        return false;
    }
    g_frameWidth = width;
    g_frameHeight = height;
    return true;
}

// Copies the rendered frame from the GPU into the layered-window DIB.
void PresentFrame() {
    if (!g_frame || !g_surface.bitmap) {
        return;
    }
    if (!g_staging) {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = g_frameWidth; desc.Height = g_frameHeight;
        desc.MipLevels = desc.ArraySize = desc.SampleDesc.Count = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        if (FAILED(g_d3d->CreateTexture2D(&desc, nullptr, &g_staging))) return;
    }
    DIBSECTION ds{};
    if (GetObjectW(g_surface.bitmap, sizeof(ds), &ds) != sizeof(ds) ||
        !ds.dsBm.bmBits) {
        return;
    }
    ComPtr<IDXGISurface> surface;
    ComPtr<ID3D11Texture2D> texture;
    if (FAILED(g_frame->GetSurface(&surface)) ||
        FAILED(surface.As(&texture))) {
        return;
    }
    ComPtr<ID3D11DeviceContext> ctx;
    g_d3d->GetImmediateContext(&ctx);
    ctx->CopyResource(g_staging.Get(), texture.Get());
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(ctx->Map(g_staging.Get(), 0, D3D11_MAP_READ, 0, &mapped))) {
        return;
    }
    int width = ds.dsBm.bmWidth, height = ds.dsBm.bmHeight;
    size_t span = (size_t)width * 4;
    const BYTE* src = (const BYTE*)mapped.pData;
    BYTE* dst = (BYTE*)ds.dsBm.bmBits;
    for (int y = 0; y < height; y++) {
        memcpy(dst + (size_t)y * span, src + (size_t)y * mapped.RowPitch,
               span);
    }
    ctx->Unmap(g_staging.Get(), 0);
}

ID2D1Bitmap* GetBitmap(App& app) {
    if (app.pixels.empty()) {
        return nullptr;
    }
    if (app.generation != g_targetGeneration) {
        app.generation = g_targetGeneration;
        app.bitmap.Reset();
        g_target->CreateBitmap(
            D2D1::SizeU(app.iconSize, app.iconSize), app.pixels.data(),
            app.iconSize * 4,
            D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96, 96),
            &app.bitmap);
    }
    return app.bitmap.Get();
}

D2D1_COLOR_F Alpha(D2D1_COLOR_F c, float a) {
    c.a = a;
    return c;
}

void Fill(const D2D1_RECT_F& r, D2D1_COLOR_F color, float radius = 0) {
    g_brush->SetColor(color);
    if (radius > 0) {
        g_target->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius),
                                       g_brush.Get());
    } else {
        g_target->FillRectangle(r, g_brush.Get());
    }
}

////////////////////////////////////////////////////////////////////////////////
// Background
//
// Adjustable: a screenshot taken before the window appears, uploaded once and
// then scaled down and blurred on the GPU as an effect graph. Downscaling
// first makes a strong blur cheap.


struct Backdrop {
    ComPtr<ID2D1Bitmap> bitmap;
    int width = 0, height = 0;  // Blurred size.
    float sourceWidth = 0, sourceHeight = 0;  // Part used, in blurred pixels.
};
[[clang::no_destroy]] Backdrop g_backdropImage;

// Scales source down by f and gaussian-blurs it by sigma / f into a new sw x
// sh GPU bitmap. rect is the part of source used; the rest is cropped.
ComPtr<ID2D1Bitmap1> BlurToBitmap(ID2D1Bitmap* source,
                                  const D2D1_RECT_F& rect, int f, float sigma,
                                  int sw, int sh,
                                  D2D1_POINT_2F dest = D2D1::Point2F(0, 0)) {
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    ComPtr<ID2D1Bitmap1> target, small;
    ComPtr<ID2D1Effect> blur;
    if (FAILED(g_target->CreateBitmap(D2D1::SizeU(sw, sh), nullptr, 0, props,
                                      &small)) ||
        FAILED(g_target->CreateBitmap(D2D1::SizeU(sw, sh), nullptr, 0, props,
                                      &target)) ||
        FAILED(g_target->CreateEffect(CLSID_D2D1GaussianBlur, &blur))) {
        return nullptr;
    }
    // Pass 1: the part of source in rect, scaled down by f and placed at dest.
    const float w = (rect.right - rect.left) / f;
    const float h = (rect.bottom - rect.top) / f;
    g_target->BeginDraw();
    g_target->SetTarget(small.Get());
    g_target->Clear(D2D1::ColorF(0, 0));
    g_target->DrawBitmap(source,
                         D2D1::RectF(dest.x, dest.y, dest.x + w, dest.y + h),
                         1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, rect);
    if (FAILED(g_target->EndDraw())) {
        return nullptr;
    }
    // Pass 2: a gaussian blur of what pass 1 drew.
    blur->SetInput(0, small.Get());
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, sigma / f);
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
    g_target->BeginDraw();
    g_target->SetTarget(target.Get());
    g_target->Clear(D2D1::ColorF(0, 0));
    g_target->DrawImage(blur.Get(), nullptr, nullptr,
                        D2D1_INTERPOLATION_MODE_LINEAR);
    if (FAILED(g_target->EndDraw())) {
        return nullptr;
    }
    return target;
}

void CaptureBackdrop(const RECT& rc, float sigma) {
    g_backdropImage = {};
    int width = rc.right - rc.left, height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0 || !EnsureTarget()) {
        return;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    HBITMAP bitmap =
        CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(dc);
        ReleaseDC(nullptr, screen);
        return;
    }
    HGDIOBJ old = SelectObject(dc, bitmap);
    // Without CAPTUREBLT, which makes the cursor flicker; the screen composed
    // by DWM already includes layered windows.
    BitBlt(dc, 0, 0, width, height, screen, rc.left, rc.top, SRCCOPY);
    GdiFlush();
    // GDI screen copies have an undefined alpha channel; this source is opaque.
    for(size_t i=0;i<(size_t)width*height;i++)((UINT32*)bits)[i]|=0xFF000000;

    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    ComPtr<ID2D1Bitmap> full;
    bool uploaded =
        SUCCEEDED(g_target->CreateBitmap(D2D1::SizeU(width, height), bits,
                                         width * 4, props, &full));
    SelectObject(dc, old);
    DeleteObject(bitmap);
    DeleteDC(dc);
    ReleaseDC(nullptr, screen);
    if (!uploaded) {
        return;
    }

    int f = sigma >= 3 ? std::clamp((int)(sigma / 3), 1, 16) : 1;
    int sw = (width + f - 1) / f, sh = (height + f - 1) / f;
    ComPtr<ID2D1Bitmap1> blurred = BlurToBitmap(
        full.Get(), D2D1::RectF(0, 0, (float)width, (float)height), f, sigma,
        sw, sh);
    if (!blurred) {
        return;
    }
    g_backdropImage.bitmap = blurred.Get();
    g_backdropImage.width = sw;
    g_backdropImage.height = sh;
    g_backdropImage.sourceWidth = (float)width / f;
    g_backdropImage.sourceHeight = (float)height / f;
}

ID2D1Bitmap* GetBackdropBitmap() {
    return g_backdropImage.bitmap.Get();
}

// The open folder's panel is frosted glass: the grid under it, blurred. The
// blur is taken from the window surface right after the grid is drawn and
// kept while nothing under the panel moves.
struct PanelBlur {
    ComPtr<ID2D1Bitmap> bitmap;
    ComPtr<ID2D1BitmapBrush> brush;
    D2D1_RECT_F rect{};  // Window pixels it was taken from.
    int x = 0, y = 0, factor = 1;
    const void* folder = nullptr;
    float scroll = 0;
    UINT generation = 0;
};
[[clang::no_destroy]] PanelBlur g_panelBlur;

void CapturePanelBlur(const D2D1_RECT_F& rect, const void* folder,
                      float scroll, float scale) {
    g_panelBlur = {};
    g_panelBlur.rect = rect;
    g_panelBlur.folder = folder;
    g_panelBlur.scroll = scroll;
    g_panelBlur.generation = g_targetGeneration;
    if (!g_frame) {
        return;
    }
    // The blur source is the frame just drawn, still on the GPU; only the
    // region under the panel is used.
    int x0 = std::clamp((int)std::floor(rect.left), 0, g_frameWidth);
    int y0 = std::clamp((int)std::floor(rect.top), 0, g_frameHeight);
    int x1 = std::clamp((int)std::ceil(rect.right), 0, g_frameWidth);
    int y1 = std::clamp((int)std::ceil(rect.bottom), 0, g_frameHeight);
    if (x1 <= x0 || y1 <= y0) {
        return;
    }
    float sigma = 24 * scale;
    int f = std::clamp((int)(sigma / 3), 1, 16);
    int sw = (x1 - x0 + f - 1) / f, sh = (y1 - y0 + f - 1) / f;
    ComPtr<ID2D1Bitmap1> blurred =
        BlurToBitmap(g_frame.Get(),
                     D2D1::RectF((float)x0, (float)y0, (float)x1, (float)y1),
                     f, sigma, sw, sh);
    if (!blurred) {
        return;
    }
    g_panelBlur.bitmap = blurred.Get();
    g_panelBlur.x = x0;
    g_panelBlur.y = y0;
    g_panelBlur.factor = f;
}

bool PanelBlurFresh(const D2D1_RECT_F& rect, const void* folder,
                    float scroll) {
    const PanelBlur& b = g_panelBlur;
    return b.folder == folder && b.generation == g_targetGeneration &&
           b.scroll == scroll && std::abs(b.rect.left - rect.left) < 0.5f &&
           std::abs(b.rect.top - rect.top) < 0.5f &&
           std::abs(b.rect.right - rect.right) < 0.5f &&
           std::abs(b.rect.bottom - rect.bottom) < 0.5f;
}

// Paints the kept blur into rect (window pixels, identity transform).
void DrawPanelBlur(const D2D1_RECT_F& rect, float radius) {
    PanelBlur& b = g_panelBlur;
    if (!b.bitmap) {
        return;
    }
    if (!b.brush) g_target->CreateBitmapBrush(
        b.bitmap.Get(),
        D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP,
                                    D2D1_EXTEND_MODE_CLAMP,
                                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR),
        &b.brush);
    if (!b.brush) {
        return;
    }
    b.brush->SetTransform(D2D1::Matrix3x2F::Scale((float)b.factor,
                                                (float)b.factor) *
                        D2D1::Matrix3x2F::Translation((float)b.x, (float)b.y));
    g_target->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius),
                                   b.brush.Get());
}

// Acrylic and Windows blur: a regular window with the DWM effect sits right
// below the layered window where everything is drawn, like in the Dock.

struct AccentPolicy {
    INT accentState;
    INT accentFlags;
    INT gradientColor;
    INT animationId;
};

struct WindowCompositionAttribData {
    DWORD attrib;
    PVOID data;
    SIZE_T size;
};

constexpr DWORD WCA_ACCENT_POLICY = 19;
constexpr INT ACCENT_DISABLED = 0;
constexpr INT ACCENT_ENABLE_BLURBEHIND = 3;
constexpr INT ACCENT_ENABLE_ACRYLICBLURBEHIND = 4;
constexpr DWORD kDwmUseImmersiveDarkMode = 20;
constexpr DWORD kDwmWindowCornerPreference = 33;
constexpr DWORD kDwmBorderColor = 34;

HWND g_backdrop;

void SetAccent(HWND hwnd, INT state, DWORD abgr) {
    using SetWindowCompositionAttribute_t =
        BOOL(WINAPI*)(HWND, WindowCompositionAttribData*);
    static auto setWindowCompositionAttribute =
        (SetWindowCompositionAttribute_t)GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (!setWindowCompositionAttribute) {
        return;
    }
    AccentPolicy policy{state, state ? 2 : 0, (INT)abgr, 0};
    WindowCompositionAttribData data{WCA_ACCENT_POLICY, &policy,
                                     sizeof(policy)};
    setWindowCompositionAttribute(hwnd, &data);
}

void ApplyBackdropEffect() {
    // Makes DWM respect the window's alpha channel, without blur.
    DWM_BLURBEHIND blur{};
    blur.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    blur.fEnable = TRUE;
    blur.hRgnBlur = CreateRectRgn(0, 0, -1, -1);
    DwmEnableBlurBehindWindow(g_backdrop, &blur);
    DeleteObject(blur.hRgnBlur);

    BOOL dark = TRUE;
    DwmSetWindowAttribute(g_backdrop, kDwmUseImmersiveDarkMode, &dark,
                          sizeof(dark));
    UINT corner = 1;  // DWMWCP_DONOTROUND
    DwmSetWindowAttribute(g_backdrop, kDwmWindowCornerPreference, &corner,
                          sizeof(corner));
    COLORREF border = 0xFFFFFFFE;  // DWMWA_COLOR_NONE
    DwmSetWindowAttribute(g_backdrop, kDwmBorderColor, &border,
                          sizeof(border));

    // The color is drawn on the top window, with the chosen opacity; acrylic
    // only requires a nonzero alpha.
    D2D1_COLOR_F c = g_settings.tint;
    DWORD abgr = (1u << 24) | ((DWORD)(c.b * 255) << 16) |
                 ((DWORD)(c.g * 255) << 8) | (DWORD)(c.r * 255);
    switch (g_settings.blurMode) {
        case BlurMode::Acrylic:
            SetAccent(g_backdrop, ACCENT_ENABLE_ACRYLICBLURBEHIND, abgr);
            break;
        case BlurMode::Blur:
            SetAccent(g_backdrop, ACCENT_ENABLE_BLURBEHIND, 0);
            break;
        default:
            SetAccent(g_backdrop, ACCENT_DISABLED, 0);
            break;
    }
}

bool UsesDwmBackdrop() {
    return g_settings.blurMode == BlurMode::Acrylic ||
           g_settings.blurMode == BlurMode::Blur;
}

////////////////////////////////////////////////////////////////////////////////
// Layout

struct Layout {
    RECT area{};  // Window position on screen.
    int width = 0, height = 0;
    float scale = 1;
    float icon = 72, labelHeight = 20, cellWidth = 140, cellHeight = 140;
    float cellSpare = 0;  // Extra row height that fills the screen with pages.
    float cellPad = 6;
    int columns = 1;
    float gridLeft = 0;
    float viewTop = 0, viewBottom = 0;  // Visible range, in whole rows.
    int visibleRows = 1;
    float contentHeight = 0;
    int perPage = 1, pages = 1;  // With the pages layout.
    float dotsY = 0;             // Center of the page dots.
    D2D1_RECT_F search{};
    D2D1_RECT_F clearButton{};  // Clear the current query.
    D2D1_RECT_F powerBar{};    // The power dock pill, aligned by dockAlignment.
    std::vector<int> powerItems;  // kPower indices shown, left to right.
    float powerButton = 36, powerGap = 6;
};

Layout g_layout;
LaunchpadMetrics g_metrics;
void FinishMetrics() {
    auto message = g_metrics.Finish(StoragePath(L"performance.csv"), Now(), g_presenter.Active() && UseGpuPresenter());
    if (!message.empty()) Wh_Log(L"%s", message.c_str());
}
// Set from GetPwrCapabilities each time the grid opens.
bool g_canSleep = true, g_canHibernate = false;
// Pixels: down the list, or, with pages, across the row of pages.
float g_scroll, g_scrollTarget;
float g_groupPagePan=0;
int g_wheelDelta;  // Wheel accumulated until it completes a row (touchpads).

[[clang::no_destroy]] ComPtr<IDWriteTextFormat> g_labelFormat, g_searchFormat,
    g_messageFormat, g_iconFormat, g_tipFormat, g_groupFormat;
float g_formatScale;
UINT g_formatVersion = 0;

void EnsureFormats(float scale) {
    if (g_labelFormat && g_formatScale == scale) {
        return;
    }
    g_formatScale = scale;
    ++g_formatVersion;
    auto make = [&](float size, DWRITE_FONT_WEIGHT weight,
                    DWRITE_TEXT_ALIGNMENT align,
                    ComPtr<IDWriteTextFormat>& format,
                    PCWSTR family = nullptr) {
        format.Reset();
        if (!family) family = g_settings.font.c_str();
        g_dwrite->CreateTextFormat(family, nullptr, weight,
                                   DWRITE_FONT_STYLE_NORMAL,
                                   DWRITE_FONT_STRETCH_NORMAL, size * scale,
                                   L"", &format);
        if (!format) {
            return;
        }
        format->SetTextAlignment(align);
        format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        ComPtr<IDWriteInlineObject> ellipsis;
        g_dwrite->CreateEllipsisTrimmingSign(format.Get(), &ellipsis);
        format->SetTrimming(&trimming, ellipsis.Get());
    };
    // The label is the only thing that has to stay legible over glass: one
    // DIP larger and semi-bold. The weight itself is a hint — DirectWrite
    // picks the closest weight the family actually has (400/700 for Microsoft
    // YaHei), which is the proper mapping, never a fake double draw.
    make((float)g_settings.labelSize + 1, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER,
         g_labelFormat);
    make(19, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER, g_groupFormat);
    make(15, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING,
         g_searchFormat);
    make(16, DWRITE_FONT_WEIGHT_SEMI_LIGHT, DWRITE_TEXT_ALIGNMENT_CENTER,
         g_messageFormat);
    make(12.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
         g_tipFormat);
    // Windows 11 has Segoe Fluent Icons; Windows 10 only the older MDL2 font,
    // which has the same code points for these glyphs.
    ComPtr<IDWriteFontCollection> fonts;
    UINT32 index;
    BOOL fluent = FALSE;
    if (SUCCEEDED(g_dwrite->GetSystemFontCollection(&fonts))) {
        fonts->FindFamilyName(L"Segoe Fluent Icons", &index, &fluent);
    }
    make(15, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER,
         g_iconFormat, fluent ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets");
}

float GridMargin() {
    return std::min(g_layout.width*.12f,std::max(24*g_layout.scale,g_layout.width*.045f));
}
float SquareCellSize(int columns) {
    return std::max(1.f,g_layout.width-2*GridMargin())/std::max(1,columns);
}
int DesiredGridColumns() {
    if(g_settings.globalColumns>0)return g_settings.globalColumns;
    if(g_squareGridColumns>0)return g_squareGridColumns;
    if(g_settings.columns>0)return g_settings.columns;
    float density=(std::max(g_settings.iconSize+12.f,(g_settings.labelSize+8)*4)+g_settings.horizontalGap)*g_layout.scale;
    return std::clamp((int)std::round((g_layout.width-2*GridMargin())/density),1,30);
}

float GroupPitchY();
float MaxScroll() {
    if (g_settings.pages) {
        return (g_layout.pages - 1) * (float)g_layout.width;
    }
    float editRoom=g_groupEditMode&&g_settings.groups&&g_query.empty()?(g_squareGridColumns?SquareCellSize(g_squareGridColumns):GroupPitchY()):0;
    return std::max(0.0f, g_layout.contentHeight + editRoom -
                              (g_layout.viewBottom - g_layout.viewTop));
}

bool GroupedView() { return g_settings.groups && g_query.empty(); }
void ComputeGroups();
std::array<D2D1_RECT_F,3> GroupEditButtons();
float MaxPagePan(int page);
void LayoutSearch() {
    auto& L = g_layout;
    float s = L.scale, W = (float)L.width;
    // The width follows the eased amount, so the pill opens into the field
    // rather than changing size in one frame.
    float t = std::clamp(g_searchExpand, 0.f, 1.f);
    t = t * t * (3 - 2 * t);
    float width =
        std::min((44.f + (240.f - 44.f) * t) * s, std::max(1.f, W - 40 * s));
    float top = std::round(std::max(28*s, L.height * 0.045f));
    L.search = {std::round((W - width) / 2), top, std::round((W + width) / 2),
                top + std::round(36*s)};
    L.clearButton = {};
    if (!g_query.empty()) {
        float button = std::round(18*s);
        float cx = L.search.right - 18*s, cy = (L.search.top + L.search.bottom) / 2;
        L.clearButton = {cx-button/2, cy-button/2, cx+button/2, cy+button/2};
    }
    // The resting search icon no longer takes layout room: the grid starts
    // one pill higher and sinks back as the field opens. The eased amount
    // keeps that move continuous while the field grows, and computing it here
    // lets the per-frame expansion callers (OnFrame and the click path)
    // update the grid position without a full relayout.
    L.viewTop = std::round(L.search.top + 28 * s) +
                std::round(std::round(36 * s) * t);
}

void ComputeLayout() {
    Layout& L = g_layout;
    float s = L.scale;
    float W = (float)L.width, H = (float)L.height;

    LayoutSearch();

    L.icon = std::round(g_settings.iconSize * s);
    L.labelHeight = std::round((g_settings.labelSize + 8) * s);  // +1 DIP label size
    L.cellPad = std::round(6 * s);
    L.cellHeight = std::round(L.icon + L.labelHeight + (18+g_settings.verticalGap) * s);
    // viewTop comes from LayoutSearch: the grid rises when the search field
    // rests (no icon), keeping the move tied to the eased expansion.

    float side = std::max(48 * s, W * 0.07f);
    float available = std::max(L.icon * 2, W - 2 * side);
    float minCell = L.icon + g_settings.horizontalGap * s;
    L.columns = g_settings.columns > 0
                    ? std::min(g_settings.columns, std::max(1,(int)(available/(L.icon+8*s))))
                    : std::clamp((int)(available / minCell), 2, 14);
    // With few columns on a wide screen, the icons don't spread out too much.
    L.cellWidth = std::floor(
        std::min(available / L.columns, std::max(minCell, L.icon * 2.6f)));
    L.gridLeft = std::round((W - L.cellWidth * L.columns) / 2);

    // Only whole rows are visible; scrolling moves row by row. Pages keep
    // room below for the dots.
    float bottomRoom = std::round((g_settings.pages ? 44 : 16) * s);

    // The power bar: a pill of round buttons, centered at the bottom, with
    // the grid (and the page dots) above it.
    L.powerItems.clear();
    if (g_settings.powerBar) {
        for (int i = 0; i < (int)ARRAYSIZE(kPower); i++) {
            auto action = (PowerAction)i;
            if (g_settings.powerShown[i] &&
                (action != PowerAction::Sleep || g_canSleep) &&
                (action != PowerAction::Hibernate || g_canHibernate)) {
                L.powerItems.push_back(i);
            }
        }
    }
    L.powerBar = {};
    if (!L.powerItems.empty()) {
        L.powerButton = std::round(36 * s);
        L.powerGap = std::round(6 * s);
        float pad = std::round(5 * s);
        int n = (int)L.powerItems.size();
        float barWidth = n * L.powerButton + (n - 1) * L.powerGap + 2 * pad;
        float barHeight = L.powerButton + 2 * pad;
        float bottom = H - std::round(std::max(22 * s, H * 0.028f));
        // A DPI scaled margin in from the edge the setting picks.
        float edge = std::round(24 * s);
        float left = g_settings.dockAlign == DockAlign::Left  ? edge
                     : g_settings.dockAlign == DockAlign::Right
                         ? W - barWidth - edge
                         : std::round((W - barWidth) / 2);
        L.powerBar = D2D1::RectF(left, bottom - barHeight, left + barWidth,
                                 bottom);
        bottomRoom = H - L.powerBar.top +
                     std::round((g_settings.pages ? 30 : 10) * s);
    }

    bool grouped=GroupedView();
    if(grouped&&g_wholeCellLayout&&g_groupPositions.empty()&&!g_groupEditMode&&!g_squareGridColumns)
        g_squareGridColumns=DesiredGridColumns();
    int squareColumns=grouped?g_squareGridColumns:DesiredGridColumns();
    if(squareColumns) {
        L.columns=squareColumns;
        L.cellWidth=L.cellHeight=SquareCellSize(squareColumns);
        L.gridLeft=GridMargin();
    }
    float viewportEnd=H-bottomRoom;
    if(grouped&&squareColumns&&g_groupEditMode)
        viewportEnd=std::min(viewportEnd,GroupEditButtons()[0].top-6*s);
    L.visibleRows=std::max(1,(int)((viewportEnd-L.viewTop)/L.cellHeight));
    L.cellSpare = 0;
    if (g_settings.pages&&!squareColumns) {
        // Pages don't scroll, so the rows spread out to the dots instead of
        // leaving the part of a row that doesn't fit empty at the bottom.
        float rowHeight =
            std::floor((H - bottomRoom - L.viewTop) / L.visibleRows);
        L.cellSpare = std::max(0.0f, rowHeight - L.cellHeight);
        L.cellHeight += L.cellSpare;
    }
    L.viewBottom = squareColumns?std::max(L.viewTop+1,viewportEnd):L.viewTop + L.visibleRows * L.cellHeight;
    L.dotsY = std::round(
        (L.viewBottom + (L.powerItems.empty() ? H : L.powerBar.top)) / 2);

    int count = (int)g_visible.size();
    int rows = (count + L.columns - 1) / L.columns;
    L.contentHeight = rows * L.cellHeight;
    L.perPage = L.visibleRows * L.columns;
    L.pages = std::max(1, (count + L.perPage - 1) / L.perPage);
    if (GroupedView()) ComputeGroups();
    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll());
    g_scroll = std::clamp(g_scroll, 0.0f, MaxScroll());
    g_groupPagePan=std::clamp(g_groupPagePan,0.f,MaxPagePan(std::max(0,(int)std::lround(g_scroll/std::max(1,L.width)))));
}

struct Cell {
    D2D1_RECT_F highlight, icon, label;
    bool wrapLabel=false;
    D2D1_RECT_F unit{};
};
// A text layout pre-rendered as white text over its soft dark shadow, padded
// so the blur has room around the layout box. Tile labels and group headers
// share this.
struct LabelShadowBitmap {
    ComPtr<ID2D1Bitmap1> bitmap;
    float padX=0, padTop=0, padBottom=0;
};
bool RenderTextWithShadow(IDWriteTextLayout* layout, float width, float height,
                          LabelShadowBitmap& out) {
    out.bitmap.Reset();
    if (!g_d2dDevice || !layout || width <= 0 || height <= 0) return false;
    // A private device context keeps this legal at any call time: the visible
    // context sits inside its own BeginDraw session while the frame is drawn,
    // and one context cannot nest BeginDraw on itself. Everything here is
    // device-scoped, so the bitmap draws fine from the visible context.
    if (!g_shadowContext && FAILED(g_d2dDevice->CreateDeviceContext(
                    D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_shadowContext)))
        return false;
    auto& dc = g_shadowContext;
    float s = g_layout.scale;
    float sigma = 2.4f * s;                       // blur radius, ~2.4 DIP
    float pad = sigma * 3;
    out.padX = pad;
    out.padTop = pad;
    out.padBottom = pad + std::round(1 * s);      // 1 DIP shadow offset below
    D2D1_SIZE_U size{
        (UINT)std::ceil(width + out.padX * 2),
        (UINT)std::ceil(height + out.padTop + out.padBottom)};
    D2D1_BITMAP_PROPERTIES1 props{
        {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED},
        0, 0, D2D1_BITMAP_OPTIONS_TARGET, nullptr};
    ComPtr<ID2D1Bitmap1> bitmap;
    if (FAILED(dc->CreateBitmap(size, nullptr, 0, props, &bitmap)))
        return false;
    // The white text goes into a command list: it feeds both the shadow
    // effect and the composite on top of it.
    ComPtr<ID2D1CommandList> list;
    if (FAILED(dc->CreateCommandList(&list))) return false;
    dc->SetTarget(list.Get());
    dc->BeginDraw();
    g_brush->SetColor(D2D1::ColorF(0xFFFFFF, 1.0f));
    dc->DrawTextLayout({0, 0}, layout, g_brush.Get(),
                       D2D1_DRAW_TEXT_OPTIONS_CLIP);
    if (FAILED(dc->EndDraw()) || FAILED(list->Close())) return false;
    ComPtr<ID2D1Effect> shadow, offset;
    if (FAILED(dc->CreateEffect(CLSID_D2D1Shadow, &shadow))) return false;
    shadow->SetInput(0, list.Get());
    shadow->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, sigma);
    shadow->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::Vector4F(0, 0, 0, 0.4f));
    if (FAILED(dc->CreateEffect(CLSID_D2D12DAffineTransform, &offset))) return false;
    offset->SetInputEffect(0, shadow.Get());
    offset->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX,
                     D2D1::Matrix3x2F::Translation(out.padX,
                                                   out.padTop + std::round(1 * s)));
    dc->SetTarget(bitmap.Get());
    dc->BeginDraw();
    dc->Clear(D2D1::ColorF(0, 0, 0, 0));
    dc->DrawImage(offset.Get());
    dc->DrawImage(list.Get(), D2D1::Point2F(out.padX, out.padTop));
    if (FAILED(dc->EndDraw())) return false;
    out.bitmap = std::move(bitmap);  // Hand the one reference to the caller.
    return true;
}
Cell GroupCellAt(int index);
Cell SpaceTileContents(const Tile& tile, Cell cell);

// The cell whose top left corner is x, y.
Cell MakeCell(float x, float y, float width, float height, float spare) {
    const Layout& L = g_layout;
    Cell c;
    c.unit={x,y,x+width,y+height-spare};
    c.highlight = D2D1::RectF(x + L.cellPad, y, x + width - L.cellPad,
                              y + height - spare - L.cellPad * 2);
    float iconLeft = std::round(x + (width - L.icon) / 2);
    float iconTop = std::round(y + 10 * L.scale);
    c.icon = D2D1::RectF(iconLeft, iconTop, iconLeft + L.icon, iconTop + L.icon);
    float labelTop = c.icon.bottom + std::round(8 * L.scale);
    c.label = D2D1::RectF(x + L.cellPad + 4 * L.scale, labelTop,
                          x + width - L.cellPad - 4 * L.scale,
                          labelTop + L.labelHeight);
    return c;
}

Cell MakeSquareCell(float x,float y,float side) {
    const auto& L=g_layout;
    float pad=std::min(3*L.scale,side*.05f);
    float line=std::min(L.labelHeight,side*.45f);
    float gap=g_settings.showLabels?std::min(6*L.scale,side*.08f):0;
    // Labels stay single-line everywhere: the second row made sparse groups
    // push their icons higher than dense ones, which read as misaligned.
    // Long names keep the character-level ellipsis from the label format.
    bool wrap=false;
    float label=g_settings.showLabels?line:0;
    float icon=std::max(0.f,std::min(L.icon,side-2*pad-label-gap));
    float top=y+(side-icon-gap-label)/2;
    float left=x+(side-icon)/2;
    return {{x+pad,y+pad,x+side-pad,y+side-pad},{left,top,left+icon,top+icon},
        {x+pad,top+icon+gap,x+side-pad,top+icon+gap+label},wrap,{x,y,x+side,y+side}};
}

Cell MakeCell(float x,float y) {
    return MakeSquareCell(x,y,g_layout.cellWidth);
}

Cell CellAt(int index) {
    if (GroupedView()) return GroupCellAt(index);
    const Layout& L = g_layout;
    float x, y;
    if (g_settings.pages) {
        int page = index / L.perPage, slot = index % L.perPage;
        x = L.gridLeft + (slot % L.columns) * L.cellWidth +
            page * (float)L.width - std::round(g_scroll);
        y = L.viewTop + (slot / L.columns) * L.cellHeight-g_groupPagePan;
    } else {
        int row = index / L.columns, col = index % L.columns;
        x = L.gridLeft + col * L.cellWidth;
        y = L.viewTop + row * L.cellHeight - std::round(g_scroll);
    }
    y += std::round(L.cellSpare / 2);  // Centered in a stretched row.
    return SpaceTileContents(g_visible[index], MakeCell(x, y));
}

// Included after CellAt: geometry uses the same MakeCell and DrawTile pipeline.
struct ExpandedGroupGeometry {
    std::shared_ptr<Folder> group;
    D2D1_RECT_F area, title;
    int start = 0, firstInGroup = 0, count = 0, columns = 1, entryOrder = 0;
    float cellWidth = 0, rowPitch = 0;
    int visibleRows = 1;
    bool wholeCells = false, squareCells = false;
};
[[clang::no_destroy]] std::vector<ExpandedGroupGeometry> g_groupGeometry;
[[clang::no_destroy]] std::vector<Cell> g_groupCells;
[[clang::no_destroy]] std::shared_ptr<Folder> g_groupDropFolder, g_groupHeaderDragging;
int g_groupDropSlot = -1;
void CancelGroupPointer();
void CancelGroupEdit();
bool ConfirmGroupEditClose();
void DrawGroupEditBar();
void GroupHeaderTick();
bool TickGroupGesture();
extern float g_appear;
extern bool g_closing;
float Ease(float t);

bool FlyInActive() {
    return g_settings.iconFlyIn && g_appear < 1.f && !g_groupEditMode;
}

float FlyInChromeOpacity() {
    return g_settings.iconFlyIn && !g_groupEditMode
        ? launchpad_fly_in::SmoothStep((g_appear - .05f) / .62f)
        : 1.f;
}

launchpad_fly_in::Pose FlyInPose(D2D1_POINT_2F center, float margin) {
    // Limit the spatial stagger to 45 ms at the default open duration.
    float delay = std::min(.18f, 45.f / g_settings.openMs);
    return launchpad_fly_in::Sample(g_appear, center.x, center.y,
                                    (float)g_layout.width, (float)g_layout.height,
                                    margin, delay);
}

[[clang::no_destroy]] std::map<const void*,int> g_groupEntryOrder;
float GroupEntryProgress(int order) {
    if (g_settings.iconFlyIn || !Animate() || g_closing || g_groupEditMode) return 1;
    float delay=std::min(order,6)*0.035f;
    return Ease(std::clamp((g_appear-delay)/(1-delay),0.0f,1.0f));
}
float GroupTileProgress(const Tile& tile) {
    if (!GroupedView() || !tile.app) return 1;
    auto p=g_groupEntryOrder.find(tile.app.get());
    return p==g_groupEntryOrder.end() ? 1 : GroupEntryProgress(p->second);
}

bool GroupContains(const D2D1_RECT_F& r, float x, float y) {
    return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}
D2D1_RECT_F ProjectGroupRect(D2D1_RECT_F r) {
    if (g_settings.pages) {
        r.left -= g_scroll; r.right -= g_scroll;
        r.top-=g_groupPagePan;r.bottom-=g_groupPagePan;
    }
    else { r.top -= g_scroll; r.bottom -= g_scroll; }
    return r;
}
// Complete pitch includes image, padding, text allocation and the user gap.
float LegacyGroupPitchX() {return std::max(g_layout.icon+2*g_layout.cellPad+g_settings.horizontalGap*g_layout.scale,
    (g_settings.labelSize+7)*4*g_layout.scale);}
float LegacyGroupPitchY() {return std::round(g_layout.icon+g_layout.labelHeight+(18+g_settings.verticalGap)*g_layout.scale);}
float GroupPitchX() {
    return (std::max(g_settings.iconSize+12.f,(g_settings.labelSize+7)*4)+g_settings.horizontalGap)*g_layout.scale;
}
float GroupPitchY() {
    return (g_settings.iconSize+g_settings.labelSize+7+18+g_settings.verticalGap)*g_layout.scale;
}
float GroupStepX() { return SquareCellSize(g_squareGridColumns)/g_layout.scale/2; }
float GroupStepY() { return GroupStepX()/2; }
float GroupXAt(int index) {
    int steps=2*g_squareGridColumns;
    int page=index/steps;
    return (page*g_layout.width+GridMargin())/g_layout.scale+(index%steps)*GroupStepX();
}
float LegacyGroupTitleHeight(bool legacyGrid=false) {
    float s=g_layout.scale;
    static UINT formatVersion=0;
    static float fontHeight=0;
    if(g_groupFormat&&g_dwrite&&formatVersion!=g_formatVersion) {
        formatVersion=g_formatVersion;
        ComPtr<IDWriteTextLayout> text;
        DWRITE_TEXT_METRICS metrics{};
        if(SUCCEEDED(g_dwrite->CreateTextLayout(L"Ag",2,g_groupFormat.Get(),1000,1000,&text))&&
           SUCCEEDED(text->GetMetrics(&metrics)))fontHeight=metrics.height/g_formatScale;
    }
    return std::max((std::max(fontHeight,24.f)+8)*s,legacyGrid?GroupPitchY()*.5f:36*s);
}
constexpr float kGroupPadding=12;
float GroupIconPitchX() { return std::round(GroupPitchX()); }
float GroupIconPitchY() { return std::round(GroupPitchY()); }
int FittingGroupColumns(float width) {
    float minimumCellWidth=g_layout.icon+2*g_layout.cellPad;
    return 1+std::max(0,(int)std::floor((width-2*kGroupPadding*g_layout.scale-minimumCellWidth+.01f)/GroupIconPitchX()));
}
GroupPosition EffectiveGroupPosition(GroupPosition p) {
    float s=g_layout.scale;
    if(p.SquareCells()&&g_squareGridColumns) {
        p.x=GroupXAt(p.gridX);p.y=p.gridY*GroupStepY();
        p.w=p.gridW*GroupStepX();p.h=p.gridH*GroupStepY();
        return p;
    }
    float stepX=GroupPitchX()/s,stepY=GroupPitchY()/s;
    if(!p.WholeCells()) {stepX*=.5f;stepY*=.5f;}
    if((p.style==0&&p.OnGrid())||p.snapped) {
        p.x=p.gridX*stepX;p.y=p.gridY*stepY;
    }
    if(p.OnGrid()) {
        p.w=p.gridW*stepX;p.h=p.gridH*stepY;
        if(p.style==2) {
            p.w+=2*kGroupPadding;
            p.h+=LegacyGroupTitleHeight()/s+2*kGroupPadding;
        }
    } else if(p.style==0&&p.columns>0&&p.rows>0) {
        p.w=24+p.columns*LegacyGroupPitchX()/s;
        p.h=60+p.rows*LegacyGroupPitchY()/s;
    }
    return p;
}
GroupPosition PositionFromArea(const D2D1_RECT_F& r) {
    float s=g_layout.scale;
    return {r.left/s,(r.top-g_layout.viewTop)/s,(r.right-r.left)/s,(r.bottom-r.top)/s};
}
D2D1_RECT_F GroupPositionRect(const GroupPosition& p) {
    float s=g_layout.scale;
    return {p.x*s,g_layout.viewTop+p.y*s,(p.x+p.w)*s,g_layout.viewTop+(p.y+p.h)*s};
}
// WM mouse coordinates and the 96-DPI D2D target use client pixels.
POINTF GroupPointerInLayout(int x,int y) {
    return {(x+(g_settings.pages?g_scroll:0))/g_layout.scale,
        (y-g_layout.viewTop+(g_settings.pages?g_groupPagePan:g_scroll))/g_layout.scale};
}
float GroupEase(double started) {
    if(!Animate())return 1;
    float t=std::clamp((float)((Now()-started)/.14),0.f,1.f);
    return 1-(1-t)*(1-t)*(1-t);
}
GroupPosition InterpolateGroup(const GroupPosition& a,const GroupPosition& b,float t) {
    return {std::lerp(a.x,b.x,t),std::lerp(a.y,b.y,t),std::lerp(a.w,b.w,t),std::lerp(a.h,b.h,t)};
}
D2D1_RECT_F GroupResizeHit(const D2D1_RECT_F& r) {
    float size=std::min({26*g_layout.scale,(r.right-r.left)/3,(r.bottom-r.top)/3});
    return {r.right-size,r.bottom-size,r.right,r.bottom};
}
GroupPosition VisualGroupPosition(const std::shared_ptr<Folder>& group,GroupPosition base) {
    if(g_groupGesture.moved) {
        if(group==g_groupGesture.group)return g_groupGesture.continuous;
        if(g_groupGesture.valid)for(const auto& node:g_groupGesture.solution)
            if(node.group==group&&!node.fixed)return node.position;
    }
    auto transition=g_groupTransitions.find(group);
    if(transition==g_groupTransitions.end())return base;
    const auto& t=transition->second;
    return InterpolateGroup(t.from,t.to,GroupEase(t.started));
}
D2D1_RECT_F VisualGroupRect(const ExpandedGroupGeometry& geometry) {
    if(g_groupTransitions.empty()&&!g_groupGesture.moved)return ProjectGroupRect(geometry.area);
    return ProjectGroupRect(GroupPositionRect(VisualGroupPosition(geometry.group,PositionFromArea(geometry.area))));
}
ExpandedGroupGeometry MakeFreeGroupGeometry(const std::shared_ptr<Folder>& group,GroupPosition position,
                                           int global,int order) {
    const auto& L=g_layout;
    float s=L.scale;
    auto p=EffectiveGroupPosition(position);
    float side=p.SquareCells()?SquareCellSize(g_squareGridColumns):0;
    float pad=(side||p.WholeCells())?0:kGroupPadding*s;
    float header=side?side/2:p.WholeCells()?GroupPitchY():LegacyGroupTitleHeight(p.style==0&&p.OnGrid());
    float storedPitch=p.OnGrid()?GroupPitchX():LegacyGroupPitchX();
    float storedRowPitch=p.OnGrid()?GroupPitchY():LegacyGroupPitchY();
    // Legacy viewport rules still determine the outside rectangle.
    float minWidth=(p.OnGrid()?storedPitch:L.icon)+2*pad;
    float width=p.style?p.w*s:std::max(minWidth,std::min(p.w*s,L.width-(p.OnGrid()?0:32*s)));
    float page=g_settings.pages?std::floor(p.x*s/std::max(1,L.width))*L.width:0;
    float x=(p.style||p.OnGrid()||p.snapped)?p.x*s:page+std::clamp(p.x*s-page,16*s,std::max(16*s,L.width-width-16*s));
    float y=L.viewTop+p.y*s;
    float height=p.style?p.h*s:std::max(header+2*pad+storedRowPitch,p.h*s);
    if(!p.style&&g_settings.pages&&!p.OnGrid())height=std::min(height,std::max(header+2*pad+storedRowPitch,L.viewBottom-y));
    if(!p.style&&!p.OnGrid()&&p.columns) {
        int oldColumns=std::clamp((int)std::floor((width-2*pad+.01f)/storedPitch),1,p.columns);
        width=2*pad+oldColumns*storedPitch;
    }
    if(!p.style&&!p.OnGrid()&&p.rows) {
        int oldRows=std::max(1,(int)std::floor((height-header-2*pad+.01f)/storedRowPitch));
        height=header+2*pad+oldRows*storedRowPitch;
    }

    int columns=FittingGroupColumns(width);
    if(side)columns=p.gridW/2;
    else if(p.WholeCells())columns=p.gridW;
    else if(p.OnGrid())columns=std::max(1,(int)std::floor((width-2*pad+.01f)/GroupPitchX()));
    else if(p.columns)columns=std::min(columns,p.columns);
    else if(g_settings.columns>0)columns=std::min(columns,g_settings.columns);
    float fitRowPitch=p.OnGrid()?GroupPitchY():GroupIconPitchY();
    int visibleRows=side?(p.gridH-2)/4:p.WholeCells()?p.gridH-1:std::max(1,(int)std::floor((height-header-2*pad+.01f)/fitRowPitch));
    int count=group->collapsed?1:(int)group->apps.size();
    int totalRows=(count+columns-1)/columns;
    auto storedRow=g_groupRows.find(group);
    int row=std::clamp(storedRow==g_groupRows.end()?0:storedRow->second,0,std::max(0,totalRows-visibleRows));
    int first=row*columns;

    ExpandedGroupGeometry geometry;
    geometry.group=group;geometry.area={x,y,x+width,y+height};
    geometry.title={x+pad,y,x+width-pad,y+header};
    geometry.start=global+first;geometry.firstInGroup=first;
    geometry.count=std::min(std::max(0,count-first),visibleRows*columns);
    geometry.columns=columns;geometry.entryOrder=order;
    geometry.cellWidth=side?side:p.WholeCells()?GroupPitchX():GroupIconPitchX();
    geometry.rowPitch=side?side:p.WholeCells()?GroupPitchY():GroupIconPitchY();
    geometry.visibleRows=visibleRows;geometry.wholeCells=p.WholeCells()||side;geometry.squareCells=side>0;
    return geometry;
}
D2D1_POINT_2F GroupCellsOrigin(const ExpandedGroupGeometry& geometry) {
    if(geometry.wholeCells)return {geometry.area.left,geometry.title.bottom};
    return {(geometry.area.left+geometry.area.right-geometry.columns*geometry.cellWidth)/2,
            geometry.title.bottom+kGroupPadding*g_layout.scale};
}
Cell MakeExpandedGroupCell(const ExpandedGroupGeometry& geometry,int slot) {
    auto origin=GroupCellsOrigin(geometry);
    float x=origin.x+(slot%geometry.columns)*geometry.cellWidth;
    float y=origin.y+(slot/geometry.columns)*geometry.rowPitch;
    auto cell=geometry.squareCells?MakeSquareCell(x,y,geometry.cellWidth):
        MakeCell(x,y,geometry.cellWidth,geometry.rowPitch,0);
    float pad=geometry.wholeCells?0:kGroupPadding*g_layout.scale;
    for(auto rect:{&cell.label,&cell.highlight}) {
        rect->left=std::max(rect->left,geometry.area.left+pad);
        rect->right=std::min(rect->right,geometry.area.right-pad);
    }
    const auto& group=*geometry.group;
    const Tile collapsed{nullptr,geometry.group};
    return SpaceTileContents(group.collapsed?collapsed:group.apps[geometry.firstInGroup+slot],cell);
}
std::optional<ExpandedGroupGeometry> GroupResizePreviewGeometry() {
    const auto& gesture=g_groupGesture;
    if(!gesture.moved||!gesture.resize||!gesture.valid)return std::nullopt;
    auto current=std::find_if(g_groupGeometry.begin(),g_groupGeometry.end(),[&](const auto& geometry){
        return geometry.group==gesture.group&&geometry.start==gesture.activeStart;
    });
    if(current==g_groupGeometry.end())return std::nullopt;
    auto before=PositionFromArea(current->area);
    const auto& draft=gesture.draft;
    if(std::abs(before.w-draft.w)<.01f&&std::abs(before.h-draft.h)<.01f)return std::nullopt;
    return MakeFreeGroupGeometry(gesture.group,draft,current->start-current->firstInGroup,current->entryOrder);
}
void ComputeFreeGroups() {
    if(g_groupPositions.empty())return;
    auto& L=g_layout;
    float bottom=L.contentHeight,right=L.pages*(float)L.width;
    int global=0,order=0;
    for(const auto& tile:g_tiles)if(auto group=tile.folder;GroupVisible(group)) {
        auto found=g_groupPositions.find(group);
        if(found!=g_groupPositions.end()) {
            auto geometry=MakeFreeGroupGeometry(group,found->second,global,order);
            g_groupGeometry.push_back(geometry);
            for(int i=0;i<geometry.count&&geometry.start+i<(int)g_groupCells.size();++i) {
                g_groupCells[geometry.start+i]=MakeExpandedGroupCell(geometry,i);
                if(!group->collapsed)if(auto app=group->apps[geometry.firstInGroup+i].app)g_groupEntryOrder[app.get()]=order;
            }
            bottom=std::max(bottom,geometry.area.bottom-L.viewTop);
            right=std::max(right,geometry.area.right);
        }
        global+=group->collapsed?1:(int)group->apps.size();
        ++order;
    }
    L.contentHeight=bottom;
    L.pages=std::max(1,(int)std::ceil((right-.01f)/std::max(1,L.width)));
}
void ComputeWholeCellGroups() {
    auto& L=g_layout;
    const float w=GroupPitchX(),h=GroupPitchY();
    const int gapX=(int)std::ceil(g_settings.groupGap*L.scale/w);
    const int gapY=(int)std::ceil(g_settings.groupGap*L.scale/h);
    const int available=std::max(1,(int)std::floor(L.width/w));
    const int viewRows=std::max(2,(int)std::floor((L.viewBottom-L.viewTop)/h));
    int visibleGroups=(int)std::count_if(g_tiles.begin(),g_tiles.end(),[](const Tile& tile){return GroupVisible(tile.folder);});
    float preferredWidth=g_settings.groupWidth>0?g_settings.groupWidth*L.scale:
        visibleGroups<=1?L.width:400*L.scale;
    int wanted=std::min(available,std::max(1,(int)std::round(preferredWidth/w)));
    if(g_settings.columns>0)wanted=std::min(wanted,g_settings.columns);
    L.cellWidth=w;L.cellHeight=h;L.cellSpare=0;L.columns=std::min(wanted,available);
    L.contentHeight=0;L.pages=0;
    L.perPage=std::max(1,L.columns*(viewRows-1));
    g_groupGeometry.clear();
    g_groupEntryOrder.clear();
    g_groupCells.assign(g_visible.size(),{});
    ComputeFreeGroups();

    // Keep automatic groups clear of existing manually placed groups.
    int page=g_settings.pages&&!g_groupGeometry.empty()?L.pages:0;
    int y=g_settings.pages||g_groupGeometry.empty()?0:(int)std::ceil((L.contentHeight-.01f)/h)+gapY;
    int left=(int)std::ceil(page*L.width/w-.0001f),x=left,rowHeight=0;
    int global=0,order=0;
    for(const auto& tile:g_tiles)if(auto group=tile.folder;GroupVisible(group)) {
        int count=group->collapsed?1:(int)group->apps.size();
        if(!g_groupPositions.count(group)) {
            int offset=0;
            do {
                int right=(int)std::floor((page+1)*L.width/w+.0001f);
                int columns=std::min(wanted,std::max(1,right-left));
                int rows=std::max(1,(count-offset+columns-1)/columns);
                if(g_settings.pages)rows=std::min(rows,viewRows-1);
                int height=rows+1;
                if(x>left&&(g_settings.groupRows||x+columns>right)) {x=left;y+=rowHeight+gapY;rowHeight=0;}
                if(g_settings.pages&&y+height>viewRows) {
                    ++page;x=left=(int)std::ceil(page*L.width/w-.0001f);y=0;rowHeight=0;
                    right=(int)std::floor((page+1)*L.width/w+.0001f);
                    columns=std::min(wanted,std::max(1,right-left));
                    rows=std::clamp((count-offset+columns-1)/columns,1,viewRows-1);
                    height=rows+1;
                }
                int chunk=std::min(count-offset,rows*columns);
                ExpandedGroupGeometry geometry;
                geometry.group=group;geometry.area={x*w,L.viewTop+y*h,(x+columns)*w,L.viewTop+(y+height)*h};
                geometry.title=geometry.area;geometry.title.bottom=geometry.title.top+h;
                geometry.start=global+offset;geometry.firstInGroup=offset;geometry.count=chunk;
                geometry.columns=columns;geometry.visibleRows=rows;geometry.entryOrder=order;
                geometry.wholeCells=true;geometry.cellWidth=w;geometry.rowPitch=h;
                g_groupGeometry.push_back(geometry);
                for(int i=0;i<chunk;++i) {
                    int index=geometry.start+i;
                    g_groupCells[index]=MakeExpandedGroupCell(geometry,i);
                    if(auto app=g_visible[index].app)g_groupEntryOrder[app.get()]=order;
                }
                L.contentHeight=std::max(L.contentHeight,(y+height)*h);
                L.pages=std::max(L.pages,page+1);
                x+=columns+gapX;rowHeight=std::max(rowHeight,height);
                offset+=chunk;
            } while(offset<count);
        }
        global+=count;++order;
    }
    L.pages=std::max(1,L.pages);
}
void ComputeSquareGroups() {
    auto& L=g_layout;
    const float side=SquareCellSize(g_squareGridColumns);
    const float quarter=side/4;
    const int columnsPerPage=g_squareGridColumns;
    const int availableY=std::max(6,(int)std::floor((L.viewBottom-L.viewTop+.01f)/quarter));
    const int maxRows=(availableY-2)/4;
    const int gapX=(int)std::ceil(g_settings.groupGap*L.scale/(side/2));
    const int gapY=(int)std::ceil(g_settings.groupGap*L.scale/quarter);
    int groups=(int)std::count_if(g_tiles.begin(),g_tiles.end(),[](const Tile& tile){return GroupVisible(tile.folder);});
    float preferred=g_settings.groupWidth>0?g_settings.groupWidth*L.scale:
        (groups<=1||g_settings.groupRows)?columnsPerPage*side:400*L.scale;
    int columns=std::clamp((int)std::round(preferred/side),1,columnsPerPage);

    L.contentHeight=0;L.pages=0;
    g_groupGeometry.clear();g_groupEntryOrder.clear();g_groupCells.assign(g_visible.size(),{});
    ComputeFreeGroups();
    int page=g_settings.pages&&!g_groupGeometry.empty()?L.pages:0;
    int y=g_settings.pages||g_groupGeometry.empty()?0:(int)std::ceil((L.contentHeight-.01f)/quarter)+gapY;
    int x=0,rowHeight=0,global=0,order=0;
    for(const auto& tile:g_tiles)if(auto group=tile.folder;GroupVisible(group)) {
        int count=group->collapsed?1:(int)group->apps.size();
        if(!g_groupPositions.count(group)) {
            int offset=0;
            do {
                int rows=std::max(1,(count-offset+columns-1)/columns);
                if(g_settings.pages)rows=std::min(rows,maxRows);
                int height=rows*4+2;
                if(x&&(g_settings.groupRows||x+columns*2>columnsPerPage*2)) {x=0;y+=rowHeight+gapY;rowHeight=0;}
                if(g_settings.pages&&y+height>availableY) {++page;x=0;y=0;rowHeight=0;}
                auto position=GroupPosition{};
                position.style=5;position.snapped=true;position.gridX=page*columnsPerPage*2+x;
                position.gridY=y;position.gridW=columns*2;position.gridH=height;
                auto geometry=MakeFreeGroupGeometry(group,position,global,order);
                int chunk=std::min(count-offset,rows*columns);
                geometry.firstInGroup=offset;geometry.start=global+offset;geometry.count=chunk;
                g_groupGeometry.push_back(geometry);
                for(int i=0;i<chunk;++i) {
                    int index=geometry.start+i;
                    g_groupCells[index]=MakeExpandedGroupCell(geometry,i);
                    if(auto app=g_visible[index].app)g_groupEntryOrder[app.get()]=order;
                }
                L.contentHeight=std::max(L.contentHeight,(y+height)*quarter);
                L.pages=std::max(L.pages,page+1);
                x+=columns*2+gapX;rowHeight=std::max(rowHeight,height);
                offset+=chunk;
            } while(offset<count);
        }
        global+=count;++order;
    }
    L.pages=std::max(1,L.pages);
}
void ComputeGroups() {
    if(g_squareGridColumns){ComputeSquareGroups();return;}
    if(g_wholeCellLayout){ComputeWholeCellGroups();return;}
    auto& L = g_layout;
    float s = L.scale, gap = g_settings.groupGap * s, pad = 12 * s, header = LegacyGroupTitleHeight();
    float margin = std::max(24 * s, L.width * 0.045f);
    float available = std::max(L.icon + 2 * pad, L.width - 2 * margin);
    float wanted = g_settings.groupWidth > 0 ? g_settings.groupWidth * s :
        (g_tiles.size() <= 1 ? available : 400 * s);
    wanted = std::max(wanted, L.icon + 2 * pad);
    int across = g_settings.groupRows ? 1 : std::max(1, (int)((available + gap) / (wanted + gap)));
    float groupWidth = (available - (across - 1) * gap) / across;
    int fitColumns=FittingGroupColumns(groupWidth);
    int columns=g_settings.columns>0?std::min(g_settings.columns,fitColumns):fitColumns;
    L.columns=columns;
    L.cellWidth=GroupIconPitchX();
    L.cellHeight=GroupIconPitchY();
    L.cellSpare = 0;
    float viewHeight = std::max(L.cellHeight + header + 2 * pad, L.viewBottom - L.viewTop);
    int maxRows = std::max(1, (int)((viewHeight - header - 2 * pad) / L.cellHeight));
    int capacity = columns * maxRows;
    g_groupGeometry.clear();
    g_groupEntryOrder.clear();
    g_groupCells.assign(g_visible.size(), {});
    float y = 0, rowHeight = 0;
    int col = 0, page = 0, global = 0, order = 0;
    bool custom=g_freeGroupLayout||g_groupEditMode;
    bool allCustom=custom&&std::all_of(g_tiles.begin(),g_tiles.end(),[](const Tile& tile){
        return !GroupVisible(tile.folder)||g_groupPositions.count(tile.folder);
    });
    for (const auto& tile : g_tiles) {
        if (!GroupVisible(tile.folder)||allCustom) continue;
        int count = tile.folder->collapsed?1:(int)tile.folder->apps.size(), offset = 0;
        bool override=custom&&g_groupPositions.count(tile.folder);
        do {
            int chunk = g_settings.pages ? std::min(capacity, count - offset) : count;
            int rows = std::max(1, (chunk + columns - 1) / columns);
            float height = header + 2 * pad + rows * L.cellHeight;
            if (col >= across) { col = 0; y += rowHeight + gap; rowHeight = 0; }
            if (g_settings.pages && y > 0 && y + height > viewHeight) {
                ++page; col = 0; y = 0; rowHeight = 0;
            }
            float x = margin + col * (groupWidth + gap) + page * (float)L.width;
            float top = L.viewTop + y;
            ExpandedGroupGeometry geometry;
            geometry.group = tile.folder;
            geometry.area = {x, top, x + groupWidth, top + height};
            geometry.title = {x + pad, top, x + groupWidth - pad, top + header};
            geometry.start = global + offset; geometry.firstInGroup = offset;
            geometry.count = chunk; geometry.columns = columns;
            geometry.cellWidth=L.cellWidth;geometry.rowPitch=L.cellHeight;geometry.visibleRows=rows;
            geometry.entryOrder = order;
            if(!override)g_groupGeometry.push_back(geometry);
            for (int i = 0; !override&&i < chunk; i++) {
                int index = global + offset + i;
                if (index < (int)g_groupCells.size()) g_groupCells[index] = MakeExpandedGroupCell(geometry,i);
                if (index < (int)g_visible.size() && g_visible[index].app) g_groupEntryOrder[g_visible[index].app.get()]=order;
            }
            ++col; rowHeight = std::max(rowHeight, height);
            offset += chunk;
        } while (offset < count);
        global += tile.folder->collapsed?1:count;
        ++order;
    }
    L.contentHeight = y + rowHeight;
    L.pages = allCustom?0:page+1;
    L.perPage = std::max(1, capacity * across);
    if(custom)ComputeFreeGroups();
    L.pages=std::max(1,L.pages);
}
float MaxPagePan(int page) {
    if(!g_settings.pages)return 0;
    if(!GroupedView())return g_visible.empty()?0:std::max(0.f,g_layout.viewTop+
        g_layout.visibleRows*g_layout.cellHeight-g_layout.viewBottom);
    if(!g_squareGridColumns)return 0;
    float start=page*(float)g_layout.width,end=start+g_layout.width,bottom=g_layout.viewBottom;
    for(const auto& geometry:g_groupGeometry)
        if(geometry.area.left>=start&&geometry.area.left<end)bottom=std::max(bottom,geometry.area.bottom);
    return bottom-g_layout.viewBottom;
}
Cell ProjectGroupCell(Cell c) {
    c.icon = ProjectGroupRect(c.icon);
    c.label = ProjectGroupRect(c.label);
    c.highlight = ProjectGroupRect(c.highlight);
    c.unit = ProjectGroupRect(c.unit);
    return c;
}
Cell GroupCellAt(int index) {
    if(index<0||index>=(int)g_groupCells.size())return {};
    return ProjectGroupCell(g_groupCells[index]);
}
bool GroupIconFits(const Cell& cell,const D2D1_RECT_F& area) {
    const auto& icon=cell.icon;
    return icon.left>=area.left&&icon.top>=area.top&&icon.right<=area.right&&icon.bottom<=area.bottom;
}
void EnsureFreeGroupVisible(int index) {
    if(!g_freeGroupLayout)return;
    for(const auto& geometry:g_groupGeometry) {
        int base=geometry.start-geometry.firstInGroup;
        if(index<base||index>=base+(geometry.group->collapsed?1:(int)geometry.group->apps.size()))continue;
        int row=(index-base)/geometry.columns;
        int next=std::clamp(g_groupRows[geometry.group],row-geometry.visibleRows+1,row);
        if(next!=g_groupRows[geometry.group]){g_groupRows[geometry.group]=next;ComputeGroups();}
        break;
    }
}
int GroupIndexAt(int x, int y) {
    if (y < g_layout.viewTop || y >= g_layout.viewBottom) return -1;
    for (int i = 0; i < (int)g_groupCells.size(); i++)
        if (GroupContains(GroupCellAt(i).highlight, (float)x, (float)y)) return i;
    return -1;
}
int GroupAtPoint(int x, int y, bool titleOnly = false) {
    if (!GroupedView() || y < g_layout.viewTop || y >= g_layout.viewBottom) return -1;
    for (int i = 0; i < (int)g_groupGeometry.size(); i++)
        if (GroupContains((titleOnly ? ProjectGroupRect(g_groupGeometry[i].title) : VisualGroupRect(g_groupGeometry[i])), (float)x, (float)y)) return i;
    return -1;
}
// Group headers render like the app labels: white semi-bold text over a soft
// shadow, pre-rendered once per name and size. Keyed by name: a rename simply
// misses, and two same-named groups can safely share one bitmap.
struct GroupTitleRender {
    std::wstring name;
    float width=0,height=0;
    UINT version=0;
    bool shrink=false;
    ComPtr<IDWriteTextLayout> layout;
    LabelShadowBitmap shadow;
};
[[clang::no_destroy]] std::map<std::wstring, GroupTitleRender> g_groupTitleRenders;

void DrawGroupHeaders(bool activeOnly,const ExpandedGroupGeometry* resizePreview) {
    const bool flying = FlyInActive();
    for(const auto& current:g_groupGeometry) {
        bool active=g_groupGesture.moved&&current.group==g_groupGesture.group&&
            current.start==g_groupGesture.activeStart;
        if(active!=activeOnly||(g_groupGesture.moved&&current.group==g_groupGesture.group&&!active))continue;
        const auto& geometry=active&&resizePreview?*resizePreview:current;
        auto area=VisualGroupRect(geometry);
        float pad=geometry.wholeCells?0:kGroupPadding*g_layout.scale;
        auto title=D2D1::RectF(area.left+pad,area.top,area.right-pad,
            std::min(area.bottom,area.top+geometry.title.bottom-geometry.title.top));
        if (area.bottom < g_layout.viewTop || area.top > g_layout.viewBottom || area.right < 0 || area.left > g_layout.width) continue;
        if(g_groupEditMode) {
            float s=g_layout.scale;
            // A small inset keeps the outlines distinct when group bounds touch.
            float inset=std::min({2.5f*s,(area.right-area.left)/4,(area.bottom-area.top)/4});
            auto outline=D2D1::RectF(area.left+inset,area.top+inset,area.right-inset,area.bottom-inset);
            g_brush->SetColor(D2D1::ColorF(active?0xEAF5FF:0x587187,.96f));
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(outline,5*s,5*s),g_brush.Get(),1.6f*s);
            auto color=D2D1::ColorF(0xFFFFFF);
            float glow=geometry.group==g_groupResizeHover?g_groupResizeGlow:0;
            if(geometry.group==g_groupGesture.group&&g_groupGesture.resize)glow=1;
            float midX=area.right-12*s,midY=area.bottom-12*s,arm=(4+1.5f*glow)*s;
            color.a=.52f+.48f*glow;g_brush->SetColor(color);
            for(float sign:{-1.f,1.f}) {
                float x=midX+sign*arm,y=midY+sign*arm;
                auto end=D2D1::Point2F(x,y);
                g_target->DrawLine(D2D1::Point2F(midX,midY),end,g_brush.Get(),1.4f*s);
                g_target->DrawLine(end,D2D1::Point2F(x-sign*arm*.8f,y),g_brush.Get(),1.4f*s);
                g_target->DrawLine(end,D2D1::Point2F(x,y-sign*arm*.8f),g_brush.Get(),1.4f*s);
            }
        }
        float progress=GroupEntryProgress(geometry.entryOrder);
        float shift=(1-progress)*30*g_layout.scale;
        title.left+=shift; title.right+=shift;
        float dy = 0.f;
        if (flying) {
            auto anchor = geometry.count ? CellAt(geometry.start).icon : title;
            auto pose = FlyInPose({(anchor.left+anchor.right)*.5f, (anchor.top+anchor.bottom)*.5f},
                                   g_settings.iconSize*g_layout.scale);
            progress = pose.opacity * launchpad_fly_in::SmoothStep((g_appear - .13f) / .36f);
            if (progress <= 0) continue;
            // Tie the header to its first tile's phase with a shorter travel.
            shift = .40f * pose.dx;
            dy = .40f * pose.dy;
            title.left += shift; title.right += shift;
            title.top += dy; title.bottom += dy;
        }
        std::wstring name=geometry.group->name;
        if (!geometry.group->unavailable.empty()) name+=L" · "+std::to_wstring(geometry.group->unavailable.size())+Tr(L" unavailable");
        float titleOpacity=geometry.group==g_groupDropFolder||geometry.group==g_groupHeaderDragging?1.f:.90f;
        float titleHeight=title.bottom-title.top;
        if(!g_groupFormat||titleHeight<=0||title.right<=title.left)continue;
        float fontSize=g_groupFormat->GetFontSize();
        bool shrink=geometry.squareCells&&titleHeight<fontSize*1.25f;
        auto& cached=g_groupTitleRenders[name];
        float titleWidth=title.right-title.left;
        if(!cached.layout||cached.version!=g_formatVersion||cached.name!=name||
           cached.width!=titleWidth||cached.height!=titleHeight||cached.shrink!=shrink) {
            cached.name=name;cached.width=titleWidth;cached.height=titleHeight;
            cached.version=g_formatVersion;cached.shrink=shrink;
            cached.layout.Reset();cached.shadow.bitmap.Reset();
            if(SUCCEEDED(g_dwrite->CreateTextLayout(name.c_str(),(UINT32)name.size(),g_groupFormat.Get(),
                titleWidth,titleHeight,&cached.layout))) {
                if(shrink)cached.layout->SetFontSize(std::max(1.f,titleHeight/1.25f),{0,(UINT32)name.size()});
                RenderTextWithShadow(cached.layout.Get(),titleWidth,titleHeight,cached.shadow);
            } else cached.layout.Reset();
        }
        if (cached.shadow.bitmap) {
            g_target->DrawBitmap(cached.shadow.bitmap.Get(),
                D2D1::RectF(title.left-cached.shadow.padX,title.top-cached.shadow.padTop,
                            title.right+cached.shadow.padX,title.bottom+cached.shadow.padBottom),
                progress*titleOpacity,D2D1_INTERPOLATION_MODE_LINEAR);
        } else {
            g_brush->SetColor(D2D1::ColorF(0xFFFFFF,progress*titleOpacity));
            if (cached.layout) g_target->DrawTextLayout({title.left,title.top},cached.layout.Get(),g_brush.Get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
            else g_target->DrawText(name.c_str(),(UINT32)name.size(),g_groupFormat.Get(),title,g_brush.Get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        if (!geometry.count) {
            auto hint = area; hint.top = title.bottom;
            if (flying) {
                hint.left += shift; hint.right += shift; hint.bottom += dy;
            }
            g_brush->SetColor(D2D1::ColorF(0xFFFFFF,flying?progress:1.f));
            PCWSTR text = Tr(g_visible.empty() ? L"Search for an app, then use Move to group" : L"Drop apps here");
            g_target->DrawText(text, (UINT32)wcslen(text), g_tipFormat.Get(), hint, g_brush.Get());
        }
        if(g_freeGroupLayout && !geometry.group->collapsed && (int)geometry.group->apps.size()>geometry.visibleRows*geometry.columns) {
            auto hint=area;hint.top=hint.bottom-16*g_layout.scale;
            if (flying) {
                hint.left += shift; hint.right += shift; hint.top += dy; hint.bottom += dy;
            }
            std::wstring text=std::to_wstring(geometry.firstInGroup+1)+L"–"+std::to_wstring(geometry.firstInGroup+geometry.count)+L" / "+std::to_wstring(geometry.group->apps.size());
            g_brush->SetColor(D2D1::ColorF(0xFFFFFF,flying?progress:1.f));g_target->DrawText(text.c_str(),(UINT32)text.size(),g_tipFormat.Get(),hint,g_brush.Get());
        }
        if (geometry.group == g_groupDropFolder && g_groupDropSlot >= geometry.firstInGroup &&
            g_groupDropSlot <= geometry.firstInGroup + geometry.count) {
            int slot = g_groupDropSlot - geometry.firstInGroup;
            auto origin=GroupCellsOrigin(geometry);
            float x=std::max(area.left,origin.x+area.left-geometry.area.left+(slot%geometry.columns)*geometry.cellWidth);
            float y=origin.y+area.top-geometry.area.top+(slot/geometry.columns)*geometry.rowPitch;
            auto color=D2D1::ColorF(0xFFFFFF,.75f);
            Fill({x,y,x+3*g_layout.scale,y+g_layout.icon+16*g_layout.scale},color,1.5f*g_layout.scale);
        }
    }
}
void DrawGroupGrid() {
    if(!g_groupEditMode||!g_squareGridColumns)return;
    const auto& L=g_layout;
    float side=SquareCellSize(g_squareGridColumns),margin=GridMargin(),s=L.scale;
    float scroll=g_settings.pages?g_groupPagePan:g_scroll;
    int row=std::max(0,(int)std::ceil(scroll/side));
    g_brush->SetColor(D2D1::ColorF(0x64778A,.32f));
    int page=g_settings.pages?std::max(0,(int)(g_scroll/L.width)):0;
    for(int i=0;i<(g_settings.pages?2:1);++i) {
        float left=margin+(g_settings.pages?(page+i)*L.width-g_scroll:0);
        float right=left+g_squareGridColumns*side;
        for(int column=0;column<=g_squareGridColumns;++column) {
            float x=left+column*side;
            g_target->DrawLine({x,L.viewTop},{x,L.viewBottom},g_brush.Get(),s);
        }
        for(float y=L.viewTop+row*side-scroll;y<=L.viewBottom;y+=side) {
            g_target->DrawLine({left,y},{right,y},g_brush.Get(),s);
        }
    }
}
void DrawGroupEditPreview() {
    const auto& gesture=g_groupGesture;
    if(!g_groupEditMode||!gesture.group||!gesture.moved)return;
    float s=g_layout.scale;
    auto color=D2D1::ColorF(gesture.valid?0xFFFFFF:0xC8C8C8,.75f);
    auto r=ProjectGroupRect(GroupPositionRect(gesture.continuous));
    if(gesture.valid) {
        auto target=ProjectGroupRect(GroupPositionRect(gesture.draft));
        float distance=std::max({std::abs(r.left-target.left),std::abs(r.top-target.top),
                                 std::abs(r.right-target.right),std::abs(r.bottom-target.bottom)});
        if(distance>4*s) {
            g_brush->SetColor(D2D1::ColorF(0x71C7FF));
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(target,5*s,5*s),g_brush.Get(),1.8f*s);
        }
    }
    if(!gesture.resize&&gesture.valid)return;
    std::wstring text=gesture.valid?
        std::to_wstring(gesture.draft.gridW/2)+L" × "+std::to_wstring((gesture.draft.gridH-2)/4):
        Tr(L"Invalid placement: release to keep previous position");
    float width=std::min((gesture.valid?82.f:252.f)*s,g_layout.width-16*s);
    float right=std::clamp(r.right-8*s,width+8*s,g_layout.width-8*s);
    float bottom=std::clamp(r.bottom-8*s,g_layout.viewTop+34*s,g_layout.viewBottom-4*s);
    auto label=D2D1::RectF(right-width,bottom-26*s,right,bottom);
    Fill(label,D2D1::ColorF(0x303030,.92f),6*s);
    g_brush->SetColor(color);
    g_target->DrawText(text.c_str(),(UINT32)text.size(),g_tipFormat.Get(),label,g_brush.Get(),
                       D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

Cell GroupVisualCell(const ExpandedGroupGeometry& geometry,Cell c) {
    auto visual=VisualGroupRect(geometry);
    auto base=ProjectGroupRect(geometry.area);
    float dx=visual.left-base.left,dy=visual.top-base.top;
    for(auto rect:{&c.icon,&c.label,&c.highlight,&c.unit}) {
        rect->left+=dx;rect->right+=dx;rect->top+=dy;rect->bottom+=dy;
    }
    return c;
}


////////////////////////////////////////////////////////////////////////////////
// Hover glow
//
// A soft halo in the accent colour behind the icon of the hovered or
// keyboard-selected tile. Blurring is expensive, so one bitmap per icon size
// and spread is built once and reused; each tile only fades its opacity,
// eased over ~150 ms.

void StartFrames();

struct GlowFade {
    const void* key = nullptr;  // The tile's app or folder.
    float alpha = 0;            // 0..1, eased when drawn.
    double last = 0;
    D2D1_POINT_2F light{.5f,.5f};
};
constexpr int kGlowSlots = 4;
[[clang::no_destroy]] GlowFade g_glowFades[kGlowSlots];
// A single client-space pointer drives the whole view, like Meapri's
// PointerField (MIT). The edge shading in this renderer is implemented in D2D.
std::optional<D2D1_POINT_2F> g_glassPointer, g_glassScenePointer;
bool g_glassClaimed=false;

bool g_glowAnimating;  // A fade is under way; OnFrame keeps rendering.

// Advances the tile's fade and returns the eased alpha to draw with.
GlowFade* UpdateGlow(const void* key, bool on) {
    GlowFade* slot = nullptr;
    for (auto& s : g_glowFades) {
        if (s.key == key) {
            slot = &s;
            break;
        }
    }
    if (!slot && on) {
        for (auto& s : g_glowFades) {
            if (!s.key) {
                slot = &s;
                break;
            }
        }
        if (!slot) {
            for (auto& s : g_glowFades) {
                if (!slot || s.alpha < slot->alpha) {
                    slot = &s;
                }
            }
        }
        slot->key = key;
        slot->alpha = 0;
        slot->last = Now();
    }
    if (!slot) {
        return nullptr;
    }
    double now = Now();
    float dt = (float)std::clamp(now - slot->last, 0.0, 0.15);
    if (!Animate()) {
        dt = 1.0f;
    }
    slot->last = now;
    slot->alpha = std::clamp(slot->alpha + (on ? dt : -dt) / 0.15f, 0.0f, 1.0f);
    if ((on&&slot->alpha<1)||(!on&&slot->alpha>0)) {
        g_glowAnimating = true;
        StartFrames();
    } else if (!on) {
        slot->key = nullptr;  // Faded out: freed for the next tile.
    }
    return slot;
}

bool InRect(const D2D1_RECT_F& r, float x, float y) {
    return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}

// Position in the grid under the point (window coordinates), counting the
// places past the last icon, or -1 outside the rows.
int GridIndexAt(int px, int py) {
    if (GroupedView()) return GroupIndexAt(px, py);
    const Layout& L = g_layout;
    float x = (float)px, y = (float)py;
    int page = 0;
    if (g_settings.pages) {
        x += std::round(g_scroll);
        page = (int)std::floor(x / L.width);
        x -= page * (float)L.width;
    }
    if (y < L.viewTop || y >= L.viewBottom || x < L.gridLeft ||
        x >= L.gridLeft + L.cellWidth * L.columns || page < 0) {
        return -1;
    }
    int col = (int)((x - L.gridLeft) / L.cellWidth);
    int row = (int)((y - L.viewTop +
                     (g_settings.pages ? g_groupPagePan : std::round(g_scroll))) /
                    L.cellHeight);
    if(row<0||(g_settings.pages&&row>=L.visibleRows))return -1;
    return page*L.perPage+row*L.columns+col;
}

// An open folder: a panel over the grid with its name and its apps.
[[clang::no_destroy]] std::shared_ptr<Folder> g_openFolder;
bool g_folderRenaming = false;
[[clang::no_destroy]] std::wstring g_folderOriginalName;
void SaveFolderName() {
    if(g_openFolder&&g_openFolder->name!=g_folderOriginalName&&SaveTiles())
        g_folderOriginalName=g_openFolder->name;
}
bool g_folderTitleHover = false;
// The folder fading out after a close, and the open/close progress (0 to 1).
[[clang::no_destroy]] std::shared_ptr<Folder> g_closingFolder;
float g_folderAnim = 1;
int g_folderRow;  // First row shown, when not all of them fit.

struct FolderLayout {
    D2D1_RECT_F panel, title;
    float left, top, cellHeight;  // Of the icons.
    int columns, rows, visibleRows;
};

FolderLayout GetFolderLayout() {
    const Layout& L = g_layout;
    float s = L.scale;
    FolderLayout F{};
    int count = g_openFolder ? (int)g_openFolder->apps.size() : 0;
    F.columns = std::min(L.columns, std::max(3, std::min(count, 6)));
    F.rows = std::max(1, (count + F.columns - 1) / F.columns);
    F.cellHeight = L.cellHeight - L.cellSpare;
    float pad = std::round(20 * s), titleHeight = std::round(56 * s);
    int fit = std::max(1, (int)((L.viewBottom - L.viewTop - titleHeight - pad) /
                                F.cellHeight));
    F.visibleRows = std::min(F.rows, fit);
    float width = F.columns * L.cellWidth + 2 * pad;
    float height = titleHeight + F.visibleRows * F.cellHeight + pad;
    float left = std::round((L.width - width) / 2);
    float top = std::round(
        std::max(L.viewTop, (L.viewTop + L.viewBottom - height) / 2));
    F.panel = D2D1::RectF(left, top, left + width, top + height);
    F.title = D2D1::RectF(left + pad, top + pad / 2, left + width - pad,
                          top + titleHeight);
    F.left = left + pad;
    F.top = top + titleHeight;
    return F;
}

Cell FolderCellAt(int index) {
    FolderLayout F = GetFolderLayout();
    return SpaceTileContents(g_openFolder->apps[index],
        MakeCell(F.left + (index % F.columns) * g_layout.cellWidth,
                 F.top + (index / F.columns - g_folderRow) * F.cellHeight));
}

int FolderIndexAt(int px, int py) {
    FolderLayout F = GetFolderLayout();
    float x = px - F.left, y = py - F.top;
    if (x < 0 || x >= F.columns * g_layout.cellWidth || y < 0 ||
        y >= F.visibleRows * F.cellHeight) {
        return -1;
    }
    return ((int)(y / F.cellHeight) + g_folderRow) * F.columns +
           (int)(x / g_layout.cellWidth);
}

// What takes the pointer and the keys: the open folder, or the grid.
const Tiles& Items() {
    return g_openFolder ? g_openFolder->apps : g_visible;
}

int Columns() {
    return g_openFolder ? GetFolderLayout().columns : g_layout.columns;
}

Cell ItemCell(int index) {
    return g_openFolder ? FolderCellAt(index) : CellAt(index);
}

// Position in Items() of the icon under the point, or -1.
int ItemAt(int x, int y) {
    int index = g_openFolder ? FolderIndexAt(x, y) : GridIndexAt(x, y);
    if (index < 0 || index >= (int)Items().size() ||
        !InRect(ItemCell(index).highlight, (float)x, (float)y)) {
        return -1;
    }
    return index;
}

// While dragging: the place under the point (the last one when past the
// end), or -1.
int SlotAt(int x, int y) {
    int index = g_openFolder ? FolderIndexAt(x, y) : GridIndexAt(x, y);
    return index < 0 ? -1 : std::min(index, (int)Items().size() - 1);
}

// The page dot under the point, or -1. The click area is larger than the dot.
int PageDotAt(int px, int py) {
    const Layout& L = g_layout;
    if (!g_settings.pages || L.pages < 2) {
        return -1;
    }
    float step = std::min(20 * L.scale, std::max(2.0f, (L.width - 48 * L.scale) / L.pages));
    float left = L.width / 2.0f - L.pages * step / 2;
    float x = (float)px, y = (float)py;
    if (std::abs(y - L.dotsY) > 14 * L.scale || x < left ||
        x >= left + L.pages * step) {
        return -1;
    }
    return (int)((x - left) / step);
}

////////////////////////////////////////////////////////////////////////////////
// Screen state and drawing

bool g_shown;        // The windows are on screen (opening, open or closing).
bool g_closing;
float g_appear;      // 0 closed, 1 open.
double g_appearVelocity;
float g_wiggle;      // Edit mode envelope: ramps to 1 while an icon is held.
double g_lastFrame;
HANDLE g_frameWake = nullptr;
double g_refreshHz = 60, g_nextFrameDeadline = 0;

void UpdateFrameRate(HMONITOR monitor) {
    MONITORINFOEXW info{}; info.cbSize = sizeof(info);
    DEVMODEW mode{}; mode.dmSize = sizeof(mode);
    g_refreshHz = 60;
    if (GetMonitorInfoW(monitor, &info) && EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &mode) &&
        mode.dmDisplayFrequency >= 24) g_refreshHz = std::clamp((double)mode.dmDisplayFrequency, 24.0, 1000.0);
    // CCD provides fractional refresh rates (for example 60000/1001) that
    // EnumDisplaySettings rounds to an integer. This only queries the display.
    for (int attempt=0; attempt<3; attempt++) {
        UINT32 pathCount=0,modeCount=0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS,&pathCount,&modeCount)!=ERROR_SUCCESS) break;
        std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
        LONG result=QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS,&pathCount,paths.data(),&modeCount,modes.data(),nullptr);
        if (result==ERROR_INSUFFICIENT_BUFFER) continue;
        if (result!=ERROR_SUCCESS) break;
        for (UINT32 i=0;i<pathCount;i++) {
            DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
            source.header.type=DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
            source.header.size=sizeof(source); source.header.adapterId=paths[i].sourceInfo.adapterId; source.header.id=paths[i].sourceInfo.id;
            auto rate=paths[i].targetInfo.refreshRate;
            if (DisplayConfigGetDeviceInfo(&source.header)==ERROR_SUCCESS && wcscmp(source.viewGdiDeviceName,info.szDevice)==0 && rate.Denominator) {
                double hz=(double)rate.Numerator/rate.Denominator;
                if (hz>=24 && hz<=1000) g_refreshHz=hz;
                break;
            }
        }
        break;
    }
    if (g_settings.frameRateLimit > 0) g_refreshHz = std::min(g_refreshHz, (double)g_settings.frameRateLimit);
}

void ArmNextFrame(bool reset) {
    if (!g_frameWake) return;
    double now = Now(), period = 1.0 / g_refreshHz;
    g_nextFrameDeadline = reset ? now + period : g_nextFrameDeadline + period;
    if (g_nextFrameDeadline <= now) g_nextFrameDeadline = now + period;
    LARGE_INTEGER due;
    due.QuadPart = -(LONGLONG)std::max(1.0, std::ceil((g_nextFrameDeadline - now) * 10000000.0));
    SetWaitableTimer(g_frameWake, &due, 0, nullptr, nullptr, FALSE);
}
double g_lastClosed;  // When it closed by losing focus.
// One keystroke can reach this window twice: the keyboard hook reports the tap
// and the launcher's own show request follows it. The two are one toggle, so
// whichever lands first acts and the other is dropped inside this window.
double g_keyToggleAt = -1000;
bool g_frameTimer;
int g_hover = -1, g_selected = -1, g_pressed = -1;
bool g_pressedEmpty;
bool g_trackingMouse;
bool g_searchHover;   // The pointer rests on the search bar.
bool g_clearHover;    // ... on the clear (x) button.
float g_searchGlow;   // 0..1, the bar's eased accent glow.

// Dragging an icon: it keeps a place in the grid or folder, drawn empty, and
// moves to the place the pointer rests on.
struct Drag {
    bool pending = false;  // Pressed on an icon; a drag once it moves.
    bool active = false;
    POINT start{}, at{};
    double pressTime = 0;  // When the press started, for the hold time.
    D2D1_POINT_2F grab{};  // Pointer minus the icon's corner.
    Tile tile;
    int target = -1;  // The icon to drop it into (makes or fills a folder).
    int slot = -1;    // The place under the pointer.
    int edge = 0;     // -1 or 1 while resting at an edge: flips or scrolls.
    bool inPanel = false;  // Has been inside the open folder.
    double targetTime = 0, slotTime = 0, edgeTime = 0;
    std::vector<std::wstring> backup;  // The arrangement before, for Esc.
};
[[clang::no_destroy]] Drag g_drag;
using AppSelection = std::vector<std::shared_ptr<App>>;
[[clang::no_destroy]] AppSelection g_appSelection;
[[clang::no_destroy]] std::unordered_set<std::wstring> g_selectedAppKeys;
struct SelectionPointer {
    AppSelection base;
    std::unordered_set<std::wstring> baseKeys;
    std::shared_ptr<App> app;
    POINT start{}, end{};
    bool pending=false, active=false;
};
[[clang::no_destroy]] SelectionPointer g_selectionPointer;
float g_batchBlend=0;
int g_batchPressed=-1,g_batchHover=-1;
size_t g_batchDisplayCount=0;
bool g_batchLaunching=false;

auto FindSelectedApp(const AppSelection& selection,const App& app) {
    return std::find_if(selection.begin(),selection.end(),[&](const auto& item){
        return item.get()==&app||AppKey(*item)==AppKey(app);
    });
}
bool AppSelected(const App& app) {
    return !g_selectedAppKeys.empty()&&g_selectedAppKeys.contains(AppKey(app));
}
void SelectionChanged() {
    g_selectedAppKeys.clear();
    for(const auto& app:g_appSelection)g_selectedAppKeys.insert(AppKey(*app));
    if(!g_appSelection.empty())g_batchDisplayCount=g_appSelection.size();
    if(!Animate())g_batchBlend=g_appSelection.empty()?0.f:1.f;
    StartFrames();
}
void ClearAppSelection() {
    g_selectionPointer={};
    g_batchPressed=g_batchHover=-1;
    if(g_appSelection.empty())return;
    g_appSelection.clear();
    SelectionChanged();
}
void ToggleAppSelection(const std::shared_ptr<App>& app) {
    auto found=FindSelectedApp(g_appSelection,*app);
    if(found==g_appSelection.end())g_appSelection.push_back(app);
    else g_appSelection.erase(found);
    SelectionChanged();
}
D2D1_RECT_F SelectionRect() {
    const auto& p=g_selectionPointer;
    return {(float)std::min(p.start.x,p.end.x),(float)std::min(p.start.y,p.end.y),
            (float)std::max(p.start.x,p.end.x),(float)std::max(p.start.y,p.end.y)};
}
D2D1_RECT_F SelectionViewport() {
    if(!g_openFolder)return {0,g_layout.viewTop,(float)g_layout.width,g_layout.viewBottom};
    auto f=GetFolderLayout();
    return {f.left,f.top,f.panel.right,f.top+f.visibleRows*f.cellHeight};
}
D2D1_RECT_F SelectionIntersection(const D2D1_RECT_F& a,const D2D1_RECT_F& b) {
    return {std::max(a.left,b.left),std::max(a.top,b.top),
            std::min(a.right,b.right),std::min(a.bottom,b.bottom)};
}
bool SelectionHasArea(const D2D1_RECT_F& r) {
    return r.right>r.left&&r.bottom>r.top;
}
bool MoveAppSelection(int x,int y) {
    auto& pointer=g_selectionPointer;
    if(!pointer.pending)return false;
    pointer.end={x,y};
    if(!pointer.active&&std::abs(x-pointer.start.x)<GetSystemMetrics(SM_CXDRAG)&&
       std::abs(y-pointer.start.y)<GetSystemMetrics(SM_CYDRAG))return true;
    pointer.active=true;
    auto rect=SelectionIntersection(SelectionRect(),SelectionViewport());
    auto selection=pointer.base;
    if(SelectionHasArea(rect)) {
        auto keys=pointer.baseKeys;
        const auto& items=Items();
        for(int i=0;i<(int)items.size();i++) {
            if(!items[i].app||!SelectionHasArea(SelectionIntersection(rect,ItemCell(i).highlight)))continue;
            if(keys.insert(AppKey(*items[i].app)).second)selection.push_back(items[i].app);
        }
    }
    if(selection!=g_appSelection) {
        g_appSelection=std::move(selection);
        SelectionChanged();
    }
    StartFrames();return true;
}

bool g_modal;  // A dialog of the grid is open: losing the focus is expected.

#ifndef SEAMLESS_PREVIEW
void ResetTaskbarStartButtons(bool ownRequest = false);
void ClearStartButtonLatch(const wchar_t* why);  // launchpad_start_xaml.h
#endif

////////////////////////////////////////////////////////////////////////////////
// Popup menus
//
// Every menu the grid opens is drawn by the mod instead of the system's #32768
// menu class: a topmost layered window with rounded corners, a blurred copy of
// whatever sits behind it, the wallpaper tint and the same light veil and white
// rim the search box and the power dock wear. HMENU stays the command source
// (every caller passes TPM_RETURNCMD), so TrackLaunchpadPopupMenu is the only
// entry point that had to change.

namespace LaunchpadMenu {

constexpr WCHAR kMenuClass[] = L"RYJSLaunchpadMenu";

// A window class outlives the image that registered it. Reusing one left by
// an earlier build dispatches into code that is no longer mapped, while
// refusing to register at all removed every menu right after an in-place
// update. Each loaded image claims a class name of its own instead, so a new
// build never inherits the old window procedure.
const WCHAR* MenuClassName() {
    static WCHAR name[80]{};
    if (!name[0]) {
        swprintf_s(name, L"%ls_%p_%llu", kMenuClass, (void*)&__ImageBase,
                   (unsigned long long)GetTickCount64());
        name[ARRAYSIZE(name) - 1] = 0;
    }
    return name;
}
constexpr UINT kAnimationTimer = 1;
constexpr UINT kSubmenuTimer = 2;
constexpr UINT kSubmenuCloseTimer = 3;
constexpr int kAnimationMs = 120;
constexpr int kSubmenuDelayMs = 140;
// The only path from a row to its submenu is across the row itself, so the
// pointer must not lose the submenu while it is still there. Leaving the row
// for another one is a different story, but a hand rarely moves in a straight
// line across a 32 DIP row: the submenu gets this much grace before it goes,
// and returning to the row (or reaching the submenu) cancels the close.
constexpr int kSubmenuCloseDelayMs = 320;

// One row of a menu. Built from the HMENU the caller filled in, so the command
// ids, the disabled states and the submenus all stay exactly as they were.
struct Node {
    std::wstring text;
    std::wstring hint;  // The part after a tab, drawn right aligned.
    UINT id = 0;
    bool separator = false;
    bool enabled = true;
    bool checked = false;
    bool radio = false;
    bool bold = false;  // The default item, hit by Enter.
    WCHAR mnemonic = 0;
    std::vector<Node> sub;
    ComPtr<IDWriteTextLayout> layout;
    ComPtr<IDWriteTextLayout> hintLayout;
};

// One open level. Everything is in screen coordinates; the window paints with
// a translation so a panel can be hit tested against the pointer directly.
struct Panel {
    std::vector<Node>* items = nullptr;
    D2D1_RECT_F rect{};
    float width = 0;
    float contentHeight = 0;
    float scroll = 0;
    int hover = -1;
    int pressed = -1;
    int parentItem = -1;
    float checkWidth = 0, arrowWidth = 0, hintWidth = 0;
};

struct State {
    std::vector<Node> root;
    std::vector<Panel> panels;
    HWND hwnd = nullptr;
    RECT work{};
    POINT origin{};
    int width = 0, height = 0;
    float scale = 1;
    float rootTop = 0;
    float widestChild = 0, tallestChild = 0;
    bool running = false;
    UINT result = 0;
    int active = 0;
    bool ignoreFirstMove = true;
    int wheel = 0;
    int pressedPanel = -1, pressedItem = -1;
    int pendingPanel = -1, pendingItem = -1;
    double appearStart = 0;
    UINT generation = 0;
    ComPtr<IDWriteTextFormat> textFormat, boldFormat, hintFormat;
    ComPtr<ID2D1Bitmap1> frame, backdrop;
    int backdropFactor = 1;
    bool frameBackdrop = false;  // Backdrop came from the grid's own frame.
    ComPtr<ID3D11Texture2D> staging;
    Surface surface;
};
// One menu at a time, and freed by Reset instead of at exit.
[[clang::no_destroy]] State g_menu;

float PadY(float scale) { return 6 * scale; }
float RowHeight(const Node& node, float scale) {
    return node.separator ? 9 * scale : 32 * scale;
}

bool PointInRect(const D2D1_RECT_F& r, float x, float y) {
    return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}

// ----------------------------------------------------------------------------
// Reading the HMENU

std::wstring MenuItemText(HMENU menu, int index) {
    WCHAR buffer[512]{};
    int length = GetMenuStringW(menu, index, buffer, ARRAYSIZE(buffer),
                                MF_BYPOSITION);
    if (length < ARRAYSIZE(buffer) - 1) return buffer;
    std::wstring text((size_t)length + 1, L'\0');
    GetMenuStringW(menu, index, text.data(), (int)text.size(), MF_BYPOSITION);
    text.resize(wcslen(text.c_str()));
    return text;
}

// "&&" is a literal ampersand and a single "&" marks the keyboard mnemonic;
// neither is drawn. A tab separates the shortcut column.
std::wstring CleanMenuItem(const std::wstring& raw, WCHAR& mnemonic,
                           std::wstring& hint) {
    std::wstring text = raw;
    size_t tab = text.find(L'\t');
    if (tab != std::wstring::npos) {
        hint = text.substr(tab + 1);
        text.erase(tab);
    }
    std::wstring out;
    out.reserve(text.size());
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] != L'&') {
            out += text[i];
            continue;
        }
        if (i + 1 < text.size() && text[i + 1] == L'&') {
            out += L'&';
            i++;
            continue;
        }
        if (i + 1 < text.size()) {
            if (!mnemonic) {
                WCHAR c = text[i + 1];
                mnemonic =
                    c >= L'a' && c <= L'z' ? (WCHAR)(c - L'a' + L'A') : c;
            }
            i++;
        }
    }
    return out;
}

bool ReadMenu(HMENU menu, std::vector<Node>& nodes) {
    int count = GetMenuItemCount(menu);
    if (count <= 0) return false;
    nodes.clear();
    nodes.reserve((size_t)count);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW info{};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(menu, (UINT)i, TRUE, &info)) continue;
        Node node;
        node.id = info.wID;
        node.separator = (info.fType & MFT_SEPARATOR) != 0;
        node.enabled = !(info.fState & (MFS_DISABLED | MFS_GRAYED));
        node.checked = (info.fState & MFS_CHECKED) != 0;
        node.radio = (info.fType & MFT_RADIOCHECK) != 0;
        node.bold = (info.fState & MFS_DEFAULT) != 0;
        if (!node.separator) {
            node.text =
                CleanMenuItem(MenuItemText(menu, i), node.mnemonic, node.hint);
        }
        if (info.hSubMenu) ReadMenu(info.hSubMenu, node.sub);
        nodes.push_back(std::move(node));
    }
    return !nodes.empty();
}

// ----------------------------------------------------------------------------
// Measuring

void EnsureFormats(State& m, float scale) {
    if (m.textFormat && m.scale == scale) return;
    m.scale = scale;
    auto make = [&](float size, DWRITE_FONT_WEIGHT weight,
                    ComPtr<IDWriteTextFormat>& format) {
        format.Reset();
        g_dwrite->CreateTextFormat(g_settings.font.c_str(), nullptr, weight,
                                   DWRITE_FONT_STYLE_NORMAL,
                                   DWRITE_FONT_STRETCH_NORMAL, size * scale,
                                   L"", &format);
        if (!format) return;
        format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        ComPtr<IDWriteInlineObject> ellipsis;
        g_dwrite->CreateEllipsisTrimmingSign(format.Get(), &ellipsis);
        format->SetTrimming(&trimming, ellipsis.Get());
    };
    make(14.0f, DWRITE_FONT_WEIGHT_NORMAL, m.textFormat);
    make(14.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD, m.boldFormat);
    make(12.5f, DWRITE_FONT_WEIGHT_NORMAL, m.hintFormat);
    if (m.hintFormat) m.hintFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
}

ComPtr<IDWriteTextLayout> MakeLayout(IDWriteTextFormat* format,
                                     const std::wstring& text, float maxWidth,
                                     float maxHeight) {
    ComPtr<IDWriteTextLayout> layout;
    if (!g_dwrite || !format || text.empty()) return layout;
    g_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(), format,
                               maxWidth, maxHeight, &layout);
    return layout;
}

void MeasurePanel(State& m, Panel& panel) {
    const float s = m.scale;
    const float rowHeight = 32 * s;
    const float maxText = 380 * s;
    float widest = 0, widestHint = 0;
    bool submenu = false, checks = false;
    for (auto& node : *panel.items) {
        if (node.separator) continue;
        if (!node.sub.empty()) submenu = true;
        if (node.checked) checks = true;
        node.layout.Reset();
        node.hintLayout.Reset();
        if (node.text.empty()) continue;
        node.layout = MakeLayout(node.bold && m.boldFormat ? m.boldFormat.Get()
                                                          : m.textFormat.Get(),
                                 node.text, maxText, rowHeight);
        if (node.layout) {
            DWRITE_TEXT_METRICS metrics{};
            if (SUCCEEDED(node.layout->GetMetrics(&metrics)))
                widest = std::max(widest, metrics.width);
        }
        if (!node.hint.empty()) {
            node.hintLayout = MakeLayout(m.hintFormat.Get(), node.hint,
                                         180 * s, rowHeight);
            if (node.hintLayout) {
                DWRITE_TEXT_METRICS metrics{};
                if (SUCCEEDED(node.hintLayout->GetMetrics(&metrics)))
                    widestHint = std::max(widestHint, metrics.width);
            }
        }
    }
    panel.checkWidth = checks ? 18 * s : 0;
    panel.arrowWidth = submenu ? 16 * s : 0;
    panel.hintWidth = widestHint > 0 ? widestHint + 18 * s : 0;
    float frame = 14 * s + panel.checkWidth + panel.hintWidth +
                  panel.arrowWidth + 14 * s;
    panel.width = std::max(172 * s, frame + widest);
    float height = 2 * PadY(s);
    for (const auto& node : *panel.items) height += RowHeight(node, s);
    panel.contentHeight = height;
}

// Every level is measured up front: the window is one fixed size for the life
// of the menu, so a submenu never has to resize it (which would need a fresh
// screenshot).
void MeasureChildren(State& m, std::vector<Node>& nodes) {
    for (auto& node : nodes) {
        if (node.sub.empty()) continue;
        Panel panel;
        panel.items = &node.sub;
        MeasurePanel(m, panel);
        m.widestChild = std::max(m.widestChild, panel.width);
        m.tallestChild = std::max(m.tallestChild, panel.contentHeight);
        MeasureChildren(m, node.sub);
    }
}

// ----------------------------------------------------------------------------
// Placement

void PlaceRoot(State& m, POINT point, UINT flags) {
    Panel& panel = m.panels[0];
    const float s = m.scale;
    const float margin = 4 * s;
    const float workWidth = (float)(m.work.right - m.work.left);
    const float workHeight = (float)(m.work.bottom - m.work.top);
    float w = std::min(panel.width, workWidth - margin * 2);
    float h = std::min(panel.contentHeight, workHeight - margin * 2);
    float x = (float)point.x, y = (float)point.y;
    if (flags & TPM_RIGHTALIGN) x -= w;
    else if (flags & TPM_CENTERALIGN) x -= w * 0.5f;
    if (flags & TPM_BOTTOMALIGN) y -= h;
    else if (flags & TPM_VCENTERALIGN) y -= h * 0.5f;
    float left = (float)m.work.left, top = (float)m.work.top;
    float maxX = (float)m.work.right - margin - w;
    float maxY = (float)m.work.bottom - margin - h;
    if (maxX < left) maxX = left;
    if (maxY < top) maxY = top;
    x = std::clamp(x, left + margin, maxX);
    y = std::clamp(y, top + margin, maxY);
    panel.rect = D2D1::RectF(x, y, x + w, y + h);
    m.rootTop = y;
}

void OpenSubmenu(int level, int item);

// ----------------------------------------------------------------------------
// Drawing

void DrawItem(State& m, Panel& panel, int index, Node& node, float top,
              float height, float left, float right) {
    const float s = m.scale;
    if (node.separator) {
        float y = std::round(top + height * 0.5f) + 0.5f;
        g_brush->SetColor(D2D1::ColorF(0xFFFFFF, 0.16f));
        g_target->DrawLine(D2D1::Point2F(left + 8 * s, y),
                           D2D1::Point2F(right - 8 * s, y), g_brush.Get(), 1);
        return;
    }
    const float radius = 8 * s;
    D2D1_RECT_F pill =
        D2D1::RectF(left, top + 1 * s, right, top + height - 1 * s);
    if (panel.hover == index) {
        // Same white wash the dock and the search box use for hover, floored so
        // the row always reads as selected even with the glow turned down.
        float glow = std::clamp(g_settings.glowStrength, 0.f, 1.f);
        Fill(pill, D2D1::ColorF(0xFFFFFF, 0.12f + 0.14f * glow), radius);
    }
    float textLeft = left + 10 * s + panel.checkWidth;
    float textRight = right - 8 * s - panel.arrowWidth - panel.hintWidth;
    if (panel.hintWidth > 0 && node.hintLayout) {
        g_brush->SetColor(D2D1::ColorF(0xFFFFFF, node.enabled ? 0.55f : 0.30f));
        g_target->DrawTextLayout(
            D2D1::Point2F(right - 8 * s - panel.arrowWidth - panel.hintWidth,
                          top),
            node.hintLayout.Get(), g_brush.Get());
    }
    if (node.layout) {
        g_brush->SetColor(node.enabled ? D2D1::ColorF(0xFFFFFF, 0.96f)
                                       : D2D1::ColorF(0xFFFFFF, 0.38f));
        g_target->DrawTextLayout(D2D1::Point2F(textLeft, top),
                                 node.layout.Get(), g_brush.Get());
    }
    if (panel.checkWidth > 0 && node.checked) {
        float cx = left + 10 * s + panel.checkWidth * 0.5f;
        float cy = top + height * 0.5f;
        g_brush->SetColor(D2D1::ColorF(0xFFFFFF, node.enabled ? 0.92f : 0.32f));
        if (node.radio) {
            g_target->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(cx, cy), 3.2f * s, 3.2f * s),
                g_brush.Get());
        } else {
            g_target->DrawLine(D2D1::Point2F(cx - 4 * s, cy),
                               D2D1::Point2F(cx - 1 * s, cy + 3.2f * s),
                               g_brush.Get(), 1.6f * s);
            g_target->DrawLine(D2D1::Point2F(cx - 1 * s, cy + 3.2f * s),
                               D2D1::Point2F(cx + 4.2f * s, cy - 3.4f * s),
                               g_brush.Get(), 1.6f * s);
        }
    }
    if (!node.sub.empty()) {
        float cx = right - 8 * s - panel.arrowWidth * 0.5f;
        float cy = top + height * 0.5f;
        float arm = 3.6f * s;
        g_brush->SetColor(D2D1::ColorF(0xFFFFFF, node.enabled ? 0.70f : 0.30f));
        g_target->DrawLine(D2D1::Point2F(cx - arm * 0.5f, cy - arm),
                           D2D1::Point2F(cx + arm * 0.7f, cy), g_brush.Get(),
                           1.4f * s);
        g_target->DrawLine(D2D1::Point2F(cx + arm * 0.7f, cy),
                           D2D1::Point2F(cx - arm * 0.5f, cy + arm),
                           g_brush.Get(), 1.4f * s);
    }
}

void DrawPanel(State& m, Panel& panel) {
    const float s = m.scale;
    const D2D1_RECT_F r = panel.rect;
    const float radius = 10 * s;
    // The popup wears the same material as the search box and the power dock,
    // so a menu opened on a tile reads as part of the grid rather than as a
    // foreign window. DrawGlassSurface() itself cannot be reused: it paints
    // into the shared frame and bails out through SelectionHasArea().
    const float shift = 2 * s;
    Fill(D2D1::RectF(r.left, r.top + shift, r.right, r.bottom + shift),
         D2D1::ColorF(0, 0.14f), radius);
    // The base is the blurred screen behind the popup, painted as a rounded
    // bitmap fill: the popup keeps its corners and nothing sharp from behind
    // can reach the labels.
    if (m.backdrop) {
        ComPtr<ID2D1BitmapBrush> brush;
        if (SUCCEEDED(g_target->CreateBitmapBrush(
                m.backdrop.Get(),
                D2D1::BitmapBrushProperties(
                    D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR),
                &brush))) {
            brush->SetTransform(D2D1::Matrix3x2F::Scale(
                (float)m.backdropFactor, (float)m.backdropFactor));
            g_target->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius),
                                           brush.Get());
        }
    }
    // Then the launcher's own recipe: the chrome veil the search bar and the
    // power dock are made of, and their white edge. When the base came from
    // the grid's frame it already wears the grid's tint — adding more would
    // make the menu a different color from the search bar. Only the tray
    // menu, whose backdrop is a raw screen copy, needs the extra shade.
    if (!m.frameBackdrop) {
        const float tintAlpha =
            std::clamp(g_settings.opacity + 0.30f, 0.f, 0.94f);
        Fill(r, Alpha(g_settings.tint, tintAlpha), radius);
    }
    Fill(r, D2D1::ColorF(0xE8E8E8, 0.07f + 0.17f * g_settings.chromeOpacity),
         radius);
    g_brush->SetColor(D2D1::ColorF(0xFFFFFF, 0.22f));
    g_target->DrawRoundedRectangle(
        D2D1::RoundedRect(
            D2D1::RectF(r.left + .5f, r.top + .5f, r.right - .5f, r.bottom - .5f),
            radius - .5f, radius - .5f),
        g_brush.Get(), 1);
    const float padY = PadY(s);
    g_target->PushAxisAlignedClip(
        D2D1::RectF(r.left, r.top + padY, r.right, r.bottom - padY),
        D2D1_ANTIALIAS_MODE_ALIASED);
    float top = r.top + padY - panel.scroll;
    float pillLeft = r.left + 6 * s, pillRight = r.right - 6 * s;
    for (int i = 0; i < (int)panel.items->size(); i++) {
        Node& node = (*panel.items)[i];
        float height = RowHeight(node, s);
        if (top + height > r.top + padY && top < r.bottom - padY) {
            DrawItem(m, panel, i, node, top, height, pillLeft, pillRight);
        }
        top += height;
    }
    g_target->PopAxisAlignedClip();
}

// BlurToBitmap() hands back a bitmap created as a render target. Direct2D
// refuses to sample one of those, so the blur is copied into an ordinary
// bitmap that a brush can read.
ComPtr<ID2D1Bitmap1> SampledBlur(ComPtr<ID2D1Bitmap1> blurred) {
    if (!blurred) return nullptr;
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_NONE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    ComPtr<ID2D1Bitmap1> out;
    if (FAILED(g_target->CreateBitmap(blurred->GetPixelSize(), nullptr, 0,
                                      props, &out)) ||
        FAILED(out->CopyFromBitmap(nullptr, blurred.Get(), nullptr))) {
        return nullptr;
    }
    return out;
}

#ifdef SEAMLESS_PREVIEW
// Offscreen previews never put a window on screen, so the harness hands over
// the grid it just drew and the capture reads that instead of the desktop.
HBITMAP g_previewScreen = nullptr;
POINT g_previewScreenOrigin = {0, 0};
#endif

// The blurred copy of whatever sits behind the menu window, taken while the
// window is still hidden. Inside the grid that is the frame the launcher just
// drew — the same pixels the search bar sits on — read straight from the GPU
// and blurred exactly like the screen copy: same blur, same tint, same light,
// and it cannot pick up the menu itself or miss the launcher. The tray menu,
// which opens with no grid on screen, falls back to a screen copy.
bool CaptureBackdrop(State& m) {
    m.backdrop.Reset();
    m.frameBackdrop = false;
    if (!g_target || m.width <= 0 || m.height <= 0) return false;
    const float sigma = 26 * m.scale;
    const int factor = std::clamp((int)(sigma / 3), 1, 16);
    const int sw = (m.width + factor - 1) / factor;
    const int sh = (m.height + factor - 1) / factor;

    // DrawBitmap out of a target bitmap works — the folder panel's frost is
    // taken from this same frame — but a brush needs an ordinary bitmap, so
    // the blur lands in SampledBlur() on the way out. The frame is at rest
    // here: right-clicks reach it fully shown, unzoomed, identity transform,
    // so frame pixels map one to one onto window position on screen.
    if (g_shown && !g_closing && g_appear >= 1 && g_frame && g_wnd) {
        RECT wnd{};
#ifdef SEAMLESS_PREVIEW
        wnd = g_layout.area;  // The preview window sits exactly on its area.
#else
        if (GetWindowRect(g_wnd, &wnd))
#endif
        {
            int x0 = m.origin.x - wnd.left, y0 = m.origin.y - wnd.top;
            int x1 = x0 + m.width, y1 = y0 + m.height;
            if (x0 >= 0 && y0 >= 0 && x1 <= g_frameWidth &&
                y1 <= g_frameHeight) {
                m.backdrop = SampledBlur(BlurToBitmap(
                    g_frame.Get(),
                    D2D1::RectF((float)x0, (float)y0, (float)x1, (float)y1),
                    factor, sigma, sw, sh));
                if (m.backdrop) {
                    m.backdropFactor = factor;
                    m.frameBackdrop = true;
                    return true;
                }
            }
        }
    }

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = m.width;
    info.bmiHeader.biHeight = -m.height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC screen = GetDC(nullptr);
    HDC memory = CreateCompatibleDC(screen);
    HBITMAP bitmap =
        CreateDIBSection(memory, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        return false;
    }
    HGDIOBJ old = SelectObject(memory, bitmap);
    // Wait for the compositor, so a grid just selected under the pointer is on
    // screen before its pixels are blurred.
    DwmFlush();
#ifdef SEAMLESS_PREVIEW
    if (g_previewScreen) {
        HDC source = CreateCompatibleDC(screen);
        HGDIOBJ previous = SelectObject(source, g_previewScreen);
        BitBlt(memory, 0, 0, m.width, m.height, source,
               m.origin.x - g_previewScreenOrigin.x,
               m.origin.y - g_previewScreenOrigin.y, SRCCOPY);
        SelectObject(source, previous);
        DeleteDC(source);
    } else
#endif
    {
        BitBlt(memory, 0, 0, m.width, m.height, screen, m.origin.x, m.origin.y,
               SRCCOPY);
    }
    GdiFlush();
    SelectObject(memory, old);
    DeleteDC(memory);
    ReleaseDC(nullptr, screen);
    // GDI screen copies leave the alpha channel undefined; the source is
    // opaque.
    for (size_t i = 0; i < (size_t)m.width * m.height; i++)
        ((UINT32*)bits)[i] |= 0xFF000000;
    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    ComPtr<ID2D1Bitmap> full;
    bool uploaded = SUCCEEDED(g_target->CreateBitmap(
        D2D1::SizeU(m.width, m.height), bits, m.width * 4, props, &full));
    DeleteObject(bitmap);
    if (!uploaded) return false;
    m.backdrop = SampledBlur(BlurToBitmap(
        full.Get(), D2D1::RectF(0, 0, (float)m.width, (float)m.height), factor,
        sigma, sw, sh));
    if (!m.backdrop) return false;
    m.backdropFactor = factor;
    return true;
}

bool EnsureMenuFrame(State& m) {
    if (m.frame) return true;
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    if (FAILED(g_target->CreateBitmap(D2D1::SizeU(m.width, m.height), nullptr,
                                      0, props, &m.frame))) {
        return false;
    }
    if (!EnsureSurface(m.surface, m.width, m.height)) {
        m.frame.Reset();
        return false;
    }
    m.staging.Reset();
    return true;
}

bool CopyToSurface(State& m) {
    if (!m.frame || !m.surface.bitmap || !g_d3d) return false;
    if (!m.staging) {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = m.width;
        desc.Height = m.height;
        desc.MipLevels = desc.ArraySize = desc.SampleDesc.Count = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        if (FAILED(g_d3d->CreateTexture2D(&desc, nullptr, &m.staging)))
            return false;
    }
    DIBSECTION ds{};
    if (GetObjectW(m.surface.bitmap, sizeof(ds), &ds) != sizeof(ds) ||
        !ds.dsBm.bmBits) {
        return false;
    }
    ComPtr<IDXGISurface> surface;
    ComPtr<ID3D11Texture2D> texture;
    if (FAILED(m.frame->GetSurface(&surface)) ||
        FAILED(surface.As(&texture))) {
        return false;
    }
    ComPtr<ID3D11DeviceContext> ctx;
    g_d3d->GetImmediateContext(&ctx);
    ctx->CopyResource(m.staging.Get(), texture.Get());
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(ctx->Map(m.staging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
        return false;
    size_t span = (size_t)m.width * 4;
    const BYTE* src = (const BYTE*)mapped.pData;
    BYTE* dst = (BYTE*)ds.dsBm.bmBits;
    for (int y = 0; y < m.height; y++)
        memcpy(dst + (size_t)y * span, src + (size_t)y * mapped.RowPitch, span);
    ctx->Unmap(m.staging.Get(), 0);
    return true;
}

float AppearAmount(State& m) {
    float t = (float)((Now() - m.appearStart) * 1000.0 / kAnimationMs);
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}

bool DrawFrame(State& m) {
    if (m.width <= 0 || m.height <= 0) return false;
    if (!EnsureTarget()) return false;
    if (m.generation != g_targetGeneration) {
        // The device went away: the cached blur belongs to the old one.
        m.frame.Reset();
        m.staging.Reset();
        m.backdrop.Reset();
        m.generation = g_targetGeneration;
    }
    if (!EnsureMenuFrame(m)) return false;
    g_target->SetTarget(m.frame.Get());
    g_target->BeginDraw();
    g_target->SetTransform(D2D1::Matrix3x2F::Translation(
        -(float)m.origin.x, -(float)m.origin.y));
    g_target->Clear(D2D1::ColorF(0, 0, 0, 0));
    for (auto& panel : m.panels) DrawPanel(m, panel);
    g_target->SetTransform(D2D1::Matrix3x2F::Identity());
    if (g_target->EndDraw() == D2DERR_RECREATE_TARGET) {
        ResetTarget();
        return false;
    }
    if (!CopyToSurface(m)) return false;
    return true;
}

// Puts the finished frame on screen, fading in over the first ~120 ms.
void Present(State& m) {
    if (!m.hwnd || !m.surface.dc) return;
    POINT dst{m.origin.x, m.origin.y}, src{0, 0};
    SIZE size{m.width, m.height};
    BLENDFUNCTION blend{AC_SRC_OVER, 0,
                        (BYTE)std::lround(255 * AppearAmount(m)),
                        AC_SRC_ALPHA};
    UpdateLayeredWindow(m.hwnd, nullptr, &dst, &size, m.surface.dc, &src, 0,
                        &blend, ULW_ALPHA);
}

bool Paint(State& m) {
    if (!m.hwnd) return false;
    if (!DrawFrame(m)) return false;
    Present(m);
    return true;
}

// ----------------------------------------------------------------------------
// Interaction

void Finish(UINT command) {
    g_menu.result = command;
    g_menu.running = false;
}

int PanelAt(float x, float y) {
    State& m = g_menu;
    for (int i = (int)m.panels.size() - 1; i >= 0; i--) {
        if (PointInRect(m.panels[i].rect, x, y)) return i;
    }
    return -1;
}

int HitTest(const Panel& panel, float x, float y) {
    if (!PointInRect(panel.rect, x, y)) return -1;
    const float s = g_menu.scale;
    float top = panel.rect.top + PadY(s) - panel.scroll;
    for (int i = 0; i < (int)panel.items->size(); i++) {
        const Node& node = (*panel.items)[i];
        float height = RowHeight(node, s);
        if (y >= top && y < top + height) {
            return node.separator || !node.enabled ? -1 : i;
        }
        top += height;
    }
    return -1;
}

void SetHover(int level, int item) {
    State& m = g_menu;
    bool changed = false;
    for (int i = 0; i < (int)m.panels.size(); i++) {
        int value = i == level ? item : -1;
        if (m.panels[i].hover != value) {
            m.panels[i].hover = value;
            changed = true;
        }
    }
    if (changed) Paint(m);
}

// What a row in an ancestor panel means for the levels opened below it.
enum class DeeperAction {
    Keep,       // The row owns the open submenu: the pointer is on its way.
    CloseNow,   // Another row with a submenu: its own level replaces this one.
    CloseSoon,  // Any other row, separator or padding edge: wait for the hand.
};

DeeperAction DeeperLevelsAction(int item, int owner, bool itemHasSubmenu) {
    if (item == owner) return DeeperAction::Keep;
    if (item >= 0 && itemHasSubmenu) return DeeperAction::CloseNow;
    return DeeperAction::CloseSoon;
}

void CloseDeeper(int level) {
    State& m = g_menu;
    m.pendingPanel = m.pendingItem = -1;
    if (m.hwnd) {
        KillTimer(m.hwnd, kSubmenuTimer);
        KillTimer(m.hwnd, kSubmenuCloseTimer);
    }
    if ((int)m.panels.size() > level + 1) {
        m.panels.resize(level + 1);
        m.active = level;
        Paint(m);
    }
}

// The row's top edge inside a panel, in screen pixels.
float RowTop(const Panel& panel, int index) {
    const float s = g_menu.scale;
    float top = panel.rect.top + PadY(s) - panel.scroll;
    for (int i = 0; i < index; i++) top += RowHeight((*panel.items)[i], s);
    return top;
}

void ScrollPanel(int level, float delta) {
    State& m = g_menu;
    if (level < 0 || level >= (int)m.panels.size()) return;
    Panel& panel = m.panels[level];
    float visible = panel.rect.bottom - panel.rect.top;
    float limit = std::max(0.0f, panel.contentHeight - visible);
    float value = std::clamp(panel.scroll + delta, 0.0f, limit);
    if (value == panel.scroll) return;
    panel.scroll = value;
    panel.hover = -1;
    m.pendingPanel = m.pendingItem = -1;
    if (m.hwnd) {
        KillTimer(m.hwnd, kSubmenuTimer);
        KillTimer(m.hwnd, kSubmenuCloseTimer);
    }
    Paint(m);
}

void EnsureVisible(int level, int index) {
    State& m = g_menu;
    Panel& panel = m.panels[level];
    float s = m.scale;
    float visible = panel.rect.bottom - panel.rect.top - PadY(s) * 2;
    float top = RowTop(panel, index) - panel.rect.top - PadY(s) + panel.scroll;
    float height = RowHeight((*panel.items)[index], s);
    float limit = std::max(0.0f, panel.contentHeight - (panel.rect.bottom - panel.rect.top));
    if (top < panel.scroll) panel.scroll = top;
    else if (top + height > panel.scroll + visible)
        panel.scroll = top + height - visible;
    panel.scroll = std::clamp(panel.scroll, 0.0f, limit);
}

void MoveHover(int step) {
    State& m = g_menu;
    if (m.panels.empty()) return;
    int level = std::clamp(m.active, 0, (int)m.panels.size() - 1);
    Panel& panel = m.panels[level];
    int count = (int)panel.items->size();
    if (!count) return;
    int index = panel.hover;
    for (int probe = 0; probe < count; probe++) {
        index = index < 0 ? (step > 0 ? 0 : count - 1)
                          : (index + step + count) % count;
        const Node& node = (*panel.items)[index];
        if (!node.separator && node.enabled) break;
    }
    panel.hover = index;
    EnsureVisible(level, index);
    Paint(m);
}

void OpenSubmenu(int level, int item) {
    State& m = g_menu;
    if (level < 0 || level >= (int)m.panels.size()) return;
    if (item < 0 || item >= (int)m.panels[level].items->size()) return;
    std::vector<Node>& nodes = *m.panels[level].items;
    if (nodes[item].sub.empty()) return;
    if ((int)m.panels.size() > level + 1 &&
        m.panels[level + 1].parentItem == item) {
        return;
    }
    const D2D1_RECT_F parentRect = m.panels[level].rect;
    const float itemTop = RowTop(m.panels[level], item);
    m.panels.resize(level + 1);
    Panel child;
    child.items = &nodes[item].sub;
    child.parentItem = item;
    MeasurePanel(m, child);
    const float s = m.scale;
    float maxHeight = (float)m.work.bottom - m.rootTop - 8 * s;
    float height = std::min(child.contentHeight, std::max(maxHeight, 4 * s));
    float x = parentRect.right - 2 * s;
    if (x + child.width > (float)m.work.right - 4 * s) {
        x = parentRect.left - child.width + 2 * s;
    }
    float minX = (float)m.work.left + 4 * s;
    float maxX = std::max(minX, (float)m.work.right - 4 * s - child.width);
    x = std::clamp(x, minX, maxX);
    float maxY = std::max(m.rootTop, (float)m.work.bottom - 4 * s - height);
    float y = std::clamp(itemTop - 4 * s, m.rootTop, maxY);
    child.rect = D2D1::RectF(x, y, x + child.width, y + height);
    m.panels.push_back(child);
    m.active = (int)m.panels.size() - 1;
    m.panels[level].hover = item;
    Paint(m);
}

void Activate(int level, int item) {
    State& m = g_menu;
    if (level < 0 || level >= (int)m.panels.size()) return;
    Panel& panel = m.panels[level];
    if (item < 0 || item >= (int)panel.items->size()) return;
    Node& node = (*panel.items)[item];
    if (node.separator || !node.enabled) return;
    if (!node.sub.empty()) {
        OpenSubmenu(level, item);
        return;
    }
    Finish(node.id);
}

void OnKey(WPARAM key) {
    State& m = g_menu;
    if (m.panels.empty()) return;
    int level = std::clamp(m.active, 0, (int)m.panels.size() - 1);
    Panel& panel = m.panels[level];
    switch (key) {
        case VK_UP:
            MoveHover(-1);
            break;
        case VK_DOWN:
            MoveHover(1);
            break;
        case VK_HOME:
            panel.hover = -1;
            MoveHover(1);
            break;
        case VK_END:
            panel.hover = -1;
            MoveHover(-1);
            break;
        case VK_PRIOR:
            ScrollPanel(level, -10 * RowHeight(Node{}, m.scale));
            break;
        case VK_NEXT:
            ScrollPanel(level, 10 * RowHeight(Node{}, m.scale));
            break;
        case VK_RIGHT:
            if (panel.hover >= 0) {
                int item = panel.hover;
                if (!(*panel.items)[item].sub.empty()) {
                    OpenSubmenu(level, item);
                    m.active = (int)m.panels.size() - 1;
                    m.panels[m.active].hover = -1;
                    MoveHover(1);
                }
            }
            break;
        case VK_LEFT:
            if (level > 0) {
                int parentItem = m.panels[level].parentItem;
                CloseDeeper(level - 1);
                m.active = level - 1;
                m.panels[level - 1].hover = parentItem;
                Paint(m);
            }
            break;
        case VK_RETURN:
            if (panel.hover >= 0) Activate(level, panel.hover);
            break;
        case VK_ESCAPE:
            if (level > 0) {
                CloseDeeper(level - 1);
                m.active = level - 1;
                Paint(m);
            } else {
                Finish(0);
            }
            break;
        default:
            break;
    }
}

void OnMnemonic(WPARAM key) {
    State& m = g_menu;
    int level = std::clamp(m.active, 0, (int)m.panels.size() - 1);
    Panel& panel = m.panels[level];
    WCHAR letter = key >= L'a' && key <= L'z' ? (WCHAR)(key - L'a' + L'A')
                                             : (WCHAR)key;
    for (int i = 0; i < (int)panel.items->size(); i++) {
        if ((*panel.items)[i].mnemonic != letter) continue;
        panel.hover = i;
        Paint(m);
        Activate(level, i);
        return;
    }
}

// ----------------------------------------------------------------------------
// The window

LRESULT CALLBACK MenuWndProc(HWND hwnd, UINT message, WPARAM wParam,
                             LPARAM lParam) {
    State& m = g_menu;
    switch (message) {
        case WM_MOUSEACTIVATE:
            return MA_ACTIVATE;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && m.running) Finish(0);
            return 0;
        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_ARROW));
            return TRUE;
        case WM_MOUSEMOVE: {
            if (m.ignoreFirstMove) {
                m.ignoreFirstMove = false;
                return 0;
            }
            float x = (float)GET_X_LPARAM(lParam) + m.origin.x;
            float y = (float)GET_Y_LPARAM(lParam) + m.origin.y;
            int level = PanelAt(x, y);
            if (level < 0) {
                SetHover(-1, -1);  // Over the shadow or outside: keep the levels.
                return 0;
            }
            int item = HitTest(m.panels[level], x, y);
            if (level < (int)m.panels.size() - 1) {
                // A level below this panel belongs to one of its rows. Keeping
                // that row's submenu alive is what makes the row reachable:
                // closing it as soon as another row is touched left no way to
                // walk from "Move to group" to the groups it lists.
                const int owner = m.panels[level + 1].parentItem;
                const bool itemHasSubmenu =
                    item >= 0 && !(*m.panels[level].items)[item].sub.empty();
                switch (DeeperLevelsAction(item, owner, itemHasSubmenu)) {
                    case DeeperAction::Keep:
                        KillTimer(hwnd, kSubmenuCloseTimer);
                        break;
                    case DeeperAction::CloseNow:
                        KillTimer(hwnd, kSubmenuCloseTimer);
                        CloseDeeper(level);
                        break;
                    case DeeperAction::CloseSoon:
                        SetTimer(hwnd, kSubmenuCloseTimer, kSubmenuCloseDelayMs,
                                 nullptr);
                        break;
                }
            } else {
                KillTimer(hwnd, kSubmenuCloseTimer);
            }
            SetHover(level, item);
            if (item >= 0 && (int)m.panels.size() == level + 1 &&
                !(*m.panels[level].items)[item].sub.empty()) {
                if (m.pendingPanel != level || m.pendingItem != item) {
                    m.pendingPanel = level;
                    m.pendingItem = item;
                    SetTimer(hwnd, kSubmenuTimer, kSubmenuDelayMs, nullptr);
                }
            } else {
                m.pendingPanel = m.pendingItem = -1;
                KillTimer(hwnd, kSubmenuTimer);
            }
            return 0;
        }
        case WM_LBUTTONDOWN: {
            float x = (float)GET_X_LPARAM(lParam) + m.origin.x;
            float y = (float)GET_Y_LPARAM(lParam) + m.origin.y;
            int level = PanelAt(x, y);
            if (level < 0) {
                Finish(0);  // The click that dismisses is swallowed.
                return 0;
            }
            int item = HitTest(m.panels[level], x, y);
            m.pressedPanel = level;
            m.pressedItem = item;
            for (auto& panel : m.panels) panel.pressed = -1;
            m.panels[level].pressed = item;
            Paint(m);
            return 0;
        }
        case WM_LBUTTONUP: {
            float x = (float)GET_X_LPARAM(lParam) + m.origin.x;
            float y = (float)GET_Y_LPARAM(lParam) + m.origin.y;
            int level = PanelAt(x, y);
            int item = level >= 0 ? HitTest(m.panels[level], x, y) : -1;
            int pressedLevel = m.pressedPanel, pressedItem = m.pressedItem;
            m.pressedPanel = m.pressedItem = -1;
            for (auto& panel : m.panels) panel.pressed = -1;
            if (level < 0) {
                Finish(0);
                return 0;
            }
            if (level == pressedLevel && item >= 0 && item == pressedItem) {
                Activate(level, item);
            } else {
                SetHover(level, item);
            }
            return 0;
        }
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            Finish(0);
            return 0;
        case WM_MOUSEWHEEL: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int level = PanelAt((float)point.x, (float)point.y);
            if (level < 0) level = m.active;
            m.wheel += GET_WHEEL_DELTA_WPARAM(wParam);
            float step = 3 * 32 * m.scale;
            while (m.wheel >= WHEEL_DELTA) {
                ScrollPanel(level, -step);
                m.wheel -= WHEEL_DELTA;
            }
            while (m.wheel <= -WHEEL_DELTA) {
                ScrollPanel(level, step);
                m.wheel += WHEEL_DELTA;
            }
            return 0;
        }
        case WM_TIMER:
            if (wParam == kSubmenuTimer) {
                KillTimer(hwnd, kSubmenuTimer);
                int level = m.pendingPanel, item = m.pendingItem;
                m.pendingPanel = m.pendingItem = -1;
                if (level >= 0 && level < (int)m.panels.size() && item >= 0 &&
                    item < (int)m.panels[level].items->size() &&
                    (int)m.panels.size() == level + 1 &&
                    m.panels[level].hover == item) {
                    if (!(*m.panels[level].items)[item].sub.empty())
                        OpenSubmenu(level, item);
                }
            } else if (wParam == kSubmenuCloseTimer) {
                KillTimer(hwnd, kSubmenuCloseTimer);
                // Re-read the pointer instead of closing blind: it may have
                // reached the submenu after the move that armed this timer.
                POINT point{};
                if (GetCursorPos(&point)) {
                    int level = PanelAt((float)point.x, (float)point.y);
                    if (level >= 0 && level < (int)m.panels.size() - 1) {
                        int item = HitTest(m.panels[level], (float)point.x,
                                           (float)point.y);
                        if (item != m.panels[level + 1].parentItem) {
                            CloseDeeper(level);
                        }
                    }
                }
            } else if (wParam == kAnimationTimer) {
                float t = AppearAmount(m);
                if (t >= 1.0f) KillTimer(hwnd, kAnimationTimer);
                Paint(m);
            }
            return 0;
        case WM_KEYDOWN:
            OnKey(wParam);
            return 0;
        case WM_SYSKEYDOWN:
            if (wParam == VK_F4 && (GetKeyState(VK_MENU) & 0x8000)) {
                Finish(0);
            } else if (wParam != VK_MENU) {
                OnMnemonic(wParam);
            }
            return 0;
        case WM_CAPTURECHANGED:
            if (m.running && (HWND)lParam != hwnd) Finish(0);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool g_menuClassRegistered = false;

bool EnsureMenuClass() {
    if (g_menuClassRegistered) return true;
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = MenuWndProc;
    wc.hInstance = (HINSTANCE)&__ImageBase;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = MenuClassName();
    if (RegisterClassExW(&wc)) {
        g_menuClassRegistered = true;
    } else {
        // The name is unique per image, so this is a real registration
        // failure rather than a class left behind by an earlier build.
        Wh_Log(L"Menu class %ls unavailable: %u", MenuClassName(),
               GetLastError());
        g_menuClassRegistered = false;
    }
    return g_menuClassRegistered;
}

void UnregisterMenuClass() {
    if (!g_menuClassRegistered) return;
    UnregisterClassW(MenuClassName(), (HINSTANCE)&__ImageBase);
    g_menuClassRegistered = false;
}

void Reset() {
    State& m = g_menu;
    if (m.hwnd) {
        KillTimer(m.hwnd, kAnimationTimer);
        KillTimer(m.hwnd, kSubmenuTimer);
        KillTimer(m.hwnd, kSubmenuCloseTimer);
    }
    FreeSurface(m.surface);
    m.surface = {};
    m.frame.Reset();
    m.backdrop.Reset();
    m.staging.Reset();
    m.root.clear();
    m.panels.clear();
    m.hwnd = nullptr;
    m.running = false;
    m.result = 0;
    m.active = 0;
    m.ignoreFirstMove = true;
    m.wheel = 0;
    m.pressedPanel = m.pressedItem = -1;
    m.pendingPanel = m.pendingItem = -1;
    m.width = m.height = 0;
    m.origin = {};
    m.rootTop = 0;
    m.widestChild = m.tallestChild = 0;
    m.backdropFactor = 1;
    m.frameBackdrop = false;
    m.work = {};
}

}  // namespace LaunchpadMenu

// Reads the HMENU, measures every level and fixes the window rectangle. One
// window size covers every level that can open, so a submenu never has to
// resize it and the blurred backdrop is taken exactly once.
bool Prepare(LaunchpadMenu::State& m, HMENU menu, int x, int y, UINT flags) {
    using namespace LaunchpadMenu;
    if (!ReadMenu(menu, m.root)) return false;
    if (!EnsureTarget()) return false;
    POINT point{x, y};
    HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(monitor, &info)) {
        info.rcWork = RECT{0, 0, GetSystemMetrics(SM_CXSCREEN),
                           GetSystemMetrics(SM_CYSCREEN)};
    }
    m.work = info.rcWork;
    UINT dpiX = USER_DEFAULT_SCREEN_DPI, dpiY = USER_DEFAULT_SCREEN_DPI;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
        dpiX = USER_DEFAULT_SCREEN_DPI;
    EnsureFormats(m, dpiX / (float)USER_DEFAULT_SCREEN_DPI);
    m.generation = g_targetGeneration;

    Panel root;
    root.items = &m.root;
    MeasurePanel(m, root);
    MeasureChildren(m, m.root);
    m.panels.push_back(root);
    PlaceRoot(m, point, flags);

    const float s = m.scale;
    const D2D1_RECT_F rootRect = m.panels[0].rect;
    float padding = 24 * s;
    float spread = m.widestChild > 0 ? m.widestChild + 12 * s : 0;
    float drop = m.tallestChild > 0 ? m.tallestChild + 12 * s : 0;
    RECT bounds{};
    bounds.left = (LONG)std::floor(
        std::max((float)m.work.left, rootRect.left - spread - padding));
    bounds.top = (LONG)std::floor(
        std::max((float)m.work.top, rootRect.top - padding));
    bounds.right = (LONG)std::ceil(
        std::min((float)m.work.right, rootRect.right + spread + padding));
    bounds.bottom = (LONG)std::ceil(
        std::min((float)m.work.bottom, rootRect.bottom + drop + padding));
    m.origin = POINT{bounds.left, bounds.top};
    m.width = std::max(1, (int)(bounds.right - bounds.left));
    m.height = std::max(1, (int)(bounds.bottom - bounds.top));
    return true;
}

UINT TrackLaunchpadPopupMenu(HMENU menu, UINT flags, int x, int y, HWND owner,
                             LPTPMPARAMS params) {
    using namespace LaunchpadMenu;
    State& m = g_menu;
    if (m.running) return 0;
    Reset();
    if (!Prepare(m, menu, x, y, flags)) return 0;
    if (!EnsureMenuClass()) return 0;
    Wh_Log(L"menu: %zu root items at %d,%d size %dx%d", m.root.size(),
           m.origin.x, m.origin.y, m.width, m.height);
    bool previousModal = g_modal;
    g_modal = true;
    g_pressed = -1;
    g_pressedEmpty = false;
    g_drag.pending = false;
    m.result = 0;
    m.running = true;
    m.hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
                             MenuClassName(), L"", WS_POPUP, m.origin.x,
                             m.origin.y, m.width, m.height, owner, nullptr,
                             (HINSTANCE)&__ImageBase, nullptr);
    if (!m.hwnd) {
        Wh_Log(L"menu: CreateWindowExW failed: %u", GetLastError());
        m.running = false;
        g_modal = previousModal;
        Reset();
        return 0;
    }
    // The window is still hidden: this is the last frame that shows the grid
    // without the menu on top of it.
    CaptureBackdrop(m);
    m.appearStart = Now();
    Paint(m);
    ShowWindow(m.hwnd, SW_SHOWNA);
    if (!SetForegroundWindow(m.hwnd)) SetActiveWindow(m.hwnd);
    SetFocus(m.hwnd);
    SetCapture(m.hwnd);
    SetTimer(m.hwnd, kAnimationTimer, 8, nullptr);

    MSG message;
    int quitCode = 0;
    bool quit = false;
    while (m.running) {
        DWORD ready = MsgWaitForMultipleObjectsEx(0, nullptr, INFINITE,
                                                  QS_ALLINPUT,
                                                  MWMO_INPUTAVAILABLE);
        if (ready == WAIT_FAILED) break;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                quit = true;
                quitCode = (int)message.wParam;
                m.running = false;
                break;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
            if (!m.running) break;
        }
    }

    if (GetCapture() == m.hwnd) ReleaseCapture();
    if (m.hwnd) DestroyWindow(m.hwnd);
    m.hwnd = nullptr;
    UINT result = m.result;
    Reset();
    g_modal = previousModal;
    Wh_Log(L"menu: closed with command %u", result);
    if (!quit && g_shown && owner && IsWindowVisible(owner)) {
        SetForegroundWindow(owner);
    }
    if (quit) PostQuitMessage(quitCode);
    return result;
}

#ifdef SEAMLESS_PREVIEW
// Offscreen menu render for the smoke harness: lays a menu out exactly as the
// window would and draws it over the launcher's last frame, so the material can
// be checked without a window, input or a screen capture. hover and submenu
// select an item and a level to open. The result is left in
// LaunchpadMenu::g_menu.surface.
bool PreviewRenderMenu(HMENU menu, int x, int y, UINT flags, int hover = -1,
                       int submenu = -1) {
    using namespace LaunchpadMenu;
    State& m = g_menu;
    if (m.running) return false;
    Reset();
    if (!Prepare(m, menu, x, y, flags)) return false;
    CaptureBackdrop(m);
    if (hover >= 0 && !m.panels.empty() &&
        hover < (int)m.panels[0].items->size()) {
        m.panels[0].hover = hover;
    }
    if (submenu >= 0 && !m.panels.empty() &&
        submenu < (int)m.panels[0].items->size()) {
        OpenSubmenu(0, submenu);
        if (m.panels.size() > 1) m.panels[1].hover = 1;
    }
    return DrawFrame(m);
}
#endif

void EndDrag(bool drop);
void StartDrag();
void DragTick();

constexpr float kTwoPi = 6.2831853f;

float Ease(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    if (g_settings.easing == L"settle") return t >= 1 ? 1 : (1 - std::exp(-6.0f * t)) / (1 - std::exp(-6.0f));
    if (g_settings.easing == L"quint") return 1 - std::pow(1 - t, 5.0f);
    return 1 - (1 - t) * (1 - t) * (1 - t);
}

// A button's hit area runs a little past what is drawn.
bool InButton(const D2D1_RECT_F& r, float x, float y) {
    float pad = 6 * g_layout.scale;
    return InRect(D2D1::RectF(r.left - pad, r.top - pad, r.right + pad,
                              r.bottom + pad),
                  x, y);
}

void DrawHoverBox(const D2D1_RECT_F& box, float radius, float alpha) {
    if(alpha>.001f)Fill(box,D2D1::ColorF(0xFFFFFF,alpha*.20f*g_settings.glowStrength),radius);
}

void DrawClearButton(float opacity) {
    if (g_openFolder || g_query.empty()) {
        return;
    }
    const D2D1_RECT_F& r = g_layout.clearButton;
    float s = g_layout.scale;
    float radius = (r.bottom - r.top) / 2;
    if (g_clearHover) {
        Fill(r, D2D1::ColorF(0xFFFFFF, 0.16f * opacity), radius);
    }
    float cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    float arm = 4 * s;
    g_brush->SetColor(D2D1::ColorF(0xFFFFFF, (g_clearHover ? 0.95f : 0.8f) * opacity));
    g_target->DrawLine(D2D1::Point2F(cx - arm, cy - arm),
                       D2D1::Point2F(cx + arm, cy + arm), g_brush.Get(),
                       1.4f * s);
    g_target->DrawLine(D2D1::Point2F(cx - arm, cy + arm),
                       D2D1::Point2F(cx + arm, cy - arm), g_brush.Get(),
                       1.4f * s);
}

////////////////////////////////////////////////////////////////////////////////
// Power dock: a solid Mocha pill of round buttons at the bottom, aligned by
// the dock position setting. Without "Skip confirmation", the first click
// arms a button (colored, "Click again to ...") and a second click on it
// runs the action.

int g_powerHover = -1;    // Slot in g_layout.powerItems.
int g_powerPressed = -1;
int g_powerArmed = -1;
// Eased glows, one per button (by kPower index) and one for the dock border.
float g_powerGlow[6];
float g_powerDockGlow;

void HideNow();
void StartFrames();

void RefreshPowerCaps() {
    SYSTEM_POWER_CAPABILITIES caps{};
    if (GetPwrCapabilities(&caps)) {
        // MinGW's header calls AoAc (modern standby) spare2[2]; the Windows
        // SDK names the three bytes Hiberboot, WakeAlarmPresent and AoAc.
        g_canSleep = caps.SystemS1 || caps.SystemS2 || caps.SystemS3 ||
                     caps.spare2[2];
        g_canHibernate = caps.SystemS4 && caps.HiberFilePresent;
    }
}

D2D1_RECT_F PowerButtonRect(int slot) {
    const Layout& L = g_layout;
    float pad = (L.powerBar.bottom - L.powerBar.top - L.powerButton) / 2;
    float left = L.powerBar.left + pad + slot * (L.powerButton + L.powerGap);
    float top = L.powerBar.top + pad;
    return D2D1::RectF(left, top, left + L.powerButton, top + L.powerButton);
}

// The slot under the point, counting the gaps so the bar has no dead spots.
int PowerAt(int x, int y) {
    const Layout& L = g_layout;
    if (g_batchBlend>0 || !g_appSelection.empty() || L.powerItems.empty() || !InRect(L.powerBar, (float)x, (float)y)) {
        return -1;
    }
    float pad = (L.powerBar.bottom - L.powerBar.top - L.powerButton) / 2;
    float step = L.powerButton + L.powerGap;
    int slot = (int)std::floor((x - L.powerBar.left - pad + L.powerGap / 2) /
                               step);
    return std::clamp(slot, 0, (int)L.powerItems.size() - 1);
}

void DisarmPower() {
    if (g_powerArmed >= 0) {
        g_powerArmed = -1;
        KillTimer(g_wnd, kTimerPowerArm);
        StartFrames();  // The glow eases back out.
    }
}

void EnablePrivilege(PCWSTR name) {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
        return;
    }
    TOKEN_PRIVILEGES tp{1};
    if (LookupPrivilegeValueW(nullptr, name, &tp.Privileges[0].Luid)) {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(token, FALSE, &tp, 0, nullptr, nullptr);
    }
    CloseHandle(token);
}

// Shut down like Start does: a fast startup shutdown when that's on.
bool FastStartupOn() {
    DWORD value = 0, size = sizeof(value);
    return RegGetValueW(HKEY_LOCAL_MACHINE,
                        L"SYSTEM\\CurrentControlSet\\Control\\Session "
                        L"Manager\\Power",
                        L"HiberbootEnabled", RRF_RT_REG_DWORD, nullptr, &value,
                        &size) == ERROR_SUCCESS &&
           value == 1;
}

// Off the UI thread: sleep and hibernate return only after the PC wakes.
// The handle is kept and joined before the mod unloads: Windhawk calls
// FreeLibrary right after Wh_ModUninit, so no mod thread may outlive it.
HANDLE g_powerThread = nullptr;

DWORD WINAPI PowerThread(void* param) {
    auto action = (PowerAction)(INT_PTR)param;
    // Never EWX_FORCE: apps with unsaved work get to ask, as with Start.
    DWORD reason = SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER |
                   SHTDN_REASON_FLAG_PLANNED;
    BOOL ok = TRUE;
    switch (action) {
        case PowerAction::Lock:
            ok = LockWorkStation();
            break;
        case PowerAction::SignOut:
            ok = ExitWindowsEx(EWX_LOGOFF, reason);
            break;
        case PowerAction::Sleep:
        case PowerAction::Hibernate:
            EnablePrivilege(SE_SHUTDOWN_NAME);
            ok = SetSuspendState(action == PowerAction::Hibernate, FALSE,
                                 FALSE);
            if (!ok && action == PowerAction::Sleep) {
                // Modern standby PCs can't be suspended by a program;
                // turning the screen off lets them go to sleep.
                if (HWND wnd = g_wnd) {
                    PostMessageW(wnd, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
                    ok = TRUE;
                }
            }
            break;
        case PowerAction::Restart:
            EnablePrivilege(SE_SHUTDOWN_NAME);
            ok = ExitWindowsEx(EWX_REBOOT, reason);
            break;
        case PowerAction::ShutDown:
            EnablePrivilege(SE_SHUTDOWN_NAME);
            ok = ExitWindowsEx(
                EWX_POWEROFF | (FastStartupOn() ? EWX_HYBRID_SHUTDOWN : 0),
                reason);
            break;
    }
    if (!ok) {
        Wh_Log(L"Power action %s failed: %u", kPower[(int)action].key,
               GetLastError());
    }
    return 0;
}

void RunPowerAction(PowerAction action) {
#ifdef SEAMLESS_PREVIEW
    MessageBoxW(g_wnd, Tr(L"Preview: power actions are disabled."), L"RYJS Launchpad", MB_OK | MB_ICONINFORMATION);
    return;
#endif
    Wh_Log(L"Power action: %s", kPower[(int)action].key);
    DisarmPower();
    HideNow();  // At once: no closing animation over a lock or sleep.
    if (g_powerThread) {
        if (WaitForSingleObject(g_powerThread, 0) != WAIT_OBJECT_0) {
            Wh_Log(L"Power action already in progress");
            return;
        }
        CloseHandle(g_powerThread);
        g_powerThread = nullptr;
    }
    g_powerThread = CreateThread(nullptr, 0, PowerThread,
                                 (void*)(INT_PTR)action, 0, nullptr);
    if (!g_powerThread) {
        Wh_Log(L"Power thread failed: %u", GetLastError());
    }
}

void PowerClick(int slot) {
    if (slot < 0 || slot >= (int)g_layout.powerItems.size()) {
        return;
    }
    if (!g_settings.powerNoConfirm && g_powerArmed != slot) {
        g_powerArmed = slot;
        SetTimer(g_wnd, kTimerPowerArm, 4000, nullptr);
        StartFrames();  // The armed glow fades in.
        return;
    }
    RunPowerAction((PowerAction)g_layout.powerItems[slot]);
}

D2D1_RECT_F BatchBar() {
    float s=g_layout.scale;
    float width=std::min(312*s,std::max(0.f,g_layout.width-16*s));
    float bottom=g_layout.height-std::round(std::max(22*s,g_layout.height*.028f));
    float cx=g_layout.width*.5f;
    return {cx-width*.5f,bottom-46*s,cx+width*.5f,bottom};
}
D2D1_RECT_F ToolBarBounds() {
    auto batch=BatchBar();
    auto normal=g_layout.powerBar;
    if(g_layout.powerItems.empty())normal={(batch.left+batch.right)*.5f,batch.top,(batch.left+batch.right)*.5f,batch.bottom};
    float t=Ease(g_batchBlend);
    return {std::lerp(normal.left,batch.left,t),std::lerp(normal.top,batch.top,t),
            std::lerp(normal.right,batch.right,t),std::lerp(normal.bottom,batch.bottom,t)};
}
int BatchActionAt(int x,int y) {
    if(g_appSelection.empty()||g_batchBlend<1.f||g_batchLaunching)return -1;
    auto bar=BatchBar();
    if(!InRect(bar,(float)x,(float)y))return -1;
    float fraction=(x-bar.left)/(bar.right-bar.left);
    return fraction<.29f?0:fraction>=.63f?1:-1;
}
bool ToolBarConsumes(int x,int y) {
    return (g_batchBlend>0||!g_appSelection.empty())&&
        (InRect(ToolBarBounds(),(float)x,(float)y)||
         (g_batchBlend<1.f&&!g_layout.powerItems.empty()&&InRect(g_layout.powerBar,(float)x,(float)y))||
         (!g_appSelection.empty()&&InRect(BatchBar(),(float)x,(float)y)));
}
void DrawGlassSurface(const D2D1_RECT_F& bar,float radius,float opacity,float hover) {
    if(!SelectionHasArea(bar)||opacity<=0)return;
    float shift=2*g_layout.scale;
    Fill({bar.left,bar.top+shift,bar.right,bar.bottom+shift},D2D1::ColorF(0,opacity*.14f),radius);
    Fill(bar,D2D1::ColorF(0xE8E8E8,opacity*(.07f+.17f*g_settings.chromeOpacity)),radius);
    g_brush->SetColor(D2D1::ColorF(0xFFFFFF,opacity*(.22f+.20f*hover)));
    g_target->DrawRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(bar.left+.5f,bar.top+.5f,bar.right-.5f,bar.bottom-.5f),
                          radius-.5f,radius-.5f),g_brush.Get(),1);
}
void DrawGlassPill(const D2D1_RECT_F& bar,float opacity,float hover) {
    DrawGlassSurface(bar,(bar.bottom-bar.top)/2,opacity,hover);
}
void DrawToolBarPill(const D2D1_RECT_F& bar,float opacity) {
    DrawGlassPill(bar,opacity,g_powerDockGlow);
}

void DrawPowerBarContent(float opacity) {
    const Layout& L=g_layout;
    if(L.powerItems.empty())return;
    float s=L.scale;
    const auto& bar=L.powerBar;
    float radius=(bar.bottom-bar.top)*.5f;
    DrawHoverBox(bar,radius,g_powerDockGlow*opacity);

    for (int slot = 0; slot < (int)L.powerItems.size(); slot++) {
        const PowerButton& b = kPower[L.powerItems[slot]];
        D2D1_RECT_F r = PowerButtonRect(slot);
        float half = L.powerButton / 2;
        bool hover = slot == g_powerHover && !g_openFolder;
        bool pressed = hover && slot == g_powerPressed;
        float glow=g_powerGlow[L.powerItems[slot]];
        float highlight=slot==g_powerArmed?.33f:pressed?.26f:.14f*glow*glow*(3-2*glow);
        Fill(r,D2D1::ColorF(0xFFFFFF,highlight*opacity),half);
        auto glyph=D2D1::ColorF(0xFFFFFF,(pressed?.77f:1.f)*opacity);
        if (g_iconFormat) {
            g_brush->SetColor(glyph);
            g_target->DrawText(&b.glyph, 1, g_iconFormat.Get(), r,
                               g_brush.Get());
        }
    }

    // The label of the button under the mouse, or the confirmation prompt,
    // in a small pill above the bar.
    int tipSlot = g_powerArmed >= 0 ? g_powerArmed
                  : g_openFolder    ? -1
                                    : g_powerHover;
    if (tipSlot < 0 || !g_tipFormat) {
        return;
    }
    const PowerButton& b = kPower[L.powerItems[tipSlot]];
    std::wstring text = tipSlot == g_powerArmed
                            ? launchpad_i18n::Text(std::wstring(L"Click again to ") + b.verb)
                            : std::wstring(Tr(b.label));
    ComPtr<IDWriteTextLayout> layout;
    g_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(),
                               g_tipFormat.Get(), 1000 * s, 100 * s, &layout);
    if (!layout) {
        return;
    }
    DWRITE_TEXT_METRICS m{};
    layout->GetMetrics(&m);
    D2D1_RECT_F button = PowerButtonRect(tipSlot);
    float w = std::ceil(m.width) + 24 * s, h = std::round(28 * s);
    float cx = (button.left + button.right) / 2;
    float left = std::round(std::clamp(cx - w / 2, 8 * s, L.width - w - 8 * s));
    float bottom = bar.top - std::round(8 * s);
    D2D1_RECT_F tip = D2D1::RectF(left, bottom - h, left + w, bottom);
    DrawGlassPill(tip,opacity,0);
    g_brush->SetColor(D2D1::ColorF(0xFFFFFF,opacity));
    g_target->DrawText(text.c_str(), (UINT32)text.size(), g_tipFormat.Get(),
                       tip, g_brush.Get());
}
std::wstring CountLabel(PCWSTR key,size_t count) {
    std::wstring text=Tr(key);
    auto offset=text.find(L"{0}");
    if(offset!=std::wstring::npos)text.replace(offset,3,std::to_wstring(count));
    return text;
}
void DrawBatchBar(float sceneOpacity) {
    float opacity=Ease(g_batchBlend)*sceneOpacity;
    auto bar=BatchBar();
    float s=g_layout.scale,width=bar.right-bar.left;
    float left=bar.left+5*s,right=bar.right-5*s;
    float middle1=bar.left+width*.29f,middle2=bar.left+width*.63f;
    D2D1_RECT_F slots[]{{left,bar.top+5*s,middle1,bar.bottom-5*s},
                        {middle1,bar.top,middle2,bar.bottom},
                        {middle2,bar.top+5*s,right,bar.bottom-5*s}};
    std::wstring labels[]{Tr(L"Cancel"),CountLabel(L"Selected {0}",g_batchDisplayCount),
                          CountLabel(L"Open {0}",g_batchDisplayCount)};
    for(int i=0;i<3;i++) {
        auto color=D2D1::ColorF(0xFFFFFF,opacity);
        int action=i==0?0:i==2?1:-1;
        if(action>=0) {
            bool hover=g_batchHover==action;
            auto accent=D2D1::ColorF(0xFFFFFF);
            accent.a=opacity*(hover?(g_batchPressed==action?.36f:.25f):i==2?.14f:0.f);
            Fill(slots[i],accent,18*s);
            if(i==2)color=D2D1::ColorF(0xFFFFFF,opacity);
        }
        g_brush->SetColor(color);
        g_target->DrawText(labels[i].c_str(),(UINT32)labels[i].size(),g_tipFormat.Get(),slots[i],g_brush.Get());
    }
}
void DrawToolBar(bool afterFolder, float opacity) {
    if (opacity <= 0.f) return;
    float blend=Ease(g_batchBlend);
    if(afterFolder&&(!g_openFolder||blend<=0))return;
    if(afterFolder||!g_openFolder||blend<=0)
        DrawToolBarPill(ToolBarBounds(),opacity*(g_layout.powerItems.empty()?blend:1.f));
    if(!afterFolder&&!g_layout.powerItems.empty()&&blend<1) {
        if(blend>0)g_target->PushLayer(D2D1::LayerParameters1(D2D1::InfiniteRect(),nullptr,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,D2D1::IdentityMatrix(),1-blend),nullptr);
        DrawPowerBarContent(opacity);
        if(blend>0)g_target->PopLayer();
    }
    if(blend>0&&(afterFolder||!g_openFolder))DrawBatchBar(opacity);
}
void DrawAppSelectionRect() {
    if(!g_selectionPointer.active)return;
    auto r=SelectionIntersection(SelectionRect(),SelectionViewport());
    if(!SelectionHasArea(r))return;
    Fill(r,D2D1::ColorF(0xFFFFFF,.13f),3*g_layout.scale);
    g_brush->SetColor(D2D1::ColorF(0xFFFFFF,.80f));
    g_target->DrawRectangle(r,g_brush.Get(),g_layout.scale);
}


////////////////////////////////////////////////////////////////////////////////
// The search field as a text editor: caret, selection and the usual keys.
// Positions are UTF-16 indices into g_query; a step crosses a whole surrogate
// pair so characters outside the basic plane stay whole.

void QueryChanged();
void Render();

// The field the editor works on: the search box, or a folder's name while
// its folder is open.
std::wstring& EditText() {
    return g_openFolder ? g_openFolder->name : g_query;
}

size_t MaxEditChars() {
    return g_openFolder ? 60 : 200;
}

// A rename only redraws; the search also refilters the apps.
void EditChanged() {
    if (g_openFolder) {
        Render();
    } else {
        QueryChanged();
    }
}

bool SearchSelection(size_t* from, size_t* to) {
    size_t size = EditText().size();
    *from = std::min(std::min(g_anchor, g_caret), size);
    *to = std::min(std::max(g_anchor, g_caret), size);
    return *from != *to;
}

size_t CaretStep(size_t pos, int dir) {
    const std::wstring& text = EditText();
    if (dir < 0) {
        pos = std::min(pos, text.size());
        if (pos >= 2 && IS_LOW_SURROGATE(text[pos - 1]) &&
            IS_HIGH_SURROGATE(text[pos - 2])) {
            return pos - 2;
        }
        return pos ? pos - 1 : 0;
    }
    if (pos + 1 < text.size() && IS_HIGH_SURROGATE(text[pos]) &&
        IS_LOW_SURROGATE(text[pos + 1])) {
        return pos + 2;
    }
    return std::min(pos + 1, text.size());
}

bool IsWordChar(WCHAR ch) {
    return iswalnum(ch) || ch == L'_';
}

size_t WordStart(size_t pos) {
    const std::wstring& text = EditText();
    size_t i = std::min(pos, text.size());
    while (i > 0 && !IsWordChar(text[i - 1])) {
        i--;
    }
    while (i > 0 && IsWordChar(text[i - 1])) {
        i--;
    }
    return i;
}

size_t WordEnd(size_t pos) {
    const std::wstring& text = EditText();
    size_t n = text.size(), i = std::min(pos, n);
    while (i < n && !IsWordChar(text[i])) {
        i++;
    }
    while (i < n && IsWordChar(text[i])) {
        i++;
    }
    return i;
}

void TouchCaret() {  // Restarts the blink, like after every keystroke.
    g_caretTick = GetTickCount64();
}

// Undo and redo for the field being edited. A snapshot is the whole field,
// which is small. Consecutive keystrokes of one kind merge into one step;
// pastes and cuts stay their own step. The folder's history resets each time
// a folder opens, so one folder's steps never land on another's name.
struct EditSnapshot {
    std::wstring text;
    size_t caret;
    size_t anchor;
};
struct EditHistory {
    std::vector<EditSnapshot> undo;
    std::vector<EditSnapshot> redo;
    int lastOp = 0;  // 0 paste or cut, 1 typing, 2 delete
    size_t lastEnd = 0;
    ULONGLONG lastTick = 0;
};
EditHistory g_hist[2];

void EditPush(int op) {  // Snapshot the field before a mutation.
    EditHistory& h = g_hist[g_openFolder != nullptr];
    ULONGLONG now = GetTickCount64();
    bool merge = (op == 1 || op == 2) && op == h.lastOp &&
                 now - h.lastTick < 1000 && g_caret == h.lastEnd &&
                 !h.undo.empty();
    if (!merge) {
        h.undo.push_back({EditText(), g_caret, g_anchor});
        if (h.undo.size() > 100) {
            h.undo.erase(h.undo.begin());
        }
        h.redo.clear();
    }
    h.lastOp = op;
    h.lastTick = now;
}

void EditDone() {  // Remember where the mutation left the caret, for merging.
    g_hist[g_openFolder != nullptr].lastEnd = g_caret;
}

void UndoEdit() {
    EditHistory& h = g_hist[g_openFolder != nullptr];
    if (h.undo.empty()) {
        return;
    }
    h.redo.push_back({EditText(), g_caret, g_anchor});
    if (h.redo.size() > 100) {
        h.redo.erase(h.redo.begin());
    }
    EditSnapshot s = h.undo.back();
    h.undo.pop_back();
    EditText() = s.text;
    g_caret = std::min(s.caret, EditText().size());
    g_anchor = std::min(s.anchor, EditText().size());
    h.lastOp = 0;
    TouchCaret();
    EditChanged();
}

void RedoEdit() {
    EditHistory& h = g_hist[g_openFolder != nullptr];
    if (h.redo.empty()) {
        return;
    }
    h.undo.push_back({EditText(), g_caret, g_anchor});
    if (h.undo.size() > 100) {
        h.undo.erase(h.undo.begin());
    }
    EditSnapshot s = h.redo.back();
    h.redo.pop_back();
    EditText() = s.text;
    g_caret = std::min(s.caret, EditText().size());
    g_anchor = std::min(s.anchor, EditText().size());
    h.lastOp = 0;
    TouchCaret();
    EditChanged();
}

bool CaretVisible() {
    UINT blink = GetCaretBlinkTime();
    if (!blink || blink > 5000) {
        blink = 530;  // The Windows default.
    }
    return (GetTickCount64() - g_caretTick) / blink % 2 == 0;
}

void SetCaret(size_t pos, bool extend) {
    g_caret = std::min(pos, EditText().size());
    if (!extend) {
        g_anchor = g_caret;
    }
    g_hist[g_openFolder != nullptr].lastOp = 0;  // A caret move starts a new
    TouchCaret();                                // undo step, like anywhere.
}

bool DeleteSelection() {
    size_t from, to;
    if (!SearchSelection(&from, &to)) {
        return false;
    }
    EditText().erase(from, to - from);
    g_caret = g_anchor = from;
    TouchCaret();
    return true;
}

void InsertAtCaret(std::wstring text) {
    DeleteSelection();
    size_t max = MaxEditChars();
    size_t room = max - std::min(EditText().size(), max);
    if (text.size() > room) {
        text.resize(room);
    }
    if (text.empty()) {
        return;
    }
    g_caret = std::min(g_caret, EditText().size());
    EditText().insert(g_caret, text);
    g_caret = g_anchor = g_caret + text.size();
    TouchCaret();
}

void CopySelection(bool cut) {
    size_t from, to;
    if (!SearchSelection(&from, &to) || !OpenClipboard(g_wnd)) {
        return;
    }
    if (EmptyClipboard()) {
        if (HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE,
                                      (to - from + 1) * sizeof(WCHAR))) {
            if (WCHAR* p = (WCHAR*)GlobalLock(mem)) {
                memcpy(p, EditText().c_str() + from,
                       (to - from) * sizeof(WCHAR));
                p[to - from] = 0;
                GlobalUnlock(mem);
                if (!SetClipboardData(CF_UNICODETEXT, mem)) {
                    GlobalFree(mem);
                }
            } else {
                GlobalFree(mem);
            }
        }
    }
    CloseClipboard();
    if (cut) {
        EditPush(0);
        DeleteSelection();
        EditDone();
        EditChanged();
    }
}

// The search field's text box, or the open folder's name.
D2D1_RECT_F FolderTitlePill() {
    auto title = GetFolderLayout().title;
    float scale = g_layout.scale;
    float width = 140 * scale;
    if (g_openFolder && g_dwrite && g_messageFormat) {
        ComPtr<IDWriteTextLayout> layout;
        const auto& name = g_openFolder->name;
        if (SUCCEEDED(g_dwrite->CreateTextLayout(name.c_str(), (UINT32)name.size(),
                g_messageFormat.Get(), 10000, 100 * scale, &layout))) {
            DWRITE_TEXT_METRICS metrics{};
            layout->GetMetrics(&metrics);
            width = std::max(width, metrics.widthIncludingTrailingWhitespace + 36 * scale);
        }
    }
    width = std::min(width, title.right-title.left);
    float cx = (title.left+title.right)/2, cy = (title.top+title.bottom)/2;
    return D2D1::RectF(cx-width/2, cy-17*scale, cx+width/2, cy+17*scale);
}

D2D1_RECT_F EditBox(bool folder) {
    if (folder) {
        auto r = FolderTitlePill();
        r.left += 14 * g_layout.scale;
        r.right -= 14 * g_layout.scale;
        return r;
    }
    const D2D1_RECT_F& r = g_layout.search;
    float s = g_layout.scale;
    if (!SearchExpanded()) return r;
    float right = g_query.empty() ? r.right - 14*s : g_layout.clearButton.left - 4*s;
    return D2D1::RectF(r.left+36*s, r.top, std::max(r.left+36*s, right), r.bottom);
}

// A text field as drawn: its layout, the box it is clipped to, and where the
// layout's left edge sits. Text too long for the box slides so the caret
// stays in view; a folder name that fits stays centered. A null layout when
// there is no format.
struct EditView {
    ComPtr<IDWriteTextLayout> layout;
    D2D1_RECT_F box;
    float originX,originY;
};
float SearchTextTop(const D2D1_RECT_F& box) {
    static UINT version=0;
    static float inkCenter=0;
    if(version!=g_formatVersion&&g_searchFormat) {
        version=g_formatVersion;
        ComPtr<IDWriteTextLayout> sample;
        if(SUCCEEDED(g_dwrite->CreateTextLayout(L"Ag中",3,g_searchFormat.Get(),1000,100,&sample))) {
            sample->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            DWRITE_OVERHANG_METRICS ink{};
            if(SUCCEEDED(sample->GetOverhangMetrics(&ink)))inkCenter=(100+ink.bottom-ink.top)/2;
        }
    }
    return (box.top+box.bottom)/2-inkCenter;
}

EditView GetEditView(bool folder) {
    EditView v{};
    v.box = EditBox(folder);
    v.originX = v.box.left;
    v.originY = folder?v.box.top:SearchTextTop(v.box);
    IDWriteTextFormat* format =
        folder ? g_messageFormat.Get() : g_searchFormat.Get();
    if (!format || (folder && !g_openFolder)) {
        return v;
    }
    const std::wstring& text = folder ? g_openFolder->name : g_query;
    g_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(), format,
                               10000, v.box.bottom - v.box.top, &v.layout);
    if (!v.layout) {
        return v;
    }
    v.layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    if(!folder)v.layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    DWRITE_TEXT_METRICS metrics{};
    v.layout->GetMetrics(&metrics);
    float width = metrics.widthIncludingTrailingWhitespace;
    float room = v.box.right - v.box.left - 3 * g_layout.scale;
    if (folder && width <= room) {
        v.originX =
            std::round(v.box.left + (v.box.right - v.box.left - width) / 2);
        return v;
    }
    float endShift = std::max(0.0f, width - room);
    float shift = endShift;
    DWRITE_HIT_TEST_METRICS m{};
    float x = 0, y = 0;
    bool editing = folder ? g_folderRenaming : !g_openFolder;
    if (editing &&
        SUCCEEDED(v.layout->HitTestTextPosition(
            (UINT32)std::min(g_caret, text.size()), FALSE, &x, &y, &m))) {
        if (x - shift < 0) {
            shift = std::min(std::max(x, 0.0f), endShift);
        }
        if (x - shift > room) {
            shift = std::min(std::max(x - room, 0.0f), endShift);
        }
    }
    v.originX = v.box.left - shift;
    return v;
}

// Position the Windows IME composition/candidate UI by the custom-drawn caret.
// Committed text goes through the same undo/filter/persistence path as typing.
bool g_imeComposing = false;
void PositionIme(HWND hwnd) {
    HIMC ime = ImmGetContext(hwnd);
    if (!ime) return;
    EditView v = GetEditView(g_openFolder != nullptr);
    float x = 0, y = 0;
    DWRITE_HIT_TEST_METRICS metrics{};
    if (v.layout) v.layout->HitTestTextPosition((UINT32)std::min(g_caret, EditText().size()), FALSE, &x, &y, &metrics);
    POINT caret{(LONG)std::clamp(v.originX + x, v.box.left, v.box.right), (LONG)v.box.bottom};
    COMPOSITIONFORM composition{};
    composition.dwStyle = CFS_POINT;
    composition.ptCurrentPos = caret;
    ImmSetCompositionWindow(ime, &composition);
    CANDIDATEFORM candidate{};
    candidate.dwStyle = CFS_EXCLUDE;
    candidate.ptCurrentPos = caret;
    candidate.rcArea = {(LONG)v.box.left, (LONG)v.box.top, (LONG)v.box.right, (LONG)v.box.bottom};
    ImmSetCandidateWindow(ime, &candidate);
    LOGFONTW font{};
    font.lfHeight = -(LONG)std::lround(15 * g_layout.scale);
    lstrcpynW(font.lfFaceName, g_settings.font.c_str(), LF_FACESIZE);
    ImmSetCompositionFontW(ime, &font);
    ImmReleaseContext(hwnd, ime);
}

// The caret index under a mouse x, in the field being edited.
size_t CaretAtPoint(float x) {
    EditView v = GetEditView(g_openFolder != nullptr);
    size_t size = EditText().size();
    if (!v.layout) {
        return size;
    }
    BOOL trailing = FALSE, inside = FALSE;
    DWRITE_HIT_TEST_METRICS m{};
    float px = std::max(
        std::min(std::max(x, v.box.left), v.box.right) - v.originX, 0.0f);
    if (FAILED(v.layout->HitTestPoint(px, (v.box.bottom + v.box.top) / 2-v.originY,
                                      &trailing, &inside, &m))) {
        return size;
    }
    return std::min((size_t)(m.textPosition + (trailing ? m.length : 0)), size);
}

// The text, with the selection behind it and a caret that blinks like
// everywhere else in Windows, when this is the field being edited.
void DrawEditView(const EditView& v, bool editing, float caretHeight, float opacity = 1.f) {
    if (!v.layout) {
        return;
    }
    const D2D1_RECT_F& box = v.box;
    float caretW = std::max(1.0f, g_layout.scale);
    g_target->PushAxisAlignedClip(
        D2D1::RectF(box.left - caretW - 1, box.top, box.right, box.bottom),
        D2D1_ANTIALIAS_MODE_ALIASED);
    size_t selFrom, selTo;
    if (editing && SearchSelection(&selFrom, &selTo)) {
        DWRITE_HIT_TEST_METRICS rects[8];
        UINT32 count = 0;
        if (SUCCEEDED(v.layout->HitTestTextRange((UINT32)selFrom,
                                                 (UINT32)(selTo - selFrom), 0,
                                                 0, rects, 8, &count))) {
            g_brush->SetColor(D2D1::ColorF(0xFFFFFF, 0.35f * opacity));
            for (UINT32 i = 0; i < count && i < 8; i++) {
                g_target->FillRectangle(
                    D2D1::RectF(v.originX + rects[i].left,
                                v.originY + rects[i].top,
                                v.originX + rects[i].left + rects[i].width,
                                v.originY + rects[i].top + rects[i].height),
                    g_brush.Get());
            }
        }
    }
    g_brush->SetColor(D2D1::ColorF(0xFFFFFF, opacity));
    g_target->DrawTextLayout(D2D1::Point2F(v.originX, v.originY), v.layout.Get(),
                             g_brush.Get());
    if (editing && CaretVisible()) {
        DWRITE_HIT_TEST_METRICS m{};
        float x = 0, y = 0;
        v.layout->HitTestTextPosition(
            (UINT32)std::min(g_caret, EditText().size()), FALSE, &x, &y, &m);
        // Just left of the insertion point, so it never touches the first
        // glyph.
        float caretX = std::round(v.originX + (float)x) - caretW;
        float mid = (box.top + box.bottom) / 2;
        Fill(D2D1::RectF(caretX, mid - caretHeight / 2, caretX + caretW,
                         mid + caretHeight / 2),
             D2D1::ColorF(0xFFFFFF, 0.9f * opacity));
    }
    g_target->PopAxisAlignedClip();
}

void DrawSearchBox(float opacity) {
    const Layout& L = g_layout;
    float s = L.scale;
    const D2D1_RECT_F& r = L.search;
    // No resting icon any more: the whole field fades in only while opening,
    // so at rest the top of the window is just the grid, one row higher.
    float t = std::clamp(g_searchExpand, 0.f, 1.f);
    if (t < 0.02f) return;
    float fade = std::min(1.f, t * 6.f);
    t = t * t * (3 - 2 * t);
    DrawGlassPill(r,opacity*fade,g_searchGlow*fade);

    float lens=5.5f*s, handle=8.5f*s;
    // The magnifier rides along with the opening: centred in the pill, 18 px in
    // once the field is out.
    float midX = (r.left + r.right) * 0.5f +
                 (r.left + 18 * s - (r.left + r.right) * 0.5f) * t;
    float cx=midX+(lens-handle)/2,cy=(r.top+r.bottom+lens-handle)/2;
    g_brush->SetColor(D2D1::ColorF(0xFFFFFF,.85f*opacity*fade));
    g_target->DrawEllipse(D2D1::Ellipse({cx,cy},lens,lens),g_brush.Get(),1.6f*s);
    g_target->DrawLine({cx+4*s,cy+4*s},{cx+handle,cy+handle},g_brush.Get(),1.6f*s);

    // The text follows the box out and is clipped to it, so it slides in
    // instead of popping up at full width.
    if (g_searchExpand < 0.02f) return;
    auto edit = GetEditView(false);
    if (g_query.empty() && g_searchFormat) {
        auto hint = edit.box;
        hint.left += 4*s;
        PCWSTR prompt = Tr(L"Search");
        g_brush->SetColor(D2D1::ColorF(0xFFFFFF, .57f*opacity*fade));
        g_target->DrawText(prompt, (UINT32)wcslen(prompt), g_searchFormat.Get(),
                           hint, g_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
    DrawEditView(edit, !g_openFolder, 18 * s, opacity * fade);
}

void DrawMessage(PCWSTR text) {
    const Layout& L = g_layout;
    if (!g_messageFormat) {
        return;
    }
    g_brush->SetColor(D2D1::ColorF(0xFFFFFF, 0.85f * FlyInChromeOpacity()));
    g_target->DrawText(text, (UINT32)wcslen(text), g_messageFormat.Get(),
                       D2D1::RectF(0, L.viewTop, (float)L.width,
                                   L.viewTop + 120 * L.scale),
                       g_brush.Get());
}

bool IsDragged(const Tile& tile) {
    return g_drag.active && tile.app == g_drag.tile.app &&
           tile.folder == g_drag.tile.folder;
}

const std::wstring& TileName(const Tile& tile) {
    return tile.folder ? tile.folder->name : tile.app->name;
}

void DrawTileIcon(const Tile& tile, const D2D1_RECT_F& r, float opacity) {
    float side = r.right - r.left;
    if (tile.folder) {
        // A rounded square with the first nine apps.
        Fill(r, D2D1::ColorF(0x424242, 0.55f * opacity), side * 0.22f);
        // A resting edge (Mocha overlay0) so the folder stands off the backdrop.
        float w = 1.25f * g_layout.scale, in = w / 2;
        g_brush->SetColor(D2D1::ColorF(0xD0D0D0, 0.6f * opacity));
        g_target->DrawRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(r.left + in, r.top + in, r.right - in, r.bottom - in),
                              side * 0.22f - in, side * 0.22f - in),
            g_brush.Get(), w);
        float pad = side * 0.12f, gap = side * 0.04f;
        float mini = (side - 2 * pad - 2 * gap) / 3;
        const Tiles& apps = tile.folder->apps;
        for (int i = 0; i < 9 && i < (int)apps.size(); i++) {
            float x = r.left + pad + (i % 3) * (mini + gap);
            float y = r.top + pad + (i / 3) * (mini + gap);
            if (ID2D1Bitmap* bitmap = GetBitmap(*apps[i].app)) {
                g_target->DrawBitmap(bitmap,
                                     D2D1::RectF(x, y, x + mini, y + mini),
                                     opacity,
                                     D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
        }
        return;
    }
    // No backplate: an icon without a bitmap draws as nothing, like
    // Launchpad's blank tiles.
    if (ID2D1Bitmap* bitmap = GetBitmap(*tile.app)) {
        g_target->DrawBitmap(bitmap, r, opacity,
                             D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    } else {
        Fill(r,D2D1::ColorF(0xFFFFFF,0.7f*opacity),side*0.20f);
        auto center=D2D1::Point2F((r.left+r.right)/2,(r.top+r.bottom)/2);
        g_brush->SetColor(D2D1::ColorF(0xFFFFFF,opacity));
        if(IsWebEntry(tile.app->target)) {
            g_target->DrawEllipse(D2D1::Ellipse(center,side*0.28f,side*0.28f),g_brush.Get(),side*0.04f);
            g_target->DrawEllipse(D2D1::Ellipse(center,side*0.12f,side*0.28f),g_brush.Get(),side*0.03f);
            g_target->DrawLine({r.left+side*0.22f,center.y},{r.right-side*0.22f,center.y},g_brush.Get(),side*0.03f);
        } else {
            g_target->DrawRectangle({r.left+side*0.2f,r.top+side*0.22f,r.right-side*0.2f,r.bottom-side*0.28f},g_brush.Get(),side*0.04f);
            g_target->DrawLine({center.x,r.bottom-side*0.28f},{center.x,r.bottom-side*0.15f},g_brush.Get(),side*0.04f);
        }
    }
}

// live: the icon takes the pointer and keys (not the grid behind a folder).
struct TileMotion {
    const void* key;
    D2D1_POINT_2F from;
};
std::vector<TileMotion> g_tileMotion;
double g_tileMotionStart = 0;
bool g_tileMotionAnimating = false;

const void* MotionKey(const Tile& tile) {
    return tile.folder ? (const void*)tile.folder.get() : (const void*)tile.app.get();
}

Cell AnimatedCell(const Tile& tile, Cell cell) {
    if (!Animate()) return cell;
    float entry=(1-GroupTileProgress(tile))*30*g_layout.scale;
    for (auto rect : {&cell.icon,&cell.label,&cell.highlight,&cell.unit}) { rect->left+=entry; rect->right+=entry; }
    float t = std::clamp((float)((Now() - g_tileMotionStart) / 0.20), 0.0f, 1.0f);
    if (t >= 1) return cell;
    for (const auto& motion : g_tileMotion) {
        if (motion.key != MotionKey(tile)) continue;
        float remaining = (1-t)*(1-t)*(1-t);
        float dx = (motion.from.x-cell.icon.left)*remaining;
        float dy = (motion.from.y-cell.icon.top)*remaining;
        for (auto rect : {&cell.icon, &cell.label, &cell.highlight, &cell.unit}) {
            rect->left += dx; rect->right += dx;
            rect->top += dy; rect->bottom += dy;
        }
        g_tileMotionAnimating |= std::abs(dx) > 0.1f || std::abs(dy) > 0.1f;
        break;
    }
    return cell;
}

void CaptureTileMotion() {
    if (!Animate()) { g_tileMotion.clear(); return; }
    std::vector<TileMotion> next;
    const Tiles& tiles = g_openFolder ? g_openFolder->apps : g_visible;
    for (int i = 0; i < (int)tiles.size(); ++i) {
        Cell c = AnimatedCell(tiles[i], g_openFolder ? FolderCellAt(i) : CellAt(i));
        next.push_back({MotionKey(tiles[i]), {c.icon.left, c.icon.top}});
    }
    g_tileMotion = std::move(next);
    g_tileMotionStart = Now();
    StartFrames();
}

struct CachedTileLabel {
    std::wstring name;
    float width = 0, height = 0;
    UINT version = 0;
    bool wrap=false;
    float inkTop = 0, inkBottom = 0;
    ComPtr<IDWriteTextLayout> layout;
    // The name pre-rendered over its soft shadow, padded so the blur fits.
    // Built on a private device context: the main one is inside a BeginDraw
    // session whenever tiles are drawn, and one context cannot nest it.
    ComPtr<ID2D1Bitmap1> shadow;
    float padX=0, padTop=0, padBottom=0;
};
[[clang::no_destroy]] std::map<const void*, CachedTileLabel> g_labelCache;
bool RenderLabelShadowBitmap(CachedTileLabel& cached);
CachedTileLabel& TileLabel(const Tile& tile, const D2D1_RECT_F& box,bool wrap) {
    auto& cached = g_labelCache[MotionKey(tile)];
    const auto& name = TileName(tile);
    float width = box.right - box.left, height = box.bottom - box.top;
    if (!cached.layout || cached.version != g_formatVersion || cached.name != name || cached.width != width || cached.height != height || cached.wrap!=wrap) {
        cached.name = name; cached.width = width; cached.height = height; cached.version = g_formatVersion; cached.wrap=wrap;
        cached.layout.Reset();
        cached.shadow.Reset();
        cached.inkTop = 0;
        cached.inkBottom = height;
        g_dwrite->CreateTextLayout(name.c_str(), (UINT32)name.size(), g_labelFormat.Get(), width, height, &cached.layout);
        if(cached.layout) {
            if(wrap) {
                static UINT metricsVersion=0;
                static DWRITE_LINE_METRICS line{};
                if(metricsVersion!=g_formatVersion) {
                    metricsVersion=g_formatVersion;
                    ComPtr<IDWriteTextLayout> sample;
                    UINT count=0;
                    if(SUCCEEDED(g_dwrite->CreateTextLayout(L"Ag中",3,g_labelFormat.Get(),1000,100,&sample)))
                        sample->GetLineMetrics(&line,1,&count);
                }
                float advance=height/2;
                float baseline=std::clamp(line.baseline+(advance-line.height)/2,0.f,advance);
                cached.layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM,advance,baseline);
                cached.layout->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
            }
            if(height<g_layout.labelHeight)
                cached.layout->SetFontSize(std::max(1.f,height/1.3f),{0,(UINT32)name.size()});
            DWRITE_OVERHANG_METRICS ink{};
            if (SUCCEEDED(cached.layout->GetOverhangMetrics(&ink))) {
                cached.inkTop = std::clamp(-ink.top, 0.f, height);
                cached.inkBottom = std::clamp(height + ink.bottom, cached.inkTop, height);
            }
        }
        // Pre-render text + shadow once per cache rebuild. Best effort: the
        // draw path falls back to plain DrawTextLayout without the bitmap.
        RenderLabelShadowBitmap(cached);
    }
    return cached;
}

// Renders the cached layout as white text over a soft dark shadow into a
// small bitmap. A private device context keeps this legal at any call time:
// the visible context sits inside its own BeginDraw session while the frame
// is drawn, and one context cannot nest BeginDraw on itself. Everything here
// is device-scoped, so the bitmap draws fine from the visible context.
bool RenderLabelShadowBitmap(CachedTileLabel& cached) {
    LabelShadowBitmap out;
    if (!RenderTextWithShadow(cached.layout.Get(), cached.width, cached.height, out))
        return false;
    cached.shadow = out.bitmap;
    cached.padX = out.padX;
    cached.padTop = out.padTop;
    cached.padBottom = out.padBottom;
    return true;
}

Cell SpaceTileContents(const Tile& tile, Cell cell) {
    float height = cell.unit.bottom - cell.unit.top;
    float icon = cell.icon.bottom - cell.icon.top;
    float inkTop = 0, inkHeight = 0;
    if (g_settings.showLabels && g_labelFormat &&
        cell.label.right > cell.label.left && cell.label.bottom > cell.label.top) {
        const auto& label = TileLabel(tile, cell.label, cell.wrapLabel);
        if (label.layout) {
            inkTop = label.inkTop;
            inkHeight = label.inkBottom - inkTop;
        }
    }
    float gap = std::max(0.f, height - icon - inkHeight) / (inkHeight > 0 ? 3 : 2);
    cell.icon.top = cell.unit.top + gap;
    cell.icon.bottom = cell.icon.top + icon;
    float boxHeight = cell.label.bottom - cell.label.top;
    cell.label.top = cell.icon.bottom + gap - inkTop;
    cell.label.bottom = cell.label.top + boxHeight;
    return cell;
}

// The radial lobe is masked by the thin bevel, so distance to the light
// controls local edge intensity continuously, including around corners.
// The inner catch approximates the grazing reflection used by glass shaders.
void DrawTileGlass(const D2D1_RECT_F& unit,D2D1_POINT_2F light,float opacity) {
    float width=unit.right-unit.left,height=unit.bottom-unit.top;
    float side=std::min(width,height),s=g_layout.scale;
    if(side<4||opacity<=.001f)return;
    float bevel=std::min(1.4f*s,side*.045f);
    D2D1_RECT_F face{unit.left+bevel,unit.top+bevel,unit.right-bevel,unit.bottom-bevel};
    auto& glass=g_tileGlass;
    if(!glass.rim||!glass.surface) {
        const D2D1_GRADIENT_STOP surface[]={
            {0,D2D1::ColorF(0xFFFFFF,.14f)},{.35f,D2D1::ColorF(0xFFFFFF,.08f)},
            {.72f,D2D1::ColorF(0xFFFFFF,.024f)},{1,D2D1::ColorF(0xFFFFFF,0)}};
        const D2D1_GRADIENT_STOP rim[]={
            {0,D2D1::ColorF(0xFFFFFF,.82f)},{.24f,D2D1::ColorF(0xFFFFFF,.62f)},
            {.58f,D2D1::ColorF(0xFFFFFF,.21f)},{1,D2D1::ColorF(0xFFFFFF,0)}};
        auto make=[&](auto& stops,auto& brush) {
            if(brush)return;
            ComPtr<ID2D1GradientStopCollection> colors;
            if(SUCCEEDED(g_target->CreateGradientStopCollection(stops,ARRAYSIZE(stops),&colors)))
                g_target->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties({0,0},{0,0},1,1),
                                                   colors.Get(),&brush);
        };
        make(surface,glass.surface);make(rim,glass.rim);
    }
    D2D1_POINT_2F center{unit.left+light.x*width,unit.top+light.y*height};
    auto place=[&](ID2D1RadialGradientBrush* brush,float radius,float strength) {
        brush->SetCenter(center);
        brush->SetRadiusX(radius*width);brush->SetRadiusY(radius*height);
        brush->SetOpacity(opacity*strength);
    };

    // The contact shadow lives in the cell's narrow perimeter, clear of its neighbours.
    Fill({face.left,face.top+bevel,face.right,unit.bottom},D2D1::ColorF(0,opacity*.10f));
    Fill({face.left,face.bottom,face.right,face.bottom+bevel*.5f},D2D1::ColorF(0,opacity*.09f));
    Fill(face,D2D1::ColorF(0xFFFFFF,.035f*opacity));
    if(glass.surface) {
        place(glass.surface.Get(),.34f,1);
        g_target->FillRectangle(face,glass.surface.Get());
    }
    if(glass.rim) {
        float inner=2*bevel;
        D2D1_RECT_F catchRect{face.left+inner,face.top+inner,face.right-inner,face.bottom-inner};
        place(glass.rim.Get(),.51f,1);
        auto catchEdge=[&](D2D1_POINT_2F from,D2D1_POINT_2F to,float distance) {
            float facing=std::clamp(1-distance*1.6f,0.f,1.f);
            glass.rim->SetOpacity(opacity*(.07f+.23f*facing*facing));
            g_target->DrawLine(from,to,glass.rim.Get(),bevel);
        };
        D2D1_POINT_2F tl{catchRect.left,catchRect.top},tr{catchRect.right,catchRect.top};
        D2D1_POINT_2F bl{catchRect.left,catchRect.bottom},br{catchRect.right,catchRect.bottom};
        catchEdge(tl,bl,light.x);catchEdge(tr,br,1-light.x);
        catchEdge(tl,tr,light.y);catchEdge(bl,br,1-light.y);
        place(glass.rim.Get(),.48f,1);
        g_target->DrawRectangle(face,glass.rim.Get(),bevel);
    }
}

void SetGlassScenePointer() {
    g_glassScenePointer.reset();
    if(!g_glassPointer||g_groupEditMode||g_closing||g_drag.active||
       g_selectionPointer.active||g_searchDrag)return;
    D2D1::Matrix3x2F transform;
    g_target->GetTransform(&transform);
    if(transform.Invert()) {
        auto point=transform.TransformPoint(*g_glassPointer);
        if(!ToolBarConsumes((int)point.x,(int)point.y))g_glassScenePointer=point;
    }
}

void DrawTile(const Tile& tile, const Cell& c, int i, bool live,
              float opacity = 1.f, D2D1_POINT_2F labelOffset = {}) {
    if (IsDragged(tile)) {
        return;  // Its place stays empty; the icon follows the pointer.
    }
    float s = g_layout.scale;
    if(live&&!g_groupEditMode&&i==g_selected) {
        auto box=c.highlight;
        box.bottom=std::max(box.bottom,c.label.bottom+labelOffset.y);
        Fill(box,D2D1::ColorF(0xFFFFFF,.16f*opacity),10*s);
    }
    if(live&&!g_groupEditMode&&tile.app) {
        const auto& bounds=c.unit;
        bool on=!g_glassClaimed&&g_glassScenePointer&&
            InRect(bounds,g_glassScenePointer->x,g_glassScenePointer->y);
        if(on)g_glassClaimed=true;
        if(auto fade=UpdateGlow(tile.app.get(),on)) {
            if(on)fade->light={(g_glassScenePointer->x-bounds.left)/(bounds.right-bounds.left),
                               (g_glassScenePointer->y-bounds.top)/(bounds.bottom-bounds.top)};
            DrawTileGlass(bounds,fade->light,Ease(fade->alpha)*opacity);
        }
    }
    D2D1_MATRIX_3X2_F base;
    bool wiggle = g_wiggle > 0.001f;
    if (wiggle) {
        // Edit mode: every icon rocks around its center, neighbours out of
        // phase, with the hold ramping the angle in and out.
        g_target->GetTransform(&base);
        float cx = (c.icon.left + c.icon.right) / 2;
        float cy = (c.icon.top + c.icon.bottom) / 2;
        float angle = 2.4f * g_wiggle *
                      std::sin((float)Now() * kTwoPi * 3.1f +
                               (c.icon.left - c.icon.top) * 0.02f);
        g_target->SetTransform(D2D1::Matrix3x2F::Rotation(
                                    angle, D2D1::Point2F(cx, cy)) *
                                base);
    }
    if(live&&!g_groupEditMode&&tile.folder&&i==g_hover)
        Fill(c.highlight,D2D1::ColorF(0xFFFFFF,.12f*opacity),8*s);
    if(live&&tile.app&&AppSelected(*tile.app)) {
        auto bounds=c.highlight;
        bounds.bottom=std::max(bounds.bottom,c.label.bottom+labelOffset.y+4*s);
        Fill(bounds,D2D1::ColorF(0xFFFFFF,.15f*opacity),7*s);
        g_brush->SetColor(D2D1::ColorF(0xFFFFFF,.72f*opacity));
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(bounds,7*s,7*s),g_brush.Get(),s);
    }
    if (live && i == g_drag.target) {
        float grow = 8 * s;
        Fill(D2D1::RectF(c.icon.left - grow, c.icon.top - grow,
                         c.icon.right + grow, c.icon.bottom + grow),
             D2D1::ColorF(0xFFFFFF, 0.3f*opacity), 18 * s);
    }
    float entryAlpha=GroupTileProgress(tile)*opacity;
    DrawTileIcon(tile, c.icon, (live && i == g_pressed ? 0.6f : 1.0f)*entryAlpha);
    if(tile.app && tile.app->unavailable) {
        auto badge=c.icon;badge.left=badge.right-18*s;badge.bottom=badge.top+18*s;
        Fill(badge,D2D1::ColorF(0xB43B3B,entryAlpha),9*s);
        g_brush->SetColor(D2D1::ColorF(0xFFFFFF,entryAlpha));
        g_target->DrawText(L"!",1,g_tipFormat.Get(),badge,g_brush.Get());
    }
    if (g_settings.showLabels && g_labelFormat && c.label.right > c.label.left && c.label.bottom > c.label.top) {
        auto& cached = TileLabel(tile, c.label,c.wrapLabel);
        if (cached.shadow) {
            // The pre-rendered name with its soft shadow: one draw, padded so
            // the blur has room around the text box.
            g_target->DrawBitmap(cached.shadow.Get(),
                D2D1::RectF(c.label.left + labelOffset.x - cached.padX,
                            c.label.top + labelOffset.y - cached.padTop,
                            c.label.right + labelOffset.x + cached.padX,
                            c.label.bottom + labelOffset.y + cached.padBottom),
                entryAlpha, D2D1_INTERPOLATION_MODE_LINEAR);
        } else {
            auto* text = cached.layout.Get();
            g_brush->SetColor(D2D1::ColorF(0xFFFFFF,entryAlpha));
            if (text) g_target->DrawTextLayout({c.label.left+labelOffset.x, c.label.top+labelOffset.y},
                                               text, g_brush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
    }
    if (wiggle) {
        g_target->SetTransform(base);
    }
}

void DrawFolder() {
    if (!g_openFolder) {
        return;
    }
    const Layout& L = g_layout;
    float s = L.scale;
    FolderLayout F = GetFolderLayout();
    auto folderColor=D2D1::ColorF(0x292929,.62f);
    Fill(F.panel, folderColor, 24 * s);
    g_brush->SetColor(D2D1::ColorF(0xFFFFFF, 0.35f));
    g_target->DrawRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(F.panel.left + 0.5f, F.panel.top + 0.5f,
                                      F.panel.right - 0.5f,
                                      F.panel.bottom - 0.5f),
                          24 * s, 24 * s),
        g_brush.Get(), 1);

    auto title = FolderTitlePill();
    DrawEditView(GetEditView(true), g_folderRenaming, 20 * s);

    const Tiles& apps = g_openFolder->apps;
    g_target->PushAxisAlignedClip(
        D2D1::RectF(F.panel.left, F.top, F.panel.right,
                    F.top + F.visibleRows * F.cellHeight),
        D2D1_ANTIALIAS_MODE_ALIASED);
    int first = g_folderRow * F.columns;
    int last = std::min((int)apps.size(),
                        (g_folderRow + F.visibleRows) * F.columns);
    SetGlassScenePointer();
    if(g_closingFolder==g_openFolder||!g_glassScenePointer||
       !InRect({F.left,F.top,F.left+F.columns*L.cellWidth,F.top+F.visibleRows*F.cellHeight},
               g_glassScenePointer->x,g_glassScenePointer->y))g_glassScenePointer.reset();
    for (int i = first; i < last; i++) {
        DrawTile(apps[i], AnimatedCell(apps[i], FolderCellAt(i)), i, true);
    }
    g_target->PopAxisAlignedClip();
}

void DrawDragged() {
    if (!g_drag.active) {
        return;
    }
    float icon = g_layout.icon, size = std::round(icon * 1.06f);
    float x = g_drag.at.x - g_drag.grab.x - (size - icon) / 2;
    float y = g_drag.at.y - g_drag.grab.y - (size - icon) / 2;
    DrawTileIcon(g_drag.tile, D2D1::RectF(x, y, x + size, y + size), 0.92f);
}

bool g_scrollHover=false;
D2D1_RECT_F ScrollThumb() {
    float maxScroll=MaxScroll();
    if(g_settings.pages||maxScroll<=0||g_openFolder)return {};
    const auto& L=g_layout;
    float track=L.viewBottom-L.viewTop;
    if(track<=0)return {};
    float thumb=std::clamp(track*track/L.contentHeight,std::min(40*L.scale,track),track);
    float top=L.viewTop+(track-thumb)*(g_scroll/maxScroll);
    float right=L.width-10*L.scale;
    return {right-4*L.scale,top,right,top+thumb};
}
void DrawGrid() {
    const Layout& L = g_layout;
    float s = L.scale;
    if (!g_appsLoaded && g_apps.empty()) {
        DrawMessage(Tr(L"Loading apps\u2026"));
        return;
    }
    if (g_visible.empty() && !GroupedView()) {
        DrawMessage(!g_query.empty() ? Tr(L"No apps found")
                    : PinnedOnly()
                        ? Tr(L"Nothing pinned yet. Type to find an app, then right-click it to pin it")
                        : Tr(L"No apps"));
        return;
    }

    const bool flying = FlyInActive();
    // Start at the screen edges and restore the normal scroll viewport
    // as the travelling icons settle into their cells.
    float viewport = flying ? launchpad_fly_in::SmoothStep(g_appear / .72f) : 1.f;
    float clipTop = L.viewTop * viewport;
    float clipBottom = std::lerp((float)L.height, L.viewBottom, viewport);
    g_target->PushAxisAlignedClip(D2D1::RectF(0, clipTop, (float)L.width, clipBottom),
                                  D2D1_ANTIALIAS_MODE_ALIASED);
    if (flying) g_glassScenePointer.reset();
    else SetGlassScenePointer();
    if(g_glassScenePointer&&(g_glassScenePointer->y<L.viewTop||g_glassScenePointer->y>=L.viewBottom))
        g_glassScenePointer.reset();
    auto drawCell=[&](int i,Cell cell) {
        if(!g_groupEditMode)cell=AnimatedCell(g_visible[i],cell);
        if(cell.unit.bottom<=L.viewTop||cell.unit.top>=L.viewBottom||cell.unit.right<=0||cell.unit.left>=L.width)return;
        if (!flying) {
            DrawTile(g_visible[i],cell,i,!g_openFolder);
            return;
        }
        auto center = D2D1::Point2F((cell.icon.left + cell.icon.right) * .5f,
                                    (cell.icon.top + cell.icon.bottom) * .5f);
        auto pose = FlyInPose(center, 1.8f * std::max(cell.unit.right - cell.unit.left,
                                                    cell.unit.bottom - cell.unit.top));
        if (pose.opacity <= 0) return;
        auto project = [&](D2D1_RECT_F r) {
            return D2D1::RectF(
                center.x + pose.dx + (r.left - center.x) * pose.scale,
                center.y + pose.dy + (r.top - center.y) * pose.scale,
                center.x + pose.dx + (r.right - center.x) * pose.scale,
                center.y + pose.dy + (r.bottom - center.y) * pose.scale);
        };
        cell.unit = project(cell.unit);
        if (cell.unit.right <= 0 || cell.unit.left >= L.width ||
            cell.unit.bottom <= clipTop || cell.unit.top >= clipBottom) return;
        cell.icon = project(cell.icon);
        cell.highlight = project(cell.highlight);
        // Carry the label beneath its icon at the original font size. Keep
        // its layout width so fractional perspective does not reflow text.
        float labelDy = pose.dy + (cell.label.top - center.y) * (pose.scale - 1.f);
        DrawTile(g_visible[i],cell,i,!g_openFolder,pose.opacity,{pose.dx,labelDy});
    };
    if(GroupedView()) {
        DrawGroupGrid();
        auto resizePreview=GroupResizePreviewGeometry();
        const auto* preview=resizePreview?&*resizePreview:nullptr;
        auto drawGroups=[&](bool activeOnly) {
            DrawGroupHeaders(activeOnly,preview);
            for(const auto& current:g_groupGeometry) {
                bool selected=g_groupGesture.moved&&current.group==g_groupGesture.group;
                bool active=selected&&current.start==g_groupGesture.activeStart;
                if(active!=activeOnly||(selected&&!active))continue;
                const auto& geometry=active&&preview?*preview:current;
                auto area=VisualGroupRect(geometry);
                if(area.right<=0||area.left>=L.width||area.bottom<=L.viewTop||area.top>=L.viewBottom)continue;
                bool clip=active||(!flying&&g_groupTransitions.count(geometry.group));
                if(clip)g_target->PushAxisAlignedClip(area,D2D1_ANTIALIAS_MODE_ALIASED);
                for(int slot=0;slot<geometry.count;++slot) {
                    int i=geometry.start+slot;
                    if(i>=(int)g_visible.size())break;
                    auto cell=active&&preview?ProjectGroupCell(MakeExpandedGroupCell(geometry,slot)):CellAt(i);
                    cell=GroupVisualCell(geometry,cell);
                    if(active&&g_groupGesture.resize&&!GroupIconFits(cell,area))continue;
                    drawCell(i,cell);
                }
                if(clip)g_target->PopAxisAlignedClip();
            }
        };
        drawGroups(false);
        if(g_groupGesture.moved)drawGroups(true);
        DrawGroupEditPreview();
    } else {
        int first,last;
        if (g_settings.pages) {  // The one or two pages on screen.
            int page=std::max(0,(int)(g_scroll/L.width));
            first=page*L.perPage;last=std::min((int)g_visible.size(),(page+2)*L.perPage);
        } else {
            int firstRow=std::max(0,(int)(g_scroll/L.cellHeight));
            int lastRow=(int)((g_scroll+L.viewBottom-L.viewTop)/L.cellHeight)+1;
            first=firstRow*L.columns;last=std::min((int)g_visible.size(),(lastRow+1)*L.columns);
        }
        for(int i=first;i<last;++i)drawCell(i,CellAt(i));
    }
    g_target->PopAxisAlignedClip();

    float chrome = FlyInChromeOpacity();
    if (g_settings.pages) {
        // One dot per page; the current one is brighter.
        if (L.pages >= 1) {
            float step = std::min(20 * s, std::max(2.0f, (L.width - 48 * s) / L.pages));
            float r = std::min(3.5f * s, step * 0.28f);
            float left = L.width / 2.0f - L.pages * step / 2 + step / 2;
            float current = g_scroll / L.width;
            for (int i = 0; i < L.pages; i++) {
                float closeness = std::max(0.0f, 1 - std::abs(current - i));
                g_brush->SetColor(D2D1::ColorF(0xFFFFFF, chrome * (0.25f + 0.65f * closeness)));
                g_target->FillEllipse(
                    D2D1::Ellipse(D2D1::Point2F(left + i * step, L.dotsY), r, r),
                    g_brush.Get());
            }
        }
        return;
    }

    auto bar=ScrollThumb();
    if(SelectionHasArea(bar))Fill(bar,D2D1::ColorF(0xFFFFFF,chrome*(g_scrollHover?.62f:.32f)),2*s);
}

void Render() {
    double renderStarted = Now();
    if (!g_shown || !EnsureTarget()) {
        return;
    }
    const Layout& L = g_layout;
    if (!EnsureFrame(L.width, L.height)) {
        return;
    }
    g_target->SetTarget(g_frame.Get());
    float W = (float)L.width, H = (float)L.height;
    const bool flyIn = g_settings.iconFlyIn && !g_groupEditMode;
    float background = flyIn ? launchpad_fly_in::SmoothStep(g_appear / .58f) : 1.f;
    float chrome = FlyInChromeOpacity();
    float windowOpacity = flyIn ? 1.f : Ease(g_appear);
    g_target->BeginDraw();
    g_target->SetTransform(D2D1::Matrix3x2F::Identity());
    // Minimum alpha instead of zero: fully transparent pixels of a layered
    // window let the click pass through to what's behind.
    g_target->Clear(D2D1::ColorF(0, 0, 0, 1 / 255.0f));
    if (g_settings.blurMode == BlurMode::Custom) {
        if (ID2D1Bitmap* bitmap = GetBackdropBitmap()) {
            g_target->DrawBitmap(
                bitmap, D2D1::RectF(0, 0, W, H), background,
                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                D2D1::RectF(0, 0, g_backdropImage.sourceWidth,
                            g_backdropImage.sourceHeight));
        }
    }
    Fill(D2D1::RectF(0, 0, W, H), Alpha(g_settings.tint, g_settings.opacity * background));
    if(g_settings.backgroundDim>0)Fill(D2D1::RectF(0,0,W,H),D2D1::ColorF(0,0,0,g_settings.backgroundDim*background));

    float zoom = flyIn || g_groupEditMode ? 1.f : 1.06f - 0.06f * Ease(g_appear);
    g_target->SetTransform(
        D2D1::Matrix3x2F::Scale(zoom, zoom, D2D1::Point2F(W / 2, H / 2)));
    if (chrome > 0.f) {
        DrawSearchBox(chrome);
        DrawClearButton(chrome);
    }
    if (chrome > 0.f && (g_layoutReadOnly || g_fullBackupBusy)) {
        PCWSTR warning=Tr(L"Layout data is unreadable or unsupported: changes are read-only. Restore a verified backup.");
        if(g_fullBackupBusy)warning=Tr(L"Complete backup/import in progress…");
        g_brush->SetColor(D2D1::ColorF(0xFAB387, chrome));
        g_target->DrawText(warning,(UINT32)wcslen(warning),g_tipFormat.Get(),
            {20*L.scale,L.search.bottom,(float)L.width-20*L.scale,L.viewTop},g_brush.Get());
    }
    g_glassClaimed=false;
    DrawGrid();
    DrawToolBar(false, chrome);
    // A closing folder keeps drawing until its fade ends.
    std::shared_ptr<Folder> realOpen = g_openFolder;
    if (!g_openFolder && g_closingFolder) {
        g_openFolder = g_closingFolder;
    }
    bool folderLayer = false;
    if (g_openFolder) {
        float e = Ease(std::clamp(g_folderAnim, 0.0f, 1.0f));
        // Dim the grid, then lay the folder's glass over it: a blur of the
        // pixels just drawn, read back from the surface when they changed.
        Fill(D2D1::RectF(0, 0, W, H), D2D1::ColorF(0x171717, 0.35f * e * chrome));
        D2D1_MATRIX_3X2_F m;
        g_target->GetTransform(&m);
        D2D1_RECT_F panel = GetFolderLayout().panel;
        // The panel grows from 92% around its centre as it fades in.
        float k = 0.92f + 0.08f * e;
        m = D2D1::Matrix3x2F::Scale(
                k, k,
                D2D1::Point2F((panel.left + panel.right) / 2,
                              (panel.top + panel.bottom) / 2)) *
            D2D1::Matrix3x2F(m._11, m._12, m._21, m._22, m._31, m._32);
        D2D1_RECT_F dev = D2D1::RectF(
            panel.left * m._11 + m._31, panel.top * m._22 + m._32,
            panel.right * m._11 + m._31, panel.bottom * m._22 + m._32);
        const void* folder = g_openFolder.get();
        g_target->SetTransform(D2D1::Matrix3x2F::Identity());
        // Folder growth clips one full-size cached blur instead of generating
        // a new blur for each intermediate panel rectangle.
        D2D1_RECT_F captureRect = g_appear >= 1 ? panel : dev;
        if (g_drag.active || !PanelBlurFresh(captureRect, folder, g_scroll)) {
            if (g_target->EndDraw() == D2DERR_RECREATE_TARGET) {
                g_openFolder = realOpen;  // Do not leave a closing folder open.
                ResetTarget();
                g_labelCache.clear();
                g_groupTitleRenders.clear();
                return;
            }
            CapturePanelBlur(captureRect, folder, g_scroll, L.scale);
            g_target->SetTarget(g_frame.Get());
            g_target->BeginDraw();
        }
        if (e * chrome < 0.999f) {
            g_target->PushLayer(
                D2D1::LayerParameters1(D2D1::InfiniteRect(), nullptr,
                                       D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                       D2D1::IdentityMatrix(), e * chrome),
                nullptr);
            folderLayer = true;
        }
        DrawPanelBlur(dev, 24 * L.scale * m._11);
        g_target->SetTransform(m);
    }
    DrawFolder();
    if (folderLayer) {
        g_target->PopLayer();
    }
    g_openFolder = realOpen;
    DrawDragged();
    g_target->SetTransform(D2D1::Matrix3x2F::Scale(zoom,zoom,D2D1::Point2F(W/2,H/2)));
    DrawAppSelectionRect();
    DrawToolBar(true, chrome);
    g_target->SetTransform(D2D1::Matrix3x2F::Identity());
    DrawGroupEditBar();

    if (g_target->EndDraw() == D2DERR_RECREATE_TARGET) {
        ResetTarget();
        // Both caches hold bitmaps of the device that just went away.
        g_labelCache.clear();
        g_groupTitleRenders.clear();
        return;
    }
    if (UseGpuPresenter()) {
        if (g_presenter.Ensure(g_wnd, g_d3d.Get(), L.width, L.height)) {
            g_presenter.Position(g_wnd, L.area);
            if (g_presenter.Present(g_d3d.Get(), g_frame.Get(), windowOpacity)) {
                DXGI_FRAME_STATISTICS statistics{};
                if (g_presenter.Statistics(statistics)) g_metrics.Presented(statistics);
                g_metrics.Rendered((Now() - renderStarted) * 1000);
                return;
            }
        }
        g_presenter.Reset(); g_presenter.failed = true;
        Wh_Log(L"GPU composition unavailable; falling back to layered-window presentation");
    }
    if (g_presenter.Active()) g_presenter.Reset();
    LaunchpadPresenter::LegacyStyle(g_wnd);
    if (!EnsureSurface(g_surface, L.width, L.height)) return;
    PresentFrame();
    POINT dst{L.area.left, L.area.top}, src{0, 0};
    SIZE size{L.width, L.height};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, (BYTE)std::lround(255 * windowOpacity),
                        AC_SRC_ALPHA};
    UpdateLayeredWindow(g_wnd, nullptr, &dst, &size, g_surface.dc, &src, 0,
                        &blend, ULW_ALPHA);
    g_metrics.Rendered((Now() - renderStarted) * 1000);
}

void StartFrames() {
    if (!g_frameTimer) {
        g_frameTimer = true;
        g_lastFrame = Now();
        if (g_frameWake) ArmNextFrame(true);
        else SetTimer(g_wnd, kTimerFrame, (UINT)std::max(1.0, std::round(1000.0 / g_refreshHz)), nullptr);
    }
}

void HideNow() {
#ifndef SEAMLESS_PREVIEW
    if (g_shown) ResetTaskbarStartButtons();
    // The taskbar is about to be revealed: drop any checked latch the keyboard
    // path left on the Start button, or the user sees a clicked-state capsule
    // with no menu behind it.
    ClearStartButtonLatch(L"latch-hide");
#endif
    CancelGroupPointer();
    CancelGroupEdit();
    ClearAppSelection();g_batchBlend=0;g_batchDisplayCount=0;g_groupTransitions.clear();
    if(!g_freeGroupLayout)g_groupPositions.clear();
    g_groupDropFolder.reset(); g_groupDropSlot = -1;
    FinishMetrics();
    if (g_frameWake) CancelWaitableTimer(g_frameWake);
    KillTimer(g_wnd, kTimerFrame);
    g_frameTimer = false;
    EndDrag(false);
    g_searchDrag = false;
    KillTimer(g_wnd, kTimerCaret);
    DisarmPower();
    g_powerHover = g_powerPressed = -1;
    std::fill(std::begin(g_powerGlow), std::end(g_powerGlow), 0.0f);
    g_powerDockGlow = 0;
    if (g_openFolder) {
        SaveFolderName();
        g_openFolder = nullptr;
    }
    g_closingFolder = nullptr;
    g_folderAnim = 0;
    HWND wnd = g_wnd;
    ShowWindow(wnd, SW_HIDE);
    ShowWindow(g_backdrop, SW_HIDE);
    g_shown = false;
    g_closing = false;
    g_appear = 0;
    g_appearVelocity = 0;
    g_backdropImage = {};
    g_panelBlur = {};
    FreeSurface(g_surface);
    g_trackingMouse = false;
    g_scrollHover = false;
    g_clearHover = g_searchHover = false;
    g_searchGlow = 0;
}

float AdvanceTransition(float value, float target, double elapsed, int durationMs) {
    float step = (float)(std::max(0.0, elapsed) * 1000.0 / std::max(1, durationMs));
    return value < target ? std::min(target, value + step) : std::max(target, value - step);
}

void OnFrame() {
    double now = Now();
    g_metrics.Frame(now);
    float dt = (float)std::clamp(now - g_lastFrame, 0.0, 5.0);
    g_lastFrame = now;
    if (!Animate()) {
        dt = 1000.0f;  // Every configured transition lands this frame.
    }
    bool animating = false;

    // Use elapsed time, independent of the number of submitted frames.
    if (g_openFolder || g_closingFolder) {
        float folderTarget = g_openFolder ? 1.0f : 0.0f;
        if (g_folderAnim != folderTarget) {
            g_folderAnim = AdvanceTransition(g_folderAnim, folderTarget, dt, g_settings.folderMs);
            animating = true;
        }
        if (!g_openFolder && g_folderAnim <= 0) {
            g_closingFolder = nullptr;
            g_panelBlur = {};
        }
    }

    float target = g_closing ? 0.0f : 1.0f;
    if (g_appear != target) {
        int duration = g_closing ? g_settings.closeMs : g_settings.openMs;
        g_appear = g_settings.iconFlyIn
            ? launchpad_fly_in::Advance(g_appear, g_appearVelocity, target, dt, duration)
            : AdvanceTransition(g_appear, target, dt, duration);
        animating |= g_appear != target;
    }
    if (g_scroll != g_scrollTarget) {
        g_scroll += (g_scrollTarget - g_scroll) * (Animate() ? 1 - std::exp(-dt * 16) : 1);
        if (std::abs(g_scrollTarget - g_scroll) < 0.5f) {
            g_scroll = g_scrollTarget;
        }
        animating |= g_scroll != g_scrollTarget;
    }

    // Edit mode only once the drag is real: the press moved past the system
    // drag threshold or has been held for the hold time. A plain click never
    // wiggles.
    float wiggleTarget =
        g_drag.active && g_settings.wiggle && Animate() ? 1.0f : 0.0f;
    if (g_wiggle != wiggleTarget) {
        g_wiggle += (wiggleTarget - g_wiggle) * (Animate() ? 1 - std::exp(-dt * 12) : 1);
        if (std::abs(wiggleTarget - g_wiggle) < 0.01f) {
            g_wiggle = wiggleTarget;
        }
        animating = true;
    }
    if (g_wiggle > 0.001f) {
        animating = true;
    }

    // A press held still for the hold time lifts the icon into edit mode.
    if (g_drag.pending && !g_drag.active &&
        now - g_drag.pressTime >= g_settings.holdMs / 1000.0) {
        StartDrag();
        animating = true;
    }
    if (g_drag.pending && !g_drag.active) animating=true;

    // The search bar's accent glow eases in and out, about 140 ms.
    float glowTarget =
        g_searchHover || g_clearHover || SearchExpanded() ? 1.0f : 0.0f;
    if (g_searchGlow != glowTarget) {
        g_searchGlow +=
            (glowTarget - g_searchGlow) * (Animate() ? 1 - std::exp(-dt * 18) : 1);
        if (std::abs(glowTarget - g_searchGlow) < 0.01f) {
            g_searchGlow = glowTarget;
        }
        animating = true;
    }
    // The search field itself: 44 px pill at rest, 240 px field open, with the
    // box, its magnifier and its text sliding together about 200 ms.
    float expandTarget = SearchExpanded() ? 1.0f : 0.0f;
    if (g_searchExpand != expandTarget) {
        g_searchExpand +=
            (expandTarget - g_searchExpand) * (Animate() ? 1 - std::exp(-dt * 16) : 1);
        if (std::abs(expandTarget - g_searchExpand) < 0.01f) {
            g_searchExpand = expandTarget;
        }
        LayoutSearch();
        animating = true;
    }
    // The power dock glows fade over about 140 ms.
    for (int slot = 0; slot < (int)g_layout.powerItems.size(); slot++) {
        float& value = g_powerGlow[g_layout.powerItems[slot]];
        float target =
            slot == g_powerHover || slot == g_powerArmed ? 1.0f : 0.0f;
        if (value != target) {
            float step = dt / 0.14f;
            value = value < target ? std::min(target, value + step)
                                   : std::max(target, value - step);
            animating = true;
        }
    }
    float dockTarget = g_powerHover >= 0 ? 1.0f : 0.0f;
    if (g_powerDockGlow != dockTarget) {
        float step = dt / 0.14f;
        g_powerDockGlow =
            g_powerDockGlow < dockTarget
                ? std::min(dockTarget, g_powerDockGlow + step)
                : std::max(dockTarget, g_powerDockGlow - step);
        animating = true;
    }

    if (g_drag.active) {
        DragTick();
        animating = true;
    }
    if (g_groupHeaderDragging) { GroupHeaderTick(); animating=true; }
    if(g_groupGesture.moved)animating|=TickGroupGesture();
    for(auto it=g_groupTransitions.begin();it!=g_groupTransitions.end();) {
        bool done=GroupEase(it->second.started)>=1;
        animating|=!done;
        if(done&&!g_groupGesture.moved)it=g_groupTransitions.erase(it);
        else ++it;
    }
    if(g_groupResizeHover&&g_groupResizeGlow<1) {
        g_groupResizeGlow=std::min(1.f,g_groupResizeGlow+(float)(dt/.14));
        animating=true;
    }
    float batchTarget=g_appSelection.empty()?0.f:1.f;
    if(g_batchBlend!=batchTarget) {
        g_batchBlend=AdvanceTransition(g_batchBlend,batchTarget,dt,Animate()?140:0);
        animating=true;
    }

    if (g_closing && g_appear <= 0) {
        HideNow();
        animating = false;
    } else {
        g_glowAnimating = false;
        g_tileMotionAnimating = false;
        Render();
        animating |= g_glowAnimating || g_tileMotionAnimating;
    }
    if (!animating) {
        FinishMetrics();
        KillTimer(g_wnd, kTimerFrame);
        if (g_frameWake) CancelWaitableTimer(g_frameWake);
        g_frameTimer = false;
    } else if (g_frameWake) {
        ArmNextFrame(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Opening and closing

// SetForegroundWindow succeeds when the calling process received the last
// input event; an empty SendInput provides it.
void ForceForeground(HWND hwnd) {
    INPUT input{};
#ifndef SEAMLESS_PREVIEW
    SendInput(1, &input, sizeof(INPUT));
#endif
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
}

void PlaceWindows() {
    const RECT& a = g_layout.area;
    // Always on top: Windows sometimes leaves "always on top" windows (the
    // Dock included) below regular ones, and positioning below the Dock
    // put Start behind the windows.
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (!g_settings.coverScreen) {
        RaiseDockWindows();
    }
    if (UsesDwmBackdrop()) {
        SetWindowPos(g_backdrop, g_wnd, a.left, a.top, a.right - a.left,
                     a.bottom - a.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
}

void Open() {
    if (g_fullImportBusy) return;
    double requested = Now();
    if (g_shown) {
        if (g_closing) {  // Reopened in the middle of closing.
            g_closing = false;
            ForceForeground(g_wnd);
            PlaceWindows();
            StartFrames();
        }
        return;
    }

    HMONITOR monitor = ChosenMonitor();
    UpdateFrameRate(monitor);
    if (g_settings.performanceLogging) g_metrics.Begin("open", requested, 0, 0, 1, g_refreshHz);
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

    Layout& L = g_layout;
    L.area = AvailableArea(monitor);
    L.width = L.area.right - L.area.left;
    L.height = L.area.bottom - L.area.top;
    L.scale = dpiX / 96.0f;
    g_metrics.width = L.width; g_metrics.height = L.height; g_metrics.scale = L.scale;
    EnsureFormats(L.scale);

    g_query.clear();
    g_searchActivated = false;
    g_searchExpand = 0;
    g_caret = g_anchor = 0;
    g_hist[0] = {};  // A fresh search starts a fresh history.
    g_searchDrag = false;
    TouchCaret();
    g_glassPointer.reset();
    for(auto& fade:g_glowFades)fade={};
    g_hover = g_selected = g_pressed = -1;
    g_scroll = g_scrollTarget = g_groupPagePan = 0;
    g_wheelDelta = 0;
    g_openFolder = nullptr;
    g_tileMotion.clear();
    g_folderRow = 0;
    g_powerHover = g_powerPressed = g_powerArmed = -1;
    RefreshPowerCaps();
    UpdateFilter();
    ComputeLayout();

    // The background screenshot is taken before any of the mod's windows appear.
    // DWM modes use this pre-open blur only as a documented approximation.
    // It cannot include DWM material/noise or later changes behind the window.
    CaptureBackdrop(L.area,g_settings.blurMode==BlurMode::None?0:g_settings.blurAmount/100.0f*60*L.scale);
    if (UsesDwmBackdrop()) {
        ApplyBackdropEffect();
    }

    g_shown = true;
    g_closing = false;
    g_appear = Animate() ? 0 : 1;
    g_appearVelocity = 0;
    Render();
    ShowWindow(g_wnd, SW_SHOW);
    if (g_metrics.active) g_metrics.visibleMs = (Now() - requested) * 1000;
    UINT blink = GetCaretBlinkTime();
    SetTimer(g_wnd, kTimerCaret, blink ? blink : 530, nullptr);
    ForceForeground(g_wnd);
    PlaceWindows();
    if (Animate()) StartFrames(); else FinishMetrics();

    if (!g_appsLoaded || Now() - g_appsTime > 60 ||
        g_appsKey != AppsKey()) {
        StartLoadingApps();
    }
}

void Close() {
    if (!g_shown || g_closing || !ConfirmGroupEditClose()) {
        return;
    }
    FinishMetrics();
    if (g_settings.performanceLogging) g_metrics.Begin("close", Now(), g_layout.width, g_layout.height, g_layout.scale, g_refreshHz);
    g_closing = true;
    g_pressed = g_powerPressed = -1;
    DisarmPower();
    EndDrag(false);
    if (g_openFolder) {
        SaveFolderName();
    }
    if (GetCapture() == g_wnd) {
        ReleaseCapture();
    }
    if (Animate()) StartFrames(); else HideNow();
}

void Toggle() {
    if (g_shown && !g_closing) {
        Close();
    } else {
        Open();
    }
}

// A lone Windows key tap can reach the window twice: the keyboard hook reports
// the tap, and Windows' own request for Start follows it into the ShowStartView
// hook. Both describe one keystroke, so the first one to arrive acts as the
// toggle and the other is dropped.
bool FirstOfKeystroke() {
    double t = Now();
    if (t - g_keyToggleAt < 0.15) return false;
    g_keyToggleAt = t;
    return true;
}

// A packaged app with no Start-menu shortcut: runas on its raw AppsFolder
// target is a silent no-op, so a temporary .lnk aimed at the target carries
// the elevation instead — the shell brokers that into an elevated launch
// (verified: the spawned Terminal answers the elevation token query).
// The link stays in the mod's own storage, is unique per launch, and is
// removed again; nothing is left in the user's profile.
void LaunchTarget(const std::wstring& target, PCWSTR verb, const App* entry,
                  bool quiet);
void ElevateViaTempLnk(const std::wstring& target) {
    auto directory = ModTempPath();
    if (directory.empty() ||
        (FAILED(SHCreateDirectoryExW(nullptr, directory.c_str(), nullptr)) &&
         GetFileAttributesW(directory.c_str()) == INVALID_FILE_ATTRIBUTES)) {
        Wh_Log(L"Cannot create the elevation folder: %s", directory.c_str());
        return;
    }
    static LONG launches = 0;
    WCHAR name[80];
    swprintf_s(name, L"runas-%lu-%llu-%ld.lnk", GetCurrentProcessId(),
               (unsigned long long)GetTickCount64(),
               InterlockedIncrement(&launches));
    std::wstring lnk = directory + L"\\" + name;

    IShellLinkW* link = nullptr;
    bool saved = false;
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&link)))) {
        IPersistFile* file = nullptr;
        if (SUCCEEDED(link->SetPath(target.c_str())) &&
            SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&file)))) {
            saved = SUCCEEDED(file->Save(lnk.c_str(), TRUE));
            file->Release();
        }
        link->Release();
    }
    if (!saved) {
        Wh_Log(L"Could not write the elevation shortcut: %s", lnk.c_str());
        DeleteFileW(lnk.c_str());
        return;
    }
    // Quiet waits for the shell to consume the link, so deleting it below
    // cannot race the elevated launch.
    LaunchTarget(lnk, L"runas", nullptr, true);
    DeleteFileW(lnk.c_str());
}

bool RequestLaunch(const std::wstring& target,PCWSTR verb,const App* entry,bool quiet=false) {
    SHELLEXECUTEINFOW sei{sizeof(sei)};
    sei.fMask = quiet ? SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI : SEE_MASK_ASYNCOK;
    sei.lpVerb = verb;
    sei.lpFile = target.c_str();
    // Normal activations retain arguments and working directory from entry edits.
    if (entry) {sei.lpParameters=entry->arguments.empty()?nullptr:entry->arguments.c_str();sei.lpDirectory=entry->workingDirectory.empty()?nullptr:entry->workingDirectory.c_str();}
    sei.nShow = SW_SHOWNORMAL;
    AllowSetForegroundWindow(ASFW_ANY);
    return ShellExecuteExW(&sei);
}
void LaunchTarget(const std::wstring& target, PCWSTR verb = nullptr,
                  const App* entry = nullptr, bool quiet = false) {
    if (RequestLaunch(target,verb,entry,quiet)) Close();
    else if (GetLastError() != ERROR_CANCELLED) {
        // A cancelled UAC prompt is the user saying no, not a broken entry.
        bool previous=g_modal;
        g_modal=true;
        MessageBoxW(g_wnd,Tr(L"Could not open this entry. Use Edit entry to repair its target."),Tr(L"Launch failed"),MB_OK|MB_ICONWARNING);
        g_modal=previous;
        if(g_shown)ForceForeground(g_wnd);
    } else if (g_shown) {
        Close();
    }
}
template<typename Request>
std::vector<std::wstring> RequestSelectedApps(const AppSelection& selection,Request request) {
    std::vector<std::wstring> failures;
    for(const auto& app:selection)if(!request(*app))failures.push_back(app->name);
    return failures;
}
void LaunchSelectedApps() {
    if(g_batchLaunching||g_appSelection.empty())return;
    g_batchLaunching=true;
    bool wasModal=std::exchange(g_modal,true);
    auto selection=g_appSelection;
    g_batchHover=-1;
    auto failures=RequestSelectedApps(selection,[](const App& app){
        return RequestLaunch(app.target,nullptr,&app,true);
    });
    ClearAppSelection();
    if(!failures.empty()) {
        std::wstring message=CountLabel(L"Could not open {0} selected apps.",failures.size());
        for(size_t i=0;i<std::min<size_t>(failures.size(),8);i++)message+=L"\n"+failures[i];
        if(failures.size()>8)message+=L"\n…";
        MessageBoxW(g_wnd,message.c_str(),Tr(L"Launch failed"),MB_OK|MB_ICONWARNING);
    }
    g_modal=wasModal;g_batchLaunching=false;
    if(failures.size()<selection.size())Close();
    else Render();
}


// The settings window, from the "..." menu. Its overrides are saved in this
// mod's writable storage. A later Windhawk edit to the same key takes priority.

constexpr WCHAR kSettingsClass[] = L"WindhawkRYJSLaunchpadSettings";
constexpr UINT WM_APP_SETTINGS_RELOAD = WM_APP + 1, WM_APP_SETTINGS_ACTIVATE = WM_APP + 2;
HANDLE g_settingsThread = nullptr;
DWORD g_settingsThreadId = 0;
std::atomic<HWND> g_settingsWnd;
std::atomic<bool> g_settingsStop;
void Settings_Close();
static float SystemCornerRadius(bool) { return 8; }
static int GetState(PCWSTR key, int fallback) { return Wh_GetIntValue(key, fallback); }
static void SetState(PCWSTR key, int value) { Wh_SetIntValue(key, value); }
static std::wstring StateKey(PCWSTR window, PCWSTR field) { return std::wstring(window)+L"_"+field; }
static void ApplyDarkWindow(HWND hwnd) { BOOL dark=TRUE; DwmSetWindowAttribute(hwnd,20,&dark,sizeof(dark)); }
namespace mocha {
constexpr UINT32 base = 0x242424, mantle = 0x1D1D1D, crust = 0x171717,
                 surface0 = 0x343434, surface1 = 0x494949, surface2 = 0x606060,
                 overlay0 = 0x777777, overlay1 = 0x909090, overlay2 = 0xA0A0A0,
                 subtext0 = 0xB4B4B4, subtext1 = 0xCCCCCC, text = 0xFFFFFF,
                 lavender = 0xB4BEFE, blue = 0x89B4FA, sapphire = 0x74C7EC,
                 sky = 0x89DCEB, teal = 0x94E2D5, green = 0xA6E3A1,
                 yellow = 0xF9E2AF, peach = 0xFAB387, maroon = 0xEBA0AC,
                 red = 0xF38BA8, mauve = 0xCBA6F7, pink = 0xF5C2E7,
                 flamingo = 0xF2CDCD, rosewater = 0xF5E0DC;
}  // namespace mocha
inline D2D1_COLOR_F D2DColor(UINT32 rgb, float a = 1.0f) {
    return D2D1::ColorF(((rgb >> 16) & 0xFF) / 255.0f, ((rgb >> 8) & 0xFF) / 255.0f,
                        (rgb & 0xFF) / 255.0f, a);
}
inline D2D1_COLOR_F D2DColorRef(COLORREF c, float a = 1.0f) {
    return D2D1::ColorF(GetRValue(c) / 255.0f, GetGValue(c) / 255.0f, GetBValue(c) / 255.0f, a);
}
inline COLORREF RefFromRgb(UINT32 rgb) { return RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF); }

COLORREF ParseColorRef(const std::wstring& text, COLORREF fallback) {
    std::wstring hex = !text.empty() && text[0] == L'#' ? text.substr(1) : text;
    if (hex.size() != 6 || hex.find_first_not_of(L"0123456789abcdefABCDEF") !=
                               std::wstring::npos) {
        return fallback;
    }
    UINT rgb = wcstoul(hex.c_str(), nullptr, 16);
    return RGB(rgb >> 16, (rgb >> 8) & 0xFF, rgb & 0xFF);
}

// ---------------------------------------------------------------------------
// 50-settings.cpp : the mod's own settings window. A sidebar of pages, grouped
// cards, toggles, dropdowns, sliders, key boxes and colour chips, all drawn
// with Direct2D in the style of Seamless Dictate's settings. It shows the same
// setting keys as Windhawk's page, using a private atomic settings store and
// a reload message. It requires neither an elevated CLI nor admin rights.
//
// One window, one Direct2D target that fills the whole client rect (the window
// edge is rounded by DWM, never by us). Text typed into a number, text or colour
// box goes through one small native EDIT that sits on top of the box while it is
// edited. Key boxes read WM_KEYDOWN themselves.
// ---------------------------------------------------------------------------

template <class T>
class SetCom {
   public:
    T* p = nullptr;
    SetCom() = default;
    SetCom(const SetCom& o) : p(o.p) {
        if (p) p->AddRef();
    }
    SetCom& operator=(const SetCom& o) {
        if (o.p) o.p->AddRef();
        reset();
        p = o.p;
        return *this;
    }
    ~SetCom() { reset(); }
    void reset() {
        if (p) p->Release();
        p = nullptr;
    }
    T** put() {
        reset();
        return &p;
    }
    T* get() const { return p; }
    T* operator->() const { return p; }
    explicit operator bool() const { return p != nullptr; }
};

struct SetField {
    enum Kind { Choice, Number, Text, Hotkey, Check, Color };
    Kind kind;
    PCWSTR key;
    PCWSTR label;
    PCWSTR desc = nullptr;
    PCWSTR def = L"";                                // The schema default ("Reset to suggested").
    std::vector<std::pair<std::wstring, std::wstring>> options;  // Value, label. Owned: the monitor list is built per window.
    int min = 0, max = 0;
    int page = 0;        // Which sidebar page shows it.
    PCWSTR group = L"";  // Heading above it on that page.
    std::wstring value;  // As loaded, so a save writes only what changed.
    std::wstring cur;    // What the window shows now.
    float fitW = 0;      // Box width that fits the text (Set_FitBoxes); 0 until measured.
    float fitText = -1;  // Measured text width behind fitW.
    std::wstring fitFor; // The text that was measured.
};

static bool Set_HasSlider(const SetField& f) {
    return f.max - f.min <= 100 || wcscmp(f.key,L"openDuration")==0 ||
        wcscmp(f.key,L"closeDuration")==0 || wcscmp(f.key,L"folderDuration")==0;
}

// Names, descriptions and option labels match the ==WindhawkModSettings== block word for word.
// Generated by test-settings\gen_table.py from src\00-header.cpp.
static const std::vector<std::wstring>& Set_InstalledFontFamilies() {
    static const std::vector<std::wstring> families = [] {
        std::vector<std::wstring> result;
        ComPtr<IDWriteFactory> factory;
        ComPtr<IDWriteFontCollection> collection;
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)factory.GetAddressOf())) ||
            FAILED(factory->GetSystemFontCollection(&collection))) return result;
        for (UINT32 i = 0; i < collection->GetFontFamilyCount(); i++) {
            ComPtr<IDWriteFontFamily> family;
            ComPtr<IDWriteLocalizedStrings> names;
            if (FAILED(collection->GetFontFamily(i, &family)) || FAILED(family->GetFamilyNames(&names))) continue;
            UINT32 index = 0, length = 0; BOOL found = FALSE;
            names->FindLocaleName(L"en-us", &index, &found);
            if (!found) index = 0;
            if (FAILED(names->GetStringLength(index, &length))) continue;
            std::wstring text(length + 1, L'\0');
            if (SUCCEEDED(names->GetString(index, text.data(), length + 1))) {
                text.resize(length);
                result.push_back(std::move(text));
            }
        }
        std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) { return ToLower(a) < ToLower(b); });
        result.erase(std::unique(result.begin(), result.end()), result.end());
        return result;
    }();
    return families;
}

// The monitors connected right now, in the order ChosenMonitor() numbers them:
// the main one first, then the others left to right (then top to bottom). Each
// label carries the panel's friendly name and resolution, so two screens of
// the same model stay apart. Values are the "1".."N" the setting already
// stores, so a choice keeps working with the static Windhawk settings block.
// The strings land in `keep`, which only ever grows: the settings fields hold
// c_str()s of them for as long as a settings window lives.
static void Set_MonitorLabels(
    std::vector<std::pair<std::wstring, std::wstring>>& keep) {
    struct Entry {
        bool primary;
        RECT rc;
        WCHAR device[32];
    };
    std::vector<Entry> monitors;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM param) -> BOOL {
            MONITORINFOEXW mi{};
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfoW(monitor, &mi)) {
                Entry e{(mi.dwFlags & MONITORINFOF_PRIMARY) != 0,
                        mi.rcMonitor, {}};
                wcscpy_s(e.device, mi.szDevice);
                ((std::vector<Entry>*)param)->push_back(std::move(e));
            }
            return TRUE;
        },
        (LPARAM)&monitors);
    std::stable_sort(monitors.begin(), monitors.end(),
                     [](const Entry& a, const Entry& b) {
                         if (a.primary != b.primary) {
                             return a.primary;
                         }
                         if (a.rc.left != b.rc.left) {
                             return a.rc.left < b.rc.left;
                         }
                         return a.rc.top < b.rc.top;
                     });
    for (size_t i = 0; i < monitors.size(); i++) {
        auto& e = monitors[i];
        DISPLAY_DEVICEW dd{sizeof(dd)};
        // iDevNum 0 under the monitor's device name reads the panel itself,
        // not the adapter: DeviceString is what the display applet shows.
        PCWSTR friendly =
            EnumDisplayDevicesW(e.device, 0, &dd, 0) && dd.DeviceString[0]
                ? dd.DeviceString
                : L"";
        int w = (int)(e.rc.right - e.rc.left), h = (int)(e.rc.bottom - e.rc.top);
        WCHAR buf[288];
        if (friendly[0]) {
            swprintf_s(buf, L"%ls %d · %ls (%d×%d)", launchpad_i18n::Text(L"Monitor").c_str(),
                       (int)i + 1, friendly, w, h);
        } else {
            swprintf_s(buf, L"%ls %d (%d×%d)", launchpad_i18n::Text(L"Monitor").c_str(),
                       (int)i + 1, w, h);
        }
        keep.emplace_back(std::to_wstring((int)i + 1), buf);
    }
}
static std::vector<SetField> Set_MakeFields() {
    using K = SetField;
    std::vector<SetField> fields = {
// Custom settings begin
{K::Check, L"gpuPresenter", L"GPU composition", L"Use GPU composition for smoother animation. Turn off for the compatibility renderer.", L"1", {}, 0, 100, 1, L"Rendering"},
{K::Check, L"performanceLogging", L"Record performance", L"Record opening latency, frame intervals and rendering cost in the mod storage folder.", L"0", {}, 0, 100, 1, L"Rendering"},
{K::Number, L"frameRateLimit", L"Frame rate limit", L"0 follows the selected monitor. Other values cap animation updates without changing the monitor.", L"0", {}, 0, 1000, 1, L"Rendering"},
{K::Choice, L"launchpadAnimation", L"Launchpad animation style", L"Choose how the launchpad opens and closes.", L"iconFlyIn", {{L"iconFlyIn", L"iOS icon fly-in"}, {L"classic", L"Classic zoom and fade"}}, 0, 100, 1, L"Motion"},
{K::Number, L"openDuration", L"Open duration (ms)", L"Duration of the opening transition.", L"280", {}, 60, 1500, 1, L"Motion"},
{K::Number, L"closeDuration", L"Close duration (ms)", L"Duration of the closing transition.", L"180", {}, 60, 1500, 1, L"Motion"},
{K::Number, L"folderDuration", L"Folder duration (ms)", L"Duration of folder expansion and collapse.", L"180", {}, 60, 1500, 1, L"Motion"},
{K::Choice, L"animationEasing", L"Animation easing", L"Choose how movement slows down. These curves do not overshoot.", L"settle", {{L"settle", L"Gentle settle"}, {L"cubic", L"Cubic ease-out"}, {L"quint", L"Quintic ease-out"}}, 0, 100, 1, L"Motion"},
{K::Check, L"reducedMotion", L"Reduce motion", L"Use immediate transitions. Windows animation accessibility settings are also respected.", L"0", {}, 0, 100, 1, L"Motion"},
{K::Number, L"labelFontSize", L"Application name size", L"Text size in DIP. Display scaling is applied automatically.", L"13", {}, 9, 32, 1, L"Typography"},
{K::Number, L"horizontalSpacing", L"Horizontal icon spacing", L"Preferred horizontal space between icons, in DIP.", L"56", {}, 8, 240, 1, L"Grid"},
{K::Number, L"verticalSpacing", L"Vertical icon spacing", L"Additional space between icon rows, in DIP.", L"16", {}, 0, 180, 1, L"Grid"},
{K::Number, L"groupWidth", L"Group width", L"Preferred group width in DIP. 0 adapts to the screen and number of groups.", L"0", {}, 0, 1600, 1, L"Expanded groups"},
{K::Number, L"groupSpacing", L"Group spacing", L"Minimum horizontal and vertical space between groups, in DIP.", L"32", {}, 8, 160, 1, L"Expanded groups"},
{K::Choice, L"groupFlow", L"Group arrangement", L"Arrange groups in columns or use one full-width group per row.", L"columns", {{L"columns", L"Responsive columns"}, {L"rows", L"Full-width rows"}}, 0, 100, 1, L"Expanded groups"},
{K::Choice, L"groupNavigation", L"Group navigation", L"Large groups continue on the next page when paging is selected.", L"scroll", {{L"scroll", L"Vertical scrolling"}, {L"pages", L"Horizontal pages"}}, 0, 100, 1, L"Expanded groups"},
{K::Choice, L"themeMode", L"Theme mode", L"Choose a preset, follow Windows, or keep your own colors.", L"custom", {{L"custom", L"Custom"}, {L"light", L"Light"}, {L"dark", L"Dark"}, {L"system", L"Follow Windows"}}, 0, 100, 2, L"Theme"},
{K::Number, L"backgroundDim", L"Background dimming", L"Additional darkening of the blurred background, 0–80 percent.", L"0", {}, 0, 80, 2, L"Readability"},
// Custom settings end
{K::Choice, L"interfaceLanguage", L"Interface language", L"Follow Windows display language. Save to apply; app names stay unchanged.", L"auto", {{L"auto", L"Automatic (Windows)"}, {L"zh-CN", L"Simplified Chinese"}, {L"en", L"English"}}, 0, 100, 2, L"Language"},
{K::Choice, L"uiFont", L"Interface font", L"Choose a locally installed font. Applies to text only; symbol icons use their own font.", L"auto", {{L"auto", L"Automatic (language default)"}}, 0, 100, 2, L"Typography"},
{K::Choice, L"winKeyAction", L"Windows key", L"Also Ctrl+Esc.", L"grid", {{L"grid", L"Open this grid"}, {L"start", L"Open the original Start"}, {L"program", L"Run the program below"}}, 0, 100, 0, L"Open Launchpad"},
{K::Choice, L"shiftWinKeyAction", L"Shift + Windows key", L"", L"start", {{L"grid", L"Open this grid"}, {L"start", L"Open the original Start"}, {L"program", L"Run the program below"}}, 0, 100, 0, L"Open Launchpad"},
{K::Choice, L"startButtonAction", L"Start button", L"The taskbar Start button, the Standalone Dock's, and every other way Start opens that is not the keyboard.", L"grid", {{L"grid", L"Open this grid"}, {L"start", L"Open the original Start"}, {L"program", L"Run the program below"}}, 0, 100, 0, L"Open Launchpad"},
{K::Text, L"program", L"Program", L"What \"Run the program below\" starts. A command line such as \"C:\\Program Files\\App\\app.exe\" --flag, or a shortcut, file, folder or web address", L"", {}, 0, 100, 0, L"Open Launchpad"},
{K::Choice, L"monitor", L"Monitor", L"Where the grid opens. The list shows the monitors connected right now, main first, then the others left to right. If the picked monitor is not connected, it opens where the mouse is.", L"mouse", {{L"mouse", L"The monitor with the mouse"}}, 0, 100, 1, L"Grid"},
{K::Choice, L"appSource", L"Apps to show", L"With pinned apps, search still finds every app. Right-click an app to pin or unpin it.", L"allApps", {{L"allApps", L"Start menu apps (the All apps list)"}, {L"desktop", L"Desktop icons"}, {L"both", L"Start menu apps and desktop icons"}, {L"pinned", L"Only the apps you pin"}}, 0, 100, 1, L"Grid"},
{K::Choice, L"gridLayout", L"Layout", L"How the grid is arranged.", L"groups", {{L"scroll", L"One list that scrolls up and down"}, {L"pages", L"Pages that flip left and right, like Launchpad"}, {L"groups", L"Expanded groups"}}, 0, 100, 1, L"Grid"},
{K::Number, L"globalColumns", L"Columns per row", L"Global grid columns, 1–30. 0 keeps the saved density or estimates it for a new layout. Preview changes in the layout editor.", L"0", {}, 0, 30, 1, L"Grid"},
{K::Check, L"coverScreen", L"Cover the whole screen", L"On, the grid also covers the taskbar, side bars and the Standalone Dock. Off, they stay visible.", L"0", {}, 0, 100, 1, L"Grid"},
{K::Number, L"gridIconSize", L"Icon size", L"24 to 192 (Suggested: 56). Pixels at 100% scaling.", L"56", {}, 24, 192, 1, L"Grid"},
{K::Check, L"showLabels", L"Icon labels", L"On, the app name shows under each icon. Off, icons only.", L"1", {}, 0, 100, 1, L"Grid"},
{K::Check, L"normalizeIcons", L"Fill small icons", L"Scale small or padded icons to full size.", L"1", {}, 0, 100, 1, L"Grid"},
{K::Choice, L"blurMode", L"Background blur", L"The kind of blur behind the grid.", L"custom", {{L"custom", L"Adjustable (uses the strength below)"}, {L"acrylic", L"Windows acrylic (DWM)"}, {L"blur", L"Windows blur (DWM)"}, {L"none", L"No blur"}}, 0, 100, 2, L"Appearance"},
{K::Number, L"blurAmount", L"Blur strength", L"0 to 100 (Suggested: 60). Only for the adjustable blur. The Windows ones have a fixed strength.", L"60", {}, 0, 100, 2, L"Appearance"},
{K::Number, L"backgroundOpacity", L"Background opacity (%)", L"0 to 100 (Suggested: 30). 0 leaves only the blur, 100 covers everything with the color.", L"30", {}, 0, 100, 2, L"Appearance"},
{K::Color, L"tintColor", L"Background color", L"In hexadecimal, like #1E1E2E.", L"#000000", {}, 0, 100, 2, L"Appearance"},
{K::Number, L"glowStrength", L"Glow strength", L"0 to 100 (Suggested: 60). How strongly hovered buttons glow.", L"60", {}, 0, 100, 2, L"Appearance"},
{K::Number, L"chromeOpacity", L"Search bar and dock opacity (%)", L"0 to 100 (Suggested: 90). How solid the search bar and the power dock are.", L"90", {}, 0, 100, 2, L"Appearance"},
{K::Check, L"powerBar", L"Power dock", L"The pill of power buttons at the bottom of the grid.", L"1", {}, 0, 100, 3, L"Power buttons"},
{K::Choice, L"dockAlignment", L"Dock position", L"Where the power dock sits along the bottom edge.", L"center", {{L"left", L"Left"}, {L"center", L"Center"}, {L"right", L"Right"}}, 0, 100, 3, L"Power buttons"},
{K::Check, L"powerButtons.lock", L"Lock", L"", L"1", {}, 0, 100, 3, L"Power buttons"},
{K::Check, L"powerButtons.signOut", L"Sign out", L"", L"1", {}, 0, 100, 3, L"Power buttons"},
{K::Check, L"powerButtons.sleep", L"Sleep", L"", L"1", {}, 0, 100, 3, L"Power buttons"},
{K::Check, L"powerButtons.hibernate", L"Hibernate", L"Shows only when hibernation is turned on in Windows.", L"1", {}, 0, 100, 3, L"Power buttons"},
{K::Check, L"powerButtons.restart", L"Restart", L"", L"1", {}, 0, 100, 3, L"Power buttons"},
{K::Check, L"powerButtons.shutDown", L"Shut down", L"", L"1", {}, 0, 100, 3, L"Power buttons"},
{K::Check, L"powerNoConfirm", L"Skip confirmation", L"On, a power button acts on the first click. Off, the first click arms it and a second click on the same button confirms.", L"0", {}, 0, 100, 3, L"Power buttons"},
{K::Check, L"wiggle", L"Edit mode wiggle", L"While you drag an icon, all icons wiggle like on iOS.", L"1", {}, 0, 100, 1, L"Grid"},
{K::Number, L"holdTime", L"Drag hold time", L"150 to 1500 (Suggested: 420). Milliseconds to hold an icon before dragging starts.", L"420", {}, 150, 1500, 1, L"Grid"},
{K::Check, L"animations", L"Animations", L"Off, everything snaps instantly. Opening, folders, hover fades, tile moves and wiggle all turn off.", L"1", {}, 0, 100, 1, L"Grid"},
{K::Check, L"settingsGlass", L"Blurred settings window", L"On, the settings window is see-through glass with Windows acrylic behind it. Off, it is solid. Applies when you save.", L"1", {}, 0, 100, 2, L"Settings window"},
{K::Check, L"nativeSettingsTitleBar", L"Native Windows title bar", L"Use the Windows title bar and window buttons for this settings window. Applies when you save.", L"0", {}, 0, 100, 2, L"Settings window"},
{K::Check, L"settingsTrayIcon", L"Tray icon", L"A Launchpad icon in the system tray. Click it to open these settings, right-click it for a menu.", L"1", {}, 0, 100, 2, L"Settings window"}
    };
    for (auto& f : fields) if (wcscmp(f.key, L"uiFont") == 0)
        for (const auto& name : Set_InstalledFontFamilies()) f.options.emplace_back(name, name);
    // Built per call and moved into the field: a function-local static here
    // would dangle as soon as it reallocated, and two threads build fields.
    std::vector<std::pair<std::wstring, std::wstring>> monitorLabels;
    Set_MonitorLabels(monitorLabels);
    for (auto& f : fields) if (wcscmp(f.key, L"monitor") == 0)
        for (auto& label : monitorLabels)
            f.options.emplace_back(std::move(label.first), std::move(label.second));
    return fields;
}

struct SetPageInfo {
    PCWSTR name, intro;
    WCHAR glyph;
};
static const SetPageInfo kSetPages[] = {
{L"Keys", L"Choose how you open Launchpad and other programs.", 0xE765},
{L"Grid", L"Apps, icon sizes, pages and motion.", 0xE80A},
{L"Look", L"Blur, colors, glow, search bar and this window.", 0xE790},
{L"Power", L"Choose which power actions appear in the dock.", 0xE7E8},
{L"About", L"Version, credits, support and links.", 0xE946},
};
constexpr int kSetPageCount = (int)(sizeof(kSetPages) / sizeof(kSetPages[0]));
// About sits apart at the end of the sidebar, as in Seamless Dictate.
constexpr int kSetAboutPage = kSetPageCount - 1;
constexpr float kSetAboutGap = 17;

// All sizes are DIPs; the render target does the scaling.
constexpr float kSetPadX = 40, kSetMaxW = 720, kSetRowMin = 56, kSetRowGap = 8, kSetBoxH = 32, kSetRowPad = 16,
                kSetDropItem = 32, kSetRailTop = 76, kSetRailItem = 36, kSetRailPitch = 40;
constexpr int kSetDropMax = 9;  // A dropdown with more options than this scrolls.
// The sidebar is a floating dock: dragged between min and max, or collapsed to an icon rail.
constexpr float kSetDockGap = 10, kSetRailDef = 232, kSetRailMin = 200, kSetRailMax = 380, kSetRailMini = 60,
                kSetRailSnap = 130, kSetEdgeGrab = 5;
// The footer is a floating pill dock at the bottom right of the page.
constexpr float kSetFootBtnH = 36, kSetFootPad = 8, kSetFootH = kSetFootBtnH + 2 * kSetFootPad, kSetFootGap = 14;
// The glass tint is one layer over the whole window, title bar included, so the two always match.
constexpr float kSetGlassTint = 0.55f, kSetNativeFade = 56;
constexpr int kSetChipCount = 10;
constexpr UINT32 kSetChips[kSetChipCount] = {
    0x000000,0x171717,0x292929,0x424242,0x626262,0x808080,0xA0A0A0,0xC0C0C0,0xE0E0E0,0xFFFFFF
};
constexpr UINT_PTR kSetTimerId = 1;
constexpr PCWSTR kSetKofiUrl = L"https://ko-fi.com/DavidHiFi";
constexpr PCWSTR kSetAuthorUrl = L"https://github.com/DavidHiFi";
constexpr PCWSTR kSetDittoUrl = L"https://windhawk.net/mods/fullscreen-start";
constexpr PCWSTR kSetRepoUrl = L"https://github.com/DavidHiFi/davids-windhawk-mods";
constexpr PCWSTR kSetTitle = L"RYJS Launchpad";
// The self-drawn title bar. Everything below it is laid out as before, in "content" DIPs.
static float kSetCapH = 40;
constexpr float kSetCapBtnW = 46, kSetHoverMs = 120;

struct SetItem {
    enum Type { Group, Row, Banner, About, Link, Support } type = Row;
    int field = -1;  // Row: index into fields.
    float y = 0, h = 0;
    std::wstring text, sub, url;
};

// What the pointer is over. id picks the hover glow.
struct SetHit {
    enum Kind { None, Rail, Row, Control, Slider, Clear, Chip, Footer, Drop, Link, Scroll, Popup, Caption, RailToggle, RailEdge } kind = None;
    int a = -1, b = -1;
    int id = -1;
};

struct SetWindow {
    HWND hwnd = nullptr, edit = nullptr, owner = nullptr;
    HFONT editFont = nullptr;
    HBRUSH editBrush = nullptr;
    std::vector<SetField> fields = Set_MakeFields();
    std::vector<SetItem> items;
    UINT dpi = 96;
    float cw = 980, ch = 700;  // Content size in DIPs: the client area below the title bar.
    HICON iconSmall = nullptr, iconBig = nullptr;
    int capDown = 0;  // Title bar button held down: 1 minimize, 2 maximize, 3 close.
    bool capTracking = false, active = true;
    bool nativeTitle = false;  // Windows draws the title bar and its buttons.
    bool glass = false;        // Acrylic behind the window; the page is drawn see-through.
    ULONGLONG lastTick = 0;
    int page = 0, recording = -1, editing = -1, focus = -1, openSel = -1;
    bool kbd = false;  // Show the focus ring (after a Tab).
    D2D1_RECT_F dropRect{};
    int dropHover = -1, dropTop = 0;  // dropTop: first option shown when the dropdown scrolls.
    bool frameApplied = false;        // Set_ApplyFrame has run once.
    float scrollY = 0, scrollTarget = 0, contentH = 0, contentX = 0, contentW = 720, pageT = 1;
    SetHit hover;
    int dragSlider = -1;
    bool dragScroll = false, tracking = false, timer = false;
    float dragOff = 0;
    // Sidebar dock: railW is the chosen open width, railCur the drawn width (eased toward
    // kSetRailMini when collapsed). railGlow / footGlow ease the dock borders on hover.
    float railW = kSetRailDef, railCur = kSetRailDef, railGlow = 0, footGlow = 0;
    bool railCollapsed = false, dragRail = false;
    float mouseX = -1, mouseY = -1;
    std::map<int, float> glow;  // Eased hover amount by hit id.
    std::map<int, float> knob;  // Eased toggle position by field.
    std::wstring status;
    bool statusError = false;
    bool saving = false, destroyed = false, render = false;
    SetCom<ID2D1HwndRenderTarget> rt;
    SetCom<ID2D1BitmapRenderTarget> layer;  // The page, drawn off screen so the footer can blur it.
    D2D1_SIZE_F layerSize{};
    std::wstring family, iconFamily;
    SetCom<IDWriteTextFormat> fLabel, fHint, fTitle, fIntro, fSection, fValue, fValueC, fButton, fIcon, fIconS,
        fIconL, fBig, fSmall, fStatusLine, fCap, fCapGlyph, fRailName;
    RECT normalRect{};  // Last restored-state rect, remembered on close.
    COLORREF customColors[16] = {};
};

static std::shared_ptr<SetWindow> g_setWin;
static UINT g_setForceDpi;  // Harness renders at a chosen scale.
static SetCom<ID2D1Factory> g_setD2d;
static SetCom<IDWriteFactory> g_setDw;
static SetCom<IWICImagingFactory> g_setWic;
#ifdef SCM_HARNESS
static std::wstring g_setFakeKeyStatus;  // Lets the test render the hotkey warning line.
#endif

static std::wstring Set_WindowText(HWND hwnd) {
    std::wstring text(GetWindowTextLengthW(hwnd) + 1, L'\0');
    text.resize(GetWindowTextW(hwnd, text.data(), (int)text.size()));
    return text;
}

static std::wstring Set_KeyStatusText() {
#ifdef SCM_HARNESS
    if (!g_setFakeKeyStatus.empty()) return g_setFakeKeyStatus;
#endif
#ifdef SEAMLESS_PREVIEW
    return launchpad_i18n::Text(L"Independent preview. Keyboard hooks and system integration are inactive.");
#else
    return std::wstring();
#endif
}

// ---- Factories and fonts ---------------------------------------------------

static bool Set_EnsureFactories() {
    if (!g_setD2d) {
        D2D1_FACTORY_OPTIONS o{};
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &o,
                                     (void**)g_setD2d.put())))
            return false;
    }
    if (!g_setDw && FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                               (IUnknown**)g_setDw.put())))
        return false;
    if (!g_setWic && FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                             __uuidof(IWICImagingFactory), (void**)g_setWic.put())))
        return false;
    return true;
}

static void Set_ReleaseFactories() {
    g_setWic.reset();
    g_setDw.reset();
    g_setD2d.reset();
}

static bool Set_FamilyExists(const std::wstring& name) {
    SetCom<IDWriteFontCollection> sys;
    if (name.empty() || FAILED(g_setDw->GetSystemFontCollection(sys.put(), FALSE))) return false;
    UINT32 index = 0;
    BOOL exists = FALSE;
    return SUCCEEDED(sys->FindFamilyName(name.c_str(), &index, &exists)) && exists;
}

// The font setting, or its Nerd Font build ("Fira Code" is installed as
// "FiraCode Nerd Font"), or Segoe UI.
static std::wstring Set_ResolveFamily() {
    auto chosen = GetStringSetting(L"uiFont");
    if (!chosen.empty() && chosen != L"auto" && Set_FamilyExists(chosen)) return chosen;
    if (launchpad_i18n::chinese.load()) return L"Microsoft YaHei UI";
    if (Set_FamilyExists(std::wstring(L"FiraCode Nerd Font"))) return std::wstring(L"FiraCode Nerd Font");
    std::wstring joined;
    for (WCHAR c : std::wstring(L"FiraCode Nerd Font"))
        if (c != L' ') joined += c;
    for (const std::wstring& face : {joined + L" Nerd Font", std::wstring(L"FiraCode Nerd Font") + L" Nerd Font", joined})
        if (Set_FamilyExists(face)) return face;
    return L"Segoe UI";
}

static float Set_TextW(IDWriteTextFormat* fmt, const std::wstring& text);

static SetCom<IDWriteTextFormat> Set_Fmt(const std::wstring& family, float size, int weight, bool wrap,
                                         DWRITE_TEXT_ALIGNMENT align = DWRITE_TEXT_ALIGNMENT_LEADING,
                                         bool vcenter = false) {
    SetCom<IDWriteTextFormat> f;
    if (FAILED(g_setDw->CreateTextFormat(family.c_str(), nullptr, (DWRITE_FONT_WEIGHT)weight, DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL, size, launchpad_i18n::Locale(), f.put())))
        return f;
    f->SetWordWrapping(wrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);
    f->SetTextAlignment(align);
    f->SetParagraphAlignment(vcenter ? DWRITE_PARAGRAPH_ALIGNMENT_CENTER : DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    if (!wrap) {
        SetCom<IDWriteInlineObject> sign;
        g_setDw->CreateEllipsisTrimmingSign(f.get(), sign.put());
        DWRITE_TRIMMING t{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        f->SetTrimming(&t, sign.get());
    }
    return f;
}

// std::wstring(L"FiraCode Nerd Font") at 500; emphasis is one step heavier, never past bold.
static void Set_MakeFormats(SetWindow& w) {
    w.family = Set_ResolveFamily();
    w.iconFamily = Set_FamilyExists(L"Segoe Fluent Icons") ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
    int base = std::clamp(500, 100, 900);
    int bold = std::max(base, std::min(700, base + 100));
    const auto L_ = DWRITE_TEXT_ALIGNMENT_LEADING, C_ = DWRITE_TEXT_ALIGNMENT_CENTER;
    w.fLabel = Set_Fmt(w.family, 14, base, true);
    w.fHint = Set_Fmt(w.family, 12.5f, base, true);
    w.fTitle = Set_Fmt(w.family, 26, bold, false);
    w.fIntro = Set_Fmt(w.family, 13.5f, base, true);
    w.fSection = Set_Fmt(w.family, 10.5f, bold, false);
    w.fValue = Set_Fmt(w.family, 13, base, false, L_, true);
    w.fValueC = Set_Fmt(w.family, 13, base, false, C_, true);
    w.fButton = Set_Fmt(w.family, 13.5f, bold, false, C_, true);
    w.fBig = Set_Fmt(w.family, 20, bold, false);
    w.fSmall = Set_Fmt(w.family, 12, base, false, L_, true);
    w.fIcon = Set_Fmt(w.iconFamily, 16, 400, false, C_, true);
    w.fIconS = Set_Fmt(w.iconFamily, 12, 400, false, C_, true);
    w.fIconL = Set_Fmt(w.iconFamily, 34, 400, false, C_, true);
    w.fCap = Set_Fmt(w.family, 13, bold, false, L_, true);
    w.fCapGlyph = Set_Fmt(w.iconFamily, 10, 400, false, C_, true);
    w.fStatusLine = Set_Fmt(w.family, 12.5f, base, false, L_, true);
    // The sidebar name must never clip at the narrowest open width: 18 down to 15.5 (above 85%).
    const float railW = kSetRailMin - 90 - 10;
    for (float size = 18; size >= 15.5f; size -= 0.5f) {
        w.fRailName = Set_Fmt(w.family, size, bold, false, L_, true);
        if (w.fRailName && Set_TextW(w.fRailName.get(), L"Launchpad") <= railW) break;
    }
}

// ---- Values ----------------------------------------------------------------

static std::wstring Set_FieldValue(const SetField& f) {
    switch (f.kind) {
        case SetField::Number: {
            if (f.cur.empty()) return f.value;
            return std::to_wstring(std::clamp(_wtoi(f.cur.c_str()), f.min, f.max));
        }
        case SetField::Check:
            return f.cur == L"0" || f.cur.empty() ? L"0" : L"1";
        default:
            return f.cur;
    }
}

static void Set_PutValue(SetField& f, const std::wstring& value) {
    switch (f.kind) {
        case SetField::Choice: {
            int index = -1;
            for (size_t i = 0; i < f.options.size(); i++) {
                if (value == f.options[i].first) index = (int)i;
            }
            for (size_t i = 0; index < 0 && i < f.options.size(); i++) {
                if (wcscmp(f.def, f.options[i].first.c_str()) == 0) index = (int)i;
            }
            f.cur = f.options[std::max(index, 0)].first;
            break;
        }
        case SetField::Check:
            f.cur = value != L"0" && !value.empty() ? L"1" : L"0";
            break;
        default:
            f.cur = value;
    }
}

static void Set_LoadFields(SetWindow& w) {
    for (auto& f : w.fields) {
        bool isInt = f.kind == SetField::Number || f.kind == SetField::Check;
        Set_PutValue(f, isInt ? std::to_wstring(IntSetting(f.key, _wtoi(f.def))) : (GetStringSetting(f.key).empty() ? std::wstring(f.def) : GetStringSetting(f.key)));
        f.value = Set_FieldValue(f);  // What the window shows, so an untouched field never saves.
    }
}

static int Set_ChangedCount(const SetWindow& w) {
    int n = 0;
    for (auto& f : w.fields)
        if (Set_FieldValue(f) != f.value) n++;
    return n;
}

static bool Set_ParseHex(const std::wstring& text, UINT32& rgb) {
    if (text.size() != 7 || text[0] != L'#') return false;
    UINT32 v = 0;
    for (size_t i = 1; i < 7; i++) {
        WCHAR c = text[i];
        int d = c >= L'0' && c <= L'9' ? c - L'0' : c >= L'a' && c <= L'f' ? c - L'a' + 10 : c >= L'A' && c <= L'F' ? c - L'A' + 10 : -1;
        if (d < 0) return false;
        v = v * 16 + (UINT32)d;
    }
    rgb = v;
    return true;
}

static std::wstring Set_HexFromRgb(UINT32 rgb) {
    WCHAR text[16];
    swprintf_s(text, L"#%06X", rgb & 0xFFFFFF);
    return text;
}

static const wchar_t* Set_ChoiceLabel(const SetField& f) {
    for (auto& o : f.options)
        if (f.cur == o.first) return Tr(o.second.c_str());
    return f.options.empty() ? L"" : Tr(f.options[0].second.c_str());
}

static bool Set_Anim(const SetWindow& w) {
    return !w.render && Animate();
}

static void Set_Invalidate(SetWindow& w) {
    if (w.hwnd && !w.render) InvalidateRect(w.hwnd, nullptr, FALSE);
}

static void Set_SetStatus(SetWindow& w, PCWSTR text, bool error = false) {
    w.status = text;
    w.statusError = error;
    Set_Invalidate(w);
}

// ---- Text measuring ---------------------------------------------------------

static float Set_TextH(IDWriteTextFormat* fmt, const std::wstring& text, float width) {
    SetCom<IDWriteTextLayout> l;
    if (!fmt || FAILED(g_setDw->CreateTextLayout(text.c_str(), (UINT32)text.size(), fmt, width, 10000, l.put())))
        return 18;
    DWRITE_TEXT_METRICS m{};
    l->GetMetrics(&m);
    return m.height;
}

static float Set_TextW(IDWriteTextFormat* fmt, const std::wstring& text) {
    SetCom<IDWriteTextLayout> l;
    if (!fmt || FAILED(g_setDw->CreateTextLayout(text.c_str(), (UINT32)text.size(), fmt, 10000, 100, l.put())))
        return 0;
    DWRITE_TEXT_METRICS m{};
    l->GetMetrics(&m);
    return m.widthIncludingTrailingWhitespace;
}

// Boxes grow to fit their text, so no option, hotkey or value is ever cut off: a choice box fits
// its longest option, a hotkey or text box its value. A box never takes more than half the row;
// past that the dropdown list still shows every option in full. Text is measured only when it
// changes. Returns true when a width changed, so the page is laid out again.
static bool Set_FitBoxes(SetWindow& w) {
    if (!w.fValue) return false;
    bool changed = false;
    const float cap = std::floor(w.contentW * 0.5f);
    for (auto& f : w.fields) {
        if (wcscmp(f.key, L"uiFont") == 0) {
            float width = std::min(cap, 280.0f);
            if (f.fitW != width) { f.fitW = width; changed = true; }
            continue;
        }
        float base, pad;
        std::wstring text;
        if (f.kind == SetField::Choice) {
            base = 200, pad = 46;  // 12 before the text, 30 for the arrow, 4 spare.
            for (const auto& o : f.options) text += std::wstring(Tr(o.second.c_str())) + L'\n';
        } else if (f.kind == SetField::Hotkey) {
            base = 220, pad = 60;  // The key cap's own padding and the clear button.
            text = f.cur;
        } else if (f.kind == SetField::Text) {
            base = 280, pad = 26;
            text = f.cur;
        } else {
            continue;
        }
        if (f.fitText < 0 || text != f.fitFor) {
            float m = 0;
            if (f.kind == SetField::Choice) {
                for (const auto& o : f.options) m = std::max(m, Set_TextW(w.fValue.get(), Tr(o.second.c_str())));
            } else {
                m = Set_TextW(w.fValue.get(), f.cur);
            }
            f.fitText = std::ceil(m);
            f.fitFor = text;
        }
        float want = std::min(std::max(base, f.fitText + pad), std::max(base, cap));
        if (fabsf(want - f.fitW) > 0.5f) {
            f.fitW = want;
            changed = true;
        }
    }
    return changed;
}

// ---- Layout -----------------------------------------------------------------

// The right-hand control's width, so the label column knows how much room it has.
static float Set_FitOr(const SetField& f, float base) { return f.fitW > 0 ? f.fitW : base; }

static float Set_ControlW(const SetField& f) {
    switch (f.kind) {
        case SetField::Check: return 40;
        case SetField::Choice: return Set_FitOr(f, 200);
        case SetField::Number: return Set_HasSlider(f) ? 150 + 12 + 64 : 96;
        case SetField::Text: return Set_FitOr(f, 280);
        case SetField::Hotkey: return Set_FitOr(f, 220);
        case SetField::Color: return 362;
    }
    return 100;
}

// The page scrolls under the floating footer, so the view is the full height; contentH carries
// a bottom margin (Set_FootClear) so the last row can rise above the footer.
static float Set_ViewH(const SetWindow& w) { return w.ch; }
static float Set_FootClear() { return kSetFootH + kSetFootGap + 18; }

static D2D1_RECT_F Set_RailDock(const SetWindow& w) {
    return {kSetDockGap, kSetDockGap, kSetDockGap + w.railCur, std::max(kSetDockGap + 120, w.ch - kSetDockGap)};
}
// Right edge of the sidebar dock: the page starts after it.
static float Set_RailSpace(const SetWindow& w) { return kSetDockGap + w.railCur; }
// 0 collapsed to the icon rail, 1 fully open.
static float Set_RailOpen(const SetWindow& w) {
    return std::clamp((w.railCur - kSetRailMini) / std::max(1.0f, w.railW - kSetRailMini), 0.0f, 1.0f);
}
static D2D1_RECT_F Set_RailToggleRect(const SetWindow& w) {
    D2D1_RECT_F d = Set_RailDock(w);
    return {d.left + 12, d.top + 14, d.left + 48, d.top + 50};
}

// Everything a field's control is made of, in window DIPs for a given row.
struct SetParts {
    D2D1_RECT_F box{}, track{}, clear{}, chip[kSetChipCount]{};
    bool slider = false;
};

static SetParts Set_Parts(const SetField& f, const D2D1_RECT_F& row) {
    SetParts p;
    float r = row.right - kSetRowPad, mid = (row.top + row.bottom) / 2, h = kSetBoxH / 2;
    switch (f.kind) {
        case SetField::Check:
            p.box = {r - 40, mid - 11, r, mid + 11};
            break;
        case SetField::Choice:
            p.box = {r - Set_FitOr(f, 200), mid - h, r, mid + h};
            break;
        case SetField::Number:
            if (Set_HasSlider(f)) {
                p.slider = true;
                p.box = {r - 64, mid - h, r, mid + h};
                p.track = {p.box.left - 12 - 150, mid - 2, p.box.left - 12, mid + 2};
            } else {
                p.box = {r - 96, mid - h, r, mid + h};
            }
            break;
        case SetField::Text:
            p.box = {r - Set_FitOr(f, 280), mid - h, r, mid + h};
            break;
        case SetField::Hotkey:
            p.box = {r - Set_FitOr(f, 220), mid - h, r, mid + h};
            p.clear = {p.box.right - 30, p.box.top, p.box.right, p.box.bottom};
            break;
        case SetField::Color: {
            p.box = {r - 96, mid - h, r, mid + h};
            float x = p.box.left - 12 - (kSetChipCount * 26 - 6);
            for (int i = 0; i < kSetChipCount; i++) p.chip[i] = {x + i * 26, mid - 10, x + i * 26 + 20, mid + 10};
            break;
        }
    }
    return p;
}

static bool Set_In(const D2D1_RECT_F& r, float x, float y) { return x >= r.left && x < r.right && y >= r.top && y < r.bottom; }

static D2D1_RECT_F Set_RowRect(const SetWindow& w, const SetItem& it) {
    return {w.contentX, it.y, w.contentX + w.contentW, it.y + it.h};  // Content y (not scrolled).
}

// Lays out the current page's items for the current width.
static void Set_BuildPage(SetWindow& w) {
    w.items.clear();
    const float space = Set_RailSpace(w), avail = w.cw - space;
    w.contentW = std::clamp(avail - 2 * kSetPadX, 420.0f, kSetMaxW);
    w.contentX = space + std::max(kSetPadX, (avail - w.contentW) / 2);
    float introH = Set_TextH(w.fIntro.get(), Tr(kSetPages[w.page].intro), w.contentW);
    float y = 36 + 36 + 4 + introH + 24;
    if (w.page == 0) {
        std::wstring status = Set_KeyStatusText();
        if (!status.empty()) {
            SetItem it;
            it.type = SetItem::Banner;
            it.text = status;
            it.y = y;
            it.h = Set_TextH(w.fHint.get(), status, w.contentW - 32) + 28;
            y += it.h + kSetRowGap + 8;
            w.items.push_back(it);
        }
    }
    PCWSTR lastGroup = nullptr;
    for (size_t k = 0; k < w.fields.size(); k++) {
        const SetField& f = w.fields[k];
        if (f.page != w.page) continue;
        if (!lastGroup || wcscmp(lastGroup, f.group) != 0) {
            SetItem g;
            g.type = SetItem::Group;
            g.text = Tr(f.group);
            y += lastGroup ? 14 : 0;
            g.y = y;
            g.h = 22;
            y += g.h + 6;
            w.items.push_back(g);
            lastGroup = f.group;
        }
        SetItem it;
        it.type = SetItem::Row;
        it.field = (int)k;
        float textW = w.contentW - 2 * kSetRowPad - Set_ControlW(f) - 24;
        float lh = Set_TextH(w.fLabel.get(), Tr(f.label), textW);
        float dh = f.desc ? Set_TextH(w.fHint.get(), Tr(f.desc), textW) + 3 : 0;
        it.y = y;
        it.h = std::max(kSetRowMin, 28 + lh + dh);
        y += it.h + kSetRowGap;
        w.items.push_back(it);
    }
    if (w.page == kSetAboutPage) {
        auto add = [&](SetItem::Type t, float h, PCWSTR text, PCWSTR sub, PCWSTR url) {
            SetItem it;
            it.type = t;
            it.y = y;
            it.h = h;
            it.text = text;
            it.sub = sub;
            it.url = url;
            y += h + kSetRowGap;
            w.items.push_back(it);
        };
        auto group = [&](PCWSTR text) {
            SetItem g;
            g.type = SetItem::Group;
            g.text = text;
            g.y = y + 14;
            g.h = 22;
            y = g.y + g.h + 6;
            w.items.push_back(g);
        };
        add(SetItem::About, 146, L"", L"", L"");
        add(SetItem::Support, 84, Tr(L"Like it? Help the mod keep going."),
            Tr(L"Free and open source. A tip on Ko-fi keeps it moving."), kSetKofiUrl);
        group(Tr(L"Author"));
        add(SetItem::Link, 56, Tr(L"DavidHiFi on GitHub"), L"github.com/DavidHiFi", kSetAuthorUrl);
        add(SetItem::Link, 56, L"Ko-fi", L"ko-fi.com/DavidHiFi", kSetKofiUrl);
        group(Tr(L"Credits"));
        add(SetItem::Link, 56, Tr(L"Based on Fullscreen Start by Rafael Mendes"), Tr(L"The original Windhawk mod"), kSetDittoUrl);
        group(Tr(L"Project"));
        add(SetItem::Link, 56, Tr(L"Upstream source on GitHub"), L"github.com/DavidHiFi/davids-windhawk-mods", kSetRepoUrl);
    }
    w.contentH = y + Set_FootClear();
    w.scrollTarget = std::clamp(w.scrollTarget, 0.0f, std::max(0.0f, w.contentH - Set_ViewH(w)));
    w.scrollY = std::clamp(w.scrollY, 0.0f, std::max(0.0f, w.contentH - Set_ViewH(w)));
}

static const SetItem* Set_ItemOfField(const SetWindow& w, int field) {
    for (auto& it : w.items)
        if (it.type == SetItem::Row && it.field == field) return &it;
    return nullptr;
}

// The footer's status line: an explicit message, else the count of unsaved changes.
static std::wstring Set_FooterStatus(const SetWindow& w, UINT32* color) {
    std::wstring status = w.status;
    if (color) *color = w.statusError ? mocha::red : (status == Tr(L"Saved") ? mocha::green : mocha::subtext0);
    int changed = Set_ChangedCount(w);
    if (status.empty() && changed)
        status = changed == 1 ? Tr(L"1 change not saved") : std::to_wstring(changed) + Tr(L" changes not saved");
    return status;
}

constexpr float kSetFootW[3] = {172, 96, 100};  // Reset to suggested, Cancel, Save.

static float Set_FooterButtonsW() { return kSetFootW[0] + kSetFootW[1] + kSetFootW[2] + 2 * 8; }

// Width the status text gets inside the footer dock (0 when there is none).
static float Set_FooterStatusW(const SetWindow& w) {
    std::wstring s = Set_FooterStatus(w, nullptr);
    if (s.empty()) return 0;
    float room = w.contentW - Set_FooterButtonsW() - 2 * kSetFootPad - 30;
    return std::max(0.0f, std::min(Set_TextW(w.fStatusLine.get(), s) + 2, room));
}

// The pill that holds the status and the three buttons, sized to fit them, right-aligned to the page.
static D2D1_RECT_F Set_FooterDock(const SetWindow& w) {
    float sw = Set_FooterStatusW(w);
    float width = Set_FooterButtonsW() + 2 * kSetFootPad + (sw > 0 ? sw + 30 : 0);
    float right = w.contentX + w.contentW, bottom = w.ch - kSetFootGap;
    return {right - width, bottom - kSetFootH, right, bottom};
}

static D2D1_RECT_F Set_FooterButton(const SetWindow& w, int which) {  // 0 reset, 1 cancel, 2 save
    D2D1_RECT_F d = Set_FooterDock(w);
    float x = d.right - kSetFootPad, top = d.top + kSetFootPad;
    D2D1_RECT_F out{};
    for (int i = 2; i >= 0; i--) {
        out = {x - kSetFootW[i], top, x, top + kSetFootBtnH};
        if (i == which) return out;
        x -= kSetFootW[i] + 8;
    }
    return out;
}

static D2D1_RECT_F Set_RailRect(const SetWindow& w, int page) {
    D2D1_RECT_F d = Set_RailDock(w);
    float y = d.top + kSetRailTop + kSetRailPitch * page + (page == kSetAboutPage ? kSetAboutGap : 0);
    return {d.left + 8, y, d.right - 8, y + kSetRailItem};
}

static D2D1_RECT_F Set_SupportButton(const SetWindow& w, const SetItem& it) {
    D2D1_RECT_F r = Set_RowRect(w, it);
    return {r.right - kSetRowPad - 148, (r.top + r.bottom) / 2 - 18, r.right - kSetRowPad, (r.top + r.bottom) / 2 + 18};
}

static D2D1_RECT_F Set_ScrollThumb(const SetWindow& w) {
    float view = Set_ViewH(w), range = w.contentH - view;
    if (range <= 0) return {};
    float th = std::max(36.0f, view * view / w.contentH);
    float top = (w.scrollY / range) * (view - th - 8) + 4;
    return {w.cw - 18, top + 4, w.cw - 12, top + th - 4};
}

// ---- Painting ---------------------------------------------------------------

static D2D1_COLOR_F Set_A1(float a) { return D2DColorRef(RGB(224,224,224), a); }
static D2D1_COLOR_F Set_A2(float a) { return D2DColorRef(RGB(255,255,255), a); }

static float Set_Ease(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return 1 - (1 - t) * (1 - t) * (1 - t);
}

struct SetPaint {
    SetWindow& w;
    ID2D1RenderTarget* rt;
    SetCom<ID2D1SolidColorBrush> sb;
    float fade = 1;
    SetPaint(SetWindow& win, ID2D1RenderTarget* target) : w(win), rt(target) {
        rt->CreateSolidColorBrush(D2DColor(0), sb.put());
    }
    ID2D1SolidColorBrush* B(UINT32 rgb, float a = 1) {
        sb->SetColor(D2DColor(rgb, a * fade));
        return sb.get();
    }
    // Soft white highlight across the control.
    SetCom<ID2D1LinearGradientBrush> G(const D2D1_RECT_F& r, float a = 1) {
        SetCom<ID2D1LinearGradientBrush> out;
        D2D1_GRADIENT_STOP stops[2] = {{0, Set_A1(a * fade)}, {1, Set_A2(a * fade)}};
        SetCom<ID2D1GradientStopCollection> coll;
        if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 2, coll.put())))
            rt->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, (r.top + r.bottom) / 2),
                                                    D2D1::Point2F(std::max(r.right, r.left + 1), (r.top + r.bottom) / 2)),
                coll.get(), out.put());
        return out;
    }
    void Fill(const D2D1_RECT_F& r, UINT32 c, float a = 1) { rt->FillRectangle(r, B(c, a)); }
    void Round(const D2D1_RECT_F& r, float rad, UINT32 c, float a = 1) {
        rt->FillRoundedRectangle(D2D1::RoundedRect(r, rad, rad), B(c, a));
    }
    void Edge(const D2D1_RECT_F& r, float rad, UINT32 c, float a, float width = 1) {
        if (a <= 0.003f) return;
        float h = width / 2;
        rt->DrawRoundedRectangle(D2D1::RoundedRect({r.left + h, r.top + h, r.right - h, r.bottom - h}, rad - h, rad - h), B(c, a), width);
    }
    void GradFill(const D2D1_RECT_F& r, float rad, float a) {
        if (a <= 0.003f) return;
        if (auto g = G(r, a)) rt->FillRoundedRectangle(D2D1::RoundedRect(r, rad, rad), g.get());
    }
    void GradRing(const D2D1_RECT_F& r, float rad, float a, float width = 1.5f) {
        if (a <= 0.003f) return;
        D2D1_RECT_F in{r.left + width / 2, r.top + width / 2, r.right - width / 2, r.bottom - width / 2};
        if (auto g = G(r, a)) rt->DrawRoundedRectangle(D2D1::RoundedRect(in, rad, rad), g.get(), width);
    }
    void Text(IDWriteTextFormat* f, const std::wstring& s, const D2D1_RECT_F& r, UINT32 c, float a = 1) {
        if (!f || s.empty()) return;
        rt->DrawText(s.c_str(), (UINT32)s.size(), f, r, B(c, a), D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
    void Glyph(IDWriteTextFormat* f, WCHAR g, const D2D1_RECT_F& r, UINT32 c, float a = 1) {
        Text(f, std::wstring(1, g), r, c, a);
    }
};

static float Set_GlowOf(const SetWindow& w, int id) {
    auto it = w.glow.find(id);
    return it == w.glow.end() ? 0 : it->second;
}

// Ids that count as "pointer is here": the row (or rail item, button, link) and the control inside it.
static void Set_HoverIds(const SetHit& h, int ids[2]) {
    ids[0] = h.id;
    ids[1] = -1;
    if (h.kind == SetHit::Control || h.kind == SetHit::Slider || h.kind == SetHit::Clear || h.kind == SetHit::Chip)
        ids[1] = 6000 + h.a;
}

static bool Set_Valid(const SetWindow& w, int k) { return k >= 0 && k < (int)w.fields.size(); }

// Value boxes: a dark fill between mantle and surface0 (crust read as black), a resting
// overlay0 edge, and the white highlight on hover or while open.
constexpr UINT32 kSetBoxFill = 0x303030;
static void Set_DrawBox(SetPaint& p, SetWindow& w, int k, const D2D1_RECT_F& box, bool active) {
    p.Round(box, 8, kSetBoxFill);
    float g = std::max(Set_GlowOf(w, 6000 + k), active ? 1.0f : 0.0f);
    p.Edge(box, 8, mocha::overlay0, 0.7f * (1 - g), 1.25f);
    p.GradRing(box, 8, g, 1.75f);
}

static void Set_DrawRow(SetPaint& p, SetWindow& w, const SetItem& it) {
    const SetField& f = w.fields[it.field];
    int k = it.field;
    D2D1_RECT_F row = Set_RowRect(w, it);
    p.Round(row, 10, mocha::base);
    p.Round(row, 10, mocha::surface0, 0.32f);
    float hov = Set_GlowOf(w, 1000 + k);
    p.Edge(row, 10, mocha::surface2, 0.55f * (1 - hov));
    p.GradFill(row, 10, 0.13f * hov);
    p.GradRing(row, 10, hov, 1.5f);
    if (w.kbd && w.focus == k) p.GradRing(row, 10, 1.0f, 2.0f);

    float textW = w.contentW - 2 * kSetRowPad - Set_ControlW(f) - 24;
    float lh = Set_TextH(w.fLabel.get(), Tr(f.label), textW);
    float dh = f.desc ? Set_TextH(w.fHint.get(), Tr(f.desc), textW) + 3 : 0;
    float top = row.top + (it.h - lh - dh) / 2;
    p.Text(w.fLabel.get(), Tr(f.label), {row.left + kSetRowPad, top, row.left + kSetRowPad + textW, top + lh + 2}, mocha::text);
    if (f.desc)
        p.Text(w.fHint.get(), Tr(f.desc), {row.left + kSetRowPad, top + lh + 3, row.left + kSetRowPad + textW, top + lh + dh + 3},
               mocha::subtext0);

    SetParts pt = Set_Parts(f, row);
    const D2D1_RECT_F& b = pt.box;
    switch (f.kind) {
        case SetField::Check: {
            bool on = f.cur == L"1";
            float kn = w.knob.count(k) ? w.knob.at(k) : (on ? 1.0f : 0.0f);
            D2D1_RECT_F tr = pt.box;
            p.Round(tr, 11, mocha::surface1);
            p.GradFill(tr, 11, kn);
            float cx = tr.left + 11 + kn * 18, cy = (tr.top + tr.bottom) / 2;
            p.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 8, 8), p.B(on ? mocha::crust : mocha::text));
            break;
        }
        case SetField::Choice: {
            Set_DrawBox(p, w, k, b, w.openSel == k);
            p.Text(w.fValue.get(), Set_ChoiceLabel(f), {b.left + 12, b.top, b.right - 30, b.bottom}, mocha::text);
            p.Glyph(w.fIconS.get(), 0xE70D, {b.right - 30, b.top, b.right - 6, b.bottom}, mocha::subtext0);
            break;
        }
        case SetField::Number: {
            bool ed = w.editing == k;
            Set_DrawBox(p, w, k, b, ed);
            if (!ed) p.Text(w.fValueC.get(), Set_FieldValue(f), b, mocha::text);
            if (pt.slider) {
                float v = (float)(_wtoi(Set_FieldValue(f).c_str()) - f.min) / (float)std::max(1, f.max - f.min);
                D2D1_RECT_F tr = pt.track;
                p.Round(tr, 2, mocha::surface1);
                D2D1_RECT_F fillR{tr.left, tr.top, tr.left + (tr.right - tr.left) * v, tr.bottom};
                p.GradFill(fillR, 2, 1);
                float cx = tr.left + (tr.right - tr.left) * v, cy = (tr.top + tr.bottom) / 2;
                float kg = std::max(Set_GlowOf(w, 6000 + k), w.dragSlider == k ? 1.0f : 0.0f);
                p.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 8 + kg, 8 + kg), p.B(mocha::text));
                p.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 5, 5), p.B(mocha::crust));
            }
            break;
        }
        case SetField::Text: {
            bool ed = w.editing == k;
            Set_DrawBox(p, w, k, b, ed);
            if (!ed) p.Text(w.fValue.get(), f.cur, {b.left + 12, b.top, b.right - 10, b.bottom}, mocha::text);
            break;
        }
        case SetField::Hotkey: {
            bool rec = w.recording == k;
            Set_DrawBox(p, w, k, b, rec);
            if (rec) {
                p.Text(w.fValue.get(), Tr(L"Press keys..."), {b.left + 12, b.top, b.right - 10, b.bottom}, mocha::text);
            } else if (f.cur.empty()) {
                p.Text(w.fValue.get(), Tr(L"Not set"), {b.left + 12, b.top, b.right - 10, b.bottom}, mocha::overlay0);
            } else {
                float tw = std::min(Set_TextW(w.fValue.get(), f.cur) + 16, b.right - b.left - 40);
                D2D1_RECT_F cap{b.left + 6, (b.top + b.bottom) / 2 - 12, b.left + 6 + tw, (b.top + b.bottom) / 2 + 12};
                p.Round(cap, 6, mocha::surface0);
                p.Text(w.fValueC.get(), f.cur, cap, mocha::text);
                float cg = Set_GlowOf(w, 7000 + k);
                p.Glyph(w.fIconS.get(), 0xE711, pt.clear, cg > 0.5f ? mocha::red : mocha::overlay1);
            }
            break;
        }
        case SetField::Color: {
            bool ed = w.editing == k;
            Set_DrawBox(p, w, k, b, ed);
            UINT32 rgb = 0;
            bool okColor = Set_ParseHex(f.cur, rgb);
            if (okColor) {
                p.rt->FillEllipse(D2D1::Ellipse(D2D1::Point2F(b.left + 17, (b.top + b.bottom) / 2), 6, 6), p.B(rgb));
            }
            if (!ed) p.Text(w.fValue.get(), f.cur, {b.left + 30, b.top, b.right - 6, b.bottom}, okColor ? mocha::text : mocha::red);
            for (int i = 0; i < kSetChipCount; i++) {
                const D2D1_RECT_F& c = pt.chip[i];
                bool sel = okColor && (rgb & 0xFFFFFF) == kSetChips[i];
                p.Round(c, 6, kSetChips[i]);
                if (sel) {
                    D2D1_RECT_F ring{c.left - 3, c.top - 3, c.right + 3, c.bottom + 3};
                    p.GradRing(ring, 8, 1, 2);
                }
            }
            break;
        }
    }
}

static void Set_DrawGroup(SetPaint& p, SetWindow& w, const SetItem& it) {
    std::wstring up = it.text;
    CharUpperBuffW(up.data(), (DWORD)up.size());
    p.Text(w.fSection.get(), up, {w.contentX + 4, it.y, w.contentX + w.contentW, it.y + it.h}, mocha::overlay2);
}

// The resting card every row uses: base, a light lift and a soft gray edge that hands over to
// the hover ring.
static void Set_DrawCard(SetPaint& p, const D2D1_RECT_F& row, float hov) {
    p.Round(row, 10, mocha::base);
    p.Round(row, 10, mocha::surface0, 0.32f);
    p.Edge(row, 10, mocha::surface2, 0.55f * (1 - hov));
}

static void Set_DrawLinkRow(SetPaint& p, SetWindow& w, const SetItem& it, int idx) {
    D2D1_RECT_F row = Set_RowRect(w, it);
    float hov = Set_GlowOf(w, 3000 + idx);
    Set_DrawCard(p, row, hov);
    p.GradFill(row, 10, 0.13f * hov);
    p.GradRing(row, 10, hov, 1.5f);
    p.Text(w.fLabel.get(), it.text, {row.left + kSetRowPad, row.top + 9, row.right - 50, row.top + 30}, mocha::text);
    p.Text(w.fHint.get(), it.sub, {row.left + kSetRowPad, row.top + 30, row.right - 50, row.bottom - 4}, mocha::subtext0);
    p.Glyph(w.fIcon.get(), 0xE8A7, {row.right - 46, row.top, row.right - 12, row.bottom}, mocha::subtext1);
}

static void Set_DrawSupport(SetPaint& p, SetWindow& w, const SetItem& it, int idx) {
    D2D1_RECT_F row = Set_RowRect(w, it);
    float card = std::max(Set_GlowOf(w, 3700 + idx), Set_GlowOf(w, 3500 + idx));
    Set_DrawCard(p, row, card);
    p.GradFill(row, 10, 0.13f * card);
    p.GradRing(row, 10, card, 1.5f);
    p.Text(w.fLabel.get(), it.text, {row.left + kSetRowPad, row.top + 14, row.right - 180, row.top + 36}, mocha::text);
    p.Text(w.fHint.get(), it.sub, {row.left + kSetRowPad, row.top + 38, row.right - 180, row.bottom - 6}, mocha::subtext0);
    D2D1_RECT_F b = Set_SupportButton(w, it);
    float hov = Set_GlowOf(w, 3500 + idx);
    p.GradFill(b, 8, 0.85f + 0.15f * hov);
    p.GradRing(b, 8, hov, 2);
    p.Text(w.fButton.get(), Tr(L"Open Ko-fi"), b, mocha::crust);
}

static void Set_DrawAbout(SetPaint& p, SetWindow& w, const SetItem& it, int idx) {
    D2D1_RECT_F row = Set_RowRect(w, it);
    float hov = Set_GlowOf(w, 3800 + idx);
    Set_DrawCard(p, row, hov);
    p.GradFill(row, 10, 0.13f * hov);
    p.GradRing(row, 10, hov, 1.5f);
    D2D1_RECT_F mark{row.left + 20, row.top + 20, row.left + 92, row.top + 92};
    p.GradFill(mark, 18, 1);
    p.Glyph(w.fIconL.get(), 0xE80A, mark, mocha::crust);
    float x = mark.right + 20;
    p.Text(w.fBig.get(), L"RYJS Launchpad", {x, row.top + 18, row.right - 16, row.top + 46}, mocha::text);
#ifdef WH_MOD_VERSION
    std::wstring ver = std::wstring(Tr(L"Version ")) + WH_MOD_VERSION + Tr(L"  \u00B7  MIT license  \u00B7  A Windhawk mod");
#else
    std::wstring ver = L"MIT license  \u00B7  A Windhawk mod";
#endif
    p.Text(w.fHint.get(), ver, {x, row.top + 48, row.right - 16, row.top + 68}, mocha::subtext0);
    p.Text(w.fHint.get(), Tr(L"Apps, folders and search in one fullscreen grid, in place of Start."),
           {x, row.top + 72, row.right - 16, row.top + 110}, mocha::subtext1);
    p.Text(w.fHint.get(), Tr(L"Maintained by ryjs. Based on DavidHiFi's Seamless Launchpad and Rafael Mendes's Fullscreen Start."),
           {x, row.top + 112, row.right - 16, row.bottom - 8}, mocha::overlay2);
}

static void Set_DrawBanner(SetPaint& p, SetWindow& w, const SetItem& it) {
    D2D1_RECT_F r = Set_RowRect(w, it);
    p.Round(r, 10, mocha::peach, 0.14f);
    p.Text(w.fHint.get(), it.text, {r.left + 16, r.top + 14, r.right - 16, r.bottom - 8}, mocha::peach);
}

// A floating dock: soft shadow, mantle fill, a quiet 1px edge at rest and the white highlight
// edge as the pointer comes over it.
static void Set_DrawDock(SetPaint& p, SetWindow& w, const D2D1_RECT_F& d, float rad, float fill, float glow) {
    p.Round({d.left, d.top + 3, d.right, d.bottom + 5}, rad + 2, mocha::crust, w.glass ? 0.28f : 0.5f);
    p.Round(d, rad, mocha::mantle, fill);
    D2D1_RECT_F in{d.left + 0.5f, d.top + 0.5f, d.right - 0.5f, d.bottom - 0.5f};
    p.rt->DrawRoundedRectangle(D2D1::RoundedRect(in, rad, rad), p.B(mocha::surface1, 0.75f * (1 - glow)), 1);
    p.GradRing(d, rad, glow, 1.5f);
}

// Panel-left mark, as in T3's sidebar button: a window with its left pane shaded while open.
static void Set_DrawPanelIcon(SetPaint& p, const D2D1_RECT_F& r, float open, UINT32 c) {
    float cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    D2D1_RECT_F box{cx - 9, cy - 7.5f, cx + 9, cy + 7.5f};
    p.Round({box.left + 1, box.top + 1, box.left + 6.5f, box.bottom - 1}, 1.5f, c, 0.25f + 0.35f * open);
    p.rt->DrawRoundedRectangle(D2D1::RoundedRect(box, 3, 3), p.B(c), 1.5f);
    p.rt->DrawLine({box.left + 6.5f, box.top + 0.75f}, {box.left + 6.5f, box.bottom - 0.75f}, p.B(c), 1.5f);
}

static void Set_DrawMark(SetPaint& p, IDWriteTextFormat* glyph, const D2D1_RECT_F& r, float a);
static void Set_DrawRail(SetPaint& p, SetWindow& w) {
    D2D1_RECT_F d = Set_RailDock(w);
    const float open = Set_RailOpen(w), textA = std::clamp((open - 0.45f) / 0.55f, 0.0f, 1.0f);
    // Over the resize edge or while dragging, the dock's own rounded border brightens and
    // thickens, so the handle follows the corners instead of floating as a separate line.
    float edge = std::max(Set_GlowOf(w, 41), w.dragRail ? 1.0f : 0.0f);
    Set_DrawDock(p, w, d, 14, w.glass ? 0.58f : 1.0f, std::max(w.railGlow * 0.85f, edge));
    if (edge > 0.003f) p.GradRing(d, 13, edge, 2.25f);

    D2D1_RECT_F tb = Set_RailToggleRect(w);
    float th = Set_GlowOf(w, 42);
    p.Round(tb, 9, mocha::surface0, 0.45f + 0.25f * th);
    p.GradFill(tb, 9, 0.16f * th);
    p.GradRing(tb, 9, 0.9f * th, 1.25f);
    if (w.kbd && w.focus == -20) p.GradRing(tb, 9, 1, 2);
    Set_DrawPanelIcon(p, tb, open, th > 0.5f ? mocha::text : mocha::subtext1);

    p.rt->PushAxisAlignedClip(D2D1::RectF(d.left, d.top, d.right - 6, d.bottom), D2D1_ANTIALIAS_MODE_ALIASED);
    if (textA > 0.003f) {
        float my = (tb.top + tb.bottom) / 2;
        Set_DrawMark(p, w.fIcon.get(), {d.left + 56, my - 13, d.left + 82, my + 13}, textA);
    }
    p.Text(w.fRailName.get(), L"RYJS", {d.left + 90, d.top + 9, d.left + w.railW - 10, d.top + 33}, mocha::text, textA);
    p.Text(w.fRailName.get(), L"Launchpad", {d.left + 90, d.top + 32, d.left + w.railW - 10, d.top + 56}, mocha::text, textA);
    for (int i = 0; i < kSetPageCount; i++) {
        D2D1_RECT_F r = Set_RailRect(w, i);
        if (i == kSetAboutPage) {
            float ly = std::round(r.top - (kSetAboutGap + kSetRailPitch - kSetRailItem) / 2);
            p.Fill({r.left + 6, ly, r.right - 6, ly + 1}, mocha::surface1, 0.7f);
        }
        float hov = Set_GlowOf(w, 100 + i);
        bool act = i == w.page;
        if (act) {
            p.GradFill(r, 8, 0.22f);
            p.GradRing(r, 8, 0.5f, 1);
            D2D1_RECT_F bar{r.left, r.top + 8, r.left + 3, r.bottom - 8};
            p.GradFill({bar.left, bar.top, bar.left + 3, bar.bottom}, 1.5f, 1);
        } else {
            p.Round(r, 8, mocha::surface0, 0.28f);
            p.Edge(r, 8, mocha::surface2, 0.5f * (1 - hov));
            p.GradFill(r, 8, 0.10f * hov);
            p.GradRing(r, 8, 0.8f * hov, 1.25f);
        }
        if (w.kbd && w.focus == -10 - i) p.GradRing(r, 8, 1, 2);
        UINT32 c = act ? mocha::text : mocha::subtext1;
        // The glyph sits in a fixed 44-wide slot, so it stays centered in the collapsed rail.
        p.Glyph(w.fIcon.get(), kSetPages[i].glyph, {r.left, r.top, r.left + 44, r.bottom}, act ? mocha::text : mocha::subtext0);
        p.Text(w.fLabel.get(), Tr(kSetPages[i].name), {r.left + 50, r.top + 8, r.left + w.railW - 24, r.bottom}, c, textA);
    }
#ifdef WH_MOD_VERSION
    p.Text(w.fSmall.get(), std::wstring(Tr(L"Version ")) + WH_MOD_VERSION,
           {d.left + 18, d.bottom - 34, d.left + w.railW - 12, d.bottom - 12}, mocha::overlay1, textA);
#endif
    p.rt->PopAxisAlignedClip();
}

static void Set_DrawFooter(SetPaint& p, SetWindow& w) {
    D2D1_RECT_F d = Set_FooterDock(w);
    const float rad = kSetFootH / 2, brad = kSetFootBtnH / 2;
    // The same see-through mantle as the rail dock, so the blurred rows under the pill read as
    // real glass instead of a near-solid bar.
    p.Round(d, rad, mocha::mantle, w.render ? 1.0f : (w.glass ? 0.58f : 0.85f));
    const D2D1_RECT_F edge{d.left + 2, d.top + 2, d.right - 2, d.bottom - 2};
    p.rt->DrawRoundedRectangle(D2D1::RoundedRect(edge, rad - 2, rad - 2),
                              p.B(mocha::overlay0, 0.9f * (1 - w.footGlow)), 2.75f);
    p.GradRing(d, rad - 1.5f, w.footGlow, 3.0f);
    UINT32 sc;
    std::wstring status = Set_FooterStatus(w, &sc);
    float sw = Set_FooterStatusW(w);
    if (sw > 0) p.Text(w.fStatusLine.get(), status, {d.left + 18, d.top, d.left + 18 + sw, d.bottom}, sc);
    const PCWSTR names[3] = {Tr(L"Reset to suggested"), Tr(L"Cancel"), Tr(L"Save")};
    for (int i = 0; i < 3; i++) {
        D2D1_RECT_F b = Set_FooterButton(w, i);
        float hov = Set_GlowOf(w, 10 + i);
        p.Round(b, brad, mocha::base, 0.98f);
        p.Edge(b, brad, mocha::overlay0, 0.78f * (1 - hov), 1.75f);
        p.GradRing(b, brad - 1, hov, 2.25f);
        p.Text(w.fButton.get(), names[i], b, mocha::text, w.saving ? 0.5f : 1.0f);
        if (w.kbd && w.focus == -1 - i) p.GradRing(b, brad, 1, 2);
    }
}


// Neutral scrollbar thumb, brighter while
// hovered or dragged. Never a flat grey thumb.
static void Set_ThumbFill(SetPaint& p, const D2D1_RECT_F& r, float a) {
    if (r.bottom <= r.top) return;
    float rad = (r.right - r.left) / 2;
    SetCom<ID2D1LinearGradientBrush> g;
    D2D1_GRADIENT_STOP stops[2] = {{0, D2DColor(mocha::text, a * p.fade)}, {1, D2DColor(mocha::text, a * p.fade)}};
    SetCom<ID2D1GradientStopCollection> coll;
    if (SUCCEEDED(p.rt->CreateGradientStopCollection(stops, 2, coll.put())) &&
        SUCCEEDED(p.rt->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.left, r.bottom)),
            coll.get(), g.put())))
        p.rt->FillRoundedRectangle(D2D1::RoundedRect(r, rad, rad), g.get());
}

static int Set_DropVisible(const SetWindow& w) {
    return w.openSel < 0 ? 0 : std::min((int)w.fields[w.openSel].options.size(), kSetDropMax);
}

// Keeps option i inside the visible part of a scrolling dropdown.
static void Set_DropShow(SetWindow& w, int i) {
    int vis = Set_DropVisible(w), n = w.openSel < 0 ? 0 : (int)w.fields[w.openSel].options.size();
    if (i < 0 || vis <= 0) return;
    if (i < w.dropTop) w.dropTop = i;
    if (i >= w.dropTop + vis) w.dropTop = i - vis + 1;
    w.dropTop = std::clamp(w.dropTop, 0, std::max(0, n - vis));
}

static void Set_DrawPopup(SetPaint& p, SetWindow& w) {
    if (w.openSel < 0) return;
    const SetField& f = w.fields[w.openSel];
    const D2D1_RECT_F& r = w.dropRect;
    const int n = (int)f.options.size(), vis = Set_DropVisible(w);
    const bool scrolls = n > vis;
    D2D1_RECT_F shadow{r.left - 1, r.top + 3, r.right + 1, r.bottom + 5};
    p.Round(shadow, 12, mocha::crust, 0.6f);
    p.Round(r, 10, mocha::mantle);
    p.GradRing(r, 10, 0.9f);
    const float right = r.right - (scrolls ? 12 : 4);
    for (int v = 0; v < vis; v++) {
        int i = w.dropTop + v;
        if (i >= n) break;
        D2D1_RECT_F it{r.left + 4, r.top + 4 + v * kSetDropItem, right, r.top + 4 + (v + 1) * kSetDropItem};
        bool sel = f.cur == f.options[i].first;
        if (i == w.dropHover) {
            p.GradFill(it, 6, 0.28f);
            p.GradRing(it, 6, 0.6f, 1);
        }
        p.Text(w.fValue.get(), Tr(f.options[i].second.c_str()), {it.left + 10, it.top, it.right - 28, it.bottom}, sel ? mocha::text : mocha::text);
        if (sel) p.Glyph(w.fIconS.get(), 0xE73E, {it.right - 28, it.top, it.right - 6, it.bottom}, mocha::text);
    }
    if (scrolls) {
        float track = r.bottom - r.top - 12, th = std::max(24.0f, track * vis / n);
        float top = r.top + 6 + (track - th) * w.dropTop / std::max(1, n - vis);
        Set_ThumbFill(p, {r.right - 9, top, r.right - 5, top + th}, 0.85f);
    }
}

// Title bar buttons, in title bar DIPs: 0 minimize, 1 maximize or restore, 2 close.
static D2D1_RECT_F Set_CapButton(const SetWindow& w, int i) {
    float right = w.cw - (2 - i) * kSetCapBtnW;
    return {right - kSetCapBtnW, 0, right, kSetCapH};
}

// y is in content DIPs, so the title bar is y < 0.
static int Set_CapButtonAt(const SetWindow& w, float x, float y) {
    if (w.nativeTitle) return -1;
    for (int i = 0; i < 3; i++)
        if (Set_In(Set_CapButton(w, i), x, y + kSetCapH)) return i;
    return -1;
}

// Icon and title are centered on the whole width, laid out from the current size on every paint,
// so a move or snap can never leave them off center.
static D2D1_RECT_F Set_CapTitleRect(const SetWindow& w) {
    float tw = std::min(Set_TextW(w.fCap.get(), kSetTitle) + 2, std::max(0.0f, w.cw - 4 * kSetCapBtnW - 40));
    float x = std::round((w.cw - (16 + 8 + tw)) / 2);
    return {x, 0, x + 16 + 8 + tw, kSetCapH};
}

static void Set_DrawMark(SetPaint& p, IDWriteTextFormat* glyph, const D2D1_RECT_F& r, float a) {
    float s = r.right - r.left;
    SetCom<ID2D1LinearGradientBrush> g;
    D2D1_GRADIENT_STOP stops[2] = {{0, D2DColor(mocha::text, a * p.fade)}, {1, D2DColor(mocha::text, a * p.fade)}};
    SetCom<ID2D1GradientStopCollection> coll;
    if (SUCCEEDED(p.rt->CreateGradientStopCollection(stops, 2, coll.put())) &&
        SUCCEEDED(p.rt->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.right, r.bottom)),
            coll.get(), g.put())))
        p.rt->FillRoundedRectangle(D2D1::RoundedRect(r, s * 0.24f, s * 0.24f), g.get());
    p.Glyph(glyph, 0xE80A, r, mocha::crust);
}

static void Set_DrawCaption(SetPaint& p, SetWindow& w) {
    if (!w.glass) p.Fill({0, 0, w.cw, kSetCapH}, mocha::crust);  // Glass: the window tint already covers it.
    D2D1_RECT_F t = Set_CapTitleRect(w);
    float mid = kSetCapH / 2;
    Set_DrawMark(p, w.fCapGlyph.get(), {t.left, mid - 8, t.left + 16, mid + 8}, w.active ? 1.0f : 0.6f);
    p.Text(w.fCap.get(), kSetTitle, {t.left + 24, 0, t.right, kSetCapH}, w.active ? mocha::text : mocha::overlay1);
    for (int i = 0; i < 3; i++) {
        D2D1_RECT_F b = Set_CapButton(w, i);
        float hov = Set_GlowOf(w, 20 + i);
        if (w.capDown == i + 1) hov = 1;
        if (i == 2) p.Fill(b, mocha::red, 0.92f * hov);
        else p.Fill(b, mocha::surface0, 0.85f * hov);
        UINT32 c = i == 2 && hov > 0.5f ? mocha::crust : (w.active ? mocha::text : mocha::overlay1);
        WCHAR glyph = i == 0 ? 0xE921 : i == 1 ? (w.hwnd && IsZoomed(w.hwnd) ? 0xE923 : 0xE922) : 0xE8BB;
        p.Glyph(w.fCapGlyph.get(), glyph, b, c);
    }
}

// Draws the whole window into rt (DIPs). The target's DPI does the scaling.
// The title bar is drawn first; everything else is in content DIPs, shifted down by kSetCapH.
static void Set_PlaceEdit(SetWindow& w);

static void Set_Draw(SetWindow& w, ID2D1RenderTarget* rt) {
    if (Set_FitBoxes(w)) {
        Set_BuildPage(w);
        Set_PlaceEdit(w);
    }
    SetPaint p(w, rt);
    const auto base = D2D1::Matrix3x2F::Translation(0, kSetCapH);
    rt->BeginDraw();
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    if (w.glass) {
        // One tint over the whole window, title bar included, so the page and the title bar are
        // the same material. With the Windows title bar (drawn by DWM with no tint) the tint fades
        // in over the top of the page instead, so there is no step at the seam (T3 pass 24 method).
        rt->Clear(D2D1::ColorF(0, 0));
        const float total = w.ch + kSetCapH;
        if (w.nativeTitle) {
            SetCom<ID2D1LinearGradientBrush> fade;
            D2D1_GRADIENT_STOP stops[2] = {{0, D2DColor(mocha::crust, kSetGlassTint)}, {1, D2DColor(mocha::crust, kSetGlassTint)}};
            SetCom<ID2D1GradientStopCollection> coll;
            if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 2, coll.put())) &&
                SUCCEEDED(rt->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, kSetNativeFade)), coll.get(),
                    fade.put())))
                rt->FillRectangle(D2D1::RectF(0, 0, w.cw, kSetNativeFade), fade.get());
            p.Fill({0, kSetNativeFade, w.cw, total}, mocha::crust, kSetGlassTint);
        } else {
            p.Fill({0, 0, w.cw, total}, mocha::crust, kSetGlassTint);
        }
    } else {
        rt->Clear(D2DColor(mocha::crust));
    }
    if (!w.nativeTitle) Set_DrawCaption(p, w);
    rt->SetTransform(base);
    float viewH = Set_ViewH(w);

    auto drawPage = [&](SetPaint& cp) {
        ID2D1RenderTarget* crt = cp.rt;
        crt->PushAxisAlignedClip(D2D1::RectF(Set_RailSpace(w) + 4, 0, w.cw, viewH), D2D1_ANTIALIAS_MODE_ALIASED);
        auto place = [&](int index, float& rise) {
            float t = w.pageT >= 1 ? 1.0f : Set_Ease(w.pageT * 1.7f - std::min(index, 9) * 0.07f);
            cp.fade = t;
            rise = (1 - t) * 10;
            crt->SetTransform(D2D1::Matrix3x2F::Translation(0, -w.scrollY + rise) * base);
        };
        float rise = 0;
        place(0, rise);
        cp.Text(w.fTitle.get(), Tr(kSetPages[w.page].name), {w.contentX, 36, w.contentX + w.contentW, 72}, mocha::text);
        place(1, rise);
        cp.Text(w.fIntro.get(), Tr(kSetPages[w.page].intro), {w.contentX, 76, w.contentX + w.contentW, 76 + 40}, mocha::subtext0);
        for (size_t i = 0; i < w.items.size(); i++) {
            const SetItem& it = w.items[i];
            if (it.y + it.h < w.scrollY - 20 || it.y > w.scrollY + viewH + 20) continue;
            place((int)i + 2, rise);
            switch (it.type) {
                case SetItem::Group: Set_DrawGroup(cp, w, it); break;
                case SetItem::Row: Set_DrawRow(cp, w, it); break;
                case SetItem::Banner: Set_DrawBanner(cp, w, it); break;
                case SetItem::About: Set_DrawAbout(cp, w, it, (int)i); break;
                case SetItem::Link: Set_DrawLinkRow(cp, w, it, (int)i); break;
                case SetItem::Support: Set_DrawSupport(cp, w, it, (int)i); break;
            }
        }
        cp.fade = 1;
        crt->SetTransform(base);
        crt->PopAxisAlignedClip();
    };

    // The footer dock floats over the page with a real blur of the rows under it, clipped to the
    // pill, all in this window. (An owned popup with its own backdrop showed square corners:
    // DWM paints an accent backdrop over the full window rectangle and ignores the region.)
    const D2D1_SIZE_F full = D2D1::SizeF(w.cw, w.ch + kSetCapH);
    bool blurred = false;
    if (!w.render) {
        if (w.layer && (w.layerSize.width != full.width || w.layerSize.height != full.height)) w.layer.reset();
        if (!w.layer && SUCCEEDED(rt->CreateCompatibleRenderTarget(full, w.layer.put()))) w.layerSize = full;
        SetCom<ID2D1Bitmap> page;
        SetCom<ID2D1DeviceContext> dc;
        SetCom<ID2D1Effect> blur;
        SetCom<ID2D1RoundedRectangleGeometry> pill;
        SetCom<ID2D1RectangleGeometry> all;
        SetCom<ID2D1PathGeometry> rest;
        D2D1_RECT_F d = Set_FooterDock(w);
        d.top += kSetCapH, d.bottom += kSetCapH;
        const float rad = kSetFootH / 2;
        if (w.layer && SUCCEEDED(rt->QueryInterface(__uuidof(ID2D1DeviceContext), (void**)dc.put())) &&
            SUCCEEDED(dc->CreateEffect(CLSID_D2D1GaussianBlur, blur.put())) &&
            SUCCEEDED(g_setD2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(d, rad, rad), pill.put())) &&
            SUCCEEDED(g_setD2d->CreateRectangleGeometry(D2D1::RectF(0, 0, full.width, full.height), all.put())) &&
            SUCCEEDED(g_setD2d->CreatePathGeometry(rest.put()))) {
            SetCom<ID2D1GeometrySink> sink;
            bool ok = SUCCEEDED(rest->Open(sink.put())) &&
                      SUCCEEDED(all->CombineWithGeometry(pill.get(), D2D1_COMBINE_MODE_EXCLUDE, nullptr, sink.get())) &&
                      SUCCEEDED(sink->Close());
            ID2D1BitmapRenderTarget* lt = w.layer.get();
            if (ok) {
                SetPaint lp(w, lt);
                lt->BeginDraw();
                lt->SetTransform(D2D1::Matrix3x2F::Identity());
                lt->Clear(D2D1::ColorF(0, 0));
                lt->SetTransform(base);
                drawPage(lp);
                ok = SUCCEEDED(lt->EndDraw()) && SUCCEEDED(lt->GetBitmap(page.put()));
            }
            if (!ok) w.layer.reset();
            if (ok) {
                rt->SetTransform(D2D1::Matrix3x2F::Identity());
                rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), rest.get()), nullptr);
                rt->DrawBitmap(page.get());
                rt->PopLayer();
                blur->SetInput(0, page.get());
                blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 9.0f);
                blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
                rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), pill.get()), nullptr);
                D2D1_RECT_F src{d.left - 1, d.top - 1, d.right + 1, d.bottom + 1};
                dc->DrawImage(blur.get(), D2D1::Point2F(src.left, src.top), src);
                rt->PopLayer();
                blurred = true;
            }
        }
    }
    if (!blurred) drawPage(p);
    rt->SetTransform(base);

    D2D1_RECT_F th = Set_ScrollThumb(w);
    Set_ThumbFill(p, th, w.dragScroll || w.hover.kind == SetHit::Scroll ? 1.0f : 0.7f);

    Set_DrawFooter(p, w);
    Set_DrawRail(p, w);
    Set_DrawPopup(p, w);
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    if (false) p.GradRing({0, 0, w.cw, w.ch + kSetCapH}, SystemCornerRadius(false), 1, 1);
    HRESULT hr = rt->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) w.rt.reset(), w.layer.reset();
}

// ---- Hit testing --------------------------------------------------------------

static SetHit Set_Hit(SetWindow& w, float x, float y) {
    SetHit h;
    if (y < 0) {
        int b = Set_CapButtonAt(w, x, y);
        if (b >= 0) {
            h.kind = SetHit::Caption;
            h.a = b;
            h.id = 20 + b;
        }
        return h;
    }
    if (w.openSel >= 0) {
        if (Set_In(w.dropRect, x, y)) {
            int v = (int)((y - w.dropRect.top - 4) / kSetDropItem), i = w.dropTop + v;
            if (v >= 0 && v < Set_DropVisible(w) && i < (int)w.fields[w.openSel].options.size() &&
                y >= w.dropRect.top + 4 && x < w.dropRect.right - (Set_DropVisible(w) < (int)w.fields[w.openSel].options.size() ? 12 : 4)) {
                h.kind = SetHit::Drop;
                h.a = w.openSel;
                h.b = i;
            } else {
                h.kind = SetHit::Popup;
            }
        }
        return h;
    }
    D2D1_RECT_F dock = Set_RailDock(w);
    if (y >= dock.top + 14 && y < dock.bottom - 14 && fabsf(x - dock.right) <= kSetEdgeGrab) {
        h.kind = SetHit::RailEdge;
        h.id = 41;
        return h;
    }
    if (x < dock.right + kSetDockGap) {
        if (Set_In(Set_RailToggleRect(w), x, y)) {
            h.kind = SetHit::RailToggle;
            h.id = 42;
            return h;
        }
        for (int i = 0; i < kSetPageCount; i++) {
            if (Set_In(Set_RailRect(w, i), x, y)) {
                h.kind = SetHit::Rail;
                h.a = i;
                h.id = 100 + i;
                return h;
            }
        }
        return h;
    }
    D2D1_RECT_F foot = Set_FooterDock(w);
    if (Set_In(foot, x, y)) {
        for (int i = 0; i < 3; i++) {
            if (Set_In(Set_FooterButton(w, i), x, y)) {
                h.kind = SetHit::Footer;
                h.a = i;
                h.id = 10 + i;
                return h;
            }
        }
        return h;
    }
    D2D1_RECT_F th = Set_ScrollThumb(w);
    if (th.bottom > th.top && x >= w.cw - 16 && y >= th.top && y <= th.bottom) {
        h.kind = SetHit::Scroll;
        return h;
    }
    float cy = y + w.scrollY;
    for (size_t i = 0; i < w.items.size(); i++) {
        const SetItem& it = w.items[i];
        D2D1_RECT_F row = Set_RowRect(w, it);
        if (!Set_In(row, x, cy)) continue;
        if (it.type == SetItem::Link) {
            h.kind = SetHit::Link;
            h.a = (int)i;
            h.id = 3000 + (int)i;
        } else if (it.type == SetItem::Support) {
            h.id = 3700 + (int)i;  // The card lights up; only its button clicks.
            if (Set_In(Set_SupportButton(w, it), x, cy)) {
                h.kind = SetHit::Link;
                h.a = (int)i;
                h.id = 3500 + (int)i;
            }
        } else if (it.type == SetItem::About) {
            h.id = 3800 + (int)i;
        } else if (it.type == SetItem::Row) {
            const SetField& f = w.fields[it.field];
            SetParts pt = Set_Parts(f, row);
            h.a = it.field;
            h.id = 1000 + it.field;
            h.kind = f.kind == SetField::Check ? SetHit::Control : SetHit::Row;
            if (f.kind == SetField::Hotkey && !f.cur.empty() && w.recording != it.field && Set_In(pt.clear, x, cy)) {
                h.kind = SetHit::Clear;
            } else if (f.kind == SetField::Color) {
                for (int c = 0; c < kSetChipCount; c++) {
                    if (Set_In(pt.chip[c], x, cy)) {
                        h.kind = SetHit::Chip;
                        h.b = c;
                    }
                }
                if (h.kind != SetHit::Chip && Set_In(pt.box, x, cy)) h.kind = SetHit::Control;
            } else if (pt.slider &&
                       Set_In({pt.track.left - 8, pt.track.top - 10, pt.track.right + 8, pt.track.bottom + 10}, x, cy)) {
                h.kind = SetHit::Slider;
            } else if (Set_In(pt.box, x, cy)) {
                h.kind = SetHit::Control;
            }
        }
        return h;
    }
    return h;
}

static bool Set_Clickable(const SetHit& h) {
    return h.kind == SetHit::Rail || h.kind == SetHit::Footer || h.kind == SetHit::Control || h.kind == SetHit::Slider ||
           h.kind == SetHit::Clear || h.kind == SetHit::Chip || h.kind == SetHit::Drop || h.kind == SetHit::Link ||
           h.kind == SetHit::Scroll || h.kind == SetHit::RailToggle;
}

// Sidebar width from a drag: below the snap point it collapses to the icon rail.
static void Set_RailDragTo(SetWindow& w, float x) {
    float width = x - kSetDockGap;
    if (width < kSetRailSnap) {
        w.railCollapsed = true;
    } else {
        w.railCollapsed = false;
        w.railW = std::clamp(width, kSetRailMin, kSetRailMax);
        w.railCur = w.railW;
    }
    Set_BuildPage(w);
    Set_Invalidate(w);
}

static void Set_ToggleRail(SetWindow& w) {
    w.railCollapsed = !w.railCollapsed;
    if (!w.railCollapsed && w.railCur < kSetRailMini) w.railCur = kSetRailMini;
    Set_Invalidate(w);
}

// ---- Editing, recording, choosing -----------------------------------------------

static std::wstring Set_FormatHotkey(UINT mods, UINT vk) {
    std::wstring text = std::to_wstring(vk);
    if (!text.empty()) return text;
    // Fallback while the capture part is a stub.
    if (mods & MOD_CONTROL) text += L"Ctrl+";
    if (mods & MOD_ALT) text += L"Alt+";
    if (mods & MOD_SHIFT) text += L"Shift+";
    if (mods & MOD_WIN) text += L"Win+";
    WCHAR name[64] = {};
    LONG scan = (LONG)MapVirtualKeyW(vk, MAPVK_VK_TO_VSC) << 16;
    if (GetKeyNameTextW(scan, name, ARRAYSIZE(name)) > 0) {
        text += name;
    } else {
        text += std::to_wstring(vk);
    }
    return text;
}

static bool Set_IsModifier(UINT vk) {
    switch (vk) {
        case VK_SHIFT: case VK_LSHIFT: case VK_RSHIFT:
        case VK_CONTROL: case VK_LCONTROL: case VK_RCONTROL:
        case VK_MENU: case VK_LMENU: case VK_RMENU:
        case VK_LWIN: case VK_RWIN:
            return true;
    }
    return false;
}

static void Set_StopRecording(SetWindow& w) {
    if (w.recording < 0) return;
    w.recording = -1;
    if (w.status.rfind(Tr(L"Press the keys"), 0) == 0) Set_SetStatus(w, L"");
    
    Set_Invalidate(w);
}

static void Set_StartRecording(SetWindow& w, int k) {
    Set_StopRecording(w);
    w.recording = k;
    // Our own keys would fire instead of reaching the window while it listens.
    if (!w.render) {
        SetFocus(w.hwnd);
    }
    Set_SetStatus(w, Tr(L"Press the keys you want. Esc cancels, Backspace clears."));
}

static void Set_ClosePopup(SetWindow& w) {
    if (w.openSel < 0) return;
    w.openSel = -1;
    w.dropHover = -1;
    w.dropTop = 0;
    Set_Invalidate(w);
}

static void Set_OpenPopup(SetWindow& w, int k) {
    const SetItem* it = Set_ItemOfField(w, k);
    if (!it) return;
    const SetField& f = w.fields[k];
    SetParts pt = Set_Parts(f, Set_RowRect(w, *it));
    float boxTop = pt.box.top - w.scrollY, boxBottom = pt.box.bottom - w.scrollY;
    const int n = (int)f.options.size(), vis = std::min(n, kSetDropMax);
    float h = (float)vis * kSetDropItem + 8;
    float top = boxBottom + 4;
    if (top + h > Set_ViewH(w) + 20 && boxTop - 4 - h > 0) top = boxTop - 4 - h;  // No room below: open upward.
    // As wide as the longest option (text, check mark, scrollbar), growing leftward from the box.
    float textW = 0;
    for (const auto& o : f.options) textW = std::max(textW, Set_TextW(w.fValue.get(), Tr(o.second.c_str())));
    float width = std::max(pt.box.right - pt.box.left, std::ceil(textW) + 52 + (n > vis ? 8 : 0));
    float left = std::max(pt.box.right - width, Set_RailSpace(w) + 8);
    w.dropRect = {left, top, std::max(pt.box.right, left + width), top + h};
    w.openSel = k;
    w.dropHover = -1;
    w.dropTop = 0;
    for (int i = 0; i < n; i++)
        if (f.cur == f.options[i].first) w.dropHover = i;
    Set_DropShow(w, w.dropHover);
    Set_Invalidate(w);
}

static void Set_CommitEdit(SetWindow& w, bool apply) {
    if (w.editing < 0) return;
    int k = w.editing;
    w.editing = -1;  // Hiding the box ends its focus; this stops that from coming back here.
    SetField& f = w.fields[k];
    std::wstring text = w.edit ? Set_WindowText(w.edit) : L"";
    if (w.edit) ShowWindow(w.edit, SW_HIDE);
    if (apply) {
        switch (f.kind) {
            case SetField::Number:
                if (!text.empty()) f.cur = std::to_wstring(std::clamp(_wtoi(text.c_str()), f.min, f.max));
                break;
            case SetField::Color: {
                while (!text.empty() && text.back() == L' ') text.pop_back();
                while (!text.empty() && text.front() == L' ') text.erase(0, 1);
                if (!text.empty() && text[0] != L'#') text = L"#" + text;
                UINT32 rgb;
                if (Set_ParseHex(text, rgb)) {
                    f.cur = Set_HexFromRgb(rgb);
                    Set_SetStatus(w, L"");
                } else {
                    Set_SetStatus(w, Tr(L"Use a color like #89B4FA."), true);
                }
                break;
            }
            default:
                f.cur = text;
        }
    }
    if (!w.render && w.hwnd) SetFocus(w.hwnd);
    Set_Invalidate(w);
}

static void Set_PlaceEdit(SetWindow& w) {
    if (w.editing < 0 || !w.edit) return;
    const SetItem* it = Set_ItemOfField(w, w.editing);
    if (!it) return;
    SetParts pt = Set_Parts(w.fields[w.editing], Set_RowRect(w, *it));
    float left = pt.box.left + (w.fields[w.editing].kind == SetField::Color ? 30 : 10), right = pt.box.right - 8;
    float mid = (pt.box.top + pt.box.bottom) / 2 - w.scrollY + kSetCapH;
    float s = w.dpi / 96.0f;
    SetWindowPos(w.edit, HWND_TOP, (int)(left * s), (int)((mid - 10) * s), (int)((right - left) * s), (int)(20 * s),
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static LRESULT CALLBACK Set_EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
    SetWindow* w = g_setWin.get();
    if (w && w->edit == hwnd) {
        switch (msg) {
            case WM_KEYDOWN:
                if (wParam == VK_RETURN || wParam == VK_TAB) {
                    Set_CommitEdit(*w, true);
                    return 0;
                }
                if (wParam == VK_ESCAPE) {
                    Set_CommitEdit(*w, false);
                    return 0;
                }
                break;
            case WM_CHAR:
                if (wParam == VK_RETURN || wParam == VK_ESCAPE || wParam == VK_TAB) return 0;  // No beep.
                break;
            case WM_KILLFOCUS:
                if (w->editing >= 0) Set_CommitEdit(*w, true);
                break;
        }
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static void Set_BeginEdit(SetWindow& w, int k) {
    Set_CommitEdit(w, true);
    Set_StopRecording(w);
    Set_ClosePopup(w);
    SetField& f = w.fields[k];
    if (w.edit) DestroyWindow(w.edit);
    if (w.editFont) DeleteObject(w.editFont);
    DWORD style = WS_CHILD | ES_AUTOHSCROLL | (f.kind == SetField::Number ? ES_NUMBER | ES_CENTER : 0);
    // Over the extended frame GDI text would come out see-through; a layered child keeps it solid.
    w.edit = CreateWindowExW(w.glass ? WS_EX_LAYERED : 0, L"EDIT", L"", style, 0, 0, 10, 10, w.hwnd, nullptr,
                             GetModuleHandleW(nullptr), nullptr);
    if (!w.edit) return;
    if (w.glass) SetLayeredWindowAttributes(w.edit, 0, 255, LWA_ALPHA);
    int base = std::clamp(500, 100, 900);
    w.editFont = CreateFontW(-MulDiv(13, w.dpi, 96), 0, 0, 0, base, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, w.family.c_str());
    SendMessageW(w.edit, WM_SETFONT, (WPARAM)w.editFont, TRUE);
    SendMessageW(w.edit, EM_SETLIMITTEXT, f.kind == SetField::Number ? 6 : f.kind == SetField::Color ? 7 : 260, 0);
    SetWindowTextW(w.edit, f.kind == SetField::Number ? Set_FieldValue(f).c_str() : f.cur.c_str());
    SetWindowSubclass(w.edit, Set_EditProc, 1, 0);
    w.editing = k;
    Set_PlaceEdit(w);
    if (!w.render) SetFocus(w.edit);
    SendMessageW(w.edit, EM_SETSEL, 0, -1);
    Set_Invalidate(w);
}

static void Set_Choose(SetWindow& w, int k, int option) {
    SetField& f = w.fields[k];
    if (option >= 0 && option < (int)f.options.size()) f.cur = f.options[option].first;
    Set_ClosePopup(w);
}

static void Set_SliderTo(SetWindow& w, int k, float x) {
    const SetItem* it = Set_ItemOfField(w, k);
    if (!it) return;
    SetField& f = w.fields[k];
    SetParts pt = Set_Parts(f, Set_RowRect(w, *it));
    float t = std::clamp((x - pt.track.left) / (pt.track.right - pt.track.left), 0.0f, 1.0f);
    f.cur = std::to_wstring(f.min + (int)(t * (f.max - f.min) + 0.5f));
    Set_Invalidate(w);
}

static void Set_Save(std::shared_ptr<SetWindow> keep);

static void Set_ResetToSuggested(SetWindow& w) {
    Set_CommitEdit(w, false);
    Set_StopRecording(w);
    Set_ClosePopup(w);
    for (auto& f : w.fields) Set_PutValue(f, f.def);
    Set_SetStatus(w, Tr(L"Suggested values are filled in. Press Save to keep them."));
}

static void Set_BuildAndClamp(SetWindow& w) {
    Set_BuildPage(w);
    Set_PlaceEdit(w);
    Set_Invalidate(w);
}

static void Set_GoPage(SetWindow& w, int page) {
    page = std::clamp(page, 0, kSetPageCount - 1);
    Set_CommitEdit(w, true);
    Set_StopRecording(w);
    Set_ClosePopup(w);
    if (page == w.page) return;
    w.page = page;
    w.scrollY = w.scrollTarget = 0;
    w.focus = -1;
    w.pageT = Set_Anim(w) ? 0.0f : 1.0f;
    Set_BuildAndClamp(w);
}

static void Set_OpenLink(PCWSTR url) {
#ifdef SCM_HARNESS
    Wh_Log(L"settings: would open %s", url);
#else
    ShellExecuteW(nullptr, L"open", url, nullptr, nullptr, SW_SHOWNORMAL);
#endif
}

// A click (or Space or Enter) on a field's control.
static void Set_ActivateField(SetWindow& w, int k, const SetHit* hit) {
    SetField& f = w.fields[k];
    if (w.recording >= 0 && w.recording != k) Set_StopRecording(w);
    switch (f.kind) {
        case SetField::Check:
            f.cur = f.cur == L"1" ? L"0" : L"1";
            Set_Invalidate(w);
            break;
        case SetField::Choice:
            if (w.openSel == k) {
                Set_ClosePopup(w);
            } else {
                Set_CommitEdit(w, true);
                Set_OpenPopup(w, k);
            }
            break;
        case SetField::Number:
        case SetField::Text:
            Set_BeginEdit(w, k);
            break;
        case SetField::Color:
            if (hit && hit->kind == SetHit::Chip) {
                Set_CommitEdit(w, false);
                f.cur = Set_HexFromRgb(kSetChips[hit->b]);
                Set_SetStatus(w, L"");
            } else {
                Set_BeginEdit(w, k);
            }
            break;
        case SetField::Hotkey:
            if (hit && hit->kind == SetHit::Clear) {
                f.cur.clear();
                Set_Invalidate(w);
            } else if (w.recording == k) {
                Set_StopRecording(w);
            } else {
                Set_CommitEdit(w, true);
                Set_StartRecording(w, k);
            }
            break;
    }
}

static void Set_FooterAction(SetWindow& w, int which) {
    if (which == 0) {
        Set_ResetToSuggested(w);
    } else if (which == 1) {
        if (!w.render) DestroyWindow(w.hwnd);  // w is gone after this.
    } else {
        Set_CommitEdit(w, true);
        if (g_setWin && g_setWin->hwnd == w.hwnd) Set_Save(g_setWin);
    }
}

// ---- Saving ---------------------------------------------------------------------

static void Set_ApplyFrame(SetWindow& w);
static void Set_Relayout(SetWindow& w);

static void Set_Save(std::shared_ptr<SetWindow> keep) {
    SetWindow& w = *keep;
    if (w.saving) return;
    Set_StopRecording(w);
    int changed = 0;
    for (const auto& field : w.fields) changed += Set_FieldValue(field) != field.value;
    if (!changed) {
        Set_SetStatus(w, Tr(L"Nothing changed"));
        return;
    }
#ifdef SEAMLESS_PREVIEW
    bool ok = true;
    for (auto& f : w.fields) {
        auto value = Set_FieldValue(f);
        if (value != f.value) {
            ok = Preview::PutSetting(f.key, value) && ok;
            if (wcscmp(f.key, L"interfaceLanguage") == 0) Preview::languageOverride.clear();
        }
    }
    if (!ok) { Set_SetStatus(w, Tr(L"Preview settings could not be saved."), true); return; }
    launchpad_i18n::Select(GetStringSetting(L"interfaceLanguage"));
    Set_MakeFormats(w);
    Set_LoadFields(w);
    Set_ApplyFrame(w);
    Set_Relayout(w);
    SetWindowTextW(w.hwnd, Tr(L"RYJS Launchpad settings"));
    Set_SetStatus(w, Tr(L"Saved"));
    if (g_wnd) PostMessageW(g_wnd, WM_APP_RELOAD, 0, 0);
    return;
#else
    std::vector<launchpad_settings_store::Change> changes;
    for (const auto& field : w.fields) {
        auto value=Set_FieldValue(field);
        if (value!=field.value) {
            std::wstring host=WindhawkUtils::StringSetting::make(field.key).get();
            changes.push_back({field.key,value,host});
        }
    }
    if (!launchpad_settings_store::Save(StoragePath(L"ui-settings.ini"),changes)) {
        Set_SetStatus(w,Tr(L"Could not save private settings. The previous file was kept."),true);
        return;
    }
    launchpad_i18n::Select(GetStringSetting(L"interfaceLanguage"));
    Set_MakeFormats(w); Set_LoadFields(w); Set_ApplyFrame(w); Set_Relayout(w);
    SetWindowTextW(w.hwnd,Tr(L"RYJS Launchpad settings"));
    Set_SetStatus(w,Tr(L"Saved"));
    if (g_wnd) PostMessageW(g_wnd,WM_APP_RELOAD,0,0);

#endif
}

// ---- Animation tick ----------------------------------------------------------------

static bool Set_Tick(SetWindow& w) {
    bool moved = false, anim = Set_Anim(w);
    ULONGLONG tickNow = GetTickCount64();
    float dt = w.lastTick ? (float)std::clamp((tickNow - w.lastTick) / 1000.0, 0.0, 0.1) : 0.015f;
    w.lastTick = tickNow;
    auto step = [&](float& v, float target, float rate) {
        float d = target - v;
        if (!anim || fabsf(d) < 0.02f) {
            if (v != target) {
                v = target;
                moved = true;
            }
            return;
        }
        v += d * (1 - std::pow(1 - rate, dt / 0.015f));
        moved = true;
    };
    int ids[2];
    Set_HoverIds(w.hover, ids);
    for (int id : ids)
        if (id >= 0 && !w.glow.count(id)) w.glow[id] = 0;
    for (auto it = w.glow.begin(); it != w.glow.end();) {
        bool on = it->first == ids[0] || it->first == ids[1];
        step(it->second, on ? 1.0f : 0.0f, 0.3f);
        if (!on && it->second == 0) {
            it = w.glow.erase(it);
        } else {
            ++it;
        }
    }
    for (size_t k = 0; k < w.fields.size(); k++) {
        if (w.fields[k].kind != SetField::Check) continue;
        float target = w.fields[k].cur == L"1" ? 1.0f : 0.0f;
        if (!w.knob.count((int)k)) w.knob[(int)k] = target;
        step(w.knob[(int)k], target, 0.35f);
    }
    if (w.scrollY != w.scrollTarget) {
        float d = w.scrollTarget - w.scrollY;
        if (!anim || fabsf(d) < 0.5f) {
            w.scrollY = w.scrollTarget;
        } else {
            w.scrollY += d * (1 - std::exp(-dt * 19));
        }
        moved = true;
        Set_PlaceEdit(w);
    }
    if (w.pageT < 1) {
        w.pageT = anim ? std::min(1.0f, w.pageT + dt / 0.22f) : 1.0f;
        moved = true;
    }
    // Sidebar open / collapse, and the dock edges lighting up under the pointer.
    if (!w.dragRail) {
        float target = w.railCollapsed ? kSetRailMini : w.railW, d = target - w.railCur;
        if (d != 0) {
            w.railCur = (!anim || fabsf(d) < 0.6f) ? target : w.railCur + d * 0.28f;
            Set_BuildPage(w);
            Set_PlaceEdit(w);
            moved = true;
        }
    }
    bool inRail = w.mouseX >= 0 && Set_In(Set_RailDock(w), w.mouseX, w.mouseY);
    D2D1_RECT_F foot = Set_FooterDock(w);
    bool inFoot = w.mouseX >= 0 && Set_In(foot, w.mouseX, w.mouseY);
    step(w.railGlow, inRail ? 1.0f : 0.0f, 0.25f);
    step(w.footGlow, inFoot ? 1.0f : 0.0f, 0.25f);
    return moved;
}

// ---- Keyboard focus ----------------------------------------------------------------

// Focus order: the page's rows, then Reset, Cancel, Save (-1, -2, -3).
static std::vector<int> Set_FocusOrder(const SetWindow& w) {
    std::vector<int> order;
    for (auto& it : w.items)
        if (it.type == SetItem::Row) order.push_back(it.field);
    order.push_back(-1);
    order.push_back(-2);
    order.push_back(-3);
    return order;
}

static void Set_EnsureVisible(SetWindow& w, float top, float bottom) {
    float view = Set_ViewH(w);
    float clear = Set_FootClear() - 18;  // Keep focused rows above the floating footer.
    if (top - 16 < w.scrollTarget) {
        w.scrollTarget = top - 16;
    } else if (bottom + 16 + clear > w.scrollTarget + view) {
        w.scrollTarget = bottom + 16 + clear - view;
    }
    w.scrollTarget = std::clamp(w.scrollTarget, 0.0f, std::max(0.0f, w.contentH - view));
}

static void Set_MoveFocus(SetWindow& w, int dir) {
    Set_CommitEdit(w, true);
    Set_ClosePopup(w);
    std::vector<int> order = Set_FocusOrder(w);
    int index = -1;
    for (size_t i = 0; i < order.size(); i++)
        if (order[i] == w.focus) index = (int)i;
    index = index < 0 ? (dir > 0 ? 0 : (int)order.size() - 1) : (index + dir + (int)order.size()) % (int)order.size();
    w.focus = order[index];
    w.kbd = true;
    if (w.focus >= 0) {
        if (const SetItem* it = Set_ItemOfField(w, w.focus)) Set_EnsureVisible(w, it->y, it->y + it->h);
    }
    Set_Invalidate(w);
}

static void Set_Adjust(SetWindow& w, int dir) {
    if (w.focus < 0) return;
    SetField& f = w.fields[w.focus];
    if (f.kind == SetField::Number) {
        int span = f.max - f.min, step = span <= 100 ? 1 : span <= 3000 ? 10 : 100;
        f.cur = std::to_wstring(std::clamp(_wtoi(Set_FieldValue(f).c_str()) + dir * step, f.min, f.max));
    } else if (f.kind == SetField::Choice && !f.options.empty()) {
        int n = (int)f.options.size(), at = 0;
        for (int i = 0; i < n; i++)
            if (f.cur == f.options[i].first) at = i;
        f.cur = f.options[(at + dir + n) % n].first;
    } else if (f.kind == SetField::Check) {
        f.cur = dir > 0 ? L"1" : L"0";
    }
    Set_Invalidate(w);
}

static bool Set_OnKey(SetWindow& w, UINT vk) {
    if (w.saving) return true;
    if (w.recording >= 0) {
        if (Set_IsModifier(vk)) return true;
        UINT mods = 0;
        if (GetKeyState(VK_CONTROL) & 0x8000) mods |= MOD_CONTROL;
        if (GetKeyState(VK_MENU) & 0x8000) mods |= MOD_ALT;
        if (GetKeyState(VK_SHIFT) & 0x8000) mods |= MOD_SHIFT;
        if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) mods |= MOD_WIN;
        SetField& f = w.fields[w.recording];
        if (vk == VK_ESCAPE && !mods) {
            // Cancel: keep what was there.
        } else if ((vk == VK_BACK || vk == VK_DELETE) && !mods) {
            f.cur.clear();
        } else {
            f.cur = Set_FormatHotkey(mods, vk);
        }
        Set_StopRecording(w);
        return true;
    }
    if (w.openSel >= 0) {
        const SetField& f = w.fields[w.openSel];
        int n = (int)f.options.size();
        if (vk == VK_ESCAPE) Set_ClosePopup(w);
        else if (vk == VK_UP) Set_DropShow(w, w.dropHover = w.dropHover < 0 ? n - 1 : (w.dropHover - 1 + n) % n);
        else if (vk == VK_DOWN) Set_DropShow(w, w.dropHover = w.dropHover < 0 ? 0 : (w.dropHover + 1) % n);
        else if (vk == VK_RETURN || vk == VK_SPACE) Set_Choose(w, w.openSel, w.dropHover);
        Set_Invalidate(w);
        return true;
    }
    bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0, shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    if (ctrl && vk == 'B') {  // Same shortcut as T3's sidebar.
        Set_ToggleRail(w);
        return true;
    }
    float view = Set_ViewH(w), maxScroll = std::max(0.0f, w.contentH - view);
    switch (vk) {
        case VK_TAB:
            if (ctrl) Set_GoPage(w, (w.page + (shift ? kSetPageCount - 1 : 1)) % kSetPageCount);
            else Set_MoveFocus(w, shift ? -1 : 1);
            return true;
        case VK_SPACE:
        case VK_RETURN:
            w.kbd = true;
            if (w.focus >= 0) Set_ActivateField(w, w.focus, nullptr);
            else if (w.focus <= -1 && w.focus >= -3) Set_FooterAction(w, -1 - w.focus);
            return true;
        case VK_LEFT:
        case VK_RIGHT:
            w.kbd = true;
            Set_Adjust(w, vk == VK_RIGHT ? 1 : -1);
            return true;
        case VK_UP: w.scrollTarget = std::clamp(w.scrollTarget - 60, 0.0f, maxScroll); break;
        case VK_DOWN: w.scrollTarget = std::clamp(w.scrollTarget + 60, 0.0f, maxScroll); break;
        case VK_PRIOR: w.scrollTarget = std::clamp(w.scrollTarget - view * 0.9f, 0.0f, maxScroll); break;
        case VK_NEXT: w.scrollTarget = std::clamp(w.scrollTarget + view * 0.9f, 0.0f, maxScroll); break;
        case VK_HOME: w.scrollTarget = 0; break;
        case VK_END: w.scrollTarget = maxScroll; break;
        case VK_ESCAPE:
            if (!w.render) DestroyWindow(w.hwnd);
            return true;
        default:
            if (ctrl && vk >= '1' && vk <= '6') {
                Set_GoPage(w, (int)(vk - '1'));
                return true;
            }
            return false;
    }
    if (!Set_Anim(w)) w.scrollY = w.scrollTarget;
    Set_PlaceEdit(w);
    Set_Invalidate(w);
    return true;
}

// ---- Window ---------------------------------------------------------------------

static bool Set_EnsureRt(SetWindow& w) {
    if (w.rt) return true;
    if (!Set_EnsureFactories()) return false;
    RECT rc;
    GetClientRect(w.hwnd, &rc);
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        (float)w.dpi, (float)w.dpi);
    return SUCCEEDED(g_setD2d->CreateHwndRenderTarget(
        props, D2D1::HwndRenderTargetProperties(w.hwnd, D2D1::SizeU(std::max(1L, rc.right), std::max(1L, rc.bottom))),
        w.rt.put()));
}

static void Set_Relayout(SetWindow& w) {
    RECT rc;
    GetClientRect(w.hwnd, &rc);
    w.cw = rc.right * 96.0f / w.dpi;
    w.ch = std::max(0.0f, rc.bottom * 96.0f / w.dpi - kSetCapH);
    if (w.rt) w.rt->Resize(D2D1::SizeU(std::max(1L, rc.right), std::max(1L, rc.bottom)));
    Set_BuildAndClamp(w);
}

// A switch's stored value, found by the end of its key so a "SettingsWindow." prefix still matches.
static bool Set_FieldOn(const SetWindow& w, PCWSTR key, bool def) {
    const size_t n = wcslen(key);
    for (const auto& f : w.fields) {
        const size_t m = wcslen(f.key);
        if (m >= n && wcscmp(f.key + m - n, key) == 0 && (m == n || f.key[m - n - 1] == L'.')) return f.value == L"1";
    }
    return def;
}

// "Native Windows title bar" and "Blurred settings window", applied to the live window: on first
// open and again whenever a save or a reload changes them, so neither needs a reopen.
static void Set_ApplyFrame(SetWindow& w) {
    if (w.render || !w.hwnd) return;
    const HWND hwnd = w.hwnd;
    const bool native = Set_FieldOn(w, L"nativeSettingsTitleBar", false), glass = Set_FieldOn(w, L"settingsGlass", true);
    const bool first = !w.frameApplied, wasGlass = w.glass;
    if (!first && native == w.nativeTitle && glass == w.glass) return;
    w.frameApplied = true;
    w.nativeTitle = native;
    w.glass = glass;
    kSetCapH = native ? 0.0f : 40.0f;
    struct SetAccentPolicy { int state, flags, abgr, animation; };
    struct SetCompositionData { DWORD attribute; void* data; SIZE_T size; };
    using SetComposition_t = BOOL(WINAPI*)(HWND, SetCompositionData*);
    auto setComposition =
        (SetComposition_t)(void*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
    if (glass) {
        // Acrylic as one sheet over the whole window, the same material Seamless Flyouts
        // gives the Launchpad settings window in Explorer: the accent acrylic with an 85%
        // Mocha base tint, under the page's own crust tint. The system backdrop
        // (DWMSBT_TRANSIENTWINDOW) went light grey over bright windows and turned to a flat
        // grey slab when the window lost focus; the accent acrylic stays the same.
        int backdrop = 1;  // DWMSBT_NONE
        DwmSetWindowAttribute(hwnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &backdrop, sizeof(backdrop));
        DWM_BLURBEHIND bb{};
        bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        bb.fEnable = TRUE;
        bb.hRgnBlur = CreateRectRgn(0, 0, -1, -1);
        DwmEnableBlurBehindWindow(hwnd, &bb);
        if (bb.hRgnBlur) DeleteObject(bb.hRgnBlur);
        if (setComposition) {
            SetAccentPolicy policy{4 /* ACCENT_ENABLE_ACRYLICBLURBEHIND */, 2, (int)0x01242424, 0};  // same as the launcher grid
            SetCompositionData data{19 /* WCA_ACCENT_POLICY */, &policy, sizeof(policy)};
            setComposition(hwnd, &data);
        }
        MARGINS margins{-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
        COLORREF none = 0xFFFFFFFE;  // DWMWA_COLOR_NONE
        DwmSetWindowAttribute(hwnd, 35 /* DWMWA_CAPTION_COLOR */, &none, sizeof(none));
    } else if (wasGlass) {
        // Glass switched off while open: back to a plain solid window.
        if (setComposition) {
            SetAccentPolicy policy{0, 0, 0, 0};
            SetCompositionData data{19 /* WCA_ACCENT_POLICY */, &policy, sizeof(policy)};
            setComposition(hwnd, &data);
        }
        DWM_BLURBEHIND bb{};
        bb.dwFlags = DWM_BB_ENABLE;
        bb.fEnable = FALSE;
        DwmEnableBlurBehindWindow(hwnd, &bb);
        MARGINS margins{0, 0, 0, 0};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
        int backdrop = 0;  // DWMSBT_AUTO
        DwmSetWindowAttribute(hwnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &backdrop, sizeof(backdrop));
        COLORREF caption = RefFromRgb(mocha::crust);
        DwmSetWindowAttribute(hwnd, 35 /* DWMWA_CAPTION_COLOR */, &caption, sizeof(caption));
    }
    // With the frame extended DWM would draw its own minimize, maximize and close buttons over the
    // built-in title bar; without a system menu it draws none. The Windows title bar keeps it.
    // Minimize, maximize and close still work from the taskbar either way.
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    SetWindowLongPtrW(hwnd, GWL_STYLE, native ? (style | WS_SYSMENU) : (style & ~WS_SYSMENU));
    // Set_WndProc's WM_NCCALCSIZE drops or keeps the system title bar to match.
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    if (!first) {
        Set_Relayout(w);
        RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
    }
}

static void Set_RememberRect(SetWindow& w) {
    if (w.render || IsIconic(w.hwnd) || IsZoomed(w.hwnd)) return;
    GetWindowRect(w.hwnd, &w.normalRect);
}

static LRESULT CALLBACK Set_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    SetWindow* w = g_setWin && g_setWin->hwnd == hwnd ? g_setWin.get() : nullptr;
    if (!w) return DefWindowProcW(hwnd, msg, wParam, lParam);
    if (w->nativeTitle) {
        LRESULT r = 0;
        if (DwmDefWindowProc(hwnd, msg, wParam, lParam, &r)) return r;  // Hover and clicks on DWM's buttons.
    }
    auto dipX = [&](LPARAM l) { return (float)(short)LOWORD(l) * 96.0f / w->dpi; };
    auto dipY = [&](LPARAM l) { return (float)(short)HIWORD(l) * 96.0f / w->dpi - kSetCapH; };
    switch (msg) {
        case WM_APP_SETTINGS_RELOAD:
            if (!w->saving) {
                launchpad_i18n::Select(GetStringSetting(L"interfaceLanguage"));
                Set_MakeFormats(*w);
                Set_LoadFields(*w);
                Set_ApplyFrame(*w);
                Set_Relayout(*w);
                SetWindowTextW(hwnd, Tr(L"RYJS Launchpad settings"));
            }
            return 0;
        case WM_APP_SETTINGS_ACTIVATE: ShowWindow(hwnd, SW_RESTORE); ForceForeground(hwnd); return 0;
        case WM_NCCALCSIZE:
            // No system title bar: keep the side and bottom frames, the client starts at the top edge.
            if (wParam && !w->render && !w->nativeTitle) {
                auto* nc = (NCCALCSIZE_PARAMS*)lParam;
                LONG top = nc->rgrc[0].top;
                LRESULT r = DefWindowProcW(hwnd, msg, wParam, lParam);
                nc->rgrc[0].top = top;
                if (IsZoomed(hwnd))
                    nc->rgrc[0].top += GetSystemMetricsForDpi(SM_CYSIZEFRAME, w->dpi) +
                                       GetSystemMetricsForDpi(SM_CXPADDEDBORDER, w->dpi);
                return r;
            }
            break;
        case WM_NCHITTEST: {
            if (w->render || w->nativeTitle) break;
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hwnd, &pt);
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (pt.x < 0 || pt.y < 0 || pt.x >= rc.right || pt.y >= rc.bottom) break;
            int frame = GetSystemMetricsForDpi(SM_CYSIZEFRAME, w->dpi) + GetSystemMetricsForDpi(SM_CXPADDEDBORDER, w->dpi);
            if (!IsZoomed(hwnd) && pt.y < frame)
                return pt.x < frame * 2 ? HTTOPLEFT : pt.x >= rc.right - frame * 2 ? HTTOPRIGHT : HTTOP;
            float x = pt.x * 96.0f / w->dpi, y = pt.y * 96.0f / w->dpi - kSetCapH;
            if (y < 0) return Set_CapButtonAt(*w, x, y) >= 0 ? HTCLIENT : HTCAPTION;
            return HTCLIENT;
        }
        case WM_ACTIVATE:
            w->active = LOWORD(wParam) != WA_INACTIVE;
            Set_Invalidate(*w);
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
        case WM_PRINTCLIENT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            if (!w->render && Set_EnsureRt(*w)) Set_Draw(*w, w->rt.get());
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CTLCOLOREDIT: {
            HDC dc = (HDC)wParam;
            SetTextColor(dc, RefFromRgb(mocha::text));
            SetBkColor(dc, RefFromRgb(kSetBoxFill));
            return (LRESULT)w->editBrush;
        }
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED && !w->render) {
                Set_Relayout(*w);
                Set_RememberRect(*w);
                RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);  // No stale title bar after a snap.
            }
            return 0;
        case WM_MOVE:
            Set_RememberRect(*w);
            return 0;
        case WM_GETMINMAXINFO: {
            auto* mm = (MINMAXINFO*)lParam;
            RECT r{0, 0, MulDiv(760, w->dpi, 96), MulDiv(520, w->dpi, 96)};
            AdjustWindowRectExForDpi(&r, GetWindowLongW(hwnd, GWL_STYLE), FALSE, GetWindowLongW(hwnd, GWL_EXSTYLE),
                                     w->dpi);
            mm->ptMinTrackSize = {r.right - r.left, r.bottom - r.top};
            return 0;
        }
        case WM_DPICHANGED: {
            if (g_setForceDpi) return 0;
            Set_CommitEdit(*w, true);
            w->dpi = HIWORD(wParam);
            if (w->rt) w->rt->SetDpi((float)w->dpi, (float)w->dpi);
            auto* r = (RECT*)lParam;
            SetWindowPos(hwnd, nullptr, r->left, r->top, r->right - r->left, r->bottom - r->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            Set_Relayout(*w);
            return 0;
        }
        case WM_TIMER:
            if (wParam == kSetTimerId) {
                if (Set_Tick(*w)) Set_Invalidate(*w);
                return 0;
            }
            break;
        case WM_MOUSEMOVE: {
            if (w->saving) return 0;
#ifdef LP_TEST_HOVER  // Only the test script's moves (marked 0x8000), never a real cursor passing over.
            if (!(wParam & 0x8000)) return 0;
#endif
            float x = dipX(lParam), y = dipY(lParam);
#ifndef LP_TEST_HOVER  // The test build feeds pointer moves by message; a leave would undo them.
            if (!w->tracking && !w->render) {
#else
            if (false) {
#endif
                TRACKMOUSEEVENT t{sizeof(t), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&t);
                w->tracking = true;
            }
            w->mouseX = x;
            w->mouseY = y;
            if (w->dragRail) {
                Set_RailDragTo(*w, x - w->dragOff);
                return 0;
            }
            if (w->dragSlider >= 0) {
                Set_SliderTo(*w, w->dragSlider, x);
                return 0;
            }
            if (w->dragScroll) {
                D2D1_RECT_F th = Set_ScrollThumb(*w);
                float view = Set_ViewH(*w), thh = th.bottom - th.top, range = view - thh - 8;
                float t = range > 0 ? std::clamp((y - w->dragOff - 4) / range, 0.0f, 1.0f) : 0;
                w->scrollTarget = w->scrollY = t * std::max(0.0f, w->contentH - view);
                Set_PlaceEdit(*w);
                Set_Invalidate(*w);
                return 0;
            }
            SetHit h = Set_Hit(*w, x, y);
            int dh = h.kind == SetHit::Drop ? h.b : -1;
            if (h.kind != w->hover.kind || h.id != w->hover.id || h.a != w->hover.a || h.b != w->hover.b ||
                dh != w->dropHover) {
                w->hover = h;
                if (w->openSel >= 0) w->dropHover = dh;
                Set_Invalidate(*w);
            }
            return 0;
        }
        case WM_MOUSELEAVE: {
            POINT pointer; GetCursorPos(&pointer);
            w->tracking = false;
            w->hover = SetHit();
            if (!w->dragRail) w->mouseX = w->mouseY = -1;
            Set_Invalidate(*w);
            return 0;
        }
        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                PCWSTR cur = w->dragRail || w->hover.kind == SetHit::RailEdge ? IDC_SIZEWE
                             : Set_Clickable(w->hover)                         ? IDC_HAND
                                                                               : IDC_ARROW;
                SetCursor(LoadCursorW(nullptr, cur));
                return TRUE;
            }
            break;
        case WM_LBUTTONDOWN: {
            if (w->saving) return 0;
            if (!w->render) SetFocus(hwnd);
            w->kbd = false;
            float x = dipX(lParam), y = dipY(lParam);
            SetHit h = Set_Hit(*w, x, y);
            if (w->openSel >= 0) {  // The popup eats the click, whatever it hits.
                if (h.kind == SetHit::Drop) Set_Choose(*w, h.a, h.b);
                else Set_ClosePopup(*w);
                return 0;
            }
            Set_CommitEdit(*w, true);
            if (w->recording >= 0 && !(h.kind == SetHit::Control && h.a == w->recording)) Set_StopRecording(*w);
            w->hover = h;
            switch (h.kind) {
                case SetHit::Rail:
                    Set_GoPage(*w, h.a);
                    break;
                case SetHit::RailToggle:
                    Set_ToggleRail(*w);
                    break;
                case SetHit::RailEdge:
                    w->dragRail = true;
                    w->dragOff = x - Set_RailDock(*w).right;
                    SetCapture(hwnd);
                    Set_Invalidate(*w);
                    break;
                case SetHit::Footer:
                    Set_FooterAction(*w, h.a);
                    return 0;  // The window may be gone.
                case SetHit::Control:
                case SetHit::Clear:
                case SetHit::Chip:
                    w->focus = h.a;
                    Set_ActivateField(*w, h.a, &h);
                    break;
                case SetHit::Slider:
                    w->focus = h.a;
                    w->dragSlider = h.a;
                    SetCapture(hwnd);
                    Set_SliderTo(*w, h.a, x);
                    break;
                case SetHit::Scroll: {
                    w->dragScroll = true;
                    w->dragOff = y - Set_ScrollThumb(*w).top;
                    SetCapture(hwnd);
                    Set_Invalidate(*w);
                    break;
                }
                case SetHit::Link:
                    Set_OpenLink(w->items[h.a].url.c_str());
                    break;
                case SetHit::Caption:
                    w->capDown = h.a + 1;
                    SetCapture(hwnd);
                    Set_Invalidate(*w);
                    break;
                default:
                    break;
            }
            return 0;
        }
        case WM_LBUTTONUP:
        case WM_CAPTURECHANGED:
            if (w->capDown) {
                int b = w->capDown - 1;
                w->capDown = 0;
                if (msg == WM_LBUTTONUP) {
                    if (GetCapture() == hwnd) ReleaseCapture();
                    SetHit h = Set_Hit(*w, dipX(lParam), dipY(lParam));
                    if (h.kind == SetHit::Caption && h.a == b) {
                        if (b == 0) ShowWindow(hwnd, SW_MINIMIZE);
                        else if (b == 1) ShowWindow(hwnd, IsZoomed(hwnd) ? SW_RESTORE : SW_MAXIMIZE);
                        else PostMessageW(hwnd, WM_CLOSE, 0, 0);
                    }
                }
                Set_Invalidate(*w);
                return 0;
            }
            if (w->dragSlider >= 0 || w->dragScroll || w->dragRail) {
                w->dragSlider = -1;
                w->dragScroll = false;
                w->dragRail = false;
                if (msg == WM_LBUTTONUP && GetCapture() == hwnd) ReleaseCapture();
                Set_Invalidate(*w);
            }
            return 0;
        case WM_MOUSEWHEEL: {
            if (w->openSel >= 0 && Set_In(w->dropRect, w->mouseX, w->mouseY)) {
                int n = (int)w->fields[w->openSel].options.size(), vis = Set_DropVisible(*w);
                int step = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -3 : 3;
                w->dropTop = std::clamp(w->dropTop + step, 0, std::max(0, n - vis));
                SetHit h = Set_Hit(*w, w->mouseX, w->mouseY);
                w->hover = h;
                w->dropHover = h.kind == SetHit::Drop ? h.b : w->dropHover;
                Set_Invalidate(*w);
                return 0;
            }
            Set_CommitEdit(*w, true);
            Set_ClosePopup(*w);
            float delta = (float)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            float maxScroll = std::max(0.0f, w->contentH - Set_ViewH(*w));
            w->scrollTarget = std::clamp(w->scrollTarget - delta * 72, 0.0f, maxScroll);
            if (!Set_Anim(*w)) w->scrollY = w->scrollTarget;
            Set_Invalidate(*w);
            return 0;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (Set_OnKey(*w, (UINT)wParam)) return 0;
            // Without a system menu (glass title bar) Windows no longer turns Alt+F4 into a close.
            if (msg == WM_SYSKEYDOWN && wParam == VK_F4 && w->recording < 0) {
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
                return 0;
            }
            break;
        case WM_CHAR:
        case WM_SYSCHAR:
            if (w->recording >= 0) return 0;
            break;
        case WM_KILLFOCUS:
            if (w->recording >= 0 && (HWND)wParam != w->edit) Set_StopRecording(*w);
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY: {
            Set_StopRecording(*w);
            if (w->timer) KillTimer(hwnd, kSetTimerId);
            if (!w->render && w->normalRect.right > w->normalRect.left) {
                const RECT& r = w->normalRect;
                SetState(StateKey(L"Settings", L"X").c_str(), r.left);
                SetState(StateKey(L"Settings", L"Y").c_str(), r.top);
                SetState(StateKey(L"Settings", L"CX").c_str(), r.right - r.left);
                SetState(StateKey(L"Settings", L"CY").c_str(), r.bottom - r.top);
            }
            if (!w->render) {
                SetState(StateKey(L"Settings", L"RailW").c_str(), (int)w->railW);
                SetState(StateKey(L"Settings", L"RailCollapsed").c_str(), w->railCollapsed ? 1 : 0);
            }
            return 0;
        }
        case WM_NCDESTROY: {
            if (w->editFont) DeleteObject(w->editFont);
            if (w->editBrush) DeleteObject(w->editBrush);
            if (w->iconSmall) DestroyIcon(w->iconSmall);
            if (w->iconBig) DestroyIcon(w->iconBig);
            if (w->owner) DestroyWindow(w->owner);
            w->destroyed = true;
            w->hwnd = nullptr;
            w->edit = nullptr;
            w->rt.reset();
            g_settingsWnd = nullptr; PostQuitMessage(0);
            g_setWin.reset();
            Set_ReleaseFactories();
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Blue to mauve rounded square with a clipboard glyph, px pixels square.
static HICON Set_MakeIcon(const std::wstring& iconFamily, int px) {
    if (px <= 0 || !Set_EnsureFactories()) return nullptr;
    SetCom<IWICBitmap> bmp;
    SetCom<ID2D1RenderTarget> rt;
    if (FAILED(g_setWic->CreateBitmap(px, px, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, bmp.put())))
        return nullptr;
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
    if (FAILED(g_setD2d->CreateWicBitmapRenderTarget(bmp.get(), props, rt.put()))) return nullptr;
    float s = (float)px;
    SetWindow dummy;
    SetPaint p(dummy, rt.get());
    auto glyph = Set_Fmt(iconFamily, s * 0.6f, 400, false, DWRITE_TEXT_ALIGNMENT_CENTER, true);
    rt->BeginDraw();
    rt->Clear(D2D1::ColorF(0, 0));
    Set_DrawMark(p, glyph.get(), {0, 0, s, s}, 1);
    if (FAILED(rt->EndDraw())) return nullptr;
    std::vector<BYTE> pix((size_t)px * px * 4);
    if (FAILED(bmp->CopyPixels(nullptr, px * 4, (UINT)pix.size(), pix.data()))) return nullptr;
    for (size_t i = 0; i < pix.size(); i += 4) {  // Icons want straight alpha.
        BYTE a = pix[i + 3];
        if (a && a < 255)
            for (int c = 0; c < 3; c++) pix[i + c] = (BYTE)std::min(255, pix[i + c] * 255 / a);
    }
    BITMAPV5HEADER bi{};
    bi.bV5Size = sizeof(bi);
    bi.bV5Width = px;
    bi.bV5Height = -px;
    bi.bV5Planes = 1;
    bi.bV5BitCount = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask = 0x00FF0000;
    bi.bV5GreenMask = 0x0000FF00;
    bi.bV5BlueMask = 0x000000FF;
    bi.bV5AlphaMask = 0xFF000000;
    void* bits = nullptr;
    HDC dc = GetDC(nullptr);
    HBITMAP color = CreateDIBSection(dc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, dc);
    if (!color || !bits) return nullptr;
    memcpy(bits, pix.data(), pix.size());
    std::vector<BYTE> maskBits((size_t)((px + 15) / 16) * 2 * px, 0);
    HBITMAP mask = CreateBitmap(px, px, 1, 1, maskBits.data());
    ICONINFO ii{TRUE, 0, 0, mask, color};
    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(mask);
    DeleteObject(color);
    return icon;
}

// The class belongs to this DLL and goes away with the settings thread. Registered under
// Explorer's handle it outlived a mod update, and the next window ran the unloaded code.
static void Set_RegisterClass() {
    UnregisterClassW(kSettingsClass, GetModuleHandleW(nullptr));  // left by builds before 2.3.0
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = Set_WndProc;
    wc.hInstance = (HINSTANCE)&__ImageBase;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kSettingsClass;
    RegisterClassExW(&wc);
}

// Builds the window, hidden. render: an off-screen copy that is never shown (tests and PNG output).
static bool Set_Create(bool render) {
    if (!Set_EnsureFactories()) return false;
    Set_RegisterClass();
    auto w = std::make_shared<SetWindow>();
    w->render = render;
    w->editBrush = CreateSolidBrush(RefFromRgb(kSetBoxFill));
    g_setWin = w;
    if (render) {
        // An owned window gets no taskbar button.
        w->owner = CreateWindowExW(WS_EX_TOOLWINDOW, L"STATIC", L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                   GetModuleHandleW(nullptr), nullptr);
    }
    const int offX = GetSystemMetrics(SM_XVIRTUALSCREEN) - 6000, offY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    // The built-in title bar until Set_ApplyFrame reads the switches.
    kSetCapH = 40.0f;
    const DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
    HWND hwnd = CreateWindowExW(0, kSettingsClass, Tr(L"RYJS Launchpad settings"), style,
                                render ? offX : CW_USEDEFAULT, render ? offY : CW_USEDEFAULT, 800, 600, w->owner,
                                nullptr, (HINSTANCE)&__ImageBase, nullptr);
    if (!hwnd) {
        g_setWin.reset();
        return false;
    }
    w->hwnd = hwnd;
    w->dpi = g_setForceDpi ? g_setForceDpi : GetDpiForWindow(hwnd);
    ApplyDarkWindow(hwnd);
    COLORREF caption = RefFromRgb(mocha::crust), captionText = RefFromRgb(mocha::text);
    DwmSetWindowAttribute(hwnd, 35 /* DWMWA_CAPTION_COLOR */, &caption, sizeof(caption));
    DwmSetWindowAttribute(hwnd, 36 /* DWMWA_TEXT_COLOR */, &captionText, sizeof(captionText));
    Set_MakeFormats(*w);
    Set_LoadFields(*w);
    if (!render) {
        w->railW = std::clamp((float)GetState(StateKey(L"Settings", L"RailW").c_str(), (int)kSetRailDef), kSetRailMin, kSetRailMax);
        w->railCollapsed = GetState(StateKey(L"Settings", L"RailCollapsed").c_str(), 0) != 0;
        w->railCur = w->railCollapsed ? kSetRailMini : w->railW;
    }
    if (!render) {
        w->iconSmall = Set_MakeIcon(w->iconFamily, GetSystemMetricsForDpi(SM_CXSMICON, w->dpi));
        w->iconBig = Set_MakeIcon(w->iconFamily, GetSystemMetricsForDpi(SM_CXICON, w->dpi));
        if (w->iconSmall) SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)w->iconSmall);
        if (w->iconBig) SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)w->iconBig);
        Set_ApplyFrame(*w);  // Title bar and glass; also drops the system title bar the first WM_NCCALCSIZE kept.
    }

    auto outer = [&](float cwDip, float chDip, SIZE& size) {
        RECT r{0, 0, MulDiv((int)cwDip, w->dpi, 96), MulDiv((int)chDip, w->dpi, 96)};
        AdjustWindowRectExForDpi(&r, style, FALSE, 0, w->dpi);
        size = {r.right - r.left, r.bottom - r.top};
    };
    SIZE size;
    if (render) {
        w->cw = 980;
        w->ch = 700;
        outer(w->cw, w->ch, size);
        SetWindowPos(hwnd, nullptr, offX, offY, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE);
        Set_BuildPage(*w);
        return true;
    }
    outer(980, 720, size);
    POINT cursor;
    GetCursorPos(&cursor);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST), &mi);
    const RECT& work = mi.rcWork;
    int width = std::min((int)size.cx, (int)(work.right - work.left) * 9 / 10);
    int height = std::min((int)size.cy, (int)(work.bottom - work.top) * 9 / 10);
    RECT r{(work.left + work.right - width) / 2, (work.top + work.bottom - height) / 2, 0, 0};
    r.right = r.left + width;
    r.bottom = r.top + height;
    int cx = GetState(StateKey(L"Settings", L"CX").c_str(), 0);
    int cy = GetState(StateKey(L"Settings", L"CY").c_str(), 0);
    if (cx > 0 && cy > 0) {
        RECT saved{GetState(StateKey(L"Settings", L"X").c_str(), 0), GetState(StateKey(L"Settings", L"Y").c_str(), 0),
                   0, 0};
        saved.right = saved.left + cx;
        saved.bottom = saved.top + cy;
        if (MonitorFromRect(&saved, MONITOR_DEFAULTTONULL)) r = saved;
    }
    SetWindowPos(hwnd, nullptr, r.left, r.top, r.right - r.left, r.bottom - r.top, SWP_NOZORDER | SWP_NOACTIVATE);
    // The monitor it landed on may have a different scale.
    UINT dpi = GetDpiForWindow(hwnd);
    if (!g_setForceDpi && dpi != w->dpi) w->dpi = dpi;
    Set_Relayout(*w);
    w->pageT = Set_Anim(*w) ? 0.0f : 1.0f;
    w->timer = SetTimer(hwnd, kSetTimerId, 15, nullptr) != 0;
    return true;
}

void Settings_Open() {
    if (g_setWin && g_setWin->hwnd && !g_setWin->render) {
        HWND hwnd = g_setWin->hwnd;
        if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
        SetForegroundWindow(hwnd);
        return;
    }
    if (g_setWin) Settings_Close();
    if (!Set_Create(false)) return;
    HWND hwnd = g_setWin->hwnd;
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
}

void Settings_Close() {
    if (g_setWin && g_setWin->hwnd) DestroyWindow(g_setWin->hwnd);
    g_setWin.reset();
}

bool Settings_IsOpen() { return g_setWin && g_setWin->hwnd && !g_setWin->render; }


// Runs on the settings thread, after its message loop or its early exit.
void SettingsThreadTeardown() {
    Settings_Close();
    UnregisterClassW(kSettingsClass, (HINSTANCE)&__ImageBase);
    Set_ReleaseFactories();
    CoUninitialize();
}

DWORD WINAPI SettingsThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    Settings_Open();
    // CloseSettingsWindow may have run while the window was being built; the
    // stop flag is what tells this thread to leave instead of entering the
    // loop, where the posted WM_QUIT would already have been consumed.
    if (g_settingsStop) {
        SettingsThreadTeardown();
        return 0;
    }
    if (g_setWin) g_settingsWnd = g_setWin->hwnd;
    MSG msg;
    while (g_settingsWnd && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    SettingsThreadTeardown();
    return 0;
}

void OpenSettingsWindow() {
    if (g_fullImportBusy) return;
    if (HWND hwnd = g_settingsWnd) {
        PostMessageW(hwnd, WM_APP_SETTINGS_ACTIVATE, 0, 0);
        return;
    }
    if (g_settingsThread) {
        // A previous window was closed: reap that thread before starting one.
        if (WaitForSingleObject(g_settingsThread, 0) == WAIT_TIMEOUT) return;
        CloseHandle(g_settingsThread);
        g_settingsThread = nullptr;
        g_settingsThreadId = 0;
    }
    g_settingsStop = false;
    g_settingsThread = CreateThread(nullptr, 0, SettingsThread, nullptr, 0,
                                    &g_settingsThreadId);
}

// The settings window lives on its own thread, and the DLL must not unload
// while either the window or the thread is still alive. The thread is asked
// to quit through its message queue and then joined without a bound: the
// thread's own exit path is what guarantees the join returns, so waiting for
// it is the only correct order. g_settingsStop covers the window that has not
// finished building yet.
void CloseSettingsWindow() {
    g_settingsStop = true;
    if (HWND hwnd = g_settingsWnd) PostMessageW(hwnd, WM_CLOSE, 0, 0);
    if (g_settingsThread) {
        if (g_settingsThreadId) {
            // The queue may not exist yet while the window is being built, so
            // post repeatedly until the thread is gone.
            do {
                PostThreadMessageW(g_settingsThreadId, WM_QUIT, 0, 0);
            } while (WaitForSingleObject(g_settingsThread, 100) == WAIT_TIMEOUT);
        } else {
            WaitForSingleObject(g_settingsThread, INFINITE);
        }
        CloseHandle(g_settingsThread);
        g_settingsThread = nullptr;
        g_settingsThreadId = 0;
    }
    g_settingsWnd = nullptr;
}

void EnsureVisible(int index) {
    if (index < 0) {
        return;
    }
    if (g_openFolder) {
        FolderLayout F = GetFolderLayout();
        int row = index / F.columns;
        g_folderRow = std::clamp(g_folderRow, row - F.visibleRows + 1, row);
        return;
    }
    const Layout& L = g_layout;
    if (GroupedView() && index < (int)g_groupCells.size()) {
        EnsureFreeGroupVisible(index);
        auto cell = g_groupCells[index];
        if (g_settings.pages) {
            int page=std::max(0,(int)std::floor(cell.icon.left/L.width));
            float target=page*(float)L.width;
            if(target!=g_scrollTarget)g_groupPagePan=0;
            g_scrollTarget=target;
            if(g_squareGridColumns) {
                if(cell.icon.top-g_groupPagePan<L.viewTop)g_groupPagePan=cell.icon.top-L.viewTop;
                if(cell.label.bottom-g_groupPagePan>L.viewBottom)g_groupPagePan=cell.label.bottom-L.viewBottom;
                g_groupPagePan=std::clamp(g_groupPagePan,0.f,MaxPagePan(page));
            }
        }
        else {
            if (cell.icon.top - g_scrollTarget < L.viewTop) g_scrollTarget = cell.icon.top - L.viewTop;
            if (cell.label.bottom - g_scrollTarget > L.viewBottom) g_scrollTarget = cell.label.bottom - L.viewBottom;
            g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll());
        }
        return;
    }
    if (g_settings.pages) {
        float target=(index/L.perPage)*(float)L.width;
        if(target!=g_scrollTarget)g_groupPagePan=0;
        g_scrollTarget=target;
        auto cell=MakeSquareCell(0,L.viewTop+((index%L.perPage)/L.columns)*L.cellHeight,L.cellWidth);
        if(cell.icon.top-g_groupPagePan<L.viewTop)g_groupPagePan=cell.icon.top-L.viewTop;
        if(cell.label.bottom-g_groupPagePan>L.viewBottom)g_groupPagePan=cell.label.bottom-L.viewBottom;
        g_groupPagePan=std::clamp(g_groupPagePan,0.f,MaxPagePan(index/L.perPage));
        return;
    }
    int row = index / L.columns;
    int firstRow = (int)std::lround(g_scrollTarget / L.cellHeight);
    if (row < firstRow) {
        g_scrollTarget = row * L.cellHeight;
    } else if (row >= firstRow + L.visibleRows) {
        g_scrollTarget = (row - L.visibleRows + 1) * L.cellHeight;
    }
    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll());
}

// With pages, any number of rows flips one page that way.
void ScrollRows(int rows) {
    if (g_settings.pages) {
        float width = (float)g_layout.width;
        float target =
            (std::lround(g_scrollTarget / width) + (rows > 0 ? 1 : -1)) * width;
        float previous=g_scrollTarget;
        g_scrollTarget = std::clamp(target, 0.0f, MaxScroll());
        if(previous!=g_scrollTarget)g_groupPagePan=0;
        StartFrames();
        return;
    }
    float cell = g_layout.cellHeight;
    float target = (std::lround(g_scrollTarget / cell) + rows) * cell;
    g_scrollTarget = std::clamp(target, 0.0f, MaxScroll());
    StartFrames();
}

////////////////////////////////////////////////////////////////////////////////
// Folders, dragging and the right-click menus

// After the arrangement changed.
void Refresh() {
    UpdateFilter();
    ComputeLayout();
    Render();
}

int IndexOf(const Tiles& tiles, const Tile& tile) {
    for (int i = 0; i < (int)tiles.size(); i++) {
        if (tiles[i].app == tile.app && tiles[i].folder == tile.folder) {
            return i;
        }
    }
    return -1;
}

bool GroupsOverlap(const GroupPosition& a,const GroupPosition& b,float gap=0) {
    return a.x<b.x+b.w+gap-.01f&&b.x<a.x+a.w+gap-.01f&&
        a.y<b.y+b.h+gap-.01f&&b.y<a.y+a.h+gap-.01f;
}
bool SameGroupBounds(const GroupPosition& a,const GroupPosition& b) {
    return std::abs(a.x-b.x)<.01f&&std::abs(a.y-b.y)<.01f&&
        std::abs(a.w-b.w)<.01f&&std::abs(a.h-b.h)<.01f;
}
bool g_groupClosePrompt=false;
int g_groupEditButton=-1;
void ResetGroupGesture(bool clearTransitions) {
    g_groupGesture={};
    if(clearTransitions)g_groupTransitions.clear();
    g_groupEditButton=-1;
    if(g_wnd&&GetCapture()==g_wnd)ReleaseCapture();
}
void EndGroupEdit() {
    g_groupResizeHover.reset();g_groupResizeGlow=0;
    g_groupEditMode=false;g_groupEditDirty=false;
    g_groupEditSnapshot.clear();g_groupEditRows.clear();
    g_pressed=-1;g_pressedEmpty=false;Refresh();
}
void CancelGroupEdit() {
    if(!g_groupEditMode)return;
    ResetGroupGesture(true);
    g_groupPositions=std::move(g_groupEditSnapshot);g_groupRows=std::move(g_groupEditRows);
    g_freeGroupLayout=g_groupEditWasFree;g_wholeCellLayout=g_groupEditWasWholeCell;g_squareGridColumns=g_groupEditPreviousColumns;
    EndGroupEdit();
}
bool FinishGroupEdit() {
    if(!g_groupEditMode)return true;
    ResetGroupGesture(true);
    if(g_groupEditDirty&&!SaveGroupPositions(true)) {
        bool modal=std::exchange(g_modal,true);
        MessageBoxW(g_wnd,Tr(L"Could not save group geometry."),Tr(L"Edit group layout"),MB_OK|MB_ICONWARNING);
        g_modal=modal;
        Refresh();return false;
    }
    EndGroupEdit();return true;
}
void AlignGroupsToMainGrid();
void ToggleGroupEdit() {
    if(g_groupEditMode){FinishGroupEdit();return;}
    if(g_groupPositionsReadOnly||g_layoutReadOnly||g_dataReadOnly||!GroupedView())return;
    g_groupEditSnapshot=g_groupPositions;g_groupEditRows=g_groupRows;
    g_groupEditPreviousColumns=g_squareGridColumns;
    g_groupEditWasWholeCell=g_wholeCellLayout;g_groupEditWasFree=g_freeGroupLayout;g_groupEditDirty=false;
    ClearAppSelection();
    g_groupEditMode=true;
    AlignGroupsToMainGrid();
    g_pressed=-1;g_pressedEmpty=false;Refresh();
}
bool ConfirmGroupEditClose() {
    if(g_groupClosePrompt)return false;
    if(!g_groupEditMode||!g_groupEditDirty)return true;
    ResetGroupGesture(true);Render();
    const TASKDIALOG_BUTTON buttons[]{
        {IDYES,Tr(L"Save layout")},{IDNO,Tr(L"Discard changes")},{IDCANCEL,Tr(L"Continue editing")}
    };
    TASKDIALOGCONFIG dialog{};
    dialog.cbSize=sizeof(dialog);dialog.hwndParent=g_wnd;
    dialog.dwFlags=TDF_ALLOW_DIALOG_CANCELLATION|TDF_POSITION_RELATIVE_TO_WINDOW;
    dialog.pszWindowTitle=Tr(L"Edit group layout");
    dialog.pszMainInstruction=Tr(L"Save changes to the group layout?");
    dialog.pButtons=buttons;dialog.cButtons=ARRAYSIZE(buttons);dialog.nDefaultButton=IDCANCEL;
    bool modal=std::exchange(g_modal,true);
    g_groupClosePrompt=true;
    int choice=IDCANCEL;
    // Resolve dynamically so Windhawk's single-file module has no new linker requirement.
    using TaskDialogProc=decltype(&TaskDialogIndirect);
    bool shown=false;
    if(HMODULE controls=LoadLibraryW(L"comctl32.dll")) {
        auto show=reinterpret_cast<TaskDialogProc>(GetProcAddress(controls,"TaskDialogIndirect"));
        shown=show&&SUCCEEDED(show(&dialog,&choice,nullptr,nullptr));
        FreeLibrary(controls);
    }
    if(!shown)choice=MessageBoxW(g_wnd,Tr(L"Save layout? Yes saves, No discards, Cancel continues editing."),
        dialog.pszWindowTitle,MB_YESNOCANCEL|MB_DEFBUTTON3|MB_ICONQUESTION);
    if(choice==IDNO)CancelGroupEdit();
    bool close=choice==IDNO||(choice==IDYES&&FinishGroupEdit());
    g_groupClosePrompt=false;g_modal=modal;
    return close;
}
int GroupResizeAt(int x,int y) {
    if(!g_groupEditMode||!GroupedView()||y<g_layout.viewTop||y>=g_layout.viewBottom)return -1;
    for(int i=(int)g_groupGeometry.size()-1;i>=0;--i)
        if(GroupContains(GroupResizeHit(VisualGroupRect(g_groupGeometry[i])),(float)x,(float)y))return i;
    return -1;
}
void UpdateGroupResizeHover(int x,int y) {
    int index=GroupResizeAt(x,y);
    auto group=index<0?nullptr:g_groupGeometry[index].group;
    if(group==g_groupResizeHover)return;
    g_groupResizeHover=group;g_groupResizeGlow=0;StartFrames();
}
std::array<D2D1_RECT_F,3> GroupEditButtons() {
    float s=g_layout.scale;
    float width=std::min(152*s,(g_layout.width-28*s)/3);
    float gap=6*s,left=(g_layout.width-3*width-2*gap)/2;
    float bottom=g_layout.height-12*s;
    if(!g_layout.powerItems.empty())bottom=g_layout.powerBar.top-6*s;
    std::array<D2D1_RECT_F,3> buttons{};
    for(auto& button:buttons) {
        button={left,bottom-36*s,left+width,bottom};
        left+=width+gap;
    }
    return buttons;
}
int GroupEditButtonAt(int x,int y) {
    if(!g_groupEditMode)return -1;
    auto buttons=GroupEditButtons();
    for(int i=0;i<(int)buttons.size();++i)
        if(GroupContains(buttons[i],(float)x,(float)y))return i;
    return -1;
}
void DrawGroupEditBar() {
    if(!g_groupEditMode)return;
    auto buttons=GroupEditButtons();
    PCWSTR labels[]{Tr(L"Cancel"),Tr(L"Equalize group spacing"),Tr(L"Finish and save")};
    for(int i=0;i<(int)buttons.size();++i) {
        auto rect=buttons[i];
        float highlight=g_groupEditButton==i?.28f:i==2?.18f:.10f;
        Fill(rect,D2D1::ColorF(0xFFFFFF,highlight),8*g_layout.scale);
        PCWSTR text=labels[i];
        g_brush->SetColor(D2D1::ColorF(0xFFFFFF));
        g_target->DrawText(text,(UINT32)wcslen(text),g_tipFormat.Get(),rect,g_brush.Get(),D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
}

int GroupGridIndex(float value,float step) {
    return (int)std::clamp(std::round(value/step),0.f,1000000.f);
}
GroupPosition MoveGroupToGrid(GroupPosition p,int x,int y) {
    p.snapped=true;p.gridX=x;p.gridY=y;
    return EffectiveGroupPosition(p);
}
int GroupXIndex(float x,int page) {
    float local=x-page*g_layout.width/g_layout.scale-GridMargin()/g_layout.scale;
    return page*2*g_squareGridColumns+GroupGridIndex(local,GroupStepX());
}
GroupPosition EditorGroupPosition(const ExpandedGroupGeometry& geometry) {
    GroupPosition position;
    position.style=5;
    position.gridW=geometry.columns*2;position.gridH=geometry.visibleRows*4+2;
    auto area=PositionFromArea(geometry.area);
    int page=g_settings.pages?std::max(0,(int)std::floor(geometry.area.left/g_layout.width)):0;
    return MoveGroupToGrid(position,GroupXIndex(area.x,page),GroupGridIndex(area.y,GroupStepY()));
}
bool GroupAxisSharesSpan(float a,float aSize,float b,float bSize) {
    return a<b+bSize-.01f&&b<a+aSize-.01f;
}
struct GroupEditBounds {
    float width,height;
    int pages;
    float margin=0;
    float PageLeft(float x) const {return g_settings.pages?std::floor(x/width)*width:0;}
    bool Fits(const GroupPosition& p) const {
        float left=PageLeft(p.x+.001f);
        return p.x>=left+margin-.01f&&p.y>=0&&left<pages*width&&p.x+p.w<=left+width-margin+.01f&&p.y+p.h<=height+.01f;
    }
};
GroupEditBounds GroupGestureBounds() {
    float view=(g_layout.viewBottom-g_layout.viewTop)/g_layout.scale;
    float height=g_settings.pages?std::max(view,g_layout.contentHeight/g_layout.scale):
        view+MaxScroll()/g_layout.scale;
    return {g_layout.width/g_layout.scale,height,
        g_settings.pages?g_layout.pages:1,GridMargin()/g_layout.scale};
}

void AlignGroupsToMainGrid() {
    int columns=DesiredGridColumns();
    if(g_squareGridColumns==columns&&std::all_of(g_groupPositions.begin(),g_groupPositions.end(),
        [](const auto& entry){return entry.second.SquareCells();}))return;
    UpdateFilter();ComputeLayout();
    auto oldGeometry=g_groupGeometry;
    g_groupPositions.clear();
    g_squareGridColumns=columns;
    g_wholeCellLayout=true;
    ComputeLayout();
    auto bounds=GroupGestureBounds();
    const float sx=GroupStepX(),sy=GroupStepY();
    const int steps=2*columns;
    std::map<std::shared_ptr<Folder>,int> segments;
    for(const auto& geometry:oldGeometry)++segments[geometry.group];

    std::vector<GroupPosition> placed;
    for(const auto& geometry:oldGeometry) {
        if(segments[geometry.group]!=1)continue;
        auto old=PositionFromArea(geometry.area);
        int page=(int)std::floor(bounds.PageLeft(old.x)/bounds.width+.01f);
        int left=page*steps,right=left+steps;
        int bottom=(int)std::floor((bounds.height+.001f)/sy);
        GroupPosition grid;
        grid.style=5;
        int width=std::min(geometry.columns,columns);
        grid.gridW=width*2;
        int rows=std::max(geometry.visibleRows,(geometry.count+width-1)/width);
        if(g_settings.pages)rows=std::min(rows,std::max(1,(bottom-2)/4));
        grid.gridH=rows*4+2;
        int x=std::clamp(GroupXIndex(old.x,page),left,right-grid.gridW);
        int y=GroupGridIndex(old.y,sy);
        if(g_settings.pages)y=std::min(y,std::max(0,bottom-grid.gridH));
        auto position=MoveGroupToGrid(grid,x,y);
        auto overlaps=[&](const GroupPosition& candidate) {
            return std::any_of(placed.begin(),placed.end(),[&](const auto& other){return GroupsOverlap(candidate,other);});
        };
        if(overlaps(position)) {
            float best=std::numeric_limits<float>::infinity();
            std::vector<int> xs{x,left},ys{y,0};
            for(const auto& other:placed) {
                xs.push_back(other.gridX-grid.gridW);
                xs.push_back(other.gridX+other.gridW);
                ys.push_back(other.gridY-grid.gridH);
                ys.push_back(other.gridY+other.gridH);
            }
            for(int cx:xs)for(int cy:ys) {
                if(cx<left||cx>right-grid.gridW||cy<0||
                    (g_settings.pages&&cy>std::max(0,bottom-grid.gridH)))continue;
                float distance=std::hypot((cx-x)*sx,(cy-y)*sy);
                if(distance>=best)continue;
                auto candidate=MoveGroupToGrid(grid,cx,cy);
                if(!overlaps(candidate)){position=candidate;best=distance;}
            }
            if(!std::isfinite(best)) {
                int nextPage=page+1;
                do {position=MoveGroupToGrid(grid,nextPage++*steps,0);} while(overlaps(position));
            }
        }
        placed.push_back(position);
        g_groupPositions[geometry.group]=position;
    }
    // Geometry for paged automatic groups is reflowed from their existing app order.
    g_freeGroupLayout=true;
    g_groupEditDirty=true;
}

// Each branch pins the active group and the groups it has already moved.
// Resolving their clearance can bring additional groups into the chain.
struct GroupCollisionSearch {
    const std::vector<GroupEditNode>& baseline;
    GroupEditBounds bounds;
    float dx,dy;
    int budget=12000;
    double bestDistance=1e30,bestDirection=-1e30;
    std::vector<GroupEditNode> best;

    void Resolve(std::vector<GroupEditNode>& nodes,std::vector<size_t>& pinned,double distance,double direction) {
        if(--budget<0||distance>bestDistance+.0001)return;
        size_t next=nodes.size();
        for(size_t i=0;i<nodes.size();++i) {
            if(std::find(pinned.begin(),pinned.end(),i)!=pinned.end())continue;
            if(std::any_of(pinned.begin(),pinned.end(),[&](size_t j){return GroupsOverlap(nodes[i].position,nodes[j].position);})) {
                if(nodes[i].fixed)return;
                if(next==nodes.size())next=i;
            }
        }
        if(next==nodes.size()) {
            if(distance<bestDistance-.0001||direction>bestDirection) {
                best=nodes;bestDistance=distance;bestDirection=direction;
            }
            return;
        }
        const auto from=baseline[next].position;
        float sx=GroupStepX(),sy=GroupStepY();
        float left=bounds.PageLeft(from.x);
        int page=(int)std::floor(left/bounds.width+.01f);
        int minX=page*2*g_squareGridColumns,maxX=minX+2*g_squareGridColumns-from.gridW;
        int maxY=(int)std::floor((bounds.height-from.h+.001f)/sy);
        // Limit the search to the local neighborhood even for very tall scrolling layouts.
        int centerX=from.gridX,centerY=GroupGridIndex(from.y,sy);
        struct Candidate {int x,y;double distance,direction;};
        std::vector<Candidate> candidates;
        auto add=[&](int gridX,int gridY) {
            float x=GroupXAt(gridX),y=gridY*sy;
            float u=(x-from.x)/sx,v=(y-from.y)/sy;
            double cost=u*u+v*v;
            if(distance+cost>bestDistance+.0001)return;
            auto rect=from;rect.x=x;rect.y=y;
            if(!bounds.Fits(rect)||bounds.PageLeft(x+.001f)!=left)return;
            if(std::any_of(pinned.begin(),pinned.end(),[&](size_t j){return GroupsOverlap(rect,nodes[j].position);}))return;
            candidates.push_back({gridX,gridY,cost,u*dx+v*dy});
        };
        for(int y=std::max(0,centerY-32);y<=std::min(maxY,centerY+32);++y)
            for(int x=std::max(minX,centerX-32);x<=std::min(maxX,centerX+32);++x)
                add(x,y);
        std::sort(candidates.begin(),candidates.end(),[](const auto& a,const auto& b){
            if(a.distance!=b.distance)return a.distance<b.distance;
            if(a.direction!=b.direction)return a.direction>b.direction;
            if(a.y!=b.y)return a.y<b.y;
            return a.x<b.x;
        });
        pinned.push_back(next);
        for(const auto& candidate:candidates) {
            if(budget<=0||distance+candidate.distance>bestDistance+.0001)break;
            nodes[next].position=MoveGroupToGrid(from,candidate.x,candidate.y);
            Resolve(nodes,pinned,distance+candidate.distance,direction+candidate.direction);
        }
        pinned.pop_back();
        nodes[next].position=from;
    }
};
std::vector<GroupEditNode> SolveGroupGesture(const GroupGeometryGesture& gesture,GroupPosition target,
                                            GroupEditBounds bounds,float dx,float dy) {
    if(!bounds.Fits(target))return {};
    auto nodes=gesture.baseline;
    auto active=std::find_if(nodes.begin(),nodes.end(),[&](const auto& node){return node.group==gesture.group;});
    if(active==nodes.end())return {};
    if(SameGroupBounds(active->position,target))return nodes;
    active->position=target;
    std::vector<size_t> pinned{(size_t)(active-nodes.begin())};
    GroupCollisionSearch search{gesture.baseline,bounds,dx,dy};
    search.Resolve(nodes,pinned,0,0);
    return search.best;
}

void CaptureGroupBaseline(GroupGeometryGesture& gesture) {
    std::map<std::shared_ptr<Folder>,int> counts;
    for(const auto& other:g_groupGeometry)++counts[other.group];
    for(const auto& other:g_groupGeometry) {
        if(other.group==gesture.group&&other.start!=gesture.activeStart)continue;
        gesture.baseline.push_back({other.group,EditorGroupPosition(other),
            other.group!=gesture.group&&counts[other.group]>1});
    }
}
bool FreeGroupPointerDown(HWND hwnd,int x,int y) {
    if(!g_groupEditMode||!GroupedView())return false;
    int resizeIndex=GroupResizeAt(x,y);
    int index=resizeIndex>=0?resizeIndex:GroupAtPoint(x,y);
    if(index<0)return y>=g_layout.viewTop&&y<g_layout.viewBottom;
    const auto& geometry=g_groupGeometry[index];
    auto area=VisualGroupRect(geometry);
    bool resize=resizeIndex>=0;
    auto title=area;title.bottom=title.top+geometry.title.bottom-geometry.title.top;
    if(!resize&&!GroupContains(title,(float)x,(float)y))return true;

    auto& gesture=g_groupGesture;
    gesture={};gesture.group=geometry.group;gesture.activeStart=geometry.start;
    gesture.original=EditorGroupPosition(geometry);
    gesture.draft=gesture.continuous=
        VisualGroupPosition(geometry.group,gesture.original);
    gesture.start=gesture.at={x,y};gesture.resize=resize;
    auto pointer=GroupPointerInLayout(x,y);
    const auto& visual=gesture.continuous;
    gesture.grab={pointer.x-visual.x,pointer.y-visual.y};
    gesture.handle={pointer.x-visual.x-visual.w,pointer.y-visual.y-visual.h};
    gesture.lastScroll=g_scroll;gesture.valid=true;
    CaptureGroupBaseline(gesture);
    if(hwnd){SetCapture(hwnd);if(GetCapture()!=hwnd)gesture={};}
    return true;
}
void RetargetGroupTransitions(const GroupGeometryGesture& gesture) {
    const auto now=Now();
    for(size_t i=0;i<gesture.baseline.size();++i) {
        const auto& node=gesture.baseline[i];
        if(node.group==gesture.group||node.fixed)continue;
        const auto& destination=gesture.valid?gesture.solution[i].position:node.position;
        auto t=g_groupTransitions.find(node.group);
        if(t!=g_groupTransitions.end()&&SameGroupBounds(t->second.to,destination))continue;
        auto visual=VisualGroupPosition(node.group,node.position);
        if(!SameGroupBounds(visual,destination))
            g_groupTransitions[node.group]={visual,destination,now};
        else g_groupTransitions.erase(node.group);
    }
}
bool FreeGroupPointerMove(int x,int y) {
    auto& gesture=g_groupGesture;if(!gesture.group)return false;
    if(!gesture.moved&&x==gesture.start.x&&y==gesture.start.y)return true;
    gesture.moved=true;gesture.at={x,y};gesture.lastScroll=g_scroll;
    float inset=26*g_layout.scale;
    int edge=g_settings.pages?(x<inset?-1:x>g_layout.width-inset?1:0):
        (y<g_layout.viewTop+inset?-1:y>g_layout.viewBottom-inset?1:0);
    if(edge!=gesture.edge){gesture.edge=edge;gesture.edgeSince=Now();}
    auto pointer=GroupPointerInLayout(x,y);
    auto raw=gesture.original,p=gesture.original;
    auto bounds=GroupGestureBounds();
    float sx=GroupStepX(),sy=GroupStepY();
    int steps=2*g_squareGridColumns;
    if(gesture.resize) {
        raw.w=std::max(2*sx,pointer.x-gesture.handle.x-raw.x);
        raw.h=std::max(6*sy,pointer.y-gesture.handle.y-raw.y);
        if(!g_settings.pages)bounds.height=std::max(bounds.height,raw.y+raw.h+2*sy);
        int maxColumns=(steps-p.gridX%steps)/2;
        int maxRows=std::max(1,(int)std::floor((bounds.height-p.y-2*sy+.001f)/(4*sy)));
        p.gridW=2*std::clamp(GroupGridIndex(raw.w,2*sx),1,std::max(1,maxColumns));
        p.gridH=2+4*std::clamp(GroupGridIndex(raw.h-2*sy,4*sy),1,maxRows);
        p=EffectiveGroupPosition(p);
    } else {
        raw.x=pointer.x-gesture.grab.x;raw.y=pointer.y-gesture.grab.y;
        float pageLeft=bounds.PageLeft(std::clamp(raw.x,0.f,bounds.width*bounds.pages-.01f));
        int page=(int)std::floor(pageLeft/bounds.width+.01f);
        int first=page*steps,last=first+steps-p.gridW;
        int lastY=std::max(0,(int)std::floor((bounds.height-p.h+.001f)/sy));
        p=MoveGroupToGrid(p,std::clamp(GroupXIndex(raw.x,page),first,std::max(first,last)),
                          std::min(GroupGridIndex(raw.y,sy),lastY));
    }
    gesture.continuous=raw;
    if(!gesture.solved||!SameGroupBounds(p,gesture.draft)) {
        float dx=(gesture.resize?p.w-gesture.original.w:p.x-gesture.original.x)/sx;
        float dy=(gesture.resize?p.h-gesture.original.h:p.y-gesture.original.y)/sy;
        // Group spacing is an arrangement preference. Only overlapping cells
        // block a drop or push a neighbor during a pointer gesture.
        gesture.solution=SolveGroupGesture(gesture,p,bounds,dx,dy);
        if(gesture.solution.empty()&&!g_settings.pages) {
            // A packed scrolling layout can grow to make room for a neighbor.
            float extra=0;
            for(const auto& node:gesture.baseline)extra=std::max(extra,node.position.h);
            bounds.height+=extra;
            gesture.solution=SolveGroupGesture(gesture,p,bounds,dx,dy);
        }
        gesture.valid=!gesture.solution.empty();
        gesture.draft=p;gesture.solved=true;
    }
    StartFrames();
    return true;
}
bool TickGroupGesture() {
    auto& gesture=g_groupGesture;
    bool canScroll=gesture.edge<0?g_scrollTarget>0:gesture.edge>0&&g_scrollTarget<MaxScroll();
    if(canScroll&&Now()-gesture.edgeSince>.34) {
        gesture.edgeSince=Now();ScrollRows(gesture.edge);
    }
    if(g_scroll!=gesture.lastScroll)FreeGroupPointerMove(gesture.at.x,gesture.at.y);
    return canScroll||g_scroll!=g_scrollTarget;
}
void ApplyGroupSolution(const GroupGeometryGesture& gesture) {
    for(size_t i=0;i<gesture.solution.size();++i) {
        const auto& node=gesture.solution[i];
        if(node.fixed)continue;
        if(!g_groupPositions.count(node.group)||!SameGroupBounds(node.position,gesture.baseline[i].position))
            g_groupPositions[node.group]=node.position;
    }
    g_freeGroupLayout=true;g_groupEditDirty=true;
    ComputeLayout();
}
// Use the closest preceding border in each row or column. Work from the
// original adjacency so adjusting one axis does not change the other axis's neighbors.
std::vector<GroupEditNode> EqualizedGroupPositions(const std::vector<GroupEditNode>& baseline,
                                                   GroupEditBounds bounds,float gap) {
    auto result=baseline;
    for(bool horizontal:{true,false}) {
        auto leadingEdge=[&](const GroupPosition& p){return horizontal?p.x:p.y;};
        auto trailingEdge=[&](const GroupPosition& p){return horizontal?p.x+p.w:p.y+p.h;};
        auto sharesSpan=[&](const GroupPosition& a,const GroupPosition& b){
            return horizontal?GroupAxisSharesSpan(a.y,a.h,b.y,b.h):GroupAxisSharesSpan(a.x,a.w,b.x,b.w);
        };
        std::vector<size_t> order;
        for(size_t i=0;i<baseline.size();++i)order.push_back(i);
        std::stable_sort(order.begin(),order.end(),[&](size_t a,size_t b){
            return leadingEdge(baseline[a].position)<leadingEdge(baseline[b].position);
        });
        for(size_t index:order) {
            const auto& before=baseline[index].position;
            if(baseline[index].fixed)continue;
            float previous=-1,destination=leadingEdge(before);
            for(size_t neighbor:order) {
                const auto& other=baseline[neighbor].position;
                if(leadingEdge(other)>=leadingEdge(before))break;
                if(bounds.PageLeft(before.x)!=bounds.PageLeft(other.x)||!sharesSpan(before,other))continue;
                float edge=trailingEdge(other);
                if(edge>leadingEdge(before)+.01f||edge<previous-.01f)continue;
                float target=trailingEdge(result[neighbor].position)+gap;
                destination=edge>previous+.01f?target:std::max(destination,target);
                previous=std::max(previous,edge);
            }
            auto& after=result[index].position;
            if(std::abs(leadingEdge(after)-destination)>.01f) {
                float step=horizontal?GroupStepX():GroupStepY();
                int page=(int)std::floor(bounds.PageLeft(after.x)/bounds.width+.01f);
                float origin=horizontal?bounds.PageLeft(after.x)+bounds.margin:0;
                int gridIndex=(int)std::ceil((destination-origin-.001f)/step)+(horizontal?page*2*g_squareGridColumns:0);
                after=MoveGroupToGrid(after,horizontal?gridIndex:after.gridX,horizontal?after.gridY:gridIndex);
            }
        }
    }
    if(!g_settings.pages)bounds.height=std::max(bounds.height,1000000.f);
    for(size_t i=0;i<result.size();++i) {
        const auto& p=result[i].position;
        if(!bounds.Fits(p)||bounds.PageLeft(p.x+.001f)!=bounds.PageLeft(baseline[i].position.x+.001f))return {};
        for(size_t j=0;j<i;++j) {
            const auto& q=result[j].position;
            float spacing=bounds.PageLeft(p.x)==bounds.PageLeft(q.x)?gap:0;
            if(GroupsOverlap(p,q,spacing))return {};
        }
    }
    return result;
}
void EqualizeGroupSpacing() {
    GroupGeometryGesture change;
    CaptureGroupBaseline(change);
    if(change.baseline.size()<2)return;
    change.solution=EqualizedGroupPositions(change.baseline,GroupGestureBounds(),g_settings.groupGap);
    if(change.solution.empty()) {
        bool modal=std::exchange(g_modal,true);
        MessageBoxW(g_wnd,Tr(L"Cannot equalize spacing in this arrangement."),Tr(L"Equalize group spacing"),MB_OK|MB_ICONINFORMATION);
        g_modal=modal;
        return;
    }
    for(size_t i=0;i<change.baseline.size();++i)
        if(!SameGroupBounds(change.baseline[i].position,change.solution[i].position)) {
            change.valid=true;
            RetargetGroupTransitions(change);
            ApplyGroupSolution(change);
            StartFrames();Render();
            return;
        }
}
bool FreeGroupPointerUp(HWND hwnd) {
    if(!g_groupEditMode)return false;
    auto gesture=std::exchange(g_groupGesture,{});
    if(hwnd&&GetCapture()==hwnd)ReleaseCapture();
    if(!gesture.group||!gesture.moved)return true;
    bool changed=gesture.valid&&!SameGroupBounds(gesture.draft,gesture.original);
    if(changed)ApplyGroupSolution(gesture);
    if(Animate()) {
        g_groupTransitions[gesture.group]={gesture.continuous,changed?gesture.draft:gesture.original,Now()};
        StartFrames();
    }
    Render();return true;
}

void RestoreGroupGeometry(HWND hwnd) {
    bool modal=std::exchange(g_modal,true);
    int answer=MessageBoxW(hwnd,Tr(L"Restore the previous saved group geometry?"),
        Tr(L"Restore previous group geometry"),MB_OKCANCEL|MB_ICONQUESTION);
    if(answer==IDOK) {
        if(LoadGroupPositions(GroupPositionFile()+L".bak")) {
            LoadTiles();Refresh();
        } else {
            MessageBoxW(hwnd,Tr(L"Could not restore group geometry."),
                Tr(L"Restore previous group geometry"),MB_OK|MB_ICONWARNING);
        }
    }
    g_modal=modal;
}
bool ScrollFreeGroup(HWND hwnd,LPARAM lp,int delta) {
    if(!g_freeGroupLayout||!GroupedView()||g_groupEditMode||GetKeyState(VK_SHIFT)<0)return false;
    POINT point{GET_X_LPARAM(lp),GET_Y_LPARAM(lp)};ScreenToClient(hwnd,&point);
    int index=GroupAtPoint(point.x,point.y);if(index<0)return false;
    auto geometry=g_groupGeometry[index];int rows=((int)geometry.group->apps.size()+geometry.columns-1)/geometry.columns;
    if(geometry.group->collapsed||!g_groupPositions.count(geometry.group))return false;
    if(rows<=geometry.visibleRows)return false;
    g_wheelDelta+=delta;int step=g_wheelDelta/WHEEL_DELTA;g_wheelDelta%=WHEEL_DELTA;
    g_groupRows[geometry.group]=std::clamp(g_groupRows[geometry.group]-step,0,rows-geometry.visibleRows);
    ComputeGroups();g_hover=-1;Render();return true;
}

// Shared operations for pointer, context-menu and keyboard group editing.
void MoveAppToGroup(const Tile& tile, const std::shared_ptr<Folder>& destination, int position) {
    if (!tile.app || !destination) return;
    std::wstring key = AppKey(*tile.app);
    for (auto& group : g_tiles) if (group.folder) {
        auto& apps = group.folder->apps;
        for (int i = (int)apps.size() - 1; i >= 0; --i) if (apps[i].app && AppKey(*apps[i].app) == key) {
            if (group.folder == destination && i < position) --position;
            apps.erase(apps.begin() + i);
        }
    }
    position = std::clamp(position, 0, (int)destination->apps.size());
    destination->apps.insert(destination->apps.begin() + position, tile);
    g_hidden.erase(key);
    if (PinnedOnly()) { g_pins.insert(key); SavePins(); }
}

void MoveGroup(const std::shared_ptr<Folder>& group, int destination) {
    int from = IndexOf(g_tiles, {nullptr, group});
    if (from < 0) return;
    Tile tile = g_tiles[from]; g_tiles.erase(g_tiles.begin() + from);
    destination = std::clamp(destination, 0, (int)g_tiles.size());
    g_tiles.insert(g_tiles.begin() + destination, tile);
}

struct GroupNameDialog {
    HWND edit = nullptr;
    bool done = false, accepted = false;
    std::wstring initial;
};
constexpr WCHAR kGroupNameClass[] = L"RYJSLaunchpadGroupName";
LRESULT CALLBACK GroupNameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* dialog = (GroupNameDialog*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (msg == WM_NCCREATE) {
        dialog = (GroupNameDialog*)((CREATESTRUCTW*)lParam)->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)dialog);
    }
    if (dialog && msg == WM_COMMAND && (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)) {
        dialog->accepted = LOWORD(wParam) == IDOK; dialog->done = true; return 0;
    }
    if (dialog && msg == WM_CLOSE) { dialog->done = true; return 0; }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

std::wstring PromptGroupName(HWND owner, const std::wstring& initial,PCWSTR caption=L"Group name",int limit=80) {
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = GroupNameProc; wc.hInstance = (HINSTANCE)&__ImageBase;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); wc.lpszClassName = kGroupNameClass;
    RegisterClassExW(&wc);
    GroupNameDialog dialog; dialog.initial = initial;
    UINT dpi = GetDpiForWindow(owner);
    auto px = [&](int dip) { return MulDiv(dip, dpi, 96); };
    RECT area{}; GetWindowRect(owner, &area);
    HWND window = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_CONTROLPARENT, kGroupNameClass, Tr(caption),
        WS_CAPTION | WS_SYSMENU, (area.left + area.right - px(440))/2, (area.top + area.bottom - px(170))/2,
        px(440), px(170), owner, nullptr, wc.hInstance, &dialog);
    if (!window) return {};
#ifdef SEAMLESS_PREVIEW
    if(Preview::uiCheck)SetWindowLongPtrW(window,GWL_EXSTYLE,GetWindowLongPtrW(window,GWL_EXSTYLE)|WS_EX_APPWINDOW);
#endif
    HFONT font = CreateFontW(-px(14), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, g_settings.font.c_str());
    dialog.edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", initial.c_str(), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
        px(18), px(20), px(392), px(32), window, (HMENU)10, wc.hInstance, nullptr);
    HWND save = CreateWindowExW(0, L"BUTTON", Tr(L"Save"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
        px(222), px(74), px(90), px(30), window, (HMENU)IDOK, wc.hInstance, nullptr);
    HWND cancel = CreateWindowExW(0, L"BUTTON", Tr(L"Cancel"), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        px(320), px(74), px(90), px(30), window, (HMENU)IDCANCEL, wc.hInstance, nullptr);
    for (HWND control : {dialog.edit, save, cancel}) SendMessageW(control, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(dialog.edit, EM_SETLIMITTEXT, limit, 0);
    bool previousModal = g_modal; g_modal = true;
    EnableWindow(owner,FALSE);ShowWindow(window, SW_SHOW); SetForegroundWindow(window); SetFocus(dialog.edit);
    SendMessageW(dialog.edit, EM_SETSEL, 0, -1);
    MSG message;
    while (!dialog.done) {
        int result = GetMessageW(&message, nullptr, 0, 0);
        if (result <= 0) { if (!result) PostQuitMessage((int)message.wParam); break; }
        if(message.message==WM_KEYDOWN&&(message.wParam==VK_RETURN||message.wParam==VK_ESCAPE)) {
            dialog.accepted=message.wParam==VK_RETURN;dialog.done=true;continue;
        }
        if (!IsDialogMessageW(window, &message)) { TranslateMessage(&message); DispatchMessageW(&message); }
    }
    std::wstring name;
    if (dialog.accepted) {
        name.resize(GetWindowTextLengthW(dialog.edit) + 1);
        name.resize(GetWindowTextW(dialog.edit, name.data(), (int)name.size()));
        std::erase_if(name, [](WCHAR c) { return c < 32; });
        size_t begin = name.find_first_not_of(L" ");
        name = begin == std::wstring::npos ? L"" : name.substr(begin, name.find_last_not_of(L" ") - begin + 1);
    }
    EnableWindow(owner,TRUE);DestroyWindow(window); DeleteObject(font);
    g_modal = previousModal;
    if (g_shown) SetForegroundWindow(owner);
    return name;
}

void CreateGroup(HWND hwnd) {
    auto name = PromptGroupName(hwnd, Tr(L"New group"));
    if (name.empty()) return;
    auto group = std::make_shared<Folder>(); group->name = std::move(name);
    g_tiles.push_back({nullptr, group}); SaveTiles(); Refresh();
}

void ShowGroupMenu(HWND hwnd, const std::shared_ptr<Folder>& group, POINT point) {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, 1, Tr(L"Rename group"));
    AppendMenuW(menu, MF_STRING, 2, Tr(L"Move group earlier"));
    AppendMenuW(menu, MF_STRING, 3, Tr(L"Move group later"));
    AppendMenuW(menu, MF_STRING, 4, Tr(L"Delete group"));
    AppendMenuW(menu, MF_STRING, 5, Tr(L"New group"));
    AppendMenuW(menu, MF_STRING, 6, Tr(g_groupEditMode?L"Finish group editing":L"Edit group layout"));
    AppendMenuW(menu, MF_STRING, 7, Tr(group->collapsed?L"Show as expanded group":L"Show as traditional folder"));
    if(g_groupEditMode)AppendMenuW(menu,MF_STRING,8,Tr(L"Cancel group editing"));
    g_modal = true;
    int command = TrackLaunchpadPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, point.x, point.y, hwnd, nullptr);
    g_modal = false; DestroyMenu(menu);
    if(command==6){ToggleGroupEdit();return;}
    if(command==8){CancelGroupEdit();return;}
    if(g_groupEditMode)return;
    if(command==7){
        if(g_groupPositionsReadOnly||g_layoutReadOnly||g_dataReadOnly)return;
        bool wasFree=g_freeGroupLayout,added=false;
        if(!g_groupPositions.count(group)) {
            auto geometry=std::find_if(g_groupGeometry.begin(),g_groupGeometry.end(),[&](const auto& item){return item.group==group;});
            if(geometry==g_groupGeometry.end())return;
            auto position=PositionFromArea(geometry->area);
            if(geometry->squareCells)position=EditorGroupPosition(*geometry);
            else if(geometry->wholeCells) {
                position.style=4;position.snapped=true;
                position.gridX=GroupGridIndex(position.x,GroupPitchX()/g_layout.scale);
                position.gridY=GroupGridIndex(position.y,GroupPitchY()/g_layout.scale);
                position.gridW=geometry->columns;position.gridH=geometry->visibleRows+1;
            }
            g_groupPositions[group]=position;
            added=true;
        }
        g_freeGroupLayout=true;
        group->collapsed=!group->collapsed;
        if(!SaveGroupPositions()) {
            group->collapsed=!group->collapsed;g_freeGroupLayout=wasFree;
            if(added)g_groupPositions.erase(group);
        }
        Refresh();return;
    }
    if (command == 1) {
        auto name = PromptGroupName(hwnd, group->name);
        if (!name.empty()) group->name = std::move(name);
    } else if (command == 2 || command == 3) {
        MoveGroup(group, IndexOf(g_tiles, {nullptr,group}) + (command == 2 ? -1 : 1));
    } else if (command == 4) {
        int index = IndexOf(g_tiles, {nullptr,group});
        if (index >= 0) g_tiles.erase(g_tiles.begin() + index);
        auto inbox = InboxGroup();
        size_t offset=inbox->apps.size()+inbox->unavailable.size();
        for (const auto& missing : group->unavailable) inbox->unavailable.emplace_back(offset+missing.first,missing.second);
        inbox->apps.insert(inbox->apps.end(), group->apps.begin(), group->apps.end());
    } else if (command == 5) { CreateGroup(hwnd); return; }
    if (command) { SaveTiles(); Refresh(); }
}

void GroupDragMove(int x, int y) {
    g_groupDropFolder.reset(); g_groupDropSlot = -1;
    int index = GroupAtPoint(x, y);
    if (index < 0) return;
    const auto& geometry = g_groupGeometry[index];
    g_groupDropFolder = geometry.group;
    auto area = ProjectGroupRect(geometry.area);
    auto origin=GroupCellsOrigin(geometry);
    float relativeX=x-area.left-(origin.x-geometry.area.left);
    float relativeY=y-area.top-(origin.y-geometry.area.top);
    int col=std::clamp((int)std::floor(relativeX/geometry.cellWidth),0,geometry.columns-1);
    int row=std::max(0,(int)std::floor(relativeY/geometry.rowPitch));
    g_groupDropSlot = std::clamp(geometry.firstInGroup + row * geometry.columns + col, 0, (int)geometry.group->apps.size());
}

[[clang::no_destroy]] std::shared_ptr<Folder> g_groupHeaderPressed;
POINT g_groupHeaderStart{};
POINT g_groupHeaderAt{};
double g_groupHeaderEdgeTime=0;
int g_groupHeaderEdge=0;
bool GroupPointerDown(HWND hwnd, int x, int y) {
    if(g_groupEditMode)return FreeGroupPointerDown(hwnd,x,y);
    int index = GroupAtPoint(x, y, true);
    if (index < 0) return false;
    g_groupHeaderPressed = g_groupGeometry[index].group;
    g_groupHeaderStart = g_groupHeaderAt = {x,y}; SetCapture(hwnd); return true;
}
bool GroupPointerMove(int x, int y) {
    if(g_groupGesture.group)return FreeGroupPointerMove(x,y);
    if(!g_groupEditMode&&g_groupHeaderPressed)return true;
    if (!g_groupHeaderPressed) return false;
    if (std::abs(x-g_groupHeaderStart.x) > GetSystemMetrics(SM_CXDRAG) || std::abs(y-g_groupHeaderStart.y) > GetSystemMetrics(SM_CYDRAG))
        g_groupHeaderDragging = g_groupHeaderPressed;
    int index = GroupAtPoint(x,y);
    g_groupDropFolder = index < 0 ? nullptr : g_groupGeometry[index].group;
    g_groupHeaderAt={x,y};
    int edge=g_settings.pages ? (x<36*g_layout.scale?-1:x>g_layout.width-36*g_layout.scale?1:0) :
        (y<g_layout.viewTop?-1:y>=g_layout.viewBottom?1:0);
    if (edge!=g_groupHeaderEdge) {g_groupHeaderEdge=edge;g_groupHeaderEdgeTime=Now();}
    StartFrames(); return true;
}
void GroupHeaderTick() {
    if (g_groupHeaderEdge && Now()-g_groupHeaderEdgeTime>0.35) {
        g_groupHeaderEdgeTime=Now(); ScrollRows(g_groupHeaderEdge);
    }
    int index=GroupAtPoint(g_groupHeaderAt.x,g_groupHeaderAt.y);
    g_groupDropFolder=index<0?nullptr:g_groupGeometry[index].group;
}
void CancelGroupPointer() {
    bool preview=(bool)g_groupGesture.group;g_groupGesture={};
    if(preview){g_groupTransitions.clear();StartFrames();}
    g_groupHeaderPressed.reset(); g_groupHeaderDragging.reset(); g_groupDropFolder.reset();
    g_groupHeaderEdge=0;
}
bool GroupPointerUp(HWND hwnd) {
    if(g_groupEditMode)return FreeGroupPointerUp(hwnd);
    if (!g_groupHeaderPressed) return false;
    if (g_groupHeaderDragging && g_groupDropFolder && g_groupHeaderDragging != g_groupDropFolder) {
        MoveGroup(g_groupHeaderDragging, IndexOf(g_tiles, {nullptr,g_groupDropFolder}));
        SaveTiles();
    }
    CancelGroupPointer();
    if (GetCapture() == hwnd) ReleaseCapture();
    Refresh(); return true;
}

void ImportOrRestoreLayout(HWND hwnd, bool restore) {
    std::wstring path = StoragePath(LayoutFile()) + L".bak";
    if (!restore) {
        WCHAR file[MAX_PATH]{};
        auto filter = std::wstring(Tr(L"Launchpad layout files")) + L'\0' + L"*.txt;*.bak" + L'\0' + L'\0';
        OPENFILENAMEW dialog{sizeof(dialog)};
        dialog.hwndOwner=hwnd; dialog.lpstrFilter=filter.c_str(); dialog.lpstrFile=file; dialog.nMaxFile=ARRAYSIZE(file);
        dialog.lpstrTitle=Tr(L"Import layout file"); dialog.Flags=OFN_FILEMUSTEXIST|OFN_NOCHANGEDIR;
        g_modal=true; BOOL selected=GetOpenFileNameW(&dialog); g_modal=false;
        if (!selected) return;
        path=file;
    }
    auto records = ReadLines(path);
    std::wstring content;
    bool groupFile = !records.empty() && records[0][0] == L"schema";
    if (records.empty() || !ryjs_data::ReadUtf8(path, content) ||
        !ryjs_data::Validate(groupFile ? L"groups-v1-import.txt" : L"layout-import.txt", content) ||
        !ryjs_data::ValidateDirectory(DataRoot(), LayoutFile())) {
        g_modal=true;
        MessageBoxW(hwnd,Tr(L"The layout file is empty, unreadable, or uses a newer version."),Tr(L"Import layout file"),MB_OK|MB_ICONWARNING);
        g_modal=false; return;
    }
    std::vector<std::wstring> lines;
    for (const auto& fields : records) {
        std::wstring line;
        for (const auto& field : fields) { if (!line.empty()) line+=L'\t'; line+=field; }
        lines.push_back(std::move(line));
    }
    auto current=StoragePath(LayoutFile());
    if (GetFileAttributesW(current.c_str())!=INVALID_FILE_ATTRIBUTES &&
        !CopyFileW(current.c_str(),(current+L".before-import.bak").c_str(),FALSE)) {
        g_modal=true;
        MessageBoxW(hwnd,Tr(L"Could not back up the current layout. Import was cancelled."),Tr(L"Import layout file"),MB_OK|MB_ICONWARNING);
        g_modal=false; return;
    }
    auto previous = ArrangementLines();
    bool previousLayoutReadOnly = g_layoutReadOnly, previousDataReadOnly = g_dataReadOnly;
    g_openFolder=nullptr; g_query.clear(); g_caret=g_anchor=0;
    g_layoutReadOnly=g_dataReadOnly=false; ArrangeTiles(lines);
    if (!SaveTiles(true)) {
        ArrangeTiles(previous);
        g_layoutReadOnly=previousLayoutReadOnly; g_dataReadOnly=previousDataReadOnly;
        g_modal=true;
        MessageBoxW(hwnd,Tr(L"Could not save the layout. The previous layout was kept."),Tr(L"Import layout file"),MB_OK|MB_ICONWARNING);
        g_modal=false;
    }
    Refresh();
}

void ExportLayout(HWND hwnd) {
    if ((!g_layoutReadOnly && !g_dataReadOnly && !SaveTiles()) || g_layoutReadOnly || g_dataReadOnly) {
        MessageBoxW(hwnd,Tr(L"Could not export the layout. Use a full backup to preserve unreadable data."),Tr(L"Export layout file"),MB_OK|MB_ICONWARNING);
        return;
    }
    WCHAR file[32768] = L"ryjs-layout.txt";
    auto filter = std::wstring(Tr(L"Launchpad layout files")) + L'\0' + L"*.txt" + L'\0' + L'\0';
    OPENFILENAMEW dialog{sizeof(dialog)};
    dialog.hwndOwner=hwnd; dialog.lpstrFilter=filter.c_str(); dialog.lpstrFile=file; dialog.nMaxFile=ARRAYSIZE(file);
    dialog.lpstrDefExt=L"txt"; dialog.lpstrTitle=Tr(L"Export layout file");
    dialog.Flags=OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR;
    g_modal=true; BOOL selected=GetSaveFileNameW(&dialog); g_modal=false;
    if (!selected) return;
    auto current=StoragePath(LayoutFile());
    if (_wcsicmp(file,current.c_str())==0 || !CopyFileW(current.c_str(),file,FALSE))
        MessageBoxW(hwnd,Tr(L"Could not save the layout. The previous layout was kept."),Tr(L"Export layout file"),MB_OK|MB_ICONWARNING);
}


// After a change that may have left the open folder with one app or none.
void DropGoneFolder() {
    TidyTiles();
    if (g_openFolder && IndexOf(g_tiles, {nullptr, g_openFolder}) < 0) {
        g_openFolder = nullptr;
    }
    g_hover = g_selected = g_pressed = -1;
}

void OpenFolder(std::shared_ptr<Folder> folder) {
    FinishMetrics();
    if (g_settings.performanceLogging) g_metrics.Begin("folder-open", Now(), g_layout.width, g_layout.height, g_layout.scale, g_refreshHz);
    g_folderRenaming = false;
    g_folderTitleHover = false;
    g_openFolder = std::move(folder);
    g_folderOriginalName=g_openFolder->name;
    g_closingFolder = nullptr;
    g_folderAnim = Animate() ? 0.0f : 1.0f;
    if (g_folderAnim < 1) {
        StartFrames();
    }
    g_folderRow = 0;
    g_hover = g_selected = g_pressed = -1;
    g_searchDrag = false;
    g_hist[1] = {};  // Fresh history per folder.
    SetCaret(EditText().size(), false);  // At the end of the name.
    Render();
}

void CloseFolder() {
    if (!g_openFolder) {
        return;
    }
    FinishMetrics();
    if (g_settings.performanceLogging) g_metrics.Begin("folder-close", Now(), g_layout.width, g_layout.height, g_layout.scale, g_refreshHz);
    if (Animate()) {
        g_closingFolder = g_openFolder;
        StartFrames();
    }
    g_folderRenaming = false;
    g_folderTitleHover = false;
    SaveFolderName();
    g_openFolder = nullptr;
    g_hover = g_selected = g_pressed = -1;
    g_searchDrag = false;
    SetCaret(g_query.size(), false);
    Refresh();
}

void ActivateItem(int index) {
    const Tiles& items = Items();
    if (index < 0 || index >= (int)items.size()) {
        return;
    }
    if (items[index].folder) {
        OpenFolder(items[index].folder);
    } else {
        LaunchTarget(items[index].app->target,nullptr,items[index].app.get());
    }
}

// Where the click doesn't count as one on empty space.
bool ClickKeepsOpen(int x, int y) {
    if(ToolBarConsumes(x,y))return true;
    // A section is a caption and a run of icons, not a panel: its empty space
    // is empty space, and clicking there closes the grid like anywhere else.
    // Only the caption itself stays a target, since it starts a drag.
    if (GroupedView() && GroupAtPoint(x, y, true) >= 0) return true;
    return g_openFolder ? InRect(GetFolderLayout().panel, (float)x, (float)y)
                        : InRect(g_layout.search, (float)x, (float)y) ||
                              PowerAt(x, y) >= 0;
}

// The words both names start with ("Microsoft Edge" and "Microsoft Teams"
// give "Microsoft"), or "Folder".
std::wstring FolderName(const std::wstring& a, const std::wstring& b) {
    size_t same = 0;
    for (size_t i = 0;; i++) {
        bool endA = i == a.size() || a[i] == L' ';
        bool endB = i == b.size() || b[i] == L' ';
        if (endA && endB) {
            same = i;
            if (i == a.size() || i == b.size()) {
                break;
            }
            continue;
        }
        if (endA || endB || towlower(a[i]) != towlower(b[i])) {
            break;
        }
    }
    return same ? a.substr(0, same) : Tr(L"Folder");
}

void StartDrag() {
    const Tiles& items = Items();
    int i = g_pressed;
    g_drag.pending = false;
    if (i < 0 || i >= (int)items.size() || !g_query.empty()) {
        return;
    }
    Cell c = ItemCell(i);
    g_drag.active = true;
    g_drag.tile = items[i];
    g_drag.at = g_drag.start;
    g_drag.grab = D2D1::Point2F(g_drag.start.x - c.icon.left,
                                g_drag.start.y - c.icon.top);
    g_drag.target = g_drag.slot = -1;
    g_drag.edge = 0;
    g_drag.inPanel = g_openFolder != nullptr;
    g_drag.backup = ArrangementLines();
    g_hover = g_selected = g_pressed = -1;
    StartFrames();
}

void DragMove(int x, int y) {
    const Layout& L = g_layout;
    double now = Now();
    g_drag.at = {x, y};
    if (g_settings.groups) {
        GroupDragMove(x,y);
        int edge = g_settings.pages ? (x < 36 * L.scale ? -1 : x > L.width - 36 * L.scale ? 1 : 0) :
            (y < L.viewTop ? -1 : y >= L.viewBottom ? 1 : 0);
        if (edge != g_drag.edge) { g_drag.edge = edge; g_drag.edgeTime = now; }
        return;
    }
    if (g_openFolder) {
        if (InRect(GetFolderLayout().panel, (float)x, (float)y)) {
            g_drag.inPanel = true;
        } else if (g_drag.inPanel) {
            // Out of the folder, into the grid right after it.
            Tiles& apps = g_openFolder->apps;
            int i = IndexOf(apps, g_drag.tile);
            if (i >= 0) {
                apps.erase(apps.begin() + i);
                int f = IndexOf(g_tiles, {nullptr, g_openFolder});
                g_tiles.insert(g_tiles.begin() + (f + 1), g_drag.tile);
            }
            g_openFolder = nullptr;
            UpdateFilter();
            ComputeLayout();
        }
    }

    // Over the middle of another icon: dropping there makes or fills a folder.
    // The rest of that icon moves nothing, so reaching the middle doesn't
    // shuffle the icons first.
    int target = -1;
    bool overIcon = false;
    if (!g_openFolder && g_drag.tile.app) {
        int i = ItemAt(x, y);
        if (i >= 0 && !IsDragged(g_visible[i])) {
            D2D1_RECT_F r = CellAt(i).icon;
            overIcon = InRect(r, (float)x, (float)y);
            float in = (r.right - r.left) * 0.2f;
            if (InRect(D2D1::RectF(r.left + in, r.top + in, r.right - in,
                                   r.bottom - in),
                       (float)x, (float)y)) {
                target = i;
            }
        }
    }
    if (target != g_drag.target) {
        g_drag.target = target;
        g_drag.targetTime = now;
    }
    int slot = overIcon ? -1 : SlotAt(x, y);
    if (slot != g_drag.slot) {
        g_drag.slot = slot;
        g_drag.slotTime = now;
    }

    int edge = 0;
    if (!g_openFolder) {
        if (g_settings.pages) {
            float zone =
                std::max(36 * L.scale, std::min(L.gridLeft, L.width * 0.06f));
            edge = x < zone ? -1 : x >= L.width - zone ? 1 : 0;
        } else {
            edge = y < L.viewTop ? -1 : y >= L.viewBottom ? 1 : 0;
        }
    }
    if (edge != g_drag.edge) {
        g_drag.edge = edge;
        g_drag.edgeTime = now;
    }
}

// Each frame of a drag: what happens when the pointer rests somewhere.
void DragTick() {
    // The page or rows under a still pointer move while scrolling.
    DragMove(g_drag.at.x, g_drag.at.y);
    double now = Now();
    if (g_drag.edge &&
        now - g_drag.edgeTime >= (g_settings.pages ? 0.6 : 0.3)) {
        g_drag.edgeTime = now;
        ScrollRows(g_drag.edge);
    }
    if (g_settings.groups) return;

    int t = g_drag.target;
    if (t >= 0 && t < (int)g_visible.size() && g_visible[t].folder &&
        now - g_drag.targetTime >= 0.7) {
        // Resting on a folder opens it, to place the app inside.
        std::shared_ptr<Folder> folder = g_visible[t].folder;
        int i = IndexOf(g_tiles, g_drag.tile);
        if (i >= 0) {
            g_tiles.erase(g_tiles.begin() + i);
        }
        folder->apps.push_back(g_drag.tile);
        UpdateFilter();
        ComputeLayout();
        OpenFolder(folder);
        FolderLayout F = GetFolderLayout();
        g_folderRow = F.rows - F.visibleRows;
        g_drag.target = g_drag.slot = -1;
        g_drag.edge = 0;
        g_drag.inPanel = false;
        return;
    }

    if (g_drag.slot >= 0 && now - g_drag.slotTime >= 0.2) {
        Tiles& tiles = g_openFolder ? g_openFolder->apps : g_tiles;
        int from = IndexOf(tiles, g_drag.tile);
        int to = std::min(g_drag.slot, (int)tiles.size() - 1);
        if (from >= 0 && to != from) {
            CaptureTileMotion();
            Tile tile = tiles[from];
            tiles.erase(tiles.begin() + from);
            tiles.insert(tiles.begin() + to, tile);
            if (!g_openFolder) {
                UpdateFilter();
                ComputeLayout();
            }
        }
    }
}

// drop: keep the new place (and make or fill a folder); otherwise everything
// goes back.
void EndDrag(bool drop) {
    g_drag.pending = false;
    if (!g_drag.active) {
        return;
    }
    if (g_settings.groups) {
        CaptureTileMotion();
        if (drop && g_groupDropFolder) MoveAppToGroup(g_drag.tile, g_groupDropFolder, g_groupDropSlot);
        if (!drop) ArrangeTiles(g_drag.backup);
        g_drag = {}; g_groupDropFolder.reset(); g_groupDropSlot = -1;
        if (drop) SaveTiles();
        Refresh(); return;
    }
    CaptureTileMotion();
    if (drop) {
        for (auto& motion : g_tileMotion) {
            if (motion.key == MotionKey(g_drag.tile)) {
                float size = std::round(g_layout.icon * 1.06f);
                motion.from = {g_drag.at.x-g_drag.grab.x-(size-g_layout.icon)/2,
                               g_drag.at.y-g_drag.grab.y-(size-g_layout.icon)/2};
            }
        }
    }
    g_drag.active = false;
    Tile tile = std::move(g_drag.tile);
    g_drag.tile = {};
    int target = g_drag.target;
    g_drag.target = g_drag.slot = -1;
    g_drag.edge = 0;
    if (!drop) {
        g_openFolder = nullptr;
        ArrangeTiles(g_drag.backup);
    } else if (target >= 0 && target < (int)g_tiles.size()) {
        Tile onto = g_tiles[target];
        int from = IndexOf(g_tiles, tile);
        if (onto.folder) {
            onto.folder->apps.push_back(tile);
        } else {
            auto folder = std::make_shared<Folder>();
            folder->name = FolderName(onto.app->name, tile.app->name);
            folder->apps = {onto, tile};
            g_tiles[target] = {nullptr, folder};
        }
        if (from >= 0) {
            g_tiles.erase(g_tiles.begin() + from);
        }
    }
    g_drag.backup.clear();
    DropGoneFolder();
    if (drop) {
        SaveTiles();
    }
    Refresh();
}

// What an app's menu needs.
struct AppPaths {
    std::wstring file;      // The program it starts, when that's a file.
    std::wstring folder;    // A Store app's install folder.
    std::wstring shortcut;  // Its shortcut in the Start menu or on the desktop.
};

std::wstring ItemString(IShellItem2* item, PCWSTR property) {
    PROPERTYKEY key;
    PWSTR value = nullptr;
    std::wstring result;
    if (SUCCEEDED(PSGetPropertyKeyFromName(property, &key)) &&
        SUCCEEDED(item->GetString(key, &value))) {
        result = value;
        CoTaskMemFree(value);
    }
    return result;
}

bool FileExists(const std::wstring& path) {
    return !path.empty() &&
           GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

// Start lists the shortcuts in these two folders and their subfolders. The
// one named like the app, or else the one that starts the same file.
std::wstring FindStartShortcut(const std::wstring& name,
                               const std::wstring& file) {
    std::vector<std::wstring> dirs, links;
    for (REFKNOWNFOLDERID id : {FOLDERID_Programs, FOLDERID_CommonPrograms}) {
        PWSTR root = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &root))) {
            dirs.push_back(root);
        }
        CoTaskMemFree(root);
    }
    while (!dirs.empty() && links.size() < 5000) {
        std::wstring dir = std::move(dirs.back());
        dirs.pop_back();
        WIN32_FIND_DATAW fd;
        HANDLE find = FindFirstFileW((dir + L"\\*").c_str(), &fd);
        if (find == INVALID_HANDLE_VALUE) {
            continue;
        }
        do {
            std::wstring path = dir + L"\\" + fd.cFileName;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (fd.cFileName[0] != L'.' &&
                    !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                    dirs.push_back(path);
                }
            } else if (EndsWith(ToLower(path), L".lnk")) {
                links.push_back(path);
            }
        } while (FindNextFileW(find, &fd));
        FindClose(find);
    }

    std::wstring wanted = L"\\" + ToLower(name) + L".lnk";
    for (const auto& link : links) {
        if (EndsWith(ToLower(link), wanted)) {
            return link;
        }
    }
    ComPtr<IShellLinkW> shellLink;
    ComPtr<IPersistFile> persist;
    if (file.empty() ||
        FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&shellLink))) ||
        FAILED(shellLink.As(&persist))) {
        return {};
    }
    std::wstring lowerFile = ToLower(file);
    for (const auto& link : links) {
        WCHAR target[MAX_PATH];
        if (SUCCEEDED(persist->Load(link.c_str(), STGM_READ)) &&
            shellLink->GetPath(target, ARRAYSIZE(target), nullptr, 0) == S_OK &&
            ToLower(target) == lowerFile) {
            return link;
        }
    }
    return {};
}

AppPaths GetAppPaths(const App& app) {
    AppPaths paths;
    std::wstring lower = ToLower(app.target);
    bool fromAppsFolder = lower.rfind(ToLower(kAppsFolder), 0) == 0;
    ComPtr<IShellItem2> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(app.target.c_str(), nullptr,
                                              IID_PPV_ARGS(&item)))) {
        paths.file = ItemString(item.Get(), L"System.Link.TargetParsingPath");
        paths.folder =
            ItemString(item.Get(), L"System.AppUserModel.PackageInstallPath");
    }
    if (!fromAppsFolder) {
        if (EndsWith(lower, L".lnk")) {
            paths.shortcut = app.target;
        } else if (paths.file.empty()) {
            paths.file = app.target;
        }
    }
    // Control Panel and the like point to "::{...}", not a file.
    if (paths.file.rfind(L"::", 0) == 0 || !FileExists(paths.file)) {
        paths.file.clear();
    }
    if (paths.shortcut.empty() && fromAppsFolder && paths.folder.empty()) {
        paths.shortcut = FindStartShortcut(app.name, paths.file);
    }
    return paths;
}

void ShowInFolder(const std::wstring& path) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0,
                                     nullptr))) {
        SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
        CoTaskMemFree(pidl);
    }
}

bool PickIconFile(HWND hwnd, std::wstring& path) {
    WCHAR file[MAX_PATH] = L"";
    OPENFILENAMEW ofn{sizeof(ofn)};
    ofn.hwndOwner = hwnd;
    auto filter = launchpad_i18n::IconFileFilter();
    ofn.lpstrFilter = filter.c_str();
    ofn.lpstrFile = file;
    ofn.nMaxFile = ARRAYSIZE(file);
    ofn.lpstrTitle = Tr(L"Choose a picture for the icon");
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
                OFN_NOCHANGEDIR;
    bool previousModal = g_modal;
    g_modal = true;
    BOOL ok = GetOpenFileNameW(&ofn);
    g_modal = previousModal;
    if (g_shown && !g_closing) {
        ForceForeground(hwnd);
    }
    if (ok) {
        path = file;
    }
    return ok;
}

// Takes the app off the grid: removed, or unpinned when only pinned apps show.
void RemoveApp(const std::shared_ptr<App>& app) {
    if (PinnedOnly()) {
        g_pins.erase(AppKey(*app));
        SavePins();
    } else {
        g_hidden.insert(AppKey(*app));
    }
    auto drop = [&](Tiles& tiles) {
        tiles.erase(std::remove_if(tiles.begin(), tiles.end(),
                                   [&](const Tile& t) { return t.app == app; }),
                    tiles.end());
    };
    drop(g_tiles);
    for (auto& tile : g_tiles) {
        if (tile.folder) {
            drop(tile.folder->apps);
        }
    }
    DropGoneFolder();
    SaveTiles();
    Refresh();
}

void TakeOutOfFolder(const Tile& tile) {
    if (!g_openFolder) {
        return;
    }
    Tiles& apps = g_openFolder->apps;
    int i = IndexOf(apps, tile);
    if (i < 0) {
        return;
    }
    apps.erase(apps.begin() + i);
    int f = IndexOf(g_tiles, {nullptr, g_openFolder});
    g_tiles.insert(g_tiles.begin() + (f + 1), tile);
    DropGoneFolder();
    SaveTiles();
    Refresh();
}

void Ungroup(const std::shared_ptr<Folder>& folder) {
    int f = IndexOf(g_tiles, {nullptr, folder});
    if (f < 0) {
        return;
    }
    Tiles apps = folder->apps;
    g_tiles.erase(g_tiles.begin() + f);
    g_tiles.insert(g_tiles.begin() + f, apps.begin(), apps.end());
    DropGoneFolder();
    SaveTiles();
    Refresh();
}

// An "&" in a menu would underline the next letter.
std::wstring MenuText(const std::wstring& text) {
    std::wstring out;
    for (WCHAR c : text) {
        if (c == L'&') {
            out += L'&';
        }
        out += c;
    }
    return out;
}

// Store apps and shell-resolved exes live behind an AppsFolder target; their
// elevation goes through a temporary .lnk, never the raw target.
bool IsPackagedTarget(const std::wstring& target) {
    return ToLower(target).rfind(ToLower(kAppsFolder), 0) == 0;
}

// Fetch only for an explicit add/update. No periodic networking or third-party proxy.
struct WebHandle {
    HINTERNET value=nullptr;
    WebHandle(HINTERNET h=nullptr):value(h){}
    ~WebHandle(){if(value)WinHttpCloseHandle(value);}
    operator HINTERNET() const{return value;}
};
struct WebIconResult {std::wstring key,url,path;};
[[clang::no_destroy]] std::vector<HANDLE> g_webWorkers;
std::atomic<bool> g_webStop=false;
constexpr UINT kWebIconReady=WM_APP+64;
std::wstring WebWide(const std::string& bytes) {
    int n=MultiByteToWideChar(CP_UTF8,0,bytes.data(),(int)bytes.size(),nullptr,0);
    std::wstring value(n,L'\0');if(n)MultiByteToWideChar(CP_UTF8,0,bytes.data(),(int)bytes.size(),value.data(),n);return value;
}
bool DownloadWeb(const std::wstring& url,std::string& bytes,std::wstring& finalUrl) {
    if (g_webStop || !IsWebEntry(url)) return false;
    URL_COMPONENTS parts{sizeof(parts)};
    parts.dwHostNameLength=parts.dwUrlPathLength=parts.dwExtraInfoLength=parts.dwUserNameLength=parts.dwPasswordLength=(DWORD)-1;
    if (!WinHttpCrackUrl(url.c_str(),0,0,&parts) || !parts.dwHostNameLength || parts.dwUserNameLength || parts.dwPasswordLength) return false;
    std::wstring host(parts.lpszHostName,parts.dwHostNameLength),path(parts.lpszUrlPath,parts.dwUrlPathLength);
    path+=std::wstring(parts.lpszExtraInfo?parts.lpszExtraInfo:L"",parts.dwExtraInfoLength);
    if(path.empty())path=L"/";
    WebHandle session(WinHttpOpen(L"RYJSLaunchpad/0.1",WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0));
    if(!session)return false;
    WinHttpSetTimeouts(session,3000,3000,3000,3000);
    WebHandle connection(WinHttpConnect(session,host.c_str(),parts.nPort,0));if(!connection)return false;
    WebHandle request(WinHttpOpenRequest(connection,L"GET",path.c_str(),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,parts.nScheme==INTERNET_SCHEME_HTTPS?WINHTTP_FLAG_SECURE:0));
    if(!request)return false;
    DWORD disabled=WINHTTP_DISABLE_COOKIES;WinHttpSetOption(request,WINHTTP_OPTION_DISABLE_FEATURE,&disabled,sizeof(disabled));
    DWORD logon=WINHTTP_AUTOLOGON_SECURITY_LEVEL_HIGH;WinHttpSetOption(request,WINHTTP_OPTION_AUTOLOGON_POLICY,&logon,sizeof(logon));
    DWORD policy=WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP;WinHttpSetOption(request,WINHTTP_OPTION_REDIRECT_POLICY,&policy,sizeof(policy));
    DWORD redirects=5;WinHttpSetOption(request,WINHTTP_OPTION_MAX_HTTP_AUTOMATIC_REDIRECTS,&redirects,sizeof(redirects));
    if(!WinHttpSendRequest(request,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,0)||!WinHttpReceiveResponse(request,nullptr))return false;
    DWORD code=0,size=sizeof(code);
    if(!WinHttpQueryHeaders(request,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,nullptr,&code,&size,nullptr)||code!=200)return false;
    bytes.clear();char buffer[16384];DWORD count=0;
    ULONGLONG began=GetTickCount64();
    while(!g_webStop) {
        if(GetTickCount64()-began>15000)return false;
        if(!WinHttpReadData(request,buffer,sizeof(buffer),&count))return false;
        if(!count)break;
        if(bytes.size()+count>2*1024*1024)return false;
        bytes.append(buffer,count);
    }
    if(g_webStop||bytes.empty())return false;
    DWORD needed=0;WinHttpQueryOption(request,WINHTTP_OPTION_URL,nullptr,&needed);
    finalUrl.resize(needed/sizeof(WCHAR));
    if(needed&&WinHttpQueryOption(request,WINHTTP_OPTION_URL,finalUrl.data(),&needed))finalUrl.resize(wcslen(finalUrl.c_str()));
    else finalUrl=url;
    return true;
}
std::wstring WebAttribute(const std::wstring& tag,const std::wstring& name) {
    // Require an attribute boundary so data-href is not mistaken for href.
    std::wregex pattern(L"(?:^|\\s)"+name+LR"attr(\s*=\s*(?:"([^"]*)"|'([^']*)'|([^\s>]+)))attr",std::regex::icase);
    std::wsmatch match;
    if(!std::regex_search(tag,match,pattern))return {};
    std::wstring value=match[1].matched?match[1].str():match[2].matched?match[2].str():match[3].str();
    for(size_t pos=0;(pos=value.find(L"&amp;",pos))!=std::wstring::npos;)value.replace(pos,5,L"&");
    return value;
}
std::vector<std::wstring> WebsiteIconCandidates(const std::wstring& html,const std::wstring& base) {
    std::vector<std::wstring> result;
    auto resolve=[&](const std::wstring& relative) {
        WCHAR url[32768]{};
        DWORD n=ARRAYSIZE(url);
        if(SUCCEEDED(UrlCombineW(base.c_str(),relative.c_str(),url,&n,0))&&IsWebEntry(url)&&std::find(result.begin(),result.end(),url)==result.end())
            result.emplace_back(url);
    };
    std::wregex links(LR"(<link\b[^>]{0,4096}>)",std::regex::icase);
    for(std::wsregex_iterator it(html.begin(),html.end(),links),end;it!=end&&result.size()<5;++it) {
        auto tag=it->str(),rel=ToLower(WebAttribute(tag,L"rel"));
        if(rel.find(L"icon")!=std::wstring::npos)resolve(WebAttribute(tag,L"href"));
    }
    resolve(L"/favicon.ico");return result;
}
DWORD WINAPI WebIconWorker(void* argument) {
    std::unique_ptr<WebIconResult> result((WebIconResult*)argument);
    CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);
    std::string bytes;std::wstring base;
    std::vector<std::wstring> candidates;
    if(DownloadWeb(result->url,bytes,base))candidates=WebsiteIconCandidates(WebWide(bytes),base);
    else candidates=WebsiteIconCandidates(L"",result->url);
    auto temp=result->path+L"."+std::to_wstring(GetCurrentThreadId())+L".download";
    for(const auto& url:candidates) {
        if(g_webStop)break;
        if(!DownloadWeb(url,bytes,base))continue;
        HANDLE file=CreateFileW(temp.c_str(),GENERIC_WRITE,0,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(file==INVALID_HANDLE_VALUE)continue;
        DWORD written=0;bool saved=WriteFile(file,bytes.data(),(DWORD)bytes.size(),&written,nullptr)&&written==bytes.size();CloseHandle(file);
        App decoded;
        ComPtr<IWICImagingFactory> factory;ComPtr<IWICBitmapDecoder> decoder;
        bool image=saved&&SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&factory)))&&
            SUCCEEDED(factory->CreateDecoderFromFilename(temp.c_str(),nullptr,GENERIC_READ,WICDecodeMetadataCacheOnLoad,&decoder));
        bool decodedImage=image?LoadIconFile(temp,128,decoded):LoadSvgIcon(temp,128,decoded);
        if(decodedImage && MoveFileExW(temp.c_str(),result->path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) {
            HWND window=g_wnd;
            if(!g_webStop&&window&&PostMessageW(window,kWebIconReady,0,(LPARAM)result.get()))result.release();
            break;
        }
        DeleteFileW(temp.c_str());
    }
    DeleteFileW(temp.c_str());CoUninitialize();return 0;
}
void StartWebIconFetch(const std::wstring& key,const std::wstring& url) {
    if(g_webStop || !IsWebEntry(url))return;
    for(auto it=g_webWorkers.begin();it!=g_webWorkers.end();) {
        if(WaitForSingleObject(*it,0)==WAIT_OBJECT_0) {
            CloseHandle(*it);
            it=g_webWorkers.erase(it);
        } else ++it;
    }
    if(g_webWorkers.size()>=4)return;
    auto result=std::make_unique<WebIconResult>();result->key=key;result->url=url;result->path=WebIconPath(key+L"|"+url);
    SHCreateDirectoryExW(nullptr,std::filesystem::path(result->path).parent_path().c_str(),nullptr);
    HANDLE worker=CreateThread(nullptr,0,WebIconWorker,result.get(),0,nullptr);
    if(worker){g_webWorkers.push_back(worker);result.release();}
}
void OnWebIconReady(WebIconResult* payload) {
    std::unique_ptr<WebIconResult> result(payload);
    if(g_customIcons.count(result->key))return;
    for(const auto& app:g_apps)if(AppKey(*app)==result->key&&app->target==result->url){LoadIconFile(result->path,IconPixels(),*app);Render();break;}
}
void StopWebIcons() {
    g_webStop=true;
    for(HANDLE worker:g_webWorkers){WaitForSingleObject(worker,INFINITE);CloseHandle(worker);}g_webWorkers.clear();
    MSG message{};while(PeekMessageW(&message,nullptr,kWebIconReady,kWebIconReady,PM_REMOVE))delete (WebIconResult*)message.lParam;
}

// One native dialog for files, shortcuts, URLs, Store apps and existing aliases.
struct EntryEditor {
    HWND fields[5]{},groups{};
    std::vector<std::shared_ptr<Folder>> destinations;
    std::shared_ptr<App> app;
    bool done=false,accepted=false;
};
constexpr WCHAR kEntryEditorClass[]=L"RYJSLaunchpadEntryEditor";
std::wstring EntryText(HWND field) {
    std::wstring value(GetWindowTextLengthW(field)+1,L'\0');
    value.resize(GetWindowTextW(field,value.data(),(int)value.size())); return value;
}
bool PickEntryFile(HWND owner,std::wstring& path,bool icon=false) {
    if (icon) return PickIconFile(owner,path);
    WCHAR file[32768]{};
    OPENFILENAMEW dialog{sizeof(dialog)}; dialog.hwndOwner=owner;
    dialog.lpstrFilter=L"Programs, shortcuts and Remote Desktop\0*.exe;*.lnk;*.rdp\0All files\0*.*\0\0";
    dialog.lpstrFile=file; dialog.nMaxFile=ARRAYSIZE(file);
    dialog.Flags=OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR;
    if (!GetOpenFileNameW(&dialog)) return false;
    path=file; return true;
}
LRESULT CALLBACK EntryEditorProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    auto* state=(EntryEditor*)GetWindowLongPtrW(hwnd,GWLP_USERDATA);
    if (msg==WM_NCCREATE) {state=(EntryEditor*)((CREATESTRUCTW*)lp)->lpCreateParams; SetWindowLongPtrW(hwnd,GWLP_USERDATA,(LONG_PTR)state);}
    if (state && msg==WM_COMMAND) {
        auto id=LOWORD(wp);
        if (id==IDCANCEL) {state->done=true;return 0;}
        if (id==IDOK) {
            auto target=EntryText(state->fields[1]),name=EntryText(state->fields[0]);
            if (target.empty() || name.empty() || name.find_first_of(L"\t\r\n")!=std::wstring::npos) {
                MessageBoxW(hwnd,Tr(L"Enter a name and a valid target."),Tr(L"Add or edit entry"),MB_OK|MB_ICONINFORMATION);return 0;
            }
            if (!IsWebEntry(target) && !ToLower(target).starts_with(L"shell:AppsFolder\\") && GetFileAttributesW(target.c_str())==INVALID_FILE_ATTRIBUTES) {
                if (MessageBoxW(hwnd,Tr(L"The target is missing. Keep it so you can repair it later?"),Tr(L"Add or edit entry"),MB_YESNO|MB_ICONQUESTION)!=IDYES) return 0;
            }
            state->accepted=state->done=true;return 0;
        }
        if (id==30 || id==31) {
            std::wstring path;
            if (PickEntryFile(hwnd,path,id==31)) {
                SetWindowTextW(state->fields[id==30?1:4],path.c_str());
                if (id==30 && EntryText(state->fields[0]).empty()) SetWindowTextW(state->fields[0],std::filesystem::path(path).stem().c_str());
            }
            return 0;
        }
        if (id==32) {
            AppList installed; AddStartApps(IconPixels(),installed);
            HMENU menu=CreatePopupMenu(); AppList packaged;
            for (const auto& app:installed) if (IsPackagedTarget(app->target)) {
                packaged.push_back(app); AppendMenuW(menu,MF_STRING,(UINT_PTR)packaged.size(),MenuText(app->name).c_str());
            }
            RECT r{};GetWindowRect(GetDlgItem(hwnd,32),&r);
            int choice=TrackLaunchpadPopupMenu(menu,TPM_RETURNCMD|TPM_NONOTIFY,r.left,r.bottom,hwnd,nullptr);
            DestroyMenu(menu);
            if (choice>0 && choice<=(int)packaged.size()) {
                auto app=packaged[choice-1];SetWindowTextW(state->fields[0],app->name.c_str());SetWindowTextW(state->fields[1],app->target.c_str());
            }
            return 0;
        }
    }
    if (state && msg==WM_CLOSE) {state->done=true;return 0;}
    return DefWindowProcW(hwnd,msg,wp,lp);
}
void EditEntry(HWND owner,const std::shared_ptr<App>& existing=nullptr,const std::shared_ptr<Folder>& destination=nullptr) {
    if (g_entriesReadOnly || g_layoutReadOnly || g_dataReadOnly) {
        MessageBoxW(owner,Tr(L"Data is read-only. Resolve the load error before editing."),Tr(L"Add or edit entry"),MB_OK|MB_ICONWARNING);return;
    }
    EntryEditor state;state.app=existing;
    WNDCLASSEXW wc{sizeof(wc)};wc.lpfnWndProc=EntryEditorProc;wc.hInstance=(HINSTANCE)&__ImageBase;
    wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);wc.lpszClassName=kEntryEditorClass;
    RegisterClassExW(&wc);
    UINT dpi=GetDpiForWindow(owner);auto px=[&](int dip){return MulDiv(dip,dpi,96);};
    RECT bounds{};GetWindowRect(owner,&bounds);
    HWND window=CreateWindowExW(WS_EX_DLGMODALFRAME|WS_EX_CONTROLPARENT,kEntryEditorClass,Tr(L"Add or edit entry"),WS_CAPTION|WS_SYSMENU,
        (bounds.left+bounds.right-px(650))/2,(bounds.top+bounds.bottom-px(420))/2,px(650),px(420),owner,nullptr,wc.hInstance,&state);
    if (!window) return;
    HFONT font=CreateFontW(-px(14),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,g_settings.font.c_str());
    auto control=[&](PCWSTR cls,PCWSTR text,DWORD style,int x,int y,int w,int h,int id) {
        HWND c=CreateWindowExW(wcscmp(cls,L"EDIT")==0?WS_EX_CLIENTEDGE:0,cls,text,WS_CHILD|WS_VISIBLE|style,px(x),px(y),px(w),px(h),window,(HMENU)(INT_PTR)id,wc.hInstance,nullptr);
        SendMessageW(c,WM_SETFONT,(WPARAM)font,TRUE);return c;
    };
    PCWSTR labels[]={L"Display name",L"File or web address",L"Launch arguments",L"Working directory",L"Custom icon (optional)"};
    std::wstring values[5];
    if (existing) {
        values[0]=existing->name;
        values[1]=existing->target;
        values[2]=existing->arguments;
        values[3]=existing->workingDirectory;
        auto it=g_customIcons.find(AppKey(*existing));
        if(it!=g_customIcons.end())values[4]=it->second;
    }
    for (int i=0;i<5;i++) {
        control(L"STATIC",Tr(labels[i]),0,18,18+i*46,160,24,0);
        state.fields[i]=control(L"EDIT",values[i].c_str(),WS_TABSTOP|ES_AUTOHSCROLL,182,14+i*46,(i==1||i==4)?346:432,30,10+i);
        SendMessageW(state.fields[i],EM_SETLIMITTEXT,i==0?160:32760,0);
    }
    control(L"BUTTON",Tr(L"Browse…"),WS_TABSTOP,536,60,78,30,30);
    control(L"BUTTON",Tr(L"Browse…"),WS_TABSTOP,536,198,78,30,31);
    control(L"STATIC",Tr(L"Destination group"),0,18,250,160,24,0);
    state.groups=control(L"COMBOBOX",L"",WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL,182,246,432,180,20);
    int selected=0;
    if (g_settings.groups) for (const auto& tile:g_tiles) if (tile.folder) {
        if (tile.folder==destination) selected=(int)state.destinations.size();
        state.destinations.push_back(tile.folder);SendMessageW(state.groups,CB_ADDSTRING,0,(LPARAM)tile.folder->name.c_str());
    }
    if (state.destinations.empty()) SendMessageW(state.groups,CB_ADDSTRING,0,(LPARAM)Tr(L"End of current grid"));
    SendMessageW(state.groups,CB_SETCURSEL,selected,0); EnableWindow(state.groups,!existing);
    control(L"BUTTON",Tr(L"Choose Store app…"),WS_TABSTOP,18,302,190,32,32);
    control(L"BUTTON",Tr(L"Save"),WS_TABSTOP|BS_DEFPUSHBUTTON,422,302,90,32,IDOK);
    control(L"BUTTON",Tr(L"Cancel"),WS_TABSTOP,524,302,90,32,IDCANCEL);
    bool previousModal=g_modal;g_modal=true;
    EnableWindow(owner,FALSE);ShowWindow(window,SW_SHOW);SetForegroundWindow(window);SetFocus(state.fields[existing?0:1]);
    MSG message{};
    while (!state.done) {
        int result=GetMessageW(&message,nullptr,0,0);
        if(result<=0){if(!result)PostQuitMessage((int)message.wParam);break;}
        if(!IsDialogMessageW(window,&message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    std::wstring name,target,args,dir,icon;int group=0;
    if (state.accepted) {
        name=EntryText(state.fields[0]);
        target=EntryText(state.fields[1]);
        args=EntryText(state.fields[2]);
        dir=EntryText(state.fields[3]);
        icon=EntryText(state.fields[4]);
        group=(int)SendMessageW(state.groups,CB_GETCURSEL,0,0);
    }
    EnableWindow(owner,TRUE);DestroyWindow(window);DeleteObject(font);g_modal=previousModal;
    if (g_shown) ForceForeground(owner);
    if (!state.accepted) return;
    std::shared_ptr<App> app=existing;
    if (!app) {
        // Same launch target and options reuse the existing identity.
        auto found=std::find_if(g_apps.begin(),g_apps.end(),[&](const auto& a){return (IsWebEntry(target)?a->target==target:ToLower(a->target)==ToLower(target))&&a->arguments==args&&ToLower(a->workingDirectory)==ToLower(dir);});
        if (found!=g_apps.end()) app=*found;
    }
    std::wstring key;
    bool created=!app;
    if (app) key=AppKey(*app);
    else {
        GUID guid{};
        CoCreateGuid(&guid);
        WCHAR id[40]{};
        StringFromGUID2(guid,id,ARRAYSIZE(id));
        key=ToLower(L"ryjs-entry:"+std::wstring(id));
        app=std::make_shared<App>();
    }
    EntryRecord record{created||app->manual,target,name,args,dir};
    bool alreadyPlaced=false;
    for(const auto& tile:g_tiles)if(tile.app==app || (tile.folder&&IndexOf(tile.folder->apps,{app,nullptr})>=0))alreadyPlaced=true;
    auto old=g_entries;g_entries[key]=record;
    if (!SaveEntries()) {g_entries=std::move(old);MessageBoxW(owner,Tr(L"Could not save entry."),Tr(L"Add or edit entry"),MB_OK|MB_ICONERROR);return;}
    ApplyEntryRecord(*app,key,record,IconPixels());
    ApplyDisplayName(*app);
    if (created) g_apps.push_back(app);
    if (!icon.empty()) {if(LoadIconFile(icon,IconPixels(),*app)){g_customIcons[key]=icon;SaveCustomIcons();}}
    else if (g_customIcons.erase(key)) {SaveCustomIcons();}
    if (!existing && !alreadyPlaced) {
        if (g_settings.groups && group>=0 && group<(int)state.destinations.size()) MoveAppToGroup({app,nullptr},state.destinations[group],(int)state.destinations[group]->apps.size());
        else {g_hidden.erase(key);if(PinnedOnly()){g_pins.insert(key);SavePins();}if(IndexOf(g_tiles,{app,nullptr})<0)g_tiles.push_back({app,nullptr});}
        SaveTiles();
    }
    g_labelCache.clear();Refresh();
    if (IsWebEntry(target)&&!g_customIcons.count(key)) StartWebIconFetch(key,target);
}
void DeleteManualEntry(const std::shared_ptr<App>& app) {
    if (!app || !app->manual || g_entriesReadOnly || g_dataReadOnly || g_layoutReadOnly) return;
    auto key=AppKey(*app);auto previous=g_entries;g_entries.erase(key);
    if (!SaveEntries()) {g_entries=std::move(previous);return;}
    for(auto& tile:g_tiles)if(tile.folder)std::erase_if(tile.folder->apps,[&](const Tile& t){return t.app==app;});
    std::erase_if(g_tiles,[&](const Tile& t){return t.app==app;});
    std::erase(g_apps,app);g_pins.erase(key);g_hidden.erase(key);g_customIcons.erase(key);
    SavePins();SaveCustomIcons();SaveTiles();Refresh();
}

// Windows supplies PowerShell and ZipArchive. The worker is embedded in the
// single-file mod, launched without a console, and never executes archive code.
// Generated from dev/full_backup_worker.ps1 and the settings schema.
constexpr wchar_t kFullBackupScript[] = LR"RYJS_BACKUP(param([Parameter(Mandatory=$true)][string]$JobPath)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.IO.Compression.FileSystem
Add-Type -AssemblyName System.IO.Compression
$utf8 = New-Object Text.UTF8Encoding($false, $true)
$maxBytes = 256MB
$job = [IO.File]::ReadAllText($JobPath, $utf8) | ConvertFrom-Json
$work = Split-Path -Parent $JobPath

function Encode-Value([string]$value) {
    return -join ([Text.Encoding]::BigEndianUnicode.GetBytes($value) | ForEach-Object { $_.ToString('X2') })
}
function Decode-Value([string]$value) {
    if ($value -notmatch '^(?:[0-9A-Fa-f]{4})*$') { throw 'Invalid encoded setting.' }
    $bytes = New-Object byte[] ($value.Length / 2)
    for ($i=0; $i -lt $bytes.Length; $i++) { $bytes[$i] = [Convert]::ToByte($value.Substring($i*2,2),16) }
    return [Text.Encoding]::BigEndianUnicode.GetString($bytes)
}
function Hash-Bytes([byte[]]$bytes) {
    $sha = [Security.Cryptography.SHA256]::Create()
    try { return -join ($sha.ComputeHash($bytes) | ForEach-Object { $_.ToString('x2') }) }
    finally { $sha.Dispose() }
}
function Object-Map($value) {
    if ($null -eq $value -or $value -is [Array] -or $value -is [string]) { throw 'Expected an object.' }
    $map = @{}
    foreach ($p in $value.PSObject.Properties) { $map[$p.Name] = $p.Value }
    return $map
}
function Decode-Entry([string]$value) {
    if ($value -cmatch '%(?!(25|09|0A|0D))') { throw 'Invalid entry encoding.' }
    return $value.Replace('%09',"`t").Replace('%0A',"`n").Replace('%0D',"`r").Replace('%25','%')
}
function Read-Ini([byte[]]$bytes) {
    $text = if ($bytes.Length -ge 2 -and $bytes[0] -eq 255 -and $bytes[1] -eq 254) {
        [Text.Encoding]::Unicode.GetString($bytes,2,$bytes.Length-2)
    } else { $utf8.GetString($bytes).TrimStart([char]0xFEFF) }
    $ini = @{}
    $section = ''
    foreach ($line in ($text -split '\r?\n')) {
        if ($line -match '^\[([^\]]+)\]$') { $section=$matches[1]; $ini[$section]=@{} }
        elseif ($line -match '^([^=]+)=(.*)$' -and $section) { $ini[$section][$matches[1]]=$matches[2] }
    }
    return $ini
}
function Read-HostSettings {
    # The mod hands over the values it already reads through Windhawk's API.
    # This works on portable Windhawk too, and never touches its registry.
    return Object-Map $job.host_settings
}
function Read-Tree([string]$root) {
    $root=[IO.Path]::GetFullPath($root).TrimEnd('\')
    $result=@{}
    $queue=New-Object 'Collections.Generic.Queue[string]'
    $queue.Enqueue($root)
    [long]$total=0
    while ($queue.Count) {
        $directory=$queue.Dequeue()
        if ((Get-Item -LiteralPath $directory -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) { throw 'Linked data directories are unsupported.' }
        foreach ($entry in (Get-ChildItem -LiteralPath $directory -Force)) {
            if ($entry.Attributes -band [IO.FileAttributes]::ReparsePoint) { throw 'Linked data files are unsupported.' }
            if ($entry.PSIsContainer) { $queue.Enqueue($entry.FullName); continue }
            if ($entry.Name.StartsWith('performance') -or $entry.Extension -in @('.tmp','.download')) { continue }
            $total += $entry.Length
            if ($total -gt $maxBytes -or $result.Count -ge 4096) { throw 'Backup exceeds the size or file-count limit.' }
            $name='data/' + $entry.FullName.Substring($root.Length+1).Replace('\','/')
            $result[$name]=[IO.File]::ReadAllBytes($entry.FullName)
        }
    }
    if (!$result.Count) { throw 'No layout data was found.' }
    return $result
}
function Same-Files($a,$b) {
    if ($a.Count -ne $b.Count) { return $false }
    foreach ($name in $a.Keys) {
        if (!$b.ContainsKey($name) -or (Hash-Bytes $a[$name]) -ne (Hash-Bytes $b[$name])) { return $false }
    }
    return $true
}
function Safe-Entry([string]$name) {
    if (!$name -or $name.Length -gt 1024 -or $name.Contains('\') -or $name.Contains(':') -or $name.Contains([string][char]0)) { throw 'Unsafe archive path.' }
    $parts=$name.Split('/')
    if ($parts[0] -notin @('data','assets','source','binary','metadata')) { throw 'Unknown archive entry category.' }
    foreach ($part in $parts) {
        if (!$part -or $part -in @('.','..') -or $part.EndsWith('.') -or $part.EndsWith(' ') -or $part -match '[<>"|?*\x00-\x1F]' -or $part -match '^(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(?:\.|$)') { throw 'Unsafe archive filename.' }
    }
}
function Validate-Data($manifest,$files) {
    $effective=Object-Map $manifest.effective_settings
    foreach ($p in $effective.GetEnumerator()) { if ($p.Value -isnot [string]) { throw 'Invalid effective setting.' } }
    $source=$effective['appSource']
    if ($source -notin @('allApps','desktop','both','pinned')) { throw 'Unsupported application source.' }
    $prefix=if ($effective['gridLayout'] -eq 'groups') { 'groups-v1-' } else { 'layout-' }
    if (!$files.ContainsKey('data/'+$prefix+$source+'.txt')) { throw 'Active layout is missing.' }
    if ($source -eq 'pinned' -and !$files.ContainsKey('data/pins.txt')) { throw 'Pinned-app data is missing.' }
    foreach ($name in $files.Keys) {
        if ($name -match '^data/groups-' -and $name -notmatch '^data/groups-v1-') { throw 'Unsupported group layout version.' }
        if ($name -eq 'data/ryjs-storage.ini') {
            $marker=Read-Ini $files[$name]
            if ($marker['storage']['schema'] -ne '1') { throw 'Unsupported storage version.' }
        }
        if ($name -eq 'data/ui-settings.ini') {
            $ini=Read-Ini $files[$name]
            if ($ini['meta'] -and $ini['meta']['version'] -ne '1') { throw 'Unsupported private-settings version.' }
        }
        if ($name -notmatch '^data/(groups-v1-|layout-|group-geometry-|entries-v1|display-names-v1|pins\.txt|icons\.txt)' -or !$name.EndsWith('.txt')) { continue }
        $text=$utf8.GetString($files[$name]).TrimStart([char]0xFEFF)
        if ($text.Contains([string][char]0)) { throw 'Invalid text data.' }
        $rows=@($text -split '\r?\n' | Where-Object { $_ -ne '' })
        $leaf=$name.Substring(5)
        if ($leaf -match '^(groups-v1-|layout-)' -and !$rows.Count) { throw 'Empty layout file.' }
        $folder=$false
        $entryKeys=New-Object 'Collections.Generic.HashSet[string]' ([StringComparer]::Ordinal)
        $geometry=@{}
        for ($i=0; $i -lt $rows.Count; $i++) {
            $f=$rows[$i].Split("`t")
            if ($leaf.StartsWith('groups-v1-') -or $leaf.StartsWith('layout-')) {
                if ($i -eq 0 -and $leaf.StartsWith('groups-v1-')) {
                    if ($rows[$i] -ne "schema`t1`tgroups") { throw 'Unsupported group layout version.' }
                    continue
                }
                if ($f.Count -lt 2 -or !$f[1]) { throw 'Invalid layout record.' }
                if ($f[0] -eq 'folder') {
                    if ($f.Count -ne 2 -and !($f.Count -eq 3 -and $f[2] -eq 'inbox')) { throw 'Invalid folder record.' }
                    $folder=$true
                } elseif ($f[0] -eq 'in' -and $folder -and $f.Count -eq 2) { }
                elseif ($f[0] -in @('app','hidden') -and $f.Count -eq 2) { if ($f[0] -eq 'app') { $folder=$false } }
                else { throw 'Unsupported layout record.' }
            } elseif ($leaf -eq 'display-names-v1.txt') {
                if ($i -eq 0) { if ($rows[$i] -ne "schema`t1`tdisplay-names") { throw 'Unsupported display name version.' }; continue }
                if ($f.Count -ne 4 -or $f[0] -ne 'name' -or !$f[1] -or !$f[2]) { throw 'Invalid display name.' }
                foreach ($value in $f) { $null=Decode-Entry $value }
                if (!$entryKeys.Add((Decode-Entry $f[1]))) { throw 'Duplicate display name identity.' }
            } elseif ($leaf -eq 'entries-v1.txt') {
                if ($i -eq 0) { if ($rows[$i] -ne "schema`t1`tentries") { throw 'Unsupported entry version.' }; continue }
                if ($f.Count -ne 7 -or $f[0] -ne 'entry' -or $f[2] -notin @('0','1') -or !$f[1] -or !$f[3] -or !$f[4]) { throw 'Invalid application entry.' }
                foreach ($value in $f) { $null=Decode-Entry $value }
                if (!$entryKeys.Add((Decode-Entry $f[1]))) { throw 'Duplicate application identity.' }
            } elseif ($leaf.StartsWith('group-geometry-')) {
                if ($i -eq 0) {
                    $squareColumns=0
                    if ($rows[0] -match '^schema	6	group-geometry	([1-9]|[12][0-9]|30)$') { $squareColumns=[int]$Matches[1] }
                    elseif ($rows[0] -notin @("schema`t1`tgroup-geometry","schema`t2`tgroup-geometry","schema`t3`tgroup-geometry","schema`t4`tgroup-geometry","schema`t5`tgroup-geometry")) { throw 'Unsupported geometry version.' }
                    continue
                }
                if ($f.Count -ne $(if ($squareColumns -or $rows[0] -in @("schema`t4`tgroup-geometry","schema`t5`tgroup-geometry")) { 18 } elseif ($rows[0] -eq "schema`t3`tgroup-geometry") { 14 } elseif ($rows[0] -eq "schema`t2`tgroup-geometry") { 10 } else { 8 }) -or $f[0] -ne 'group' -or $f[1] -notmatch '^\d+$' -or !$f[2] -or $f[7] -notin @('group','folder')) { throw 'Invalid group geometry.' }
                foreach ($value in $f[3..6]) {
                    $number=0.0
                    if (![double]::TryParse($value,[Globalization.NumberStyles]::Float,[Globalization.CultureInfo]::InvariantCulture,[ref]$number) -or [double]::IsNaN($number) -or [double]::IsInfinity($number) -or $number -lt 0 -or $number -gt 1000000) { throw 'Invalid group dimensions.' }
                }
                if ([double]::Parse($f[5],[Globalization.CultureInfo]::InvariantCulture) -lt $(if($squareColumns){0.000001}else{48}) -or [double]::Parse($f[6],[Globalization.CultureInfo]::InvariantCulture) -lt $(if($squareColumns){0.000001}else{60})) { throw 'Group dimensions are too small.' }
                if ($f.Count -ge 10) {
                    if ($f[8] -notmatch '^\d{1,5}$' -or $f[9] -notmatch '^\d{1,5}$' -or [int]$f[8] -gt 10000 -or [int]$f[9] -gt 10000 -or (([int]$f[8] -eq 0) -ne ([int]$f[9] -eq 0))) { throw 'Invalid icon cell counts.' }
                }
                if ($f.Count -ge 14) {
                    foreach ($value in $f[10..13]) {
                        if ($value -notmatch '^\d{1,7}$' -or [int]$value -gt 1000000) { throw 'Invalid half-grid index.' }
                    }
                    $grid=@($f[10..13] | ForEach-Object { [int]$_ })
                    if ($f.Count -eq 14) {
                        if (!(($grid[2] -gt 0 -and $grid[3] -gt 0 -and [int]$f[8] -eq 0) -or ($grid | Measure-Object -Sum).Sum -eq 0)) { throw 'Invalid half-grid bounds.' }
                    } else {
                        if (($f[14] -notmatch '^[0123]$' -and !((($squareColumns -or $rows[0] -eq "schema`t5`tgroup-geometry") -and $f[14] -eq '4') -or ($squareColumns -and $f[14] -eq '5'))) -or $f[15] -notmatch '^[01]$') { throw 'Invalid geometry mode.' }
                        $body = $grid[2] -gt 0
                        if ($body -ne ($grid[3] -gt 0) -or
                            ($f[14] -eq '0' -and $body -and [int]$f[8] -ne 0) -or
                            ($f[14] -ne '0' -and $body -ne ($f[14] -in @('2','4','5'))) -or
                            ($f[14] -eq '4' -and ($f[15] -ne '1' -or $grid[3] -lt 2)) -or
                            ($f[14] -eq '5' -and ($f[15] -ne '1' -or $grid[2] -lt 2 -or $grid[2] % 2 -ne 0 -or
                                $grid[3] -lt 6 -or $grid[3] % 4 -ne 2 -or $grid[0] % (2*$squareColumns) + $grid[2] -gt 2*$squareColumns)) -or
                            (!$body -and $f[15] -eq '0' -and ($grid[0] -ne 0 -or $grid[1] -ne 0))) { throw 'Invalid half-grid bounds.' }
                        $restore=@(0.0,0.0)
                        for ($j=0;$j -lt 2;$j++) {
                            $number=0.0
                            if (![double]::TryParse($f[16+$j],[Globalization.NumberStyles]::Float,[Globalization.CultureInfo]::InvariantCulture,[ref]$number) -or [double]::IsNaN($number) -or $number -lt 0 -or $number -gt 1000000) { throw 'Invalid original group size.' }
                            $restore[$j]=$number
                        }
                        if (($restore[0] -ne 0 -or $restore[1] -ne 0) -and ($restore[0] -lt 48 -or $restore[1] -lt 60)) { throw 'Invalid original group size.' }
                    }
                }
                $ordinal=0
                if (![int]::TryParse($f[1],[ref]$ordinal) -or $geometry.ContainsKey($ordinal)) { throw 'Duplicate or invalid group ordinal.' }
                $geometry[$ordinal]=Decode-Entry $f[2]
            } elseif ($leaf -eq 'pins.txt') {
                if ($f.Count -ne 2 -or $f[0] -ne 'pin' -or !$f[1]) { throw 'Invalid pin.' }
            } elseif ($leaf -eq 'icons.txt') {
                if ($f.Count -ne 3 -or $f[0] -ne 'icon' -or !$f[1] -or !$f[2]) { throw 'Invalid icon mapping.' }
            }
        }
        if ($leaf -in @('entries-v1.txt','display-names-v1.txt') -or $leaf.StartsWith('group-geometry-')) { if (!$rows.Count) { throw 'Empty versioned data.' } }
        if ($leaf.StartsWith('group-geometry-')) {
            $layout='data/'+$leaf.Substring('group-geometry-'.Length)
            if ($effective['gridLayout'] -eq 'groups') { $layout=$layout.Replace('data/layout-','data/groups-v1-') }
            if (!$files.ContainsKey($layout)) { throw 'Group geometry has no matching layout.' }
            $ordinal=0
            $folders=@{}
            foreach ($line in ($utf8.GetString($files[$layout]).TrimStart([char]0xFEFF) -split '\r?\n')) {
                $record=$line.Split("`t")
                if ($record[0] -eq 'folder') { $folders[$ordinal]=$record[1]; $ordinal++ }
                elseif ($record[0] -eq 'app') { $ordinal++ }
            }
            if (!$squareColumns -and $rows[0] -notin @("schema`t3`tgroup-geometry","schema`t4`tgroup-geometry","schema`t5`tgroup-geometry") -and $folders.Count -ne $geometry.Count) { throw 'Group geometry is incomplete.' }
            foreach ($index in $geometry.Keys) {
                if (!$folders.ContainsKey($index) -or $geometry[$index] -cne $folders[$index]) { throw 'Group geometry does not match its layout.' }
            }
        }
    }
    $icons=Object-Map $manifest.icons
    foreach ($asset in $icons.Values) { if ($asset -isnot [string] -or !$asset.StartsWith('assets/') -or !$files.ContainsKey($asset)) { throw 'Custom icon asset is missing.' } }
    if ($files.ContainsKey('data/icons.txt')) {
        foreach ($line in ($utf8.GetString($files['data/icons.txt']) -split '\r?\n')) {
            if (!$line) { continue }
            $f=$line.Split("`t")
            if (!$icons.ContainsKey($f[2])) { throw 'Custom icon was not included in the backup.' }
        }
    }
}
function Read-Archive([string]$path) {
    if ((Get-Item -LiteralPath $path).Length -gt 320MB) { throw 'Archive exceeds size limit.' }
    $zip=[IO.Compression.ZipFile]::OpenRead($path)
    try {
        $names=New-Object 'Collections.Generic.HashSet[string]' ([StringComparer]::OrdinalIgnoreCase)
        [long]$total=0
        if ($zip.Entries.Count -gt 4096) { throw 'Too many archive entries.' }
        $files=@{}
        foreach ($entry in $zip.Entries) {
            if (!$names.Add($entry.FullName)) { throw 'Duplicate or case-conflicting archive path.' }
            if ($entry.FullName -ne 'manifest.json') { Safe-Entry $entry.FullName }
            if ((($entry.ExternalAttributes -shr 16) -band 61440) -eq 40960) { throw 'Linked archive entries are unsupported.' }
            $total += $entry.Length
            if ($entry.Length -gt $maxBytes -or $total -gt $maxBytes) { throw 'Expanded backup exceeds size limit.' }
            $stream=$entry.Open()
            $buffer=New-Object IO.MemoryStream
            try {
                $chunk=New-Object byte[] 16384
                while (($read=$stream.Read($chunk,0,$chunk.Length)) -gt 0) {
                    if ($buffer.Length+$read -gt $entry.Length -or $buffer.Length+$read -gt $maxBytes) { throw 'Expanded entry exceeds its declared size.' }
                    $buffer.Write($chunk,0,$read)
                }
                if ($buffer.Length -ne $entry.Length) { throw 'Archive entry length mismatch.' }
                $files[$entry.FullName]=$buffer.ToArray()
            } finally { $stream.Dispose(); $buffer.Dispose() }
        }
        if (!$files.ContainsKey('manifest.json')) { throw 'This is not a complete Launchpad backup.' }
        $manifest=$utf8.GetString($files['manifest.json']) | ConvertFrom-Json
        if ($manifest.schema -ne 1) { throw 'Unsupported backup version.' }
        if ($manifest.source_mod -notin @('local@ryjs-launchpad','ryjs-launchpad','local@seamless-launchpad-custom','seamless-launchpad-custom','local@seamless-launchpad','seamless-launchpad','local@fullscreen-start-plus','fullscreen-start-plus')) { throw 'Backup belongs to a different application.' }
        $expected=Object-Map $manifest.files
        if ($expected.Count+1 -ne $files.Count) { throw 'Archive entries differ from manifest.' }
        foreach ($name in $expected.Keys) {
            Safe-Entry $name
            if (!$files.ContainsKey($name) -or $files[$name].Length -ne $expected[$name].size -or (Hash-Bytes $files[$name]) -cne $expected[$name].sha256) { throw 'Backup checksum mismatch.' }
        }
        $files.Remove('manifest.json')
        Validate-Data $manifest $files
        return @{ manifest=$manifest; files=$files; hash=(Hash-Bytes ([IO.File]::ReadAllBytes($path))) }
    } finally { $zip.Dispose() }
}
function Export-Complete {
    $archivePath=[IO.Path]::GetFullPath($job.archive)
    $storagePath=[IO.Path]::GetFullPath($job.storage).TrimEnd('\')+'\'
    if ($archivePath.StartsWith($storagePath,[StringComparison]::OrdinalIgnoreCase) -or [IO.Path]::GetExtension($archivePath) -ne '.zip') { throw 'Choose a ZIP file outside application data.' }
    $files=Read-Tree $job.storage
    $snapshot=@{} + $files
    $hostSettings=Read-HostSettings
    $effective=Object-Map $job.defaults
    foreach ($key in $hostSettings.Keys) { $effective[$key]=[string]$hostSettings[$key] }
    if ($files.ContainsKey('data/ui-settings.ini')) {
        $ini=Read-Ini $files['data/ui-settings.ini']
        if ($ini['meta']['version'] -ne '1') { throw 'Unsupported private-settings version.' }
        foreach ($key in $ini['present'].Keys) {
            if ($ini['present'][$key] -ne '1') { continue }
            $baseline=if ($hostSettings.ContainsKey($key)) { [string]$hostSettings[$key] } else { '' }
            if ((Decode-Value $ini['host'][$key]) -ceq $baseline) { $effective[$key]=Decode-Value $ini['values'][$key] }
        }
    }
    $icons=@{}
    if ($files.ContainsKey('data/icons.txt')) {
        foreach ($line in ($utf8.GetString($files['data/icons.txt']) -split '\r?\n')) {
            if (!$line) { continue }
            $f=$line.Split("`t")
            if ($f.Count -ne 3 -or $f[0] -ne 'icon') { throw 'Invalid custom icon mapping.' }
            $blob=[IO.File]::ReadAllBytes($f[2])
            $name='assets/'+(Hash-Bytes $blob)+[IO.Path]::GetExtension($f[2]).ToLowerInvariant()
            $files[$name]=$blob
            $icons[$f[2]]=$name
        }
    }
    $instructions=@'
RYJS Launchpad 完整备份
在启动器菜单选择“导入完整备份”，选中本 ZIP，校验后确认即可。
导入仅恢复布局、设置和图标；source/binary 仅用于兼容旧备份，不会安装或执行。
替换前的数据保留在原 data-v1 同级的 .before-import-* 目录。
'@
    $files['metadata/恢复说明.txt']=$utf8.GetBytes($instructions)
    $manifest=[ordered]@{schema=1; producer='ryjs-native-1'; source_mod=$job.mod_id; version=$job.version; created=[DateTimeOffset]::Now.ToString('o'); host_settings=$hostSettings; effective_settings=$effective; icons=$icons; files=@{}}
    foreach ($name in $files.Keys) { $manifest.files[$name]=@{size=$files[$name].Length;sha256=(Hash-Bytes $files[$name])} }
    Validate-Data ($manifest | ConvertTo-Json -Depth 8 | ConvertFrom-Json) $files
    if (!(Same-Files $snapshot (Read-Tree $job.storage))) { throw 'Layout changed during backup. Please retry.' }
    $temporary=$job.archive+'.pending-'+[Guid]::NewGuid().ToString('N')
    try {
        $stream=[IO.File]::Open($temporary,[IO.FileMode]::CreateNew)
        $zip=$null
        try {
            $zip=New-Object IO.Compression.ZipArchive($stream,[IO.Compression.ZipArchiveMode]::Create)
            $files['manifest.json']=$utf8.GetBytes(($manifest | ConvertTo-Json -Depth 8))
            foreach ($name in ($files.Keys | Sort-Object)) {
                $entry=$zip.CreateEntry($name,[IO.Compression.CompressionLevel]::Optimal)
                $output=$entry.Open()
                try { $output.Write($files[$name],0,$files[$name].Length) } finally { $output.Dispose() }
            }
        } finally { if($zip){$zip.Dispose()}; $stream.Dispose() }
        $verified=Read-Archive $temporary
        if (Test-Path -LiteralPath $job.archive) { [IO.File]::Replace($temporary,$job.archive,$null) }
        else { [IO.File]::Move($temporary,$job.archive) }
        return @{ summary=$job.archive; hash=$verified.hash; previous='' }
    } finally { if (Test-Path -LiteralPath $temporary) { try { Remove-Item -LiteralPath $temporary -Force } catch {} } }
}
function Import-Complete($checked) {
    if (!$job.expected_hash -or $checked.hash -cne $job.expected_hash) { throw 'Archive changed after confirmation.' }
    $destination=[IO.Path]::GetFullPath($job.storage).TrimEnd('\')
    if (!$job.preview) {
        $parent=Split-Path -Parent $destination
        if ([IO.Path]::GetFileName($destination) -ne 'data-v1' -or [IO.Path]::GetFileName($parent) -notin @('local@ryjs-launchpad','ryjs-launchpad')) { throw 'Import is restricted to RYJS storage.' }
        if (!(Test-Path -LiteralPath (Join-Path $destination 'ryjs-storage.ini'))) { throw 'Existing RYJS storage marker is missing.' }
    }
    $ancestor=$destination
    while ($ancestor) {
        if ((Test-Path -LiteralPath $ancestor) -and ((Get-Item -LiteralPath $ancestor -Force).Attributes -band [IO.FileAttributes]::ReparsePoint)) { throw 'Import destination contains a link.' }
        $ancestor=Split-Path -Parent $ancestor
    }
    $hostSettings=Read-HostSettings
    $stage=$destination+'.restore-'+[Guid]::NewGuid().ToString('N')
    [IO.Directory]::CreateDirectory($stage) | Out-Null
    foreach ($name in $checked.files.Keys) {
        if ($name.StartsWith('data/')) { $relative=$name.Substring(5) }
        elseif ($name.StartsWith('assets/')) { $relative=$name }
        else { continue } # Source, DLLs and registry exports are never installed or executed.
        $target=[IO.Path]::GetFullPath((Join-Path $stage $relative))
        if (!$target.StartsWith($stage+'\',[StringComparison]::OrdinalIgnoreCase)) { throw 'Import path escaped staging.' }
        [IO.Directory]::CreateDirectory((Split-Path -Parent $target)) | Out-Null
        [IO.File]::WriteAllBytes($target,$checked.files[$name])
    }
    $icons=Object-Map $checked.manifest.icons
    if ($icons.Count) {
        $iconFile=Join-Path $stage 'icons.txt'
        $lines=foreach ($line in ([IO.File]::ReadAllText($iconFile,$utf8) -split '\r?\n')) {
            if (!$line) { continue }
            $f=$line.Split("`t")
            if ($icons.ContainsKey($f[2])) { $f[2]=Join-Path $destination $icons[$f[2]] }
            $f -join "`t"
        }
        [IO.File]::WriteAllText($iconFile,($lines -join "`r`n")+"`r`n",$utf8)
    }
    $effective=Object-Map $checked.manifest.effective_settings
    $ui=Join-Path $stage 'ui-settings.ini'
    if (Test-Path -LiteralPath $ui) { [IO.File]::Copy($ui,(Join-Path $stage 'import-original-ui-settings.ini'),$true) }
    $text="[meta]`r`nversion=1`r`n"
    foreach ($section in @('host','values','present')) {
        $text+='['+$section+"]`r`n"
        foreach ($key in ($effective.Keys | Sort-Object)) {
            if ($key -match '[\r\n=\[\]]') { throw 'Invalid setting name.' }
            $value=if ($section -eq 'present') { '1' } elseif ($section -eq 'values') { Encode-Value ([string]$effective[$key]) } else { Encode-Value ([string]$hostSettings[$key]) }
            $text+=$key+'='+$value+"`r`n"
        }
    }
    [IO.File]::WriteAllText($ui,$text,[Text.Encoding]::Unicode)
    [IO.File]::WriteAllText((Join-Path $stage 'ryjs-storage.ini'),"[storage]`r`nschema=1`r`n",[Text.Encoding]::Unicode)
    $previous=$destination+'.before-import-'+[DateTime]::Now.ToString('yyyyMMdd-HHmmss')+'-'+[Guid]::NewGuid().ToString('N')
    $hadPrevious=Test-Path -LiteralPath $destination
    if ($hadPrevious) { [IO.Directory]::Move($destination,$previous) }
    try { [IO.Directory]::Move($stage,$destination) }
    catch {
        if ($hadPrevious -and !(Test-Path -LiteralPath $destination)) { [IO.Directory]::Move($previous,$destination) }
        throw
    }
    return @{summary=$destination; hash=$checked.hash; previous=$(if($hadPrevious){$previous}else{''})}
}
try {
    if ($job.mode -eq 'export') { $result=Export-Complete }
    elseif ($job.mode -in @('inspect','import')) {
        $checked=Read-Archive $job.archive
        if ($job.mode -eq 'import') { $result=Import-Complete $checked }
        else {
            $source=[string]$checked.manifest.source_mod
            $version=[string]$checked.manifest.version
            $result=@{summary=($source+' '+$version+"`r`n"+$checked.files.Count+' files');hash=$checked.hash;previous=''}
        }
    } else { throw 'Unknown backup operation.' }
    $resultText="[result]`r`nstatus=ok`r`nsummary="+(Encode-Value $result.summary)+"`r`nhash="+$result.hash+"`r`nprevious="+(Encode-Value $result.previous)+"`r`n"
} catch {
    $resultText="[result]`r`nstatus=error`r`nsummary="+(Encode-Value $_.Exception.Message)+"`r`n"
}
[IO.File]::WriteAllText((Join-Path $work 'result.ini'),$resultText,[Text.Encoding]::Unicode)
)RYJS_BACKUP";
constexpr wchar_t kFullBackupDefaults[] = LR"RYJS_DEFAULTS({"interfaceLanguage": "auto", "uiFont": "auto", "winKeyAction": "grid", "shiftWinKeyAction": "start", "startButtonAction": "grid", "program": "", "monitor": "mouse", "appSource": "allApps", "gridLayout": "groups", "gridColumns": "0", "globalColumns": "0", "coverScreen": "0", "gridIconSize": "56", "showLabels": "1", "normalizeIcons": "1", "blurMode": "custom", "blurAmount": "60", "backgroundOpacity": "30", "tintColor": "#000000", "glowStrength": "60", "chromeOpacity": "90", "powerBar": "1", "dockAlignment": "center", "powerButtons.lock": "1", "powerButtons.signOut": "1", "powerButtons.sleep": "1", "powerButtons.hibernate": "1", "powerButtons.restart": "1", "powerButtons.shutDown": "1", "powerNoConfirm": "0", "wiggle": "1", "holdTime": "420", "animations": "1", "settingsGlass": "1", "nativeSettingsTitleBar": "0", "settingsTrayIcon": "1", "gpuPresenter": "1", "performanceLogging": "0", "frameRateLimit": "0", "openDuration": "280", "closeDuration": "180", "animationEasing": "settle", "reducedMotion": "0", "labelFontSize": "13", "horizontalSpacing": "56", "verticalSpacing": "16", "groupWidth": "0", "groupSpacing": "32", "groupFlow": "columns", "groupNavigation": "scroll", "folderDuration": "180", "themeMode": "custom", "backgroundDim": "0", "launchpadAnimation": "iconFlyIn"})RYJS_DEFAULTS";

constexpr UINT kFullBackupDone=WM_APP+65;
struct FullBackupJob {
    HANDLE process=nullptr,thread=nullptr;
    std::wstring work,mode,archive;
};
[[clang::no_destroy]] std::unique_ptr<FullBackupJob> g_fullBackupJob;

std::wstring BackupJsonString(const std::wstring& value) {
    std::wstring out=L"\"";
    for(WCHAR c:value) {
        if(c==L'"'||c==L'\\'){out+=L'\\';out+=c;}
        else if(c<32){WCHAR escaped[8]{};swprintf_s(escaped,L"\\u%04x",(unsigned)c);out+=escaped;}
        else out+=c;
    }
    return out+L"\"";
}
bool BackupWriteUtf8(const std::wstring& path,const std::wstring& text) {
    int size=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text.data(),(int)text.size(),nullptr,0,nullptr,nullptr);
    if(size<=0)return false;
    std::string bytes(size,'\0');
    WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text.data(),(int)text.size(),bytes.data(),size,nullptr,nullptr);
    HANDLE file=CreateFileW(path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(file==INVALID_HANDLE_VALUE)return false;
    DWORD written=0;
    bool ok=WriteFile(file,bytes.data(),(DWORD)bytes.size(),&written,nullptr)&&written==bytes.size();
    CloseHandle(file);return ok;
}
std::wstring BackupReadResult(const std::wstring& work,PCWSTR key,bool encoded=true) {
    auto value=launchpad_settings_store::Read(work+L"\\result.ini",L"result",key);
    if(!encoded)return value;
    std::wstring decoded;
    return launchpad_settings_store::Decode(value,decoded)?decoded:L"";
}
std::wstring BackupWorkRoot() {
    return ModTempPath();
}
void BackupDeleteWork(const std::wstring& work) {
    auto root = BackupWorkRoot();
    if (root.empty() || work.empty()) return;
    auto parent = std::filesystem::path(work).parent_path().lexically_normal().wstring();
    auto expected = std::filesystem::path(root).lexically_normal().wstring();
    if (_wcsicmp(parent.c_str(), expected.c_str()) ||
        !std::filesystem::path(work).filename().wstring().starts_with(L"RYJSBackup-")) return;
    std::error_code error;
    std::filesystem::remove_all(work, error);
}
DWORD WINAPI FullBackupWait(void* parameter) {
    auto* job=(FullBackupJob*)parameter;
    WaitForSingleObject(job->process,INFINITE);
    HWND window=g_wnd;
    if(window)PostMessageW(window,kFullBackupDone,0,0);
    return 0;
}
void StopFullBackupJob() {
    if(!g_fullBackupJob)return;
    WaitForSingleObject(g_fullBackupJob->process,INFINITE);
    if(g_fullBackupJob->thread){WaitForSingleObject(g_fullBackupJob->thread,INFINITE);CloseHandle(g_fullBackupJob->thread);}
    CloseHandle(g_fullBackupJob->process);
    BackupDeleteWork(g_fullBackupJob->work);
    g_fullBackupJob.reset();g_fullBackupBusy=false;
}
bool StartFullBackupJob(const std::wstring& mode,const std::wstring& archive,const std::wstring& expected=L"") {
    if(g_fullBackupJob)return false;
    auto job=std::make_unique<FullBackupJob>();job->mode=mode;job->archive=archive;
    WCHAR id[40]{},system[32768]{};
    GUID guid{};if(FAILED(CoCreateGuid(&guid)))return false;
    StringFromGUID2(guid,id,ARRAYSIZE(id));
    auto workRoot=BackupWorkRoot();
    if(workRoot.empty()||!GetSystemDirectoryW(system,ARRAYSIZE(system)))return false;
    SHCreateDirectoryExW(nullptr,workRoot.c_str(),nullptr);
    job->work=workRoot+L"\\RYJSBackup-"+id;
    if(!CreateDirectoryW(job->work.c_str(),nullptr))return false;
#ifdef SEAMLESS_PREVIEW
    std::wstring preview=L"true";
    std::wstring modId=L"ryjs-launchpad";
#else
    std::wstring preview=L"false";
    std::wstring modId=L"ryjs-launchpad";
    WCHAR storage[32768]{};
    if (Wh_GetModStoragePath(storage, ARRAYSIZE(storage)))
        modId = std::filesystem::path(storage).filename().wstring();
#endif
    std::wstring config=L"{\"mode\":"+BackupJsonString(mode)+L",\"archive\":"+BackupJsonString(archive)+
        L",\"expected_hash\":"+BackupJsonString(expected)+L",\"storage\":"+BackupJsonString(DataRoot())+
        L",\"mod_id\":"+BackupJsonString(modId)+L",\"version\":"+BackupJsonString(WH_MOD_VERSION)+
        L",\"preview\":"+preview;
    std::wstring host=L"{";
    bool first=true;
    for(const auto& field:Set_MakeFields()) {
        if(!first)host+=L",";
        first=false;
#ifdef SEAMLESS_PREVIEW
        auto baseline=Preview::Setting(field.key);
#else
        std::wstring baseline=WindhawkUtils::StringSetting::make(field.key).get();
#endif
        host+=BackupJsonString(field.key)+L":"+BackupJsonString(baseline);
    }
    // A legacy density setting the built-in window hides: the backup still
    // carries it so an imported profile keeps its saved column count.
    if(!first)host+=L",";
#ifdef SEAMLESS_PREVIEW
    std::wstring columns=Preview::Setting(L"gridColumns");
#else
    std::wstring columns=WindhawkUtils::StringSetting::make(L"gridColumns").get();
#endif
    host+=BackupJsonString(L"gridColumns")+L":"+BackupJsonString(columns);
    config+=L",\"defaults\":"+std::wstring(kFullBackupDefaults)+L",\"host_settings\":"+host+L"}}";
    auto script=job->work+L"\\worker.ps1",jobFile=job->work+L"\\job.json";
    if(!BackupWriteUtf8(script,L"\uFEFF"+std::wstring(kFullBackupScript))||!BackupWriteUtf8(jobFile,config)) {
        BackupDeleteWork(job->work);return false;
    }
    auto executable=std::wstring(system)+L"\\WindowsPowerShell\\v1.0\\powershell.exe";
    auto command=L"\""+executable+L"\" -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File \""+script+L"\" -JobPath \""+jobFile+L"\"";
    STARTUPINFOW startup{sizeof(startup)};startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
    PROCESS_INFORMATION process{};
    if(!CreateProcessW(executable.c_str(),command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,job->work.c_str(),&startup,&process)) {
        BackupDeleteWork(job->work);return false;
    }
    CloseHandle(process.hThread);job->process=process.hProcess;
    job->thread=CreateThread(nullptr,0,FullBackupWait,job.get(),0,nullptr);
    if(!job->thread) {
        WaitForSingleObject(job->process,INFINITE);CloseHandle(job->process);BackupDeleteWork(job->work);return false;
    }
    g_fullBackupJob=std::move(job);g_fullBackupBusy=true;
    if(g_shown)Render();return true;
}
void BackupResumeAfterImport(bool succeeded) {
    g_fullImportBusy=false;g_workerStop=false;g_webStop=false;
    if(succeeded) {
        WritePrivateProfileStringW(nullptr,nullptr,nullptr,(DataRoot()+L"\\ui-settings.ini").c_str());
#ifdef SEAMLESS_PREVIEW
        for(const auto& field:Set_MakeFields()) {
            auto stored=launchpad_settings_store::Read(DataRoot()+L"\\ui-settings.ini",L"values",field.key);
            std::wstring value;if(!stored.empty()&&launchpad_settings_store::Decode(stored,value))Preview::PutSetting(field.key,value);
        }
#endif
        g_dataReadOnly=g_layoutReadOnly=g_entriesReadOnly=false;
        g_appsKey=-1;g_appsLoaded=false;g_appsTime=0;
        LoadSettings();g_labelFormat.Reset();StartLoadingApps();Open();
    }
}
bool BackupCloseSettings() {
    g_settingsStop=true;
    if(HWND window=g_settingsWnd)PostMessageW(window,WM_CLOSE,0,0);
    if(g_settingsThread) {
        if(WaitForSingleObject(g_settingsThread,15000)!=WAIT_OBJECT_0)return false;
        CloseHandle(g_settingsThread);g_settingsThread=nullptr;
    }
    return true;
}
void OnFullBackupDone(HWND owner) {
    if(!g_fullBackupJob)return;
    auto mode=g_fullBackupJob->mode,archive=g_fullBackupJob->archive;
    bool ok=BackupReadResult(g_fullBackupJob->work,L"status",false)==L"ok";
    auto summary=BackupReadResult(g_fullBackupJob->work,L"summary");
    auto hash=BackupReadResult(g_fullBackupJob->work,L"hash",false);
    auto previous=BackupReadResult(g_fullBackupJob->work,L"previous");
    StopFullBackupJob();
    if(summary.empty())summary=Tr(L"The backup helper did not return a result.");
    bool oldModal=g_modal;g_modal=true;
    if(!ok) {
        MessageBoxW(owner,(std::wstring(Tr(L"Complete backup/import failed:"))+L"\n\n"+summary).c_str(),Tr(L"Backup and restore"),MB_OK|MB_ICONERROR);
        if(mode==L"import")BackupResumeAfterImport(false);
    } else if(mode==L"inspect") {
        auto message=std::wstring(Tr(L"Backup verified:"))+L"\n"+summary+L"\n\n"+
            Tr(L"Replace the current layout, icons and settings? The previous data will be kept for rollback.");
        if(MessageBoxW(owner,message.c_str(),Tr(L"Import complete backup"),MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDYES) {
            if(!BackupCloseSettings()) {
                MessageBoxW(owner,Tr(L"Close the settings window and try importing again."),Tr(L"Backup and restore"),MB_OK|MB_ICONERROR);
                g_modal=oldModal;return;
            }
            HideNow();g_fullImportBusy=true;
            WritePrivateProfileStringW(nullptr,nullptr,nullptr,(DataRoot()+L"\\ui-settings.ini").c_str());
            g_workerStop=true;
            if(g_worker){WaitForSingleObject(g_worker,INFINITE);CloseHandle(g_worker);g_worker=nullptr;}
            MSG queued{};
            while(PeekMessageW(&queued,nullptr,WM_APP_APPS,WM_APP_APPS,PM_REMOVE))delete (AppList*)queued.lParam;
            StopWebIcons();
            if(!StartFullBackupJob(L"import",archive,hash)) {
                BackupResumeAfterImport(false);
                MessageBoxW(owner,Tr(L"Could not start the backup helper."),Tr(L"Backup and restore"),MB_OK|MB_ICONERROR);
            }
        }
    } else {
        auto message=std::wstring(Tr(mode==L"export"?L"Complete backup saved and verified:":L"Complete backup imported:"))+L"\n\n"+summary;
        if(!previous.empty())message+=L"\n\n"+std::wstring(Tr(L"Previous data:"))+L"\n"+previous;
        MessageBoxW(owner,message.c_str(),Tr(L"Backup and restore"),MB_OK|MB_ICONINFORMATION);
        if(mode==L"import")BackupResumeAfterImport(true);
    }
    g_modal=oldModal;if(g_shown)Render();
}
void ShowFullBackupDialog(HWND owner,bool importing) {
    if(g_fullBackupJob) {
        MessageBoxW(owner,Tr(L"A backup or import is already running."),Tr(L"Backup and restore"),MB_OK|MB_ICONINFORMATION);return;
    }
    WCHAR file[32768]{};
    if(!importing) {
        SYSTEMTIME now{};GetLocalTime(&now);
        swprintf_s(file,L"RYJS-Launchpad-%04u%02u%02u-%02u%02u%02u.ryjs.zip",now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond);
    }
    PWSTR desktop=nullptr;SHGetKnownFolderPath(FOLDERID_Desktop,0,nullptr,&desktop);
    auto filter=std::wstring(Tr(L"Complete Launchpad backups"))+L'\0'+L"*.zip"+L'\0'+L'\0';
    OPENFILENAMEW dialog{sizeof(dialog)};dialog.hwndOwner=owner;dialog.lpstrFile=file;dialog.nMaxFile=ARRAYSIZE(file);
    dialog.lpstrFilter=filter.c_str();dialog.lpstrDefExt=L"zip";dialog.lpstrInitialDir=desktop;
    dialog.lpstrTitle=Tr(importing?L"Import complete backup":L"Create complete backup");
    dialog.Flags=OFN_NOCHANGEDIR|OFN_EXPLORER|(importing?OFN_FILEMUSTEXIST:OFN_OVERWRITEPROMPT);
    bool oldModal=g_modal;g_modal=true;
    BOOL selected=importing?GetOpenFileNameW(&dialog):GetSaveFileNameW(&dialog);
    CoTaskMemFree(desktop);g_modal=oldModal;
    if(!selected)return;
    if(!importing && !g_layoutReadOnly && !g_dataReadOnly && !SaveTiles()) {
        MessageBoxW(owner,Tr(L"Could not save the current layout before backup."),Tr(L"Backup and restore"),MB_OK|MB_ICONERROR);return;
    }
    if(!StartFullBackupJob(importing?L"inspect":L"export",file))MessageBoxW(owner,Tr(L"Could not start the backup helper."),Tr(L"Backup and restore"),MB_OK|MB_ICONERROR);
}


void ShowTileMenu(HWND hwnd, int index, POINT pt) {
    enum {
        kOpen = 1,
        kRunAs,
        kFileLocation,
        kShortcutLocation,
        kChangeIcon,
        kRestoreIcon,
        kOutOfFolder,
        kRemove,
        kProperties,
        kUngroup,
        kPin,
    };
    Tile tile = Items()[index];
    g_selected = index;
    Render();
    HMENU menu = CreatePopupMenu();
    AppPaths paths;
    std::wstring key;
    if (tile.folder) {
        AppendMenuW(menu, MF_STRING, kOpen, Tr(L"Open"));
        AppendMenuW(menu, MF_STRING, kUngroup, Tr(L"Take the apps out"));
    } else {
        paths = GetAppPaths(*tile.app);
        key = AppKey(*tile.app);
        AppendMenuW(menu, MF_STRING, kOpen, Tr(L"Open"));
        if (g_settings.groups) {
            HMENU groups = CreatePopupMenu();
            for (int i = 0; i < (int)g_tiles.size(); i++) if (g_tiles[i].folder)
                AppendMenuW(groups, MF_STRING, 1000+i, MenuText(g_tiles[i].folder->name).c_str());
            AppendMenuW(menu, MF_POPUP, (UINT_PTR)groups, Tr(L"Move to group"));
            AppendMenuW(menu, MF_STRING, 91, Tr(L"Remove from group"));
        }
        AppendMenuW(menu, MF_STRING, 95, Tr(L"Rename"));
        if(g_displayNames.count(key)&&!g_displayNames[key].custom.empty())AppendMenuW(menu,MF_STRING,96,Tr(L"Restore original name"));
        AppendMenuW(menu, MF_STRING, 92, Tr(L"Edit entry…"));
        if (tile.app->manual) AppendMenuW(menu, MF_STRING, 93, Tr(L"Delete manual entry"));
        if (IsWebEntry(tile.app->target)) AppendMenuW(menu, MF_STRING, 94, Tr(L"Refresh website icon"));
        // Anything with a launchable file can be elevated: a classic .exe,
        // any Start-menu shortcut, or a packaged app through a temporary
        // .lnk aimed at its AppsFolder target.
        if (!paths.shortcut.empty() || !paths.folder.empty() ||
            IsPackagedTarget(tile.app->target) ||
            (!paths.file.empty() && EndsWith(ToLower(paths.file), L".exe"))) {
            AppendMenuW(menu, MF_STRING, kRunAs, Tr(L"Run as administrator"));
        }
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu,
                    MF_STRING | (paths.file.empty() && paths.folder.empty()
                                     ? MF_GRAYED
                                     : 0),
                    kFileLocation, Tr(L"Open file location"));
        AppendMenuW(menu, MF_STRING | (paths.shortcut.empty() ? MF_GRAYED : 0),
                    kShortcutLocation, Tr(L"Open shortcut location"));
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu, MF_STRING, kChangeIcon, Tr(L"Change icon\u2026"));
        if (g_customIcons.count(key)) {
            AppendMenuW(menu, MF_STRING, kRestoreIcon,
                        Tr(L"Restore original icon"));
        }
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        if (g_openFolder) {
            AppendMenuW(menu, MF_STRING, kOutOfFolder, Tr(L"Take out of folder"));
        }
        if (!PinnedOnly()) {
            AppendMenuW(menu, MF_STRING, kRemove, Tr(L"Remove from this grid"));
        } else if (g_pins.count(key)) {
            AppendMenuW(menu, MF_STRING, kRemove, Tr(L"Unpin from this grid"));
        } else {
            AppendMenuW(menu, MF_STRING, kPin, Tr(L"Pin to this grid"));
        }
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(menu,
                    MF_STRING | (paths.shortcut.empty() && paths.file.empty()
                                     ? MF_GRAYED
                                     : 0),
                    kProperties, Tr(L"Properties"));
    }
    SetMenuDefaultItem(menu, kOpen, FALSE);
    g_modal = true;
    int command = TrackLaunchpadPopupMenu(
        menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY, pt.x, pt.y, hwnd,
        nullptr);
    g_modal = false;
    DestroyMenu(menu);

    int size = g_appsKey & 0xFFFF;
    if(tile.app&&(command==95||command==96)) {
        auto name=command==95?PromptGroupName(hwnd,tile.app->name,L"Application name",160):std::wstring();
        if(command==95&&name.empty())return;
        if(!SetDisplayName(tile.app,name))MessageBoxW(hwnd,Tr(L"Could not save entry."),Tr(L"Application name"),MB_OK|MB_ICONWARNING);
        g_labelCache.clear();Refresh();return;
    }
    if (tile.app && command==92) {EditEntry(hwnd,tile.app);return;}
    if (tile.app && command==93) {DeleteManualEntry(tile.app);return;}
    if (tile.app && command==94) {g_customIcons.erase(AppKey(*tile.app));SaveCustomIcons();StartWebIconFetch(AppKey(*tile.app),tile.app->target);return;}
    if (g_settings.groups && tile.app && (command == 91 || (command >= 1000 && command < 1000 + (int)g_tiles.size()))) {
        auto destination = command == 91 ? InboxGroup() : g_tiles[command - 1000].folder;
        MoveAppToGroup(tile, destination, destination ? (int)destination->apps.size() : 0);
        SaveTiles(); Refresh(); return;
    }
    const std::wstring& linkOrFile =
        paths.shortcut.empty() ? paths.file : paths.shortcut;
    switch (command) {
        case kOpen:
            if (tile.folder) {
                OpenFolder(tile.folder);
            } else {
                LaunchTarget(tile.app->target,nullptr,tile.app.get());
            }
            break;
        case kUngroup:
            Ungroup(tile.folder);
            break;
        case kRunAs:
            // A Store app's .lnk brokers an elevated launch; the raw
            // AppsFolder target — with or without a real exe inside it — is
            // a silent no-op. Packaged apps go through the temporary .lnk.
            if (IsPackagedTarget(tile.app->target)) {
                ElevateViaTempLnk(tile.app->target);
            } else {
                LaunchTarget(linkOrFile, L"runas",tile.app.get());
            }
            break;
        case kFileLocation:
            if (paths.file.empty()) {
                LaunchTarget(paths.folder);
            } else {
                Close();
                ShowInFolder(paths.file);
            }
            break;
        case kShortcutLocation:
            Close();
            ShowInFolder(paths.shortcut);
            break;
        case kChangeIcon: {
            std::wstring picture;
            if (PickIconFile(hwnd, picture)) {
                if (LoadIconFile(picture, size, *tile.app)) {
                    g_customIcons[key] = picture;
                    SaveCustomIcons();
                } else {
                    Wh_Log(L"No picture in %s", picture.c_str());
                }
            }
            Render();
            break;
        }
        case kRestoreIcon:
            g_customIcons.erase(key);
            SaveCustomIcons();
            ReloadAppIcon(size, *tile.app);
            Render();
            break;
        case kOutOfFolder:
            TakeOutOfFolder(tile);
            break;
        case kRemove:
            RemoveApp(tile.app);
            break;
        case kPin:
            if(g_settings.groups) {
                auto destination=InboxGroup();
                MoveAppToGroup(tile,destination,(int)destination->apps.size());
            } else {
                g_pins.insert(key);
                SavePins();
                g_hidden.erase(key);
                g_tiles.push_back(tile);
            }
            SaveTiles();
            Refresh();
            break;
        case kProperties:
            Close();
            SHObjectProperties(nullptr, SHOP_FILEPATH, linkOrFile.c_str(),
                               nullptr);
            break;
    }
}

// The menu key: the menu of the selected icon.
void ShowSelectedMenu() {
    if (g_drag.active || g_selected < 0 || g_selected >= (int)Items().size()) {
        return;
    }
    Cell c = ItemCell(g_selected);
    POINT pt{(LONG)c.icon.left, (LONG)c.icon.bottom};
    ClientToScreen(g_wnd, &pt);
    ShowTileMenu(g_wnd, g_selected, pt);
}

// The "..." menu, below the button, or at the pointer after a right-click on
// empty space.
void ShowOptionsMenu(HWND hwnd, POINT pt) {
    enum { kOpenSettings = 1, kRestoreAll, kSort, kRestoreFirst = 100 };
    // Removed apps that are still installed, A to Z like the list.
    std::vector<std::shared_ptr<App>> removed;
    for (const auto& app : g_apps) {
        if (g_hidden.count(AppKey(*app)) && removed.size() < 500) {
            removed.push_back(app);
        }
    }
    HMENU menu = CreatePopupMenu();
    #ifdef SEAMLESS_PREVIEW
    AppendMenuW(menu, MF_STRING, 90, Tr(L"Exit preview"));
#endif
    AppendMenuW(menu, MF_STRING, kOpenSettings, Tr(L"Open settings"));
    AppendMenuW(menu, MF_STRING, 54, Tr(L"Add entry…"));
    AppendMenuW(menu, MF_STRING, 56, Tr(L"Create complete backup…"));
    AppendMenuW(menu, MF_STRING, 57, Tr(L"Import complete backup…"));
    if(g_settings.groups)AppendMenuW(menu,MF_STRING,55,Tr(g_groupEditMode?L"Finish group editing":L"Edit group layout"));
    if(g_groupEditMode)AppendMenuW(menu,MF_STRING,58,Tr(L"Cancel group editing"));
    if (g_settings.groups) AppendMenuW(menu, MF_STRING, 50, Tr(L"New group"));
    AppendMenuW(menu, MF_STRING, 51, Tr(L"Restore layout backup"));
    if(g_settings.groups&&GetFileAttributesW((GroupPositionFile()+L".bak").c_str())!=INVALID_FILE_ATTRIBUTES)
        AppendMenuW(menu,MF_STRING,59,Tr(L"Restore previous group geometry"));
    AppendMenuW(menu, MF_STRING, 52, Tr(L"Import layout file"));
    AppendMenuW(menu, MF_STRING, 53, Tr(L"Export layout file"));
    HMENU sub = CreatePopupMenu();
    for (int i = 0; i < (int)removed.size(); i++) {
        AppendMenuW(sub, MF_STRING, kRestoreFirst + i,
                    MenuText(removed[i]->name).c_str());
    }
    if (!removed.empty()) {
        AppendMenuW(sub, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(sub, MF_STRING, kRestoreAll, Tr(L"Bring them all back"));
    }
    AppendMenuW(menu, MF_POPUP | (removed.empty() ? MF_GRAYED : 0),
                (UINT_PTR)sub, Tr(L"Bring back a removed app"));
    AppendMenuW(menu,
                MF_STRING | (!g_appsLoaded ? MF_GRAYED : 0),
                kSort, Tr(g_settings.groups ? L"Sort apps within groups" : L"Sort A to Z"));
    UINT flags = TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY;
    g_modal = true;
    int command = TrackLaunchpadPopupMenu(menu, flags, pt.x, pt.y, hwnd, nullptr);
    g_modal = false;
    DestroyMenu(menu);

#ifdef SEAMLESS_PREVIEW
    if (command == 90) { if(ConfirmGroupEditClose())PostMessageW(hwnd, WM_APP_QUIT, 0, 0); return; }
#endif
    if(command==58){CancelGroupEdit();return;}
    if(g_groupEditMode&&command!=55)return;
    if (command == 50) { CreateGroup(hwnd); return; }
    if (command == 51 || command == 52) { ImportOrRestoreLayout(hwnd, command == 51); return; }
    if (command == 53) { ExportLayout(hwnd); return; }
    if (command == 59) { RestoreGroupGeometry(hwnd); return; }
    if (command == 54) { EditEntry(hwnd); return; }
    if (command == 55) { ToggleGroupEdit(); return; }
    if (command == 56 || command == 57) { ShowFullBackupDialog(hwnd,command==57); return; }
    if (command == kOpenSettings) {
        HideNow();
        OpenSettingsWindow();
    } else if (command == kRestoreAll ||
               (command >= kRestoreFirst &&
                command < kRestoreFirst + (int)removed.size())) {
        if (command == kRestoreAll) {
            for (const auto& app : removed) {
                g_hidden.erase(AppKey(*app));
            }
        } else {
            g_hidden.erase(AppKey(*removed[command - kRestoreFirst]));
        }
        ArrangeTiles(ArrangementLines(),true);  // Explicit restore puts them at the end.
        DropGoneFolder();
        SaveTiles();
        Refresh();
    } else if (command == kSort) {
        if (g_settings.groups) {
            for (auto& group : g_tiles) if (group.folder)
                std::stable_sort(group.folder->apps.begin(), group.folder->apps.end(), [](const Tile& a, const Tile& b) { return NameLess(a.app->name,b.app->name); });
            SaveTiles(); Refresh(); return;
        }
        g_modal = true;
        int answer = MessageBoxW(
            hwnd,
            Tr(L"Put every app back in A to Z order and out of its folder? Removed apps stay removed."),
            Tr(L"Sort A to Z"), MB_OKCANCEL | MB_ICONQUESTION);
        g_modal = false;
        if (g_shown && !g_closing) {
            ForceForeground(hwnd);
        }
        if (answer == IDOK) {
            std::vector<std::wstring> lines;
            for (const auto& key : g_hidden) {
                lines.push_back(L"hidden\t" + key);
            }
            ArrangeTiles(lines);
            DropGoneFolder();
            g_scroll = g_scrollTarget = g_groupPagePan = 0;
            SaveTiles();
            Refresh();
        }
    }
}

void QueryChanged() {
    UpdateFilter();
    g_hover = -1;
    g_selected = g_query.empty() || g_visible.empty() ? -1 : 0;
    g_scroll = g_scrollTarget = g_groupPagePan = 0;
    ComputeLayout();
    StartFrames();  // The glow eases in as typing starts.
    Render();
}

void PasteText() {
    if (!OpenClipboard(g_wnd)) {
        return;
    }
    if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
        if (auto* text = (PCWSTR)GlobalLock(data)) {
            std::wstring line = text;
            GlobalUnlock(data);
            line = line.substr(0, line.find_first_of(L"\r\n"));
            // Tabs would split the saved folder name; typed ones never get in.
            std::erase_if(line, [](WCHAR c) { return c < 32; });
            EditPush(0);
            InsertAtCaret(line);
            EditDone();
        }
    }
    CloseClipboard();
    EditChanged();
}

void OnChar(WCHAR ch) {
    if(g_groupEditMode)return;
    if (g_openFolder && !g_folderRenaming) return;
    if (g_drag.active || g_drag.pending) {
        return;
    }
    switch (ch) {
        case 8: {  // Backspace: the selection, or a code point before the caret.
            size_t from, to;
            if (SearchSelection(&from, &to) || g_caret > 0) {
                EditPush(2);
                if (!DeleteSelection()) {
                    size_t prev = CaretStep(g_caret, -1);
                    EditText().erase(prev, g_caret - prev);
                    g_caret = g_anchor = prev;
                }
                EditChanged();
                EditDone();
            }
            TouchCaret();
            return;
        }
        case 0x7F: {  // Ctrl+Backspace: the word before the caret.
            size_t from, to;
            if (SearchSelection(&from, &to) || g_caret > 0) {
                EditPush(2);
                if (!DeleteSelection()) {
                    size_t start = WordStart(g_caret);
                    EditText().erase(start, g_caret - start);
                    g_caret = g_anchor = start;
                }
                EditChanged();
                EditDone();
            }
            TouchCaret();
            return;
        }
        case 0x01:  // Ctrl+A: select all.
            g_anchor = 0;
            g_caret = EditText().size();
            TouchCaret();
            Render();
            return;
        case 0x03:  // Ctrl+C
            CopySelection(false);
            return;
        case 0x18:  // Ctrl+X
            CopySelection(true);
            return;
        case 0x16:  // Ctrl+V
            PasteText();
            return;
        case 0x1A:  // Ctrl+Z: undo; Ctrl+Shift+Z: redo.
            if (GetKeyState(VK_SHIFT) < 0) {
                RedoEdit();
            } else {
                UndoEdit();
            }
            return;
        case 0x19:  // Ctrl+Y: redo.
            RedoEdit();
            return;
    }
    if (ch >= 32 && EditText().size() < MaxEditChars()) {
        EditPush(1);
        InsertAtCaret(std::wstring(1, ch));
        EditDone();
        EditChanged();
        // Typing is how the field opens now: there is no resting icon to
        // click, so a keystroke both fills and reveals the field.
        if (!g_openFolder) g_searchActivated = true;
    }
}

void MoveSelection(int delta) {
    const Tiles& items = Items();
    if (items.empty()) {
        return;
    }
    int count = (int)items.size();
    if (g_selected < 0) {
        g_selected = 0;
    } else {
        int next = g_selected + delta;
        if (next >= 0 && next < count) {
            g_selected = next;
        } else if (delta > 0 && delta == Columns()) {
            g_selected = count - 1;  // Down on the last incomplete row.
        }
    }
    EnsureVisible(g_selected);
    Render();
    StartFrames();
}

void MoveGroupSelection(WPARAM key) {
    if (g_visible.empty()) return;
    if (g_selected<0 || g_selected>=(int)g_groupCells.size()) {
        g_selected=0;
        for (int i=0;i<(int)g_groupCells.size();i++) {
            auto c=GroupCellAt(i);
            if (c.icon.left>=0 && c.icon.right<=g_layout.width && c.icon.top>=g_layout.viewTop && c.label.bottom<=g_layout.viewBottom) {g_selected=i;break;}
        }
    } else {
        auto current=g_groupCells[g_selected].icon;
        float cx=(current.left+current.right)/2, cy=(current.top+current.bottom)/2;
        float best=1.0e30f; int next=g_selected;
        for (int i=0;i<(int)g_groupCells.size();i++) if (i!=g_selected) {
            auto c=g_groupCells[i].icon;
            float dx=(c.left+c.right)/2-cx,dy=(c.top+c.bottom)/2-cy;
            float forward=key==VK_LEFT?-dx:key==VK_RIGHT?dx:key==VK_UP?-dy:dy;
            float cross=key==VK_LEFT||key==VK_RIGHT?std::abs(dy):std::abs(dx);
            float score=forward+cross*4;
            if (forward>0.5f && score<best) {best=score;next=i;}
        }
        g_selected=next;
    }
    EnsureVisible(g_selected); Render(); StartFrames();
}

void OnKeyDown(WPARAM key) {
    if(g_groupEditMode&&key==VK_RETURN){ToggleGroupEdit();return;}
    if(key==VK_ESCAPE&&g_groupEditMode) {
        if(g_groupGesture.group) {
            ResetGroupGesture(true);
            Refresh();
        } else CancelGroupEdit();
        return;
    }
    if(g_groupEditMode||g_batchLaunching)return;
    if(key==VK_ESCAPE&&(!g_appSelection.empty()||g_selectionPointer.pending||g_selectionPointer.app)) {
        ClearAppSelection();
        g_pressed=-1;g_pressedEmpty=false;
        if(GetCapture()==g_wnd)ReleaseCapture();
        Render();return;
    }
    if (key == VK_ESCAPE && g_groupHeaderPressed) {
        CancelGroupPointer(); if (GetCapture() == g_wnd) ReleaseCapture(); Render(); return;
    }
    if (g_drag.active && key != VK_ESCAPE) return;
    if (GroupedView() && (key==VK_LEFT || key==VK_RIGHT || key==VK_UP || key==VK_DOWN)) {
        MoveGroupSelection(key); return;
    }
    if (g_openFolder && g_folderRenaming && (key == VK_RETURN || key == VK_ESCAPE)) {
        g_folderRenaming = false;
        g_searchDrag = false;
        g_anchor = g_caret;
        SaveFolderName();
        Render();
        return;
    }
    if (g_openFolder && !g_folderRenaming) {
        switch (key) {
            case VK_LEFT: MoveSelection(-1); break;
            case VK_RIGHT: MoveSelection(1); break;
            case VK_UP: MoveSelection(-Columns()); break;
            case VK_DOWN: MoveSelection(Columns()); break;
            case VK_RETURN: if (g_selected >= 0) ActivateItem(g_selected); break;
            case VK_ESCAPE: break;
            case VK_APPS: ShowSelectedMenu(); break;
        }
        if (key != VK_ESCAPE) return;
    }
    if (g_drag.active && key != VK_ESCAPE) {
        return;
    }
    switch (key) {
        case VK_ESCAPE:
            if (g_drag.active) {
                EndDrag(false);
                ReleaseCapture();
            } else if (g_powerArmed >= 0) {
                DisarmPower();
                Render();
            } else if (g_openFolder) {
                CloseFolder();
            } else if (!g_query.empty()) {
                EditPush(0);  // Ctrl+Z brings the cleared search back.
                g_query.clear();
                g_searchActivated = false;
                g_caret = g_anchor = 0;
                QueryChanged();
            } else if (g_searchActivated) {
                g_searchActivated = false;
                LayoutSearch();
                StartFrames();
                Render();
            } else {
                Close();
            }
            break;
        case VK_RETURN:
            if (g_selected >= 0) {
                ActivateItem(g_selected);
            } else if (!g_query.empty() && !g_visible.empty()) {
                ActivateItem(0);
            }
            break;
        case VK_APPS:
            ShowSelectedMenu();
            break;
        case VK_LEFT:
            if (!EditText().empty()) {
                bool word = GetKeyState(VK_CONTROL) < 0;
                SetCaret(word ? WordStart(g_caret) : CaretStep(g_caret, -1),
                         GetKeyState(VK_SHIFT) < 0);
                Render();
            } else {
                MoveSelection(-1);
            }
            break;
        case VK_RIGHT:
            if (!EditText().empty()) {
                bool word = GetKeyState(VK_CONTROL) < 0;
                SetCaret(word ? WordEnd(g_caret) : CaretStep(g_caret, 1),
                         GetKeyState(VK_SHIFT) < 0);
                Render();
            } else {
                MoveSelection(1);
            }
            break;
        case VK_UP:
            MoveSelection(-Columns());
            break;
        case VK_DOWN:
            MoveSelection(Columns());
            break;
        case VK_PRIOR:
            if (!g_openFolder) {
                ScrollRows(-g_layout.visibleRows);
            }
            break;
        case VK_NEXT:
            if (!g_openFolder) {
                ScrollRows(g_layout.visibleRows);
            }
            break;
        case VK_HOME:
            if (!EditText().empty()) {
                SetCaret(0, GetKeyState(VK_SHIFT) < 0);
                Render();
            }
            break;
        case VK_END:
            if (!EditText().empty()) {
                SetCaret(EditText().size(), GetKeyState(VK_SHIFT) < 0);
                Render();
            }
            break;
        case VK_DELETE:
            if (!EditText().empty()) {
                size_t dfrom, dto;
                if (SearchSelection(&dfrom, &dto) || g_caret < EditText().size()) {
                    EditPush(2);
                    bool changed = DeleteSelection();
                    if (!changed) {
                        size_t next = CaretStep(g_caret, 1);
                        if (next != g_caret) {
                            EditText().erase(g_caret, next - g_caret);
                            changed = true;
                        }
                    }
                    if (changed) {
                        EditChanged();
                        EditDone();
                    }
                }
                TouchCaret();
            }
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Opening instead of the Windows Start
//
// Every way of opening Start (Windows key, Ctrl+Esc, the taskbar's Start
// button, WM_SYSCOMMAND/SC_TASKLIST from the Dock) goes through
// XamlLauncher::ShowStartView in twinui.pcshell.dll. The hook asks the mod's
// window to toggle and doesn't call the original, so the Windows Start never
// opens.

// Observed on Windows 11 build 26300: the Windows key and Ctrl+Esc arrive as
// method 1 with flag 8; WM_SYSCOMMAND/SC_TASKLIST (the Dock's Start button)
// also arrives as method 1, without that flag.
constexpr int kShowMethodKeyboard = 1;
constexpr int kShowFlagKeyboard = 0x8;

bool IsKeyboardShow(int showMethod, int showFlags) {
    return showMethod == kShowMethodKeyboard && (showFlags & kShowFlagKeyboard);
}

// Shift + Windows key
//
// Windows doesn't open Start for the Windows key pressed with Shift, so
// ShowStartView never sees it. A low-level keyboard hook on its own thread
// (the window thread can be busy drawing) watches for a Windows key press
// with Shift down during it, and no other key pressed in between (Shift+Win+S and
// the like stay Windows shortcuts). For the grid and the original Start, the
// window presses Ctrl+Esc once the keys are up, and the ShowStartView hook
// gives that one the Shift + Windows key action.
//
// Windows keeps keys typed in an administrator window from this hook, and
// drops the Ctrl+Esc it would press there. The optional helper
// (helper/launchpad-keys.cpp) runs as administrator with the same hook. While its
// window exists, this hook leaves the key to it, and the helper sends
// WM_APP_SHIFT_WIN with wParam 1, then presses Ctrl+Esc itself if the answer
// is 1.
constexpr WCHAR kHelperClass[] = L"RYJSLaunchpadKeys";

std::atomic<ULONGLONG> g_shiftWinUntil;
HANDLE g_keyThread;
DWORD g_keyThreadId;

LRESULT CALLBACK KeyboardHook(int code, WPARAM wParam, LPARAM lParam) {
    // Keyboards that send Shift+Win from one key (macro keys) let go of Shift
    // a moment before Win, so Shift counts if it was down at any point.
    static bool winDown, otherKey, withShift;
    if (code == HC_ACTION) {
        DWORD vk = ((KBDLLHOOKSTRUCT*)lParam)->vkCode;
        bool up = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
        bool shift = vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT;
        if (vk == VK_LWIN || vk == VK_RWIN) {
            if (!up && !winDown) {
                winDown = true;
                otherKey = false;
                withShift = GetAsyncKeyState(VK_SHIFT) < 0;
            } else if (up && winDown) {
                winDown = false;
                HWND wnd = g_wnd;
                if (!otherKey && withShift && wnd &&
                    !FindWindowW(kHelperClass, nullptr)) {
                    PostMessageW(wnd, WM_APP_SHIFT_WIN, 0, 0);
                } else if (!otherKey && !withShift && wnd &&
                           g_winKeyAction.load() == KeyAction::Grid) {
                    // A plain tap: Windows would open Start here, and the mod
                    // answers that in the ShowStartView hook. A second tap never
                    // reaches that hook, so the window decides by its own state
                    // whether this one closes the grid. The key only drives the
                    // grid when it is set to: on Start or on a program, Windows
                    // keeps this tap.
                    PostMessageW(wnd, WM_APP_WIN_TAP, 0, 0);
                }
            }
        } else if (shift && !up && winDown) {
            withShift = true;
        } else if (!up && !shift && winDown) {
            otherKey = true;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

DWORD WINAPI KeyThread(void*) {
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook,
                                   GetModuleHandleW(nullptr), 0);
    if (!hook) {
        Wh_Log(L"Keyboard hook failed: %u", GetLastError());
        return 0;
    }
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
    }
    UnhookWindowsHookEx(hook);
    return 0;
}

bool OwnsTaskbar() {
#ifdef SEAMLESS_PREVIEW
    return true;
#else
    DWORD pid = 0;
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    return tray && GetWindowThreadProcessId(tray, &pid) &&
           pid == GetCurrentProcessId();
#endif
}

#ifndef SEAMLESS_PREVIEW

// Classic/StartAllBack taskbars latch their Start button independently of
// XamlLauncher. Consume the complete mouse gesture before that native latch
// runs; ShowStartView remains responsible for keyboard and other callers.
constexpr WCHAR kStartPressProperty[] = L"Launchpad.StartPress." WH_MOD_ID;
std::vector<HWND> g_startButtonWindows;  // Launcher thread only.
HWINEVENTHOOK g_startButtonEvents;
std::atomic<bool> g_startButtonReplacementActive{false};

UINT StartButtonCleanupMessage() {
    static UINT message = RegisterWindowMessageW(L"Launchpad.StartButtonCleanup." WH_MOD_ID);
    return message;
}
UINT StartButtonResetMessage() {
    static UINT message = RegisterWindowMessageW(L"Launchpad.StartButtonReset." WH_MOD_ID);
    return message;
}

bool WindowClassIs(HWND window, PCWSTR expected) {
    WCHAR name[64];
    return GetClassNameW(window, name, ARRAYSIZE(name)) &&
           _wcsicmp(name, expected) == 0;
}

HWND TaskbarStartButton(HWND window) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid != GetCurrentProcessId()) return nullptr;
    HWND root = GetAncestor(window, GA_ROOT);
    if (!WindowClassIs(root, L"Shell_TrayWnd") &&
        !WindowClassIs(root, L"Shell_SecondaryTrayWnd") &&
        !WindowClassIs(root, L"SIBTaskbar")) return nullptr;
    for (HWND current = window; current && current != root; current = GetParent(current)) {
        if (WindowClassIs(current, L"Start") || WindowClassIs(current, L"SIBOrb"))
            return current;
    }
    return nullptr;
}

void ClearStartButtonPress(HWND window) {
    RemovePropW(window, kStartPressProperty);
    if (GetCapture() == window) ReleaseCapture();
}

LRESULT CALLBACK StartButtonSubclass(HWND window, UINT message, WPARAM wp,
                                    LPARAM lp, DWORD_PTR button) {
    if(message==WM_LBUTTONDOWN||message==WM_LBUTTONUP||message==WM_CAPTURECHANGED||
       message==WM_CANCELMODE||message==WM_MOUSELEAVE||message==WM_KILLFOCUS) {
        Wh_Log(L"Start lifecycle hwnd=%p button=%p msg=%04x press=%lld capture=%p foreground=%p",
            window,(HWND)button,message,(long long)(INT_PTR)GetPropW(window,kStartPressProperty),GetCapture(),GetForegroundWindow());
    }
    if (message == StartButtonResetMessage()) {
        // The mouse pair alone does not clear a previously latched state or
        // the native hover left behind when the launcher covers the button.
        // Perform cleanup on its owning thread, after the click has unwound.
        if ((wp && !g_startButtonReplacementActive) ||
            (!wp && g_startButtonAction == KeyAction::Start) || (INT_PTR)GetPropW(window,kStartPressProperty)>1) return 0;
        DefSubclassProc(window, WM_CANCELMODE, 0, 0);
        DefSubclassProc(window, BM_SETSTATE, FALSE, 0);
        DefSubclassProc(window, BM_SETCHECK, BST_UNCHECKED, 0);
        DefSubclassProc(window, WM_MOUSELEAVE, 0, 0);
        InvalidateRect(window, nullptr, TRUE);
        return 0;
    }
    if (message == StartButtonCleanupMessage() || message == WM_NCDESTROY) {
        ClearStartButtonPress(window);
        return DefSubclassProc(window, message, wp, lp);
    }
    switch (message) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK: {
            KeyAction action = g_startButtonAction.load();
            if (!g_wnd || action == KeyAction::Start) break;
            // 1 means cancelled; other values remember the action at press
            // time, so a settings change cannot split the down/up pair.
            if (!SetPropW(window, kStartPressProperty,
                          (HANDLE)(INT_PTR)(2 + (int)action))) break;
            SetCapture(window);
            if (GetCapture() != window) {
                RemovePropW(window, kStartPressProperty);
                break;
            }
            return 0;
        }
        case WM_LBUTTONUP:
        case WM_NCLBUTTONUP: {
            INT_PTR press = (INT_PTR)GetPropW(window, kStartPressProperty);
            if (!press) break;
            bool captured = GetCapture() == window;
            ClearStartButtonPress(window);
            POINT point{(short)LOWORD(lp), (short)HIWORD(lp)};
            bool validPoint = message == WM_NCLBUTTONUP || ClientToScreen(window, &point);
            RECT rect;
            HWND launcher = g_wnd;
            g_startButtonReplacementActive = true;
            PostMessageW(window, StartButtonResetMessage(), 1, 0);
            if (press > 1 && captured && validPoint && launcher &&
                GetWindowRect((HWND)button, &rect) && PtInRect(&rect, point)) {
                KeyAction action = (KeyAction)(press - 2);
                PostMessageW(launcher, action == KeyAction::Program ? WM_APP_RUN : WM_APP_TOGGLE, 0, 0);
            }
            // Even a cancelled gesture owns its release. Never give the
            // native button an up for a down that it did not receive.
            return 0;
        }
        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
            if (GetPropW(window, kStartPressProperty)) {
                // Keep a release tombstone so an unmatched UP stays consumed,
                // but do not let that cancelled gesture block visual cleanup.
                SetPropW(window, kStartPressProperty, (HANDLE)1);
                if (message == WM_CANCELMODE && GetCapture() == window) ReleaseCapture();
                g_startButtonReplacementActive=true;
                PostMessageW(window,StartButtonResetMessage(),1,0);
            }
            break;
    }
    return DefSubclassProc(window, message, wp, lp);
}

void AttachStartButtonWindow(HWND window) {
    HWND button = TaskbarStartButton(window);
    if (!button) return;
    // Reapplying the same subclass is idempotent, including after HWND reuse.
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(window, StartButtonSubclass, (DWORD_PTR)button)) {
        Wh_Log(L"Couldn't subclass Start button: %u", GetLastError());
        return;
    }
    if (std::find(g_startButtonWindows.begin(), g_startButtonWindows.end(), window) == g_startButtonWindows.end()) {
        g_startButtonWindows.push_back(window);
        Wh_Log(L"Start button subclass attached: %p (button %p)", window, button);
    }
    PostMessageW(window, StartButtonResetMessage(), 0, 0);
}

void ResetTaskbarStartButtons(bool ownRequest) {
    if (ownRequest) g_startButtonReplacementActive = true;
    if (!g_startButtonReplacementActive) return;
    for (HWND window : g_startButtonWindows)
        PostMessageW(window, StartButtonResetMessage(), 1, 0);
}

BOOL CALLBACK AttachStartButtonChild(HWND window, LPARAM) {
    AttachStartButtonWindow(window);
    return TRUE;
}

BOOL CALLBACK AttachTaskbarStartButtons(HWND window, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (pid == GetCurrentProcessId() &&
        (WindowClassIs(window, L"Shell_TrayWnd") ||
         WindowClassIs(window, L"Shell_SecondaryTrayWnd") || WindowClassIs(window, L"SIBTaskbar")))
        EnumChildWindows(window, AttachStartButtonChild, 0);
    return TRUE;
}

void CALLBACK StartButtonWindowEvent(HWINEVENTHOOK, DWORD event, HWND window,
                                    LONG object, LONG child, DWORD, DWORD) {
    if (!g_startButtonEvents || !window || object != OBJID_WINDOW || child != CHILDID_SELF ||
        (event != EVENT_OBJECT_CREATE && event != EVENT_OBJECT_SHOW)) return;
    AttachStartButtonWindow(window);
    // A taskbar or orb can be announced after its children were created.
    if (TaskbarStartButton(window) || WindowClassIs(window, L"Shell_TrayWnd") ||
        WindowClassIs(window, L"Shell_SecondaryTrayWnd") || WindowClassIs(window, L"SIBTaskbar"))
        EnumChildWindows(window, AttachStartButtonChild, 0);
}

void StartButtonHooksStart() {
    // Register before scanning, so taskbars created during the scan are not
    // missed. Out-of-context callbacks run on this existing message loop.
    g_startButtonEvents = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW, nullptr,
        StartButtonWindowEvent, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    if (!g_startButtonEvents) Wh_Log(L"Start button window events unavailable: %u", GetLastError());
    EnumWindows(AttachTaskbarStartButtons, 0);
}

void StartButtonHooksStop() {
    if (g_startButtonEvents) {
        UnhookWinEvent(g_startButtonEvents);
        g_startButtonEvents = nullptr;
    }
    for (HWND window : g_startButtonWindows) {
        DWORD pid = 0;
        GetWindowThreadProcessId(window, &pid);
        if (pid != GetCurrentProcessId()) continue;
        // Cleanup runs on the button's thread before detaching the callback.
        SendMessageW(window, StartButtonCleanupMessage(), 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, StartButtonSubclass);
    }
    g_startButtonWindows.clear();
}


// Windows 11 Start is a XAML ExperienceToggleButton, not a Win32 "Start"
// child window. Its OnToggle latches IsChecked before Click reaches our
// ShowStartView hook. Intercept that toggle when replacing the native menu;
// otherwise no native menu-close notification ever clears the checked state.
// Hook the WinRT ABI setter: pThis is an actual COM interface, so identifying
// the element requires no implementation offsets or foreign memory writes.
std::atomic<bool> g_nativeStartPassthrough{false};
// The checked-state latch was diagnosed with this trace. Writing it runs on
// the taskbar's UI thread on every press, release and pointer move, so it is
// compiled out of normal builds; set this to true only for that diagnosis.
constexpr bool kStartStateTrace = false;
void StartStateTrace(PCWSTR event,int checked=-2,int pressed=-2,int hover=-2,int focus=-2) {
    if(!kStartStateTrace)return;
    auto path=StoragePath(L"start-state-trace.log");
    HANDLE file=CreateFileW(path.c_str(),FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(file==INVALID_HANDLE_VALUE)return;
    LARGE_INTEGER size{};if(GetFileSizeEx(file,&size)&&size.QuadPart<(1<<20)) {
        char row[560];int n=snprintf(row,sizeof(row),"%llu %ls checked=%d pressed=%d hover=%d replacement=%d native=%d focus=%d\r\n",
            GetTickCount64(),event,checked,pressed,hover,(int)g_startButtonReplacementActive.load(),(int)g_nativeStartPassthrough.load(),focus);
        if(n>0&&n<(int)sizeof(row)){DWORD written;WriteFile(file,row,n,&written,nullptr);}
    }
    CloseHandle(file);
}
// The keyboard path latches the button's checked state without ever running
// OnToggle (no trace, no hook there), and the native menu close that would
// clear it never happens either. Remember the element whenever a hook sees it,
// so the launcher can wipe the latch when it opens and hides. XAML objects
// live on the taskbar's UI thread, so the reset is posted through the
// element's own dispatcher.
static winrt::weak_ref<winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton> g_startButtonElement;
static void RememberStartButton(const winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton& element) {
    try {g_startButtonElement=winrt::make_weak(element);} catch(...) {}
}
void ClearStartButtonLatch(const wchar_t* why) {
    auto element=g_startButtonElement.get();
    if(!element) {
        // No hook has seen the button yet: the clear could not run. Trace it,
        // so a silent no-op is never mistaken for a working reset.
        StartStateTrace(why,-3,-3,-3);
        return;
    }
    try {
        auto dispatcher=element.Dispatcher();
        dispatcher.RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,[element,why]() {
            try {
                auto checked=element.IsChecked();
                // A null checked state is the three-state "indeterminate" —
                // it reads as neither true nor false but still paints a
                // pressed-looking capsule, so it counts as a latch too.
                bool latch=!checked||checked.Value();
                if(latch)element.IsChecked(winrt::box_value(false).as<winrt::Windows::Foundation::IReference<bool>>());
                // The keyboard path also leaves the keyboard focus ring on the
                // button: Start would hand it focus and move it away on close,
                // and neither happens when the menu is replaced.
                int focus=-2;
                auto control=element.try_as<winrt::Windows::UI::Xaml::Controls::Control>();
                if(control) {
                    focus=(int)control.FocusState();
                    if(control.FocusState()!=winrt::Windows::UI::Xaml::FocusState::Unfocused)
                        control.Focus(winrt::Windows::UI::Xaml::FocusState::Unfocused);
                    // The capsule is painted by the template's visual states,
                    // not by the logical properties above; a replaced toggle
                    // can leave the state machine stuck mid-gesture. Force the
                    // template back to its resting states as well.
                    winrt::Windows::UI::Xaml::VisualStateManager::GoToState(control,L"Normal",false);
                    winrt::Windows::UI::Xaml::VisualStateManager::GoToState(control,L"Unfocused",false);
                }
                StartStateTrace(why,latch?0:-1,element.IsPressed(),element.IsPointerOver(),focus);
            } catch(...) {}
        });
    } catch(...) {}
}
winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton StartToggleElement(void* object) {
    try {
        winrt::Windows::Foundation::IInspectable value{nullptr};winrt::copy_from_abi(value,object);
        auto element=value.try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
        if(element&&winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetAutomationId(element)==L"StartButton")
            return element.try_as<winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton>();
    } catch(...) {}
    return nullptr;
}
void TraceStartElement(PCWSTR event,winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton const& element) {
    if(!element)return;
    try {auto checked=element.IsChecked();StartStateTrace(event,checked?(checked.Value()?1:0):-1,element.IsPressed(),element.IsPointerOver());}catch(...){}
}
using XamlStartToggle_t=HRESULT(WINAPI*)(void*);
using XamlStartPointer_t=HRESULT(WINAPI*)(void*,void*);
XamlStartToggle_t g_xamlStartToggleOriginal=nullptr;
XamlStartPointer_t g_xamlStartPressedOriginal=nullptr,g_xamlStartReleasedOriginal=nullptr,g_xamlStartExitedOriginal=nullptr;
HRESULT WINAPI XamlStartToggleHook(void* object) {
    auto element=StartToggleElement(object);
    if(element)RememberStartButton(element);
    TraceStartElement(L"toggle-before",element);
    if(element&&g_wnd&&g_startButtonAction.load()!=KeyAction::Start) {
        // Only the selection latch belongs to the native menu. Leave the
        // pointer lifecycle and the following Click/ShowStartView untouched.
        // Also clear a latch left by a previous version or native-menu click.
        try {element.IsChecked(winrt::box_value(false).as<winrt::Windows::Foundation::IReference<bool>>());}
        catch(...) {return g_xamlStartToggleOriginal(object);}
        TraceStartElement(L"toggle-replaced",element);
        return S_OK;
    }
    auto result=g_xamlStartToggleOriginal(object);
    TraceStartElement(L"toggle-after",element);return result;
}
HRESULT WINAPI XamlStartPressedHook(void* object,void* args) {
    auto element=StartToggleElement(object);TraceStartElement(L"press-before",element);
    auto result=g_xamlStartPressedOriginal(object,args);TraceStartElement(L"press-after",element);return result;
}
HRESULT WINAPI XamlStartReleasedHook(void* object,void* args) {
    auto element=StartToggleElement(object);TraceStartElement(L"release-before",element);
    auto result=g_xamlStartReleasedOriginal(object,args);TraceStartElement(L"release-after",element);return result;
}
HRESULT WINAPI XamlStartExitedHook(void* object,void* args) {
    auto element=StartToggleElement(object);
    auto result=g_xamlStartExitedOriginal(object,args);
    TraceStartElement(L"exit-after",element);return result;
}
// IsChecked's dependency-property callback: every change of the checked state
// passes here, including the programmatic writes of the taskbar's view model.
// The keyboard path latches the button through exactly such a write — it
// never runs OnToggle and never touches IsExperienceVisible, so without this
// hook both the latch and the element reference behind ClearStartButtonLatch
// stay invisible. The sender is the button itself (a DependencyObject).
using XamlStartCheckedChange_t=void(WINAPI*)(void*,void*,void*);
XamlStartCheckedChange_t g_xamlStartCheckedChangeOriginal=nullptr;
void WINAPI XamlStartCheckedChangeHook(void* pThis,void* sender,void* property) {
    auto element=sender?StartToggleElement(sender):nullptr;
    if(element)RememberStartButton(element);
    g_xamlStartCheckedChangeOriginal(pThis,sender,property);
    TraceStartElement(L"checked-change",element);
    bool replacing=g_wnd&&!g_nativeStartPassthrough&&
        (g_startButtonAction!=KeyAction::Start||g_startButtonReplacementActive);
    if(element&&replacing)ClearStartButtonLatch(L"latch-checked-clear");
}
using XamlStartVisible_t=HRESULT(WINAPI*)(void*,bool);
XamlStartVisible_t g_xamlStartVisibleOriginal=nullptr;
std::atomic<bool> g_xamlStartInstalling{false};
using XamlLoadLibrary_t=HMODULE(WINAPI*)(LPCWSTR,HANDLE,DWORD);
XamlLoadLibrary_t g_xamlLoadLibraryOriginal=nullptr;
bool HookXamlStartVisibility();
HMODULE WINAPI XamlTaskbarLoadHook(LPCWSTR name,HANDLE file,DWORD flags) {
    auto module=g_xamlLoadLibraryOriginal(name,file,flags);
    if(module&&!g_xamlStartVisibleOriginal&&
       (module==GetModuleHandleW(L"Taskbar.View.dll")||module==GetModuleHandleW(L"ExplorerExtensions.dll"))) {
        if(HookXamlStartVisibility())Wh_ApplyHookOperations();
    }
    return module;
}
HRESULT WINAPI XamlStartVisibleHook(void* object,bool visible) {
    bool start=false;
    winrt::Windows::Foundation::IInspectable inspectable{nullptr};
    try {
        winrt::copy_from_abi(inspectable,object);
        auto element=inspectable.try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
        start=element&&winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetAutomationId(element)==L"StartButton";
    } catch(...) { }
    if(start) {
        bool replacing=g_wnd&&!g_nativeStartPassthrough&&
            (g_startButtonAction!=KeyAction::Start||g_startButtonReplacementActive);
        bool effective=visible&&!replacing;
        // The keyboard path never reaches the toggle or pointer hooks, so the
        // visibility request is the only trace it leaves. Log the element's
        // state with it: this is where a keyboard-driven latch becomes
        // visible.
        auto toggle=start?inspectable.try_as<winrt::Windows::UI::Xaml::Controls::Primitives::ToggleButton>():nullptr;
        if(toggle)RememberStartButton(toggle);
        try {
            if(toggle) {
                auto checked=toggle.IsChecked();
                StartStateTrace(visible?L"visible-true":L"visible-false",
                    checked?(checked.Value()?1:0):-1,toggle.IsPressed(),toggle.IsPointerOver());
            } else StartStateTrace(visible?L"visible-true":L"visible-false");
        } catch(...) {StartStateTrace(visible?L"visible-true-err":L"visible-false-err");}
        Wh_Log(L"XAML Start visibility requested=%d effective=%d replacing=%d native=%d",
            visible,effective,replacing,(bool)g_nativeStartPassthrough);
        if(visible&&replacing)ClearStartButtonLatch(L"latch-visible-clear");
        auto result=g_xamlStartVisibleOriginal(object,effective);
        if(!visible)g_nativeStartPassthrough=false;
        return result;
    }
    return g_xamlStartVisibleOriginal(object,visible);
}
bool HookXamlStartVisibility() {
    if(g_xamlStartVisibleOriginal)return true;
    if(g_xamlStartInstalling.exchange(true))return false;
    struct InstallingGuard {~InstallingGuard(){g_xamlStartInstalling=false;}} guard;
    auto module=GetModuleHandleW(L"Taskbar.View.dll");
    if(!module)module=GetModuleHandleW(L"ExplorerExtensions.dll");
    if(!module){
        // Hook the kernelbase export, not the kernel32 thunk: a call that goes
        // straight to kernelbase (WinRT activation of Taskbar.View.dll) never
        // reaches a hook placed on the kernel32 import.
        auto kernelbase=GetModuleHandleW(L"kernelbase.dll");
        auto loadLibrary=kernelbase
            ?reinterpret_cast<XamlLoadLibrary_t>(GetProcAddress(kernelbase,"LoadLibraryExW"))
            :nullptr;
        if(loadLibrary&&!g_xamlLoadLibraryOriginal){
            // Registered here; Windhawk applies queued hooks when Wh_ModInit
            // returns, so no explicit apply is needed (and during init the API
            // refuses one anyway).
            WindhawkUtils::SetFunctionHook(loadLibrary,XamlTaskbarLoadHook,
                                           &g_xamlLoadLibraryOriginal);
        }
        Wh_Log(L"XAML taskbar not loaded; waiting for its module load.");return false;
    }
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[]={
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::ExperienceToggleButton,struct winrt::Windows::UI::Xaml::Controls::Primitives::IToggleButtonOverrides>::OnToggle(void))"},
            &g_xamlStartToggleOriginal,XamlStartToggleHook,true},
        {{LR"(private: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::OnIsCheckedPropertyChanged(struct winrt::Windows::UI::Xaml::DependencyObject const &,struct winrt::Windows::UI::Xaml::DependencyProperty const &))"},
            &g_xamlStartCheckedChangeOriginal,XamlStartCheckedChangeHook,true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::ExperienceToggleButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"},
            &g_xamlStartPressedOriginal,XamlStartPressedHook,true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::ExperienceToggleButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerReleased(void *))"},
            &g_xamlStartReleasedOriginal,XamlStartReleasedHook,true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::ExperienceToggleButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &g_xamlStartExitedOriginal,XamlStartExitedHook,true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::ExperienceToggleButton,struct winrt::Taskbar::IExperienceToggleButton>::put_IsExperienceVisible(bool))"},
            &g_xamlStartVisibleOriginal,XamlStartVisibleHook,true},
    };
    bool ok=WindhawkUtils::HookSymbols(module,taskbarViewHooks,ARRAYSIZE(taskbarViewHooks))&&g_xamlStartVisibleOriginal;
    // The checked-change hook is the one that sees the keyboard path's latch:
    // record whether its symbol actually resolved.
    StartStateTrace(g_xamlStartCheckedChangeOriginal?L"hooks-installed-cc1":L"hooks-installed-cc0",
        g_xamlStartToggleOriginal!=nullptr,g_xamlStartPressedOriginal!=nullptr,g_xamlStartReleasedOriginal!=nullptr);
    Wh_Log(L"XAML Start visibility hook available=%d",ok);return ok;
}

#endif

#ifndef SEAMLESS_PREVIEW
// The ExperienceToggleButton hooks install when the taskbar's XAML module is
// already mapped, or from the kernelbase LoadLibraryExW hook when it loads
// later. A ShowStartView request runs the same check as a last-resort
// self-heal, so no polling timer is needed.
void EnsureXamlStartHooks() {
    static bool attempted = false;
    if (g_xamlStartVisibleOriginal) return;
    if (attempted || (!GetModuleHandleW(L"Taskbar.View.dll") &&
                      !GetModuleHandleW(L"ExplorerExtensions.dll")))
        return;
    attempted = true;
    bool ok = HookXamlStartVisibility();
    if (ok) Wh_ApplyHookOperations();
    StartStateTrace(ok ? L"xaml-hook-lazy-ok" : L"xaml-hook-lazy-fail");
}
#endif

using XamlLauncher_ShowStartView_t = HRESULT(WINAPI*)(void* pThis,
                                                      int showMethod,
                                                      int showFlags);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original;
HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis,
                                               int showMethod,
                                               int showFlags) {
    Wh_Log(L"> method %d, flags %d", showMethod, showFlags);
    const bool shiftWin = GetTickCount64() < g_shiftWinUntil;
    const bool keyboard = IsKeyboardShow(showMethod, showFlags);
    KeyAction action = shiftWin
                           ? g_shiftWinKeyAction.load()
                       : keyboard
                           ? g_winKeyAction.load()
                           : g_startButtonAction.load();
    g_shiftWinUntil = 0;
#ifndef SEAMLESS_PREVIEW
    // The keyboard path never reaches the toggle or pointer hooks, so the
    // show request is the only record of it in the state trace.
    {
        std::wstring note = std::wstring(L"show-") +
                            (shiftWin ? L"shiftwin" : keyboard ? L"key" : L"button") +
                            L"-m" + std::to_wstring(showMethod) +
                            L"-f" + std::to_wstring(showFlags) +
                            (action == KeyAction::Grid ? L"-grid"
                             : action == KeyAction::Start ? L"-start"
                                                          : L"-program");
        StartStateTrace(note.c_str());
    }
    // A mouse click reaches here even when the XAML hooks never installed.
    // Install them now so the latch this click left on the button is the
    // last one; every later click is intercepted inside OnToggle itself.
    EnsureXamlStartHooks();
#endif
    HWND wnd = g_wnd;
    if (action == KeyAction::Start || !wnd) {
#ifndef SEAMLESS_PREVIEW
        g_startButtonReplacementActive = false;
        g_nativeStartPassthrough=true;
#endif
        return XamlLauncher_ShowStartView_Original(pThis, showMethod,
                                                   showFlags);
    }
#ifndef SEAMLESS_PREVIEW
    g_nativeStartPassthrough=false;
    g_startButtonReplacementActive=true;
#endif
    PostMessageW(wnd, action == KeyAction::Program ? WM_APP_RUN
                                                   : WM_APP_TOGGLE,
                 !shiftWin && !keyboard, 0);
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// Windows


void OnAppsLoaded(AppList* list, int key) {
    if(g_groupEditMode){delete list;g_appsTime=0;return;}
    if (g_fullImportBusy) { delete list; return; }
    if (g_drag.active || g_modal || g_groupHeaderPressed || !g_appSelection.empty() || g_selectionPointer.pending) {
        // Not under a dragged icon or an open menu: read again on the next
        // open.
        delete list;
        g_appsTime = 0;
        return;
    }
    if (g_openFolder) {
        SaveFolderName();
        g_openFolder = nullptr;
    }
    g_apps = list ? std::move(*list) : AppList{};
    g_labelCache.clear();
    delete list;
    g_appsLoaded = true;
    g_appsKey = key;
    g_appsNormalized = g_settings.normalizeIcons;
    ApplyEntries(key & 0xFFFF);
    ApplyDisplayNames();
    LoadTiles();
    ApplyCustomIcons(key & 0xFFFF);
    if (key != AppsKey()) {
        // The settings changed while this list was read: the reload then
        // found the worker busy. It has posted and is about to end.
        WaitForSingleObject(g_worker, 1000);
        StartLoadingApps();
    }
    if (g_shown) {
        UpdateFilter();
        int count = (int)g_visible.size();
        if (g_selected >= count) {
            g_selected = count ? 0 : -1;
        }
        g_hover = g_pressed = -1;
        ComputeLayout();
        Render();
    }
}

// The tray icon (Look > Tray icon): click opens the settings, right-click shows a menu.
// It lives on the grid's window and thread, like the rest of Launchpad.
constexpr UINT WM_APP_TRAY = WM_APP + 21;
constexpr UINT kTrayId = 1;
static HICON g_trayIcon;
static bool g_trayShown;
static UINT g_taskbarCreated;

// A blue to mauve rounded square with a 3 by 3 grid of dots, drawn per pixel so no
// resource file is needed.
static HICON MakeTrayIcon(int size) {
    BITMAPV5HEADER bi{};
    bi.bV5Size = sizeof(bi); bi.bV5Width = size; bi.bV5Height = -size; bi.bV5Planes = 1;
    bi.bV5BitCount = 32; bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask = 0x00FF0000; bi.bV5GreenMask = 0x0000FF00; bi.bV5BlueMask = 0x000000FF; bi.bV5AlphaMask = 0xFF000000;
    void* bits = nullptr;
    HDC dc = GetDC(nullptr);
    HBITMAP color = CreateDIBSection(dc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, dc);
    if (!color) return nullptr;
    auto px = (UINT32*)bits;
    const float n = (float)size, rad = n * 0.26f, ss = 4;
    auto inRound = [&](float x, float y) {
        float cx = std::clamp(x, rad, n - rad), cy = std::clamp(y, rad, n - rad);
        return (x - cx) * (x - cx) + (y - cy) * (y - cy) <= rad * rad;
    };
    auto inDot = [&](float x, float y) {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++) {
                float cx = n * (0.3f + 0.2f * i), cy = n * (0.3f + 0.2f * j), r = n * 0.065f;
                if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= r * r) return true;
            }
        return false;
    };
    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++) {
            float cover = 0, dot = 0;
            for (int sy = 0; sy < ss; sy++)
                for (int sx = 0; sx < ss; sx++) {
                    float fx = x + (sx + 0.5f) / ss, fy = y + (sy + 0.5f) / ss;
                    if (inRound(fx, fy)) { cover++; if (inDot(fx, fy)) dot++; }
                }
            cover /= ss * ss; dot /= ss * ss;
            float t = std::clamp((x + y) / (2.0f * n), 0.0f, 1.0f);
            // 0x89B4FA to 0xCBA6F7, with the dots in crust (0x171717).
            float r = (0x89 + (0xCB - 0x89) * t), g = (0xB4 + (0xA6 - 0xB4) * t), b = (0xFA + (0xF7 - 0xFA) * t);
            float k = cover > 0 ? dot / cover : 0;
            r = r * (1 - k) + 0x11 * k; g = g * (1 - k) + 0x11 * k; b = b * (1 - k) + 0x1B * k;
            BYTE a = (BYTE)(cover * 255 + 0.5f);
            px[y * size + x] = ((UINT32)a << 24) | ((UINT32)(r * cover) << 16) | ((UINT32)(g * cover) << 8) | (UINT32)(b * cover);
        }
    HBITMAP mask = CreateBitmap(size, size, 1, 1, nullptr);
    ICONINFO ii{TRUE, 0, 0, mask, color};
    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(mask);
    DeleteObject(color);
    return icon;
}

static void UpdateTrayIcon(HWND hwnd) {
    NOTIFYICONDATAW nid{sizeof(nid)};
    nid.hWnd = hwnd;
    nid.uID = kTrayId;
    if (!g_settings.trayIcon) {
        if (g_trayShown) Shell_NotifyIconW(NIM_DELETE, &nid);
        g_trayShown = false;
        return;
    }
    if (!g_trayIcon) g_trayIcon = MakeTrayIcon(GetSystemMetricsForDpi(SM_CXSMICON, GetDpiForSystem()));
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
    nid.uCallbackMessage = WM_APP_TRAY;
    nid.hIcon = g_trayIcon;
    wcscpy_s(nid.szTip, L"RYJS Launchpad");
    if (g_trayShown && Shell_NotifyIconW(NIM_MODIFY, &nid)) return;
    g_trayShown = Shell_NotifyIconW(NIM_ADD, &nid) != FALSE;
    if (g_trayShown) {
        nid.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nid);
    }
}

static void RemoveTrayIcon(HWND hwnd) {
    if (g_trayShown) {
        NOTIFYICONDATAW nid{sizeof(nid)};
        nid.hWnd = hwnd;
        nid.uID = kTrayId;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        g_trayShown = false;
    }
    if (g_trayIcon) {
        DestroyIcon(g_trayIcon);
        g_trayIcon = nullptr;
    }
}

static void OnTrayMessage(HWND hwnd, LPARAM lParam) {
    switch (LOWORD(lParam)) {
        case NIN_SELECT:
        case NIN_KEYSELECT:
            OpenSettingsWindow();
            break;
        case WM_CONTEXTMENU: {
            HMENU menu = CreatePopupMenu();
            AppendMenuW(menu, MF_STRING, 1, Tr(L"Settings"));
            AppendMenuW(menu, MF_STRING, 2, Tr(L"Open Launchpad"));
#ifdef SEAMLESS_PREVIEW
            AppendMenuW(menu, MF_STRING, 90, Tr(L"Exit preview"));
#endif
            SetMenuDefaultItem(menu, 1, FALSE);
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            int cmd = TrackLaunchpadPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON, pt.x, pt.y, hwnd, nullptr);
            DestroyMenu(menu);
#ifdef SEAMLESS_PREVIEW
            if (cmd == 90 && ConfirmGroupEditClose()) PostMessageW(hwnd, WM_APP_QUIT, 0, 0);
#endif
            if (cmd == 1) OpenSettingsWindow();
            if (cmd == 2) PostMessageW(hwnd, WM_APP_TOGGLE, 0, 0);
            break;
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_taskbarCreated && msg == g_taskbarCreated) {
        g_trayShown = false;  // The new taskbar has no icons yet.
        UpdateTrayIcon(hwnd);
        return 0;
    }
    switch (msg) {
        case WM_APP + 20: OpenSettingsWindow(); return 0;
        case WM_APP + 62:
            if (g_shown) {
                if(!ConfirmGroupEditClose())return 0;
                auto query = g_query;
                bool searchActivated = g_searchActivated;
                HideNow();
                Open();
                g_query = std::move(query);
                g_searchActivated = searchActivated;
                // Open() starts the field collapsed; a reload keeps the text,
                // so the field starts where that text says it belongs instead
                // of folding up and growing again.
                g_searchExpand = SearchExpanded() ? 1.f : 0.f;
                g_caret = g_anchor = g_query.size();
                UpdateFilter();
                ComputeLayout();
                Render();
            }
            return 0;
#ifdef SEAMLESS_PREVIEW
        case WM_APP + 60: Preview_RunSmoke(hwnd); return 0;
        case WM_APP + 61: Open(); return 0;
#endif
        case WM_APP_TRAY: OnTrayMessage(hwnd, lParam); return 0;
        case WM_APP_TOGGLE:
#ifndef SEAMLESS_PREVIEW
            ResetTaskbarStartButtons(true);
#endif
            // The keyboard hook reports the same Windows key tap that produced
            // this request, so only the first of the pair toggles.
            if (!FirstOfKeystroke()) {
                return 0;
            }
            // Clicking the taskbar's Start button while this one is open first
            // takes the focus (which closes it) and then asks for Start: that
            // click was meant to close it.
            if ((!g_settings.iconFlyIn || wParam) && (!g_shown || g_closing) &&
                Now() - g_lastClosed < 0.5) {
                if (g_settings.iconFlyIn) g_lastClosed = 0;
                return 0;
            }
            Toggle();
            return 0;

        case WM_APP_WIN_TAP:
            // A lone Windows key tap. The grid follows the key itself rather
            // than Windows' idea of Start: once the mod answered the first
            // show, Windows believes Start is up and says nothing on the next
            // tap, so the key is the only reliable source for closing.
            if (!FirstOfKeystroke()) {
                return 0;
            }
#ifndef SEAMLESS_PREVIEW
            ResetTaskbarStartButtons(true);
#endif
            Toggle();
            return 0;

        case WM_APP_APPS:
            OnAppsLoaded((AppList*)lParam, (int)wParam);
            return 0;
        case kWebIconReady: OnWebIconReady((WebIconResult*)lParam); return 0;
        case kFullBackupDone: OnFullBackupDone(hwnd); return 0;

        case WM_APP_RELOAD:
            if (g_fullImportBusy || !ConfirmGroupEditClose()) return 0;
            if (g_shown) HideNow(); // Save an edited folder using the previous layout mode.
            LoadSettings();
            g_labelFormat.Reset();
            UpdateTrayIcon(hwnd);
            if (g_shown) {
                HideNow();
            }
            if (g_appsKey != AppsKey() ||
                g_appsNormalized != g_settings.normalizeIcons) {
                StartLoadingApps();
            } else if (g_appsLoaded) {
                LoadTiles();
            }
            return 0;

        case WM_APP_RUN:
            if (g_fullImportBusy || !ConfirmGroupEditClose()) return 0;
#ifndef SEAMLESS_PREVIEW
            ResetTaskbarStartButtons(true);
#endif
            if (g_shown) {
                HideNow();
            }
            RunProgram();
            return 0;

        case WM_APP_SHIFT_WIN:
            // Explorer runs this mod in each of its processes; only the
            // taskbar's one answers the key.
            if (!OwnsTaskbar()) {
                return 0;
            }
            if (g_shiftWinKeyAction == KeyAction::Program) {
                PostMessageW(hwnd, WM_APP_RUN, 0, 0);
                return 0;
            }
            if (g_shown && g_shiftWinKeyAction == KeyAction::Start) {
                if(!ConfirmGroupEditClose())return 0;
                HideNow();
            }
            if (wParam == 1) {
                // The helper presses Ctrl+Esc once the keys are up.
                g_shiftWinUntil = GetTickCount64() + 3000;
                return 1;
            }
            // The grid and the original Start open through Ctrl+Esc, once
            // Shift is up (with Shift it's Task Manager): only a Start that
            // Windows opens may take the foreground. SC_TASKLIST to the
            // taskbar does nothing here.
            SetTimer(hwnd, kTimerShiftWin, 20, nullptr);
            return 0;

        case WM_APP_QUIT:
            RemoveTrayIcon(hwnd);
            DestroyWindow(hwnd);
            return 0;

        case WM_TIMER:
            switch (wParam) {
                case kTimerFrame:
                    OnFrame();
                    break;
                case kTimerShiftWin:
                    if (GetAsyncKeyState(VK_SHIFT) >= 0 &&
                        GetAsyncKeyState(VK_LWIN) >= 0 &&
                        GetAsyncKeyState(VK_RWIN) >= 0) {
                        KillTimer(hwnd, kTimerShiftWin);
                        g_shiftWinUntil = GetTickCount64() + 1000;
                        INPUT keys[4] = {};
                        WORD vks[4] = {VK_CONTROL, VK_ESCAPE, VK_ESCAPE,
                                       VK_CONTROL};
                        for (int i = 0; i < 4; i++) {
                            keys[i].type = INPUT_KEYBOARD;
                            keys[i].ki.wVk = vks[i];
                            keys[i].ki.dwFlags = i < 2 ? 0 : KEYEVENTF_KEYUP;
                        }
                        SendInput(4, keys, sizeof(INPUT));
                    }
                    break;
                case kTimerPowerArm:
                    DisarmPower();
                    Render();
                    break;
                case kTimerCaret:
                    // The search caret blinks by plain redraws.
                    if (g_shown && (g_openFolder ? g_folderRenaming : SearchExpanded())) {
                        Render();
                    }
                    break;
                case kTimerFirstLoad:
                    // Only in the process that owns the taskbar: short-lived
                    // explorer.exe processes (explorer.exe /select,...) get
                    // the mod too, and exit before they'd need the list.
                    if (OwnsTaskbar()) {
                        KillTimer(hwnd, kTimerFirstLoad);
                        if (!g_appsLoaded) {
                            StartLoadingApps();
                        }
                    }
                    break;
            }
            return 0;

        case WM_ACTIVATE:
#ifdef SEAMLESS_PREVIEW
            if(Preview::uiCheck)return 0;
#endif
            if (LOWORD(wParam) == WA_INACTIVE && g_shown && !g_closing &&
                !g_modal) {
                g_lastClosed = Now();
                Close();
            }
            return 0;

        case WM_SETCURSOR:
            if(LOWORD(lParam)==HTCLIENT&&g_groupEditMode) {
                POINT pt{};GetCursorPos(&pt);ScreenToClient(hwnd,&pt);
                if((g_groupGesture.group&&g_groupGesture.resize)||GroupResizeAt(pt.x,pt.y)>=0) {
                    SetCursor(LoadCursorW(nullptr,IDC_SIZENWSE));return TRUE;
                }
            }
            break;
        case WM_MOUSEMOVE: {
            if (!g_trackingMouse) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                g_trackingMouse = TrackMouseEvent(&tme);
            }
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            bool pointerChanged=!g_glassPointer||g_glassPointer->x!=x||g_glassPointer->y!=y;
            g_glassPointer=D2D1::Point2F((float)x,(float)y);
            auto bar=ScrollThumb();
            bool scrollHover=SelectionHasArea(bar)&&InButton(bar,(float)x,(float)y);
            if(std::exchange(g_scrollHover,scrollHover)!=scrollHover)Render();
            if (GroupPointerMove(x,y)||MoveAppSelection(x,y)) return 0;
            UpdateGroupResizeHover(x,y);
            int batchHover=BatchActionAt(x,y);
            if(batchHover!=g_batchHover){g_batchHover=batchHover;StartFrames();}
            if (g_searchDrag) {
                SetCaret(CaretAtPoint((float)x), true);
                Render();
                return 0;
            }
            // A drag is real past the system drag threshold only.
            if (g_drag.pending && !g_drag.active &&
                (std::abs(x - g_drag.start.x) >
                     GetSystemMetrics(SM_CXDRAG) ||
                 std::abs(y - g_drag.start.y) >
                     GetSystemMetrics(SM_CYDRAG))) {
                StartDrag();
            }
            if (g_drag.active) {
                DragMove(x, y);
                return 0;
            }
            bool titleHover = g_openFolder && InRect(FolderTitlePill(), (float)x, (float)y);
            if (titleHover != g_folderTitleHover) {
                g_folderTitleHover = titleHover;
                Render();
            }
            int hover = ItemAt(x, y);
            bool clearHover = !g_openFolder && !g_query.empty() &&
                              InButton(g_layout.clearButton, (float)x, (float)y);
            bool searchHover = !g_openFolder && !clearHover &&
                               g_searchExpand > 0.02f &&
                               InRect(g_layout.search, (float)x, (float)y);
            int powerHover = g_openFolder ? -1 : PowerAt(x, y);
            if (hover != g_hover ||
                clearHover != g_clearHover ||
                searchHover != g_searchHover ||
                powerHover != g_powerHover) {
                g_hover = hover;
                g_clearHover = clearHover;
                g_searchHover = searchHover;
                g_powerHover = powerHover;
                StartFrames();
            }
            if(pointerChanged&&!g_groupEditMode&&(g_glassClaimed||g_openFolder||
               (y>=g_layout.viewTop&&y<g_layout.viewBottom)))StartFrames();
            return 0;
        }

        case WM_MOUSELEAVE:
            g_glassPointer.reset();
            if(std::exchange(g_scrollHover,false))Render();
            g_folderTitleHover = false;
            g_batchHover=-1;UpdateGroupResizeHover(-1,-1);StartFrames();
            g_trackingMouse = false;
            if (g_hover >= 0 || g_clearHover || g_searchHover ||
                g_powerHover >= 0) {
                g_hover = -1;
                g_clearHover = false;
                g_searchHover = false;
                g_powerHover = -1;
                StartFrames();
                Render();
                StartFrames();
            }
            return 0;

        case WM_LBUTTONDOWN: {
            if (g_modal||g_batchLaunching) { g_pressed=-1; g_pressedEmpty=false; return 0; }
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (g_closing) {
                return 0;  // A click during the closing fade does nothing.
            }
            if(int action=GroupEditButtonAt(x,y);action>=0) {
                g_groupEditButton=action;SetCapture(hwnd);Render();return 0;
            }
            if(ToolBarConsumes(x,y)) {
                g_batchPressed=BatchActionAt(x,y);
                if(g_batchPressed>=0)SetCapture(hwnd);
                Render();return 0;
            }
            if (GroupPointerDown(hwnd,x,y)) return 0;
            if (!g_openFolder) {
                int power = PowerAt(x, y);
                if (power >= 0) {
                    g_powerPressed = power;
                    SetCapture(hwnd);
                    Render();
                    return 0;
                }
                if (g_powerArmed >= 0) {
                    DisarmPower();  // A click elsewhere cancels the prompt.
                }
                if (!g_query.empty() &&
                    InButton(g_layout.clearButton, (float)x, (float)y)) {
                    EditPush(0);  // Ctrl+Z brings the cleared search back.
                    g_query.clear();
                    g_searchActivated = true;
                    g_caret = g_anchor = 0;
                    g_clearHover = false;
                    QueryChanged();
                    return 0;
                }
                int dot = PageDotAt(x, y);
                if (dot >= 0) {
                    float target=dot*(float)g_layout.width;
                    if(g_scrollTarget!=target)g_groupPagePan=0;
                    g_scrollTarget=target;
                    StartFrames();
                    return 0;
                }
            }
            if(g_groupEditMode){FreeGroupPointerDown(hwnd,x,y);return 0;}
            // The resting field is invisible, so it also stops taking clicks:
            // only a field that has opened (typing or a kept-activated clear)
            // takes pointer input. Folder renaming keeps its own pill.
            bool editHit = (g_openFolder || g_searchExpand > 0.02f) &&
                           InRect(g_openFolder ? FolderTitlePill() : g_layout.search,
                                  (float)x, (float)y);
            if (g_openFolder && g_folderRenaming && !editHit) {
                g_folderRenaming = false;
                SaveFolderName();
            }
            if (editHit) {
                if (g_openFolder) {
                    g_folderRenaming = true;
                } else {
                    g_searchActivated = true;
                    if (g_query.empty()) {
                        g_selected = -1;
                        LayoutSearch();
                        StartFrames();
                    }
                }
                // A click in the search field, or in an open folder's name,
                // places the caret; holding the button and moving selects
                // text. A second click on the same spot selects the word.
                UINT64 tick = GetTickCount64();
                bool dbl = tick - g_lastSearchClick <=
                               GetDoubleClickTime() &&
                           std::abs(x - g_lastSearchX) <=
                               4 * g_layout.scale;
                g_lastSearchClick = tick;
                g_lastSearchX = x;
                if (dbl) {
                    size_t pos = CaretAtPoint((float)x);
                    g_anchor = WordStart(pos);
                    g_caret = WordEnd(pos);
                } else {
                    SetCaret(CaretAtPoint((float)x), false);
                }
                g_searchDrag = true;
                SetCapture(hwnd);
                Render();
                return 0;
            }
            if (g_anchor != g_caret) {
                g_anchor = g_caret;  // A click elsewhere drops the selection.
            }
            SetCapture(hwnd);
            g_pressed = ItemAt(x, y);
            g_pressedEmpty = g_pressed < 0 && !ClickKeepsOpen(x, y);
            if(g_pressed>=0&&Items()[g_pressed].app&&((wParam&MK_CONTROL)||!g_appSelection.empty()))
                g_selectionPointer.app=Items()[g_pressed].app;
            else if(g_pressed<0&&(!g_openFolder||InRect(GetFolderLayout().panel,(float)x,(float)y))) {
                g_selectionPointer.base=g_appSelection;
                g_selectionPointer.baseKeys=g_selectedAppKeys;
                g_selectionPointer.start=g_selectionPointer.end={x,y};
                g_selectionPointer.pending=true;
            }
            g_drag.pending = g_pressed >= 0 && !g_selectionPointer.app && g_appSelection.empty() && g_query.empty();
            g_drag.pressTime = Now();
            g_drag.start = {x, y};
            if (g_drag.pending) {
                StartFrames();  // Ticks the hold time toward the edit mode.
            }
            Render();
            return 0;
        }

        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if(g_groupEditButton>=0) {
                int action=std::exchange(g_groupEditButton,-1);
                if(GetCapture()==hwnd)ReleaseCapture();
                if(action==GroupEditButtonAt(x,y)) {
                    if(action==0)CancelGroupEdit();
                    else if(action==1)EqualizeGroupSpacing();
                    else FinishGroupEdit();
                } else Render();
                return 0;
            }
            if(g_groupGesture.group)FreeGroupPointerMove(x,y);
            if (GroupPointerUp(hwnd)) return 0;
            if(g_batchPressed>=0) {
                int action=std::exchange(g_batchPressed,-1);
                if(GetCapture()==hwnd)ReleaseCapture();
                if(action==BatchActionAt(x,y)) {
                    if(action==0)ClearAppSelection();
                    else LaunchSelectedApps();
                }
                Render();return 0;
            }
            if(g_selectionPointer.pending)MoveAppSelection(x,y);
            auto selection=std::exchange(g_selectionPointer,{});
            if(selection.active||selection.app) {
                g_pressed=-1;g_pressedEmpty=false;
                if(GetCapture()==hwnd)ReleaseCapture();
                int index=ItemAt(x,y);
                if(selection.app&&index>=0&&Items()[index].app==selection.app)ToggleAppSelection(selection.app);
                Render();return 0;
            }
            g_searchDrag = false;
            bool dragged = g_drag.active;
            // ReleaseCapture fires WM_CAPTURECHANGED synchronously, and that
            // handler clears the press; snapshot it before the capture goes.
            int powerPressed = g_powerPressed;
            int pressed = std::exchange(g_pressed,-1);
            bool pressedEmpty = std::exchange(g_pressedEmpty,false);
            EndDrag(true);  // Before the capture goes, which would undo it.
            if (GetCapture() == hwnd) {
                ReleaseCapture();
            }
            g_powerPressed = -1;
            if (powerPressed >= 0) {
                if (PowerAt(x, y) == powerPressed) {
                    PowerClick(powerPressed);  // May hide the grid.
                }
                Render();
                return 0;
            }
            if (dragged) {
                return 0;
            }
            int item = ItemAt(x, y);
            if (pressed >= 0 && item == pressed) {
                ActivateItem(item);
            } else if (pressedEmpty && item < 0 && g_appSelection.empty() && !ClickKeepsOpen(x, y)) {
                // Clicking on empty space closes, like in Launchpad.
                if (g_openFolder) {
                    CloseFolder();
                } else {
                    Close();
                }
            } else {
                Render();
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            if(g_selectionPointer.active) {
                g_appSelection=std::move(g_selectionPointer.base);SelectionChanged();
            }
            g_selectionPointer={};
            g_pressed=-1;g_pressedEmpty=false;g_batchPressed=-1;g_groupEditButton=-1;
            CancelGroupPointer();
            EndDrag(false);  // Another window took the mouse.
            g_searchDrag = false;
            g_powerPressed = -1;
            return 0;

        case WM_RBUTTONUP: {
            if (g_drag.active||g_groupGesture.group||g_selectionPointer.pending||g_batchLaunching) {
                return 0;
            }
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if(ToolBarConsumes(x,y))return 0;
            POINT pt{x, y};
            ClientToScreen(hwnd, &pt);
            int item = ItemAt(x, y);
            int group = GroupAtPoint(x,y);
            if (group >= 0 && (item < 0 || g_groupEditMode)) {
                ShowGroupMenu(hwnd,g_groupGeometry[group].group,pt);
            } else if (item >= 0) {
                ShowTileMenu(hwnd, item, pt);
            } else if (!g_openFolder) {
                ShowOptionsMenu(hwnd, pt);
            }
            return 0;
        }

        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
            if(msg==WM_MOUSEWHEEL&&!g_openFolder&&!(GET_KEYSTATE_WPARAM(wParam)&MK_SHIFT)&&
                g_settings.pages) {
                int page=std::max(0,(int)std::lround(g_scroll/std::max(1,g_layout.width)));
                float limit=MaxPagePan(page);
                float next=std::clamp(g_groupPagePan-GET_WHEEL_DELTA_WPARAM(wParam)*
                    g_layout.cellHeight/WHEEL_DELTA,0.f,limit);
                if(next!=g_groupPagePan) {
                    g_groupPagePan=next;
                    if(g_groupGesture.group)FreeGroupPointerMove(g_groupGesture.at.x,g_groupGesture.at.y);
                    Render();
                    return 0;
                }
            }
            if(msg==WM_MOUSEWHEEL&&!g_openFolder&&ScrollFreeGroup(hwnd,lParam,GET_WHEEL_DELTA_WPARAM(wParam)))return 0;
            if (g_openFolder) {
                if (msg == WM_MOUSEWHEEL) {
                    g_wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
                    if (std::abs(g_wheelDelta) >= WHEEL_DELTA) {
                        FolderLayout F = GetFolderLayout();
                        g_folderRow =
                            std::clamp(g_folderRow - g_wheelDelta / WHEEL_DELTA,
                                       0, F.rows - F.visibleRows);
                        g_wheelDelta %= WHEEL_DELTA;
                        Render();
                    }
                }
                return 0;
            }
            // Sideways (tilt wheel, touchpad swipe) only flips pages.
            if (msg == WM_MOUSEHWHEEL && !g_settings.pages) {
                return 0;
            }
            g_wheelDelta += msg == WM_MOUSEWHEEL
                                ? GET_WHEEL_DELTA_WPARAM(wParam)
                                : -GET_WHEEL_DELTA_WPARAM(wParam);
            if (std::abs(g_wheelDelta) >= WHEEL_DELTA) {
                ScrollRows(-g_wheelDelta / WHEEL_DELTA);
                g_wheelDelta %= WHEEL_DELTA;
            }
            return 0;

        case WM_IME_STARTCOMPOSITION:
            g_imeComposing = true;
            PositionIme(hwnd);
            break;
        case WM_IME_ENDCOMPOSITION:
            g_imeComposing = false;
            break;
        case WM_IME_COMPOSITION: {
            PositionIme(hwnd);
            if ((lParam & GCS_RESULTSTR) && (!g_openFolder || g_folderRenaming)) {
                HIMC ime = ImmGetContext(hwnd);
                if (ime) {
                    LONG bytes = ImmGetCompositionStringW(ime, GCS_RESULTSTR, nullptr, 0);
                    if (bytes > 0 && bytes <= 65536) {
                        std::wstring committed((size_t)bytes / sizeof(WCHAR), L'\0');
                        if (ImmGetCompositionStringW(ime, GCS_RESULTSTR, committed.data(), bytes) == bytes) {
                            std::erase_if(committed, [](WCHAR c) { return c < 32; });
                            EditPush(0);
                            InsertAtCaret(committed);
                            EditDone();
                            EditChanged();
                        }
                    }
                    ImmReleaseContext(hwnd, ime);
                }
                return 0;
            }
            break;
        }
        case WM_KEYDOWN:
            if (g_imeComposing) break;
            OnKeyDown(wParam);
            return 0;

        case WM_CHAR:
            OnChar((WCHAR)wParam);
            return 0;

        case WM_CLOSE:  // Alt+F4
            Close();
            return 0;

        case WM_DPICHANGED:
            if (g_shown && std::abs(HIWORD(wParam)/96.0f-g_layout.scale)>0.001f) PostMessageW(hwnd,WM_APP+62,0,0);
            return 0;

        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            RefreshSystemAnimations();
            if(msg==WM_SETTINGCHANGE&&g_settings.theme==L"system")PostMessageW(hwnd,WM_APP_RELOAD,0,0);
            if (g_shown && (msg == WM_DISPLAYCHANGE || wParam == SPI_SETWORKAREA)) {
                if(ConfirmGroupEditClose())HideNow();
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK BackdropWndProc(HWND hwnd,
                                 UINT msg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND: {
            // Black with zero alpha: only the DWM effect shows.
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect((HDC)wParam, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
            return 1;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            FillRect(dc, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HANDLE g_thread;
HANDLE g_readyEvent;
HANDLE g_integrationLease;

DWORD WINAPI StartThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    OleInitialize(nullptr);
    g_frameWake = CreateWaitableTimerExW(nullptr, nullptr, 0x00000002 /* high resolution */, TIMER_ALL_ACCESS);
    if (!g_frameWake) g_frameWake = CreateWaitableTimerW(nullptr, FALSE, nullptr);
    LoadSettings();
#ifndef SEAMLESS_PREVIEW
    // Storage was validated before installing shell hooks in Wh_ModInit.
#endif

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        (IUnknown**)g_dwrite.GetAddressOf());

    HINSTANCE instance = (HINSTANCE)&__ImageBase;
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWindowClass;
    RegisterClassExW(&wc);

    WNDCLASSEXW bc{sizeof(bc)};
    bc.lpfnWndProc = BackdropWndProc;
    bc.hInstance = instance;
    bc.lpszClassName = kBackdropClass;
    RegisterClassExW(&bc);

    g_backdrop = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE |
            WS_EX_TRANSPARENT,
        kBackdropClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance,
        nullptr);
    HWND wnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
                               kWindowClass, L"Start", WS_POPUP, 0, 0, 0, 0,
                               nullptr, nullptr, instance, nullptr);
#ifdef SEAMLESS_PREVIEW
    // Isolated UI QA only: let automation enumerate the otherwise tool-only canvas.
    if(Preview::uiCheck&&wnd) {
        auto style=GetWindowLongPtrW(wnd,GWL_EXSTYLE);
        SetWindowLongPtrW(wnd,GWL_EXSTYLE,(style&~WS_EX_TOOLWINDOW)|WS_EX_APPWINDOW);
        SetWindowTextW(wnd,L"RYJS Launchpad UI validation");
    }
#endif
    bool ok = g_d2d && g_dwrite && g_backdrop && wnd;
    if (ok) {
        g_wnd = wnd;
#ifndef SEAMLESS_PREVIEW
        StartButtonHooksStart();
#endif
        g_taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
        ChangeWindowMessageFilterEx(wnd, g_taskbarCreated, MSGFLT_ALLOW, nullptr);
        UpdateTrayIcon(wnd);
        // The app list is read a bit later, so it doesn't weigh on explorer's
        // startup.
        SetTimer(wnd, kTimerFirstLoad, 3000, nullptr);
    }
    SetEvent(g_readyEvent);
#ifdef SEAMLESS_PREVIEW
    if (ok) {
        if (!Preview::smoke) SetTimer(wnd, kTimerFirstLoad, 1, nullptr);
        PostMessageW(wnd, Preview::smoke ? WM_APP + 60 : (Preview::settingsOnStart ? WM_APP + 20 : WM_APP_TOGGLE), 0, 0);
    }
#endif

    if (ok) {
        MSG msg;
        bool quit = false;
        while (!quit) {
            DWORD count = g_frameWake && g_frameTimer ? 1 : 0;
            DWORD ready = MsgWaitForMultipleObjectsEx(count, count ? &g_frameWake : nullptr, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (count && ready == WAIT_OBJECT_0 && g_frameTimer) OnFrame();
            // Bound message draining so continuous input cannot starve frames.
            for (int budget = 0; budget < 64 && PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE); budget++) {
                if (msg.message == WM_QUIT) { quit = true; break; }
                TranslateMessage(&msg); DispatchMessageW(&msg);
            }
            if (ready == WAIT_FAILED) { Wh_Log(L"Frame message wait failed: %u", GetLastError()); break; }
        }
    } else {
        Wh_Log(L"Failed to create Start");
        if (wnd) {
            DestroyWindow(wnd);
        }
    }
    g_wnd = nullptr;
    StopFullBackupJob();
#ifndef SEAMLESS_PREVIEW
    StartButtonHooksStop();
#endif

    g_workerStop = true;
    StopWebIcons();
    if (g_worker) {
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    // A list delivered after the window stopped handling messages.
    MSG msg;
    while (PeekMessageW(&msg, nullptr, WM_APP_APPS, WM_APP_APPS, PM_REMOVE)) {
        delete (AppList*)msg.lParam;
    }

    if (g_backdrop) {
        DestroyWindow(g_backdrop);
        g_backdrop = nullptr;
    }
    AppList().swap(g_apps);  // Also frees the memory (no_destroy).
    Tiles().swap(g_tiles);
    Tiles().swap(g_visible);
    CancelGroupPointer();
    std::vector<ExpandedGroupGeometry>().swap(g_groupGeometry);
    std::vector<Cell>().swap(g_groupCells);
    g_groupTransitions.clear();
    g_groupResizeHover.reset();
    g_selectionPointer={};
    AppSelection().swap(g_appSelection);
    std::unordered_set<std::wstring>().swap(g_selectedAppKeys);
    g_groupEntryOrder.clear();
    g_openFolder = nullptr;
    g_drag = {};
    g_backdropImage = {};
    g_panelBlur = {};
    FreeSurface(g_surface);
    g_labelFormat.Reset();
    g_labelCache.clear();
    g_groupTitleRenders.clear();
    // The menu keeps its own DirectWrite objects for the life of the module.
    LaunchpadMenu::g_menu.textFormat.Reset();
    LaunchpadMenu::g_menu.boldFormat.Reset();
    LaunchpadMenu::g_menu.hintFormat.Reset();
    g_searchFormat.Reset();
    g_messageFormat.Reset();
    g_iconFormat.Reset();
    g_tipFormat.Reset();
    g_groupFormat.Reset();
    ResetTarget();
    if (g_frameWake) { CloseHandle(g_frameWake); g_frameWake = nullptr; }
    g_dwrite.Reset();
    g_d2d.Reset();
    UnregisterClassW(kWindowClass, instance);
    UnregisterClassW(kBackdropClass, instance);
    UnregisterClassW(kGroupNameClass, instance);
    UnregisterClassW(kEntryEditorClass, instance);
    LaunchpadMenu::UnregisterMenuClass();
    OleUninitialize();
    return 0;
}

// With "Launch folder windows in a separate process", folder windows run in
// their own explorer.exe, which never shows Start.
bool IsFolderWindowsProcess() {
    std::wstring commandLine = ToLower(GetCommandLineW());
    return commandLine.find(L"/factory") != std::wstring::npos ||
           commandLine.find(L"-embedding") != std::wstring::npos ||
           commandLine.find(L"/separate") != std::wstring::npos;
}

#ifndef SEAMLESS_PREVIEW
bool HookShowStartView() {
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return false;
    }
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original,
            XamlLauncher_ShowStartView_Hook,
        },
    };
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}

#endif
}  // namespace

#ifndef SEAMLESS_PREVIEW
BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (IsFolderWindowsProcess()) {
        Wh_Log(L"Folder windows process; Start stays in the taskbar's one");
        return FALSE;
    }
    // A differently named DLL must not stack a second Start interceptor on a
    // known running predecessor. Never disable or unload that mod for the user.
    if (FindWindowW(L"WindhawkSeamlessLaunchpad", nullptr) ||
        FindWindowW(L"WindhawkFullscreenStartPlus", nullptr)) {
        Wh_Log(L"Another Launchpad is running. Disable it before enabling RYJS; original data was not changed.");
        return FALSE;
    }
    g_integrationLease = CreateMutexW(nullptr, FALSE, L"Local\\RYJSLaunchpad.Integration");
    if (!g_integrationLease || GetLastError() == ERROR_ALREADY_EXISTS) {
        if (g_integrationLease) CloseHandle(g_integrationLease);
        g_integrationLease = nullptr;
        return FALSE;
    }
    bool storageReady = false;
    try { storageReady = PrepareStorage(); } catch (...) { }
    if (!storageReady) {
        if (DataRoot().empty()) {
            Wh_Log(L"RYJS storage path is unavailable; no hooks installed.");
            CloseHandle(g_integrationLease); g_integrationLease = nullptr;
            return FALSE;
        }
        // A damaged or unsupported data folder must not take Start away for
        // good: open what can be read and keep every write disabled, so the
        // user can still restore a backup from the settings window.
        g_dataReadOnly = true;
        g_layoutReadOnly = true;
        Wh_Log(L"RYJS storage is damaged or unsupported; opening read-only.");
    }
    LoadSettings();
    if (!HookShowStartView()) {
        CloseHandle(g_integrationLease); g_integrationLease = nullptr;
        return FALSE;
    }
    HookXamlStartVisibility();
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, StartThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        CloseHandle(g_integrationLease); g_integrationLease = nullptr;
        return FALSE;
    }
    g_keyThread =
        CreateThread(nullptr, 0, KeyThread, nullptr, 0, &g_keyThreadId);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (!g_thread) {
        CloseSettingsWindow();
        if (g_integrationLease) { CloseHandle(g_integrationLease); g_integrationLease = nullptr; }
        return;
    }
    if (g_keyThread) {
        // Repeated: the thread may not have its message queue yet.
        do {
            PostThreadMessageW(g_keyThreadId, WM_QUIT, 0, 0);
        } while (WaitForSingleObject(g_keyThread, 100) == WAIT_TIMEOUT);
        CloseHandle(g_keyThread);
        g_keyThread = nullptr;
    }
    WaitForSingleObject(g_readyEvent, INFINITE);
    if (HWND wnd = g_wnd) {
        PostMessageW(wnd, WM_APP_QUIT, 0, 0);
    }
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
    // Sleep and hibernate return only after the PC wakes; every other action
    // returns at once, so this join cannot hang a normal unload.
    if (g_powerThread) {
        WaitForSingleObject(g_powerThread, INFINITE);
        CloseHandle(g_powerThread);
        g_powerThread = nullptr;
    }
    // Last: until the grid's thread ends, its "..." menu can open the window.
    CloseSettingsWindow();
    if (g_integrationLease) { CloseHandle(g_integrationLease); g_integrationLease = nullptr; }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (HWND wnd = g_wnd) {
        PostMessageW(wnd, WM_APP_RELOAD, 0, 0);
    }
    if (HWND wnd = g_settingsWnd) {
        PostMessageW(wnd, WM_APP_SETTINGS_RELOAD, 0, 0);
    }
}

#endif // !SEAMLESS_PREVIEW
