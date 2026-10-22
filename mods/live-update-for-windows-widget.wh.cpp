// ==WindhawkMod==
// @id              live-update-for-windows-widget
// @name            Live Update for Windows 11
// @name:vi-VN      Live Update for Windows 11
// @description     Live taskbar activities: media, Windows Clock, battery, tasks, recording, displays, hotspot, privacy and lunar almanac
// @description:vi-VN Widget đa năng: Phương tiện, Đồng hồ, Pin, Tiến trình, Quay OBS, Trình chiếu, Quyền riêng tư, Âm lịch & Tử vi
// @version         2.10.4
// @author          caotu2k5
// @github          https://github.com/caotu2k5
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi -lgdi32 -luser32 -lwindowscodecs -luuid -ladvapi32 -lshell32 -lversion -lsetupapi
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Live Update for Windows 11

Live activities in one slot on the native Windows 11 primary taskbar: **Media**, **Battery**,
**Windows Clock**, **Lunar calendar & Almanac**, **Privacy activity**, **Task progress**, **OBS recording**,
**Displays & projection**, **Wi-Fi hotspot**, **UltraViewer sessions**, and optional **RAM/temperature alerts**.
The Vietnamese culture reader is a subpage of Lunar & Almanac.
Each activity can be enabled independently. The mod uses the technical ID
`live-update-for-windows-widget`. Disable the old mod before enabling this one;
Windhawk treats different IDs as separate installations and does not migrate settings automatically.

## Live activities

- **Media:** album artwork, track title and `elapsed / duration · artist`. The popup
  has Fluent previous/next controls and a larger circular play/pause button, with
  pointer, keyboard, disabled states and accessible names. Only players integrated
  with Windows media controls are supported. The most recently started playback is selected. Numbered buttons let you select a session until new playback starts.
- **Battery:** charge percentage, external-power/charging state and estimated time
  to full or remaining runtime. It uses Windows estimates, capacity/rate reports, then
  measured capacity changes when rate data is absent. Measuring needs at least two
  minutes and a measurable change (two percentage points on percentage-only hardware).
  Estimates reset on charging/discharging changes or sleep gaps. Charge limits or
  unsupported controllers may prevent an estimate; no constant charging speed is assumed.
  Connecting external power or crossing 20%, 10% or 5% opens an optional preview
  for 3–5 seconds (default 4). A threshold rearms after a 3-point recovery.
- **Automatic selection:** new playback, an observed clock start, connecting power or newly reported privacy
  use can select that activity. Track progress does not repeatedly change selection.
  A manually opened popup is not replaced. Closing a charging preview keeps the
  Battery activity selected. Disable automatic selection in Settings if preferred.
- **Independent activities:** disable Lunar calendar & Almanac without disabling
  Media or Battery. One activity has no switching arrows or popup tabs. With no
  available enabled activities, a compact settings gear remains.

Media changes use Windows media-session events; power changes use power notifications.
Commands do not wait for metadata or artwork downloads. Artwork is decoded on its
own background worker. The one-second tick updates elapsed time and the local calendar;
a slower recovery read handles missed provider events. A player's own response time
still depends on that application.

## Windows Clock (experimental reader)

The Clock activity shows **Stopwatch**, **Timer** and enabled **Alarm** entries
observed in Microsoft's Windows Clock app. It is status-only: it does not launch,
navigate or control the app, and it does not create independent timers.

Version 2.10.2 separates a countdown's remaining duration from the ending-time
badge (for example, `00:02:48` versus `18:46`). Three-part durations and identified
remaining-time readouts take priority over labels. The taskbar prefers a running
timer over paused presets. English/Vietnamese spoken durations, split digits and
stopwatch readouts are supported; lap rows are excluded.

Once a counter has been observed running, its last sampled value and monotonic
time anchor keep the display advancing when Clock is minimized, closed, or on
another page. Timer values count down to zero; paused values stay unchanged.
Reopening a readable page resynchronizes its state, including pauses and resets.
Missing scans do not erase known counters or repeat start/completion previews.

**Source limitation:** Windows Clock still needs to expose a readable page for
the initial observation and for later external changes. When a source cannot be
read, the status says **Last synced state**; an inferred expiry is **Estimated**.
The mod cannot confirm an unseen pause, cancellation, reset or alarm change.
These are bounded, in-memory observations for the current mod session, not a
replacement background alarm service and not persistence across Explorer restarts.

An observed start or timer completion can show a brief preview. A previously read,
enabled alarm with a readable schedule can show a reminder within 15 minutes and
a due-time preview, once per occurrence. Unavailable-source reminders are marked
as based on the last synced state. Windows Clock remains responsible for sounds.

The reader uses bounded raw-view UI Automation scans on a worker thread. Only
plain counter values are retained, with at most 48 entries; no UI Automation
objects survive a scan. Minimized/closed windows are not scanned. Controls are
in Settings > Live activities > Windows Clock.

## Lunar calendar & Almanac

Local UTC+7 calculations use the Hồ Ngọc Đức algorithm. The compact first line
includes the lunar date and Can Chi year; the second line rotates almanac information
at 15-second intervals and pins holidays instead of rotating them.

The daily popup includes Can Chi, zodiac, Nạp Âm, auspicious hours, Trực, suggested
activities and travel directions. Traditional almanac content is cultural information.

- **Previous / Today / Next:** navigate days, or months while Month view is open.
- **Month:** a seven-column calendar with solar and lunar dates. A third line marks
  holidays; select a day to read full holiday names and its almanac. Leap months use `*`.
- **List:** 13 lunar festival entries and 37 solar holidays/commemorations. Lunar New
  Year's Eve uses the actual final day of the lunar year, including 29-day months.
- **Culture:** opens the Vietnamese culture reader within Lunar & Almanac; Back returns to the calendar.
- **Regional default:** -1 enables Lunar & Almanac only when Windows country/region is Vietnam.
  Set 0 to disable or 1 to enable anywhere. Saved 0/1 preferences remain unchanged.

## Popup behavior and appearance

Click the taskbar widget to open or close its popup. A manual popup has no timeout.
Interacting with an automatic preview keeps it open. Click outside or press Esc to close.
Opening animates for 300 ms, closing for 240 ms. Switching activities, calendar pages,
privacy categories, media sessions, culture pages or app usage adds a 220 ms fade/slide.
The compact left/right arrows slide content horizontally in the selected direction;
the arrows stay still. Activity tabs also use a horizontal transition.
Content updates do not restart navigation animations; Windows reduced-motion settings
are respected for these transitions. Full popups measure their page content;
Month view shows all six weeks when screen space permits. Day view sizes holiday
text to its content and removes the holiday banner when there is no holiday. Long lists and small screens retain vertical scrolling. Activity tabs use
left/right navigation buttons with no horizontal scrollbar and disappear with one activity.
Automatic charging, low-battery, privacy, task, display and hotspot previews use a compact card. Clicking
one opens its full page and removes the timeout.

The gear opens a separate settings process with a wide, centered search field,
a sidebar, section headings and rounded setting cards. Seven pages cover General,
Appearance & layout, Live activities, Task progress, OBS, Lunar & Almanac, and Device management.
The sidebar collapses to icons. Settings has optional Windows Desktop Acrylic on supported
Windows 11 builds, with an opaque fallback when effects are unavailable.
Related controls are grouped within each page, including Battery & Bluetooth,
Privacy, Browsers & downloads, and Files & archives. Search spans all pages and is debounced.

The interface runs on demand in Windows PowerShell/.NET WPF, with its own window
and taskbar identity. Its embedded script is sent over stdin: no helper executable
or script file is installed. A local pipe is restricted to the current user and
verified against the child process ID. Closing the settings window exits that
process and releases its memory. The taskbar component still runs in Explorer;
this does not move the entire mod out of Explorer. Windows policy must allow
Windows PowerShell and WPF to run. No execution-policy bypass or elevation is used.

Switches and lists save immediately; text/number fields have Save buttons.
The three shared Windhawk fields remain Language and the two widths. Existing
preferences are preserved. A compact settings gear remains when no activity is active.

**Liquid glass** is the default for new settings. Existing theme choices are preserved.
Choose **Windows blur**, **Liquid glass**, or **Off (opaque)** for the popup.
This mod's CPU policy falls back to Windows Blur for Intel Core i 6th generation or older,
and AMD processors before Ryzen/Zen. Unrecognized CPUs also fall back to Blur. This is a
product performance policy, not a claim that these CPUs cannot render Windows effects.
If Windows disables transparency, the surface becomes opaque.
Glass adds a GPU-transformed backdrop with a slight lens magnification, lower tint,
a bright edge and pointer-following specular highlights. This is an approximation
using Windows composition, not Apple's Liquid Glass renderer. The effect falls back
when unsupported and respects high contrast. The mod no longer disables effects because battery saver is active.
Glass popup text switches between light and dark according to the background.
A temporary 16×16 luminance sample is taken on opening and at most every two seconds
while visible; no image is saved or transmitted. The taskbar root stays transparent
and its text continues to follow the taskbar theme. Its background is a separate layer:
idle is transparent, hover shows 30% opacity, and press/open-popup shows the full
material. Liquid Glass is used when selected and supported; otherwise the taskbar
uses Windows composition blur, including when the popup theme is Off. The blur
source is behind the background layer, never the text. A native Acrylic/opaque
fallback handles unavailable composition; high contrast takes priority. No
Explorer-wide transparency settings are modified. Glass buttons retain their rims
and pointer/pressed states.
Selected buttons follow Windows' accent color;
high-contrast colors take priority.

Activity preferences have one canonical per-mod store; the old quick-menu override
layer is migrated once. Windhawk's Settings tab contains only Language, Compact width
and Popup width. Saving those fields imports them without resetting activity switches.
When `windhawk-cli.exe` with the current `mod settings set <id> key=value` syntax is
available and permitted to write, widget changes to the shared fields are sent back
through that supported CLI. The command uses this mod's actual runtime ID.
**Windhawk 1.x has no settings-write API or CLI**, and some Windhawk 2 installations
reject writes from an unelevated process. In either case the widget keeps its saved
values and reports that sync is unavailable/rejected; it does not pretend that the
Windhawk form changed, elevate a process, or directly edit engine registry/INI files.
On these installations, save Language/Width from Windhawk when both views must match.

The **Hide** button in each full popup temporarily removes that activity. New relevant
activity can reveal it again: new playback/task, a power or low-battery change,
new privacy use, recording, projection/hotspot change, or a new calendar day.
Ordinary elapsed-time and percentage updates do not repeatedly reveal it.
**Show hidden widgets** in General settings restores hidden activities immediately.
Hide is distinct from disabling an activity in Settings.

The selected theme controls the mod's effects. Battery saver no longer changes the
mod's blur/glass eligibility, and the `AdvancedEffectsEnabled` power-saving signal is
not used as a veto. High contrast retains an opaque fallback. Windows' compositor can
still limit actual transparency independently of the mod. No Explorer-wide transparency
setting is changed.

## Controls and compact mode

Settings text/password fields and dropdowns use rounded templates with system-accent
focus indicators, a floating dropdown surface and a selected-item marker. Liquid glass
uses two non-interactive rim strokes above the content; routine data updates no longer
replace the glass border with a flat theme border.

**Collapsed taskbar** shows a Live Update icon and one line with the number of available
activity categories, without switching arrows. This counts live activities, not Windows
notifications. Toggle it beside the popup gear or in Appearance & layout. Clicking the
collapsed slot opens the selected activity as usual.

**Memory & storage** lists fixed local volumes with total/free space and a usage bar,
and opens Windows storage settings on request. It does not delete files or modify partitions.
Volumes are queried only while the settings resource page is open.

## Media controls and multiple sessions

Drag or click the seek slider to change position when the player supports Windows
media seeking. The thumb tooltip uses `m:ss` or `h:mm:ss`, not raw seconds. Elapsed time is left of the centered transport buttons; total duration
is on the right. Numbered buttons below select among up to 16 playing/paused Windows
media sessions; they disappear when only one session exists. These are media sessions,
not playlist entries or a guaranteed list of every browser tab. Commands target the
selected session. Artwork is fetched separately and cannot block transport commands.

## Privacy activity

Microphone activity uses Core Audio capture sessions. Camera activity uses the Windows
Media Foundation sensor activity monitor and process IDs; neither provider records or
opens a capture stream. Capability Access Manager records from the current user and
machine are a fallback. Location also reads the passive Windows location-use signal
(WNF_LFS_STATE), at most four times per second, without requesting coordinates. If it
reports use, the mod also reads the visible native tray location label on the taskbar
UI thread to obtain the application's display name. It does not open the tooltip or
guess from the foreground app. If neither Windows source exposes a usable identity,
the list says Windows Location Service. Native labels can remain in the Windows UI language.
The private signal is validated and falls back to activity records when unavailable.
Registry changes wake the provider; a recovery read handles missed events.
Privacy previews use capability sessions with a 15-second quiet period and a startup
baseline: app-name resolution, brief signal gaps and recent-history rows do not reopen previews.

Application rows appear above capability buttons for Microphone, Camera and Location.
Combined native tooltips are parsed one sensor section at a time. A Location row
cannot include a Microphone heading. Identical display names within a capability
are merged; active use takes precedence over a recent-history row.
Each button filters the list and uses its matching icon. Short location
requests may finish between refreshes: completed requests remain visible for 15 seconds
as **Recently accessed**, rather than being labelled active. Coverage still depends on
Windows reporting the activity; an empty list does not prove that a sensor is unused.

## Bluetooth batteries

The Battery page includes connected Bluetooth devices, their names and the battery
percentage Windows reports. It also works on a desktop without an internal battery.
A newly connected device or a new 20% / 10% low-battery threshold can show a short
Battery preview; the initial device list is a silent baseline. Thresholds rearm after
three percentage points of recovery. Disable **Battery · Bluetooth devices** to stop
this provider, or disable Battery automatic popup to keep the section without previews.

A dedicated worker watches Classic and LE device changes and uses a 15-second
recovery scan, so slow Bluetooth reads cannot block other system providers. It merges the two Windows container identity properties, reads driver battery
properties and, for already connected LE devices without a value, tries the standard
GATT Battery Service. Reads are bounded; at most four GATT devices are probed per scan.
It does not pair devices, change the radio, open audio streams or force a GATT connection.
Unknown levels are displayed as unavailable, not 0%. Some devices/drivers do not publish
battery data, and driver reports may arrive more slowly than the device's actual level.

## Memory monitoring

The Memory settings page shows physical RAM usage, the separate settings process,
and up to 100 application groups sorted by working set. Explorer's working set is
labelled as including Windows and other mods: Windows does not provide an accurate
per-mod share of that process's XAML/heap allocations. Shared pages can appear in
more than one application's working set; the list is not a sum of physical RAM.

Optional alerts cross 80%, 90% and 95%, show the top five application groups, and
rearm only after usage drops more than three percentage points below a threshold.
They use the same manual/automatic popup rules as other activities. The RAM activity
appears during memory pressure; ordinary RAM management remains in Settings.
The page opens Task Manager for managing applications. It does not kill applications
or force working-set trimming. Closing settings releases its own process normally.

Physical memory is sampled every five seconds. Process enumeration runs only during
memory pressure or while the Memory page is being watched. Only the top five entries
are retained outside that page, and all process handles are closed after each read.

## Battery controls and app usage

Two power-mode selectors control **Plugged in** and **On battery**, using Windows'
PowerGet/SetUserConfiguredAC/DCPowerMode APIs. Unsupported configurations disable the
selector; failed changes display an error instead of pretending the mode was applied.
Windows energy-saving policies can override the effective mode.

**App usage time** opens a dedicated page with a Back button and application/time rows.
It shows foreground application time, excluding sleep, the lock screen
and idle periods longer than a minute. While plugged in it shows the current observed
PC session; on battery it shows time since the observed unplug event. Collection starts
when the mod is enabled: it cannot reconstruct earlier boot/unplug history. Reloading
the mod resets the in-memory counters. No app-usage log is written to disk. At most 128
application totals are retained, and the usage list refreshes only while its page is open.

## Task progress and application selection

Choose supported source applications in Settings. Downloads show **Downloading N files**
with one row per identified active download; filenames stay internal for matching status updates. File operations retain their operation and filename where available.
When the application exposes only aggregate taskbar progress, the widget shows
**Downloading / Overall progress**: it cannot infer a file count or individual percentages.

This widget monitors progress only, including downloads and file operations. It has no
Pause, Resume or Cancel commands. Open application brings the existing source window
forward; manage the operation in that application. Background observation never opens
browser download UI or invokes application controls.

The provider reads exposed progress/status data and optionally a browser's download table.
Database locks are respected. Per-file information depends on the source application.
Missing percentages are indeterminate; aggregate progress is not duplicated across files.
Completion/cancellation previews require a reported outcome. No process is suspended or killed.

## UltraViewer connections

Active UltraViewer session windows supply outgoing peer names/IDs. Native incoming-session
labels are recognized where exposed. The mod does not read passwords, chat, or the remote
desktop. Opening the main window alone is not treated as a connection. Two consecutive
missing samples confirm disconnection; a compact receipt closes after four seconds.
Incoming labels that the installed version does not expose cannot be detected reliably.
Completed-session log records are no longer used as live connection events.

## Device temperature

Enable LibreHardwareMonitor's **Remote Web Server** on this PC. The local port defaults
to **8085** and is configurable under Device management. This optional source is required;
the mod does not install a sensor driver. Only `http://127.0.0.1:<port>/data.json` is read,
with redirects disabled, a 256 KiB response cap and bounded waits on a separate worker.
Supported CPU/GPU readings are sampled every five seconds; unavailable sources retry
after 30 seconds. Missing readings show Unavailable, never zero. Alerts trigger at
70/80/90 °C and rearm five degrees below each threshold. These are configurable-feature
notification thresholds, not universal hardware damage limits.

Right-click the taskbar widget to collapse it, select a surface theme or open Settings.
A confirmed OBS recording stop produces a compact four-second receipt, including when
Stop was pressed in the OBS widget. Losing the OBS connection is not a confirmed stop.



## Displays & projection

The display widget shows active external Windows display paths, monitor names,
the reported connection type and duplicate/extend mode. It reads
[QueryDisplayConfig](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-querydisplayconfig),
with display-change notifications and a five-second recovery check. Connecting,
disconnecting or changing the projection topology can open a short preview;
the initial configuration does not produce a startup popup.

HDMI, DisplayPort, DVI, VGA, indirect displays and Miracast paths are recognized when
Windows reports them. USB-C display output often appears as DisplayPort: the API
cannot reliably identify the physical cable. Browser-only casting to Chromecast
or a remote video receiver is outside this provider. Merely opening Win+P is not
treated as casting. **Projection options** opens Windows' native projection menu;
**Display settings** opens Windows Settings. No display mode is changed automatically.

## Wi-Fi hotspot

The widget reads Windows Mobile hotspot state, SSID, connected-client count and the device list via
[NetworkOperatorTetheringManager](https://learn.microsoft.com/en-us/uwp/api/windows.networking.networkoperators.networkoperatortetheringmanager).
The full popup lists device hostnames when provided, plus reported IP/MAC addresses.
If a hostname is unavailable, the MAC address identifies the device. An unavailable
list is labelled explicitly rather than presented as an empty list.
It checks once per second while enabled and reacts to network-status changes.
Turning the hotspot on/off or a confirmed client-count change can show a short
preview. Startup, profile refreshes and temporary unavailable readings do not
repeat the preview. Unknown client counts are not presented as zero.

The provider needs Windows and the Wi-Fi driver to expose the tethering APIs to
Explorer. If access is denied or the provider fails, status is unavailable, not Off.
**Turn off hotspot** calls Windows' `StopTetheringAsync` only when pressed. The button
is disabled while the request is pending; failures are reported. No hotspot is started
automatically. The Settings button opens **Mobile hotspot**. SSID/password and sharing
configuration are left unchanged.

## OBS Studio recording

Enable **Tools > WebSocket Server Settings > Enable WebSocket server** in OBS Studio
(WebSocket 5, included in OBS 28+). Set the mod's **OBS WebSocket port** (default 4455)
and **OBS WebSocket password** to match OBS. The password is stored in mod settings;
remove it before sharing settings exports. The connection is restricted to `127.0.0.1`.

The Recording widget appears while OBS is recording. Its clock is anchored to
OBS milliseconds and advances locally between responses, with half-round-trip
latency compensation (capped at 250 ms). A lightweight 100 ms UI tick changes text
only when the displayed second changes. Responses predating a recording-state
event are discarded. This is a status clock, not a frame-accurate OBS timecode. It includes **Pause / Resume** and **Stop recording** buttons. A new recording can
automatically select the widget. Paused time does not advance; a stopped recording
disappears unless its manual page is open. Some OBS encoders/output configurations
do not support pausing; rejected commands display an error.

The mod checks for the OBS process before opening a connection.
Status uses OBS output events and one-second queries while recording (three seconds
while idle). Disconnections clear stale recording state and retry after five seconds.
No recording is started automatically, no OBS hotkeys are simulated, and no recordings
or credentials are sent to a remote service. This feature does not monitor other recorders.

## Vietnamese culture reader

The built-in topic index includes clothing, festivals, customs, arts, food and crafts.
Article text and thumbnails are fetched only when opened. The default provider is
**Wikipedia in the selected language**, with a source/revision-history link and CC BY-SA attribution.
English articles use Wikipedia language links; unavailable translations show an English message.
These are community-written reference articles, not a separately verified editorial
collection. Consult their cited sources for historical claims.

Only the currently viewed page is retained. There is no article history cache and no
local content/image cache file. Leaving the culture page or closing the popup cancels
obsolete requests and releases article text, images and their XAML controls. Opening
an article again fetches it again. Temporary download/render buffers still use memory;
this is not a zero-RAM reader. Other activities work without an Internet connection.
HTTPS requests, parsing and image decoding run off the taskbar thread with size and
time limits. The culture worker starts only when the reader is first opened.

To maintain your own editorial collection, set **Custom culture catalog URL** to a
public HTTPS JSON endpoint with this structure:

```json
{"articles":[{"id":"ao-dai","title":"Áo dài","category":"Trang phục","articleUrl":"https://YOUR-HOST/articles/ao-dai.json"}]}
```

Each article endpoint returns:

```json
{"title":"Áo dài","content":"Your sourced article text…","sourceUrl":"https://YOUR-SOURCE/","imageUrl":"https://YOUR-HOST/ao-dai.jpg","imageSourceUrl":"https://YOUR-IMAGE-CREDIT/"}
```

For English catalogs, add `language: "en"` to each entry and point to English article
content, or supply `titleEn`, `categoryEn` and `articleUrlEn`. The mod does not translate
custom article text. Replace the illustrative URLs with your actual endpoints. Images are optional.
The catalog is limited to 200 topics; JSON responses to 1 MiB, compressed image bytes
to 4 MiB, and displayed article text to 24,000 characters. Longer articles link to the
full source. No account, API key or additional local program is required.

## Settings and compatibility

English labels are the default; Auto uses Vietnamese on a Vietnamese Windows UI.
English also localizes almanac terms, hours, stems, branches and the built-in culture index.
All settings are grouped in the gear window; Windhawk exposes only language and sizes.
Existing activity preferences are migrated when the previous keys remain available.
Choose **Appearance · Progress bar style**: Thin (classic), Rounded capsule (default),
or Segmented. This applies to battery/task bars; the media seek slider stays interactive.
Unknown progress and high-contrast mode use the native Windows progress bar.

| Setting | Default | Meaning |
| --- | --- | --- |
| Enable taskbar widget | On | Master switch |
| Lunar calendar & Almanac | -1 (region) | Windows region Vietnam; 0 off, 1 on |
| Displays / Wi-Fi hotspot | On | Independent providers with optional change previews |
| Progress bar style | Rounded capsule | Thin, capsule or segmented |
| Media / Battery / OBS recording | On | Independent activity switches; OBS requires WebSocket setup |
| Windows Clock / Clock preview | On | Status-only reader; background estimate after first sync; alarm reminder within 15 minutes |
| Privacy activity / Task progress | On | Independent activity switches |
| Culture reader | On | Button inside Lunar & Almanac |
| Privacy / Task automatic popup | On | Four-second previews; manual view takes priority |
| Automatically select new activities | On | React to newly started activities |
| Always show battery | Off | Otherwise shows on external power or at 20% or below |
| Battery automatic popup | On | Charging / low-battery previews |
| Preview duration | 4 s | Range 3–5 s |
| Position | `tray_left` | Tray placement reserves its own space |
| Widget width | 270 DIP | Range 140–480 |
| Popup width | 540 DIP | Range 320–640 |
| Left overlay offset | 12 DIP | Range 0–2500; only for `root_left` |

Other tray positions are before/after the clock and rightmost. Optional `root_left`
is an overlay and can cover native buttons; adjust its offset or use a tray position.
**Compatibility:** Windows 11 x64, build 22000 or later, with the native primary
XAML taskbar. Windows 10, replacement taskbars and secondary taskbars are not supported.
Unsupported Windows versions are rejected before hooks or provider threads start.
The mod does not alter the native clock, clock tooltip, calendar flyout or application notifications.
Task progress support is limited to the reporting applications and protocol described above.

Install from the Windhawk catalog and configure activities from the widget gear.
For diagnostics, enable Detailed debug logs and open the log viewer before enabling the mod.
The current implementation targets x64.

## Tiếng Việt

**Live Update for Windows 11** gom nhiều hoạt động vào một vị trí trên taskbar.
Có công tắc riêng **Âm lịch & Tử vi**, Phương tiện, Pin, Quyền riêng tư, Tiến trình,
Quay OBS, Trình chiếu và Điểm phát sóng.
Văn hóa Việt nằm trong nút **Văn hóa** của Âm lịch & Tử vi. Mặc định -1 bật lịch
khi Quốc gia/khu vực Windows là Việt Nam; 0 tắt, 1 bật mọi khu vực. Giữ lựa chọn cũ 0/1.
Chỉ một mục thì ẩn nút chuyển/tab; không có hoạt động thì giữ bánh răng cài đặt nhỏ.

**Bản 2.9:** cửa sổ cài đặt chạy trong tiến trình riêng, thanh tìm kiếm dài ở giữa,
chia 7 trang và các nhóm chức năng ngay trong từng trang. Đóng cửa sổ thì tiến trình
cài đặt thoát; phần gắn vào taskbar vẫn nằm trong Explorer. Giao diện dùng Windows
PowerShell/WPF có sẵn, mã nhúng trong file mod, không tạo file script riêng.

**RAM:** trang cài đặt hiển thị mức dùng RAM, tiến trình cài đặt và danh sách ứng dụng.
RAM của Explorer được ghi riêng, không coi đó là RAM của mod. Cảnh báo 80%/90%/95%
hiện top 5 ứng dụng; có chống lặp khi phần trăm dao động. Nút Trình quản lý tác vụ
giúp quản lý ứng dụng; không ép dọn RAM hay tự tắt ứng dụng.

**Bluetooth:** theo dõi riêng Classic/LE, ghép container và bổ sung Battery Service
cho thiết bị LE đã kết nối. Thiết bị không cung cấp mức pin vẫn ghi chưa có dữ liệu.

**Tải xuống:** hiển thị số file và các thanh tiến trình riêng khi ứng dụng cung cấp dữ liệu;
không hiển thị tên file. Nếu chỉ có tiến độ tổng, hiển thị Tổng tiến độ, không đoán số file.
Chỉ theo dõi trạng thái; bỏ toàn bộ nút Tạm dừng/Tiếp tục/Hủy, kể cả thao tác tệp.
Nút Mở ứng dụng đưa cửa sổ nguồn ra trước để bạn thao tác trong ứng dụng đó.

**UltraViewer:** đọc cửa sổ phiên đang hoạt động để hiện tên máy/ID kết nối đi;
nhận kết nối đến khi phiên bản UltraViewer xuất nhãn tương ứng. Khi ngắt kết nối,
preview tự đóng sau 4 giây; không dùng nhật ký phiên đã kết thúc làm sự kiện kết nối mới.

**OBS:** chỉ báo đã dừng sau khi OBS xác nhận dừng quay; mất kết nối không được coi là dừng.
**Nhiệt độ:** trong Quản lý thiết bị, bật cảnh báo CPU/GPU 70/80/90 °C. Cần chạy
LibreHardwareMonitor và bật Remote Web Server, cổng mặc định 8085. Nếu không có cảm biến,
hiện Chưa có dữ liệu. Theo dõi mỗi 5 giây, cảnh báo lặp lại sau khi nhiệt độ giảm 5 °C.
**Chuột phải:** thu gọn Live Update, chọn giao diện hoặc mở cài đặt.

**Thu gọn:** nút cạnh bánh răng và mục Giao diện chuyển taskbar thành biểu tượng + số hoạt động,
không có mũi tên chuyển. Menu cài đặt thu gọn thành biểu tượng và có blur Windows.
**Quản lý thiết bị:** dung lượng ổ cục bộ, thanh mức sử dụng và nút mở cài đặt lưu trữ Windows.

**Bản 2.10.2:** sửa đọc nhầm giờ kết thúc thành thời gian hẹn giờ còn lại;
ưu tiên bộ hẹn giờ đang chạy trên taskbar. Bộ đếm đã đọc được tiếp tục hiển thị
khi thu nhỏ/đóng Đồng hồ hoặc đổi trang, không bị xóa sau 4 giây. Khi đọc lại được
thì đồng bộ trạng thái tạm dừng, đặt lại và thời gian mới. Bỏ nút Mở Đồng hồ Windows.

**Nền taskbar:** bình thường trong suốt; rê chuột hiện nền 30%; bấm/đang mở popup
hiện đầy đủ. Nền có lớp riêng, không làm mờ chữ. Chọn Liquid Glass thì dùng kính khi
được hỗ trợ; Blur hoặc Off của popup vẫn dùng blur trên taskbar.

**Đồng hồ:** chỉ hiển thị trạng thái Bấm giờ, Hẹn giờ và Báo thức từ Đồng hồ
Windows (tiếng Việt/Anh). Cần đọc được trang tương ứng ít nhất một lần. Khi ứng
dụng thu nhỏ, đóng hoặc trang không còn đọc được, widget tiếp tục từ mốc chạy đã
ghi nhận và hiện **Theo lần đồng bộ cuối**; hết giờ suy ra từ mốc này có ghi
**Ước tính**. Không thể xác nhận các thay đổi chưa đọc được trong ứng dụng.
Nhắc trước 15 phút dựa trên lịch báo thức đang bật đã đọc được; ứng dụng Đồng hồ
vẫn chịu trách nhiệm phát chuông. Dữ liệu giữ trong phiên mod, không qua khởi động
lại Explorer. Widget không mở hay điều khiển ứng dụng, không tạo bộ đếm độc lập.

**Tương thích:** chỉ hỗ trợ Windows 11 x64, build 22000 trở lên, với taskbar XAML
chính của Windows. Không hỗ trợ Windows 10, taskbar thay thế hoặc taskbar màn hình phụ.
Mod kiểm tra phiên bản Windows trước khi cài hook và khởi động các luồng theo dõi.
**Mặc định:** Liquid Glass cho cấu hình mới, giữ lựa chọn đã lưu. Intel Core i thế hệ 6 trở xuống,
AMD trước Ryzen/Zen hoặc CPU chưa nhận diện sẽ dùng Windows Blur; tắt trong suốt thì dùng nền đặc.
**Kính:** thêm phóng nền nhẹ, viền sáng và phản sáng theo con trỏ bằng Windows
Composition; popup đổi màu chữ theo độ sáng nền từ mẫu 16×16 tạm thời. Nền taskbar trong suốt, màu chữ taskbar giữ theo hệ thống. Đây là hiệu ứng mô phỏng,
không phải bộ dựng Liquid Glass của Apple.
Windhawk chỉ còn ngôn ngữ và hai chiều rộng. Lưu tại Windhawk không đặt lại các công
tắc widget. Khi Windhawk 2 có CLI phù hợp và quyền ghi, ba mục chung được đồng bộ
ngược; Windhawk 1.x hoặc CLI bị từ chối thì chỉ lưu trong widget và báo rõ giới hạn.
Không tự nâng quyền hoặc sửa registry/INI của engine.

Chỉ báo quyền riêng tư tách đúng từng mục trong tooltip Windows, không lặp tên ứng
dụng hoặc đưa dòng Micro vào Vị trí. **Tắt điểm phát sóng** thực hiện yêu cầu tắt qua
Windows, có trạng thái đang tắt và báo lỗi. Không có hoạt động vẫn giữ bánh răng nhỏ
để mở lại cài đặt.

**Pin Bluetooth** nằm trong widget Pin: tên thiết bị, phần trăm do Windows báo và
preview khi kết nối / pin 20% / 10%. Không giả mức pin khi thiết bị không cung cấp.
**Điểm phát sóng** có danh sách thiết bị, tên khi có, địa chỉ IP/MAC.
Lịch ngày bỏ chiều cao tối thiểu thừa của ô ngày lễ. Tên ứng dụng được hiển thị dễ đọc.

Phương tiện có ảnh bìa, tên bài và dòng `thời gian · nghệ sĩ`; cụm nút Fluent có nút phát/tạm
 dừng lớn ở giữa. Thay đổi nhạc và nguồn điện được nhận theo sự kiện; việc tải ảnh
không chặn lệnh điều khiển. Bật nhạc/cắm nguồn có thể tự chọn mục tương ứng.

Popup mở bằng tay không tự hết giờ; preview sạc/pin yếu mặc định 4 giây. Bấm hoặc
cuộn trong preview để giữ mở. Chiều cao theo nội dung từng trang. Mod không còn tự
vô hiệu blur/kính vì chế độ tiết kiệm pin; vẫn ưu tiên tương phản cao. Windows có thể
giới hạn hiệu ứng ở cấp compositor. Animation mở 300 ms,
đóng 240 ms. Âm lịch & Tử vi vẫn luân chuyển 15 giây và ghim ngày lễ.

Nút **Tháng** hiển thị cả Dương/Âm lịch và tên lễ ở dòng thứ ba. Bấm ngày để xem
chi tiết. Nút **Lễ** giữ danh sách lễ, kỷ niệm. **Văn hóa** mở trang đọc có nút quay lại.

**Phương tiện:** có thanh tua nếu trình phát hỗ trợ; thời gian ở hai bên cụm nút điều khiển.
Các ô số bên dưới chọn phiên media; chỉ một phiên thì ẩn hàng này. Phiên vừa bắt đầu
phát được ưu tiên. Không hiển thị tên ứng dụng phát nhạc. Thời gian sử dụng ứng dụng
của mục Pin có trang riêng và nút Quay lại.

**Quyền riêng tư:** theo dõi phiên thu âm của micro; camera/vị trí dùng hoạt động
Windows ghi nhận nên vẫn có giới hạn. Có thể tự mở popup 4 giây khi có hoạt động mới.

**Tiến trình:** chọn ứng dụng trong Settings, thêm tên `.exe` cách nhau bằng dấu `;`
nếu cần. Tên file/nút điều khiển được đọc thêm từ giao diện tải xuống hoặc hộp thoại
tệp nếu ứng dụng cung cấp qua UI Automation. Trình duyệt có thể cần mở trang Tải xuống
để các nút còn tồn tại. Nếu chưa có nguồn, bấm **Mở mục tải xuống** để mở giao diện tải của trình duyệt.
Nút chưa được nguồn hỗ trợ sẽ bị vô hiệu hóa; không giả tên file hoặc gửi lệnh mù.
Chỉ báo hoàn thành/hủy khi có bằng chứng tương ứng; nếu mới gửi được lệnh hủy thì ghi
**Đã yêu cầu hủy**. Không suy ra thành công chỉ vì thanh tiến trình biến mất.

**Văn hóa Việt:** chỉ tải khi mở, bỏ cache bài đã xem; rời trang/đóng popup thì giải
phóng nội dung và ảnh. Mở lại cần Internet. Vẫn cần RAM tạm để tải và hiển thị nội dung.

Các phần mạng, giải mã ảnh và kiểm tra tiến trình chạy ngoài luồng giao diện. Dữ liệu
và danh sách đều có giới hạn; thao tác mở bằng tay được ưu tiên trước popup tự động.
**Trình chiếu:** hiển thị màn hình ngoài đang hoạt động, kết nối Windows báo và chế
độ nhân đôi/mở rộng; có preview khi đổi kết nối. USB-C có thể hiện là DisplayPort;
không theo dõi truyền riêng tab Chrome. **Điểm phát sóng:** trạng thái bật/tắt, tên
mạng, số thiết bị và danh sách thiết bị khi Windows cung cấp; không tự đổi cấu hình mạng.

Trong Settings, mục **Giao diện · Kiểu thanh tiến trình** có Mảnh / Bo tròn / Chia
đoạn. Ghi chú hướng dẫn được chuyển khỏi nội dung live; thông báo lỗi cần thao tác
và trạng thái chưa có dữ liệu vẫn được giữ lại.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- language: auto
  $name: Language
  $name:vi-VN: Ngôn ngữ
  $description: Open the gear on the widget for all settings. These three fields are also imported when saved here.
  $description:vi-VN: Mở bánh răng trên widget để chỉnh toàn bộ cài đặt. Ba mục này cũng được áp dụng khi lưu tại đây.
  $options:
  - auto: System default (Auto)
  - en: English
  - vi: Tiếng Việt
- widgetWidth: 270
  $name: Compact widget width (DIP)
  $name:vi-VN: Chiều rộng widget thu gọn (DIP)
  $description: "Range: 140–480 DIP."
- expandedWidth: 540
  $name: Popup width (DIP)
  $name:vi-VN: Chiều rộng popup (DIP)
  $description: "Range: 320–640 DIP."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <appmodel.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <d2d1effects.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <uiautomationclient.h>
#include <uiautomationcoreapi.h>
#include <shellapi.h>
#include <propsys.h>
#include <propkey.h>
#include <intrin.h>
#include <cstring>
#include <wincodec.h>
#include <powrprof.h>
#include <array>
#include <limits>
#include <memory>
#include <tuple>
#include <stdexcept>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cwctype>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>


#undef GetCurrentTime

#include <unknwn.h>
#include <mmdeviceapi.h>
#include <mfapi.h>
#include <mfidl.h>
#include <audiopolicy.h>
#include <tlhelp32.h>
#include <map>
#include <set>
#include <inspectable.h>
#include <setupapi.h>
#include <devpkey.h>
#include <psapi.h>
#include <sddl.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Networking.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Devices.Power.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.System.h>
// Button.Click is inherited from ButtonBase; its projected definition is here.
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <limits>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.Networking.NetworkOperators.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Security.Cryptography.Core.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.Headers.h>

namespace wf = winrt::Windows::Foundation;
namespace wcomp = winrt::Windows::UI::Composition;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

std::atomic<bool> g_shuttingDown{false};


////////////////////////////////////////////////////////////////////////////////
// Settings & Localization

enum class ModLanguage { Auto, English, Vietnamese };

bool GlassCpuAllowed(std::string_view vendor, unsigned family, unsigned model, std::string brand) {
    for (auto& ch : brand) if (ch >= 'A' && ch <= 'Z') ch += 'a' - 'A';
    if (vendor == "AuthenticAMD") return family == 0x17 || family == 0x19 || family == 0x1A;
    if (vendor != "GenuineIntel") return false;
    if (brand.find("core") != std::string::npos) {
        if (brand.find("ultra") != std::string::npos) return true;
        for (auto marker : {"i3-", "i5-", "i7-", "i9-"}) {
            auto at = brand.find(marker); if (at == std::string::npos) continue;
            at += 3; size_t end = at;
            while (end < brand.size() && brand[end] >= '0' && brand[end] <= '9') ++end;
            auto digits = brand.substr(at, end - at);
            if (digits.empty()) return false;
            if (digits.size() == 3) return false; // First-generation Core i.
            if (digits.size() == 1) return digits[0] >= '7'; // e.g. i7-7Y75 / i7-6Y75.
            unsigned generation = digits[0] - '0';
            if (digits.size() >= 5 || (digits.size() == 4 && end < brand.size() && brand[end] == 'g'))
                generation = generation * 10 + digits[1] - '0';
            return generation >= 7;
        }
    }
    if (family != 6) return false;
    // Recognized post-Skylake client models; unknown CPUs use the blur fallback.
    switch (model) {
        case 0x8E: case 0x9E: case 0x66: case 0x7D: case 0x7E:
        case 0xA5: case 0xA6: case 0x8C: case 0x8D: case 0xA7:
        case 0x97: case 0x9A: case 0xB7: case 0xBA: case 0xBF:
        case 0xAA: case 0xAC: case 0xC5: case 0xC6: case 0xBD: return true;
        default: return false;
    }
}
bool DetectGlassCpuSupport() {
#if defined(_M_X64) || defined(__x86_64__)
    int info[4]{}; __cpuid(info,0);
    if (info[0] < 1) return false;
    char vendor[13]{};std::memcpy(vendor,&info[1],4);std::memcpy(vendor+4,&info[3],4);std::memcpy(vendor+8,&info[2],4);
    __cpuid(info,1);unsigned signature=static_cast<unsigned>(info[0]);
    unsigned baseFamily=(signature>>8)&15, family=baseFamily, model=(signature>>4)&15;
    if(baseFamily==15)family+=(signature>>20)&255;
    if(baseFamily==6 || baseFamily==15)model|=((signature>>16)&15)<<4;
    char brand[49]{};__cpuid(info,static_cast<int>(0x80000000u));
    if(static_cast<unsigned>(info[0])>=0x80000004u)for(unsigned i=0;i<3;++i){__cpuid(info,static_cast<int>(0x80000002u+i));std::memcpy(brand+i*16,info,16);}
    return GlassCpuAllowed(vendor,family,model,brand);
#else
    return false;
#endif
}
bool g_glassCpuSupported = false;
bool SupportedTaskbarBuild(unsigned major,unsigned build) {return major==10 && build>=22000;}
bool HasSupportedWindowsVersion() {
    using RtlGetVersion_t=LONG(WINAPI*)(OSVERSIONINFOW*);
    auto getVersion=reinterpret_cast<RtlGetVersion_t>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"RtlGetVersion"));
    OSVERSIONINFOW version{};version.dwOSVersionInfoSize=sizeof(version);
    if (!getVersion || getVersion(&version)!=0) return false;
    return SupportedTaskbarBuild(version.dwMajorVersion,version.dwBuildNumber);
}

struct ModSettings {
    ModLanguage language = ModLanguage::Auto;
    bool enableTaskbarWidget = true;
    std::wstring position = L"tray_left";
    int widgetLeftOffset = 12;
    int widgetWidth = 270;
    int expandedWidth = 540;
    std::wstring progressStyle = L"capsule";
    std::wstring popupTheme = L"glass";
    bool bluetoothBattery = true;
    bool memoryAlerts = true, temperatureAlerts = true;
    int temperaturePort = 8085;
    bool collapsedTaskbar = false, settingsBlur = true, remoteAlerts = true;
    bool browserDownloadNames = true;
    bool enableDisplayWidget = true, displayAutoPopup = true;
    bool enableHotspotWidget = true, hotspotAutoPopup = true;
    bool enableLunarWidget = true;
    bool autoSelectActivity = true;
    bool enablePrivacyWidget = true;
    bool privacyAutoPopup = true;
    bool enableTaskProgress = true;
    bool taskProgressAutoPopup = true;
    bool progressExplorer = true, progressChrome = true, progressEdge = true;
    bool progressFirefox = false, progressSevenZip = false;
    std::wstring progressOtherApps;
    bool enableCultureWidget = true;
    std::wstring cultureCatalogUrl;
    bool enableClockWidget = true, clockAutoPopup = true;
    bool enableMediaWidget = true;
    bool enableObsWidget = true;
    int obsPort = 4455;
    std::wstring obsPassword;
    bool enableBatteryWidget = true;
    bool alwaysShowBattery = false;
    bool batteryAutoPopup = true;
    int batteryAutoPopupSeconds = 4;
};

std::mutex g_settingsMutex;
ModSettings g_settings;

template <typename T>
T ClampVal(T val, T minVal, T maxVal) {
    return std::max(minVal, std::min(maxVal, val));
}

bool ResolveLunarEnabled(int preference, bool vietnamRegion) {
    return preference < 0 ? vietnamRegion : preference != 0;
}
enum class WidgetSettingType { Toggle, Number, Choice, Text, Password };
struct WidgetSettingSpec {
    PCWSTR key; int section; WidgetSettingType type;
    PCWSTR en; PCWSTR vi; PCWSTR defaultValue;
    int minimum, maximum;
};
constexpr WidgetSettingSpec kWidgetSettings[] = {
    {L"collapsedTaskbar", 1, WidgetSettingType::Toggle, L"Collapsed taskbar · activity count", L"Thu gọn taskbar · số hoạt động", L"0", 0, 1},
    {L"settingsBlur", 1, WidgetSettingType::Toggle, L"Windows blur in settings", L"Làm mờ Windows trong cài đặt", L"1", 0, 1},
    {L"remoteAlerts", 2, WidgetSettingType::Toggle, L"UltraViewer connection alerts", L"Cảnh báo kết nối UltraViewer", L"1", 0, 1},
    {L"language", 0, WidgetSettingType::Choice, L"Language", L"Ngôn ngữ", L"auto", 0, 0},
    {L"enableTaskbarWidget", 0, WidgetSettingType::Toggle, L"Enable Live Update", L"Bật Live Update", L"1", 0, 1},
    {L"autoSelectActivity", 0, WidgetSettingType::Toggle, L"Follow new activities", L"Tự chọn hoạt động mới", L"1", 0, 1},
    {L"batteryAutoPopupSeconds", 0, WidgetSettingType::Number, L"Preview duration (seconds)", L"Thời gian preview (giây)", L"4", 3, 5},
    {L"position", 1, WidgetSettingType::Choice, L"Taskbar position", L"Vị trí trên taskbar", L"tray_left", 0, 0},
    {L"widgetWidth", 1, WidgetSettingType::Number, L"Compact width (DIP)", L"Chiều rộng thu gọn (DIP)", L"270", 140, 480},
    {L"expandedWidth", 1, WidgetSettingType::Number, L"Popup width (DIP)", L"Chiều rộng popup (DIP)", L"540", 320, 640},
    {L"widgetLeftOffset", 1, WidgetSettingType::Number, L"Left overlay offset (DIP)", L"Lề trái khi đặt đè (DIP)", L"12", 0, 2500},
    {L"popupTheme", 1, WidgetSettingType::Choice, L"Surface theme", L"Giao diện bề mặt", L"glass", 0, 0},
    {L"progressStyle", 1, WidgetSettingType::Choice, L"Progress bar style", L"Kiểu thanh tiến trình", L"capsule", 0, 0},
    {L"enableClockWidget", 2, WidgetSettingType::Toggle, L"Windows Clock · Stopwatch, timers & alarms", L"Đồng hồ Windows · Bấm giờ, hẹn giờ & báo thức", L"1", 0, 1},
    {L"clockAutoPopup", 2, WidgetSettingType::Toggle, L"Clock · Previews & 15-minute alarm reminder", L"Đồng hồ · Preview & nhắc báo thức trước 15 phút", L"1", 0, 1},
    {L"enableMediaWidget", 2, WidgetSettingType::Toggle, L"Media", L"Phương tiện", L"1", 0, 1},
    {L"enableBatteryWidget", 2, WidgetSettingType::Toggle, L"Battery", L"Pin", L"1", 0, 1},
    {L"alwaysShowBattery", 2, WidgetSettingType::Toggle, L"Battery · Always visible", L"Pin · Luôn hiển thị", L"0", 0, 1},
    {L"batteryAutoPopup", 2, WidgetSettingType::Toggle, L"Battery · Automatic preview", L"Pin · Tự mở preview", L"1", 0, 1},
    {L"bluetoothBattery", 2, WidgetSettingType::Toggle, L"Battery · Bluetooth devices", L"Pin · Thiết bị Bluetooth", L"1", 0, 1},
    {L"enablePrivacyWidget", 2, WidgetSettingType::Toggle, L"Privacy activity", L"Quyền riêng tư", L"1", 0, 1},
    {L"privacyAutoPopup", 2, WidgetSettingType::Toggle, L"Privacy · Automatic preview", L"Quyền riêng tư · Tự mở preview", L"1", 0, 1},
    {L"display.enabled", 2, WidgetSettingType::Toggle, L"Displays & projection", L"Màn hình & Trình chiếu", L"1", 0, 1},
    {L"display.autoPopup", 2, WidgetSettingType::Toggle, L"Displays · Automatic preview", L"Trình chiếu · Tự mở preview", L"1", 0, 1},
    {L"hotspot.enabled", 2, WidgetSettingType::Toggle, L"Wi-Fi hotspot", L"Điểm phát sóng Wi-Fi", L"1", 0, 1},
    {L"hotspot.autoPopup", 2, WidgetSettingType::Toggle, L"Hotspot · Automatic preview", L"Điểm phát sóng · Tự mở preview", L"1", 0, 1},
    {L"enableTaskProgress", 3, WidgetSettingType::Toggle, L"Task progress", L"Tiến trình tác vụ", L"1", 0, 1},
    {L"taskProgressAutoPopup", 3, WidgetSettingType::Toggle, L"Automatic preview", L"Tự mở preview", L"1", 0, 1},
    {L"progressExplorer", 3, WidgetSettingType::Toggle, L"File Explorer", L"File Explorer", L"1", 0, 1},
    {L"progressChrome", 3, WidgetSettingType::Toggle, L"Google Chrome", L"Google Chrome", L"1", 0, 1},
    {L"progressEdge", 3, WidgetSettingType::Toggle, L"Microsoft Edge", L"Microsoft Edge", L"1", 0, 1},
    {L"progressFirefox", 3, WidgetSettingType::Toggle, L"Mozilla Firefox", L"Mozilla Firefox", L"0", 0, 1},
    {L"progressSevenZip", 3, WidgetSettingType::Toggle, L"7-Zip", L"7-Zip", L"0", 0, 1},
    {L"progressOtherApps", 3, WidgetSettingType::Text, L"Additional applications (.exe; .exe)", L"Ứng dụng khác (.exe; .exe)", L"", 0, 4096},
    {L"browserDownloadNames", 3, WidgetSettingType::Toggle, L"Per-download progress details", L"Tiến độ từng lượt tải", L"1", 0, 1},
    {L"temperatureAlerts", 6, WidgetSettingType::Toggle, L"CPU/GPU alerts at 70, 80, 90 °C", L"Cảnh báo CPU/GPU ở 70, 80, 90 °C", L"1", 0, 1},
    {L"temperaturePort", 6, WidgetSettingType::Number, L"Local sensor server port", L"Cổng máy chủ cảm biến cục bộ", L"8085", 1, 65535},
    {L"memoryAlerts", 6, WidgetSettingType::Toggle, L"RAM alerts at 80%, 90%, 95%", L"Cảnh báo RAM ở 80%, 90%, 95%", L"1", 0, 1},
    {L"enableObsWidget", 4, WidgetSettingType::Toggle, L"OBS recording", L"Quay màn hình OBS", L"1", 0, 1},
    {L"obsPort", 4, WidgetSettingType::Number, L"WebSocket port", L"Cổng WebSocket", L"4455", 1024, 65535},
    {L"obsPassword", 4, WidgetSettingType::Password, L"WebSocket password", L"Mật khẩu WebSocket", L"", 0, 1024},
    {L"enableLunarWidget", 5, WidgetSettingType::Choice, L"Lunar calendar & Almanac", L"Âm lịch & Tử vi", L"-1", -1, 1},
    {L"enableCultureWidget", 5, WidgetSettingType::Toggle, L"Vietnamese culture reader", L"Mục Văn hóa Việt", L"1", 0, 1},
    {L"cultureCatalogUrl", 5, WidgetSettingType::Text, L"Custom culture catalog (HTTPS)", L"Danh mục văn hóa riêng (HTTPS)", L"", 0, 2048},
};

struct SettingOption { PCWSTR value, en, vi; };
std::vector<SettingOption> WidgetSettingOptions(std::wstring_view key) {
    if (key == L"language") return {{L"auto", L"System default", L"Theo hệ thống"}, {L"vi", L"Vietnamese", L"Tiếng Việt"}, {L"en", L"English", L"English"}};
    if (key == L"position") return {{L"tray_left", L"Tray · Leftmost", L"Khay hệ thống · Bên trái"},
        {L"tray_before_clock", L"Tray · Before clock", L"Khay hệ thống · Trước đồng hồ"},
        {L"tray_after_clock", L"Tray · After clock", L"Khay hệ thống · Sau đồng hồ"},
        {L"tray_right", L"Tray · Rightmost", L"Khay hệ thống · Bên phải"},
        {L"root_left", L"Left overlay", L"Đè lên góc trái taskbar"}};
    if (key == L"popupTheme") return {{L"blur", L"Windows blur", L"Làm mờ Windows"}, {L"glass", L"Liquid glass", L"Liquid glass"}, {L"solid", L"Off (opaque)", L"Tắt (nền đặc)"}};
    if (key == L"progressStyle") return {{L"thin", L"Thin", L"Mảnh"}, {L"capsule", L"Rounded", L"Bo tròn"}, {L"segments", L"Segmented", L"Chia đoạn"}};
    if (key == L"enableLunarWidget") return {{L"-1", L"Auto · Vietnam region", L"Tự động · Khu vực Việt Nam"}, {L"1", L"On", L"Bật"}, {L"0", L"Off", L"Tắt"}};
    return {};
}
bool SharedHostSetting(std::wstring_view key) {
    return key == L"language" || key == L"widgetWidth" || key == L"expandedWidth";
}
std::wstring ReadStoredText(std::wstring const& key, PCWSTR fallback = L"") {
    WCHAR buffer[4098]{};
    return Wh_GetStringValue(key.c_str(), buffer, ARRAYSIZE(buffer)) ? std::wstring(buffer) : fallback;
}
std::wstring HostSettingValue(WidgetSettingSpec const& spec) {
    if (spec.type == WidgetSettingType::Toggle || spec.type == WidgetSettingType::Number ||
        std::wstring_view(spec.key) == L"enableLunarWidget") return std::to_wstring(Wh_GetIntSetting(spec.key));
    return WindhawkUtils::StringSetting::make(spec.key).get();
}
bool ValidateWidgetSetting(WidgetSettingSpec const& spec, std::wstring const& value) {
    if (spec.type == WidgetSettingType::Toggle) return value == L"0" || value == L"1";
    if (spec.type == WidgetSettingType::Choice) {
        auto options = WidgetSettingOptions(spec.key);
        return std::any_of(options.begin(), options.end(), [&](auto const& option) { return value == option.value; });
    }
    if (spec.type == WidgetSettingType::Number) {
        if (value.empty() || value.size() > 8 || value.find_first_not_of(L"0123456789") != std::wstring::npos) return false;
        auto number = wcstol(value.c_str(), nullptr, 10);
        return number >= spec.minimum && number <= spec.maximum;
    }
    if (value.size() > static_cast<size_t>(spec.maximum)) return false;
    if (std::wstring_view(spec.key) == L"cultureCatalogUrl" && !value.empty()) return value.starts_with(L"https://");
    return true;
}
std::wstring WidgetSettingValue(WidgetSettingSpec const& spec) {
    auto value = ReadStoredText(std::wstring(L"config.") + spec.key, spec.defaultValue);
    return ValidateWidgetSetting(spec, value) ? value : spec.defaultValue;
}
// One canonical store for widget preferences. Only the three shared host fields
// are imported; saving Windhawk settings never resets the activity switches.
std::mutex g_configStorageMutex;
std::atomic<bool> g_hostSyncInFlight{false};
void ImportHostSettings(bool force) {
    std::lock_guard<std::mutex> lock(g_configStorageMutex);
    bool first = !Wh_GetIntValue(L"config.schema28", 0);
    bool legacy = first && Wh_GetIntSetting(L"obsPort") >= 1024;
    bool saved = true;
    for (auto const& spec : kWidgetSettings) {
        std::wstring key = std::wstring(L"config.") + spec.key;
        if (first) {
            auto value = legacy ? HostSettingValue(spec) : std::wstring(spec.defaultValue);
            if (!ValidateWidgetSetting(spec, value)) value = spec.defaultValue;
            int quick = Wh_GetIntValue((std::wstring(L"quick.") + spec.key).c_str(), -1);
            if (quick >= 0 && (spec.type == WidgetSettingType::Toggle || std::wstring_view(spec.key) == L"enableLunarWidget"))
                value = quick ? L"1" : L"0";
            if (std::wstring_view(spec.key) == L"popupTheme" && quick >= 0 && quick < 3)
                value = quick == 0 ? L"blur" : quick == 1 ? L"glass" : L"solid";
            saved = Wh_SetStringValue(key.c_str(), value.c_str()) && saved;
        }
        if (!SharedHostSetting(spec.key)) continue;
        auto host = HostSettingValue(spec);
        if (!ValidateWidgetSetting(spec, host)) host = spec.defaultValue;
        auto baseline = ReadStoredText(std::wstring(L"host.") + spec.key, L"<unset>");
        if (first || force || host != baseline) {
            if (!g_hostSyncInFlight) saved = Wh_SetStringValue(key.c_str(), host.c_str()) && saved;
            saved = Wh_SetStringValue((std::wstring(L"host.") + spec.key).c_str(), host.c_str()) && saved;
        }
    }
    if (first && saved) Wh_SetIntValue(L"config.schema28", 1);
}
void LoadSettings(bool importHost = false) {
    ImportHostSettings(importHost);
    ModSettings s;
    auto value = [](PCWSTR key) -> std::wstring {
        for (auto const& spec : kWidgetSettings) if (!wcscmp(spec.key, key)) return WidgetSettingValue(spec);
        return {};
    };
    auto flag = [&](PCWSTR key) { return value(key) == L"1"; };
    auto number = [&](PCWSTR key) { return static_cast<int>(wcstol(value(key).c_str(), nullptr, 10)); };
    auto lang = value(L"language");
    s.language = lang == L"vi" ? ModLanguage::Vietnamese : lang == L"en" ? ModLanguage::English : ModLanguage::Auto;
    WCHAR region[16]{};
    bool vietnamRegion = GetUserDefaultGeoName(region, ARRAYSIZE(region)) > 0 && !_wcsicmp(region, L"VN");
    s.enableLunarWidget = ResolveLunarEnabled(number(L"enableLunarWidget"), vietnamRegion);

    s.enableTaskbarWidget = flag(L"enableTaskbarWidget");
    s.autoSelectActivity = flag(L"autoSelectActivity");
    s.batteryAutoPopupSeconds = number(L"batteryAutoPopupSeconds");
    s.position = value(L"position");
    s.widgetWidth = number(L"widgetWidth");
    s.expandedWidth = number(L"expandedWidth");
    s.widgetLeftOffset = number(L"widgetLeftOffset");
    s.popupTheme = value(L"popupTheme");
    s.progressStyle = value(L"progressStyle");
    s.enableMediaWidget = flag(L"enableMediaWidget");
    s.enableBatteryWidget = flag(L"enableBatteryWidget");
    s.alwaysShowBattery = flag(L"alwaysShowBattery");
    s.batteryAutoPopup = flag(L"batteryAutoPopup");
    s.bluetoothBattery = flag(L"bluetoothBattery");
    s.memoryAlerts = flag(L"memoryAlerts");
    s.temperatureAlerts = flag(L"temperatureAlerts"); s.temperaturePort = number(L"temperaturePort");
    s.collapsedTaskbar = flag(L"collapsedTaskbar");
    s.settingsBlur = flag(L"settingsBlur");
    s.remoteAlerts = flag(L"remoteAlerts");
    s.browserDownloadNames = flag(L"browserDownloadNames");
    s.enablePrivacyWidget = flag(L"enablePrivacyWidget");
    s.privacyAutoPopup = flag(L"privacyAutoPopup");
    s.enableDisplayWidget = flag(L"display.enabled");
    s.displayAutoPopup = flag(L"display.autoPopup");
    s.enableHotspotWidget = flag(L"hotspot.enabled");
    s.hotspotAutoPopup = flag(L"hotspot.autoPopup");
    s.enableTaskProgress = flag(L"enableTaskProgress");
    s.taskProgressAutoPopup = flag(L"taskProgressAutoPopup");
    s.progressExplorer = flag(L"progressExplorer");
    s.progressChrome = flag(L"progressChrome");
    s.progressEdge = flag(L"progressEdge");
    s.progressFirefox = flag(L"progressFirefox");
    s.progressSevenZip = flag(L"progressSevenZip");
    s.progressOtherApps = value(L"progressOtherApps");
    s.enableClockWidget = flag(L"enableClockWidget"); s.clockAutoPopup = flag(L"clockAutoPopup");
    s.enableObsWidget = flag(L"enableObsWidget");
    s.obsPort = number(L"obsPort");
    s.obsPassword = value(L"obsPassword");
    s.enableCultureWidget = flag(L"enableCultureWidget");
    s.cultureCatalogUrl = value(L"cultureCatalogUrl");
    std::lock_guard<std::mutex> lock(g_settingsMutex); g_settings = std::move(s);
}

ModSettings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

bool UseVietnameseLabels(ModSettings const& settings) {
    if (settings.language == ModLanguage::Vietnamese) return true;
    if (settings.language == ModLanguage::English) return false;
    LANGID langId = GetUserDefaultUILanguage();
    return PRIMARYLANGID(langId) == LANG_VIETNAMESE;
}

PCWSTR PickText(bool vi, PCWSTR viText, PCWSTR enText) {
    return vi ? viText : enText;
}

// Background providers own all their WinRT objects. The UI receives values only.
struct ImagePixels {
    uint32_t width = 0, height = 0;
    std::vector<uint8_t> bytes;
};
struct PrivacyUse {
    std::wstring capability, app;
    uint64_t started = 0;
    bool recent = false;
    bool operator==(PrivacyUse const&) const = default;
};
struct PrivacySnapshot {
    std::vector<PrivacyUse> uses;
    bool available = false;
};
constexpr UINT kLiveUpdateMessage = WM_APP + 0x451;
std::atomic<HWND> g_liveUiWindow{nullptr};
std::atomic<bool> g_liveUiQueued{false};
void NotifyLiveUi() {
    if (g_shuttingDown) return;
    HWND window = g_liveUiWindow.load();
    if (window && !g_liveUiQueued.exchange(true) &&
        !PostMessageW(window, kLiveUpdateMessage, 0, 0)) g_liveUiQueued = false;
}
HMODULE ThisModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<PCWSTR>(&ThisModule), &module);
    return module;
}

// Live providers publish value-only snapshots. No XAML object leaves the UI thread.
enum class ActivityKind { None, Lunar, Media, Battery, Privacy, Culture, Download, Obs, Display, Hotspot, Memory, Remote, Clock };
enum class MediaAction { None, PlayPause, Previous, Next, Seek };
enum class BatteryAlert { None, Charging, Low20, Low10, Low5 };
int64_t SeekTargetTicks(int64_t start, int64_t minimum, int64_t maximum, int64_t seconds) {
    long double target = static_cast<long double>(start) + static_cast<long double>(seconds) * 10000000.0L;
    return static_cast<int64_t>(std::clamp(target, static_cast<long double>(minimum), static_cast<long double>(maximum)));
}

struct MediaSnapshot {
    bool available = false;
    bool playing = false;
    bool canToggle = false;
    bool canPrevious = false;
    bool canNext = false, canSeek = false;
    int64_t seekMin = 0, seekMax = 0;
    uint64_t sessionId = 0;
    uint64_t playbackSerial = 0;
    uint64_t generation = 0;
    std::wstring title, artist, app;
    bool commandPending = false, commandFailed = false;
    std::shared_ptr<ImagePixels> artwork;
    bool operator==(MediaSnapshot const&) const = default;
    int64_t positionSeconds = 0;
    int64_t durationSeconds = 0;
};

struct MediaSessionChoice {
    uint64_t id = 0;
    std::wstring app;
    bool playing = false;
    bool operator==(MediaSessionChoice const&) const = default;
};
struct MediaCandidate {
    uint64_t id;
    bool playing;
    uint64_t started;
};
uint64_t ChooseMediaSession(std::vector<MediaCandidate> const& candidates, uint64_t systemCurrent,
                            uint64_t requested, uint64_t requestedSerial) {
    if (candidates.empty()) return 0;
    auto current = std::find_if(candidates.begin(), candidates.end(), [&](auto const& item) { return item.id == systemCurrent; });
    if (current == candidates.end()) current = candidates.begin();
    auto latest = candidates.end();
    for (auto it = candidates.begin(); it != candidates.end(); ++it)
        if (it->playing && (latest == candidates.end() || it->started > latest->started)) latest = it;
    auto manual = std::find_if(candidates.begin(), candidates.end(), [&](auto const& item) { return item.id == requested; });
    if (manual != candidates.end() && (latest == candidates.end() || latest->started <= requestedSerial)) return manual->id;
    if (latest != candidates.end() && (latest->started > 0 || !current->playing)) return latest->id;
    return current->id;
}
enum class TaskOperation { Generic, Download, Files, Copy, Move, Delete, Archive };
enum class TaskAction { None, Pause, Resume, Cancel };
struct TaskFileInfo {
    std::wstring filename;
    unsigned controls = 0;
    int percent = -1;
    bool operator==(TaskFileInfo const&) const = default;
};
struct TaskControlState {
    uintptr_t window = 0, sourceWindow = 0;
    uint64_t sampledAt = 0;
    unsigned controls = 0; // Observed labels only: pause=1, resume=2, cancel=4.
    TaskOperation operation = TaskOperation::Generic;
    std::wstring filename;
    std::vector<TaskFileInfo> files;
    bool operator==(TaskControlState const&) const = default;
};
struct TaskProgress {
    uintptr_t window = 0;
    uint64_t id = 0;
    std::wstring app, title;
    int percent = -1;
    unsigned state = 0;
    TaskControlState control;
    std::wstring fileKey;
    bool operator==(TaskProgress const&) const = default;
};
struct BatterySnapshot {
    bool valid = false;
    bool present = false;
    bool pluggedIn = false;
    bool charging = false;
    int percent = -1;
    int minutesToFull = -1;
    int minutesRemaining = -1;
    bool operator==(BatterySnapshot const&) const = default;
};

// Providers added later (for example a browser download bridge) feed this model
// and register their page in AvailableActivities; there is no notification reader.
struct ObsSnapshot {
    bool connected = false, recording = false, paused = false;
    bool commandPending = false, commandFailed = false;
    int64_t elapsedSeconds = 0, elapsedMilliseconds = 0;
    uint64_t sampledAt = 0;
    uint64_t generation = 0;
    uint64_t stoppedSerial = 0, stoppedUntil = 0;
    bool operator==(ObsSnapshot const&) const = default;
};
enum class ObsAction { None, Pause, Resume, Stop };
struct ObsCommand { ObsAction action = ObsAction::None; uint64_t generation = 0; };
struct DisplayTarget {
    std::wstring id, name;
    unsigned technology = 0;
    bool duplicate = false;
    bool operator==(DisplayTarget const&) const = default;
};
struct DisplaySnapshot {
    bool available = false, internalActive = false;
    unsigned sourceCount = 0;
    std::vector<DisplayTarget> targets;
    bool operator==(DisplaySnapshot const&) const = default;
};
struct HotspotClient {
    std::wstring mac, name, address;
    bool operator==(HotspotClient const&) const = default;
};
struct BluetoothDeviceBattery {
    std::wstring id, name;
    int percent = -1;
    bool operator==(BluetoothDeviceBattery const&) const = default;
};
struct BluetoothSnapshot {
    bool available = false;
    std::vector<BluetoothDeviceBattery> devices;
    bool operator==(BluetoothSnapshot const&) const = default;
};
struct HotspotSnapshot {
    bool available = false, on = false, changing = false;
    int clients = -1;
    bool commandPending = false, commandFailed = false;
    std::wstring ssid;
    bool clientsAvailable = false;
    std::vector<HotspotClient> devices;
    bool operator==(HotspotSnapshot const&) const = default;
};
enum class TaskOutcome { None, Completed, Cancelled };
struct TaskNotice {
    uint64_t serial = 0, expires = 0;
    std::wstring filename;
    TaskOutcome outcome = TaskOutcome::None;
};
struct MemoryApplication {
    std::wstring name;
    uint64_t workingSet = 0, privateBytes = 0;
    unsigned processes = 0;
};
struct MemorySnapshot {
    bool valid = false;
    unsigned percent = 0;
    uint64_t total = 0, available = 0, explorerWorkingSet = 0;
    uint64_t settingsWorkingSet = 0, sampledAt = 0;
    std::vector<MemoryApplication> top;
};
struct MemoryAlerts {
    unsigned fired = 0;
    int Observe(unsigned percent) {
        int level = 0;
        constexpr unsigned levels[] = {80, 90, 95};
        for (unsigned i = 0; i < 3; ++i) {
            if (percent + 3 < levels[i]) fired &= ~(1u << i);
            if (percent >= levels[i] && !(fired & (1u << i))) { fired |= 1u << i; level = levels[i]; }
        }
        return level;
    }
};
struct TemperatureSnapshot {
    int cpu = -1, gpu = -1;
    bool operator==(TemperatureSnapshot const&) const = default;
};
struct TemperatureAlerts {
    unsigned fired = 0;
    int Observe(int value) {
        if (value < 0) return 0;
        int level=0; constexpr int levels[]={70,80,90};
        for(unsigned i=0;i<3;++i) {
            if(value <= levels[i]-5) fired &= ~(1u<<i);
            if(value >= levels[i] && !(fired&(1u<<i))) {fired|=1u<<i;level=levels[i];}
        }
        return level;
    }
};
std::wstring TemperatureText(TemperatureSnapshot const& t, bool vi) {
    auto value=[&](int v){return v < 0 ? std::wstring(PickText(vi,L"Chưa có dữ liệu",L"Unavailable")) : std::to_wstring(v)+L" °C";};
    return L"CPU: "+value(t.cpu)+L" · GPU: "+value(t.gpu);
}
struct StorageVolume {
    std::wstring root, label;
    uint64_t total = 0, free = 0;
};
struct RemoteSnapshot {
    uint64_t serial = 0, observedAt = 0;
    bool incoming = false, active = false;
    std::vector<std::wstring> peers;
};
enum class ClockKind { Stopwatch, Timer, Alarm };
enum class ClockRunState { Unknown, Paused, Running };
enum class ClockNotice { None, Started, TimerFinished, AlarmSoon, AlarmDue };
struct ClockEntry {
    std::wstring key,label,sourceKey;
    ClockKind kind=ClockKind::Stopwatch;
    ClockRunState run=ClockRunState::Unknown;
    int64_t milliseconds=0,alarmUtc=0,sourceMilliseconds=0;
    ClockRunState sourceRun=ClockRunState::Unknown;
    // sampledAt anchors the counter; observedAt records actual source access.
    uint64_t sampledAt=0,observedAt=0;
    int64_t remindedAlarmUtc=0,announcedAlarmUtc=0;
    bool finished=false,finishedEstimated=false,enabled=true;
    bool operator==(ClockEntry const&) const=default;
};
struct ClockSnapshot {
    uintptr_t window=0;
    std::vector<ClockEntry> entries;
    uint64_t sampledAt=0,activitySerial=0,noticeSerial=0,noticeUntil=0;
    ClockNotice notice=ClockNotice::None;
    std::wstring noticeLabel,noticeKey;
    bool noticeEstimated=false;
};
bool ClockEntryFresh(ClockEntry const& entry,uint64_t now) {
    if(!entry.observedAt || now<entry.observedAt || now-entry.observedAt>4000)return false;
    // A visible provider can also freeze its readout. Do not call that live data.
    return entry.run!=ClockRunState::Running || (now>=entry.sampledAt && now-entry.sampledAt<=4000);
}
int64_t ClockReading(ClockEntry const& entry,uint64_t now) {
    uint64_t delta=now>=entry.sampledAt?now-entry.sampledAt:0;
    if(entry.run!=ClockRunState::Running || entry.kind==ClockKind::Alarm)return entry.milliseconds;
    // GetTickCount64 includes sleep and is unaffected by wall-clock adjustments.
    // Keep the last confirmed running anchor while the source page is unavailable.
    delta=std::min<uint64_t>(delta,static_cast<uint64_t>(INT64_MAX-std::max<int64_t>(0,entry.milliseconds)));
    return entry.kind==ClockKind::Timer?std::max<int64_t>(0,entry.milliseconds-static_cast<int64_t>(delta)):entry.milliseconds+static_cast<int64_t>(delta);
}
int64_t ClockDisplaySeconds(ClockEntry const& entry,uint64_t now) {
    auto reading=std::max<int64_t>(0,ClockReading(entry,now));
    return reading/1000+(entry.kind==ClockKind::Timer && reading%1000?1:0);
}
struct ParsedClockTime {int hours=0,minutes=0,seconds=0,milliseconds=0;bool secondsPresent=false;};
bool ParseClockTime(std::wstring text,ParsedClockTime& result) {
    std::erase_if(text,[](wchar_t c){return c==0x200e || c==0x200f || (c>=0x2066 && c<=0x2069) || (c>=0x202a && c<=0x202e);});
    for(auto& c:text){if(c==L'：')c=L':';if(c==0xa0 || c==0x202f)c=L' ';}
    size_t colon=text.find(L':');if(colon==std::wstring::npos || !colon)return false;
    size_t start=colon;
    while(start && (iswspace(text[start-1]) || text[start-1]==0x200e || text[start-1]==0x200f))--start;
    while(start && text[start-1]>=L'0' && text[start-1]<=L'9')--start;
    if(start==colon || text[start]<L'0' || text[start]>L'9')return false;
    size_t at=start;int values[3]{};int count=0;
    while(count<3) {
        if(at>=text.size() || text[at]<L'0' || text[at]>L'9')return false;
        int value=0,digits=0;
        while(at<text.size() && text[at]>=L'0' && text[at]<=L'9') {if(++digits>3)return false;value=value*10+text[at++]-L'0';}
        values[count++]=value;
        while(at<text.size() && (iswspace(text[at]) || text[at]==0x200e || text[at]==0x200f))++at;
        if(at>=text.size() || text[at]!=L':')break;
        if(count==3)return false;
        ++at;while(at<text.size() && iswspace(text[at]))++at;
    }
    if(count<2 || values[1]>=60 || (count==3 && values[2]>=60))return false;
    result={values[0],values[1],count==3?values[2]:0,0,count==3};
    if(at+1<text.size() && (text[at]==L'.' || text[at]==L',') && text[at+1]>=L'0' && text[at+1]<=L'9') {
        ++at;int place=100,digits=0;
        while(at<text.size() && text[at]>=L'0' && text[at]<=L'9'){
            if(place){result.milliseconds+=(text[at]-L'0')*place;place/=10;}++at;++digits;
        }
        if(!digits)return false;
    }
    return true;
}
int64_t ClockDurationMs(ParsedClockTime const& t) {
    return (t.secondsPresent?(int64_t(t.hours)*3600+t.minutes*60+t.seconds):(int64_t(t.hours)*60+t.minutes))*1000+t.milliseconds;
}
bool AlarmInReminderWindow(int64_t due,int64_t now) {return due>now && due-now<=15*60*1000;}

// Worker-owned plain values. No UI objects, app handles or polling timers are retained.
struct ClockTracker {
    ClockSnapshot state;
    uint64_t nextIdentity=0;

    void Notice(ClockNotice type,ClockEntry const& entry,uint64_t now,bool estimated=false) {
        state.notice=type;state.noticeLabel=entry.label;state.noticeKey=entry.key;
        state.noticeEstimated=estimated;++state.noticeSerial;state.noticeUntil=now+4000;
    }

    void Update(std::vector<ClockEntry> observations,uint64_t now,int64_t utc,bool enabled) {
        if(!enabled) {
            state.entries.clear();state.sampledAt=0;state.notice=ClockNotice::None;
            state.noticeUntil=0;state.noticeLabel.clear();state.noticeKey.clear();
            return;
        }
        if(!observations.empty())state.sampledAt=now;
        std::vector<std::wstring> used;
        for(auto entry:observations) {
            auto sourceKey=entry.key;
            auto old=std::find_if(state.entries.begin(),state.entries.end(),[&](auto const& e) {
                return e.sourceKey==sourceKey && e.kind==entry.kind;
            });
            // Runtime IDs can change when the page/app is recreated. Only rematch
            // unambiguous labels; two same-name timers must never become one timer.
            if(old==state.entries.end()) {
                auto same=[&](auto const& e){return e.kind==entry.kind && e.label==entry.label;};
                if(std::count_if(observations.begin(),observations.end(),same)==1 &&
                    std::count_if(state.entries.begin(),state.entries.end(),same)==1)
                    old=std::find_if(state.entries.begin(),state.entries.end(),same);
            }
            if(old!=state.entries.end() && std::find(used.begin(),used.end(),old->key)!=used.end())continue;
            if(!entry.enabled) {
                if(old!=state.entries.end())state.entries.erase(old);
                continue;
            }
            const bool previous=old!=state.entries.end();
            entry.key=previous?old->key:L"clock:"+std::to_wstring(++nextIdentity);
            entry.sourceKey=sourceKey;entry.observedAt=now;
            entry.sourceMilliseconds=entry.milliseconds;entry.sourceRun=entry.run;
            used.push_back(entry.key);
            if(previous && old->finishedEstimated && entry.kind==ClockKind::Timer && entry.milliseconds>0 &&
                entry.milliseconds==old->sourceMilliseconds && entry.run==old->sourceRun) {
                // A frozen provider must not restart a completed local estimate
                // indefinitely. A changed value/state will resynchronize it.
                old->sourceKey=sourceKey;old->observedAt=now;
                continue;
            }
            if(entry.kind==ClockKind::Alarm) {
                if(previous) {
                    entry.remindedAlarmUtc=old->remindedAlarmUtc;
                    entry.announcedAlarmUtc=old->announcedAlarmUtc;
                }
            } else {
                if(previous && entry.run==ClockRunState::Unknown) {
                    const auto elapsed=now>=old->observedAt?now-old->observedAt:0;
                    const int64_t movement=entry.kind==ClockKind::Timer?
                        old->milliseconds-entry.milliseconds:entry.milliseconds-old->milliseconds;
                    if(elapsed<=4000 && movement>0 && movement<=static_cast<int64_t>(elapsed)+2000)
                        entry.run=ClockRunState::Running;
                    else if(old->run==ClockRunState::Running && entry.milliseconds==old->milliseconds)
                        entry.run=ClockRunState::Running;
                }
                if(previous && entry.run==ClockRunState::Running && old->run==entry.run &&
                    old->milliseconds==entry.milliseconds)entry.sampledAt=old->sampledAt;
                if(entry.kind==ClockKind::Timer && entry.milliseconds==0 && previous &&
                    (old->run==ClockRunState::Running || old->finished))entry.finished=true;
                if(entry.finished) {
                    entry.milliseconds=0;entry.run=ClockRunState::Paused;
                    if(previous && old->finished)entry.sampledAt=old->sampledAt;
                    else Notice(ClockNotice::TimerFinished,entry,now);
                } else if(entry.run==ClockRunState::Running && (!previous || old->run!=ClockRunState::Running)) {
                    ++state.activitySerial;Notice(ClockNotice::Started,entry,now);
                }
            }
            if(previous)*old=std::move(entry);else state.entries.push_back(std::move(entry));
        }

        // Only counters that were actually observed running advance in the background.
        // Missing/empty scans, page switches and closing the source app are not resets.
        for(auto& entry:state.entries) {
            if(entry.kind==ClockKind::Timer && !entry.finished && entry.run==ClockRunState::Running &&
                ClockReading(entry,now)==0) {
                entry.finishedEstimated=true;entry.finished=true;entry.run=ClockRunState::Paused;
                entry.milliseconds=0;entry.sampledAt=now;
                Notice(ClockNotice::TimerFinished,entry,now,true);
            } else if(entry.kind==ClockKind::Alarm) {
                if(AlarmInReminderWindow(entry.alarmUtc,utc) && entry.remindedAlarmUtc!=entry.alarmUtc) {
                    entry.remindedAlarmUtc=entry.alarmUtc;++state.activitySerial;
                    Notice(ClockNotice::AlarmSoon,entry,now,!ClockEntryFresh(entry,now));
                }
                if(entry.alarmUtc<=utc && utc-entry.alarmUtc<60000 && entry.announcedAlarmUtc!=entry.alarmUtc) {
                    entry.announcedAlarmUtc=entry.alarmUtc;
                    Notice(ClockNotice::AlarmDue,entry,now,!ClockEntryFresh(entry,now));
                }
            }
        }
        std::erase_if(state.entries,[&](auto const& entry) {
            return entry.kind==ClockKind::Alarm && entry.alarmUtc<utc-60000;
        });
        // Bounded session history: unseen inactive entries are evicted first.
        while(state.entries.size()>48) {
            auto oldest=std::min_element(state.entries.begin(),state.entries.end(),[](auto const& a,auto const& b) {
                if((a.run==ClockRunState::Running)!=(b.run==ClockRunState::Running))return a.run!=ClockRunState::Running;
                return a.observedAt<b.observedAt;
            });
            state.entries.erase(oldest);
        }
    }
};

struct LiveSnapshot {
    ClockSnapshot clock;
    MediaSnapshot media;
    BatterySnapshot battery;
    PrivacySnapshot privacy;
    std::vector<MediaSessionChoice> mediaSessions;
    std::vector<TaskProgress> tasks;
    ObsSnapshot obs;
    DisplaySnapshot display;
    HotspotSnapshot hotspot;
    BluetoothSnapshot bluetooth;
    TaskNotice taskNotice;
    MemorySnapshot memory;
    RemoteSnapshot remote;
    TemperatureSnapshot temperature;
};

struct BatteryAlertTracker {
    bool initialized = false;
    bool wasCharging = false;
    bool wasPlugged = false;
    unsigned alerted = 0;

    BatteryAlert Observe(BatterySnapshot const& b) {
        if (!b.valid || !b.present) return BatteryAlert::None;
        const bool startedCharging = initialized && !wasPlugged && b.pluggedIn;
        wasPlugged = b.pluggedIn;
        initialized = true;
        wasCharging = b.charging;
        constexpr int levels[] = {20, 10, 5};
        constexpr BatteryAlert alerts[] = {
            BatteryAlert::Low20, BatteryAlert::Low10, BatteryAlert::Low5};
        if (b.percent >= 0) {
            for (unsigned i = 0; i < 3; ++i) {
                // Hysteresis prevents 19/20/21% fluctuations from repeating alerts.
                if (b.percent >= levels[i] + 3) alerted &= ~(1u << i);
            }
        }
        if (startedCharging) return BatteryAlert::Charging;
        if (b.charging || b.pluggedIn || b.percent < 0) return BatteryAlert::None;
        BatteryAlert result = BatteryAlert::None;
        for (unsigned i = 0; i < 3; ++i) {
            if (b.percent <= levels[i] && !(alerted & (1u << i))) {
                alerted |= 1u << i;
                result = alerts[i]; // One popup, using the most urgent crossed level.
            }
        }
        return result;
    }
};

int EstimateChargeMinutes(int full, int remaining, double rateMilliwatts) {
    if (full <= 0 || remaining < 0 || remaining >= full ||
        !std::isfinite(rateMilliwatts) || rateMilliwatts <= 0) return -1;
    double minutes = (static_cast<double>(full) - remaining) * 60.0 / rateMilliwatts;
    if (minutes > 1440.0) return -1;
    return std::max(1, static_cast<int>(std::ceil(minutes)));
}

std::vector<ActivityKind> AvailableActivities(LiveSnapshot const& state, ModSettings const& settings,
        ActivityKind selected, bool keepSelected) {
    std::vector<ActivityKind> result;
    if (!settings.enableTaskbarWidget) return result;
    if (settings.enableLunarWidget) result.push_back(ActivityKind::Lunar);
    if (settings.enableMediaWidget && (state.media.available || (keepSelected && selected == ActivityKind::Media)))
        result.push_back(ActivityKind::Media);
    if(settings.enableClockWidget && (!state.clock.entries.empty() || GetTickCount64()<state.clock.noticeUntil || (keepSelected && selected==ActivityKind::Clock)))
        result.push_back(ActivityKind::Clock);
    auto const& b = state.battery;
    if (settings.enableBatteryWidget && ((b.valid && b.present && (settings.alwaysShowBattery || b.pluggedIn ||
        (b.percent >= 0 && b.percent <= 20))) || !state.bluetooth.devices.empty() || (keepSelected && selected == ActivityKind::Battery)))
        result.push_back(ActivityKind::Battery);
    if (settings.enablePrivacyWidget && (!state.privacy.uses.empty() || (keepSelected && selected == ActivityKind::Privacy)))
        result.push_back(ActivityKind::Privacy);
    if (settings.enableTaskProgress && (!state.tasks.empty() || (keepSelected && selected == ActivityKind::Download)))
        result.push_back(ActivityKind::Download);
    if (settings.enableObsWidget && (state.obs.recording || GetTickCount64() < state.obs.stoppedUntil || (keepSelected && selected == ActivityKind::Obs)))
        result.push_back(ActivityKind::Obs);

    if (settings.enableDisplayWidget && (!state.display.targets.empty() || (keepSelected && selected == ActivityKind::Display)))
        result.push_back(ActivityKind::Display);
    if (settings.enableHotspotWidget && (state.hotspot.on || (keepSelected && selected == ActivityKind::Hotspot)))
        result.push_back(ActivityKind::Hotspot);
    if (settings.remoteAlerts && state.remote.serial &&
        (state.remote.active || GetTickCount64() - state.remote.observedAt < 4500 || (keepSelected && selected == ActivityKind::Remote)))
        result.push_back(ActivityKind::Remote);
    if ((settings.memoryAlerts && state.memory.valid && state.memory.percent >= 80) || (settings.temperatureAlerts && std::max(state.temperature.cpu,state.temperature.gpu)>=70) || (keepSelected && selected==ActivityKind::Memory))
        result.push_back(ActivityKind::Memory);
    return result;
}

std::mutex g_liveMutex;
LiveSnapshot g_liveSnapshot;
ObsSnapshot g_obsSnapshot;
ClockSnapshot g_clockSnapshot;
HANDLE g_clockThread=nullptr;
DisplaySnapshot g_displaySnapshot;
HotspotSnapshot g_hotspotSnapshot;
BluetoothSnapshot g_bluetoothSnapshot;
TaskNotice g_taskNotice;
MemorySnapshot g_memorySnapshot;
std::vector<StorageVolume> g_storageVolumes;
RemoteSnapshot g_remoteSnapshot;
TemperatureSnapshot g_temperatureSnapshot;
HANDLE g_temperatureThread = nullptr;
std::atomic<DWORD> g_settingsHostPid{0};
std::atomic<bool> g_memoryDetailsRequested{false};
std::atomic<uint64_t> g_settingsRevision{1};
std::atomic<bool> g_bluetoothDirty{true};
std::map<uint64_t, TaskControlState> g_taskControls;
HANDLE g_systemWake = nullptr, g_systemThread = nullptr, g_taskControlWake = nullptr, g_taskControlThread = nullptr;
HANDLE g_bluetoothWake = nullptr, g_bluetoothThread = nullptr;
std::atomic<bool> g_displayDirty{true};
ObsCommand g_obsCommand;
HANDLE g_obsWake = nullptr, g_obsThread = nullptr;
struct NativeLocationLabel { std::wstring apps; uint64_t observed = 0; };
NativeLocationLabel g_nativeLocation;
std::atomic<uint64_t> g_nativeLocationRevision{0};
struct PendingMediaCommand {
    MediaAction action = MediaAction::None;
    uint64_t generation = 0;
    int64_t position = 0;
};
std::atomic<uint64_t> g_requestedMediaSession{0};
std::atomic<uint64_t> g_mediaActivitySerial{0}, g_requestedMediaSerial{0};
PendingMediaCommand g_mediaCommand;
HANDLE g_liveStopEvent = nullptr;
HANDLE g_liveWakeEvent = nullptr;
HANDLE g_liveThread = nullptr;

int64_t ObsElapsedMilliseconds(ObsSnapshot const& state, uint64_t now) {
    if (!state.recording) return 0;
    uint64_t advance = state.connected && !state.paused && state.sampledAt && now >= state.sampledAt
        ? std::min<uint64_t>(now - state.sampledAt, 4000) : 0;
    return state.elapsedMilliseconds + static_cast<int64_t>(advance);
}
LiveSnapshot ReadLiveSnapshot() {
    std::lock_guard<std::mutex> lock(g_liveMutex);
    auto result = g_liveSnapshot; result.obs = g_obsSnapshot; result.clock=g_clockSnapshot;
    result.obs.elapsedSeconds = ObsElapsedMilliseconds(result.obs, GetTickCount64()) / 1000;
    result.display = g_displaySnapshot;
    result.hotspot = g_hotspotSnapshot;
    result.bluetooth = g_bluetoothSnapshot;
    result.taskNotice = g_taskNotice;
    result.memory = g_memorySnapshot;
    result.remote = g_remoteSnapshot;
    result.temperature = g_temperatureSnapshot;
    std::vector<TaskProgress> detailed;
    for (auto task : result.tasks) {
        auto found = g_taskControls.find(task.id);
        if (found != g_taskControls.end() && found->second.window == task.window) task.control = found->second;
        // Keep stale identifiers private for a safe retry, but do not present an
        // old per-file count as current when only aggregate progress remains.
        bool fresh = task.control.sampledAt && GetTickCount64()-task.control.sampledAt <= 4000;
        if (task.control.files.empty() || !fresh) detailed.push_back(std::move(task));
        else {
            auto files = std::move(task.control.files);
            for (auto const& file : files) {
                if (detailed.size() >= 32) break;
                auto copy = task; copy.fileKey = file.filename; copy.control.filename = file.filename;
                copy.control.controls = file.controls;
                if (file.percent >= 0) copy.percent = file.percent;
                else if (files.size() > 1) copy.percent = -1; // Aggregate taskbar percentage is not a per-file percentage.
                if (file.controls & 2) copy.state = 8;
                detailed.push_back(std::move(copy));
            }
        }
    }
    result.tasks = std::move(detailed);
    return result;
}

// No async completion handler points into this DLL. Pending operations are
// cancelled on shutdown; their results are consumed only after completion.
template <typename Operation, typename T>
bool WaitProviderOperation(Operation const& operation, T& result, DWORD timeout = 5000) {
    if (WaitForSingleObject(g_liveStopEvent, 0) == WAIT_OBJECT_0) {
        try { operation.Cancel(); } catch (...) {}
        return false;
    }
    const ULONGLONG deadline = GetTickCount64() + timeout;
    try {
        while (operation.Status() == wf::AsyncStatus::Started) {
            if (WaitForSingleObject(g_liveStopEvent, 25) == WAIT_OBJECT_0 ||
                GetTickCount64() >= deadline) {
                operation.Cancel();
                return false;
            }
        }
        if (operation.Status() != wf::AsyncStatus::Completed) return false;
        result = operation.GetResults();
        return true;
    } catch (...) { return false; }
}

struct BatteryEstimateState {
    struct Sample { uint64_t time; double remaining; };
    std::vector<Sample> samples;
    int mode = 0, source = 0;
    double full = 0, rate = 0;
    uint64_t lastRateTime = 0;
    int Observe(uint64_t now, int direction, double capacity, double remaining, double reportedRate, int units) {
        if (!direction || capacity <= 0 || remaining < 0 || remaining > capacity) { *this = {}; return -1; }
        if (mode != direction || source != units || std::abs(full - capacity) > capacity * .01 ||
            (!samples.empty() && (now < samples.back().time || now - samples.back().time > 120000))) {
            *this = {}; mode = direction; source = units; full = capacity;
        }
        if (samples.empty() || now - samples.back().time >= 10000) samples.push_back({now, remaining});
        while (samples.size() > 181 || (!samples.empty() && now - samples.front().time > 1800000)) samples.erase(samples.begin());
        double observedRate = 0;
        if (std::isfinite(reportedRate) && reportedRate * direction > 0 && std::abs(reportedRate) < capacity * 10)
            observedRate = std::abs(reportedRate);
        if (!observedRate && samples.size() >= 2 && now - samples.front().time >= 120000) {
            double change = (remaining - samples.front().remaining) * direction;
            if (change >= (units == 3 ? 2.0 : capacity * .002))
                observedRate = change * 3600000.0 / (now - samples.front().time);
        }
        if (!observedRate) {
            if (!lastRateTime || now - lastRateTime > 120000) return -1;
        } else {
            rate = rate > 0 ? rate * .75 + observedRate * .25 : observedRate;
            lastRateTime = now;
        }
        double minutes = (direction > 0 ? capacity - remaining : remaining) * 60 / rate;
        if (!std::isfinite(minutes) || minutes < 0 || minutes > 1440) return -1;
        return std::max(1, static_cast<int>(std::ceil(minutes)));
    }
};

BatterySnapshot ReadBatterySnapshot(bool winrtReady, BatteryEstimateState& estimate) {
    BatterySnapshot b;
    SYSTEM_POWER_STATUS power{};
    if (!GetSystemPowerStatus(&power) || power.BatteryFlag == 255) { estimate = {}; return b; }
    b.valid = true; b.present = (power.BatteryFlag & 128) == 0;
    b.pluggedIn = power.ACLineStatus == 1;
    b.charging = b.present && (power.BatteryFlag & 8) != 0;
    if (!b.present) { estimate = {}; return b; }
    if (power.BatteryLifePercent <= 100) b.percent = power.BatteryLifePercent;
    if (!b.pluggedIn && power.BatteryLifeTime != DWORD(-1) && power.BatteryLifeTime <= 86400)
        b.minutesRemaining = static_cast<int>((power.BatteryLifeTime + 59) / 60);
    double full = 0, remaining = -1, rate = 0; int source = 0;
    if (winrtReady) try {
        auto report = winrt::Windows::Devices::Power::Battery::AggregateBattery().GetReport();
        auto f = report.FullChargeCapacityInMilliwattHours(), r = report.RemainingCapacityInMilliwattHours();
        auto watts = report.ChargeRateInMilliwatts();
        if (f && r && f.Value() > 0 && r.Value() >= 0 && r.Value() <= f.Value()) {
            full = f.Value(); remaining = r.Value(); rate = watts ? watts.Value() : 0; source = 1;
        }
    } catch (...) {}
    // SYSTEM_BATTERY_STATE::Rate is signed despite its DWORD declaration (Microsoft Learn).
    using QueryPower = LONG (WINAPI*)(POWER_INFORMATION_LEVEL, PVOID, ULONG, PVOID, ULONG);
    HMODULE powerLibrary = LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (powerLibrary) {
        auto query = reinterpret_cast<QueryPower>(GetProcAddress(powerLibrary, "CallNtPowerInformation"));
        SYSTEM_BATTERY_STATE native{};
        if (query && query(SystemBatteryState, nullptr, 0, &native, sizeof(native)) >= 0 && native.BatteryPresent) {
            b.charging = native.Charging != FALSE;
            if (!b.pluggedIn && b.minutesRemaining < 0 && native.EstimatedTime != DWORD(-1) && native.EstimatedTime <= 86400)
                b.minutesRemaining = static_cast<int>((native.EstimatedTime + 59) / 60);
            if (!source && native.MaxCapacity > 0 && native.MaxCapacity != DWORD(-1) && native.RemainingCapacity <= native.MaxCapacity) {
                full = native.MaxCapacity; remaining = native.RemainingCapacity;
                rate = native.Rate == DWORD(-1) ? 0 : static_cast<LONG>(native.Rate); source = 2;
            } else if (source && !rate && native.Rate != DWORD(-1)) rate = static_cast<LONG>(native.Rate);
        }
        FreeLibrary(powerLibrary);
    }
    if (!source && b.percent >= 0) { full = 100; remaining = b.percent; source = 3; }
    if (b.percent < 0 && full > 0) b.percent = static_cast<int>(remaining * 100 / full);
    int mode = b.charging ? 1 : !b.pluggedIn ? -1 : 0;
    int minutes = estimate.Observe(GetTickCount64(), mode, full, remaining, rate, source);
    if (b.charging) b.minutesToFull = minutes;
    else if (!b.pluggedIn && b.minutesRemaining < 0) b.minutesRemaining = minutes;
    return b;
}

// The event delegates only signal a shared wake object. Revocation and delegate
// lifetime draining happen before the worker exits and the mod can unload.
struct ProviderWake {
    HANDLE event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    std::atomic<unsigned> flags{~0u};
    ~ProviderWake() { if (event) CloseHandle(event); }
    void Signal(unsigned value) { flags.fetch_or(value); if (event) SetEvent(event); }
};
constexpr unsigned kMediaState = 1, kMediaMetadata = 2, kBatteryState = 4, kEffects = 8;
HANDLE g_artWake = nullptr, g_artThread = nullptr;
std::wstring g_artRequest;
uint64_t g_artGeneration = 0;
std::wstring g_artResultKey;
uint64_t g_artResultGeneration = 0;
std::shared_ptr<ImagePixels> g_artResultPixels;
std::atomic<bool> g_effectsAllowed{true};
std::atomic<bool> g_forcePowerRefresh{true};

std::shared_ptr<ImagePixels> DecodeImage(std::vector<uint8_t> const& bytes, UINT edge) {
    if (bytes.empty() || bytes.size() > 4 * 1024 * 1024) return {};
    winrt::com_ptr<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(factory.put())))) return {};
    winrt::com_ptr<IWICStream> stream;
    winrt::check_hresult(factory->CreateStream(stream.put()));
    winrt::check_hresult(stream->InitializeFromMemory(const_cast<BYTE*>(bytes.data()), static_cast<DWORD>(bytes.size())));
    winrt::com_ptr<IWICBitmapDecoder> decoder;
    winrt::check_hresult(factory->CreateDecoderFromStream(stream.get(), nullptr, WICDecodeMetadataCacheOnDemand, decoder.put()));
    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    winrt::check_hresult(decoder->GetFrame(0, frame.put()));
    UINT width = 0, height = 0;
    winrt::check_hresult(frame->GetSize(&width, &height));
    if (!width || !height || width > 16384 || height > 16384 || uint64_t(width) * height > 40000000) return {};
    double scale = std::min(1.0, double(edge) / std::max(width, height));
    width = std::max(1u, static_cast<UINT>(width * scale));
    height = std::max(1u, static_cast<UINT>(height * scale));
    winrt::com_ptr<IWICBitmapScaler> scaler;
    winrt::check_hresult(factory->CreateBitmapScaler(scaler.put()));
    winrt::check_hresult(scaler->Initialize(frame.get(), width, height, WICBitmapInterpolationModeFant));
    winrt::com_ptr<IWICFormatConverter> converter;
    winrt::check_hresult(factory->CreateFormatConverter(converter.put()));
    winrt::check_hresult(converter->Initialize(scaler.get(), GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom));
    auto result = std::make_shared<ImagePixels>();
    result->width = width; result->height = height;
    result->bytes.resize(size_t(width) * height * 4);
    winrt::check_hresult(converter->CopyPixels(nullptr, width * 4,
        static_cast<UINT>(result->bytes.size()), result->bytes.data()));
    return result;
}

template<class T> void CancelOperation(T& operation) {
    if (!operation) return;
    try { operation.Cancel(); } catch (...) {}
    operation = nullptr;
}

std::wstring LowerText(std::wstring text) {
    if (!text.empty()) CharLowerBuffW(text.data(), static_cast<DWORD>(text.size()));
    return text;
}
std::mutex g_appNamesMutex;
std::map<std::wstring, std::wstring> g_appNames;
std::wstring FriendlyAppName(std::wstring name) {
    auto key = LowerText(name);
    { std::lock_guard<std::mutex> lock(g_appNamesMutex);
      auto found = g_appNames.find(key); if (found != g_appNames.end()) return found->second; }
    static constexpr std::pair<PCWSTR, PCWSTR> known[] = {{L"chrome.exe", L"Google Chrome"}, {L"msedge.exe", L"Microsoft Edge"},
        {L"firefox.exe", L"Mozilla Firefox"}, {L"explorer.exe", L"File Explorer"}, {L"obs64.exe", L"OBS Studio"},
        {L"7zfm.exe", L"7-Zip"}, {L"7zg.exe", L"7-Zip"}, {L"code.exe", L"Visual Studio Code"}};
    for (auto const& item : known) if (key == item.first) return item.second;
    if (key.ends_with(L".exe")) name.resize(name.size() - 4);
    return name;
}
void RememberAppName(std::wstring const& exe, PCWSTR path) {
    auto key = LowerText(exe);
    { std::lock_guard<std::mutex> lock(g_appNamesMutex); if (g_appNames.contains(key)) return; }
    std::wstring display = FriendlyAppName(exe);
    DWORD ignored = 0, size = GetFileVersionInfoSizeW(path, &ignored);
    if (size > 0 && size <= 1024 * 1024) {
        std::vector<BYTE> bytes(size);
        if (GetFileVersionInfoW(path, 0, size, bytes.data())) {
            struct Translation { WORD language, codePage; }; Translation* values = nullptr; UINT length = 0;
            if (VerQueryValueW(bytes.data(), L"\\VarFileInfo\\Translation", reinterpret_cast<void**>(&values), &length)) {
                for (size_t i = 0; i < std::min<size_t>(length / sizeof(Translation), 16); ++i) {
                    WCHAR query[100]; swprintf_s(query, L"\\StringFileInfo\\%04x%04x\\FileDescription", values[i].language, values[i].codePage);
                    WCHAR* text = nullptr; UINT count = 0;
                    if (VerQueryValueW(bytes.data(), query, reinterpret_cast<void**>(&text), &count) && text && count > 1 && count < 256) {
                        display.assign(text, count - 1); break;
                    }
                }
            }
        }
    }
    std::lock_guard<std::mutex> lock(g_appNamesMutex);
    if (g_appNames.size() < 256) g_appNames.emplace(key, std::move(display));
}
std::wstring ProcessName(DWORD pid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return {};
    WCHAR path[32768]; DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (!ok) return {};
    std::wstring name(path, size); auto slash = name.find_last_of(L"\\/");
    name = slash == std::wstring::npos ? name : name.substr(slash + 1);
    RememberAppName(name, path);
    return name;
}
std::set<std::wstring> RunningProcessNames() {
    std::set<std::wstring> names;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return names;
    PROCESSENTRY32W entry{}; entry.dwSize = sizeof(entry);
    if (Process32FirstW(snapshot, &entry)) do { names.insert(LowerText(entry.szExeFile)); }
        while (Process32NextW(snapshot, &entry));
    CloseHandle(snapshot); return names;
}
constexpr unsigned kCaptureState = 16, kCaptureDevices = 32;
// Callbacks only enqueue audio-session references and wake the provider; all UI work stays on its thread.
struct CaptureSignals : winrt::implements<CaptureSignals, IAudioSessionEvents, IAudioSessionNotification, IMMNotificationClient> {
    std::shared_ptr<ProviderWake> wake;
    std::mutex mutex;
    std::vector<winrt::com_ptr<IAudioSessionControl>> incoming;
    explicit CaptureSignals(std::shared_ptr<ProviderWake> value) : wake(std::move(value)) {}
    HRESULT STDMETHODCALLTYPE OnSessionCreated(IAudioSessionControl* session) noexcept override {
        try {
            std::lock_guard<std::mutex> lock(mutex);
            if (session && incoming.size() < 128) { winrt::com_ptr<IAudioSessionControl> value; value.copy_from(session); incoming.push_back(std::move(value)); }
            wake->Signal(kCaptureState);
        } catch (...) {}
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnStateChanged(AudioSessionState) noexcept override { wake->Signal(kCaptureState); return S_OK; }
    HRESULT STDMETHODCALLTYPE OnSessionDisconnected(AudioSessionDisconnectReason) noexcept override { wake->Signal(kCaptureState | kCaptureDevices); return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(LPCWSTR,LPCGUID) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnIconPathChanged(LPCWSTR,LPCGUID) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(float,BOOL,LPCGUID) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(DWORD,float[],DWORD,LPCGUID) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID,LPCGUID) noexcept override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR,DWORD) noexcept override { wake->Signal(kCaptureDevices | kCaptureState); return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) noexcept override { wake->Signal(kCaptureDevices | kCaptureState); return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) noexcept override { wake->Signal(kCaptureDevices | kCaptureState); return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow,ERole,LPCWSTR) noexcept override { wake->Signal(kCaptureDevices | kCaptureState); return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR,PROPERTYKEY) noexcept override { return S_OK; }
};
class CaptureMonitor {
    winrt::com_ptr<IMMDeviceEnumerator> devices;
    winrt::com_ptr<CaptureSignals> signals;
    std::vector<winrt::com_ptr<IAudioSessionManager2>> managers;
    struct Entry { winrt::com_ptr<IAudioSessionControl2> control; std::wstring id, app; };
    std::vector<Entry> entries;
    bool ready = false, deviceNotification = false;
    void Add(winrt::com_ptr<IAudioSessionControl> const& control) {
        auto value = control.try_as<IAudioSessionControl2>();
        if (!value || entries.size() >= 256) return;
        LPWSTR raw = nullptr;
        if (FAILED(value->GetSessionInstanceIdentifier(&raw))) return;
        std::wstring id(raw ? raw : L""); CoTaskMemFree(raw);
        if (id.empty() || std::any_of(entries.begin(), entries.end(), [&](auto const& e) { return e.id == id; })) return;
        DWORD pid = 0; value->GetProcessId(&pid);
        std::wstring app = ProcessName(pid);
        if (app.empty()) app = L"PID " + std::to_wstring(pid);
        if (SUCCEEDED(value->RegisterAudioSessionNotification(signals.get()))) entries.push_back({value, std::move(id), std::move(app)});
    }
public:
    explicit CaptureMonitor(std::shared_ptr<ProviderWake> wake) : signals(winrt::make_self<CaptureSignals>(std::move(wake))) {}
    ~CaptureMonitor() { Clear(); }
    bool Ready() const { return ready; }
    void Clear() {
        ready = false;
        if (deviceNotification && devices) devices->UnregisterEndpointNotificationCallback(signals.get());
        deviceNotification = false;
        for (auto const& manager : managers) manager->UnregisterSessionNotification(signals.get());
        for (auto const& entry : entries) entry.control->UnregisterAudioSessionNotification(signals.get());
        managers.clear(); entries.clear(); devices = nullptr;
        if (signals) { std::lock_guard<std::mutex> lock(signals->mutex); signals->incoming.clear(); }
    }
    void Release() { signals = nullptr; }
    void Rebuild() {
        Clear();
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(devices.put())))) return;
        deviceNotification = SUCCEEDED(devices->RegisterEndpointNotificationCallback(signals.get()));
        winrt::com_ptr<IMMDeviceCollection> collection;
        if (FAILED(devices->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, collection.put()))) return;
        UINT count = 0; collection->GetCount(&count); bool all = count > 0;
        for (UINT i = 0; i < std::min<UINT>(count, 32); ++i) {
            winrt::com_ptr<IMMDevice> device;
            winrt::com_ptr<IAudioSessionManager2> manager;
            if (FAILED(collection->Item(i, device.put())) || FAILED(device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_INPROC_SERVER,
                    nullptr, manager.put_void()))) { all = false; continue; }
            if (FAILED(manager->RegisterSessionNotification(signals.get()))) { all = false; continue; }
            managers.push_back(manager);
            winrt::com_ptr<IAudioSessionEnumerator> enumerator;
            if (FAILED(manager->GetSessionEnumerator(enumerator.put()))) { all = false; continue; }
            int n = 0; enumerator->GetCount(&n);
            for (int j = 0; j < std::min(n, 256); ++j) { winrt::com_ptr<IAudioSessionControl> control;
                if (SUCCEEDED(enumerator->GetSession(j, control.put()))) Add(control); }
        }
        ready = all;
    }
    void AppendActive(PrivacySnapshot& snapshot) {
        if (!signals) return;
        std::vector<winrt::com_ptr<IAudioSessionControl>> incoming;
        { std::lock_guard<std::mutex> lock(signals->mutex); incoming.swap(signals->incoming); }
        for (auto const& value : incoming) Add(value);
        for (auto it = entries.begin(); it != entries.end();) {
            AudioSessionState state{};
            if (FAILED(it->control->GetState(&state)) || state == AudioSessionStateExpired) {
                it->control->UnregisterAudioSessionNotification(signals.get()); it = entries.erase(it); continue;
            }
            if (state == AudioSessionStateActive && std::none_of(snapshot.uses.begin(), snapshot.uses.end(), [&](auto const& use) {
                return use.capability == L"microphone" && _wcsicmp(use.app.c_str(), it->app.c_str()) == 0;
            })) snapshot.uses.push_back({L"microphone", it->app, 0});
            ++it;
        }
        snapshot.available = snapshot.available || ready;
        std::sort(snapshot.uses.begin(), snapshot.uses.end(), [](auto const& a, auto const& b) {
            return std::tie(a.capability, a.app) < std::tie(b.capability, b.app);
        });
    }
};

#ifndef __IMFSensorActivityMonitor_INTERFACE_DEFINED__
// The camera activity interfaces from the Windows SDK (mfidl.h).
// Declared here because Windhawk's MinGW headers do not project them yet.
struct IMFSensorProcessActivity : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetProcessId(ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamingState(BOOL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamingMode(int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetReportTime(FILETIME*) = 0;
};
struct IMFSensorActivityReport : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetFriendlyName(LPWSTR, ULONG, ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSymbolicLink(LPWSTR, ULONG, ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessCount(ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessActivity(ULONG, IMFSensorProcessActivity**) = 0;
};
struct IMFSensorActivitiesReport : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetActivityReport(ULONG, IMFSensorActivityReport**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetActivityReportByDeviceName(LPCWSTR, IMFSensorActivityReport**) = 0;
};
struct IMFSensorActivitiesReportCallback : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE OnActivitiesReport(IMFSensorActivitiesReport*) = 0;
};
__CRT_UUID_DECL(IMFSensorActivitiesReportCallback, 0xde5072ee, 0xdbe3, 0x46dc, 0x8a, 0x87, 0xb6, 0xf6, 0x31, 0x19, 0x47, 0x51);
struct IMFSensorActivityMonitor : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Start() = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop() = 0;
};
#endif
// Camera activity is observed without opening a capture stream.
struct CameraSignals : winrt::implements<CameraSignals, IMFSensorActivitiesReportCallback> {
    std::shared_ptr<ProviderWake> wake;
    std::shared_ptr<int> lifetime;
    std::mutex mutex;
    std::map<std::wstring, std::set<DWORD>> devices;
    std::atomic<bool> receivedReport{false};
    CameraSignals(std::shared_ptr<ProviderWake> value, std::shared_ptr<int> alive) : wake(std::move(value)), lifetime(std::move(alive)) {}
    HRESULT STDMETHODCALLTYPE OnActivitiesReport(IMFSensorActivitiesReport* report) noexcept override {
        try {
            ULONG count = 0;
            if (!report || FAILED(report->GetCount(&count))) return S_OK;
            receivedReport = true;
            std::lock_guard<std::mutex> lock(mutex);
            if (!count) devices.clear();
            for (ULONG i = 0; i < std::min<ULONG>(count, 64); ++i) {
                winrt::com_ptr<IMFSensorActivityReport> item;
                if (FAILED(report->GetActivityReport(i, item.put()))) continue;
                WCHAR link[2048]{}; ULONG written = 0, processes = 0;
                if (FAILED(item->GetSymbolicLink(link, ARRAYSIZE(link), &written)) ||
                    FAILED(item->GetProcessCount(&processes))) continue;
                std::set<DWORD> active;
                for (ULONG j = 0; j < std::min<ULONG>(processes, 128); ++j) {
                    winrt::com_ptr<IMFSensorProcessActivity> process;
                    ULONG pid = 0; BOOL streaming = FALSE;
                    if (SUCCEEDED(item->GetProcessActivity(j, process.put())) &&
                        SUCCEEDED(process->GetStreamingState(&streaming)) && streaming &&
                        SUCCEEDED(process->GetProcessId(&pid)) && pid) active.insert(pid);
                }
                if (active.empty()) devices.erase(link);
                else if (devices.size() < 64 || devices.contains(link)) devices[link] = std::move(active);
            }
            wake->Signal(kCaptureState);
        } catch (...) {}
        return S_OK;
    }
};
class CameraMonitor {
    HMODULE module = nullptr, platform = nullptr;
    using Shutdown = HRESULT (WINAPI*)();
    Shutdown shutdown = nullptr;
    bool platformStarted = false;
    winrt::com_ptr<IMFSensorActivityMonitor> monitor;
    winrt::com_ptr<CameraSignals> callback;
    std::shared_ptr<int> lifetime = std::make_shared<int>(0);
    std::shared_ptr<ProviderWake> wake;
public:
    explicit CameraMonitor(std::shared_ptr<ProviderWake> value) : wake(std::move(value)) {}
    ~CameraMonitor() { Stop(); }
    void Start() {
        if (monitor) return;
        if (!platform) platform = LoadLibraryExW(L"mfplat.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        using Startup = HRESULT (WINAPI*)(ULONG, DWORD);
        auto startup = platform ? reinterpret_cast<Startup>(GetProcAddress(platform, "MFStartup")) : nullptr;
        shutdown = platform ? reinterpret_cast<Shutdown>(GetProcAddress(platform, "MFShutdown")) : nullptr;
        if (!platformStarted && startup && shutdown) platformStarted = SUCCEEDED(startup(MF_VERSION, MFSTARTUP_NOSOCKET));
        if (!platformStarted) return;
        if (!module) module = LoadLibraryExW(L"mfsensorgroup.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        using Create = HRESULT (WINAPI*)(IMFSensorActivitiesReportCallback*, IMFSensorActivityMonitor**);
        auto create = module ? reinterpret_cast<Create>(GetProcAddress(module, "MFCreateSensorActivityMonitor")) : nullptr;
        if (!create) { Stop(); return; }
        callback = winrt::make_self<CameraSignals>(wake, lifetime);
        HRESULT result = create(callback.get(), monitor.put());
        if (SUCCEEDED(result)) result = monitor->Start();
        if (FAILED(result)) { Stop(); Wh_Log(L"Camera activity monitor: 0x%08X", result); }
        else Wh_Log(L"Camera activity monitor started");
    }
    void Stop() {
        if (monitor) {
            monitor->Stop();
            winrt::com_ptr<IMFShutdown> close;
            if (SUCCEEDED(monitor->QueryInterface(IID_PPV_ARGS(close.put())))) close->Shutdown();
        }
        monitor = nullptr;
        // The monitor must release its callback before its code and wake event go away.
        callback = nullptr;
        while (lifetime.use_count() > 1) WaitForSingleObject(wake->event, 10);
        if (module) { FreeLibrary(module); module = nullptr; }
        if (platformStarted && shutdown) shutdown();
        platformStarted = false; shutdown = nullptr;
        if (platform) { FreeLibrary(platform); platform = nullptr; }
    }
    void Release() { Stop(); wake.reset(); }
    bool Ready() const { return callback && callback->receivedReport.load(); }
    void Append(PrivacySnapshot& snapshot) {
        if (!callback) return;
        std::set<DWORD> processes;
        { std::lock_guard<std::mutex> lock(callback->mutex);
          for (auto const& [device, ids] : callback->devices) processes.insert(ids.begin(), ids.end()); }
        for (DWORD pid : processes) {
            auto app = ProcessName(pid);
            if (app.empty()) app = L"PID " + std::to_wstring(pid);
            if (std::none_of(snapshot.uses.begin(), snapshot.uses.end(), [&](auto const& use) {
                return use.capability == L"webcam" && _wcsicmp(use.app.c_str(), app.c_str()) == 0;
            })) snapshot.uses.push_back({L"webcam", app, 0});
        }
        snapshot.available = true;
    }
};

struct PowerModeSnapshot {
    int ac = -1, dc = -1;
    DWORD error = 0;
    bool operator==(PowerModeSnapshot const&) const = default;
};
struct AppUsage { std::wstring app; uint64_t milliseconds = 0; };
struct UsageSnapshot {
    std::vector<AppUsage> apps;
    bool onBattery = false;
    uint64_t measuredSince = 0, revision = 0;
};
PowerModeSnapshot g_powerModes;
UsageSnapshot g_usageSnapshot;
std::atomic<int> g_powerModeRequest{-1};
std::atomic<bool> g_batteryPageOpen{false};
std::atomic<bool> g_usageForegroundChanged{false};
void CALLBACK UsageForegroundEvent(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) {
    g_usageForegroundChanged = true;
    if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
}
uint64_t ForegroundUsageInterval(uint64_t now, uint64_t previous, uint32_t lastInput, bool foreground) {
    if (!foreground || now < previous || now - previous > 2000 || uint32_t(now) - lastInput >= 60000) return 0;
    return now - previous;
}
class BatteryTools {
    using GetMode = DWORD (WINAPI*)(GUID*);
    using SetMode = DWORD (WINAPI*)(const GUID*);
    HMODULE library = nullptr;
    GetMode getAc = nullptr, getDc = nullptr;
    SetMode setAc = nullptr, setDc = nullptr;
    PowerModeSnapshot modes;
    std::map<std::wstring, uint64_t> bootUsage, batteryUsage;
    uint64_t lastTick = 0, started = 0, unpluggedAt = 0, nextPublish = 0, revision = 0;
    bool wasPlugged = true, initialized = false;
    HWND lastWindow = nullptr;
    std::wstring foreground;
    HWINEVENTHOOK foregroundHook = nullptr;
    static const GUID& ModeGuid(int index) {
        static constexpr GUID modes[] = {
            {0x961cc777,0x2547,0x4f9d,{0x81,0x74,0x7d,0x86,0x18,0x1b,0x8a,0x7a}},
            {}, {0xded574b5,0x45a0,0x4f42,{0x87,0x37,0x46,0x34,0x5c,0x09,0xc2,0x38}}};
        return modes[index];
    }
    int Read(GetMode get) {
        GUID guid{};
        if (!get || get(&guid) != ERROR_SUCCESS) return -1;
        for (int i = 0; i < 3; ++i) if (IsEqualGUID(guid, ModeGuid(i))) return i;
        return -1;
    }
public:
    BatteryTools() {
        library = LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (library) {
            getAc = reinterpret_cast<GetMode>(GetProcAddress(library, "PowerGetUserConfiguredACPowerMode"));
            getDc = reinterpret_cast<GetMode>(GetProcAddress(library, "PowerGetUserConfiguredDCPowerMode"));
            setAc = reinterpret_cast<SetMode>(GetProcAddress(library, "PowerSetUserConfiguredACPowerMode"));
            setDc = reinterpret_cast<SetMode>(GetProcAddress(library, "PowerSetUserConfiguredDCPowerMode"));
        }
        foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
            nullptr, UsageForegroundEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
    }
    ~BatteryTools() { if (foregroundHook) UnhookWinEvent(foregroundHook); if (library) FreeLibrary(library); }
    void Tick(bool enabled, bool force) {
        uint64_t now = GetTickCount64();
        if (!enabled) { initialized = false; bootUsage.clear(); batteryUsage.clear(); g_powerModeRequest = -1; return; }
        SYSTEM_POWER_STATUS status{};
        if (!GetSystemPowerStatus(&status)) return;
        bool plugged = status.ACLineStatus != 0;
        if (!initialized) { started = now; unpluggedAt = now; lastTick = now; wasPlugged = plugged; initialized = true; }
        if (wasPlugged && !plugged) { batteryUsage.clear(); unpluggedAt = now; }
        LASTINPUTINFO input{sizeof(input), 0};
        bool hasInput = GetLastInputInfo(&input) != FALSE;
        HWND window = GetForegroundWindow();
        uint64_t delta = ForegroundUsageInterval(now, lastTick, input.dwTime, hasInput && window);
        // Exclude sleep, a stalled worker, the lock screen, and idle periods.
        if (delta && !foreground.empty()) {
            if (bootUsage.size() < 128 || bootUsage.contains(foreground)) bootUsage[foreground] += delta;
            if (!wasPlugged && !plugged && (batteryUsage.size() < 128 || batteryUsage.contains(foreground)))
                batteryUsage[foreground] += delta;
        }
        if (window != lastWindow || g_usageForegroundChanged.exchange(false)) {
            DWORD pid = 0; GetWindowThreadProcessId(window, &pid);
            foreground = ProcessName(pid); lastWindow = window;
            if (_wcsicmp(foreground.c_str(), L"LockApp.exe") == 0 || _wcsicmp(foreground.c_str(), L"LogonUI.exe") == 0)
                foreground.clear();
        }
        lastTick = now; wasPlugged = plugged;
        int request = g_powerModeRequest.exchange(-1);
        if (request >= 0 && request < 6) {
            auto set = request < 3 ? setAc : setDc;
            modes.error = set ? set(&ModeGuid(request % 3)) : ERROR_NOT_SUPPORTED;
            force = true;
        }
        if (force || (g_batteryPageOpen && now >= nextPublish)) {
            modes.ac = setAc ? Read(getAc) : -1; modes.dc = setDc ? Read(getDc) : -1;
            UsageSnapshot usage; usage.onBattery = !plugged; usage.revision = ++revision;
            usage.measuredSince = plugged ? started : unpluggedAt;
            for (auto const& [app, ms] : plugged ? bootUsage : batteryUsage) if (ms) usage.apps.push_back({app, ms});
            std::sort(usage.apps.begin(), usage.apps.end(), [](auto const& a, auto const& b) { return a.milliseconds > b.milliseconds; });
            { std::lock_guard<std::mutex> lock(g_liveMutex); g_powerModes = modes; g_usageSnapshot = std::move(usage); }
            nextPublish = now + 5000; NotifyLiveUi();
        }
    }
};

// Explorer's taskband message protocol; observed passively, never consumed or redirected.
// Protocol: cairoshell/ManagedShell, TasksService.cs (WM_USER + 64 / + 65).
// Scale: cairoshell/cairoshell, CairoDesktop.Taskbar/TaskButton.xaml, Maximum="65534".
int TaskbandPercent(int64_t value) {
    return value < 0 || value > 65534 ? -1 : static_cast<int>((value * 100 + 32767) / 65534);
}
struct RawTaskProgress {
    uintptr_t window = 0;
    int percent = -1;
    unsigned state = 0;
    uint64_t epoch = 0;
    ULONGLONG started = 0;
    bool completeReported = false;
};
std::mutex g_taskProgressMutex;
std::map<uintptr_t, RawTaskProgress> g_rawTaskProgress;
std::vector<RawTaskProgress> g_endedTaskSignals;
std::atomic<bool> g_taskProgressEnabled{false};
std::atomic<uint64_t> g_taskProgressRevision{0};
uint64_t g_taskProgressEpoch = 0;
HHOOK g_taskProgressCallHook = nullptr, g_taskProgressGetHook = nullptr;
HWND g_taskProgressTarget = nullptr;
void ObserveTaskProgress(HWND destination, UINT message, WPARAM wParam, LPARAM lParam) noexcept {
    if (!g_taskProgressEnabled || g_shuttingDown || destination != g_taskProgressTarget ||
        (message != WM_USER + 64 && message != WM_USER + 65)) return;
    if (!wParam || !IsWindow(reinterpret_cast<HWND>(wParam)) || (message == WM_USER + 64 && (lParam < 0 || lParam > 65534)) ||
        (message == WM_USER + 65 && lParam != 0 && lParam != 1 && lParam != 2 && lParam != 4 && lParam != 8)) return;
    try {
        std::lock_guard<std::mutex> lock(g_taskProgressMutex);
        uintptr_t id = static_cast<uintptr_t>(wParam);
        auto it = g_rawTaskProgress.find(id);
        if (it == g_rawTaskProgress.end()) {
            if (g_rawTaskProgress.size() >= 128 || !lParam) return;
            it = g_rawTaskProgress.emplace(id, RawTaskProgress{id, -1, 0, 0}).first;
        }
        auto before = it->second;
        if (message == WM_USER + 64) {
            if (!lParam && !it->second.state) return; // Chrome also sends 0 when clearing its taskbar badge.
            if (lParam > 0) it->second.completeReported = lParam == 65534;
            it->second.percent = TaskbandPercent(lParam);
            if (lParam > 0 && it->second.state != 4 && it->second.state != 8) it->second.state = 2;
        } else {
            if (!lParam && before.state && before.epoch) {
                if (g_endedTaskSignals.size() >= 64) g_endedTaskSignals.erase(g_endedTaskSignals.begin());
                g_endedTaskSignals.push_back(before);
                if (g_taskControlWake) SetEvent(g_taskControlWake);
            }
            it->second.state = static_cast<unsigned>(lParam);
            if (lParam == 0 || lParam == 1) it->second.percent = -1;
        }
        if (!before.state && it->second.state) {
            it->second.epoch = ++g_taskProgressEpoch;
            it->second.started = GetTickCount64();
        }
        if (before.state != it->second.state || before.percent != it->second.percent) {
            ++g_taskProgressRevision;
            if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
        }
    } catch (...) {}
}
LRESULT CALLBACK TaskProgressCallHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= 0) { auto message = reinterpret_cast<CWPSTRUCT const*>(lParam);
        ObserveTaskProgress(message->hwnd, message->message, message->wParam, message->lParam); }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}
LRESULT CALLBACK TaskProgressGetHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= 0 && wParam == PM_REMOVE) { auto message = reinterpret_cast<MSG const*>(lParam);
        ObserveTaskProgress(message->hwnd, message->message, message->wParam, message->lParam); }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}
void StopTaskProgressHooks() {
    g_taskProgressEnabled = false;
    if (g_taskProgressCallHook) UnhookWindowsHookEx(g_taskProgressCallHook);
    if (g_taskProgressGetHook) UnhookWindowsHookEx(g_taskProgressGetHook);
    g_taskProgressCallHook = g_taskProgressGetHook = nullptr; g_taskProgressTarget = nullptr;
    { std::lock_guard<std::mutex> lock(g_taskProgressMutex); g_rawTaskProgress.clear(); g_endedTaskSignals.clear(); ++g_taskProgressRevision; }
}
void EnsureTaskProgressHooks(HWND tray, bool enabled) {
    if (!enabled) { StopTaskProgressHooks(); return; }
    HWND target = reinterpret_cast<HWND>(GetPropW(tray, L"TaskbandHWND"));
    // Do not guess a window: WM_USER messages have different meanings on other classes.
    if (target == g_taskProgressTarget && g_taskProgressCallHook && g_taskProgressGetHook) return;
    StopTaskProgressHooks();
    if (!target || GetWindowThreadProcessId(target, nullptr) != GetCurrentThreadId()) return;
    g_taskProgressTarget = target;
    g_taskProgressCallHook = SetWindowsHookExW(WH_CALLWNDPROC, TaskProgressCallHook, nullptr, GetCurrentThreadId());
    g_taskProgressGetHook = SetWindowsHookExW(WH_GETMESSAGE, TaskProgressGetHook, nullptr, GetCurrentThreadId());
    if (!g_taskProgressCallHook || !g_taskProgressGetHook) { StopTaskProgressHooks(); return; }
    g_taskProgressEnabled = true;
    Wh_Log(L"Task progress observer attached to %p", target);
}
bool MatchesTaskApp(std::wstring name, ModSettings const& settings) {
    name = LowerText(std::move(name));
    if (name == L"explorer.exe") return settings.progressExplorer;
    if (name == L"chrome.exe") return settings.progressChrome;
    if (name == L"msedge.exe") return settings.progressEdge;
    if (name == L"firefox.exe") return settings.progressFirefox;
    if (name == L"7zfm.exe" || name == L"7zg.exe") return settings.progressSevenZip;
    auto extra = LowerText(settings.progressOtherApps.substr(0, 4096)); size_t offset = 0;
    for (unsigned count = 0; offset < extra.size() && count < 32; ++count) {
        auto end = extra.find(L';', offset); if (end == std::wstring::npos) end = extra.size();
        auto value = extra.substr(offset, end - offset);
        auto first = value.find_first_not_of(L" \t\r\n"), last = value.find_last_not_of(L" \t\r\n");
        if (first != std::wstring::npos && value.substr(first, last - first + 1) == name) return true;
        offset = end + 1;
    }
    return false;
}
std::vector<TaskProgress> ReadTaskProgress(ModSettings const& settings, ULONGLONG now) {
    struct Metadata { DWORD pid = 0; std::wstring app, title; ULONGLONG checked = 0; };
    static std::map<uintptr_t, Metadata> metadata;
    static std::vector<TaskProgress> previous;
    static uint64_t revision = ~0ULL;
    static ULONGLONG checked = 0;
    static unsigned priorFlags = ~0u;
    static std::wstring priorExtra;
    unsigned flags = settings.progressExplorer | (settings.progressChrome << 1) | (settings.progressEdge << 2) |
        (settings.progressFirefox << 3) | (settings.progressSevenZip << 4);
    if (!settings.enableTaskbarWidget || !settings.enableTaskProgress) {
        metadata.clear(); previous.clear(); revision = ~0ULL; return {};
    }
    auto currentRevision = g_taskProgressRevision.load();
    if (revision == currentRevision && now - checked < 1000 && priorFlags == flags && priorExtra == settings.progressOtherApps)
        return previous;
    revision = currentRevision; checked = now; priorFlags = flags; priorExtra = settings.progressOtherApps;
    std::vector<TaskProgress> result;
    std::vector<RawTaskProgress> raw;
    { std::lock_guard<std::mutex> lock(g_taskProgressMutex);
      for (auto it = g_rawTaskProgress.begin(); it != g_rawTaskProgress.end();) {
          if (!it->second.state || !IsWindow(reinterpret_cast<HWND>(it->first))) it = g_rawTaskProgress.erase(it);
          else { raw.push_back(it->second); ++it; }
      } }
    std::erase_if(metadata, [&](auto const& item) { return std::none_of(raw.begin(), raw.end(), [&](auto const& value) { return value.window == item.first; }); });
    HWND foreground = GetAncestor(GetForegroundWindow(), GA_ROOT);
    std::stable_sort(raw.begin(), raw.end(), [foreground](auto const& a, auto const& b) {
        auto priority = [foreground](auto const& item) {
            bool retained = std::any_of(previous.begin(), previous.end(), [&](auto const& task) {
                return task.window == item.window && task.id == item.epoch;
            });
            return retained ? 2 : reinterpret_cast<HWND>(item.window) == foreground ? 1 : 0;
        };
        return priority(a) > priority(b);
    });
    for (auto const& item : raw) {
        HWND window = reinterpret_cast<HWND>(item.window); DWORD pid = 0;
        GetWindowThreadProcessId(window, &pid);
        auto& info = metadata[item.window];
        if (info.pid != pid || now - info.checked >= 5000 || info.app.empty()) {
            info.pid = pid; info.checked = now; info.app = ProcessName(pid);
            WCHAR title[256]{};
            if (pid != GetCurrentProcessId()) GetWindowTextW(window, title, ARRAYSIZE(title));
            info.title = title;
        }
        if (!MatchesTaskApp(info.app, settings) || now - item.started < 400) continue;
        // A zero normal value alone is not evidence of a running operation.
        if (item.state == 2 && item.percent < 0) continue;
        bool browser = _wcsicmp(info.app.c_str(), L"chrome.exe") == 0 || _wcsicmp(info.app.c_str(), L"msedge.exe") == 0;
        if (browser && std::any_of(result.begin(), result.end(), [&](auto const& task) {
            auto source = metadata.find(task.window);
            return source != metadata.end() && source->second.pid == pid &&
                task.app == info.app && task.state == item.state && task.percent == item.percent;
        })) continue; // Chromium mirrors its aggregate download progress on multiple windows.
        result.push_back({item.window, item.epoch, info.app, info.title, item.percent, item.state, {}, {}});
        if (result.size() >= 32) break;
    }
    previous = result;
    return result;
}

bool IsUnfinishedPrivacyInterval(bool hasStart, uint64_t start, bool hasStop, uint64_t stop, uint64_t now) {
    return hasStart && start != 0 && start <= now && (!hasStop || stop < start);
}
// 0 = inactive, 1 = current interval, 2 = recently completed location request.
int PrivacyIntervalState(bool hasStart, uint64_t start, bool hasStop, uint64_t stop,
                         uint64_t now, uint64_t uptimeMs, bool location) {
    if (IsUnfinishedPrivacyInterval(hasStart, start, hasStop, stop, now) &&
        (now - start) / 10000 <= uptimeMs) return 1;
    if (location && hasStart && hasStop && start && stop >= start && stop <= now &&
        now - stop <= 150000000ULL) return 2;
    return 0;
}

uint64_t FileTimeValue(FILETIME value) {
    return (uint64_t(value.dwHighDateTime) << 32) | value.dwLowDateTime;
}
// Windows' activity timestamps are implementation details, not permission grants.
// Camera/location fallback accepts an unfinished interval; desktop entries also require a live process.
PrivacySnapshot ReadPrivacyActivity(bool includeMicrophone = true, bool includeCamera = true) {
    PrivacySnapshot result;
    FILETIME current{}; GetSystemTimeAsFileTime(&current);
    uint64_t now = FileTimeValue(current);
    auto running = RunningProcessNames();
    constexpr PCWSTR root = L"Software\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore\\";
    for (HKEY hive : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE})
    for (PCWSTR capability : {L"microphone", L"webcam", L"location"}) {
        if (!includeMicrophone && wcscmp(capability, L"microphone") == 0) continue;
        if (!includeCamera && wcscmp(capability, L"webcam") == 0) continue;
        std::wstring path = std::wstring(root) + capability;
        HKEY key = nullptr;
        if (RegOpenKeyExW(hive, path.c_str(), 0, KEY_READ | KEY_WOW64_64KEY, &key) != ERROR_SUCCESS) continue;
        result.available = true;
        auto readEntry = [&](HKEY parent, PCWSTR name, bool desktop) {
            HKEY app = nullptr;
            if (RegOpenKeyExW(parent, name, 0, KEY_READ, &app) != ERROR_SUCCESS) return;
            uint64_t start = 0, stop = 0; DWORD size = sizeof(start);
            bool hasStart = RegGetValueW(app, nullptr, L"LastUsedTimeStart", RRF_RT_REG_QWORD,
                nullptr, &start, &size) == ERROR_SUCCESS;
            size = sizeof(stop);
            bool hasStop = RegGetValueW(app, nullptr, L"LastUsedTimeStop", RRF_RT_REG_QWORD,
                nullptr, &stop, &size) == ERROR_SUCCESS;
            RegCloseKey(app);
            int interval = PrivacyIntervalState(hasStart, start, hasStop, stop, now,
                GetTickCount64(), wcscmp(capability, L"location") == 0);
            if (!interval) return;
            bool active = interval == 1;
            std::wstring label = name;
            if (desktop) {
                std::replace(label.begin(), label.end(), L'#', L'\\');
                size_t slash = label.find_last_of(L'\\');
                if (slash != std::wstring::npos) label.erase(0, slash + 1);
            }
            if (desktop && active && !running.contains(LowerText(label))) return;
            if (!hasStop && !desktop) return;
            if (label.empty()) label = L"Unknown application";
            auto old = std::find_if(result.uses.begin(), result.uses.end(), [&](auto const& use) {
                return use.capability == capability && _wcsicmp(use.app.c_str(), label.c_str()) == 0;
            });
            if (old == result.uses.end()) result.uses.push_back({capability, label, start, !active});
            else if (active) *old = {capability, label, start, false};
        };
        for (DWORD index = 0; index < 1024; ++index) {
            WCHAR name[512]; DWORD size = ARRAYSIZE(name);
            if (RegEnumKeyExW(key, index, name, &size, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
            if (_wcsicmp(name, L"NonPackaged") != 0) readEntry(key, name, false);
            else {
                HKEY desktop = nullptr;
                if (RegOpenKeyExW(key, name, 0, KEY_READ, &desktop) != ERROR_SUCCESS) continue;
                for (DWORD n = 0; n < 1024; ++n) {
                    WCHAR app[2048]; DWORD count = ARRAYSIZE(app);
                    if (RegEnumKeyExW(desktop, n, app, &count, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
                    readEntry(desktop, app, true);
                }
                RegCloseKey(desktop);
            }
        }
        RegCloseKey(key);
    }
    std::sort(result.uses.begin(), result.uses.end(), [](auto const& a, auto const& b) {
        return std::tie(a.capability, a.app, a.started) < std::tie(b.capability, b.app, b.started);
    });
    return result;
}

// Passive location-use signal. Never instantiates Geolocator or requests coordinates.
// WNF_LFS_STATE: ionescu007/wnfun, WellKnownWnfNames.py; payload is a Windows BOOL.
// NtQueryWnfStateData ABI: winsiderss/phnt, ntexapi.h. Unsupported states fail closed.
int ReadLocationUseSignal() {
    using Query = LONG (NTAPI*)(const uint64_t*, const GUID*, const void*, ULONG*, void*, ULONG*);
    static auto query = reinterpret_cast<Query>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQueryWnfStateData"));
    if (!query) return -1;
    constexpr uint64_t locationState = 0x41950821a3bc0875ULL;
    ULONG stamp = 0, size = sizeof(BOOL); BOOL active = FALSE;
    if (query(&locationState, nullptr, nullptr, &stamp, &active, &size) < 0 ||
        size != sizeof(BOOL) || (active != FALSE && active != TRUE)) return -1;
    return active ? 1 : 0;
}

LRESULT CALLBACK ProviderWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_POWERBROADCAST || message == WM_SETTINGCHANGE || message == WM_THEMECHANGED) {
        g_forcePowerRefresh = true;
        if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

void RunLiveProviderLoop(bool winrtReady) {
    using namespace winrt::Windows::Media::Control;
    auto wake = std::make_shared<ProviderWake>();
    if (!wake->event) { Wh_Log(L"Provider event creation failed: %lu", GetLastError()); return; }
    GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    GlobalSystemMediaTransportControlsSession session{nullptr};
    struct SessionSignals {
        std::atomic<bool> playing{false};
        std::atomic<uint64_t> started{0};
    };
    struct SessionEntry {
        GlobalSystemMediaTransportControlsSession session{nullptr};
        uint64_t id = 0;
        std::shared_ptr<SessionSignals> signals;
        GlobalSystemMediaTransportControlsSession::PlaybackInfoChanged_revoker playback;
        GlobalSystemMediaTransportControlsSession::MediaPropertiesChanged_revoker metadata;
    };
    std::vector<SessionEntry> sessions;
    std::vector<MediaSessionChoice> choices;
    uint64_t nextSessionId = 0;
    GlobalSystemMediaTransportControlsSessionManager::SessionsChanged_revoker sessionsToken;
    GlobalSystemMediaTransportControlsSessionManager::CurrentSessionChanged_revoker managerToken;
    GlobalSystemMediaTransportControlsSession::PlaybackInfoChanged_revoker playbackToken;
    GlobalSystemMediaTransportControlsSession::MediaPropertiesChanged_revoker metadataToken;
    GlobalSystemMediaTransportControlsSession::TimelinePropertiesChanged_revoker timelineToken;
    winrt::Windows::UI::ViewManagement::UISettings uiSettings{nullptr};
    winrt::Windows::UI::ViewManagement::UISettings::AdvancedEffectsEnabledChanged_revoker effectsToken;
    winrt::Windows::Devices::Power::Battery batteryDevice{nullptr};
    winrt::Windows::Devices::Power::Battery::ReportUpdated_revoker batteryToken;
    wf::IAsyncOperation<GlobalSystemMediaTransportControlsSessionManager> managerRequest{nullptr};
    wf::IAsyncOperation<GlobalSystemMediaTransportControlsSessionMediaProperties> propertiesRequest{nullptr};
    wf::IAsyncOperation<bool> actionRequest{nullptr};
    ULONGLONG managerDeadline = 0, propertiesDeadline = 0, actionDeadline = 0, propertiesRetry = 0;
    uint64_t generation = 0, propertiesGeneration = 0;
    MediaSnapshot media;
    BatterySnapshot battery;
    PrivacySnapshot privacy;
    CaptureMonitor capture(wake);
    CameraMonitor camera(wake);
    BatteryTools batteryTools;
    std::vector<TaskProgress> tasks;
    BatteryEstimateState batteryEstimate;
    ULONGLONG nextRetry = 0, nextRefresh = 0, nextPrivacy = 0, nextDevices = 0, nextMediaPoll = 0, nextSessionScan = 0;
    bool metadataDirty = true;
    int locationInUse = -1;
    WNDCLASSEXW wc{}; wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ProviderWindowProc; wc.hInstance = ThisModule();
    wc.lpszClassName = L"WindhawkLiveUpdateProvider";
    RegisterClassExW(&wc);
    HWND powerWindow = CreateWindowExW(0, wc.lpszClassName, L"", WS_POPUP,
        0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);
    std::vector<HPOWERNOTIFY> powerTokens;
    for (auto guid : {&GUID_ACDC_POWER_SOURCE, &GUID_BATTERY_PERCENTAGE_REMAINING}) {
        if (powerWindow) if (auto token = RegisterPowerSettingNotification(powerWindow, guid, DEVICE_NOTIFY_WINDOW_HANDLE))
            powerTokens.push_back(token);
    }
    HKEY privacyKey = nullptr, machinePrivacyKey = nullptr;
    HANDLE privacyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore",
        0, KEY_NOTIFY, &privacyKey);
    RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore",
        0, KEY_NOTIFY | KEY_WOW64_64KEY, &machinePrivacyKey);
    auto armPrivacy = [&] {
        if (!privacyEvent) return;
        ResetEvent(privacyEvent);
        for (HKEY* key : {&privacyKey, &machinePrivacyKey}) if (*key) {
            LSTATUS status = RegNotifyChangeKeyValue(*key, TRUE, REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,
                privacyEvent, TRUE);
            if (status != ERROR_SUCCESS) { RegCloseKey(*key); *key = nullptr; }
        }
    };
    armPrivacy();
    if (winrtReady) {
        try {
            uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
            effectsToken = uiSettings.AdvancedEffectsEnabledChanged(winrt::auto_revoke,
                [wake](auto const&, auto const&) { wake->Signal(kEffects); });
            batteryDevice = winrt::Windows::Devices::Power::Battery::AggregateBattery();
            batteryToken = batteryDevice.ReportUpdated(winrt::auto_revoke,
                [wake](auto const&, auto const&) { wake->Signal(kBatteryState); });
        } catch (...) {}
    }
    while (WaitForSingleObject(g_liveStopEvent, 0) != WAIT_OBJECT_0) {
        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        auto settings = GetSettings();
        const auto now = GetTickCount64();
        unsigned changes = wake->flags.exchange(0);
        bool forcePower = g_forcePowerRefresh.exchange(false);
        batteryTools.Tick(settings.enableTaskbarWidget && settings.enableBatteryWidget, forcePower);
        if (forcePower || changes & (kBatteryState | kEffects) || now >= nextRefresh) {
            battery = settings.enableTaskbarWidget && settings.enableBatteryWidget
                ? ReadBatterySnapshot(winrtReady, batteryEstimate) : BatterySnapshot{};
            // Theme choice controls effects; battery saver is not a mod-side veto.
            // AdvancedEffectsEnabled also changes with power saving, so it is not used here.
            bool allowed = true;
            HIGHCONTRASTW contrast{}; contrast.cbSize = sizeof(contrast);
            if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) &&
                (contrast.dwFlags & HCF_HIGHCONTRASTON)) allowed = false;
            if (g_effectsAllowed.exchange(allowed) != allowed) NotifyLiveUi();
            nextRefresh = now + 10000; // Recovery if a provider misses an event.
        }
        if (settings.enableTaskbarWidget && settings.enablePrivacyWidget && winrtReady) {
            if (forcePower || now >= nextDevices || (changes & kCaptureDevices)) {
                capture.Rebuild(); camera.Start(); nextDevices = now + 30000;
                static int lastCaptureStatus = -1;
                if (lastCaptureStatus != int(capture.Ready())) {
                    lastCaptureStatus = int(capture.Ready());
                    Wh_Log(L"Microphone capture provider: %s", capture.Ready() ? L"active" : L"using registry fallback");
                }
            }
        } else { capture.Clear(); camera.Stop(); }
        static uint64_t nativeLocationRevision = 0;
        auto labelRevision = g_nativeLocationRevision.load();
        bool nativeLocationChanged = labelRevision != nativeLocationRevision;
        nativeLocationRevision = labelRevision;
        int locationSignal = settings.enableTaskbarWidget && settings.enablePrivacyWidget ? ReadLocationUseSignal() : -1;
        bool locationChanged = locationSignal != locationInUse;
        locationInUse = locationSignal;
        if (nativeLocationChanged || locationChanged || (privacyEvent && WaitForSingleObject(privacyEvent, 0) == WAIT_OBJECT_0) || now >= nextPrivacy || forcePower || (changes & kCaptureState)) {
            armPrivacy(); // Rearm before reading; a concurrent change remains signaled.
            privacy = settings.enableTaskbarWidget && settings.enablePrivacyWidget
                ? ReadPrivacyActivity(!capture.Ready(), !camera.Ready()) : PrivacySnapshot{};
            if (settings.enableTaskbarWidget && settings.enablePrivacyWidget) {
                capture.AppendActive(privacy); camera.Append(privacy);
                NativeLocationLabel native;
                { std::lock_guard<std::mutex> lock(g_liveMutex); native = g_nativeLocation; }
                bool nativeActive = !native.apps.empty() && now >= native.observed && now - native.observed < 2500;
                if (nativeActive) {
                    // Use the shell's own app attribution, not the foreground window or permission grants.
                    std::erase_if(privacy.uses, [](auto const& use) { return use.capability == L"location"; });
                    size_t start = 0;
                    while (start < native.apps.size()) {
                        auto end = native.apps.find(L'\n', start);
                        auto app = native.apps.substr(start, end == std::wstring::npos ? end : end - start);
                        if (!app.empty()) privacy.uses.push_back({L"location", std::move(app), 0, false});
                        if (end == std::wstring::npos) break;
                        start = end + 1;
                    }
                    privacy.available = true;
                } else if (locationInUse == 0) {
                    std::erase_if(privacy.uses, [](auto const& use) { return use.capability == L"location" && !use.recent; });
                } else if (locationInUse == 1 && std::none_of(privacy.uses.begin(), privacy.uses.end(), [](auto const& use) {
                    return use.capability == L"location" && !use.recent;
                })) {
                    privacy.uses.push_back({L"location", L"Windows Location Service", 0, false});
                    privacy.available = true;
                }
                std::sort(privacy.uses.begin(), privacy.uses.end(), [](auto const& a, auto const& b) {
                    return std::tie(a.capability, a.app) < std::tie(b.capability, b.app);
                });
            }
            nextPrivacy = now + 2000; // Recovery for providers that do not update ConsentStore notifications.
        }
        tasks = ReadTaskProgress(settings, now);
        if (!settings.enableTaskbarWidget || !settings.enableMediaWidget || !winrtReady) {
            playbackToken.revoke(); metadataToken.revoke(); timelineToken.revoke(); managerToken.revoke(); sessionsToken.revoke();
            CancelOperation(propertiesRequest); CancelOperation(managerRequest); CancelOperation(actionRequest);
            session = nullptr; manager = nullptr; sessions.clear(); choices.clear(); media = {}; nextRetry = 0;
        } else if ((changes & (kMediaState | kMediaMetadata)) || managerRequest || propertiesRequest || actionRequest ||
                   now >= nextMediaPoll || (g_requestedMediaSession.load() && g_requestedMediaSession.load() != media.sessionId) || [&] {
                       std::lock_guard<std::mutex> lock(g_liveMutex); return g_mediaCommand.action != MediaAction::None;
                   }()) try {
            nextMediaPoll = now + 1000;
            if (!manager && !managerRequest && now >= nextRetry) {
                managerRequest = GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
                managerDeadline = now + 5000; nextRetry = now + 10000;
            }
            if (managerRequest) {
                if (managerRequest.Status() == wf::AsyncStatus::Completed) {
                    manager = managerRequest.GetResults(); managerRequest = nullptr;
                    managerToken = manager.CurrentSessionChanged(winrt::auto_revoke,
                        [wake](auto const&, auto const&) { wake->Signal(kMediaState | kMediaMetadata); });
                    sessionsToken = manager.SessionsChanged(winrt::auto_revoke,
                        [wake](auto const&, auto const&) { wake->Signal(kMediaState); });
                    changes |= kMediaState | kMediaMetadata;
                } else if (managerRequest.Status() != wf::AsyncStatus::Started || now >= managerDeadline)
                    CancelOperation(managerRequest);
            }
            if (manager) {
                auto systemCurrent = manager.GetCurrentSession();
                if ((changes & kMediaState) || now >= nextSessionScan) {
                    nextSessionScan = now + 1000;
                    std::vector<SessionEntry> live;
                    choices.clear();
                    for (auto const& item : manager.GetSessions()) {
                        if (live.size() >= 16) break;
                        try {
                            auto it = std::find_if(sessions.begin(), sessions.end(), [&](auto const& e) { return e.session == item; });
                            auto status = item.GetPlaybackInfo().PlaybackStatus();
                            bool playing = status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                            bool paused = status == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Paused;
                            SessionEntry entry;
                            if (it != sessions.end()) entry = std::move(*it);
                            else {
                                entry.session = item; entry.id = ++nextSessionId;
                                entry.signals = std::make_shared<SessionSignals>();
                                entry.signals->playing = playing;
                                if (playing && session) entry.signals->started = ++g_mediaActivitySerial;
                                auto signal = entry.signals;
                                entry.playback = item.PlaybackInfoChanged(winrt::auto_revoke,
                                    [wake, signal](auto const& sender, auto const&) noexcept {
                                        try {
                                            bool running = sender.GetPlaybackInfo().PlaybackStatus() ==
                                                GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                                            if (running && !signal->playing.exchange(running)) signal->started = ++g_mediaActivitySerial;
                                            else signal->playing = running;
                                            wake->Signal(kMediaState);
                                        } catch (...) { wake->Signal(kMediaState); }
                                    });
                                entry.metadata = item.MediaPropertiesChanged(winrt::auto_revoke,
                                    [wake, signal](auto const&, auto const&) noexcept {
                                        if (signal->playing) signal->started = ++g_mediaActivitySerial;
                                        wake->Signal(kMediaState);
                                    });
                            }
                            if (playing && !entry.signals->playing.exchange(playing)) entry.signals->started = ++g_mediaActivitySerial;
                            else entry.signals->playing = playing;
                            if (playing || paused) choices.push_back({entry.id, std::wstring(item.SourceAppUserModelId()), playing});
                            live.push_back(std::move(entry));
                        } catch (...) { /* A closing player must not reset the other sessions. */ }
                    }
                    sessions = std::move(live);
                }
                std::vector<MediaCandidate> candidates;
                uint64_t systemId = 0;
                for (auto const& entry : sessions) {
                    if (std::none_of(choices.begin(), choices.end(), [&](auto const& c) { return c.id == entry.id; })) continue;
                    candidates.push_back({entry.id, entry.signals->playing.load(), entry.signals->started.load()});
                    if (entry.session == systemCurrent) systemId = entry.id;
                }
                uint64_t requested = g_requestedMediaSession.load();
                uint64_t selectedId = ChooseMediaSession(candidates, systemId, requested, g_requestedMediaSerial.load());
                if (selectedId != requested) g_requestedMediaSession = 0;
                auto chosen = std::find_if(sessions.begin(), sessions.end(), [&](auto const& e) { return e.id == selectedId; });
                GlobalSystemMediaTransportControlsSession current = chosen == sessions.end() ? nullptr : chosen->session;
                if (current != session) {
                    playbackToken.revoke(); metadataToken.revoke(); timelineToken.revoke();
                    CancelOperation(propertiesRequest); CancelOperation(actionRequest);
                    session = current; media = {}; media.generation = ++generation; metadataDirty = true; propertiesRetry = 0;
                    if (session) {
                        playbackToken = session.PlaybackInfoChanged(winrt::auto_revoke,
                            [wake](auto const&, auto const&) { wake->Signal(kMediaState); });
                        metadataToken = session.MediaPropertiesChanged(winrt::auto_revoke,
                            [wake](auto const&, auto const&) { wake->Signal(kMediaMetadata); });
                        timelineToken = session.TimelinePropertiesChanged(winrt::auto_revoke,
                            [wake](auto const&, auto const&) { wake->Signal(kMediaState); });
                    }
                }
                if (session) {
                    auto info = session.GetPlaybackInfo(); auto controls = info.Controls();
                    media.playing = info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                    media.available = media.playing || info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Paused;
                    media.app = session.SourceAppUserModelId().c_str();
                    auto chosenId = std::find_if(sessions.begin(), sessions.end(), [&](auto const& e) { return e.session == session; });
                    media.sessionId = chosenId == sessions.end() ? 0 : chosenId->id;
                    media.playbackSerial = chosenId == sessions.end() ? 0 : chosenId->signals->started.load();
                    media.canSeek = controls.IsPlaybackPositionEnabled();
                    media.canPrevious = controls.IsPreviousEnabled(); media.canNext = controls.IsNextEnabled();
                    media.canToggle = controls.IsPlayPauseToggleEnabled() ||
                        (media.playing ? controls.IsPauseEnabled() : controls.IsPlayEnabled());
                    // Send commands before metadata work; no async operation blocks this loop.
                    if (actionRequest && (actionRequest.Status() != wf::AsyncStatus::Started || now >= actionDeadline)) {
                        bool accepted = actionRequest.Status() == wf::AsyncStatus::Completed && actionRequest.GetResults();
                        media.commandFailed = !accepted;
                        CancelOperation(actionRequest);
                    }
                    PendingMediaCommand command;
                    if (!actionRequest) {
                        std::lock_guard<std::mutex> lock(g_liveMutex);
                        command = std::exchange(g_mediaCommand, {});
                    }
                    if (command.generation == generation && command.action != MediaAction::None) {
                        media.commandFailed = false;
                        if (command.action == MediaAction::Previous && media.canPrevious) actionRequest = session.TrySkipPreviousAsync();
                        else if (command.action == MediaAction::Next && media.canNext) actionRequest = session.TrySkipNextAsync();
                        else if (command.action == MediaAction::Seek && media.canSeek) {
                            auto range = session.GetTimelineProperties();
                            int64_t minimum = range.MinSeekTime().count(), maximum = range.MaxSeekTime().count();
                            if (maximum > minimum) actionRequest = session.TryChangePlaybackPositionAsync(
                                SeekTargetTicks(range.StartTime().count(), minimum, maximum, command.position));
                        }
                        else if (command.action == MediaAction::PlayPause && media.canToggle) {
                            if (controls.IsPlayPauseToggleEnabled()) actionRequest = session.TryTogglePlayPauseAsync();
                            else if (media.playing) actionRequest = session.TryPauseAsync();
                            else actionRequest = session.TryPlayAsync();
                        }
                        actionDeadline = now + 3000;
                    }
                    media.commandPending = !!actionRequest;
                    if (changes & kMediaMetadata) {
                        metadataDirty = true; propertiesRetry = 0;
                        CancelOperation(propertiesRequest);
                    }
                    if (propertiesRequest) {
                        if (propertiesRequest.Status() == wf::AsyncStatus::Completed) {
                            auto properties = propertiesRequest.GetResults(); propertiesRequest = nullptr;
                            if (propertiesGeneration == generation) {
                                std::wstring title = properties.Title().c_str(), artist = properties.Artist().c_str();
                                if (title != media.title || artist != media.artist) {
                                    media.generation = ++generation;
                                    CancelOperation(actionRequest); media.commandPending = false;
                                }
                                media.title = std::move(title); media.artist = std::move(artist);
                                std::wstring key = media.app + L"\n" + media.title + L"\n" + media.artist;
                                {
                                    std::lock_guard<std::mutex> lock(g_liveMutex);
                                    if (key != g_artRequest || g_artGeneration != generation) {
                                        g_artRequest = key; g_artGeneration = generation;
                                        media.artwork.reset();
                                        if (g_artWake) SetEvent(g_artWake);
                                    }
                                }
                            }
                        } else if (propertiesRequest.Status() != wf::AsyncStatus::Started || now >= propertiesDeadline) {
                            CancelOperation(propertiesRequest); metadataDirty = true; propertiesRetry = now + 2000;
                        }
                    }
                    if (metadataDirty && !propertiesRequest && now >= propertiesRetry) {
                        propertiesRequest = session.TryGetMediaPropertiesAsync();
                        propertiesGeneration = generation; propertiesDeadline = now + 5000; metadataDirty = false;
                    }
                    auto timeline = session.GetTimelineProperties();
                    int64_t start = timeline.StartTime().count();
                    media.seekMin = std::max<int64_t>(0, (timeline.MinSeekTime().count() - start) / 10000000);
                    media.seekMax = std::max<int64_t>(0, (timeline.MaxSeekTime().count() - start) / 10000000);
                    media.canSeek = media.canSeek && media.seekMax > media.seekMin;
                    double position = double(timeline.Position().count() - start) / 10000000.0;
                    if (media.playing) {
                        double elapsed = std::chrono::duration<double>(winrt::clock::now() - timeline.LastUpdatedTime()).count();
                        auto playbackRate = info.PlaybackRate(); double rate = playbackRate ? playbackRate.Value() : 1;
                        if (elapsed >= 0 && elapsed < 86400 && std::isfinite(rate)) position += elapsed * rate;
                    }
                    media.durationSeconds = std::max<int64_t>(0, (timeline.EndTime().count() - start) / 10000000);
                    if (!std::isfinite(position)) position = 0;
                    position = std::clamp(position, 0.0, static_cast<double>(INT64_MAX / 10000000));
                    media.positionSeconds = static_cast<int64_t>(position);
                    if (media.durationSeconds) media.positionSeconds = std::min(media.positionSeconds, media.durationSeconds);
                }
            }
        } catch (...) {
            playbackToken.revoke(); metadataToken.revoke(); timelineToken.revoke(); managerToken.revoke(); sessionsToken.revoke();
            CancelOperation(propertiesRequest); CancelOperation(actionRequest); CancelOperation(managerRequest);
            session = nullptr; manager = nullptr; sessions.clear(); choices.clear(); media = {}; nextRetry = now + 10000;
        }
        bool changed = false, taskControlsChanged = false;
        {
            std::lock_guard<std::mutex> lock(g_liveMutex);
            auto const& previous = g_liveSnapshot.media;
            std::wstring mediaKey = media.app + L"\n" + media.title + L"\n" + media.artist;
            media.artwork = media.generation == g_artResultGeneration && mediaKey == g_artResultKey
                ? g_artResultPixels : nullptr;
            if (!media.available) {
                media.artwork.reset(); g_artResultPixels.reset(); g_artResultKey.clear();
            }
            changed = !(previous == media) || !(g_liveSnapshot.battery == battery) ||
                g_liveSnapshot.privacy.uses != privacy.uses || g_liveSnapshot.privacy.available != privacy.available ||
                g_liveSnapshot.mediaSessions != choices || g_liveSnapshot.tasks != tasks;
            taskControlsChanged = g_liveSnapshot.tasks.size() != tasks.size() || !std::equal(
                g_liveSnapshot.tasks.begin(), g_liveSnapshot.tasks.end(), tasks.begin(), [](auto const& a, auto const& b) {
                    return a.id == b.id && a.window == b.window && a.state == b.state;
                });
            g_liveSnapshot.media=media;g_liveSnapshot.battery=battery;g_liveSnapshot.privacy=privacy;
            g_liveSnapshot.mediaSessions=choices;g_liveSnapshot.tasks=tasks;
        }
        if (taskControlsChanged && g_taskControlWake) SetEvent(g_taskControlWake);
        if (changed) NotifyLiveUi();
        HANDLE events[] = {g_liveStopEvent, g_liveWakeEvent, wake->event, privacyEvent};
        DWORD timeout = managerRequest || propertiesRequest || actionRequest ? 20 :
            settings.enablePrivacyWidget && settings.enableTaskbarWidget ? 250 : 1000;
        // The 1s tick only interpolates elapsed playback and provides recovery.
        MsgWaitForMultipleObjects(privacyEvent ? 4 : 3, events, FALSE, timeout, QS_ALLINPUT);
    }
    playbackToken.revoke(); metadataToken.revoke(); timelineToken.revoke(); managerToken.revoke(); sessionsToken.revoke();
    capture.Clear(); capture.Release(); camera.Release();
    effectsToken.revoke(); batteryToken.revoke();
    CancelOperation(propertiesRequest); CancelOperation(actionRequest); CancelOperation(managerRequest);
    sessions.clear();
    session = nullptr; manager = nullptr; uiSettings = nullptr; batteryDevice = nullptr;
    // Releasing registered delegates also releases their shared wake references.
    while (wake.use_count() > 1) WaitForSingleObject(wake->event, 10);
    for (auto token : powerTokens) UnregisterPowerSettingNotification(token);
    if (powerWindow) DestroyWindow(powerWindow);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    if (privacyKey) RegCloseKey(privacyKey);
    if (machinePrivacyKey) RegCloseKey(machinePrivacyKey);
    if (privacyEvent) CloseHandle(privacyEvent);
}

DWORD WINAPI ArtworkThreadProc(void*) {
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        {
            using namespace winrt::Windows::Media::Control;
            using namespace winrt::Windows::Storage::Streams;
            GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
            HANDLE events[] = {g_liveStopEvent, g_artWake};
            while (WaitForMultipleObjects(2, events, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) {
                try {
                    std::wstring key; uint64_t generation;
                    { std::lock_guard<std::mutex> lock(g_liveMutex); key = g_artRequest; generation = g_artGeneration; }
                    if (!manager && !WaitProviderOperation(GlobalSystemMediaTransportControlsSessionManager::RequestAsync(), manager)) continue;
                    GlobalSystemMediaTransportControlsSession session{nullptr};
                    GlobalSystemMediaTransportControlsSessionMediaProperties properties{nullptr};
                    for (auto const& candidate : manager.GetSessions()) {
                        std::wstring app = candidate.SourceAppUserModelId().c_str();
                        if (key.rfind(app + L"\n", 0) != 0) continue;
                        GlobalSystemMediaTransportControlsSessionMediaProperties p{nullptr};
                        if (!WaitProviderOperation(candidate.TryGetMediaPropertiesAsync(), p) || !p) continue;
                        if (app + L"\n" + std::wstring(p.Title()) + L"\n" + std::wstring(p.Artist()) == key) {
                            session = candidate; properties = p; break;
                        }
                    }
                    if (!session || !properties) continue;
                    std::wstring actualKey = std::wstring(session.SourceAppUserModelId()) + L"\n" +
                        std::wstring(properties.Title()) + L"\n" + std::wstring(properties.Artist());
                    if (actualKey != key || !properties.Thumbnail()) continue;
                    IRandomAccessStreamWithContentType stream{nullptr};
                    if (!WaitProviderOperation(properties.Thumbnail().OpenReadAsync(), stream) || !stream ||
                        stream.Size() == 0 || stream.Size() > 4 * 1024 * 1024) continue;
                    DataReader reader(stream.GetInputStreamAt(0)); uint32_t loaded = 0;
                    if (!WaitProviderOperation(reader.LoadAsync(static_cast<uint32_t>(stream.Size())), loaded)) continue;
                    std::vector<uint8_t> bytes(loaded); reader.ReadBytes(bytes); reader.Close(); stream.Close();
                    auto pixels = DecodeImage(bytes, 256);
                    { std::lock_guard<std::mutex> lock(g_liveMutex);
                      if (g_artRequest == key && g_artGeneration == generation) {
                          g_artResultKey = key; g_artResultGeneration = generation;
                          g_artResultPixels = std::move(pixels);
                          auto const& current = g_liveSnapshot.media;
                          if (current.generation == generation && current.app + L"\n" + current.title + L"\n" + current.artist == key)
                              g_liveSnapshot.media.artwork = g_artResultPixels;
                      } }
                    NotifyLiveUi();
                } catch (...) {}
            }
        }
        winrt::uninit_apartment();
    } catch (...) {}
    return 0;
}

// Culture is fetched on demand; only the currently displayed page is retained.
struct CultureTopic { std::wstring id, title, category, articleUrl; };
struct CultureArticle {
    std::wstring id, title, text, source, imageSource, error;
    std::shared_ptr<ImagePixels> image;
};
struct CultureState {
    std::vector<CultureTopic> topics;
    CultureArticle article;
    bool loading = false, catalogLoading = false;
    std::wstring error;
    uint64_t revision = 0;
};
std::mutex g_cultureMutex;
CultureState g_culture;
std::wstring g_cultureRequest, g_cultureCatalogUrl;
std::atomic<uint64_t> g_cultureRequestId{0};
thread_local uint64_t t_cultureRequestId = 0;
HANDLE g_cultureWake = nullptr, g_cultureThread = nullptr;
std::mutex g_cultureThreadMutex;
std::vector<CultureTopic> BuiltinCultureTopics(bool vi) {
    std::vector<CultureTopic> topics{
        {L"Áo dài", L"Áo dài", L"Trang phục", L""},
        {L"Áo tứ thân", L"Áo tứ thân", L"Trang phục", L""},
        {L"Nón lá", L"Nón lá", L"Trang phục", L""},
        {L"Tết Nguyên Đán", L"Tết Nguyên Đán", L"Lễ Tết", L""},
        {L"Tết Trung thu", L"Tết Trung thu", L"Lễ Tết", L""},
        {L"Tết Đoan ngọ", L"Tết Đoan ngọ", L"Lễ Tết", L""},
        {L"Giỗ Tổ Hùng Vương", L"Giỗ Tổ Hùng Vương", L"Lễ Tết", L""},
        {L"Vu-lan", L"Vu Lan", L"Lễ Tết", L""},
        {L"Táo quân", L"Ông Công, ông Táo", L"Phong tục", L""},
        {L"Tín ngưỡng thờ cúng tổ tiên", L"Thờ cúng tổ tiên", L"Phong tục", L""},
        {L"Trầu cau", L"Trầu cau", L"Phong tục", L""},
        {L"Quan họ", L"Quan họ", L"Nghệ thuật", L""},
        {L"Ca trù", L"Ca trù", L"Nghệ thuật", L""},
        {L"Nhã nhạc cung đình Huế", L"Nhã nhạc cung đình Huế", L"Nghệ thuật", L""},
        {L"Múa rối nước", L"Múa rối nước", L"Nghệ thuật", L""},
        {L"Đờn ca tài tử", L"Đờn ca tài tử", L"Nghệ thuật", L""},
        {L"Tranh Đông Hồ", L"Tranh Đông Hồ", L"Làng nghề", L""},
        {L"Gốm Bát Tràng", L"Gốm Bát Tràng", L"Làng nghề", L""},
        {L"Bánh chưng", L"Bánh chưng", L"Ẩm thực", L""},
        {L"Bánh giầy", L"Bánh giầy", L"Ẩm thực", L""},
        {L"Phở", L"Phở", L"Ẩm thực", L""},
        {L"Cốm", L"Cốm", L"Ẩm thực", L""}
    };
    if (!vi) {
        static const PCWSTR titles[] = {
            L"Ao dai — traditional long dress",
            L"Four-panel dress",
            L"Conical hat",
            L"Vietnamese Lunar New Year",
            L"Mid-Autumn Festival",
            L"Double Fifth Festival",
            L"Hung Kings commemoration",
            L"Vu Lan Festival",
            L"Kitchen Gods",
            L"Ancestor worship",
            L"Betel and areca",
            L"Quan ho folk singing",
            L"Ca tru singing",
            L"Hue royal court music",
            L"Water puppetry",
            L"Southern amateur music",
            L"Dong Ho woodblock prints",
            L"Bat Trang pottery",
            L"Square sticky rice cake",
            L"Round sticky rice cake",
            L"Pho noodle soup",
            L"Green rice flakes"
        };
        for (size_t i = 0; i < topics.size(); ++i) {
            topics[i].title = titles[i];
            topics[i].category = i < 3 ? L"Clothing" : i < 8 ? L"Festivals" : i < 11 ? L"Traditions" :
                i < 16 ? L"Arts" : i < 18 ? L"Crafts" : L"Food";
        }
    }
    return topics;
}

template<class Operation, class T>
bool WaitCultureOperation(Operation const& operation, T& result, DWORD timeout = 5000) {
    const auto deadline = GetTickCount64() + timeout;
    try {
        while (operation.Status() == wf::AsyncStatus::Started) {
            if (g_cultureRequestId.load() != t_cultureRequestId || GetTickCount64() >= deadline ||
                WaitForSingleObject(g_liveStopEvent, 25) == WAIT_OBJECT_0) { operation.Cancel(); return false; }
        }
        if (g_cultureRequestId.load() != t_cultureRequestId || operation.Status() != wf::AsyncStatus::Completed) return false;
        result = operation.GetResults(); return true;
    } catch (...) { return false; }
}
std::vector<uint8_t> FetchHttps(winrt::Windows::Web::Http::HttpClient const& client,
                               std::wstring const& url, size_t limit) {
    using namespace winrt::Windows::Web::Http;
    using namespace winrt::Windows::Storage::Streams;
    if (url.size() > 2048 || url.rfind(L"https://", 0) != 0) throw std::runtime_error("HTTPS required");
    wf::Uri uri(url);
    if (!uri.UserName().empty() || !uri.Password().empty()) throw std::runtime_error("URL credentials rejected");
    HttpResponseMessage response{nullptr};
    if (!WaitCultureOperation(client.GetAsync(uri, HttpCompletionOption::ResponseHeadersRead), response, 6000) ||
        !response || !response.IsSuccessStatusCode()) throw std::runtime_error("HTTP request failed");
    auto length = response.Content().Headers().ContentLength();
    if (length && length.Value() > limit) throw std::runtime_error("Response too large");
    IInputStream stream{nullptr};
    if (!WaitCultureOperation(response.Content().ReadAsInputStreamAsync(), stream, 3000))
        throw std::runtime_error("Response stream unavailable");
    DataReader reader(stream); reader.InputStreamOptions(InputStreamOptions::Partial);
    std::vector<uint8_t> data;
    ULONGLONG deadline = GetTickCount64() + 10000;
    while (GetTickCount64() < deadline) {
        uint32_t count = 0;
        if (!WaitCultureOperation(reader.LoadAsync(8192), count, 3000)) throw std::runtime_error("Read cancelled");
        if (!count) { reader.Close(); response.Close(); return data; }
        if (data.size() + count > limit) throw std::runtime_error("Response too large");
        size_t offset = data.size(); data.resize(offset + count);
        reader.ReadBytes(winrt::array_view<uint8_t>(data.data() + offset, data.data() + data.size()));
    }
    throw std::runtime_error("Read timed out");
}
winrt::Windows::Data::Json::JsonObject FetchJson(winrt::Windows::Web::Http::HttpClient const& client,
                                               std::wstring const& url) {
    auto bytes = FetchHttps(client, url, 1024 * 1024);
    auto text = winrt::to_hstring(std::string_view(reinterpret_cast<char const*>(bytes.data()), bytes.size()));
    return winrt::Windows::Data::Json::JsonObject::Parse(text);
}
CultureState ReadCultureState() { std::lock_guard<std::mutex> lock(g_cultureMutex); return g_culture; }
DWORD WINAPI CultureThreadProc(void*);
void RequestCulture(std::wstring const& id) {
    if (g_shuttingDown) return;
    {
        std::lock_guard<std::mutex> lock(g_cultureThreadMutex);
        if (g_shuttingDown) return;
        if (g_cultureThread && WaitForSingleObject(g_cultureThread, 0) == WAIT_OBJECT_0) {
            CloseHandle(g_cultureThread); g_cultureThread = nullptr;
        }
        if (!g_cultureThread && g_cultureWake) g_cultureThread = CreateThread(nullptr, 0, CultureThreadProc, nullptr, 0, nullptr);
        if (!g_cultureThread) {
            { std::lock_guard<std::mutex> stateLock(g_cultureMutex);
              g_culture.error = PickText(UseVietnameseLabels(GetSettings()), L"Không khởi động được trình đọc", L"Unable to start reader");
              g_culture.loading = g_culture.catalogLoading = false; ++g_culture.revision; }
            NotifyLiveUi(); return;
        }
    }
    auto settings = GetSettings();
    {
        std::lock_guard<std::mutex> lock(g_cultureMutex);
        g_cultureRequest = id; g_cultureCatalogUrl = settings.cultureCatalogUrl;
        ++g_cultureRequestId;
        g_culture.loading = !id.empty(); g_culture.catalogLoading = id.empty();
        g_culture.error.clear();
        g_culture.article = {};
        ++g_culture.revision;
    }
    if (g_cultureWake) SetEvent(g_cultureWake);
    NotifyLiveUi();
}
DWORD WINAPI CultureThreadProc(void*) {
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        {
            using namespace winrt::Windows::Web::Http;
            using namespace winrt::Windows::Web::Http::Filters;
            HANDLE events[] = {g_liveStopEvent, g_cultureWake};
            while (WaitForMultipleObjects(2, events, FALSE, INFINITE) == WAIT_OBJECT_0 + 1) {
                const bool vi = UseVietnameseLabels(GetSettings());
                std::wstring id, catalog; uint64_t requestId;
                { std::lock_guard<std::mutex> lock(g_cultureMutex);
                  id = g_cultureRequest; catalog = g_cultureCatalogUrl; requestId = g_cultureRequestId; }
                HttpBaseProtocolFilter filter;
                filter.CacheControl().ReadBehavior(HttpCacheReadBehavior::NoCache);
                filter.CacheControl().WriteBehavior(HttpCacheWriteBehavior::NoCache);
                filter.CookieUsageBehavior(HttpCookieUsageBehavior::NoCookies);
                filter.AllowUI(false);
                HttpClient client(filter);
                client.DefaultRequestHeaders().UserAgent().ParseAdd(L"LiveUpdateForWindows11/2.4 (Windhawk; culture reader)");
                CultureArticle article; article.id = id;
                std::vector<CultureTopic> topics;
                std::wstring error;
                try {
                    t_cultureRequestId = requestId;
                    if (id.empty()) {
                        if (catalog.empty()) topics = BuiltinCultureTopics(vi);
                        else {
                            auto json = FetchJson(client, catalog);
                            auto entries = json.GetNamedArray(L"articles");
                            for (auto entry : entries) {
                                if (topics.size() >= 200) break;
                                auto value = entry.GetObject();
                                bool localized = !vi && value.GetNamedString(L"language", L"") != L"en";
                                CultureTopic topic{std::wstring(value.GetNamedString(L"id", L"")),
                                    std::wstring(value.GetNamedString(localized ? L"titleEn" : L"title", L"")),
                                    std::wstring(value.GetNamedString(localized ? L"categoryEn" : L"category", L"")),
                                    std::wstring(value.GetNamedString(localized ? L"articleUrlEn" : L"articleUrl", L""))};
                                if (!topic.id.empty() && topic.id.size() <= 200 && !topic.title.empty() &&
                                    topic.title.size() <= 200 && topic.articleUrl.rfind(L"https://", 0) == 0)
                                    topics.push_back(std::move(topic));
                            }
                            if (topics.empty()) throw std::runtime_error("Empty catalog");
                        }
                    } else {
                        {
                            std::wstring imageUrl;
                            if (catalog.empty()) {
                                std::wstring articleTitle = id;
                                if (!vi) {
                                    auto languageLinks = FetchJson(client, std::wstring(L"https://vi.wikipedia.org/w/api.php?action=query&format=json&formatversion=2&redirects=1&prop=langlinks&lllang=en&titles=") + wf::Uri::EscapeComponent(id).c_str());
                                    auto pages = languageLinks.GetNamedObject(L"query").GetNamedArray(L"pages");
                                    if (!pages.Size() || !pages.GetAt(0).GetObject().HasKey(L"langlinks"))
                                        throw std::runtime_error("English article unavailable");
                                    auto links = pages.GetAt(0).GetObject().GetNamedArray(L"langlinks");
                                    if (!links.Size()) throw std::runtime_error("English article unavailable");
                                    auto link = links.GetAt(0).GetObject();
                                    articleTitle = link.GetNamedString(L"title", link.GetNamedString(L"*", L""));
                                    if (articleTitle.empty()) throw std::runtime_error("English article unavailable");
                                }
                                auto escaped = wf::Uri::EscapeComponent(articleTitle);
                                auto json = FetchJson(client, std::wstring(vi ? L"https://vi.wikipedia.org" : L"https://en.wikipedia.org") +
                                    L"/w/api.php?action=query&format=json&formatversion=2&redirects=1&prop=extracts%7Cpageimages%7Cinfo&inprop=url&explaintext=1&piprop=thumbnail&pithumbsize=480&titles=" + escaped.c_str());
                                auto pages = json.GetNamedObject(L"query").GetNamedArray(L"pages");
                                if (!pages.Size()) throw std::runtime_error("Missing article");
                                auto page = pages.GetAt(0).GetObject();
                                article.title = page.GetNamedString(L"title", id);
                                article.text = page.GetNamedString(L"extract", L"");
                                article.source = page.GetNamedString(L"fullurl", L"");
                                if (page.HasKey(L"thumbnail")) imageUrl = page.GetNamedObject(L"thumbnail").GetNamedString(L"source", L"");
                                article.imageSource = article.source;
                            } else {
                                auto state = ReadCultureState();
                                auto topic = std::find_if(state.topics.begin(), state.topics.end(), [&](auto const& item) { return item.id == id; });
                                if (topic == state.topics.end()) throw std::runtime_error("Unknown topic");
                                auto json = FetchJson(client, topic->articleUrl);
                                article.title = json.GetNamedString(L"title", topic->title);
                                article.text = json.GetNamedString(L"content", L"");
                                article.source = json.GetNamedString(L"sourceUrl", L"");
                                article.imageSource = json.GetNamedString(L"imageSourceUrl", article.source);
                                imageUrl = json.GetNamedString(L"imageUrl", L"");
                            }
                            if (article.text.empty()) throw std::runtime_error("Empty article");
                            if (article.title.size() > 200) article.title.resize(200);
                            if (article.text.size() > 24000) article.text = article.text.substr(0, 24000) + L"\n…";
                            if (!imageUrl.empty()) try { article.image = DecodeImage(FetchHttps(client, imageUrl, 4 * 1024 * 1024), 640); } catch (...) {}

                        }
                    }
                } catch (...) {
                    error = PickText(vi, L"Không tải được nội dung. Kiểm tra kết nối hoặc thử lại.",
                        L"Unable to load an article in this language. Check your connection or choose another topic.");
                }
                { std::lock_guard<std::mutex> lock(g_cultureMutex);
                  if (requestId != g_cultureRequestId) continue;
                  if (id.empty() && error.empty()) g_culture.topics = std::move(topics);
                  if (!id.empty()) g_culture.article = std::move(article);
                  g_culture.error = error; g_culture.loading = false; g_culture.catalogLoading = false;
                  ++g_culture.revision; }
                client.Close(); filter.Close();
                NotifyLiveUi();
            }
        }
        winrt::uninit_apartment();
    } catch (...) {
        std::lock_guard<std::mutex> lock(g_cultureMutex);
        g_culture.loading = false; g_culture.catalogLoading = false;
        g_culture.error = L"HTTP provider unavailable / Không khởi tạo được kết nối";
        ++g_culture.revision;
        NotifyLiveUi();
    }
    return 0;
}

DWORD WINAPI LiveProviderThreadProc(void*) {
    bool initialized = false;
    try { winrt::init_apartment(winrt::apartment_type::multi_threaded); initialized = true; }
    catch (...) { Wh_Log(L"Media APIs unavailable; retaining Win32 battery information."); }
    try { RunLiveProviderLoop(initialized); }
    catch (...) { Wh_Log(L"Live provider stopped after an unexpected error."); }
    // All WinRT locals in RunLiveProviderLoop have already been released here.
    if (initialized) winrt::uninit_apartment();
    return 0;
}

// OBS WebSocket v5: obsproject/obs-websocket/docs/generated/protocol.md.
// Loopback only. Network, authentication and JSON parsing never run on the UI thread.
struct ObsInbox {
    std::mutex mutex;
    struct Message { std::wstring text; uint64_t receivedAt; };
    std::vector<Message> messages;
    std::atomic<bool> closed{false};
    HANDLE event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    ~ObsInbox() { if (event) CloseHandle(event); }
};
std::wstring ObsSha256(std::wstring const& text) {
    using namespace winrt::Windows::Security::Cryptography;
    auto hash = Core::HashAlgorithmProvider::OpenAlgorithm(Core::HashAlgorithmNames::Sha256());
    auto bytes = CryptographicBuffer::ConvertStringToBinary(text, BinaryStringEncoding::Utf8);
    return std::wstring(CryptographicBuffer::EncodeToBase64String(hash.HashData(bytes)));
}
bool WaitObsConnect(wf::IAsyncAction const& operation) {
    auto deadline = GetTickCount64() + 2500;
    while (operation.Status() == wf::AsyncStatus::Started) {
        if (WaitForSingleObject(g_liveStopEvent, 20) == WAIT_OBJECT_0 || GetTickCount64() > deadline) {
            operation.Cancel(); return false;
        }
    }
    if (operation.Status() != wf::AsyncStatus::Completed) return false;
    operation.GetResults(); return true;
}
ObsSnapshot ObserveObsReceipt(ObsSnapshot const& old, ObsSnapshot next, uint64_t now) {
    next.stoppedSerial = old.stoppedSerial; next.stoppedUntil = old.stoppedUntil;
    if (old.connected && next.connected && old.recording && !next.recording) {
        ++next.stoppedSerial; next.stoppedUntil = now + 5000;
    }
    if (next.recording) next.stoppedUntil = 0;
    return next;
}
void PublishObs(ObsSnapshot const& incoming) {
    ObsSnapshot value;
    bool changed;
    {
        std::lock_guard<std::mutex> lock(g_liveMutex);
        value = ObserveObsReceipt(g_obsSnapshot, incoming, GetTickCount64());
        changed = !(g_obsSnapshot == value);
        g_obsSnapshot = value;
    }
    if (changed) NotifyLiveUi();
}
bool ApplyObsStatus(ObsSnapshot& state, bool active, bool paused, double duration, uint64_t& epoch, uint64_t sampledAt) {
    if (!std::isfinite(duration) || duration < 0 || duration > 31536000000.0) return false;
    if (active && !state.recording) state.generation = ++epoch;
    state.recording = active; state.paused = active && paused;
    state.elapsedMilliseconds = active ? static_cast<int64_t>(duration) : 0;
    state.elapsedSeconds = state.elapsedMilliseconds / 1000;
    state.sampledAt = sampledAt;
    return true;
}
void RunObsConnection(ModSettings const& settings, uint64_t& epoch) {
    using namespace winrt::Windows::Networking::Sockets;
    using namespace winrt::Windows::Storage::Streams;
    using namespace winrt::Windows::Data::Json;
    auto inbox = std::make_shared<ObsInbox>();
    if (!inbox->event) return;
    MessageWebSocket socket;
    socket.Control().MessageType(SocketMessageType::Utf8);
    socket.Control().MaxMessageSize(65536);
    auto received = socket.MessageReceived(winrt::auto_revoke, [inbox](auto const&, auto const& args) {
        try {
            auto reader = args.GetDataReader(); reader.UnicodeEncoding(UnicodeEncoding::Utf8);
            if (reader.UnconsumedBufferLength() > 65536) { inbox->closed = true; SetEvent(inbox->event); return; }
            std::wstring text(reader.ReadString(reader.UnconsumedBufferLength()));
            { std::lock_guard<std::mutex> lock(inbox->mutex);
              if (inbox->messages.size() < 16) inbox->messages.push_back({std::move(text), GetTickCount64()}); else inbox->closed = true; }
        } catch (...) { inbox->closed = true; }
        SetEvent(inbox->event);
    });
    auto closed = socket.Closed(winrt::auto_revoke, [inbox](auto const&, auto const&) {
        inbox->closed = true; SetEvent(inbox->event);
    });
    try {
        if (!WaitObsConnect(socket.ConnectAsync(wf::Uri(L"ws://127.0.0.1:" + std::to_wstring(settings.obsPort)))))
            throw winrt::hresult_error(E_FAIL);
        DataWriter writer(socket.OutputStream()); writer.UnicodeEncoding(UnicodeEncoding::Utf8);
        auto send = [&](int opcode, JsonObject const& data) {
            JsonObject message; message.SetNamedValue(L"op", JsonValue::CreateNumberValue(opcode));
            message.SetNamedValue(L"d", data); writer.WriteString(message.Stringify());
            uint32_t stored = 0;
            if (!WaitProviderOperation(writer.StoreAsync(), stored, 1500)) throw winrt::hresult_error(E_FAIL);
        };
        auto request = [&](PCWSTR type, std::wstring const& id) {
            JsonObject data; data.SetNamedValue(L"requestType", JsonValue::CreateStringValue(type));
            data.SetNamedValue(L"requestId", JsonValue::CreateStringValue(id)); send(6, data);
        };
        bool identified = false, hello = false;
        uint64_t started = GetTickCount64(), nextPoll = 0, statusDeadline = 0, commandDeadline = 0, sequence = 0;
        std::wstring pendingStatus, pendingCommand;
        uint64_t statusSent = 0, eventSerial = 0, statusSerial = 0;
        ObsSnapshot state; state.generation = ++epoch;
        while (!inbox->closed && WaitForSingleObject(g_liveStopEvent, 0) != WAIT_OBJECT_0) {
            auto current = GetSettings();
            if (!current.enableTaskbarWidget || !current.enableObsWidget || current.obsPort != settings.obsPort ||
                current.obsPassword != settings.obsPassword) break;
            auto now = GetTickCount64();
            if ((!identified && now - started > 5000) || (statusDeadline && now >= statusDeadline) ||
                (commandDeadline && now >= commandDeadline)) break;
            std::vector<ObsInbox::Message> messages;
            { std::lock_guard<std::mutex> lock(inbox->mutex); messages.swap(inbox->messages); }
            for (auto const& message : messages) {
                auto object = JsonObject::Parse(message.text); int op = static_cast<int>(object.GetNamedNumber(L"op"));
                auto data = object.GetNamedObject(L"d");
                if (op == 0 && !hello) {
                    hello = true;
                    JsonObject identity; identity.SetNamedValue(L"rpcVersion", JsonValue::CreateNumberValue(1));
                    identity.SetNamedValue(L"eventSubscriptions", JsonValue::CreateNumberValue(64)); // Outputs only.
                    if (data.HasKey(L"authentication")) {
                        auto auth = data.GetNamedObject(L"authentication");
                        std::wstring salt(auth.GetNamedString(L"salt")), challenge(auth.GetNamedString(L"challenge"));
                        if (salt.size() > 1024 || challenge.size() > 1024) throw winrt::hresult_error(E_INVALIDARG);
                        auto secret = ObsSha256(settings.obsPassword + salt);
                        identity.SetNamedValue(L"authentication", JsonValue::CreateStringValue(ObsSha256(secret + challenge)));
                    }
                    send(1, identity);
                } else if (op == 2 && hello) {
                    identified = true; state.connected = true;
                } else if (op == 5 && identified && data.GetNamedString(L"eventType", L"") == L"RecordStateChanged") {
                    auto event = data.GetNamedObject(L"eventData");
                    bool active = event.GetNamedBoolean(L"outputActive", false);
                    auto output = event.GetNamedString(L"outputState", L"");
                    if (output == L"OBS_WEBSOCKET_OUTPUT_STARTING" || output == L"OBS_WEBSOCKET_OUTPUT_STOPPING") { nextPoll = 0; continue; }
                    state.elapsedMilliseconds = ObsElapsedMilliseconds(state, message.receivedAt);
                    state.sampledAt = message.receivedAt; ++eventSerial;
                    if (active && !state.recording) { state.generation = ++epoch; state.elapsedMilliseconds = 0; }
                    state.elapsedSeconds = state.elapsedMilliseconds / 1000;
                    state.recording = active;
                    if (output == L"OBS_WEBSOCKET_OUTPUT_PAUSED") state.paused = true;
                    else if (output == L"OBS_WEBSOCKET_OUTPUT_RESUMED" || output == L"OBS_WEBSOCKET_OUTPUT_STARTED") state.paused = false;
                    nextPoll = 0;
                } else if (op == 7 && identified) {
                    std::wstring id(data.GetNamedString(L"requestId"));
                    bool ok = data.GetNamedObject(L"requestStatus").GetNamedBoolean(L"result", false);
                    if (id == pendingStatus) {
                        pendingStatus.clear(); statusDeadline = 0;
                        if (!ok) throw winrt::hresult_error(E_FAIL);
                        auto result = data.GetNamedObject(L"responseData");
                        bool active = result.GetNamedBoolean(L"outputActive");
                        double duration = result.GetNamedNumber(L"outputDuration");
                        uint64_t rtt = message.receivedAt >= statusSent ? message.receivedAt - statusSent : 0;
                        uint64_t sampleTime = message.receivedAt - std::min<uint64_t>(rtt / 2, 250);
                        if (statusSerial != eventSerial) { nextPoll = 0; continue; }
                        if (!ApplyObsStatus(state, active, result.GetNamedBoolean(L"outputPaused"), duration, epoch, sampleTime))
                            throw winrt::hresult_error(E_INVALIDARG);
                    } else if (id == pendingCommand) {
                        pendingCommand.clear(); commandDeadline = 0; state.commandPending = false;
                        state.commandFailed = !ok; nextPoll = 0;
                    }
                }
            }
            if (identified) {
                ObsCommand command;
                { std::lock_guard<std::mutex> lock(g_liveMutex); command = g_obsCommand; g_obsCommand = {}; }
                if (command.action != ObsAction::None && command.generation == state.generation && state.recording && pendingCommand.empty()) {
                    pendingCommand = L"command-" + std::to_wstring(++sequence);
                    PCWSTR type = command.action == ObsAction::Stop ? L"StopRecord" :
                        command.action == ObsAction::Pause ? L"PauseRecord" : L"ResumeRecord";
                    request(type, pendingCommand); commandDeadline = now + 4000;
                    state.commandPending = true; state.commandFailed = false;
                }
                if (pendingStatus.empty() && now >= nextPoll) {
                    pendingStatus = L"status-" + std::to_wstring(++sequence);
                    statusSent = GetTickCount64(); statusSerial = eventSerial;
                    request(L"GetRecordStatus", pendingStatus); statusDeadline = statusSent + 4000;
                    nextPoll = now + (state.recording ? 1000 : 3000);
                }
            }
            PublishObs(state);
            HANDLE events[] = {g_liveStopEvent, inbox->event, g_obsWake};
            auto tick = GetTickCount64();
            DWORD wait = identified && pendingStatus.empty() ? static_cast<DWORD>(std::clamp<int64_t>(
                static_cast<int64_t>(nextPoll) - static_cast<int64_t>(tick), 1, 1000)) : 1000;
            WaitForMultipleObjects(3, events, FALSE, wait);
        }
        writer.DetachStream(); writer.Close();
    } catch (...) { /* Reconnect without exposing credentials or server messages in logs. */ }
    received.revoke(); closed.revoke();
    try { socket.Close(1000, L""); } catch (...) {}
    socket = nullptr;
    // In-flight delegates own inbox until they have completely returned from mod code.
    while (inbox.use_count() > 1) WaitForSingleObject(inbox->event, 10);
    PublishObs({});
}
DWORD WINAPI ObsThreadProc(void*) {
    bool initialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded); initialized = true;
        uint64_t epoch = 0;
        while (WaitForSingleObject(g_liveStopEvent, 0) != WAIT_OBJECT_0) {
            auto settings = GetSettings();
            if (settings.enableTaskbarWidget && settings.enableObsWidget) {
                auto processes = RunningProcessNames();
                if (processes.contains(L"obs64.exe") || processes.contains(L"obs32.exe")) {
                    try { RunObsConnection(settings, epoch); } catch (...) { PublishObs({}); }
                } else PublishObs({});
            } else PublishObs({});
            HANDLE events[] = {g_liveStopEvent, g_obsWake};
            if (WaitForMultipleObjects(2, events, FALSE, 5000) == WAIT_OBJECT_0) break;
        }
    } catch (...) {}
    if (initialized) winrt::uninit_apartment();
    return 0;
}
void QueueObsCommand(ObsAction action) {
    if (g_shuttingDown) return;
    { std::lock_guard<std::mutex> lock(g_liveMutex);
      if (!g_obsSnapshot.connected || !g_obsSnapshot.recording || g_obsSnapshot.commandPending || g_obsCommand.action != ObsAction::None) return;
      g_obsCommand = {action, g_obsSnapshot.generation}; }
    if (g_obsWake) SetEvent(g_obsWake);
}


// System display paths and mobile hotspot are read on their own MTA worker.
// Only active display targets are shown; opening Win+P alone is not a projection.
bool IsInternalDisplay(unsigned technology) {
    return technology == static_cast<unsigned>(DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL) ||
        technology == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
        technology == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED ||
        technology == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS;
}
DisplaySnapshot ReadDisplaySnapshot() {
    DisplaySnapshot result;
    for (int attempt = 0; attempt < 3; ++attempt) {
        UINT32 pathCount = 0, modeCount = 0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS ||
            pathCount > 128 || modeCount > 1024) return result;
        std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
        LONG status = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(), &modeCount, modes.data(), nullptr);
        if (status == ERROR_INSUFFICIENT_BUFFER) continue;
        if (status != ERROR_SUCCESS) return result;
        paths.resize(pathCount); result.available = true;
        using Source = std::tuple<LONG, DWORD, UINT32>;
        std::map<Source, unsigned> sources;
        for (auto const& path : paths) if (path.flags & DISPLAYCONFIG_PATH_ACTIVE)
            ++sources[{path.sourceInfo.adapterId.HighPart, path.sourceInfo.adapterId.LowPart, path.sourceInfo.id}];
        result.sourceCount = static_cast<unsigned>(sources.size());
        for (auto const& path : paths) {
            if (!(path.flags & DISPLAYCONFIG_PATH_ACTIVE)) continue;
            unsigned technology = static_cast<unsigned>(path.targetInfo.outputTechnology);
            if (IsInternalDisplay(technology)) { result.internalActive = true; continue; }
            DISPLAYCONFIG_TARGET_DEVICE_NAME name{};
            name.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
            name.header.size = sizeof(name); name.header.adapterId = path.targetInfo.adapterId;
            name.header.id = path.targetInfo.id;
            bool named = DisplayConfigGetDeviceInfo(&name.header) == ERROR_SUCCESS;
            if (named && name.outputTechnology == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_MIRACAST)
                technology = DISPLAYCONFIG_OUTPUT_TECHNOLOGY_MIRACAST;
            DisplayTarget target;
            target.id = std::to_wstring(path.targetInfo.adapterId.HighPart) + L":" +
                std::to_wstring(path.targetInfo.adapterId.LowPart) + L":" + std::to_wstring(path.targetInfo.id);
            if (named) target.name.assign(name.monitorFriendlyDeviceName,
                wcsnlen(name.monitorFriendlyDeviceName, ARRAYSIZE(name.monitorFriendlyDeviceName)));
            target.technology = technology;
            target.duplicate = sources[{path.sourceInfo.adapterId.HighPart, path.sourceInfo.adapterId.LowPart, path.sourceInfo.id}] > 1;
            result.targets.push_back(std::move(target));
        }
        std::sort(result.targets.begin(), result.targets.end(), [](auto const& a, auto const& b) { return a.id < b.id; });
        return result;
    }
    return result;
}
std::atomic<bool> g_hotspotStopRequested{false};
class HotspotReader {
    using Manager = winrt::Windows::Networking::NetworkOperators::NetworkOperatorTetheringManager;
    std::vector<Manager> managers;
    uint64_t enumerated = 0;
public:
    bool Stop() {
        using namespace winrt::Windows::Networking::NetworkOperators;
        for (auto const& manager : managers) try {
            if (manager.TetheringOperationalState() != TetheringOperationalState::On) continue;
            NetworkOperatorTetheringOperationResult result{nullptr};
            return WaitProviderOperation(manager.StopTetheringAsync(), result, 8000) && result &&
                result.Status() == TetheringOperationStatus::Success;
        } catch (...) { return false; }
        return false;
    }
    HotspotSnapshot Read(uint64_t now, bool refresh) {
        using namespace winrt::Windows::Networking::Connectivity;
        using namespace winrt::Windows::Networking::NetworkOperators;
        if (refresh || !enumerated || now - enumerated >= 30000) {
            enumerated = now;
            std::vector<Manager> next;
            try {
                auto profiles = NetworkInformation::GetConnectionProfiles();
                auto internet = NetworkInformation::GetInternetConnectionProfile();
                auto add = [&](ConnectionProfile const& profile) {
                    if (!profile || next.size() >= 8) return;
                    try { next.push_back(Manager::CreateFromConnectionProfile(profile)); } catch (...) {}
                };
                add(internet);
                for (auto const& profile : profiles) if (profile != internet) add(profile);
                // Keep a working manager during an upstream-network transition.
                if (!next.empty()) managers = std::move(next);
            } catch (...) {}
        }
        HotspotSnapshot result;
        for (auto const& manager : managers) {
            try {
                auto state = manager.TetheringOperationalState();
                if (state == TetheringOperationalState::Unknown) continue;
                HotspotSnapshot candidate;
                candidate.available = true;
                candidate.on = state == TetheringOperationalState::On;
                candidate.changing = state == TetheringOperationalState::InTransition;
                if (candidate.on) {
                    try { auto count = manager.ClientCount(); if (count <= 256) candidate.clients = static_cast<int>(count); } catch (...) {}
                    try { candidate.ssid = manager.GetCurrentAccessPointConfiguration().Ssid(); } catch (...) {}
                    if (candidate.ssid.size() > 128) candidate.ssid.resize(128);
                    try {
                        auto clients = manager.GetTetheringClients();
                        candidate.clientsAvailable = true;
                        for (auto const& client : clients) {
                            if (candidate.devices.size() >= 64) break;
                            HotspotClient item; item.mac = client.MacAddress();
                            for (auto const& host : client.HostNames()) {
                                std::wstring value(host.DisplayName()); if (value.size() > 128) value.resize(128);
                                if (host.Type() == winrt::Windows::Networking::HostNameType::DomainName && item.name.empty()) item.name = value;
                                else if (item.address.empty()) item.address = value;
                            }
                            candidate.devices.push_back(std::move(item));
                        }
                        std::sort(candidate.devices.begin(), candidate.devices.end(), [](auto const& a, auto const& b) { return a.mac < b.mac; });
                    } catch (...) {}

                    return candidate;
                }
                if (!result.available || candidate.changing) result = std::move(candidate);
            } catch (...) {}
        }
        return result; // An unavailable provider never masquerades as "Off".
    }
    void Reset() { managers.clear(); enumerated = 0; }
};
// Enumerate Classic and LE through their selectors, then merge Windows device
// properties by container. Never pair or connect a disconnected device.
int BluetoothPercent(wf::IInspectable const& object) {
    auto value = object.try_as<wf::IPropertyValue>();
    if (!value) return -1;
    int64_t number = -1;
    switch (value.Type()) {
        case wf::PropertyType::UInt8: number = value.GetUInt8(); break;
        case wf::PropertyType::UInt16: number = value.GetUInt16(); break;
        case wf::PropertyType::UInt32: number = value.GetUInt32(); break;
        case wf::PropertyType::Int16: number = value.GetInt16(); break;
        case wf::PropertyType::Int32: number = value.GetInt32(); break;
        default: break;
    }
    return number >= 0 && number <= 100 ? static_cast<int>(number) : -1;
}
BluetoothSnapshot ReadBluetoothBatteries() {
    using namespace winrt::Windows::Devices::Enumeration;
    using namespace winrt::Windows::Devices::Bluetooth;
    using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;
    BluetoothSnapshot result;
    std::vector<std::pair<DeviceInformation, bool>> endpoints;
    auto props = winrt::single_threaded_vector<winrt::hstring>({L"System.Devices.ContainerId",
        L"System.Devices.Aep.ContainerId", L"System.Devices.BatteryLife", L"System.Devices.Aep.IsConnected"});
    // A failed Classic query must not suppress a successful LE query (or vice versa).
    for (bool le : {false, true}) try {
        auto selector = le ? BluetoothLEDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected)
                          : BluetoothDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected);
        DeviceInformationCollection collection{nullptr};
        if (!WaitProviderOperation(DeviceInformation::FindAllAsync(selector, props, DeviceInformationKind::AssociationEndpoint), collection, 1500)) continue;
        result.available = true;
        for (auto const& item : collection) { if (endpoints.size() >= 32) break; endpoints.emplace_back(item, le); }
    } catch (...) {}
    std::map<std::wstring, int> levels;
    constexpr DEVPROPKEY driverLevel{{0x104ea319,0x6ee2,0x4701,{0xbd,0x47,0x8d,0xdb,0xf4,0x25,0xbb,0xe5}},2};
    constexpr DEVPROPKEY shellLevel{{0x49cd1f76,0x5626,0x4b17,{0xa4,0xe8,0x18,0xb4,0xaa,0x1a,0x22,0x13}},10};
    constexpr DEVPROPKEY containerKey{{0x8c7ed206,0x3f8a,0x4827,{0xb3,0xab,0xae,0x9e,0x1f,0xae,0xfc,0x6c}},2};
    // Enumerators avoid scanning every device class on the machine.
    for (PCWSTR enumerator : {L"BTHENUM", L"BTHLEDEVICE", L"BTHLE", L"HID"}) {
        HDEVINFO info = SetupDiGetClassDevsW(nullptr, enumerator, nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
        if (info == INVALID_HANDLE_VALUE) continue;
        SP_DEVINFO_DATA data{}; data.cbSize = sizeof(data);
        for (DWORD i = 0; i < 512 && SetupDiEnumDeviceInfo(info, i, &data); ++i) {
            if (g_shuttingDown) break;
            GUID container{}; DEVPROPTYPE type = 0; DWORD bytes = 0;
            if (!SetupDiGetDevicePropertyW(info, &data, &containerKey, &type, reinterpret_cast<PBYTE>(&container), sizeof(container), &bytes, 0) || type != DEVPROP_TYPE_GUID) continue;
            for (auto const& key : {driverLevel, shellLevel}) {
                DWORD value = 0;
                if (SetupDiGetDevicePropertyW(info, &data, &key, &type, reinterpret_cast<PBYTE>(&value), sizeof(value), &bytes, 0) && value <= 100 &&
                    ((type == DEVPROP_TYPE_BYTE && bytes == 1) || (type == DEVPROP_TYPE_UINT32 && bytes == 4))) {
                    levels[std::wstring(winrt::to_hstring(container))] = static_cast<int>(value); break;
                }
            }
        }
        SetupDiDestroyDeviceInfoList(info);
    }
    unsigned gattReads = 0;
    for (auto const& [device, le] : endpoints) try {
        if (g_shuttingDown) break;
        BluetoothDeviceBattery item; item.id = device.Id(); item.name = device.Name();
        auto values = device.Properties();
        if (values.HasKey(L"System.Devices.BatteryLife")) item.percent = BluetoothPercent(values.Lookup(L"System.Devices.BatteryLife"));
        for (PCWSTR key : {L"System.Devices.Aep.ContainerId", L"System.Devices.ContainerId"}) {
            if (!values.HasKey(key)) continue;
            auto value = values.Lookup(key).try_as<wf::IPropertyValue>();
            if (value && value.Type() == wf::PropertyType::Guid && value.GetGuid() != winrt::guid{}) {
                item.id = winrt::to_hstring(value.GetGuid());
                auto found = levels.find(item.id); if (item.percent < 0 && found != levels.end()) item.percent = found->second;
                break;
            }
        }
        // Standard Battery Service fallback, bounded and only while already connected.
        if (item.percent < 0 && le && gattReads++ < 4) {
            BluetoothLEDevice peer{nullptr};
            if (WaitProviderOperation(BluetoothLEDevice::FromIdAsync(device.Id()), peer, 500) && peer) {
                struct ClosePeer { BluetoothLEDevice& p; ~ClosePeer() { try { p.Close(); } catch (...) {} } } closePeer{peer};
                if (peer.ConnectionStatus() == BluetoothConnectionStatus::Connected) {
                    GattDeviceServicesResult services{nullptr};
                    if (WaitProviderOperation(peer.GetGattServicesForUuidAsync(GattServiceUuids::Battery(), BluetoothCacheMode::Cached), services, 500) &&
                        services.Status() == GattCommunicationStatus::Success) {
                        for (auto const& service : services.Services()) {
                            struct CloseService { GattDeviceService s; ~CloseService() { try { s.Close(); } catch (...) {} } } closeService{service};
                            GattCharacteristicsResult chars{nullptr};
                            if (!WaitProviderOperation(service.GetCharacteristicsForUuidAsync(GattCharacteristicUuids::BatteryLevel(), BluetoothCacheMode::Cached), chars, 500) ||
                                chars.Status() != GattCommunicationStatus::Success) continue;
                            for (auto const& characteristic : chars.Characteristics()) {
                                GattReadResult read{nullptr};
                                if (WaitProviderOperation(characteristic.ReadValueAsync(BluetoothCacheMode::Uncached), read, 500) &&
                                    read.Status() == GattCommunicationStatus::Success && read.Value().Length() == 1) {
                                    auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(read.Value());
                                    auto percent = reader.ReadByte(); if (percent <= 100) item.percent = percent;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (item.name.empty()) item.name = L"Bluetooth";
        if (item.name.size() > 128) item.name.resize(128);
        auto duplicate = std::find_if(result.devices.begin(), result.devices.end(), [&](auto const& old) { return old.id == item.id; });
        if (duplicate == result.devices.end()) result.devices.push_back(std::move(item));
        else if (duplicate->percent < 0) duplicate->percent = item.percent;
    } catch (...) {}
    std::sort(result.devices.begin(), result.devices.end(), [](auto const& a, auto const& b) { return a.id < b.id; });
    return result;
}

struct BluetoothAlertTracker {
    bool initialized = false;
    std::map<std::wstring, unsigned> thresholds;
    std::set<std::wstring> connected;
    BluetoothDeviceBattery Observe(BluetoothSnapshot const& state) {
        BluetoothDeviceBattery notice;
        if (!state.available) return notice;
        std::set<std::wstring> next;
        for (auto const& device : state.devices) {
            next.insert(device.id);
            auto& bits = thresholds[device.id];
            bool newlyConnected = !connected.contains(device.id);
            if (newlyConnected) bits = 0;
            bool low = false;
            for (auto [level, mask] : {std::pair{20, 1u}, std::pair{10, 2u}}) {
                if (device.percent >= level + 3) bits &= ~mask;
                if (device.percent >= 0 && device.percent <= level && !(bits & mask)) { bits |= mask; low = true; }
            }
            if (initialized && (newlyConnected || low)) notice = device;
        }
        std::erase_if(thresholds, [&](auto const& item) { return !next.contains(item.first); });
        connected = std::move(next); initialized = true;
        return notice;
    }
};

// Supported Windhawk 2 CLI sync. Never writes the engine's registry/INI files
// and never requests elevation. Windhawk 1.x retains local preferences instead.
std::atomic<bool> g_hostSyncRequested{false};
std::atomic<int> g_hostSyncStatus{0}; // 0 idle, 1 queued, 2 synced, 3 unavailable, 4 rejected
void NotifySettingsStatus(int status) {
    g_hostSyncStatus = status; ++g_settingsRevision;
}
std::wstring FindWindhawkCli() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snapshot == INVALID_HANDLE_VALUE) return {};
    MODULEENTRY32W module{}; module.dwSize = sizeof(module); std::wstring result;
    if (Module32FirstW(snapshot, &module)) do {
        if (_wcsicmp(module.szModule, L"windhawk.dll")) continue;
        std::wstring directory = module.szExePath;
        for (int level = 0; level < 6; ++level) {
            auto slash = directory.find_last_of(L"\\/"); if (slash == std::wstring::npos) break;
            directory.resize(slash);
            auto file = directory + L"\\windhawk-cli.exe";
            DWORD attributes = GetFileAttributesW(file.c_str());
            if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY)) { result = file; break; }
        }
        if (!result.empty()) break;
    } while (Module32NextW(snapshot, &module));
    CloseHandle(snapshot); return result;
}
struct HostSettingsSync {
    HANDLE process = nullptr;
    uint64_t deadline = 0;
    ~HostSettingsSync() { if (process) CloseHandle(process); }
    void Tick() {
        if (process) {
            if (WaitForSingleObject(process, 0) == WAIT_OBJECT_0) {
                DWORD code = 1; GetExitCodeProcess(process, &code); CloseHandle(process); process = nullptr;
                if (!g_hostSyncRequested) g_hostSyncInFlight = false;
                NotifySettingsStatus(code == 0 ? 2 : 4);
            } else if (GetTickCount64() >= deadline) {
                // Do not kill another executable. This process contains no mod callbacks.
                CloseHandle(process); process = nullptr; NotifySettingsStatus(4);
                g_hostSyncRequested = false; g_hostSyncInFlight = false;
            }
            return;
        }
        if (!g_hostSyncRequested.exchange(false) || g_shuttingDown) return;
        auto cli = FindWindhawkCli();
        if (cli.empty()) { g_hostSyncInFlight = false; NotifySettingsStatus(3); return; }
        auto settings = GetSettings();
        auto lang = settings.language == ModLanguage::Vietnamese ? L"vi" : settings.language == ModLanguage::English ? L"en" : L"auto";
        std::wstring command = L"\"" + cli + L"\" mod settings set " + std::wstring(WH_MOD_ID) +
            L" language=" + lang + L" widgetWidth=" + std::to_wstring(settings.widgetWidth) +
            L" expandedWidth=" + std::to_wstring(settings.expandedWidth);
        STARTUPINFOW startup{}; startup.cb = sizeof(startup); PROCESS_INFORMATION info{};
        if (!CreateProcessW(cli.c_str(), command.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
                nullptr, nullptr, &startup, &info)) { g_hostSyncInFlight = false; NotifySettingsStatus(4); return; }
        CloseHandle(info.hThread); process = info.hProcess; deadline = GetTickCount64() + 10000;
    }
};

// Physical load is cheap to sample. Enumerate processes only for an open RAM
// page or memory pressure, and retain five aggregate entries, never process handles.
MemorySnapshot SampleMemory(bool detail) {
    MemorySnapshot result;
    MEMORYSTATUSEX status{}; status.dwLength = sizeof(status);
    if (!GlobalMemoryStatusEx(&status)) return result;
    result.valid = true; result.percent = status.dwMemoryLoad;
    result.total = status.ullTotalPhys; result.available = status.ullAvailPhys;
    result.sampledAt = GetTickCount64();
    PROCESS_MEMORY_COUNTERS_EX own{};
    if (K32GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&own), sizeof(own)))
        result.explorerWorkingSet = own.WorkingSetSize;
    DWORD settingsPid = g_settingsHostPid.load();
    if (settingsPid) {
        HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, settingsPid);
        if (process) {
            PROCESS_MEMORY_COUNTERS_EX counts{};
            if (K32GetProcessMemoryInfo(process, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counts), sizeof(counts)))
                result.settingsWorkingSet = counts.WorkingSetSize;
            CloseHandle(process);
        }
    }
    auto settings = GetSettings();
    if (!detail && (!settings.enableTaskbarWidget || !settings.memoryAlerts || result.percent < 80)) return result;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return result;
    std::map<std::wstring, MemoryApplication> groups;
    PROCESSENTRY32W entry{}; entry.dwSize = sizeof(entry);
    unsigned visited = 0;
    if (Process32FirstW(snapshot, &entry)) do {
        if (++visited > 4096 || g_shuttingDown) break;
        HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
        if (!process) continue;
        PROCESS_MEMORY_COUNTERS_EX counts{};
        if (K32GetProcessMemoryInfo(process, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counts), sizeof(counts))) {
            auto key = LowerText(entry.szExeFile);
            if (entry.th32ProcessID == settingsPid) key = L"live-update-settings";
            auto& group = groups[key];
            group.name = key == L"live-update-settings" ? L"Live Update · Settings" : FriendlyAppName(entry.szExeFile);
            group.workingSet += counts.WorkingSetSize;
            group.privateBytes += counts.PrivateUsage; ++group.processes;
        }
        CloseHandle(process);
    } while (Process32NextW(snapshot, &entry));
    CloseHandle(snapshot);
    for (auto& [key, group] : groups) result.top.push_back(std::move(group));
    auto count = std::min<size_t>(detail ? 100 : 5, result.top.size());
    std::partial_sort(result.top.begin(), result.top.begin() + count, result.top.end(),
        [](auto const& a, auto const& b) { return a.workingSet > b.workingSet; });
    result.top.resize(count); result.top.shrink_to_fit();
    return result;
}
std::wstring MemorySize(uint64_t bytes) {
    wchar_t buffer[40];
    if (bytes >= 1024ull * 1024 * 1024) swprintf_s(buffer, L"%.1f GB", bytes / (1024.0 * 1024 * 1024));
    else swprintf_s(buffer, L"%.0f MB", bytes / (1024.0 * 1024));
    return buffer;
}
std::wstring MemoryTopText(MemorySnapshot const& state) {
    std::wstring text;
    size_t count = 0;
    for (auto const& app : state.top) {
        if (++count > 5) break;
        if (!text.empty()) text += L"\n";
        text += app.name + L"  ·  " + MemorySize(app.workingSet);
    }
    return text;
}

struct SystemWakeSignal {
    HANDLE event = nullptr;
    std::atomic<bool> networkChanged{true};
};
struct BluetoothChangeWatch {
    using Watcher = winrt::Windows::Devices::Enumeration::DeviceWatcher;
    std::array<Watcher,2> watchers{nullptr,nullptr};
    std::array<Watcher::Added_revoker,2> added;
    std::array<Watcher::Updated_revoker,2> updated;
    std::array<Watcher::Removed_revoker,2> removed;
    void Stop() {
        for(size_t i=0;i<watchers.size();++i) {
            added[i].revoke();updated[i].revoke();removed[i].revoke();
            if(watchers[i]) {try{watchers[i].Stop();}catch(...){} watchers[i]=nullptr;}
        }
    }
    ~BluetoothChangeWatch(){Stop();}
    void Start(std::shared_ptr<SystemWakeSignal> const& signal) {
        using namespace winrt::Windows::Devices::Enumeration;
        using namespace winrt::Windows::Devices::Bluetooth;
        auto changed=[signal](auto const&,auto const&) noexcept {g_bluetoothDirty=true;SetEvent(signal->event);};
        for(size_t i=0;i<2;++i) {
            if(watchers[i])continue;
            try {
                auto selector=i ? BluetoothLEDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected)
                    : BluetoothDevice::GetDeviceSelectorFromConnectionStatus(BluetoothConnectionStatus::Connected);
                auto properties=winrt::single_threaded_vector<winrt::hstring>({L"System.Devices.Aep.IsConnected",L"System.Devices.BatteryLife"});
                watchers[i]=DeviceInformation::CreateWatcher(selector,properties,DeviceInformationKind::AssociationEndpoint);
                added[i]=watchers[i].Added(winrt::auto_revoke,changed);
                updated[i]=watchers[i].Updated(winrt::auto_revoke,changed);
                removed[i]=watchers[i].Removed(winrt::auto_revoke,changed);
                watchers[i].Start();
            } catch(...) {added[i].revoke();updated[i].revoke();removed[i].revoke();watchers[i]=nullptr;}
        }
    }
};

// Bluetooth drivers may respond slowly. Keep discovery/GATT on its own worker
// so it cannot stall memory sampling, hotspot commands or settings synchronization.
DWORD WINAPI BluetoothProviderThreadProc(void*) {
    bool initialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded); initialized = true;
        auto signal = std::make_shared<SystemWakeSignal>(); signal->event = g_bluetoothWake;
        struct CallbackGuard {
            std::shared_ptr<SystemWakeSignal>& signal;
            ~CallbackGuard() { while (signal.use_count() > 1) WaitForSingleObject(signal->event, 10); }
        } guard{signal};
        BluetoothChangeWatch watcher;
        BluetoothSnapshot snapshot;
        uint64_t readAt = 0, goodAt = 0;
        while (WaitForSingleObject(g_liveStopEvent, 0) != WAIT_OBJECT_0) {
            auto settings = GetSettings(); auto now = GetTickCount64();

            bool enabled = settings.enableTaskbarWidget && settings.enableBatteryWidget && settings.bluetoothBattery;
            if (enabled) {
                watcher.Start(signal);
                // Coalesce bursts from the device watcher. Disabling the feature
                // still takes effect immediately; recovery reads are every 15 s.
                if (!readAt || now - readAt >= 15000 || (g_bluetoothDirty.load() && now - readAt >= 1000)) {
                    g_bluetoothDirty = false;
                    auto value = ReadBluetoothBatteries(); readAt = GetTickCount64();
                    if (value.available) { snapshot = std::move(value); goodAt = readAt; }
                    else { snapshot.available = false; if (readAt - goodAt > 30000) snapshot.devices.clear(); }
                }
            } else { watcher.Stop(); snapshot = {}; readAt = 0; }
            bool changed;
            { std::lock_guard<std::mutex> lock(g_liveMutex);
              changed = !(g_bluetoothSnapshot == snapshot); g_bluetoothSnapshot = snapshot; }
            if (changed) NotifyLiveUi();
            HANDLE events[] = {g_liveStopEvent, g_bluetoothWake};
            WaitForMultipleObjects(2, events, FALSE, enabled ? (g_bluetoothDirty.load() ? 1000 : 15000) : INFINITE);
        }
    } catch (...) { Wh_Log(L"Bluetooth battery provider unavailable"); }
    if (initialized) winrt::uninit_apartment();
    return 0;
}

// Fixed volumes only: never wake removable drives or wait on network shares.
std::vector<StorageVolume> ReadStorageVolumes() {
    std::vector<StorageVolume> result;
    DWORD drives = GetLogicalDrives();
    DWORD oldMode = 0; SetThreadErrorMode(SEM_FAILCRITICALERRORS, &oldMode);
    for (unsigned i = 0; i < 26 && !g_shuttingDown; ++i) {
        if (!(drives & (1u << i))) continue;
        WCHAR root[] = L"A:\\"; root[0] += i;
        if (GetDriveTypeW(root) != DRIVE_FIXED) continue;
        ULARGE_INTEGER available{}, total{}, free{};
        if (!GetDiskFreeSpaceExW(root,&available,&total,&free) || !total.QuadPart) continue;
        WCHAR label[256]{};
        GetVolumeInformationW(root,label,ARRAYSIZE(label),nullptr,nullptr,nullptr,nullptr,0);
        result.push_back({root,label,total.QuadPart,std::min(total.QuadPart,free.QuadPart)});
    }
    SetThreadErrorMode(oldMode,nullptr);
    return result;
}

// Observe native session windows. Completed-session logs are not live state.
std::wstring UltraPeerFromTitle(std::wstring title) {
    constexpr std::wstring_view suffix = L" - UltraViewer";
    if (!title.ends_with(suffix)) return {};
    title.resize(title.size() - suffix.size());
    auto start = title.find_last_of(L'(');
    if (start == std::wstring::npos || start == 0 || title.back() != L')') return {};
    auto id = title.substr(start + 1, title.size() - start - 2);
    std::erase(id, L' ');
    if (id.size() < 5 || id.size() > 16 || id.find_first_not_of(L"0123456789") != std::wstring::npos) return {};
    return title;
}
class UltraConnectionReader {
    RemoteSnapshot latest;
    unsigned missing = 0;
public:
    void Reset() { latest = {}; missing = 0; }
    RemoteSnapshot Read(uint64_t now) {
        struct Scan { std::vector<std::wstring> peers; bool incoming = false; std::map<DWORD,bool> owners; } scan;
        EnumWindows([](HWND window, LPARAM parameter)->BOOL {
            auto& scan = *reinterpret_cast<Scan*>(parameter);
            if (!IsWindowVisible(window) || scan.peers.size() >= 16) return TRUE;
            WCHAR title[512]{}; GetWindowTextW(window,title,ARRAYSIZE(title));
            if (!wcsstr(title,L"UltraViewer")) return TRUE;
            DWORD pid = 0; GetWindowThreadProcessId(window,&pid);
            if (!scan.owners.contains(pid)) { auto exe = LowerText(ProcessName(pid)); scan.owners[pid] = exe == L"ultraviewer_desktop.exe" || exe == L"ultraviewer.exe"; }
            if (!scan.owners[pid]) return TRUE;
            auto peer = UltraPeerFromTitle(title);
            if (!peer.empty()) { scan.peers.push_back(std::move(peer)); return TRUE; }
            if (!std::wstring_view(title).starts_with(L"UltraViewer (")) return TRUE;
            struct Labels { bool incoming=false; unsigned count=0; } labels;
            EnumChildWindows(window,[](HWND child, LPARAM context)->BOOL {
                auto& labels=*reinterpret_cast<Labels*>(context);
                if (++labels.count > 128) return FALSE;
                WCHAR cls[32]{};GetClassNameW(child,cls,ARRAYSIZE(cls));
                if (_wcsicmp(cls,L"Static") || !IsWindowVisible(child)) return TRUE;
                WCHAR value[512]{};DWORD_PTR result=0;
                if (!SendMessageTimeoutW(child,WM_GETTEXT,ARRAYSIZE(value),reinterpret_cast<LPARAM>(value),SMTO_ABORTIFHUNG|SMTO_BLOCK,50,&result)) return TRUE;
                auto text=LowerText(value);
                if(text.find(L"đang xem máy tính của bạn")!=std::wstring::npos || text.find(L"máy tính đang xem bạn")!=std::wstring::npos ||
                   text.find(L"controlling your computer")!=std::wstring::npos || text.find(L"viewing your computer")!=std::wstring::npos) labels.incoming=true;
                return TRUE;
            },reinterpret_cast<LPARAM>(&labels));
            scan.incoming = scan.incoming || labels.incoming;
            return TRUE;
        },reinterpret_cast<LPARAM>(&scan));
        std::sort(scan.peers.begin(),scan.peers.end());
        scan.peers.erase(std::unique(scan.peers.begin(),scan.peers.end()),scan.peers.end());
        bool active = !scan.peers.empty() || scan.incoming;
        if (!active && latest.active && ++missing < 2) return latest;
        if (active) missing = 0;
        if (active != latest.active || (active && (scan.peers != latest.peers || scan.incoming != latest.incoming))) {
            ++latest.serial; latest.observedAt=now; latest.active=active;
            if (active) { latest.peers=std::move(scan.peers); latest.incoming=scan.incoming; }
        }
        return latest;
    }
};
std::wstring RemoteConnectionText(RemoteSnapshot const& remote,bool vi) {
    if (!remote.active) return PickText(vi,L"Đã ngắt kết nối",L"Disconnected");
    return remote.incoming ? PickText(vi,L"Máy này đang được điều khiển",L"This PC is being controlled") :
        PickText(vi,L"Đang điều khiển máy từ xa",L"Controlling a remote PC");
}
std::wstring RemotePeerText(RemoteSnapshot const& remote) {
    std::wstring text;
    for(auto const& peer:remote.peers) { if(!text.empty()) text+=L"\n"; text+=peer; }
    return text;
}

DWORD WINAPI SystemActivityThreadProc(void*) {
    bool initialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded); initialized = true;
        auto signal = std::make_shared<SystemWakeSignal>(); signal->event = g_systemWake;
        winrt::Windows::Networking::Connectivity::NetworkInformation::NetworkStatusChanged_revoker changed;
        struct SubscriptionGuard {
            decltype(changed)& subscription;
            std::shared_ptr<SystemWakeSignal>& signal;
            ~SubscriptionGuard() {
                subscription.revoke();
                while (signal.use_count() > 1) WaitForSingleObject(signal->event, 10);
            }
        } guard{changed, signal};
        try {
            changed = winrt::Windows::Networking::Connectivity::NetworkInformation::NetworkStatusChanged(
                winrt::auto_revoke, [signal](auto const&) {
                    signal->networkChanged = true; SetEvent(signal->event);
                });
        } catch (...) {}
        {
            HostSettingsSync settingsSync;
            HotspotReader hotspot;
            bool hotspotCommandFailed = false;
            uint64_t displayRead = 0, memoryRead = 0, storageRead = 0;
            UltraConnectionReader ultra; bool remoteVisible=false;
            DisplaySnapshot display;
            while (WaitForSingleObject(g_liveStopEvent, 0) != WAIT_OBJECT_0) {
                settingsSync.Tick();
                auto memoryNow = GetTickCount64();
                if (!memoryRead || memoryNow - memoryRead >= 5000) {
                    auto memory = SampleMemory(g_memoryDetailsRequested.load()); memoryRead = memoryNow;
                    { std::lock_guard<std::mutex> lock(g_liveMutex); g_memorySnapshot = std::move(memory); }
                    NotifyLiveUi();
                }
                auto settings = GetSettings(); auto now = GetTickCount64();
                if (g_memoryDetailsRequested && (!storageRead || now-storageRead >= 10000)) {
                    auto volumes=ReadStorageVolumes(); storageRead=now;
                    std::lock_guard<std::mutex> lock(g_liveMutex); g_storageVolumes=std::move(volumes);
                }
                RemoteSnapshot remote;
                if(settings.enableTaskbarWidget && settings.remoteAlerts) remote=ultra.Read(now); else ultra.Reset();
                bool visible=remote.serial && (remote.active || now-remote.observedAt<4500);
                bool remoteUpdated=visible!=remoteVisible;remoteVisible=visible;
                {std::lock_guard<std::mutex> lock(g_liveMutex);remoteUpdated=remoteUpdated||g_remoteSnapshot.serial!=remote.serial;g_remoteSnapshot=remote;}
                if (settings.enableTaskbarWidget && settings.enableDisplayWidget) {
                    if (g_displayDirty.exchange(false) || !displayRead || now - displayRead >= 5000) {
                        display = ReadDisplaySnapshot(); displayRead = now;
                    }
                } else { display = {}; displayRead = 0; }
                HotspotSnapshot network;
                if (settings.enableTaskbarWidget && settings.enableHotspotWidget)
                    network = hotspot.Read(now, signal->networkChanged.exchange(false));
                else hotspot.Reset();
                if (g_hotspotStopRequested.exchange(false)) {
                    if (settings.enableTaskbarWidget && settings.enableHotspotWidget) {
                        network.commandPending = true;
                        { std::lock_guard<std::mutex> lock(g_liveMutex); g_hotspotSnapshot = network; }
                        NotifyLiveUi();
                        hotspotCommandFailed = !hotspot.Stop();
                        network = hotspot.Read(GetTickCount64(), true);
                    }
                }
                if (network.available && !network.on && !network.changing) hotspotCommandFailed = false;
                network.commandFailed = hotspotCommandFailed;
                bool update;
                { std::lock_guard<std::mutex> lock(g_liveMutex);
                  update = !(g_displaySnapshot == display) || !(g_hotspotSnapshot == network);
                  g_displaySnapshot = display; g_hotspotSnapshot = network; }
                if (update || remoteUpdated) NotifyLiveUi();
                HANDLE events[] = {g_liveStopEvent, g_systemWake};
                WaitForMultipleObjects(2, events, FALSE,
                    settingsSync.process || g_hostSyncRequested ? 100 : settings.enableTaskbarWidget && (settings.enableHotspotWidget || settings.remoteAlerts) ? 1000 : 5000);
            }
        }

    } catch (...) { Wh_Log(L"System activity provider unavailable"); }
    if (initialized) winrt::uninit_apartment();
    return 0;
}

// Native file-operation controls are queried and invoked off the taskbar thread.
// No application is suspended or killed. Unsupported sources retain Show application.
struct AutomationText {
    BSTR value = nullptr;
    ~AutomationText() { SysFreeString(value); }
    std::wstring Text() const { return value ? std::wstring(value, SysStringLen(value)) : L""; }
};
TaskOperation OperationFromTask(TaskProgress const& task) {
    auto app = LowerText(task.app);
    if (app == L"chrome.exe" || app == L"msedge.exe" || app == L"firefox.exe" || app == L"brave.exe") return TaskOperation::Download;
    if (app == L"explorer.exe") return TaskOperation::Files;
    if (app == L"7zfm.exe" || app == L"7zg.exe") return TaskOperation::Archive;
    return TaskOperation::Generic;
}
TaskAction ActionFromButtonName(std::wstring name) {
    name = LowerText(std::move(name)); std::erase(name, L'&');
    auto first = name.find_first_not_of(L" \t\r\n"), last = name.find_last_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return TaskAction::None;
    name = name.substr(first, last - first + 1);
    // Tooltip shortcut suffixes do not change the action.
    auto shortcut = name.find(L" ("); if (shortcut != std::wstring::npos) name.resize(shortcut);
    if (name == L"pause" || name == L"pause transfer" || name == L"pause download" || name == L"tạm dừng" ||
        name == L"tạm ngừng" || name == L"tạm dừng tải xuống") return TaskAction::Pause;
    if (name == L"resume" || name == L"resume transfer" || name == L"resume download" || name == L"tiếp tục" ||
        name == L"tiếp tục tải xuống") return TaskAction::Resume;
    if (name == L"cancel" || name == L"cancel transfer" || name == L"cancel download" || name == L"hủy" ||
        name == L"huỷ" || name == L"hủy tải xuống" || name == L"huỷ tải xuống") return TaskAction::Cancel;
    return TaskAction::None;
}
unsigned TaskActionMask(TaskAction action) {
    return action == TaskAction::Pause ? 1 : action == TaskAction::Resume ? 2 : action == TaskAction::Cancel ? 4 : 0;
}
bool TaskStillActive(TaskProgress const& task) {
    HWND window = reinterpret_cast<HWND>(task.window);
    if (!IsWindow(window)) return false;
    std::lock_guard<std::mutex> lock(g_taskProgressMutex);
    auto found = g_rawTaskProgress.find(task.window);
    return found != g_rawTaskProgress.end() && found->second.epoch == task.id && found->second.state != 0;
}
thread_local uint64_t g_automationDeadline = 0;
bool AutomationBudgetExpired() { return g_shuttingDown || (g_automationDeadline && GetTickCount64() >= g_automationDeadline); }
std::wstring UiaName(IUIAutomationElement* element) {
    AutomationText value; if (element && !AutomationBudgetExpired()) element->get_CurrentName(&value.value); return value.Text();
}
std::vector<winrt::com_ptr<IUIAutomationElement>> UiaChildrenOfType(IUIAutomation* automation, IUIAutomationElement* root, int typeId) {
    std::vector<winrt::com_ptr<IUIAutomationElement>> result;
    if (AutomationBudgetExpired()) return result;
    VARIANT type{}; type.vt = VT_I4; type.lVal = typeId;
    winrt::com_ptr<IUIAutomationCondition> condition; winrt::com_ptr<IUIAutomationElementArray> elements;
    if (FAILED(automation->CreatePropertyCondition(UIA_ControlTypePropertyId, type, condition.put())) ||
        FAILED(root->FindAll(TreeScope_Descendants, condition.get(), elements.put())) || !elements) return result;
    int count = 0; if (FAILED(elements->get_Length(&count)) || count > 512) return result;
    for (int i = 0; i < count && !AutomationBudgetExpired(); ++i) {
        winrt::com_ptr<IUIAutomationElement> element;
        if (SUCCEEDED(elements->GetElement(i, element.put())) && element) result.push_back(std::move(element));
    }
    return result;
}
bool PlausibleFileName(std::wstring const& name) {
    if (name.empty() || name.size() > 255 || name.find_first_of(L"\\/:*?\"<>|\r\n") != std::wstring::npos) return false;
    auto dot = name.find_last_of(L'.');
    return dot != std::wstring::npos && dot > 0 && dot + 1 < name.size() && name.size() - dot <= 16;
}
std::pair<TaskAction, std::wstring> DownloadActionName(std::wstring const& text) {
    auto lower = LowerText(text);
    static constexpr std::pair<PCWSTR, TaskAction> prefixes[] = {
        {L"pause ", TaskAction::Pause}, {L"resume ", TaskAction::Resume}, {L"cancel ", TaskAction::Cancel},
        {L"tạm dừng ", TaskAction::Pause}, {L"tiếp tục ", TaskAction::Resume}, {L"hủy ", TaskAction::Cancel}, {L"huỷ ", TaskAction::Cancel}};
    for (auto const& [prefix, action] : prefixes) if (lower.starts_with(prefix)) {
        auto file = text.substr(wcslen(prefix)); if (PlausibleFileName(file)) return {action, file};
    }
    return {TaskAction::None, L""};
}
TaskOperation FileOperationFromText(std::wstring text) {
    text = LowerText(std::move(text));
    if (text.find(L"copying") != std::wstring::npos || text.find(L"sao chép") != std::wstring::npos) return TaskOperation::Copy;
    if (text.find(L"moving") != std::wstring::npos || text.find(L"di chuyển") != std::wstring::npos || text.find(L"đang chuyển") != std::wstring::npos) return TaskOperation::Move;
    if (text.find(L"deleting") != std::wstring::npos || text.find(L"recycling") != std::wstring::npos ||
        text.find(L"đang xóa") != std::wstring::npos) return TaskOperation::Delete;
    return TaskOperation::Files;
}
bool IsDownloadPageUrl(std::wstring value) {
    value = LowerText(std::move(value));
    if (auto hash = value.find_first_of(L"?#"); hash != std::wstring::npos) value.resize(hash);
    if (!value.empty() && value.back() == L'/') value.pop_back();
    return value == L"chrome://downloads" || value == L"edge://downloads" ||
        value == L"brave://downloads" || value == L"about:downloads";
}
winrt::com_ptr<IUIAutomationElement> DownloadRoot(IUIAutomation* automation, IUIAutomationElement* window) {
    winrt::com_ptr<IUIAutomationTreeWalker> walker; automation->get_ControlViewWalker(walker.put());
    if (!walker) return nullptr;
    std::vector<std::pair<winrt::com_ptr<IUIAutomationElement>, unsigned>> pending;
    winrt::com_ptr<IUIAutomationElement> first; first.copy_from(window); pending.emplace_back(first, 0);
    for (size_t index = 0; index < pending.size() && index < 384 && !AutomationBudgetExpired(); ++index) {
        auto [element, depth] = pending[index]; int type = 0; element->get_CurrentControlType(&type);
        if (type == UIA_DocumentControlTypeId) continue;
        auto name = LowerText(UiaName(element.get()));
        if ((type == UIA_WindowControlTypeId || type == UIA_PaneControlTypeId || type == UIA_GroupControlTypeId) &&
            (name == L"lịch sử tải xuống gần đây" || name == L"recent download history" || name == L"nhật ký tải xuống gần đây" || name == L"nhật ký tải xuống" ||
             name == L"lịch sử tải xuống" || name == L"recent downloads" || name == L"download history" ||
             name == L"downloads" || name == L"tải xuống")) return element;
        // Verify the address control as well as its value. A browser Find box
        // containing "chrome://downloads" must not authorize reading a web page.
        if (type == UIA_EditControlTypeId || type == UIA_ComboBoxControlTypeId) {
            AutomationText automationId; element->get_CurrentAutomationId(&automationId.value);
            auto id = LowerText(automationId.Text());
            bool address = id == L"omnibox" || id == L"urlbar-input" || id == L"addresseditbox" ||
                name == L"address and search bar" || name == L"thanh địa chỉ và tìm kiếm" ||
                name == L"search or enter web address" || name == L"search or enter address" ||
                name == L"tìm kiếm hoặc nhập địa chỉ" || name == L"tìm kiếm hoặc nhập địa chỉ web";
            winrt::com_ptr<IUIAutomationValuePattern> pattern;
            if (address && SUCCEEDED(element->GetCurrentPatternAs(UIA_ValuePatternId, __uuidof(IUIAutomationValuePattern), pattern.put_void()))) {
                AutomationText value; pattern->get_CurrentValue(&value.value);
                if (IsDownloadPageUrl(value.Text())) {
                    auto documents = UiaChildrenOfType(automation, window, UIA_DocumentControlTypeId);
                    // Use the visible internal document, not the whole browser UI.
                    for (auto const& document : documents) {
                        BOOL offscreen = TRUE; document->get_CurrentIsOffscreen(&offscreen);
                        if (!offscreen) return document;
                    }
                    return nullptr;
                }
            }
        }
        if (depth >= 14) continue;
        winrt::com_ptr<IUIAutomationElement> child;
        if (FAILED(walker->GetFirstChildElement(element.get(), child.put()))) continue;
        while (child && pending.size() < 384) {
            pending.emplace_back(child, depth + 1); winrt::com_ptr<IUIAutomationElement> next;
            if (FAILED(walker->GetNextSiblingElement(child.get(), next.put()))) break;
            child = std::move(next);
        }
    }
    return nullptr;
}
std::wstring UiaId(IUIAutomationElement* element) {
    AutomationText value; if (!AutomationBudgetExpired()) element->get_CurrentAutomationId(&value.value); return LowerText(value.Text());
}
std::vector<HWND> TaskSourceWindows(TaskProgress const& task) {
    HWND origin = reinterpret_cast<HWND>(task.window); DWORD pid = 0; GetWindowThreadProcessId(origin, &pid);
    struct State { DWORD pid; HWND origin; bool browser; std::vector<HWND> windows; } state{pid, origin,
        OperationFromTask(task) == TaskOperation::Download, {origin}};
    EnumWindows([](HWND candidate, LPARAM parameter) -> BOOL {
        auto& state = *reinterpret_cast<State*>(parameter);
        if (state.windows.size() >= 12 || !IsWindowVisible(candidate) || candidate == state.origin) return TRUE;
        DWORD pid = 0; GetWindowThreadProcessId(candidate, &pid);
        WCHAR cls[96]{}; GetClassNameW(candidate, cls, ARRAYSIZE(cls));
        if (state.browser) {
            if (pid == state.pid && (wcsstr(cls, L"Chrome_WidgetWin") || !wcscmp(cls, L"MozillaWindowClass"))) state.windows.push_back(candidate);
        } else if (!wcscmp(cls, L"OperationStatusWindow") || !wcscmp(cls, L"#32770")) {
            if (pid == state.pid || LowerText(ProcessName(pid)) == L"explorer.exe") state.windows.push_back(candidate);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&state));
    return state.windows;
}
struct FileOperationRoot {
    winrt::com_ptr<IUIAutomationElement> element;
    HWND window = nullptr;
    TaskOperation operation = TaskOperation::Files;
    std::wstring filename;
};
FileOperationRoot DescribeFileOperation(IUIAutomation* automation, HWND window, bool details = true) {
    FileOperationRoot result; result.window = window;
    if (AutomationBudgetExpired() || FAILED(automation->ElementFromHandle(window, result.element.put())) || !result.element) return {};
    WCHAR cls[96]{}; GetClassNameW(window, cls, ARRAYSIZE(cls));
    bool native = !wcscmp(cls, L"OperationStatusWindow");
    result.operation = FileOperationFromText(UiaName(result.element.get()));
    if (native && !details) return result;
    std::set<std::wstring> possibleFiles;
    for (int type : {UIA_TextControlTypeId, UIA_HyperlinkControlTypeId}) {
        bool nextIsFile = false;
        for (auto const& text : UiaChildrenOfType(automation, result.element.get(), type)) {
            auto value = UiaName(text.get()); auto lower = LowerText(value);
            auto operation = FileOperationFromText(value); if (operation != TaskOperation::Files) result.operation = operation;
            if (nextIsFile && !value.empty() && value.size() <= 255) { result.filename = value; nextIsFile = false; }
            for (PCWSTR prefix : {L"name:", L"tên:", L"file name:", L"tên tệp:", L"tên tập tin:"}) if (lower.starts_with(prefix)) {
                auto file = value.substr(wcslen(prefix)); auto first = file.find_first_not_of(L" \t");
                if (first != std::wstring::npos && file.size() - first <= 255) result.filename = file.substr(first);
                else nextIsFile = true;
            }
            if (PlausibleFileName(value)) possibleFiles.insert(value);
            auto id = UiaId(text.get());
            if ((id == L"filename" || id == L"filenamevalue" || id == L"currentitemname") && !value.empty() && value.size() <= 255)
                result.filename = value;
        }
    }
    bool progress = !UiaChildrenOfType(automation, result.element.get(), UIA_ProgressBarControlTypeId).empty();
    // A generic Save/Open/confirmation dialog is never a file-operation target.
    if (!native && (!progress || result.operation == TaskOperation::Files)) return {};
    if (result.filename.empty() && possibleFiles.size() == 1) result.filename = *possibleFiles.begin();
    return result;
}
TaskAction FileButtonAction(IUIAutomationElement* element, unsigned state) {
    auto action = ActionFromButtonName(UiaName(element));
    auto id = UiaId(element);
    if (action == TaskAction::None) {
        AutomationText help; element->get_CurrentHelpText(&help.value);
        action = ActionFromButtonName(help.Text());
    }
    if (action == TaskAction::None && (id == L"pausebutton" || id == L"pause_button" || id == L"pauseresumebutton" || id == L"pause"))
        action = state == 8 ? TaskAction::Resume : TaskAction::Pause;
    if (id == L"resumebutton" || id == L"resume_button") action = TaskAction::Resume;
    if (id == L"cancelbutton" || id == L"cancel_button" || id == L"cancel" || id == L"2") action = TaskAction::Cancel;
    return action;
}
struct DownloadRow {
    winrt::com_ptr<IUIAutomationElement> element;
    std::wstring filename;
    bool active = false;
};
std::wstring DownloadFilenameElement(IUIAutomationElement* element) {
    auto value = UiaName(element);
    auto id = UiaId(element);
    if ((id == L"name" || id == L"filelink" || id == L"filename") && !value.empty() && value.size() <= 512 &&
        value.find_first_of(L"\r\n\\/") == std::wstring::npos && value.find(L'\u2026') == std::wstring::npos) return value;
    if (PlausibleFileName(value) && value.find(L'\u2026') == std::wstring::npos) return value;
    // Chromium exposes the unelided filename through the title/help text.
    AutomationText help; element->get_CurrentHelpText(&help.value);
    auto full = help.Text();
    return PlausibleFileName(full) && full.find(L'\u2026') == std::wstring::npos ? full : L"";
}
DownloadRow FindDownloadRow(IUIAutomation* automation, IUIAutomationElement* child, IUIAutomationTreeWalker* walker) {
    DownloadRow result;
    if (!walker) return result;
    winrt::com_ptr<IUIAutomationElement> parent; parent.copy_from(child);
    for (int depth = 0; depth < 7 && !AutomationBudgetExpired(); ++depth) {
        winrt::com_ptr<IUIAutomationElement> next;
        if (FAILED(walker->GetParentElement(parent.get(), next.put())) || !next) break;
        parent = std::move(next); int type = 0; parent->get_CurrentControlType(&type);
        if (type == UIA_DocumentControlTypeId || type == UIA_WindowControlTypeId) break;
        std::set<std::wstring> files;
        for (int t : {UIA_HyperlinkControlTypeId, UIA_TextControlTypeId}) {
            for (auto const& text : UiaChildrenOfType(automation, parent.get(), t)) {
                auto value = DownloadFilenameElement(text.get()); if (!value.empty()) files.insert(value);
            }
        }
        if (files.size() > 1) return {}; // Never climb into a multi-download list.
        if (files.size() == 1) {
            result.element = parent; result.filename = *files.begin();
            result.active = !UiaChildrenOfType(automation, parent.get(), UIA_ProgressBarControlTypeId).empty();
            if (!result.active) for(auto const& button:UiaChildrenOfType(automation,parent.get(),UIA_ButtonControlTypeId)) {
                auto action=ActionFromButtonName(UiaName(button.get()));
                if(action==TaskAction::Pause || action==TaskAction::Resume || action==TaskAction::Cancel) {result.active=true;break;}
            }
            if (result.active) return result;
        }
    }
    return result;
}
std::wstring DownloadRowFilename(IUIAutomation* automation, IUIAutomationElement* button, IUIAutomationTreeWalker* walker) {
    return FindDownloadRow(automation, button, walker).filename;
}
bool IsDownloadOverflow(IUIAutomationElement* button) {
    auto id = UiaId(button), name = LowerText(UiaName(button));
    return id == L"more-actions" || id == L"actionmenubutton" ||
        name == L"more actions" || name == L"thao tác khác" || name == L"các thao tác khác" ||
        name == L"tác vụ khác" || name == L"hành động khác";
}
// Read-only progress observation. No InvokePattern, BM_CLICK or transfer commands.
TaskControlState ProbeTaskControls(IUIAutomation* automation, TaskProgress const& task) {
    g_automationDeadline=GetTickCount64()+1500;
    TaskControlState result;result.window=task.window;result.operation=OperationFromTask(task);
    if(!automation || !TaskStillActive(task))return result;
    bool download=result.operation==TaskOperation::Download;
    if(!download && result.operation!=TaskOperation::Files)return result;
    std::vector<winrt::com_ptr<IUIAutomationElement>> roots;
    auto candidates=TaskSourceWindows(task);
    if(download) {
        for(HWND window:candidates) {
            if(AutomationBudgetExpired())break;
            winrt::com_ptr<IUIAutomationElement> root;
            if(FAILED(automation->ElementFromHandle(window,root.put())) || !root)continue;
            root=DownloadRoot(automation,root.get());
            if(root){roots.push_back(std::move(root));break;}
        }
    } else {
        std::vector<FileOperationRoot> valid;
        for(HWND window:candidates) {
            auto root=DescribeFileOperation(automation,window);if(!root.element)continue;
            if(window==reinterpret_cast<HWND>(task.window)){valid.clear();valid.push_back(std::move(root));break;}
            valid.push_back(std::move(root));
        }
        if(valid.size()!=1)return result;
        result.operation=valid.front().operation;result.filename=valid.front().filename;
        result.sourceWindow=reinterpret_cast<uintptr_t>(valid.front().window);
        roots.push_back(std::move(valid.front().element));
    }
    if(!roots.empty())result.sampledAt=GetTickCount64();
    std::map<std::wstring,TaskFileInfo> files;
    winrt::com_ptr<IUIAutomationTreeWalker> walker;automation->get_ControlViewWalker(walker.put());
    for(auto const& root:roots) {
        if(download)for(auto const& bar:UiaChildrenOfType(automation,root.get(),UIA_ProgressBarControlTypeId)) {
            auto filename=DownloadRowFilename(automation,bar.get(),walker.get());
            if(filename.empty() || (!files.contains(filename) && files.size()>=32))continue;
            auto& item=files[filename];item.filename=filename;
            winrt::com_ptr<IUIAutomationRangeValuePattern> range;
            if(SUCCEEDED(bar->GetCurrentPatternAs(UIA_RangeValuePatternId,__uuidof(IUIAutomationRangeValuePattern),range.put_void()))) {
                double value=0,minimum=0,maximum=0;
                if(SUCCEEDED(range->get_CurrentValue(&value)) && SUCCEEDED(range->get_CurrentMinimum(&minimum)) && SUCCEEDED(range->get_CurrentMaximum(&maximum)) &&
                    std::isfinite(value) && std::isfinite(minimum) && std::isfinite(maximum) && maximum>minimum && value>=minimum && value<=maximum)
                    item.percent=std::clamp(int((value-minimum)*100/(maximum-minimum)),0,100);
            }
        }
        for(auto const& button:UiaChildrenOfType(automation,root.get(),UIA_ButtonControlTypeId)) {
            BOOL offscreen=TRUE;button->get_CurrentIsOffscreen(&offscreen);if(offscreen)continue;
            auto label=UiaName(button.get());
            auto type=download?ActionFromButtonName(label):FileButtonAction(button.get(),task.state);
            std::wstring filename;
            if(download) {
                auto parsed=DownloadActionName(label);
                if(parsed.first!=TaskAction::None){type=parsed.first;filename=parsed.second;}
                if(IsDownloadOverflow(button.get())) {
                    auto row=FindDownloadRow(automation,button.get(),walker.get());
                    if(!row.active)continue;filename=row.filename;
                    type=task.state==8?TaskAction::Resume:TaskAction::Pause;
                }
                if(type==TaskAction::None)continue;
                if(filename.empty())filename=DownloadRowFilename(automation,button.get(),walker.get());
                if(filename.empty())continue;
            }
            if(type==TaskAction::None || (!files.contains(filename) && files.size()>=32))continue;
            auto& item=files[filename];item.filename=filename;item.controls|=TaskActionMask(type);
        }
    }
    for(auto& [filename,item]:files) {
        if((item.controls&3)==3)item.controls&=~(task.state==8?1u:2u);
        if(download)result.files.push_back(std::move(item));else result.controls=item.controls;
    }
    if(AutomationBudgetExpired() && result.files.empty())result.sampledAt=0;
    return result;
}

TaskOutcome ExplicitTaskOutcome(IUIAutomation* automation, TaskProgress const& task, std::wstring const& filename) {
    g_automationDeadline = GetTickCount64() + 1000;
    if (!automation || filename.empty() || g_shuttingDown) return TaskOutcome::None;
    winrt::com_ptr<IUIAutomationElement> root;
    if (FAILED(automation->ElementFromHandle(reinterpret_cast<HWND>(task.window), root.put())) || !root) return TaskOutcome::None;
    if (OperationFromTask(task) == TaskOperation::Download) { root = DownloadRoot(automation, root.get()); if (!root) return TaskOutcome::None; }
    winrt::com_ptr<IUIAutomationTreeWalker> walker; automation->get_ControlViewWalker(walker.put());
    if (!walker) return TaskOutcome::None;
    std::vector<winrt::com_ptr<IUIAutomationElement>> matches;
    for (int type : {UIA_HyperlinkControlTypeId, UIA_TextControlTypeId}) {
        for (auto const& element : UiaChildrenOfType(automation, root.get(), type))
            if (UiaName(element.get()) == filename) matches.push_back(element);
        if (!matches.empty()) break;
    }
    if (matches.size() != 1) return TaskOutcome::None;
    auto row = matches.front();
    for (int depth = 0; depth < 4; ++depth) {
        winrt::com_ptr<IUIAutomationElement> parent;
        if (FAILED(walker->GetParentElement(row.get(), parent.put())) || !parent) break;
        row = std::move(parent); bool cancelled = false, completed = false, otherFile = false;
        for (auto const& text : UiaChildrenOfType(automation, row.get(), UIA_TextControlTypeId)) {
            auto original = UiaName(text.get()); auto value = LowerText(original);
            if (PlausibleFileName(original) && original != filename) otherFile = true;
            cancelled |= value == L"cancelled" || value == L"canceled" || value == L"đã hủy" || value == L"đã huỷ";
            completed |= value == L"completed" || value == L"done" || value == L"đã hoàn thành" || value == L"hoàn tất";
        }
        if (otherFile) break;
        if (cancelled != completed) return cancelled ? TaskOutcome::Cancelled : TaskOutcome::Completed;
    }
    return TaskOutcome::None;
}

TaskOutcome EndedTaskOutcome(bool reportedFull, unsigned state) {
    return reportedFull && state == 2 ? TaskOutcome::Completed : TaskOutcome::None;
}
void PublishTaskNotice(std::wstring filename, TaskOutcome outcome) {
    if (outcome == TaskOutcome::None) return;
    { std::lock_guard<std::mutex> lock(g_liveMutex);
      g_taskNotice = {g_taskNotice.serial + 1, GetTickCount64() + 6000, std::move(filename), outcome}; }
    NotifyLiveUi();
}
// Read only download metadata, not browsing URLs, cookies or credentials.
// No copy of History is made and no database is ever opened read/write.
struct sqlite3;
struct sqlite3_stmt;
struct BrowserDownloadReader {
    HMODULE module = nullptr;
    bool reportedReadFailure = false;
    int (*open)(const char*, sqlite3**, int, const char*) = nullptr;
    int (*close)(sqlite3*) = nullptr;
    int (*prepare)(sqlite3*, const char*, int, sqlite3_stmt**, const char**) = nullptr;
    int (*step)(sqlite3_stmt*) = nullptr;
    int (*finalize)(sqlite3_stmt*) = nullptr;
    int (*limit)(sqlite3*,int,int) = nullptr;
    int (*busyTimeout)(sqlite3*,int) = nullptr;
    const void* (*text16)(sqlite3_stmt*, int) = nullptr;
    int64_t (*integer)(sqlite3_stmt*, int) = nullptr;
    void (*progress)(sqlite3*, int, int (*)(void*), void*) = nullptr;
    struct Cached { uint64_t sampled = 0; std::vector<TaskFileInfo> files; };
    std::map<std::wstring, Cached> cache;
    BrowserDownloadReader() {
        module = LoadLibraryExW(L"winsqlite3.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) return;
        open = reinterpret_cast<decltype(open)>(GetProcAddress(module, "sqlite3_open_v2"));
        close = reinterpret_cast<decltype(close)>(GetProcAddress(module, "sqlite3_close"));
        prepare = reinterpret_cast<decltype(prepare)>(GetProcAddress(module, "sqlite3_prepare_v2"));
        step = reinterpret_cast<decltype(step)>(GetProcAddress(module, "sqlite3_step"));
        finalize = reinterpret_cast<decltype(finalize)>(GetProcAddress(module, "sqlite3_finalize"));
        limit = reinterpret_cast<decltype(limit)>(GetProcAddress(module, "sqlite3_limit"));
        busyTimeout = reinterpret_cast<decltype(busyTimeout)>(GetProcAddress(module, "sqlite3_busy_timeout"));
        text16 = reinterpret_cast<decltype(text16)>(GetProcAddress(module, "sqlite3_column_text16"));
        integer = reinterpret_cast<decltype(integer)>(GetProcAddress(module, "sqlite3_column_int64"));
        progress = reinterpret_cast<decltype(progress)>(GetProcAddress(module, "sqlite3_progress_handler"));
    }
    ~BrowserDownloadReader() { if (module) FreeLibrary(module); }
    void ReadProfile(std::wstring const& path, std::vector<TaskFileInfo>& result, uint64_t deadline) {
        if (!open || !close || !prepare || !step || !finalize || !text16 || !integer || !progress || !limit) return;
        sqlite3* db = nullptr;
        // SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_PRIVATECACHE.
        int code = open(winrt::to_string(path).c_str(), &db, 0x1 | 0x8000 | 0x40000, nullptr);
        struct Close { sqlite3*& db; decltype(close) fn; ~Close() { if (db) fn(db); } } closer{db, close};
        if (code || !db) return;
        limit(db,0,65536); limit(db,1,4096);
        if (busyTimeout) busyTimeout(db, static_cast<int>(std::min<uint64_t>(60, deadline > GetTickCount64() ? deadline - GetTickCount64() : 0)));
        progress(db, 500, [](void* value) -> int { return g_shuttingDown || GetTickCount64() > *static_cast<uint64_t*>(value); }, &deadline);
        sqlite3_stmt* statement = nullptr;
        const char* sql = "SELECT target_path,current_path,received_bytes,total_bytes,state FROM "
            "(SELECT id,target_path,current_path,received_bytes,total_bytes,state FROM downloads ORDER BY id DESC LIMIT 128) WHERE state=0";
        int prepared = prepare(db, sql, -1, &statement, nullptr);
        if (prepared != 0) {
            if (!reportedReadFailure) { Wh_Log(L"Download metadata unavailable (SQLite %d); using accessible download rows", prepared); reportedReadFailure = true; }
            if (statement) finalize(statement);
            return;
        }
        struct Finalize { sqlite3_stmt* s; decltype(finalize) fn; ~Finalize() { fn(s); } } finalizer{statement, finalize};
        while (result.size() < 32 && step(statement) == 100 && GetTickCount64() <= deadline) {
            auto target = static_cast<PCWSTR>(text16(statement, 0));
            if (!target || !*target) continue; // Unconfirmed name: do not guess from a URL or temp filename.
            std::wstring filename(target); if (filename.size() > 32768) continue;
            auto slash = filename.find_last_of(L"\\/"); if (slash != std::wstring::npos) filename.erase(0, slash + 1);
            if (filename.empty() || filename.size() > 512) continue;
            std::erase_if(filename, [](wchar_t c) { return c < 32 || (c >= 0x202a && c <= 0x202e) || (c >= 0x2066 && c <= 0x2069); });
            auto received = integer(statement, 2), total = integer(statement, 3);
            TaskFileInfo file; file.filename = std::move(filename);
            if (total > 0 && received >= 0) file.percent = static_cast<int>(std::clamp(100.0 * received / total, 0.0, 100.0));
            result.push_back(std::move(file));
        }
    }
    std::vector<TaskFileInfo> Read(std::wstring const& app, int aggregatePercent) {
        PCWSTR suffix = app == L"chrome.exe" ? L"\\Google\\Chrome\\User Data" :
            app == L"msedge.exe" ? L"\\Microsoft\\Edge\\User Data" :
            app == L"brave.exe" ? L"\\BraveSoftware\\Brave-Browser\\User Data" : nullptr;
        if (!suffix) return {};
        auto& saved = cache[app]; auto now = GetTickCount64();
        if (!saved.sampled || now - saved.sampled >= (saved.files.empty() ? 10000u : 2000u)) {
            saved.sampled = now; saved.files.clear();
            WCHAR local[32768]; DWORD size = GetEnvironmentVariableW(L"LOCALAPPDATA", local, ARRAYSIZE(local));
            if (!size || size >= ARRAYSIZE(local)) return {};
            std::wstring root = std::wstring(local) + suffix;
            auto deadline = now + 200; // Bounded total SQL budget, never on the taskbar/UI thread.
            ReadProfile(root + L"\\Default\\History", saved.files, deadline);
            WIN32_FIND_DATAW entry{}; HANDLE search = FindFirstFileW((root + L"\\Profile *").c_str(), &entry);
            if (search != INVALID_HANDLE_VALUE) {
                unsigned profiles = 0;
                do {
                    if (++profiles > 16 || GetTickCount64() > deadline || g_shuttingDown) break;
                    if (entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                        ReadProfile(root + L"\\" + entry.cFileName + L"\\History", saved.files, deadline);
                } while (FindNextFileW(search, &entry));
                FindClose(search);
            }
        }
        auto files = saved.files;
        // Chromium publishes live aggregate progress more often than its History DB.
        // It is safe to use it for one file only; never assign aggregate % to each of several files.
        if (files.size() == 1 && aggregatePercent >= 0) files.front().percent = aggregatePercent;
        return files;
    }
};
void MergeBrowserDownloadNames(TaskControlState& state, std::vector<TaskFileInfo> files) {
    if (files.empty()) return;
    if (!state.files.empty()) {
        for (auto& live : state.files) {
            if (live.percent >= 0) continue;
            auto found = std::find_if(files.begin(), files.end(), [&](auto const& f) { return f.filename == live.filename; });
            if (found != files.end()) live.percent = found->percent;
        }
    } else state.files = std::move(files);
    state.operation = TaskOperation::Download;
    state.filename = state.files.size() == 1 ? state.files.front().filename : L"";
    state.controls = state.files.size() == 1 ? state.files.front().controls : 0;
    state.sampledAt = GetTickCount64();
}

void PreserveDownloadIdentity(TaskControlState& state, TaskControlState const& previous, TaskProgress const& task) {
    if (OperationFromTask(task) != TaskOperation::Download || !state.files.empty() || state.sampledAt ||
        !previous.sampledAt || previous.files.empty() || previous.window != task.window) return;
    if (GetTickCount64() - previous.sampledAt > 30000) return;
    state.files = previous.files; state.sampledAt = previous.sampledAt;
    state.filename = previous.filename; state.controls = previous.controls;
    for (auto& file : state.files) {
        file.percent = state.files.size() == 1 ? task.percent : -1;
        if (file.controls & 3) file.controls = (file.controls & ~3u) | (task.state == 8 ? 2u : 1u);
    }
    if (state.files.size() == 1) state.controls = state.files.front().controls;
}

DWORD WINAPI TaskControlThreadProc(void*) {
    bool initialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded); initialized = true;
        {
            winrt::com_ptr<IUIAutomation2> automation;
            if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER,
                    __uuidof(IUIAutomation2), automation.put_void()))) {
                automation->put_ConnectionTimeout(300); automation->put_TransactionTimeout(300);
                automation->put_AutoSetFocus(FALSE);
            }
            std::map<uint64_t, TaskProgress> observedTasks;
            BrowserDownloadReader downloads;
            while (WaitForSingleObject(g_liveStopEvent, 0) != WAIT_OBJECT_0) {
                std::vector<TaskProgress> tasks;
                { std::lock_guard<std::mutex> lock(g_liveMutex);
                  tasks = g_liveSnapshot.tasks; }
                auto settings = GetSettings();
                bool enabled = settings.enableTaskbarWidget && settings.enableTaskProgress;
                if (!enabled) tasks.clear();
                if (tasks.empty() || !settings.browserDownloadNames) downloads.cache.clear();
                std::vector<RawTaskProgress> ended;
                { std::lock_guard<std::mutex> lock(g_taskProgressMutex); ended.swap(g_endedTaskSignals); }
                for (auto const& task : tasks) observedTasks[task.id] = task;
                for (auto const& event : ended) {
                    auto known = observedTasks.find(event.epoch); if (!enabled || known == observedTasks.end()) continue;
                    TaskControlState control;
                    { std::lock_guard<std::mutex> lock(g_liveMutex); auto it = g_taskControls.find(event.epoch);
                      if (it != g_taskControls.end()) control = it->second; }
                    auto filename = control.filename;
                    if (filename.empty() && !control.files.empty()) {
                        filename = control.files.front().filename;
                        if (control.files.size() > 1) filename += L" +" + std::to_wstring(control.files.size() - 1);
                    }
                    auto outcome = EndedTaskOutcome(event.completeReported, event.state);
                    if (outcome != TaskOutcome::Completed && control.files.size() <= 1) {
                        auto explicitOutcome = ExplicitTaskOutcome(automation.get(), known->second, filename);
                        if (explicitOutcome != TaskOutcome::None) outcome = explicitOutcome;
                    }
                    PublishTaskNotice(OperationFromTask(known->second) == TaskOperation::Download ? L"" : filename, outcome);
                }
                for (auto const& task : tasks) {
                    if (WaitForSingleObject(g_liveStopEvent, 0) == WAIT_OBJECT_0) break;
                    TaskControlState state;
                    try { state=ProbeTaskControls(automation.get(),task); }
                    catch(...) {state.window=task.window;}
                    if (settings.browserDownloadNames && OperationFromTask(task) == TaskOperation::Download)
                        MergeBrowserDownloadNames(state, downloads.Read(LowerText(task.app), task.percent));
                    TaskControlState previous;
                    { std::lock_guard<std::mutex> lock(g_liveMutex); auto old = g_taskControls.find(task.id);
                      if (old != g_taskControls.end()) previous = old->second; }
                    if (!state.files.empty()) for (auto const& file : previous.files) {
                        if (std::none_of(state.files.begin(), state.files.end(), [&](auto const& current) { return current.filename == file.filename; }))
                            PublishTaskNotice(L"", ExplicitTaskOutcome(automation.get(), task, file.filename));
                    }
                    bool changed;
                    { std::lock_guard<std::mutex> lock(g_liveMutex);
                      auto old = g_taskControls.find(task.id);
                      if (old != g_taskControls.end()) {
                          if (state.filename.empty() && old->second.sourceWindow && IsWindow(reinterpret_cast<HWND>(old->second.sourceWindow))) state.filename = old->second.filename;
                          PreserveDownloadIdentity(state, old->second, task);
                      }
                      auto comparable = state;
                      if (old != g_taskControls.end()) comparable.sampledAt = old->second.sampledAt;
                      changed = old == g_taskControls.end() || !(old->second == comparable);
                      g_taskControls[task.id] = state; }
                    if (changed) NotifyLiveUi();
                }
                { std::lock_guard<std::mutex> lock(g_liveMutex);
                  std::erase_if(g_taskControls, [&](auto const& item) {
                      return std::none_of(tasks.begin(), tasks.end(), [&](auto const& task) { return task.id == item.first; });
                  }); }
                std::erase_if(observedTasks, [&](auto const& item) {
                    return std::none_of(tasks.begin(), tasks.end(), [&](auto const& task) { return task.id == item.first; });
                });
                HANDLE events[] = {g_liveStopEvent, g_taskControlWake};
                WaitForMultipleObjects(2, events, FALSE, tasks.empty() ? 2000 : 1000);
            }
        }
    } catch (...) { Wh_Log(L"Task controls unavailable"); }
    if (initialized) winrt::uninit_apartment();
    return 0;
}

int TemperatureDevice(std::wstring const& id) {
    if(id.starts_with(L"/intelcpu/") || id.starts_with(L"/amdcpu/")) return 1;
    if(id.starts_with(L"/gpu-nvidia/") || id.starts_with(L"/gpu-amd/") || id.starts_with(L"/gpu-intel/") || id.starts_with(L"/nvidiagpu/") || id.starts_with(L"/atigpu/")) return 2;
    return 0;
}
template<class Operation,class T>
bool WaitTemperature(Operation const& op,T& result,DWORD milliseconds) {
    auto deadline=GetTickCount64()+milliseconds;
    try {
        while(op.Status()==wf::AsyncStatus::Started) {
            if(WaitForSingleObject(g_liveStopEvent,20)==WAIT_OBJECT_0 || GetTickCount64()>=deadline) {op.Cancel();return false;}
        }
        if(op.Status()!=wf::AsyncStatus::Completed) return false;
        result=op.GetResults();return true;
    } catch(...) {return false;}
}
TemperatureSnapshot ReadTemperature(winrt::Windows::Web::Http::HttpClient const& client,int port) {
    using namespace winrt::Windows::Web::Http;
    using namespace winrt::Windows::Storage::Streams;
    using namespace winrt::Windows::Data::Json;
    HttpResponseMessage response{nullptr};
    if(!WaitTemperature(client.GetAsync(wf::Uri(L"http://127.0.0.1:"+std::to_wstring(port)+L"/data.json"),HttpCompletionOption::ResponseHeadersRead),response,700) || !response.IsSuccessStatusCode()) return {};
    constexpr size_t limit=256*1024;
    auto length=response.Content().Headers().ContentLength();
    if(length && length.Value()>limit) return {};
    IInputStream stream{nullptr};
    if(!WaitTemperature(response.Content().ReadAsInputStreamAsync(),stream,400)) return {};
    DataReader reader(stream);reader.InputStreamOptions(InputStreamOptions::Partial);
    std::vector<uint8_t> data;bool complete=false;auto deadline=GetTickCount64()+800;
    while(GetTickCount64()<deadline) {
        uint32_t loaded=0;if(!WaitTemperature(reader.LoadAsync(8192),loaded,200)) break;
        if(!loaded){complete=true;break;}
        if(data.size()+loaded>limit) break;
        auto start=data.size();data.resize(start+loaded);
        reader.ReadBytes(winrt::array_view<uint8_t>(data.data()+start,data.data()+data.size()));
    }
    reader.Close();response.Close();
    if(!complete || data.empty()) return {};
    auto root=JsonObject::Parse(winrt::to_hstring(std::string_view(reinterpret_cast<char const*>(data.data()),data.size())));
    TemperatureSnapshot result;std::vector<JsonObject> nodes{root};
    for(size_t i=0;i<nodes.size() && i<2048;++i) {
        auto const node=nodes[i];
        if(node.GetNamedString(L"Type",L"")==L"Temperature") {
            auto device=TemperatureDevice(LowerText(std::wstring(node.GetNamedString(L"SensorId",L""))));
            double value=-1;auto raw=node.GetNamedValue(L"RawValue",nullptr);
            if(raw && raw.ValueType()==JsonValueType::Number) value=raw.GetNumber();
            else {std::wstring formatted=node.GetNamedString(L"Value",L"").c_str();std::replace(formatted.begin(),formatted.end(),L',',L'.');
                if(formatted.find(L"°C")!=std::wstring::npos) value=wcstod(formatted.c_str(),nullptr);}
            if(device && std::isfinite(value) && value>0 && value<=150) {
                int& target=device==1 ? result.cpu : result.gpu;target=std::max(target,static_cast<int>(std::lround(value)));
            }
        }
        auto children=node.GetNamedArray(L"Children",nullptr);
        if(children) for(auto const& child:children) if(child.ValueType()==JsonValueType::Object && nodes.size()<2048) nodes.push_back(child.GetObject());
    }
    return result;
}
DWORD WINAPI TemperatureThreadProc(void*) {
    bool initialized=false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);initialized=true;
        {
            using namespace winrt::Windows::Web::Http;using namespace winrt::Windows::Web::Http::Filters;
            HttpBaseProtocolFilter filter;filter.AllowAutoRedirect(false);filter.AllowUI(false);filter.UseProxy(false);
            filter.CookieUsageBehavior(HttpCookieUsageBehavior::NoCookies);
            filter.CacheControl().ReadBehavior(HttpCacheReadBehavior::NoCache);filter.CacheControl().WriteBehavior(HttpCacheWriteBehavior::NoCache);
            HttpClient client(filter);uint64_t next=0;int lastPort=0;
            while(WaitForSingleObject(g_liveStopEvent,0)!=WAIT_OBJECT_0) {
                auto settings=GetSettings();auto now=GetTickCount64();bool enabled=settings.temperatureAlerts || g_memoryDetailsRequested;
                if(lastPort!=settings.temperaturePort){next=0;lastPort=settings.temperaturePort;}
                if(!enabled || now>=next) {
                    TemperatureSnapshot value;
                    if(enabled) try {value=ReadTemperature(client,settings.temperaturePort);}catch(...){}
                    bool changed;
                    {std::lock_guard<std::mutex> lock(g_liveMutex);changed=!(g_temperatureSnapshot==value);g_temperatureSnapshot=value;}
                    if(changed)NotifyLiveUi();
                    next=now+(value.cpu<0 && value.gpu<0 ? 30000 : 5000);
                }
                if(WaitForSingleObject(g_liveStopEvent,1000)==WAIT_OBJECT_0)break;
            }
        }
    }catch(...){}
    if(initialized)winrt::uninit_apartment();return 0;
}

int64_t ClockUtcMilliseconds(){FILETIME ft{};GetSystemTimeAsFileTime(&ft);return static_cast<int64_t>(FileTimeValue(ft)/10000);}
struct ClockWindow {HWND element=nullptr,top=nullptr;DWORD pid=0;};
bool IsWindowsClockProcess(DWORD pid) {
    HANDLE process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);if(!process)return false;
    WCHAR family[256]{};UINT32 size=ARRAYSIZE(family);
    LONG status=GetPackageFamilyName(process,&size,family);CloseHandle(process);
    return status==ERROR_SUCCESS && _wcsicmp(family,L"Microsoft.WindowsAlarms_8wekyb3d8bbwe")==0;
}
ClockWindow FindWindowsClock() {
    struct Scan {ClockWindow found;std::map<DWORD,bool> processes;unsigned windows=0;} scan;
    EnumWindows([](HWND window,LPARAM data)->BOOL {
        auto& scan=*reinterpret_cast<Scan*>(data);if(++scan.windows>512)return FALSE;
        if(!IsWindowVisible(window))return TRUE;
        DWORD pid=0;GetWindowThreadProcessId(window,&pid);
        auto [it,inserted]=scan.processes.try_emplace(pid,false);if(inserted)it->second=IsWindowsClockProcess(pid);
        if(it->second){scan.found={window,window,pid};return FALSE;}
        WCHAR type[64]{};GetClassNameW(window,type,ARRAYSIZE(type));
        if(_wcsicmp(type,L"ApplicationFrameWindow")!=0)return TRUE;
        struct Children {Scan* scan;HWND top;unsigned count=0;} children{&scan,window};
        EnumChildWindows(window,[](HWND child,LPARAM data)->BOOL {
            auto& c=*reinterpret_cast<Children*>(data);if(++c.count>64)return FALSE;
            DWORD pid=0;GetWindowThreadProcessId(child,&pid);
            auto [it,inserted]=c.scan->processes.try_emplace(pid,false);if(inserted)it->second=IsWindowsClockProcess(pid);
            if(it->second){c.scan->found={child,c.top,pid};return FALSE;}return TRUE;
        },reinterpret_cast<LPARAM>(&children));
        return scan.found.element?FALSE:TRUE;
    },reinterpret_cast<LPARAM>(&scan));
    return scan.found;
}
// Plain snapshots only: UI Automation references never reach the taskbar thread.
struct ClockNode {
    std::wstring id,name,value,runtimeKey;
    int parent=-1,type=0,toggle=-1;
    bool selected=false,enabled=false,offscreen=true;
    RECT rect{};
};
int ClockIntProperty(IUIAutomationElement* element,PROPERTYID property) {
    VARIANT value{};VariantInit(&value);int result=-1;
    if(SUCCEEDED(element->GetCachedPropertyValue(property,&value))) {
        if(value.vt==VT_BOOL)result=value.boolVal!=VARIANT_FALSE;
        else if(value.vt==VT_I4)result=value.lVal;
    }
    VariantClear(&value);return result;
}
std::wstring ClockStringProperty(IUIAutomationElement* element,PROPERTYID property) {
    VARIANT value{};VariantInit(&value);std::wstring result;
    if(SUCCEEDED(element->GetCachedPropertyValue(property,&value))) {
        if(value.vt==VT_BSTR && value.bstrVal)result.assign(value.bstrVal,std::min<UINT>(SysStringLen(value.bstrVal),512));
        else if(value.vt==(VT_ARRAY|VT_I4) && value.parray && SafeArrayGetDim(value.parray)==1) {
            LONG begin=0,end=-1;
            if(SUCCEEDED(SafeArrayGetLBound(value.parray,1,&begin)) && SUCCEEDED(SafeArrayGetUBound(value.parray,1,&end)) && end-begin<24)
                for(LONG i=begin;i<=end;++i){LONG part=0;if(SUCCEEDED(SafeArrayGetElement(value.parray,&i,&part)))result+=std::to_wstring(part)+L".";}
        }
    }
    VariantClear(&value);return result;
}
std::vector<ClockNode> ReadClockNodes(IUIAutomation2* automation,HWND window) {
    std::vector<ClockNode> nodes;if(!automation || !window)return nodes;
    g_automationDeadline=GetTickCount64()+1500;
    winrt::com_ptr<IUIAutomationCacheRequest> cache;
    if(FAILED(automation->CreateCacheRequest(cache.put())))return nodes;
    for(auto property:{UIA_NamePropertyId,UIA_AutomationIdPropertyId,UIA_ControlTypePropertyId,UIA_IsOffscreenPropertyId,
        UIA_IsEnabledPropertyId,UIA_BoundingRectanglePropertyId,UIA_SelectionItemIsSelectedPropertyId,
        UIA_ToggleToggleStatePropertyId,UIA_ValueValuePropertyId,UIA_RuntimeIdPropertyId})cache->AddProperty(property);
    cache->put_TreeScope(TreeScope_Element);
    winrt::com_ptr<IUIAutomationCondition> rawCondition;
    if(SUCCEEDED(automation->get_RawViewCondition(rawCondition.put())) && rawCondition)
        cache->put_TreeFilter(rawCondition.get());
    winrt::com_ptr<IUIAutomationElement> root;
    if(FAILED(automation->ElementFromHandleBuildCache(window,cache.get(),root.put())) || !root)return nodes;
    // Raw view includes custom readouts and digit TextBlocks omitted by ControlView.
    winrt::com_ptr<IUIAutomationTreeWalker> walker;automation->get_RawViewWalker(walker.put());if(!walker)return nodes;
    struct Pending {winrt::com_ptr<IUIAutomationElement> element;int parent;unsigned depth;};
    std::vector<Pending> pending;pending.push_back({std::move(root),-1,0});bool complete=true;
    for(size_t i=0;i<pending.size() && i<512 && !AutomationBudgetExpired();++i) {
        auto current=pending[i];ClockNode node;node.parent=current.parent;
        auto element=current.element.get();
        node.name=ClockStringProperty(element,UIA_NamePropertyId);node.id=LowerText(ClockStringProperty(element,UIA_AutomationIdPropertyId));
        node.value=ClockStringProperty(element,UIA_ValueValuePropertyId);node.runtimeKey=ClockStringProperty(element,UIA_RuntimeIdPropertyId);
        node.type=ClockIntProperty(element,UIA_ControlTypePropertyId);
        node.offscreen=ClockIntProperty(element,UIA_IsOffscreenPropertyId)!=0;
        node.enabled=ClockIntProperty(element,UIA_IsEnabledPropertyId)==1;
        node.selected=ClockIntProperty(element,UIA_SelectionItemIsSelectedPropertyId)==1;
        node.toggle=ClockIntProperty(element,UIA_ToggleToggleStatePropertyId);
        element->get_CachedBoundingRectangle(&node.rect);
        bool omit=node.id.find(L"lapsandsplits")!=std::wstring::npos || node.id.find(L"laplist")!=std::wstring::npos;
        int index=static_cast<int>(nodes.size());nodes.push_back(std::move(node));
        if(omit || (i && nodes.back().offscreen))continue;
        if(current.depth>=24){complete=false;continue;}
        winrt::com_ptr<IUIAutomationElement> child;
        if(FAILED(walker->GetFirstChildElementBuildCache(element,cache.get(),child.put()))){complete=false;continue;}
        while(child && pending.size()<512 && !AutomationBudgetExpired()) {
            pending.push_back({child,index,current.depth+1});
            winrt::com_ptr<IUIAutomationElement> next;
            if(FAILED(walker->GetNextSiblingElementBuildCache(child.get(),cache.get(),next.put()))){complete=false;break;}
            child=std::move(next);
        }
        if(child)complete=false;
    }
    if(!complete || AutomationBudgetExpired() || nodes.size()!=pending.size())nodes.clear();
    return nodes;
}

bool ClockNodeUnder(std::vector<ClockNode> const& nodes,int child,int parent) {
    for(int i=child,depth=0;i>=0 && i<static_cast<int>(nodes.size()) && depth<32;i=nodes[i].parent,++depth)if(i==parent)return true;
    return false;
}
bool ClockTextHas(std::wstring const& text,std::initializer_list<PCWSTR> words) {
    return std::any_of(words.begin(),words.end(),[&](auto word){return text.find(word)!=std::wstring::npos;});
}
bool ClockLap(std::vector<ClockNode> const& nodes,int index) {
    for(int i=index;i>=0;i=nodes[i].parent)if(ClockTextHas(nodes[i].id,{L"lapsandsplits",L"laplist",L"laptime",L"splittime",L"lapitem"}))return true;
    return false;
}
int ClockPage(std::vector<ClockNode> const& nodes) {
    for(auto const& node:nodes)if(!node.offscreen && (node.selected || node.toggle==1)) {
        auto name=LowerText(node.name);
        if(ClockTextHas(name,{L"stopwatch",L"bấm giờ"}) || node.id.find(L"stopwatch")!=std::wstring::npos)return 0;
        if(ClockTextHas(name,{L"timer",L"hẹn giờ"}) || node.id==L"timerbutton")return 1;
        if(ClockTextHas(name,{L"alarm",L"báo thức"}) || node.id==L"alarmbutton")return 2;
    }
    for(auto const& n:nodes)if(!n.offscreen) {
        if(ClockTextHas(n.id,{L"stopwatchtimer",L"stopwatchdisplay",L"stopwatchplaypause",L"stopwatchreset"}))return 0;
        if(ClockTextHas(n.id,{L"timertime",L"countdown",L"timerdisplay",L"timercontrol",L"timerplaypause",L"timerpause"}))return 1;
        if(ClockTextHas(n.id,{L"alarmtime",L"alarmtoggleswitch"}))return 2;
    }
    return -1;
}
bool ClockTransport(ClockNode const& n) {
    if(n.offscreen || !n.enabled || (n.type!=UIA_ButtonControlTypeId && n.type!=UIA_CheckBoxControlTypeId && n.type!=UIA_CustomControlTypeId))return false;
    auto name=LowerText(n.name);
    return ClockTextHas(n.id,{L"playpause",L"pausebutton",L"startbutton",L"resumebutton"}) ||
        ClockTextHas(name,{L"pause",L"resume",L"tạm dừng",L"tiếp tục",L"bắt đầu"}) || name==L"start" || name==L"stop" || name==L"dừng";
}
int ClockRow(std::vector<ClockNode> const& nodes,int index,int page) {
    if(page==0)return 0;
    for(int p=index,depth=0;p>=0 && depth<24;p=nodes[p].parent,++depth)
        if(nodes[p].type==UIA_ListItemControlTypeId || nodes[p].type==UIA_DataItemControlTypeId ||
            ClockTextHas(nodes[p].id,{L"timercard",L"alarmcard",L"timeritem",L"alarmitem",L"timercontrol"}))return p;
    if(page==1)for(int p=index,depth=0;p>=0 && depth<24;p=nodes[p].parent,++depth)
        for(int c=0;c<static_cast<int>(nodes.size());++c)
            if(ClockTransport(nodes[c]) && ClockNodeUnder(nodes,c,p))return p;
    return -1;
}
ClockRunState ClockRowRun(std::vector<ClockNode> const& nodes,int row) {
    bool pause=false,start=false;
    for(int i=0;i<static_cast<int>(nodes.size());++i) {
        auto const& n=nodes[i];if(!ClockTransport(n) || !ClockNodeUnder(nodes,i,row))continue;
        auto name=LowerText(n.name);
        bool namedPause=ClockTextHas(name,{L"pause",L"tạm dừng"}) || name==L"stop" || name==L"dừng";
        bool namedStart=ClockTextHas(name,{L"start",L"resume",L"bắt đầu",L"tiếp tục"});
        if(namedPause || namedStart){pause|=namedPause;start|=namedStart;continue;}
        // A combined PlayPause automation ID alone does not indicate its state.
        if(n.id.find(L"playpause")==std::wstring::npos) {
            pause|=ClockTextHas(n.id,{L"pausebutton"});start|=ClockTextHas(n.id,{L"startbutton",L"resumebutton"});
        }
    }
    return pause==start?ClockRunState::Unknown:pause?ClockRunState::Running:ClockRunState::Paused;
}
bool ParseClockDuration(std::wstring text,int64_t& milliseconds) {
    ParsedClockTime time;if(ParseClockTime(text,time)){milliseconds=ClockDurationMs(time);return true;}
    text=LowerText(text);int64_t total=0;bool found=false;unsigned units=0;
    for(size_t pos=0;pos<text.size();) {
        if(text[pos]<L'0' || text[pos]>L'9'){++pos;continue;}
        size_t begin=pos;int64_t whole=0;while(pos<text.size() && text[pos]>=L'0' && text[pos]<=L'9') {
            if(pos-begin>6)return false;whole=whole*10+text[pos++]-L'0';
        }
        int fraction=0,place=100;
        if(pos<text.size() && (text[pos]==L',' || text[pos]==L'.') && pos+1<text.size() && iswdigit(text[pos+1])) {
            ++pos;while(pos<text.size() && iswdigit(text[pos])) {if(place){fraction+=(text[pos]-L'0')*place;place/=10;}++pos;}
        }
        while(pos<text.size() && iswspace(text[pos]))++pos;
        size_t wordStart=pos;while(pos<text.size() && (iswalpha(text[pos]) || (text[pos]>=0x80 && !iswspace(text[pos]))))++pos;
        auto unit=text.substr(wordStart,pos-wordStart);int64_t scale=0;unsigned bit=0;
        if(unit==L"hour" || unit==L"hours" || unit==L"hr" || unit==L"hrs" || unit==L"giờ"){scale=3600000;bit=1;}
        else if(unit==L"minute" || unit==L"minutes" || unit==L"min" || unit==L"mins" || unit==L"phút"){scale=60000;bit=2;}
        else if(unit==L"second" || unit==L"seconds" || unit==L"sec" || unit==L"secs" || unit==L"giây"){scale=1000;bit=4;}
        else if(unit==L"millisecond" || unit==L"milliseconds" || unit==L"ms"){scale=1;bit=8;}
        else continue;
        if(units&bit)return false;units|=bit;found=true;total+=whole*scale+fraction*scale/1000;
    }
    if(!found || total>999LL*3600000)return false;milliseconds=total;return true;
}
std::wstring ClockReadout(std::vector<ClockNode> const& nodes,int index) {
    auto const& node=nodes[index];
    if(!node.value.empty())return node.value;
    int64_t ignored;if(ParseClockDuration(node.name,ignored))return node.name;
    // A custom readout may expose separate hour/minute/second TextBlocks.
    std::wstring joined;unsigned count=0;
    for(int i=0;i<static_cast<int>(nodes.size());++i) {
        auto const& part=nodes[i];if(part.offscreen || part.parent!=index || part.type!=UIA_TextControlTypeId)continue;
        auto text=part.value.empty()?part.name:part.value;
        bool fragment=!text.empty() && std::all_of(text.begin(),text.end(),[](wchar_t c){return iswdigit(c)||iswspace(c)||c==L':'||c==L'：'||c==L'.'||c==L','||c==0x200e||c==0x200f;});
        if(!fragment)return node.name;
        if(++count>12)return node.name;
        joined+=text;
    }
    return count>=3?joined:node.name;
}
bool ClockEndTimeOrLabel(std::vector<ClockNode> const& nodes,int index,int row) {
    for(int p=index,depth=0;p>=0 && p!=row && depth<24;p=nodes[p].parent,++depth) {
        if(ClockTextHas(nodes[p].id,{L"timername",L"alarmname",L"durationlabel",L"originalduration",L"initialduration",
            L"endtime",L"finishtime",L"completiontime",L"targettime",L"timeofcompletion",L"timeofend"}))return true;
    }
    auto name=LowerText(nodes[index].name);
    return ClockTextHas(name,{L"ends at",L"finishes at",L"finish at",L"complete at",L"kết thúc lúc",L"kết thúc vào",L"hết giờ lúc",L"hết giờ vào"});
}
bool ClockDurationHint(ClockNode const& node) {
    return ClockTextHas(node.id,{L"remaining",L"countdown",L"stopwatchtime",L"stopwatchdisplay"}) ||
        ClockTextHas(LowerText(node.name),{L"remaining",L"còn lại"});
}
std::wstring ClockRowText(std::vector<ClockNode> const& nodes,int row) {
    std::wstring text;
    for(int i=0;i<static_cast<int>(nodes.size());++i)if(!nodes[i].offscreen && ClockNodeUnder(nodes,i,row) &&
        (nodes[i].type==UIA_TextControlTypeId || i==row)) {if(text.size()>4096)break;text+=nodes[i].name+L" ";}
    return LowerText(text);
}

int64_t NextObservedAlarm(ParsedClockTime time,std::wstring const& text,int64_t now) {
    if(time.secondsPresent || time.hours>23)return 0;
    bool pm=ClockTextHas(text,{L" pm",L" p.m.",L" ch"});
    bool am=ClockTextHas(text,{L" am",L" a.m.",L" sa"});
    if(pm && am)return 0;
    if(pm || am){if(time.hours<1 || time.hours>12)return 0;time.hours=time.hours%12+(pm?12:0);}
    unsigned days=0;
    const std::array<std::initializer_list<PCWSTR>,7> labels={{{L"sunday",L" sun",L"chủ nhật"},{L"monday",L" mon",L"thứ hai"},
        {L"tuesday",L" tue",L"thứ ba"},{L"wednesday",L" wed",L"thứ tư"},{L"thursday",L" thu",L"thứ năm"},
        {L"friday",L" fri",L"thứ sáu"},{L"saturday",L" sat",L"thứ bảy"}}};
    for(unsigned i=0;i<7;++i)if(ClockTextHas(L" "+text,labels[i]))days|=1u<<i;
    if(ClockTextHas(text,{L"every day",L"daily",L"hàng ngày",L"hằng ngày",L"mỗi ngày"}))days=127;
    bool tomorrow=ClockTextHas(text,{L"tomorrow",L"ngày mai"});
    bool today=ClockTextHas(text,{L"today",L"hôm nay"});
    bool once=ClockTextHas(text,{L"once",L"one time",L"một lần",L"không lặp",L"never"});
    // Do not guess an unsupported locale's repeat schedule from the clock face.
    if(!days && !tomorrow && !today && !once)return 0;
    SYSTEMTIME local{};GetLocalTime(&local);local.wHour=local.wMinute=local.wSecond=local.wMilliseconds=0;
    FILETIME midnight{};if(!SystemTimeToFileTime(&local,&midnight))return 0;
    auto midnightValue=FileTimeValue(midnight);
    for(unsigned offset=0;offset<=7;++offset) {
        if(!days && tomorrow && offset!=1)continue;
        if(!days && today && !tomorrow && offset!=0)continue;
        uint64_t ticks=midnightValue+offset*864000000000ULL;
        FILETIME ft{static_cast<DWORD>(ticks),static_cast<DWORD>(ticks>>32)};
        SYSTEMTIME date{},utc{};if(!FileTimeToSystemTime(&ft,&date))continue;
        if(days && !(days&(1u<<date.wDayOfWeek)))continue;
        date.wHour=static_cast<WORD>(time.hours);date.wMinute=static_cast<WORD>(time.minutes);
        if(!TzSpecificLocalTimeToSystemTime(nullptr,&date,&utc) || !SystemTimeToFileTime(&utc,&ft))continue;
        int64_t due=static_cast<int64_t>(FileTimeValue(ft)/10000);
        if(due>now)return due;
    }
    return 0;
}
std::vector<ClockEntry> DecodeClockNodes(std::vector<ClockNode> const& nodes,int64_t utcNow,uint64_t now) {
    std::vector<ClockEntry> result;int page=ClockPage(nodes);if(page<0)return result;
    for(auto const& n:nodes)if(!n.offscreen && n.type==UIA_ButtonControlTypeId &&
        ClockTextHas(n.id,{L"savealarm",L"alarmsave",L"savetimer",L"timersave",L"savebutton"}))return result;
    struct Candidate {int node;ParsedClockTime time;int64_t duration;long long score;};std::map<int,Candidate> rows;
    for(int i=0;i<static_cast<int>(nodes.size());++i) {
        auto const& n=nodes[i];if(n.offscreen || ClockLap(nodes,i))continue;
        if(n.type!=UIA_TextControlTypeId && n.type!=UIA_CustomControlTypeId && n.type!=UIA_GroupControlTypeId &&
            n.type!=UIA_PaneControlTypeId && n.type!=UIA_ListItemControlTypeId && n.type!=UIA_DataItemControlTypeId)continue;
        int row=ClockRow(nodes,i,page);if(row<0)continue;
        if(page!=2 && ClockEndTimeOrLabel(nodes,i,row))continue;
        auto text=ClockReadout(nodes,i);ParsedClockTime time;int64_t duration=0;
        bool colon=ParseClockTime(text,time);
        if(page==2 ? (!colon || time.secondsPresent) : !ParseClockDuration(text,duration))continue;
        const bool durationHint=ClockDurationHint(n);
        // A bare hh:mm badge is the timer's ending wall-clock time. Accept mm:ss
        // only from a positively identified remaining-time/stopwatch readout.
        if(page==1 && colon && !time.secondsPresent && !durationHint)continue;
        long long score=std::min<long long>(99999,std::max(0L,n.rect.right-n.rect.left)*static_cast<long long>(std::max(0L,n.rect.bottom-n.rect.top)));
        if(page!=2) {
            const bool secondsUnit=ClockTextHas(LowerText(text),{L"second",L" sec",L"giây"});
            // Time format outranks Value and automation-ID hints: a 3-part
            // duration must beat a 2-part completion label even with Value set.
            score+=(colon && time.secondsPresent?8:!colon && secondsUnit?8:colon?4:1)*1000000LL;
            if(!colon && !secondsUnit && !durationHint) {
                bool hasChildren=false;
                for(auto const& child:nodes)hasChildren|=child.parent==i && !child.offscreen;
                if(hasChildren || i==row)continue;
                // Never interpret a card title such as "3 minutes" as a readout.
                bool label=false;
                for(auto const& child:nodes)if(child.parent==row && ClockTextHas(child.id,{L"timername",L"alarmname"}))label|=child.name==text;
                if(label)continue;
            }
        }
        if(!n.value.empty())score+=100000;
        if(durationHint || ClockTextHas(n.id,{L"timertext",L"timedisplay",L"readout"}))score+=200000;
        auto found=rows.find(row);if(found==rows.end() || found->second.score<score)rows.insert_or_assign(row,Candidate{i,time,duration,score});
    }
    for(auto const& [row,candidate]:rows) {
        if(result.size()>=16)break;
        ClockEntry entry;entry.kind=static_cast<ClockKind>(page);entry.sampledAt=entry.observedAt=now;
        std::wstring label;
        for(int i=0;i<static_cast<int>(nodes.size());++i)if(!nodes[i].offscreen && ClockNodeUnder(nodes,i,row) &&
            ClockTextHas(nodes[i].id,{L"alarmname",L"timername"}) && nodes[i].type==UIA_TextControlTypeId){label=nodes[i].name.substr(0,100);break;}
        entry.label=label;
        auto identity=page==0?std::wstring(L"stopwatch"):!nodes[row].runtimeKey.empty()?nodes[row].runtimeKey:
            nodes[row].id+L":"+label+L":"+std::to_wstring(row);
        entry.key=std::to_wstring(page)+L":"+identity;
        if(page==2) {
            bool on=false,off=false;
            for(int i=0;i<static_cast<int>(nodes.size());++i)if(ClockNodeUnder(nodes,i,row) && !nodes[i].offscreen && nodes[i].toggle>=0) {
                on|=nodes[i].toggle==ToggleState_On;off|=nodes[i].toggle==ToggleState_Off;
            }
            if(!on && !off)continue;
            entry.enabled=on && !off;
            entry.alarmUtc=NextObservedAlarm(candidate.time,ClockRowText(nodes,row),utcNow);
            if(entry.enabled && !entry.alarmUtc)continue;
            entry.key+=L":"+std::to_wstring(candidate.time.hours)+L":"+std::to_wstring(candidate.time.minutes);
        } else {
            entry.milliseconds=candidate.duration;entry.run=ClockRowRun(nodes,row);
            // A future badge such as "hết giờ lúc 18:46" is not a completion.
            entry.finished=page==1 && candidate.duration==0 &&
                ClockTextHas(ClockRowText(nodes,row),{L"time's up",L"timer finished",L"hết giờ",L"đã hết giờ"});
        }
        result.push_back(std::move(entry));
    }
    return result;
}

DWORD WINAPI ClockProviderThreadProc(void*) {
    bool initialized=false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);initialized=true;
        {
            winrt::com_ptr<IUIAutomation2> automation;
            if(SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation8),nullptr,CLSCTX_INPROC_SERVER,__uuidof(IUIAutomation2),automation.put_void()))) {
                automation->put_ConnectionTimeout(200);automation->put_TransactionTimeout(200);automation->put_AutoSetFocus(FALSE);
            }
            ClockTracker tracker;ClockWindow cachedWindow;uint64_t nextDiscovery=0;
            int lastDiagnostic=-999;
            while(WaitForSingleObject(g_liveStopEvent,0)!=WAIT_OBJECT_0) {
                auto settings=GetSettings();uint64_t now=GetTickCount64();int64_t utc=ClockUtcMilliseconds();
                ClockWindow window;
                if(settings.enableTaskbarWidget && settings.enableClockWidget) {
                    if(now>=nextDiscovery || (cachedWindow.element && !IsWindow(cachedWindow.element))) {
                        cachedWindow=FindWindowsClock();nextDiscovery=now+3000;
                    }
                    window=cachedWindow;
                }
                std::vector<ClockEntry> entries;
                if(window.element && !IsIconic(window.top))try {
                    DWORD pid=0;GetWindowThreadProcessId(window.element,&pid);
                    if(pid==window.pid) {
                        auto nodes=ReadClockNodes(automation.get(),window.element);
                        now=GetTickCount64();utc=ClockUtcMilliseconds();entries=DecodeClockNodes(nodes,utc,now);
                        int diagnostic=ClockPage(nodes)*1000+static_cast<int>(entries.size());
                        if(diagnostic!=lastDiagnostic){Wh_Log(L"Windows Clock: page=%d, nodes=%zu, activities=%zu",ClockPage(nodes),nodes.size(),entries.size());lastDiagnostic=diagnostic;}
                    }else{cachedWindow={};nextDiscovery=0;window={};}
                }catch(...){}
                auto const previous=tracker.state;
                // Prefix transient runtime IDs with their source process/window.
                for(auto& entry:entries)entry.key=std::to_wstring(window.pid)+L":"+
                    std::to_wstring(reinterpret_cast<uintptr_t>(window.element))+L":"+entry.key;
                tracker.state.window=reinterpret_cast<uintptr_t>(window.top);
                tracker.Update(std::move(entries),now,utc,settings.enableTaskbarWidget && settings.enableClockWidget);
                auto const& next=tracker.state;
                bool sameEntries=next.entries.size()==previous.entries.size() && std::equal(next.entries.begin(),next.entries.end(),previous.entries.begin(),[&](auto const& a,auto const& b) {
                    return a.key==b.key && a.label==b.label && a.kind==b.kind && a.run==b.run && a.finished==b.finished &&
                        a.finishedEstimated==b.finishedEstimated && a.alarmUtc==b.alarmUtc && a.milliseconds/1000==b.milliseconds/1000 &&
                        ClockEntryFresh(a,now)==ClockEntryFresh(b,now);
                });
                const bool changed=!sameEntries || next.window!=previous.window || next.noticeSerial!=previous.noticeSerial;
                {std::lock_guard<std::mutex> lock(g_liveMutex);g_clockSnapshot=next;}
                if(changed)NotifyLiveUi();
                // No UIA work while minimized/closed; anchors still advance. The
                // existing taskbar UI timer repaints values once per second.
                const DWORD delay=window.element && !IsIconic(window.top)?500:next.entries.empty()?3000:1000;
                if(WaitForSingleObject(g_liveStopEvent,delay)==WAIT_OBJECT_0)break;
            }
        }
    }catch(...){Wh_Log(L"Windows Clock provider unavailable.");}
    if(initialized)winrt::uninit_apartment();return 0;
}

void StartLiveProviders() {
    g_liveStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_liveWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_systemWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_bluetoothWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_taskControlWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if(g_liveStopEvent)g_clockThread=CreateThread(nullptr,0,ClockProviderThreadProc,nullptr,0,nullptr);
    if (g_liveStopEvent) g_temperatureThread=CreateThread(nullptr,0,TemperatureThreadProc,nullptr,0,nullptr);
    if (g_liveStopEvent && g_systemWake) g_systemThread = CreateThread(nullptr, 0, SystemActivityThreadProc, nullptr, 0, nullptr);
    if (g_liveStopEvent && g_bluetoothWake) g_bluetoothThread = CreateThread(nullptr, 0, BluetoothProviderThreadProc, nullptr, 0, nullptr);
    if (g_liveStopEvent && g_taskControlWake) g_taskControlThread = CreateThread(nullptr, 0, TaskControlThreadProc, nullptr, 0, nullptr);
    g_obsWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_liveStopEvent && g_obsWake) g_obsThread = CreateThread(nullptr, 0, ObsThreadProc, nullptr, 0, nullptr);
    g_artWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_cultureWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_liveStopEvent && g_artWake) g_artThread = CreateThread(nullptr, 0, ArtworkThreadProc, nullptr, 0, nullptr);

    if (g_liveStopEvent && g_liveWakeEvent)
        g_liveThread = CreateThread(nullptr, 0, LiveProviderThreadProc, nullptr, 0, nullptr);
    if (!g_liveThread) Wh_Log(L"Could not start live providers: %lu", GetLastError());
}

void StopLiveProviders() {
    if (g_liveStopEvent) SetEvent(g_liveStopEvent);
    if(g_clockThread){WaitForSingleObject(g_clockThread,INFINITE);CloseHandle(g_clockThread);g_clockThread=nullptr;}
    if(g_temperatureThread){WaitForSingleObject(g_temperatureThread,INFINITE);CloseHandle(g_temperatureThread);g_temperatureThread=nullptr;}
    if (g_systemThread) { WaitForSingleObject(g_systemThread, INFINITE); CloseHandle(g_systemThread); g_systemThread = nullptr; }
    if (g_bluetoothThread) { WaitForSingleObject(g_bluetoothThread, INFINITE); CloseHandle(g_bluetoothThread); g_bluetoothThread = nullptr; }
    if (g_taskControlThread) { WaitForSingleObject(g_taskControlThread, INFINITE); CloseHandle(g_taskControlThread); g_taskControlThread = nullptr; }
    if (g_obsThread) { WaitForSingleObject(g_obsThread, INFINITE); CloseHandle(g_obsThread); g_obsThread = nullptr; }
    HANDLE cultureThread = nullptr;
    { std::lock_guard<std::mutex> lock(g_cultureThreadMutex); cultureThread = std::exchange(g_cultureThread, nullptr); }
    if (cultureThread) { WaitForSingleObject(cultureThread, INFINITE); CloseHandle(cultureThread); }
    if (g_artThread) { WaitForSingleObject(g_artThread, INFINITE); CloseHandle(g_artThread); g_artThread = nullptr; }
    if (g_liveThread) {
        // Called on Windhawk's engine thread, never while holding a UI/data lock.
        WaitForSingleObject(g_liveThread, INFINITE);
        CloseHandle(g_liveThread);
        g_liveThread = nullptr;
    }
}

void ReleaseLiveProviderHandles() {
    if (g_systemWake) CloseHandle(g_systemWake);
    if (g_bluetoothWake) CloseHandle(g_bluetoothWake);
    g_bluetoothWake = nullptr;
    if (g_taskControlWake) CloseHandle(g_taskControlWake);
    g_systemWake = g_taskControlWake = nullptr;
    if (g_obsWake) CloseHandle(g_obsWake);
    g_obsWake = nullptr;
    if (g_artWake) CloseHandle(g_artWake);
    if (g_cultureWake) CloseHandle(g_cultureWake);
    g_artWake = g_cultureWake = nullptr;
    if (g_liveWakeEvent) CloseHandle(g_liveWakeEvent);
    if (g_liveStopEvent) CloseHandle(g_liveStopEvent);
    g_liveWakeEvent = g_liveStopEvent = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// String & Date Utilities

std::wstring TrimWhitespace(std::wstring const& text) {
    size_t begin = 0, end = text.size();
    while (begin < end && iswspace(text[begin])) begin++;
    while (end > begin && iswspace(text[end - 1])) end--;
    return text.substr(begin, end - begin);
}

int32_t MakeDateKey(int year, int month, int day) {
    return year * 10000 + month * 100 + day;
}

void OffsetSystemDateByDays(SYSTEMTIME& st, int deltaDays) {
    FILETIME ft{};
    if (!SystemTimeToFileTime(&st, &ft)) return;
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    LONGLONG ticksPerDay = 864000000000LL;
    uli.QuadPart = static_cast<ULONGLONG>(static_cast<LONGLONG>(uli.QuadPart) + deltaDays * ticksPerDay);
    ft.dwLowDateTime = uli.LowPart;
    ft.dwHighDateTime = uli.HighPart;
    FileTimeToSystemTime(&ft, &st);
}

////////////////////////////////////////////////////////////////////////////////
// Pure Arithmetic Vietnamese Lunar Calendar (Hồ Ngọc Đức Algorithm, UTC+7)

const PCWSTR CAN_10[] = {
    L"Giáp", L"Ất", L"Bính", L"Đinh", L"Mậu", L"Kỷ", L"Canh", L"Tân", L"Nhâm", L"Quý"
};
const PCWSTR CHI_12[] = {
    L"Tý", L"Sửu", L"Dần", L"Mão", L"Thìn", L"Tỵ", L"Ngọ", L"Mùi", L"Thân", L"Dậu", L"Tuất", L"Hợi"
};

// Vietnamese 12 Zodiac Animals (Mão = Cat 🐈)
const PCWSTR ZODIAC_EMOJI_12[] = {
    L"🐀", L"🐂", L"🐯", L"🐈", L"🐉", L"🐍", L"🐴", L"🐐", L"🐒", L"🐓", L"🐕", L"🐖"
};

const PCWSTR ZODIAC_VI_NAME_12[] = {
    L"Chuột", L"Trâu", L"Hổ", L"Mèo", L"Rồng", L"Rắn", L"Ngựa", L"Dê", L"Khỉ", L"Gà", L"Chó", L"Lợn"
};

const PCWSTR ZODIAC_EN_NAME_12[] = {
    L"Rat", L"Buffalo", L"Tiger", L"Cat", L"Dragon", L"Snake", L"Horse", L"Goat", L"Monkey", L"Rooster", L"Dog", L"Pig"
};

const PCWSTR STEM_EN_10[] = {
    L"Yang Wood", L"Yin Wood", L"Yang Fire", L"Yin Fire", L"Yang Earth", L"Yin Earth",
    L"Yang Metal", L"Yin Metal", L"Yang Water", L"Yin Water"
};
std::wstring LocalizedCanChi(int stem, int branch, bool vi) {
    return std::wstring(vi ? CAN_10[stem] : STEM_EN_10[stem]) + L" " +
        (vi ? CHI_12[branch] : ZODIAC_EN_NAME_12[branch]);
}

std::wstring GetCanChiYear(int year) {
    int canIdx = ((year + 6) % 10 + 10) % 10;
    int chiIdx = ((year + 8) % 12 + 12) % 12;
    return std::wstring(CAN_10[canIdx]) + L" " + CHI_12[chiIdx];
}

int CalculateJDN(int sDay, int sMonth, int sYear) {
    int a = (14 - sMonth) / 12;
    int y = sYear + 4800 - a;
    int m = sMonth + 12 * a - 3;
    return sDay + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}

constexpr double kPi = 3.14159265358979323846;

int IntFloor(double d) {
    return static_cast<int>(std::floor(d));
}

double NewMoonJDN(int k) {
    double T = k / 1236.85;
    double T2 = T * T;
    double T3 = T2 * T;
    double dr = kPi / 180.0;
    double Jd1 = 2415020.75933 + 29.53058868 * k + 0.0001178 * T2 - 0.000000155 * T3;
    Jd1 += 0.00033 * std::sin((166.56 + 132.87 * T - 0.009173 * T2) * dr);
    double M = 359.2242 + 29.10535608 * k - 0.0000333 * T2 - 0.00000347 * T3;
    double Mpr = 306.0253 + 385.81691806 * k + 0.0107306 * T2 + 0.00001236 * T3;
    double F = 21.2964 + 390.67050646 * k - 0.0016528 * T2 - 0.00000239 * T3;
    double C1 = (0.1734 - 0.000393 * T) * std::sin(M * dr) + 0.0021 * std::sin(2 * dr * M);
    C1 = C1 - 0.4068 * std::sin(Mpr * dr) + 0.0161 * std::sin(dr * 2 * Mpr);
    C1 = C1 - 0.0004 * std::sin(dr * 3 * Mpr);
    C1 = C1 + 0.0104 * std::sin(dr * 2 * F) - 0.0051 * std::sin(dr * (M + Mpr));
    C1 = C1 - 0.0074 * std::sin(dr * (M - Mpr)) + 0.0004 * std::sin(dr * (2 * F + M));
    C1 = C1 - 0.0004 * std::sin(dr * (2 * F - M)) - 0.0006 * std::sin(dr * (2 * F + Mpr));
    C1 = C1 + 0.0010 * std::sin(dr * (2 * F - Mpr)) + 0.0005 * std::sin(dr * (2 * Mpr + M));
    double deltat;
    if (T < -11) {
        deltat = 0.001 + 0.000839 * T + 0.0002261 * T2 - 0.00000845 * T3 - 0.000000081 * T * T3;
    } else {
        deltat = -0.000278 + 0.000265 * T + 0.000262 * T2;
    }
    return Jd1 + C1 - deltat;
}

int GetNewMoonDay(int k, double timeZone) {
    return IntFloor(NewMoonJDN(k) + 0.5 + timeZone / 24.0);
}

double SunLongitudeRadians(double jdn) {
    double T = (jdn - 2451545.0) / 36525.0;
    double T2 = T * T;
    double dr = kPi / 180.0;
    double M = 357.52910 + 35999.05030 * T - 0.0001559 * T2 - 0.00000048 * T * T2;
    double L0 = 280.46645 + 36000.76983 * T + 0.0003032 * T2;
    double DL = (1.914600 - 0.004817 * T - 0.000014 * T2) * std::sin(dr * M);
    DL += (0.019993 - 0.000101 * T) * std::sin(dr * 2 * M) + 0.000290 * std::sin(dr * 3 * M);
    double L = (L0 + DL) * dr;
    L -= kPi * 2 * IntFloor(L / (kPi * 2));
    return L;
}

int GetSunLongitudeSector(int dayNumber, double timeZone) {
    return IntFloor(SunLongitudeRadians(dayNumber - 0.5 - timeZone / 24.0) / kPi * 6.0);
}

int GetLunarMonth11(int yy, double timeZone) {
    int off = CalculateJDN(31, 12, yy) - 2415021;
    int k = IntFloor(off / 29.530588853);
    int nm = GetNewMoonDay(k, timeZone);
    int sunLong = GetSunLongitudeSector(nm, timeZone);
    if (sunLong >= 9) {
        nm = GetNewMoonDay(k - 1, timeZone);
    }
    return nm;
}

int GetLeapMonthOffset(int a11, double timeZone) {
    int k = IntFloor((a11 - 2415021.076998695) / 29.530588853 + 0.5);
    int last = 0;
    int i = 1;
    int arc = GetSunLongitudeSector(GetNewMoonDay(k + i, timeZone), timeZone);
    do {
        last = arc;
        i++;
        arc = GetSunLongitudeSector(GetNewMoonDay(k + i, timeZone), timeZone);
    } while (arc != last && i < 14);
    return i - 1;
}

void GetVietnameseLunarDate(int sYear, int sMonth, int sDay,
                            int& lDay, int& lMonth, int& lYear,
                            bool& isLeap, std::wstring& canChi) {
    constexpr double kVietnamTimeZone = 7.0;
    int dayNumber = CalculateJDN(sDay, sMonth, sYear);
    int k = IntFloor((dayNumber - 2415021.076998695) / 29.530588853);
    int monthStart = GetNewMoonDay(k + 1, kVietnamTimeZone);
    if (monthStart > dayNumber) {
        monthStart = GetNewMoonDay(k, kVietnamTimeZone);
    }
    int a11 = GetLunarMonth11(sYear, kVietnamTimeZone);
    int b11 = a11;
    if (a11 >= monthStart) {
        lYear = sYear;
        a11 = GetLunarMonth11(sYear - 1, kVietnamTimeZone);
    } else {
        lYear = sYear + 1;
        b11 = GetLunarMonth11(sYear + 1, kVietnamTimeZone);
    }
    lDay = dayNumber - monthStart + 1;
    int diff = IntFloor((monthStart - a11) / 29.0);
    isLeap = false;
    lMonth = diff + 11;
    if (b11 - a11 > 365) {
        int leapMonthDiff = GetLeapMonthOffset(a11, kVietnamTimeZone);
        if (diff >= leapMonthDiff) {
            lMonth = diff + 10;
            if (diff == leapMonthDiff) {
                isLeap = true;
            }
        }
    }
    if (lMonth > 12) {
        lMonth -= 12;
    }
    if (lMonth >= 11 && diff < 4) {
        lYear -= 1;
    }
    canChi = GetCanChiYear(lYear);
}

////////////////////////////////////////////////////////////////////////////////
// Holidays & Horoscope Data Tables

// One shared catalog for the compact widget, daily popup and holiday-list page.
// Sources (annual dates, not a leave / substitute-holiday schedule):
// https://doanthanhnien.vn/danh-muc/gioi-thieu-hoi-sinh-vien-viet-nam
// https://baochinhphu.vn/chuong-trinh-nghe-thuat-duong-len-phia-truoc-chao-mung-ngay-thanh-lap-dang-cong-san-viet-nam-102260203224622576.htm
// https://baochinhphu.vn/ky-niem-135-nam-ngay-sinh-chu-tich-ho-chi-minh-lanh-dao-dang-nha-nuoc-vao-lang-vieng-bac-10225051909201906.htm
// https://vnpa.moh.gov.vn/chuc-mung-ngay-thay-thuoc-viet-nam-27-02/
// https://cov.gov.vn/tin-tuc/ngay-sach-va-van-hoa-doc-viet-nam-168847.html
// https://baochinhphu.vn/ngay-bao-chi-cach-mang-viet-nam-21-6.html
// https://baochinhphu.vn/ngay-gia-dinh-viet-nam.html
// https://pbgdpl.cantho.gov.vn/ngay-truyen-thong-hoi-lien-hiep-thanh-nien-viet-nam-15-thang-10
// https://mattran.org.vn/hoat-dong/thong-bao-ve-viec-khong-to-chuc-tiep-khach-nhan-hoa-chuc-mung-nhan-dip-ky-niem-95-nam-ngay-truyen-thong-mat-tran-to-quoc-viet-nam-18111930-18112025-67598.html
// https://thanglong.chinhphu.vn/le-hoi-ky-niem-236-nam-chien-thang-ngoc-hoi-dong-da-103250202152726341.htm
// https://vi.wikipedia.org/wiki/C%C3%A1c_ng%C3%A0y_l%E1%BB%85_%E1%BB%9F_Vi%E1%BB%87t_Nam
struct HolidayEntry {
    int day; // 0 = last day of lunar year, which can be 29 or 30.
    int month;
    PCWSTR viName;
    PCWSTR enName;
};

static const HolidayEntry kSolarHolidays[] = {
    { 1,  1, L"Tết Dương lịch", L"New Year's Day" },
    { 9,  1, L"Ngày truyền thống Học sinh, Sinh viên Việt Nam", L"Vietnamese Students' Day" },
    { 3,  2, L"Ngày thành lập Đảng Cộng sản Việt Nam", L"Communist Party of Vietnam Founding Day" },
    { 14, 2, L"Lễ Tình nhân", L"Valentine's Day" },
    { 27, 2, L"Ngày Thầy thuốc Việt Nam", L"Vietnamese Doctors' Day" },
    { 3,  3, L"Ngày truyền thống Bộ đội Biên phòng", L"Vietnam Border Guard Day" },
    { 8,  3, L"Quốc tế Phụ nữ", L"International Women's Day" },
    { 26, 3, L"Ngày thành lập Đoàn TNCS Hồ Chí Minh", L"Ho Chi Minh Communist Youth Union Founding Day" },
    { 27, 3, L"Ngày Thể thao Việt Nam", L"Vietnam Sports Day" },
    { 21, 4, L"Ngày Sách và Văn hóa đọc Việt Nam", L"Vietnam Book and Reading Culture Day" },
    { 30, 4, L"Giải phóng miền Nam, thống nhất đất nước", L"Reunification Day" },
    { 1,  5, L"Quốc tế Lao động", L"International Workers' Day" },
    { 7,  5, L"Ngày chiến thắng Điện Biên Phủ", L"Dien Bien Phu Victory Day" },
    { 15, 5, L"Ngày thành lập Đội TNTP Hồ Chí Minh", L"Ho Chi Minh Young Pioneer Organization Founding Day" },
    { 19, 5, L"Ngày sinh Chủ tịch Hồ Chí Minh", L"President Ho Chi Minh's Birthday" },
    { 1,  6, L"Quốc tế Thiếu nhi", L"International Children's Day" },
    { 5,  6, L"Ngày Bác Hồ ra đi tìm đường cứu nước", L"Ho Chi Minh's Departure to Seek National Salvation" },
    { 21, 6, L"Ngày Báo chí Cách mạng Việt Nam", L"Vietnam Revolutionary Press Day" },
    { 28, 6, L"Ngày Gia đình Việt Nam", L"Vietnamese Family Day" },
    { 27, 7, L"Ngày Thương binh - Liệt sĩ", L"War Invalids and Martyrs' Day" },
    { 28, 7, L"Ngày thành lập Công đoàn Việt Nam", L"Vietnam Trade Union Founding Day" },
    { 19, 8, L"Cách mạng Tháng Tám thành công", L"August Revolution Day" },
    { 19, 8, L"Ngày truyền thống Công an nhân dân", L"People's Public Security Traditional Day" },
    { 2,  9, L"Quốc khánh Việt Nam", L"Vietnam National Day" },
    { 10,10, L"Ngày Giải phóng Thủ đô", L"Hanoi Liberation Day" },
    { 13,10, L"Ngày Doanh nhân Việt Nam", L"Vietnamese Entrepreneurs' Day" },
    { 15,10, L"Ngày truyền thống Hội Liên hiệp Thanh niên Việt Nam", L"Vietnam Youth Federation Traditional Day" },
    { 20,10, L"Ngày Phụ nữ Việt Nam", L"Vietnamese Women's Day" },
    { 9, 11, L"Ngày Pháp luật Việt Nam", L"Vietnam Law Day" },
    { 18,11, L"Ngày truyền thống Mặt trận Tổ quốc Việt Nam", L"Vietnam Fatherland Front Traditional Day" },
    { 20,11, L"Ngày Nhà giáo Việt Nam", L"Vietnamese Teachers' Day" },
    { 23,11, L"Ngày Di sản văn hóa Việt Nam", L"Vietnam Cultural Heritage Day" },
    { 24,11, L"Ngày Văn hóa Việt Nam", L"Vietnam Culture Day" },
    { 19,12, L"Ngày Toàn quốc kháng chiến", L"National Resistance Day" },
    { 22,12, L"Ngày thành lập Quân đội nhân dân Việt Nam", L"Vietnam People's Army Founding Day" },
    { 24,12, L"Đêm Giáng Sinh", L"Christmas Eve" },
    { 25,12, L"Lễ Giáng Sinh", L"Christmas Day" },
};

static const HolidayEntry kLunarHolidays[] = {
    { 1,  1, L"Mùng 1 Tết Nguyên Đán", L"Lunar New Year (Day 1)" },
    { 2,  1, L"Mùng 2 Tết Nguyên Đán", L"Lunar New Year (Day 2)" },
    { 3,  1, L"Mùng 3 Tết Nguyên Đán", L"Lunar New Year (Day 3)" },
    { 5,  1, L"Lễ hội Ngọc Hồi - Đống Đa", L"Ngoc Hoi - Dong Da Festival" },
    { 15, 1, L"Tết Nguyên Tiêu (Rằm tháng Giêng)", L"First Full Moon Festival" },
    { 3,  3, L"Tết Hàn Thực", L"Cold Food Festival" },
    { 10, 3, L"Giỗ Tổ Hùng Vương", L"Hung Kings' Commemoration Day" },
    { 15, 4, L"Lễ Phật Đản", L"Buddha's Birthday" },
    { 5,  5, L"Tết Đoan Ngọ", L"Mid-Year Festival" },
    { 15, 7, L"Lễ Vu Lan", L"Vu Lan Festival" },
    { 15, 8, L"Tết Trung Thu", L"Mid-Autumn Festival" },
    { 23,12, L"Ông Công Ông Táo", L"Kitchen Guardians Day" },
    { 0, 12, L"Tất niên / Giao thừa", L"Lunar New Year's Eve" },
};

bool IsVietnameseLunarYearEnd(int year, int month, int day,
                            int lunarDay, int lunarMonth, bool leap) {
    if (leap || lunarMonth != 12 || lunarDay < 29) return false;
    static const int monthDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int days = monthDays[month - 1];
    if (month == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) ++days;
    if (++day > days) {
        day = 1;
        if (++month > 12) { month = 1; ++year; }
    }
    int nextDay, nextMonth, nextYear;
    bool nextLeap;
    std::wstring nextCanChi;
    GetVietnameseLunarDate(year, month, day, nextDay, nextMonth, nextYear, nextLeap, nextCanChi);
    return nextDay == 1 && nextMonth == 1 && !nextLeap;
}

std::wstring GetVietnameseHolidayNames(int sDay, int sMonth,
                                      int lDay, int lMonth, bool isLeap, bool vi,
                                      bool lunarYearEnd = false) {
    std::wstring names;
    auto addName = [&](PCWSTR name) {
        if (!names.empty()) names += L" • ";
        names += name;
    };
    for (const auto& entry : kSolarHolidays) {
        if (sDay == entry.day && sMonth == entry.month)
            addName(PickText(vi, entry.viName, entry.enName));
    }
    if (!isLeap) {
        bool matchedLunarFestival = false;
        for (const auto& entry : kLunarHolidays) {
            bool matches = entry.day == 0 ? lunarYearEnd
                : (lDay == entry.day && lMonth == entry.month);
            if (matches) {
                addName(PickText(vi, entry.viName, entry.enName));
                matchedLunarFestival = true;
            }
        }
        if (!matchedLunarFestival) {
            if (lDay == 1) addName(PickText(vi, L"Mùng 1 đầu tháng (Sóc)", L"New Moon Day"));
            else if (lDay == 15) addName(PickText(vi, L"Ngày Rằm (Vọng)", L"Full Moon Day"));
        }
    }
    return names;
}

std::wstring BuildHolidayCatalogText(bool vi) {
    std::wstring text = PickText(vi, L"LỄ TRUYỀN THỐNG · ÂM LỊCH\n\n",
                                    L"TRADITIONAL FESTIVALS · LUNAR CALENDAR\n\n");
    auto append = [&](HolidayEntry const& entry, bool lunar) {
        if (entry.day == 0) {
            text += PickText(vi, L"Ngày cuối tháng 12 ÂL (29 hoặc 30)",
                                L"Last day of lunar month 12 (29 or 30)");
        } else {
            if (entry.day < 10) text += L"0";
            text += std::to_wstring(entry.day) + L"/";
            if (entry.month < 10) text += L"0";
            text += std::to_wstring(entry.month);
            if (lunar) text += PickText(vi, L" ÂL", L" lunar");
        }
        text += L" — ";
        text += PickText(vi, entry.viName, entry.enName);
        text += L"\n\n";
    };
    for (const auto& entry : kLunarHolidays) append(entry, true);
    text += PickText(vi, L"NGÀY LỄ & KỶ NIỆM · DƯƠNG LỊCH\n\n",
                        L"HOLIDAYS & COMMEMORATIONS · SOLAR CALENDAR\n\n");
    for (const auto& entry : kSolarHolidays) append(entry, false);
    text += PickText(vi, L"Mùng 1 và ngày Rằm: hiển thị theo từng tháng âm lịch.\n"
                        L"Các lễ âm lịch không lặp lại trong tháng nhuận.",
                        L"New Moon and Full Moon days are shown each lunar month.\n"
                        L"Lunar festivals do not repeat in leap months.");
    return text;
}


enum class FiveElement { Metal, Wood, Water, Fire, Earth };

struct ComputedHoroscope {
    int sDay = 1, sMonth = 1, sYear = 2000;
    int lDay = 1, lMonth = 1, lYear = 2000;
    bool isLeap = false;
    int canDayIdx = 0;
    int chiDayIdx = 0;
    PCWSTR weekdayStr = L"";
    PCWSTR zodiacEmoji = L"🐉";
    PCWSTR zodiacAnimalName = L"";
    FiveElement element = FiveElement::Metal;
    std::wstring hourCanChi;
    std::wstring dayCanChi;
    std::wstring monthCanChi;
    std::wstring yearCanChi;
    PCWSTR napAmStr = L"";
    PCWSTR starName = L"";
    PCWSTR starShortName = L"";
    bool isHoangDao = false;
    PCWSTR trucName = L"";
    PCWSTR goodFor = L"";
    PCWSTR badFor = L"";
    PCWSTR gioHoangDao = L"";
    std::wstring tuoiXung;
    PCWSTR huongHyThan = L"";
    PCWSTR huongTaiThan = L"";
    std::wstring specialHoliday;
};

ComputedHoroscope ComputeHoroscopeForDate(int sYear, int sMonth, int sDay, bool vi, int hour = 0) {
    ComputedHoroscope info;
    info.sYear = sYear;
    info.sMonth = sMonth;
    info.sDay = sDay;

    GetVietnameseLunarDate(sYear, sMonth, sDay,
                           info.lDay, info.lMonth, info.lYear,
                           info.isLeap, info.yearCanChi);

    int jdn = CalculateJDN(sDay, sMonth, sYear);

    static const PCWSTR kWeekdaysVi[] = {
        L"Thứ Hai", L"Thứ Ba", L"Thứ Tư", L"Thứ Năm", L"Thứ Sáu", L"Thứ Bảy", L"Chủ Nhật"
    };
    static const PCWSTR kWeekdaysEn[] = {
        L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday", L"Sunday"
    };
    info.weekdayStr = vi ? kWeekdaysVi[jdn % 7] : kWeekdaysEn[jdn % 7];

    info.canDayIdx = (jdn + 9) % 10;
    info.chiDayIdx = (jdn + 1) % 12;
    info.yearCanChi = LocalizedCanChi((info.lYear + 6) % 10, (info.lYear + 8) % 12, vi);
    info.dayCanChi = LocalizedCanChi(info.canDayIdx, info.chiDayIdx, vi);
    info.zodiacEmoji = ZODIAC_EMOJI_12[info.chiDayIdx];
    info.zodiacAnimalName = vi ? ZODIAC_VI_NAME_12[info.chiDayIdx] : ZODIAC_EN_NAME_12[info.chiDayIdx];

    int canMonthIdx = (info.lYear * 12 + info.lMonth + 3) % 10;
    int chiMonthIdx = (info.lMonth + 1) % 12;
    info.monthCanChi = LocalizedCanChi(canMonthIdx, chiMonthIdx, vi);

    int chiHourIdx = ((hour + 1) / 2) % 12;
    int canHourIdx = ((info.canDayIdx % 5) * 2 + chiHourIdx) % 10;
    info.hourCanChi = LocalizedCanChi(canHourIdx, chiHourIdx, vi);

    struct NapAmEntry { PCWSTR vi; PCWSTR en; FiveElement elem; };
    static const NapAmEntry kNapAm[30] = {
        { L"Hải Trung Kim (Vàng trong biển)",    L"Sea Metal",        FiveElement::Metal },
        { L"Lô Trung Hỏa (Lửa trong lò)",        L"Furnace Fire",      FiveElement::Fire  },
        { L"Đại Lâm Mộc (Gỗ rừng già)",          L"Forest Wood",        FiveElement::Wood  },
        { L"Lộ Bàng Thổ (Đất ven đường)",        L"Roadside Earth",     FiveElement::Earth },
        { L"Kiếm Phong Kim (Vàng mũi kiếm)",     L"Sword Metal",     FiveElement::Metal },
        { L"Sơn Đầu Hỏa (Lửa đầu núi)",          L"Mountain Fire",      FiveElement::Fire  },
        { L"Giản Hạ Thủy (Nước dưới khe)",       L"Stream Water",      FiveElement::Water },
        { L"Thành Đầu Thổ (Đất tường thành)",    L"Fortress Earth",   FiveElement::Earth },
        { L"Bạch Lạp Kim (Vàng chân đèn)",       L"Candlestick Metal", FiveElement::Metal },
        { L"Dương Liễu Mộc (Gỗ cây liễu)",       L"Willow Wood",     FiveElement::Wood  },
        { L"Tuyền Trung Thủy (Nước trong suối)", L"Spring Water",  FiveElement::Water },
        { L"Ốc Thượng Thổ (Đất mái nhà)",        L"Roof Earth",       FiveElement::Earth },
        { L"Tích Lịch Hỏa (Lửa sấm sét)",        L"Thunderbolt Fire", FiveElement::Fire  },
        { L"Tùng Bách Mộc (Gỗ tùng bách)",       L"Pine Wood",        FiveElement::Wood  },
        { L"Trường Lưu Thủy (Nước chảy dài)",    L"River Water",    FiveElement::Water },
        { L"Sa Trung Kim (Vàng trong cát)",      L"Sand Metal",       FiveElement::Metal },
        { L"Sơn Hạ Hỏa (Lửa dưới núi)",          L"Foothill Fire",       FiveElement::Fire  },
        { L"Bình Địa Mộc (Gỗ đồng bằng)",        L"Plain Wood",        FiveElement::Wood  },
        { L"Bích Thượng Thổ (Đất tò vò)",        L"Wall Earth",     FiveElement::Earth },
        { L"Kim Bạch Kim (Vàng pha bạc)",        L"Gold Foil Metal",   FiveElement::Metal },
        { L"Phúc Đăng Hỏa (Lửa đèn chụp)",       L"Lamp Fire",        FiveElement::Fire  },
        { L"Thiên Hà Thủy (Nước ngân hà)",       L"Celestial Water",  FiveElement::Water },
        { L"Đại Trạch Thổ (Đất nền nhà)",        L"Marsh Earth",      FiveElement::Earth },
        { L"Thoa Xuyến Kim (Vàng trang sức)",    L"Jewelry Metal",   FiveElement::Metal },
        { L"Tang Đố Mộc (Gỗ cây dâu)",           L"Mulberry Wood",      FiveElement::Wood  },
        { L"Đại Khê Thủy (Nước khe lớn)",        L"Rapids Water",      FiveElement::Water },
        { L"Sa Trung Thổ (Đất pha cát)",         L"Sand Earth",        FiveElement::Earth },
        { L"Thiên Thượng Hỏa (Lửa trên trời)",   L"Solar Fire",    FiveElement::Fire  },
        { L"Thạch Lựu Mộc (Gỗ cây lựu)",         L"Pomegranate Wood", FiveElement::Wood  },
        { L"Đại Hải Thủy (Nước biển lớn)",       L"Ocean Water",       FiveElement::Water },
    };
    int hoaGiapIdx = (6 * info.canDayIdx - 5 * info.chiDayIdx + 60) % 60;
    info.napAmStr = PickText(vi, kNapAm[hoaGiapIdx / 2].vi, kNapAm[hoaGiapIdx / 2].en);
    info.element = kNapAm[hoaGiapIdx / 2].elem;

    struct StarInfo { PCWSTR fullName; PCWSTR shortName; bool isHoangDao; };
    static const StarInfo kStars[12] = {
        { L"Thanh Long Hoàng Đạo", L"Thanh Long (Hoàng Đạo)", true  },
        { L"Minh Đường Hoàng Đạo", L"Minh Đường (Hoàng Đạo)", true  },
        { L"Thiên Hình Hắc Đạo",   L"Thiên Hình (Hắc Đạo)",   false },
        { L"Chu Tước Hắc Đạo",     L"Chu Tước (Hắc Đạo)",     false },
        { L"Kim Quỹ Hoàng Đạo",    L"Kim Quỹ (Hoàng Đạo)",    true  },
        { L"Kim Đường Hoàng Đạo",  L"Kim Đường (Hoàng Đạo)",  true  },
        { L"Bạch Hổ Hắc Đạo",      L"Bạch Hổ (Hắc Đạo)",      false },
        { L"Ngọc Đường Hoàng Đạo", L"Ngọc Đường (Hoàng Đạo)", true  },
        { L"Thiên Lao Hắc Đạo",    L"Thiên Lao (Hắc Đạo)",    false },
        { L"Nguyên Vũ Hắc Đạo",    L"Nguyên Vũ (Hắc Đạo)",    false },
        { L"Tư Mệnh Hoàng Đạo",    L"Tư Mệnh (Hoàng Đạo)",    true  },
        { L"Câu Trận Hắc Đạo",     L"Câu Trận (Hắc Đạo)",     false }
    };
    int thanhLongStartChi = ((info.lMonth - 1) % 6) * 2;
    int starIdx = (info.chiDayIdx - thanhLongStartChi + 12) % 12;
    static const PCWSTR starsEn[] = {
        L"Azure Dragon", L"Bright Hall", L"Heavenly Punishment", L"Vermilion Bird", L"Golden Coffer", L"Golden Hall",
        L"White Tiger", L"Jade Hall", L"Heavenly Prison", L"Black Tortoise", L"Life Governor", L"Hooked Array"
    };
    info.starName = vi ? kStars[starIdx].fullName : starsEn[starIdx];
    info.starShortName = vi ? kStars[starIdx].shortName : starsEn[starIdx];
    info.isHoangDao = kStars[starIdx].isHoangDao;

    struct TrucInfo {
        PCWSTR name;
        PCWSTR goodVi;
        PCWSTR badVi;
        PCWSTR goodEn;
        PCWSTR badEn;
    };
    static const TrucInfo kTruc[12] = {
        { L"Kiến",  L"Xuất hành, nhậm chức, cầu tài, ký kết",         L"Động thổ, đào đất, mở kho",
                    L"Travel, starting a role, signing contracts",     L"Groundbreaking, excavation" },
        { L"Trừ",   L"Thanh tẩy, chữa bệnh, dọn dẹp, cầu bình an",    L"Khai trương lớn, đi xa",
                    L"Cleansing, medical treatment, decluttering",     L"Grand openings, long journeys" },
        { L"Mãn",   L"Cúng tế, cầu tài lộc, khai trương, mở kho",      L"Kiện tụng, tranh chấp",
                    L"Prayers, wealth seeking, business opening",      L"Litigation, disputes" },
        { L"Bình",  L"Tu sửa, bàn bạc công việc, giao dịch, cưới hỏi", L"Khởi công lớn, thưa kiện",
                    L"Repairs, negotiations, trading, weddings",       L"Major construction, lawsuits" },
        { L"Định",  L"Ký hợp đồng, cầu danh, nhập học, mưu sự",        L"Xuất hành xa, kiện tụng",
                    L"Contracts, enrollment, long-term planning",      L"Long-distance travel, lawsuits" },
        { L"Chấp",  L"Xây dựng, tu tạo, lập khế ước, tuyển dụng",     L"Xuất tiền lớn, dọn nhà",
                    L"Building, renovations, hiring, agreements",      L"Large expenses, moving house" },
        { L"Phá",   L"Phá dỡ, giải quyết việc tồn đọng, chữa bệnh",    L"Khai trương, cưới hỏi, ký kết",
                    L"Demolition, resolving backlog, medical care",    L"Grand openings, weddings, contracts" },
        { L"Nguy",  L"Tế tự, cầu an, nghỉ ngơi, cẩn trọng đi lại",     L"Mạo hiểm, khởi công, đi sông nước",
                    L"Prayers for peace, rest, cautious travel",       L"High-risk activities, construction" },
        { L"Thành", L"Khai trương, nhập học, cưới hỏi, cầu tài lộc",   L"Kiện tụng, tranh cãi",
                    L"Grand openings, study, weddings, investments",   L"Litigation, arguments" },
        { L"Thu",   L"Thu hoạch, tích trữ, đòi nợ, cất giữ tài sản",   L"Khởi công mới, xuất hành xa",
                    L"Harvesting, saving, collecting debts",           L"Starting new projects, long trips" },
        { L"Khai",  L"Khai trương, động thổ, xuất hành, nhậm chức",    L"An táng, tranh chấp",
                    L"Grand openings, groundbreaking, travel",         L"Burials, disputes" },
        { L"Bế",    L"Đắp đê, tu bổ, cất giữ tài vật, lập kế hoạch",   L"Khai trương, xuất hành xa",
                    L"Maintenance, saving assets, planning",           L"Grand openings, long journeys" },
    };
    int trucIdx = (info.chiDayIdx - chiMonthIdx + 12) % 12;
    static const PCWSTR officersEn[] = {L"Establish", L"Remove", L"Full", L"Balance", L"Stable", L"Hold",
        L"Break", L"Danger", L"Success", L"Receive", L"Open", L"Close"};
    info.trucName = vi ? kTruc[trucIdx].name : officersEn[trucIdx];
    info.goodFor = PickText(vi, kTruc[trucIdx].goodVi, kTruc[trucIdx].goodEn);
    info.badFor = PickText(vi, kTruc[trucIdx].badVi, kTruc[trucIdx].badEn);

    static const PCWSTR kGioHoangDao[6] = {
        L"Tý (23-1h), Sửu (1-3h), Mão (5-7h), Ngọ (11-13h), Thân (15-17h), Dậu (17-19h)",
        L"Dần (3-5h), Mão (5-7h), Tỵ (9-11h), Thân (15-17h), Tuất (19-21h), Hợi (21-23h)",
        L"Tý (23-1h), Sửu (1-3h), Thìn (7-9h), Tỵ (9-11h), Mùi (13-15h), Tuất (19-21h)",
        L"Tý (23-1h), Dần (3-5h), Mão (5-7h), Ngọ (11-13h), Mùi (13-15h), Dậu (17-19h)",
        L"Dần (3-5h), Thìn (7-9h), Tỵ (9-11h), Thân (15-17h), Dậu (17-19h), Hợi (21-23h)",
        L"Sửu (1-3h), Thìn (7-9h), Ngọ (11-13h), Mùi (13-15h), Tuất (19-21h), Hợi (21-23h)"
    };
    static const PCWSTR kGioHoangDaoEn[6] = {
        L"Rat (23-1h), Buffalo (1-3h), Cat (5-7h), Horse (11-13h), Monkey (15-17h), Rooster (17-19h)",
        L"Tiger (3-5h), Cat (5-7h), Snake (9-11h), Monkey (15-17h), Dog (19-21h), Pig (21-23h)",
        L"Rat (23-1h), Buffalo (1-3h), Dragon (7-9h), Snake (9-11h), Goat (13-15h), Dog (19-21h)",
        L"Rat (23-1h), Tiger (3-5h), Cat (5-7h), Horse (11-13h), Goat (13-15h), Rooster (17-19h)",
        L"Tiger (3-5h), Dragon (7-9h), Snake (9-11h), Monkey (15-17h), Rooster (17-19h), Pig (21-23h)",
        L"Buffalo (1-3h), Dragon (7-9h), Horse (11-13h), Goat (13-15h), Dog (19-21h), Pig (21-23h)"
    };
    info.gioHoangDao = vi ? kGioHoangDao[info.chiDayIdx % 6] : kGioHoangDaoEn[info.chiDayIdx % 6];

    int xungChiIdx = (info.chiDayIdx + 6) % 12;
    int xungCan1 = (info.canDayIdx + 4) % 10;
    int xungCan2 = (info.canDayIdx + 6) % 10;
    info.tuoiXung = LocalizedCanChi(xungCan1, xungChiIdx, vi) + L", " + LocalizedCanChi(xungCan2, xungChiIdx, vi);

    struct BilingualDir { PCWSTR vi; PCWSTR en; };
    static const BilingualDir kHuongHyThan[10] = {
        { L"Đông Bắc", L"Northeast" }, { L"Tây Bắc",  L"Northwest" },
        { L"Tây Nam",  L"Southwest" }, { L"Chính Nam", L"South" },
        { L"Đông Nam", L"Southeast" }, { L"Đông Bắc", L"Northeast" },
        { L"Tây Bắc",  L"Northwest" }, { L"Tây Nam",  L"Southwest" },
        { L"Chính Nam", L"South" },    { L"Đông Nam", L"Southeast" },
    };
    static const BilingualDir kHuongTaiThan[10] = {
        { L"Đông Nam",  L"Southeast" }, { L"Đông Nam",  L"Southeast" },
        { L"Chính Đông", L"East" },     { L"Chính Đông", L"East" },
        { L"Chính Bắc", L"North" },     { L"Chính Nam", L"South" },
        { L"Tây Nam",   L"Southwest" }, { L"Tây Nam",   L"Southwest" },
        { L"Chính Tây", L"West" },      { L"Tây Bắc",   L"Northwest" },
    };
    info.huongHyThan = PickText(vi, kHuongHyThan[info.canDayIdx].vi, kHuongHyThan[info.canDayIdx].en);
    info.huongTaiThan = PickText(vi, kHuongTaiThan[info.canDayIdx].vi, kHuongTaiThan[info.canDayIdx].en);

    info.specialHoliday = GetVietnameseHolidayNames(
        sDay, sMonth, info.lDay, info.lMonth, info.isLeap, vi,
        IsVietnameseLunarYearEnd(sYear, sMonth, sDay, info.lDay, info.lMonth, info.isLeap));

    return info;
}

// Compact widget presentation: independent from XAML so timing can be tested.
constexpr uint64_t kHoroscopeRotationMs = 15000;

struct WidgetRotationState {
    bool initialized = false;
    int dateKey = 0;
    bool vietnamese = true;
    bool holiday = false;
    size_t rowCount = 0;
    uint64_t startedAt = 0;

    size_t Select(int today, bool vi, bool pinnedHoliday, size_t count, uint64_t now) {
        if (!initialized || dateKey != today || vietnamese != vi ||
            holiday != pinnedHoliday || rowCount != count || now < startedAt) {
            initialized = true;
            dateKey = today;
            vietnamese = vi;
            holiday = pinnedHoliday;
            rowCount = count;
            startedAt = now;
        }
        if (pinnedHoliday || count == 0) return 0;
        return static_cast<size_t>(((now - startedAt) / kHoroscopeRotationMs) % count);
    }
};

std::wstring FormatWidgetLunarHeader(ComputedHoroscope const& h, bool vi) {
    auto twoDigits = [](int n) { return (n < 10 ? L"0" : L"") + std::to_wstring(n); };
    std::wstring text = vi ? L"Âm lịch: " : L"Lunar: ";
    text += twoDigits(h.lDay) + L"/" + twoDigits(h.lMonth);
    if (h.isLeap) text += vi ? L" nhuận" : L" (leap)";
    text += vi ? L" năm " : L" year of ";
    return text + h.yearCanChi;
}

void AppendWidgetListRows(std::vector<std::wstring>& rows, PCWSTR prefix, PCWSTR list) {
    if (!list || !*list) return;
    std::wstring text = list;
    size_t start = 0;
    while (start < text.size()) {
        size_t end = text.find(L',', start);
        std::wstring item = TrimWhitespace(text.substr(start, end - start));
        if (!item.empty()) rows.push_back(std::wstring(prefix) + item);
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
}

std::vector<std::wstring> BuildWidgetHoroscopeRows(ComputedHoroscope const& h, bool vi) {
    std::vector<std::wstring> rows;
    rows.push_back(std::wstring(h.zodiacEmoji) + (vi ? L" Ngày " : L" Day ") + h.dayCanChi);
    rows.push_back(std::wstring(vi ? L"📅 Tháng " : L"📅 Month ") + h.monthCanChi);
    rows.push_back(std::wstring(vi ? L"🕒 Giờ " : L"🕒 Hour ") + h.hourCanChi);
    rows.push_back(std::wstring(vi ? L"☯ Mệnh: " : L"☯ Element: ") + h.napAmStr);
    rows.push_back(std::wstring(h.isHoangDao ? L"🌟 " : L"⚪ ") +
                   (vi ? h.starShortName : (h.isHoangDao ? L"Auspicious day" : L"Inauspicious day")));
    rows.push_back(std::wstring(vi ? L"📜 Trực: " : L"📜 Officer: ") + h.trucName);
    AppendWidgetListRows(rows, vi ? L"⏰ Giờ tốt: " : L"⏰ Good hour: ", h.gioHoangDao);
    AppendWidgetListRows(rows, vi ? L"🔮 Nên: " : L"🔮 Good for: ", h.goodFor);
    AppendWidgetListRows(rows, vi ? L"⛔ Kỵ: " : L"⛔ Avoid: ", h.badFor);
    AppendWidgetListRows(rows, vi ? L"🧭 Tuổi xung: " : L"🧭 Clash: ", h.tuoiXung.c_str());
    rows.push_back(std::wstring(vi ? L"🧭 Hỷ thần: " : L"🧭 Joy: ") + h.huongHyThan);
    rows.push_back(std::wstring(vi ? L"💰 Tài thần: " : L"💰 Wealth: ") + h.huongTaiThan);
    return rows;
}

std::wstring SelectWidgetSecondLine(ComputedHoroscope const& h, bool vi,
                                    uint64_t now, WidgetRotationState& state) {
    int today = MakeDateKey(h.sYear, h.sMonth, h.sDay);
    // Pin holiday and event names instead of rotating the almanac rows.
    if (!h.specialHoliday.empty()) {
        state.Select(today, vi, true, 1, now);
        return L"🎉 " + h.specialHoliday;
    }
    auto rows = BuildWidgetHoroscopeRows(h, vi);
    size_t index = state.Select(today, vi, false, rows.size(), now);
    return rows.empty() ? std::wstring{} : rows[index];
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar-thread-only, value data; no XAML references or article content.
ComputedHoroscope const& CachedHoroscope(SYSTEMTIME const& st, bool vi) {
    struct Cached { int year = -1, month = 0, day = 0, hour = 0; bool vi = false; ComputedHoroscope value; };
    static std::array<Cached, 2> values;
    static unsigned next = 0;
    for (auto const& item : values) if (item.year == st.wYear && item.month == st.wMonth &&
        item.day == st.wDay && item.hour == st.wHour && item.vi == vi) return item.value;
    auto& item = values[next++ % values.size()];
    item = {st.wYear, st.wMonth, st.wDay, st.wHour, vi,
        ComputeHoroscopeForDate(st.wYear, st.wMonth, st.wDay, vi, st.wHour)};
    return item.value;
}

// Taskbar.dll XAML Injection & DesktopWindowXamlSource Popup (explorer.exe)

void* CTaskBand_ITaskListWndSite_vftable = nullptr;

using CTaskBand_GetTaskbarHost_t = void**(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;

using Std_Ref_Decref_t = void(WINAPI*)(void* pThis);
Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;

std::atomic<bool> g_taskbarDllHookAttempted{false};
std::atomic<bool> g_taskbarDllHooked{false};

void RequestTaskbarWidgetInjection();
using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    // The HWND can exist before its XAML tree. Queue work after startup returns.
    if (!g_shuttingDown) RequestTaskbarWidgetInjection();
}

// Called on the taskbar UI thread; repeated status messages are rate-limited.
void ReportWidgetStatus(PCWSTR status) {
    static std::wstring lastStatus;
    static ULONGLONG lastTime = 0;
    ULONGLONG now = GetTickCount64();
    if (lastStatus != status || now - lastTime >= 30000) {
        Wh_Log(L"%s", status);
        lastStatus = status;
        lastTime = now;
    }
}

bool HookTaskbarDllSymbols(HMODULE module) {
    if (!module) return false;

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})",
            },
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const)",
                LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void) const)",
                LR"(protected: class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const)",
                LR"(protected: class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void) const)",
                LR"(protected: class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )",
            },
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const)",
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void) const)",
                LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )",
            },
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {
                LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))",
            },
            &Std_Ref_Decref_Original,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
            true, // Optional: CreateWindowExW and AfterInit also schedule injection.
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Taskbar.dll symbol resolution failed. Check symbol download/build compatibility.");
        return false;
    }
    Wh_Log(L"Taskbar.dll symbols ready; GetTaskbarHost=%p, FrameHeight=%p",
           reinterpret_cast<void*>(CTaskBand_GetTaskbarHost_Original),
           reinterpret_cast<void*>(TaskbarHost_FrameHeight_Original));
    return true;
}

void TryHookTaskbarDllSymbols(HMODULE module, bool applyHooks) {
    if (!module || g_shuttingDown || g_taskbarDllHookAttempted.exchange(true)) return;
    if (HookTaskbarDllSymbols(module)) {
        g_taskbarDllHooked = true;
        if (applyHooks) Wh_ApplyHookOperations();
    }
}

// 1. Find HWND of Shell_TrayWnd in current explorer.exe process
HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows([](HWND hwnd, LPARAM param) -> BOOL {
        DWORD pid = 0;
        wchar_t className[64]{};
        if (GetWindowThreadProcessId(hwnd, &pid) &&
            pid == GetCurrentProcessId() &&
            GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
            _wcsicmp(className, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(param) = hwnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

// 2. Extract XamlRoot from Windows 11 Taskbar internal structures
wux::XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    if (!taskbarWnd || !g_taskbarDllHooked || !CTaskBand_ITaskListWndSite_vftable ||
        !CTaskBand_GetTaskbarHost_Original || !TaskbarHost_FrameHeight_Original ||
        !Std_Ref_Decref_Original) {
        ReportWidgetStatus(L"Waiting for Taskbar.dll symbols (GetTaskbarHost/FrameHeight/_Decref).");
        return nullptr;
    }

    HWND taskSwWnd = reinterpret_cast<HWND>(GetPropW(taskbarWnd, L"TaskbandHWND"));
    if (!taskSwWnd) {
        ReportWidgetStatus(L"Waiting for TaskbandHWND.");
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskSwWnd, 0));
    if (!taskBand) {
        ReportWidgetStatus(L"Waiting for the taskband object.");
        return nullptr;
    }

    void* taskBandForSite = taskBand;
    for (int i = 0; i < 20; ++i) {
        if (*reinterpret_cast<void**>(taskBandForSite) == CTaskBand_ITaskListWndSite_vftable) {
            break;
        }
        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
        if (i == 19) {
            ReportWidgetStatus(L"ITaskListWndSite not found in taskband; unsupported layout.");
            return nullptr;
        }
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);
    struct HostReferenceGuard {
        void* controlBlock;
        ~HostReferenceGuard() {
            if (controlBlock) Std_Ref_Decref_Original(controlBlock);
        }
    } hostReferenceGuard{taskbarHostSharedPtr[1]};
    if (!taskbarHostSharedPtr[0]) {
        ReportWidgetStatus(L"Waiting for TaskbarHost.");
        return nullptr;
    }

    size_t elementOffset = 0;
    bool signatureMatched = false;
#if defined(_M_X64) || defined(__x86_64__)
    const BYTE* bytes = reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (bytes[0] == 0x48 && bytes[1] == 0x83 && bytes[2] == 0xEC &&
        bytes[4] == 0x48 && bytes[5] == 0x83 && bytes[6] == 0xC1 &&
        bytes[7] <= 0x7F) {
        elementOffset = bytes[7];
        signatureMatched = true;
    }
#elif defined(_M_ARM64) || defined(__aarch64__)
    const DWORD* words = reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    if (words[0] == 0xD503237F && (words[1] & 0xFFC07FFF) == 0xA9807BFD &&
        words[2] == 0x910003FD && (words[3] & 0xFFF00FE0) == 0xF8400C00) {
        elementOffset = (words[3] >> 12) & 0xFF;
        signatureMatched = true;
    }
#endif
    if (!signatureMatched) {
        // Never infer an object offset from leftover bytes after a detour jump.
        ReportWidgetStatus(L"Unsupported or hooked FrameHeight prologue; no guessed pointer dereference.");
        return nullptr;
    }

    auto elementUnknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + elementOffset);
    wux::FrameworkElement taskbarElement{nullptr};
    if (elementUnknown) {
        HRESULT hr = elementUnknown->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                                    winrt::put_abi(taskbarElement));
        if (FAILED(hr)) {
            ReportWidgetStatus(L"TaskbarHost element does not expose FrameworkElement.");
            return nullptr;
        }
    }

    auto root = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    if (!root) ReportWidgetStatus(L"Waiting for the taskbar XamlRoot.");
    return root;
}

// 3. Traverse XAML visual tree to find element by Name and resolve injection target
wux::FrameworkElement FindVisualElement(wux::DependencyObject const& root,
                                        std::wstring_view value, bool byClass,
                                        int depth = 48) {
    if (!root || depth <= 0) {
        return nullptr;
    }
    try {
        if (auto element = root.try_as<wux::FrameworkElement>()) {
            if (byClass ? winrt::get_class_name(element) == value : element.Name() == value)
                return element;
        }
    } catch (...) {}

    int count = 0;
    try { count = wuxm::VisualTreeHelper::GetChildrenCount(root); } catch (...) { return nullptr; }
    for (int i = 0; i < count; ++i) {
        wux::DependencyObject child{nullptr};
        try {
            child = wuxm::VisualTreeHelper::GetChild(root, i);
        } catch (...) { continue; }
        if (!child) {
            continue;
        }
        // A non-FrameworkElement visual can still contain the tray. Traverse it.
        if (auto found = FindVisualElement(child, value, byClass, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

wux::FrameworkElement FindChildByName(wux::FrameworkElement const& root,
                                      std::wstring_view name, int depth = 48) {
    return FindVisualElement(root, name, false, depth);
}

wux::FrameworkElement FindTaskbarFrame(wux::FrameworkElement const& root, int depth = 8) {
    if (!root || depth <= 0) return nullptr;
    // Some builds identify the frame by runtime class, without the expected Name.
    if (winrt::get_class_name(root) == L"Taskbar.TaskbarFrame") return root;
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i).try_as<wux::FrameworkElement>();
        if (auto frame = FindTaskbarFrame(child, depth - 1)) return frame;
    }
    return nullptr;
}

// Called only on the taskbar UI thread; no UI reference is retained by the worker.
std::wstring NativeTooltipText(wf::IInspectable const& value, unsigned depth = 0) {
    if (!value || depth > 4) return {};
    if (auto property = value.try_as<wf::IPropertyValue>(); property && property.Type() == wf::PropertyType::String)
        return std::wstring(property.GetString());
    if (auto text = value.try_as<wuxc::TextBlock>()) return std::wstring(text.Text());
    if (auto content = value.try_as<wuxc::ContentControl>()) return NativeTooltipText(content.Content(), depth + 1);
    if (auto panel = value.try_as<wuxc::Panel>()) {
        std::wstring text;
        for (auto const& child : panel.Children()) {
            auto part = NativeTooltipText(child, depth + 1);
            if (!part.empty()) { if (!text.empty()) text += L"\n"; text += part; }
            if (text.size() > 1024) break;
        }
        return text.substr(0, 1024);
    }
    return {};
}
// A combined native tooltip contains several sensor sections. Only consume the
// location section; never turn the microphone/camera heading into an app name.
std::wstring LocationAppsFromLabel(std::wstring label, bool locationElement) {
    if (label.empty() || label.size() > 1024) return {};
    auto lower = LowerText(label);
    size_t begin = std::wstring::npos;
    for (PCWSTR token : {L"location", L"vị trí"}) {
        auto pos = lower.find(token);
        if (pos != std::wstring::npos && (begin == std::wstring::npos || pos < begin)) begin = pos;
    }
    if (begin == std::wstring::npos) return {}; // An element ID alone is not attribution.
    (void)locationElement;
    auto separator = label.find_first_of(L":\n", begin);
    if (separator == std::wstring::npos) return {};
    auto end = label.size();
    for (PCWSTR token : {L"microphone", L"micrô", L"micro ", L"camera", L"máy ảnh", L"máy quay"}) {
        auto pos = lower.find(token, separator + 1);
        if (pos != std::wstring::npos) {
            auto line = label.rfind(L'\n', pos);
            end = std::min(end, line == std::wstring::npos ? pos : line);
        }
    }
    if (end <= separator) return {};
    std::wstring result;
    auto section = label.substr(separator + 1, end - separator - 1);
    size_t pos = 0;
    std::set<std::wstring> seen;
    while (pos < section.size()) {
        auto next = section.find_first_of(L"\r\n", pos);
        auto line = section.substr(pos, next == std::wstring::npos ? next : next - pos);
        auto first = line.find_first_not_of(L" \t"), last = line.find_last_not_of(L" \t");
        if (first != std::wstring::npos) {
            line = line.substr(first, last - first + 1);
            auto key = LowerText(line);
            if (line.size() <= 180 && line.find(L':') == std::wstring::npos && seen.insert(key).second) {
                if (!result.empty()) result += L"\n";
                result += line;
            }
        }
        if (next == std::wstring::npos) break;
        pos = next + 1;
    }
    return result;
}
void RefreshNativeLocationLabel(HWND taskbar) {
    static uint64_t next = 0;
    auto now = GetTickCount64(); if (now < next) return; next = now + 1000;
    std::wstring apps;
    try {
        auto settings = GetSettings();
        auto root = settings.enablePrivacyWidget && settings.enableTaskbarWidget ? GetTaskbarXamlRoot(taskbar) : nullptr;
        auto content = root ? root.Content().try_as<wux::FrameworkElement>() : nullptr;
        auto tray = content ? FindVisualElement(content, L"SystemTray.SystemTrayFrame", true, 16) : nullptr;
        if (!tray && content) tray = FindChildByName(content, L"SystemTrayFrame", 16);
        if (!tray && content) tray = FindChildByName(content, L"SystemTrayFrameGrid", 16);
        std::vector<wux::FrameworkElement> pending;
        if (tray) pending.push_back(tray);
        for (unsigned visited = 0; !pending.empty() && visited < 256; ++visited) {
            auto element = pending.back(); pending.pop_back();
            if (element.Visibility() != wux::Visibility::Visible || element.Name() == L"WindhawkVnLunarTaskbarWidget") continue;
            auto type = std::wstring(winrt::get_class_name(element));
            auto id = std::wstring(element.Name()) + L" " + std::wstring(wux::Automation::AutomationProperties::GetAutomationId(element));
            bool location = LowerText(id + L" " + type).find(L"location") != std::wstring::npos;
            auto name = std::wstring(wux::Automation::AutomationProperties::GetName(element));
            apps = LocationAppsFromLabel(name, location);
            if (apps.empty() && (location || type.find(L"IconView") != std::wstring::npos || type.find(L"OmniButton") != std::wstring::npos)) try {
                auto tooltip = NativeTooltipText(wuxc::ToolTipService::GetToolTip(element));
                apps = LocationAppsFromLabel(tooltip, location);
                if (apps.empty()) {
                    namespace peers = wux::Automation::Peers;
                    auto peer = peers::FrameworkElementAutomationPeer::FromElement(element);
                    if (!peer) peer = peers::FrameworkElementAutomationPeer::CreatePeerForElement(element);
                    if (peer) apps = LocationAppsFromLabel(std::wstring(peer.GetName()), location);
                }
            } catch (...) {} // One unavailable peer must not stop the rest of the tray scan.
            if (!apps.empty()) break;
            int count = wuxm::VisualTreeHelper::GetChildrenCount(element);
            for (int i = 0; i < count && pending.size() < 256; ++i)
                if (auto child = wuxm::VisualTreeHelper::GetChild(element, i).try_as<wux::FrameworkElement>()) pending.push_back(child);
        }
    } catch (...) {}
    bool changed;
    { std::lock_guard<std::mutex> lock(g_liveMutex);
      changed = g_nativeLocation.apps != apps;
      g_nativeLocation = {std::move(apps), now}; }
    if (changed) { ++g_nativeLocationRevision; if (g_liveWakeEvent) SetEvent(g_liveWakeEvent); }
}

wuxc::Grid FindTaskbarRootGrid(wux::FrameworkElement const& root) {
    auto frame = FindTaskbarFrame(root);
    if (!frame) frame = FindChildByName(root, L"TaskbarFrame", 8);
    if (!frame) return nullptr;
    if (auto control = frame.try_as<wuxc::Control>()) control.ApplyTemplate();

    // Only use the frame's own grid, never a button's similarly named RootGrid.
    int count = wuxm::VisualTreeHelper::GetChildrenCount(frame);
    for (int i = 0; i < count; ++i) {
        auto grid = wuxm::VisualTreeHelper::GetChild(frame, i).try_as<wuxc::Grid>();
        if (grid && grid.Name() == L"RootGrid") return grid;
    }

    // Allow template wrappers, anchoring the search to the taskbar's repeater.
    auto repeater = FindChildByName(frame, L"TaskbarFrameRepeater", 8);
    wux::DependencyObject ancestor = repeater;
    for (int i = 0; ancestor && i < 8; ++i) {
        ancestor = wuxm::VisualTreeHelper::GetParent(ancestor);
        if (!ancestor || ancestor == frame) break;
        auto grid = ancestor.try_as<wuxc::Grid>();
        if (grid && grid.Name() == L"RootGrid") return grid;
    }
    return nullptr;
}

struct InjectionTarget {
    wuxc::Panel panel{nullptr};
    int column = -1;
    int childIndex = -1;
    bool overlay = false;
};

wux::FrameworkElement GetDirectPanelChild(wuxc::Panel const& panel,
                                           wux::FrameworkElement element) {
    wux::DependencyObject current = element;
    for (int i = 0; current && i < 32; ++i) {
        auto parent = wuxm::VisualTreeHelper::GetParent(current);
        if (parent == panel) return current.try_as<wux::FrameworkElement>();
        current = parent;
    }
    return nullptr;
}

int GetPanelChildIndex(wuxc::Panel const& panel, wux::FrameworkElement const& element) {
    auto child = GetDirectPanelChild(panel, element);
    uint32_t index = 0;
    if (child && panel.Children().IndexOf(child, index)) return static_cast<int>(index);
    return -1;
}

wux::FrameworkElement FindTrayClock(wuxc::Panel const& panel) {
    // Resolve the containing child, not a clock text block's stale Grid.Column.
    for (PCWSTR name : {L"NotificationCenterButton", L"ClockButton", L"ClockStack",
                        L"SecondaryClockStack", L"Clock"}) {
        auto anchor = FindChildByName(panel, name);
        auto child = GetDirectPanelChild(panel, anchor);
        if (child && child.Visibility() == wux::Visibility::Visible) return child;
    }
    return nullptr;
}

wuxc::Panel FindSystemTrayPanel(wux::FrameworkElement const& root) {
    auto named = FindChildByName(root, L"SystemTrayFrameGrid");
    if (named) {
        if (auto panel = named.try_as<wuxc::Panel>()) return panel;
    }
    auto frame = FindVisualElement(root, L"SystemTray.SystemTrayFrame", true);
    if (!frame) frame = FindChildByName(root, L"SystemTrayFrame");
    if (frame) {
        if (auto control = frame.try_as<wuxc::Control>()) control.ApplyTemplate();
        named = FindChildByName(frame, L"SystemTrayFrameGrid");
        if (named) {
            if (auto panel = named.try_as<wuxc::Panel>()) return panel;
        }
    }

    // Template names can change. Find the nearest layout panel shared by the
    // clock and another tray control; never pick an inner clock-content grid.
    auto scope = frame ? frame : root;
    auto clock = FindChildByName(scope, L"NotificationCenterButton");
    if (!clock) clock = FindChildByName(scope, L"ClockButton");
    if (!clock) clock = FindChildByName(scope, L"ClockStack");
    auto peer = FindChildByName(scope, L"ShowDesktopStack");
    if (!peer) peer = FindChildByName(scope, L"ControlCenterButton");
    wux::DependencyObject current = clock;
    for (int i = 0; current && peer && i < 32; ++i) {
        current = wuxm::VisualTreeHelper::GetParent(current);
        if (!current || current == frame) break;
        auto panel = current.try_as<wuxc::Panel>();
        if (!panel) continue;
        if (!panel.try_as<wuxc::Grid>() && !panel.try_as<wuxc::StackPanel>()) continue;
        auto clockChild = GetDirectPanelChild(panel, clock);
        auto peerChild = GetDirectPanelChild(panel, peer);
        if (clockChild && peerChild && clockChild != peerChild) return panel;
    }
    return nullptr;
}

void LogTrayLookupFailure(wux::FrameworkElement const& root, PCWSTR position) {
    static ULONGLONG lastLogged = 0;
    auto now = GetTickCount64();
    if (lastLogged && now - lastLogged < 30000) return;
    lastLogged = now;
    Wh_Log(L"Target unavailable: position=%s; visual tree follows", position);
    int remaining = 100;
    auto dump = [&](auto&& self, wux::DependencyObject const& node, int depth) -> void {
        if (!node || depth > 12 || remaining-- <= 0) return;
        try {
            auto fe = node.try_as<wux::FrameworkElement>();
            Wh_Log(L"[Widget tree] depth=%d class=%s name=%s", depth,
                   winrt::get_class_name(node).c_str(), fe ? fe.Name().c_str() : L"");
            int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
            for (int i = 0; i < count && remaining > 0; ++i)
                self(self, wuxm::VisualTreeHelper::GetChild(node, i), depth + 1);
        } catch (...) {}
    };
    dump(dump, root, 0);
}

InjectionTarget ResolveInjectionTarget(wux::FrameworkElement const& root,
                                       ModSettings const& settings) {
    if (settings.position != L"root_left") {
        auto panel = FindSystemTrayPanel(root);
        if (!panel) return {};

        // New Explorer builds retain the name SystemTrayFrameGrid but use a
        // StackPanel. Its child order drives layout; Grid.Column is ignored.
        if (auto stack = panel.try_as<wuxc::StackPanel>()) {
            int index = -1;
            if (settings.position == L"tray_left") {
                index = 0;
            } else if (settings.position == L"tray_right") {
                index = static_cast<int>(stack.Children().Size());
            } else if (settings.position == L"tray_before_clock" ||
                       settings.position == L"tray_after_clock") {
                index = GetPanelChildIndex(panel, FindTrayClock(panel));
                if (index >= 0 && settings.position == L"tray_after_clock") ++index;
            }
            if (index >= 0) return {panel, -1, index, false};
            return {};
        }

        if (auto trayGrid = panel.try_as<wuxc::Grid>()) {
            int col = -1;
            if (settings.position == L"tray_left") {
                col = 0;
            } else if (settings.position == L"tray_right") {
                col = std::max(1, static_cast<int>(trayGrid.ColumnDefinitions().Size()));
            } else if (settings.position == L"tray_before_clock" ||
                       settings.position == L"tray_after_clock") {
                if (auto clock = FindTrayClock(panel)) {
                    col = wuxc::Grid::GetColumn(clock);
                    if (settings.position == L"tray_after_clock")
                        col += std::max(1, wuxc::Grid::GetColumnSpan(clock));
                }
            }
            if (col >= 0) return {panel, col, -1, false};
        }
        return {}; // Wait for the requested tray target instead of silently moving left.
    }

    auto rootGrid = FindTaskbarRootGrid(root);
    if (rootGrid) {
        return {rootGrid, 0, -1, true};
    }

    return {};
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar Compact Widget & Expanded Popup State (Taskbar UI Thread)

constexpr PCWSTR kTaskbarWidgetElementName = L"WindhawkVnLunarTaskbarWidget";
constexpr PCWSTR kExpandedPopupWindowClass = L"Windhawk_VnLunarExpandedPopup_Class";
constexpr UINT_PTR kTaskbarEnsureTimerId   = 0x202601;

HWND g_taskbarWnd = nullptr;
bool g_taskbarTimerStarted = false;
bool g_ensuringTaskbarWidget = false;
// Release every strong UI reference on the taskbar thread during explicit
// teardown; never release these objects from CRT destructors at process exit.
[[clang::no_destroy]] wuxc::Panel g_injectedParentPanel{nullptr};
[[clang::no_destroy]] wuxc::ColumnDefinition g_injectedColumnDef{nullptr};
[[clang::no_destroy]] wuxc::ColumnDefinition g_injectedImplicitColumnDef{nullptr};
struct ShiftedTrayChild {
    winrt::weak_ref<wux::FrameworkElement> element;
    int column = 0;
    int span = 1;
    bool moved = false;
    bool widened = false;
};
// Contains only weak UI references; automatic destruction is safe at process exit.
std::vector<ShiftedTrayChild> g_shiftedTrayChildren;
[[clang::no_destroy]] wuxc::Border g_playerGrid{nullptr}; // The compact taskbar widget root border
[[clang::no_destroy]] wuxc::Border g_widgetBadgeBorder{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_widgetZodiacText{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_widgetLine1Text{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_widgetLine2Text{nullptr};
bool g_widgetHovered = false,g_widgetPressed=false;
[[clang::no_destroy]] wuxc::Grid g_widgetBackdropLayer{nullptr};
[[clang::no_destroy]] wuxc::Border g_widgetMaterialRim{nullptr};
void UpdateWidgetMaterial(bool light,bool open);
void ReleaseWidgetMaterial();
void SizeWidgetMaterial();
std::wstring g_currentInjectedPosition;
WidgetRotationState g_widgetRotation;

// Expanded Popup Window & XAML Host State
constexpr int kExpandedPopupHeightDip = 316;
constexpr UINT kPopupOpenDurationMs = 300;
constexpr UINT kPopupCloseDurationMs = 240;
constexpr UINT_PTR kPopupCloseTimerId = 0x202602;
bool g_popupClosing = false;
ULONGLONG g_popupCloseDeadline = 0;
HWND g_expandedPopup = nullptr;
HWND g_popupXamlChild = nullptr;
[[clang::no_destroy]] wuxh::DesktopWindowXamlSource g_popupXamlSource{nullptr};
bool g_popupBackdropDirty = true;
int g_popupBackdropTheme = -1;
[[clang::no_destroy]] wuxc::Border g_popupRootCard{nullptr};
[[clang::no_destroy]] wuxc::Border g_popupBackdropLayer{nullptr};
[[clang::no_destroy]] wuxc::Border g_popupGlassRim{nullptr};
[[clang::no_destroy]] wuxc::Border g_popupGlassInnerRim{nullptr};
[[clang::no_destroy]] wcomp::ContainerVisual g_popupBackdropVisual{nullptr};
[[clang::no_destroy]] wcomp::CompositionColorBrush g_popupTintBrush{nullptr};
[[clang::no_destroy]] wuxc::Border g_popupBadgeBorder{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupZodiacText{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupTitleSolar{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupTitleLunar{nullptr};
[[clang::no_destroy]] wuxc::Border g_popupHolidayBanner{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupHolidayText{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupRowBattu{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupRowMenhTruc{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupRowGioHoangDao{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupRowNenKy{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupRowXungHuong{nullptr};
[[clang::no_destroy]] wuxc::ScrollViewer g_popupHoroscopeScroll{nullptr};
[[clang::no_destroy]] wuxc::ScrollViewer g_popupHolidayScroll{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupHolidayListText{nullptr};
[[clang::no_destroy]] wuxc::Button g_popupHolidayListButton{nullptr};
bool g_popupShowingHolidays = false;
int g_popupHolidayListLanguage = -1;

RECT g_popupSourceRect{};
RECT g_popupFinalRect{};
RECT g_popupHostRect{};
RECT g_lastPopupWindowRect{};
bool g_popupOpensDown = false;
bool g_taskbarAtTop = false;
int g_popupDayOffset = 0;

winrt::Windows::UI::Color GetFiveElementColor(FiveElement elem, bool isLightTheme);

enum class PopupMode { Hidden, Automatic, Manual };
constexpr UINT_PTR kPopupActivityTimerId = 0x202603;
PopupMode g_popupMode = PopupMode::Hidden;
ULONGLONG g_popupAutoDeadline = 0;
bool g_popupLastPrimaryDown = false;
ActivityKind g_selectedActivity = ActivityKind::Lunar;

LiveSnapshot g_uiLive;
BatteryAlertTracker g_batteryAlertTracker;
BatteryAlert g_batteryNotice = BatteryAlert::None;
BluetoothAlertTracker g_bluetoothAlerts;
BluetoothDeviceBattery g_bluetoothNotice;
uint64_t g_bluetoothNoticeUntil = 0;
std::map<ActivityKind, std::wstring> g_hiddenActivities;
[[clang::no_destroy]] wuxc::Button g_quickSettings{nullptr};
[[clang::no_destroy]] wuxc::Button g_collapseTaskbar{nullptr};
[[clang::no_destroy]] wuxc::MenuFlyout g_quickMenu{nullptr};
bool g_quickMenuOpen = false;
[[clang::no_destroy]] wuxc::Button g_hideActivity{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_bluetoothList{nullptr};
std::vector<BluetoothDeviceBattery> g_renderedBluetooth;
int g_renderedBluetoothLanguage = -1;
void ShowQuickSettings();
void HideCurrentActivity();
bool g_batteryNeedsAttention = false;
std::vector<ActivityKind> g_renderedTabs;
int g_renderedTabLanguage = -1;
ActivityKind g_renderedSelectedTab = ActivityKind::Download;
bool g_renderedBatteryAttention = false;

// Released on the taskbar thread together with the existing XAML controls.
[[clang::no_destroy]] wuxc::Button g_widgetPreviousButton{nullptr};
[[clang::no_destroy]] wuxc::Button g_widgetNextButton{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_popupCalendarNavigation{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_popupTabs{nullptr};
[[clang::no_destroy]] wuxc::ScrollViewer g_popupLiveScroll{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupLiveTitle{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupLiveDetail{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_popupLiveStatus{nullptr};
[[clang::no_destroy]] wuxc::Grid g_popupLiveProgress{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_popupMediaControls{nullptr};
[[clang::no_destroy]] wuxc::Button g_popupMediaPrevious{nullptr};
[[clang::no_destroy]] wuxc::Button g_popupMediaPlayPause{nullptr};
[[clang::no_destroy]] wuxc::Button g_popupMediaNext{nullptr};

void RefreshTaskbarWidgetData();
void RefreshExpandedPopupData();
void RequestPopupRefresh() { NotifyLiveUi(); }
void BeginCloseExpandedPopup(HWND hwnd);
bool AnimatePopupTransition(bool opening, bool freshOpen);
void CancelPopupClose(HWND hwnd);
void OpenExpandedPopup(bool automatic);
void PromotePopupToManual();

void QueueMediaAction(MediaAction action);
[[clang::no_destroy]] wuxc::Viewbox g_widgetLunarHeaderView{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_widgetLiveTitle{nullptr};
[[clang::no_destroy]] wuxc::ColumnDefinition g_widgetPreviousColumn{nullptr};
[[clang::no_destroy]] wuxc::ColumnDefinition g_widgetNextColumn{nullptr};
[[clang::no_destroy]] wuxc::ScrollViewer g_popupTabsScroll{nullptr};
[[clang::no_destroy]] wuxc::ScrollViewer g_monthScroll{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_monthBody{nullptr};
[[clang::no_destroy]] wuxc::Button g_monthButton{nullptr};
[[clang::no_destroy]] wuxc::ScrollViewer g_cultureScroll{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_cultureBody{nullptr};
[[clang::no_destroy]] wuxc::TextBox g_cultureSearch{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_cultureResults{nullptr};
[[clang::no_destroy]] wuxc::FontIcon g_playPauseIcon{nullptr};
[[clang::no_destroy]] wuxm::Imaging::WriteableBitmap g_mediaBitmap{nullptr};
std::shared_ptr<ImagePixels> g_renderedArtwork;
bool g_showingMonth = false;
int g_monthOffset = 0;
int g_renderedMonth = -1, g_renderedMonthSelection = -1, g_renderedMonthStyle = -1, g_renderedMonthDay = -1;
uint64_t g_renderedCultureRevision = ~0ULL;
int g_renderedCultureLanguage = -1;
int g_renderedCultureTheme = -1;
std::wstring g_cultureSelectedId;
bool g_activityObserved = false, g_previousPlaying = false, g_previousPlugged = false;
uint64_t g_previousMediaGeneration = 0;
std::vector<PrivacyUse> g_previousPrivacyUses;
ActivityKind g_lastUserActivity = ActivityKind::Lunar;
ULONGLONG g_lastManualSwitch = 0;

bool IsElementLightTheme(wux::FrameworkElement const& fe);
void ShowPopupHolidayList(bool show);
void ResetPopupScrollPosition(wuxc::ScrollViewer const& scroll);
void HideExpandedPopupImmediately(HWND hwnd);
void EnsureTaskbarWidgetInjectedOnThread();
void ProcessLiveActivityAlerts();

struct IWidgetBufferAccess : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Buffer(BYTE** value) = 0;
};
__CRT_UUID_DECL(IWidgetBufferAccess, 0x905a0fef, 0xbc53, 0x11df, 0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda);

wuxm::Imaging::WriteableBitmap BitmapFromPixels(std::shared_ptr<ImagePixels> const& pixels) {
    if (!pixels) return nullptr;
    wuxm::Imaging::WriteableBitmap bitmap(pixels->width, pixels->height);
    BYTE* destination = nullptr;
    winrt::check_hresult(bitmap.PixelBuffer().as<IWidgetBufferAccess>()->Buffer(&destination));
    if (!destination || bitmap.PixelBuffer().Capacity() < pixels->bytes.size() ||
        uint64_t(pixels->width) * pixels->height * 4 != pixels->bytes.size())
        throw winrt::hresult_invalid_argument();
    memcpy(destination, pixels->bytes.data(), pixels->bytes.size());
    bitmap.Invalidate();
    return bitmap;
}
void ApplyArtwork(wuxc::Border const& border, wuxc::TextBlock const& glyph) {
    if (!border || !glyph) return;
    auto pixels = g_uiLive.media.artwork;
    bool display = g_selectedActivity == ActivityKind::Media && !!pixels;
    if (!pixels) { g_mediaBitmap = nullptr; g_renderedArtwork.reset(); }
    glyph.Visibility(display ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    if (!display) return;
    if (g_renderedArtwork != pixels || !g_mediaBitmap) {
        g_mediaBitmap = BitmapFromPixels(pixels); g_renderedArtwork = pixels;
    }
    wuxm::ImageBrush brush; brush.ImageSource(g_mediaBitmap); brush.Stretch(wuxm::Stretch::UniformToFill);
    border.Background(brush);
}

wuxc::FontIcon FluentIcon(PCWSTR glyph, double size) {
    wuxc::FontIcon icon;
    icon.FontFamily(wuxm::FontFamily(L"Segoe MDL2 Assets"));
    icon.Glyph(glyph); icon.FontSize(size);
    return icon;
}
winrt::Windows::UI::Color SystemAccentColor() {
    try { return winrt::Windows::UI::ViewManagement::UISettings().GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Accent); }
    catch (...) { return {255, 0, 120, 215}; }
}
[[clang::no_destroy]] wuxc::ControlTemplate g_glassButtonTemplate{nullptr};
bool g_glassTemplateFailed=false;
std::wstring EffectivePopupTheme(ModSettings const& settings) {
    return settings.popupTheme==L"glass" && !g_glassCpuSupported ? L"blur" : settings.popupTheme;
}
bool UseGlassSurfaces() {return g_effectsAllowed.load() && EffectivePopupTheme(GetSettings())==L"glass";}
wuxm::LinearGradientBrush GlassEdgeBrush() {
    wuxm::LinearGradientBrush edge;edge.StartPoint({0,0});edge.EndPoint({1,1});
    for(auto [offset,color]:{std::pair{0.0,winrt::Windows::UI::Color{220,255,255,255}},
        std::pair{.32,winrt::Windows::UI::Color{75,190,225,255}},
        std::pair{.52,winrt::Windows::UI::Color{18,255,255,255}},
        std::pair{.78,winrt::Windows::UI::Color{92,255,215,238}},
        std::pair{1.0,winrt::Windows::UI::Color{160,255,255,255}}}){
        wuxm::GradientStop stop;stop.Offset(offset);stop.Color(color);edge.GradientStops().Append(stop);
    }
    return edge;
}
void ApplyGlassButtons(wux::DependencyObject const& root) {
    if(!root || g_shuttingDown)return;
    bool enabled=UseGlassSurfaces();
    try {
        if(enabled && !g_glassButtonTemplate && !g_glassTemplateFailed) {
            g_glassButtonTemplate=wux::Markup::XamlReader::Load(LR"GLASS(
<ControlTemplate xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" TargetType="Button">
 <Grid x:Name="Surface" Background="Transparent">
  <VisualStateManager.VisualStateGroups><VisualStateGroup x:Name="CommonStates">
   <VisualState x:Name="Normal"/>
   <VisualState x:Name="PointerOver"><VisualState.Setters><Setter Target="Highlight.Opacity" Value="0.3"/></VisualState.Setters></VisualState>
   <VisualState x:Name="Pressed"><VisualState.Setters><Setter Target="Highlight.Opacity" Value="0.5"/><Setter Target="Content.Opacity" Value="0.75"/></VisualState.Setters></VisualState>
   <VisualState x:Name="Disabled"><VisualState.Setters><Setter Target="Surface.Opacity" Value="0.4"/></VisualState.Setters></VisualState>
  </VisualStateGroup></VisualStateManager.VisualStateGroups>
  <Border CornerRadius="{TemplateBinding CornerRadius}">
   <Border.Background><AcrylicBrush BackgroundSource="Backdrop" TintColor="#B0B0B0" TintOpacity="0.06" FallbackColor="#50777777"/></Border.Background>
  </Border>
  <Border CornerRadius="{TemplateBinding CornerRadius}" Background="{TemplateBinding Background}" Opacity="0.28"/>
  <Border x:Name="Highlight" CornerRadius="{TemplateBinding CornerRadius}" Opacity="0.16">
   <Border.Background><LinearGradientBrush StartPoint="0,0" EndPoint="1,1"><GradientStop Color="#FFFFFFFF" Offset="0"/><GradientStop Color="#00FFFFFF" Offset="0.45"/><GradientStop Color="#88FFFFFF" Offset="1"/></LinearGradientBrush></Border.Background>
  </Border>
  <Border CornerRadius="{TemplateBinding CornerRadius}" BorderThickness="1.2">
   <Border.BorderBrush><LinearGradientBrush StartPoint="0,0" EndPoint="1,1"><GradientStop Color="#DEFFFFFF" Offset="0"/><GradientStop Color="#45BEDFFF" Offset="0.3"/><GradientStop Color="#12FFFFFF" Offset="0.5"/><GradientStop Color="#60FFDAEE" Offset="0.8"/><GradientStop Color="#B0FFFFFF" Offset="1"/></LinearGradientBrush></Border.BorderBrush>
  </Border>
  <Border CornerRadius="{TemplateBinding CornerRadius}" Margin="2" BorderThickness="0.5" BorderBrush="#20FFFFFF"/>
  <ContentPresenter x:Name="Content" Content="{TemplateBinding Content}" ContentTemplate="{TemplateBinding ContentTemplate}" Padding="{TemplateBinding Padding}" HorizontalContentAlignment="{TemplateBinding HorizontalContentAlignment}" VerticalContentAlignment="{TemplateBinding VerticalContentAlignment}" Foreground="{TemplateBinding Foreground}"/>
 </Grid>
</ControlTemplate>)GLASS").as<wuxc::ControlTemplate>();
        }
        // Only traverse this mod's visual subtree; native taskbar controls are untouched.
        std::vector<wux::DependencyObject> pending{root};
        for(size_t i=0;i<pending.size() && i<1024;++i){
            auto element=pending[i];
            if(auto button=element.try_as<wuxc::Button>()){
                if(enabled && g_glassButtonTemplate && button.Template()!=g_glassButtonTemplate) button.Template(g_glassButtonTemplate);
                else if(!enabled && g_glassButtonTemplate && button.Template()==g_glassButtonTemplate) button.ClearValue(wuxc::Control::TemplateProperty());
                continue;
            }
            int count=wuxm::VisualTreeHelper::GetChildrenCount(element);
            for(int j=0;j<count && pending.size()<1024;++j) pending.push_back(wuxm::VisualTreeHelper::GetChild(element,j));
        }
    } catch (...) {if(!g_glassButtonTemplate)g_glassTemplateFailed=true;}
}

void StyleAccentButton(wuxc::Control const& button, int state = 0) {
    auto color = SystemAccentColor();
    try {
        if (winrt::Windows::UI::ViewManagement::AccessibilitySettings().HighContrast()) {
            COLORREF background = GetSysColor(COLOR_HIGHLIGHT), foreground = GetSysColor(COLOR_HIGHLIGHTTEXT);
            button.Background(wuxm::SolidColorBrush({255, GetRValue(background), GetGValue(background), GetBValue(background)}));
            button.Foreground(wuxm::SolidColorBrush({255, GetRValue(foreground), GetGValue(foreground), GetBValue(foreground)})); return;
        }
    } catch (...) {}
    if (state) { auto adjust = [state](uint8_t value) { return static_cast<uint8_t>(state == 2 ? value * .82 : value + (255 - value) * .14); };
        color.R = adjust(color.R); color.G = adjust(color.G); color.B = adjust(color.B); }
    auto linear = [](uint8_t v) { double x = v / 255.0; return x <= .04045 ? x / 12.92 : std::pow((x + .055) / 1.055, 2.4); };
    double light = .2126 * linear(color.R) + .7152 * linear(color.G) + .0722 * linear(color.B);
    button.Background(wuxm::SolidColorBrush(color));
    button.Foreground(wuxm::SolidColorBrush(light > .179 ? winrt::Windows::UI::Color{255, 0, 0, 0} : winrt::Windows::UI::Color{255, 255, 255, 255}));
    if(UseGlassSurfaces()) button.Foreground(wuxm::SolidColorBrush(IsElementLightTheme(button) ?
        winrt::Windows::UI::Color{255,20,20,24} : winrt::Windows::UI::Color{255,245,245,248}));
}
wuxc::Button CreateMediaButton(PCWSTR glyph, MediaAction action, bool primary) {
    wuxc::Button button;
    auto icon = FluentIcon(glyph, primary ? 24 : 19);
    if (primary) g_playPauseIcon = icon;
    button.Content(icon);
    button.Width(primary ? 54 : 42); button.Height(primary ? 54 : 42);
    button.MinWidth(0); button.MinHeight(0); button.Padding({0, 0, 0, 0});
    button.CornerRadius(primary ? wux::CornerRadius{27, 27, 27, 27} : wux::CornerRadius{21, 21, 21, 21});
    button.BorderThickness({1, 1, 1, 1});
    button.VerticalAlignment(wux::VerticalAlignment::Center);
    auto weak = winrt::make_weak(button);
    auto paint = [weak, primary](int state) noexcept {
        if (g_shuttingDown) return;
        try { if (auto b = weak.get()) {
            bool light = IsElementLightTheme(b);
            auto color = primary ? SystemAccentColor()
                : light ? winrt::Windows::UI::Color{22, 0, 0, 0} : winrt::Windows::UI::Color{24, 255, 255, 255};
            if (!primary && state) color.A = state == 2 ? 65 : 45;
            b.Background(wuxm::SolidColorBrush(color));
            b.Foreground(wuxm::SolidColorBrush(primary || light ? winrt::Windows::UI::Color{255, 18, 27, 40}
                : winrt::Windows::UI::Color{255, 245, 245, 250}));
            if (primary) StyleAccentButton(b, state);
            b.BorderBrush(wuxm::SolidColorBrush(light ? winrt::Windows::UI::Color{25, 0, 0, 0}
                : winrt::Windows::UI::Color{32, 255, 255, 255}));
        } } catch (...) { Wh_Log(L"Media button appearance update failed"); }
    };
    button.PointerEntered([paint](auto const&, auto const&) { paint(1); });
    button.PointerExited([paint](auto const&, auto const&) { paint(0); });
    button.PointerPressed([paint](auto const&, auto const&) { paint(2); });
    button.PointerReleased([paint](auto const&, auto const&) { paint(1); });
    button.PointerCanceled([paint](auto const&, auto const&) { paint(0); });
    button.ActualThemeChanged([paint](auto const&, auto const&) { paint(0); });
    button.IsEnabledChanged([weak](auto const&, auto const&) {
        if (g_shuttingDown) return;
        try { if (auto b = weak.get()) b.Opacity(b.IsEnabled() ? 1.0 : 0.35); } catch (...) {}
    });
    button.Click([action](auto const&, auto const&) noexcept { try { QueueMediaAction(action); } catch (...) {} });
    paint(0);
    return button;
}

std::wstring PrivacyTitle(PrivacySnapshot const& state, bool vi) {
    std::wstring result;
    for (PCWSTR kind : {L"microphone", L"webcam", L"location"}) {
        bool found = std::any_of(state.uses.begin(), state.uses.end(), [&](auto const& use) { return use.capability == kind; });
        if (!found) continue;
        if (!result.empty()) result += L" + ";
        result += wcscmp(kind, L"microphone") == 0 ? PickText(vi, L"Micro", L"Microphone") :
            wcscmp(kind, L"webcam") == 0 ? L"Camera" : PickText(vi, L"Vị trí", L"Location");
    }
    return result.empty() ? PickText(vi, L"Không có hoạt động được báo", L"No reported activity") : result;
}
std::wstring PrivacyAppName(std::wstring const& app, bool vi) {
    return app == L"Windows Location Service" ? PickText(vi, L"Dịch vụ vị trí Windows (chưa có tên ứng dụng)",
        L"Windows Location Service (application not reported)") : FriendlyAppName(app);
}
std::wstring PrivacyApps(PrivacySnapshot const& state) {
    bool vi = UseVietnameseLabels(GetSettings());
    std::wstring result;
    std::set<std::wstring> names;
    for (auto const& use : state.uses) {
        auto name = PrivacyAppName(use.app, vi);
        if (!names.insert(LowerText(name)).second) continue;
        if (!result.empty()) result += L" · "; result += name;
    }
    return result;
}

void ObserveSystemActivityChanges(ModSettings const& settings);
void ObserveActivitySelection(ModSettings const& settings) {
    ObserveSystemActivityChanges(settings);
    bool manual = g_popupMode == PopupMode::Manual && g_expandedPopup && IsWindowVisible(g_expandedPopup);
    bool playing = settings.enableMediaWidget && g_uiLive.media.available && g_uiLive.media.playing;
    bool plugged = settings.enableBatteryWidget && g_uiLive.battery.valid && g_uiLive.battery.present && g_uiLive.battery.pluggedIn;
    bool newMedia = playing && (!g_previousPlaying || g_previousMediaGeneration != g_uiLive.media.generation);
    bool newPower = plugged && !g_previousPlugged;
    static uint64_t previousClock=0;
    bool newClock=settings.enableClockWidget && g_uiLive.clock.activitySerial!=previousClock;
    previousClock=g_uiLive.clock.activitySerial;
    static uint64_t previousObs = 0;
    bool newRecording = settings.enableObsWidget && g_uiLive.obs.recording && previousObs != g_uiLive.obs.generation;
    previousObs = g_uiLive.obs.recording ? g_uiLive.obs.generation : 0;
    bool newPrivacy = settings.enablePrivacyWidget && std::any_of(g_uiLive.privacy.uses.begin(), g_uiLive.privacy.uses.end(),
        [](auto const& use) { return !use.recent && std::none_of(g_previousPrivacyUses.begin(), g_previousPrivacyUses.end(), [&](auto const& old) {
            return old.capability == use.capability && !old.recent;
        }); });
    if (settings.autoSelectActivity && !manual && g_popupMode != PopupMode::Automatic && GetTickCount64() - g_lastManualSwitch > 1200) {
        if (newMedia) g_selectedActivity = ActivityKind::Media;
        if (newPower) g_selectedActivity = ActivityKind::Battery;
        if (newRecording) g_selectedActivity = ActivityKind::Obs;
        if (newClock) g_selectedActivity=ActivityKind::Clock;
        if (newPrivacy && g_activityObserved) g_selectedActivity = ActivityKind::Privacy;
    }
    g_previousPlaying = playing; g_previousPlugged = plugged;
    g_previousMediaGeneration = g_uiLive.media.generation;
    g_previousPrivacyUses = g_uiLive.privacy.uses;
    g_activityObserved = true;
}

wuxc::TextBlock WrappedText(std::wstring const& text, double size = 12) {
    wuxc::TextBlock block; block.Text(text); block.FontSize(size);
    block.TextWrapping(wux::TextWrapping::Wrap); return block;
}
void ChangeCalendarPage(int direction) {
    if (g_showingMonth) g_monthOffset = ClampVal(g_monthOffset + direction, -1200, 1200);
    else g_popupDayOffset = ClampVal(g_popupDayOffset + direction, -36525, 36525);
    ShowPopupHolidayList(false); RefreshExpandedPopupData();
}
void RefreshMonthCalendar(bool vi, bool light) {
    if (!g_monthScroll || !g_monthBody) return;
    bool visible = g_showingMonth && g_selectedActivity == ActivityKind::Lunar;
    g_monthScroll.Visibility(visible ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (!visible) return;
    if (g_popupHoroscopeScroll) g_popupHoroscopeScroll.Visibility(wux::Visibility::Collapsed);
    if (g_popupHolidayScroll) g_popupHolidayScroll.Visibility(wux::Visibility::Collapsed);
    SYSTEMTIME today{}; GetLocalTime(&today);
    int absoluteMonth = int(today.wYear) * 12 + today.wMonth - 1 + g_monthOffset;
    int year = absoluteMonth / 12, month = absoluteMonth % 12 + 1;
    g_popupTitleSolar.Text(std::wstring(PickText(vi, L"Tháng ", L"Month ")) + std::to_wstring(month) + L" / " + std::to_wstring(year));
    g_popupTitleLunar.Text(PickText(vi, L"Dương lịch · Âm lịch · Ngày lễ", L"Solar · Lunar · Holidays"));
    int style = (vi ? 1 : 0) | (light ? 2 : 0);
    if (g_renderedMonth == absoluteMonth && g_renderedMonthSelection == g_popupDayOffset && g_renderedMonthStyle == style && g_renderedMonthDay == today.wDay) return;
    g_renderedMonth = absoluteMonth; g_renderedMonthSelection = g_popupDayOffset; g_renderedMonthStyle = style; g_renderedMonthDay = today.wDay;
    g_monthBody.Children().Clear();
    wuxc::Grid grid;
    for (int i = 0; i < 7; ++i) {
        wuxc::ColumnDefinition col; col.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
        grid.ColumnDefinitions().Append(col);
    }
    for (int i = 0; i < 7; ++i) { wuxc::RowDefinition row; row.Height(wux::GridLengthHelper::Auto()); grid.RowDefinitions().Append(row); }
    PCWSTR labelsVi[] = {L"T2", L"T3", L"T4", L"T5", L"T6", L"T7", L"CN"};
    PCWSTR labelsEn[] = {L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat", L"Sun"};
    for (int i = 0; i < 7; ++i) {
        auto label = WrappedText(vi ? labelsVi[i] : labelsEn[i]); label.HorizontalAlignment(wux::HorizontalAlignment::Center);
        label.Margin({0, 4, 0, 8}); wuxc::Grid::SetColumn(label, i); grid.Children().Append(label);
    }
    SYSTEMTIME date{}; date.wYear = year; date.wMonth = month; date.wDay = 1; date.wHour = 12;
    FILETIME first{}; SystemTimeToFileTime(&date, &first); FileTimeToSystemTime(&first, &date);
    int firstColumn = (date.wDayOfWeek + 6) % 7;
    OffsetSystemDateByDays(date, -firstColumn);
    SYSTEMTIME normalizedToday = today; normalizedToday.wHour = 12;
    normalizedToday.wMinute = normalizedToday.wSecond = normalizedToday.wMilliseconds = 0;
    FILETIME todayFt{}; SystemTimeToFileTime(&normalizedToday, &todayFt);
    for (int i = 0; i < 42; ++i) {
        int lunarDay, lunarMonth, lunarYear; bool leap; std::wstring canChi;
        GetVietnameseLunarDate(date.wYear, date.wMonth, date.wDay, lunarDay, lunarMonth, lunarYear, leap, canChi);
        std::wstring holiday = GetVietnameseHolidayNames(date.wDay, date.wMonth, lunarDay, lunarMonth, leap, vi,
            IsVietnameseLunarYearEnd(date.wYear, date.wMonth, date.wDay, lunarDay, lunarMonth, leap));
        FILETIME dateFt{}; SystemTimeToFileTime(&date, &dateFt);
        int offset = static_cast<int>((static_cast<int64_t>(FileTimeValue(dateFt)) - static_cast<int64_t>(FileTimeValue(todayFt))) / 864000000000LL);
        wuxc::Button cell; cell.MinWidth(0); cell.MinHeight(0); cell.Height(66);
        cell.HorizontalAlignment(wux::HorizontalAlignment::Stretch); cell.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
        cell.Padding({3, 4, 3, 4}); cell.Margin({1, 1, 1, 1}); cell.CornerRadius({7, 7, 7, 7});
        cell.Opacity(date.wMonth == month ? 1.0 : 0.42);
        if (offset == g_popupDayOffset) StyleAccentButton(cell);
        else cell.Background(wuxm::SolidColorBrush({0, 0, 0, 0}));
        if (offset == 0) { cell.BorderThickness({1, 1, 1, 1}); cell.BorderBrush(wuxm::SolidColorBrush(SystemAccentColor())); }
        wuxc::StackPanel stack;
        auto solar = WrappedText(std::to_wstring(date.wDay), 15); solar.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        auto lunar = WrappedText(std::to_wstring(lunarDay) + L"/" + std::to_wstring(lunarMonth) + (leap ? L"*" : L""), 10);
        lunar.Opacity(0.75);
        auto event = WrappedText(holiday, 9); event.TextWrapping(wux::TextWrapping::NoWrap);
        event.TextTrimming(wux::TextTrimming::CharacterEllipsis); event.MaxLines(1);
        event.Foreground(wuxm::SolidColorBrush(light ? winrt::Windows::UI::Color{255, 180, 60, 30} : winrt::Windows::UI::Color{255, 255, 160, 130}));
        stack.Children().Append(solar); stack.Children().Append(lunar); stack.Children().Append(event); cell.Content(stack);
        std::wstring full = std::to_wstring(date.wDay) + L"/" + std::to_wstring(date.wMonth) + L"/" + std::to_wstring(date.wYear) +
            PickText(vi, L" · Âm lịch ", L" · Lunar ") + std::to_wstring(lunarDay) + L"/" + std::to_wstring(lunarMonth) + L"\n" + holiday;
        wuxc::ToolTipService::SetToolTip(cell, winrt::box_value(full));
        wux::Automation::AutomationProperties::SetName(cell, full);
        cell.Click([offset](auto const&, auto const&) {
            g_popupDayOffset = offset; g_showingMonth = false; RefreshExpandedPopupData();
            ResetPopupScrollPosition(g_popupHoroscopeScroll);
        });
        wuxc::Grid::SetRow(cell, 1 + i / 7); wuxc::Grid::SetColumn(cell, i % 7); grid.Children().Append(cell);
        OffsetSystemDateByDays(date, 1);
    }
    g_monthBody.Children().Append(grid);
    g_monthBody.Children().Append(WrappedText(PickText(vi, L"Chọn ngày để xem đầy đủ ngày lễ và Tử vi. *: tháng nhuận.",
        L"Select a day for its full holiday names and almanac. *: leap month."), 11));
}

[[clang::no_destroy]] wuxc::Slider g_mediaSeek{nullptr};
[[clang::no_destroy]] wuxc::Grid g_mediaTransport{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_mediaElapsed{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_mediaDuration{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_mediaSessionButtons{nullptr};
[[clang::no_destroy]] wuxc::ScrollViewer g_mediaSessionScroll{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_taskProgressList{nullptr};
std::vector<MediaSessionChoice> g_renderedMediaSessions;
uint64_t g_renderedMediaSelection = 0;
bool g_settingSeek = false, g_draggingSeek = false;
uint64_t g_seekGeneration = 0;
ULONGLONG g_seekQuietUntil = 0;
std::vector<TaskProgress> g_renderedTaskProgress;
int g_renderedTaskLanguage = -1;
struct PrivacyAlertTracker {
    bool initialized = false;
    uint64_t initializedAt = 0;
    std::map<std::wstring, uint64_t> lastSeen;
    bool Observe(std::vector<PrivacyUse> const& uses, uint64_t now) {
        bool alert = false;
        if (!initialized) initializedAt = now;
        for (auto const& use : uses) {
            if (use.recent) continue;
            auto old = lastSeen.find(use.capability);
            if (initialized && now - initializedAt >= 2000 && (old == lastSeen.end() || now - old->second > 15000)) alert = true;
            lastSeen[use.capability] = now;
        }
        initialized = true; return alert;
    }
};
PrivacyAlertTracker g_privacyAlerts;
std::vector<PrivacyUse> g_alertPrivacyUses;
std::vector<TaskProgress> g_alertTasks;
std::atomic<bool> g_culturePageOpen{false};

void ReleaseCulturePage() {
    g_culturePageOpen = false;
    { std::lock_guard<std::mutex> lock(g_cultureMutex);
      ++g_cultureRequestId; g_culture = {}; g_cultureRequest.clear(); }
    if (g_cultureResults) g_cultureResults.Children().Clear();
    if (g_cultureSearch) g_cultureSearch.Text(L"");
    g_cultureSelectedId.clear(); g_renderedCultureRevision = ~0ULL;
}
void QueueSeek() {
    if (!g_mediaSeek || !g_uiLive.media.canSeek || g_seekGeneration != g_uiLive.media.generation) return;
    auto requested = g_requestedMediaSession.load();
    if (requested && requested != g_uiLive.media.sessionId) return;
    PromotePopupToManual();
    { std::lock_guard<std::mutex> lock(g_liveMutex);
      // Coalesce pointer/key updates; only the most recent requested position matters.
      g_mediaCommand = {MediaAction::Seek, g_seekGeneration, static_cast<int64_t>(g_mediaSeek.Value())}; }
    g_seekQuietUntil = GetTickCount64() + 700;
    if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
}
void RefreshMediaSessionButtons(bool vi) {
    if (!g_mediaSessionButtons) return;
    auto const& sessions = g_uiLive.mediaSessions;
    g_mediaSessionScroll.Visibility(sessions.size() > 1 ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (sessions == g_renderedMediaSessions && g_renderedMediaSelection == g_uiLive.media.sessionId) return;
    g_renderedMediaSessions = sessions; g_renderedMediaSelection = g_uiLive.media.sessionId;
    g_mediaSessionButtons.Children().Clear();
    for (size_t i = 0; i < sessions.size(); ++i) {
        auto choice = sessions[i]; wuxc::Button button;
        button.Content(winrt::box_value(std::to_wstring(i + 1))); button.MinWidth(30); button.Height(28);
        button.Padding({8, 0, 8, 0}); button.CornerRadius({6, 6, 6, 6});
        if (choice.id == g_uiLive.media.sessionId) StyleAccentButton(button);
        std::wstring name = std::wstring(PickText(vi, L"Phiên ", L"Session ")) + std::to_wstring(i + 1);
        wuxc::ToolTipService::SetToolTip(button, winrt::box_value(name));
        wux::Automation::AutomationProperties::SetName(button, name);
        button.Click([id = choice.id](auto const&, auto const&) {
            PromotePopupToManual(); g_draggingSeek = false; g_requestedMediaSession = id; g_requestedMediaSerial = ++g_mediaActivitySerial;
            if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
        });
        g_mediaSessionButtons.Children().Append(button);
    }
}
// A native ProgressBar retains accessibility and indeterminate animation.
// Determinate decoration adds capsule/segment styles without polling or a layout timer.
uint64_t g_progressVisualRevision = 0;
wuxc::Grid CreateProgressView() {
    wuxc::Grid root;
    wuxc::ProgressBar native; native.Minimum(0); native.Maximum(100); native.MinHeight(0);
    root.Children().Append(native);
    wuxc::Grid decoration; decoration.IsHitTestVisible(false); root.Children().Append(decoration);
    return root;
}
void UpdateProgressView(wuxc::Grid const& root, int percent, unsigned state = 2) {
    if (!root) return;
    auto style = GetSettings().progressStyle;
    auto native = root.Children().GetAt(0).as<wuxc::ProgressBar>();
    auto decoration = root.Children().GetAt(1).as<wuxc::Grid>();
    auto key = style + L":" + std::to_wstring(percent) + L":" + std::to_wstring(state) +
        L":" + std::to_wstring(g_progressVisualRevision) + L":" + std::to_wstring(static_cast<int>(native.ActualTheme()));
    if (native.Tag() && winrt::unbox_value_or<winrt::hstring>(native.Tag(), L"") == key) return;
    native.Tag(winrt::box_value(key));
    bool unknown = percent < 0 || state == 1;
    HIGHCONTRASTW hc{}; hc.cbSize = sizeof(hc);
    bool highContrast = SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0) && (hc.dwFlags & HCF_HIGHCONTRASTON);
    if (unknown || highContrast) style = L"thin";
    native.IsIndeterminate(unknown); native.Value(std::clamp(percent, 0, 100));
    native.Opacity(style == L"thin" ? 1 : 0);
    native.Height(style == L"thin" ? 4 : 10); root.Height(style == L"thin" ? 4 : 10);
    if (state == 4 && !highContrast) native.Foreground(wuxm::SolidColorBrush({255, 235, 90, 90}));
    else if (state == 8 && !highContrast) native.Foreground(wuxm::SolidColorBrush({255, 240, 185, 70}));
    else native.ClearValue(wuxc::Control::ForegroundProperty());
    decoration.Visibility(style == L"thin" ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    if (style == L"thin") return;
    auto tag = root.Tag();
    if (!tag || winrt::unbox_value_or<winrt::hstring>(tag, L"") != style) {
        root.Tag(winrt::box_value(style)); decoration.Children().Clear(); decoration.ColumnDefinitions().Clear();
        int count = style == L"segments" ? 12 : 1;
        for (int i = 0; i < count; ++i) {
            wuxc::ColumnDefinition column; column.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
            decoration.ColumnDefinitions().Append(column);
            wuxc::Border track; track.Background(wuxm::SolidColorBrush({45, 128, 128, 128}));
            double radius = count == 1 ? 5 : 2;
            track.CornerRadius({radius, radius, radius, radius});
            track.Margin({0, 0, i + 1 == count ? 0.0 : 3.0, 0});
            wuxc::Grid fillGrid;
            fillGrid.ColumnDefinitions().Append(wuxc::ColumnDefinition());
            fillGrid.ColumnDefinitions().Append(wuxc::ColumnDefinition());
            wuxc::Border fill; fill.CornerRadius({radius, radius, radius, radius});
            fillGrid.Children().Append(fill); track.Child(fillGrid);
            wuxc::Grid::SetColumn(track, i); decoration.Children().Append(track);
        }
    }
    auto accent = winrt::Windows::UI::ViewManagement::UISettings().GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Accent);
    if (state == 4) accent = {255, 235, 90, 90}; else if (state == 8) accent = {255, 240, 185, 70};
    wuxm::SolidColorBrush brush(accent);
    unsigned count = decoration.Children().Size();
    for (unsigned i = 0; i < count; ++i) {
        auto grid = decoration.Children().GetAt(i).as<wuxc::Border>().Child().as<wuxc::Grid>();
        double fraction = std::clamp(double(percent) * count / 100.0 - i, 0.0, 1.0);
        grid.ColumnDefinitions().GetAt(0).Width(wux::GridLengthHelper::FromValueAndType(fraction, wux::GridUnitType::Star));
        grid.ColumnDefinitions().GetAt(1).Width(wux::GridLengthHelper::FromValueAndType(1 - fraction, wux::GridUnitType::Star));
        auto fill = grid.Children().GetAt(0).as<wuxc::Border>(); fill.Background(brush);
        fill.Visibility(fraction > 0 ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    }
}
std::wstring TaskProgressText(TaskProgress const& task, bool vi) {
    auto operation = task.control.operation == TaskOperation::Generic ? OperationFromTask(task) : task.control.operation;
    PCWSTR verb = PickText(vi, L"Đang chạy", L"In progress");
    switch (operation) {
        case TaskOperation::Download: verb = PickText(vi, L"Đang tải xuống", L"Downloading"); break;
        case TaskOperation::Files: verb = PickText(vi, L"Thao tác tệp", L"File operation"); break;
        case TaskOperation::Copy: verb = PickText(vi, L"Đang sao chép", L"Copying"); break;
        case TaskOperation::Move: verb = PickText(vi, L"Đang di chuyển", L"Moving"); break;
        case TaskOperation::Delete: verb = PickText(vi, L"Đang xóa", L"Deleting"); break;
        case TaskOperation::Archive: verb = PickText(vi, L"Đang xử lý tệp nén", L"Archive operation"); break;
        default: break;
    }
    if (task.state == 4) verb = PickText(vi, L"Cần xử lý", L"Needs attention");
    if (task.state == 8) verb = PickText(vi, L"Tạm dừng", L"Paused");
    return std::wstring(verb) + (task.percent >= 0 && task.state != 1 ? L" · " + std::to_wstring(task.percent) + L"%" : L"…");
}
std::wstring TaskDisplayName(TaskProgress const& task, bool vi) {
    auto op = task.control.operation == TaskOperation::Generic ? OperationFromTask(task) : task.control.operation;
    if (op == TaskOperation::Download) return PickText(vi, L"Tải xuống", L"Download");
    if (!task.control.filename.empty()) return task.control.filename;
    if (op == TaskOperation::Files || op == TaskOperation::Copy || op == TaskOperation::Move || op == TaskOperation::Delete)
        return PickText(vi, L"Thao tác tệp", L"File operation");
    return FriendlyAppName(task.app);
}
std::wstring DownloadSummary(std::vector<TaskProgress> const& tasks, bool vi) {
    size_t count = 0; bool aggregate = false;
    for (auto const& task : tasks) if (OperationFromTask(task) == TaskOperation::Download) {
        if (task.fileKey.empty()) aggregate = true; else ++count;
    }
    if (aggregate) return PickText(vi, L"Đang tải xuống", L"Downloading");
    return std::wstring(PickText(vi,L"Đang tải xuống ",L"Downloading ")) + std::to_wstring(count) +
        (vi ? L" file" : count == 1 ? L" file" : L" files");
}
std::wstring TaskNoticeText(TaskNotice const& notice, bool vi) {
    return notice.outcome == TaskOutcome::Completed ? PickText(vi, L"Đã hoàn thành", L"Completed") :
        notice.outcome == TaskOutcome::Cancelled ? PickText(vi, L"Đã hủy", L"Cancelled") : L"";
}
bool HasTaskNotice(LiveSnapshot const& state) { return state.taskNotice.serial && GetTickCount64() < state.taskNotice.expires; }
void RefreshTaskProgressPage(bool vi) {
    if (!g_taskProgressList) return;
    auto const& tasks = g_uiLive.tasks;
    bool sameRows = tasks.size() == g_renderedTaskProgress.size() &&
        g_taskProgressList.Children().Size() == tasks.size() && g_renderedTaskLanguage == int(vi);
    for (size_t i = 0; sameRows && i < tasks.size(); ++i)
        sameRows = g_renderedTaskProgress[i].id == tasks[i].id && g_renderedTaskProgress[i].window == tasks[i].window && g_renderedTaskProgress[i].fileKey == tasks[i].fileKey;
    if (!sameRows) {
        g_taskProgressList.Children().Clear();
        for (auto const& task : tasks) {
            wuxc::StackPanel row; row.Spacing(7); row.Margin({0, 2, 0, 8});
            auto heading = WrappedText(L"", 14); heading.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
            row.Children().Append(heading); row.Children().Append(WrappedText(L"", 12));
            row.Children().Append(CreateProgressView());
            wuxc::StackPanel buttons; buttons.Orientation(wuxc::Orientation::Horizontal); buttons.Spacing(6);
            wuxc::Button button;button.Padding({10,5,10,5});button.CornerRadius({6,6,6,6});
            button.Click([id=task.id,window=task.window](auto const&,auto const&) noexcept {
                if(g_shuttingDown)return;
                try {
                    PromotePopupToManual();auto state=ReadLiveSnapshot();
                    auto found=std::find_if(state.tasks.begin(),state.tasks.end(),[&](auto const& t){return t.id==id && t.window==window;});
                    if(found==state.tasks.end() || !TaskStillActive(*found))return;
                    HWND hwnd=reinterpret_cast<HWND>(found->control.sourceWindow?found->control.sourceWindow:window);
                    if(IsIconic(hwnd))ShowWindowAsync(hwnd,SW_RESTORE);SetForegroundWindow(hwnd);
                }catch(...){}
            });
            buttons.Children().Append(button);row.Children().Append(buttons);
            g_taskProgressList.Children().Append(row);
        }
    }
    unsigned downloadIndex=0;
    for (uint32_t i = 0; i < tasks.size(); ++i) {
        auto const& task = tasks[i]; auto row = g_taskProgressList.Children().GetAt(i).as<wuxc::StackPanel>();
        bool fileDownload = OperationFromTask(task) == TaskOperation::Download;
        if(fileDownload)++downloadIndex;
        auto name = fileDownload ? (task.fileKey.empty() ? std::wstring(PickText(vi,L"Tổng tiến độ",L"Overall progress")) :
            std::wstring(PickText(vi,L"File ",L"File ")) + std::to_wstring(downloadIndex)) : TaskDisplayName(task, vi);
        auto caption = name + L" · " + TaskProgressText(task, vi);
        row.Children().GetAt(0).as<wuxc::TextBlock>().Text(caption);
        auto title = row.Children().GetAt(1).as<wuxc::TextBlock>();
        bool download = OperationFromTask(task) == TaskOperation::Download;
        title.Text(download || OperationFromTask(task) == TaskOperation::Files ? L"" : task.title);
        title.Visibility(title.Text().empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        UpdateProgressView(row.Children().GetAt(2).as<wuxc::Grid>(), task.percent, task.state);
        auto buttons = row.Children().GetAt(3).as<wuxc::StackPanel>();
        auto open = buttons.Children().GetAt(0).as<wuxc::Button>();
        open.Content(winrt::box_value(PickText(vi,L"Mở ứng dụng",L"Open application")));
        wuxc::ToolTipService::SetToolTip(open,winrt::box_value(FriendlyAppName(task.app)));
    }
    g_renderedTaskProgress = tasks; g_renderedTaskLanguage = int(vi);
}

std::wstring FormatPlaybackTime(int64_t seconds);
[[clang::no_destroy]] wuxc::Grid g_tabsHost{nullptr};
[[clang::no_destroy]] wuxc::Button g_tabsPrevious{nullptr};
[[clang::no_destroy]] wuxc::Button g_tabsNext{nullptr};
[[clang::no_destroy]] wuxc::Grid g_popupHeader{nullptr};
[[clang::no_destroy]] wuxc::Border g_popupDivider{nullptr};
[[clang::no_destroy]] wuxc::Button g_cultureButton{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_privacySelector{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_privacyAppList{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_obsControls{nullptr};
[[clang::no_destroy]] wuxc::Button g_obsPause{nullptr};
[[clang::no_destroy]] wuxc::Button g_obsStop{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_batteryTools{nullptr};
[[clang::no_destroy]] wuxc::ComboBox g_powerAcCombo{nullptr};
[[clang::no_destroy]] wuxc::ComboBox g_powerDcCombo{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_powerModeStatus{nullptr};
[[clang::no_destroy]] wuxc::Button g_usageButton{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_usageList{nullptr};
[[clang::no_destroy]] wuxc::Border g_compactNotice{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_noticeText{nullptr};
std::wstring g_selectedPrivacy = L"microphone";
std::vector<PrivacyUse> g_renderedPrivacy;
std::wstring g_renderedPrivacySelection;
int g_renderedPrivacyLanguage = -1, g_powerControlLanguage = -1;
bool g_showUsage = false, g_updatingPowerModes = false;
uint64_t g_renderedUsageRevision = ~0ULL;
int g_renderedUsageLanguage = -1;

struct PlaybackTimeConverter : winrt::implements<PlaybackTimeConverter, wux::Data::IValueConverter> {
    wf::IInspectable Convert(wf::IInspectable const& value, wux::Interop::TypeName const&,
                            wf::IInspectable const&, winrt::hstring const&) {
        double seconds = winrt::unbox_value_or<double>(value, 0);
        if (!std::isfinite(seconds)) seconds = 0;
        return winrt::box_value(FormatPlaybackTime(static_cast<int64_t>(std::clamp(seconds, 0.0, 922337203685.0))));
    }
    wf::IInspectable ConvertBack(wf::IInspectable const&, wux::Interop::TypeName const&,
                                wf::IInspectable const&, winrt::hstring const&) { throw winrt::hresult_not_implemented(); }
};
PCWSTR CapabilityIcon(std::wstring const& kind) {
    return kind == L"webcam" ? L"\uE722" : kind == L"location" ? L"\uE81D" : L"\uE720";
}
PCWSTR CapabilityName(std::wstring const& kind, bool vi) {
    return kind == L"webcam" ? L"Camera" : kind == L"location" ? PickText(vi, L"Vị trí", L"Location") : PickText(vi, L"Micro", L"Microphone");
}
void NormalizePrivacySelection() {
    auto const& uses = g_uiLive.privacy.uses;
    if (!uses.empty() && std::none_of(uses.begin(), uses.end(), [](auto const& use) { return use.capability == g_selectedPrivacy; }))
        g_selectedPrivacy = uses.front().capability;
}
void PrivacyGlyph(wuxc::TextBlock const& text) {
    if (!text) return;
    NormalizePrivacySelection();
    std::wstring icons;
    for (PCWSTR kind : {L"microphone", L"webcam", L"location"})
        if (std::any_of(g_uiLive.privacy.uses.begin(), g_uiLive.privacy.uses.end(), [&](auto const& use) { return use.capability == kind; }))
            icons += CapabilityIcon(kind);
    text.FontFamily(wuxm::FontFamily(L"Segoe MDL2 Assets"));
    text.FontSize(icons.size() > 1 ? 12 : 20);
    text.Text(icons.empty() ? L"\uE72E" : icons);
    text.Visibility(wux::Visibility::Visible);
}
void RefreshPrivacyPage(bool vi) {
    NormalizePrivacySelection();
    g_popupLiveTitle.Text(L"");
    g_privacySelector.Visibility(g_uiLive.privacy.uses.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    g_popupLiveDetail.Text(L"");
    g_popupLiveStatus.Text(g_uiLive.privacy.uses.empty() ? PickText(vi, L"Không có hoạt động được báo.", L"No reported activity.") : L"");
    if (g_renderedPrivacy == g_uiLive.privacy.uses && g_renderedPrivacySelection == g_selectedPrivacy && g_renderedPrivacyLanguage == int(vi)) return;
    g_renderedPrivacy = g_uiLive.privacy.uses; g_renderedPrivacySelection = g_selectedPrivacy; g_renderedPrivacyLanguage = int(vi);
    g_privacySelector.Children().Clear(); g_privacyAppList.Children().Clear();
    int kinds = 0;
    for (std::wstring kind : {L"microphone", L"webcam", L"location"}) {
        if (std::none_of(g_renderedPrivacy.begin(), g_renderedPrivacy.end(), [&](auto const& use) { return use.capability == kind; })) continue;
        ++kinds;
        wuxc::Button button; wuxc::StackPanel content; content.Orientation(wuxc::Orientation::Horizontal); content.Spacing(6);
        content.Children().Append(FluentIcon(CapabilityIcon(kind), 14));
        content.Children().Append(WrappedText(CapabilityName(kind, vi), 12));
        button.Content(content); button.Padding({8, 5, 8, 5}); button.CornerRadius({6, 6, 6, 6});
        if (kind == g_selectedPrivacy) StyleAccentButton(button);
        button.Click([kind](auto const&, auto const&) { PromotePopupToManual(); g_selectedPrivacy = kind; RequestPopupRefresh(); });
        g_privacySelector.Children().Append(button);
    }
    g_privacySelector.Visibility(kinds > 0 ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    std::map<std::wstring, std::pair<std::wstring, bool>> applications;
    for (auto const& use : g_renderedPrivacy) if (use.capability == g_selectedPrivacy) {
        auto name = PrivacyAppName(use.app, vi);
        auto [item, inserted] = applications.try_emplace(LowerText(name), name, use.recent);
        if (!inserted) item->second.second = item->second.second && use.recent;
    }
    for (auto const& [key, entry] : applications) {
        auto const& [name, recent] = entry;
        wuxc::Border row; row.Padding({10, 8, 10, 8}); row.CornerRadius({6, 6, 6, 6});
        row.Background(wuxm::SolidColorBrush({18, 128, 128, 128}));
        row.Child(WrappedText(name + (recent ? PickText(vi, L" · Vừa truy cập", L" · Recently accessed") :
            PickText(vi, L" · Đang sử dụng", L" · In use")), 13));
        g_privacyAppList.Children().Append(row);
    }
}
void UpdateTabArrows() {
    if (!g_popupTabsScroll || !g_tabsPrevious) return;
    double offset = g_popupTabsScroll.HorizontalOffset(), extent = g_popupTabsScroll.ScrollableWidth();
    bool overflow = g_popupTabsScroll.Visibility() == wux::Visibility::Visible && extent > 1;
    g_tabsPrevious.Visibility(overflow ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_tabsNext.Visibility(overflow ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_tabsPrevious.IsEnabled(offset > 1); g_tabsNext.IsEnabled(offset < extent - 1);
}
void ScrollActivityTabs(int direction) {
    if (!g_popupTabsScroll) return;
    PromotePopupToManual();
    double offset = std::clamp(g_popupTabsScroll.HorizontalOffset() + direction * std::max(80.0, g_popupTabsScroll.ViewportWidth() * 0.7),
        0.0, g_popupTabsScroll.ScrollableWidth());
    g_popupTabsScroll.ChangeView(winrt::box_value(offset).as<wf::IReference<double>>(), nullptr, nullptr, false);
}
void RefreshBluetoothList(bool vi) {
    if (!g_bluetoothList) return;
    auto const& devices = g_uiLive.bluetooth.devices;
    g_bluetoothList.Visibility(!g_showUsage && GetSettings().bluetoothBattery ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (devices == g_renderedBluetooth && g_renderedBluetoothLanguage == int(vi)) return;
    g_renderedBluetooth = devices; g_renderedBluetoothLanguage = int(vi);
    g_bluetoothList.Children().Clear();
    auto heading = WrappedText(PickText(vi, L"Pin Bluetooth", L"Bluetooth batteries"), 15);
    heading.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold()); g_bluetoothList.Children().Append(heading);
    if (devices.empty()) {
        g_bluetoothList.Children().Append(WrappedText(PickText(vi, L"Chưa có thiết bị Bluetooth đang kết nối được báo.", L"No connected Bluetooth devices reported."), 12));
        return;
    }
    for (auto const& device : devices) {
        wuxc::Border card; card.Padding({10, 8, 10, 8}); card.CornerRadius({7, 7, 7, 7});
        card.Background(wuxm::SolidColorBrush({18, 128, 128, 128}));
        wuxc::StackPanel row; row.Spacing(6);
        auto text = device.name + L" · " + (device.percent >= 0 ? std::to_wstring(device.percent) + L"%" :
            PickText(vi, L"Chưa có mức pin", L"Battery unavailable"));
        row.Children().Append(WrappedText(text, 13));
        if (device.percent >= 0) {
            auto bar = CreateProgressView(); UpdateProgressView(bar, device.percent, device.percent <= 10 ? 4 : 2);
            row.Children().Append(bar);
        }
        card.Child(row); g_bluetoothList.Children().Append(card);
    }
}
void RefreshBatteryTools(bool vi) {
    RefreshBluetoothList(vi);
    PowerModeSnapshot modes; UsageSnapshot usage;
    { std::lock_guard<std::mutex> lock(g_liveMutex); modes = g_powerModes; if (g_showUsage) usage = g_usageSnapshot; }
    g_updatingPowerModes = true;
    if (g_powerControlLanguage != int(vi)) {
        g_powerControlLanguage = int(vi);
        for (auto combo : {g_powerAcCombo, g_powerDcCombo}) {
            combo.Items().Clear();
            for (PCWSTR label : {PickText(vi, L"Hiệu suất nguồn tốt nhất", L"Best power efficiency"),
                PickText(vi, L"Cân bằng", L"Balanced"), PickText(vi, L"Hiệu suất tốt nhất", L"Best performance")})
                combo.Items().Append(winrt::box_value(label));
        }
        g_powerAcCombo.Header(winrt::box_value(PickText(vi, L"Đã cắm điện", L"Plugged in")));
        g_powerDcCombo.Header(winrt::box_value(PickText(vi, L"Đang dùng pin", L"On battery")));
    }
    g_powerAcCombo.SelectedIndex(modes.ac); g_powerAcCombo.IsEnabled(modes.ac >= 0);
    g_powerDcCombo.SelectedIndex(modes.dc); g_powerDcCombo.IsEnabled(modes.dc >= 0);
    g_updatingPowerModes = false;
    std::wstring status = modes.ac < 0 || modes.dc < 0 ? PickText(vi,
        L"Chế độ nguồn không khả dụng trên cấu hình Windows này.", L"Power mode is unavailable on this Windows configuration.") : L"";
    if (modes.error) status = std::wstring(PickText(vi, L"Windows chưa áp dụng thay đổi. Mã lỗi: ", L"Windows did not apply the change. Error: ")) + std::to_wstring(modes.error);
    g_powerModeStatus.Text(status);
    g_powerModeStatus.Visibility(status.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    for (auto control : {g_powerAcCombo, g_powerDcCombo}) control.Visibility(g_showUsage ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    if (g_showUsage) g_powerModeStatus.Visibility(wux::Visibility::Collapsed);
    g_usageButton.Content(winrt::box_value(g_showUsage ? PickText(vi, L"← Quay lại", L"← Back") : PickText(vi, L"Thời gian sử dụng ứng dụng", L"App usage time")));
    g_usageList.Visibility(g_showUsage ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (!g_showUsage) return;
    if (usage.revision == g_renderedUsageRevision && g_renderedUsageLanguage == int(vi)) return;
    g_renderedUsageRevision = usage.revision; g_renderedUsageLanguage = int(vi);
    g_usageList.Children().Clear();
    auto scope = PickText(vi, usage.onBattery ? L"Phiên dùng pin" : L"Phiên máy hiện tại", usage.onBattery ? L"Battery session" : L"Current PC session");
    uint64_t elapsed = GetTickCount64() >= usage.measuredSince ? GetTickCount64() - usage.measuredSince : 0;
    std::wstring note = std::wstring(scope) + PickText(vi, L" · Đã theo dõi ", L" · Observed for ") + FormatPlaybackTime(elapsed / 1000);
    auto summary = WrappedText(note, 12);
    wuxc::ToolTipService::SetToolTip(summary, winrt::box_value(PickText(vi,
        L"Chỉ tính thời gian tương tác từ lúc bật mod; bỏ qua ngủ, khóa máy và không thao tác quá 1 phút.",
        L"Measured since enabling the mod. Sleep, lock and idle over one minute are excluded.")));
    g_usageList.Children().Append(summary);
    if (usage.apps.empty()) g_usageList.Children().Append(WrappedText(PickText(vi, L"Chưa có dữ liệu sử dụng.", L"No usage data yet."), 12));
    for (auto const& item : usage.apps) {
        wuxc::Border row; row.Padding({12, 10, 12, 10}); row.CornerRadius({8, 8, 8, 8});
        row.Background(wuxm::SolidColorBrush({18, 128, 128, 128}));
        wuxc::Grid grid;
        wuxc::ColumnDefinition nameColumn, timeColumn;
        nameColumn.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
        timeColumn.Width(wux::GridLengthHelper::Auto());
        grid.ColumnDefinitions().Append(nameColumn); grid.ColumnDefinitions().Append(timeColumn);
        auto name = WrappedText(FriendlyAppName(item.app), 13);
        auto time = WrappedText(FormatPlaybackTime(item.milliseconds / 1000), 13);
        time.Margin({12, 0, 0, 0}); time.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        wuxc::Grid::SetColumn(time, 1); grid.Children().Append(name); grid.Children().Append(time);
        row.Child(grid); g_usageList.Children().Append(row);
    }
}
void ReleaseAdaptiveControls() {
    if (g_quickMenu) { try { g_quickMenu.Hide(); } catch (...) {} }
    g_quickMenu = nullptr; g_quickMenuOpen = false;
    g_quickSettings = nullptr; g_collapseTaskbar = nullptr; g_hideActivity = nullptr; g_bluetoothList = nullptr;
    g_renderedBluetooth.clear(); g_renderedBluetoothLanguage = -1;
    g_tabsHost = nullptr; g_tabsPrevious = nullptr; g_tabsNext = nullptr; g_popupHeader = nullptr; g_popupDivider = nullptr;
    g_cultureButton = nullptr; g_privacySelector = nullptr; g_privacyAppList = nullptr; g_batteryTools = nullptr;
    g_powerAcCombo = nullptr; g_powerDcCombo = nullptr; g_powerModeStatus = nullptr; g_usageButton = nullptr; g_usageList = nullptr;
    g_compactNotice = nullptr; g_noticeText = nullptr; g_renderedPrivacy.clear(); g_renderedPrivacySelection.clear();
    g_renderedPrivacyLanguage = -1; g_powerControlLanguage = -1; g_showUsage = false; g_batteryPageOpen = false;
    g_renderedUsageRevision = ~0ULL; g_renderedUsageLanguage = -1;
}

[[clang::no_destroy]] wuxc::StackPanel g_systemDetails{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_systemOptions{nullptr};
[[clang::no_destroy]] wuxc::Button g_systemSettingsButton{nullptr};
[[clang::no_destroy]] wuxc::Button g_projectionButton{nullptr};
[[clang::no_destroy]] wuxc::Button g_hotspotStopButton{nullptr};
DisplaySnapshot g_renderedDisplay;
HotspotSnapshot g_renderedHotspot;
int g_renderedSystemLanguage = -1;
ActivityKind g_renderedSystemActivity = ActivityKind::None;

PCWSTR DisplayConnectionName(unsigned technology, bool vi) {
    switch (technology) {
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI: return L"HDMI";
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL: return L"DisplayPort";
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DVI: return L"DVI";
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HD15: return L"VGA";
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_MIRACAST: return L"Miracast";
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INDIRECT_WIRED: return PickText(vi, L"Màn hình có dây", L"Wired display");
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INDIRECT_VIRTUAL: return PickText(vi, L"Màn hình ảo", L"Virtual display");
        default: return PickText(vi, L"Màn hình ngoài", L"External display");
    }
}
std::wstring DisplayModeText(DisplaySnapshot const& display, bool vi) {
    if (!display.available) return PickText(vi, L"Chưa đọc được trạng thái màn hình", L"Display status unavailable");
    if (display.targets.empty()) return PickText(vi, L"Không có màn hình ngoài đang hoạt động", L"No active external display");
    if (std::any_of(display.targets.begin(), display.targets.end(), [](auto const& target) { return target.duplicate; }))
        return PickText(vi, L"Đang nhân đôi màn hình", L"Duplicating displays");
    if (display.sourceCount > 1) return PickText(vi, L"Đang mở rộng màn hình", L"Extending displays");
    return PickText(vi, L"Màn hình ngoài đang hoạt động", L"External display active");
}
std::wstring DisplayNames(DisplaySnapshot const& display, bool vi) {
    std::wstring text;
    for (auto const& target : display.targets) {
        if (!text.empty()) text += L" · ";
        text += target.name.empty() ? DisplayConnectionName(target.technology, vi) : target.name;
        if (text.size() > 256) { text.resize(256); text += L"…"; break; }
    }
    return text;
}
std::wstring HotspotStateText(HotspotSnapshot const& hotspot, bool vi) {
    if (!hotspot.available) return PickText(vi, L"Chưa đọc được trạng thái", L"Status unavailable");
    if (hotspot.changing) return PickText(vi, L"Đang chuyển trạng thái…", L"Changing state…");
    if (!hotspot.on) return PickText(vi, L"Đã tắt điểm phát sóng", L"Hotspot off");
    if (hotspot.clients < 0) return PickText(vi, L"Điểm phát sóng đang bật", L"Hotspot on");
    return std::to_wstring(hotspot.clients) + PickText(vi, L" thiết bị kết nối", hotspot.clients == 1 ? L" connected device" : L" connected devices");
}
void SetActivityGlyph(wuxc::TextBlock const& text, PCWSTR glyph) {
    text.FontFamily(wuxm::FontFamily(L"Segoe MDL2 Assets")); text.FontSize(22); text.Text(glyph);
}
void RefreshSystemPage(bool vi) {
    bool displayPage = g_selectedActivity == ActivityKind::Display;
    auto const& display = g_uiLive.display; auto const& hotspot = g_uiLive.hotspot;
    g_popupLiveProgress.Visibility(wux::Visibility::Collapsed);
    g_popupLiveStatus.Text(L"");
    g_popupLiveTitle.FontSize(20);
    if (displayPage) {
        SetActivityGlyph(g_popupZodiacText, L"\uE7F4");
        g_popupTitleLunar.Text(display.targets.empty() ? L"" :
            std::to_wstring(display.targets.size()) + PickText(vi, L" màn hình ngoài", L" external displays"));
        g_popupLiveTitle.Text(DisplayModeText(display, vi)); g_popupLiveDetail.Text(L"");
    } else {
        SetActivityGlyph(g_popupZodiacText, L"\uEC15");
        g_popupTitleLunar.Text(hotspot.on ? PickText(vi, L"Đã bật", L"On") : L"");
        g_popupLiveTitle.Text(HotspotStateText(hotspot, vi));
        g_popupLiveDetail.Text(hotspot.on ? hotspot.ssid : L"");
    }
    g_systemSettingsButton.Content(winrt::box_value(displayPage ? PickText(vi, L"Cài đặt màn hình", L"Display settings") :
        PickText(vi, L"Cài đặt điểm phát sóng", L"Hotspot settings")));
    g_projectionButton.Content(winrt::box_value(PickText(vi, L"Tùy chọn chiếu", L"Projection options")));
    g_projectionButton.Visibility(displayPage ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (g_hotspotStopButton) {
        g_hotspotStopButton.Visibility(!displayPage && hotspot.on ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        g_hotspotStopButton.Content(winrt::box_value(PickText(vi,
            hotspot.commandPending ? L"Đang tắt…" : L"Tắt điểm phát sóng",
            hotspot.commandPending ? L"Turning off…" : L"Turn off hotspot")));
        g_hotspotStopButton.IsEnabled(hotspot.available && !hotspot.changing && !hotspot.commandPending);
    }
    if (!displayPage && hotspot.commandFailed)
        g_popupLiveStatus.Text(PickText(vi, L"Không tắt được điểm phát sóng. Mở cài đặt Windows để kiểm tra.",
            L"Could not turn off hotspot. Check Windows settings."));
    if (g_renderedSystemLanguage == int(vi) && g_renderedSystemActivity == g_selectedActivity &&
        g_renderedDisplay == display && g_renderedHotspot == hotspot) return;
    g_renderedSystemLanguage = int(vi); g_renderedSystemActivity = g_selectedActivity;
    g_renderedDisplay = display; g_renderedHotspot = hotspot;
    g_systemDetails.Children().Clear();
    if (displayPage) for (auto const& target : display.targets) {
        wuxc::Border card; card.Padding({10, 8, 10, 8}); card.CornerRadius({7, 7, 7, 7});
        card.Background(wuxm::SolidColorBrush({18, 128, 128, 128}));
        wuxc::StackPanel row; row.Spacing(3);
        if (!target.name.empty()) row.Children().Append(WrappedText(target.name, 14));
        row.Children().Append(WrappedText(DisplayConnectionName(target.technology, vi), 12));
        card.Child(row); g_systemDetails.Children().Append(card);
    }
    if (!displayPage && hotspot.on) {
        if (!hotspot.clientsAvailable) g_systemDetails.Children().Append(WrappedText(
            PickText(vi, L"Windows chưa cung cấp danh sách thiết bị.", L"Windows did not provide the device list."), 12));
        for (auto const& device : hotspot.devices) {
            wuxc::Border card; card.Padding({10, 8, 10, 8}); card.CornerRadius({7, 7, 7, 7});
            card.Background(wuxm::SolidColorBrush({18, 128, 128, 128}));
            wuxc::StackPanel row; row.Spacing(3);
            row.Children().Append(WrappedText(device.name.empty() ? device.mac : device.name, 14));
            auto detail = device.address;
            if (!device.name.empty() && !device.mac.empty()) detail += (detail.empty() ? L"" : L" · ") + device.mac;
            if (!detail.empty()) row.Children().Append(WrappedText(detail, 12));
            card.Child(row); g_systemDetails.Children().Append(card);
        }
    }
}
void OpenSystemActivitySettings(bool projectionOptions) noexcept {
    if (g_shuttingDown) return;
    try {
        PromotePopupToManual();
        if (projectionOptions) {
            WCHAR directory[MAX_PATH]{};
            UINT length = GetSystemDirectoryW(directory, ARRAYSIZE(directory));
            if (length && length < ARRAYSIZE(directory)) {
                auto path = std::wstring(directory) + L"\\DisplaySwitch.exe";
                ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            }
        } else ShellExecuteW(nullptr, L"open", g_selectedActivity == ActivityKind::Display ?
            L"ms-settings:display" : L"ms-settings:network-mobilehotspot", nullptr, nullptr, SW_SHOWNORMAL);
    } catch (...) {}
}
struct SystemActivityAlerts {
    bool displayReady = false, hotspotReady = false;
    DisplaySnapshot display;
    HotspotSnapshot hotspot;
    ActivityKind Observe(LiveSnapshot const& state, ModSettings const& settings) {
        ActivityKind activity = ActivityKind::None;
        if (!settings.enableDisplayWidget) displayReady = false;
        else if (state.display.available) {
            bool changed = display.targets.size() != state.display.targets.size() || display.sourceCount != state.display.sourceCount;
            for (size_t i = 0; !changed && i < display.targets.size(); ++i)
                changed = display.targets[i].id != state.display.targets[i].id || display.targets[i].duplicate != state.display.targets[i].duplicate;
            if (displayReady && changed && (!display.targets.empty() || !state.display.targets.empty())) activity = ActivityKind::Display;
            display = state.display; displayReady = true;
        }
        if (!settings.enableHotspotWidget) hotspotReady = false;
        else if (state.hotspot.available && !state.hotspot.changing) {
            if (hotspotReady && (hotspot.on != state.hotspot.on ||
                (state.hotspot.on && hotspot.clients >= 0 && state.hotspot.clients >= 0 && hotspot.clients != state.hotspot.clients))) activity = ActivityKind::Hotspot;
            hotspot = state.hotspot; hotspotReady = true;
        }
        return activity;
    }
};
SystemActivityAlerts g_systemAlerts;
ActivityKind g_pendingSystemAlert = ActivityKind::None, g_systemNoticeActivity = ActivityKind::None;
uint64_t g_systemNoticeUntil = 0;
void ObserveSystemActivityChanges(ModSettings const& settings) {
    auto activity = g_systemAlerts.Observe(g_uiLive, settings);
    if (activity == ActivityKind::None) return;
    g_pendingSystemAlert = activity;
    bool preview = activity == ActivityKind::Display ? settings.displayAutoPopup : settings.hotspotAutoPopup;
    if (preview && g_popupMode != PopupMode::Manual) {
        // Keep the slot long enough to show a disconnect/off preview even when
        // the last active provider has just ended. The UI-thread deadline is bounded.
        g_systemNoticeActivity = activity; g_systemNoticeUntil = GetTickCount64() + 5500;
    }
}

void ReleaseClockControls();
void ReleaseNewControls() {
    ReleaseClockControls();
    g_systemDetails = nullptr; g_systemOptions = nullptr; g_systemSettingsButton = nullptr; g_projectionButton = nullptr; g_hotspotStopButton = nullptr;
    g_renderedSystemLanguage = -1; g_renderedSystemActivity = ActivityKind::None;
    g_renderedDisplay = {}; g_renderedHotspot = {};
    g_obsControls = nullptr; g_obsPause = nullptr; g_obsStop = nullptr;
    ReleaseAdaptiveControls();
    g_mediaSeek = nullptr; g_mediaTransport = nullptr; g_mediaElapsed = nullptr; g_mediaDuration = nullptr;
    g_mediaSessionButtons = nullptr; g_mediaSessionScroll = nullptr; g_taskProgressList = nullptr;
    g_renderedMediaSessions.clear(); g_renderedMediaSelection = 0; g_renderedTaskProgress.clear();
    g_draggingSeek = false; g_renderedTaskLanguage = -1;
}

void RefreshCultureResults() {
    if (!g_cultureResults || !g_culturePageOpen) return;
    auto state = ReadCultureState(); bool vi = UseVietnameseLabels(GetSettings());
    g_cultureResults.Children().Clear();
    auto add = [&](std::wstring const& value, double size = 12) {
        auto block = WrappedText(value, size); block.Margin({0, 4, 0, 8}); g_cultureResults.Children().Append(block);
    };
    auto makeButton = [](PCWSTR title, auto handler) { wuxc::Button button; button.Content(winrt::box_value(title));
        button.CornerRadius({7, 7, 7, 7}); button.Click(handler); return button; };
    if (!g_cultureSelectedId.empty()) {
        g_cultureResults.Children().Append(makeButton(PickText(vi, L"‹ Danh mục", L"‹ Topics"), [](auto const&, auto const&) {
            g_cultureSelectedId.clear(); RequestCulture(L""); g_renderedCultureRevision = ~0ULL; RefreshExpandedPopupData();
        }));
    }
    if (state.loading || state.catalogLoading) { add(PickText(vi, L"Đang tải…", L"Loading…")); return; }
    if (!state.error.empty()) {
        add(PickText(vi, L"Không tải được nội dung. Kiểm tra kết nối và thử lại.", L"Unable to load content. Check your connection and retry."));
        g_cultureResults.Children().Append(makeButton(PickText(vi, L"Thử lại", L"Retry"), [](auto const&, auto const&) { RequestCulture(g_cultureSelectedId); }));
        return;
    }
    if (g_cultureSelectedId.empty()) {
        std::wstring query = g_cultureSearch ? std::wstring(g_cultureSearch.Text()) : L"";
        std::transform(query.begin(), query.end(), query.begin(), towlower);
        bool any = false;
        for (auto const& topic : state.topics) {
            std::wstring search = topic.title + L" " + topic.category;
            std::transform(search.begin(), search.end(), search.begin(), towlower);
            if (!query.empty() && search.find(query) == std::wstring::npos) continue;
            wuxc::Button button; button.MinWidth(0); button.CornerRadius({7, 7, 7, 7});
            button.HorizontalAlignment(wux::HorizontalAlignment::Stretch); button.HorizontalContentAlignment(wux::HorizontalAlignment::Left);
            button.Content(WrappedText(topic.title + (topic.category.empty() ? L"" : L" · " + topic.category), 13));
            button.Click([id = topic.id](auto const&, auto const&) { g_cultureSelectedId = id; RequestCulture(id); });
            g_cultureResults.Children().Append(button); any = true;
        }
        if (!any) add(PickText(vi, L"Không có chủ đề phù hợp.", L"No matching topics."));
        add(GetSettings().cultureCatalogUrl.empty()
            ? PickText(vi, L"Bài viết trực tuyến từ Wikipedia tiếng Việt. Nội dung cộng đồng biên soạn; xem nguồn và lịch sử sửa đổi tại bài gốc.",
                L"Online articles from English Wikipedia. Community-written content; consult the source and revision history.")
            : PickText(vi, L"Nội dung từ danh mục trực tuyến đã cấu hình.", L"Content from your configured online catalog."), 11);
    } else {
        auto const& article = state.article;
        add(article.title, 19);
        if (article.image) {
            wuxc::Image image; image.Source(BitmapFromPixels(article.image)); image.MaxHeight(160);
            image.Stretch(wuxm::Stretch::Uniform); image.HorizontalAlignment(wux::HorizontalAlignment::Left);
            g_cultureResults.Children().Append(image);
        }
        add(article.text, 13);
        auto link = [&](std::wstring const& url, PCWSTR title) {
            if (url.rfind(L"https://", 0) != 0) return;
            wuxc::HyperlinkButton button; button.Content(winrt::box_value(title)); button.NavigateUri(wf::Uri(url));
            g_cultureResults.Children().Append(button);
        };
        link(article.source, PickText(vi, L"Nguồn · Bài đầy đủ và lịch sử sửa đổi", L"Source · Full article and revision history"));
        if (article.image) link(article.imageSource, PickText(vi, L"Nguồn ảnh và giấy phép", L"Image source and licensing"));
        if (GetSettings().cultureCatalogUrl.empty()) {
            add(PickText(vi, L"Văn bản: Wikipedia, CC BY-SA 4.0; bản trích có thể được rút ngắn. Ảnh có giấy phép riêng tại nguồn.",
                L"Text: Wikipedia, CC BY-SA 4.0; excerpt may be shortened. Images have separate licenses at the source."), 10);
            link(L"https://creativecommons.org/licenses/by-sa/4.0/", L"CC BY-SA 4.0");
        }
    }
}
void RefreshCulturePage(bool vi) {
    if (!g_cultureScroll) return;
    bool show = g_selectedActivity == ActivityKind::Culture && g_popupMode == PopupMode::Manual;
    int theme = g_popupRootCard ? static_cast<int>(g_popupRootCard.RequestedTheme()) : 0;
    if (g_renderedCultureTheme != theme) { g_renderedCultureTheme = theme; g_renderedCultureRevision = ~0ULL; }
    g_cultureScroll.Visibility(show ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (!show || g_popupMode == PopupMode::Hidden) {
        if (g_culturePageOpen) ReleaseCulturePage();
        return;
    }
    g_culturePageOpen = true;
    g_popupTitleSolar.Text(PickText(vi, L"Văn hóa Việt", L"Vietnamese culture"));
    g_popupTitleLunar.Text(PickText(vi, L"Trang phục · Lễ Tết · Nghệ thuật · Phong tục", L"Clothing · Festivals · Arts · Traditions"));
    g_popupZodiacText.Text(L"📖"); g_popupZodiacText.Visibility(wux::Visibility::Visible);
    { std::lock_guard<std::mutex> lock(g_cultureMutex);
      if (g_renderedCultureRevision == g_culture.revision && g_renderedCultureLanguage == int(vi)) return; }

    auto state = ReadCultureState();
    if (state.topics.empty() && !state.catalogLoading && !state.loading && state.error.empty() && g_cultureSelectedId.empty()) {
        RequestCulture(L""); state = ReadCultureState();
    }
    g_cultureSearch.Visibility(g_cultureSelectedId.empty() ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_cultureSearch.PlaceholderText(PickText(vi, L"Tìm chủ đề hoặc nhóm văn hóa…", L"Search topics or categories…"));
    if (g_renderedCultureRevision != state.revision || g_renderedCultureLanguage != int(vi)) {
        g_renderedCultureRevision = state.revision; g_renderedCultureLanguage = int(vi);
        RefreshCultureResults(); ResetPopupScrollPosition(g_cultureScroll);
    }
}
void ReleaseExtendedControls() {
    ReleaseCulturePage(); ReleaseNewControls();
    g_monthScroll = nullptr; g_monthBody = nullptr; g_monthButton = nullptr;
    g_cultureScroll = nullptr; g_cultureBody = nullptr; g_cultureSearch = nullptr; g_cultureResults = nullptr;
    g_popupTabsScroll = nullptr; g_playPauseIcon = nullptr; g_mediaBitmap = nullptr; g_renderedArtwork.reset();
    g_renderedMonth = -1; g_renderedCultureRevision = ~0ULL;
}

std::wstring ActivityLabel(ActivityKind kind, bool vi) {
    switch (kind) {
        case ActivityKind::Media: return PickText(vi, L"Phương tiện", L"Media");
        case ActivityKind::Battery: return PickText(vi, L"Pin", L"Battery");
        case ActivityKind::Privacy: return PickText(vi, L"Quyền riêng tư", L"Privacy");
        case ActivityKind::Culture: return PickText(vi, L"Văn hóa Việt", L"Vietnamese culture");
        case ActivityKind::None: return L"";
        case ActivityKind::Clock: return PickText(vi,L"Đồng hồ",L"Clock");
        case ActivityKind::Obs: return PickText(vi, L"Quay màn hình", L"Recording");
        case ActivityKind::Display: return PickText(vi, L"Trình chiếu", L"Displays");
        case ActivityKind::Hotspot: return PickText(vi, L"Điểm phát sóng", L"Hotspot");
        case ActivityKind::Download: return PickText(vi, L"Tiến trình", L"Tasks");
        case ActivityKind::Remote: return L"UltraViewer";
        case ActivityKind::Memory: return PickText(vi, L"Thiết bị", L"Device");
        default: return PickText(vi, L"Âm lịch & Tử vi", L"Lunar & Almanac");
    }
}

std::wstring FormatDurationMinutes(int minutes, bool vi) {
    if (minutes < 0) return L"";
    if (minutes < 60) return std::to_wstring(minutes) + PickText(vi, L" phút", L" min");
    return std::to_wstring(minutes / 60) + PickText(vi, L" giờ ", L" hr ") +
        std::to_wstring(minutes % 60) + PickText(vi, L" phút", L" min");
}

std::wstring FormatPlaybackTime(int64_t seconds) {
    seconds = std::max<int64_t>(0, seconds);
    auto pair = [](int64_t value) { return (value < 10 ? L"0" : L"") + std::to_wstring(value); };
    if (seconds >= 3600) return std::to_wstring(seconds / 3600) + L":" + pair(seconds / 60 % 60) + L":" + pair(seconds % 60);
    return std::to_wstring(seconds / 60) + L":" + pair(seconds % 60);
}

std::wstring ClockKindLabel(ClockKind kind,bool vi) {
    return kind==ClockKind::Stopwatch?PickText(vi,L"Bấm giờ",L"Stopwatch"):
        kind==ClockKind::Timer?PickText(vi,L"Hẹn giờ",L"Timer"):PickText(vi,L"Báo thức",L"Alarm");
}
std::wstring ClockValueText(ClockEntry const& entry) {
    WCHAR buffer[64]{};
    if(entry.kind==ClockKind::Alarm) {
        uint64_t ticks=static_cast<uint64_t>(entry.alarmUtc)*10000;
        FILETIME ft{static_cast<DWORD>(ticks),static_cast<DWORD>(ticks>>32)};SYSTEMTIME utc{},local{};
        if(FileTimeToSystemTime(&ft,&utc) && SystemTimeToTzSpecificLocalTime(nullptr,&utc,&local))
            swprintf_s(buffer,L"%02u:%02u",local.wHour,local.wMinute);
    } else {
        // Countdown faces round upward; stopwatches show completed seconds.
        auto seconds=ClockDisplaySeconds(entry,GetTickCount64());
        swprintf_s(buffer,L"%02lld:%02lld:%02lld",static_cast<long long>(seconds/3600),static_cast<long long>(seconds/60%60),static_cast<long long>(seconds%60));
    }
    return buffer;
}
std::wstring ClockEntryStatus(ClockEntry const& entry,bool vi) {
    auto now=GetTickCount64();std::wstring text;
    if(entry.kind==ClockKind::Alarm) {
        int64_t minutes=std::max<int64_t>(0,(entry.alarmUtc-ClockUtcMilliseconds()+59999)/60000);
        text=std::to_wstring(minutes)+PickText(vi,L" phút nữa",L" min remaining");
    } else if(entry.finished) {
        text=entry.finishedEstimated?PickText(vi,L"Đã hết giờ · Ước tính",L"Time is up · Estimated"):
            PickText(vi,L"Đã hết giờ",L"Time is up");
    } else text=entry.run==ClockRunState::Running?PickText(vi,L"Đang chạy",L"Running"):
        entry.run==ClockRunState::Paused?PickText(vi,L"Tạm dừng",L"Paused"):PickText(vi,L"Trạng thái chưa xác định",L"State unavailable");
    if(!ClockEntryFresh(entry,now) && !entry.finishedEstimated)
        text+=PickText(vi,L" · Theo lần đồng bộ cuối",L" · Last synced state");
    return text;
}
ClockEntry const* PrimaryClockEntry(ClockSnapshot const& clock) {
    if(GetTickCount64()<clock.noticeUntil && clock.notice==ClockNotice::TimerFinished)
        for(auto const& entry:clock.entries)if(entry.key==clock.noticeKey)return &entry;
    ClockEntry const* timer=nullptr;ClockEntry const* stopwatch=nullptr;
    for(auto const& entry:clock.entries)if(entry.run==ClockRunState::Running) {
        if(entry.kind==ClockKind::Timer && (!timer || ClockReading(entry,GetTickCount64())<ClockReading(*timer,GetTickCount64())))timer=&entry;
        if(entry.kind==ClockKind::Stopwatch)stopwatch=&entry;
    }
    if(timer)return timer;
    if(stopwatch)return stopwatch;
    auto utc=ClockUtcMilliseconds();
    auto alarm=std::find_if(clock.entries.begin(),clock.entries.end(),[&](auto const& entry){return entry.kind==ClockKind::Alarm && AlarmInReminderWindow(entry.alarmUtc,utc);});
    return alarm!=clock.entries.end()?&*alarm:clock.entries.empty()?nullptr:&clock.entries.front();
}
std::wstring ClockNoticeText(ClockSnapshot const& clock,bool vi) {
    std::wstring text;
    switch(clock.notice) {
        case ClockNotice::Started:text=PickText(vi,L"Đồng hồ đã bắt đầu",L"Clock started");break;
        case ClockNotice::TimerFinished:text=PickText(vi,L"Đã hết giờ hẹn",L"Timer finished");break;
        case ClockNotice::AlarmSoon:text=PickText(vi,L"Báo thức trong vòng 15 phút",L"Alarm within 15 minutes");break;
        case ClockNotice::AlarmDue:text=PickText(vi,L"Đã đến giờ báo thức",L"Alarm time reached");break;
        default:break;
    }
    if(!clock.noticeLabel.empty())text+=L" · "+clock.noticeLabel;
    if(clock.noticeEstimated)text+=PickText(vi,L" · Theo lần đồng bộ cuối",L" · Last synced state");
    return text;
}
[[clang::no_destroy]] wuxc::StackPanel g_clockList{nullptr};
std::vector<std::wstring> g_renderedClockKeys;
int g_renderedClockLanguage=-1;
void ReleaseClockControls(){g_clockList=nullptr;g_renderedClockKeys.clear();g_renderedClockLanguage=-1;}
void RefreshClockPopup(bool vi) {
    auto const& clock=g_uiLive.clock;
    g_popupTitleLunar.Text(L"Windows Clock");g_popupZodiacText.Text(L"⏱");
    g_popupLiveProgress.Visibility(wux::Visibility::Collapsed);
    g_popupLiveTitle.Text(clock.entries.empty()?PickText(vi,L"Chưa ghi nhận bộ đếm nào",L"No counters observed yet"):L"");
    g_popupLiveDetail.Text(L"");
    g_popupLiveStatus.Text(L"");
    if(!g_clockList)return;
    std::vector<std::wstring> keys;for(auto const& entry:clock.entries)keys.push_back(entry.key);
    if(keys!=g_renderedClockKeys || g_renderedClockLanguage!=int(vi)) {
        g_clockList.Children().Clear();
        for(auto const& entry:clock.entries) {
            wuxc::StackPanel row;row.Spacing(3);row.Margin({0,0,0,8});
            row.Children().Append(WrappedText(ClockKindLabel(entry.kind,vi)+(entry.label.empty()?L"":L" · "+entry.label),13));
            row.Children().Append(WrappedText(L"",28));row.Children().Append(WrappedText(L"",12));
            g_clockList.Children().Append(row);
        }
        g_renderedClockKeys=std::move(keys);g_renderedClockLanguage=int(vi);
    }
    for(uint32_t i=0;i<clock.entries.size();++i) {
        auto row=g_clockList.Children().GetAt(i).as<wuxc::StackPanel>();
        auto const& entry=clock.entries[i];
        row.Children().GetAt(0).as<wuxc::TextBlock>().Text(ClockKindLabel(entry.kind,vi)+(entry.label.empty()?L"":L" · "+entry.label));
        row.Children().GetAt(1).as<wuxc::TextBlock>().Text(ClockValueText(entry));
        row.Children().GetAt(2).as<wuxc::TextBlock>().Text(ClockEntryStatus(clock.entries[i],vi));
    }
}

std::wstring BatteryHeadline(BatterySnapshot const& b, bool vi) {
    if (!b.valid) return PickText(vi, L"Chưa đọc được pin", L"Battery unavailable");
    if (!b.present) return PickText(vi, L"Không có pin", L"No battery");
    std::wstring label;
    if (b.charging) label = PickText(vi, L"Đang sạc", L"Charging");
    else if (b.pluggedIn && b.percent == 100) label = PickText(vi, L"Đã sạc đầy", L"Fully charged");
    else if (b.pluggedIn) label = PickText(vi, L"Đã cắm nguồn", L"Plugged in");
    else if (b.percent >= 0 && b.percent <= 20) label = PickText(vi, L"Pin yếu", L"Low battery");
    else label = PickText(vi, L"Đang dùng pin", L"On battery");
    return label + L": " + (b.percent >= 0 ? std::to_wstring(b.percent) + L"%" : L"");
}

std::wstring BatteryDetail(BatterySnapshot const& b, bool vi) {
    if (!b.valid || !b.present) return PickText(vi, L"Đang chờ dữ liệu thiết bị", L"Waiting for device data");
    if (b.charging && b.minutesToFull >= 0)
        return std::wstring(PickText(vi, L"Ước tính đầy sau ", L"Estimated full in ")) +
            FormatDurationMinutes(b.minutesToFull, vi);
    if (b.charging) return PickText(vi, L"Đang đo tốc độ sạc để ước tính…", L"Measuring charging rate for an estimate…");
    if (b.pluggedIn) return b.percent == 100
        ? PickText(vi, L"Đang dùng nguồn điện", L"Running on external power")
        : PickText(vi, L"Hiện không sạc pin", L"Not currently charging");
    if (b.minutesRemaining >= 0)
        return std::wstring(PickText(vi, L"Ước tính còn ", L"Estimated remaining: ")) +
            FormatDurationMinutes(b.minutesRemaining, vi);
    return PickText(vi, L"Đang đo mức tiêu thụ để ước tính…", L"Measuring power use for an estimate…");
}

std::wstring ActivityEventKey(ActivityKind kind) {
    auto const& s = g_uiLive;
    switch (kind) {
        case ActivityKind::Lunar: { SYSTEMTIME time{}; GetLocalTime(&time); return std::to_wstring(time.wYear * 10000 + time.wMonth * 100 + time.wDay); }
        case ActivityKind::Media: return s.media.available ? std::to_wstring(s.media.generation) + L":" + std::to_wstring(s.media.playbackSerial) : L"";
        case ActivityKind::Battery: {
            std::wstring key = std::to_wstring(s.battery.pluggedIn) + L":" + std::to_wstring(s.battery.percent < 0 ? -1 :
                s.battery.percent <= 5 ? 5 : s.battery.percent <= 10 ? 10 : s.battery.percent <= 20 ? 20 : 100);
            for (auto const& d : s.bluetooth.devices) key += L":" + d.id + L":" + std::to_wstring(d.percent < 0 ? -1 : d.percent <= 10 ? 10 : d.percent <= 20 ? 20 : 100);
            return key;
        }
        case ActivityKind::Privacy: {
            std::wstring key;
            for (auto const& use : s.privacy.uses) if (!use.recent) key += use.capability + L":" + std::to_wstring(use.started);
            return key;
        }
        case ActivityKind::Download: {
            std::wstring key;
            for (auto const& task : s.tasks) key += std::to_wstring(task.id) + L":" + task.fileKey;
            if (HasTaskNotice(s)) key += L"notice:" + std::to_wstring(s.taskNotice.serial);
            return key;
        }
        case ActivityKind::Clock: return std::to_wstring(s.clock.activitySerial)+L":"+std::to_wstring(s.clock.noticeSerial);
        case ActivityKind::Remote: return std::to_wstring(s.remote.serial);
        case ActivityKind::Memory: return std::to_wstring(s.memory.percent >= 95 ? 95 : s.memory.percent >= 90 ? 90 : s.memory.percent >= 80 ? 80 : 0)+L":"+std::to_wstring(std::max(s.temperature.cpu,s.temperature.gpu)/10);
        case ActivityKind::Obs: return s.obs.recording ? std::to_wstring(s.obs.generation) : L"";
        case ActivityKind::Display: {
            std::wstring key;
            for (auto const& target : s.display.targets) key += target.id + std::to_wstring(target.duplicate);
            return key;
        }
        case ActivityKind::Hotspot: return s.hotspot.on ? L"on:" + std::to_wstring(s.hotspot.clients) : L"";
        default: return L"";
    }
}
void RefreshHiddenActivities() {
    std::erase_if(g_hiddenActivities, [](auto const& hidden) {
        auto key = ActivityEventKey(hidden.first); return !key.empty() && key != hidden.second;
    });
}
void HideCurrentActivity() {
    auto kind = g_selectedActivity == ActivityKind::Culture ? ActivityKind::Lunar : g_selectedActivity;
    if (kind == ActivityKind::None) return;
    g_hiddenActivities[kind] = ActivityEventKey(kind);
    HideExpandedPopupImmediately(g_expandedPopup);
    g_selectedActivity = ActivityKind::None;
    RequestPopupRefresh();
}
void WakeSettingsProviders() {
    if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
    if (g_systemWake) SetEvent(g_systemWake);
    if (g_bluetoothWake) SetEvent(g_bluetoothWake);
    if (g_obsWake) SetEvent(g_obsWake);
    if (g_taskControlWake) SetEvent(g_taskControlWake);
    RequestPopupRefresh();
}
// Settings is a separate, on-demand WPF process. The script is embedded in this
// source and passed through stdin; no .ps1, helper executable or configuration file
// is installed. Only the authenticated child can use the local pipe.
HANDLE g_settingsHostThread = nullptr;
std::mutex g_settingsHostMutex;
constexpr UINT kApplyRemoteSettings = WM_APP + 0x497;
constexpr UINT kRestoreHiddenActivities = WM_APP + 0x498;
PCWSTR SettingsSectionName(int section, bool vi) {
    static constexpr PCWSTR en[] = {L"General", L"Appearance & layout", L"Live activities", L"Task progress", L"OBS recording", L"Lunar & Almanac", L"Device management"};
    static constexpr PCWSTR vn[] = {L"Chung", L"Giao diện & Bố cục", L"Hoạt động trực tiếp", L"Tiến trình tác vụ", L"Quay màn hình OBS", L"Âm lịch & Tử vi", L"Quản lý thiết bị"};
    return vi ? vn[ClampVal(section, 0, 6)] : en[ClampVal(section, 0, 6)];
}
PCWSTR SettingsGroup(WidgetSettingSpec const& spec, bool vi) {
    std::wstring_view key = spec.key;
    switch (spec.section) {
        case 0: return key == L"language" ? PickText(vi,L"Ngôn ngữ",L"Language") : PickText(vi,L"Hoạt động & Preview",L"Activities & previews");
        case 1: return key == L"popupTheme" || key == L"progressStyle" || key == L"settingsBlur" ? PickText(vi,L"Hiệu ứng & Màu sắc",L"Effects & colors") : PickText(vi,L"Vị trí & Kích thước",L"Position & size");
        case 2:
            if(key==L"enableClockWidget" || key==L"clockAutoPopup")return PickText(vi,L"Đồng hồ Windows",L"Windows Clock");
            if (key == L"remoteAlerts") return PickText(vi,L"Điều khiển từ xa",L"Remote control");
            if (key == L"enableMediaWidget") return PickText(vi,L"Phương tiện",L"Media");
            if (key == L"enableBatteryWidget" || key == L"alwaysShowBattery" || key == L"batteryAutoPopup" || key == L"bluetoothBattery") return PickText(vi,L"Pin & Bluetooth",L"Battery & Bluetooth");
            if (key == L"enablePrivacyWidget" || key == L"privacyAutoPopup") return PickText(vi,L"Quyền riêng tư",L"Privacy");
            if (key.starts_with(L"display.")) return PickText(vi,L"Trình chiếu",L"Displays");
            return PickText(vi,L"Điểm phát sóng",L"Hotspot");
        case 3:
            if (key == L"enableTaskProgress" || key == L"taskProgressAutoPopup") return PickText(vi,L"Hiển thị",L"Display");
            if (key == L"progressExplorer" || key == L"progressSevenZip") return PickText(vi,L"Tệp & Lưu trữ",L"Files & archives");
            if (key == L"progressOtherApps") return PickText(vi,L"Ứng dụng khác",L"Other applications");
            return PickText(vi,L"Trình duyệt & Tải xuống",L"Browsers & downloads");
        case 4: return PickText(vi,L"Kết nối OBS",L"OBS connection");
        case 5: return key == L"enableLunarWidget" ? PickText(vi,L"Âm lịch & Tử vi",L"Lunar & Almanac") : PickText(vi,L"Văn hóa Việt",L"Vietnamese culture");
        default: return PickText(vi,L"Cảnh báo & Theo dõi",L"Alerts & monitoring");
    }
}
std::wstring SettingHelp(WidgetSettingSpec const& spec, bool vi) {
    std::wstring_view key = spec.key;
    if(key==L"enableClockWidget" || key==L"clockAutoPopup")return PickText(vi,
        L"Chỉ hiển thị trạng thái. Sau lần đọc đầu, tiếp tục từ mốc đã ghi nhận khi thu nhỏ/đóng ứng dụng; đánh dấu Theo lần đồng bộ cuối cho đến khi đọc lại được. Không xác nhận được thay đổi chưa đồng bộ.",
        L"Status only. After the first observation, counters continue from their last anchor while the app is minimized/closed, marked Last synced state until read again. Unobserved changes cannot be confirmed.");
    if(key==L"popupTheme") return g_glassCpuSupported ? PickText(vi,L"Mặc định Liquid Glass. Off chỉ tắt hiệu ứng popup; nền taskbar vẫn dùng blur khi rê/bấm, không làm mờ chữ.",L"Liquid Glass by default. Off applies to the popup; taskbar hover/press keeps blur on a separate background layer.") :
        PickText(vi,L"Liquid Glass không dùng trên CPU này; tự chuyển sang Windows Blur. Mốc: Intel Core i thế hệ 7+, AMD Ryzen/Zen trở lên; CPU chưa nhận diện dùng Blur.",L"Liquid Glass is unavailable on this CPU; Windows Blur is used. Policy: Intel Core i 7th gen+, AMD Ryzen/Zen or newer; unrecognized CPUs use Blur.");
    if (key == L"temperatureAlerts" || key == L"temperaturePort") return PickText(vi,L"Cần LibreHardwareMonitor bật Remote Web Server trên máy này (mặc định 8085). Không có cảm biến sẽ hiện Chưa có dữ liệu.",L"Requires LibreHardwareMonitor Remote Web Server on this PC (default 8085). Missing sensors show Unavailable.");
    if (spec.type == WidgetSettingType::Number) return std::to_wstring(spec.minimum) + L"–" + std::to_wstring(spec.maximum);
    if (key == L"position") return PickText(vi,L"Vị trí khay hệ thống chừa chỗ riêng.",L"Tray positions reserve their own space.");
    if (key == L"browserDownloadNames") return PickText(vi,L"Tách tiến độ khi trình duyệt cung cấp từng lượt tải; không hiển thị tên file.",L"Separate progress when individual downloads are available; filenames stay hidden.");
    if (key == L"remoteAlerts") return PickText(vi,L"Theo dõi cửa sổ phiên UltraViewer đang hoạt động; không đọc mật khẩu hay nội dung điều khiển.",L"Observe active UltraViewer session windows; no passwords or remote desktop content are read.");
    if (key == L"bluetoothBattery") return PickText(vi,L"Chỉ thiết bị đang kết nối có cung cấp mức pin.",L"Connected devices that report their battery level.");
    if (key == L"enableLunarWidget") return PickText(vi,L"Tự động dựa trên khu vực Windows là Việt Nam.",L"Auto uses the Windows country/region setting.");
    if (key == L"progressOtherApps") return PickText(vi,L"Ngăn cách tên .exe bằng dấu ;",L"Separate executable names with ;");
    if (key == L"cultureCatalogUrl") return PickText(vi,L"Để trống để dùng Wikipedia.",L"Leave empty to use Wikipedia.");
    return {};
}
winrt::Windows::Data::Json::JsonObject SettingsSchema() {
    using namespace winrt::Windows::Data::Json;
    auto settings = GetSettings(); bool vi = UseVietnameseLabels(settings);
    JsonObject result; JsonArray sections, specs;
    auto put = [&](PCWSTR key, std::wstring_view value) { result.SetNamedValue(key, JsonValue::CreateStringValue(value)); };
    put(L"title",PickText(vi,L"Live Update · Cài đặt",L"Live Update · Settings"));
    result.SetNamedValue(L"vi",JsonValue::CreateBooleanValue(vi));
    DWORD light = 1, size = sizeof(light);
    RegGetValueW(HKEY_CURRENT_USER,L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",L"AppsUseLightTheme",RRF_RT_REG_DWORD,nullptr,&light,&size);
    result.SetNamedValue(L"light",JsonValue::CreateBooleanValue(light != 0));
    result.SetNamedValue(L"blur",JsonValue::CreateBooleanValue(settings.settingsBlur && g_effectsAllowed.load()));
    result.SetNamedValue(L"sidebarCompact",JsonValue::CreateBooleanValue(Wh_GetIntValue(L"ui.sidebarCompact",0) != 0));
    auto accent = SystemAccentColor(); WCHAR color[12]; swprintf_s(color,L"#%02X%02X%02X",accent.R,accent.G,accent.B); put(L"accent",color);
    put(L"accentText",accent.R * .299 + accent.G * .587 + accent.B * .114 > 155 ? L"#161616" : L"#FFFFFF");
    result.SetNamedValue(L"revision",JsonValue::CreateNumberValue(static_cast<double>(g_settingsRevision.load() * 2 + (g_effectsAllowed.load() ? 1 : 0))));
    put(L"status",PickText(vi,L"Lưu tự động. Đóng cửa sổ để giải phóng bộ nhớ của giao diện cài đặt.",L"Changes save automatically. Close this window to release the settings process memory."));
    PCWSTR icons[] = {L"⚙",L"◈",L"▦",L"↓",L"●",L"▦",L"▤"};
    for (int i = 0; i < 7; ++i) {
        JsonObject section; section.SetNamedValue(L"id",JsonValue::CreateNumberValue(i));
        section.SetNamedValue(L"label",JsonValue::CreateStringValue(SettingsSectionName(i,vi)));
        section.SetNamedValue(L"glyph",JsonValue::CreateStringValue(icons[i])); sections.Append(section);
    }
    // Stable grouping: a group's cards stay adjacent even if preference keys were added later.
    for (int section = 0; section < 7; ++section) {
        std::vector<std::wstring> groups;
        for (auto const& spec : kWidgetSettings) if (spec.section == section) {
            std::wstring group = SettingsGroup(spec,vi);
            if (std::find(groups.begin(),groups.end(),group) == groups.end()) groups.push_back(group);
        }
        for (auto const& group : groups) for (auto const& spec : kWidgetSettings) {
            if (spec.section != section || group != SettingsGroup(spec,vi)) continue;
            JsonObject row;
            auto field = [&](PCWSTR key,std::wstring_view value) { row.SetNamedValue(key,JsonValue::CreateStringValue(value)); };
            field(L"key",spec.key); field(L"label",vi ? spec.vi : spec.en); field(L"group",group);
            field(L"search",std::wstring(spec.en)+L" "+spec.vi); field(L"help",SettingHelp(spec,vi)); field(L"value",WidgetSettingValue(spec));
            field(L"type",spec.type == WidgetSettingType::Toggle ? L"toggle" : spec.type == WidgetSettingType::Choice ? L"choice" :
                spec.type == WidgetSettingType::Number ? L"number" : spec.type == WidgetSettingType::Password ? L"password" : L"text");
            row.SetNamedValue(L"section",JsonValue::CreateNumberValue(section));
            row.SetNamedValue(L"maximum",JsonValue::CreateNumberValue(spec.maximum));
            JsonArray options; for (auto const& option : WidgetSettingOptions(spec.key)) {
                JsonObject value; value.SetNamedValue(L"value",JsonValue::CreateStringValue(option.value));
                value.SetNamedValue(L"label",JsonValue::CreateStringValue(vi ? option.vi : option.en)); options.Append(value);
            }
            row.SetNamedValue(L"options",options); specs.Append(row);
        }
    }
    result.SetNamedValue(L"sections",sections); result.SetNamedValue(L"settings",specs);
    JsonObject words;
    auto word = [&](PCWSTR key,PCWSTR en,PCWSTR vn){ words.SetNamedValue(key,JsonValue::CreateStringValue(vi ? vn : en)); };
    word(L"save",L"Save",L"Lưu"); word(L"search",L"Find a setting",L"Tìm cài đặt");
    word(L"results",L"Search results",L"Kết quả tìm kiếm"); word(L"restore",L"Show hidden widgets",L"Hiện lại các widget đã ẩn");
    word(L"taskmgr",L"Open Task Manager",L"Mở Trình quản lý tác vụ");
    word(L"closeSettings",L"Close settings",L"Đóng cài đặt");
    word(L"storage",L"Volumes · Capacity and free space",L"Ổ đĩa · Dung lượng và chỗ trống");
    word(L"storageSettings",L"Windows storage settings",L"Cài đặt lưu trữ Windows");
    word(L"sidebar",L"Collapse / expand navigation",L"Thu gọn / mở rộng menu");
    word(L"apps",L"Applications · Working set",L"Ứng dụng · Bộ nhớ đang dùng");
    if (g_hostSyncStatus == 3) put(L"status",PickText(vi,L"Đã lưu. Windhawk này không có CLI đồng bộ ngược ba mục chung.",L"Saved. This Windhawk has no CLI to sync the three shared fields back."));
    if (g_hostSyncStatus == 4) put(L"status",PickText(vi,L"Đã lưu. Windhawk chưa chấp nhận đồng bộ ngược.",L"Saved. Windhawk has not accepted reverse synchronization."));
    result.SetNamedValue(L"words",words); return result;
}
void IdentifySettingsWindow() {
    DWORD process = g_settingsHostPid.load();
    if (!process) return;
    EnumWindows([](HWND window, LPARAM value) -> BOOL {
        DWORD pid = 0; GetWindowThreadProcessId(window, &pid);
        if (pid != static_cast<DWORD>(value) || !IsWindowVisible(window)) return TRUE;
        winrt::com_ptr<IPropertyStore> store;
        if (SUCCEEDED(SHGetPropertyStoreForWindow(window, __uuidof(IPropertyStore), store.put_void()))) {
            PROPVARIANT id{}; id.vt = VT_LPWSTR; id.pwszVal = const_cast<PWSTR>(L"Windhawk.LiveUpdate.Settings");
            store->SetValue(PKEY_AppUserModel_ID, id);
        }
        return FALSE;
    }, process);
}
winrt::Windows::Data::Json::JsonObject SettingsRequest(winrt::Windows::Data::Json::JsonObject const& request) {
    using namespace winrt::Windows::Data::Json;
    auto op = request.GetNamedString(L"op",L""); bool vi = UseVietnameseLabels(GetSettings());
    JsonObject response; response.SetNamedValue(L"ok",JsonValue::CreateBooleanValue(true));
    if (op == L"schema") return SettingsSchema();
    if (op == L"ready") IdentifySettingsWindow();
    if (op == L"version") response.SetNamedValue(L"revision",JsonValue::CreateNumberValue(static_cast<double>(g_settingsRevision.load() * 2 + (g_effectsAllowed.load() ? 1 : 0))));
    else if (op == L"set") {
        std::wstring key(request.GetNamedString(L"key",L"")), value(request.GetNamedString(L"value",L""));
        auto spec = std::find_if(std::begin(kWidgetSettings),std::end(kWidgetSettings),[&](auto const& s){return key == s.key;});
        bool ok = spec != std::end(kWidgetSettings) && ValidateWidgetSetting(*spec,value);
        if (ok) {
            { std::lock_guard<std::mutex> lock(g_configStorageMutex); ok = Wh_SetStringValue((L"config."+key).c_str(),value.c_str()); }
            if (ok) {
                LoadSettings(); ++g_settingsRevision;
                if (SharedHostSetting(key)) { g_hostSyncInFlight = true; NotifySettingsStatus(1); g_hostSyncRequested = true; }
                g_forcePowerRefresh = true; g_displayDirty = true; g_bluetoothDirty = true; WakeSettingsProviders();
                if (auto window = g_liveUiWindow.load()) PostMessageW(window,kApplyRemoteSettings,0,0);
            }
        }
        response.SetNamedValue(L"ok",JsonValue::CreateBooleanValue(ok));
        response.SetNamedValue(L"message",JsonValue::CreateStringValue(ok ? PickText(vi,L"Đã lưu.",L"Saved.") : PickText(vi,L"Không lưu được. Kiểm tra giá trị.",L"Could not save. Check the value.")));
    } else if (op == L"memory") {
        MemorySnapshot memory; { std::lock_guard<std::mutex> lock(g_liveMutex); memory = g_memorySnapshot; }
        response.SetNamedValue(L"valid",JsonValue::CreateBooleanValue(memory.valid));
        response.SetNamedValue(L"percent",JsonValue::CreateNumberValue(memory.percent));
        auto summary = std::to_wstring(memory.percent)+L"% · "+MemorySize(memory.total-memory.available)+L" / "+MemorySize(memory.total);
        response.SetNamedValue(L"summary",JsonValue::CreateStringValue(summary));
        TemperatureSnapshot temperature;{std::lock_guard<std::mutex> lock(g_liveMutex);temperature=g_temperatureSnapshot;}
        response.SetNamedValue(L"temperature",JsonValue::CreateStringValue(TemperatureText(temperature,vi)));
        auto details = std::wstring(PickText(vi,L"Cửa sổ cài đặt: ",L"Settings process: "))+MemorySize(memory.settingsWorkingSet)+
            PickText(vi,L"\nExplorer (gồm các thành phần khác): ",L"\nExplorer (includes other components): ")+MemorySize(memory.explorerWorkingSet)+
            PickText(vi,L"\nKhông thể tách chính xác RAM riêng của mod trong Explorer. Bộ nhớ dùng chung có thể được tính ở nhiều ứng dụng.",
                L"\nMod-only RAM inside Explorer cannot be isolated. Shared pages can be counted in multiple applications.");
        response.SetNamedValue(L"details",JsonValue::CreateStringValue(details));
        JsonArray apps; for (auto const& app : memory.top) {
            JsonObject row; row.SetNamedValue(L"name",JsonValue::CreateStringValue(app.name));
            row.SetNamedValue(L"size",JsonValue::CreateStringValue(MemorySize(app.workingSet))); apps.Append(row);
        }
        response.SetNamedValue(L"apps",apps);
        std::vector<StorageVolume> volumes; {std::lock_guard<std::mutex> lock(g_liveMutex);volumes=g_storageVolumes;}
        JsonArray disks;
        for(auto const& volume:volumes){JsonObject disk;
            disk.SetNamedValue(L"name",JsonValue::CreateStringValue(volume.root+L"  "+volume.label));
            disk.SetNamedValue(L"percent",JsonValue::CreateNumberValue(volume.total ? 100.0*(volume.total-volume.free)/volume.total : 0));
            disk.SetNamedValue(L"size",JsonValue::CreateStringValue(MemorySize(volume.free)+PickText(vi,L" trống / ",L" free / ")+MemorySize(volume.total)));
            disks.Append(disk);
        }
        response.SetNamedValue(L"volumes",disks);
    } else if (op == L"watchMemory") { g_memoryDetailsRequested = request.GetNamedBoolean(L"on",false); if(g_systemWake) SetEvent(g_systemWake); }
    else if (op == L"restore") { if(auto window=g_liveUiWindow.load()) PostMessageW(window,kRestoreHiddenActivities,0,0); }
    else if (op == L"sidebar") { Wh_SetIntValue(L"ui.sidebarCompact",request.GetNamedBoolean(L"compact",false)); }
    else if (op == L"storageSettings") { ShellExecuteW(nullptr,L"open",L"ms-settings:storagesense",nullptr,nullptr,SW_SHOWNORMAL); }
    else if (op == L"taskmgr") {
        WCHAR path[MAX_PATH]; UINT n=GetSystemDirectoryW(path,ARRAYSIZE(path));
        if(n && n < ARRAYSIZE(path)-14) { std::wstring exe=std::wstring(path)+L"\\Taskmgr.exe"; ShellExecuteW(nullptr,L"open",exe.c_str(),nullptr,nullptr,SW_SHOWNORMAL); }
    }
    return response;
}
// All outstanding IPC is cancelable before DLL unload. A child never receives a
// function pointer into Explorer; the pipe contains bounded UTF-8 JSON only.
bool SettingsPipeIO(HANDLE pipe,HANDLE child,void* data,DWORD bytes,bool writing,DWORD& transferred) {
    OVERLAPPED ov{}; ov.hEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr); if(!ov.hEvent) return false;
    BOOL ok=writing ? WriteFile(pipe,data,bytes,&transferred,&ov) : ReadFile(pipe,data,bytes,&transferred,&ov);
    if(!ok && GetLastError()==ERROR_IO_PENDING) {
        HANDLE wait[]={g_liveStopEvent,child,ov.hEvent}; DWORD status=WaitForMultipleObjects(3,wait,FALSE,INFINITE);
        if(status!=WAIT_OBJECT_0+2) { CancelIoEx(pipe,&ov); GetOverlappedResult(pipe,&ov,&transferred,TRUE); ok=FALSE; }
        else ok=GetOverlappedResult(pipe,&ov,&transferred,FALSE);
    }
    CloseHandle(ov.hEvent); return ok != FALSE;
}
constexpr char kSettingsScript[] = R"V29PS(
& {
$ErrorActionPreference='Stop'
Add-Type -AssemblyName PresentationFramework, PresentationCore, WindowsBase
$global:LU_pipe=New-Object System.IO.Pipes.NamedPipeClientStream('.', '__PIPE__', [System.IO.Pipes.PipeDirection]::InOut)
$global:LU_pipe.Connect(8000)
$global:LU_reader=New-Object System.IO.StreamReader($global:LU_pipe,[System.Text.Encoding]::UTF8,$false,4096,$true)
$global:LU_writer=New-Object System.IO.StreamWriter($global:LU_pipe,(New-Object System.Text.UTF8Encoding($false)),4096,$true)
$global:LU_writer.AutoFlush=$true
function global:Rpc($value) {
    $global:LU_writer.WriteLine(($value | ConvertTo-Json -Compress -Depth 8))
    $pending=$global:LU_reader.ReadLineAsync()
    if (!$pending.Wait(2000)) { $global:LU_pipe.Dispose(); throw 'Live Update is not responding.' }
    if ($null -eq $pending.Result) { throw 'Live Update has stopped.' }
    return ($pending.Result | ConvertFrom-Json)
}
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class LuBackdrop {
 [StructLayout(LayoutKind.Sequential)] public struct Margins { public int L,R,T,B; }
 [DllImport("dwmapi.dll")] static extern int DwmSetWindowAttribute(IntPtr h,int a,ref int v,int n);
 [DllImport("dwmapi.dll")] static extern int DwmExtendFrameIntoClientArea(IntPtr h,ref Margins m);
 public static bool Apply(IntPtr h,bool enabled,bool dark) {
  int d=dark?1:0;DwmSetWindowAttribute(h,20,ref d,4);
  int material=enabled?3:1;
  int hr=DwmSetWindowAttribute(h,38,ref material,4);
  bool ok=enabled&&hr>=0;
  var margin=new Margins();if(ok){margin.L=margin.R=margin.T=margin.B=-1;}
  if(DwmExtendFrameIntoClientArea(h,ref margin)<0) ok=false;
  return ok;
 }
}
'@
$global:LU_data=Rpc @{op='schema'}
$global:LU_sidebarCompact=$global:LU_data.sidebarCompact
$global:LU_section=0; $global:LU_building=$false
$global:LU_brush=New-Object System.Windows.Media.BrushConverter
function global:Brush($color) { return $global:LU_brush.ConvertFromString($color) }
function global:Text($value,$size=14) {
    $t=New-Object System.Windows.Controls.TextBlock
    $t.Text=[string]$value; $t.FontSize=$size; $t.TextWrapping='Wrap'; $t.VerticalAlignment='Center'
    return $t
}
function global:Pick($en,$vi) { if ($global:LU_data.vi) { return $vi }; return $en }
[xml]$markup=@'
<Window xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation" xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" Title="Live Update" Width="1080" Height="740" MinWidth="800" MinHeight="480" WindowStartupLocation="CenterScreen" FontFamily="Segoe UI" FontSize="14" ShowInTaskbar="True">
 <Window.Icon><DrawingImage><DrawingImage.Drawing><DrawingGroup>
  <GeometryDrawing Brush="#7460CE" Geometry="M 2,2 L 30,2 30,30 2,30 Z"/>
  <GeometryDrawing Brush="White" Geometry="M 7,8 L 25,8 25,12 7,12 Z M 7,15 L 18,15 18,18 7,18 Z M 7,21 L 23,21 23,24 7,24 Z"/>
 </DrawingGroup></DrawingImage.Drawing></DrawingImage></Window.Icon>
 <Window.Resources>
  <SolidColorBrush x:Key="Accent" Color="#6750A4"/>
  <SolidColorBrush x:Key="Card" Color="#FFFFFF"/>
  <SolidColorBrush x:Key="Edge" Color="#20000000"/>
  <Style TargetType="Button">
   <Setter Property="Padding" Value="14,9"/><Setter Property="Margin" Value="0,0,6,0"/>
   <Setter Property="Background" Value="{DynamicResource Card}"/><Setter Property="Foreground" Value="{Binding Foreground,RelativeSource={RelativeSource AncestorType=Window}}"/>
   <Setter Property="BorderBrush" Value="{DynamicResource Edge}"/><Setter Property="BorderThickness" Value="1"/>
   <Setter Property="Template"><Setter.Value><ControlTemplate TargetType="Button">
    <Border x:Name="Frame" Background="{TemplateBinding Background}" BorderBrush="{TemplateBinding BorderBrush}" BorderThickness="{TemplateBinding BorderThickness}" CornerRadius="7" Padding="{TemplateBinding Padding}"><ContentPresenter VerticalAlignment="Center" HorizontalAlignment="{TemplateBinding HorizontalContentAlignment}"/></Border>
    <ControlTemplate.Triggers><Trigger Property="IsMouseOver" Value="True"><Setter TargetName="Frame" Property="Opacity" Value="0.8"/></Trigger><Trigger Property="IsEnabled" Value="False"><Setter TargetName="Frame" Property="Opacity" Value="0.45"/></Trigger></ControlTemplate.Triggers>
   </ControlTemplate></Setter.Value></Setter>
  </Style>
  <SolidColorBrush x:Key="TextColor" Color="#202024"/>
  <SolidColorBrush x:Key="ControlFill" Color="#FFFFFF"/>
  <SolidColorBrush x:Key="ControlEdge" Color="#26000000"/>
  <SolidColorBrush x:Key="ControlHover" Color="#0B000000"/>
  <SolidColorBrush x:Key="SelectionFill" Color="#10000000"/>
  <SolidColorBrush x:Key="PopupFill" Color="#FAFAFC"/>
  <Style TargetType="TextBox">
   <Setter Property="MinHeight" Value="36"/><Setter Property="Padding" Value="11,7"/>
   <Setter Property="Background" Value="{DynamicResource ControlFill}"/>
   <Setter Property="Foreground" Value="{DynamicResource TextColor}"/>
   <Setter Property="CaretBrush" Value="{DynamicResource TextColor}"/>
   <Setter Property="BorderBrush" Value="{DynamicResource ControlEdge}"/>
   <Setter Property="BorderThickness" Value="1"/><Setter Property="VerticalContentAlignment" Value="Center"/>
   <Setter Property="Template"><Setter.Value><ControlTemplate TargetType="TextBox">
    <Grid><Border x:Name="Field" CornerRadius="6" Background="{TemplateBinding Background}" BorderBrush="{TemplateBinding BorderBrush}" BorderThickness="{TemplateBinding BorderThickness}">
     <ScrollViewer x:Name="PART_ContentHost" Margin="{TemplateBinding Padding}"/>
    </Border><Border x:Name="FocusLine" Height="2" CornerRadius="1" Margin="2,0,2,0" VerticalAlignment="Bottom" Background="{DynamicResource Accent}" Visibility="Collapsed"/></Grid>
    <ControlTemplate.Triggers>
     <Trigger Property="IsKeyboardFocused" Value="True"><Setter TargetName="FocusLine" Property="Visibility" Value="Visible"/></Trigger>
     <Trigger Property="IsEnabled" Value="False"><Setter TargetName="Field" Property="Opacity" Value="0.45"/></Trigger>
    </ControlTemplate.Triggers>
   </ControlTemplate></Setter.Value></Setter>
  </Style>
  <Style TargetType="PasswordBox">
   <Setter Property="MinHeight" Value="36"/><Setter Property="Padding" Value="11,7"/>
   <Setter Property="Background" Value="{DynamicResource ControlFill}"/>
   <Setter Property="Foreground" Value="{DynamicResource TextColor}"/>
   <Setter Property="CaretBrush" Value="{DynamicResource TextColor}"/>
   <Setter Property="BorderBrush" Value="{DynamicResource ControlEdge}"/>
   <Setter Property="Template"><Setter.Value><ControlTemplate TargetType="PasswordBox">
    <Grid><Border CornerRadius="6" Background="{TemplateBinding Background}" BorderBrush="{TemplateBinding BorderBrush}" BorderThickness="1">
     <ScrollViewer x:Name="PART_ContentHost" Margin="{TemplateBinding Padding}"/>
    </Border><Border x:Name="FocusLine" Height="2" CornerRadius="1" Margin="2,0,2,0" VerticalAlignment="Bottom" Background="{DynamicResource Accent}" Visibility="Collapsed"/></Grid>
    <ControlTemplate.Triggers><Trigger Property="IsKeyboardFocused" Value="True"><Setter TargetName="FocusLine" Property="Visibility" Value="Visible"/></Trigger><Trigger Property="IsEnabled" Value="False"><Setter Property="Opacity" Value="0.45"/></Trigger></ControlTemplate.Triggers>
   </ControlTemplate></Setter.Value></Setter>
  </Style>
  <Style TargetType="ComboBoxItem">
   <Setter Property="Foreground" Value="{DynamicResource TextColor}"/><Setter Property="MinHeight" Value="38"/>
   <Setter Property="HorizontalContentAlignment" Value="Stretch"/>
   <Setter Property="Template"><Setter.Value><ControlTemplate TargetType="ComboBoxItem">
    <Grid Margin="4,2"><Border x:Name="ItemFrame" CornerRadius="5" Background="Transparent" Padding="14,8,12,8">
     <ContentPresenter VerticalAlignment="Center" HorizontalAlignment="{TemplateBinding HorizontalContentAlignment}"/>
    </Border><Border x:Name="SelectionMark" Width="3" Height="16" CornerRadius="2" HorizontalAlignment="Left" VerticalAlignment="Center" Margin="2,0,0,0" Background="{DynamicResource Accent}" Visibility="Collapsed"/></Grid>
    <ControlTemplate.Triggers>
     <Trigger Property="IsHighlighted" Value="True"><Setter TargetName="ItemFrame" Property="Background" Value="{DynamicResource ControlHover}"/></Trigger>
     <Trigger Property="IsSelected" Value="True"><Setter TargetName="ItemFrame" Property="Background" Value="{DynamicResource SelectionFill}"/><Setter TargetName="SelectionMark" Property="Visibility" Value="Visible"/></Trigger>
     <Trigger Property="IsEnabled" Value="False"><Setter Property="Opacity" Value="0.45"/></Trigger>
    </ControlTemplate.Triggers>
   </ControlTemplate></Setter.Value></Setter>
  </Style>
  <Style TargetType="ComboBox">
   <Setter Property="MinHeight" Value="36"/><Setter Property="Foreground" Value="{DynamicResource TextColor}"/>
   <Setter Property="Background" Value="{DynamicResource ControlFill}"/><Setter Property="BorderBrush" Value="{DynamicResource ControlEdge}"/>
   <Setter Property="ScrollViewer.HorizontalScrollBarVisibility" Value="Disabled"/>
   <Setter Property="ScrollViewer.VerticalScrollBarVisibility" Value="Auto"/>
   <Setter Property="Template"><Setter.Value><ControlTemplate TargetType="ComboBox">
    <Grid>
     <ToggleButton x:Name="DropButton" Style="{x:Null}" Focusable="False" ClickMode="Press" HorizontalContentAlignment="Stretch"
       IsChecked="{Binding IsDropDownOpen,Mode=TwoWay,RelativeSource={RelativeSource TemplatedParent}}">
      <ToggleButton.Template><ControlTemplate TargetType="ToggleButton">
       <Border x:Name="ToggleFrame" CornerRadius="6" Background="{DynamicResource ControlFill}" BorderBrush="{DynamicResource ControlEdge}" BorderThickness="1">
        <Grid><Border x:Name="Hover" CornerRadius="6" Background="Transparent"/>
         <Path x:Name="Chevron" Data="M 0,0 L 4,4 8,0" Stroke="{DynamicResource TextColor}" StrokeThickness="1.2" HorizontalAlignment="Right" VerticalAlignment="Center" Margin="0,0,12,0"/></Grid>
       </Border><ControlTemplate.Triggers>
        <Trigger Property="IsMouseOver" Value="True"><Setter TargetName="Hover" Property="Background" Value="{DynamicResource ControlHover}"/></Trigger>
        <Trigger Property="IsChecked" Value="True"><Setter TargetName="ToggleFrame" Property="BorderBrush" Value="{DynamicResource Accent}"/></Trigger>
       </ControlTemplate.Triggers>
      </ControlTemplate></ToggleButton.Template>
     </ToggleButton>
     <ContentPresenter Content="{TemplateBinding SelectionBoxItem}" ContentTemplate="{TemplateBinding SelectionBoxItemTemplate}" ContentTemplateSelector="{TemplateBinding ItemTemplateSelector}"
       Margin="12,6,32,6" HorizontalAlignment="Left" VerticalAlignment="Center" IsHitTestVisible="False"/>
     <Border x:Name="KeyboardFocus" BorderThickness="1" CornerRadius="6" BorderBrush="{DynamicResource Accent}" IsHitTestVisible="False" Visibility="Collapsed"/>
     <Popup x:Name="PART_Popup" Placement="Bottom" VerticalOffset="3" AllowsTransparency="True" Focusable="False" PopupAnimation="Fade" IsOpen="{TemplateBinding IsDropDownOpen}">
      <Border Background="{DynamicResource PopupFill}" BorderBrush="{DynamicResource ControlEdge}" BorderThickness="1" CornerRadius="8" Padding="0,4" Margin="7,3,7,10"
        MinWidth="{Binding ActualWidth,RelativeSource={RelativeSource TemplatedParent}}" TextElement.Foreground="{DynamicResource TextColor}">
       <Border.Effect><DropShadowEffect BlurRadius="14" ShadowDepth="3" Opacity="0.18"/></Border.Effect>
       <ScrollViewer MaxHeight="360" CanContentScroll="True" HorizontalScrollBarVisibility="Disabled" VerticalScrollBarVisibility="Auto">
        <ItemsPresenter KeyboardNavigation.DirectionalNavigation="Contained"/>
       </ScrollViewer>
      </Border>
     </Popup>
    </Grid>
    <ControlTemplate.Triggers>
     <Trigger Property="IsKeyboardFocusWithin" Value="True"><Setter TargetName="KeyboardFocus" Property="Visibility" Value="Visible"/></Trigger>
     <Trigger Property="IsEnabled" Value="False"><Setter Property="Opacity" Value="0.45"/></Trigger>
    </ControlTemplate.Triggers>
   </ControlTemplate></Setter.Value></Setter>
  </Style>
  <Style TargetType="ToggleButton">
   <Setter Property="Width" Value="42"/><Setter Property="Height" Value="24"/>
   <Setter Property="Template"><Setter.Value><ControlTemplate TargetType="ToggleButton">
    <Border x:Name="Track" CornerRadius="12" BorderThickness="1" BorderBrush="{DynamicResource Edge}" Background="#777777"><Ellipse x:Name="Thumb" Fill="White" Width="16" Height="16" Margin="3" HorizontalAlignment="Left"/></Border>
    <ControlTemplate.Triggers><Trigger Property="IsChecked" Value="True"><Setter TargetName="Track" Property="Background" Value="{DynamicResource Accent}"/><Setter TargetName="Thumb" Property="HorizontalAlignment" Value="Right"/></Trigger></ControlTemplate.Triggers>
   </ControlTemplate></Setter.Value></Setter>
  </Style>
 </Window.Resources>
 <Grid Margin="24,18,24,18">
  <Grid.RowDefinitions><RowDefinition Height="58"/><RowDefinition Height="*"/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
  <Grid.ColumnDefinitions><ColumnDefinition x:Name="NavColumn" Width="212"/><ColumnDefinition Width="24"/><ColumnDefinition Width="*"/></Grid.ColumnDefinitions>
  <StackPanel Orientation="Horizontal" VerticalAlignment="Top">
   <Button x:Name="NavToggle" Width="40" Height="38" Padding="0" Margin="0,0,10,0" FontFamily="Segoe MDL2 Assets" Content="&#xE700;"/>
   <TextBlock x:Name="NavTitle" Text="Live Update" FontSize="20" FontWeight="SemiBold" VerticalAlignment="Center"/>
  </StackPanel>
  <Border x:Name="SearchFrame" Grid.ColumnSpan="3" Width="520" Height="38" HorizontalAlignment="Center" VerticalAlignment="Top" Background="{DynamicResource Card}" BorderBrush="{DynamicResource Edge}" BorderThickness="1" CornerRadius="19">
   <Grid Margin="14,0,12,0"><Grid.ColumnDefinitions><ColumnDefinition Width="24"/><ColumnDefinition Width="*"/></Grid.ColumnDefinitions>
    <TextBlock Text="&#xE721;" FontFamily="Segoe MDL2 Assets" VerticalAlignment="Center"/>
    <TextBox x:Name="Search" Grid.Column="1" BorderThickness="0" Background="Transparent" Padding="4,7,4,7" MaxLength="80"/>
    <TextBlock x:Name="SearchHint" Grid.Column="1" Margin="6,0,0,0" Opacity="0.55" IsHitTestVisible="False" VerticalAlignment="Center"/>
   </Grid>
  </Border>
  <ScrollViewer Grid.Row="1" VerticalScrollBarVisibility="Auto" HorizontalScrollBarVisibility="Disabled"><StackPanel x:Name="Navigation"/></ScrollViewer>
  <Grid Grid.Row="1" Grid.Column="2"><Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition Height="*"/></Grid.RowDefinitions>
   <TextBlock x:Name="Heading" FontSize="28" FontWeight="SemiBold" Margin="0,0,0,18"/>
   <ScrollViewer x:Name="ContentScroll" Grid.Row="1" HorizontalScrollBarVisibility="Disabled" VerticalScrollBarVisibility="Auto"><StackPanel x:Name="Body" Margin="0,0,14,12"/></ScrollViewer>
  </Grid>
  <TextBlock x:Name="Status" Grid.Row="2" Grid.ColumnSpan="3" Margin="0,12,0,0" FontSize="12" Opacity="0.7" TextWrapping="Wrap"/>
 </Grid>
</Window>
'@
$global:LU_window=[System.Windows.Markup.XamlReader]::Load((New-Object System.Xml.XmlNodeReader($markup)))
foreach ($name in @('Search','SearchFrame','SearchHint','Navigation','Heading','Body','Status','ContentScroll','NavToggle','NavTitle','NavColumn')) { Set-Variable -Name ('LU_'+$name) -Scope Global -Value $global:LU_window.FindName($name) }
function global:Theme {
    $light=$global:LU_data.light
    $handle=(New-Object System.Windows.Interop.WindowInteropHelper($global:LU_window)).Handle
    $glass=$false
    if($handle -ne [IntPtr]::Zero){
        $glass=[LuBackdrop]::Apply($handle,($global:LU_data.blur -and ![System.Windows.SystemParameters]::HighContrast),!$light)
        $source=[System.Windows.Interop.HwndSource]::FromHwnd($handle)
        if($source){$source.CompositionTarget.BackgroundColor=[System.Windows.Media.Colors]::Transparent}
    }
    $global:LU_window.Background=Brush $(if($glass){'#12000000'}elseif($light){'#F3F3F7'}else{'#202022'})
    $global:LU_window.Foreground=Brush $(if($light){'#202024'}else{'#F4F4F6'})
    $global:LU_window.Resources['Card']=Brush $(if($glass){if($light){'#A8FFFFFF'}else{'#A82D2D30'}}elseif($light){'#FFFFFF'}else{'#2D2D30'})
    $global:LU_window.Resources['Edge']=Brush $(if($light){'#20000000'}else{'#30FFFFFF'})
    $global:LU_window.Resources['Accent']=Brush $global:LU_data.accent
    $global:LU_window.Resources['TextColor']=Brush $(if($light){'#202024'}else{'#F4F4F6'})
    $global:LU_window.Resources['ControlFill']=Brush $(if($light){'#FEFEFE'}else{'#38383B'})
    $global:LU_window.Resources['ControlEdge']=Brush $(if($light){'#26000000'}else{'#28FFFFFF'})
    $global:LU_window.Resources['ControlHover']=Brush $(if($light){'#0B000000'}else{'#10FFFFFF'})
    $global:LU_window.Resources['SelectionFill']=Brush $(if($light){'#10000000'}else{'#18FFFFFF'})
    $global:LU_window.Resources['PopupFill']=Brush $(if($light){'#FAFAFC'}else{'#2C2C30'})
}
function global:Save($key,$value) {
    if($global:LU_building){return}
    try {
        $r=Rpc @{op='set';key=$key;value=[string]$value}
        $global:LU_Status.Text=$r.message
        if(!$r.ok){return}
        $global:LU_data=Rpc @{op='schema'}
        if($key -eq 'language' -or $key -eq 'settingsBlur'){ Build }
    } catch { $global:LU_Status.Text=$_.Exception.Message }
}
function global:Button($caption,$action) {
    $b=New-Object System.Windows.Controls.Button; $b.Content=$caption
    $b.Add_Click($action); return $b
}
function global:Card($spec) {
    $card=New-Object System.Windows.Controls.Border
    $card.CornerRadius=7; $card.BorderThickness=1; $card.Padding='16,14,16,14'; $card.Margin='0,0,0,3'
    $card.SetResourceReference([System.Windows.Controls.Border]::BackgroundProperty,'Card')
    $card.SetResourceReference([System.Windows.Controls.Border]::BorderBrushProperty,'Edge')
    $row=New-Object System.Windows.Controls.Grid
    $a=New-Object System.Windows.Controls.ColumnDefinition; $a.Width=New-Object System.Windows.GridLength(1,[System.Windows.GridUnitType]::Star)
    $b=New-Object System.Windows.Controls.ColumnDefinition; $b.Width=[System.Windows.GridLength]::Auto
    $row.ColumnDefinitions.Add($a);$row.ColumnDefinitions.Add($b)
    $label=New-Object System.Windows.Controls.StackPanel; $label.Margin='0,0,20,0'; $label.VerticalAlignment='Center'
    $label.Children.Add((Text $spec.label)) > $null
    if($spec.help){$h=Text $spec.help 12; $h.Opacity=.65;$h.Margin='0,4,0,0';$label.Children.Add($h)>$null}
    $row.Children.Add($label)>$null
    $key=[string]$spec.key
    if($spec.type -eq 'toggle') {
        $editor=New-Object System.Windows.Controls.Primitives.ToggleButton; $editor.IsChecked=($spec.value -eq '1')
        [System.Windows.Automation.AutomationProperties]::SetName($editor,$spec.label)
        $editor.Add_Click({param($sender,$e);Save $key $(if($sender.IsChecked){'1'}else{'0'})}.GetNewClosure())
    } elseif($spec.type -eq 'choice') {
        $editor=New-Object System.Windows.Controls.ComboBox; $editor.Width=215; $editor.Padding='8,6,8,6'
        foreach($o in $spec.options){$item=New-Object System.Windows.Controls.ComboBoxItem;$item.Content=$o.label;$item.Tag=$o.value;$editor.Items.Add($item)>$null;if($o.value -eq $spec.value){$editor.SelectedItem=$item}}
        $editor.Add_SelectionChanged({param($sender,$e);if($sender.SelectedItem){Save $key ([string]$sender.SelectedItem.Tag)}}.GetNewClosure())
    } else {
        $editor=New-Object System.Windows.Controls.StackPanel; $editor.Orientation='Horizontal'
        if($spec.type -eq 'password') {$box=New-Object System.Windows.Controls.PasswordBox;$box.Password=$spec.value;$box.MaxLength=$spec.maximum}
        else {$box=New-Object System.Windows.Controls.TextBox;$box.Text=$spec.value;$box.MaxLength=$(if($spec.type -eq 'number'){8}else{$spec.maximum})}
        $box.Width=$(if($spec.type -eq 'number'){90}else{190});$box.Margin='0,0,6,0';$box.VerticalContentAlignment='Center'
        $type=[string]$spec.type
        $save=Button $global:LU_data.words.save ({ $value=if($type -eq 'password'){$box.Password}else{$box.Text};Save $key $value }.GetNewClosure())
        $editor.Children.Add($box)>$null;$editor.Children.Add($save)>$null
    }
    $editor.VerticalAlignment='Center';[System.Windows.Controls.Grid]::SetColumn($editor,1);$row.Children.Add($editor)>$null
    $card.Child=$row;return $card
}
function global:RenderMemory {
    if($global:LU_section -ne 6 -or $global:LU_Search.Text){return}
    try {
        $m=Rpc @{op='memory'}
        if(!$m.valid){return}
        $global:LU_Temperature.Text=$m.temperature
        $global:LU_MemorySummary.Text=$m.summary
        $global:LU_MemoryProgress.Value=$m.percent
        $global:LU_MemoryDetails.Text=$m.details
        $global:LU_DiskList.Children.Clear()
        foreach($disk in $m.volumes){
            $box=New-Object System.Windows.Controls.Border;$box.Padding='14';$box.Margin='0,0,0,6';$box.CornerRadius=7
            $box.SetResourceReference([System.Windows.Controls.Border]::BackgroundProperty,'Card')
            $stack=New-Object System.Windows.Controls.StackPanel
            $title=Text $disk.name;$title.FontWeight='SemiBold';$stack.Children.Add($title)>$null
            $bar=New-Object System.Windows.Controls.ProgressBar;$bar.Height=6;$bar.Maximum=100;$bar.Value=$disk.percent;$bar.Margin='0,9,0,7'
            $bar.Foreground=Brush $(if($disk.percent -ge 90){'#C94F45'}else{$global:LU_data.accent})
            $stack.Children.Add($bar)>$null;$stack.Children.Add((Text $disk.size 12))>$null
            $box.Child=$stack;$global:LU_DiskList.Children.Add($box)>$null
        }
        $global:LU_MemoryList.Children.Clear()
        foreach($app in $m.apps){
            $line=New-Object System.Windows.Controls.Grid;$line.Margin='0,7,0,7'
            $column=New-Object System.Windows.Controls.ColumnDefinition;$column.Width=New-Object System.Windows.GridLength(1,[System.Windows.GridUnitType]::Star);$line.ColumnDefinitions.Add($column)
            $column=New-Object System.Windows.Controls.ColumnDefinition;$column.Width=[System.Windows.GridLength]::Auto;$line.ColumnDefinitions.Add($column)
            $line.Children.Add((Text $app.name))>$null;$usage=Text $app.size;$usage.FontWeight='SemiBold';[System.Windows.Controls.Grid]::SetColumn($usage,1);$line.Children.Add($usage)>$null
            $global:LU_MemoryList.Children.Add($line)>$null
        }
    } catch {$global:LU_Status.Text=$_.Exception.Message}
}
function global:Build {
    $global:LU_building=$true
    try {
        Theme
        $global:LU_window.Title=$global:LU_data.title;$global:LU_SearchHint.Text=$global:LU_data.words.search
        $global:LU_NavColumn.Width=New-Object System.Windows.GridLength($(if($global:LU_sidebarCompact){54}else{212}))
        $global:LU_NavTitle.Visibility=$(if($global:LU_sidebarCompact){'Collapsed'}else{'Visible'})
        $global:LU_NavToggle.ToolTip=$global:LU_data.words.sidebar
        $global:LU_Navigation.Children.Clear();$global:LU_Body.Children.Clear()
        foreach($section in $global:LU_data.sections){
            $id=[int]$section.id
            $button=Button $(if($global:LU_sidebarCompact){$section.glyph}else{$section.glyph+'   '+$section.label}) ({ $global:LU_section=$id;$global:LU_Search.Text='';$null=Rpc @{op='watchMemory';on=($id -eq 6)};Build;$global:LU_ContentScroll.ScrollToTop() }.GetNewClosure())
            $button.ToolTip=$section.label;[System.Windows.Automation.AutomationProperties]::SetName($button,$section.label)
            $button.FontFamily='Segoe UI';$button.HorizontalContentAlignment='Left';$button.Margin='0,0,0,5';$button.MinHeight=44
            if($id -eq $global:LU_section -and !$global:LU_Search.Text){$button.Background=Brush $global:LU_data.accent;$button.Foreground=Brush $global:LU_data.accentText}
            $global:LU_Navigation.Children.Add($button)>$null
        }
        $query=$global:LU_Search.Text.Trim().ToLowerInvariant()
        $global:LU_Heading.Text=$(if($query){$global:LU_data.words.results}else{$global:LU_data.sections[$global:LU_section].label})
        $lastGroup=''
        foreach($spec in $global:LU_data.settings){
            if($query){if(($spec.label+' '+$spec.key+' '+$spec.group+' '+$spec.search).ToLowerInvariant().IndexOf($query) -lt 0){continue}}
            elseif([int]$spec.section -ne $global:LU_section){continue}
            if($spec.group -ne $lastGroup){$heading=Text $spec.group 14;$heading.FontWeight='SemiBold';$heading.Margin='2,20,0,10';$global:LU_Body.Children.Add($heading)>$null;$lastGroup=$spec.group}
            $global:LU_Body.Children.Add((Card $spec))>$null
        }
        if(!$query -and $global:LU_section -eq 0){
            $button=Button $global:LU_data.words.restore { $null=Rpc @{op='restore'} }
            $button.HorizontalAlignment='Left';$button.Margin='0,14,0,0';$global:LU_Body.Children.Add($button)>$null
        }
        if(!$query -and $global:LU_section -eq 6){
            $global:LU_Temperature=Text '' 18;$global:LU_Temperature.Margin='0,16,0,8';$global:LU_Body.Children.Add($global:LU_Temperature)>$null
            $global:LU_MemorySummary=Text '' 24;$global:LU_MemorySummary.Margin='0,20,0,12';$global:LU_Body.Children.Add($global:LU_MemorySummary)>$null
            $global:LU_MemoryProgress=New-Object System.Windows.Controls.ProgressBar;$global:LU_MemoryProgress.Height=8;$global:LU_MemoryProgress.Maximum=100;$global:LU_MemoryProgress.Foreground=Brush $global:LU_data.accent;$global:LU_Body.Children.Add($global:LU_MemoryProgress)>$null
            $global:LU_MemoryDetails=Text '' 12;$global:LU_MemoryDetails.Opacity=.7;$global:LU_MemoryDetails.Margin='0,12,0,14';$global:LU_Body.Children.Add($global:LU_MemoryDetails)>$null
            $buttons=New-Object System.Windows.Controls.StackPanel;$buttons.Orientation='Horizontal'
            $buttons.Children.Add((Button $global:LU_data.words.taskmgr {$null=Rpc @{op='taskmgr'}}))>$null
            $buttons.Children.Add((Button $global:LU_data.words.closeSettings {$global:LU_window.Close()}))>$null
            $global:LU_Body.Children.Add($buttons)>$null
            $heading=Text $global:LU_data.words.storage 16;$heading.FontWeight='SemiBold';$heading.Margin='0,24,0,12';$global:LU_Body.Children.Add($heading)>$null
            $global:LU_DiskList=New-Object System.Windows.Controls.StackPanel;$global:LU_Body.Children.Add($global:LU_DiskList)>$null
            $storageButton=Button $global:LU_data.words.storageSettings {$null=Rpc @{op='storageSettings'}};$storageButton.HorizontalAlignment='Left';$global:LU_Body.Children.Add($storageButton)>$null
            $heading=Text $global:LU_data.words.apps 16;$heading.FontWeight='SemiBold';$heading.Margin='0,24,0,12';$global:LU_Body.Children.Add($heading)>$null
            $global:LU_MemoryList=New-Object System.Windows.Controls.StackPanel;$global:LU_Body.Children.Add($global:LU_MemoryList)>$null
            RenderMemory
        }
        $global:LU_Status.Text=$global:LU_data.status
    } finally {$global:LU_building=$false}
}
$global:LU_NavToggle.Add_Click({
    $global:LU_sidebarCompact=!$global:LU_sidebarCompact
    try{$null=Rpc @{op='sidebar';compact=$global:LU_sidebarCompact}}catch{}
    Build
})
$global:LU_searchTimer=New-Object System.Windows.Threading.DispatcherTimer
$global:LU_searchTimer.Interval=[TimeSpan]::FromMilliseconds(200)
$global:LU_searchTimer.Add_Tick({$global:LU_searchTimer.Stop();Build})
$global:LU_Search.Add_TextChanged({$global:LU_SearchHint.Visibility=$(if($global:LU_Search.Text){'Collapsed'}else{'Visible'});if(!$global:LU_building){$global:LU_searchTimer.Stop();$global:LU_searchTimer.Start()}})
$global:LU_window.Add_SizeChanged({
    $global:LU_SearchFrame.Width=[Math]::Min(560,[Math]::Max(300,$global:LU_window.ActualWidth-450))
})
$global:LU_timer=New-Object System.Windows.Threading.DispatcherTimer
$global:LU_timer.Interval=[TimeSpan]::FromSeconds(5)
$global:LU_timer.Add_Tick({
    try {
        if($global:LU_window.WindowState -eq 'Minimized'){return}
        $v=Rpc @{op='version'}
        if($v.revision -ne $global:LU_data.revision -and !($global:LU_window.IsKeyboardFocusWithin -and ([System.Windows.Input.Keyboard]::FocusedElement -is [System.Windows.Controls.TextBox] -or [System.Windows.Input.Keyboard]::FocusedElement -is [System.Windows.Controls.PasswordBox]))){$global:LU_data=Rpc @{op='schema'};Build}
        RenderMemory
    } catch { $global:LU_timer.Stop();$global:LU_window.Close() }
})
$global:LU_window.Add_StateChanged({try{$null=Rpc @{op='watchMemory';on=($global:LU_section -eq 6 -and $global:LU_window.WindowState -ne 'Minimized')}}catch{}})
$global:LU_window.Add_ContentRendered({try{$null=Rpc @{op='ready'};Theme}catch{}})
$global:LU_window.Add_Closed({$global:LU_timer.Stop();$global:LU_searchTimer.Stop();try{$null=Rpc @{op='close'}}catch{};$global:LU_pipe.Dispose()})
Build
$global:LU_timer.Start()
$null=$global:LU_window.ShowDialog()
}


)V29PS";
DWORD WINAPI SettingsHostThread(void*) {
    HANDLE pipe=INVALID_HANDLE_VALUE, child=nullptr, inputRead=nullptr, inputWrite=nullptr, nullOutput=INVALID_HANDLE_VALUE, job=nullptr;
    bool apartment=false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded); apartment=true;
        GUID guid; winrt::check_hresult(CoCreateGuid(&guid));
        std::wstring pipeName=L"LiveUpdate.Settings."+std::wstring(winrt::to_hstring(guid));
        // Use the current user SID, not Everyone/Authenticated Users.
        HANDLE token=nullptr; winrt::check_bool(OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token));
        DWORD length=0; GetTokenInformation(token,TokenUser,nullptr,0,&length);
        std::vector<BYTE> user(length); BOOL got=GetTokenInformation(token,TokenUser,user.data(),length,&length); CloseHandle(token); winrt::check_bool(got);
        LPWSTR sid=nullptr; winrt::check_bool(ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(user.data())->User.Sid,&sid));
        std::wstring acl=L"D:P(A;;GA;;;SY)(A;;GA;;;"+std::wstring(sid)+L")"; LocalFree(sid);
        PSECURITY_DESCRIPTOR descriptor=nullptr; winrt::check_bool(ConvertStringSecurityDescriptorToSecurityDescriptorW(acl.c_str(),SDDL_REVISION_1,&descriptor,nullptr));
        SECURITY_ATTRIBUTES pipeSecurity{sizeof(pipeSecurity),descriptor,FALSE};
        pipe=CreateNamedPipeW((L"\\\\.\\pipe\\"+pipeName).c_str(),PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
            PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,65536,65536,0,&pipeSecurity);
        LocalFree(descriptor); winrt::check_bool(pipe!=INVALID_HANDLE_VALUE);
        SECURITY_ATTRIBUTES inherit{sizeof(inherit),nullptr,TRUE};
        winrt::check_bool(CreatePipe(&inputRead,&inputWrite,&inherit,65536)); SetHandleInformation(inputWrite,HANDLE_FLAG_INHERIT,0);
        nullOutput=CreateFileW(L"NUL",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,&inherit,OPEN_EXISTING,0,nullptr);
        winrt::check_bool(nullOutput!=INVALID_HANDLE_VALUE);
        STARTUPINFOEXW startup{}; startup.StartupInfo.cb=sizeof(startup);
        startup.StartupInfo.dwFlags=STARTF_USESTDHANDLES; startup.StartupInfo.hStdInput=inputRead;
        startup.StartupInfo.hStdOutput=nullOutput; startup.StartupInfo.hStdError=nullOutput;
        SIZE_T attributeBytes=0; InitializeProcThreadAttributeList(nullptr,1,0,&attributeBytes);
        std::vector<BYTE> attributes(attributeBytes); startup.lpAttributeList=reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(attributes.data());
        winrt::check_bool(InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&attributeBytes));
        HANDLE handles[]={inputRead,nullOutput};
        BOOL attributeOk=UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,handles,sizeof(handles),nullptr,nullptr);
        WCHAR directory[MAX_PATH]; UINT n=GetSystemDirectoryW(directory,ARRAYSIZE(directory));
        std::wstring executable=n && n<ARRAYSIZE(directory) ? std::wstring(directory)+L"\\WindowsPowerShell\\v1.0\\powershell.exe" : L"";
        std::wstring command=L"\""+executable+L"\" -NoLogo -NoProfile -NonInteractive -STA -WindowStyle Hidden -Command -";
        PROCESS_INFORMATION process{};
        BOOL launched=attributeOk && !executable.empty() && CreateProcessW(executable.c_str(),command.data(),nullptr,nullptr,TRUE,
            CREATE_NO_WINDOW|CREATE_SUSPENDED|EXTENDED_STARTUPINFO_PRESENT,nullptr,nullptr,&startup.StartupInfo,&process);
        DeleteProcThreadAttributeList(startup.lpAttributeList); winrt::check_bool(launched);
        child=process.hProcess; g_settingsHostPid=process.dwProcessId;
        job=CreateJobObjectW(nullptr,nullptr);
        if(job) { JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{}; limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
            if(!SetInformationJobObject(job,JobObjectExtendedLimitInformation,&limits,sizeof(limits)) || !AssignProcessToJobObject(job,child)) {CloseHandle(job);job=nullptr;} }
        ResumeThread(process.hThread); CloseHandle(process.hThread); CloseHandle(inputRead); inputRead=nullptr; CloseHandle(nullOutput); nullOutput=INVALID_HANDLE_VALUE;
        std::string script=kSettingsScript; auto marker=script.find("__PIPE__"); script.replace(marker,8,winrt::to_string(pipeName));
        DWORD sent=0; winrt::check_bool(WriteFile(inputWrite,script.data(),static_cast<DWORD>(script.size()),&sent,nullptr) && sent==script.size());
        CloseHandle(inputWrite); inputWrite=nullptr;
        OVERLAPPED connection{}; connection.hEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr); winrt::check_bool(connection.hEvent!=nullptr);
        BOOL connected=ConnectNamedPipe(pipe,&connection); DWORD error=GetLastError();
        if(!connected && error==ERROR_IO_PENDING) {
            HANDLE waits[]={g_liveStopEvent,child,connection.hEvent}; auto status=WaitForMultipleObjects(3,waits,FALSE,15000); DWORD bytes=0;
            if(status==WAIT_OBJECT_0+2) connected=GetOverlappedResult(pipe,&connection,&bytes,FALSE);
            else {CancelIoEx(pipe,&connection);GetOverlappedResult(pipe,&connection,&bytes,TRUE);}
        } else if(!connected && error==ERROR_PIPE_CONNECTED) connected=TRUE;
        CloseHandle(connection.hEvent); winrt::check_bool(connected);
        ULONG client=0; winrt::check_bool(GetNamedPipeClientProcessId(pipe,&client) && client==process.dwProcessId);
        std::string pending;
        while(!g_shuttingDown) {
            char bytes[4096]; DWORD received=0;
            if(!SettingsPipeIO(pipe,child,bytes,sizeof(bytes),false,received) || !received) break;
            pending.append(bytes,received); if(pending.size()>65536) break;
            auto newline=pending.find('\n'); if(newline==std::string::npos) continue;
            auto request=winrt::Windows::Data::Json::JsonObject::Parse(winrt::to_hstring(pending.substr(0,newline)));
            pending.erase(0,newline+1);
            auto response=winrt::to_string(SettingsRequest(request).Stringify())+"\n";
            DWORD written=0; if(!SettingsPipeIO(pipe,child,response.data(),static_cast<DWORD>(response.size()),true,written) || written!=response.size()) break;
            if(request.GetNamedString(L"op",L"")==L"close") break;
        }
    } catch (...) { if(!g_shuttingDown) Wh_Log(L"Settings host could not be started or disconnected"); }
    g_memoryDetailsRequested=false; g_settingsHostPid=0;
    if(pipe!=INVALID_HANDLE_VALUE){CancelIoEx(pipe,nullptr);DisconnectNamedPipe(pipe);CloseHandle(pipe);}
    if(inputRead)CloseHandle(inputRead); if(inputWrite)CloseHandle(inputWrite); if(nullOutput!=INVALID_HANDLE_VALUE)CloseHandle(nullOutput);
    if(job)CloseHandle(job); // Only this mod's settings child belongs to the job.
    if(child)CloseHandle(child); if(apartment)winrt::uninit_apartment(); return 0;
}
void ShowQuickSettings() {
    if(g_shuttingDown) return;
    HideExpandedPopupImmediately(g_expandedPopup);
    std::lock_guard<std::mutex> lock(g_settingsHostMutex);
    if(g_settingsHostThread && WaitForSingleObject(g_settingsHostThread,0)==WAIT_TIMEOUT) {
        DWORD pid=g_settingsHostPid.load();
        EnumWindows([](HWND window,LPARAM value)->BOOL { DWORD pid=0;GetWindowThreadProcessId(window,&pid);
            if(pid==static_cast<DWORD>(value) && IsWindowVisible(window)) {ShowWindowAsync(window,SW_RESTORE);SetForegroundWindow(window);return FALSE;} return TRUE;},pid);
        return;
    }
    if(g_settingsHostThread) CloseHandle(g_settingsHostThread);
    g_settingsHostThread=CreateThread(nullptr,0,SettingsHostThread,nullptr,0,nullptr);
}
void SetQuickOption(PCWSTR key,std::wstring const& value) {
    {std::lock_guard<std::mutex> lock(g_configStorageMutex);if(!Wh_SetStringValue((std::wstring(L"config.")+key).c_str(),value.c_str()))return;}
    LoadSettings();++g_settingsRevision;g_popupBackdropDirty=true;WakeSettingsProviders();
    RefreshTaskbarWidgetData();
}
void ShowTaskbarContextMenu(wf::Point point) {
    auto settings=GetSettings();bool vi=UseVietnameseLabels(settings);
    if(g_quickMenu)g_quickMenu.Hide();
    g_quickMenu=wuxc::MenuFlyout();
    wuxc::ToggleMenuFlyoutItem collapse;collapse.Text(PickText(vi,L"Thu gọn Live Update",L"Collapse Live Update"));collapse.IsChecked(settings.collapsedTaskbar);
    collapse.Click([](auto const& sender,auto const&){try{SetQuickOption(L"collapsedTaskbar",sender.template as<wuxc::ToggleMenuFlyoutItem>().IsChecked()?L"1":L"0");}catch(...){}});
    g_quickMenu.Items().Append(collapse);
    wuxc::MenuFlyoutSubItem themes;themes.Text(PickText(vi,L"Giao diện",L"Appearance"));
    for(auto const& choice:WidgetSettingOptions(L"popupTheme")) {
        wuxc::ToggleMenuFlyoutItem item;item.Text(vi?choice.vi:choice.en);item.IsChecked(settings.popupTheme==choice.value);
        std::wstring value=choice.value;item.Click([value](auto const&,auto const&){try{SetQuickOption(L"popupTheme",value);}catch(...){}});themes.Items().Append(item);
    }
    g_quickMenu.Items().Append(themes);g_quickMenu.Items().Append(wuxc::MenuFlyoutSeparator());
    wuxc::MenuFlyoutItem options;options.Text(PickText(vi,L"Cài đặt",L"Settings"));options.Click([](auto const&,auto const&){try{ShowQuickSettings();}catch(...){}});g_quickMenu.Items().Append(options);
    g_quickMenu.Opened([](auto const&,auto const&){g_quickMenuOpen=true;});
    g_quickMenu.Closed([](auto const&,auto const&){g_quickMenuOpen=false;});
    g_quickMenu.ShowAt(g_playerGrid,point);
}

void StopSettingsHost() {
    std::lock_guard<std::mutex> lock(g_settingsHostMutex);
    if(g_settingsHostThread) {CancelSynchronousIo(g_settingsHostThread);WaitForSingleObject(g_settingsHostThread,INFINITE);CloseHandle(g_settingsHostThread);g_settingsHostThread=nullptr;}
}
void RefreshSettingsWindow() { ++g_settingsRevision; }

std::vector<ActivityKind> CurrentActivities(ModSettings const& settings) {
    RefreshHiddenActivities();
    auto result = AvailableActivities(g_uiLive, settings, g_selectedActivity, g_popupMode != PopupMode::Hidden);
    bool enabled = g_systemNoticeActivity == ActivityKind::Display ? settings.enableDisplayWidget : settings.enableHotspotWidget;
    if (settings.enableTaskbarWidget && enabled && g_systemNoticeActivity != ActivityKind::None &&
        GetTickCount64() < g_systemNoticeUntil && std::find(result.begin(), result.end(), g_systemNoticeActivity) == result.end())
        result.push_back(g_systemNoticeActivity);
    if (settings.enableTaskbarWidget && settings.enableTaskProgress && HasTaskNotice(g_uiLive) && std::find(result.begin(), result.end(), ActivityKind::Download) == result.end())
        result.push_back(ActivityKind::Download);
    std::erase_if(result, [](auto kind) { return g_hiddenActivities.contains(kind); });
    return result;
}

void NormalizeSelectedActivity(ModSettings const& settings) {
    if (g_popupMode == PopupMode::Automatic) return;
    if (g_selectedActivity == ActivityKind::Culture && settings.enableLunarWidget && settings.enableCultureWidget) return;
    auto activities = CurrentActivities(settings);
    if (std::find(activities.begin(), activities.end(), g_selectedActivity) != activities.end()) return;
    if (std::find(activities.begin(), activities.end(), g_lastUserActivity) != activities.end())
        g_selectedActivity = g_lastUserActivity;
    else g_selectedActivity = activities.empty() ? ActivityKind::None : activities.front();
}

int g_activityDirection = 1;
void SelectActivity(ActivityKind kind) {
    if (g_shuttingDown) return;
    auto activities = CurrentActivities(GetSettings());
    auto old = std::find(activities.begin(), activities.end(), g_selectedActivity);
    auto next = std::find(activities.begin(), activities.end(), kind);
    if (old != activities.end() && next != activities.end() && old != next) g_activityDirection = next > old ? 1 : -1;
    PromotePopupToManual();
    if (kind != ActivityKind::Culture && g_culturePageOpen) ReleaseCulturePage();
    g_selectedActivity = kind;
    g_lastUserActivity = kind == ActivityKind::Culture ? ActivityKind::Lunar : kind; g_lastManualSwitch = GetTickCount64();
    g_showUsage = false;
    if (kind == ActivityKind::Battery) g_batteryNeedsAttention = false;
    RequestPopupRefresh();
}

void CycleActivity(int direction) {
    auto activities = CurrentActivities(GetSettings());
    if (g_selectedActivity == ActivityKind::Culture) g_selectedActivity = ActivityKind::Lunar;
    if (activities.size() < 2) return;
    auto found = std::find(activities.begin(), activities.end(), g_selectedActivity);
    int index = found == activities.end() ? 0 : static_cast<int>(found - activities.begin());
    index = (index + direction + static_cast<int>(activities.size())) % static_cast<int>(activities.size());
    SelectActivity(activities[index]);
    g_activityDirection = direction < 0 ? -1 : 1;
}

void QueueMediaAction(MediaAction action) {
    if (g_shuttingDown) return;
    auto requested = g_requestedMediaSession.load();
    if (requested && requested != g_uiLive.media.sessionId) return;
    PromotePopupToManual();
    {
        std::lock_guard<std::mutex> lock(g_liveMutex);
        if (g_mediaCommand.action != MediaAction::None) return;
        g_mediaCommand = {action, g_uiLive.media.generation};
    }
    if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
}

void RefreshActivityTabs(bool vi) {
    if (!g_popupTabs) return;
    auto activities = CurrentActivities(GetSettings());
    if (g_tabsHost) g_tabsHost.Visibility(wux::Visibility::Visible);
    if (g_popupTabsScroll) g_popupTabsScroll.Visibility(activities.size() > 1 ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    UpdateTabArrows();
    if (g_hideActivity) g_hideActivity.Content(winrt::box_value(PickText(vi, L"Ẩn", L"Hide")));
    if (g_collapseTaskbar) {
        auto text = GetSettings().collapsedTaskbar ? PickText(vi,L"Mở rộng widget taskbar",L"Expand taskbar widget") : PickText(vi,L"Thu gọn widget taskbar",L"Collapse taskbar widget");
        g_collapseTaskbar.Content(FluentIcon(GetSettings().collapsedTaskbar ? L"\uE740" : L"\uE73F",17));
        wuxc::ToolTipService::SetToolTip(g_collapseTaskbar,winrt::box_value(text));
        wux::Automation::AutomationProperties::SetName(g_collapseTaskbar,text);
    }
    if (g_quickSettings) { wuxc::ToolTipService::SetToolTip(g_quickSettings, winrt::box_value(PickText(vi, L"Cài đặt widget", L"Widget settings")));
        wux::Automation::AutomationProperties::SetName(g_quickSettings, PickText(vi, L"Cài đặt widget", L"Widget settings")); }
    if (activities == g_renderedTabs && g_renderedTabLanguage == static_cast<int>(vi) &&
        g_renderedSelectedTab == g_selectedActivity &&
        g_renderedBatteryAttention == g_batteryNeedsAttention) return;
    g_popupTabs.Children().Clear();
    for (auto kind : activities) {
        wuxc::Button button;
        auto label = ActivityLabel(kind, vi);
        if (kind == ActivityKind::Battery && g_batteryNeedsAttention) label += L" !";
        button.Content(winrt::box_value(winrt::hstring(label)));
        button.Padding({12, 5, 12, 5});
        button.MinWidth(76);
        button.CornerRadius({6, 6, 6, 6});
        if (kind == g_selectedActivity || (kind == ActivityKind::Lunar && g_selectedActivity == ActivityKind::Culture))
            StyleAccentButton(button);
        button.Click([kind](wf::IInspectable const&, wux::RoutedEventArgs const&) { SelectActivity(kind); });
        wux::Automation::AutomationProperties::SetName(button, label);
        g_popupTabs.Children().Append(button);
    }
    g_renderedTabs = std::move(activities);
    g_renderedTabLanguage = static_cast<int>(vi);
    g_renderedSelectedTab = g_selectedActivity;
    g_renderedBatteryAttention = g_batteryNeedsAttention;
}

void RefreshLivePopupPage(bool vi, bool light) {
    bool clockPage=g_selectedActivity==ActivityKind::Clock;
    if(g_clockList)g_clockList.Visibility(clockPage?wux::Visibility::Visible:wux::Visibility::Collapsed);
    const bool lunar = g_selectedActivity == ActivityKind::Lunar;
    if (g_obsControls) g_obsControls.Visibility(g_selectedActivity == ActivityKind::Obs ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    bool systemPage = g_selectedActivity == ActivityKind::Display || g_selectedActivity == ActivityKind::Hotspot;
    g_systemDetails.Visibility(systemPage ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_systemOptions.Visibility(systemPage ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_privacySelector.Visibility(wux::Visibility::Collapsed);
    g_privacyAppList.Visibility(g_selectedActivity == ActivityKind::Privacy ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_batteryTools.Visibility(g_selectedActivity == ActivityKind::Battery ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_popupZodiacText.FontFamily(wuxm::FontFamily(L"Segoe UI Emoji")); g_popupZodiacText.FontSize(24);
    g_popupZodiacText.Visibility(wux::Visibility::Visible);
    if (g_selectedActivity != ActivityKind::Media && g_selectedActivity != ActivityKind::Lunar)
        g_popupBadgeBorder.Background(wuxm::SolidColorBrush(GetFiveElementColor(FiveElement::Water, light)));
    if (g_popupCalendarNavigation)
        g_popupCalendarNavigation.Visibility(lunar || g_selectedActivity == ActivityKind::Culture ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (g_popupLiveScroll)
        g_popupLiveScroll.Visibility(lunar || g_selectedActivity == ActivityKind::Culture ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    RefreshActivityTabs(vi);
    if (g_monthButton) g_monthButton.Content(winrt::box_value(g_showingMonth ? PickText(vi, L"Ngày", L"Day") : PickText(vi, L"Tháng", L"Month")));
    if (g_cultureButton) {
        bool culture = g_selectedActivity == ActivityKind::Culture;
        auto nav = g_popupCalendarNavigation.try_as<wuxc::StackPanel>();
        if (nav) for (auto const& child : nav.Children()) child.Visibility(culture ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        g_cultureButton.Visibility(GetSettings().enableCultureWidget ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        g_cultureButton.Content(winrt::box_value(culture ? PickText(vi, L"← Quay lại", L"← Back") : PickText(vi, L"Văn hóa", L"Culture")));
    }
    RefreshMonthCalendar(vi, light);
    RefreshCulturePage(vi);
    if (lunar || !g_popupLiveTitle) return;
    if (g_popupHoroscopeScroll) g_popupHoroscopeScroll.Visibility(wux::Visibility::Collapsed);
    if (g_popupHolidayScroll) g_popupHolidayScroll.Visibility(wux::Visibility::Collapsed);
    auto foreground = wuxm::SolidColorBrush(light ? winrt::Windows::UI::Color{255, 20, 20, 24}
                                                : winrt::Windows::UI::Color{255, 245, 245, 248});
    g_popupLiveTitle.Foreground(foreground);
    g_popupLiveDetail.Foreground(foreground);
    g_popupLiveStatus.Foreground(foreground);
    g_popupTitleSolar.Text(ActivityLabel(g_selectedActivity, vi));
    if (g_selectedActivity == ActivityKind::Culture) return;
    const bool mediaPage = g_selectedActivity == ActivityKind::Media;
    g_popupMediaControls.Visibility(mediaPage ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_mediaTransport.Visibility(mediaPage ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_mediaSeek.Visibility(mediaPage ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_mediaSessionScroll.Visibility(mediaPage && g_uiLive.mediaSessions.size() > 1 ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    g_taskProgressList.Visibility(g_selectedActivity == ActivityKind::Download ? wux::Visibility::Visible : wux::Visibility::Collapsed);

    g_popupLiveTitle.FontSize(mediaPage ? 20.0 : 28.0);
    if (mediaPage) {
        auto const& m = g_uiLive.media;
        StyleAccentButton(g_popupMediaPlayPause);
        g_popupZodiacText.Text(L"♫");
        g_popupBadgeBorder.Background(wuxm::SolidColorBrush(GetFiveElementColor(FiveElement::Water, light)));
        g_popupTitleLunar.Text(m.available ? PickText(vi, m.playing ? L"Đang phát" : L"Tạm dừng",
            m.playing ? L"Playing" : L"Paused") : PickText(vi, L"Phiên phát đã kết thúc", L"Playback ended"));
        g_popupLiveTitle.Text(!m.available ? PickText(vi, L"Chưa có nội dung đang phát", L"No active playback")
            : m.title.empty() ? PickText(vi, L"Không có tiêu đề", L"Untitled media") : m.title);
        g_popupLiveDetail.Text(m.available ? m.artist : L"");
        std::wstring status;
        if (m.commandFailed) status += PickText(vi, L"\nTrình phát chưa nhận lệnh. Thử lại.", L"\nThe player did not accept the command. Retry.");
        g_popupLiveStatus.Text(status);
        g_popupLiveProgress.Visibility(wux::Visibility::Collapsed);
        g_mediaElapsed.Foreground(foreground); g_mediaDuration.Foreground(foreground);
        g_mediaElapsed.Text(FormatPlaybackTime(g_draggingSeek && g_seekGeneration == m.generation ? static_cast<int64_t>(g_mediaSeek.Value()) : m.positionSeconds));
        g_mediaDuration.Text(m.durationSeconds > 0 ? FormatPlaybackTime(m.durationSeconds) : L"—");
        if (g_seekGeneration != m.generation) { g_draggingSeek = false; g_seekQuietUntil = 0; }
        g_seekGeneration = m.generation;
        g_settingSeek = true;
        struct SeekGuard { ~SeekGuard() { g_settingSeek = false; } } seekGuard;
        g_mediaSeek.IsEnabled(m.available && m.canSeek);
        g_mediaSeek.Minimum(0);
        g_mediaSeek.Maximum(static_cast<double>(std::max<int64_t>(1, std::max(m.durationSeconds, m.seekMax))));
        g_mediaSeek.Minimum(static_cast<double>(std::max<int64_t>(0, m.seekMin)));
        if (!g_draggingSeek && GetTickCount64() >= g_seekQuietUntil) g_mediaSeek.Value(static_cast<double>(m.positionSeconds));
        g_settingSeek = false;
        wuxc::ToolTipService::SetToolTip(g_mediaSeek, winrt::box_value(PickText(vi,
            m.canSeek ? L"Kéo để tua" : L"Trình phát chưa hỗ trợ tua qua Windows",
            m.canSeek ? L"Drag to seek" : L"Seeking is unavailable through Windows")));
        RefreshMediaSessionButtons(vi);
        if (g_playPauseIcon) g_playPauseIcon.Glyph(m.playing ? L"\uE769" : L"\uE768");
        ApplyArtwork(g_popupBadgeBorder, g_popupZodiacText);
        g_popupMediaPlayPause.IsEnabled(m.available && m.canToggle);
        wux::Automation::AutomationProperties::SetName(g_popupMediaPlayPause, PickText(vi, L"Phát / tạm dừng", L"Play / pause"));
        g_popupMediaPrevious.IsEnabled(m.available && m.canPrevious);
        wux::Automation::AutomationProperties::SetName(g_popupMediaPrevious, PickText(vi, L"Trước", L"Previous track"));
        g_popupMediaNext.IsEnabled(m.available && m.canNext);
        wux::Automation::AutomationProperties::SetName(g_popupMediaNext, PickText(vi, L"Tiếp theo", L"Next track"));
        wuxc::ToolTipService::SetToolTip(g_popupMediaPrevious,
            winrt::box_value(PickText(vi, L"Trước", L"Previous track")));
        wuxc::ToolTipService::SetToolTip(g_popupMediaNext,
            winrt::box_value(PickText(vi, L"Tiếp theo", L"Next track")));
        wuxc::ToolTipService::SetToolTip(g_popupMediaPlayPause,
            winrt::box_value(PickText(vi, L"Phát / tạm dừng", L"Play / pause")));
    } else if (systemPage) {
        RefreshSystemPage(vi);
    } else if (g_selectedActivity == ActivityKind::Clock) {
        RefreshClockPopup(vi);
    } else if (g_selectedActivity == ActivityKind::Obs) {
        auto const& obs = g_uiLive.obs;
        g_popupZodiacText.Text(L"⏺"); g_popupTitleLunar.Text(L"OBS Studio");
        g_popupLiveTitle.Text(obs.recording ? FormatPlaybackTime(obs.elapsedSeconds) : PickText(vi, L"Không quay màn hình", L"Not recording"));
        g_popupLiveDetail.Text(!obs.connected ? PickText(vi, L"Chưa kết nối OBS", L"OBS disconnected") :
            obs.recording ? PickText(vi, obs.paused ? L"Đã tạm dừng quay" : L"Đang quay màn hình", obs.paused ? L"Recording paused" : L"Recording") : L"");
        g_popupLiveProgress.Visibility(wux::Visibility::Collapsed);
        g_popupLiveStatus.Text(obs.commandFailed ? PickText(vi, L"OBS chưa nhận lệnh hoặc chế độ quay không hỗ trợ tạm dừng.",
            L"OBS rejected the command, or pausing is unavailable with this recording configuration.") : L"");
        g_obsPause.Content(winrt::box_value(PickText(vi, obs.paused ? L"▶ Tiếp tục" : L"⏸ Tạm dừng", obs.paused ? L"▶ Resume" : L"⏸ Pause")));
        g_obsStop.Content(winrt::box_value(PickText(vi, L"■ Dừng quay", L"■ Stop recording")));
        g_obsPause.IsEnabled(obs.connected && obs.recording && !obs.commandPending);
        g_obsStop.IsEnabled(obs.connected && obs.recording && !obs.commandPending);
    } else if (g_selectedActivity == ActivityKind::Download) {
        g_popupZodiacText.Text(L"↧");
        g_popupTitleLunar.Text(std::any_of(g_uiLive.tasks.begin(),g_uiLive.tasks.end(),[](auto const& t){return OperationFromTask(t)==TaskOperation::Download;}) ?
            DownloadSummary(g_uiLive.tasks,vi) : std::to_wstring(g_uiLive.tasks.size()) + PickText(vi, L" tác vụ", L" tasks"));
        g_popupLiveTitle.Text(g_uiLive.tasks.empty() ? PickText(vi, L"Không có tác vụ đang chạy", L"No active tasks") : L"");
        g_popupLiveDetail.Text(L""); g_popupLiveProgress.Visibility(wux::Visibility::Collapsed);
        g_popupLiveStatus.Text(L"");
        RefreshTaskProgressPage(vi);
        if (HasTaskNotice(g_uiLive)) {
            g_popupLiveStatus.Text(g_uiLive.taskNotice.filename + (g_uiLive.taskNotice.filename.empty() ? L"" : L" · ") + TaskNoticeText(g_uiLive.taskNotice, vi));
            if (g_uiLive.tasks.empty()) g_popupLiveTitle.Text(L"");
        }
    } else if (g_selectedActivity == ActivityKind::Remote) {
        SetActivityGlyph(g_popupZodiacText,L"\uE7F4");
        g_popupTitleLunar.Text(RemoteConnectionText(g_uiLive.remote,vi));
        g_popupLiveTitle.Text(L""); g_popupLiveDetail.Text(RemotePeerText(g_uiLive.remote));
        g_popupLiveProgress.Visibility(wux::Visibility::Collapsed); g_popupLiveStatus.Text(L"");
    } else if (g_selectedActivity == ActivityKind::Memory) {
        auto const& m = g_uiLive.memory;
        SetActivityGlyph(g_popupZodiacText, L"\uE950");
        g_popupTitleLunar.Text(m.percent >= 95 ? PickText(vi,L"RAM gần đầy",L"Memory nearly full") : PickText(vi,L"Mức sử dụng RAM",L"Memory usage"));
        g_popupLiveTitle.Text(std::to_wstring(m.percent) + L"%");
        g_popupLiveDetail.Text(MemorySize(m.total-m.available) + L" / " + MemorySize(m.total));
        UpdateProgressView(g_popupLiveProgress,m.percent);
        g_popupLiveProgress.Visibility(wux::Visibility::Visible);
        g_popupLiveStatus.Text(TemperatureText(g_uiLive.temperature,vi)+L"\n"+MemoryTopText(m));
    } else if (g_selectedActivity == ActivityKind::Privacy) {
        PrivacyGlyph(g_popupZodiacText);
        g_popupTitleLunar.Text(L"");
        g_popupLiveProgress.Visibility(wux::Visibility::Collapsed);
        RefreshPrivacyPage(vi);
    } else {
        auto const& b = g_uiLive.battery;
        g_popupZodiacText.Text(b.charging ? L"⚡" : L"🔋");
        g_popupBadgeBorder.Background(wuxm::SolidColorBrush(GetFiveElementColor(
            b.percent >= 0 && b.percent <= 20 && !b.pluggedIn ? FiveElement::Fire : FiveElement::Wood, light)));
        g_popupTitleLunar.Text(b.present ? BatteryHeadline(b, vi) : L"Bluetooth");
        g_popupLiveTitle.Text(b.valid && b.present && b.percent >= 0 ? std::to_wstring(b.percent) + L"%" : L"");
        g_popupLiveDetail.Text(b.present ? BatteryDetail(b, vi) : L"");
        std::wstring status;
        if (b.valid && b.present && !b.pluggedIn && b.percent >= 0 && b.percent <= 20)
            status = PickText(vi, L"Pin yếu. Hãy kết nối bộ sạc.", L"Battery is low. Connect your charger.");
        else if (b.present && b.pluggedIn && !b.charging && b.percent < 100)
            status = PickText(vi, L"Bộ điều khiển pin hiện không báo đang sạc.", L"The battery controller is not reporting active charging.");
        g_popupLiveStatus.Text(status);
        RefreshBatteryTools(vi);
        UpdateProgressView(g_popupLiveProgress, b.percent);
        g_popupLiveProgress.Visibility(!g_showUsage && b.valid && b.present && b.percent >= 0
            ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        if (g_showUsage) {
            g_popupTitleSolar.Text(PickText(vi, L"Thời gian sử dụng ứng dụng", L"App usage time"));
            g_popupLiveTitle.Text(L""); g_popupLiveDetail.Text(L""); g_popupLiveStatus.Text(L"");
        }
    }
}

void ReleaseLivePopupControls() {
    ReleaseExtendedControls();
    g_popupCalendarNavigation = nullptr;
    g_popupTabs = nullptr;
    g_popupLiveScroll = nullptr;
    g_popupLiveTitle = nullptr;
    g_popupLiveDetail = nullptr;
    g_popupLiveStatus = nullptr;
    g_popupLiveProgress = nullptr;
    g_popupMediaControls = nullptr;
    g_popupMediaPrevious = nullptr;
    g_popupMediaPlayPause = nullptr;
    g_popupMediaNext = nullptr;
    g_renderedTabs.clear();
    g_renderedTabLanguage = -1;
}

double PopupDpiScale(HWND hwnd) {
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 96;
    if (dpi == 0) dpi = 96;
    return static_cast<double>(dpi) / 96.0;
}

int PopupDipToPx(double dip, HWND hwnd) {
    return static_cast<int>(std::lround(dip * PopupDpiScale(hwnd)));
}

bool TaskbarPopupOpensDown(RECT const& taskbar, MONITORINFO const& mi) {
    int monitorMidY = (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
    int taskbarMidY = (taskbar.top + taskbar.bottom) / 2;
    return taskbarMidY < monitorMidY;
}

winrt::Windows::UI::Color GetFiveElementColor(FiveElement elem, bool isLightTheme) {
    switch (elem) {
        case FiveElement::Metal:
            return isLightTheme ? winrt::Windows::UI::Color{65, 200, 150, 20}
                                : winrt::Windows::UI::Color{85, 225, 180, 50};
        case FiveElement::Wood:
            return isLightTheme ? winrt::Windows::UI::Color{60, 35, 160, 85}
                                : winrt::Windows::UI::Color{85, 50, 190, 105};
        case FiveElement::Water:
            return isLightTheme ? winrt::Windows::UI::Color{60, 35, 125, 215}
                                : winrt::Windows::UI::Color{85, 55, 150, 240};
        case FiveElement::Fire:
            return isLightTheme ? winrt::Windows::UI::Color{60, 215, 65, 55}
                                : winrt::Windows::UI::Color{85, 235, 85, 70};
        case FiveElement::Earth:
        default:
            return isLightTheme ? winrt::Windows::UI::Color{65, 175, 115, 55}
                                : winrt::Windows::UI::Color{85, 195, 135, 75};
    }
}

bool IsElementLightTheme(wux::FrameworkElement const& fe) {
    if (fe) {
        try {
            return fe.ActualTheme() == wux::ElementTheme::Light;
        } catch (...) {}
    }
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme",
                     RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

void UpdateWidgetHoverVisual() {
    if(!g_playerGrid)return;
    try {
        bool open=g_expandedPopup && IsWindowVisible(g_expandedPopup);
        UpdateWidgetMaterial(IsElementLightTheme(g_playerGrid),open);
        ApplyGlassButtons(g_playerGrid);
    }catch(...){}
}

void RefreshExpandedPopupData();

bool g_popupContrastValid=false,g_popupContrastLight=false;
uint64_t g_popupContrastSampled=0;
double LinearSrgb(unsigned component) {double v=component/255.0;return v<=.04045?v/12.92:std::pow((v+.055)/1.055,2.4);}
bool PopupLightFromLuminance(double value,bool valid,bool previous) {
    return value>(valid ? (previous ? .14 : .23) : .179);
}
bool SamplePopupContrast(bool force=false) {
    if(!g_expandedPopup || !UseGlassSurfaces())return false;
    auto now=GetTickCount64();if(!force && now-g_popupContrastSampled<2000)return false;g_popupContrastSampled=now;
    RECT rect{};if(!GetWindowRect(g_expandedPopup,&rect)||rect.right<=rect.left||rect.bottom<=rect.top)return false;
    HDC screen=GetDC(nullptr);if(!screen)return false;HDC memory=CreateCompatibleDC(screen);
    BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=16;info.bmiHeader.biHeight=-16;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
    void* pixels=nullptr;HBITMAP bitmap=CreateDIBSection(screen,&info,DIB_RGB_COLORS,&pixels,nullptr,0);
    bool changed=false;
    if(memory && bitmap && pixels) {
        auto old=SelectObject(memory,bitmap);SetStretchBltMode(memory,HALFTONE);
        if(StretchBlt(memory,0,0,16,16,screen,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,SRCCOPY)) {
            GdiFlush();std::array<double,256> values;auto data=static_cast<uint32_t*>(pixels);
            for(size_t i=0;i<values.size();++i)values[i]=.2126*LinearSrgb((data[i]>>16)&255)+.7152*LinearSrgb((data[i]>>8)&255)+.0722*LinearSrgb(data[i]&255);
            std::nth_element(values.begin(),values.begin()+128,values.end());
            bool light=PopupLightFromLuminance(values[128],g_popupContrastValid,g_popupContrastLight);
            changed=!g_popupContrastValid || light!=g_popupContrastLight;g_popupContrastLight=light;g_popupContrastValid=true;
        }
        SelectObject(memory,old);
    }
    if(bitmap)DeleteObject(bitmap);if(memory)DeleteDC(memory);ReleaseDC(nullptr,screen);return changed;
}

void AnimateContentChange(wux::UIElement const& element, int direction = 0);
constexpr UINT_PTR kObsClockTimerId = 0x202604;
bool g_obsClockTimer = false;
void RefreshObsClock() {
    if (g_shuttingDown || g_selectedActivity != ActivityKind::Obs) return;
    ObsSnapshot obs;
    { std::lock_guard<std::mutex> lock(g_liveMutex); obs = g_obsSnapshot; }
    if (!obs.recording) return;
    bool vi = UseVietnameseLabels(GetSettings());
    auto elapsed = FormatPlaybackTime(ObsElapsedMilliseconds(obs, GetTickCount64()) / 1000);
    auto text = elapsed + PickText(vi, obs.paused ? L" · Tạm dừng" : L" · Đang quay",
        obs.paused ? L" · Paused" : L" · Recording");
    if (g_widgetLine2Text && g_widgetLine2Text.Text() != text) {
        g_widgetLine2Text.Text(text);
        if (g_playerGrid && g_widgetLiveTitle) wuxc::ToolTipService::SetToolTip(g_playerGrid,
            winrt::box_value(std::wstring(g_widgetLiveTitle.Text()) + L"\n" + text));
    }
    if (g_popupLiveTitle && g_expandedPopup && IsWindowVisible(g_expandedPopup) &&
        g_popupLiveTitle.Text() != elapsed) g_popupLiveTitle.Text(elapsed);
}
void UpdateObsClockTimer() {
    HWND window = g_liveUiWindow.load();
    bool run = window && g_selectedActivity == ActivityKind::Obs && g_uiLive.obs.connected &&
        g_uiLive.obs.recording && !g_uiLive.obs.paused;
    if (run && !g_obsClockTimer) g_obsClockTimer = SetTimer(window, kObsClockTimerId, 100, nullptr) != 0;
    else if (!run && g_obsClockTimer) { if (window) KillTimer(window, kObsClockTimerId); g_obsClockTimer = false; }
}
void RefreshTaskbarWidgetData() {
    if (!g_playerGrid) return;
    try {
        ModSettings settings = GetSettings();
        bool vi = UseVietnameseLabels(settings);
        bool light = IsElementLightTheme(g_playerGrid);
        g_uiLive = ReadLiveSnapshot();
        ObserveActivitySelection(settings);
        NormalizeSelectedActivity(settings);
        if (g_popupMode == PopupMode::Automatic &&
            ((g_selectedActivity == ActivityKind::Download && g_uiLive.tasks.empty() && !HasTaskNotice(g_uiLive)) ||
             (g_selectedActivity == ActivityKind::Privacy && g_uiLive.privacy.uses.empty()))) {
            BeginCloseExpandedPopup(g_expandedPopup);
            return;
        }

        SYSTEMTIME st{};
        GetLocalTime(&st);
        auto const& h = CachedHoroscope(st, vi);

        bool settingsOnly = g_selectedActivity == ActivityKind::None;
        bool collapsed = settings.collapsedTaskbar && !settingsOnly;
        g_playerGrid.Width(settingsOnly ? 42 : collapsed ? 140 : static_cast<double>(settings.widgetWidth));
        g_playerGrid.MaxWidth(settingsOnly ? 42 : collapsed ? 140 : static_cast<double>(settings.widgetWidth));
        if (g_widgetBadgeBorder) g_widgetBadgeBorder.Margin({0, 0, settingsOnly ? 0.0 : 8.0, 0});
        if (g_widgetLine2Text) g_widgetLine2Text.Visibility(settingsOnly || collapsed ? wux::Visibility::Collapsed : wux::Visibility::Visible);
        if (settings.position == L"root_left") {
            g_playerGrid.Margin({static_cast<double>(settings.widgetLeftOffset), 0.0, 0.0, 0.0});
        } else {
            g_playerGrid.Margin({4.0, 0.0, 4.0, 0.0});
        }

        if (g_widgetBadgeBorder) {
            g_widgetBadgeBorder.Background(wuxm::SolidColorBrush(GetFiveElementColor(h.element, light)));
        }
        if (g_widgetZodiacText) {
            g_widgetZodiacText.Text(h.zodiacEmoji);
        }

        std::wstring line1 = FormatWidgetLunarHeader(h, vi);
        bool isHoliday = !h.specialHoliday.empty();
        std::wstring line2 = SelectWidgetSecondLine(h, vi, GetTickCount64(), g_widgetRotation);
        auto activities = CurrentActivities(settings);
        bool showNavigation = activities.size() > 1 && !collapsed;
        auto navigationWidth = wux::GridLengthHelper::FromValueAndType(showNavigation ? 22 : 0, wux::GridUnitType::Pixel);
        if (g_widgetPreviousColumn) g_widgetPreviousColumn.Width(navigationWidth);
        if (g_widgetNextColumn) g_widgetNextColumn.Width(navigationWidth);
        for (auto const& button : {g_widgetPreviousButton, g_widgetNextButton}) {
            if (!button) continue;
            button.Visibility(showNavigation ? wux::Visibility::Visible : wux::Visibility::Collapsed);
            button.IsHitTestVisible(showNavigation);
            button.IsEnabled(showNavigation);
            button.IsTabStop(showNavigation);
        }
        if (g_widgetPreviousButton) wuxc::ToolTipService::SetToolTip(g_widgetPreviousButton,
            winrt::box_value(PickText(vi, L"Widget trước", L"Previous widget")));
        if (g_widgetNextButton) wuxc::ToolTipService::SetToolTip(g_widgetNextButton,
            winrt::box_value(PickText(vi, L"Widget tiếp theo", L"Next widget")));
        g_widgetZodiacText.FontFamily(wuxm::FontFamily(L"Segoe UI Emoji")); g_widgetZodiacText.FontSize(22);
        if (g_selectedActivity == ActivityKind::Media) {
            auto const& m = g_uiLive.media;
            isHoliday = false;
            line1 = m.available ? (m.title.empty() ? PickText(vi, L"Đang phát media", L"Media playback") : m.title)
                                : PickText(vi, L"Phiên phát đã kết thúc", L"Playback ended");
            line2 = m.available ? FormatPlaybackTime(m.positionSeconds) : L"";
            if (m.available && m.durationSeconds > 0) line2 += L" / " + FormatPlaybackTime(m.durationSeconds);
            if (!m.artist.empty()) line2 += L" · " + m.artist;
            g_widgetZodiacText.Text(L"♫");
            g_widgetBadgeBorder.Background(wuxm::SolidColorBrush(GetFiveElementColor(FiveElement::Water, light)));
        } else if(g_selectedActivity==ActivityKind::Clock) {
            isHoliday=false;
            auto entry=PrimaryClockEntry(g_uiLive.clock);
            g_widgetZodiacText.Text(entry && entry->kind==ClockKind::Alarm?L"⏰":entry && entry->kind==ClockKind::Timer?L"⏳":L"⏱");
            line1=entry?ClockKindLabel(entry->kind,vi)+(entry->label.empty()?L"":L" · "+entry->label):ActivityLabel(ActivityKind::Clock,vi);
            line2=entry?ClockValueText(*entry)+L" · "+ClockEntryStatus(*entry,vi):ClockNoticeText(g_uiLive.clock,vi);
        } else if (g_selectedActivity == ActivityKind::Obs) {
            auto const& obs = g_uiLive.obs;
            isHoliday = false; g_widgetZodiacText.Text(L"⏺");
            line1 = PickText(vi, L"Quay màn hình · OBS", L"Screen recording · OBS");
            line2 = obs.recording ? FormatPlaybackTime(obs.elapsedSeconds) + PickText(vi,
                obs.paused ? L" · Tạm dừng" : L" · Đang quay", obs.paused ? L" · Paused" : L" · Recording") :
                PickText(vi, L"Đã kết thúc quay", L"Recording ended");
        } else if (g_selectedActivity == ActivityKind::Display) {
            isHoliday = false; SetActivityGlyph(g_widgetZodiacText, L"\uE7F4");
            line1 = DisplayModeText(g_uiLive.display, vi); line2 = DisplayNames(g_uiLive.display, vi);
        } else if (g_selectedActivity == ActivityKind::Remote) {
            line1=g_uiLive.remote.active && !g_uiLive.remote.peers.empty() ? g_uiLive.remote.peers.front() : L"UltraViewer";line2=RemoteConnectionText(g_uiLive.remote,vi);
            SetActivityGlyph(g_widgetZodiacText,L"\uE7F4");
        } else if (g_selectedActivity == ActivityKind::Memory) {
            isHoliday = g_uiLive.memory.percent >= 90; SetActivityGlyph(g_widgetZodiacText,L"\uE950");
            line1 = L"RAM · " + std::to_wstring(g_uiLive.memory.percent) + L"%";
            line2 = MemorySize(g_uiLive.memory.total-g_uiLive.memory.available) + L" / " + MemorySize(g_uiLive.memory.total);
            if(std::max(g_uiLive.temperature.cpu,g_uiLive.temperature.gpu)>=70) {line1=PickText(vi,L"Nhiệt độ cao",L"High temperature");line2=TemperatureText(g_uiLive.temperature,vi);}
        } else if (g_selectedActivity == ActivityKind::Hotspot) {
            isHoliday = false; SetActivityGlyph(g_widgetZodiacText, L"\uEC15");
            line1 = PickText(vi, L"Điểm phát sóng Wi-Fi", L"Wi-Fi hotspot");
            line2 = HotspotStateText(g_uiLive.hotspot, vi);
        } else if (g_selectedActivity == ActivityKind::Download) {
            isHoliday = false; g_widgetZodiacText.Text(L"↧");
            line1 = g_uiLive.tasks.empty() ? PickText(vi, L"Không có tác vụ", L"No active tasks") : (OperationFromTask(g_uiLive.tasks.front()) == TaskOperation::Download ? DownloadSummary(g_uiLive.tasks,vi) : TaskDisplayName(g_uiLive.tasks.front(), vi));
            line2 = g_uiLive.tasks.empty() ? L"" : TaskProgressText(g_uiLive.tasks.front(), vi);
            if (g_uiLive.tasks.size() > 1) line1 += L" +" + std::to_wstring(g_uiLive.tasks.size() - 1);
            if (g_uiLive.tasks.empty() && HasTaskNotice(g_uiLive)) {
                line1 = g_uiLive.taskNotice.filename.empty() ? PickText(vi, L"Tác vụ", L"Task") : g_uiLive.taskNotice.filename;
                line2 = TaskNoticeText(g_uiLive.taskNotice, vi);
            }
        } else if (g_selectedActivity == ActivityKind::Privacy) {
            isHoliday = false; line1 = PrivacyTitle(g_uiLive.privacy, vi); line2 = PrivacyApps(g_uiLive.privacy);
            PrivacyGlyph(g_widgetZodiacText);
        } else if (g_selectedActivity == ActivityKind::Battery) {
            auto const& b = g_uiLive.battery;
            isHoliday = b.valid && !b.pluggedIn && b.percent >= 0 && b.percent <= 20;
            line1 = BatteryHeadline(b, vi);
            line2 = BatteryDetail(b, vi);
            if ((!b.present || (!g_bluetoothNotice.id.empty() && GetTickCount64() < g_bluetoothNoticeUntil)) && !g_uiLive.bluetooth.devices.empty()) {
                auto const& device = !g_bluetoothNotice.id.empty() && GetTickCount64() < g_bluetoothNoticeUntil ? g_bluetoothNotice : g_uiLive.bluetooth.devices.front();
                line1 = device.name; line2 = device.percent >= 0 ? std::to_wstring(device.percent) + L"% · Bluetooth" : L"Bluetooth";
            }
            g_widgetZodiacText.Text(b.charging ? L"⚡" : L"🔋");
            g_widgetBadgeBorder.Background(wuxm::SolidColorBrush(GetFiveElementColor(
                isHoliday ? FiveElement::Fire : FiveElement::Wood, light)));
        }


        winrt::Windows::UI::Color primaryCol = light
            ? winrt::Windows::UI::Color{255, 22, 22, 24}
            : winrt::Windows::UI::Color{255, 245, 245, 248};
        winrt::Windows::UI::Color secondaryCol;
        if (isHoliday) {
            secondaryCol = light
                ? winrt::Windows::UI::Color{255, 196, 43, 28}
                : winrt::Windows::UI::Color{255, 255, 150, 135};
        } else {
            secondaryCol = light
                ? winrt::Windows::UI::Color{225, 65, 65, 70}
                : winrt::Windows::UI::Color{215, 205, 205, 212};
        }

        if (settingsOnly) {
            isHoliday = false; line1 = L"Live Update"; line2 = PickText(vi, L"Cài đặt widget", L"Widget settings");
            SetActivityGlyph(g_widgetZodiacText, L"\uE713");
        }
        if (collapsed) {
            auto count = AvailableActivities(g_uiLive, settings, ActivityKind::None, false).size();
            line1 = std::to_wstring(count) + (vi ? L" hoạt động" : count==1 ? L" activity" : L" activities"); line2.clear();
            SetActivityGlyph(g_widgetZodiacText,L"\uE9D9");
            g_widgetBadgeBorder.Background(wuxm::SolidColorBrush({30,127,127,127}));
        }
        bool lunarHeader = !collapsed && (g_selectedActivity == ActivityKind::Lunar || g_selectedActivity == ActivityKind::Culture);
        if (g_widgetLunarHeaderView) g_widgetLunarHeaderView.Visibility(lunarHeader ? wux::Visibility::Visible : wux::Visibility::Collapsed);
        if (g_widgetLiveTitle) {
            g_widgetLiveTitle.Visibility(lunarHeader || settingsOnly ? wux::Visibility::Collapsed : wux::Visibility::Visible);
            g_widgetLiveTitle.Text(line1);
            g_widgetLiveTitle.Foreground(wuxm::SolidColorBrush(primaryCol));
        }
        bool linesChanged = false;
        if (g_widgetLine1Text) {
            g_widgetLine1Text.Visibility(settingsOnly ? wux::Visibility::Collapsed : wux::Visibility::Visible);
            if (std::wstring_view(g_widgetLine1Text.Text()) != std::wstring_view(line1)) {
                g_widgetLine1Text.Text(line1);
                linesChanged = true;
            }
            g_widgetLine1Text.Foreground(wuxm::SolidColorBrush(primaryCol));
        }
        if (g_widgetLine2Text) {
            if (std::wstring_view(g_widgetLine2Text.Text()) != std::wstring_view(line2)) {
                g_widgetLine2Text.Text(line2);
                linesChanged = true;
            }
            g_widgetLine2Text.Foreground(wuxm::SolidColorBrush(secondaryCol));
        }
        if (linesChanged) {
            // Long names remain available in full without increasing taskbar width.
            wuxc::ToolTipService::SetToolTip(g_playerGrid,
                winrt::box_value(winrt::hstring{line1 + L"\n" + line2}));
        }

        static ActivityKind renderedActivity = ActivityKind::None;
        if (!collapsed && renderedActivity != ActivityKind::None && renderedActivity != g_selectedActivity)
            for (auto const& element : {g_widgetBadgeBorder.as<wux::UIElement>(), g_widgetLine2Text.as<wux::UIElement>(),
                    g_widgetLunarHeaderView.as<wux::UIElement>(), g_widgetLiveTitle.as<wux::UIElement>()})
                AnimateContentChange(element, g_activityDirection);
        renderedActivity = g_selectedActivity;
        UpdateObsClockTimer();
        if (!collapsed) ApplyArtwork(g_widgetBadgeBorder, g_widgetZodiacText);
        else g_widgetZodiacText.Visibility(wux::Visibility::Visible);
        UpdateWidgetHoverVisual();
        if (g_expandedPopup && IsWindowVisible(g_expandedPopup)) RefreshExpandedPopupData();
    } catch (...) {}
}

// 4. Screen geometry & positioning for the Expanded Horoscope Popup
bool GetElementScreenRect(wux::FrameworkElement const& element, RECT& result) {
    if (!element || !g_taskbarWnd) {
        return false;
    }

    try {
        HWND host=g_taskbarWnd;
        if(!host || !IsWindow(host))return false;
        POINT origin{};ClientToScreen(host,&origin);
        auto transform=element.TransformToVisual(nullptr);
        auto bounds=transform.TransformBounds({0,0,static_cast<float>(element.ActualWidth()),static_cast<float>(element.ActualHeight())});
        double scale=PopupDpiScale(host);
        result.left=origin.x+static_cast<int>(std::lround(bounds.X*scale));
        result.top=origin.y+static_cast<int>(std::lround(bounds.Y*scale));
        result.right=result.left+static_cast<int>(std::lround(bounds.Width*scale));
        result.bottom=result.top+static_cast<int>(std::lround(bounds.Height*scale));
        return result.right > result.left && result.bottom > result.top;
    } catch (...) {
        return false;
    }
}

void ApplyPopupPresentation(bool vi) {
    bool compact = g_popupMode == PopupMode::Automatic;
    if (g_popupTitleLunar) g_popupTitleLunar.Visibility(g_popupTitleLunar.Text().empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    for (auto label : {g_popupLiveTitle, g_popupLiveDetail, g_popupLiveStatus}) if (label)
        label.Visibility(label.Text().empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    if (g_popupLiveScroll) {
        auto panel = g_popupLiveScroll.Content().try_as<wuxc::StackPanel>();
        if (panel) for (auto const& child : panel.Children()) {
            auto element = child.try_as<wux::FrameworkElement>();
            if (element && element != g_mediaSeek) element.Margin({0, 0, 0, 6});
        }
    }
    g_compactNotice.Visibility(compact ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    bool batteryOpen = !compact && g_selectedActivity == ActivityKind::Battery && g_popupMode != PopupMode::Hidden;
    if (g_batteryPageOpen.exchange(batteryOpen) != batteryOpen && batteryOpen) {
        g_forcePowerRefresh = true; if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
    }
    if (g_hideActivity) g_hideActivity.Visibility(compact ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    if (!compact) return;
    for (auto scroll : {g_popupHoroscopeScroll, g_popupHolidayScroll, g_popupLiveScroll, g_monthScroll, g_cultureScroll})
        if (scroll) scroll.Visibility(wux::Visibility::Collapsed);
    g_popupCalendarNavigation.Visibility(wux::Visibility::Collapsed);
    g_tabsHost.Visibility(wux::Visibility::Collapsed);
    std::wstring message;
    if (g_selectedActivity == ActivityKind::Privacy) {
        g_popupTitleSolar.Text(PrivacyTitle(g_uiLive.privacy, vi));
        g_popupTitleLunar.Text(L"");
        message = PrivacyApps(g_uiLive.privacy);
    } else if (g_selectedActivity == ActivityKind::Battery) {
        g_popupTitleSolar.Text(BatteryHeadline(g_uiLive.battery, vi));
        g_popupTitleLunar.Text(BatteryDetail(g_uiLive.battery, vi));
        if (!g_bluetoothNotice.id.empty() && GetTickCount64() < g_bluetoothNoticeUntil) {
            g_popupTitleSolar.Text(g_bluetoothNotice.name);
            g_popupTitleLunar.Text(g_bluetoothNotice.percent >= 0 ? std::to_wstring(g_bluetoothNotice.percent) + L"%" : L"Bluetooth");
            message = PickText(vi, g_bluetoothNotice.percent >= 0 && g_bluetoothNotice.percent <= 20 ? L"Pin Bluetooth yếu" : L"Đã kết nối Bluetooth",
                g_bluetoothNotice.percent >= 0 && g_bluetoothNotice.percent <= 20 ? L"Bluetooth battery is low" : L"Bluetooth connected");
        } else message = g_batteryNeedsAttention ? PickText(vi, L"Pin yếu — hãy kết nối bộ sạc.", L"Low battery — connect your charger.") : L"";
    } else if (g_selectedActivity == ActivityKind::Download) {
        g_popupTitleSolar.Text(g_uiLive.tasks.empty() ? ActivityLabel(g_selectedActivity, vi) : OperationFromTask(g_uiLive.tasks.front())==TaskOperation::Download ? DownloadSummary(g_uiLive.tasks,vi) : TaskDisplayName(g_uiLive.tasks.front(), vi));
        g_popupTitleLunar.Text(g_uiLive.tasks.empty() ? L"" : TaskProgressText(g_uiLive.tasks.front(), vi));
        message = L"";
        if (HasTaskNotice(g_uiLive)) {
            g_popupTitleSolar.Text(g_uiLive.taskNotice.filename.empty() ? ActivityLabel(g_selectedActivity, vi) : g_uiLive.taskNotice.filename);
            g_popupTitleLunar.Text(TaskNoticeText(g_uiLive.taskNotice, vi));
        }
    }
    if(g_selectedActivity==ActivityKind::Clock) {
        g_popupTitleSolar.Text(ClockNoticeText(g_uiLive.clock,vi));
        auto entry=PrimaryClockEntry(g_uiLive.clock);
        g_popupTitleLunar.Text(entry?ClockValueText(*entry):L"Windows Clock");
    } else if (g_selectedActivity == ActivityKind::Obs) {
        g_popupTitleSolar.Text(PickText(vi, g_uiLive.obs.recording ? L"Đang quay màn hình" : L"Đã dừng quay màn hình", g_uiLive.obs.recording ? L"Screen recording" : L"Screen recording stopped"));
        g_popupTitleLunar.Text(L"OBS Studio");
    } else if (g_selectedActivity == ActivityKind::Remote) {
        g_popupTitleSolar.Text(L"UltraViewer");
        g_popupTitleLunar.Text(RemoteConnectionText(g_uiLive.remote,vi));
        message=RemotePeerText(g_uiLive.remote);
    } else if (g_selectedActivity == ActivityKind::Memory) {
        g_popupTitleSolar.Text(g_uiLive.memory.percent >= 95 ? PickText(vi,L"RAM gần đầy",L"Memory nearly full") : PickText(vi,L"RAM đang sử dụng cao",L"High memory usage"));
        g_popupTitleLunar.Text(std::to_wstring(g_uiLive.memory.percent)+L"% · "+MemorySize(g_uiLive.memory.total-g_uiLive.memory.available)+L" / "+MemorySize(g_uiLive.memory.total));
        if(std::max(g_uiLive.temperature.cpu,g_uiLive.temperature.gpu)>=70) {
            g_popupTitleSolar.Text(PickText(vi,L"Nhiệt độ thiết bị cao",L"High device temperature"));
            g_popupTitleLunar.Text(TemperatureText(g_uiLive.temperature,vi));
        }
        message = MemoryTopText(g_uiLive.memory);
    } else if (g_selectedActivity == ActivityKind::Display) {
        g_popupTitleSolar.Text(DisplayModeText(g_uiLive.display, vi));
        g_popupTitleLunar.Text(DisplayNames(g_uiLive.display, vi));
    } else if (g_selectedActivity == ActivityKind::Hotspot) {
        g_popupTitleSolar.Text(PickText(vi, L"Điểm phát sóng Wi-Fi", L"Wi-Fi hotspot"));
        g_popupTitleLunar.Text(HotspotStateText(g_uiLive.hotspot, vi));
    }
    g_popupTitleLunar.Visibility(g_popupTitleLunar.Text().empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
    g_noticeText.MaxLines(g_selectedActivity == ActivityKind::Memory ? 10 : 3);
    g_noticeText.Text(message);
    g_compactNotice.Visibility(message.empty() ? wux::Visibility::Collapsed : wux::Visibility::Visible);
}
float MeasurePopupElement(wux::FrameworkElement const& element, float width) {
    if (!element || element.Visibility() == wux::Visibility::Collapsed) return 0;
    element.Measure({std::max(1.0f, width), std::numeric_limits<float>::infinity()});
    return element.DesiredSize().Height;
}
int DesiredPopupHeight(int width) {
    const float inner = static_cast<float>(std::max(1, width - 30));
    float height = 24 + MeasurePopupElement(g_popupHeader, inner) + MeasurePopupElement(g_popupDivider, inner);
    if (g_popupMode == PopupMode::Automatic)
        return static_cast<int>(std::ceil(height + MeasurePopupElement(g_compactNotice, inner)));
    float body = 0;
    for (auto scroll : {g_popupHoroscopeScroll, g_popupHolidayScroll, g_popupLiveScroll, g_monthScroll, g_cultureScroll}) {
        if (!scroll || scroll.Visibility() == wux::Visibility::Collapsed) continue;
        auto content = scroll.Content().try_as<wux::FrameworkElement>();
        if (scroll == g_popupLiveScroll) {
            if (auto stack = content.try_as<wuxc::StackPanel>()) {
                body = 4;
                for (auto const& child : stack.Children()) {
                    if (auto element = child.try_as<wux::FrameworkElement>()) body += MeasurePopupElement(element, inner - 24);
                }
            } else body = MeasurePopupElement(content, inner - 16);
        } else body = MeasurePopupElement(content, inner - 16);
        body += 4;
        if (scroll == g_popupHolidayScroll || scroll == g_cultureScroll ||
                   (g_selectedActivity == ActivityKind::Battery && g_showUsage) || g_selectedActivity == ActivityKind::Download || g_selectedActivity==ActivityKind::Clock) {
            body = std::min(body, 460.0f);
        }
        break;
    }
    height += body + MeasurePopupElement(g_tabsHost, inner);
    return static_cast<int>(std::ceil(height));
}

void CapturePopupSourceGeometry(int& outCardWidthDip, int& outCardHeightDip) {
    ModSettings settings = GetSettings();
    RECT taskbar{};
    GetWindowRect(g_taskbarWnd, &taskbar);
    if (!GetElementScreenRect(g_playerGrid, g_popupSourceRect)) {
        int leftPx = PopupDipToPx(settings.widgetLeftOffset, g_taskbarWnd);
        g_popupSourceRect = {
            taskbar.left + leftPx, taskbar.top,
            taskbar.left + leftPx + PopupDipToPx(settings.widgetWidth, g_taskbarWnd),
            taskbar.bottom
        };
    }

    HMONITOR monitor = MonitorFromRect(&g_popupSourceRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        monitorInfo.rcMonitor = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
        monitorInfo.rcWork = monitorInfo.rcMonitor;
    }

    double dpiScale = PopupDpiScale(g_taskbarWnd);
    bool opensDown = TaskbarPopupOpensDown(taskbar, monitorInfo);
    g_popupOpensDown = opensDown;
    g_taskbarAtTop = opensDown;
    int popupGap = PopupDipToPx(opensDown ? 6.0 : 10.0, g_taskbarWnd);
    int edgePad = PopupDipToPx(12.0, g_taskbarWnd);

    int availableWidthPx = std::max(1L, monitorInfo.rcWork.right - monitorInfo.rcWork.left - 2 * edgePad);
    int availableHeightPx = opensDown
        ? std::max(1L, monitorInfo.rcWork.bottom - taskbar.bottom - popupGap - edgePad)
        : std::max(1L, taskbar.top - popupGap - monitorInfo.rcWork.top - edgePad);
    outCardWidthDip = std::min(settings.expandedWidth,
        std::max(1, static_cast<int>(std::floor(availableWidthPx / dpiScale))));
    if (g_popupMode == PopupMode::Automatic) outCardWidthDip = std::min(outCardWidthDip, 380);
    // Measure this page at its actual width, then clamp only to available screen space.
    outCardHeightDip = std::min(DesiredPopupHeight(outCardWidthDip),
        std::max(1, static_cast<int>(std::floor(availableHeightPx / dpiScale))));

    int widthPx = PopupDipToPx(outCardWidthDip, g_taskbarWnd);
    int heightPx = PopupDipToPx(outCardHeightDip, g_taskbarWnd);
    int minLeft = monitorInfo.rcWork.left + edgePad;
    int maxLeft = std::max(minLeft, static_cast<int>(monitorInfo.rcWork.right) - edgePad - widthPx);
    int left = ClampVal(static_cast<int>(g_popupSourceRect.left), minLeft, maxLeft);
    int top = opensDown ? taskbar.bottom + popupGap : taskbar.top - popupGap - heightPx;
    g_popupFinalRect = {left, top, left + widthPx, top + heightPx};
    g_popupHostRect = g_popupFinalRect;
}

void PositionExpandedPopup() {
    if (!g_expandedPopup || !g_taskbarWnd) {
        return;
    }

    RECT hostRect = g_popupHostRect;
    int width = std::max(1L, hostRect.right - hostRect.left);
    int height = std::max(1L, hostRect.bottom - hostRect.top);

    if (!EqualRect(&hostRect, &g_lastPopupWindowRect)) {
        g_lastPopupWindowRect = hostRect;
        SetWindowPos(g_expandedPopup, HWND_TOPMOST,
                     hostRect.left, hostRect.top, width, height,
                     SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
    }
    if (g_popupXamlChild) {
        MoveWindow(g_popupXamlChild, 0, 0, width, height, TRUE);
    }
}

// Minimal D2D effect interop avoids a Win2D runtime/package dependency.
// The desktop source is still Windows HostBackdrop; this is a glass treatment,
// not Apple's private renderer and not screen capture.
struct IVnGraphicsEffectD2DInterop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(PCWSTR,UINT*,UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT,IInspectable**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT,IInspectable**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT*) = 0;
};
__CRT_UUID_DECL(IVnGraphicsEffectD2DInterop,0x2fc57384,0xa068,0x44d7,0xa3,0x31,0x30,0x98,0x2f,0xcf,0x71,0x77);
struct GlassLensEffect : winrt::implements<GlassLensEffect,
        winrt::Windows::Graphics::Effects::IGraphicsEffect,
        winrt::Windows::Graphics::Effects::IGraphicsEffectSource,IVnGraphicsEffectD2DInterop> {
    winrt::hstring name{L"Refraction"};
    winrt::Windows::Graphics::Effects::IGraphicsEffectSource source{nullptr};
    winrt::hstring Name() { return name; }
    void Name(winrt::hstring const& value) { name=value; }
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if(!id)return E_POINTER; *id=CLSID_D2D12DAffineTransform;return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(PCWSTR property,UINT* index,UINT* mapping) noexcept override {
        if(!property||!index||!mapping)return E_POINTER;
        if(wcscmp(property,L"TransformMatrix"))return E_INVALIDARG;
        *index=2;*mapping=1;return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {if(!count)return E_POINTER;*count=4;return S_OK;}
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {if(!count)return E_POINTER;*count=1;return S_OK;}
    HRESULT STDMETHODCALLTYPE GetSource(UINT index,::IInspectable** value) noexcept override {
        if(!value)return E_POINTER;*value=nullptr;if(index)return E_INVALIDARG;
        winrt::copy_to_abi(source,*reinterpret_cast<void**>(value));return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index,::IInspectable** value) noexcept override {
        if(!value)return E_POINTER;*value=nullptr;
        try {
            wf::IInspectable property{nullptr};
            if(index==0)property=wf::PropertyValue::CreateUInt32(1); // linear interpolation
            else if(index==1)property=wf::PropertyValue::CreateUInt32(1); // hard border
            else if(index==2) { const float matrix[]={1.035f,0,0,1.035f,-8.0f,-5.0f};property=wf::PropertyValue::CreateSingleArray(matrix); }
            else if(index==3)property=wf::PropertyValue::CreateSingle(0);
            else return E_INVALIDARG;
            auto boxed = property.as<wf::IPropertyValue>();
            winrt::copy_to_abi(boxed,*reinterpret_cast<void**>(value));return S_OK;
        } catch(...) {return winrt::to_hresult();}
    }
};
// Gaussian blur samples only the layer BEHIND this visual, never the widget text.
struct WidgetBlurEffect : winrt::implements<WidgetBlurEffect,
        winrt::Windows::Graphics::Effects::IGraphicsEffect,
        winrt::Windows::Graphics::Effects::IGraphicsEffectSource,IVnGraphicsEffectD2DInterop> {
    winrt::hstring name{L"WidgetBlur"};
    winrt::Windows::Graphics::Effects::IGraphicsEffectSource source{nullptr};
    float amount=12;
    winrt::hstring Name(){return name;}
    void Name(winrt::hstring const& value){name=value;}
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {if(!id)return E_POINTER;*id=CLSID_D2D1GaussianBlur;return S_OK;}
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(PCWSTR property,UINT* index,UINT* mapping) noexcept override {
        if(!property||!index||!mapping)return E_POINTER;
        if(wcscmp(property,L"BlurAmount"))return E_INVALIDARG;*index=0;*mapping=1;return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {if(!count)return E_POINTER;*count=3;return S_OK;}
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {if(!count)return E_POINTER;*count=1;return S_OK;}
    HRESULT STDMETHODCALLTYPE GetSource(UINT index,::IInspectable** value) noexcept override {
        if(!value)return E_POINTER;*value=nullptr;if(index)return E_INVALIDARG;
        winrt::copy_to_abi(source,*reinterpret_cast<void**>(value));return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index,::IInspectable** value) noexcept override {
        if(!value)return E_POINTER;*value=nullptr;
        try {
            wf::IInspectable property{nullptr};
            if(index==0)property=wf::PropertyValue::CreateSingle(amount);
            else if(index==1)property=wf::PropertyValue::CreateUInt32(1); // balanced optimization
            else if(index==2)property=wf::PropertyValue::CreateUInt32(1); // hard border
            else return E_INVALIDARG;
            winrt::copy_to_abi(property.as<wf::IPropertyValue>(),*reinterpret_cast<void**>(value));return S_OK;
        }catch(...){return winrt::to_hresult();}
    }
};
[[clang::no_destroy]] wcomp::ContainerVisual g_widgetMaterialVisual{nullptr};
[[clang::no_destroy]] wcomp::CompositionRoundedRectangleGeometry g_widgetMaterialClip{nullptr};
int g_widgetMaterialStyle=-1;
float WidgetBackgroundOpacity(bool hover,bool pressed,bool open){return pressed||open?1.0f:hover?0.30f:0.0f;}
void SizeWidgetMaterial() {
    if(!g_widgetBackdropLayer || !g_widgetMaterialVisual || !g_widgetMaterialClip)return;
    auto width=std::max(0.f,static_cast<float>(g_widgetBackdropLayer.ActualWidth()));
    auto height=std::max(0.f,static_cast<float>(g_widgetBackdropLayer.ActualHeight()));
    g_widgetMaterialClip.Size({width,height});
    float radius=std::min({6.f,width*.5f,height*.5f});g_widgetMaterialClip.CornerRadius({radius,radius});
}
void ReleaseWidgetMaterial() {
    if(g_widgetBackdropLayer)try{wuxh::ElementCompositionPreview::SetElementChildVisual(g_widgetBackdropLayer,nullptr);}catch(...){}
    g_widgetMaterialVisual=nullptr;g_widgetMaterialClip=nullptr;
    g_widgetBackdropLayer=nullptr;g_widgetMaterialRim=nullptr;
    g_widgetMaterialStyle=-1;g_widgetPressed=false;
}
void UpdateWidgetMaterial(bool light,bool open) {
    if(!g_widgetBackdropLayer || !g_widgetMaterialRim)return;
    bool allowed=g_effectsAllowed.load();
    bool glass=allowed && g_glassCpuSupported && GetSettings().popupTheme==L"glass";
    int style=(light?1:0)|(glass?2:0)|(allowed?4:0);
    if(style!=g_widgetMaterialStyle) {
        wuxh::ElementCompositionPreview::SetElementChildVisual(g_widgetBackdropLayer,nullptr);
        g_widgetMaterialVisual=nullptr;g_widgetMaterialClip=nullptr;
        auto tint=light?winrt::Windows::UI::Color{255,241,241,246}:winrt::Windows::UI::Color{255,33,34,40};
        g_widgetBackdropLayer.Background(wuxm::SolidColorBrush(tint));
        g_widgetMaterialRim.BorderBrush(glass?GlassEdgeBrush().as<wuxm::Brush>():wuxm::SolidColorBrush(light?
            winrt::Windows::UI::Color{40,0,0,0}:winrt::Windows::UI::Color{65,255,255,255}).as<wuxm::Brush>());
        if(allowed)try {
            auto compositor=wuxh::ElementCompositionPreview::GetElementVisual(g_widgetBackdropLayer).Compositor();
            auto container=compositor.CreateContainerVisual();container.RelativeSizeAdjustment({1,1});
            auto source=wcomp::CompositionEffectSourceParameter(L"Background");
            auto blur=winrt::make_self<WidgetBlurEffect>();blur->source=source;blur->amount=glass?3.f:12.f;
            winrt::Windows::Graphics::Effects::IGraphicsEffect graph=blur.as<winrt::Windows::Graphics::Effects::IGraphicsEffect>();
            if(glass) {
                auto lens=winrt::make_self<GlassLensEffect>();lens->source=graph.as<winrt::Windows::Graphics::Effects::IGraphicsEffectSource>();
                graph=lens.as<winrt::Windows::Graphics::Effects::IGraphicsEffect>();
            }
            auto brush=compositor.CreateEffectFactory(graph).CreateBrush();
            brush.SetSourceParameter(L"Background",compositor.CreateBackdropBrush());
            auto backdrop=compositor.CreateSpriteVisual();backdrop.RelativeSizeAdjustment({1,1});backdrop.Brush(brush);
            container.Children().InsertAtBottom(backdrop);
            auto overlay=compositor.CreateSpriteVisual();overlay.RelativeSizeAdjustment({1,1});
            tint.A=static_cast<uint8_t>(glass?(light?55:65):(light?150:135));overlay.Brush(compositor.CreateColorBrush(tint));container.Children().InsertAtTop(overlay);
            if(glass) {
                auto sheen=compositor.CreateSpriteVisual();sheen.RelativeSizeAdjustment({1,1});
                auto gradient=compositor.CreateLinearGradientBrush();gradient.StartPoint({0,0});gradient.EndPoint({1,1});
                gradient.ColorStops().Append(compositor.CreateColorGradientStop(0,{70,255,255,255}));
                gradient.ColorStops().Append(compositor.CreateColorGradientStop(.5f,{0,255,255,255}));
                gradient.ColorStops().Append(compositor.CreateColorGradientStop(1,{25,255,255,255}));
                sheen.Brush(gradient);container.Children().InsertAtTop(sheen);
            }
            g_widgetMaterialClip=compositor.CreateRoundedRectangleGeometry();
            container.Clip(compositor.CreateGeometricClip(g_widgetMaterialClip));g_widgetMaterialVisual=container;
            wuxh::ElementCompositionPreview::SetElementChildVisual(g_widgetBackdropLayer,container);
            g_widgetBackdropLayer.Background(wuxm::SolidColorBrush({0,0,0,0}));SizeWidgetMaterial();
        }catch(...) {
            try{wuxh::ElementCompositionPreview::SetElementChildVisual(g_widgetBackdropLayer,nullptr);}catch(...){}
            g_widgetMaterialVisual=nullptr;g_widgetMaterialClip=nullptr;
            // Native Acrylic is the fallback for Blur and popup-theme Off alike.
            try {
                wuxm::AcrylicBrush acrylic;acrylic.BackgroundSource(wuxm::AcrylicBackgroundSource::Backdrop);
                tint.A=255;acrylic.TintColor(tint);acrylic.TintOpacity(.60);acrylic.FallbackColor(tint);
                g_widgetBackdropLayer.Background(acrylic);
            }catch(...){}
        }
        else {
            COLORREF color=GetSysColor(COLOR_WINDOW);
            g_widgetBackdropLayer.Background(wuxm::SolidColorBrush({255,GetRValue(color),GetGValue(color),GetBValue(color)}));
        }
        g_widgetMaterialStyle=style;
    }
    float opacity=WidgetBackgroundOpacity(g_widgetHovered,g_widgetPressed,open);
    // Opacity belongs to the background siblings, not to the root or text.
    g_widgetBackdropLayer.Opacity(opacity);g_widgetMaterialRim.Opacity(opacity);
}

[[clang::no_destroy]] wcomp::CompositionEffectBrush g_glassLens{nullptr};
[[clang::no_destroy]] wcomp::CompositionRadialGradientBrush g_glassHighlight{nullptr};
void SizeGlassLens() {
    if(!g_glassLens||!g_popupBackdropLayer)return;
    try {
        float w=static_cast<float>(g_popupBackdropLayer.ActualWidth()),h=static_cast<float>(g_popupBackdropLayer.ActualHeight());
        g_glassLens.Properties().InsertMatrix3x2(L"Refraction.TransformMatrix",{1.035f,0,0,1.035f,-w*.0175f,-h*.0175f});
    } catch(...) {}
}
void AddGlassHighlights(wcomp::Compositor const& compositor,wcomp::ContainerVisual const& container) {
    auto sheen=compositor.CreateSpriteVisual();sheen.RelativeSizeAdjustment({1,1});
    auto gradient=compositor.CreateLinearGradientBrush();gradient.StartPoint({0,0});gradient.EndPoint({.85f,1});
    for(auto [offset,alpha] : {std::pair{0.f,65},std::pair{.22f,12},std::pair{.55f,0},std::pair{1.f,28}})
        gradient.ColorStops().Append(compositor.CreateColorGradientStop(offset,{static_cast<uint8_t>(alpha),255,255,255}));
    sheen.Brush(gradient);container.Children().InsertAtTop(sheen);
    auto highlight=compositor.CreateSpriteVisual();highlight.RelativeSizeAdjustment({1,1});
    auto radial=compositor.CreateRadialGradientBrush();radial.EllipseCenter({.15f,.05f});radial.EllipseRadius({.65f,.5f});
    radial.ColorStops().Append(compositor.CreateColorGradientStop(0,{48,255,255,255}));
    radial.ColorStops().Append(compositor.CreateColorGradientStop(1,{0,255,255,255}));
    highlight.Brush(radial);container.Children().InsertAtTop(highlight);g_glassHighlight=radial;
}

void ClearPopupCompositionBackdrop() {
    if (g_popupBackdropLayer) {
        try { wuxh::ElementCompositionPreview::SetElementChildVisual(g_popupBackdropLayer, nullptr); }
        catch (...) {}
    }
    g_popupBackdropVisual = nullptr;
    g_popupTintBrush = nullptr;
    g_glassLens = nullptr; g_glassHighlight = nullptr;
}

bool SetPopupCompositionBackdrop(bool light) {
    if (!g_popupBackdropLayer || !g_expandedPopup || !g_popupXamlChild) return false;
    try {
        // Paint the sampled desktop INSIDE the island, behind the content.
        // This does not require the island to reveal its parent HWND's pixels.
        BOOL enabled = TRUE;
        HRESULT hostHr = DwmSetWindowAttribute(g_expandedPopup, 17, &enabled, sizeof(enabled));
        HRESULT islandHr = DwmSetWindowAttribute(g_popupXamlChild, 17, &enabled, sizeof(enabled));
        if (FAILED(hostHr) && FAILED(islandHr)) return false;
        DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUND;
        DwmSetWindowAttribute(g_expandedPopup, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
        BOOL dark = light ? FALSE : TRUE;
        DwmSetWindowAttribute(g_expandedPopup, 20, &dark, sizeof(dark));

        if (!g_popupBackdropVisual) {
            auto visual = wuxh::ElementCompositionPreview::GetElementVisual(g_popupBackdropLayer);
            auto compositor = visual.Compositor();
            auto container = compositor.CreateContainerVisual();
            container.RelativeSizeAdjustment({1.0f, 1.0f});
            auto backdrop = compositor.CreateSpriteVisual();
            backdrop.RelativeSizeAdjustment({1.0f, 1.0f});
            auto desktop = compositor.CreateHostBackdropBrush();
            backdrop.Brush(desktop);
            if (UseGlassSurfaces()) {
                try {
                    auto effect = winrt::make_self<GlassLensEffect>();
                    effect->source = wcomp::CompositionEffectSourceParameter(L"Desktop");
                    auto factory = compositor.CreateEffectFactory(effect.as<winrt::Windows::Graphics::Effects::IGraphicsEffect>(), {L"Refraction.TransformMatrix"});
                    g_glassLens = factory.CreateBrush(); g_glassLens.SetSourceParameter(L"Desktop", desktop);
                    backdrop.Brush(g_glassLens); SizeGlassLens();
                } catch (...) { g_glassLens = nullptr; }
            }
            auto tint = compositor.CreateSpriteVisual();
            tint.RelativeSizeAdjustment({1.0f, 1.0f});
            auto tintBrush = compositor.CreateColorBrush();
            tint.Brush(tintBrush);
            container.Children().InsertAtBottom(backdrop);
            container.Children().InsertAtTop(tint);
            if (UseGlassSurfaces()) AddGlassHighlights(compositor, container);
            wuxh::ElementCompositionPreview::SetElementChildVisual(g_popupBackdropLayer, container);
            g_popupBackdropVisual = container;
            g_popupTintBrush = tintBrush;
        }
        bool glass = UseGlassSurfaces();
        g_popupTintBrush.Color(light ? winrt::Windows::UI::Color{static_cast<uint8_t>(glass ? 24 : 145), 246, 246, 250}
                                    : winrt::Windows::UI::Color{static_cast<uint8_t>(glass ? 38 : 140), 32, 33, 38});
        g_popupRootCard.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
        Wh_Log(L"Composition HostBackdrop attached: host=0x%08X, island=0x%08X",
               static_cast<unsigned int>(hostHr), static_cast<unsigned int>(islandHr));
        return true;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Composition failed: 0x%08X", static_cast<unsigned int>(e.code().value));
        ClearPopupCompositionBackdrop();
        return false;
    }
}

void ApplyPopupBackdrop(bool light) {
    if (!g_popupRootCard || !g_expandedPopup) return;
    auto theme = EffectivePopupTheme(GetSettings());
    bool glass = theme == L"glass";
    bool allowed = g_effectsAllowed.load() && theme != L"solid";
    int state = int(light) | (allowed ? 2 : 0) | (glass ? 4 : 0);
    if (!g_popupBackdropDirty && g_popupBackdropTheme == state) return;
    if (glass && allowed) {
        wuxm::LinearGradientBrush edge; edge.StartPoint({0, 0}); edge.EndPoint({1, 1});
        for (auto [offset, alpha] : {std::pair{0.0, 180}, std::pair{.45, 35}, std::pair{1.0, 100}}) {
            wuxm::GradientStop stop; stop.Offset(offset); stop.Color({static_cast<uint8_t>(alpha), 255, 255, 255}); edge.GradientStops().Append(stop);
        }
        g_popupRootCard.BorderBrush(GlassEdgeBrush());
        if (g_popupGlassRim) g_popupGlassRim.BorderBrush(GlassEdgeBrush());
        if (g_popupGlassInnerRim) {
            wuxm::LinearGradientBrush inner; inner.StartPoint({0, 0}); inner.EndPoint({1, 1});
            for (auto [offset, color] : {std::pair{0.0, winrt::Windows::UI::Color{85,255,255,255}},
                std::pair{0.5, winrt::Windows::UI::Color{18,255,255,255}},
                std::pair{1.0, winrt::Windows::UI::Color{65,0,0,0}}}) {
                wuxm::GradientStop stop; stop.Offset(offset); stop.Color(color); inner.GradientStops().Append(stop);
            }
            g_popupGlassInnerRim.BorderBrush(inner);
        }
    }
    auto rimVisibility = glass && allowed ? wux::Visibility::Visible : wux::Visibility::Collapsed;
    if (g_popupGlassRim) g_popupGlassRim.Visibility(rimVisibility);
    if (g_popupGlassInnerRim) g_popupGlassInnerRim.Visibility(rimVisibility);
    ClearPopupCompositionBackdrop();
    if (!allowed || !SetPopupCompositionBackdrop(light)) {
        BOOL disabled = FALSE;
        DwmSetWindowAttribute(g_expandedPopup, 17, &disabled, sizeof(disabled));
        if (g_popupXamlChild) DwmSetWindowAttribute(g_popupXamlChild, 17, &disabled, sizeof(disabled));
        auto fallback = light ? winrt::Windows::UI::Color{255, 246, 246, 250} : winrt::Windows::UI::Color{255, 32, 33, 38};
        g_popupRootCard.Background(wuxm::SolidColorBrush(fallback));
    }
    g_popupBackdropTheme = state; g_popupBackdropDirty = false;
}

void ResetPopupScrollPosition(wuxc::ScrollViewer const& scroll) {
    if (!scroll) return;
    try {
        scroll.ChangeView(nullptr, winrt::box_value(0.0).as<wf::IReference<double>>(), nullptr, true);
    } catch (...) {}
}

void ShowPopupHolidayList(bool show) {
    g_popupShowingHolidays = show;
    if (show) g_showingMonth = false;
    if (g_popupHoroscopeScroll)
        g_popupHoroscopeScroll.Visibility(!show && g_selectedActivity == ActivityKind::Lunar ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (g_popupHolidayScroll)
        g_popupHolidayScroll.Visibility(show && g_selectedActivity == ActivityKind::Lunar ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (g_popupHolidayListButton) {
        bool vi = UseVietnameseLabels(GetSettings());
        g_popupHolidayListButton.Content(winrt::box_value(
            show ? L"↩" : PickText(vi, L"Lễ", L"List")));
        wuxc::ToolTipService::SetToolTip(g_popupHolidayListButton, winrt::box_value(
            show ? PickText(vi, L"Trở về Tử Vi", L"Back to horoscope")
                 : PickText(vi, L"Ngày lễ & kỷ niệm Việt Nam", L"Vietnamese holidays and commemorations")));
    }
}

void ReleasePopupScrollControls() {
    ReleaseLivePopupControls();
    g_popupHoroscopeScroll = nullptr;
    g_popupHolidayScroll = nullptr;
    g_popupHolidayListText = nullptr;
    g_popupHolidayListButton = nullptr;
    g_popupShowingHolidays = false;
    g_popupHolidayListLanguage = -1;
}

void AnimateContentChange(wux::UIElement const& element, int direction) {
    if (!element) return;
    try {
        if (!winrt::Windows::UI::ViewManagement::UISettings().AnimationsEnabled()) return;
        auto visual = wuxh::ElementCompositionPreview::GetElementVisual(element);
        auto compositor = visual.Compositor();
        auto fade = compositor.CreateScalarKeyFrameAnimation(); fade.Duration(std::chrono::milliseconds(220));
        fade.InsertKeyFrame(0, .18f); fade.InsertKeyFrame(1, 1);
        visual.StartAnimation(L"Opacity", fade);
        // Animate translation rather than Offset so the XAML layout position stays intact.
        wuxh::ElementCompositionPreview::SetIsTranslationEnabled(element, true);
        auto slide = compositor.CreateVector3KeyFrameAnimation(); slide.Duration(std::chrono::milliseconds(220));
        auto easing = compositor.CreateCubicBezierEasingFunction({.16f, 1}, {.3f, 1});
        slide.InsertKeyFrame(0, {direction ? 24.0f * direction : 0, direction ? 0.0f : 8.0f, 0}); slide.InsertKeyFrame(1, {0, 0, 0}, easing);
        visual.StartAnimation(L"Translation", slide);
    } catch (...) {}
}
std::wstring g_renderedPageKey;
void AnimatePageIfChanged() {
    std::wstring key = std::to_wstring(static_cast<int>(g_selectedActivity));
    if (g_selectedActivity == ActivityKind::Lunar)
        key += L":" + std::to_wstring(g_showingMonth) + L":" + std::to_wstring(g_popupShowingHolidays) +
            L":" + std::to_wstring(g_popupDayOffset) + L":" + std::to_wstring(g_monthOffset);
    if (g_selectedActivity == ActivityKind::Battery) key += L":" + std::to_wstring(g_showUsage);
    if (g_selectedActivity == ActivityKind::Privacy) key += L":" + g_selectedPrivacy;
    if (g_selectedActivity == ActivityKind::Culture) key += L":" + g_cultureSelectedId;
    if (g_selectedActivity == ActivityKind::Media) key += L":" + std::to_wstring(g_uiLive.media.sessionId);
    static ActivityKind lastActivity = ActivityKind::None;
    bool activityChanged = lastActivity != ActivityKind::None && lastActivity != g_selectedActivity;
    lastActivity = g_selectedActivity;
    if (!g_renderedPageKey.empty() && g_renderedPageKey != key && g_popupMode == PopupMode::Manual &&
        g_expandedPopup && IsWindowVisible(g_expandedPopup) && !g_popupClosing)
        AnimateContentChange(g_popupRootCard.Child(), activityChanged ? g_activityDirection : 0);
    g_renderedPageKey = std::move(key);
    ApplyGlassButtons(g_popupRootCard);
}

void RefreshExpandedPopupData() {
    static bool refreshing = false;
    if (!g_popupRootCard || g_shuttingDown || refreshing) return;
    refreshing = true;
    struct RefreshGuard { bool& flag; ~RefreshGuard() { flag = false; } } guard{refreshing};
    try {
        ModSettings settings = GetSettings();
        bool vi = UseVietnameseLabels(settings);
        bool light = UseGlassSurfaces() && g_popupContrastValid ? g_popupContrastLight : IsElementLightTheme(g_playerGrid);
        g_uiLive = ReadLiveSnapshot();
        NormalizeSelectedActivity(settings);
        if (g_popupMode == PopupMode::Automatic &&
            ((g_selectedActivity == ActivityKind::Download && g_uiLive.tasks.empty() && !HasTaskNotice(g_uiLive)) ||
             (g_selectedActivity == ActivityKind::Privacy && g_uiLive.privacy.uses.empty()))) {
            BeginCloseExpandedPopup(g_expandedPopup);
            return;
        }

        SYSTEMTIME st{};
        GetLocalTime(&st);
        if (g_popupDayOffset != 0) {
            OffsetSystemDateByDays(st, g_popupDayOffset);
        }
        auto const& h = CachedHoroscope(st, vi);

        g_popupRootCard.RequestedTheme(light ? wux::ElementTheme::Light : wux::ElementTheme::Dark);

        winrt::Windows::UI::Color cardBorder = light
            ? winrt::Windows::UI::Color{40, 0, 0, 0}
            : winrt::Windows::UI::Color{45, 255, 255, 255};
        winrt::Windows::UI::Color textPrimary = light
            ? winrt::Windows::UI::Color{255, 20, 20, 24}
            : winrt::Windows::UI::Color{255, 245, 245, 248};
        winrt::Windows::UI::Color accentLunar = light
            ? winrt::Windows::UI::Color{255, 196, 43, 28}
            : winrt::Windows::UI::Color{255, 255, 145, 130};

        if (!UseGlassSurfaces())
            g_popupRootCard.BorderBrush(wuxm::SolidColorBrush(cardBorder));
        ApplyPopupBackdrop(light);
        if (g_selectedActivity != ActivityKind::Lunar) {
            if (g_popupHolidayBanner) g_popupHolidayBanner.Visibility(wux::Visibility::Collapsed);
            if (g_popupTitleSolar) g_popupTitleSolar.Foreground(wuxm::SolidColorBrush(textPrimary));
            if (g_popupTitleLunar) g_popupTitleLunar.Foreground(wuxm::SolidColorBrush(accentLunar));
            RefreshLivePopupPage(vi, light);
            ApplyPopupPresentation(vi);
            int cardW = 380, cardH = kExpandedPopupHeightDip;
            CapturePopupSourceGeometry(cardW, cardH);
            g_popupRootCard.Width(cardW); g_popupRootCard.Height(cardH);
            PositionExpandedPopup();
            AnimatePageIfChanged();
            return;
        }


        if (g_popupBadgeBorder) {
            g_popupBadgeBorder.Background(wuxm::SolidColorBrush(GetFiveElementColor(h.element, light)));
        }
        if (g_popupZodiacText) {
            g_popupZodiacText.Text(h.zodiacEmoji);
            g_popupZodiacText.Visibility(wux::Visibility::Visible);
        }

        WCHAR solarTitle[128]{};
        _snwprintf_s(solarTitle, _TRUNCATE, L"%s, %02d/%02d/%d",
                     h.weekdayStr, h.sDay, h.sMonth, h.sYear);
        if (g_popupTitleSolar) {
            g_popupTitleSolar.Text(solarTitle);
            g_popupTitleSolar.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        WCHAR lunarTitle[160]{};
        if (vi) {
            _snwprintf_s(lunarTitle, _TRUNCATE, L"Âm lịch: %02d/%02d%s • Năm %s",
                         h.lDay, h.lMonth, h.isLeap ? L" (Nhuận)" : L"", h.yearCanChi.c_str());
        } else {
            _snwprintf_s(lunarTitle, _TRUNCATE, L"Lunar: %02d/%02d%s • Year of %s",
                         h.lDay, h.lMonth, h.isLeap ? L" (Leap)" : L"", h.yearCanChi.c_str());
        }
        if (g_popupTitleLunar) {
            g_popupTitleLunar.Text(lunarTitle);
            g_popupTitleLunar.Foreground(wuxm::SolidColorBrush(accentLunar));
        }

        bool hasHoliday = !h.specialHoliday.empty();
        if (g_popupHolidayBanner && g_popupHolidayText) {
            if (hasHoliday) {
                g_popupHolidayText.Text(L"🎉 " + h.specialHoliday);
                g_popupHolidayText.Foreground(wuxm::SolidColorBrush(accentLunar));
                g_popupHolidayBanner.Visibility(wux::Visibility::Visible);
            } else {
                g_popupHolidayText.Text(PickText(vi, L"Không có ngày lễ hôm nay", L"No holiday today"));
                g_popupHolidayText.Foreground(wuxm::SolidColorBrush(textPrimary));
                g_popupHolidayBanner.Visibility(wux::Visibility::Collapsed);
            }
        }

        WCHAR buf[512]{};
        if (g_popupRowBattu) {
            if (vi) {
                _snwprintf_s(buf, _TRUNCATE, L"%s Ngày %s (Con %s) • Tháng %s • Giờ %s",
                             h.zodiacEmoji, h.dayCanChi.c_str(), h.zodiacAnimalName,
                             h.monthCanChi.c_str(), h.hourCanChi.c_str());
            } else {
                _snwprintf_s(buf, _TRUNCATE, L"%s Day %s (%s) • Month %s • Hour %s",
                             h.zodiacEmoji, h.dayCanChi.c_str(), h.zodiacAnimalName,
                             h.monthCanChi.c_str(), h.hourCanChi.c_str());
            }
            g_popupRowBattu.Text(buf);
            g_popupRowBattu.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        if (g_popupRowMenhTruc) {
            if (vi) {
                _snwprintf_s(buf, _TRUNCATE, L"☯ Mệnh: %s\n%s %s (%s) • Trực: %s",
                             h.napAmStr, h.isHoangDao ? L"🌟" : L"⚪",
                             h.starName, h.isHoangDao ? L"Ngày Tốt" : L"Hắc Đạo", h.trucName);
            } else {
                _snwprintf_s(buf, _TRUNCATE, L"☯ Element: %s\n%s %s (%s) • Officer: %s",
                             h.napAmStr, h.isHoangDao ? L"🌟" : L"⚪",
                             h.starName, h.isHoangDao ? L"Auspicious" : L"Inauspicious", h.trucName);
            }
            g_popupRowMenhTruc.Text(buf);
            g_popupRowMenhTruc.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        if (g_popupRowGioHoangDao) {
            if (vi) {
                _snwprintf_s(buf, _TRUNCATE, L"⏰ Giờ Hoàng Đạo: %s", h.gioHoangDao);
            } else {
                _snwprintf_s(buf, _TRUNCATE, L"⏰ Auspicious Hours: %s", h.gioHoangDao);
            }
            g_popupRowGioHoangDao.Text(buf);
            g_popupRowGioHoangDao.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        if (g_popupRowNenKy) {
            if (vi) {
                _snwprintf_s(buf, _TRUNCATE, L"🔮 Nên: %s\n⛔ Kỵ: %s", h.goodFor, h.badFor);
            } else {
                _snwprintf_s(buf, _TRUNCATE, L"🔮 Good for: %s\n⛔ Avoid: %s", h.goodFor, h.badFor);
            }
            g_popupRowNenKy.Text(buf);
            g_popupRowNenKy.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        if (g_popupRowXungHuong) {
            if (vi) {
                _snwprintf_s(buf, _TRUNCATE, L"🧭 Tuổi xung: %s • Xuất hành: %s (Hỷ), %s (Tài)",
                             h.tuoiXung.c_str(), h.huongHyThan, h.huongTaiThan);
            } else {
                _snwprintf_s(buf, _TRUNCATE, L"🧭 Clash: %s • Directions: %s (Joy), %s (Wealth)",
                             h.tuoiXung.c_str(), h.huongHyThan, h.huongTaiThan);
            }
            g_popupRowXungHuong.Text(buf);
            g_popupRowXungHuong.Foreground(wuxm::SolidColorBrush(textPrimary));
        }

        if (g_popupHolidayListText) {
            if (g_popupHolidayListLanguage != static_cast<int>(vi)) {
                g_popupHolidayListText.Text(BuildHolidayCatalogText(vi));
                g_popupHolidayListLanguage = static_cast<int>(vi);
            }
            g_popupHolidayListText.Foreground(wuxm::SolidColorBrush(textPrimary));
        }
        ShowPopupHolidayList(g_popupShowingHolidays);
        RefreshLivePopupPage(vi, light);

        ApplyPopupPresentation(vi);
        int cardW = 380, cardH = kExpandedPopupHeightDip;
        CapturePopupSourceGeometry(cardW, cardH);
        g_popupRootCard.Width(static_cast<double>(cardW));
        g_popupRootCard.Height(static_cast<double>(cardH));

        PositionExpandedPopup();
        AnimatePageIfChanged();
    } catch (...) {}
}

wuxc::Border CreatePopupRowSection(wuxc::TextBlock& tbOut, bool withBottomDivider = true) {
    wuxc::Border row;
    row.Padding({4, 5, 4, 6});
    if (withBottomDivider) {
        row.BorderThickness({0, 0, 0, 1});
        row.BorderBrush(wuxm::SolidColorBrush(winrt::Windows::UI::Color{28, 128, 128, 128}));
    }

    wuxc::TextBlock tb;
    tb.FontSize(12.2);
    tb.LineHeight(18.0);
    tb.TextWrapping(wux::TextWrapping::Wrap);
    row.Child(tb);
    tbOut = tb;
    return row;
}

void BeginCloseExpandedPopup(HWND hwnd);

bool InitializePopupXamlHost(HWND hwnd) {
    try {
        g_popupXamlSource = wuxh::DesktopWindowXamlSource();
        g_popupBackdropDirty = true;
        g_popupBackdropTheme = -1;
        auto native = g_popupXamlSource.as<IDesktopWindowXamlSourceNative>();
        winrt::check_hresult(native->AttachToWindow(hwnd));
        winrt::check_hresult(native->get_WindowHandle(&g_popupXamlChild));

        wuxc::Border rootCard;
        rootCard.CornerRadius({8, 8, 8, 8});
        rootCard.BorderThickness({1, 1, 1, 1});
        rootCard.Padding({14, 12, 14, 10});
        rootCard.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        rootCard.VerticalAlignment(wux::VerticalAlignment::Stretch);
        auto keepOpen = wuxi::PointerEventHandler([](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const&) {
            PromotePopupToManual();
        });
        // Listen even when a Button or ScrollViewer handled the pointer event.
        rootCard.AddHandler(wux::UIElement::PointerPressedEvent(), winrt::box_value(keepOpen), true);
        rootCard.AddHandler(wux::UIElement::PointerWheelChangedEvent(), winrt::box_value(keepOpen), true);
        rootCard.KeyDown([](wf::IInspectable const&, wuxi::KeyRoutedEventArgs const& args) {
            if (args.Key() != winrt::Windows::System::VirtualKey::Escape) PromotePopupToManual();
            if (args.Key() == winrt::Windows::System::VirtualKey::Escape) {
                args.Handled(true);
                BeginCloseExpandedPopup(g_expandedPopup);
            }
        });

        wuxc::Grid mainGrid;
        wuxc::RowDefinition headerRow;
        headerRow.Height(wux::GridLengthHelper::Auto());
        wuxc::RowDefinition dividerRow;
        dividerRow.Height(wux::GridLengthHelper::Auto());
        wuxc::RowDefinition bodyRow;
        bodyRow.Height(wux::GridLengthHelper::FromValueAndType(1.0, wux::GridUnitType::Star));
        mainGrid.RowDefinitions().Append(headerRow);
        mainGrid.RowDefinitions().Append(dividerRow);
        mainGrid.RowDefinitions().Append(bodyRow);
        wuxc::RowDefinition tabsRow;
        tabsRow.Height(wux::GridLengthHelper::Auto());
        mainGrid.RowDefinitions().Append(tabsRow);

        wuxc::StackPanel bodyStack;
        bodyStack.Spacing(4.0);

        wuxc::Grid headerGrid;
        wuxc::ColumnDefinition colBadge;
        colBadge.Width(wux::GridLengthHelper::Auto());
        wuxc::ColumnDefinition colTitles;
        colTitles.Width(wux::GridLengthHelper::FromValueAndType(1.0, wux::GridUnitType::Star));
        wuxc::ColumnDefinition colNav;
        colNav.Width(wux::GridLengthHelper::Auto());
        headerGrid.ColumnDefinitions().Append(colBadge);
        headerGrid.ColumnDefinitions().Append(colTitles);
        headerGrid.ColumnDefinitions().Append(colNav);
        headerGrid.Margin({0, 0, 0, 6});

        wuxc::Border badgeBorder;
        badgeBorder.Width(44.0);
        badgeBorder.Height(44.0);
        badgeBorder.CornerRadius({10, 10, 10, 10});
        badgeBorder.Margin({0, 0, 12, 0});
        badgeBorder.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock zodiacTb;
        zodiacTb.FontSize(24.0);
        zodiacTb.HorizontalAlignment(wux::HorizontalAlignment::Center);
        zodiacTb.VerticalAlignment(wux::VerticalAlignment::Center);
        badgeBorder.Child(zodiacTb);
        wuxc::Grid::SetColumn(badgeBorder, 0);
        headerGrid.Children().Append(badgeBorder);

        wuxc::StackPanel titleStack;
        titleStack.VerticalAlignment(wux::VerticalAlignment::Center);

        wuxc::TextBlock titleSolar;
        titleSolar.FontSize(14.0);
        titleSolar.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        titleSolar.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        titleSolar.MaxLines(1);

        wuxc::TextBlock titleLunar;
        titleLunar.FontSize(12.0);
        titleLunar.FontWeight(winrt::Windows::UI::Text::FontWeights::Medium());
        titleLunar.Margin({0, 2, 0, 0});
        titleLunar.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        titleLunar.MaxLines(1);

        titleStack.Children().Append(titleSolar);
        titleStack.Children().Append(titleLunar);
        wuxc::Grid::SetColumn(titleStack, 1);
        headerGrid.Children().Append(titleStack);
        g_hideActivity = wuxc::Button(); g_hideActivity.Content(winrt::box_value(L"Hide"));
        g_hideActivity.Padding({8, 4, 8, 4}); g_hideActivity.CornerRadius({6, 6, 6, 6});
        g_hideActivity.VerticalAlignment(wux::VerticalAlignment::Center);
        g_hideActivity.Click([](auto const&, auto const&) noexcept { try { HideCurrentActivity(); } catch (...) {} });
        wuxc::Grid::SetColumn(g_hideActivity, 2); headerGrid.Children().Append(g_hideActivity);

        wuxc::StackPanel navStack;
        navStack.Orientation(wuxc::Orientation::Horizontal);
        navStack.Spacing(4.0);
        navStack.VerticalAlignment(wux::VerticalAlignment::Center);
        navStack.Margin({8, 0, 0, 0});

        auto makeNavBtn = [](PCWSTR label, auto onClick) {
            wuxc::Button btn;
            btn.Content(winrt::box_value(label));
            btn.FontSize(11.5);
            btn.Padding({6, 4, 6, 5});
            btn.MinWidth(28.0);
            btn.CornerRadius({6, 6, 6, 6});
            btn.Click(onClick);
            return btn;
        };

        navStack.Children().Append(makeNavBtn(L"◀", [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            ShowPopupHolidayList(false);
            if (g_showingMonth) g_monthOffset = ClampVal(g_monthOffset - 1, -1200, 1200); else --g_popupDayOffset;
            RefreshExpandedPopupData();
            ResetPopupScrollPosition(g_popupHoroscopeScroll);
        }));
        navStack.Children().Append(makeNavBtn(L"●", [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            ShowPopupHolidayList(false);
            g_popupDayOffset = 0; g_monthOffset = 0;
            RefreshExpandedPopupData();
            ResetPopupScrollPosition(g_popupHoroscopeScroll);
        }));
        navStack.Children().Append(makeNavBtn(L"▶", [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            ShowPopupHolidayList(false);
            if (g_showingMonth) g_monthOffset = ClampVal(g_monthOffset + 1, -1200, 1200); else ++g_popupDayOffset;
            RefreshExpandedPopupData();
            ResetPopupScrollPosition(g_popupHoroscopeScroll);
        }));

        auto holidayListButton = makeNavBtn(L"Lễ", [](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            ShowPopupHolidayList(!g_popupShowingHolidays);
            RefreshExpandedPopupData();
        });
        navStack.Children().Append(holidayListButton);
        g_monthButton = makeNavBtn(L"Month", [](auto const&, auto const&) {
            g_showingMonth = !g_showingMonth; ShowPopupHolidayList(false); RefreshExpandedPopupData();
            ResetPopupScrollPosition(g_monthScroll);
        });
        navStack.Children().Append(g_monthButton);
        g_cultureButton = makeNavBtn(L"Culture", [](auto const&, auto const&) {
            SelectActivity(g_selectedActivity == ActivityKind::Culture ? ActivityKind::Lunar : ActivityKind::Culture);
        });
        navStack.Children().Append(g_cultureButton);


        for (int i = 0; i < 2; ++i) {
            wuxc::RowDefinition row; row.Height(wux::GridLengthHelper::Auto());
            headerGrid.RowDefinitions().Append(row);
        }
        wuxc::Grid::SetRow(navStack, 1); wuxc::Grid::SetColumn(navStack, 0); wuxc::Grid::SetColumnSpan(navStack, 3);
        navStack.Margin({0, 7, 0, 2});
        headerGrid.Children().Append(navStack);
        wuxc::Grid::SetRow(headerGrid, 0);
        mainGrid.Children().Append(headerGrid); g_popupHeader = headerGrid;

        wuxc::Border headerDiv;
        headerDiv.Height(1.0);
        headerDiv.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{35, 128, 128, 128}));
        headerDiv.Margin({0, 0, 0, 4});
        wuxc::Grid::SetRow(headerDiv, 1);
        mainGrid.Children().Append(headerDiv); g_popupDivider = headerDiv;

        wuxc::Border holidayBanner;
        holidayBanner.CornerRadius({6, 6, 6, 6});
        holidayBanner.Padding({8, 4, 8, 5});
        holidayBanner.Margin({0, 2, 0, 0});
        holidayBanner.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{30, 255, 100, 90}));
        holidayBanner.MinHeight(0);
        holidayBanner.Visibility(wux::Visibility::Collapsed);

        wuxc::TextBlock holidayTb;
        holidayTb.FontSize(12.0);
        holidayTb.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        holidayTb.TextWrapping(wux::TextWrapping::Wrap);
        holidayBanner.Child(holidayTb);
        bodyStack.Children().Append(holidayBanner);

        bodyStack.Children().Append(CreatePopupRowSection(g_popupRowBattu, true));
        bodyStack.Children().Append(CreatePopupRowSection(g_popupRowMenhTruc, true));
        bodyStack.Children().Append(CreatePopupRowSection(g_popupRowGioHoangDao, true));
        bodyStack.Children().Append(CreatePopupRowSection(g_popupRowNenKy, true));
        bodyStack.Children().Append(CreatePopupRowSection(g_popupRowXungHuong, false));

        // A ScrollViewer in the star row is measured with the remaining height.
        // A vertical StackPanel around it would measure infinity and prevent scrolling.
        auto makeScroll = []() {
            wuxc::ScrollViewer scroll;
            scroll.VerticalScrollMode(wuxc::ScrollMode::Enabled);
            scroll.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
            scroll.HorizontalScrollMode(wuxc::ScrollMode::Disabled);
            scroll.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
            scroll.ZoomMode(wuxc::ZoomMode::Disabled);
            scroll.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
            scroll.VerticalContentAlignment(wux::VerticalAlignment::Top);
            scroll.Padding({0, 0, 16, 4}); // Reserve space for the scrollbar thumb.
            scroll.IsTabStop(true);
            return scroll;
        };

        auto horoscopeScroll = makeScroll();
        horoscopeScroll.Content(bodyStack);
        wuxc::Grid::SetRow(horoscopeScroll, 2);
        mainGrid.Children().Append(horoscopeScroll);

        wuxc::TextBlock holidayListText;
        holidayListText.FontSize(12.2);
        holidayListText.LineHeight(19.0);
        holidayListText.TextWrapping(wux::TextWrapping::Wrap);
        holidayListText.Margin({4, 6, 4, 8});
        auto holidayScroll = makeScroll();
        holidayScroll.Content(holidayListText);
        holidayScroll.Visibility(wux::Visibility::Collapsed);
        wuxc::Grid::SetRow(holidayScroll, 2);
        mainGrid.Children().Append(holidayScroll);

        wuxc::StackPanel liveBody;
        liveBody.Spacing(0);
        liveBody.Margin({4, 4, 4, 0});
        g_popupLiveTitle = wuxc::TextBlock();
        g_popupLiveTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        g_popupLiveTitle.TextWrapping(wux::TextWrapping::Wrap);
        g_popupLiveDetail = wuxc::TextBlock();
        g_popupLiveDetail.FontSize(13);
        g_popupLiveDetail.TextWrapping(wux::TextWrapping::Wrap);
        g_popupLiveStatus = wuxc::TextBlock();
        g_popupLiveStatus.FontSize(12);
        g_popupLiveStatus.TextWrapping(wux::TextWrapping::Wrap);
        g_popupLiveProgress = CreateProgressView();
        g_popupMediaControls = wuxc::StackPanel();
        g_popupMediaControls.Orientation(wuxc::Orientation::Horizontal);
        g_popupMediaControls.HorizontalAlignment(wux::HorizontalAlignment::Center);
        g_popupMediaControls.Spacing(12);
        g_popupMediaPrevious = CreateMediaButton(L"\uE892", MediaAction::Previous, false);
        g_popupMediaPlayPause = CreateMediaButton(L"\uE768", MediaAction::PlayPause, true);
        g_popupMediaNext = CreateMediaButton(L"\uE893", MediaAction::Next, false);
        g_popupMediaControls.Spacing(16);
        for (auto const& button : {g_popupMediaPrevious, g_popupMediaPlayPause, g_popupMediaNext})
            g_popupMediaControls.Children().Append(button);
        liveBody.Children().Append(g_popupLiveTitle);
        liveBody.Children().Append(g_popupLiveDetail);
        liveBody.Children().Append(g_popupLiveProgress);
        g_mediaSeek = wuxc::Slider();
        g_mediaSeek.Minimum(0); g_mediaSeek.Maximum(1); g_mediaSeek.StepFrequency(1);
        g_mediaSeek.Margin({-8, -8, -8, -8}); g_mediaSeek.IsThumbToolTipEnabled(true);
        g_mediaSeek.ThumbToolTipValueConverter(winrt::make<PlaybackTimeConverter>());
        wux::Automation::AutomationProperties::SetName(g_mediaSeek, L"Playback position");
        g_mediaSeek.ValueChanged([](auto const&, auto const&) noexcept {
            if (g_shuttingDown || g_settingSeek) return;
            try {
                if (!g_draggingSeek) QueueSeek();
                if (g_mediaElapsed && g_mediaSeek) g_mediaElapsed.Text(FormatPlaybackTime(static_cast<int64_t>(g_mediaSeek.Value())));
            } catch (...) { Wh_Log(L"Seek UI update failed"); }
        });
        auto startSeek = wuxi::PointerEventHandler([](auto const&, auto const&) noexcept {
            if (g_shuttingDown) return;
            try { if (g_mediaSeek && g_mediaSeek.IsEnabled()) { g_draggingSeek = true; PromotePopupToManual(); } }
            catch (...) { g_draggingSeek = false; }
        });
        auto endSeek = wuxi::PointerEventHandler([](auto const&, auto const&) noexcept {
            if (g_shuttingDown) return;
            try { if (g_draggingSeek) { g_draggingSeek = false; QueueSeek(); } }
            catch (...) { g_draggingSeek = false; }
        });
        auto cancelSeek = wuxi::PointerEventHandler([](auto const&, auto const&) { g_draggingSeek = false; });
        g_mediaSeek.AddHandler(wux::UIElement::PointerPressedEvent(), winrt::box_value(startSeek), true);
        g_mediaSeek.AddHandler(wux::UIElement::PointerReleasedEvent(), winrt::box_value(endSeek), true);
        g_mediaSeek.AddHandler(wux::UIElement::PointerCaptureLostEvent(), winrt::box_value(endSeek), true);
        g_mediaSeek.AddHandler(wux::UIElement::PointerCanceledEvent(), winrt::box_value(cancelSeek), true);
        liveBody.Children().Append(g_mediaSeek);
        g_mediaTransport = wuxc::Grid();
        for (int i = 0; i < 3; ++i) {
            wuxc::ColumnDefinition column;
            column.Width(i == 1 ? wux::GridLengthHelper::Auto() : wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
            g_mediaTransport.ColumnDefinitions().Append(column);
        }
        g_mediaElapsed = wuxc::TextBlock(); g_mediaDuration = wuxc::TextBlock();
        g_mediaElapsed.FontSize(12); g_mediaDuration.FontSize(12);
        g_mediaElapsed.VerticalAlignment(wux::VerticalAlignment::Center);
        g_mediaDuration.VerticalAlignment(wux::VerticalAlignment::Center);
        g_mediaDuration.HorizontalAlignment(wux::HorizontalAlignment::Right);
        wuxc::Grid::SetColumn(g_popupMediaControls, 1); wuxc::Grid::SetColumn(g_mediaDuration, 2);
        g_mediaTransport.Children().Append(g_mediaElapsed);
        g_mediaTransport.Children().Append(g_popupMediaControls);
        g_mediaTransport.Children().Append(g_mediaDuration);
        liveBody.Children().Append(g_mediaTransport);
        g_mediaSessionButtons = wuxc::StackPanel(); g_mediaSessionButtons.Orientation(wuxc::Orientation::Horizontal);
        g_mediaSessionButtons.Spacing(6); g_mediaSessionButtons.HorizontalAlignment(wux::HorizontalAlignment::Center);
        g_mediaSessionScroll = wuxc::ScrollViewer(); g_mediaSessionScroll.Content(g_mediaSessionButtons);
        g_mediaSessionScroll.VerticalContentAlignment(wux::VerticalAlignment::Top);
        g_mediaSessionScroll.HorizontalScrollMode(wuxc::ScrollMode::Enabled);
        g_mediaSessionScroll.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
        g_mediaSessionScroll.VerticalScrollMode(wuxc::ScrollMode::Disabled);
        g_mediaSessionScroll.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
        liveBody.Children().Append(g_mediaSessionScroll);
        g_taskProgressList = wuxc::StackPanel(); g_taskProgressList.Spacing(6);
        liveBody.Children().Append(g_taskProgressList);
        g_clockList=wuxc::StackPanel();g_clockList.Spacing(4);
        liveBody.Children().Append(g_clockList);

        g_privacySelector = wuxc::StackPanel(); g_privacySelector.Orientation(wuxc::Orientation::Horizontal); g_privacySelector.Spacing(6);
        g_privacyAppList = wuxc::StackPanel(); g_privacyAppList.Spacing(6);
        liveBody.Children().Append(g_privacyAppList); liveBody.Children().Append(g_privacySelector);
        g_batteryTools = wuxc::StackPanel(); g_batteryTools.Spacing(0);
        g_powerAcCombo = wuxc::ComboBox(); g_powerDcCombo = wuxc::ComboBox();
        for (auto combo : {g_powerAcCombo, g_powerDcCombo}) {
            combo.HorizontalAlignment(wux::HorizontalAlignment::Stretch); combo.MinWidth(0); combo.Margin({0, 0, 0, 8});
            g_batteryTools.Children().Append(combo);
        }
        g_powerAcCombo.SelectionChanged([](auto const&, auto const&) {
            if (g_updatingPowerModes || g_powerAcCombo.SelectedIndex() < 0) return;
            PromotePopupToManual(); g_powerModeRequest = g_powerAcCombo.SelectedIndex();
            if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
        });
        g_powerDcCombo.SelectionChanged([](auto const&, auto const&) {
            if (g_updatingPowerModes || g_powerDcCombo.SelectedIndex() < 0) return;
            PromotePopupToManual(); g_powerModeRequest = 3 + g_powerDcCombo.SelectedIndex();
            if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
        });
        g_powerModeStatus = WrappedText(L"", 11); g_batteryTools.Children().Append(g_powerModeStatus);
        g_usageButton = wuxc::Button(); g_usageButton.CornerRadius({6, 6, 6, 6}); g_usageButton.Margin({0, 0, 0, 8});
        g_usageButton.Click([](auto const&, auto const&) {
            PromotePopupToManual(); g_showUsage = !g_showUsage; g_forcePowerRefresh = true;
            if (g_liveWakeEvent) SetEvent(g_liveWakeEvent); RequestPopupRefresh();
        });
        g_batteryTools.Children().Append(g_usageButton);
        g_usageList = wuxc::StackPanel(); g_usageList.Spacing(6); g_batteryTools.Children().Append(g_usageList);
        g_obsControls = wuxc::StackPanel(); g_obsControls.Orientation(wuxc::Orientation::Horizontal);
        g_obsControls.Spacing(10); g_obsControls.Margin({0, 12, 0, 8});
        g_obsControls.HorizontalAlignment(wux::HorizontalAlignment::Center);
        g_obsPause = wuxc::Button(); g_obsStop = wuxc::Button();
        for (auto const& button : {g_obsPause, g_obsStop}) {
            button.CornerRadius({8, 8, 8, 8}); button.Padding({14, 9, 14, 9}); g_obsControls.Children().Append(button);
        }
        g_obsPause.Click([](auto const&, auto const&) {
            auto obs = ReadLiveSnapshot().obs; QueueObsCommand(obs.paused ? ObsAction::Resume : ObsAction::Pause);
        });
        g_obsStop.Click([](auto const&, auto const&) { QueueObsCommand(ObsAction::Stop); });
        g_systemDetails = wuxc::StackPanel(); g_systemDetails.Spacing(6);
        g_systemOptions = wuxc::StackPanel(); g_systemOptions.Orientation(wuxc::Orientation::Horizontal); g_systemOptions.Spacing(8);
        g_systemSettingsButton = wuxc::Button(); g_projectionButton = wuxc::Button(); g_hotspotStopButton = wuxc::Button();
        for (auto button : {g_systemSettingsButton, g_projectionButton, g_hotspotStopButton}) {
            button.CornerRadius({7, 7, 7, 7}); button.Padding({10, 7, 10, 7}); g_systemOptions.Children().Append(button);
        }
        g_systemSettingsButton.Click([](auto const&, auto const&) noexcept { OpenSystemActivitySettings(false); });
        g_projectionButton.Click([](auto const&, auto const&) noexcept { OpenSystemActivitySettings(true); });
        g_hotspotStopButton.Click([](auto const&, auto const&) noexcept {
            try {
                PromotePopupToManual();
                if (g_uiLive.hotspot.on && !g_uiLive.hotspot.commandPending && !g_hotspotStopRequested.exchange(true)) {
                    { std::lock_guard<std::mutex> lock(g_liveMutex); g_hotspotSnapshot.commandPending = true; }
                    if (g_systemWake) SetEvent(g_systemWake);
                    g_hotspotStopButton.IsEnabled(false); NotifyLiveUi();
                }
            } catch (...) {}
        });
        liveBody.Children().Append(g_systemDetails); liveBody.Children().Append(g_systemOptions);
        liveBody.Children().Append(g_obsControls);
        liveBody.Children().Append(g_batteryTools);
        liveBody.Children().Append(g_popupLiveStatus);
        g_popupLiveScroll = makeScroll();
        g_popupLiveScroll.Content(liveBody);
        g_popupLiveScroll.Visibility(wux::Visibility::Collapsed);
        wuxc::Grid::SetRow(g_popupLiveScroll, 2);
        mainGrid.Children().Append(g_popupLiveScroll);

        g_monthBody = wuxc::StackPanel(); g_monthBody.Spacing(6);
        g_monthScroll = makeScroll(); g_monthScroll.Content(g_monthBody); g_monthScroll.Visibility(wux::Visibility::Collapsed);
        wuxc::Grid::SetRow(g_monthScroll, 2); mainGrid.Children().Append(g_monthScroll);
        g_cultureBody = wuxc::StackPanel(); g_cultureBody.Spacing(8);
        g_cultureSearch = wuxc::TextBox(); g_cultureSearch.MaxLength(120);
        g_cultureSearch.TextChanged([](auto const&, auto const&) { RefreshCultureResults(); });
        g_cultureBody.Children().Append(g_cultureSearch);
        g_cultureResults = wuxc::StackPanel(); g_cultureResults.Spacing(6); g_cultureBody.Children().Append(g_cultureResults);
        g_cultureScroll = makeScroll(); g_cultureScroll.Content(g_cultureBody); g_cultureScroll.Visibility(wux::Visibility::Collapsed);
        wuxc::Grid::SetRow(g_cultureScroll, 2); mainGrid.Children().Append(g_cultureScroll);
        g_bluetoothList = wuxc::StackPanel(); g_bluetoothList.Spacing(8);
        g_batteryTools.Children().Append(g_bluetoothList);
        g_popupTabs = wuxc::StackPanel();
        g_popupTabs.Orientation(wuxc::Orientation::Horizontal);
        g_popupTabs.Spacing(6);
        wuxc::ScrollViewer tabsScroll;
        tabsScroll.HorizontalScrollMode(wuxc::ScrollMode::Enabled);
        tabsScroll.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Hidden);
        tabsScroll.VerticalScrollMode(wuxc::ScrollMode::Disabled);
        tabsScroll.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
        tabsScroll.ZoomMode(wuxc::ZoomMode::Disabled);
        tabsScroll.Margin({4, 0, 4, 0});
        tabsScroll.Content(g_popupTabs);
        g_popupTabsScroll = tabsScroll;
        g_tabsHost = wuxc::Grid(); g_tabsHost.Margin({0, 8, 0, 0});
        for (int i = 0; i < 5; ++i) { wuxc::ColumnDefinition col;
            col.Width(i == 1 ? wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star) : wux::GridLengthHelper::Auto());
            g_tabsHost.ColumnDefinitions().Append(col); }
        g_tabsPrevious = makeNavBtn(L"◀", [](auto const&, auto const&) { ScrollActivityTabs(-1); });
        g_tabsNext = makeNavBtn(L"▶", [](auto const&, auto const&) { ScrollActivityTabs(1); });
        wux::Automation::AutomationProperties::SetName(g_tabsPrevious, L"Scroll activities left");
        wux::Automation::AutomationProperties::SetName(g_tabsNext, L"Scroll activities right");
        wuxc::Grid::SetColumn(tabsScroll, 1); wuxc::Grid::SetColumn(g_tabsNext, 2);
        g_tabsHost.Children().Append(g_tabsPrevious); g_tabsHost.Children().Append(tabsScroll); g_tabsHost.Children().Append(g_tabsNext);
        g_quickSettings = makeNavBtn(L"", [](auto const&, auto const&) noexcept { try { ShowQuickSettings(); } catch (...) {} });
        g_quickSettings.Content(FluentIcon(L"\uE713", 17)); g_quickSettings.Margin({6, 0, 0, 0});
        g_collapseTaskbar = makeNavBtn(L"", [](auto const&, auto const&) noexcept {
            try { PromotePopupToManual(); auto value=GetSettings().collapsedTaskbar ? L"0" : L"1";
                { std::lock_guard<std::mutex> lock(g_configStorageMutex); if(!Wh_SetStringValue(L"config.collapsedTaskbar",value)) return; }
                LoadSettings(); ++g_settingsRevision; RefreshTaskbarWidgetData(); RequestPopupRefresh();
            } catch (...) {}
        });
        g_collapseTaskbar.Content(FluentIcon(L"\uE73F",17)); g_collapseTaskbar.Margin({6,0,0,0});
        wuxc::Grid::SetColumn(g_collapseTaskbar,3); g_tabsHost.Children().Append(g_collapseTaskbar);
        wuxc::Grid::SetColumn(g_quickSettings, 4); g_tabsHost.Children().Append(g_quickSettings);
        tabsScroll.ViewChanged([](auto const&, auto const&) { UpdateTabArrows(); });
        tabsScroll.SizeChanged([](auto const&, auto const&) { UpdateTabArrows(); });
        g_popupTabs.SizeChanged([](auto const&, auto const&) { UpdateTabArrows(); });
        wuxc::Grid::SetRow(g_tabsHost, 3); mainGrid.Children().Append(g_tabsHost);
        g_compactNotice = wuxc::Border(); g_compactNotice.Padding({4, 6, 4, 4});
        g_noticeText = WrappedText(L"", 13); g_noticeText.MaxLines(6); g_noticeText.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        g_compactNotice.Child(g_noticeText); g_compactNotice.Visibility(wux::Visibility::Collapsed);
        wuxc::Grid::SetRow(g_compactNotice, 2); mainGrid.Children().Append(g_compactNotice);
        g_popupCalendarNavigation = navStack;
        g_renderedTabs.clear();
        g_renderedTabLanguage = -1;

        rootCard.Child(mainGrid);
        wuxc::Grid popupLayers;
        wuxc::Border backdropLayer;
        backdropLayer.IsHitTestVisible(false);
        popupLayers.Children().Append(backdropLayer);
        popupLayers.Children().Append(rootCard);
        // Above the backdrop and content; the strokes never intercept input.
        g_popupGlassRim = wuxc::Border(); g_popupGlassRim.IsHitTestVisible(false);
        g_popupGlassRim.CornerRadius({9,9,9,9}); g_popupGlassRim.BorderThickness({1.5,1.5,1.5,1.5});
        g_popupGlassRim.Visibility(wux::Visibility::Collapsed);
        popupLayers.Children().Append(g_popupGlassRim);
        g_popupGlassInnerRim = wuxc::Border(); g_popupGlassInnerRim.IsHitTestVisible(false);
        g_popupGlassInnerRim.Margin({2,2,2,2}); g_popupGlassInnerRim.CornerRadius({7,7,7,7});
        g_popupGlassInnerRim.BorderThickness({0.75,0.75,0.75,0.75});
        g_popupGlassInnerRim.Visibility(wux::Visibility::Collapsed);
        popupLayers.Children().Append(g_popupGlassInnerRim);
        g_popupXamlSource.Content(popupLayers);
        g_popupBackdropLayer = backdropLayer;
        backdropLayer.SizeChanged([](auto const&, auto const&) noexcept { SizeGlassLens(); });
        rootCard.PointerMoved([](auto const& sender, wuxi::PointerRoutedEventArgs const& args) noexcept {
            try {
                if (!g_glassHighlight || !g_effectsAllowed.load()) return;
                auto card = sender.template as<wux::UIElement>();
                auto point = args.GetCurrentPoint(card).Position();
                auto width = g_popupBackdropLayer.ActualWidth(), height = g_popupBackdropLayer.ActualHeight();
                if (width > 0 && height > 0) g_glassHighlight.EllipseCenter({static_cast<float>(point.X/width),static_cast<float>(point.Y/height)});
            } catch (...) {}
        });

        g_popupRootCard = rootCard;
        g_popupBadgeBorder = badgeBorder;
        g_popupZodiacText = zodiacTb;
        g_popupTitleSolar = titleSolar;
        g_popupTitleLunar = titleLunar;
        g_popupHolidayBanner = holidayBanner;
        g_popupHolidayText = holidayTb;
        g_popupHoroscopeScroll = horoscopeScroll;
        g_popupHolidayScroll = holidayScroll;
        g_popupHolidayListText = holidayListText;
        g_popupHolidayListButton = holidayListButton;
        g_popupHolidayListLanguage = -1;
        ShowPopupHolidayList(false);

        if (g_popupXamlChild) {
            RECT rc{};
            GetClientRect(hwnd, &rc);
            SetWindowPos(g_popupXamlChild, nullptr, 0, 0,
                         rc.right - rc.left, rc.bottom - rc.top,
                         SWP_SHOWWINDOW | SWP_NOACTIVATE);
        }
        return true;
    } catch (...) {
        Wh_Log(L"InitializePopupXamlHost failed: 0x%08X", static_cast<unsigned>(winrt::to_hresult()));
        ReleasePopupScrollControls();
        ClearPopupCompositionBackdrop();
        g_popupBackdropLayer = nullptr;
    g_popupGlassRim = nullptr; g_popupGlassInnerRim = nullptr;
        if (g_popupXamlSource) {
            try { g_popupXamlSource.Close(); } catch (...) {}
            g_popupXamlSource = nullptr;
        }
        g_popupXamlChild = nullptr;
        return false;
    }
}

// Animate the content without disconnecting the separate HostBackdrop visual.
// Replacing an animation starts from its current presentation value, so quick
// close/open clicks reverse smoothly instead of jumping back to a hidden card.
bool AnimatePopupTransition(bool opening, bool freshOpen = false) {
    if (!g_popupRootCard) return false;
    wcomp::Visual visual{nullptr};
    try {
        g_popupRootCard.UpdateLayout();
        visual = wuxh::ElementCompositionPreview::GetElementVisual(g_popupRootCard);
        auto compositor = visual.Compositor();
        auto easing = compositor.CreateCubicBezierEasingFunction(
            opening ? wf::Numerics::float2{0.16f, 1.0f} : wf::Numerics::float2{0.4f, 0.0f},
            opening ? wf::Numerics::float2{0.3f, 1.0f} : wf::Numerics::float2{1.0f, 1.0f});

        if (freshOpen) {
            visual.StopAnimation(L"Opacity");
            visual.StopAnimation(L"Scale");
            visual.Opacity(0.0f);
            visual.Scale({0.96f, 0.96f, 1.0f});
        }
        visual.CenterPoint({
            static_cast<float>(g_popupRootCard.ActualWidth() / 2.0),
            g_popupOpensDown ? 0.0f
                            : static_cast<float>(g_popupRootCard.ActualHeight()),
            0.0f
        });
        auto duration = std::chrono::milliseconds{
            opening ? kPopupOpenDurationMs : kPopupCloseDurationMs};
        auto fade = compositor.CreateScalarKeyFrameAnimation();
        fade.Duration(duration);
        fade.InsertExpressionKeyFrame(0.0f, L"this.StartingValue");
        fade.InsertKeyFrame(1.0f, opening ? 1.0f : 0.0f, easing);
        auto zoom = compositor.CreateVector3KeyFrameAnimation();
        zoom.Duration(duration);
        zoom.InsertExpressionKeyFrame(0.0f, L"this.StartingValue");
        zoom.InsertKeyFrame(1.0f, opening ? wf::Numerics::float3{1.0f, 1.0f, 1.0f}
                                        : wf::Numerics::float3{0.96f, 0.96f, 1.0f}, easing);
        visual.StartAnimation(L"Opacity", fade);
        visual.StartAnimation(L"Scale", zoom);
        return true;
    } catch (...) {
        if (visual) {
            try {
                visual.StopAnimation(L"Opacity");
                visual.StopAnimation(L"Scale");
                visual.Opacity(1.0f);
                visual.Scale({1.0f, 1.0f, 1.0f});
            } catch (...) {}
        }
        Wh_Log(L"Animation unavailable; using immediate visibility change.");
        return false;
    }
}

void CancelPopupClose(HWND hwnd) {
    if (hwnd) KillTimer(hwnd, kPopupCloseTimerId);
    g_popupClosing = false;
    g_popupCloseDeadline = 0;
}

void HideExpandedPopupImmediately(HWND hwnd) {
    g_renderedPageKey.clear();
    g_batteryPageOpen = false;
    CancelPopupClose(hwnd);
    if (hwnd) KillTimer(hwnd, kPopupActivityTimerId);

    if (g_culturePageOpen) ReleaseCulturePage();
    if (g_selectedActivity == ActivityKind::Culture) g_selectedActivity = ActivityKind::Lunar;
    g_showUsage = false;
    g_popupMode = PopupMode::Hidden;
    g_popupAutoDeadline = 0;
    if (hwnd && IsWindow(hwnd)) ShowWindow(hwnd, SW_HIDE);
    if (!g_shuttingDown) RefreshTaskbarWidgetData();
    UpdateWidgetHoverVisual();
}

void BeginCloseExpandedPopup(HWND hwnd) {
    if (!hwnd || !IsWindowVisible(hwnd) || g_popupClosing) return;
    g_popupClosing = true;
    if (!AnimatePopupTransition(false)) {
        HideExpandedPopupImmediately(hwnd);
        return;
    }
    g_popupCloseDeadline = GetTickCount64() + kPopupCloseDurationMs;
    // Asynchronous: keep the XAML island alive for the closing transition.
    if (!SetTimer(hwnd, kPopupCloseTimerId, kPopupCloseDurationMs, nullptr)) {
        HideExpandedPopupImmediately(hwnd);
    }
}

void PromotePopupToManual() {
    if (g_popupMode != PopupMode::Automatic) return;
    g_popupMode = PopupMode::Manual;
    g_popupAutoDeadline = 0;
    if (g_popupClosing) {
        CancelPopupClose(g_expandedPopup);
        AnimatePopupTransition(true, false);
    }
    g_batteryNeedsAttention = false;
    RequestPopupRefresh();
    if (g_expandedPopup) SetForegroundWindow(g_expandedPopup);
}

void PollPopupInteraction(HWND hwnd) {
    if (g_quickMenuOpen) return;
    if (!IsWindowVisible(hwnd) || g_popupMode == PopupMode::Hidden) return;
    const int primaryButton = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
    const bool down = (GetAsyncKeyState(primaryButton) & 0x8000) != 0;
    if (down && !g_popupLastPrimaryDown) {
        POINT point{};
        RECT popup{}, widget{};
        if (GetCursorPos(&point) && GetWindowRect(hwnd, &popup)) {
            if (PtInRect(&popup, point)) PromotePopupToManual();
            else if (!(g_powerAcCombo && g_powerAcCombo.IsDropDownOpen()) &&
                     !(g_powerDcCombo && g_powerDcCombo.IsDropDownOpen()) &&
                     !(GetElementScreenRect(g_playerGrid, widget) && PtInRect(&widget, point)))
                BeginCloseExpandedPopup(hwnd);
        }
    }
    g_popupLastPrimaryDown = down;
    if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) BeginCloseExpandedPopup(hwnd);
    if (g_popupMode == PopupMode::Automatic && g_popupAutoDeadline != 0 &&
        GetTickCount64() >= g_popupAutoDeadline) BeginCloseExpandedPopup(hwnd);
}

void ProcessLiveActivityAlerts() {
    if (g_shuttingDown) return;
    auto settings = GetSettings();
    if (!settings.enableTaskbarWidget) return;
    auto systemAlert = std::exchange(g_pendingSystemAlert, ActivityKind::None);
    bool newPrivacy = g_privacyAlerts.Observe(settings.enablePrivacyWidget ? g_uiLive.privacy.uses : std::vector<PrivacyUse>{}, GetTickCount64());
    static std::set<uint64_t> announcedTasks;
    bool newTask = false;
    for (auto const& task : g_uiLive.tasks) {
        bool ready = task.state != 0;
        if (settings.enableTaskProgress && ready && announcedTasks.insert(task.id).second) newTask = true;
    }
    std::erase_if(announcedTasks, [&](auto id) {
        return std::none_of(g_uiLive.tasks.begin(),g_uiLive.tasks.end(),[&](auto const& task){return task.id==id;});
    });
    g_alertPrivacyUses = g_uiLive.privacy.uses; g_alertTasks = g_uiLive.tasks;
    auto alert = settings.enableBatteryWidget ? g_batteryAlertTracker.Observe(g_uiLive.battery) : BatteryAlert::None;
    if (g_uiLive.battery.valid && (g_uiLive.battery.charging || g_uiLive.battery.percent > 22)) g_batteryNeedsAttention = false;
    if (alert != BatteryAlert::None) {
        g_batteryNotice = alert; g_batteryNeedsAttention = alert != BatteryAlert::Charging;
    }
    auto bluetoothAlert = settings.enableBatteryWidget && settings.bluetoothBattery ?
        g_bluetoothAlerts.Observe(g_uiLive.bluetooth) : BluetoothDeviceBattery{};
    if (!bluetoothAlert.id.empty() && settings.enableBatteryWidget && settings.bluetoothBattery) {
        g_bluetoothNotice = bluetoothAlert; g_bluetoothNoticeUntil = GetTickCount64() + 5000;
        g_hiddenActivities.erase(ActivityKind::Battery);
    }
    if (!g_playerGrid) return;
    ActivityKind show = ActivityKind::None;
    if ((systemAlert == ActivityKind::Display && settings.displayAutoPopup) ||
        (systemAlert == ActivityKind::Hotspot && settings.hotspotAutoPopup)) show = systemAlert;
    static uint64_t lastTaskNotice = 0;
    bool finishedTask = settings.enableTaskProgress && HasTaskNotice(g_uiLive) && g_uiLive.taskNotice.serial != lastTaskNotice;
    lastTaskNotice = g_uiLive.taskNotice.serial;
    if ((newTask || finishedTask) && settings.taskProgressAutoPopup) show = ActivityKind::Download;
    if (alert != BatteryAlert::None && settings.batteryAutoPopup) show = ActivityKind::Battery;
    if (!bluetoothAlert.id.empty() && settings.batteryAutoPopup) show = ActivityKind::Battery;
    if (newPrivacy && settings.privacyAutoPopup) show = ActivityKind::Privacy;
    static uint64_t lastClockNotice=0;
    if(settings.enableClockWidget && settings.clockAutoPopup && g_uiLive.clock.noticeSerial!=lastClockNotice && GetTickCount64()<g_uiLive.clock.noticeUntil)
        show=ActivityKind::Clock;
    lastClockNotice=g_uiLive.clock.noticeSerial;
    static uint64_t announcedObsStop = 0;
    bool stoppedObs = settings.enableObsWidget && g_uiLive.obs.stoppedSerial != announcedObsStop && GetTickCount64() < g_uiLive.obs.stoppedUntil;
    announcedObsStop = g_uiLive.obs.stoppedSerial;
    if (stoppedObs) show = ActivityKind::Obs;
    static uint64_t lastRemote = 0;
    if(settings.remoteAlerts && g_uiLive.remote.serial && g_uiLive.remote.serial != lastRemote) {
        lastRemote=g_uiLive.remote.serial; show=ActivityKind::Remote;
    }
    static TemperatureAlerts cpuAlerts,gpuAlerts;
    if(settings.temperatureAlerts && (cpuAlerts.Observe(g_uiLive.temperature.cpu) | gpuAlerts.Observe(g_uiLive.temperature.gpu))) show=ActivityKind::Memory;
    static MemoryAlerts memoryAlerts;
    if (settings.memoryAlerts && g_uiLive.memory.valid && memoryAlerts.Observe(g_uiLive.memory.percent)) show = ActivityKind::Memory;
    if (show != ActivityKind::None) g_hiddenActivities.erase(show);
    bool manual = g_popupMode == PopupMode::Manual && g_expandedPopup && IsWindowVisible(g_expandedPopup);
    if (manual && !(stoppedObs && g_selectedActivity == ActivityKind::Obs)) { if (show != ActivityKind::None) RefreshExpandedPopupData(); return; }
    if (show != ActivityKind::None) {
        g_selectedActivity = show; OpenExpandedPopup(true); RefreshTaskbarWidgetData();
    } else if ((newTask || systemAlert != ActivityKind::None) && settings.autoSelectActivity && g_popupMode != PopupMode::Automatic) {
        g_selectedActivity = newTask ? ActivityKind::Download : systemAlert; RefreshTaskbarWidgetData();
    }
}

bool IsSourceInsideButton(wf::IInspectable const& source) {
    auto element = source.try_as<wux::DependencyObject>();
    for (int depth = 0; element && depth < 16; ++depth) {
        if (element.try_as<wuxc::Button>()) return true;
        element = wuxm::VisualTreeHelper::GetParent(element);
    }
    return false;
}

LRESULT CALLBACK ExpandedPopupWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    try {
        switch (msg) {
            case WM_THEMECHANGED:
            case WM_SETTINGCHANGE:
            case WM_DWMCOMPOSITIONCHANGED:
                g_popupBackdropDirty = true;
                if (IsWindowVisible(hwnd)) RefreshExpandedPopupData();
                break;

            case WM_SHOWWINDOW:
                if (!wParam) {
                    CancelPopupClose(hwnd);
                    KillTimer(hwnd, kPopupActivityTimerId);
                } else g_popupBackdropDirty = true;
                break;

            case WM_CLOSE:
                BeginCloseExpandedPopup(hwnd);
                return 0;

            case WM_TIMER:
                if (wParam == kPopupActivityTimerId) {
                    PollPopupInteraction(hwnd);
                    if(IsWindowVisible(hwnd) && SamplePopupContrast()) RefreshExpandedPopupData();
                    return 0;
                }
                if (wParam == kPopupCloseTimerId) {
                    // A queued timer can outlive KillTimer. The phase and deadline
                    // also protect a reopened popup and a later closing transition.
                    if (g_popupClosing && GetTickCount64() >= g_popupCloseDeadline)
                        HideExpandedPopupImmediately(hwnd);
                    return 0;
                }
                break;

            case WM_NCDESTROY:
                KillTimer(hwnd, kPopupActivityTimerId);
                g_popupMode = PopupMode::Hidden;
                CancelPopupClose(hwnd);
                break;

            case WM_SIZE:
                if (g_popupXamlChild) {
                    MoveWindow(g_popupXamlChild, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
                }
                return 0;

            case WM_ACTIVATE: {
                if (LOWORD(wParam) == WA_INACTIVE) {
                    POINT cursor{};
                    RECT widgetRect{};
                    const int primaryButton = GetSystemMetrics(SM_SWAPBUTTON)
                        ? VK_RBUTTON : VK_LBUTTON;
                    const bool clickingWidget =
                        (GetAsyncKeyState(primaryButton) & 0x8000) != 0 &&
                        GetCursorPos(&cursor) &&
                        GetElementScreenRect(g_playerGrid, widgetRect) &&
                        PtInRect(&widgetRect, cursor);

                    // Tapped will toggle a click on the widget. Hiding here first
                    // would make the same click reopen the popup immediately.
                    if (!clickingWidget && !g_quickMenuOpen && g_popupMode == PopupMode::Manual) {
                        BeginCloseExpandedPopup(hwnd);
                    }
                } else {
                    g_popupBackdropDirty = true;
                }
                break; // Let DefWindowProc assign keyboard focus on activation.
            }

            case WM_SYSKEYDOWN:
            case WM_KEYDOWN:
                if (wParam == VK_ESCAPE) {
                    BeginCloseExpandedPopup(hwnd);
                }
                return 0;
        }
    } catch (...) { Wh_Log(L"Popup event failed; waiting for next update"); }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void OpenExpandedPopup(bool automatic) {
    if (g_shuttingDown) return;
    if (automatic) {
        auto state = ReadLiveSnapshot();
        if ((g_selectedActivity==ActivityKind::Clock && GetTickCount64()>=state.clock.noticeUntil) ||
            (g_selectedActivity == ActivityKind::Download && state.tasks.empty() && !HasTaskNotice(state)) ||
            (g_selectedActivity == ActivityKind::Privacy && state.privacy.uses.empty()) ||
            (g_selectedActivity == ActivityKind::Obs && (state.obs.recording || GetTickCount64() >= state.obs.stoppedUntil)) ||
            (g_selectedActivity != ActivityKind::Clock && g_selectedActivity != ActivityKind::Obs && g_selectedActivity != ActivityKind::Download && g_selectedActivity != ActivityKind::Privacy &&
             g_selectedActivity != ActivityKind::Battery && g_selectedActivity != ActivityKind::Display &&
             g_selectedActivity != ActivityKind::Hotspot && g_selectedActivity != ActivityKind::Memory && g_selectedActivity != ActivityKind::Remote)) return;
    }
    const bool wasVisible = g_expandedPopup && IsWindowVisible(g_expandedPopup);
    if (!g_expandedPopup || !IsWindow(g_expandedPopup)) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = ExpandedPopupWndProc;
        wc.hInstance = ThisModule();
        wc.lpszClassName = kExpandedPopupWindowClass;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        // Replace a stale registration before assigning this module's WndProc.
        UnregisterClassW(kExpandedPopupWindowClass, wc.hInstance);
        if (!RegisterClassExW(&wc)) {
            Wh_Log(L"Popup class registration failed: %lu", GetLastError());
            return;
        }

        g_expandedPopup = CreateWindowExW(
            // Keep a redirected host surface for the popup's composition backdrop.
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
            kExpandedPopupWindowClass,
            L"",
            WS_POPUP,
            0, 0, 540, kExpandedPopupHeightDip,
            g_taskbarWnd, nullptr, wc.hInstance, nullptr);

        if (!g_expandedPopup) Wh_Log(L"Popup creation failed: %lu", GetLastError());
        if (!g_expandedPopup || !InitializePopupXamlHost(g_expandedPopup)) {
            if (g_expandedPopup) {
                DestroyWindow(g_expandedPopup);
                g_expandedPopup = nullptr;
            }
            return;
        }
    }

    g_popupMode = automatic ? PopupMode::Automatic : PopupMode::Manual;
    g_popupAutoDeadline = automatic ? GetTickCount64() + (g_selectedActivity == ActivityKind::Battery ? GetSettings().batteryAutoPopupSeconds * 1000 : 4000) : 0;
    g_popupDayOffset = 0;
    ShowPopupHolidayList(false);
    g_lastPopupWindowRect = {};
    RefreshExpandedPopupData();
    if (automatic && ((g_selectedActivity == ActivityKind::Download && g_uiLive.tasks.empty() && !HasTaskNotice(g_uiLive)) ||
                      (g_selectedActivity == ActivityKind::Privacy && g_uiLive.privacy.uses.empty()))) return;

    ResetPopupScrollPosition(g_popupHoroscopeScroll);
    ResetPopupScrollPosition(g_popupHolidayScroll);
    CancelPopupClose(g_expandedPopup);
    AnimatePopupTransition(true, !wasVisible);
    if(!wasVisible) {PositionExpandedPopup();g_popupContrastValid=false;SamplePopupContrast(true);RefreshExpandedPopupData();}
    ShowWindow(g_expandedPopup, automatic ? SW_SHOWNOACTIVATE : SW_SHOW);
    PositionExpandedPopup();
    if (!automatic) SetForegroundWindow(g_expandedPopup);
    const int primaryButton = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
    g_popupLastPrimaryDown = (GetAsyncKeyState(primaryButton) & 0x8000) != 0;
    if (!SetTimer(g_expandedPopup, kPopupActivityTimerId, 100, nullptr) && automatic) {
        HideExpandedPopupImmediately(g_expandedPopup);
        return;
    }
    // Activation/visibility can reset Explorer-owned transient materials.
    // Apply again with a visible, sized HWND instead of caching the pre-show call.
    g_popupBackdropDirty = true;
    RefreshExpandedPopupData();
    UpdateWidgetHoverVisual();
}

void ToggleExpandedPopup() {
    if (g_shuttingDown) return;
    if (g_selectedActivity == ActivityKind::None) { ShowQuickSettings(); return; }
    if (g_expandedPopup && IsWindowVisible(g_expandedPopup)) {
        if (g_popupMode == PopupMode::Automatic) {
            PromotePopupToManual();
            RefreshTaskbarWidgetData();
        } else if (g_popupClosing) {
            CancelPopupClose(g_expandedPopup);
            AnimatePopupTransition(true);
            SetForegroundWindow(g_expandedPopup);
        } else BeginCloseExpandedPopup(g_expandedPopup);
        return;
    }
    OpenExpandedPopup(false);
}

void RemoveInjectedTaskbarWidget() {
    ReleaseWidgetMaterial();
    if (auto window = g_liveUiWindow.load()) KillTimer(window, kObsClockTimerId);
    g_obsClockTimer = false;
    // Settings changes and unload must not leave a timer targeting this DLL.
    HideExpandedPopupImmediately(g_expandedPopup);

    if (g_injectedParentPanel) {
        try {
            uint32_t idx = 0;
            if (g_playerGrid && g_injectedParentPanel.Children().IndexOf(g_playerGrid, idx)) {
                g_injectedParentPanel.Children().RemoveAt(idx);
            }
            auto grid = g_injectedParentPanel.try_as<wuxc::Grid>();
            if (grid && g_injectedColumnDef) {
                uint32_t colIdx = 0;
                if (grid.ColumnDefinitions().IndexOf(g_injectedColumnDef, colIdx)) {
                    // Undo our insertion. Otherwise repeated position changes shift
                    // the native tray controls farther right on every update.
                    for (auto const& saved : g_shiftedTrayChildren) {
                        // Restore only properties that this mod successfully changed.
                        // Do not overwrite a later, different change by another mod.
                        if (auto element = saved.element.get()) {
                            if (saved.moved && wuxc::Grid::GetColumn(element) == saved.column + 1)
                                wuxc::Grid::SetColumn(element, saved.column);
                            if (saved.widened && wuxc::Grid::GetColumnSpan(element) == saved.span + 1)
                                wuxc::Grid::SetColumnSpan(element, saved.span);
                        }
                    }
                    grid.ColumnDefinitions().RemoveAt(colIdx);
                }
            }
            if (grid && g_injectedImplicitColumnDef) {
                uint32_t colIdx = 0;
                if (grid.ColumnDefinitions().IndexOf(g_injectedImplicitColumnDef, colIdx))
                    grid.ColumnDefinitions().RemoveAt(colIdx);
            }
        } catch (...) {}
    }

    g_glassButtonTemplate = nullptr;g_glassTemplateFailed=false;
    g_playerGrid = nullptr;
    g_widgetBadgeBorder = nullptr;
    g_widgetZodiacText = nullptr;
    g_widgetLine1Text = nullptr;
    g_widgetLunarHeaderView = nullptr;
    g_widgetLiveTitle = nullptr;
    g_widgetLine2Text = nullptr;
    g_widgetPreviousColumn = nullptr; g_widgetNextColumn = nullptr;
    g_widgetPreviousButton = nullptr;
    g_widgetNextButton = nullptr;
    g_injectedParentPanel = nullptr;
    g_injectedColumnDef = nullptr;
    g_injectedImplicitColumnDef = nullptr;
    g_shiftedTrayChildren.clear();
    g_widgetHovered = false;
    g_currentInjectedPosition.clear();
    g_widgetRotation = {};
}

void DestroyTaskbarWidgetAndPopupOnThread() {

    StopTaskProgressHooks();
    if (HWND uiWindow = g_liveUiWindow.exchange(nullptr)) DestroyWindow(uiWindow);
    UnregisterClassW(L"WindhawkLiveUpdateUi", ThisModule());
    g_liveUiQueued = false;
    if (g_taskbarWnd) {
        KillTimer(g_taskbarWnd, kTaskbarEnsureTimerId);
    }
    g_taskbarTimerStarted = false;
    RemoveInjectedTaskbarWidget();

    ClearPopupCompositionBackdrop();
    g_popupBackdropLayer = nullptr;
    g_popupGlassRim = nullptr; g_popupGlassInnerRim = nullptr;
    if (g_popupXamlSource) {
        try { g_popupXamlSource.Close(); } catch (...) {}
        g_popupXamlSource = nullptr;
    }

    g_popupRootCard = nullptr;
    g_popupBadgeBorder = nullptr;
    g_popupZodiacText = nullptr;
    g_popupTitleSolar = nullptr;
    g_popupTitleLunar = nullptr;
    g_popupHolidayBanner = nullptr;
    g_popupHolidayText = nullptr;
    g_popupRowBattu = nullptr;
    g_popupRowMenhTruc = nullptr;
    g_popupRowGioHoangDao = nullptr;
    g_popupRowNenKy = nullptr;
    g_popupRowXungHuong = nullptr;
    ReleasePopupScrollControls();
    g_popupXamlChild = nullptr;

    if (g_expandedPopup && IsWindow(g_expandedPopup)) {
        DestroyWindow(g_expandedPopup);
        g_expandedPopup = nullptr;
    }
    // The class contains a WndProc in this DLL; don't retain it across mod reloads.
    UnregisterClassW(kExpandedPopupWindowClass, ThisModule());
    g_taskbarWnd = nullptr;
}

wuxc::Border BuildTaskbarWidgetElement(ModSettings const& settings) {
    wuxc::Border rootBorder;
    rootBorder.Name(kTaskbarWidgetElementName);
    rootBorder.Height(38.0);
    rootBorder.Width(static_cast<double>(settings.widgetWidth));
    rootBorder.MaxWidth(static_cast<double>(settings.widgetWidth));
    rootBorder.CornerRadius({6, 6, 6, 6});
    rootBorder.BorderThickness({1, 1, 1, 1});
    rootBorder.Padding({0, 0, 0, 0});
    rootBorder.VerticalAlignment(wux::VerticalAlignment::Center);
    rootBorder.IsHitTestVisible(true);
    rootBorder.Visibility(wux::Visibility::Visible);
    rootBorder.Opacity(1.0);
    rootBorder.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));

    wuxc::Grid contentGrid;
    wuxc::ColumnDefinition colIcon;
    colIcon.Width(wux::GridLengthHelper::Auto());
    wuxc::ColumnDefinition colText;
    colText.Width(wux::GridLengthHelper::FromValueAndType(1.0, wux::GridUnitType::Star));
    contentGrid.ColumnDefinitions().Append(colIcon);
    contentGrid.ColumnDefinitions().Append(colText);

    wuxc::Border badgeBorder;
    badgeBorder.Width(28.0);
    badgeBorder.Height(28.0);
    badgeBorder.CornerRadius({6, 6, 6, 6});
    badgeBorder.Margin({0, 0, 8, 0});
    badgeBorder.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock zodiacTb;
    zodiacTb.FontSize(16.5);
    zodiacTb.HorizontalAlignment(wux::HorizontalAlignment::Center);
    zodiacTb.VerticalAlignment(wux::VerticalAlignment::Center);
    badgeBorder.Child(zodiacTb);

    wuxc::Grid::SetColumn(badgeBorder, 0);
    contentGrid.Children().Append(badgeBorder);

    wuxc::StackPanel textStack;
    textStack.VerticalAlignment(wux::VerticalAlignment::Center);

    wuxc::TextBlock line1Tb;
    line1Tb.FontSize(11.8);
    line1Tb.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    line1Tb.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    line1Tb.MaxLines(1);

    wuxc::TextBlock line2Tb;
    line2Tb.FontSize(10.8);
    line2Tb.Margin({0, 0, 0, 0});
    line2Tb.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    line2Tb.MaxLines(1);

    wuxc::Viewbox lunarHeader;
    lunarHeader.Stretch(wuxm::Stretch::Uniform);
    lunarHeader.StretchDirection(wuxc::StretchDirection::DownOnly);
    lunarHeader.HorizontalAlignment(wux::HorizontalAlignment::Left);
    lunarHeader.Child(line1Tb);
    g_widgetLunarHeaderView = lunarHeader;
    g_widgetLiveTitle = wuxc::TextBlock();
    g_widgetLiveTitle.FontSize(11.8);
    g_widgetLiveTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    g_widgetLiveTitle.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    g_widgetLiveTitle.MaxLines(1);
    wuxc::Grid titleLayer;
    titleLayer.Children().Append(lunarHeader);
    titleLayer.Children().Append(g_widgetLiveTitle);
    textStack.Children().Append(titleLayer);
    textStack.Children().Append(line2Tb);

    wuxc::Grid::SetColumn(textStack, 1);
    contentGrid.Children().Append(textStack);

    wuxc::Grid navigationGrid;
    for (int i = 0; i < 3; ++i) {
        wuxc::ColumnDefinition column;
        column.Width(wux::GridLengthHelper::FromValueAndType(i == 1 ? 1.0 : 22.0,
            i == 1 ? wux::GridUnitType::Star : wux::GridUnitType::Pixel));
        navigationGrid.ColumnDefinitions().Append(column);
        if (i == 0) g_widgetPreviousColumn = column;
        if (i == 2) g_widgetNextColumn = column;
    }
    auto makeSwitchButton = [](PCWSTR label, int direction) {
        wuxc::Button button;
        button.Content(winrt::box_value(label));
        button.MinWidth(0);
        button.Width(20);
        button.Height(28);
        button.Padding({0, 0, 0, 0});
        button.FontSize(10);
        button.VerticalAlignment(wux::VerticalAlignment::Center);
        button.CornerRadius({4, 4, 4, 4});
        button.Click([direction](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            CycleActivity(direction);
        });
        return button;
    };
    g_widgetPreviousButton = makeSwitchButton(L"◀", -1);
    g_widgetNextButton = makeSwitchButton(L"▶", 1);
    wuxc::Grid::SetColumn(g_widgetPreviousButton, 0);
    wuxc::Grid::SetColumn(contentGrid, 1);
    wuxc::Grid::SetColumn(g_widgetNextButton, 2);
    navigationGrid.Children().Append(g_widgetPreviousButton);
    navigationGrid.Children().Append(contentGrid);
    navigationGrid.Children().Append(g_widgetNextButton);
    wuxc::Grid surface;
    g_widgetBackdropLayer=wuxc::Grid();g_widgetBackdropLayer.IsHitTestVisible(false);
    g_widgetMaterialRim=wuxc::Border();g_widgetMaterialRim.IsHitTestVisible(false);
    g_widgetMaterialRim.CornerRadius({6,6,6,6});g_widgetMaterialRim.BorderThickness({1,1,1,1});
    g_widgetBackdropLayer.SizeChanged([](auto const&,auto const&){try{SizeWidgetMaterial();}catch(...) {}});
    surface.Children().Append(g_widgetBackdropLayer);surface.Children().Append(g_widgetMaterialRim);
    navigationGrid.Margin({2,1,2,1});surface.Children().Append(navigationGrid);
    rootBorder.Child(surface);

    rootBorder.PointerEntered([](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const& args) {
        g_widgetHovered = true;
        UpdateWidgetHoverVisual();
        args.Handled(true);
    });

    rootBorder.PointerExited([](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const& args) {
        // Child exit events must not clear hover while the pointer is still in the card.
        auto position=args.GetCurrentPoint(g_playerGrid).Position();
        g_widgetHovered=position.X>=0 && position.Y>=0 && position.X<g_playerGrid.ActualWidth() && position.Y<g_playerGrid.ActualHeight();
        if(!g_widgetHovered)g_widgetPressed=false;
        UpdateWidgetHoverVisual();
        args.Handled(true);
    });

    rootBorder.PointerPressed([](wf::IInspectable const&, wuxi::PointerRoutedEventArgs const& args) {
        if(!IsSourceInsideButton(args.OriginalSource()) && args.GetCurrentPoint(g_playerGrid).Properties().IsLeftButtonPressed()) {
            g_widgetPressed=true;UpdateWidgetHoverVisual();
        }
        args.Handled(true);
    });
    auto release=[](wf::IInspectable const&,wuxi::PointerRoutedEventArgs const&){g_widgetPressed=false;UpdateWidgetHoverVisual();};
    rootBorder.PointerReleased(release);rootBorder.PointerCanceled(release);rootBorder.PointerCaptureLost(release);

    rootBorder.RightTapped([](wf::IInspectable const&,wuxi::RightTappedRoutedEventArgs const& args){
        args.Handled(true);if(!g_shuttingDown)try{ShowTaskbarContextMenu(args.GetPosition(g_playerGrid));}catch(...){}
    });
    rootBorder.Tapped([](wf::IInspectable const&, wuxi::TappedRoutedEventArgs const& args) {
        if (!g_shuttingDown && !IsSourceInsideButton(args.OriginalSource())) ToggleExpandedPopup();
        args.Handled(true);
    });

    g_widgetBadgeBorder = badgeBorder;
    g_widgetZodiacText = zodiacTb;
    g_widgetLine1Text = line1Tb;
    g_widgetLine2Text = line2Tb;

    return rootBorder;
}

LRESULT CALLBACK LiveUiWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    try {
        if (message == WM_TIMER && wParam == kObsClockTimerId) { RefreshObsClock(); return 0; }
        if (message == kRestoreHiddenActivities) { g_hiddenActivities.clear(); RequestPopupRefresh(); return 0; }
        if (message == kApplyRemoteSettings) {
            HideExpandedPopupImmediately(g_expandedPopup);
            g_popupBackdropDirty = true; g_renderedTabLanguage = -1;
            g_renderedBluetoothLanguage = -1; g_renderedPrivacyLanguage = -1;
            g_renderedMonth = -1; g_renderedCultureLanguage = -1; ++g_progressVisualRevision;
            { std::lock_guard<std::mutex> lock(g_cultureMutex); ++g_cultureRequestId; g_culture = {}; }
            EnsureTaskbarWidgetInjectedOnThread(); return 0;
        }
        if (message == kLiveUpdateMessage) {
            g_liveUiQueued = false;
            if (!g_shuttingDown) {
                EnsureTaskbarWidgetInjectedOnThread(); ProcessLiveActivityAlerts();
            }
            return 0;
        }
        if (message == WM_DISPLAYCHANGE || message == WM_DEVICECHANGE) {
            g_displayDirty = true; g_bluetoothDirty = true; if (g_systemWake) SetEvent(g_systemWake);
            if (g_bluetoothWake) SetEvent(g_bluetoothWake);
        }
        if (message == WM_SETTINGCHANGE || message == WM_THEMECHANGED || message == WM_DWMCOMPOSITIONCHANGED) {
            ++g_settingsRevision;
            ++g_progressVisualRevision;
            g_renderedTabLanguage = -1; g_renderedPrivacyLanguage = -1; g_renderedMediaSelection = 0; g_renderedMonth = -1;
            g_popupBackdropDirty = true; g_forcePowerRefresh = true;
            if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
        }
    } catch (...) { g_liveUiQueued = false; Wh_Log(L"Live UI refresh failed; waiting for next update"); }
    return DefWindowProcW(window, message, wParam, lParam);
}
void EnsureLiveUiHost() {
    if (g_liveUiWindow.load()) return;
    WNDCLASSEXW wc{}; wc.cbSize = sizeof(wc); wc.hInstance = ThisModule();
    wc.lpszClassName = L"WindhawkLiveUpdateUi"; wc.lpfnWndProc = LiveUiWindowProc;
    RegisterClassExW(&wc);
    g_liveUiWindow = CreateWindowExW(0, wc.lpszClassName, L"", WS_POPUP, 0, 0, 0, 0,
        nullptr, nullptr, wc.hInstance, nullptr);
    g_liveUiQueued = false; g_obsClockTimer = false;
}

void EnsureTaskbarWidgetInjectedOnThread() {
    if (g_shuttingDown || g_ensuringTaskbarWidget) return;
    struct EnsureGuard {
        EnsureGuard() { g_ensuringTaskbarWidget = true; }
        ~EnsureGuard() { g_ensuringTaskbarWidget = false; }
    } ensureGuard;

    ModSettings settings = GetSettings();

    if (!g_taskbarWnd || !IsWindow(g_taskbarWnd)) {
        g_taskbarTimerStarted = false;
        g_taskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!g_taskbarWnd) return;
    }

    if (GetWindowThreadProcessId(g_taskbarWnd, nullptr) != GetCurrentThreadId()) {
        Wh_Log(L"Injection must run on the taskbar UI thread.");
        return;
    }
    EnsureLiveUiHost();
    EnsureTaskProgressHooks(g_taskbarWnd, settings.enableTaskbarWidget && settings.enableTaskProgress);
    if (!g_taskbarTimerStarted) {
        g_taskbarTimerStarted = SetTimer(g_taskbarWnd, kTaskbarEnsureTimerId, 1000,
            [](HWND, UINT, UINT_PTR, DWORD) {
                try { EnsureTaskbarWidgetInjectedOnThread(); ProcessLiveActivityAlerts(); }
                catch (...) { Wh_Log(L"Activity timer update failed"); }
            }) != 0;
        if (!g_taskbarTimerStarted) Wh_Log(L"SetTimer failed: %lu", GetLastError());
    }

    RefreshNativeLocationLabel(g_taskbarWnd);
    g_uiLive = ReadLiveSnapshot();
    ObserveActivitySelection(settings);
    NormalizeSelectedActivity(settings);
    // Keep a compact settings launcher when there is no active widget.
    if (CurrentActivities(settings).empty()) g_selectedActivity = ActivityKind::None;
    if (g_playerGrid && g_injectedParentPanel) {
        try {
            if (g_currentInjectedPosition != settings.position) {
                RemoveInjectedTaskbarWidget();
            } else {
                uint32_t idx = 0;
                if (g_injectedParentPanel.Children().IndexOf(g_playerGrid, idx) &&
                    g_playerGrid.XamlRoot() != nullptr) {
                    RefreshTaskbarWidgetData();
                    if (g_playerGrid.ActualWidth() <= 0 || g_playerGrid.ActualHeight() <= 0 ||
                        g_playerGrid.Visibility() != wux::Visibility::Visible) {
                        ReportWidgetStatus(L"Attached but has no visible size; check taskbar styles/layout.");
                    }
                    return;
                }
                RemoveInjectedTaskbarWidget();
            }
        } catch (...) {
            RemoveInjectedTaskbarWidget();
        }
    }

    try {
        wux::XamlRoot xamlRoot = GetTaskbarXamlRoot(g_taskbarWnd);
        if (!xamlRoot) return;

        auto rootElement = xamlRoot.Content().try_as<wux::FrameworkElement>();
        if (!rootElement) {
            ReportWidgetStatus(L"Waiting for XamlRoot.Content.");
            return;
        }

        if (auto oldWidget = FindChildByName(rootElement, kTaskbarWidgetElementName)) {
            if (auto oldParent = wuxm::VisualTreeHelper::GetParent(oldWidget).try_as<wuxc::Panel>()) {
                uint32_t oldIdx = 0;
                if (oldParent.Children().IndexOf(oldWidget, oldIdx)) {
                    oldParent.Children().RemoveAt(oldIdx);
                }
            }
        }

        InjectionTarget target = ResolveInjectionTarget(rootElement, settings);
        if (!target.panel) {
            ReportWidgetStatus(L"Waiting for the requested TaskbarFrame.RootGrid or system-tray target.");
            LogTrayLookupFailure(rootElement, settings.position.c_str());
            return;
        }

        wuxc::Border widget = BuildTaskbarWidgetElement(settings);
        // Set ownership before mutation so an exception can roll back insertion.
        g_playerGrid = widget;
        g_injectedParentPanel = target.panel;
        auto targetGrid = target.panel.try_as<wuxc::Grid>();

        if (target.overlay) {
            widget.HorizontalAlignment(wux::HorizontalAlignment::Left);
            widget.Margin({static_cast<double>(settings.widgetLeftOffset), 0.0, 0.0, 0.0});
            wuxc::Grid::SetColumn(widget, 0);
            wuxc::Grid::SetColumnSpan(widget, std::max(1, static_cast<int>(targetGrid.ColumnDefinitions().Size())));
            wuxc::Grid::SetRow(widget, 0);
            wuxc::Grid::SetRowSpan(widget, std::max(1, static_cast<int>(targetGrid.RowDefinitions().Size())));
            wuxc::Canvas::SetZIndex(widget, 1000);
            target.panel.Children().Append(widget);
            g_injectedColumnDef = nullptr;
        } else if (target.childIndex >= 0) {
            widget.HorizontalAlignment(wux::HorizontalAlignment::Center);
            widget.Margin({4.0, 0.0, 4.0, 0.0});
            int index = ClampVal(target.childIndex, 0, static_cast<int>(target.panel.Children().Size()));
            target.panel.Children().InsertAt(index, widget);
        } else {
            wuxc::ColumnDefinition colDef;
            colDef.Width(wux::GridLengthHelper::Auto());
            auto cols = targetGrid.ColumnDefinitions();
            if (cols.Size() == 0) {
                // Preserve the Grid's implicit original column before inserting ours.
                wuxc::ColumnDefinition implicitColumn;
                implicitColumn.Width(wux::GridLengthHelper::Auto());
                cols.Append(implicitColumn);
                g_injectedImplicitColumnDef = implicitColumn;
            }
            int insertCol = ClampVal(target.column, 0, static_cast<int>(cols.Size()));

            cols.InsertAt(insertCol, colDef);
            g_injectedColumnDef = colDef;
            for (auto child : target.panel.Children()) {
                // Children() yields UIElement; Grid's attached-property APIs
                // require FrameworkElement in Windows.UI.Xaml (UWP).
                auto element = child.try_as<wux::FrameworkElement>();
                if (!element) continue;
                int c = wuxc::Grid::GetColumn(element);
                int span = wuxc::Grid::GetColumnSpan(element);
                if (c >= insertCol || c + span > insertCol) {
                    g_shiftedTrayChildren.push_back({winrt::make_weak(element), c, span, false, false});
                    auto& saved = g_shiftedTrayChildren.back();
                    if (c >= insertCol) {
                        wuxc::Grid::SetColumn(element, c + 1);
                        saved.moved = true;
                    } else {
                        wuxc::Grid::SetColumnSpan(element, span + 1);
                        saved.widened = true;
                    }
                }
            }

            widget.HorizontalAlignment(wux::HorizontalAlignment::Center);
            widget.Margin({4.0, 0.0, 4.0, 0.0});
            wuxc::Grid::SetColumn(widget, insertCol);
            wuxc::Canvas::SetZIndex(widget, 1000);
            target.panel.Children().Append(widget);
        }

        g_playerGrid = widget;
        g_injectedParentPanel = target.panel;
        g_currentInjectedPosition = settings.position;

        RefreshTaskbarWidgetData();
        Wh_Log(L"Injected: position=%s, parent=%s, column=%d, childIndex=%d, width=%d DIP",
               settings.position.c_str(), winrt::get_class_name(target.panel).c_str(),
               target.column, target.childIndex, settings.widgetWidth);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Injection failed: 0x%08X - %s",
               static_cast<unsigned int>(e.code().value), e.message().c_str());
        RemoveInjectedTaskbarWidget();
    } catch (...) {
        Wh_Log(L"Unexpected exception during injection.");
        RemoveInjectedTaskbarWidget();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Widget Lifecycle & Taskbar UI Thread Dispatch

std::atomic<UINT> g_runFromWindowThreadViaPostMessageMsg{0};

UINT RunFromWindowThreadViaPostMessageMsg() {
    UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
    if (!message) {
        message = RegisterWindowMessageW(L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);
        g_runFromWindowThreadViaPostMessageMsg.store(message);
    }
    return message;
}

std::mutex g_pendingPostHooksMutex;
HHOOK g_pendingPostHook = nullptr;
HWND g_pendingPostWindow = nullptr;

// Called on the taskbar thread, both when consuming the request and on unload.
void UnhookPendingPostHooks(bool discardMessage = true) {
    HHOOK hook;
    HWND window;
    {
        std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
        hook = std::exchange(g_pendingPostHook, nullptr);
        window = std::exchange(g_pendingPostWindow, nullptr);
    }
    if (hook) UnhookWindowsHookEx(hook);
    if (window && discardMessage) {
        MSG msg{};
        UINT message = g_runFromWindowThreadViaPostMessageMsg.load();
        while (PeekMessageW(&msg, window, message, message, PM_REMOVE)) {}
    }
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;
    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) return false;

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0, reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

void RequestTaskbarWidgetInjection() {
    if (g_shuttingDown) return;
    HWND hwnd = FindCurrentProcessTaskbarWnd();
    if (!hwnd) {
        // StartTaskbar/CreateWindowExW will retry when it exists.
        return;
    }
    const UINT message = RunFromWindowThreadViaPostMessageMsg();
    DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (!threadId || !message) return;

    std::lock_guard<std::mutex> lock(g_pendingPostHooksMutex);
    // Recheck under the lock so no producer can queue a request after teardown.
    if (g_shuttingDown || g_pendingPostHook) return;
    HHOOK hook = SetWindowsHookExW(
        WH_GETMESSAGE,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION && wParam == PM_REMOVE) {
                MSG* msg = reinterpret_cast<MSG*>(lParam);
                if (msg->message == g_runFromWindowThreadViaPostMessageMsg.load()) {
                    UnhookPendingPostHooks(false);
                    msg->message = WM_NULL;
                    msg->wParam = 0;
                    msg->lParam = 0;
                    if (!g_shuttingDown) try {
                        EnsureTaskbarWidgetInjectedOnThread();
                    } catch(winrt::hresult_error const& e) {
                        Wh_Log(L"Taskbar initialization failed: 0x%08X",static_cast<unsigned>(e.code().value));
                    } catch(...) {
                        Wh_Log(L"Taskbar initialization failed.");
                    }
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        DWORD error=GetLastError();
        Wh_Log(L"Failed to install taskbar initialization hook: %lu", error);
        return;
    }
    g_pendingPostHook = hook;
    g_pendingPostWindow = hwnd;
    if (!PostMessageW(hwnd, message, 0, 0)) {
        DWORD error = GetLastError();
        UnhookWindowsHookEx(hook);
        g_pendingPostHook = nullptr;
        g_pendingPostWindow = nullptr;
        Wh_Log(L"Failed to queue taskbar initialization: %lu", error);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
                                DWORD style, int x, int y, int width, int height,
                                HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, className, windowName, style,
                                        x, y, width, height, parent, menu, instance, param);
    if (hwnd && !g_shuttingDown) {
        // The class can be supplied as an ATOM, so inspect the created HWND.
        WCHAR actualClass[64]{};
        if (GetClassNameW(hwnd, actualClass, ARRAYSIZE(actualClass)) &&
            _wcsicmp(actualClass, L"Shell_TrayWnd") == 0) {
            RequestTaskbarWidgetInjection();
        }
    }
    return hwnd;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) return module;

    if (!g_shuttingDown && GetModuleHandleW(L"Taskbar.dll") == module) {
        TryHookTaskbarDllSymbols(module, true);
    }
    return module;
}

template <typename Prototype>
bool InstallHook(HMODULE module, PCSTR functionName, Prototype* hook, Prototype** original) {
    if (!module) {
        Wh_Log(L"Module is null when hooking %S", functionName);
        return false;
    }
    Prototype* target = reinterpret_cast<Prototype*>(GetProcAddress(module, functionName));
    if (!target) {
        Wh_Log(L"Failed to resolve %S", functionName);
        return false;
    }
    if (!WindhawkUtils::SetFunctionHook(target, hook, original)) {
        Wh_Log(L"Failed to set function hook for %S", functionName);
        return false;
    }
    return true;
}

BOOL Wh_ModInit() {
    if(!HasSupportedWindowsVersion()) {
        Wh_Log(L"This mod requires Windows 11 x64 (build 22000 or later) with the native XAML taskbar. Windows 10 is not supported.");
        return FALSE;
    }
    g_glassCpuSupported=DetectGlassCpuSupport();
    if(!g_glassCpuSupported)Wh_Log(L"Liquid Glass CPU policy: using Windows blur fallback.");
    Wh_Log(L"Initializing taskbar widget.");
    g_shuttingDown = false;

    LoadSettings();
    RunFromWindowThreadViaPostMessageMsg();

    TryHookTaskbarDllSymbols(GetModuleHandleW(L"Taskbar.dll"), false);

    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBaseModule) {
        Wh_Log(L"Failed to get kernelbase.dll handle");
        return FALSE;
    }
    InstallHook(kernelBaseModule, "LoadLibraryExW", LoadLibraryExW_Hook, &LoadLibraryExW_Original);

    HMODULE user32Module = LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    InstallHook(user32Module, "CreateWindowExW", CreateWindowExW_Hook, &CreateWindowExW_Original);
    StartLiveProviders();
    return TRUE;
}

void Wh_ModAfterInit() {
    TryHookTaskbarDllSymbols(GetModuleHandleW(L"Taskbar.dll"), true);

    if (HWND hTrayWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTrayWnd, [](PVOID) { EnsureTaskbarWidgetInjectedOnThread(); }, nullptr);
    }
}

void Wh_ModBeforeUninit() {
    // Detach widget/popup handlers and timers on their owning UI thread.
    g_shuttingDown = true;
    StopLiveProviders();
    StopSettingsHost();
    if (HWND hTrayWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTrayWnd,
            [](PVOID) {
                UnhookPendingPostHooks();
                DestroyTaskbarWidgetAndPopupOnThread();
            },
            nullptr);
    }
    ReleaseLiveProviderHandles();
}

void Wh_ModSettingsChanged() {
    LoadSettings(true);
    if (g_obsWake) SetEvent(g_obsWake);
    g_displayDirty = true; g_bluetoothDirty = true;
    if (g_bluetoothWake) SetEvent(g_bluetoothWake);
    if (g_systemWake) SetEvent(g_systemWake);
    if (g_taskControlWake) SetEvent(g_taskControlWake);
    g_forcePowerRefresh = true;
    { std::lock_guard<std::mutex> lock(g_cultureMutex);
      ++g_cultureRequestId; g_culture = {}; }
    if (g_liveWakeEvent) SetEvent(g_liveWakeEvent);
    if (HWND hTrayWnd = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThread(
            hTrayWnd,
            [](PVOID) {
                HideExpandedPopupImmediately(g_expandedPopup);
                RefreshSettingsWindow();
                g_batteryAlertTracker = {}; g_bluetoothAlerts = {}; g_hiddenActivities.clear();
                g_alertPrivacyUses.clear(); g_alertTasks.clear(); g_privacyAlerts = {}; g_systemAlerts = {};
                g_pendingSystemAlert = g_systemNoticeActivity = ActivityKind::None; g_systemNoticeUntil = 0;
                g_cultureSelectedId.clear(); g_renderedCultureRevision = ~0ULL;
                g_batteryNeedsAttention = false;
                EnsureTaskbarWidgetInjectedOnThread();
                if (g_expandedPopup && IsWindowVisible(g_expandedPopup)) {
                    RefreshExpandedPopupData();
                }
            },
            nullptr);
    }
}
