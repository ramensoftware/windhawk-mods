// ==WindhawkMod==
// @id              taskbar-disk-space
// @name            Taskbar Disk Space
// @description     Disk space on the Windows 11 taskbar with drive selection, color themes and system shortcuts.
// @description:ru-RU Место на диске на панели задач Windows 11: выбор диска, цветовые темы и системные команды.
// @version         0.12.25
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

- **Left-click:** choose a local fixed drive.
- **Right-click → Theme:** choose System (neutral hover), Color bar (default), or Text only (no hover background). Selection persists.
- **Right-click → Position:** choose Automatic, Left or Right. The choice applies immediately and persists; changing position in settings overrides the menu choice.
- **Right-click → Monitor:** immediately select the primary taskbar, all taskbars or a detected display. Selection persists; changing the monitor setting overrides the menu choice.
- **Mini design → Automatic mini design:** enabled by default; tries the compact layout when it is narrower and the normal layout does not fit. Manual Mini design takes priority.
- **Low disk space:** choose Disabled, 5/10/15/20 %, or 5/10/20/50 GiB; custom thresholds remain available in settings.
- **Reset appearance:** restore Color bar, green/red, Free / Total, one decimal place, normal layout and automatic compact mode; keep drive, monitor, position and warning threshold.
- **Apply Windhawk settings:** discard menu overrides and apply the settings page; menu-only parameters return to defaults. Menu selections do not rewrite settings-page fields; menu checkmarks show effective values.
- **Right-click → Precision:** choose 0, 1 (default) or 2 decimal places for GiB and percentages. Values are rounded; selection persists.
- **Right-click → Format:** choose Free / Total, Used / Total, or Free, %. The selection persists and works with Mini design. Used space is total minus space available to the current user, including quotas.
- **Right-click → Colors:** select one of ten palettes for Color bar; disabled in the other themes. Mini design works with every theme.
- **Right-click → Mini design → Hide drive name:** toggle the compact single-line layout with the drive name hidden. The choice persists; changing Hide drive name in settings overrides it.
- **Right-click → Open selected drive:** open the current drive in File Explorer.
- **Right-click → This PC:** open File Explorer at This PC.
- **Right-click → Disk Management:** open the system console with administrator elevation.
- **Right-click → Refresh now:** reread disk space without waiting for the refresh interval.

Drive and theme selections persist across Explorer restarts. Changing the initial
drive or color scheme in Windhawk settings overrides the corresponding menu choice.
The hover surface follows the Start button background height and corner radius;
width follows the text and space available between native buttons and the tray. A 12-DIP gap is kept; below 72 DIPs the indicator is temporarily hidden. Placement is rechecked at least every five seconds. Hover and pressed transitions respect
Windows animation settings. No tooltip is shown.

Low disk space warning settings offer Disabled (default), Percent, or GiB, with an integer threshold (default 10). At or below the threshold, text turns red in every appearance mode, including Mini design. Refresh now also checks the threshold. High-contrast text colors are preserved; zero or unavailable capacity does not trigger the warning.

Monitor selection for display is a single dropdown offering Primary taskbar, All taskbars and automatically detected displays with model names when available, falling back to device labels. Resolution is not shown. Options refresh every five seconds; reopen the dropdown after a connection change. Windows must show a taskbar on the selected display. A disconnected selection is retained. Menus and hover states remain independent. Windhawk 1.7.3 uses a text field (`primary`, `all`, or a device name such as `\\.\DISPLAY2`) because dynamic lists require Windhawk 2.0.

Refresh defaults to 10 minutes. Placement options are Automatic (configurable left
offset), Left (15 DIPs), and Right (before the system tray). Requires Windhawk 1.7.3
or 2.0 and the native bottom taskbar on Windows 11 22H2+. ExplorerPatcher and
StartAllBack are unsupported. Only fixed local drives are listed; capacity checks
read volume metadata without writing user data.

**Recent changes:** 0.12.25 waits for final UI teardown before unloading and removes redundant monitor-label writes; 0.12.24 makes English the default description and restores the separate-mod rationale; 0.12.23 adds four current screenshots; 0.12.22 groups compact controls in Mini design and adds icons to remaining menu items; 0.12.21 adds automatic compact layout, warning presets, reset/apply actions and menu icons. 0.12.20 adds precision; 0.12.19 adds Position; 0.12.18 improves placement; 0.12.17 adds monitor model names.

## Screenshots

[![Taskbar Disk Space — скриншот 1](https://i.imgur.com/QsiHo0m.png)](https://imgur.com/QsiHo0m)

[![Taskbar Disk Space — скриншот 2](https://i.imgur.com/VyaAfGR.png)](https://imgur.com/VyaAfGR)

[![Taskbar Disk Space — скриншот 3](https://i.imgur.com/fHFa648.png)](https://imgur.com/fHFa648)

[![Taskbar Disk Space — скриншот 4](https://i.imgur.com/zqPRJ9V.png)](https://imgur.com/zqPRJ9V)

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

## Управление с панели задач

| Действие | Результат |
|---|---|
| Левая кнопка мыши | Список доступных фиксированных локальных дисков |
| Правая кнопка → **Тема** | Системная, Цветная полоса или Только текст |
| Правая кнопка → **Цвета** | Десять палитр для темы «Цветная полоса» |
| Правая кнопка → **Положение** | Автоматически, Слева или Справа; выбор применяется сразу и сохраняется |
| Правая кнопка → **Монитор** | Основная панель, все панели или обнаруженный экран; применяется сразу |
| Правая кнопка → **Нехватка места** | Выключение и готовые пороги в процентах или ГиБ |
| Правая кнопка → **Сбросить оформление** | Возвращает внешний вид по умолчанию |
| Правая кнопка → **Применить настройки Windhawk** | Возвращает значения страницы настроек |
| Правая кнопка → **Точность** | 0, 1 или 2 знака после запятой для ГиБ и процентов; выбор сохраняется |
| Правая кнопка → **Формат** | Свободно / Всего, Занято / Всего или Свободно, %; выбор сохраняется |
| Правая кнопка → **Мини дизайн → Скрывать имя диска** | Скрывает имя диска и включает одну компактную строку; повторное нажатие возвращает две строки |
| Правая кнопка → **Открыть выбранный диск** | Открывает выбранный в индикаторе диск в Проводнике |
| Правая кнопка → **Этот компьютер** | Открывает раздел Проводника со списком дисков |
| Правая кнопка → **Управление дисками** | Открывает системную консоль с запросом прав администратора |
| Правая кнопка → **Обновить сейчас** | Перечитывает свободное и общее место без ожидания интервала |

Выбранные диск и тема сохраняются после перезапуска Explorer. Изменение начального
диска или цветовой схемы в настройках Windhawk заменяет соответствующий выбор из меню.
«Мини дизайн» сохраняется после перезапуска Explorer. Изменение настройки «Скрывать имя диска» заменяет выбор из меню. Остальные настройки сохраняют выбор.

## Рамка и цветовые темы

В меню «Тема» доступны **Системная** (нейтральная подсветка), **Цветная полоса** (свободное/занятое место, включена по умолчанию) и **Только текст** (без подсветки). Выбор сохраняется после перезапуска Explorer. Подменю «Цвета» доступно только для цветной полосы; другие темы сохраняют выбранную палитру. Мини дизайн работает с любой темой.

При наведении появляется закруглённая полупрозрачная рамка. Её высота и скругление
берутся у подсветки кнопки **«Пуск»**, ширина автоматически подстраивается под текст
и доступное место. Длинный текст сокращается с многоточием. Доступная ширина учитывает кнопки панели и трей с зазором 12 логических единиц. Если остаётся менее 72 единиц, индикатор временно скрывается и возвращается после освобождения места. Расчёт повторяется при обновлении интерфейса, не реже раза в пять секунд.

Подсветка плавно меняется при наведении и нажатии; анимации учитывают системную
настройку Windows. Всплывающая подсказка не используется.

Цветная полоса показывает долю свободного и занятого места. По умолчанию свободное
место зелёное, занятое — красное. Доступны также синий/оранжевый,
бирюзовый/фиолетовый, фиолетовый/жёлтый, бирюзовый/розовый, лаймовый/индиго,
янтарный/тёмно-синий, мятный/коралловый, небесный/пурпурный и белый/серый.
Цвет текста учитывает светлую, тёмную и высококонтрастную темы Windows.

## Быстрые настройки и восстановление

- **Мини дизайн → Авто мини дизайн** включён по умолчанию. Когда обычный вид не помещается,
  индикатор пробует компактную строку и использует её только если она уже обычного
  вида. После освобождения места возвращается прежний вид. Ручной «Мини дизайн»
  остаётся приоритетным; если и компактному виду не хватает места, текст сокращается
  или индикатор временно скрывается.
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
- **Автоматический** режим: слева с настраиваемым отступом.
- **Слева**: фиксированный отступ 15 логических единиц.
- **Справа**: непосредственно перед системным треем.

При левом выравнивании кнопок панели режимы слева резервируют место под индикатор.
При центральном выравнивании кнопки не сдвигаются. Размеры учитывают масштаб Windows.

## Несколько мониторов

Подменю **«Монитор»** по правой кнопке показывает актуальные экраны, основную панель и все панели. Нажатие сразу применяет и сохраняет выбор; текущий вариант отмечен галочкой. Работает в Windhawk 1.7.3 и 2.0. Изменение выбора в настройках заменяет выбор из меню, остальные настройки его сохраняют.

Один пункт **«Выбор монитора для отображения»** предлагает основную панель, все панели и автоматически обнаруженные мониторы с реальным названием модели, если Windows его предоставляет; иначе используется прежняя подпись устройства. Список обновляется раз в пять секунд; после подключения или отключения экрана переоткройте выпадающее меню. Ручной ввод имени устройства в Windhawk 2.0 не нужен. Windows должна показывать панель задач на выбранном мониторе. Если выбранный экран отключён, индикатор не отображается до его возвращения. Диск, формат и оформление общие; меню и анимации независимы. На дополнительной панели без трея режим «Справа» использует отступ 15 логических единиц от правого края. В Windhawk 1.7.3 динамический список недоступен: это текстовое поле со значениями `primary`, `all` или именем устройства, например `\\.\DISPLAY2`.

## Совместимость

Windhawk **1.7.3 и 2.0**, штатная нижняя горизонтальная панель задач
**Windows 11 22H2 и новее**. ExplorerPatcher и StartAllBack не поддерживаются.
В Windhawk 2.0 список дисков в настройках дополнительно показывает метки томов.
Меню индикатора показывает метки томов в обеих версиях.

В меню доступны только фиксированные локальные диски. Сетевые ресурсы и съёмные
флеш-накопители не перечисляются. Проверка ёмкости читает сведения о томе через
`GetDiskFreeSpaceExW` и не записывает пользовательские данные на диск.

## Изменения

- **0.12.25:** при выгрузке ожидается удаление интерфейса и обработчиков; убраны повторные записи статических подписей мониторов.

- **0.12.24:** английское описание размещено первым; восстановлено пояснение о назначении отдельного мода.

- **0.12.23:** добавлены четыре актуальных скриншота в описание и README.
- **0.12.22:** ручной и автоматический компактный режимы объединены в подменю «Мини дизайн»; добавлены иконки остальных пунктов меню.
- **0.12.21:** автоматический компактный вид, быстрые пороги, сброс оформления, применение настроек Windhawk и иконки меню; добавлены регрессионные проверки.
- **0.12.20:** добавлен выбор точности чисел: 0, 1 или 2 знака после запятой.
- **0.12.19:** добавлен выбор положения через меню правой кнопки с сохранением выбора.

Полная история версий сохраняется в README проекта.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
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
  $description: Preset colors for free and used space on hover.
  $description:ru-RU: Заранее заготовленная пара цветов для свободного и занятого места при наведении.
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
  $description: Used only in Automatic mode. Left is fixed at 15; Right is placed beside the system tray.
  $description:ru-RU: Действует только в автоматическом режиме. Слева отступ фиксирован — 15; справа индикатор располагается рядом с треем.
  #! $min: 0
  #! $max: 1200
  #! $format: slider
- ReserveSpace: "auto"
  $name: Position relative to Start
  $name:ru-RU: Положение относительно пуска
  $description: Automatic keeps the current placement. Left uses a fixed 15-pixel offset; Right places the indicator beside the system tray.
  $description:ru-RU: Автоматически сохраняет прежнее размещение. Слева — фиксированный отступ 15 пикселей; справа — рядом с треем.
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
    bool autoCompact = true;
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

    std::wstring title;
    std::wstring capacity;
    double freeRatio = 0.0;
    bool hasRatio = false;
    bool lowSpace = false;
    std::wstring compactTitle;
    std::wstring compactCapacity;
};
// XAML references must be released on their owning UI thread, never by static
// destructors during Explorer process shutdown.
struct UiState {
    Grid root{nullptr};
    Border surface{nullptr};
    LinearGradientBrush hoverBrush{nullptr};
    GradientStop freeStop{nullptr};
    GradientStop freeEndStop{nullptr};
    GradientStop usedStartStop{nullptr};
    GradientStop usedStop{nullptr};
    MenuFlyout driveMenu{nullptr};
    StackPanel widget{nullptr};
    TextBlock title{nullptr};
    TextBlock capacity{nullptr};
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

void LoadSettings() {
    Settings settings;
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
    settings.autoCompact = LocalStringValue(L"SelectedAutoCompact") != L"0";
    const auto precision = LocalStringValue(L"SelectedPrecision");
    if (precision == L"0" || precision == L"1" || precision == L"2")
        settings.precision = precision[0] - L'0';
    const auto format = LocalStringValue(L"SelectedFormat");
    if (format == L"free" || format == L"used" || format == L"percent")
        settings.format = format;
    const auto appearance = LocalStringValue(L"SelectedAppearance");
    if (appearance == L"system" || appearance == L"bar" || appearance == L"text")
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
    const bool fixed = GetDriveTypeW(root.c_str()) == DRIVE_FIXED;
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

std::vector<std::wstring> FixedDrives() {
    std::vector<std::wstring> drives;
    const DWORD mask = GetLogicalDrives();
    if (!mask) return drives;
    for (int i = 0; i < 26; ++i) {
        std::wstring drive{static_cast<wchar_t>(L'A' + i), L':'};
        if ((mask & (1u << i)) &&
            GetDriveTypeW((drive + L"\\").c_str()) == DRIVE_FIXED) {
            drives.push_back(std::move(drive));
        }
    }
    return drives;
}

void SelectDrive(const std::wstring& drive) {
    const auto drives = FixedDrives();
    if (std::find(drives.begin(), drives.end(), drive) == drives.end()) return;
    Wh_SetStringValue(kSelectedDriveValue, drive.c_str());
    {
        std::lock_guard lock(g_settingsMutex);
        g_settings.drive = drive;
    }
    if (g_changed) SetEvent(g_changed);
    Wh_Log(L"Selected drive changed from the taskbar to %s", drive.c_str());
}

FontIcon MenuIcon(PCWSTR glyph);

void ShowDriveMenu() {
    if (!g_ui.surface) return;
    const bool russian = IsRussianUi();
    const auto drives = FixedDrives();
    const Settings settings = CurrentSettings();
    MenuFlyout menu;
    for (const auto& drive : drives) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE8B7"));
        std::wstring label = VolumeName(drive + L"\\");
        if (label.empty()) label = UiText(russian, L"Local Disk", L"Локальный диск");
        std::wstring text = (drive == settings.drive ? L"✓ " : L"  ") +
                            label + L" (" + drive + L")";
        item.Text(text);
        item.Click([drive](winrt::Windows::Foundation::IInspectable const&,
                           winrt::Windows::UI::Xaml::RoutedEventArgs const&) {
            SelectDrive(drive);
        });
        menu.Items().Append(item);
    }
    if (drives.empty()) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE8B7"));
        item.Text(UiText(russian, L"No local fixed drives available",
                         L"Нет доступных локальных дисков"));
        item.IsEnabled(false);
        menu.Items().Append(item);
    }
    // MenuFlyout is a native WinUI control, so its open/close animation,
    // shadows, corner radius and theme colors follow the taskbar automatically.
    g_ui.driveMenu = menu;
    g_ui.driveMenu.ShowAt(g_ui.surface);
}

struct MonitorOption { std::wstring device; std::wstring label; };

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
                DisplayConfigGetDeviceInfo(&target.header) != ERROR_SUCCESS ||
                !target.monitorFriendlyDeviceName[0]) continue;
            auto existing = std::find_if(names.begin(), names.end(), [&](const auto& name) {
                return _wcsicmp(name.device.c_str(), source.viewGdiDeviceName) == 0;
            });
            if (existing == names.end())
                names.push_back({source.viewGdiDeviceName, target.monitorFriendlyDeviceName});
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
        if (name != friendlyNames.end()) monitor.label = name->label + L" (" + monitor.device + L")";
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
        g_settings.autoCompact = true;
    }
    if (g_changed) SetEvent(g_changed);
}

void ApplyConfiguredSettings() {
    constexpr PCWSTR values[] = {
        L"SelectedDrive", L"SelectedTheme", L"SelectedMiniDesign", L"SelectedAppearance",
        L"SelectedFormat", L"SelectedPrecision", L"SelectedMonitor", L"SelectedPosition",
        L"SelectedLowSpace", L"SelectedAutoCompact"
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
    themeMenu.Text(UiText(russian, L"Theme", L"Тема"));
    constexpr ThemeOption appearances[] = {
        {L"system", L"System", L"Системная"},
        {L"bar", L"Color bar", L"Цветная полоса"},
        {L"text", L"Text only", L"Только текст"},
    };
    for (const auto& appearance : appearances) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE790"));
        std::wstring label = settings.appearance == appearance.id ? L"✓ " : L"  ";
        label += russian ? appearance.russian : appearance.english;
        item.Text(label);
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
    colorMenu.IsEnabled(settings.appearance == L"bar");
    for (const auto& theme : themes) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE790"));
        std::wstring label = settings.colorScheme == theme.id ? L"✓ " : L"  ";
        label += russian ? theme.russian : theme.english;
        item.Text(label);
        item.Click([id = std::wstring(theme.id)](auto const&, auto const&) {
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
    constexpr PCWSTR russianLabels[] = {L"Без дробной части", L"1 знак после запятой", L"2 знака после запятой"};
    constexpr PCWSTR englishLabels[] = {L"No decimal places", L"1 decimal place", L"2 decimal places"};
    for (int precision = 0; precision <= 2; ++precision) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE8EF"));
        std::wstring label = settings.precision == precision ? L"✓ " : L"  ";
        label += russian ? russianLabels[precision] : englishLabels[precision];
        item.Text(label);
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
    auto appendMonitor = [&](const std::wstring& id, const std::wstring& label) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE7F4"));
        item.Text((settings.displayOn == id ? L"✓ " : L"  ") + label);
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
    for (const auto& monitor : DetectMonitors()) appendMonitor(monitor.device, monitor.label);
    monitorMenu.Icon(MenuIcon(L"\uE7F4"));
    menu.Items().Append(monitorMenu);
    MenuFlyoutSubItem positionMenu;
    positionMenu.Text(UiText(russian, L"Position", L"Положение"));
    constexpr ThemeOption positions[] = {
        {L"auto", L"Automatic", L"Автоматически"},
        {L"left", L"Left", L"Слева"},
        {L"right", L"Right", L"Справа"},
    };
    for (const auto& position : positions) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE8A9"));
        std::wstring label = settings.position == position.id ? L"✓ " : L"  ";
        label += russian ? position.russian : position.english;
        item.Text(label);
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
    miniMenu.Text(UiText(russian, L"Mini design", L"Мини дизайн"));
    ToggleMenuFlyoutItem miniItem;
    miniItem.Icon(MenuIcon(L"\uE8A7"));
    miniItem.Text(UiText(russian, L"Hide drive name", L"Скрывать имя диска"));
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
    autoMiniItem.Text(UiText(russian, L"Automatic mini design", L"Авто мини дизайн"));
    autoMiniItem.IsChecked(settings.autoCompact);
    autoMiniItem.Click([](auto const& sender, auto const&) {
        const bool enabled = sender.template as<ToggleMenuFlyoutItem>().IsChecked();
        Wh_SetStringValue(L"SelectedAutoCompact", enabled ? L"1" : L"0");
        {
            std::lock_guard lock(g_settingsMutex);
            g_settings.autoCompact = enabled;
        }
        if (g_changed) SetEvent(g_changed);
    });
    miniMenu.Items().Append(autoMiniItem);
    menu.Items().Append(miniMenu);
    MenuFlyoutSubItem lowSpaceMenu;
    lowSpaceMenu.Icon(MenuIcon(L"\uE7BA"));
    lowSpaceMenu.Text(UiText(russian, L"Low disk space", L"Нехватка места"));
    auto appendThreshold = [&](const std::wstring& mode, int threshold, const std::wstring& label) {
        MenuFlyoutItem item;
        item.Icon(MenuIcon(L"\uE7BA"));
        const bool selected = settings.lowSpaceMode == mode &&
            (mode == L"off" || settings.lowSpaceThreshold == threshold);
        item.Text((selected ? L"✓ " : L"  ") + label);
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
        current.IsEnabled(false);
        current.Text(UiText(russian, L"Current: ", L"Текущий порог: ") +
            std::to_wstring(settings.lowSpaceThreshold) +
            (settings.lowSpaceMode == L"percent" ? L" %" : (russian ? L" ГиБ" : L" GiB")));
        lowSpaceMenu.Items().Append(current);
    }
    menu.Items().Append(lowSpaceMenu);
    menu.Items().Append(MenuFlyoutSeparator());
    MenuFlyoutItem openDriveItem;
    openDriveItem.Text(UiText(russian, L"Open selected drive", L"Открыть выбранный диск"));
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
    refreshItem.Click([](auto const&, auto const&) {
        // Wake the worker, which resets nextRead and reads volume metadata.
        // Keep disk I/O off the taskbar UI thread.
        if (g_changed) SetEvent(g_changed);
    });
    refreshItem.Icon(MenuIcon(L"\uE72C"));
    menu.Items().Append(refreshItem);
    MenuFlyoutItem resetItem;
    resetItem.Text(UiText(russian, L"Reset appearance", L"Сбросить оформление"));
    resetItem.Icon(MenuIcon(L"\uE777"));
    resetItem.Click([](auto const&, auto const&) { ResetAppearance(); });
    menu.Items().Append(resetItem);
    MenuFlyoutItem configuredItem;
    configuredItem.Text(UiText(russian, L"Apply Windhawk settings", L"Применить настройки Windhawk"));
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

void ApplyHoverBackground() {
    if (!g_ui.surface) return;
    const bool light = g_ui.surface.ActualTheme() == ElementTheme::Light;
    Color freeColor{};
    Color usedColor{};
    {
        if (g_ui.hasRatio && g_ui.appearance == L"bar") {
            // Цветовая пара выбирается в настройках / The color pair is
            // selected in settings. Alpha stays deliberately low so text
            // remains the primary visual.
            const ColorPair pair = ColorScheme(g_ui.colorScheme);
            freeColor = pair.free;
            usedColor = pair.used;
        } else {
            // Match the quiet neutral hover surface used by taskbar buttons
            // when disk capacity is unavailable.
            Color color{};
            HIGHCONTRASTW contrast{sizeof(contrast)};
            SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0);
            if (contrast.dwFlags & HCF_HIGHCONTRASTON) {
                const COLORREF highlight = GetSysColor(COLOR_HIGHLIGHT);
                color = Color{110, GetRValue(highlight), GetGValue(highlight),
                               GetBValue(highlight)};
            } else {
                bool resourceColor = false;
                try {
                    auto resource = Application::Current().Resources().Lookup(
                        winrt::box_value(L"SystemControlHighlightListLowBrush"));
                    if (auto brush = resource.try_as<SolidColorBrush>()) {
                        color = brush.Color();
                        resourceColor = true;
                    }
                } catch (...) {
                    // Older taskbar builds may not expose this WinUI resource.
                }
                if (!resourceColor) {
                    color = light ? Color{24, 0, 0, 0} : Color{32, 255, 255, 255};
                }
            }
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
    if (g_ui.appearance == L"text" || (!g_ui.hovered && !g_ui.pressed)) {
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

void PublishDrives(std::array<std::optional<std::wstring>, 26>& published,
                  bool russian) {
    DWORD mask = GetLogicalDrives();
    if (!mask) return;  // Do not discard the previous list on enumeration error.
    for (int i = 0; i < 26; ++i) {
        std::wstring drive{static_cast<wchar_t>(L'A' + i), L':'};
        std::wstring key = L"::wh_select_option::Drive::" + drive;
        std::wstring root = drive + L"\\";
        std::wstring label;
        if ((mask & (1u << i)) && GetDriveTypeW(root.c_str()) == DRIVE_FIXED) {
            label = VolumeName(root);
            if (label.empty()) label = UiText(russian, L"Local Disk", L"Локальный диск");
            label += L" (" + drive + L")";
        }
        if (published[i].has_value() && published[i].value() == label) continue;
        if (label.empty()) Wh_DeleteValue(key.c_str());
        else Wh_SetStringValue(key.c_str(), label.c_str());
        published[i] = std::move(label);
    }
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
    return root ? FindFrame(root.Content()) : nullptr;
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

void MeasureTaskbarButtons(DependencyObject const& parent, Grid const& root,
                           ButtonBounds& bounds, int depth = 0) {
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
                bounds.left = std::min(bounds.left, static_cast<double>(rect.X));
                bounds.right = std::max(bounds.right, static_cast<double>(rect.X + rect.Width));
                bounds.found = true;
                return;
            } catch (...) { /* Layout can be rebuilding during a topology change. */ }
        }
    }
    const int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i)
        MeasureTaskbarButtons(VisualTreeHelper::GetChild(parent, i), root, bounds, depth + 1);
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

void RemoveUi() {
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

bool UpdateUi(HWND window, const Settings& settings, const Reading& reading,
              bool previousThreadExited) {
    int taskbarFrameHeight = 0;
    auto frame = TaskbarFrame(window, &taskbarFrameHeight);
    if (!frame) return false;
    auto rootElement = FindElement(frame, L"RootGrid");
    auto root = rootElement ? rootElement.try_as<Grid>() : nullptr;
    if (!root) return false;
    auto trayInset = settings.position == L"right" ? TrayInset(window) : std::optional<double>{};
    if (settings.position == L"right" && !trayInset) {
        wchar_t className[64]{};
        GetClassNameW(window, className, ARRAYSIZE(className));
        // A secondary panel can have no notification area; use its right edge.
        if (wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) trayInset = 15.0;
    }
    if (settings.position == L"right" && !trayInset) {
        if (g_ui.thread == GetCurrentThreadId()) RemoveUi();
        return false;
    }
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
    // TaskbarHost reports the whole taskbar height, not the visible hover frame.
    // Read the Start button background itself so taskbar customizations and DPI
    // scaling are reflected in the indicator without a separate size setting.
    double hoverHeight = taskbarFrameHeight > 8 ? taskbarFrameHeight - 8.0 : 40.0;
    auto startButton = FindStartButton(root);
    if (startButton) {
        auto background = FindElement(startButton, L"BackgroundElement");
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
    if (g_ui.surface.Height() != hoverHeight) g_ui.surface.Height(hoverHeight);
    g_ui.widget.Orientation(settings.hideDriveName ? Orientation::Horizontal :
                            Orientation::Vertical);
    g_ui.capacity.Margin(settings.hideDriveName ? Thickness{8, 0, 0, 0} :
                         Thickness{0, 0, 0, 0});
    // Let the Border measure its content so the hover frame does not extend
    // far beyond the text. Only the available taskbar space limits its width.
    const double leftOffset = settings.position == L"left" ? 15.0 :
                              settings.position == L"auto" ? settings.offset : 0.0;
    const double inset = trayInset.value_or(leftOffset);
    const bool reserveSpace = settings.position != L"right" && TaskbarIconsLeftAligned();
    ButtonBounds buttons;
    MeasureTaskbarButtons(root, root, buttons);
    double maxWidth = std::numeric_limits<double>::infinity();
    if (root.ActualWidth() > 0) {
        maxWidth = AvailableIndicatorWidth(root.ActualWidth(), inset,
            TrayInset(window).value_or(15.0), settings.position == L"right",
            reserveSpace, buttons);
    }
    // Hide rather than cover native buttons if even a short value will not fit.
    const bool fits = maxWidth >= 72.0;
    g_ui.surface.Visibility(Visibility::Visible);
    if (g_ui.surface.MaxWidth() != maxWidth) g_ui.surface.MaxWidth(maxWidth);
    if (!std::isnan(g_ui.surface.Width())) {
        g_ui.surface.Width(std::numeric_limits<double>::quiet_NaN());
    }
    auto setLayout = [&](bool compact) {
        g_ui.widget.Orientation(compact ? Orientation::Horizontal : Orientation::Vertical);
        g_ui.capacity.Margin(compact ? Thickness{8, 0, 0, 0} : Thickness{0, 0, 0, 0});
        const auto& title = compact && !reading.compactTitle.empty() ? reading.compactTitle : reading.title;
        const auto& capacity = compact && !reading.compactCapacity.empty() ? reading.compactCapacity : reading.capacity;
        if (g_ui.title.Text() != title) g_ui.title.Text(title);
        if (g_ui.capacity.Text() != capacity) g_ui.capacity.Text(capacity);
    };
    setLayout(settings.hideDriveName);
    if (!settings.hideDriveName && settings.autoCompact) {
        g_ui.surface.MaxWidth(std::numeric_limits<double>::infinity());
        const winrt::Windows::Foundation::Size unlimited{
            std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
        g_ui.surface.Measure(unlimited);
        const double normalWidth = g_ui.surface.DesiredSize().Width;
        if (normalWidth > maxWidth) {
            setLayout(true);
            g_ui.surface.Measure(unlimited);
            if (!ShouldUseCompact(false, true, normalWidth, g_ui.surface.DesiredSize().Width, maxWidth))
                setLayout(false);
        }
        g_ui.surface.MaxWidth(maxWidth);
    }
    // Measure the current text before reserving space for taskbar buttons.
    g_ui.surface.Measure(winrt::Windows::Foundation::Size{
        static_cast<float>(maxWidth), std::numeric_limits<float>::infinity()});
    const double width = fits ? g_ui.surface.DesiredSize().Width : 0.0;
    if (!fits) g_ui.surface.Visibility(Visibility::Collapsed);
    const auto alignment = settings.position == L"right" ?
        HorizontalAlignment::Right : HorizontalAlignment::Left;
    if (g_ui.surface.HorizontalAlignment() != alignment)
        g_ui.surface.HorizontalAlignment(alignment);
    Thickness placement = settings.position == L"right" ?
        Thickness{0, 0, inset, 0} : Thickness{inset, 0, 0, 0};
    if (g_ui.surface.Margin() != placement) g_ui.surface.Margin(placement);
    g_ui.freeRatio = reading.freeRatio;
    g_ui.hasRatio = reading.hasRatio;
    g_ui.colorScheme = settings.colorScheme;
    g_ui.appearance = settings.appearance;
    ApplyHoverBackground();
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
    if (monitors.empty()) return;
    const auto previous = LocalStringValue(L"PublishedMonitorDevices");
    size_t offset = 0;
    while (offset < previous.size()) {
        const auto end = previous.find(L'|', offset);
        const auto device = previous.substr(offset, end == std::wstring::npos ? end : end - offset);
        if (!device.empty() && std::none_of(monitors.begin(), monitors.end(),
                [&](const auto& monitor) { return monitor.device == device; }))
            Wh_DeleteValue((L"::wh_select_option::DisplayOn::" + device).c_str());
        if (end == std::wstring::npos) break;
        offset = end + 1;
    }
    std::wstring devices;
    for (const auto& monitor : monitors) {
        const auto key = L"::wh_select_option::DisplayOn::" + monitor.device;
        if (LocalStringValue(key.c_str()) != monitor.label)
            Wh_SetStringValue(key.c_str(), monitor.label.c_str());
        devices += monitor.device + L"|";
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
    Reading reading;
    bool uiDirty = true;
    std::array<std::optional<std::wstring>, 26> published;
    HANDLE events[] = {g_stop, g_changed};
    try {
        while (WaitForSingleObject(g_stop, 0) != WAIT_OBJECT_0) {
            const auto settings = CurrentSettings();
            const auto now = GetTickCount64();
            if (now >= nextRead) {
                reading = ReadDisk(settings, IsRussianUi());
                nextRead = now + settings.interval * 1000ULL;
                uiDirty = true;
            }
            if (now >= nextEnumeration) {
                PublishDrives(published, IsRussianUi());
                nextEnumeration = now + 30000;
            }
            if (now >= nextUiProbe) {
                uiDirty = true;
                PublishMonitors();
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
                if (topologyChanged) {
                    for (const auto& window : windows)
                        Wh_Log(L"Available taskbar: %s (%s)", window.device.c_str(),
                               window.primary ? L"primary" : L"secondary");
                    knownWindows = windows;
                }
                std::vector<TaskbarWindow> selected;
                for (const auto& window : windows) {
                    if (IsSelectedTaskbar(settings, window)) selected.push_back(window);
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
            const auto nextDue = std::min(nextRead, std::min(nextEnumeration, nextUiProbe));
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
    PublishMonitors();
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
