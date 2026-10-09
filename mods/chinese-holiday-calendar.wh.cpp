// ==WindhawkMod==
// @id              chinese-holiday-calendar
// @name            Chinese Holiday Calendar
// @name:zh-CN      中国节假日日历
// @description     Show Chinese statutory holidays and adjusted workdays in the Windows 11 calendar flyout
// @description:zh-CN 在 Windows 11 点击任务栏时间后弹出的日历中显示中国法定节假日与调休安排
// @version         0.17
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

Windows 11 的日历（点击任务栏右下角时间/日期弹出）默认只在数字下方显示农历或节气。这个 mod 会把
中国法定节假日和调休安排写进日历的日期单元格：

- **放假日**：文字换成接口返回的节日名（如"国庆节"，颜色默认蓝色）。
- **调休上班日**：文字默认换成"补班"（颜色默认红色）。
- 把"替换农历文字"关掉，就保留系统原本的农历/节气文字，只用颜色区分放假和调休。
- 其余日期保持系统原本的农历/节气显示，完全不动。

日历里没有可以放额外元素的容器（日期单元格只有"日期数字 + 农历文字"两个文本），而且系统会在切换
月份后异步重写农历文字，所以 mod 只用颜色和文字本身来表示放假/补班。

## 数据来源

默认使用 [holiday.ailcc.com](https://holiday.ailcc.com/api/holiday/ics) 的 ICS 接口：

```
https://holiday.ailcc.com/api/holiday/ics
```

这是一个 iCalendar（.ics）订阅源，一次返回多年数据，不需要按年份请求，接口地址可以改成任意其他
ICS 订阅源。

订阅源里除了法定节假日，还包含节气、传统节日、洋节等事件，它们是不带"（休）"/"（班）"标记的普通
条目。mod 只认带标记的条目，所以节气之类的日期不会被误标成放假。

订阅源里的日期会先检查是否真的存在（含闰年）：像 `20260231` 这种不存在的日期会被整条丢掉，
而不是"顺手"挪到 3 月 3 日去。

事件没有 DTEND 时按单日事件处理（它开始的那一天）；写了 DTEND 却解析不出来、或者结束日期不比
开始日期晚的，整条事件会被丢掉，日志里会有一条计数说明丢了几条——一个笔误不该悄悄变成"那天放假"。

数据默认每 24 小时刷新一次；请求失败时保留上次的数据，10 分钟后重试。没有网络时，日历保持系统
原本的样子。

## 颜色

两个颜色设置填颜色值，默认是"放假蓝 `4EA1FF`、调休红 `FF5A5A`"。可以填 `RRGGBB`、`#RRGGBB`、
`AARRGGBB` 或 `#AARRGGBB`。Windhawk 1.7.3 的设置界面没有颜色选择器，只能手填颜色值；想把颜色调准，
可以先在画图之类的工具里取到十六进制值再填进来。只想改颜色、不改文字，把"替换农历文字"关掉即可。

mod 只给自己标过的格子改颜色，并且在第一次改色之前记下这个文本块原来的前景色（本地值），
需要还原时原样写回去；原本没有本地值就清掉，让样式和继承来的颜色重新生效。所以像"相邻月份
的日期被系统调暗"这种由系统自己设的本地颜色，不会被 mod 弄丢。

## 关于识别

mod 每次都会重新遍历日历单元格的视觉树来定位"农历文字"（Windows 11 里叫 `LunarTextBlock`）：单元格
一共只有三四个子元素，走一遍很便宜，但可以保证不会往系统已经换掉的旧文本块里写字。日期数字是单元格
里唯一的纯数字文本，mod 永远不会动它。万一某个 Windows 版本改了模板结构，mod 会退回按字号自动识别，
并在日志里打印一次单元格结构（元素类型、名称、文字、字号）供排查。

mod 只替换农历文字的内容和颜色，不改动单元格的结构，也不会重复写入同一个单元格。系统在切换月份后
会异步重写农历文字，有时连文字块本身都会换掉，所以 mod 会定期复查日历单元格：只有发现放假/补班的
文字被系统覆盖时才会再写一次，其余单元格不做任何写入。

## 致谢

注入 XAML 视觉树的部分移植自 m417z 的
[Windows 11 Notification Center Styler](https://github.com/m417z/my-windhawk-mods)（GPL-3.0），
注入方式与 lonfro 的 "Agenda in Calendar View" 一致。

---

# Chinese Holiday Calendar (English)

Shows Chinese statutory holidays and adjusted workdays inside the Windows 11
calendar flyout, the one that opens when you click the taskbar clock.

- **Days off**: the line under the day number is replaced with the holiday name
  from the data source (for example `国庆节`) and drawn in blue by default.
- **Adjusted workdays (调休)**: replaced with `补班` and drawn in red by default.
- **Replace the lunar text = off**: keeps the system's own lunar date or solar
  term and only changes the colour.
- **Every other day**: left exactly as the system draws it.

## Why it works the way it does

A day cell of that calendar contains only two texts, the day number and one
lunar-text block, and there is no container to put an extra element in. Inserting
an element makes the shell rebuild the day cells over and over (measured: 470
insertions in 37 seconds, and an empty calendar afterwards), so the mod says
everything with the existing text and its colour. The shell also rewrites its own
lunar text asynchronously after a month change, which is why the mod re-checks
the visible cells once a second and only writes when a cell no longer shows what
it wants.

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
| Enabled | Master switch. |
| Data source (ICS) | Any iCalendar (.ics) URL. The default one returns several years at once. |
| Refresh interval (hours) | How long fetched data is reused (default 24 h). |
| Replace the lunar text | On: replace the text (default). Off: keep the lunar text, colour only. |
| Day-off text | Empty means the holiday name from the feed. `{name}` is expanded too. |
| Adjusted-workday text | Default `补班`. Empty means keep the lunar text, colour only. `{name}` shows the name of the holiday the workday belongs to (a 国庆 workday would read `国庆节`). |
| Day-off colour | `RRGGBB`, `#RRGGBB`, `AARRGGBB` or `#AARRGGBB`. |
| Workday colour | Same as above. |

Windhawk 1.7.3 has no colour picker for mod settings, so the colour has to be
typed as a value; the `#! $format: colorRgb` annotation is only for Windhawk
versions which do support a picker.

## Data source

The default source is the ICS feed at
[holiday.ailcc.com](https://holiday.ailcc.com/api/holiday/ics). It carries about
400 events for 2024 to 2028; solar terms, traditional festivals and foreign
holidays carry no full-width or half-width "day off"/"workday" marker and are
ignored, so only statutory holidays and adjusted workdays get marked. The URL can
be replaced with any other ICS feed. Data is fetched every 24 hours, a failed
request keeps the previous data and is retried after 10 minutes, and a feed may
describe at most 20000 dates in total.

Every date a feed names is checked for being a date which exists, leap years
included: an entry such as `20260231` is dropped instead of being walked into
3 March.

An event without a `DTEND` is the single day it starts on. A `DTEND` which is
there but cannot be parsed, or which is not after `DTSTART`, drops the whole
event, and a log line counts the ones which were dropped that way.

Deliberately not supported: recurrence rules (`RRULE`, `EXDATE` and
`RECURRENCE-ID` are read as the single occurrence the event names, and a one-time
log line says so), UTC offset conversion (a date-time is taken as the date it
names), and any badge or corner marker inside a cell.

## How it finds the lunar text

The mod walks the three or four children of a day cell on every pass, never
writes to the day number (it is the only all-digit text in a cell), prefers the
block the calendar names `LunarTextBlock`, falls back to the smallest text that
has content, and remembers the element it wrote to by interface pointer so that a
block the shell has already replaced is not written to any more.

## Verification

`selftest\selftest.cmd` compiles the mod together with a small test program and
runs 51 assertions over the parsing code (date formats, dates which do not exist,
folded ICS lines, escaping, exclusive `DTEND`, a `DTEND` which cannot be used, the
date cap, ...). The source also starts with a design-notes section listing the
trade-off behind every non-obvious decision.

## Credits

The XAML injection part is ported from m417z's
[Windows 11 Notification Center Styler](https://github.com/m417z/my-windhawk-mods)
(GPL-3.0); the injection approach matches lonfro's "Agenda in Calendar View".
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enable the mod
  $name:zh-CN: 启用
  $description: When turned off, the calendar is left untouched.
  $description:zh-CN: 关闭后不会修改日历。
- dataSourceUrl: "https://holiday.ailcc.com/api/holiday/ics"
  $name: Data source (ICS)
  $name:zh-CN: 数据接口（ICS）
  $description: >-
    An iCalendar (.ics) address. The default one returns several years of data at once.
  $description:zh-CN: iCalendar（.ics）订阅地址，一次返回多年数据。
- refreshIntervalHours: 24
  $name: Refresh interval (hours)
  $name:zh-CN: 刷新间隔（小时）
  $description: Fetched data is reused for this long before the source is asked again.
  $description:zh-CN: 数据在这个时间内不会重复请求。
- replaceText: true
  $name: Replace the lunar text
  $name:zh-CN: 替换农历文字
  $description: >-
    On: a day off shows the name of the holiday (国庆节, for example) and an adjusted
    workday shows 补班, in place of the lunar text of the cell. Off: the lunar text stays
    and only the colour tells a day off from an adjusted workday.
  $description:zh-CN: >-
    打开时，放假日显示节日名（如"国庆节"）、调休日显示"补班"，替换单元格里原本的农历文字；
    关闭时保留农历文字，只用颜色区分放假和调休。
- offDayText: ""
  $name: Text of a day off
  $name:zh-CN: 放假日的显示文字
  $description: >-
    Used only while "replace the lunar text" is on. Empty means the name from the data
    source, as it is (国庆节, for example); a fixed text can be written instead, and
    {name} stands for the name of the holiday.
  $description:zh-CN: >-
    仅在"替换农历文字"打开时生效。留空表示使用接口里的节日名，原样显示（如"国庆节"）；
    也可以填固定文字，或用 {name} 表示节日名。
- workdayText: "补班"
  $name: Text of an adjusted workday
  $name:zh-CN: 调休上班日的显示文字
  $description: >-
    Used only while "replace the lunar text" is on. Empty keeps the lunar text and only
    the colour changes. {name} is the name of the holiday the workday belongs to, so a
    workday of 国庆节 can read 国庆节 as well.
  $description:zh-CN: >-
    仅在"替换农历文字"打开时生效。留空表示保留原来的农历文字、只改颜色。{name} 是接口里的节日名，
    调休日填 {name} 会跟着显示所属节日的名字（例如国庆调休那天也显示"国庆节"）。
- offDayColor: "4EA1FF"
  $name: Colour of a day off (blue by default)
  $name:zh-CN: 放假日文字颜色（默认蓝）
  $description: >-
    A colour value, for example 4EA1FF (blue) or #FF5A5A (red); the eight digit form
    #AARRGGBB is accepted as well. The settings UI of Windhawk 1.7.3 has no colour
    picker, so the value is typed by hand.
  $description:zh-CN: >-
    填颜色值，如 4EA1FF（蓝）、#FF5A5A（红），也支持 8 位的 #AARRGGBB。
    Windhawk 1.7.3 的设置界面没有颜色选择器，只能手填。
  #! $format: colorRgb
- workdayColor: "FF5A5A"
  $name: Colour of an adjusted workday (red by default)
  $name:zh-CN: 调休上班日文字颜色（默认红）
  $description: >-
    Same written forms as above, for example FF5A5A (red) or E81123 (dark red).
  $description:zh-CN: 填法同上，如 FF5A5A（红）、E81123（深红）。
  #! $format: colorRgb
*/
// ==/WindhawkModSettings==

// =============================================================================
// Design notes (for whoever reviews this next, an AI or a person)
//
// The sections below are the decisions behind this mod, why they were made, and what
// they cost. Several of the odd-looking parts are the way they are because of what
// the shell was measured to do, not out of taste, so it is worth reading this before
// changing them. Every log line named here can be seen in DbgView, or in the
// DbgViewMini tab of the Windhawk editor.
//
// ---------------------------------------------------------------------------
// 1. Scope
//
//    - One job: draw the Chinese statutory holidays and the adjusted workdays into
//      the calendar which Windows 11 opens when the taskbar clock is clicked.
//    - A day cell has no container to put an extra element in (see 3), so the mod
//      says what it has to say with the text and the colour which are already there.
//    - The data source is a configurable ICS feed (holiday.ailcc.com by default);
//      what the parser can and cannot do is in 9.
//    - The structure, the layout and the behaviour of the calendar are left alone,
//      and switching the mod off restores them (see 13).
//
// ---------------------------------------------------------------------------
// 2. Why the visual tree is injected into
//
//    - Windows has no public API to add anything to its own calendar, which lives
//      inside ShellExperienceHost.exe (and ShellHost.exe on newer builds) and is
//      drawn in XAML.
//    - The injection (VisualTreeWatcher + TAP) is therefore taken from m417z's
//      "Windows 11 Notification Center Styler"; the GPL notice below is for it.
//    - The scaffolding (visualtreewatcher / tap / simplefactory / module / api, plus
//      the CreateWindowInBand(Ex), RegOpenKeyExW and RegQueryValueExW hooks) is kept
//      as close to upstream as it can be, so that it stays comparable when upstream
//      is updated. The mod's own logic starts at "Calendar UI".
//    - Two places deliberately differ from upstream, both to fix a weakness upstream
//      has as well, and both are commented where they are:
//        * The VisualTreeWatcher constructor takes its reference with AddRef before
//          CreateThread. Upstream does it the other way round (create the thread,
//          then AddRef), which leaves a window in which a fast thread gives back the
//          last reference and the creating thread then AddRefs a freed object; the
//          path where AdviseVisualTreeChange fails at once is the shortest one.
//        * Every Wh_SetFunctionHook return value in Wh_ModInit is checked and
//          reported by name. Upstream ignores them, and a hook which was not
//          installed then shows up as "the injection sometimes does nothing", with
//          no reason in the log. The two registry hooks get an extra "Initialization
//          is incomplete" line, because the injection does not work without them,
//          but Wh_ModInit still returns TRUE: a failed load would hide the rest of
//          the log, and the mod does everything else without a window hook.
//
// ---------------------------------------------------------------------------
// 3. What a day cell really contains (measured on Windows 11, 2026-10)
//
//       CalendarViewDayItem size=41x41
//         TextBlock                        fontSize=14  text="20"     <- day number
//         TextBlock #LunarTextBlock        fontSize=11  text="<lunar>" <- lunar date
//         Border                           size=41x41                 <- sometimes
//
//    A cell holds two texts, the day number and one lunar text, and sometimes a
//    border, so GetChildrenCount(dayItem) is 2 or 3. This comes from the structure
//    snapshots LogDayItemStructure prints into the log, and is not a guess.
//
//    What follows from it:
//    - A badge in the corner of the cell cannot be done. Inserting an element makes
//      the shell rebuild its day cells, which triggers another insertion: 470
//      insertions in 37 seconds were measured, and the calendar ended up empty.
//      Putting an InlineUIContainer into the day number fails as well
//      (Inlines().Append answers 0x80070057). Text and colour only.
//    - There is nothing else in a cell to write to, so the mod rewrites the lunar text
//      block and nothing else.
//
// ---------------------------------------------------------------------------
// 4. State: one day cell is one DayCell, identified by an interface pointer
//
//    - The key is winrt::get_abi(item), the raw interface pointer. Comparing the
//      elements instead does not work (two winrt references to the same XAML element
//      compare unequal), and item.Tag belongs to the shell and is not the mod's to
//      use. Several early bugs came from those two mistakes.
//    - The state lives in t_cells, which is thread_local, because a XAML element can
//      only be touched on the thread which owns it. A day item is held through a
//      weak_ref: the mod lets go as soon as the shell destroys it, and the sweep
//      drops the entries which are gone.
//    - The shell recycles day items, giving the same item another date when the month
//      changes, so a DayCell remembers its key (the date). A different key means the
//      shell filled the cell in again, and the text and colour state of the old date
//      is dropped (see 7).
//
// ---------------------------------------------------------------------------
// 5. The lunar text block is looked up again on every pass, never cached for long
//
//    - The shell replaces those text blocks when the month changes, and a write to an
//      element which is no longer in the calendar does nothing at all, which looks
//      exactly like "the mod stopped working". FindLabelTextBlock therefore walks the
//      cell again on every pass (three or four children, which is cheap), and prefers
//      the element it wrote to last time if that element is still there, so that
//      callbacks are not registered twice and writes are not repeated.
//    - Only a text block whose text is not all digits is a candidate: the day number
//      is the only all-digit text in a cell, while the lunar text is a lunar day, a
//      solar term or a festival name. This rule replaced an earlier font-size
//      heuristic which once wrote into a day number in a real log
//      (Day 20260925: "1" -> "中秋节").
//    - The name wins (LunarTextBlock); the font-size heuristic is only the fallback
//      for a block which matches no name and is not a number either. Getting that
//      wrong costs one cell, it does not break the structure.
//
// ---------------------------------------------------------------------------
// 6. The one-second sweep is what the shell's asynchronous rewrite forced
//
//    - After a month change the shell rewrites the lunar text some time after the mod
//      has written to it, and it may replace the text block as well, so the holiday
//      name would be taken away again. Two things cover that:
//      a) a Text property callback on the text block (OnLabelTextChanged), which
//         applies to the cell again as soon as the text is written back;
//      b) a sweep of the tracked day cells once a second, which also covers a text
//         block which was replaced together with its callback.
//    - The sweep writes only when a cell does not show what the mod wants, so an idle
//      calendar costs a few dozen property reads per second and no layout passes.
//      That is what makes one second acceptable; do not shorten it (it would touch
//      the shell's elements more often) and do not remove it (it is what covers the
//      case above).
//    - The timer is a DispatcherQueueTimer on the UI thread which owns the day cells.
//      It is stopped when the tracked list becomes empty (the calendar was closed, or
//      the cells were recycled) and started again when a cell shows up.
//
// ---------------------------------------------------------------------------
// 7. Saving and restoring: savedText, savedForeground and ownBrush
//
//    - savedText is only trustworthy while the same cell shows the same date in the
//      same text block, so it is cleared when the key changes or the text block is
//      replaced. It is written back only when the mod no longer wants to change the
//      cell (replacement switched off, mod disabled, unloading).
//    - The colour is not "another brush kept beside it": ReadLocalValue() is used to
//      keep the local value of Foreground as it is, together with whether there was
//      one at all. A local value is put back; no local value means ClearValue, so
//      that the style and the inherited value apply again. Clearing alone is not
//      enough: the shell's template sets Foreground as a local value on the element
//      (which is how the days of the neighbouring months are dimmed), and clearing it
//      drops the label to the colour of its parent instead of its own. An early
//      version kept the brush in a weak_ref, which cannot be read back once the shell
//      lets go of it, and fell back to ClearValue the same way.
//    - The colour is saved before the first colour write, and not while replacing the
//      text: the text and the colour are two independent settings, since the colour is
//      written with "replace the lunar text" switched off as well, and reading
//      Foreground on a cell which was already coloured gives back the mod's own brush.
//    - "Is the colour on the label the mod's own?" is answered by object identity and
//      not by the colour: every colour write creates a SolidColorBrush which is kept
//      in ownBrush (a strong reference, so the object stays alive and its address
//      cannot be reused by another brush), and the comparison is on the interface
//      pointer. This blocks both directions of the mistake:
//        * when saving: a brush of the mod's own is not stored as the original colour
//          of the date. Without that, a recycled cell would have the colour written
//          for the old date saved as the original colour of the new one, and the day
//          would be restored to it later.
//        * when restoring: only the colour the mod wrote is taken off. A colour the
//          shell has set since, the dimmed neighbouring days being the example, is
//          left alone, because clearing it would throw the shell's own value away.
//    - A cell which the shell gives another date keeps ownBrush and the "the mod
//      coloured this" flag, and drops only the saved local value: the text state
//      belongs to the date, the colour state belongs to the text block element, and
//      the shell does not touch a colour the mod put on the label while it changes the
//      date. Only ownBrush recognises it, and that is what makes the rule above work.
//    - Residual risk, recorded honestly: that judgement rests on ownBrush, and the
//      state itself can be lost (CellFor drops the whole entry when a day item is
//      destroyed and its address is taken by another one). If the shell then reuses a
//      text block the mod has coloured, without re-applying the template's Foreground,
//      the judgement fails once. Re-attaching a text block to a template does re-apply
//      the template's value, which overwrites the mod's colour, so this is hard to
//      hit; closing it completely would mean keeping every brush the mod has ever
//      created in a global table, with the memory and the "when may it be dropped"
//      question that brings.
//    - IsOwnText() answers "is the text on this cell one the mod could have written".
//      It stays because there is a window appliedText does not cover: when a cell is
//      recycled for a new date, appliedText has been cleared while the cell still
//      shows the holiday name the mod wrote, and a name which is not recognised as the
//      mod's own would be saved as the shell's text and later restored onto a day
//      which is not a holiday. The price is that the shell's own lunar text which
//      happens to equal a holiday name (the real Mid-Autumn Festival, for example) is
//      not saved, and in that case the text the mod wants to write is the same anyway,
//      so nothing goes wrong.
//
// ---------------------------------------------------------------------------
// 8. The tracked list: deduplication and a limit
//
//    - The shell reports a day item as added again whenever it puts the item back into
//      the calendar, which is what a month change does. One session had "Day item
//      added" 1085 times for the 42 cells on screen, so TrackDayItem deduplicates by
//      identity, and a repeated report only moves the entry to the recent end.
//    - The limit kMaxTrackedDayItems: a destroyed cell is normally recycled, but a
//      shell which keeps cells in a pool would make the list grow forever. The limit
//      bounds the cost of a sweep, and a cell which is dropped and shown again is
//      reported as added again and comes back. Hitting the limit logs "Tracking is at
//      its limit ...", which is how pooling is recognised.
//    - The sweep indexes the list instead of iterating it: applying to a day item can
//      add an entry, which would invalidate an iterator.
//
// ---------------------------------------------------------------------------
// 9. The data layer: what the ICS parser does and does not do
//
//    Supported:
//    - folded lines (RFC 5545: a continuation starts with a space or a tab)
//    - escapes: \n \\ \, \; and both the full-width and the half-width form of the
//      day-off and workday markers
//    - DTSTART / DTEND / SUMMARY, with the parameter part (;VALUE=DATE;TZID=...)
//      ignored
//    - DTEND is exclusive, as in RFC 5545: 2/15 to 2/24 gives 2/15..2/23
//    - an event without a DTEND is a single day, the one it starts on. A DTEND which
//      is there but cannot be used is a different thing: it did not parse, or it does
//      not come after DTSTART as RFC 5545 requires, and the whole event is then
//      dropped, counted, and reported in one log line. Reading it as a single day
//      would turn a typo in a feed into "this day is a holiday" in silence, and the
//      whole point of the mod is that the days it shows are the right ones.
//    - only entries which carry a day-off or a workday marker count: the feed also
//      carries solar terms and traditional festivals without one, and those are not
//      statutory holidays.
//    Not supported, and said so rather than pretended otherwise:
//    - RRULE / EXDATE / RECURRENCE-ID: only the occurrence the event names is read,
//      with a one-time log line
//    - time zone conversion: a date-time is read as the date it names. The default
//      source is made of all-day ("VALUE=DATE") events, a source which writes
//      00:00:00Z is still the same day at +8, and only a timestamp close to UTC
//      midnight would differ by a day. To be added when it is actually needed.
//    - at most kMaxHolidayDates = 20000 dates per source. The URL is user
//      configurable, and an event written as "1900-01-01 to 2100-01-01" would keep
//      the worker thread busy for tens of thousands of dates, holding up the next
//      refresh and StopWorkerThread with it. The limit is therefore checked inside
//      the loop, and hitting it logs "describes more than 20000 holiday dates".
//
// ---------------------------------------------------------------------------
// 10. Dates: how they are represented and parsed
//
//    - A date is an int32 "yyyymmdd" (MakeDateKey). Integer comparison is then date
//      comparison, and the same value serves as a map key and as a range endpoint
//      (DTSTART < key < DTEND), which is one whole date type less. The price is that
//      the value is not continuous (20260228 + 1 is not 20260301), so every
//      day-by-day step has to go through NextDateKey() and never through arithmetic.
//    - ParseDateKey accepts "20260215", "2026-02-15", "2026/2/15" and
//      "20260215T000000Z"; a time which follows a date becomes a part of its own and
//      is ignored. The compact form is recognised by "the first part has eight
//      digits", not by "there is one part", which would miss the forms carrying a
//      time.
//    - Every date goes through IsValidDate: the year, the month and the day have to
//      exist, leap years included. A date which does not exist returns 0 and the
//      caller skips the event. It cannot be accepted and corrected later: a range is
//      walked with NextDateKey, which normalises the "+1 day" of 2026-02-31 into
//      2026-03-03, so the mod would invent dates the source never named. The year is
//      limited to 1900..9999, which turns away a "19700101" placeholder and keeps
//      MakeDateKey inside an int32.
//    - DateKeyFromDateTime turns the XAML DateTime, 100 ns ticks since 1601, into a
//      local date: CalendarViewDayItem.Date is UTC midnight, and it has to be turned
//      into a local date to line up with the dates of the feed.
//
// ---------------------------------------------------------------------------
// 11. The colour settings
//
//    - Six or eight hexadecimal digits (RRGGBB / #RRGGBB / AARRGGBB / #AARRGGBB).
//      Windhawk 2.0 writes the eight-digit form "#FF4EA1FF", so both are accepted.
//    - The settings carry a `#! $format: colorRgb` annotation. A search through the
//      Windhawk 1.7.3 installation showed that neither its UI nor its engine knows
//      that format, so it is a forward-compatible marker only and the colour is still
//      typed by hand; it is kept so that an upgrade turns it into a picker.
//    - An early version used the {red,green,blue} sub-key form, and leftovers such as
//      offDayColor.red may still be in the registry. In 1.7.3 that form is three
//      number boxes and awkward to use, hence the string form. Leftover keys are
//      ignored when the settings are read.
//
// ---------------------------------------------------------------------------
// 12. Threads
//
//    - The data is fetched on a worker thread of the mod's own (WorkerThreadProc),
//      because it blocks on a network request, and the UI threads are told to redraw
//      afterwards. Day cells are read and written only on the thread which owns them,
//      which is what the thread_local state is for.
//    - "Do something on the thread of that window" is spelled RunFromWindowThread
//      (SendMessage plus a hook) or RunFromWindowThreadViaPostMessage (PostMessage
//      plus a hook). ShellHost and ShellExperienceHost differ in their timing, and
//      initialising when a ControlCenterWindow has just been created is too early
//      there, which is why that one goes through the PostMessage version.
//    - Refreshing: once every 24 hours by default, a failed request keeps the data
//      which is already there and is retried after 10 minutes, and changing the
//      settings clears the data only when the URL changed and reuses it otherwise,
//      which keeps the calendar from flashing back to the lunar text and fetching
//      again.
//    - Every step of starting the worker thread and of handing a refresh over is
//      checked: a failed CreateEvent or CreateThread closes the handles which were
//      created and gives up on the start, with the mod keeping the data it has, and a
//      failed SetEvent takes the pending flag back and records an attempt. Earlier
//      versions ignored those return values, and one event which was not created left
//      g_fetchPending true for good: every later refresh was dropped in silence, and
//      switching the mod off and on again did not help.
//    - A failed start is not permanent: RequestHolidayData asks for the thread again
//      on every request, and does so before the "is the data still fresh" test, since
//      a request which cannot be answered must not consume the refresh interval.
//      StartWorkerThread serialises itself with g_workerThreadMutex, so two threads
//      asking at the same time cannot create two threads and two event pairs. Unload
//      sets g_shuttingDown first, so that a request arriving during it cannot start
//      the thread again.
//
// ---------------------------------------------------------------------------
// 13. Unloading and restoring
//
//    - Wh_ModUninit: set the shutdown flag, stop the worker thread, take the
//      injection out, and then restore every cell on each window thread: unregister
//      the two property callbacks, write the saved text and colour back, clear the
//      state, stop the timer.
//    - The restore assumes the cell is still alive. A cell the shell has destroyed
//      does not need one, because the shell fills it in itself. The calendar is not
//      guaranteed to look untouched the moment the mod is unloaded, since the shell
//      needs a moment to repaint, but nothing broken is left behind: the mod never
//      changes the element tree, it only writes text and a foreground colour.
//
// ---------------------------------------------------------------------------
// 14. Known limits and things which were not verified (the honest list)
//
//    - The appearance can only be verified in the real Windows 11 shell. What can be
//      verified here is the MSVC syntax and link check and the parser assertions of
//      selftest (see 15).
//    - The structure of a day cell depends on the Windows build. A build which
//      changes its template can lead to "No text block to replace" (the log prints a
//      structure snapshot once) or to the font-size fallback being used.
//    - A badge in a cell cannot be done with the current structure, see 3.
//    - A colour picker depends on the Windhawk version, see 11.
//
// ---------------------------------------------------------------------------
// 15. How to verify it, without clicking the calendar
//
//    - C:\Users\yzfar\Desktop\hawk\selftest\selftest.cmd compiles the mod together
//      with a small test program through MSVC (windhawk.h next to it is a stub of the
//      API) and runs selftest.exe, which prints the result of 51 assertions over the
//      date formats, folded lines, escapes, the exclusive DTEND, the date validation,
//      the DTENDs which are dropped, the date cap and the {name} expansion. It exits
//      with the number of failures, so 0 means all of them passed.
//    - The compiler is vcvars64.bat of D:\Software\Microsoft Visual Studio\18\
//      Community, with the WinRT headers of cppwinrt in Windows Kits 10.0.26100.0.
//      Windhawk itself compiles with clang, so MSVC is only a check, which is where
//      warnings such as C4068 (unknown pragma "clang") come from. They are expected,
//      and the mod this was ported from has them as well.
//    - The log lines worth looking at on a live system:
//        Day <date>: "<old>" -> "<new>"      the text of a cell was written
//        No text block to replace ...        no lunar text found, with a snapshot of
//                                            the structure of the cell
//        Tracking is at its limit ...        the shell is pooling day items
//        The data source describes more than 20000 holiday dates   data was cut off
//        events are dropped: their DTEND ...  a DTEND could not be used, with a count
//        Initialization is incomplete: ...   a hook is missing, and the line names it
//        There is no worker thread ...       a refresh could not be handed over
//        Could not hand the refresh over ...
//        Substituting the XAML debug key     the injection, and the suppressing of
//        Reporting DisableCompositionDiag as set     the composition diagnostics
//
// ---------------------------------------------------------------------------
// 16. The settings block
//
//    - Every setting carries the English text as $name and $description, which is what a
//      language without a variant of its own falls back to, plus a ":zh-CN" variant with
//      the Chinese text. That is the documented way to localise settings, and the mods of
//      the official store are written the same way, with the base text in English.
//    - The name of a setting (enabled, dataSourceUrl, ...) is the key a stored value is
//      kept under, so a label may be localised but a name must never be changed: the
//      settings a user already has would be lost.
//    - The lines which begin with "#! " are annotations Windhawk 1.7.3 does not know;
//      the marker makes them plain YAML comments to 1.7.3, and Windhawk 2.0 reads the
//      line with the marker removed. A block which marks one annotation has to mark every
//      annotation of that kind in it, which is why both colour settings carry the
//      `#! $format: colorRgb` line.
//    - The defaults are written here and in the Settings struct, so the two have to be
//      changed together.
//
// =============================================================================
//
// Parts of the injection code below are ported from the "Windows 11 Notification
// Center Styler" mod by m417z, which is licensed under GPL-3.0:
//
//   Copyright (C) 2026 m417z
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <xamlom.h>

#include <atomic>
#include <cstdint>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.h>

std::atomic<bool> g_initialized;
thread_local bool g_initializedForThread;

// The XAML composition diagnostics rebuild a process-wide visual tree walker
// without any locking whenever a DirectComposition visual is added, so any UI
// thread which adds one corrupts the heap while another thread is in the same
// code. Only element mutations are needed here, and those are reported by an
// unrelated code path, so the composition diagnostics are kept from being
// created at all: XamlDiagnostics::CreateCompVisualDiag skips them when the
// HKLM\Software\Microsoft\XAML\Debug\DisableCompositionDiag value is 1.
// Windows.UI.Xaml.dll reads and caches the value once, from within
// AdviseVisualTreeChange, so answering that single read is enough. The value is
// not written to the registry, which would be a machine-wide side effect; the two
// hooks below fake that one read instead.
thread_local bool g_reportCompositionDiagAsDisabled;

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

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
namespace wux = winrt::Windows::UI::Xaml;

// Defined after the mod's own types are known.
void HandleDayItemAdded(wux::Controls::CalendarViewDayItem const& item);
void QueueDiagnosticsRelease(InstanceHandle handle);
void FlushDiagnosticsReleasesIfQuiet();
void ForgetFakedDebugKeys();

#pragma endregion  // winrt_hpp

#pragma region visualtreewatcher_hpp

// XamlDiagnostics implements this interface too, and xamlom.h does not declare
// it. UnregisterInstance closes the runtime object cached for a handle, the
// only reference the diagnostics keep to an element once it was reported.
static constexpr GUID IID_IXamlDiagnosticsTestHooks = {
    0x735941a2, 0x3ee3, 0x495a, {0x8d, 0xa9, 0x97, 0x26, 0x27, 0x00, 0x30, 0x75}};

struct IXamlDiagnosticsTestHooks : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE UnregisterInstance(InstanceHandle handle) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetDispatcherQueueForObject(
        InstanceHandle handle,
        void** dispatcherQueue) = 0;
};

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
   public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site);

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;

    VisualTreeWatcher(VisualTreeWatcher&&) = delete;
    VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;

    ~VisualTreeWatcher();

    void UnadviseVisualTreeChange();

    void ReleaseDiagnosticsReference(InstanceHandle handle);

   private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation,
                                                 VisualElement element,
                                                 VisualMutationType mutationType) override;
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle element,
                                                    VisualElementState elementState,
                                                    LPCWSTR context) noexcept override;

    wf::IInspectable FromHandle(InstanceHandle handle) {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(
            handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));
        return obj;
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
    winrt::com_ptr<IXamlDiagnosticsTestHooks> m_XamlDiagnosticsTestHooks = nullptr;
};

#pragma endregion  // visualtreewatcher_hpp

#pragma region visualtreewatcher_cpp

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site)
    : m_XamlDiagnostics(site.as<IXamlDiagnostics>()) {
    Wh_Log(L"Constructing VisualTreeWatcher");

    HRESULT hr = m_XamlDiagnostics->QueryInterface(
        IID_IXamlDiagnosticsTestHooks, m_XamlDiagnosticsTestHooks.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"IXamlDiagnosticsTestHooks is unavailable: %08X", hr);
    }

    // Calling AdviseVisualTreeChange from the current thread causes the app to
    // hang in Advising::RunOnUIThread sometimes. Creating a new thread and
    // calling it from there fixes it.
    //
    // The reference the new thread gives back at its end is taken before the thread
    // exists: a thread which ran through before the creating thread got to AddRef
    // would give back the last reference and leave the object freed, and the AddRef
    // behind it would then write to freed memory. This is deliberately different
    // from the mod this is ported from, which takes the reference afterwards.
    AddRef();
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            auto watcher = reinterpret_cast<VisualTreeWatcher*>(lpParam);
            auto service = watcher->m_XamlDiagnostics.as<IVisualTreeService3>();
            g_reportCompositionDiagAsDisabled = true;
            // Only the keys handed out for this call are answered below.
            ForgetFakedDebugKeys();
            HRESULT hr = service->AdviseVisualTreeChange(watcher);
            g_reportCompositionDiagAsDisabled = false;
            ForgetFakedDebugKeys();
            watcher->Release();
            if (FAILED(hr)) {
                Wh_Log(L"AdviseVisualTreeChange error %08X", hr);
            }
            return 0;
        },
        this, 0, nullptr);
    if (!thread) {
        // The thread never took the reference over, so it is this thread's to drop.
        Wh_Log(L"Failed to create the watcher thread (error %u)", GetLastError());
        Release();
        return;
    }
    CloseHandle(thread);
}

VisualTreeWatcher::~VisualTreeWatcher() {
    Wh_Log(L"Destructing VisualTreeWatcher");
}

void VisualTreeWatcher::UnadviseVisualTreeChange() {
    Wh_Log(L"UnadviseVisualTreeChange VisualTreeWatcher");
    HRESULT hr =
        m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
    if (FAILED(hr)) {
        Wh_Log(L"UnadviseVisualTreeChange failed with error %08X", hr);
    }
}

// Drops the reference the diagnostics hold for a reported element. Without it,
// every element ever reported to the callback would stay alive for the lifetime
// of the process.
void VisualTreeWatcher::ReleaseDiagnosticsReference(InstanceHandle handle) {
    if (!m_XamlDiagnosticsTestHooks) {
        return;
    }

    HRESULT hr = m_XamlDiagnosticsTestHooks->UnregisterInstance(handle);
    if (FAILED(hr)) {
        Wh_Log(L"UnregisterInstance failed with error %08X", hr);
    }
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation relation,
                                              VisualElement element,
                                              VisualMutationType mutationType) try {
    if (mutationType == Add && element.Type &&
        wcsstr(element.Type, L"CalendarViewDayItem") && g_initializedForThread) {
        try {
            auto inspectable = FromHandle(element.Handle);
            if (auto dayItem =
                    inspectable.try_as<wux::Controls::CalendarViewDayItem>()) {
                HandleDayItemAdded(dayItem);
            }
        } catch (...) {
            Wh_Log(L"Error handling day item: %08X", winrt::to_hresult());
        }
    }

    // A tree discarded whole is never dismantled, so it reports no removals to
    // be released by; the queue is drained on the dispatcher instead, once the
    // walk which produced the reports has finished.
    QueueDiagnosticsRelease(element.Handle);
    if (mutationType == Add) {
        QueueDiagnosticsRelease(relation.Parent);
    }
    FlushDiagnosticsReleasesIfQuiet();

    return S_OK;
} catch (...) {
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);

    // Returning an error prevents (some?) further messages, always return
    // success.
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle,
                                                 VisualElementState,
                                                 LPCWSTR) noexcept {
    return S_OK;
}

#pragma endregion  // visualtreewatcher_cpp

#pragma region tap_hpp

#include <ocidl.h>

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// {C85D8CC7-5463-40E8-A432-F5916B6427E5}
static constexpr CLSID CLSID_WindhawkTAP = {
    0xc85d8cc7, 0x5463, 0x40e8, {0xa4, 0x32, 0xf5, 0x91, 0x6b, 0x64, 0x27, 0xe5}};

class WindhawkTAP
    : public winrt::implements<WindhawkTAP, IObjectWithSite, winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite) override;
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppvSite) noexcept override;

   private:
    winrt::com_ptr<IUnknown> site;
};

#pragma endregion  // tap_hpp

#pragma region tap_cpp

HRESULT WindhawkTAP::SetSite(IUnknown* pUnkSite) try {
    // Only ever 1 VTW at once.
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    site.copy_from(pUnkSite);

    if (site) {
        // Decrease refcount increased by InitializeXamlDiagnosticsEx.
        FreeLibrary(GetCurrentModuleHandle());

        g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
    }

    return S_OK;
} catch (...) {
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

HRESULT WindhawkTAP::GetSite(REFIID riid, void** ppvSite) noexcept {
    return site.as(riid, ppvSite);
}

#pragma endregion  // tap_cpp

#pragma region simplefactory_hpp

#include <Unknwn.h>

template <class T>
struct SimpleFactory
    : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter,
                                             REFIID riid,
                                             void** ppvObject) override try {
        if (!pUnkOuter) {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        } else {
            return CLASS_E_NOAGGREGATION;
        }
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

#pragma endregion  // simplefactory_hpp

#pragma region module_cpp

#include <combaseapi.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllGetClassObject(REFCLSID rclsid,
                                                REFIID riid,
                                                LPVOID* ppv) try {
    if (rclsid == CLSID_WindhawkTAP) {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    } else {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
} catch (...) {
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllCanUnloadNow() {
    if (winrt::get_module_lock()) {
        return S_FALSE;
    } else {
        return S_OK;
    }
}

#pragma clang diagnostic pop

#pragma endregion  // module_cpp

#pragma region api_cpp

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX = decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkTAP() noexcept {
    HMODULE module = GetCurrentModuleHandle();
    if (!module) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location))) {
        case 0:
        case ARRAYSIZE(location):
            return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux(LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32));
    if (!wux) [[unlikely]] {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) [[unlikely]] {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // I didn't find a better way than trying many connections until one works.
    // Reference:
    // https://github.com/microsoft/microsoft-ui-xaml/blob/d74a0332cf0d5e58f12eddce1070fa7a79b4c2db/src/dxaml/xcp/dxaml/lib/DXamlCore.cpp#L2782
    HRESULT hr;
    for (int i = 0; i < 10000; i++) {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"VisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), L"", location,
                  CLSID_WindhawkTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }

    return hr;
}

#pragma endregion  // api_cpp

////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cwctype>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <initguid.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

////////////////////////////////////////////////////////////////////////////////
// Settings

// The snapshot of the settings. The defaults are written twice on purpose: here, for
// a setting which cannot be read, and in the WindhawkModSettings YAML, which is what
// the settings UI starts from, so the two have to be changed together. LoadSettings
// replaces the whole snapshot through a shared_ptr, so the UI threads and the worker
// thread each take an immutable copy and no lock is held for long.
struct Settings {
    bool enabled = true;
    std::wstring dataSourceUrl = L"https://holiday.ailcc.com/api/holiday/ics";
    int refreshIntervalHours = 24;
    bool replaceText = true;
    std::wstring offDayText;
    std::wstring workdayText = L"补班";
    winrt::Windows::UI::Color offDayColor{255, 78, 161, 255};
    winrt::Windows::UI::Color workdayColor{255, 255, 90, 90};
};

std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settings = std::make_shared<Settings>();

Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return *g_settings;
}

std::wstring ReadStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    if (value) {
        Wh_FreeStringSetting(value);
    }
    return result;
}

// Colour parsing, and why so many written forms are accepted: Windhawk 2.0's colour
// picker writes "#FF4EA1FF" (eight digits, AARRGGBB), while a value typed by hand is
// usually "4EA1FF" (six digits, no '#', taken as opaque). Both lengths, with or
// without the '#', are accepted; a value which does not parse returns nullopt and the
// caller falls back to the default colour instead of turning the calendar black.
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

// A colour setting which can't be read (an empty value, or a configuration file
// from an older version of the mod) falls back to the default colour.
winrt::Windows::UI::Color ReadColorSetting(PCWSTR name,
                                           winrt::Windows::UI::Color fallback) {
    if (auto color = ParseHexColor(ReadStringSetting(name))) {
        return *color;
    }
    return fallback;
}

void LoadSettings() {
    auto settings = std::make_shared<Settings>();

    settings->enabled = Wh_GetIntSetting(L"enabled") != 0;
    settings->dataSourceUrl = ReadStringSetting(L"dataSourceUrl");
    settings->refreshIntervalHours = Wh_GetIntSetting(L"refreshIntervalHours");
    settings->replaceText = Wh_GetIntSetting(L"replaceText") != 0;
    settings->offDayText = ReadStringSetting(L"offDayText");
    settings->workdayText = ReadStringSetting(L"workdayText");
    settings->offDayColor =
        ReadColorSetting(L"offDayColor", Settings{}.offDayColor);
    settings->workdayColor =
        ReadColorSetting(L"workdayColor", Settings{}.workdayColor);

    if (settings->refreshIntervalHours < 1) {
        settings->refreshIntervalHours = 1;
    }
    if (settings->dataSourceUrl.empty()) {
        settings->dataSourceUrl = Settings{}.dataSourceUrl;
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = settings;
    }

}

////////////////////////////////////////////////////////////////////////////////
// Small helpers

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

struct HolidayInfo {
    std::wstring name;
    bool isOffDay = false;
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

bool LookupHoliday(int32_t key, HolidayInfo& out) {
    std::lock_guard<std::mutex> lock(g_holidaysMutex);
    auto it = g_holidays.find(key);
    if (it == g_holidays.end()) {
        return false;
    }
    out = it->second;
    return true;
}

// The feed covers every year at once, so the data replaces the old data.
void StoreHolidays(HolidayMap parsed) {
    std::lock_guard<std::mutex> lock(g_holidaysMutex);
    g_holidays = std::move(parsed);
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

// "元旦节（休）" -> name "元旦节", day off. "春节（班）" -> name "春节",
// adjusted workday.
//
// The default feed also carries solar terms, western holidays and other plain
// annotations, without a marker: those are not statutory holidays, so an entry
// only counts when it carries a （休）/（班） style marker.
bool ClassifyIcsSummary(std::wstring const& summary,
                        std::wstring& name,
                        bool& isOffDay) {
    std::wstring text = UnescapeIcsText(summary);
    isOffDay = true;

    struct Marker {
        const wchar_t* text;
        bool isOffDay;
    };
    static const Marker kMarkers[] = {
        {L"（补班）", false}, {L"(补班)", false}, {L"【补班】", false}, {L"[补班]", false},
        {L"（班）", false},   {L"(班)", false},   {L"【班】", false},   {L"[班]", false},
        {L"（休）", true},    {L"(休)", true},    {L"【休】", true},    {L"[休]", true},
        {L"（放假）", true},  {L"(放假)", true},
    };

    bool marked = false;
    for (const auto& marker : kMarkers) {
        size_t pos = text.find(marker.text);
        if (pos != std::wstring::npos) {
            text.erase(pos, wcslen(marker.text));
            isOffDay = marker.isOffDay;
            marked = true;
            break;
        }
    }

    // A trailing "休"/"班" separated from the name, e.g. "元旦节 休".
    if (!marked) {
        std::wstring trimmed = TrimWhitespace(text);
        if (trimmed.size() >= 2) {
            wchar_t last = trimmed[trimmed.size() - 1];
            wchar_t before = trimmed[trimmed.size() - 2];
            if ((last == L'休' || last == L'班') &&
                (iswspace(before) || before == L'-' || before == L'/' ||
                 before == L'·' || before == L'—')) {
                isOffDay = last == L'休';
                marked = true;
                text = trimmed.substr(0, trimmed.size() - 1);
            }
        }
    }

    if (!marked) {
        return false;
    }

    name = TrimWhitespace(text);
    return !name.empty();
}

// Reported once per process: a source which uses these fields would show too few
// dates, and the log line says why.
std::atomic<bool> g_loggedRecurrence{false};

// The most dates a feed may describe. The address of the data source can be
// changed, and an event written as "from 1900 to 2100" would otherwise keep the
// worker thread busy for tens of thousands of dates, which would also hold up
// the next refresh and the shutdown.
constexpr size_t kMaxHolidayDates = 20000;

bool ParseIcs(std::wstring const& text, HolidayMap& out) {
    auto lines = SplitIcsLines(text);

    bool inEvent = false;
    bool hasRecurrence = false;
    bool hitDateLimit = false;
    bool hasEnd = false;
    size_t droppedRanges = 0;
    int32_t start = 0;
    int32_t end = 0;
    std::wstring summary;
    size_t added = 0;

    auto flushEvent = [&]() {
        if (!start) {
            return;
        }
        if (added >= kMaxHolidayDates) {
            // An earlier event already filled the feed: this one is dropped.
            hitDateLimit = true;
            return;
        }

        std::wstring name;
        bool isOffDay = true;
        if (!ClassifyIcsSummary(summary, name, isOffDay)) {
            return;
        }

        if (!hasEnd) {
            // No DTEND at all: a zero-length event, which is the single day it
            // starts on. That is what a feed which spells out one day per event
            // sends, and the only case which is read that way.
            end = NextDateKey(start);
        } else if (end <= start) {
            // A DTEND which is there but does not make a range: it did not parse,
            // or it is not later than DTSTART. The event is dropped instead of being
            // read as a single day, because a wrong end would put the holiday name
            // on dates the feed never named.
            droppedRanges++;
            return;
        }

        for (int32_t key = start; key && key < end; key = NextDateKey(key)) {
            // The limit is looked at inside the loop as well: a single event
            // which spans millions of days is what a limit at the start of the
            // event does not catch.
            if (added >= kMaxHolidayDates) {
                hitDateLimit = true;
                break;
            }

            HolidayInfo info;
            info.name = name;
            info.isOffDay = isOffDay;
            out[key] = std::move(info);
            added++;
        }
    };

    for (const auto& line : lines) {
        if (line == L"BEGIN:VEVENT") {
            inEvent = true;
            start = 0;
            end = 0;
            hasEnd = false;
            summary.clear();
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
            start = ParseDateKey(value);
        } else if (name == L"DTEND") {
            end = ParseDateKey(value);
            hasEnd = true;
        } else if (name == L"SUMMARY") {
            summary = value;
        } else if (name == L"RRULE" || name == L"EXDATE" ||
                   name == L"RECURRENCE-ID") {
            // A recurring event is read as the one occurrence it names. The
            // default source doesn't use any of these fields.
            hasRecurrence = true;
        }
    }

    if (hasRecurrence && !g_loggedRecurrence.exchange(true)) {
        Wh_Log(
            L"The data source uses recurrence rules (RRULE, EXDATE or "
            L"RECURRENCE-ID); only the occurrence each event names is read");
    }

    if (hitDateLimit) {
        Wh_Log(L"The data source describes more than %zu holiday dates; the "
               L"rest is ignored",
               kMaxHolidayDates);
    }

    if (droppedRanges) {
        Wh_Log(L"%zu events are dropped: their DTEND is missing a date, or does "
               L"not come after their DTSTART",
               droppedRanges);
    }

    return !out.empty();
}


// The data source is an iCalendar feed: the only format the mod reads.
bool ParseHolidayData(std::wstring const& body, HolidayMap& out) {
    if (body.find(L"BEGIN:VEVENT") == std::wstring::npos &&
        body.find(L"BEGIN:VCALENDAR") == std::wstring::npos) {
        Wh_Log(L"The data source did not return an iCalendar feed");
        return false;
    }

    return ParseIcs(body, out);
}

////////////////////////////////////////////////////////////////////////////////
// Background fetching

void SweepDayItems();
void RefreshCalendarOnUiThreads();

// The worker thread is created at initialisation, and a failed start is retried
// from a refresh request (see RequestHolidayData). The mutex is what keeps two
// threads which ask at the same time from creating two threads or two event pairs.
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

std::wstring BuildUrl(std::wstring urlTemplate, int year) {
    std::wstring yearText = std::to_wstring(year);
    size_t pos;
    while ((pos = urlTemplate.find(L"{year}")) != std::wstring::npos) {
        urlTemplate.replace(pos, 6, yearText);
    }
    return urlTemplate;
}

// Fetching and parsing both happen on the worker thread: Wh_GetUrlContent blocks on
// network I/O, and doing that on a UI thread would hold up the taskbar. A parse which
// fails does not overwrite the data which is already there, so the calendar keeps
// showing the last good result when the network or the API is down (g_haveData only
// decides which retry delay applies).
bool FetchHolidayData() {
    Settings settings = GetSettings();

    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        g_lastFetchAttemptTick = GetTickCount64();
    }

    if (settings.dataSourceUrl.empty()) {
        return false;
    }

    SYSTEMTIME now{};
    GetLocalTime(&now);
    std::wstring url = BuildUrl(settings.dataSourceUrl, now.wYear);
    Wh_Log(L"Fetching %s", url.c_str());

    const WH_URL_CONTENT* content = Wh_GetUrlContent(url.c_str(), nullptr);
    if (!content) {
        Wh_Log(L"Wh_GetUrlContent returned null");
        return false;
    }

    std::wstring body;
    bool ok = content->statusCode == 200 && content->data && content->length;
    if (ok) {
        body = Utf8ToWide(content->data, content->length);
    } else {
        Wh_Log(L"Wh_GetUrlContent returned HTTP status %d", content->statusCode);
    }
    Wh_FreeUrlContent(content);

    if (!ok || body.empty()) {
        return false;
    }

    HolidayMap parsed;
    if (!ParseHolidayData(body, parsed)) {
        Wh_Log(L"Failed to parse the holiday data (%zu characters)", body.size());
        return false;
    }

    size_t entryCount = parsed.size();
    StoreHolidays(std::move(parsed));

    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        g_haveData = true;
    }

    Wh_Log(L"Loaded %zu holiday dates from the data source", entryCount);
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
// asks for the thread again on every refresh, so a start which failed at
// initialisation is retried instead of lasting for the whole session.
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

void StopWorkerThread() {
    std::lock_guard<std::mutex> lock(g_workerThreadMutex);

    HANDLE workerThread = g_hWorkerThread.exchange(nullptr);

    if (HANDLE stopEvent = g_hStopEvent.load()) {
        SetEvent(stopEvent);
    }
    if (workerThread) {
        WaitForSingleObject(workerThread, INFINITE);
        CloseHandle(workerThread);
    }
    if (HANDLE workEvent = g_hWorkEvent.exchange(nullptr)) {
        CloseHandle(workEvent);
    }
    if (HANDLE stopEvent = g_hStopEvent.exchange(nullptr)) {
        CloseHandle(stopEvent);
    }
}

// The decision whether a fetch is due lives here: no repeated request inside the
// refresh interval, and a shorter 10 minute retry interval after a failure. "force" is
// only for the case where the data has to be re-read at once, such as a changed URL;
// the callers pass false today and get the same effect by clearing the attempt tick
// through ResetHolidayData.
void RequestHolidayData(bool force) {
    Settings settings = GetSettings();
    if (!settings.enabled || g_shuttingDown) {
        return;
    }

    // The worker thread is started once at initialisation; a start which failed
    // there is retried here, on every request. This has to happen before the "is
    // the data still fresh" test below, because a request which could not be
    // answered must not consume the refresh interval. StartWorkerThread takes a
    // lock, so a request from another thread cannot create a second one.
    if (!g_hWorkerThread.load()) {
        StartWorkerThread();
    }

    const ULONGLONG refreshInterval =
        static_cast<ULONGLONG>(settings.refreshIntervalHours) * 60ULL * 60ULL * 1000ULL;

    {
        std::lock_guard<std::mutex> lock(g_fetchStateMutex);
        if (!force && g_lastFetchAttemptTick) {
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
    }
}

////////////////////////////////////////////////////////////////////////////////
// Calendar UI

////////////////////////////////////////////////////////////////////////////////
// Looking inside a day cell

// CalendarViewDayItem.Date is UTC midnight (00:00Z). The date is therefore taken from
// the local time: reading it as UTC would put every cell one day back on a machine
// with a negative offset. A conversion which fails returns 0, and the caller skips
// that cell - writing nothing is better than writing the wrong date.
int32_t DateKeyFromDateTime(wf::DateTime const& dateTime) {
    ULARGE_INTEGER uli;
    uli.QuadPart = static_cast<ULONGLONG>(dateTime.time_since_epoch().count());

    FILETIME fileTime;
    fileTime.dwLowDateTime = uli.LowPart;
    fileTime.dwHighDateTime = uli.HighPart;

    SYSTEMTIME systemTimeUtc{};
    SYSTEMTIME systemTimeLocal{};
    if (!FileTimeToSystemTime(&fileTime, &systemTimeUtc)) {
        return 0;
    }
    if (!SystemTimeToTzSpecificLocalTime(nullptr, &systemTimeUtc,
                                         &systemTimeLocal)) {
        return 0;
    }

    return MakeDateKey(systemTimeLocal.wYear, systemTimeLocal.wMonth,
                       systemTimeLocal.wDay);
}

// The name the Windows 11 calendar gives the text block which holds the lunar
// date or the solar term.
// It is a strong hint rather than the only rule: another Windows build may name the
// block differently, which is why FindLabelTextBlock has fallbacks (design note 5).
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

// A day item of this thread, with the identity it is tracked by. The calendar
// reports a day item as added again every time it is put back into the calendar,
// which is what a month change does, so an item is tracked here only once.
struct TrackedDayItem {
    void* id = nullptr;
    winrt::weak_ref<wuxc::CalendarViewDayItem> item{nullptr};
};

// The most day items to keep an eye on. A day item which the calendar no longer
// shows is normally released, and is dropped on the next pass; a day item which
// the shell keeps alive in a pool would stay forever, so the list is bounded. A
// day item which is shown again is reported as added again, and tracked again.
constexpr size_t kMaxTrackedDayItems = 128;

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

// Remembers a day item the shell reported. A repeated report of a day item which
// is already tracked only makes it the most recent one, so that the list holds
// the day items the calendar used last.
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
// still costs a layout pass, and every element mutation is reported back to the
// mod.
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
    }
}

// Whether a text is one the mod could have written. A cell which the shell reused
// for another month still holds the mod's text of the old date, and that text is
// not worth remembering as the shell's own.
// appliedText cannot cover that window, because the shell recycles a cell by clearing
// what it holds while the mod's own holiday name is still on it; the name is then the
// only thing left to recognise. Design note 7 has the rest of the trade-off.
bool IsOwnText(std::wstring const& text, Settings const& settings) {
    if (text.empty()) {
        return false;
    }
    if (text == settings.workdayText || text == settings.offDayText) {
        return true;
    }

    std::lock_guard<std::mutex> lock(g_holidaysMutex);
    for (const auto& entry : g_holidays) {
        if (entry.second.name == text) {
            return true;
        }
    }

    return false;
}

// The text a cell should show. The settings may contain {name}, which stands for
// the holiday name from the data source, and an empty text means "leave the text
// alone and only colour the day".
std::wstring ExpandName(std::wstring const& text, std::wstring const& name) {
    if (text.empty()) {
        return {};
    }

    // Every "{name}" is replaced, like the "{year}" of the URL template: a text
    // which says it twice means it twice. The result is built up instead of
    // replacing in place so that a name which itself contains "{name}" cannot
    // send the loop around forever.
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

// One day cell, in full. The holiday name and its colour go into the cell's lunar text
// block, which is all the shell's day cell has room for:
//   1) turn the cell's Date into a yyyymmdd key;
//   2) look the key up, which says whether the day is a day off and what the holiday
//      is called;
//   3) find the lunar text block, anew on every pass (design note 5);
//   4) decide the text: keep it, use the holiday name, or use the fixed text from the
//      settings;
//   5) decide the colour: the day-off or the workday colour, and put the shell's own
//      colour back for a day which is not a holiday any more;
//   6) remember what was written, so that it can be restored later.
//
// Nothing is written while the cell already shows the wanted value: every write costs
// a layout pass and is reported back to the mod through the diagnostics, which is
// where the flicker and the log spam of the early versions came from.
void ApplyToDayItem(wuxc::CalendarViewDayItem const& item) {
    Settings settings = GetSettings();
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
    // A disabled mod behaves like a day which is not a holiday: whatever the mod
    // wrote is taken back.
    bool hasInfo = settings.enabled && LookupHoliday(key, info);

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
            Wh_Log(L"No text block to replace in the day cell of %d", key);
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
    // With "replace the lunar text" switched off, the cell keeps the text of the
    // shell and only the colour is written.
    if (hasInfo && settings.replaceText) {
        if (info.isOffDay) {
            desired = settings.offDayText.empty()
                          ? info.name
                          : ExpandName(settings.offDayText, info.name);
        } else {
            desired = ExpandName(settings.workdayText, info.name);
        }
    }

    std::wstring current = ElementText(label);

    if (!desired.empty() && current != desired) {
        if (current != cell.appliedText && !IsOwnText(current, settings)) {
            // The shell's own text is being replaced: remember it.
            cell.savedText = current;
            cell.hasSavedText = true;
        }
        try {
            label.Text(desired);
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
            }
        } catch (...) {
        }
        cell.hasSavedText = false;
    }

    cell.appliedText = desired;

    // The colour is what marks the day, whichever text the cell ends up showing,
    // so it does not depend on the text having been replaced. The shell's own
    // colour is saved right before the first colour write: the colour is written
    // with "replace the lunar text" switched off as well, and reading it back
    // later would save the mod's own brush as the original one once the shell has
    // rewritten the text of a cell the mod has already coloured.
    if (hasInfo) {
        SaveForeground(cell, label);
        cell.ownBrush = SetForegroundIfDifferent(
            label,
            info.isOffDay ? settings.offDayColor : settings.workdayColor,
            cell.ownBrush);
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
// callback are gone. The cells are therefore looked at again from time to time
// while the calendar is alive. Only a cell whose text the shell took back is
// written to, so a pass over an unchanged calendar costs a few comparisons.
thread_local winrt::Windows::System::DispatcherQueueTimer t_sweepTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    t_sweepTimerRevoker;

constexpr int kSweepIntervalMs = 1000;

// Starts the periodic re-check, if it isn't running already. The timer is
// stopped again once there are no day items left, and started again here when
// the calendar is opened.
// The interval is one second because the shell writes its lunar text "a moment later"
// after a month change, and the case where it replaces the text block and the callback
// with it is what the sweep covers. Since the UI is written only when a value differs,
// an idle second costs a few dozen property reads and nothing else. Do not shorten the
// interval, which would touch the shell's elements more often, and do not remove it,
// which would miss that case.
void EnsureSweepTimer() {
    try {
        if (!t_sweepTimer) {
            auto dispatcherQueue =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (!dispatcherQueue) {
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

// Applies the mod's data to every day item of this thread. Runs on every timer
// tick, and after the data or the settings changed.
// Again, only a cell which does not show what the mod wants is written to, so a pass
// over an unchanged calendar does nothing at all.
void SweepDayItems() {
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

    // No day items left: the calendar was closed, and the timer is started again
    // when it is opened.
    if (t_dayItems.empty() && t_sweepTimer) {
        try {
            t_sweepTimer.Stop();
        } catch (...) {
        }
    }
}

// Called for every day cell the shell creates, and for every one it puts back
// into the calendar when the displayed month changes.
// The shell re-reports a day item as added every time it puts one back, which a month
// change does, so this is called repeatedly; TrackDayItem deduplicates, which keeps the
// list from growing with use (design note 8).
void HandleDayItemAdded(wuxc::CalendarViewDayItem const& item) {
    TrackDayItem(item);

    ApplyToDayItem(item);
    EnsureSweepTimer();
    RequestHolidayData(false);
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

    t_cells.clear();
    t_dayItems.clear();
    t_labelOwners.clear();

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
// Diagnostics releases

thread_local std::vector<InstanceHandle> g_pendingDiagnosticsRelease;
thread_local ULONGLONG g_lastDiagnosticsReleaseQueueTick;
thread_local bool g_diagnosticsReleaseDrainQueued;
thread_local winrt::Windows::System::DispatcherQueueTimer
    g_diagnosticsReleaseDrainTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    g_diagnosticsReleaseDrainTimerTickRevoker;

// Long enough to sit out a tree being built.
constexpr ULONGLONG kDiagnosticsReleaseDelay = 200;
constexpr int kDiagnosticsReleaseDrainDelay = 50;

// The diagnostics cache a strong reference for every element which was reported, and
// give it back only through UnregisterInstance: without that, elements are never freed
// and the process only grows. The reference cannot be given back inside the report
// callback, which arrives from inside XAML's own enter and leave walks, where dropping
// the last reference destroys the element the walk is still visiting. The handles are
// therefore queued first, and given back on a one-shot timer once the reports have been
// quiet for 200 ms (plus the 50 ms delay of the timer itself), when the walk is over.
void FlushDiagnosticsReleases() {
    auto pending = std::move(g_pendingDiagnosticsRelease);
    g_pendingDiagnosticsRelease.clear();

    VisualTreeWatcher* watcher = g_visualTreeWatcher.get();
    if (!watcher) {
        return;
    }

    // A handle is queued once per report naming it, so a parent appears once
    // per child.
    std::sort(pending.begin(), pending.end());
    pending.erase(std::unique(pending.begin(), pending.end()), pending.end());

    for (InstanceHandle handle : pending) {
        watcher->ReleaseDiagnosticsReference(handle);
    }
}

void QueueDiagnosticsRelease(InstanceHandle handle) {
    if (!handle) {
        return;
    }

    g_pendingDiagnosticsRelease.push_back(handle);
    g_lastDiagnosticsReleaseQueueTick = GetTickCount64();
}

void DrainDiagnosticsReleases() {
    g_diagnosticsReleaseDrainQueued = false;
    FlushDiagnosticsReleases();
}

// Reports arrive from inside XAML's own Enter and Leave walks, and a release
// there re-enters the diagnostics while the tree is being mutated: dropping the
// last reference to an element the walk is still visiting destroys it mid-walk.
// The drain therefore waits on a one-shot timer, which the thread teardown can
// stop, rather than on a dispatcher item, which it cannot.
void FlushDiagnosticsReleasesIfQuiet() {
    if (g_pendingDiagnosticsRelease.empty() || g_diagnosticsReleaseDrainQueued ||
        GetTickCount64() - g_lastDiagnosticsReleaseQueueTick <
            kDiagnosticsReleaseDelay) {
        return;
    }

    try {
        if (!g_diagnosticsReleaseDrainTimer) {
            auto dispatcherQueue =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (!dispatcherQueue) {
                Wh_Log(L"No dispatcher queue, elements will be held");
                return;
            }

            g_diagnosticsReleaseDrainTimer = dispatcherQueue.CreateTimer();
            g_diagnosticsReleaseDrainTimer.IsRepeating(false);
            g_diagnosticsReleaseDrainTimer.Interval(
                std::chrono::milliseconds{kDiagnosticsReleaseDrainDelay});
            g_diagnosticsReleaseDrainTimerTickRevoker =
                g_diagnosticsReleaseDrainTimer.Tick(
                    winrt::auto_revoke,
                    [](winrt::Windows::System::DispatcherQueueTimer const&,
                       wf::IInspectable const&) { DrainDiagnosticsReleases(); });
        }

        g_diagnosticsReleaseDrainTimer.Start();
        g_diagnosticsReleaseDrainQueued = true;
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }
}

void StopDiagnosticsReleases() {
    g_pendingDiagnosticsRelease.clear();

    if (g_diagnosticsReleaseDrainTimer) {
        try {
            g_diagnosticsReleaseDrainTimer.Stop();
        } catch (...) {
        }
    }

    g_diagnosticsReleaseDrainTimerTickRevoker.revoke();
    g_diagnosticsReleaseDrainTimer = nullptr;
    g_diagnosticsReleaseDrainQueued = false;
}

////////////////////////////////////////////////////////////////////////////////
// Process plumbing

enum class Target {
    ShellExperienceHost,
    ShellHost,
};

Target g_target = Target::ShellExperienceHost;

void InitializeForCurrentThread() {
    if (g_initializedForThread) {
        return;
    }

    g_initializedForThread = true;
}

void UninitializeForCurrentThread() {
    if (!g_initializedForThread) {
        return;
    }

    UninitializeDayItemsForCurrentThread();
    StopDiagnosticsReleases();

    g_initializedForThread = false;
}

void InitializeSettingsAndTap() {
    if (g_initialized.exchange(true)) {
        return;
    }

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"Error %08X", hr);
    }
}

void UninitializeSettingsAndTap() {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    g_initialized = false;
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
    static const UINT runFromWindowThreadRegisteredMsgViaPostMessage =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
        HHOOK hook;
    };

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
                    runFromWindowThreadRegisteredMsgViaPostMessage) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)msg->lParam;
                    if (param) {
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
    if (!PostMessage(hWnd, runFromWindowThreadRegisteredMsgViaPostMessage, 0,
                     (LPARAM)param)) {
        UnhookWindowsHookEx(hook);
        delete param;
        return false;
    }

    return true;
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName, PCSTR funcName) {
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    switch (g_target) {
        case Target::ShellExperienceHost:
            if (bTextualClassName &&
                _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                Wh_Log(L"Initializing - created core window: %08X via %S",
                       (DWORD)(ULONG_PTR)hWnd, funcName);
                InitializeForCurrentThread();
                InitializeSettingsAndTap();
            }
            break;

        case Target::ShellHost:
            if (bTextualClassName &&
                _wcsicmp(lpClassName, L"ControlCenterWindow") == 0) {
                Wh_Log(L"Initializing - created ControlCenterWindow: %08X via %S",
                       (DWORD)(ULONG_PTR)hWnd, funcName);
                // Initializing at this point is too early and doesn't work.
                RunFromWindowThreadViaPostMessage(
                    hWnd,
                    [](PVOID) {
                        InitializeForCurrentThread();
                        InitializeSettingsAndTap();
                    },
                    nullptr);
            }
            break;
    }
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

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExW_Original;

// The handles this hook hands out in place of the XAML debug key, which are the only
// ones the RegQueryValueExW hook below answers. Remembering them is what keeps that
// answer narrow: answering "1" for every query of a value named DisableCompositionDiag
// would also answer any other code on the thread which happens to ask for that name. The
// substituted key is a new handle value on every open, so there is no fixed HKEY to
// compare against. The list is cleared before and after every AdviseVisualTreeChange, so
// a handle which is closed and re-issued by the system cannot survive across one call,
// and no RegCloseKey hook is needed either.
//
// The key is opened and queried from inside AdviseVisualTreeChange, which the mod calls
// on a thread of its own, but the thread a query arrives on is not something to depend
// on, so the handles are kept for every thread as well.
std::mutex g_fakedDebugKeysMutex;
std::vector<HKEY> g_fakedDebugKeys;
thread_local std::vector<HKEY> t_fakedDebugKeys;

void RememberFakedDebugKey(HKEY key) {
    if (!key) {
        return;
    }

    t_fakedDebugKeys.push_back(key);
    std::lock_guard<std::mutex> lock(g_fakedDebugKeysMutex);
    g_fakedDebugKeys.push_back(key);
}

void ForgetFakedDebugKeys() {
    t_fakedDebugKeys.clear();
    std::lock_guard<std::mutex> lock(g_fakedDebugKeysMutex);
    g_fakedDebugKeys.clear();
}

bool IsFakedDebugKey(HKEY key) {
    for (HKEY faked : t_fakedDebugKeys) {
        if (faked == key) {
            return true;
        }
    }

    std::lock_guard<std::mutex> lock(g_fakedDebugKeysMutex);
    for (HKEY faked : g_fakedDebugKeys) {
        if (faked == key) {
            return true;
        }
    }

    return false;
}

LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey,
                                  LPCWSTR lpSubKey,
                                  DWORD ulOptions,
                                  REGSAM samDesired,
                                  PHKEY phkResult) {
    LSTATUS result = RegOpenKeyExW_Original(hKey, lpSubKey, ulOptions,
                                            samDesired, phkResult);
    if (result == ERROR_SUCCESS || !g_reportCompositionDiagAsDisabled ||
        hKey != HKEY_LOCAL_MACHINE || !lpSubKey ||
        _wcsicmp(lpSubKey, L"Software\\Microsoft\\XAML\\Debug") != 0) {
        return result;
    }

    // The key usually doesn't exist, and the value isn't queried unless the key
    // could be opened, so hand out a key which does exist.
    Wh_Log(L"Substituting the XAML debug key");
    LSTATUS substituted = RegOpenKeyExW_Original(
        HKEY_LOCAL_MACHINE, L"Software\\Microsoft", ulOptions, samDesired,
        phkResult);
    if (substituted == ERROR_SUCCESS && phkResult) {
        RememberFakedDebugKey(*phkResult);
    }
    return substituted;
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;
// "The value is 1" is answered only for a handle the mod faked itself. Every part of the
// condition is needed: the flag says this is that one read, the value name says it is
// that value, and the handle says it is the key the mod handed out.
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey,
                                     LPCWSTR lpValueName,
                                     LPDWORD lpReserved,
                                     LPDWORD lpType,
                                     LPBYTE lpData,
                                     LPDWORD lpcbData) {
    if (!g_reportCompositionDiagAsDisabled || !lpValueName ||
        _wcsicmp(lpValueName, L"DisableCompositionDiag") != 0 ||
        !IsFakedDebugKey(hKey)) {
        return RegQueryValueExW_Original(hKey, lpValueName, lpReserved, lpType,
                                         lpData, lpcbData);
    }

    Wh_Log(L"Reporting DisableCompositionDiag as set");

    if (lpType) {
        *lpType = REG_DWORD;
    }

    if (lpData && (!lpcbData || *lpcbData < sizeof(DWORD))) {
        if (lpcbData) {
            *lpcbData = sizeof(DWORD);
        }
        return ERROR_MORE_DATA;
    }

    if (lpData) {
        *reinterpret_cast<DWORD*>(lpData) = 1;
    }

    if (lpcbData) {
        *lpcbData = sizeof(DWORD);
    }

    return ERROR_SUCCESS;
}

std::vector<HWND> GetCoreWnds() {
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

            switch (g_target) {
                case Target::ShellExperienceHost:
                    if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                        param.hWnds->push_back(hWnd);
                    }
                    break;

                case Target::ShellHost:
                    if (_wcsicmp(szClassName, L"ControlCenterWindow") == 0) {
                        param.hWnds->push_back(hWnd);
                    }
                    break;
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

void RefreshCalendarOnUiThreads() {
    for (HWND hCoreWnd : GetCoreWnds()) {
        RunFromWindowThread(hCoreWnd, [](PVOID) { SweepDayItems(); }, nullptr);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk lifecycle

// Installs one hook and says so when it does not work. A hook which silently does
// not get installed shows up much later, as "the mod does nothing", which is far
// harder to read than the line this writes. The name is a narrow string because
// GetProcAddress takes one, and %S prints it from this wide format.
bool InstallHook(HMODULE module, PCSTR functionName, void* hook, void** original) {
    if (!module) {
        Wh_Log(L"Initialization is incomplete: the module of %S is not loaded",
               functionName);
        return false;
    }

    void* target = (void*)GetProcAddress(module, functionName);
    if (!target) {
        Wh_Log(L"Initialization is incomplete: %S is not in its module",
               functionName);
        return false;
    }

    if (!Wh_SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Initialization is incomplete: the hook of %S could not be set",
               functionName);
        return false;
    }

    return true;
}

// Why the hooks are set here: CreateWindowInBand(Ex) has to be hooked before a window is
// created, otherwise a window which existed before the mod was loaded, or one being made
// while it loads, is missed and never gets its holiday marks. The worker thread which
// fetches the data is started here as well.
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
    StartWorkerThread();

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    InstallHook(user32Module, "CreateWindowInBand", (void*)CreateWindowInBand_Hook,
                (void**)&CreateWindowInBand_Original);
    InstallHook(user32Module, "CreateWindowInBandEx",
                (void*)CreateWindowInBandEx_Hook,
                (void**)&CreateWindowInBandEx_Original);

    // These two are what makes the XAML diagnostics report the visual tree to the
    // mod: without them AdviseVisualTreeChange answers with an error (the watcher
    // thread logs it) and the calendar is never touched. They are therefore reported
    // as the hooks the mod depends on. The two calls are not combined with &&, so
    // that a failing first one does not keep the second from being tried.
    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    const bool openKeyHook =
        InstallHook(kernelBaseModule, "RegOpenKeyExW", (void*)RegOpenKeyExW_Hook,
                    (void**)&RegOpenKeyExW_Original);
    const bool queryValueHook = InstallHook(
        kernelBaseModule, "RegQueryValueExW", (void*)RegQueryValueExW_Hook,
        (void**)&RegQueryValueExW_Original);
    if (!openKeyHook || !queryValueHook) {
        Wh_Log(L"Initialization is incomplete: the calendar will not be modified "
               L"without the registry hooks");
    }

    return TRUE;
}

// Initialised after the load: a window which is already there gets its
// InitializeForCurrentThread here, so that a mod which is loaded after the window was
// created is not missed, and the data is fetched up front so that the calendar has
// content the first time it is opened.
void Wh_ModAfterInit() {
    Wh_Log(L">");

    bool initialize = false;

    for (auto hCoreWnd : GetCoreWnds()) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { InitializeForCurrentThread(); }, nullptr);
        initialize = true;
    }

    if (initialize) {
        InitializeSettingsAndTap();
    }

    // Fetch the data up front so that the calendar is populated the first time
    // it is opened.
    RequestHolidayData(false);
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Before anything else: a refresh request which arrives while the mod is being
    // unloaded must not start a worker thread again.
    g_shuttingDown = true;

    StopWorkerThread();
    UninitializeSettingsAndTap();

    for (auto hCoreWnd : GetCoreWnds()) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { UninitializeForCurrentThread(); }, nullptr);
    }
}

// Changing the settings: only a changed data source clears the data which was fetched,
// because the other settings (colours, texts, whether to replace) are only a different
// way of drawing the same data, and the cells can be repainted from it. Dropping the
// data instead would make the calendar fall back to the lunar text until the next fetch.
// Data which is still inside the refresh interval is not fetched again either, so
// changing a colour does not cost a request.
void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    const std::wstring previousUrl = GetSettings().dataSourceUrl;
    LoadSettings();
    const Settings settings = GetSettings();

    // The holidays of one data source are not the holidays of another, but the
    // data of the same source is kept: dropping it would make the calendar fall
    // back to the lunar text until the next fetch is done.
    if (settings.dataSourceUrl != previousUrl) {
        ResetHolidayData();
    }

    // Re-apply the new settings to the day items which are already on screen.
    RefreshCalendarOnUiThreads();

    // Data which is still fresh is not fetched again, so that changing a colour
    // doesn't cost a request.
    RequestHolidayData(false);
}
