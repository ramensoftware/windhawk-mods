// ==WindhawkMod==
// @id              custom-top-bar
// @name            Custom Top Bar
// @name:ru-RU      Custom Top Bar
// @description     A customizable extra bar: apps, clock, calendar, Wi-Fi, volume, keyboard layout, battery, media player, timers and more
// @description:ru-RU Настраиваемая дополнительная панель: приложения, часы, календарь, Wi-Fi, громкость, раскладка, батарея, плеер, таймеры и многое другое
// @version         0.15
// @author          MrAllayi
// @github          https://github.com/mrallayi
// @include         windhawk.exe
// @compilerOptions -lgdi32 -luser32 -lshell32 -liphlpapi -lkernel32 -lwlanapi -lbthprops -lshcore -ldwmapi -lole32 -lpowrprof -ladvapi32 -lws2_32 -DWIN32_LEAN_AND_MEAN
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Top Bar

An additional bar (top or bottom of the screen). The Windows taskbar stays
where it is.

The settings are split into sections: layout, size, colors, fonts, apps,
clock, right-side features, Wi-Fi and Bluetooth, timers, custom
items.

**Layout** is chosen in the settings ("Item arrangement"):
* Apps on the left, clock in the center, features on the right;
* Features on the left, clock in the center, apps on the right;
* Apps on the left, features on the right, clock at the far right edge.

**Apps:** pinned apps and a mini player.
* To pin: just drag a shortcut, program, file or folder onto the bar (from
  the desktop or Explorer). A blue line shows where the icon will land.
* Change the order by dragging icons along the bar with the mouse (hold the
  icon and drag it) or via right click: "Move left / right".
* Right click on an icon: open, show in folder, move, unpin.
* What to show for pinned apps: name and icon, name only, or icon only.

**Win button (far left corner):** off by default. Choose a logo: Windows 7,
10, 11, Linux, Apple. A click opens a quick actions menu (Start, screenshot,
Task Manager, lock, sleep, restart, etc.).

**Clock:** a click opens a calendar with events and time zones, a right
click shows the list of time zones.

**Features:** CPU / RAM / network, "Hidden icons" button, menu
button (three bars), keyboard layout (language), Wi-Fi, Bluetooth, volume,
battery, timers (timer, stopwatch, Pomodoro, alarm), notifications,
right-side app icons, custom text / image.
* **Feature order:** hold any feature and drag it with the mouse to the
  desired place (a blue line shows where it will land). Or right click a
  feature: "Move left / right", "Reset feature order". The order is
  remembered.

## Notes
* The mod runs in its own process (a Windhawk tool mod) and not inside
  Explorer, so it cannot affect Explorer's stability. If Explorer restarts,
  the bar is recreated automatically. The mod does not use the network.
* The menu button (three bars) sits right after the "Hidden icons" arrow and
  is enabled by default. A click opens the Windows 11 quick settings
  (network, sound, brightness) - the same window as the network and sound
  tray icons. Where the window appears is set in the settings ("Where to
  open the menu button window"): by default next to your bar (below the top
  bar or above the bottom one), or like in Windows - near the taskbar in the
  bottom right corner. On Windows 10 the notification center opens.
* Keyboard layout: shows the input language of the active window (RU, EN,
  ...). A click switches the layout (like Win+Space).
* Volume: the mouse wheel over the icon changes the volume, right click
  opens a menu (mute, mixer, sound settings), left click opens the Windows
  11 quick settings (Win+A).
* The "Hidden icons" button opens the system hidden tray icons window (it
  presses Win+B and Enter). For this the Windows taskbar must stay in
  place - third-party programs cannot embed the tray icons themselves into
  the bar.
* Right-side apps: the "Right-side apps" list in the settings. Change the
  order by dragging an icon with the mouse or via right click (Move left /
  right). The whole group of apps can be moved relative to other features
  via right click.
* Custom text / image: the size is set in the settings. Image: png, jpg,
  ico, bmp.
* Win: when the Win key is pressed, a hidden bar smoothly appears for about
  4 seconds.
* The mini player detects the track from the player window title (for
  example Spotify: "Artist - Title") and controls players with media keys.
* The notifications button opens the notification center (and resets the
  counter). The counter (optional, in the settings) is approximate: it
  counts the popup banners that you have seen.
* Timer: a click on the alert turns it off, the menu contains a timer pause.
* Calendar: events are set as lines like `2026-12-31 Text`. A line without a
  year `12-31 Text` repeats every year (birthdays, etc.). In the open
  calendar the mouse wheel, arrow keys, PgUp/PgDn and Esc work.
* Time zones: lines like `Tokyo=Tokyo Standard Time`.
* Bluetooth: only classic devices are shown (the BLE API does not list
  them).
* Colors are set in HEX: `#202020`.
* Widget icons come from the Segoe MDL2 Assets font.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- layout:
  - position: top
    $name: Bar position
    $name:ru-RU: Положение панели
    $options:
    - top: Top
    - bottom: Bottom
    $options:ru-RU:
    - top: Сверху
    - bottom: Снизу
  - arrangement: appsLeft
    $name: Item arrangement
    $name:ru-RU: Расположение элементов
    $options:
    - appsLeft: Apps on the left, clock in the center, features on the right
    - funcLeft: Features on the left, clock in the center, apps on the right
    - clockRight: Apps on the left, features and clock on the right
    $options:ru-RU:
    - appsLeft: Приложения слева, часы по центру, функции справа
    - funcLeft: Функции слева, часы по центру, приложения справа
    - clockRight: Приложения слева, функции и часы справа
  - winButton: none
    $name: Win button in the left corner
    $name:ru-RU: Кнопка Win в левом углу
    $description: Click opens a quick actions menu (screenshot, lock, sleep, etc.).
    $description:ru-RU: Клик открывает меню быстрых действий (скриншот, блокировка, сон и т. д.).
    $options:
    - none: Off
    - win7: Windows 7
    - win10: Windows 10
    - win11: Windows 11
    - linux: Linux
    - apple: Apple
    $options:ru-RU:
    - none: Выключена
    - win7: Windows 7
    - win10: Windows 10
    - win11: Windows 11
    - linux: Linux
    - apple: Apple
  - monitors: primary
    $name: Monitors
    $name:ru-RU: Мониторы
    $options:
    - primary: Primary only
    - all: All monitors
    $options:ru-RU:
    - primary: Только основной
    - all: Все мониторы
  - reserveSpace: true
    $name: Reserve space (windows do not go under the bar)
    $name:ru-RU: Резервировать место (окна не заходят под панель)
  - hideInFullscreen: true
    $name: Hide in fullscreen apps and games
    $name:ru-RU: Скрывать в полноэкранных приложениях и играх
  $name: "1. Layout"
  $name:ru-RU: "1. Расположение"
- look:
  - barHeight: 49
    $name: Bar height (px)
    $name:ru-RU: Высота панели (px)
  - padding: 12
    $name: Inner padding (px)
    $name:ru-RU: Внутренний отступ (px)
  - itemSpacing: 12
    $name: Spacing between items (px)
    $name:ru-RU: Расстояние между элементами (px)
  - verticalOffset: 0
    $name: Vertical content offset (px)
    $name:ru-RU: Вертикальный сдвиг содержимого (px)
  - marginTop: 0
    $name: Top margin (px, floating bar)
    $name:ru-RU: Отступ сверху (px, плавающая панель)
  - marginBottom: 0
    $name: Bottom margin (px, floating bar)
    $name:ru-RU: Отступ снизу (px, плавающая панель)
  - marginLeft: 0
    $name: Left margin (px, floating bar)
    $name:ru-RU: Отступ слева (px, плавающая панель)
  - marginRight: 0
    $name: Right margin (px, floating bar)
    $name:ru-RU: Отступ справа (px, плавающая панель)
  - cornerRadius: 0
    $name: Corner radius (px)
    $name:ru-RU: Радиус скругления углов (px)
  - animations: true
    $name: Animations (fade-in, highlight, pulse)
    $name:ru-RU: Анимации (плавное появление, подсветка, пульсация)
  $name: "2. Size and spacing"
  $name:ru-RU: "2. Размеры и отступы"
- colors:
  - colorMode: manual
    $name: Color mode
    $name:ru-RU: Режим цветов
    $options:
    - manual: Custom colors
    - theme: Windows light / dark theme
    - accent: Windows accent color
    $options:ru-RU:
    - manual: Свои цвета
    - theme: Светлая / тёмная тема Windows
    - accent: Акцентный цвет Windows
  - backgroundColor: "#202020"
    $name: Bar color (HEX)
    $name:ru-RU: Цвет панели (HEX)
  - textColor: "#FFFFFF"
    $name: Text color (HEX)
    $name:ru-RU: Цвет текста (HEX)
  - opacity: 255
    $name: Background opacity (1-255)
    $name:ru-RU: Непрозрачность фона (1-255)
  - effect: none
    $name: Effect behind the bar
    $name:ru-RU: Эффект под панелью
    $options:
    - none: None
    - blur: Blur
    - acrylic: Acrylic
    - mica: Mica (Windows 11)
    $options:ru-RU:
    - none: Нет
    - blur: Размытие
    - acrylic: Акрил
    - mica: Мика (Windows 11)
  - borderColor: "#808080"
    $name: Border color (HEX)
    $name:ru-RU: Цвет рамки (HEX)
  - borderWidth: 0
    $name: Border width (px)
    $name:ru-RU: Толщина рамки (px)
  - shadowSize: 0
    $name: Shadow size (px, needs a margin from the edges)
    $name:ru-RU: Размер тени (px, нужен отступ от краёв)
  - shadowOpacity: 120
    $name: Shadow opacity (0-255)
    $name:ru-RU: Непрозрачность тени (0-255)
  $name: "3. Colors and effects"
  $name:ru-RU: "3. Цвета и эффекты"
- fonts:
  - fontName: "Segoe UI"
    $name: Font
    $name:ru-RU: Шрифт
  - fontSize: 11
    $name: Font size (pt)
    $name:ru-RU: Размер шрифта (pt)
  - clockFontSize: 12
    $name: Clock font size (pt)
    $name:ru-RU: Размер шрифта часов (pt)
  - clockBold: false
    $name: Bold clock
    $name:ru-RU: Жирный шрифт часов
  - iconSize: 22
    $name: Icon size (px)
    $name:ru-RU: Размер значков (px)
  $name: "4. Fonts and icons"
  $name:ru-RU: "4. Шрифты и значки"
- apps:
  - pinDisplay: both
    $name: What to show for pinned apps
    $name:ru-RU: Что показывать для закреплённых приложений
    $options:
    - both: Name and icon
    - name: Name only
    - icon: Icon only
    $options:ru-RU:
    - both: Название и значок
    - name: Только название
    - icon: Только значок
  - pinNameLength: 7
    $name: Name length (characters)
    $name:ru-RU: Длина названия (символов)
    $description: Long names are truncated to this number of characters.
    $description:ru-RU: Длинные названия обрезаются до этого числа символов.
  - pinnedApps: [""]
    $name: Pinned apps
    $name:ru-RU: Закреплённые приложения
    $description: "Optional: shortcuts, programs and folders can simply be dragged onto the bar. Here: paths to .exe / .lnk / folders / files."
    $description:ru-RU: "Необязательно: ярлыки, программы и папки можно просто перетащить на панель. Здесь: пути к .exe / .lnk / папкам / файлам."
  - showMedia: false
    $name: Mini player
    $name:ru-RU: Мини-плеер
  - mediaProcesses: "Spotify.exe"
    $name: Player processes
    $name:ru-RU: Процессы плеера
    $description: Comma-separated, for example Spotify.exe, AIMP.exe
    $description:ru-RU: Через запятую, например Spotify.exe, AIMP.exe
  $name: "5. Apps and player"
  $name:ru-RU: "5. Приложения и плеер"
- clock:
  - clockFormat: timeWeekdayDate
    $name: What to show in the clock
    $name:ru-RU: Что показывать в часах
    $options:
    - time: Time
    - timeWeekday: Time + weekday
    - timeDate: Time + date
    - timeWeekdayDate: Time + weekday + date
    $options:ru-RU:
    - time: Время
    - timeWeekday: Время + день недели
    - timeDate: Время + дата
    - timeWeekdayDate: Время + день недели + дата
  - hourFormat: system
    $name: Clock format
    $name:ru-RU: Формат часов
    $options:
    - system: System default
    - h24: 24-hour
    - h12: 12-hour (AM/PM)
    $options:ru-RU:
    - system: Как в системе
    - h24: 24-часовой
    - h12: 12-часовой (AM/PM)
  - customTimeFormat: ""
    $name: Custom time format
    $name:ru-RU: Свой формат времени
    $description: For example HH:mm:ss or h:mm tt. If filled in, replaces the clock format above.
    $description:ru-RU: Например HH:mm:ss или h:mm tt. Если заполнено, заменяет формат часов выше.
  - showSeconds: false
    $name: Show seconds
    $name:ru-RU: Показывать секунды
  - dateFormat: "d MMMM yyyy"
    $name: Date format
    $name:ru-RU: Формат даты
  - language: system
    $name: Language of day and month names
    $name:ru-RU: Язык названий дней и месяцев
    $options:
    - system: System default
    - ru-RU: Русский
    - en-US: English (US)
    - en-GB: English (UK)
    - uk-UA: Українська
    - de-DE: Deutsch
    - fr-FR: Français
    - es-ES: Español
    - it-IT: Italiano
    - nl-NL: Nederlands
    - pl-PL: Polski
    - ja-JP: 日本語
    - zh-CN: 中文
    $options:ru-RU:
    - system: Как в системе
    - ru-RU: Русский
    - en-US: English (US)
    - en-GB: English (UK)
    - uk-UA: Українська
    - de-DE: Deutsch
    - fr-FR: Français
    - es-ES: Español
    - it-IT: Italiano
    - nl-NL: Nederlands
    - pl-PL: Polski
    - ja-JP: 日本語
    - zh-CN: 中文
  - weekStartsMonday: true
    $name: Week starts on Monday
    $name:ru-RU: Неделя начинается с понедельника
  - calendarEvents: [""]
    $name: Calendar events
    $name:ru-RU: События календаря
    $description: "Format: 2026-12-31 Event text. Without a year (12-31 Event text) the event repeats every year."
    $description:ru-RU: "Формат: 2026-12-31 Текст события. Без года (12-31 Текст события) событие повторяется каждый год."
  - timeZones: [""]
    $name: Time zones in the calendar
    $name:ru-RU: Часовые пояса в календаре
    $description: "Format: Label=Windows key, for example Tokyo=Tokyo Standard Time"
    $description:ru-RU: "Формат: Подпись=ключ Windows, например Токио=Tokyo Standard Time"
  $name: "6. Clock and calendar"
  $name:ru-RU: "6. Часы и календарь"
- funcs:
  - showCpu: true
    $name: CPU
    $name:ru-RU: Процессор (CPU)
  - showMem: true
    $name: RAM
    $name:ru-RU: Память (RAM)
  - showNet: true
    $name: Network speed
    $name:ru-RU: Скорость сети
  - showTrayChevron: true
    $name: Show "Hidden icons" button (arrow)
    $name:ru-RU: Показывать кнопку «Скрытые значки» (стрелка)
  - showMenuButton: true
    $name: "Menu button (quick settings - network, sound, brightness)"
    $name:ru-RU: "Кнопка меню (быстрые настройки: сеть, звук, яркость)"
  - menuWindowPlace: bar
    $name: Where to open the menu button window
    $name:ru-RU: Где открывать окно кнопки меню
    $description: "Next to my bar: the window appears below the top bar (or above the bottom one). Like Windows: near the taskbar, in the bottom right corner."
    $description:ru-RU: "Рядом с моей панелью: окно появляется под верхней панелью (или над нижней). Как в Windows: возле панели задач, в правом нижнем углу."
    $options:
    - bar: Next to my bar
    - system: Like Windows (near the taskbar)
    $options:ru-RU:
    - bar: Рядом с моей панелью
    - system: Как в Windows (возле панели задач)
  - showLang: true
    $name: Keyboard layout (input language)
    $name:ru-RU: Раскладка клавиатуры (язык ввода)
  - showVolume: true
    $name: Volume
    $name:ru-RU: Громкость
  - showBattery: true
    $name: Battery (laptops only)
    $name:ru-RU: Батарея (только ноутбуки)
  - showBell: true
    $name: Notifications button
    $name:ru-RU: Кнопка уведомлений
  - notifCounter: false
    $name: Notification counter (approximate)
    $name:ru-RU: Счётчик уведомлений (приблизительный)
    $description: Counts popup banners since the notification center was last opened. It does not show the real number of unread notifications.
    $description:ru-RU: Считает всплывающие баннеры с момента последнего открытия центра уведомлений. Реальное число непрочитанных уведомлений не показывает.
  - notifKeywords: "notification,уведомлени"
    $name: Words in the notification banner window title
    $name:ru-RU: Слова в заголовке окна баннера уведомления
    $description: Comma-separated, case-insensitive. If the counter does not trigger, add a word from the banner title in your Windows language.
    $description:ru-RU: Через запятую, без учёта регистра. Если счётчик не срабатывает, добавьте слово из заголовка баннера на языке вашей Windows.
  $name: "7. Right-side features"
  $name:ru-RU: "7. Функции справа"
- network:
  - showWifi: true
    $name: Wi-Fi / network
    $name:ru-RU: Wi-Fi / сеть
  - wifiShowName: true
    $name: Wi-Fi - network name
    $name:ru-RU: "Wi-Fi: название сети"
  - wifiShowSignal: true
    $name: Wi-Fi - signal strength
    $name:ru-RU: "Wi-Fi: уровень сигнала"
  - wifiShowIp: false
    $name: Wi-Fi - IP address
    $name:ru-RU: "Wi-Fi: IP-адрес"
  - showBluetooth: true
    $name: Bluetooth (connected devices)
    $name:ru-RU: Bluetooth (подключённые устройства)
  - btShowNames: true
    $name: Bluetooth - show device name
    $name:ru-RU: "Bluetooth: показывать название устройства"
  $name: "8. Wi-Fi and Bluetooth"
  $name:ru-RU: "8. Wi-Fi и Bluetooth"
- timers:
  - showTimer: true
    $name: Timer / stopwatch / Pomodoro / alarm
    $name:ru-RU: Таймер / секундомер / Помодоро / будильник
  - pomodoroWork: 25
    $name: Pomodoro - work (min)
    $name:ru-RU: "Помодоро: работа (мин)"
  - pomodoroBreak: 5
    $name: Pomodoro - break (min)
    $name:ru-RU: "Помодоро: перерыв (мин)"
  - alarmTime: "07:30"
    $name: Alarm time (HH:mm)
    $name:ru-RU: Время будильника (ЧЧ:мм)
  - alarmEnabled: false
    $name: Alarm enabled
    $name:ru-RU: Будильник включён
  $name: "9. Timers and alarm"
  $name:ru-RU: "9. Таймеры и будильник"
- custom:
  - rightApp: ""
    $name: Right-side app (icon)
    $name:ru-RU: Приложение справа (значок)
  - rightApps: [""]
    $name: Right-side apps (list)
    $name:ru-RU: Приложения справа (список)
    $description: Paths to .exe / .lnk / folders. The order is changed by dragging icons along the bar.
    $description:ru-RU: Пути к .exe / .lnk / папкам. Порядок меняется перетаскиванием значков по панели.
  - rightText: ""
    $name: Custom text
    $name:ru-RU: Свой текст
  - rightTextSize: 11
    $name: Custom text size (pt)
    $name:ru-RU: Размер своего текста (pt)
  - rightImage: ""
    $name: Custom image (path to png/jpg/ico/bmp)
    $name:ru-RU: Своя картинка (путь к png/jpg/ico/bmp)
  - rightImageSize: 22
    $name: Custom image height (px)
    $name:ru-RU: Высота своей картинки (px)
  $name: "10. Custom text, image, right-side apps"
  $name:ru-RU: "10. Свой текст, картинка, приложения справа"
*/
// ==/WindhawkModSettings==

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <ole2.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellscalingapi.h>
#include <dwmapi.h>
#include <wlanapi.h>
#include <bluetoothapis.h>
#include <powrprof.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <vector>

// ------------------------------------------------------------------ consts --

#define WM_APPBAR (WM_USER + 1)
#define WM_RELOAD (WM_USER + 2)

enum { TM_TICK = 1, TM_SLOW, TM_FS, TM_ANIM, TM_REBUILD, TM_KEY };

static const wchar_t* kBarClass = L"WH_TopBar2";
static const wchar_t* kFxClass = L"WH_TopBar2_Fx";
static const wchar_t* kCalClass = L"WH_TopBar2_Cal";
static const wchar_t* kCtlClass = L"WH_TopBar2_Ctl";
static const DWORD kKey = 0x00FF00FF;  // "transparent" color key used when drawing

#ifndef WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
#define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

// ---------------------------------------------------------------- settings --

struct EventItem {
    int y = 0, m = 0, d = 0;
    std::wstring text;
};
struct TzEntry {
    std::wstring label, key;
};

struct Settings {
    std::wstring position = L"top", monitors = L"primary", colorMode = L"manual",
                 layout = L"appsLeft", winButton = L"none", pinDisplay = L"both",
                 menuPlace = L"bar";
    std::wstring bg, text, borderColor, effect, font, clockFormat, hourFormat,
        customTime, dateFormat, language, mediaProcesses, alarmTime, rightApp, rightText, rightImage;
    int barHeight = 49, sideWidth = 180, padding = 12, itemSpacing = 12,
        mLeft = 0, mRight = 0, mTop = 0, mBottom = 0, radius = 0, yOffset = 0, opacity = 255, borderWidth = 0,
        shadowSize = 0, shadowOpacity = 120, fontSize = 11, clockFontSize = 12,
        iconSize = 22, pomoWork = 25, pomoBreak = 5, pinNameLen = 7,
        rightTextSize = 11, rightImageSize = 22;
    bool reserveSpace = true, hideFs = true, clockBold = false, animations = true,
         showSeconds = false, weekMonday = true, showCpu = true, showMem = true,
         showNet = true, showBattery = true, showWifi = true, wifiName = true,
         wifiSignal = true, wifiIp = false, showBt = true, btNames = true,
         showMedia = false, showTimer = true,
         alarmEnabled = false, showBell = true,
         showTray = true, showMenu = true, showLang = true, showVolume = true;
    std::vector<std::wstring> pinned, rightApps;
    std::vector<EventItem> events;
    std::vector<TzEntry> tz;
    std::vector<std::wstring> mediaProcs;
    std::vector<std::wstring> notifKeys;
    std::wstring notifKeywords;
    bool notifCounter = false;
    int alarmH = -1, alarmM = -1;
};

static Settings g_s;
static bool g_vertical = false;  // the bar is always horizontal (top / bottom)
static COLORREF g_bgColor = RGB(32, 32, 32), g_textColor = RGB(255, 255, 255),
                g_accent = RGB(0, 120, 215), g_borderColor = RGB(128, 128, 128);

// -------------------------------------------------------------- shared data --

struct Data {
    int cpu = 0, mem = 0;
    double down = 0, up = 0;
    bool hasBattery = false, charging = false, ac = false;
    int batPct = -1;
    bool wifiUp = false;
    std::wstring ssid, ip;
    int wifiSignal = 0;
    int btCount = 0;
    std::wstring btNames;
    std::wstring track;
    int volPct = -1;
    bool volMuted = false;
    std::wstring lang;
};
static Data g_d;
static CRITICAL_SECTION g_cs;
struct Lock {
    Lock() { EnterCriticalSection(&g_cs); }
    ~Lock() { LeaveCriticalSection(&g_cs); }
};

static std::atomic<bool> g_stop{false};
static std::atomic<int> g_workers{0};
static std::atomic<bool> g_btBusy{false};
static HANDLE g_thread = nullptr;
static std::atomic<HWND> g_ctl{nullptr};
static ULONGLONG g_btNext = 0;
static int g_notifCount = 0;
static bool g_toastPrev = false;

// ---------------------------------------------------------- elements model --

enum Act {
    A_NONE, A_PIN, A_CLOCK, A_MPREV, A_MPLAY, A_MNEXT, A_MTRACK, A_STATS,
    A_WIFI, A_BT, A_BATTERY, A_TIMER, A_BELL, A_START, A_RIGHTAPP,
    A_LANG, A_VOLUME, A_TRAY, A_CUSTOM, A_MENU
};

struct Elem {
    Act act = A_NONE;
    int idx = 0;
    std::wstring glyph, text;
    HICON icon = nullptr;
    bool pulse = false;
    int font = 0;  // 0 = Segoe MDL2 Assets, 1 = Segoe UI Symbol
    bool accent = false;
};

struct PinInfo {
    std::wstring raw, path, name;
    HICON icon = nullptr;
    FILETIME mtime{};
};

static std::vector<PinInfo> g_pins;
static std::vector<PinInfo> g_rightApps;
static HICON g_customIcon = nullptr;
static int g_customW = 0, g_customH = 0;
static RECT g_trayAnchor{};
static int g_trayEdge = 0;  // 0 = top, 1 = bottom
static std::vector<Elem> g_left, g_right;  // g_left = apps, g_right = features
static Elem g_clock;
static bool g_hasClock = false;
static Elem g_start;
static bool g_hasStart = false;
static uint64_t g_sig = 0;
static bool g_pulseActive = false;
static std::map<int, float> g_hover;
static int g_fade = 255;
static int g_fadeTarget = 255;      // target opacity (0 = hide, 255 = show)
static bool g_hiding = false;       // fading out
static ULONGLONG g_peekUntil = 0;   // until when the bar is shown after a Win key press
static bool g_winPrev = false;
static bool g_animRunning = false;
static bool g_fsHidden = false;
static void FinishHide();

struct Bar {
    HWND hwnd = nullptr, fx = nullptr;
    HMONITOR mon = nullptr;
    RECT monRc{}, pos{};
    UINT dpi = 96;
    bool registered = false, dirty = true, visible = true, tracking = false;
    HFONT fText = nullptr, fClock = nullptr, fIcon = nullptr, fSym = nullptr, fCustom = nullptr;
    HDC mem = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ oldBmp = nullptr;
    void* bits = nullptr;
    int bw = 0, bh = 0;
    std::vector<RECT> rl, rr;
    RECT rClock{}, rStart{};
    int hotKey = -1;
    int dragIdx = -1, dropIdx = -1;
    bool dragging = false, extDrag = false, dropReg = false;
    POINT dragStart{}, dragPt{};
    int downList = -1, downI = -1, downKey = -1;
    int dragKind = 0;  // 0 = pinned apps, 1 = right-side apps, 2 = features (order)
};
static std::vector<Bar*> g_bars;

struct Hit {
    int list = -1, i = -1;  // list: 0 = apps, 1 = features, 2 = clock, 3 = Win button
};

static int Sc(const Bar* b, int v) { return MulDiv(v, b->dpi, 96); }

// Handle of this mod's own module (used for window classes and windows).
static HINSTANCE ModInst() {
    static HINSTANCE h = nullptr;
    if (!h)
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)&ModInst, (HMODULE*)&h);
    return h;
}

// ----------------------------------------------------------------- helpers --

static std::wstring TakeString(PCWSTR s) {
    std::wstring r = s ? s : L"";
    if (s) Wh_FreeStringSetting(s);
    return r;
}

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n\"");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t\r\n\"");
    return s.substr(a, b - a + 1);
}

static std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

static COLORREF ParseColor(const std::wstring& t, COLORREF def) {
    std::wstring h = Trim(t);
    if (!h.empty() && h[0] == L'#') h.erase(0, 1);
    if (h.size() != 6) return def;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(h.c_str(), &end, 16);
    if (!end || *end) return def;
    return RGB((v >> 16) & 255, (v >> 8) & 255, v & 255);
}

static COLORREF Blend(COLORREF a, COLORREF b, int t) {  // t: 0..256
    if (t < 0) t = 0;
    if (t > 256) t = 256;
    return RGB((GetRValue(a) * (256 - t) + GetRValue(b) * t) >> 8,
               (GetGValue(a) * (256 - t) + GetGValue(b) * t) >> 8,
               (GetBValue(a) * (256 - t) + GetBValue(b) * t) >> 8);
}

static int Luma(COLORREF c) {
    return (GetRValue(c) * 299 + GetGValue(c) * 587 + GetBValue(c) * 114) / 1000;
}

static DWORD ToDib(COLORREF c) {
    return ((DWORD)GetRValue(c) << 16) | ((DWORD)GetGValue(c) << 8) | GetBValue(c);
}

static uint64_t HashStr(uint64_t h, const std::wstring& s) {
    for (wchar_t c : s) {
        h ^= (uint64_t)c;
        h *= 1099511628211ULL;
    }
    h ^= 0xFF;
    h *= 1099511628211ULL;
    return h;
}

static std::wstring ExpandPath(const std::wstring& p) {
    wchar_t buf[MAX_PATH * 2];
    DWORD n = ExpandEnvironmentStringsW(p.c_str(), buf, ARRAYSIZE(buf));
    if (n == 0 || n > ARRAYSIZE(buf)) return p;
    return buf;
}

static std::wstring FromUtf8(const char* s, int len) {
    if (len <= 0) return L"";
    int n = MultiByteToWideChar(CP_UTF8, 0, s, len, nullptr, 0);
    std::wstring w(n, 0);
    MultiByteToWideChar(CP_UTF8, 0, s, len, &w[0], n);
    return w;
}

static std::wstring FmtSpeed(double b) {
    wchar_t buf[32];
    if (b >= 1048576.0)
        swprintf(buf, 32, L"%.1f MB/s", b / 1048576.0);
    else
        swprintf(buf, 32, L"%.0f KB/s", b / 1024.0);
    return buf;
}

static std::wstring FmtMs(ULONGLONG ms) {
    ULONGLONG s = ms / 1000;
    wchar_t buf[32];
    if (s >= 3600)
        swprintf(buf, 32, L"%llu:%02llu:%02llu", s / 3600, (s / 60) % 60, s % 60);
    else
        swprintf(buf, 32, L"%02llu:%02llu", s / 60, s % 60);
    return buf;
}

static int DayOfWeek(int y, int m, int d) {  // 0 = Sunday
    static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    if (m < 3) y -= 1;
    return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

static int DaysInMonth(int y, int m) {
    static const int dm[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) return 29;
    return dm[m - 1];
}

static LPCWSTR LocaleName() {
    if (g_s.language.empty() || g_s.language == L"system") return LOCALE_NAME_USER_DEFAULT;
    return g_s.language.c_str();
}

static void SplitLower(const std::wstring& src, std::vector<std::wstring>& out) {
    std::wstring cur;
    for (wchar_t c : src + L",") {
        if (c == L',' || c == L';') {
            cur = Lower(Trim(cur));
            if (!cur.empty()) out.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
}

// --------------------------------------------------------------- load config --

static void LoadSettings() {
    Settings s;
    auto S = [](const wchar_t* n) { return TakeString(Wh_GetStringSetting(n)); };
    auto I = [](const wchar_t* n) { return Wh_GetIntSetting(n); };

    // 1. Layout
    s.position = S(L"layout.position");
    s.layout = S(L"layout.arrangement");
    s.winButton = S(L"layout.winButton");
    s.monitors = S(L"layout.monitors");
    s.reserveSpace = I(L"layout.reserveSpace") != 0;
    s.hideFs = I(L"layout.hideInFullscreen") != 0;

    // 2. Size and spacing
    s.barHeight = I(L"look.barHeight");
    s.padding = I(L"look.padding");
    s.itemSpacing = I(L"look.itemSpacing");
    s.yOffset = I(L"look.verticalOffset");
    s.mTop = I(L"look.marginTop");
    s.mBottom = I(L"look.marginBottom");
    s.mLeft = I(L"look.marginLeft");
    s.mRight = I(L"look.marginRight");
    s.radius = I(L"look.cornerRadius");
    s.animations = I(L"look.animations") != 0;

    // 3. Colors and effects
    s.colorMode = S(L"colors.colorMode");
    s.bg = S(L"colors.backgroundColor");
    s.text = S(L"colors.textColor");
    s.opacity = I(L"colors.opacity");
    s.effect = S(L"colors.effect");
    s.borderColor = S(L"colors.borderColor");
    s.borderWidth = I(L"colors.borderWidth");
    s.shadowSize = I(L"colors.shadowSize");
    s.shadowOpacity = I(L"colors.shadowOpacity");

    // 4. Fonts and icons
    s.font = S(L"fonts.fontName");
    s.fontSize = I(L"fonts.fontSize");
    s.clockFontSize = I(L"fonts.clockFontSize");
    s.clockBold = I(L"fonts.clockBold") != 0;
    s.iconSize = I(L"fonts.iconSize");

    // 5. Apps and player
    s.pinDisplay = S(L"apps.pinDisplay");
    s.pinNameLen = I(L"apps.pinNameLength");
    s.showMedia = I(L"apps.showMedia") != 0;
    s.mediaProcesses = S(L"apps.mediaProcesses");

    // 6. Clock and calendar
    s.clockFormat = S(L"clock.clockFormat");
    s.hourFormat = S(L"clock.hourFormat");
    s.customTime = Trim(S(L"clock.customTimeFormat"));
    s.showSeconds = I(L"clock.showSeconds") != 0;
    s.dateFormat = S(L"clock.dateFormat");
    s.language = S(L"clock.language");
    s.weekMonday = I(L"clock.weekStartsMonday") != 0;

    // 7. Right-side features
    s.showCpu = I(L"funcs.showCpu") != 0;
    s.showMem = I(L"funcs.showMem") != 0;
    s.showNet = I(L"funcs.showNet") != 0;
    s.showTray = I(L"funcs.showTrayChevron") != 0;
    s.showMenu = I(L"funcs.showMenuButton") != 0;
    s.menuPlace = S(L"funcs.menuWindowPlace");
    if (s.menuPlace != L"system") s.menuPlace = L"bar";
    s.showLang = I(L"funcs.showLang") != 0;
    s.showVolume = I(L"funcs.showVolume") != 0;
    s.showBattery = I(L"funcs.showBattery") != 0;
    s.showBell = I(L"funcs.showBell") != 0;
    s.notifCounter = I(L"funcs.notifCounter") != 0;
    s.notifKeywords = S(L"funcs.notifKeywords");
    SplitLower(s.notifKeywords, s.notifKeys);

    // 8. Wi-Fi and Bluetooth
    s.showWifi = I(L"network.showWifi") != 0;
    s.wifiName = I(L"network.wifiShowName") != 0;
    s.wifiSignal = I(L"network.wifiShowSignal") != 0;
    s.wifiIp = I(L"network.wifiShowIp") != 0;
    s.showBt = I(L"network.showBluetooth") != 0;
    s.btNames = I(L"network.btShowNames") != 0;

    // 9. Timers
    s.showTimer = I(L"timers.showTimer") != 0;
    s.pomoWork = I(L"timers.pomodoroWork");
    s.pomoBreak = I(L"timers.pomodoroBreak");
    s.alarmTime = Trim(S(L"timers.alarmTime"));
    s.alarmEnabled = I(L"timers.alarmEnabled") != 0;

    // 10. Custom items
    s.rightApp = Trim(S(L"custom.rightApp"));
    s.rightText = S(L"custom.rightText");
    s.rightTextSize = I(L"custom.rightTextSize");
    s.rightImage = Trim(S(L"custom.rightImage"));
    s.rightImageSize = I(L"custom.rightImageSize");
    if (s.rightTextSize < 6 || s.rightTextSize > 72) s.rightTextSize = 11;
    if (s.rightImageSize < 8 || s.rightImageSize > 256) s.rightImageSize = 22;

    for (int i = 0; i < 32; i++) {
        std::wstring p = Trim(TakeString(Wh_GetStringSetting(L"apps.pinnedApps[%d]", i)));
        if (!p.empty()) s.pinned.push_back(p);
    }
    for (int i = 0; i < 16; i++) {
        std::wstring p = Trim(TakeString(Wh_GetStringSetting(L"custom.rightApps[%d]", i)));
        if (!p.empty()) s.rightApps.push_back(p);
    }
    for (int i = 0; i < 64; i++) {
        std::wstring e = Trim(TakeString(Wh_GetStringSetting(L"clock.calendarEvents[%d]", i)));
        if (e.size() < 5) continue;
        wchar_t* end = nullptr;
        EventItem it;
        long first = wcstol(e.c_str(), &end, 10);
        if (!end || *end != L'-') continue;
        long second = wcstol(end + 1, &end, 10);
        if (end && *end == L'-') {  // YYYY-MM-DD
            it.y = (int)first;
            it.m = (int)second;
            it.d = (int)wcstol(end + 1, &end, 10);
        } else {  // MM-DD: every year
            it.y = 0;
            it.m = (int)first;
            it.d = (int)second;
        }
        if (it.m < 1 || it.m > 12 || it.d < 1 || it.d > 31) continue;
        it.text = Trim(end ? end : L"");
        if (!it.text.empty() && (it.text[0] == L'|' || it.text[0] == L'-'))
            it.text = Trim(it.text.substr(1));
        s.events.push_back(it);
    }
    for (int i = 0; i < 8; i++) {
        std::wstring z = Trim(TakeString(Wh_GetStringSetting(L"clock.timeZones[%d]", i)));
        size_t eq = z.find(L'=');
        if (eq == std::wstring::npos) continue;
        TzEntry t;
        t.label = Trim(z.substr(0, eq));
        t.key = Trim(z.substr(eq + 1));
        if (!t.key.empty()) s.tz.push_back(t);
    }
    {
        std::wstring cur;
        for (wchar_t c : s.mediaProcesses + L",") {
            if (c == L',' || c == L';') {
                cur = Lower(Trim(cur));
                if (!cur.empty()) s.mediaProcs.push_back(cur);
                cur.clear();
            } else {
                cur += c;
            }
        }
    }
    if (s.alarmTime.size() >= 4) {
        wchar_t* end = nullptr;
        int h = (int)wcstol(s.alarmTime.c_str(), &end, 10);
        if (end && *end == L':') {
            int m = (int)wcstol(end + 1, nullptr, 10);
            if (h >= 0 && h < 24 && m >= 0 && m < 60) {
                s.alarmH = h;
                s.alarmM = m;
            }
        }
    }

    auto clampi = [](int& v, int lo, int hi) {
        if (v < lo) v = lo;
        if (v > hi) v = hi;
    };
    clampi(s.barHeight, 16, 300);
    clampi(s.padding, 0, 80);
    clampi(s.itemSpacing, 0, 80);
    clampi(s.mLeft, 0, 500);
    clampi(s.mRight, 0, 500);
    clampi(s.mTop, 0, 500);
    clampi(s.mBottom, 0, 500);
    clampi(s.radius, 0, 100);
    clampi(s.yOffset, -50, 50);
    clampi(s.opacity, 1, 255);
    clampi(s.borderWidth, 0, 10);
    clampi(s.shadowSize, 0, 40);
    clampi(s.shadowOpacity, 0, 255);
    clampi(s.fontSize, 6, 40);
    clampi(s.clockFontSize, 6, 60);
    clampi(s.iconSize, 8, 96);
    clampi(s.pomoWork, 1, 180);
    clampi(s.pomoBreak, 1, 60);
    clampi(s.pinNameLen, 3, 30);
    if (s.font.empty()) s.font = L"Segoe UI";
    if (s.dateFormat.empty()) s.dateFormat = L"d MMMM yyyy";
    if (s.position != L"bottom") s.position = L"top";  // legacy left/right values -> top
    if (s.layout != L"funcLeft" && s.layout != L"clockRight") s.layout = L"appsLeft";
    if (s.pinDisplay != L"name" && s.pinDisplay != L"icon") s.pinDisplay = L"both";

    g_s = s;
    g_vertical = false;
}

static bool SystemLight() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz) != ERROR_SUCCESS)
        return false;
    return v != 0;
}

static COLORREF SystemAccent() {
    DWORD c = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&c, &opaque)))
        return RGB((c >> 16) & 255, (c >> 8) & 255, c & 255);
    return RGB(0, 120, 215);
}

// returns true if colors changed
static bool ResolveColors() {
    COLORREF bg = g_bgColor, tx = g_textColor, ac = g_accent, bd = g_borderColor;
    ac = SystemAccent();
    if (g_s.colorMode == L"theme") {
        if (SystemLight()) {
            bg = RGB(243, 243, 243);
            tx = RGB(20, 20, 20);
        } else {
            bg = RGB(32, 32, 32);
            tx = RGB(255, 255, 255);
        }
    } else if (g_s.colorMode == L"accent") {
        bg = ac;
        tx = Luma(ac) > 150 ? RGB(0, 0, 0) : RGB(255, 255, 255);
    } else {
        bg = ParseColor(g_s.bg, RGB(32, 32, 32));
        tx = ParseColor(g_s.text, RGB(255, 255, 255));
    }
    bd = ParseColor(g_s.borderColor, RGB(128, 128, 128));
    bool ch = bg != g_bgColor || tx != g_textColor || ac != g_accent || bd != g_borderColor;
    g_bgColor = bg;
    g_textColor = tx;
    g_accent = ac;
    g_borderColor = bd;
    return ch;
}

// -------------------------------------------------------------------- icons --

struct IImgList : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Add(HBITMAP, HBITMAP, int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReplaceIcon(int, HICON, int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOverlayImage(int, int) = 0;
    virtual HRESULT STDMETHODCALLTYPE Replace(int, HBITMAP, HBITMAP) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddMasked(HBITMAP, COLORREF, int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Draw(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove(int) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIcon(int, UINT, HICON*) = 0;
};
static const GUID kIID_IImageList = {0x46EB5926, 0x582E, 0x4017, {0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x09, 0x50}};
typedef HRESULT(WINAPI* SHGetImageList_t)(int, REFIID, void**);

static HICON LoadBigIcon(const std::wstring& path) {
    SHFILEINFOW sfi{};
    if (!SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX)) return nullptr;
    int want = MulDiv(g_s.iconSize, (int)GetDpiForSystem(), 96);
    HICON res = nullptr;
    static SHGetImageList_t fn = (SHGetImageList_t)GetProcAddress(GetModuleHandleW(L"shell32.dll"), "SHGetImageList");
    if (fn) {
        IImgList* il = nullptr;
        int cat = want <= 32 ? 0 : (want <= 48 ? 2 : 4);  // LARGE / EXTRALARGE / JUMBO
        if (SUCCEEDED(fn(cat, kIID_IImageList, (void**)&il)) && il) {
            il->GetIcon(sfi.iIcon, 1 /*ILD_TRANSPARENT*/, &res);
            il->Release();
        }
    }
    if (!res) {
        SHFILEINFOW s2{};
        if (SHGetFileInfoW(path.c_str(), 0, &s2, sizeof(s2), SHGFI_ICON | SHGFI_LARGEICON)) res = s2.hIcon;
    }
    return res;
}

// Image (png/jpg/bmp/ico) -> HICON via the Windows shell.
static HICON LoadImageIcon(const std::wstring& path, int* outW, int* outH) {
    *outW = *outH = 0;
    HICON res = nullptr;
    int want = MulDiv(g_s.rightImageSize, (int)GetDpiForSystem(), 96);
    IShellItemImageFactory* f = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&f))) && f) {
        HBITMAP hb = nullptr;
        SIZE sz{want * 4, want};
        // request a wide area: the image is fitted by height
        if (SUCCEEDED(f->GetImage(sz, SIIGBF_BIGGERSIZEOK | SIIGBF_THUMBNAILONLY, &hb)) && hb) {
            BITMAP bm{};
            GetObject(hb, sizeof(bm), &bm);
            if (bm.bmWidth > 0 && bm.bmHeight > 0) {
                HBITMAP mask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, nullptr);
                ICONINFO ii{};
                ii.fIcon = TRUE;
                ii.hbmColor = hb;
                ii.hbmMask = mask;
                res = CreateIconIndirect(&ii);
                DeleteObject(mask);
                if (res) {
                    *outW = bm.bmWidth;
                    *outH = bm.bmHeight;
                }
            }
            DeleteObject(hb);
        }
        f->Release();
    }
    if (!res) {
        res = LoadBigIcon(path);
        if (res) *outW = *outH = want;
    }
    return res;
}

// Label name: as in Explorer, but without the extension for files.
static std::wstring PinDisplayName(const std::wstring& path) {
    SHFILEINFOW sfi{};
    std::wstring n;
    if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME)) n = sfi.szDisplayName;
    if (n.empty()) {
        size_t sl = path.find_last_of(L"\\/");
        n = sl == std::wstring::npos ? path : path.substr(sl + 1);
    }
    DWORD at = GetFileAttributesW(path.c_str());
    bool isDir = at != INVALID_FILE_ATTRIBUTES && (at & FILE_ATTRIBUTE_DIRECTORY);
    if (!isDir) {
        size_t dot = n.find_last_of(L'.');
        if (dot != std::wstring::npos && dot > 0 && n.size() - dot <= 6) n.erase(dot);
    }
    return n;
}

static FILETIME FileMTime(const std::wstring& p) {
    WIN32_FILE_ATTRIBUTE_DATA d{};
    if (GetFileAttributesExW(p.c_str(), GetFileExInfoStandard, &d)) return d.ftLastWriteTime;
    return FILETIME{};
}

static void ClearIcons() {
    for (auto& p : g_pins)
        if (p.icon) DestroyIcon(p.icon);
    g_pins.clear();
    for (auto& p : g_rightApps)
        if (p.icon) DestroyIcon(p.icon);
    g_rightApps.clear();
    if (g_customIcon) DestroyIcon(g_customIcon);
    g_customIcon = nullptr;
    g_customW = g_customH = 0;
}

static std::vector<std::wstring> LoadStoredList(const wchar_t* name) {
    std::vector<wchar_t> buf(32768, 0);
    Wh_GetStringValue(name, buf.data(), (int)buf.size() - 1);
    std::vector<std::wstring> out;
    std::wstring cur;
    for (const wchar_t* c = buf.data();; c++) {
        if (*c == L'\n' || *c == 0) {
            if (!cur.empty()) out.push_back(cur);
            cur.clear();
            if (!*c) break;
        } else {
            cur += *c;
        }
    }
    return out;
}

static void SaveStoredList(const wchar_t* name, const std::vector<std::wstring>& v) {
    std::wstring o;
    for (auto& x : v) {
        if (!o.empty()) o += L"\n";
        o += x;
    }
    Wh_SetStringValue(name, o.c_str());
}

static bool HasStr(const std::vector<std::wstring>& v, const std::wstring& s) {
    return std::find(v.begin(), v.end(), s) != v.end();
}

static void SavePinOrder() {
    std::vector<std::wstring> v;
    for (auto& p : g_pins) v.push_back(p.raw);
    SaveStoredList(L"pinOrder", v);
}

// ------------------------------------------------- order of right-side features --

static const wchar_t* kDefTokens[] = {L"stats", L"tray", L"menu", L"lang", L"wifi", L"bt",
                                      L"volume", L"battery", L"timer", L"bell", L"apps", L"custom"};
static std::vector<std::wstring> g_rightOrder;

static const wchar_t* TokenOf(Act a) {
    switch (a) {
        case A_STATS: return L"stats";
        case A_TRAY: return L"tray";
        case A_MENU: return L"menu";
        case A_LANG: return L"lang";
        case A_WIFI: return L"wifi";
        case A_BT: return L"bt";
        case A_VOLUME: return L"volume";
        case A_BATTERY: return L"battery";
        case A_TIMER: return L"timer";
        case A_BELL: return L"bell";
        case A_RIGHTAPP: return L"apps";
        case A_CUSTOM: return L"custom";
        default: return L"";
    }
}

static void LoadRightOrder() {
    std::vector<std::wstring> stored = LoadStoredList(L"rightOrderV2"), o;
    const size_t N = sizeof(kDefTokens) / sizeof(kDefTokens[0]);
    for (auto& t : stored) {
        bool known = false;
        for (auto* k : kDefTokens)
            if (t == k) known = true;
        if (known && !HasStr(o, t)) o.push_back(t);
    }
    // insert missing tokens right after the previous token from the default list
    for (size_t i = 0; i < N; i++) {
        std::wstring cur = kDefTokens[i];
        if (HasStr(o, cur)) continue;
        if (i == 0) {
            o.insert(o.begin(), cur);
            continue;
        }
        auto it = std::find(o.begin(), o.end(), std::wstring(kDefTokens[i - 1]));
        if (it == o.end())
            o.push_back(cur);
        else
            o.insert(it + 1, cur);
    }
    g_rightOrder = o;
}

static void SaveRightTokens() { SaveStoredList(L"rightOrderV2", g_rightOrder); }

static void ResetRightOrder() {
    Wh_SetStringValue(L"rightOrderV2", L"");
    LoadRightOrder();
}

// Sequence of features currently visible on the bar (right-side apps are one group).
static std::vector<std::wstring> VisibleTokens() {
    std::vector<std::wstring> v;
    for (auto& e : g_right) {
        std::wstring t = TokenOf(e.act);
        if (t.empty()) continue;
        if (v.empty() || v.back() != t) v.push_back(t);
    }
    return v;
}

static void MoveTokenRel(const std::wstring& t, const std::wstring& ref, bool after) {
    if (t == ref) return;
    g_rightOrder.erase(std::remove(g_rightOrder.begin(), g_rightOrder.end(), t), g_rightOrder.end());
    auto it = std::find(g_rightOrder.begin(), g_rightOrder.end(), ref);
    if (it == g_rightOrder.end()) {
        g_rightOrder.push_back(t);
    } else {
        if (after) ++it;
        g_rightOrder.insert(it, t);
    }
    SaveRightTokens();
}

static bool CanMoveToken(const std::wstring& t, int delta) {
    auto vis = VisibleTokens();
    auto it = std::find(vis.begin(), vis.end(), t);
    if (it == vis.end()) return false;
    int to = (int)(it - vis.begin()) + delta;
    return to >= 0 && to < (int)vis.size();
}

static bool MoveTokenStep(const std::wstring& t, int delta) {
    auto vis = VisibleTokens();
    auto it = std::find(vis.begin(), vis.end(), t);
    if (it == vis.end()) return false;
    int to = (int)(it - vis.begin()) + delta;
    if (to < 0 || to >= (int)vis.size()) return false;
    MoveTokenRel(t, vis[to], delta > 0);
    return true;
}

// Drag and drop: from = index in g_right, to = insertion slot index (0..size).
static bool DropRightToken(int from, int to) {
    if (from < 0 || from >= (int)g_right.size()) return false;
    std::wstring t = TokenOf(g_right[from].act);
    if (t.empty()) return false;
    if (to >= (int)g_right.size()) {
        auto vis = VisibleTokens();
        if (vis.empty() || vis.back() == t) return false;
        MoveTokenRel(t, vis.back(), true);
        return true;
    }
    if (to < 0) return false;
    std::wstring u = TokenOf(g_right[to].act);
    if (u.empty() || u == t) return false;
    bool after = to > 0 && TokenOf(g_right[to - 1].act) == std::wstring(u);  // insertion inside the group
    MoveTokenRel(t, u, after);
    return true;
}

// Pinned = list from settings (except those hidden by the user)
// + those added by drag and drop. The order is stored separately.
static void BuildIcons() {
    ClearIcons();
    auto hidden = LoadStoredList(L"pinHidden");
    auto extra = LoadStoredList(L"pinExtra");
    std::vector<std::wstring> base, ordered;
    for (auto& p : g_s.pinned)
        if (!HasStr(hidden, p) && !HasStr(base, p)) base.push_back(p);
    for (auto& p : extra)
        if (!HasStr(base, p)) base.push_back(p);
    for (auto& r : LoadStoredList(L"pinOrder"))
        if (HasStr(base, r) && !HasStr(ordered, r)) ordered.push_back(r);
    for (auto& b : base)
        if (!HasStr(ordered, b)) ordered.push_back(b);

    for (auto& r : ordered) {
        PinInfo p;
        p.raw = r;
        p.path = ExpandPath(r);
        p.icon = LoadBigIcon(p.path);
        p.mtime = FileMTime(p.path);
        p.name = PinDisplayName(p.path);
        g_pins.push_back(p);
    }
    // right-side apps: legacy setting + list; the order is stored separately
    std::vector<std::wstring> rbase, rord;
    if (!g_s.rightApp.empty()) rbase.push_back(g_s.rightApp);
    for (auto& r : g_s.rightApps)
        if (!HasStr(rbase, r)) rbase.push_back(r);
    for (auto& r : LoadStoredList(L"rightAppOrder"))
        if (HasStr(rbase, r) && !HasStr(rord, r)) rord.push_back(r);
    for (auto& r : rbase)
        if (!HasStr(rord, r)) rord.push_back(r);
    for (auto& r : rord) {
        PinInfo p;
        p.raw = r;
        p.path = ExpandPath(r);
        p.icon = LoadBigIcon(p.path);
        p.name = PinDisplayName(p.path);
        g_rightApps.push_back(p);
    }
    if (!g_s.rightImage.empty())
        g_customIcon = LoadImageIcon(ExpandPath(g_s.rightImage), &g_customW, &g_customH);
}

static void SaveRightOrder() {
    std::vector<std::wstring> v;
    for (auto& p : g_rightApps) v.push_back(p.raw);
    SaveStoredList(L"rightAppOrder", v);
}

static bool MoveRightApp(int idx, int delta) {
    int to = idx + delta;
    if (idx < 0 || to < 0 || idx >= (int)g_rightApps.size() || to >= (int)g_rightApps.size()) return false;
    std::swap(g_rightApps[idx], g_rightApps[to]);
    SaveRightOrder();
    return true;
}

// Pin a path at position "at" (drag and drop onto the bar).
static bool PinPath(const std::wstring& path, int at) {
    if (path.empty() || g_pins.size() >= 64) return false;
    std::wstring lp = Lower(path);
    for (auto& q : g_pins)
        if (Lower(q.raw) == lp || Lower(q.path) == lp) return false;
    PinInfo p;
    p.raw = path;
    p.path = ExpandPath(path);
    p.icon = LoadBigIcon(p.path);
    if (!p.icon) return false;
    p.mtime = FileMTime(p.path);
    p.name = PinDisplayName(p.path);
    if (at < 0 || at > (int)g_pins.size()) at = (int)g_pins.size();
    g_pins.insert(g_pins.begin() + at, p);

    auto extra = LoadStoredList(L"pinExtra");
    auto hidden = LoadStoredList(L"pinHidden");
    hidden.erase(std::remove(hidden.begin(), hidden.end(), path), hidden.end());
    if (!HasStr(g_s.pinned, path) && !HasStr(extra, path)) extra.push_back(path);
    SaveStoredList(L"pinExtra", extra);
    SaveStoredList(L"pinHidden", hidden);
    SavePinOrder();
    return true;
}

static void UnpinAt(int idx) {
    if (idx < 0 || idx >= (int)g_pins.size()) return;
    std::wstring raw = g_pins[idx].raw;
    if (g_pins[idx].icon) DestroyIcon(g_pins[idx].icon);
    g_pins.erase(g_pins.begin() + idx);

    auto extra = LoadStoredList(L"pinExtra");
    extra.erase(std::remove(extra.begin(), extra.end(), raw), extra.end());
    SaveStoredList(L"pinExtra", extra);
    if (HasStr(g_s.pinned, raw)) {
        auto hidden = LoadStoredList(L"pinHidden");
        if (!HasStr(hidden, raw)) hidden.push_back(raw);
        SaveStoredList(L"pinHidden", hidden);
    }
    SavePinOrder();
}

// Move a pinned icon to the adjacent position (delta = -1 left, +1 right).
static bool MovePin(int idx, int delta) {
    int to = idx + delta;
    if (idx < 0 || to < 0 || idx >= (int)g_pins.size() || to >= (int)g_pins.size()) return false;
    std::swap(g_pins[idx], g_pins[to]);
    SavePinOrder();
    return true;
}

// ------------------------------------------------------------------- stats --

static ULONGLONG FtU64(const FILETIME& f) {
    return ((ULONGLONG)f.dwHighDateTime << 32) | f.dwLowDateTime;
}

static void UpdateStats() {
    static ULONGLONG pIdle = 0, pKernel = 0, pUser = 0;
    FILETIME fi, fk, fu;
    if (GetSystemTimes(&fi, &fk, &fu)) {
        ULONGLONG ci = FtU64(fi), ck = FtU64(fk), cu = FtU64(fu);
        if (pKernel || pUser) {
            ULONGLONG dIdle = ci - pIdle;
            ULONGLONG total = (ck - pKernel) + (cu - pUser);
            if (total) g_d.cpu = (int)((total - dIdle) * 100 / total);
        }
        pIdle = ci;
        pKernel = ck;
        pUser = cu;
    }
    MEMORYSTATUSEX m{};
    m.dwLength = sizeof(m);
    if (GlobalMemoryStatusEx(&m)) g_d.mem = (int)m.dwMemoryLoad;

    if (g_s.showNet) {
        static ULONGLONG pIn = 0, pOut = 0, pTick = 0;
        ULONGLONG sIn = 0, sOut = 0;
        PMIB_IF_TABLE2 tbl = nullptr;
        if (GetIfTable2(&tbl) == NO_ERROR && tbl) {
            for (ULONG i = 0; i < tbl->NumEntries; i++) {
                const MIB_IF_ROW2& r = tbl->Table[i];
                if (r.Type == IF_TYPE_SOFTWARE_LOOPBACK || r.OperStatus != IfOperStatusUp) continue;
                sIn += r.InOctets;
                sOut += r.OutOctets;
            }
            FreeMibTable(tbl);
        }
        ULONGLONG now = GetTickCount64();
        if (pTick && now > pTick && sIn >= pIn && sOut >= pOut) {
            double dt = (now - pTick) / 1000.0;
            g_d.down = (sIn - pIn) / dt;
            g_d.up = (sOut - pOut) / dt;
        }
        pIn = sIn;
        pOut = sOut;
        pTick = now;
    }

    SYSTEM_POWER_STATUS ps;
    if (GetSystemPowerStatus(&ps)) {
        g_d.hasBattery = ps.BatteryFlag != 128 && ps.BatteryFlag != 255;
        g_d.batPct = ps.BatteryLifePercent == 255 ? -1 : (int)ps.BatteryLifePercent;
        g_d.charging = (ps.BatteryFlag & 8) != 0;
        g_d.ac = ps.ACLineStatus == 1;
    }
}

// ---------------------------------------------------- volume / keyboard layout --

// Volume is handled by a separate thread (COM does not block the bar UI).
static std::atomic<int> g_volDelta{0};
static std::atomic<bool> g_volToggle{false};

static IAudioEndpointVolume* OpenDefaultVolume() {
    IMMDeviceEnumerator* en = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&en)) || !en)
        return nullptr;
    IMMDevice* dev = nullptr;
    HRESULT hr = en->GetDefaultAudioEndpoint(eRender, eMultimedia, &dev);
    en->Release();
    if (FAILED(hr) || !dev) return nullptr;
    IAudioEndpointVolume* vol = nullptr;
    hr = dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&vol);
    dev->Release();
    return FAILED(hr) ? nullptr : vol;
}

static DWORD WINAPI VolumeWorker(LPVOID) {
    bool com = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
    IAudioEndpointVolume* vol = nullptr;
    ULONGLONG nextOpen = 0, reopenAt = 0;
    while (!g_stop) {
        ULONGLONG now = GetTickCount64();
        if (vol && now >= reopenAt) {  // every 10 s re-acquire the default output device
            vol->Release();
            vol = nullptr;
        }
        if (!vol && now >= nextOpen) {
            nextOpen = now + 3000;
            vol = OpenDefaultVolume();
            reopenAt = now + 10000;
            if (!vol) {
                Lock l;
                g_d.volPct = -1;
            }
        }
        if (vol) {
            int d = g_volDelta.exchange(0);
            bool tg = g_volToggle.exchange(false);
            float v = 0;
            BOOL m = FALSE;
            if (d && SUCCEEDED(vol->GetMasterVolumeLevelScalar(&v))) {
                v += d / 100.0f;
                if (v < 0) v = 0;
                if (v > 1) v = 1;
                vol->SetMasterVolumeLevelScalar(v, nullptr);
                if (v > 0) vol->SetMute(FALSE, nullptr);
            }
            if (tg && SUCCEEDED(vol->GetMute(&m))) vol->SetMute(!m, nullptr);
            if (SUCCEEDED(vol->GetMasterVolumeLevelScalar(&v)) && SUCCEEDED(vol->GetMute(&m))) {
                Lock l;
                g_d.volPct = (int)(v * 100.0f + 0.5f);
                g_d.volMuted = m != FALSE;
            } else {
                vol->Release();
                vol = nullptr;
            }
        }
        Sleep(250);
    }
    if (vol) vol->Release();
    if (com) CoUninitialize();
    g_workers--;
    return 0;
}

// Commands from the UI: update the number on the bar immediately, the thread applies it to the system.
static void AdjustVolume(int deltaPct) {
    g_volDelta += deltaPct;
    Lock l;
    if (g_d.volPct >= 0) {
        g_d.volPct = std::max(0, std::min(100, g_d.volPct + deltaPct));
        if (g_d.volPct > 0) g_d.volMuted = false;
    }
}

static void ToggleMute() {
    g_volToggle = true;
    Lock l;
    g_d.volMuted = !g_d.volMuted;
}

// Input language of the active window (RU, EN...). true if it changed.
static bool UpdateLang() {
    HWND fg = GetForegroundWindow();
    DWORD tid = fg ? GetWindowThreadProcessId(fg, nullptr) : 0;
    HKL k = GetKeyboardLayout(tid);
    LANGID lid = LOWORD((UINT_PTR)k);
    wchar_t buf[16] = L"";
    if (!GetLocaleInfoW(MAKELCID(lid, SORT_DEFAULT), LOCALE_SISO639LANGNAME, buf, 16)) return false;
    std::wstring s = buf;
    for (auto& c : s) c = (wchar_t)towupper(c);
    if (s.empty() || s == g_d.lang) return false;
    g_d.lang = s;
    return true;
}

static void UpdateWifi() {
    std::wstring ssid;
    int sig = 0;
    bool up = false;
    HANDLE h = nullptr;
    DWORD ver = 0;
    if (WlanOpenHandle(2, nullptr, &ver, &h) == ERROR_SUCCESS) {
        PWLAN_INTERFACE_INFO_LIST list = nullptr;
        if (WlanEnumInterfaces(h, nullptr, &list) == ERROR_SUCCESS && list) {
            for (DWORD i = 0; i < list->dwNumberOfItems; i++) {
                if (list->InterfaceInfo[i].isState != wlan_interface_state_connected) continue;
                PWLAN_CONNECTION_ATTRIBUTES a = nullptr;
                DWORD sz = 0;
                if (WlanQueryInterface(h, &list->InterfaceInfo[i].InterfaceGuid,
                                       wlan_intf_opcode_current_connection, nullptr, &sz,
                                       (PVOID*)&a, nullptr) == ERROR_SUCCESS && a) {
                    const DOT11_SSID& s = a->wlanAssociationAttributes.dot11Ssid;
                    ssid = FromUtf8((const char*)s.ucSSID, (int)s.uSSIDLength);
                    sig = (int)a->wlanAssociationAttributes.wlanSignalQuality;
                    up = true;
                    WlanFreeMemory(a);
                }
                break;
            }
            WlanFreeMemory(list);
        }
        WlanCloseHandle(h, nullptr);
    }

    std::wstring ip, ipAny;
    ULONG size = 0;
    ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
    if (GetAdaptersAddresses(AF_INET, flags, nullptr, nullptr, &size) == ERROR_BUFFER_OVERFLOW && size) {
        std::vector<BYTE> buf(size);
        PIP_ADAPTER_ADDRESSES aa = (PIP_ADAPTER_ADDRESSES)buf.data();
        if (GetAdaptersAddresses(AF_INET, flags, nullptr, aa, &size) == NO_ERROR) {
            for (auto* a = aa; a; a = a->Next) {
                if (a->OperStatus != IfOperStatusUp || a->IfType == IF_TYPE_SOFTWARE_LOOPBACK ||
                    a->IfType == IF_TYPE_TUNNEL)
                    continue;
                if (!a->FirstGatewayAddress || !a->FirstUnicastAddress) continue;
                sockaddr_in* sin = (sockaddr_in*)a->FirstUnicastAddress->Address.lpSockaddr;
                wchar_t tmp[64] = L"";
                if (!sin || !InetNtopW(AF_INET, &sin->sin_addr, tmp, 64)) continue;
                if (a->IfType == IF_TYPE_IEEE80211) {  // the Wi-Fi adapter has priority
                    ip = tmp;
                    break;
                }
                if (ipAny.empty()) ipAny = tmp;
            }
        }
    }
    if (ip.empty()) ip = ipAny;

    Lock l;
    g_d.wifiUp = up;
    g_d.ssid = ssid;
    g_d.wifiSignal = sig;
    g_d.ip = ip;
}

// ------------------------------------------------------------ worker threads --

static std::mutex g_workersMutex;
static std::vector<HANDLE> g_workerThreads;

// Every worker handle is kept so that unloading can wait for all of them.
static void StartWorker(LPTHREAD_START_ROUTINE fn, LPVOID arg) {
    g_workers++;
    HANDLE h = CreateThread(nullptr, 0, fn, arg, 0, nullptr);
    if (!h) {
        g_workers--;
        return;
    }
    std::lock_guard<std::mutex> lock(g_workersMutex);
    for (size_t i = 0; i < g_workerThreads.size();) {  // drop finished threads
        if (WaitForSingleObject(g_workerThreads[i], 0) == WAIT_OBJECT_0) {
            CloseHandle(g_workerThreads[i]);
            g_workerThreads.erase(g_workerThreads.begin() + i);
        } else {
            i++;
        }
    }
    g_workerThreads.push_back(h);
}

static DWORD WINAPI BtWorker(LPVOID) {
    BLUETOOTH_DEVICE_SEARCH_PARAMS p{};
    p.dwSize = sizeof(p);
    p.fReturnAuthenticated = FALSE;
    p.fReturnRemembered = FALSE;
    p.fReturnUnknown = FALSE;
    p.fReturnConnected = TRUE;
    p.fIssueInquiry = FALSE;
    p.cTimeoutMultiplier = 0;
    p.hRadio = nullptr;
    BLUETOOTH_DEVICE_INFO di{};
    di.dwSize = sizeof(di);
    std::wstring names;
    int n = 0;
    HBLUETOOTH_DEVICE_FIND hf = BluetoothFindFirstDevice(&p, &di);
    if (hf) {
        do {
            if (di.fConnected) {
                n++;
                if (!names.empty()) names += L", ";
                names += di.szName[0] ? di.szName : L"Bluetooth";
            }
        } while (BluetoothFindNextDevice(hf, &di));
        BluetoothFindDeviceClose(hf);
    }
    {
        Lock l;
        g_d.btCount = n;
        g_d.btNames = names;
    }
    g_btBusy = false;
    g_workers--;
    return 0;
}

static std::atomic<bool> g_wifiBusy{false};

static DWORD WINAPI WifiWorker(LPVOID) {
    UpdateWifi();
    g_wifiBusy = false;
    g_workers--;
    return 0;
}

// -------------------------------------------------------------------- media --

struct MediaCtx {
    std::vector<std::wstring>* procs;
    std::wstring found;
    HWND hwnd = nullptr;
};

static HWND g_mediaWnd = nullptr;
static ULONGLONG g_mediaScanNext = 0;

static bool IsMediaProc(HWND h, const std::vector<std::wstring>& procs) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    HANDLE ph = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!ph) return false;
    wchar_t path[MAX_PATH];
    DWORD sz = MAX_PATH;
    bool ok = QueryFullProcessImageNameW(ph, 0, path, &sz) != 0;
    CloseHandle(ph);
    if (!ok) return false;
    const wchar_t* base = wcsrchr(path, L'\\');
    base = base ? base + 1 : path;
    std::wstring nm = Lower(base);
    for (auto& p : procs)
        if (nm == p) return true;
    return false;
}

static BOOL CALLBACK MediaEnum(HWND h, LPARAM lp) {
    MediaCtx* c = (MediaCtx*)lp;
    if (!IsWindowVisible(h)) return TRUE;
    wchar_t t[256];
    if (GetWindowTextW(h, t, 256) <= 0 || !wcsstr(t, L" - ")) return TRUE;
    if (!IsMediaProc(h, *c->procs)) return TRUE;
    c->found = t;
    c->hwnd = h;
    return FALSE;
}

static std::wstring CleanTrack(std::wstring s) {
    static const wchar_t* suffixes[] = {L" - VLC media player", L" - YouTube Music", L" - YouTube",
                                        L" - Google Chrome", L" - Microsoft Edge", L" - Mozilla Firefox"};
    for (bool again = true; again;) {
        again = false;
        for (auto* suf : suffixes) {
            size_t n = wcslen(suf);
            if (s.size() > n && s.compare(s.size() - n, n, suf) == 0) {
                s.erase(s.size() - n);
                again = true;
            }
        }
    }
    return s;
}

// The track is detected from the player window title. While music is playing it is
// "Artist - Title"; when paused the title usually has no " - ".
static void UpdateMedia() {
    if (!g_s.showMedia) {
        g_d.track.clear();
        g_mediaWnd = nullptr;
        return;
    }
    std::wstring found;
    if (g_mediaWnd && IsWindow(g_mediaWnd) && IsWindowVisible(g_mediaWnd)) {
        wchar_t t[256];
        if (GetWindowTextW(g_mediaWnd, t, 256) > 0 && wcsstr(t, L" - ")) found = t;
    } else {
        g_mediaWnd = nullptr;
    }
    if (found.empty()) {
        ULONGLONG now = GetTickCount64();
        if (now >= g_mediaScanNext) {  // full window enumeration at most once every 2 seconds
            g_mediaScanNext = now + 2000;
            MediaCtx c{&g_s.mediaProcs, L"", nullptr};
            EnumWindows(MediaEnum, (LPARAM)&c);
            if (c.hwnd) {
                g_mediaWnd = c.hwnd;
                found = c.found;
            }
        }
    }
    found = CleanTrack(found);
    if (found.size() > 48) found = found.substr(0, 47) + L"\u2026";
    g_d.track = found;
}

static void MediaKey(WORD vk) {
    keybd_event((BYTE)vk, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event((BYTE)vk, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

// ------------------------------------------------------------------- timers --

struct Timers {
    int mode = 0;  // 0 = none, 1 = timer, 2 = pomodoro
    ULONGLONG endTick = 0;
    bool paused = false;
    ULONGLONG remain = 0;
    bool pomoWork = true;
    int pomoDone = 0;
    bool swRunning = false;
    ULONGLONG swStart = 0, swAccum = 0;
    bool alarmOn = false;
    int lastAlarmDay = -1;
    ULONGLONG ringUntil = 0;
    std::wstring ringText;
};
static Timers g_t;

static void Ring(const std::wstring& text) {
    g_t.ringText = text;
    g_t.ringUntil = GetTickCount64() + 10000;
}

static void UpdateTimers() {
    ULONGLONG now = GetTickCount64();
    if (g_t.mode != 0 && !g_t.paused && now >= g_t.endTick) {
        if (g_t.mode == 1) {
            g_t.mode = 0;
            Ring(L"\u23F3 Timer finished");
        } else {
            if (g_t.pomoWork) g_t.pomoDone++;
            g_t.pomoWork = !g_t.pomoWork;
            int mins = g_t.pomoWork ? g_s.pomoWork : g_s.pomoBreak;
            g_t.endTick = now + (ULONGLONG)mins * 60000;
            Ring(g_t.pomoWork ? L"Pomodoro: time to work" : L"Pomodoro: break time!");
        }
    }
    if (g_t.alarmOn && g_s.alarmH >= 0) {
        SYSTEMTIME lt;
        GetLocalTime(&lt);
        if (lt.wHour == g_s.alarmH && lt.wMinute == g_s.alarmM && g_t.lastAlarmDay != lt.wDay) {
            g_t.lastAlarmDay = lt.wDay;
            Ring(L"\u23F0 Alarm");
        }
    }
    if (g_t.ringUntil && now < g_t.ringUntil) {
        MessageBeep(MB_ICONEXCLAMATION);
    } else {
        g_t.ringUntil = 0;
    }
}

static std::wstring TimerText(bool* pulse) {
    ULONGLONG now = GetTickCount64();
    if (g_t.ringUntil) {
        *pulse = true;
        return g_t.ringText;
    }
    std::wstring r;
    auto add = [&](const std::wstring& s) {
        if (!r.empty()) r += g_vertical ? L"\n" : L"  ";
        r += s;
    };
    if (g_t.mode != 0) {
        ULONGLONG left = g_t.paused ? g_t.remain : (g_t.endTick > now ? g_t.endTick - now : 0);
        std::wstring t = g_t.paused ? L"\u23F8 " : L"";
        if (g_t.mode == 1) {
            t += L"\u23F3 " + FmtMs(left);
        } else {
            t += (g_t.pomoWork ? L"Work " : L"Rest ") + FmtMs(left);
            if (g_t.pomoDone > 0) t += L" \u00B7" + std::to_wstring(g_t.pomoDone);
        }
        add(t);
    }
    if (g_t.swRunning || g_t.swAccum) {
        ULONGLONG v = g_t.swAccum + (g_t.swRunning ? now - g_t.swStart : 0);
        add(L"\u23F1 " + FmtMs(v));
    }
    if (g_t.alarmOn && g_s.alarmH >= 0) {
        wchar_t b[16];
        swprintf(b, 16, L"\u23F0 %02d:%02d", g_s.alarmH, g_s.alarmM);
        add(b);
    }
    if (r.empty()) r = L"\u23F1";
    return r;
}

// ------------------------------------------------------------- content model --

static std::wstring BuildClockText() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    LPCWSTR loc = LocaleName();
    std::wstring pat = g_s.customTime;
    if (pat.empty()) {
        if (g_s.hourFormat == L"h24")
            pat = g_s.showSeconds ? L"HH:mm:ss" : L"HH:mm";
        else if (g_s.hourFormat == L"h12")
            pat = g_s.showSeconds ? L"h:mm:ss tt" : L"h:mm tt";
    }
    wchar_t t[96] = L"", wd[64] = L"", d[96] = L"";
    if (pat.empty())
        GetTimeFormatEx(loc, g_s.showSeconds ? 0 : TIME_NOSECONDS, &st, nullptr, t, ARRAYSIZE(t));
    else
        GetTimeFormatEx(loc, 0, &st, pat.c_str(), t, ARRAYSIZE(t));
    GetDateFormatEx(loc, 0, &st, L"dddd", wd, ARRAYSIZE(wd), nullptr);
    GetDateFormatEx(loc, 0, &st, g_s.dateFormat.c_str(), d, ARRAYSIZE(d), nullptr);

    const std::wstring& f = g_s.clockFormat;
    const wchar_t* sep = g_vertical ? L"\n" : L"  ";
    std::wstring r = t;
    if (f == L"timeWeekday")
        r += sep + std::wstring(wd);
    else if (f == L"timeDate")
        r += sep + std::wstring(d);
    else if (f == L"timeWeekdayDate")
        r += sep + std::wstring(wd) + (g_vertical ? L"\n" : L", ") + d;
    return r;
}

// returns true if the content changed
static bool BuildContent() {
    std::vector<Elem> L, R;
    const wchar_t* sep = g_vertical ? L"\n" : L"    ";
    std::wstring btNames, track, ssid, ip;
    int btCount, wifiSignal, volPct;
    bool wifiUp, volMuted;
    {
        Lock l;
        volPct = g_d.volPct;
        volMuted = g_d.volMuted;
        btNames = g_d.btNames;
        btCount = g_d.btCount;
        track = g_d.track;
        wifiUp = g_d.wifiUp;
        ssid = g_d.ssid;
        ip = g_d.ip;
        wifiSignal = g_d.wifiSignal;
    }
    bool pulseAny = false;

    for (size_t i = 0; i < g_pins.size(); i++) {
        Elem e;
        e.act = A_PIN;
        e.idx = (int)i;
        bool showIcon = g_s.pinDisplay != L"name";
        bool showName = g_s.pinDisplay != L"icon";
        if (showIcon) e.icon = g_pins[i].icon;
        if (!e.icon) showName = true;  // no icon - show at least the name
        if (showName) {
            std::wstring nm = g_pins[i].name;
            if ((int)nm.size() > g_s.pinNameLen) nm = nm.substr(0, g_s.pinNameLen) + L"\u2026";
            e.text = nm;
        }
        L.push_back(e);
    }
    if (g_s.showMedia) {
        Elem e;
        e.act = A_MPREV;
        e.glyph = L"\uE892";
        L.push_back(e);
        e = Elem();
        e.act = A_MPLAY;
        e.glyph = track.empty() ? L"\uE768" : L"\uE769";
        L.push_back(e);
        e = Elem();
        e.act = A_MNEXT;
        e.glyph = L"\uE893";
        L.push_back(e);
        if (!track.empty()) {
            e = Elem();
            e.act = A_MTRACK;
            e.text = track;
            L.push_back(e);
        }
    }

    if (g_s.showCpu || g_s.showMem || g_s.showNet) {
        std::wstring t;
        wchar_t buf[48];
        auto add = [&](const std::wstring& s) {
            if (!t.empty()) t += sep;
            t += s;
        };
        if (g_s.showCpu) {
            swprintf(buf, 48, L"CPU %d%%", g_d.cpu);
            add(buf);
        }
        if (g_s.showMem) {
            swprintf(buf, 48, L"RAM %d%%", g_d.mem);
            add(buf);
        }
        if (g_s.showNet) add(L"\u2193 " + FmtSpeed(g_d.down) + L"  \u2191 " + FmtSpeed(g_d.up));
        Elem e;
        e.act = A_STATS;
        e.text = t;
        R.push_back(e);
    }
    // --- replacement for the right part of the taskbar: hidden icons, menu, language, network, sound, battery
    if (g_s.showTray) {
        Elem e;
        e.act = A_TRAY;
        e.glyph = L"\uE70E";
        R.push_back(e);
    }
    if (g_s.showMenu) {
        Elem e;
        e.act = A_MENU;
        e.glyph = L"\uE700";  // three horizontal bars
        R.push_back(e);
    }
    if (g_s.showLang && !g_d.lang.empty()) {
        Elem e;
        e.act = A_LANG;
        e.text = g_d.lang;
        R.push_back(e);
    }
    if (g_s.showWifi && (wifiUp || !ip.empty())) {
        Elem e;
        e.act = A_WIFI;
        e.glyph = wifiUp ? L"\uE701" : L"\uE839";
        std::wstring t;
        auto add = [&](const std::wstring& s) {
            if (s.empty()) return;
            if (!t.empty()) t += g_vertical ? L"\n" : L" \u00B7 ";
            t += s;
        };
        if (wifiUp) {
            if (g_s.wifiName) add(ssid);
            if (g_s.wifiSignal) {
                wchar_t b[16];
                swprintf(b, 16, L"%d%%", wifiSignal);
                add(b);
            }
        }
        if (g_s.wifiIp) add(ip);
        e.text = t;
        R.push_back(e);
    }
    if (g_s.showBt && btCount > 0) {
        Elem e;
        e.act = A_BT;
        e.glyph = L"\uE702";
        if (g_s.btNames) {
            size_t comma = btNames.find(L',');
            e.text = comma == std::wstring::npos ? btNames : btNames.substr(0, comma);
            if (btCount > 1) e.text += L" +" + std::to_wstring(btCount - 1);
        } else {
            e.text = std::to_wstring(btCount);
        }
        R.push_back(e);
    }
    if (g_s.showVolume && volPct >= 0) {
        Elem e;
        e.act = A_VOLUME;
        int v = volPct;
        if (volMuted)
            e.glyph = L"\uE74F";
        else if (v == 0)
            e.glyph = L"\uE992";
        else if (v < 34)
            e.glyph = L"\uE993";
        else if (v < 67)
            e.glyph = L"\uE994";
        else
            e.glyph = L"\uE995";
        e.text = volMuted ? std::wstring(L"0%") : std::to_wstring(v) + L"%";
        R.push_back(e);
    }
    if (g_s.showBattery && g_d.hasBattery && g_d.batPct >= 0) {
        Elem e;
        e.act = A_BATTERY;
        int lvl = g_d.batPct / 10;
        e.glyph = lvl >= 10 ? L"\uE83F" : std::wstring(1, (wchar_t)(0xE850 + lvl));
        e.text = (g_d.charging ? L"\u26A1" : L"") + std::to_wstring(g_d.batPct) + L"%";
        e.pulse = g_s.animations && g_d.batPct <= 15 && !g_d.charging && !g_d.ac;
        pulseAny |= e.pulse;
        R.push_back(e);
    }
    if (g_s.showTimer) {
        Elem e;
        e.act = A_TIMER;
        bool pulse = false;
        e.text = TimerText(&pulse);
        e.pulse = pulse && g_s.animations;
        pulseAny |= e.pulse;
        R.push_back(e);
    }
    if (g_s.showBell) {
        Elem e;
        e.act = A_BELL;
        e.glyph = L"\uEA8F";
        if (g_s.notifCounter && g_notifCount > 0) {
            e.text = g_notifCount > 99 ? std::wstring(L"99+") : std::to_wstring(g_notifCount);
            e.accent = true;
        }
        R.push_back(e);
    }
    for (size_t i = 0; i < g_rightApps.size(); i++) {
        Elem e;
        e.act = A_RIGHTAPP;
        e.idx = (int)i;
        e.icon = g_rightApps[i].icon;
        R.push_back(e);
    }
    if (!g_s.rightText.empty() || g_customIcon) {
        Elem e;
        e.act = A_CUSTOM;
        e.text = g_s.rightText;
        e.icon = g_customIcon;
        R.push_back(e);
    }
    {   // feature order: saved (via drag and drop / menu)
        auto rank = [&](const Elem& e) {
            std::wstring t = TokenOf(e.act);
            for (size_t i = 0; i < g_rightOrder.size(); i++)
                if (g_rightOrder[i] == t) return (int)i;
            return 1000;
        };
        std::stable_sort(R.begin(), R.end(), [&](const Elem& a, const Elem& b) { return rank(a) < rank(b); });
    }

    Elem C;
    C.act = A_CLOCK;
    C.text = BuildClockText();

    // Win button
    int startStyle = 0;
    if (g_s.winButton == L"win7") startStyle = 1;
    else if (g_s.winButton == L"win10") startStyle = 2;
    else if (g_s.winButton == L"win11") startStyle = 3;
    else if (g_s.winButton == L"linux") startStyle = 4;
    else if (g_s.winButton == L"apple") startStyle = 5;
    Elem S;
    if (startStyle) {
        S.act = A_START;
        S.idx = startStyle;
    }

    uint64_t h = 1469598103934665603ULL;
    auto hashList = [&](const std::vector<Elem>& v) {
        for (auto& e : v) {
            h = HashStr(h, e.glyph);
            h = HashStr(h, e.text);
            h ^= (uint64_t)e.act * 31 + (uint64_t)e.idx * 7 + (uint64_t)(uintptr_t)e.icon + (e.pulse ? 1 : 0) + (e.accent ? 2 : 0);
            h *= 1099511628211ULL;
        }
    };
    hashList(L);
    hashList(R);
    h = HashStr(h, C.text);
    h ^= (uint64_t)startStyle * 131;
    h *= 1099511628211ULL;

    g_left.swap(L);
    g_right.swap(R);
    g_clock = C;
    g_hasClock = true;
    g_start = S;
    g_hasStart = startStyle != 0;
    g_pulseActive = pulseAny;
    bool changed = h != g_sig;
    g_sig = h;
    return changed;
}

// ---------------------------------------------------------------- bar drawing --

// Area of the bar itself inside the window: left/right/top/bottom margins.
static RECT BodyRect(const Bar* b, int w, int h) {
    return RECT{Sc(b, g_s.mLeft), Sc(b, g_s.mTop), w - Sc(b, g_s.mRight), h - Sc(b, g_s.mBottom)};
}

static void BuildFonts(Bar* b) {
    if (b->fText) DeleteObject(b->fText);
    if (b->fClock) DeleteObject(b->fClock);
    if (b->fIcon) DeleteObject(b->fIcon);
    if (b->fSym) DeleteObject(b->fSym);
    if (b->fCustom) DeleteObject(b->fCustom);
    auto mk = [&](int pt, bool bold, const wchar_t* face) {
        return CreateFontW(-MulDiv(pt, (int)b->dpi, 72), 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, FALSE,
                           FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, face);
    };
    b->fText = mk(g_s.fontSize, false, g_s.font.c_str());
    b->fClock = mk(g_s.clockFontSize, g_s.clockBold, g_s.font.c_str());
    b->fIcon = mk(g_s.fontSize + 1, false, L"Segoe MDL2 Assets");
    b->fSym = mk(g_s.fontSize + 1, false, L"Segoe UI Symbol");
    b->fCustom = mk(g_s.rightTextSize, false, g_s.font.c_str());
}

static bool EnsureDib(Bar* b, int w, int h) {
    if (b->bmp && b->bw == w && b->bh == h) return true;
    if (!b->mem) {
        HDC scr = GetDC(nullptr);
        b->mem = CreateCompatibleDC(scr);
        ReleaseDC(nullptr, scr);
    }
    if (b->bmp) {
        SelectObject(b->mem, b->oldBmp);
        DeleteObject(b->bmp);
        b->bmp = nullptr;
    }
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    b->bmp = CreateDIBSection(b->mem, &bi, DIB_RGB_COLORS, &b->bits, nullptr, 0);
    if (!b->bmp) return false;
    b->oldBmp = SelectObject(b->mem, b->bmp);
    b->bw = w;
    b->bh = h;
    return true;
}

static void MeasureElem(Bar* b, HDC dc, const Elem& e, int maxW, SIZE* out) {
    int isz = Sc(b, g_s.iconSize);
    out->cx = out->cy = 0;
    if (e.act == A_RIGHTAPP || e.act == A_START) {
        out->cx = out->cy = isz;
        return;
    }
    if (e.act == A_CUSTOM) {
        int ih = e.icon ? Sc(b, g_s.rightImageSize) : 0;
        int iw = (ih && g_customH > 0) ? MulDiv(ih, g_customW, g_customH) : 0;
        SIZE ts{0, 0};
        if (!e.text.empty()) {
            SelectObject(dc, b->fCustom);
            RECT r{0, 0, 100000, 0};
            DrawTextW(dc, e.text.c_str(), -1, &r, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
            ts.cx = r.right;
            ts.cy = r.bottom;
        }
        out->cx = iw + ((iw && ts.cx) ? Sc(b, 6) : 0) + ts.cx;
        out->cy = std::max(ih, (int)ts.cy);
        return;
    }
    if (e.act == A_PIN) {
        bool hasIcon = e.icon != nullptr;
        int tw = 0, th = 0;
        if (!e.text.empty()) {
            SelectObject(dc, b->fText);
            RECT r{0, 0, 100000, 0};
            DrawTextW(dc, e.text.c_str(), -1, &r, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
            tw = r.right;
            th = r.bottom;
        }
        out->cx = (hasIcon ? isz : 0) + ((hasIcon && tw) ? Sc(b, 6) : 0) + tw;
        out->cy = std::max(hasIcon ? isz : 0, th);
        if (out->cx == 0) out->cx = out->cy = isz;
        return;
    }
    SIZE gs{0, 0}, ts{0, 0};
    if (!e.glyph.empty()) {
        SelectObject(dc, e.font == 1 ? b->fSym : b->fIcon);
        GetTextExtentPoint32W(dc, e.glyph.c_str(), (int)e.glyph.size(), &gs);
    }
    if (!e.text.empty()) {
        SelectObject(dc, e.act == A_CLOCK ? b->fClock : b->fText);
        RECT r{0, 0, maxW, 0};
        UINT f = DT_CALCRECT | DT_NOPREFIX | (g_vertical ? DT_WORDBREAK : DT_SINGLELINE);
        DrawTextW(dc, e.text.c_str(), -1, &r, f);
        ts.cx = r.right;
        ts.cy = r.bottom;
    }
    int gap = (gs.cx && ts.cx) ? Sc(b, 6) : 0;
    out->cx = gs.cx + gap + ts.cx;
    out->cy = std::max(gs.cy, ts.cy);
}

// Layout: the Win button is always in the far left corner, the rest depends on the mode.
static void DoLayout(Bar* b, HDC dc, const RECT& body) {
    b->rl.assign(g_left.size(), RECT{});
    b->rr.assign(g_right.size(), RECT{});
    b->rStart = RECT{};
    int pad = Sc(b, g_s.padding), sp = Sc(b, g_s.itemSpacing);
    SIZE s;
    int yo = Sc(b, g_s.yOffset);
    int cy = (body.top + body.bottom) / 2 + yo;
    // center app icons vertically, everything else spans the full bar height
    // (text with DT_VCENTER is centered exactly, regardless of the font)
    auto place = [&](int x0, int x1, const Elem& e, int h) {
        if (e.act == A_PIN || e.act == A_RIGHTAPP || e.act == A_CUSTOM || e.act == A_START)
            return RECT{x0, cy - h / 2, x1, cy - h / 2 + h};
        return RECT{x0, body.top + yo, x1, body.bottom + yo};
    };
    int lx = body.left + pad;
    int rx = body.right - pad;
    if (g_hasStart) {
        MeasureElem(b, dc, g_start, 100000, &s);
        b->rStart = place(lx, lx + s.cx, g_start, s.cy);
        lx += s.cx + sp;
    }
    auto layL = [&](const std::vector<Elem>& v, std::vector<RECT>& out) {
        for (size_t i = 0; i < v.size(); i++) {
            MeasureElem(b, dc, v[i], 100000, &s);
            out[i] = place(lx, lx + s.cx, v[i], s.cy);
            lx += s.cx + sp;
        }
    };
    auto layR = [&](const std::vector<Elem>& v, std::vector<RECT>& out) {
        for (size_t k = v.size(); k-- > 0;) {
            MeasureElem(b, dc, v[k], 100000, &s);
            out[k] = place(rx - s.cx, rx, v[k], s.cy);
            rx -= s.cx + sp;
        }
    };
    bool funcLeft = g_s.layout == L"funcLeft";
    bool clockRight = g_s.layout == L"clockRight";

    // in clockRight mode the clock takes the far right edge - place it BEFORE the features
    MeasureElem(b, dc, g_clock, 100000, &s);
    if (clockRight) {
        b->rClock = place(rx - s.cx, rx, g_clock, s.cy);
        rx -= s.cx + sp;
    } else {
        int cx = (body.left + body.right) / 2;
        b->rClock = place(cx - s.cx / 2, cx - s.cx / 2 + s.cx, g_clock, s.cy);
    }

    if (funcLeft) {
        layL(g_right, b->rr);
        layR(g_left, b->rl);
    } else {
        layL(g_left, b->rl);
        layR(g_right, b->rr);
    }
}

static int KeyOf(const Elem& e) { return (int)e.act * 1000 + e.idx; }

static void PolyFill(HDC dc, COLORREF c, const POINT* p, int n) {
    HBRUSH br = CreateSolidBrush(c);
    HGDIOBJ ob = SelectObject(dc, br);
    HGDIOBJ op = SelectObject(dc, GetStockObject(NULL_PEN));
    Polygon(dc, p, n);
    SelectObject(dc, op);
    SelectObject(dc, ob);
    DeleteObject(br);
}

static void EllFill(HDC dc, COLORREF c, int l, int t, int r, int bt) {
    HBRUSH br = CreateSolidBrush(c);
    HGDIOBJ ob = SelectObject(dc, br);
    HGDIOBJ op = SelectObject(dc, GetStockObject(NULL_PEN));
    Ellipse(dc, l, t, r + 1, bt + 1);
    SelectObject(dc, op);
    SelectObject(dc, ob);
    DeleteObject(br);
}

static void RectFill(HDC dc, COLORREF c, int l, int t, int r, int bt) {
    HBRUSH br = CreateSolidBrush(c);
    RECT rc{l, t, r, bt};
    FillRect(dc, &rc, br);
    DeleteObject(br);
}

// Win button logos are drawn with shapes.
// 1 - Windows 7, 2 - Windows 10, 3 - Windows 11, 4 - Linux, 5 - Apple.
static void DrawStartLogoRaw(HDC dc, int style, const RECT& rc, COLORREF col) {
    int S = rc.right - rc.left;
    int X = rc.left, Y = rc.top;
    auto fx = [&](double f) { return X + (int)(f * S + 0.5); };
    auto fy = [&](double f) { return Y + (int)(f * S + 0.5); };
    // ellipse by center and radii (as fractions of the size)
    auto ell = [&](COLORREF c, double cx, double cy, double rx, double ry) {
        EllFill(dc, c, fx(cx - rx), fy(cy - ry), fx(cx + rx), fy(cy + ry));
    };
    switch (style) {
        case 1: {  // Windows 7: four colored panes, a "waving flag"
            const COLORREF cols[4] = {RGB(244, 83, 37), RGB(129, 188, 6), RGB(5, 166, 240), RGB(255, 186, 8)};
            const double g = 0.03;
            auto sh = [](double x) { return (0.5 - x) * 0.16; };
            for (int r = 0; r < 2; r++) {
                for (int c = 0; c < 2; c++) {
                    double xa = c == 0 ? 0.06 : 0.5 + g, xb = c == 0 ? 0.5 - g : 0.94;
                    double ya = r == 0 ? 0.10 : 0.5 + g, yb = r == 0 ? 0.5 - g : 0.90;
                    POINT p[4] = {{fx(xa), fy(ya + sh(xa))}, {fx(xb), fy(ya + sh(xb))},
                                  {fx(xb), fy(yb + sh(xb))}, {fx(xa), fy(yb + sh(xa))}};
                    PolyFill(dc, cols[r * 2 + c], p, 4);
                }
            }
            break;
        }
        case 2:  // Windows 10: four panes in perspective
            RectFill(dc, col, fx(0.06), fy(0.16), fx(0.47), fy(0.47));
            RectFill(dc, col, fx(0.06), fy(0.53), fx(0.47), fy(0.84));
            RectFill(dc, col, fx(0.53), fy(0.06), fx(0.94), fy(0.47));
            RectFill(dc, col, fx(0.53), fy(0.53), fx(0.94), fy(0.94));
            break;
        case 3: {  // Windows 11: four squares, sizes are computed in pixels (gap is always >= 1 px)
            int m = std::max(1, S / 10);
            int gap = std::max(1, S / 12);
            int cell = (S - 2 * m - gap) / 2;
            if (cell < 1) cell = 1;
            int total = cell * 2 + gap;
            int x0 = X + (S - total) / 2, y0 = Y + (S - total) / 2;
            RectFill(dc, col, x0, y0, x0 + cell, y0 + cell);
            RectFill(dc, col, x0 + cell + gap, y0, x0 + total, y0 + cell);
            RectFill(dc, col, x0, y0 + cell + gap, x0 + cell, y0 + total);
            RectFill(dc, col, x0 + cell + gap, y0 + cell + gap, x0 + total, y0 + total);
            break;
        }
        case 4: {  // Linux: Tux the penguin (transparent "holes" - belly and eyes)
            COLORREF orange = RGB(250, 178, 0);
            COLORREF hole = (COLORREF)kKey;  // transparent color
            ell(col, 0.50, 0.64, 0.29, 0.34);    // body
            ell(col, 0.20, 0.62, 0.09, 0.22);    // left wing
            ell(col, 0.80, 0.62, 0.09, 0.22);    // right wing
            ell(col, 0.50, 0.25, 0.20, 0.22);    // head
            ell(hole, 0.50, 0.68, 0.18, 0.24);   // belly
            ell(hole, 0.42, 0.20, 0.060, 0.075); // eyes
            ell(hole, 0.58, 0.20, 0.060, 0.075);
            ell(col, 0.43, 0.21, 0.025, 0.035);  // pupils
            ell(col, 0.57, 0.21, 0.025, 0.035);
            ell(orange, 0.50, 0.31, 0.10, 0.045); // beak
            ell(orange, 0.31, 0.94, 0.15, 0.055); // feet
            ell(orange, 0.69, 0.94, 0.15, 0.055);
            break;
        }
        case 5: {  // Apple: an apple with a bite and a leaf
            ell(col, 0.37, 0.50, 0.25, 0.25);
            ell(col, 0.63, 0.50, 0.25, 0.25);
            ell(col, 0.38, 0.68, 0.26, 0.26);
            ell(col, 0.62, 0.68, 0.26, 0.26);
            RectFill(dc, col, fx(0.37), fy(0.32), fx(0.63), fy(0.90));
            ell((COLORREF)kKey, 0.93, 0.40, 0.14, 0.15);  // bite (transparent)
            POINT leaf[6] = {{fx(0.49), fy(0.27)}, {fx(0.52), fy(0.14)}, {fx(0.60), fy(0.06)},
                             {fx(0.72), fy(0.03)}, {fx(0.68), fy(0.14)}, {fx(0.58), fy(0.24)}};
            PolyFill(dc, col, leaf, 6);
            break;
        }
    }
}

// Linux and Apple: draw 4x larger and downscale (edge smoothing).
static void DrawStartLogoAA(HDC dc, int style, const RECT& rc, COLORREF col) {
    const int K = 4;
    int S = rc.right - rc.left;
    if (S <= 0) return;
    int B = S * K;
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = B;
    bi.bmiHeader.biHeight = -B;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC mdc = CreateCompatibleDC(dc);
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(mdc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        DeleteDC(mdc);
        return;
    }
    HGDIOBJ old = SelectObject(mdc, bmp);
    DWORD* px = (DWORD*)bits;
    std::fill(px, px + (size_t)B * B, kKey);
    RECT big{0, 0, B, B};
    DrawStartLogoRaw(mdc, style, big, col);
    GdiFlush();
    for (int y = 0; y < S; y++) {
        for (int x = 0; x < S; x++) {
            int r = 0, g = 0, bl = 0, n = 0;
            for (int j = 0; j < K; j++) {
                for (int i = 0; i < K; i++) {
                    DWORD p = px[(size_t)(y * K + j) * B + (x * K + i)] & 0x00FFFFFF;
                    if (p == kKey) continue;
                    r += (p >> 16) & 255;
                    g += (p >> 8) & 255;
                    bl += p & 255;
                    n++;
                }
            }
            if (!n) continue;
            COLORREF src = RGB(r / n, g / n, bl / n);
            COLORREF d = GetPixel(dc, rc.left + x, rc.top + y);
            SetPixel(dc, rc.left + x, rc.top + y, Blend(d, src, n * 256 / (K * K)));
        }
    }
    SelectObject(mdc, old);
    DeleteObject(bmp);
    DeleteDC(mdc);
}

static void DrawStartLogo(HDC dc, int style, const RECT& rc, COLORREF col) {
    if (style == 4 || style == 5)
        DrawStartLogoAA(dc, style, rc, col);
    else
        DrawStartLogoRaw(dc, style, rc, col);
}

static void DrawElem(Bar* b, HDC dc, const Elem& e, const RECT& rc, COLORREF col) {
    int isz = Sc(b, g_s.iconSize);
    if (e.act == A_START) {
        DrawStartLogo(dc, e.idx, rc, col);
        return;
    }
    if (e.act == A_RIGHTAPP) {
        if (e.icon) DrawIconEx(dc, rc.left, rc.top, e.icon, isz, isz, 0, nullptr, DI_NORMAL);
        return;
    }
    if (e.act == A_CUSTOM) {
        int ih = e.icon ? Sc(b, g_s.rightImageSize) : 0;
        int iw = (ih && g_customH > 0) ? MulDiv(ih, g_customW, g_customH) : 0;
        int x = rc.left;
        if (e.icon && iw) {
            int iy = rc.top + ((rc.bottom - rc.top) - ih) / 2;
            DrawIconEx(dc, x, iy, e.icon, iw, ih, 0, nullptr, DI_NORMAL);
            x += iw + (e.text.empty() ? 0 : Sc(b, 6));
        }
        if (!e.text.empty()) {
            SetTextColor(dc, col);
            SelectObject(dc, b->fCustom);
            RECT tr{x, rc.top, rc.right, rc.bottom};
            DrawTextW(dc, e.text.c_str(), -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }
        return;
    }
    if (e.act == A_PIN) {
        int x = rc.left;
        if (e.icon) {
            int iy = rc.top + ((rc.bottom - rc.top) - isz) / 2;
            DrawIconEx(dc, x, iy, e.icon, isz, isz, 0, nullptr, DI_NORMAL);
            x += isz + (e.text.empty() ? 0 : Sc(b, 6));
        }
        if (!e.text.empty()) {
            SetTextColor(dc, col);
            SelectObject(dc, b->fText);
            RECT tr{x, rc.top, rc.right, rc.bottom};
            DrawTextW(dc, e.text.c_str(), -1, &tr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        }
        return;
    }
    int x = rc.left;
    SetTextColor(dc, col);
    if (!e.glyph.empty()) {
        SelectObject(dc, e.font == 1 ? b->fSym : b->fIcon);
        SIZE gs{};
        GetTextExtentPoint32W(dc, e.glyph.c_str(), (int)e.glyph.size(), &gs);
        RECT gr{x, rc.top, x + gs.cx, rc.bottom};
        DrawTextW(dc, e.glyph.c_str(), -1, &gr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        x += gs.cx + (e.text.empty() ? 0 : Sc(b, 6));
    }
    if (!e.text.empty()) {
        SelectObject(dc, e.act == A_CLOCK ? b->fClock : b->fText);
        RECT tr{x, rc.top, rc.right, rc.bottom};
        UINT f = DT_NOPREFIX;
        if (g_vertical)
            f |= DT_WORDBREAK | (e.act == A_CLOCK ? DT_CENTER : DT_LEFT);
        else
            f |= DT_SINGLELINE | DT_VCENTER | (e.act == A_CLOCK ? DT_CENTER : DT_LEFT);
        DrawTextW(dc, e.text.c_str(), -1, &tr, f);
    }
}

static bool DragMatch(int kind, Act a) {
    if (kind == 2) return a != A_NONE;
    return a == (kind == 1 ? A_RIGHTAPP : A_PIN);
}

static void DropPos(Bar* b, int* outCoord) {
    // insertion coordinate along the bar axis for b->dropIdx
    *outCoord = 0;
    int pinPos = 0;
    int last = 0;
    int kind = b->dragKind;
    bool rt = kind >= 1;
    const std::vector<Elem>& lst = rt ? g_right : g_left;
    const std::vector<RECT>& rcs = rt ? b->rr : b->rl;
    int sp = Sc(b, g_s.itemSpacing) / 2;
    for (size_t i = 0; i < lst.size() && i < rcs.size(); i++) {
        if (!DragMatch(kind, lst[i].act)) continue;
        const RECT& r = rcs[i];
        if (pinPos == b->dropIdx) {
            *outCoord = r.left - sp;
            return;
        }
        last = r.right + sp;
        pinPos++;
    }
    if (pinPos == 0) {
        RECT body = BodyRect(b, b->pos.right - b->pos.left, b->pos.bottom - b->pos.top);
        int pad = Sc(b, g_s.padding);
        bool onRight = (rt != (g_s.layout == L"funcLeft"));
        if (onRight)
            last = (g_s.layout == L"clockRight") ? b->rClock.left - Sc(b, g_s.itemSpacing) : body.right - pad;
        else
            last = g_hasStart ? b->rStart.right + Sc(b, g_s.itemSpacing) : body.left + pad;
    }
    *outCoord = last;
}

static void RenderBar(Bar* b) {
    if (!b->hwnd || !b->visible) return;
    int w = b->pos.right - b->pos.left, h = b->pos.bottom - b->pos.top;
    if (w <= 0 || h <= 0 || !EnsureDib(b, w, h)) return;

    DWORD* px = (DWORD*)b->bits;
    std::fill(px, px + (size_t)w * h, kKey);
    HDC dc = b->mem;
    SelectObject(dc, GetStockObject(NULL_PEN));
    SelectObject(dc, GetStockObject(DC_BRUSH));
    SetBkMode(dc, TRANSPARENT);

    int rad = Sc(b, g_s.radius);
    RECT body = BodyRect(b, w, h);
    bool fxOn = b->fx != nullptr;

    int S = Sc(b, g_s.shadowSize);
    if (S > 60) S = 60;
    if (S > 0 && g_s.shadowOpacity > 0) {
        for (int k = S; k >= 1; k--) {
            SetDCBrushColor(dc, RGB(k, 1, 2));
            RoundRect(dc, body.left - k, body.top - k, body.right + k + 1, body.bottom + k + 1,
                      (rad + k) * 2, (rad + k) * 2);
        }
    }
    int bwid = Sc(b, g_s.borderWidth);
    if (bwid > 0) {
        SetDCBrushColor(dc, g_borderColor);
        RoundRect(dc, body.left, body.top, body.right + 1, body.bottom + 1, rad * 2, rad * 2);
        SetDCBrushColor(dc, g_bgColor);
        int ir = std::max(0, rad - bwid);
        RoundRect(dc, body.left + bwid, body.top + bwid, body.right - bwid + 1, body.bottom - bwid + 1,
                  ir * 2, ir * 2);
    } else {
        SetDCBrushColor(dc, g_bgColor);
        RoundRect(dc, body.left, body.top, body.right + 1, body.bottom + 1, rad * 2, rad * 2);
    }

    DoLayout(b, dc, body);

    float phase = 0.5f + 0.5f * sinf((float)(GetTickCount64() % 1200) / 1200.0f * 6.2832f);
    int ph256 = (int)(phase * 256);
    int pad = Sc(b, g_s.itemSpacing) / 2 + 2;

    auto drawList = [&](const std::vector<Elem>& v, const std::vector<RECT>& rects, int listId) {
        for (size_t i = 0; i < v.size() && i < rects.size(); i++) {
            const Elem& e = v[i];
            if (b->dragging) {
                if (b->dragKind == 0 && listId == 0 && e.act == A_PIN && e.idx == b->dragIdx) continue;
                if (b->dragKind == 1 && listId == 1 && e.act == A_RIGHTAPP && e.idx == b->dragIdx) continue;
                if (b->dragKind == 2 && listId == 1 && (int)i == b->dragIdx) continue;
            }
            RECT rc = rects[i];
            if (e.act != A_NONE) {
                auto it = g_hover.find(KeyOf(e));
                if (it != g_hover.end() && it->second > 0.01f) {
                    COLORREF hc = Blend(g_bgColor, g_textColor, (int)(it->second * 48));
                    SetDCBrushColor(dc, hc);
                    RECT hr = rc;
                    if (!g_vertical) {
                        // text items have a rect spanning the full bar height - limit the highlight
                        int hh = std::min((int)(rc.bottom - rc.top), Sc(b, g_s.iconSize) + Sc(b, 8));
                        int mid = (rc.top + rc.bottom) / 2;
                        hr.top = mid - hh / 2;
                        hr.bottom = mid + hh / 2;
                    }
                    RoundRect(dc, hr.left - pad, hr.top - pad / 2, hr.right + pad + 1, hr.bottom + pad / 2 + 1,
                              Sc(b, 8), Sc(b, 8));
                }
            }
            COLORREF col = e.pulse ? Blend(g_textColor, g_accent, ph256) : (e.accent ? g_accent : g_textColor);
            DrawElem(b, dc, e, rc, col);
        }
    };
    drawList(g_left, b->rl, 0);
    drawList(g_right, b->rr, 1);
    if (g_hasClock) {
        std::vector<Elem> cv{g_clock};
        std::vector<RECT> cr{b->rClock};
        drawList(cv, cr, 2);
    }
    if (g_hasStart) {
        std::vector<Elem> sv{g_start};
        std::vector<RECT> sr{b->rStart};
        drawList(sv, sr, 3);
    }

    bool dragValid = false;
    if (b->dragging && b->dragIdx >= 0) {
        if (b->dragKind == 2)
            dragValid = b->dragIdx < (int)g_right.size() && b->dragIdx < (int)b->rr.size();
        else
            dragValid = b->dragIdx < (int)(b->dragKind == 1 ? g_rightApps.size() : g_pins.size());
    }
    if (dragValid || b->extDrag) {
        int isz = Sc(b, g_s.iconSize);
        if (dragValid && b->dragKind == 2) {
            RECT r = b->rr[b->dragIdx];
            int rw = r.right - r.left, rh = r.bottom - r.top;
            RECT gr{b->dragPt.x - rw / 2, b->dragPt.y - rh / 2, b->dragPt.x - rw / 2 + rw, b->dragPt.y - rh / 2 + rh};
            DrawElem(b, dc, g_right[b->dragIdx], gr, g_textColor);
        } else {
            HICON dicon = nullptr;
            if (!b->extDrag && dragValid) dicon = b->dragKind == 1 ? g_rightApps[b->dragIdx].icon : g_pins[b->dragIdx].icon;
            if (dicon)
                DrawIconEx(dc, b->dragPt.x - isz / 2, b->dragPt.y - isz / 2, dicon, isz, isz,
                           0, nullptr, DI_NORMAL);
        }
        int c;
        DropPos(b, &c);
        SetDCBrushColor(dc, g_accent);
        SelectObject(dc, GetStockObject(DC_BRUSH));
        SelectObject(dc, GetStockObject(NULL_PEN));
        int t = Sc(b, 2);
        Rectangle(dc, c - t / 2, body.top + Sc(b, 4), c + t, body.bottom - Sc(b, 4));
    }

    // alpha channel
    DWORD bgRgb = ToDib(g_bgColor);
    DWORD op = (DWORD)g_s.opacity;
    DWORD bgPremul = (op << 24) | (((GetRValue(g_bgColor) * op) / 255) << 16) |
                     (((GetGValue(g_bgColor) * op) / 255) << 8) | ((GetBValue(g_bgColor) * op) / 255);
    DWORD shadowA = (DWORD)g_s.shadowOpacity;
    size_t total = (size_t)w * h;
    for (size_t i = 0; i < total; i++) {
        DWORD p = px[i] & 0x00FFFFFF;
        if (p == kKey) {
            px[i] = 0;
        } else if (p == bgRgb) {
            px[i] = fxOn ? 0x01000000 : bgPremul;  // alpha=1: visually transparent, but still catches the mouse
        } else if (S > 0 && ((p >> 8) & 255) == 1 && (p & 255) == 2 && ((p >> 16) & 255) >= 1 &&
                   ((p >> 16) & 255) <= (DWORD)S) {
            DWORD k = (p >> 16) & 255;
            DWORD a = shadowA * ((DWORD)S - k + 1) / ((DWORD)S + 1);
            px[i] = a << 24;
        } else {
            px[i] = p | 0xFF000000;
        }
    }

    POINT ptDst{b->pos.left, b->pos.top};
    SIZE sz{w, h};
    POINT ptSrc{0, 0};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, (BYTE)g_fade, AC_SRC_ALPHA};
    HDC scr = GetDC(nullptr);
    if (!UpdateLayeredWindow(b->hwnd, scr, &ptDst, &sz, dc, &ptSrc, 0, &bf, ULW_ALPHA))
        Wh_Log(L"UpdateLayeredWindow failed: %lu", GetLastError());
    ReleaseDC(nullptr, scr);
}

static void FlushBars() {
    for (auto* b : g_bars) {
        if (b->dirty && b->visible) {
            b->dirty = false;
            RenderBar(b);
        }
    }
}

static void MarkAll() {
    for (auto* b : g_bars) b->dirty = true;
}

static void EnsureAnim() {
    if (!g_animRunning && g_ctl) {
        SetTimer(g_ctl, TM_ANIM, 40, nullptr);
        g_animRunning = true;
    }
}

static void AnimTick() {
    bool more = false;
    if (g_fade < g_fadeTarget) {
        g_fade = std::min(g_fadeTarget, g_fade + 36);
        more = true;
        MarkAll();
    } else if (g_fade > g_fadeTarget) {
        g_fade = std::max(g_fadeTarget, g_fade - 36);
        more = true;
        MarkAll();
        if (g_fade == g_fadeTarget && g_hiding) {
            FlushBars();
            FinishHide();
            more = false;
        }
    }
    for (auto& kv : g_hover) {
        float target = 0;
        for (auto* b : g_bars)
            if (b->hotKey == kv.first) target = 1;
        float v = kv.second;
        if (v != target) {
            v += (target > v) ? 0.34f : -0.34f;
            if (v < 0) v = 0;
            if (v > 1) v = 1;
            kv.second = v;
            MarkAll();
            if (v != target) more = true;
        }
    }
    if (g_pulseActive) {
        MarkAll();
        more = true;
    }
    FlushBars();
    if (!more) {
        KillTimer(g_ctl, TM_ANIM);
        g_animRunning = false;
    }
}

// --------------------------------------------------------- effect / appbar ----

struct ACCENT_POLICY_ {
    int state, flags;
    DWORD gradient;
    int anim;
};
struct WCA_DATA_ {
    int attrib;
    PVOID data;
    SIZE_T size;
};
typedef BOOL(WINAPI* SetWCA_t)(HWND, WCA_DATA_*);

static void ApplyEffect(Bar* b) {
    if (!b->fx) return;
    if (g_s.effect == L"mica") {
        int type = 2;  // DWMSBT_MAINWINDOW
        DwmSetWindowAttribute(b->fx, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));
        BOOL dark = Luma(g_bgColor) < 128;
        DwmSetWindowAttribute(b->fx, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
        MARGINS m{-1, -1, -1, -1};
        DwmExtendFrameIntoClientArea(b->fx, &m);
    } else {
        static SetWCA_t fn = (SetWCA_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
        if (!fn) return;
        ACCENT_POLICY_ ap{};
        ap.state = g_s.effect == L"acrylic" ? 4 : 3;
        ap.flags = 2;
        ap.gradient = ((DWORD)g_s.opacity << 24) | ((DWORD)GetBValue(g_bgColor) << 16) |
                      ((DWORD)GetGValue(g_bgColor) << 8) | GetRValue(g_bgColor);
        WCA_DATA_ d{19, &ap, sizeof(ap)};
        fn(b->fx, &d);
    }
}

static void PositionBar(Bar* b) {
    if (!b->hwnd) return;
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(b->mon, &mi);
    b->monRc = mi.rcMonitor;
    UINT dx = 96, dy = 96;
    if (SUCCEEDED(GetDpiForMonitor(b->mon, MDT_EFFECTIVE_DPI, &dx, &dy)) && dx) b->dpi = dx;

    // Window thickness = bar + top/bottom margins.
    int th = Sc(b, g_s.barHeight);
    int total = th + Sc(b, g_s.mTop) + Sc(b, g_s.mBottom);
    UINT edge = ABE_TOP;
    RECT rc = mi.rcMonitor;
    if (g_s.position == L"bottom") {
        edge = ABE_BOTTOM;
        rc.top = rc.bottom - total;
    } else {
        rc.bottom = rc.top + total;
    }

    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd = b->hwnd;
    if (g_s.reserveSpace) {
        if (!b->registered) {
            abd.uCallbackMessage = WM_APPBAR;
            SHAppBarMessage(ABM_NEW, &abd);
            b->registered = true;
        }
        abd.uEdge = edge;
        abd.rc = rc;
        SHAppBarMessage(ABM_QUERYPOS, &abd);
        if (edge == ABE_TOP)
            abd.rc.bottom = abd.rc.top + total;
        else
            abd.rc.top = abd.rc.bottom - total;
        SHAppBarMessage(ABM_SETPOS, &abd);
        rc = abd.rc;
    } else if (b->registered) {
        SHAppBarMessage(ABM_REMOVE, &abd);
        b->registered = false;
    }

    b->pos = rc;
    if (b->fx) {
        SetWindowPos(b->fx, HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                     SWP_NOACTIVATE);
        int rad = Sc(b, g_s.radius);
        RECT br = BodyRect(b, rc.right - rc.left, rc.bottom - rc.top);
        HRGN rgn = CreateRoundRectRgn(br.left, br.top, br.right + 1, br.bottom + 1, rad * 2, rad * 2);
        SetWindowRgn(b->fx, rgn, TRUE);
        ApplyEffect(b);
    }
    SetWindowPos(b->hwnd, HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                 SWP_NOACTIVATE);
    RECT actual;
    if (GetWindowRect(b->hwnd, &actual)) b->pos = actual;
    Wh_Log(L"Bar rect: %ld,%ld - %ld,%ld dpi=%u", b->pos.left, b->pos.top, b->pos.right, b->pos.bottom, b->dpi);
    b->dirty = true;
}

// --------------------------------------------------------------------- popups --

static HWND g_cal = nullptr;
static int g_calY = 0, g_calM = 0, g_calDpi = 96;
static ULONGLONG g_calClosedAt = 0;
static RECT g_cPrev{}, g_cNext{}, g_cToday{};

static int CalScale(int v) { return MulDiv(v, g_calDpi, 96); }

// y == 0: the event repeats every year
static bool EventOn(const EventItem& e, int y, int m) { return e.m == m && (e.y == 0 || e.y == y); }

static std::vector<std::wstring> CalTzLines() {
    std::vector<std::wstring> out;
    SYSTEMTIME utc;
    GetSystemTime(&utc);
    LPCWSTR loc = LocaleName();
    for (auto& z : g_s.tz) {
        std::wstring key = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\" + z.key;
#pragma pack(push, 1)
        struct TZI {
            LONG bias, stdBias, dstBias;
            SYSTEMTIME stdDate, dstDate;
        } raw{};
#pragma pack(pop)
        DWORD sz = sizeof(raw);
        if (RegGetValueW(HKEY_LOCAL_MACHINE, key.c_str(), L"TZI", RRF_RT_REG_BINARY, nullptr, &raw, &sz) !=
            ERROR_SUCCESS) {
            out.push_back(z.label + L"   \u2014  (unknown time zone key)");
            continue;
        }
        TIME_ZONE_INFORMATION tzi{};
        tzi.Bias = raw.bias;
        tzi.StandardBias = raw.stdBias;
        tzi.DaylightBias = raw.dstBias;
        tzi.StandardDate = raw.stdDate;
        tzi.DaylightDate = raw.dstDate;
        SYSTEMTIME lt;
        if (!SystemTimeToTzSpecificLocalTime(&tzi, &utc, &lt)) {
            out.push_back(z.label + L"   \u2014");
            continue;
        }
        wchar_t t[32], d[32];
        GetTimeFormatEx(loc, TIME_NOSECONDS, &lt, nullptr, t, 32);
        GetDateFormatEx(loc, 0, &lt, L"ddd", d, 32, nullptr);
        out.push_back(z.label + L"   " + t + L"  " + d);
    }
    return out;
}

static std::vector<std::wstring> CalEventLines() {
    std::vector<std::wstring> out;
    std::vector<EventItem> ev;
    for (auto& e : g_s.events)
        if (EventOn(e, g_calY, g_calM)) ev.push_back(e);
    std::sort(ev.begin(), ev.end(), [](const EventItem& a, const EventItem& b) { return a.d < b.d; });
    for (auto& e : ev) out.push_back(std::to_wstring(e.d) + L"   " + e.text);
    if (out.size() > 8) out.resize(8);
    return out;
}

static void CalSize(int* w, int* h) {
    int cell = CalScale(40), row = CalScale(30);
    *w = cell * 7 + CalScale(24);
    int lines = (int)CalEventLines().size() + (int)CalTzLines().size();
    int extra = lines ? CalScale(12) + lines * CalScale(22) : 0;
    *h = CalScale(48) + CalScale(26) + row * 6 + CalScale(12) + extra;
}

static void CalPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);
    HDC dc = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
    HGDIOBJ old = SelectObject(dc, bmp);

    HBRUSH br = CreateSolidBrush(g_bgColor);
    FillRect(dc, &rc, br);
    DeleteObject(br);
    HBRUSH frame = CreateSolidBrush(Blend(g_bgColor, g_textColor, 70));
    FrameRect(dc, &rc, frame);
    DeleteObject(frame);

    int fh = -MulDiv(11, g_calDpi, 72);
    HFONT f = CreateFontW(fh, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0,
                          g_s.font.c_str());
    HFONT fb = CreateFontW(fh, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0,
                           g_s.font.c_str());
    HFONT fi = CreateFontW(fh, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0,
                           L"Segoe MDL2 Assets");
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, g_textColor);
    LPCWSTR loc = LocaleName();

    int cell = CalScale(40), row = CalScale(30), left = CalScale(12);
    int hdr = CalScale(48);

    // header
    SYSTEMTIME st{};
    st.wYear = (WORD)g_calY;
    st.wMonth = (WORD)g_calM;
    st.wDay = 1;
    wchar_t title[64];
    GetDateFormatEx(loc, 0, &st, L"MMMM yyyy", title, 64, nullptr);
    SelectObject(dc, fb);
    RECT tr{left + cell, 0, left + cell * 6, hdr};
    DrawTextW(dc, title, -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    SelectObject(dc, fi);
    g_cPrev = {left, CalScale(8), left + cell, hdr - CalScale(8)};
    g_cNext = {left + cell * 6, CalScale(8), left + cell * 7, hdr - CalScale(8)};
    DrawTextW(dc, L"\uE76B", -1, &g_cPrev, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    DrawTextW(dc, L"\uE76C", -1, &g_cNext, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // weekdays
    SelectObject(dc, f);
    SetTextColor(dc, Blend(g_bgColor, g_textColor, 150));
    for (int k = 0; k < 7; k++) {
        int dow = g_s.weekMonday ? (k + 1) % 7 : k;  // 0 = Sunday
        SYSTEMTIME w{};
        w.wYear = 2023;
        w.wMonth = 1;
        w.wDay = (WORD)(1 + dow);  // January 1, 2023 is a Sunday
        wchar_t nm[16];
        GetDateFormatEx(loc, 0, &w, L"ddd", nm, 16, nullptr);
        RECT r{left + k * cell, hdr, left + (k + 1) * cell, hdr + CalScale(26)};
        DrawTextW(dc, nm, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }

    // grid
    SYSTEMTIME now;
    GetLocalTime(&now);
    int first = DayOfWeek(g_calY, g_calM, 1);
    int off = g_s.weekMonday ? (first + 6) % 7 : first;
    int dim = DaysInMonth(g_calY, g_calM);
    int gy = hdr + CalScale(26);
    SelectObject(dc, GetStockObject(NULL_PEN));
    SelectObject(dc, GetStockObject(DC_BRUSH));
    for (int d = 1; d <= dim; d++) {
        int idx = off + d - 1;
        int cx = left + (idx % 7) * cell, cy = gy + (idx / 7) * row;
        RECT r{cx, cy, cx + cell, cy + row};
        bool today = (g_calY == now.wYear && g_calM == now.wMonth && d == now.wDay);
        bool hasEv = false;
        for (auto& e : g_s.events)
            if (EventOn(e, g_calY, g_calM) && e.d == d) hasEv = true;
        SetTextColor(dc, g_textColor);
        if (today) {
            SetDCBrushColor(dc, g_accent);
            RoundRect(dc, r.left + 3, r.top + 2, r.right - 2, r.bottom - 1, CalScale(8), CalScale(8));
            SetTextColor(dc, Luma(g_accent) > 150 ? RGB(0, 0, 0) : RGB(255, 255, 255));
        }
        wchar_t num[8];
        swprintf(num, 8, L"%d", d);
        RECT nr = r;
        nr.bottom -= hasEv ? CalScale(4) : 0;
        DrawTextW(dc, num, -1, &nr, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        if (hasEv) {
            int dcx = (r.left + r.right) / 2, dcy = r.bottom - CalScale(5), rr = CalScale(2);
            SetDCBrushColor(dc, today ? RGB(255, 255, 255) : g_accent);
            Ellipse(dc, dcx - rr, dcy - rr, dcx + rr + 1, dcy + rr + 1);
        }
    }
    g_cToday = {};

    // events and time zones
    int y = gy + row * 6 + CalScale(8);
    auto lines = CalEventLines();
    auto tzl = CalTzLines();
    SelectObject(dc, f);
    if (!lines.empty() || !tzl.empty()) {
        HBRUSH sep = CreateSolidBrush(Blend(g_bgColor, g_textColor, 50));
        RECT sr{left, y, rc.right - left, y + 1};
        FillRect(dc, &sr, sep);
        DeleteObject(sep);
        y += CalScale(8);
    }
    SetTextColor(dc, g_accent);
    for (auto& l : lines) {
        RECT r{left, y, rc.right - left, y + CalScale(22)};
        DrawTextW(dc, l.c_str(), -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
        y += CalScale(22);
    }
    SetTextColor(dc, g_textColor);
    for (auto& l : tzl) {
        RECT r{left, y, rc.right - left, y + CalScale(22)};
        DrawTextW(dc, l.c_str(), -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS);
        y += CalScale(22);
    }

    BitBlt(hdc, 0, 0, rc.right, rc.bottom, dc, 0, 0, SRCCOPY);
    DeleteObject(f);
    DeleteObject(fb);
    DeleteObject(fi);
    SelectObject(dc, old);
    DeleteObject(bmp);
    DeleteDC(dc);
    EndPaint(hwnd, &ps);
}

static void CalRefresh(HWND hwnd) {
    int w, h;
    CalSize(&w, &h);
    RECT r;
    GetWindowRect(hwnd, &r);
    // for a bottom bar the calendar grows upward so it doesn't overlap the bar
    int y = (g_s.position == L"bottom") ? r.bottom - h : r.top;
    SetWindowPos(hwnd, nullptr, r.left, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(hwnd, nullptr, FALSE);
}

static void CalShift(HWND hwnd, int d) {
    g_calM += d;
    if (g_calM < 1) {
        g_calM = 12;
        g_calY--;
    }
    if (g_calM > 12) {
        g_calM = 1;
        g_calY++;
    }
    CalRefresh(hwnd);
}

static void CalToday(HWND hwnd) {
    SYSTEMTIME n;
    GetLocalTime(&n);
    g_calY = n.wYear;
    g_calM = n.wMonth;
    CalRefresh(hwnd);
}

static LRESULT CALLBACK CalProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT:
            CalPaint(hwnd);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE) DestroyWindow(hwnd);
            return 0;
        case WM_LBUTTONUP: {
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            if (PtInRect(&g_cPrev, pt)) {
                CalShift(hwnd, -1);
            } else if (PtInRect(&g_cNext, pt)) {
                CalShift(hwnd, 1);
            } else {
                RECT title{CalScale(12) + CalScale(40), 0, CalScale(12) + CalScale(240), CalScale(48)};
                if (PtInRect(&title, pt)) CalToday(hwnd);  // click on the header: return to the current month
            }
            return 0;
        }
        case WM_MOUSEWHEEL:
            CalShift(hwnd, GET_WHEEL_DELTA_WPARAM(wp) > 0 ? -1 : 1);
            return 0;
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE)
                DestroyWindow(hwnd);
            else if (wp == VK_LEFT || wp == VK_PRIOR)
                CalShift(hwnd, -1);
            else if (wp == VK_RIGHT || wp == VK_NEXT)
                CalShift(hwnd, 1);
            else if (wp == VK_HOME)
                CalToday(hwnd);
            return 0;
        case WM_TIMER:
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        case WM_DESTROY:
            if (g_cal == hwnd) g_cal = nullptr;
            g_calClosedAt = GetTickCount64();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void ToggleCalendar(Bar* b, const RECT& clockRc) {
    if (g_cal) {
        DestroyWindow(g_cal);
        g_cal = nullptr;
        return;
    }
    if (GetTickCount64() - g_calClosedAt < 400) return;  // the click on the clock closed the calendar
    SYSTEMTIME n;
    GetLocalTime(&n);
    g_calY = n.wYear;
    g_calM = n.wMonth;
    g_calDpi = (int)b->dpi;
    int w, h;
    CalSize(&w, &h);
    int cx = b->pos.left + (clockRc.left + clockRc.right) / 2;
    int x = cx - w / 2, y;
    if (g_s.position == L"bottom")
        y = b->pos.top - h;
    else
        y = b->pos.bottom;
    x = std::max((int)b->monRc.left, std::min(x, (int)b->monRc.right - w));
    y = std::max((int)b->monRc.top, std::min(y, (int)b->monRc.bottom - h));
    g_cal = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kCalClass, L"WH Calendar", WS_POPUP, x, y, w, h,
                            nullptr, nullptr, ModInst(), nullptr);
    if (g_cal) {
        ShowWindow(g_cal, SW_SHOW);
        SetForegroundWindow(g_cal);
        SetTimer(g_cal, 1, 5000, nullptr);
    }
}

// ----------------------------------------------------------------- actions ----

// While a menu is open the bar windows must not be destroyed: rebuilds, setting
// reloads and unloading are postponed until the menu closes.
static int g_modalDepth = 0;
static bool g_reloadPending = false, g_closePending = false;

static void OnModalEnd() {
    if (g_modalDepth > 0) return;
    HWND c = g_ctl;
    if (!c) return;
    if (g_closePending) {
        PostMessageW(c, WM_CLOSE, 0, 0);
    } else if (g_reloadPending) {
        g_reloadPending = false;
        PostMessageW(c, WM_RELOAD, 0, 0);
    }
}

static void PopupMenuAt(Bar* b, HMENU m, const RECT& rc, int* cmd) {
    UINT flags = TPM_RETURNCMD | TPM_NONOTIFY;
    int x, y;
    if (g_s.position == L"bottom") {
        flags |= TPM_BOTTOMALIGN | TPM_LEFTALIGN;
        x = b->pos.left + rc.left;
        y = b->pos.top + rc.top;
    } else {
        flags |= TPM_TOPALIGN | TPM_LEFTALIGN;
        x = b->pos.left + rc.left;
        y = b->pos.bottom;
    }
    HWND hw = b->hwnd;  // "b" may be freed while the menu is open, so it is not used after this point
    SetForegroundWindow(hw);
    g_modalDepth++;
    *cmd = TrackPopupMenu(m, flags, x, y, 0, hw, nullptr);
    g_modalDepth--;
    PostMessageW(hw, WM_NULL, 0, 0);
    OnModalEnd();
}

static void TimerMenu(Bar* b, const RECT& rc) {
    HMENU m = CreatePopupMenu();
    HMENU sub = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 201, g_t.mode == 2 ? L"Pomodoro: stop" : L"Pomodoro: start");
    const int mins[] = {1, 5, 10, 15, 25, 30, 45, 60, 90};
    const int nMins = (int)(sizeof(mins) / sizeof(mins[0]));
    for (int i = 0; i < nMins; i++) {
        wchar_t t[32];
        swprintf(t, 32, L"%d min", mins[i]);
        AppendMenuW(sub, MF_STRING, 250 + i, t);
    }
    AppendMenuW(m, MF_POPUP, (UINT_PTR)sub, L"Timer");
    if (g_t.mode != 0) {
        AppendMenuW(m, MF_STRING, 260, g_t.paused ? L"Resume" : L"Pause");
        AppendMenuW(m, MF_STRING, 261, g_t.mode == 1 ? L"Stop timer" : L"Stop Pomodoro");
    }
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 210, g_t.swRunning ? L"Stopwatch: pause" : L"Stopwatch: start");
    AppendMenuW(m, MF_STRING | ((g_t.swRunning || g_t.swAccum) ? 0 : MF_GRAYED), 211, L"Stopwatch: reset");
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    wchar_t at[64];
    if (g_s.alarmH >= 0)
        swprintf(at, 64, L"Alarm %02d:%02d: %ls", g_s.alarmH, g_s.alarmM, g_t.alarmOn ? L"turn off" : L"turn on");
    else
        swprintf(at, 64, L"Alarm (set the time in settings)");
    AppendMenuW(m, MF_STRING | (g_s.alarmH >= 0 ? 0 : MF_GRAYED), 220, at);
    if (g_t.ringUntil) AppendMenuW(m, MF_STRING, 230, L"Turn off the alert");

    int cmd = 0;
    PopupMenuAt(b, m, rc, &cmd);
    DestroyMenu(m);
    ULONGLONG now = GetTickCount64();
    if (cmd == 201) {
        if (g_t.mode == 2) {
            g_t.mode = 0;
            g_t.paused = false;
        } else {
            g_t.mode = 2;
            g_t.paused = false;
            g_t.pomoWork = true;
            g_t.pomoDone = 0;
            g_t.endTick = now + (ULONGLONG)g_s.pomoWork * 60000;
        }
    } else if (cmd >= 250 && cmd < 250 + nMins) {
        g_t.mode = 1;
        g_t.paused = false;
        g_t.endTick = now + (ULONGLONG)mins[cmd - 250] * 60000;
    } else if (cmd == 260 && g_t.mode != 0) {
        if (g_t.paused) {
            g_t.endTick = now + g_t.remain;
            g_t.paused = false;
        } else {
            g_t.remain = g_t.endTick > now ? g_t.endTick - now : 0;
            g_t.paused = true;
        }
    } else if (cmd == 261) {
        g_t.mode = 0;
        g_t.paused = false;
    } else if (cmd == 210) {
        if (g_t.swRunning) {
            g_t.swAccum += now - g_t.swStart;
            g_t.swRunning = false;
        } else {
            g_t.swStart = now;
            g_t.swRunning = true;
        }
    } else if (cmd == 211) {
        g_t.swRunning = false;
        g_t.swAccum = 0;
    } else if (cmd == 220) {
        g_t.alarmOn = !g_t.alarmOn;
        g_t.lastAlarmDay = -1;
    } else if (cmd == 230) {
        g_t.ringUntil = 0;
    }
    BuildContent();
    MarkAll();
    FlushBars();
}

static void EnableShutdownPrivilege() {
    HANDLE tok = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tok)) return;
    TOKEN_PRIVILEGES tp{};
    tp.PrivilegeCount = 1;
    if (LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid)) {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(tok, FALSE, &tp, 0, nullptr, nullptr);
    }
    CloseHandle(tok);
}

// With hibernation enabled, SetSuspendState(FALSE,...) hibernates instead of sleeping.
// SetSystemPowerState(TRUE, FALSE) puts the machine to sleep.
static void SleepNow() {
    typedef BOOL(WINAPI * SetSysPS_t)(BOOL, BOOL);
    static SetSysPS_t fn = (SetSysPS_t)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "SetSystemPowerState");
    EnableShutdownPrivilege();
    if (!fn || !fn(TRUE, FALSE)) SetSuspendState(FALSE, FALSE, FALSE);
}

static void SendWinCombo(WORD key, bool shift) {
    std::vector<INPUT> v;
    auto add = [&](WORD vk, bool up) {
        INPUT i{};
        i.type = INPUT_KEYBOARD;
        i.ki.wVk = vk;
        i.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
        v.push_back(i);
    };
    add(VK_LWIN, false);
    if (shift) add(VK_SHIFT, false);
    add(key, false);
    add(key, true);
    if (shift) add(VK_SHIFT, true);
    add(VK_LWIN, true);
    SendInput((UINT)v.size(), v.data(), sizeof(INPUT));
}

static void SendKeyPress(WORD key) {
    INPUT in[2]{};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = key;
    in[1].type = INPUT_KEYBOARD;
    in[1].ki.wVk = key;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

static std::atomic<bool> g_powerAsk{false};

static DWORD WINAPI PowerConfirmWorker(LPVOID arg) {
    bool restart = (int)(INT_PTR)arg == 304;
    bool com = SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));
    if (!g_stop &&
        MessageBoxW(nullptr, restart ? L"Restart the computer?" : L"Shut down the computer?", L"Top Bar",
                    MB_YESNO | MB_ICONQUESTION | MB_TOPMOST | MB_SETFOREGROUND) == IDYES &&
        !g_stop)
        ShellExecuteW(nullptr, L"open", L"shutdown.exe", restart ? L"/r /t 0" : L"/s /t 0", nullptr, SW_HIDE);
    if (com) CoUninitialize();
    g_powerAsk = false;
    g_workers--;
    return 0;
}

static void Screenshot() {
    HINSTANCE r = ShellExecuteW(nullptr, L"open", L"ms-screenclip:", nullptr, nullptr, SW_SHOWNORMAL);
    if ((INT_PTR)r <= 32) SendWinCombo('S', true);  // fallback: Win+Shift+S
}

// Win button menu (formerly the "Quick actions" button).
static void QuickMenu(Bar* b, const RECT& rc) {
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 310, L"Start menu");
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 301, L"Screenshot (region)");
    AppendMenuW(m, MF_STRING, 307, L"Task Manager");
    AppendMenuW(m, MF_STRING, 308, L"Windows Settings");
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 302, L"Lock");
    AppendMenuW(m, MF_STRING, 303, L"Sleep");
    AppendMenuW(m, MF_STRING, 309, L"Hibernate");
    AppendMenuW(m, MF_STRING, 306, L"Sign out");
    AppendMenuW(m, MF_STRING, 304, L"Restart");
    AppendMenuW(m, MF_STRING, 305, L"Shut down");
    int cmd = 0;
    PopupMenuAt(b, m, rc, &cmd);
    DestroyMenu(m);
    switch (cmd) {
        case 310: SendKeyPress(VK_LWIN); break;
        case 301: Screenshot(); break;
        case 302: LockWorkStation(); break;
        case 303: SleepNow(); break;
        case 309: SetSuspendState(TRUE, FALSE, FALSE); break;
        case 304:
        case 305:
            if (!g_powerAsk.exchange(true)) StartWorker(PowerConfirmWorker, (LPVOID)(INT_PTR)cmd);
            break;
        case 306: ShellExecuteW(nullptr, L"open", L"shutdown.exe", L"/l", nullptr, SW_HIDE); break;
        case 307: ShellExecuteW(nullptr, L"open", L"taskmgr.exe", nullptr, nullptr, SW_SHOWNORMAL); break;
        case 308: ShellExecuteW(nullptr, L"open", L"ms-settings:", nullptr, nullptr, SW_SHOWNORMAL); break;
    }
}

static void ClockMenu(Bar* b, const RECT& rc) {
    auto lines = CalTzLines();
    HMENU m = CreatePopupMenu();
    if (lines.empty()) {
        AppendMenuW(m, MF_STRING | MF_GRAYED, 1, L"No time zones set (see mod settings)");
    } else {
        for (size_t i = 0; i < lines.size(); i++) AppendMenuW(m, MF_STRING, 100 + i, lines[i].c_str());
    }
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 900, L"Date and time settings");
    int cmd = 0;
    PopupMenuAt(b, m, rc, &cmd);
    DestroyMenu(m);
    if (cmd == 900) ShellExecuteW(nullptr, L"open", L"ms-settings:dateandtime", nullptr, nullptr, SW_SHOWNORMAL);
}

static bool IsWin11() {
    typedef LONG(WINAPI * RtlGetVersion_t)(OSVERSIONINFOW*);
    static int cached = -1;
    if (cached < 0) {
        cached = 0;
        RtlGetVersion_t fn = (RtlGetVersion_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
        OSVERSIONINFOW vi{};
        vi.dwOSVersionInfoSize = sizeof(vi);
        if (fn && fn(&vi) == 0 && vi.dwBuildNumber >= 22000) cached = 1;
    }
    return cached == 1;
}

// Win11: Win+N, Win10: Win+A
static void OpenNotificationCenter() { SendWinCombo(IsWin11() ? 'N' : 'A', false); }

static void ResetNotifCount() {
    if (g_notifCount == 0) return;
    g_notifCount = 0;
    BuildContent();
    MarkAll();
    FlushBars();
}

// System hidden tray icons window: Win+B focuses the notification
// area, Enter opens the hidden icons window.
static HWND FindOverflowWnd() {
    const wchar_t* cls[] = {L"TopLevelWindowForOverflowXamlIsland", L"NotifyIconOverflowWindow"};
    for (auto c : cls) {
        HWND h = FindWindowW(c, nullptr);
        if (h && IsWindowVisible(h)) return h;
    }
    return nullptr;
}

static DWORD WINAPI HiddenIconsWorker(LPVOID) {
    RECT anc = g_trayAnchor;
    int edge = g_trayEdge;
    bool haveAnchor = anc.right > anc.left && anc.bottom > anc.top;
    SendWinCombo('B', false);
    Sleep(250);
    SendKeyPress(VK_RETURN);
    if (haveAnchor) {
        int stable = 0;
        for (int i = 0; i < 70 && !g_stop && stable < 4; i++) {
            Sleep(25);
            HWND w = FindOverflowWnd();
            if (!w) continue;
            RECT r{};
            if (!GetWindowRect(w, &r)) continue;
            int ww = r.right - r.left, hh = r.bottom - r.top;
            if (ww < 30 || hh < 30) continue;
            MONITORINFO mi{sizeof(mi)};
            GetMonitorInfoW(MonitorFromRect(&anc, MONITOR_DEFAULTTONEAREST), &mi);
            const RECT& mr = mi.rcMonitor;
            int gap = 8, x, y;
            x = (anc.left + anc.right) / 2 - ww / 2;
            y = edge == 0 ? anc.bottom + gap : anc.top - hh - gap;
            x = std::max((int)mr.left + 4, std::min(x, (int)mr.right - ww - 4));
            y = std::max((int)mr.top + 4, std::min(y, (int)mr.bottom - hh - 4));
            if (r.left == x && r.top == y) {
                stable++;
            } else {
                stable = 0;
                SetWindowPos(w, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
    }
    g_workers--;
    return 0;
}

// Done in a separate thread so the bar does not "freeze" during the pause.
static void OpenHiddenIcons() { StartWorker(HiddenIconsWorker, nullptr); }

// Windows 11 quick settings window (network, sound, brightness).
static RECT g_menuAnchor{};
static int g_menuEdge = 0;  // 0 = bar on top, 1 = bottom
static std::atomic<bool> g_qsBusy{false};

static HWND FindQuickSettingsWnd() {
    const wchar_t* cls[] = {L"ControlCenterWindow"};
    for (auto c : cls) {
        HWND h = FindWindowW(c, nullptr);
        if (h && IsWindowVisible(h)) return h;
    }
    return nullptr;
}

// Presses Win+A and moves the window that appears next to the button on the bar.
static DWORD WINAPI QuickSettingsWorker(LPVOID) {
    RECT anc = g_menuAnchor;
    int edge = g_menuEdge;
    SendWinCombo('A', false);
    int stable = 0;
    for (int i = 0; i < 120 && !g_stop && stable < 6; i++) {
        Sleep(25);
        HWND w = FindQuickSettingsWnd();
        if (!w) continue;
        RECT r{};
        if (!GetWindowRect(w, &r)) continue;
        int ww = r.right - r.left, hh = r.bottom - r.top;
        if (ww < 100 || hh < 100) continue;
        MONITORINFO mi{sizeof(mi)};
        GetMonitorInfoW(MonitorFromRect(&anc, MONITOR_DEFAULTTONEAREST), &mi);
        const RECT& mr = mi.rcMonitor;
        int gap = 8;
        int x = (anc.left + anc.right) / 2 - ww / 2;
        int y = edge == 0 ? anc.bottom + gap : anc.top - hh - gap;
        x = std::max((int)mr.left + 4, std::min(x, (int)mr.right - ww - 4));
        y = std::max((int)mr.top + 4, std::min(y, (int)mr.bottom - hh - 4));
        if (r.left == x && r.top == y) {
            stable++;
        } else {
            stable = 0;
            SetWindowPos(w, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
    g_qsBusy = false;
    g_workers--;
    return 0;
}

// Menu button: opens quick settings. Depending on the setting, the window is moved
// next to our bar (below a top bar / above a bottom bar) or stays where
// Windows shows it.
static void OpenQuickSettings(Bar* b, const RECT& rc) {
    if (!IsWin11() || g_s.menuPlace != L"bar") {
        SendWinCombo('A', false);
        return;
    }
    if (g_qsBusy.exchange(true)) return;  // the previous move is still in progress
    RECT sr = rc;
    OffsetRect(&sr, b->pos.left, b->pos.top);
    // take the bounds of the bar itself (without outer margins) so the window sticks to it
    RECT body = BodyRect(b, b->pos.right - b->pos.left, b->pos.bottom - b->pos.top);
    sr.top = b->pos.top + body.top;
    sr.bottom = b->pos.top + body.bottom;
    g_menuAnchor = sr;
    g_menuEdge = g_s.position == L"bottom" ? 1 : 0;
    StartWorker(QuickSettingsWorker, nullptr);
}

static void RefreshVolumeUi() {
    BuildContent();
    MarkAll();
    FlushBars();
}

static const Elem* ElemOf(const Hit& h) {
    if (h.list == 0 && h.i >= 0 && h.i < (int)g_left.size()) return &g_left[h.i];
    if (h.list == 1 && h.i >= 0 && h.i < (int)g_right.size()) return &g_right[h.i];
    if (h.list == 2 && g_hasClock) return &g_clock;
    if (h.list == 3 && g_hasStart) return &g_start;
    return nullptr;
}

static RECT RectOf(Bar* b, const Hit& h) {
    if (h.list == 0 && h.i < (int)b->rl.size()) return b->rl[h.i];
    if (h.list == 1 && h.i < (int)b->rr.size()) return b->rr[h.i];
    if (h.list == 3) return b->rStart;
    return b->rClock;
}

static Hit HitTest(Bar* b, POINT pt) {
    int sp = Sc(b, g_s.itemSpacing) / 2 + 2;
    auto in = [&](const RECT& r) {
        RECT q = r;
        InflateRect(&q, sp, sp);
        return PtInRect(&q, pt) != 0;
    };
    if (g_hasStart && in(b->rStart)) return Hit{3, 0};
    for (size_t i = 0; i < g_left.size() && i < b->rl.size(); i++)
        if (g_left[i].act != A_NONE && in(b->rl[i])) return Hit{0, (int)i};
    for (size_t i = 0; i < g_right.size() && i < b->rr.size(); i++)
        if (g_right[i].act != A_NONE && in(b->rr[i])) return Hit{1, (int)i};
    if (g_hasClock && in(b->rClock)) return Hit{2, 0};
    return Hit{};
}

static void Activate(Bar* b, const Hit& h) {
    const Elem* e = ElemOf(h);
    if (!e) return;
    RECT rc = RectOf(b, h);
    switch (e->act) {
        case A_PIN:
            if (e->idx >= 0 && e->idx < (int)g_pins.size())
                ShellExecuteW(nullptr, L"open", g_pins[e->idx].path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            break;
        case A_RIGHTAPP:
            if (e->idx >= 0 && e->idx < (int)g_rightApps.size())
                ShellExecuteW(nullptr, L"open", g_rightApps[e->idx].path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            break;
        case A_START: QuickMenu(b, rc); break;
        case A_CUSTOM: break;
        case A_CLOCK: ToggleCalendar(b, rc); break;
        case A_MPREV: MediaKey(VK_MEDIA_PREV_TRACK); break;
        case A_MPLAY: MediaKey(VK_MEDIA_PLAY_PAUSE); break;
        case A_MNEXT: MediaKey(VK_MEDIA_NEXT_TRACK); break;
        case A_STATS: ShellExecuteW(nullptr, L"open", L"taskmgr.exe", nullptr, nullptr, SW_SHOWNORMAL); break;
        case A_WIFI: ShellExecuteW(nullptr, L"open", L"ms-settings:network-wifi", nullptr, nullptr, SW_SHOWNORMAL); break;
        case A_BT: ShellExecuteW(nullptr, L"open", L"ms-settings:bluetooth", nullptr, nullptr, SW_SHOWNORMAL); break;
        case A_BATTERY: ShellExecuteW(nullptr, L"open", L"ms-settings:batterysaver", nullptr, nullptr, SW_SHOWNORMAL); break;
        case A_TRAY: {
            // remember where the click was so the hidden icons window opens next to the button
            RECT sr = rc;
            OffsetRect(&sr, b->pos.left, b->pos.top);
            g_trayAnchor = sr;
            g_trayEdge = g_s.position == L"bottom" ? 1 : 0;
            OpenHiddenIcons();
            break;
        }
        case A_MENU:
            OpenQuickSettings(b, rc);  // quick settings (network, sound, brightness) next to the bar
            break;
        case A_LANG: SendWinCombo(VK_SPACE, false); break;
        case A_VOLUME:
            if (IsWin11())
                SendWinCombo('A', false);  // Windows 11 quick settings
            else
                ShellExecuteW(nullptr, L"open", L"ms-settings:sound", nullptr, nullptr, SW_SHOWNORMAL);
            break;
        case A_MTRACK:
            if (g_mediaWnd && IsWindow(g_mediaWnd)) {
                if (IsIconic(g_mediaWnd)) ShowWindow(g_mediaWnd, SW_RESTORE);
                SetForegroundWindow(g_mediaWnd);
            }
            break;
        case A_TIMER:
            if (g_t.ringUntil) {  // clicking the alert turns it off
                g_t.ringUntil = 0;
                BuildContent();
                MarkAll();
                FlushBars();
            } else {
                TimerMenu(b, rc);
            }
            break;
        case A_BELL:
            OpenNotificationCenter();
            ResetNotifCount();
            break;
        default: break;
    }
}

// Insertion slot index by coordinate. For kind 2 (feature order) this is the item index in g_right.
static int DropIndexFor(Bar* b, POINT pt) {
    int pinPos = 0;
    int kind = b->dragKind;
    bool rt = kind >= 1;
    const std::vector<Elem>& lst = rt ? g_right : g_left;
    const std::vector<RECT>& rcs = rt ? b->rr : b->rl;
    for (size_t i = 0; i < lst.size() && i < rcs.size(); i++) {
        if (!DragMatch(kind, lst[i].act)) continue;
        const RECT& r = rcs[i];
        int center = (r.left + r.right) / 2;
        if (pt.x < center) return pinPos;
        pinPos++;
    }
    return pinPos;
}

// ---------------------------------------------------------------- windows ----

static void PinMenu(Bar* b, const Hit& h) {
    const Elem* e = ElemOf(h);
    if (!e || e->act != A_PIN) return;
    int idx = e->idx;
    if (idx < 0 || idx >= (int)g_pins.size()) return;
    RECT rc = RectOf(b, h);
    std::wstring path = g_pins[idx].path;
    bool canLeft = idx > 0, canRight = idx + 1 < (int)g_pins.size();
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, L"Open");
    AppendMenuW(m, MF_STRING, 2, L"Show in folder");
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING | (canLeft ? 0 : MF_GRAYED), 4, L"Move left");
    AppendMenuW(m, MF_STRING | (canRight ? 0 : MF_GRAYED), 5, L"Move right");
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 3, L"Unpin");
    int cmd = 0;
    PopupMenuAt(b, m, rc, &cmd);
    DestroyMenu(m);
    if (cmd == 1) {
        ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    } else if (cmd == 2) {
        std::wstring arg = L"/select,\"" + path + L"\"";
        ShellExecuteW(nullptr, L"open", L"explorer.exe", arg.c_str(), nullptr, SW_SHOWNORMAL);
    } else if (cmd == 3) {
        UnpinAt(idx);
        BuildContent();
        MarkAll();
        FlushBars();
    } else if (cmd == 4 || cmd == 5) {
        if (MovePin(idx, cmd == 4 ? -1 : 1)) {
            BuildContent();
            MarkAll();
            FlushBars();
        }
    }
}

static void RightAppMenu(Bar* b, const Hit& h) {
    const Elem* e = ElemOf(h);
    if (!e || e->act != A_RIGHTAPP) return;
    int idx = e->idx;
    if (idx < 0 || idx >= (int)g_rightApps.size()) return;
    RECT rc = RectOf(b, h);
    std::wstring path = g_rightApps[idx].path;
    bool canLeft = idx > 0, canRight = idx + 1 < (int)g_rightApps.size();
    std::wstring tok = L"apps";
    bool gLeft = CanMoveToken(tok, -1), gRight = CanMoveToken(tok, 1);
    HMENU m = CreatePopupMenu();
    AppendMenuW(m, MF_STRING, 1, L"Open");
    AppendMenuW(m, MF_STRING, 2, L"Show in folder");
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING | (canLeft ? 0 : MF_GRAYED), 4, L"Move left");
    AppendMenuW(m, MF_STRING | (canRight ? 0 : MF_GRAYED), 5, L"Move right");
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING | (gLeft ? 0 : MF_GRAYED), 6, L"Move app group left");
    AppendMenuW(m, MF_STRING | (gRight ? 0 : MF_GRAYED), 7, L"Move app group right");
    int cmd = 0;
    PopupMenuAt(b, m, rc, &cmd);
    DestroyMenu(m);
    bool refresh = false;
    if (cmd == 1) {
        ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    } else if (cmd == 2) {
        std::wstring arg = L"/select,\"" + path + L"\"";
        ShellExecuteW(nullptr, L"open", L"explorer.exe", arg.c_str(), nullptr, SW_SHOWNORMAL);
    } else if (cmd == 4 || cmd == 5) {
        refresh = MoveRightApp(idx, cmd == 4 ? -1 : 1);
    } else if (cmd == 6 || cmd == 7) {
        refresh = MoveTokenStep(tok, cmd == 6 ? -1 : 1);
    }
    if (refresh) {
        BuildContent();
        MarkAll();
        FlushBars();
    }
}

// Menu of any right-side feature: its own items + moving along the bar.
static void FuncMenu(Bar* b, const Hit& h) {
    const Elem* e = ElemOf(h);
    if (!e || h.list != 1 || e->act == A_NONE || e->act == A_RIGHTAPP) return;
    Act act = e->act;
    RECT rc = RectOf(b, h);
    std::wstring tok = TokenOf(act);
    bool canL = CanMoveToken(tok, -1), canR = CanMoveToken(tok, 1);
    bool muted;
    {
        Lock l;
        muted = g_d.volMuted;
    }
    HMENU m = CreatePopupMenu();
    if (act == A_VOLUME) {
        AppendMenuW(m, MF_STRING, 1, muted ? L"Unmute" : L"Mute");
        AppendMenuW(m, MF_STRING, 2, L"Volume mixer");
        AppendMenuW(m, MF_STRING, 3, L"Sound settings");
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    } else if (act == A_BELL && g_s.notifCounter) {
        AppendMenuW(m, MF_STRING, 6, L"Reset counter");
        AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    }
    AppendMenuW(m, MF_STRING | (canL ? 0 : MF_GRAYED), 4, L"Move left");
    AppendMenuW(m, MF_STRING | (canR ? 0 : MF_GRAYED), 5, L"Move right");
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING, 11, L"Reset feature order");
    int cmd = 0;
    PopupMenuAt(b, m, rc, &cmd);
    DestroyMenu(m);
    bool refresh = false;
    if (cmd == 1) {
        ToggleMute();
        refresh = true;
    } else if (cmd == 2) {
        ShellExecuteW(nullptr, L"open", L"ms-settings:apps-volume", nullptr, nullptr, SW_SHOWNORMAL);
    } else if (cmd == 3) {
        ShellExecuteW(nullptr, L"open", L"ms-settings:sound", nullptr, nullptr, SW_SHOWNORMAL);
    } else if (cmd == 6) {
        ResetNotifCount();
    } else if (cmd == 4 || cmd == 5) {
        refresh = MoveTokenStep(tok, cmd == 4 ? -1 : 1);
    } else if (cmd == 11) {
        ResetRightOrder();
        refresh = true;
    }
    if (refresh) {
        BuildContent();
        MarkAll();
        FlushBars();
    }
}

// Accepting files, shortcuts and folders dropped onto the bar.
static void CollectDropPaths(IDataObject* d, std::vector<std::wstring>& out) {
    FORMATETC f{CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM sm{};
    if (SUCCEEDED(d->GetData(&f, &sm))) {
        HDROP hd = (HDROP)sm.hGlobal;
        UINT n = DragQueryFileW(hd, 0xFFFFFFFF, nullptr, 0);
        for (UINT i = 0; i < n; i++) {
            wchar_t buf[MAX_PATH * 2];
            if (DragQueryFileW(hd, i, buf, ARRAYSIZE(buf))) out.push_back(buf);
        }
        ReleaseStgMedium(&sm);
    }
    if (!out.empty()) return;
    IShellItemArray* arr = nullptr;
    if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(d, __uuidof(IShellItemArray), (void**)&arr)) && arr) {
        DWORD cnt = 0;
        arr->GetCount(&cnt);
        for (DWORD i = 0; i < cnt; i++) {
            IShellItem* it = nullptr;
            if (SUCCEEDED(arr->GetItemAt(i, &it)) && it) {
                PWSTR nm = nullptr;
                if (SUCCEEDED(it->GetDisplayName(SIGDN_FILESYSPATH, &nm)) && nm) {
                    out.push_back(nm);
                    CoTaskMemFree(nm);
                }
                it->Release();
            }
        }
        arr->Release();
    }
}

class BarDrop : public IDropTarget {
    LONG ref_ = 1;
    Bar* bar_;
    bool accept_ = false;

    static bool Accepts(IDataObject* d) {
        FORMATETC f{CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        if (d->QueryGetData(&f) == S_OK) return true;
        IShellItemArray* a = nullptr;
        if (SUCCEEDED(SHCreateShellItemArrayFromDataObject(d, __uuidof(IShellItemArray), (void**)&a)) && a) {
            a->Release();
            return true;
        }
        return false;
    }
    int IndexAt(POINTL p) {
        POINT pt{p.x, p.y};
        ScreenToClient(bar_->hwnd, &pt);
        bar_->dragKind = 0;
        return DropIndexFor(bar_, pt);
    }
    void Hover(POINTL p, DWORD* eff) {
        if (!accept_) {
            *eff = DROPEFFECT_NONE;
            return;
        }
        int i = IndexAt(p);
        if (i != bar_->dropIdx || !bar_->extDrag) {
            bar_->dropIdx = i;
            bar_->dragKind = 0;
            bar_->extDrag = true;
            bar_->dirty = true;
            FlushBars();
        }
        *eff = (*eff & DROPEFFECT_LINK) ? DROPEFFECT_LINK : ((*eff & DROPEFFECT_COPY) ? DROPEFFECT_COPY : DROPEFFECT_NONE);
    }

   public:
    explicit BarDrop(Bar* b) : bar_(b) {}
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (IsEqualIID(riid, __uuidof(IUnknown)) || IsEqualIID(riid, __uuidof(IDropTarget))) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&ref_); }
    STDMETHODIMP_(ULONG) Release() override {
        ULONG r = InterlockedDecrement(&ref_);
        if (!r) delete this;
        return r;
    }
    STDMETHODIMP DragEnter(IDataObject* d, DWORD, POINTL p, DWORD* eff) override {
        accept_ = d && Accepts(d);
        Hover(p, eff);
        return S_OK;
    }
    STDMETHODIMP DragOver(DWORD, POINTL p, DWORD* eff) override {
        Hover(p, eff);
        return S_OK;
    }
    STDMETHODIMP DragLeave() override {
        accept_ = false;
        bar_->extDrag = false;
        bar_->dropIdx = -1;
        bar_->dirty = true;
        FlushBars();
        return S_OK;
    }
    STDMETHODIMP Drop(IDataObject* d, DWORD, POINTL p, DWORD* eff) override {
        bar_->extDrag = false;
        int at = accept_ ? IndexAt(p) : -1;
        bar_->dropIdx = -1;
        std::vector<std::wstring> paths;
        if (d && accept_) CollectDropPaths(d, paths);
        bool changed = false;
        for (auto& path : paths) {
            if (PinPath(path, at)) {
                changed = true;
                if (at >= 0) at++;
            }
        }
        *eff = changed ? DROPEFFECT_LINK : DROPEFFECT_NONE;
        accept_ = false;
        BuildContent();
        MarkAll();
        FlushBars();
        return S_OK;
    }
};

static Bar* BarOf(HWND h) { return (Bar*)GetWindowLongPtrW(h, GWLP_USERDATA); }

static LRESULT CALLBACK FxProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_ERASEBKGND) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect((HDC)wp, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
        return 1;
    }
    if (msg == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK BarProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Bar* b = BarOf(hwnd);
    if (!b) return DefWindowProcW(hwnd, msg, wp, lp);
    switch (msg) {
        case WM_MOUSEMOVE: {
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            if (!b->tracking) {
                TRACKMOUSEEVENT t{sizeof(t), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&t);
                b->tracking = true;
            }
            if (b->dragIdx >= 0 && !b->dragging) {
                if (abs(pt.x - b->dragStart.x) > Sc(b, 5) || abs(pt.y - b->dragStart.y) > Sc(b, 5))
                    b->dragging = true;
            }
            if (b->dragging) {
                b->dragPt = pt;
                b->dropIdx = DropIndexFor(b, pt);
                b->dirty = true;
                FlushBars();
                return 0;
            }
            int key = -1;
            Hit h = HitTest(b, pt);
            if (const Elem* e = ElemOf(h)) key = KeyOf(*e);
            if (key != b->hotKey) {
                b->hotKey = key;
                if (key >= 0) g_hover[key];
                if (!g_s.animations) {
                    for (auto& kv : g_hover) kv.second = (kv.first == key) ? 1.0f : 0.0f;
                    b->dirty = true;
                    FlushBars();
                } else {
                    EnsureAnim();
                }
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            b->tracking = false;
            if (b->hotKey != -1) {
                b->hotKey = -1;
                if (!g_s.animations) {
                    for (auto& kv : g_hover) kv.second = 0;
                    b->dirty = true;
                    FlushBars();
                } else {
                    EnsureAnim();
                }
            }
            return 0;
        case WM_SETCURSOR:
            if (LOWORD(lp) == HTCLIENT) {
                SetCursor(LoadCursorW(nullptr, b->hotKey >= 0 ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            break;
        case WM_LBUTTONDOWN: {
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            Hit h = HitTest(b, pt);
            b->downList = h.list;
            b->downI = h.i;
            b->dragIdx = -1;
            b->dragging = false;
            b->dragKind = 0;
            const Elem* e = ElemOf(h);
            if (e && e->act == A_PIN && g_pins.size() > 1) {
                b->dragIdx = e->idx;
                b->dragStart = pt;
            } else if (e && e->act == A_RIGHTAPP && g_rightApps.size() > 1) {
                b->dragKind = 1;
                b->dragIdx = e->idx;
                b->dragStart = pt;
            } else if (e && h.list == 1 && e->act != A_NONE && e->act != A_RIGHTAPP && g_right.size() > 1) {
                b->dragKind = 2;  // dragging a feature: changes the order
                b->dragIdx = h.i;
                b->dragStart = pt;
            }
            b->downKey = e ? KeyOf(*e) : -1;
            SetCapture(hwnd);
            return 0;
        }
        case WM_LBUTTONUP: {
            if (GetCapture() == hwnd) ReleaseCapture();
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            if (b->dragging) {
                int from = b->dragIdx, to = DropIndexFor(b, pt);
                b->dragging = false;
                b->dragIdx = -1;
                if (b->dragKind == 2) {
                    DropRightToken(from, to);
                } else {
                    std::vector<PinInfo>& vec = b->dragKind == 1 ? g_rightApps : g_pins;
                    if (from >= 0 && from < (int)vec.size() && to != from && to != from + 1) {
                        PinInfo it = vec[from];
                        vec.erase(vec.begin() + from);
                        if (to > from) to--;
                        vec.insert(vec.begin() + to, it);
                        if (b->dragKind == 1) SaveRightOrder(); else SavePinOrder();
                    }
                }
                BuildContent();
                MarkAll();
                FlushBars();
                return 0;
            }
            b->dragIdx = -1;
            Hit h = HitTest(b, pt);
            // compare the item itself, not its index: the item list may be updated between press and release
            const Elem* eu = ElemOf(h);
            if (eu && b->downKey >= 0 && KeyOf(*eu) == b->downKey) {
                Activate(b, h);
                b = BarOf(hwnd);  // the bar may have been rebuilt while a menu was open
                if (!b) return 0;
            }
            b->downKey = -1;
            return 0;
        }
        case WM_RBUTTONUP: {
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            Hit h = HitTest(b, pt);
            const Elem* e = ElemOf(h);
            if (!e) return 0;
            Act act = e->act;
            if (act == A_CLOCK) ClockMenu(b, RectOf(b, h));
            else if (act == A_START) QuickMenu(b, RectOf(b, h));
            else if (act == A_PIN) PinMenu(b, h);
            else if (act == A_RIGHTAPP) RightAppMenu(b, h);
            else if (h.list == 1 && act != A_NONE) FuncMenu(b, h);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            POINT pt{(short)LOWORD(lp), (short)HIWORD(lp)};
            ScreenToClient(hwnd, &pt);
            Hit h = HitTest(b, pt);
            const Elem* e = ElemOf(h);
            if (e && e->act == A_VOLUME) {
                AdjustVolume(GET_WHEEL_DELTA_WPARAM(wp) > 0 ? 5 : -5);
                RefreshVolumeUi();
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
            if (b->dragging) {
                b->dragging = false;
                b->dragIdx = -1;
                b->dirty = true;
                FlushBars();
            }
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTCLIENT;
        case WM_APPBAR:
            if (wp == ABN_POSCHANGED) {
                PositionBar(b);
                FlushBars();
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static BOOL CALLBACK MonEnum(HMONITOR hm, HDC, LPRECT, LPARAM lp) {
    ((std::vector<HMONITOR>*)lp)->push_back(hm);
    return TRUE;
}

static void DestroyBars() {
    if (g_cal) {
        DestroyWindow(g_cal);
        g_cal = nullptr;
    }
    for (auto* b : g_bars) {
        if (b->registered) {
            APPBARDATA abd{};
            abd.cbSize = sizeof(abd);
            abd.hWnd = b->hwnd;
            SHAppBarMessage(ABM_REMOVE, &abd);
            b->registered = false;
        }
        if (b->hwnd) {
            if (b->dropReg) RevokeDragDrop(b->hwnd);
            SetWindowLongPtrW(b->hwnd, GWLP_USERDATA, 0);
            DestroyWindow(b->hwnd);
        }
        if (b->fx) DestroyWindow(b->fx);
        if (b->bmp) {
            SelectObject(b->mem, b->oldBmp);
            DeleteObject(b->bmp);
        }
        if (b->mem) DeleteDC(b->mem);
        if (b->fText) DeleteObject(b->fText);
        if (b->fClock) DeleteObject(b->fClock);
        if (b->fIcon) DeleteObject(b->fIcon);
        if (b->fSym) DeleteObject(b->fSym);
        if (b->fCustom) DeleteObject(b->fCustom);
        delete b;
    }
    g_bars.clear();
}

static void CreateBars() {
    DestroyBars();
    std::vector<HMONITOR> mons;
    EnumDisplayMonitors(nullptr, nullptr, MonEnum, (LPARAM)&mons);
    HMONITOR primary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    HINSTANCE hi = ModInst();
    for (HMONITOR hm : mons) {
        if (g_s.monitors != L"all" && hm != primary) continue;
        Bar* b = new Bar();
        b->mon = hm;
        UINT dx = 96, dy = 96;
        if (SUCCEEDED(GetDpiForMonitor(hm, MDT_EFFECTIVE_DPI, &dx, &dy)) && dx) b->dpi = dx;
        if (g_s.effect != L"none") {
            b->fx = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, kFxClass,
                                    L"WH Top Bar FX", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, hi, nullptr);
        }
        b->hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                                  kBarClass, L"WH Top Bar", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr, hi, nullptr);
        if (!b->hwnd) {
            if (b->fx) DestroyWindow(b->fx);
            delete b;
            continue;
        }
        SetWindowLongPtrW(b->hwnd, GWLP_USERDATA, (LONG_PTR)b);
        {
            BarDrop* dt = new BarDrop(b);
            if (SUCCEEDED(RegisterDragDrop(b->hwnd, dt))) b->dropReg = true;
            dt->Release();
        }
        BuildFonts(b);
        g_bars.push_back(b);
        PositionBar(b);
    }
    g_fade = g_s.animations ? 0 : 255;
    g_fadeTarget = 255;
    g_hiding = false;
    for (auto* b : g_bars) {
        if (b->fx && !g_fsHidden) ShowWindow(b->fx, SW_SHOWNOACTIVATE);
        if (!g_fsHidden) ShowWindow(b->hwnd, SW_SHOWNOACTIVATE);
    }
    MarkAll();
    FlushBars();
    if (g_s.animations) EnsureAnim();
}

// -------------------------------------------------------------- controller ----

static bool IsFullscreenBusy() {
    QUERY_USER_NOTIFICATION_STATE st;
    if (SUCCEEDED(SHQueryUserNotificationState(&st)) &&
        (st == QUNS_BUSY || st == QUNS_RUNNING_D3D_FULL_SCREEN || st == QUNS_PRESENTATION_MODE))
        return true;
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    for (auto* b : g_bars)
        if (fg == b->hwnd) return false;
    wchar_t cls[64];
    GetClassNameW(fg, cls, 64);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") || !wcscmp(cls, L"Shell_TrayWnd")) return false;
    RECT r;
    if (!GetWindowRect(fg, &r)) return false;
    HMONITOR hm = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hm, &mi)) return false;
    return r.left <= mi.rcMonitor.left && r.top <= mi.rcMonitor.top && r.right >= mi.rcMonitor.right &&
           r.bottom >= mi.rcMonitor.bottom && !(GetWindowLongW(fg, GWL_STYLE) & WS_CAPTION);
}

// Finally hide the bar (after the fade-out).
static void FinishHide() {
    g_hiding = false;
    g_fsHidden = true;
    for (auto* b : g_bars) {
        b->visible = false;
        ShowWindow(b->hwnd, SW_HIDE);
        if (b->fx) ShowWindow(b->fx, SW_HIDE);
    }
    if (g_cal) DestroyWindow(g_cal);
}

// Hide the bar in fullscreen apps and smoothly show it
// for a few seconds when the Win key is pressed.
static void CheckFullscreen() {
    bool peek = GetTickCount64() < g_peekUntil;
    bool fs = g_s.hideFs && !peek && IsFullscreenBusy();
    if (fs) {
        if (!g_fsHidden && !g_hiding) {
            g_hiding = true;
            g_fadeTarget = 0;
            for (auto* b : g_bars)
                if (b->fx) ShowWindow(b->fx, SW_HIDE);
            if (g_s.animations) {
                EnsureAnim();
            } else {
                g_fade = 0;
                FinishHide();
            }
        }
        return;
    }
    if (g_hiding) {  // changed our mind: fullscreen ended while the fade-out was in progress
        g_hiding = false;
        g_fadeTarget = 255;
        for (auto* b : g_bars) {
            if (b->fx) ShowWindow(b->fx, SW_SHOWNOACTIVATE);
            b->dirty = true;
        }
        EnsureAnim();
        return;
    }
    if (g_fsHidden) {
        g_fsHidden = false;
        g_fade = g_s.animations ? 0 : 255;
        g_fadeTarget = 255;
        for (auto* b : g_bars) {
            b->visible = true;
            b->dirty = true;
            if (b->fx) {
                SetWindowPos(b->fx, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                ShowWindow(b->fx, SW_SHOWNOACTIVATE);
            }
            SetWindowPos(b->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            ShowWindow(b->hwnd, SW_SHOWNOACTIVATE);
        }
        FlushBars();
        if (g_s.animations) EnsureAnim();
    }
}

// Win key press (polling the key state, no global hooks).
static void PollWinKey() {
    bool down = (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);
    if (down && !g_winPrev) {
        g_peekUntil = GetTickCount64() + 4000;
        CheckFullscreen();
    }
    g_winPrev = down;
}

static BOOL CALLBACK ToastEnum(HWND h, LPARAM lp) {
    wchar_t cls[64];
    if (!GetClassNameW(h, cls, 64) || wcscmp(cls, L"Windows.UI.Core.CoreWindow")) return TRUE;
    if (!IsWindowVisible(h)) return TRUE;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked) return TRUE;
    wchar_t t[128];
    if (GetWindowTextW(h, t, 128) <= 0) return TRUE;
    std::wstring title = Lower(t);
    for (auto& k : g_s.notifKeys) {
        if (title.find(k) != std::wstring::npos) {
            *(bool*)lp = true;
            return FALSE;
        }
    }
    return TRUE;
}

// Approximate counter: counts notification banners appearing on screen.
static void PollToasts() {
    if (!g_s.showBell || !g_s.notifCounter || g_s.notifKeys.empty()) {
        g_toastPrev = false;
        return;
    }
    bool found = false;
    EnumWindows(ToastEnum, (LPARAM)&found);
    if (found && !g_toastPrev) {
        g_notifCount++;
        if (BuildContent()) MarkAll();
        FlushBars();
    }
    g_toastPrev = found;
}

// Quickly (every 0.5 s) track keyboard layout changes.
static void PollLang() {
    if (g_s.showLang) UpdateLang();
    if (BuildContent()) MarkAll();  // also pick up the new volume
    FlushBars();
}

static void ApplyAll() {
    LoadSettings();
    ResolveColors();
    g_t.alarmOn = g_s.alarmEnabled;
    g_t.lastAlarmDay = -1;
    BuildIcons();
    LoadRightOrder();
    g_btNext = 0;
    UpdateStats();
    UpdateWifi();
    UpdateMedia();
    UpdateLang();
    BuildContent();
    CreateBars();
}

static void OnTick() {
    UpdateStats();
    UpdateTimers();
    UpdateMedia();
    ULONGLONG now = GetTickCount64();
    if (g_s.showBt && now >= g_btNext) {
        g_btNext = now + 15000;
        if (!g_btBusy.exchange(true)) StartWorker(BtWorker, nullptr);
    }
    if (BuildContent()) MarkAll();
    if (g_pulseActive) EnsureAnim();
    FlushBars();
}

static void OnSlow() {
    if (!g_wifiBusy.exchange(true)) StartWorker(WifiWorker, nullptr);
    if (ResolveColors()) {
        for (auto* b : g_bars) ApplyEffect(b);
        MarkAll();
    }
    bool changed = false;
    for (auto& p : g_pins) {
        FILETIME mt = FileMTime(p.path);
        if (FtU64(mt) != FtU64(p.mtime)) {
            if (p.icon) DestroyIcon(p.icon);
            p.icon = LoadBigIcon(p.path);
            p.mtime = mt;
            changed = true;
        }
    }
    for (auto* b : g_bars) {
        UINT dx = 96, dy = 96;
        if (SUCCEEDED(GetDpiForMonitor(b->mon, MDT_EFFECTIVE_DPI, &dx, &dy)) && dx && dx != b->dpi) {
            SetTimer(g_ctl, TM_REBUILD, 300, nullptr);
            break;
        }
    }
    if (changed || BuildContent()) MarkAll();
    FlushBars();
}

static UINT g_msgTaskbarCreated = 0;

static LRESULT CALLBACK CtlProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (g_msgTaskbarCreated && msg == g_msgTaskbarCreated) {  // Explorer restarted: the appbar must be re-registered
        SetTimer(hwnd, TM_REBUILD, 1000, nullptr);
        return 0;
    }
    switch (msg) {
        case WM_TIMER:
            if (wp == TM_TICK) OnTick();
            else if (wp == TM_SLOW) OnSlow();
            else if (wp == TM_FS) {
                CheckFullscreen();
                PollToasts();
                PollLang();
            }
            else if (wp == TM_ANIM) AnimTick();
            else if (wp == TM_KEY) PollWinKey();
            else if (wp == TM_REBUILD) {
                if (g_modalDepth > 0) return 0;  // a menu is open: try again on the next tick
                KillTimer(hwnd, TM_REBUILD);
                CreateBars();
            }
            return 0;
        case WM_RELOAD:
            if (g_modalDepth > 0) {  // a menu is open: apply the new settings after it closes
                g_reloadPending = true;
                return 0;
            }
            ApplyAll();
            return 0;
        case WM_CLOSE:
            if (g_modalDepth > 0) {  // close the menu first, then shut down
                g_closePending = true;
                EndMenu();
                return 0;
            }
            DestroyWindow(hwnd);
            return 0;
        case WM_DISPLAYCHANGE:
            SetTimer(hwnd, TM_REBUILD, 500, nullptr);
            return 0;
        case WM_SETTINGCHANGE:
            if (ResolveColors()) {
                MarkAll();
                FlushBars();
            }
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, TM_TICK);
            KillTimer(hwnd, TM_SLOW);
            KillTimer(hwnd, TM_FS);
            KillTimer(hwnd, TM_ANIM);
            KillTimer(hwnd, TM_KEY);
            DestroyBars();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI MainThread(LPVOID) {
    if (g_stop) return 0;

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    bool ole = SUCCEEDED(OleInitialize(nullptr));
    g_msgTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

    HINSTANCE hi = ModInst();
    WNDCLASSW wc{};
    wc.hInstance = hi;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    auto reg = [&](WNDPROC proc, const wchar_t* name, UINT style) {
        wc.lpfnWndProc = proc;
        wc.lpszClassName = name;
        wc.style = style;
        return RegisterClassW(&wc) != 0;
    };
    bool classesOk = reg(BarProc, kBarClass, 0) && reg(FxProc, kFxClass, 0) &&
                     reg(CalProc, kCalClass, CS_DROPSHADOW) && reg(CtlProc, kCtlClass, 0);

    if (classesOk) {
        g_ctl = CreateWindowExW(WS_EX_TOOLWINDOW, kCtlClass, L"WH Top Bar Controller", WS_POPUP, 0, 0, 0, 0,
                                nullptr, nullptr, hi, nullptr);
        HWND ctl = g_ctl;
        if (ctl) {
            if (g_stop) {
                DestroyWindow(ctl);  // unloading was requested while the thread was starting
            } else {
                ApplyAll();
                SetTimer(ctl, TM_TICK, 1000, nullptr);
                SetTimer(ctl, TM_SLOW, 5000, nullptr);
                SetTimer(ctl, TM_FS, 500, nullptr);
                SetTimer(ctl, TM_KEY, 60, nullptr);
                StartWorker(VolumeWorker, nullptr);
            }
            MSG msg;
            while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            g_ctl = nullptr;
        }
    } else {
        Wh_Log(L"RegisterClassW failed: %lu", GetLastError());
    }
    ClearIcons();
    UnregisterClassW(kBarClass, hi);
    UnregisterClassW(kFxClass, hi);
    UnregisterClassW(kCalClass, hi);
    UnregisterClassW(kCtlClass, hi);
    if (ole) OleUninitialize();
    return 0;
}

// ------------------------------------------------------------ Windhawk API ----

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");
    InitializeCriticalSection(&g_cs);
    g_stop = false;
    g_thread = CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    if (!g_thread) {
        DeleteCriticalSection(&g_cs);
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    g_stop = true;
    HWND h = g_ctl;
    if (h) PostMessageW(h, WM_CLOSE, 0, 0);

    // Wait for the UI thread and for every worker thread. The wait is bounded because
    // a stuck file system call must not hang Windhawk; the tool process exits right
    // after this function returns, so nothing keeps running inside the mod.
    bool clean = true;
    if (g_thread) {
        if (WaitForSingleObject(g_thread, 10000) != WAIT_OBJECT_0) clean = false;
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_workersMutex);
        threads.swap(g_workerThreads);
    }
    for (HANDLE t : threads) {
        if (WaitForSingleObject(t, 5000) != WAIT_OBJECT_0) clean = false;
        CloseHandle(t);
    }
    if (clean) DeleteCriticalSection(&g_cs);
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    HWND h = g_ctl;
    if (h) PostMessageW(h, WM_RELOAD, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
