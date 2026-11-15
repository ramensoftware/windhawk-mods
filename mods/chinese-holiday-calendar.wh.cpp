// ==WindhawkMod==
// @id              chinese-holiday-calendar
// @name            Lunar & Holiday Calendar
// @name:zh-CN      农历与节假日日历
// @name:vi-VN      Lịch âm & ngày lễ
// @description     Write your own text and colour into the Windows 11 calendar: holidays from an iCalendar (.ics) feed, dates matched by your rules, and optionally the Vietnamese lunar calendar
// @description:zh-CN 在 Windows 11 日历里写自己的文字和颜色：按规则标记 ICS 订阅中的节假日，或直接标记农历/阳历日期；也可以换成越南农历
// @description:vi-VN Viết chữ và màu vào bảng lịch Windows 11: ngày lễ từ link iCalendar (.ics) hoặc ngày do quy tắc của bạn chọn; có thể hiện lịch âm Việt Nam
// @version         1.0
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
**Languages:** English · 中文 · Tiếng Việt

![Lunar & Holiday Calendar View](https://i.imgur.com/SBafHNL.png)

# Lunar & Holiday Calendar

The Windows 11 calendar flyout prints a lunar date, solar term or festival name in each day
cell. This mod writes holiday text and colour into that line by rules you write; rules can
also mark a lunar or solar date directly. Feeds from any country work.

- **Default**: days marked `休` show the holiday name in blue; days marked `班` show `补班` in red.
- Other days keep the system's line; with the Vietnamese lunar calendar on, they show the
  Vietnamese lunar date.

## Requirements

The mod writes into the **second line of a day cell** (usually the one which shows the lunar
date, solar term or festival name), so that line has to be on:

Settings -> Time & language -> Date & time -> "Show additional calendars in the taskbar"
-> Simplified Chinese (Lunar)

With that line switched off a day cell usually has nothing to write into, and no mark
shows up on the calendar. The Vietnamese lunar calendar below writes into that same line.
When a cell already has content but no usable system-named second-line text block (missing or
empty), the mod skips it instead of guessing at another one and says so in the log.

## Settings

| Setting | What to fill in |
| --- | --- |
| Data source (ICS) | The address of your `.ics` feed. **Empty by default, and then no feed is read**; leave it empty if you only mark days with rules. |
| Refresh interval (hours) | How often the feed is read again (24 h by default). |
| Vietnamese lunar calendar | Day cells show the Vietnamese lunar date by default; a Chinese lunar date at the top of the calendar is written in Vietnamese too. See below. |
| Tooltip on hover | The full name (or the text your rule writes) when the pointer rests on a marked day. Useful when a long name does not fit in the cell. |
| Highlight rules | Which days are marked, in which colour and with which text. See below. |
| Text to remove | The pieces of the text of an event which are cut off it (default `（休）` and `（班）`). See below. |

Changing the rules or the text to remove does not force a new download: the data already
read is marked again with the new settings.

## Vietnamese lunar calendar

With "Vietnamese lunar calendar (replace the lunar text)" on:

- a day cell shows the Vietnamese lunar date (`15/8`; `1/2N` in a leap month) unless a rule
  gives another text;
- when the lunar date line at the top of the calendar is in the Chinese format, it is written
  in Vietnamese (day, month and Can Chi year, e.g. `15 Tháng 8 Năm Bính Ngọ`);
- the dates are calculated offline (UTC+7) and need no feed.

The Vietnamese holidays are not built into the mod: they come from a feed, or from a rule
written below.

## Highlight rules

One item per kind of day, three fields each:

    Word: 休        Colour: #4EA1FF        Text: {name}

- **Word**: one word; an event is marked when its text contains it. A `*` rule takes every
  event no other rule matched, wherever it appears in the list.
- **Colour**: `#RRGGBB`; empty sets no colour.
- **Text**: `{name}` is the event text (after "Text to remove"); empty uses the cell's own
  line (the system line, or the Vietnamese date).
- A rule with no word, or with neither colour nor text, does nothing. The first match wins.

A few examples, each of them one item of the list:

| Word | Colour | Text | What it does |
| --- | --- | --- | --- |
| `班` | `#FF5A5A` | `补班` | an adjusted workday shows `补班`, in red (default) |
| `休` | `#4EA1FF` | `{name}` | a day off shows the name of the holiday, in blue (default) |
| `*` | `#4EA1FF` | `{name}` | every event of the feed shows its own text, in blue |
| `祝日` | `#E81123` | (empty) | 祝日 of a Japanese feed is coloured, the cell text kept |
| `nghỉ lễ` | `#FF5A5A` | `{name}` | days of a Vietnamese feed which say `nghỉ lễ`, in red |

### Marking a day of the calendar

"Word" may name a day instead of a word, which marks a day no feed carries:

| What to write | What it asks for |
| --- | --- |
| `{lunar: d=15}` | the fifteenth day of every lunar month, leap months included |
| `{lunar: m=8, d=15}` | the fifteenth of the eighth month (not the leap one) |
| `{lunar: m=8L, d=15}` | the fifteenth of the leap eighth month; `{lunar: m=8, d=15, leap}` means the same |
| `{lunar: m=9}` | every day of the ninth month |
| `{solar: m=4, d=30}` | the thirtieth of April |

- `m` is the month and `d` the day, separated by commas, in any order and with any case.
- `m=8L`, or a separate `leap` field, asks for the leap month; a month written without either
  means the ordinary month. A token with only `d` takes leap months too.
- `solar` has no leap month, so `L` and `leap` are not used there.
- A solar day goes up to 31 and a lunar one up to 30; a day the named month does not have
  (30 February, for example) is a typo.
- To mark both the ordinary and the leap month of the same number, write two items:
  `{lunar: m=8, d=15}` and `{lunar: m=8L, d=15}`.
- A token which cannot be read (unknown key, out-of-range value, repeated field, no date)
  marks no day.
- Which calendar a lunar day is read in is what the switch above says: the Vietnamese one with
  it on, the Chinese one the cells show with it off.
- A day whose event is matched by a rule of the feed is settled by that rule; a rule of this
  kind only answers for the days no event rule marks.
- A rule of this kind has no event text, so `{name}` is empty; an empty text keeps the date.
  One item is one date pattern.

A set of Vietnamese holidays (each line is one item of the list):

| Word | Colour | Text | What it does |
| --- | --- | --- | --- |
| `{lunar: d=1}` | `#FF5A5A` | (empty) | the first day of every lunar month, red, the date kept |
| `{lunar: d=15}` | `#FF5A5A` | (empty) | every Rằm, red, the date kept |
| `{lunar: m=1, d=1}` | `#E81123` | `Tết Nguyên đán` | Vietnamese New Year |
| `{lunar: m=3, d=10}` | `#E81123` | `Giỗ tổ Hùng Vương` | the day of the Hùng kings |
| `{lunar: m=5, d=5}` | `#E81123` | `Đoan Ngọ` | the mid-year festival |
| `{lunar: m=8, d=15}` | `#E81123` | `Tết Trung thu` | Mid-Autumn |
| `{lunar: m=12, d=23}` | `#E81123` | `Ông Táo` | the kitchen god |
| `{solar: m=4, d=30}` | `#E81123` | `Ngày Thống nhất` | Reunification Day |
| `{solar: m=9, d=2}` | `#E81123` | `Quốc khánh` | the national day |

## Text to remove

A string written here is removed from the displayed event text exactly as written, every time
it occurs (brackets and spaces included). It changes only the display, not which rule matches.
The two default items are the Chinese markers:

```
（休）       国庆节（休）                     shows  国庆节
```

`（班）` works the same; a fixed rule text still wins. Add other markers as needed, for
example `(nghỉ bù)`. The defaults use full-width brackets; add half-width `(休)`, `(班)` or
another form yourself. Empty the list and nothing is removed.
If the whole name is removed, only the colour or the date stays.

## Data source

The mod ships with no holiday data and requests only the address you enter. Examples:

- Apple iCloud holidays for China: `https://p10-calendars.icloud.com/holiday/CN_zh.ics`
- Apple iCloud holidays for Vietnam: `https://p10-calendars.icloud.com/holiday/VN_vi.ics`
- `https://holiday.ailcc.com/api/holiday/ics` (China, several years at once)

An event of a feed looks like this, and **the word of a rule is looked for in it**:

```
SUMMARY:春节（休）
```

A usual date-based `.ics` feed works. If a refresh fails, marks already loaded stay; with no
data, the calendar still shows the system content (or the Vietnamese lunar calendar) and the
date rules.

## Credits

The way the calendar window is found comes from lonfro's
[Agenda in Calendar View](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/agenda-in-calendar-view.wh.cpp)
(GPL-3.0), and the cross-thread helpers follow m417z's
[Windows 11 Notification Center Styler](https://github.com/m417z/my-windhawk-mods).

The Vietnamese lunar calendar was contributed by [@phatMT97](https://github.com/phatMT97) and
[@vantruong-dev](https://github.com/vantruong-dev), using astronomical calculations based on Jean Meeus's
*Astronomical Algorithms* (2nd Edition 1998, chapters 25 & 49) and Claus Tøndering's Calendar FAQ.

---

# 农历与节假日日历

Windows 11 的日历（点击任务栏右下角的时间弹出）会在日期格里显示农历、节气或节日名。这个 mod 按你写的
规则，把节假日的文字和颜色写进这一行；也可以直接用规则标记农历或阳历日期。任何国家的订阅源都能用。

- **默认**：`休` 标到的日子显示节日名（蓝），`班` 标到的日子显示 `补班`（红）。
- 其他日子保留系统那一行；打开越南农历后显示越南农历日期。

## 使用前提

mod 写的是日期格里的**第二行**（通常就是显示农历、节气或节日名的那一行），所以先把它打开：

设置 → 时间和语言 → 日期和时间 → "在任务栏中显示其他日历" → 选中"简体中文（农历）"

不打开这一行，日期格子里往往就没有可写的地方，日历上也看不出标记——越南农历写的也是这一行。
格子已经有内容、但找不到可用的第二行文本块（不存在或没有文字）时，mod 不会猜别的文本块，直接跳过
这个格子并在日志里说明。

## 设置项

| 设置 | 填什么 |
| --- | --- |
| 数据来源 | 你的 `.ics` 订阅地址。**留空（默认）则不读取订阅源**；只用日期规则标记时可以不填。 |
| 刷新间隔（小时） | 隔多久重新下载一次（默认 24 小时）。 |
| 越南农历（替换农历文字） | 日期格默认显示越南农历；日历顶部的农历日期是中文格式时也改成越南写法。见下文。 |
| 悬停时显示完整文字 | 鼠标停在有标记的日期上，显示完整名称（或规则写的文字）。名字太长、日期格显示不下时用。 |
| 高亮规则 | 哪些日子被标记、标成什么颜色和文字。见下文。 |
| 剔除的文字 | 从事件原文里删掉的那几段文字（默认 `（休）`、`（班）`）。见下文。 |

改规则、改"剔除的文字"不会立即重新下载：mod 会拿已经读到的数据按新设置重新标一遍。

## 越南农历

打开"越南农历（替换农历文字）"后：

- 日期格默认显示越南农历日期，写成 `日/月`（如 `15/8`），闰月写成 `1/2N`；有规则文字时显示
  规则文字。
- 日历顶部的农历日期行是中文格式时，整行换成越南写法：日期、月份和干支年（如
  `15 Tháng 8 Năm Bính Ngọ`）。
- 日期离线计算（UTC+7），不依赖订阅源。

越南的节假日不内置在 mod 里：要么订阅源里有，要么用下面的规则标。

## 高亮规则

一项一类日子，每项填三栏：

    要找的词：休        颜色：#4EA1FF        显示的文字：{name}

- **要找的词**：只填一个词，它出现在事件文字里就算命中。默认两项是"班"和"休"；填 `*` 是**兜底**：
  所有没被别的规则命中过的事件都归它，写在哪一项都可以——别国的订阅源大多没有"休"/"班"，
  加一项填 `*` 就能全标上。
- **颜色**：`#RRGGBB`；留空不设置颜色。
- **显示的文字**：`{name}` 是事件原文（先按"剔除的文字"删除）；留空沿用日期格自己的文字
  （系统那一行，或越南农历日期）。
- 颜色和文字都留空、或要找的词留空，这一项不生效；第一条命中的生效。

几个例子（每行都是列表里的一项）：

| 要找的词 | 颜色 | 显示的文字 | 效果 |
| --- | --- | --- | --- |
| `班` | `#FF5A5A` | `补班` | 调休上班的日子显示"补班"、红色（默认） |
| `休` | `#4EA1FF` | `{name}` | 放假的日子显示节日名、蓝色（默认） |
| `*` | `#4EA1FF` | `{name}` | 订阅源里的所有事件都显示原文、蓝色 |
| `祝日` | `#E81123` | （留空） | 日文订阅源里"祝日"的日子只改颜色，沿用日期格文字 |
| `nghỉ lễ` | `#FF5A5A` | `{name}` | 越南订阅源里"nghỉ lễ"的日子显示原文、红色 |

### 用规则标农历日子

"要找的词"里也可以写一个日子，这样不依赖订阅源也能标记某一天：

| 写法 | 意思 |
| --- | --- |
| `{lunar: d=15}` | 每个农历月的十五（含闰月） |
| `{lunar: m=8, d=15}` | 农历八月十五（不算闰八月十五） |
| `{lunar: m=8L, d=15}` | 闰八月十五；也可写 `{lunar: m=8, d=15, leap}` |
| `{lunar: m=9}` | 农历九月的每一天 |
| `{solar: m=4, d=30}` | 阳历 4 月 30 日 |

- `m` 月份、`d` 日子，用逗号分隔；顺序、大小写和空格都不影响。
- `m=8L`，或者加一个 `leap` 字段，表示闰月；写了月份但没加，表示那个号码的本气月。只写
  `d` 不写月份时，闰月也算。
- `solar` 没有闰月，不能写 `L` 或 `leap`。
- 阳历日子可以到 31，农历日子最多 30；写了月份但该月没有这一天（例如 2 月 30 日）也算写法有误。
- 同一个月份的本气月和闰月都想标，就写两项：`{lunar: m=8, d=15}` 和
  `{lunar: m=8L, d=15}`。
- 写法有误（未知键、月份或日子越界、重复字段、没有日期）这一项不生效。
- 农历日期按哪本历算**看上面那个开关**：开着按越南农历，关着按系统显示的中国农历。
- 同一天有被订阅源规则命中的事件时，订阅源优先；订阅源里没有命中的日子才由这类规则标记。
- 这类规则没有事件原文，所以 `{name}` 是空的；文字留空就保留原有日期。一项只写一个日期条件。

越南节假日的一组例子（每行都是列表里的一项）：

| 要找的词 | 颜色 | 显示的文字 | 效果 |
| --- | --- | --- | --- |
| `{lunar: d=1}` | `#FF5A5A` | （留空） | 每个农历初一，保留农历日期，红色 |
| `{lunar: d=15}` | `#FF5A5A` | （留空） | 每个农历十五（Rằm），保留农历日期，红色 |
| `{lunar: m=1, d=1}` | `#E81123` | `Tết Nguyên đán` | 越南春节 |
| `{lunar: m=3, d=10}` | `#E81123` | `Giỗ tổ Hùng Vương` | 雄王纪念日 |
| `{lunar: m=5, d=5}` | `#E81123` | `Đoan Ngọ` | 端午节 |
| `{lunar: m=8, d=15}` | `#E81123` | `Tết Trung thu` | 中秋节 |
| `{lunar: m=12, d=23}` | `#E81123` | `Ông Táo` | 灶君 |
| `{solar: m=4, d=30}` | `#E81123` | `Ngày Thống nhất` | 越南统一日 |
| `{solar: m=9, d=2}` | `#E81123` | `Quốc khánh` | 越南国庆 |

## 剔除的文字

事件原文里不想要的那一段，原样写一条，它就从显示的文字里消失（出现几次删几次，写什么删什么，
括号和空格都算）。它只影响显示，不影响规则匹配。默认两条是中文订阅源的标记：

```
（休）       国庆节（休）                    显示成  国庆节
```

`（班）` 也一样；如果规则显示固定文字，就显示那段固定文字。其他订阅源可以自己加标记，例如
`(nghỉ bù)`。默认只列了全角括号；半角 `(休)`、`(班)` 或其他写法自己加一条。清空列表就什么都不删。
如果整个名字都被删空，这一天只保留颜色或原有日期。

## 数据来源

mod 不内置节假日数据，只会请求你填的地址。下面只是示例：

- 苹果 iCloud 中国节假日：`https://p10-calendars.icloud.com/holiday/CN_zh.ics`
- 苹果 iCloud 越南节假日：`https://p10-calendars.icloud.com/holiday/VN_vi.ics`
- `https://holiday.ailcc.com/api/holiday/ics`（中国，一次返回多年）

订阅源里每个事件大致长这样，**"要找的词"就是在这里面找**：

```
SUMMARY:春节（休）
```

常见按日期的 `.ics` 订阅都能用。暂时读不到新数据时，已加载的标记会保留；没有数据时日历仍显示
系统内容（或越南农历）和日期规则。

## 致谢

日历窗口的定位方式来自 lonfro 的
[Agenda in Calendar View](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/agenda-in-calendar-view.wh.cpp)
（GPL-3.0），跨线程调用沿用 m417z 的
[Windows 11 Notification Center Styler](https://github.com/m417z/my-windhawk-mods)。

越南农历计算由 [@phatMT97](https://github.com/phatMT97) 与
[@vantruong-dev](https://github.com/vantruong-dev) 贡献，基于 Jean Meeus《Astronomical Algorithms》（第 2 版 1998，第 25 与 49 章）与 Claus Tøndering Calendar FAQ 的天文学算法实现。

---

# Lịch âm và ngày lễ

Bảng lịch Windows 11 (bấm vào đồng hồ ở khay hệ thống) hiện ngày âm, tiết khí hoặc tên ngày lễ trong ô
ngày. Mod này ghi chữ và màu ngày lễ vào dòng đó theo quy tắc bạn viết; quy tắc cũng có thể tô trực tiếp
một ngày âm hoặc dương. Nguồn của bất kỳ nước nào đều dùng được.

- **Mặc định**: ngày có `休` hiện tên ngày lễ (xanh); ngày có `班` hiện `补班` (đỏ).
- Các ngày khác giữ dòng của hệ thống; bật lịch Việt Nam thì hiện ngày âm lịch Việt Nam.

## Yêu cầu

Mod viết vào **dòng chữ thứ hai** của ô ngày (thường là dòng hiện ngày âm, tiết khí hoặc tên ngày lễ),
nên phải bật dòng đó trước:

Cài đặt → Thời gian & ngôn ngữ → Ngày & giờ → "Hiển thị lịch bổ sung trên khay hệ thống" → chọn
"Trung (Giản thể, Âm lịch)"

Không bật dòng đó thì ô ngày thường không còn chỗ nào để viết, và trên lịch sẽ không thấy dấu nào. Lịch
âm Việt Nam bên dưới cũng viết vào chính dòng đó.
Khi ô đã có nội dung nhưng không có khối chữ thứ hai dùng được do hệ thống đặt tên (không có hoặc
đang trống), mod bỏ qua ô đó thay vì đoán khối khác và ghi vào log.

## Cài đặt

| Cài đặt | Điền gì |
| --- | --- |
| Nguồn dữ liệu (ICS) | Địa chỉ `.ics` của bạn. **Để trống (mặc định) thì không đọc nguồn nào**; có thể để trống nếu chỉ tô ngày bằng quy tắc. |
| Khoảng thời gian làm mới (giờ) | Bao lâu tải lại nguồn một lần (mặc định 24 giờ). |
| Lịch âm Việt Nam (thay chữ âm lịch) | Ô ngày mặc định hiện ngày âm Việt Nam; dòng ngày âm ở đầu bảng lịch cũng được viết theo kiểu Việt Nam. Xem bên dưới. |
| Hiện đầy đủ khi rê chuột | Hiện tên đầy đủ (hoặc chữ quy tắc ghi) khi đưa chuột lên ngày được tô. Dùng khi tên quá dài, ô không hiện hết. |
| Quy tắc tô màu | Ngày nào được tô, màu gì và chữ gì. Xem bên dưới. |
| Chữ cần bỏ | Những đoạn chữ bị bỏ khỏi nguyên văn sự kiện (mặc định `（休）`, `（班）`). Xem bên dưới. |

Đổi quy tắc hay "chữ cần bỏ" không buộc tải lại ngay: mod tô lại dữ liệu đã đọc theo cài đặt mới.

## Lịch âm Việt Nam

Khi bật "Lịch âm Việt Nam (thay chữ âm lịch)":

- ô ngày mặc định hiện ngày âm Việt Nam (`15/8`; tháng nhuận `1/2N`), trừ khi quy tắc cho chữ khác;
- nếu dòng ngày âm ở đầu bảng lịch là kiểu Trung Quốc, dòng đó được viết theo kiểu Việt Nam
  (ngày, tháng, năm Can Chi, ví dụ `15 Tháng 8 Năm Bính Ngọ`);
- ngày được tính offline (UTC+7), không cần nguồn.

Ngày lễ Việt Nam không được nhúng sẵn trong mod: lấy từ nguồn dữ liệu, hoặc từ quy tắc bên dưới.

## Quy tắc tô màu

Mỗi mục là một loại ngày, gồm ba ô:

    Từ: 休        Màu: #4EA1FF        Chữ: {name}

- **Từ**: một từ; sự kiện có chứa từ đó thì được tô. Quy tắc `*` nhận mọi sự kiện không
  quy tắc nào khác khớp, đặt ở đâu trong danh sách cũng được.
- **Màu**: `#RRGGBB`; để trống là không đặt màu.
- **Chữ**: `{name}` là nguyên văn sự kiện (sau "Chữ cần bỏ"); để trống là dùng dòng của ô
  (dòng hệ thống, hoặc ngày âm Việt Nam).
- Mục không có từ, hoặc không có cả màu lẫn chữ, không có tác dụng. Mục đầu tiên khớp được dùng.

Vài ví dụ (mỗi dòng là một mục trong danh sách):

| Từ | Màu | Chữ | Tác dụng |
| --- | --- | --- | --- |
| `班` | `#FF5A5A` | `补班` | ngày làm bù hiện 补班, màu đỏ (mặc định) |
| `休` | `#4EA1FF` | `{name}` | ngày nghỉ hiện tên ngày lễ, màu xanh (mặc định) |
| `*` | `#4EA1FF` | `{name}` | mọi sự kiện của nguồn đều hiện nguyên văn, màu xanh |
| `祝日` | `#E81123` | (trống) | ngày 祝日 của nguồn Nhật chỉ đổi màu, giữ dòng hiện có |
| `nghỉ lễ` | `#FF5A5A` | `{name}` | ngày có "nghỉ lễ" của nguồn Việt hiện nguyên văn, màu đỏ |

### Tô một ngày âm lịch bằng quy tắc

Ô "Từ" cũng có thể viết một ngày thay cho một từ, nhờ đó tô được ngày mà không nguồn nào có:

| Viết gì | Nghĩa |
| --- | --- |
| `{lunar: d=15}` | ngày rằm (15) của mọi tháng âm, kể cả tháng nhuận |
| `{lunar: m=8, d=15}` | 15 tháng 8 âm (tháng thường, không phải tháng nhuận) |
| `{lunar: m=8L, d=15}` | 15 tháng 8 nhuận; `{lunar: m=8, d=15, leap}` cũng là nghĩa đó |
| `{lunar: m=9}` | mọi ngày của tháng 9 âm |
| `{solar: m=4, d=30}` | ngày 30 tháng 4 dương lịch |

- `m` là tháng, `d` là ngày, cách nhau bằng dấu phẩy; thứ tự, chữ hoa thường và dấu cách đều được.
- `m=8L`, hoặc thêm ô `leap`, để chỉ lấy tháng nhuận; có tháng mà không thêm gì thì là tháng
  thường. Chỉ ghi `d` mà không ghi tháng thì tháng nhuận cũng được tính.
- `solar` không có tháng nhuận, nên không dùng `L` hay `leap`.
- Ngày dương có thể đến 31, ngày âm tối đa 30; ngày mà tháng đã ghi không có (ví dụ 30 tháng 2)
  cũng là viết sai.
- Muốn tô cả tháng thường và tháng nhuận của cùng một số thì viết hai mục:
  `{lunar: m=8, d=15}` và `{lunar: m=8L, d=15}`.
- Viết sai (khóa lạ, giá trị quá giới hạn, lặp ô, không có ngày) thì mục đó không tô ngày nào.
- Ngày âm được đọc theo lịch mà công tắc ở trên nói: bật thì theo lịch Việt Nam, tắt thì theo lịch Trung
  Quốc mà các ô đang hiện.
- Ngày có sự kiện được quy tắc của nguồn khớp thì do quy tắc đó quyết định; quy tắc kiểu này chỉ
  dùng cho những ngày không quy tắc nguồn nào tô.
- Quy tắc kiểu này không có nguyên văn sự kiện nên `{name}` rỗng; để trống Chữ là giữ ngày.
  Mỗi mục một mẫu ngày.

Một bộ ngày lễ Việt Nam (mỗi dòng là một mục trong danh sách):

| Từ | Màu | Chữ | Tác dụng |
| --- | --- | --- | --- |
| `{lunar: d=1}` | `#FF5A5A` | (trống) | mùng 1 mọi tháng âm, màu đỏ, giữ ngày âm |
| `{lunar: d=15}` | `#FF5A5A` | (trống) | ngày rằm mọi tháng âm, màu đỏ, giữ ngày âm |
| `{lunar: m=1, d=1}` | `#E81123` | `Tết Nguyên đán` | Tết Nguyên đán |
| `{lunar: m=3, d=10}` | `#E81123` | `Giỗ tổ Hùng Vương` | Giỗ tổ Hùng Vương |
| `{lunar: m=5, d=5}` | `#E81123` | `Đoan Ngọ` | Tết Đoan Ngọ |
| `{lunar: m=8, d=15}` | `#E81123` | `Tết Trung thu` | Tết Trung thu |
| `{lunar: m=12, d=23}` | `#E81123` | `Ông Táo` | Ông Táo về trời |
| `{solar: m=4, d=30}` | `#E81123` | `Ngày Thống nhất` | Ngày Thống nhất đất nước |
| `{solar: m=9, d=2}` | `#E81123` | `Quốc khánh` | Quốc khánh |

## Chữ cần bỏ

Chuỗi viết ở đây bị xóa khỏi chữ hiển thị đúng như đã viết, mọi lần xuất hiện (kể cả ngoặc và dấu cách).
Nó chỉ đổi phần hiển thị, không đổi việc quy tắc nào khớp. Mặc định là dấu của nguồn Trung Quốc:

```
（休）        国庆节（休）                     hiện  国庆节
```

`（班）` cũng vậy; nếu quy tắc cho chữ cố định thì chữ đó được hiện. Có thể thêm dấu của nguồn
khác, ví dụ `(nghỉ bù)`. Mặc định chỉ có dạng ngoặc full-width; hãy tự thêm `(休)`, `(班)` hoặc
cách viết khác. Xóa hết danh sách thì không bỏ gì.
Nếu xóa hết tên, chỉ còn màu hoặc ngày.

## Nguồn dữ liệu

Mod không kèm dữ liệu ngày lễ và chỉ đọc địa chỉ bạn nhập. Ví dụ:

- Ngày lễ Trung Quốc của Apple iCloud: `https://p10-calendars.icloud.com/holiday/CN_zh.ics`
- Ngày lễ Việt Nam của Apple iCloud: `https://p10-calendars.icloud.com/holiday/VN_vi.ics`
- `https://holiday.ailcc.com/api/holiday/ics` (Trung Quốc, nhiều năm một lần)

Mỗi sự kiện trong nguồn trông như thế này, và **"Từ" của quy tắc được tìm trong đó**:

```
SUMMARY:春节（休）
```

Các link `.ics` thường dùng theo ngày đều được. Nếu đọc lại thất bại, các dấu đã tải vẫn giữ; khi chưa
có dữ liệu, lịch vẫn hiện nội dung hệ thống (hoặc lịch Việt Nam) và quy tắc ngày.

## Ghi công

Cách tìm cửa sổ lịch dựa theo mod
[Agenda in Calendar View](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/agenda-in-calendar-view.wh.cpp)
của lonfro (GPL-3.0); các helper gọi xuyên luồng dựa theo
[Windows 11 Notification Center Styler](https://github.com/m417z/my-windhawk-mods) của m417z.

Lịch âm Việt Nam do [@phatMT97](https://github.com/phatMT97) và
[@vantruong-dev](https://github.com/vantruong-dev) đóng góp, sử dụng thuật toán thiên văn dựa trên
*Astronomical Algorithms* (Jean Meeus, xuất bản lần 2 năm 1998, chương 25 & 49) và Claus Tøndering Calendar FAQ.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- dataSourceUrl: ""
  $name: Data source (ICS, empty = no feed)
  $name:zh-CN: 数据来源（ICS 地址，留空则不读取订阅源）
  $name:vi-VN: Nguồn dữ liệu (ICS, để trống = không đọc nguồn)
  $description: >-
    An iCalendar (.ics) feed. Only events matched by the rules below are shown.
  $description:zh-CN: >-
    iCalendar（.ics）订阅地址。只有被下面规则匹配到的事件会显示。
  $description:vi-VN: >-
    Link iCalendar (.ics). Chỉ các sự kiện khớp quy tắc bên dưới được hiện.
- refreshIntervalHours: 24
  $name: Refresh interval (hours)
  $name:zh-CN: 刷新间隔（小时）
  $name:vi-VN: Khoảng thời gian làm mới (giờ)
  $description: >-
    How long loaded data is reused before the feed is read again (default 24).
  $description:zh-CN: 已加载的数据用多久后重新读取订阅源（默认 24 小时）。
  $description:vi-VN: Dữ liệu đã đọc được dùng bao lâu trước khi đọc lại nguồn (mặc định 24 giờ).
- vietnameseLunar: false
  $name: Vietnamese lunar calendar (replace the lunar text)
  $name:zh-CN: 越南农历（替换农历文字）
  $name:vi-VN: Lịch âm Việt Nam (thay chữ âm lịch)
  $description: >-
    On: day cells show the Vietnamese lunar date (14/9; 14/9N in a leap month) unless a
    rule gives another text. A Chinese lunar date at the top of the calendar is written in
    Vietnamese too. Calculated offline.
    Off (default): no Vietnamese date is written.
  $description:zh-CN: >-
    打开：日期格默认显示越南农历日期（14/9，闰月 14/9N），有规则文字时显示规则文字；
    日历顶部的农历日期是中文格式时，也整行改成越南写法。日期离线计算。
    关闭（默认）：不写越南日期。
  $description:vi-VN: >-
    Bật: ô ngày mặc định hiện ngày âm Việt Nam (14/9; tháng nhuận 14/9N), trừ khi quy tắc
    cho chữ khác; dòng ngày âm kiểu Trung Quốc ở đầu bảng lịch cũng được viết theo kiểu
    Việt Nam. Tính offline.
    Tắt (mặc định): không ghi ngày âm Việt Nam.
- showHolidayTooltip: true
  $name: Tooltip on hover
  $name:zh-CN: 悬停时显示完整文字
  $name:vi-VN: Hiện đầy đủ khi rê chuột
  $description: >-
    Resting the pointer on a marked day shows its full name, or the text the rule writes.
    Useful when a long name does not fit in the cell.
  $description:zh-CN: >-
    鼠标停在有标记的日期上时，显示完整名称（或规则写的文字）。名字太长、日期格显示不下时用。
  $description:vi-VN: >-
    Đưa chuột lên ngày được tô để xem tên đầy đủ, hoặc chữ mà quy tắc ghi. Dùng khi tên
    quá dài, ô không hiện hết.
- highlightRules:
  - - word: "班"
      $name: Word to look for
      $name:zh-CN: 要找的词
      $name:vi-VN: Từ cần tìm
      $description: >-
        One word, matched anywhere in an event's text. "*" takes every event no other rule
        matched. A date may be written instead, e.g. "{lunar: d=15}"; see the README.
      $description:zh-CN: >-
        一个词，事件文字里包含它就算命中。填 "*" 兜底；也可以填日期，例如
        "{lunar: d=15}"，写法见 README。
      $description:vi-VN: >-
        Một từ, khớp ở bất kỳ đâu trong nội dung sự kiện. "*" nhận mọi sự kiện không quy tắc
        nào khớp. Có thể viết ngày thay cho từ, ví dụ "{lunar: d=15}"; xem README.
    - color: "#FF5A5A"
      $name: Colour
      $name:zh-CN: 颜色
      $name:vi-VN: Màu
      $description: >-
        Text colour (#RRGGBB). Empty sets no colour.
      $description:zh-CN: >-
        文字颜色（#RRGGBB）。留空不设置颜色。
      $description:vi-VN: >-
        Màu chữ (#RRGGBB). Để trống là không đặt màu.
      #! $format: colorRgb
    - text: "补班"
      $name: Text to show
      $name:zh-CN: 显示的文字
      $name:vi-VN: Chữ hiển thị
      $description: >-
        Cell text. "{name}" is the event text; empty uses the cell's own line (the system
        line, or the Vietnamese date) and only changes the colour.
      $description:zh-CN: >-
        日期格显示的文字。"{name}" 是事件原文；留空沿用日期格自己的文字（系统那一行，或
        越南农历日期），只改颜色。
      $description:vi-VN: >-
        Chữ trong ô ngày. "{name}" là nguyên văn sự kiện; để trống là dùng dòng của ô (dòng
        hệ thống, hoặc ngày âm Việt Nam) và chỉ đổi màu.
  - - word: "休"
      $name: Word to look for
      $name:zh-CN: 要找的词
      $name:vi-VN: Từ cần tìm
      $description: >-
        One word, matched anywhere in an event's text. "*" takes every event no other rule
        matched. A date may be written instead, e.g. "{lunar: d=15}"; see the README.
      $description:zh-CN: >-
        一个词，事件文字里包含它就算命中。填 "*" 兜底；也可以填日期，例如
        "{lunar: d=15}"，写法见 README。
      $description:vi-VN: >-
        Một từ, khớp ở bất kỳ đâu trong nội dung sự kiện. "*" nhận mọi sự kiện không quy tắc
        nào khớp. Có thể viết ngày thay cho từ, ví dụ "{lunar: d=15}"; xem README.
    - color: "#4EA1FF"
      $name: Colour
      $name:zh-CN: 颜色
      $name:vi-VN: Màu
      $description: >-
        Text colour (#RRGGBB). Empty sets no colour.
      $description:zh-CN: >-
        文字颜色（#RRGGBB）。留空不设置颜色。
      $description:vi-VN: >-
        Màu chữ (#RRGGBB). Để trống là không đặt màu.
      #! $format: colorRgb
    - text: "{name}"
      $name: Text to show
      $name:zh-CN: 显示的文字
      $name:vi-VN: Chữ hiển thị
      $description: >-
        Cell text. "{name}" is the event text; empty uses the cell's own line (the system
        line, or the Vietnamese date) and only changes the colour.
      $description:zh-CN: >-
        日期格显示的文字。"{name}" 是事件原文；留空沿用日期格自己的文字（系统那一行，或
        越南农历日期），只改颜色。
      $description:vi-VN: >-
        Chữ trong ô ngày. "{name}" là nguyên văn sự kiện; để trống là dùng dòng của ô (dòng
        hệ thống, hoặc ngày âm Việt Nam) và chỉ đổi màu.
  $name: Highlight rules
  $name:zh-CN: 高亮规则
  $name:vi-VN: Quy tắc tô màu
  $description: >-
    One item per kind of day. A rule with no word, or with neither colour nor text, does
    nothing. The first match wins; a "*" rule takes every event no other rule matched,
    wherever it appears.
  $description:zh-CN: >-
    一项一类日子。没有词，或颜色和文字都留空的项不生效。从上往下第一条命中；"*" 只接住
    没有被其他规则命中过的事件，写在哪一项都可以。
  $description:vi-VN: >-
    Mỗi mục một loại ngày. Mục không có từ, hoặc không có cả màu lẫn chữ, không có tác dụng.
    Mục đầu tiên khớp được dùng; quy tắc "*" nhận mọi sự kiện không quy tắc nào khác khớp,
    đặt ở đâu cũng được.
- removeTexts: ["（休）", "（班）"]
  $name: Text to remove from the name
  $name:zh-CN: 剔除的文字
  $name:vi-VN: Chữ cần bỏ khỏi tên
  $description: >-
    Strings removed from the event text exactly as written, every occurrence. The defaults
    are the Chinese markers; clear the list to remove nothing.
  $description:zh-CN: >-
    从事件原文里原样删除的文字，出现几次删几次。默认是中文订阅源的标记；清空列表则不删除。
  $description:vi-VN: >-
    Chuỗi bị xóa khỏi nguyên văn đúng như đã viết, mọi lần xuất hiện. Mặc định là dấu của
    nguồn Trung Quốc; xóa hết danh sách thì không xóa gì.
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
// The lunar calendar calculation is an independent implementation written for this mod under
// GPL-3.0, based on public astronomical algorithms from Jean Meeus's "Astronomical Algorithms"
// (2nd Edition 1998, chapters 25 & 49) and Claus Tøndering's Calendar FAQ:
//
//   Copyright (C) 2026 phatMT97, vantruong-dev
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
#include <cmath>
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
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxd = winrt::Windows::UI::Xaml::Data;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

////////////////////////////////////////////////////////////////////////////////
// Settings

// A day of a lunar month, as the calendar has it: the day of it, the month, and which of
// the two months of a number a year may have - 0 the ordinary one and 1 the leap one.
struct LunarDay {
    int day = 0;
    int month = 0;
    int leap = 0;
};

// A day of the calendar a rule may ask for instead of a word of an event. The word of such
// a rule is written by hand, as one of these:
//
//     {lunar: d=15}                 every fifteenth day of a lunar month
//     {lunar: m=2, d=15}            the fifteenth of the second month
//     {lunar: m=8L, d=15}           the fifteenth of the leap eighth month
//     {lunar: m=9}                  every day of the ninth month
//     {solar: m=4, d=30}            the thirtieth of April
//
// "m" and "d" are the month and the day. "L" after the number of a month, or a bare
// "leap" field, asks for the leap month.
//
// A month written by hand is the ordinary month of that number: a year which has its eighth
// month twice has a leap one as well, and the day a festival falls on is the one which is
// not the leap one - Mid-Autumn is the fifteenth of the eighth month, not of the leap eighth
// month. A month left out takes both, which is what makes "{lunar: d=1}" the first day of
// every month, leap months included: a leap month has a first day too. The leap month alone
// is asked for with "m=8L" or "leap"; the ordinary and the leap month together take two
// rules.
//
// What a day of the lunar month is in the first place is decided by the switch above the
// list: with the Vietnamese lunar calendar on, the dates are the Vietnamese ones the cells
// show; with it off they are read as the calendar of the system itself, the Chinese one,
// which is the calendar the cell carries.
struct DateSpec {
    // Whether the day is a day of the lunar month or of the solar calendar. A solar day has
    // no leap month, and a token which names one is rejected.
    bool lunar = true;
    // Which day of the month and which month, 0 standing for "any": a token which names no
    // day takes every day of the month it names, and one which names neither is rejected.
    int day = 0;
    int month = 0;
    // -1 no restriction, 0 the ordinary month, 1 the leap one.
    int leap = -1;
};

// One rule of the "highlight rules" setting: the word which marks an event, the colour
// the day is drawn in, and the text the cell shows. The settings UI has one item of the
// list per rule, with the three fields written below:
//
//     Word: 休        Colour: #4EA1FF        Text: {name}
//
// A field which is left out asks for that part of the day to be left alone: a rule with
// no colour keeps the colour the shell gave the cell, and a rule with no text keeps the
// line the cell shows - so a rule can change the colour of a day, the text of a day, or
// both. A rule with neither does nothing. What the fields are is in the
// WindhawkModSettings block above, which the two have to be changed together.
struct HighlightRule {
    // The one word this rule looks for. "*" is the catch-all, which takes every event no
    // other rule matched, and an empty word is a rule which marks nothing - the state of
    // an item which was just added.
    std::wstring word;
    std::optional<winrt::Windows::UI::Color> color;
    std::wstring text;
};

// A rule whose word names a day of the calendar rather than a word of an event: the rule
// itself - its colour and its text are what the day is drawn with - and the day it asks for,
// read out of the word once, when the settings are read, rather than on every pass.
struct DayRule {
    HighlightRule rule;
    DateSpec day;
};

// The snapshot of the settings. The scalar defaults are written twice on purpose: here,
// for a setting which cannot be read, and in the WindhawkModSettings YAML, which is what
// the settings UI starts from, so the two have to be changed together. The rule and
// removal lists have no defaults here: the YAML is where their default items come from.
// LoadSettings publishes a whole new snapshot as a shared_ptr to an immutable object, and
// every caller takes that pointer rather than a copy of the struct: a cell is looked at
// on every pass of the sweep, and copying a rule list for each of them is work with no
// purpose behind it.
struct Settings {
    // The Vietnamese lunar calendar: a cell shows the lunar date of that calendar unless a
    // rule gives it another text, and the header the Vietnamese date and Can Chi year, both
    // calculated offline. Off by default, which leaves the calendar as the system drew it.
    // It has nothing to do with the feed, and the feed works the same either way.
    bool vietnameseLunar = false;
    // Empty by default on purpose: the mod then contacts nothing at all. The date rules and
    // the Vietnamese switch still work. The address is the user's to fill in.
    std::wstring dataSourceUrl;
    int refreshIntervalHours = 24;
    // Whether a marked day shows its full name when the pointer rests on it. On by
    // default: a name can be longer than a day cell, and the tooltip has room for all of it.
    bool showHolidayTooltip = true;
    // The rule list as the setting is parsed into it; an empty list marks nothing.
    std::vector<HighlightRule> rules;
    // The rules of that list which name a day of the calendar, and whether any of them asks
    // for a day of the lunar month - which is one of the two reasons the lunar date of a day
    // is worked out at all (the other is the switch above).
    std::vector<DayRule> dayRules;
    bool needsLunarDate = false;
    // What the user asked to be taken out of the name of a marked day, one entry of the
    // list per string. Empty takes nothing out, which is the state of a feed whose names
    // are already what the user wants to read.
    std::vector<std::wstring> removeTexts;
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

// Defined with the iCalendar reading below. The settings are put in the same Unicode form
// as the feed there, so a word typed with a combining mark still matches the feed's
// precomposed text.
std::wstring NormalizeNfc(std::wstring const& text);

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
// per rule, each with three fields - the word, the colour and the text:
//
//     Word:   休
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

// The rule list of the settings. A rule whose colour cannot be read is dropped rather
// than guessed at, and the log names it: a list which marks days differently from what
// it says is the kind of thing nobody finds by looking. An item which does not describe
// anything yet - no word, or neither a colour nor a text - is skipped, which is also
// the state of one the user is in the middle of filling in.
std::vector<HighlightRule> LoadHighlightRules() {
    std::vector<HighlightRule> rules;
    std::wstring firstNoColour;
    size_t droppedColour = 0;
    size_t droppedWords = 0;

    for (int i = 0; i < kMaxHighlightRules; i++) {
        const std::wstring word = NormalizeNfc(
            TrimWhitespace(ReadStringSetting(L"highlightRules[%d].word", i)));
        const std::wstring colorText =
            TrimWhitespace(ReadStringSetting(L"highlightRules[%d].color", i));
        const std::wstring text = NormalizeNfc(
            TrimWhitespace(ReadStringSetting(L"highlightRules[%d].text", i)));

        // An item which is not written at all is skipped: that is the state of an item
        // the user has just added, and of every index past the end of the list.
        if (word.empty() && colorText.empty() && text.empty()) {
            continue;
        }

        // A rule without a word marks nothing - "*" is what a rule which takes the
        // events no other rule matched is written with - so it is left out as well.
        if (word.empty()) {
            droppedWords++;
            continue;
        }

        HighlightRule rule;
        rule.word = word;
        rule.text = text;

        // No colour is not a mistake: it asks for the colour the shell gave the cell to
        // be left as it is, which is what a rule which only writes a text wants.
        if (!colorText.empty()) {
            rule.color = ParseHexColor(colorText);
        }
        if (!colorText.empty() && !rule.color) {
            droppedColour++;
            if (firstNoColour.empty()) {
                firstNoColour = word;
            }
            continue;
        }

        // Nothing would change on the day this rule matches: the item is half written,
        // or it is a rule which is only there for the word it looks for, which is
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
        ReadStringSetting(L"highlightRules[%d].word", kMaxHighlightRules));
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
        Wh_Log(L"%zu highlight rule(s) have no word and are ignored - the rule which "
               L"takes what the others did not is written \"*\"",
               droppedWords);
    }

    return rules;
}

// How many entries of the "text to remove" list are read. A list setting has no length to
// ask for, so the walk has to stop somewhere and this is where: nothing needs this many
// strings to be taken out, and a list which is longer is cut off with a log line.
constexpr int kMaxRemoveTexts = 16;

// The text the user asked to be taken out of the name of a marked day: one string per
// entry of the list, one item of the settings UI per string. The entries are read exactly
// as they are written, which is how they are used as well (see RemoveAll): no trimming,
// no separators to escape, so a string which has to lose a comma or a bracket is written
// with it. An item which is empty is skipped - that is the state of an item just added
// and of the empty item the list starts from - and the walk goes on rather than stopping
// there, so an item left empty in the middle does not hide the ones below it.
std::vector<std::wstring> LoadRemoveTexts() {
    std::vector<std::wstring> texts;
    for (int i = 0; i < kMaxRemoveTexts; i++) {
        const std::wstring text = NormalizeNfc(ReadStringSetting(L"removeTexts[%d]", i));
        if (text.empty()) {
            continue;
        }
        texts.push_back(text);
    }

    const std::wstring beyond =
        ReadStringSetting(L"removeTexts[%d]", kMaxRemoveTexts);
    if (!beyond.empty()) {
        Wh_Log(L"More than %d texts to remove are set; the rest is ignored",
               kMaxRemoveTexts);
    }

    return texts;
}

// Whether two rule lists are the same. This is what tells a settings change which
// changes the marking from one which only changes something else (the refresh interval,
// say): the data which is already loaded has to be marked again in the first case, and
// only drawn again in the second. The text to remove belongs to the marking as well -
// it is taken out of the name of a marked day - and is compared next to this, where the
// settings change is looked at (see Wh_ModSettingsChanged).
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
        if (left[i].word != right[i].word || left[i].text != right[i].text ||
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

// Whether a word is written as a day of the calendar: "{lunar: ...}" or "{solar: ...}".
// Nothing else in the settings is written in braces, so the shape is enough to tell the two
// kinds of word apart; whether the token itself makes sense is decided below.
bool IsDateTokenShape(std::wstring const& word) {
    return word.size() >= 2 && word.front() == L'{' && word.back() == L'}';
}

// Whether a text is one of these two words, ignoring case: the parts of a token are written
// by hand, and "M=8" and "m=8" ask for the same month.
bool SameTokenWord(std::wstring const& text, PCWSTR word) {
    const int length = static_cast<int>(wcslen(word));
    return static_cast<int>(text.size()) == length &&
           CompareStringOrdinal(text.c_str(), length, word, length, TRUE) == CSTR_EQUAL;
}

// Defined with the date helpers below; a solar token whose month is written may name no
// day that month has, and the answer is needed here already.
int DaysInMonth(int year, int month);

// The day a word asks for, or nothing when the word is not written as one. A token is
// written by hand, so a typo is worth rejecting rather than reading as a day nobody asked
// for: the keys are "d" and "m", the optional field is the bare word "leap", a month is
// 1 to 12, and a day is 1 to 30 in a lunar month or 1 to 31 in a solar one. A word which
// is not written as a token at all is the word of an event and is not read here.
bool ParseDateToken(std::wstring const& word, DateSpec& out) {
    // "{lunar:" ... "}": the shortest token there is is "{lunar:d=1}".
    if (!IsDateTokenShape(word) || word.size() < 10) {
        return false;
    }
    const size_t colon = word.find(L':');
    if (colon == std::wstring::npos || colon + 2 > word.size()) {
        return false;
    }

    DateSpec spec;
    const std::wstring kind = TrimWhitespace(word.substr(1, colon - 1));
    if (SameTokenWord(kind, L"lunar")) {
        spec.lunar = true;
    } else if (SameTokenWord(kind, L"solar")) {
        spec.lunar = false;
    } else {
        return false;
    }

    auto readNumber = [](std::wstring const& text, int smallest, int largest, int& value) {
        if (text.empty() || text.size() > 2) {
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
    };

    // The fields, separated by ",". A field which is empty - "d=15,," and a trailing "," -
    // is a typo like any other, and is not read as "the rest of the token says nothing".
    const std::wstring body = word.substr(colon + 1, word.size() - colon - 2);
    bool seenDay = false;
    bool seenMonth = false;
    bool seenLeap = false;
    for (size_t position = 0; position <= body.size();) {
        size_t end = position;
        while (end < body.size() && body[end] != L',') {
            end++;
        }
        const std::wstring field = TrimWhitespace(body.substr(position, end - position));
        position = end + 1;
        if (field.empty()) {
            return false;
        }

        const size_t equals = field.find(L'=');
        if (equals == std::wstring::npos) {
            if (!spec.lunar || seenLeap || !SameTokenWord(field, L"leap")) {
                return false;
            }
            spec.leap = 1;
            seenLeap = true;
            continue;
        }
        const std::wstring key = TrimWhitespace(field.substr(0, equals));
        const std::wstring value = TrimWhitespace(field.substr(equals + 1));

        if (SameTokenWord(key, L"d")) {
            const int largestDay = spec.lunar ? 30 : 31;
            if (seenDay || !readNumber(value, 1, largestDay, spec.day)) {
                return false;
            }
            seenDay = true;
        } else if (SameTokenWord(key, L"m")) {
            std::wstring number = value;
            bool writtenAsLeap = false;
            if (!number.empty() &&
                (number.back() == L'L' || number.back() == L'l')) {
                writtenAsLeap = true;
                number.pop_back();
            }
            if (seenMonth || !readNumber(number, 1, 12, spec.month)) {
                return false;
            }
            if (writtenAsLeap) {
                if (!spec.lunar || seenLeap) {
                    return false;
                }
                spec.leap = 1;
                seenLeap = true;
            }
            seenMonth = true;
        } else {
            return false;
        }
    }

    // A solar day is checked against the month when one is written, so that "m=2, d=30"
    // is a typo rather than a rule which can never match. February is checked as a leap
    // February, which is the only way "m=2, d=29" can be a day.
    if (!spec.lunar && seenMonth &&
        spec.day > DaysInMonth(2000, spec.month)) {
        return false;
    }

    // A token which names neither a day nor a month asks for nothing, and a day nobody asked
    // for is not marked.
    if (!seenDay && !seenMonth) {
        return false;
    }

    // A month written by hand is the ordinary month of that number, and the leap one has to be
    // asked for with "L" or "leap": the day a festival falls on is the one of the
    // month which is not the leap one (Mid-Autumn is the fifteenth of the eighth month, and a
    // year with a leap eighth month has a fifteenth day in that month as well). A token which
    // names no month keeps both, since a month left out is not a month which was left out of
    // the token - "d=1" is the first day of every month, and a leap month has one too.
    if (seenMonth && !seenLeap) {
        spec.leap = 0;
    }

    out = spec;
    return true;
}

// The rules of a list which name a day of the calendar, read out of it once, and whether any
// of them asks for a day of the lunar month. A word which is written as a token and cannot
// be read is dropped and named in the log: a rule which marks no day the reader can make
// sense of is the kind of thing nobody finds by looking.
void CollectDayRules(std::vector<HighlightRule> const& rules,
                     std::vector<DayRule>& dayRules,
                     bool& needsLunarDate) {
    std::wstring firstUnread;
    size_t dropped = 0;

    for (const auto& rule : rules) {
        if (!IsDateTokenShape(rule.word)) {
            continue;  // the word of an event
        }

        DateSpec spec;
        if (!ParseDateToken(rule.word, spec)) {
            if (firstUnread.empty()) {
                firstUnread = rule.word;
            }
            dropped++;
            continue;
        }

        DayRule entry;
        entry.rule = rule;
        entry.day = spec;
        dayRules.push_back(std::move(entry));
        if (spec.lunar) {
            needsLunarDate = true;
        }
    }

    if (dropped) {
        Wh_Log(L"%zu highlight rule(s) are dropped: the word is written as a day of the "
               L"calendar but is not one (the rule of \"%s\")",
               dropped, firstUnread.c_str());
    }
}

// The rule which marks this text. The rules are tried in the order they are written and
// the first one whose word appears in the text wins, so the specific rule goes above the
// general one. The catch-all ("*") is used only when no other rule matched, wherever it
// stands in the list: it is the "everything else" rule, and a list which writes it first
// means what a list which writes it last means.
const HighlightRule* MatchHighlightRule(std::wstring const& text,
                                        std::vector<HighlightRule> const& rules) {
    for (const auto& rule : rules) {
        // A word written as a day of the calendar is not a word of an event: it is matched
        // against the date of a day (see MatchDayRule) and never against this text.
        if (IsDateTokenShape(rule.word)) {
            continue;
        }
        if (rule.word != L"*" && FindWord(text, rule.word) != std::wstring::npos) {
            return &rule;
        }
    }

    for (const auto& rule : rules) {
        if (rule.word == L"*") {
            return &rule;
        }
    }
    return nullptr;
}

// Takes every appearance of "what" out of "text". What the settings say is what is taken
// out, character for character: an entry is not a pattern, a space or a bracket is a part
// of it like any other character, and the case has to be the one which is written. An
// entry which appears twice has both of them taken out, and "nothing at all" is not
// looked for - a search for it would answer with a place between every two characters.
void RemoveAll(std::wstring& text, std::wstring const& what) {
    if (what.empty()) {
        return;
    }

    size_t at = text.find(what);
    while (at != std::wstring::npos) {
        text.erase(at, what.size());
        at = text.find(what, at);
    }
}

void LoadSettings() {
    auto settings = std::make_shared<Settings>();

    settings->vietnameseLunar = Wh_GetIntSetting(L"vietnameseLunar") != 0;
    settings->dataSourceUrl = ReadStringSetting(L"dataSourceUrl");
    settings->refreshIntervalHours = Wh_GetIntSetting(L"refreshIntervalHours");
    settings->showHolidayTooltip = Wh_GetIntSetting(L"showHolidayTooltip") != 0;
    settings->rules = LoadHighlightRules();
    settings->removeTexts = LoadRemoveTexts();

    // Which rules name a day of the calendar, and whether the lunar date of a day is needed
    // at all: the rules of an event never ask for it, and the date of a day is only worked
    // out for the switch and for the rules which name one.
    CollectDayRules(settings->rules, settings->dayRules, settings->needsLunarDate);
    if (settings->vietnameseLunar) {
        settings->needsLunarDate = true;
    }

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
               L"settings of this mod (a word of an event, a colour and a text)");
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

// What the calendar shows for one date: the text of the event which covers it, with the
// strings the user asked to be taken out of it deleted, and the text and the colour of
// the rule which marked it. Which rule that is gets decided when the feed is read - see
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
// +day) inside an int32 as well, and turns away years before 1900 or after 9999.
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

// The time of day a value names, in seconds since midnight, and 0 for a date without a
// time (or a value whose time cannot be read). Midnight is 0 too, which is what an all-day
// date has: the two end a range the same way.
int TimeOfDaySeconds(std::wstring const& text) {
    const size_t t = text.find(L'T');
    if (t == std::wstring::npos) {
        return 0;
    }

    std::wstring digits;
    for (size_t i = t + 1; i < text.size() && digits.size() < 6; i++) {
        if (text[i] >= L'0' && text[i] <= L'9') {
            digits += text[i];
        }
    }

    // Hours and minutes are the shortest form a feed writes; anything shorter is not a
    // time this can tell apart from midnight.
    if (digits.size() < 4) {
        return 0;
    }

    const int hour = (digits[0] - L'0') * 10 + (digits[1] - L'0');
    const int minute = (digits[2] - L'0') * 10 + (digits[3] - L'0');
    const int second =
        digits.size() >= 6 ? (digits[4] - L'0') * 10 + (digits[5] - L'0') : 0;
    if (hour > 23 || minute > 59 || second > 59) {
        return 0;
    }
    return hour * 3600 + minute * 60 + second;
}

// True when a value names a time of day other than midnight. An end of "20260103T170000"
// covers the third of January until five in the afternoon, while one of "20260103T000000"
// stops where that day begins: the two do not end the same way, so the time is looked at
// even though the mod only marks whole days.
bool HasTimeOfDay(std::wstring const& text) {
    return TimeOfDaySeconds(text) != 0;
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
// Bumped on every settings change. A fetch or a re-mark which started under an older
// value is dropped instead of overwriting data which belongs to the settings now in
// force; the increment and the store both happen under g_holidaysMutex.
std::atomic<uint64_t> g_settingsGeneration{0};
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
// kept with it: see the comment above g_holidaysBody. False means the settings changed
// since the caller started, and its result belongs to settings which are no longer in
// force; nothing is stored then.
bool StoreHolidays(uint64_t generation,
                   std::wstring const& body,
                   HolidayMap parsed) {
    std::lock_guard<std::mutex> lock(g_holidaysMutex);
    if (g_settingsGeneration.load(std::memory_order_relaxed) != generation) {
        return false;
    }
    g_holidays = std::move(parsed);
    g_holidaysBody = body;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// The lunar calendar of a day
//
// Two things need the date of a day as a date rather than as the text the shell drew: the
// switch which shows the Vietnamese lunar calendar in the cells and the header, and a rule
// whose word names a day of a month (see DateSpec). The text the shell prints cannot be
// read back into one - it is "廿二", "立冬" or "中秋节" depending on the day, and a day
// whose text is a solar term or a festival is exactly the day such a rule is written for -
// so the date is calculated here, offline, and never fetched.
//
// The calculation is an independent astronomical implementation based on Jean Meeus's
// "Astronomical Algorithms" (2nd Edition 1998, chapters 25 & 49), contributed to this mod by @phatMT97
// and @vantruong-dev, who maintain the Vietnamese side of it. It takes the hour the
// calendar is read at as its argument: Vietnam is at the seventh hour east of Greenwich and
// China at the eighth, and that is the whole difference between the two calendars - a new
// moon close enough to midnight can bring them a day apart, which is why the same
// calculation answers for both rather than two of them.
//
// The dates of the Chinese calendar this answers with are the same as the ones Windows
// shows for the years this mod is used in, checked day by day against the official festival
// dates of 2024 to 2029. As with the Chinese calendar, a new moon occurring within seconds
// of midnight (such as 1967-07-08 00:00:03 UTC+7 in Vietnam, or in 2242) lies within the
// inherent truncation error of the Meeus series and can differ by one day from published
// historical tables. Outside these rare midnight boundaries, the calculation matches both
// official Chinese calendar tables and Vietnamese calendar software.

namespace VietnameseLunar {

constexpr double kPI = 3.14159265358979323846;
constexpr double kDegToRad = kPI / 180.0;

// Epoch J2000.0: Mean New Moon of 2000 January 6 14:20 TT (Jean Meeus, 1998, ch. 49).
constexpr double kJ2000NewMoonEpochJd = 2451550.09766;
constexpr double kSynodicMonth = 29.530588861;

// Julian Day Number from Gregorian date (Claus Tøndering's algorithm).
inline int JulianDayFromGregorian(int day, int month, int year) {
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    int jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
    if (jd < 2299161) {
        jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - 32083;
    }
    return jd;
}

// Apparent ecliptic longitude of the Sun in degrees [0, 360) (Jean Meeus, 1998, ch. 25).
// Includes equation of the center, aberration (-0.00569 deg) and nutation correction.
inline double SunEclipticLongitudeDeg(double jde) {
    double t = (jde - 2451545.0) / 36525.0;
    double t2 = t * t;
    double meanAnomaly = 357.52910 + 35999.05030 * t - 0.0001559 * t2 - 0.00000048 * t * t2;
    double meanLongitude = 280.46645 + 36000.76983 * t + 0.0003032 * t2;

    double correction = (1.914600 - 0.004817 * t - 0.000014 * t2) * std::sin(kDegToRad * meanAnomaly);
    correction += (0.019993 - 0.000101 * t) * std::sin(kDegToRad * 2.0 * meanAnomaly);
    correction += 0.000290 * std::sin(kDegToRad * 3.0 * meanAnomaly);

    double trueLongitude = meanLongitude + correction;
    double omega = 125.04 - 1934.136 * t;
    double apparentLongitude = trueLongitude - 0.00569 - 0.00478 * std::sin(kDegToRad * omega);
    apparentLongitude -= 360.0 * std::floor(apparentLongitude / 360.0);
    return apparentLongitude;
}

// Time difference Delta T (TT - UT) in seconds (Espenak & Meeus, 2006; NASA polynomial series).
inline double DeltaTSeconds(double year) {
    if (year < 500.0) {
        double u = (year - 1820.0) / 100.0;
        return -20.0 + 32.0 * u * u;
    }
    if (year < 1600.0) {
        double u = (year - 1000.0) / 100.0;
        return 1574.2 - 556.01 * u + 71.2347 * u * u + 0.319781 * std::pow(u, 3) -
               0.8503463 * std::pow(u, 4) - 0.005050998 * std::pow(u, 5) + 0.0083572073 * std::pow(u, 6);
    }
    if (year < 1700.0) {
        double t = year - 1600.0;
        return 120.0 - 0.9808 * t - 0.01532 * t * t + t * t * t / 7129.0;
    }
    if (year < 1800.0) {
        double t = year - 1700.0;
        return 8.83 + 0.1603 * t - 0.0059285 * t * t + 0.00013336 * t * t * t - t * t * t * t / 1174000.0;
    }
    if (year < 1860.0) {
        double t = year - 1800.0;
        return 13.72 - 0.332447 * t + 0.0068612 * t * t + 0.0041116 * t * t * t -
               0.00037436 * t * t * t * t + 0.0000121272 * std::pow(t, 5) -
               0.0000001699 * std::pow(t, 6) + 0.000000000875 * std::pow(t, 7);
    }
    if (year < 1900.0) {
        double t = year - 1860.0;
        return 7.62 + 0.5737 * t - 0.251754 * t * t + 0.01680668 * t * t * t -
               0.0004473624 * t * t * t * t + std::pow(t, 5) / 233174.0;
    }
    if (year < 1920.0) {
        double t = year - 1900.0;
        return -2.79 + 1.494119 * t - 0.0598939 * t * t + 0.0061966 * t * t * t - 0.000197 * t * t * t * t;
    }
    if (year < 1941.0) {
        double t = year - 1920.0;
        return 21.20 + 0.84493 * t - 0.076100 * t * t + 0.0020936 * t * t * t;
    }
    if (year < 1961.0) {
        double t = year - 1950.0;
        return 29.07 + 0.407 * t - t * t / 233.0 + t * t * t / 2547.0;
    }
    if (year < 1986.0) {
        double t = year - 1975.0;
        return 45.45 + 1.067 * t - t * t / 260.0 - t * t * t / 718.0;
    }
    if (year < 2005.0) {
        double t = year - 2000.0;
        return 63.86 + 0.3345 * t - 0.060374 * t * t + 0.0017275 * t * t * t +
               0.000651814 * t * t * t * t + 0.00002373599 * std::pow(t, 5);
    }
    if (year < 2050.0) {
        double t = year - 2000.0;
        return 62.92 + 0.32217 * t + 0.005589 * t * t;
    }
    if (year < 2150.0) {
        double u = (year - 1820.0) / 100.0;
        return -20.0 + 32.0 * u * u - 0.5628 * (2150.0 - year);
    }
    double u = (year - 1820.0) / 100.0;
    return -20.0 + 32.0 * u * u;
}

// Major solar term index (0-11, where 9 corresponds to Winter Solstice / Đông Chí)
// at the given local Julian Day and time zone.
inline int MajorSolarTermIndex(int jdn, double timeZone) {
    double jdUt = static_cast<double>(jdn) - 0.5 - timeZone / 24.0;
    double approxYear = 2000.0 + (jdUt - 2451545.0) / 365.25;
    double jde = jdUt + DeltaTSeconds(approxYear) / 86400.0;
    return static_cast<int>(std::floor(SunEclipticLongitudeDeg(jde) / 30.0)) % 12;
}

// True New Moon time in JD (UT) (Jean Meeus, Astronomical Algorithms 2nd ed. 1998, ch. 49).
// Includes Table 49.A periodic terms (with eccentricity factor E) and Table 49.B planetary perturbations.
inline double NewMoonJulianDay(int k) {
    double t = static_cast<double>(k) / 1236.85;
    double t2 = t * t;
    double t3 = t2 * t;
    double t4 = t2 * t2;

    double jde = kJ2000NewMoonEpochJd + kSynodicMonth * k +
                 0.00015437 * t2 - 0.000000150 * t3 + 0.00000000073 * t4;

    double E = 1.0 - 0.002516 * t - 0.0000074 * t2;
    double sunAnomaly = (2.5534 + 29.10535670 * k - 0.0000014 * t2 - 0.00000011 * t3) * kDegToRad;
    double moonAnomaly = (201.5643 + 385.81693528 * k + 0.0107582 * t2 + 0.00001238 * t3 - 0.000000058 * t4) * kDegToRad;
    double moonArgument = (160.7108 + 390.67050284 * k - 0.0016118 * t2 - 0.00000227 * t3 + 0.000000011 * t4) * kDegToRad;
    double omega = (124.7746 - 1.56375588 * k + 0.0020672 * t2 + 0.00000215 * t3) * kDegToRad;

    // Periodic terms from Table 49.A with exact signs and eccentricity factor E
    double correction =
        -0.40720 * std::sin(moonAnomaly) +
        0.17241 * E * std::sin(sunAnomaly) +
        0.01608 * std::sin(2.0 * moonAnomaly) +
        0.01039 * std::sin(2.0 * moonArgument) +
        0.00739 * E * std::sin(moonAnomaly - sunAnomaly) -
        0.00514 * E * std::sin(moonAnomaly + sunAnomaly) +
        0.00208 * E * E * std::sin(2.0 * sunAnomaly) -
        0.00111 * std::sin(moonAnomaly - 2.0 * moonArgument) -
        0.00057 * std::sin(moonAnomaly + 2.0 * moonArgument) +
        0.00056 * E * std::sin(2.0 * moonAnomaly + sunAnomaly) -
        0.00042 * std::sin(3.0 * moonAnomaly) +
        0.00042 * E * std::sin(sunAnomaly + 2.0 * moonArgument) +
        0.00038 * E * std::sin(sunAnomaly - 2.0 * moonArgument) -
        0.00024 * E * std::sin(2.0 * moonAnomaly - sunAnomaly) -
        0.00017 * std::sin(omega) -
        0.00007 * std::sin(moonAnomaly + 2.0 * sunAnomaly) +
        0.00004 * std::sin(2.0 * moonAnomaly - 2.0 * moonArgument) +
        0.00004 * std::sin(3.0 * sunAnomaly) +
        0.00003 * std::sin(moonAnomaly + sunAnomaly - 2.0 * moonArgument) +
        0.00003 * std::sin(2.0 * moonAnomaly + 2.0 * moonArgument) -
        0.00003 * std::sin(moonAnomaly + sunAnomaly + 2.0 * moonArgument) +
        0.00003 * std::sin(moonAnomaly - sunAnomaly + 2.0 * moonArgument) -
        0.00002 * std::sin(moonAnomaly - sunAnomaly - 2.0 * moonArgument) -
        0.00002 * std::sin(3.0 * moonAnomaly + sunAnomaly) +
        0.00002 * std::sin(4.0 * moonAnomaly);

    // Planetary perturbations from Table 49.B (remaining 13 terms)
    static const double A0[13] = {251.88, 251.83, 349.42, 84.66, 141.74, 207.14, 154.84, 34.52, 207.19, 291.34, 161.72, 239.56, 331.55};
    static const double A1[13] = {0.016321, 26.651886, 36.412478, 18.206239, 53.303771, 2.453732, 7.306860, 27.261239, 0.121824, 1.844379, 24.198154, 25.513099, 3.592518};
    static const double C[13]  = {165e-6, 164e-6, 126e-6, 110e-6, 62e-6, 60e-6, 56e-6, 47e-6, 42e-6, 40e-6, 37e-6, 35e-6, 23e-6};

    double planetary = 325e-6 * std::sin((299.77 + 0.107408 * k - 0.009173 * t2) * kDegToRad);
    for (int i = 0; i < 13; i++) {
        planetary += C[i] * std::sin((A0[i] + A1[i] * k) * kDegToRad);
    }

    double j = jde + correction + planetary;
    double approxYear = 2000.0 + (j - 2451545.0) / 365.25;
    return j - DeltaTSeconds(approxYear) / 86400.0;
}

// Calendar day (local JDN) on which the k-th New Moon falls.
inline int NewMoonDay(int k, double timeZone) {
    return static_cast<int>(std::floor(NewMoonJulianDay(k) + 0.5 + timeZone / 24.0));
}

// A specific lunation identified by its lunation number k (from J2000) and local start day.
struct Lunation {
    int index = 0;
    int day = 0;
};

// Finds the lunation on or immediately preceding local Julian day jd.
inline Lunation FindNewMoonBefore(int jd, double timeZone) {
    int k = static_cast<int>(std::floor((static_cast<double>(jd) - kJ2000NewMoonEpochJd) / kSynodicMonth));
    int day = NewMoonDay(k, timeZone);
    while (true) {
        int nextDay = NewMoonDay(k + 1, timeZone);
        if (nextDay <= jd) {
            k++;
            day = nextDay;
        } else {
            break;
        }
    }
    while (day > jd) {
        k--;
        day = NewMoonDay(k, timeZone);
    }
    return {k, day};
}

// Finds Month 11 (the month containing Major Solar Term 9 / Winter Solstice) for Gregorian year 'year'.
// A lunation contains Winter Solstice if the solar term index transitions from <= 8 to >= 9.
inline Lunation FindMonth11(int year, double timeZone) {
    int midDec = JulianDayFromGregorian(15, 12, year);
    Lunation lun = FindNewMoonBefore(midDec, timeZone);
    for (int k = lun.index - 1; k <= lun.index + 2; k++) {
        int t0 = MajorSolarTermIndex(NewMoonDay(k, timeZone), timeZone);
        int t1 = MajorSolarTermIndex(NewMoonDay(k + 1, timeZone), timeZone);
        if (t0 <= 8 && t1 >= 9) {
            return {k, NewMoonDay(k, timeZone)};
        }
    }
    return lun;
}

// Finds the 1-based offset (from Month 11) of the leap month in a 13-month lunar year (Sui).
// By the 24 solar terms rule (无中气置闰), the leap month is the first month that does not contain a Major Solar Term.
// Returns 0 if no month without a Major Solar Term is found.
inline int FindLeapMonthOffset(int k11, double timeZone) {
    int prevTerm = MajorSolarTermIndex(NewMoonDay(k11 + 1, timeZone), timeZone);
    for (int i = 1; i <= 12; i++) {
        int nextTerm = MajorSolarTermIndex(NewMoonDay(k11 + i + 1, timeZone), timeZone);
        if (nextTerm == prevTerm) {
            return i;
        }
        prevTerm = nextTerm;
    }
    return 0;
}

// Converts a solar (Gregorian) date to an East Asian lunisolar date (Vietnam UTC+7, China UTC+8).
// The lunisolar calendar is based on astronomical rules:
// - Days begin at midnight local standard time; Day 1 of each month is the day of the New Moon.
// - Month 11 is the month that contains the Winter Solstice (Đông Chí / 冬至).
// - A solar cycle (Sui / 岁) spans from Month 11 to the next Month 11. If it contains 13 months,
//   the first month without a Major Solar Term (Trung khí / 中气) is a leap month.
// - Civil lunar years (Năm / 年) begin at Month 1 (Tết / Lunar New Year).
inline void convertSolar2Lunar(int day, int month, int year, int& lunarDay, int& lunarMonth,
                               int& lunarYear, int& lunarLeap, double timeZone) {
    int jd = JulianDayFromGregorian(day, month, year);
    Lunation curLun = FindNewMoonBefore(jd, timeZone);
    lunarDay = jd - curLun.day + 1;

    // Anchor at Month 11 (the month containing the Winter Solstice) for the current Gregorian year.
    Lunation lun11ThisYear = FindMonth11(year, timeZone);

    int suiYear = 0;
    Lunation lun11Prev;
    Lunation lun11Next;

    if (curLun.day >= lun11ThisYear.day) {
        // Date is in the Sui anchored at Month 11 of year 'year'
        suiYear = year + 1;
        lun11Prev = lun11ThisYear;
        lun11Next = FindMonth11(year + 1, timeZone);
    } else {
        // Date is in the Sui anchored at Month 11 of year 'year - 1'
        suiYear = year;
        lun11Next = lun11ThisYear;
        lun11Prev = FindMonth11(year - 1, timeZone);
    }

    int monthOffset = curLun.index - lun11Prev.index; // 0 for Month 11
    int totalMonths = lun11Next.index - lun11Prev.index; // 12 (regular) or 13 (leap)
    int leapOffset = (totalMonths == 13) ? FindLeapMonthOffset(lun11Prev.index, timeZone) : 0;

    lunarLeap = 0;
    int m = monthOffset + 11;
    if (leapOffset > 0 && monthOffset >= leapOffset) {
        if (monthOffset == leapOffset) {
            lunarLeap = 1;
        }
        m = monthOffset + 10;
    }
    if (m > 12) {
        m -= 12;
    }
    lunarMonth = m;

    // In a Sui from Month 11 to next Month 11:
    // Month 11 and 12 precede Lunar New Year (Tết) and belong to civil year (suiYear - 1).
    // Months 1 through 10 belong to civil year (suiYear).
    if (lunarMonth >= 11) {
        lunarYear = suiYear - 1;
    } else {
        lunarYear = suiYear;
    }
}

// Year name in the sexagenary cycle (Can Chi).
inline std::wstring getCanChiYear(int lunarYear) {
    static const wchar_t* const kCan[] = {
        L"Giáp", L"Ất", L"Bính", L"Đinh", L"Mậu", L"Kỷ", L"Canh", L"Tân", L"Nhâm", L"Quý"};
    static const wchar_t* const kChi[] = {
        L"Tý", L"Sửu",  L"Dần",  L"Mão",  L"Thìn", L"Tỵ",
        L"Ngọ", L"Mùi", L"Thân", L"Dậu",  L"Tuất", L"Hợi"};
    int canIdx = (lunarYear + 6) % 10;
    if (canIdx < 0) {
        canIdx += 10;
    }
    int chiIdx = (lunarYear + 8) % 12;
    if (chiIdx < 0) {
        chiIdx += 12;
    }
    return std::wstring(kCan[canIdx]) + L" " + kChi[chiIdx];
}

}  // namespace VietnameseLunar

// The two calendars the dates are read in: the Vietnamese one the switch asks for, and the
// Chinese one, which is what the cells of the system's calendar show.
enum class LunarCalendar { Vietnamese, Chinese };

constexpr double kVietnamTimeZone = 7.0;
constexpr double kChinaTimeZone = 8.0;

// The date of a day: the text a cell shows, the text the header shows, and the same date in
// numbers, which is what a rule written as a day of a month is matched against.
struct LunarDateInfo {
    std::wstring name;        // "14/9", and "14/9N" in a leap month
    std::wstring headerText;  // "14 Tháng 9 Năm Bính Ngọ"
    LunarDay day;
};

LunarDateInfo CalculateLunarDate(int32_t key, LunarCalendar calendar) {
    const double timeZone =
        calendar == LunarCalendar::Vietnamese ? kVietnamTimeZone : kChinaTimeZone;
    const int year = static_cast<int>(key / 10000);
    const int month = static_cast<int>(key / 100) % 100;
    const int day = static_cast<int>(key % 100);

    int lunarDay = 0;
    int lunarMonth = 0;
    int lunarYear = 0;
    int lunarLeap = 0;
    VietnameseLunar::convertSolar2Lunar(day, month, year, lunarDay, lunarMonth, lunarYear,
                                       lunarLeap, timeZone);

    LunarDateInfo info;
    info.day.day = lunarDay;
    info.day.month = lunarMonth;
    info.day.leap = lunarLeap;
    info.name = std::to_wstring(lunarDay) + L"/" + std::to_wstring(lunarMonth) +
                (lunarLeap ? L"N" : L"");
    info.headerText = std::to_wstring(lunarDay) + L" Tháng " + std::to_wstring(lunarMonth) +
                      (lunarLeap ? L" (Nhuận)" : L"") + L" Năm " +
                      VietnameseLunar::getCanChiYear(lunarYear);
    return info;
}

// The date of a day is asked for by every cell of a month and again by the header, and again
// on every pass of the sweep, while the answer is the same for the whole day: it is kept.
// One map per calendar, since there are two of them and the date of a day in one is not the
// date of it in the other.
constexpr size_t kMaxLunarDates = 4096;

std::mutex g_lunarDateMutex;
std::unordered_map<int32_t, LunarDateInfo> g_lunarDates[2];

void LookupLunarDate(int32_t key, LunarCalendar calendar, LunarDateInfo& out) {
    const size_t index = calendar == LunarCalendar::Vietnamese ? 0 : 1;
    std::lock_guard<std::mutex> lock(g_lunarDateMutex);
    auto it = g_lunarDates[index].find(key);
    if (it != g_lunarDates[index].end()) {
        out = it->second;
        return;
    }

    // A date is a pure function of the day it belongs to, so a map which grew past this is
    // dropped rather than kept for the rest of the session: months browsed through for years
    // cost a multiplication each the next time they are looked at, which is nothing.
    if (g_lunarDates[index].size() >= kMaxLunarDates) {
        g_lunarDates[index].clear();
    }

    out = CalculateLunarDate(key, calendar);
    g_lunarDates[index][key] = out;
}

// Whether a text is one the mod wrote itself: the "14/9" the Vietnamese lunar calendar puts
// in a cell, and "14/9N" in a leap month. IsOwnText asks this so that a date the mod wrote
// is not mistaken for the shell's text when deciding whether to save the text binding.
bool IsVietnameseLunarText(std::wstring const& text) {
    if (text.empty() || text.size() > 7) {
        return false;
    }
    const size_t slash = text.find(L'/');
    if (slash == std::wstring::npos || slash == 0 || slash + 1 >= text.size()) {
        return false;
    }
    for (size_t i = 0; i < slash; i++) {
        if (!iswdigit(text[i])) {
            return false;
        }
    }
    const size_t end = text.back() == L'N' ? text.size() - 1 : text.size();
    if (end <= slash + 1) {
        return false;
    }
    for (size_t i = slash + 1; i < end; i++) {
        if (!iswdigit(text[i])) {
            return false;
        }
    }
    return true;
}

// Whether the date of a day is the day a rule asks for. A lunar day is only known once it
// was worked out, which happens exactly when a rule of that kind is written or the switch is
// on, so a null date answers with no rather than with a guess.
bool DateMatches(DateSpec const& wanted, LunarDateInfo const* lunar, int month, int day) {
    if (wanted.lunar) {
        if (!lunar) {
            return false;
        }
        if (wanted.day != 0 && wanted.day != lunar->day.day) {
            return false;
        }
        if (wanted.month != 0 && wanted.month != lunar->day.month) {
            return false;
        }
        return wanted.leap < 0 || wanted.leap == lunar->day.leap;
    }

    if (wanted.day != 0 && wanted.day != day) {
        return false;
    }
    return wanted.month == 0 || wanted.month == month;
}

// The rule which asks for the date of this day, if one is written. The rules are tried in
// the order they are written, like the rules of an event, so the specific rule goes above
// the general one. The rules of the feed are not among them: a day which the feed names is
// settled by the rule of that event, and a day no event names is marked by one of these or
// by nothing at all.
const DayRule* MatchDayRule(std::vector<DayRule> const& dayRules,
                            LunarDateInfo const* lunar,
                            int month,
                            int day) {
    for (const auto& entry : dayRules) {
        if (DateMatches(entry.day, lunar, month, day)) {
            return &entry;
        }
    }
    return nullptr;
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

// The same text in Unicode Normalization Form C, in which a letter with a mark over it is one
// character rather than a letter and a combining mark. A feed written on a Mac answers with
// the decomposed form as often as not, the way Vietnamese and Chinese are typed there, and a
// rule or an entry of the list to remove is written by the user in the composed one: the two
// are the same text to a reader and different to the mod, so the day would not be marked and
// the marker would stay in the name. Everything below reads the one form: the match, the
// text taken out, and the name the cell shows.
std::wstring NormalizeNfc(std::wstring const& text) {
    if (text.empty()) {
        return {};
    }
    int length =
        NormalizeString(NormalizationC, text.c_str(), static_cast<int>(text.size()),
                        nullptr, 0);
    if (length <= 0) {
        return text;
    }
    std::wstring normalized(static_cast<size_t>(length), L'\0');
    length = NormalizeString(NormalizationC, text.c_str(), static_cast<int>(text.size()),
                             normalized.data(), length);
    if (length <= 0) {
        return text;
    }
    normalized.resize(static_cast<size_t>(length));
    return normalized;
}

// Whether an event is marked at all, and how: the rule which matched decides the colour
// and the text of the day, and the name is the text of the event as the feed writes it,
// with what the user asked to be taken out of it (the "text to remove" setting) deleted.
// Nothing else is taken out - the word which marked the day is left where it stands, and
// an entry of that list is what takes a marker off a name.
//
// "元旦节（休）" with the rules the mod ships with is one such event: the rule of 休
// matches, so the day is drawn in its blue and the cell shows "元旦节（休）" - or
// "元旦节", with "（休）" written in the list above. An event which no rule matches - a
// solar term, a western holiday, anything the rules were not written for - is left out of
// the data, so that it can never be taken for a holiday.
//
// The list is used here, after the match and not before it: which day is marked is the
// feed's answer and not the settings' one, so an entry which covers a word of a rule -
// the whole "（休）" of "国庆节（休）", say - cannot stop a day from being marked.
bool ClassifyEventSummary(std::wstring const& summary,
                          std::vector<HighlightRule> const& rules,
                          HolidayInfo& out,
                          std::vector<std::wstring> const& removeTexts) {
    std::wstring text = NormalizeNfc(UnescapeIcsText(summary));

    const HighlightRule* rule = MatchHighlightRule(text, rules);
    if (!rule) {
        return false;
    }

    for (const auto& what : removeTexts) {
        RemoveAll(text, what);
    }

    out.name = TrimWhitespace(text);
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

// The length a DURATION names, in seconds, for an event which writes its end that way
// instead of with a DTEND. Weeks, days, hours, minutes and seconds are all read, because
// where the event ends decides which dates it covers: "P1DT8H" from nine in the morning
// ends in the afternoon of the next day, and that day is a part of the range. False is a
// value the mod cannot read, and the event is dropped then rather than given an end nobody
// wrote.
bool ParseDurationSeconds(std::wstring const& text, int64_t& seconds) {
    size_t i = 0;
    if (i < text.size() && text[i] == L'-') {
        return false;  // an end before the start is not a range
    }
    if (i < text.size() && text[i] == L'+') {
        i++;
    }
    if (i >= text.size() || text[i] != L'P') {
        return false;
    }
    i++;

    int64_t total = 0;
    bool inTimePart = false;
    bool sawSomething = false;
    while (i < text.size()) {
        if (text[i] == L'T') {
            inTimePart = true;
            i++;
            continue;
        }
        if (text[i] < L'0' || text[i] > L'9') {
            return false;
        }
        int number = 0;
        while (i < text.size() && text[i] >= L'0' && text[i] <= L'9') {
            number = number * 10 + (text[i] - L'0');
            // Checked while reading, so a run of digits which is too long is turned away
            // before it can overflow the int it is read into.
            if (number > 10000) {
                return false;
            }
            i++;
        }
        if (i >= text.size()) {
            return false;  // a number which is not followed by a part of its own
        }
        const wchar_t part = text[i++];
        int64_t partSeconds = 0;
        if (part == L'W' && !inTimePart) {
            partSeconds = static_cast<int64_t>(number) * 7 * 86400;
        } else if (part == L'D' && !inTimePart) {
            partSeconds = static_cast<int64_t>(number) * 86400;
        } else if (part == L'H' && inTimePart) {
            partSeconds = static_cast<int64_t>(number) * 3600;
        } else if (part == L'M' && inTimePart) {
            partSeconds = static_cast<int64_t>(number) * 60;
        } else if (part == L'S' && inTimePart) {
            partSeconds = number;
        } else {
            return false;
        }
        total += partSeconds;
        sawSomething = true;
    }

    seconds = total;
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
              HolidayMap& out,
              std::vector<std::wstring> const& removeTexts) {
    auto lines = SplitIcsLines(text);

    // One VEVENT as the feed wrote it. The events are collected before a date is drawn
    // from any of them, because a yearly rule which names no end is expanded over the
    // years the feed itself is about, and not over years of the mod's own choosing.
    struct Event {
        int32_t start = 0;
        // Where the start is in its day, and how long the event lasts when it writes a
        // DURATION: together they say where it ends, which decides the dates it covers.
        int startSecondsOfDay = 0;
        int64_t durationSeconds = 0;
        int32_t end = 0;
        bool hasEnd = false;
        // The end wrote a time of day which is not midnight, so its date is covered.
        bool endHasTimeOfDay = false;
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
            event.startSecondsOfDay = TimeOfDaySeconds(value);
        } else if (name == L"DTEND") {
            event.end = ParseDateKey(value);
            event.hasEnd = true;
            event.endHasTimeOfDay = HasTimeOfDay(value);
        } else if (name == L"DURATION") {
            // An event may write how long it lasts instead of when it ends. A duration
            // which cannot be read leaves the event with no end the mod knows, and an end
            // nobody wrote is not guessed at.
            if (!ParseDurationSeconds(value, event.durationSeconds)) {
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
    // expanded over. A feed which consists only of a rule written years ago would name no
    // year near today, so the expansion reaches next year as well.
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
    SYSTEMTIME now{};
    GetLocalTime(&now);
    const int currentYear = now.wYear;

    bool sawUnexpandedRecurrence = false;
    size_t added = 0;

    for (const auto& collected : events) {
        if (added >= kMaxHolidayDates) {
            // An earlier event already filled the feed: this one is dropped.
            hitDateLimit = true;
            break;
        }

        HolidayInfo info;
        if (!ClassifyEventSummary(collected.summary, rules, info, removeTexts)) {
            continue;
        }

        // Where the event ends, exclusive like a DTEND: an all-day DTEND names the first
        // day which is not covered, while one which wrote a time other than midnight (say
        // 17:00) covers the whole of its own date as well. A DURATION reaches to its day
        // the same way, and an event which wrote neither is its start day.
        int32_t end = 0;
        if (collected.hasEnd) {
            if (collected.end < collected.start) {
                // A DTEND which is there but ends before it starts: it did not parse, or
                // the feed is wrong. The event is dropped instead of being read as a single
                // day, because a wrong end would put the name on dates the feed never
                // named.
                droppedRanges++;
                continue;
            }
            // A timed event may end on the day it starts (09:00 to 17:00, say): that is
            // one day here, not an empty range. The same rule follows when the end is on a
            // later date, so the last date of a timed range is marked as well.
            if (collected.end == collected.start) {
                end = NextDateKey(collected.start);
            } else if (collected.endHasTimeOfDay) {
                end = NextDateKey(collected.end);
            } else {
                end = collected.end;
            }
        } else if (collected.durationSeconds > 0) {
            // A duration is added to the time of the start, and where that lands decides
            // the dates: an end which is not midnight (17:00, say) covers that whole date,
            // the same way a DTEND with such a time does.
            const int64_t total =
                collected.startSecondsOfDay + collected.durationSeconds;
            int32_t lastDate = collected.start;
            for (int64_t days = total / 86400; days > 0 && lastDate; days--) {
                lastDate = NextDateKey(lastDate);
            }
            end = lastDate && total % 86400 != 0 ? NextDateKey(lastDate) : lastDate;
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
            if (!key) {
                return;
            }
            out[key] = info;
            added++;
        };

        // How many days the event covers. An occurrence which is not the one the event
        // itself writes - an RDATE, or a later year of a yearly rule - lasts as long as
        // this one, so the same number of days is marked from that occurrence's date.
        int lengthDays = 0;
        for (int32_t key = collected.start;
             key && key < end && lengthDays < kMaxHolidayDates;
             key = NextDateKey(key)) {
            lengthDays++;
        }

        // The limit is looked at inside the loop as well: a single event which spans
        // millions of days is what a limit at the start of the event does not catch.
        auto addRange = [&](int32_t from) {
            // EXDATE cancels the occurrence which starts on that date, so its whole
            // range is left out rather than only the one day it names.
            if (isExcluded(from)) {
                return;
            }
            int32_t key = from;
            for (int day = 0; day < lengthDays && key; day++) {
                if (added >= kMaxHolidayDates) {
                    hitDateLimit = true;
                    return;
                }
                addDate(key);
                key = NextDateKey(key);
            }
        };

        addRange(collected.start);

        // The dates the event names of their own, which a feed writes as an RDATE: each of
        // them is an occurrence of the event, so each lasts as long as the event does.
        for (int32_t key : collected.ownDates) {
            if (added >= kMaxHolidayDates) {
                hitDateLimit = true;
                break;
            }
            addRange(key);
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
                // itself covers, and at least through next year, so that a feed made only
                // of an old yearly rule still marks the days near today.
                const int lastYearToUse =
                    (collected.yearly.count || collected.yearly.until)
                        ? 9999
                        : (lastYear > currentYear + 1 ? lastYear : currentYear + 1);
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
                    addRange(key);
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
                      HolidayMap& out,
                      std::vector<std::wstring> const& removeTexts) {
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

    return ParseIcs(body, rules, out, removeTexts);
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
bool CalendarIsShownNow();

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
                return L"https" + s.substr(colonSlashSlash);
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
    // A change which only touched the rules is not a reason to throw the response away:
    // the body is marked again with the settings which are current now. The generation is
    // read before the settings, so a change in between is caught by the check in
    // StoreHolidays.
    size_t entryCount = 0;
    std::shared_ptr<const Settings> current;
    for (;;) {
        const uint64_t generation =
            g_settingsGeneration.load(std::memory_order_relaxed);
        current = GetSettings();
        if (NormalizeIcsUrl(current->dataSourceUrl) != url) {
            Wh_Log(L"The data source changed while the feed was on its way; the "
                   L"response is dropped");
            return false;
        }

        HolidayMap parsed;
        if (!ParseHolidayData(body, current->rules, parsed, current->removeTexts)) {
            Wh_Log(L"Failed to parse the holiday data (%zu characters)", body.size());
            return false;
        }
        entryCount = parsed.size();

        // The body is stored with the data even when nothing was marked: the rules are a
        // setting like any other, and a feed which is read but marks nothing is one which
        // has to be marked again - from the body - as soon as the rules say so (see
        // Wh_ModSettingsChanged).
        if (StoreHolidays(generation, body, std::move(parsed))) {
            break;
        }

        // A setting changed while the body was being marked. The address is still the one
        // this response was asked for, so the response is the feed of this mod: it is
        // marked again with the current settings, rather than dropped and asked for again
        // after the retry delay.
        Wh_Log(L"The settings changed while the feed was being read; it is marked "
               L"again with the current settings");
    }

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

// Marks the data which is already loaded with a new rule list, and with the new text to
// remove, instead of asking the data source for the same feed again. The body which was
// kept is what makes this possible; it is read here rather than at the point the setting
// changed so that the parse - which walks every date of the feed - happens outside the
// lock which guards the data, and StoreHolidays publishes the result in one piece.
void RemarkStoredHolidays(std::vector<HighlightRule> const& rules,
                          std::vector<std::wstring> const& removeTexts) {
    const uint64_t generation = g_settingsGeneration.load(std::memory_order_relaxed);
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
    if (!ParseHolidayData(body, rules, parsed, removeTexts)) {
        // The body is the one which was read before, so this cannot happen except
        // through a change the mod does not know about; the data which is on screen is
        // left as it is rather than taken away.
        Wh_Log(L"The feed which was read before does not read again; the data is kept "
               L"as it is");
        return;
    }

    size_t entryCount = parsed.size();
    if (!StoreHolidays(generation, body, std::move(parsed))) {
        Wh_Log(L"The settings changed again while the feed was being marked; this "
               L"result is dropped");
        return;
    }

    if (entryCount) {
        Wh_Log(L"Marked the feed which was read before again with %zu rule(s) and %zu "
               L"text(s) to remove: %zu dates",
               rules.size(), removeTexts.size(), entryCount);
    } else {
        Wh_Log(L"Marked the feed which was read before again with %zu rule(s) and %zu "
               L"text(s) to remove, which match no event of it: no date is marked",
               rules.size(), removeTexts.size());
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

// The name the Windows 11 calendar gives the text block which holds the lunar date,
// solar term or festival name. This is the only block the mod writes into: a build
// which does not have it is left alone instead of guessed at.
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

// Whether the element is laid out and visible. A collapsed or not-yet-measured
// element is not a line the calendar is showing.
bool IsElementVisible(wux::DependencyObject const& element) {
    auto frameworkElement = element.try_as<wux::FrameworkElement>();
    if (!frameworkElement) {
        return false;
    }
    try {
        if (frameworkElement.Visibility() != wux::Visibility::Visible ||
            frameworkElement.Opacity() <= 0) {
            return false;
        }
        return frameworkElement.ActualWidth() > 0 && frameworkElement.ActualHeight() > 0;
    } catch (...) {
        return false;
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
            line += L" visibility=" + std::to_wstring((int)frameworkElement.Visibility());
            line += L" opacity=" + std::to_wstring((int)(frameworkElement.Opacity() * 100));
            line += L" size=" + std::to_wstring((int)frameworkElement.ActualWidth()) +
                    L"x" + std::to_wstring((int)frameworkElement.ActualHeight());
        }

        Wh_Log(L"%s", line.c_str());
    }
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

// Finds the text block which holds the shell's lunar/solar-term text. Only a
// non-empty block Windows names "LunarTextBlock" is accepted; a text block picked
// by its font size or contents would be a guess, and an empty template is not a
// line the calendar is showing.
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

    wuxc::TextBlock named = nullptr;
    for (const auto& found : descendants) {
        auto textBlock = found.element.try_as<wuxc::TextBlock>();
        if (!textBlock || ElementName(textBlock) != kLunarTextBlockName) {
            continue;
        }
        // A named block which has no text is not a usable second line yet: the
        // shell may not have filled the cell in, or this build may keep an empty
        // template here. Writing a holiday name into it would put text into a
        // line the calendar is not showing.
        if (ElementText(textBlock).empty()) {
            continue;
        }
        if (!IsElementVisible(textBlock)) {
            continue;
        }
        // The block the mod wrote to before wins while it is still there, so that
        // what was written there is not written a second time.
        if (preferredId && winrt::get_abi(textBlock) == preferredId) {
            return textBlock;
        }
        if (!named) {
            named = textBlock;
        }
    }
    return named;
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
    // date was filled in by the shell again, so the per-date state is refreshed.
    int32_t key = 0;
    // The binding the shell's template had on Text before the mod replaced it with a
    // local value. Re-applying it lets the shell's own data source - ShortTitle or
    // LunarTitle - supply the text again instead of the mod writing it back.
    wuxd::BindingBase savedTextBinding{nullptr};
    bool hasSavedTextBinding = false;
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
    // The text the mod wants the cell to show, if any: the mod's own text is told
    // from the shell's by this, both when the cell is recycled and when the shell
    // writes over it.
    std::wstring appliedText;
    // Whether the mod's colour is on the label right now. This belongs to the
    // label element rather than to the date: an element the shell reuses for
    // another month still carries the mod's brush.
    bool hadColor = false;
    // The tooltip the mod put on the day item, the text it shows, and the tooltip
    // the shell had there before. The tooltip belongs to the item, not to the text
    // block the mod writes to.
    std::wstring tooltipText;
    winrt::Windows::Foundation::IInspectable savedToolTip{nullptr};
    bool hasTooltip = false;
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
// and the calendar holds more than the six weeks a month view shows. A full list makes a
// pass drop the cells it found first, so the limit has to leave room for the tree; it is
// there for a shell which keeps day items alive in a pool the calendar no longer shows.
// A day item which is gone is released and dropped on the next pass, and one which is
// shown again is found again by the walk, and tracked again.
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
// A day cell the mod could not find the named second-line block in is worth reporting
// once, with the structure of the cell, and not once per cell.
thread_local bool t_loggedMissingTextBlock = false;
// Whether the shell's lunar line turned out to be a binding, reported once.
thread_local bool t_loggedTextBinding = false;
thread_local bool t_loggedRestoreBinding = false;
thread_local bool t_loggedNoTextBinding = false;

void LogMissingSecondLine(wuxc::CalendarViewDayItem const& item,
                          int32_t key,
                          bool empty) {
    if (t_loggedMissingTextBlock) {
        return;
    }
    t_loggedMissingTextBlock = true;
    if (empty) {
        Wh_Log(L"Day %d: the named \"%s\" text block is empty; the second line is "
               L"left unchanged",
               key, kLunarTextBlockName);
    } else {
        Wh_Log(L"No usable \"%s\" text block in the day cell of %d (missing or "
               L"empty); the second line is left unchanged. The additional "
               L"calendar may be off, or this Windows build may name the block "
               L"differently",
               kLunarTextBlockName, key);
    }
    LogDayItemStructure(item);
}

// Hitting the limit of tracked day items says that the shell is holding on to
// day items which the calendar no longer shows, which is worth reporting once.
thread_local bool t_loggedTrackLimit = false;
// Set by whatever a pass over a cell actually writes - the text, the colour, or the
// colour being taken back. SweepDayItems() uses it to tell "the shell is still rewriting
// the cells" from "nothing has happened for a while", which is what stops the timer again.
thread_local bool t_applyWrote = false;
// Set while a sweep sees a second-line text which is not the text the mod wrote
// for that cell - that is, a text the shell still owns. If a sweep finds cells
// without a holiday rule but no such text, the system's second line is gone and
// the mod's holiday names have to be taken off.
thread_local bool t_seenNativeSecondLine = false;
thread_local bool t_seenCellWithoutInfo = false;
// Whether the window which hosts the calendar of this thread is on screen. A closed
// flyout keeps its day cells alive and lets the shell empty their lunar text, which
// looks exactly like a calendar whose additional calendar was switched off. Only the
// window itself tells the two apart, and only a shown calendar may be read as "the
// second line is gone".
thread_local bool t_calendarShown = true;

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

// Puts the shell's own text back. The calendar's LunarTextBlock is bound to the day
// model in the shell's template; writing a local value replaces that binding. Putting
// the binding back makes the shell's own data source supply the text again, which is
// the value the system would draw for its current calendar settings. Returns false when
// no binding was saved; the caller then takes the mod's text off instead.
bool RestoreLabelBinding(wuxc::TextBlock const& label, DayCell& cell) {
    if (!cell.hasSavedTextBinding || !cell.savedTextBinding) {
        return false;
    }

    try {
        label.SetBinding(wuxc::TextBlock::TextProperty(), cell.savedTextBinding);
        cell.savedTextBinding = nullptr;
        cell.hasSavedTextBinding = false;
        t_applyWrote = true;
        if (!t_loggedRestoreBinding) {
            t_loggedRestoreBinding = true;
            Wh_Log(L"Restored the second line by re-applying the shell's binding");
        }
        return true;
    } catch (...) {
        return false;
    }
}

// Puts the tooltip of the day item back the way the shell had it: the object it
// carried before the mod wrote one, or no tooltip at all when it had none. Does
// nothing while the mod has not put one there.
void RestoreTooltip(wuxc::CalendarViewDayItem const& item, DayCell& cell) {
    if (!cell.hasTooltip) {
        return;
    }

    try {
        if (cell.savedToolTip) {
            wuxc::ToolTipService::SetToolTip(item, cell.savedToolTip);
        } else {
            item.ClearValue(wuxc::ToolTipService::ToolTipProperty());
        }
        t_applyWrote = true;
    } catch (...) {
    }

    cell.tooltipText.clear();
    cell.savedToolTip = nullptr;
    cell.hasTooltip = false;
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
// not worth treating as the shell's text when deciding whether to save a binding.
// appliedText cannot cover that window, because the shell recycles a cell by clearing
// what it holds while the mod's own holiday name is still on it; the name is then the
// only thing left to recognise. The price is that a lunar text which happens to equal a
// holiday name is not taken for the shell's own, which is harmless: the text the mod
// wants to write is the same one.
//
// A text of a rule which holds no {name} is recognised as it is written; a rule whose
// text is built from the name of the event is not, and what is written then is the name
// itself, which is covered by the second test below.
bool IsOwnText(std::wstring const& text, Settings const& settings) {
    if (text.empty()) {
        return false;
    }
    // The Vietnamese lunar date is the mod's own text as well: a cell which shows it does
    // not show the text of the shell, so it must not be taken as the source of a binding
    // to save (see IsVietnameseLunarText).
    if (settings.vietnameseLunar && IsVietnameseLunarText(text)) {
        return true;
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

// What the tooltip of a marked day says. An event gives its name; a rule which names a
// day of the calendar instead has no event, so the text that rule writes is shown. A rule
// which only colours the day has nothing to say and gets no tooltip.
std::wstring HolidayTooltipText(HolidayInfo const& info) {
    return !info.name.empty() ? info.name : ExpandName(info.text, info.name);
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
//   6) remember the shell's binding, so that the text can be restored later.
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

    // Read once per cell: only a calendar which is on screen may be read as "the
    // system's second line is gone". A closed flyout keeps the cells alive but lets
    // the shell empty their text, and clearing the mod's text then would wipe the
    // calendar the user is about to open.
    const bool calendarShown = CalendarIsShownNow();

    // A day item which now shows another date was filled in by the shell again. A
    // binding saved for the label element still applies and is kept; the colour state
    // stays too, because it belongs to the label element, which is still the same one
    // and still carries the mod's brush.
    //
    // The text the mod wrote is kept when a binding was saved: writing it as a local
    // value detached that binding, so the shell cannot update the line for the new date
    // until the binding is put back. Keeping appliedText lets the restore branch
    // recognise its own text and do that. Without a saved binding there is nothing to
    // put back, and the shell's own text is what the new date should show.
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
        if (!cell.hasSavedTextBinding) {
            cell.appliedText.clear();
        }
        cell.savedForeground = nullptr;
        cell.hasSavedForeground = false;
        cell.savedForegroundWasLocal = false;
    }

    const int month = (key / 100) % 100;
    const int day = key % 100;

    // The date of the day, in the calendar the switch names: what a cell shows while the
    // switch is on, and what a rule written as a day of a month is matched against with the
    // switch off as well. A day needs neither of the two - no switch and no rule of that
    // kind - and the calculation is skipped then, since it is the one thing a pass costs
    // that the shell does not do for it (see kMaxLunarDates).
    LunarDateInfo lunar;
    const bool haveLunar = settings->needsLunarDate;
    if (haveLunar) {
        LookupLunarDate(key,
                        settings->vietnameseLunar ? LunarCalendar::Vietnamese
                                                  : LunarCalendar::Chinese,
                        lunar);
    }

    // The rule which marks this day: the one an event of the feed matched, or - for a day no
    // event names - one whose word asks for the date of the day itself (see MatchDayRule).
    // The feed asks first: a day an event covers is settled by that event's rule, the date of
    // it or not.
    HolidayInfo info;
    bool hasInfo = LookupHoliday(key, info);
    if (!hasInfo) {
        const DayRule* dayRule =
            MatchDayRule(settings->dayRules, haveLunar ? &lunar : nullptr, month, day);
        if (dayRule) {
            hasInfo = true;
            info.text = dayRule->rule.text;
            info.color = dayRule->rule.color;
        }
    }

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

    // The shell's named lunar/solar-term text block.
    wuxc::TextBlock label = FindLabelTextBlock(item, cell.labelId);
    if (!label) {
        if (calendarShown && !hasInfo && DayItemHasContent(item)) {
            t_seenCellWithoutInfo = true;
        }
        // Only a calendar which is on screen can be showing a stale holiday name.
        // A closed flyout keeps the cells alive but lets the shell empty their
        // second line, which must not be mistaken for the line being switched off.
        if (calendarShown) {
            // A block the mod wrote to before can still be in the cell while the
            // named block is gone, hidden or empty. Do not leave the holiday name
            // behind in it.
            if (auto previous = cell.label.get()) {
                if (!cell.appliedText.empty() &&
                    ElementText(previous) == cell.appliedText) {
                    try {
                        previous.Text(L"");
                        cell.appliedText.clear();
                        t_applyWrote = true;
                    } catch (...) {
                    }
                }
            }
            if (DayItemHasContent(item)) {
                LogMissingSecondLine(item, key, false);
            }
            RestoreTooltip(item, cell);
        }
        return;
    }

    // A text block which wasn't seen before: register the callback which notices
    // the shell writing its own text, so that the holiday name survives it.
    if (cell.label.get() != label) {
        if (auto previous = cell.label.get()) {
            if (!cell.appliedText.empty() &&
                ElementText(previous) == cell.appliedText) {
                try {
                    previous.Text(L"");
                    t_applyWrote = true;
                } catch (...) {
                }
            }
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
        cell.appliedText.clear();
        cell.savedTextBinding = nullptr;
        cell.hasSavedTextBinding = false;
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

    // With the Vietnamese lunar calendar on, the cell carries the Vietnamese date of the day:
    // a rule which names no text leaves it there and only colours the day, and so does a day
    // no rule marks at all. What the cell shows otherwise is what the rule which marked the
    // day says, and nothing else: a rule whose text field was left out asks for the text of
    // the cell to stay, which ExpandName() answers with nothing for - the day is then marked
    // by colour alone, and a text built from the name of an event which has none of its own
    // (a rule written as a day of a month has no event) leaves the cell alone as well.
    std::wstring desired;
    if (settings->vietnameseLunar) {
        desired = lunar.name;
    }
    if (hasInfo && !info.text.empty()) {
        const std::wstring wanted = ExpandName(info.text, info.name);
        if (!wanted.empty()) {
            desired = wanted;
        }
    }

    std::wstring current = ElementText(label);
    if (calendarShown && !hasInfo) {
        t_seenCellWithoutInfo = true;
    }
    if (calendarShown && !current.empty() && current != cell.appliedText) {
        t_seenNativeSecondLine = true;
    }
    if (current.empty() && (hasInfo || settings->vietnameseLunar)) {
        // The shell can clear the line after the lookup above. An empty second
        // line is not a place to write a holiday name or a colour.
        if (calendarShown) {
            LogMissingSecondLine(item, key, true);
            RestoreTooltip(item, cell);
        }
        return;
    }

    // The tooltip of a marked day: the name of the event, or the text of a rule which
    // names a day instead. It belongs to the day item rather than to the text block, so
    // a name too long for the cell can still be read in full. The shell's own tooltip is
    // saved before the first write and put back when the option is off, the day is no
    // longer marked, or the cell has no line to show.
    std::wstring tooltip =
        hasInfo && settings->showHolidayTooltip ? HolidayTooltipText(info) : std::wstring();
    if (!tooltip.empty()) {
        if (!cell.hasTooltip || cell.tooltipText != tooltip) {
            try {
                if (!cell.hasTooltip) {
                    cell.savedToolTip = wuxc::ToolTipService::GetToolTip(item);
                }
                wuxc::ToolTipService::SetToolTip(
                    item, winrt::box_value(winrt::hstring(tooltip)));
                cell.tooltipText = std::move(tooltip);
                cell.hasTooltip = true;
                t_applyWrote = true;
            } catch (...) {
            }
        }
    } else {
        RestoreTooltip(item, cell);
    }

    if (!desired.empty() && current != desired) {
        if (current != cell.appliedText && !IsOwnText(current, *settings)) {
            // Remember the binding the shell's text came from, before the local value
            // below replaces it. The shell writes this line through a binding to the
            // day model, so putting that binding back is what makes the shell supply
            // the text again.
            try {
                bool savedBinding = false;
                if (auto expression =
                        label.GetBindingExpression(
                            wuxc::TextBlock::TextProperty())) {
                    if (auto binding = expression.ParentBinding()) {
                        cell.savedTextBinding = binding;
                        cell.hasSavedTextBinding = true;
                        savedBinding = true;
                        if (!t_loggedTextBinding) {
                            t_loggedTextBinding = true;
                            Wh_Log(L"Day %d: the second line is bound; the "
                                   L"binding is saved for restore",
                                   key);
                        }
                    }
                }
                if (!savedBinding && !t_loggedNoTextBinding) {
                    t_loggedNoTextBinding = true;
                    Wh_Log(L"Day %d: the second line is not bound; the mod's "
                           L"text will be taken off on restore",
                           key);
                }
            } catch (...) {
            }
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
        // The mod's text is not wanted any more: put the shell's binding back, or
        // take the text off when this build did not give the line a binding.
        if (!RestoreLabelBinding(label, cell)) {
            try {
                label.Text(L"");
                t_applyWrote = true;
            } catch (...) {
            }
        }
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

// Takes off the texts this mod wrote while the system's second line is no longer
// present. This is the fallback for a shell which leaves the last text in cells
// instead of clearing the line when the additional calendar is switched off.
size_t ClearModTextOnCurrentThread() {
    size_t cleared = 0;
    for (auto& entry : t_cells) {
        DayCell& cell = *entry.second;
        auto item = cell.item.get();
        if (!item || !IsElementVisible(item)) {
            continue;
        }
        auto label = cell.label.get();
        if (!label || cell.appliedText.empty() ||
            ElementText(label) != cell.appliedText) {
            continue;
        }

        // The system's line is gone, so the binding - if the shell uses one - will
        // evaluate to an empty value. Putting the binding back is still better than
        // leaving a local empty value on the block: when the line comes back, the
        // binding follows the shell again instead of staying detached.
        if (!RestoreLabelBinding(label, cell)) {
            try {
                label.Text(L"");
                t_applyWrote = true;
            } catch (...) {
            }
        }

        cell.appliedText.clear();
        cleared++;

        RestoreTooltip(item, cell);

        if (cell.hadColor) {
            RestoreForeground(label, cell);
            cell.hadColor = false;
        }
    }
    return cleared;
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
                } else {
                    t_calendarShown = false;
                }
            });
        t_windowTriggers.visibilityChangedToken = coreWindow.VisibilityChanged(
            [](wuc::CoreWindow const&,
               wuc::VisibilityChangedEventArgs const& args) {
                if (args.Visible()) {
                    OnCalendarOpenedOnCurrentThread();
                } else {
                    t_calendarShown = false;
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

bool IsCalendarWindowClass(PCWSTR className);

// The cheap answer for a cell which is being worked on right now: the CoreWindow knows
// its own visibility without a walk, and the newer target uses what the last sweep found
// out. A text callback can arrive while the flyout is closing, before the next sweep has
// looked at the window, so the window is asked here rather than only once per pass.
bool CalendarIsShownNow() {
    if (t_windowTriggers.coreWindow) {
        try {
            return t_calendarShown && t_windowTriggers.coreWindow.Visible();
        } catch (...) {
        }
    }
    return t_calendarShown;
}

// Whether the calendar of this thread is on screen. The CoreWindow answers this where
// there is one; the newer target has no CoreWindow, so its window is looked for by the
// thread it was created on. A thread without a calendar window at all is not a reason to
// believe the calendar is hidden.
bool CalendarWindowIsShownOnCurrentThread() {
    if (t_windowTriggers.coreWindow) {
        try {
            return t_windowTriggers.coreWindow.Visible();
        } catch (...) {
        }
    }

    struct WindowSearch {
        bool found = false;
        bool visible = false;
    } search;

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WindowSearch& search = *reinterpret_cast<WindowSearch*>(lParam);
            WCHAR className[32];
            if (!GetClassName(hWnd, className, ARRAYSIZE(className)) ||
                !IsCalendarWindowClass(className)) {
                return TRUE;
            }

            search.found = true;
            if (IsWindowVisible(hWnd)) {
                search.visible = true;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&search));

    return !search.found || search.visible;
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

////////////////////////////////////////////////////////////////////////////////
// The header of the calendar
//
// The line above the month is the lunar date of the day in the words and the calendar of the
// system ("农历八月十五"). The Vietnamese lunar calendar is read with the name of the year
// rather than with the date ("14 Tháng 9 Năm Bính Ngọ"), which is what the switch puts
// there: a Vietnamese reader looks for it in that line.
//
// A line is only ever touched when it says a lunar date of the shell's own form: the header
// of the flyout carries the month which is on screen as well, and a line which is not a
// lunar date is left as it is. What the shell wrote is remembered per text block, so that a
// calendar which is no longer a Vietnamese one - the switch turned off, the mod unloaded -
// does not keep the date of the mod behind.

// Whether a text is the lunar date of the shell's header: the word the shell writes it with,
// the year in front of the month or left out, the month as 正月 to 腊月 (the leap one as
// well), and the day as 初一 to 三十. The header also carries the name of the month which is
// on screen and, in another month, the name of a solar term or of a festival; none of those
// is a date, and each of them answers with no here.
bool IsChineseLunarHeaderText(std::wstring const& text) {
    auto trim = [](std::wstring value) {
        const size_t first = value.find_first_not_of(L" \t\r\n");
        if (first == std::wstring::npos) {
            return std::wstring();
        }
        const size_t last = value.find_last_not_of(L" \t\r\n");
        return value.substr(first, last - first + 1);
    };

    std::wstring value = trim(text);

    // The word the shell writes the lunar date with, in either script.
    if (value.rfind(L"农历", 0) == 0 || value.rfind(L"農曆", 0) == 0) {
        value = trim(value.erase(0, 2));
    }

    // The year, when the shell writes one: "丙午年八月十五" and "2026年八月十五" both name
    // the month after it, and the name of the year is not what is read here.
    const size_t yearEnd = value.find(L'年');
    const size_t firstMonthEnd = value.find(L'月');
    if (yearEnd != std::wstring::npos && firstMonthEnd != std::wstring::npos &&
        yearEnd < firstMonthEnd) {
        value = trim(value.erase(0, yearEnd + 1));
    }

    const size_t monthEnd = value.find(L'月');
    if (monthEnd == std::wstring::npos || monthEnd == 0 || monthEnd + 1 >= value.size()) {
        return false;
    }

    std::wstring month = trim(value.substr(0, monthEnd));
    if (!month.empty() && (month[0] == L'闰' || month[0] == L'閏')) {
        month.erase(0, 1);
    }
    // A month is one character, and the two months of the tenth to the twelfth are written
    // with two ("十一", "十二"; 腊月 and its written form "臘月" are the twelfth).
    if (month != L"十一" && month != L"十二") {
        if (month.size() != 1 ||
            std::wstring(L"正一二三四五六七八九十冬腊臘").find(month[0]) ==
                std::wstring::npos) {
            return false;
        }
    }

    const std::wstring day = trim(value.substr(monthEnd + 1));
    if (day.empty()) {
        return false;
    }
    if (day.size() == 2 && day[0] == L'初') {
        return std::wstring(L"一二三四五六七八九十").find(day[1]) != std::wstring::npos;
    }
    if (day == L"十" || day == L"二十" || day == L"三十" || day == L"廿") {
        return true;
    }
    if (day.size() == 2 && (day[0] == L'十' || day[0] == L'廿')) {
        return std::wstring(L"一二三四五六七八九").find(day[1]) != std::wstring::npos;
    }
    return false;
}

// What the mod wrote into one header and what the shell had written there: the text block of
// a header is used for the date of the day and for the month which is on screen, so the text
// of the shell is kept to be put back when the mod is not the one writing there any more.
// The header is set directly by the shell and has no binding to re-apply, which is why this
// one still saves its text.
struct LunarHeaderState {
    winrt::weak_ref<wuxc::TextBlock> label{nullptr};
    std::wstring savedText;
    std::wstring appliedText;
};

thread_local std::unordered_map<void*, LunarHeaderState> t_lunarHeaders;

// One text block of the tree, which may be the header of the calendar: the Vietnamese date
// is written where the shell wrote the lunar date of its own, kept there while the shell
// leaves the line alone, and forgotten when the block is used for something else.
//
// The date is the one of the day the mod was asked about, which is today: the header of the
// flyout is the date of today and not of the day which is selected - it does not move when
// another day is clicked, and it is the date which is on screen above the month as well.
void CheckAndApplyLunarHeader(wuxc::TextBlock const& textBlock, int32_t lunarDateKey) {
    const std::wstring current = ElementText(textBlock);
    void* id = winrt::get_abi(textBlock);
    auto it = t_lunarHeaders.find(id);
    const bool tracked = it != t_lunarHeaders.end() && it->second.label.get() == textBlock;

    if (tracked && current == it->second.appliedText) {
        // The line the mod wrote is still there, but the date of the day may be another one
        // by now: the shell rewrites the text of its header when the date it shows changes,
        // and the mod's own text is put back then.
        LunarDateInfo info;
        LookupLunarDate(lunarDateKey, LunarCalendar::Vietnamese, info);
        if (current != info.headerText) {
            try {
                textBlock.Text(info.headerText);
                it->second.appliedText = info.headerText;
                t_applyWrote = true;
            } catch (...) {
            }
        }
    } else if (IsChineseLunarHeaderText(current)) {
        LunarHeaderState& state = t_lunarHeaders[id];
        if (!tracked) {
            state = {};
            state.label = winrt::make_weak(textBlock);
        }
        state.savedText = current;

        LunarDateInfo info;
        LookupLunarDate(lunarDateKey, LunarCalendar::Vietnamese, info);
        try {
            textBlock.Text(info.headerText);
            state.appliedText = info.headerText;
            t_applyWrote = true;
        } catch (...) {
        }
    } else if (tracked) {
        // The block holds something else now - the name of another month, say - and what the
        // mod wrote into it is gone: there is nothing left to put back.
        t_lunarHeaders.erase(it);
    }
}

// Puts the text of the shell back into every header the mod wrote to on this thread, and
// forgets them: this calendar is not a Vietnamese one any more.
void UninitializeLunarHeaderTextOnCurrentThread() {
    for (auto& entry : t_lunarHeaders) {
        auto label = entry.second.label.get();
        if (!label) {
            continue;
        }
        try {
            if (!entry.second.appliedText.empty() &&
                ElementText(label) == entry.second.appliedText) {
                label.Text(entry.second.savedText);
            }
        } catch (...) {
        }
    }
    decltype(t_lunarHeaders)().swap(t_lunarHeaders);
}

// A thread without a XAML window is worth reporting once: "the mod does nothing" is
// otherwise hard to tell from "this month has no holidays".
thread_local bool t_loggedNoWindow = false;

void CollectDayItemsFromElement(wux::DependencyObject const& root,
                                size_t& budget,
                                int depth,
                                bool checkHeader,
                                int32_t todayDate) {
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

    // The header of the calendar is looked for on the walk which finds the day cells rather
    // than by a walk of its own: the tree is read either way, and a second walk over the
    // notification list above the calendar is a walk the shell would pay for.
    if (checkHeader) {
        if (auto textBlock = root.try_as<wuxc::TextBlock>()) {
            CheckAndApplyLunarHeader(textBlock, todayDate);
        }
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

        CollectDayItemsFromElement(child, budget, depth + 1, checkHeader, todayDate);
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

    // The header of the calendar is the lunar date of today, and the dates are only worth
    // working out - and only looked for in the tree - while the switch asks for them.
    const std::shared_ptr<const Settings> settings = GetSettings();
    const bool checkHeader = settings->vietnameseLunar;
    int32_t todayDate = 0;
    if (checkHeader) {
        SYSTEMTIME today{};
        GetLocalTime(&today);
        todayDate = MakeDateKey(today.wYear, today.wMonth, today.wDay);
    }

    try {
        size_t budget = kMaxScannedElements;

        if (auto content = window.Content()) {
            CollectDayItemsFromElement(content, budget, 0, checkHeader, todayDate);
        }

        // A flyout can be a popup rather than part of the window's content.
        for (auto popup : wuxm::VisualTreeHelper::GetOpenPopups(window)) {
            if (budget == 0) {
                break;
            }
            if (auto child = popup.Child()) {
                CollectDayItemsFromElement(child, budget, 0, checkHeader, todayDate);
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
    t_seenNativeSecondLine = false;
    t_seenCellWithoutInfo = false;

    // The check below asks whether any ordinary cell still has the shell's own second-line
    // text. With the Vietnamese lunar calendar on the mod writes into every cell, so no
    // such text is left to find and the check would always answer "the line is gone".
    // Only a calendar the mod does not own completely can be judged that way.
    const std::shared_ptr<const Settings> settings = GetSettings();
    const bool canJudgeSecondLine = !settings->vietnameseLunar;

    // A pass arms state on this thread - the window events, the tracked cells and their
    // callbacks, the timer which called it - so the thread is initialized first: the unload
    // only takes apart what it finds on a thread which is marked as initialized.
    InitializeForCurrentThread();

    // The day cells are looked for on every pass rather than remembered from a report, and
    // the window events are asked for again until the CoreWindow of this thread is there
    // to be watched.
    RegisterCalendarTriggersOnCurrentThread();
    {
        const bool shown = CalendarWindowIsShownOnCurrentThread();
        if (shown != t_calendarShown) {
            Wh_Log(L"The calendar window is now %s",
                   shown ? L"shown" : L"hidden");
            t_calendarShown = shown;
        }
    }
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

    // The shell can leave the mod's text in the cells after the additional
    // calendar is switched off. If no ordinary cell still has a second-line text
    // of its own, take the mod's text off instead of leaving it on screen.
    if (canJudgeSecondLine && CalendarIsShownNow() &&
        !t_seenNativeSecondLine && t_seenCellWithoutInfo) {
        const size_t cleared = ClearModTextOnCurrentThread();
        if (cleared) {
            Wh_Log(L"The system's second line is gone; cleared %zu holiday text(s)",
                   cleared);
        }
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

    // A calendar which is not a Vietnamese one any more - the switch was turned off - gets
    // the text of the shell back in its header. The walk above does not do it: it stops at
    // the day cells, and a header is not one of them.
    if (!GetSettings()->vietnameseLunar) {
        UninitializeLunarHeaderTextOnCurrentThread();
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
// property callbacks, puts the shell's binding and colour back while the cell still shows
// what the mod wrote, clears the state and stops the timer. A cell the shell has
// already repainted is left alone. Only cells which are still alive are handled; one
// the shell has destroyed is filled in by the shell itself. Nothing here changes the
// element tree, so the worst case is that the text and the colour are repainted a
// moment later.
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

        RestoreTooltip(item, cell);

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
                // Only take back a text the mod wrote and the shell has not
                // repainted since. A known binding is re-applied and the shell
                // supplies the current value; without one, the mod's text is taken
                // off rather than left in a cell the shell owns again.
                if (!cell.appliedText.empty() &&
                    ElementText(label) == cell.appliedText) {
                    if (!RestoreLabelBinding(label, cell)) {
                        label.Text(L"");
                    }
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
    UninitializeLunarHeaderTextOnCurrentThread();
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
// a CoreWindow, so the activation events are not available there.
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

// The window being hidden is the moment the flyout is closed. The cells stay alive and
// the shell may empty their second line on the way out; recording the hide keeps that
// from being read as the additional calendar having been switched off.
void OnWindowHidden(HWND hWnd) {
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

    RunFromWindowThreadViaPostMessage(
        hWnd, [](PVOID) { t_calendarShown = false; }, nullptr);
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

// The return value of ShowWindow says whether the window was visible before, not whether
// the call worked, so it is passed through and not tested.
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    const BOOL wasVisible = ShowWindow_Original(hWnd, nCmdShow);

    if (nCmdShow == SW_HIDE) {
        OnWindowHidden(hWnd);
    } else {
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
// checks the signatures: a hook whose signature has drifted from the function it
// replaces is an error here rather than a crash inside the shell.
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
// because the other settings (the rules, the text to remove) are only a different way of
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

    // A fetch or a re-mark which started under the previous settings is stale from here
    // on. The increment happens under the data lock, so a store cannot slip in between
    // its check and its write.
    {
        std::lock_guard<std::mutex> lock(g_holidaysMutex);
        g_settingsGeneration.fetch_add(1, std::memory_order_relaxed);
    }

    // The holidays of one data source are not the holidays of another, but the
    // data of the same source is kept: dropping it would make the calendar fall
    // back to the lunar text until the next fetch is done.
    if (settings->dataSourceUrl != previous->dataSourceUrl) {
        ResetHolidayData();
    } else if (!SameHighlightRules(previous->rules, settings->rules) ||
               previous->removeTexts != settings->removeTexts) {
        // The data is the feed of the same address, and only the marking of it changed:
        // the body which was kept is read again, so that the new rules take effect at
        // once instead of with the next fetch - which may be a day away - and without
        // asking the source for a feed it already answered with. Changing what is taken
        // out of a name changes the name of a day, which is a part of the marking as
        // well, so it is read again the same way.
        RemarkStoredHolidays(settings->rules, settings->removeTexts);
    }

    // Re-apply the new settings to the day items which are already on screen.
    RefreshCalendarOnUiThreads();

    // Data which is still fresh is not fetched again, so that changing a colour
    // doesn't cost a request.
    RequestHolidayData();
}
