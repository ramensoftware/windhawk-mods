// ==WindhawkMod==
// @id              usagi-taskbar-island
// @name            Usagi Taskbar Island
// @description     Modular Fluent Design taskbar island for Windows 11 (Media Player & WireGuard VPN toggle with Split Tunneling).
// @description:ru-RU Модульный островок в стиле Fluent Design для панели задач Windows 11 (медиаплеер и кнопка WireGuard VPN с раздельным туннелированием).
// @version         1.0.0
// @author          usagi
// @github          https://github.com/usagi-dead
// @license         MIT
// @include         *
// @exclude         audiodg.exe
// @exclude         svchost.exe
// @exclude         dwm.exe
// @exclude         csrss.exe
// @exclude         smss.exe
// @exclude         wininit.exe
// @exclude         winlogon.exe
// @exclude         services.exe
// @exclude         lsass.exe
// @exclude         fontdrvhost.exe
// @exclude         conhost.exe
// @exclude         sihost.exe
// @exclude         ctfmon.exe
// @exclude         taskhostw.exe
// @exclude         dllhost.exe
// @exclude         WmiPrvSE.exe
// @exclude         spoolsv.exe
// @exclude         RuntimeBroker.exe
// @exclude         backgroundTaskHost.exe
// @exclude         SearchHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         LockApp.exe
// @exclude         SystemSettings.exe
// @exclude         TextInputHost.exe
// @exclude         smartscreen.exe
// @exclude         consent.exe
// @exclude         WerFault.exe
// @exclude         SecurityHealthService.exe
// @exclude         SecurityHealthSystray.exe
// @exclude         MsMpEng.exe
// @exclude         NisSrv.exe
// @exclude         NVDisplay.Container.exe
// @exclude         GameInputRedistService.exe
// @exclude         windhawk.exe
// @exclude         windhawk-ui.exe
// @exclude         wireguard.exe
// @exclude         *\Windows\System32\DriverStore\*
// @exclude         *\Windows\SystemApps\*
// @exclude         *\Program Files\Microsoft GameInput\*
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -luser32 -lwindowsapp -lshell32 -lshlwapi -lshcore -ladvapi32 -lws2_32 -liphlpapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Usagi Taskbar Island

A modular Fluent Design island for the **Windows 11 taskbar**, combining a customizable **Media Bar + Mini-Player Popup** and a passwordless **WireGuard VPN toggle** with per-app **Split Tunneling**.

[Русская документация ниже](#русская-документация)

---

## Features

- **Taskbar Island (`Island`):**
  - Place in **unified** (single pill) or **separate** (Media & VPN apart) capsule mode on any taskbar edge (`Left edge`, `Center`, `Right edge`, `System Tray Left/Right`).
  - Full **multi-monitor support**: show on a specific monitor (`Island -> Monitor number`, `1` = Primary) or on **all monitors** simultaneously.
  - Background styles: **Transparent**, **Solid color**, or Windows 11 **Acrylic glass**, plus configurable height, padding, corner radius, borders, and `ThemeShadow`.
- **Media Bar (`MediaBar`):**
  - Displays album cover art (square or adaptive `1:1` / `16:9`), track title & artist (with optional smooth marquee scrolling), and transport controls (`Prev`, `Play/Pause`, `Next`, plus optional `Shuffle`, `Repeat`, `Library`).
  - Toggle any individual element on/off and fine-tune every width, font size, gap, and padding.
  - Native Windows 11 tile hover highlight, customizable colors (`auto` or `HEX/RGB`), mouse wheel shortcuts (next/prev track or switch active session), and full right-click context menu.
  - Automatically maps cryptic browser/app AUMIDs (Firefox, Chrome, Edge, Opera, Yandex, etc.) to readable names.
- **Mini-Player Popup (`MiniPlayer`):**
  - Left-clicking the track title or cover art opens a borderless **Translucent** / **Fluent Acrylic** / **Solid** popup card.
  - Adaptive cover art preserving aspect ratio (`1:1`, `16:9`, portrait), interactive seek bar with timestamps, Shuffle / Repeat / Library controls, and an active media-source switcher pill bar.
  - Opens directly above the player (clamped inside screen work area) or anchored to any fixed screen corner/edge.
- **WireGuard VPN Toggle (`VpnConnection`, `VpnAppearance`):**
  - One-click **passwordless** (no UAC prompt) VPN connect/disconnect powered by on-demand Windows Task Scheduler tasks (`WindhawkVPN_Start` & `WindhawkVPN_Stop`), compatible with WireGuard and AmneziaWG tunnel services.
  - 11 built-in vector SVG icons (`shield_solid`, `shield`, `shield_check_solid`, `globe`, `world`, `cloud_check`, etc.), independent per-state icon HEX/RGB/`auto` colors, customizable status text, status indicator dot, and optional tray notifications.
- **Split Tunneling (`SplitTunneling`):**
  - Lightweight Winsock bypass (`connect` / `WSAConnect` / `sendto` / `WSASendTo` / `bind`) routing selected apps or folders directly through your physical adapter outside the VPN tunnel, with optional IPv6 blocking to prevent IPv6 leaks.
- **VK Music Integration (`vk-music-bridge.user.js`):**
  - Optional local loopback bridge (`127.0.0.1:49321`) syncing Shuffle, Repeat (`Off` / `Playlist` / `One Track`), and Add/Remove from Library with VK (`vk.com`, `vk.ru`, `music.vk.com`) in Firefox/Chrome/Edge/etc.

---

## Requirements

- **Windows 11 (x64)** with **Windhawk**.
- **Optional (for VPN module):** [WireGuard for Windows](https://www.wireguard.com/install/) (or AmneziaWG) and your local `.conf` tunnel profile. If you only want the Media Player, turn off `Island -> Enable VPN button module`.
- **Optional (for VK Music Shuffle/Repeat/Library sync):** A browser userscript manager such as **Violentmonkey** or **Tampermonkey** with the companion userscript.

---

## Setup Guides

### 1. WireGuard VPN (Passwordless Toggle — No UAC)
1. Install WireGuard and keep your tunnel `.conf` file in a permanent location (for example, `C:\WireGuard\my-vpn.conf`). *(Ensure the filename matches a valid WireGuard tunnel name without special characters).*
2. Open **Mod Settings -> VPN - Connection & Behavior**:
   - Verify **Path to `wireguard.exe`** (default: `C:\Program Files\WireGuard\wireguard.exe`).
   - Set **Path to `.conf` file** to your `.conf` path.
   - Click **Save settings**.
3. **Right-click** the VPN button on the taskbar and select **"Setup / update passwordless toggle (1 time)"** (or click the VPN button once and approve UAC). Approve the single UAC prompt to register `WindhawkVPN_Start` and `WindhawkVPN_Stop` in Windows Task Scheduler.
4. All subsequent left-clicks toggle VPN silently without UAC prompts. If you later move or rename your `.conf` file, run that context-menu item once more.

### 2. Split Tunneling (App Bypass)
1. Open **Mod Settings -> Split Tunneling** and enable **Enable Split Tunneling**.
2. Add rules to **Excluded applications**:
   - Exact executable name: `firefox.exe`
   - Process stem: `steam`
   - Folder prefix: `C:\Games\` (applies to executables launched from inside that folder)
   - Make sure **Enabled** is checked for each entry.
3. Click **Save settings** and **restart** any newly added applications so Windhawk's hook activates inside them.
4. *(Optional Security Tip)* Because Split Tunneling uses `@include *` with system exclusions, if Windows Smart App Control / Code Integrity logs blocked unsigned DLL loads in unrelated third-party apps, you can open Windhawk's **Advanced** tab for this mod and restrict the **Custom process inclusion list** strictly to `explorer.exe` plus your excluded apps.

### 3. VK Music Bridge (Shuffle, Repeat & Library)
Standard Windows media controls (SMTC) do not expose VK's Shuffle, Repeat, or Add-to-Library state. To enable two-way sync:
1. Ensure `MediaBar -> Style, controls & VK Bridge -> Enable VK Music HTTP Bridge` is enabled (`true`, port `49321` by default).
2. Install **Violentmonkey** or **Tampermonkey** in your browser.
3. Install the companion userscript: [vk-music-bridge.user.js](https://raw.githubusercontent.com/usagi-dead/usagi-taskbar-island/main/vk-music-bridge.user.js).
4. Refresh open VK tabs (`F5`). Only the actively playing tab reports state (`localStorage` leader election).

---

## Controls & Quick Tips
- **Left-click track cover/title:** Open / close the Mini-Player popup.
- **Right-click Media Bar:** Full playback context menu (Play/Pause, Next/Prev, Stop, -/+5s, Shuffle, Repeat submenu, Library toggle, active media source selector).
- **Right-click VPN button:** Connect/Disconnect, Setup passwordless task, Open WireGuard GUI, Open `.conf` file.
- **Multi-monitor:** Default is **Monitor `1` (Primary)**. If you use a second monitor, change `Island -> Monitor number` to `2`, or switch `Monitor display mode` to `All monitors`.

---

## Русская документация

Модульный островок в стиле **Fluent Design** для **панели задач Windows 11**. Объединяет компактный **Медиаплеер** со всплывающим окном полного просмотра и кнопку **WireGuard VPN** с переключением без UAC и раздельным туннелированием (Split Tunneling).

### Основные возможности
- **Островок (`Island`):** единая капсула или раздельные кнопки (Плеер и VPN отдельно), выбор монитора (`1`, `2`, или все мониторы), позиция на панели (`Левый край`, `Центр`, `Правый край`, `Трей`), стиль фона (`Прозрачный`, `Сплошной`, `Acrylic`), скругление углов, рамка и объёмная тень `ThemeShadow`.
- **Медиаплеер на панели (`MediaBar`):** обложка (квадратная или адаптивная `1:1`/`16:9`), название и исполнитель (с плавной бегущей строкой), гибкое включение/отключение любых кнопок и точные отступы, нативный hover Win11, действие колеса мыши, контекстное меню ПКМ и понятные названия приложений (`Firefox`, `Chrome`, `Edge` и др. вместо ID).
- **Окно полного просмотра (`MiniPlayer`):** безрамочное окно **Translucent** / **Acrylic** / **Сплошное**, большая адаптивная обложка, прогресс-бар с перемоткой, переключение между запущенными источниками медиа, кнопки `Перемешать`, `Повтор` и `В библиотеку`.
- **Кнопка WireGuard VPN (`VpnConnection`, `VpnAppearance`):** переключение туннеля в 1 клик без окна UAC через Планировщик заданий (`WindhawkVPN_Start` / `WindhawkVPN_Stop`), совместимость с WireGuard и AmneziaWG, 11 векторных SVG-иконок с отдельным цветом под каждый статус (`#RRGGBB` или `"auto"`), текст, точка-индикатор и уведомления.
- **Раздельное туннелирование (`SplitTunneling`):** направляет выбранные программы или папки напрямую в интернет через физический сетевой адаптер мимо VPN (с опциональной блокировкой IPv6 от утечек).
- **Мост ВК Музыки (`vk-music-bridge.user.js`):** локальный HTTP-мост (`127.0.0.1:49321`) для двусторонней синхронизации `Перемешать`, `Повтор` (`Выкл` / `Плейлист` / `Один трек`) и `Добавить/убрать из библиотеки` на `vk.com`, `vk.ru` и `music.vk.com`.

### Что понадобится для настройки

#### 1. WireGuard VPN (переключение без UAC)
1. Установите [WireGuard для Windows](https://www.wireguard.com/install/) (или AmneziaWG) и положите свой `.conf` файл в постоянную папку (например, `C:\WireGuard\my-vpn.conf`). Если вам нужен только медиаплеер — просто отключите `Island -> Включить модуль кнопки VPN`.
2. Откройте настройки мода **VPN — Подключение и поведение**:
   - Проверьте **Путь к `wireguard.exe`** (по умолчанию `C:\Program Files\WireGuard\wireguard.exe`).
   - Введите полный путь в поле **Путь к файлу `.conf`**.
   - Нажмите **Сохранить настройки**.
3. Нажмите **ПКМ** по кнопке VPN на панели задач и выберите **«Настроить / обновить запуск без UAC (1 раз)»**, один раз подтвердив UAC. После этого переключение VPN по ЛКМ будет происходить мгновенно без запросов прав администратора.

#### 2. Раздельное туннелирование (Split Tunneling)
1. В разделе **Split Tunneling** включите **Включить раздельное туннелирование**.
2. В списке **Исключённые приложения** добавьте нужные процессы: имя exe (`firefox.exe`), имя без расширения (`steam`) или путь к папке (`C:\Games\`), и отметьте **Включено**.
3. Сохраните настройки и **перезапустите** добавленные программы.
4. *(По желанию)* На вкладке **Дополнительно** (Advanced) настроек Windhawk можно указать свой список включения процессов (`explorer.exe` и ваши приложения из Split Tunneling), чтобы не загружать DLL мода в посторонние процессы.

#### 3. Синхронизация с ВК Музыкой (`vk-music-bridge.user.js`)
1. Убедитесь, что включена опция `Включить мост ВК Музыки` (порт `49321`).
2. Установите в браузер расширение **Violentmonkey** или **Tampermonkey**.
3. Установите скрипт [vk-music-bridge.user.js](https://raw.githubusercontent.com/usagi-dead/usagi-taskbar-island/main/vk-music-bridge.user.js) и обновите вкладки с VK (`F5`).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Island:
  - enableMediaPlayer: true
    $name: Enable Media Player module
    $name:ru-RU: Включить модуль медиаплеера
  - enableVpnButton: true
    $name: Enable VPN button module
    $name:ru-RU: Включить модуль кнопки VPN
  - monitorMode: "single"
    $name: Monitor display mode
    $name:ru-RU: Режим мониторов
    $options:
    - "single": "Single monitor"
    - "all": "All monitors"
    $options:ru-RU:
    - "single": "Только выбранный монитор"
    - "all": "Все мониторы"
  - monitor: 1
    $name: Monitor number (for Single monitor mode)
    $name:ru-RU: Номер монитора (для режима одного монитора)
    $description: "1 = Primary monitor, 2 = Second monitor, etc."
    $description:ru-RU: "1 = Основной монитор, 2 = Второй монитор и т.д."
  - position: "taskbar_left_edge"
    $name: Island position
    $name:ru-RU: Положение островка
    $options:
    - "taskbar_left_edge": "Taskbar - Left edge (Overlay)"
    - "taskbar_center_edge": "Taskbar - Center (Overlay)"
    - "taskbar_right_edge": "Taskbar - Right edge (Overlay)"
    - "tray_left": "System Tray - Left"
    - "tray_right": "System Tray - Right"
    $options:ru-RU:
    - "taskbar_left_edge": "Панель задач - Левый край (вплотную)"
    - "taskbar_center_edge": "Панель задач - Центр"
    - "taskbar_right_edge": "Панель задач - Правый край"
    - "tray_left": "Трей - Слева"
    - "tray_right": "Трей - Справа"
  - marginLeft: 0
    $name: Outer left margin from taskbar edge (px)
    $name:ru-RU: Отступ от левого края экрана (пикс.)
  - marginRight: 0
    $name: Outer right margin (px)
    $name:ru-RU: Внешний отступ справа (пикс.)
  - height: 40
    $name: Island height (px)
    $name:ru-RU: Высота островка (пикс.)
  - capsuleMode: "unified"
    $name: Capsule layout mode
    $name:ru-RU: Режим капсул островка
    $options:
    - "unified": "Unified island (shared pill)"
    - "separate": "Separate pills (Media & VPN apart)"
    $options:ru-RU:
    - "unified": "Единый островок (общий фон и рамка)"
    - "separate": "Раздельные капсулы (отдельно для Плеера и VPN)"
  - moduleSpacing: 6
    $name: Spacing between Media & VPN modules (px)
    $name:ru-RU: Расстояние между Плеером и VPN (пикс.)
  - showModuleDivider: false
    $name: Show vertical divider between modules
    $name:ru-RU: Показывать разделитель между модулями
  - paddingX: 4
    $name: Island horizontal inner padding (px)
    $name:ru-RU: Внутренний боковой отступ контейнера (пикс.)
  - paddingY: 0
    $name: Island vertical inner padding (px)
    $name:ru-RU: Внутренний вертикальный отступ контейнера (пикс.)
  - cornerRadius: 6
    $name: Corner radius (px)
    $name:ru-RU: Радиус скругления углов (пикс.)
  - backgroundStyle: "transparent"
    $name: Island background style
    $name:ru-RU: Стиль фона островка
    $options:
    - "transparent": "Transparent"
    - "solid": "Solid color"
    - "acrylic": "Acrylic glass (Windows 11)"
    $options:ru-RU:
    - "transparent": "Прозрачный"
    - "solid": "Сплошной цвет"
    - "acrylic": "Акриловое стекло (Windows 11)"
  - backgroundColor: "#232323"
    $name: Background / Acrylic tint color (HEX or R G B)
    $name:ru-RU: Цвет фона / тинта акрила (HEX или R G B)
  - backgroundOpacity: 0
    $name: Background opacity (0-100)
    $name:ru-RU: Непрозрачность фона (0-100)
  - borderColorMode: "auto"
    $name: Border color mode
    $name:ru-RU: Цвет рамки
    $options:
    - "auto": "Auto (match Windows theme)"
    - "custom": "Custom HEX / RGB"
    $options:ru-RU:
    - "auto": "Авто (по теме Windows)"
    - "custom": "Свой цвет (HEX / RGB)"
  - borderColor: "#FFFFFF"
    $name: Custom border color (HEX or R G B)
    $name:ru-RU: Свой цвет рамки (HEX или R G B)
  - borderOpacity: 0
    $name: Border opacity (0-100)
    $name:ru-RU: Непрозрачность рамки (0-100)
  - borderThickness: 0
    $name: Border thickness (0-3 px)
    $name:ru-RU: Толщина рамки (0-3 пикс.)
  - enableShadow: false
    $name: Enable floating shadow (ThemeShadow)
    $name:ru-RU: Объёмная тень островка (ThemeShadow)
  $name: Island — Position & Style
  $name:ru-RU: Островок — Позиция и стиль

- MediaBar:
  - Elements:
    - showAlbumArt: true
      $name: Show album cover art
      $name:ru-RU: Показывать обложку трека
    - showTitle: true
      $name: Show track title
      $name:ru-RU: Показывать название трека
    - showArtist: true
      $name: Show artist name
      $name:ru-RU: Показывать исполнителя
    - showPlaybackButtons: true
      $name: Show Prev / Play-Pause / Next buttons
      $name:ru-RU: Показывать кнопки Пред. / Пауза / След.
    - showShuffleButton: false
      $name: Show Shuffle button
      $name:ru-RU: Вынести кнопку «Перемешать»
    - showRepeatButton: false
      $name: Show Repeat button
      $name:ru-RU: Вынести кнопку «Повтор»
    - showLikeButton: false
      $name: Show Add to Library button (VK)
      $name:ru-RU: Вынести кнопку «В библиотеку» (ВК)
    $name: Visible elements
    $name:ru-RU: Отображаемые элементы
  - Layout:
    - paddingLeft: 4
      $name: Player left padding (px)
      $name:ru-RU: Внутренний отступ плеера слева (пикс.)
    - paddingRight: 5
      $name: Player right padding (px)
      $name:ru-RU: Внутренний отступ плеера справа (пикс.)
    - artToTextGap: 8
      $name: Gap between cover art and text (px)
      $name:ru-RU: Отступ между обложкой и текстом (пикс.)
    - textToControlsGap: 8
      $name: Gap between track info and buttons (px)
      $name:ru-RU: Отступ между инфо о треке и кнопками (пикс.)
    - buttonSpacing: 2
      $name: Spacing between control buttons (px)
      $name:ru-RU: Расстояние между кнопками (пикс.)
    - artSize: 32
      $name: Album art height / size (px)
      $name:ru-RU: Размер обложки (пикс.)
    - artCornerRadius: 5
      $name: Album art corner radius (px)
      $name:ru-RU: Скругление обложки (пикс.)
    - adaptiveArtWidth: false
      $name: Adaptive album art width (1:1 / 16:9)
      $name:ru-RU: Адаптивная ширина обложки (1:1 / 16:9)
    - artMaxWidth: 58
      $name: Max adaptive album art width (px)
      $name:ru-RU: Макс. ширина обложки при адаптации (пикс.)
    - textWidth: 130
      $name: Text area width (px)
      $name:ru-RU: Ширина текстовой области (пикс.)
    - titleFontSize: 12
      $name: Title font size (px)
      $name:ru-RU: Размер шрифта названия (пикс.)
    - artistFontSize: 10
      $name: Artist font size (px)
      $name:ru-RU: Размер шрифта исполнителя (пикс.)
    - textSpacing: 1
      $name: Vertical gap between title and artist (px)
      $name:ru-RU: Вертикальный отступ между названием и автором (пикс.)
    - enableTitleMarquee: false
      $name: Smooth scrolling for long title (marquee)
      $name:ru-RU: Плавная прокрутка длинного названия
    - enableArtistMarquee: false
      $name: Smooth scrolling for long artist (marquee)
      $name:ru-RU: Плавная прокрутка длинного имени автора
    - buttonSize: 26
      $name: Control button size (px)
      $name:ru-RU: Размер кнопок управления (пикс.)
    $name: Sizes & spacing
    $name:ru-RU: Размеры и отступы
  - Style:
    - hoverEffect: "auto"
      $name: Player hover effect (Win11 tile style)
      $name:ru-RU: Эффект при наведении на плеер (подсветка Win11)
      $options:
      - "auto": "Auto (match Windows theme)"
      - "dark": "Dark theme highlight"
      - "light": "Light theme highlight"
      - "off": "Disabled"
      $options:ru-RU:
      - "auto": "Авто (по теме Windows)"
      - "dark": "Тёмный"
      - "light": "Светлый"
      - "off": "Выключен"
    - hoverAnimation: true
      $name: Smooth hover animation
      $name:ru-RU: Плавная анимация наведения
    - colorMode: "auto"
      $name: Color mode
      $name:ru-RU: Цвета текста и кнопок
      $options:
      - "auto": "Auto (match Windows theme)"
      - "custom": "Custom HEX / RGB"
      $options:ru-RU:
      - "auto": "Авто (по теме Windows)"
      - "custom": "Свои цвета (HEX / RGB)"
    - textColor: "#FFFFFF"
      $name: Title text color (HEX or R G B)
      $name:ru-RU: Цвет названия трека (HEX или R G B)
    - artistColor: "#B8B8B8"
      $name: Artist text color (HEX or R G B)
      $name:ru-RU: Цвет исполнителя (HEX или R G B)
    - buttonColor: "#FFFFFF"
      $name: Buttons color (HEX or R G B)
      $name:ru-RU: Цвет кнопок (HEX или R G B)
    - accentColor: "#4CC2FF"
      $name: Accent color (Shuffle / Repeat / Library / Seek bar)
      $name:ru-RU: Акцентный цвет (Шафл / Повтор / Библиотека / Прогресс)
    - enableContextMenu: true
      $name: Enable right-click context menu
      $name:ru-RU: Включить контекстное меню по ПКМ
    - mouseWheelAction: "none"
      $name: Mouse wheel action over Media Bar
      $name:ru-RU: Действие колеса мыши над плеером
      $options:
      - "none": "Do nothing"
      - "track": "Next / Previous track"
      - "session": "Switch media source (session)"
      $options:ru-RU:
      - "none": "Отключено"
      - "track": "Следующий / Предыдущий трек"
      - "session": "Переключить источник (сессию)"
    - enableVkBridge: true
      $name: Enable VK Music HTTP Bridge (Shuffle, Repeat, Library)
      $name:ru-RU: Включить мост ВК Музыки (Шафл, Повтор, Библиотека)
    - vkBridgePort: 49321
      $name: VK Bridge local port
      $name:ru-RU: Локальный порт моста ВК Музыки
    $name: Style, controls & VK Bridge
    $name:ru-RU: Стиль, управление и мост ВК
  $name: Media Bar — Taskbar Player
  $name:ru-RU: Медиаплеер — Панель задач

- MiniPlayer:
  - enabled: true
    $name: Open Mini-Player popup on left click (art / text)
    $name:ru-RU: Открывать полный просмотр по ЛКМ (обложка / текст)
  - windowStyle: "translucent"
    $name: Window glass style
    $name:ru-RU: Стиль окна полного просмотра
    $options:
    - "translucent": "Translucent glass (borderless backdrop blur)"
    - "acrylic": "Fluent Acrylic (stronger tint)"
    - "solid": "Solid background"
    $options:ru-RU:
    - "translucent": "Translucent стекло (глубокое размытие без рамок)"
    - "acrylic": "Fluent Acrylic (плотнее)"
    - "solid": "Сплошной фон"
  - showBorder: false
    $name: Show thin 1px window border
    $name:ru-RU: Показывать тонкую рамку вокруг окна
  - tintOpacity: 25
    $name: Background tint / opacity (5-95%)
    $name:ru-RU: Плотность тонировки фона (5-95%)
  - cornerRadius: 12
    $name: Window corner radius (px)
    $name:ru-RU: Скругление углов окна (пикс.)
  - width: 296
    $name: Popup width (px)
    $name:ru-RU: Ширина окна (пикс.)
  - paddingX: 18
    $name: Horizontal inner padding (px)
    $name:ru-RU: Боковой внутренний отступ (пикс.)
  - paddingY: 16
    $name: Vertical inner padding (px)
    $name:ru-RU: Вертикальный внутренний отступ (пикс.)
  - elementSpacing: 12
    $name: Spacing between popup sections (px)
    $name:ru-RU: Отступ между блоками окна (пикс.)
  - showSessionBar: true
    $name: Show media source switcher (when multiple players run)
    $name:ru-RU: Показывать переключатель источников медиа
  - showAlbumArt: true
    $name: Show large cover art
    $name:ru-RU: Показывать большую обложку
  - adaptiveArtSize: true
    $name: Adaptive cover aspect ratio (1:1 / 16:9 / portrait)
    $name:ru-RU: Адаптивные пропорции обложки (подстраивается под 1:1 / 16:9)
  - artMaxHeight: 220
    $name: Max cover art height (px)
    $name:ru-RU: Максимальная высота обложки (пикс.)
  - artCornerRadius: 10
    $name: Cover art corner radius (px)
    $name:ru-RU: Скругление большой обложки (пикс.)
  - showArtist: true
    $name: Show artist name
    $name:ru-RU: Показывать исполнителя
  - showLikeButton: true
    $name: Show Add to Library button (VK)
    $name:ru-RU: Показывать кнопку «В библиотеку»
  - showSeekBar: true
    $name: Show seek progress bar & timestamps
    $name:ru-RU: Показывать прогресс-бар и таймкоды
  - showShuffleButton: true
    $name: Show Shuffle button
    $name:ru-RU: Показывать кнопку «Перемешать»
  - showRepeatButton: true
    $name: Show Repeat button
    $name:ru-RU: Показывать кнопку «Повтор»
  - placementMode: "near"
    $name: Popup placement mode
    $name:ru-RU: Позиция окна на экране
    $options:
    - "near": "Above the player (auto-clamped to screen)"
    - "screen": "Fixed position on screen"
    $options:ru-RU:
    - "near": "Над плеером (с защитой от краёв экрана)"
    - "screen": "Фиксированный угол экрана"
  - horizontalAlign: "left"
    $name: Horizontal screen anchor (fixed position mode)
    $name:ru-RU: Горизонтальное выравнивание (фиксированный режим)
    $options:
    - "left": "Left"
    - "center": "Center"
    - "right": "Right"
    $options:ru-RU:
    - "left": "Слева"
    - "center": "По центру"
    - "right": "Справа"
  - verticalAlign: "bottom"
    $name: Vertical screen anchor (fixed position mode)
    $name:ru-RU: Вертикальное выравнивание (фиксированный режим)
    $options:
    - "bottom": "Bottom (above taskbar)"
    - "center": "Center"
    - "top": "Top"
    $options:ru-RU:
    - "bottom": "Снизу (над панелью задач)"
    - "center": "По центру"
    - "top": "Сверху"
  - edgeOffsetX: 12
    $name: Horizontal screen edge margin (px)
    $name:ru-RU: Горизонтальный отступ от края экрана (пикс.)
  - edgeOffsetY: 12
    $name: Vertical screen edge margin (px)
    $name:ru-RU: Вертикальный отступ от края экрана (пикс.)
  - openAnimation: true
    $name: Smooth open reveal animation
    $name:ru-RU: Плавная анимация появления окна
  $name: Mini-Player — Full View Popup
  $name:ru-RU: Полный просмотр — Мини-плеер

- VpnConnection:
  - wireguardExePath: "C:\\Program Files\\WireGuard\\wireguard.exe"
    $name: Path to wireguard.exe
    $name:ru-RU: Путь к wireguard.exe
  - configPath: ""
    $name: Path to .conf file
    $name:ru-RU: Путь к файлу .conf
    $description: "Full path to your WireGuard .conf file (e.g. C:\\WireGuard\\my-vpn.conf). After changing, right-click the VPN button to update passwordless tasks."
    $description:ru-RU: "Полный путь к .conf файлу WireGuard (например, C:\\WireGuard\\my-vpn.conf). После изменения нажмите ПКМ по кнопке VPN для обновления задач без UAC."
  - showNotifications: true
    $name: Show notification on VPN state change
    $name:ru-RU: Показывать уведомление при смене состояния VPN
  - silentNotifications: true
    $name: Silent notifications (no sound)
    $name:ru-RU: Тихие уведомления (без звука)
  - showTooltip: false
    $name: Show tooltip on hover
    $name:ru-RU: Показывать подсказку при наведении
  - enableContextMenu: true
    $name: Enable right-click context menu
    $name:ru-RU: Включить контекстное меню по ПКМ
  $name: VPN — Connection & Behavior
  $name:ru-RU: VPN — Подключение и поведение

- VpnAppearance:
  - buttonPaddingX: 8
    $name: Button horizontal padding (px)
    $name:ru-RU: Внутренний боковой отступ кнопки VPN (пикс.)
  - showIcon: true
    $name: Show icon
    $name:ru-RU: Показывать иконку
  - iconConnected: "shield_solid"
    $name: Icon when connected
    $name:ru-RU: Иконка при включенном VPN
    $options:
    - "shield_check_solid": "Shield with check (Solid)"
    - "shield_exclamation_solid": "Shield with exclamation (Solid)"
    - "shield_solid": "Shield (Solid)"
    - "shield": "Shield (Outline)"
    - "shield_exclamation": "Shield with exclamation (Outline)"
    - "globe": "Globe"
    - "world": "World"
    - "cloud_check": "Cloud Check"
    - "database": "Server / Database"
    - "physics": "Physics / Atom"
    - "exclamation": "Exclamation"
    $options:ru-RU:
    - "shield_check_solid": "Щит с галочкой (залитый)"
    - "shield_exclamation_solid": "Щит с восклицанием (залитый)"
    - "shield_solid": "Щит (залитый)"
    - "shield": "Щит (контурный)"
    - "shield_exclamation": "Щит с восклицанием (контурный)"
    - "globe": "Глобус (сетка)"
    - "world": "Планета (карта)"
    - "cloud_check": "Облако с галочкой"
    - "database": "Сервер / База данных"
    - "physics": "Атом"
    - "exclamation": "Восклицание"
  - iconDisconnected: "shield"
    $name: Icon when disconnected
    $name:ru-RU: Иконка при выключенном VPN
    $options:
    - "same": "Same as Connected"
    - "shield_check_solid": "Shield with check (Solid)"
    - "shield_exclamation_solid": "Shield with exclamation (Solid)"
    - "shield_solid": "Shield (Solid)"
    - "shield": "Shield (Outline)"
    - "shield_exclamation": "Shield with exclamation (Outline)"
    - "globe": "Globe"
    - "world": "World"
    - "cloud_check": "Cloud Check"
    - "database": "Server / Database"
    - "physics": "Physics / Atom"
    - "exclamation": "Exclamation"
    $options:ru-RU:
    - "same": "Та же, что при включенном"
    - "shield_check_solid": "Щит с галочкой (залитый)"
    - "shield_exclamation_solid": "Щит с восклицанием (залитый)"
    - "shield_solid": "Щит (залитый)"
    - "shield": "Щит (контурный)"
    - "shield_exclamation": "Щит с восклицанием (контурный)"
    - "globe": "Глобус (сетка)"
    - "world": "Планета (карта)"
    - "cloud_check": "Облако с галочкой"
    - "database": "Сервер / База данных"
    - "physics": "Атом"
    - "exclamation": "Восклицание"
  - iconSize: 14
    $name: Icon size (px)
    $name:ru-RU: Размер иконки (пикс.)
  - iconColorConnected: "#FFFFFF"
    $name: Icon color — Connected (HEX, RGB, or "auto")
    $name:ru-RU: Цвет иконки — Включен (HEX, RGB или "auto")
    $description: Custom HEX (#RRGGBB / #RGB / #AARRGGBB), R G B, or "auto" to follow status color.
    $description:ru-RU: Свой цвет HEX (#RRGGBB / #RGB / #AARRGGBB), R G B или "auto" для автоцвета.
  - iconColorTransitioning: "#F39C12"
    $name: Icon color — Transitioning (HEX, RGB, or "auto")
    $name:ru-RU: Цвет иконки — Переключение (HEX, RGB или "auto")
    $description: Custom HEX (#RRGGBB / #RGB / #AARRGGBB), R G B, or "auto" to follow status color.
    $description:ru-RU: Свой цвет HEX (#RRGGBB / #RGB / #AARRGGBB), R G B или "auto" для автоцвета.
  - iconColorDisconnected: "#FFFFFF"
    $name: Icon color — Disconnected (HEX, RGB, or "auto")
    $name:ru-RU: Цвет иконки — Выключен (HEX, RGB или "auto")
    $description: Custom HEX (#RRGGBB / #RGB / #AARRGGBB), R G B, or "auto" to match taskbar theme.
    $description:ru-RU: Свой цвет HEX (#RRGGBB / #RGB / #AARRGGBB), R G B или "auto" для цвета темы.
  - showText: true
    $name: Show text label
    $name:ru-RU: Показывать текст
  - textConnected: "VPN ON"
    $name: Text when connected
    $name:ru-RU: Текст при включенном VPN
  - textDisconnected: "VPN OFF"
    $name: Text when disconnected
    $name:ru-RU: Текст при выключенном VPN
  - fontSize: 12
    $name: Text font size (px)
    $name:ru-RU: Размер текста (пикс.)
  - fontWeight: "regular"
    $name: Text weight
    $name:ru-RU: Насыщенность шрифта
    $options:
    - "regular": "Regular"
    - "medium": "Medium"
    - "semibold": "SemiBold"
    $options:ru-RU:
    - "regular": "Обычный (Regular)"
    - "medium": "Средний (Medium)"
    - "semibold": "Полужирный (SemiBold)"
  - textColorMode: "auto"
    $name: Text color mode
    $name:ru-RU: Цвет текста
    $options:
    - "auto": "Auto (match Windows theme)"
    - "custom": "Custom HEX / RGB"
    $options:ru-RU:
    - "auto": "Авто (по теме Windows)"
    - "custom": "Свой цвет (HEX / RGB)"
  - textColorConnected: "#FFFFFF"
    $name: Custom text color (Connected)
    $name:ru-RU: Свой цвет текста (VPN включен)
  - textColorDisconnected: "#D0D0D0"
    $name: Custom text color (Disconnected)
    $name:ru-RU: Свой цвет текста (VPN выключен)
  - showStatusDot: true
    $name: Show status indicator dot
    $name:ru-RU: Показывать точку-индикатор статуса
  - dotPosition: "right"
    $name: Indicator dot position
    $name:ru-RU: Позиция индикатора
    $options:
    - "right": "Right of text"
    - "left": "Left of text"
    $options:ru-RU:
    - "right": "Справа от текста"
    - "left": "Слева от текста"
  - dotSize: 7
    $name: Indicator dot size (px)
    $name:ru-RU: Размер индикатора (пикс.)
  - dotSpacing: 6
    $name: Gap between text and dot (px)
    $name:ru-RU: Отступ индикатора от текста (пикс.)
    $description: "Fixed spacing between the text label and the status indicator dot."
    $description:ru-RU: "Фиксированный отступ между текстом и точкой-индикатором."
  - dotOffsetY: 0
    $name: Vertical dot offset (px, -8..8)
    $name:ru-RU: Вертикальная коррекция точки (пикс., -8..8)
    $description: "Fine-tune vertical alignment of the status dot relative to text."
    $description:ru-RU: "Тонкая подстройка вертикального положения точки относительно текста."
  - colorConnected: "#2ECC71"
    $name: Indicator color — Connected (HEX or R G B)
    $name:ru-RU: Цвет индикатора — Включен (HEX или R G B)
  - colorTransitioning: "#F39C12"
    $name: Indicator color — Transitioning (HEX or R G B)
    $name:ru-RU: Цвет индикатора — Переключение (HEX или R G B)
  - colorDisconnected: "#9095A5A6"
    $name: Indicator color — Disconnected (HEX or R G B)
    $name:ru-RU: Цвет индикатора — Выключен (HEX или R G B)
  $name: VPN Button Appearance
  $name:ru-RU: VPN — Внешний вид кнопки

- SplitTunneling:
  - enabled: false
    $name: Enable Split Tunneling
    $name:ru-RU: Включить раздельное туннелирование
    $description: "Excluded enabled apps will bypass the VPN via your physical network adapter. Restart newly added apps after saving."
    $description:ru-RU: "Включённые ниже приложения будут выходить в сеть напрямую мимо VPN через физический адаптер. Новые приложения нужно перезапустить."
  - blockIpv6: true
    $name: Block IPv6 in excluded apps (prevents IPv6 leaks)
    $name:ru-RU: Блокировать IPv6 у исключений (защита от утечек)
  - excludedApps:
    - - process: ""
        $name: Process or folder path
        $name:ru-RU: Процесс или папка
        $description: "Process name (e.g. firefox.exe), stem (firefox), or folder path (C:\\Games\\)."
        $description:ru-RU: "Имя процесса (firefox.exe), имя без .exe (firefox) или путь к папке (C:\\Games\\)."
      - enabled: false
        $name: Enabled
        $name:ru-RU: Включено
    $name: Excluded applications (bypass VPN)
    $name:ru-RU: Исключённые приложения (мимо VPN)
  $name: Split Tunneling
  $name:ru-RU: Раздельное туннелирование (Split Tunneling)
*/
// ==/WindhawkModSettings==

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#undef GetCurrentTime
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

#include <windows.h>
#include <winsvc.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shcore.h>
#include <shobjidl.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cwctype>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#ifndef IP_UNICAST_IF
#define IP_UNICAST_IF 31
#endif
#ifndef IPV6_UNICAST_IF
#define IPV6_UNICAST_IF 31
#endif

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Media::Animation;

namespace {

constexpr wchar_t kIslandRootName[]  = L"UsagiTaskbarIsland";
constexpr wchar_t kTaskStartName[]   = L"WindhawkVPN_Start";
constexpr wchar_t kTaskStopName[]    = L"WindhawkVPN_Stop";
constexpr UINT    kNotifIconId       = 0x7788A0;

enum class VpnState {
    Disconnected = 0,
    Transitioning = 1,
    Connected = 2
};

enum class RepeatMode {
    Off = 0,
    All = 1,
    One = 2
};

enum class MediaCommand {
    None = 0,
    Prev = 1,
    PlayPause = 2,
    Next = 3,
    Stop = 4,
    Rewind5 = 5,
    Forward5 = 6,
    ToggleShuffle = 7,
    CycleRepeat = 8,
    SetRepeatOff = 9,
    SetRepeatAll = 10,
    SetRepeatOne = 11,
    ToggleLike = 12,
    Seek = 13,
    SwitchNextSession = 14,
    SwitchToSession = 15
};

struct SessionInfo {
    std::wstring aumid;
    std::wstring label;
    bool         isPlaying = false;
};

struct MediaSnapshot {
    bool                     hasMedia = false;
    bool                     isPlaying = false;
    std::wstring             title;
    std::wstring             artist;
    std::wstring             activeAumid;
    std::wstring             activeAppName;
    int64_t                  positionTicks = 0;
    int64_t                  durationTicks = 0;
    bool                     shuffleEnabled = false;
    RepeatMode               repeatMode = RepeatMode::Off;
    bool                     inLibrary = false;
    bool                     vkBridgeConnected = false;
    std::vector<BYTE>        thumbnailBytes;
    uint64_t                 thumbnailHash = 0;
    std::vector<SessionInfo> sessions;
};

struct ExcludedAppRule {
    std::wstring process;
    bool enabled = true;
};

struct IslandSettings {
    // Island Container
    bool         enableMediaPlayer = true;
    bool         enableVpnButton   = true;
    std::wstring monitorMode       = L"single";
    int          monitor           = 1;
    std::wstring position          = L"taskbar_left_edge";
    std::wstring capsuleMode       = L"unified";
    int          moduleSpacing     = 6;
    bool         showModuleDivider = false;
    int          marginLeft        = 0;
    int          marginRight       = 0;
    int          height            = 40;
    int          paddingX          = 4;
    int          paddingY          = 0;
    int          cornerRadius      = 6;
    std::wstring backgroundStyle   = L"transparent";
    winrt::Windows::UI::Color backgroundColor{0xFF, 0x23, 0x23, 0x23};
    int          backgroundOpacity = 0;
    std::wstring borderColorMode   = L"auto";
    winrt::Windows::UI::Color borderColor{0xFF, 0xFF, 0xFF, 0xFF};
    int          borderOpacity     = 0;
    int          borderThickness   = 0;
    bool         enableShadow      = false;

    // Media Bar (Taskbar)
    bool         mediaShowAlbumArt     = true;
    int          mediaArtSize          = 32;
    int          mediaArtCornerRadius  = 5;
    bool         mediaAdaptiveArtWidth = false;
    int          mediaArtMaxWidth      = 58;
    bool         mediaShowTitle        = true;
    bool         mediaShowArtist       = true;
    int          mediaTextWidth        = 130;
    int          mediaTitleFontSize    = 12;
    int          mediaArtistFontSize   = 10;
    int          mediaTextSpacing      = 1;
    bool         mediaTitleMarquee     = false;
    bool         mediaArtistMarquee    = false;
    int          mediaPadLeft          = 4;
    int          mediaPadRight         = 5;
    int          mediaArtToTextGap     = 8;
    int          mediaTextToControlsGap = 8;
    int          mediaButtonSpacing    = 2;
    bool         mediaShowPlaybackBtns = true;
    bool         mediaShowShuffleBtn   = false;
    bool         mediaShowRepeatBtn    = false;
    bool         mediaShowLikeBtn      = false;
    int          mediaButtonSize       = 26;
    std::wstring mediaColorMode        = L"auto";
    winrt::Windows::UI::Color mediaTextColor{0xFF, 0xFF, 0xFF, 0xFF};
    winrt::Windows::UI::Color mediaArtistColor{0xFF, 0xB8, 0xB8, 0xB8};
    winrt::Windows::UI::Color mediaButtonColor{0xFF, 0xFF, 0xFF, 0xFF};
    winrt::Windows::UI::Color mediaAccentColor{0xFF, 0x4C, 0xC2, 0xFF};
    std::wstring mediaHoverEffect      = L"auto";
    bool         mediaHoverAnimation   = true;
    bool         mediaEnableCtxMenu    = true;
    std::wstring mediaWheelAction      = L"none";
    bool         enableVkBridge        = true;
    int          vkBridgePort          = 49321;

    // Mini-Player Popup
    bool         mediaEnableMiniPlayer   = true;
    std::wstring miniPlayerWindowStyle   = L"translucent";
    bool         miniPlayerShowBorder    = false;
    int          miniPlayerTintOpacity   = 25;
    int          miniPlayerCornerRadius  = 12;
    int          miniPlayerWidth         = 296;
    int          miniPlayerPadX          = 18;
    int          miniPlayerPadY          = 16;
    int          miniPlayerElemSpacing   = 12;
    bool         miniPlayerShowSessionBar = true;
    bool         miniPlayerShowArt       = true;
    bool         miniPlayerAdaptiveArt   = true;
    int          miniPlayerArtMaxHeight  = 220;
    int          miniPlayerArtRadius     = 10;
    bool         miniPlayerShowArtist    = true;
    bool         miniPlayerShowLike      = true;
    bool         miniPlayerShowSeekBar   = true;
    bool         miniPlayerShowShuffle   = true;
    bool         miniPlayerShowRepeat    = true;
    std::wstring miniPlayerPlacement     = L"near";
    std::wstring miniPlayerHAlign        = L"left";
    std::wstring miniPlayerVAlign        = L"bottom";
    int          miniPlayerEdgeOffsetX   = 12;
    int          miniPlayerEdgeOffsetY   = 12;
    bool         miniPlayerAnimation     = true;

    // VPN Connection & Interaction
    std::wstring wgExePath           = L"C:\\Program Files\\WireGuard\\wireguard.exe";
    std::wstring configPath          = L"";
    std::wstring tunnelName          = L"";
    bool         showNotifications   = true;
    bool         silentNotifications = true;
    bool         showTooltip         = false;
    bool         enableContextMenu   = true;

    // VPN Button Appearance
    int          vpnButtonPadX    = 8;
    bool         showIcon         = true;
    std::wstring iconConnected    = L"shield_solid";
    std::wstring iconDisconnected = L"shield";
    int          iconSize         = 14;
    bool         hasIconColorConnected     = true;
    bool         hasIconColorTransitioning = true;
    bool         hasIconColorDisconnected  = true;
    winrt::Windows::UI::Color iconColorConnected{0xFF, 0xFF, 0xFF, 0xFF};
    winrt::Windows::UI::Color iconColorTransitioning{0xFF, 0xF3, 0x9C, 0x12};
    winrt::Windows::UI::Color iconColorDisconnected{0xFF, 0xFF, 0xFF, 0xFF};
    bool         showText         = true;
    std::wstring textConnected    = L"VPN ON";
    std::wstring textDisconnected = L"VPN OFF";
    int          fontSize         = 12;
    std::wstring fontWeight       = L"regular";
    std::wstring textColorMode    = L"auto";
    winrt::Windows::UI::Color textColorConnected{0xFF, 0xFF, 0xFF, 0xFF};
    winrt::Windows::UI::Color textColorDisconnected{0xFF, 0xD0, 0xD0, 0xD0};
    bool         showStatusDot    = true;
    std::wstring dotPosition      = L"right";
    int          dotSize          = 7;
    int          dotSpacing       = 6;
    int          dotOffsetY       = 0;
    winrt::Windows::UI::Color colorConnected{0xFF, 0x2E, 0xCC, 0x71};
    winrt::Windows::UI::Color colorTransitioning{0xFF, 0xF3, 0x9C, 0x12};
    winrt::Windows::UI::Color colorDisconnected{0x90, 0x95, 0xA5, 0xA6};

    // Split Tunneling
    bool splitEnabled = false;
    bool blockIpv6    = true;
    std::vector<ExcludedAppRule> excludedApps;
};

struct VpnModuleUI {
    Border    capsule{nullptr};
    Grid      slot{nullptr};
    Border    buttonPill{nullptr};
    Grid      iconHost{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path iconPathConnected{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path iconPathDisconnected{nullptr};
    TextBlock textBlock{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Ellipse statusDot{nullptr};
};

struct MarqueeLine {
    bool               enabled = false;
    double             viewWidth = 130.0;
    double             offset = 0.0;
    bool               forward = true;
    int                pauseTicks = 30;
    TextBlock          textBlock{nullptr};
    TranslateTransform transform{nullptr};
};

struct MediaModuleUI {
    Border     capsule{nullptr};
    Border     hoverPlate{nullptr};
    Grid       slot{nullptr};
    bool       isHovered = false;
    bool       isPressed = false;
    Grid       clickArea{nullptr};
    Grid       artBox{nullptr};
    Border     artBackdrop{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Rectangle artRect{nullptr};
    ImageBrush artBrush{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path fallbackNote{nullptr};
    MarqueeLine titleLine;
    MarqueeLine artistLine;
    DispatcherTimer marqueeTimer{nullptr};
    winrt::event_token marqueeTickToken{};
    Border     shuffleBtn{nullptr};
    Border     prevBtn{nullptr};
    Border     playPauseBtn{nullptr};
    Border     nextBtn{nullptr};
    Border     repeatBtn{nullptr};
    Border     likeBtn{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path shufflePath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path prevPath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path playPath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path pausePath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path nextPath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path repeatAllPath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path repeatOnePath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path addPath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path checkPath{nullptr};
    uint64_t   lastThumbHash = 0;
};

struct MiniPlayerUI {
    Controls::Flyout flyout{nullptr};
    Border     card{nullptr};
    StackPanel sessionBar{nullptr};
    Grid       artBox{nullptr};
    Border     artBackdrop{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Rectangle artRect{nullptr};
    ImageBrush artBrush{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path fallbackNote{nullptr};
    TextBlock  titleBlock{nullptr};
    TextBlock  artistBlock{nullptr};
    Border     likeBtn{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path likeAddPath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path likeCheckPath{nullptr};
    Grid       seekHitArea{nullptr};
    Border     seekTrackBg{nullptr};
    Border     seekTrackFill{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Ellipse seekKnob{nullptr};
    TranslateTransform seekKnobTransform{nullptr};
    TextBlock  posText{nullptr};
    TextBlock  durText{nullptr};
    Border     shuffleBtn{nullptr};
    Border     prevBtn{nullptr};
    Border     playPauseBtn{nullptr};
    Border     nextBtn{nullptr};
    Border     repeatBtn{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path shufflePath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path prevPath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path playPath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path pausePath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path nextPath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path repeatAllPath{nullptr};
    winrt::Windows::UI::Xaml::Shapes::Path repeatOnePath{nullptr};
    uint64_t   lastThumbHash = 0;
    size_t     lastSessionSig = 0;
    bool       isSeeking = false;
    double     seekRatio = 0.0;

    void Reset() {
        repeatOnePath = nullptr;
        repeatAllPath = nullptr;
        nextPath = nullptr;
        pausePath = nullptr;
        playPath = nullptr;
        prevPath = nullptr;
        shufflePath = nullptr;
        repeatBtn = nullptr;
        nextBtn = nullptr;
        playPauseBtn = nullptr;
        prevBtn = nullptr;
        shuffleBtn = nullptr;
        durText = nullptr;
        posText = nullptr;
        seekKnobTransform = nullptr;
        seekKnob = nullptr;
        seekTrackFill = nullptr;
        seekTrackBg = nullptr;
        seekHitArea = nullptr;
        likeCheckPath = nullptr;
        likeAddPath = nullptr;
        likeBtn = nullptr;
        artistBlock = nullptr;
        titleBlock = nullptr;
        fallbackNote = nullptr;
        artBrush = nullptr;
        artRect = nullptr;
        artBackdrop = nullptr;
        artBox = nullptr;
        sessionBar = nullptr;
        card = nullptr;
        flyout = nullptr;
        lastThumbHash = 0;
        lastSessionSig = 0;
        isSeeking = false;
        seekRatio = 0.0;
    }
};

struct IslandHost {
    HWND               hWnd = nullptr;
    Grid               injectionParent{nullptr};
    Grid               islandRoot{nullptr};
    Border             islandPill{nullptr};
    StackPanel         modulesRow{nullptr};
    Border             moduleDivider{nullptr};
    int                injectedColumn = -1;
    bool               isTray = false;
    int                marginLeft = 4;
    int                marginRight = 4;
    std::wstring       position = L"taskbar_left_edge";
    MediaModuleUI      media;
    VpnModuleUI        vpn;
};

std::mutex     g_settingsMutex;
IslandSettings g_settings;

std::mutex    g_mediaMutex;
MediaSnapshot g_mediaSnapshot;
std::wstring  g_preferredSessionAumid;
std::wstring  g_pendingTargetSessionAumid;
std::unordered_map<std::wstring, std::wstring> g_appNameCache;

bool              g_isExplorer = false;
std::wstring      g_procFullPathLower;
std::wstring      g_procFileNameLower;
std::atomic<bool> g_splitBypassActive{false};
std::atomic<bool> g_blockIpv6{true};

std::atomic<VpnState>  g_vpnState{VpnState::Disconnected};
std::atomic<VpnState>  g_lastStableState{VpnState::Disconnected};
std::atomic<bool>      g_unloading{false};
std::atomic<bool>      g_toggleRequested{false};
std::atomic<bool>      g_rebuildRequested{false};
std::atomic<bool>      g_needsUiUpdate{false};
std::atomic<int>       g_pendingMediaCmd{0};
std::atomic<int64_t>   g_pendingSeekTicks{0};
std::atomic<bool>      g_miniPlayerOpen{false};
HWND                   g_miniPlayerAnchorHwnd = nullptr;

// VK Music HTTP Bridge shared state
std::atomic<int>       g_vkPendingCmd{0};  // 1=shuffle, 2=repeat, 3=add, 4/5/6=repeat:0/1/2
std::atomic<bool>      g_vkShuffle{false};
std::atomic<int>       g_vkRepeat{0};      // 0=off, 1=all, 2=one
std::atomic<bool>      g_vkLiked{false};
std::atomic<ULONGLONG> g_vkLastSeenTick{0};
std::atomic<ULONGLONG> g_vkV2SeenTick{0};
std::atomic<ULONGLONG> g_vkLastCmdTick{0};
std::atomic<bool>      g_vkBridgeExtended{false};

HANDLE g_workerThread   = nullptr;
HANDLE g_mediaThread    = nullptr;
HANDLE g_vkBridgeThread = nullptr;
HANDLE g_stopEvent      = nullptr;
HANDLE g_wakeEvent      = nullptr;
HANDLE g_mediaWakeEvent = nullptr;

std::mutex        g_mountedMutex;
std::vector<HWND> g_mountedHwnds;

// Accessed strictly on taskbar UI threads.
[[clang::no_destroy]] std::vector<IslandHost> g_hosts;
[[clang::no_destroy]] MiniPlayerUI g_miniPlayer;

using CTaskBand_GetTaskbarHost_t          = void*(WINAPI*)(void*, void*);
using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void*);
using TaskbarHost_FrameHeight_t           = int(WINAPI*)(void*);
using Std_Ref_Decref_t                    = void(WINAPI*)(void*);
using TrayUI_StartTaskbar_t               = void(WINAPI*)(void*);

CTaskBand_GetTaskbarHost_t          CTaskBand_GetTaskbarHost_Original          = nullptr;
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;
TaskbarHost_FrameHeight_t           TaskbarHost_FrameHeight_Original           = nullptr;
Std_Ref_Decref_t                    Std_Ref_Decref_Original                    = nullptr;
TrayUI_StartTaskbar_t               TrayUI_StartTaskbar_Original               = nullptr;
void* CTaskBand_ITaskListWndSite_vftable          = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

bool IsRussianUi() {
    static const bool kIsRu = (PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_RUSSIAN);
    return kIsRu;
}

std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return s;
}

std::wstring Trim(std::wstring_view sv) {
    size_t first = sv.find_first_not_of(L" \t\r\n");
    if (first == std::wstring_view::npos) return {};
    size_t last = sv.find_last_not_of(L" \t\r\n");
    return std::wstring(sv.substr(first, last - first + 1));
}

std::wstring StripQuotes(std::wstring s) {
    s = Trim(s);
    if (s.size() >= 2 && s.front() == L'"' && s.back() == L'"') {
        s = s.substr(1, s.size() - 2);
    }
    return Trim(s);
}

std::wstring ExtractTunnelNameFromPath(std::wstring path) {
    path = StripQuotes(std::move(path));
    size_t slash = path.find_last_of(L"\\/");
    std::wstring file = (slash != std::wstring::npos) ? path.substr(slash + 1) : path;
    if (file.size() > 6 && _wcsicmp(file.c_str() + file.size() - 6, L".dpapi") == 0) {
        file.resize(file.size() - 6);
    }
    if (file.size() > 5 && _wcsicmp(file.c_str() + file.size() - 5, L".conf") == 0) {
        file.resize(file.size() - 5);
    }
    return file;
}

bool HexDigitVal(wchar_t c, BYTE& out) {
    if (c >= L'0' && c <= L'9') { out = static_cast<BYTE>(c - L'0'); return true; }
    if (c >= L'a' && c <= L'f') { out = static_cast<BYTE>(10 + c - L'a'); return true; }
    if (c >= L'A' && c <= L'F') { out = static_cast<BYTE>(10 + c - L'A'); return true; }
    return false;
}

bool ParseHexByte(std::wstring_view sv, size_t offset, BYTE& out) {
    BYTE hi = 0, lo = 0;
    if (!HexDigitVal(sv[offset], hi) || !HexDigitVal(sv[offset + 1], lo)) return false;
    out = static_cast<BYTE>((hi << 4) | lo);
    return true;
}

bool TryParseColor(std::wstring_view raw, winrt::Windows::UI::Color& out) {
    std::wstring s = Trim(raw);
    if (s.empty()) return false;

    auto asciiLower = [](std::wstring_view v) {
        std::wstring res(v);
        for (wchar_t& c : res) {
            if (c >= L'A' && c <= L'Z') c = static_cast<wchar_t>(c - L'A' + L'a');
        }
        return res;
    };

    bool hadRgba = false;
    bool hadCssFunc = false;
    std::wstring low = asciiLower(s);
    if (low.rfind(L"rgba", 0) == 0 && s.size() > 4 && (s[4] == L'(' || s[4] == L' ')) {
        hadRgba = true;
        hadCssFunc = true;
        s.erase(0, 4);
    } else if (low.rfind(L"rgb", 0) == 0 && s.size() > 3 && (s[3] == L'(' || s[3] == L' ')) {
        hadCssFunc = true;
        s.erase(0, 3);
    }
    s = Trim(s);
    if (s.empty()) return false;

    if (!hadCssFunc && s.front() == L'#') {
        s.erase(s.begin());
    }

    auto parseHexNibble = [](wchar_t ch, BYTE& nibble) -> bool {
        if (ch >= L'0' && ch <= L'9') { nibble = static_cast<BYTE>(ch - L'0'); return true; }
        if (ch >= L'a' && ch <= L'f') { nibble = static_cast<BYTE>(10 + ch - L'a'); return true; }
        if (ch >= L'A' && ch <= L'F') { nibble = static_cast<BYTE>(10 + ch - L'A'); return true; }
        return false;
    };

    if (!hadCssFunc && s.find_first_of(L" ,;\t()") == std::wstring::npos) {
        if (s.size() == 3) {
            BYTE r = 0, g = 0, b = 0;
            if (parseHexNibble(s[0], r) && parseHexNibble(s[1], g) && parseHexNibble(s[2], b)) {
                out = {0xFF, static_cast<BYTE>(r * 17), static_cast<BYTE>(g * 17), static_cast<BYTE>(b * 17)};
                return true;
            }
            return false;
        }
        if (s.size() == 4) {
            BYTE a = 0, r = 0, g = 0, b = 0;
            if (parseHexNibble(s[0], a) && parseHexNibble(s[1], r) &&
                parseHexNibble(s[2], g) && parseHexNibble(s[3], b)) {
                out = {static_cast<BYTE>(a * 17), static_cast<BYTE>(r * 17),
                       static_cast<BYTE>(g * 17), static_cast<BYTE>(b * 17)};
                return true;
            }
            return false;
        }
        if (s.size() == 6) {
            BYTE r = 0, g = 0, b = 0;
            if (ParseHexByte(s, 0, r) && ParseHexByte(s, 2, g) && ParseHexByte(s, 4, b)) {
                out = {0xFF, r, g, b};
                return true;
            }
            return false;
        }
        if (s.size() == 8) {
            BYTE a = 0, r = 0, g = 0, b = 0;
            if (ParseHexByte(s, 0, a) && ParseHexByte(s, 2, r) &&
                ParseHexByte(s, 4, g) && ParseHexByte(s, 6, b)) {
                out = {a, r, g, b};
                return true;
            }
            return false;
        }
    }

    std::vector<double> parts;
    size_t pos = 0;
    while (pos < s.size()) {
        size_t start = s.find_first_not_of(L" ,;\t()", pos);
        if (start == std::wstring::npos) break;
        size_t end = s.find_first_of(L" ,;\t()", start);
        std::wstring tok = s.substr(start, end == std::wstring::npos ? std::wstring::npos : end - start);
        try {
            parts.push_back(std::stod(tok));
        } catch (...) {
            return false;
        }
        if (end == std::wstring::npos) break;
        pos = end + 1;
    }

    if (parts.size() == 3) {
        out = {
            0xFF,
            static_cast<BYTE>(std::clamp(static_cast<int>(parts[0] + 0.5), 0, 255)),
            static_cast<BYTE>(std::clamp(static_cast<int>(parts[1] + 0.5), 0, 255)),
            static_cast<BYTE>(std::clamp(static_cast<int>(parts[2] + 0.5), 0, 255))
        };
        return true;
    }
    if (parts.size() == 4) {
        if (hadRgba) {
            const double alphaVal = (parts[3] >= 0.0 && parts[3] <= 1.0 && (parts[3] != 0.0 && parts[3] != 1.0 ? true : (s.find(L'.') != std::wstring::npos || parts[3] <= 1.0)))
                ? (parts[3] * 255.0)
                : parts[3];
            out = {
                static_cast<BYTE>(std::clamp(static_cast<int>(alphaVal + 0.5), 0, 255)),
                static_cast<BYTE>(std::clamp(static_cast<int>(parts[0] + 0.5), 0, 255)),
                static_cast<BYTE>(std::clamp(static_cast<int>(parts[1] + 0.5), 0, 255)),
                static_cast<BYTE>(std::clamp(static_cast<int>(parts[2] + 0.5), 0, 255))
            };
        } else {
            out = {
                static_cast<BYTE>(std::clamp(static_cast<int>(parts[0] + 0.5), 0, 255)),
                static_cast<BYTE>(std::clamp(static_cast<int>(parts[1] + 0.5), 0, 255)),
                static_cast<BYTE>(std::clamp(static_cast<int>(parts[2] + 0.5), 0, 255)),
                static_cast<BYTE>(std::clamp(static_cast<int>(parts[3] + 0.5), 0, 255))
            };
        }
        return true;
    }
    return false;
}

winrt::Windows::UI::Color ParseColor(std::wstring_view raw, winrt::Windows::UI::Color fallback) {
    winrt::Windows::UI::Color parsed{};
    return TryParseColor(raw, parsed) ? parsed : fallback;
}

bool TryParseCustomColor(std::wstring_view raw, winrt::Windows::UI::Color& out) {
    std::wstring s = Trim(raw);
    if (s.empty()) return false;
    std::wstring low = s;
    for (wchar_t& c : low) {
        if (c >= L'A' && c <= L'Z') c = static_cast<wchar_t>(c - L'A' + L'a');
    }
    if (low == L"auto" || low == L"default" || low == L"none") {
        return false;
    }
    return TryParseColor(s, out);
}

void InitCurrentProcessInfo() {
    wchar_t path[MAX_PATH]{};
    DWORD len = GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (len > 0 && len < MAX_PATH) {
        g_procFullPathLower = ToLower(path);
        const wchar_t* name = PathFindFileNameW(path);
        g_procFileNameLower = name ? ToLower(name) : L"";
        g_isExplorer = (_wcsicmp(name ? name : L"", L"explorer.exe") == 0);
    }
}

bool MatchesProcessRule(std::wstring_view rawRule,
                        std::wstring_view fullPathLower,
                        std::wstring_view fileNameLower) {
    std::wstring rule = ToLower(StripQuotes(std::wstring(rawRule)));
    if (rule.empty()) return false;

    for (auto& ch : rule) {
        if (ch == L'/') ch = L'\\';
    }

    if (rule.find(L'\\') != std::wstring::npos) {
        if (fullPathLower == rule) return true;
        if (fullPathLower.size() > rule.size() &&
            fullPathLower.compare(0, rule.size(), rule) == 0) {
            return rule.back() == L'\\' || fullPathLower[rule.size()] == L'\\';
        }
        return false;
    }

    if (fileNameLower == rule) return true;

    std::wstring_view procStem = fileNameLower;
    if (procStem.size() > 4 && procStem.substr(procStem.size() - 4) == L".exe") {
        procStem.remove_suffix(4);
    }
    std::wstring_view ruleStem = rule;
    if (ruleStem.size() > 4 && ruleStem.substr(ruleStem.size() - 4) == L".exe") {
        ruleStem.remove_suffix(4);
    }
    return !procStem.empty() && procStem == ruleStem;
}

bool IsCurrentProcessExcluded(const IslandSettings& s) {
    if (!s.splitEnabled) return false;
    for (const auto& rule : s.excludedApps) {
        if (!rule.enabled) continue;
        if (MatchesProcessRule(rule.process, g_procFullPathLower, g_procFileNameLower)) {
            return true;
        }
    }
    return false;
}

IslandSettings ReadSettingsFromWindhawk() {
    auto ReadRawStr = [](const wchar_t* key) -> std::wstring {
        PCWSTR raw = Wh_GetStringSetting(key);
        std::wstring result = raw ? raw : L"";
        Wh_FreeStringSetting(raw);
        return result;
    };
    auto ReadStr = [&ReadRawStr](const wchar_t* key, const wchar_t* defVal) -> std::wstring {
        std::wstring result = ReadRawStr(key);
        return result.empty() ? defVal : result;
    };

    IslandSettings s;

    // Split Tunneling (read first so non-explorer processes can exit early if unneeded)
    s.splitEnabled = Wh_GetIntSetting(L"SplitTunneling.enabled") != 0;
    s.blockIpv6    = Wh_GetIntSetting(L"SplitTunneling.blockIpv6") != 0;

    int emptyStreak = 0;
    for (int i = 0; i < 256; ++i) {
        PCWSTR procRaw = Wh_GetStringSetting(L"SplitTunneling.excludedApps[%d].process", i);
        std::wstring proc = procRaw ? Trim(procRaw) : L"";
        Wh_FreeStringSetting(procRaw);

        int enabledVal = Wh_GetIntSetting(L"SplitTunneling.excludedApps[%d].enabled", i);
        if (proc.empty() && enabledVal == 0) {
            if (++emptyStreak >= 3) break;
            continue;
        }
        emptyStreak = 0;
        if (!proc.empty()) {
            s.excludedApps.push_back({std::move(proc), enabledVal != 0});
        }
    }

    // Connection paths
    s.wgExePath  = StripQuotes(ReadStr(L"VpnConnection.wireguardExePath", L"C:\\Program Files\\WireGuard\\wireguard.exe"));
    s.configPath = StripQuotes(ReadRawStr(L"VpnConnection.configPath"));
    s.tunnelName = ExtractTunnelNameFromPath(s.configPath);

    if (!g_isExplorer) {
        return s;
    }

    // Island Container
    s.enableMediaPlayer = Wh_GetIntSetting(L"Island.enableMediaPlayer") != 0;
    s.enableVpnButton   = Wh_GetIntSetting(L"Island.enableVpnButton") != 0;
    s.monitorMode       = ReadStr(L"Island.monitorMode", L"single");
    s.monitor           = std::clamp(Wh_GetIntSetting(L"Island.monitor"), 1, 16);
    s.position          = ReadStr(L"Island.position", L"taskbar_left_edge");
    s.capsuleMode       = ReadStr(L"Island.capsuleMode", L"unified");
    s.moduleSpacing     = std::clamp(Wh_GetIntSetting(L"Island.moduleSpacing"), 0, 32);
    s.showModuleDivider = Wh_GetIntSetting(L"Island.showModuleDivider") != 0;
    s.marginLeft        = std::clamp(Wh_GetIntSetting(L"Island.marginLeft"), 0, 500);
    s.marginRight       = std::clamp(Wh_GetIntSetting(L"Island.marginRight"), 0, 500);
    s.height            = std::clamp(Wh_GetIntSetting(L"Island.height"), 24, 64);
    s.paddingX          = std::clamp(Wh_GetIntSetting(L"Island.paddingX"), 0, 32);
    s.paddingY          = std::clamp(Wh_GetIntSetting(L"Island.paddingY"), 0, 12);
    s.cornerRadius      = std::clamp(Wh_GetIntSetting(L"Island.cornerRadius"), 0, 24);
    s.backgroundStyle   = ReadStr(L"Island.backgroundStyle", L"transparent");
    s.backgroundColor   = ParseColor(ReadStr(L"Island.backgroundColor", L"#232323"), {0xFF, 0x23, 0x23, 0x23});
    s.backgroundOpacity = std::clamp(Wh_GetIntSetting(L"Island.backgroundOpacity"), 0, 100);
    s.borderColorMode   = ReadStr(L"Island.borderColorMode", L"auto");
    s.borderColor       = ParseColor(ReadStr(L"Island.borderColor", L"#FFFFFF"), {0xFF, 0xFF, 0xFF, 0xFF});
    s.borderOpacity     = std::clamp(Wh_GetIntSetting(L"Island.borderOpacity"), 0, 100);
    s.borderThickness   = std::clamp(Wh_GetIntSetting(L"Island.borderThickness"), 0, 3);
    s.enableShadow      = Wh_GetIntSetting(L"Island.enableShadow") != 0;

    // Media Bar (Taskbar)
    s.mediaShowAlbumArt     = Wh_GetIntSetting(L"MediaBar.Elements.showAlbumArt") != 0;
    s.mediaShowTitle        = Wh_GetIntSetting(L"MediaBar.Elements.showTitle") != 0;
    s.mediaShowArtist       = Wh_GetIntSetting(L"MediaBar.Elements.showArtist") != 0;
    s.mediaShowPlaybackBtns = Wh_GetIntSetting(L"MediaBar.Elements.showPlaybackButtons") != 0;
    s.mediaShowShuffleBtn   = Wh_GetIntSetting(L"MediaBar.Elements.showShuffleButton") != 0;
    s.mediaShowRepeatBtn    = Wh_GetIntSetting(L"MediaBar.Elements.showRepeatButton") != 0;
    s.mediaShowLikeBtn      = Wh_GetIntSetting(L"MediaBar.Elements.showLikeButton") != 0;

    s.mediaPadLeft           = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.paddingLeft"), 0, 32);
    s.mediaPadRight          = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.paddingRight"), 0, 32);
    s.mediaArtToTextGap      = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.artToTextGap"), 0, 32);
    s.mediaTextToControlsGap = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.textToControlsGap"), 0, 32);
    s.mediaButtonSpacing     = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.buttonSpacing"), 0, 16);
    s.mediaArtSize           = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.artSize"), 20, 44);
    s.mediaArtCornerRadius   = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.artCornerRadius"), 0, 20);
    s.mediaAdaptiveArtWidth  = Wh_GetIntSetting(L"MediaBar.Layout.adaptiveArtWidth") != 0;
    s.mediaArtMaxWidth       = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.artMaxWidth"), s.mediaArtSize, 96);
    s.mediaTextWidth         = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.textWidth"), 60, 320);
    s.mediaTitleFontSize     = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.titleFontSize"), 8, 20);
    s.mediaArtistFontSize    = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.artistFontSize"), 8, 18);
    s.mediaTextSpacing       = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.textSpacing"), -2, 8);
    s.mediaTitleMarquee      = Wh_GetIntSetting(L"MediaBar.Layout.enableTitleMarquee") != 0;
    s.mediaArtistMarquee     = Wh_GetIntSetting(L"MediaBar.Layout.enableArtistMarquee") != 0;
    s.mediaButtonSize        = std::clamp(Wh_GetIntSetting(L"MediaBar.Layout.buttonSize"), 20, 36);

    s.mediaHoverEffect    = ReadStr(L"MediaBar.Style.hoverEffect", L"auto");
    s.mediaHoverAnimation = Wh_GetIntSetting(L"MediaBar.Style.hoverAnimation") != 0;
    s.mediaColorMode      = ReadStr(L"MediaBar.Style.colorMode", L"auto");
    s.mediaTextColor      = ParseColor(ReadStr(L"MediaBar.Style.textColor", L"#FFFFFF"), {0xFF, 0xFF, 0xFF, 0xFF});
    s.mediaArtistColor    = ParseColor(ReadStr(L"MediaBar.Style.artistColor", L"#B8B8B8"), {0xFF, 0xB8, 0xB8, 0xB8});
    s.mediaButtonColor    = ParseColor(ReadStr(L"MediaBar.Style.buttonColor", L"#FFFFFF"), {0xFF, 0xFF, 0xFF, 0xFF});
    s.mediaAccentColor    = ParseColor(ReadStr(L"MediaBar.Style.accentColor", L"#4CC2FF"), {0xFF, 0x4C, 0xC2, 0xFF});
    s.mediaEnableCtxMenu  = Wh_GetIntSetting(L"MediaBar.Style.enableContextMenu") != 0;
    s.mediaWheelAction    = ReadStr(L"MediaBar.Style.mouseWheelAction", L"none");
    s.enableVkBridge      = Wh_GetIntSetting(L"MediaBar.Style.enableVkBridge") != 0;
    s.vkBridgePort        = std::clamp(Wh_GetIntSetting(L"MediaBar.Style.vkBridgePort"), 1024, 65535);

    // Mini-Player Popup
    s.mediaEnableMiniPlayer    = Wh_GetIntSetting(L"MiniPlayer.enabled") != 0;
    s.miniPlayerWindowStyle    = ReadStr(L"MiniPlayer.windowStyle", L"translucent");
    s.miniPlayerShowBorder     = Wh_GetIntSetting(L"MiniPlayer.showBorder") != 0;
    s.miniPlayerTintOpacity    = std::clamp(Wh_GetIntSetting(L"MiniPlayer.tintOpacity"), 5, 95);
    s.miniPlayerCornerRadius   = std::clamp(Wh_GetIntSetting(L"MiniPlayer.cornerRadius"), 0, 24);
    s.miniPlayerWidth          = std::clamp(Wh_GetIntSetting(L"MiniPlayer.width"), 240, 460);
    s.miniPlayerPadX           = std::clamp(Wh_GetIntSetting(L"MiniPlayer.paddingX"), 8, 32);
    s.miniPlayerPadY           = std::clamp(Wh_GetIntSetting(L"MiniPlayer.paddingY"), 8, 32);
    s.miniPlayerElemSpacing    = std::clamp(Wh_GetIntSetting(L"MiniPlayer.elementSpacing"), 4, 24);
    s.miniPlayerShowSessionBar = Wh_GetIntSetting(L"MiniPlayer.showSessionBar") != 0;
    s.miniPlayerShowArt        = Wh_GetIntSetting(L"MiniPlayer.showAlbumArt") != 0;
    s.miniPlayerAdaptiveArt    = Wh_GetIntSetting(L"MiniPlayer.adaptiveArtSize") != 0;
    s.miniPlayerArtMaxHeight   = std::clamp(Wh_GetIntSetting(L"MiniPlayer.artMaxHeight"), 100, 320);
    s.miniPlayerArtRadius      = std::clamp(Wh_GetIntSetting(L"MiniPlayer.artCornerRadius"), 0, 24);
    s.miniPlayerShowArtist     = Wh_GetIntSetting(L"MiniPlayer.showArtist") != 0;
    s.miniPlayerShowLike       = Wh_GetIntSetting(L"MiniPlayer.showLikeButton") != 0;
    s.miniPlayerShowSeekBar    = Wh_GetIntSetting(L"MiniPlayer.showSeekBar") != 0;
    s.miniPlayerShowShuffle    = Wh_GetIntSetting(L"MiniPlayer.showShuffleButton") != 0;
    s.miniPlayerShowRepeat     = Wh_GetIntSetting(L"MiniPlayer.showRepeatButton") != 0;
    s.miniPlayerPlacement      = ReadStr(L"MiniPlayer.placementMode", L"near");
    s.miniPlayerHAlign         = ReadStr(L"MiniPlayer.horizontalAlign", L"left");
    s.miniPlayerVAlign         = ReadStr(L"MiniPlayer.verticalAlign", L"bottom");
    s.miniPlayerEdgeOffsetX    = std::clamp(Wh_GetIntSetting(L"MiniPlayer.edgeOffsetX"), 0, 200);
    s.miniPlayerEdgeOffsetY    = std::clamp(Wh_GetIntSetting(L"MiniPlayer.edgeOffsetY"), 0, 200);
    s.miniPlayerAnimation      = Wh_GetIntSetting(L"MiniPlayer.openAnimation") != 0;

    // Notifications & Interaction
    s.showNotifications   = Wh_GetIntSetting(L"VpnConnection.showNotifications") != 0;
    s.silentNotifications = Wh_GetIntSetting(L"VpnConnection.silentNotifications") != 0;
    s.showTooltip         = Wh_GetIntSetting(L"VpnConnection.showTooltip") != 0;
    s.enableContextMenu   = Wh_GetIntSetting(L"VpnConnection.enableContextMenu") != 0;

    // VPN Appearance
    s.vpnButtonPadX          = std::clamp(Wh_GetIntSetting(L"VpnAppearance.buttonPaddingX"), 0, 32);
    s.showIcon               = Wh_GetIntSetting(L"VpnAppearance.showIcon") != 0;
    s.iconConnected          = ReadStr(L"VpnAppearance.iconConnected", L"shield_solid");
    s.iconDisconnected       = ReadStr(L"VpnAppearance.iconDisconnected", L"shield");
    s.iconSize                  = std::clamp(Wh_GetIntSetting(L"VpnAppearance.iconSize"), 8, 32);
    s.hasIconColorConnected     = TryParseCustomColor(ReadRawStr(L"VpnAppearance.iconColorConnected"), s.iconColorConnected);
    s.hasIconColorTransitioning = TryParseCustomColor(ReadRawStr(L"VpnAppearance.iconColorTransitioning"), s.iconColorTransitioning);
    s.hasIconColorDisconnected  = TryParseCustomColor(ReadRawStr(L"VpnAppearance.iconColorDisconnected"), s.iconColorDisconnected);
    s.showText               = Wh_GetIntSetting(L"VpnAppearance.showText") != 0;
    s.textConnected         = ReadStr(L"VpnAppearance.textConnected", L"VPN ON");
    s.textDisconnected      = ReadStr(L"VpnAppearance.textDisconnected", L"VPN OFF");
    s.fontSize              = std::clamp(Wh_GetIntSetting(L"VpnAppearance.fontSize"), 8, 24);
    s.fontWeight            = ReadStr(L"VpnAppearance.fontWeight", L"regular");
    s.textColorMode         = ReadStr(L"VpnAppearance.textColorMode", L"auto");
    s.textColorConnected    = ParseColor(ReadStr(L"VpnAppearance.textColorConnected", L"#FFFFFF"), {0xFF, 0xFF, 0xFF, 0xFF});
    s.textColorDisconnected = ParseColor(ReadStr(L"VpnAppearance.textColorDisconnected", L"#D0D0D0"), {0xFF, 0xD0, 0xD0, 0xD0});
    s.showStatusDot         = Wh_GetIntSetting(L"VpnAppearance.showStatusDot") != 0;
    s.dotPosition           = ReadStr(L"VpnAppearance.dotPosition", L"right");
    s.dotSize               = std::clamp(Wh_GetIntSetting(L"VpnAppearance.dotSize"), 4, 16);
    s.dotSpacing            = std::clamp(Wh_GetIntSetting(L"VpnAppearance.dotSpacing"), 0, 32);
    s.dotOffsetY            = std::clamp(Wh_GetIntSetting(L"VpnAppearance.dotOffsetY"), -8, 8);
    s.colorConnected        = ParseColor(ReadStr(L"VpnAppearance.colorConnected", L"#2ECC71"), {0xFF, 0x2E, 0xCC, 0x71});
    s.colorTransitioning    = ParseColor(ReadStr(L"VpnAppearance.colorTransitioning", L"#F39C12"), {0xFF, 0xF3, 0x9C, 0x12});
    s.colorDisconnected     = ParseColor(ReadStr(L"VpnAppearance.colorDisconnected", L"#9095A5A6"), {0x90, 0x95, 0xA5, 0xA6});

    return s;
}

void ReloadSettings() {
    IslandSettings next = ReadSettingsFromWindhawk();
    const bool excluded = IsCurrentProcessExcluded(next);
    const bool blockV6  = next.blockIpv6;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = std::move(next);
    }
    g_splitBypassActive.store(excluded);
    g_blockIpv6.store(blockV6);
}

IslandSettings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// ===========================================================================
// SPLIT TUNNELING ENGINE (non-explorer processes)
// ===========================================================================
struct PhysicalAdapterInfo {
    DWORD ifIndex = 0;
    DWORD ipv4Addr = 0;
    bool  vpnActive = false;
};

PhysicalAdapterInfo GetPhysicalAdapterInfo() {
    static std::atomic<DWORD>     cachedIfIndex{0};
    static std::atomic<DWORD>     cachedIp{0};
    static std::atomic<bool>      cachedVpnActive{false};
    static std::atomic<ULONGLONG> lastCheck{0};

    const ULONGLONG now = GetTickCount64();
    if (now - lastCheck.load() < 3000) {
        return {cachedIfIndex.load(), cachedIp.load(), cachedVpnActive.load()};
    }

    ULONG bufLen = 32768;
    std::vector<BYTE> buffer(bufLen);
    auto* pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
    const ULONG flags = GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_SKIP_ANYCAST |
                        GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

    DWORD ret = GetAdaptersAddresses(AF_INET, flags, nullptr, pAddresses, &bufLen);
    if (ret == ERROR_BUFFER_OVERFLOW) {
        buffer.resize(bufLen);
        pAddresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
        ret = GetAdaptersAddresses(AF_INET, flags, nullptr, pAddresses, &bufLen);
    }
    if (ret != NO_ERROR) {
        return {cachedIfIndex.load(), cachedIp.load(), cachedVpnActive.load()};
    }

    std::wstring tunnelLower;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        tunnelLower = ToLower(g_settings.tunnelName);
    }

    bool  foundVpn = false;
    DWORD physIfIndex = 0;
    DWORD physIp = 0;

    for (auto* curr = pAddresses; curr != nullptr; curr = curr->Next) {
        if (curr->OperStatus != IfOperStatusUp) continue;

        std::wstring desc = curr->Description ? ToLower(curr->Description) : L"";
        std::wstring friendly = curr->FriendlyName ? ToLower(curr->FriendlyName) : L"";
        const bool isVpnAdapter =
            desc.find(L"wireguard") != std::wstring::npos ||
            desc.find(L"wintun") != std::wstring::npos ||
            desc.find(L"amnezia") != std::wstring::npos ||
            desc.find(L"tap-") != std::wstring::npos ||
            (!tunnelLower.empty() && friendly == tunnelLower);

        if (isVpnAdapter) {
            foundVpn = true;
            continue;
        }

        if (physIfIndex != 0) continue;
        if (curr->IfType == IF_TYPE_SOFTWARE_LOOPBACK || curr->IfType == IF_TYPE_TUNNEL) continue;
        if (!curr->FirstGatewayAddress) continue;

        for (auto* ua = curr->FirstUnicastAddress; ua != nullptr; ua = ua->Next) {
            if (ua->Address.lpSockaddr && ua->Address.lpSockaddr->sa_family == AF_INET) {
                const auto* sin = reinterpret_cast<const sockaddr_in*>(ua->Address.lpSockaddr);
                const DWORD ip = sin->sin_addr.s_addr;
                if (ip != 0 && ip != htonl(INADDR_LOOPBACK)) {
                    physIfIndex = curr->IfIndex;
                    physIp = ip;
                    break;
                }
            }
        }
    }

    cachedIfIndex.store(physIfIndex);
    cachedIp.store(physIp);
    cachedVpnActive.store(foundVpn);
    lastCheck.store(now);
    return {physIfIndex, physIp, foundVpn};
}

bool ApplySplitTunnelToSocket(SOCKET s, const sockaddr* name) {
    if (!g_splitBypassActive.load() || !name) return true;

    const auto info = GetPhysicalAdapterInfo();
    if (!info.vpnActive || info.ifIndex == 0) {
        return true;
    }

    if (name->sa_family == AF_INET) {
        const auto* dest = reinterpret_cast<const sockaddr_in*>(name);
        const DWORD dstHost = ntohl(dest->sin_addr.s_addr);
        if ((dstHost >> 24) == 127 || dstHost == 0) {
            return true;
        }

        const DWORD netIfIndex = htonl(info.ifIndex);
        setsockopt(s, IPPROTO_IP, IP_UNICAST_IF,
                   reinterpret_cast<const char*>(&netIfIndex), sizeof(netIfIndex));

        sockaddr_in currentBind{};
        int namelen = sizeof(currentBind);
        if (getsockname(s, reinterpret_cast<sockaddr*>(&currentBind), &namelen) != 0 ||
            currentBind.sin_addr.s_addr == 0) {
            sockaddr_in bindAddr{};
            bindAddr.sin_family = AF_INET;
            bindAddr.sin_addr.s_addr = info.ipv4Addr;
            bindAddr.sin_port = 0;
            bind(s, reinterpret_cast<const sockaddr*>(&bindAddr), sizeof(bindAddr));
        }
        return true;
    }

    if (name->sa_family == AF_INET6) {
        const auto* dest6 = reinterpret_cast<const sockaddr_in6*>(name);
        if (IN6_IS_ADDR_LOOPBACK(&dest6->sin6_addr) || IN6_IS_ADDR_LINKLOCAL(&dest6->sin6_addr)) {
            return true;
        }
        if (g_blockIpv6.load()) {
            WSASetLastError(WSAEHOSTUNREACH);
            return false;
        }
    }

    return true;
}

using connect_t    = int(WSAAPI*)(SOCKET, const sockaddr*, int);
using WSAConnect_t = int(WSAAPI*)(SOCKET, const sockaddr*, int, LPWSABUF, LPWSABUF, LPQOS, LPQOS);
using sendto_t     = int(WSAAPI*)(SOCKET, const char*, int, int, const sockaddr*, int);
using WSASendTo_t  = int(WSAAPI*)(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, const sockaddr*, int, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE);

connect_t    connect_Original    = nullptr;
WSAConnect_t WSAConnect_Original = nullptr;
sendto_t     sendto_Original     = nullptr;
WSASendTo_t  WSASendTo_Original  = nullptr;

int WSAAPI connect_Hook(SOCKET s, const sockaddr* name, int namelen) {
    if (!ApplySplitTunnelToSocket(s, name)) return SOCKET_ERROR;
    return connect_Original(s, name, namelen);
}

int WSAAPI WSAConnect_Hook(SOCKET s, const sockaddr* name, int namelen,
                           LPWSABUF lpCallerData, LPWSABUF lpCalleeData, LPQOS lpSQOS, LPQOS lpGQOS) {
    if (!ApplySplitTunnelToSocket(s, name)) return SOCKET_ERROR;
    return WSAConnect_Original(s, name, namelen, lpCallerData, lpCalleeData, lpSQOS, lpGQOS);
}

int WSAAPI sendto_Hook(SOCKET s, const char* buf, int len, int flags, const sockaddr* to, int tolen) {
    if (!ApplySplitTunnelToSocket(s, to)) return SOCKET_ERROR;
    return sendto_Original(s, buf, len, flags, to, tolen);
}

int WSAAPI WSASendTo_Hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent,
                          DWORD dwFlags, const sockaddr* lpTo, int iTolen,
                          LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
    if (!ApplySplitTunnelToSocket(s, lpTo)) return SOCKET_ERROR;
    return WSASendTo_Original(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent,
                              dwFlags, lpTo, iTolen, lpOverlapped, lpCompletionRoutine);
}

bool HookWinsockForSplitTunneling() {
    HMODULE hWs2 = LoadLibraryExW(L"ws2_32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hWs2) return false;
    Wh_SetFunctionHook(reinterpret_cast<void*>(GetProcAddress(hWs2, "connect")),
                       reinterpret_cast<void*>(connect_Hook), reinterpret_cast<void**>(&connect_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(GetProcAddress(hWs2, "WSAConnect")),
                       reinterpret_cast<void*>(WSAConnect_Hook), reinterpret_cast<void**>(&WSAConnect_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(GetProcAddress(hWs2, "sendto")),
                       reinterpret_cast<void*>(sendto_Hook), reinterpret_cast<void**>(&sendto_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(GetProcAddress(hWs2, "WSASendTo")),
                       reinterpret_cast<void*>(WSASendTo_Hook), reinterpret_cast<void**>(&WSASendTo_Original));
    return true;
}

// ===========================================================================
// EXPLORER UI DISPATCH & TASKBAR DISCOVERY
// ===========================================================================
UINT GetRunFromWindowThreadMsg() {
    static const UINT kMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return kMsg;
}

using WindowThreadProc = void(*)(void*);

bool RunFromWindowThread(HWND hWnd, WindowThreadProc proc, void* param) {
    struct Payload {
        WindowThreadProc proc;
        void* param;
    };

    if (!hWnd || !IsWindow(hWnd)) return false;

    const DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;

    if (tid == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    const UINT msgId = GetRunFromWindowThreadMsg();
    if (!msgId) return false;

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) CALLBACK -> LRESULT {
            if (code == HC_ACTION) {
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == GetRunFromWindowThreadMsg()) {
                    auto* p = reinterpret_cast<Payload*>(cwp->lParam);
                    if (p && p->proc) {
                        p->proc(p->param);
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, tid);
    if (!hook) return false;

    Payload payload{proc, param};
    SendMessageW(hWnd, msgId, 0, reinterpret_cast<LPARAM>(&payload));
    UnhookWindowsHookEx(hook);
    return true;
}

bool IsSystemLightTheme() {
    DWORD val = 0;
    DWORD size = sizeof(val);
    if (RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"SystemUsesLightTheme",
            RRF_RT_DWORD, nullptr, &val, &size) == ERROR_SUCCESS) {
        return val != 0;
    }
    return false;
}

SolidColorBrush MakeBrush(winrt::Windows::UI::Color color) {
    SolidColorBrush brush;
    brush.Color(color);
    return brush;
}

struct MonitorEntry {
    HMONITOR hMon = nullptr;
    bool isPrimary = false;
    RECT rect{};
};

std::vector<MonitorEntry> GetSortedMonitors() {
    std::vector<MonitorEntry> monitors;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMon, HDC, LPRECT lprcMonitor, LPARAM lParam) CALLBACK -> BOOL {
            auto* list = reinterpret_cast<std::vector<MonitorEntry>*>(lParam);
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            bool primary = false;
            RECT rc = lprcMonitor ? *lprcMonitor : RECT{};
            if (GetMonitorInfoW(hMon, &mi)) {
                primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
                rc = mi.rcMonitor;
            }
            list->push_back({hMon, primary, rc});
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&monitors));

    std::sort(monitors.begin(), monitors.end(), [](const MonitorEntry& a, const MonitorEntry& b) {
        if (a.isPrimary != b.isPrimary) return a.isPrimary;
        if (a.rect.left != b.rect.left) return a.rect.left < b.rect.left;
        return a.rect.top < b.rect.top;
    });

    return monitors;
}

HWND FindTaskbarWndForMonitor(HMONITOR targetMonitor) {
    if (!targetMonitor) return nullptr;

    struct Ctx {
        HMONITOR target;
        HWND result;
    } ctx{targetMonitor, nullptr};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) CALLBACK -> BOOL {
            auto* c = reinterpret_cast<Ctx*>(lParam);
            DWORD pid = 0;
            wchar_t cls[64] = {};
            if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
                GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
                (_wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
                 _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0)) {
                if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) == c->target) {
                    c->result = hWnd;
                    return FALSE;
                }
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));

    return ctx.result;
}

std::vector<HWND> FindTargetTaskbarWnds(const IslandSettings& s) {
    std::vector<HWND> result;
    const auto monitors = GetSortedMonitors();
    if (monitors.empty()) return result;

    if (s.monitorMode == L"all") {
        for (const auto& m : monitors) {
            if (HWND hWnd = FindTaskbarWndForMonitor(m.hMon)) {
                result.push_back(hWnd);
            }
        }
        return result;
    }

    const int idx = s.monitor - 1;
    if (idx >= 0 && static_cast<size_t>(idx) < monitors.size()) {
        if (HWND hWnd = FindTaskbarWndForMonitor(monitors[idx].hMon)) {
            result.push_back(hWnd);
        }
    }
    return result;
}

bool IsReadableMemoryRange(const void* address, size_t size) {
    if (!address || size == 0) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(address, &mbi, sizeof(mbi)) ||
        mbi.State != MEM_COMMIT ||
        (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }
    const auto start = reinterpret_cast<uintptr_t>(address);
    const auto regionStart = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
    const auto regionEnd = regionStart + mbi.RegionSize;
    return start >= regionStart && start <= regionEnd && size <= regionEnd - start;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    wchar_t clsBuf[64] = {};
    if (!GetClassNameW(hTaskbarWnd, clsBuf, ARRAYSIZE(clsBuf))) return nullptr;

    const bool isSecondary = _wcsicmp(clsBuf, L"Shell_SecondaryTrayWnd") == 0;
    HWND hTaskSwWnd = isSecondary
        ? FindWindowExW(hTaskbarWnd, nullptr, L"WorkerW", nullptr)
        : reinterpret_cast<HWND>(GetPropW(hTaskbarWnd, L"TaskbandHWND"));
    if (!hTaskSwWnd) return nullptr;

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(hTaskSwWnd, 0));
    if (!taskBand) return nullptr;

    void* expectedVftable = isSecondary
        ? CSecondaryTaskBand_ITaskListWndSite_vftable
        : CTaskBand_ITaskListWndSite_vftable;
    auto getTaskbarHost = isSecondary
        ? CSecondaryTaskBand_GetTaskbarHost_Original
        : CTaskBand_GetTaskbarHost_Original;
    if (!expectedVftable || !getTaskbarHost) return nullptr;

    void* site = taskBand;
    for (int i = 0;; ++i) {
        if (!IsReadableMemoryRange(site, sizeof(void*))) return nullptr;
        if (*reinterpret_cast<void**>(site) == expectedVftable) break;
        if (i == 20) return nullptr;
        site = reinterpret_cast<void**>(site) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(site, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0]) {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    auto releaseHost = [&]() {
        if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
            Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
        }
    };

    size_t offset = 0;
    bool offsetOk = false;
#if defined(_M_X64) || defined(__x86_64__)
    const BYTE* bytes = reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (IsReadableMemoryRange(bytes, 8) &&
        bytes[0] == 0x48 && bytes[1] == 0x83 && bytes[2] == 0xEC &&
        bytes[4] == 0x48 && bytes[5] == 0x83 && bytes[6] == 0xC1 && bytes[7] <= 0x7F) {
        offset = bytes[7];
        offsetOk = true;
    }
#endif

    const void* hostElemPtrAddr = reinterpret_cast<const BYTE*>(taskbarHostSharedPtr[0]) + offset;
    if (!offsetOk || !IsReadableMemoryRange(hostElemPtrAddr, sizeof(IUnknown*))) {
        releaseHost();
        return nullptr;
    }

    IUnknown* unk = *reinterpret_cast<IUnknown* const*>(hostElemPtrAddr);
    if (!unk) {
        releaseHost();
        return nullptr;
    }

    FrameworkElement elem{nullptr};
    const HRESULT hr = unk->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(elem));
    XamlRoot xamlRoot = elem ? elem.XamlRoot() : nullptr;
    releaseHost();
    return SUCCEEDED(hr) ? xamlRoot : nullptr;
}

FrameworkElement FindChildByName(FrameworkElement const& root, std::wstring_view name, int depth = 32) {
    if (!root || depth == 0) return nullptr;
    const int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) continue;
        if (child.Name() == name) return child;
        if (auto found = FindChildByName(child, name, depth - 1)) return found;
    }
    return nullptr;
}

Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    if (!root) return nullptr;
    const int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (child && winrt::get_class_name(child) == L"Taskbar.TaskbarFrame") {
            auto rootGrid = FindChildByName(child, L"RootGrid");
            return rootGrid ? rootGrid.try_as<Grid>() : nullptr;
        }
    }
    return nullptr;
}

// ===========================================================================
// WIREGUARD SERVICE, SCHTASKS SYNC & NOTIFICATIONS
// ===========================================================================
VpnState QueryVpnServiceState(const std::wstring& tunnelName) {
    if (tunnelName.empty()) return VpnState::Disconnected;

    SC_HANDLE hSCM = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (!hSCM) return VpnState::Disconnected;

    const std::wstring wgName = L"WireGuardTunnel$" + tunnelName;
    SC_HANDLE hSvc = OpenServiceW(hSCM, wgName.c_str(), SERVICE_QUERY_STATUS);
    if (!hSvc) {
        const std::wstring amneziaName = L"AmneziaWGTunnel$" + tunnelName;
        hSvc = OpenServiceW(hSCM, amneziaName.c_str(), SERVICE_QUERY_STATUS);
    }

    VpnState result = VpnState::Disconnected;
    if (hSvc) {
        SERVICE_STATUS_PROCESS ssp{};
        DWORD bytesNeeded = 0;
        if (QueryServiceStatusEx(hSvc, SC_STATUS_PROCESS_INFO,
                                 reinterpret_cast<LPBYTE>(&ssp), sizeof(ssp), &bytesNeeded)) {
            if (ssp.dwCurrentState == SERVICE_RUNNING) {
                result = VpnState::Connected;
            } else if (ssp.dwCurrentState == SERVICE_START_PENDING ||
                       ssp.dwCurrentState == SERVICE_STOP_PENDING) {
                result = VpnState::Transitioning;
            }
        }
        CloseServiceHandle(hSvc);
    }

    CloseServiceHandle(hSCM);
    return result;
}

bool RunProcessHidden(const wchar_t* exe, const std::wstring& args, DWORD& exitCode) {
    std::wstring cmdLine = std::wstring(L"\"") + exe + L"\" " + args;
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi{};

    if (!CreateProcessW(nullptr, cmdLine.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        return false;
    }

    bool completed = false;
    if (g_stopEvent) {
        HANDLE waitHandles[] = {g_stopEvent, pi.hProcess};
        const DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, 5000);
        if (waitRes == WAIT_OBJECT_0 + 1) {
            completed = GetExitCodeProcess(pi.hProcess, &exitCode) != FALSE;
        }
    } else {
        const DWORD waitRes = WaitForSingleObject(pi.hProcess, 5000);
        if (waitRes == WAIT_OBJECT_0) {
            completed = GetExitCodeProcess(pi.hProcess, &exitCode) != FALSE;
        }
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return completed;
}

bool RunProcessCaptureOutput(const wchar_t* exe, const std::wstring& args,
                             std::string& outBytes, DWORD& exitCode) {
    outBytes.clear();

    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE readPipe = nullptr;
    HANDLE writePipe = nullptr;
    if (!CreatePipe(&readPipe, &writePipe, &sa, 0)) {
        return false;
    }
    SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

    std::wstring cmdLine = std::wstring(L"\"") + exe + L"\" " + args;
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = writePipe;
    si.hStdError = writePipe;

    PROCESS_INFORMATION pi{};
    if (!CreateProcessW(nullptr, cmdLine.data(), nullptr, nullptr, TRUE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        CloseHandle(writePipe);
        CloseHandle(readPipe);
        return false;
    }

    // Close write end in parent so ReadFile sees EOF once child exits.
    CloseHandle(writePipe);

    char chunk[1024];
    DWORD bytesRead = 0;
    while (!g_unloading.load() && ReadFile(readPipe, chunk, sizeof(chunk), &bytesRead, nullptr) && bytesRead > 0) {
        outBytes.append(chunk, bytesRead);
    }

    bool completed = false;
    if (g_stopEvent) {
        HANDLE waitHandles[] = {g_stopEvent, pi.hProcess};
        const DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, 4000);
        if (waitRes == WAIT_OBJECT_0 + 1) {
            completed = GetExitCodeProcess(pi.hProcess, &exitCode) != FALSE;
        }
    } else {
        if (WaitForSingleObject(pi.hProcess, 4000) == WAIT_OBJECT_0) {
            completed = GetExitCodeProcess(pi.hProcess, &exitCode) != FALSE;
        }
    }

    CloseHandle(readPipe);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return completed;
}

std::wstring DecodeTaskXmlToLower(const std::string& bytes) {
    if (bytes.empty()) return {};

    std::wstring decoded;
    const bool hasUtf16Bom = bytes.size() >= 2 &&
                             static_cast<unsigned char>(bytes[0]) == 0xFF &&
                             static_cast<unsigned char>(bytes[1]) == 0xFE;
    const bool looksUtf16 = hasUtf16Bom || (bytes.size() >= 4 && bytes[1] == '\0' && bytes[3] == '\0');

    if (looksUtf16) {
        const size_t offset = hasUtf16Bom ? 2 : 0;
        const size_t wcharCount = (bytes.size() - offset) / sizeof(wchar_t);
        decoded.resize(wcharCount);
        memcpy(decoded.data(), bytes.data() + offset, wcharCount * sizeof(wchar_t));
    } else {
        int needed = MultiByteToWideChar(CP_UTF8, 0, bytes.data(), static_cast<int>(bytes.size()), nullptr, 0);
        if (needed > 0) {
            decoded.resize(static_cast<size_t>(needed));
            MultiByteToWideChar(CP_UTF8, 0, bytes.data(), static_cast<int>(bytes.size()),
                                decoded.data(), needed);
        }
    }

    return ToLower(std::move(decoded));
}

bool ScheduledTaskMatchesConfig(const wchar_t* taskName,
                                const std::wstring& expectedExe,
                                const std::wstring& expectedArgSubstr) {
    std::string xmlBytes;
    DWORD exitCode = 1;
    const std::wstring queryArgs = std::wstring(L"/query /tn \"") + taskName + L"\" /xml";
    if (!RunProcessCaptureOutput(L"schtasks.exe", queryArgs, xmlBytes, exitCode) || exitCode != 0) {
        return false;
    }

    const std::wstring xmlLower = DecodeTaskXmlToLower(xmlBytes);
    if (xmlLower.empty()) return false;

    return xmlLower.find(ToLower(expectedExe)) != std::wstring::npos &&
           xmlLower.find(ToLower(expectedArgSubstr)) != std::wstring::npos;
}

void SetupPasswordlessTasks(const IslandSettings& s) {
    if (s.wgExePath.empty() || s.configPath.empty() || s.tunnelName.empty()) return;

    const std::wstring cmd =
        L"/c schtasks /create /tn \"" + std::wstring(kTaskStartName) +
        L"\" /tr \"\\\"" + s.wgExePath + L"\\\" /installtunnelservice \\\"" + s.configPath +
        L"\\\"\" /sc ONCE /st 00:00 /rl HIGHEST /f & "
        L"schtasks /create /tn \"" + std::wstring(kTaskStopName) +
        L"\" /tr \"\\\"" + s.wgExePath + L"\\\" /uninstalltunnelservice " + s.tunnelName +
        L"\" /sc ONCE /st 00:00 /rl HIGHEST /f";

    ShellExecuteW(nullptr, L"runas", L"cmd.exe", cmd.c_str(), nullptr, SW_HIDE);
}

void RequestVpnToggle() {
    if (g_unloading.load()) return;
    g_toggleRequested.store(true);
    if (g_wakeEvent) SetEvent(g_wakeEvent);
}

// ===========================================================================
// MOUNTED HWND TRACKING & UI HELPERS
// ===========================================================================
std::vector<HWND> GetMountedHwndsSnapshot() {
    std::lock_guard<std::mutex> lock(g_mountedMutex);
    return g_mountedHwnds;
}

void AddMountedHwnd(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_mountedMutex);
    if (std::find(g_mountedHwnds.begin(), g_mountedHwnds.end(), hWnd) == g_mountedHwnds.end()) {
        g_mountedHwnds.push_back(hWnd);
    }
}

void RemoveMountedHwnd(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_mountedMutex);
    g_mountedHwnds.erase(std::remove(g_mountedHwnds.begin(), g_mountedHwnds.end(), hWnd),
                         g_mountedHwnds.end());
}

IslandHost* FindHostByHwnd(HWND hWnd) {
    for (auto& host : g_hosts) {
        if (host.hWnd == hWnd) return &host;
    }
    return nullptr;
}

constexpr std::wstring_view kSvgShield =
    L"M11.9999 23.999C11.7902 23.9991 11.5827 23.9551 11.3909 23.87C10.9999 23.699 1.90894 19.568 1.90894 11.999V7.24601C1.90769 6.09129 2.27044 4.96556 2.94563 4.02881C3.62081 3.09206 4.57408 2.39196 5.66994 2.02801L11.5249 0.0760076C11.8334 -0.0259869 12.1665 -0.0259869 12.4749 0.0760076L18.3299 2.02801C19.4257 2.39209 20.3789 3.09223 21.0541 4.02895C21.7292 4.96567 22.092 6.09133 22.0909 7.24601V11.999C22.0909 20.617 12.8669 23.791 12.4749 23.922C12.3218 23.9732 12.1614 23.9993 11.9999 23.999V23.999ZM11.9999 3.08001L6.61894 4.87401C6.121 5.03985 5.68789 5.3582 5.38098 5.78393C5.07407 6.20966 4.90892 6.72118 4.90894 7.24601V11.999C4.90894 16.734 10.3299 19.951 12.0769 20.864C13.8339 20.131 19.0909 17.481 19.0909 11.999V7.24601C19.091 6.72118 18.9258 6.20966 18.6189 5.78393C18.312 5.3582 17.8789 5.03985 17.3809 4.87401L11.9999 3.08001Z";

constexpr std::wstring_view kSvgShieldExclamation =
    L"M12 24.001C11.767 24.001 11.5372 23.9466 11.329 23.842L10.949 23.651C8.852 22.601 2 18.672 2 12.152V7.05199C1.99921 5.94257 2.34835 4.86116 2.99775 3.96166C3.64715 3.06217 4.56371 2.39041 5.617 2.04199L11.528 0.0769917C11.8346 -0.0240363 12.1654 -0.0240363 12.472 0.0769917L18.383 2.03699C19.4363 2.38541 20.3529 3.05717 21.0023 3.95666C21.6517 4.85616 22.0008 5.93757 22 7.04699V12.147C22 19.517 15.078 22.875 12.957 23.728L12.563 23.887C12.3845 23.9611 12.1933 23.9999 12 24.001V24.001ZM12 3.08099L6.561 4.88099C6.10567 5.03134 5.70952 5.32177 5.42916 5.71079C5.1488 6.0998 4.9986 6.56748 5 7.04699V12.147C5 16.754 10.143 19.852 12.068 20.847C13.968 20.039 19 17.403 19 12.147V7.04699C19.0006 6.56816 18.85 6.10138 18.5697 5.71317C18.2894 5.32495 17.8937 5.03512 17.439 4.88499L12 3.08099ZM13.5 12.501V6.50099C13.5 6.10317 13.342 5.72164 13.0607 5.44033C12.7794 5.15903 12.3978 5.00099 12 5.00099C11.6022 5.00099 11.2206 5.15903 10.9393 5.44033C10.658 5.72164 10.5 6.10317 10.5 6.50099V12.501C10.5 12.8988 10.658 13.2803 10.9393 13.5617C11.2206 13.843 11.6022 14.001 12 14.001C12.3978 14.001 12.7794 13.843 13.0607 13.5617C13.342 13.2803 13.5 12.8988 13.5 12.501ZM12 15.001C11.7033 15.001 11.4133 15.089 11.1666 15.2538C10.92 15.4186 10.7277 15.6529 10.6142 15.927C10.5007 16.2011 10.4709 16.5027 10.5288 16.7936C10.5867 17.0846 10.7296 17.3519 10.9393 17.5617C11.1491 17.7714 11.4164 17.9143 11.7074 17.9722C11.9983 18.03 12.2999 18.0003 12.574 17.8868C12.8481 17.7733 13.0824 17.581 13.2472 17.3343C13.412 17.0877 13.5 16.7977 13.5 16.501C13.5 16.1032 13.342 15.7216 13.0607 15.4403C12.7794 15.159 12.3978 15.001 12 15.001V15.001Z";

constexpr std::wstring_view kSvgShieldSolid =
    L"M18.581 2.14073L12.316 0.0517291C12.1109 -0.016592 11.8891 -0.016592 11.684 0.0517291L5.418 2.14073C4.42209 2.47178 3.55582 3.10823 2.94224 3.95967C2.32866 4.81111 1.99897 5.83424 2 6.88373V12.0007C2 19.5637 11.2 23.7407 11.594 23.9147C11.7218 23.9715 11.8601 24.0009 12 24.0009C12.1399 24.0009 12.2782 23.9715 12.406 23.9147C12.8 23.7407 22 19.5637 22 12.0007V6.88373C22.001 5.83412 21.6711 4.81089 21.0574 3.95944C20.4436 3.10799 19.5771 2.47161 18.581 2.14073V2.14073Z";

constexpr std::wstring_view kSvgShieldCheckSolid =
    L"M18.581 2.14073L12.316 0.0517291C12.1109 -0.016592 11.8891 -0.016592 11.684 0.0517291L5.419 2.14073C4.42291 2.47161 3.55642 3.10799 2.94265 3.95944C2.32887 4.81089 1.99904 5.83412 2 6.88373V12.0007C2 19.5637 11.2 23.7407 11.594 23.9147C11.7218 23.9715 11.8601 24.0009 12 24.0009C12.1399 24.0009 12.2782 23.9715 12.406 23.9147C12.8 23.7407 22 19.5637 22 12.0007V6.88373C22.001 5.83412 21.6711 4.81089 21.0574 3.95944C20.4436 3.10799 19.5771 2.47161 18.581 2.14073V2.14073ZM16.718 9.71773L12.446 13.9897C12.2712 14.1657 12.0632 14.3052 11.834 14.4002C11.6048 14.4951 11.3591 14.5435 11.111 14.5427H11.078C10.8249 14.5389 10.5752 14.4837 10.344 14.3806C10.1128 14.2774 9.90498 14.1285 9.733 13.9427L7.427 11.5427C7.32713 11.4501 7.24725 11.338 7.19226 11.2133C7.13726 11.0887 7.10831 10.9542 7.10718 10.8179C7.10606 10.6817 7.13278 10.5467 7.1857 10.4211C7.23863 10.2956 7.31665 10.1822 7.41497 10.0879C7.51329 9.99363 7.62985 9.92042 7.75748 9.87278C7.88511 9.82515 8.02113 9.80409 8.15719 9.81091C8.29325 9.81773 8.42648 9.85227 8.54872 9.91243C8.67095 9.97258 8.7796 10.0571 8.868 10.1607L11.112 12.5007L15.3 8.30073C15.4886 8.11857 15.7412 8.01778 16.0034 8.02006C16.2656 8.02234 16.5164 8.1275 16.7018 8.31291C16.8872 8.49832 16.9924 8.74913 16.9947 9.01133C16.997 9.27353 16.8962 9.52613 16.714 9.71473L16.718 9.71773Z";

constexpr std::wstring_view kSvgShieldExclamationSolid =
    L"M18.581 2.14073L12.316 0.0517291C12.1109 -0.016592 11.8891 -0.016592 11.684 0.0517291L5.418 2.14073C4.42209 2.47178 3.55582 3.10823 2.94224 3.95967C2.32866 4.81111 1.99897 5.83424 2 6.88373V12.0007C2 19.5637 11.2 23.7407 11.594 23.9147C11.7218 23.9715 11.8601 24.0009 12 24.0009C12.1399 24.0009 12.2782 23.9715 12.406 23.9147C12.8 23.7407 22 19.5637 22 12.0007V6.88373C22.001 5.83412 21.6711 4.81089 21.0574 3.95944C20.4436 3.10799 19.5771 2.47161 18.581 2.14073V2.14073ZM12 19.0007C11.8022 19.0007 11.6089 18.9421 11.4444 18.8322C11.28 18.7223 11.1518 18.5661 11.0761 18.3834C11.0004 18.2007 10.9806 17.9996 11.0192 17.8056C11.0578 17.6117 11.153 17.4335 11.2929 17.2936C11.4327 17.1538 11.6109 17.0585 11.8049 17.0199C11.9989 16.9814 12.2 17.0012 12.3827 17.0769C12.5654 17.1525 12.7216 17.2807 12.8315 17.4452C12.9414 17.6096 13 17.8029 13 18.0007C13 18.2659 12.8946 18.5203 12.7071 18.7078C12.5196 18.8954 12.2652 19.0007 12 19.0007ZM13 14.0007C13 14.2659 12.8946 14.5203 12.7071 14.7078C12.5196 14.8954 12.2652 15.0007 12 15.0007C11.7348 15.0007 11.4804 14.8954 11.2929 14.7078C11.1054 14.5203 11 14.2659 11 14.0007V6.00073C11 5.73551 11.1054 5.48116 11.2929 5.29362C11.4804 5.10609 11.7348 5.00073 12 5.00073C12.2652 5.00073 12.5196 5.10609 12.7071 5.29362C12.8946 5.48116 13 5.73551 13 6.00073V14.0007Z";

constexpr std::wstring_view kSvgGlobe =
    L"M12 0C9.62663 0 7.30655 0.703788 5.33316 2.02236C3.35977 3.34094 1.8217 5.21509 0.913451 7.4078C0.00519943 9.60051 -0.232441 12.0133 0.230582 14.3411C0.693605 16.6689 1.83649 18.8071 3.51472 20.4853C5.19295 22.1635 7.33115 23.3064 9.65892 23.7694C11.9867 24.2324 14.3995 23.9948 16.5922 23.0866C18.7849 22.1783 20.6591 20.6402 21.9776 18.6668C23.2962 16.6935 24 14.3734 24 12C23.9966 8.81846 22.7312 5.76821 20.4815 3.51852C18.2318 1.26883 15.1815 0.00344108 12 0V0ZM20.941 11H17.463C17.2362 8.39489 16.4558 5.86823 15.174 3.589C16.7235 4.17499 18.0815 5.17671 19.0989 6.48411C20.1163 7.79151 20.7537 9.35402 20.941 11V11ZM9.68501 14H14.315C13.9213 16.0948 13.1359 18.0964 12 19.9C10.864 18.0965 10.0786 16.0948 9.68501 14V14ZM9.55301 11C9.83307 8.54378 10.67 6.18386 12 4.1C13.3302 6.18379 14.1671 8.54374 14.447 11H9.55301ZM8.82601 3.589C7.54417 5.86823 6.76377 8.39489 6.53701 11H3.05901C3.24635 9.35402 3.88372 7.79151 4.9011 6.48411C5.91847 5.17671 7.2765 4.17499 8.82601 3.589V3.589ZM3.23201 14H6.64101C6.97749 16.2511 7.7177 18.423 8.82601 20.411C7.43814 19.885 6.20149 19.0248 5.22569 17.9064C4.24989 16.7881 3.56509 15.4463 3.23201 14V14ZM15.174 20.411C16.2828 18.4231 17.023 16.2512 17.359 14H20.768C20.4349 15.4463 19.7501 16.7881 18.7743 17.9064C17.7985 19.0248 16.5619 19.885 15.174 20.411V20.411Z";

constexpr std::wstring_view kSvgWorld =
    L"M12 0C9.62663 0 7.30655 0.703788 5.33316 2.02236C3.35977 3.34094 1.8217 5.21509 0.913451 7.4078C0.00519943 9.60051 -0.232441 12.0133 0.230582 14.3411C0.693605 16.6689 1.83649 18.8071 3.51472 20.4853C5.19295 22.1635 7.33115 23.3064 9.65892 23.7694C11.9867 24.2324 14.3995 23.9948 16.5922 23.0866C18.7849 22.1783 20.6591 20.6402 21.9776 18.6668C23.2962 16.6935 24 14.3734 24 12C23.9966 8.81846 22.7312 5.76821 20.4815 3.51852C18.2318 1.26883 15.1815 0.00344108 12 0V0ZM12.5 20.975V19.69C12.5 18.5787 12.0586 17.513 11.2728 16.7272C10.487 15.9414 9.42126 15.5 8.31001 15.5H8.30001C7.88714 15.5 7.47831 15.4187 7.09688 15.2607C6.71545 15.1026 6.36889 14.871 6.07701 14.579L3.02401 11.524C3.10556 9.93535 3.60741 8.39683 4.47829 7.06565C5.34917 5.73447 6.55785 4.65836 7.98081 3.94728C9.40378 3.23621 10.99 2.91566 12.5774 3.01839C14.1649 3.12112 15.6966 3.64345 17.016 4.532C16.7512 4.72633 16.4315 4.83139 16.103 4.832H14.669C14.4645 4.832 14.2621 4.87232 14.0732 4.95065C13.8844 5.02899 13.7128 5.1438 13.5684 5.28851C13.424 5.43322 13.3095 5.605 13.2315 5.79401C13.1535 5.98301 13.1136 6.18554 13.114 6.39V7.432C13.114 7.84441 12.9502 8.23993 12.6586 8.53155C12.3669 8.82317 11.9714 8.987 11.559 8.987V8.987C11.3545 8.98647 11.1518 9.02631 10.9627 9.10422C10.7736 9.18213 10.6017 9.29659 10.4569 9.44103C10.312 9.58548 10.1972 9.75708 10.1188 9.946C10.0404 10.1349 10 10.3375 10 10.542V11.584C10 11.9964 10.1638 12.3919 10.4555 12.6836C10.7471 12.9752 11.1426 13.139 11.555 13.139H15.711C16.1234 13.139 16.5189 13.3028 16.8106 13.5945C17.1022 13.8861 17.266 14.2816 17.266 14.694V16.214C17.266 16.4183 17.3063 16.6206 17.3845 16.8094C17.4628 16.9981 17.5775 17.1696 17.722 17.314L18.555 18.147C16.9767 19.8349 14.8074 20.8481 12.5 20.975Z";

constexpr std::wstring_view kSvgCloudCheck =
    L"M11.671 23.9999C11.212 24.0001 10.7574 23.9096 10.3334 23.7337C9.90934 23.5578 9.5242 23.3 9.20004 22.9749L5.81004 19.5559C5.52998 19.2733 5.37364 18.8911 5.37542 18.4932C5.3772 18.0953 5.53696 17.7145 5.81954 17.4344C6.10213 17.1544 6.4844 16.998 6.88226 16.9998C7.28012 17.0016 7.66098 17.1613 7.94104 17.4439L11.326 20.8579C11.4195 20.9495 11.5452 21.0009 11.676 21.0009C11.8069 21.0009 11.9326 20.9495 12.026 20.8579L17.44 15.4429C17.7214 15.1615 18.1031 15.0034 18.501 15.0034C18.899 15.0034 19.2806 15.1615 19.562 15.4429C19.8434 15.7243 20.0015 16.106 20.0015 16.5039C20.0015 16.9019 19.8434 17.2835 19.562 17.5649L14.147 22.9789C13.822 23.3038 13.4359 23.5613 13.0109 23.7365C12.586 23.9118 12.1307 24.0013 11.671 23.9999V23.9999ZM2.08504 18.8999C2.46834 18.7782 2.78863 18.5111 2.9771 18.1558C3.16557 17.8005 3.2072 17.3855 3.09304 16.9999C2.93659 16.3879 2.99457 15.7408 3.25732 15.1663C3.52007 14.5919 3.97173 14.1248 4.53704 13.8429C5.11534 13.5314 5.54658 13.0034 5.73627 12.3745C5.92596 11.7457 5.85864 11.0673 5.54904 10.4879C5.0335 9.4695 4.87301 8.30802 5.09304 7.18794C5.2821 6.15232 5.78996 5.20163 6.54566 4.46871C7.30135 3.73578 8.26713 3.25724 9.30804 3.09994C9.99248 2.99638 10.6907 3.03051 11.3618 3.20034C12.0329 3.37017 12.6633 3.67227 13.216 4.08894C13.7602 4.49701 14.2174 5.00962 14.5609 5.59673C14.9043 6.18384 15.1271 6.83361 15.216 7.50794C15.2598 7.8345 15.4098 8.13758 15.6429 8.37038C15.8761 8.60317 16.1794 8.75272 16.506 8.79594C17.8205 8.97008 19.0177 9.64276 19.85 10.6748C20.6824 11.7069 21.0863 13.0194 20.978 14.3409V14.3939C20.9644 14.5881 20.9895 14.7831 21.052 14.9675C21.1145 15.1519 21.213 15.322 21.342 15.4678C21.4709 15.6137 21.6276 15.7324 21.8029 15.8171C21.9782 15.9018 22.1686 15.9507 22.363 15.9609C22.7639 15.9822 23.1571 15.8457 23.4586 15.5807C23.76 15.3157 23.9458 14.9432 23.976 14.5429C24.1225 12.6484 23.6033 10.7621 22.508 9.20934C21.4127 7.65658 19.8099 6.53463 17.976 6.03694C17.5022 4.30244 16.4643 2.77511 15.026 1.69595C14.1577 1.03857 13.167 0.561028 12.1118 0.291274C11.0567 0.0215188 9.95832 -0.0350268 8.88104 0.124947C7.22049 0.372024 5.67866 1.13183 4.47111 2.29815C3.26355 3.46446 2.45065 4.97897 2.14604 6.62994C1.8295 8.25545 2.01829 9.93892 2.68704 11.4539C1.65911 12.1044 0.863558 13.0637 0.414443 14.1942C-0.0346712 15.3247 -0.114276 16.5684 0.187044 17.7469L0.234044 17.9209C0.322152 18.2248 0.507457 18.4915 0.76158 18.6801C1.0157 18.8686 1.32463 18.9687 1.64104 18.9649C1.79138 18.9645 1.94088 18.9426 2.08504 18.8999V18.8999Z";

constexpr std::wstring_view kSvgDatabase =
    L"M22.5622 8.5C23.2538 7.79844 23.7225 6.90798 23.9092 5.94068C24.096 4.97338 23.9924 3.97245 23.6117 3.06385C23.2309 2.15524 22.5899 1.37955 21.7694 0.834372C20.9488 0.289197 19.9853 -0.00110165 19.0002 0H5.00016C4.015 -0.00110165 3.05152 0.289197 2.23095 0.834372C1.41038 1.37955 0.769392 2.15524 0.388632 3.06385C0.00787267 3.97245 -0.0956446 4.97338 0.0911059 5.94068C0.277856 6.90798 0.746531 7.79844 1.43816 8.5C0.516762 9.43189 0 10.6895 0 12C0 13.3105 0.516762 14.5681 1.43816 15.5C0.746531 16.2016 0.277856 17.092 0.0911059 18.0593C-0.0956446 19.0266 0.00787267 20.0276 0.388632 20.9362C0.769392 21.8448 1.41038 22.6205 2.23095 23.1656C3.05152 23.7108 4.015 24.0011 5.00016 24H19.0002C19.9853 24.0011 20.9488 23.7108 21.7694 23.1656C22.5899 22.6205 23.2309 21.8448 23.6117 20.9362C23.9924 20.0276 24.096 19.0266 23.9092 18.0593C23.7225 17.092 23.2538 16.2016 22.5622 15.5C23.4836 14.5681 24.0003 13.3105 24.0003 12C24.0003 10.6895 23.4836 9.43189 22.5622 8.5V8.5ZM5.00016 14C4.46972 14 3.96102 13.7893 3.58594 13.4142C3.21087 13.0391 3.00016 12.5304 3.00016 12C3.00016 11.4696 3.21087 10.9609 3.58594 10.5858C3.96102 10.2107 4.46972 10 5.00016 10V10.5C5.00016 10.8978 5.15819 11.2794 5.4395 11.5607C5.7208 11.842 6.10233 12 6.50016 12C6.89798 12 7.27951 11.842 7.56082 11.5607C7.84212 11.2794 8.00016 10.8978 8.00016 10.5V10H10.0002V10.5C10.0002 10.8978 10.1582 11.2794 10.4395 11.5607C10.7208 11.842 11.1023 12 11.5002 12C11.898 12 12.2795 11.842 12.5608 11.5607C12.8421 11.2794 13.0002 10.8978 13.0002 10.5V10H19.0002C19.5306 10 20.0393 10.2107 20.4144 10.5858C20.7894 10.9609 21.0002 11.4696 21.0002 12C21.0002 12.5304 20.7894 13.0391 20.4144 13.4142C20.0393 13.7893 19.5306 14 19.0002 14H5.00016ZM5.00016 3V3.5C5.00016 3.89783 5.15819 4.27936 5.4395 4.56066C5.7208 4.84197 6.10233 5 6.50016 5C6.89798 5 7.27951 4.84197 7.56082 4.56066C7.84212 4.27936 8.00016 3.89783 8.00016 3.5V3H10.0002V3.5C10.0002 3.89783 10.1582 4.27936 10.4395 4.56066C10.7208 4.84197 11.1023 5 11.5002 5C11.898 5 12.2795 4.84197 12.5608 4.56066C12.8421 4.27936 13.0002 3.89783 13.0002 3.5V3H19.0002C19.5306 3 20.0393 3.21072 20.4144 3.58579C20.7894 3.96086 21.0002 4.46957 21.0002 5C21.0002 5.53044 20.7894 6.03914 20.4144 6.41422C20.0393 6.78929 19.5306 7 19.0002 7H5.00016C4.46972 7 3.96102 6.78929 3.58594 6.41422C3.21087 6.03914 3.00016 5.53044 3.00016 5C3.00016 4.46957 3.21087 3.96086 3.58594 3.58579C3.96102 3.21072 4.46972 3 5.00016 3V3ZM19.0002 21H5.00016C4.46972 21 3.96102 20.7893 3.58594 20.4142C3.21087 20.0391 3.00016 19.5304 3.00016 19C3.00016 18.4696 3.21087 17.9609 3.58594 17.5858C3.96102 17.2107 4.46972 17 5.00016 17V17.5C5.00016 17.8978 5.15819 18.2794 5.4395 18.5607C5.7208 18.842 6.10233 19 6.50016 19C6.89798 19 7.27951 18.842 7.56082 18.5607C7.84212 18.2794 8.00016 17.8978 8.00016 17.5V17H10.0002V17.5C10.0002 17.8978 10.1582 18.2794 10.4395 18.5607C10.7208 18.842 11.1023 19 11.5002 19C11.898 19 12.2795 18.842 12.5608 18.5607C12.8421 18.2794 13.0002 17.8978 13.0002 17.5V17H19.0002C19.5306 17 20.0393 17.2107 20.4144 17.5858C20.7894 17.9609 21.0002 18.4696 21.0002 19C21.0002 19.5304 20.7894 20.0391 20.4144 20.4142C20.0393 20.7893 19.5306 21 19.0002 21Z";

constexpr std::wstring_view kSvgPhysics =
    L"M22.4 1.59241C20.588 -0.214594 17.4 -1.16659 11.983 2.60041C6.57096 -1.16559 3.37796 -0.214594 1.56996 1.59241C-0.238037 3.39941 -1.19004 6.59041 2.57796 12.0004C-1.19004 17.4104 -0.237037 20.6004 1.56996 22.4084C2.06847 22.9222 2.6664 23.329 3.32728 23.604C3.98817 23.8791 4.69816 24.0167 5.41396 24.0084C7.11396 24.0084 9.27096 23.3084 11.983 21.4144C14.696 23.3034 16.851 24.0094 18.552 24.0094C19.2678 24.018 19.9779 23.8806 20.6388 23.6055C21.2997 23.3304 21.8976 22.9234 22.396 22.4094C24.2 20.6004 25.157 17.4104 21.388 12.0004C25.157 6.59041 24.2 3.40041 22.4 1.59241V1.59241ZM18.472 3.00841C18.8043 2.99772 19.1353 3.0549 19.4448 3.17646C19.7543 3.29802 20.0357 3.48141 20.272 3.71541C21.627 5.06941 20.857 7.31541 19.449 9.51541C18.743 8.69441 17.962 7.84041 17.049 6.93141C16.136 6.02241 15.285 5.24241 14.464 4.53641C15.6395 3.67824 17.0235 3.15063 18.472 3.00841V3.00841ZM17.617 12.0004C16.649 13.1714 15.659 14.2144 14.929 14.9444C14.199 15.6744 13.154 16.6644 11.983 17.6314C10.812 16.6644 9.76796 15.6754 9.03796 14.9444C8.30796 14.2134 7.31696 13.1714 6.34996 12.0004C7.31696 10.8294 8.30696 9.78641 9.03796 9.05641C9.76896 8.32641 10.811 7.33541 11.983 6.36841C13.155 7.33541 14.198 8.32541 14.929 9.05641C15.66 9.78741 16.649 10.8294 17.617 12.0004ZM3.69396 3.71541C3.93015 3.48136 4.2116 3.29794 4.52109 3.17638C4.83059 3.05482 5.16162 2.99766 5.49396 3.00841C6.94203 3.15123 8.32536 3.67956 9.49996 4.53841C8.67796 5.24441 7.82396 6.02441 6.91496 6.93341C6.00596 7.84241 5.22296 8.70041 4.51696 9.51741C3.10996 7.31341 2.33896 5.06941 3.69396 3.71541V3.71541ZM3.69396 20.2854C2.33896 18.9314 3.10996 16.6854 4.51696 14.4854C5.22296 15.3064 6.00396 16.1604 6.91696 17.0694C7.82996 17.9784 8.67896 18.7694 9.49596 19.4694C7.28996 20.8714 5.04796 21.6384 3.69396 20.2854V20.2854ZM20.272 20.2854C18.917 21.6394 16.672 20.8714 14.472 19.4674C15.29 18.7674 16.148 17.9724 17.053 17.0674C17.958 16.1624 18.743 15.3044 19.453 14.4834C20.857 16.6874 21.627 18.9314 20.272 20.2854V20.2854Z M12 14.0002C13.1046 14.0002 14 13.1048 14 12.0002C14 10.8957 13.1046 10.0002 12 10.0002C10.8954 10.0002 10 10.8957 10 12.0002C10 13.1048 10.8954 14.0002 12 14.0002Z";

constexpr std::wstring_view kSvgExclamation =
    L"M12 0C5.37258 0 0 5.37258 0 12C0 18.6274 5.37258 24 12 24C18.6274 24 24 18.6274 24 12C23.9928 5.37553 18.6245 0.00717187 12 0ZM12 21C7.02942 21 3 16.9706 3 12C3 7.02942 7.02942 3 12 3C16.9706 3 21 7.02942 21 12C20.9945 16.9683 16.9683 20.9945 12 21Z M12 15.091C12.8284 15.091 13.5 14.4195 13.5 13.591V6.59106C13.5 5.76264 12.8284 5.09106 12 5.09106C11.1716 5.09106 10.5 5.76264 10.5 6.59106V13.591C10.5 14.4195 11.1716 15.091 12 15.091Z M12.0051 19C12.8336 19 13.5051 18.3284 13.5051 17.5C13.5051 16.6716 12.8336 16 12.0051 16C11.1767 16 10.5051 16.6716 10.5051 17.5C10.5051 18.3284 11.1767 19 12.0051 19Z";

constexpr std::wstring_view kSvgMediaPlay =
    L"M7.5 4.4C6.6 3.9 5.5 4.5 5.5 5.6V18.4C5.5 19.5 6.6 20.1 7.5 19.6L19.1 13.2C20.1 12.7 20.1 11.3 19.1 10.8L7.5 4.4Z";

constexpr std::wstring_view kSvgMediaPause =
    L"M7 4.5H8C8.83 4.5 9.5 5.17 9.5 6V18C9.5 18.83 8.83 19.5 8 19.5H7C6.17 19.5 5.5 18.83 5.5 18V6C5.5 5.17 6.17 4.5 7 4.5Z M16 4.5H17C17.83 4.5 18.5 5.17 18.5 6V18C18.5 18.83 17.83 19.5 17 19.5H16C15.17 19.5 14.5 18.83 14.5 18V6C14.5 5.17 15.17 4.5 16 4.5Z";

constexpr std::wstring_view kSvgMediaPrev =
    L"M5.75 5C6.44 5 7 5.56 7 6.25V17.75C7 18.44 6.44 19 5.75 19C5.06 19 4.5 18.44 4.5 17.75V6.25C4.5 5.56 5.06 5 5.75 5Z M17.7 5.6L9.1 10.9C8.2 11.4 8.2 12.6 9.1 13.1L17.7 18.4C18.6 18.9 19.5 18.3 19.5 17.3V6.7C19.5 5.7 18.6 5.1 17.7 5.6Z";

constexpr std::wstring_view kSvgMediaNext =
    L"M18.25 5C18.94 5 19.5 5.56 19.5 6.25V17.75C19.5 18.44 18.94 19 18.25 19C17.56 19 17 18.44 17 17.75V6.25C17 5.56 17.56 5 18.25 5Z M6.3 5.6L14.9 10.9C15.8 11.4 15.8 12.6 14.9 13.1L6.3 18.4C5.4 18.9 4.5 18.3 4.5 17.3V6.7C4.5 5.7 5.4 5.1 6.3 5.6Z";

constexpr std::wstring_view kSvgMediaNote =
    L"M14 4.5L19.5 3.1C20.1 2.95 20.6 3.4 20.6 4V6.2C20.6 6.65 20.3 7.05 19.85 7.16L16 8.1V15.5C16 17.43 14.43 19 12.5 19C10.57 19 9 17.43 9 15.5C9 13.57 10.57 12 12.5 12C13.05 12 13.56 12.12 14 12.34V4.5Z";

constexpr std::wstring_view kSvgMediaShuffle =
    L"M16.3 5.2C16.7 4.8 17.3 4.8 17.7 5.2L20.7 8.2C21.1 8.6 21.1 9.2 20.7 9.6L17.7 12.6C17.3 13 16.7 13 16.3 12.6C15.9 12.2 15.9 11.6 16.3 11.2L17.6 9.9H15.2C14.1 9.9 13.1 10.5 12.5 11.4L10.3 14.9C9.3 16.4 7.7 17.3 5.9 17.3H4C3.4 17.3 3 16.9 3 16.3C3 15.7 3.4 15.3 4 15.3H5.9C7 15.3 8 14.7 8.6 13.8L10.8 10.3C11.8 8.8 13.4 7.9 15.2 7.9H17.6L16.3 6.6C15.9 6.2 15.9 5.6 16.3 5.2Z M4 6.7H5.9C7.4 6.7 8.8 7.3 9.7 8.4L8.6 10.1C7.9 9.2 6.9 8.7 5.9 8.7H4C3.4 8.7 3 8.3 3 7.7C3 7.1 3.4 6.7 4 6.7Z M17.7 14.2L20.7 17.2C21.1 17.6 21.1 18.2 20.7 18.6L17.7 21.6C17.3 22 16.7 22 16.3 21.6C15.9 21.2 15.9 20.6 16.3 20.2L17.6 18.9H15.2C13.8 18.9 12.5 18.3 11.5 17.3L12.7 15.6C13.3 16.4 14.2 16.9 15.2 16.9H17.6L16.3 15.6C15.9 15.2 15.9 14.6 16.3 14.2C16.7 13.8 17.3 13.8 17.7 14.2Z";

constexpr std::wstring_view kSvgMediaRepeat =
    L"M17.3 3.3C17.7 2.9 18.3 2.9 18.7 3.3L21.5 6.1C21.9 6.5 21.9 7.1 21.5 7.5L18.7 10.3C18.3 10.7 17.7 10.7 17.3 10.3C16.9 9.9 16.9 9.3 17.3 8.9L18.4 7.8H7.5C5.8 7.8 4.5 9.1 4.5 10.8V12C4.5 12.6 4.1 13 3.5 13C2.9 13 2.5 12.6 2.5 12V10.8C2.5 8 4.7 5.8 7.5 5.8H18.4L17.3 4.7C16.9 4.3 16.9 3.7 17.3 3.3Z M20.5 11C21.1 11 21.5 11.4 21.5 12V13.2C21.5 16 19.3 18.2 16.5 18.2H5.6L6.7 19.3C7.1 19.7 7.1 20.3 6.7 20.7C6.3 21.1 5.7 21.1 5.3 20.7L2.5 17.9C2.1 17.5 2.1 16.9 2.5 16.5L5.3 13.7C5.7 13.3 6.3 13.3 6.7 13.7C7.1 14.1 7.1 14.7 6.7 15.1L5.6 16.2H16.5C18.2 16.2 19.5 14.9 19.5 13.2V12C19.5 11.4 19.9 11 20.5 11Z";

constexpr std::wstring_view kSvgMediaRepeatOne =
    L"M17.3 3.3C17.7 2.9 18.3 2.9 18.7 3.3L21.5 6.1C21.9 6.5 21.9 7.1 21.5 7.5L18.7 10.3C18.3 10.7 17.7 10.7 17.3 10.3C16.9 9.9 16.9 9.3 17.3 8.9L18.4 7.8H7.5C5.8 7.8 4.5 9.1 4.5 10.8V12C4.5 12.6 4.1 13 3.5 13C2.9 13 2.5 12.6 2.5 12V10.8C2.5 8 4.7 5.8 7.5 5.8H18.4L17.3 4.7C16.9 4.3 16.9 3.7 17.3 3.3Z M20.5 11C21.1 11 21.5 11.4 21.5 12V13.2C21.5 16 19.3 18.2 16.5 18.2H5.6L6.7 19.3C7.1 19.7 7.1 20.3 6.7 20.7C6.3 21.1 5.7 21.1 5.3 20.7L2.5 17.9C2.1 17.5 2.1 16.9 2.5 16.5L5.3 13.7C5.7 13.3 6.3 13.3 6.7 13.7C7.1 14.1 7.1 14.7 6.7 15.1L5.6 16.2H16.5C18.2 16.2 19.5 14.9 19.5 13.2V12C19.5 11.4 19.9 11 20.5 11Z M11.2 9.2H12.9V14.8H11.4V10.7L10.3 11.3L9.8 10.2L11.2 9.2Z";

constexpr std::wstring_view kSvgMediaAdd =
    L"M12 4.5C12.55 4.5 13 4.95 13 5.5V11H18.5C19.05 11 19.5 11.45 19.5 12C19.5 12.55 19.05 13 18.5 13H13V18.5C13 19.05 12.55 19.5 12 19.5C11.45 19.5 11 19.05 11 18.5V13H5.5C4.95 13 4.5 12.55 4.5 12C4.5 11.45 4.95 11 5.5 11H11V5.5C11 4.95 11.45 4.5 12 4.5Z";

constexpr std::wstring_view kSvgMediaCheck =
    L"M19.7 6.3C20.1 6.7 20.1 7.3 19.7 7.7L10.4 17C10 17.4 9.4 17.4 9 17L4.3 12.3C3.9 11.9 3.9 11.3 4.3 10.9C4.7 10.5 5.3 10.5 5.7 10.9L9.7 14.9L18.3 6.3C18.7 5.9 19.3 5.9 19.7 6.3Z";

MediaSnapshot GetMediaSnapshot() {
    std::lock_guard<std::mutex> lock(g_mediaMutex);
    return g_mediaSnapshot;
}

void RequestMediaCommand(MediaCommand cmd) {
    if (g_unloading.load()) return;
    g_pendingMediaCmd.store(static_cast<int>(cmd));
    if (g_mediaWakeEvent) SetEvent(g_mediaWakeEvent);
}

void RequestMediaSeek(int64_t ticks) {
    if (g_unloading.load()) return;
    g_pendingSeekTicks.store(std::max<int64_t>(0, ticks));
    g_pendingMediaCmd.store(static_cast<int>(MediaCommand::Seek));
    if (g_mediaWakeEvent) SetEvent(g_mediaWakeEvent);
}

void RequestSessionSwitch(const std::wstring& aumid) {
    if (g_unloading.load()) return;
    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        g_pendingTargetSessionAumid = aumid;
    }
    g_pendingMediaCmd.store(static_cast<int>(MediaCommand::SwitchToSession));
    if (g_mediaWakeEvent) SetEvent(g_mediaWakeEvent);
}

std::wstring FormatTimeFromTicks(int64_t ticks) {
    if (ticks < 0) ticks = 0;
    const int64_t totalSecs = ticks / 10000000LL;
    const int64_t hours = totalSecs / 3600;
    const int64_t mins = (totalSecs % 3600) / 60;
    const int64_t secs = totalSecs % 60;

    wchar_t buf[32]{};
    if (hours > 0) {
        swprintf_s(buf, L"%lld:%02lld:%02lld", hours, mins, secs);
    } else {
        swprintf_s(buf, L"%lld:%02lld", mins, secs);
    }
    return buf;
}

std::wstring ResolveAppDisplayName(const std::wstring& aumid) {
    if (aumid.empty()) return L"Media";

    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        auto it = g_appNameCache.find(aumid);
        if (it != g_appNameCache.end()) {
            return it->second;
        }
    }

    std::wstring resolved;
    try {
        const std::wstring shellPath = L"shell:AppsFolder\\" + aumid;
        IShellItem* pItem = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(shellPath.c_str(), nullptr, IID_PPV_ARGS(&pItem))) && pItem) {
            PWSTR pName = nullptr;
            if (SUCCEEDED(pItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pName)) && pName) {
                resolved = Trim(pName);
                CoTaskMemFree(pName);
            }
            pItem->Release();
        }
    } catch (...) {}

    if (resolved.empty()) {
        const std::wstring low = ToLower(aumid);
        if (low.find(L"firefox") != std::wstring::npos ||
            low.find(L"308046b0af4a39cb") != std::wstring::npos ||
            low.find(L"e7cf176e110c211b") != std::wstring::npos ||
            low.find(L"6f193ccc56814779") != std::wstring::npos) {
            resolved = L"Firefox";
        } else if (low.find(L"chrome") != std::wstring::npos) {
            resolved = L"Chrome";
        } else if (low.find(L"msedge") != std::wstring::npos) {
            resolved = L"Edge";
        } else if (low.find(L"opera") != std::wstring::npos) {
            resolved = L"Opera";
        } else if (low.find(L"yandex") != std::wstring::npos) {
            resolved = IsRussianUi() ? L"Яндекс" : L"Yandex";
        } else if (low.find(L"brave") != std::wstring::npos) {
            resolved = L"Brave";
        } else if (low.find(L"vivaldi") != std::wstring::npos) {
            resolved = L"Vivaldi";
        } else if (low.find(L"zen") != std::wstring::npos) {
            resolved = L"Zen";
        } else if (low.find(L"spotify") != std::wstring::npos) {
            resolved = L"Spotify";
        } else if (low.find(L"telegram") != std::wstring::npos) {
            resolved = L"Telegram";
        } else if (low.find(L"vlc") != std::wstring::npos) {
            resolved = L"VLC";
        } else if (low.find(L"foobar") != std::wstring::npos) {
            resolved = L"foobar2000";
        } else {
            std::wstring s = aumid;
            const size_t bang = s.find(L'!');
            if (bang != std::wstring::npos) s.resize(bang);
            const size_t semi = s.find(L';');
            if (semi != std::wstring::npos) s.resize(semi);
            const size_t dot = s.rfind(L'.');
            if (dot != std::wstring::npos && s.size() - dot <= 5) s.resize(dot);
            const size_t slash = s.find_last_of(L"\\/");
            if (slash != std::wstring::npos) s = s.substr(slash + 1);
            resolved = s.empty() ? L"Media" : s;
        }
    } else {
        const std::wstring lowRes = ToLower(resolved);
        if (lowRes == L"google chrome") resolved = L"Chrome";
        else if (lowRes == L"microsoft edge") resolved = L"Edge";
    }

    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        if (g_appNameCache.size() < 128) {
            g_appNameCache[aumid] = resolved;
        }
    }
    return resolved;
}

bool IsBrowserSession(std::wstring_view aumid, std::wstring_view resolvedName) {
    const std::wstring a = ToLower(std::wstring(aumid));
    const std::wstring r = ToLower(std::wstring(resolvedName));
    static constexpr std::wstring_view kBrowsers[] = {
        L"firefox", L"chrome", L"msedge", L"edge", L"opera", L"brave",
        L"yandex", L"vivaldi", L"zen", L"waterfox", L"floorp", L"librewolf",
        L"arc", L"browser", L"браузер",
        L"308046b0af4a39cb", L"e7cf176e110c211b", L"6f193ccc56814779"
    };
    for (const auto needle : kBrowsers) {
        if (a.find(needle) != std::wstring::npos || r.find(needle) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

void ApplyMiniPlayerArtToBrush(ImageBrush const& brush,
                               std::vector<BYTE> const& bytes,
                               bool adaptive,
                               double maxW,
                               double maxH,
                               FrameworkElement const& artBox) {
    if (!brush || bytes.empty()) return;
    try {
        IStream* pRawStream = SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
        if (!pRawStream) return;
        winrt::com_ptr<IStream> comStream;
        comStream.attach(pRawStream);

        winrt::Windows::Storage::Streams::IRandomAccessStream rasStream{nullptr};
        const HRESULT hr = ::CreateRandomAccessStreamOverStream(
            comStream.get(), BSOS_DEFAULT,
            winrt::guid_of<winrt::Windows::Storage::Streams::IRandomAccessStream>(),
            winrt::put_abi(rasStream));
        if (FAILED(hr) || !rasStream) return;

        winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage bmp;
        bmp.DecodePixelHeight(320);

        if (artBox) {
            if (!adaptive) {
                artBox.Width(maxW);
                artBox.Height(std::min(maxH, std::round(maxW * 0.65)));
            } else {
                const double initSide = std::min(maxW, maxH);
                artBox.Width(initSide);
                artBox.Height(initSide);
                bmp.ImageOpened([artBox, maxW, maxH](auto const& sender, auto const&) {
                    if (g_unloading.load()) return;
                    try {
                        if (auto b = sender.template try_as<winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage>()) {
                            const int pw = b.PixelWidth();
                            const int ph = b.PixelHeight();
                            if (pw > 0 && ph > 0) {
                                const double ratio = static_cast<double>(pw) / static_cast<double>(ph);
                                double w = maxW;
                                double h = maxH;
                                if (ratio >= (maxW / maxH)) {
                                    w = maxW;
                                    h = std::clamp(std::round(maxW / ratio), 90.0, maxH);
                                } else {
                                    h = maxH;
                                    w = std::clamp(std::round(maxH * ratio), 100.0, maxW);
                                }
                                artBox.Width(w);
                                artBox.Height(h);
                            }
                        }
                    } catch (...) {}
                });
            }
        }

        brush.ImageSource(bmp);
        bmp.SetSourceAsync(rasStream);
    } catch (...) {}
}

void ApplyAlbumArtToBrush(ImageBrush const& brush,
                          std::vector<BYTE> const& bytes,
                          int decodeHeight,
                          bool adaptiveWidth,
                          double targetHeight,
                          double maxWidth,
                          FrameworkElement const& artBox,
                          FrameworkElement const& artBackdrop,
                          FrameworkElement const& artRect) {
    if (!brush || bytes.empty()) return;
    try {
        IStream* pRawStream = SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
        if (!pRawStream) return;
        winrt::com_ptr<IStream> comStream;
        comStream.attach(pRawStream);

        winrt::Windows::Storage::Streams::IRandomAccessStream rasStream{nullptr};
        const HRESULT hr = ::CreateRandomAccessStreamOverStream(
            comStream.get(), BSOS_DEFAULT,
            winrt::guid_of<winrt::Windows::Storage::Streams::IRandomAccessStream>(),
            winrt::put_abi(rasStream));
        if (FAILED(hr) || !rasStream) return;

        winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage bmp;
        bmp.DecodePixelHeight(decodeHeight);
        if (adaptiveWidth && artBox) {
            artBox.Width(targetHeight);
            if (artBackdrop) artBackdrop.Width(targetHeight);
            if (artRect) artRect.Width(targetHeight);
            bmp.ImageOpened([artBox, artBackdrop, artRect, targetHeight, maxWidth](auto const& sender, auto const&) {
                if (g_unloading.load()) return;
                try {
                    if (auto b = sender.template try_as<winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage>()) {
                        const int pw = b.PixelWidth();
                        const int ph = b.PixelHeight();
                        if (pw > 0 && ph > 0) {
                            const double w = std::clamp(
                                std::round(targetHeight * static_cast<double>(pw) / static_cast<double>(ph)),
                                targetHeight, maxWidth);
                            if (artBox) artBox.Width(w);
                            if (artBackdrop) artBackdrop.Width(w);
                            if (artRect) artRect.Width(w);
                        }
                    }
                } catch (...) {}
            });
        }
        brush.ImageSource(bmp);
        bmp.SetSourceAsync(rasStream);
    } catch (...) {}
}

void ApplySurfaceStyle(Border const& pill, const IslandSettings& s, bool isLight) {
    if (!pill) return;

    const double cr = static_cast<double>(s.cornerRadius);
    pill.CornerRadius({cr, cr, cr, cr});

    const double bt = static_cast<double>(s.borderThickness);
    pill.BorderThickness({bt, bt, bt, bt});

    const BYTE borderAlpha = (s.borderThickness > 0)
        ? static_cast<BYTE>((s.borderOpacity * 255) / 100)
        : 0;
    winrt::Windows::UI::Color borderCol;
    if (s.borderColorMode == L"custom") {
        borderCol = {borderAlpha, s.borderColor.R, s.borderColor.G, s.borderColor.B};
    } else {
        borderCol = isLight
            ? winrt::Windows::UI::Color{borderAlpha, 0x00, 0x00, 0x00}
            : winrt::Windows::UI::Color{borderAlpha, 0xFF, 0xFF, 0xFF};
    }
    pill.BorderBrush(MakeBrush(borderCol));

    if (s.backgroundStyle == L"acrylic") {
        try {
            AcrylicBrush acrylic;
            acrylic.BackgroundSource(AcrylicBackgroundSource::HostBackdrop);
            acrylic.TintColor(s.backgroundColor);
            const double tintOp = std::clamp(static_cast<double>(s.backgroundOpacity) / 100.0, 0.05, 0.95);
            acrylic.TintOpacity(tintOp);
            acrylic.TintLuminosityOpacity(isLight ? 0.55 : 0.45);
            const BYTE fbAlpha = static_cast<BYTE>(std::clamp((s.backgroundOpacity * 255) / 100, 60, 235));
            acrylic.FallbackColor({fbAlpha, s.backgroundColor.R, s.backgroundColor.G, s.backgroundColor.B});
            pill.Background(acrylic);
        } catch (...) {
            const BYTE bgAlpha = static_cast<BYTE>((s.backgroundOpacity * 255) / 100);
            pill.Background(MakeBrush({bgAlpha, s.backgroundColor.R, s.backgroundColor.G, s.backgroundColor.B}));
        }
    } else if (s.backgroundStyle == L"solid") {
        const BYTE bgAlpha = static_cast<BYTE>((s.backgroundOpacity * 255) / 100);
        pill.Background(MakeBrush({bgAlpha, s.backgroundColor.R, s.backgroundColor.G, s.backgroundColor.B}));
    } else {
        pill.Background(MakeBrush({0x00, 0x00, 0x00, 0x00}));
    }

    if (s.enableShadow) {
        try {
            ThemeShadow shadow;
            pill.Shadow(shadow);
            pill.Translation({0.f, 0.f, 16.f});
        } catch (...) {}
    } else {
        try {
            pill.Shadow(nullptr);
            pill.Translation({0.f, 0.f, 0.f});
        } catch (...) {}
    }
}

void ClearSurfaceStyle(Border const& pill) {
    if (!pill) return;
    pill.Background(MakeBrush({0x00, 0x00, 0x00, 0x00}));
    pill.BorderThickness({0, 0, 0, 0});
    pill.BorderBrush(MakeBrush({0x00, 0x00, 0x00, 0x00}));
    pill.Padding({0, 0, 0, 0});
    try {
        pill.Shadow(nullptr);
        pill.Translation({0.f, 0.f, 0.f});
    } catch (...) {}
}

std::wstring_view GetSvgIconPathData(const std::wstring& iconKey) {
    if (iconKey == L"shield_check_solid") return kSvgShieldCheckSolid;
    if (iconKey == L"shield_exclamation_solid") return kSvgShieldExclamationSolid;
    if (iconKey == L"shield_solid") return kSvgShieldSolid;
    if (iconKey == L"shield") return kSvgShield;
    if (iconKey == L"shield_exclamation") return kSvgShieldExclamation;
    if (iconKey == L"globe") return kSvgGlobe;
    if (iconKey == L"world") return kSvgWorld;
    if (iconKey == L"cloud_check") return kSvgCloudCheck;
    if (iconKey == L"database") return kSvgDatabase;
    if (iconKey == L"physics") return kSvgPhysics;
    if (iconKey == L"exclamation") return kSvgExclamation;
    return kSvgShieldCheckSolid;
}

winrt::Windows::UI::Xaml::Shapes::Path CreateSvgIconPath(std::wstring_view pathData, double size) {
    try {
        std::wstring xaml =
            L"<Path xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
            L"Stretch=\"Uniform\" HorizontalAlignment=\"Center\" VerticalAlignment=\"Center\" "
            L"Data=\"F1 ";
        xaml.append(pathData);
        xaml.append(L"\" />");
        auto obj = winrt::Windows::UI::Xaml::Markup::XamlReader::Load(xaml);
        if (auto p = obj.try_as<winrt::Windows::UI::Xaml::Shapes::Path>()) {
            p.Width(size);
            p.Height(size);
            return p;
        }
    } catch (...) {
        Wh_Log(L"Failed to parse SVG path XAML");
    }
    return nullptr;
}

winrt::Windows::UI::Text::FontWeight GetConfiguredFontWeight(const std::wstring& w) {
    if (w == L"regular") return winrt::Windows::UI::Text::FontWeights::Normal();
    if (w == L"medium") return winrt::Windows::UI::Text::FontWeights::Medium();
    return winrt::Windows::UI::Text::FontWeights::SemiBold();
}

std::wstring BuildTooltipText(const IslandSettings& s, VpnState st) {
    const bool ru = IsRussianUi();
    std::wstring statusStr = ru ? L"Выключен" : L"Disconnected";
    if (st == VpnState::Connected) statusStr = ru ? L"Подключён" : L"Connected";
    else if (st == VpnState::Transitioning) statusStr = ru ? L"Переключение..." : L"Transitioning...";

    const std::wstring tunnelDisplay = s.tunnelName.empty()
        ? (ru ? L"не настроен" : L"not configured")
        : s.tunnelName;
    std::wstring tip = L"Usagi Taskbar Island — WireGuard (" + tunnelDisplay + L")\n" +
        (ru ? L"Статус: " : L"Status: ") + statusStr;
    if (s.splitEnabled) {
        int activeCount = 0;
        for (const auto& rule : s.excludedApps) {
            if (rule.enabled) ++activeCount;
        }
        tip += ru
            ? (L"\nSplit Tunneling: вкл (" + std::to_wstring(activeCount) + L" искл.)")
            : (L"\nSplit Tunneling: on (" + std::to_wstring(activeCount) + L" excluded)");
    } else {
        tip += ru ? L"\nSplit Tunneling: выкл" : L"\nSplit Tunneling: off";
    }
    if (ru) {
        tip += s.enableContextMenu
            ? L"\nЛКМ: вкл/выкл • ПКМ: меню"
            : L"\nЛКМ: вкл/выкл";
    } else {
        tip += s.enableContextMenu
            ? L"\nLeft-click: toggle • Right-click: menu"
            : L"\nLeft-click: toggle";
    }
    return tip;
}

Border MakeTransportButton(double size,
                           double cornerRadius,
                           bool filledIdle,
                           winrt::Windows::UI::Xaml::Shapes::Path const& primaryPath,
                           winrt::Windows::UI::Xaml::Shapes::Path const& secondaryPath,
                           MediaCommand cmd) {
    const bool lightAtCreate = IsSystemLightTheme();
    const winrt::Windows::UI::Color idleBg = filledIdle
        ? (lightAtCreate ? winrt::Windows::UI::Color{0x18, 0x00, 0x00, 0x00}
                         : winrt::Windows::UI::Color{0x1A, 0xFF, 0xFF, 0xFF})
        : winrt::Windows::UI::Color{0x00, 0xFF, 0xFF, 0xFF};
    const winrt::Windows::UI::Color idleBorder{0x00, 0xFF, 0xFF, 0xFF};

    Border btn;
    btn.Width(size);
    btn.Height(size);
    btn.CornerRadius({cornerRadius, cornerRadius, cornerRadius, cornerRadius});
    btn.Background(MakeBrush(idleBg));
    btn.BorderThickness({1, 1, 1, 1});
    btn.BorderBrush(MakeBrush(idleBorder));
    btn.VerticalAlignment(VerticalAlignment::Center);
    try {
        BrushTransition bt;
        bt.Duration(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(83)));
        btn.BackgroundTransition(bt);
    } catch (...) {}

    Grid iconHost;
    iconHost.VerticalAlignment(VerticalAlignment::Center);
    iconHost.HorizontalAlignment(HorizontalAlignment::Center);
    if (primaryPath) iconHost.Children().Append(primaryPath);
    if (secondaryPath) iconHost.Children().Append(secondaryPath);
    btn.Child(iconHost);

    btn.PointerEntered([btn, filledIdle](auto const&, auto const&) {
        if (g_unloading.load()) return;
        const bool light = IsSystemLightTheme();
        btn.Background(MakeBrush(light
            ? winrt::Windows::UI::Color{static_cast<BYTE>(filledIdle ? 0x2A : 0x99), 0xFF, 0xFF, 0xFF}
            : winrt::Windows::UI::Color{static_cast<BYTE>(filledIdle ? 0x2C : 0x18), 0xFF, 0xFF, 0xFF}));
        btn.BorderBrush(MakeBrush(light
            ? winrt::Windows::UI::Color{0x14, 0x00, 0x00, 0x00}
            : winrt::Windows::UI::Color{0x22, 0xFF, 0xFF, 0xFF}));
    });

    btn.PointerExited([btn, filledIdle, idleBorder](auto const&, auto const&) {
        if (g_unloading.load()) return;
        const bool light = IsSystemLightTheme();
        const winrt::Windows::UI::Color restoreBg = filledIdle
            ? (light ? winrt::Windows::UI::Color{0x18, 0x00, 0x00, 0x00}
                     : winrt::Windows::UI::Color{0x1A, 0xFF, 0xFF, 0xFF})
            : winrt::Windows::UI::Color{0x00, 0xFF, 0xFF, 0xFF};
        btn.Background(MakeBrush(restoreBg));
        btn.BorderBrush(MakeBrush(idleBorder));
    });

    btn.PointerPressed([btn](auto const& sender, Input::PointerRoutedEventArgs const& e) {
        if (g_unloading.load()) return;
        const auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
        const bool allowLeft = (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonPressed ||
                                kind == winrt::Windows::UI::Input::PointerUpdateKind::Other);
        if (!allowLeft) return;

        if (auto el = sender.template try_as<UIElement>()) {
            el.CapturePointer(e.Pointer());
        }
        const bool light = IsSystemLightTheme();
        btn.Background(MakeBrush(light
            ? winrt::Windows::UI::Color{0x55, 0xFF, 0xFF, 0xFF}
            : winrt::Windows::UI::Color{0x0C, 0xFF, 0xFF, 0xFF}));
        e.Handled(true);
    });

    btn.PointerReleased([btn, cmd, filledIdle](auto const& sender, Input::PointerRoutedEventArgs const& e) {
        if (g_unloading.load()) return;
        const auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
        const bool isLeft = (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased ||
                             kind == winrt::Windows::UI::Input::PointerUpdateKind::Other);

        bool inside = false;
        if (auto el = sender.template try_as<UIElement>()) {
            el.ReleasePointerCapture(e.Pointer());
            const auto pos = e.GetCurrentPoint(el).Position();
            const auto sz = el.RenderSize();
            inside = (pos.X >= 0 && pos.X <= sz.Width && pos.Y >= 0 && pos.Y <= sz.Height);
        }
        if (!isLeft) return;

        const bool light = IsSystemLightTheme();
        btn.Background(MakeBrush(light
            ? winrt::Windows::UI::Color{static_cast<BYTE>(filledIdle ? 0x2A : 0x99), 0xFF, 0xFF, 0xFF}
            : winrt::Windows::UI::Color{static_cast<BYTE>(filledIdle ? 0x2C : 0x18), 0xFF, 0xFF, 0xFF}));

        if (inside) {
            RequestMediaCommand(cmd);
        }
        e.Handled(true);
    });

    return btn;
}

bool IsMediaHoverLight(const std::wstring& mode) {
    if (mode == L"light") return true;
    if (mode == L"dark") return false;
    return IsSystemLightTheme();
}

Brush MakeElevationBorderBrush(bool light) {
    const winrt::Windows::UI::Color topCol = light
        ? winrt::Windows::UI::Color{0x08, 0x00, 0x00, 0x00}
        : winrt::Windows::UI::Color{0x28, 0xFF, 0xFF, 0xFF};
    const winrt::Windows::UI::Color botCol = light
        ? winrt::Windows::UI::Color{0x10, 0x00, 0x00, 0x00}
        : winrt::Windows::UI::Color{0x0A, 0xFF, 0xFF, 0xFF};
    try {
        LinearGradientBrush lgb;
        lgb.StartPoint({0.5f, 0.0f});
        lgb.EndPoint({0.5f, 1.0f});
        GradientStop s1, s2;
        s1.Color(topCol);
        s1.Offset(0.0);
        s2.Color(botCol);
        s2.Offset(1.0);
        lgb.GradientStops().Append(s1);
        lgb.GradientStops().Append(s2);
        return lgb;
    } catch (...) {
        return MakeBrush(topCol);
    }
}

void UpdateMediaHoverVisual(MediaModuleUI& media, const IslandSettings& s, HWND hostHwnd) {
    if (!media.hoverPlate) return;
    const winrt::Windows::UI::Color transparent{0x00, 0xFF, 0xFF, 0xFF};
    if (s.mediaHoverEffect == L"off") {
        media.hoverPlate.Background(MakeBrush(transparent));
        media.hoverPlate.BorderBrush(MakeBrush(transparent));
        return;
    }

    const bool flyoutActive = g_miniPlayerOpen.load() && (hostHwnd == g_miniPlayerAnchorHwnd);
    const bool hovered = media.isHovered || flyoutActive;
    const bool pressed = media.isPressed;
    const bool light = IsMediaHoverLight(s.mediaHoverEffect);

    if (pressed) {
        media.hoverPlate.Background(MakeBrush(light
            ? winrt::Windows::UI::Color{0x4D, 0xFF, 0xFF, 0xFF}
            : winrt::Windows::UI::Color{0x0A, 0xFF, 0xFF, 0xFF}));
        media.hoverPlate.BorderBrush(MakeBrush(light
            ? winrt::Windows::UI::Color{0x05, 0x00, 0x00, 0x00}
            : winrt::Windows::UI::Color{0x0A, 0xFF, 0xFF, 0xFF}));
    } else if (hovered) {
        media.hoverPlate.Background(MakeBrush(light
            ? winrt::Windows::UI::Color{0x99, 0xFF, 0xFF, 0xFF}
            : winrt::Windows::UI::Color{0x0F, 0xFF, 0xFF, 0xFF}));
        media.hoverPlate.BorderBrush(MakeElevationBorderBrush(light));
    } else {
        media.hoverPlate.Background(MakeBrush(transparent));
        media.hoverPlate.BorderBrush(MakeBrush(transparent));
    }
}

void RefreshMediaHoverForHwnd(HWND hWnd) {
    if (!hWnd || g_unloading.load()) return;
    if (IslandHost* h = FindHostByHwnd(hWnd)) {
        UpdateMediaHoverVisual(h->media, GetSettingsSnapshot(), hWnd);
    }
}

void CloseMiniPlayerOnUiThread() {
    HWND prevAnchor = g_miniPlayerAnchorHwnd;
    if (g_miniPlayer.flyout) {
        try { g_miniPlayer.flyout.Hide(); } catch (...) {}
    }
    g_miniPlayerOpen.store(false);
    g_miniPlayerAnchorHwnd = nullptr;
    g_miniPlayer.Reset();
    if (prevAnchor) {
        RefreshMediaHoverForHwnd(prevAnchor);
    }
}

void UpdateSeekBarVisuals(double ratio, int64_t durationTicks) {
    if (!g_miniPlayer.seekHitArea || !g_miniPlayer.seekTrackFill) return;
    ratio = std::clamp(ratio, 0.0, 1.0);
    double trackW = g_miniPlayer.seekHitArea.ActualWidth();
    if (trackW < 10.0) trackW = 264.0;
    const double fillW = ratio * trackW;
    g_miniPlayer.seekTrackFill.Width(fillW);
    if (g_miniPlayer.seekKnobTransform) {
        const double knobX = std::clamp(fillW - 5.0, 0.0, std::max(0.0, trackW - 10.0));
        g_miniPlayer.seekKnobTransform.X(knobX);
    }
    if (g_miniPlayer.posText) {
        g_miniPlayer.posText.Text(FormatTimeFromTicks(
            static_cast<int64_t>(ratio * static_cast<double>(durationTicks))));
    }
}

void RefreshMiniPlayerUI(const IslandSettings& s, const MediaSnapshot& media) {
    if (!g_miniPlayerOpen.load() || !g_miniPlayer.card || g_unloading.load()) return;

    try {
        const bool isLight = IsSystemLightTheme();
        const winrt::Windows::UI::Color titleCol = (s.mediaColorMode == L"custom")
            ? s.mediaTextColor
            : (isLight ? winrt::Windows::UI::Color{0xFF, 0x10, 0x10, 0x10}
                       : winrt::Windows::UI::Color{0xFF, 0xFF, 0xFF, 0xFF});
        const winrt::Windows::UI::Color artistCol = (s.mediaColorMode == L"custom")
            ? s.mediaArtistColor
            : titleCol;
        const winrt::Windows::UI::Color btnCol = (s.mediaColorMode == L"custom")
            ? s.mediaButtonColor
            : titleCol;
        const winrt::Windows::UI::Color accentCol = s.mediaAccentColor;

        // Session switcher pills (visible only when > 1 media session)
        if (g_miniPlayer.sessionBar) {
            if (media.sessions.size() > 1) {
                g_miniPlayer.sessionBar.Visibility(Visibility::Visible);
                size_t sig = media.sessions.size();
                for (const auto& sess : media.sessions) {
                    sig = sig * 131 + std::hash<std::wstring>{}(sess.aumid) + (sess.isPlaying ? 7 : 0);
                }
                sig = sig * 131 + std::hash<std::wstring>{}(media.activeAumid);
                if (sig != g_miniPlayer.lastSessionSig) {
                    g_miniPlayer.lastSessionSig = sig;
                    g_miniPlayer.sessionBar.Children().Clear();
                    for (const auto& sess : media.sessions) {
                        const bool isActive = (sess.aumid == media.activeAumid);
                        Border pill;
                        pill.CornerRadius({10, 10, 10, 10});
                        pill.Padding({10, 4, 10, 4});
                        pill.BorderThickness({1, 1, 1, 1});
                        if (isActive) {
                            pill.Background(MakeBrush({0x32, accentCol.R, accentCol.G, accentCol.B}));
                            pill.BorderBrush(MakeBrush({0xB0, accentCol.R, accentCol.G, accentCol.B}));
                        } else {
                            pill.Background(MakeBrush(isLight
                                ? winrt::Windows::UI::Color{0x12, 0x00, 0x00, 0x00}
                                : winrt::Windows::UI::Color{0x14, 0xFF, 0xFF, 0xFF}));
                            pill.BorderBrush(MakeBrush({0x00, 0xFF, 0xFF, 0xFF}));
                        }

                        TextBlock lbl;
                        lbl.Text(sess.label);
                        lbl.FontFamily(FontFamily(L"Segoe UI Variable Display"));
                        lbl.FontSize(11.0);
                        lbl.FontWeight(isActive
                            ? winrt::Windows::UI::Text::FontWeights::SemiBold()
                            : winrt::Windows::UI::Text::FontWeights::Normal());
                        lbl.Foreground(MakeBrush(titleCol));
                        lbl.Opacity(isActive ? 1.0 : 0.72);
                        pill.Child(lbl);

                        const std::wstring targetAumid = sess.aumid;
                        pill.PointerReleased([targetAumid](auto const&, Input::PointerRoutedEventArgs const& e) {
                            if (g_unloading.load()) return;
                            const auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
                            if (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased ||
                                kind == winrt::Windows::UI::Input::PointerUpdateKind::Other) {
                                RequestSessionSwitch(targetAumid);
                                e.Handled(true);
                            }
                        });
                        g_miniPlayer.sessionBar.Children().Append(pill);
                    }
                }
            } else {
                g_miniPlayer.sessionBar.Visibility(Visibility::Collapsed);
                g_miniPlayer.lastSessionSig = 0;
            }
        }

        // Cover art
        if (g_miniPlayer.artBackdrop) {
            g_miniPlayer.artBackdrop.Background(MakeBrush(isLight
                ? winrt::Windows::UI::Color{0x18, 0x00, 0x00, 0x00}
                : winrt::Windows::UI::Color{0x1C, 0xFF, 0xFF, 0xFF}));
        }
        if (g_miniPlayer.fallbackNote) {
            g_miniPlayer.fallbackNote.Fill(MakeBrush(titleCol));
            g_miniPlayer.fallbackNote.Opacity(0.55);
        }
        if (media.thumbnailHash != g_miniPlayer.lastThumbHash) {
            const double innerW = std::max(120.0, static_cast<double>(s.miniPlayerWidth - s.miniPlayerPadX * 2));
            const double maxH = static_cast<double>(s.miniPlayerArtMaxHeight);
            if (!media.thumbnailBytes.empty() && g_miniPlayer.artBrush && g_miniPlayer.artRect) {
                ApplyMiniPlayerArtToBrush(g_miniPlayer.artBrush, media.thumbnailBytes,
                                          s.miniPlayerAdaptiveArt, innerW, maxH, g_miniPlayer.artBox);
                g_miniPlayer.artRect.Visibility(Visibility::Visible);
                if (g_miniPlayer.fallbackNote) g_miniPlayer.fallbackNote.Visibility(Visibility::Collapsed);
            } else {
                if (g_miniPlayer.artBox) {
                    const double fb = std::min(innerW, std::min(maxH, 180.0));
                    g_miniPlayer.artBox.Width(fb);
                    g_miniPlayer.artBox.Height(fb);
                }
                if (g_miniPlayer.artBrush) g_miniPlayer.artBrush.ImageSource(nullptr);
                if (g_miniPlayer.artRect) g_miniPlayer.artRect.Visibility(Visibility::Collapsed);
                if (g_miniPlayer.fallbackNote) g_miniPlayer.fallbackNote.Visibility(Visibility::Visible);
            }
            g_miniPlayer.lastThumbHash = media.thumbnailHash;
        }

        // Title & artist
        if (g_miniPlayer.titleBlock) {
            const bool ru = IsRussianUi();
            g_miniPlayer.titleBlock.Text(media.hasMedia
                ? (media.title.empty() ? (ru ? L"Без названия" : L"Untitled") : media.title)
                : (ru ? L"Нет активного медиа" : L"No active media"));
            g_miniPlayer.titleBlock.Foreground(MakeBrush(titleCol));
        }
        if (g_miniPlayer.artistBlock) {
            g_miniPlayer.artistBlock.Text(media.artist.empty() ? L" " : media.artist);
            g_miniPlayer.artistBlock.Foreground(MakeBrush(artistCol));
            if (s.mediaColorMode != L"custom") g_miniPlayer.artistBlock.Opacity(0.72);
            else g_miniPlayer.artistBlock.Opacity(1.0);
        }

        // Like / Library button
        if (g_miniPlayer.likeBtn) {
            g_miniPlayer.likeBtn.Visibility(media.hasMedia ? Visibility::Visible : Visibility::Collapsed);
        }
        if (g_miniPlayer.likeAddPath && g_miniPlayer.likeCheckPath) {
            g_miniPlayer.likeAddPath.Visibility(media.inLibrary ? Visibility::Collapsed : Visibility::Visible);
            g_miniPlayer.likeCheckPath.Visibility(media.inLibrary ? Visibility::Visible : Visibility::Collapsed);
            g_miniPlayer.likeAddPath.Fill(MakeBrush(btnCol));
            g_miniPlayer.likeAddPath.Opacity(0.80);
            g_miniPlayer.likeCheckPath.Fill(MakeBrush(accentCol));
        }

        // Seek bar & timestamps
        if (g_miniPlayer.seekTrackBg) {
            g_miniPlayer.seekTrackBg.Background(MakeBrush(isLight
                ? winrt::Windows::UI::Color{0x24, 0x00, 0x00, 0x00}
                : winrt::Windows::UI::Color{0x28, 0xFF, 0xFF, 0xFF}));
        }
        if (g_miniPlayer.seekTrackFill) {
            g_miniPlayer.seekTrackFill.Background(MakeBrush(accentCol));
        }
        if (g_miniPlayer.seekKnob) {
            g_miniPlayer.seekKnob.Fill(MakeBrush(accentCol));
        }
        if (g_miniPlayer.posText) g_miniPlayer.posText.Foreground(MakeBrush(titleCol));
        if (g_miniPlayer.durText) {
            g_miniPlayer.durText.Foreground(MakeBrush(titleCol));
            g_miniPlayer.durText.Text(media.durationTicks > 0
                ? FormatTimeFromTicks(media.durationTicks)
                : L"0:00");
        }
        if (!g_miniPlayer.isSeeking) {
            const double ratio = (media.durationTicks > 0)
                ? std::clamp(static_cast<double>(media.positionTicks) /
                             static_cast<double>(media.durationTicks), 0.0, 1.0)
                : 0.0;
            UpdateSeekBarVisuals(ratio, media.durationTicks);
        }

        // 5 Transport buttons
        auto brushBtn = MakeBrush(btnCol);
        auto brushAcc = MakeBrush(accentCol);
        if (g_miniPlayer.prevPath) g_miniPlayer.prevPath.Fill(brushBtn);
        if (g_miniPlayer.nextPath) g_miniPlayer.nextPath.Fill(brushBtn);
        if (g_miniPlayer.playPath) {
            g_miniPlayer.playPath.Fill(brushBtn);
            g_miniPlayer.playPath.Visibility(media.isPlaying ? Visibility::Collapsed : Visibility::Visible);
        }
        if (g_miniPlayer.pausePath) {
            g_miniPlayer.pausePath.Fill(brushBtn);
            g_miniPlayer.pausePath.Visibility(media.isPlaying ? Visibility::Visible : Visibility::Collapsed);
        }
        if (g_miniPlayer.shufflePath) {
            g_miniPlayer.shufflePath.Fill(media.shuffleEnabled ? brushAcc : brushBtn);
            g_miniPlayer.shufflePath.Opacity(media.shuffleEnabled ? 1.0 : 0.45);
        }
        if (g_miniPlayer.repeatAllPath && g_miniPlayer.repeatOnePath) {
            const bool repOn = (media.repeatMode != RepeatMode::Off);
            const bool repOne = (media.repeatMode == RepeatMode::One);
            g_miniPlayer.repeatAllPath.Visibility(repOne ? Visibility::Collapsed : Visibility::Visible);
            g_miniPlayer.repeatOnePath.Visibility(repOne ? Visibility::Visible : Visibility::Collapsed);
            auto& activeRep = repOne ? g_miniPlayer.repeatOnePath : g_miniPlayer.repeatAllPath;
            activeRep.Fill(repOn ? brushAcc : brushBtn);
            activeRep.Opacity(repOn ? 1.0 : 0.45);
        }
    } catch (...) {}
}

void ShowMiniPlayerFlyout(HWND anchorHwnd, FrameworkElement const& target) {
    if (!target || g_unloading.load()) return;

    if (g_miniPlayerOpen.load()) {
        CloseMiniPlayerOnUiThread();
        return;
    }

    try {
        const IslandSettings s = GetSettingsSnapshot();
        const MediaSnapshot media = GetMediaSnapshot();
        const bool isLight = IsSystemLightTheme();

        const double cardWidth = static_cast<double>(s.miniPlayerWidth);
        const double cardRadius = static_cast<double>(s.miniPlayerCornerRadius);

        Controls::Flyout flyout;
        Style flyoutStyle(winrt::xaml_typename<Controls::FlyoutPresenter>());
        flyoutStyle.Setters().Append(Setter(
            Controls::Control::BackgroundProperty(),
            winrt::box_value(MakeBrush({0x00, 0x00, 0x00, 0x00}))));
        flyoutStyle.Setters().Append(Setter(
            FrameworkElement::MarginProperty(),
            winrt::box_value(Thickness{0, 0, 0, 0})));
        flyoutStyle.Setters().Append(Setter(
            Controls::Control::PaddingProperty(),
            winrt::box_value(Thickness{0, 0, 0, 0})));
        flyoutStyle.Setters().Append(Setter(
            Controls::Control::BorderThicknessProperty(),
            winrt::box_value(Thickness{0, 0, 0, 0})));
        flyoutStyle.Setters().Append(Setter(
            FrameworkElement::MinWidthProperty(),
            winrt::box_value(cardWidth + 24.0)));
        flyoutStyle.Setters().Append(Setter(
            FrameworkElement::MaxWidthProperty(),
            winrt::box_value(cardWidth + 48.0)));
        flyoutStyle.Setters().Append(Setter(
            FrameworkElement::MaxHeightProperty(),
            winrt::box_value(10000.0)));
        flyout.FlyoutPresenterStyle(flyoutStyle);

        Grid outerHost;

        Border card;
        card.Width(cardWidth);
        card.CornerRadius({cardRadius, cardRadius, cardRadius, cardRadius});
        card.Padding({
            static_cast<double>(s.miniPlayerPadX), static_cast<double>(s.miniPlayerPadY),
            static_cast<double>(s.miniPlayerPadX), static_cast<double>(s.miniPlayerPadY)});
        card.Margin({12, 12, 12, 12});

        if (s.miniPlayerShowBorder) {
            card.BorderThickness({1, 1, 1, 1});
            card.BorderBrush(MakeBrush(isLight
                ? winrt::Windows::UI::Color{0x1C, 0x00, 0x00, 0x00}
                : winrt::Windows::UI::Color{0x24, 0xFF, 0xFF, 0xFF}));
        } else {
            card.BorderThickness({0, 0, 0, 0});
            card.BorderBrush(nullptr);
        }

        const double tintRatio = std::clamp(static_cast<double>(s.miniPlayerTintOpacity) / 100.0, 0.05, 0.95);
        if (s.miniPlayerWindowStyle == L"solid") {
            const BYTE solidAlpha = static_cast<BYTE>(std::clamp((s.miniPlayerTintOpacity * 255) / 100, 180, 255));
            card.Background(MakeBrush(isLight
                ? winrt::Windows::UI::Color{solidAlpha, 0xF3, 0xF3, 0xF3}
                : winrt::Windows::UI::Color{solidAlpha, 0x1C, 0x1C, 0x1C}));
        } else {
            const bool isTranslucent = (s.miniPlayerWindowStyle != L"acrylic");
            try {
                AcrylicBrush acrylic;
                acrylic.BackgroundSource(AcrylicBackgroundSource::HostBackdrop);
                acrylic.TintColor(isLight
                    ? winrt::Windows::UI::Color{0xFF, 0xF5, 0xF5, 0xF5}
                    : winrt::Windows::UI::Color{0xFF, 0x14, 0x14, 0x14});
                acrylic.TintOpacity(isTranslucent ? tintRatio : std::max(0.42, tintRatio));
                acrylic.TintLuminosityOpacity(isTranslucent
                    ? (isLight ? 0.50 : 0.42)
                    : (isLight ? 0.62 : 0.48));
                acrylic.FallbackColor(isLight
                    ? winrt::Windows::UI::Color{0xCC, 0xF2, 0xF2, 0xF2}
                    : winrt::Windows::UI::Color{0xCC, 0x18, 0x18, 0x18});
                card.Background(acrylic);
            } catch (...) {
                card.Background(MakeBrush(isLight
                    ? winrt::Windows::UI::Color{0xCC, 0xF2, 0xF2, 0xF2}
                    : winrt::Windows::UI::Color{0xCC, 0x18, 0x18, 0x18}));
            }
        }

        try {
            ThemeShadow shadow;
            card.Shadow(shadow);
            card.Translation({0.f, 0.f, 32.f});
        } catch (...) {}

        StackPanel rootCol;
        rootCol.Orientation(Orientation::Vertical);
        rootCol.Spacing(static_cast<double>(s.miniPlayerElemSpacing));

        // Session switcher bar (optional)
        StackPanel sessionBar{nullptr};
        if (s.miniPlayerShowSessionBar) {
            sessionBar = StackPanel();
            sessionBar.Orientation(Orientation::Horizontal);
            sessionBar.HorizontalAlignment(HorizontalAlignment::Center);
            sessionBar.Spacing(6);
            sessionBar.Visibility(Visibility::Collapsed);
            rootCol.Children().Append(sessionBar);
        }

        // Large rounded cover art (optional, adaptive proportions)
        Grid artBox{nullptr};
        Border artBackdrop{nullptr};
        winrt::Windows::UI::Xaml::Shapes::Rectangle artRect{nullptr};
        ImageBrush artBrush{nullptr};
        winrt::Windows::UI::Xaml::Shapes::Path fallbackNote{nullptr};

        if (s.miniPlayerShowArt) {
            const double innerW = std::max(120.0, cardWidth - static_cast<double>(s.miniPlayerPadX * 2));
            const double maxH = static_cast<double>(s.miniPlayerArtMaxHeight);
            const double initSize = std::min(innerW, maxH);
            const double artCr = static_cast<double>(s.miniPlayerArtRadius);

            artBox = Grid();
            artBox.Width(initSize);
            artBox.Height(s.miniPlayerAdaptiveArt ? initSize : std::min(maxH, std::round(innerW * 0.65)));
            artBox.HorizontalAlignment(HorizontalAlignment::Center);

            artBackdrop = Border();
            artBackdrop.CornerRadius({artCr, artCr, artCr, artCr});
            artBackdrop.HorizontalAlignment(HorizontalAlignment::Stretch);
            artBackdrop.VerticalAlignment(VerticalAlignment::Stretch);
            fallbackNote = CreateSvgIconPath(kSvgMediaNote, 36.0);
            if (fallbackNote) {
                artBackdrop.Child(fallbackNote);
            }
            artBox.Children().Append(artBackdrop);

            artRect = winrt::Windows::UI::Xaml::Shapes::Rectangle();
            artRect.RadiusX(artCr);
            artRect.RadiusY(artCr);
            artRect.HorizontalAlignment(HorizontalAlignment::Stretch);
            artRect.VerticalAlignment(VerticalAlignment::Stretch);
            artRect.Visibility(Visibility::Collapsed);
            artBrush = ImageBrush();
            artBrush.Stretch(Stretch::UniformToFill);
            artRect.Fill(artBrush);
            artBox.Children().Append(artRect);

            rootCol.Children().Append(artBox);
        }

        // Track title/artist + optional Add to library button
        Grid infoGrid;
        ColumnDefinition colText;
        colText.Width({1.0, GridUnitType::Star});
        infoGrid.ColumnDefinitions().Append(colText);
        if (s.miniPlayerShowLike) {
            ColumnDefinition colLike;
            colLike.Width({1.0, GridUnitType::Auto});
            infoGrid.ColumnDefinitions().Append(colLike);
        }

        StackPanel textCol;
        textCol.VerticalAlignment(VerticalAlignment::Center);
        textCol.Spacing(3);
        textCol.Margin({0, 0, s.miniPlayerShowLike ? 8.0 : 0.0, 0});

        TextBlock titleBlock;
        titleBlock.FontFamily(FontFamily(L"Segoe UI Variable Display"));
        titleBlock.FontSize(14.5);
        titleBlock.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        titleBlock.TextTrimming(TextTrimming::CharacterEllipsis);
        titleBlock.TextWrapping(TextWrapping::NoWrap);
        textCol.Children().Append(titleBlock);

        TextBlock artistBlock{nullptr};
        if (s.miniPlayerShowArtist) {
            artistBlock = TextBlock();
            artistBlock.FontFamily(FontFamily(L"Segoe UI Variable Display"));
            artistBlock.FontSize(12.0);
            artistBlock.FontWeight(winrt::Windows::UI::Text::FontWeights::Normal());
            artistBlock.TextTrimming(TextTrimming::CharacterEllipsis);
            artistBlock.TextWrapping(TextWrapping::NoWrap);
            textCol.Children().Append(artistBlock);
        }

        Grid::SetColumn(textCol, 0);
        infoGrid.Children().Append(textCol);

        Border likeBtn{nullptr};
        winrt::Windows::UI::Xaml::Shapes::Path likeAddPath{nullptr};
        winrt::Windows::UI::Xaml::Shapes::Path likeCheckPath{nullptr};
        if (s.miniPlayerShowLike) {
            likeAddPath = CreateSvgIconPath(kSvgMediaAdd, 16.0);
            likeCheckPath = CreateSvgIconPath(kSvgMediaCheck, 16.0);
            if (likeCheckPath) likeCheckPath.Visibility(Visibility::Collapsed);
            likeBtn = MakeTransportButton(32.0, 8.0, false, likeAddPath, likeCheckPath, MediaCommand::ToggleLike);
            Grid::SetColumn(likeBtn, 1);
            infoGrid.Children().Append(likeBtn);
        }

        rootCol.Children().Append(infoGrid);

        // Seek bar + timestamps (optional)
        Grid seekHitArea{nullptr};
        Border seekTrackBg{nullptr};
        Border seekTrackFill{nullptr};
        winrt::Windows::UI::Xaml::Shapes::Ellipse seekKnob{nullptr};
        TranslateTransform knobTransform{nullptr};
        TextBlock posText{nullptr};
        TextBlock durText{nullptr};

        if (s.miniPlayerShowSeekBar) {
            StackPanel seekCol;
            seekCol.Spacing(2);

            seekHitArea = Grid();
            seekHitArea.Height(18);
            seekHitArea.Background(MakeBrush({0x00, 0x00, 0x00, 0x00}));
            seekHitArea.HorizontalAlignment(HorizontalAlignment::Stretch);

            seekTrackBg = Border();
            seekTrackBg.Height(4);
            seekTrackBg.CornerRadius({2, 2, 2, 2});
            seekTrackBg.VerticalAlignment(VerticalAlignment::Center);
            seekTrackBg.HorizontalAlignment(HorizontalAlignment::Stretch);
            seekHitArea.Children().Append(seekTrackBg);

            seekTrackFill = Border();
            seekTrackFill.Height(4);
            seekTrackFill.CornerRadius({2, 2, 2, 2});
            seekTrackFill.VerticalAlignment(VerticalAlignment::Center);
            seekTrackFill.HorizontalAlignment(HorizontalAlignment::Left);
            seekTrackFill.Width(0);
            seekHitArea.Children().Append(seekTrackFill);

            seekKnob = winrt::Windows::UI::Xaml::Shapes::Ellipse();
            seekKnob.Width(10);
            seekKnob.Height(10);
            seekKnob.VerticalAlignment(VerticalAlignment::Center);
            seekKnob.HorizontalAlignment(HorizontalAlignment::Left);
            knobTransform = TranslateTransform();
            seekKnob.RenderTransform(knobTransform);
            seekHitArea.Children().Append(seekKnob);

            seekHitArea.SizeChanged([](auto const&, auto const&) {
                if (!g_miniPlayerOpen.load() || g_unloading.load()) return;
                const MediaSnapshot m = GetMediaSnapshot();
                const double r = g_miniPlayer.isSeeking
                    ? g_miniPlayer.seekRatio
                    : (m.durationTicks > 0
                        ? static_cast<double>(m.positionTicks) / static_cast<double>(m.durationTicks)
                        : 0.0);
                UpdateSeekBarVisuals(r, m.durationTicks);
            });

            seekHitArea.PointerPressed([](auto const& sender, Input::PointerRoutedEventArgs const& e) {
                if (g_unloading.load()) return;
                const MediaSnapshot m = GetMediaSnapshot();
                if (m.durationTicks <= 0) return;
                if (auto el = sender.template try_as<UIElement>()) {
                    el.CapturePointer(e.Pointer());
                    const double w = el.RenderSize().Width;
                    if (w > 1.0) {
                        const double ratio = std::clamp(e.GetCurrentPoint(el).Position().X / w, 0.0, 1.0);
                        g_miniPlayer.isSeeking = true;
                        g_miniPlayer.seekRatio = ratio;
                        UpdateSeekBarVisuals(ratio, m.durationTicks);
                    }
                }
                e.Handled(true);
            });

            seekHitArea.PointerMoved([](auto const& sender, Input::PointerRoutedEventArgs const& e) {
                if (!g_miniPlayer.isSeeking || g_unloading.load()) return;
                const MediaSnapshot m = GetMediaSnapshot();
                if (auto el = sender.template try_as<UIElement>()) {
                    const double w = el.RenderSize().Width;
                    if (w > 1.0) {
                        const double ratio = std::clamp(e.GetCurrentPoint(el).Position().X / w, 0.0, 1.0);
                        g_miniPlayer.seekRatio = ratio;
                        UpdateSeekBarVisuals(ratio, m.durationTicks);
                    }
                }
                e.Handled(true);
            });

            seekHitArea.PointerReleased([](auto const& sender, Input::PointerRoutedEventArgs const& e) {
                if (!g_miniPlayer.isSeeking || g_unloading.load()) return;
                const MediaSnapshot m = GetMediaSnapshot();
                double ratio = g_miniPlayer.seekRatio;
                if (auto el = sender.template try_as<UIElement>()) {
                    el.ReleasePointerCapture(e.Pointer());
                    const double w = el.RenderSize().Width;
                    if (w > 1.0) {
                        ratio = std::clamp(e.GetCurrentPoint(el).Position().X / w, 0.0, 1.0);
                    }
                }
                g_miniPlayer.isSeeking = false;
                if (m.durationTicks > 0) {
                    RequestMediaSeek(static_cast<int64_t>(ratio * static_cast<double>(m.durationTicks)));
                }
                e.Handled(true);
            });

            seekCol.Children().Append(seekHitArea);

            Grid timeRow;
            posText = TextBlock();
            posText.FontFamily(FontFamily(L"Segoe UI Variable Display"));
            posText.FontSize(11.0);
            posText.HorizontalAlignment(HorizontalAlignment::Left);
            posText.Opacity(0.70);
            posText.Text(L"0:00");
            timeRow.Children().Append(posText);

            durText = TextBlock();
            durText.FontFamily(FontFamily(L"Segoe UI Variable Display"));
            durText.FontSize(11.0);
            durText.HorizontalAlignment(HorizontalAlignment::Right);
            durText.Opacity(0.70);
            durText.Text(L"0:00");
            timeRow.Children().Append(durText);
            seekCol.Children().Append(timeRow);

            rootCol.Children().Append(seekCol);
        }

        // Transport buttons (Shuffle / Prev / PlayPause / Next / Repeat)
        StackPanel controlsRow;
        controlsRow.Orientation(Orientation::Horizontal);
        controlsRow.HorizontalAlignment(HorizontalAlignment::Center);
        controlsRow.Spacing(8);

        winrt::Windows::UI::Xaml::Shapes::Path shufflePath{nullptr};
        Border shuffleBtn{nullptr};
        if (s.miniPlayerShowShuffle) {
            shufflePath = CreateSvgIconPath(kSvgMediaShuffle, 15.0);
            shuffleBtn = MakeTransportButton(34.0, 8.0, false, shufflePath, nullptr, MediaCommand::ToggleShuffle);
            controlsRow.Children().Append(shuffleBtn);
        }

        auto prevPath = CreateSvgIconPath(kSvgMediaPrev, 15.0);
        Border prevBtn = MakeTransportButton(38.0, 8.0, false, prevPath, nullptr, MediaCommand::Prev);
        controlsRow.Children().Append(prevBtn);

        auto playPath = CreateSvgIconPath(kSvgMediaPlay, 16.0);
        auto pausePath = CreateSvgIconPath(kSvgMediaPause, 16.0);
        if (pausePath) pausePath.Visibility(Visibility::Collapsed);
        Border playPauseBtn = MakeTransportButton(44.0, 22.0, true, playPath, pausePath, MediaCommand::PlayPause);
        controlsRow.Children().Append(playPauseBtn);

        auto nextPath = CreateSvgIconPath(kSvgMediaNext, 15.0);
        Border nextBtn = MakeTransportButton(38.0, 8.0, false, nextPath, nullptr, MediaCommand::Next);
        controlsRow.Children().Append(nextBtn);

        winrt::Windows::UI::Xaml::Shapes::Path repeatAllPath{nullptr};
        winrt::Windows::UI::Xaml::Shapes::Path repeatOnePath{nullptr};
        Border repeatBtn{nullptr};
        if (s.miniPlayerShowRepeat) {
            repeatAllPath = CreateSvgIconPath(kSvgMediaRepeat, 15.0);
            repeatOnePath = CreateSvgIconPath(kSvgMediaRepeatOne, 15.0);
            if (repeatOnePath) repeatOnePath.Visibility(Visibility::Collapsed);
            repeatBtn = MakeTransportButton(34.0, 8.0, false, repeatAllPath, repeatOnePath, MediaCommand::CycleRepeat);
            controlsRow.Children().Append(repeatBtn);
        }

        rootCol.Children().Append(controlsRow);

        card.Child(rootCol);
        outerHost.Children().Append(card);
        flyout.Content(outerHost);

        CompositeTransform animTransform;
        if (s.miniPlayerAnimation) {
            card.RenderTransform(animTransform);
            card.Opacity(0.0);
        }

        g_miniPlayer.Reset();
        g_miniPlayer.flyout = flyout;
        g_miniPlayer.card = card;
        g_miniPlayer.sessionBar = sessionBar;
        g_miniPlayer.artBox = artBox;
        g_miniPlayer.artBackdrop = artBackdrop;
        g_miniPlayer.artRect = artRect;
        g_miniPlayer.artBrush = artBrush;
        g_miniPlayer.fallbackNote = fallbackNote;
        g_miniPlayer.titleBlock = titleBlock;
        g_miniPlayer.artistBlock = artistBlock;
        g_miniPlayer.likeBtn = likeBtn;
        g_miniPlayer.likeAddPath = likeAddPath;
        g_miniPlayer.likeCheckPath = likeCheckPath;
        g_miniPlayer.seekHitArea = seekHitArea;
        g_miniPlayer.seekTrackBg = seekTrackBg;
        g_miniPlayer.seekTrackFill = seekTrackFill;
        g_miniPlayer.seekKnob = seekKnob;
        g_miniPlayer.seekKnobTransform = knobTransform;
        g_miniPlayer.posText = posText;
        g_miniPlayer.durText = durText;
        g_miniPlayer.shuffleBtn = shuffleBtn;
        g_miniPlayer.prevBtn = prevBtn;
        g_miniPlayer.playPauseBtn = playPauseBtn;
        g_miniPlayer.nextBtn = nextBtn;
        g_miniPlayer.repeatBtn = repeatBtn;
        g_miniPlayer.shufflePath = shufflePath;
        g_miniPlayer.prevPath = prevPath;
        g_miniPlayer.playPath = playPath;
        g_miniPlayer.pausePath = pausePath;
        g_miniPlayer.nextPath = nextPath;
        g_miniPlayer.repeatAllPath = repeatAllPath;
        g_miniPlayer.repeatOnePath = repeatOnePath;

        Controls::Primitives::FlyoutPlacementMode placementMode =
            Controls::Primitives::FlyoutPlacementMode::TopEdgeAlignedLeft;
        winrt::Windows::Foundation::Point anchorPoint{0.f, 0.f};
        FrameworkElement rootContent{nullptr};
        double animStartY = 14.0;

        try {
            auto xamlRoot = target.XamlRoot();
            if (xamlRoot) {
                rootContent = xamlRoot.Content().try_as<FrameworkElement>();
            }

            bool hasWorkArea = false;
            double workLeft = 0.0, workTop = 0.0, workRight = 0.0, workBottom = 0.0;
            POINT originPx{0, 0};
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);

            if (anchorHwnd && IsWindow(anchorHwnd)) {
                HMONITOR mon = MonitorFromWindow(anchorHwnd, MONITOR_DEFAULTTONEAREST);
                if (mon && GetMonitorInfoW(mon, &mi) && ClientToScreen(anchorHwnd, &originPx)) {
                    double scale = 1.0;
                    try {
                        if (xamlRoot) scale = xamlRoot.RasterizationScale();
                    } catch (...) {}
                    if (scale <= 0.0) scale = 1.0;

                    workLeft   = static_cast<double>(mi.rcWork.left   - originPx.x) / scale;
                    workTop    = static_cast<double>(mi.rcWork.top    - originPx.y) / scale;
                    workRight  = static_cast<double>(mi.rcWork.right  - originPx.x) / scale;
                    workBottom = static_cast<double>(mi.rcWork.bottom - originPx.y) / scale;
                    hasWorkArea = (workRight > workLeft && workBottom > workTop);
                }
            }

            using FPM = Controls::Primitives::FlyoutPlacementMode;

            if (s.miniPlayerPlacement == L"screen" && hasWorkArea) {
                const bool hLeft  = (s.miniPlayerHAlign == L"left");
                const bool hRight = (s.miniPlayerHAlign == L"right");
                const bool vTop   = (s.miniPlayerVAlign == L"top");
                const bool vBot   = (s.miniPlayerVAlign == L"bottom");

                const double anchorX = hLeft
                    ? workLeft + static_cast<double>(s.miniPlayerEdgeOffsetX) - 12.0
                    : (hRight
                        ? workRight - static_cast<double>(s.miniPlayerEdgeOffsetX) + 12.0
                        : (workLeft + workRight) * 0.5);
                const double anchorY = vTop
                    ? workTop + static_cast<double>(s.miniPlayerEdgeOffsetY) - 12.0
                    : (vBot
                        ? workBottom - static_cast<double>(s.miniPlayerEdgeOffsetY) + 12.0
                        : (workTop + workBottom) * 0.5);

                if (vBot) {
                    placementMode = hLeft ? FPM::TopEdgeAlignedLeft
                                  : (hRight ? FPM::TopEdgeAlignedRight : FPM::Top);
                    animStartY = 14.0;
                } else if (vTop) {
                    placementMode = hLeft ? FPM::BottomEdgeAlignedLeft
                                  : (hRight ? FPM::BottomEdgeAlignedRight : FPM::Bottom);
                    animStartY = -14.0;
                } else {
                    placementMode = hLeft ? FPM::Right : (hRight ? FPM::Left : FPM::Top);
                    animStartY = 10.0;
                }
                anchorPoint = {static_cast<float>(anchorX), static_cast<float>(anchorY)};
            } else if (rootContent) {
                auto xform = target.TransformToVisual(rootContent);
                auto pt = xform.TransformPoint({0.f, 0.f});
                const double targetW = std::max(1.0, target.ActualWidth());
                const double centerX = static_cast<double>(pt.X) + targetW * 0.5;
                const double totalWidth = cardWidth + 24.0;
                const double pad = static_cast<double>(std::max(0, s.miniPlayerEdgeOffsetX));
                const double vGap = static_cast<double>(std::max(4, s.miniPlayerEdgeOffsetY));

                double leftEdge = centerX - totalWidth * 0.5;
                if (hasWorkArea) {
                    leftEdge = std::clamp(
                        leftEdge,
                        workLeft + pad - 12.0,
                        std::max(workLeft + pad - 12.0, workRight - pad - totalWidth + 12.0));
                } else {
                    leftEdge = std::max(0.0, leftEdge);
                }

                const bool taskbarAtTop = hasWorkArea && (mi.rcWork.top > originPx.y + 10);
                double anchorY = 0.0;
                if (taskbarAtTop) {
                    placementMode = FPM::BottomEdgeAlignedLeft;
                    anchorY = workTop + vGap - 12.0;
                    animStartY = -14.0;
                } else {
                    placementMode = FPM::TopEdgeAlignedLeft;
                    anchorY = hasWorkArea
                        ? std::min(workBottom - vGap + 12.0, static_cast<double>(pt.Y) + 6.0)
                        : (static_cast<double>(pt.Y) + 6.0);
                    animStartY = 14.0;
                }
                anchorPoint = {static_cast<float>(leftEdge), static_cast<float>(anchorY)};
            }
        } catch (...) {}

        if (s.miniPlayerAnimation && animTransform) {
            try { animTransform.TranslateY(animStartY); } catch (...) {}
        }

        const bool enableOpenAnim = s.miniPlayerAnimation;
        flyout.Opened([card, animTransform, enableOpenAnim, animStartY](auto const&, auto const&) {
            try {
                auto node = card.template try_as<DependencyObject>();
                for (int i = 0; i < 12 && node; ++i) {
                    if (auto p = node.template try_as<Controls::FlyoutPresenter>()) {
                        const int n = VisualTreeHelper::GetChildrenCount(p);
                        for (int j = 0; j < n; ++j) {
                            if (auto b = VisualTreeHelper::GetChild(p, j).template try_as<Border>()) {
                                b.Shadow(nullptr);
                                break;
                            }
                        }
                        break;
                    }
                    node = VisualTreeHelper::GetParent(node);
                }
            } catch (...) {}

            if (!g_unloading.load()) {
                RefreshMiniPlayerUI(GetSettingsSnapshot(), GetMediaSnapshot());
            }

            if (enableOpenAnim && animTransform) {
                try {
                    animTransform.TranslateY(animStartY);
                    card.Opacity(0.0);

                    Storyboard sb;
                    const auto dur = DurationHelper::FromTimeSpan(std::chrono::milliseconds(190));

                    DoubleAnimation slideAnim;
                    slideAnim.From(animStartY);
                    slideAnim.To(0.0);
                    slideAnim.Duration(dur);
                    CircleEase ease;
                    ease.EasingMode(EasingMode::EaseOut);
                    slideAnim.EasingFunction(ease);
                    Storyboard::SetTarget(slideAnim, card);
                    Storyboard::SetTargetProperty(
                        slideAnim, L"(UIElement.RenderTransform).(CompositeTransform.TranslateY)");
                    sb.Children().Append(slideAnim);

                    DoubleAnimation fadeAnim;
                    fadeAnim.From(0.0);
                    fadeAnim.To(1.0);
                    fadeAnim.Duration(dur);
                    Storyboard::SetTarget(fadeAnim, card);
                    Storyboard::SetTargetProperty(fadeAnim, L"Opacity");
                    sb.Children().Append(fadeAnim);

                    sb.Begin();
                } catch (...) {
                    try {
                        card.Opacity(1.0);
                        animTransform.TranslateY(0.0);
                    } catch (...) {}
                }
            } else {
                try { card.Opacity(1.0); } catch (...) {}
            }
        });

        flyout.Closed([](auto const&, auto const&) {
            HWND prevAnchor = g_miniPlayerAnchorHwnd;
            g_miniPlayerOpen.store(false);
            g_miniPlayerAnchorHwnd = nullptr;
            g_miniPlayer.Reset();
            if (prevAnchor) {
                RefreshMediaHoverForHwnd(prevAnchor);
            }
        });

        g_miniPlayerOpen.store(true);
        g_miniPlayerAnchorHwnd = anchorHwnd;
        RefreshMiniPlayerUI(s, media);
        RefreshMediaHoverForHwnd(anchorHwnd);

        try {
            if (rootContent) flyout.OverlayInputPassThroughElement(rootContent);
            flyout.ShouldConstrainToRootBounds(false);
            flyout.Placement(placementMode);
        } catch (...) {}

        Controls::Primitives::FlyoutShowOptions opts;
        opts.Placement(placementMode);
        if (rootContent) {
            opts.Position(anchorPoint);
            flyout.ShowAt(rootContent, opts);
        } else {
            flyout.ShowAt(target, opts);
        }
        if (g_mediaWakeEvent) SetEvent(g_mediaWakeEvent);
    } catch (...) {
        CloseMiniPlayerOnUiThread();
    }
}

void ShowMediaContextMenu(FrameworkElement const& target) {
    if (!target || g_unloading.load()) return;
    try {
        const bool ru = IsRussianUi();
        const MediaSnapshot media = GetMediaSnapshot();
        MenuFlyout menu;

        MenuFlyoutItem playItem;
        playItem.Text(media.isPlaying
            ? (ru ? L"Пауза" : L"Pause")
            : (ru ? L"Воспроизвести" : L"Play"));
        playItem.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::PlayPause); });
        menu.Items().Append(playItem);

        MenuFlyoutItem nextItem;
        nextItem.Text(ru ? L"Следующий трек" : L"Next track");
        nextItem.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::Next); });
        menu.Items().Append(nextItem);

        MenuFlyoutItem prevItem;
        prevItem.Text(ru ? L"Предыдущий трек" : L"Previous track");
        prevItem.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::Prev); });
        menu.Items().Append(prevItem);

        MenuFlyoutItem stopItem;
        stopItem.Text(ru ? L"Стоп" : L"Stop");
        stopItem.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::Stop); });
        menu.Items().Append(stopItem);

        menu.Items().Append(MenuFlyoutSeparator{});

        MenuFlyoutItem rwItem;
        rwItem.Text(ru ? L"Назад 5 сек" : L"Rewind 5s");
        rwItem.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::Rewind5); });
        menu.Items().Append(rwItem);

        MenuFlyoutItem fwItem;
        fwItem.Text(ru ? L"Вперёд 5 сек" : L"Forward 5s");
        fwItem.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::Forward5); });
        menu.Items().Append(fwItem);

        menu.Items().Append(MenuFlyoutSeparator{});

        ToggleMenuFlyoutItem shuffleItem;
        shuffleItem.Text(ru ? L"Перемешать (Shuffle)" : L"Shuffle");
        shuffleItem.IsChecked(media.shuffleEnabled);
        shuffleItem.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::ToggleShuffle); });
        menu.Items().Append(shuffleItem);

        MenuFlyoutSubItem repeatSub;
        repeatSub.Text(ru ? L"Повтор (Repeat)" : L"Repeat");

        ToggleMenuFlyoutItem repOff;
        repOff.Text(ru ? L"Выкл" : L"Off");
        repOff.IsChecked(media.repeatMode == RepeatMode::Off);
        repOff.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::SetRepeatOff); });
        repeatSub.Items().Append(repOff);

        ToggleMenuFlyoutItem repAll;
        repAll.Text(ru ? L"Плейлист" : L"Playlist");
        repAll.IsChecked(media.repeatMode == RepeatMode::All);
        repAll.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::SetRepeatAll); });
        repeatSub.Items().Append(repAll);

        ToggleMenuFlyoutItem repOne;
        repOne.Text(ru ? L"Один трек" : L"One track");
        repOne.IsChecked(media.repeatMode == RepeatMode::One);
        repOne.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::SetRepeatOne); });
        repeatSub.Items().Append(repOne);

        menu.Items().Append(repeatSub);

        ToggleMenuFlyoutItem likeItem;
        likeItem.Text(media.inLibrary
            ? (ru ? L"Убрать из библиотеки ВК" : L"Remove from VK Library")
            : (ru ? L"Добавить в библиотеку ВК" : L"Add to VK Library"));
        likeItem.IsChecked(media.inLibrary);
        likeItem.Click([](auto const&, auto const&) { RequestMediaCommand(MediaCommand::ToggleLike); });
        menu.Items().Append(likeItem);

        if (media.sessions.size() > 1) {
            menu.Items().Append(MenuFlyoutSeparator{});
            MenuFlyoutSubItem srcSub;
            srcSub.Text(ru ? L"Источник медиа" : L"Media source");
            for (const auto& sess : media.sessions) {
                ToggleMenuFlyoutItem srcItem;
                srcItem.Text(sess.isPlaying ? (sess.label + L" ▶") : sess.label);
                srcItem.IsChecked(sess.aumid == media.activeAumid);
                std::wstring aumid = sess.aumid;
                srcItem.Click([aumid](auto const&, auto const&) { RequestSessionSwitch(aumid); });
                srcSub.Items().Append(srcItem);
            }
            menu.Items().Append(srcSub);
        }

        try { menu.ShouldConstrainToRootBounds(false); } catch (...) {}
        Controls::Primitives::FlyoutShowOptions opts;
        opts.Placement(Controls::Primitives::FlyoutPlacementMode::Top);
        menu.ShowAt(target, opts);
    } catch (...) {}
}

void RefreshHostUI(IslandHost& host, const IslandSettings& s) {
    if (!host.islandRoot || g_unloading.load()) return;

    try {
        const VpnState st = g_vpnState.load();
        const VpnState textState = (st == VpnState::Transitioning)
            ? g_lastStableState.load()
            : st;
        const bool isConnected = (textState == VpnState::Connected);
        const bool isLight = IsSystemLightTheme();

        winrt::Windows::UI::Color fgColor;
        if (s.textColorMode == L"custom") {
            fgColor = isConnected ? s.textColorConnected : s.textColorDisconnected;
        } else {
            fgColor = isLight
                ? winrt::Windows::UI::Color{0xFF, 0x10, 0x10, 0x10}
                : winrt::Windows::UI::Color{0xFF, 0xFF, 0xFF, 0xFF};
        }

        winrt::Windows::UI::Color dotColor;
        if (st == VpnState::Connected) {
            dotColor = s.colorConnected;
        } else if (st == VpnState::Transitioning) {
            dotColor = s.colorTransitioning;
        } else {
            dotColor = s.colorDisconnected;
        }

        if (s.enableVpnButton && host.vpn.slot) {
            if (host.vpn.statusDot) {
                host.vpn.statusDot.Fill(MakeBrush(dotColor));
            }

            winrt::Windows::UI::Color iconColor;
            double iconOpacity = 1.0;
            if (st == VpnState::Connected) {
                iconColor = s.hasIconColorConnected ? s.iconColorConnected : s.colorConnected;
                iconOpacity = 1.0;
            } else if (st == VpnState::Transitioning) {
                iconColor = s.hasIconColorTransitioning ? s.iconColorTransitioning : s.colorTransitioning;
                iconOpacity = s.hasIconColorTransitioning ? 1.0 : 0.95;
            } else {
                iconColor = s.hasIconColorDisconnected ? s.iconColorDisconnected : fgColor;
                iconOpacity = s.hasIconColorDisconnected ? 1.0 : 0.80;
            }

            if (host.vpn.iconPathConnected) {
                auto iconBrush = MakeBrush(iconColor);
                if (host.vpn.iconPathDisconnected) {
                    if (isConnected) {
                        host.vpn.iconPathConnected.Visibility(Visibility::Visible);
                        host.vpn.iconPathConnected.Fill(iconBrush);
                        host.vpn.iconPathConnected.Opacity(iconOpacity);
                        host.vpn.iconPathDisconnected.Visibility(Visibility::Collapsed);
                    } else {
                        host.vpn.iconPathConnected.Visibility(Visibility::Collapsed);
                        host.vpn.iconPathDisconnected.Visibility(Visibility::Visible);
                        host.vpn.iconPathDisconnected.Fill(iconBrush);
                        host.vpn.iconPathDisconnected.Opacity(iconOpacity);
                    }
                } else {
                    host.vpn.iconPathConnected.Visibility(Visibility::Visible);
                    host.vpn.iconPathConnected.Fill(iconBrush);
                    host.vpn.iconPathConnected.Opacity(iconOpacity);
                }
            }

            if (host.vpn.textBlock) {
                host.vpn.textBlock.Text(isConnected ? s.textConnected : s.textDisconnected);
                host.vpn.textBlock.Foreground(MakeBrush(fgColor));
                host.vpn.textBlock.Opacity((s.textColorMode == L"custom" || isConnected) ? 1.0 : 0.80);
            }

            if (s.showTooltip) {
                ToolTipService::SetToolTip(host.vpn.slot, winrt::box_value(BuildTooltipText(s, st)));
            } else {
                ToolTipService::SetToolTip(host.vpn.slot, nullptr);
            }
        }

        const MediaSnapshot media = GetMediaSnapshot();
        const bool showMediaNow = s.enableMediaPlayer && media.hasMedia;
        if (s.enableMediaPlayer && host.media.slot) {
            host.media.slot.Visibility(showMediaNow ? Visibility::Visible : Visibility::Collapsed);
            if (showMediaNow) {
                const winrt::Windows::UI::Color mediaTitleCol = (s.mediaColorMode == L"custom")
                    ? s.mediaTextColor
                    : (isLight ? winrt::Windows::UI::Color{0xFF, 0x10, 0x10, 0x10}
                               : winrt::Windows::UI::Color{0xFF, 0xFF, 0xFF, 0xFF});
                const winrt::Windows::UI::Color mediaArtistCol = (s.mediaColorMode == L"custom")
                    ? s.mediaArtistColor
                    : mediaTitleCol;
                const winrt::Windows::UI::Color mediaBtnCol = (s.mediaColorMode == L"custom")
                    ? s.mediaButtonColor
                    : mediaTitleCol;
                const winrt::Windows::UI::Color artBg = isLight
                    ? winrt::Windows::UI::Color{0x18, 0x00, 0x00, 0x00}
                    : winrt::Windows::UI::Color{0x1A, 0xFF, 0xFF, 0xFF};

                if (host.media.artBackdrop) {
                    host.media.artBackdrop.Background(MakeBrush(artBg));
                }
                if (host.media.fallbackNote) {
                    host.media.fallbackNote.Fill(MakeBrush(mediaTitleCol));
                    host.media.fallbackNote.Opacity(0.65);
                }

                if (host.media.titleLine.textBlock) {
                    if (host.media.titleLine.textBlock.Text() != media.title) {
                        host.media.titleLine.textBlock.Text(media.title);
                        host.media.titleLine.offset = 0.0;
                        host.media.titleLine.forward = true;
                        host.media.titleLine.pauseTicks = 45;
                        if (host.media.titleLine.transform) host.media.titleLine.transform.X(0.0);
                    }
                    host.media.titleLine.textBlock.Foreground(MakeBrush(mediaTitleCol));
                }

                if (host.media.artistLine.textBlock) {
                    if (host.media.artistLine.textBlock.Text() != media.artist) {
                        host.media.artistLine.textBlock.Text(media.artist);
                        host.media.artistLine.offset = 0.0;
                        host.media.artistLine.forward = true;
                        host.media.artistLine.pauseTicks = 45;
                        if (host.media.artistLine.transform) host.media.artistLine.transform.X(0.0);
                    }
                    host.media.artistLine.textBlock.Foreground(MakeBrush(mediaArtistCol));
                    host.media.artistLine.textBlock.Opacity(s.mediaColorMode == L"custom" ? 1.0 : 0.72);
                    host.media.artistLine.textBlock.Visibility(
                        media.artist.empty() ? Visibility::Collapsed : Visibility::Visible);
                }

                auto btnBrush = MakeBrush(mediaBtnCol);
                auto accBrush = MakeBrush(s.mediaAccentColor);
                if (host.media.prevPath) host.media.prevPath.Fill(btnBrush);
                if (host.media.nextPath) host.media.nextPath.Fill(btnBrush);
                if (host.media.playPath) {
                    host.media.playPath.Fill(btnBrush);
                    host.media.playPath.Visibility(media.isPlaying ? Visibility::Collapsed : Visibility::Visible);
                }
                if (host.media.pausePath) {
                    host.media.pausePath.Fill(btnBrush);
                    host.media.pausePath.Visibility(media.isPlaying ? Visibility::Visible : Visibility::Collapsed);
                }
                if (host.media.shufflePath) {
                    host.media.shufflePath.Fill(media.shuffleEnabled ? accBrush : btnBrush);
                    host.media.shufflePath.Opacity(media.shuffleEnabled ? 1.0 : 0.45);
                }
                if (host.media.repeatAllPath && host.media.repeatOnePath) {
                    const bool repOn = (media.repeatMode != RepeatMode::Off);
                    const bool repOne = (media.repeatMode == RepeatMode::One);
                    host.media.repeatAllPath.Visibility(repOne ? Visibility::Collapsed : Visibility::Visible);
                    host.media.repeatOnePath.Visibility(repOne ? Visibility::Visible : Visibility::Collapsed);
                    auto& activeRep = repOne ? host.media.repeatOnePath : host.media.repeatAllPath;
                    activeRep.Fill(repOn ? accBrush : btnBrush);
                    activeRep.Opacity(repOn ? 1.0 : 0.45);
                }
                if (host.media.addPath && host.media.checkPath) {
                    host.media.addPath.Visibility(media.inLibrary ? Visibility::Collapsed : Visibility::Visible);
                    host.media.checkPath.Visibility(media.inLibrary ? Visibility::Visible : Visibility::Collapsed);
                    host.media.addPath.Fill(btnBrush);
                    host.media.addPath.Opacity(0.75);
                    host.media.checkPath.Fill(accBrush);
                }

                if (s.mediaShowAlbumArt && media.thumbnailHash != host.media.lastThumbHash) {
                    if (!media.thumbnailBytes.empty() && host.media.artBrush && host.media.artRect) {
                        const double artH = static_cast<double>(s.mediaArtSize);
                        ApplyAlbumArtToBrush(
                            host.media.artBrush, media.thumbnailBytes, 64,
                            s.mediaAdaptiveArtWidth, artH, static_cast<double>(s.mediaArtMaxWidth),
                            host.media.artBox, host.media.artBackdrop, host.media.artRect);
                        host.media.artRect.Visibility(Visibility::Visible);
                        if (host.media.fallbackNote) {
                            host.media.fallbackNote.Visibility(Visibility::Collapsed);
                        }
                    } else {
                        const double artH = static_cast<double>(s.mediaArtSize);
                        if (host.media.artBox) host.media.artBox.Width(artH);
                        if (host.media.artBackdrop) host.media.artBackdrop.Width(artH);
                        if (host.media.artRect) {
                            host.media.artRect.Width(artH);
                            host.media.artRect.Visibility(Visibility::Collapsed);
                        }
                        if (host.media.artBrush) host.media.artBrush.ImageSource(nullptr);
                        if (host.media.fallbackNote) host.media.fallbackNote.Visibility(Visibility::Visible);
                    }
                    host.media.lastThumbHash = media.thumbnailHash;
                }
                UpdateMediaHoverVisual(host.media, s, host.hWnd);
            }
        }

        if (host.moduleDivider) {
            host.moduleDivider.Visibility(showMediaNow ? Visibility::Visible : Visibility::Collapsed);
            host.moduleDivider.Background(MakeBrush(isLight
                ? winrt::Windows::UI::Color{0x24, 0x00, 0x00, 0x00}
                : winrt::Windows::UI::Color{0x28, 0xFF, 0xFF, 0xFF}));
        }

        if (g_miniPlayerOpen.load() && host.hWnd == g_miniPlayerAnchorHwnd) {
            RefreshMiniPlayerUI(s, media);
        }

        const bool anyVisible = s.enableVpnButton || showMediaNow;
        host.islandRoot.Visibility(anyVisible ? Visibility::Visible : Visibility::Collapsed);
    } catch (...) {}
}

void ShowVpnContextMenu(FrameworkElement const& target) {
    if (!target || g_unloading.load()) return;
    try {
        const IslandSettings s = GetSettingsSnapshot();
        const VpnState st = g_vpnState.load();

        const bool ru = IsRussianUi();
        MenuFlyout menu;

        MenuFlyoutItem toggleItem;
        toggleItem.Text(st == VpnState::Connected
            ? (ru ? L"Отключить VPN" : L"Disconnect VPN")
            : (ru ? L"Включить VPN" : L"Connect VPN"));
        toggleItem.Click([](auto const&, auto const&) {
            RequestVpnToggle();
        });
        menu.Items().Append(toggleItem);

        menu.Items().Append(MenuFlyoutSeparator{});

        MenuFlyoutItem syncTasksItem;
        syncTasksItem.Text(ru
            ? L"Настроить / обновить запуск без UAC (1 раз)"
            : L"Setup / update passwordless toggle (1 time)");
        if (s.configPath.empty() || s.wgExePath.empty()) {
            syncTasksItem.IsEnabled(false);
        }
        syncTasksItem.Click([](auto const&, auto const&) {
            SetupPasswordlessTasks(GetSettingsSnapshot());
        });
        menu.Items().Append(syncTasksItem);

        MenuFlyoutItem openWgItem;
        openWgItem.Text(ru ? L"Открыть WireGuard" : L"Open WireGuard");
        if (s.wgExePath.empty()) {
            openWgItem.IsEnabled(false);
        }
        openWgItem.Click([exe = s.wgExePath](auto const&, auto const&) {
            ShellExecuteW(nullptr, L"open", exe.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        });
        menu.Items().Append(openWgItem);

        MenuFlyoutItem openConfItem;
        openConfItem.Text(ru ? L"Открыть конфиг (.conf)" : L"Open config (.conf)");
        if (s.configPath.empty()) {
            openConfItem.IsEnabled(false);
        }
        openConfItem.Click([conf = s.configPath](auto const&, auto const&) {
            ShellExecuteW(nullptr, L"open", conf.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        });
        menu.Items().Append(openConfItem);

        try { menu.ShouldConstrainToRootBounds(false); } catch (...) {}
        Controls::Primitives::FlyoutShowOptions opts;
        opts.Placement(Controls::Primitives::FlyoutPlacementMode::Top);
        menu.ShowAt(target, opts);
    } catch (...) {}
}

VpnModuleUI BuildVpnModuleUI(HWND hostHwnd, const IslandSettings& s) {
    VpnModuleUI ui;

    Grid slot;
    slot.VerticalAlignment(VerticalAlignment::Stretch);

    Border capsule;
    capsule.VerticalAlignment(VerticalAlignment::Stretch);

    const winrt::Windows::UI::Color transparentColor{0x00, 0xFF, 0xFF, 0xFF};
    const double innerRadius = static_cast<double>(std::max(0, s.cornerRadius - 1));
    const double btnPadX = static_cast<double>(s.vpnButtonPadX);

    Border buttonPill;
    buttonPill.CornerRadius({innerRadius, innerRadius, innerRadius, innerRadius});
    buttonPill.Background(MakeBrush(transparentColor));
    buttonPill.BorderThickness({1, 1, 1, 1});
    buttonPill.BorderBrush(MakeBrush(transparentColor));
    buttonPill.Padding({btnPadX, 0, btnPadX, 0});
    buttonPill.VerticalAlignment(VerticalAlignment::Stretch);
    try {
        BrushTransition bt;
        bt.Duration(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(83)));
        buttonPill.BackgroundTransition(bt);
    } catch (...) {}

    Grid contentHost;
    contentHost.VerticalAlignment(VerticalAlignment::Center);
    contentHost.HorizontalAlignment(HorizontalAlignment::Center);

    const double dotGap = static_cast<double>(s.dotSpacing);
    const auto fw = GetConfiguredFontWeight(s.fontWeight);

    auto populateRow = [&](StackPanel const& targetRow, const std::wstring* sampleText, bool isSpacer) {
        targetRow.Orientation(Orientation::Horizontal);
        targetRow.VerticalAlignment(VerticalAlignment::Center);
        targetRow.HorizontalAlignment(HorizontalAlignment::Center);
        targetRow.Spacing(0);

        auto appendDot = [&]() {
            if (!s.showStatusDot) return;
            winrt::Windows::UI::Xaml::Shapes::Ellipse dot;
            dot.Width(static_cast<double>(s.dotSize));
            dot.Height(static_cast<double>(s.dotSize));
            dot.VerticalAlignment(VerticalAlignment::Center);

            if (s.showText) {
                if (s.dotPosition == L"left") {
                    dot.Margin({0, 0, dotGap, 0});
                } else {
                    dot.Margin({dotGap, 0, 0, 0});
                }
            } else if (s.showIcon) {
                dot.Margin({dotGap, 0, 0, 0});
            }

            if (s.dotOffsetY != 0) {
                TranslateTransform tt;
                tt.Y(static_cast<double>(s.dotOffsetY));
                dot.RenderTransform(tt);
            }

            targetRow.Children().Append(dot);
            if (!isSpacer) {
                ui.statusDot = dot;
            }
        };

        if (s.showIcon) {
            const double iconSz = static_cast<double>(s.iconSize);
            const Thickness iconMargin = s.showText ? Thickness{0, 0, 6.0, 0} : Thickness{0, 0, 0, 0};
            if (isSpacer) {
                Border iconSpacer;
                iconSpacer.Width(iconSz);
                iconSpacer.Height(iconSz);
                iconSpacer.VerticalAlignment(VerticalAlignment::Center);
                iconSpacer.Margin(iconMargin);
                targetRow.Children().Append(iconSpacer);
            } else {
                Grid iconHost;
                iconHost.Width(iconSz);
                iconHost.Height(iconSz);
                iconHost.VerticalAlignment(VerticalAlignment::Center);
                iconHost.Margin(iconMargin);

                auto onPath = CreateSvgIconPath(GetSvgIconPathData(s.iconConnected), iconSz);
                if (onPath) {
                    iconHost.Children().Append(onPath);
                    ui.iconPathConnected = onPath;
                }

                const std::wstring effectiveOff = (s.iconDisconnected == L"same")
                    ? s.iconConnected
                    : s.iconDisconnected;
                if (effectiveOff != s.iconConnected) {
                    auto offPath = CreateSvgIconPath(GetSvgIconPathData(effectiveOff), iconSz);
                    if (offPath) {
                        offPath.Visibility(Visibility::Collapsed);
                        iconHost.Children().Append(offPath);
                        ui.iconPathDisconnected = offPath;
                    }
                }

                targetRow.Children().Append(iconHost);
                ui.iconHost = iconHost;
            }
        }

        if (s.dotPosition == L"left") {
            appendDot();
        }

        if (s.showText) {
            TextBlock label;
            label.FontFamily(FontFamily(L"Segoe UI Variable Display"));
            label.FontSize(static_cast<double>(s.fontSize));
            label.FontWeight(fw);
            label.TextLineBounds(TextLineBounds::Tight);
            label.VerticalAlignment(VerticalAlignment::Center);
            label.HorizontalAlignment(HorizontalAlignment::Left);
            if (sampleText) {
                label.Text(*sampleText);
            }
            targetRow.Children().Append(label);
            if (!isSpacer) {
                ui.textBlock = label;
            }
        }

        if (s.dotPosition != L"left") {
            appendDot();
        }
    };

    if (s.showText) {
        for (const auto& txt : {s.textConnected, s.textDisconnected}) {
            StackPanel spacerRow;
            spacerRow.Opacity(0.0);
            spacerRow.IsHitTestVisible(false);
            populateRow(spacerRow, &txt, true);
            contentHost.Children().Append(spacerRow);
        }
    }

    StackPanel row;
    populateRow(row, nullptr, false);
    contentHost.Children().Append(row);

    buttonPill.Child(contentHost);
    capsule.Child(buttonPill);
    slot.Children().Append(capsule);

    const bool enableContextMenu = s.enableContextMenu;

    slot.PointerEntered([hostHwnd, buttonPill](auto const&, auto const&) {
        if (g_unloading.load()) return;
        if (IslandHost* h = FindHostByHwnd(hostHwnd)) {
            if (h->media.isHovered || h->media.isPressed) {
                h->media.isHovered = false;
                h->media.isPressed = false;
                UpdateMediaHoverVisual(h->media, GetSettingsSnapshot(), hostHwnd);
            }
        }
        const bool light = IsSystemLightTheme();
        buttonPill.Background(MakeBrush(light
            ? winrt::Windows::UI::Color{0x99, 0xFF, 0xFF, 0xFF}
            : winrt::Windows::UI::Color{0x15, 0xFF, 0xFF, 0xFF}));
        buttonPill.BorderBrush(MakeBrush(light
            ? winrt::Windows::UI::Color{0x12, 0x00, 0x00, 0x00}
            : winrt::Windows::UI::Color{0x20, 0xFF, 0xFF, 0xFF}));
    });

    slot.PointerExited([buttonPill, transparentColor](auto const&, auto const&) {
        if (g_unloading.load()) return;
        buttonPill.Background(MakeBrush(transparentColor));
        buttonPill.BorderBrush(MakeBrush(transparentColor));
    });

    slot.PointerCanceled([buttonPill, transparentColor](auto const&, auto const&) {
        if (g_unloading.load()) return;
        buttonPill.Background(MakeBrush(transparentColor));
        buttonPill.BorderBrush(MakeBrush(transparentColor));
    });

    slot.PointerPressed([buttonPill, enableContextMenu](auto const& sender, Input::PointerRoutedEventArgs const& e) {
        if (g_unloading.load()) return;
        const auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
        const bool allowRight = enableContextMenu &&
                                (kind == winrt::Windows::UI::Input::PointerUpdateKind::RightButtonPressed);
        const bool allowLeft = (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonPressed ||
                                kind == winrt::Windows::UI::Input::PointerUpdateKind::Other);
        if (!allowLeft && !allowRight) return;

        if (auto el = sender.template try_as<UIElement>()) {
            el.CapturePointer(e.Pointer());
        }
        const bool light = IsSystemLightTheme();
        buttonPill.Background(MakeBrush(light
            ? winrt::Windows::UI::Color{0x55, 0xFF, 0xFF, 0xFF}
            : winrt::Windows::UI::Color{0x0A, 0xFF, 0xFF, 0xFF}));
        e.Handled(true);
    });

    slot.PointerReleased([buttonPill, enableContextMenu](auto const& sender, Input::PointerRoutedEventArgs const& e) {
        if (g_unloading.load()) return;
        const auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
        const bool isRight = (kind == winrt::Windows::UI::Input::PointerUpdateKind::RightButtonReleased);
        const bool isLeft = (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased ||
                             kind == winrt::Windows::UI::Input::PointerUpdateKind::Other);

        if (isRight && !enableContextMenu) return;

        bool inside = false;
        if (auto el = sender.template try_as<UIElement>()) {
            el.ReleasePointerCapture(e.Pointer());
            const auto pos = e.GetCurrentPoint(el).Position();
            const auto sz = el.RenderSize();
            inside = (pos.X >= 0 && pos.X <= sz.Width && pos.Y >= 0 && pos.Y <= sz.Height);
        }

        if (!isLeft && !isRight) return;

        const bool light = IsSystemLightTheme();
        buttonPill.Background(MakeBrush(light
            ? winrt::Windows::UI::Color{0x99, 0xFF, 0xFF, 0xFF}
            : winrt::Windows::UI::Color{0x15, 0xFF, 0xFF, 0xFF}));

        if (inside) {
            if (isLeft) {
                RequestVpnToggle();
            } else if (isRight && enableContextMenu) {
                if (auto fe = sender.template try_as<FrameworkElement>()) {
                    ShowVpnContextMenu(fe);
                }
            }
        }
        e.Handled(true);
    });

    ui.capsule = capsule;
    ui.slot = slot;
    ui.buttonPill = buttonPill;
    return ui;
}

MediaModuleUI BuildMediaModuleUI(HWND hostHwnd, const IslandSettings& s) {
    MediaModuleUI ui;

    Grid slot;
    slot.VerticalAlignment(VerticalAlignment::Stretch);
    slot.Visibility(Visibility::Collapsed);
    slot.Background(MakeBrush({0x00, 0xFF, 0xFF, 0xFF}));

    Border capsule;
    capsule.VerticalAlignment(VerticalAlignment::Stretch);

    const double innerRadius = static_cast<double>(std::max(0, s.cornerRadius - 1));

    Border hoverPlate;
    hoverPlate.CornerRadius({innerRadius, innerRadius, innerRadius, innerRadius});
    hoverPlate.Background(MakeBrush({0x00, 0xFF, 0xFF, 0xFF}));
    hoverPlate.BorderThickness({1, 1, 1, 1});
    hoverPlate.BorderBrush(MakeBrush({0x00, 0xFF, 0xFF, 0xFF}));
    hoverPlate.Padding({
        static_cast<double>(s.mediaPadLeft), 0.0,
        static_cast<double>(s.mediaPadRight), 0.0});
    hoverPlate.VerticalAlignment(VerticalAlignment::Stretch);
    if (s.mediaHoverAnimation) {
        try {
            BrushTransition bt;
            bt.Duration(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(83)));
            hoverPlate.BackgroundTransition(bt);
        } catch (...) {}
    }

    StackPanel row;
    row.Orientation(Orientation::Horizontal);
    row.VerticalAlignment(VerticalAlignment::Center);
    row.Spacing(static_cast<double>(s.mediaTextToControlsGap));

    const bool hasClickInfo = s.mediaShowAlbumArt || s.mediaShowTitle || s.mediaShowArtist;
    if (hasClickInfo) {
        Grid clickArea;
        clickArea.VerticalAlignment(VerticalAlignment::Center);
        clickArea.Background(MakeBrush({0x00, 0xFF, 0xFF, 0xFF}));

        StackPanel infoRow;
        infoRow.Orientation(Orientation::Horizontal);
        infoRow.VerticalAlignment(VerticalAlignment::Center);
        infoRow.Spacing(static_cast<double>(s.mediaArtToTextGap));

        if (s.mediaShowAlbumArt) {
            const double artH = static_cast<double>(s.mediaArtSize);
            const double artCr = static_cast<double>(s.mediaArtCornerRadius);

            Grid artBox;
            artBox.Width(artH);
            artBox.Height(artH);
            artBox.VerticalAlignment(VerticalAlignment::Center);

            Border artBackdrop;
            artBackdrop.Width(artH);
            artBackdrop.Height(artH);
            artBackdrop.CornerRadius({artCr, artCr, artCr, artCr});
            auto fallbackNote = CreateSvgIconPath(kSvgMediaNote, std::max(10.0, artH * 0.44));
            if (fallbackNote) {
                artBackdrop.Child(fallbackNote);
                ui.fallbackNote = fallbackNote;
            }
            artBox.Children().Append(artBackdrop);
            ui.artBackdrop = artBackdrop;

            winrt::Windows::UI::Xaml::Shapes::Rectangle artRect;
            artRect.Width(artH);
            artRect.Height(artH);
            artRect.RadiusX(artCr);
            artRect.RadiusY(artCr);
            artRect.Visibility(Visibility::Collapsed);

            ImageBrush artBrush;
            artBrush.Stretch(Stretch::UniformToFill);
            artRect.Fill(artBrush);
            artBox.Children().Append(artRect);
            ui.artRect = artRect;
            ui.artBrush = artBrush;

            infoRow.Children().Append(artBox);
            ui.artBox = artBox;
        }

        if (s.mediaShowTitle || s.mediaShowArtist) {
            const double textW = static_cast<double>(s.mediaTextWidth);
            StackPanel textCol;
            textCol.Width(textW);
            textCol.VerticalAlignment(VerticalAlignment::Center);
            textCol.HorizontalAlignment(HorizontalAlignment::Left);
            textCol.Spacing(0);

            auto makeTextLine = [&](double fontSize,
                                    winrt::Windows::UI::Text::FontWeight fw,
                                    Thickness outerMargin,
                                    bool marquee) -> MarqueeLine {
                MarqueeLine line;
                line.enabled = marquee;
                line.viewWidth = textW;
                line.pauseTicks = 45;

                TextBlock tb;
                tb.FontFamily(FontFamily(L"Segoe UI Variable Display"));
                tb.FontSize(fontSize);
                tb.FontWeight(fw);
                tb.TextWrapping(TextWrapping::NoWrap);
                tb.VerticalAlignment(VerticalAlignment::Center);
                tb.HorizontalAlignment(HorizontalAlignment::Left);
                line.textBlock = tb;

                if (marquee) {
                    TranslateTransform tt;
                    tb.RenderTransform(tt);
                    line.transform = tt;

                    Grid clipBox;
                    clipBox.Width(textW);
                    clipBox.Margin(outerMargin);
                    clipBox.HorizontalAlignment(HorizontalAlignment::Left);
                    clipBox.VerticalAlignment(VerticalAlignment::Center);
                    try {
                        RectangleGeometry clipGeom;
                        clipBox.Clip(clipGeom);
                        clipBox.SizeChanged([clipGeom](winrt::Windows::Foundation::IInspectable const&, SizeChangedEventArgs const& e) {
                            try {
                                const auto sz = e.NewSize();
                                clipGeom.Rect({0.0f, -2.0f, sz.Width, sz.Height + 4.0f});
                            } catch (...) {}
                        });
                    } catch (...) {}

                    StackPanel track;
                    track.Orientation(Orientation::Horizontal);
                    track.HorizontalAlignment(HorizontalAlignment::Left);
                    track.VerticalAlignment(VerticalAlignment::Center);
                    track.Children().Append(tb);
                    clipBox.Children().Append(track);
                    textCol.Children().Append(clipBox);
                } else {
                    tb.Margin(outerMargin);
                    tb.TextTrimming(TextTrimming::CharacterEllipsis);
                    textCol.Children().Append(tb);
                }

                return line;
            };

            if (s.mediaShowTitle) {
                ui.titleLine = makeTextLine(
                    static_cast<double>(s.mediaTitleFontSize),
                    winrt::Windows::UI::Text::FontWeights::SemiBold(),
                    {0, 0, 0, 0},
                    s.mediaTitleMarquee);
            }

            if (s.mediaShowArtist) {
                const double topGap = s.mediaShowTitle ? static_cast<double>(s.mediaTextSpacing) : 0.0;
                ui.artistLine = makeTextLine(
                    static_cast<double>(s.mediaArtistFontSize),
                    winrt::Windows::UI::Text::FontWeights::Normal(),
                    {0, topGap, 0, 0},
                    s.mediaArtistMarquee);
                if (ui.artistLine.textBlock) {
                    ui.artistLine.textBlock.Opacity(s.mediaColorMode == L"custom" ? 1.0 : 0.72);
                }
            }

            infoRow.Children().Append(textCol);

            const bool needMarqueeTimer =
                (s.mediaShowTitle && s.mediaTitleMarquee) ||
                (s.mediaShowArtist && s.mediaArtistMarquee);
            if (needMarqueeTimer) {
                try {
                    DispatcherTimer timer;
                    timer.Interval(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(33)));
                    winrt::weak_ref<Grid> weakSlot = winrt::make_weak(slot);
                    ui.marqueeTickToken = timer.Tick([hostHwnd, weakSlot](winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&) {
                        if (g_unloading.load() || !weakSlot.get()) return;
                        auto stepMarquee = [](MarqueeLine& line) {
                            if (!line.enabled || !line.textBlock || !line.transform) return;
                            if (line.textBlock.Visibility() != Visibility::Visible) return;
                            double fullW = line.textBlock.ActualWidth();
                            if (fullW < 1.0) fullW = line.textBlock.DesiredSize().Width;
                            const double overflow = fullW - line.viewWidth;
                            if (overflow <= 2.0) {
                                if (line.offset != 0.0) {
                                    line.offset = 0.0;
                                    line.forward = true;
                                    line.pauseTicks = 45;
                                    line.transform.X(0.0);
                                }
                                return;
                            }
                            if (line.pauseTicks > 0) {
                                --line.pauseTicks;
                                return;
                            }
                            constexpr double kSpeedPx = 0.75;
                            if (line.forward) {
                                line.offset += kSpeedPx;
                                if (line.offset >= overflow) {
                                    line.offset = overflow;
                                    line.forward = false;
                                    line.pauseTicks = 45;
                                }
                            } else {
                                line.offset -= kSpeedPx;
                                if (line.offset <= 0.0) {
                                    line.offset = 0.0;
                                    line.forward = true;
                                    line.pauseTicks = 45;
                                }
                            }
                            line.transform.X(-line.offset);
                        };
                        if (IslandHost* h = FindHostByHwnd(hostHwnd)) {
                            stepMarquee(h->media.titleLine);
                            stepMarquee(h->media.artistLine);
                        }
                    });
                    timer.Start();
                    ui.marqueeTimer = timer;
                } catch (...) {}
            }
        }

        clickArea.Children().Append(infoRow);

        if (s.mediaEnableMiniPlayer) {
            clickArea.PointerPressed([hostHwnd](auto const& sender, Input::PointerRoutedEventArgs const& e) {
                if (g_unloading.load()) return;
                const auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
                const bool isLeft = (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonPressed ||
                                     kind == winrt::Windows::UI::Input::PointerUpdateKind::Other);
                if (!isLeft) return;
                if (auto el = sender.template try_as<UIElement>()) {
                    el.CapturePointer(e.Pointer());
                }
                if (IslandHost* h = FindHostByHwnd(hostHwnd)) {
                    h->media.isPressed = true;
                    UpdateMediaHoverVisual(h->media, GetSettingsSnapshot(), hostHwnd);
                }
                e.Handled(true);
            });

            clickArea.PointerReleased([hostHwnd, slot](auto const& sender, Input::PointerRoutedEventArgs const& e) {
                if (g_unloading.load()) return;
                const auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
                const bool isLeft = (kind == winrt::Windows::UI::Input::PointerUpdateKind::LeftButtonReleased ||
                                     kind == winrt::Windows::UI::Input::PointerUpdateKind::Other);
                if (!isLeft) return;

                bool inside = false;
                if (auto el = sender.template try_as<UIElement>()) {
                    el.ReleasePointerCapture(e.Pointer());
                    const auto pos = e.GetCurrentPoint(el).Position();
                    const auto sz = el.RenderSize();
                    inside = (pos.X >= 0 && pos.X <= sz.Width && pos.Y >= 0 && pos.Y <= sz.Height);
                }
                if (IslandHost* h = FindHostByHwnd(hostHwnd)) {
                    h->media.isPressed = false;
                    UpdateMediaHoverVisual(h->media, GetSettingsSnapshot(), hostHwnd);
                }
                if (inside) {
                    ShowMiniPlayerFlyout(hostHwnd, slot);
                }
                e.Handled(true);
            });

            clickArea.PointerCaptureLost([hostHwnd](auto const&, auto const&) {
                if (g_unloading.load()) return;
                if (IslandHost* h = FindHostByHwnd(hostHwnd)) {
                    h->media.isPressed = false;
                    UpdateMediaHoverVisual(h->media, GetSettingsSnapshot(), hostHwnd);
                }
            });
        }

        row.Children().Append(clickArea);
        ui.clickArea = clickArea;
    }

    StackPanel controlsRow;
    controlsRow.Orientation(Orientation::Horizontal);
    controlsRow.VerticalAlignment(VerticalAlignment::Center);
    controlsRow.Spacing(static_cast<double>(s.mediaButtonSpacing));

    const double btnSz = static_cast<double>(s.mediaButtonSize);
    const double iconSz = std::max(9.0, btnSz * 0.44);

    if (s.mediaShowShuffleBtn) {
        ui.shufflePath = CreateSvgIconPath(kSvgMediaShuffle, iconSz);
        ui.shuffleBtn = MakeTransportButton(btnSz, 4.0, false, ui.shufflePath, nullptr, MediaCommand::ToggleShuffle);
        controlsRow.Children().Append(ui.shuffleBtn);
    }

    if (s.mediaShowPlaybackBtns) {
        ui.prevPath = CreateSvgIconPath(kSvgMediaPrev, iconSz);
        ui.prevBtn = MakeTransportButton(btnSz, 4.0, false, ui.prevPath, nullptr, MediaCommand::Prev);
        controlsRow.Children().Append(ui.prevBtn);

        ui.playPath = CreateSvgIconPath(kSvgMediaPlay, iconSz + 1.0);
        ui.pausePath = CreateSvgIconPath(kSvgMediaPause, iconSz + 1.0);
        if (ui.pausePath) ui.pausePath.Visibility(Visibility::Collapsed);
        ui.playPauseBtn = MakeTransportButton(btnSz, 4.0, false, ui.playPath, ui.pausePath, MediaCommand::PlayPause);
        controlsRow.Children().Append(ui.playPauseBtn);

        ui.nextPath = CreateSvgIconPath(kSvgMediaNext, iconSz);
        ui.nextBtn = MakeTransportButton(btnSz, 4.0, false, ui.nextPath, nullptr, MediaCommand::Next);
        controlsRow.Children().Append(ui.nextBtn);
    }

    if (s.mediaShowRepeatBtn) {
        ui.repeatAllPath = CreateSvgIconPath(kSvgMediaRepeat, iconSz);
        ui.repeatOnePath = CreateSvgIconPath(kSvgMediaRepeatOne, iconSz);
        if (ui.repeatOnePath) ui.repeatOnePath.Visibility(Visibility::Collapsed);
        ui.repeatBtn = MakeTransportButton(btnSz, 4.0, false, ui.repeatAllPath, ui.repeatOnePath, MediaCommand::CycleRepeat);
        controlsRow.Children().Append(ui.repeatBtn);
    }

    if (s.mediaShowLikeBtn) {
        ui.addPath = CreateSvgIconPath(kSvgMediaAdd, iconSz);
        ui.checkPath = CreateSvgIconPath(kSvgMediaCheck, iconSz);
        if (ui.checkPath) ui.checkPath.Visibility(Visibility::Collapsed);
        ui.likeBtn = MakeTransportButton(btnSz, 4.0, false, ui.addPath, ui.checkPath, MediaCommand::ToggleLike);
        controlsRow.Children().Append(ui.likeBtn);
    }

    if (controlsRow.Children().Size() > 0) {
        row.Children().Append(controlsRow);
    }

    if (s.mediaEnableCtxMenu) {
        slot.PointerPressed([](auto const& sender, Input::PointerRoutedEventArgs const& e) {
            if (g_unloading.load()) return;
            const auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
            if (kind != winrt::Windows::UI::Input::PointerUpdateKind::RightButtonPressed) return;
            if (auto el = sender.template try_as<UIElement>()) {
                el.CapturePointer(e.Pointer());
            }
            e.Handled(true);
        });

        slot.PointerReleased([](auto const& sender, Input::PointerRoutedEventArgs const& e) {
            if (g_unloading.load()) return;
            const auto kind = e.GetCurrentPoint(nullptr).Properties().PointerUpdateKind();
            if (kind != winrt::Windows::UI::Input::PointerUpdateKind::RightButtonReleased) return;

            bool inside = false;
            if (auto el = sender.template try_as<UIElement>()) {
                el.ReleasePointerCapture(e.Pointer());
                const auto pos = e.GetCurrentPoint(el).Position();
                const auto sz = el.RenderSize();
                inside = (pos.X >= 0 && pos.X <= sz.Width && pos.Y >= 0 && pos.Y <= sz.Height);
            }
            if (inside) {
                if (auto fe = sender.template try_as<FrameworkElement>()) {
                    ShowMediaContextMenu(fe);
                }
            }
            e.Handled(true);
        });
    }

    if (s.mediaWheelAction != L"none") {
        const std::wstring wheelAction = s.mediaWheelAction;
        slot.PointerWheelChanged([wheelAction](winrt::Windows::Foundation::IInspectable const&, Input::PointerRoutedEventArgs const& e) {
            if (g_unloading.load()) return;
            const int delta = e.GetCurrentPoint(nullptr).Properties().MouseWheelDelta();
            if (delta == 0) return;
            if (wheelAction == L"track") {
                RequestMediaCommand(delta > 0 ? MediaCommand::Next : MediaCommand::Prev);
                e.Handled(true);
            } else if (wheelAction == L"session") {
                RequestMediaCommand(MediaCommand::SwitchNextSession);
                e.Handled(true);
            }
        });
    }

    slot.PointerEntered([hostHwnd](auto const&, auto const&) {
        if (g_unloading.load()) return;
        if (IslandHost* h = FindHostByHwnd(hostHwnd)) {
            h->media.isHovered = true;
            UpdateMediaHoverVisual(h->media, GetSettingsSnapshot(), hostHwnd);
        }
    });

    slot.PointerExited([hostHwnd](auto const&, auto const&) {
        if (g_unloading.load()) return;
        if (IslandHost* h = FindHostByHwnd(hostHwnd)) {
            h->media.isHovered = false;
            h->media.isPressed = false;
            UpdateMediaHoverVisual(h->media, GetSettingsSnapshot(), hostHwnd);
        }
    });

    slot.PointerCanceled([hostHwnd](auto const&, auto const&) {
        if (g_unloading.load()) return;
        if (IslandHost* h = FindHostByHwnd(hostHwnd)) {
            h->media.isHovered = false;
            h->media.isPressed = false;
            UpdateMediaHoverVisual(h->media, GetSettingsSnapshot(), hostHwnd);
        }
    });

    slot.PointerCaptureLost([hostHwnd](auto const&, auto const&) {
        if (g_unloading.load()) return;
        if (IslandHost* h = FindHostByHwnd(hostHwnd)) {
            h->media.isHovered = false;
            h->media.isPressed = false;
            UpdateMediaHoverVisual(h->media, GetSettingsSnapshot(), hostHwnd);
        }
    });

    hoverPlate.Child(row);
    capsule.Child(hoverPlate);
    slot.Children().Append(capsule);

    ui.capsule = capsule;
    ui.hoverPlate = hoverPlate;
    ui.slot = slot;
    return ui;
}

void BuildIslandForHost(IslandHost& host, const IslandSettings& s) {
    Grid root;
    root.Name(kIslandRootName);
    root.Height(static_cast<double>(s.height));
    root.VerticalAlignment(VerticalAlignment::Center);
    root.HorizontalAlignment(HorizontalAlignment::Left);

    try {
        TransitionCollection transitions;
        RepositionThemeTransition repo;
        transitions.Append(repo);
        root.Transitions(transitions);
    } catch (...) {}

    const bool light = IsSystemLightTheme();

    Border pill;
    pill.VerticalAlignment(VerticalAlignment::Stretch);
    pill.HorizontalAlignment(HorizontalAlignment::Stretch);

    StackPanel modulesRow;
    modulesRow.Orientation(Orientation::Horizontal);
    modulesRow.VerticalAlignment(VerticalAlignment::Stretch);
    modulesRow.Spacing(static_cast<double>(s.moduleSpacing));

    if (s.enableMediaPlayer) {
        host.media = BuildMediaModuleUI(host.hWnd, s);
        modulesRow.Children().Append(host.media.slot);
    }

    if (s.enableMediaPlayer && s.enableVpnButton && s.showModuleDivider) {
        Border div;
        div.Width(1.0);
        div.Height(std::max(12.0, static_cast<double>(s.height) * 0.45));
        div.VerticalAlignment(VerticalAlignment::Center);
        div.CornerRadius({0.5, 0.5, 0.5, 0.5});
        div.Visibility(Visibility::Collapsed);
        modulesRow.Children().Append(div);
        host.moduleDivider = div;
    }

    if (s.enableVpnButton) {
        host.vpn = BuildVpnModuleUI(host.hWnd, s);
        modulesRow.Children().Append(host.vpn.slot);
    }

    if (s.capsuleMode == L"separate") {
        ClearSurfaceStyle(pill);
        if (host.media.capsule) {
            ApplySurfaceStyle(host.media.capsule, s, light);
            host.media.capsule.Padding({
                0.0, static_cast<double>(s.paddingY),
                0.0, static_cast<double>(s.paddingY)});
        }
        if (host.vpn.capsule) {
            ApplySurfaceStyle(host.vpn.capsule, s, light);
            host.vpn.capsule.Padding({
                0.0, static_cast<double>(s.paddingY),
                0.0, static_cast<double>(s.paddingY)});
        }
    } else {
        ApplySurfaceStyle(pill, s, light);
        pill.Padding({
            static_cast<double>(s.paddingX), static_cast<double>(s.paddingY),
            static_cast<double>(s.paddingX), static_cast<double>(s.paddingY)});
        if (host.media.capsule) ClearSurfaceStyle(host.media.capsule);
        if (host.vpn.capsule) ClearSurfaceStyle(host.vpn.capsule);
    }

    pill.Child(modulesRow);
    root.Children().Append(pill);

    host.islandRoot = root;
    host.islandPill = pill;
    host.modulesRow = modulesRow;
}

void RemoveHostUI(IslandHost& host) {
    if (g_miniPlayerOpen.load() && host.hWnd == g_miniPlayerAnchorHwnd) {
        CloseMiniPlayerOnUiThread();
    }
    if (host.injectionParent) {
        try {
            for (int i = static_cast<int>(host.injectionParent.Children().Size()) - 1; i >= 0; --i) {
                auto fe = host.injectionParent.Children().GetAt(i).try_as<FrameworkElement>();
                if (fe && fe.Name() == kIslandRootName) {
                    host.injectionParent.Children().RemoveAt(i);
                }
            }

            if (host.injectedColumn >= 0 &&
                host.injectedColumn < static_cast<int>(host.injectionParent.ColumnDefinitions().Size())) {
                for (uint32_t i = 0; i < host.injectionParent.Children().Size(); ++i) {
                    if (auto child = host.injectionParent.Children().GetAt(i).try_as<FrameworkElement>()) {
                        const int col = Grid::GetColumn(child);
                        if (col > host.injectedColumn) {
                            Grid::SetColumn(child, col - 1);
                        }
                    }
                }
                host.injectionParent.ColumnDefinitions().RemoveAt(host.injectedColumn);
            }
        } catch (...) {}
    }

    if (host.media.marqueeTimer) {
        try { host.media.marqueeTimer.Stop(); } catch (...) {}
        if (host.media.marqueeTickToken.value) {
            try { host.media.marqueeTimer.Tick(host.media.marqueeTickToken); } catch (...) {}
            host.media.marqueeTickToken = {};
        }
        host.media.marqueeTimer = nullptr;
    }

    host.media.checkPath = nullptr;
    host.media.addPath = nullptr;
    host.media.repeatOnePath = nullptr;
    host.media.repeatAllPath = nullptr;
    host.media.nextPath = nullptr;
    host.media.pausePath = nullptr;
    host.media.playPath = nullptr;
    host.media.prevPath = nullptr;
    host.media.shufflePath = nullptr;
    host.media.likeBtn = nullptr;
    host.media.repeatBtn = nullptr;
    host.media.nextBtn = nullptr;
    host.media.playPauseBtn = nullptr;
    host.media.prevBtn = nullptr;
    host.media.shuffleBtn = nullptr;
    host.media.artistLine.transform = nullptr;
    host.media.artistLine.textBlock = nullptr;
    host.media.titleLine.transform = nullptr;
    host.media.titleLine.textBlock = nullptr;
    host.media.fallbackNote = nullptr;
    host.media.artBrush = nullptr;
    host.media.artRect = nullptr;
    host.media.artBackdrop = nullptr;
    host.media.artBox = nullptr;
    host.media.clickArea = nullptr;
    host.media.hoverPlate = nullptr;
    host.media.capsule = nullptr;
    host.media.slot = nullptr;
    host.media.lastThumbHash = 0;
    host.media.isHovered = false;
    host.media.isPressed = false;

    host.vpn.statusDot = nullptr;
    host.vpn.textBlock = nullptr;
    host.vpn.iconPathDisconnected = nullptr;
    host.vpn.iconPathConnected = nullptr;
    host.vpn.iconHost = nullptr;
    host.vpn.buttonPill = nullptr;
    host.vpn.capsule = nullptr;
    host.vpn.slot = nullptr;
    host.moduleDivider = nullptr;
    host.modulesRow = nullptr;
    host.islandPill = nullptr;
    host.islandRoot = nullptr;
    host.injectionParent = nullptr;
    host.injectedColumn = -1;
}

void PruneInvalidHostsOnUiThread() {
    for (int i = static_cast<int>(g_hosts.size()) - 1; i >= 0; --i) {
        if (!IsWindow(g_hosts[i].hWnd)) {
            HWND deadHwnd = g_hosts[i].hWnd;
            g_hosts[i].injectionParent = nullptr;
            RemoveHostUI(g_hosts[i]);
            g_hosts.erase(g_hosts.begin() + i);
            RemoveMountedHwnd(deadHwnd);
        }
    }
    if (g_miniPlayerOpen.load() && g_miniPlayerAnchorHwnd && !IsWindow(g_miniPlayerAnchorHwnd)) {
        CloseMiniPlayerOnUiThread();
    }
}

void RemoveHostForHwndOnUiThread(HWND hWnd) {
    PruneInvalidHostsOnUiThread();
    for (size_t i = 0; i < g_hosts.size(); ++i) {
        if (g_hosts[i].hWnd == hWnd) {
            RemoveHostUI(g_hosts[i]);
            g_hosts.erase(g_hosts.begin() + static_cast<ptrdiff_t>(i));
            break;
        }
    }
    RemoveMountedHwnd(hWnd);
}

void InjectHostForHwndOnUiThread(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd) || g_unloading.load()) return;

    const IslandSettings s = GetSettingsSnapshot();

    // Remove existing instance on this HWND first.
    RemoveHostForHwndOnUiThread(hWnd);

    try {
        XamlRoot xamlRoot = GetTaskbarXamlRoot(hWnd);
        if (!xamlRoot) return;
        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) return;

        Grid targetGrid = nullptr;
        bool isTray = (s.position == L"tray_left" || s.position == L"tray_right");

        if (isTray) {
            if (auto tf = FindChildByName(root, L"SystemTrayFrameGrid")) {
                targetGrid = tf.try_as<Grid>();
            }
        }
        if (!targetGrid) {
            targetGrid = FindTaskbarRootGrid(root);
            isTray = false;
        }
        if (!targetGrid) return;

        // Clear any leftover orphaned element with our root name.
        for (int i = static_cast<int>(targetGrid.Children().Size()) - 1; i >= 0; --i) {
            auto fe = targetGrid.Children().GetAt(i).try_as<FrameworkElement>();
            if (fe && fe.Name() == kIslandRootName) {
                targetGrid.Children().RemoveAt(i);
            }
        }

        IslandHost host;
        host.hWnd = hWnd;
        host.injectionParent = targetGrid;
        host.isTray = isTray;
        host.marginLeft = s.marginLeft;
        host.marginRight = s.marginRight;
        host.position = s.position;

        BuildIslandForHost(host, s);
        if (!host.islandRoot) return;

        if (isTray) {
            const int insertCol = (s.position == L"tray_left")
                ? 0
                : static_cast<int>(targetGrid.ColumnDefinitions().Size());
            ColumnDefinition newCol;
            newCol.Width({1.0, GridUnitType::Auto});

            if (insertCol >= static_cast<int>(targetGrid.ColumnDefinitions().Size())) {
                targetGrid.ColumnDefinitions().Append(newCol);
            } else {
                targetGrid.ColumnDefinitions().InsertAt(insertCol, newCol);
                for (uint32_t i = 0; i < targetGrid.Children().Size(); ++i) {
                    if (auto child = targetGrid.Children().GetAt(i).try_as<FrameworkElement>()) {
                        const int c = Grid::GetColumn(child);
                        if (c >= insertCol) {
                            Grid::SetColumn(child, c + 1);
                        }
                    }
                }
            }
            host.islandRoot.Margin({static_cast<double>(s.marginLeft), 0, static_cast<double>(s.marginRight), 0});
            Grid::SetColumn(host.islandRoot, insertCol);
            targetGrid.Children().Append(host.islandRoot);
            host.injectedColumn = insertCol;
        } else {
            const double leftM = static_cast<double>(s.marginLeft);
            double rightM = static_cast<double>(s.marginRight);

            if (s.position == L"taskbar_center_edge") {
                host.islandRoot.HorizontalAlignment(HorizontalAlignment::Center);
            } else if (s.position == L"taskbar_right_edge") {
                host.islandRoot.HorizontalAlignment(HorizontalAlignment::Right);
                if (auto trayFrame = FindChildByName(targetGrid, L"SystemTrayFrameGrid")) {
                    rightM += trayFrame.ActualWidth() + 4.0;
                }
            } else {
                host.islandRoot.HorizontalAlignment(HorizontalAlignment::Left);
            }

            host.islandRoot.Margin({leftM, 0, rightM, 0});
            Grid::SetColumn(host.islandRoot, 0);
            const int colCount = static_cast<int>(targetGrid.ColumnDefinitions().Size());
            if (colCount > 1) {
                Grid::SetColumnSpan(host.islandRoot, colCount);
            }
            Canvas::SetZIndex(host.islandRoot, 1001);
            targetGrid.Children().Append(host.islandRoot);
            host.injectedColumn = -1;
        }

        g_hosts.push_back(std::move(host));
        AddMountedHwnd(hWnd);

        IslandHost* mounted = FindHostByHwnd(hWnd);
        if (mounted) {
            RefreshHostUI(*mounted, s);
        }
    } catch (...) {
        Wh_Log(L"Exception during InjectHostForHwndOnUiThread for HWND %p", hWnd);
    }
}

void RefreshAllHostsUi(const IslandSettings& s) {
    const auto hwnds = GetMountedHwndsSnapshot();
    for (HWND hWnd : hwnds) {
        if (!IsWindow(hWnd)) continue;
        struct Param {
            HWND hWnd;
            const IslandSettings* settings;
        } param{hWnd, &s};
        RunFromWindowThread(hWnd, [](void* raw) {
            if (g_unloading.load()) return;
            auto* p = reinterpret_cast<Param*>(raw);
            if (IslandHost* host = FindHostByHwnd(p->hWnd)) {
                RefreshHostUI(*host, *p->settings);
            }
        }, &param);
    }
}

void RemoveHostsOnCurrentThread() {
    const DWORD curTid = GetCurrentThreadId();
    for (int i = static_cast<int>(g_hosts.size()) - 1; i >= 0; --i) {
        if (GetWindowThreadProcessId(g_hosts[i].hWnd, nullptr) == curTid) {
            HWND h = g_hosts[i].hWnd;
            RemoveHostUI(g_hosts[i]);
            g_hosts.erase(g_hosts.begin() + i);
            RemoveMountedHwnd(h);
        }
    }
}

// ===========================================================================
// WORKER THREAD (toggle, polling, multi-monitor sync, balloon notifications)
// ===========================================================================
void ShowBalloonMessage(HWND& ownerHwnd, const wchar_t* message, bool silent,
                        bool& notifActive, ULONGLONG& notifExpireTick) {
    HWND targetOwner = ownerHwnd;
    if (!targetOwner || !IsWindow(targetOwner)) {
        targetOwner = FindWindowW(L"Shell_TrayWnd", nullptr);
    }
    if (!targetOwner) return;

    NOTIFYICONDATAW nid{};
    nid.cbSize = sizeof(nid);
    nid.hWnd   = targetOwner;
    nid.uID    = kNotifIconId;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_INFO;
    nid.hIcon  = LoadIconW(nullptr, IDI_SHIELD);
    wcscpy_s(nid.szTip, L"Usagi Taskbar Island — VPN");
    wcscpy_s(nid.szInfoTitle, L"Usagi Taskbar Island");
    wcsncpy_s(nid.szInfo, message ? message : L"", _TRUNCATE);
    nid.dwInfoFlags = NIIF_INFO | NIIF_RESPECT_QUIET_TIME | (silent ? NIIF_NOSOUND : 0);

    if (notifActive && ownerHwnd == targetOwner) {
        Shell_NotifyIconW(NIM_MODIFY, &nid);
    } else {
        if (notifActive && ownerHwnd && ownerHwnd != targetOwner) {
            NOTIFYICONDATAW oldNid{};
            oldNid.cbSize = sizeof(oldNid);
            oldNid.hWnd = ownerHwnd;
            oldNid.uID = kNotifIconId;
            Shell_NotifyIconW(NIM_DELETE, &oldNid);
        }
        Shell_NotifyIconW(NIM_ADD, &nid);
        Shell_NotifyIconW(NIM_MODIFY, &nid);
    }

    ownerHwnd = targetOwner;
    notifActive = true;
    notifExpireTick = GetTickCount64() + 3500;
}

void ShowBalloonNotification(HWND& ownerHwnd, bool connected, bool silent,
                             bool& notifActive, ULONGLONG& notifExpireTick) {
    const wchar_t* msg = connected
        ? (IsRussianUi() ? L"VPN включен" : L"VPN connected")
        : (IsRussianUi() ? L"VPN выключен" : L"VPN disconnected");
    ShowBalloonMessage(ownerHwnd, msg, silent, notifActive, notifExpireTick);
}

void DismissBalloonNotification(HWND& ownerHwnd, bool& notifActive) {
    if (!notifActive) return;
    if (ownerHwnd) {
        NOTIFYICONDATAW nid{};
        nid.cbSize = sizeof(nid);
        nid.hWnd   = ownerHwnd;
        nid.uID    = kNotifIconId;
        Shell_NotifyIconW(NIM_DELETE, &nid);
    }
    ownerHwnd = nullptr;
    notifActive = false;
}

void ExecuteToggleOnWorkerThread(const IslandSettings& s,
                                 HWND& notifOwnerHwnd, bool& notifActive, ULONGLONG& notifExpireTick) {
    if (s.wgExePath.empty() || s.configPath.empty() || s.tunnelName.empty()) {
        Wh_Log(L"WireGuard configPath or wireguardExePath is empty; skipping toggle");
        if (s.showNotifications && !g_unloading.load()) {
            ShowBalloonMessage(notifOwnerHwnd,
                               IsRussianUi() ? L"Укажите путь к .conf файлу WireGuard в настройках мода"
                                             : L"Set your WireGuard .conf path in mod settings",
                               s.silentNotifications, notifActive, notifExpireTick);
        }
        g_vpnState.store(VpnState::Disconnected);
        g_needsUiUpdate.store(false);
        RefreshAllHostsUi(s);
        return;
    }

    const VpnState cur = QueryVpnServiceState(s.tunnelName);
    const bool wantStop = (cur == VpnState::Connected);
    const VpnState prevStable = wantStop ? VpnState::Connected : VpnState::Disconnected;

    g_lastStableState.store(prevStable);
    g_vpnState.store(VpnState::Transitioning);
    RefreshAllHostsUi(s);

    const wchar_t* taskName = wantStop ? kTaskStopName : kTaskStartName;
    const std::wstring expectedArg = wantStop
        ? (L"/uninstalltunnelservice " + s.tunnelName)
        : s.configPath;

    bool ranTask = false;
    if (ScheduledTaskMatchesConfig(taskName, s.wgExePath, expectedArg)) {
        DWORD exitCode = 1;
        const std::wstring runArgs = std::wstring(L"/run /tn \"") + taskName + L"\"";
        ranTask = RunProcessHidden(L"schtasks.exe", runArgs, exitCode) && (exitCode == 0);
    }

    if (!ranTask && !g_unloading.load()) {
        Wh_Log(L"Task %s missing, mismatched, or failed; falling back to UAC runas", taskName);
        const std::wstring wgParams = wantStop
            ? (L"/uninstalltunnelservice " + s.tunnelName)
            : (L"/installtunnelservice \"" + s.configPath + L"\"");

        SHELLEXECUTEINFOW sei{};
        sei.cbSize = sizeof(sei);
        sei.lpVerb = L"runas";
        sei.lpFile = s.wgExePath.c_str();
        sei.lpParameters = wgParams.c_str();
        sei.nShow = SW_HIDE;
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;

        if (ShellExecuteExW(&sei) && sei.hProcess) {
            if (g_stopEvent) {
                HANDLE waitHandles[] = {g_stopEvent, sei.hProcess};
                WaitForMultipleObjects(2, waitHandles, FALSE, 6000);
            } else {
                WaitForSingleObject(sei.hProcess, 6000);
            }
            CloseHandle(sei.hProcess);
        }
    }

    for (int i = 0; i < 25 && !g_unloading.load(); ++i) {
        if (g_stopEvent && WaitForSingleObject(g_stopEvent, 200) == WAIT_OBJECT_0) {
            return;
        }
        const VpnState st = QueryVpnServiceState(s.tunnelName);
        if ((wantStop && st == VpnState::Disconnected) ||
            (!wantStop && st == VpnState::Connected)) {
            break;
        }
    }

    const VpnState finalState = QueryVpnServiceState(s.tunnelName);
    if (finalState != VpnState::Transitioning) {
        g_lastStableState.store(finalState);
        if (finalState != prevStable && s.showNotifications && !g_unloading.load()) {
            ShowBalloonNotification(notifOwnerHwnd, finalState == VpnState::Connected,
                                    s.silentNotifications, notifActive, notifExpireTick);
        }
    }

    g_vpnState.store(finalState);
    g_needsUiUpdate.store(false);
    RefreshAllHostsUi(s);
}

void SyncMountedIslands(const IslandSettings& s, bool forceRebuild) {
    const std::vector<HWND> targetHwnds = FindTargetTaskbarWnds(s);
    const std::vector<HWND> mountedHwnds = GetMountedHwndsSnapshot();

    auto isTarget = [&](HWND h) {
        return std::find(targetHwnds.begin(), targetHwnds.end(), h) != targetHwnds.end();
    };
    auto isMounted = [&](HWND h) {
        return std::find(mountedHwnds.begin(), mountedHwnds.end(), h) != mountedHwnds.end();
    };

    if (forceRebuild) {
        for (HWND h : mountedHwnds) {
            if (IsWindow(h)) {
                RunFromWindowThread(h, [](void* raw) {
                    RemoveHostForHwndOnUiThread(reinterpret_cast<HWND>(raw));
                }, h);
            } else {
                RemoveMountedHwnd(h);
            }
        }
        for (HWND h : targetHwnds) {
            if (g_unloading.load()) return;
            RunFromWindowThread(h, [](void* raw) {
                InjectHostForHwndOnUiThread(reinterpret_cast<HWND>(raw));
            }, h);
        }
        return;
    }

    for (HWND h : mountedHwnds) {
        if (!IsWindow(h) || !isTarget(h)) {
            if (IsWindow(h)) {
                RunFromWindowThread(h, [](void* raw) {
                    RemoveHostForHwndOnUiThread(reinterpret_cast<HWND>(raw));
                }, h);
            } else {
                RemoveMountedHwnd(h);
            }
        }
    }

    for (HWND h : targetHwnds) {
        if (g_unloading.load()) return;
        if (!isMounted(h)) {
            RunFromWindowThread(h, [](void* raw) {
                InjectHostForHwndOnUiThread(reinterpret_cast<HWND>(raw));
            }, h);
        }
    }
}

DWORD WINAPI WorkerThreadProc(void*) {
    HWND      notifOwnerHwnd = nullptr;
    bool      notifActive = false;
    ULONGLONG notifExpireTick = 0;

    while (!g_unloading.load()) {
        const IslandSettings s = GetSettingsSnapshot();
        const std::vector<HWND> targetHwnds = FindTargetTaskbarWnds(s);
        const std::vector<HWND> mountedHwnds = GetMountedHwndsSnapshot();

        bool allTargetsMounted = !targetHwnds.empty();
        for (HWND h : targetHwnds) {
            if (std::find(mountedHwnds.begin(), mountedHwnds.end(), h) == mountedHwnds.end()) {
                allTargetsMounted = false;
                break;
            }
        }

        DWORD timeoutMs = allTargetsMounted ? 1500 : 500;
        if (notifActive) {
            const ULONGLONG now = GetTickCount64();
            if (now >= notifExpireTick) {
                DismissBalloonNotification(notifOwnerHwnd, notifActive);
            } else {
                const DWORD remaining = static_cast<DWORD>(notifExpireTick - now);
                timeoutMs = std::min(timeoutMs, std::max<DWORD>(50, remaining));
            }
        }

        HANDLE waitHandles[] = {g_stopEvent, g_wakeEvent};
        const DWORD wait = WaitForMultipleObjects(2, waitHandles, FALSE, timeoutMs);
        if (wait == WAIT_OBJECT_0 || g_unloading.load()) {
            break;
        }

        if (notifActive && GetTickCount64() >= notifExpireTick) {
            DismissBalloonNotification(notifOwnerHwnd, notifActive);
        }

        const IslandSettings latest = GetSettingsSnapshot();

        if (g_toggleRequested.exchange(false)) {
            ExecuteToggleOnWorkerThread(latest, notifOwnerHwnd, notifActive, notifExpireTick);
            if (g_unloading.load()) break;
        } else {
            const VpnState actual = QueryVpnServiceState(latest.tunnelName);
            if (actual != VpnState::Transitioning) {
                g_lastStableState.store(actual);
            }
            if (g_vpnState.exchange(actual) != actual) {
                g_needsUiUpdate.store(true);
            }
        }

        const bool rebuild = g_rebuildRequested.exchange(false);
        SyncMountedIslands(latest, rebuild);

        if (!rebuild && g_needsUiUpdate.exchange(false)) {
            RefreshAllHostsUi(latest);
        }
    }

    DismissBalloonNotification(notifOwnerHwnd, notifActive);
    return 0;
}

uint64_t HashBytesFnv1a(const std::vector<BYTE>& data) {
    if (data.empty()) return 0;
    uint64_t h = 14695981039346656037ULL;
    for (BYTE b : data) {
        h ^= b;
        h *= 1099511628211ULL;
    }
    return h == 0 ? 1 : h;
}

RepeatMode FromWinrtRepeatMode(winrt::Windows::Media::MediaPlaybackAutoRepeatMode m) {
    using M = winrt::Windows::Media::MediaPlaybackAutoRepeatMode;
    if (m == M::List) return RepeatMode::All;
    if (m == M::Track) return RepeatMode::One;
    return RepeatMode::Off;
}

winrt::Windows::Media::MediaPlaybackAutoRepeatMode ToWinrtRepeatMode(RepeatMode m) {
    using M = winrt::Windows::Media::MediaPlaybackAutoRepeatMode;
    if (m == RepeatMode::All) return M::List;
    if (m == RepeatMode::One) return M::Track;
    return M::None;
}

DWORD WINAPI VkBridgeThreadProc(void*) {
    WSADATA wsa{};
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        return 0;
    }

    SOCKET listenSock = INVALID_SOCKET;
    int boundPort = 0;

    auto closeListen = [&]() {
        if (listenSock != INVALID_SOCKET) {
            closesocket(listenSock);
            listenSock = INVALID_SOCKET;
            boundPort = 0;
        }
    };

    while (!g_unloading.load()) {
        if (g_stopEvent && WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
            break;
        }

        const IslandSettings s = GetSettingsSnapshot();
        if (!s.enableMediaPlayer || !s.enableVkBridge) {
            closeListen();
            g_vkLastSeenTick.store(0);
            g_vkV2SeenTick.store(0);
            if (g_stopEvent && WaitForSingleObject(g_stopEvent, 300) == WAIT_OBJECT_0) {
                break;
            }
            continue;
        }

        const int desiredPort = std::clamp(s.vkBridgePort, 1024, 65535);
        if (listenSock == INVALID_SOCKET || boundPort != desiredPort) {
            closeListen();
            listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (listenSock != INVALID_SOCKET) {
                u_long nb = 1;
                ioctlsocket(listenSock, FIONBIO, &nb);
                BOOL reuse = TRUE;
                setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR,
                           reinterpret_cast<const char*>(&reuse), sizeof(reuse));
                sockaddr_in addr{};
                addr.sin_family = AF_INET;
                addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                addr.sin_port = htons(static_cast<u_short>(desiredPort));
                if (bind(listenSock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == 0 &&
                    listen(listenSock, 8) == 0) {
                    boundPort = desiredPort;
                } else {
                    closeListen();
                }
            }
            if (listenSock == INVALID_SOCKET) {
                if (g_stopEvent && WaitForSingleObject(g_stopEvent, 1000) == WAIT_OBJECT_0) {
                    break;
                }
                continue;
            }
        }

        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(listenSock, &readSet);
        timeval tv{0, 100000};
        const int sel = select(0, &readSet, nullptr, nullptr, &tv);
        if (g_unloading.load()) break;
        if (sel <= 0) continue;

        sockaddr_in cliAddr{};
        int cliLen = sizeof(cliAddr);
        SOCKET cli = accept(listenSock, reinterpret_cast<sockaddr*>(&cliAddr), &cliLen);
        if (cli == INVALID_SOCKET) continue;

        DWORD rcvTimeout = 200;
        setsockopt(cli, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&rcvTimeout), sizeof(rcvTimeout));
        u_long blockMode = 0;
        ioctlsocket(cli, FIONBIO, &blockMode);

        char reqBuf[1024]{};
        const int received = recv(cli, reqBuf, sizeof(reqBuf) - 1, 0);
        const std::string_view req(reqBuf, received > 0 ? static_cast<size_t>(received) : 0);

        size_t lineEnd = req.find('\r');
        if (lineEnd == std::string_view::npos) lineEnd = req.find('\n');
        const std::string_view firstLine = (lineEnd != std::string_view::npos) ? req.substr(0, lineEnd) : req;

        const bool isOptions = (firstLine.rfind("OPTIONS ", 0) == 0);
        const bool isGet = (firstLine.rfind("GET ", 0) == 0);

        std::string body = "none";
        if (isGet) {
            auto getParam = [&](std::string_view key, int& outVal) -> bool {
                const std::string needle = std::string(key) + "=";
                const size_t q = firstLine.find('?');
                if (q == std::string_view::npos) return false;
                const size_t sp = firstLine.find(' ', q);
                const std::string_view query = (sp != std::string_view::npos)
                    ? firstLine.substr(q + 1, sp - q - 1)
                    : firstLine.substr(q + 1);
                size_t pos = 0;
                while (pos < query.size()) {
                    if (query.substr(pos).rfind(needle, 0) == 0) {
                        size_t valPos = pos + needle.size();
                        if (valPos < query.size() && query[valPos] >= '0' && query[valPos] <= '9') {
                            int val = 0;
                            while (valPos < query.size() && query[valPos] >= '0' && query[valPos] <= '9') {
                                val = val * 10 + (query[valPos] - '0');
                                ++valPos;
                            }
                            outVal = val;
                            return true;
                        }
                    }
                    const size_t amp = query.find('&', pos);
                    if (amp == std::string_view::npos) break;
                    pos = amp + 1;
                }
                return false;
            };

            const ULONGLONG now = GetTickCount64();
            int reqVer = 0;
            getParam("v", reqVer);
            if (reqVer >= 2) {
                g_vkV2SeenTick.store(now);
            }
            const ULONGLONG v2Seen = g_vkV2SeenTick.load();
            const bool ignoreLegacy = (reqVer < 2) && (v2Seen != 0) && ((now - v2Seen) < 3000);

            if (!ignoreLegacy) {
                int hasAudio = 0, shuf = 0, rep = 0, liked = 0;
                const bool parsedHas = getParam("has", hasAudio);
                if (!parsedHas || hasAudio != 0) {
                    const ULONGLONG prevSeen = g_vkLastSeenTick.exchange(now);
                    if (prevSeen == 0 && g_mediaWakeEvent) {
                        SetEvent(g_mediaWakeEvent);
                    }

                    const bool hasPendingCmd = (g_vkPendingCmd.load() != 0);
                    const ULONGLONG lastCmd = g_vkLastCmdTick.load();
                    const bool settleWindow = hasPendingCmd || (lastCmd != 0 && (now - lastCmd) < 550);

                    if (getParam("s", shuf) && !settleWindow) {
                        const bool b = (shuf != 0);
                        if (g_vkShuffle.exchange(b) != b && g_mediaWakeEvent) SetEvent(g_mediaWakeEvent);
                    }
                    if (getParam("r", rep) && !settleWindow) {
                        const int r = std::clamp(rep, 0, 2);
                        if (g_vkRepeat.exchange(r) != r && g_mediaWakeEvent) SetEvent(g_mediaWakeEvent);
                    }
                    if (getParam("l", liked)) {
                        g_vkBridgeExtended.store(true);
                        if (!settleWindow) {
                            const bool b = (liked != 0);
                            if (g_vkLiked.exchange(b) != b && g_mediaWakeEvent) SetEvent(g_mediaWakeEvent);
                        }
                    }
                } else {
                    if (g_vkLastSeenTick.exchange(0) != 0 && g_mediaWakeEvent) {
                        SetEvent(g_mediaWakeEvent);
                    }
                }

                const int cmd = g_vkPendingCmd.exchange(0);
                if (cmd != 0) {
                    g_vkLastCmdTick.store(GetTickCount64());
                }
                const bool ext = g_vkBridgeExtended.load();
                switch (cmd) {
                    case 1: body = "shuffle"; break;
                    case 2: body = "repeat"; break;
                    case 3: body = "add"; break;
                    case 4: body = ext ? "repeat:0" : "repeat"; break;
                    case 5: body = ext ? "repeat:1" : "repeat"; break;
                    case 6: body = ext ? "repeat:2" : "repeat"; break;
                    default: body = "none"; break;
                }
            }
        }

        char hdr[512]{};
        if (isOptions) {
            const int hlen = std::snprintf(hdr, sizeof(hdr),
                "HTTP/1.1 204 No Content\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Private-Network: true\r\n"
                "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
                "Access-Control-Max-Age: 86400\r\n"
                "Connection: close\r\n\r\n");
            if (hlen > 0) send(cli, hdr, hlen, 0);
        } else {
            const int hlen = std::snprintf(hdr, sizeof(hdr),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain; charset=utf-8\r\n"
                "Content-Length: %zu\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Private-Network: true\r\n"
                "Cache-Control: no-store\r\n"
                "Connection: close\r\n\r\n",
                body.size());
            if (hlen > 0) send(cli, hdr, hlen, 0);
            if (!body.empty()) send(cli, body.data(), static_cast<int>(body.size()), 0);
        }
        closesocket(cli);
    }

    closeListen();
    WSACleanup();
    return 0;
}

winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession SelectActiveSession(
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager const& mgr,
    const std::wstring& preferredAumid,
    const std::wstring& lastAumid,
    std::vector<SessionInfo>& outSessions) {
    using Session = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession;
    using PlaybackStatus = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus;

    outSessions.clear();
    if (!mgr) return nullptr;

    Session preferredMatch{nullptr};
    Session playingLastMatch{nullptr};
    Session firstPlaying{nullptr};
    Session idleLastMatch{nullptr};
    Session firstAny{nullptr};

    try {
        auto sessions = mgr.GetSessions();
        const uint32_t count = sessions.Size();
        for (uint32_t i = 0; i < count; ++i) {
            auto s = sessions.GetAt(i);
            if (!s) continue;
            if (!firstAny) firstAny = s;

            std::wstring aumid;
            try { aumid = std::wstring(s.SourceAppUserModelId()); } catch (...) {}

            bool isPlaying = false;
            try {
                auto info = s.GetPlaybackInfo();
                isPlaying = info && info.PlaybackStatus() == PlaybackStatus::Playing;
            } catch (...) {}

            if (!aumid.empty()) {
                bool dup = false;
                for (auto& existing : outSessions) {
                    if (existing.aumid == aumid) {
                        existing.isPlaying = existing.isPlaying || isPlaying;
                        dup = true;
                        break;
                    }
                }
                if (!dup) {
                    outSessions.push_back(SessionInfo{aumid, ResolveAppDisplayName(aumid), isPlaying});
                }
            }

            if (!preferredAumid.empty() && aumid == preferredAumid && !preferredMatch) {
                preferredMatch = s;
            }
            if (isPlaying) {
                if (!lastAumid.empty() && aumid == lastAumid && !playingLastMatch) {
                    playingLastMatch = s;
                }
                if (!firstPlaying) {
                    firstPlaying = s;
                }
            } else if (!lastAumid.empty() && aumid == lastAumid && !idleLastMatch) {
                idleLastMatch = s;
            }
        }
    } catch (...) {}

    if (preferredMatch) return preferredMatch;
    if (playingLastMatch) return playingLastMatch;
    if (firstPlaying) return firstPlaying;
    if (idleLastMatch) return idleLastMatch;

    try {
        if (auto cur = mgr.GetCurrentSession()) {
            return cur;
        }
    } catch (...) {}

    return firstAny;
}

DWORD WINAPI MediaWorkerThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    {
        using SessionManager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager;
        using Session = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession;
        using PlaybackStatus = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus;

        SessionManager sessionMgr{nullptr};
        std::wstring lastAumid;
        std::wstring cachedTrackKey;
        std::vector<BYTE> cachedThumbBytes;
        uint64_t cachedThumbHash = 0;
        uint32_t pollCounter = 0;
        bool firstIter = true;

        while (!g_unloading.load()) {
            const IslandSettings s = GetSettingsSnapshot();
            const DWORD activePollMs = g_miniPlayerOpen.load() ? 350 : 500;
            const DWORD timeoutMs = firstIter ? 0 : (s.enableMediaPlayer ? activePollMs : 1000);
            firstIter = false;

            HANDLE waitHandles[] = {g_stopEvent, g_mediaWakeEvent};
            const DWORD wait = WaitForMultipleObjects(2, waitHandles, FALSE, timeoutMs);
            if (wait == WAIT_OBJECT_0 || g_unloading.load()) {
                break;
            }

            const IslandSettings latest = GetSettingsSnapshot();
            if (!latest.enableMediaPlayer) {
                g_pendingMediaCmd.store(0);
                bool hadMedia = false;
                {
                    std::lock_guard<std::mutex> lock(g_mediaMutex);
                    hadMedia = g_mediaSnapshot.hasMedia;
                    if (hadMedia) {
                        g_mediaSnapshot = {};
                    }
                    g_preferredSessionAumid.clear();
                    g_pendingTargetSessionAumid.clear();
                }
                lastAumid.clear();
                cachedTrackKey.clear();
                cachedThumbBytes.clear();
                cachedThumbHash = 0;
                if (hadMedia) {
                    RefreshAllHostsUi(latest);
                }
                continue;
            }

            if (!sessionMgr) {
                try {
                    sessionMgr = SessionManager::RequestAsync().get();
                } catch (...) {
                    sessionMgr = nullptr;
                }
                if (g_unloading.load()) break;
            }

            std::wstring preferredAumid;
            {
                std::lock_guard<std::mutex> lock(g_mediaMutex);
                preferredAumid = g_preferredSessionAumid;
            }

            std::vector<SessionInfo> sessions;
            Session session = SelectActiveSession(sessionMgr, preferredAumid, lastAumid, sessions);

            if (!preferredAumid.empty()) {
                bool stillExists = false;
                for (const auto& info : sessions) {
                    if (info.aumid == preferredAumid) {
                        stillExists = true;
                        break;
                    }
                }
                if (!stillExists) {
                    preferredAumid.clear();
                    std::lock_guard<std::mutex> lock(g_mediaMutex);
                    g_preferredSessionAumid.clear();
                }
            }

            const int cmdVal = g_pendingMediaCmd.exchange(0);
            if (cmdVal != 0) {
                const auto cmd = static_cast<MediaCommand>(cmdVal);
                if (cmd == MediaCommand::SwitchToSession) {
                    std::wstring target;
                    {
                        std::lock_guard<std::mutex> lock(g_mediaMutex);
                        target = std::move(g_pendingTargetSessionAumid);
                        g_pendingTargetSessionAumid.clear();
                        g_preferredSessionAumid = target;
                    }
                    preferredAumid = target;
                    session = SelectActiveSession(sessionMgr, preferredAumid, lastAumid, sessions);
                } else if (cmd == MediaCommand::SwitchNextSession && sessions.size() > 1) {
                    std::wstring curAumid;
                    if (session) {
                        try { curAumid = std::wstring(session.SourceAppUserModelId()); } catch (...) {}
                    }
                    size_t curIdx = 0;
                    for (size_t i = 0; i < sessions.size(); ++i) {
                        if (sessions[i].aumid == curAumid) {
                            curIdx = i;
                            break;
                        }
                    }
                    const std::wstring nextAumid = sessions[(curIdx + 1) % sessions.size()].aumid;
                    {
                        std::lock_guard<std::mutex> lock(g_mediaMutex);
                        g_preferredSessionAumid = nextAumid;
                    }
                    preferredAumid = nextAumid;
                    session = SelectActiveSession(sessionMgr, preferredAumid, lastAumid, sessions);
                } else if (session) {
                    std::wstring curAumid;
                    try { curAumid = std::wstring(session.SourceAppUserModelId()); } catch (...) {}
                    const std::wstring curAppName = ResolveAppDisplayName(curAumid);
                    const ULONGLONG vkSeen = g_vkLastSeenTick.load();
                    const bool useVkBridge = latest.enableVkBridge &&
                                             (vkSeen != 0) &&
                                             ((GetTickCount64() - vkSeen) < 5000) &&
                                             IsBrowserSession(curAumid, curAppName);
                    try {
                        if (cmd == MediaCommand::Prev) {
                            session.TrySkipPreviousAsync().get();
                        } else if (cmd == MediaCommand::PlayPause) {
                            auto info = session.GetPlaybackInfo();
                            const bool playing = info && info.PlaybackStatus() == PlaybackStatus::Playing;
                            if (playing) {
                                session.TryPauseAsync().get();
                            } else {
                                session.TryPlayAsync().get();
                            }
                        } else if (cmd == MediaCommand::Next) {
                            session.TrySkipNextAsync().get();
                        } else if (cmd == MediaCommand::Stop) {
                            session.TryStopAsync().get();
                        } else if (cmd == MediaCommand::Seek || cmd == MediaCommand::Rewind5 || cmd == MediaCommand::Forward5) {
                            if (auto tl = session.GetTimelineProperties()) {
                                const int64_t startT = tl.StartTime().count();
                                const int64_t endT = tl.EndTime().count();
                                int64_t targetAbs = startT;
                                if (cmd == MediaCommand::Seek) {
                                    int64_t rel = g_pendingSeekTicks.load();
                                    if (endT > startT) rel = std::clamp<int64_t>(rel, 0, endT - startT);
                                    targetAbs = startT + std::max<int64_t>(0, rel);
                                } else {
                                    int64_t curPos = tl.Position().count();
                                    auto info = session.GetPlaybackInfo();
                                    const bool playing = info && info.PlaybackStatus() == PlaybackStatus::Playing;
                                    if (playing) {
                                        FILETIME ftNow{};
                                        GetSystemTimeAsFileTime(&ftNow);
                                        ULARGE_INTEGER uiNow{};
                                        uiNow.LowPart = ftNow.dwLowDateTime;
                                        uiNow.HighPart = ftNow.dwHighDateTime;
                                        const int64_t nowFt = static_cast<int64_t>(uiNow.QuadPart);
                                        const int64_t updFt = tl.LastUpdatedTime().time_since_epoch().count();
                                        if (updFt > 0 && nowFt >= updFt && (nowFt - updFt) < 36000000000LL * 4) {
                                            curPos += (nowFt - updFt);
                                        }
                                    }
                                    const int64_t delta = (cmd == MediaCommand::Forward5) ? 50000000LL : -50000000LL;
                                    targetAbs = curPos + delta;
                                    if (endT > startT) {
                                        targetAbs = std::clamp(targetAbs, startT, endT);
                                    } else {
                                        targetAbs = std::max(targetAbs, startT);
                                    }
                                }
                                session.TryChangePlaybackPositionAsync(targetAbs).get();
                            }
                        } else if (cmd == MediaCommand::ToggleShuffle) {
                            if (useVkBridge) {
                                g_vkLastCmdTick.store(GetTickCount64());
                                g_vkShuffle.store(!g_vkShuffle.load());
                                g_vkPendingCmd.store(1);
                            } else {
                                bool cur = false;
                                if (auto info = session.GetPlaybackInfo()) {
                                    if (auto sh = info.IsShuffleActive()) cur = sh.Value();
                                }
                                session.TryChangeShuffleActiveAsync(!cur).get();
                            }
                        } else if (cmd == MediaCommand::CycleRepeat ||
                                   cmd == MediaCommand::SetRepeatOff ||
                                   cmd == MediaCommand::SetRepeatAll ||
                                   cmd == MediaCommand::SetRepeatOne) {
                            if (useVkBridge) {
                                g_vkLastCmdTick.store(GetTickCount64());
                                if (cmd == MediaCommand::CycleRepeat) {
                                    g_vkRepeat.store((g_vkRepeat.load() + 1) % 3);
                                    g_vkPendingCmd.store(2);
                                } else if (cmd == MediaCommand::SetRepeatOff) {
                                    g_vkRepeat.store(0);
                                    g_vkPendingCmd.store(4);
                                } else if (cmd == MediaCommand::SetRepeatAll) {
                                    g_vkRepeat.store(1);
                                    g_vkPendingCmd.store(5);
                                } else {
                                    g_vkRepeat.store(2);
                                    g_vkPendingCmd.store(6);
                                }
                            } else {
                                RepeatMode nextMode = RepeatMode::Off;
                                if (cmd == MediaCommand::CycleRepeat) {
                                    RepeatMode cur = RepeatMode::Off;
                                    if (auto info = session.GetPlaybackInfo()) {
                                        if (auto rm = info.AutoRepeatMode()) {
                                            cur = FromWinrtRepeatMode(rm.Value());
                                        }
                                    }
                                    nextMode = static_cast<RepeatMode>((static_cast<int>(cur) + 1) % 3);
                                } else if (cmd == MediaCommand::SetRepeatAll) {
                                    nextMode = RepeatMode::All;
                                } else if (cmd == MediaCommand::SetRepeatOne) {
                                    nextMode = RepeatMode::One;
                                }
                                session.TryChangeAutoRepeatModeAsync(ToWinrtRepeatMode(nextMode)).get();
                            }
                        } else if (cmd == MediaCommand::ToggleLike) {
                            if (useVkBridge) {
                                g_vkLastCmdTick.store(GetTickCount64());
                                g_vkLiked.store(!g_vkLiked.load());
                                g_vkPendingCmd.store(3);
                            }
                        }
                    } catch (...) {}
                }

                if (g_stopEvent && WaitForSingleObject(g_stopEvent, 90) == WAIT_OBJECT_0) {
                    break;
                }
                session = SelectActiveSession(sessionMgr, preferredAumid, lastAumid, sessions);
            }

            MediaSnapshot nextSnap;
            nextSnap.sessions = std::move(sessions);

            if (session) {
                std::wstring aumid;
                try { aumid = std::wstring(session.SourceAppUserModelId()); } catch (...) {}
                const std::wstring appName = ResolveAppDisplayName(aumid);
                if (!aumid.empty()) {
                    lastAumid = aumid;
                    nextSnap.activeAumid = aumid;
                    nextSnap.activeAppName = appName;
                }

                const ULONGLONG vkSeen = g_vkLastSeenTick.load();
                const bool vkConnected = latest.enableVkBridge &&
                                         (vkSeen != 0) &&
                                         ((GetTickCount64() - vkSeen) < 5000) &&
                                         IsBrowserSession(aumid, appName);
                nextSnap.vkBridgeConnected = vkConnected;

                try {
                    auto info = session.GetPlaybackInfo();
                    if (info) {
                        nextSnap.isPlaying = (info.PlaybackStatus() == PlaybackStatus::Playing);
                        if (vkConnected) {
                            nextSnap.shuffleEnabled = g_vkShuffle.load();
                            nextSnap.repeatMode = static_cast<RepeatMode>(std::clamp(g_vkRepeat.load(), 0, 2));
                            nextSnap.inLibrary = g_vkLiked.load();
                        } else {
                            if (auto sh = info.IsShuffleActive()) {
                                nextSnap.shuffleEnabled = sh.Value();
                            }
                            if (auto rm = info.AutoRepeatMode()) {
                                nextSnap.repeatMode = FromWinrtRepeatMode(rm.Value());
                            }
                        }
                    }
                } catch (...) {}

                try {
                    if (auto tl = session.GetTimelineProperties()) {
                        const int64_t startT = tl.StartTime().count();
                        const int64_t endT = tl.EndTime().count();
                        int64_t posT = tl.Position().count();
                        if (endT > startT) {
                            if (nextSnap.isPlaying) {
                                FILETIME ftNow{};
                                GetSystemTimeAsFileTime(&ftNow);
                                ULARGE_INTEGER uiNow{};
                                uiNow.LowPart = ftNow.dwLowDateTime;
                                uiNow.HighPart = ftNow.dwHighDateTime;
                                const int64_t nowFt = static_cast<int64_t>(uiNow.QuadPart);
                                const int64_t updFt = tl.LastUpdatedTime().time_since_epoch().count();
                                if (updFt > 0 && nowFt >= updFt && (nowFt - updFt) < 36000000000LL * 4) {
                                    posT += (nowFt - updFt);
                                }
                            }
                            nextSnap.durationTicks = endT - startT;
                            nextSnap.positionTicks = std::clamp<int64_t>(posT - startT, 0, nextSnap.durationTicks);
                        }
                    }
                } catch (...) {}

                try {
                    auto props = session.TryGetMediaPropertiesAsync().get();
                    if (g_unloading.load()) break;
                    if (props) {
                        nextSnap.title = std::wstring(props.Title());
                        nextSnap.artist = std::wstring(props.Artist());
                        nextSnap.hasMedia = (!nextSnap.title.empty() || !nextSnap.artist.empty());

                        const std::wstring trackKey = aumid + L"\x1f" + nextSnap.title + L"\x1f" + nextSnap.artist;
                        const bool trackChanged = (trackKey != cachedTrackKey);
                        if (trackChanged) {
                            cachedTrackKey = trackKey;
                            cachedThumbBytes.clear();
                            cachedThumbHash = 0;
                        }

                        if (nextSnap.hasMedia) {
                            const bool shouldFetchThumb =
                                trackChanged || cachedThumbBytes.empty() || (pollCounter % 4 == 0);
                            if (shouldFetchThumb) {
                                std::vector<BYTE> fetchedBytes;
                                uint64_t fetchedHash = 0;
                                if (auto thumbRef = props.Thumbnail()) {
                                    try {
                                        auto stream = thumbRef.OpenReadAsync().get();
                                        if (stream) {
                                            const UINT64 sz = stream.Size();
                                            if (sz > 0 && sz < 4 * 1024 * 1024) {
                                                winrt::Windows::Storage::Streams::DataReader reader(stream);
                                                reader.LoadAsync(static_cast<UINT32>(sz)).get();
                                                fetchedBytes.resize(static_cast<size_t>(sz));
                                                reader.ReadBytes(winrt::array_view<BYTE>(fetchedBytes));
                                                reader.DetachStream();
                                                fetchedHash = HashBytesFnv1a(fetchedBytes);
                                            }
                                        }
                                    } catch (...) {
                                        fetchedBytes.clear();
                                        fetchedHash = 0;
                                    }
                                }
                                cachedThumbBytes = std::move(fetchedBytes);
                                cachedThumbHash = fetchedHash;
                            }
                            nextSnap.thumbnailBytes = cachedThumbBytes;
                            nextSnap.thumbnailHash = cachedThumbHash;
                        }
                    }
                } catch (...) {}
            } else {
                lastAumid.clear();
                cachedTrackKey.clear();
                cachedThumbBytes.clear();
                cachedThumbHash = 0;
            }

            ++pollCounter;

            bool changed = false;
            {
                std::lock_guard<std::mutex> lock(g_mediaMutex);
                auto sessionsEqual = [](const std::vector<SessionInfo>& a, const std::vector<SessionInfo>& b) {
                    if (a.size() != b.size()) return false;
                    for (size_t i = 0; i < a.size(); ++i) {
                        if (a[i].aumid != b[i].aumid ||
                            a[i].label != b[i].label ||
                            a[i].isPlaying != b[i].isPlaying) return false;
                    }
                    return true;
                };

                const bool coreChanged =
                    (nextSnap.hasMedia != g_mediaSnapshot.hasMedia) ||
                    (nextSnap.isPlaying != g_mediaSnapshot.isPlaying) ||
                    (nextSnap.title != g_mediaSnapshot.title) ||
                    (nextSnap.artist != g_mediaSnapshot.artist) ||
                    (nextSnap.activeAumid != g_mediaSnapshot.activeAumid) ||
                    (nextSnap.shuffleEnabled != g_mediaSnapshot.shuffleEnabled) ||
                    (nextSnap.repeatMode != g_mediaSnapshot.repeatMode) ||
                    (nextSnap.inLibrary != g_mediaSnapshot.inLibrary) ||
                    (nextSnap.vkBridgeConnected != g_mediaSnapshot.vkBridgeConnected) ||
                    (nextSnap.thumbnailHash != g_mediaSnapshot.thumbnailHash) ||
                    !sessionsEqual(nextSnap.sessions, g_mediaSnapshot.sessions);

                const bool timeChanged = g_miniPlayerOpen.load() &&
                    ((nextSnap.positionTicks / 5000000LL) != (g_mediaSnapshot.positionTicks / 5000000LL) ||
                     nextSnap.durationTicks != g_mediaSnapshot.durationTicks);

                changed = coreChanged || timeChanged;
                if (changed || nextSnap.positionTicks != g_mediaSnapshot.positionTicks) {
                    g_mediaSnapshot = std::move(nextSnap);
                }
            }

            if (changed && !g_unloading.load()) {
                RefreshAllHostsUi(latest);
            }
        }
    }
    winrt::uninit_apartment();
    return 0;
}

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (g_unloading.load()) return;

    RemoveHostsOnCurrentThread();
    g_rebuildRequested.store(true);
    g_needsUiUpdate.store(true);
    if (g_wakeEvent) SetEvent(g_wakeEvent);
}

bool HookTaskbarDllSymbols() {
    HMODULE hTaskbar = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hTaskbar) return false;

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"}, &CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"}, &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"}, &CSecondaryTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &Std_Ref_Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"}, &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook},
    };

    return WindhawkUtils::HookSymbols(hTaskbar, hooks, ARRAYSIZE(hooks));
}

}  // namespace

BOOL Wh_ModInit() {
    g_unloading.store(false);
    g_toggleRequested.store(false);
    g_rebuildRequested.store(false);
    g_needsUiUpdate.store(false);
    g_pendingMediaCmd.store(0);
    g_pendingSeekTicks.store(0);
    g_miniPlayerOpen.store(false);
    g_miniPlayerAnchorHwnd = nullptr;
    g_vkPendingCmd.store(0);
    g_vkShuffle.store(false);
    g_vkRepeat.store(0);
    g_vkLiked.store(false);
    g_vkLastSeenTick.store(0);
    g_vkV2SeenTick.store(0);
    g_vkLastCmdTick.store(0);
    g_vkBridgeExtended.store(false);

    InitCurrentProcessInfo();
    ReloadSettings();

    if (g_isExplorer) {
        if (!HookTaskbarDllSymbols()) {
            Wh_Log(L"Failed to hook taskbar.dll symbols");
            return FALSE;
        }
        return TRUE;
    }

    if (g_splitBypassActive.load()) {
        return HookWinsockForSplitTunneling() ? TRUE : FALSE;
    }

    return FALSE;
}

void Wh_ModAfterInit() {
    if (!g_isExplorer) return;

    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        g_mediaSnapshot = {};
        g_preferredSessionAumid.clear();
        g_pendingTargetSessionAumid.clear();
        g_appNameCache.clear();
    }

    const IslandSettings s = GetSettingsSnapshot();
    const VpnState st = QueryVpnServiceState(s.tunnelName);
    g_vpnState.store(st);
    g_lastStableState.store(st == VpnState::Connected ? VpnState::Connected : VpnState::Disconnected);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_mediaWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    g_mediaThread = CreateThread(nullptr, 0, MediaWorkerThreadProc, nullptr, 0, nullptr);
    g_vkBridgeThread = CreateThread(nullptr, 0, VkBridgeThreadProc, nullptr, 0, nullptr);

    g_rebuildRequested.store(true);
    if (g_wakeEvent) SetEvent(g_wakeEvent);
    if (g_mediaWakeEvent) SetEvent(g_mediaWakeEvent);
}

void Wh_ModSettingsChanged() {
    ReloadSettings();

    if (!g_isExplorer) {
        return;
    }

    const IslandSettings s = GetSettingsSnapshot();
    const VpnState st = QueryVpnServiceState(s.tunnelName);
    g_vpnState.store(st);
    g_lastStableState.store(st == VpnState::Connected ? VpnState::Connected : VpnState::Disconnected);

    g_rebuildRequested.store(true);
    g_needsUiUpdate.store(true);
    if (g_wakeEvent) SetEvent(g_wakeEvent);
    if (g_mediaWakeEvent) SetEvent(g_mediaWakeEvent);
}

void Wh_ModUninit() {
    g_unloading.store(true);
    g_splitBypassActive.store(false);

    if (!g_isExplorer) return;

    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_mediaThread) {
        WaitForSingleObject(g_mediaThread, 5000);
        CloseHandle(g_mediaThread);
        g_mediaThread = nullptr;
    }
    if (g_vkBridgeThread) {
        WaitForSingleObject(g_vkBridgeThread, 5000);
        CloseHandle(g_vkBridgeThread);
        g_vkBridgeThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
    if (g_mediaWakeEvent) {
        CloseHandle(g_mediaWakeEvent);
        g_mediaWakeEvent = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(g_mediaMutex);
        g_mediaSnapshot = {};
        g_preferredSessionAumid.clear();
        g_pendingTargetSessionAumid.clear();
        g_appNameCache.clear();
    }

    const auto hwnds = GetMountedHwndsSnapshot();
    bool hasDeadHwnd = false;
    for (HWND hWnd : hwnds) {
        if (IsWindow(hWnd)) {
            RunFromWindowThread(hWnd, [](void* raw) {
                RemoveHostForHwndOnUiThread(reinterpret_cast<HWND>(raw));
            }, hWnd);
        } else {
            hasDeadHwnd = true;
            RemoveMountedHwnd(hWnd);
        }
    }

    if (hasDeadHwnd) {
        if (HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr); hTray && IsWindow(hTray)) {
            RunFromWindowThread(hTray, [](void*) {
                PruneInvalidHostsOnUiThread();
            }, nullptr);
        }
    }
}
