// ==WindhawkMod==
// @id              taskbar-modding
// @name            Taskbar modding
// @name:zh-CN      任务栏魔改
// @description     Custom Win11 taskbar: floating rounded, left-click grouped icons cycle windows. Auto-hide on maximized/overlapped windows. Adjust tray icons, macOS Dock hover zoom, 2-line custom clock + real-time performance stats.
// @description:zh-CN 自定义Win11任务栏：浮动圆角、左键分组图标循环窗口。在最大化/重叠的窗口自动隐藏。调整托盘图标、macOS Dock悬停缩放、两行自定义时钟+实时性能统计
// @version         1.16.3
// @author          chaochenzhudian
// @github          https://github.com/chaochenzhudian
// @include         explorer.exe
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -lole32 -loleaut32 -lpdh -lruntimeobject -luser32 -lwindowsapp -ldxgi -lwininet -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 任务栏魔改（Taskbar modding）

## 悬浮圆角任务栏

让 Windows 11 任务栏变成悬浮的圆角条：长度、圆角、任务栏区域高度、悬浮间距都可以调，
悬浮条以外的空白区域可以点击穿透到桌面，托盘的右键菜单和左键快速设置面板都会对齐到悬浮条右端。
背景可直接选择跟随系统、完全透明、色彩通透定制、纯色或线性渐变，
也可以继续使用自定义 XAML 画刷。

## 左键点击循环切换窗口

左键点击“合并的任务栏项”时，在该分组的窗口之间循环切换，而不是弹出缩略图预览；
按住 Ctrl 点击仍然显示缩略图预览。此外 Win+数字 快捷键也会循环切换窗口。

这部分功能来自 m417z 的 "Cycle through taskbar windows on click"
（taskbar-left-click-cycle）mod。

## 最大化时自动隐藏任务栏

在系统里开启“自动隐藏任务栏”后，可以选择只在当前有窗口最大化（或与任务栏区域相交）时自动隐藏，
也可以设为任何时刻自动隐藏；其余模式会保持显示。可以设置为只依据前台窗口、排除指定程序、或只对主显示器生效。

这部分功能来自 m417z 的 "Taskbar auto-hide when maximized"
（taskbar-auto-hide-when-maximized）mod。

## 托盘系统图标

可以隐藏任务栏系统托盘里的图标：音量、网络、电池、麦克风、定位（GPS）、
Studio Effects、Recall、语言栏（含附加图标）、铃铛（可设为无新通知时隐藏），
还可以把电池图标改成灰度，以及调整右下角“显示桌面”按钮的宽度。

这部分功能来自 m417z 的 "Taskbar tray system icon tweaks"
（taskbar-tray-system-icon-tweaks）mod。

## 任务栏时钟

自定义时钟的时间/日期格式（含 12/24 小时与秒）、星期显示方式，以及时钟上下两行和悬停提示的内容。
内容支持占位符：`%time%`、`%date%`、`%weekday%`、`%weekday_num%`、`%weeknum%`、
`%weeknum_iso%`、`%dayofyear%`、`%timezone%`、`%cpu%`、`%ram%`、`%ram_used%`、
`%ram_total%`、`%cpu_temp%`、`%cpu_temp_f%`、`%gpu%`、`%battery%`。

新增支持：

- 网络速率：`%upload_speed%`、`%download_speed%`、`%total_speed%`
- 磁盘速率：`%disk_read%`、`%disk_write%`、`%disk_total%`
- 显存：`%vram%`、`%vram_used%`、`%vram_total%`、`%vram_shared%`、
  `%vram_shared_used%`、`%vram_shared_total%`
- 媒体播放器：`%media_title%`、`%media_artist%`、`%media_album%`、
  `%media_status%`、`%media_info%`
- 天气与新闻：`%weather%`、`%news%`（第一条新闻）、`%web1%`、`%web1_full%`，
  更多条目可继续使用 `%web2%` 等
- 换行：`%n%` 或 `%newline%`

时钟上下行分别支持自定义颜色、字体、字号、字重、字形、拉伸、对齐、字符间距和行高。
将上行/下行设为 `-` 可保留系统默认的时间/日期。

示例：

- 网络与磁盘：`↓%download_speed% ↑%upload_speed%  R:%disk_read% W:%disk_write%`
- 显存：`VRAM:%vram% (%vram_used%/%vram_total%)`
- 媒体：`%media_status% %media_info%`
- 天气：`%weather%`
- 第一条新闻：`%news%`，完整内容可用悬停提示 `%web1_full%`

这部分功能改编自 m417z 的 "Taskbar Clock Customization"
（taskbar-clock-customization）mod。

**分区开关**：每个分区顶部各有一个总开关，关掉后立即恢复系统原有行为
**说明**：本 MOD 不仅美化了任务栏同时也整合了众多优秀创作者的作品，专门针对国内用户做适配，方便大家看懂上手，以此推广这款软件强大的能力
**特别鸣谢**：m417z's mods Windows 11 Taskbar Styler, Cycle through taskbar windows on click, Taskbar auto-hide when maximized, Taskbar tray system icon tweaks, Taskbar Clock Customization) and Taskbar Dock Animation Plus
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- floatingBar:
  - enabled: true
    $name: 启用悬浮任务栏
    $description: >-
      关闭后任务栏恢复系统原有的外观与位置（空白区域不再点击穿透，
      托盘菜单和快速设置面板也不再对齐到悬浮条右端）。
  - taskbarWidth: 1250
    $name: 任务栏长度
    $description: >-
      悬浮条的总长度（包含右下角系统托盘区），单位为物理像素。0 表示根据内容自动宽度。
  - taskbarAreaHeight: 60
    $name: 任务栏区域高度
    $description: >-
      悬浮条顶端到屏幕底端的距离（即整个“任务栏区域”的高度），单位为物理像素。
      0 表示跟随系统任务栏高度（一般 48 像素）。悬浮条高度 = 本项 − 悬浮间距。
  - floatMargin: 5
    $name: 悬浮间距
    $description: >-
      悬浮条底端到屏幕底端的距离，单位为物理像素。数值越大，悬浮条整体离屏幕底边越高。
  - cornerRadius: 14
    $name: 任务栏圆角半径
    $description: 悬浮条的圆角半径，单位为物理像素。
  - hoverCornerRadius: 20
    $name: 任务栏待选框圆角半径
    $description: >-
      任务栏图标悬停时待选框的圆角半径，单位为物理像素。
      0 表示直角，数值越大越圆滑。
  - backgroundStyle: custom
    $name: 背景画刷
    $description: >-
      选择悬浮条背景样式。预设会使用下方的颜色、透明度和渐变参数；
      “自定义 XAML”使用原来的自定义画刷标记，留空时等同跟随系统。
    $options:
    - custom: 自定义 XAML（留空跟随系统）
    - system: 跟随系统（系统原背景）
    - transparent: 完全透明
    - translucent: 色彩通透定制
    - solid: 纯色
    - gradient: 线性渐变
  - backgroundColor: "#202020"
    $name: 背景颜色
    $description: >-
      用于色彩通透定制和纯色样式。格式为 #RRGGBB 或 #AARRGGBB。
      纯色样式会保留颜色自带的透明度；色彩通透定制使用下方的透明度设置。
  - backgroundOpacity: 75
    $name: 背景透明度
    $description: >-
      0 = 完全透明，100 = 完全不透明。用于色彩通透定制和渐变样式。
  - gradientColor1: "#1F2937"
    $name: 渐变起始颜色
    $description: 线性渐变的起始颜色，格式为 #RRGGBB 或 #AARRGGBB。
  - gradientColor2: "#111827"
    $name: 渐变结束颜色
    $description: 线性渐变的结束颜色，格式为 #RRGGBB 或 #AARRGGBB。
  - gradientAngle: 45
    $name: 渐变角度
    $description: >-
      0 = 从左到右，45 = 左上到右下，90 = 从上到下，180 = 从右到左。
  - gradientStartPosition: 0
    $name: 渐变起始位置（%）
    $description: >-
      第一个颜色在悬浮条渐变方向上的落点，百分比相对于悬浮条的实际尺寸。
      0 = 起点，100 = 终点。
  - gradientEndPosition: 100
    $name: 渐变结束位置（%）
    $description: >-
      第二个颜色在悬浮条渐变方向上的落点，百分比相对于悬浮条的实际尺寸。
      0 = 起点，100 = 终点；与起始位置相同可形成清晰分界，
      0 / 100 为普通整条渐变。
  - backgroundBrush: ""
    $name: 自定义 XAML
    $description: >-
      仅当背景画刷选择“自定义 XAML”时使用。留空则沿用系统任务栏原有背景。
      示例：<SolidColorBrush Color="#CC202020" />
  $name: 悬浮任务栏
- cycling:
  - cycleThroughWindows: true
    $name: 左键点击循环切换窗口
    $description: >-
      左键点击“合并的任务栏项”时，在该分组内的窗口之间循环切换，而不是弹出缩略图预览；
      按住 Ctrl 点击仍会显示缩略图预览。此外，Win+数字 快捷键也会循环切换窗口。
  $name: 窗口切换
- autoHide:
  - enabled: true
    $name: 启用自动隐藏任务栏
    $description: >-
      关闭后恢复系统本身的自动隐藏行为（不再只在窗口最大化或与任务栏相交时隐藏）。
  - mode: intersected
    $name: 模式
    $description: >-
      “任何时刻自动隐藏”会忽略窗口是否最大化或与任务栏相交；
      鼠标移到屏幕边缘时仍可按系统的自动隐藏行为临时唤出。
    $options:
    - intersected: 最大化或与任务栏相交时隐藏
    - maximized: 仅窗口最大化时隐藏
    - always: 任何时刻自动隐藏
    - never: 从不自动隐藏
  - foregroundWindowOnly: false
    $name: 仅对前台窗口生效
    $description: 只根据当前前台窗口来判断是否需要隐藏任务栏。
  - excludedPrograms: [""]
    $name: 排除的程序
    $description: >-
      这些程序的窗口即使最大化或与任务栏相交，也不会让任务栏自动隐藏。
      可以填进程名、完整路径或应用 ID，例如：mspaint.exe、
      C:\Windows\System32\notepad.exe、Microsoft.WindowsCalculator_8wekyb3d8bbwe!App
  - primaryMonitorOnly: false
    $name: 仅主显示器
    $description: >-
      只对主显示器的任务栏生效，副屏沿用系统默认的自动隐藏行为。
  $name: 自动隐藏任务栏
- trayIcons:
  - hideVolumeIcon: false
    $name: 隐藏音量图标
  - hideNetworkIcon: false
    $name: 隐藏网络图标
  - hideBatteryIcon: false
    $name: 隐藏电池图标
  - grayscaleBatteryIcon: false
    $name: 电池图标灰度化
    $description: >-
      电池图标使用标准文字颜色，而不是系统默认的彩色（绿/黄等）样式。
  - hideMicrophoneIcon: false
    $name: 隐藏麦克风图标
  - hideGeolocationIcon: false
    $name: 隐藏定位（GPS）图标
  - hideStudioEffectsIcon: false
    $name: 隐藏 Studio Effects 图标
  - hideRecallIcon: false
    $name: 隐藏 Recall 图标
  - hideLanguageBar: false
    $name: 隐藏语言栏
  - hideLanguageSupplementaryIcons: false
    $name: 隐藏语言栏附加图标
  - hideBellIcon: never
    $name: 隐藏铃铛图标
    $options:
    - never: 从不
    - whenInactive: 没有新通知时
    - whenInactiveAndNoDnd: 没有新通知且未开启“勿扰”时
    - always: 总是
  - showDesktopButtonWidth: 12
    $name: “显示桌面”按钮宽度
    $description: >-
      单位为 XAML 单位（DIP），范围 0-800，默认 12。
  $name: 托盘系统图标
- dockAnimation:
  - enabled: true
    $name: 启用悬停放大动画
    $description: >-
      鼠标在任务栏图标上移动时，像 macOS Dock 那样放大光标附近的图标并推开相邻图标。
  - animationType: 0
    $name: 动画曲线
    $description: 0 = 余弦（平滑），1 = 线性，2 = 指数（更灵敏）
  - maxScale: 140
    $name: 最大缩放（%）
    $description: 图标最大可放大到多少（101-220，例如 140 = 放大 40%）
  - effectRadius: 100
    $name: 效果半径（像素）
    $description: 距离光标多远范围内应用动画
  - spacingFactor: 50
    $name: 间距灵敏度（%）
    $description: 动画时相邻图标被推开的程度
  - bounceDelay: 500
    $name: 弹跳开始延迟（毫秒）
    $description: 光标停下多久后开始空闲“呼吸”动画
  - focusDuration: 150
    $name: 聚焦动画时长（毫秒）
    $description: 淡入/淡出动画的时长
  - disableVerticalBounce: false
    $name: 禁用垂直“弹跳”
    $description: 关闭上下“呼吸”位移（对垂直任务栏有用）
  - taskbarLabelsMode: false
    $name: 任务栏标签兼容模式
    $description: >-
      若任务栏显示窗口标题（矩形按钮），启用此项以修正动画居中。
  - excludeSystemButtonsMode: 0
    $name: 排除系统按钮
    $description: 0 = 全部动画，1 = 排除开始按钮，2 = 只对应用图标动画
  - lerpSpeed: 60
    $name: 平滑度（Lerp 速度）
    $description: 越高越灵敏，越低越平滑；0 = 直接应用（不平滑）
  - disableBounce: false
    $name: 禁用弹跳效果
    $description: 完全关闭空闲时的“呼吸”动画
  $name: Dock 悬停动画
- clock:
  - enabled: true
    $name: 启用时钟自定义
    $description: >-
      自定义任务栏时钟的时间/日期格式、上下两行内容与悬停提示文本。
  - showSeconds: false
    $name: 显示秒
    $description: 是否在时间中显示秒（会以秒级刷新时钟，略微增加开销）。
  - timeFormat: "HH':'mm':'ss"
    $name: 时间格式
    $description: >-
      %time% 使用的格式（GetTimeFormatEx 语法），留空使用系统默认。
      参考：https://learn.microsoft.com/windows/win32/api/datetimeapi/nf-datetimeapi-gettimeformatex
  - dateFormat: "yyyy年MM月dd日"
    $name: 日期格式
    $description: >-
      %date% 使用的格式（GetDateFormatEx 日期图片语法），留空使用系统默认。
  - dateLocale: ""
    $name: 日期区域
    $description: >-
      用于日期和星期的区域名（如 zh-CN、en-US），留空使用系统默认。
  - weekdayFormat: dddd
    $name: 星期格式
    $options:
    - dddd: 完整星期名
    - ddd: 缩写星期名
    - custom: 自定义
  - weekdayFormatCustom: "周日, 周一, 周二, 周三, 周四, 周五, 周六"
    $name: 自定义星期名
    $description: 星期格式设为“自定义”时使用，按周日到周六的顺序用逗号分隔。
  - topLine: "ram:%ram%  ACPI:%cpu_temp%"
    $name: 上行内容
    $description: >-
      时钟上行显示的文本，设为 "-" 使用系统默认时间。可用占位符：%time%、%date%、
      %weekday%、%weekday_num%、%weeknum%、%weeknum_iso%、%dayofyear%、%timezone%、
      %cpu%、%ram%、%cpu_temp%、%gpu%、网络/磁盘速率、显存、媒体播放器、
      天气 %weather%、新闻 %web1% 等，完整列表见模块说明。
  - bottomLine: "cpu:%cpu%   gpu:%gpu%"
    $name: 下行内容
    $description: 时钟下行显示的文本，占位符同上，设为 "-" 使用系统默认日期。
  - tooltipLine: "%date% %weekday% %time%"
    $name: 悬停提示附加文本
    $description: >-
      鼠标悬停时显示的文本，占位符同上。媒体标题、完整新闻内容等较长的文本
      建议放在这里，例如 %media_title% - %media_artist% 或 %web1_full%。
  - tooltipLineMode: replace
    $name: 悬停提示模式
    $options:
    - append: 追加到系统提示之后
    - replace: 替换系统提示
  - percentageFormat: spacePaddingAndSymbol
    $name: 百分比格式
    $options:
    - spacePaddingAndSymbol: 补空格并加百分号
    - spacePadding: 补空格，仅数字
    - singleSpacePadding: 补一个空格，仅数字（等宽字体）
    - zeroPadding: 补零，仅数字
    - noPadding: 不补，仅数字
  - updateInterval: 1
    $name: 性能数据刷新间隔（秒）
  - networkMetricsFormat: mbsDynamic
    $name: 网络速率格式
    $options:
    - mbs: MB/s
    - mbsNumberOnly: MB/s，仅数字
    - mbsDynamic: MB/s 或 KB/s（自动）
    - mbits: MBit/s
    - mbitsNumberOnly: MBit/s，仅数字
    - mbitsDynamic: MBit/s 或 KBit/s（自动）
  - networkMetricsFixedDecimals: -1
    $name: 网络速率小数位
    $description: -1 表示自动保持等宽，0 及以上表示固定小数位。
  - diskMetricsFormat: sameAsNetwork
    $name: 磁盘速率格式
    $options:
    - sameAsNetwork: 与网络速率格式相同
    - mbs: MB/s
    - mbsNumberOnly: MB/s，仅数字
    - mbsDynamic: MB/s 或 KB/s（自动）
    - mbits: MBit/s
    - mbitsNumberOnly: MBit/s，仅数字
    - mbitsDynamic: MBit/s 或 KBit/s（自动）
  - diskMetricsFixedDecimals: -1
    $name: 磁盘速率小数位
    $description: -1 表示自动保持等宽，0 及以上表示固定小数位。
  - networkAdapterName: ""
    $name: 网络适配器名称
    $description: 留空时汇总所有网络适配器；支持部分匹配，例如 Intel、Ethernet、WLAN。
  - gpuAdapterName: ""
    $name: GPU 适配器名称
    $description: 留空时自动选择显存最大的 GPU；支持部分匹配。
  - mediaInfoFormat: "%media_artist% - %media_title%"
    $name: 媒体播放时的信息格式
  - noMediaText: "无媒体播放"
    $name: 没有媒体播放时的文本
  - mediaMaxLength: 28
    $name: 媒体信息最大长度
    $description: 超出长度时用省略号截断，0 表示不限制。
  - mediaRemoveBrackets: false
    $name: 移除媒体信息中的括号内容
  - mediaIgnoredPlayers: [""]
    $name: 忽略的媒体播放器
    $description: 按应用 ID 进行不区分大小写的部分匹配，例如 Spotify、vlc。
  - weatherLocation: ""
    $name: 天气位置
    $description: 留空使用当前位置；也可填写城市或 wttr.in 支持的位置。
  - weatherFormat: "%c %t %w"
    $name: 天气格式
    $description: 使用 wttr.in 的格式语法，例如 "%c 🌡️%t 🌬️%w"。
  - weatherUnits: autoDetect
    $name: 天气单位
    $options:
    - autoDetect: 自动
    - uscs: 美制单位
    - metric: 公制
    - metricMsWind: 公制，风速使用 m/s
  - webContentsItems:
    - - url: "https://rss.nytimes.com/services/xml/rss/nyt/World.xml"
        $name: 新闻/网页地址
      - blockStart: "<item>"
        $name: 内容块起点
      - start: "<title>"
        $name: 正文起点
      - end: "</title>"
        $name: 正文终点
      - maxLength: 28
        $name: 最大长度
  - webContentsUpdateInterval: 10
    $name: 天气/新闻更新间隔（分钟）
  - timeStyle:
    - textColor: ""
      $name: 上行文字颜色
      $description: 留空跟随系统；支持 #RRGGBB、#AARRGGBB 或颜色名称。
    - fontFamily: ""
      $name: 上行字体
    - fontSize: 0
      $name: 上行字号
      $description: 0 表示系统默认。
    - fontWeight: ""
      $name: 上行字重
      $options:
      - "": 默认
      - Thin: 极细
      - ExtraLight: 特细
      - Light: 细
      - SemiLight: 半细
      - Normal: 常规
      - Medium: 中等
      - SemiBold: 半粗
      - Bold: 粗
      - ExtraBold: 特粗
      - Black: 黑体
      - ExtraBlack: 特黑
    - fontStyle: ""
      $name: 上行字形
      $options:
      - "": 默认
      - Normal: 常规
      - Oblique: 倾斜
      - Italic: 斜体
    - fontStretch: ""
      $name: 上行字体拉伸
      $description: 仅部分字体支持。
    - textAlignment: ""
      $name: 上行对齐
      $options:
      - "": 默认
      - Left: 左对齐
      - Center: 居中
      - Right: 右对齐
      - Justify: 两端对齐
    - characterSpacing: 0
      $name: 上行字符间距
    - lineHeight: 0
      $name: 上行行高
      $description: 0 表示系统默认。
    $name: 时钟上行样式
  - dateStyle:
    - textColor: ""
      $name: 下行文字颜色
      $description: 留空跟随系统；支持 #RRGGBB、#AARRGGBB 或颜色名称。
    - fontFamily: ""
      $name: 下行字体
    - fontSize: 0
      $name: 下行字号
      $description: 0 表示系统默认。
    - fontWeight: ""
      $name: 下行字重
      $options:
      - "": 默认
      - Thin: 极细
      - ExtraLight: 特细
      - Light: 细
      - SemiLight: 半细
      - Normal: 常规
      - Medium: 中等
      - SemiBold: 半粗
      - Bold: 粗
      - ExtraBold: 特粗
      - Black: 黑体
      - ExtraBlack: 特黑
    - fontStyle: ""
      $name: 下行字形
      $options:
      - "": 默认
      - Normal: 常规
      - Oblique: 倾斜
      - Italic: 斜体
    - fontStretch: ""
      $name: 下行字体拉伸
      $description: 仅部分字体支持。
    - textAlignment: ""
      $name: 下行对齐
      $options:
      - "": 默认
      - Left: 左对齐
      - Center: 居中
      - Right: 右对齐
      - Justify: 两端对齐
    - characterSpacing: 0
      $name: 下行字符间距
    - lineHeight: 0
      $name: 下行行高
      $description: 0 表示系统默认。
    $name: 时钟下行样式
  $name: 任务栏时钟
*/
// ==/WindhawkModSettings==

// Taskbar modding
//
// Based on the floating-taskbar mod (1.6.7), with three more features added on
// top of it. The settings are grouped accordingly.
//
// 1. Floating bar (floatingBar.*) - from floating-taskbar.
//    Has an on/off switch (floatingBar.enabled).
// 2. Window cycling (cycling.*) - from the "Cycle through taskbar windows on
//    click" mod (taskbar-left-click-cycle):
//    Clicking a combined taskbar item cycles through its windows instead of
//    opening the thumbnail previews; Ctrl+click still opens the previews, and
//    the Win+# hotkeys cycle as well. Can be turned off in the settings.
// 3. Auto-hide when maximized (autoHide.*) - from the "Taskbar auto-hide when
//    maximized" mod (taskbar-auto-hide-when-maximized):
//    While the system's auto-hide taskbar option is on, the taskbar is kept
//    shown unless a window is maximized or intersects the taskbar area.
//    Has an on/off switch (autoHide.enabled).
// 4. Tray system icons (trayIcons.*) - from the "Taskbar tray system icon
//    tweaks" mod (taskbar-tray-system-icon-tweaks):
//    Hiding system tray icons (volume, network, battery, microphone, location,
//    Studio Effects, Recall, language bar, bell) and adjusting the battery icon
//    color and the "Show desktop" button width.
// 5. Dock hover animation (dockAnimation.*) - from "Taskbar Dock Animation
//    Plus" (taskbar-dock-animation-plus):
//    macOS-like scaling of the taskbar icons around the mouse cursor.
// 6. Taskbar clock (clock.*) - from the "Taskbar Clock Customization" mod
//    (taskbar-clock-customization):
//    Custom time/date formats, custom clock lines built from patterns, and
//    performance metrics such as CPU, RAM, GPU, CPU temperature and battery.
//
// Changes inherited from floating-taskbar 1.6.7:
// - The Quick Settings panel is positioned before it's shown, so that it no
//   longer appears at the default position and then jumps to the right end of
//   the floating bar. The panel is hosted by ShellHost.exe on recent Windows
//   versions, so the mod is now loaded into that process as well and adjusts
//   the panel position in SetWindowPos.
// - The taskbar frame's parent height is now restored as well, so that the
//   taskbar area shrinks back when taskbarAreaHeight is set to 0 or when the
//   mod is unloaded.
// - The hook entry points no longer let C++/WinRT exceptions escape, which
//   could crash explorer.
// - The settings and the Control Center state are now accessed in a
//   thread-safe way.
// - The LoadLibraryExW hook is kept installed while SystemTray.dll isn't
//   loaded yet, so that the tray menu hook can't be missed.
// - The mod waits for in-flight hook calls before being unloaded.

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <windows.h>
#include <dwmapi.h>
#include <dxgi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <shlwapi.h>
#include <wininet.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <functional>
#include <list>
#include <limits>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Automation;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Hosting;
using namespace winrt::Windows::UI::Xaml::Markup;
using namespace winrt::Windows::UI::Xaml::Media;

#if __cplusplus < 202302L
// Missing in older MinGW headers.
DECLARE_HANDLE(CO_MTA_USAGE_COOKIE);
WINOLEAPI CoIncrementMTAUsage(CO_MTA_USAGE_COOKIE* pCookie);
WINOLEAPI CoDecrementMTAUsage(CO_MTA_USAGE_COOKIE Cookie);
#endif

// The settings are written by LoadSettings on the mod's own thread, but they
// are read by the taskbar UI threads and by the Control Center watchdog
// thread. Keep the numeric values in atomics so that a settings change can
// never race with a read; the string setting is copied under g_settingsMutex.
std::atomic<int> g_settingTaskbarWidth{0};
std::atomic<int> g_settingTaskbarAreaHeight{0};
std::atomic<int> g_settingFloatMargin{0};
std::atomic<int> g_settingCornerRadius{0};
std::atomic<int> g_settingHoverCornerRadius{20};
std::atomic<bool> g_settingFloatingBarEnabled{true};
std::atomic<bool> g_settingCycleThroughWindows{true};
std::atomic<double> g_systemTaskbarContentHeightDip{0.0};
std::mutex g_settingsMutex;

double GetSystemTaskbarContentHeightDip() {
    double height = g_systemTaskbarContentHeightDip.load();
    return height > 0 ? height : 48.0;
}

enum class BackgroundStyle {
    custom,
    system,
    transparent,
    translucent,
    solid,
    gradient,
};

struct BackgroundSettings {
    BackgroundStyle style = BackgroundStyle::custom;
    std::wstring color = L"#202020";
    int opacity = 75;
    std::wstring gradientColor1 = L"#1F2937";
    std::wstring gradientColor2 = L"#111827";
    int gradientAngle = 45;
    int gradientStartPosition = 0;
    int gradientEndPosition = 100;
    std::wstring customXaml;
};

BackgroundSettings g_backgroundSettings;

// Auto-hide when maximized (from the "Taskbar auto-hide when maximized" mod).
enum class AutoHideMode {
    intersected,
    maximized,
    always,
    never,
};

std::atomic<int> g_settingAutoHideMode{(int)AutoHideMode::intersected};
std::atomic<bool> g_settingAutoHideEnabled{true};
std::atomic<bool> g_settingAutoHideForegroundWindowOnly{false};
std::atomic<bool> g_settingAutoHidePrimaryMonitorOnly{false};
std::mutex g_autoHideExcludedProgramsMutex;
std::unordered_set<std::wstring> g_autoHideExcludedPrograms;

// Auto-hide state, see the "Auto-hide when maximized" section below.
bool g_wasAutoHideProcessed = false;
bool g_wasAutoHideDisabled = false;
std::mutex g_winEventHookThreadMutex;
std::atomic<HANDLE> g_winEventHookThread{nullptr};
std::unordered_map<void*, HWND> g_taskbarsKeptShown;
std::unordered_map<HWND, void*> g_taskbarToViewCoordinator;
UINT_PTR g_pendingEventsTimer = 0;
std::atomic<HWND> g_multitaskingViewHwnd{nullptr};
std::atomic<HWND> g_altTabViewHwnd{nullptr};

// TrayUI::_HandleTrayPrivateSettingMessage
constexpr UINT kHandleTrayPrivateSettingMessage = WM_USER + 0x1CA;

enum {
    kTrayPrivateSettingAutoHideGet = 3,
    kTrayPrivateSettingAutoHideSet = 4,
};

constexpr WCHAR kUpdateTaskbarStatePendingTickCount[] =
    L"Windhawk_UpdateTaskbarStatePendingTickCount_" WH_MOD_ID;

enum {
    kTrayUITimerHide = 2,
    kTrayUITimerUnhide = 3,
};

static const UINT g_getTaskbarRectRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_GetTaskbarRect_" WH_MOD_ID);

static const UINT g_updateTaskbarStateRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_UpdateTaskbarState_" WH_MOD_ID);

static const UINT g_refreshIconInsetRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_RefreshIconInset_" WH_MOD_ID);

void RefreshTaskbarIconInsetOnCurrentThread();

// Tray system icon tweaks (from the "Taskbar tray system icon tweaks" mod).
enum class HideBellIcon {
    never,
    whenInactive,
    whenInactiveAndNoDnd,
    always,
};

std::atomic<bool> g_settingHideVolumeIcon{false};
std::atomic<bool> g_settingHideNetworkIcon{false};
std::atomic<bool> g_settingHideBatteryIcon{false};
std::atomic<bool> g_settingGrayscaleBatteryIcon{false};
std::atomic<bool> g_settingHideMicrophoneIcon{false};
std::atomic<bool> g_settingHideGeolocationIcon{false};
std::atomic<bool> g_settingHideStudioEffectsIcon{false};
std::atomic<bool> g_settingHideRecallIcon{false};
std::atomic<bool> g_settingHideLanguageBar{false};
std::atomic<bool> g_settingHideLanguageSupplementaryIcons{false};
std::atomic<int> g_settingHideBellIcon{(int)HideBellIcon::never};
std::atomic<int> g_settingShowDesktopButtonWidth{12};

// Dock hover animation (from the "Taskbar Dock Animation Plus" mod).
struct DockSettings {
    bool enabled = true;
    int animationType = 0;
    double maxScale = 1.4;
    int effectRadius = 100;
    double spacingFactor = 0.5;
    int bounceDelay = 500;
    double focusDuration = 150.0;
    bool disableVerticalBounce = false;
    bool taskbarLabelsMode = false;
    int excludeSystemButtonsMode = 0;
    double lerpSpeed = 60.0;
    bool disableBounce = false;
};

std::mutex g_dockSettingsMutex;
DockSettings g_dockSettings;

DockSettings GetDockSettings() {
    std::lock_guard<std::mutex> lock(g_dockSettingsMutex);
    return g_dockSettings;
}

// Taskbar clock customization (from the "Taskbar Clock Customization" mod).
enum class ClockTooltipLineMode {
    append,
    replace,
};

enum class ClockPercentageFormat {
    spacePaddingAndSymbol,
    spacePadding,
    singleSpacePadding,
    zeroPadding,
    noPadding,
};

enum class ClockTransferFormat {
    mbs,
    mbsNumberOnly,
    mbsDynamic,
    mbits,
    mbitsNumberOnly,
    mbitsDynamic,
};

struct ClockWebContentSettings {
    std::wstring url;
    std::wstring blockStart;
    std::wstring start;
    std::wstring end;
    int maxLength = 28;
};

struct ClockTextStyleSettings {
    std::wstring textColor;
    std::wstring fontFamily;
    std::wstring fontWeight;
    std::wstring fontStyle;
    std::wstring fontStretch;
    std::wstring textAlignment;
    int fontSize = 0;
    int characterSpacing = 0;
    int lineHeight = 0;
};

struct ClockSettings {
    bool enabled = true;
    bool showSeconds = false;
    std::wstring timeFormat = L"HH':'mm':'ss";
    std::wstring dateFormat = L"yyyy年MM月dd日";
    std::wstring dateLocale;
    std::wstring weekdayFormat = L"dddd";
    std::wstring weekdayFormatCustom =
        L"周日, 周一, 周二, 周三, 周四, 周五, 周六";
    std::wstring topLine = L"ram:%ram%  ACPI:%cpu_temp%";
    std::wstring bottomLine = L"cpu:%cpu%   gpu:%gpu%";
    std::wstring tooltipLine = L"%date% %weekday% %time%";
    ClockTooltipLineMode tooltipLineMode = ClockTooltipLineMode::replace;
    ClockPercentageFormat percentageFormat =
        ClockPercentageFormat::spacePaddingAndSymbol;
    int updateInterval = 1;
    ClockTransferFormat networkMetricsFormat =
        ClockTransferFormat::mbsDynamic;
    int networkMetricsFixedDecimals = -1;
    bool diskMetricsSameAsNetwork = true;
    ClockTransferFormat diskMetricsFormat = ClockTransferFormat::mbsDynamic;
    int diskMetricsFixedDecimals = -1;
    std::wstring networkAdapterName;
    std::wstring gpuAdapterName;
    std::wstring mediaInfoFormat = L"%media_artist% - %media_title%";
    std::wstring noMediaText = L"无媒体播放";
    int mediaMaxLength = 28;
    bool mediaRemoveBrackets = false;
    std::vector<std::wstring> mediaIgnoredPlayers;
    std::wstring weatherLocation;
    std::wstring weatherFormat = L"%c %t %w";
    std::wstring weatherUnits = L"autoDetect";
    std::vector<ClockWebContentSettings> webContentsItems;
    int webContentsUpdateInterval = 10;
    ClockTextStyleSettings timeStyle;
    ClockTextStyleSettings dateStyle;
};

std::mutex g_clockSettingsMutex;
ClockSettings g_clockSettings;

ClockSettings GetClockSettings() {
    std::lock_guard<std::mutex> lock(g_clockSettingsMutex);
    return g_clockSettings;
}

std::atomic<unsigned> g_settingsVersion{1};
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_systemTrayDllLoaded{false};
std::atomic<bool> g_hooksApplied{false};
// Counts the hook functions that are currently running, so that the mod can
// wait for them to finish before its code gets unloaded.
std::atomic<int> g_hookCallCounter{0};
std::atomic<bool> g_loggedMissingRootGrid{false};
std::atomic<bool> g_loggedMissingRects{false};
std::atomic<bool> g_loggedMissingTray{false};
std::atomic<bool> g_loggedMissingIsland{false};
std::atomic<bool> g_loggedRegionBusy{false};
std::atomic<bool> g_loggedMissingXamlRoot{false};
std::atomic<bool> g_loggedNoDirectApply{false};
std::atomic<bool> g_loggedStripUnknown{false};

class HookScope {
   public:
    HookScope() {
        g_hookCallCounter.fetch_add(1);
    }

    ~HookScope() {
        g_hookCallCounter.fetch_sub(1);
    }
};

void* g_ctaskBandTaskListWndSiteVftable = nullptr;
void* g_csecondaryTaskBandTaskListWndSiteVftable = nullptr;
void* g_taskbarHostFrameHeight = nullptr;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
using CSecondaryTaskBand_GetTaskbarHost_t =
    void*(WINAPI*)(void* pThis, void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original =
    nullptr;
using RefCountDecref_t = void(WINAPI*)(void* pThis);
RefCountDecref_t RefCountDecref_Original = nullptr;

struct SavedBackgroundFill {
    winrt::weak_ref<FrameworkElement> element;
    Brush originalFill{nullptr};
    winrt::weak_ref<Panel> parentPanel;
    Brush originalParentBackground{nullptr};
};

struct SavedBackgroundRadius {
    winrt::weak_ref<FrameworkElement> element;
    double radiusX = 0;
    double radiusY = 0;
};

struct SavedTrayHoverBorder {
    winrt::weak_ref<FrameworkElement> element;
    double height = std::numeric_limits<double>::quiet_NaN();
    double maxHeight = std::numeric_limits<double>::infinity();
    VerticalAlignment verticalAlignment{VerticalAlignment::Stretch};
    Thickness margin{};
    int64_t heightToken = 0;
    int64_t maxHeightToken = 0;
    int64_t verticalAlignmentToken = 0;
    int64_t marginToken = 0;
};

struct SavedTrayControlShape {
    winrt::weak_ref<FrameworkElement> element;
    CornerRadius cornerRadius{};
    bool useSystemFocusVisuals = true;
    int64_t cornerRadiusToken = 0;
    int64_t focusVisualToken = 0;
};

enum class CornerShapeKind {
    Border,
    Grid,
    ContentPresenter,
};

struct SavedTrayCornerShape {
    winrt::weak_ref<FrameworkElement> element;
    CornerShapeKind kind = CornerShapeKind::Border;
    CornerRadius cornerRadius{};
    int64_t token = 0;
};

struct SavedTrayRectShape {
    winrt::weak_ref<FrameworkElement> element;
    double radiusX = 0.0;
    double radiusY = 0.0;
    int64_t radiusXToken = 0;
    int64_t radiusYToken = 0;
};

std::atomic<double> g_hoverRadiusDip{12.8};
std::atomic<double> g_trayHoverHeightDip{34.0};
std::atomic<double> g_trayRowHeightDip{75.2};
std::atomic<DWORD> g_lastTrayHoverReapplyTick{0};
void AppendTrayLayoutDiag(PCWSTR text);
void AppendTrayAlignmentDiag(PCWSTR text);
void AppendClockFeatureDiag(PCWSTR text);
void ApplyToAllTaskbarsNow();
void ClockApplyTextStyles(FrameworkElement const& trayElement);
void ClockClearTextStyles(FrameworkElement const& trayElement);

// Windows paints a separate HoverFlyoutBackgroundControl behind the tray
// anchor. It lives outside TaskbarFrame/SystemTrayFrame, so searching only
// those subtrees never found it. On the first hover after an Explorer restart
// it is laid out at the full icon-row height (about 48 DIP, plus padding),
// which is the large light square. The normal rounded tray capsule is a
// different BackgroundBorder inside the tray subtree, so suppressing this
// overlay leaves the capsule intact.
struct SavedHoverFlyoutVisual {
    winrt::weak_ref<FrameworkElement> element;
    double opacity = 1.0;
    bool isHitTestVisible = true;
    Visibility visibility = Visibility::Visible;
    bool compositionVisible = true;
    int64_t opacityToken = 0;
    int64_t visibilityToken = 0;
};

std::mutex g_hoverFlyoutMutex;
std::vector<SavedHoverFlyoutVisual> g_savedHoverFlyoutVisuals;
winrt::weak_ref<FrameworkElement> g_hoverFlyoutRoot;
winrt::event_token g_hoverFlyoutLayoutToken{};
bool g_hoverFlyoutHookInstalled = false;

// The tray hover surface is animated by the shell with a scale/translation
// that can outlive the visual-state transition. A local Height value cannot
// beat that transform, which is why the same 90.4 DIP-wide capsule can be
// seen at 32.8 DIP and at 62.4 DIP. Remember and neutralize the transform on
// the background and its immediate background wrappers.
struct SavedHoverTransform {
    winrt::weak_ref<FrameworkElement> element;
    winrt::Windows::Foundation::Numerics::float3 compositionScale{};
    winrt::Windows::Foundation::Numerics::float3 compositionCenterPoint{};
    bool scalePinned = false;
};

struct SavedHoverBackgroundTransform {
    winrt::weak_ref<FrameworkElement> element;
    Transform renderTransform{nullptr};
    winrt::Windows::Foundation::Point renderTransformOrigin{};
    winrt::Windows::Foundation::Numerics::float3 translation{};
    winrt::Windows::Foundation::Numerics::float3 compositionScale{};
    winrt::Windows::Foundation::Numerics::float3 compositionCenterPoint{};
    int64_t renderTransformToken = 0;
};

std::mutex g_hoverTransformMutex;
std::vector<SavedHoverTransform> g_savedHoverTransforms;
std::mutex g_hoverBackgroundTransformMutex;
std::vector<SavedHoverBackgroundTransform>
    g_savedHoverBackgroundTransforms;
std::atomic<bool> g_hoverStateDumpRequested{false};
std::atomic<DWORD> g_hoverPointerEnterTick{0};
winrt::weak_ref<FrameworkElement> g_hoverDumpTray;
std::atomic<bool> g_hoverWatchdogStop{false};
HANDLE g_hoverWatchdogThread = nullptr;

struct SavedMultiWindowHighlight {
    winrt::weak_ref<FrameworkElement> element;
    double opacity = 1.0;
    Visibility visibility = Visibility::Visible;
    bool compositionVisible = true;
    int64_t opacityToken = 0;
    int64_t visibilityToken = 0;
};

std::mutex g_multiWindowHighlightMutex;
std::vector<SavedMultiWindowHighlight> g_savedMultiWindowHighlights;

struct SavedTaskButtonClip {
    winrt::weak_ref<FrameworkElement> element;
    RectangleGeometry originalClip{nullptr};
};

std::mutex g_taskButtonClipMutex;
std::vector<SavedTaskButtonClip> g_savedTaskButtonClips;

struct SavedTaskButtonStackStyle {
    winrt::weak_ref<FrameworkElement> element;
    double width = std::numeric_limits<double>::quiet_NaN();
    double height = std::numeric_limits<double>::quiet_NaN();
    HorizontalAlignment horizontalAlignment{HorizontalAlignment::Stretch};
    VerticalAlignment verticalAlignment{VerticalAlignment::Stretch};
    winrt::Windows::Foundation::Numerics::float3 translation{};
    Transform renderTransform{nullptr};
    bool isHitTestVisible = true;
    CornerRadius cornerRadius{};
    Brush background{nullptr};
    bool isBorder = false;
    bool isPanel = false;
    bool isPresenter = false;
};

std::mutex g_taskButtonStackMutex;
std::vector<SavedTaskButtonStackStyle> g_savedTaskButtonStackStyles;

struct TaskButtonStackCardRecord {
    winrt::weak_ref<FrameworkElement> button;
    winrt::weak_ref<FrameworkElement> main;
    winrt::weak_ref<FrameworkElement> nativeStack;
    int64_t nativeVisibilityToken = 0;
    int64_t nativeOpacityToken = 0;
    double mainWidth = std::numeric_limits<double>::quiet_NaN();
    double mainHeight = std::numeric_limits<double>::quiet_NaN();
    HorizontalAlignment mainHorizontalAlignment{
        HorizontalAlignment::Stretch};
    VerticalAlignment mainVerticalAlignment{VerticalAlignment::Stretch};
    CornerRadius mainCornerRadius{};
    bool mainStyleSaved = false;
    Visibility nativeVisibility = Visibility::Visible;
    double nativeOpacity = 1.0;
    bool nativeCompositionVisible = true;
    bool nativeStyleSaved = false;
    int lastLoggedCount = -1;
};

std::mutex g_taskButtonStackCardMutex;
std::vector<TaskButtonStackCardRecord> g_taskButtonStackCards;

std::mutex g_taskButtonOverflowMutex;
std::vector<winrt::weak_ref<FrameworkElement>> g_hiddenTaskButtonOverflowLayers;

DWORD WINAPI TrayHoverReapplyProc(LPVOID) {
    // Let the system finish its visual-state transition before re-centering
    // the OmniButton and its hover background. Applying from inside the
    // property callback would re-enter the XAML layout pass.
    Sleep(30);
    if (!g_unloading) {
        ApplyToAllTaskbarsNow();
    }
    return 0;
}

void ScheduleTrayHoverReapply() {
    if (g_unloading) {
        return;
    }

    DWORD now = GetTickCount();
    DWORD last = g_lastTrayHoverReapplyTick.load();
    if (now - last < 100 ||
        !g_lastTrayHoverReapplyTick.compare_exchange_strong(last, now)) {
        return;
    }

    HANDLE thread = CreateThread(nullptr, 0, TrayHoverReapplyProc, nullptr, 0,
                                 nullptr);
    if (thread) {
        CloseHandle(thread);
    }
}

struct SavedHoverBorderShape {
    winrt::weak_ref<FrameworkElement> element;
    CornerRadius cornerRadius{};
    int64_t cornerRadiusToken = 0;
    int64_t heightToken = 0;
    int64_t visibilityToken = 0;
};

struct SavedBackgroundLayer {
    winrt::weak_ref<FrameworkElement> element;
    Brush background{nullptr};
    bool isPanel = false;
    bool isBorder = false;
    bool isPresenter = false;
};

struct AppliedElement {
    DWORD threadId = 0;
    unsigned version = 0;
    winrt::weak_ref<FrameworkElement> frame;
    winrt::weak_ref<Grid> rootGrid;
    winrt::weak_ref<FrameworkElement> tray;
    winrt::event_token trayPointerEnteredToken{};
    bool trayPointerEnteredAttached = false;
    bool trayArranged = false;
    // Explorer can expose the tray element before its XAML subtree has a
    // size. The hover backgrounds and OmniButtons only exist after that
    // layout pass, so remember whether the last apply saw real tray content.
    bool trayContentReady = false;
    double savedFrameMinWidth = 0;
    double savedFrameMaxWidth = std::numeric_limits<double>::infinity();
    double savedFrameMaxHeight = std::numeric_limits<double>::infinity();
    HorizontalAlignment savedFrameAlignment{HorizontalAlignment::Stretch};
    double savedFrameHeight = std::numeric_limits<double>::quiet_NaN();
    VerticalAlignment savedFrameVerticalAlignment{VerticalAlignment::Stretch};
    Thickness savedFrameMargin{};
    bool frameHeightSet = false;
    bool frameMarginSet = false;
    // The system sets the alignment of this repeater to Center or Left
    // according to the taskbar alignment setting. The mod forces it to Left so
    // that the icons are always aligned to the left end of the floating bar.
    winrt::weak_ref<FrameworkElement> iconRepeater;
    HorizontalAlignment savedRepeaterAlignment{HorizontalAlignment::Center};
    Thickness savedRepeaterMargin{};
    double savedRepeaterHeight = std::numeric_limits<double>::quiet_NaN();
    VerticalAlignment savedRepeaterVerticalAlignment{
        VerticalAlignment::Stretch};
    bool repeaterAlignmentSet = false;
    // The taskbar's visible background is the Fill of BackgroundFill inside
    // Taskbar.TaskbarBackground, not the outer RootGrid background.
    std::vector<SavedBackgroundFill> savedBackgroundFills;
    std::vector<SavedBackgroundRadius> savedBackgroundRadii;
    Brush appliedBackgroundFill{nullptr};
    Brush appliedRootBackground{nullptr};
    bool backgroundFillApplied = false;
    std::vector<SavedTrayHoverBorder> savedTrayHoverBorders;
    std::vector<SavedTrayHoverBorder> savedTrayOmniButtons;
    std::vector<SavedTrayHoverBorder> savedTrayStackLayouts;
    std::vector<SavedTrayControlShape> savedTrayControlShapes;
    std::vector<SavedTrayCornerShape> savedTrayCornerShapes;
    std::vector<SavedTrayRectShape> savedTrayRectShapes;
    std::vector<SavedHoverBorderShape> savedHoverBorderShapes;
    std::vector<SavedBackgroundLayer> savedBackgroundLayers;
    // The parent of the taskbar frame gets an explicit height while a custom
    // taskbar area height is in use, so remember the original value for a
    // later restore.
    winrt::weak_ref<FrameworkElement> frameParent;
    double savedParentHeight = std::numeric_limits<double>::quiet_NaN();
    bool parentHeightSet = false;
    Thickness savedTrayMargin{};
    double savedTrayHeight = std::numeric_limits<double>::quiet_NaN();
    VerticalAlignment savedTrayVerticalAlignment{VerticalAlignment::Stretch};
    // The tray frame can be taller than its inner SystemTrayFrameGrid. The
    // inner grid must be stretched too, otherwise the OmniButton offset that
    // centers the clock inside a tall bar is clipped by the original 48 DIP
    // row.
    winrt::weak_ref<FrameworkElement> trayFrameGrid;
    double savedTrayFrameGridHeight = std::numeric_limits<double>::quiet_NaN();
    VerticalAlignment savedTrayFrameGridVerticalAlignment{
        VerticalAlignment::Stretch};
    bool trayFrameGridHeightSet = false;
    DWORD lastApplyTick = 0;
    HWND islandHwnd = nullptr;
    HWND topLevelWnd = nullptr;
    RECT appliedRegion{};
    bool hasAppliedRegion = false;
    int regionRetries = 0;
};

std::mutex g_appliedMutex;
std::vector<AppliedElement> g_applied;

void SaveBackgroundFillState(AppliedElement& record,
                             FrameworkElement const& element) {
    for (auto const& state : record.savedBackgroundFills) {
        if (state.element.get() == element) {
            return;
        }
    }

    auto rect = element.try_as<Shapes::Rectangle>();
    if (!rect) {
        return;
    }

    SavedBackgroundFill state;
    state.element = winrt::make_weak(element);
    state.originalFill = rect.Fill();
    if (auto parent = VisualTreeHelper::GetParent(element).try_as<Panel>()) {
        state.parentPanel = winrt::make_weak(parent);
        state.originalParentBackground = parent.Background();
    }
    record.savedBackgroundFills.push_back(std::move(state));
}

void RestoreBackgroundFills(
    std::vector<SavedBackgroundFill> const& savedStates) {
    for (auto const& state : savedStates) {
        if (auto element = state.element.get()) {
            try {
                if (auto rect = element.try_as<Shapes::Rectangle>()) {
                    rect.Fill(state.originalFill);
                }
            } catch (...) {
            }
        }

        if (auto parent = state.parentPanel.get()) {
            try {
                parent.Background(state.originalParentBackground);
            } catch (...) {
            }
        }
    }
}

void SaveBackgroundRadiusState(AppliedElement& record,
                               FrameworkElement const& element,
                               Shapes::Rectangle const& rect) {
    for (auto const& state : record.savedBackgroundRadii) {
        if (state.element.get() == element) {
            return;
        }
    }

    SavedBackgroundRadius state;
    state.element = winrt::make_weak(element);
    state.radiusX = rect.RadiusX();
    state.radiusY = rect.RadiusY();
    record.savedBackgroundRadii.push_back(std::move(state));
}

void RestoreBackgroundRadii(
    std::vector<SavedBackgroundRadius> const& savedStates) {
    for (auto const& state : savedStates) {
        if (auto element = state.element.get()) {
            try {
                if (auto rect = element.try_as<Shapes::Rectangle>()) {
                    rect.RadiusX(state.radiusX);
                    rect.RadiusY(state.radiusY);
                }
            } catch (...) {
            }
        }
    }
}

bool ClassNameEndsWith(winrt::hstring const& className,
                       std::wstring_view suffix);
bool ElementNameEquals(FrameworkElement const& element,
                       std::wstring_view expected);

void CollectTrayHoverBorders(FrameworkElement const& root,
                             std::vector<FrameworkElement>& result) {
    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        try {
            auto className = winrt::get_class_name(child);
            if (ClassNameEndsWith(className, L"Border") &&
                (ElementNameEquals(child, L"BackgroundBorder") ||
                 ElementNameEquals(child, L"BackgroundElement"))) {
                result.push_back(child);
            }
        } catch (...) {
        }

        CollectTrayHoverBorders(child, result);
    }
}

void ApplyTrayHoverBorderLayout(FrameworkElement const& element,
                                double height);

void SaveTrayHoverBorderState(AppliedElement& record,
                              FrameworkElement const& element) {
    for (auto const& state : record.savedTrayHoverBorders) {
        if (state.element.get() == element) {
            return;
        }
    }

    auto border = element.try_as<Border>();
    if (!border) {
        return;
    }

    SavedTrayHoverBorder state;
    state.element = winrt::make_weak(element);
    state.height = border.Height();
    state.maxHeight = border.MaxHeight();
    state.verticalAlignment = border.VerticalAlignment();
    state.margin = border.Margin();

    auto enforceLayout = [](DependencyObject const& sender,
                            DependencyProperty const&) {
        if (g_unloading) {
            return;
        }
        auto border = sender.try_as<Border>();
        if (!border || border.Visibility() != Visibility::Visible) {
            return;
        }
        double targetHeight = g_trayHoverHeightDip.load();
        if (targetHeight > 0) {
            ApplyTrayHoverBorderLayout(border, targetHeight);
        }
    };
    state.heightToken = border.RegisterPropertyChangedCallback(
        FrameworkElement::HeightProperty(), enforceLayout);
    state.maxHeightToken = border.RegisterPropertyChangedCallback(
        FrameworkElement::MaxHeightProperty(), enforceLayout);
    state.verticalAlignmentToken = border.RegisterPropertyChangedCallback(
        FrameworkElement::VerticalAlignmentProperty(), enforceLayout);
    state.marginToken = border.RegisterPropertyChangedCallback(
        FrameworkElement::MarginProperty(), enforceLayout);
    record.savedTrayHoverBorders.push_back(std::move(state));
}

void RestoreTrayHoverBorders(
    std::vector<SavedTrayHoverBorder> const& savedStates) {
    for (auto const& state : savedStates) {
        if (auto element = state.element.get()) {
            try {
                if (auto border = element.try_as<Border>()) {
                    if (state.heightToken) {
                        border.UnregisterPropertyChangedCallback(
                            FrameworkElement::HeightProperty(),
                            state.heightToken);
                    }
                    if (state.maxHeightToken) {
                        border.UnregisterPropertyChangedCallback(
                            FrameworkElement::MaxHeightProperty(),
                            state.maxHeightToken);
                    }
                    if (state.verticalAlignmentToken) {
                        border.UnregisterPropertyChangedCallback(
                            FrameworkElement::VerticalAlignmentProperty(),
                            state.verticalAlignmentToken);
                    }
                    if (state.marginToken) {
                        border.UnregisterPropertyChangedCallback(
                            FrameworkElement::MarginProperty(),
                            state.marginToken);
                    }
                    border.Height(state.height);
                    border.MaxHeight(state.maxHeight);
                    border.VerticalAlignment(state.verticalAlignment);
                    border.Margin(state.margin);
                }
            } catch (...) {
            }
        }
    }
}

void ApplyHoverTransformResetChain(FrameworkElement const& border);
void ResetHoverBackgroundTransform(FrameworkElement const& element);

void ApplyTrayHoverBorderLayout(FrameworkElement const& element,
                                double height) {
    try {
        if (auto border = element.try_as<Border>()) {
            double currentHeight = border.Height();
            double currentMaxHeight = border.MaxHeight();
            auto currentAlignment = border.VerticalAlignment();
            Thickness currentMargin = border.Margin();
            bool corrected = false;
            if (std::isnan(currentHeight) ||
                std::abs(currentHeight - height) > 0.5) {
                border.Height(height);
                corrected = true;
            }
            if (std::isnan(border.MaxHeight()) ||
                std::abs(border.MaxHeight() - height) > 0.5) {
                border.MaxHeight(height);
                corrected = true;
            }
            if (border.VerticalAlignment() != VerticalAlignment::Center) {
                border.VerticalAlignment(VerticalAlignment::Center);
                corrected = true;
            }
            Thickness margin = border.Margin();
            if (std::abs(margin.Top) > 0.01 ||
                std::abs(margin.Bottom) > 0.01) {
                margin.Top = 0;
                margin.Bottom = 0;
                border.Margin(margin);
                corrected = true;
            }

            if (corrected) {
                WCHAR diag[640];
                auto className = winrt::get_class_name(border);
                auto name = border.Name();
                swprintf_s(
                    diag,
                    L"correct tray hover %s/%s before h=%.1f max=%.1f "
                    L"top=%.1f bottom=%.1f va=%d after h=%.1f max=%.1f "
                    L"top=%.1f bottom=%.1f va=%d",
                    className.c_str(), name.c_str(), currentHeight,
                    currentMaxHeight, currentMargin.Top,
                    currentMargin.Bottom,
                    static_cast<int>(currentAlignment), border.Height(),
                    border.MaxHeight(), border.Margin().Top,
                    border.Margin().Bottom,
                    static_cast<int>(border.VerticalAlignment()));
                AppendTrayAlignmentDiag(diag);
            }
        }
    } catch (...) {
    }

    ApplyHoverTransformResetChain(element);
}

void CollectTrayOmniButtons(FrameworkElement const& root,
                            std::vector<FrameworkElement>& result) {
    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        try {
            if (ClassNameEndsWith(winrt::get_class_name(child),
                                  L"OmniButton")) {
                result.push_back(child);
            }
        } catch (...) {
        }

        CollectTrayOmniButtons(child, result);
    }
}

double GetFixedTrayOmniButtonHeight(FrameworkElement const& element) {
    if (ElementNameEquals(element, L"ControlCenterButton")) {
        return g_trayHoverHeightDip.load() + 0.4;
    }
    if (ElementNameEquals(element, L"NotificationCenterButton")) {
        return g_trayHoverHeightDip.load() + 2.0;
    }
    return 0.0;
}

double GetTrayOmniButtonRowHeight(FrameworkElement const& element) {
    try {
        auto parent =
            VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
        if (parent && parent.ActualHeight() > 0) {
            return parent.ActualHeight();
        }
    } catch (...) {
    }
    return g_trayRowHeightDip.load();
}

void EnsureTrayElementVerticalLayout(FrameworkElement const& element) {
    if (!element) {
        return;
    }

    try {
        double beforeHeight = element.Height();
        double beforeTop = element.Margin().Top;
        int beforeAlignment =
            static_cast<int>(element.VerticalAlignment());
        double rowHeight = GetTrayOmniButtonRowHeight(element);
        bool corrected = false;
        double fixedHeight = GetFixedTrayOmniButtonHeight(element);
        if (fixedHeight > 0) {
            if (std::isnan(element.Height()) ||
                std::abs(element.Height() - fixedHeight) > 0.1) {
                element.Height(fixedHeight);
                corrected = true;
            }
            if (std::isnan(element.MaxHeight()) ||
                std::abs(element.MaxHeight() - fixedHeight) > 0.1) {
                element.MaxHeight(fixedHeight);
                corrected = true;
            }

            if (rowHeight > 0) {
                double offset =
                    std::max(0.0, (rowHeight - fixedHeight) / 2.0);
                if (element.VerticalAlignment() !=
                    VerticalAlignment::Top) {
                    element.VerticalAlignment(VerticalAlignment::Top);
                    corrected = true;
                }
                Thickness margin = element.Margin();
                if (std::abs(margin.Top - offset) > 0.05 ||
                    std::abs(margin.Bottom) > 0.05) {
                    margin.Top = offset;
                    margin.Bottom = 0;
                    element.Margin(margin);
                    corrected = true;
                }
            }
        } else {
            if (element.VerticalAlignment() !=
                VerticalAlignment::Center) {
                element.VerticalAlignment(VerticalAlignment::Center);
                corrected = true;
            }
            Thickness margin = element.Margin();
            if (std::abs(margin.Top) > 0.05 ||
                std::abs(margin.Bottom) > 0.05) {
                margin.Top = 0;
                margin.Bottom = 0;
                element.Margin(margin);
                corrected = true;
            }
        }

        if (corrected) {
            WCHAR diag[640];
            auto className = winrt::get_class_name(element);
            auto name = element.Name();
            swprintf_s(
                diag,
                L"correct tray element %s/%s row=%.1f fixed=%.1f "
                L"before h=%.1f top=%.1f va=%d after h=%.1f top=%.1f va=%d",
                className.c_str(), name.c_str(), rowHeight, fixedHeight,
                beforeHeight, beforeTop, beforeAlignment, element.Height(),
                element.Margin().Top,
                static_cast<int>(element.VerticalAlignment()));
            AppendTrayAlignmentDiag(diag);
            }
    } catch (...) {
    }
}

void SaveTrayElementLayout(std::vector<SavedTrayHoverBorder>& savedStates,
                           FrameworkElement const& element) {
    for (auto const& state : savedStates) {
        if (state.element.get() == element) {
            return;
        }
    }

    SavedTrayHoverBorder state;
    state.element = winrt::make_weak(element);
    state.height = element.Height();
    state.maxHeight = element.MaxHeight();
    state.verticalAlignment = element.VerticalAlignment();
    state.margin = element.Margin();

    auto enforceLayout = [](DependencyObject const& sender,
                            DependencyProperty const&) {
        if (g_unloading) {
            return;
        }
        if (auto element = sender.try_as<FrameworkElement>()) {
            EnsureTrayElementVerticalLayout(element);
        }
    };
    state.heightToken = element.RegisterPropertyChangedCallback(
        FrameworkElement::HeightProperty(), enforceLayout);
    state.maxHeightToken = element.RegisterPropertyChangedCallback(
        FrameworkElement::MaxHeightProperty(), enforceLayout);
    state.verticalAlignmentToken = element.RegisterPropertyChangedCallback(
        FrameworkElement::VerticalAlignmentProperty(), enforceLayout);
    state.marginToken = element.RegisterPropertyChangedCallback(
        FrameworkElement::MarginProperty(), enforceLayout);
    EnsureTrayElementVerticalLayout(element);

    savedStates.push_back(std::move(state));
}

void RestoreTrayElementLayouts(
    std::vector<SavedTrayHoverBorder> const& savedStates) {
    for (auto const& state : savedStates) {
        if (auto element = state.element.get()) {
            try {
                if (state.heightToken) {
                    element.UnregisterPropertyChangedCallback(
                        FrameworkElement::HeightProperty(),
                        state.heightToken);
                }
                if (state.maxHeightToken) {
                    element.UnregisterPropertyChangedCallback(
                        FrameworkElement::MaxHeightProperty(),
                        state.maxHeightToken);
                }
                if (state.verticalAlignmentToken) {
                    element.UnregisterPropertyChangedCallback(
                        FrameworkElement::VerticalAlignmentProperty(),
                        state.verticalAlignmentToken);
                }
                if (state.marginToken) {
                    element.UnregisterPropertyChangedCallback(
                        FrameworkElement::MarginProperty(),
                        state.marginToken);
                }
                element.Height(state.height);
                element.MaxHeight(state.maxHeight);
                element.VerticalAlignment(state.verticalAlignment);
                element.Margin(state.margin);
            } catch (...) {
            }
        }
    }
}

void CollectTrayControls(FrameworkElement const& root,
                         std::vector<FrameworkElement>& result) {
    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        if (child.try_as<Controls::Control>()) {
            result.push_back(child);
        }

        CollectTrayControls(child, result);
    }
}

void SaveTrayControlShape(AppliedElement& record,
                          FrameworkElement const& element) {
    for (auto const& state : record.savedTrayControlShapes) {
        if (state.element.get() == element) {
            return;
        }
    }

    auto control = element.try_as<Controls::Control>();
    if (!control) {
        return;
    }

    SavedTrayControlShape state;
    state.element = winrt::make_weak(element);
    state.cornerRadius = control.CornerRadius();
    state.useSystemFocusVisuals = control.UseSystemFocusVisuals();

    state.cornerRadiusToken = control.RegisterPropertyChangedCallback(
        Controls::Control::CornerRadiusProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            auto control =
                sender.try_as<Controls::Control>();
            if (!control) {
                return;
            }

            double radius = g_hoverRadiusDip.load();
            CornerRadius current = control.CornerRadius();
            if (radius > 0 &&
                std::abs(current.TopLeft - radius) > 0.05) {
                control.CornerRadius(
                    CornerRadius{radius, radius, radius, radius});
            }
        });

    state.focusVisualToken = control.RegisterPropertyChangedCallback(
        Controls::Control::UseSystemFocusVisualsProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            auto control =
                sender.try_as<Controls::Control>();
            if (control && control.UseSystemFocusVisuals()) {
                control.UseSystemFocusVisuals(false);
            }
        });

    record.savedTrayControlShapes.push_back(std::move(state));
}

void ApplyTrayControlShape(FrameworkElement const& element, double radius) {
    try {
        if (auto control = element.try_as<Controls::Control>()) {
            control.CornerRadius(
                CornerRadius{radius, radius, radius, radius});
            control.UseSystemFocusVisuals(false);
        }
    } catch (...) {
    }
}

void RestoreTrayControlShapes(
    std::vector<SavedTrayControlShape> const& savedStates) {
    for (auto const& state : savedStates) {
        if (auto element = state.element.get()) {
            try {
                if (auto control = element.try_as<Controls::Control>()) {
                    if (state.cornerRadiusToken) {
                        control.UnregisterPropertyChangedCallback(
                            Controls::Control::CornerRadiusProperty(),
                            state.cornerRadiusToken);
                    }
                    if (state.focusVisualToken) {
                        control.UnregisterPropertyChangedCallback(
                            Controls::Control::UseSystemFocusVisualsProperty(),
                            state.focusVisualToken);
                    }
                    control.CornerRadius(state.cornerRadius);
                    control.UseSystemFocusVisuals(
                        state.useSystemFocusVisuals);
                }
            } catch (...) {
            }
        }
    }
}

CornerRadius GetTrayCornerShapeRadius(FrameworkElement const& element,
                                      CornerShapeKind kind) {
    try {
        switch (kind) {
        case CornerShapeKind::Border:
            if (auto border = element.try_as<Border>()) {
                return border.CornerRadius();
            }
            break;
        case CornerShapeKind::Grid:
            if (auto grid = element.try_as<Grid>()) {
                return grid.CornerRadius();
            }
            break;
        case CornerShapeKind::ContentPresenter:
            if (auto presenter =
                    element.try_as<ContentPresenter>()) {
                return presenter.CornerRadius();
            }
            break;
        }
    } catch (...) {
    }
    return CornerRadius{};
}

void SetTrayCornerShapeRadius(FrameworkElement const& element,
                              CornerShapeKind kind,
                              CornerRadius radius) {
    try {
        switch (kind) {
        case CornerShapeKind::Border:
            if (auto border = element.try_as<Border>()) {
                border.CornerRadius(radius);
            }
            break;
        case CornerShapeKind::Grid:
            if (auto grid = element.try_as<Grid>()) {
                grid.CornerRadius(radius);
            }
            break;
        case CornerShapeKind::ContentPresenter:
            if (auto presenter =
                    element.try_as<ContentPresenter>()) {
                presenter.CornerRadius(radius);
            }
            break;
        }
    } catch (...) {
    }
}

DependencyProperty GetTrayCornerShapeProperty(CornerShapeKind kind) {
    switch (kind) {
    case CornerShapeKind::Border:
        return Border::CornerRadiusProperty();
    case CornerShapeKind::Grid:
        return Grid::CornerRadiusProperty();
    case CornerShapeKind::ContentPresenter:
        return ContentPresenter::CornerRadiusProperty();
    }
    return nullptr;
}

void SaveAndApplyTrayCornerShape(AppliedElement& record,
                                 FrameworkElement const& element,
                                 CornerShapeKind kind,
                                 double radius) {
    for (auto const& state : record.savedTrayCornerShapes) {
        if (state.element.get() == element) {
            return;
        }
    }

    SavedTrayCornerShape state;
    state.element = winrt::make_weak(element);
    state.kind = kind;
    state.cornerRadius = GetTrayCornerShapeRadius(element, kind);

    SetTrayCornerShapeRadius(
        element, kind,
        CornerRadius{radius, radius, radius, radius});

    DependencyProperty property = GetTrayCornerShapeProperty(kind);
    if (property) {
        state.token = element.RegisterPropertyChangedCallback(
            property,
            [kind](DependencyObject const& sender,
                   DependencyProperty const&) {
                auto element = sender.try_as<FrameworkElement>();
                if (!element) {
                    return;
                }

                double target = g_hoverRadiusDip.load();
                if (target <= 0) {
                    return;
                }

                CornerRadius current =
                    GetTrayCornerShapeRadius(element, kind);
                if (std::abs(current.TopLeft - target) > 0.05) {
                    SetTrayCornerShapeRadius(
                        element, kind,
                        CornerRadius{target, target, target, target});
                }
            });
    }

    record.savedTrayCornerShapes.push_back(std::move(state));
}

void ApplyTrayCornerShapesRecursive(AppliedElement& record,
                                    FrameworkElement const& root,
                                    double radius,
                                    int depth = 0) {
    if (depth > 10) {
        return;
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        CornerShapeKind kind = CornerShapeKind::Border;
        bool apply = false;
        if (child.try_as<Border>()) {
            kind = CornerShapeKind::Border;
            apply = true;
        } else if (auto grid = child.try_as<Grid>()) {
            kind = CornerShapeKind::Grid;
            try {
                apply = grid.Background() != nullptr;
            } catch (...) {
            }
        } else if (auto presenter = child.try_as<ContentPresenter>()) {
            kind = CornerShapeKind::ContentPresenter;
            try {
                apply = presenter.Background() != nullptr;
            } catch (...) {
            }
        }

        if (apply && child.ActualWidth() >= 30.0 &&
            child.ActualHeight() >= 20.0) {
            SaveAndApplyTrayCornerShape(record, child, kind, radius);

            static std::atomic<int> s_cornerLogs{0};
            if (s_cornerLogs.fetch_add(1) < 30) {
                WCHAR diag[600];
                auto className = winrt::get_class_name(child);
                auto elementName = child.Name();
                swprintf_s(diag,
                           L"tray corner class=%s name=%s w=%.1f h=%.1f "
                           L"r=%.1f",
                           className.c_str(), elementName.c_str(),
                           child.ActualWidth(), child.ActualHeight(),
                           radius);
                AppendTrayLayoutDiag(diag);
            }
        }

        ApplyTrayCornerShapesRecursive(record, child, radius, depth + 1);
    }
}

void RestoreTrayCornerShapes(
    std::vector<SavedTrayCornerShape> const& savedStates) {
    for (auto const& state : savedStates) {
        if (auto element = state.element.get()) {
            try {
                DependencyProperty property =
                    GetTrayCornerShapeProperty(state.kind);
                if (property && state.token) {
                    element.UnregisterPropertyChangedCallback(property,
                                                              state.token);
                }
                SetTrayCornerShapeRadius(element, state.kind,
                                         state.cornerRadius);
            } catch (...) {
            }
        }
    }
}

void SaveAndApplyTrayRectShape(AppliedElement& record,
                               FrameworkElement const& element,
                               double radius) {
    for (auto const& state : record.savedTrayRectShapes) {
        if (state.element.get() == element) {
            return;
        }
    }

    auto rect = element.try_as<Shapes::Rectangle>();
    if (!rect) {
        return;
    }

    SavedTrayRectShape state;
    state.element = winrt::make_weak(element);
    state.radiusX = rect.RadiusX();
    state.radiusY = rect.RadiusY();
    rect.RadiusX(radius);
    rect.RadiusY(radius);

    state.radiusXToken = element.RegisterPropertyChangedCallback(
        Shapes::Rectangle::RadiusXProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            auto rect = sender.try_as<Shapes::Rectangle>();
            if (!rect) {
                return;
            }
            double target = g_hoverRadiusDip.load();
            if (target > 0 &&
                std::abs(rect.RadiusX() - target) > 0.05) {
                rect.RadiusX(target);
            }
        });
    state.radiusYToken = element.RegisterPropertyChangedCallback(
        Shapes::Rectangle::RadiusYProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            auto rect = sender.try_as<Shapes::Rectangle>();
            if (!rect) {
                return;
            }
            double target = g_hoverRadiusDip.load();
            if (target > 0 &&
                std::abs(rect.RadiusY() - target) > 0.05) {
                rect.RadiusY(target);
            }
        });

    record.savedTrayRectShapes.push_back(std::move(state));
}

void ApplyTrayRectShapesRecursive(AppliedElement& record,
                                  FrameworkElement const& root,
                                  double radius,
                                  int depth = 0) {
    if (depth > 10) {
        return;
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        if (auto rect = child.try_as<Shapes::Rectangle>()) {
            bool hasFill = false;
            try {
                hasFill = rect.Fill() != nullptr;
            } catch (...) {
            }
            if (hasFill && child.ActualWidth() >= 40.0 &&
                child.ActualHeight() >= 25.0) {
                SaveAndApplyTrayRectShape(record, child, radius);

                static std::atomic<int> s_rectLogs{0};
                if (s_rectLogs.fetch_add(1) < 20) {
                    WCHAR diag[600];
                    auto className = winrt::get_class_name(child);
                    auto elementName = child.Name();
                    swprintf_s(diag,
                               L"tray rect class=%s name=%s w=%.1f h=%.1f "
                               L"r=%.1f",
                               className.c_str(), elementName.c_str(),
                               child.ActualWidth(), child.ActualHeight(),
                               radius);
                    AppendTrayLayoutDiag(diag);
                }
            }
        }

        ApplyTrayRectShapesRecursive(record, child, radius, depth + 1);
    }
}

void RestoreTrayRectShapes(
    std::vector<SavedTrayRectShape> const& savedStates) {
    for (auto const& state : savedStates) {
        if (auto element = state.element.get()) {
            try {
                auto rect = element.try_as<Shapes::Rectangle>();
                if (!rect) {
                    continue;
                }
                if (state.radiusXToken) {
                    element.UnregisterPropertyChangedCallback(
                        Shapes::Rectangle::RadiusXProperty(),
                        state.radiusXToken);
                }
                if (state.radiusYToken) {
                    element.UnregisterPropertyChangedCallback(
                        Shapes::Rectangle::RadiusYProperty(),
                        state.radiusYToken);
                }
                rect.RadiusX(state.radiusX);
                rect.RadiusY(state.radiusY);
            } catch (...) {
            }
        }
    }
}

void ApplyTrayOmniButtonLayout(FrameworkElement const& element,
                               double rowHeight) {
    if (!(GetFixedTrayOmniButtonHeight(element) > 0)) {
        return;
    }
    if (rowHeight > 0 &&
        !(GetTrayOmniButtonRowHeight(element) > 0)) {
        g_trayRowHeightDip = rowHeight;
    }
    EnsureTrayElementVerticalLayout(element);
}

void SaveAndResetHoverTransform(FrameworkElement const& element) {
    std::lock_guard<std::mutex> lock(g_hoverTransformMutex);
    for (auto& saved : g_savedHoverTransforms) {
        if (saved.element.get() == element) {
            try {
                auto visual = ElementCompositionPreview::GetElementVisual(element);
                if (!saved.scalePinned) {
                    auto compositor = visual.Compositor();
                    auto scaleAnimation =
                        compositor.CreateExpressionAnimation(L"Vector3(1,1,1)");
                    visual.StartAnimation(L"Scale", scaleAnimation);
                    saved.scalePinned = true;
                }
            } catch (...) {
            }
            return;
        }
    }

    SavedHoverTransform saved;
    saved.element = winrt::make_weak(element);
    try {
        auto visual = ElementCompositionPreview::GetElementVisual(element);
        saved.compositionScale = visual.Scale();
        saved.compositionCenterPoint = visual.CenterPoint();
    } catch (...) {
        return;
    }

    try {
        auto visual = ElementCompositionPreview::GetElementVisual(element);
        auto compositor = visual.Compositor();
        auto scaleAnimation =
            compositor.CreateExpressionAnimation(L"Vector3(1,1,1)");
        visual.StartAnimation(L"Scale", scaleAnimation);
        saved.scalePinned = true;
    } catch (...) {
        return;
    }

    g_savedHoverTransforms.push_back(std::move(saved));
}

void ResetHoverBackgroundTransformElement(FrameworkElement const& element) {
    if (g_unloading || !element) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_hoverBackgroundTransformMutex);
    for (auto const& saved : g_savedHoverBackgroundTransforms) {
        if (saved.element.get() == element) {
            try {
                element.RenderTransform(nullptr);
                element.Translation(
                    winrt::Windows::Foundation::Numerics::float3{0, 0, 0});
                auto visual = ElementCompositionPreview::GetElementVisual(element);
                visual.StopAnimation(L"Offset");
                visual.StopAnimation(L"Size");
            } catch (...) {
            }
            return;
        }
    }

    SavedHoverBackgroundTransform saved;
    saved.element = winrt::make_weak(element);
    try {
        saved.renderTransform = element.RenderTransform();
        saved.renderTransformOrigin = element.RenderTransformOrigin();
        saved.translation = element.Translation();
        auto visual = ElementCompositionPreview::GetElementVisual(element);
        saved.compositionScale = visual.Scale();
        saved.compositionCenterPoint = visual.CenterPoint();
    } catch (...) {
        return;
    }

    saved.renderTransformToken = element.RegisterPropertyChangedCallback(
        UIElement::RenderTransformProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            if (g_unloading) {
                return;
            }
            auto element = sender.try_as<FrameworkElement>();
            if (element && element.RenderTransform()) {
                element.RenderTransform(nullptr);
            }
        });

    try {
        element.RenderTransform(nullptr);
        element.RenderTransformOrigin(
            winrt::Windows::Foundation::Point{0.5, 0.5});
        element.Translation(
            winrt::Windows::Foundation::Numerics::float3{0, 0, 0});
        auto visual = ElementCompositionPreview::GetElementVisual(element);
        visual.StopAnimation(L"Offset");
        visual.StopAnimation(L"Size");
        visual.Scale(
            winrt::Windows::Foundation::Numerics::float3{1, 1, 1});
        visual.CenterPoint(
            winrt::Windows::Foundation::Numerics::float3{0, 0, 0});
    } catch (...) {
        return;
    }

    g_savedHoverBackgroundTransforms.push_back(std::move(saved));
}

void ResetHoverBackgroundTransform(FrameworkElement const& element) {
    if (g_unloading || !element) {
        return;
    }

    ResetHoverBackgroundTransformElement(element);

    try {
        FrameworkElement parent =
            VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
        if (!parent) {
            return;
        }
        auto className = winrt::get_class_name(parent);
        bool wrapper = ClassNameEndsWith(className, L"Grid") ||
                       ClassNameEndsWith(className, L"ContentPresenter") ||
                       ClassNameEndsWith(className, L"Border");
        double borderWidth = element.ActualWidth();
        double parentWidth = parent.ActualWidth();
        if (wrapper && borderWidth > 0 && parentWidth >= borderWidth - 20 &&
            parentWidth <= borderWidth + 40) {
            ResetHoverBackgroundTransformElement(parent);
        }
    } catch (...) {
    }
}

void RestoreHoverBackgroundTransforms() {
    std::vector<SavedHoverBackgroundTransform> savedTransforms;
    {
        std::lock_guard<std::mutex> lock(g_hoverBackgroundTransformMutex);
        savedTransforms.swap(g_savedHoverBackgroundTransforms);
    }

    for (auto const& saved : savedTransforms) {
        if (auto element = saved.element.get()) {
            try {
                if (saved.renderTransformToken) {
                    element.UnregisterPropertyChangedCallback(
                        UIElement::RenderTransformProperty(),
                        saved.renderTransformToken);
                }
                element.RenderTransform(saved.renderTransform);
                element.RenderTransformOrigin(saved.renderTransformOrigin);
                element.Translation(saved.translation);
                auto visual = ElementCompositionPreview::GetElementVisual(element);
                visual.Scale(saved.compositionScale);
                visual.CenterPoint(saved.compositionCenterPoint);
            } catch (...) {
            }
        }
    }
}

void ApplyHoverTransformResetChain(FrameworkElement const& border) {
    if (g_unloading || !border) {
        return;
    }

    SaveAndResetHoverTransform(border);

    double borderWidth = border.ActualWidth();
    FrameworkElement parent =
        VisualTreeHelper::GetParent(border).try_as<FrameworkElement>();
    for (int depth = 0; depth < 2 && parent; depth++) {
        try {
            auto className = winrt::get_class_name(parent);
            bool iconContainer =
                ClassNameEndsWith(className, L"Stack") ||
                ClassNameEndsWith(className, L"StackListView") ||
                ClassNameEndsWith(className, L"OmniButton") ||
                ClassNameEndsWith(className, L"IconView");
            double parentWidth = parent.ActualWidth();
            if (!iconContainer && borderWidth > 0 && parentWidth > 0 &&
                parentWidth >= borderWidth - 20 &&
                parentWidth <= borderWidth + 40) {
                SaveAndResetHoverTransform(parent);
            }
        } catch (...) {
        }
        parent = VisualTreeHelper::GetParent(parent).try_as<FrameworkElement>();
    }
}

void NormalizeTrayHoverTransformsRecursive(FrameworkElement const& root,
                                           int depth = 0) {
    if (g_unloading || !root || depth > 14) {
        return;
    }

    try {
        auto className = winrt::get_class_name(root);
        bool isText = ClassNameEndsWith(className, L"TextBlock") ||
                      ClassNameEndsWith(className, L"TextIconContent") ||
                      ClassNameEndsWith(className, L"AdaptiveTextBlock");
        bool isIconContainer = ClassNameEndsWith(className, L"IconView") ||
                               ClassNameEndsWith(className, L"StackListView");
        double width = root.ActualWidth();
        double height = root.ActualHeight();
        if (!isText && !isIconContainer && width >= 60 && width <= 160 &&
            height >= 20 && height <= 90) {
            SaveAndResetHoverTransform(root);
        }
    } catch (...) {
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }
    for (int i = 0; i < count; i++) {
        try {
            auto child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
            if (child) {
                NormalizeTrayHoverTransformsRecursive(child, depth + 1);
            }
        } catch (...) {
        }
    }
}

void RestoreHoverTransforms() {
    std::vector<SavedHoverTransform> savedTransforms;
    {
        std::lock_guard<std::mutex> lock(g_hoverTransformMutex);
        savedTransforms.swap(g_savedHoverTransforms);
    }

    for (auto const& saved : savedTransforms) {
        if (auto element = saved.element.get()) {
            try {
                auto visual = ElementCompositionPreview::GetElementVisual(element);
                visual.StopAnimation(L"Scale");
                visual.Scale(saved.compositionScale);
                visual.CenterPoint(saved.compositionCenterPoint);
            } catch (...) {
            }
        }
    }
}

void CollectHoverBackgroundBorders(FrameworkElement const& root,
                                   std::vector<FrameworkElement>& result) {
    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        try {
            auto className = winrt::get_class_name(child);
            if (ClassNameEndsWith(className, L"Border") &&
                (ElementNameEquals(child, L"BackgroundElement") ||
                 ElementNameEquals(child, L"BackgroundBorder"))) {
                result.push_back(child);
            }
        } catch (...) {
        }

        CollectHoverBackgroundBorders(child, result);
    }
}

void ApplyHoverBorderShape(FrameworkElement const& element, double radius);

void SaveHoverBorderShape(AppliedElement& record,
                          FrameworkElement const& element,
                          bool trayContext) {
    for (auto const& state : record.savedHoverBorderShapes) {
        if (state.element.get() == element) {
            return;
        }
    }

    auto border = element.try_as<Border>();
    if (!border) {
        return;
    }

    SavedHoverBorderShape state;
    state.element = winrt::make_weak(element);
    state.cornerRadius = border.CornerRadius();

    bool isTrayBorder =
        trayContext || ElementNameEquals(element, L"BackgroundBorder");
    state.cornerRadiusToken = border.RegisterPropertyChangedCallback(
        Border::CornerRadiusProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            auto border = sender.try_as<Border>();
            if (!border) {
                return;
            }

            double radius = g_hoverRadiusDip.load();
            CornerRadius current = border.CornerRadius();
            if (radius > 0 &&
                std::abs(current.TopLeft - radius) > 0.05) {
                border.CornerRadius(
                    CornerRadius{radius, radius, radius, radius});
            }
        });

    if (isTrayBorder) {
        state.heightToken = border.RegisterPropertyChangedCallback(
            FrameworkElement::HeightProperty(),
            [](DependencyObject const& sender, DependencyProperty const&) {
                auto border = sender.try_as<Border>();
                if (!border) {
                    return;
                }

                double height = g_trayHoverHeightDip.load();
                if (height > 0) {
                    ApplyTrayHoverBorderLayout(border, height);
                }
                ScheduleTrayHoverReapply();
            });
    }

    state.visibilityToken = border.RegisterPropertyChangedCallback(
        UIElement::VisibilityProperty(),
        [isTrayBorder](DependencyObject const& sender,
                       DependencyProperty const&) {
            auto border = sender.try_as<Border>();
            if (!border || border.Visibility() != Visibility::Visible) {
                return;
            }

            double radius = g_hoverRadiusDip.load();
            if (radius > 0) {
                ApplyHoverBorderShape(border, radius);
            }

            if (isTrayBorder) {
                double height = g_trayHoverHeightDip.load();
                if (height > 0) {
                    ApplyTrayHoverBorderLayout(border, height);
                }

                // The capsule can appear before Explorer has re-centered its
                // OmniButton parent. Move the nearest OmniButton to the
                // middle of the tray row immediately instead of waiting for
                // the next full apply.
                try {
                    FrameworkElement parent =
                        VisualTreeHelper::GetParent(border)
                            .try_as<FrameworkElement>();
                    for (int depth = 0; depth < 5 && parent; depth++) {
                        auto className = winrt::get_class_name(parent);
                        if (ClassNameEndsWith(className, L"OmniButton")) {
                            ApplyTrayOmniButtonLayout(
                                parent, g_trayRowHeightDip.load());
                            break;
                        }
                        parent = VisualTreeHelper::GetParent(parent)
                                     .try_as<FrameworkElement>();
                    }
                } catch (...) {
                }

                ScheduleTrayHoverReapply();
            }

            static std::atomic<int> s_hoverReinforceLogs{0};
            if (s_hoverReinforceLogs.fetch_add(1) < 12) {
                AppendTrayLayoutDiag(
                    isTrayBorder ? L"hover reinforce name=BackgroundBorder"
                                 : L"hover reinforce name=BackgroundElement");
            }
        });

    record.savedHoverBorderShapes.push_back(std::move(state));
}

void RestoreHoverBorderShapes(
    std::vector<SavedHoverBorderShape> const& savedStates) {
    for (auto const& state : savedStates) {
        if (auto element = state.element.get()) {
            try {
                if (auto border = element.try_as<Border>()) {
                    if (state.cornerRadiusToken) {
                        border.UnregisterPropertyChangedCallback(
                            Border::CornerRadiusProperty(),
                            state.cornerRadiusToken);
                    }
                    if (state.heightToken) {
                        border.UnregisterPropertyChangedCallback(
                            FrameworkElement::HeightProperty(),
                            state.heightToken);
                    }
                    if (state.visibilityToken) {
                        border.UnregisterPropertyChangedCallback(
                            UIElement::VisibilityProperty(),
                            state.visibilityToken);
                    }
                    border.CornerRadius(state.cornerRadius);
                }
            } catch (...) {
            }
        }
    }
}

void ApplyHoverBorderShape(FrameworkElement const& element, double radius) {
    try {
        if (auto border = element.try_as<Border>()) {
            border.CornerRadius(
                CornerRadius{radius, radius, radius, radius});
        }
    } catch (...) {
    }
}

void CollectTaskbarBackgroundLayers(FrameworkElement const& root,
                                    std::vector<FrameworkElement>& result,
                                    bool insideBackground = false) {
    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        bool isBackground = false;
        try {
            isBackground =
                ClassNameEndsWith(winrt::get_class_name(child),
                                  L"TaskbarBackground") &&
                !ElementNameEquals(child, L"HoverFlyoutBackgroundControl");
        } catch (...) {
        }

        if (isBackground) {
            result.push_back(child);
        }

        if (insideBackground || isBackground) {
            if (child.try_as<Grid>()) {
                result.push_back(child);
            }
        }

        CollectTaskbarBackgroundLayers(child, result,
                                       insideBackground || isBackground);
    }
}

void SaveBackgroundLayerState(AppliedElement& record,
                              FrameworkElement const& element) {
    for (auto const& state : record.savedBackgroundLayers) {
        if (state.element.get() == element) {
            return;
        }
    }

    SavedBackgroundLayer state;
    state.element = winrt::make_weak(element);
    if (auto panel = element.try_as<Panel>()) {
        state.isPanel = true;
        state.background = panel.Background();
    } else if (auto control = element.try_as<Control>()) {
        state.background = control.Background();
    } else if (auto border = element.try_as<Border>()) {
        state.isBorder = true;
        state.background = border.Background();
    } else if (auto presenter = element.try_as<ContentPresenter>()) {
        state.isPresenter = true;
        state.background = presenter.Background();
    } else {
        return;
    }
    record.savedBackgroundLayers.push_back(std::move(state));
}

void ClearBackgroundLayer(FrameworkElement const& element,
                          Brush transparentBrush) {
    try {
        if (auto panel = element.try_as<Panel>()) {
            panel.Background(transparentBrush);
        } else if (auto control = element.try_as<Control>()) {
            control.Background(transparentBrush);
        } else if (auto border = element.try_as<Border>()) {
            border.Background(transparentBrush);
        } else if (auto presenter = element.try_as<ContentPresenter>()) {
            presenter.Background(transparentBrush);
        }
    } catch (...) {
    }
}

void RestoreBackgroundLayers(
    std::vector<SavedBackgroundLayer> const& savedStates) {
    for (auto const& state : savedStates) {
        if (auto element = state.element.get()) {
            try {
                if (state.isPanel) {
                    if (auto panel = element.try_as<Panel>()) {
                        panel.Background(state.background);
                    }
                } else if (state.isBorder) {
                    if (auto border = element.try_as<Border>()) {
                        border.Background(state.background);
                    }
                } else if (state.isPresenter) {
                    if (auto presenter = element.try_as<ContentPresenter>()) {
                        presenter.Background(state.background);
                    }
                } else if (auto control = element.try_as<Control>()) {
                    control.Background(state.background);
                }
            } catch (...) {
            }
        }
    }
}

// The taskbar creates a separate TaskbarBackground named
// HoverFlyoutBackgroundControl behind the tray anchor when Quick Settings is
// open. It is intentionally excluded from the normal bar material cleanup,
// but on a floating gradient bar that system material shows up as a light
// square behind the rounded hover capsule. Collect only its material layers;
// the BackgroundElement/BackgroundBorder hover visuals stay untouched.
void CollectFlyoutMaterialLayers(FrameworkElement const& root,
                                 std::vector<FrameworkElement>& result,
                                 int depth = 0) {
    if (depth > 12) {
        return;
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        try {
            if (ElementNameEquals(child, L"BackgroundElement") ||
                ElementNameEquals(child, L"BackgroundBorder")) {
                continue;
            }

            if (auto rect = child.try_as<Shapes::Rectangle>()) {
                if (rect.Fill()) {
                    result.push_back(child);
                }
            } else if (auto panel = child.try_as<Panel>()) {
                if (panel.Background()) {
                    result.push_back(child);
                }
            } else if (auto control = child.try_as<Control>()) {
                if (control.Background()) {
                    result.push_back(child);
                }
            } else if (auto border = child.try_as<Border>()) {
                if (border.Background()) {
                    result.push_back(child);
                }
            } else if (auto presenter = child.try_as<ContentPresenter>()) {
                if (presenter.Background()) {
                    result.push_back(child);
                }
            }
        } catch (...) {
        }

        CollectFlyoutMaterialLayers(child, result, depth + 1);
    }
}

void CollectHoverFlyoutLayers(FrameworkElement const& root,
                              std::vector<FrameworkElement>& result,
                              int depth = 0) {
    if (depth > 12) {
        return;
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        if (ElementNameEquals(child, L"HoverFlyoutBackgroundControl")) {
            result.push_back(child);
            CollectFlyoutMaterialLayers(child, result, depth + 1);
            continue;
        }

        CollectHoverFlyoutLayers(child, result, depth + 1);
    }
}

bool IsTrayInteractiveClass(winrt::hstring const& className) {
    return ClassNameEndsWith(className, L"IconView") ||
           ClassNameEndsWith(className, L"NormalButton") ||
           ClassNameEndsWith(className, L"AccentButton") ||
           ClassNameEndsWith(className, L"OmniButton") ||
           ClassNameEndsWith(className, L"OmniButtonCenter") ||
           ClassNameEndsWith(className, L"OmniButtonLeft");
}

// The system tray frame is a sibling of TaskbarFrame and is drawn on top of
// the right end of the floating bar. Clearing only SystemTrayFrame's own
// Background leaves the material painted by SystemTrayFrameGrid and the
// container layers below it, which shows up as a vertical tint seam at the
// app-icon/tray boundary. Clear only container backgrounds here; the named
// hover borders and the interactive icon views must keep their visuals.
void CollectTrayBackgroundLayers(FrameworkElement const& root,
                                 std::vector<FrameworkElement>& result,
                                 double trayWidth,
                                 int depth = 0) {
    if (depth > 8) {
        return;
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        if (ElementNameEquals(child, L"BackgroundElement") ||
            ElementNameEquals(child, L"BackgroundBorder") ||
            ElementNameEquals(child, L"HoverFlyoutBackgroundControl")) {
            continue;
        }

        // The tray can carry its own TaskbarBackground with a named
        // BackgroundFill rectangle, mirroring the main taskbar. A container
        // Background alone is not enough to remove that fill.
        if (auto rect = child.try_as<Shapes::Rectangle>()) {
            if (ElementNameEquals(child, L"BackgroundFill")) {
                result.push_back(child);
            }
            continue;
        }

        winrt::hstring className;
        try {
            className = winrt::get_class_name(child);
        } catch (...) {
        }

        // Only the tray frame's material grid and a nested TaskbarBackground
        // paint bar-wide layers. Keep the icon stacks and their hover
        // containers untouched.
        bool isTrayMaterial = false;
        try {
            isTrayMaterial =
                ElementNameEquals(child, L"SystemTrayFrameGrid") ||
                ClassNameEndsWith(className, L"TaskbarBackground");
        } catch (...) {
        }

        bool interactive = false;
        try {
            interactive = IsTrayInteractiveClass(className);
        } catch (...) {
        }

        if (!interactive) {
            bool hasBackgroundProperty = false;
            Media::Brush background = nullptr;
            double childWidth = 0.0;
            try {
                if (auto panel = child.try_as<Panel>()) {
                    hasBackgroundProperty = true;
                    background = panel.Background();
                } else if (auto control = child.try_as<Control>()) {
                    hasBackgroundProperty = true;
                    background = control.Background();
                }
                childWidth = child.ActualWidth();
            } catch (...) {
            }

            bool isWideContainer =
                trayWidth > 0 && childWidth >= trayWidth * 0.8;
            if (hasBackgroundProperty && background &&
                (isTrayMaterial || isWideContainer)) {
                result.push_back(child);
            }
        }

        if (!interactive) {
            CollectTrayBackgroundLayers(child, result, trayWidth, depth + 1);
        }
    }
}

void AppendTrayLayoutDiag(PCWSTR text);
void KickTaskbarHeightUpdate();

std::atomic<HWND> g_controlCenterWnd{nullptr};
HWINEVENTHOOK g_controlCenterWinEventHook = nullptr;
std::atomic<unsigned long> g_lastControlCenterFixTick{0};
std::atomic<DWORD> g_lastTaskbarHeightKickTick{0};
std::atomic<double> g_lastPillWidthPx{0.0};
constexpr double kControlCenterRightInsetDip = 12.8;
// Left inset of the taskbar icons inside the floating bar. The system applies
// a similar inset in its own left-aligned layout; applying it ourselves makes
// the icon row look the same with either system alignment setting.
constexpr double kTaskbarIconsLeftInsetDip = 11.0;
// Measured native insets: about 9 DIP with Center alignment and about 21 DIP
// with Left alignment. The target inset from the reference screenshot is
// 20 DIP, so Center needs +11 DIP and Left needs -1 DIP. Subtracting 10 DIP
// in Left mode over-shifted the row by about 9 DIP.
constexpr double kTaskbarIconsLeftModeCorrectionDip = 1.0;
HANDLE g_controlCenterWatchdogThread = nullptr;
std::atomic<bool> g_controlCenterWatchdogStop{false};
std::atomic<int> g_controlCenterDiagWrites{0};
HANDLE g_taskbarAlignmentWatcherThread = nullptr;
HANDLE g_taskbarAlignmentWatcherStopEvent = nullptr;
std::atomic<bool> g_taskbarAlignmentWatcherStop{false};

thread_local HWND g_cachedTaskbarWnd = nullptr;
thread_local bool g_cachedTaskbarWndSearched = false;

// The mod runs both in the taskbar process (explorer.exe) and in the process
// which hosts the Quick Settings panel (ShellHost.exe on recent Windows
// versions), as the latter is the one that positions the panel.
enum class HostKind {
    Taskbar,
    ControlCenter,
};

HostKind g_hostKind = HostKind::Taskbar;

HostKind GetHostKind() {
    WCHAR path[MAX_PATH];
    if (!GetModuleFileName(nullptr, path, ARRAYSIZE(path))) {
        return HostKind::Taskbar;
    }

    PCWSTR fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;

    if (_wcsicmp(fileName, L"ShellHost.exe") == 0 ||
        _wcsicmp(fileName, L"ShellExperienceHost.exe") == 0) {
        return HostKind::ControlCenter;
    }

    return HostKind::Taskbar;
}

// A window which is owned by another process can only be moved after it has
// already been shown, which makes it visibly jump. Instead, the taskbar side
// publishes the position that it wants as a property of the panel window, and
// the process which hosts the panel applies it while the window is being
// moved.
constexpr WCHAR kControlCenterTargetRightProp[] =
    L"FloatingTaskbarControlCenterTargetRight";

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
using MoveWindow_t = decltype(&MoveWindow);
MoveWindow_t MoveWindow_Original;

bool IsOwnControlCenterWindow(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    DWORD processId = 0;
    if (!GetWindowThreadProcessId(hWnd, &processId) ||
        processId != GetCurrentProcessId()) {
        return false;
    }

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"ControlCenterWindow") == 0;
}

bool IsTaskbarWindowClass(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    WCHAR className[64];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

void EnsureTaskbarWindowHeight(HWND hWnd, int areaHeightPx) {
    if (!hWnd || areaHeightPx <= 0) {
        return;
    }

    RECT rect{};
    if (!GetWindowRect(hWnd, &rect)) {
        return;
    }

    int currentHeight = rect.bottom - rect.top;
    if (currentHeight <= 0 || currentHeight >= areaHeightPx) {
        return;
    }

    int newY = rect.top;
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(MONITORINFO);
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
        int monitorBottom = monitorInfo.rcMonitor.bottom;
        if (rect.bottom >= monitorBottom - 2 &&
            rect.bottom <= monitorBottom + 2) {
            newY = monitorBottom - areaHeightPx;
        }
    }

    SetWindowPos(hWnd, nullptr, rect.left, newY, rect.right - rect.left,
                 areaHeightPx, SWP_NOZORDER | SWP_NOACTIVATE);

    {
        WCHAR diag[240];
        swprintf_s(diag, L"taskbar EnsureHeight h=%d->%d y=%d->%d",
                   currentHeight, areaHeightPx, rect.top, newY);
        AppendTrayLayoutDiag(diag);
    }
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    HookScope hookScope;

    // Explorer initially creates the auto-hide taskbar at its default 48 DIP
    // height and only grows it when it is first shown. Force every explicit
    // taskbar resize to the configured area height so the first visible frame
    // is already the correct height.
    if (IsTaskbarWindowClass(hWnd)) {
        const int areaHeightPx = g_settingTaskbarAreaHeight.load();
        if (g_settingFloatingBarEnabled.load() && areaHeightPx > 0) {
            RECT rect{};
            GetWindowRect(hWnd, &rect);

            int currentWidth = rect.right - rect.left;
            int currentHeight = rect.bottom - rect.top;
            int requestedHeight =
                (uFlags & SWP_NOSIZE) ? currentHeight : cy;
            int requestedWidth = (uFlags & SWP_NOSIZE) ? currentWidth : cx;

            if (currentWidth <= 0 || currentHeight <= 0 ||
                requestedWidth <= 0 || requestedHeight <= 0 ||
                requestedHeight >= areaHeightPx) {
                if (!IsOwnControlCenterWindow(hWnd)) {
                    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y,
                                                 cx, cy, uFlags);
                }
            } else {

                int newX = X;
                int newY = Y;
                if (uFlags & SWP_NOMOVE) {
                    newX = rect.left;
                    newY = rect.top;
                } else if (Y == 0 && rect.top != 0) {
                    // Some shell paths pass a zero position while the window
                    // is already anchored at the bottom. Keep its current top.
                    newY = rect.top;
                }

                MONITORINFO monitorInfo{};
                monitorInfo.cbSize = sizeof(MONITORINFO);
                HMONITOR monitor =
                    MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
                if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
                    int monitorBottom = monitorInfo.rcMonitor.bottom;
                    int originalBottom = newY + requestedHeight;
                    if (originalBottom >= monitorBottom - 2 &&
                        originalBottom <= monitorBottom + 2) {
                        // The taskbar is bottom-anchored. Keep its bottom on
                        // the monitor edge while replacing the height.
                        newY = monitorBottom - areaHeightPx;
                    }
                }

                {
                    WCHAR diag[240];
                    swprintf_s(diag,
                               L"taskbar SetWindowPos cy=%d->%d y=%d->%d",
                               requestedHeight, areaHeightPx, Y, newY);
                    AppendTrayLayoutDiag(diag);
                }

                uFlags &= ~(SWP_NOSIZE | SWP_NOMOVE);
                return SetWindowPos_Original(hWnd, hWndInsertAfter, newX,
                                             newY, requestedWidth,
                                             areaHeightPx, uFlags);
            }
        }
    }

    if (!IsOwnControlCenterWindow(hWnd)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    }

    ULONG_PTR targetRightPx =
        (ULONG_PTR)GetPropW(hWnd, kControlCenterTargetRightProp);
    if (!targetRightPx ||
        (uFlags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE)) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    }

    RECT rect{};
    GetWindowRect(hWnd, &rect);

    if (uFlags & SWP_NOMOVE) {
        // The window is being resized, adjust its position as well.
        uFlags &= ~SWP_NOMOVE;
        X = rect.left;
        Y = rect.top;
    }

    int width = (uFlags & SWP_NOSIZE) ? rect.right - rect.left : cx;
    if (width <= 0) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    }

    X = static_cast<int>(targetRightPx) - width;

    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

BOOL WINAPI MoveWindow_Hook(HWND hWnd,
                            int X,
                            int Y,
                            int nWidth,
                            int nHeight,
                            BOOL bRepaint) {
    HookScope hookScope;

    if (IsTaskbarWindowClass(hWnd)) {
        const int areaHeightPx = g_settingTaskbarAreaHeight.load();
        if (g_settingFloatingBarEnabled.load() && areaHeightPx > 0 &&
            nWidth > 0 && nHeight > 0 && nHeight < areaHeightPx) {
            int newY = Y;
            MONITORINFO monitorInfo{};
            monitorInfo.cbSize = sizeof(MONITORINFO);
            HMONITOR monitor =
                MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
                int monitorBottom = monitorInfo.rcMonitor.bottom;
                if (Y + nHeight >= monitorBottom - 2 &&
                    Y + nHeight <= monitorBottom + 2) {
                    newY = monitorBottom - areaHeightPx;
                }
            }

            {
                WCHAR diag[240];
                swprintf_s(diag,
                           L"taskbar MoveWindow h=%d->%d y=%d->%d",
                           nHeight, areaHeightPx, Y, newY);
                AppendTrayLayoutDiag(diag);
            }

            return MoveWindow_Original(hWnd, X, newY, nWidth, areaHeightPx,
                                       bRepaint);
        }
    }

    return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

void LoadSettings() {
    auto readString = [](PCWSTR name) {
        WindhawkUtils::StringSetting setting =
            WindhawkUtils::StringSetting::make(name);
        return std::wstring(setting.get());
    };

    bool floatingBarEnabled =
        Wh_GetIntSetting(L"floatingBar.enabled") != 0;

    int taskbarWidth = Wh_GetIntSetting(L"floatingBar.taskbarWidth");
    if (taskbarWidth < 0) {
        taskbarWidth = 0;
    }

    int taskbarAreaHeight = Wh_GetIntSetting(L"floatingBar.taskbarAreaHeight");
    if (taskbarAreaHeight < 0) {
        taskbarAreaHeight = 0;
    }

    int floatMargin = Wh_GetIntSetting(L"floatingBar.floatMargin");
    if (floatMargin < 0) {
        floatMargin = 0;
    }

    int cornerRadius = Wh_GetIntSetting(L"floatingBar.cornerRadius");
    if (cornerRadius < 0) {
        cornerRadius = 0;
    }

    int hoverCornerRadius =
        Wh_GetIntSetting(L"floatingBar.hoverCornerRadius");
    hoverCornerRadius = std::clamp(hoverCornerRadius, 0, 1000);

    bool cycleThroughWindows =
        Wh_GetIntSetting(L"cycling.cycleThroughWindows") != 0;

    AutoHideMode autoHideMode = AutoHideMode::intersected;
    bool autoHideEnabled = Wh_GetIntSetting(L"autoHide.enabled") != 0;
    PCWSTR autoHideModeSetting = Wh_GetStringSetting(L"autoHide.mode");
    if (wcscmp(autoHideModeSetting, L"maximized") == 0) {
        autoHideMode = AutoHideMode::maximized;
    } else if (wcscmp(autoHideModeSetting, L"always") == 0) {
        autoHideMode = AutoHideMode::always;
    } else if (wcscmp(autoHideModeSetting, L"never") == 0) {
        autoHideMode = AutoHideMode::never;
    }
    Wh_FreeStringSetting(autoHideModeSetting);

    bool autoHideForegroundWindowOnly =
        Wh_GetIntSetting(L"autoHide.foregroundWindowOnly") != 0;
    bool autoHidePrimaryMonitorOnly =
        Wh_GetIntSetting(L"autoHide.primaryMonitorOnly") != 0;

    std::unordered_set<std::wstring> excludedPrograms;
    for (int i = 0;; i++) {
        PCWSTR program =
            Wh_GetStringSetting(L"autoHide.excludedPrograms[%d]", i);

        bool hasProgram = program && *program;
        if (hasProgram) {
            std::wstring programUpper = program;
            LCMapStringEx(
                LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, programUpper.data(),
                static_cast<int>(programUpper.length()), programUpper.data(),
                static_cast<int>(programUpper.length()), nullptr, nullptr, 0);

            excludedPrograms.insert(std::move(programUpper));
        }

        Wh_FreeStringSetting(program);

        if (!hasProgram) {
            break;
        }
    }

    BackgroundSettings backgroundSettings;
    PCWSTR backgroundStyle =
        Wh_GetStringSetting(L"floatingBar.backgroundStyle");
    if (wcscmp(backgroundStyle, L"system") == 0) {
        backgroundSettings.style = BackgroundStyle::system;
    } else if (wcscmp(backgroundStyle, L"transparent") == 0) {
        backgroundSettings.style = BackgroundStyle::transparent;
    } else if (wcscmp(backgroundStyle, L"translucent") == 0 ||
               wcscmp(backgroundStyle, L"acrylic") == 0) {
        backgroundSettings.style = BackgroundStyle::translucent;
    } else if (wcscmp(backgroundStyle, L"solid") == 0) {
        backgroundSettings.style = BackgroundStyle::solid;
    } else if (wcscmp(backgroundStyle, L"gradient") == 0) {
        backgroundSettings.style = BackgroundStyle::gradient;
    } else {
        backgroundSettings.style = BackgroundStyle::custom;
    }
    Wh_FreeStringSetting(backgroundStyle);

    backgroundSettings.color = readString(L"floatingBar.backgroundColor");
    backgroundSettings.opacity =
        std::clamp(Wh_GetIntSetting(L"floatingBar.backgroundOpacity"), 0, 100);
    backgroundSettings.gradientColor1 =
        readString(L"floatingBar.gradientColor1");
    backgroundSettings.gradientColor2 =
        readString(L"floatingBar.gradientColor2");
    backgroundSettings.gradientAngle =
        Wh_GetIntSetting(L"floatingBar.gradientAngle");
    backgroundSettings.gradientStartPosition = std::clamp(
        Wh_GetIntSetting(L"floatingBar.gradientStartPosition"), 0, 100);
    backgroundSettings.gradientEndPosition = std::clamp(
        Wh_GetIntSetting(L"floatingBar.gradientEndPosition"), 0, 100);
    backgroundSettings.customXaml =
        readString(L"floatingBar.backgroundBrush");

    // Tray system icon tweaks.
    g_settingHideVolumeIcon =
        Wh_GetIntSetting(L"trayIcons.hideVolumeIcon") != 0;
    g_settingHideNetworkIcon =
        Wh_GetIntSetting(L"trayIcons.hideNetworkIcon") != 0;
    g_settingHideBatteryIcon =
        Wh_GetIntSetting(L"trayIcons.hideBatteryIcon") != 0;
    g_settingGrayscaleBatteryIcon =
        Wh_GetIntSetting(L"trayIcons.grayscaleBatteryIcon") != 0;
    g_settingHideMicrophoneIcon =
        Wh_GetIntSetting(L"trayIcons.hideMicrophoneIcon") != 0;
    g_settingHideGeolocationIcon =
        Wh_GetIntSetting(L"trayIcons.hideGeolocationIcon") != 0;
    g_settingHideStudioEffectsIcon =
        Wh_GetIntSetting(L"trayIcons.hideStudioEffectsIcon") != 0;
    g_settingHideRecallIcon = Wh_GetIntSetting(L"trayIcons.hideRecallIcon") != 0;
    g_settingHideLanguageBar =
        Wh_GetIntSetting(L"trayIcons.hideLanguageBar") != 0;
    g_settingHideLanguageSupplementaryIcons =
        Wh_GetIntSetting(L"trayIcons.hideLanguageSupplementaryIcons") != 0;

    int hideBellIcon = (int)HideBellIcon::never;
    PCWSTR hideBellIconSetting = Wh_GetStringSetting(L"trayIcons.hideBellIcon");
    if (wcscmp(hideBellIconSetting, L"whenInactive") == 0) {
        hideBellIcon = (int)HideBellIcon::whenInactive;
    } else if (wcscmp(hideBellIconSetting, L"whenInactiveAndNoDnd") == 0) {
        hideBellIcon = (int)HideBellIcon::whenInactiveAndNoDnd;
    } else if (wcscmp(hideBellIconSetting, L"always") == 0) {
        hideBellIcon = (int)HideBellIcon::always;
    }
    Wh_FreeStringSetting(hideBellIconSetting);
    g_settingHideBellIcon = hideBellIcon;

    g_settingShowDesktopButtonWidth =
        std::clamp(Wh_GetIntSetting(L"trayIcons.showDesktopButtonWidth"), 0,
                   800);

    // Dock hover animation.
    {
        DockSettings dockSettings;

        int rawMaxScale = Wh_GetIntSetting(L"dockAnimation.maxScale");
        if (rawMaxScale < 101) {
            rawMaxScale = 101;
        } else if (rawMaxScale > 220) {
            rawMaxScale = 220;
        }

        int effectRadius = Wh_GetIntSetting(L"dockAnimation.effectRadius");
        if (effectRadius <= 0) {
            effectRadius = 100;
        }

        int spacingFactor = Wh_GetIntSetting(L"dockAnimation.spacingFactor");
        if (spacingFactor < 0) {
            spacingFactor = 50;
        }

        int bounceDelay = Wh_GetIntSetting(L"dockAnimation.bounceDelay");
        if (bounceDelay < 0) {
            bounceDelay = 100;
        }

        int focusDuration = Wh_GetIntSetting(L"dockAnimation.focusDuration");
        if (focusDuration <= 0) {
            focusDuration = 150;
        }

        int lerpSpeed = Wh_GetIntSetting(L"dockAnimation.lerpSpeed");
        if (lerpSpeed < 0) {
            lerpSpeed = 0;
        } else if (lerpSpeed > 60) {
            lerpSpeed = 60;
        }

        dockSettings.enabled =
            Wh_GetIntSetting(L"dockAnimation.enabled") != 0;
        dockSettings.animationType =
            Wh_GetIntSetting(L"dockAnimation.animationType");
        dockSettings.maxScale = rawMaxScale / 100.0;
        dockSettings.effectRadius = effectRadius;
        dockSettings.spacingFactor = spacingFactor / 100.0;
        dockSettings.bounceDelay = bounceDelay;
        dockSettings.focusDuration = focusDuration;
        dockSettings.disableVerticalBounce =
            Wh_GetIntSetting(L"dockAnimation.disableVerticalBounce") != 0;
        dockSettings.taskbarLabelsMode =
            Wh_GetIntSetting(L"dockAnimation.taskbarLabelsMode") != 0;
        dockSettings.excludeSystemButtonsMode =
            Wh_GetIntSetting(L"dockAnimation.excludeSystemButtonsMode");
        dockSettings.lerpSpeed = lerpSpeed;
        dockSettings.disableBounce =
            Wh_GetIntSetting(L"dockAnimation.disableBounce") != 0;

        std::lock_guard<std::mutex> lock(g_dockSettingsMutex);
        g_dockSettings = dockSettings;
    }

    // Taskbar clock customization.
    {
        ClockSettings clockSettings;

        auto readString = [](PCWSTR name) {
            WindhawkUtils::StringSetting setting =
                WindhawkUtils::StringSetting::make(name);
            return std::wstring(setting.get());
        };

        clockSettings.enabled = Wh_GetIntSetting(L"clock.enabled") != 0;
        clockSettings.showSeconds =
            Wh_GetIntSetting(L"clock.showSeconds") != 0;
        clockSettings.timeFormat = readString(L"clock.timeFormat");
        clockSettings.dateFormat = readString(L"clock.dateFormat");
        clockSettings.dateLocale = readString(L"clock.dateLocale");
        clockSettings.weekdayFormat = readString(L"clock.weekdayFormat");
        clockSettings.weekdayFormatCustom =
            readString(L"clock.weekdayFormatCustom");
        clockSettings.topLine = readString(L"clock.topLine");
        clockSettings.bottomLine = readString(L"clock.bottomLine");
        clockSettings.tooltipLine = readString(L"clock.tooltipLine");

        PCWSTR tooltipLineMode = Wh_GetStringSetting(L"clock.tooltipLineMode");
        clockSettings.tooltipLineMode =
            wcscmp(tooltipLineMode, L"append") == 0
                ? ClockTooltipLineMode::append
                : ClockTooltipLineMode::replace;
        Wh_FreeStringSetting(tooltipLineMode);

        PCWSTR percentageFormat =
            Wh_GetStringSetting(L"clock.percentageFormat");
        if (wcscmp(percentageFormat, L"spacePadding") == 0) {
            clockSettings.percentageFormat =
                ClockPercentageFormat::spacePadding;
        } else if (wcscmp(percentageFormat, L"singleSpacePadding") == 0) {
            clockSettings.percentageFormat =
                ClockPercentageFormat::singleSpacePadding;
        } else if (wcscmp(percentageFormat, L"zeroPadding") == 0) {
            clockSettings.percentageFormat = ClockPercentageFormat::zeroPadding;
        } else if (wcscmp(percentageFormat, L"noPadding") == 0) {
            clockSettings.percentageFormat = ClockPercentageFormat::noPadding;
        }
        Wh_FreeStringSetting(percentageFormat);

        int updateInterval = Wh_GetIntSetting(L"clock.updateInterval");
        clockSettings.updateInterval = updateInterval > 0 ? updateInterval : 1;

        auto parseTransferFormat = [](PCWSTR value) {
            if (wcscmp(value, L"mbs") == 0) {
                return ClockTransferFormat::mbs;
            }
            if (wcscmp(value, L"mbsNumberOnly") == 0) {
                return ClockTransferFormat::mbsNumberOnly;
            }
            if (wcscmp(value, L"mbits") == 0) {
                return ClockTransferFormat::mbits;
            }
            if (wcscmp(value, L"mbitsNumberOnly") == 0) {
                return ClockTransferFormat::mbitsNumberOnly;
            }
            if (wcscmp(value, L"mbitsDynamic") == 0) {
                return ClockTransferFormat::mbitsDynamic;
            }
            return ClockTransferFormat::mbsDynamic;
        };

        PCWSTR networkMetricsFormat =
            Wh_GetStringSetting(L"clock.networkMetricsFormat");
        clockSettings.networkMetricsFormat =
            parseTransferFormat(networkMetricsFormat);
        Wh_FreeStringSetting(networkMetricsFormat);

        clockSettings.networkMetricsFixedDecimals = std::clamp(
            Wh_GetIntSetting(L"clock.networkMetricsFixedDecimals"), -1, 6);

        PCWSTR diskMetricsFormat =
            Wh_GetStringSetting(L"clock.diskMetricsFormat");
        clockSettings.diskMetricsSameAsNetwork =
            wcscmp(diskMetricsFormat, L"sameAsNetwork") == 0;
        if (!clockSettings.diskMetricsSameAsNetwork) {
            clockSettings.diskMetricsFormat =
                parseTransferFormat(diskMetricsFormat);
        }
        Wh_FreeStringSetting(diskMetricsFormat);

        clockSettings.diskMetricsFixedDecimals = std::clamp(
            Wh_GetIntSetting(L"clock.diskMetricsFixedDecimals"), -1, 6);
        clockSettings.networkAdapterName =
            readString(L"clock.networkAdapterName");
        clockSettings.gpuAdapterName =
            readString(L"clock.gpuAdapterName");
        clockSettings.mediaInfoFormat =
            readString(L"clock.mediaInfoFormat");
        clockSettings.noMediaText = readString(L"clock.noMediaText");
        clockSettings.mediaMaxLength = std::clamp(
            Wh_GetIntSetting(L"clock.mediaMaxLength"), 0, 4096);
        clockSettings.mediaRemoveBrackets =
            Wh_GetIntSetting(L"clock.mediaRemoveBrackets") != 0;

        for (int i = 0;; i++) {
            PCWSTR player = Wh_GetStringSetting(
                L"clock.mediaIgnoredPlayers[%d]", i);
            bool hasPlayer = player && *player;
            if (hasPlayer) {
                clockSettings.mediaIgnoredPlayers.emplace_back(player);
            }
            Wh_FreeStringSetting(player);
            if (!hasPlayer) {
                break;
            }
        }

        clockSettings.weatherLocation =
            readString(L"clock.weatherLocation");
        clockSettings.weatherFormat =
            readString(L"clock.weatherFormat");
        if (clockSettings.weatherFormat.empty()) {
            clockSettings.weatherFormat = L"%c %t %w";
        }
        clockSettings.weatherUnits =
            readString(L"clock.weatherUnits");

        for (int i = 0; i < 16; i++) {
            PCWSTR urlSetting = Wh_GetStringSetting(
                L"clock.webContentsItems[%d].url", i);
            if (!urlSetting || !*urlSetting) {
                Wh_FreeStringSetting(urlSetting);
                break;
            }

            ClockWebContentSettings item;
            item.url = urlSetting;
            Wh_FreeStringSetting(urlSetting);

            PCWSTR blockStart = Wh_GetStringSetting(
                L"clock.webContentsItems[%d].blockStart", i);
            PCWSTR start = Wh_GetStringSetting(
                L"clock.webContentsItems[%d].start", i);
            PCWSTR end = Wh_GetStringSetting(
                L"clock.webContentsItems[%d].end", i);
            item.blockStart = blockStart ? blockStart : L"";
            item.start = start ? start : L"";
            item.end = end ? end : L"";
            Wh_FreeStringSetting(blockStart);
            Wh_FreeStringSetting(start);
            Wh_FreeStringSetting(end);
            item.maxLength = std::clamp(
                Wh_GetIntSetting(
                    L"clock.webContentsItems[%d].maxLength", i),
                0, 4096);
            clockSettings.webContentsItems.push_back(std::move(item));
        }

        clockSettings.webContentsUpdateInterval = std::clamp(
            Wh_GetIntSetting(L"clock.webContentsUpdateInterval"), 1, 1440);

        auto readTextStyle = [&](PCWSTR prefix) {
            ClockTextStyleSettings style;
            WCHAR key[128];
#define CLOCK_READ_STYLE_STRING(field, suffix)                              \
    swprintf_s(key, L"%s.%s", prefix, suffix);                              \
    style.field = readString(key)
            CLOCK_READ_STYLE_STRING(textColor, L"textColor");
            CLOCK_READ_STYLE_STRING(fontFamily, L"fontFamily");
            CLOCK_READ_STYLE_STRING(fontWeight, L"fontWeight");
            CLOCK_READ_STYLE_STRING(fontStyle, L"fontStyle");
            CLOCK_READ_STYLE_STRING(fontStretch, L"fontStretch");
            CLOCK_READ_STYLE_STRING(textAlignment, L"textAlignment");
#undef CLOCK_READ_STYLE_STRING
            swprintf_s(key, L"%s.fontSize", prefix);
            style.fontSize = std::clamp(Wh_GetIntSetting(key), 0, 200);
            swprintf_s(key, L"%s.characterSpacing", prefix);
            style.characterSpacing =
                std::clamp(Wh_GetIntSetting(key), -1000, 1000);
            swprintf_s(key, L"%s.lineHeight", prefix);
            style.lineHeight = std::clamp(Wh_GetIntSetting(key), 0, 1000);
            return style;
        };

        clockSettings.timeStyle = readTextStyle(L"clock.timeStyle");
        clockSettings.dateStyle = readTextStyle(L"clock.dateStyle");

        std::lock_guard<std::mutex> lock(g_clockSettingsMutex);
        g_clockSettings = std::move(clockSettings);
    }

    g_settingTaskbarWidth = taskbarWidth;
    g_settingTaskbarAreaHeight = taskbarAreaHeight;
    g_settingFloatMargin = floatMargin;
    g_settingCornerRadius = cornerRadius;
    g_settingHoverCornerRadius = hoverCornerRadius;
    g_settingFloatingBarEnabled = floatingBarEnabled;
    g_settingCycleThroughWindows = cycleThroughWindows;
    g_settingAutoHideMode = (int)autoHideMode;
    g_settingAutoHideEnabled = autoHideEnabled;
    g_settingAutoHideForegroundWindowOnly = autoHideForegroundWindowOnly;
    g_settingAutoHidePrimaryMonitorOnly = autoHidePrimaryMonitorOnly;

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_backgroundSettings = std::move(backgroundSettings);
    }

    {
        std::lock_guard<std::mutex> lock(g_autoHideExcludedProgramsMutex);
        g_autoHideExcludedPrograms = std::move(excludedPrograms);
    }
}

void ResetLoggedFlags() {
    g_loggedMissingRootGrid = false;
    g_loggedMissingRects = false;
    g_loggedMissingTray = false;
    g_loggedMissingIsland = false;
    g_loggedRegionBusy = false;
    g_loggedMissingXamlRoot = false;
    g_loggedNoDirectApply = false;
    g_loggedStripUnknown = false;
}

bool ClassNameEquals(winrt::hstring const& className,
                     std::wstring_view expected) {
    return std::wstring_view(className.c_str(), className.size()) == expected;
}

bool ClassNameEndsWith(winrt::hstring const& className,
                       std::wstring_view suffix) {
    std::wstring_view name(className.c_str(), className.size());
    if (name.size() < suffix.size()) {
        return false;
    }

    size_t offset = name.size() - suffix.size();
    if (offset > 0 && name[offset - 1] != L'.') {
        return false;
    }

    return name.compare(offset, suffix.size(), suffix) == 0;
}

bool ElementNameEquals(FrameworkElement const& element,
                       std::wstring_view expected) {
    auto elementName = element.Name();
    return std::wstring_view(elementName.c_str(), elementName.size()) ==
           expected;
}

FrameworkElement FindChildByPredicate(
    FrameworkElement const& root,
    std::function<bool(FrameworkElement const&)> const& predicate) {
    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        if (predicate(child)) {
            return child;
        }

        FrameworkElement found = FindChildByPredicate(child, predicate);
        if (found) {
            return found;
        }
    }

    return nullptr;
}

bool IsHoverFlyoutBackgroundElement(FrameworkElement const& element) {
    try {
        auto name = element.Name();
        std::wstring_view nameView(name.c_str(), name.size());
        return nameView.find(L"HoverFlyoutBackground") !=
               std::wstring_view::npos;
    } catch (...) {
        return false;
    }
}

void HideHoverFlyoutBackgroundVisual(FrameworkElement const& element) {
    std::lock_guard<std::mutex> lock(g_hoverFlyoutMutex);
    for (auto const& saved : g_savedHoverFlyoutVisuals) {
        if (saved.element.get() == element) {
            // A hover transition can repaint the visual without changing the
            // XAML layout. Reassert on every scan so it cannot flash back.
            try {
                ElementCompositionPreview::GetElementVisual(element)
                    .IsVisible(false);
                element.Opacity(0.0);
                element.IsHitTestVisible(false);
                element.Visibility(Visibility::Collapsed);
            } catch (...) {
            }
            return;
        }
    }

    SavedHoverFlyoutVisual saved;
    saved.element = winrt::make_weak(element);
    try {
        saved.opacity = element.Opacity();
        saved.isHitTestVisible = element.IsHitTestVisible();
        saved.visibility = element.Visibility();
        saved.compositionVisible =
            ElementCompositionPreview::GetElementVisual(element).IsVisible();
    } catch (...) {
        return;
    }

    // The system fades this layer in with a visual-state animation. Keep the
    // opacity pinned while leaving the element in the visual tree, so the
    // flyout's anchor/layout behavior is unchanged.
    saved.opacityToken = element.RegisterPropertyChangedCallback(
        UIElement::OpacityProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            if (g_unloading) {
                return;
            }
            auto element = sender.try_as<FrameworkElement>();
            if (element && element.Opacity() != 0.0) {
                element.Opacity(0.0);
            }
        });
    saved.visibilityToken = element.RegisterPropertyChangedCallback(
        UIElement::VisibilityProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            if (g_unloading) {
                return;
            }
            auto element = sender.try_as<FrameworkElement>();
            if (element && element.Visibility() != Visibility::Collapsed) {
                element.Visibility(Visibility::Collapsed);
            }
        });
    try {
        // The system's hover state storyboards animate BackgroundElement
        // opacity and outrank local XAML values, so hide the layer on its
        // composition visual as well.
        ElementCompositionPreview::GetElementVisual(element).IsVisible(false);
        element.Opacity(0.0);
        element.IsHitTestVisible(false);
        element.Visibility(Visibility::Collapsed);
    } catch (...) {
        return;
    }

    static std::atomic<int> s_logs{0};
    if (s_logs.fetch_add(1) < 12) {
        try {
            WCHAR diag[600];
            auto className = winrt::get_class_name(element);
            auto elementName = element.Name();
            swprintf_s(diag,
                       L"hoverflyout hide class=%s name=%s w=%.1f h=%.1f "
                       L"opacity=%.2f",
                       className.c_str(), elementName.c_str(),
                       element.ActualWidth(), element.ActualHeight(),
                       saved.opacity);
            AppendTrayLayoutDiag(diag);
        } catch (...) {
        }
    }

    g_savedHoverFlyoutVisuals.push_back(std::move(saved));
}

void SuppressHoverFlyoutBackgroundSubtree(FrameworkElement const& root,
                                          int depth = 0) {
    if (depth > 40) {
        return;
    }

    if (IsHoverFlyoutBackgroundElement(root)) {
        HideHoverFlyoutBackgroundVisual(root);
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }
        if (child) {
            SuppressHoverFlyoutBackgroundSubtree(child, depth + 1);
        }
    }
}

void SuppressHoverFlyoutBackgrounds(FrameworkElement const& root) {
    if (g_unloading || !root) {
        return;
    }

    SuppressHoverFlyoutBackgroundSubtree(root);

    // The hover flyout can be hosted in an open Popup rather than as a direct
    // descendant of the XamlRoot content.
    try {
        auto xamlRoot = root.XamlRoot();
        if (xamlRoot) {
            for (auto const& popup :
                 VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot)) {
                auto child = popup.Child().try_as<FrameworkElement>();
                if (child) {
                    SuppressHoverFlyoutBackgroundSubtree(child);
                }
            }
        }
    } catch (...) {
    }
}

void RestoreHoverFlyoutSuppression() {
    std::vector<SavedHoverFlyoutVisual> savedVisuals;
    winrt::weak_ref<FrameworkElement> hookRoot;
    winrt::event_token layoutToken{};
    bool hookInstalled = false;

    {
        std::lock_guard<std::mutex> lock(g_hoverFlyoutMutex);
        savedVisuals.swap(g_savedHoverFlyoutVisuals);
        hookRoot = g_hoverFlyoutRoot;
        layoutToken = g_hoverFlyoutLayoutToken;
        hookInstalled = g_hoverFlyoutHookInstalled;
        g_hoverFlyoutRoot = nullptr;
        g_hoverFlyoutLayoutToken = {};
        g_hoverFlyoutHookInstalled = false;
    }

    if (hookInstalled) {
        if (auto root = hookRoot.get()) {
            try {
                root.LayoutUpdated(layoutToken);
            } catch (...) {
            }
        }
    }

    for (auto const& saved : savedVisuals) {
        if (auto element = saved.element.get()) {
            try {
                if (saved.opacityToken) {
                    element.UnregisterPropertyChangedCallback(
                        UIElement::OpacityProperty(), saved.opacityToken);
                }
                if (saved.visibilityToken) {
                    element.UnregisterPropertyChangedCallback(
                        UIElement::VisibilityProperty(),
                        saved.visibilityToken);
                }
                ElementCompositionPreview::GetElementVisual(element)
                    .IsVisible(saved.compositionVisible);
                element.Opacity(saved.opacity);
                element.IsHitTestVisible(saved.isHitTestVisible);
                element.Visibility(saved.visibility);
            } catch (...) {
            }
        }
    }
}

void HideMultiWindowHighlight(FrameworkElement const& element) {
    std::lock_guard<std::mutex> lock(g_multiWindowHighlightMutex);
    for (auto const& saved : g_savedMultiWindowHighlights) {
        if (saved.element.get() == element) {
            try {
                ElementCompositionPreview::GetElementVisual(element)
                    .IsVisible(false);
                element.Opacity(0.0);
                element.Visibility(Visibility::Collapsed);
            } catch (...) {
            }
            return;
        }
    }

    SavedMultiWindowHighlight saved;
    saved.element = winrt::make_weak(element);
    try {
        saved.opacity = element.Opacity();
        saved.visibility = element.Visibility();
        saved.compositionVisible =
            ElementCompositionPreview::GetElementVisual(element).IsVisible();
    } catch (...) {
        return;
    }

    saved.opacityToken = element.RegisterPropertyChangedCallback(
        UIElement::OpacityProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            if (g_unloading) {
                return;
            }
            auto element = sender.try_as<FrameworkElement>();
            if (element && element.Opacity() != 0.0) {
                element.Opacity(0.0);
            }
        });
    saved.visibilityToken = element.RegisterPropertyChangedCallback(
        UIElement::VisibilityProperty(),
        [](DependencyObject const& sender, DependencyProperty const&) {
            if (g_unloading) {
                return;
            }
            auto element = sender.try_as<FrameworkElement>();
            if (element && element.Visibility() != Visibility::Collapsed) {
                element.Visibility(Visibility::Collapsed);
            }
        });

    try {
        ElementCompositionPreview::GetElementVisual(element).IsVisible(false);
        element.Opacity(0.0);
        element.Visibility(Visibility::Collapsed);
    } catch (...) {
        return;
    }

    g_savedMultiWindowHighlights.push_back(std::move(saved));
}

void HideMultiWindowHighlightsRecursive(FrameworkElement const& root,
                                        int depth) {
    if (!root || depth > 40) {
        return;
    }

    if (ElementNameEquals(root, L"MultiWindowElement")) {
        HideMultiWindowHighlight(root);
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }
    for (int i = 0; i < count; i++) {
        try {
            auto child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
            if (child) {
                HideMultiWindowHighlightsRecursive(child, depth + 1);
            }
        } catch (...) {
        }
    }
}

void HideMultiWindowHighlights(FrameworkElement const& root) {
    if (g_unloading || !root) {
        return;
    }
    HideMultiWindowHighlightsRecursive(root, 0);
}

void RestoreMultiWindowHighlights() {
    std::vector<SavedMultiWindowHighlight> savedHighlights;
    {
        std::lock_guard<std::mutex> lock(g_multiWindowHighlightMutex);
        savedHighlights.swap(g_savedMultiWindowHighlights);
    }

    for (auto const& saved : savedHighlights) {
        if (auto element = saved.element.get()) {
            try {
                if (saved.opacityToken) {
                    element.UnregisterPropertyChangedCallback(
                        UIElement::OpacityProperty(), saved.opacityToken);
                }
                if (saved.visibilityToken) {
                    element.UnregisterPropertyChangedCallback(
                        UIElement::VisibilityProperty(),
                        saved.visibilityToken);
                }
                ElementCompositionPreview::GetElementVisual(element)
                    .IsVisible(saved.compositionVisible);
                element.Opacity(saved.opacity);
                element.Visibility(saved.visibility);
            } catch (...) {
            }
        }
    }
}

bool IsHoverFlyoutSuppressionInstalled() {
    std::lock_guard<std::mutex> lock(g_hoverFlyoutMutex);
    return g_hoverFlyoutHookInstalled;
}

void DumpHoverState(FrameworkElement const& root,
                    FrameworkElement const& tray);
void NormalizeTaskButtonClips(FrameworkElement const& root);
void NormalizeTaskButtonStackEffects(FrameworkElement const& root);
void HideNarrowTaskbarOverflowLayers(FrameworkElement const& root);
void AppendTaskButtonStackDiag(PCWSTR text);

void EnsureHoverFlyoutSuppression(FrameworkElement const& frame) {
    if (g_unloading || g_settingTaskbarAreaHeight.load() <= 0 || !frame) {
        return;
    }

    FrameworkElement root = nullptr;
    try {
        if (auto xamlRoot = frame.XamlRoot()) {
            root = xamlRoot.Content().try_as<FrameworkElement>();
        }
    } catch (...) {
    }
    if (!root) {
        return;
    }

    bool installHook = false;
    {
        std::lock_guard<std::mutex> lock(g_hoverFlyoutMutex);
        if (g_hoverFlyoutRoot.get() != root) {
            if (g_hoverFlyoutHookInstalled) {
                if (auto oldRoot = g_hoverFlyoutRoot.get()) {
                    try {
                        oldRoot.LayoutUpdated(g_hoverFlyoutLayoutToken);
                    } catch (...) {
                    }
                }
            }
            g_hoverFlyoutRoot = winrt::make_weak(root);
            g_hoverFlyoutLayoutToken = {};
            g_hoverFlyoutHookInstalled = false;
            installHook = true;
        }
    }

    if (installHook) {
        auto token = root.LayoutUpdated(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               winrt::Windows::Foundation::IInspectable const&) {
                if (g_unloading) {
                    return;
                }
                if (auto root = sender.try_as<FrameworkElement>()) {
                    SuppressHoverFlyoutBackgrounds(root);
                    NormalizeTaskButtonStackEffects(root);
                    DWORD enterTick = g_hoverPointerEnterTick.load();
                    if (g_hoverStateDumpRequested.load() &&
                        GetTickCount() - enterTick >= 120 &&
                        g_hoverStateDumpRequested.exchange(false)) {
                        if (auto tray = g_hoverDumpTray.get()) {
                            try {
                                DumpHoverState(root, tray);
                            } catch (...) {
                            }
                        }
                    }
                }
            });

        std::lock_guard<std::mutex> lock(g_hoverFlyoutMutex);
        g_hoverFlyoutLayoutToken = token;
        g_hoverFlyoutHookInstalled = true;
    }

    SuppressHoverFlyoutBackgrounds(root);
}

void CollectBackgroundRects(FrameworkElement const& root,
                            std::vector<FrameworkElement>& result) {
    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        auto className = winrt::get_class_name(child);
        if (ClassNameEquals(className, L"Taskbar.TaskbarBackground") &&
            ElementNameEquals(child, L"HoverFlyoutBackgroundControl")) {
            continue;
        }

        if (ClassNameEndsWith(className, L"Rectangle") &&
            (ElementNameEquals(child, L"BackgroundFill") ||
             ElementNameEquals(child, L"BackgroundStroke"))) {
            result.push_back(child);
        }

        CollectBackgroundRects(child, result);
    }
}

Grid FindRootGrid(FrameworkElement const& frame) {
    FrameworkElement rootElement = FindChildByPredicate(
        frame, [](FrameworkElement const& element) {
            return ElementNameEquals(element, L"RootGrid");
        });
    return rootElement ? rootElement.try_as<Grid>() : nullptr;
}

FrameworkElement FindTaskbarIconRepeater(FrameworkElement const& frame) {
    return FindChildByPredicate(frame, [](FrameworkElement const& element) {
        return ElementNameEquals(element, L"TaskbarFrameRepeater");
    });
}

// Returns true if the system taskbar alignment is set to "Center".
bool IsTaskbarCenterAligned() {
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
                      L"Advanced",
                      0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    DWORD value = 0;
    DWORD size = sizeof(value);
    DWORD type = 0;
    LONG result = RegQueryValueExW(hKey, L"TaskbarAl", nullptr, &type,
                                   (LPBYTE)&value, &size);
    RegCloseKey(hKey);

    return result == ERROR_SUCCESS && value != 0;
}

double GetTaskbarIconsLeftInsetDip() {
    return IsTaskbarCenterAligned()
               ? kTaskbarIconsLeftInsetDip
               : -kTaskbarIconsLeftModeCorrectionDip;
}

Brush FindOriginalBackgroundBrush(
    std::vector<FrameworkElement> const& backgroundRects) {
    for (auto const& element : backgroundRects) {
        if (ElementNameEquals(element, L"BackgroundFill")) {
            if (auto rect = element.try_as<Shapes::Rectangle>()) {
                return rect.Fill();
            }
        }
    }

    return nullptr;
}

int HexDigitValue(wchar_t ch) {
    if (ch >= L'0' && ch <= L'9') {
        return ch - L'0';
    }
    if (ch >= L'a' && ch <= L'f') {
        return ch - L'a' + 10;
    }
    if (ch >= L'A' && ch <= L'F') {
        return ch - L'A' + 10;
    }
    return -1;
}

bool ParseHexColor(std::wstring_view value,
                   unsigned& rgb,
                   unsigned& alpha,
                   bool& hasAlpha) {
    if (!value.empty() && value.front() == L'#') {
        value.remove_prefix(1);
    }
    if (value.size() != 6 && value.size() != 8) {
        return false;
    }

    unsigned parsed = 0;
    for (wchar_t ch : value) {
        int digit = HexDigitValue(ch);
        if (digit < 0) {
            return false;
        }
        parsed = (parsed << 4) | static_cast<unsigned>(digit);
    }

    if (value.size() == 6) {
        rgb = parsed;
        alpha = 0xFF;
        hasAlpha = false;
    } else {
        alpha = (parsed >> 24) & 0xFF;
        rgb = parsed & 0xFFFFFF;
        hasAlpha = true;
    }
    return true;
}

struct ParsedBackgroundColor {
    unsigned rgb = 0x202020;
    unsigned alpha = 0xFF;
    bool hasAlpha = false;
};

ParsedBackgroundColor ParseBackgroundColor(std::wstring_view value,
                                           std::wstring_view fallback) {
    ParsedBackgroundColor result;
    unsigned alpha = 0xFF;
    bool hasAlpha = false;
    if (ParseHexColor(value, result.rgb, alpha, hasAlpha)) {
        result.alpha = alpha;
        result.hasAlpha = hasAlpha;
        return result;
    }
    if (ParseHexColor(fallback, result.rgb, alpha, hasAlpha)) {
        result.alpha = alpha;
        result.hasAlpha = hasAlpha;
        return result;
    }
    return result;
}

winrt::Windows::UI::Color MakeColor(unsigned rgb, unsigned alpha) {
    return winrt::Windows::UI::Color{
        static_cast<uint8_t>(alpha & 0xFF),
        static_cast<uint8_t>((rgb >> 16) & 0xFF),
        static_cast<uint8_t>((rgb >> 8) & 0xFF),
        static_cast<uint8_t>(rgb & 0xFF)};
}

unsigned AlphaFromOpacity(int opacityPercent) {
    return static_cast<unsigned>(
        std::lround(std::clamp(opacityPercent, 0, 100) * 255.0 / 100.0));
}

std::wstring EnsureXamlNamespace(std::wstring xaml) {
    size_t firstTag = xaml.find(L'<');
    if (firstTag == std::wstring::npos) {
        return xaml;
    }

    size_t firstTagClose = xaml.find(L'>', firstTag);
    if (firstTagClose == std::wstring::npos) {
        return xaml;
    }

    size_t xmlnsPos = xaml.find(L"xmlns", firstTag);
    if (xmlnsPos != std::wstring::npos && xmlnsPos < firstTagClose) {
        return xaml;
    }

    size_t insertPos = xaml.find_first_of(L" \t\r\n/>", firstTag + 1);
    if (insertPos == std::wstring::npos || insertPos > firstTagClose) {
        insertPos = firstTagClose;
    }
    xaml.insert(insertPos,
                L" xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
                L"presentation\"");
    return xaml;
}

Brush CreateBackgroundBrush(BackgroundSettings const& settings) {
    try {
        switch (settings.style) {
        case BackgroundStyle::custom: {
            if (settings.customXaml.empty()) {
                return nullptr;
            }
            try {
                return XamlReader::Load(winrt::hstring(
                                            EnsureXamlNamespace(
                                                settings.customXaml)))
                    .try_as<Brush>();
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Failed to parse custom background brush: %08X",
                       static_cast<unsigned int>(ex.code()));
                return nullptr;
            }
        }

        case BackgroundStyle::system:
            return nullptr;

        case BackgroundStyle::transparent: {
            SolidColorBrush brush;
            brush.Color(MakeColor(0, 0));
            return brush;
        }

        case BackgroundStyle::solid: {
            ParsedBackgroundColor color =
                ParseBackgroundColor(settings.color, L"#202020");
            SolidColorBrush brush;
            brush.Color(MakeColor(
                color.rgb, color.hasAlpha ? color.alpha : 0xFF));
            return brush;
        }

        case BackgroundStyle::translucent: {
            ParsedBackgroundColor color =
                ParseBackgroundColor(settings.color, L"#202020");
            SolidColorBrush brush;
            brush.Color(MakeColor(color.rgb,
                                  AlphaFromOpacity(settings.opacity)));
            return brush;
        }

        case BackgroundStyle::gradient: {
            ParsedBackgroundColor color1 =
                ParseBackgroundColor(settings.gradientColor1, L"#1F2937");
            ParsedBackgroundColor color2 =
                ParseBackgroundColor(settings.gradientColor2, L"#111827");
            unsigned alpha = AlphaFromOpacity(settings.opacity);

            LinearGradientBrush brush;
            GradientStop stop1;
            stop1.Color(MakeColor(color1.rgb, alpha));
            stop1.Offset(
                std::clamp(settings.gradientStartPosition, 0, 100) / 100.0);
            GradientStop stop2;
            stop2.Color(MakeColor(color2.rgb, alpha));
            stop2.Offset(
                std::clamp(settings.gradientEndPosition, 0, 100) / 100.0);
            brush.GradientStops().Append(stop1);
            brush.GradientStops().Append(stop2);

            double angle =
                settings.gradientAngle * 3.14159265358979323846 / 180.0;
            double dx = std::cos(angle);
            double dy = std::sin(angle);
            brush.StartPoint(winrt::Windows::Foundation::Point{
                static_cast<float>(0.5 - dx / 2),
                static_cast<float>(0.5 - dy / 2)});
            brush.EndPoint(winrt::Windows::Foundation::Point{
                static_cast<float>(0.5 + dx / 2),
                static_cast<float>(0.5 + dy / 2)});
            return brush;
        }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to create background brush (style=%d): %08X",
               static_cast<int>(settings.style),
               static_cast<unsigned int>(ex.code()));
        return nullptr;
    }

    return nullptr;
}

struct TaskbarWindowSearchContext {
    DWORD threadId;
    HWND result;
};

BOOL CALLBACK FindTaskbarWindowEnumProc(HWND hWnd, LPARAM lParam) {
    auto context = reinterpret_cast<TaskbarWindowSearchContext*>(lParam);

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") != 0 &&
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
        return TRUE;
    }

    if (GetWindowThreadProcessId(hWnd, nullptr) != context->threadId) {
        return TRUE;
    }

    context->result = hWnd;
    return FALSE;
}

HWND FindTaskbarWindowForCurrentThread() {
    TaskbarWindowSearchContext context{GetCurrentThreadId(), nullptr};
    EnumWindows(FindTaskbarWindowEnumProc, reinterpret_cast<LPARAM>(&context));
    return context.result;
}

HWND GetTaskbarWindowForCurrentThread() {
    if (g_cachedTaskbarWnd && IsWindow(g_cachedTaskbarWnd)) {
        return g_cachedTaskbarWnd;
    }

    if (g_cachedTaskbarWndSearched) {
        return nullptr;
    }

    g_cachedTaskbarWndSearched = true;
    g_cachedTaskbarWnd = FindTaskbarWindowForCurrentThread();
    return g_cachedTaskbarWnd;
}

double GetTaskbarWindowScale(HWND taskbarWnd) {
    if (taskbarWnd) {
        UINT dpi = GetDpiForWindow(taskbarWnd);
        if (dpi) {
            return static_cast<double>(dpi) / 96.0;
        }
    }

    return 0.0;
}

double GetWindowWidthDip(HWND taskbarWnd, double scale) {
    if (!taskbarWnd || scale <= 0) {
        return 0.0;
    }

    RECT rect{};
    if (!GetWindowRect(taskbarWnd, &rect)) {
        return 0.0;
    }

    double width = (rect.right - rect.left) / scale;
    return width > 0 ? width : 0.0;
}

double GetWindowHeightDip(HWND taskbarWnd, double scale) {
    if (!taskbarWnd || scale <= 0) {
        return 0.0;
    }

    RECT rect{};
    if (!GetWindowRect(taskbarWnd, &rect)) {
        return 0.0;
    }

    double height = (rect.bottom - rect.top) / scale;
    return height > 0 ? height : 0.0;
}

double GetTaskbarScale(FrameworkElement const& frame) {
    try {
        if (auto xamlRoot = frame.XamlRoot()) {
            double rasterizationScale = xamlRoot.RasterizationScale();
            if (rasterizationScale > 0) {
                return rasterizationScale;
            }
        }
    } catch (...) {
    }

    return 1.0;
}

double GetEffectiveScale(FrameworkElement const& frame, HWND taskbarWnd) {
    double scale = GetTaskbarWindowScale(taskbarWnd);
    if (scale <= 0) {
        scale = GetTaskbarScale(frame);
    }
    return scale;
}

double GetTaskbarAreaHeightDip(int areaHeightSetting,
                               FrameworkElement const& frame,
                               HWND taskbarWnd,
                               double scale) {
    if (areaHeightSetting > 0 && scale > 0) {
        return areaHeightSetting / scale;
    }

    try {
        double height = frame.ActualHeight();
        if (height > 0) {
            return height;
        }
    } catch (...) {
    }

    return GetWindowHeightDip(taskbarWnd, scale);
}

double GetTaskbarWidthDip(int taskbarWidthSetting,
                          FrameworkElement const& frame,
                          HWND taskbarWnd,
                          double scale) {
    if (taskbarWidthSetting <= 0) {
        return 0.0;
    }

    double width = taskbarWidthSetting / scale;

    double windowWidth = GetWindowWidthDip(taskbarWnd, scale);
    if (windowWidth <= 0) {
        try {
            if (auto xamlRoot = frame.XamlRoot()) {
                windowWidth = xamlRoot.Size().Width;
            }
        } catch (...) {
        }
    }

    if (windowWidth > 0 && width > windowWidth) {
        width = windowWidth;
    }

    return width;
}

double GetFloatMarginDip(int floatMarginSetting, double scale) {
    if (scale <= 0) {
        return 0.0;
    }

    return floatMarginSetting / scale;
}

double GetFrameHeightDip(int areaHeightSetting, HWND taskbarWnd, double scale) {
    if (areaHeightSetting <= 0 || scale <= 0) {
        return 0.0;
    }

    return areaHeightSetting / scale;
}

double GetBarHeightDip(double areaHeight,
                       double windowHeight,
                       double floatMargin) {
    if (areaHeight <= 0) {
        return 0.0;
    }

    double visibleHeight = areaHeight;
    if (windowHeight > 0 && windowHeight < visibleHeight) {
        visibleHeight = windowHeight;
    }

    double barHeight = visibleHeight - floatMargin;
    if (barHeight < 1) {
        barHeight = 1;
    }

    return barHeight;
}

double GetTrayGapDip(int taskbarWidthSetting,
                     FrameworkElement const& frame,
                     FrameworkElement const& trayElement,
                     HWND taskbarWnd,
                     double scale) {
    double windowWidth = GetWindowWidthDip(taskbarWnd, scale);

    double pillWidth = 0.0;
    if (taskbarWidthSetting > 0) {
        pillWidth = GetTaskbarWidthDip(taskbarWidthSetting, frame, taskbarWnd,
                                       scale);
    } else {
        try {
            pillWidth = frame.ActualWidth();
        } catch (...) {
        }
    }

    if (windowWidth <= 0 || pillWidth <= 0 || windowWidth <= pillWidth) {
        return 0.0;
    }

    double gap = (windowWidth - pillWidth) / 2.0;

    double trayWidth = 0.0;
    if (trayElement) {
        try {
            trayWidth = trayElement.ActualWidth();
        } catch (...) {
        }
    }

    if (trayWidth > 0 && gap > pillWidth - trayWidth) {
        gap = pillWidth - trayWidth;
    }

    return gap > 0 ? gap : 0.0;
}

FrameworkElement FindSiblingTray(FrameworkElement const& frame) {
    FrameworkElement parent = nullptr;
    try {
        parent = VisualTreeHelper::GetParent(frame).try_as<FrameworkElement>();
    } catch (...) {
        return nullptr;
    }

    if (!parent) {
        return nullptr;
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(parent);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < count; i++) {
        FrameworkElement child = nullptr;
        try {
            child =
                VisualTreeHelper::GetChild(parent, i).try_as<FrameworkElement>();
        } catch (...) {
        }

        if (!child) {
            continue;
        }

        try {
            if (ClassNameEquals(winrt::get_class_name(child),
                                L"SystemTray.SystemTrayFrame")) {
                return child;
            }
        } catch (...) {
        }
    }

    return nullptr;
}

// While a custom taskbar area height is in use, the taskbar frame gets an
// explicit height and its parent has to be stretched to that height as well,
// otherwise explorer's own layout updates squeeze the frame back. The original
// parent height is remembered so that it can be restored later on.
void SetParentHeightForFrame(FrameworkElement const& frame,
                             double heightDip,
                             AppliedElement& record) {
    if (!(heightDip > 0) || std::isnan(heightDip)) {
        return;
    }

    try {
        FrameworkElement parent =
            VisualTreeHelper::GetParent(frame).try_as<FrameworkElement>();
        if (!parent) {
            return;
        }

        if (!record.parentHeightSet) {
            record.savedParentHeight = parent.Height();
            record.frameParent = winrt::make_weak(parent);
            record.parentHeightSet = true;
        }

        parent.Height(heightDip);
    } catch (...) {
    }
}

// A blur-only acrylic base. The gradient keeps its own colors and opacity and
// is painted on the existing BackgroundFill rectangle above this brush. The
// blur removes hard wallpaper edges (a high-contrast ridge can otherwise look
// like a vertical seam through a translucent gradient) without adding another
// visual element or changing the icon/hover layers.
Brush CreateBackdropBlurBrush() {
    try {
        AcrylicBrush brush;
        brush.TintColor(winrt::Windows::UI::Color{0, 0, 0, 0});
        brush.TintOpacity(0.0);
        brush.TintLuminosityOpacity(nullptr);
        brush.FallbackColor(winrt::Windows::UI::Color{0, 0, 0, 0});
        return brush;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to create backdrop blur brush: %08X",
               static_cast<unsigned int>(ex.code()));
        return nullptr;
    }
}

void ReassertParentHeight(AppliedElement const& record, double heightDip) {
    if (!record.parentHeightSet || !(heightDip > 0) ||
        std::isnan(heightDip)) {
        return;
    }

    if (auto parent = record.frameParent.get()) {
        try {
            parent.Height(heightDip);
        } catch (...) {
        }
    }
}

HWND FindIslandWindow(HWND taskbarWnd) {
    if (!taskbarWnd) {
        return nullptr;
    }

    return FindWindowExW(
        taskbarWnd, nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
}

struct FindTaskbarWindowContext {
    HMONITOR monitor;
    HWND result;
};

BOOL CALLBACK FindTaskbarWindowOnMonitorEnumProc(HWND hWnd, LPARAM lParam) {
    auto context = reinterpret_cast<FindTaskbarWindowContext*>(lParam);

    DWORD dwProcessId = 0;
    WCHAR className[64];
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId() ||
        !GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") != 0 &&
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
        return TRUE;
    }

    if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL) != context->monitor) {
        return TRUE;
    }

    context->result = hWnd;
    return FALSE;
}

HWND FindTaskbarWindowOnMonitor(HMONITOR monitor) {
    FindTaskbarWindowContext context{monitor, nullptr};
    EnumWindows(FindTaskbarWindowOnMonitorEnumProc,
                reinterpret_cast<LPARAM>(&context));

    if (!context.result) {
        context.result = FindWindowW(L"Shell_TrayWnd", nullptr);
    }

    return context.result;
}

void AppendControlCenterDiag(const wchar_t* text) {
    if (g_controlCenterDiagWrites.load() >= 40) {
        return;
    }

    g_controlCenterDiagWrites.fetch_add(1);

    WCHAR path[MAX_PATH];
    DWORD length = GetTempPathW(ARRAYSIZE(path), path);
    if (!length || length + 40 >= ARRAYSIZE(path)) {
        return;
    }

    wcscat_s(path, L"floating-taskbar-quicksettings.txt");

    HANDLE file = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    SYSTEMTIME st{};
    GetLocalTime(&st);

    WCHAR line[600];
    int count = swprintf_s(line, L"[%02u:%02u:%02u] %s\n", st.wHour, st.wMinute,
                           st.wSecond, text);
    if (count > 0) {
        DWORD written = 0;
        WriteFile(file, line, static_cast<DWORD>(count * sizeof(WCHAR)),
                  &written, nullptr);
    }

    CloseHandle(file);
}

void RepositionControlCenterWindow(HWND ccWnd, const wchar_t* reason) {
    if (g_unloading || !ccWnd || !IsWindow(ccWnd)) {
        return;
    }

    if (!g_settingFloatingBarEnabled.load()) {
        // Drop the target position so that the panel host stops applying it.
        RemovePropW(ccWnd, kControlCenterTargetRightProp);
        return;
    }

    int taskbarWidth = g_settingTaskbarWidth.load();

    HWND taskbarWnd = FindTaskbarWindowOnMonitor(
        MonitorFromWindow(ccWnd, MONITOR_DEFAULTTONEAREST));
    if (!taskbarWnd) {
        return;
    }

    RECT taskbarRect{};
    RECT ccRect{};
    if (!GetWindowRect(taskbarWnd, &taskbarRect) ||
        !GetWindowRect(ccWnd, &ccRect)) {
        return;
    }

    double scale = GetTaskbarWindowScale(taskbarWnd);
    if (scale <= 0) {
        return;
    }

    double ccWidth = static_cast<double>(ccRect.right - ccRect.left);

    double windowWidthPx =
        static_cast<double>(taskbarRect.right - taskbarRect.left);
    double pillWidthPx = taskbarWidth > 0 ? static_cast<double>(taskbarWidth)
                                          : g_lastPillWidthPx.load();
    if (pillWidthPx <= 0) {
        return;
    }
    if (pillWidthPx > windowWidthPx) {
        pillWidthPx = windowWidthPx;
    }

    double barRightPx =
        static_cast<double>(taskbarRect.left) +
        (windowWidthPx + pillWidthPx) / 2.0;
    long targetRightPx =
        std::lround(barRightPx + std::lround(kControlCenterRightInsetDip *
                                             scale));

    // Publish the target position for the process which hosts the panel, which
    // applies it while the window is being moved. This way, the panel is in the
    // right place as soon as it appears instead of jumping there later on.
    SetPropW(ccWnd, kControlCenterTargetRightProp,
             (HANDLE)(ULONG_PTR)targetRightPx);

    if (ccWidth <= 0) {
        return;
    }

    long newLeft = std::lround(targetRightPx - ccWidth);

    if (ccRect.left == newLeft) {
        return;
    }

    if (SetWindowPos(ccWnd, nullptr, newLeft, ccRect.top, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE)) {
        WCHAR text[400];
        swprintf_s(text,
                   L"%s corrected hwnd=%p from=%ld to=%ld width=%ld "
                   L"pill=%.0f inset=%ld scale=%.2f",
                   reason ? reason : L"?", ccWnd, ccRect.left, newLeft,
                   ccRect.right - ccRect.left, pillWidthPx,
                   std::lround(kControlCenterRightInsetDip * scale), scale);
        AppendControlCenterDiag(text);
    }
}

BOOL CALLBACK RepositionControlCenterEnumProc(HWND hWnd, LPARAM lParam) {
    WCHAR className[64];
    if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"ControlCenterWindow") == 0) {
        RepositionControlCenterWindow(
            hWnd, reinterpret_cast<const wchar_t*>(lParam));
    }

    return TRUE;
}

bool IsControlCenterWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"ControlCenterWindow") == 0;
}

void RefreshControlCenterWindow() {
    if (IsControlCenterWindow(g_controlCenterWnd.load())) {
        return;
    }

    g_controlCenterWnd.store(nullptr);

    HWND found = FindWindowW(L"ControlCenterWindow", nullptr);
    if (IsControlCenterWindow(found)) {
        g_controlCenterWnd.store(found);
    }
}

void RepositionControlCenterCached(const wchar_t* reason) {
    if (g_unloading) {
        return;
    }

    g_lastControlCenterFixTick = GetTickCount();
    RefreshControlCenterWindow();

    if (HWND ccWnd = g_controlCenterWnd.load()) {
        RepositionControlCenterWindow(ccWnd, reason);
    }
}

void RepositionControlCenterWindows(const wchar_t* reason) {
    if (g_unloading) {
        return;
    }

    g_lastControlCenterFixTick = GetTickCount();
    RefreshControlCenterWindow();
    EnumWindows(RepositionControlCenterEnumProc,
                reinterpret_cast<LPARAM>(reason));
}

void CALLBACK ControlCenterLocationChanged(HWINEVENTHOOK hook,
                                           DWORD event,
                                           HWND hWnd,
                                           LONG idObject,
                                           LONG idChild,
                                           DWORD threadId,
                                           DWORD time) {
    HookScope hookScope;

    if (g_unloading || idObject != OBJID_WINDOW || idChild != 0 || !hWnd) {
        return;
    }

    if (hWnd == g_controlCenterWnd.load()) {
        RepositionControlCenterWindow(hWnd, L"event");
        return;
    }

    if (GetTickCount() - g_lastControlCenterFixTick.load() > 600) {
        g_lastControlCenterFixTick = GetTickCount();
        RefreshControlCenterWindow();
        if (hWnd == g_controlCenterWnd.load()) {
            RepositionControlCenterWindow(hWnd, L"event-refresh");
        }
    }
}

void ApplyToAllTaskbarsNow();

std::atomic<bool> g_controlCenterWasVisible{false};

void ApplyWhenControlCenterShown() {
    HWND ccWnd = FindWindowW(L"ControlCenterWindow", nullptr);
    bool visible = ccWnd && IsWindowVisible(ccWnd);
    if (visible) {
        if (!g_controlCenterWasVisible.exchange(true)) {
            // Quick Settings can create or repaint the taskbar's
            // HoverFlyoutBackgroundControl after the startup apply. Refresh
            // the taskbar once on the visible transition so that material is
            // cleared before the flyout settles.
            ApplyToAllTaskbarsNow();
        }
    } else {
        g_controlCenterWasVisible = false;
    }
}

DWORD WINAPI ControlCenterWatchdogProc(LPVOID param) {
    while (!g_controlCenterWatchdogStop.load()) {
        if (!g_unloading) {
            RepositionControlCenterWindows(L"watch");
            ApplyWhenControlCenterShown();
        }

        for (int i = 0; i < 15 && !g_controlCenterWatchdogStop.load(); i++) {
            Sleep(100);
            if (!g_unloading) {
                ApplyWhenControlCenterShown();
            }
        }
    }

    return 0;
}

void StartControlCenterWatchdog() {
    if (g_controlCenterWatchdogThread) {
        return;
    }

    g_controlCenterWatchdogStop = false;
    g_controlCenterWatchdogThread = CreateThread(
        nullptr, 0, ControlCenterWatchdogProc, nullptr, 0, nullptr);
}

void StopControlCenterWatchdog() {
    g_controlCenterWatchdogStop = true;

    if (g_controlCenterWatchdogThread) {
        WaitForSingleObject(g_controlCenterWatchdogThread, 3000);
        CloseHandle(g_controlCenterWatchdogThread);
        g_controlCenterWatchdogThread = nullptr;
    }
}

void InstallControlCenterHookOnCurrentThread(PVOID) {
    if (g_controlCenterWinEventHook) {
        return;
    }

    g_controlCenterWinEventHook =
        SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE,
                        EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                        ControlCenterLocationChanged, 0, 0,
                        WINEVENT_OUTOFCONTEXT);
}

void RemoveControlCenterHook() {
    if (g_controlCenterWinEventHook) {
        UnhookWinEvent(g_controlCenterWinEventHook);
        g_controlCenterWinEventHook = nullptr;
    }

    g_controlCenterWnd.store(nullptr);
    g_lastPillWidthPx.store(0.0);
}

bool GetElementRectDip(FrameworkElement const& element,
                       UIElement const& contentElement,
                       winrt::Windows::Foundation::Rect& rect) {
    if (!element || !contentElement) {
        return false;
    }

    try {
        double width = element.ActualWidth();
        double height = element.ActualHeight();
        if (width <= 0 || height <= 0) {
            return false;
        }

        rect = element.TransformToVisual(contentElement)
                   .TransformBounds({0.0f, 0.0f, static_cast<float>(width),
                                     static_cast<float>(height)});
        return rect.Width > 0 && rect.Height > 0;
    } catch (...) {
    }

    return false;
}

void SaveAndApplyTaskButtonClip(
    FrameworkElement const& element,
    winrt::Windows::Foundation::Rect const& clipRect) {
    if (!element || clipRect.Width <= 0 || clipRect.Height <= 0) {
        return;
    }

    RectangleGeometry geometry;
    geometry.Rect(clipRect);

    std::lock_guard<std::mutex> lock(g_taskButtonClipMutex);
    for (auto const& saved : g_savedTaskButtonClips) {
        if (saved.element.get() == element) {
            element.Clip(geometry);
            return;
        }
    }

    SavedTaskButtonClip saved;
    saved.element = winrt::make_weak(element);
    try {
        saved.originalClip = element.Clip();
        element.Clip(geometry);
    } catch (...) {
        return;
    }
    g_savedTaskButtonClips.push_back(std::move(saved));
}

void CollectTaskButtonBackgroundLayers(FrameworkElement const& root,
                                       std::vector<FrameworkElement>& result,
                                       int depth = 0) {
    if (!root || depth > 10) {
        return;
    }

    if (ElementNameEquals(root, L"BackgroundElement") ||
        ElementNameEquals(root, L"MultiWindowElement")) {
        result.push_back(root);
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }
    for (int i = 0; i < count; i++) {
        try {
            auto child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
            if (child) {
                CollectTaskButtonBackgroundLayers(child, result, depth + 1);
            }
        } catch (...) {
        }
    }
}

int GetTaskButtonWindowCount(FrameworkElement const& button) {
    try {
        auto automationName = AutomationProperties::GetName(button);
        std::wstring text(automationName.c_str(), automationName.size());
        size_t marker = text.find(L"个运行窗口");
        if (marker == std::wstring::npos) {
            marker = text.find(L"running window");
        }
        size_t end = marker == std::wstring::npos ? text.size() : marker;
        size_t pos = end;
        while (pos > 0 &&
               (text[pos - 1] == L' ' || text[pos - 1] == L'-' ||
                text[pos - 1] == L':')) {
            pos--;
        }
        size_t digitsEnd = pos;
        while (pos > 0 && text[pos - 1] >= L'0' && text[pos - 1] <= L'9') {
            pos--;
        }
        if (digitsEnd > pos) {
            return _wtoi(text.substr(pos, digitsEnd - pos).c_str());
        }
    } catch (...) {
    }
    return 0;
}

void ApplyTaskButtonClip(FrameworkElement const& button) {
    if (g_unloading || !button) {
        return;
    }

    std::vector<FrameworkElement> layers;
    CollectTaskButtonBackgroundLayers(button, layers);
    FrameworkElement primary = nullptr;
    winrt::Windows::Foundation::Rect primaryRect{};
    for (auto const& layer : layers) {
        winrt::Windows::Foundation::Rect rect{};
        if (!GetElementRectDip(layer, button, rect) || rect.Width < 20 ||
            rect.Height < 20) {
            continue;
        }
        if (!primary || rect.X < primaryRect.X) {
            primary = layer;
            primaryRect = rect;
        }
    }
    if (!primary) {
        return;
    }

    double buttonHeight = button.ActualHeight();
    if (!(buttonHeight > 0)) {
        return;
    }
    winrt::Windows::Foundation::Rect clipRect{
        std::max(0.0f, primaryRect.X), 0.0f, primaryRect.Width + 1.0f,
        static_cast<float>(buttonHeight)};
    SaveAndApplyTaskButtonClip(button, clipRect);

    FrameworkElement parent =
        VisualTreeHelper::GetParent(button).try_as<FrameworkElement>();
    if (parent &&
        ClassNameEquals(winrt::get_class_name(parent),
                        L"Taskbar.TaskListButtonPanel")) {
        winrt::Windows::Foundation::Rect parentRect{};
        if (GetElementRectDip(primary, parent, parentRect)) {
            winrt::Windows::Foundation::Rect parentClip{
                std::max(0.0f, parentRect.X), 0.0f,
                parentRect.Width + 1.0f,
                static_cast<float>(parent.ActualHeight())};
            SaveAndApplyTaskButtonClip(parent, parentClip);
        }
    }
}

void NormalizeTaskButtonClipsRecursive(FrameworkElement const& root,
                                       int depth) {
    if (!root || depth > 20) {
        return;
    }

    try {
        if (ClassNameEquals(winrt::get_class_name(root),
                            L"Taskbar.TaskListButton")) {
            ApplyTaskButtonClip(root);
        }
    } catch (...) {
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }
    for (int i = 0; i < count; i++) {
        try {
            auto child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
            if (child) {
                NormalizeTaskButtonClipsRecursive(child, depth + 1);
            }
        } catch (...) {
        }
    }
}

void NormalizeTaskButtonClips(FrameworkElement const& root) {
    if (g_unloading || !root) {
        return;
    }
    NormalizeTaskButtonClipsRecursive(root, 0);
}

void RestoreTaskButtonClips() {
    std::vector<SavedTaskButtonClip> savedClips;
    {
        std::lock_guard<std::mutex> lock(g_taskButtonClipMutex);
        savedClips.swap(g_savedTaskButtonClips);
    }
    for (auto const& saved : savedClips) {
        if (auto element = saved.element.get()) {
            try {
                element.Clip(saved.originalClip);
            } catch (...) {
            }
        }
    }
}

void ApplyTaskButtonStackEffect(FrameworkElement const& button) {
    if (g_unloading || !button) {
        return;
    }

    std::vector<FrameworkElement> layers;
    CollectTaskButtonBackgroundLayers(button, layers);
    int windowCount = GetTaskButtonWindowCount(button);
    FrameworkElement main = nullptr;
    FrameworkElement stack = nullptr;
    for (auto const& layer : layers) {
        if (ElementNameEquals(layer, L"MultiWindowElement")) {
            stack = layer;
        } else if (ElementNameEquals(layer, L"BackgroundElement")) {
            if (!main) {
                main = layer;
            } else if (!stack) {
                stack = layer;
            }
        }
    }
    if (!main || !stack || main == stack) {
        return;
    }

    double mainWidth = main.ActualWidth();
    double mainHeight = main.ActualHeight();
    if (!(mainWidth > 0) || !(mainHeight > 0)) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_taskButtonStackMutex);
    for (auto const& saved : g_savedTaskButtonStackStyles) {
        if (saved.element.get() == stack) {
            try {
                stack.Width(mainWidth);
                stack.Height(mainHeight);
                stack.HorizontalAlignment(HorizontalAlignment::Center);
                stack.VerticalAlignment(VerticalAlignment::Center);
                stack.Translation(
                    winrt::Windows::Foundation::Numerics::float3{5, -2, 0});
                stack.RenderTransform(nullptr);
                stack.IsHitTestVisible(false);
            } catch (...) {
            }
            return;
        }
    }

    SavedTaskButtonStackStyle saved;
    saved.element = winrt::make_weak(stack);
    try {
        saved.width = stack.Width();
        saved.height = stack.Height();
        saved.horizontalAlignment = stack.HorizontalAlignment();
        saved.verticalAlignment = stack.VerticalAlignment();
        saved.translation = stack.Translation();
        saved.renderTransform = stack.RenderTransform();
        saved.isHitTestVisible = stack.IsHitTestVisible();
        if (auto border = stack.try_as<Border>()) {
            saved.isBorder = true;
            saved.cornerRadius = border.CornerRadius();
            saved.background = border.Background();
        } else if (auto grid = stack.try_as<Grid>()) {
            saved.isPanel = true;
            saved.cornerRadius = grid.CornerRadius();
            saved.background = grid.Background();
        } else if (auto presenter = stack.try_as<ContentPresenter>()) {
            saved.isPresenter = true;
            saved.cornerRadius = presenter.CornerRadius();
            saved.background = presenter.Background();
        }
    } catch (...) {
        return;
    }

    try {
        stack.Width(mainWidth);
        stack.Height(mainHeight);
        stack.HorizontalAlignment(HorizontalAlignment::Center);
        stack.VerticalAlignment(VerticalAlignment::Center);
        stack.Translation(
            winrt::Windows::Foundation::Numerics::float3{5, -2, 0});
        stack.RenderTransform(nullptr);
        stack.IsHitTestVisible(false);

        CornerRadius mainRadius{};
        Brush mainBackground{nullptr};
        if (auto mainBorder = main.try_as<Border>()) {
            mainRadius = mainBorder.CornerRadius();
            mainBackground = mainBorder.Background();
        } else if (auto mainGrid = main.try_as<Grid>()) {
            mainRadius = mainGrid.CornerRadius();
            mainBackground = mainGrid.Background();
        } else if (auto mainPresenter = main.try_as<ContentPresenter>()) {
            mainRadius = mainPresenter.CornerRadius();
            mainBackground = mainPresenter.Background();
        }

        if (auto border = stack.try_as<Border>()) {
            border.CornerRadius(mainRadius);
            if (mainBackground) {
                border.Background(mainBackground);
            }
        } else if (auto grid = stack.try_as<Grid>()) {
            grid.CornerRadius(mainRadius);
            if (mainBackground) {
                grid.Background(mainBackground);
            }
        } else if (auto presenter = stack.try_as<ContentPresenter>()) {
            presenter.CornerRadius(mainRadius);
            if (mainBackground) {
                presenter.Background(mainBackground);
            }
        }
    } catch (...) {
        return;
    }

    g_savedTaskButtonStackStyles.push_back(std::move(saved));
}

constexpr double kMultiWindowBackplateSizeDip = 40.0;

CornerRadius GetTaskButtonMainCornerRadius(
    FrameworkElement const& main) {
    try {
        if (auto border = main.try_as<Border>()) {
            return border.CornerRadius();
        }
        if (auto grid = main.try_as<Grid>()) {
            return grid.CornerRadius();
        }
        if (auto presenter = main.try_as<ContentPresenter>()) {
            return presenter.CornerRadius();
        }
    } catch (...) {
    }
    return CornerRadius{};
}

void SetTaskButtonMainCornerRadius(FrameworkElement const& main,
                                   CornerRadius radius) {
    try {
        if (auto border = main.try_as<Border>()) {
            border.CornerRadius(radius);
        } else if (auto grid = main.try_as<Grid>()) {
            grid.CornerRadius(radius);
        } else if (auto presenter = main.try_as<ContentPresenter>()) {
            presenter.CornerRadius(radius);
        }
    } catch (...) {
    }
}

void HideTaskButtonNativeStack(FrameworkElement const& nativeStack) {
    if (!nativeStack) {
        return;
    }

    try {
        if (nativeStack.Visibility() != Visibility::Collapsed) {
            nativeStack.Visibility(Visibility::Collapsed);
        }
        if (nativeStack.Opacity() != 0.0) {
            nativeStack.Opacity(0.0);
        }
        auto visual =
            ElementCompositionPreview::GetElementVisual(nativeStack);
        if (visual.IsVisible()) {
            visual.IsVisible(false);
        }
    } catch (...) {
    }
}

void ApplyMultiWindowTaskButtonBackplate(
    FrameworkElement const& main,
    FrameworkElement const& nativeStack) {
    if (!main) {
        return;
    }

    try {
        main.Width(kMultiWindowBackplateSizeDip);
        main.Height(kMultiWindowBackplateSizeDip);
        main.HorizontalAlignment(HorizontalAlignment::Center);
        main.VerticalAlignment(VerticalAlignment::Center);
        double radius = g_hoverRadiusDip.load();
        if (radius > 0) {
            SetTaskButtonMainCornerRadius(
                main, CornerRadius{radius, radius, radius, radius});
        }
    } catch (...) {
    }
    HideTaskButtonNativeStack(nativeStack);
}

void SaveTaskButtonMainStyle(TaskButtonStackCardRecord& record,
                             FrameworkElement const& main) {
    if (!main) {
        return;
    }

    try {
        record.mainWidth = main.Width();
        record.mainHeight = main.Height();
        record.mainHorizontalAlignment = main.HorizontalAlignment();
        record.mainVerticalAlignment = main.VerticalAlignment();
        record.mainCornerRadius = GetTaskButtonMainCornerRadius(main);
        record.mainStyleSaved = true;
    } catch (...) {
    }
}

void RestoreTaskButtonMainStyle(TaskButtonStackCardRecord const& record) {
    auto main = record.main.get();
    if (!main || !record.mainStyleSaved) {
        return;
    }

    try {
        main.Width(record.mainWidth);
        main.Height(record.mainHeight);
        main.HorizontalAlignment(record.mainHorizontalAlignment);
        main.VerticalAlignment(record.mainVerticalAlignment);
        SetTaskButtonMainCornerRadius(main, record.mainCornerRadius);
    } catch (...) {
    }
}

void DumpTaskButtonStackParents(FrameworkElement const& button,
                                FrameworkElement const& main,
                                FrameworkElement const& nativeStack,
                                int windowCount) {
    FrameworkElement current = main;
    for (int depth = 0; depth < 10; depth++) {
        auto parent =
            VisualTreeHelper::GetParent(current).try_as<FrameworkElement>();
        if (!parent) {
            break;
        }

        WCHAR diag[640];
        RectangleGeometry clip{nullptr};
        WCHAR clipText[96] = L"clip=no";
        try {
            clip = parent.Clip();
            if (clip) {
                auto rect = clip.Rect();
                swprintf_s(clipText, L"clip=%.1f,%.1f,%.1fx%.1f",
                           rect.X, rect.Y, rect.Width, rect.Height);
            }
        } catch (...) {
        }

        WCHAR nativeText[128] = L"native=none";
        if (nativeStack) {
            try {
                auto rect =
                    nativeStack.TransformToVisual(button)
                        .TransformBounds(
                            {0.0f, 0.0f,
                             static_cast<float>(nativeStack.ActualWidth()),
                             static_cast<float>(nativeStack.ActualHeight())});
                swprintf_s(nativeText,
                           L"native=%.1f,%.1f,%.1fx%.1f vis=%d",
                           rect.X, rect.Y, rect.Width, rect.Height,
                           static_cast<int>(nativeStack.Visibility()));
            } catch (...) {
            }
        }

        auto className = winrt::get_class_name(parent);
        auto name = parent.Name();
        swprintf_s(
            diag,
            L"stack parent d=%d %s/%s %.1fx%.1f %s %s", depth,
            className.c_str(), name.c_str(), parent.ActualWidth(),
            parent.ActualHeight(), clipText, nativeText);
        AppendTaskButtonStackDiag(diag);

        if (parent == button) {
            break;
        }
        current = parent;
    }

    WCHAR diag[320];
    auto radius = GetTaskButtonMainCornerRadius(main);
    swprintf_s(diag,
               L"stack backplate count=%d main=%.1fx%.1f radius=%.1f",
               windowCount, main.ActualWidth(), main.ActualHeight(),
               radius.TopLeft);
    AppendTaskButtonStackDiag(diag);
}

void HideNarrowTaskButtonOverflowRecursive(
    FrameworkElement const& root,
    double mainWidth,
    double mainHeight,
    int depth) {
    if (!root || depth > 10) {
        return;
    }

    try {
        auto className = winrt::get_class_name(root);
        bool content =
            ClassNameEndsWith(className, L"TextBlock") ||
            ClassNameEndsWith(className, L"Image") ||
            ClassNameEndsWith(className, L"IconView");
        double width = root.ActualWidth();
        double height = root.ActualHeight();
        if (!content && width > 0 && height > 0 &&
            width < mainWidth * 0.6 && height > mainHeight * 1.15) {
            std::lock_guard<std::mutex> lock(g_taskButtonOverflowMutex);
            bool alreadyHidden = false;
            for (auto const& hidden : g_hiddenTaskButtonOverflowLayers) {
                if (hidden.get() == root) {
                    alreadyHidden = true;
                    break;
                }
            }
            if (!alreadyHidden) {
                ElementCompositionPreview::GetElementVisual(root)
                    .IsVisible(false);
                g_hiddenTaskButtonOverflowLayers.push_back(
                    winrt::make_weak(root));
            }
        }
    } catch (...) {
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }
    for (int i = 0; i < count; i++) {
        try {
            auto child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
            if (child) {
                HideNarrowTaskButtonOverflowRecursive(
                    child, mainWidth, mainHeight, depth + 1);
            }
        } catch (...) {
        }
    }
}

void RestoreTaskButtonOverflowLayers() {
    std::vector<winrt::weak_ref<FrameworkElement>> hiddenLayers;
    {
        std::lock_guard<std::mutex> lock(g_taskButtonOverflowMutex);
        hiddenLayers.swap(g_hiddenTaskButtonOverflowLayers);
    }
    for (auto const& weakLayer : hiddenLayers) {
        if (auto layer = weakLayer.get()) {
            try {
                ElementCompositionPreview::GetElementVisual(layer)
                    .IsVisible(true);
            } catch (...) {
            }
        }
    }
}

void HideNarrowTaskbarOverflowRecursive(FrameworkElement const& root,
                                        FrameworkElement const& coordinateRoot,
                                        int depth) {
    if (!root || depth > 30) {
        return;
    }

    try {
        auto className = winrt::get_class_name(root);
        bool content =
            ClassNameEndsWith(className, L"TextBlock") ||
            ClassNameEndsWith(className, L"Image") ||
            ClassNameEndsWith(className, L"IconView");
        double width = root.ActualWidth();
        double height = root.ActualHeight();
        winrt::Windows::Foundation::Rect rect{};
        if (!content && width > 0 && height > 0 && width < 24 &&
            height > 40 && GetElementRectDip(root, coordinateRoot, rect) &&
            rect.X < 1100) {
            std::lock_guard<std::mutex> lock(g_taskButtonOverflowMutex);
            bool alreadyHidden = false;
            for (auto const& hidden : g_hiddenTaskButtonOverflowLayers) {
                if (hidden.get() == root) {
                    alreadyHidden = true;
                    break;
                }
            }
            if (!alreadyHidden) {
                ElementCompositionPreview::GetElementVisual(root)
                    .IsVisible(false);
                g_hiddenTaskButtonOverflowLayers.push_back(
                    winrt::make_weak(root));
            }
        }
    } catch (...) {
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }
    for (int i = 0; i < count; i++) {
        try {
            auto child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
            if (child) {
                HideNarrowTaskbarOverflowRecursive(child, coordinateRoot,
                                                   depth + 1);
            }
        } catch (...) {
        }
    }
}

void HideNarrowTaskbarOverflowLayers(FrameworkElement const& root) {
    if (g_unloading || !root) {
        return;
    }
    HideNarrowTaskbarOverflowRecursive(root, root, 0);
}

void EnsureTaskButtonStackCard(FrameworkElement const& button) {
    if (g_unloading || !button) {
        return;
    }

    int windowCount = GetTaskButtonWindowCount(button);
    std::vector<FrameworkElement> layers;
    CollectTaskButtonBackgroundLayers(button, layers);
    FrameworkElement main = nullptr;
    FrameworkElement nativeStack = nullptr;
    for (auto const& layer : layers) {
        if (ElementNameEquals(layer, L"MultiWindowElement")) {
            nativeStack = layer;
        } else if (ElementNameEquals(layer, L"BackgroundElement")) {
            if (!main) {
                main = layer;
            } else if (!nativeStack) {
                nativeStack = layer;
            }
        }
    }
    bool multiWindow = windowCount > 1;

    if (!multiWindow) {
        HideTaskButtonNativeStack(nativeStack);
        std::lock_guard<std::mutex> lock(g_taskButtonStackCardMutex);
        for (auto& record : g_taskButtonStackCards) {
            if (record.button.get() == button) {
                HideTaskButtonNativeStack(record.nativeStack.get());
                RestoreTaskButtonMainStyle(record);
            }
        }
        return;
    }

    if (!main || main == nativeStack) {
        return;
    }

    double mainWidth = main.ActualWidth();
    double mainHeight = main.ActualHeight();
    if (!(mainWidth > 0) || !(mainHeight > 0)) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_taskButtonStackCardMutex);
    for (auto& record : g_taskButtonStackCards) {
        if (record.button.get() == button) {
            record.main = winrt::make_weak(main);
            record.nativeStack =
                nativeStack ? winrt::make_weak(nativeStack)
                            : winrt::weak_ref<FrameworkElement>{};
            ApplyMultiWindowTaskButtonBackplate(main, nativeStack);

            if (record.lastLoggedCount != windowCount) {
                record.lastLoggedCount = windowCount;
                auto radius = GetTaskButtonMainCornerRadius(main);
                WCHAR diag[512];
                swprintf_s(diag,
                           L"stack decision count=%d main=%.1fx%.1f "
                           L"radius=%.1f nativeVis=%d",
                           windowCount, main.ActualWidth(),
                           main.ActualHeight(), radius.TopLeft,
                           nativeStack
                               ? static_cast<int>(
                                     nativeStack.Visibility())
                               : -1);
                AppendTaskButtonStackDiag(diag);
            }
            return;
        }
    }

    TaskButtonStackCardRecord record;
    record.button = winrt::make_weak(button);
    record.main = winrt::make_weak(main);
    record.nativeStack =
        nativeStack ? winrt::make_weak(nativeStack)
                    : winrt::weak_ref<FrameworkElement>{};
    SaveTaskButtonMainStyle(record, main);

    try {
        if (nativeStack) {
            record.nativeVisibility = nativeStack.Visibility();
            record.nativeOpacity = nativeStack.Opacity();
            record.nativeCompositionVisible =
                ElementCompositionPreview::GetElementVisual(nativeStack)
                    .IsVisible();
            record.nativeStyleSaved = true;
        }
    } catch (...) {
    }

    auto buttonWeak = winrt::make_weak(button);
    auto mainWeak = winrt::make_weak(main);
    auto nativeWeak =
        nativeStack ? winrt::make_weak(nativeStack)
                    : winrt::weak_ref<FrameworkElement>{};
    auto updateState = [buttonWeak, mainWeak, nativeWeak](
                           DependencyObject const&,
                           DependencyProperty const&) {
        if (g_unloading) {
            return;
        }
        auto button = buttonWeak.get();
        auto main = mainWeak.get();
        if (button && main) {
            int count = GetTaskButtonWindowCount(button);
            if (count > 1) {
                ApplyMultiWindowTaskButtonBackplate(
                    main, nativeWeak.get());
            } else {
                HideTaskButtonNativeStack(nativeWeak.get());
            }
        }
    };

    if (nativeStack) {
        record.nativeVisibilityToken =
            nativeStack.RegisterPropertyChangedCallback(
                UIElement::VisibilityProperty(), updateState);
        record.nativeOpacityToken =
            nativeStack.RegisterPropertyChangedCallback(
                UIElement::OpacityProperty(), updateState);
    }

    ApplyMultiWindowTaskButtonBackplate(main, nativeStack);
    record.lastLoggedCount = windowCount;
    {
        WCHAR diag[640];
        auto radius = GetTaskButtonMainCornerRadius(main);
        swprintf_s(
            diag,
            L"stack create count=%d button=%.1fx%.1f main=%.1fx%.1f "
            L"radius=%.1f nativeVis=%d",
            windowCount, button.ActualWidth(), button.ActualHeight(),
            main.ActualWidth(), main.ActualHeight(), radius.TopLeft,
            nativeStack ? static_cast<int>(nativeStack.Visibility()) : -1);
        AppendTaskButtonStackDiag(diag);
    }
    DumpTaskButtonStackParents(button, main, nativeStack, windowCount);
    g_taskButtonStackCards.push_back(std::move(record));
}

void EnsureTaskButtonStackCardsRecursive(FrameworkElement const& root,
                                         int depth) {
    if (!root || depth > 20) {
        return;
    }

    try {
        if (ClassNameEquals(winrt::get_class_name(root),
                            L"Taskbar.TaskListButton")) {
            EnsureTaskButtonStackCard(root);
        }
    } catch (...) {
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }
    for (int i = 0; i < count; i++) {
        try {
            auto child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
            if (child) {
                EnsureTaskButtonStackCardsRecursive(child, depth + 1);
            }
        } catch (...) {
        }
    }
}

void NormalizeTaskButtonStackEffectsRecursive(FrameworkElement const& root,
                                              int depth) {
    if (!root || depth > 20) {
        return;
    }

    try {
        if (ClassNameEquals(winrt::get_class_name(root),
                            L"Taskbar.TaskListButton")) {
            ApplyTaskButtonStackEffect(root);
        }
    } catch (...) {
    }

    int count = 0;
    try {
        count = VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }
    for (int i = 0; i < count; i++) {
        try {
            auto child =
                VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
            if (child) {
                NormalizeTaskButtonStackEffectsRecursive(child, depth + 1);
            }
        } catch (...) {
        }
    }
}

void NormalizeTaskButtonStackEffects(FrameworkElement const& root) {
    if (g_unloading || !root) {
        return;
    }
    EnsureTaskButtonStackCardsRecursive(root, 0);
}

void RestoreTaskButtonStackCards() {
    std::vector<TaskButtonStackCardRecord> records;
    {
        std::lock_guard<std::mutex> lock(g_taskButtonStackCardMutex);
        records.swap(g_taskButtonStackCards);
    }
    for (auto const& record : records) {
        auto native = record.nativeStack.get();
        if (native) {
            try {
                if (record.nativeVisibilityToken) {
                    native.UnregisterPropertyChangedCallback(
                        UIElement::VisibilityProperty(),
                        record.nativeVisibilityToken);
                }
                if (record.nativeOpacityToken) {
                    native.UnregisterPropertyChangedCallback(
                        UIElement::OpacityProperty(),
                        record.nativeOpacityToken);
                }
                if (record.nativeStyleSaved) {
                    native.Visibility(record.nativeVisibility);
                    native.Opacity(record.nativeOpacity);
                }
                ElementCompositionPreview::GetElementVisual(native)
                    .IsVisible(record.nativeCompositionVisible);
            } catch (...) {
            }
        }
        RestoreTaskButtonMainStyle(record);
    }
}

void RestoreTaskButtonStackEffects() {
    std::vector<SavedTaskButtonStackStyle> savedStyles;
    {
        std::lock_guard<std::mutex> lock(g_taskButtonStackMutex);
        savedStyles.swap(g_savedTaskButtonStackStyles);
    }
    for (auto const& saved : savedStyles) {
        if (auto element = saved.element.get()) {
            try {
                element.Width(saved.width);
                element.Height(saved.height);
                element.HorizontalAlignment(saved.horizontalAlignment);
                element.VerticalAlignment(saved.verticalAlignment);
                element.Translation(saved.translation);
                element.RenderTransform(saved.renderTransform);
                element.IsHitTestVisible(saved.isHitTestVisible);
                if (saved.isBorder) {
                    if (auto border = element.try_as<Border>()) {
                        border.CornerRadius(saved.cornerRadius);
                        border.Background(saved.background);
                    }
                } else if (saved.isPanel) {
                    if (auto grid = element.try_as<Grid>()) {
                        grid.CornerRadius(saved.cornerRadius);
                        grid.Background(saved.background);
                    }
                } else if (saved.isPresenter) {
                    if (auto presenter = element.try_as<ContentPresenter>()) {
                        presenter.CornerRadius(saved.cornerRadius);
                        presenter.Background(saved.background);
                    }
                }
            } catch (...) {
            }
        }
    }
}

void UpdateClickThroughRegion(FrameworkElement const& rootGrid,
                              FrameworkElement const& trayElement,
                              AppliedElement& record) {
    if (g_unloading) {
        return;
    }

    HWND taskbarWnd = record.topLevelWnd;
    if (!taskbarWnd || !IsWindow(taskbarWnd)) {
        taskbarWnd = GetTaskbarWindowForCurrentThread();
    }

    HWND islandHwnd = record.islandHwnd;
    if (!islandHwnd || !IsWindow(islandHwnd)) {
        islandHwnd = FindIslandWindow(taskbarWnd);
        if (!islandHwnd) {
            if (!g_loggedMissingIsland.exchange(true)) {
                Wh_Log(L"Taskbar XAML island window not found");
            }
            return;
        }
        record.islandHwnd = islandHwnd;
    }

    g_loggedMissingIsland = false;

    HWND topLevelWnd = GetAncestor(islandHwnd, GA_ROOT);
    if (!topLevelWnd) {
        return;
    }

    auto frame = record.frame.get();
    if (!frame) {
        return;
    }

    UIElement contentElement = nullptr;
    try {
        if (auto xamlRoot = frame.XamlRoot()) {
            contentElement = xamlRoot.Content().try_as<UIElement>();
        }
    } catch (...) {
    }
    if (!contentElement) {
        return;
    }

    winrt::Windows::Foundation::Rect pillDip{};
    if (!GetElementRectDip(rootGrid, contentElement, pillDip)) {
        return;
    }

    winrt::Windows::Foundation::Rect trayDip{};
    bool hasTrayRect = GetElementRectDip(trayElement, contentElement, trayDip);

    double scale = 0.0;
    UINT dpi = GetDpiForWindow(islandHwnd);
    if (dpi) {
        scale = static_cast<double>(dpi) / 96.0;
    }
    if (scale <= 0) {
        scale = GetTaskbarScale(frame);
    }
    if (scale <= 0) {
        return;
    }

    RECT topLevelRect{};
    if (!GetWindowRect(topLevelWnd, &topLevelRect)) {
        return;
    }

    POINT islandOrigin{0, 0};
    if (!ClientToScreen(islandHwnd, &islandOrigin)) {
        return;
    }

    int topLevelWidth = topLevelRect.right - topLevelRect.left;
    int topLevelHeight = topLevelRect.bottom - topLevelRect.top;

    auto toWindowRect = [&](winrt::Windows::Foundation::Rect const& dip) {
        int left =
            (islandOrigin.x - topLevelRect.left) +
            static_cast<int>(std::lround(dip.X * scale));
        int top = (islandOrigin.y - topLevelRect.top) +
                  static_cast<int>(std::lround(dip.Y * scale));
        int right = left + static_cast<int>(std::lround(dip.Width * scale));
        int bottom = top + static_cast<int>(std::lround(dip.Height * scale));
        RECT rect{std::max(0, std::min(left, topLevelWidth)),
                  std::max(0, std::min(top, topLevelHeight)),
                  std::max(0, std::min(right, topLevelWidth)),
                  std::max(0, std::min(bottom, topLevelHeight))};
        return rect;
    };

    RECT regionRect = toWindowRect(pillDip);

    if (hasTrayRect) {
        RECT trayRect = toWindowRect(trayDip);
        UnionRect(&regionRect, &regionRect, &trayRect);
    }

    if (regionRect.right <= regionRect.left ||
        regionRect.bottom <= regionRect.top) {
        return;
    }

    RECT currentRegionBox{};
    bool hasRegion = GetWindowRgnBox(topLevelWnd, &currentRegionBox) != ERROR;
    bool regionIsOurs =
        hasRegion && record.hasAppliedRegion &&
        EqualRect(&currentRegionBox, &record.appliedRegion);

    if (hasRegion && !regionIsOurs) {
        RECT fullRegionBox{0, 0, topLevelWidth, topLevelHeight};
        if (!EqualRect(&currentRegionBox, &fullRegionBox)) {
            if (!g_loggedRegionBusy.exchange(true)) {
                Wh_Log(L"Taskbar window region is owned by Explorer");
            }
            return;
        }
    }

    if (regionIsOurs && EqualRect(&record.appliedRegion, &regionRect)) {
        return;
    }

    HRGN region = CreateRectRgn(regionRect.left, regionRect.top,
                                regionRect.right, regionRect.bottom);
    if (!region) {
        return;
    }

    if (SetWindowRgn(topLevelWnd, region, TRUE)) {
        record.topLevelWnd = topLevelWnd;
        record.appliedRegion = regionRect;
        record.hasAppliedRegion = true;
    } else {
        DeleteObject(region);
    }
}

void RemoveClickThroughRegion(AppliedElement const& record) {
    if (!record.hasAppliedRegion || !record.topLevelWnd ||
        !IsWindow(record.topLevelWnd)) {
        return;
    }

    RECT currentRegionBox{};
    if (GetWindowRgnBox(record.topLevelWnd, &currentRegionBox) == ERROR ||
        !EqualRect(&currentRegionBox, &record.appliedRegion)) {
        return;
    }

    SetWindowRgn(record.topLevelWnd, nullptr, TRUE);
}

bool RestoreFrame(AppliedElement const& record) {
    auto frame = record.frame.get();
    if (!frame) {
        return true;
    }

    if (record.trayPointerEnteredAttached) {
        if (auto tray = record.tray.get()) {
            try {
                tray.PointerEntered(record.trayPointerEnteredToken);
            } catch (...) {
            }
        }
    }

    try {
        frame.MinWidth(record.savedFrameMinWidth);
        frame.MaxWidth(record.savedFrameMaxWidth);
        frame.HorizontalAlignment(record.savedFrameAlignment);
        frame.VerticalAlignment(record.savedFrameVerticalAlignment);
        if (record.frameHeightSet) {
            frame.MaxHeight(record.savedFrameMaxHeight);
            frame.Height(record.savedFrameHeight);
        }
        if (record.frameMarginSet) {
            frame.Margin(record.savedFrameMargin);
        }
    } catch (winrt::hresult_error const&) {
        return false;
    } catch (...) {
        return false;
    }

    if (record.parentHeightSet) {
        if (auto parent = record.frameParent.get()) {
            try {
                if (std::isnan(record.savedParentHeight)) {
                    parent.ClearValue(FrameworkElement::HeightProperty());
                } else {
                    parent.Height(record.savedParentHeight);
                }
            } catch (...) {
            }
        }
    }

    if (record.repeaterAlignmentSet) {
        if (auto iconRepeater = record.iconRepeater.get()) {
            try {
                iconRepeater.HorizontalAlignment(
                    record.savedRepeaterAlignment);
                iconRepeater.Margin(record.savedRepeaterMargin);
                iconRepeater.Height(record.savedRepeaterHeight);
                iconRepeater.VerticalAlignment(
                    record.savedRepeaterVerticalAlignment);
            } catch (...) {
            }
        }
    }

    RemoveClickThroughRegion(record);

    if (auto tray = record.tray.get()) {
        try {
            tray.Margin(record.savedTrayMargin);
            tray.Height(record.savedTrayHeight);
            tray.VerticalAlignment(record.savedTrayVerticalAlignment);
            RestoreTrayHoverBorders(record.savedTrayHoverBorders);
            RestoreTrayElementLayouts(record.savedTrayOmniButtons);
            RestoreTrayElementLayouts(record.savedTrayStackLayouts);
            RestoreTrayControlShapes(record.savedTrayControlShapes);
            RestoreTrayCornerShapes(record.savedTrayCornerShapes);
            RestoreTrayRectShapes(record.savedTrayRectShapes);
        } catch (...) {
        }
    }

    Grid rootGrid = FindRootGrid(frame);
    if (rootGrid) {
        try {
            rootGrid.ClearValue(FrameworkElement::WidthProperty());
            rootGrid.ClearValue(
                FrameworkElement::HorizontalAlignmentProperty());
            rootGrid.ClearValue(Grid::CornerRadiusProperty());
            rootGrid.ClearValue(Panel::BackgroundProperty());
            rootGrid.ClearValue(FrameworkElement::MarginProperty());
            rootGrid.ClearValue(FrameworkElement::HeightProperty());
            rootGrid.ClearValue(FrameworkElement::VerticalAlignmentProperty());
        } catch (...) {
        }
    }

    std::vector<FrameworkElement> backgroundRects;
    if (rootGrid) {
        CollectBackgroundRects(rootGrid, backgroundRects);
    }

    RestoreBackgroundFills(record.savedBackgroundFills);
    RestoreBackgroundRadii(record.savedBackgroundRadii);
    RestoreBackgroundLayers(record.savedBackgroundLayers);
    RestoreHoverBorderShapes(record.savedHoverBorderShapes);

    for (auto const& element : backgroundRects) {
        try {
            element.ClearValue(UIElement::VisibilityProperty());
        } catch (...) {
        }
    }

    return true;
}

bool ApplyToFrame(FrameworkElement const& frame) {
    if (g_unloading) {
        return false;
    }

    if (!g_settingFloatingBarEnabled.load()) {
        if (IsHoverFlyoutSuppressionInstalled()) {
            RestoreHoverFlyoutSuppression();
        }
        return false;
    }

    const int taskbarWidthSetting = g_settingTaskbarWidth.load();
    const int taskbarAreaHeightSetting = g_settingTaskbarAreaHeight.load();
    const int floatMarginSetting = g_settingFloatMargin.load();
    const int cornerRadiusSetting = g_settingCornerRadius.load();
    const int hoverCornerRadiusSetting = g_settingHoverCornerRadius.load();
    const unsigned settingsVersion = g_settingsVersion.load();

    if (taskbarAreaHeightSetting > 0) {
        EnsureHoverFlyoutSuppression(frame);
    } else if (IsHoverFlyoutSuppressionInstalled()) {
        RestoreHoverFlyoutSuppression();
    }

    // Explorer creates the taskbar window at its default height and only
    // grows it when the bar is shown. Correct it before reading the window
    // height or touching the XAML layout, so barHeight is never capped to the
    // default height on the first pass.
    HWND taskbarWnd = GetTaskbarWindowForCurrentThread();
    if (taskbarAreaHeightSetting > 0) {
        EnsureTaskbarWindowHeight(taskbarWnd, taskbarAreaHeightSetting);
    }

    Grid rootGrid = FindRootGrid(frame);
    if (!rootGrid) {
        if (!g_loggedMissingRootGrid.exchange(true)) {
            Wh_Log(L"RootGrid not found, will retry on the next layout");
        }
        return false;
    }

    g_loggedMissingRootGrid = false;
    NormalizeTaskButtonStackEffects(rootGrid);

    if (g_systemTaskbarContentHeightDip.load() <= 0) {
        double contentHeight = 0;
        try {
            contentHeight = frame.ActualHeight();
        } catch (...) {
        }
        // Windows 11's normal taskbar content is 48 DIP high. If the frame has
        // already been stretched by an earlier apply, don't use that expanded
        // height as the content height.
        if (!(contentHeight > 0) || contentHeight > 60) {
            contentHeight = 48.0;
        }
        g_systemTaskbarContentHeightDip = contentHeight;
    }

    std::vector<FrameworkElement> backgroundRects;
    CollectBackgroundRects(rootGrid, backgroundRects);

    if (backgroundRects.empty()) {
        if (!g_loggedMissingRects.exchange(true)) {
            Wh_Log(L"Background rectangles not found");
        }
    } else {
        g_loggedMissingRects = false;
    }

    double scale = GetEffectiveScale(frame, taskbarWnd);
    auto toDip = [scale](int pixels) {
        return static_cast<double>(pixels) / scale;
    };

    FrameworkElement trayElement = FindSiblingTray(frame);
    if (!trayElement && !g_loggedMissingTray.exchange(true)) {
        Wh_Log(L"System tray frame not found next to the taskbar frame");
    }
    if (trayElement) {
        g_loggedMissingTray = false;
    }

    AppliedElement record{};
    bool hasRecord = false;
    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto it = g_applied.begin(); it != g_applied.end();) {
            auto appliedFrame = it->frame.get();
            if (!appliedFrame || appliedFrame == frame) {
                if (appliedFrame) {
                    record = std::move(*it);
                    hasRecord = true;
                }
                it = g_applied.erase(it);
            } else {
                ++it;
            }
        }
    }

    if (!hasRecord) {
        try {
            record.savedFrameMinWidth = frame.MinWidth();
            record.savedFrameMaxWidth = frame.MaxWidth();
            record.savedFrameMaxHeight = frame.MaxHeight();
            record.savedFrameAlignment = frame.HorizontalAlignment();
            record.savedFrameHeight = frame.Height();
            record.savedFrameVerticalAlignment = frame.VerticalAlignment();
            record.savedFrameMargin = frame.Margin();
        } catch (...) {
        }
    }

    if (trayElement) {
        g_hoverDumpTray = winrt::make_weak(trayElement);
        if (!record.trayPointerEnteredAttached) {
            try {
                record.trayPointerEnteredToken = trayElement.PointerEntered(
                    [](winrt::Windows::Foundation::IInspectable const&,
                       winrt::Windows::UI::Xaml::Input::
                           PointerRoutedEventArgs const&) {
                        g_hoverPointerEnterTick = GetTickCount();
                        g_hoverStateDumpRequested = true;
                    });
                record.trayPointerEnteredAttached = true;
            } catch (...) {
            }
        }
    }

    double appliedFrameHeightDip = 0.0;

    try {
        if (taskbarWidthSetting > 0) {
            double pillWidth = GetTaskbarWidthDip(taskbarWidthSetting, frame,
                                                  taskbarWnd, scale);
            frame.MinWidth(pillWidth);
            frame.MaxWidth(pillWidth);
            rootGrid.Width(pillWidth);
            rootGrid.HorizontalAlignment(HorizontalAlignment::Center);
        } else {
            frame.MinWidth(record.savedFrameMinWidth);
            frame.MaxWidth(record.savedFrameMaxWidth);
            rootGrid.ClearValue(FrameworkElement::WidthProperty());
            rootGrid.ClearValue(
                FrameworkElement::HorizontalAlignmentProperty());
        }

        frame.HorizontalAlignment(HorizontalAlignment::Center);

        if (taskbarAreaHeightSetting > 0) {
            frame.VerticalAlignment(VerticalAlignment::Bottom);
            frame.MaxHeight(std::numeric_limits<double>::infinity());
            appliedFrameHeightDip =
                GetFrameHeightDip(taskbarAreaHeightSetting, taskbarWnd, scale);
            if (appliedFrameHeightDip > 0) {
                frame.Height(appliedFrameHeightDip);
                record.frameHeightSet = true;
            }
        } else {
            frame.VerticalAlignment(record.savedFrameVerticalAlignment);
            if (record.frameHeightSet) {
                frame.MaxHeight(record.savedFrameMaxHeight);
                frame.Height(record.savedFrameHeight);
                record.frameHeightSet = false;
            }
        }

        frame.Margin(Thickness{0, 0, 0, 0});
        record.frameMarginSet = true;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to apply frame properties: %08X %s",
               static_cast<unsigned int>(ex.code()), ex.message().c_str());
        {
            std::lock_guard<std::mutex> lock(g_appliedMutex);
            g_applied.push_back(std::move(record));
        }
        return false;
    }

    if (record.frameHeightSet && appliedFrameHeightDip > 0) {
        SetParentHeightForFrame(frame, appliedFrameHeightDip, record);
    }

    // The system centers the taskbar icons when the taskbar alignment is set
    // to "Center". Force the icons to the left end of the floating bar instead,
    // so that the layout is the same with either system setting.
    FrameworkElement iconRepeater = FindTaskbarIconRepeater(frame);
    if (iconRepeater) {
        try {
            if (!record.repeaterAlignmentSet) {
                record.savedRepeaterAlignment =
                    iconRepeater.HorizontalAlignment();
                record.savedRepeaterMargin = iconRepeater.Margin();
                record.savedRepeaterHeight = iconRepeater.Height();
                record.savedRepeaterVerticalAlignment =
                    iconRepeater.VerticalAlignment();
                record.iconRepeater = winrt::make_weak(iconRepeater);
                record.repeaterAlignmentSet = true;
            }

            iconRepeater.HorizontalAlignment(HorizontalAlignment::Left);

            if (taskbarAreaHeightSetting > 0) {
                iconRepeater.Height(
                    GetSystemTaskbarContentHeightDip());
                iconRepeater.VerticalAlignment(VerticalAlignment::Center);
            } else {
                iconRepeater.Height(record.savedRepeaterHeight);
                iconRepeater.VerticalAlignment(
                    record.savedRepeaterVerticalAlignment);
            }

            double leftInset = GetTaskbarIconsLeftInsetDip();
            Thickness repeaterMargin = iconRepeater.Margin();
            if (std::abs(repeaterMargin.Left - leftInset) > 0.5) {
                repeaterMargin.Left = leftInset;
                iconRepeater.Margin(repeaterMargin);
            }
        } catch (...) {
        }
    }

    double areaHeight = GetTaskbarAreaHeightDip(taskbarAreaHeightSetting, frame,
                                                taskbarWnd, scale);
    double floatMargin = GetFloatMarginDip(floatMarginSetting, scale);
    double windowHeight = GetWindowHeightDip(taskbarWnd, scale);
    // Use the configured area height for the content layout. The actual
    // window may still be at Explorer's default height on the first pass;
    // EnsureTaskbarWindowHeight above corrects it, and the layout must not be
    // capped to the old window height in the meantime.
    double barHeight =
        areaHeight > 0 ? std::max(1.0, areaHeight - floatMargin) : 0.0;

    if (areaHeight <= 0 && !g_loggedStripUnknown.exchange(true)) {
        Wh_Log(L"Taskbar strip height not available yet");
    }
    if (areaHeight > 0) {
        g_loggedStripUnknown = false;
    }

    try {
        double cornerRadius = toDip(cornerRadiusSetting);
        rootGrid.CornerRadius(
            CornerRadius{cornerRadius, cornerRadius, cornerRadius,
                         cornerRadius});

        if (areaHeight > 0) {
            rootGrid.Height(barHeight);
            rootGrid.VerticalAlignment(VerticalAlignment::Bottom);
            rootGrid.Margin(Thickness{0, 0, 0, floatMargin});
        } else {
            rootGrid.ClearValue(FrameworkElement::HeightProperty());
            rootGrid.ClearValue(FrameworkElement::VerticalAlignmentProperty());
            rootGrid.Margin(Thickness{0, 0, 0, 0});
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to apply grid properties: %08X %s",
               static_cast<unsigned int>(ex.code()), ex.message().c_str());
        {
            std::lock_guard<std::mutex> lock(g_appliedMutex);
            g_applied.push_back(std::move(record));
        }
        return false;
    }

    bool trayArranged = false;
    if (trayElement) {
        if (!hasRecord) {
            try {
                record.savedTrayMargin = trayElement.Margin();
                record.savedTrayHeight = trayElement.Height();
                record.savedTrayVerticalAlignment =
                    trayElement.VerticalAlignment();
            } catch (...) {
            }
        }

        double gapDip =
            GetTrayGapDip(taskbarWidthSetting, frame, trayElement, taskbarWnd,
                          scale);

        try {
            if (areaHeight > 0) {
                double contentHeight =
                    GetSystemTaskbarContentHeightDip();
                trayElement.Height(barHeight);
                // Align the tray to the same top edge as the pill. The old
                // Bottom + floatMargin combination depended on the system
                // preserving the bottom margin; when that margin was reset
                // the whole tray (and the clock) dropped below the pill.
                trayElement.VerticalAlignment(VerticalAlignment::Top);
                trayElement.Margin(Thickness{0, 0, gapDip, 0});

                FrameworkElement trayFrameGrid = FindChildByPredicate(
                    trayElement, [](FrameworkElement const& element) {
                        return ElementNameEquals(element,
                                                 L"SystemTrayFrameGrid");
                    });
                if (trayFrameGrid) {
                    if (!record.trayFrameGridHeightSet) {
                        record.savedTrayFrameGridHeight =
                            trayFrameGrid.Height();
                        record.savedTrayFrameGridVerticalAlignment =
                            trayFrameGrid.VerticalAlignment();
                        record.trayFrameGrid =
                            winrt::make_weak(trayFrameGrid);
                        record.trayFrameGridHeightSet = true;
                    }
                    // Without this, the inner tray row keeps its original
                    // 48 DIP height. Offsetting the clock by half of the
                    // larger outer height then pushes its bottom line past
                    // the row and clips it.
                    trayFrameGrid.Height(barHeight);
                    trayFrameGrid.VerticalAlignment(
                        VerticalAlignment::Stretch);
                }

                // Explorer can create the tray stacks after the first
                // height pass. Those stacks keep their original Bottom
                // alignment until a later system layout, which is the
                // several-second low-then-centered jump after an Explorer
                // restart. Center the named tray stacks explicitly together
                // with the height pass.
                constexpr PCWSTR kTrayStackNames[] = {
                    L"MainStack",
                    L"NonActivatableStack",
                    L"ControlCenterButton",
                    L"NotificationCenterButton",
                    L"ShowDesktopStack",
                };
                for (PCWSTR stackName : kTrayStackNames) {
                    FrameworkElement stack = FindChildByPredicate(
                        trayElement,
                        [stackName](FrameworkElement const& element) {
                            return ElementNameEquals(element, stackName);
                        });
                    if (!stack) {
                        continue;
                    }

                    SaveTrayElementLayout(record.savedTrayStackLayouts,
                                          stack);
                    try {
                        stack.VerticalAlignment(
                            VerticalAlignment::Center);
                        Thickness stackMargin = stack.Margin();
                        stackMargin.Top = 0;
                        stackMargin.Bottom = 0;
                        stack.Margin(stackMargin);
                    } catch (...) {
                    }

                    {
                        WCHAR stackDiag[600];
                        auto className =
                            winrt::get_class_name(stack);
                        swprintf_s(
                            stackDiag,
                            L"tray stack name=%s class=%s ah=%.1f h=%.1f "
                            L"va=%d",
                            stackName, className.c_str(),
                            stack.ActualHeight(), stack.Height(),
                            static_cast<int>(stack.VerticalAlignment()));
                        AppendTrayLayoutDiag(stackDiag);
                    }
                }

                std::vector<FrameworkElement> hoverBorders;
                CollectTrayHoverBorders(trayElement, hoverBorders);
                double hoverHeight =
                    std::max(1.0, contentHeight - 14.0);
                g_trayHoverHeightDip = hoverHeight;
                double trayCornerRadius =
                    toDip(hoverCornerRadiusSetting);
                g_hoverRadiusDip = trayCornerRadius;

                std::vector<FrameworkElement> trayControls;
                CollectTrayControls(trayElement, trayControls);
                for (auto const& control : trayControls) {
                    SaveTrayControlShape(record, control);
                    ApplyTrayControlShape(control, trayCornerRadius);

                    static std::atomic<int> s_trayControlLogs{0};
                    if (s_trayControlLogs.fetch_add(1) < 24) {
                        WCHAR controlDiag[600];
                        auto className =
                            winrt::get_class_name(control);
                        auto elementName = control.Name();
                        swprintf_s(
                            controlDiag,
                            L"tray control class=%s name=%s corner=%.1f "
                            L"focus=0",
                            className.c_str(), elementName.c_str(),
                            trayCornerRadius);
                        AppendTrayLayoutDiag(controlDiag);
                    }
                }

                ApplyTrayCornerShapesRecursive(record, trayElement,
                                               trayCornerRadius);
                ApplyTrayRectShapesRecursive(record, trayElement,
                                             trayCornerRadius);

                for (auto const& element : hoverBorders) {
                    SaveTrayHoverBorderState(record, element);
                    ApplyTrayHoverBorderLayout(element, hoverHeight);
                }

                double rowHeight =
                    areaHeight > 0 ? barHeight : trayElement.ActualHeight();
                if (!(rowHeight > 0)) {
                    rowHeight = barHeight;
                }
                g_trayRowHeightDip = rowHeight;
                std::vector<FrameworkElement> omniButtons;
                CollectTrayOmniButtons(trayElement, omniButtons);
                for (auto const& element : omniButtons) {
                    SaveTrayElementLayout(
                        record.savedTrayOmniButtons, element);
                    ApplyTrayOmniButtonLayout(element, rowHeight);
                }

                {
                    WCHAR diag[600];
                    swprintf_s(
                        diag,
                        L"tray ah=%.1f content=%.1f hover=%.1f omni=%d "
                        L"borders=%d grid=%.1f/%.1f",
                        trayElement.ActualHeight(), contentHeight, hoverHeight,
                        static_cast<int>(omniButtons.size()),
                        static_cast<int>(hoverBorders.size()),
                        trayFrameGrid ? trayFrameGrid.ActualHeight() : -1.0,
                        trayFrameGrid ? trayFrameGrid.Height() : -1.0);
                    AppendTrayLayoutDiag(diag);

                    for (auto const& element : omniButtons) {
                        auto className = winrt::get_class_name(element);
                        Thickness margin = element.Margin();
                        double parentHeight = -1.0;
                        if (auto parent =
                                VisualTreeHelper::GetParent(element)
                                    .try_as<FrameworkElement>()) {
                            parentHeight = parent.ActualHeight();
                        }
                        swprintf_s(
                            diag,
                            L"omni class=%s ah=%.1f h=%.1f top=%.1f "
                            L"bottom=%.1f va=%d parent=%.1f",
                            className.c_str(), element.ActualHeight(),
                            element.Height(), margin.Top, margin.Bottom,
                            static_cast<int>(element.VerticalAlignment()),
                            parentHeight);
                        AppendTrayLayoutDiag(diag);
                    }

                    for (auto const& element : hoverBorders) {
                        auto className = winrt::get_class_name(element);
                        auto border = element.try_as<Border>();
                        Thickness margin = element.Margin();
                        swprintf_s(
                            diag,
                            L"border class=%s h=%.1f top=%.1f bottom=%.1f "
                            L"va=%d",
                            className.c_str(),
                            border ? border.Height() : -1.0, margin.Top,
                            margin.Bottom,
                            static_cast<int>(element.VerticalAlignment()));
                        AppendTrayLayoutDiag(diag);
                    }
                }
            } else {
                trayElement.Height(record.savedTrayHeight);
                trayElement.VerticalAlignment(
                    record.savedTrayVerticalAlignment);
                trayElement.Margin(Thickness{0, 0, gapDip, 0});
                if (record.trayFrameGridHeightSet) {
                    if (auto grid = record.trayFrameGrid.get()) {
                        try {
                            grid.Height(record.savedTrayFrameGridHeight);
                            grid.VerticalAlignment(
                                record.savedTrayFrameGridVerticalAlignment);
                        } catch (...) {
                        }
                    }
                    record.trayFrameGridHeightSet = false;
                }
                RestoreTrayHoverBorders(record.savedTrayHoverBorders);
                record.savedTrayHoverBorders.clear();
                RestoreTrayElementLayouts(record.savedTrayOmniButtons);
                record.savedTrayOmniButtons.clear();
                RestoreTrayElementLayouts(record.savedTrayStackLayouts);
                record.savedTrayStackLayouts.clear();
                RestoreTrayControlShapes(record.savedTrayControlShapes);
                record.savedTrayControlShapes.clear();
                RestoreTrayCornerShapes(record.savedTrayCornerShapes);
                record.savedTrayCornerShapes.clear();
                RestoreTrayRectShapes(record.savedTrayRectShapes);
                record.savedTrayRectShapes.clear();
            }
            trayArranged = true;
            record.trayArranged = true;
            record.trayContentReady =
                trayElement.ActualWidth() >= 1.0 &&
                trayElement.ActualHeight() >= 1.0;
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Failed to apply tray properties: %08X",
                   static_cast<unsigned int>(ex.code()));
        }
    }
    if (!trayElement) {
        record.trayContentReady = false;
    }

    if (trayArranged && trayElement && record.trayContentReady) {
        NormalizeTrayHoverTransformsRecursive(trayElement);
        ClockApplyTextStyles(trayElement);
    }

    {
        std::vector<FrameworkElement> rootHoverBackgrounds;
        std::vector<FrameworkElement> trayHoverBackgrounds;
        CollectHoverBackgroundBorders(rootGrid, rootHoverBackgrounds);
        if (trayElement) {
            CollectHoverBackgroundBorders(trayElement, trayHoverBackgrounds);
        }
        double hoverCornerRadiusDip = toDip(hoverCornerRadiusSetting);
        g_hoverRadiusDip = hoverCornerRadiusDip;
        for (auto const& element : rootHoverBackgrounds) {
            SaveHoverBorderShape(record, element, false);
            ApplyHoverBorderShape(element, hoverCornerRadiusDip);
        }
        double trayHoverHeight = g_trayHoverHeightDip.load();
        for (auto const& element : trayHoverBackgrounds) {
            SaveHoverBorderShape(record, element, true);
            ApplyHoverBorderShape(element, hoverCornerRadiusDip);
            if (trayHoverHeight > 0) {
                ApplyTrayHoverBorderLayout(element, trayHoverHeight);
            }
        }

        std::vector<FrameworkElement> hoverBackgrounds = rootHoverBackgrounds;
        hoverBackgrounds.insert(hoverBackgrounds.end(),
                                trayHoverBackgrounds.begin(),
                                trayHoverBackgrounds.end());
        WCHAR diag[600];
        swprintf_s(diag, L"hoverBackgrounds=%d",
                   static_cast<int>(hoverBackgrounds.size()));
        AppendTrayLayoutDiag(diag);
        for (auto const& element : hoverBackgrounds) {
            auto border = element.try_as<Border>();
            auto className = winrt::get_class_name(element);
            auto elementName = element.Name();
            CornerRadius radius =
                border ? border.CornerRadius() : CornerRadius{};
            swprintf_s(diag, L"hover class=%s name=%s r=%.1f",
                       className.c_str(), elementName.c_str(),
                       radius.TopLeft);
            AppendTrayLayoutDiag(diag);
        }
    }

    BackgroundSettings backgroundSettings;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        backgroundSettings = g_backgroundSettings;
    }
    try {
        Brush customBrush = CreateBackgroundBrush(backgroundSettings);
        Brush blurBrush =
            customBrush &&
                    backgroundSettings.style == BackgroundStyle::gradient
                ? CreateBackdropBlurBrush()
                : nullptr;
        {
            WCHAR blurDiag[160];
            swprintf_s(blurDiag, L"bg blur=%s",
                       blurBrush ? L"acrylic" : L"none");
            AppendTrayLayoutDiag(blurDiag);
        }

        Brush effectiveBrush =
            customBrush ? customBrush
                        : FindOriginalBackgroundBrush(backgroundRects);

        if (blurBrush) {
            // Keep the acrylic blur behind the existing bar content, then
            // paint the gradient on the BackgroundFill rectangle so the
            // configured colors and opacity stay unchanged.
            rootGrid.Background(blurBrush);
        } else if (customBrush) {
            // Paint the custom brush on the common background layer so a
            // gradient spans the whole floating bar instead of restarting in
            // each BackgroundFill segment.
            rootGrid.Background(customBrush);
        } else if (effectiveBrush) {
            rootGrid.Background(effectiveBrush);
        } else {
            rootGrid.ClearValue(Panel::BackgroundProperty());
        }

        if (customBrush) {
            // The visible taskbar background is the Fill of the BackgroundFill
            // rectangle. The TaskbarBackground grid underneath it also paints
            // a system material layer on recent Windows builds, so clear that
            // layer to avoid covering the custom brush.
            SolidColorBrush transparentBrush;
            transparentBrush.Color(winrt::Windows::UI::Color{0, 0, 0, 0});

            std::vector<FrameworkElement> backgroundLayers;
            CollectTaskbarBackgroundLayers(rootGrid, backgroundLayers);
            for (auto const& element : backgroundLayers) {
                SaveBackgroundLayerState(record, element);
                ClearBackgroundLayer(element, transparentBrush);
            }
            if (trayElement) {
                SaveBackgroundLayerState(record, trayElement);
                ClearBackgroundLayer(trayElement, transparentBrush);

                std::vector<FrameworkElement> trayBackgroundLayers;
                CollectTrayBackgroundLayers(trayElement,
                                            trayBackgroundLayers,
                                            trayElement.ActualWidth());
                for (auto const& element : trayBackgroundLayers) {
                    if (auto rect = element.try_as<Shapes::Rectangle>()) {
                        SaveBackgroundFillState(record, element);
                        rect.Fill(transparentBrush);
                    } else {
                        SaveBackgroundLayerState(record, element);
                        ClearBackgroundLayer(element, transparentBrush);
                    }
                }

                {
                    WCHAR trayDiag[600];
                    swprintf_s(trayDiag,
                               L"tray bg layers=%d trayW=%.1f trayH=%.1f",
                               static_cast<int>(trayBackgroundLayers.size()),
                               trayElement.ActualWidth(),
                               trayElement.ActualHeight());
                    AppendTrayLayoutDiag(trayDiag);

                    for (auto const& element : trayBackgroundLayers) {
                        auto className = winrt::get_class_name(element);
                        auto elementName = element.Name();
                        bool isFill =
                            element.try_as<Shapes::Rectangle>() != nullptr;
                        swprintf_s(trayDiag,
                                   L"tray %s class=%s name=%s w=%.1f h=%.1f",
                                   isFill ? L"fill" : L"bg",
                                   className.c_str(), elementName.c_str(),
                                   element.ActualWidth(),
                                   element.ActualHeight());
                        AppendTrayLayoutDiag(trayDiag);
                    }
                }
            }

            std::vector<FrameworkElement> flyoutLayers;
            CollectHoverFlyoutLayers(frame, flyoutLayers);
            if (trayElement) {
                CollectHoverFlyoutLayers(trayElement, flyoutLayers);
            }
            for (auto const& element : flyoutLayers) {
                if (auto rect = element.try_as<Shapes::Rectangle>()) {
                    SaveBackgroundFillState(record, element);
                    rect.Fill(transparentBrush);
                } else {
                    SaveBackgroundLayerState(record, element);
                    ClearBackgroundLayer(element, transparentBrush);
                }
            }

            {
                WCHAR flyoutDiag[600];
                swprintf_s(flyoutDiag, L"flyout layers=%d",
                           static_cast<int>(flyoutLayers.size()));
                AppendTrayLayoutDiag(flyoutDiag);
                for (auto const& element : flyoutLayers) {
                    auto className = winrt::get_class_name(element);
                    auto elementName = element.Name();
                    swprintf_s(flyoutDiag,
                               L"flyout class=%s name=%s w=%.1f h=%.1f",
                               className.c_str(), elementName.c_str(),
                               element.ActualWidth(),
                               element.ActualHeight());
                    AppendTrayLayoutDiag(flyoutDiag);
                }
            }

            SaveBackgroundLayerState(record, frame);
            ClearBackgroundLayer(frame, transparentBrush);

            double cornerRadiusDip = toDip(cornerRadiusSetting);
            for (auto const& element : backgroundRects) {
                if (auto rect = element.try_as<Shapes::Rectangle>()) {
                    SaveBackgroundRadiusState(record, element, rect);
                    rect.RadiusX(cornerRadiusDip);
                    rect.RadiusY(cornerRadiusDip);
                }
            }

            for (auto const& element : backgroundRects) {
                if (!ElementNameEquals(element, L"BackgroundFill")) {
                    continue;
                }

                if (auto rect = element.try_as<Shapes::Rectangle>()) {
                    SaveBackgroundFillState(record, element);
                    rect.Fill(blurBrush ? customBrush : transparentBrush);
                    element.Visibility(Visibility::Visible);

                    if (auto parent =
                            VisualTreeHelper::GetParent(element)
                                .try_as<Panel>()) {
                        parent.Background(transparentBrush);
                    }
                }
            }

            for (auto const& element : backgroundRects) {
                if (ElementNameEquals(element, L"BackgroundStroke")) {
                    try {
                        if (backgroundSettings.style ==
                            BackgroundStyle::transparent) {
                            element.Visibility(Visibility::Collapsed);
                        } else {
                            element.ClearValue(
                                UIElement::VisibilityProperty());
                        }
                    } catch (...) {
                    }
                }
            }

            {
                WCHAR diag[600];
                swprintf_s(diag, L"bg rects=%d layers=%d rootW=%.1f",
                           static_cast<int>(backgroundRects.size()),
                           static_cast<int>(backgroundLayers.size()),
                           rootGrid.ActualWidth());
                AppendTrayLayoutDiag(diag);

                for (auto const& element : backgroundRects) {
                    auto rect = element.try_as<Shapes::Rectangle>();
                    auto elementName = element.Name();
                    auto fill = rect ? rect.Fill() : nullptr;
                    winrt::hstring fillClass =
                        fill ? winrt::get_class_name(fill)
                             : winrt::hstring(L"null");
                    swprintf_s(diag,
                               L"bg rect name=%s w=%.1f h=%.1f fill=%s",
                               elementName.c_str(), element.ActualWidth(),
                               element.ActualHeight(), fillClass.c_str());
                    AppendTrayLayoutDiag(diag);
                }

                for (auto const& element : backgroundLayers) {
                    auto className = winrt::get_class_name(element);
                    auto elementName = element.Name();
                    swprintf_s(diag, L"bg layer class=%s name=%s w=%.1f h=%.1f",
                               className.c_str(), elementName.c_str(),
                               element.ActualWidth(),
                               element.ActualHeight());
                    AppendTrayLayoutDiag(diag);
                }

                static std::atomic<int> s_bgTreeDumps{0};
                if (s_bgTreeDumps.fetch_add(1) < 4) {
                    for (auto const& element : backgroundRects) {
                        if (!ElementNameEquals(element, L"BackgroundFill")) {
                            continue;
                        }

                        FrameworkElement current = element;
                        for (int depth = 0; depth < 8; depth++) {
                            auto parent = VisualTreeHelper::GetParent(current)
                                              .try_as<FrameworkElement>();
                            if (!parent) {
                                break;
                            }

                            auto className = winrt::get_class_name(parent);
                            auto elementName = parent.Name();
                            winrt::hstring bgClass(L"null");
                            if (auto panel = parent.try_as<Panel>()) {
                                auto background = panel.Background();
                                if (background) {
                                    bgClass =
                                        winrt::get_class_name(background);
                                }
                            } else if (auto control =
                                           parent.try_as<Control>()) {
                                auto background = control.Background();
                                if (background) {
                                    bgClass =
                                        winrt::get_class_name(background);
                                }
                            }

                            swprintf_s(
                                diag,
                                L"bg parent depth=%d class=%s name=%s "
                                L"w=%.1f h=%.1f bg=%s",
                                depth, className.c_str(), elementName.c_str(),
                                parent.ActualWidth(), parent.ActualHeight(),
                                bgClass.c_str());
                            AppendTrayLayoutDiag(diag);
                            current = parent;
                        }
                        break;
                    }
                }
            }

            record.appliedBackgroundFill =
                blurBrush ? customBrush : transparentBrush;
            record.appliedRootBackground =
                blurBrush ? blurBrush : customBrush;
            record.backgroundFillApplied = true;
        } else {
            RestoreBackgroundFills(record.savedBackgroundFills);
            record.savedBackgroundFills.clear();
            RestoreBackgroundRadii(record.savedBackgroundRadii);
            record.savedBackgroundRadii.clear();
            RestoreBackgroundLayers(record.savedBackgroundLayers);
            record.savedBackgroundLayers.clear();
            record.appliedBackgroundFill = nullptr;
            record.appliedRootBackground = nullptr;
            record.backgroundFillApplied = false;

            for (auto const& element : backgroundRects) {
                try {
                    element.ClearValue(UIElement::VisibilityProperty());
                } catch (...) {
                }
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to apply background: %08X",
               static_cast<unsigned int>(ex.code()));
    }

    record.threadId = GetCurrentThreadId();
    record.version = settingsVersion;
    record.lastApplyTick = GetTickCount();
    try {
        record.frame = winrt::make_weak(frame);
        record.rootGrid = winrt::make_weak(rootGrid);
        record.tray = winrt::weak_ref<FrameworkElement>{};
        if (trayElement) {
            record.tray = winrt::make_weak(trayElement);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Failed to create weak references: %08X",
               static_cast<unsigned int>(ex.code()));
    }

    UpdateClickThroughRegion(rootGrid, trayElement, record);
    if (!record.hasAppliedRegion) {
        record.regionRetries++;
    }
    bool regionApplied = record.hasAppliedRegion;

    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        g_applied.push_back(std::move(record));
    }

    try {
        double pillWidthPx = taskbarWidthSetting > 0
                                 ? static_cast<double>(taskbarWidthSetting)
                                 : rootGrid.ActualWidth() * scale;
        if (pillWidthPx > 0) {
            g_lastPillWidthPx.store(pillWidthPx);
        }
    } catch (...) {
    }

    if (GetTickCount() - g_lastControlCenterFixTick.load() > 500) {
        RepositionControlCenterWindows(L"layout");
    }

    double frameDip = 0.0;
    double frameHeightDip = 0.0;
    double frameWidthDip = 0.0;
    double frameMarginLeft = 0.0;
    double frameMarginRight = 0.0;
    double rootGridDip = 0.0;
    double rootGridWidthDip = 0.0;
    double parentDip = 0.0;
    double parentHeightDip = 0.0;
    double parentWidthDip = 0.0;
    try {
        frameDip = frame.ActualHeight();
        frameHeightDip = frame.Height();
        frameWidthDip = frame.ActualWidth();
        Thickness frameMargin = frame.Margin();
        frameMarginLeft = frameMargin.Left;
        frameMarginRight = frameMargin.Right;
        rootGridDip = rootGrid.ActualHeight();
        rootGridWidthDip = rootGrid.ActualWidth();
        if (auto parentElement = VisualTreeHelper::GetParent(frame)
                                     .try_as<FrameworkElement>()) {
            parentDip = parentElement.ActualHeight();
            parentHeightDip = parentElement.Height();
            parentWidthDip = parentElement.ActualWidth();
        }
    } catch (...) {
    }

    Wh_Log(
        L"Applied: width=%d area=%d margin=%d corner=%d areaDip=%.1f "
        L"barDip=%.1f marginDip=%.1f clamped=%d tray=%d region=%d",
        taskbarWidthSetting, taskbarAreaHeightSetting, floatMarginSetting,
        cornerRadiusSetting, areaHeight, barHeight, floatMargin,
        (areaHeight - floatMargin > barHeight + 0.5) ? 1 : 0,
        trayArranged ? 1 : 0, regionApplied ? 1 : 0);

    Wh_Log(
        L"Geometry: frameDip=%.1f frameHeight=%.1f frameW=%.1f "
        L"frameMarginL=%.1f frameMarginR=%.1f rootGridDip=%.1f "
        L"rootGridW=%.1f windowDip=%.1f windowW=%.1f parentDip=%.1f "
        L"parentHeight=%.1f parentW=%.1f",
        frameDip, frameHeightDip, frameWidthDip, frameMarginLeft,
        frameMarginRight, rootGridDip, rootGridWidthDip, windowHeight,
        GetWindowWidthDip(taskbarWnd, scale), parentDip, parentHeightDip,
        parentWidthDip);

    return true;
}

void HandleFrameEvent(void* pThis) {
    if (g_unloading) {
        return;
    }

    if (!g_settingFloatingBarEnabled.load()) {
        return;
    }

    try {
        FrameworkElement frame = nullptr;
        ((IUnknown*)pThis)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(frame));
        if (!frame) {
            return;
        }

        if (!ClassNameEquals(winrt::get_class_name(frame),
                             L"Taskbar.TaskbarFrame")) {
            return;
        }

        if (GetTickCount() - g_lastControlCenterFixTick.load() > 400) {
            RepositionControlCenterCached(L"pointer");
        }

        bool needsApply = true;
        bool needsHeightKick = false;
        {
            std::lock_guard<std::mutex> lock(g_appliedMutex);
            unsigned version = g_settingsVersion.load();
            const int taskbarWidthSetting = g_settingTaskbarWidth.load();
            const int taskbarAreaHeightSetting =
                g_settingTaskbarAreaHeight.load();
            const int floatMarginSetting = g_settingFloatMargin.load();
            const int cornerRadiusSetting = g_settingCornerRadius.load();

            for (auto const& applied : g_applied) {
                auto appliedFrame = applied.frame.get();
                if (!appliedFrame || appliedFrame != frame) {
                    continue;
                }

                if (applied.version != version) {
                    break;
                }

                needsApply = false;

                // If the tray was still 0x0 during the last apply, its
                // OmniButton and BackgroundElement tree did not exist yet.
                // Re-apply as soon as the tray has a real size; otherwise
                // the first Quick Settings opening shows the system's
                // full-height square background instead of the configured
                // rounded capsule.
                if (!applied.trayContentReady) {
                    if (auto appliedTray = applied.tray.get()) {
                        if (appliedTray.ActualWidth() < 1.0 ||
                            appliedTray.ActualHeight() < 1.0) {
                            needsApply = true;
                        }
                    }
                }

                HWND taskbarWnd = GetTaskbarWindowForCurrentThread();
                double scale = GetEffectiveScale(appliedFrame, taskbarWnd);

                if (taskbarAreaHeightSetting > 0 && scale > 0) {
                    double expectedWindowHeight = GetFrameHeightDip(
                        taskbarAreaHeightSetting, taskbarWnd, scale);
                    double currentWindowHeight =
                        GetWindowHeightDip(taskbarWnd, scale);
                    if (expectedWindowHeight > 0 &&
                        currentWindowHeight > 0 &&
                        currentWindowHeight + 1.0 < expectedWindowHeight) {
                        // Explorer keeps the Shell_TrayWnd window at its
                        // default height when the XAML frame grows before
                        // the window has been created/rebuilt. Re-apply the
                        // frame and kick the window height after it.
                        needsApply = true;
                        needsHeightKick = true;
                    }
                }

                if (taskbarWidthSetting > 0) {
                    double expectedWidth = GetTaskbarWidthDip(
                        taskbarWidthSetting, appliedFrame, taskbarWnd, scale);
                    double currentWidth = appliedFrame.ActualWidth();
                    if (currentWidth <= 0 ||
                        std::abs(currentWidth - expectedWidth) > 2.0) {
                        needsApply = true;
                    }
                }

                if (appliedFrame.HorizontalAlignment() !=
                    HorizontalAlignment::Center) {
                    needsApply = true;
                }

                Thickness frameMargin = appliedFrame.Margin();
                if (std::abs(frameMargin.Left) > 0.5 ||
                    std::abs(frameMargin.Top) > 0.5 ||
                    std::abs(frameMargin.Right) > 0.5 ||
                    std::abs(frameMargin.Bottom) > 0.5) {
                    needsApply = true;
                }

                if (auto appliedRepeater = applied.iconRepeater.get()) {
                    double expectedInset = GetTaskbarIconsLeftInsetDip();
                    if (appliedRepeater.HorizontalAlignment() !=
                            HorizontalAlignment::Left ||
                        std::abs(appliedRepeater.Margin().Left -
                                 expectedInset) > 0.5) {
                        needsApply = true;
                    }

                    if (!needsApply && taskbarAreaHeightSetting > 0) {
                        double contentHeight =
                            GetSystemTaskbarContentHeightDip();
                        if (std::isnan(appliedRepeater.Height()) ||
                            std::abs(appliedRepeater.Height() -
                                     contentHeight) > 0.5 ||
                            appliedRepeater.VerticalAlignment() !=
                                VerticalAlignment::Center) {
                            needsApply = true;
                        }
                    }
                }

                if (applied.backgroundFillApplied) {
                    for (auto const& saved : applied.savedBackgroundFills) {
                        auto element = saved.element.get();
                        auto rect =
                            element ? element.try_as<Shapes::Rectangle>()
                                    : nullptr;
                        if (!rect ||
                            winrt::get_abi(rect.Fill()) !=
                                winrt::get_abi(
                                    applied.appliedBackgroundFill)) {
                            needsApply = true;
                            break;
                        }
                    }

                    if (!needsApply) {
                        if (auto appliedRootGrid = applied.rootGrid.get()) {
                            if (winrt::get_abi(
                                    appliedRootGrid.Background()) !=
                                winrt::get_abi(
                                    applied.appliedRootBackground)) {
                                needsApply = true;
                            }
                        }
                    }

                    double expectedRadius =
                        scale > 0 ? cornerRadiusSetting / scale : 0.0;
                    for (auto const& saved : applied.savedBackgroundRadii) {
                        auto element = saved.element.get();
                        auto rect =
                            element ? element.try_as<Shapes::Rectangle>()
                                    : nullptr;
                        if (!rect ||
                            std::abs(rect.RadiusX() - expectedRadius) > 0.5 ||
                            std::abs(rect.RadiusY() - expectedRadius) > 0.5) {
                            needsApply = true;
                            break;
                        }
                    }
                }

                if (auto appliedRootGrid = applied.rootGrid.get()) {
                    double expectedRadius = scale > 0
                                                ? cornerRadiusSetting / scale
                                                : 0.0;
                    if (taskbarWidthSetting > 0 && scale > 0) {
                        double expectedGridWidth = GetTaskbarWidthDip(
                            taskbarWidthSetting, appliedFrame, taskbarWnd,
                            scale);
                        double currentGridWidth = appliedRootGrid.Width();
                        if (std::isnan(currentGridWidth) ||
                            std::abs(currentGridWidth - expectedGridWidth) >
                                2.0) {
                            needsApply = true;
                        }
                    }
                    CornerRadius radius = appliedRootGrid.CornerRadius();
                    if (std::abs(radius.TopLeft - expectedRadius) > 0.5 ||
                        std::abs(radius.TopRight - expectedRadius) > 0.5 ||
                        std::abs(radius.BottomRight - expectedRadius) > 0.5 ||
                        std::abs(radius.BottomLeft - expectedRadius) > 0.5) {
                        needsApply = true;
                    }
                }

                double areaHeight = GetTaskbarAreaHeightDip(
                    taskbarAreaHeightSetting, appliedFrame, taskbarWnd, scale);
                double floatMargin =
                    GetFloatMarginDip(floatMarginSetting, scale);
                double barHeight =
                    GetBarHeightDip(areaHeight,
                                    GetWindowHeightDip(taskbarWnd, scale),
                                    floatMargin);

                if (!needsApply && areaHeight > 0) {
                    if (auto appliedRootGrid = applied.rootGrid.get()) {
                        double currentHeight = appliedRootGrid.Height();
                        if (std::isnan(currentHeight) ||
                            std::abs(currentHeight - barHeight) > 0.5) {
                            needsApply = true;
                        } else {
                            Thickness margin = appliedRootGrid.Margin();
                            if (std::abs(margin.Bottom - floatMargin) > 0.5 ||
                                appliedRootGrid.VerticalAlignment() !=
                                    VerticalAlignment::Bottom) {
                                needsApply = true;
                            }
                        }
                    } else {
                        needsApply = true;
                    }
                }

                if (!needsApply && taskbarAreaHeightSetting > 0) {
                    double expectedAreaHeight = GetFrameHeightDip(
                        taskbarAreaHeightSetting, taskbarWnd, scale);
                    double currentAreaHeight = appliedFrame.Height();
                    if (std::isnan(currentAreaHeight) ||
                        std::abs(currentAreaHeight - expectedAreaHeight) >
                            0.5 ||
                        appliedFrame.VerticalAlignment() !=
                            VerticalAlignment::Bottom) {
                        needsApply = true;
                    }
                }

                if (!needsApply && applied.trayArranged) {
                    FrameworkElement currentTray =
                        FindSiblingTray(appliedFrame);
                    if (currentTray) {
                        double gapDip =
                            GetTrayGapDip(taskbarWidthSetting, appliedFrame,
                                          currentTray, taskbarWnd, scale);
                        Thickness margin = currentTray.Margin();
                        double contentHeight =
                            GetSystemTaskbarContentHeightDip();
                        if (std::abs(margin.Right - gapDip) > 0.5 ||
                            std::abs(margin.Left) > 0.5 ||
                            std::abs(margin.Top) > 0.5 ||
                            std::abs(margin.Bottom) > 0.5) {
                            needsApply = true;
                        }

                        if (!needsApply && areaHeight > 0) {
                            double currentTrayHeight = currentTray.Height();
                            if (std::isnan(currentTrayHeight) ||
                                std::abs(currentTrayHeight -
                                         barHeight) > 0.5 ||
                                currentTray.VerticalAlignment() !=
                                    VerticalAlignment::Top) {
                                needsApply = true;
                            }
                        }

                        if (!needsApply && areaHeight > 0) {
                            std::vector<FrameworkElement> hoverBorders;
                            CollectTrayHoverBorders(currentTray,
                                                    hoverBorders);
                            double hoverHeight =
                                std::max(1.0, contentHeight - 14.0);
                            for (auto const& element : hoverBorders) {
                                auto border = element.try_as<Border>();
                                if (!border ||
                                    std::isnan(border.Height()) ||
                                    border.Height() <= 0 ||
                                    border.Height() > hoverHeight + 4.0 ||
                                    border.VerticalAlignment() !=
                                        VerticalAlignment::Center) {
                                    needsApply = true;
                                    break;
                                }
                            }

                            if (!needsApply) {
                                double rowHeight = currentTray.ActualHeight();
                                if (!(rowHeight > 0)) {
                                    rowHeight = barHeight;
                                }
                                std::vector<FrameworkElement> omniButtons;
                                CollectTrayOmniButtons(currentTray,
                                                       omniButtons);
                                for (auto const& element : omniButtons) {
                                    double buttonHeight =
                                        element.ActualHeight();
                                    if (!(buttonHeight > 0)) {
                                        buttonHeight = element.Height();
                                    }
                                    double expectedOffset = std::max(
                                        0.0, (rowHeight - buttonHeight) / 2.0);
                                    Thickness margin = element.Margin();
                                    if (element.VerticalAlignment() !=
                                            VerticalAlignment::Top ||
                                        std::abs(margin.Top -
                                                 expectedOffset) > 0.5 ||
                                        std::abs(margin.Bottom) > 0.5) {
                                        needsApply = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                if (!needsApply) {
                    std::vector<FrameworkElement> hoverBackgrounds;
                    if (auto appliedRootGrid = applied.rootGrid.get()) {
                        CollectHoverBackgroundBorders(appliedRootGrid,
                                                      hoverBackgrounds);
                    }
                    if (auto appliedTray = applied.tray.get()) {
                        CollectHoverBackgroundBorders(appliedTray,
                                                      hoverBackgrounds);
                    }
                    double expectedHoverRadius =
                        scale > 0
                            ? g_settingHoverCornerRadius.load() / scale
                            : 0.0;
                    for (auto const& element : hoverBackgrounds) {
                        auto border = element.try_as<Border>();
                        if (!border) {
                            continue;
                        }
                        CornerRadius radius = border.CornerRadius();
                        if (std::abs(radius.TopLeft -
                                     expectedHoverRadius) > 0.5 ||
                            std::abs(radius.TopRight -
                                     expectedHoverRadius) > 0.5 ||
                            std::abs(radius.BottomRight -
                                     expectedHoverRadius) > 0.5 ||
                            std::abs(radius.BottomLeft -
                                     expectedHoverRadius) > 0.5) {
                            needsApply = true;
                            break;
                        }
                    }
                }

                if (!needsApply) {
                    if (!applied.hasAppliedRegion) {
                        if (applied.regionRetries < 20) {
                            needsApply = true;
                        }
                    } else if (applied.topLevelWnd &&
                               IsWindow(applied.topLevelWnd)) {
                        RECT currentRegionBox{};
                        if (GetWindowRgnBox(applied.topLevelWnd,
                                            &currentRegionBox) == ERROR) {
                            needsApply = true;
                        } else if (!EqualRect(&currentRegionBox,
                                              &applied.appliedRegion)) {
                            RECT topLevelRect{};
                            if (GetWindowRect(applied.topLevelWnd,
                                              &topLevelRect)) {
                                RECT fullRegionBox{
                                    0, 0,
                                    topLevelRect.right - topLevelRect.left,
                                    topLevelRect.bottom - topLevelRect.top};
                                if (EqualRect(&currentRegionBox,
                                              &fullRegionBox)) {
                                    needsApply = true;
                                }
                            }
                        }
                    }
                }

                if (needsApply &&
                    (GetTickCount() - applied.lastApplyTick) < 500) {
                    needsApply = false;
                }

                break;
            }
        }

        if (needsApply) {
            ApplyToFrame(frame);
            if (needsHeightKick) {
                KickTaskbarHeightUpdate();
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"HandleFrameEvent failed: %08X",
               static_cast<unsigned int>(ex.code()));
    } catch (...) {
    }
}

XamlRoot GetXamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    if (!taskbarHostSharedPtr[0]) {
        if (RefCountDecref_Original) {
            RefCountDecref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

    const BYTE* b = (const BYTE*)g_taskbarHostFrameHeight;
    if (b && b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
        b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
        taskbarElementIUnknownOffset = b[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        if (RefCountDecref_Original) {
            RefCountDecref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    }

    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (RefCountDecref_Original) {
        RefCountDecref_Original(taskbarHostSharedPtr[1]);
    }

    return result;
}

XamlRoot GetXamlRootForTaskbarWindow(HWND taskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original || !g_taskbarHostFrameHeight) {
        return nullptr;
    }

    WCHAR className[64];
    if (!GetClassName(taskbarWnd, className, ARRAYSIZE(className))) {
        return nullptr;
    }

    bool secondary = _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;

    HWND hTaskSwitchWnd = nullptr;
    void* vftable = nullptr;

    if (secondary) {
        if (!g_csecondaryTaskBandTaskListWndSiteVftable ||
            !CSecondaryTaskBand_GetTaskbarHost_Original) {
            return nullptr;
        }
        hTaskSwitchWnd =
            FindWindowExW(taskbarWnd, nullptr, L"WorkerW", nullptr);
        vftable = g_csecondaryTaskBandTaskListWndSiteVftable;
    } else {
        if (!g_ctaskBandTaskListWndSiteVftable) {
            return nullptr;
        }
        hTaskSwitchWnd = (HWND)GetProp(taskbarWnd, L"TaskbandHWND");
        vftable = g_ctaskBandTaskListWndSiteVftable;
    }

    if (!hTaskSwitchWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwitchWnd, 0);
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    bool found = false;
    for (int i = 0; i < 20; i++) {
        if (*(void**)taskBandForTaskListWndSite == vftable) {
            found = true;
            break;
        }
        taskBandForTaskListWndSite =
            (void**)((BYTE*)taskBandForTaskListWndSite + sizeof(void*));
    }

    if (!found) {
        return nullptr;
    }

    void* taskbarHostSharedPtr[2]{};
    if (secondary) {
        CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                                   taskbarHostSharedPtr);
    } else {
        CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                          taskbarHostSharedPtr);
    }

    return GetXamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

FrameworkElement FindTaskbarFrameInXamlRoot(XamlRoot const& xamlRoot) {
    if (!xamlRoot) {
        return nullptr;
    }

    FrameworkElement contentElement = nullptr;
    try {
        contentElement = xamlRoot.Content().try_as<FrameworkElement>();
    } catch (...) {
    }

    if (!contentElement) {
        return nullptr;
    }

    return FindChildByPredicate(contentElement,
                                [](FrameworkElement const& element) {
                                    return ClassNameEquals(
                                        winrt::get_class_name(element),
                                        L"Taskbar.TaskbarFrame");
                                });
}

void AppendUtf8Line(FILE* file, std::wstring const& line) {
    if (!file) {
        return;
    }
    int size = WideCharToMultiByte(CP_UTF8, 0, line.c_str(), -1, nullptr, 0,
                                   nullptr, nullptr);
    if (size <= 1) {
        return;
    }
    std::string utf8(static_cast<size_t>(size - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, line.c_str(), -1, utf8.data(), size,
                        nullptr, nullptr);
    fwrite(utf8.data(), 1, utf8.size(), file);
    fwrite("\r\n", 1, 2, file);
}

void DumpHoverElement(FILE* file,
                      FrameworkElement const& root,
                      FrameworkElement const& element,
                      winrt::Windows::Foundation::Rect const& trayRect,
                      int depth,
                      int& count) {
    if (!file || !element || depth > 30 || count >= 1500) {
        return;
    }

    winrt::Windows::Foundation::Rect rect{};
    bool hasRect = GetElementRectDip(element, root, rect);
    bool nearTray =
        hasRect && rect.X + rect.Width >= trayRect.X - 120.0f &&
        rect.X <= trayRect.X + trayRect.Width + 120.0f &&
        rect.Y + rect.Height >= trayRect.Y - 120.0f &&
        rect.Y <= trayRect.Y + trayRect.Height + 120.0f;
    auto name = element.Name();
    bool namedBackground =
        name == L"BackgroundElement" || name == L"BackgroundBorder";
    if (!nearTray && !namedBackground) {
        int childCount = 0;
        try {
            childCount = VisualTreeHelper::GetChildrenCount(element);
        } catch (...) {
        }
        for (int i = 0; i < childCount && count < 1500; i++) {
            try {
                auto child = VisualTreeHelper::GetChild(element, i)
                                 .try_as<FrameworkElement>();
                if (child) {
                    DumpHoverElement(file, root, child, trayRect, depth + 1,
                                     count);
                }
            } catch (...) {
            }
        }
        return;
    }

    count++;
    auto className = winrt::get_class_name(element);
    Thickness margin = element.Margin();
    auto translation = element.Translation();
    auto renderTransform = element.RenderTransform();
    std::wstring transformText = L"null";
    if (renderTransform) {
        auto transformClass = winrt::get_class_name(renderTransform);
        transformText = transformClass.c_str();
        if (auto scaleTransform = renderTransform.try_as<ScaleTransform>()) {
            WCHAR buf[256];
            swprintf_s(buf, L"%s(sx=%.3f sy=%.3f cx=%.3f cy=%.3f)",
                       transformClass.c_str(), scaleTransform.ScaleX(),
                       scaleTransform.ScaleY(), scaleTransform.CenterX(),
                       scaleTransform.CenterY());
            transformText = buf;
        } else if (auto translateTransform =
                       renderTransform.try_as<TranslateTransform>()) {
            WCHAR buf[256];
            swprintf_s(buf, L"%s(x=%.3f y=%.3f)", transformClass.c_str(),
                       translateTransform.X(), translateTransform.Y());
            transformText = buf;
        } else if (auto compositeTransform =
                       renderTransform.try_as<CompositeTransform>()) {
            WCHAR buf[256];
            swprintf_s(
                buf, L"%s(sx=%.3f sy=%.3f tx=%.3f ty=%.3f cx=%.3f cy=%.3f)",
                transformClass.c_str(), compositeTransform.ScaleX(),
                compositeTransform.ScaleY(), compositeTransform.TranslateX(),
                compositeTransform.TranslateY(),
                compositeTransform.CenterX(), compositeTransform.CenterY());
            transformText = buf;
        } else if (auto matrixTransform =
                       renderTransform.try_as<MatrixTransform>()) {
            WCHAR buf[256];
            auto matrix = matrixTransform.Matrix();
            swprintf_s(
                buf,
                L"%s(m11=%.3f m12=%.3f m21=%.3f m22=%.3f ox=%.3f oy=%.3f)",
                transformClass.c_str(), matrix.M11, matrix.M12, matrix.M21,
                matrix.M22, matrix.OffsetX, matrix.OffsetY);
            transformText = buf;
        }
    }

    auto visual = ElementCompositionPreview::GetElementVisual(element);
    auto scale = visual.Scale();
    auto offset = visual.Offset();
    auto visualSize = visual.Size();
    auto centerPoint = visual.CenterPoint();

    WCHAR line[2048];
    swprintf_s(
        line,
        L"depth=%d class=%s name=%s rect=%.1f,%.1f,%.1f,%.1f "
        L"actual=%.1f,%.1f vis=%d opacity=%.2f hit=%d margin=%.1f,%.1f,"
        L"%.1f,%.1f align=%d/%d translation=%.2f,%.2f,%.2f transform=%s "
        L"origin=%.2f,%.2f visualScale=%.3f,%.3f,%.3f "
        L"visualOffset=%.2f,%.2f,%.2f visualSize=%.2f,%.2f "
        L"visualCenter=%.2f,%.2f,%.2f visualVisible=%d",
        depth, className.c_str(), name.c_str(), rect.X, rect.Y,
        rect.Width, rect.Height, element.ActualWidth(), element.ActualHeight(),
        static_cast<int>(element.Visibility()), element.Opacity(),
        element.IsHitTestVisible() ? 1 : 0, margin.Left, margin.Top,
        margin.Right, margin.Bottom,
        static_cast<int>(element.HorizontalAlignment()),
        static_cast<int>(element.VerticalAlignment()), translation.x,
        translation.y, translation.z, transformText.c_str(),
        element.RenderTransformOrigin().X, element.RenderTransformOrigin().Y,
        scale.x, scale.y, scale.z, offset.x, offset.y, offset.z,
        visualSize.x, visualSize.y, centerPoint.x, centerPoint.y,
        centerPoint.z, visual.IsVisible() ? 1 : 0);
    AppendUtf8Line(file, line);

    FrameworkElement parent =
        VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    for (int i = 0; i < 5 && parent; i++) {
        auto parentClass = winrt::get_class_name(parent);
        auto parentName = parent.Name();
        swprintf_s(line, L"  parent%d class=%s name=%s actual=%.1f,%.1f",
                   i + 1, parentClass.c_str(), parentName.c_str(),
                   parent.ActualWidth(), parent.ActualHeight());
        AppendUtf8Line(file, line);
        parent =
            VisualTreeHelper::GetParent(parent).try_as<FrameworkElement>();
    }

    int childCount = 0;
    try {
        childCount = VisualTreeHelper::GetChildrenCount(element);
    } catch (...) {
    }
    for (int i = 0; i < childCount && count < 1500; i++) {
        try {
            auto child = VisualTreeHelper::GetChild(element, i)
                             .try_as<FrameworkElement>();
            if (child) {
                DumpHoverElement(file, root, child, trayRect, depth + 1, count);
            }
        } catch (...) {
        }
    }
}

void DumpHoverState(FrameworkElement const& root,
                    FrameworkElement const& tray) {
    if (g_unloading || !root || !tray) {
        return;
    }

    winrt::Windows::Foundation::Rect trayRect{};
    if (!GetElementRectDip(tray, root, trayRect)) {
        return;
    }

    WCHAR tempPath[MAX_PATH];
    if (!GetTempPathW(MAX_PATH, tempPath)) {
        return;
    }
    WCHAR dumpPath[MAX_PATH];
    swprintf_s(dumpPath, L"%staskbar-hover-state-dump.txt", tempPath);

    FILE* file = nullptr;
    _wfopen_s(&file, dumpPath, L"wb");
    if (!file) {
        return;
    }

    const unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    fwrite(bom, 1, sizeof(bom), file);

    WCHAR header[1024];
    swprintf_s(header,
               L"tray rect=%.1f,%.1f,%.1f,%.1f hoverHeight=%.1f rowHeight=%.1f",
               trayRect.X, trayRect.Y, trayRect.Width, trayRect.Height,
               g_trayHoverHeightDip.load(), g_trayRowHeightDip.load());
    AppendUtf8Line(file, header);

    int count = 0;
    try {
        DumpHoverElement(file, root, root, trayRect, 0, count);
    } catch (...) {
    }
    fclose(file);
    Wh_Log(L"Wrote hover state dump: %s", dumpPath);
}

void HoverWatchdogOnCurrentThread(PVOID param) {
    if (g_unloading || !param) {
        return;
    }

    POINT cursor = *static_cast<POINT*>(param);
    HWND taskbarWnd = GetTaskbarWindowForCurrentThread();
    if (!taskbarWnd) {
        return;
    }

    XamlRoot xamlRoot = GetXamlRootForTaskbarWindow(taskbarWnd);
    FrameworkElement frame =
        FindTaskbarFrameInXamlRoot(xamlRoot);
    if (!frame) {
        return;
    }
    FrameworkElement tray = FindSiblingTray(frame);
    if (!tray) {
        return;
    }
    FrameworkElement root = nullptr;
    try {
        root = xamlRoot.Content().try_as<FrameworkElement>();
    } catch (...) {
    }
    if (!root) {
        return;
    }

    winrt::Windows::Foundation::Rect trayRect{};
    if (!GetElementRectDip(tray, root, trayRect)) {
        return;
    }

    RECT windowRect{};
    if (!GetWindowRect(taskbarWnd, &windowRect)) {
        return;
    }
    double scale = GetEffectiveScale(frame, taskbarWnd);
    RECT trayScreenRect{
        windowRect.left + static_cast<LONG>(std::lround(trayRect.X * scale)),
        windowRect.top + static_cast<LONG>(std::lround(trayRect.Y * scale)),
        windowRect.left +
            static_cast<LONG>(std::lround((trayRect.X + trayRect.Width) *
                                          scale)),
        windowRect.top +
            static_cast<LONG>(std::lround((trayRect.Y + trayRect.Height) *
                                          scale))};
    bool inside = PtInRect(&trayScreenRect, cursor) != FALSE;

    static bool s_inside = false;
    static DWORD s_enterTick = 0;
    static bool s_dumped = false;
    if (!inside) {
        s_inside = false;
        s_dumped = false;
    } else if (!s_inside) {
        s_inside = true;
        s_enterTick = GetTickCount();
        s_dumped = false;
    }

    std::vector<FrameworkElement> hoverBorders;
    CollectTrayHoverBorders(tray, hoverBorders);
    double hoverHeight = g_trayHoverHeightDip.load();
    for (auto const& border : hoverBorders) {
        ApplyTrayHoverBorderLayout(border, hoverHeight);
    }
    std::vector<FrameworkElement> omniButtons;
    CollectTrayOmniButtons(tray, omniButtons);
    for (auto const& button : omniButtons) {
        if (GetFixedTrayOmniButtonHeight(button) > 0) {
            ApplyTrayOmniButtonLayout(button, g_trayRowHeightDip.load());
        }
    }
    NormalizeTrayHoverTransformsRecursive(tray);

    if (inside && !s_dumped &&
        GetTickCount() - s_enterTick >= 100) {
        s_dumped = true;
        try {
            DumpHoverState(root, tray);
        } catch (...) {
        }
    }
}

bool RunFromWindowThread(HWND hWnd, void (*proc)(PVOID), PVOID procParam);
void ForEachTaskbarWindow(void (*proc)(HWND));

DWORD WINAPI HoverWatchdogProc(LPVOID) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    DWORD lastMaintenanceTick = 0;
    while (!g_hoverWatchdogStop.load() && !g_unloading) {
        DWORD now = GetTickCount();
        if (g_settingTaskbarAreaHeight.load() > 0 &&
            now - lastMaintenanceTick >= 100) {
            lastMaintenanceTick = now;
            ForEachTaskbarWindow([](HWND hWnd) {
                POINT cursor{};
                GetCursorPos(&cursor);
                RunFromWindowThread(hWnd, HoverWatchdogOnCurrentThread,
                                    &cursor);
            });
        }

        Sleep(50);
    }
    return 0;
}

void StartHoverWatchdog() {
    if (g_hoverWatchdogThread || g_hostKind != HostKind::Taskbar) {
        return;
    }
    g_hoverWatchdogStop = false;
    g_hoverWatchdogThread =
        CreateThread(nullptr, 0, HoverWatchdogProc, nullptr, 0, nullptr);
}

void StopHoverWatchdog() {
    g_hoverWatchdogStop = true;
    if (g_hoverWatchdogThread) {
        WaitForSingleObject(g_hoverWatchdogThread, 3000);
        CloseHandle(g_hoverWatchdogThread);
        g_hoverWatchdogThread = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Tray system icon tweaks, adapted from the "Taskbar tray system icon tweaks"
// mod (taskbar-tray-system-icon-tweaks).
//
// A tray icon is identified by the glyph code point of the icon font that is
// used for its text, and then hidden (or restyled) by adjusting the visual
// tree. Icon views are hooked at creation, and dynamic icons (microphone,
// bell, ...) are re-evaluated when their glyph or name changes.
// ---------------------------------------------------------------------------

FrameworkElement EnumParentElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (enumCallback(parent)) {
            return parent;
        }
    }
}

FrameworkElement GetParentElementByName(FrameworkElement element, PCWSTR name) {
    return EnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!GetParentElementByName(element, name);
}

FrameworkElement GetParentElementByClassName(FrameworkElement element,
                                             PCWSTR className) {
    return EnumParentElements(element, [className](FrameworkElement parent) {
        return winrt::get_class_name(parent) == className;
    });
}

bool IsChildOfElementByClassName(FrameworkElement element, PCWSTR className) {
    return !!GetParentElementByClassName(element, className);
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// https://stackoverflow.com/a/3382894
std::wstring StringToHex(std::wstring_view input) {
    static const WCHAR kHexDigits[] = L"0123456789ABCDEF";

    std::wstring output;
    output.reserve(input.length() * 5);
    for (WCHAR c : input) {
        output.push_back(kHexDigits[(c >> 12) & 15]);
        output.push_back(kHexDigits[(c >> 8) & 15]);
        output.push_back(kHexDigits[(c >> 4) & 15]);
        output.push_back(kHexDigits[c & 15]);
        output.push_back(L' ');
    }

    if (!output.empty() && output.back() == L' ') {
        output.resize(output.length() - 1);
    }

    return output;
}

enum class SystemTrayIconIdent {
    kUnknown,
    kNone,
    kVolume,
    kNetwork,
    kBattery,
    kMicrophone,
    kGeolocation,
    kMicrophoneAndGeolocation,
    kBellEmpty,
    kBellEmptyDnd,
    kBellFull,
    kBellFullDnd,
    kLanguage,
    kStudioEffects,
    kRecall,
};

SystemTrayIconIdent IdentifySystemTrayIconFromText(std::wstring_view text) {
    switch (text.length()) {
        case 0:
            return SystemTrayIconIdent::kNone;

        case 1:
            break;

        default:
            return SystemTrayIconIdent::kUnknown;
    }

    switch (text[0]) {
        case L'\uE74F':  // Mute
        case L'\uE992':  // Volume0
        case L'\uE993':  // Volume1
        case L'\uE994':  // Volume2
        case L'\uE995':  // Volume3
        case L'\uEA85':  // VolumeDisabled
        case L'\uEBC5':  // VolumeBars
            return SystemTrayIconIdent::kVolume;

        case L'\uE709':  // Airplane
        case L'\uE7F4':  // TVMonitor
        case L'\uE839':  // Ethernet
        case L'\uE86C':  // SignalBars1
        case L'\uE86D':  // SignalBars2
        case L'\uE86E':  // SignalBars3
        case L'\uE86F':  // SignalBars4
        case L'\uE870':  // SignalBars5
        case L'\uEAA1':  // Drinks
        case L'\uEAA2':  // DropShot
        case L'\uEAA3':  // FlavorProfile
        case L'\uEAA4':  // Float
        case L'\uEAA5':  // FluteGlass
        case L'\uEAA8':  // GobletGlass
        case L'\uEC1E':  // SignalRoaming
        case L'\uEC3C':  // MobWifi1
        case L'\uEC3D':  // MobWifi2
        case L'\uEC3E':  // MobWifi3
        case L'\uEC3F':  // MobWifi4
        case L'\uF384':  // NetworkOffline
        case L'\uF8C0':  // SysLocationArrow
        case L'\uF8C1':  // SysMicrophone
        case L'\uF8C2':  // SysVideo
        case L'\uF8C3':  // SysWifi4
        case L'\uF8C4':  // SysWifi3
        case L'\uF8C5':  // SysWifi2
        case L'\uF8C6':  // SysWifi1
        case L'\uF8C7':  // SysSignalBars5
        case L'\uF8C8':  // SysSignalBars4
        case L'\uF8C9':  // SysSignalBars3
        case L'\uF8CA':  // SysSignalBars2
        case L'\uF8CB':  // SysSignalBars1
        case L'\uF8CC':  // SysNetworkOffline
            return SystemTrayIconIdent::kNetwork;

        // Charging levels.
        case L'\uE3C1':  // Private Use
        case L'\uE3C2':  // Private Use
        case L'\uE3C3':  // Private Use
        case L'\uE3C4':  // Private Use
        case L'\uE3C5':  // Private Use
        case L'\uE3C6':  // Private Use
        case L'\uE3C7':  // Private Use
        case L'\uE3C8':  // Private Use
        case L'\uE3C9':  // Private Use
        case L'\uE3CA':  // Private Use
        case L'\uE3CB':  // Private Use
        case L'\uE408':  // Private Use
        case L'\uE409':  // Private Use
        case L'\uE40A':  // Private Use
        case L'\uE40B':  // Private Use
        case L'\uE40C':  // Private Use
        case L'\uE40D':  // Private Use
        case L'\uE40E':  // Private Use
        case L'\uE40F':  // Private Use
        case L'\uE410':  // Private Use
        case L'\uE411':  // Private Use
        case L'\uE412':  // Private Use
        case L'\uE413':  // Private Use
        case L'\uE414':  // Private Use
        case L'\uE415':  // Private Use
        case L'\uE416':  // Private Use
        case L'\uE417':  // Private Use
        case L'\uE418':  // Private Use
        case L'\uE419':  // Private Use
        case L'\uE41A':  // Private Use
        case L'\uE41B':  // Private Use
        case L'\uE41C':  // Private Use
        case L'\uE41D':  // Private Use
        case L'\uEBA0':  // MobBattery0
        case L'\uEBA1':  // MobBattery1
        case L'\uEBA2':  // MobBattery2
        case L'\uEBA3':  // MobBattery3
        case L'\uEBA4':  // MobBattery4
        case L'\uEBA5':  // MobBattery5
        case L'\uEBA6':  // MobBattery6
        case L'\uEBA7':  // MobBattery7
        case L'\uEBA8':  // MobBattery8
        case L'\uEBA9':  // MobBattery9
        case L'\uEBAA':  // MobBattery10
        case L'\uEBAB':  // MobBatteryCharging0
        case L'\uEBAC':  // MobBatteryCharging1
        case L'\uEBAD':  // MobBatteryCharging2
        case L'\uEBAE':  // MobBatteryCharging3
        case L'\uEBAF':  // MobBatteryCharging4
        case L'\uEBB0':  // MobBatteryCharging5
        case L'\uEBB1':  // MobBatteryCharging6
        case L'\uEBB2':  // MobBatteryCharging7
        case L'\uEBB3':  // MobBatteryCharging8
        case L'\uEBB4':  // MobBatteryCharging9
        case L'\uEBB5':  // MobBatteryCharging10
        case L'\uEBB6':  // MobBatterySaver0
        case L'\uEBB7':  // MobBatterySaver1
        case L'\uEBB8':  // MobBatterySaver2
        case L'\uEBB9':  // MobBatterySaver3
        case L'\uEBBA':  // MobBatterySaver4
        case L'\uEBBB':  // MobBatterySaver5
        case L'\uEBBC':  // MobBatterySaver6
        case L'\uEBBD':  // MobBatterySaver7
        case L'\uEBBE':  // MobBatterySaver8
        case L'\uEBBF':  // MobBatterySaver9
        case L'\uEBC0':  // MobBatterySaver10
        // Misc.
        case L'\uEB17':
        case L'\uEC02':
        case L'\uF1E8':
            return SystemTrayIconIdent::kBattery;

        case L'\uE361':  // Private Use
        case L'\uE720':  // Microphone
        case L'\uEC71':  // MicOn
            return SystemTrayIconIdent::kMicrophone;

        case L'\uE37A':
            return SystemTrayIconIdent::kGeolocation;

        case L'\uF47F':
            return SystemTrayIconIdent::kMicrophoneAndGeolocation;

        case L'\uF2A3':  // Empty bell
            return SystemTrayIconIdent::kBellEmpty;

        case L'\uF285':  // Empty bell, Do Not Disturb
            return SystemTrayIconIdent::kBellEmptyDnd;

        case L'\uF2A5':  // Full bell
            return SystemTrayIconIdent::kBellFull;

        case L'\uF2A8':  // Full bell, Do Not Disturb
            return SystemTrayIconIdent::kBellFullDnd;

        // Language supplementary icons.
        // Found by installing all the built-in input methods from:
        // https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-language-pack-default-values?view=windows-11#input-method-editors
        // and identifying the icon code in the fonts Segoe Fluent and
        // AXPIcons.ttf.
        case L'\uE4D7':  // (Maybe) English Private mode
        case L'\uE4D8':  // (Maybe) Chinese Private mode
        case L'\uE5BF':  // (Maybe) English mode locked
        case L'\uE97E':  // HalfAlpha
        case L'\uE97F':  // FullAlpha
        case L'\uE980':  // Key12On (Korean mode)
        case L'\uE982':  // QWERTYOn (Chinese mode)
        case L'\uE983':  // QWERTYOff (English mode)
        case L'\uE986':  // FullHiragana
        case L'\uE987':  // FullKatakana
        case L'\uE988':  // HalfKatakana
        case L'\uEB90':  // StatusErrorFull (Input method disabled)
        case L'\uEE41':  // FullHiraganaPrivateMode
        case L'\uEE42':  // FullKatakanaPrivateMode
        case L'\uEE43':  // HalfAlphaPrivateMode
        case L'\uEE44':  // HalfKatakanaPrivateMode
        case L'\uEE45':  // FullAlphaPrivateMode
        case L'\uEE75':  // (Maybe) HalfAlpha
        case L'\uEE76':  // (Maybe) HalfAlphaPrivateMode
            return SystemTrayIconIdent::kLanguage;

        case L'\uEABC':
            return SystemTrayIconIdent::kStudioEffects;

        // From:
        // C:\Windows\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\SystemTray\Assets\RecallIcons.ttf
        case L'\uEC83':
        case L'\uEADD':
        case L'\uEB16':
        case L'\uEF97':
        case L'\uF1C6':
            return SystemTrayIconIdent::kRecall;

        default:
            return SystemTrayIconIdent::kUnknown;
    }
}

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

[[clang::no_destroy]] std::optional<
    std::list<FrameworkElementLoadedEventRevoker>> g_autoRevokerList{
    std::in_place};

winrt::weak_ref<Controls::TextBlock> g_mainStackInnerTextBlock;
int64_t g_mainStackTextChangedToken;

winrt::weak_ref<FrameworkElement> g_bellSystemTrayIconElement;
int64_t g_bellAutomationNameChangedToken;

winrt::weak_ref<FrameworkElement> g_controlCenterStackPanel;
winrt::event_token g_controlCenterStackPanelSizeChangedToken;

struct BatteryTextBlockState {
    winrt::weak_ref<Controls::TextBlock> textBlock;
    Media::Brush savedForeground{nullptr};
    int64_t foregroundChangedToken{0};
};

[[clang::no_destroy]] std::optional<std::vector<BatteryTextBlockState>>
    g_batteryTextBlockStates{std::in_place};

// Local MinWidth/MaxWidth values replaced by the show desktop width override,
// nullopt for a property that had no local value.
struct ShowDesktopWidthState {
    winrt::weak_ref<FrameworkElement> element;
    std::optional<double> savedMinWidth;
    std::optional<double> savedMaxWidth;
};

std::vector<ShowDesktopWidthState> g_showDesktopWidthStates;

void ApplyMainStackIconViewStyle(FrameworkElement notifyIconViewElement) {
    FrameworkElement systemTrayTextIconContent = nullptr;

    FrameworkElement child = notifyIconViewElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        systemTrayTextIconContent = child;
    } else {
        Wh_Log(L"Failed to get SystemTray.TextIconContent");
        return;
    }

    Controls::TextBlock innerTextBlock = nullptr;

    child = systemTrayTextIconContent;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"Base")) &&
        (child = FindChildByName(child, L"InnerTextBlock"))) {
        innerTextBlock = child.as<Controls::TextBlock>();
    } else {
        Wh_Log(L"Failed to get InnerTextBlock");
        return;
    }

    auto shouldHide = [](Controls::TextBlock innerTextBlock) {
        auto text = innerTextBlock.Text();
        auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

        bool hide = false;
        if (!g_unloading) {
            switch (systemTrayIconIdent) {
                case SystemTrayIconIdent::kMicrophone:
                    hide = g_settingHideMicrophoneIcon.load();
                    break;

                case SystemTrayIconIdent::kGeolocation:
                    hide = g_settingHideGeolocationIcon.load();
                    break;

                case SystemTrayIconIdent::kMicrophoneAndGeolocation:
                    hide = g_settingHideMicrophoneIcon.load() &&
                           g_settingHideGeolocationIcon.load();
                    break;

                case SystemTrayIconIdent::kStudioEffects:
                    hide = g_settingHideStudioEffectsIcon.load();
                    break;

                case SystemTrayIconIdent::kRecall:
                    hide = g_settingHideRecallIcon.load();
                    break;

                case SystemTrayIconIdent::kNone:
                    // Happens when the icon is about to disappear.
                    break;

                default:
                    Wh_Log(L"Failed");
                    break;
            }
        }

        Wh_Log(L"Main stack icon %d (%s), hide=%d", (int)systemTrayIconIdent,
               StringToHex(text).c_str(), hide);

        return hide;
    };

    bool hide = shouldHide(innerTextBlock);

    notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                          : Visibility::Visible);

    if (!g_unloading && !g_mainStackInnerTextBlock.get()) {
        auto notifyIconViewElementWeakRef =
            winrt::make_weak(notifyIconViewElement);
        g_mainStackInnerTextBlock = innerTextBlock;
        g_mainStackTextChangedToken =
            innerTextBlock.RegisterPropertyChangedCallback(
                Controls::TextBlock::TextProperty(),
                [notifyIconViewElementWeakRef, shouldHide](
                    DependencyObject sender, DependencyProperty property) {
                    try {
                        auto innerTextBlock =
                            sender.try_as<Controls::TextBlock>();
                        if (!innerTextBlock) {
                            return;
                        }

                        auto notifyIconViewElement =
                            notifyIconViewElementWeakRef.get();
                        if (!notifyIconViewElement) {
                            return;
                        }

                        bool hide = shouldHide(innerTextBlock);

                        notifyIconViewElement.Visibility(
                            hide ? Visibility::Collapsed
                                 : Visibility::Visible);
                    } catch (...) {
                    }
                });
    }
}

void ApplyNonActivatableStackIconViewStyle(
    FrameworkElement notifyIconViewElement) {
    FrameworkElement child = notifyIconViewElement;
    bool hide = false;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid"))) {
        child = EnumChildElements(child, [&hide](FrameworkElement child) {
            auto className = winrt::get_class_name(child);
            if (className == L"SystemTray.TextIconContent") {
                Controls::TextBlock innerTextBlock = nullptr;

                if ((child = FindChildByName(child, L"ContainerGrid")) &&
                    (child = FindChildByName(child, L"Base")) &&
                    (child = FindChildByName(child, L"InnerTextBlock"))) {
                    innerTextBlock = child.as<Controls::TextBlock>();
                } else {
                    Wh_Log(L"Failed to get InnerTextBlock");
                    return false;
                }

                auto text = innerTextBlock.Text();
                auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

                if (systemTrayIconIdent == SystemTrayIconIdent::kLanguage) {
                    Wh_Log(L"Language supplementary icon %d (%s)",
                           (int)systemTrayIconIdent, StringToHex(text).c_str());
                    hide = g_settingHideLanguageSupplementaryIcons.load();
                    return true;
                } else {
                    Wh_Log(L"Language bar unknown icon %d (%s)",
                           (int)systemTrayIconIdent, StringToHex(text).c_str());
                    return false;
                }
            } else if (className == L"SystemTray.ImageIconContent") {
                Wh_Log(L"Language supplementary icon (image)");
                hide = g_settingHideLanguageSupplementaryIcons.load();
                return true;
            } else if (className == L"SystemTray.LanguageTextIconContent" ||
                       className == L"SystemTray.LanguageImageIconContent") {
                Wh_Log(L"Language bar main icon");
                hide = g_settingHideLanguageBar.load();
                return true;
            }

            Wh_Log(L"Unsupported class name %s of child", className.c_str());
            return false;
        });
    }

    if (!child) {
        // Some input methods use LanguageImageIconContent/ImageIconContent
        // instead of LanguageTextIconContent/TextIconContent with icon fonts.
        // If the language bar is hidden and the user switches from a "text"
        // input method to a "image" input method, the invisible element will
        // not be populated with the new type of icon content but become empty
        // instead. Then the icon will be permanently hidden even after
        // disabling the mod. This code forces the empty element to become
        // visible and populated, fixing this issue.
        if (Media::VisualTreeHelper::GetChildrenCount(notifyIconViewElement) ==
            0) {
            notifyIconViewElement.Visibility(Visibility::Visible);
        }
        return;
    }

    hide = !g_unloading && hide;

    notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                          : Visibility::Visible);
}

// Path inside SystemTray.BatteryIconContent:
// Grid#ContainerGrid > StackPanel > Grid > TextBlock.
//
// The system sets a colored Foreground brush on each battery TextBlock.
// Clearing the local Foreground value lets the inherited TextFillColorPrimary
// take over, giving a grayscale look. The system re-applies the colored brush
// on theme changes (and similar updates), so we listen for ForegroundProperty
// changes, save the latest value (so we can restore it on disable), and clear
// it again.
void ApplyBatteryIconGrayscaleStyle(FrameworkElement batteryIconContent) {
    bool grayscale = !g_unloading && g_settingGrayscaleBatteryIcon.load();

    // Drop any state for TextBlocks that no longer exist.
    std::erase_if(*g_batteryTextBlockStates,
                  [](const BatteryTextBlockState& s) {
                      return !s.textBlock.get();
                  });

    // Nothing to apply and nothing to restore - skip the tree traversal.
    if (!grayscale && g_batteryTextBlockStates->empty()) {
        return;
    }

    FrameworkElement grid = nullptr;

    FrameworkElement child = batteryIconContent;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel")) &&
        (child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid"))) {
        grid = child;
    } else {
        Wh_Log(L"Failed to navigate to battery Grid");
        return;
    }

    EnumChildElements(grid, [grayscale](FrameworkElement textChild) {
        auto textBlock = textChild.try_as<Controls::TextBlock>();
        if (!textBlock) {
            return false;
        }

        auto it = g_batteryTextBlockStates->begin();
        for (; it != g_batteryTextBlockStates->end(); ++it) {
            if (it->textBlock.get() == textBlock) {
                break;
            }
        }
        bool managed = (it != g_batteryTextBlockStates->end());

        if (grayscale && !managed) {
            auto localForeground =
                textBlock
                    .ReadLocalValue(Controls::TextBlock::ForegroundProperty())
                    .try_as<Media::Brush>();
            if (!localForeground) {
                // No local Foreground set, so clearing would be a no-op and
                // there'd be nothing meaningful to restore later.
                return false;
            }

            BatteryTextBlockState state;
            state.textBlock = textBlock;
            state.savedForeground = localForeground;
            state.foregroundChangedToken =
                textBlock.RegisterPropertyChangedCallback(
                    Controls::TextBlock::ForegroundProperty(),
                    [](DependencyObject sender, DependencyProperty) {
                        try {
                            auto tb = sender.try_as<Controls::TextBlock>();
                            if (!tb) {
                                return;
                            }
                            auto fg =
                                tb.ReadLocalValue(
                                      Controls::TextBlock::ForegroundProperty())
                                    .try_as<Media::Brush>();
                            if (!fg) {
                                return;
                            }
                            for (auto& s : *g_batteryTextBlockStates) {
                                if (s.textBlock.get() == tb) {
                                    s.savedForeground = fg;
                                    break;
                                }
                            }
                            tb.as<DependencyObject>().ClearValue(
                                Controls::TextBlock::ForegroundProperty());
                        } catch (...) {
                        }
                    });
            g_batteryTextBlockStates->push_back(std::move(state));
            textBlock.as<DependencyObject>().ClearValue(
                Controls::TextBlock::ForegroundProperty());
        } else if (!grayscale && managed) {
            textBlock.UnregisterPropertyChangedCallback(
                Controls::TextBlock::ForegroundProperty(),
                it->foregroundChangedToken);
            textBlock.Foreground(it->savedForeground);
            g_batteryTextBlockStates->erase(it);
        }
        return false;
    });
}

void SetItemContainerMargin(FrameworkElement container,
                            double left,
                            double right) {
    if (left == 0 && right == 0) {
        container.ClearValue(FrameworkElement::MarginProperty());
    } else {
        container.Margin(Thickness{left, 0, right, 0});
    }
}

// A hidden icon keeps its (empty) item container in the panel, and the panel
// adds Spacing for every child regardless of visibility or size. Cancel the
// extra spacing with negative margins on the neighboring visible items, and
// hide the button if all icons are hidden.
void UpdateControlCenterButtonStackPanel(FrameworkElement stackPanel) {
    double spacing = 0;
    if (auto panel = stackPanel.try_as<Controls::StackPanel>()) {
        spacing = panel.Spacing();
    }

    // Margins are assigned once the next visible item (or the end) is known,
    // so that the trailing hidden items can be folded into the right margin.
    FrameworkElement pendingContainer = nullptr;
    double pendingLeft = 0;
    int hiddenRun = 0;

    EnumChildElements(stackPanel, [&](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        auto systemTrayIconElement = FindChildByName(child, L"SystemTrayIcon")
                                         .try_as<Controls::Control>();
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        if (!systemTrayIconElement.IsEnabled()) {
            hiddenRun++;
            return false;
        }

        if (pendingContainer) {
            SetItemContainerMargin(pendingContainer, pendingLeft, 0);
        }

        pendingContainer = child;
        pendingLeft = -spacing * hiddenRun;
        hiddenRun = 0;
        return false;
    });

    if (pendingContainer) {
        SetItemContainerMargin(pendingContainer, pendingLeft,
                               -spacing * hiddenRun);
    }

    // The button template has some width of its own (e.g. the background
    // border margins), so an empty panel alone doesn't make it disappear.
    FrameworkElement controlCenterButton =
        GetParentElementByName(stackPanel, L"ControlCenterButton");
    if (!controlCenterButton) {
        Wh_Log(L"Failed to get ControlCenterButton");
        return;
    }

    controlCenterButton.Visibility(pendingContainer ? Visibility::Visible
                                                    : Visibility::Collapsed);
}

void ApplyControlCenterButtonIconStyle(FrameworkElement systemTrayIconElement) {
    FrameworkElement contentGrid = nullptr;

    FrameworkElement child = systemTrayIconElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentGrid"))) {
        contentGrid = child;
    } else {
        Wh_Log(L"Failed to get ContentGrid");
        return;
    }

    bool hide = false;
    FrameworkElement systemTrayTextIconContent =
        FindChildByClassName(contentGrid, L"SystemTray.BatteryIconContent");
    if (systemTrayTextIconContent) {
        if (!g_unloading) {
            hide = g_settingHideBatteryIcon.load();
        }

        ApplyBatteryIconGrayscaleStyle(systemTrayTextIconContent);
    } else {
        systemTrayTextIconContent =
            FindChildByClassName(contentGrid, L"SystemTray.TextIconContent");
        if (!systemTrayTextIconContent) {
            Wh_Log(L"Failed to get SystemTray.TextIconContent");
            return;
        }

        Controls::TextBlock innerTextBlock = nullptr;

        child = systemTrayTextIconContent;
        if ((child = FindChildByName(child, L"ContainerGrid")) &&
            (child = FindChildByName(child, L"Base")) &&
            (child = FindChildByName(child, L"InnerTextBlock"))) {
            innerTextBlock = child.as<Controls::TextBlock>();
        } else {
            Wh_Log(L"Failed to get InnerTextBlock");
            return;
        }

        auto text = innerTextBlock.Text();
        auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

        if (!g_unloading) {
            switch (systemTrayIconIdent) {
                case SystemTrayIconIdent::kVolume:
                    hide = g_settingHideVolumeIcon.load();
                    break;

                case SystemTrayIconIdent::kNetwork:
                    hide = g_settingHideNetworkIcon.load();
                    break;

                case SystemTrayIconIdent::kBattery:
                    hide = g_settingHideBatteryIcon.load();
                    break;

                default:
                    Wh_Log(L"Failed");
                    break;
            }
        }

        Wh_Log(L"System tray icon %d (%s), hide=%d", (int)systemTrayIconIdent,
               StringToHex(text).c_str(), hide);
    }

    bool hidden =
        systemTrayTextIconContent.Visibility() == Visibility::Collapsed;
    if (hide != hidden) {
        systemTrayTextIconContent.Visibility(hide ? Visibility::Collapsed
                                                  : Visibility::Visible);
        if (auto control = systemTrayIconElement.try_as<Controls::Control>()) {
            control.IsEnabled(!hide);
        } else {
            Wh_Log(L"Failed");
        }
    }

    // The layout depends on the siblings too, e.g. a newly added visible icon
    // must show the button even if this icon's own state is unchanged.
    FrameworkElement parent = systemTrayIconElement;
    if ((parent = Media::VisualTreeHelper::GetParent(parent)
                      .try_as<FrameworkElement>()) &&
        winrt::get_class_name(parent) ==
            L"Windows.UI.Xaml.Controls.ContentPresenter" &&
        (parent = Media::VisualTreeHelper::GetParent(parent)
                      .try_as<FrameworkElement>()) &&
        winrt::get_class_name(parent) ==
            L"Windows.UI.Xaml.Controls.StackPanel") {
        // Removing an item container changes the panel size, which is the only
        // signal that the margins of the remaining items are stale.
        if (!g_unloading && !g_controlCenterStackPanel.get()) {
            g_controlCenterStackPanel = parent;
            g_controlCenterStackPanelSizeChangedToken = parent.SizeChanged(
                [](winrt::Windows::Foundation::IInspectable const& sender,
                   SizeChangedEventArgs const& e) {
                    try {
                        auto stackPanel = sender.try_as<FrameworkElement>();
                        if (!stackPanel) {
                            Wh_Log(L"Failed to get sender");
                            return;
                        }

                        UpdateControlCenterButtonStackPanel(stackPanel);
                    } catch (...) {
                    }
                });
        }

        UpdateControlCenterButtonStackPanel(parent);
    } else {
        Wh_Log(L"Failed");
    }
}

void ApplyBellIconStyle(FrameworkElement systemTrayIconElement);

// When the clock is hidden, the bell icon behaves differently - its content is
// recreated each time the bell icon changes. Therefore we retry until the
// content is there.
void ApplyBellIconStyleWithRetry(FrameworkElement systemTrayIconElement,
                                 int attempt) {
    if (attempt == 10 || g_unloading) {
        return;
    }

    FrameworkElement containerGrid =
        FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (!containerGrid) {
        systemTrayIconElement.Dispatcher().TryRunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
            [systemTrayIconElement, attempt]() {
                try {
                    ApplyBellIconStyleWithRetry(systemTrayIconElement,
                                                attempt + 1);
                } catch (...) {
                }
            });
        return;
    }

    ApplyBellIconStyle(systemTrayIconElement);
}

void ApplyBellIconStyle(FrameworkElement systemTrayIconElement) {
    FrameworkElement containerGrid =
        FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (!containerGrid) {
        Wh_Log(L"Failed to get ContainerGrid");
        return;
    }

    FrameworkElement systemTrayTextIconContent = nullptr;
    bool hasContentPresenterForMissingClock = true;

    // When the clock is hidden, the element path is:
    //
    // #ContainerGrid > #ContentPresenter > #ContentGrid >
    // SystemTray.TextIconContent
    //
    // When the clock is visible, ContentPresenter is missing:
    //
    // #ContainerGrid > #ContentGrid > SystemTray.TextIconContent
    FrameworkElement child =
        FindChildByName(containerGrid, L"ContentPresenter");
    if (!child) {
        hasContentPresenterForMissingClock = false;
        child = containerGrid;
    }

    if ((child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        systemTrayTextIconContent = child;
    } else {
        return;
    }

    auto contentPresenter =
        Media::VisualTreeHelper::GetParent(systemTrayIconElement)
            .try_as<FrameworkElement>();

    bool hide = false;

    if (!g_unloading) {
        HideBellIcon hideBellIcon = (HideBellIcon)g_settingHideBellIcon.load();
        if (hideBellIcon == HideBellIcon::always) {
            hide = true;
        } else if (hideBellIcon == HideBellIcon::whenInactive ||
                   hideBellIcon == HideBellIcon::whenInactiveAndNoDnd) {
            Controls::TextBlock innerTextBlock = nullptr;

            child = systemTrayTextIconContent;
            if ((child = FindChildByName(child, L"ContainerGrid")) &&
                (child = FindChildByName(child, L"Base")) &&
                (child = FindChildByName(child, L"InnerTextBlock"))) {
                innerTextBlock = child.as<Controls::TextBlock>();
            } else {
                Wh_Log(L"Failed to get InnerTextBlock");
                return;
            }

            auto shouldHide = [](Controls::TextBlock innerTextBlock) {
                auto text = innerTextBlock.Text();
                auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);
                switch (systemTrayIconIdent) {
                    case SystemTrayIconIdent::kBellEmpty:
                        return true;

                    case SystemTrayIconIdent::kBellEmptyDnd:
                        return (HideBellIcon)g_settingHideBellIcon.load() !=
                               HideBellIcon::whenInactiveAndNoDnd;

                    case SystemTrayIconIdent::kBellFull:
                    case SystemTrayIconIdent::kBellFullDnd:
                        return false;

                    default:
                        Wh_Log(L"Unknown bell icon %d (%s)",
                               (int)systemTrayIconIdent,
                               StringToHex(text).c_str());
                        return false;
                }
            };

            hide = shouldHide(innerTextBlock);

            if (!g_bellSystemTrayIconElement.get()) {
                g_bellSystemTrayIconElement = systemTrayIconElement;
                g_bellAutomationNameChangedToken =
                    systemTrayIconElement.RegisterPropertyChangedCallback(
                        Automation::AutomationProperties::NameProperty(),
                        [](DependencyObject sender, DependencyProperty) {
                            try {
                                auto bellSystemTrayIconElement =
                                    sender.try_as<FrameworkElement>();
                                if (!bellSystemTrayIconElement) {
                                    return;
                                }

                                ApplyBellIconStyleWithRetry(
                                    bellSystemTrayIconElement, 0);
                            } catch (...) {
                            }
                        });
            }
        }
    }

    Wh_Log(L"Bell icon, hide=%d", hide);

    systemTrayTextIconContent.Visibility(hide ? Visibility::Collapsed
                                              : Visibility::Visible);

    if (contentPresenter) {
        if (hide && hasContentPresenterForMissingClock) {
            contentPresenter.MaxWidth(0);
        } else {
            contentPresenter.ClearValue(FrameworkElement::MaxWidthProperty());
        }
    }
}

// MinWidth/MaxWidth take precedence over an explicit Width, so they override
// the width without having to replace the Width value itself. The local values
// are saved the first time an element is seen, for restoring in
// RestoreTrayIconStyles.
void OverrideShowDesktopWidth(FrameworkElement element, double width) {
    bool managed = false;
    for (const auto& state : g_showDesktopWidthStates) {
        if (state.element.get() == element) {
            managed = true;
            break;
        }
    }

    if (!managed) {
        g_showDesktopWidthStates.push_back({
            .element = element,
            .savedMinWidth =
                element.ReadLocalValue(FrameworkElement::MinWidthProperty())
                    .try_as<double>(),
            .savedMaxWidth =
                element.ReadLocalValue(FrameworkElement::MaxWidthProperty())
                    .try_as<double>(),
        });
    }

    element.MinWidth(width);
    element.MaxWidth(width);
}

void ApplyShowDesktopStyle(FrameworkElement systemTrayIconElement) {
    if (g_unloading) {
        // Restored in RestoreTrayIconStyles.
        return;
    }

    auto showDesktopStack =
        GetParentElementByName(systemTrayIconElement, L"ShowDesktopStack");
    if (!showDesktopStack) {
        Wh_Log(L"Failed");
        return;
    }

    // Drop any state for elements that no longer exist.
    std::erase_if(g_showDesktopWidthStates, [](const ShowDesktopWidthState& s) {
        return !s.element.get();
    });

    int width = g_settingShowDesktopButtonWidth.load();
    Wh_Log(L"Show desktop button, width=%d", width);

    OverrideShowDesktopWidth(systemTrayIconElement, width);
    OverrideShowDesktopWidth(showDesktopStack, width);

    // The container grid has an explicit width, which keeps its content at the
    // default size regardless of the width of the icon view.
    auto containerGrid =
        FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (containerGrid) {
        OverrideShowDesktopWidth(containerGrid, width);
    } else {
        Wh_Log(L"Failed to get ContainerGrid");
    }
}

bool ApplyMainStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyMainStackIconViewStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyNonActivatableStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyNonActivatableStackIconViewStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyControlCenterButtonStyle(FrameworkElement controlCenterButton) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = controlCenterButton;
    if ((child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyControlCenterButtonIconStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyNotificationCenterButtonStyle(FrameworkElement controlCenterButton) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = controlCenterButton;
    if ((child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyBellIconStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyShowDesktopStackStyle(FrameworkElement container) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplyShowDesktopStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

void ApplyTrayIconStyles(XamlRoot const& xamlRoot) {
    try {
        FrameworkElement systemTrayFrameGrid = nullptr;

        FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
        if (child &&
            (child = FindChildByClassName(child,
                                          L"SystemTray.SystemTrayFrame")) &&
            (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
            systemTrayFrameGrid = child;
        }

        if (!systemTrayFrameGrid) {
            return;
        }

        if (FrameworkElement mainStack =
                FindChildByName(systemTrayFrameGrid, L"MainStack")) {
            ApplyMainStackStyle(mainStack);
        }

        if (FrameworkElement nonActivatableStack =
                FindChildByName(systemTrayFrameGrid, L"NonActivatableStack")) {
            ApplyNonActivatableStackStyle(nonActivatableStack);
        }

        if (FrameworkElement controlCenterButton =
                FindChildByName(systemTrayFrameGrid, L"ControlCenterButton")) {
            ApplyControlCenterButtonStyle(controlCenterButton);
        }

        if (FrameworkElement notificationCenterButton = FindChildByName(
                systemTrayFrameGrid, L"NotificationCenterButton")) {
            ApplyNotificationCenterButtonStyle(notificationCenterButton);
        }

        if (FrameworkElement showDesktopStack =
                FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack")) {
            ApplyShowDesktopStackStyle(showDesktopStack);
        }
    } catch (...) {
    }
}

void RestoreTrayIconStyles() {
    if (!g_autoRevokerList || !g_batteryTextBlockStates) {
        return;
    }

    try {
        g_autoRevokerList->clear();

        if (auto bellSystemTrayIconElement =
                g_bellSystemTrayIconElement.get()) {
            bellSystemTrayIconElement.UnregisterPropertyChangedCallback(
                Automation::AutomationProperties::NameProperty(),
                g_bellAutomationNameChangedToken);
            g_bellSystemTrayIconElement = nullptr;
            g_bellAutomationNameChangedToken = 0;
        }

        if (auto mainStackInnerTextBlock = g_mainStackInnerTextBlock.get()) {
            mainStackInnerTextBlock.UnregisterPropertyChangedCallback(
                Controls::TextBlock::TextProperty(),
                g_mainStackTextChangedToken);
            g_mainStackInnerTextBlock = nullptr;
            g_mainStackTextChangedToken = 0;
        }

        if (auto controlCenterStackPanel = g_controlCenterStackPanel.get()) {
            controlCenterStackPanel.SizeChanged(
                g_controlCenterStackPanelSizeChangedToken);
            g_controlCenterStackPanel = nullptr;
            g_controlCenterStackPanelSizeChangedToken = {};
        }

        // Unregister and restore battery TextBlock foregrounds. ApplyStyle
        // below will re-register if grayscale is still enabled. This also
        // covers stale entries whose icon view was destroyed.
        for (auto& state : *g_batteryTextBlockStates) {
            auto textBlock = state.textBlock.get();
            if (!textBlock) {
                continue;
            }
            textBlock.UnregisterPropertyChangedCallback(
                Controls::TextBlock::ForegroundProperty(),
                state.foregroundChangedToken);
            textBlock.Foreground(state.savedForeground);
        }
        g_batteryTextBlockStates->clear();

        // Restore the show desktop width overrides. ApplyStyle below
        // re-applies them unless the mod is unloading.
        for (auto& state : g_showDesktopWidthStates) {
            auto element = state.element.get();
            if (!element) {
                continue;
            }
            if (state.savedMinWidth) {
                element.MinWidth(*state.savedMinWidth);
            } else {
                element.ClearValue(FrameworkElement::MinWidthProperty());
            }
            if (state.savedMaxWidth) {
                element.MaxWidth(*state.savedMaxWidth);
            } else {
                element.ClearValue(FrameworkElement::MaxWidthProperty());
            }
        }
        g_showDesktopWidthStates.clear();
    } catch (...) {
    }

    if (g_unloading) {
        g_autoRevokerList.reset();
        g_batteryTextBlockStates.reset();
    }
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    HookScope hookScope;

    void* ret = IconView_IconView_Original(pThis);

    // A new icon view has nothing to restore, and the revoker list is released
    // while unloading.
    if (g_unloading || !g_autoRevokerList) {
        return ret;
    }

    try {
        FrameworkElement iconView = nullptr;
        ((IUnknown**)pThis)[1]->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(iconView));
        if (!iconView) {
            return ret;
        }

        g_autoRevokerList->emplace_back();
        auto autoRevokerIt = g_autoRevokerList->end();
        --autoRevokerIt;

        *autoRevokerIt = iconView.Loaded(
            winrt::auto_revoke_t{},
            [autoRevokerIt](winrt::Windows::Foundation::IInspectable const&
                                sender,
                            RoutedEventArgs const& e) {
                try {
                    if (g_autoRevokerList) {
                        g_autoRevokerList->erase(autoRevokerIt);
                    }

                    auto iconView = sender.try_as<FrameworkElement>();
                    if (!iconView) {
                        return;
                    }

                    auto className = winrt::get_class_name(iconView);

                    if (className == L"SystemTray.IconView" &&
                        iconView.Name() == L"SystemTrayIcon") {
                        if (IsChildOfElementByName(iconView, L"MainStack")) {
                            ApplyMainStackIconViewStyle(iconView);
                        } else if (IsChildOfElementByName(
                                       iconView, L"NonActivatableStack")) {
                            ApplyNonActivatableStackIconViewStyle(iconView);
                        } else if (IsChildOfElementByName(
                                       iconView, L"ControlCenterButton")) {
                            ApplyControlCenterButtonIconStyle(iconView);
                        } else if (IsChildOfElementByName(
                                       iconView,
                                       L"NotificationCenterButton")) {
                            ApplyBellIconStyle(iconView);
                        } else if (IsChildOfElementByName(
                                       iconView, L"ShowDesktopStack")) {
                            ApplyShowDesktopStyle(iconView);
                        }
                    }
                } catch (...) {
                }
            });
    } catch (...) {
    }

    return ret;
}

void ApplyNowOnTaskbarThread(PVOID param) {
    HWND taskbarWnd = (HWND)param;
    if (g_unloading) {
        return;
    }

    try {
        XamlRoot xamlRoot = GetXamlRootForTaskbarWindow(taskbarWnd);
        if (!xamlRoot) {
            if (!g_loggedMissingXamlRoot.exchange(true)) {
                Wh_Log(L"Taskbar XAML root not found");
            }
            return;
        }

        g_loggedMissingXamlRoot = false;

        // Tray system icon tweaks.
        ApplyTrayIconStyles(xamlRoot);

        FrameworkElement frame = FindTaskbarFrameInXamlRoot(xamlRoot);
        if (!frame) {
            if (!g_loggedNoDirectApply.exchange(true)) {
                Wh_Log(L"Taskbar frame not found in the XAML root");
            }
            return;
        }

        g_loggedNoDirectApply = false;

        ApplyToFrame(frame);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"ApplyNowOnTaskbarThread failed: %08X",
               static_cast<unsigned int>(ex.code()));
    } catch (...) {
    }
}

// ---------------------------------------------------------------------------
// Auto-hide when maximized, adapted from the "Taskbar auto-hide when
// maximized" mod (taskbar-auto-hide-when-maximized).
//
// While the system's auto-hide taskbar option is on, the taskbar is kept shown
// unless a window is maximized or intersects the taskbar area, in which case
// the regular auto-hide behavior is left alone.
// ---------------------------------------------------------------------------

using IsWindowArranged_t = BOOL(WINAPI*)(HWND hwnd);
IsWindowArranged_t pIsWindowArranged = nullptr;

// Private API for window band (z-order band).
// https://blog.adeltax.com/window-z-order-in-windows-10/
using GetWindowBand_t = BOOL(WINAPI*)(HWND hWnd, PDWORD pdwBand);
GetWindowBand_t pGetWindowBand = nullptr;

// https://devblogs.microsoft.com/oldnewthing/20200302-00/?p=103507
bool IsWindowCloaked(HWND hwnd) {
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &isCloaked,
                                           sizeof(isCloaked))) &&
           isCloaked;
}

enum class MultitaskingViewType {
    None,
    WinTab,
    AltTab,
};

// Detects Alt+Tab or Win+Tab and returns the type.
MultitaskingViewType GetMultitaskingViewType(HWND hWnd) {
    // Must be in the current process (explorer.exe).
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return MultitaskingViewType::None;
    }

    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className)) ||
        _wcsicmp(className, L"XamlExplorerHostIslandWindow") != 0) {
        return MultitaskingViewType::None;
    }

    // The Win+Tab window uses band ZBID_IMMERSIVE_APPCHROME.
    constexpr DWORD ZBID_IMMERSIVE_APPCHROME = 5;

    // The Alt+Tab window uses band ZBID_SYSTEM_TOOLS. The virtual desktop
    // switcher (which we don't want) uses band ZBID_IMMERSIVE_EDGY.
    constexpr DWORD ZBID_SYSTEM_TOOLS = 16;

    DWORD band = 0;
    if (!pGetWindowBand || !pGetWindowBand(hWnd, &band)) {
        return MultitaskingViewType::None;
    }

    MultitaskingViewType type;
    if (band == ZBID_IMMERSIVE_APPCHROME) {
        type = MultitaskingViewType::WinTab;
    } else if (band == ZBID_SYSTEM_TOOLS) {
        type = MultitaskingViewType::AltTab;
    } else {
        return MultitaskingViewType::None;
    }

    // Check thread description for "MultitaskingView".
    HANDLE hThread =
        OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, dwThreadId);
    if (!hThread) {
        return MultitaskingViewType::None;
    }

    bool isMultitaskingView = false;
    PWSTR description = nullptr;
    if (SUCCEEDED(GetThreadDescription(hThread, &description)) && description) {
        isMultitaskingView = wcscmp(description, L"MultitaskingView") == 0;
        LocalFree(description);
    }

    CloseHandle(hThread);
    return isMultitaskingView ? type : MultitaskingViewType::None;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

HWND FindTaskbarWindows(std::unordered_set<HWND>* secondaryTaskbarWindows) {
    secondaryTaskbarWindows->clear();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return nullptr;
    }

    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!taskbarThreadId) {
        return nullptr;
    }

    auto enumWindowsProc = [&secondaryTaskbarWindows](HWND hWnd) -> BOOL {
        WCHAR szClassName[32];
        if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
            return TRUE;
        }

        if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
            secondaryTaskbarWindows->insert(hWnd);
        }

        return TRUE;
    };

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return hTaskbarWnd;
}

bool GetTaskbarRectForMonitor(HMONITOR monitor, RECT* rect) {
    SetRectEmpty(rect);

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return false;
    }

    SendMessage(hTaskbarWnd, g_getTaskbarRectRegisteredMsg, (WPARAM)monitor,
                (LPARAM)rect);
    return true;
}

// https://gist.github.com/m417z/451dfc2dad88d7ba88ed1814779a26b4
std::wstring GetWindowAppId(HWND hWnd) {
    // {c8900b66-a973-584b-8cae-355b7f55341b}
    constexpr winrt::guid CLSID_StartMenuCacheAndAppResolver{
        0x660b90c8,
        0x73a9,
        0x4b58,
        {0x8c, 0xae, 0x35, 0x5b, 0x7f, 0x55, 0x34, 0x1b}};

    // {de25675a-72de-44b4-9373-05170450c140}
    constexpr winrt::guid IID_IAppResolver_8{
        0xde25675a,
        0x72de,
        0x44b4,
        {0x93, 0x73, 0x05, 0x17, 0x04, 0x50, 0xc1, 0x40}};

    struct IAppResolver_8 : public IUnknown {
       public:
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcut() = 0;
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcutObject() = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForWindow(HWND hWnd,
                          WCHAR** pszAppId,
                          void* pUnknown1,
                          void* pUnknown2,
                          void* pUnknown3) = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForProcess(DWORD dwProcessId,
                           WCHAR** pszAppId,
                           void* pUnknown1,
                           void* pUnknown2,
                           void* pUnknown3) = 0;
    };

    HRESULT hr;
    std::wstring result;

    CO_MTA_USAGE_COOKIE cookie;
    bool mtaUsageIncreased = SUCCEEDED(CoIncrementMTAUsage(&cookie));

    winrt::com_ptr<IAppResolver_8> appResolver;
    hr = CoCreateInstance(CLSID_StartMenuCacheAndAppResolver, nullptr,
                          CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                          IID_IAppResolver_8, appResolver.put_void());
    if (SUCCEEDED(hr)) {
        WCHAR* pszAppId;
        hr = appResolver->GetAppIDForWindow(hWnd, &pszAppId, nullptr, nullptr,
                                            nullptr);
        if (SUCCEEDED(hr)) {
            result = pszAppId;
            CoTaskMemFree(pszAppId);
        }
    }

    appResolver = nullptr;

    if (mtaUsageIncreased) {
        CoDecrementMTAUsage(cookie);
    }

    return result;
}

std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }

    WCHAR processPath[MAX_PATH];

    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }

    CloseHandle(hProcess);

    PCWSTR processFileName = wcsrchr(processPath, L'\\');
    if (!processFileName) {
        return std::wstring{};
    }

    processFileName++;
    return processFileName;
}

std::wstring GetWindowLogInfo(HWND hWnd) {
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &dwProcessId);
    std::wstring processName = GetProcessFileName(dwProcessId);

    WCHAR className[256];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        wcscpy_s(className, L"<unknown>");
    }

    WCHAR windowName[256];
    if (!GetWindowText(hWnd, windowName, ARRAYSIZE(windowName))) {
        windowName[0] = L'\0';
    }

    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

    RECT rect{};
    GetWindowRect(hWnd, &rect);

    WCHAR buffer[1024];
    swprintf_s(buffer,
               L"window %08X: PID=%u, process=%s, class=%s, name=%s, "
               L"style=0x%08X, exStyle=0x%08X, rect={%d,%d,%d,%d}",
               (DWORD)(DWORD_PTR)hWnd, dwProcessId, processName.c_str(),
               className, windowName, style, exStyle, rect.left, rect.top,
               rect.right, rect.bottom);
    return buffer;
}

bool IsWindowExcluded(HWND hWnd) {
    {
        std::lock_guard<std::mutex> lock(g_autoHideExcludedProgramsMutex);
        if (g_autoHideExcludedPrograms.empty()) {
            return false;
        }
    }

    DWORD resolvedWindowProcessPathLen = 0;
    WCHAR resolvedWindowProcessPath[MAX_PATH];
    WCHAR resolvedWindowProcessPathUpper[MAX_PATH];

    DWORD dwProcessId = 0;
    if (GetWindowThreadProcessId(hWnd, &dwProcessId)) {
        HANDLE hProcess =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
        if (hProcess) {
            DWORD dwSize = ARRAYSIZE(resolvedWindowProcessPath);
            if (QueryFullProcessImageName(hProcess, 0,
                                          resolvedWindowProcessPath, &dwSize)) {
                resolvedWindowProcessPathLen = dwSize;
            }

            CloseHandle(hProcess);
        }
    }

    if (resolvedWindowProcessPathLen > 0) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                      resolvedWindowProcessPath,
                      resolvedWindowProcessPathLen + 1,
                      resolvedWindowProcessPathUpper,
                      resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);
    } else {
        *resolvedWindowProcessPath = L'\0';
        *resolvedWindowProcessPathUpper = L'\0';
    }

    std::wstring appId = GetWindowAppId(hWnd);
    if (!appId.empty()) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, appId.data(),
                      static_cast<int>(appId.length()), appId.data(),
                      static_cast<int>(appId.length()), nullptr, nullptr, 0);
    }

    std::lock_guard<std::mutex> lock(g_autoHideExcludedProgramsMutex);

    if (resolvedWindowProcessPathLen > 0 &&
        g_autoHideExcludedPrograms.contains(resolvedWindowProcessPathUpper)) {
        return true;
    }

    if (PCWSTR programFileNameUpper =
            wcsrchr(resolvedWindowProcessPathUpper, L'\\')) {
        programFileNameUpper++;
        if (*programFileNameUpper &&
            g_autoHideExcludedPrograms.contains(programFileNameUpper)) {
            return true;
        }
    }

    if (!appId.empty() && g_autoHideExcludedPrograms.contains(appId.c_str())) {
        return true;
    }

    return false;
}

bool CanHideTaskbarForWindow(HWND hWnd,
                             HMONITOR monitor,
                             const MONITORINFO* monitorInfo,
                             const RECT* taskbarRect) {
    if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd) ||
        (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
        return false;
    }

    // Ignore the Alt+Tab overlay so it doesn't affect the taskbar state.
    if (hWnd == g_altTabViewHwnd) {
        return false;
    }

    if (hWnd == GetShellWindow() || GetProp(hWnd, L"DesktopWindow")) {
        return false;
    }

    // Exclude menus (#32768).
    if (GetClassWord(hWnd, GCW_ATOM) == 32768) {
        return false;
    }

    // Check this after the other checks, as it's the most expensive one.
    if (IsWindowExcluded(hWnd)) {
        return false;
    }

    WINDOWPLACEMENT wp{
        .length = sizeof(WINDOWPLACEMENT),
    };

    if (GetWindowPlacement(hWnd, &wp) && wp.showCmd == SW_SHOWMAXIMIZED) {
        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) == monitor) {
            return true;
        }

        return false;
    }

    bool isWindowArranged = pIsWindowArranged && pIsWindowArranged(hWnd);
    if (isWindowArranged &&
        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) != monitor) {
        return false;
    }

    RECT windowRect{};
    DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect,
                          sizeof(windowRect));

    // Don't keep the taskbar shown for a fullscreen window.
    if (EqualRect(&windowRect, &monitorInfo->rcMonitor)) {
        return true;
    }

    // It makes sense to treat arranged windows (e.g. with Win+left) as
    // maximized, as they occupy the whole monitor height. Still check for
    // intersection, as a window can also just occupy the upper side of the
    // screen (e.g. Win+left, Win+up).
    AutoHideMode mode = (AutoHideMode)g_settingAutoHideMode.load();
    if (mode == AutoHideMode::intersected ||
        (mode == AutoHideMode::maximized && isWindowArranged)) {
        RECT intersectRect;
        if (IntersectRect(&intersectRect, &windowRect, taskbarRect)) {
            return true;
        }
    }

    return false;
}

bool ShouldKeepTaskbarShown(HWND hTaskbarWnd, HMONITOR monitor) {
    if (!g_settingAutoHideEnabled.load()) {
        return true;
    }

    if (g_settingAutoHidePrimaryMonitorOnly.load() &&
        monitor != MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY)) {
        return false;
    }

    AutoHideMode mode = (AutoHideMode)g_settingAutoHideMode.load();
    if (mode == AutoHideMode::never) {
        return true;
    }
    if (mode == AutoHideMode::always) {
        return false;
    }

    // Always show the taskbar when MultitaskingView (Win+Tab) is active.
    if (g_multitaskingViewHwnd) {
        return true;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    RECT taskbarRect{};
    GetTaskbarRectForMonitor(monitor, &taskbarRect);

    if (g_settingAutoHideForegroundWindowOnly.load()) {
        HWND hForegroundWnd = GetForegroundWindow();
        return !hForegroundWnd ||
               !CanHideTaskbarForWindow(hForegroundWnd, monitor, &monitorInfo,
                                        &taskbarRect);
    }

    bool canHideTaskbar = false;

    DWORD dwTaskbarThreadId = GetCurrentThreadId();

    auto enumWindowsProc = [&](HWND hWnd) -> BOOL {
        if (GetWindowThreadProcessId(hWnd, nullptr) == dwTaskbarThreadId) {
            return TRUE;
        }

        canHideTaskbar =
            CanHideTaskbarForWindow(hWnd, monitor, &monitorInfo, &taskbarRect);
        if (!canHideTaskbar) {
            return TRUE;
        }

        Wh_Log(L"Can hide taskbar %08X for %s", (DWORD)(DWORD_PTR)hTaskbarWnd,
               GetWindowLogInfo(hWnd).c_str());
        return FALSE;
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return !canHideTaskbar;
}

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

void* QueryViaVtableBackwards(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr - 1;
    }
    return ptr;
}

DWORD WINAPI WinEventHookThread(LPVOID lpThreadParameter);

std::atomic<int> g_autoHideDiagWrites{0};

void AppendAutoHideDiag(PCWSTR text) {
    if (g_autoHideDiagWrites.load() >= 80) {
        return;
    }
    g_autoHideDiagWrites.fetch_add(1);

    WCHAR path[MAX_PATH];
    DWORD length = GetTempPathW(ARRAYSIZE(path), path);
    if (!length || length + 40 >= ARRAYSIZE(path)) {
        return;
    }

    wcscat_s(path, L"floating-taskbar-autohide.txt");

    HANDLE file = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    SYSTEMTIME st{};
    GetLocalTime(&st);

    WCHAR line[600];
    int count = swprintf_s(line, L"[%02u:%02u:%02u.%03u] %s\n", st.wHour,
                           st.wMinute, st.wSecond, st.wMilliseconds, text);
    if (count > 0) {
        DWORD written = 0;
        WriteFile(file, line, static_cast<DWORD>(count * sizeof(WCHAR)),
                  &written, nullptr);
    }

    CloseHandle(file);
}

std::atomic<int> g_trayLayoutDiagWrites{0};
std::atomic<int> g_taskButtonStackDiagWrites{0};
std::atomic<int> g_trayAlignmentDiagWrites{0};
std::atomic<int> g_clockFeatureDiagWrites{0};

void AppendTrayLayoutDiag(PCWSTR text) {
    if (g_trayLayoutDiagWrites.load() >= 300) {
        return;
    }
    g_trayLayoutDiagWrites.fetch_add(1);

    WCHAR path[MAX_PATH];
    DWORD length = GetTempPathW(ARRAYSIZE(path), path);
    if (!length || length + 40 >= ARRAYSIZE(path)) {
        return;
    }

    wcscat_s(path, L"taskbar-tray-layout-diag.txt");

    HANDLE file = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    WCHAR line[600];
    int count = swprintf_s(line, L"%s\n", text);
    if (count > 0) {
        DWORD written = 0;
        WriteFile(file, line, static_cast<DWORD>(count * sizeof(WCHAR)),
                  &written, nullptr);
    }

    CloseHandle(file);
}

void AppendTaskButtonStackDiag(PCWSTR text) {
    if (g_taskButtonStackDiagWrites.load() >= 500) {
        return;
    }
    g_taskButtonStackDiagWrites.fetch_add(1);

    WCHAR path[MAX_PATH];
    DWORD length = GetTempPathW(ARRAYSIZE(path), path);
    if (!length || length + 40 >= ARRAYSIZE(path)) {
        return;
    }

    wcscat_s(path, L"taskbar-taskbutton-stack-diag.txt");

    HANDLE file = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    SYSTEMTIME st{};
    GetLocalTime(&st);
    WCHAR line[900];
    int count = swprintf_s(line, L"[%02u:%02u:%02u.%03u] %s\n", st.wHour,
                           st.wMinute, st.wSecond, st.wMilliseconds, text);
    if (count > 0) {
        DWORD written = 0;
        WriteFile(file, line, static_cast<DWORD>(count * sizeof(WCHAR)),
                  &written, nullptr);
    }

    CloseHandle(file);
}

void AppendTrayAlignmentDiag(PCWSTR text) {
    if (g_trayAlignmentDiagWrites.load() >= 1000) {
        return;
    }
    g_trayAlignmentDiagWrites.fetch_add(1);

    WCHAR path[MAX_PATH];
    DWORD length = GetTempPathW(ARRAYSIZE(path), path);
    if (!length || length + 44 >= ARRAYSIZE(path)) {
        return;
    }
    wcscat_s(path, L"taskbar-tray-alignment-diag.txt");

    HANDLE file = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    SYSTEMTIME st{};
    GetLocalTime(&st);
    WCHAR line[1100];
    int count = swprintf_s(line, L"[%02u:%02u:%02u.%03u] %s\n", st.wHour,
                           st.wMinute, st.wSecond, st.wMilliseconds, text);
    if (count > 0) {
        DWORD written = 0;
        WriteFile(file, line, static_cast<DWORD>(count * sizeof(WCHAR)),
                  &written, nullptr);
    }

    CloseHandle(file);
}

void AppendClockFeatureDiag(PCWSTR text) {
    if (g_clockFeatureDiagWrites.load() >= 500) {
        return;
    }
    g_clockFeatureDiagWrites.fetch_add(1);

    WCHAR path[MAX_PATH];
    DWORD length = GetTempPathW(ARRAYSIZE(path), path);
    if (!length || length + 40 >= ARRAYSIZE(path)) {
        return;
    }
    wcscat_s(path, L"taskbar-clock-features-diag.txt");

    HANDLE file = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    SYSTEMTIME st{};
    GetLocalTime(&st);
    WCHAR line[1100];
    int count = swprintf_s(line, L"[%02u:%02u:%02u.%03u] %s\n", st.wHour,
                           st.wMinute, st.wSecond, st.wMilliseconds, text);
    if (count > 0) {
        DWORD written = 0;
        WriteFile(file, line, static_cast<DWORD>(count * sizeof(WCHAR)),
                  &written, nullptr);
    }
    CloseHandle(file);
}

void AdjustTaskbar(HWND hMMTaskbarWnd, bool clearPendingWhenDone = false) {
    if (!g_settingAutoHideEnabled.load()) {
        return;
    }

    if ((AutoHideMode)g_settingAutoHideMode.load() != AutoHideMode::never) {
        if (!g_winEventHookThread) {
            std::lock_guard<std::mutex> guard(g_winEventHookThreadMutex);

            if (!g_winEventHookThread) {
                g_winEventHookThread = CreateThread(
                    nullptr, 0, WinEventHookThread, nullptr, 0, nullptr);
            }
        }
    }

    PostMessage(hMMTaskbarWnd, g_updateTaskbarStateRegisteredMsg,
                clearPendingWhenDone, 0);
}

void AdjustAllTaskbars() {
    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (hWnd) {
        AdjustTaskbar(hWnd);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        AdjustTaskbar(hSecondaryWnd);
    }
}

bool AdjustAllTaskbarsIfNotPending() {
    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindTaskbarWindows(&secondaryTaskbarWindows);

    DWORD currentTickCount = GetTickCount();
    DWORD pendingTickCount = 0;

    if (hWnd) {
        DWORD tickCount = (DWORD)(DWORD_PTR)GetProp(
            hWnd, kUpdateTaskbarStatePendingTickCount);
        if (tickCount > pendingTickCount) {
            pendingTickCount = tickCount;
        }
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        DWORD tickCount = (DWORD)(DWORD_PTR)GetProp(
            hSecondaryWnd, kUpdateTaskbarStatePendingTickCount);
        if (tickCount > pendingTickCount) {
            pendingTickCount = tickCount;
        }
    }

    // Consider times larger than 10 seconds as expired, to prevent having it
    // stuck in this state.
    if (pendingTickCount && currentTickCount - pendingTickCount < 1000 * 10) {
        return false;
    }

    if (hWnd) {
        SetProp(hWnd, kUpdateTaskbarStatePendingTickCount,
                (HANDLE)(DWORD_PTR)currentTickCount);
        AdjustTaskbar(hWnd, /*clearPendingWhenDone=*/true);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        SetProp(hSecondaryWnd, kUpdateTaskbarStatePendingTickCount,
                (HANDLE)(DWORD_PTR)currentTickCount);
        AdjustTaskbar(hSecondaryWnd, /*clearPendingWhenDone=*/true);
    }

    return true;
}

// Called on a taskbar UI thread when the auto-hide enhancement is turned off:
// release the taskbars which were kept shown and restore the system auto-hide
// setting if it was forced on.
void AutoHideReleaseOnCurrentThread() {
    if (g_wasAutoHideDisabled) {
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (hTaskbarWnd) {
            SendMessage(hTaskbarWnd, kHandleTrayPrivateSettingMessage,
                        kTrayPrivateSettingAutoHideSet, FALSE);
        }
        g_wasAutoHideDisabled = false;
        g_wasAutoHideProcessed = false;
    }

    std::vector<HWND> taskbarsToHide;
    for (auto const& pair : g_taskbarsKeptShown) {
        taskbarsToHide.push_back(pair.second);
    }
    g_taskbarsKeptShown.clear();

    for (HWND hWnd : taskbarsToHide) {
        if (IsWindow(hWnd)) {
            SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);
        }
    }
}

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, bool expanded);
ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original;
bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         bool expanded) {
    HookScope hookScope;

    g_taskbarToViewCoordinator[hMMTaskbarWnd] = pThis;

    // Return true if the taskbar should be kept shown.
    for (const auto& pair : g_taskbarsKeptShown) {
        if (pair.second == hMMTaskbarWnd) {
            Wh_Log(L"Returning true for taskbar kept shown");
            return true;
        }
    }

    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(
        pThis, hMMTaskbarWnd, expanded);
}

using ViewCoordinator_UpdateIsExpanded_t =
    void(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, int reason);
ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;

void UpdateViewCoordinatorIsExpanded(HWND hWnd) {
    // From ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged.
    constexpr int kReasonIsPointerOverTaskbarFrameChanged = 7;

    if (ViewCoordinator_UpdateIsExpanded_Original) {
        auto it = g_taskbarToViewCoordinator.find(hWnd);
        if (it != g_taskbarToViewCoordinator.end()) {
            ViewCoordinator_UpdateIsExpanded_Original(
                it->second, hWnd, kReasonIsPointerOverTaskbarFrameChanged);
        }
    }
}

void* TrayUI_vftable_IInspectable;
void* TrayUI_vftable_ITrayComponentHost;
void* CSecondaryTray_vftable_ISecondaryTray;

using TrayUI_GetStuckMonitor_t = HMONITOR(WINAPI*)(void* pThis);
TrayUI_GetStuckMonitor_t TrayUI_GetStuckMonitor_Original;

using CSecondaryTray_GetMonitor_t = HMONITOR(WINAPI*)(void* pThis);
CSecondaryTray_GetMonitor_t CSecondaryTray_GetMonitor_Original;

using TrayUI_GetStuckRectForMonitor_t = bool(WINAPI*)(void* pThis,
                                                      HMONITOR hMonitor,
                                                      RECT* rect);
TrayUI_GetStuckRectForMonitor_t TrayUI_GetStuckRectForMonitor_Original;

using TrayUI_GetStuckRectForMonitor_Win10_t = RECT*(WINAPI*)(void* pThis,
                                                             RECT* rect,
                                                             HMONITOR hMonitor);
TrayUI_GetStuckRectForMonitor_Win10_t
    TrayUI_GetStuckRectForMonitor_Win10_Original;

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    HookScope hookScope;

    auto it = g_taskbarsKeptShown.find(pThis);
    if (it != g_taskbarsKeptShown.end()) {
        AppendAutoHideDiag(L"hide hook: blocked (kept shown)");
        KillTimer(it->second, kTrayUITimerHide);
        return;
    }

    TrayUI__Hide_Original(pThis);
}

using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
    HookScope hookScope;

    auto it = g_taskbarsKeptShown.find(pThis);
    if (it != g_taskbarsKeptShown.end()) {
        KillTimer(it->second, kTrayUITimerHide);
        return;
    }

    CSecondaryTray__AutoHide_Original(pThis, param1);
}

using TrayUI_Unhide_t = void(WINAPI*)(void* pThis,
                                      int trayUnhideFlags,
                                      int unhideRequest);
TrayUI_Unhide_t TrayUI_Unhide_Original;

using CSecondaryTray__Unhide_t = void(WINAPI*)(void* pThis,
                                               int trayUnhideFlags,
                                               int unhideRequest);
CSecondaryTray__Unhide_t CSecondaryTray__Unhide_Original;

using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis,
                                          HWND hWnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis,
                                   HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   bool* flag) {
    HookScope hookScope;

    if (Msg == WM_NCCREATE) {
        AdjustTaskbar(hWnd);
    } else if (Msg == WM_NCDESTROY) {
        if (TrayUI_vftable_IInspectable) {
            g_taskbarsKeptShown.erase(
                QueryViaVtableBackwards(pThis, TrayUI_vftable_IInspectable));
        }
        g_taskbarToViewCoordinator.erase(hWnd);
    } else if (Msg == kHandleTrayPrivateSettingMessage) {
        // Prevent auto-hide from being disabled while the mod is loaded.
        if (g_settingAutoHideEnabled.load() &&
            (DWORD)wParam == kTrayPrivateSettingAutoHideSet) {
            BOOL bSetAutoHideEnabled = (BOOL)lParam;
            if (!bSetAutoHideEnabled) {
                return 0;
            }
        }
    } else if (Msg == g_refreshIconInsetRegisteredMsg) {
        RefreshTaskbarIconInsetOnCurrentThread();
        return 0;
    } else if (Msg == g_getTaskbarRectRegisteredMsg) {
        HMONITOR monitor = (HMONITOR)wParam;
        RECT* rect = (RECT*)lParam;
        if (TrayUI_GetStuckRectForMonitor_Original) {
            if (!TrayUI_GetStuckRectForMonitor_Original(pThis, monitor, rect)) {
                SetRectEmpty(rect);
            }
        } else if (TrayUI_GetStuckRectForMonitor_Win10_Original) {
            TrayUI_GetStuckRectForMonitor_Win10_Original(pThis, rect, monitor);
        } else {
            SetRectEmpty(rect);
        }
    } else if (Msg == g_updateTaskbarStateRegisteredMsg) {
        if (!g_settingAutoHideEnabled.load() ||
            !TrayUI_GetStuckMonitor_Original || !TrayUI_Unhide_Original ||
            !TrayUI_vftable_IInspectable ||
            !TrayUI_vftable_ITrayComponentHost) {
            return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam,
                                           flag);
        }

        if (!g_wasAutoHideProcessed) {
            g_wasAutoHideProcessed = true;
            LRESULT autoHideState =
                SendMessage(hWnd, kHandleTrayPrivateSettingMessage,
                            kTrayPrivateSettingAutoHideGet, 0);
            g_wasAutoHideDisabled = !autoHideState;
            if (g_wasAutoHideDisabled) {
                SendMessage(hWnd, kHandleTrayPrivateSettingMessage,
                            kTrayPrivateSettingAutoHideSet, TRUE);
            }

            WCHAR text[128];
            swprintf_s(text, L"first update: autoHide=%d forced=%d",
                       (int)autoHideState, g_wasAutoHideDisabled ? 1 : 0);
            AppendAutoHideDiag(text);
        }

        HMONITOR monitor = TrayUI_GetStuckMonitor_Original(pThis);
        bool keepShown = ShouldKeepTaskbarShown(hWnd, monitor);

        void* pTrayUI_IInspectable =
            QueryViaVtableBackwards(pThis, TrayUI_vftable_IInspectable);

        bool keptShown = g_taskbarsKeptShown.contains(pTrayUI_IInspectable);

        {
            HWND hForegroundWnd = GetForegroundWindow();
            WINDOWPLACEMENT wp{sizeof(wp)};
            GetWindowPlacement(hForegroundWnd, &wp);

            WCHAR text[256];
            swprintf_s(text,
                       L"update: keepShown=%d keptShown=%d fg=0x%X showCmd=%d "
                       L"monitor=%p",
                       keepShown ? 1 : 0, keptShown ? 1 : 0,
                       (unsigned)(ULONG_PTR)hForegroundWnd, (int)wp.showCmd,
                       (void*)monitor);
            AppendAutoHideDiag(text);
        }

        if (keepShown != keptShown) {
            Wh_Log(L"> keepShown=%d", keepShown);

            if (keepShown) {
                g_taskbarsKeptShown[pTrayUI_IInspectable] = hWnd;

                void* pTrayUI_ITrayComponentHost =
                    QueryViaVtable(pThis, TrayUI_vftable_ITrayComponentHost);
                TrayUI_Unhide_Original(pTrayUI_ITrayComponentHost, 0, 0);

                UpdateViewCoordinatorIsExpanded(hWnd);
            } else {
                g_taskbarsKeptShown.erase(pTrayUI_IInspectable);

                SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);

                // Hide directly as well, in case the timer id differs on this
                // Windows build.
                if (TrayUI__Hide_Original) {
                    TrayUI__Hide_Original(pTrayUI_IInspectable);
                }

                AppendAutoHideDiag(L"released: hide requested");

                UpdateViewCoordinatorIsExpanded(hWnd);
            }
        }

        if (wParam) {
            RemoveProp(hWnd, kUpdateTaskbarStatePendingTickCount);
        }
    }

    LRESULT ret =
        TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);

    return ret;
}

using CSecondaryTray_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis,
                                             HWND hWnd,
                                             UINT Msg,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    HookScope hookScope;

    if (Msg == WM_NCCREATE) {
        AdjustTaskbar(hWnd);
    } else if (Msg == WM_NCDESTROY) {
        g_taskbarsKeptShown.erase(pThis);
        g_taskbarToViewCoordinator.erase(hWnd);
    } else if (Msg == g_refreshIconInsetRegisteredMsg) {
        RefreshTaskbarIconInsetOnCurrentThread();
        return 0;
    } else if (Msg == g_updateTaskbarStateRegisteredMsg) {
        if (!CSecondaryTray_GetMonitor_Original ||
            !CSecondaryTray__Unhide_Original ||
            !CSecondaryTray_vftable_ISecondaryTray) {
            return CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam,
                                                     lParam);
        }

        void* pCSecondaryTray_ISecondaryTray =
            QueryViaVtable(pThis, CSecondaryTray_vftable_ISecondaryTray);

        HMONITOR monitor =
            CSecondaryTray_GetMonitor_Original(pCSecondaryTray_ISecondaryTray);

        bool keepShown = ShouldKeepTaskbarShown(hWnd, monitor);

        bool keptShown = g_taskbarsKeptShown.contains(pThis);

        if (keepShown != keptShown) {
            Wh_Log(L"> keepShown=%d", keepShown);

            if (keepShown) {
                g_taskbarsKeptShown[pThis] = hWnd;

                CSecondaryTray__Unhide_Original(pThis, 0, 0);

                UpdateViewCoordinatorIsExpanded(hWnd);
            } else {
                g_taskbarsKeptShown.erase(pThis);

                SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);

                UpdateViewCoordinatorIsExpanded(hWnd);
            }
        }

        if (wParam) {
            RemoveProp(hWnd, kUpdateTaskbarStatePendingTickCount);
        }
    }

    LRESULT ret =
        CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);

    return ret;
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook,
                           DWORD event,
                           HWND hWnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD dwEventThread,
                           DWORD dwmsEventTime) {
    HookScope hookScope;

    if (idObject != OBJID_WINDOW ||
        (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) || IsTaskbarWindow(hWnd)) {
        return;
    }

    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (hParentWnd && hParentWnd != GetDesktopWindow()) {
        return;
    }

    // Check for Multitasking View window state changes.
    {
        auto multitaskingViewType = GetMultitaskingViewType(hWnd);
        bool isTrackedWinTab = hWnd == g_multitaskingViewHwnd;
        bool isTrackedAltTab = hWnd == g_altTabViewHwnd;

        if (multitaskingViewType != MultitaskingViewType::None ||
            isTrackedWinTab || isTrackedAltTab) {
            auto& targetHwnd =
                (multitaskingViewType == MultitaskingViewType::AltTab ||
                 isTrackedAltTab)
                    ? g_altTabViewHwnd
                    : g_multitaskingViewHwnd;

            bool entering = event == EVENT_OBJECT_SHOW ||
                            event == EVENT_OBJECT_UNCLOAKED ||
                            event == EVENT_OBJECT_CREATE;
            bool leaving = event == EVENT_OBJECT_HIDE ||
                           event == EVENT_OBJECT_CLOAKED ||
                           event == EVENT_OBJECT_DESTROY;

            if (entering) {
                HWND expected = nullptr;
                if (!targetHwnd.compare_exchange_strong(expected, hWnd)) {
                    return;
                }
                Wh_Log(
                    L"MultitaskingView entering (%s)",
                    &targetHwnd == &g_altTabViewHwnd ? L"Alt+Tab" : L"Win+Tab");
            } else if (leaving) {
                HWND expected = hWnd;
                if (!targetHwnd.compare_exchange_strong(expected, nullptr)) {
                    return;
                }
                Wh_Log(
                    L"MultitaskingView leaving (%s)",
                    &targetHwnd == &g_altTabViewHwnd ? L"Alt+Tab" : L"Win+Tab");
            } else {
                return;
            }

            // Fall through to trigger the timer for all taskbars.
        }
    }

    if (g_pendingEventsTimer) {
        return;
    }

    g_pendingEventsTimer = SetTimer(
        nullptr, 0, 200,
        [](HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
            if (!AdjustAllTaskbarsIfNotPending()) {
                Wh_Log(L"Adjustment already pending, will retry later...");
                return;
            }

            KillTimer(nullptr, g_pendingEventsTimer);
            g_pendingEventsTimer = 0;
        });
}

DWORD WINAPI WinEventHookThread(LPVOID lpThreadParameter) {
    HWINEVENTHOOK winObjectEventHook1 =
        SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook1) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winObjectEventHook2 = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook2) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winObjectEventHook3 =
        SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook3) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winSystemEventHook1 = nullptr;
    if (g_settingAutoHideForegroundWindowOnly.load()) {
        winSystemEventHook1 =
            SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                            nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        if (!winSystemEventHook1) {
            Wh_Log(L"Error: SetWinEventHook");
        }
    }

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
            msg.wParam = 0;
            break;
        }

        if (msg.hwnd == NULL && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (winObjectEventHook1) {
        UnhookWinEvent(winObjectEventHook1);
    }

    if (winObjectEventHook2) {
        UnhookWinEvent(winObjectEventHook2);
    }

    if (winObjectEventHook3) {
        UnhookWinEvent(winObjectEventHook3);
    }

    if (winSystemEventHook1) {
        UnhookWinEvent(winSystemEventHook1);
    }

    g_multitaskingViewHwnd = nullptr;
    g_altTabViewHwnd = nullptr;

    return 0;
}

void StopAutoHideWinEventHookThread() {
    std::lock_guard<std::mutex> guard(g_winEventHookThreadMutex);

    if (g_winEventHookThread) {
        PostThreadMessage(GetThreadId(g_winEventHookThread), WM_APP, 0, 0);
        WaitForSingleObject(g_winEventHookThread, INFINITE);
        CloseHandle(g_winEventHookThread);
        g_winEventHookThread = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Cycling through the windows of a combined taskbar item, adapted from the
// "Cycle through taskbar windows on click" mod (taskbar-left-click-cycle).
//
// The taskbar asks ShouldCycleWindows whether a click on a combined item
// should cycle through the windows of the group or open the thumbnail
// previews. _HandleClick is hooked for the taskbar implementations which
// don't ask (e.g. the old taskbar), and for the Win+# hotkeys.
// ---------------------------------------------------------------------------

bool g_shouldCycleWindowsHandled = false;
bool g_inHandleWinNumHotKey = false;

using ShouldCycleWindows_t = bool(WINAPI*)();
ShouldCycleWindows_t ShouldCycleWindows_Original = nullptr;
bool WINAPI ShouldCycleWindows_Hook() {
    HookScope hookScope;

    if (!g_settingCycleThroughWindows.load()) {
        return ShouldCycleWindows_Original();
    }

    g_shouldCycleWindowsHandled = true;
    bool ctrlKeyDown = GetKeyState(VK_CONTROL) < 0;
    return !ctrlKeyDown;
}

using CTaskBtnGroup_GetGroupType_t = int(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroupType_t CTaskBtnGroup_GetGroupType_Original = nullptr;

using CTaskListWnd_HandleClick_t = void(WINAPI*)(void* pThis,
                                                 void* taskBtnGroup,
                                                 int taskItemIndex,
                                                 int clickAction,
                                                 int param4,
                                                 int param5);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original = nullptr;
void WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                          void* taskBtnGroup,
                                          int taskItemIndex,
                                          int clickAction,
                                          int param4,
                                          int param5) {
    HookScope hookScope;

    auto callOriginal = [&]() {
        CTaskListWnd_HandleClick_Original(pThis, taskBtnGroup, taskItemIndex,
                                          clickAction, param4, param5);
    };

    if (!g_settingCycleThroughWindows.load() ||
        !CTaskBtnGroup_GetGroupType_Original) {
        callOriginal();
        return;
    }

    // Group types:
    // 1 - Single item or multiple uncombined items
    // 2 - Pinned item
    // 3 - Multiple combined items
    int groupType = CTaskBtnGroup_GetGroupType_Original(taskBtnGroup);
    if (groupType != 3) {
        callOriginal();
        return;
    }

    constexpr int kClick = 0;
    constexpr int kForward = 1;
    constexpr int kBack = 2;
    constexpr int kCtrlClick = 4;

    int newClickAction = clickAction;
    if (g_inHandleWinNumHotKey) {
        // Win+# is being handled: cycle through the windows instead of the
        // default action.
        if (clickAction == kForward || clickAction == kBack) {
            newClickAction = kCtrlClick;
        } else if (clickAction == kCtrlClick) {
            newClickAction = kForward;
        }
    } else if (!g_shouldCycleWindowsHandled) {
        // The taskbar didn't ask ShouldCycleWindows, so a plain click would
        // open the thumbnail previews. Swap the actions.
        if (clickAction == kClick) {
            newClickAction = kCtrlClick;
        } else if (clickAction == kCtrlClick) {
            newClickAction = kClick;
        }
    }

    CTaskListWnd_HandleClick_Original(pThis, taskBtnGroup, taskItemIndex,
                                      newClickAction, param4, param5);
}

using CTaskListWnd_HandleWinNumHotKey_t = HRESULT(WINAPI*)(void* pThis,
                                                           short param1,
                                                           WORD param2);
CTaskListWnd_HandleWinNumHotKey_t CTaskListWnd_HandleWinNumHotKey_Original =
    nullptr;
HRESULT WINAPI CTaskListWnd_HandleWinNumHotKey_Hook(void* pThis,
                                                    short param1,
                                                    WORD param2) {
    HookScope hookScope;

    if (!g_settingCycleThroughWindows.load()) {
        return CTaskListWnd_HandleWinNumHotKey_Original(pThis, param1, param2);
    }

    g_inHandleWinNumHotKey = true;

    HRESULT ret =
        CTaskListWnd_HandleWinNumHotKey_Original(pThis, param1, param2);

    g_inHandleWinNumHotKey = false;

    return ret;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &g_ctaskBandTaskListWndSiteVftable,
            nullptr,
            true,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &g_csecondaryTaskBandTaskListWndSiteVftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &g_taskbarHostFrameHeight,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &RefCountDecref_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            &CTaskBtnGroup_GetGroupType_Original,
            nullptr,
            true,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleWinNumHotKey(short,unsigned short))"},
            &CTaskListWnd_HandleWinNumHotKey_Original,
            CTaskListWnd_HandleWinNumHotKey_Hook,
            true,
        },
        // Auto-hide when maximized.
        {
            {
                // Windows 11.
                LR"(const TrayUI::`vftable'{for `IInspectable'})",

                // Windows 10.
                LR"(const TrayUI::`vftable'{for `ITrayDeskBand'})",
            },
            &TrayUI_vftable_IInspectable,
            nullptr,
            true,
        },
        {
            {LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})"},
            &TrayUI_vftable_ITrayComponentHost,
            nullptr,
            true,
        },
        {
            {LR"(const CSecondaryTray::`vftable'{for `ISecondaryTray'})"},
            &CSecondaryTray_vftable_ISecondaryTray,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual struct HMONITOR__ * __cdecl TrayUI::GetStuckMonitor(void))"},
            &TrayUI_GetStuckMonitor_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual struct HMONITOR__ * __cdecl CSecondaryTray::GetMonitor(void))"},
            &CSecondaryTray_GetMonitor_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual bool __cdecl TrayUI::GetStuckRectForMonitor(struct HMONITOR__ *,struct tagRECT *))"},
            &TrayUI_GetStuckRectForMonitor_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual struct tagRECT __cdecl TrayUI::GetStuckRectForMonitor(struct HMONITOR__ *))"},
            &TrayUI_GetStuckRectForMonitor_Win10_Original,
            nullptr,
            true,
        },
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
        {
            {LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &TrayUI_Unhide_Original,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &CSecondaryTray__Unhide_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
            true,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_v_WndProc_Original,
            CSecondaryTray_v_WndProc_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for taskbar.dll");
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Taskbar clock customization, adapted from the "Taskbar Clock
// Customization" mod (taskbar-clock-customization).
//
// The clock's text is customized by replacing the strings which are returned
// by GetTimeFormatEx/GetDateFormatEx while the clock data model is refreshing.
// The replacement text is built from patterns such as %time% and %cpu%.
// ---------------------------------------------------------------------------

thread_local bool g_clockInRefreshIcon = false;
thread_local bool g_clockInGetToolTipString = false;
thread_local bool g_clockNeedAdjustTimer = false;
thread_local bool g_clockInMediaInfo = false;

using ClockGetTimeFormatEx_t = int(WINAPI*)(LPCWSTR lpLocaleName,
                                            DWORD dwFlags,
                                            const SYSTEMTIME* lpTime,
                                            LPCWSTR lpFormat,
                                            LPWSTR lpTimeStr,
                                            int cchTime);
ClockGetTimeFormatEx_t ClockGetTimeFormatEx_Original = nullptr;

using ClockGetDateFormatEx_t = int(WINAPI*)(LPCWSTR lpLocaleName,
                                            DWORD dwFlags,
                                            const SYSTEMTIME* lpDate,
                                            LPCWSTR lpFormat,
                                            LPWSTR lpDateStr,
                                            int cchDate,
                                            LPCWSTR lpCalendar);
ClockGetDateFormatEx_t ClockGetDateFormatEx_Original = nullptr;

using ClockGetLocalTime_t = void(WINAPI*)(LPSYSTEMTIME lpSystemTime);
ClockGetLocalTime_t ClockGetLocalTime_Original = nullptr;

std::wstring ClockReplaceAll(std::wstring_view source,
                             std::wstring_view from,
                             std::wstring_view to) {
    std::wstring result;
    size_t pos = 0;
    while (true) {
        size_t found = source.find(from, pos);
        if (found == std::wstring_view::npos) {
            result.append(source.substr(pos));
            break;
        }
        result.append(source.substr(pos, found - pos));
        result.append(to);
        pos = found + from.size();
    }
    return result;
}

std::wstring ClockTrim(std::wstring_view s) {
    size_t start = 0;
    while (start < s.size() && iswspace(s[start])) {
        start++;
    }
    size_t end = s.size();
    while (end > start && iswspace(s[end - 1])) {
        end--;
    }
    return std::wstring(s.substr(start, end - start));
}

std::vector<std::wstring> ClockSplitList(std::wstring_view text,
                                         wchar_t separator) {
    std::vector<std::wstring> result;
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t found = text.find(separator, pos);
        if (found == std::wstring_view::npos) {
            result.push_back(ClockTrim(text.substr(pos)));
            break;
        }
        result.push_back(ClockTrim(text.substr(pos, found - pos)));
        pos = found + 1;
    }
    return result;
}

std::wstring ClockFormatTime(SYSTEMTIME const& time,
                             std::wstring const& format,
                             bool showSeconds) {
    if (!ClockGetTimeFormatEx_Original) {
        return L"";
    }

    DWORD flags = showSeconds ? 0 : TIME_NOSECONDS;
    std::wstring formatToUse = format;
    if (!showSeconds && !formatToUse.empty()) {
        formatToUse = ClockReplaceAll(formatToUse, L"':'ss", L"");
    }

    WCHAR buffer[256] = {};
    ClockGetTimeFormatEx_Original(
        nullptr, flags, &time,
        formatToUse.empty() ? nullptr : formatToUse.c_str(), buffer,
        ARRAYSIZE(buffer));
    return buffer;
}

std::wstring ClockFormatDate(SYSTEMTIME const& time,
                             std::wstring const& format,
                             std::wstring const& locale) {
    if (!ClockGetDateFormatEx_Original) {
        return L"";
    }

    WCHAR buffer[256] = {};
    ClockGetDateFormatEx_Original(
        locale.empty() ? nullptr : locale.c_str(), DATE_AUTOLAYOUT, &time,
        format.empty() ? nullptr : format.c_str(), buffer,
        ARRAYSIZE(buffer), nullptr);
    return buffer;
}

std::wstring ClockFormatWeekday(SYSTEMTIME const& time,
                                ClockSettings const& settings) {
    if (settings.weekdayFormat == L"custom") {
        std::vector<std::wstring> names =
            ClockSplitList(settings.weekdayFormatCustom, L',');
        if (time.wDayOfWeek < names.size()) {
            return names[time.wDayOfWeek];
        }
        return L"-";
    }

    std::wstring format =
        settings.weekdayFormat.empty() ? L"dddd" : settings.weekdayFormat;
    if (!ClockGetDateFormatEx_Original) {
        return L"";
    }

    WCHAR buffer[128] = {};
    ClockGetDateFormatEx_Original(
        settings.dateLocale.empty() ? nullptr : settings.dateLocale.c_str(),
        DATE_AUTOLAYOUT, &time, format.c_str(), buffer, ARRAYSIZE(buffer),
        nullptr);
    return buffer;
}

bool ClockIsLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int ClockDayOfYear(SYSTEMTIME const& time) {
    static const int kDaysBeforeMonth[] = {0,  31, 59,  90,  120, 151,
                                           181, 212, 243, 273, 304, 334};
    int day = kDaysBeforeMonth[time.wMonth > 0 && time.wMonth <= 12
                                   ? time.wMonth - 1
                                   : 0] +
              time.wDay;
    if (time.wMonth > 2 && ClockIsLeapYear(time.wYear)) {
        day++;
    }
    return day;
}

int ClockIsoWeeksInYear(int year) {
    SYSTEMTIME jan1{};
    jan1.wYear = (WORD)year;
    jan1.wMonth = 1;
    jan1.wDay = 1;
    FILETIME ft{};
    SYSTEMTIME jan1Utc{};
    if (!SystemTimeToFileTime(&jan1, &ft) ||
        !FileTimeToSystemTime(&ft, &jan1Utc)) {
        return 52;
    }
    return (jan1Utc.wDayOfWeek == 4 ||
            (jan1Utc.wDayOfWeek == 3 && ClockIsLeapYear(year)))
               ? 53
               : 52;
}

int ClockIsoWeekNumber(SYSTEMTIME const& time) {
    int weekdayIso = time.wDayOfWeek == 0 ? 7 : time.wDayOfWeek;
    int week = (ClockDayOfYear(time) - weekdayIso + 10) / 7;
    if (week < 1) {
        week = ClockIsoWeeksInYear(time.wYear - 1);
    } else if (week > ClockIsoWeeksInYear(time.wYear)) {
        week = 1;
    }
    return week;
}

int ClockWeekNumber(SYSTEMTIME const& time, int startDayOfWeek) {
    int dayOfYear = ClockDayOfYear(time);
    int jan1DayOfWeek =
        (time.wDayOfWeek - (dayOfYear - 1)) % 7;
    if (jan1DayOfWeek < 0) {
        jan1DayOfWeek += 7;
    }
    int offset = (jan1DayOfWeek - startDayOfWeek + 7) % 7;
    return (dayOfYear - 1 + offset) / 7 + 1;
}

int ClockStartDayOfWeek() {
    DWORD firstDayOfWeek = 0;
    WCHAR value[64];
    if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, value,
                      ARRAYSIZE(value))) {
        firstDayOfWeek = _wtoi(value);
    }
    // LOCALE_IFIRSTDAYOFWEEK is 0 = Monday ... 6 = Sunday, while SYSTEMTIME's
    // wDayOfWeek is 0 = Sunday ... 6 = Saturday.
    return (int)((firstDayOfWeek + 1) % 7);
}

std::wstring ClockFormatTimeZone() {
    TIME_ZONE_INFORMATION tzi{};
    DWORD type = GetTimeZoneInformation(&tzi);

    LONG bias = tzi.Bias;
    if (type == TIME_ZONE_ID_DAYLIGHT) {
        bias += tzi.DaylightBias;
    } else if (type == TIME_ZONE_ID_STANDARD) {
        bias += tzi.StandardBias;
    }

    LONG offsetMinutes = -bias;
    WCHAR buffer[32];
    swprintf_s(buffer, L"UTC%+03ld:%02ld", offsetMinutes / 60,
               labs(offsetMinutes % 60));
    return buffer;
}

void ClockFormatPercent(int value,
                        int maxValue,
                        ClockPercentageFormat format,
                        PWSTR buffer,
                        size_t bufferSize) {
    if (value > maxValue) {
        value = maxValue;
    }

    PCWSTR padding = L"";
    PCWSTR suffix = L"";

    switch (format) {
        case ClockPercentageFormat::spacePaddingAndSymbol:
            padding = L"  ";
            suffix = L"%";
            break;

        case ClockPercentageFormat::spacePadding:
            padding = L"  ";
            break;

        case ClockPercentageFormat::singleSpacePadding:
            padding = L" ";
            break;

        case ClockPercentageFormat::zeroPadding:
            padding = L"0";
            break;

        case ClockPercentageFormat::noPadding:
            break;
    }

    PCWSTR prefix = value < 10 ? padding : L"";
    swprintf_s(buffer, bufferSize, L"%s%d%s", prefix, value, suffix);
}

// ---- System performance metrics -------------------------------------------

struct ClockMetrics {
    int cpuPercent = 0;
    int ramPercent = 0;
    double ramUsedGb = 0.0;
    double ramTotalGb = 0.0;
    int cpuTempC = 0;
    bool cpuTempValid = false;
    int gpuPercent = 0;
    double networkUploadBps = 0.0;
    double networkDownloadBps = 0.0;
    double diskReadBps = 0.0;
    double diskWriteBps = 0.0;
    double vramUsedBytes = 0.0;
    double vramTotalBytes = 0.0;
    double vramSharedUsedBytes = 0.0;
    double vramSharedTotalBytes = 0.0;
    bool networkValid = false;
    bool diskValid = false;
    bool vramValid = false;
    bool vramSharedValid = false;
    int batteryPercent = 0;
    bool batteryValid = false;
};

std::mutex g_clockMetricsMutex;
ClockMetrics g_clockMetrics;
ULONGLONG g_clockLastSampleTick = 0;
ULONGLONG g_clockPrevIdleTime = 0;
ULONGLONG g_clockPrevKernelTime = 0;
ULONGLONG g_clockPrevUserTime = 0;

PDH_HQUERY g_clockPdhQuery = nullptr;
std::vector<PDH_HCOUNTER> g_clockCpuTempCounters;
std::vector<PDH_HCOUNTER> g_clockGpuCounters;
std::vector<PDH_HCOUNTER> g_clockNetworkUploadCounters;
std::vector<PDH_HCOUNTER> g_clockNetworkDownloadCounters;
std::vector<PDH_HCOUNTER> g_clockDiskReadCounters;
std::vector<PDH_HCOUNTER> g_clockDiskWriteCounters;
std::vector<PDH_HCOUNTER> g_clockVramUsedCounters;
std::vector<PDH_HCOUNTER> g_clockVramSharedUsedCounters;
ULONGLONG g_clockPdhCountersRefreshTick = 0;

bool ClockContainsNoCase(std::wstring_view text, std::wstring_view needle) {
    if (needle.empty()) {
        return true;
    }
    if (needle.size() > text.size()) {
        return false;
    }
    for (size_t i = 0; i + needle.size() <= text.size(); i++) {
        bool match = true;
        for (size_t j = 0; j < needle.size(); j++) {
            if (towlower(text[i + j]) != towlower(needle[j])) {
                match = false;
                break;
            }
        }
        if (match) {
            return true;
        }
    }
    return false;
}

struct ClockGpuAdapterInfo {
    std::wstring description;
    std::wstring luid;
    double dedicatedTotalGb = 0.0;
    double sharedTotalGb = 0.0;
};

std::optional<ClockGpuAdapterInfo> ClockGetGpuAdapterInfo(
    PCWSTR adapterFilter) {
    IDXGIFactory1* factory = nullptr;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr) || !factory) {
        return std::nullopt;
    }

    DXGI_ADAPTER_DESC bestDesc{};
    bool found = false;
    for (UINT i = 0;; i++) {
        IDXGIAdapter* adapter = nullptr;
        hr = factory->EnumAdapters(i, &adapter);
        if (hr == DXGI_ERROR_NOT_FOUND) {
            break;
        }
        if (FAILED(hr) || !adapter) {
            break;
        }

        DXGI_ADAPTER_DESC desc{};
        if (SUCCEEDED(adapter->GetDesc(&desc))) {
            bool nameMatches =
                !adapterFilter || !*adapterFilter ||
                ClockContainsNoCase(desc.Description, adapterFilter);
            if (nameMatches &&
                (!found ||
                 desc.DedicatedVideoMemory > bestDesc.DedicatedVideoMemory)) {
                bestDesc = desc;
                found = true;
                if (adapterFilter && *adapterFilter) {
                    adapter->Release();
                    break;
                }
            }
        }
        adapter->Release();
    }
    factory->Release();

    if (!found) {
        return std::nullopt;
    }

    WCHAR luid[32];
    swprintf_s(luid, L"0x%08X_0x%08X", bestDesc.AdapterLuid.HighPart,
               bestDesc.AdapterLuid.LowPart);
    constexpr double kGb = 1024.0 * 1024.0 * 1024.0;
    return ClockGpuAdapterInfo{
        bestDesc.Description,
        luid,
        bestDesc.DedicatedVideoMemory / kGb,
        bestDesc.SharedSystemMemory / kGb,
    };
}

void ClockAddPdhWildcardCounters(PDH_HQUERY query,
                                 PCWSTR englishPath,
                                 std::vector<PDH_HCOUNTER>& counters,
                                 PCWSTR instanceFilter = nullptr) {
    for (PDH_HCOUNTER counter : counters) {
        PdhRemoveCounter(counter);
    }
    counters.clear();

    if (!wcschr(englishPath, L'*')) {
        PDH_HCOUNTER counter = nullptr;
        if (PdhAddEnglishCounter(query, englishPath, 0, &counter) ==
            ERROR_SUCCESS) {
            counters.push_back(counter);
        }
        return;
    }

    PDH_HCOUNTER tempCounter = nullptr;
    if (PdhAddEnglishCounter(query, englishPath, 0, &tempCounter) !=
        ERROR_SUCCESS) {
        return;
    }

    DWORD size = 0;
    PDH_STATUS status = PdhGetCounterInfo(tempCounter, FALSE, &size, nullptr);
    if (status != (PDH_STATUS)PDH_MORE_DATA || !size) {
        PdhRemoveCounter(tempCounter);
        return;
    }

    std::vector<BYTE> infoBuffer(size);
    auto* counterInfo =
        reinterpret_cast<PDH_COUNTER_INFO*>(infoBuffer.data());
    status = PdhGetCounterInfo(tempCounter, FALSE, &size, counterInfo);
    PdhRemoveCounter(tempCounter);
    if (status != ERROR_SUCCESS) {
        return;
    }

    DWORD pathSize = 0;
    status = PdhExpandWildCardPath(nullptr, counterInfo->szFullPath, nullptr,
                                   &pathSize, 0);
    if (status != (PDH_STATUS)PDH_MORE_DATA || !pathSize) {
        return;
    }

    std::vector<WCHAR> paths(pathSize);
    status =
        PdhExpandWildCardPath(nullptr, counterInfo->szFullPath, paths.data(),
                              &pathSize, 0);
    if (status != ERROR_SUCCESS) {
        return;
    }

    for (PCWSTR path = paths.data(); *path; path += wcslen(path) + 1) {
        if (instanceFilter && *instanceFilter &&
            !ClockContainsNoCase(path, instanceFilter)) {
            continue;
        }
        PDH_HCOUNTER counter = nullptr;
        if (PdhAddCounter(query, path, 0, &counter) == ERROR_SUCCESS) {
            counters.push_back(counter);
        }
    }
}

void ClockRefreshPdhCountersIfNeeded(ULONGLONG now) {
    if (g_clockPdhCountersRefreshTick &&
        now - g_clockPdhCountersRefreshTick < 30000) {
        return;
    }
    g_clockPdhCountersRefreshTick = now;
    ClockAddPdhWildcardCounters(
        g_clockPdhQuery, L"\\Thermal Zone Information(*)\\Temperature",
        g_clockCpuTempCounters);

    ClockSettings settings = GetClockSettings();
    auto gpuInfo = ClockGetGpuAdapterInfo(settings.gpuAdapterName.c_str());
    PCWSTR gpuFilter =
        gpuInfo ? gpuInfo->luid.c_str() : nullptr;
    PCWSTR networkFilter =
        settings.networkAdapterName.empty()
            ? nullptr
            : settings.networkAdapterName.c_str();

    ClockAddPdhWildcardCounters(g_clockPdhQuery,
                                L"\\GPU Engine(*)\\Utilization Percentage",
                                g_clockGpuCounters, gpuFilter);
    ClockAddPdhWildcardCounters(
        g_clockPdhQuery, L"\\Network Interface(*)\\Bytes Sent/sec",
        g_clockNetworkUploadCounters, networkFilter);
    ClockAddPdhWildcardCounters(
        g_clockPdhQuery, L"\\Network Interface(*)\\Bytes Received/sec",
        g_clockNetworkDownloadCounters, networkFilter);
    ClockAddPdhWildcardCounters(
        g_clockPdhQuery, L"\\PhysicalDisk(_Total)\\Disk Read Bytes/sec",
        g_clockDiskReadCounters);
    ClockAddPdhWildcardCounters(
        g_clockPdhQuery, L"\\PhysicalDisk(_Total)\\Disk Write Bytes/sec",
        g_clockDiskWriteCounters);
    ClockAddPdhWildcardCounters(
        g_clockPdhQuery, L"\\GPU Adapter Memory(*)\\Dedicated Usage",
        g_clockVramUsedCounters, gpuFilter);
    ClockAddPdhWildcardCounters(
        g_clockPdhQuery, L"\\GPU Adapter Memory(*)\\Shared Usage",
        g_clockVramSharedUsedCounters, gpuFilter);

    if (gpuInfo) {
        g_clockMetrics.vramTotalBytes =
            gpuInfo->dedicatedTotalGb * 1024.0 * 1024.0 * 1024.0;
        g_clockMetrics.vramSharedTotalBytes =
            gpuInfo->sharedTotalGb * 1024.0 * 1024.0 * 1024.0;
        g_clockMetrics.vramValid = gpuInfo->dedicatedTotalGb > 0.0;
        g_clockMetrics.vramSharedValid = gpuInfo->sharedTotalGb > 0.0;
    } else {
        g_clockMetrics.vramValid = false;
        g_clockMetrics.vramSharedValid = false;
    }
}

void ClockSampleMetrics() {
    ClockSettings settings = GetClockSettings();
    if (!settings.enabled) {
        return;
    }

    ULONGLONG now = GetTickCount64();

    std::lock_guard<std::mutex> lock(g_clockMetricsMutex);
    if (g_clockLastSampleTick &&
        now - g_clockLastSampleTick <
            (ULONGLONG)settings.updateInterval * 1000) {
        return;
    }
    g_clockLastSampleTick = now;

    // CPU usage.
    FILETIME idleTime{}, kernelTime{}, userTime{};
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        auto toUlongLong = [](FILETIME const& ft) {
            return ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
        };
        ULONGLONG idle = toUlongLong(idleTime);
        ULONGLONG kernel = toUlongLong(kernelTime);
        ULONGLONG user = toUlongLong(userTime);

        if (g_clockPrevIdleTime != 0 || g_clockPrevKernelTime != 0 ||
            g_clockPrevUserTime != 0) {
            ULONGLONG total = (kernel - g_clockPrevKernelTime) +
                              (user - g_clockPrevUserTime);
            ULONGLONG idleDelta = idle - g_clockPrevIdleTime;
            if (total > 0 && idleDelta <= total) {
                g_clockMetrics.cpuPercent =
                    (int)((total - idleDelta) * 100 / total);
            }
        }

        g_clockPrevIdleTime = idle;
        g_clockPrevKernelTime = kernel;
        g_clockPrevUserTime = user;
    }

    // Memory usage.
    MEMORYSTATUSEX memoryStatus{};
    memoryStatus.dwLength = sizeof(memoryStatus);
    if (GlobalMemoryStatusEx(&memoryStatus)) {
        g_clockMetrics.ramPercent = (int)memoryStatus.dwMemoryLoad;
        g_clockMetrics.ramTotalGb =
            (double)memoryStatus.ullTotalPhys / (1024.0 * 1024 * 1024);
        g_clockMetrics.ramUsedGb = (double)(memoryStatus.ullTotalPhys -
                                            memoryStatus.ullAvailPhys) /
                                   (1024.0 * 1024 * 1024);
    }

    // Battery.
    SYSTEM_POWER_STATUS powerStatus{};
    if (GetSystemPowerStatus(&powerStatus) &&
        powerStatus.BatteryLifePercent != 255) {
        g_clockMetrics.batteryValid = true;
        g_clockMetrics.batteryPercent = powerStatus.BatteryLifePercent;
    } else {
        g_clockMetrics.batteryValid = false;
    }

    // PDH-based metrics (CPU temperature and GPU usage).
    if (!g_clockPdhQuery) {
        if (PdhOpenQuery(nullptr, 0, &g_clockPdhQuery) != ERROR_SUCCESS) {
            g_clockPdhQuery = nullptr;
            return;
        }
        g_clockPdhCountersRefreshTick = 0;
    }

    ClockRefreshPdhCountersIfNeeded(now);

    if (PdhCollectQueryData(g_clockPdhQuery) != ERROR_SUCCESS) {
        return;
    }

    {
        double sum = 0.0;
        int count = 0;
        for (PDH_HCOUNTER counter : g_clockCpuTempCounters) {
            PDH_FMT_COUNTERVALUE value{};
            if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr,
                                            &value) == ERROR_SUCCESS &&
                value.doubleValue >= 200.0) {
                sum += value.doubleValue;
                count++;
            }
        }
        if (count > 0) {
            g_clockMetrics.cpuTempValid = true;
            g_clockMetrics.cpuTempC =
                (int)(sum / count - 273.15);
        } else {
            g_clockMetrics.cpuTempValid = false;
        }
    }

    {
        double sum = 0.0;
        for (PDH_HCOUNTER counter : g_clockGpuCounters) {
            PDH_FMT_COUNTERVALUE value{};
            if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr,
                                            &value) == ERROR_SUCCESS) {
                sum += value.doubleValue;
            }
        }
        g_clockMetrics.gpuPercent = (int)std::min(100.0, sum + 0.5);
    }

    auto sumCounters = [](std::vector<PDH_HCOUNTER> const& counters,
                          bool& valid) {
        double sum = 0.0;
        size_t count = 0;
        for (PDH_HCOUNTER counter : counters) {
            PDH_FMT_COUNTERVALUE value{};
            if (PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr,
                                            &value) == ERROR_SUCCESS) {
                sum += value.doubleValue;
                count++;
            }
        }
        valid = count > 0;
        return sum;
    };

    {
        bool uploadValid = false;
        bool downloadValid = false;
        double upload =
            sumCounters(g_clockNetworkUploadCounters, uploadValid);
        double download =
            sumCounters(g_clockNetworkDownloadCounters, downloadValid);
        g_clockMetrics.networkUploadBps = upload;
        g_clockMetrics.networkDownloadBps = download;
        g_clockMetrics.networkValid = uploadValid || downloadValid;
    }

    {
        bool readValid = false;
        bool writeValid = false;
        g_clockMetrics.diskReadBps =
            sumCounters(g_clockDiskReadCounters, readValid);
        g_clockMetrics.diskWriteBps =
            sumCounters(g_clockDiskWriteCounters, writeValid);
        g_clockMetrics.diskValid = readValid || writeValid;
    }

    {
        bool dedicatedValid = false;
        bool sharedValid = false;
        g_clockMetrics.vramUsedBytes =
            sumCounters(g_clockVramUsedCounters, dedicatedValid);
        g_clockMetrics.vramSharedUsedBytes =
            sumCounters(g_clockVramSharedUsedCounters, sharedValid);
        g_clockMetrics.vramValid =
            dedicatedValid && g_clockMetrics.vramTotalBytes > 0.0;
        g_clockMetrics.vramSharedValid =
            sharedValid && g_clockMetrics.vramSharedTotalBytes > 0.0;
    }

    static ULONGLONG s_lastFeatureDiagTick = 0;
    if (!s_lastFeatureDiagTick || now - s_lastFeatureDiagTick >= 30000) {
        s_lastFeatureDiagTick = now;
        WCHAR diag[900];
        swprintf_s(
            diag,
            L"metrics cpu=%d gpu=%d net=%.2f/%.2f Bps netValid=%d "
            L"disk=%.2f/%.2f Bps diskValid=%d vram=%.1f/%.1f GB "
            L"vramValid=%d shared=%.1f/%.1f GB sharedValid=%d",
            g_clockMetrics.cpuPercent, g_clockMetrics.gpuPercent,
            g_clockMetrics.networkDownloadBps,
            g_clockMetrics.networkUploadBps,
            g_clockMetrics.networkValid ? 1 : 0,
            g_clockMetrics.diskReadBps, g_clockMetrics.diskWriteBps,
            g_clockMetrics.diskValid ? 1 : 0,
            g_clockMetrics.vramUsedBytes / (1024.0 * 1024.0 * 1024.0),
            g_clockMetrics.vramTotalBytes / (1024.0 * 1024.0 * 1024.0),
            g_clockMetrics.vramValid ? 1 : 0,
            g_clockMetrics.vramSharedUsedBytes /
                (1024.0 * 1024.0 * 1024.0),
            g_clockMetrics.vramSharedTotalBytes /
                (1024.0 * 1024.0 * 1024.0),
            g_clockMetrics.vramSharedValid ? 1 : 0);
        AppendClockFeatureDiag(diag);
    }
}

void ClockFormatTransfer(double bytesPerSecond,
                         ClockTransferFormat format,
                         int fixedDecimals,
                         PWSTR buffer,
                         size_t bufferSize) {
    constexpr double kKb = 1024.0;
    constexpr double kMb = 1024.0 * 1024.0;
    constexpr double kKbit = 1000.0 / 8.0;
    constexpr double kMbit = 1000.0 * 1000.0 / 8.0;

    double value = 0.0;
    PCWSTR unit = L"";
    switch (format) {
        case ClockTransferFormat::mbs:
            value = bytesPerSecond / kMb;
            unit = L" MB/s";
            break;
        case ClockTransferFormat::mbsNumberOnly:
            value = bytesPerSecond / kMb;
            break;
        case ClockTransferFormat::mbsDynamic:
            if (bytesPerSecond < 1000.0 * kKb) {
                value = bytesPerSecond / kKb;
                unit = L" KB/s";
            } else {
                value = bytesPerSecond / kMb;
                unit = L" MB/s";
            }
            break;
        case ClockTransferFormat::mbits:
            value = bytesPerSecond / kMbit;
            unit = L" MBit/s";
            break;
        case ClockTransferFormat::mbitsNumberOnly:
            value = bytesPerSecond / kMbit;
            break;
        case ClockTransferFormat::mbitsDynamic:
            if (bytesPerSecond < 1000.0 * kKbit) {
                value = bytesPerSecond / kKbit;
                unit = L" KBit/s";
            } else {
                value = bytesPerSecond / kMbit;
                unit = L" MBit/s";
            }
            break;
    }

    int decimals = fixedDecimals;
    if (decimals < 0) {
        decimals = value < 10.0 ? 2 : (value < 100.0 ? 1 : 0);
    }
    swprintf_s(buffer, bufferSize, L"%.*f%s", decimals, value, unit);
}

bool ClockAnyLineContains(ClockSettings const& settings,
                          std::wstring_view token) {
    return settings.topLine.find(token) != std::wstring::npos ||
           settings.bottomLine.find(token) != std::wstring::npos ||
           settings.tooltipLine.find(token) != std::wstring::npos ||
           (settings.topLine.find(L"%media_info%") != std::wstring::npos &&
            settings.mediaInfoFormat.find(token) != std::wstring::npos) ||
           (settings.bottomLine.find(L"%media_info%") != std::wstring::npos &&
            settings.mediaInfoFormat.find(token) != std::wstring::npos) ||
           (settings.tooltipLine.find(L"%media_info%") != std::wstring::npos &&
            settings.mediaInfoFormat.find(token) != std::wstring::npos);
}

std::mutex g_clockWebMutex;
std::atomic<bool> g_clockWebLoaded{false};
std::vector<std::optional<std::wstring>> g_clockWebStrings;
std::vector<std::optional<std::wstring>> g_clockWebStringsFull;
std::optional<std::wstring> g_clockWeatherString;
std::atomic<HANDLE> g_clockWebThread{nullptr};
HANDLE g_clockWebRefreshEvent = nullptr;
HANDLE g_clockWebStopEvent = nullptr;
std::mutex g_clockHttpMutex;
HINTERNET g_clockActiveHttpRequest = nullptr;

void ClockCancelActiveHttpRequest() {
    std::lock_guard<std::mutex> lock(g_clockHttpMutex);
    if (g_clockActiveHttpRequest) {
        InternetCloseHandle(g_clockActiveHttpRequest);
        g_clockActiveHttpRequest = nullptr;
    }
}

std::wstring ClockEscapeUrl(std::wstring_view value) {
    if (value.empty()) {
        return {};
    }

    std::wstring input(value);
    DWORD size = 512;
    std::wstring output(size, L'\0');
    HRESULT hr = UrlEscapeW(
        input.c_str(), output.data(), &size,
        URL_ESCAPE_ASCII_URI_COMPONENT | URL_ESCAPE_AS_UTF8);
    if (hr == E_POINTER) {
        output.resize(size);
        hr = UrlEscapeW(
            input.c_str(), output.data(), &size,
            URL_ESCAPE_ASCII_URI_COMPONENT | URL_ESCAPE_AS_UTF8);
    }
    if (FAILED(hr)) {
        return {};
    }
    output.resize(wcslen(output.c_str()));
    return output;
}

std::optional<std::wstring> ClockHttpGet(std::wstring const& url) {
    HINTERNET internet = InternetOpenW(
        L"taskbar-modding clock", INTERNET_OPEN_TYPE_PRECONFIG, nullptr,
        nullptr, 0);
    if (!internet) {
        return std::nullopt;
    }

    DWORD timeout = 8000;
    InternetSetOptionW(internet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout,
                       sizeof(timeout));
    InternetSetOptionW(internet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout,
                       sizeof(timeout));

    HINTERNET request = InternetOpenUrlW(
        internet, url.c_str(), nullptr, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE |
            INTERNET_FLAG_SECURE,
        0);
    if (!request) {
        InternetCloseHandle(internet);
        return std::nullopt;
    }
    {
        std::lock_guard<std::mutex> lock(g_clockHttpMutex);
        g_clockActiveHttpRequest = request;
    }

    std::string bytes;
    char buffer[8192];
    DWORD read = 0;
    while (InternetReadFile(request, buffer, sizeof(buffer), &read) && read) {
        if (bytes.size() + read > 4 * 1024 * 1024) {
            break;
        }
        bytes.append(buffer, read);
    }
    DWORD error = GetLastError();
    {
        std::lock_guard<std::mutex> lock(g_clockHttpMutex);
        if (g_clockActiveHttpRequest == request) {
            g_clockActiveHttpRequest = nullptr;
            InternetCloseHandle(request);
        }
    }
    InternetCloseHandle(internet);
    if (bytes.empty() && error != ERROR_SUCCESS) {
        return std::nullopt;
    }

    int chars = MultiByteToWideChar(CP_UTF8, 0, bytes.data(),
                                    (int)bytes.size(), nullptr, 0);
    if (chars <= 0) {
        return std::nullopt;
    }
    std::wstring result(chars, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                        result.data(), chars);
    return result;
}

std::wstring ClockStripMarkup(std::wstring text) {
    std::wstring plain;
    plain.reserve(text.size());
    for (size_t i = 0; i < text.size();) {
        constexpr std::wstring_view kCdataStart = L"<![CDATA[";
        constexpr std::wstring_view kCdataEnd = L"]]>";
        if (text.compare(i, kCdataStart.size(), kCdataStart) == 0) {
            size_t start = i + kCdataStart.size();
            size_t end = text.find(kCdataEnd, start);
            if (end == std::wstring::npos) {
                plain.append(text, start, std::wstring::npos);
                break;
            }
            plain.append(text, start, end - start);
            i = end + kCdataEnd.size();
            continue;
        }
        if (text[i] == L'<') {
            size_t end = text.find(L'>', i + 1);
            if (end == std::wstring::npos) {
                break;
            }
            plain.push_back(L' ');
            i = end + 1;
            continue;
        }
        plain.push_back(text[i]);
        i++;
    }

    struct Entity {
        PCWSTR entity;
        PCWSTR value;
    };
    static const Entity kEntities[] = {
        {L"&amp;", L"&"},   {L"&lt;", L"<"},   {L"&gt;", L">"},
        {L"&quot;", L"\""}, {L"&apos;", L"'"},
        {L"&#39;", L"'"},
        {L"&nbsp;", L" "},
    };
    for (auto const& entity : kEntities) {
        plain = ClockReplaceAll(plain, entity.entity, entity.value);
    }

    std::wstring decoded;
    decoded.reserve(plain.size());
    for (size_t i = 0; i < plain.size();) {
        if (plain[i] == L'&' && i + 2 < plain.size() &&
            plain[i + 1] == L'#') {
            size_t end = plain.find(L';', i + 2);
            if (end != std::wstring::npos && end - i <= 12) {
                size_t pos = i + 2;
                bool hex = pos < end &&
                           (plain[pos] == L'x' || plain[pos] == L'X');
                if (hex) {
                    pos++;
                }
                uint32_t codePoint = 0;
                bool valid = pos < end;
                for (; pos < end; pos++) {
                    wchar_t ch = plain[pos];
                    uint32_t digit;
                    if (ch >= L'0' && ch <= L'9') {
                        digit = ch - L'0';
                    } else if (hex && ch >= L'a' && ch <= L'f') {
                        digit = ch - L'a' + 10;
                    } else if (hex && ch >= L'A' && ch <= L'F') {
                        digit = ch - L'A' + 10;
                    } else {
                        valid = false;
                        break;
                    }
                    codePoint = codePoint * (hex ? 16 : 10) + digit;
                }
                if (valid && codePoint > 0 && codePoint <= 0x10FFFF) {
                    if (codePoint <= 0xFFFF) {
                        decoded.push_back((wchar_t)codePoint);
                    } else {
                        codePoint -= 0x10000;
                        decoded.push_back(
                            (wchar_t)(0xD800 + (codePoint >> 10)));
                        decoded.push_back(
                            (wchar_t)(0xDC00 + (codePoint & 0x3FF)));
                    }
                    i = end + 1;
                    continue;
                }
            }
        }
        decoded.push_back(plain[i++]);
    }
    plain = std::move(decoded);

    std::wstring collapsed;
    collapsed.reserve(plain.size());
    bool lastSpace = false;
    for (wchar_t ch : plain) {
        bool space = iswspace(ch) != 0;
        if (space && lastSpace) {
            continue;
        }
        collapsed.push_back(space ? L' ' : ch);
        lastSpace = space;
    }
    return ClockTrim(collapsed);
}

std::wstring ClockTruncate(std::wstring text, int maxLength) {
    if (maxLength > 0 && text.size() > (size_t)maxLength) {
        text.resize(maxLength);
        if (text.size() >= 3) {
            text[text.size() - 3] = L'.';
            text[text.size() - 2] = L'.';
            text[text.size() - 1] = L'.';
        }
    }
    return text;
}

std::wstring ClockExtractWebContent(
    std::wstring const& content,
    ClockWebContentSettings const& item) {
    size_t blockStart = 0;
    if (!item.blockStart.empty()) {
        blockStart = content.find(item.blockStart);
        if (blockStart == std::wstring::npos) {
            return {};
        }
    }

    size_t start = blockStart;
    if (!item.start.empty()) {
        start = content.find(item.start, blockStart);
        if (start == std::wstring::npos) {
            return {};
        }
        start += item.start.size();
    }

    size_t end = content.size();
    if (!item.end.empty()) {
        end = content.find(item.end, start);
        if (end == std::wstring::npos) {
            end = content.size();
        }
    }

    return ClockStripMarkup(content.substr(start, end - start));
}

void ClockUpdateWebContent() {
    ClockSettings settings = GetClockSettings();
    if (!settings.enabled) {
        return;
    }

    bool needWeb = ClockAnyLineContains(settings, L"%news%");
    for (size_t i = 0; i < settings.webContentsItems.size(); i++) {
        WCHAR token[32];
        swprintf_s(token, L"%%web%zu%%", i + 1);
        WCHAR tokenFull[40];
        swprintf_s(tokenFull, L"%%web%zu_full%%", i + 1);
        if (ClockAnyLineContains(settings, token) ||
            ClockAnyLineContains(settings, tokenFull)) {
            needWeb = true;
            break;
        }
    }
    bool needWeather = ClockAnyLineContains(settings, L"%weather%");
    if (!needWeb && !needWeather) {
        return;
    }

    std::vector<std::optional<std::wstring>> values;
    std::vector<std::optional<std::wstring>> fullValues;
    values.resize(settings.webContentsItems.size());
    fullValues.resize(settings.webContentsItems.size());

    if (needWeb) {
        std::wstring lastUrl;
        std::optional<std::wstring> cachedContent;
        for (size_t i = 0; i < settings.webContentsItems.size(); i++) {
            auto const& item = settings.webContentsItems[i];
            if (item.url != lastUrl) {
                lastUrl = item.url;
                cachedContent = ClockHttpGet(item.url);
            }
            if (!cachedContent) {
                continue;
            }
            std::wstring extracted =
                ClockExtractWebContent(*cachedContent, item);
            fullValues[i] = extracted;
            values[i] = ClockTruncate(extracted, item.maxLength);
        }
    }

    std::optional<std::wstring> weather;
    if (needWeather) {
        std::wstring format = settings.weatherFormat.empty()
                                  ? L"%c %t %w"
                                  : settings.weatherFormat;
        // Replace %c with a marker so spaces added by wttr.in can be removed.
        format = ClockReplaceAll(format, L"%c", L"%c\uE000");
        std::wstring url = L"https://wttr.in/";
        url += ClockEscapeUrl(settings.weatherLocation);
        url += L"?";
        if (settings.weatherUnits == L"uscs") {
            url += L"u&";
        } else if (settings.weatherUnits == L"metric") {
            url += L"m&";
        } else if (settings.weatherUnits == L"metricMsWind") {
            url += L"M&";
        }
        url += L"format=";
        url += ClockEscapeUrl(format);
        auto content = ClockHttpGet(url);
        if (content && !content->empty() &&
            *content != L"This query is already being processed") {
            std::wstring cleaned;
            size_t pos = 0;
            while (pos < content->size()) {
                size_t marker = content->find(L'\uE000', pos);
                if (marker == std::wstring::npos) {
                    cleaned.append(*content, pos, std::wstring::npos);
                    break;
                }
                size_t end = marker;
                while (end > pos && (*content)[end - 1] == L' ') {
                    end--;
                }
                cleaned.append(*content, pos, end - pos);
                pos = marker + 1;
            }
            weather = std::move(cleaned);
        }
    }

    size_t webSuccess = 0;
    for (auto const& value : fullValues) {
        if (value) {
            webSuccess++;
        }
    }
    bool weatherSuccess = weather.has_value();
    bool allSuccess =
        (!needWeb ||
         webSuccess == settings.webContentsItems.size()) &&
        (!needWeather || weatherSuccess);
    {
        std::lock_guard<std::mutex> lock(g_clockWebMutex);
        g_clockWebStrings = std::move(values);
        g_clockWebStringsFull = std::move(fullValues);
        if (weather) {
            g_clockWeatherString = std::move(weather);
        }
    }
    g_clockWebLoaded = allSuccess;

    WCHAR diag[320];
    swprintf_s(diag,
               L"web update items=%zu/%zu weather=%d content=%d",
               webSuccess, settings.webContentsItems.size(),
               weatherSuccess ? 1 : 0, needWeb ? 1 : 0);
    AppendClockFeatureDiag(diag);
}

DWORD WINAPI ClockWebThreadProc(LPVOID) {
    HANDLE events[2];
    {
        std::lock_guard<std::mutex> lock(g_clockWebMutex);
        events[0] = g_clockWebStopEvent;
        events[1] = g_clockWebRefreshEvent;
    }

    while (!g_unloading) {
        ClockUpdateWebContent();
        ClockSettings settings = GetClockSettings();
        DWORD waitSeconds =
            (DWORD)std::max(1, settings.webContentsUpdateInterval) * 60;
        if (!g_clockWebLoaded.load() && waitSeconds > 30) {
            waitSeconds = 30;
        }
        DWORD result = WaitForMultipleObjects(2, events, FALSE,
                                              waitSeconds * 1000);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) {
            break;
        }
    }
    return 0;
}

void ClockStartWebUpdateThreadIfNeeded() {
    if (g_hostKind != HostKind::Taskbar || g_unloading) {
        return;
    }

    ClockSettings settings = GetClockSettings();
    bool needed = ClockAnyLineContains(settings, L"%weather%") ||
                  ClockAnyLineContains(settings, L"%news%");
    for (size_t i = 0; i < settings.webContentsItems.size() && !needed; i++) {
        WCHAR token[32];
        swprintf_s(token, L"%%web%zu%%", i + 1);
        WCHAR tokenFull[40];
        swprintf_s(tokenFull, L"%%web%zu_full%%", i + 1);
        needed = ClockAnyLineContains(settings, token) ||
                 ClockAnyLineContains(settings, tokenFull);
    }
    if (!needed || g_clockWebThread.load()) {
        if (g_clockWebRefreshEvent) {
            SetEvent(g_clockWebRefreshEvent);
        }
        return;
    }

    std::lock_guard<std::mutex> lock(g_clockWebMutex);
    g_clockWebRefreshEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_clockWebStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_clockWebRefreshEvent || !g_clockWebStopEvent) {
        if (g_clockWebRefreshEvent) {
            CloseHandle(g_clockWebRefreshEvent);
            g_clockWebRefreshEvent = nullptr;
        }
        if (g_clockWebStopEvent) {
            CloseHandle(g_clockWebStopEvent);
            g_clockWebStopEvent = nullptr;
        }
        return;
    }
    HANDLE thread = CreateThread(nullptr, 0, ClockWebThreadProc, nullptr, 0,
                                 nullptr);
    if (thread) {
        g_clockWebThread = thread;
    }
}

void ClockStopWebUpdateThread() {
    HANDLE thread = g_clockWebThread.exchange(nullptr);
    HANDLE refreshEvent = nullptr;
    HANDLE stopEvent = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_clockWebMutex);
        refreshEvent = g_clockWebRefreshEvent;
        stopEvent = g_clockWebStopEvent;
        g_clockWebRefreshEvent = nullptr;
        g_clockWebStopEvent = nullptr;
    }
    if (!thread) {
        return;
    }
    if (stopEvent) {
        SetEvent(stopEvent);
    }
    ClockCancelActiveHttpRequest();
    WaitForSingleObject(thread, 12000);
    CloseHandle(thread);
    if (refreshEvent) {
        CloseHandle(refreshEvent);
    }
    if (stopEvent) {
        CloseHandle(stopEvent);
    }

    std::lock_guard<std::mutex> lock(g_clockWebMutex);
    g_clockWebStrings.clear();
    g_clockWebStringsFull.clear();
    g_clockWeatherString.reset();
    g_clockWebLoaded = false;
}

std::mutex g_clockMediaMutex;
std::wstring g_clockMediaTitle;
std::wstring g_clockMediaArtist;
std::wstring g_clockMediaAlbum;
std::wstring g_clockMediaStatus;
bool g_clockMediaActive = false;
ULONGLONG g_clockMediaLastRefreshTick = 0;
winrt::Windows::Media::Control::
    GlobalSystemMediaTransportControlsSessionManager g_clockMediaManager{
        nullptr};

void ClockRemoveBracketContent(std::wstring& value, wchar_t open,
                               wchar_t close) {
    size_t start = 0;
    while ((start = value.find(open, start)) != std::wstring::npos) {
        size_t end = value.find(close, start + 1);
        if (end == std::wstring::npos) {
            break;
        }
        size_t removeStart = start;
        size_t removeEnd = end + 1;
        if (start > 0 && value[start - 1] == L' ') {
            removeStart--;
        } else if (end + 1 < value.size() && value[end + 1] == L' ') {
            removeEnd++;
        }
        value.erase(removeStart, removeEnd - removeStart);
        start = removeStart;
    }
}

bool ClockMediaPlayerIgnored(std::wstring appId,
                             ClockSettings const& settings) {
    std::transform(appId.begin(), appId.end(), appId.begin(), towlower);
    for (auto ignored : settings.mediaIgnoredPlayers) {
        if (ignored.empty()) {
            continue;
        }
        std::transform(ignored.begin(), ignored.end(), ignored.begin(),
                       towlower);
        if (appId.find(ignored) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

void ClockRefreshMediaData(ClockSettings const& settings) {
    if (!ClockAnyLineContains(settings, L"%media_")) {
        return;
    }

    ULONGLONG now = GetTickCount64();
    std::lock_guard<std::mutex> lock(g_clockMediaMutex);
    if (g_clockMediaLastRefreshTick &&
        now - g_clockMediaLastRefreshTick <
            (ULONGLONG)settings.updateInterval * 1000) {
        return;
    }
    g_clockMediaLastRefreshTick = now;

    try {
        if (!g_clockMediaManager) {
            g_clockMediaManager =
                winrt::Windows::Media::Control::
                    GlobalSystemMediaTransportControlsSessionManager::
                        RequestAsync()
                            .get();
        }
        auto session = g_clockMediaManager.GetCurrentSession();
        if (!session) {
            g_clockMediaActive = false;
            return;
        }
        if (ClockMediaPlayerIgnored(
                std::wstring(session.SourceAppUserModelId()), settings)) {
            g_clockMediaActive = false;
            return;
        }

        auto properties = session.TryGetMediaPropertiesAsync().get();
        if (!properties) {
            g_clockMediaActive = false;
            return;
        }

        auto playback = session.GetPlaybackInfo().PlaybackStatus();
        using MediaStatus = winrt::Windows::Media::Control::
            GlobalSystemMediaTransportControlsSessionPlaybackStatus;
        if (playback == MediaStatus::Playing) {
            g_clockMediaStatus = L"\u23EF";
        } else if (playback == MediaStatus::Paused) {
            g_clockMediaStatus = L"\u23F8";
        } else if (playback == MediaStatus::Stopped) {
            g_clockMediaStatus = L"\u23F9";
        } else {
            g_clockMediaStatus.clear();
        }

        auto normalize = [&](winrt::hstring const& value) {
            std::wstring result(value);
            if (settings.mediaRemoveBrackets) {
                ClockRemoveBracketContent(result, L'(', L')');
                ClockRemoveBracketContent(result, L'[', L']');
                result = ClockTrim(result);
            }
            return result;
        };
        g_clockMediaTitle = normalize(properties.Title());
        g_clockMediaArtist = normalize(properties.Artist());
        g_clockMediaAlbum = normalize(properties.AlbumTitle());
        g_clockMediaActive = true;

        static std::wstring s_lastMediaDiag;
        WCHAR diag[640];
        swprintf_s(diag, L"media title=%s artist=%s album=%s status=%s",
                   g_clockMediaTitle.c_str(), g_clockMediaArtist.c_str(),
                   g_clockMediaAlbum.c_str(), g_clockMediaStatus.c_str());
        if (s_lastMediaDiag != diag) {
            s_lastMediaDiag = diag;
            AppendClockFeatureDiag(diag);
        }
    } catch (...) {
        g_clockMediaActive = false;
    }
}

std::wstring ClockGetMediaField(int kind,
                                ClockSettings const& settings) {
    ClockRefreshMediaData(settings);
    std::lock_guard<std::mutex> lock(g_clockMediaMutex);
    switch (kind) {
        case 0:
            return g_clockMediaActive ? g_clockMediaTitle : L"";
        case 1:
            return g_clockMediaActive ? g_clockMediaArtist : L"";
        case 2:
            return g_clockMediaActive ? g_clockMediaAlbum : L"";
        case 3:
            return g_clockMediaActive ? g_clockMediaStatus : L"";
    }
    return L"";
}

std::wstring ClockGetMetricText(int kind, ClockSettings const& settings) {
    ClockSampleMetrics();

    ClockMetrics metrics;
    {
        std::lock_guard<std::mutex> lock(g_clockMetricsMutex);
        metrics = g_clockMetrics;
    }

    WCHAR buffer[128];

    switch (kind) {
        case 0:  // %cpu%
            ClockFormatPercent(metrics.cpuPercent, 100,
                               settings.percentageFormat, buffer,
                               ARRAYSIZE(buffer));
            return buffer;

        case 1:  // %ram%
            ClockFormatPercent(metrics.ramPercent, 100,
                               settings.percentageFormat, buffer,
                               ARRAYSIZE(buffer));
            return buffer;

        case 2:  // %ram_used%
            swprintf_s(buffer, L"%.1f GB", metrics.ramUsedGb);
            return buffer;

        case 3:  // %ram_total%
            swprintf_s(buffer, L"%.1f GB", metrics.ramTotalGb);
            return buffer;

        case 4:  // %cpu_temp%
            if (!metrics.cpuTempValid) {
                return L"-";
            }
            swprintf_s(buffer, L"%d\u00B0C", metrics.cpuTempC);
            return buffer;

        case 5:  // %cpu_temp_f%
            if (!metrics.cpuTempValid) {
                return L"-";
            }
            swprintf_s(buffer, L"%d\u00B0F",
                       (int)(metrics.cpuTempC * 9 / 5 + 32));
            return buffer;

        case 6:  // %gpu%
            ClockFormatPercent(metrics.gpuPercent, 100,
                               settings.percentageFormat, buffer,
                               ARRAYSIZE(buffer));
            return buffer;

        case 7:  // %battery%
            if (!metrics.batteryValid) {
                return L"-";
            }
            ClockFormatPercent(metrics.batteryPercent, 100,
                               settings.percentageFormat, buffer,
                               ARRAYSIZE(buffer));
            return buffer;

        case 8:  // %upload_speed%
        case 9:  // %download_speed%
        case 10: // %total_speed%
            if (!metrics.networkValid) {
                return L"-";
            }
            {
                double value = kind == 8
                                   ? metrics.networkUploadBps
                                   : (kind == 9
                                          ? metrics.networkDownloadBps
                                          : metrics.networkUploadBps +
                                                metrics.networkDownloadBps);
                ClockFormatTransfer(
                    value, settings.networkMetricsFormat,
                    settings.networkMetricsFixedDecimals, buffer,
                    ARRAYSIZE(buffer));
            }
            return buffer;

        case 11: // %disk_read%
        case 12: // %disk_write%
        case 13: // %disk_total%
            if (!metrics.diskValid) {
                return L"-";
            }
            {
                ClockTransferFormat format =
                    settings.diskMetricsSameAsNetwork
                        ? settings.networkMetricsFormat
                        : settings.diskMetricsFormat;
                int decimals = settings.diskMetricsSameAsNetwork
                                   ? settings.networkMetricsFixedDecimals
                                   : settings.diskMetricsFixedDecimals;
                double value = kind == 11
                                   ? metrics.diskReadBps
                                   : (kind == 12
                                          ? metrics.diskWriteBps
                                          : metrics.diskReadBps +
                                                metrics.diskWriteBps);
                ClockFormatTransfer(value, format, decimals, buffer,
                                    ARRAYSIZE(buffer));
            }
            return buffer;

        case 14: // %vram%
            if (!metrics.vramValid || metrics.vramTotalBytes <= 0.0) {
                return L"-";
            }
            ClockFormatPercent(
                (int)std::min(
                    100.0,
                    metrics.vramUsedBytes * 100.0 /
                        metrics.vramTotalBytes),
                100, settings.percentageFormat, buffer,
                ARRAYSIZE(buffer));
            return buffer;

        case 15: // %vram_used%
            if (!metrics.vramValid) {
                return L"-";
            }
            swprintf_s(buffer, L"%.1f GB",
                       metrics.vramUsedBytes / (1024.0 * 1024.0 * 1024.0));
            return buffer;

        case 16: // %vram_total%
            if (metrics.vramTotalBytes <= 0.0) {
                return L"-";
            }
            swprintf_s(buffer, L"%.1f GB",
                       metrics.vramTotalBytes / (1024.0 * 1024.0 * 1024.0));
            return buffer;

        case 17: // %vram_shared%
            if (!metrics.vramSharedValid ||
                metrics.vramSharedTotalBytes <= 0.0) {
                return L"-";
            }
            ClockFormatPercent(
                (int)std::min(100.0,
                              metrics.vramSharedUsedBytes * 100.0 /
                                  metrics.vramSharedTotalBytes),
                100, settings.percentageFormat, buffer,
                ARRAYSIZE(buffer));
            return buffer;

        case 18: // %vram_shared_used%
            if (!metrics.vramSharedValid) {
                return L"-";
            }
            swprintf_s(buffer, L"%.1f GB",
                       metrics.vramSharedUsedBytes /
                           (1024.0 * 1024.0 * 1024.0));
            return buffer;

        case 19: // %vram_shared_total%
            if (metrics.vramSharedTotalBytes <= 0.0) {
                return L"-";
            }
            swprintf_s(buffer, L"%.1f GB",
                       metrics.vramSharedTotalBytes /
                           (1024.0 * 1024.0 * 1024.0));
            return buffer;
    }

    return L"";
}

std::wstring ClockFormatLine(ClockSettings const& settings,
                             std::wstring const& format) {
    SYSTEMTIME time{};
    if (ClockGetLocalTime_Original) {
        ClockGetLocalTime_Original(&time);
    } else {
        GetLocalTime(&time);
    }

    std::wstring result;
    result.reserve(format.size() + 32);

    size_t i = 0;
    while (i < format.size()) {
        if (format[i] == L'%') {
            std::wstring_view rest(format.c_str() + i, format.size() - i);
            std::wstring replacement;
            size_t tokenLength = 0;

            if (rest.starts_with(L"%vram_shared_used%")) {
                tokenLength = 18;
                replacement = ClockGetMetricText(18, settings);
            } else if (rest.starts_with(L"%vram_shared_total%")) {
                tokenLength = 19;
                replacement = ClockGetMetricText(19, settings);
            } else if (rest.starts_with(L"%vram_shared%")) {
                tokenLength = 13;
                replacement = ClockGetMetricText(17, settings);
            } else if (rest.starts_with(L"%vram_used%")) {
                tokenLength = 11;
                replacement = ClockGetMetricText(15, settings);
            } else if (rest.starts_with(L"%vram_total%")) {
                tokenLength = 12;
                replacement = ClockGetMetricText(16, settings);
            } else if (rest.starts_with(L"%vram%")) {
                tokenLength = 6;
                replacement = ClockGetMetricText(14, settings);
            } else if (rest.starts_with(L"%upload_speed%")) {
                tokenLength = 14;
                replacement = ClockGetMetricText(8, settings);
            } else if (rest.starts_with(L"%download_speed%")) {
                tokenLength = 16;
                replacement = ClockGetMetricText(9, settings);
            } else if (rest.starts_with(L"%total_speed%")) {
                tokenLength = 13;
                replacement = ClockGetMetricText(10, settings);
            } else if (rest.starts_with(L"%disk_read%")) {
                tokenLength = 11;
                replacement = ClockGetMetricText(11, settings);
            } else if (rest.starts_with(L"%disk_write%")) {
                tokenLength = 12;
                replacement = ClockGetMetricText(12, settings);
            } else if (rest.starts_with(L"%disk_total%")) {
                tokenLength = 12;
                replacement = ClockGetMetricText(13, settings);
            } else if (rest.starts_with(L"%media_info%")) {
                tokenLength = 12;
                if (!g_clockInMediaInfo) {
                    ClockRefreshMediaData(settings);
                    bool active = false;
                    {
                        std::lock_guard<std::mutex> lock(g_clockMediaMutex);
                        active = g_clockMediaActive;
                    }
                    g_clockInMediaInfo = true;
                    replacement = ClockFormatLine(
                        settings, active ? settings.mediaInfoFormat
                                          : settings.noMediaText);
                    g_clockInMediaInfo = false;
                    if (settings.mediaMaxLength > 0) {
                        replacement = ClockTruncate(
                            std::move(replacement),
                            settings.mediaMaxLength);
                    }
                }
            } else if (rest.starts_with(L"%media_title%")) {
                tokenLength = 13;
                replacement = ClockGetMediaField(0, settings);
            } else if (rest.starts_with(L"%media_artist%")) {
                tokenLength = 14;
                replacement = ClockGetMediaField(1, settings);
            } else if (rest.starts_with(L"%media_album%")) {
                tokenLength = 13;
                replacement = ClockGetMediaField(2, settings);
            } else if (rest.starts_with(L"%media_status%")) {
                tokenLength = 14;
                replacement = ClockGetMediaField(3, settings);
            } else if (rest.starts_with(L"%weather%")) {
                tokenLength = 9;
                std::lock_guard<std::mutex> lock(g_clockWebMutex);
                replacement = g_clockWeatherString
                                  ? *g_clockWeatherString
                                  : L"Loading...";
            } else if (rest.starts_with(L"%news%")) {
                tokenLength = 6;
                std::lock_guard<std::mutex> lock(g_clockWebMutex);
                replacement = !g_clockWebStrings.empty() &&
                                      g_clockWebStrings[0]
                                  ? *g_clockWebStrings[0]
                                  : L"Loading...";
            } else if (rest.starts_with(L"%web")) {
                size_t digitPos = 4;
                int number = 0;
                while (digitPos < rest.size() &&
                       rest[digitPos] >= L'0' && rest[digitPos] <= L'9') {
                    number = number * 10 + (rest[digitPos] - L'0');
                    digitPos++;
                }
                if (number > 0 && digitPos < rest.size() &&
                    rest[digitPos] == L'%') {
                    tokenLength = digitPos + 1;
                    std::lock_guard<std::mutex> lock(g_clockWebMutex);
                    size_t index = (size_t)number - 1;
                    replacement =
                        index < g_clockWebStrings.size() &&
                                g_clockWebStrings[index]
                            ? *g_clockWebStrings[index]
                            : L"Loading...";
                } else if (number > 0 &&
                           rest.substr(digitPos).starts_with(L"_full%")) {
                    tokenLength = digitPos + 6;
                    std::lock_guard<std::mutex> lock(g_clockWebMutex);
                    size_t index = (size_t)number - 1;
                    replacement =
                        index < g_clockWebStringsFull.size() &&
                                g_clockWebStringsFull[index]
                            ? *g_clockWebStringsFull[index]
                            : L"Loading...";
                }
            } else if (rest.starts_with(L"%time%")) {
                tokenLength = 6;
                replacement = ClockFormatTime(time, settings.timeFormat,
                                              settings.showSeconds);
            } else if (rest.starts_with(L"%date%")) {
                tokenLength = 6;
                replacement = ClockFormatDate(time, settings.dateFormat,
                                              settings.dateLocale);
            } else if (rest.starts_with(L"%weekday_num%")) {
                tokenLength = 13;
                int dayNumber =
                    ((time.wDayOfWeek - ClockStartDayOfWeek() + 7) % 7) + 1;
                replacement = std::to_wstring(dayNumber);
            } else if (rest.starts_with(L"%weekday%")) {
                tokenLength = 9;
                replacement = ClockFormatWeekday(time, settings);
            } else if (rest.starts_with(L"%weeknum_iso%")) {
                tokenLength = 13;
                replacement = std::to_wstring(ClockIsoWeekNumber(time));
            } else if (rest.starts_with(L"%weeknum%")) {
                tokenLength = 9;
                replacement = std::to_wstring(
                    ClockWeekNumber(time, ClockStartDayOfWeek()));
            } else if (rest.starts_with(L"%dayofyear%")) {
                tokenLength = 11;
                replacement = std::to_wstring(ClockDayOfYear(time));
            } else if (rest.starts_with(L"%timezone%")) {
                tokenLength = 10;
                replacement = ClockFormatTimeZone();
            } else if (rest.starts_with(L"%cpu_temp_f%")) {
                tokenLength = 12;
                replacement = ClockGetMetricText(5, settings);
            } else if (rest.starts_with(L"%cpu_temp%")) {
                tokenLength = 10;
                replacement = ClockGetMetricText(4, settings);
            } else if (rest.starts_with(L"%cpu%")) {
                tokenLength = 5;
                replacement = ClockGetMetricText(0, settings);
            } else if (rest.starts_with(L"%ram_used%")) {
                tokenLength = 10;
                replacement = ClockGetMetricText(2, settings);
            } else if (rest.starts_with(L"%ram_total%")) {
                tokenLength = 11;
                replacement = ClockGetMetricText(3, settings);
            } else if (rest.starts_with(L"%ram%")) {
                tokenLength = 5;
                replacement = ClockGetMetricText(1, settings);
            } else if (rest.starts_with(L"%gpu%")) {
                tokenLength = 5;
                replacement = ClockGetMetricText(6, settings);
            } else if (rest.starts_with(L"%battery%")) {
                tokenLength = 9;
                replacement = ClockGetMetricText(7, settings);
            } else if (rest.starts_with(L"%newline%")) {
                tokenLength = 9;
                replacement = L"\n";
            } else if (rest.starts_with(L"%n%")) {
                tokenLength = 3;
                replacement = L"\n";
            }

            if (tokenLength > 0) {
                result += replacement;
                i += tokenLength;
                continue;
            }
        }

        result.push_back(format[i]);
        i++;
    }

    return result;
}

bool ClockIsWin11_22H2OrNewer() {
    struct OsVersionInfo {
        ULONG dwOSVersionInfoSize;
        ULONG dwMajorVersion;
        ULONG dwMinorVersion;
        ULONG dwBuildNumber;
        ULONG dwPlatformId;
        WCHAR szCSDVersion[128];
    };

    OsVersionInfo info{};
    info.dwOSVersionInfoSize = sizeof(info);

    using RtlGetVersion_t = LONG(WINAPI*)(OsVersionInfo*);
    auto rtlGetVersion = (RtlGetVersion_t)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
    if (rtlGetVersion && rtlGetVersion(&info) == 0) {
        return info.dwBuildNumber >= 22621;
    }

    return true;
}

bool ClockLineHasMetrics(std::wstring const& line) {
    static const wchar_t* kMetricPatterns[] = {
        L"%cpu%",      L"%ram%",     L"%ram_used%", L"%ram_total%",
        L"%cpu_temp%", L"%cpu_temp_f%", L"%gpu%",   L"%battery%",
        L"%upload_speed%", L"%download_speed%", L"%total_speed%",
        L"%disk_read%", L"%disk_write%", L"%disk_total%",
        L"%vram%", L"%vram_used%", L"%vram_total%",
        L"%vram_shared%", L"%vram_shared_used%",
        L"%vram_shared_total%", L"%media_title%", L"%media_artist%",
        L"%media_album%", L"%media_status%", L"%media_info%",
        L"%weather%", L"%news%", L"%web",
    };

    for (PCWSTR pattern : kMetricPatterns) {
        if (line.find(pattern) != std::wstring::npos) {
            return true;
        }
    }

    return false;
}

bool ClockNeedsFastRefresh(ClockSettings const& settings) {
    if (!settings.enabled) {
        return false;
    }
    if (settings.showSeconds) {
        return true;
    }
    return (settings.topLine != L"-" &&
            ClockLineHasMetrics(settings.topLine)) ||
           (settings.bottomLine != L"-" &&
            ClockLineHasMetrics(settings.bottomLine)) ||
           settings.tooltipLine.find(L"%media_") != std::wstring::npos ||
           settings.tooltipLine.find(L"%web") != std::wstring::npos ||
           settings.tooltipLine.find(L"%weather%") != std::wstring::npos ||
           settings.tooltipLine.find(L"%news%") != std::wstring::npos;
}

void ClockApplyTextBlockStyle(Controls::TextBlock const& textBlock,
                              ClockTextStyleSettings const& style) {
    if (!textBlock) {
        return;
    }

    try {
        if (!style.textColor.empty()) {
            auto color =
                Markup::XamlBindingHelper::ConvertValue(
                    winrt::xaml_typename<winrt::Windows::UI::Color>(),
                    winrt::box_value(winrt::hstring(style.textColor)))
                    .as<winrt::Windows::UI::Color>();
            textBlock.Foreground(SolidColorBrush(color));
        } else {
            textBlock.ClearValue(Controls::TextBlock::ForegroundProperty());
        }

        if (!style.fontFamily.empty()) {
            auto family =
                Markup::XamlBindingHelper::ConvertValue(
                    winrt::xaml_typename<
                        winrt::Windows::UI::Xaml::Media::FontFamily>(),
                    winrt::box_value(winrt::hstring(style.fontFamily)))
                    .as<winrt::Windows::UI::Xaml::Media::FontFamily>();
            textBlock.FontFamily(family);
        } else {
            textBlock.ClearValue(Controls::TextBlock::FontFamilyProperty());
        }

        if (style.fontSize > 0) {
            textBlock.FontSize(style.fontSize);
        } else {
            textBlock.ClearValue(Controls::TextBlock::FontSizeProperty());
        }

        if (!style.fontWeight.empty()) {
            auto weight =
                Markup::XamlBindingHelper::ConvertValue(
                    winrt::xaml_typename<
                        winrt::Windows::UI::Text::FontWeight>(),
                    winrt::box_value(winrt::hstring(style.fontWeight)))
                    .as<winrt::Windows::UI::Text::FontWeight>();
            textBlock.FontWeight(weight);
        } else {
            textBlock.ClearValue(Controls::TextBlock::FontWeightProperty());
        }

        if (!style.fontStyle.empty()) {
            auto fontStyle =
                Markup::XamlBindingHelper::ConvertValue(
                    winrt::xaml_typename<
                        winrt::Windows::UI::Text::FontStyle>(),
                    winrt::box_value(winrt::hstring(style.fontStyle)))
                    .as<winrt::Windows::UI::Text::FontStyle>();
            textBlock.FontStyle(fontStyle);
        } else {
            textBlock.ClearValue(Controls::TextBlock::FontStyleProperty());
        }

        if (!style.fontStretch.empty()) {
            auto stretch =
                Markup::XamlBindingHelper::ConvertValue(
                    winrt::xaml_typename<
                        winrt::Windows::UI::Text::FontStretch>(),
                    winrt::box_value(winrt::hstring(style.fontStretch)))
                    .as<winrt::Windows::UI::Text::FontStretch>();
            textBlock.FontStretch(stretch);
        } else {
            textBlock.ClearValue(Controls::TextBlock::FontStretchProperty());
        }

        if (!style.textAlignment.empty()) {
            auto alignment =
                Markup::XamlBindingHelper::ConvertValue(
                    winrt::xaml_typename<
                        winrt::Windows::UI::Xaml::TextAlignment>(),
                    winrt::box_value(winrt::hstring(style.textAlignment)))
                    .as<winrt::Windows::UI::Xaml::TextAlignment>();
            textBlock.TextAlignment(alignment);
        } else {
            textBlock.ClearValue(Controls::TextBlock::TextAlignmentProperty());
        }

        if (style.characterSpacing != 0) {
            textBlock.CharacterSpacing(style.characterSpacing);
        } else {
            textBlock.ClearValue(
                Controls::TextBlock::CharacterSpacingProperty());
        }

        if (style.lineHeight > 0) {
            textBlock.LineHeight(style.lineHeight);
            textBlock.LineStackingStrategy(
                LineStackingStrategy::BlockLineHeight);
        } else {
            textBlock.ClearValue(Controls::TextBlock::LineHeightProperty());
            textBlock.ClearValue(
                Controls::TextBlock::LineStackingStrategyProperty());
        }
    } catch (...) {
    }
}

void ClockApplyTextStyles(FrameworkElement const& trayElement) {
    if (!trayElement) {
        return;
    }

    ClockSettings settings = GetClockSettings();
    auto timeTextBlock = FindChildByPredicate(
        trayElement, [](FrameworkElement const& element) {
            return ElementNameEquals(element, L"TimeInnerTextBlock");
        }).try_as<Controls::TextBlock>();
    auto dateTextBlock = FindChildByPredicate(
        trayElement, [](FrameworkElement const& element) {
            return ElementNameEquals(element, L"DateInnerTextBlock");
        }).try_as<Controls::TextBlock>();
    ClockTextStyleSettings emptyStyle;
    ClockApplyTextBlockStyle(
        timeTextBlock, settings.enabled ? settings.timeStyle : emptyStyle);
    ClockApplyTextBlockStyle(
        dateTextBlock, settings.enabled ? settings.dateStyle : emptyStyle);
}

void ClockClearTextStyles(FrameworkElement const& trayElement) {
    if (!trayElement) {
        return;
    }
    ClockTextStyleSettings emptyStyle;
    auto timeTextBlock = FindChildByPredicate(
        trayElement, [](FrameworkElement const& element) {
            return ElementNameEquals(element, L"TimeInnerTextBlock");
        }).try_as<Controls::TextBlock>();
    auto dateTextBlock = FindChildByPredicate(
        trayElement, [](FrameworkElement const& element) {
            return ElementNameEquals(element, L"DateInnerTextBlock");
        }).try_as<Controls::TextBlock>();
    ClockApplyTextBlockStyle(timeTextBlock, emptyStyle);
    ClockApplyTextBlockStyle(dateTextBlock, emptyStyle);
}

// The clock doesn't re-format its text on its own when the settings change.
// Touching a value in the "AdditionalClocks" registry key triggers the clock's
// settings watcher, which refreshes it immediately.
void ClockForceRefresh() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    HKEY hSubKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Control Panel\\TimeDate\\AdditionalClocks", 0,
                      KEY_WRITE, &hSubKey) != ERROR_SUCCESS) {
        return;
    }

    constexpr WCHAR kTempValueName[] =
        L"_temp_windhawk_taskbar-modding-clock";
    if (RegSetValueExW(hSubKey, kTempValueName, 0, REG_SZ,
                       (const BYTE*)L"", sizeof(WCHAR)) == ERROR_SUCCESS) {
        RegDeleteValueW(hSubKey, kTempValueName);
    }

    RegCloseKey(hSubKey);
}

int WINAPI ClockGetTimeFormatEx_Hook(LPCWSTR lpLocaleName,
                                     DWORD dwFlags,
                                     CONST SYSTEMTIME* lpTime,
                                     LPCWSTR lpFormat,
                                     LPWSTR lpTimeStr,
                                     int cchTime) {
    if (g_clockInRefreshIcon && !g_clockInGetToolTipString) {
        ClockSettings settings = GetClockSettings();
        if (settings.enabled && settings.topLine != L"-") {
            if (!cchTime) {
                return 256;
            }

            std::wstring line = ClockFormatLine(settings, settings.topLine);
            wcsncpy_s(lpTimeStr, cchTime, line.c_str(), _TRUNCATE);
            return (int)line.size() + 1;
        }
    }

    return ClockGetTimeFormatEx_Original(lpLocaleName, dwFlags, lpTime,
                                         lpFormat, lpTimeStr, cchTime);
}

int WINAPI ClockGetDateFormatEx_Hook(LPCWSTR lpLocaleName,
                                     DWORD dwFlags,
                                     CONST SYSTEMTIME* lpDate,
                                     LPCWSTR lpFormat,
                                     LPWSTR lpDateStr,
                                     int cchDate,
                                     LPCWSTR lpCalendar) {
    if (g_clockInRefreshIcon && !g_clockInGetToolTipString &&
        !(dwFlags & DATE_LONGDATE)) {
        ClockSettings settings = GetClockSettings();
        if (settings.enabled && settings.bottomLine != L"-") {
            if (!cchDate) {
                return 256;
            }

            std::wstring line = ClockFormatLine(settings, settings.bottomLine);
            wcsncpy_s(lpDateStr, cchDate, line.c_str(), _TRUNCATE);
            return (int)line.size() + 1;
        }
    }

    return ClockGetDateFormatEx_Original(lpLocaleName, dwFlags, lpDate,
                                         lpFormat, lpDateStr, cchDate,
                                         lpCalendar);
}

void WINAPI ClockGetLocalTime_Hook(LPSYSTEMTIME lpSystemTime) {
    ClockGetLocalTime_Original(lpSystemTime);

    if (g_clockInRefreshIcon && !g_clockInGetToolTipString &&
        g_clockNeedAdjustTimer && lpSystemTime) {
        g_clockNeedAdjustTimer = false;

        // Make the clock schedule the next refresh in one second.
        lpSystemTime->wSecond = 59;
    }
}

using ClockRefreshIcon_t = void(WINAPI*)(void* pThis, void* update);
ClockRefreshIcon_t ClockRefreshIcon_Original = nullptr;
ClockRefreshIcon_t ClockRefreshIcon2_Original = nullptr;

void ClockRefreshIconImpl(void* pThis,
                          void* update,
                          ClockRefreshIcon_t original) {
    ClockSettings settings = GetClockSettings();
    g_clockNeedAdjustTimer = ClockNeedsFastRefresh(settings);

    g_clockInRefreshIcon = true;
    original(pThis, update);
    g_clockInRefreshIcon = false;

    g_clockNeedAdjustTimer = false;
}

void WINAPI ClockRefreshIcon_Hook(void* pThis, void* update) {
    HookScope hookScope;
    ClockRefreshIconImpl(pThis, update, ClockRefreshIcon_Original);
}

void WINAPI ClockRefreshIcon2_Hook(void* pThis, void* update) {
    HookScope hookScope;
    ClockRefreshIconImpl(pThis, update, ClockRefreshIcon2_Original);
}

using ClockGetToolTipString_t =
    void*(WINAPI*)(void* pThis, void* a1, void* a2, void* a3, void* a4);
ClockGetToolTipString_t ClockGetToolTipString_Original = nullptr;
ClockGetToolTipString_t ClockGetToolTipString2_Original = nullptr;
ClockGetToolTipString_t ClockGetToolTipString_2_Original = nullptr;
ClockGetToolTipString_t ClockGetToolTipString2_2_Original = nullptr;

void ClockUpdateToolTipString(void* tooltipPtrPtr) {
    ClockSettings settings = GetClockSettings();
    if (!settings.enabled || settings.tooltipLine.empty()) {
        return;
    }

    std::wstring extraLine =
        ClockFormatLine(settings, settings.tooltipLine);
    if (extraLine.empty()) {
        return;
    }

    struct HStringHeader {
        uint32_t flags;
        uint32_t length;
        uint32_t padding1;
        uint32_t padding2;
        wchar_t const* ptr;
    };

    struct SharedHStringHeader : HStringHeader {
        int32_t count;
        wchar_t buffer[1];
    };

    auto* tooltipHeader = *(SharedHStringHeader**)tooltipPtrPtr;
    if (!tooltipHeader) {
        return;
    }

    constexpr wchar_t kSeparator[] = L"\r\n\r\n";
    constexpr size_t kSeparatorLength = 4;

    uint64_t bytesRequired = sizeof(SharedHStringHeader);
    if (settings.tooltipLineMode == ClockTooltipLineMode::replace) {
        bytesRequired += sizeof(wchar_t) * extraLine.size();
    } else {
        bytesRequired += sizeof(wchar_t) * (tooltipHeader->length +
                                            kSeparatorLength +
                                            extraLine.size());
    }

    auto* newHeader = (SharedHStringHeader*)HeapReAlloc(
        GetProcessHeap(), 0, tooltipHeader, bytesRequired);
    if (!newHeader) {
        return;
    }

    *(SharedHStringHeader**)tooltipPtrPtr = newHeader;
    newHeader->ptr = newHeader->buffer;

    if (settings.tooltipLineMode == ClockTooltipLineMode::replace) {
        memcpy(newHeader->buffer, extraLine.c_str(),
               sizeof(wchar_t) * extraLine.size());
        newHeader->length = (uint32_t)extraLine.size();
    } else {
        memcpy(newHeader->buffer + newHeader->length, kSeparator,
               sizeof(wchar_t) * kSeparatorLength);
        newHeader->length += (uint32_t)kSeparatorLength;

        memcpy(newHeader->buffer + newHeader->length, extraLine.c_str(),
               sizeof(wchar_t) * extraLine.size());
        newHeader->length += (uint32_t)extraLine.size();
    }

    newHeader->buffer[newHeader->length] = L'\0';
}

void* ClockGetToolTipStringImpl(void* pThis,
                                void* a1,
                                void* a2,
                                void* a3,
                                void* a4,
                                ClockGetToolTipString_t original) {
    g_clockInGetToolTipString = true;

    void* ret = original(pThis, a1, a2, a3, a4);

    if (ret) {
        ClockUpdateToolTipString(ret);
    }

    g_clockInGetToolTipString = false;

    return ret;
}

void* WINAPI ClockGetToolTipString_Hook(void* pThis,
                                        void* a1,
                                        void* a2,
                                        void* a3,
                                        void* a4) {
    HookScope hookScope;
    return ClockGetToolTipStringImpl(pThis, a1, a2, a3, a4,
                                     ClockGetToolTipString_Original);
}

void* WINAPI ClockGetToolTipString2_Hook(void* pThis,
                                         void* a1,
                                         void* a2,
                                         void* a3,
                                         void* a4) {
    HookScope hookScope;
    return ClockGetToolTipStringImpl(pThis, a1, a2, a3, a4,
                                     ClockGetToolTipString2_Original);
}

void* WINAPI ClockGetToolTipString_2_Hook(void* pThis,
                                          void* a1,
                                          void* a2,
                                          void* a3,
                                          void* a4) {
    HookScope hookScope;
    return ClockGetToolTipStringImpl(pThis, a1, a2, a3, a4,
                                     ClockGetToolTipString_2_Original);
}

void* WINAPI ClockGetToolTipString2_2_Hook(void* pThis,
                                           void* a1,
                                           void* a2,
                                           void* a3,
                                           void* a4) {
    HookScope hookScope;
    return ClockGetToolTipStringImpl(pThis, a1, a2, a3, a4,
                                     ClockGetToolTipString2_2_Original);
}

using ClockCreateTimer_t = void*(WINAPI*)(void* a1,
                                          void* a2,
                                          void* a3,
                                          void* a4);
ClockCreateTimer_t ClockCreateTimer_Original = nullptr;
void* WINAPI ClockCreateTimer_Hook(void* a1,
                                   void* a2,
                                   void* a3,
                                   void* a4) {
    HookScope hookScope;

    // In newer Windows 11 versions there are only 3 arguments, but the fourth
    // one is ignored, so reading it is harmless.
    ULONGLONG** elapse =
        (ULONGLONG**)(ClockIsWin11_22H2OrNewer() ? &a3 : &a4);

    if (g_clockInRefreshIcon && !g_clockInGetToolTipString && elapse &&
        *elapse && **elapse == 10000000ULL) {
        // Align the next refresh to the next second to avoid skipping seconds.
        SYSTEMTIME time{};
        if (ClockGetLocalTime_Original) {
            ClockGetLocalTime_Original(&time);
        } else {
            GetLocalTime(&time);
        }
        ULONGLONG elapseNew = 10000ULL * (1000 - time.wMilliseconds);
        *elapse = &elapseNew;
    }

    return ClockCreateTimer_Original(a1, a2, a3, a4);
}

using ClockCreateTimerLambda_t =
    void*(WINAPI*)(DWORD_PTR** a1, void* a2, void* a3);
ClockCreateTimerLambda_t ClockCreateTimerLambda_Original = nullptr;
void* WINAPI ClockCreateTimerLambda_Hook(DWORD_PTR** a1,
                                         void* a2,
                                         void* a3) {
    HookScope hookScope;

    DWORD_PTR* elapse = a1 ? a1[1] : nullptr;

    if (g_clockInRefreshIcon && !g_clockInGetToolTipString && elapse &&
        *elapse == 10000000ULL) {
        SYSTEMTIME time{};
        if (ClockGetLocalTime_Original) {
            ClockGetLocalTime_Original(&time);
        } else {
            GetLocalTime(&time);
        }
        *elapse = 10000ULL * (1000 - time.wMilliseconds);
    }

    return ClockCreateTimerLambda_Original(a1, a2, a3);
}

using ClockICalendarSecond_t = int(WINAPI*)(void* pThis);
ClockICalendarSecond_t ClockICalendarSecond_Original = nullptr;
int WINAPI ClockICalendarSecond_Hook(void* pThis) {
    HookScope hookScope;

    if (g_clockInRefreshIcon && !g_clockInGetToolTipString &&
        g_clockNeedAdjustTimer) {
        g_clockNeedAdjustTimer = false;

        // Make the next refresh happen in a second.
        return 59;
    }

    return ClockICalendarSecond_Original(pThis);
}

using ClockDateTimeIconContent_OnApplyTemplate_t =
    HRESULT(WINAPI*)(LPVOID pThis);
ClockDateTimeIconContent_OnApplyTemplate_t
    ClockDateTimeIconContent_OnApplyTemplate_Original = nullptr;

HRESULT WINAPI ClockDateTimeIconContent_OnApplyTemplate_Hook(LPVOID pThis) {
    HookScope hookScope;
    HRESULT result =
        ClockDateTimeIconContent_OnApplyTemplate_Original(pThis);
    if (SUCCEEDED(result) && pThis) {
        FrameworkElement element{nullptr};
        ((IUnknown*)pThis)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(element));
        if (element) {
            ClockApplyTextStyles(element);
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// Dock hover animation, adapted from the "Taskbar Dock Animation Plus" mod
// (taskbar-dock-animation-plus).
//
// Pointer moves over the taskbar frame drive a CompositionTarget::Rendering
// loop which scales the icons around the cursor and shifts the neighbors, and
// which also runs an idle "breathing" bounce for the closest icon.
// ---------------------------------------------------------------------------

constexpr double kDockMouseStopThreshold = 0.5;
constexpr double kDockBouncePeriodMs = 1200.0;
constexpr double kDockBounceScaleAmount = 0.05;
constexpr double kDockBounceTranslateY = -4.0;

winrt::event_token g_dockRenderingToken;
std::atomic<bool> g_dockIsRenderingHooked{false};
std::atomic<void*> g_dockActiveContextKey{nullptr};
std::chrono::steady_clock::time_point g_dockLastSignificantMoveTime;
std::atomic<double> g_dockLastMouseX{-1.0};
std::atomic<bool> g_dockIsBouncing{false};
std::atomic<double> g_dockBounceIntensity{0.0};
std::chrono::steady_clock::time_point g_dockBounceStartTime;
std::atomic<bool> g_dockIsMouseInside{false};
std::atomic<double> g_dockAnimationIntensity{0.0};
std::chrono::steady_clock::time_point g_dockLastRenderTime;

enum class DockTaskbarEdge {
    Bottom,
    Top,
    Left,
    Right,
};

struct DockIconAnimState {
    double waveScale = 1.0;
    double waveTranslateX = 0.0;
};

struct DockTaskbarIconInfo {
    winrt::weak_ref<FrameworkElement> element;
    double originalCenterX = 0.0;
    double elementWidth = 0.0;
    DockIconAnimState state;
};

struct DockHostSignature {
    int count = -1;
    double width = -1.0;
    uint64_t orderHash = 0;
};

struct DockAnimationContext {
    bool isInitialized = false;
    winrt::weak_ref<FrameworkElement> taskbarFrame;
    winrt::weak_ref<FrameworkElement> iconHost;
    std::vector<DockTaskbarIconInfo> icons;
    DockHostSignature lastSig;
    std::chrono::steady_clock::time_point lastDirtyCheck{};
    DockTaskbarEdge edge = DockTaskbarEdge::Bottom;
    bool isVertical = false;
    HWND hWnd = nullptr;
    RECT lastTrayRect = {0, 0, 0, 0};
};

// One context per taskbar frame (keyed by the TaskbarFrame instance pointer).
std::mutex g_dockContextsMutex;
std::map<void*, DockAnimationContext> g_dockContexts;

DockAnimationContext* FindDockContext(void* key) {
    std::lock_guard<std::mutex> lock(g_dockContextsMutex);
    auto it = g_dockContexts.find(key);
    return it == g_dockContexts.end() ? nullptr : &it->second;
}

void DockOnRendering(winrt::Windows::Foundation::IInspectable const&,
                     winrt::Windows::Foundation::IInspectable const&);

void DockUpdateTaskbarEdge(DockAnimationContext& ctx) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) {
        return;
    }

    HWND hWnd = ctx.hWnd;
    if (!hWnd || !IsWindow(hWnd)) {
        hWnd = GetTaskbarWindowForCurrentThread();
        ctx.hWnd = hWnd;
    }

    if (!hWnd) {
        if (frame.ActualWidth() < frame.ActualHeight()) {
            ctx.edge = DockTaskbarEdge::Left;
        } else {
            ctx.edge = DockTaskbarEdge::Bottom;
        }
        ctx.isVertical = (ctx.edge == DockTaskbarEdge::Left ||
                          ctx.edge == DockTaskbarEdge::Right);
        return;
    }

    RECT rect = {0};
    if (!GetWindowRect(hWnd, &rect)) {
        return;
    }

    if (rect.left == ctx.lastTrayRect.left &&
        rect.top == ctx.lastTrayRect.top &&
        rect.right == ctx.lastTrayRect.right &&
        rect.bottom == ctx.lastTrayRect.bottom) {
        return;
    }

    ctx.lastTrayRect = rect;

    LONG w = rect.right - rect.left;
    LONG h = rect.bottom - rect.top;
    if (w < h) {
        HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = {sizeof(mi)};
        if (GetMonitorInfo(hMonitor, &mi)) {
            if (rect.left <=
                mi.rcMonitor.left +
                    (mi.rcMonitor.right - mi.rcMonitor.left) / 2) {
                ctx.edge = DockTaskbarEdge::Left;
            } else {
                ctx.edge = DockTaskbarEdge::Right;
            }
        } else {
            ctx.edge = DockTaskbarEdge::Left;
        }
    } else {
        HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = {sizeof(mi)};
        if (GetMonitorInfo(hMonitor, &mi)) {
            if (rect.top <=
                mi.rcMonitor.top +
                    (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2) {
                ctx.edge = DockTaskbarEdge::Top;
            } else {
                ctx.edge = DockTaskbarEdge::Bottom;
            }
        } else {
            ctx.edge = DockTaskbarEdge::Bottom;
        }
    }
    ctx.isVertical = (ctx.edge == DockTaskbarEdge::Left ||
                      ctx.edge == DockTaskbarEdge::Right);
}

double DockCalculateScale(double distance,
                          double radius,
                          double maxScale,
                          int animationType) {
    if (distance > radius) {
        return 1.0;
    }

    double t = distance / radius;
    double factor = 0.0;

    switch (animationType) {
        case 1:  // Linear
            factor = 1.0 - t;
            break;
        case 2:  // Exponential
            factor = pow(1.0 - t, 3);
            break;
        case 0:  // Cosine
        default:
            factor = (cos(t * 3.14159) + 1.0) / 2.0;
            break;
    }
    if (factor < 0) {
        factor = 0;
    }
    return (maxScale - 1.0) * factor + 1.0;
}

void DockApplyAnimation(double mouseX,
                        DockAnimationContext& ctx,
                        double intensity,
                        double dtSec,
                        DockSettings const& settings) {
    const double spacingFactor = settings.spacingFactor;
    std::vector<double> scales(ctx.icons.size());
    std::vector<double> extraWidths(ctx.icons.size());
    double totalExpansion = 0.0;
    size_t closestIconIndex = (size_t)-1;
    double minDistance = settings.effectRadius + 1.0;
    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) {
        return;
    }

    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) {
            continue;
        }
        double distance = 0.0;
        if (settings.taskbarLabelsMode) {
            double iconStart = ctx.icons[i].originalCenterX;
            double iconEnd = iconStart + ctx.icons[i].elementWidth;
            if (mouseX < iconStart) {
                distance = iconStart - mouseX;
            } else if (mouseX > iconEnd) {
                distance = mouseX - iconEnd;
            } else {
                distance = 0.0;
            }
        } else {
            distance = std::abs(mouseX - ctx.icons[i].originalCenterX);
        }
        scales[i] = DockCalculateScale(
            distance, (double)settings.effectRadius, settings.maxScale,
            settings.animationType);
        double size =
            ctx.isVertical ? element.ActualHeight() : element.ActualWidth();
        extraWidths[i] = (scales[i] - 1.0) * size * spacingFactor;
        totalExpansion += extraWidths[i];

        if (distance < minDistance) {
            minDistance = distance;
            closestIconIndex = i;
        }
    }

    double bounceScaleFactor = 1.0;
    double bounceTranslateOffset = 0.0;

    double currentBounceIntensity = g_dockBounceIntensity.load();

    if (currentBounceIntensity > 0.0 && closestIconIndex != (size_t)-1 &&
        minDistance < (settings.effectRadius / 2.0)) {
        auto elapsed_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - g_dockBounceStartTime)
                .count();
        double t =
            std::fmod(elapsed_ms, kDockBouncePeriodMs) / kDockBouncePeriodMs;
        double normalizedWave = std::sin(t * 3.14159);
        bounceScaleFactor = 1.0 + (normalizedWave * kDockBounceScaleAmount *
                                   currentBounceIntensity);
        bounceTranslateOffset =
            normalizedWave * kDockBounceTranslateY * currentBounceIntensity;

        if (ctx.isVertical) {
            if (ctx.edge == DockTaskbarEdge::Left) {
                bounceTranslateOffset *= -1.0;
            }
        } else {
            if (ctx.edge == DockTaskbarEdge::Top) {
                bounceTranslateOffset *= -1.0;
            }
        }
    }

    double alpha = 1.0;
    if (settings.lerpSpeed > 0.0) {
        alpha = 1.0 - std::exp(-settings.lerpSpeed * dtSec);
    }
    alpha = std::clamp(alpha, 0.0, 1.0);

    double cumulativeShift = 0.0;

    for (size_t i = 0; i < ctx.icons.size(); i++) {
        auto element = ctx.icons[i].element.get();
        if (!element) {
            continue;
        }
        auto tg = element.RenderTransform().try_as<TransformGroup>();
        if (!tg || tg.Children().Size() < 4) {
            continue;
        }
        auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
        auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
        auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
        auto bounceTranslate =
            tg.Children().GetAt(3).try_as<TranslateTransform>();

        if (!waveScale || !waveTranslate || !bounceScale || !bounceTranslate) {
            continue;
        }
        double selfShift = extraWidths[i] / 2.0;
        double centerOffset = totalExpansion / 2.0;
        double finalShift = cumulativeShift + selfShift - centerOffset;
        cumulativeShift += extraWidths[i];
        double targetScale = 1.0 + (scales[i] - 1.0) * intensity;
        double targetTranslate = finalShift * intensity;

        if (settings.lerpSpeed > 0.0) {
            ctx.icons[i].state.waveScale +=
                (targetScale - ctx.icons[i].state.waveScale) * alpha;
            ctx.icons[i].state.waveTranslateX +=
                (targetTranslate - ctx.icons[i].state.waveTranslateX) * alpha;
        } else {
            ctx.icons[i].state.waveScale = targetScale;
            ctx.icons[i].state.waveTranslateX = targetTranslate;
        }

        waveScale.ScaleX(ctx.icons[i].state.waveScale);
        waveScale.ScaleY(ctx.icons[i].state.waveScale);

        if (ctx.isVertical) {
            waveTranslate.X(0.0);
            waveTranslate.Y(ctx.icons[i].state.waveTranslateX);
        } else {
            waveTranslate.X(ctx.icons[i].state.waveTranslateX);
            waveTranslate.Y(0.0);
        }

        if (i == closestIconIndex && currentBounceIntensity > 0.0) {
            bounceScale.ScaleX(bounceScaleFactor);
            bounceScale.ScaleY(bounceScaleFactor);

            if (!settings.disableBounce && !settings.disableVerticalBounce) {
                if (ctx.isVertical) {
                    bounceTranslate.X(bounceTranslateOffset);
                    bounceTranslate.Y(0.0);
                } else {
                    bounceTranslate.X(0.0);
                    bounceTranslate.Y(bounceTranslateOffset);
                }
            } else {
                bounceTranslate.X(0.0);
                bounceTranslate.Y(0.0);
            }
        } else {
            bounceScale.ScaleX(1.0);
            bounceScale.ScaleY(1.0);
            bounceTranslate.X(0.0);
            bounceTranslate.Y(0.0);
        }
    }
}

void DockResetAllIconScales(std::vector<DockTaskbarIconInfo>& icons) {
    if (icons.empty()) {
        return;
    }

    try {
        for (auto& iconInfo : icons) {
            auto element = iconInfo.element.get();
            if (!element) {
                continue;
            }
            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) {
                continue;
            }
            auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
            auto waveTranslate =
                tg.Children().GetAt(1).try_as<TranslateTransform>();
            auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
            auto bounceTranslate =
                tg.Children().GetAt(3).try_as<TranslateTransform>();

            if (waveScale) {
                waveScale.ScaleX(1.0);
                waveScale.ScaleY(1.0);
            }
            if (waveTranslate) {
                waveTranslate.X(0.0);
                waveTranslate.Y(0.0);
            }
            if (bounceScale) {
                bounceScale.ScaleX(1.0);
                bounceScale.ScaleY(1.0);
            }
            if (bounceTranslate) {
                bounceTranslate.X(0.0);
                bounceTranslate.Y(0.0);
            }
        }
    } catch (...) {
    }
}

void DockOnPointerMoved(void* pThisKey,
                        Input::PointerRoutedEventArgs const& args) {
    try {
        auto* ctx = FindDockContext(pThisKey);
        if (!ctx) {
            return;
        }
        auto frame = ctx->taskbarFrame.get();
        if (!frame) {
            return;
        }

        g_dockIsMouseInside = true;
        g_dockActiveContextKey = pThisKey;

        DockUpdateTaskbarEdge(*ctx);

        double mousePos = ctx->isVertical
                              ? args.GetCurrentPoint(frame).Position().Y
                              : args.GetCurrentPoint(frame).Position().X;

        if (std::abs(mousePos - g_dockLastMouseX.load()) >
            kDockMouseStopThreshold) {
            g_dockLastSignificantMoveTime = std::chrono::steady_clock::now();
        }
        g_dockLastMouseX = mousePos;

        if (!g_dockIsRenderingHooked.exchange(true)) {
            g_dockLastRenderTime = std::chrono::steady_clock::now();
            g_dockRenderingToken =
                Media::CompositionTarget::Rendering(DockOnRendering);
            g_dockLastSignificantMoveTime = std::chrono::steady_clock::now();
        }
    } catch (...) {
    }
}

void DockOnPointerExited(void* pThisKey) {
    if (g_dockActiveContextKey.load() == pThisKey) {
        g_dockIsMouseInside = false;
        g_dockIsBouncing = false;
    }
}

FrameworkElement DockFindChildByClassNamePartial(FrameworkElement element,
                                                  PCWSTR partialName) {
    if (!element) {
        return nullptr;
    }
    return EnumChildElements(element, [partialName](FrameworkElement child) {
        auto className = winrt::get_class_name(child);
        return std::wstring_view(className).find(partialName) !=
               std::wstring_view::npos;
    });
}

FrameworkElement DockFindIconHost(FrameworkElement const& taskbarFrame) {
    FrameworkElement host = DockFindChildByClassNamePartial(
        taskbarFrame, L"TaskbarFrameRepeater");
    if (!host) {
        host = FindChildByClassName(taskbarFrame, L"TaskbarFrameRepeater");
    }
    if (!host) {
        host = FindChildByClassName(taskbarFrame, L"TaskbarItemHost");
    }
    return host;
}

uint64_t DockFnv1aMix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

DockHostSignature DockComputeHostSignature(FrameworkElement const& host,
                                           bool isVertical) {
    DockHostSignature sig;
    sig.count = VisualTreeHelper::GetChildrenCount(host);
    sig.width = isVertical ? host.ActualHeight() : host.ActualWidth();

    uint64_t h = 1469598103934665603ULL;
    for (int i = 0; i < sig.count; i++) {
        auto child = VisualTreeHelper::GetChild(host, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        h = DockFnv1aMix(h, (uint64_t)winrt::get_abi(child));
        double size = isVertical ? child.ActualHeight() : child.ActualWidth();
        h = DockFnv1aMix(h, (uint64_t)(size * 10.0));
    }
    sig.orderHash = h;
    return sig;
}

bool DockSigDifferent(DockHostSignature const& a, DockHostSignature const& b) {
    if (a.count != b.count) {
        return true;
    }
    if (std::abs(a.width - b.width) > 0.5) {
        return true;
    }
    if (a.orderHash != b.orderHash) {
        return true;
    }
    return false;
}

enum class DockButtonKind {
    App,
    Start,
    Search,
    TaskView,
    Widgets,
    Weather,
    SystemOther,
};

std::wstring DockHStringToWstring(winrt::hstring const& s) {
    return std::wstring(s.c_str());
}

std::wstring DockToLower(std::wstring s) {
    for (auto& ch : s) {
        ch = (wchar_t)towlower(ch);
    }
    return s;
}

DockButtonKind DockClassifyButton(FrameworkElement const& e) {
    if (!e) {
        return DockButtonKind::SystemOther;
    }

    std::wstring cn = DockHStringToWstring(winrt::get_class_name(e));
    std::wstring feName = DockHStringToWstring(e.Name());
    std::wstring aid = DockHStringToWstring(
        Automation::AutomationProperties::GetAutomationId(e));

    std::wstring cnL = DockToLower(cn);
    std::wstring aidL = DockToLower(aid);
    std::wstring feL = DockToLower(feName);

    auto matches = [&](const wchar_t* s) {
        return cnL.find(s) != std::wstring::npos ||
               aidL.find(s) != std::wstring::npos ||
               feL.find(s) != std::wstring::npos;
    };

    if (matches(L"startbutton")) {
        return DockButtonKind::Start;
    }
    if (matches(L"searchbutton")) {
        return DockButtonKind::Search;
    }
    if (matches(L"taskviewbutton")) {
        return DockButtonKind::TaskView;
    }
    if (matches(L"widgetsbutton")) {
        return DockButtonKind::Widgets;
    }
    if (matches(L"weather")) {
        return DockButtonKind::Weather;
    }

    if (matches(L"appid:")) {
        return DockButtonKind::App;
    }

    if (cnL == L"taskbar.tasklistbutton" ||
        (cnL.size() >= 13 &&
         cnL.rfind(L".tasklistbutton") == cnL.size() - 13)) {
        return DockButtonKind::App;
    }

    return DockButtonKind::SystemOther;
}

bool DockShouldAnimateElement(FrameworkElement const& e,
                              DockSettings const& settings) {
    const int mode = settings.excludeSystemButtonsMode;
    if (mode == 0) {
        return true;
    }

    DockButtonKind k = DockClassifyButton(e);

    if (mode == 2) {
        return k == DockButtonKind::App;
    }
    if (mode == 1) {
        return k != DockButtonKind::Start;
    }

    return true;
}

void DockResetElementTransforms(FrameworkElement const& element) {
    auto tg = element.RenderTransform().try_as<TransformGroup>();
    if (!tg || tg.Children().Size() < 4) {
        return;
    }

    auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
    auto waveTranslate = tg.Children().GetAt(1).try_as<TranslateTransform>();
    auto bounceScale = tg.Children().GetAt(2).try_as<ScaleTransform>();
    auto bounceTranslate = tg.Children().GetAt(3).try_as<TranslateTransform>();

    if (waveScale) {
        waveScale.ScaleX(1.0);
        waveScale.ScaleY(1.0);
    }
    if (waveTranslate) {
        waveTranslate.X(0.0);
        waveTranslate.Y(0.0);
    }
    if (bounceScale) {
        bounceScale.ScaleX(1.0);
        bounceScale.ScaleY(1.0);
    }
    if (bounceTranslate) {
        bounceTranslate.X(0.0);
        bounceTranslate.Y(0.0);
    }
}

bool DockRebaseIconGeometryFast(DockAnimationContext& ctx,
                                DockSettings const& settings) {
    auto frame = ctx.taskbarFrame.get();
    if (!frame) {
        return false;
    }

    bool changed = false;

    for (auto it = ctx.icons.begin(); it != ctx.icons.end();) {
        if (!it->element.get()) {
            it = ctx.icons.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }

    for (auto& info : ctx.icons) {
        auto e = info.element.get();
        if (!e) {
            continue;
        }

        try {
            double oldX = info.originalCenterX;
            double oldW = info.elementWidth;

            auto t = e.TransformToVisual(frame);
            auto pt = t.TransformPoint({0, 0});
            double w = ctx.isVertical ? e.ActualHeight() : e.ActualWidth();
            info.elementWidth = w;

            float xOrigin = 0.5f;
            float yOrigin = 0.5f;
            if (ctx.isVertical) {
                xOrigin = (ctx.edge == DockTaskbarEdge::Left) ? 0.0f : 1.0f;
            } else {
                bool isTop = (ctx.edge == DockTaskbarEdge::Top);
                yOrigin = settings.disableVerticalBounce
                              ? 0.5f
                              : (isTop ? 0.0f : 1.0f);
            }

            if (settings.taskbarLabelsMode) {
                double newStartX = ctx.isVertical ? pt.Y : pt.X;
                info.originalCenterX = newStartX;

                float iconCenterPx = 24.0f;
                float iconCenterProportion =
                    (w > 0.0) ? (iconCenterPx / (float)w) : 0.5f;
                if (ctx.isVertical) {
                    e.RenderTransformOrigin({xOrigin, iconCenterProportion});
                } else {
                    e.RenderTransformOrigin({iconCenterProportion, yOrigin});
                }

                if (std::abs(newStartX - oldX) > 0.5) {
                    changed = true;
                }
            } else {
                double newCenterX =
                    ctx.isVertical ? (pt.Y + (w / 2.0)) : (pt.X + (w / 2.0));
                info.originalCenterX = newCenterX;

                if (ctx.isVertical) {
                    e.RenderTransformOrigin({xOrigin, yOrigin});
                } else {
                    e.RenderTransformOrigin({0.5f, yOrigin});
                }

                if (std::abs(newCenterX - oldX) > 0.5) {
                    changed = true;
                }
            }

            if (std::abs(w - oldW) > 0.5) {
                changed = true;
            }
        } catch (...) {
        }
    }

    if (changed && ctx.icons.size() > 1) {
        std::sort(ctx.icons.begin(), ctx.icons.end(),
                  [](DockTaskbarIconInfo const& a,
                     DockTaskbarIconInfo const& b) {
                      return a.originalCenterX < b.originalCenterX;
                  });
    }

    return changed;
}

// Builds or refreshes the icon list and sets up the transforms used by the
// animation (scale + translate).
void DockRefreshIconPositions(DockAnimationContext& ctx,
                              DockSettings const& settings) {
    // Preserve the animation state across rebuilds.
    std::unordered_map<void*, DockIconAnimState> prevStates;
    std::unordered_set<void*> prevElements;

    for (auto& old : ctx.icons) {
        if (auto e = old.element.get()) {
            void* key = winrt::get_abi(e);
            prevStates[key] = old.state;
            prevElements.insert(key);
        }
    }

    std::vector<DockTaskbarIconInfo> newIcons;

    auto taskbarFrame = ctx.taskbarFrame.get();
    if (!taskbarFrame) {
        return;
    }

    auto isTaskListButtonExact = [](FrameworkElement const& e) -> bool {
        if (!e) {
            return false;
        }
        return winrt::get_class_name(e) == L"Taskbar.TaskListButton";
    };

    auto isTaskListButtonPanel = [](FrameworkElement const& e) -> bool {
        if (!e) {
            return false;
        }
        return winrt::get_class_name(e) == L"Taskbar.TaskListButtonPanel";
    };

    std::function<FrameworkElement(FrameworkElement)> findFirstTaskListButton =
        [&](FrameworkElement root) -> FrameworkElement {
        if (!root) {
            return nullptr;
        }
        if (isTaskListButtonExact(root)) {
            return root;
        }

        int c = VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < c; i++) {
            auto ch = VisualTreeHelper::GetChild(root, i)
                          .try_as<FrameworkElement>();
            if (!ch) {
                continue;
            }
            if (isTaskListButtonExact(ch)) {
                return ch;
            }
            auto deep = findFirstTaskListButton(ch);
            if (deep) {
                return deep;
            }
        }
        return nullptr;
    };

    // The element which carries the automation id/name used to classify the
    // button.
    std::function<FrameworkElement(FrameworkElement)> findClassifierNode =
        [&](FrameworkElement root) -> FrameworkElement {
        if (!root) {
            return nullptr;
        }

        auto aid = DockHStringToWstring(
            Automation::AutomationProperties::GetAutomationId(root));
        auto nm = DockHStringToWstring(
            Automation::AutomationProperties::GetName(root));
        if (!aid.empty() || !nm.empty()) {
            return root;
        }

        int c = VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < c; i++) {
            auto ch = VisualTreeHelper::GetChild(root, i)
                          .try_as<FrameworkElement>();
            if (!ch) {
                continue;
            }
            auto hit = findClassifierNode(ch);
            if (hit) {
                return hit;
            }
        }
        return nullptr;
    };

    // Candidate in fallback: TaskListButton + TaskListButtonPanel.
    auto isFallbackCandidate = [&](FrameworkElement const& e) -> bool {
        if (!e) {
            return false;
        }

        if (isTaskListButtonExact(e) || isTaskListButtonPanel(e)) {
            return true;
        }

        auto cn = winrt::get_class_name(e);
        if (std::wstring_view(cn).find(L"Taskbar.") !=
            std::wstring_view::npos) {
            auto aid = DockHStringToWstring(
                Automation::AutomationProperties::GetAutomationId(e));
            auto nm = DockHStringToWstring(
                Automation::AutomationProperties::GetName(e));
            if (!aid.empty() || !nm.empty()) {
                return true;
            }
        }

        return false;
    };

    auto setupAndAddElement = [&](FrameworkElement const& element) {
        try {
            if (!element) {
                return;
            }

            auto tg = element.RenderTransform().try_as<TransformGroup>();
            if (!tg || tg.Children().Size() < 4) {
                tg = TransformGroup();
                tg.Children().Append(ScaleTransform());      // 0 waveScale
                tg.Children().Append(TranslateTransform());  // 1 waveTranslate
                tg.Children().Append(ScaleTransform());      // 2 bounceScale
                tg.Children().Append(TranslateTransform());  // 3 bounceTranslate
                element.RenderTransform(tg);
            }

            float xOrigin = 0.5f;
            float yOrigin = 0.5f;
            if (ctx.isVertical) {
                xOrigin = (ctx.edge == DockTaskbarEdge::Left) ? 0.0f : 1.0f;
            } else {
                bool isTop = (ctx.edge == DockTaskbarEdge::Top);
                yOrigin = settings.disableVerticalBounce
                              ? 0.5f
                              : (isTop ? 0.0f : 1.0f);
            }

            auto transform = element.TransformToVisual(taskbarFrame);
            auto point = transform.TransformPoint({0, 0});

            DockTaskbarIconInfo info;
            info.element = element;
            info.elementWidth = ctx.isVertical ? element.ActualHeight()
                                               : element.ActualWidth();

            if (settings.taskbarLabelsMode) {
                float iconCenterPx = 24.0f;
                float iconCenterProportion =
                    (info.elementWidth > 0)
                        ? (iconCenterPx / (float)info.elementWidth)
                        : 0.5f;

                if (ctx.isVertical) {
                    element.RenderTransformOrigin(
                        {xOrigin, iconCenterProportion});
                } else {
                    element.RenderTransformOrigin(
                        {iconCenterProportion, yOrigin});
                }
                info.originalCenterX = ctx.isVertical ? point.Y : point.X;
            } else {
                if (ctx.isVertical) {
                    element.RenderTransformOrigin({xOrigin, yOrigin});
                } else {
                    element.RenderTransformOrigin({0.5f, yOrigin});
                }
                info.originalCenterX =
                    ctx.isVertical ? (point.Y + (info.elementWidth / 2.0))
                                   : (point.X + (info.elementWidth / 2.0));
            }

            void* key = winrt::get_abi(element);

            auto it = prevStates.find(key);
            if (it != prevStates.end()) {
                info.state = it->second;

                auto waveScale = tg.Children().GetAt(0).try_as<ScaleTransform>();
                auto waveTranslate =
                    tg.Children().GetAt(1).try_as<TranslateTransform>();
                if (waveScale) {
                    waveScale.ScaleX(info.state.waveScale);
                    waveScale.ScaleY(info.state.waveScale);
                }
                if (waveTranslate) {
                    if (ctx.isVertical) {
                        waveTranslate.Y(info.state.waveTranslateX);
                    } else {
                        waveTranslate.X(info.state.waveTranslateX);
                    }
                }
            }

            newIcons.push_back(info);
        } catch (...) {
        }
    };

    // Structure-based host search.
    auto findSmartHost = [&]() -> FrameworkElement {
        std::vector<FrameworkElement> nodes;
        nodes.reserve(512);

        std::vector<FrameworkElement> stack;
        stack.reserve(512);
        stack.push_back(taskbarFrame);

        while (!stack.empty()) {
            auto cur = stack.back();
            stack.pop_back();
            if (!cur) {
                continue;
            }

            nodes.push_back(cur);

            int c = VisualTreeHelper::GetChildrenCount(cur);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(cur, i)
                              .try_as<FrameworkElement>();
                if (ch) {
                    stack.push_back(ch);
                }
            }
        }

        std::function<bool(FrameworkElement, int)> subtreeHasIconLike =
            [&](FrameworkElement root, int depth) -> bool {
            if (!root || depth <= 0) {
                return false;
            }
            if (isTaskListButtonExact(root) || isTaskListButtonPanel(root)) {
                return true;
            }

            int c = VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(root, i)
                              .try_as<FrameworkElement>();
                if (!ch) {
                    continue;
                }
                if (subtreeHasIconLike(ch, depth - 1)) {
                    return true;
                }
            }
            return false;
        };

        FrameworkElement best = nullptr;
        int bestHits = 0;
        double bestWidth = 0.0;

        for (auto const& n : nodes) {
            int c = VisualTreeHelper::GetChildrenCount(n);
            if (c < 3 || c > 120) {
                continue;
            }

            int hits = 0;
            for (int i = 0; i < c; i++) {
                auto ch = VisualTreeHelper::GetChild(n, i)
                              .try_as<FrameworkElement>();
                if (!ch) {
                    continue;
                }
                if (subtreeHasIconLike(ch, 5)) {
                    hits++;
                }
            }

            if (hits >= 3) {
                double w = (double)n.ActualWidth();
                if (hits > bestHits || (hits == bestHits && w > bestWidth)) {
                    best = n;
                    bestHits = hits;
                    bestWidth = w;
                }
            }
        }

        return best;
    };

    // Host selection.
    auto host = ctx.iconHost.get();
    if (!host) {
        host = findSmartHost();
        if (host) {
            ctx.iconHost = host;
        }
    }

    try {
        if (host) {
            // Main path: enumerate the children of the host.
            int count = VisualTreeHelper::GetChildrenCount(host);
            for (int i = 0; i < count; i++) {
                auto child = VisualTreeHelper::GetChild(host, i)
                                 .try_as<FrameworkElement>();
                if (!child) {
                    continue;
                }

                FrameworkElement classifier = findClassifierNode(child);
                FrameworkElement base = classifier ? classifier : child;

                bool animate = (settings.excludeSystemButtonsMode == 0)
                                   ? true
                                   : DockShouldAnimateElement(base, settings);

                // Target: prefer TaskListButton inside, else child.
                FrameworkElement target = findFirstTaskListButton(child);
                if (!target) {
                    target = child;
                }

                if (!animate) {
                    void* kT = winrt::get_abi(target);
                    if (prevElements.contains(kT)) {
                        DockResetElementTransforms(target);
                    }

                    void* kC = winrt::get_abi(child);
                    if (prevElements.contains(kC)) {
                        DockResetElementTransforms(child);
                    }
                    continue;
                }

                setupAndAddElement(target);
            }

            ctx.lastSig = DockComputeHostSignature(host, ctx.isVertical);
        } else {
            // Fallback: recurse and include TaskListButtonPanel too.
            std::function<void(FrameworkElement)> recurse =
                [&](FrameworkElement element) {
                    if (!element) {
                        return;
                    }

                    if (isFallbackCandidate(element)) {
                        FrameworkElement classifier =
                            findClassifierNode(element);
                        FrameworkElement base =
                            classifier ? classifier : element;

                        bool animate =
                            (settings.excludeSystemButtonsMode == 0)
                                ? true
                                : DockShouldAnimateElement(base, settings);

                        FrameworkElement target =
                            findFirstTaskListButton(element);
                        if (!target) {
                            target = element;
                        }

                        if (!animate) {
                            void* k = winrt::get_abi(target);
                            if (prevElements.contains(k)) {
                                DockResetElementTransforms(target);
                            }
                        } else {
                            setupAndAddElement(target);
                        }
                    }

                    int c = VisualTreeHelper::GetChildrenCount(element);
                    for (int i = 0; i < c; i++) {
                        auto ch = VisualTreeHelper::GetChild(element, i)
                                      .try_as<FrameworkElement>();
                        if (ch) {
                            recurse(ch);
                        }
                    }
                };

            recurse(taskbarFrame);
        }

        std::sort(newIcons.begin(), newIcons.end(),
                  [](DockTaskbarIconInfo const& a,
                     DockTaskbarIconInfo const& b) {
                      return a.originalCenterX < b.originalCenterX;
                  });

        ctx.icons = std::move(newIcons);
    } catch (...) {
    }
}

void DockInitializeContext(void* pThisKey,
                           FrameworkElement const& taskbarFrame) {
    try {
        DockAnimationContext ctx;
        ctx.taskbarFrame = taskbarFrame;
        ctx.isInitialized = false;
        ctx.hWnd = GetTaskbarWindowForCurrentThread();
        DockUpdateTaskbarEdge(ctx);

        std::lock_guard<std::mutex> lock(g_dockContextsMutex);
        g_dockContexts[pThisKey] = std::move(ctx);
    } catch (...) {
    }
}

// Main animation loop.
void DockOnRendering(winrt::Windows::Foundation::IInspectable const&,
                     winrt::Windows::Foundation::IInspectable const&) {
    try {
        DockSettings settings = GetDockSettings();

        if (!settings.enabled) {
            if (g_dockIsRenderingHooked.exchange(false)) {
                Media::CompositionTarget::Rendering(g_dockRenderingToken);
            }
            return;
        }

        auto now = std::chrono::steady_clock::now();

        double deltaTime =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                now - g_dockLastRenderTime)
                .count();
        g_dockLastRenderTime = now;

        double dtSec = std::max(0.0, deltaTime) / 1000.0;

        double intensityChange = deltaTime / settings.focusDuration;
        double currentIntensity = g_dockAnimationIntensity.load();

        if (g_dockIsMouseInside) {
            currentIntensity =
                std::min(1.0, currentIntensity + intensityChange);
        } else {
            currentIntensity =
                std::max(0.0, currentIntensity - intensityChange);
        }

        g_dockAnimationIntensity = currentIntensity;

        void* pThisKey = g_dockActiveContextKey.load();
        if (pThisKey == nullptr) {
            if (currentIntensity <= 0.0) {
                Media::CompositionTarget::Rendering(g_dockRenderingToken);
                g_dockIsRenderingHooked = false;
            }
            return;
        }

        auto* ctx = FindDockContext(pThisKey);
        if (!ctx) {
            return;
        }

        if (!g_dockIsMouseInside && currentIntensity <= 0.0) {
            Media::CompositionTarget::Rendering(g_dockRenderingToken);
            g_dockIsRenderingHooked = false;

            DockResetAllIconScales(ctx->icons);
            ctx->isInitialized = false;
            ctx->icons.clear();

            g_dockActiveContextKey = nullptr;
            g_dockLastMouseX = -1.0;
            g_dockIsBouncing = false;
            return;
        }

        if (!ctx->isInitialized) {
            DockRefreshIconPositions(*ctx, settings);
            ctx->isInitialized = true;
        }

        // Dirty check.
        if (g_dockIsMouseInside) {
            auto sinceCheck =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - ctx->lastDirtyCheck)
                    .count();

            if (sinceCheck > 120) {
                ctx->lastDirtyCheck = now;

                auto frame = ctx->taskbarFrame.get();
                if (frame) {
                    DockUpdateTaskbarEdge(*ctx);

                    auto host = ctx->iconHost.get();
                    if (!host) {
                        host = DockFindIconHost(frame);
                        if (host) {
                            ctx->iconHost = host;
                        }
                    }

                    if (host) {
                        DockHostSignature sig =
                            DockComputeHostSignature(host, ctx->isVertical);

                        if (DockSigDifferent(sig, ctx->lastSig)) {
                            DockRefreshIconPositions(*ctx, settings);
                        } else {
                            DockRebaseIconGeometryFast(*ctx, settings);
                        }

                        ctx->lastSig = sig;
                    }
                }
            }
        }

        if (ctx->icons.empty()) {
            if (ctx->isInitialized) {
                DockRefreshIconPositions(*ctx, settings);
            }
            if (ctx->icons.empty()) {
                return;
            }
        }

        if (settings.disableBounce) {
            g_dockIsBouncing = false;
        } else {
            auto elapsedSinceMove =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - g_dockLastSignificantMoveTime)
                    .count();

            if (g_dockIsMouseInside &&
                elapsedSinceMove > settings.bounceDelay) {
                if (!g_dockIsBouncing.exchange(true)) {
                    g_dockBounceStartTime = now;
                }
            } else {
                g_dockIsBouncing = false;
            }
        }

        double targetBounceIntensity = g_dockIsBouncing ? 1.0 : 0.0;
        double bounceFadeDuration = settings.focusDuration / 1000.0;
        double bounceIntensityChange = dtSec / bounceFadeDuration;
        double currentBounceIntensity = g_dockBounceIntensity.load();
        if (targetBounceIntensity > currentBounceIntensity) {
            currentBounceIntensity =
                std::min(targetBounceIntensity,
                         currentBounceIntensity + bounceIntensityChange);
        } else {
            currentBounceIntensity =
                std::max(targetBounceIntensity,
                         currentBounceIntensity - bounceIntensityChange);
        }
        g_dockBounceIntensity = currentBounceIntensity;

        double mouseX = g_dockLastMouseX.load();
        if (mouseX == -1.0) {
            mouseX = 0.0;
        }

        DockApplyAnimation(mouseX, *ctx, currentIntensity, dtSec, settings);
    } catch (...) {
        if (g_dockIsRenderingHooked.exchange(false)) {
            try {
                Media::CompositionTarget::Rendering(g_dockRenderingToken);
            } catch (...) {
            }
        }
    }
}

// Called on a taskbar UI thread: resets the animation state of the taskbars
// owned by that thread, so that settings changes and unloading restore the
// original transforms.
void DockResetOnCurrentThread() {
    DWORD threadId = GetCurrentThreadId();
    std::vector<DockAnimationContext*> contexts;

    {
        std::lock_guard<std::mutex> lock(g_dockContextsMutex);
        for (auto& pair : g_dockContexts) {
            auto& ctx = pair.second;
            HWND hWnd = ctx.hWnd;
            if (hWnd &&
                GetWindowThreadProcessId(hWnd, nullptr) == threadId) {
                contexts.push_back(&ctx);
            }
        }
    }

    if (contexts.empty()) {
        return;
    }

    for (auto* ctx : contexts) {
        DockResetAllIconScales(ctx->icons);
        ctx->isInitialized = false;
        ctx->icons.clear();
    }

    if (g_dockIsRenderingHooked.exchange(false)) {
        try {
            Media::CompositionTarget::Rendering(g_dockRenderingToken);
        } catch (...) {
        }
    }

    g_dockActiveContextKey = nullptr;
    g_dockIsBouncing = false;
    g_dockBounceIntensity = 0.0;
    g_dockIsMouseInside = false;
    g_dockAnimationIntensity = 0.0;
    g_dockLastMouseX = -1.0;
}

void DockClearContexts() {
    std::lock_guard<std::mutex> lock(g_dockContextsMutex);
    g_dockContexts.clear();
}

void DockOnTaskbarPointerMoved(void* pThis, void* pArgs) {
    if (g_unloading) {
        return;
    }

    DockSettings settings = GetDockSettings();
    if (!settings.enabled) {
        return;
    }

    try {
        FrameworkElement element = nullptr;
        ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(element));
        if (!element ||
            winrt::get_class_name(element) != L"Taskbar.TaskbarFrame") {
            return;
        }

        Input::PointerRoutedEventArgs args = nullptr;
        ((IUnknown*)pArgs)
            ->QueryInterface(
                winrt::guid_of<Input::PointerRoutedEventArgs>(),
                winrt::put_abi(args));
        if (!args) {
            return;
        }

        bool hasContext;
        {
            std::lock_guard<std::mutex> lock(g_dockContextsMutex);
            hasContext = g_dockContexts.contains(pThis);
        }
        if (!hasContext) {
            DockInitializeContext(pThis, element);
        }

        DockOnPointerMoved(pThis, args);
    } catch (...) {
    }
}

void DockOnTaskbarPointerExited(void* pThis) {
    if (g_unloading) {
        return;
    }
    DockOnPointerExited(pThis);
}

using TaskbarFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 winrt::Windows::Foundation::Size size,
                 winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    HookScope hookScope;

    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
    HandleFrameEvent(pThis);
    return ret;
}

using TaskbarFrame_OnApplyTemplate_t = int(WINAPI*)(void* pThis);
TaskbarFrame_OnApplyTemplate_t TaskbarFrame_OnApplyTemplate_Original;
int WINAPI TaskbarFrame_OnApplyTemplate_Hook(void* pThis) {
    HookScope hookScope;

    int ret = TaskbarFrame_OnApplyTemplate_Original(pThis);
    HandleFrameEvent(pThis);
    return ret;
}

using TaskbarFrame_OnLoaded_t =
    void(WINAPI*)(void* pThis, void* sender, void* args);
TaskbarFrame_OnLoaded_t TaskbarFrame_OnLoaded_Original;
void WINAPI TaskbarFrame_OnLoaded_Hook(void* pThis, void* sender, void* args) {
    HookScope hookScope;

    TaskbarFrame_OnLoaded_Original(pThis, sender, args);
    HandleFrameEvent(pThis);
}

using TaskbarFrame_OnPointerMoved_t =
    int(WINAPI*)(void* pThis, void* args);
TaskbarFrame_OnPointerMoved_t TaskbarFrame_OnPointerMoved_Original;
int WINAPI TaskbarFrame_OnPointerMoved_Hook(void* pThis, void* args) {
    HookScope hookScope;

    DockOnTaskbarPointerMoved(pThis, args);

    int ret = TaskbarFrame_OnPointerMoved_Original(pThis, args);
    HandleFrameEvent(pThis);
    return ret;
}

using TaskbarFrame_OnPointerExited_t =
    int(WINAPI*)(void* pThis, void* args);
TaskbarFrame_OnPointerExited_t TaskbarFrame_OnPointerExited_Original;
int WINAPI TaskbarFrame_OnPointerExited_Hook(void* pThis, void* args) {
    HookScope hookScope;

    DockOnTaskbarPointerExited(pThis);

    int ret = TaskbarFrame_OnPointerExited_Original(pThis, args);
    return ret;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

void ReassertFramePropertiesOnCurrentThread() {
    if (g_unloading) {
        return;
    }

    DWORD threadId = GetCurrentThreadId();
    std::vector<AppliedElement> records;
    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto const& applied : g_applied) {
            if (applied.threadId == threadId) {
                records.push_back(applied);
            }
        }
    }

    for (auto const& record : records) {
        auto frame = record.frame.get();
        if (!frame) {
            continue;
        }

        try {
            const int taskbarWidthSetting = g_settingTaskbarWidth.load();
            const int taskbarAreaHeightSetting =
                g_settingTaskbarAreaHeight.load();
            const int floatMarginSetting = g_settingFloatMargin.load();

            HWND taskbarWnd = GetTaskbarWindowForCurrentThread();
            double scale = GetEffectiveScale(frame, taskbarWnd);
            double areaHeight = GetTaskbarAreaHeightDip(
                taskbarAreaHeightSetting, frame, taskbarWnd, scale);
            double floatMargin = GetFloatMarginDip(floatMarginSetting, scale);
            double barHeight =
                GetBarHeightDip(areaHeight,
                                GetWindowHeightDip(taskbarWnd, scale),
                                floatMargin);

            if (taskbarWidthSetting > 0) {
                double pillWidth = GetTaskbarWidthDip(
                    taskbarWidthSetting, frame, taskbarWnd, scale);
                frame.MinWidth(pillWidth);
                frame.MaxWidth(pillWidth);
                if (auto rootGrid = record.rootGrid.get()) {
                    rootGrid.Width(pillWidth);
                    rootGrid.HorizontalAlignment(HorizontalAlignment::Center);
                }
            }

            frame.HorizontalAlignment(HorizontalAlignment::Center);
            frame.Margin(Thickness{0, 0, 0, 0});

            if (taskbarAreaHeightSetting > 0 && scale > 0) {
                frame.MaxHeight(std::numeric_limits<double>::infinity());
                double frameHeightDip = GetFrameHeightDip(
                    taskbarAreaHeightSetting, taskbarWnd, scale);
                if (frameHeightDip > 0) {
                    frame.Height(frameHeightDip);
                    // Explorer's frame height update shrinks the frame's parent
                    // back, so reassert the parent height as well.
                    ReassertParentHeight(record, frameHeightDip);
                }
                frame.VerticalAlignment(VerticalAlignment::Bottom);
            }

            if (areaHeight > 0 && barHeight > 0) {
                if (auto rootGrid = record.rootGrid.get()) {
                    rootGrid.Height(barHeight);
                    rootGrid.VerticalAlignment(VerticalAlignment::Bottom);
                    rootGrid.Margin(Thickness{0, 0, 0, floatMargin});
                }

                if (auto tray = record.tray.get()) {
                    double gapDip = GetTrayGapDip(taskbarWidthSetting, frame,
                                                  tray, taskbarWnd, scale);
                    double contentHeight =
                        GetSystemTaskbarContentHeightDip();
                    tray.Height(barHeight);
                    tray.VerticalAlignment(VerticalAlignment::Bottom);
                    tray.Margin(Thickness{0, 0, gapDip, floatMargin});

                    std::vector<FrameworkElement> hoverBorders;
                    CollectTrayHoverBorders(tray, hoverBorders);
                    double hoverHeight =
                        std::max(1.0, contentHeight - 14.0);
                    for (auto const& element : hoverBorders) {
                        ApplyTrayHoverBorderLayout(element, hoverHeight);
                    }

                    double rowHeight = tray.ActualHeight();
                    if (!(rowHeight > 0)) {
                        rowHeight = barHeight;
                    }
                    std::vector<FrameworkElement> omniButtons;
                    CollectTrayOmniButtons(tray, omniButtons);
                    for (auto const& element : omniButtons) {
                        ApplyTrayOmniButtonLayout(element, rowHeight);
                    }
                }

                if (auto repeater = record.iconRepeater.get()) {
                    repeater.Height(GetSystemTaskbarContentHeightDip());
                    repeater.VerticalAlignment(VerticalAlignment::Center);
                }
            }

            {
                std::vector<FrameworkElement> hoverBackgrounds;
                if (auto rootGrid = record.rootGrid.get()) {
                    CollectHoverBackgroundBorders(rootGrid,
                                                  hoverBackgrounds);
                }
                if (auto tray = record.tray.get()) {
                    CollectHoverBackgroundBorders(tray,
                                                  hoverBackgrounds);
                }
                double hoverCornerRadiusDip =
                    scale > 0
                        ? g_settingHoverCornerRadius.load() / scale
                        : 0.0;
                for (auto const& element : hoverBackgrounds) {
                    ApplyHoverBorderShape(element, hoverCornerRadiusDip);
                }
            }
        } catch (...) {
        }
    }
}

using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    HookScope hookScope;

    TaskbarController_UpdateFrameHeight_Original(pThis);
    ReassertFramePropertiesOnCurrentThread();
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))",
            },
            &TaskbarFrame_MeasureOverride_Original,
            TaskbarFrame_MeasureOverride_Hook,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))",
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnApplyTemplate(void))",
            },
            &TaskbarFrame_OnApplyTemplate_Original,
            TaskbarFrame_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnLoaded(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Windows::UI::Xaml::RoutedEventArgs const &))",
            },
            &TaskbarFrame_OnLoaded_Original,
            TaskbarFrame_OnLoaded_Hook,
            true,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))",
            },
            &TaskbarFrame_OnPointerMoved_Original,
            TaskbarFrame_OnPointerMoved_Hook,
            true,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))",
            },
            &TaskbarFrame_OnPointerExited_Original,
            TaskbarFrame_OnPointerExited_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))",
            },
            &TaskbarController_UpdateFrameHeight_Original,
            TaskbarController_UpdateFrameHeight_Hook,
            true,
        },
        {
            {
                LR"(bool __cdecl ShouldCycleWindows(void))",
            },
            &ShouldCycleWindows_Original,
            ShouldCycleWindows_Hook,
            true,
        },
        // Auto-hide when maximized.
        {
            {
                LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))",
            },
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))",
            },
            &ViewCoordinator_UpdateIsExpanded_Original,
            nullptr,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    Wh_Log(L"HookSymbols succeeded");
    return true;
}

bool HookSystemTrayDllSymbols(HMODULE module);

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HookScope hookScope;

    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module || g_unloading) {
        return module;
    }

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module) {
        if (!g_taskbarViewDllLoaded.exchange(true)) {
            Wh_Log(L"Taskbar.View.dll loaded, hooking symbols");
            if (HookTaskbarViewDllSymbols(module)) {
                if (!g_hooksApplied.exchange(true)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (!g_systemTrayDllLoaded && GetModuleHandle(L"SystemTray.dll") == module) {
        if (!g_systemTrayDllLoaded.exchange(true)) {
            Wh_Log(L"SystemTray.dll loaded, hooking symbols");
            if (HookSystemTrayDllSymbols(module)) {
                Wh_ApplyHookOperations();
            }
        }
    }

    return module;
}

struct ForEachTaskbarWindowContext {
    void (*proc)(HWND);
};

BOOL CALLBACK ForEachTaskbarWindowEnumProc(HWND hWnd, LPARAM lParam) {
    auto context = reinterpret_cast<ForEachTaskbarWindowContext*>(lParam);

    DWORD dwProcessId = 0;
    WCHAR className[64];
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId() ||
        !GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (_wcsicmp(className, L"Shell_TrayWnd") != 0 &&
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
        return TRUE;
    }

    context->proc(hWnd);
    return TRUE;
}

void ForEachTaskbarWindow(void (*proc)(HWND)) {
    ForEachTaskbarWindowContext context{proc};
    EnumWindows(ForEachTaskbarWindowEnumProc,
                reinterpret_cast<LPARAM>(&context));
}

void KickTaskbarRelayout() {
    if (!g_settingFloatingBarEnabled.load()) {
        return;
    }

    ForEachTaskbarWindow([](HWND hWnd) {
        SendMessageTimeoutW(hWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE,
                            0, SMTO_ABORTIFHUNG | SMTO_NORMAL, 1000, nullptr);
    });
}

using RunFromWindowThreadProc_t = void (*)(PVOID);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}

void RefreshTaskbarIconInsetOnCurrentThread() {
    if (g_unloading || !g_settingFloatingBarEnabled.load()) {
        return;
    }

    DWORD threadId = GetCurrentThreadId();
    std::vector<AppliedElement> records;
    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto const& applied : g_applied) {
            if (applied.threadId == threadId) {
                records.push_back(applied);
            }
        }
    }

    const double leftInset = GetTaskbarIconsLeftInsetDip();
    for (auto const& record : records) {
        auto repeater = record.iconRepeater.get();
        if (!repeater) {
            continue;
        }

        try {
            bool changed = false;
            if (repeater.HorizontalAlignment() != HorizontalAlignment::Left) {
                repeater.HorizontalAlignment(HorizontalAlignment::Left);
                changed = true;
            }

            Thickness margin = repeater.Margin();
            if (std::abs(margin.Left - leftInset) > 0.5) {
                margin.Left = leftInset;
                repeater.Margin(margin);
                changed = true;
            }

            if (changed) {
                Wh_Log(L"Reasserted taskbar icon inset: %.1f DIP (center=%d)",
                       leftInset, IsTaskbarCenterAligned() ? 1 : 0);
            }
        } catch (...) {
        }
    }
}

void RefreshTaskbarIconInsetForAllTaskbars() {
    if (g_hostKind != HostKind::Taskbar || g_unloading) {
        return;
    }

    ForEachTaskbarWindow([](HWND hWnd) {
        // Post instead of SendMessage: the watcher thread must stay
        // interruptible while the mod is being unloaded.
        PostMessage(hWnd, g_refreshIconInsetRegisteredMsg, 0, 0);
    });
}

// The shell can re-apply its native taskbar alignment after the mod's last
// apply, which reverts the forced left alignment or the alignment-specific
// margin until the next pointer event. Reassert the inset a few times while
// the shell settles so the row is correct without requiring mouse movement.
void SettleTaskbarIconInsetAfterAlignmentChange() {
    constexpr DWORD kDelaysMs[] = {0, 150, 400, 900};
    DWORD elapsedMs = 0;
    for (DWORD delayMs : kDelaysMs) {
        if (delayMs > elapsedMs) {
            DWORD waitMs = delayMs - elapsedMs;
            if (g_taskbarAlignmentWatcherStopEvent &&
                WaitForSingleObject(g_taskbarAlignmentWatcherStopEvent,
                                    waitMs) == WAIT_OBJECT_0) {
                return;
            }
            elapsedMs = delayMs;
        }

        RefreshTaskbarIconInsetForAllTaskbars();
    }
}

DWORD WINAPI TaskbarAlignmentWatcherProc(LPVOID) {
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
            0, KEY_NOTIFY | KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) {
        return 0;
    }

    HANDLE regEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    bool lastCenter = IsTaskbarCenterAligned();

    // Also cover a late native layout pass after the initial apply.
    SettleTaskbarIconInsetAfterAlignmentChange();

    while (!g_taskbarAlignmentWatcherStop.load()) {
        if (!regEvent ||
            RegNotifyChangeKeyValue(hKey, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                                    regEvent, TRUE) != ERROR_SUCCESS) {
            if (WaitForSingleObject(g_taskbarAlignmentWatcherStopEvent, 500) ==
                WAIT_OBJECT_0) {
                break;
            }

            bool center = IsTaskbarCenterAligned();
            if (center != lastCenter) {
                lastCenter = center;
                SettleTaskbarIconInsetAfterAlignmentChange();
            }
            continue;
        }

        HANDLE handles[] = {g_taskbarAlignmentWatcherStopEvent, regEvent};
        DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }
        if (waitResult != WAIT_OBJECT_0 + 1) {
            continue;
        }

        bool center = IsTaskbarCenterAligned();
        if (center != lastCenter) {
            lastCenter = center;
            Wh_Log(L"Taskbar alignment changed to %s; refreshing icon inset",
                   center ? L"Center" : L"Left");
            SettleTaskbarIconInsetAfterAlignmentChange();
        }
    }

    if (regEvent) {
        CloseHandle(regEvent);
    }
    RegCloseKey(hKey);
    return 0;
}

void StartTaskbarAlignmentWatcher() {
    if (g_hostKind != HostKind::Taskbar ||
        g_taskbarAlignmentWatcherThread) {
        return;
    }

    g_taskbarAlignmentWatcherStop = false;
    g_taskbarAlignmentWatcherStopEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_taskbarAlignmentWatcherStopEvent) {
        return;
    }

    g_taskbarAlignmentWatcherThread = CreateThread(
        nullptr, 0, TaskbarAlignmentWatcherProc, nullptr, 0, nullptr);
    if (!g_taskbarAlignmentWatcherThread) {
        CloseHandle(g_taskbarAlignmentWatcherStopEvent);
        g_taskbarAlignmentWatcherStopEvent = nullptr;
    }
}

void StopTaskbarAlignmentWatcher() {
    g_taskbarAlignmentWatcherStop = true;
    if (g_taskbarAlignmentWatcherStopEvent) {
        SetEvent(g_taskbarAlignmentWatcherStopEvent);
    }

    if (g_taskbarAlignmentWatcherThread) {
        WaitForSingleObject(g_taskbarAlignmentWatcherThread, 3000);
        CloseHandle(g_taskbarAlignmentWatcherThread);
        g_taskbarAlignmentWatcherThread = nullptr;
    }

    if (g_taskbarAlignmentWatcherStopEvent) {
        CloseHandle(g_taskbarAlignmentWatcherStopEvent);
        g_taskbarAlignmentWatcherStopEvent = nullptr;
    }
}

using PositionContextMenuPopup_t = void(WINAPI*)(void const* popup,
                                                 Grid const* anchorGrid,
                                                 double param3,
                                                 int taskbarLocation,
                                                 double param5);
PositionContextMenuPopup_t PositionContextMenuPopup_Original;
void WINAPI PositionContextMenuPopup_Hook(void const* popup,
                                          Grid const* anchorGrid,
                                          double param3,
                                          int taskbarLocation,
                                          double param5) {
    HookScope hookScope;

    PositionContextMenuPopup_Original(popup, anchorGrid, param3,
                                      taskbarLocation, param5);

    if (!g_settingFloatingBarEnabled.load() || taskbarLocation != 3 ||
        !popup) {
        return;
    }

    try {
        auto popupObject =
            *static_cast<winrt::Windows::UI::Xaml::Controls::Primitives::
                             Popup const*>(popup);
        if (!popupObject) {
            return;
        }

        double menuWidthDip = 0.0;
        double menuHeightDip = 0.0;
        if (auto child = popupObject.Child()) {
            if (auto childElement = child.try_as<FrameworkElement>()) {
                menuWidthDip = childElement.ActualWidth();
                menuHeightDip = childElement.ActualHeight();
            }
        }

        HWND taskbarWnd = GetTaskbarWindowForCurrentThread();
        double scale = GetTaskbarWindowScale(taskbarWnd);
        if (scale <= 0) {
            scale = 1.0;
        }

        HWND islandHwnd = FindIslandWindow(taskbarWnd);
        if (menuWidthDip <= 1.0 || menuHeightDip <= 1.0 || !islandHwnd) {
            return;
        }

        POINT islandOrigin{};
        ClientToScreen(islandHwnd, &islandOrigin);
        POINT cursor{};
        GetCursorPos(&cursor);

        double cursorDipX = (cursor.x - islandOrigin.x) / scale;
        popupObject.HorizontalOffset(cursorDipX - menuWidthDip / 2.0);
        popupObject.VerticalOffset(-(menuHeightDip + 4.0));
    } catch (...) {
    }
}

bool HookSystemTrayDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                LR"(void __cdecl ContextMenuPositionHelper::PositionContextMenuPopup(struct winrt::Windows::UI::Xaml::Controls::Primitives::Popup const &,struct winrt::Windows::UI::Xaml::Controls::Grid const &,double,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,double))",
                LR"(public: static void __cdecl ContextMenuPositionHelper::PositionContextMenuPopup(struct winrt::Windows::UI::Xaml::Controls::Primitives::Popup const &,struct winrt::Windows::UI::Xaml::Controls::Grid const &,double,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,double))",
            },
            &PositionContextMenuPopup_Original,
            PositionContextMenuPopup_Hook,
            true,
        },
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
            true,
        },
        // Taskbar clock customization.
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockRefreshIcon_Original,
            ClockRefreshIcon_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::RefreshIcon(class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockRefreshIcon2_Original,
            ClockRefreshIcon2_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockGetToolTipString_Original,
            ClockGetToolTipString_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::GetTimeToolTipString(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockGetToolTipString2_Original,
            ClockGetToolTipString2_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString2(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockGetToolTipString_2_Original,
            ClockGetToolTipString_2_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::GetTimeToolTipString2(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockGetToolTipString2_2_Original,
            ClockGetToolTipString2_2_Hook,
            true,
        },
        {
            {LR"(public: int __cdecl winrt::impl::consume_Windows_Globalization_ICalendar<struct winrt::Windows::Globalization::ICalendar>::Second(void)const )"},
            &ClockICalendarSecond_Original,
            ClockICalendarSecond_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::DateTimeIconContent,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
            &ClockDateTimeIconContent_OnApplyTemplate_Original,
            ClockDateTimeIconContent_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: static __cdecl winrt::Windows::System::Threading::ThreadPoolTimer::CreateTimer(struct winrt::Windows::System::Threading::TimerElapsedHandler const &,class std::chrono::duration<__int64,struct std::ratio<1,10000000> > const &))",
                LR"(public: struct winrt::Windows::System::Threading::ThreadPoolTimer __cdecl winrt::impl::consume_Windows_System_Threading_IThreadPoolTimerStatics<struct winrt::Windows::System::Threading::IThreadPoolTimerStatics>::CreateTimer(struct winrt::Windows::System::Threading::TimerElapsedHandler const &,class std::chrono::duration<__int64,struct std::ratio<1,10000000> > const &)const )",
            },
            &ClockCreateTimer_Original,
            ClockCreateTimer_Hook,
            true,
        },
        {
            {
                LR"(public: __cdecl <lambda_b19cf72fe9674443383aa89d5c22450b>::operator()(struct winrt::Windows::System::Threading::IThreadPoolTimerStatics const &)const )",
                LR"(public: struct winrt::Windows::System::Threading::ThreadPoolTimer __cdecl <lambda_b19cf72fe9674443383aa89d5c22450b>::operator()(struct winrt::Windows::System::Threading::IThreadPoolTimerStatics const &)const )",
            },
            &ClockCreateTimerLambda_Original,
            ClockCreateTimerLambda_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for SystemTray.dll");
        return false;
    }

    return true;
}
void KickTaskbarHeightUpdate() {
    if (!g_settingFloatingBarEnabled.load()) {
        return;
    }

    if (g_settingTaskbarAreaHeight.load() <= 0) {
        return;
    }

    DWORD now = GetTickCount();
    if (now - g_lastTaskbarHeightKickTick.load() < 100) {
        return;
    }
    g_lastTaskbarHeightKickTick = now;

    ForEachTaskbarWindow([](HWND hWnd) {
        SendMessageW(hWnd, WM_SETTINGCHANGE, 0x009F, 0);

        HWND hReBar = FindWindowExW(hWnd, nullptr, L"ReBarWindow32", nullptr);
        if (!hReBar) {
            return;
        }

        HWND hTaskSw =
            FindWindowExW(hReBar, nullptr, L"MSTaskSwWClass", nullptr);
        if (hTaskSw) {
            SendMessageW(hTaskSw, 0x452, 3, 0);
        }
    });
}

void ApplyToAllTaskbarsNow() {
    ForEachTaskbarWindow([](HWND hWnd) {
        RunFromWindowThread(hWnd, ApplyNowOnTaskbarThread, hWnd);
    });
}

DWORD WINAPI TrayLayoutStartupBurstProc(LPVOID) {
    // Explorer creates parts of the tray after the first ApplyToFrame pass.
    // A few quick re-applies make the initial frame centered instead of
    // waiting for the next natural layout event (which was about two seconds
    // after an Explorer restart).
    constexpr DWORD kDelays[] = {50, 120, 200, 300, 500, 1000, 2000};
    for (DWORD delay : kDelays) {
        Sleep(delay);
        if (g_unloading) {
            break;
        }
        ApplyToAllTaskbarsNow();
        KickTaskbarHeightUpdate();
    }
    return 0;
}

void StartTrayLayoutStartupBurst() {
    if (!g_settingFloatingBarEnabled.load()) {
        return;
    }

    HANDLE thread = CreateThread(nullptr, 0, TrayLayoutStartupBurstProc,
                                 nullptr, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    }
}

std::atomic<bool> g_trayReadyWatchRunning{false};
std::atomic<bool> g_trayReadyWatchNeedsMore{false};
std::atomic<int> g_trayReadyWatchTaskbars{0};

void ReapplyIfTrayNotReadyOnCurrentThread(PVOID param) {
    HWND hWnd = (HWND)param;
    bool notReady = false;
    bool foundRecord = false;

    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        DWORD threadId = GetCurrentThreadId();
        for (auto const& applied : g_applied) {
            if (applied.threadId != threadId) {
                continue;
            }

            foundRecord = true;
            auto tray = applied.tray.get();
            if (!applied.trayArranged || !applied.trayContentReady ||
                !tray || tray.ActualWidth() < 1.0 ||
                tray.ActualHeight() < 1.0) {
                notReady = true;
            }
        }
    }

    if (!foundRecord) {
        notReady = true;
    }

    if (notReady) {
        g_trayReadyWatchNeedsMore = true;
        ApplyNowOnTaskbarThread(hWnd);
    }
}

DWORD WINAPI TrayReadyWatchProc(LPVOID) {
    // The fixed startup burst can finish before Explorer has built the tray
    // subtree. Keep checking until the tray has a real size, then let one
    // final apply shrink and round the OmniButton hover backgrounds. This
    // also covers taskbars created after the initial frame.
    for (int i = 0; i < 40; i++) {
        Sleep(250);
        if (g_unloading || !g_settingFloatingBarEnabled.load()) {
            break;
        }

        g_trayReadyWatchNeedsMore = false;
        g_trayReadyWatchTaskbars = 0;
        ForEachTaskbarWindow([](HWND hWnd) {
            g_trayReadyWatchTaskbars.fetch_add(1);
            RunFromWindowThread(hWnd, ReapplyIfTrayNotReadyOnCurrentThread,
                                hWnd);
        });

        if (g_trayReadyWatchTaskbars.load() > 0 &&
            !g_trayReadyWatchNeedsMore.load()) {
            break;
        }
    }

    g_trayReadyWatchRunning = false;
    return 0;
}

void StartTrayReadyWatcher() {
    if (!g_settingFloatingBarEnabled.load()) {
        return;
    }
    if (g_trayReadyWatchRunning.exchange(true)) {
        return;
    }

    HANDLE thread = CreateThread(nullptr, 0, TrayReadyWatchProc, nullptr, 0,
                                 nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_trayReadyWatchRunning = false;
    }
}

void ReapplyOnCurrentThread(PVOID) {
    DWORD threadId = GetCurrentThreadId();
    std::vector<winrt::weak_ref<FrameworkElement>> frames;

    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto const& applied : g_applied) {
            if (applied.threadId == threadId) {
                frames.push_back(applied.frame);
            }
        }
    }

    for (auto const& frame : frames) {
        if (auto element = frame.get()) {
            try {
                ApplyToFrame(element);
            } catch (...) {
            }
        }
    }
}

void RestoreOnCurrentThread(PVOID) {
    RestoreHoverFlyoutSuppression();
    RestoreHoverTransforms();
    RestoreHoverBackgroundTransforms();
    RestoreMultiWindowHighlights();
    RestoreTaskButtonClips();
    RestoreTaskButtonStackEffects();
    RestoreTaskButtonStackCards();
    RestoreTaskButtonOverflowLayers();

    // Tray system icon tweaks are restored and re-applied together with the
    // taskbar frame properties.
    RestoreTrayIconStyles();

    // When the auto-hide enhancement is disabled, let the system hide the
    // taskbar again and restore its auto-hide setting.
    if (!g_settingAutoHideEnabled.load()) {
        AutoHideReleaseOnCurrentThread();
    }

    // Dock hover animation: restore the icon transforms of this thread's
    // taskbars and stop the animation loop.
    DockResetOnCurrentThread();

    DWORD threadId = GetCurrentThreadId();
    std::vector<AppliedElement> pending;
    std::vector<FrameworkElement> traysToClear;

    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto it = g_applied.begin(); it != g_applied.end();) {
            if (it->threadId == threadId) {
                if (auto tray = it->tray.get()) {
                    traysToClear.push_back(tray);
                }
                pending.push_back(std::move(*it));
                it = g_applied.erase(it);
            } else {
                ++it;
            }
        }
    }

    for (auto const& tray : traysToClear) {
        ClockClearTextStyles(tray);
    }

    std::vector<AppliedElement> failed;
    for (auto& applied : pending) {
        bool restored = false;
        try {
            restored = RestoreFrame(applied);
        } catch (...) {
            restored = false;
        }

        if (!restored) {
            failed.push_back(std::move(applied));
        }
    }

    if (!failed.empty()) {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        for (auto& applied : failed) {
            g_applied.push_back(std::move(applied));
        }
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_hostKind = GetHostKind();

    // The Quick Settings panel is positioned by the process which hosts it, so
    // the mod is loaded into that process as well and adjusts the position
    // there. In the taskbar process this hook is a no-op.
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);
    WindhawkUtils::SetFunctionHook(MoveWindow, MoveWindow_Hook,
                                   &MoveWindow_Original);

    if (g_hostKind == HostKind::ControlCenter) {
        Wh_Log(L"Control Center host process");
        return TRUE;
    }

    // Private user32 functions used by the auto-hide logic.
    if (HMODULE hUser32Module =
            LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)) {
        pIsWindowArranged = (IsWindowArranged_t)GetProcAddress(
            hUser32Module, "IsWindowArranged");
        pGetWindowBand = (GetWindowBand_t)GetProcAddress(hUser32Module,
                                                         "GetWindowBand");
    }

    // Taskbar clock customization: hook the system date/time formatting
    // functions, which the clock uses while refreshing its data model.
    if (HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll")) {
        auto pGetTimeFormatEx = (decltype(&GetTimeFormatEx))GetProcAddress(
            kernelBaseModule, "GetTimeFormatEx");
        auto pGetDateFormatEx = (decltype(&GetDateFormatEx))GetProcAddress(
            kernelBaseModule, "GetDateFormatEx");
        auto pGetLocalTime = (decltype(&GetLocalTime))GetProcAddress(
            kernelBaseModule, "GetLocalTime");

        if (pGetTimeFormatEx && pGetDateFormatEx && pGetLocalTime) {
            WindhawkUtils::SetFunctionHook(pGetTimeFormatEx,
                                           ClockGetTimeFormatEx_Hook,
                                           &ClockGetTimeFormatEx_Original);
            WindhawkUtils::SetFunctionHook(pGetDateFormatEx,
                                           ClockGetDateFormatEx_Hook,
                                           &ClockGetDateFormatEx_Original);
            WindhawkUtils::SetFunctionHook(pGetLocalTime,
                                           ClockGetLocalTime_Hook,
                                           &ClockGetLocalTime_Original);
        } else {
            Wh_Log(L"Taskbar clock: failed to find date/time functions");
        }
    }

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Taskbar symbols are not available, the taskbar window won't "
               L"be found by the mod");
    }

    // The LoadLibraryExW hook is needed as long as one of the modules isn't
    // loaded yet, so that its symbols can be hooked when it is.
    bool delayLoadingNeeded = false;

    if (HMODULE systemTrayModule = GetModuleHandle(L"SystemTray.dll")) {
        g_systemTrayDllLoaded = true;
        HookSystemTrayDllSymbols(systemTrayModule);
    } else {
        delayLoadingNeeded = true;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        delayLoadingNeeded = true;
    }

    if (delayLoadingNeeded) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_hostKind == HostKind::ControlCenter) {
        return;
    }

    if (g_taskbarViewDllLoaded) {
        g_hooksApplied = true;
    }

    ApplyToAllTaskbarsNow();
    StartTrayLayoutStartupBurst();
    StartTrayReadyWatcher();
    KickTaskbarRelayout();
    KickTaskbarHeightUpdate();
    StartTaskbarAlignmentWatcher();

    if (HWND primaryTaskbarWnd = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        RunFromWindowThread(primaryTaskbarWnd,
                            InstallControlCenterHookOnCurrentThread, nullptr);
    }

    if (g_settingFloatingBarEnabled.load()) {
        RepositionControlCenterWindows(L"init");
        StartControlCenterWatchdog();
        StartHoverWatchdog();
    }

    // Auto-hide when maximized.
    AdjustAllTaskbars();

    // Apply the clock settings right away instead of waiting for the clock's
    // next natural refresh.
    ClockForceRefresh();
    ClockStartWebUpdateThreadIfNeeded();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    bool prevAutoHideForegroundWindowOnly =
        g_settingAutoHideForegroundWindowOnly.load();

    LoadSettings();
    g_settingsVersion.fetch_add(1);
    ResetLoggedFlags();

    if (g_hostKind == HostKind::ControlCenter) {
        return;
    }

    if (g_unloading) {
        return;
    }

    ForEachTaskbarWindow([](HWND hWnd) {
        RunFromWindowThread(hWnd, RestoreOnCurrentThread, nullptr);
    });

    ApplyToAllTaskbarsNow();
    KickTaskbarHeightUpdate();
    StartTrayLayoutStartupBurst();
    StartTrayReadyWatcher();

    ForEachTaskbarWindow([](HWND hWnd) {
        RunFromWindowThread(hWnd, ReapplyOnCurrentThread, nullptr);
    });

    RepositionControlCenterWindows(L"settings");

    // Auto-hide when maximized.
    if ((AutoHideMode)g_settingAutoHideMode.load() == AutoHideMode::never ||
        !g_settingAutoHideEnabled.load() ||
        prevAutoHideForegroundWindowOnly !=
            g_settingAutoHideForegroundWindowOnly.load()) {
        StopAutoHideWinEventHookThread();
    }

    AdjustAllTaskbars();

    // The Control Center watchdog is only needed for the floating bar.
    if (g_settingFloatingBarEnabled.load()) {
        StartControlCenterWatchdog();
        StartHoverWatchdog();
    } else {
        StopControlCenterWatchdog();
        StopHoverWatchdog();
    }

    // Apply the clock settings right away.
    ClockForceRefresh();
    ClockStartWebUpdateThreadIfNeeded();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;
    StopTaskbarAlignmentWatcher();

    if (g_hostKind == HostKind::ControlCenter) {
        return;
    }

    ForEachTaskbarWindow([](HWND hWnd) {
        RunFromWindowThread(hWnd, RestoreOnCurrentThread, nullptr);
    });

    {
        std::lock_guard<std::mutex> lock(g_appliedMutex);
        g_applied.clear();
    }

    DockClearContexts();

    // Close the clock's performance query.
    {
        std::lock_guard<std::mutex> lock(g_clockMetricsMutex);
        if (g_clockPdhQuery) {
            PdhCloseQuery(g_clockPdhQuery);
            g_clockPdhQuery = nullptr;
        }
        g_clockCpuTempCounters.clear();
        g_clockGpuCounters.clear();
        g_clockNetworkUploadCounters.clear();
        g_clockNetworkDownloadCounters.clear();
        g_clockDiskReadCounters.clear();
        g_clockDiskWriteCounters.clear();
        g_clockVramUsedCounters.clear();
        g_clockVramSharedUsedCounters.clear();
        g_clockLastSampleTick = 0;
        g_clockPdhCountersRefreshTick = 0;
    }

    StopControlCenterWatchdog();
    StopHoverWatchdog();
    ClockStopWebUpdateThread();
    RemoveControlCenterHook();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hostKind == HostKind::Taskbar) {
        StopAutoHideWinEventHookThread();

        if (g_wasAutoHideDisabled) {
            HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
            if (hTaskbarWnd) {
                SendMessage(hTaskbarWnd, kHandleTrayPrivateSettingMessage,
                            kTrayPrivateSettingAutoHideSet, FALSE);
            }
        }
    }

    // Hooks are already removed at this point, but another thread may still be
    // running one of the hook functions, so wait for it to finish before the
    // mod's code is unloaded.
    while (g_hookCallCounter.load() > 0) {
        Sleep(100);
    }
}
