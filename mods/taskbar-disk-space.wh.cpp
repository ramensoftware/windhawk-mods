// ==WindhawkMod==
// @id              taskbar-disk-space
// @name            Taskbar Disk Space
// @description     Disk space on the Windows 11 taskbar with drive selection, color themes and system shortcuts.
// @description:ru-RU Место на диске на панели задач Windows 11: выбор диска, цветовые темы и системные команды.
// @version         0.17.4
// @author          Fatalko
// @github          https://github.com/Fatalko
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32
// ==/WindhawkMod==

// Taskbar XAML discovery is adapted from taskbar-multirow by Michael Maltsev
// and taskbar-system-info by Yevhenii Starychenko (both GPL-3.0):
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multirow.wh.cpp
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-system-info.wh.cpp
// This program is free software under GPL v3; WITHOUT ANY WARRANTY.
// License: https://www.gnu.org/licenses/gpl-3.0.html

// ==WindhawkModReadme==
/*
# Taskbar Disk Space

Display the selected local drive's label, free space and total capacity on the
Windows 11 taskbar. Show two lines with a drive name, or one compact line with
the name hidden.

- **Left-click:** choose a local drive.
- **Right-click → Theme:** choose System (neutral hover), Color bar (default), Capacity background, or Text only (no hover background). Capacity background fills each drive in the list with its free/used proportions, with no bottom strip; its opacity follows the native Start frame. Selection persists.
  High contrast disables the list's capacity background; letter tiles retain their own appearance. Windows animation preferences are respected.
- **Right-click → Position:** choose Automatic, Left or Right. The choice applies immediately and persists; changing position in settings overrides the menu choice.
- **Right-click → Monitor:** immediately select the primary taskbar, all taskbars or a detected display. Selection persists; changing the monitor setting overrides the menu choice.
- **Compact mode → Automatic:** always enabled, checked and unavailable for switching; a previously saved disabled value is migrated to enabled; in list mode, compact further only when no card fits in the selected design. Single-drive mode keeps its full/compact/tiles fallback. Manual Compact mode takes priority.
- **Low disk space:** choose Disabled, 5/10/15/20 %, or 5/10/20/50 GiB; custom thresholds remain available in settings.
- **Reset appearance:** restore Color bar, green/red, Free / Total, one decimal place, normal layout and automatic compact mode; keep drive, monitor, position and warning threshold.
- **Apply Windhawk settings:** discard menu overrides and apply the settings page; menu-only parameters return to defaults. Menu selections do not rewrite settings-page fields; menu checkmarks show effective values.
- **Right-click → Precision:** choose 0, 1 (default) or 2 decimal places for GiB and percentages. Values are rounded; selection persists.
- **Right-click → Format:** choose Free / Total, Used / Total, or Free, %. The selection persists and works with Compact mode. Used space is total minus space available to the current user, including quotas.
- **Right-click → Colors:** select one of ten palettes for Color bar or Capacity background. In other themes the dimmed submenu can be opened for explanations, but color selections have no effect. Compact mode works with every theme.
- **Right-click → Compact mode → Hide drive name:** toggle the compact single-line layout with the drive name hidden. The choice persists; changing Hide drive name in settings overrides it.
- **Right-click → Open selected drive:** open the current drive in File Explorer.
- **Right-click → This PC:** open File Explorer at This PC.
- **Right-click → Disk Management:** open the system console with administrator elevation.
- **Right-click → Refresh now:** reread disk space without waiting for the refresh interval.

Menus use cached drive and monitor lists collected by the worker (drive connections/removals every two seconds with a full refresh every 30 seconds, monitors on taskbar topology changes detected every five seconds, with a 60-second fallback). Reopen the menu after a device change to see the refreshed list.

Drive and theme selections persist across Explorer restarts. Changing the initial
drive or color scheme in Windhawk settings overrides the corresponding menu choice.
In single-drive mode, the hover surface follows the Start button background height and corner radius;
width follows the text and space available between native buttons and the tray. A 12-DIP gap is kept; a measured list button is tried before hiding the indicator. Placement is rechecked at least every five seconds. Drive-list cards and More use the native frame height and corner radius; text is centered inside each card. Neutral and capacity backgrounds share the effective alpha of a translucent native Start brush, including brush opacity; if unavailable, a system brush or a 16/255 (about 6%) fallback is used. Vertical cards use this height as a minimum so stacked text can grow. Hover and pressed transitions respect
Windows animation settings. Capacity tooltips are available on the list and overflow buttons.

Low disk space warning settings offer Disabled (default), Percent, or GiB, with an integer threshold (default 10). At or below the threshold, text turns red in every appearance mode, including Compact mode. Refresh now also checks the threshold. High-contrast text colors are preserved; zero or unavailable capacity does not trigger the warning.

Monitor selection for display is a single dropdown offering Primary taskbar, All taskbars and automatically detected displays with model names when available, falling back to device labels. Resolution is not shown. Taskbar topology is checked every five seconds; display options refresh on topology changes and at least once per minute; reopen the dropdown after a connection change. Windows must show a taskbar on the selected display. A disconnected selection is retained. Monitor choices use the Windows monitor device path when available, mapped to the current DISPLAY name; if Windows cannot supply a path, the GDI name remains the fallback. Menus and hover states remain independent. Windhawk 1.7.3 uses a text field (`primary`, `all`, or a device name such as `\\.\DISPLAY2`) because dynamic lists require Windhawk 2.0.

Refresh defaults to 10 minutes. Placement options are Automatic (free left/right sections), Left (15 DIPs), and Right (before the system tray). Requires Windhawk 1.7.3
or 2.0 and the native bottom taskbar on Windows 11 22H2+. ExplorerPatcher and
StartAllBack are unsupported. Local and removable drives are listed; capacity checks
read volume metadata without writing user data.


## Vertical taskbars — experimental

Single-drive and all-drive modes support a narrow stacked layout on left/right taskbars: drive letter, value, divider, total and unit. Vertical panels always use Automatic placement in the upper free gap. The Position menu offers only Automatic (top); the saved horizontal position is restored when the taskbar returns to a horizontal edge. Only visible button bounds are used; off-screen elements are ignored. Native buttons and tray space are excluded, and the indicator is hidden if no safe gap fits. All-drive mode stacks the volumes vertically. When the list exceeds the safe gap, scroll the indicator with the mouse wheel; horizontal scrolling is disabled. Horizontal taskbars retain their existing layout. Enable layout diagnostics if the indicator remains hidden.

## Layout diagnostics

Enable **Taskbar layout diagnostics** temporarily and start Windhawk debug logging. Move the taskbar to each edge and collect lines beginning with `[Taskbar layout]`. They report window geometry, XAML discovery and available width without changing placement. Vertical taskbar support is experimental.

## Automatic placement and compact views

On horizontal taskbars, the mod measures each drive card and the free sections between visible buttons, Widgets and the tray in XAML logical units. In list mode, Automatic position chooses the side that fits more whole cards in the selected design; equal counts prefer the left section after Widgets for centered icons, or the right section before the tray for left-aligned icons. Manual side selection is preserved.

In list mode, show the largest fitting prefix in drive-letter order and reserve space for a More · N button containing the hidden count. Preserve the selected normal, compact or letter-tile design. Hover More for a passive capacity card of the hidden drives; left-click opens the complete drive menu. The selected group and full drive snapshot remain intact. For example, eight drives may appear as C:, D:, E: and More · 5. Capacity readings, warnings, palettes and formats remain individual to each drive.

If no whole card plus More fits, Automatic may try compact text and then letter tiles; it no longer reduces an entire list when some cards already fit in the selected view. If no card fits, use Drives · N; hide only when even that button cannot fit or native geometry is unavailable. Manual Drives · N always takes priority. Placement is checked at least every five seconds; shrinking is immediate on the next check, while restoring a card requires eight spare DIPs to reduce flicker near a width threshold. Single-drive and vertical-list behavior is unchanged. High contrast preserves system colors and disables capacity backgrounds.

In drive-list mode, the overall container stays transparent. System and Color bar appearances highlight only the hovered drive with a neutral background; Color bar keeps its bottom free/used strip without adding a capacity fill on hover. Capacity background retains its persistent fill and adds a subtle neutral highlight over the hovered card; letter tiles receive the same highlight. The highlight fades in and out over 120 ms, respects Windows animation preferences and leaves the capacity fill opacity unchanged. Text only retains no hover background. Compact mode → Drives · N manually enables the list button, taking priority over letter tiles and hidden names. Its tooltip is a rounded card with the group name, drive count and individual drive panels: letter, name, capacity in the current format and precision, free-space percentage and a free/used bar in the selected palette. Names and capacity wrap to up to two lines. The passive card lays out long lists in columns within the monitor work area; if they cannot all fit, it reports the remaining count. Light, dark and high-contrast Windows themes are supported. The manual list button is also available on vertical taskbars.

Opt-in `[Taskbar layout]` logs include DPI, both free sections, measured widths of all four views, chosen side, visible/hidden counts, final width and mode (0 full, 1 compact, 2 tiles, 3 list button, -1 no room).

The left-click groups are Automatic mode, Local drives and External drives; Selected drives provides a saved manual set. Automatic mode here displays all detected drives; taskbar positioning is a separate setting. Middle-clicking a drive opens it in File Explorer without changing the selected group. Every command and submenu option in both taskbar menus has a short native hover tooltip in Russian or English, following the Windows interface language. Drive hints explain left-click selection and middle-click opening. Dimmed informational entries also support hints without applying settings; separators have no tooltip.

## Manual selection of multiple drives

Open **Selected drives** in either taskbar menu and check the required letters. Each check applies immediately and activates the manual set; internal and external drives can be combined. **Show selected drives** restores the saved set after switching groups; turning it off returns to the previous single drive. Choosing a drive outside this submenu still activates single-drive mode.

The taskbar, More · N and Drives · N card contain only currently connected drives in the manual set, in drive-letter order. Newly connected unchecked devices are excluded. A disconnected selected drive keeps its check and appears as disconnected in the submenu; it returns to the taskbar when the same letter reconnects. Selection follows letters, not physical devices: if Windows assigns a letter to another volume, that volume is displayed.

An empty set shows No drives selected rather than reverting to all drives. Hover hints explain the checkboxes; middle-click opens a connected drive in File Explorer without changing the set. Explorer restarts and Reset appearance preserve the selection. **Apply Windhawk settings** clears manual checks because the set is configured only through the menu.

## All local drives
The left-click menu offers Automatic mode, Local drives, External drives and Selected drives, with saved selections and distinct icons. External drives include removable media and devices reporting USB/IEEE 1394 or removable media through Windows storage properties. If a fixed drive cannot be queried, it remains in the local group.

USB flash drives and external disks are detected automatically within approximately two seconds after Windows assigns or removes a drive letter. Automatic/local/external groups include newly detected matching drives; the manual set includes only checked letters, while single-drive mode keeps its selected drive. Network drives and devices without drive letters are excluded.


Enable **Show all drives** in settings or **Automatic mode** in either taskbar menu. Each local volume has its own column, warning color and free/used strip in Color bar mode. Format, precision, themes and Compact mode apply to all columns. Selecting a particular drive returns to single-drive mode; turning the all-drive option off restores the previous selected drive.

Volumes appear in drive-letter order. Custom names apply only to the configured drive. Drive-letter changes are detected every two seconds, with a full inventory refresh every 30 seconds; capacity uses the configured interval and Refresh now. On horizontal taskbars, only the fitting prefix is shown, with More · N for the remaining drives. Automatic is a fallback when no card fits in the selected design. No aggregate capacity is shown. Network drives remain excluded; removable USB drives are supported.

## Screenshots

[![Taskbar Disk Space — скриншот 1](https://i.imgur.com/QsiHo0m.png)](https://imgur.com/QsiHo0m)

[![Taskbar Disk Space — скриншот 2](https://i.imgur.com/VyaAfGR.png)](https://imgur.com/VyaAfGR)

[![Taskbar Disk Space — скриншот 3](https://i.imgur.com/fHFa648.png)](https://imgur.com/fHFa648)

[![Taskbar Disk Space — скриншот 4](https://i.imgur.com/zqPRJ9V.png)](https://imgur.com/zqPRJ9V)

[![Taskbar Disk Space — screenshot 5](https://i.imgur.com/7Zs15Sq.png)](https://imgur.com/7Zs15Sq)


## Why this is a separate mod

Both this mod and taskbar-disk-space-label display free space and total capacity. This mod focuses on interactive taskbar controls: switching drives, appearance, compact layouts, monitor and position selection, and system shortcuts from the indicator menus.

## Русский

### Taskbar Disk Space

Свободное и общее место на выбранном локальном диске — прямо на панели задач Windows 11.
Меняйте диск и цветовую тему через меню индикатора, без открытия настроек Windhawk.

## Скриншоты / Screenshots

[![Taskbar Disk Space — скриншот 1](https://i.imgur.com/QsiHo0m.png)](https://imgur.com/QsiHo0m)

[![Taskbar Disk Space — скриншот 2](https://i.imgur.com/VyaAfGR.png)](https://imgur.com/VyaAfGR)

[![Taskbar Disk Space — скриншот 3](https://i.imgur.com/fHFa648.png)](https://imgur.com/fHFa648)

[![Taskbar Disk Space — скриншот 4](https://i.imgur.com/zqPRJ9V.png)](https://imgur.com/zqPRJ9V)

[![Taskbar Disk Space — screenshot 5](https://i.imgur.com/7Zs15Sq.png)](https://imgur.com/7Zs15Sq)

## Отображение

С именем диска — две строки:

```text
Данные (D:)
Свободно 128,4 из 931,5 ГиБ
```

При включённой настройке **«Скрывать имя диска»** — одна компактная строка:

```text
(D:) 128,4 / 931,5 ГиБ
```

Подменю **«Точность»** задаёт 0, 1 или 2 знака после запятой; по умолчанию используется 1. Числа округляются, выбор сохраняется и работает во всех форматах и мини-дизайне.

Подменю **«Формат»** переключает свободный объём, занятый объём или процент свободного места. Все варианты работают в мини-дизайне; цветная полоса всегда показывает свободное/занятое место. При квотах занятый объём вычисляется как общий объём минус доступное текущему пользователю место.

Можно использовать метку тома или задать своё имя. Недоступный диск помечается
соответствующим сообщением. ГиБ — единицы по 1024³ байт.

Меню используют кеш рабочего потока: подключение и отключение дисков проверяется каждые две секунды с полным обновлением раз в 30 секунд, мониторы — при изменении панелей (проверка раз в пять секунд), с контрольным обновлением раз в минуту. После изменения устройств переоткройте меню, когда список обновится.

## Вертикальная панель — экспериментально

Для одного или всех дисков на левой и правой панели используется узкий вид: буква диска, значение, разделитель, общий объём и единица друг под другом. На вертикальных панелях действует только автоматическое размещение в свободном участке сверху. В меню положения доступно только «Автоматически (сверху)»; сохранённое горизонтальное положение восстанавливается при возврате панели вниз или вверх. Учитываются только видимые границы кнопок; элементы за пределами панели игнорируются. Кнопки и трей исключаются; если безопасного места нет, индикатор скрывается. В режиме всех дисков они располагаются сверху вниз. Если список длиннее безопасного участка, прокручивайте его колёсиком внутри индикатора; горизонтальная прокрутка отключена. Горизонтальное размещение сохраняется. Если индикатор не появляется, включите диагностику положения панели.

## Диагностика положения панели

Временно включите **«Диагностика положения панели»** и журнал отладки Windhawk. Переместите панель вниз, вверх, влево и вправо; сохраните строки `[Taskbar layout]`. Они показывают размеры, обнаружение XAML и доступную ширину, не изменяя размещение. Поддержка вертикальной панели экспериментальная.

## Автоматическое размещение и компактные виды

На горизонтальной панели мод измеряет каждую карточку диска и свободные участки между видимыми кнопками, виджетом и треем в логических единицах XAML. В режиме списка автоматическое положение выбирает сторону, где помещается больше целых карточек выбранного вида. При одинаковом количестве центральные значки предпочитают левый участок после виджета, а значки с левым выравниванием — правый перед треем. Ручной выбор стороны сохраняется.

В режиме списка отображается столько первых дисков в порядке букв, сколько помещается целиком, с заранее отведённым местом для кнопки «Ещё · N». Она показывает число скрытых дисков. Выбранный обычный вид, компактный текст или плитки сохраняются. При наведении на «Ещё» появляется пассивная карточка с данными скрытых дисков, при нажатии левой кнопкой — полное меню выбора. Выбранная группа и полный список показаний сохраняются. Например, восемь дисков могут отображаться как C:, D:, E: и «Ещё · 5». Форматы, палитры и предупреждения применяются отдельно к каждому диску.

Если не помещается ни одна карточка вместе с «Ещё», авто мини-дизайн может попробовать компактный текст, затем плитки; весь список больше не уменьшается, если часть карточек уже помещается в выбранном виде. Если ни одна карточка не помещается, остаётся «Диски · N»; индикатор скрывается только при отсутствии места даже для неё или при недоступной геометрии панели. Ручной «Диски · N» всегда имеет приоритет. Проверка выполняется не реже раза в пять секунд; сокращение происходит при ближайшей проверке, а для возврата карточки нужны восемь свободных единиц XAML сверх её ширины, чтобы уменьшить мигание у границы. Одиночный режим и вертикальный список сохраняют прежнее поведение. В высококонтрастной теме сохраняются системные цвета, цветовые фоны отключены.

В режиме списка общий контейнер остаётся прозрачным. В темах «Системная» и «Цветная полоса» нейтральным фоном подсвечивается только диск под курсором; нижняя полоса свободного/занятого места сохраняется, заливка объёма при наведении не появляется. «Фон свободного места» сохраняет постоянную заливку, а при наведении поверх неё появляется лёгкая нейтральная подсветка только выбранной курсором карточки; такая же подсветка есть у плиток букв. Она плавно появляется и исчезает за 120 мс, учитывает настройки анимации Windows и не меняет прозрачность самой заливки места. «Только текст» сохраняет отсутствие подсветки. «Компактный режим → Диски · N» вручную включает кнопку списка; этот вариант имеет приоритет над плитками и скрытием имени. Подсказка оформлена как скруглённая карточка с названием группы, количеством дисков и отдельными блоками: буква, имя, объём в текущем формате и точности, процент свободного места и шкала свободно/занято в выбранной палитре. Имена и объёмы ограничены двумя строками. Карточка пассивная: большие списки размещаются в колонках в пределах рабочей области монитора; если все диски не помещаются, показано количество оставшихся. Поддерживаются светлая, тёмная и высококонтрастная темы Windows. В вертикальном положении ручная кнопка списка также доступна.

В режиме диагностики журнал `[Taskbar layout]` показывает DPI, оба свободных участка, измеренную ширину четырёх вариантов, выбранную сторону, число видимых/скрытых дисков, итоговую ширину и вид (0 — полный, 1 — мини, 2 — плитки, 3 — кнопка списка, -1 — нет места).

В меню левой кнопки группы называются «Автоматический режим», «Локальные диски», «Внешние диски»; «Выбранные диски» задаёт сохранённый ручной набор. Здесь «Автоматический режим» означает показ всех обнаруженных дисков; выбор положения панели задаётся отдельно. Средняя кнопка мыши на конкретном диске открывает его в Проводнике, не меняя выбранную группу. У каждого пункта обоих меню, включая подменю и варианты выбора, есть короткая штатная подсказка при наведении. Язык соответствует интерфейсу Windows — русский или английский. Подсказка диска поясняет выбор левой кнопкой и открытие средней. Серые информационные строки также поддерживают подсказки, не изменяя настройки; у разделителей подсказок нет.

## Ручной выбор нескольких дисков

В обоих меню индикатора откройте **«Выбранные диски»** и отметьте нужные буквы. Каждая отметка применяется сразу и включает ручной набор; можно смешивать локальные и внешние диски. **«Показывать выбранные диски»** возвращает сохранённый набор после переключения на другую группу; выключение возвращает прежний одиночный диск. Выбор отдельного диска вне этого подменю по-прежнему включает одиночный режим.

На панели, в «Ещё · N» и карточке «Диски · N» учитываются только подключённые диски ручного набора, в порядке букв. Новые неотмеченные устройства в набор не добавляются. При отключении отмеченного диска его отметка сохраняется, в подменю появляется строка «отключён», а на панели он возвращается при подключении с той же буквой. Выбор привязан к буквам, а не к физическому устройству: если Windows назначит букву другому тому, отображаться будет этот том.

Если снять все отметки, отображается сообщение «Диски не выбраны»; автоматический показ всех дисков не включается. Подсказки поясняют назначение галочек; средняя кнопка на подключённом диске открывает его в Проводнике, не меняя набор. Перезапуск Explorer и «Сбросить оформление» сохраняют выбор. **«Применить настройки Windhawk»** очищает ручные отметки, поскольку набор задаётся только через меню.

## Все локальные диски
В меню левой кнопки доступны «Автоматический режим», «Локальные диски», «Внешние диски» и «Выбранные диски». Выбранная группа сохраняется. Локальные диски и внешние устройства имеют разные значки. Внешними считаются съёмные носители и диски, для которых Windows сообщает USB/IEEE 1394 или съёмный носитель; если запрос свойств фиксированного диска недоступен, он остаётся в локальной группе.

USB-флешки и внешние диски определяются автоматически примерно за две секунды после назначения или удаления буквы диска Windows. В автоматической/локальной/внешней группе новые подходящие диски появляются сами; в ручном наборе — только отмеченные буквы. Одиночный режим сохраняет выбранный диск. Сетевые диски и устройства без буквы не отображаются.


Включите **«Показывать все диски»** в настройках или **«Автоматический режим»** в меню левой или правой кнопки. Диски показаны колонками в порядке букв. Для каждого отдельно отображаются объём, предупреждение о нехватке места и тонкая полоса свободного/занятого места в теме «Цветная полоса». Формат, точность, тема и мини-дизайн общие.

Выбор конкретного диска возвращает одиночный режим. Отключение режима возвращает прежний выбранный диск. Собственное имя применяется только к начальному диску. Изменения букв дисков проверяются каждые две секунды, полный список обновляется раз в 30 секунд; объём — по заданному интервалу и через «Обновить сейчас». На горизонтальной панели отображаются только помещающиеся первые диски, остальные доступны через «Ещё · N». Автоматический компактный режим используется, если в выбранном виде не помещается ни одна карточка. Общий объём дисков не суммируется. USB-флешки и внешние диски включены; сетевые диски исключены.

## Управление с панели задач

| Действие | Результат |
|---|---|
| Левая кнопка мыши | Список доступных локальных и съёмных дисков |
| Правая кнопка → **Тема** | Системная, Цветная полоса, Фон свободного места или Только текст |
| Правая кнопка → **Цвета** | Десять палитр для цветной полосы и фона свободного места |
| Правая кнопка → **Положение** | Автоматически, Слева или Справа; выбор применяется сразу и сохраняется |
| Правая кнопка → **Монитор** | Основная панель, все панели или обнаруженный экран; применяется сразу |
| Правая кнопка → **Нехватка места** | Выключение и готовые пороги в процентах или ГиБ |
| Правая кнопка → **Сбросить оформление** | Возвращает внешний вид по умолчанию |
| Правая кнопка → **Применить настройки Windhawk** | Возвращает значения страницы настроек |
| Правая кнопка → **Точность** | 0, 1 или 2 знака после запятой для ГиБ и процентов; выбор сохраняется |
| Правая кнопка → **Формат** | Свободно / Всего, Занято / Всего или Свободно, %; выбор сохраняется |
| Правая кнопка → **Компактный режим → Скрывать имя диска** | Скрывает имя диска и включает одну компактную строку; повторное нажатие возвращает две строки |
| Правая кнопка → **Открыть выбранный диск** | Открывает выбранный в индикаторе диск в Проводнике |
| Правая кнопка → **Этот компьютер** | Открывает раздел Проводника со списком дисков |
| Правая кнопка → **Управление дисками** | Открывает системную консоль с запросом прав администратора |
| Правая кнопка → **Обновить сейчас** | Перечитывает свободное и общее место без ожидания интервала |

Выбранные диск и тема сохраняются после перезапуска Explorer. Изменение начального
диска или цветовой схемы в настройках Windhawk заменяет соответствующий выбор из меню.
«Компактный режим» сохраняется после перезапуска Explorer. Изменение настройки «Скрывать имя диска» заменяет выбор из меню. Остальные настройки сохраняют выбор.

## Рамка и цветовые темы

В меню «Тема» доступны **Системная** (нейтральная подсветка), **Цветная полоса** (свободное/занятое место, включена по умолчанию), **Фон свободного места** и **Только текст** (без подсветки). Новая тема постоянно показывает доли свободного/занятого места фоном каждого диска списка, без нижней полосы; прозрачность фона соответствует рамке «Пуска». В высококонтрастном режиме цветовой фон отключён, а плитки букв сохраняют собственное оформление. Выбор сохраняется после перезапуска Explorer. Менять палитру можно для цветной полосы и фона; в других темах серое подменю «Цвета» открывается для пояснений, но выбор цвета не действует и прежняя палитра сохраняется. Компактный режим работает с любой темой.

Карточки списка и кнопка «Ещё» используют высоту и скругление рамки «Пуска»; текст центрируется. Нейтральный и цветной фон используют её эффективный альфа-канал с учётом прозрачности кисти. Если полупрозрачную кисть получить нельзя, используется системная кисть или резервное значение 16/255 (около 6% непрозрачности). На вертикальной панели эта высота служит минимумом, чтобы не обрезать многострочный текст.

В одиночном режиме при наведении появляется закруглённая полупрозрачная рамка. Её высота и скругление
берутся у подсветки кнопки **«Пуск»**, ширина автоматически подстраивается под текст
и доступное место. Длинный текст сокращается с многоточием. Доступная ширина учитывает кнопки панели и трей с зазором 12 логических единиц. Если остаётся менее 72 единиц, индикатор временно скрывается и возвращается после освобождения места. Расчёт повторяется при обновлении интерфейса, не реже раза в пять секунд.

Подсветка плавно меняется при наведении и нажатии; анимации учитывают системную
настройку Windows. Для кнопок «Диски · N» и «Ещё · N» доступны подсказки с данными дисков.

Цветная полоса показывает долю свободного и занятого места. По умолчанию свободное
место зелёное, занятое — красное. Доступны также синий/оранжевый,
бирюзовый/фиолетовый, фиолетовый/жёлтый, бирюзовый/розовый, лаймовый/индиго,
янтарный/тёмно-синий, мятный/коралловый, небесный/пурпурный и белый/серый.
Цвет текста учитывает светлую, тёмную и высококонтрастную темы Windows.

## Быстрые настройки и восстановление

- **Компактный режим → Автоматически** всегда включён: пункт отмечен галочкой и недоступен для переключения. Старое сохранённое выключение автоматически заменяется включением. В режиме списка сначала сохраняются помещающиеся карточки выбранного вида с кнопкой «Ещё · N»; более компактный вид пробуется, если не помещается ни одна карточка. Одиночный режим сохраняет прежний переход к компактной строке и плиткам. Ручной «Компактный режим» остаётся приоритетным.
- **Нехватка места** предлагает выключение, пороги 5/10/15/20 % и 5/10/20/50 ГиБ.
  Текущий пользовательский порог также отображается; произвольное значение можно
  задать в настройках Windhawk.
- **Сбросить оформление** возвращает цветную полосу, зелёный/красный, свободно/всего,
  один знак после запятой, обычный вид и автоматический компактный режим. Диск,
  монитор, положение и порог нехватки места сохраняются.
- **Применить настройки Windhawk** сбрасывает выбор из меню и применяет значения
  страницы настроек. Для параметров, доступных только в меню, восстанавливает
  значения по умолчанию.

Галочки меню показывают действующие значения. Меню хранит выбор отдельно от страницы
настроек Windhawk, поэтому поля страницы не переписываются при нажатии в меню.
Изменение соответствующего поля страницы заменяет выбор из меню; другие поля
его сохраняют. Для принудительного возврата используйте «Применить настройки Windhawk».
## Индикация нехватки места

В настройках выберите «Индикация нехватки места»: выключена (по умолчанию), проценты или ГиБ. Задайте целый порог (по умолчанию 10). Если доступное текущему пользователю место не превышает порог, текст становится красным во всех темах и в мини-дизайне. Цвет возвращается к обычному после освобождения места. Проверка выполняется при обновлении данных, в том числе через «Обновить сейчас». Для нулевого или недоступного объёма предупреждение не включается. В высококонтрастной теме сохраняется системный цвет текста. Уведомления не показываются.

## Настройки и размещение

Положение можно менять сразу через подменю **«Положение»** по правой кнопке. Текущий вариант отмечен галочкой; выбор сохраняется после перезапуска Explorer. Изменение положения в настройках Windhawk заменяет выбор из меню, остальные настройки его сохраняют.

- Начальный диск: **C:**. Собственное имя — необязательно.
- Обновление: раз в **10 минут** по умолчанию; диапазон — 1–3600 секунд.
- **Автоматический** режим: свободный участок слева или справа; предпочтительный левый отступ ограничен доступным местом.
- **Слева**: фиксированный отступ 15 логических единиц.
- **Справа**: непосредственно перед системным треем.

При левом выравнивании кнопок ручной режим «Слева» резервирует место; автоматическое положение предпочитает правый участок.
При центральном выравнивании кнопки не сдвигаются. Размеры учитывают масштаб Windows.

## Несколько мониторов

Подменю **«Монитор»** по правой кнопке показывает актуальные экраны, основную панель и все панели. Нажатие сразу применяет и сохраняет выбор; текущий вариант отмечен галочкой. Работает в Windhawk 1.7.3 и 2.0. Изменение выбора в настройках заменяет выбор из меню, остальные настройки его сохраняют.

Один пункт **«Выбор монитора для отображения»** предлагает основную панель, все панели и автоматически обнаруженные мониторы с реальным названием модели, если Windows его предоставляет; иначе используется прежняя подпись устройства. Изменения панелей проверяются раз в пять секунд; список обновляется при изменениях и не реже раза в минуту; после подключения или отключения экрана переоткройте выпадающее меню. Ручной ввод имени устройства в Windhawk 2.0 не нужен. Windows должна показывать панель задач на выбранном мониторе. Если выбранный экран отключён, индикатор не отображается до его возвращения. Выбор сохраняется по пути устройства монитора Windows и сопоставляется с актуальным номером DISPLAY; если путь недоступен, используется имя DISPLAY. Диск, формат и оформление общие; меню и анимации независимы. На дополнительной панели без трея режим «Справа» использует отступ 15 логических единиц от правого края. В Windhawk 1.7.3 динамический список недоступен: это текстовое поле со значениями `primary`, `all` или именем устройства, например `\\.\DISPLAY2`.

## Совместимость

Windhawk **1.7.3 и 2.0**, штатная нижняя горизонтальная панель задач
**Windows 11 22H2 и новее**. ExplorerPatcher и StartAllBack не поддерживаются.
В Windhawk 2.0 список дисков в настройках дополнительно показывает метки томов.
Меню индикатора показывает метки томов в обеих версиях.

В меню доступны локальные диски, USB-флешки и внешние диски. Сетевые ресурсы не перечисляются. Проверка ёмкости читает сведения о томе через
`GetDiskFreeSpaceExW` и не записывает пользовательские данные на диск.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LayoutDiagnostics: false
  $name: Taskbar layout diagnostics
  $name:ru-RU: Диагностика положения панели
  $description: Enable temporarily with Windhawk debug logging to investigate taskbar placement. Does not change layout.
  $description:ru-RU: Временно включите вместе с журналом отладки Windhawk для проверки положения панели. Не меняет разметку.
- ShowAllDrives: false
  $name: Show all drives
  $name:ru-RU: Показывать все диски
  $description: Show each local drive in a separate column. USB flash drives and external disks are included; network drives are excluded.
  $description:ru-RU: Каждый локальный или съёмный диск отображается отдельной колонкой. USB-флешки и внешние диски включены; сетевые диски исключены.
- Drive: "C:"
  $name: Drive
  $name:ru-RU: Диск
  $description: Initial drive. After startup, click the taskbar indicator to switch drives. Windhawk 2.0 also shows detected volume labels.
  $description:ru-RU: Начальный диск. После запуска диск можно менять нажатием по индикатору на панели задач. В Windhawk 2.0 также видны метки обнаруженных томов.
  #! $dynamicSelect: true
  $options:
    - "A:": "A:"
    - "B:": "B:"
    - "C:": "C:"
    - "D:": "D:"
    - "E:": "E:"
    - "F:": "F:"
    - "G:": "G:"
    - "H:": "H:"
    - "I:": "I:"
    - "J:": "J:"
    - "K:": "K:"
    - "L:": "L:"
    - "M:": "M:"
    - "N:": "N:"
    - "O:": "O:"
    - "P:": "P:"
    - "Q:": "Q:"
    - "R:": "R:"
    - "S:": "S:"
    - "T:": "T:"
    - "U:": "U:"
    - "V:": "V:"
    - "W:": "W:"
    - "X:": "X:"
    - "Y:": "Y:"
    - "Z:": "Z:"
  $options:ru-RU:
    - "A:": "A:"
    - "B:": "B:"
    - "C:": "C:"
    - "D:": "D:"
    - "E:": "E:"
    - "F:": "F:"
    - "G:": "G:"
    - "H:": "H:"
    - "I:": "I:"
    - "J:": "J:"
    - "K:": "K:"
    - "L:": "L:"
    - "M:": "M:"
    - "N:": "N:"
    - "O:": "O:"
    - "P:": "P:"
    - "Q:": "Q:"
    - "R:": "R:"
    - "S:": "S:"
    - "T:": "T:"
    - "U:": "U:"
    - "V:": "V:"
    - "W:": "W:"
    - "X:": "X:"
    - "Y:": "Y:"
    - "Z:": "Z:"
- DisplayName: ""
  $name: Custom drive name
  $name:ru-RU: Своё имя диска
  $description: Leave empty to use the Windows volume label. The drive letter is always shown.
  $description:ru-RU: Оставьте пустым, чтобы использовать метку тома из Windows. Буква диска отображается всегда.
- HideDriveName: false
  $name: Hide drive name
  $name:ru-RU: Скрывать имя диска
  $description: Shows only the drive letter in the first line, for example (E:). Disabled by default.
  $description:ru-RU: Оставляет только букву диска в верхней строке, например (E:). По умолчанию выключено.
- FreeColor: "green-red"
  $name: Free/used colors
  $name:ru-RU: Цвета свободно/всего
  $description: Preset colors for free and used space in capacity strips and backgrounds.
  $description:ru-RU: Пара цветов свободного и занятого места для полос и фонового отображения объёма.
  $options:
    - "green-red": "Green / Red"
    - "blue-orange": "Blue / Orange"
    - "cyan-purple": "Cyan / Purple"
    - "violet-yellow": "Violet / Yellow"
    - "teal-pink": "Teal / Pink"
    - "lime-indigo": "Lime / Indigo"
    - "amber-navy": "Amber / Navy"
    - "mint-coral": "Mint / Coral"
    - "sky-magenta": "Sky / Magenta"
    - "white-gray": "White / Gray"
  $options:ru-RU:
    - "green-red": "Зелёный / красный"
    - "blue-orange": "Синий / оранжевый"
    - "cyan-purple": "Бирюзовый / фиолетовый"
    - "violet-yellow": "Фиолетовый / жёлтый"
    - "teal-pink": "Бирюзовый / розовый"
    - "lime-indigo": "Лаймовый / индиго"
    - "amber-navy": "Янтарный / тёмно-синий"
    - "mint-coral": "Мятный / коралловый"
    - "sky-magenta": "Небесный / пурпурный"
    - "white-gray": "Белый / серый"
- DisplayOn: "primary"
  $name: Monitor selection for display
  $name:ru-RU: Выбор монитора для отображения
  $description: 'Choose the primary taskbar, all taskbars or a detected monitor. The list refreshes automatically; reopen it after connecting a display. Windows must show a taskbar on the selected monitor. Windhawk 1.7.3 uses a text field: primary, all or a device name such as \\.\DISPLAY2.'
  $description:ru-RU: 'Выберите основную панель, все панели или обнаруженный монитор. Список обновляется автоматически; после подключения экрана переоткройте его. Windows должна показывать панель задач на выбранном мониторе. В Windhawk 1.7.3 используется текстовое поле: primary, all или имя устройства, например \\.\DISPLAY2.'
  #! $dynamicSelect: true
  #! $options:
  #!   - "primary": "Primary taskbar"
  #!   - "all": "All taskbars"
  #! $options:ru-RU:
  #!   - "primary": "Основная панель"
  #!   - "all": "Все панели"
- LowSpaceMode: "off"
  $name: Low disk space warning
  $name:ru-RU: Индикация нехватки места
  $description: Changes text color when available space is at or below the threshold. No notifications are shown.
  $description:ru-RU: Меняет цвет текста, если доступное место не превышает порог. Уведомления не показываются.
  $options:
    - "off": "Disabled"
    - "percent": "Percent"
    - "gib": "GiB"
  $options:ru-RU:
    - "off": "Выключена"
    - "percent": "Проценты"
    - "gib": "ГиБ"
- LowSpaceThreshold: 10
  $name: Low space threshold
  $name:ru-RU: Порог нехватки места
  $description: Integer threshold in the selected unit. Percent values above 100 are treated as 100; GiB values are limited to 1000000.
  $description:ru-RU: Целый порог в выбранных единицах. Проценты выше 100 считаются как 100; ГиБ ограничены значением 1000000.
  #! $min: 0
  #! $max: 1000000
- UpdateInterval: 600
  $name: Update interval (seconds)
  $name:ru-RU: Интервал обновления (секунды)
  $description: How often to refresh the disk reading. The default is 600 seconds (10 minutes).
  $description:ru-RU: Как часто обновлять данные о диске. По умолчанию 600 секунд (10 минут).
  #! $min: 1
  #! $max: 3600
- LeftOffset: 160
  $name: Offset in automatic mode
  $name:ru-RU: Отступ в автоматическом режиме
  $description: Preferred offset within the free left section in Automatic position mode; clamped to fit the content. Left uses 15; Right is placed beside the tray.
  $description:ru-RU: Предпочтительный отступ внутри свободного левого участка при автоматическом положении; ограничивается доступным местом. Слева — 15; справа — возле трея.
  #! $min: 0
  #! $max: 1200
  #! $format: slider
- ReserveSpace: "auto"
  $name: Position relative to Start
  $name:ru-RU: Положение относительно пуска
  $description: Applies to horizontal taskbars. Automatic chooses a free left or right section; Left uses a fixed 15-DIP offset; Right places the indicator beside the tray. Vertical taskbars always use Automatic placement at the top.
  $description:ru-RU: Для горизонтальной панели. Автоматически выбирает свободный участок слева или справа; слева — отступ 15 логических единиц; справа — рядом с треем. На вертикальной панели всегда действует автоматическое размещение сверху.
  $options:
    - "auto": "Automatic"
    - "right": "Right"
    - "left": "Left"
  $options:ru-RU:
    - "auto": "автоматический"
    - "right": "справа"
    - "left": "слева"
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <shellapi.h>
#include <winioctl.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <limits>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>

using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;

namespace {
constexpr wchar_t kWidgetName[] = L"WindhawkTaskbarDiskSpace";
struct Settings {
    bool layoutDiagnostics = false;
    bool showAllDrives = false;
    std::wstring driveGroup = L"all";
    DWORD manualDrives = 0;
    bool autoCompact = true;
    bool letterTiles = false;
    bool summaryButton = false;
    int precision = 1;
    std::wstring displayOn = L"primary";
    std::wstring lowSpaceMode = L"off";
    int lowSpaceThreshold = 10;
    std::wstring format = L"free";
    std::wstring drive;
    std::wstring configuredDrive;
    std::wstring displayName;
    bool hideDriveName = false;
    std::wstring colorScheme = L"green-red";
    std::wstring appearance = L"bar";
    int interval = 600;
    int offset = 160;
    std::wstring position = L"auto";
};
struct Reading {
    // A single reading has no children; all-drive mode contains individual readings.

    std::wstring title;
    std::wstring capacity;
    double freeRatio = 0.0;
    bool hasRatio = false;
    bool lowSpace = false;
    std::wstring compactTitle;
    std::wstring compactCapacity;
    std::vector<Reading> drives;
};
std::wstring DriveTooltip(const Reading& reading) {
    if (reading.drives.empty()) return reading.title + L" — " + reading.capacity;
    std::wstring text;
    for (const auto& disk : reading.drives) {
        if (!text.empty()) text += L"\n";
        text += disk.title + L" — " + disk.capacity;
    }
    return text;
}

Reading HiddenDriveReading(const Reading& reading, size_t visible) {
    Reading hidden;
    if (visible < reading.drives.size())
        hidden.drives.assign(reading.drives.begin() + visible, reading.drives.end());
    return hidden;
}

struct TooltipDisk {
    std::wstring letter, name, capacity;
    double freeRatio = 0.0;
    bool hasRatio = false, lowSpace = false;
};

std::vector<TooltipDisk> TooltipDisks(const Reading& reading, bool showAll) {
    std::vector<TooltipDisk> disks;
    auto append = [&](const Reading& disk) {
        TooltipDisk item;
        item.letter = disk.compactTitle;
        if (item.letter.size() >= 4 && item.letter.front() == L'(' && item.letter.back() == L')')
            item.letter = item.letter.substr(1, item.letter.size() - 2);
        if (item.letter.empty()) item.letter = L"?";
        item.name = disk.title;
        if (!disk.compactTitle.empty()) {
            const auto suffix = L" " + disk.compactTitle;
            if (item.name.ends_with(suffix)) item.name.resize(item.name.size() - suffix.size());
            else if (item.name == disk.compactTitle) item.name.clear();
        }
        item.capacity = disk.capacity;
        item.hasRatio = disk.hasRatio && std::isfinite(disk.freeRatio);
        item.freeRatio = item.hasRatio ? std::clamp(disk.freeRatio, 0.0, 1.0) : 0.0;
        item.lowSpace = disk.lowSpace;
        disks.push_back(std::move(item));
    };
    if (!reading.drives.empty()) {
        for (const auto& disk : reading.drives) append(disk);
    } else if (!showAll) {
        append(reading);
    }
    return disks;
}

struct TooltipLayout {
    size_t columns = 1, rows = 1, visible = 0;
    double width = 330;
};

TooltipLayout PassiveTooltipLayout(size_t count, double workWidth, double workHeight) {
    TooltipLayout layout;
    if (!std::isfinite(workWidth) || workWidth <= 0) workWidth = 800;
    if (!std::isfinite(workHeight) || workHeight <= 0) workHeight = 600;
    const double availableWidth = std::max(1.0,workWidth-62.0);
    const double availableHeight = std::max(1.0,workHeight-140.0);
    const auto maxRows = std::max<size_t>(1,static_cast<size_t>(availableHeight/104.0));
    const auto maxColumns = std::max<size_t>(1,static_cast<size_t>((availableWidth+8.0)/248.0));
    layout.columns = std::min(maxColumns,std::max<size_t>(1,(count+maxRows-1)/maxRows));
    layout.visible = std::min(count,maxRows*layout.columns);
    layout.rows = std::min(maxRows,std::max<size_t>(1,layout.visible));
    const double columnWidth = std::min(330.0,
        (availableWidth-8.0*(layout.columns-1))/layout.columns);
    layout.width = columnWidth*layout.columns+8.0*(layout.columns-1);
    return layout;
}

BYTE EffectiveBrushAlpha(BYTE alpha, double opacity) {
    if (!std::isfinite(opacity)) return 0;
    return static_cast<BYTE>(std::lround(alpha * std::clamp(opacity, 0.0, 1.0)));
}

double DriveBackgroundOpacity(std::wstring_view appearance, bool hasRatio,
                              bool hovered, bool highContrast) {
    if (appearance == L"text") return 0.0;
    if (appearance == L"background" && hasRatio && !highContrast)
        return 1.0;
    return hovered ? 1.0 : 0.0;
}

struct DriveCell {
    Border surface{nullptr};
    LinearGradientBrush hover{nullptr};
    Border highlightLayer{nullptr};
    SolidColorBrush highlight{nullptr};
    winrt::Windows::UI::Xaml::Media::Animation::Storyboard animation{nullptr};
    winrt::event_token entered{}, exited{};
    bool handlers = false;
    bool hovered = false;
    bool letterTile = false;
    double hoverTarget = 0.0;
    double highlightTarget = 0.0;
    double freeRatio = 0;
    bool hasRatio = false;
    StackPanel panel{nullptr};
    TextBlock title{nullptr};
    TextBlock capacity{nullptr};
    Grid bar{nullptr};
    Border freePart{nullptr};
    Border usedPart{nullptr};
    ColumnDefinition freeColumn{nullptr};
    ColumnDefinition usedColumn{nullptr};
};
// XAML references must be released on their owning UI thread, never by static
// destructors during Explorer process shutdown.
struct UiState {
    std::wstring diagnosticSignature;
    std::wstring tooltipSignature;
    ToolTip summaryTip{nullptr};
    FrameworkElement tooltipOwner{nullptr};
    bool summary = false;
    Border overflowSurface{nullptr};
    TextBlock overflowText{nullptr};
    size_t visibleDrives = 0, hiddenDrives = 0;
    std::wstring listLayoutSignature;
    int listLayoutMode = -1;
    FrameworkElement frame{nullptr};
    FrameworkElement startButton{nullptr};
    FrameworkElement startBackground{nullptr};
    Grid root{nullptr};
    Border surface{nullptr};
    LinearGradientBrush hoverBrush{nullptr};
    GradientStop freeStop{nullptr};
    GradientStop freeEndStop{nullptr};
    GradientStop usedStartStop{nullptr};
    GradientStop usedStop{nullptr};
    MenuFlyout driveMenu{nullptr};
    StackPanel widget{nullptr};
    ScrollViewer verticalViewport{nullptr};
    TextBlock title{nullptr};
    TextBlock capacity{nullptr};
    std::vector<DriveCell> driveCells;
    FrameworkElement repeater{nullptr};
    double reserved = 0;
    double lastMargin = 0;
    bool marginApplied = false;
    DWORD thread = 0;
    winrt::event_token tappedToken{};
    bool hasTappedHandler = false;
    winrt::event_token rightTappedToken{};
    bool hasRightTappedHandler = false;
    winrt::event_token pointerEnteredToken{};
    winrt::event_token pointerExitedToken{};
    bool hasPointerHandlers = false;
    bool hovered = false;
    bool pressed = false;
    winrt::event_token pointerPressedToken{};
    winrt::event_token pointerReleasedToken{};
    winrt::event_token pointerCanceledToken{};
    winrt::Windows::UI::Xaml::Media::Animation::Storyboard hoverAnimation{nullptr};
    Color targetFree{};
    Color targetUsed{};
    double freeRatio = 0.0;
    bool hasRatio = false;
    std::wstring colorScheme = L"green-red";
    std::wstring appearance = L"bar";
};
// A dispatch and every input handler select their own taskbar's state. XAML
// references remain on their owning thread, including when panels share a thread.
thread_local UiState* g_activeUi = nullptr;
struct UiScope {
    UiState* previous;
    explicit UiScope(UiState* state) : previous(g_activeUi) { g_activeUi = state; }
    ~UiScope() { g_activeUi = previous; }
};
#define g_ui (*g_activeUi)
[[clang::no_destroy]] Settings g_settings;
std::mutex g_settingsMutex;
HANDLE g_stop = nullptr;
HANDLE g_changed = nullptr;
HANDLE g_worker = nullptr;
HMODULE g_taskbarModule = nullptr;
UINT g_dispatchMessage = 0;
constexpr wchar_t kSelectedDriveValue[] = L"SelectedDrive";
constexpr wchar_t kConfiguredDriveValue[] = L"ConfiguredDrive";

using GetHost_t = void*(WINAPI*)(void*, void*);
GetHost_t g_getHost = nullptr;
GetHost_t g_getSecondaryHost = nullptr;
using FrameHeight_t = int(WINAPI*)(void*);
FrameHeight_t g_frameHeight = nullptr;
using Decref_t = void(WINAPI*)(void*);
Decref_t g_decref = nullptr;
void* g_siteVtable = nullptr;
void* g_secondarySiteVtable = nullptr;
size_t g_elementOffset = 0;

std::wstring StringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result(value ? value : L"");
    Wh_FreeStringSetting(value);
    return result;
}

bool IsRussianUi() {
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_RUSSIAN;
}

bool TaskbarIconsLeftAligned() {
    HKEY key = nullptr;
    constexpr wchar_t keyPath[] =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
    if (RegOpenKeyExW(HKEY_CURRENT_USER, keyPath, 0, KEY_READ, &key) !=
        ERROR_SUCCESS) {
        return false;
    }
    DWORD alignment = 1;
    DWORD type = 0;
    DWORD size = sizeof(alignment);
    const LONG result = RegQueryValueExW(key, L"TaskbarAl", nullptr, &type,
                                         reinterpret_cast<BYTE*>(&alignment), &size);
    RegCloseKey(key);
    // Windows uses 0 for left and 1 for centered. Missing or malformed values
    // are treated as centered, which avoids changing the user's layout.
    return result == ERROR_SUCCESS && type == REG_DWORD && size == sizeof(alignment) &&
           alignment == 0;
}

std::wstring UiText(bool russian, PCWSTR english, PCWSTR russianText) {
    return russian ? std::wstring(russianText) : std::wstring(english);
}

std::wstring FormatGiB(double value, bool russian, int precision) {
    precision = std::clamp(precision, 0, 2);
    std::wstring input = std::to_wstring(value);
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH]{};
    wchar_t output[64]{};
    const bool localeAvailable = russian
        ? (wcscpy_s(localeName, ARRAYSIZE(localeName), L"ru-RU") == 0)
        : (GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName)) != 0);
    wchar_t decimalSeparator[8]{};
    wchar_t thousandSeparator[8]{};
    NUMBERFMTW numberFormat{};
    numberFormat.NumDigits = precision;
    numberFormat.LeadingZero = 1;
    numberFormat.Grouping = 0;
    numberFormat.lpDecimalSep = decimalSeparator;
    numberFormat.lpThousandSep = thousandSeparator;
    if (localeAvailable &&
        GetLocaleInfoEx(localeName, LOCALE_SDECIMAL, decimalSeparator,
                        ARRAYSIZE(decimalSeparator)) &&
        GetLocaleInfoEx(localeName, LOCALE_STHOUSAND, thousandSeparator,
                        ARRAYSIZE(thousandSeparator)) &&
        GetNumberFormatEx(localeName, 0, input.c_str(), &numberFormat, output,
                          ARRAYSIZE(output)) &&
        output[0] != L'\0') {
        std::wstring formatted(output);
        return formatted;
    }
    _snwprintf_s(output, ARRAYSIZE(output), _TRUNCATE, L"%.*f", precision, value);
    std::wstring fallback(output);
    if (russian) std::replace(fallback.begin(), fallback.end(), L'.', L',');
    return fallback;
}

std::wstring LocalStringValue(PCWSTR name) {
    wchar_t value[4096]{};
    if (!Wh_GetStringValue(name, value, ARRAYSIZE(value))) return {};
    return value;
}

// Only accept a drive letter, not an arbitrary path or a network share.
std::wstring NormalizeDrive(std::wstring value) {
    const auto first = value.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return {};
    value = value.substr(first, value.find_last_not_of(L" \t\r\n") - first + 1);
    if (value.size() == 3 && (value[2] == L'\\' || value[2] == L'/')) value.pop_back();
    if (value.size() == 1) value += L':';
    if (value.size() != 2 || value[1] != L':') return {};
    value[0] = static_cast<wchar_t>(towupper(value[0]));
    if (value[0] < L'A' || value[0] > L'Z') return {};
    return value;
}

DWORD ParseManualDrives(std::wstring_view text) {
    DWORD mask = 0;
    size_t cursor = 0;
    while (cursor < text.size()) {
        const auto end = text.find_first_of(L";, \t\r\n",cursor);
        const auto token = text.substr(cursor,end == std::wstring_view::npos ? text.size()-cursor : end-cursor);
        const auto drive = NormalizeDrive(std::wstring(token));
        if (!drive.empty()) mask |= DWORD{1} << (drive[0]-L'A');
        if (end == std::wstring_view::npos) break;
        cursor = end+1;
    }
    return mask;
}

std::wstring SerializeManualDrives(DWORD mask) {
    std::wstring text;
    for (int letter = 0; letter < 26; ++letter) {
        if (!(mask & (DWORD{1} << letter))) continue;
        if (!text.empty()) text += L';';
        text += static_cast<wchar_t>(L'A'+letter);
        text += L':';
    }
    return text;
}

bool MatchesDriveGroup(const Settings& settings, const std::wstring& drive, bool external) {
    if (settings.driveGroup == L"local") return !external;
    if (settings.driveGroup == L"external") return external;
    if (settings.driveGroup == L"manual") {
        const auto normalized = NormalizeDrive(drive);
        return !normalized.empty() && (settings.manualDrives & (DWORD{1} << (normalized[0]-L'A'))) != 0;
    }
    return true;
}

void LoadSettings() {
    Settings settings;
    settings.layoutDiagnostics = Wh_GetIntSetting(L"LayoutDiagnostics") != 0;
    const auto configuredAll = Wh_GetIntSetting(L"ShowAllDrives") ? L"1" : L"0";
    auto selectedAll = LocalStringValue(L"SelectedAllDrives");
    if (LocalStringValue(L"ConfiguredAllDrives") != configuredAll ||
        (selectedAll != L"0" && selectedAll != L"1")) {
        selectedAll = configuredAll;
        Wh_DeleteValue(L"SelectedDriveGroup");
        Wh_SetStringValue(L"ConfiguredAllDrives", configuredAll);
        Wh_SetStringValue(L"SelectedAllDrives", selectedAll.c_str());
    }
    settings.showAllDrives = selectedAll == L"1";
    settings.driveGroup = LocalStringValue(L"SelectedDriveGroup");
    if (settings.driveGroup != L"local" && settings.driveGroup != L"external" && settings.driveGroup != L"manual")
        settings.driveGroup = L"all";
    settings.manualDrives = ParseManualDrives(LocalStringValue(L"SelectedManualDrives"));
    const std::wstring configuredDrive = NormalizeDrive(StringSetting(L"Drive"));
    const std::wstring previousConfigured =
        NormalizeDrive(LocalStringValue(kConfiguredDriveValue));
    std::wstring selectedDrive = NormalizeDrive(LocalStringValue(kSelectedDriveValue));
    // The settings field is the initial/default choice. A panel click is kept
    // separately, but changing the settings field intentionally starts there.
    if (previousConfigured != configuredDrive || selectedDrive.empty()) {
        selectedDrive = configuredDrive;
        Wh_SetStringValue(kConfiguredDriveValue, configuredDrive.c_str());
        Wh_SetStringValue(kSelectedDriveValue, selectedDrive.c_str());
    }
    settings.drive = std::move(selectedDrive);
    settings.configuredDrive = configuredDrive;
    settings.displayName = StringSetting(L"DisplayName");
    const std::wstring configuredMini = Wh_GetIntSetting(L"HideDriveName") ? L"1" : L"0";
    auto selectedMini = LocalStringValue(L"SelectedMiniDesign");
    if (LocalStringValue(L"ConfiguredMiniDesign") != configuredMini ||
        (selectedMini != L"0" && selectedMini != L"1")) {
        selectedMini = configuredMini;
        Wh_SetStringValue(L"ConfiguredMiniDesign", configuredMini.c_str());
        Wh_SetStringValue(L"SelectedMiniDesign", selectedMini.c_str());
    }
    settings.hideDriveName = selectedMini == L"1";
    settings.letterTiles = LocalStringValue(L"SelectedLetterTiles") == L"1";
    settings.summaryButton = LocalStringValue(L"SelectedSummaryButton") == L"1";
    const std::wstring colorScheme = StringSetting(L"FreeColor");
    if (colorScheme == L"green-red" || colorScheme == L"blue-orange" ||
        colorScheme == L"cyan-purple" || colorScheme == L"violet-yellow" ||
        colorScheme == L"teal-pink" || colorScheme == L"lime-indigo" ||
        colorScheme == L"amber-navy" || colorScheme == L"mint-coral" ||
        colorScheme == L"sky-magenta" || colorScheme == L"white-gray") {
        settings.colorScheme = colorScheme;
    } else if (colorScheme == L"green" || colorScheme.empty()) {
        // Keep old installations on the original green/red appearance.
        // Старые установки сохраняют исходную зелёно-красную палитру.
        settings.colorScheme = L"green-red";
    } else if (colorScheme == L"blue") {
        settings.colorScheme = L"blue-orange";
    } else if (colorScheme == L"cyan") {
        settings.colorScheme = L"cyan-purple";
    } else if (colorScheme == L"purple") {
        settings.colorScheme = L"violet-yellow";
    } else if (colorScheme == L"orange") {
        settings.colorScheme = L"amber-navy";
    }
    // A context-menu choice persists until the configured color scheme changes.
    const auto configuredTheme = settings.colorScheme;
    auto selectedTheme = LocalStringValue(L"SelectedTheme");
    const std::array<std::wstring_view, 10> themeIds{
        L"green-red", L"blue-orange", L"cyan-purple", L"violet-yellow",
        L"teal-pink", L"lime-indigo", L"amber-navy", L"mint-coral",
        L"sky-magenta", L"white-gray"};
    if (LocalStringValue(L"ConfiguredTheme") != configuredTheme ||
        std::find(themeIds.begin(), themeIds.end(), selectedTheme) == themeIds.end()) {
        selectedTheme = configuredTheme;
        Wh_SetStringValue(L"ConfiguredTheme", configuredTheme.c_str());
        Wh_SetStringValue(L"SelectedTheme", selectedTheme.c_str());
    }
    settings.colorScheme = selectedTheme;
    settings.autoCompact = true;
    if (LocalStringValue(L"SelectedAutoCompact") != L"1")
        Wh_SetStringValue(L"SelectedAutoCompact", L"1");
    const auto precision = LocalStringValue(L"SelectedPrecision");
    if (precision == L"0" || precision == L"1" || precision == L"2")
        settings.precision = precision[0] - L'0';
    const auto format = LocalStringValue(L"SelectedFormat");
    if (format == L"free" || format == L"used" || format == L"percent")
        settings.format = format;
    const auto appearance = LocalStringValue(L"SelectedAppearance");
    if (appearance == L"system" || appearance == L"bar" ||
        appearance == L"background" || appearance == L"text")
        settings.appearance = appearance;
    // Annotations are editor hints, so validate even on Windhawk 2.0.
    settings.displayOn = StringSetting(L"DisplayOn");
    // Preserve the older two-field setting when upgrading an installed mod.
    if (settings.displayOn == L"monitor") {
        settings.displayOn = StringSetting(L"MonitorDevice");
        if (!settings.displayOn.empty())
            Wh_SetStringValue(L"LegacyMonitorDevice", settings.displayOn.c_str());
        else settings.displayOn = LocalStringValue(L"LegacyMonitorDevice");
    }
    if (settings.displayOn.empty()) settings.displayOn = L"primary";
    const auto configuredMonitor = settings.displayOn;
    auto selectedMonitor = LocalStringValue(L"SelectedMonitor");
    if (LocalStringValue(L"ConfiguredMonitor") != configuredMonitor || selectedMonitor.empty()) {
        selectedMonitor = configuredMonitor;
        Wh_SetStringValue(L"ConfiguredMonitor", configuredMonitor.c_str());
        Wh_SetStringValue(L"SelectedMonitor", selectedMonitor.c_str());
    }
    settings.displayOn = selectedMonitor;
    settings.lowSpaceMode = StringSetting(L"LowSpaceMode");
    if (settings.lowSpaceMode != L"percent" && settings.lowSpaceMode != L"gib")
        settings.lowSpaceMode = L"off";
    settings.lowSpaceThreshold = std::clamp(Wh_GetIntSetting(L"LowSpaceThreshold"),
        0, settings.lowSpaceMode == L"percent" ? 100 : 1000000);
    const auto configuredLowSpace = settings.lowSpaceMode + L":" + std::to_wstring(settings.lowSpaceThreshold);
    auto selectedLowSpace = LocalStringValue(L"SelectedLowSpace");
    if (LocalStringValue(L"ConfiguredLowSpace") != configuredLowSpace || selectedLowSpace.empty()) {
        selectedLowSpace = configuredLowSpace;
        Wh_SetStringValue(L"ConfiguredLowSpace", configuredLowSpace.c_str());
        Wh_SetStringValue(L"SelectedLowSpace", selectedLowSpace.c_str());
    }
    const auto separator = selectedLowSpace.find(L':');
    if (separator != std::wstring::npos) {
        const auto mode = selectedLowSpace.substr(0, separator);
        const auto value = selectedLowSpace.substr(separator + 1);
        if ((mode == L"off" || mode == L"percent" || mode == L"gib") &&
            !value.empty() && value.size() <= 7 &&
            value.find_first_not_of(L"0123456789") == std::wstring::npos) {
            settings.lowSpaceMode = mode;
            settings.lowSpaceThreshold = std::clamp(static_cast<int>(wcstol(value.c_str(), nullptr, 10)),
                0, mode == L"percent" ? 100 : 1000000);
        }
    }
    settings.interval = std::clamp(Wh_GetIntSetting(L"UpdateInterval"), 1, 3600);
    settings.offset = std::clamp(Wh_GetIntSetting(L"LeftOffset"), 0, 1200);
    settings.position = StringSetting(L"ReserveSpace");
    // Keep the old setting key so existing installations retain their choice.
    if (settings.position == L"always" || settings.position == L"true" ||
        settings.position == L"1") settings.position = L"left";
    else if (settings.position != L"left" && settings.position != L"right")
        settings.position = L"auto";
    const auto configuredPosition = settings.position;
    auto selectedPosition = LocalStringValue(L"SelectedPosition");
    if (LocalStringValue(L"ConfiguredPosition") != configuredPosition ||
        (selectedPosition != L"auto" && selectedPosition != L"left" && selectedPosition != L"right")) {
        selectedPosition = configuredPosition;
        Wh_SetStringValue(L"ConfiguredPosition", configuredPosition.c_str());
        Wh_SetStringValue(L"SelectedPosition", selectedPosition.c_str());
    }
    settings.position = selectedPosition;
    std::lock_guard lock(g_settingsMutex);
    g_settings = std::move(settings);
}

Settings CurrentSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

std::wstring VolumeName(const std::wstring& root) {
    wchar_t label[MAX_PATH + 1]{};
    if (!GetVolumeInformationW(root.c_str(), label, ARRAYSIZE(label), nullptr,
                               nullptr, nullptr, nullptr, 0)) return {};
    std::wstring result(label);
    // Labels go into both XAML and the dynamic-selection local storage.
    std::replace(result.begin(), result.end(), L'\r', L' ');
    std::replace(result.begin(), result.end(), L'\n', L' ');
    return result;
}

std::wstring CapacityText(ULONGLONG freeBytes, ULONGLONG totalBytes, bool compact,
                          bool russian, const std::wstring& format, int precision) {
    constexpr double gib = 1024.0 * 1024.0 * 1024.0;
    if (format == L"percent") {
        if (!totalBytes) return UiText(russian, L"Free — %", L"Свободно — %");
        const double percentage = std::clamp(static_cast<double>(freeBytes) /
            static_cast<double>(totalBytes), 0.0, 1.0) * 100.0;
        return (compact ? L"" : (russian ? L"Свободно " : L"Free ")) +
               FormatGiB(percentage, russian, precision) + L" %";
    }
    const bool used = format == L"used";
    const ULONGLONG displayedBytes = used ? totalBytes - std::min(freeBytes, totalBytes) : freeBytes;
    const std::wstring free = FormatGiB(displayedBytes / gib, russian, precision);
    const std::wstring total = FormatGiB(totalBytes / gib, russian, precision);
    if (compact) {
        return free + L" / " + total + (russian ? L" ГиБ" : L" GiB");
    }
    return std::wstring(used ? (russian ? L"Занято " : L"Used ") :
                              (russian ? L"Свободно " : L"Free ")) + free +
           (russian ? L" из " : L" of ") + total +
           (russian ? L" ГиБ" : L" GiB");
}

bool LowSpaceReached(ULONGLONG available, ULONGLONG total,
                     const std::wstring& mode, int threshold) {
    if (!total) return false;
    if (mode == L"percent")
        return static_cast<long double>(available) * 100.0L <=
            static_cast<long double>(total) * std::clamp(threshold, 0, 100);
    if (mode == L"gib")
        return available <= static_cast<ULONGLONG>(std::clamp(threshold, 0, 1000000)) * (1ULL << 30);
    return false;
}

Reading ReadDisk(const Settings& settings, bool russian) {
    if (settings.drive.empty()) {
        return {UiText(russian, L"Select a drive", L"Выберите диск"),
                UiText(russian, L"Invalid drive letter", L"Некорректная буква диска")};
    }
    std::wstring root = settings.drive + L"\\";
    const UINT driveType = GetDriveTypeW(root.c_str());
    const bool fixed = driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE;
    std::wstring title;
    if (settings.hideDriveName) {
        // Только буква диска / Drive letter only.
        title = L"(" + settings.drive + L")";
    } else {
        std::wstring label;
        if (!settings.displayName.empty() &&
            (settings.configuredDrive.empty() || settings.drive == settings.configuredDrive)) {
            label = settings.displayName;
        } else if (fixed) {
            label = VolumeName(root);
        }
        if (label.empty()) label = UiText(russian, L"Local Disk", L"Локальный диск");
        title = label + L" (" + settings.drive + L")";
    }
    Reading result{title, UiText(russian, L"Drive unavailable", L"Диск недоступен")};
    result.compactTitle = L"(" + settings.drive + L")";
    result.compactCapacity = result.capacity;
    if (!fixed) return result;
    ULARGE_INTEGER available{}, total{};
    if (GetDiskFreeSpaceExW(root.c_str(), &available, &total, nullptr)) {
        result.capacity = CapacityText(available.QuadPart, total.QuadPart,
                                       settings.hideDriveName, russian, settings.format, settings.precision);
        result.compactCapacity = CapacityText(available.QuadPart, total.QuadPart,
            true, russian, settings.format, settings.precision);
        if (total.QuadPart != 0) {
            result.freeRatio = std::clamp(
                static_cast<double>(available.QuadPart) /
                    static_cast<double>(total.QuadPart),
                0.0, 1.0);
            result.hasRatio = true;
            result.lowSpace = LowSpaceReached(available.QuadPart, total.QuadPart,
                settings.lowSpaceMode, settings.lowSpaceThreshold);
        }
    }
    return result;
}

struct DriveOption { std::wstring drive; std::wstring label; bool external = false; };
struct MonitorOption {
    std::wstring device;
    std::wstring label;
    std::wstring id;
};

std::wstring ResolveMonitorDevice(const std::wstring& selected,
                                 const std::vector<MonitorOption>& monitors) {
    for (const auto& monitor : monitors) {
        if (_wcsicmp(selected.c_str(), monitor.id.c_str()) == 0 && !monitor.id.empty())
            return monitor.device;
    }
    // Unavailable physical IDs remain unmatched; legacy GDI names still work.
    return selected;
}
std::mutex g_menuCacheMutex;
std::vector<DriveOption> g_cachedDrives;
std::vector<MonitorOption> g_cachedMonitors;

std::vector<DriveOption> CachedDrives() {
    std::lock_guard lock(g_menuCacheMutex);
    return g_cachedDrives;
}

std::vector<MonitorOption> CachedMonitors() {
    std::lock_guard lock(g_menuCacheMutex);
    return g_cachedMonitors;
}

Reading ReadDrives(const Settings& settings, bool russian) {
    if (!settings.showAllDrives) return ReadDisk(settings, russian);
    Reading result;
    for (const auto& option : CachedDrives()) {
        if (!MatchesDriveGroup(settings,option.drive,option.external)) continue;
        Settings diskSettings = settings;
        diskSettings.drive = option.drive;
        auto disk = ReadDisk(diskSettings, russian);
        if (!result.title.empty()) result.title += L"; ";
        result.title += disk.title + L": " + disk.capacity;
        result.drives.push_back(std::move(disk));
    }
    if (result.drives.empty()) {
        const bool emptySelection = settings.driveGroup == L"manual" && !settings.manualDrives;
        result.title = emptySelection ? UiText(russian,L"No drives selected",L"Диски не выбраны") :
            UiText(russian, L"No matching drives", L"Нет дисков выбранной группы");
        result.capacity = emptySelection ? UiText(russian,L"Choose drives in the menu",L"Отметьте диски в меню") :
            UiText(russian, L"Drive unavailable", L"Диск недоступен");
    }
    return result;
}

void SetAllDrives(bool enabled, const std::wstring& group = L"all") {
    {
        std::lock_guard lock(g_settingsMutex);
        Wh_SetStringValue(L"SelectedDriveGroup", group.c_str());
        Wh_SetStringValue(L"SelectedAllDrives", enabled ? L"1" : L"0");
        g_settings.showAllDrives = enabled;
        g_settings.driveGroup = group;
    }
    if (g_changed) SetEvent(g_changed);
}

void SetManualDrive(const std::wstring& drive, bool enabled) {
    const auto normalized = NormalizeDrive(drive);
    if (normalized.empty()) return;
    const DWORD bit = DWORD{1} << (normalized[0]-L'A');
    {
        // Merge into the current selection, so different taskbar menus cannot
        // overwrite each other's choices with an old captured settings snapshot.
        std::lock_guard lock(g_settingsMutex);
        if (enabled) g_settings.manualDrives |= bit;
        else g_settings.manualDrives &= ~bit;
        const auto value = SerializeManualDrives(g_settings.manualDrives);
        Wh_SetStringValue(L"SelectedManualDrives",value.c_str());
        Wh_SetStringValue(L"SelectedDriveGroup",L"manual");
        Wh_SetStringValue(L"SelectedAllDrives",L"1");
        g_settings.showAllDrives = true;
        g_settings.driveGroup = L"manual";
    }
    if (g_changed) SetEvent(g_changed);
}

void SelectDrive(const std::wstring& drive) {
    const auto drives = CachedDrives();
    if (std::none_of(drives.begin(), drives.end(), [&](const auto& option) {
            return option.drive == drive;
        })) return;
    {
        std::lock_guard lock(g_settingsMutex);
        Wh_SetStringValue(kSelectedDriveValue, drive.c_str());
        Wh_SetStringValue(L"SelectedAllDrives", L"0");
        g_settings.drive = drive;
        g_settings.showAllDrives = false;
    }
    if (g_changed) SetEvent(g_changed);
    Wh_Log(L"Selected drive changed from the taskbar to %s", drive.c_str());
}

FontIcon MenuIcon(PCWSTR glyph);

void MenuHint(FrameworkElement const& item, const std::wstring& text) {
    // Let XAML own the plain-text tooltip and its open/close lifetime.
    ToolTipService::SetToolTip(item,winrt::box_value(text));
    Automation::AutomationProperties::SetHelpText(item,text);
}

void MenuHint(FrameworkElement const& item, bool russian, PCWSTR english, PCWSTR translated) {
    MenuHint(item,UiText(russian,english,translated));
}

void DriveMiddleClick(UIElement const& item, const std::wstring& drive) {
    // Receive middle clicks even if the menu handles the routed event internally.
    item.AddHandler(UIElement::PointerPressedEvent(),winrt::box_value(
        winrt::Windows::UI::Xaml::Input::PointerEventHandler{
            [drive](auto const& sender, auto const& args) {
                if (!args.GetCurrentPoint(sender.template as<UIElement>()).Properties().IsMiddleButtonPressed()) return;
                args.Handled(true);
                const auto root = drive+L"\\";
                const auto result = reinterpret_cast<INT_PTR>(ShellExecuteW(
                    nullptr,L"open",root.c_str(),nullptr,nullptr,SW_SHOWNORMAL));
                if (result <= 32) Wh_Log(L"Cannot open drive %s: ShellExecute error %d",
                    drive.c_str(),static_cast<int>(result));
            }}),true);
}

void AppendManualDriveMenu(MenuFlyout const& menu, bool russian, const Settings& settings,
                           const std::vector<DriveOption>& drives) {
    MenuFlyoutSubItem manual;
    const bool active = settings.showAllDrives && settings.driveGroup == L"manual";
    manual.Text((active ? std::wstring(L"✓ ") : std::wstring{})+
        UiText(russian,L"Selected drives",L"Выбранные диски"));
    manual.Icon(MenuIcon(L"\uE73A"));
    MenuHint(manual,russian,L"Check the drives to display; the saved selection follows drive letters",
        L"Отметьте диски для отображения; сохранённый выбор привязан к буквам");
    ToggleMenuFlyoutItem useSelection;
    useSelection.Text(UiText(russian,L"Show selected drives",L"Показывать выбранные диски"));
    useSelection.Icon(MenuIcon(L"\uE73A"));
    useSelection.IsChecked(active);
    MenuHint(useSelection,russian,L"Restore this saved set; turning off returns to the selected single drive",
        L"Включить сохранённый набор; выключение возвращает выбранный одиночный диск");
    useSelection.Click([](auto const& sender, auto const&) {
        SetAllDrives(sender.template as<ToggleMenuFlyoutItem>().IsChecked(),L"manual");
    });
    manual.Items().Append(useSelection);
    manual.Items().Append(MenuFlyoutSeparator());
    DWORD available = 0;
    auto append = [&](const std::wstring& drive, const std::wstring& label, bool external, bool connected) {
        ToggleMenuFlyoutItem item;
        item.Text(label+L" ("+drive+L")"+(connected ? L"" : UiText(russian,L" — disconnected",L" — отключён")));
        item.Icon(MenuIcon(connected ? (external ? L"\uE88E" : L"\uEDA2") : L"\uE8B7"));
        item.IsChecked((settings.manualDrives & ParseManualDrives(drive)) != 0);
        if (!connected) item.Opacity(0.55);
        MenuHint(item,russian,
            connected ? L"Check to include this drive; middle-click opens it in File Explorer" :
                L"Saved drive is disconnected; uncheck to remove it from the set",
            connected ? L"Отметьте для включения в набор; средняя кнопка открывает диск в Проводнике" :
                L"Сохранённый диск отключён; снимите отметку, чтобы убрать его из набора");
        item.Click([drive](auto const& sender, auto const&) {
            SetManualDrive(drive,sender.template as<ToggleMenuFlyoutItem>().IsChecked());
        });
        if (connected) DriveMiddleClick(item,drive);
        manual.Items().Append(item);
    };
    for (const auto& option : drives) {
        available |= ParseManualDrives(option.drive);
        const auto label = option.label.empty() ? UiText(russian,
            option.external ? L"External drive" : L"Local drive",
            option.external ? L"Внешний диск" : L"Локальный диск") : option.label;
        append(option.drive,label,option.external,true);
    }
    const DWORD disconnected = settings.manualDrives & ~available;
    if (disconnected) manual.Items().Append(MenuFlyoutSeparator());
    for (int letter = 0; letter < 26; ++letter) {
        if (disconnected & (DWORD{1} << letter))
            append(std::wstring{static_cast<wchar_t>(L'A'+letter),L':'},
                UiText(russian,L"Saved drive",L"Сохранённый диск"),false,false);
    }
    if (drives.empty() && !disconnected) {
        MenuFlyoutItem empty;
        empty.Text(UiText(russian,L"No available drives",L"Нет доступных дисков"));
        empty.Opacity(0.55);
        MenuHint(empty,russian,L"Reconnect the device and reopen the menu",
            L"Подключите устройство и переоткройте меню");
        manual.Items().Append(empty);
    }
    menu.Items().Append(manual);
}

void ShowDriveMenu() {
    if (!g_ui.surface) return;
    const bool russian = IsRussianUi();
    const auto drives = CachedDrives();
    const Settings settings = CurrentSettings();
    MenuFlyout menu;
    for (const auto* group : {L"all", L"local", L"external"}) {
        ToggleMenuFlyoutItem item;
        const std::wstring selection = group;
        const bool external = selection == L"external";
        item.Text(selection == L"all" ? UiText(russian, L"Automatic mode", L"Автоматический режим") :
            external ? UiText(russian, L"External drives", L"Внешние диски") :
            UiText(russian, L"Local drives", L"Локальные диски"));
        item.Icon(MenuIcon(selection == L"all" ? L"\uE713" : external ? L"\uE88E" : L"\uEDA2"));
        MenuHint(item,russian,
            selection == L"all" ? L"Show all detected drives; only fitting cards appear on the taskbar" :
            external ? L"Show only USB flash drives and external disks" : L"Show only internal local drives",
            selection == L"all" ? L"Показывать все обнаруженные диски; на панели — только помещающиеся карточки" :
            external ? L"Показывать только USB-флешки и внешние диски" : L"Показывать только внутренние локальные диски");
        item.IsChecked(settings.showAllDrives && settings.driveGroup == selection);
        item.Click([selection](auto const&, auto const&) { SetAllDrives(true, selection); });
        menu.Items().Append(item);
    }
    AppendManualDriveMenu(menu,russian,settings,drives);
    menu.Items().Append(MenuFlyoutSeparator());
    for (const auto& option : drives) {
        const auto& drive = option.drive;
        MenuFlyoutItem item;
        item.Icon(MenuIcon(option.external ? L"\uE88E" : L"\uEDA2"));
        std::wstring label = option.label;
        if (label.empty()) label = option.external ? UiText(russian, L"External drive", L"Внешний диск") : UiText(russian, L"Local Disk", L"Локальный диск");
        std::wstring text = (!settings.showAllDrives && drive == settings.drive ? L"✓ " : L"  ") +
                            label + L" (" + drive + L")";
        item.Text(text);
        MenuHint(item,russian,
            L"Left-click to show this drive; middle-click to open it in File Explorer",
            L"Левая кнопка — показывать этот диск; средняя — открыть его в Проводнике");
        DriveMiddleClick(item,drive);

        item.Click([drive](winrt::Windows::Foundation::IInspectable const&,
                           winrt::Windows::UI::Xaml::RoutedEventArgs const&) {
            SelectDrive(drive);
        });
        menu.Items().Append(item);
    }
    if (drives.empty()) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE8B7"));
        item.Text(UiText(russian, L"No local drives available",
                         L"Нет доступных локальных дисков"));
        MenuHint(item,russian,L"No supported drive letters were detected; reconnect the device and reopen the menu",
            L"Подходящие диски с буквами не найдены; подключите устройство и переоткройте меню");
        item.Opacity(0.55); // Informational, with no Click action; keep hover hints accessible.
        menu.Items().Append(item);
    }
    // MenuFlyout is a native WinUI control, so its open/close animation,
    // shadows, corner radius and theme colors follow the taskbar automatically.
    g_ui.driveMenu = menu;
    g_ui.driveMenu.ShowAt(g_ui.surface);
}

std::vector<MonitorOption> MonitorFriendlyNames() {
    std::vector<MonitorOption> names;
    // Match the CCD source name to the GDI device used by taskbar selection.
    for (int attempt = 0; attempt < 3; ++attempt) {
        UINT32 pathCount = 0, modeCount = 0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS)
            return names;
        std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
        const LONG result = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(),
                                               &modeCount, modes.data(), nullptr);
        if (result == ERROR_INSUFFICIENT_BUFFER) continue;
        if (result != ERROR_SUCCESS) return names;
        for (UINT32 i = 0; i < pathCount; ++i) {
            const auto& path = paths[i];
            DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
            source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
            source.header.size = sizeof(source);
            source.header.adapterId = path.sourceInfo.adapterId;
            source.header.id = path.sourceInfo.id;
            DISPLAYCONFIG_TARGET_DEVICE_NAME target{};
            target.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
            target.header.size = sizeof(target);
            target.header.adapterId = path.targetInfo.adapterId;
            target.header.id = path.targetInfo.id;
            if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS ||
                DisplayConfigGetDeviceInfo(&target.header) != ERROR_SUCCESS) continue;
            auto existing = std::find_if(names.begin(), names.end(), [&](const auto& name) {
                return _wcsicmp(name.device.c_str(), source.viewGdiDeviceName) == 0;
            });
            if (existing == names.end())
                names.push_back({source.viewGdiDeviceName, target.monitorFriendlyDeviceName,
                                 target.monitorDevicePath});
            else if (existing->label != target.monitorFriendlyDeviceName)
                existing->label += L" / " + std::wstring(target.monitorFriendlyDeviceName);
        }
        break;
    }
    return names;
}

std::vector<MonitorOption> DetectMonitors() {
    std::vector<MonitorOption> monitors;
    const BOOL enumerated = EnumDisplayMonitors(nullptr, nullptr,
        [](HMONITOR handle, HDC, LPRECT, LPARAM context) -> BOOL {
            MONITORINFOEXW info{};
            info.cbSize = sizeof(info);
            if (!GetMonitorInfoW(handle, &info)) return TRUE;
            DISPLAY_DEVICEW display{};
            display.cb = sizeof(display);
            std::wstring label = info.szDevice;
            if (EnumDisplayDevicesW(info.szDevice, 0, &display, 0) && display.DeviceString[0])
                label += L" — " + std::wstring(display.DeviceString);
            // Dynamic option labels must be single-line on portable installs too.
            std::replace(label.begin(), label.end(), L'\r', L' ');
            std::replace(label.begin(), label.end(), L'\n', L' ');
            reinterpret_cast<std::vector<MonitorOption>*>(context)->push_back({info.szDevice, label});
            return TRUE;
        }, reinterpret_cast<LPARAM>(&monitors));
    if (!enumerated) monitors.clear();
    const auto friendlyNames = MonitorFriendlyNames();
    for (auto& monitor : monitors) {
        const auto name = std::find_if(friendlyNames.begin(), friendlyNames.end(), [&](const auto& candidate) {
            return _wcsicmp(candidate.device.c_str(), monitor.device.c_str()) == 0;
        });
        if (name != friendlyNames.end()) {
            if (!name->label.empty()) monitor.label = name->label + L" (" + monitor.device + L")";
            monitor.id = name->id;
        }
        if (monitor.id.empty()) monitor.id = monitor.device;
        std::replace(monitor.label.begin(), monitor.label.end(), L'\r', L' ');
        std::replace(monitor.label.begin(), monitor.label.end(), L'\n', L' ');
    }
    return monitors;
}


FontIcon MenuIcon(PCWSTR glyph) {
    FontIcon icon;
    icon.FontFamily(FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(glyph);
    return icon;
}

void ResetAppearance() {
    Wh_SetStringValue(L"SelectedAppearance", L"bar");
    Wh_SetStringValue(L"SelectedTheme", L"green-red");
    Wh_SetStringValue(L"SelectedFormat", L"free");
    Wh_SetStringValue(L"SelectedPrecision", L"1");
    Wh_SetStringValue(L"SelectedMiniDesign", L"0");
    Wh_SetStringValue(L"SelectedAutoCompact", L"1");
    {
        std::lock_guard lock(g_settingsMutex);
        g_settings.appearance = L"bar";
        g_settings.colorScheme = L"green-red";
        g_settings.format = L"free";
        g_settings.precision = 1;
        g_settings.hideDriveName = false;
        g_settings.letterTiles = false;
        g_settings.summaryButton = false;
        Wh_DeleteValue(L"SelectedSummaryButton");
        Wh_DeleteValue(L"SelectedLetterTiles");
        g_settings.autoCompact = true;
    }
    if (g_changed) SetEvent(g_changed);
}

void ApplyConfiguredSettings() {
    constexpr PCWSTR values[] = {
        L"SelectedDrive", L"SelectedTheme", L"SelectedMiniDesign", L"SelectedAppearance",
        L"SelectedFormat", L"SelectedPrecision", L"SelectedMonitor", L"SelectedPosition",
        L"SelectedLowSpace", L"SelectedAutoCompact", L"SelectedAllDrives", L"SelectedDriveGroup", L"SelectedManualDrives", L"SelectedLetterTiles", L"SelectedSummaryButton"
    };
    for (const auto value : values) Wh_DeleteValue(value);
    LoadSettings();
    if (g_changed) SetEvent(g_changed);
}

void ShowThemeMenu() {
    if (!g_ui.surface) return;
    const bool russian = IsRussianUi();
    const Settings settings = CurrentSettings();
    struct ThemeOption { PCWSTR id; PCWSTR english; PCWSTR russian; };
    constexpr ThemeOption themes[] = {
        {L"green-red", L"Green / Red", L"Зелёный / Красный"},
        {L"blue-orange", L"Blue / Orange", L"Синий / Оранжевый"},
        {L"cyan-purple", L"Cyan / Purple", L"Бирюзовый / Фиолетовый"},
        {L"violet-yellow", L"Violet / Yellow", L"Фиолетовый / Жёлтый"},
        {L"teal-pink", L"Teal / Pink", L"Бирюзовый / Розовый"},
        {L"lime-indigo", L"Lime / Indigo", L"Лаймовый / Индиго"},
        {L"amber-navy", L"Amber / Navy", L"Янтарный / Тёмно-синий"},
        {L"mint-coral", L"Mint / Coral", L"Мятный / Коралловый"},
        {L"sky-magenta", L"Sky / Magenta", L"Небесный / Пурпурный"},
        {L"white-gray", L"White / Gray", L"Белый / Серый"},
    };
    MenuFlyout menu;
    MenuFlyoutSubItem themeMenu;
    ToggleMenuFlyoutItem allDrives;
    allDrives.Text(UiText(russian, L"Automatic mode", L"Автоматический режим"));
    allDrives.Icon(MenuIcon(L"\uE713"));
    MenuHint(allDrives,russian,L"Toggle all detected drives; turning off restores the selected single drive",
        L"Показывать все обнаруженные диски; выключение возвращает выбранный одиночный диск");
    allDrives.IsChecked(settings.showAllDrives && settings.driveGroup == L"all");
    allDrives.Click([enabled = !(settings.showAllDrives && settings.driveGroup == L"all")](auto const&, auto const&) {
        SetAllDrives(enabled);
    });
    menu.Items().Append(allDrives);
    AppendManualDriveMenu(menu,russian,settings,CachedDrives());
    menu.Items().Append(MenuFlyoutSeparator());
    themeMenu.Text(UiText(russian, L"Theme", L"Тема"));
    MenuHint(themeMenu,russian,L"Choose the indicator background and capacity visualization",
        L"Выбрать фон индикатора и оформление свободного/занятого места");
    constexpr ThemeOption appearances[] = {
        {L"system", L"System", L"Системная"},
        {L"bar", L"Color bar", L"Цветная полоса"},
        {L"background", L"Capacity background", L"Фон свободного места"},
        {L"text", L"Text only", L"Только текст"},
    };
    for (const auto& appearance : appearances) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE790"));
        std::wstring label = settings.appearance == appearance.id ? L"✓ " : L"  ";
        label += russian ? appearance.russian : appearance.english;
        item.Text(label);
        const std::wstring appearanceId = appearance.id;
        MenuHint(item,russian,
            appearanceId == L"system" ? L"Neutral Windows-style hover background" :
            appearanceId == L"bar" ? L"A free/used strip below each drive; neutral hover for drive lists" :
            appearanceId == L"background" ? L"Persistent free/used background for each drive, without a bottom strip" :
            L"Text without an indicator hover background",
            appearanceId == L"system" ? L"Нейтральный фон при наведении в стиле Windows" :
            appearanceId == L"bar" ? L"Полоса свободно/занято под каждым диском; нейтральная подсветка в списке" :
            appearanceId == L"background" ? L"Постоянный фон свободно/занято у каждого диска, без нижней полосы" :
            L"Текст без фона индикатора при наведении");
        item.Click([id = std::wstring(appearance.id)](auto const&, auto const&) {
            Wh_SetStringValue(L"SelectedAppearance", id.c_str());
            {
                std::lock_guard lock(g_settingsMutex);
                g_settings.appearance = id;
            }
            if (g_changed) SetEvent(g_changed);
        });
        themeMenu.Items().Append(item);
    }
    themeMenu.Icon(MenuIcon(L"\uE790"));
    menu.Items().Append(themeMenu);
    MenuFlyoutSubItem colorMenu;
    colorMenu.Icon(MenuIcon(L"\uE790"));
    colorMenu.Text(UiText(russian, L"Colors", L"Цвета"));
    const bool colorsAvailable = settings.appearance == L"bar" || settings.appearance == L"background";
    colorMenu.Opacity(colorsAvailable ? 1.0 : 0.55);
    MenuHint(colorMenu,russian,
        colorsAvailable ? L"Choose colors for free and used space" :
            L"Colors are available in Color bar and Capacity background themes",
        colorsAvailable ? L"Выбрать цвета свободного и занятого места" :
            L"Цвета доступны в темах «Цветная полоса» и «Фон свободного места»");
    for (const auto& theme : themes) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE790"));
        std::wstring label = settings.colorScheme == theme.id ? L"✓ " : L"  ";
        label += russian ? theme.russian : theme.english;
        item.Text(label);
        MenuHint(item,colorsAvailable ? UiText(russian,L"Free / used colors: ",L"Цвета свободно / занято: ") +
            std::wstring(russian ? theme.russian : theme.english) :
            UiText(russian,L"First select Color bar or Capacity background",
                L"Сначала выберите «Цветная полоса» или «Фон свободного места»"));
        item.Click([id = std::wstring(theme.id)](auto const&, auto const&) {
            const auto current = CurrentSettings();
            if (current.appearance != L"bar" && current.appearance != L"background") return;
            Wh_SetStringValue(L"SelectedTheme", id.c_str());
            {
                std::lock_guard lock(g_settingsMutex);
                g_settings.colorScheme = id;
            }
            if (g_changed) SetEvent(g_changed);
        });
        colorMenu.Items().Append(item);
    }
    menu.Items().Append(colorMenu);
    MenuFlyoutSubItem formatMenu;
    formatMenu.Icon(MenuIcon(L"\uE8A5"));
    formatMenu.Text(UiText(russian, L"Format", L"Формат"));
    MenuHint(formatMenu,russian,L"Choose which capacity values appear on the taskbar",
        L"Выбрать данные о месте, которые отображаются на панели");
    constexpr ThemeOption formats[] = {
        {L"free", L"Free / Total", L"Свободно / Всего"},
        {L"used", L"Used / Total", L"Занято / Всего"},
        {L"percent", L"Free, %", L"Свободно, %"},
    };
    for (const auto& format : formats) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE8A5"));
        std::wstring label = settings.format == format.id ? L"✓ " : L"  ";
        label += russian ? format.russian : format.english;
        item.Text(label);
        const std::wstring formatId = format.id;
        MenuHint(item,russian,
            formatId == L"free" ? L"Available and total capacity in GiB" :
            formatId == L"used" ? L"Used and total capacity in GiB" : L"Available space as a percentage of total capacity",
            formatId == L"free" ? L"Свободный и общий объём в ГиБ" :
            formatId == L"used" ? L"Занятый и общий объём в ГиБ" : L"Свободное место в процентах от общего объёма");
        item.Click([id = std::wstring(format.id)](auto const&, auto const&) {
            Wh_SetStringValue(L"SelectedFormat", id.c_str());
            {
                std::lock_guard lock(g_settingsMutex);
                g_settings.format = id;
            }
            if (g_changed) SetEvent(g_changed);
        });
        formatMenu.Items().Append(item);
    }
    menu.Items().Append(formatMenu);
    MenuFlyoutSubItem precisionMenu;
    precisionMenu.Icon(MenuIcon(L"\uE8EF"));
    precisionMenu.Text(UiText(russian, L"Precision", L"Точность"));
    MenuHint(precisionMenu,russian,L"Choose decimal places for capacity and percentages",
        L"Выбрать число знаков после запятой для объёма и процентов");
    constexpr PCWSTR russianLabels[] = {L"Без дробной части", L"1 знак после запятой", L"2 знака после запятой"};
    constexpr PCWSTR englishLabels[] = {L"No decimal places", L"1 decimal place", L"2 decimal places"};
    for (int precision = 0; precision <= 2; ++precision) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE8EF"));
        std::wstring label = settings.precision == precision ? L"✓ " : L"  ";
        label += russian ? russianLabels[precision] : englishLabels[precision];
        item.Text(label);
        MenuHint(item,UiText(russian,L"Round capacity and percentages to decimal places: ",
            L"Округлять объём и проценты; знаков после запятой: ")+std::to_wstring(precision));
        item.Click([precision](auto const&, auto const&) {
            Wh_SetStringValue(L"SelectedPrecision", std::to_wstring(precision).c_str());
            {
                std::lock_guard lock(g_settingsMutex);
                g_settings.precision = precision;
            }
            if (g_changed) SetEvent(g_changed);
        });
        precisionMenu.Items().Append(item);
    }
    menu.Items().Append(precisionMenu);
    MenuFlyoutSubItem monitorMenu;
    monitorMenu.Text(UiText(russian, L"Monitor", L"Монитор"));
    MenuHint(monitorMenu,russian,L"Choose the taskbars where the indicator appears",
        L"Выбрать панели задач, на которых отображается индикатор");
    auto appendMonitor = [&](const std::wstring& id, const std::wstring& label) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE7F4"));
        item.Text((settings.displayOn == id ? L"✓ " : L"  ") + label);
        MenuHint(item,russian,
            id == L"primary" ? L"Show only on the primary monitor taskbar" :
            id == L"all" ? L"Show on every available taskbar" : L"Show only on this monitor taskbar",
            id == L"primary" ? L"Показывать только на панели основного монитора" :
            id == L"all" ? L"Показывать на всех доступных панелях задач" : L"Показывать только на панели этого монитора");
        item.Click([id](auto const&, auto const&) {
            Wh_SetStringValue(L"SelectedMonitor", id.c_str());
            {
                std::lock_guard lock(g_settingsMutex);
                g_settings.displayOn = id;
            }
            if (g_changed) SetEvent(g_changed);
        });
        monitorMenu.Items().Append(item);
    };
    appendMonitor(L"primary", UiText(russian, L"Primary taskbar", L"Основная панель"));
    appendMonitor(L"all", UiText(russian, L"All taskbars", L"Все панели"));
    monitorMenu.Items().Append(MenuFlyoutSeparator());
    for (const auto& monitor : CachedMonitors()) appendMonitor(monitor.id, monitor.label);
    monitorMenu.Icon(MenuIcon(L"\uE7F4"));
    menu.Items().Append(monitorMenu);
    MenuFlyoutSubItem positionMenu;
    positionMenu.Text(UiText(russian, L"Position", L"Положение"));
    MenuHint(positionMenu,russian,L"Choose the indicator position on the taskbar",
        L"Выбрать положение индикатора на панели задач");
    constexpr ThemeOption positions[] = {
        {L"auto", L"Automatic", L"Автоматически"},
        {L"left", L"Left", L"Слева"},
        {L"right", L"Right", L"Справа"},
    };
    const bool verticalPosition = g_ui.root && g_ui.root.ActualHeight() > g_ui.root.ActualWidth();
    if (verticalPosition) {
        ToggleMenuFlyoutItem automatic;
        automatic.Text(UiText(russian, L"Automatic (top)", L"Автоматически (сверху)"));
        automatic.Icon(MenuIcon(L"\uE8A9"));
        automatic.IsChecked(true);
        automatic.Opacity(0.55);
        MenuHint(automatic,russian,L"Vertical taskbars use only the upper free section",
            L"На вертикальной панели используется только свободный участок сверху");
        automatic.Click([](auto const& sender, auto const&) {
            sender.template as<ToggleMenuFlyoutItem>().IsChecked(true);
        });
        positionMenu.Items().Append(automatic);
    }
    for (const auto& position : positions) {
        if (verticalPosition) break;
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE8A9"));
        std::wstring label = settings.position == position.id ? L"✓ " : L"  ";
        label += russian ? position.russian : position.english;
        item.Text(label);
        const std::wstring positionId = position.id;
        MenuHint(item,russian,
            positionId == L"auto" ? L"Choose a free section automatically; lists prefer the side fitting more cards" :
            positionId == L"left" ? L"Use the left taskbar section" : L"Use the right free section before the tray",
            positionId == L"auto" ? L"Автоматически выбрать свободный участок; для списка — более вместительную сторону" :
            positionId == L"left" ? L"Использовать левый участок панели задач" : L"Использовать свободный участок справа перед треем");
        item.Click([id = std::wstring(position.id)](auto const&, auto const&) {
            Wh_SetStringValue(L"SelectedPosition", id.c_str());
            {
                std::lock_guard lock(g_settingsMutex);
                g_settings.position = id;
            }
            if (g_changed) SetEvent(g_changed);
        });
        positionMenu.Items().Append(item);
    }
    positionMenu.Icon(MenuIcon(L"\uE8A9"));
    menu.Items().Append(positionMenu);
    MenuFlyoutSubItem miniMenu;
    miniMenu.Icon(MenuIcon(L"\uE8A7"));
    miniMenu.Text(UiText(russian, L"Compact mode", L"Компактный режим"));
    MenuHint(miniMenu,russian,L"Choose compact drive cards or a drive-count button",
        L"Выбрать компактные карточки дисков или кнопку со счётчиком");
    ToggleMenuFlyoutItem miniItem;
    miniItem.Icon(MenuIcon(L"\uE8A7"));
    miniItem.Text(UiText(russian, L"Hide drive name", L"Скрывать имя диска"));
    MenuHint(miniItem,russian,L"Hide volume names; keep drive letters and capacity on one line",
        L"Скрыть имена дисков; оставить буквы и данные о месте в одну строку");
    miniItem.IsChecked(settings.hideDriveName);
    miniItem.Click([](auto const& sender, auto const&) {
        const bool enabled = sender.template as<ToggleMenuFlyoutItem>().IsChecked();
        Wh_SetStringValue(L"SelectedMiniDesign", enabled ? L"1" : L"0");
        {
            std::lock_guard lock(g_settingsMutex);
            g_settings.hideDriveName = enabled;
        }
        if (g_changed) SetEvent(g_changed);
    });
    miniMenu.Items().Append(miniItem);
    ToggleMenuFlyoutItem autoMiniItem;
    autoMiniItem.Icon(MenuIcon(L"\uE713"));
    autoMiniItem.Text(UiText(russian, L"Automatic", L"Автоматически"));
    MenuHint(autoMiniItem,russian,L"Always enabled: use a smaller design when the selected design cannot fit a drive card",
        L"Всегда включено: использовать более компактный вид, если карточка выбранного вида не помещается");
    autoMiniItem.IsChecked(true);
    autoMiniItem.IsEnabled(false);
    miniMenu.Items().Append(autoMiniItem);
    ToggleMenuFlyoutItem tileItem;
    tileItem.Text(UiText(russian, L"Letters and capacity bar", L"Буквы дисков и полоса"));
    MenuHint(tileItem,russian,L"Show drive letters on a free/used capacity background",
        L"Показывать буквы дисков на фоне свободного и занятого места");
    tileItem.Icon(MenuIcon(L"\uE8A7"));
    tileItem.IsChecked(settings.letterTiles);
    tileItem.Click([](auto const& sender, auto const&) {
        const bool enabled = sender.template as<ToggleMenuFlyoutItem>().IsChecked();
        Wh_SetStringValue(L"SelectedLetterTiles", enabled ? L"1" : L"0");
        { std::lock_guard lock(g_settingsMutex); g_settings.letterTiles = enabled; }
        if (g_changed) SetEvent(g_changed);
    });
    miniMenu.Items().Append(tileItem);
    ToggleMenuFlyoutItem summaryItem;
    summaryItem.Text(UiText(russian, L"Drives · N", L"Диски · N"));
    MenuHint(summaryItem,russian,L"Replace cards with a drive count; hover for capacity details",
        L"Заменить карточки счётчиком дисков; данные о месте — при наведении");
    summaryItem.Icon(MenuIcon(L"\uE8A7"));
    summaryItem.IsChecked(settings.summaryButton);
    summaryItem.Click([](auto const& sender, auto const&) {
        const bool enabled = sender.template as<ToggleMenuFlyoutItem>().IsChecked();
        Wh_SetStringValue(L"SelectedSummaryButton", enabled ? L"1" : L"0");
        { std::lock_guard lock(g_settingsMutex); g_settings.summaryButton = enabled; }
        if (g_changed) SetEvent(g_changed);
    });
    miniMenu.Items().Append(summaryItem);
    menu.Items().Append(miniMenu);
    MenuFlyoutSubItem lowSpaceMenu;
    lowSpaceMenu.Icon(MenuIcon(L"\uE7BA"));
    lowSpaceMenu.Text(UiText(russian, L"Low disk space", L"Нехватка места"));
    MenuHint(lowSpaceMenu,russian,L"Choose when low free space highlights a drive text",
        L"Выбрать порог свободного места для предупреждения цветом текста");
    auto appendThreshold = [&](const std::wstring& mode, int threshold, const std::wstring& label) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE7BA"));
        const bool selected = settings.lowSpaceMode == mode &&
            (mode == L"off" || settings.lowSpaceThreshold == threshold);
        item.Text((selected ? L"✓ " : L"  ") + label);
        MenuHint(item,mode == L"off" ? UiText(russian,L"Disable low-space text warnings",
            L"Не выделять текст при нехватке свободного места") :
            UiText(russian,L"Highlight a drive when free space is at or below ",
                L"Выделять текст диска, когда свободно не больше ") + std::to_wstring(threshold) +
                (mode == L"percent" ? L" %" : russian ? L" ГиБ" : L" GiB"));
        item.Click([mode, threshold](auto const&, auto const&) {
            Wh_SetStringValue(L"SelectedLowSpace", (mode + L":" + std::to_wstring(threshold)).c_str());
            {
                std::lock_guard lock(g_settingsMutex);
                g_settings.lowSpaceMode = mode;
                g_settings.lowSpaceThreshold = threshold;
            }
            if (g_changed) SetEvent(g_changed);
        });
        lowSpaceMenu.Items().Append(item);
    };
    appendThreshold(L"off", 0, UiText(russian, L"Disabled", L"Выключена"));
    for (const int threshold : {5, 10, 15, 20})
        appendThreshold(L"percent", threshold, std::to_wstring(threshold) + L" %");
    lowSpaceMenu.Items().Append(MenuFlyoutSeparator());
    for (const int threshold : {5, 10, 20, 50})
        appendThreshold(L"gib", threshold, std::to_wstring(threshold) + (russian ? L" ГиБ" : L" GiB"));
    if (settings.lowSpaceMode != L"off") {
        MenuFlyoutItem current;
        current.Icon(MenuIcon(L"\uE7BA"));
        current.Opacity(0.55);
        MenuHint(current,russian,L"The saved warning threshold; change it using the options above",
            L"Сохранённый порог предупреждения; изменить его можно пунктами выше");
        current.Text(UiText(russian, L"Current: ", L"Текущий порог: ") +
            std::to_wstring(settings.lowSpaceThreshold) +
            (settings.lowSpaceMode == L"percent" ? L" %" : (russian ? L" ГиБ" : L" GiB")));
        lowSpaceMenu.Items().Append(current);
    }
    menu.Items().Append(lowSpaceMenu);
    menu.Items().Append(MenuFlyoutSeparator());
    MenuFlyoutItem openDriveItem;
    openDriveItem.Text(UiText(russian, L"Open selected drive", L"Открыть выбранный диск"));
    MenuHint(openDriveItem,russian,L"Open the selected single drive in File Explorer",
        L"Открыть выбранный одиночный диск в Проводнике");
    openDriveItem.Click([](auto const&, auto const&) {
        const auto drive = NormalizeDrive(CurrentSettings().drive);
        if (drive.empty()) return;
        const std::wstring root = drive + L"\\";
        const auto result = reinterpret_cast<INT_PTR>(ShellExecuteW(
            nullptr, L"open", root.c_str(), nullptr, nullptr, SW_SHOWNORMAL));
        if (result <= 32)
            Wh_Log(L"Cannot open selected drive %s: ShellExecute error %d",
                   drive.c_str(), static_cast<int>(result));
    });
    openDriveItem.Icon(MenuIcon(L"\uE8B7"));
    menu.Items().Append(openDriveItem);
    MenuFlyoutItem computerItem;
    computerItem.Text(UiText(russian, L"This PC", L"Этот компьютер"));
    MenuHint(computerItem,russian,L"Open This PC in File Explorer",
        L"Открыть раздел «Этот компьютер» в Проводнике");
    computerItem.Click([](auto const&, auto const&) {
        wchar_t windowsDirectory[MAX_PATH]{};
        const UINT length = GetWindowsDirectoryW(windowsDirectory, ARRAYSIZE(windowsDirectory));
        if (!length || length >= ARRAYSIZE(windowsDirectory)) {
            Wh_Log(L"Cannot locate Windows directory to open This PC");
            return;
        }
        const std::wstring explorer = std::wstring(windowsDirectory) + L"\\explorer.exe";
        const auto result = reinterpret_cast<INT_PTR>(ShellExecuteW(
            nullptr, L"open", explorer.c_str(),
            L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}", nullptr, SW_SHOWNORMAL));
        if (result <= 32) Wh_Log(L"Cannot open This PC: ShellExecute error %d", static_cast<int>(result));
    });
    computerItem.Icon(MenuIcon(L"\uE7F4"));
    menu.Items().Append(computerItem);
    MenuFlyoutItem diskManagementItem;
    diskManagementItem.Text(UiText(russian, L"Disk Management", L"Управление дисками"));
    MenuHint(diskManagementItem,russian,L"Open Windows Disk Management with administrator elevation",
        L"Открыть управление дисками Windows с запросом прав администратора");
    diskManagementItem.Click([](auto const&, auto const&) {
        wchar_t systemDirectory[MAX_PATH]{};
        const UINT length = GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
        if (!length || length >= ARRAYSIZE(systemDirectory)) {
            Wh_Log(L"Cannot locate system directory to open Disk Management");
            return;
        }
        const std::wstring mmc = std::wstring(systemDirectory) + L"\\mmc.exe";
        const std::wstring console = L"\"" + std::wstring(systemDirectory) + L"\\diskmgmt.msc\"";
        const auto result = reinterpret_cast<INT_PTR>(ShellExecuteW(
            nullptr, L"runas", mmc.c_str(), console.c_str(), nullptr, SW_SHOWNORMAL));
        if (result <= 32)
            Wh_Log(L"Cannot open Disk Management: ShellExecute error %d", static_cast<int>(result));
    });
    diskManagementItem.Icon(MenuIcon(L"\uE713"));
    menu.Items().Append(diskManagementItem);
    menu.Items().Append(MenuFlyoutSeparator());
    MenuFlyoutItem refreshItem;
    refreshItem.Text(UiText(russian, L"Refresh now", L"Обновить сейчас"));
    MenuHint(refreshItem,russian,L"Read current disk space without waiting for the update interval",
        L"Обновить данные о месте на дисках, не дожидаясь заданного интервала");
    refreshItem.Click([](auto const&, auto const&) {
        // Wake the worker, which resets nextRead and reads volume metadata.
        // Keep disk I/O off the taskbar UI thread.
        if (g_changed) SetEvent(g_changed);
    });
    refreshItem.Icon(MenuIcon(L"\uE72C"));
    menu.Items().Append(refreshItem);
    MenuFlyoutItem resetItem;
    resetItem.Text(UiText(russian, L"Reset appearance", L"Сбросить оформление"));
    MenuHint(resetItem,russian,L"Restore default appearance; keep drive, monitor, position and warning threshold",
        L"Вернуть стандартный вид; сохранить диск, монитор, положение и порог предупреждения");
    resetItem.Icon(MenuIcon(L"\uE777"));
    resetItem.Click([](auto const&, auto const&) { ResetAppearance(); });
    menu.Items().Append(resetItem);
    MenuFlyoutItem configuredItem;
    configuredItem.Text(UiText(russian, L"Apply Windhawk settings", L"Применить настройки Windhawk"));
    MenuHint(configuredItem,russian,L"Clear saved menu overrides and apply the Windhawk settings page",
        L"Сбросить сохранённый выбор меню и применить страницу настроек Windhawk");
    configuredItem.Icon(MenuIcon(L"\uE713"));
    configuredItem.Click([](auto const&, auto const&) { ApplyConfiguredSettings(); });
    menu.Items().Append(configuredItem);
    if (g_ui.driveMenu) g_ui.driveMenu.Hide();
    g_ui.driveMenu = menu;
    menu.ShowAt(g_ui.surface);
}

struct ColorPair {
    Color free;
    Color used;
};

ColorPair ColorScheme(const std::wstring& name) {
    // Пары свободно/всего / Free/used color pairs.
    if (name == L"blue-orange") {
        return {Color{32, 70, 135, 220}, Color{32, 225, 135, 35}};
    }
    if (name == L"cyan-purple") {
        return {Color{32, 25, 180, 200}, Color{32, 145, 80, 190}};
    }
    if (name == L"violet-yellow") {
        return {Color{32, 145, 80, 190}, Color{32, 220, 195, 45}};
    }
    if (name == L"teal-pink") {
        return {Color{32, 40, 165, 145}, Color{32, 215, 90, 165}};
    }
    if (name == L"lime-indigo") {
        return {Color{32, 165, 205, 55}, Color{32, 95, 55, 175}};
    }
    if (name == L"amber-navy") {
        return {Color{32, 225, 165, 35}, Color{32, 90, 45, 25}};
    }
    if (name == L"mint-coral") {
        return {Color{32, 150, 210, 170}, Color{32, 230, 110, 85}};
    }
    if (name == L"sky-magenta") {
        return {Color{32, 95, 175, 235}, Color{32, 205, 45, 165}};
    }
    if (name == L"white-gray") {
        return {Color{26, 245, 245, 245}, Color{32, 100, 100, 100}};
    }
    // Default: green free space and red used space.
    return {Color{32, 45, 190, 70}, Color{32, 205, 55, 45}};
}

StackPanel SummaryTooltipContent(const Reading& reading, const Settings& settings,
                                 bool russian, bool light, bool highContrast,
                                 const TooltipLayout& layout, bool overflow) {
    const auto disks = TooltipDisks(reading, settings.showAllDrives);
    auto systemColor = [](int index) {
        const auto rgb = GetSysColor(index);
        return Color{255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb)};
    };
    const Color primary = highContrast ? systemColor(COLOR_WINDOWTEXT) :
        light ? Color{255,28,28,28} : Color{255,245,245,245};
    const Color secondary = highContrast ? primary :
        light ? Color{255,96,96,96} : Color{255,170,170,170};
    const Color quiet = highContrast ? systemColor(COLOR_WINDOW) :
        light ? Color{6,0,0,0} : Color{9,255,255,255};
    const Color warning = highContrast ? primary :
        light ? Color{255,175,30,30} : Color{255,255,135,125};
    auto colors = ColorScheme(settings.colorScheme);
    colors.free.A = 230;
    colors.used.A = 140;
    if (highContrast) {
        colors.free = systemColor(COLOR_HIGHLIGHT);
        colors.used = systemColor(COLOR_WINDOW);
    }
    auto text = [&](const std::wstring& value, double size, Color color, bool bold = false) {
        TextBlock label;
        label.Text(value);
        label.FontFamily(FontFamily(L"Segoe UI Variable Text"));
        label.FontSize(size);
        label.Foreground(SolidColorBrush(color));
        if (bold) label.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        return label;
    };
    StackPanel body;
    body.Width(layout.width);
    // A taskbar ToolTip is informational. ScrollViewer introduces focus/input
    // machinery into a popup that the shell unloads between pointer hovers.
    body.IsHitTestVisible(false);
    Grid header;
    for (const auto unit : {GridUnitType::Star, GridUnitType::Auto}) {
        ColumnDefinition column;
        column.Width(GridLength{1.0,unit});
        header.ColumnDefinitions().Append(column);
    }
    StackPanel heading;
    heading.Children().Append(text(overflow ?
        UiText(russian,L"Hidden drives",L"Диски вне панели") :
        UiText(russian,L"Disk space",L"Место на дисках"),16,primary,true));
    const auto group = !settings.showAllDrives ? UiText(russian,L"Selected drive",L"Выбранный диск") :
        settings.driveGroup == L"local" ? UiText(russian,L"Local drives",L"Локальные диски") :
        settings.driveGroup == L"external" ? UiText(russian,L"External drives",L"Внешние диски") :
        settings.driveGroup == L"manual" ? UiText(russian,L"Selected drives",L"Выбранные диски") :
        UiText(russian,L"All drives",L"Все диски");
    auto subtitle = text(group,11,secondary);
    subtitle.Margin(Thickness{0,3,0,0});
    heading.Children().Append(subtitle);
    header.Children().Append(heading);
    Border count;
    count.MinWidth(30);
    count.Height(28);
    count.Padding(Thickness{8,0,8,0});
    count.CornerRadius(CornerRadius{7});
    count.Background(SolidColorBrush(quiet));
    count.VerticalAlignment(VerticalAlignment::Center);
    auto number = text(std::to_wstring(disks.size()),13,primary,true);
    number.VerticalAlignment(VerticalAlignment::Center);
    number.TextAlignment(TextAlignment::Center);
    count.Child(number);
    Grid::SetColumn(count,1);
    header.Children().Append(count);
    body.Children().Append(header);
    Border divider;
    divider.Height(1);
    divider.Margin(Thickness{0,10,0,10});
    divider.Background(SolidColorBrush(highContrast ? primary :
        light ? Color{24,0,0,0} : Color{28,255,255,255}));
    body.Children().Append(divider);
    Grid rows;
    for (size_t column = 0; column < layout.columns; ++column) {
        ColumnDefinition definition;
        definition.Width(GridLength{1,GridUnitType::Star});
        rows.ColumnDefinitions().Append(definition);
    }
    for (size_t row = 0; row < layout.rows; ++row) {
        RowDefinition definition;
        definition.Height(GridLength{1,GridUnitType::Auto});
        rows.RowDefinitions().Append(definition);
    }
    for (size_t index = 0; index < layout.visible; ++index) {
        const auto& disk = disks[index];
        const auto foreground = disk.lowSpace ? warning : primary;
        Border card;
        card.CornerRadius(CornerRadius{8});
        card.Padding(Thickness{10,9,10,9});
        card.Margin(Thickness{index / layout.rows ? 8.0 : 0.0,0,0,
            index % layout.rows + 1 < layout.rows && index + 1 < layout.visible ? 7.0 : 0.0});
        Grid::SetColumn(card,static_cast<int>(index/layout.rows));
        Grid::SetRow(card,static_cast<int>(index%layout.rows));
        card.Background(SolidColorBrush(quiet));
        if (highContrast) {
            card.BorderThickness(Thickness{1});
            card.BorderBrush(SolidColorBrush(primary));
        }
        StackPanel contents;
        Grid details;
        for (const auto length : {GridLength{44,GridUnitType::Pixel},
                                 GridLength{1,GridUnitType::Star},
                                 GridLength{64,GridUnitType::Pixel}}) {
            ColumnDefinition column;
            column.Width(length);
            details.ColumnDefinitions().Append(column);
        }
        Border badge;
        badge.Width(34);
        badge.Height(34);
        badge.HorizontalAlignment(HorizontalAlignment::Left);
        badge.VerticalAlignment(VerticalAlignment::Top);
        badge.CornerRadius(CornerRadius{7});
        auto badgeColor = colors.free;
        badgeColor.A = highContrast ? 0 : 26;
        badge.Background(SolidColorBrush(badgeColor));
        if (highContrast) {
            badge.BorderBrush(SolidColorBrush(primary));
            badge.BorderThickness(Thickness{1});
        }
        auto letter = text(disk.letter,12,primary,true);
        letter.VerticalAlignment(VerticalAlignment::Center);
        letter.TextAlignment(TextAlignment::Center);
        badge.Child(letter);
        details.Children().Append(badge);
        StackPanel labels;
        labels.Margin(Thickness{0,0,8,0});
        auto name = text(disk.name.empty() ? UiText(russian,L"Drive",L"Диск") : disk.name,12,foreground,true);
        name.TextWrapping(TextWrapping::Wrap);
        name.TextTrimming(TextTrimming::CharacterEllipsis);
        name.MaxLines(2);
        labels.Children().Append(name);
        auto capacity = text(disk.capacity,11,disk.lowSpace ? warning : secondary);
        capacity.Margin(Thickness{0,3,0,0});
        capacity.TextWrapping(TextWrapping::Wrap);
        capacity.TextTrimming(TextTrimming::CharacterEllipsis);
        capacity.MaxLines(2);
        labels.Children().Append(capacity);
        Grid::SetColumn(labels,1);
        details.Children().Append(labels);
        StackPanel percentage;
        auto percent = text(disk.hasRatio ? FormatGiB(disk.freeRatio * 100.0,russian,settings.precision) + L" %" : L"—",
                            13,foreground,true);
        percent.TextAlignment(TextAlignment::Right);
        percentage.Children().Append(percent);
        auto caption = text(UiText(russian,L"free",L"свободно"),10,secondary);
        caption.TextAlignment(TextAlignment::Right);
        caption.Margin(Thickness{0,3,0,0});
        percentage.Children().Append(caption);
        Grid::SetColumn(percentage,2);
        details.Children().Append(percentage);
        contents.Children().Append(details);
        if (disk.hasRatio) {
            LinearGradientBrush fill;
            fill.StartPoint({0,0.5}); fill.EndPoint({1,0.5});
            for (int i = 0; i < 4; ++i) {
                GradientStop stop;
                stop.Color(i < 2 ? colors.free : colors.used);
                stop.Offset(i == 0 ? 0 : i == 3 ? 1 : disk.freeRatio);
                fill.GradientStops().Append(stop);
            }
            Border bar;
            bar.Height(highContrast ? 6 : 4);
            bar.Margin(Thickness{0,8,0,0});
            bar.CornerRadius(CornerRadius{2});
            bar.Background(fill);
            if (highContrast) {
                bar.BorderThickness(Thickness{1});
                bar.BorderBrush(SolidColorBrush(primary));
            }
            contents.Children().Append(bar);
        }
        card.Child(contents);
        Automation::AutomationProperties::SetName(card,
            disk.letter + L". " + disk.name + L". " + disk.capacity);
        rows.Children().Append(card);
    }
    if (disks.empty()) {
        auto empty = text(reading.title + L"\n" + reading.capacity,12,secondary);
        empty.TextWrapping(TextWrapping::Wrap);
        rows.Children().Append(empty);
    }
    body.Children().Append(rows);
    if (layout.visible < disks.size()) {
        auto overflow = text(UiText(russian,L"More drives: ",L"Ещё дисков: ") +
            std::to_wstring(disks.size()-layout.visible) +
            UiText(russian,L" · Open the drive menu",L" · Откройте меню выбора"),11,secondary);
        overflow.Margin(Thickness{0,8,0,0});
        overflow.TextWrapping(TextWrapping::Wrap);
        body.Children().Append(overflow);
    }
    StackPanel legend;
    legend.Orientation(Orientation::Horizontal);
    legend.Margin(Thickness{0,10,0,0});
    auto swatch = [&](const std::wstring& caption, Color color) {
        StackPanel item;
        item.Orientation(Orientation::Horizontal);
        item.Margin(Thickness{0,0,16,0});
        Border dot;
        dot.Width(7); dot.Height(7);
        dot.CornerRadius(CornerRadius{4});
        dot.VerticalAlignment(VerticalAlignment::Center);
        dot.Margin(Thickness{0,0,5,0});
        dot.Background(SolidColorBrush(color));
        if (highContrast) {
            dot.BorderBrush(SolidColorBrush(primary));
            dot.BorderThickness(Thickness{1});
        }
        item.Children().Append(dot);
        item.Children().Append(text(caption,10,secondary));
        return item;
    };
    legend.Children().Append(swatch(UiText(russian,L"Free",L"Свободно"),colors.free));
    legend.Children().Append(swatch(UiText(russian,L"Used",L"Занято"),colors.used));
    legend.Visibility(disks.empty() ? Visibility::Collapsed : Visibility::Visible);
    body.Children().Append(legend);
    return body;
}

void ClearCapacityTooltip() {
    if (g_ui.summaryTip) {
        g_ui.summaryTip.IsOpen(false);
        if (g_ui.tooltipOwner) ToolTipService::SetToolTip(g_ui.tooltipOwner,nullptr);
        g_ui.summaryTip = nullptr;
    }
    g_ui.tooltipOwner = nullptr;
    g_ui.tooltipSignature.clear();
}

void UpdateSummaryTooltip(HWND window, const Settings& settings, const Reading& reading,
                          bool vertical) {
    const bool overflow = !g_ui.summary && g_ui.hiddenDrives > 0;
    FrameworkElement owner = g_ui.summary ? g_ui.surface.as<FrameworkElement>() :
        overflow ? g_ui.overflowSurface.as<FrameworkElement>() : nullptr;
    if (g_ui.tooltipOwner != owner || !owner) {
        ClearCapacityTooltip();
        g_ui.tooltipOwner = owner;
    }
    if (!owner) return;
    const auto hidden = overflow ? HiddenDriveReading(reading,g_ui.visibleDrives) : Reading{};
    const auto& tooltipReading = overflow ? hidden : reading;
    MONITORINFO monitor{sizeof(monitor)};
    const bool hasMonitor = GetMonitorInfoW(MonitorFromWindow(window,MONITOR_DEFAULTTONEAREST),&monitor) != FALSE;
    const double scale = std::max(96U,GetDpiForWindow(window)) / 96.0;
    const double workWidth = hasMonitor ? (monitor.rcWork.right-monitor.rcWork.left)/scale : 800;
    const double workHeight = hasMonitor ? (monitor.rcWork.bottom-monitor.rcWork.top)/scale : 600;
    const auto disks = TooltipDisks(tooltipReading,settings.showAllDrives);
    const auto layout = PassiveTooltipLayout(disks.size(),workWidth,workHeight);
    const double width = layout.width+30.0;
    HIGHCONTRASTW contrast{sizeof(contrast)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST,sizeof(contrast),&contrast,0);
    const bool highContrast = (contrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
    const bool light = g_ui.surface.ActualTheme() == ElementTheme::Light;
    const bool russian = IsRussianUi();
    std::wstring signature = DriveTooltip(tooltipReading) + L"|" + std::to_wstring(overflow) +
        L"|" + settings.driveGroup + L"|" +
        settings.colorScheme + L"|" + std::to_wstring(settings.showAllDrives) +
        std::to_wstring(settings.precision) + std::to_wstring(russian) +
        std::to_wstring(light) + std::to_wstring(highContrast) +
        L"|" + std::to_wstring(width) + L"|" + std::to_wstring(layout.rows) +
        L"|" + std::to_wstring(layout.columns) + L"|" + std::to_wstring(layout.visible);
    for (const auto& disk : disks)
        signature += L"|" + disk.letter + L"|" + std::to_wstring(disk.freeRatio) +
            std::to_wstring(disk.hasRatio) + std::to_wstring(disk.lowSpace);
    if (!g_ui.summaryTip) {
        g_ui.summaryTip = ToolTip();
        g_ui.summaryTip.Padding(Thickness{14,12,14,12});
        g_ui.summaryTip.BorderThickness(Thickness{1});
        g_ui.summaryTip.CornerRadius(CornerRadius{12});
        g_ui.summaryTip.IsHitTestVisible(false);
        g_ui.summaryTip.XamlRoot(owner.XamlRoot());
        g_ui.summaryTip.PlacementTarget(owner);
    }
    // Never replace a live popup's content or change its XamlRoot while the
    // framework is processing hover/close input. Refresh on the next closed probe.
    const bool open = g_ui.summaryTip.IsOpen();
    if (!open && signature != g_ui.tooltipSignature) {
        g_ui.summaryTip.RequestedTheme(light ? ElementTheme::Light : ElementTheme::Dark);
        const auto backgroundRgb = highContrast ? GetSysColor(COLOR_WINDOW) :
            light ? RGB(250,250,250) : RGB(32,32,32);
        const auto borderRgb = highContrast ? GetSysColor(COLOR_WINDOWTEXT) :
            light ? RGB(216,216,216) : RGB(64,64,64);
        g_ui.summaryTip.Background(SolidColorBrush(Color{255,GetRValue(backgroundRgb),GetGValue(backgroundRgb),GetBValue(backgroundRgb)}));
        g_ui.summaryTip.BorderBrush(SolidColorBrush(Color{255,GetRValue(borderRgb),GetGValue(borderRgb),GetBValue(borderRgb)}));
        g_ui.summaryTip.Width(width);
        g_ui.summaryTip.MaxWidth(width);
        g_ui.summaryTip.Content(SummaryTooltipContent(tooltipReading,settings,russian,light,highContrast,layout,overflow));
        Automation::AutomationProperties::SetName(g_ui.summaryTip,DriveTooltip(tooltipReading));
        g_ui.tooltipSignature = std::move(signature);
        if (ToolTipService::GetToolTip(owner) != g_ui.summaryTip)
            ToolTipService::SetToolTip(owner,g_ui.summaryTip);
    }
    using winrt::Windows::UI::Xaml::Controls::Primitives::PlacementMode;
    RECT taskbar{};
    GetWindowRect(window,&taskbar);
    const bool firstEdge = hasMonitor && (vertical ?
        std::abs(taskbar.left-monitor.rcMonitor.left) <= std::abs(taskbar.right-monitor.rcMonitor.right) :
        std::abs(taskbar.top-monitor.rcMonitor.top) <= std::abs(taskbar.bottom-monitor.rcMonitor.bottom));
    const auto placement = vertical ? (firstEdge ? PlacementMode::Right : PlacementMode::Left) :
        firstEdge ? PlacementMode::Bottom : PlacementMode::Top;
    if (!open && g_ui.summaryTip.Placement() != placement) g_ui.summaryTip.Placement(placement);
}

Color NeutralHoverColor(bool light) {
    HIGHCONTRASTW contrast{sizeof(contrast)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0);
    if (contrast.dwFlags & HCF_HIGHCONTRASTON) {
        const COLORREF highlight = GetSysColor(COLOR_HIGHLIGHT);
        return Color{110, GetRValue(highlight), GetGValue(highlight), GetBValue(highlight)};
    }
    if (auto border = g_ui.startBackground.try_as<Border>()) {
        if (auto brush = border.Background().try_as<SolidColorBrush>()) {
            auto color = brush.Color();
            color.A = EffectiveBrushAlpha(color.A, brush.Opacity() * border.Opacity());
            if (color.A && color.A < 255) return color;
        }
    }
    try {
        auto resource = Application::Current().Resources().Lookup(
            winrt::box_value(L"SystemControlHighlightListLowBrush"));
        if (auto brush = resource.try_as<SolidColorBrush>()) {
            auto color = brush.Color();
            color.A = EffectiveBrushAlpha(color.A, brush.Opacity());
            if (color.A && color.A < 255) return color;
        }
    } catch (...) {}
    return light ? Color{16, 0, 0, 0} : Color{16, 255, 255, 255};
}

void ApplyHoverBackground() {
    if (!g_ui.surface) return;
    const bool light = g_ui.surface.ActualTheme() == ElementTheme::Light;
    Color freeColor{};
    Color usedColor{};
    {
        if (g_ui.hasRatio && (g_ui.appearance == L"bar" || g_ui.appearance == L"background")) {
            // Цветовая пара выбирается в настройках / The color pair is
            // selected in settings. Alpha stays deliberately low so text
            // remains the primary visual.
            const ColorPair pair = ColorScheme(g_ui.colorScheme);
            freeColor = pair.free;
            usedColor = pair.used;
            freeColor.A = usedColor.A = NeutralHoverColor(light).A;
        } else {
            const Color color = NeutralHoverColor(light);
            freeColor = color;
            usedColor = color;
        }
    }
    if (!g_ui.hoverBrush) {
        g_ui.hoverBrush = LinearGradientBrush();
        g_ui.hoverBrush.StartPoint(winrt::Windows::Foundation::Point{0, 0.5});
        g_ui.hoverBrush.EndPoint(winrt::Windows::Foundation::Point{1, 0.5});
        g_ui.freeStop = GradientStop();
        g_ui.freeEndStop = GradientStop();
        g_ui.usedStartStop = GradientStop();
        g_ui.usedStop = GradientStop();
        g_ui.freeStop.Offset(0);
        g_ui.usedStop.Offset(1);
        g_ui.hoverBrush.GradientStops().Append(g_ui.freeStop);
        g_ui.hoverBrush.GradientStops().Append(g_ui.freeEndStop);
        g_ui.hoverBrush.GradientStops().Append(g_ui.usedStartStop);
        g_ui.hoverBrush.GradientStops().Append(g_ui.usedStop);
        g_ui.surface.Background(g_ui.hoverBrush);
    }
    const double ratio = std::clamp(g_ui.freeRatio, 0.0, 1.0);
    g_ui.freeEndStop.Offset(ratio);
    g_ui.usedStartStop.Offset(ratio);
    if (g_ui.appearance == L"text" ||
        (!g_ui.driveCells.empty() && !g_ui.summary) ||
        (!g_ui.hovered && !g_ui.pressed)) {
        freeColor.A = 0;
        usedColor.A = 0;
    } else if (g_ui.pressed) {
        freeColor.A = static_cast<uint8_t>(freeColor.A / 2);
        usedColor.A = static_cast<uint8_t>(usedColor.A / 2);
    }
    if (g_ui.targetFree == freeColor && g_ui.targetUsed == usedColor) return;
    g_ui.targetFree = freeColor;
    g_ui.targetUsed = usedColor;
    const std::array<GradientStop, 4> stops{
        g_ui.freeStop, g_ui.freeEndStop, g_ui.usedStartStop, g_ui.usedStop};
    std::array<Color, 4> current{};
    for (size_t i = 0; i < stops.size(); ++i) current[i] = stops[i].Color();
    if (g_ui.hoverAnimation) g_ui.hoverAnimation.Stop();
    BOOL animate = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animate, 0);
    if (!animate) {
        for (size_t i = 0; i < stops.size(); ++i)
            stops[i].Color(i < 2 ? freeColor : usedColor);
        return;
    }
    using namespace winrt::Windows::UI::Xaml::Media::Animation;
    g_ui.hoverAnimation = Storyboard();
    for (size_t i = 0; i < stops.size(); ++i) {
        const Color target = i < 2 ? freeColor : usedColor;
        // Keep the final base value after this animation is replaced or stopped.
        stops[i].Color(target);
        ColorAnimation animation;
        animation.From(current[i]);
        animation.To(target);
        animation.Duration(Duration{winrt::Windows::Foundation::TimeSpan{
            g_ui.pressed ? 800000 : 1200000}});
        animation.EnableDependentAnimation(true);
        CubicEase easing;
        easing.EasingMode(EasingMode::EaseOut);
        animation.EasingFunction(easing);
        Storyboard::SetTarget(animation, stops[i]);
        Storyboard::SetTargetProperty(animation, L"Color");
        g_ui.hoverAnimation.Children().Append(animation);
    }
    g_ui.hoverAnimation.Begin();
}

// Called only by the worker: menus use the cached classification.
bool IsExternalDrive(const std::wstring& drive, UINT driveType) {
    if (driveType == DRIVE_REMOVABLE) return true;
    const std::wstring device = L"\\\\.\\" + drive;
    HANDLE handle = CreateFileW(device.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                nullptr, OPEN_EXISTING, 0, nullptr);
    if (handle == INVALID_HANDLE_VALUE) return false;
    STORAGE_PROPERTY_QUERY query{};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;
    STORAGE_DEVICE_DESCRIPTOR descriptor{};
    DWORD returned = 0;
    const bool queried = DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY,
        &query, sizeof(query), &descriptor, sizeof(descriptor), &returned, nullptr) != FALSE;
    CloseHandle(handle);
    return queried && returned >= offsetof(STORAGE_DEVICE_DESCRIPTOR, RawPropertiesLength) &&
        (descriptor.BusType == BusTypeUsb || descriptor.BusType == BusType1394 ||
         descriptor.RemovableMedia);
}

void PublishDrives(std::array<std::optional<std::wstring>, 26>& published,
                  bool russian) {
    DWORD mask = GetLogicalDrives();
    if (!mask) return;  // Do not discard the previous list on enumeration error.
    std::vector<DriveOption> drives;
    for (int i = 0; i < 26; ++i) {
        std::wstring drive{static_cast<wchar_t>(L'A' + i), L':'};
        std::wstring key = L"::wh_select_option::Drive::" + drive;
        std::wstring root = drive + L"\\";
        std::wstring label;
        const UINT driveType = (mask & (1u << i)) ? GetDriveTypeW(root.c_str()) : DRIVE_UNKNOWN;
        if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
            label = VolumeName(root);
            drives.push_back({drive, label, IsExternalDrive(drive, driveType)});
            if (label.empty()) label = UiText(russian, L"Local Disk", L"Локальный диск");
            label += L" (" + drive + L")";
        }
        if (published[i].has_value() && published[i].value() == label) continue;
        if (label.empty()) Wh_DeleteValue(key.c_str());
        else Wh_SetStringValue(key.c_str(), label.c_str());
        published[i] = std::move(label);
    }
    std::lock_guard lock(g_menuCacheMutex);
    g_cachedDrives = std::move(drives);
}

FrameworkElement FindElement(DependencyObject const& parent, PCWSTR name, int depth = 0) {
    if (!parent || depth > 24) return nullptr;
    auto element = parent.try_as<FrameworkElement>();
    if (element && element.Name() == name) return element;
    const int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto found = FindElement(VisualTreeHelper::GetChild(parent, i), name, depth + 1);
        if (found) return found;
    }
    return nullptr;
}

FrameworkElement FindFrame(DependencyObject const& parent, int depth = 0) {
    if (!parent || depth > 24) return nullptr;
    auto element = parent.try_as<FrameworkElement>();
    if (element && winrt::get_class_name(element) == L"Taskbar.TaskbarFrame") return element;
    for (int i = 0; i < VisualTreeHelper::GetChildrenCount(parent); ++i) {
        auto found = FindFrame(VisualTreeHelper::GetChild(parent, i), depth + 1);
        if (found) return found;
    }
    return nullptr;
}

FrameworkElement FindStartButton(DependencyObject const& parent, int depth = 0) {
    if (!parent || depth > 24) return nullptr;
    auto element = parent.try_as<FrameworkElement>();
    if (element && Automation::AutomationProperties::GetAutomationId(element) == L"StartButton")
        return element;
    const int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto found = FindStartButton(VisualTreeHelper::GetChild(parent, i), depth + 1);
        if (found) return found;
    }
    return nullptr;
}

FrameworkElement TaskbarFrame(HWND window, int* frameHeight = nullptr) {
    wchar_t className[64]{};
    GetClassNameW(window, className, ARRAYSIZE(className));
    const bool secondary = wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
    if (secondary && (!g_secondarySiteVtable || !g_getSecondaryHost)) return nullptr;
    HWND bandWindow = secondary ? FindWindowExW(window, nullptr, L"WorkerW", nullptr) :
        reinterpret_cast<HWND>(GetPropW(window, L"TaskbandHWND"));
    if (!bandWindow) return nullptr;
    auto site = reinterpret_cast<void**>(GetWindowLongPtrW(bandWindow, 0));
    if (!site) return nullptr;
    int i = 0;
    const auto expectedVtable = secondary ? g_secondarySiteVtable : g_siteVtable;
    for (; i < 20 && *site != expectedVtable; ++i, ++site) {}
    if (i == 20) return nullptr;
    void* host[2]{};
    (secondary ? g_getSecondaryHost : g_getHost)(site, host);
    struct Guard {
        void* ref;
        ~Guard() { if (ref) g_decref(ref); }
    } guard{host[1]};
    if (!host[0] || !host[1]) return nullptr;
    if (frameHeight && g_frameHeight) *frameHeight = g_frameHeight(host[0]);
    auto unknown = *reinterpret_cast<::IUnknown**>(
        static_cast<BYTE*>(host[0]) + g_elementOffset);
    if (!unknown) return nullptr;
    FrameworkElement element{nullptr};
    if (FAILED(unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                      winrt::put_abi(element)))) return nullptr;
    auto root = element.XamlRoot();
    if (!root) return nullptr;
    if (g_ui.frame && g_ui.frame.IsLoaded() && g_ui.frame.XamlRoot() == root)
        return g_ui.frame;
    return FindFrame(root.Content());
}

std::optional<double> TrayInset(HWND taskbar) {
    HWND tray = nullptr;
    EnumChildWindows(taskbar, [](HWND child, LPARAM context) -> BOOL {
        wchar_t name[64]{};
        if (GetClassNameW(child, name, ARRAYSIZE(name)) &&
            wcscmp(name, L"TrayNotifyWnd") == 0) {
            *reinterpret_cast<HWND*>(context) = child;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&tray));
    RECT taskbarRect{}, trayRect{};
    if (!tray || !GetWindowRect(taskbar, &taskbarRect) ||
        !GetWindowRect(tray, &trayRect) ||
        trayRect.left < taskbarRect.left || trayRect.left >= taskbarRect.right) return std::nullopt;
    const UINT dpi = GetDpiForWindow(taskbar);
    return (taskbarRect.right - trayRect.left) * 96.0 / (dpi ? dpi : 96) + 15.0;
}

struct ButtonBounds {
    double left = std::numeric_limits<double>::infinity();
    double right = 0;
    bool found = false;
};

bool IncludeVisibleButton(double start, double length, double panelLength,
                          ButtonBounds& bounds) {
    if (!std::isfinite(start) || !std::isfinite(length) || length <= 0 || panelLength <= 0)
        return false;
    const double end = start + length;
    if (end <= 0 || start >= panelLength) return false;
    bounds.left = std::min(bounds.left, std::max(0.0,start));
    bounds.right = std::max(bounds.right, std::min(panelLength,end));
    bounds.found = true;
    return true;
}

void MeasureTaskbarButtons(DependencyObject const& parent, Grid const& root,
                           ButtonBounds& bounds, int depth = 0, bool vertical = false) {
    if (!parent || depth > 24) return;
    if (auto element = parent.try_as<FrameworkElement>()) {
        if (element.Visibility() != Visibility::Visible) return;
        const auto type = winrt::get_class_name(element);
        const bool button = type == L"Taskbar.ExperienceToggleButton" ||
            type == L"Taskbar.SearchBoxButton" || type == L"Taskbar.TaskListButton" ||
            type == L"Taskbar.TaskListLabeledButton";
        if (button && element.ActualWidth() > 0 && element.ActualHeight() > 0) {
            try {
                const auto rect = element.TransformToVisual(root).TransformBounds(
                    winrt::Windows::Foundation::Rect{0, 0,
                        static_cast<float>(element.ActualWidth()),
                        static_cast<float>(element.ActualHeight())});
                IncludeVisibleButton(vertical ? rect.Y : rect.X,
                    vertical ? rect.Height : rect.Width,
                    vertical ? root.ActualHeight() : root.ActualWidth(), bounds);
                return;
            } catch (...) { /* Layout can be rebuilding during a topology change. */ }
        }
    }
    const int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i)
        MeasureTaskbarButtons(VisualTreeHelper::GetChild(parent, i), root, bounds, depth + 1, vertical);
}

// Pure calculation: left-aligned panels reserve room and must use button span,
// not current button coordinates, to avoid a feedback loop with our own margin.
double AvailableIndicatorWidth(double panelWidth, double inset, double trayInset,
                               bool right, bool reserve, const ButtonBounds& buttons) {
    constexpr double gap = 12.0;
    double available = std::max(0.0, panelWidth - inset);
    if (right) {
        if (buttons.found) available = std::min(available, panelWidth - inset - buttons.right - gap);
    } else {
        const double end = std::max(0.0, panelWidth - trayInset);
        available = std::min(available, end - inset);
        if (buttons.found) {
            const double limit = reserve ? end - (buttons.right - buttons.left) - gap :
                                           buttons.left - gap;
            available = std::min(available, limit - inset);
        }
    }
    return std::max(0.0, available);
}

bool ShouldUseCompact(bool manual, bool automatic, double normalWidth,
                      double compactWidth, double availableWidth) {
    return manual || (automatic && normalWidth > availableWidth && compactWidth < normalWidth);
}

struct LayoutGap { double left = 0; double width = 0; };
struct HorizontalGaps { LayoutGap left; LayoutGap right; };

HorizontalGaps FindHorizontalGaps(double panelWidth, double trayInset, double startX,
                                  std::vector<LayoutGap> occupied) {
    HorizontalGaps result;
    if (!std::isfinite(panelWidth) || !std::isfinite(trayInset) || !std::isfinite(startX) ||
        panelWidth <= 0 || trayInset < 0) return result;
    const double end = std::max(0.0, panelWidth - trayInset);
    startX = std::clamp(startX, 0.0, end);
    occupied.erase(std::remove_if(occupied.begin(), occupied.end(), [](const auto& item) {
        return !std::isfinite(item.left) || !std::isfinite(item.width) || item.width <= 0;
    }), occupied.end());
    std::sort(occupied.begin(), occupied.end(), [](const auto& a, const auto& b) {
        return a.left < b.left;
    });
    double cursor = 15.0, rightCursor = startX + 12.0;
    for (const auto& item : occupied) {
        if (!std::isfinite(item.left) || !std::isfinite(item.width) || item.width <= 0) continue;
        const double a = std::max(0.0, item.left), b = std::min(end, item.left + item.width);
        if (b <= a) continue;
        const double stop = std::min(startX - 12.0, a - 12.0);
        if (stop - cursor > result.left.width) result.left = {cursor, stop - cursor};
        if (a < startX) cursor = std::max(cursor, b + 12.0);
        if (b > startX) {
            const double rightStop = a - 12.0;
            if (rightStop - rightCursor > result.right.width)
                result.right = {rightCursor, rightStop - rightCursor};
            rightCursor = std::max(rightCursor, b + 12.0);
        }
    }
    const double remaining = std::min(end, startX - 12.0) - cursor;
    if (remaining > result.left.width) result.left = {cursor, remaining};
    if (end - rightCursor >= result.right.width)
        result.right = {rightCursor, std::max(0.0, end - rightCursor)};
    return result;
}

struct LayoutChoice { int mode = -1; bool right = false; LayoutGap gap; };

LayoutChoice ChooseHorizontalLayout(const HorizontalGaps& gaps, bool automatic,
                                    bool preferRight, bool fixedRight,
                                    const std::array<double,4>& widths,
                                    int firstMode, bool allowCompact) {
    for (int mode = firstMode; mode < 4; ++mode) {
        if (!allowCompact && mode != firstMode && mode != 3) continue;
        if (!std::isfinite(widths[mode]) || widths[mode] <= 0) continue;
        for (int attempt = 0; attempt < (automatic ? 2 : 1); ++attempt) {
            const bool right = automatic ? (attempt == 0 ? preferRight : !preferRight) : fixedRight;
            const auto gap = right ? gaps.right : gaps.left;
            if (widths[mode] <= gap.width) return {mode, right, gap};
        }
    }
    return {};
}

struct DrivePrefixFit { size_t visible = 0; double width = 0; };

DrivePrefixFit FitDrivePrefix(const std::vector<double>& cells,
                             const std::vector<double>& overflowWidths,
                             double available, size_t previous = SIZE_MAX) {
    if (!std::isfinite(available) || available <= 0 || cells.empty() ||
        overflowWidths.size() <= cells.size()) return {};
    std::vector<double> prefix(cells.size()+1,0);
    for (size_t i = 0; i < cells.size(); ++i) {
        if (!std::isfinite(cells[i]) || cells[i] <= 0) return {};
        prefix[i+1] = prefix[i] + cells[i] + (i ? 12.0 : 0.0);
    }
    for (size_t count = cells.size(); count > 0; --count) {
        const size_t hidden = cells.size()-count;
        const double overflow = hidden ? overflowWidths[hidden] : 0;
        if (!std::isfinite(overflow) || (hidden && overflow <= 0)) continue;
        const double width = 16.0 + prefix[count] + (hidden ? 12.0+overflow : 0.0);
        // Shrink immediately; require 8 spare DIPs before restoring another card.
        const double headroom = previous != SIZE_MAX && count > previous ? 8.0 : 0.0;
        if (width+headroom <= available) return {count,width};
    }
    return {};
}

struct DriveListChoice {
    int mode = -1;
    bool right = false;
    LayoutGap gap;
    size_t visible = 0;
    double width = 0;
};

DriveListChoice ChooseDriveListLayout(const HorizontalGaps& gaps, bool automatic,
        bool preferRight, bool fixedRight, const std::array<std::vector<double>,3>& cells,
        const std::vector<double>& overflowWidths, double summaryWidth, int firstMode,
        bool allowCompact, int previousMode = -1, size_t previousVisible = 0) {
    for (int mode = firstMode; mode < 3; ++mode) {
        if (!allowCompact && mode != firstMode) continue;
        DriveListChoice best;
        for (int attempt = 0; attempt < (automatic ? 2 : 1); ++attempt) {
            const bool right = automatic ? (attempt == 0 ? preferRight : !preferRight) : fixedRight;
            const auto gap = right ? gaps.right : gaps.left;
            const size_t previous = previousMode >= 0 &&
                (mode == previousMode || previousMode == 3) ? previousVisible : SIZE_MAX;
            const auto fit = FitDrivePrefix(cells[mode],overflowWidths,gap.width,previous);
            if (fit.visible > best.visible) best = {mode,right,gap,fit.visible,fit.width};
        }
        // Preserve the selected design. Only compact further if no whole card fits.
        if (best.visible) return best;
    }
    const auto summary = ChooseHorizontalLayout(gaps,automatic,preferRight,fixedRight,
        {0,0,0,summaryWidth},3,false);
    if (summary.mode >= 0) return {3,summary.right,summary.gap,0,summaryWidth};
    return {};
}

void CollectHorizontalButtons(DependencyObject const& parent, Grid const& root,
                              std::vector<LayoutGap>& occupied, int depth = 0) {
    if (!parent || depth > 24) return;
    if (auto element = parent.try_as<FrameworkElement>()) {
        if (element.Name() == kWidgetName || element.Visibility() != Visibility::Visible) return;
        const auto type = winrt::get_class_name(element);
        const auto id = Automation::AutomationProperties::GetAutomationId(element);
        const bool button = type == L"Taskbar.ExperienceToggleButton" ||
            type == L"Taskbar.SearchBoxButton" || type == L"Taskbar.TaskListButton" ||
            type == L"Taskbar.TaskListLabeledButton" || type == L"Taskbar.WidgetsButton" ||
            id == L"WidgetsButton" || id == L"WidgetsToggle" || id == L"StartButton";
        if (button && element.ActualWidth() > 0 && element.ActualHeight() > 0) {
            try {
                const auto rect = element.TransformToVisual(root).TransformBounds(
                    winrt::Windows::Foundation::Rect{0,0,static_cast<float>(element.ActualWidth()),
                                                       static_cast<float>(element.ActualHeight())});
                if (rect.X + rect.Width > 0 && rect.X < root.ActualWidth())
                    occupied.push_back({rect.X, rect.Width});
                return;
            } catch (...) {}
        }
    }
    for (int i = 0; i < VisualTreeHelper::GetChildrenCount(parent); ++i)
        CollectHorizontalButtons(VisualTreeHelper::GetChild(parent,i), root, occupied, depth+1);
}

struct VerticalGap { double top = 0; double height = 0; };

VerticalGap AvailableVerticalGap(double panelHeight, double trayTop,
                                 const ButtonBounds& buttons) {
    const double end = std::max(0.0, std::min(panelHeight - 12.0, trayTop - 12.0));
    if (!buttons.found) return {12.0, std::max(0.0, end - 12.0)};
    return {12.0, std::max(0.0, std::min(end, buttons.left - 12.0) - 12.0)};
}

std::wstring VerticalCapacity(std::wstring text) {
    const auto divider = text.find(L" / ");
    if (divider != std::wstring::npos) text.replace(divider, 3, L"\n/\n");
    const auto unit = text.find_last_of(L' ');
    if (unit != std::wstring::npos) text[unit] = L'\n';
    return text;
}

void ApplyDriveHover(size_t index, bool animate) {
    using namespace winrt::Windows::UI::Xaml::Media::Animation;
    if (index >= g_ui.driveCells.size()) return;
    auto& cell = g_ui.driveCells[index];
    if (!cell.surface) return;
    HIGHCONTRASTW contrast{sizeof(contrast)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0);
    const double target = DriveBackgroundOpacity(g_ui.appearance,
        cell.hasRatio && !cell.letterTile, cell.hovered,
        (contrast.dwFlags & HCF_HIGHCONTRASTON) != 0);
    if (!cell.hover) {
        cell.hover = LinearGradientBrush();
        cell.hover.StartPoint({0,0.5}); cell.hover.EndPoint({1,0.5});
        for (int i = 0; i < 4; ++i) cell.hover.GradientStops().Append(GradientStop());
        cell.hover.Opacity(0);
        cell.surface.Background(cell.hover);
    }
    const bool capacityBackground = g_ui.appearance == L"background" &&
        cell.hasRatio && !cell.letterTile && !(contrast.dwFlags & HCF_HIGHCONTRASTON);
    const Color neutral = NeutralHoverColor(cell.surface.ActualTheme() == ElementTheme::Light);
    auto highlightColor = neutral;
    highlightColor.A = EffectiveBrushAlpha(neutral.A, 0.5);
    cell.highlight.Color(highlightColor);
    const double highlightTarget = cell.hovered && g_ui.appearance != L"text" &&
        (capacityBackground || cell.letterTile) && !(contrast.dwFlags & HCF_HIGHCONTRASTON) ? 1.0 : 0.0;
    auto colors = capacityBackground ? ColorScheme(g_ui.colorScheme) : ColorPair{neutral,neutral};
    if (capacityBackground) colors.free.A = colors.used.A = neutral.A;
    for (int i = 0; i < 4; ++i) {
        auto stop = cell.hover.GradientStops().GetAt(i);
        stop.Color(i < 2 ? colors.free : colors.used);
        stop.Offset(i == 0 ? 0 : i == 3 ? 1 : cell.freeRatio);
    }
    if (!animate && cell.animation && cell.hoverTarget == target &&
        cell.highlightTarget == highlightTarget) return;
    const double current = cell.hover.Opacity();
    const double currentHighlight = cell.highlight.Opacity();
    if (cell.animation) { cell.animation.Stop(); cell.animation = nullptr; }
    cell.hoverTarget = target;
    cell.highlightTarget = highlightTarget;
    BOOL animations = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION,0,&animations,0);
    if (!animate || !animations ||
        (std::abs(target-current)<0.01 && std::abs(highlightTarget-currentHighlight)<0.01)) {
        cell.hover.Opacity(target);
        cell.highlight.Opacity(highlightTarget);
        return;
    }
    cell.hover.Opacity(current);
    cell.highlight.Opacity(currentHighlight);
    cell.animation = Storyboard();
    auto appendFade = [&](Brush const& brush, double from, double to) {
        DoubleAnimation fade;
        fade.From(from); fade.To(to);
        fade.Duration(DurationHelper::FromTimeSpan(std::chrono::milliseconds(120)));
        fade.EnableDependentAnimation(true);
        Storyboard::SetTarget(fade,brush);
        Storyboard::SetTargetProperty(fade,L"Opacity");
        cell.animation.Children().Append(fade);
    };
    appendFade(cell.hover,current,target);
    appendFade(cell.highlight,currentHighlight,highlightTarget);
    cell.animation.Begin();
}

void ClearDriveCells() {
    for (auto& cell : g_ui.driveCells) {
        if (cell.animation) { try { cell.animation.Stop(); } catch (...) {} }
        if (cell.surface && cell.handlers) {
            try { cell.surface.PointerEntered(cell.entered); cell.surface.PointerExited(cell.exited); }
            catch (...) { Wh_Log(L"Cannot remove drive hover handlers"); }
        }
    }
    g_ui.driveCells.clear();
}

void RemoveUi() {
    if (g_ui.summaryTip) {
        try {
            g_ui.summaryTip.IsOpen(false);
            if (g_ui.tooltipOwner) ToolTipService::SetToolTip(g_ui.tooltipOwner,nullptr);
        } catch (...) { Wh_Log(L"Cannot close disk summary tooltip"); }
    }
    ClearDriveCells();
    if (g_ui.hoverAnimation) {
        try { g_ui.hoverAnimation.Stop(); } catch (...) {}
    }
    // Restore only the margin we actually own, preserving external changes.
    if (g_ui.repeater && g_ui.marginApplied) {
        try {
            auto margin = g_ui.repeater.Margin();
            if (std::abs(margin.Left - g_ui.lastMargin) < 0.01) {
                margin.Left -= g_ui.reserved;
                g_ui.repeater.Margin(margin);
            }
        } catch (...) { Wh_Log(L"Cannot restore taskbar margin"); }
    }
    if (g_ui.surface && g_ui.hasTappedHandler) {
        try {
            g_ui.surface.Tapped(g_ui.tappedToken);
        } catch (...) {
            Wh_Log(L"Cannot remove taskbar disk click handler");
        }
    }
    if (g_ui.surface && g_ui.hasRightTappedHandler) {
        try { g_ui.surface.RightTapped(g_ui.rightTappedToken); }
        catch (...) { Wh_Log(L"Cannot remove taskbar disk context-menu handler"); }
    }
    if (g_ui.surface && g_ui.hasPointerHandlers) {
        try {
            g_ui.surface.PointerEntered(g_ui.pointerEnteredToken);
            g_ui.surface.PointerExited(g_ui.pointerExitedToken);
            g_ui.surface.PointerPressed(g_ui.pointerPressedToken);
            g_ui.surface.PointerReleased(g_ui.pointerReleasedToken);
            g_ui.surface.PointerCanceled(g_ui.pointerCanceledToken);
        } catch (...) {
            Wh_Log(L"Cannot remove taskbar disk hover handlers");
        }
    }
    if (g_ui.driveMenu) {
        try {
            g_ui.driveMenu.Hide();
        } catch (...) {
            Wh_Log(L"Cannot close taskbar disk menu");
        }
    }
    if (g_ui.root && g_ui.surface) {
        try {
            uint32_t index;
            if (g_ui.root.Children().IndexOf(g_ui.surface, index)) g_ui.root.Children().RemoveAt(index);
        } catch (...) { Wh_Log(L"Taskbar visual tree was already removed"); }
    }
    g_ui = {};
}

void LogTaskbarLayout(HWND window, const Settings& settings, PCWSTR stage,
                      const std::wstring& detail = L"") {
    if (!settings.layoutDiagnostics) {
        g_ui.diagnosticSignature.clear();
        return;
    }
    RECT rect{};
    const bool hasRect = GetWindowRect(window, &rect) != FALSE;
    MONITORINFO info{sizeof(info)};
    const bool hasMonitor = GetMonitorInfoW(
        MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &info) != FALSE;
    std::wstring edge = L"unknown";
    const LONG width = rect.right - rect.left;
    const LONG height = rect.bottom - rect.top;
    if (hasRect && hasMonitor) {
        if (width >= height)
            edge = std::abs(rect.top - info.rcMonitor.top) <=
                std::abs(rect.bottom - info.rcMonitor.bottom) ? L"top" : L"bottom";
        else
            edge = std::abs(rect.left - info.rcMonitor.left) <=
                std::abs(rect.right - info.rcMonitor.right) ? L"left" : L"right";
    }
    const auto signature = std::wstring(stage) + L" edge=" + edge +
        L" pixels=" + std::to_wstring(width) + L"x" + std::to_wstring(height) +
        L" dpi=" + std::to_wstring(GetDpiForWindow(window)) +
        L" position=" + settings.position + L" " + detail;
    if (g_ui.diagnosticSignature == signature) return;
    g_ui.diagnosticSignature = signature;
    Wh_Log(L"[Taskbar layout] hwnd=%p %s", window, signature.c_str());
}

bool UpdateUi(HWND window, const Settings& settings, const Reading& reading,
              bool previousThreadExited) {
    if (!settings.layoutDiagnostics) g_ui.diagnosticSignature.clear();
    int taskbarFrameHeight = 0;
    auto frame = TaskbarFrame(window, &taskbarFrameHeight);
    if (!frame) {
        LogTaskbarLayout(window, settings, L"frame-not-found");
        return false;
    }
    auto rootElement = g_ui.frame == frame && g_ui.root && g_ui.root.IsLoaded() ?
        g_ui.root.as<FrameworkElement>() : FindElement(frame, L"RootGrid");
    auto root = rootElement ? rootElement.try_as<Grid>() : nullptr;
    if (!root) {
        LogTaskbarLayout(window, settings, L"root-grid-not-found",
            L"frame-size=" + std::to_wstring(frame.ActualWidth()) + L"x" +
            std::to_wstring(frame.ActualHeight()));
        return false;
    }
    RECT taskbarRect{};
    const bool vertical = GetWindowRect(window, &taskbarRect) &&
        taskbarRect.bottom - taskbarRect.top > taskbarRect.right - taskbarRect.left;
    if (g_ui.thread && (g_ui.thread != GetCurrentThreadId() || previousThreadExited)) {
        // The old UI must be removed on its own thread. If that thread is gone,
        // its XAML references cannot safely be released on this thread. Retire
        // them without running their destructors and build a fresh widget here.
        if (!previousThreadExited) return false;
        auto* retired = new UiState(std::move(g_ui));
        (void)retired;
        g_ui = {};
        Wh_Log(L"Previous taskbar UI thread exited; rebuilding disk space widget");
    }
    if (g_ui.root != root) {
        RemoveUi();
        g_ui.thread = GetCurrentThreadId();
        g_ui.frame = frame;
        g_ui.root = root;
        g_ui.repeater = FindElement(root, L"TaskbarFrameRepeater");
        g_ui.surface = Border();
        g_ui.surface.Name(kWidgetName);
        // Border gives the hover state the rounded, quiet Fluent surface used
        // by Windows 11 taskbar buttons. Its transparent background keeps the
        // whole measured surface clickable, including its padding.
        g_ui.surface.IsHitTestVisible(true);
        g_ui.surface.CornerRadius(CornerRadius{4, 4, 4, 4});
        g_ui.surface.Padding(Thickness{8, 2, 8, 2});
        g_ui.surface.HorizontalAlignment(HorizontalAlignment::Left);
        g_ui.surface.VerticalAlignment(VerticalAlignment::Center);
        Canvas::SetZIndex(g_ui.surface, 1000);
        Grid::SetColumnSpan(g_ui.surface, std::max(1, static_cast<int>(root.ColumnDefinitions().Size())));
        g_ui.widget = StackPanel();
        g_ui.widget.IsHitTestVisible(false);
        g_ui.widget.Orientation(Orientation::Vertical);
        g_ui.widget.HorizontalAlignment(HorizontalAlignment::Stretch);
        g_ui.widget.VerticalAlignment(VerticalAlignment::Center);
        g_ui.title = TextBlock();
        g_ui.capacity = TextBlock();
        g_ui.overflowSurface = Border();
        g_ui.overflowSurface.CornerRadius(CornerRadius{4});
        g_ui.overflowSurface.Padding(Thickness{8,4,8,4});
        g_ui.overflowSurface.Margin(Thickness{12,0,0,0});
        g_ui.overflowSurface.VerticalAlignment(VerticalAlignment::Center);
        g_ui.overflowSurface.Background(SolidColorBrush(Color{12,128,128,128}));
        g_ui.overflowSurface.Visibility(Visibility::Collapsed);
        g_ui.overflowText = TextBlock();
        g_ui.overflowText.IsHitTestVisible(false);
        g_ui.overflowText.FontFamily(FontFamily(L"Segoe UI Variable Text"));
        g_ui.overflowText.FontSize(12);
        g_ui.overflowText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        g_ui.overflowSurface.Child(g_ui.overflowText);
        for (auto text : {g_ui.title, g_ui.capacity}) {
            text.FontFamily(FontFamily(L"Segoe UI Variable Text"));
            text.FontSize(12);
            text.VerticalAlignment(VerticalAlignment::Center);
            text.TextTrimming(TextTrimming::CharacterEllipsis);
            text.TextWrapping(TextWrapping::NoWrap);
            g_ui.widget.Children().Append(text);
        }
        g_ui.surface.Child(g_ui.widget);
        g_ui.title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        g_ui.tappedToken = g_ui.surface.Tapped(
            [state = &g_ui](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::UI::Xaml::Input::TappedRoutedEventArgs const&) {
                UiScope scope(state);
                ShowDriveMenu();
            });
        g_ui.hasTappedHandler = true;
        g_ui.rightTappedToken = g_ui.surface.RightTapped(
            [state = &g_ui](auto const&, winrt::Windows::UI::Xaml::Input::RightTappedRoutedEventArgs const& args) {
                UiScope scope(state);
                args.Handled(true);
                ShowThemeMenu();
            });
        g_ui.hasRightTappedHandler = true;
        g_ui.pointerEnteredToken = g_ui.surface.PointerEntered(
            [state = &g_ui](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) {
                UiScope scope(state);
                g_ui.hovered = true;
                ApplyHoverBackground();
            });
        g_ui.pointerExitedToken = g_ui.surface.PointerExited(
            [state = &g_ui](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) {
                UiScope scope(state);
                g_ui.hovered = false;
                g_ui.pressed = false;
                ApplyHoverBackground();
            });
        g_ui.pointerPressedToken = g_ui.surface.PointerPressed(
            [state = &g_ui](auto const&, auto const& args) {
                UiScope scope(state);
                if (!args.GetCurrentPoint(g_ui.surface).Properties().IsLeftButtonPressed()) return;
                g_ui.pressed = true;
                ApplyHoverBackground();
            });
        g_ui.pointerReleasedToken = g_ui.surface.PointerReleased(
            [state = &g_ui](auto const&, auto const&) {
                UiScope scope(state);
                g_ui.pressed = false;
                ApplyHoverBackground();
            });
        g_ui.pointerCanceledToken = g_ui.surface.PointerCanceled(
            [state = &g_ui](auto const&, auto const&) {
                UiScope scope(state);
                g_ui.pressed = false;
                g_ui.hovered = false;
                ApplyHoverBackground();
            });
        g_ui.hasPointerHandlers = true;
        root.Children().Append(g_ui.surface);
        Wh_Log(L"Disk space widget added to taskbar");
    }
    const bool multiple = !reading.drives.empty();
    if (vertical && multiple && !g_ui.verticalViewport) {
        g_ui.surface.Child(nullptr);
        g_ui.verticalViewport = ScrollViewer();
        g_ui.verticalViewport.Padding(Thickness{0,0,0,0});
        g_ui.verticalViewport.BorderThickness(Thickness{0,0,0,0});
        g_ui.verticalViewport.Background(SolidColorBrush(Color{0,0,0,0}));
        g_ui.verticalViewport.HorizontalScrollMode(ScrollMode::Disabled);
        g_ui.verticalViewport.HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
        g_ui.verticalViewport.VerticalScrollMode(ScrollMode::Enabled);
        g_ui.verticalViewport.VerticalScrollBarVisibility(ScrollBarVisibility::Hidden);
        g_ui.verticalViewport.Content(g_ui.widget);
        g_ui.surface.Child(g_ui.verticalViewport);
    } else if ((!vertical || !multiple) && g_ui.verticalViewport) {
        g_ui.verticalViewport.Content(nullptr);
        g_ui.surface.Child(nullptr);
        g_ui.verticalViewport = nullptr;
        g_ui.surface.Child(g_ui.widget);
    }
    if (g_ui.summary || g_ui.driveCells.size() != reading.drives.size() ||
        (multiple && g_ui.widget.Children().Size() != reading.drives.size()+1)) {
        if (g_ui.tooltipOwner == g_ui.overflowSurface) ClearCapacityTooltip();
        g_ui.widget.Children().Clear();
        ClearDriveCells();
        g_ui.summary = false;
        if (multiple) {
            for (size_t i = 0; i < reading.drives.size(); ++i) {
                DriveCell cell;
                cell.surface = Border();
                cell.surface.CornerRadius(CornerRadius{4,4,4,4});
                cell.surface.Padding(Thickness{4,2,4,2});
                cell.surface.Background(SolidColorBrush(Color{0,0,0,0}));
                cell.panel = StackPanel();
                cell.panel.IsHitTestVisible(false);
                cell.highlight = SolidColorBrush(Color{0,0,0,0});
                cell.highlight.Opacity(0);
                cell.highlightLayer = Border();
                cell.highlightLayer.IsHitTestVisible(false);
                cell.highlightLayer.Margin(Thickness{-4,-2,-4,-2});
                cell.highlightLayer.Background(cell.highlight);
                Grid cellContent;
                cellContent.Children().Append(cell.highlightLayer);
                cellContent.Children().Append(cell.panel);
                cell.surface.Child(cellContent);
                cell.entered = cell.surface.PointerEntered([state=&g_ui,i](auto const&, auto const&) {
                    UiScope scope(state);
                    if (i < g_ui.driveCells.size()) { g_ui.driveCells[i].hovered = true; ApplyDriveHover(i,true); }
                });
                cell.exited = cell.surface.PointerExited([state=&g_ui,i](auto const&, auto const&) {
                    UiScope scope(state);
                    if (i < g_ui.driveCells.size()) { g_ui.driveCells[i].hovered = false; ApplyDriveHover(i,true); }
                });
                cell.handlers = true;
                cell.panel.Margin(Thickness{0,0,0,0});
                cell.title = TextBlock();
                cell.capacity = TextBlock();
                for (auto text : {cell.title, cell.capacity}) {
                    text.FontFamily(FontFamily(L"Segoe UI Variable Text"));
                    text.FontSize(12);
                    text.TextTrimming(TextTrimming::CharacterEllipsis);
                    text.TextWrapping(TextWrapping::NoWrap);
                    cell.panel.Children().Append(text);
                }
                cell.title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
                cell.bar = Grid();
                cell.bar.Height(2);
                cell.bar.Margin(Thickness{0, 1, 0, 0});
                cell.freeColumn = ColumnDefinition();
                cell.usedColumn = ColumnDefinition();
                cell.bar.ColumnDefinitions().Append(cell.freeColumn);
                cell.bar.ColumnDefinitions().Append(cell.usedColumn);
                cell.freePart = Border();
                cell.usedPart = Border();
                Grid::SetColumn(cell.usedPart, 1);
                cell.bar.Children().Append(cell.freePart);
                cell.bar.Children().Append(cell.usedPart);
                cell.panel.Children().Append(cell.bar);
                g_ui.widget.Children().Append(cell.surface);
                g_ui.driveCells.push_back(std::move(cell));
            }
            g_ui.widget.Children().Append(g_ui.overflowSurface);
        } else {
            g_ui.widget.Children().Append(g_ui.title);
            g_ui.widget.Children().Append(g_ui.capacity);
        }
    }
    g_ui.widget.IsHitTestVisible(multiple);
    // TaskbarHost reports the whole taskbar height, not the visible hover frame.
    // Read the Start button background itself so taskbar customizations and DPI
    // scaling are reflected in the indicator without a separate size setting.
    double hoverHeight = taskbarFrameHeight > 8 && taskbarFrameHeight <= 256 ? taskbarFrameHeight - 8.0 : 40.0;
    if (!g_ui.startButton || !g_ui.startButton.IsLoaded()) {
        g_ui.startButton = FindStartButton(root);
        g_ui.startBackground = nullptr;
    }
    auto startButton = g_ui.startButton;
    if (startButton) {
        if (!g_ui.startBackground || !g_ui.startBackground.IsLoaded())
            g_ui.startBackground = FindElement(startButton, L"BackgroundElement");
        auto background = g_ui.startBackground;
        if (background && background.ActualHeight() > 0) {
            hoverHeight = background.ActualHeight();
            if (auto border = background.try_as<Border>()) {
                if (g_ui.surface.CornerRadius() != border.CornerRadius())
                    g_ui.surface.CornerRadius(border.CornerRadius());
            }
        } else if (startButton.ActualHeight() > 0) {
            hoverHeight = startButton.ActualHeight();
        }
    }
    g_ui.surface.MinHeight(hoverHeight);
    if (vertical) {
        g_ui.surface.Height(std::numeric_limits<double>::quiet_NaN());
        g_ui.surface.Padding(Thickness{4,3,4,3});
        Grid::SetRowSpan(g_ui.surface, std::max(1, static_cast<int>(root.RowDefinitions().Size())));
    } else {
        g_ui.surface.Height(hoverHeight);
        g_ui.surface.Padding(Thickness{8,2,8,2});
        Grid::SetRowSpan(g_ui.surface, 1);
    }
    const auto nativeRadius = g_ui.surface.CornerRadius();
    for (auto& cell : g_ui.driveCells) {
        cell.surface.CornerRadius(nativeRadius);
        cell.highlightLayer.CornerRadius(nativeRadius);
        cell.surface.Height(vertical ? std::numeric_limits<double>::quiet_NaN() : hoverHeight);
        cell.surface.MinHeight(hoverHeight);
        cell.panel.VerticalAlignment(VerticalAlignment::Center);
    }
    g_ui.overflowSurface.CornerRadius(nativeRadius);
    g_ui.overflowSurface.Height(hoverHeight);
    g_ui.overflowText.VerticalAlignment(VerticalAlignment::Center);
    auto overflowColor = NeutralHoverColor(g_ui.surface.ActualTheme() == ElementTheme::Light);
    g_ui.overflowSurface.Background(SolidColorBrush(overflowColor));
    for (auto text : {g_ui.title, g_ui.capacity}) {
        text.FontSize(vertical ? 11 : 12);
        text.TextAlignment(vertical ? TextAlignment::Center : TextAlignment::Left);
        text.TextWrapping(vertical ? TextWrapping::Wrap : TextWrapping::NoWrap);
    }
    g_ui.widget.Orientation(settings.hideDriveName ? Orientation::Horizontal :
                            Orientation::Vertical);
    g_ui.capacity.Margin(settings.hideDriveName ? Thickness{8, 0, 0, 0} :
                         Thickness{0, 0, 0, 0});
    // Let the Border measure its content so the hover frame does not extend
    // far beyond the text. Only the available taskbar space limits its width.
    const double leftOffset = settings.position == L"left" ? 15.0 : static_cast<double>(settings.offset);
    const bool reserveSpace = !vertical && settings.position == L"left" && TaskbarIconsLeftAligned();
    // Restore our prior reservation before automatic geometry, without touching other mods' margins.
    if (!reserveSpace && g_ui.repeater && g_ui.marginApplied) {
        auto margin = g_ui.repeater.Margin();
        if (std::abs(margin.Left - g_ui.lastMargin) < 0.01) {
            margin.Left -= g_ui.reserved;
            g_ui.repeater.Margin(margin);
        }
        g_ui.marginApplied = false;
        g_ui.reserved = 0;
    }
    ButtonBounds buttons;
    MeasureTaskbarButtons(root, root, buttons, 0, vertical);
    VerticalGap verticalGap;
    double maxWidth = std::numeric_limits<double>::infinity();
    double inset = 15.0;
    bool placedRight = settings.position == L"right";
    bool fits = true;
    HorizontalGaps horizontalGaps;
    if (vertical) {
        double trayTop = root.ActualHeight();
        auto tray = FindElement(frame, L"SystemTrayFrame");
        if (!tray) tray = FindElement(frame, L"SystemTrayFrameGrid");
        if (tray && tray.ActualHeight() > 0) {
            try {
                trayTop = tray.TransformToVisual(root).TransformPoint(
                    winrt::Windows::Foundation::Point{0,0}).Y;
            } catch (...) { trayTop = 0; }
        } else if (HWND trayWindow = FindWindowExW(window, nullptr, L"TrayNotifyWnd", nullptr)) {
            RECT rect{};
            const auto dpi = GetDpiForWindow(window);
            trayTop = GetWindowRect(trayWindow, &rect) ?
                (rect.top - taskbarRect.top) * 96.0 / (dpi ? dpi : 96) : 0;
        }
        verticalGap = AvailableVerticalGap(root.ActualHeight(), trayTop, buttons);
        maxWidth = std::max(0.0, root.ActualWidth() - 4.0);
    }
    if (!vertical) {
        double trayWidth = TrayInset(window).value_or(0.0);
        bool trayFound = trayWidth > 0;
        auto tray = FindElement(frame, L"SystemTrayFrame");
        if (!tray) tray = FindElement(frame, L"SystemTrayFrameGrid");
        if (tray && tray.Visibility() == Visibility::Visible && tray.ActualWidth() > 0) {
            try {
                const auto x = tray.TransformToVisual(root).TransformPoint(
                    winrt::Windows::Foundation::Point{0,0}).X;
                if (x >= 0 && x < root.ActualWidth()) {
                    trayWidth = root.ActualWidth() - x + 12.0;
                    trayFound = true;
                }
            } catch (...) {}
        }
        wchar_t className[64]{};
        GetClassNameW(window, className, ARRAYSIZE(className));
        if (!trayFound && wcscmp(className,L"Shell_SecondaryTrayWnd") == 0) {
            trayWidth = 15; trayFound = true;
        }
        std::vector<LayoutGap> occupied;
        CollectHorizontalButtons(root, root, occupied);
        double startX = buttons.found ? buttons.left : 0;
        if (g_ui.startButton && g_ui.startButton.IsLoaded()) {
            try { startX = g_ui.startButton.TransformToVisual(root).TransformPoint(
                    winrt::Windows::Foundation::Point{0,0}).X; } catch (...) {}
        }
        horizontalGaps = FindHorizontalGaps(root.ActualWidth(), trayWidth, startX, occupied);
        if (reserveSpace) horizontalGaps.left = {15, AvailableIndicatorWidth(
            root.ActualWidth(),15,trayWidth,false,true,buttons)};
        // Unknown native geometry is transient; avoid covering elements while it rebuilds.
        if (!buttons.found || !trayFound) horizontalGaps = {};
    }
    g_ui.surface.Visibility(Visibility::Visible);
    g_ui.surface.Width(std::numeric_limits<double>::quiet_NaN());
    g_ui.surface.MaxWidth(std::numeric_limits<double>::infinity());
    // DesiredSize includes Margin. Measure only contents, never the previous screen position.
    // Leaving a right-hand offset here caused widths to grow on each subsequent UI probe.
    g_ui.surface.Margin(Thickness{0,0,0,0});
    auto setLayout = [&](int mode) {
        const bool compact = mode == 1;
        const bool tiles = mode == 2;
        if (!vertical) g_ui.surface.Height(hoverHeight + (multiple ? 4.0 : 0.0));
        g_ui.widget.MinWidth(0);
        g_ui.widget.Background(nullptr);
        g_ui.title.TextAlignment(TextAlignment::Left);
        g_ui.capacity.Visibility(Visibility::Visible);
        auto tileBackground = [&](const Reading& disk) -> Brush {
            HIGHCONTRASTW contrast{sizeof(contrast)};
            SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0);
            if (!disk.hasRatio || (contrast.dwFlags & HCF_HIGHCONTRASTON)) return nullptr;
            const auto colors = ColorScheme(settings.colorScheme);
            Color free = colors.free, used = colors.used;
            free.A = used.A = NeutralHoverColor(g_ui.surface.ActualTheme() == ElementTheme::Light).A;
            LinearGradientBrush brush;
            brush.StartPoint({0,0.5}); brush.EndPoint({1,0.5});
            for (int i = 0; i < 4; ++i) {
                GradientStop stop;
                stop.Offset(i == 0 ? 0 : i == 3 ? 1 : disk.freeRatio);
                stop.Color(i < 2 ? free : used);
                brush.GradientStops().Append(stop);
            }
            return brush;
        };
        if (tiles && !multiple) {
            g_ui.widget.Orientation(Orientation::Vertical);
            g_ui.widget.MinWidth(32);
            g_ui.widget.Background(tileBackground(reading));
            g_ui.title.Text(reading.compactTitle.empty() ? L"?" : reading.compactTitle);
            g_ui.title.TextAlignment(TextAlignment::Center);
            g_ui.capacity.Visibility(Visibility::Collapsed);
            return;
        }
        if (vertical && !multiple) {
            g_ui.widget.Orientation(Orientation::Vertical);
            g_ui.capacity.Margin(Thickness{0,2,0,0});
            g_ui.title.Text(reading.compactTitle.empty() ? reading.title : reading.compactTitle);
            g_ui.capacity.Text(VerticalCapacity(reading.compactCapacity.empty() ? reading.capacity : reading.compactCapacity));
            return;
        }
        if (multiple) {
            g_ui.widget.Orientation(vertical ? Orientation::Vertical : Orientation::Horizontal);
            for (size_t i = 0; i < g_ui.driveCells.size(); ++i) {
                auto& cell = g_ui.driveCells[i];
                const auto& disk = reading.drives[i];
                cell.surface.Visibility(Visibility::Visible);
                cell.letterTile = tiles;
                cell.surface.Margin(vertical ? Thickness{0, i ? 8.0 : 0.0, 0, 0} :
                    Thickness{i ? 12.0 : 0.0, 0, 0, 0});
                cell.panel.MaxWidth(std::numeric_limits<double>::infinity());
                cell.panel.MinWidth(tiles ? 32 : 0);
                cell.panel.Height(tiles ? std::min(hoverHeight-4.0,32.0) : std::numeric_limits<double>::quiet_NaN());
                cell.panel.Background(tiles ? tileBackground(disk) : nullptr);
                for (auto text : {cell.title,cell.capacity}) {
                    text.FontSize(vertical ? 11 : 12);
                    text.TextAlignment(vertical || tiles ? TextAlignment::Center : TextAlignment::Left);
                    text.VerticalAlignment(VerticalAlignment::Center);
                    text.TextWrapping(vertical ? TextWrapping::Wrap : TextWrapping::NoWrap);
                }
                cell.title.Text(tiles ? disk.compactTitle : vertical ? disk.compactTitle : compact ? disk.compactTitle + L" " + disk.compactCapacity : disk.title);
                cell.capacity.Text(vertical ? VerticalCapacity(disk.compactCapacity.empty() ? disk.capacity : disk.compactCapacity) : disk.capacity);
                cell.capacity.Visibility(tiles || (!vertical && compact) ? Visibility::Collapsed : Visibility::Visible);
                cell.bar.Visibility(!tiles && settings.appearance == L"bar" && disk.hasRatio ?
                    Visibility::Visible : Visibility::Collapsed);
                const auto colors = ColorScheme(settings.colorScheme);
                auto free = colors.free;
                auto used = colors.used;
                free.A = used.A = 180;
                cell.freePart.Background(SolidColorBrush(free));
                cell.usedPart.Background(SolidColorBrush(used));
                cell.freeColumn.Width(GridLength{disk.freeRatio, GridUnitType::Star});
                cell.usedColumn.Width(GridLength{1.0 - disk.freeRatio, GridUnitType::Star});
            }
            return;
        }
        g_ui.widget.Orientation(compact ? Orientation::Horizontal : Orientation::Vertical);
        g_ui.capacity.Margin(compact ? Thickness{8, 0, 0, 0} : Thickness{0, 0, 0, 0});
        const auto& title = compact && !reading.compactTitle.empty() ? reading.compactTitle : reading.title;
        const auto& capacity = compact && !reading.compactCapacity.empty() ? reading.compactCapacity : reading.capacity;
        if (g_ui.title.Text() != title) g_ui.title.Text(title);
        if (g_ui.capacity.Text() != capacity) g_ui.capacity.Text(capacity);
    };
    const auto count = settings.showAllDrives ? reading.drives.size() : 1;
    const auto summaryText = UiText(IsRussianUi(), L"Drives · ", L"Диски · ") + std::to_wstring(count);
    auto setSummary = [&] {
        if (!vertical) g_ui.surface.Height(hoverHeight);
        if (g_ui.tooltipOwner == g_ui.overflowSurface) ClearCapacityTooltip();
        g_ui.widget.Children().Clear();
        g_ui.widget.Children().Append(g_ui.title);
        g_ui.widget.Background(nullptr);
        g_ui.widget.MinWidth(0);
        g_ui.widget.Orientation(Orientation::Vertical);
        g_ui.title.Text(vertical ? UiText(IsRussianUi(),L"Drives\n",L"Диски\n") + std::to_wstring(count) : summaryText);
        g_ui.title.TextAlignment(vertical ? TextAlignment::Center : TextAlignment::Left);
        g_ui.summary = true;
    };
    int layoutMode = settings.summaryButton ? 3 : settings.letterTiles ? 2 : settings.hideDriveName ? 1 : 0;
    g_ui.hiddenDrives = 0;
    std::array<std::vector<double>,3> measuredCells;
    std::array<double,4> measured{};
    const winrt::Windows::Foundation::Size unlimited{
        std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
    if (!vertical) {
        for (int mode = 0; mode < 3; ++mode) {
            setLayout(mode);
            g_ui.surface.Measure(unlimited);
            measured[mode] = g_ui.surface.DesiredSize().Width;
            for (auto& cell : g_ui.driveCells) {
                cell.surface.Measure(unlimited);
                const auto margin = cell.surface.Margin();
                measuredCells[mode].push_back(cell.surface.DesiredSize().Width-margin.Left-margin.Right);
            }
            if (multiple) {
                // Do not collapse the tooltip's owner just to measure an all-drive view.
                // The live overflow popup stays attached throughout a layout probe.
                measured[mode] = 16.0;
                for (size_t i = 0; i < measuredCells[mode].size(); ++i)
                    measured[mode] += measuredCells[mode][i] + (i ? 12.0 : 0.0);
            }
        }
        TextBlock summaryMeasure;
        summaryMeasure.FontFamily(g_ui.title.FontFamily());
        summaryMeasure.FontSize(12);
        summaryMeasure.FontWeight(g_ui.title.FontWeight());
        summaryMeasure.Text(summaryText);
        summaryMeasure.Measure(unlimited);
        measured[3] = summaryMeasure.DesiredSize().Width + 16.0;
        double chosenWidth = 0;
        size_t visible = count;
        LayoutChoice choice;
        if (multiple) {
            std::vector<double> overflowWidths(count+1,0);
            TextBlock moreMeasure;
            moreMeasure.FontFamily(g_ui.overflowText.FontFamily());
            moreMeasure.FontSize(g_ui.overflowText.FontSize());
            moreMeasure.FontWeight(g_ui.overflowText.FontWeight());
            for (size_t hidden = 1; hidden <= count; ++hidden) {
                moreMeasure.Text(UiText(IsRussianUi(),L"More · ",L"Ещё · ")+std::to_wstring(hidden));
                moreMeasure.Measure(unlimited);
                overflowWidths[hidden] = moreMeasure.DesiredSize().Width+16.0;
            }
            std::wstring key = settings.position + L"|" + settings.driveGroup + L"|" +
                settings.format + L"|" + std::to_wstring(settings.precision) + L"|" +
                std::to_wstring(layoutMode) + std::to_wstring(settings.autoCompact) + L"|" +
                std::to_wstring(GetDpiForWindow(window));
            for (const auto& disk : reading.drives) key += L"|" + disk.title;
            const auto list = ChooseDriveListLayout(horizontalGaps,settings.position == L"auto",
                TaskbarIconsLeftAligned(),settings.position == L"right",measuredCells,overflowWidths,
                measured[3],layoutMode,settings.autoCompact,
                key == g_ui.listLayoutSignature ? g_ui.listLayoutMode : -1,
                g_ui.visibleDrives);
            choice = {list.mode,list.right,list.gap};
            chosenWidth = list.width;
            visible = list.visible;
            g_ui.listLayoutSignature = std::move(key);
            g_ui.listLayoutMode = list.mode;
        } else {
            choice = ChooseHorizontalLayout(horizontalGaps,settings.position == L"auto",
                TaskbarIconsLeftAligned(),settings.position == L"right",measured,layoutMode,settings.autoCompact);
            if (choice.mode >= 0) chosenWidth = measured[choice.mode];
            g_ui.listLayoutMode = -1;
            g_ui.listLayoutSignature.clear();
        }
        if (choice.mode == 3) visible = 0;
        g_ui.visibleDrives = visible;
        fits = choice.mode >= 0;
        if (fits) {
            layoutMode = choice.mode; placedRight = choice.right;
            maxWidth = choice.gap.width; inset = choice.gap.left;
            if (placedRight) inset += maxWidth - chosenWidth;
            if (settings.position == L"auto" && !placedRight)
                inset = std::clamp(static_cast<double>(settings.offset), inset,
                                   inset + maxWidth - chosenWidth);
            if (layoutMode == 3) setSummary();
            else {
                setLayout(layoutMode);
                if (multiple) {
                    g_ui.hiddenDrives = count-visible;
                    for (size_t i = visible; i < g_ui.driveCells.size(); ++i) {
                        g_ui.driveCells[i].hovered = false;
                        g_ui.driveCells[i].surface.Visibility(Visibility::Collapsed);
                    }
                    if (g_ui.hiddenDrives) {
                        g_ui.overflowText.Text(UiText(IsRussianUi(),L"More · ",L"Ещё · ")+
                            std::to_wstring(g_ui.hiddenDrives));
                        g_ui.overflowSurface.Visibility(Visibility::Visible);
                        Automation::AutomationProperties::SetName(g_ui.overflowSurface,
                            UiText(IsRussianUi(),L"Hidden drives: ",L"Диски вне панели: ")+
                            std::to_wstring(g_ui.hiddenDrives));
                        Automation::AutomationProperties::SetHelpText(g_ui.overflowSurface,
                            UiText(IsRussianUi(),L"Hover for disk space; click to open the drive menu",
                                L"Наведите для просмотра места; нажмите для открытия меню дисков"));
                    }
                }
            }
            maxWidth = choice.gap.left + choice.gap.width - inset;
        } else { maxWidth = 0; if (layoutMode == 3) setSummary(); else setLayout(layoutMode); }
        LogTaskbarLayout(window, settings, fits ? L"layout-visible" : L"layout-hidden-no-space",
            L"root=" + std::to_wstring(root.ActualWidth()) +
            L" drives=" + std::to_wstring(count) +
            L" left=" + std::to_wstring(horizontalGaps.left.left) + L":" + std::to_wstring(horizontalGaps.left.width) +
            L" right=" + std::to_wstring(horizontalGaps.right.left) + L":" + std::to_wstring(horizontalGaps.right.width) +
            L" measured-full=" + std::to_wstring(measured[0]) +
            L" compact=" + std::to_wstring(measured[1]) + L" tiles=" + std::to_wstring(measured[2]) +
            L" summary=" + std::to_wstring(measured[3]) +
            L" chosen=" + std::to_wstring(fits ? layoutMode : -1) +
            L" visible=" + std::to_wstring(fits ? visible : 0) +
            L" hidden=" + std::to_wstring(fits ? count-visible : count) +
            L" chosen-width=" + std::to_wstring(chosenWidth) +
            L" side=" + (placedRight ? std::wstring(L"right") : std::wstring(L"left")) +
            L" buttons=" + std::to_wstring(buttons.left) + L".." + std::to_wstring(buttons.right));
    } else {
        g_ui.listLayoutMode = -1;
        g_ui.listLayoutSignature.clear();
        g_ui.visibleDrives = count;
        if (layoutMode == 3) setSummary(); else setLayout(layoutMode);
        fits = buttons.found && maxWidth >= 28.0 && verticalGap.height >= 100.0;
    }
    if (!g_ui.hiddenDrives) {
        if (g_ui.tooltipOwner == g_ui.overflowSurface) ClearCapacityTooltip();
        g_ui.overflowSurface.Visibility(Visibility::Collapsed);
    }
    g_ui.surface.MaxWidth(maxWidth);
    if (multiple && vertical && !g_ui.summary && std::isfinite(maxWidth)) {
        const double columnWidth = vertical ? std::max(1.0, maxWidth - 8.0) : std::max(1.0,
            (maxWidth - 16.0 - 12.0 * (reading.drives.size() - 1)) / reading.drives.size());
        for (auto& cell : g_ui.driveCells) cell.panel.MaxWidth(columnWidth);
    }
    if (g_ui.verticalViewport) {
        const double contentWidth = std::max(1.0,maxWidth-8.0);
        const double contentHeight = std::max(1.0,verticalGap.height-6.0);
        g_ui.widget.Measure(winrt::Windows::Foundation::Size{
            static_cast<float>(contentWidth),std::numeric_limits<float>::infinity()});
        g_ui.verticalViewport.Width(contentWidth);
        g_ui.verticalViewport.Height(std::min(contentHeight,
            static_cast<double>(g_ui.widget.DesiredSize().Height)));
        g_ui.verticalViewport.MaxWidth(contentWidth);
        g_ui.verticalViewport.MaxHeight(contentHeight);
    }
    // Measure the current text before reserving space for taskbar buttons.
    g_ui.surface.Measure(winrt::Windows::Foundation::Size{
        static_cast<float>(maxWidth), vertical ? static_cast<float>(std::max(0.0,verticalGap.height)) :
            std::numeric_limits<float>::infinity()});
    if (vertical) fits = fits && g_ui.surface.DesiredSize().Height <= verticalGap.height;
    else fits = fits && std::isfinite(g_ui.surface.DesiredSize().Width) &&
        g_ui.surface.DesiredSize().Width <= maxWidth + 0.5 && inset >= 0 &&
        inset + g_ui.surface.DesiredSize().Width <= root.ActualWidth() + 0.5;
    if (vertical && settings.layoutDiagnostics) LogTaskbarLayout(window, settings,
        fits ? L"vertical-visible" : L"vertical-hidden-no-space",
        L"root=" + std::to_wstring(root.ActualWidth()) + L"x" + std::to_wstring(root.ActualHeight()) +
        L" buttons-y=" + std::to_wstring(buttons.left) + L".." + std::to_wstring(buttons.right) +
        L" gap-top=" + std::to_wstring(verticalGap.top) + L" gap-height=" + std::to_wstring(verticalGap.height) +
        L" desired=" + std::to_wstring(g_ui.surface.DesiredSize().Width) + L"x" +
        std::to_wstring(g_ui.surface.DesiredSize().Height));
    const double width = fits ? g_ui.surface.DesiredSize().Width : 0.0;
    if (!fits) g_ui.surface.Visibility(Visibility::Collapsed);
    const auto alignment = vertical ? HorizontalAlignment::Center : HorizontalAlignment::Left;
    if (g_ui.surface.HorizontalAlignment() != alignment)
        g_ui.surface.HorizontalAlignment(alignment);
    g_ui.surface.VerticalAlignment(vertical ? VerticalAlignment::Top : VerticalAlignment::Center);
    Thickness placement = vertical ? Thickness{0,verticalGap.top,0,0} : Thickness{inset,0,0,0};
    if (g_ui.surface.Margin() != placement) g_ui.surface.Margin(placement);
    g_ui.freeRatio = reading.freeRatio;
    g_ui.hasRatio = reading.hasRatio;
    UpdateSummaryTooltip(window,settings,reading,vertical);
    g_ui.colorScheme = settings.colorScheme;
    g_ui.appearance = settings.appearance;
    ApplyHoverBackground();
    for (size_t i = 0; i < g_ui.driveCells.size(); ++i) {
        auto& cell = g_ui.driveCells[i];
        cell.freeRatio = reading.drives[i].freeRatio;
        cell.hasRatio = reading.drives[i].hasRatio;
        ApplyDriveHover(i,false);
    }
    if (g_ui.repeater) {
        auto margin = g_ui.repeater.Margin();
        double base = margin.Left;
        if (g_ui.marginApplied && std::abs(base - g_ui.lastMargin) < 0.01) base -= g_ui.reserved;
        const bool reserve = reserveSpace && fits;
        g_ui.reserved = reserve ? leftOffset + width + 12 : 0;
        double desired = base + g_ui.reserved;
        if (std::abs(margin.Left - desired) > 0.01) {
            margin.Left = desired;
            g_ui.repeater.Margin(margin);
        }
        g_ui.marginApplied = reserve;
        g_ui.lastMargin = desired;
    }
    // No theme event handlers or polling timers are retained in Explorer's XAML tree.
    // The worker dispatches on new readings, settings changes and a five-second
    // probe that detects a recreated taskbar without polling every second.
    HIGHCONTRASTW contrast{sizeof(contrast)};
    SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0);
    COLORREF rgb = (contrast.dwFlags & HCF_HIGHCONTRASTON) ? GetSysColor(COLOR_WINDOWTEXT) :
        g_ui.widget.ActualTheme() == ElementTheme::Light ? RGB(24, 24, 24) : RGB(245, 245, 245);
    // Preserve the user's high-contrast foreground for readability.
    if (reading.lowSpace && !(contrast.dwFlags & HCF_HIGHCONTRASTON)) {
        rgb = g_ui.widget.ActualTheme() == ElementTheme::Light ?
            RGB(180, 30, 30) : RGB(255, 125, 125);
    }
    Color color{255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb)};
    for (auto text : {g_ui.title, g_ui.capacity}) {
        auto brush = text.Foreground().try_as<SolidColorBrush>();
        if (!brush || brush.Color() != color) text.Foreground(SolidColorBrush(color));
    }
    // Use the normal foreground for the overflow count, independently of warning drives.
    const COLORREF overflowRgb = (contrast.dwFlags & HCF_HIGHCONTRASTON) ? GetSysColor(COLOR_WINDOWTEXT) :
        g_ui.widget.ActualTheme() == ElementTheme::Light ? RGB(24,24,24) : RGB(245,245,245);
    g_ui.overflowText.Foreground(SolidColorBrush(Color{255,GetRValue(overflowRgb),GetGValue(overflowRgb),GetBValue(overflowRgb)}));
    for (size_t i = 0; i < g_ui.driveCells.size(); ++i) {
        auto& cell = g_ui.driveCells[i];
        const auto& disk = reading.drives[i];
        COLORREF cellRgb = (contrast.dwFlags & HCF_HIGHCONTRASTON) ? GetSysColor(COLOR_WINDOWTEXT) :
            g_ui.widget.ActualTheme() == ElementTheme::Light ? RGB(24,24,24) : RGB(245,245,245);
        if (disk.lowSpace && !(contrast.dwFlags & HCF_HIGHCONTRASTON))
            cellRgb = g_ui.widget.ActualTheme() == ElementTheme::Light ? RGB(180,30,30) : RGB(255,125,125);
        Color cellColor{255,GetRValue(cellRgb),GetGValue(cellRgb),GetBValue(cellRgb)};
        cell.title.Foreground(SolidColorBrush(cellColor));
        cell.capacity.Foreground(SolidColorBrush(cellColor));
        if (contrast.dwFlags & HCF_HIGHCONTRASTON) {
            cell.bar.Visibility(Visibility::Collapsed);
            cell.panel.Background(nullptr);
        }
    }
    auto accessible = reading.title + L". " + reading.capacity;
    Automation::AutomationProperties::SetName(g_ui.surface, accessible);
    Automation::AutomationProperties::SetHelpText(
        g_ui.surface, UiText(IsRussianUi(), L"Click to choose a local drive",
                             L"Нажмите для выбора локального диска"));
    return true;
}

struct Dispatch {
    HWND window;
    const Settings* settings;
    const Reading* reading;
    bool remove;
    UiState* state;
    HANDLE completed;
    bool invoked = false;
    bool previousThreadExited = false;
};
std::atomic<Dispatch*> g_pending{nullptr};

LRESULT CALLBACK DispatchHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto message = reinterpret_cast<const CWPSTRUCT*>(lParam);
        Dispatch* call = g_pending.load();
        // Never dereference pointers supplied by an arbitrary window message.
        if (call && message->message == g_dispatchMessage && message->hwnd == call->window &&
            message->lParam == reinterpret_cast<LPARAM>(call) &&
            g_pending.compare_exchange_strong(call, nullptr)) {
            UiScope scope(call->state);
            try {
                if (call->remove) {
                    RemoveUi();
                    call->invoked = true;
                } else {
                    call->invoked = UpdateUi(call->window, *call->settings,
                                             *call->reading, call->previousThreadExited);
                }
            } catch (...) {
                Wh_Log(L"Taskbar update failed: %08X", static_cast<unsigned>(winrt::to_hresult()));
                // Never release the previous thread's XAML references here.
                if (!g_ui.thread || g_ui.thread == GetCurrentThreadId()) RemoveUi();
            }
            SetEvent(call->completed);
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool RunOnTaskbar(Dispatch& call, bool blocking = false) {
    DWORD process = 0;
    DWORD thread = GetWindowThreadProcessId(call.window, &process);
    if (!thread || process != GetCurrentProcessId()) return false;
    call.completed = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!call.completed) return false;
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, DispatchHook, nullptr, thread);
    if (!hook) {
        CloseHandle(call.completed);
        return false;
    }
    g_pending.store(&call);
    // A hung taskbar must not prevent the worker from observing g_stop. If the
    // hook has already claimed the call, wait for it before freeing stack data
    // or unloading the hook code. Otherwise a late message cannot use the call.
    if (blocking) {
        // Final teardown must finish revoking handlers before the mod unloads.
        SendMessageW(call.window, g_dispatchMessage, 0,
                     reinterpret_cast<LPARAM>(&call));
    } else {
        DWORD_PTR result = 0;
        SendMessageTimeoutW(call.window, g_dispatchMessage, 0,
                            reinterpret_cast<LPARAM>(&call),
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &result);
    }
    Dispatch* expected = &call;
    if (!g_pending.compare_exchange_strong(expected, nullptr)) {
        WaitForSingleObject(call.completed, INFINITE);
        // The event can be set just before DispatchHook returns. A second
        // synchronous message confirms the taskbar thread has left that hook.
        SendMessageW(call.window, WM_NULL, 0, 0);
    }
    UnhookWindowsHookEx(hook);
    CloseHandle(call.completed);
    return call.invoked;
}

void PublishMonitors() {
    const auto monitors = DetectMonitors();
    {
        std::lock_guard lock(g_menuCacheMutex);
        g_cachedMonitors = monitors;
    }
    if (monitors.empty()) return;
    {
        std::lock_guard lock(g_settingsMutex);
        // Upgrade a previously selected GDI name while its monitor is present.
        for (const auto& monitor : monitors) {
            if (_wcsicmp(g_settings.displayOn.c_str(), monitor.device.c_str()) == 0 &&
                _wcsicmp(monitor.id.c_str(), monitor.device.c_str()) != 0) {
                g_settings.displayOn = monitor.id;
                Wh_SetStringValue(L"SelectedMonitor", monitor.id.c_str());
                break;
            }
        }
    }
    const auto previous = LocalStringValue(L"PublishedMonitorDevices");
    size_t offset = 0;
    while (offset < previous.size()) {
        const auto end = previous.find(L'|', offset);
        const auto device = previous.substr(offset, end == std::wstring::npos ? end : end - offset);
        if (!device.empty() && std::none_of(monitors.begin(), monitors.end(),
                [&](const auto& monitor) { return monitor.id == device; }))
            Wh_DeleteValue((L"::wh_select_option::DisplayOn::" + device).c_str());
        if (end == std::wstring::npos) break;
        offset = end + 1;
    }
    std::wstring devices;
    for (const auto& monitor : monitors) {
        const auto key = L"::wh_select_option::DisplayOn::" + monitor.id;
        if (LocalStringValue(key.c_str()) != monitor.label)
            Wh_SetStringValue(key.c_str(), monitor.label.c_str());
        devices += monitor.id + L"|";
    }
    if (previous != devices) Wh_SetStringValue(L"PublishedMonitorDevices", devices.c_str());

}

struct TaskbarWindow {
    HWND window;
    DWORD thread;
    std::wstring device;
    bool primary;
};

bool IsSelectedTaskbar(const Settings& settings, const TaskbarWindow& window) {
    return settings.displayOn == L"all" ||
        (settings.displayOn == L"primary" && window.primary) ||
        _wcsicmp(settings.displayOn.c_str(), window.device.c_str()) == 0;
}

std::vector<TaskbarWindow> TaskbarWindows() {
    std::vector<TaskbarWindow> windows;
    EnumWindows([](HWND window, LPARAM context) -> BOOL {
        DWORD process = 0;
        const DWORD thread = GetWindowThreadProcessId(window, &process);
        if (!thread || process != GetCurrentProcessId()) return TRUE;
        wchar_t name[64]{};
        if (!GetClassNameW(window, name, ARRAYSIZE(name))) return TRUE;
        const bool primary = wcscmp(name, L"Shell_TrayWnd") == 0;
        if (!primary && wcscmp(name, L"Shell_SecondaryTrayWnd") != 0) return TRUE;
        MONITORINFOEXW monitor{};
        monitor.cbSize = sizeof(monitor);
        if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor))
            return TRUE;
        reinterpret_cast<std::vector<TaskbarWindow>*>(context)->push_back(
            {window, thread, monitor.szDevice, primary});
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));
    return windows;
}

HWND WindowOnThread(DWORD thread) {
    if (!thread) return nullptr;
    HWND result = nullptr;
    EnumThreadWindows(thread, [](HWND window, LPARAM context) -> BOOL {
        *reinterpret_cast<HWND*>(context) = window;
        return FALSE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

struct TaskbarSlot {
    HWND window;
    DWORD thread;
    HANDLE threadHandle;
    UiState* state;
};

bool RemoveSlot(TaskbarSlot& slot, bool blocking = false) {
    if (WaitForSingleObject(slot.threadHandle, 0) == WAIT_OBJECT_0) {
        // Never release apartment-bound XAML objects after their owner exits.
        // The state is retired just as in the former single-taskbar path.
        Wh_Log(L"Retiring disk indicator state from exited taskbar thread %u", slot.thread);
        CloseHandle(slot.threadHandle);
        return true;
    }
    HWND window = IsWindow(slot.window) &&
        GetWindowThreadProcessId(slot.window, nullptr) == slot.thread ?
        slot.window : WindowOnThread(slot.thread);
    if (!window) return false;
    Dispatch remove{window, nullptr, nullptr, true, slot.state};
    if (!RunOnTaskbar(remove, blocking)) return false;
    // RemoveUi has already released all XAML references on the owner thread.
    delete slot.state;
    CloseHandle(slot.threadHandle);
    return true;
}

DWORD WINAPI Worker(void*) {
    SetThreadErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX, nullptr);
    std::vector<TaskbarSlot> slots;
    std::vector<TaskbarWindow> knownWindows;
    constexpr ULONGLONG uiProbeInterval = 5000;
    ULONGLONG nextRead = 0, nextEnumeration = 0, nextUiProbe = 0;
    ULONGLONG nextMonitorCheck = 0, nextDriveCheck = 0;
    DWORD lastDriveMask = 0;
    Reading reading;
    bool uiDirty = true;
    std::array<std::optional<std::wstring>, 26> published;
    HANDLE events[] = {g_stop, g_changed};
    try {
        while (WaitForSingleObject(g_stop, 0) != WAIT_OBJECT_0) {
            const auto settings = CurrentSettings();
            const auto now = GetTickCount64();
            if (now >= nextDriveCheck) {
                const DWORD mask = GetLogicalDrives();
                if (mask && mask != lastDriveMask) {
                    lastDriveMask = mask;
                    nextEnumeration = 0;
                    nextRead = 0;
                    uiDirty = true;
                }
                nextDriveCheck = now + 2000;
            }
            if (now >= nextEnumeration) {
                const auto previous = CachedDrives();
                PublishDrives(published, IsRussianUi());
                const auto current = CachedDrives();
                if (settings.showAllDrives && (previous.size() != current.size() ||
                    !std::equal(previous.begin(), previous.end(), current.begin(),
                        [](const auto& a, const auto& b) {
                            return a.drive == b.drive && a.label == b.label && a.external == b.external;
                        }))) nextRead = 0;
                nextEnumeration = now + 30000;
            }
            if (now >= nextRead) {
                reading = ReadDrives(settings, IsRussianUi());
                nextRead = now + settings.interval * 1000ULL;
                uiDirty = true;
            }
            if (now >= nextUiProbe) {
                uiDirty = true;
                nextUiProbe = now + uiProbeInterval;
            }
            if (uiDirty) {
                auto windows = TaskbarWindows();
                const bool topologyChanged = windows.size() != knownWindows.size() ||
                    !std::equal(windows.begin(), windows.end(), knownWindows.begin(),
                        [](const auto& a, const auto& b) {
                            return a.window == b.window && a.thread == b.thread &&
                                   a.device == b.device && a.primary == b.primary;
                        });
                // Taskbar topology changes trigger immediate display discovery.
                // A slower fallback covers driver/name changes with unchanged HWNDs.
                if (topologyChanged || now >= nextMonitorCheck) {
                    PublishMonitors();
                    nextMonitorCheck = now + 60000;
                }
                if (topologyChanged) {
                    for (const auto& window : windows)
                        Wh_Log(L"Available taskbar: %s (%s)", window.device.c_str(),
                               window.primary ? L"primary" : L"secondary");
                    knownWindows = windows;
                }
                Settings placementSettings = settings;
                placementSettings.displayOn = ResolveMonitorDevice(
                    CurrentSettings().displayOn, CachedMonitors());
                std::vector<TaskbarWindow> selected;
                for (const auto& window : windows) {
                    if (IsSelectedTaskbar(placementSettings, window)) selected.push_back(window);
                }
                // Remove panels that disappeared or are no longer selected.
                for (size_t i = 0; i < slots.size();) {
                    const auto& slot = slots[i];
                    const bool retained = WaitForSingleObject(slot.threadHandle, 0) == WAIT_TIMEOUT &&
                        std::any_of(selected.begin(), selected.end(), [&](const auto& window) {
                            return window.window == slot.window && window.thread == slot.thread;
                        });
                    if (!retained && RemoveSlot(slots[i])) slots.erase(slots.begin() + i);
                    else ++i;
                }
                bool allUpdated = true;
                for (const auto& window : selected) {
                    auto slot = std::find_if(slots.begin(), slots.end(), [&](const auto& candidate) {
                        return candidate.window == window.window && candidate.thread == window.thread;
                    });
                    if (slot == slots.end()) {
                        HANDLE threadHandle = OpenThread(SYNCHRONIZE, FALSE, window.thread);
                        if (!threadHandle) { allUpdated = false; continue; }
                        slots.push_back({window.window, window.thread, threadHandle, new UiState});
                        slot = slots.end() - 1;
                        Wh_Log(L"Disk indicator taskbar device: %s (%s)", window.device.c_str(),
                               window.primary ? L"primary" : L"secondary");
                    }
                    Dispatch call{window.window, &settings, &reading, false, slot->state};
                    if (!RunOnTaskbar(call)) allUpdated = false;
                }
                uiDirty = !allUpdated;
            }
            const auto nextDue = std::min(nextDriveCheck, std::min(nextRead, std::min(nextEnumeration, nextUiProbe)));
            const DWORD timeout = nextDue > now ?
                static_cast<DWORD>(std::min<ULONGLONG>(5000, nextDue - now)) : 0;
            const DWORD wait = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, timeout);
            if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) break;
            if (wait == WAIT_OBJECT_0 + 1) {
                nextRead = 0;
                uiDirty = true;
            }
        }
    } catch (...) {
        Wh_Log(L"Disk space worker failed: %08X", static_cast<unsigned>(winrt::to_hresult()));
    }
    for (auto& slot : slots) {
        if (!RemoveSlot(slot, true)) {
            Wh_Log(L"Taskbar teardown dispatch failed for thread %u", slot.thread);
            CloseHandle(slot.threadHandle);
        }
    }
    return 0;
}

bool ResolveTaskbar() {
    g_taskbarModule = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_taskbarModule) return false;
    // taskbar.dll
    WindhawkUtils::SYMBOL_HOOK taskbarSymbols[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_siteVtable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_secondarySiteVtable, nullptr, true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"}, &g_getSecondaryHost, nullptr, true},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &g_getHost},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &g_frameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &g_decref},
    };
    if (!WindhawkUtils::HookSymbols(g_taskbarModule, taskbarSymbols,
                                    ARRAYSIZE(taskbarSymbols))) return false;
    // Validate the implementation before extracting the FrameworkElement field.
#if defined(_M_X64)
    const BYTE* code = reinterpret_cast<const BYTE*>(g_frameHeight);
    if (code[0] != 0x48 || code[1] != 0x83 || code[2] != 0xEC ||
        code[4] != 0x48 || code[5] != 0x83 || code[6] != 0xC1 || code[7] > 0x7F) return false;
    g_elementOffset = code[7];
#elif defined(_M_ARM64)
    const DWORD* code = reinterpret_cast<const DWORD*>(g_frameHeight);
    if (code[0] != 0xD503237F || (code[1] & 0xFFC07FFF) != 0xA9807BFD ||
        code[2] != 0x910003FD || (code[3] & 0xFFF00FE0) != 0xF8400C00) return false;
    g_elementOffset = (code[3] >> 12) & 0xFF;
#else
#error Unsupported architecture
#endif
    return g_elementOffset != 0;
}

void ReleaseHandles() {
    if (g_worker) { CloseHandle(g_worker); g_worker = nullptr; }
    if (g_stop) { CloseHandle(g_stop); g_stop = nullptr; }
    if (g_changed) { CloseHandle(g_changed); g_changed = nullptr; }
    if (g_taskbarModule) { FreeLibrary(g_taskbarModule); g_taskbarModule = nullptr; }
}
} // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    if (!ResolveTaskbar()) {
        Wh_Log(L"Unsupported taskbar or unavailable symbols; Windows 11 22H2+ native taskbar required");
        ReleaseHandles();
        return FALSE;
    }
    g_dispatchMessage = RegisterWindowMessageW(L"Windhawk.TaskbarDiskSpace.Dispatch");
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_changed = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_dispatchMessage || !g_stop || !g_changed) {
        ReleaseHandles();
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    g_worker = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_worker) Wh_Log(L"Cannot start disk space worker: %u", GetLastError());
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_changed) SetEvent(g_changed);
}

void Wh_ModBeforeUninit() {
    if (g_stop) SetEvent(g_stop);
    if (g_worker) WaitForSingleObject(g_worker, INFINITE);
}

void Wh_ModUninit() {
    ReleaseHandles();
    std::lock_guard lock(g_settingsMutex);
    Settings empty;
    std::swap(g_settings, empty);
}
