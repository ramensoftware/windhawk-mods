// ==WindhawkMod==
// @id              chinese-holiday-calendar
// @name            Chinese Holiday Calendar
// @name:zh-CN      中国节假日日历
// @description     Show the Chinese statutory holidays and adjusted workdays of an ICS feed you supply in the Windows 11 calendar flyout, marked by rules you write yourself
// @description:zh-CN 在 Windows 11 日历中按你自己写的规则，标记你填的 ICS 订阅源里的中国法定节假日与调休安排
// @version         0.22
// @author          dcsmf
// @github          https://github.com/dcsmf
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 中国节假日日历

![Chinese Holiday Calendar View](https://i.imgur.com/SBafHNL.png)

Windows 11 的日历（点击任务栏右下角时间/日期弹出）默认只在数字下方显示农历或节气。这个 mod 会把你提供的
节假日数据写进日历的日期单元格，标记哪些日子、怎么标记则由你自己写的规则决定：

- **放假/节日**：默认"休"标记的日子显示节日名（如"国庆节"），蓝色。
- **调休上班日**：默认"班"标记的日子显示"补班"，红色。
- 规则写在设置里，是一条列表：一项一类日子，每项填词、颜色、文字（见下文的"高亮规则"）。想把订阅源里的
  **所有**事件都高亮（别国的节假日订阅源大多没有"休"/"班"这样的标记），加一项、词那栏填 `*` 就行。
- 留空的栏表示不动：文字留空就保留系统原本的农历/节气文字、只用颜色区分；颜色留空就只换文字、
  颜色保持系统原样。
- 其余日期保持系统原本的农历/节气显示，完全不动。

日历里没有可以放额外元素的容器（日期单元格只有"日期数字 + 农历文字"两个文本），而且系统会在切换
月份后异步重写农历文字，所以 mod 只用颜色和文字本身来表示放假/补班。

## 使用前提

mod 是往日期单元格里的**第二行文字**写的，也就是系统显示农历/节气的那一行，所以任务栏日历要打开
"其他日历"这一行：设置 → 时间和语言 → 日期和时间 → "在任务栏中显示其他日历"，选中"简体中文
（农历）"。把它关掉的话，日期格子里很可能就没有可写的那一行，日历上也就看不出放假/调休（真遇到
这种情况，日志里会说明）。

另外，"这一格是哪一天"是按格子代表的本地日期算出来的，跟格子布置在哪个时区无关，所以在东八区、
欧美时区都不会差一天。

## 数据来源

**这个 mod 默认不联网，也不内置任何节假日数据，不存在内置的接口地址。** 它只读你在设置里填的那个
地址；地址留空时，它一个请求也不发，日历保持系统原本的样子。

要让它显示节假日，请填一个 **iCalendar（`.ics`）订阅地址**。事件的 `SUMMARY` 里出现你规则里的词
（默认是"休"和"班"）才会被标记，形如：

```
BEGIN:VEVENT
DTSTART;VALUE=DATE:20260215
DTEND;VALUE=DATE:20260224
SUMMARY:春节（休）
END:VEVENT
```

一个可以直接用的示例地址是 `https://holiday.ailcc.com/api/holiday/ics`（一次返回多年数据，事件里
带（休）/（班）标记）。它只是示例：mod 不会自己去访问它，除非你把这个地址填进设置里；任何其他符合
上面格式的 ICS 订阅源也一样可用。

另一个可用的示例是苹果 iCloud 的中国大陆节假日日历
`https://p10-calendars.icloud.com/holiday/CN_zh.ics`：这个地址返回的响应是 gzip 压缩的，mod 会先
解压再解析。

解析规则：

- 只有 `SUMMARY` 里出现你规则里的词（默认是"休"和"班"）的条目才会被标记：匹配到的那条规则决定这一
  天的颜色和文字（默认放假日显示节日名、调休日显示"补班"）。一个词都没匹配上的条目（节气、传统
  节日、洋节等）不标记，所以它们不会被误当成放假；想让它们也算上，加一条兜底规则就行（词那一栏
  填 `*`）。日期用的是事件写的那一天：`DTSTART`/`DTEND` 带不带时间都可以，时间部分会被
  忽略（所以 `20260215`、`2026-02-15`、`20260215T000000Z` 都读作 2 月 15 日）。
- `DTEND` 是**不含**的那一天，和 RFC 5545 一致：上面这条表示 2 月 15 日到 23 日放假。没写 `DTEND`
  的事件按它开始的那一天处理；用 `DURATION`（如 `P3D`、`P1W`）写时长的事件按它算。
- 每个日期都会先检查是否真的存在（含闰年）：`20260231` 这种不存在的日期整条丢掉，而不是"顺手"挪到
  3 月 3 日；写了 `DTEND`/`DURATION` 却解析不出来、或结束日期不比开始日期晚的事件也会被丢掉
  （日志里有一条计数）。
- 只在填了地址之后才会去取数据，之后默认每 24 小时（可改）刷新一次；请求失败时保留上次的数据，
  10 分钟后重试。取不到数据（没填地址、没网、地址失效）时，日历保持系统原本的样子，不会变成一片
  空白，也不会报错刷屏。
- 响应如果是 gzip 或 zlib 压缩的，会先解压再解析（上面那个苹果地址就是这么返回的）。其他压缩方式
  （brotli、zstd 之类）读不了：遇到时日志里会说明这一点，而不是把二进制当成文本乱读。

从订阅源里读的东西：事件的 `DTSTART`/`DTEND`、用 `DURATION` 写的时长、`RDATE` 点名的那几天、
`EXDATE` 排除掉的那几天，以及"每年重复"这一种 `RRULE`（`FREQ=YEARLY`，可带 `COUNT`、`UNTIL`、
`INTERVAL`，最多再带 `BYMONTH`/`BYMONTHDAY` 且要和事件自己的日子一致）。**不做**的：其他形式的重复
规则（`FREQ=MONTHLY` 之类，或带 `BYDAY` 这种部件）按事件写的那一天读，日志里说明一次；时区换算
（事件写哪天就是哪天）；以及单元格里的角标。

## 高亮规则

"高亮规则"决定日历上标记哪些事件、怎么标记：**一项就是一类日子**，每项填三栏（词、颜色、文字），
想加一类就点 Add，不想要就删掉那一项。**留空的栏表示不动**，所以可以只改颜色、也可以只换文字。

    要找的词：休,放假        颜色：#4EA1FF        显示的文字：{name}

- **词**：在事件的 `SUMMARY` 里查找，多个用逗号隔开（`休,放假,holiday`），出现其中一个就生效。
  查找不看大小写，也不要求词前后有分隔符。填 `*` 是**兜底**：接收所有没被别的规则匹配到的事件；
  留空则这一项什么都不标记（刚加进来还没填的状态）。
- **颜色**：取色器选一个，或者手填 `#RRGGBB`（也认 `RRGGBB` 和 8 位 `AARRGGBB`）。**留空表示不动颜色**
  ——这时这一项只换文字。填错的规则会被丢掉，日志里点名。
- **文字**：**留空表示不动文字**——这时这一项只改颜色。`{name}` 是事件的 `SUMMARY` 去掉匹配到的词
  之后剩下的内容（那个词被一对括号单独括着时，括号也一起去掉）。

从上往下找，**第一条匹配上的生效**，所以把具体的那条放在前面。默认的两项是"休"/"放假"（蓝色、显示
节日名）和"班"/"补班"（红色、显示"补班"），和以前版本一样。

几个例子（每行都是往列表里加的一项）：

| 词 | 颜色 | 文字 | 效果 |
| --- | --- | --- | --- |
| `*` | `#4EA1FF` | `{name}` | 订阅源里的所有事件都显示事件名、蓝色（不需要任何标记） |
| `休,放假` | `#4EA1FF` | `{name}` | "休"或"放假"标到的日子显示事件名、蓝色 |
| `班,补班` | `#FF5A5A` | `补班` | "班"标到的日子显示"补班"、红色 |
| `祝日` | `#E81123` | （留空） | 日文订阅源里"祝日"的日子只改颜色、不换文字 |
| `Holiday` | （留空） | `休息` | 英文订阅源里 "Holiday" 的日子只换文字、颜色保持系统原样 |

三栏里"词"必填（或者写 `*` 兜底），颜色和文字至少要填一个：两个都留空的项不会做事。

mod 只给自己标过的格子改颜色，并且在第一次改色之前记下这个文本块原来的前景色（本地值），
需要还原时原样写回去；原本没有本地值就清掉，让样式和继承来的颜色重新生效。所以像"相邻月份
的日期被系统调暗"这种由系统自己设的本地颜色，不会被 mod 弄丢。

## 关于识别

日期单元格不是被"通知"来的：mod 在日历窗口自己的 UI 线程上直接读 XAML 树（`Window::Current()`
往下走），每一轮复查都重新找一遍，所以不注入任何东西，也用不到 XAML 诊断。

定位"农历文字"（Windows 11 里叫 `LunarTextBlock`）同样是每轮重新遍历单元格的视觉树：单元格一共
只有三四个子元素，走一遍很便宜，但可以保证不会往系统已经换掉的旧文本块里写字。日期数字是单元格
里唯一的纯数字文本，mod 永远不会动它。万一某个 Windows 版本改了模板结构，mod 会退回按字号自动识别，
并在日志里打印一次单元格结构（元素类型、名称、文字、字号）供排查。

mod 只替换农历文字的内容和颜色，不改动单元格的结构，也不会重复写入同一个单元格。系统在切换月份后
会异步重写农历文字，有时连文字块本身都会换掉，所以 mod 会在日历变化之后的几秒内逐秒复查单元格：
只有发现放假/补班的文字被系统覆盖时才会再写一次，其余单元格不做任何写入。复查由"日历窗口出现或
被显示、日期变化、文字变化、数据或设置变化"触发，并且在连续几秒没有任何写入之后自己停下来——
系统的日历即使关掉了也仍然被 shell 留着，定时器不能一直跑下去。

改规则不用重新下载：mod 会把最近一次读到的订阅源内容留着，规则一改就照着新规则把已经读到的数据重新
标一遍（日志里会记一条），只有换了订阅地址才会把旧数据丢掉。

## 致谢

日历窗口的定位方式来自 lonfro 的
[Agenda in Calendar View](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/agenda-in-calendar-view.wh.cpp)
（GPL-3.0），`RunFromWindowThread` 这类跨线程调用沿用 m417z 的
[Windows 11 Notification Center Styler](https://github.com/m417z/my-windhawk-mods)。

mod **不使用 XAML 诊断（`InitializeXamlDiagnosticsEx`）**，所以在同一个进程里可以和
Windows 11 Notification Center Styler、UWPSpy 这类同样用诊断的工具一起用。

---

# Chinese Holiday Calendar (English)

Shows Chinese statutory holidays and adjusted workdays inside the Windows 11
calendar flyout, the one that opens when you click the taskbar clock, using the
holiday data you point it at and the marking rules you write.

- **Days off**: the line under the day number is replaced with the holiday name
  from the data source (for example `国庆节`) and drawn in blue by default.
- **Adjusted workdays (调休)**: replaced with `补班` and drawn in red by default.
- **The rules are yours**: which words mark a day, in which colour and with which
  text is a list in the settings, one item per kind of day (see "Highlight rules").
  A feed of another country, whose events carry no 休/班 marker at all, is one item
  away: write `*` in its words.
- **A field left empty is left alone**: an empty text keeps the system's own lunar date
  or solar term and only changes the colour, an empty colour only changes the text.
- **Every other day**: left exactly as the system draws it.

## Requirements

The mod writes into the **second line of a day cell** - the lunar date or solar term the
system prints there - so the taskbar calendar has to show that line: Settings -> Time &
language -> Date & time -> "Show additional calendars in the taskbar", with Simplified
Chinese (Lunar) selected. With it switched off, a day cell may have no text to write into,
and nothing extra appears on the calendar (the log says so when that happens).

The date a cell stands for is worked out from the cell itself rather than from the local
time zone of the machine, so no holiday lands a day early or late west of UTC either.

## Why it works the way it does

A day cell of that calendar contains only two texts, the day number and one
lunar-text block, and there is no container to put an extra element in. Inserting
an element makes the shell rebuild the day cells over and over (measured: 470
insertions in 37 seconds, and an empty calendar afterwards), so the mod says
everything with the existing text and its colour. The shell also rewrites its own
lunar text asynchronously after a month change, which is why the mod re-checks
the visible cells once a second, for a few seconds after everything which can
start such a rewrite, and only writes when a cell no longer shows what it wants.
The re-check is started by the calendar window appearing or being shown, by the date
or the text of a cell changing, and by the data or the settings changing; it stops
itself after five passes without a write, because the shell keeps the day cells of a
closed flyout alive and a timer which only stopped when they went away would never
stop.

The colour of a day is written only into a cell the mod is marking, and the value
the cell had before the first write is remembered (the local value of
`Foreground`) so that it can be put back exactly; a cell which had no local value
gets the property cleared, so that its style and inherited colour apply again.
Clearing the property looks the same most of the time, but not always: a value
the shell sets on the element itself - it dims the days of the neighbouring
months that way - would be lost instead of restored. Whether a colour on the label
is the mod's own is decided by the brush object and not by the colour it holds, so
the mod never saves its own colour as an original one, and never takes away a
colour the shell has set since.

## Settings

| Setting | Meaning |
| --- | --- |
| Data source (ICS) | Any iCalendar (.ics) URL. Its events are marked by the highlight rules below. Empty by default, and then the mod fetches nothing at all and the calendar is left as it is. |
| Refresh interval (hours) | How long fetched data is reused (default 24 h). Only used while a data source is set. |
| Highlight rules | Which events are marked, in which colour and with which text: a list of items, one per kind of day, each with a Words, a Colour and a Text field (a field left empty is left alone). See "Highlight rules" below. |

## Data source

The mod ships with no holiday data and no built-in service address: it reads only
the address you enter, and with an empty address (the default) it makes no request
at all and the calendar stays exactly as Windows draws it.

Enter the address of an **iCalendar (`.ics`) feed**. An event is marked when one of the
words of a highlight rule appears in its `SUMMARY`; the rules mark 休 and 班 by default,
so a feed whose day-off and workday entries look like this is marked out of the box:

```
BEGIN:VEVENT
DTSTART;VALUE=DATE:20260215
DTEND;VALUE=DATE:20260224
SUMMARY:春节（休）
END:VEVENT
```

One feed which works this way is `https://holiday.ailcc.com/api/holiday/ics` (several
years at once, with the day-off and workday markers). It is an example: the mod does
not contact it - or anything else - unless you paste that address into the settings.
Any other feed in the format above works as well.

Another one is Apple's iCloud holiday calendar for the mainland,
`https://p10-calendars.icloud.com/holiday/CN_zh.ics`: it is answered
gzip-compressed whatever the request asks for, which the mod undoes before it reads
the body.

- Only events whose `SUMMARY` carries one of the words of a highlight rule are
  marked, and the rule which matched decides the colour and the text of that day
  (by default a day off shows the festival name and an adjusted workday `补班`).
  Entries which no rule matches - solar terms, traditional festivals, foreign
  holidays - are left alone, so they are never mistaken for a holiday; a
  catch-all rule (`*`, see "Highlight rules") is what marks those as well. The
  date an event names is used: a time part in `DTSTART`/`DTEND` is accepted and
  ignored, so `20260215`, `2026-02-15` and `20260215T000000Z` all mean 15 February.
- `DTEND` is exclusive, as in RFC 5545: the event above is 15 to 23 February. An
  event without a `DTEND` is the single day it starts on, and a `DURATION` (`P3D`,
  `P1W`) is read as the end it stands for.
- Every date is checked for being a date which exists, leap years included:
  `20260231` is dropped instead of being walked into 3 March, and an event whose
  `DTEND` or `DURATION` cannot be parsed, or which is not after its `DTSTART`, is
  dropped as well (a log line counts those).
- Data is fetched once an address is set, and reused for 24 hours (configurable); a
  failed request keeps the previous data and is retried after 10 minutes, and a
  feed may describe at most 20000 dates in total.
- A response which is compressed with gzip or zlib is decompressed before it is
  parsed. A body compressed in any other way (brotli and zstd are the others a
  server may pick) cannot be read: the log says so instead of the mod reading the
  bytes as text.

What is read of a feed: the `DTSTART`/`DTEND` of an event, a `DURATION` written instead of a
`DTEND`, the days an `RDATE` names, the days an `EXDATE` leaves out, and the plain yearly
`RRULE` - `FREQ=YEARLY`, with `COUNT`, `UNTIL`, `INTERVAL`, and at most `BYMONTH` and
`BYMONTHDAY` naming the day the event already starts on. Deliberately not expanded: any
other form of recurrence (`FREQ=MONTHLY` and the like, or a part such as `BYDAY`), which is
read as the single occurrence its event names, with a one-time log line saying so; a time
zone (the date an event names is the date used); and any badge or corner marker inside a
cell.

## Highlight rules

The "Highlight rules" setting says which events are marked on the calendar and how:
**one item is one kind of day**, with three fields each - the words, the colour and the
text. Add an item for another kind of day, remove the ones you do not want. **A field
which is empty is left alone**, so a rule can change the colour of a day, its text, or
both.

    Words: 休,放假        Colour: #4EA1FF        Text: {name}

- **Words**: looked for in the `SUMMARY` of an event, separated by commas
  (`休,放假,holiday`); one of them appearing is enough. The search ignores case, and
  needs no separators around the word. `*` is the **catch-all**: it takes every event no
  other rule matched. Empty words mark nothing - the state of an item which has just been
  added.
- **Colour**: pick one, or write `#RRGGBB` by hand (`RRGGBB` and the eight digit
  `AARRGGBB` are read as well). **Empty leaves the colour of the cell as it is**, so the
  rule only writes a text. A colour which does not parse drops the rule, and the log
  names it.
- **Text**: **empty keeps the lunar text**, so the rule only changes the colour.
  `{name}` is the `SUMMARY` of the event with the matching word taken out (and with the
  brackets around it, when it is written in brackets of its own).

The items are tried in the order they are in the list and the **first match wins**, so the
specific rule goes first. The two items the mod ships with mark 休/放假 blue with the name
of the holiday and 班/补班 red with 补班 - what the mod marked before.

A few examples, each of them one item of the list:

| Words | Colour | Text | What it does |
| --- | --- | --- | --- |
| `*` | `#4EA1FF` | `{name}` | every event of the feed shows its name, in blue (no marker needed) |
| `休,放假` | `#4EA1FF` | `{name}` | days marked 休 or 放假 show the name of the event, in blue |
| `班,补班` | `#FF5A5A` | `补班` | days marked 班 show `补班`, in red |
| `祝日` | `#E81123` | (empty) | 祝日 of a Japanese feed is coloured, and its own text is kept |
| `Holiday` | (empty) | `休息` | days of an English feed which say Holiday show `休息`, in the colour of the cell |

The words are what every rule needs (or `*`); of the colour and the text, at least one
has to be filled in - an item with neither of them does nothing.

Changing the rules does not download the feed again: the mod keeps the text of the feed it
read last and marks the data it already has again with the new rules (a log line says so).
Only another address throws the old data away.

## How it finds the calendar

Nothing reports a day cell to the mod: it reads the XAML tree of the calendar window on
that window's own UI thread (`Window::Current()` and down) and finds the day cells again
on every pass. Nothing is injected and no XAML diagnostics are involved.

Inside a day cell, the mod walks the three or four children on every pass, never writes
to the day number (it is the only all-digit text in a cell), prefers the block the
calendar names `LunarTextBlock`, falls back to the smallest text that has content, and
remembers the element it wrote to by interface pointer so that a block the shell has
already replaced is not written to any more.

## Credits

The way the calendar window is found follows lonfro's
[Agenda in Calendar View](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/agenda-in-calendar-view.wh.cpp)
(GPL-3.0); the cross-thread helpers such as `RunFromWindowThread` follow m417z's
[Windows 11 Notification Center Styler](https://github.com/m417z/my-windhawk-mods).

The mod does **not** use XAML Diagnostics (`InitializeXamlDiagnosticsEx`), so it can be
used together with Windows 11 Notification Center Styler and tools such as UWPSpy, which
use it themselves.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dataSourceUrl: ""
  $name: Data source (ICS, empty = no data)
  $name:zh-CN: 数据来源（ICS 地址，留空则不显示节假日）
  $description: >-
    An iCalendar (.ics) feed, for example
    https://p10-calendars.icloud.com/holiday/CN_zh.ics. The events of it which the
    highlight rules below match are marked; nothing else is read.
  $description:zh-CN: >-
    一个 iCalendar（.ics）订阅地址，例如 https://p10-calendars.icloud.com/holiday/CN_zh.ics。
    里面被下面"高亮规则"匹配到的事件会标在日历上；除了这个地址，mod 不访问任何网址。
- refreshIntervalHours: 24
  $name: Refresh interval (hours)
  $name:zh-CN: 刷新间隔（小时）
  $description: >-
    How long fetched data is reused before the source is asked again.
  $description:zh-CN: 这段时间内不重复下载。
- highlightRules:
  - - words: "休,放假"
      $name: Words to look for
      $name:zh-CN: 要找的词
      $description: >-
        Words looked for in the text of an event, separated by commas; a day is marked
        when one of them appears. "*" takes every event no other rule matched. An item
        with no words marks nothing.
      $description:zh-CN: >-
        在事件文字里查找的词，多个用逗号隔开，出现其中一个就按这条规则标记。填 "*" 表示兜底：
        所有没被别的规则匹配到的事件都归它。这一栏留空则这一项不标记任何日子（刚加进来还没填）。
    - color: "#4EA1FF"
      $name: Colour
      $name:zh-CN: 颜色
      $description: >-
        The colour of the text of a marked day (#RRGGBB). Empty leaves the colour as it
        is; a colour which is not a colour drops the rule, and the log names it.
      $description:zh-CN: >-
        被标记那一天的文字颜色（#RRGGBB）。留空表示不动颜色（只换文字）；填错的规则会被丢掉，
        日志里点名。
      #! $format: colorRgb
    - text: "{name}"
      $name: Text to show
      $name:zh-CN: 显示的文字
      $description: >-
        Replaces the lunar text. Leave empty for colour only; {name} is the event text
        with the matching word taken out.
      $description:zh-CN: >-
        替换农历文字。留空则只改颜色；{name} 是事件文字去掉匹配到的词之后剩下的内容。
  - - words: "班,补班"
      $name: Words to look for
      $name:zh-CN: 要找的词
    - color: "#FF5A5A"
      $name: Colour
      $name:zh-CN: 颜色
      #! $format: colorRgb
    - text: "补班"
      $name: Text to show
      $name:zh-CN: 显示的文字
  $name: Highlight rules
  $name:zh-CN: 高亮规则
  $description: >-
    One item per kind of day: words, colour, text. A field which is empty is left alone.
    The first rule which matches a day marks it; "*" is the catch-all. The two items
    above are the default.
  $description:zh-CN: >-
    一项一类日子：词、颜色、文字，留空的栏表示不动（留空颜色只换文字，留空文字只改颜色）。
    从上往下第一条匹配上的生效，"*" 是兜底。上面两项是默认值。
*/
// ==/WindhawkModSettings==

//
// The way the calendar window is found - hooking CreateWindowInBand(Ex), running on the
// window's own UI thread and walking the XAML tree from Window::Current() - follows the
// "Agenda in Calendar View" mod by lonfro, which is licensed under GPL-3.0:
//
//   Copyright (C) 2026 lonfro
//
// The helper which runs code on another window's thread is taken from the "Windows 11
// Notification Center Styler" mod by m417z, also GPL-3.0:
//
//   Copyright (C) 2026 m417z
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <atomic>
#include <cstdint>
#include <mutex>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>

thread_local bool g_initializedForThread;

// What this mod deliberately does not do: it is not a consumer of the XAML Diagnostics
// TAP (InitializeXamlDiagnosticsEx). Only one consumer of it can be active in a process
// at a time, so a mod which uses it stops working as soon as the next one is enabled -
// and the popular styling mods of this very process (Windows 11 Notification Center
// Styler, for one) are consumers. The calendar window is reached the way "Agenda in
// Calendar View" does it instead: CreateWindowInBand(Ex) and ShowWindow are hooked, the
// mod runs on the window's own UI thread, and the XAML tree is read from
// Window::Current() there (see CollectDayItemsOnCurrentThread).

////////////////////////////////////////////////////////////////////////////////

#pragma region winrt_hpp

#include <Unknwn.h>

#include <winrt/base.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

// forward declare namespaces we alias
namespace winrt {
    namespace Windows {
        namespace Foundation {}
        namespace UI::Xaml {}
    }
}

// alias some long namespaces for convenience
namespace wf = winrt::Windows::Foundation;
namespace wuc = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;

#pragma endregion  // winrt_hpp

// WindhawkUtils::StringSetting and WindhawkUtils::SetFunctionHook. The header ships with
// the compiler Windhawk builds mods with.
#include <windhawk_utils.h>

////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <cstdlib>
#include <cwctype>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

////////////////////////////////////////////////////////////////////////////////
// Settings

// One rule of the "highlight rules" setting: the words which mark an event, the colour
// the day is drawn in, and the text the cell shows. The settings UI has one item of the
// list per rule, with the three fields written below:
//
//     Words:  休,放假        Colour: #4EA1FF        Text: {name}
//
// A field which is left out asks for that part of the day to be left alone: a rule with
// no colour keeps the colour the shell gave the cell, and a rule with no text keeps the
// lunar text of it - so a rule can change the colour of a day, the text of a day, or
// both. What the fields are is in the WindhawkModSettings block above, which the two
// have to be changed together.
struct HighlightRule {
    std::vector<std::wstring> words;
    std::optional<winrt::Windows::UI::Color> color;
    std::wstring text;
};

// The snapshot of the settings. The defaults are written twice on purpose: here, for
// a setting which cannot be read, and in the WindhawkModSettings YAML, which is what
// the settings UI starts from, so the two have to be changed together. LoadSettings
// publishes a whole new snapshot as a shared_ptr to an immutable object, and every
// caller takes that pointer rather than a copy of the struct: a cell is looked at on
// every pass of the sweep, and copying a rule list for each of them is work with no
// purpose behind it.
struct Settings {
    // Empty by default on purpose: the mod then contacts nothing at all and the
    // calendar is left as the system drew it. The address is the user's to fill in.
    std::wstring dataSourceUrl;
    int refreshIntervalHours = 24;
    // The rule list as the setting is parsed into it; an empty list marks nothing.
    std::vector<HighlightRule> rules;
};

std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settings = std::make_shared<Settings>();

std::shared_ptr<const Settings> GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// Wh_GetStringSetting never answers null (a setting which cannot be read - and a field
// of a list item which is not there - comes back as an empty string, see windhawk_api.h),
// and the StringSetting wrapper frees the copy the engine hands out, so there is nothing
// to check here and nothing to free by hand. The index of a list setting is passed the
// way the engine expects it, as the argument of the "%d" of its name.
template <typename... Args>
std::wstring ReadStringSetting(PCWSTR name, Args... args) {
    const WindhawkUtils::StringSetting value =
        WindhawkUtils::StringSetting::make(name, args...);
    return std::wstring(value.get());
}

// Colour parsing, and why so many written forms are accepted: a colour of a rule is
// written as "RRGGBB", often with a '#' in front of it, or as eight digits (AARRGGBB) by
// someone used to CSS. Both lengths, with or without the '#', are accepted; a value
// which does not parse returns nullopt, and the caller drops the rule it belongs to
// rather than marking days in a colour nobody chose.
std::optional<winrt::Windows::UI::Color> ParseHexColor(std::wstring const& text) {
    std::wstring value;
    for (wchar_t c : text) {
        if (!iswspace(c)) {
            value += c;
        }
    }
    if (!value.empty() && value[0] == L'#') {
        value = value.substr(1);
    }
    if (value.size() != 6 && value.size() != 8) {
        return std::nullopt;
    }

    auto hexDigit = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') {
            return c - L'0';
        }
        if (c >= L'a' && c <= L'f') {
            return c - L'a' + 10;
        }
        if (c >= L'A' && c <= L'F') {
            return c - L'A' + 10;
        }
        return -1;
    };

    uint8_t bytes[4] = {0xFF, 0x00, 0x00, 0x00};
    size_t offset = value.size() == 8 ? 0 : 1;
    for (size_t i = 0; i + 1 < value.size(); i += 2) {
        int hi = hexDigit(value[i]);
        int lo = hexDigit(value[i + 1]);
        if (hi < 0 || lo < 0) {
            return std::nullopt;
        }
        bytes[offset + i / 2] = static_cast<uint8_t>((hi << 4) | lo);
    }

    return winrt::Windows::UI::Color{bytes[0], bytes[1], bytes[2], bytes[3]};
}

std::wstring TrimWhitespace(std::wstring const& text) {
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && iswspace(text[begin])) {
        begin++;
    }
    while (end > begin && iswspace(text[end - 1])) {
        end--;
    }
    return text.substr(begin, end - begin);
}

////////////////////////////////////////////////////////////////////////////////
// The highlight rules
//
// Which days the calendar marks, and how, is a list of rules in the settings, one item
// per rule, each with three fields - the words, the colour and the text:
//
//     Words:  休,放假
//     Colour: #4EA1FF
//     Text:   {name}
//
// A list setting has no length to ask for: a field of an item which is not there is
// answered with an empty string, which is what the walk below goes by. The reading is
// forgiving about what it finds - an item nobody has filled in yet is skipped, and a
// rule which cannot be understood is dropped and named in the log - while a rule which
// is quietly drawn in a colour nobody chose would be worse than one which does nothing.

// How many rules are read from the settings. Nothing needs this many kinds of day, and
// a list which is longer than this is cut off with a log line rather than walked for
// ever - the list itself has no end to find.
constexpr int kMaxHighlightRules = 32;

// The words of a rule, separated by commas - the full-width one as well, since a rule is
// as likely to be written with that one on a Chinese system. "*" is the catch-all: the
// rule which takes every event no other rule matched, and which is written that way
// because a rule with no words at all is one which marks nothing.
void SplitRuleWords(std::wstring const& text, std::vector<std::wstring>& words) {
    std::wstring current;

    for (size_t i = 0; i <= text.size(); i++) {
        wchar_t c = i < text.size() ? text[i] : L',';
        if (c != L',' && c != L'，') {
            current += c;
            continue;
        }
        std::wstring word = TrimWhitespace(current);
        current.clear();
        if (word.empty()) {
            continue;
        }
        if (word == L"*") {
            words.clear();
            return;
        }
        words.push_back(std::move(word));
    }
}

// The rule list of the settings. A rule whose colour cannot be read is dropped rather
// than guessed at, and the log names it: a list which marks days differently from what
// it says is the kind of thing nobody finds by looking. An item which does not describe
// anything yet - no words, or neither a colour nor a text - is skipped, which is also
// the state of one the user is in the middle of filling in.
std::vector<HighlightRule> LoadHighlightRules() {
    std::vector<HighlightRule> rules;
    std::wstring firstNoColour;
    size_t droppedColour = 0;
    size_t droppedWords = 0;

    for (int i = 0; i < kMaxHighlightRules; i++) {
        const std::wstring words =
            TrimWhitespace(ReadStringSetting(L"highlightRules[%d].words", i));
        const std::wstring colorText =
            TrimWhitespace(ReadStringSetting(L"highlightRules[%d].color", i));
        const std::wstring text =
            TrimWhitespace(ReadStringSetting(L"highlightRules[%d].text", i));

        // An item which is not written at all is skipped: that is the state of an item
        // the user has just added, and of every index past the end of the list.
        if (words.empty() && colorText.empty() && text.empty()) {
            continue;
        }

        // A rule without words marks nothing - "*" is what a rule which takes the
        // events no other rule matched is written with - so it is left out as well.
        if (words.empty()) {
            droppedWords++;
            continue;
        }

        HighlightRule rule;
        SplitRuleWords(words, rule.words);
        rule.text = text;

        // No colour is not a mistake: it asks for the colour the shell gave the cell to
        // be left as it is, which is what a rule which only writes a text wants.
        if (!colorText.empty()) {
            rule.color = ParseHexColor(colorText);
        }
        if (!colorText.empty() && !rule.color) {
            droppedColour++;
            if (firstNoColour.empty()) {
                firstNoColour = words;
            }
            continue;
        }

        // Nothing would change on the day this rule matches: the item is half written,
        // or it is a rule which is only there for the words it looks for, which is
        // nothing a calendar can show.
        if (!rule.color && rule.text.empty()) {
            continue;
        }

        rules.push_back(std::move(rule));
    }

    // The end of the list is not written anywhere, so this is where the walk above
    // stops; a rule on the far side of the limit is worth a word, since its days would
    // otherwise be marked by nothing at all.
    const std::wstring beyond = TrimWhitespace(
        ReadStringSetting(L"highlightRules[%d].words", kMaxHighlightRules));
    if (!beyond.empty()) {
        Wh_Log(L"More than %d highlight rules are set; the rest is ignored",
               kMaxHighlightRules);
    }

    if (droppedColour) {
        Wh_Log(L"%zu highlight rule(s) are dropped: the colour is not a colour (the "
               L"rule of \"%s\")",
               droppedColour, firstNoColour.c_str());
    }
    if (droppedWords) {
        Wh_Log(L"%zu highlight rule(s) have no words and are ignored - the rule which "
               L"takes what the others did not is written \"*\"",
               droppedWords);
    }

    return rules;
}

// Whether two rule lists are the same. This is what tells a settings change which
// changes the marking from one which only changes something else (the refresh interval,
// say): the data which is already loaded has to be marked again in the first case, and
// only drawn again in the second.
bool SameHighlightRules(std::vector<HighlightRule> const& left,
                        std::vector<HighlightRule> const& right) {
    auto sameColor = [](std::optional<winrt::Windows::UI::Color> const& a,
                        std::optional<winrt::Windows::UI::Color> const& b) {
        if (a.has_value() != b.has_value()) {
            return false;
        }
        return !a || (a->A == b->A && a->R == b->R && a->G == b->G && a->B == b->B);
    };

    if (left.size() != right.size()) {
        return false;
    }
    for (size_t i = 0; i < left.size(); i++) {
        if (left[i].words != right[i].words || left[i].text != right[i].text ||
            !sameColor(left[i].color, right[i].color)) {
            return false;
        }
    }
    return true;
}

// A plain search which ignores case. Which letters are the same letter in which case is
// Windows' answer (CompareStringOrdinal) and not the C library's, which knows only ASCII
// in the locale a process starts in. The exact search is tried first: that is what every
// match of a Chinese feed is, and it keeps the cost of a parse down.
size_t FindWord(std::wstring const& text, std::wstring const& word) {
    if (word.empty() || word.size() > text.size()) {
        return std::wstring::npos;
    }

    size_t at = text.find(word);
    if (at != std::wstring::npos) {
        return at;
    }

    for (size_t i = 0; i + word.size() <= text.size(); i++) {
        if (CompareStringOrdinal(text.c_str() + i, (int)word.size(), word.c_str(),
                                 (int)word.size(), TRUE) == CSTR_EQUAL) {
            return i;
        }
    }
    return std::wstring::npos;
}

// The rule which marks this text, and the word of it which was found. The rules are
// tried in the order they are written and the first one with a word which appears in the
// text wins, so the specific rule goes above the general one. A rule without words - the
// catch-all - is used only when no rule with a word matched, wherever it stands in the
// list: it is the "everything else" rule, and a list which writes it first means what a
// list which writes it last means.
const HighlightRule* MatchHighlightRule(std::wstring const& text,
                                        std::vector<HighlightRule> const& rules,
                                        size_t& position,
                                        size_t& length) {
    position = 0;
    length = 0;

    for (const auto& rule : rules) {
        size_t best = std::wstring::npos;
        size_t bestLength = 0;

        for (const auto& word : rule.words) {
            size_t at = FindWord(text, word);
            if (at == std::wstring::npos) {
                continue;
            }
            // The word which starts leftmost names the day: the name of an event is
            // what is left of its text once that word is taken out, and "中秋国庆"
            // should not lose its tail to a rule which looks for "国庆" as well. Two
            // words which start at the same place are settled by the longer one.
            if (best == std::wstring::npos || at < best ||
                (at == best && word.size() > bestLength)) {
                best = at;
                bestLength = word.size();
            }
        }

        if (best != std::wstring::npos) {
            position = best;
            length = bestLength;
            return &rule;
        }
    }

    for (const auto& rule : rules) {
        if (rule.words.empty()) {
            return &rule;
        }
    }
    return nullptr;
}

// The name of an event: its text without the word which marked it. A rule whose words
// are all the event says leaves an empty name, which is nothing a cell can show; the
// caller writes nothing then and the day is marked by colour alone.
//
// A word which sits in a pair of brackets takes the brackets with it when they hold
// nothing else ("国庆节（休）" -> "国庆节"): the brackets are there to set the marker
// apart from the name, and taking only the word out would leave an empty pair behind,
// which is not a name either. A bracketed marker is how a feed writes it in any
// language, so this is not a rule about Chinese: "Holiday (observed)" loses the
// brackets the same way.
std::wstring EventNameWithoutWord(std::wstring const& text,
                                  size_t position,
                                  size_t length) {
    size_t begin = position;
    size_t end = position + length;
    if (length && begin > 0 && end < text.size()) {
        const wchar_t before = text[begin - 1];
        const wchar_t after = text[end];
        const bool wrapped =
            (before == L'（' && after == L'）') ||
            (before == L'(' && after == L')') ||
            (before == L'【' && after == L'】') ||
            (before == L'[' && after == L']') ||
            (before == L'{' && after == L'}') ||
            (before == L'〔' && after == L'〕');
        if (wrapped) {
            begin--;
            end++;
        }
    }

    std::wstring name = text;
    if (end > begin && begin <= name.size() && end <= name.size()) {
        name.erase(begin, end - begin);
    }
    return TrimWhitespace(name);
}

void LoadSettings() {
    auto settings = std::make_shared<Settings>();

    settings->dataSourceUrl = ReadStringSetting(L"dataSourceUrl");
    settings->refreshIntervalHours = Wh_GetIntSetting(L"refreshIntervalHours");
    settings->rules = LoadHighlightRules();

    if (settings->refreshIntervalHours < 1) {
        settings->refreshIntervalHours = 1;
    }
    // An empty address is a valid setting, not a missing one: it means "no data
    // source", which is what the mod ships with. An empty rule list is a valid setting
    // as well - it means "mark nothing" - so no rules are invented here; a word in the
    // log is what the user of a calendar which stays empty gets, since the list is the
    // one thing the marking needs.
    if (settings->rules.empty() && !settings->dataSourceUrl.empty()) {
        Wh_Log(L"No highlight rule is set, so no date is marked; add one in the "
               L"settings of this mod (the words of an event, a colour and a text)");
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = settings;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Small helpers

std::wstring Utf8ToWide(const char* data, size_t length) {
    if (!data || !length) {
        return {};
    }
    int size =
        MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(length), nullptr, 0);
    if (size <= 0) {
        return {};
    }
    std::wstring result(static_cast<size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(length), result.data(),
                        size);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Holiday data

// What the calendar shows for one date: the name of the event which covers it, with the
// word which marked it taken out, and the text and the colour of the rule which marked
// it. Which rule that is gets decided when the feed is read - see
// ClassifyEventSummary - so that a day cell is drawn from this alone. A colour which is
// not there is the rule asking for the colour of the cell to be left alone.
struct HolidayInfo {
    std::wstring name;
    std::wstring text;
    std::optional<winrt::Windows::UI::Color> color;
};

using HolidayMap = std::unordered_map<int32_t, HolidayInfo>;

// A date is an int32 "yyyymmdd", so that integer comparison is date comparison and the
// same value serves both as a map key and as a range endpoint (DTSTART < key < DTEND).
// The price is that the value is not continuous - 20260228 + 1 is not 20260301 - so
// every day-by-day step has to go through NextDateKey() and never through arithmetic.
int32_t MakeDateKey(int year, int month, int day) {
    return year * 10000 + month * 100 + day;
}

// The number of days of a month, or 0 when "month" is not a month. The leap rule
// lives here so that the expansion (NextDateKey) and the validation (IsValidDate)
// can never disagree about the 29th of February.
int DaysInMonth(int year, int month) {
    if (month < 1 || month > 12) {
        return 0;
    }
    static const int kDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) {
        return 29;
    }
    return kDays[month - 1];
}

// Whether a year/month/day triple is a date which exists. Every date the parser
// accepts goes through this, because a date which does not exist is not harmless
// here: a range is walked with NextDateKey(), which turns the "+1 day" of
// 2026-02-31 into 2026-03-03, so a typo in a feed would mark a real date which
// the feed never named. The year range keeps MakeDateKey() (year*10000+month*100
// +day) inside an int32 as well, and turns away a "19700101" placeholder.
bool IsValidDate(int year, int month, int day) {
    if (year < 1900 || year > 9999) {
        return false;
    }
    return day >= 1 && day <= DaysInMonth(year, month);
}

// Accepts "2026-01-01", "2026/1/1", "20260101" and "20260101T000000Z". A time
// which follows a date is a part of its own, and the date it belongs to is used
// as it is: the holidays of this source are whole days ("VALUE=DATE"), and a
// source which sends midnight UTC means the same day here.
int32_t ParseDateKey(std::wstring const& text) {
    std::vector<int> parts;
    std::wstring current;
    for (wchar_t c : text) {
        if (c >= L'0' && c <= L'9') {
            current += c;
        } else if (!current.empty()) {
            parts.push_back((int)wcstol(current.c_str(), nullptr, 10));
            current.clear();
        }
    }
    if (!current.empty()) {
        parts.push_back((int)wcstol(current.c_str(), nullptr, 10));
    }

    // The compact form is recognised by "the first part is eight digits" and not
    // by "there is exactly one part": "20260101T000000Z" comes out of the split
    // above as two parts. A run of digits too long for an int32 is returned as
    // LONG_MAX by wcstol and is then turned away by the year check below.
    int year = 0;
    int month = 0;
    int day = 0;
    if (!parts.empty() && parts[0] >= 10000101) {
        int value = parts[0];
        year = value / 10000;
        month = (value / 100) % 100;
        day = value % 100;
    } else if (parts.size() >= 3) {
        year = parts[0];
        month = parts[1];
        day = parts[2];
    } else {
        return 0;
    }

    if (!IsValidDate(year, month, day)) {
        return 0;
    }
    return MakeDateKey(year, month, day);
}

// The next day. The leap rule lives in DaysInMonth, so a 2024-02-29 in the feed is
// stepped over correctly.
int32_t NextDateKey(int32_t key) {
    int year = key / 10000;
    int month = (key / 100) % 100;
    int day = key % 100;

    int days = DaysInMonth(year, month);
    if (days == 0) {
        return 0;
    }

    day++;
    if (day > days) {
        day = 1;
        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }

    return MakeDateKey(year, month, day);
}

std::mutex g_holidaysMutex;
HolidayMap g_holidays;
// The body of the last feed which was read. It is kept so that a rule list which is
// changed afterwards can be put on the data which is already loaded, without asking the
// source for the same feed a second time (see RemarkStoredHolidays); changing the
// address throws it away along with the data (see ResetHolidayData).
std::wstring g_holidaysBody;

bool LookupHoliday(int32_t key, HolidayInfo& out) {
    std::lock_guard<std::mutex> lock(g_holidaysMutex);
    auto it = g_holidays.find(key);
    if (it == g_holidays.end()) {
        return false;
    }
    out = it->second;
    return true;
}

// The feed covers every year at once, so the data replaces the old data. The body is
// kept with it: see the comment above g_holidaysBody.
void StoreHolidays(std::wstring const& body, HolidayMap parsed) {
    std::lock_guard<std::mutex> lock(g_holidaysMutex);
    g_holidays = std::move(parsed);
    g_holidaysBody = body;
}

////////////////////////////////////////////////////////////////////////////////
// iCalendar parsing
//
// The default data source is an .ics feed with one VEVENT per holiday period:
//
//   BEGIN:VEVENT
//   DTSTART;VALUE=DATE:20260215
//   DTEND;VALUE=DATE:20260224
//   SUMMARY:春节（休）
//   END:VEVENT
//
// DTEND is exclusive, as RFC 5545 requires.

std::vector<std::wstring> SplitIcsLines(std::wstring const& text) {
    std::vector<std::wstring> lines;
    std::wstring current;

    auto flush = [&]() {
        if (current.empty()) {
            return;
        }
        // A line starting with a space or a tab continues the previous one.
        if ((current[0] == L' ' || current[0] == L'\t') && !lines.empty()) {
            lines.back() += current.substr(1);
        } else {
            lines.push_back(current);
        }
        current.clear();
    };

    for (size_t i = 0; i < text.size(); i++) {
        wchar_t c = text[i];
        if (c == L'\r' || c == L'\n') {
            flush();
            if (c == L'\r' && i + 1 < text.size() && text[i + 1] == L'\n') {
                i++;
            }
        } else {
            current += c;
        }
    }
    flush();

    return lines;
}

std::wstring UnescapeIcsText(std::wstring const& text) {
    std::wstring result;
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] != L'\\' || i + 1 >= text.size()) {
            result += text[i];
            continue;
        }
        switch (text[++i]) {
            case L'n':
            case L'N':
                result += L' ';
                break;
            case L'\\':
                result += L'\\';
                break;
            case L';':
                result += L';';
                break;
            case L',':
                result += L',';
                break;
            default:
                result += text[i];
                break;
        }
    }
    return result;
}

// Whether an event is marked at all, and how: the rule which matched decides the colour
// and the text of the day, and the name is what is left of the text of the event once
// the word which matched is taken out.
//
// "元旦节（休）" with the rules the mod ships with is one such event: the rule of 休
// matches, so the day is drawn in its blue and the cell shows "元旦节". An event which
// no rule matches - a solar term, a western holiday, anything the rules were not
// written for - is left out of the data, so that it can never be taken for a holiday.
bool ClassifyEventSummary(std::wstring const& summary,
                          std::vector<HighlightRule> const& rules,
                          HolidayInfo& out) {
    std::wstring text = UnescapeIcsText(summary);

    size_t position = 0;
    size_t length = 0;
    const HighlightRule* rule = MatchHighlightRule(text, rules, position, length);
    if (!rule) {
        return false;
    }

    out.name = EventNameWithoutWord(text, position, length);
    out.text = rule->text;
    out.color = rule->color;
    return true;
}

// Reported once per process: a recurrence rule of a form the mod does not expand would
// show too few dates, and the log line says why.
std::atomic<bool> g_loggedRecurrence{false};

// The most dates a feed may describe. The address of the data source can be
// changed, and an event written as "from 1900 to 2100" would otherwise keep the
// worker thread busy for tens of thousands of dates, which would also hold up
// the next refresh and the shutdown.
constexpr size_t kMaxHolidayDates = 20000;

// Whether a text is a word of the feed, ignoring case: the parts of a rule are written in
// capitals by convention ("FREQ=YEARLY") and the mod does not care either way.
bool SameWord(std::wstring const& text, PCWSTR word) {
    const int length = static_cast<int>(wcslen(word));
    return static_cast<int>(text.size()) == length &&
           CompareStringOrdinal(text.c_str(), length, word, length, TRUE) == CSTR_EQUAL;
}

// Whether a number written in the feed reads as one, and is inside a range.
bool ReadNumber(std::wstring const& text, int smallest, int largest, int& value) {
    if (text.empty() || text.size() > 8) {
        return false;
    }
    int number = 0;
    for (wchar_t c : text) {
        if (c < L'0' || c > L'9') {
            return false;
        }
        number = number * 10 + (c - L'0');
    }
    if (number < smallest || number > largest) {
        return false;
    }
    value = number;
    return true;
}

// The number of days a DURATION names, for an event which writes its end that way instead
// of with a DTEND. The time part of a duration is below a day and is not counted: the mod
// reads dates and never looks at a time, so "PT12H" is the day the event starts on. False
// is a value the mod cannot read, and the event is dropped then rather than given an end
// nobody wrote.
bool ParseDurationDays(std::wstring const& text, int& days) {
    size_t i = 0;
    if (i < text.size() && (text[i] == L'+' || text[i] == L'-')) {
        if (text[i] == L'-') {
            return false;  // an end before the start is not a range
        }
        i++;
    }
    if (i >= text.size() || text[i] != L'P') {
        return false;
    }
    i++;

    int total = 0;
    bool inTimePart = false;
    bool sawSomething = false;
    while (i < text.size()) {
        if (text[i] == L'T') {
            inTimePart = true;
            i++;
            continue;
        }
        if (!iswdigit(text[i])) {
            return false;
        }
        int number = 0;
        while (i < text.size() && iswdigit(text[i])) {
            number = number * 10 + (text[i] - L'0');
            i++;
        }
        if (i >= text.size() || number > 10000) {
            return false;  // a number which is not followed by a part of its own
        }
        const wchar_t part = text[i++];
        if (part == L'W' && !inTimePart) {
            total += number * 7;
        } else if (part == L'D' && !inTimePart) {
            total += number;
        } else if ((part == L'H' || part == L'M' || part == L'S') && inTimePart) {
            // Below a day: the mod reads dates.
        } else {
            return false;
        }
        sawSomething = true;
    }

    days = total;
    return sawSomething;
}

// What an RRULE asks for. The mod expands the plain yearly form - a day of the year which
// comes round every year, or every N years - because that is what a calendar of fixed
// holidays is made of, and because it is the one form a date map can hold. Any other form
// is marked "not simple" here and read as the single occurrence its event names, with the
// log line saying so; pretending to expand one would be worse than saying that.
struct YearlyRule {
    bool asked = false;
    bool simple = true;
    int intervalYears = 1;
    int count = 0;      // 0: no COUNT was written
    int32_t until = 0;  // 0: no UNTIL was written
    int byMonth = 0;    // 0: no BYMONTH was written
    int byMonthDay = 0;
};

YearlyRule ParseYearlyRule(std::wstring const& value) {
    YearlyRule rule;
    rule.asked = true;

    bool sawFrequency = false;
    size_t pos = 0;
    while (pos < value.size()) {
        const size_t next = value.find(L';', pos);
        const std::wstring part =
            value.substr(pos, next == std::wstring::npos ? std::wstring::npos : next - pos);
        pos = next == std::wstring::npos ? value.size() : next + 1;
        if (part.empty()) {
            continue;
        }

        const size_t equals = part.find(L'=');
        if (equals == std::wstring::npos) {
            rule.simple = false;
            continue;
        }
        const std::wstring name = part.substr(0, equals);
        const std::wstring partValue = part.substr(equals + 1);

        if (SameWord(name, L"FREQ")) {
            sawFrequency = true;
            if (!SameWord(partValue, L"YEARLY")) {
                rule.simple = false;
            }
        } else if (SameWord(name, L"INTERVAL")) {
            if (!ReadNumber(partValue, 1, 1000, rule.intervalYears)) {
                rule.simple = false;
            }
        } else if (SameWord(name, L"COUNT")) {
            if (!ReadNumber(partValue, 1, 100000, rule.count)) {
                rule.simple = false;
            }
        } else if (SameWord(name, L"UNTIL")) {
            rule.until = ParseDateKey(partValue);
            if (!rule.until) {
                rule.simple = false;
            }
        } else if (SameWord(name, L"BYMONTH")) {
            if (!ReadNumber(partValue, 1, 12, rule.byMonth)) {
                rule.simple = false;
            }
        } else if (SameWord(name, L"BYMONTHDAY")) {
            if (!ReadNumber(partValue, 1, 31, rule.byMonthDay)) {
                rule.simple = false;
            }
        } else {
            // BYDAY, BYWEEKNO, BYSETPOS, WKST and everything else: forms whose expansion
            // is a calendar of its own, and which the mod does not pretend to read.
            rule.simple = false;
        }
    }

    if (!sawFrequency) {
        rule.simple = false;
    }
    return rule;
}

// The dates a comma-separated value names, as RDATE and EXDATE write them. A value which
// does not read as a date is skipped: a feed is not turned away for one field the mod
// does not use anyway.
void AppendDateKeys(std::wstring const& value, std::vector<int32_t>& keys) {
    size_t pos = 0;
    while (pos <= value.size()) {
        const size_t comma = value.find(L',', pos);
        const std::wstring part =
            value.substr(pos, comma == std::wstring::npos ? std::wstring::npos : comma - pos);
        const int32_t key = ParseDateKey(part);
        if (key) {
            keys.push_back(key);
        }
        if (comma == std::wstring::npos) {
            break;
        }
        pos = comma + 1;
    }
}

bool ParseIcs(std::wstring const& text,
              std::vector<HighlightRule> const& rules,
              HolidayMap& out) {
    auto lines = SplitIcsLines(text);

    // One VEVENT as the feed wrote it. The events are collected before a date is drawn
    // from any of them, because a yearly rule which names no end is expanded over the
    // years the feed itself is about, and not over years of the mod's own choosing.
    struct Event {
        int32_t start = 0;
        int32_t end = 0;
        bool hasEnd = false;
        int durationDays = 0;
        // A field the mod could not read as what it says it is. No end is guessed for
        // such an event, so it is dropped rather than given one.
        bool broken = false;
        std::wstring summary;
        std::vector<int32_t> ownDates;       // RDATE
        std::vector<int32_t> excludedDates;  // EXDATE
        YearlyRule yearly;
    };

    std::vector<Event> events;
    bool inEvent = false;
    bool hitDateLimit = false;
    size_t droppedRanges = 0;
    Event event;

    auto flushEvent = [&]() {
        if (event.broken) {
            droppedRanges++;
        } else if (event.start) {
            events.push_back(std::move(event));
        }
    };

    for (const auto& line : lines) {
        if (line == L"BEGIN:VEVENT") {
            inEvent = true;
            event = Event{};
            continue;
        }
        if (line == L"END:VEVENT") {
            if (inEvent) {
                flushEvent();
            }
            inEvent = false;
            continue;
        }
        if (!inEvent) {
            continue;
        }

        size_t colon = line.find(L':');
        if (colon == std::wstring::npos) {
            continue;
        }

        std::wstring name = line.substr(0, colon);
        std::wstring value = line.substr(colon + 1);
        size_t semicolon = name.find(L';');
        if (semicolon != std::wstring::npos) {
            name = name.substr(0, semicolon);
        }

        if (name == L"DTSTART") {
            event.start = ParseDateKey(value);
        } else if (name == L"DTEND") {
            event.end = ParseDateKey(value);
            event.hasEnd = true;
        } else if (name == L"DURATION") {
            // An event may write how long it lasts instead of when it ends. A duration
            // which cannot be read leaves the event with no end the mod knows, and an end
            // nobody wrote is not guessed at.
            if (!ParseDurationDays(value, event.durationDays)) {
                event.broken = true;
            }
        } else if (name == L"SUMMARY") {
            event.summary = value;
        } else if (name == L"RDATE") {
            AppendDateKeys(value, event.ownDates);
        } else if (name == L"EXDATE") {
            AppendDateKeys(value, event.excludedDates);
        } else if (name == L"RRULE") {
            event.yearly = ParseYearlyRule(value);
        }
        // RECURRENCE-ID and the rest of the fields of a feed are not read: an override of
        // one occurrence carries the DTSTART of that occurrence, which is the day it
        // stands for.
    }

    // The years the feed is about, which is what a yearly rule with no end of its own is
    // expanded over: the mod neither invents a horizon of its own nor a date outside the
    // span of the feed it was given.
    int firstYear = 0;
    int lastYear = 0;
    auto noteYear = [&](int32_t key) {
        if (!key) {
            return;
        }
        const int year = key / 10000;
        if (!firstYear || year < firstYear) {
            firstYear = year;
        }
        if (year > lastYear) {
            lastYear = year;
        }
    };
    for (const auto& collected : events) {
        noteYear(collected.start);
        noteYear(collected.end);
        for (int32_t key : collected.ownDates) {
            noteYear(key);
        }
        for (int32_t key : collected.excludedDates) {
            noteYear(key);
        }
    }

    bool sawUnexpandedRecurrence = false;
    size_t added = 0;

    for (const auto& collected : events) {
        if (added >= kMaxHolidayDates) {
            // An earlier event already filled the feed: this one is dropped.
            hitDateLimit = true;
            break;
        }

        HolidayInfo info;
        if (!ClassifyEventSummary(collected.summary, rules, info)) {
            continue;
        }

        // Where the event ends, exclusive like a DTEND: the DTEND when it wrote one, the
        // day its DURATION reaches to when it wrote that instead, and the single day it
        // starts on when it wrote neither.
        int32_t end = 0;
        if (collected.hasEnd) {
            if (collected.end <= collected.start) {
                // A DTEND which is there but does not make a range: it did not parse, or
                // it is not later than DTSTART. The event is dropped instead of being read
                // as a single day, because a wrong end would put the holiday name on dates
                // the feed never named.
                droppedRanges++;
                continue;
            }
            end = collected.end;
        } else if (collected.durationDays > 0) {
            end = collected.start;
            for (int day = 0; day < collected.durationDays && end; day++) {
                end = NextDateKey(end);
            }
            if (!end) {
                droppedRanges++;
                continue;
            }
        } else {
            end = NextDateKey(collected.start);
        }

        auto isExcluded = [&collected](int32_t key) {
            for (int32_t excluded : collected.excludedDates) {
                if (excluded == key) {
                    return true;
                }
            }
            return false;
        };
        auto addDate = [&](int32_t key) {
            if (!key || isExcluded(key)) {
                return;
            }
            out[key] = info;
            added++;
        };

        for (int32_t key = collected.start; key && key < end; key = NextDateKey(key)) {
            // The limit is looked at inside the loop as well: a single event which spans
            // millions of days is what a limit at the start of the event does not catch.
            if (added >= kMaxHolidayDates) {
                hitDateLimit = true;
                break;
            }
            addDate(key);
        }

        // The dates the event names of their own, which a feed writes as an RDATE: one
        // event may be a list of days rather than a period, and they may sit outside it.
        for (int32_t key : collected.ownDates) {
            if (added >= kMaxHolidayDates) {
                hitDateLimit = true;
                break;
            }
            addDate(key);
        }

        // A yearly rule: the same day of the same month, the next year and the year after
        // that, which is the form a calendar of fixed holidays is written in and the only
        // one a map of dates can hold. BYMONTH and BYMONTHDAY of such a rule name the day
        // the event already starts on; a rule which names another day, or which carries a
        // part such as BYDAY, is left to the log line below and read as the occurrence its
        // event names.
        const int startMonth = (collected.start / 100) % 100;
        const int startDay = collected.start % 100;
        if (collected.yearly.asked) {
            const bool plainYearly =
                collected.yearly.simple &&
                (collected.yearly.byMonth == 0 ||
                 collected.yearly.byMonth == startMonth) &&
                (collected.yearly.byMonthDay == 0 ||
                 collected.yearly.byMonthDay == startDay);
            if (!plainYearly) {
                sawUnexpandedRecurrence = true;
            } else {
                const int startYear = collected.start / 10000;
                // A rule which names an end of its own - a COUNT or an UNTIL - is expanded
                // to that end; one which names neither is expanded over the years the feed
                // itself covers, so that a date outside the feed is never invented.
                const int lastYearToUse =
                    (collected.yearly.count || collected.yearly.until) ? 9999 : lastYear;
                int marked = 1;  // the day of the event itself
                for (int year = startYear + collected.yearly.intervalYears;
                     year <= lastYearToUse;
                     year += collected.yearly.intervalYears) {
                    if (collected.yearly.count && marked >= collected.yearly.count) {
                        break;
                    }
                    if (added >= kMaxHolidayDates) {
                        hitDateLimit = true;
                        break;
                    }
                    // A year which has no such day is left out, which is what a rule of the
                    // 29th of February runs into.
                    if (!IsValidDate(year, startMonth, startDay)) {
                        continue;
                    }
                    const int32_t key = MakeDateKey(year, startMonth, startDay);
                    if (collected.yearly.until && key > collected.yearly.until) {
                        break;
                    }
                    addDate(key);
                    marked++;
                }
            }
        }
    }

    if (sawUnexpandedRecurrence && !g_loggedRecurrence.exchange(true)) {
        Wh_Log(L"A recurrence rule of the data source is not a plain yearly one; such an "
               L"event is read as the one occurrence it names");
    }

    if (hitDateLimit) {
        Wh_Log(L"The data source describes more than %zu holiday dates; the "
               L"rest is ignored",
               kMaxHolidayDates);
    }

    if (droppedRanges) {
        Wh_Log(L"%zu events are dropped: their DTEND or DURATION does not read as one, "
               L"or their DTEND does not come after their DTSTART",
               droppedRanges);
    }

    return true;
}


// The data source is an iCalendar feed: the only format the mod reads. False means the
// body is not a feed at all; a feed which marks nothing - no rule matched, or the feed
// has no event - reads fine and leaves "out" empty, which the caller reports.
bool ParseHolidayData(std::wstring const& body,
                      std::vector<HighlightRule> const& rules,
                      HolidayMap& out) {
    if (body.find(L"BEGIN:VEVENT") == std::wstring::npos &&
        body.find(L"BEGIN:VCALENDAR") == std::wstring::npos) {
        Wh_Log(L"The data source did not return an iCalendar feed");
        // A body which is still binary after the compression the mod knows about
        // has been taken care of is compressed with something else, or it is not
        // a calendar at all. The line above is what a user notices; this one says
        // which of the two it is.
        if (body.find(L'\0') != std::wstring::npos ||
            body.find(L'\xfffd') != std::wstring::npos) {
            Wh_Log(L"The body does not look like text: it may be compressed in "
                   L"a way the mod does not decode");
        }
        return false;
    }

    return ParseIcs(body, rules, out);
}

////////////////////////////////////////////////////////////////////////////////
// Decompressing the response
//
// Wh_GetUrlContent hands the body back exactly as it arrived on the wire, and a
// feed may arrive compressed: the iCloud holiday feed, for one, answers with gzip
// whatever the request asks for, and the parser can only read text. A body which
// is gzip (RFC 1952) or zlib (RFC 1950) is therefore expanded here first.
//
// Both wrappers carry a raw deflate stream (RFC 1951), which is what is decoded
// below. A stream which is damaged, or compressed in a way this does not know, is
// reported as a failure instead of being passed on as half a feed: half a feed
// would quietly mark the wrong days.

// The most a body may decode to. The feed itself is capped at kMaxHolidayDates as
// well, so this only has to keep a small response which expands into gigabytes
// from exhausting the taskbar's memory.
constexpr size_t kMaxDecodedBodyBytes = 4 * 1024 * 1024;

// A gzip stream starts with 0x1f 0x8b, which no text starts with, so these two
// bytes settle the question on their own.
bool IsGzipBody(const char* data, size_t length) {
    return length >= 2 && (unsigned char)data[0] == 0x1f &&
           (unsigned char)data[1] == 0x8b;
}

// A zlib stream starts with CMF and FLG: compression method 8 (deflate), a header
// whose two bytes are a multiple of 31, and - the last check here - no preset
// dictionary, which the mod has no way to supply. Text can in principle start with
// two such bytes, which is why the caller only believes this for a body which is
// not already the text the parser reads.
bool IsZlibBody(const char* data, size_t length) {
    if (length < 2) {
        return false;
    }
    unsigned cmf = (unsigned char)data[0];
    unsigned flg = (unsigned char)data[1];
    return (cmf & 0x0f) == 8 && (flg & 0x20) == 0 &&
           (cmf * 256 + flg) % 31 == 0;
}

// Whether the body already is the iCalendar text the parser wants. Such a body is
// never decompressed: it cannot be gzip, and this is what keeps a plain text body
// whose first two bytes happen to look like zlib from being treated as compressed.
bool LooksLikeIcsText(std::string const& body) {
    return body.find("BEGIN:VCALENDAR") != std::string::npos ||
           body.find("BEGIN:VEVENT") != std::string::npos;
}

// Whether the body is compressed in one of the two ways this decodes.
bool IsCompressedBody(const char* data, size_t length) {
    return IsGzipBody(data, length) || IsZlibBody(data, length);
}

// deflate is read bit by bit, the least significant bit of a byte first.
class DeflateBits {
   public:
    DeflateBits(const unsigned char* data, size_t length)
        : m_data(data), m_length(length) {}

    // The next "count" bits, "count" being 16 at most. False means the stream
    // ended inside them, which is what turns a truncated response into a failure
    // instead of a shorter feed.
    bool Read(int count, unsigned& value) {
        while (m_bits < count) {
            if (m_position >= m_length) {
                return false;
            }
            m_window |= (unsigned)m_data[m_position++] << m_bits;
            m_bits += 8;
        }
        value = m_window & ((1u << count) - 1);
        m_window >>= count;
        m_bits -= count;
        return true;
    }

    // A stored block starts on the next byte boundary; whatever is left of the
    // current byte is padding.
    void AlignToByte() {
        m_window = 0;
        m_bits = 0;
    }

    // What is left, for a stored block, which is read as bytes and not as bits.
    const unsigned char* Position() const { return m_data + m_position; }
    size_t Remaining() const { return m_length - m_position; }
    void Skip(size_t count) { m_position += count; }

   private:
    const unsigned char* m_data;
    size_t m_length;
    size_t m_position = 0;
    unsigned m_window = 0;
    int m_bits = 0;
};

// One canonical Huffman code, the only kind deflate uses. counts[length] is how
// many symbols have a code of that many bits, and "symbols" lists every symbol
// which has a code, in code order.
struct HuffmanCode {
    int counts[16] = {};
    std::vector<short> symbols;

    // Builds the code from the length of each symbol's code; a length of 0 means
    // that the symbol has no code. False means the lengths do not describe a
    // usable code, which is a damaged stream.
    bool Build(const unsigned char* lengths, size_t lengthCount) {
        for (size_t i = 0; i < lengthCount; i++) {
            counts[lengths[i]]++;
        }
        counts[0] = 0;

        // How much of the code space is still unassigned at each length: lengths
        // which assign more codes than there is room for are the damaged case.
        int left = 1;
        for (int length = 1; length <= 15; length++) {
            left = (left << 1) - counts[length];
            if (left < 0) {
                return false;
            }
        }

        int offsets[16] = {};
        for (int length = 1; length < 15; length++) {
            offsets[length + 1] = offsets[length] + counts[length];
        }
        symbols.assign((size_t)(offsets[15] + counts[15]), 0);
        for (size_t i = 0; i < lengthCount; i++) {
            if (lengths[i]) {
                symbols[offsets[lengths[i]]++] = (short)i;
            }
        }
        return true;
    }

    // The next symbol, or false when the stream ends inside a code. A code is
    // walked bit by bit, which is all a feed of this size needs.
    bool Decode(DeflateBits& bits, unsigned& symbol) const {
        int code = 0;
        int first = 0;
        int index = 0;
        for (int length = 1; length <= 15; length++) {
            unsigned bit;
            if (!bits.Read(1, bit)) {
                return false;
            }
            code |= (int)bit;

            // The codes of this length start at "first": a code which lands below
            // that belongs to a shorter code, and the walk continues.
            int count = counts[length];
            if (code - count < first) {
                symbol = (unsigned)symbols[index + (code - first)];
                return true;
            }
            index += count;
            first = (first + count) << 1;
            code <<= 1;
        }
        return false;
    }
};

// A length or a distance code of deflate (RFC 1951, 3.2.5): the value it stands
// for, and how many extra bits follow it.
struct DeflateValue {
    unsigned base;
    unsigned extraBits;
};

constexpr DeflateValue kDeflateLengths[29] = {
    {3, 0},   {4, 0},   {5, 0},    {6, 0},    {7, 0},    {8, 0},    {9, 0},
    {10, 0},  {11, 1},  {13, 1},   {15, 1},   {17, 1},   {19, 2},   {23, 2},
    {27, 2},  {31, 2},  {35, 3},   {43, 3},   {51, 3},   {59, 3},   {67, 4},
    {83, 4},  {99, 4},  {115, 4},  {131, 5},  {163, 5},  {195, 5},  {227, 5},
    {258, 0}};

constexpr DeflateValue kDeflateDistances[30] = {
    {1, 0},     {2, 0},      {3, 0},      {4, 0},      {5, 1},      {7, 1},
    {9, 2},     {13, 2},     {17, 3},     {25, 3},     {33, 4},     {49, 4},
    {65, 5},    {97, 5},     {129, 6},    {193, 6},    {257, 7},    {385, 7},
    {513, 8},   {769, 8},    {1025, 9},   {1537, 9},   {2049, 10},  {3073, 10},
    {4097, 11}, {6145, 11},  {8193, 12},  {12289, 12}, {16385, 13}, {24577, 13}};

// The order the lengths of the code-length code are stored in (RFC 1951, 3.2.7):
// the lengths which are needed most come first.
constexpr unsigned char kCodeLengthOrder[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5,
                                                11, 4, 12, 3, 13, 2, 14, 1, 15};

// The two codes a fixed-Huffman block uses (RFC 1951, 3.2.6).
bool BuildFixedCodes(HuffmanCode& lengths, HuffmanCode& distances) {
    unsigned char literalLengths[288];
    for (int i = 0; i < 288; i++) {
        literalLengths[i] = i < 144   ? 8    // literals 0..143
                            : i < 256 ? 9    // literals 144..255
                            : i < 280 ? 7    // the end-of-block code and 257..279
                                      : 8;   // 280..287
    }
    unsigned char distanceLengths[30];
    for (int i = 0; i < 30; i++) {
        distanceLengths[i] = 5;
    }
    return lengths.Build(literalLengths, 288) &&
           distances.Build(distanceLengths, 30);
}

// The two codes a dynamic-Huffman block describes for itself (RFC 1951, 3.2.7).
bool ReadDynamicCodes(DeflateBits& bits,
                      HuffmanCode& lengths,
                      HuffmanCode& distances) {
    unsigned count;
    if (!bits.Read(5, count)) {
        return false;
    }
    const size_t literalCount = count + 257;
    if (!bits.Read(5, count)) {
        return false;
    }
    const size_t distanceCount = count + 1;
    if (!bits.Read(4, count)) {
        return false;
    }
    const size_t codeLengthCount = count + 4;

    // A block cannot carry more codes than the format defines.
    if (literalCount > 286 || distanceCount > 30) {
        return false;
    }

    unsigned char codeLengths[19] = {};
    for (size_t i = 0; i < codeLengthCount; i++) {
        unsigned value;
        if (!bits.Read(3, value)) {
            return false;
        }
        codeLengths[kCodeLengthOrder[i]] = (unsigned char)value;
    }

    HuffmanCode codeLengthCode;
    if (!codeLengthCode.Build(codeLengths, 19)) {
        return false;
    }

    // The lengths of the two codes, run-length encoded with that code: 16 repeats
    // the length before it, and 17 and 18 stand for runs of lengths which are 0,
    // meaning "this symbol has no code".
    unsigned char allLengths[286 + 30] = {};
    const size_t total = literalCount + distanceCount;
    size_t index = 0;
    while (index < total) {
        unsigned symbol;
        if (!codeLengthCode.Decode(bits, symbol)) {
            return false;
        }

        unsigned repeat = 1;
        unsigned extra = 0;
        unsigned char value = (unsigned char)symbol;
        if (symbol == 16) {
            if (index == 0) {
                return false;
            }
            value = allLengths[index - 1];
            if (!bits.Read(2, extra)) {
                return false;
            }
            repeat = 3 + extra;
        } else if (symbol == 17 || symbol == 18) {
            value = 0;
            if (!bits.Read(symbol == 17 ? 3 : 7, extra)) {
                return false;
            }
            repeat = (symbol == 17 ? 3 : 11) + extra;
        } else if (symbol > 18) {
            // 19 and up are not code lengths.
            return false;
        }

        if (index + repeat > total) {
            return false;
        }
        while (repeat--) {
            allLengths[index++] = value;
        }
    }

    // A block which has no end-of-block code cannot be ended.
    if (allLengths[256] == 0) {
        return false;
    }

    return lengths.Build(allLengths, literalCount) &&
           distances.Build(allLengths + literalCount, distanceCount);
}

// One Huffman-coded block: literals are written out, and a length together with a
// distance copies what has already been written (RFC 1951, 3.2.3).
bool ReadCompressedBlock(DeflateBits& bits,
                         const HuffmanCode& lengths,
                         const HuffmanCode& distances,
                         std::string& out) {
    while (true) {
        unsigned symbol;
        if (!lengths.Decode(bits, symbol)) {
            return false;
        }

        if (symbol < 256) {
            out += (char)symbol;
        } else if (symbol == 256) {
            return true;
        } else {
            symbol -= 257;
            if (symbol >= 29) {
                // 286 and 287 are not used by the format.
                return false;
            }

            unsigned length = kDeflateLengths[symbol].base;
            if (kDeflateLengths[symbol].extraBits) {
                unsigned extra;
                if (!bits.Read(kDeflateLengths[symbol].extraBits, extra)) {
                    return false;
                }
                length += extra;
            }

            unsigned distanceSymbol;
            if (!distances.Decode(bits, distanceSymbol)) {
                return false;
            }
            if (distanceSymbol >= 30) {
                return false;
            }

            unsigned distance = kDeflateDistances[distanceSymbol].base;
            if (kDeflateDistances[distanceSymbol].extraBits) {
                unsigned extra;
                if (!bits.Read(kDeflateDistances[distanceSymbol].extraBits,
                               extra)) {
                    return false;
                }
                distance += extra;
            }

            // The copy may overlap what it copies - a distance of one repeats the
            // byte just written - so it is done one byte at a time.
            if (distance > out.size()) {
                return false;
            }
            for (unsigned i = 0; i < length; i++) {
                char byte = out[out.size() - distance];
                out += byte;
            }
        }

        if (out.size() > kMaxDecodedBodyBytes) {
            return false;
        }
    }
}

// The deflate stream itself (RFC 1951): a series of blocks, the last of which says
// that it is the last.
bool InflateDeflate(const unsigned char* data, size_t length, std::string& out) {
    DeflateBits bits(data, length);

    unsigned isLast = 0;
    do {
        if (!bits.Read(1, isLast)) {
            return false;
        }

        unsigned type;
        if (!bits.Read(2, type)) {
            return false;
        }

        if (type == 0) {
            // A stored block: the length of the data, the same length with every
            // bit flipped, and then the data as it is.
            bits.AlignToByte();
            if (bits.Remaining() < 4) {
                return false;
            }
            const unsigned char* header = bits.Position();
            unsigned storedLength = header[0] | (header[1] << 8);
            unsigned complement = header[2] | (header[3] << 8);
            if ((storedLength ^ 0xffff) != complement) {
                return false;
            }
            bits.Skip(4);
            if (bits.Remaining() < storedLength) {
                return false;
            }
            out.append((const char*)bits.Position(), storedLength);
            bits.Skip(storedLength);
        } else if (type == 1 || type == 2) {
            HuffmanCode lengths;
            HuffmanCode distances;
            if (type == 1) {
                if (!BuildFixedCodes(lengths, distances)) {
                    return false;
                }
            } else if (!ReadDynamicCodes(bits, lengths, distances)) {
                return false;
            }
            if (!ReadCompressedBlock(bits, lengths, distances, out)) {
                return false;
            }
        } else {
            // A type of 3 is reserved, and there is no way to read past it.
            return false;
        }

        if (out.size() > kMaxDecodedBodyBytes) {
            return false;
        }
    } while (!isLast);

    return true;
}

// The CRC-32 and the adler-32 of the two wrappers, over the decoded bytes.
uint32_t Crc32(std::string const& text) {
    uint32_t crc = 0xffffffff;
    for (unsigned char byte : text) {
        crc ^= byte;
        for (int bit = 0; bit < 8; bit++) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xedb88320u : 0u);
        }
    }
    return crc ^ 0xffffffff;
}

uint32_t Adler32(std::string const& text) {
    uint32_t a = 1;
    uint32_t b = 0;
    for (unsigned char byte : text) {
        a = (a + byte) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) | a;
}

// Steps "offset" past the zero-terminated string which starts there.
bool SkipZeroTerminated(const unsigned char* data, size_t length, size_t& offset) {
    while (offset < length && data[offset]) {
        offset++;
    }
    if (offset >= length) {
        return false;
    }
    offset++;  // the terminating zero
    return true;
}

// The four bytes at "data" as a little-endian number, which is how a stored
// block's length and both trailers are written.
uint32_t ReadLittleEndian(const unsigned char* data) {
    return (uint32_t)data[0] | ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
}

// Decodes a gzip or zlib body into "out". False means the body is compressed but
// could not be decoded, and then there is no feed at all: the caller keeps the
// data it already has rather than showing a feed which is quietly wrong.
bool DecompressBody(const char* data, size_t length, std::string& out) {
    // "out" is empty unless the whole body decoded: a caller which keeps the data
    // it already has has to be able to tell "nothing decoded" from "half of it".
    out.clear();

    const unsigned char* bytes = (const unsigned char*)data;
    const bool gzip = IsGzipBody(data, length);
    size_t offset = 0;

    if (gzip) {
        // The ten-byte header - magic, method, flags, time, extra flags, OS - and
        // then the fields the flags name (RFC 1952, 2.3.1).
        if (length < 10 || bytes[2] != 8) {
            return false;
        }
        const unsigned char flags = bytes[3];
        offset = 10;

        if (flags & 0x04) {  // FEXTRA: a two-byte length, then that many bytes
            if (length - offset < 2) {
                return false;
            }
            size_t extra =
                (size_t)bytes[offset] | ((size_t)bytes[offset + 1] << 8);
            offset += 2;
            if (length - offset < extra) {
                return false;
            }
            offset += extra;
        }
        if (flags & 0x08) {  // FNAME: a zero-terminated file name
            if (!SkipZeroTerminated(bytes, length, offset)) {
                return false;
            }
        }
        if (flags & 0x10) {  // FCOMMENT: a zero-terminated comment
            if (!SkipZeroTerminated(bytes, length, offset)) {
                return false;
            }
        }
        if (flags & 0x02) {  // FHCRC: the header's own two-byte checksum
            if (length - offset < 2) {
                return false;
            }
            offset += 2;
        }
    } else if (IsZlibBody(data, length)) {
        // The two-byte zlib header. IsZlibBody has already turned away a stream
        // which asks for a preset dictionary, since there is none to give it.
        offset = 2;
    } else {
        return false;
    }

    std::string decoded;
    if (!InflateDeflate(bytes + offset, length - offset, decoded)) {
        return false;
    }

    // The trailer, which both wrappers carry for the decoded data. Checking it is
    // what keeps a response which lost or gained bytes on the way from being read
    // as a feed which is quietly wrong.
    if (gzip) {
        if (length < 8) {
            return false;
        }
        const unsigned char* trailer = bytes + length - 8;
        if (ReadLittleEndian(trailer) != Crc32(decoded) ||
            ReadLittleEndian(trailer + 4) != (uint32_t)decoded.size()) {
            return false;
        }
    } else {
        if (length < 4) {
            return false;
        }
        if (ReadLittleEndian(bytes + length - 4) != Adler32(decoded)) {
            return false;
        }
    }

    out = std::move(decoded);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Background fetching

void SweepDayItems();
void StartSweepTimer();
void RefreshCalendarOnUiThreads();

// The worker thread exists only while a data source is set: it is created by the
// first refresh request which has something to fetch, and a failed start is retried
// by the next one (see RequestHolidayData). The mutex is what keeps two threads which
// ask at the same time from creating two threads or two event pairs.
std::mutex g_workerThreadMutex;
std::atomic<HANDLE> g_hWorkerThread{nullptr};
std::atomic<HANDLE> g_hWorkEvent{nullptr};
std::atomic<HANDLE> g_hStopEvent{nullptr};
std::atomic<bool> g_fetchPending{false};
// Set while the mod is unloading: a refresh request which arrives during the
// unload must not start a worker thread again.
std::atomic<bool> g_shuttingDown{false};

std::mutex g_fetchStateMutex;
ULONGLONG g_lastFetchAttemptTick = 0;
bool g_haveData = false;

// After a failed request, retry after this delay instead of waiting for the
// whole refresh interval.
constexpr ULONGLONG kFetchRetryDelayMs = 10 * 60 * 1000;

// Normalize an ICS feed URL and ensure it uses HTTP or HTTPS.
std::wstring NormalizeIcsUrl(std::wstring const& url) {
    if (url.empty()) {
        return url;
    }

    std::wstring s = url;

    // Trim whitespace
    size_t begin = 0;
    while (begin < s.size() && iswspace(s[begin])) {
        begin++;
    }
    size_t end = s.size();
    while (end > begin && iswspace(s[end - 1])) {
        end--;
    }
    s = s.substr(begin, end - begin);

    // Whitespace-only input trims to nothing: report it as unusable instead of
    // falling through and building a bare "https://".
    if (s.empty()) {
        return s;
    }

    // Extract and normalize the URL scheme.
    size_t colonSlashSlash = s.find(L"://");
    if (colonSlashSlash != std::wstring::npos) {
        std::wstring scheme;
        for (size_t i = 0; i < colonSlashSlash; i++) {
            if (iswalpha(s[i])) {
                scheme += (wchar_t) towlower(s[i]);
            } else {
                scheme.clear();
                break;
            }
        }
        if (!scheme.empty()) {
            if (scheme == L"https" || scheme == L"http") {
                return s;
            }
            if (scheme == L"webcal") {
                return L"http" + s.substr(colonSlashSlash);
            }
            if (scheme == L"webcals") {
                return L"https" + s.substr(colonSlashSlash);
            }
        }
    }

    // Preserve protocol-relative URLs and default them to HTTPS.
    if (s.size() >= 2 && s[0] == L'/' && s[1] == L'/') {
        return L"https:" + s;
    }

    // Assume HTTPS when no supported scheme is specified.
    return L"https://" + s;
}

// Fetching and parsing both happen on the worker thread: Wh_GetUrlContent blocks on
// network I/O, and doing that on a UI thread would hold up the taskbar. A parse which
// fails does not overwrite the data which is already there, so the calendar keeps
// showing the last good result when the network or the API is down (g_haveData only
// decides which retry delay applies). An empty address is not a failure to report: it
// means the mod has no data source, and RequestHolidayData never gets here then.
bool FetchHolidayData() {
    const std::shared_ptr<const Settings> settings = GetSettings();

    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        g_lastFetchAttemptTick = GetTickCount64();
    }

    if (settings->dataSourceUrl.empty()) {
        return false;
    }

    const std::wstring url = NormalizeIcsUrl(settings->dataSourceUrl);
    if (url.empty()) {
        return false;
    }

    if (url != settings->dataSourceUrl) {
        Wh_Log(L"Using normalized URL: %s (was: %s)",
               url.c_str(), settings->dataSourceUrl.c_str());
    } else {
        Wh_Log(L"Fetching %s", url.c_str());
    }

    const WH_URL_CONTENT* content =
        Wh_GetUrlContent(url.c_str(), nullptr);
    if (!content) {
        Wh_Log(L"Wh_GetUrlContent returned null");
        return false;
    }

    // The body is copied out before the content is freed: a compressed body is
    // expanded below, and Wh_FreeUrlContent takes the bytes away.
    std::string rawBody;
    bool ok = content->statusCode == 200 && content->data && content->length;
    if (ok) {
        rawBody.assign(content->data, content->length);
    } else {
        Wh_Log(L"Wh_GetUrlContent returned HTTP status %d", content->statusCode);
    }
    Wh_FreeUrlContent(content);

    if (!ok || rawBody.empty()) {
        return false;
    }

    // Wh_GetUrlContent hands back what the server sent, and a server may send the
    // feed compressed, so a body which is gzip or zlib is expanded before it is
    // turned into text. The two bytes of the gzip magic cannot be text at all; the
    // two bytes a zlib header consists of can, which is why a body which already
    // is the text the parser reads is left exactly as it is.
    std::string bodyBytes;
    if (!LooksLikeIcsText(rawBody) &&
        IsCompressedBody(rawBody.data(), rawBody.size())) {
        if (!DecompressBody(rawBody.data(), rawBody.size(), bodyBytes)) {
            Wh_Log(L"The response is compressed, but it is not a stream which "
                   L"could be decompressed");
            return false;
        }
        Wh_Log(L"Decompressed the response from %zu to %zu bytes",
               rawBody.size(), bodyBytes.size());
    } else {
        bodyBytes = std::move(rawBody);
    }

    std::wstring body = Utf8ToWide(bodyBytes.data(), bodyBytes.size());
    if (body.empty()) {
        return false;
    }

    // The settings are read again here, after the response: the rules may have been
    // changed while it was on its way, and the list which is current is the one the
    // user expects to see the feed marked with. The address is the one this response
    // was asked for, so a response which arrives after the address was changed is
    // dropped - its data would be the data of an address which is no longer set.
    const std::shared_ptr<const Settings> current = GetSettings();
    if (NormalizeIcsUrl(current->dataSourceUrl) != url) {
        Wh_Log(L"The data source changed while the feed was on its way; the "
               L"response is dropped");
        return false;
    }

    HolidayMap parsed;
    if (!ParseHolidayData(body, current->rules, parsed)) {
        Wh_Log(L"Failed to parse the holiday data (%zu characters)", body.size());
        return false;
    }

    size_t entryCount = parsed.size();
    // The body is stored with the data even when nothing was marked: the rules are a
    // setting like any other, and a feed which is read but marks nothing is one which
    // has to be marked again - from the body - as soon as the rules say so (see
    // Wh_ModSettingsChanged).
    StoreHolidays(body, std::move(parsed));

    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        g_haveData = true;
    }

    if (entryCount) {
        Wh_Log(L"Loaded %zu marked dates from the data source", entryCount);
    } else if (current->rules.empty()) {
        Wh_Log(L"The feed is read, but no highlight rule is set, so no date is marked; "
               L"add one in the settings of this mod");
    } else {
        Wh_Log(L"The feed is read, but no event of it matches the %zu highlight "
               L"rule(s), so no date is marked",
               current->rules.size());
    }
    return true;
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    while (true) {
        HANDLE stopEvent = g_hStopEvent.load();
        HANDLE workEvent = g_hWorkEvent.load();
        if (!stopEvent || !workEvent) {
            break;
        }

        HANDLE events[2] = {stopEvent, workEvent};
        DWORD result = WaitForMultipleObjects(2, events, FALSE, INFINITE);
        if (result == WAIT_OBJECT_0) {
            break;
        }
        if (result != WAIT_OBJECT_0 + 1) {
            break;
        }

        g_fetchPending = false;

        if (FetchHolidayData()) {
            RefreshCalendarOnUiThreads();
        }
    }

    return 0;
}

// Every step is checked here. A CreateEvent which fails would leave the worker
// thread with a null handle for that event (a thread which finds one exits at
// once), and a request handed to a thread which was never started would stay
// pending for good, so that every later refresh is dropped silently. On a failure
// the handles which were created are closed again and the mod simply runs without
// a worker thread: it keeps showing the data it already has, and RequestHolidayData
// asks for the thread again on every refresh, so a start which failed once is retried
// instead of lasting for the whole session.
void StartWorkerThread() {
    std::lock_guard<std::mutex> lock(g_workerThreadMutex);

    if (g_hWorkerThread.load() || g_shuttingDown) {
        return;
    }

    HANDLE workEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HANDLE stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!workEvent || !stopEvent) {
        Wh_Log(L"Failed to create the worker events (error %u)", GetLastError());
        if (workEvent) {
            CloseHandle(workEvent);
        }
        if (stopEvent) {
            CloseHandle(stopEvent);
        }
        return;
    }

    // The handles go in before the thread starts: the thread reads them, and a
    // thread which finds a null handle returns immediately.
    g_hWorkEvent.store(workEvent);
    g_hStopEvent.store(stopEvent);

    HANDLE thread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!thread) {
        Wh_Log(L"Failed to create the worker thread (error %u)", GetLastError());
        g_hWorkEvent.store(nullptr);
        g_hStopEvent.store(nullptr);
        CloseHandle(workEvent);
        CloseHandle(stopEvent);
        return;
    }

    g_hWorkerThread.store(thread);
}

// The handles are taken out of the globals and the lock is dropped before the wait.
// Waiting while holding g_workerThreadMutex is a deadlock: the worker thread may be
// inside RefreshCalendarOnUiThreads, which waits for a UI thread, and that UI thread
// may have passed the g_shuttingDown test of RequestHolidayData and be about to block
// on this mutex in StartWorkerThread. The window is narrow - the unload has to land
// between those two - but closing it costs nothing. Taking the handles first also
// keeps the worker from being handed a new request while it is being stopped.
void StopWorkerThread() {
    HANDLE workerThread;
    HANDLE workEvent;
    HANDLE stopEvent;

    {
        std::lock_guard<std::mutex> lock(g_workerThreadMutex);

        workerThread = g_hWorkerThread.exchange(nullptr);
        workEvent = g_hWorkEvent.exchange(nullptr);
        stopEvent = g_hStopEvent.exchange(nullptr);

        if (stopEvent) {
            SetEvent(stopEvent);
        }
    }

    // The worker is joined rather than detached. Wh_GetUrlContent cannot be cancelled,
    // so the unload waits for a request which is in flight (bounded by the engine's
    // own timeout), but WorkerThreadProc is mod code and must not be left running into
    // an image which is about to be unmapped.
    if (workerThread) {
        WaitForSingleObject(workerThread, INFINITE);
        CloseHandle(workerThread);
    }
    if (workEvent) {
        CloseHandle(workEvent);
    }
    if (stopEvent) {
        CloseHandle(stopEvent);
    }
}

// The decision whether a fetch is due lives here: no repeated request inside the
// refresh interval, and a shorter 10 minute retry interval after a failure. A caller
// which has to re-read the data at once - a changed address, for example - does it by
// clearing the attempt tick through ResetHolidayData rather than by a flag here.
void RequestHolidayData() {
    const std::shared_ptr<const Settings> settings = GetSettings();
    if (g_shuttingDown) {
        return;
    }

    // No data source: nothing to fetch and no worker thread to create for it. This is
    // the default state of the mod, and it is what keeps the mod from doing anything
    // at all - no thread, no request - until an address is filled in.
    if (settings->dataSourceUrl.empty()) {
        return;
    }

    // The worker thread is started on the first request which has a data source; a
    // start which failed is retried here, on every request. This has to happen before
    // the "is the data still fresh" test below, because a request which could not be
    // answered must not consume the refresh interval. StartWorkerThread takes a lock,
    // so a request from another thread cannot create a second one.
    if (!g_hWorkerThread.load()) {
        StartWorkerThread();
    }

    const ULONGLONG refreshInterval =
        static_cast<ULONGLONG>(settings->refreshIntervalHours) * 60ULL * 60ULL * 1000ULL;

    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        if (g_lastFetchAttemptTick) {
            ULONGLONG interval = g_haveData ? refreshInterval : kFetchRetryDelayMs;
            if (GetTickCount64() - g_lastFetchAttemptTick < interval) {
                return;
            }
        }
    }

    if (g_fetchPending.exchange(true)) {
        return;
    }

    // The request is only answered while the worker thread is there to take it:
    // an event which cannot be signalled (or a worker thread which was never
    // started) would otherwise leave the flag set, and every later refresh would
    // be dropped without a word.
    if (HANDLE workEvent = g_hWorkEvent.load()) {
        if (SetEvent(workEvent)) {
            return;
        }
        Wh_Log(L"Could not hand the refresh over to the worker thread (error %u)",
               GetLastError());
    } else {
        Wh_Log(L"There is no worker thread, so the holiday data cannot be fetched");
    }

    // Nothing is going to answer this request: taking the flag back keeps the next
    // refresh from being dropped, and the attempt is counted so that this path is
    // not taken again on every day cell the shell creates.
    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        g_lastFetchAttemptTick = GetTickCount64();
    }
    g_fetchPending = false;
}

void ResetHolidayData() {
    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        g_lastFetchAttemptTick = 0;
        g_haveData = false;
    }
    {
        std::lock_guard<std::mutex> lock(g_holidaysMutex);
        g_holidays.clear();
        g_holidaysBody.clear();
    }
}

// Marks the data which is already loaded with a new rule list, instead of asking the
// data source for the same feed again. The body which was kept is what makes this
// possible; it is read here rather than at the point the setting changed so that the
// parse - which walks every date of the feed - happens outside the lock which guards the
// data, and StoreHolidays publishes the result in one piece.
void RemarkStoredHolidays(std::vector<HighlightRule> const& rules) {
    std::wstring body;
    {
        std::lock_guard<std::mutex> lock(g_holidaysMutex);
        body = g_holidaysBody;
    }
    if (body.empty()) {
        // No feed has been read yet, or the address changed and threw it away: there is
        // nothing to mark, and the fetch which is on its way does it.
        return;
    }

    HolidayMap parsed;
    if (!ParseHolidayData(body, rules, parsed)) {
        // The body is the one which was read before, so this cannot happen except
        // through a change the mod does not know about; the data which is on screen is
        // left as it is rather than taken away.
        Wh_Log(L"The feed which was read before does not read again; the data is kept "
               L"as it is");
        return;
    }

    size_t entryCount = parsed.size();
    StoreHolidays(body, std::move(parsed));

    if (entryCount) {
        Wh_Log(L"Marked the feed which was read before again with %zu rule(s): %zu "
               L"dates",
               rules.size(), entryCount);
    } else {
        Wh_Log(L"Marked the feed which was read before again with %zu rule(s), which "
               L"match no event of it: no date is marked",
               rules.size());
    }
}

////////////////////////////////////////////////////////////////////////////////
// Calendar UI

////////////////////////////////////////////////////////////////////////////////
// Looking inside a day cell

// The date a day cell stands for.
//
// CalendarViewDayItem.Date is a Windows.Foundation.DateTime: 100 ns ticks since 1601, with
// no zone attached, so the same value can be read in two ways, and the two differ by the
// local UTC offset:
//
//   * the ticks name local midnight of the date the cell was made for, which is what the
//     shell does when it fills a cell for a calendar date;
//   * the ticks name midnight UTC of that date.
//
// Which one Windows uses cannot be observed on this machine, and does not have to be: the
// choice below is right either way, in every time zone.
//
//   * On or east of UTC, the local reading is the cell's date under both readings of the
//     ticks. With local midnight it is the date itself, and with midnight UTC the local
//     time of 00:00Z is still the same date in the same zone (an hour or more of the same
//     day, never the day before).
//   * West of UTC, the UTC reading is the cell's date under both. With midnight UTC it is
//     the date itself, and with local midnight the two readings name the same date anyway:
//     local midnight lies behind UTC, but less than a day behind.
//
// So "west of UTC ? the UTC date : the local date" is right in all four combinations, and
// it is a property of the cell alone - nothing is remembered and nothing is guessed, which
// also means a day cell the shell is still filling in cannot be read against a day number
// left over from the date it had before.
//
// Which side of UTC the machine is on is taken from the two readings of this very value,
// not from the time zone of the process: the local wall clock is behind the UTC wall clock
// exactly when the offset is negative, and the difference carries the offset in force on
// that date, daylight saving included.
//
// A conversion which fails returns 0, and the caller skips that cell: writing nothing is
// better than writing the wrong date.
int32_t DateKeyFromDateTime(wf::DateTime const& dateTime) {
    ULARGE_INTEGER uli;
    uli.QuadPart = static_cast<ULONGLONG>(dateTime.time_since_epoch().count());

    FILETIME fileTime;
    fileTime.dwLowDateTime = uli.LowPart;
    fileTime.dwHighDateTime = uli.HighPart;

    SYSTEMTIME systemTimeUtc{};
    if (!FileTimeToSystemTime(&fileTime, &systemTimeUtc)) {
        return 0;
    }

    SYSTEMTIME systemTimeLocal{};
    if (!SystemTimeToTzSpecificLocalTime(nullptr, &systemTimeUtc,
                                         &systemTimeLocal)) {
        return 0;
    }

    // Both wall-clock readings as ticks: the local one is behind the UTC one exactly when
    // this machine is west of UTC on that date.
    auto ticksOf = [](SYSTEMTIME const& systemTime, ULONGLONG& ticks) -> bool {
        FILETIME fileTimeOfSystemTime{};
        if (!SystemTimeToFileTime(&systemTime, &fileTimeOfSystemTime)) {
            return false;
        }
        ULARGE_INTEGER value;
        value.LowPart = fileTimeOfSystemTime.dwLowDateTime;
        value.HighPart = fileTimeOfSystemTime.dwHighDateTime;
        ticks = value.QuadPart;
        return true;
    };

    ULONGLONG localTicks = 0;
    ULONGLONG utcTicks = 0;
    if (!ticksOf(systemTimeLocal, localTicks) || !ticksOf(systemTimeUtc, utcTicks)) {
        return 0;
    }

    const SYSTEMTIME& date =
        localTicks < utcTicks ? systemTimeUtc : systemTimeLocal;
    return MakeDateKey(date.wYear, date.wMonth, date.wDay);
}

// The name the Windows 11 calendar gives the text block which holds the lunar
// date or the solar term.
// It is a strong hint rather than the only rule: another Windows build may name the
// block differently, which is why FindLabelTextBlock has fallbacks.
constexpr PCWSTR kLunarTextBlockName = L"LunarTextBlock";

struct FoundElement {
    wux::DependencyObject element{nullptr};
    int depth = 0;
};

// Walks the visual tree of a day cell. The limits (256 elements, 24 levels) are purely
// defensive and are never reached by a cell of three or four elements; what they guard
// against is an element being destroyed by the shell while the walk is in progress.
void CollectDescendants(wux::DependencyObject const& root,
                        std::vector<FoundElement>& out) {
    out.push_back({root, 0});

    for (size_t i = 0; i < out.size() && i < 256; i++) {
        wux::DependencyObject current = out[i].element;
        int depth = out[i].depth;
        if (!current || depth > 24) {
            continue;
        }

        int count = 0;
        try {
            count = wuxm::VisualTreeHelper::GetChildrenCount(current);
        } catch (...) {
            continue;
        }

        for (int childIndex = 0; childIndex < count; childIndex++) {
            wux::DependencyObject child = nullptr;
            try {
                child = wuxm::VisualTreeHelper::GetChild(current, childIndex);
            } catch (...) {
                continue;
            }
            if (!child) {
                continue;
            }
            out.push_back({child, depth + 1});
        }
    }
}

std::wstring ElementName(wux::DependencyObject const& element) {
    if (auto frameworkElement = element.try_as<wux::FrameworkElement>()) {
        try {
            return frameworkElement.Name().c_str();
        } catch (...) {
        }
    }
    return {};
}

std::wstring ElementText(wuxc::TextBlock const& textBlock) {
    try {
        return TrimWhitespace(textBlock.Text().c_str());
    } catch (...) {
        return {};
    }
}

// Prints what the day cell is made of, so that a cell which doesn't look the way
// the mod expects can be diagnosed from the log.
void LogDayItemStructure(wuxc::CalendarViewDayItem const& item) {
    std::vector<FoundElement> descendants;
    CollectDescendants(item, descendants);

    Wh_Log(L"Day cell structure (%zu elements):", descendants.size());
    for (const auto& found : descendants) {
        if (!found.element) {
            continue;
        }

        std::wstring line(found.depth * 2, L' ');
        line += winrt::get_class_name(found.element).c_str();

        if (std::wstring name = ElementName(found.element); !name.empty()) {
            line += L" #" + name;
        }

        if (auto textBlock = found.element.try_as<wuxc::TextBlock>()) {
            line += L" fontSize=" + std::to_wstring((int)textBlock.FontSize());
            if (std::wstring text = ElementText(textBlock); !text.empty()) {
                line += L" text=\"" + text + L"\"";
            }
        }

        if (auto frameworkElement = found.element.try_as<wux::FrameworkElement>()) {
            line += L" size=" + std::to_wstring((int)frameworkElement.ActualWidth()) +
                    L"x" + std::to_wstring((int)frameworkElement.ActualHeight());
        }

        Wh_Log(L"%s", line.c_str());
    }
}

// The day number is the only text in a day cell which is made of digits: the
// lunar date below it is "廿二", "立冬" or "中秋节". A text block which holds a
// number is therefore never the one the mod may write to.
bool IsNumberText(std::wstring const& text) {
    if (text.empty()) {
        return false;
    }
    for (wchar_t c : text) {
        if (c < L'0' || c > L'9') {
            return false;
        }
    }
    return true;
}

// Whether the shell has filled the day cell in at all. A day item which was just
// created has no content yet, and there is nothing to report about it.
bool DayItemHasContent(wuxc::CalendarViewDayItem const& item) {
    try {
        return wuxm::VisualTreeHelper::GetChildrenCount(item) > 0;
    } catch (...) {
    }
    return false;
}

// Finds the text block which holds the shell's lunar/solar-term text: the text
// block inside the day cell which is not the day number.
//
// This is done again on every pass rather than remembered: the shell takes the
// text blocks out of a day item and puts new ones in when it reuses the item for
// another month, and a text block which is no longer in the calendar takes an
// update to nowhere. A day cell has three or four children, so walking it is
// cheap.
wuxc::TextBlock FindLabelTextBlock(wuxc::CalendarViewDayItem const& item,
                                   void* preferredId) {
    std::vector<FoundElement> descendants;
    CollectDescendants(item, descendants);

    std::vector<wuxc::TextBlock> candidates;
    for (const auto& found : descendants) {
        auto textBlock = found.element.try_as<wuxc::TextBlock>();
        if (!textBlock || IsNumberText(ElementText(textBlock))) {
            continue;
        }
        candidates.push_back(textBlock);
    }

    if (candidates.empty()) {
        return nullptr;
    }

    // The text block the mod wrote to before, as long as the calendar still
    // shows it, so that what was written there is not written a second time.
    if (preferredId) {
        for (const auto& candidate : candidates) {
            if (winrt::get_abi(candidate) == preferredId) {
                return candidate;
            }
        }
    }

    // The name the Windows 11 calendar gives its lunar text block wins over the
    // heuristics below. Other builds may name it differently, and the heuristics
    // are what is used then.
    for (const auto& candidate : candidates) {
        if (ElementName(candidate) == kLunarTextBlockName) {
            return candidate;
        }
    }

    // The lunar text is the smaller of the texts: prefer a text block which
    // already has text, and among those the one with the smallest font.
    wuxc::TextBlock best = nullptr;
    double bestFontSize = 0;
    bool bestHasText = false;
    for (const auto& candidate : candidates) {
        bool hasText = !ElementText(candidate).empty();
        double fontSize = 0;
        try {
            fontSize = candidate.FontSize();
        } catch (...) {
        }

        if (!best || (hasText != bestHasText && hasText) ||
            (hasText == bestHasText && fontSize < bestFontSize)) {
            best = candidate;
            bestFontSize = fontSize;
            bestHasText = hasText;
        }
    }

    return best;
}

////////////////////////////////////////////////////////////////////////////////
// The state of the day cells

// The state of one day cell. The shell recycles day items and replaces the text
// blocks inside them, so the state is kept here and looked up by the day item's
// identity instead of by remembering the elements themselves.
struct DayCell {
    winrt::weak_ref<wuxc::CalendarViewDayItem> item{nullptr};
    // The text block the mod writes to, and the callback which notices the shell
    // writing its own lunar text over the mod's holiday name.
    winrt::weak_ref<wuxc::TextBlock> label{nullptr};
    void* labelId = nullptr;
    int64_t labelChangedToken = 0;
    // The date this state was computed for. A day item which now shows another
    // date was filled in by the shell again, so its text belongs to the shell.
    int32_t key = 0;
    // The shell's own lunar/solar-term text, saved before the first change. Only
    // trusted while the same cell shows the same date.
    std::wstring savedText;
    bool hasSavedText = false;
    // The brush the label carried before the mod wrote its own colour, when it
    // carried one of its own, and whether it did at all. Held by a strong
    // reference: the shell drops its own reference to a brush once it is applied,
    // and a brush which is gone can't be put back.
    wuxm::Brush savedForeground{nullptr};
    bool hasSavedForeground = false;
    bool savedForegroundWasLocal = false;
    // The brush the mod itself wrote, if it wrote one. The colour on the label is
    // compared with this one to tell the mod's own colour from the colour of the
    // shell - two brushes which hold the same colour are still two brushes. The
    // reference stays alive, so the object it names can't be reused by another
    // brush and make a later comparison wrong.
    wuxm::Brush ownBrush{nullptr};
    // What the mod wrote into the cell, if anything.
    std::wstring appliedText;
    // Whether the mod's colour is on the label right now. This belongs to the
    // label element rather than to the date: an element the shell reuses for
    // another month still carries the mod's brush.
    bool hadColor = false;
    int64_t dateChangedToken = 0;
    bool busy = false;
};

// A day item of this thread, with the identity it is tracked by. The walk comes across
// the same day item on every pass - a month change puts the same ones back into the
// calendar - so an item is tracked here only once.
struct TrackedDayItem {
    void* id = nullptr;
    winrt::weak_ref<wuxc::CalendarViewDayItem> item{nullptr};
};

// The most day items to keep an eye on. The walk finds every day item the calendar holds,
// and the calendar holds more than the six weeks a month view shows: the first limit this
// had (128, from when the calendar reported its cells one by one) was reached by a single
// pass of the walk on a live flyout. A full list makes a pass drop the cells it found
// first, so the limit has to leave room for the tree; what it is really for is the
// opposite case - a shell which keeps day items alive in a pool the calendar no longer
// shows. A day item which is gone is released and dropped on the next pass, and one which
// is shown again is found again by the walk, and tracked again.
constexpr size_t kMaxTrackedDayItems = 512;

// The state of every day cell of this thread, keyed by the day item's identity.
// Two references to the same XAML element compare unequal, and the element's Tag
// belongs to the shell, so the raw interface pointer is what identifies a cell.
// The cells are held behind a pointer: adding a cell while another one is being
// worked on must not move it.
thread_local std::unordered_map<void*, std::unique_ptr<DayCell>> t_cells;
thread_local std::vector<TrackedDayItem> t_dayItems;
// The day item which owns a text block, so that a text change can be traced back
// to the cell it belongs to. The parent chain is not usable for this: the text
// block of a day cell reports no parent at all.
thread_local std::unordered_map<void*, winrt::weak_ref<wuxc::CalendarViewDayItem>>
    t_labelOwners;
// A day cell the mod could not find the lunar text in is worth reporting once,
// with the structure of the cell, and not once per cell.
thread_local bool t_loggedMissingTextBlock = false;
// Hitting the limit of tracked day items says that the shell is holding on to
// day items which the calendar no longer shows, which is worth reporting once.
thread_local bool t_loggedTrackLimit = false;
// Set by whatever a pass over a cell actually writes - the text, the colour, or the
// colour being taken back. SweepDayItems() uses it to tell "the shell is still rewriting
// the cells" from "nothing has happened for a while", which is what stops the timer again.
thread_local bool t_applyWrote = false;

void ApplyToDayItem(wuxc::CalendarViewDayItem const& item);

void* CellId(wuxc::CalendarViewDayItem const& item) {
    return winrt::get_abi(item);
}

DayCell& CellFor(wuxc::CalendarViewDayItem const& item) {
    std::unique_ptr<DayCell>& entry = t_cells[CellId(item)];
    if (entry && entry->item.get() != item) {
        // The day item this state was kept for is gone, and another one is
        // using its address: the state belongs to the old one.
        entry = nullptr;
    }
    if (!entry) {
        entry = std::make_unique<DayCell>();
        entry->item = winrt::make_weak(item);
    }
    return *entry;
}

// Remembers a day item the walk found. Finding a day item which is already tracked
// again only makes it the most recent one, so that the list holds the day items the
// calendar used last.
void TrackDayItem(wuxc::CalendarViewDayItem const& item) {
    void* id = winrt::get_abi(item);

    for (auto it = t_dayItems.begin(); it != t_dayItems.end(); ++it) {
        if (it->id != id) {
            continue;
        }
        if (it->item.get()) {
            TrackedDayItem tracked = *it;
            t_dayItems.erase(it);
            t_dayItems.push_back(tracked);
        } else {
            // The day item was released, and its address is in use again.
            it->item = winrt::make_weak(item);
        }
        return;
    }

    if (t_dayItems.size() >= kMaxTrackedDayItems) {
        if (!t_loggedTrackLimit) {
            t_loggedTrackLimit = true;
            Wh_Log(L"Tracking is at its limit of %zu day items",
                   kMaxTrackedDayItems);
        }
        t_dayItems.erase(t_dayItems.begin());
    }
    t_dayItems.push_back({id, winrt::make_weak(item)});
}

// Whether two brushes are the same object. This is what tells the mod's own colour
// apart from the colour of the shell: two brushes which hold the same colour are
// still two different brushes, and only the object shows which one it is.
bool SameBrush(wuxm::Brush const& left, wuxm::Brush const& right) {
    return left && right && winrt::get_abi(left) == winrt::get_abi(right);
}

// Whether the colour the label carries right now is the one the mod itself wrote.
// The effective value is compared rather than the local one: a local value which is
// not a brush (a binding, for example) resolves to the brush below and is never the
// mod's own, which is the answer this is used for.
bool IsOwnBrush(wuxc::TextBlock const& textBlock, wuxm::Brush const& ownBrush) {
    if (!ownBrush) {
        return false;
    }
    try {
        return SameBrush(textBlock.Foreground(), ownBrush);
    } catch (...) {
        return false;
    }
}

// The foreground is written only when it differs: a write which changes nothing
// still costs a layout pass, and it would count as a write for the sweep which is
// watching the shell rewrite the cells (see kSweepQuietTicksToStop).
// A new SolidColorBrush is created instead of the shell's brush being modified: the
// shell may share one brush between several elements of a template, and changing it
// would change them as well. The shell's own brush is kept as it is and put back on
// the day the mod stops marking the cell.
//
// The brush which is on the label afterwards is returned, so that the caller can
// tell the mod's own colour from the shell's later. A brush which was already
// there counts as the mod's own only when it is the very brush written before
// (previousOwn): claiming a brush of the shell's which happens to hold the same
// colour would clear the shell's own value on the day the mod stops marking.
wuxm::Brush SetForegroundIfDifferent(wuxc::TextBlock const& textBlock,
                                     winrt::Windows::UI::Color const& color,
                                     wuxm::Brush const& previousOwn) {
    try {
        if (auto current = textBlock.Foreground().try_as<wuxm::SolidColorBrush>()) {
            if (current.Color() == color) {
                return SameBrush(current, previousOwn) ? previousOwn : nullptr;
            }
        }
        wuxm::SolidColorBrush brush(color);
        textBlock.Foreground(brush);
        t_applyWrote = true;
        return brush;
    } catch (...) {
        return nullptr;
    }
}

// Remembers the colour the label carried before the mod wrote its own, once per
// label element. The value is read from the local-value store instead of by
// reading the property back: reading the property back gives the effective value,
// which is the mod's own brush once the shell has rewritten the text of a cell the
// mod has already coloured, and which would also be pinned as a local value of the
// label although it never was one.
//
// Why the local value is read: the shell's Foreground can be a local value written on
// the element, which is how the days of the neighbouring months are dimmed, or a style
// and inherited value, and telling those two apart is what keeps the restore from
// dropping the first one to the colour of its parent. A local value which is not a
// brush, a binding for instance, counts as no local value: the mod only has to take its
// own layer of colour off.
void SaveForeground(DayCell& cell, wuxc::TextBlock const& label) {
    if (cell.hasSavedForeground) {
        return;
    }
    try {
        // A brush of the mod's own is not an original colour: it is what an earlier
        // pass wrote, and saving it would make the mod put its own blue back on the
        // day the cell stops being a holiday. A recycled day item is what runs into
        // this, because the shell gives it its new date without touching a colour
        // the mod has already put on the label. Nothing is saved then, and a later
        // pass saves whatever the shell has set by the time it looks again; as long
        // as nothing was saved, the restore falls back to clearing the property.
        if (IsOwnBrush(label, cell.ownBrush)) {
            return;
        }
        wf::IInspectable value =
            label.ReadLocalValue(wuxc::TextBlock::ForegroundProperty());
        if (auto brush = value.try_as<wuxm::Brush>()) {
            cell.savedForeground = brush;
            cell.savedForegroundWasLocal = true;
        }
        cell.hasSavedForeground = true;
    } catch (...) {
        // Nothing was saved: the colour is then taken back by clearing the
        // property, which is what the cell falls back to anyway.
    }
}

// Puts the colour of the label back the way the shell had it. A label whose
// colour came from a local value gets that value back; a label which had none gets
// the property cleared, so that the style and the inheritance it used before apply
// again.
void RestoreForeground(wuxc::TextBlock const& label, DayCell const& cell) {
    try {
        // Only a colour the mod itself wrote is taken back. A value the shell has
        // set since - the dimmed colour of the days of the neighbouring months, for
        // example - is left alone: the mod's colour is already gone from the label
        // by then, and clearing the property would take the shell's own value away
        // with it.
        if (!IsOwnBrush(label, cell.ownBrush)) {
            return;
        }
        if (cell.savedForegroundWasLocal && cell.savedForeground) {
            label.Foreground(cell.savedForeground);
        } else {
            label.ClearValue(wuxc::TextBlock::ForegroundProperty());
        }
        t_applyWrote = true;
    } catch (...) {
    }
}

// The shell's lunar text of a cell which was reused for another month is written
// after the mod has put the holiday name there, and that would take the name away
// again. The text callback is what catches it: when the text is not the one the
// mod wrote, the cell is applied to once more.
// The callback only applies to the cell once more; the real decisions are all in
// ApplyToDayItem, which refuses to re-enter while it is running, so the write the mod
// does itself does not come back as another change.
void OnLabelTextChanged(wux::DependencyObject const& sender) {
    auto it = t_labelOwners.find(winrt::get_abi(sender));
    if (it == t_labelOwners.end()) {
        return;
    }
    if (auto item = it->second.get()) {
        ApplyToDayItem(item);
        // The shell has just written its own text over the mod's: the moment the sweep
        // exists for, so it gets the full quiet period again.
        StartSweepTimer();
    }
}

// Whether a text is one the mod could have written. A cell which the shell reused
// for another month still holds the mod's text of the old date, and that text is
// not worth remembering as the shell's own.
// appliedText cannot cover that window, because the shell recycles a cell by clearing
// what it holds while the mod's own holiday name is still on it; the name is then the
// only thing left to recognise. The price is that a lunar text which happens to equal a
// holiday name is not saved as the shell's own, which is harmless: the text the mod wants
// to write is the same one.
//
// A text of a rule which holds no {name} is recognised as it is written; a rule whose
// text is built from the name of the event is not, and neither is it in the version
// before the rules - what is written then is the name itself, which is covered by the
// second test below.
bool IsOwnText(std::wstring const& text, Settings const& settings) {
    if (text.empty()) {
        return false;
    }
    for (const auto& rule : settings.rules) {
        if (!rule.text.empty() && rule.text.find(L"{name}") == std::wstring::npos &&
            rule.text == text) {
            return true;
        }
    }

    std::lock_guard<std::mutex> lock(g_holidaysMutex);
    for (const auto& entry : g_holidays) {
        if (entry.second.name == text) {
            return true;
        }
    }

    return false;
}

// The text a cell should show. The text of a rule may contain {name}, which stands for
// the name of the event, and an empty text - a rule which has no text field - means
// "leave the text alone and only colour the day".
std::wstring ExpandName(std::wstring const& text, std::wstring const& name) {
    if (text.empty()) {
        return {};
    }

    // Every "{name}" is replaced: a text which says it twice means it twice. The result
    // is built up instead of replacing in place so that a name which itself contains
    // "{name}" cannot send the loop around forever.
    std::wstring result;
    size_t pos = 0;
    for (;;) {
        size_t found = text.find(L"{name}", pos);
        if (found == std::wstring::npos) {
            result.append(text, pos, std::wstring::npos);
            break;
        }
        result.append(text, pos, found - pos);
        result += name;
        pos = found + 6;
    }
    return result;
}

// One day cell, in full. The name of the event which covers this date, and the text and
// the colour of the rule which marked it, go into the cell's lunar text block, which is
// all the shell's day cell has room for:
//   1) turn the cell's Date into a yyyymmdd key;
//   2) look the key up, which says what marks the day and what that day is called;
//   3) find the lunar text block, anew on every pass;
//   4) decide the text: keep the lunar text, or write the text of the rule which marked
//      the day;
//   5) decide the colour: the colour of that same rule - and put the shell's own colour
//      back for a day which is marked no more, or which a rule marks by text alone;
//   6) remember what was written, so that it can be restored later.
//
// Nothing is written while the cell already shows the wanted value: every write costs a
// layout pass, and a write which changed nothing would also keep the sweep running for
// another round (see kSweepQuietTicksToStop).
void ApplyToDayItem(wuxc::CalendarViewDayItem const& item) {
    const std::shared_ptr<const Settings> settings = GetSettings();
    if (!item) {
        return;
    }

    int32_t key = DateKeyFromDateTime(item.Date());
    if (!key) {
        return;
    }

    DayCell& cell = CellFor(item);
    if (cell.busy) {
        return;
    }
    cell.busy = true;
    struct BusyGuard {
        bool& busy;
        ~BusyGuard() { busy = false; }
    } guard{cell.busy};

    // A day item which now shows another date was filled in by the shell again,
    // so its text is the shell's own and there is nothing left to restore. The
    // colour state stays, because it belongs to the label element, which is still
    // the same one and still carries the mod's brush.
    //
    // The saved colour value is dropped, but the brush the mod wrote itself
    // (ownBrush) is kept: the shell re-templates a recycled cell for its new date
    // without touching a colour the mod has put there, so a value saved for the old
    // date may be stale, while ownBrush is what still tells the mod's own colour
    // from a colour the shell has set for the new date. That is what keeps
    // SaveForeground() from saving the mod's own blue as the original colour of the
    // new date, and RestoreForeground() from clearing a colour the shell has put
    // there in the meantime.
    if (cell.key != key) {
        cell.key = key;
        cell.savedText.clear();
        cell.appliedText.clear();
        cell.hasSavedText = false;
        cell.savedForeground = nullptr;
        cell.hasSavedForeground = false;
        cell.savedForegroundWasLocal = false;
    }

    HolidayInfo info;
    const bool hasInfo = LookupHoliday(key, info);

    // The shell recycles day items, and a recycled item keeps whatever the mod
    // put into it, so a new date is applied to as well.
    if (cell.dateChangedToken == 0) {
        try {
            cell.dateChangedToken = item.RegisterPropertyChangedCallback(
                wuxc::CalendarViewDayItem::DateProperty(),
                [](wux::DependencyObject const& sender,
                   wux::DependencyProperty const&) {
                    if (auto dayItem =
                            sender.try_as<wuxc::CalendarViewDayItem>()) {
                        ApplyToDayItem(dayItem);
                        StartSweepTimer();
                    }
                });
        } catch (...) {
            Wh_Log(L"Failed to register the date callback: %08X",
                   winrt::to_hresult());
        }
    }

    // The shell's lunar/solar-term text block.
    wuxc::TextBlock label = FindLabelTextBlock(item, cell.labelId);
    if (!label) {
        if (DayItemHasContent(item) && !t_loggedMissingTextBlock) {
            t_loggedMissingTextBlock = true;
            Wh_Log(L"No text block to replace in the day cell of %d - the cell has no "
                   L"second line, which is where the mod writes, so the additional "
                   L"calendar of the taskbar has to be switched on",
                   key);
            LogDayItemStructure(item);
        }
        return;
    }

    // A text block which wasn't seen before: register the callback which notices
    // the shell writing its own text, so that the holiday name survives it.
    if (cell.label.get() != label) {
        if (auto previous = cell.label.get()) {
            if (cell.labelChangedToken) {
                try {
                    previous.UnregisterPropertyChangedCallback(
                        wuxc::TextBlock::TextProperty(), cell.labelChangedToken);
                } catch (...) {
                }
            }
            t_labelOwners.erase(winrt::get_abi(previous));
        }
        cell.labelChangedToken = 0;

        cell.label = winrt::make_weak(label);
        cell.labelId = winrt::get_abi(label);
        cell.savedText.clear();
        cell.appliedText.clear();
        cell.hasSavedText = false;
        cell.savedForeground = nullptr;
        cell.hasSavedForeground = false;
        cell.savedForegroundWasLocal = false;
        // A text block the shell has replaced carries no colour of the mod's, so
        // nothing of the mod is left on it to recognise or to take back.
        cell.ownBrush = nullptr;
        cell.hadColor = false;
        t_labelOwners[cell.labelId] = winrt::make_weak(item);

        try {
            cell.labelChangedToken = label.RegisterPropertyChangedCallback(
                wuxc::TextBlock::TextProperty(),
                [](wux::DependencyObject const& sender,
                   wux::DependencyProperty const&) {
                    OnLabelTextChanged(sender);
                });
        } catch (...) {
            Wh_Log(L"Failed to register the text callback: %08X",
                   winrt::to_hresult());
        }

    }

    std::wstring desired;
    // What the cell shows is what the rule which marked the day says, and nothing else:
    // a rule whose text field was left out asks for the text of the shell to stay, which
    // ExpandName() answers with nothing for - the day is then marked by colour alone.
    if (hasInfo) {
        desired = ExpandName(info.text, info.name);
    }

    std::wstring current = ElementText(label);

    if (!desired.empty() && current != desired) {
        if (current != cell.appliedText && !IsOwnText(current, *settings)) {
            // The shell's own text is being replaced: remember it.
            cell.savedText = current;
            cell.hasSavedText = true;
        }
        try {
            label.Text(desired);
            t_applyWrote = true;
            Wh_Log(L"Day %d: \"%s\" -> \"%s\"", key, current.c_str(),
                   desired.c_str());
        } catch (...) {
        }
    } else if (desired.empty() && !cell.appliedText.empty() &&
               current == cell.appliedText) {
        // The mod's text is not wanted any more: put the shell's own text back.
        try {
            if (cell.hasSavedText) {
                label.Text(cell.savedText);
                t_applyWrote = true;
            }
        } catch (...) {
        }
        cell.hasSavedText = false;
    }

    cell.appliedText = desired;

    // The colour is what marks the day, whichever text the cell ends up showing, so it
    // does not depend on the text having been replaced. A rule which asks for no colour
    // leaves the colour of the cell alone - and takes a colour of this mod's own back,
    // for a cell which carried one from the rules before. The shell's own colour is
    // saved right before the first colour write: reading the colour back later would
    // save the mod's own brush as the original one once the shell has rewritten the text
    // of a cell the mod has already coloured.
    if (hasInfo && info.color) {
        SaveForeground(cell, label);
        cell.ownBrush = SetForegroundIfDifferent(label, *info.color, cell.ownBrush);
        cell.hadColor = true;
    } else if (cell.hadColor) {
        RestoreForeground(label, cell);
        cell.hadColor = false;
    }

}

////////////////////////////////////////////////////////////////////////////////
// Keeping the calendar up to date

// The shell writes the lunar text of a day cell which it reused for another month
// asynchronously - after the mod has put the holiday name there - and it may
// replace the text block as well, in which case the mod's text and its text
// callback are gone. The cells are therefore looked at again a few times after
// everything which can start such a rewrite. Only a cell whose text the shell took
// back is written to, so a pass over an unchanged calendar costs a few comparisons.
thread_local winrt::Windows::System::DispatcherQueueTimer t_sweepTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    t_sweepTimerRevoker;

constexpr int kSweepIntervalMs = 1000;

// How many passes without a write end the sweep. One second per pass, so the calendar is
// watched for about five seconds after the last write. Why a limit at all: the shell keeps
// the day cells of the flyout alive after it is closed, so a timer which only stops when
// the tracked list becomes empty would wake the shell process once a second for the rest
// of the session. Why five seconds are enough: the rewrite the sweep exists for follows
// an event the mod already sees - a day item being put back into the calendar, the date
// of a cell changing, or the text of a cell changing - and each of those starts the timer
// again with the full period. Do not shorten the interval, which would touch the shell's
// elements more often, and do not drop the sweep, which is what covers a text block the
// shell replaced together with its callback.
constexpr int kSweepQuietTicksToStop = 5;

thread_local int t_sweepQuietTicks = 0;

void StopSweepTimer() {
    t_sweepQuietTicks = 0;

    if (t_sweepTimer) {
        try {
            t_sweepTimer.Stop();
        } catch (...) {
        }
    }
}

// Starts the periodic re-check again, if it isn't running already, and gives the quiet
// count a fresh start: this is called from everything which the shell's late rewrite
// follows (see kSweepQuietTicksToStop).
// The re-check is also what finds the day cells, so a caller which knows that the
// calendar can have changed - a window which appeared, an activation, a changed setting -
// starts it even when no day item is tracked yet, which is the state before a calendar
// was found for the first time. Nothing here reads the calendar: a caller which wants the
// marks right away rather than on the next pass calls SweepDayItems() as well.
void StartSweepTimer() {
    t_sweepQuietTicks = 0;

    try {
        if (!t_sweepTimer) {
            auto dispatcherQueue =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (!dispatcherQueue) {
                Wh_Log(L"No dispatcher queue: the calendar cannot be watched");
                return;
            }

            t_sweepTimer = dispatcherQueue.CreateTimer();
            t_sweepTimer.Interval(std::chrono::milliseconds{kSweepIntervalMs});
            t_sweepTimerRevoker = t_sweepTimer.Tick(
                winrt::auto_revoke,
                [](winrt::Windows::System::DispatcherQueueTimer const&,
                   wf::IInspectable const&) { SweepDayItems(); });
        }

        if (!t_sweepTimer.IsRunning()) {
            t_sweepTimer.Start();
        }
    } catch (...) {
        Wh_Log(L"Failed to create the sweep timer: %08X", winrt::to_hresult());
    }
}

////////////////////////////////////////////////////////////////////////////////
// Finding the calendar and its day cells

// The CoreWindow of this thread, with the two events which say that the calendar can
// have been opened. They are taken lazily and given back on the same thread: a handler
// is a delegate into this module's image, so one which is left behind makes the shell
// call into an unmapped module the next time the flyout is opened.
struct CoreWindowTriggers {
    wuc::CoreWindow coreWindow{nullptr};
    winrt::event_token activatedToken{};
    winrt::event_token visibilityChangedToken{};
};

thread_local CoreWindowTriggers t_windowTriggers;

// The calls above arrive in bursts - an activation, a change of visibility and the
// window being shown are one open - so they are debounced (see
// OnCalendarOpenedOnCurrentThread).
constexpr ULONGLONG kCalendarOpenedDebounceMs = 500;
thread_local ULONGLONG t_lastCalendarOpenedTick = 0;

void OnCalendarOpenedOnCurrentThread();

// Defined with the rest of the process plumbing.
void InitializeForCurrentThread();

// Takes the activation events of this thread's CoreWindow, once. A CoreWindow which is
// not there yet is neither an error nor the end of it: the ShellHost target has none at
// all, and when a CoreWindow is created the object is not necessarily there the moment
// its window is (the window hook runs before XAML gets that far), so this is asked for
// again on every pass of the sweep until it works.
void RegisterCalendarTriggersOnCurrentThread() {
    try {
        if (t_windowTriggers.coreWindow) {
            return;
        }

        auto coreWindow = wuc::CoreWindow::GetForCurrentThread();
        if (!coreWindow) {
            return;
        }

        // The window is remembered before the events are registered, so that a
        // registration which throws halfway through still has the tokens it did get
        // revoked when the thread is uninitialized.
        t_windowTriggers.coreWindow = coreWindow;
        t_windowTriggers.activatedToken = coreWindow.Activated(
            [](wuc::CoreWindow const&, wuc::WindowActivatedEventArgs const& args) {
                if (args.WindowActivationState() !=
                    wuc::CoreWindowActivationState::Deactivated) {
                    OnCalendarOpenedOnCurrentThread();
                }
            });
        t_windowTriggers.visibilityChangedToken = coreWindow.VisibilityChanged(
            [](wuc::CoreWindow const&,
               wuc::VisibilityChangedEventArgs const& args) {
                if (args.Visible()) {
                    OnCalendarOpenedOnCurrentThread();
                }
            });

        Wh_Log(L"Watching the window of thread %u", GetCurrentThreadId());
    } catch (...) {
        Wh_Log(L"Failed to register the window events: %08X", winrt::to_hresult());
    }
}

// Gives the events back, on the thread which took them.
void UnregisterCalendarTriggersOnCurrentThread() {
    try {
        auto coreWindow = t_windowTriggers.coreWindow;
        if (coreWindow) {
            if (t_windowTriggers.activatedToken.value != 0) {
                coreWindow.Activated(t_windowTriggers.activatedToken);
            }
            if (t_windowTriggers.visibilityChangedToken.value != 0) {
                coreWindow.VisibilityChanged(
                    t_windowTriggers.visibilityChangedToken);
            }
        }
    } catch (...) {
    }

    t_windowTriggers = {};
}

// A window which was just shown does not have its calendar yet: the day cells come with the
// layout which follows. The root element is watched until they are found, which is what
// makes the marks appear together with the calendar rather than up to one pass of the sweep
// later - the walk of the sweep is the fallback for a window whose layout does not change
// again, and for a flyout which keeps the day cells it already has.
thread_local wux::FrameworkElement t_layoutUpdatedElement{nullptr};
thread_local winrt::event_token t_layoutUpdatedToken{};
thread_local ULONGLONG t_layoutUpdatedStartTick = 0;
thread_local ULONGLONG t_lastLayoutUpdatedPassTick = 0;

// How long a watch stays armed, and how often it is allowed to walk. The window is not
// watched forever: a calendar which is not found in this time is not there, and the walk
// is repeated by the sweep anyway.
constexpr ULONGLONG kLayoutUpdatedWatchMs = 5000;
constexpr ULONGLONG kLayoutUpdatedThrottleMs = 100;

void RevokeLayoutUpdatedWatch() {
    if (t_layoutUpdatedToken.value != 0) {
        auto element = t_layoutUpdatedElement;
        if (element) {
            try {
                element.LayoutUpdated(t_layoutUpdatedToken);
            } catch (...) {
            }
        }
    }

    t_layoutUpdatedToken = {};
    t_layoutUpdatedElement = nullptr;
}

void OnCalendarLayoutUpdatedOnCurrentThread() {
    const ULONGLONG now = GetTickCount64();
    if (now - t_layoutUpdatedStartTick > kLayoutUpdatedWatchMs) {
        RevokeLayoutUpdatedWatch();
        return;
    }

    // A layout pass of the shell arrives in bursts as well, and one walk per burst says
    // the same thing as one walk per pass.
    if (now - t_lastLayoutUpdatedPassTick < kLayoutUpdatedThrottleMs) {
        return;
    }
    t_lastLayoutUpdatedPassTick = now;

    SweepDayItems();

    // Found: the marks are on the calendar, and the sweep is what watches it from here.
    if (!t_dayItems.empty()) {
        RevokeLayoutUpdatedWatch();
    }
}

// Arms the watch above for the window of this thread. Called with every open, so that a
// flyout which was closed and shown again is watched again; an armed watch is left alone.
void WatchCalendarLayoutOnCurrentThread() {
    if (t_layoutUpdatedToken.value != 0) {
        return;
    }

    try {
        auto window = wux::Window::Current();
        if (!window) {
            return;
        }

        auto root = window.Content().try_as<wux::FrameworkElement>();
        if (!root) {
            return;
        }

        t_layoutUpdatedElement = root;
        t_layoutUpdatedStartTick = GetTickCount64();
        t_lastLayoutUpdatedPassTick = 0;
        t_layoutUpdatedToken = root.LayoutUpdated(
            [](wf::IInspectable const&, wf::IInspectable const&) {
                OnCalendarLayoutUpdatedOnCurrentThread();
            });
    } catch (...) {
        RevokeLayoutUpdatedWatch();
    }
}

// How far a walk over a window goes. The limits are wide for a tree the calendar is in -
// the notification list above it can be long - and what they guard against is the window
// which has no calendar at all (the hardware flyouts of the same process share this code)
// and a tree the shell is changing while it is being read. A walk which stops at these
// limits is a walk which found nothing, not a wrong answer, and the next pass tries again.
constexpr size_t kMaxScannedElements = 16384;
constexpr int kMaxScanDepth = 96;

// A thread without a XAML window is worth reporting once: "the mod does nothing" is
// otherwise hard to tell from "this month has no holidays".
thread_local bool t_loggedNoWindow = false;

void CollectDayItemsFromElement(wux::DependencyObject const& root,
                                size_t& budget,
                                int depth) {
    if (!root || budget == 0 || depth > kMaxScanDepth) {
        return;
    }
    budget--;

    // A day cell is neither looked into nor looked past: what is inside it is the day
    // number and the lunar text of that one day, and there is no calendar below it.
    if (auto dayItem = root.try_as<wuxc::CalendarViewDayItem>()) {
        TrackDayItem(dayItem);
        return;
    }

    int count = 0;
    try {
        count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return;
    }

    for (int childIndex = 0; childIndex < count; childIndex++) {
        wux::DependencyObject child = nullptr;
        try {
            child = wuxm::VisualTreeHelper::GetChild(root, childIndex);
        } catch (...) {
            continue;
        }

        CollectDayItemsFromElement(child, budget, depth + 1);
    }
}

// Finds the day cells of the calendar by walking the XAML tree of the window of this
// thread, and tracks them: the pass which follows applies the holiday data to them.
//
// This walk is what replaced the XAML diagnostics. The diagnostics reported every day
// cell as it appeared, which is precise, but it costs a diagnostics consumer for the
// whole session and that is a consumer no other mod of this process can have. A walk on
// every pass of the sweep costs a walk of the calendar tree for the few seconds the sweep
// runs after a calendar can have changed, and it does not miss a cell the shell recycles,
// because it does not remember cells: it looks at what is there.
//
// Runs on the UI thread of a calendar window.
void CollectDayItemsOnCurrentThread() {
    wux::Window window = nullptr;
    try {
        window = wux::Window::Current();
    } catch (...) {
        return;
    }

    if (!window) {
        if (!t_loggedNoWindow) {
            t_loggedNoWindow = true;
            Wh_Log(L"No XAML window on thread %u", GetCurrentThreadId());
        }
        return;
    }

    try {
        size_t budget = kMaxScannedElements;

        if (auto content = window.Content()) {
            CollectDayItemsFromElement(content, budget, 0);
        }

        // A flyout can be a popup rather than part of the window's content.
        for (auto popup : wuxm::VisualTreeHelper::GetOpenPopups(window)) {
            if (budget == 0) {
                break;
            }
            if (auto child = popup.Child()) {
                CollectDayItemsFromElement(child, budget, 0);
            }
        }
    } catch (...) {
    }
}

// Everything which says that the calendar of this thread can have been opened or changed:
// a window which appeared, an activation, the window being shown, a changed setting. The
// cells are marked right away rather than on the next pass of the sweep, so that the
// flyout does not open with the marks missing for a second.
// Runs on the UI thread of a calendar window.
void OnCalendarOpenedOnCurrentThread() {
    // Every thread whose state this arms is initialized here, so that the unload takes that
    // state apart again: this path is reached from a window which was shown, and that is not
    // necessarily a window the CreateWindowInBand(Ex) hooks ever saw.
    InitializeForCurrentThread();

    const ULONGLONG now = GetTickCount64();
    if (now - t_lastCalendarOpenedTick < kCalendarOpenedDebounceMs) {
        return;
    }
    t_lastCalendarOpenedTick = now;

    RegisterCalendarTriggersOnCurrentThread();
    SweepDayItems();
    StartSweepTimer();
    WatchCalendarLayoutOnCurrentThread();
    RequestHolidayData();
}

// Applies the mod's data to every day item of this thread. Runs on every timer
// tick, and after the data or the settings changed.
// Again, only a cell which does not show what the mod wants is written to, so a pass
// over an unchanged calendar does nothing at all.
void SweepDayItems() {
    // Everything a pass writes sets this, so that a pass which only reads can be told
    // from one which changed something (see kSweepQuietTicksToStop).
    t_applyWrote = false;

    // A pass arms state on this thread - the window events, the tracked cells and their
    // callbacks, the timer which called it - so the thread is initialized first: the unload
    // only takes apart what it finds on a thread which is marked as initialized.
    InitializeForCurrentThread();

    // The day cells are looked for on every pass rather than remembered from a report, and
    // the window events are asked for again until the CoreWindow of this thread is there
    // to be watched.
    RegisterCalendarTriggersOnCurrentThread();
    CollectDayItemsOnCurrentThread();

    // The list is only indexed, never iterated over: applying to a day item can
    // put another one into it.
    for (size_t i = 0; i < t_dayItems.size();) {
        auto item = t_dayItems[i].item.get();
        if (!item) {
            t_dayItems.erase(t_dayItems.begin() + i);
            continue;
        }

        ApplyToDayItem(item);
        i++;
    }

    for (auto it = t_cells.begin(); it != t_cells.end();) {
        if (it->second->item.get()) {
            ++it;
        } else {
            it = t_cells.erase(it);
        }
    }

    for (auto it = t_labelOwners.begin(); it != t_labelOwners.end();) {
        if (it->second.get()) {
            ++it;
        } else {
            it = t_labelOwners.erase(it);
        }
    }

    // A pass which wrote nothing is one more reason to believe the shell is done with
    // rewriting the cells, and the timer stops once enough of them follow each other.
    // Without that the timer would run for the rest of the session, because the shell
    // keeps the day cells of a closed flyout alive.
    // A pass which found no day item at all counts as a quiet one instead of stopping the
    // timer on the spot: the calendar of a window which was just created is usually not
    // built yet when it is first asked, and the pass which finds it comes later.
    if (t_applyWrote) {
        t_sweepQuietTicks = 0;
    } else if (++t_sweepQuietTicks >= kSweepQuietTicksToStop) {
        StopSweepTimer();
    }
}

// Undoes everything the mod did to the day cells of this thread: unregisters the two
// property callbacks, writes the saved text and colour back, clears the state and stops
// the timer. Only cells which are still alive are handled; one the shell has destroyed
// is filled in by the shell itself. Nothing here changes the element tree, so the worst
// case is that the text and the colour are repainted a moment later.
void UninitializeDayItemsForCurrentThread() {
    for (auto& entry : t_cells) {
        DayCell& cell = *entry.second;
        auto item = cell.item.get();
        if (!item) {
            continue;
        }

        if (cell.dateChangedToken != 0) {
            try {
                item.UnregisterPropertyChangedCallback(
                    wuxc::CalendarViewDayItem::DateProperty(),
                    cell.dateChangedToken);
            } catch (...) {
            }
            cell.dateChangedToken = 0;
        }

        if (auto label = cell.label.get()) {
            if (cell.labelChangedToken) {
                try {
                    label.UnregisterPropertyChangedCallback(
                        wuxc::TextBlock::TextProperty(), cell.labelChangedToken);
                } catch (...) {
                }
                cell.labelChangedToken = 0;
            }
            try {
                // The text goes back only when the shell's own text was saved.
                // A cell whose text the mod wrote without saving it keeps the
                // holiday name until the shell repaints it, which it does when
                // the calendar is opened again.
                if (cell.hasSavedText) {
                    label.Text(cell.savedText);
                }
                if (cell.hadColor) {
                    RestoreForeground(label, cell);
                }
            } catch (...) {
            }
        }
    }

    // Swapped with an empty container instead of cleared: clear() keeps the buckets and
    // the capacity on a UI thread of the shell, which outlives the mod, so the memory of
    // a full calendar would stay allocated after every unload. (Assigning "{}" would do
    // the same, but for a map whose mapped type is a unique_ptr it picks the
    // initializer-list assignment, which does not compile.)
    decltype(t_cells)().swap(t_cells);
    decltype(t_dayItems)().swap(t_dayItems);
    decltype(t_labelOwners)().swap(t_labelOwners);
    t_sweepQuietTicks = 0;

    try {
        if (t_sweepTimer) {
            t_sweepTimer.Stop();
        }
    } catch (...) {
    }
    t_sweepTimerRevoker.revoke();
    t_sweepTimer = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Process plumbing

enum class Target {
    ShellExperienceHost,
    ShellHost,
};

Target g_target = Target::ShellExperienceHost;

// The message RunFromWindowThreadViaPostMessage posts. Registered by Wh_ModInit before any
// hook of this mod can post, and kept here rather than inside that function because the
// unload asks for the same message: a post which is still queued when the mod is unloaded
// has to be retrieved while this module is still mapped (see
// DrainPendingRunFromWindowThreadPostsOnCurrentThread). A read which saw the zero of a
// registration which never happened would skip that drain, so the variable is atomic: the
// hook procedures and the unload read it on other threads than the one which registers it.
std::atomic<UINT> g_runFromWindowThreadViaPostMessageMsg{0};

// Registers the message if it is not registered yet. Wh_ModInit calls this once, where there
// is a single thread and no hook of this mod can be running; the registration left here is
// the fallback for a load which never got that far.
UINT RunFromWindowThreadViaPostMessageMsg() {
    UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
    if (!message) {
        message =
            RegisterWindowMessage(L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);
        g_runFromWindowThreadViaPostMessageMsg.store(message);
    }

    return message;
}

// The hooks of the posts which have not been retrieved yet. A post whose window is gone
// before its thread retrieves it is dropped with its hook still installed, and a hook whose
// procedure is in a module which is being unmapped takes the shell down with it, so
// Wh_ModUninit unhooks whatever is left.
//
// The parameter of a post which is unhooked that way is left to the process: whether the
// hook procedure is running at that moment cannot be told from here, and freeing a
// parameter which it is about to free itself would be a double free.
std::mutex g_pendingPostHooksMutex;
std::vector<HHOOK> g_pendingPostHooks;

void RememberPendingPostHook(HHOOK hook) {
    std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
    g_pendingPostHooks.push_back(hook);
}

// Called from the hook procedure, before the callback runs: the hook is the procedure's own
// from that point on, and an unload which runs at the same time must not unhook it as well.
void ForgetPendingPostHook(HHOOK hook) {
    std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
    for (auto it = g_pendingPostHooks.begin(); it != g_pendingPostHooks.end(); ++it) {
        if (*it == hook) {
            g_pendingPostHooks.erase(it);
            break;
        }
    }
}

void UnhookPendingPostHooks() {
    std::vector<HHOOK> hooks;
    {
        std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
        hooks.swap(g_pendingPostHooks);
    }

    for (HHOOK hook : hooks) {
        Wh_Log(L"Unhooking a post which was never retrieved: %08X",
               (DWORD)(ULONG_PTR)hook);
        UnhookWindowsHookEx(hook);
    }
}

// Retrieves the posts of RunFromWindowThreadViaPostMessage which are still queued for this
// thread, so that their callbacks run now rather than after this thread was torn down: a
// callback which ran then would arm the window events, a sweep timer, the layout watch and
// the day cells of a thread the unload has already taken apart, nothing would take them
// apart again, and the sweep timer it left behind would fire into the unmapped module.
// Retrieving the message runs the WH_GETMESSAGE hook of the post, and that hook procedure is
// what runs the callback, removes the hook and frees its parameter. The teardown which
// follows in UninitializeForCurrentThread then sees whatever the callback armed.
//
// PeekMessage dispatches the sent messages of this thread as well, which is harmless here:
// this runs on a thread which is alive, and the sent messages of this thread are the other
// RunFromWindowThread calls.
void DrainPendingRunFromWindowThreadPostsOnCurrentThread() {
    const UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
    if (!message) {
        return;
    }

    MSG msg;
    while (PeekMessage(&msg, nullptr, message, message, PM_REMOVE)) {
    }
}

void InitializeForCurrentThread() {
    if (g_initializedForThread) {
        return;
    }

    g_initializedForThread = true;

    // The events of this thread's window, taken here and asked for again on every pass of
    // the sweep: a CoreWindow is not necessarily there yet when its window is.
    RegisterCalendarTriggersOnCurrentThread();
}

void UninitializeForCurrentThread() {
    // Before the flag is looked at: a post which is still queued for this thread runs its
    // callback here, while this module is still mapped, and the state that callback arms is
    // then taken apart by the teardown below - which is why the callback initializes the
    // thread it runs on (see OnCalendarOpenedOnCurrentThread).
    DrainPendingRunFromWindowThreadPostsOnCurrentThread();

    if (!g_initializedForThread) {
        return;
    }

    // On this thread: the window events are delegates into this module, and the day cells
    // are XAML objects of the thread they were found on.
    UnregisterCalendarTriggersOnCurrentThread();
    RevokeLayoutUpdatedWatch();
    UninitializeDayItemsForCurrentThread();

    g_initializedForThread = false;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

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

bool RunFromWindowThreadViaPostMessage(HWND hWnd,
                                       RunFromWindowThreadProc_t proc,
                                       PVOID procParam) {
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
        HHOOK hook;
    };

    const UINT message = RunFromWindowThreadViaPostMessageMsg();

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_GETMESSAGE,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION && wParam == PM_REMOVE) {
                MSG* msg = (MSG*)lParam;
                if (msg->message ==
                    g_runFromWindowThreadViaPostMessageMsg.load()) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)msg->lParam;
                    if (param) {
                        ForgetPendingPostHook(param->hook);
                        param->proc(param->procParam);
                        UnhookWindowsHookEx(param->hook);
                        delete param;
                        msg->lParam = 0;
                    }
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    auto* param = new RUN_FROM_WINDOW_THREAD_PARAM{
        .proc = proc,
        .procParam = procParam,
        .hook = hook,
    };
    RememberPendingPostHook(hook);
    if (!PostMessage(hWnd, message, 0, (LPARAM)param)) {
        ForgetPendingPostHook(hook);
        UnhookWindowsHookEx(hook);
        delete param;
        return false;
    }

    return true;
}

// Whether a window of this process is one the calendar is looked for in. The two targets
// name their window differently: ShellExperienceHost uses the CoreWindow of the flyout,
// ShellHost the ControlCenterWindow the newer builds moved it to.
bool IsCalendarWindowClass(PCWSTR className) {
    switch (g_target) {
        case Target::ShellExperienceHost:
            return _wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0;

        case Target::ShellHost:
            return _wcsicmp(className, L"ControlCenterWindow") == 0;
    }

    return false;
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName, PCSTR funcName) {
    // Nothing is posted while the mod is being unloaded, for the same reason as in
    // OnWindowShown.
    if (g_shuttingDown) {
        return;
    }

    // The class name is only a string when the caller passed one: the same call also
    // accepts a numeric atom, which names no class to compare against.
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (!bTextualClassName || !IsCalendarWindowClass(lpClassName)) {
        return;
    }

    Wh_Log(L"Initializing - created calendar window: %08X via %S",
           (DWORD)(ULONG_PTR)hWnd, funcName);

    // Posted rather than sent, for the reason the ShellHost target always had: the window
    // exists before its XAML does, so this runs once the thread is free to run it. The
    // first pass of it usually finds nothing yet - the calendar is built when the flyout
    // is opened - which is what the sweep it arms is for.
    RunFromWindowThreadViaPostMessage(
        hWnd,
        [](PVOID) {
            InitializeForCurrentThread();
            OnCalendarOpenedOnCurrentThread();
        },
        nullptr);
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
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
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle,
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
                                      DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);
    return hWnd;
}

// The window being shown is the moment the flyout is opened, and it is the signal both
// targets have in common: the newer builds moved the calendar into a window which is not
// a CoreWindow, so the activation events are not available there. A hide is ignored - the
// marks are not taken back when the flyout is closed, the shell repaints the cells itself.
void OnWindowShown(HWND hWnd) {
    // Nothing is posted while the mod is being unloaded: the post would have to be drained
    // or unhooked again, and the calendar is not marked after this point anyway.
    if (g_shuttingDown) {
        return;
    }

    DWORD dwProcessId = 0;
    if (!hWnd || !GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId()) {
        return;
    }

    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 ||
        !IsCalendarWindowClass(szClassName)) {
        return;
    }

    // Posted, not sent: this runs inside window management, and blocking it on the UI
    // thread which is being told about its own window is a deadlock waiting to happen.
    RunFromWindowThreadViaPostMessage(
        hWnd, [](PVOID) { OnCalendarOpenedOnCurrentThread(); }, nullptr);
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

// The return value of ShowWindow says whether the window was visible before, not whether
// the call worked, so it is passed through and not tested.
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    const BOOL wasVisible = ShowWindow_Original(hWnd, nCmdShow);

    if (nCmdShow != SW_HIDE) {
        OnWindowShown(hWnd);
    }

    return wasVisible;
}

std::vector<HWND> GetCalendarWindows() {
    struct ENUM_WINDOWS_PARAM {
        std::vector<HWND>* hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (IsCalendarWindowClass(szClassName)) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

void RefreshCalendarOnUiThreads() {
    for (HWND hCoreWnd : GetCalendarWindows()) {
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                SweepDayItems();
                // The shell rewrites the text of a cell after such a change as well, so
                // the watch is armed again rather than only used once.
                StartSweepTimer();
            },
            nullptr);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk lifecycle

// Installs one hook and says so when it does not work. A hook which silently does
// not get installed shows up much later, as "the mod does nothing", which is far
// harder to read than the line this writes. The name is a narrow string because
// GetProcAddress takes one, and %S prints it from this wide format.
//
// The hook, the original and the target all carry the same Prototype, so the compiler
// checks the signatures: the void* casts this had before let a hook whose signature had
// drifted from the function it replaces compile, and that mistake shows up as a crash
// inside the shell, not as an error here.
template <typename Prototype>
bool InstallHook(HMODULE module,
                 PCSTR functionName,
                 Prototype* hook,
                 Prototype** original) {
    if (!module) {
        Wh_Log(L"Initialization is incomplete: the module of %S is not loaded",
               functionName);
        return false;
    }

    Prototype* target =
        reinterpret_cast<Prototype*>(GetProcAddress(module, functionName));
    if (!target) {
        Wh_Log(L"Initialization is incomplete: %S is not in its module",
               functionName);
        return false;
    }

    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Initialization is incomplete: the hook of %S could not be set",
               functionName);
        return false;
    }

    return true;
}

// Why the hooks are set here: the window has to be hooked before it is created or shown,
// otherwise a window which existed before the mod was loaded, or one being made while it
// loads, is missed and never gets its holiday marks.
BOOL Wh_ModInit() {
    Wh_Log(L">");

    // A mod is loaded again after it was unloaded, so the flag which stops a refresh
    // request from starting a worker thread during an unload is cleared here.
    g_shuttingDown = false;

    g_target = Target::ShellExperienceHost;

    WCHAR moduleFilePath[MAX_PATH];
    switch (GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    LoadSettings();
    // No worker thread is started here: the thread is created by the first refresh
    // request which has a data source to fetch, so a mod which is left without a data
    // source never creates one (see RequestHolidayData).

    // Registered here rather than on the first post: this runs on one thread, before any
    // hook of this mod can post, so the message every other thread reads is written once
    // and does not change afterwards (see RunFromWindowThreadViaPostMessageMsg).
    RunFromWindowThreadViaPostMessageMsg();

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    InstallHook(user32Module, "CreateWindowInBand", CreateWindowInBand_Hook,
                &CreateWindowInBand_Original);
    InstallHook(user32Module, "CreateWindowInBandEx",
                CreateWindowInBandEx_Hook, &CreateWindowInBandEx_Original);
    // The flyout being shown is what says that the calendar is about to be looked at
    // again: the days of a month are drawn when it is opened, and the day cells of the
    // last time can be gone by then.
    InstallHook(user32Module, "ShowWindow", ShowWindow_Hook, &ShowWindow_Original);

    return TRUE;
}

// Initialised after the load: a window which is already there gets its initialization and
// its first pass here, so that a mod which is loaded after the window was created is not
// missed, and the data is fetched up front so that the calendar has content the first time
// it is opened.
void Wh_ModAfterInit() {
    Wh_Log(L">");

    for (auto hCoreWnd : GetCalendarWindows()) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                InitializeForCurrentThread();
                OnCalendarOpenedOnCurrentThread();
            },
            nullptr);
    }

    // Fetch the data up front so that the calendar is populated the first time it is
    // opened. With no data source set - the default - this does nothing at all.
    RequestHolidayData();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Before anything else: a refresh request which arrives while the mod is being
    // unloaded must not start a worker thread again.
    g_shuttingDown = true;

    StopWorkerThread();

    // One thread at a time, and on the thread which took the window events: the day cells
    // are XAML objects of the thread they were found on, and the window events are
    // delegates into this module, which is about to be unmapped.
    for (auto hCoreWnd : GetCalendarWindows()) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { UninitializeForCurrentThread(); }, nullptr);
    }

    // A post whose window is gone before its thread retrieved it is left with its hook
    // installed, and the procedure of that hook is in this module.
    UnhookPendingPostHooks();
}

// Changing the settings: only a changed data source clears the data which was fetched,
// because the other settings (the rules, whether to replace) are only a different way of
// drawing the same data, and the cells can be repainted from it. Dropping the data
// instead would make the calendar fall back to the lunar text until the next fetch.
// Data which is still inside the refresh interval is not fetched again either, so
// changing a rule does not cost a request.
// Emptying the address is a change like any other: it clears the data as well, so the
// marks are taken off the calendar and nothing is fetched again.
void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    const std::shared_ptr<const Settings> previous = GetSettings();
    LoadSettings();
    const std::shared_ptr<const Settings> settings = GetSettings();

    // The holidays of one data source are not the holidays of another, but the
    // data of the same source is kept: dropping it would make the calendar fall
    // back to the lunar text until the next fetch is done.
    if (settings->dataSourceUrl != previous->dataSourceUrl) {
        ResetHolidayData();
    } else if (!SameHighlightRules(previous->rules, settings->rules)) {
        // The data is the feed of the same address, and only the marking of it changed:
        // the body which was kept is read again, so that the new rules take effect at
        // once instead of with the next fetch - which may be a day away - and without
        // asking the source for a feed it already answered with.
        RemarkStoredHolidays(settings->rules);
    }

    // Re-apply the new settings to the day items which are already on screen.
    RefreshCalendarOnUiThreads();

    // Data which is still fresh is not fetched again, so that changing a colour
    // doesn't cost a request.
    RequestHolidayData();
}
